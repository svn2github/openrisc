/*++

FTC MPSSE Interface DLLs - Copyright © FTDI Ltd. 2009


The source code to the FTCI2C, FTCJTAG and FTCSPI DLLs is provided as-is and no warranty is made as to its function or reliability.  

This source code may be freely modified and redistributed, provided that the FTDI Ltd. copyright notice remains intact.

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    FT2232cMpsseJtag.cpp

Abstract:

    FT2232C Dual Device Device Class Implementation.

Environment:

    kernel & user mode

Revision History:

    07/02/05    kra     Created.
    24/08/05    kra     Added new function JTAG_GenerateClockPulses and new error code FTC_INVALID_NUMBER_CLOCK_PULSES
	
--*/

#define WIO_DEFINED

#include <unistd.h>
#include <stdio.h>
//#include <linux/unistd.h> // -- Possibly not needed, trying without -jb
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <string.h>
#include <time.h>

// included <sched.h> for sched_yield() calls, 
// used in place of Windows' Sleep(0) calls, 
// however may not be good idea, not sure it's
// exactly the same thing!! -- Julius
#include <sched.h>


#include "FT2232cMpsseJtag.h"


FTC_STATUS FT2232cMpsseJtag::CheckWriteDataToExternalDeviceBitsBytesParameters(DWORD dwNumBitsToWrite, DWORD dwNumBytesToWrite)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesForNumBits = 0;

  if (((dwNumBitsToWrite >= MIN_NUM_BITS) && (dwNumBitsToWrite <= MAX_NUM_BITS)) &&
      ((dwNumBytesToWrite >= MIN_NUM_BYTES) && (dwNumBytesToWrite <= MAX_NUM_BYTES)))
  {
    dwNumBytesForNumBits = (dwNumBitsToWrite / NUMBITSINBYTE);

    if ((dwNumBitsToWrite % NUMBITSINBYTE) > 0)
      dwNumBytesForNumBits = dwNumBytesForNumBits + 1;

    if (dwNumBytesForNumBits > dwNumBytesToWrite)
      Status = FTC_NUMBER_BYTES_TOO_SMALL;
  }
  else
  {
    if ((dwNumBitsToWrite < MIN_NUM_BITS) || (dwNumBitsToWrite > MAX_NUM_BITS))
      Status = FTC_INVALID_NUMBER_BITS;
    else
      Status = FTC_INVALID_NUMBER_BYTES;
  }

  return Status;
}

void FT2232cMpsseJtag::AddByteToOutputBuffer(DWORD dwOutputByte, BOOL bClearOutputBuffer)
{
  DWORD dwNumBytesToSend_temp = 0;

  if (iCommandsSequenceDataDeviceIndex == -1)
    FTC_AddByteToOutputBuffer(dwOutputByte, bClearOutputBuffer);
  else
  {
    // This is used when you are building up a sequence of commands ie write, read and write/read
    dwNumBytesToSend_temp = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend;

    (*OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pCommandsSequenceDataOutPutBuffer)[dwNumBytesToSend_temp] = (dwOutputByte & '\xFF');

    dwNumBytesToSend_temp = dwNumBytesToSend_temp + 1;

    OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend = dwNumBytesToSend_temp;
  }
}

FTC_STATUS FT2232cMpsseJtag::SetDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;
  
  AddByteToOutputBuffer(SET_CLOCK_FREQUENCY_CMD, true);
  AddByteToOutputBuffer(dwClockDivisor, false);
  AddByteToOutputBuffer((dwClockDivisor >> 8), false);

  Status = FTC_SendBytesToDevice(ftHandle);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::InitDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  // set general purpose I/O low pins 1-4 all to input except TDO
  AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, true);

  dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xF0');
  dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x08'); // TDI,TCK start low
  AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\xF0');
  dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x0B');
  AddByteToOutputBuffer(dwSavedLowPinsDirection, false);

  // set general purpose I/O high pins 1-4 all to input
  AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, false);
  AddByteToOutputBuffer(0, false);
  AddByteToOutputBuffer(0, false);

  Status = FTC_SendBytesToDevice(ftHandle);
  
  if (Status == FTC_SUCCESS)
    SetDataInOutClockFrequency(ftHandle, dwClockDivisor);

  return Status;
}

// This procedure sets the JTAG to a new state
void FT2232cMpsseJtag::SetJTAGToNewState(DWORD dwNewJtagState, DWORD dwNumTmsClocks, BOOL bDoReadOperation)
{
  if ((dwNumTmsClocks >= 1) && (dwNumTmsClocks <= 7))
  {
    if (bDoReadOperation == TRUE)
      AddByteToOutputBuffer(CLK_DATA_TMS_READ_CMD, false);
    else
      AddByteToOutputBuffer(CLK_DATA_TMS_NO_READ_CMD, false);

    AddByteToOutputBuffer(((dwNumTmsClocks - 1) & '\xFF'), false);
    AddByteToOutputBuffer((dwNewJtagState & '\xFF'), false);
  }
}

// This function returns the number of TMS clocks to work out the last bit of TDO
DWORD FT2232cMpsseJtag::MoveJTAGFromOneStateToAnother(JtagStates NewJtagState, DWORD dwLastDataBit, BOOL bDoReadOperation)
{
  DWORD dwNumTmsClocks = 0;

  if (CurrentJtagState == Undefined)
  {
    SetJTAGToNewState('\x7F', 7, false);
    CurrentJtagState = TestLogicReset;
  }

  switch (CurrentJtagState)
  {
    case TestLogicReset:
      dwNumTmsClocks = TestLogicResetToNewJTAGStateNumTMSClocks[NewJtagState];
      SetJTAGToNewState((TestLogicResetToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;
    case RunTestIdle:
      dwNumTmsClocks = RunTestIdleToNewJTAGStateNumTMSClocks[NewJtagState];
      SetJTAGToNewState((RunTestIdleToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;
    case PauseDataRegister:
      dwNumTmsClocks = PauseDataRegToNewJTAGStateNumTMSClocks[NewJtagState];
      SetJTAGToNewState((PauseDataRegToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;
    case PauseInstructionRegister:
      dwNumTmsClocks = PauseInstructionRegToNewJTAGStateNumTMSClocks[NewJtagState];
      SetJTAGToNewState((PauseInstructionRegToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;
    case ShiftDataRegister:
      dwNumTmsClocks = ShiftDataRegToNewJTAGStateNumTMSClocks[NewJtagState];
      SetJTAGToNewState((ShiftDataRegToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;
    case ShiftInstructionRegister:
      dwNumTmsClocks = ShiftInstructionRegToNewJTAGStateNumTMSClocks[NewJtagState];
      SetJTAGToNewState((ShiftInstructionRegToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;
  default :
    // Added default state -- Julius
    dwNumTmsClocks = TestLogicResetToNewJTAGStateNumTMSClocks[NewJtagState];
    SetJTAGToNewState((TestLogicResetToNewJTAGState[NewJtagState] | (dwLastDataBit << 7)), dwNumTmsClocks, bDoReadOperation);
    break;    
  }

  CurrentJtagState = NewJtagState;

  return dwNumTmsClocks;
}

FTC_STATUS FT2232cMpsseJtag::ResetTAPContollerExternalDeviceSetToTestIdleMode(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  // set I/O low bits all out except TDO
  AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, true);

  dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFF');
  // TDI,TCK start low
  dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x08');
  AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\xFF');
  dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x0B');
  AddByteToOutputBuffer(dwSavedLowPinsDirection, false);

  //MoveJTAGFromOneStateToAnother(TestLogicReset, 1, false);JtagStates
  MoveJTAGFromOneStateToAnother(Undefined, 1, false);

  MoveJTAGFromOneStateToAnother(RunTestIdle, NO_LAST_DATA_BIT, FALSE);

  Status = FTC_SendBytesToDevice(ftHandle);
  
  return Status;
}

FTC_STATUS FT2232cMpsseJtag::SetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                                              DWORD dwLowPinsDirection, DWORD dwLowPinsValue,
                                                              BOOL bControlHighInputOutputPins,
                                                              DWORD dwHighPinsDirection, DWORD dwHighPinsValue)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if (bControlLowInputOutputPins != false)
  {
    // output on the general purpose I/O low pins 1-4
    AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, true);

    // shift left by 4 bits ie move general purpose I/O low pins 1-4 from bits 0-3 to bits 4-7
    dwLowPinsValue = ((dwLowPinsValue & '\x0F') << 4);

    dwSavedLowPinsValue = (dwSavedLowPinsValue & '\x0F');
    dwSavedLowPinsValue = (dwSavedLowPinsValue | dwLowPinsValue);
    AddByteToOutputBuffer(dwSavedLowPinsValue, false);

    // shift left by 4 bits ie move general purpose I/O low pins 1-4 from bits 0-3 to bits 4-7
    dwLowPinsDirection = ((dwLowPinsDirection & '\x0F') << 4);

    dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\x0F');
    dwSavedLowPinsDirection = (dwSavedLowPinsDirection | dwLowPinsDirection); 
    AddByteToOutputBuffer(dwSavedLowPinsDirection, false);

    Status = FTC_SendBytesToDevice(ftHandle);
  }

  if (Status == FTC_SUCCESS)
  {
    if (bControlHighInputOutputPins != false)
    {
      // output on the general purpose I/O high pins 1-4
      AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, true);

      dwHighPinsValue = (dwHighPinsValue & '\x0F');
      AddByteToOutputBuffer(dwHighPinsValue, false);

      dwHighPinsDirection = (dwHighPinsDirection & '\x0F');
      AddByteToOutputBuffer(dwHighPinsDirection, false);

      Status = FTC_SendBytesToDevice(ftHandle);
    }
  } 

  return Status;
}

void FT2232cMpsseJtag::GetGeneralPurposeInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTC_LOW_HIGH_PINS pPinsInputData)
{
  if ((dwInputStatesReturnedValue & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
    pPinsInputData->bPin1LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
    pPinsInputData->bPin2LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
    pPinsInputData->bPin3LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
    pPinsInputData->bPin4LowHighState = TRUE;
}

FTC_STATUS FT2232cMpsseJtag::GetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                                              PFTC_LOW_HIGH_PINS pLowPinsInputData,
                                                              BOOL bControlHighInputOutputPins,
                                                              PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;

  pLowPinsInputData->bPin1LowHighState = false;
  pLowPinsInputData->bPin2LowHighState = false;
  pLowPinsInputData->bPin3LowHighState = false;
  pLowPinsInputData->bPin4LowHighState = false;
  pHighPinsInputData->bPin1LowHighState = false;
  pHighPinsInputData->bPin2LowHighState = false;
  pHighPinsInputData->bPin3LowHighState = false;
  pHighPinsInputData->bPin4LowHighState = false;

  // Put in this small delay incase the application programmer does a get GPIOs immediately after a set GPIOs
  //Sleep(5);
  // Removed Sleep() call and replaced with linux sleep to give up 
  // timeslice - sched_yield() -- not sure is best idea! -- Julius
  sched_yield();

  if (bControlLowInputOutputPins != false)
  {
    // Get the number of bytes in the device input buffer
    Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

    if (Status == FTC_SUCCESS)
    {
      if (dwNumBytesDeviceInputBuffer > 0)
        Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

      if (Status == FTC_SUCCESS)
      {
        // get the states of the general purpose I/O low pins 1-4
        AddByteToOutputBuffer(GET_LOW_BYTE_DATA_BITS_CMD, true);
        AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);
        Status = FTC_SendBytesToDevice(ftHandle);

        if (Status == FTC_SUCCESS)
        {
          Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer);

          if (Status == FTC_SUCCESS)
          {
            Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

            if (Status == FTC_SUCCESS)
              // shift right by 4 bits ie move general purpose I/O low pins 1-4 from bits 4-7 to bits 0-3
              GetGeneralPurposeInputOutputPinsInputStates((InputBuffer[0] >> 4), pLowPinsInputData);
          }
        }
      }
    }
  }

  if (bControlHighInputOutputPins != false)
  {
    // Get the number of bytes in the device input buffer
    Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

    if (Status == FTC_SUCCESS)
    {
      if (dwNumBytesDeviceInputBuffer > 0)
        Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

      if (Status == FTC_SUCCESS)
      {
        // get the states of the general purpose I/O high pins 1-4
        AddByteToOutputBuffer(GET_HIGH_BYTE_DATA_BITS_CMD, true);
        AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);
        Status = FTC_SendBytesToDevice(ftHandle);

        if (Status == FTC_SUCCESS)
        {
          Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer);

          if (Status == FTC_SUCCESS)
          {
            Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

            if (Status == FTC_SUCCESS)
              GetGeneralPurposeInputOutputPinsInputStates(InputBuffer[0], pHighPinsInputData);
          }
        }
      }
    }
  }

  return Status;
}

void FT2232cMpsseJtag::AddWriteCommandDataToOutPutBuffer(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                                         PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                         DWORD dwTapControllerState)
{
  DWORD dwModNumBitsToWrite = 0;
  DWORD dwDataBufferIndex = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;
  DWORD dwLastDataBit = 0;
  DWORD dwDataBitIndex = 0;

  // adjust for bit count of 1 less than no of bits
  dwModNumBitsToWrite = (dwNumBitsToWrite - 1);

  if (bInstructionTestData == false)
    MoveJTAGFromOneStateToAnother(ShiftDataRegister, NO_LAST_DATA_BIT, false);
  else
    MoveJTAGFromOneStateToAnother(ShiftInstructionRegister, NO_LAST_DATA_BIT, false);

  dwNumDataBytes = (dwModNumBitsToWrite / 8);

  if (dwNumDataBytes > 0)
  {
    // Number of whole bytes
    dwNumDataBytes = (dwNumDataBytes - 1);

    // clk data bytes out on -ve clk LSB
    AddByteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD, false);
    AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), false);
    AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), false);

    // now add the data bytes to go out
    do
    {
      AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], false);
      dwDataBufferIndex = (dwDataBufferIndex + 1);
    }
    while (dwDataBufferIndex < (dwNumDataBytes + 1));
  }

  dwNumRemainingDataBits = (dwModNumBitsToWrite % 8);

  if (dwNumRemainingDataBits > 0)
  {
    dwNumRemainingDataBits = (dwNumRemainingDataBits - 1);

    //clk data bits out on -ve clk LSB
    AddByteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD, false);
    AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), false);
    AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], false);
  }

  // get last bit
  dwLastDataBit = (*pWriteDataBuffer)[dwDataBufferIndex];
  dwDataBitIndex = (dwNumBitsToWrite % 8);

  if (dwDataBitIndex == 0)
    dwLastDataBit = (dwLastDataBit >> ((8 - dwDataBitIndex) - 1));
  else
    dwLastDataBit = (dwLastDataBit >> (dwDataBitIndex - 1));

  // end it in state passed in, take 1 off the dwTapControllerState variable to correspond with JtagStates enumerated types
  MoveJTAGFromOneStateToAnother(JtagStates((dwTapControllerState - 1)), dwLastDataBit, false);
}

FTC_STATUS FT2232cMpsseJtag::WriteDataToExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                       DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  FTC_ClearOutputBuffer();

  AddWriteCommandDataToOutPutBuffer(bInstructionTestData, dwNumBitsToWrite, pWriteDataBuffer,
                                    dwNumBytesToWrite, dwTapControllerState);

  Status = FTC_SendBytesToDevice(ftHandle);

  return Status;
}

void FT2232cMpsseJtag::GetNumDataBytesToRead(DWORD dwNumBitsToRead, LPDWORD lpdwNumDataBytesToRead, LPDWORD lpdwNumRemainingDataBits)
{
  DWORD dwModNumBitsToRead = 0;
  DWORD dwNumDataBytesToRead = 0;
  DWORD dwNumRemainingDataBits = 0;

  // adjust for bit count of 1 less than no of bits
  dwModNumBitsToRead = (dwNumBitsToRead - 1);

  // Number of whole bytes to read
  dwNumDataBytesToRead = (dwModNumBitsToRead / 8);

  // number of remaining bits
  dwNumRemainingDataBits = (dwModNumBitsToRead % 8);

  // increase the number of whole bytes if bits left over
  if (dwNumRemainingDataBits > 0)
    dwNumDataBytesToRead = (dwNumDataBytesToRead + 1);

  // adjust for SHR of incoming byte
  dwNumRemainingDataBits = (8 - dwNumRemainingDataBits);

  // add 1 for TMS read byte
  dwNumDataBytesToRead = (dwNumDataBytesToRead + 1);

  *lpdwNumDataBytesToRead = dwNumDataBytesToRead;
  *lpdwNumRemainingDataBits = dwNumRemainingDataBits;
}

// This will work out the number of whole bytes to read and adjust for the TMS read
FTC_STATUS FT2232cMpsseJtag::GetDataFromExternalDevice(FTC_HANDLE ftHandle, DWORD dwNumBitsToRead, DWORD dwNumTmsClocks,
                                                       PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumReadDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;
  DWORD dwNumDataBytesRead = 0;
  //  DWORD dwNumBytesDeviceInputBuffer = 0;
  InputByteBuffer InputBuffer;
  DWORD dwBytesReadIndex = 0;
  BYTE LastDataBit = 0;

  GetNumDataBytesToRead(dwNumBitsToRead, &dwNumReadDataBytes, &dwNumRemainingDataBits);

  Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, &InputBuffer, dwNumReadDataBytes, &dwNumDataBytesRead);

  if (Status == FTC_SUCCESS)
  {
    // adjust last 2 bytes
    if (dwNumRemainingDataBits < 8)
    {
      InputBuffer[dwNumReadDataBytes - 2] = (InputBuffer[dwNumReadDataBytes - 2] >> dwNumRemainingDataBits);
      LastDataBit = (InputBuffer[dwNumReadDataBytes - 1] << (dwNumTmsClocks - 1));
      LastDataBit = (LastDataBit & '\x80'); // strip the rest
      InputBuffer[dwNumReadDataBytes - 2] = (InputBuffer[dwNumReadDataBytes - 2] | (LastDataBit >> (dwNumRemainingDataBits - 1)));

      dwNumReadDataBytes = (dwNumReadDataBytes - 1);

      for (dwBytesReadIndex = 0 ; dwBytesReadIndex < dwNumReadDataBytes; dwBytesReadIndex++)
        (*pReadDataBuffer)[dwBytesReadIndex] = InputBuffer[dwBytesReadIndex];
    }
    else // case for 0 bit shift in data + TMS read bit
    {
      LastDataBit = (InputBuffer[dwNumReadDataBytes - 1] << (dwNumTmsClocks - 1));
      LastDataBit = (LastDataBit >> 7); // strip the rest
      InputBuffer[dwNumReadDataBytes - 1] = LastDataBit;

      for (dwBytesReadIndex = 0 ; dwBytesReadIndex < dwNumReadDataBytes; dwBytesReadIndex++)
        (*pReadDataBuffer)[dwBytesReadIndex] = InputBuffer[dwBytesReadIndex];
    }

    *lpdwNumBytesReturned = dwNumReadDataBytes;
  }

  return Status;
}

DWORD FT2232cMpsseJtag::AddReadCommandToOutputBuffer(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
{
  DWORD dwModNumBitsToRead = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;
  DWORD dwNumTmsClocks = 0;

  // adjust for bit count of 1 less than no of bits
  dwModNumBitsToRead = (dwNumBitsToRead - 1);

  if (bInstructionTestData == false)
    MoveJTAGFromOneStateToAnother(ShiftDataRegister, NO_LAST_DATA_BIT, false);
  else
    MoveJTAGFromOneStateToAnother(ShiftInstructionRegister, NO_LAST_DATA_BIT, false);

  dwNumDataBytes = (dwModNumBitsToRead / 8);

  if (dwNumDataBytes > 0)
  {
    // Number of whole bytes
    dwNumDataBytes = (dwNumDataBytes - 1);

    // clk data bytes out on -ve clk LSB
    AddByteToOutputBuffer(CLK_DATA_BYTES_IN_ON_POS_CLK_LSB_FIRST_CMD, false);
    AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), false);
    AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), false);
  }

  // number of remaining bits
  dwNumRemainingDataBits = (dwModNumBitsToRead % 8);

  if (dwNumRemainingDataBits > 0)
  {
    dwNumRemainingDataBits = (dwNumRemainingDataBits - 1);

    //clk data bits out on -ve clk LSB
    AddByteToOutputBuffer(CLK_DATA_BITS_IN_ON_POS_CLK_LSB_FIRST_CMD, false);
    AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), false);
  }

  // end it in state passed in, take 1 off the dwTapControllerState variable to correspond with JtagStates enumerated types
  dwNumTmsClocks = MoveJTAGFromOneStateToAnother(JtagStates((dwTapControllerState - 1)), NO_LAST_DATA_BIT, true);

  return dwNumTmsClocks;
}

FTC_STATUS FT2232cMpsseJtag::ReadDataFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
                                                        PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                                        DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumTmsClocks = 0;

  FTC_ClearOutputBuffer();

  dwNumTmsClocks = AddReadCommandToOutputBuffer(bInstructionTestData, dwNumBitsToRead, dwTapControllerState);

  AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS)
    Status = GetDataFromExternalDevice(ftHandle, dwNumBitsToRead, dwNumTmsClocks, pReadDataBuffer, lpdwNumBytesReturned);

  return Status;
}

DWORD FT2232cMpsseJtag::AddWriteReadCommandDataToOutPutBuffer(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                              PWriteDataByteBuffer pWriteDataBuffer,
                                                              DWORD dwNumBytesToWrite, DWORD dwTapControllerState)
{
  DWORD dwModNumBitsToWriteRead = 0;
  DWORD dwNumWriteDataBytes = 0;
  DWORD dwDataBufferIndex = 0;
  DWORD dwNumRemainingDataBits = 0;
  DWORD dwLastDataBit = 0;
  DWORD dwDataBitIndex = 0;
  DWORD dwNumTmsClocks = 0;

  // adjust for bit count of 1 less than no of bits
  dwModNumBitsToWriteRead = (dwNumBitsToWriteRead - 1);

  if (bInstructionTestData == false)
    MoveJTAGFromOneStateToAnother(ShiftDataRegister, NO_LAST_DATA_BIT, false);
  else
    MoveJTAGFromOneStateToAnother(ShiftInstructionRegister, NO_LAST_DATA_BIT, false);

  dwNumWriteDataBytes = (dwModNumBitsToWriteRead / 8);

  if (dwNumWriteDataBytes > 0)
  {
    // Number of whole bytes
    dwNumWriteDataBytes = (dwNumWriteDataBytes - 1);

    // clk data bytes out on -ve in +ve clk LSB
    AddByteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_IN_ON_POS_CLK_LSB_FIRST_CMD, false);
    AddByteToOutputBuffer((dwNumWriteDataBytes & '\xFF'), false);
    AddByteToOutputBuffer(((dwNumWriteDataBytes / 256) & '\xFF'), false);

    // now add the data bytes to go out
    do
    {
      AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], false);
      dwDataBufferIndex = (dwDataBufferIndex + 1);
    }
    while (dwDataBufferIndex < (dwNumWriteDataBytes + 1));
  }

  dwNumRemainingDataBits = (dwModNumBitsToWriteRead % 8);

  if (dwNumRemainingDataBits > 0)
  {
    dwNumRemainingDataBits = (dwNumRemainingDataBits - 1);

    // clk data bits out on -ve in +ve clk LSB
    AddByteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_IN_ON_POS_CLK_LSB_FIRST_CMD, false);
    AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), false);
    AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], false);
  }

  // get last bit
  dwLastDataBit = (*pWriteDataBuffer)[dwDataBufferIndex];
  dwDataBitIndex = (dwNumBitsToWriteRead % 8);

  if (dwDataBitIndex == 8)
    dwLastDataBit = (dwLastDataBit >> ((8 - dwDataBitIndex) - 1));
  else
    dwLastDataBit = (dwLastDataBit >> (dwDataBitIndex - 1));

  // end it in state passed in, take 1 off the dwTapControllerState variable to correspond with JtagStates enumerated types
  dwNumTmsClocks = MoveJTAGFromOneStateToAnother(JtagStates((dwTapControllerState - 1)), dwLastDataBit, true);

  return dwNumTmsClocks;
}

FTC_STATUS FT2232cMpsseJtag::WriteReadDataToFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                               PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                               PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                                               DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumRemainingDataBits = 0;
  BYTE LastDataBit = 0;
  DWORD dwNumTmsClocks = 0;
  DWORD dwNumReadDataBytes = 0;
  InputByteBuffer InputBuffer;
  DWORD dwNumDataBytesRead = 0;
  DWORD dwBytesReadIndex = 0;

  FTC_ClearOutputBuffer();

  dwNumTmsClocks = AddWriteReadCommandDataToOutPutBuffer(bInstructionTestData, dwNumBitsToWriteRead,
                                                         pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);

  AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);

  GetNumDataBytesToRead(dwNumBitsToWriteRead, &dwNumReadDataBytes, &dwNumRemainingDataBits);

  Status = FTC_SendReadBytesToFromDevice(ftHandle, &InputBuffer, dwNumReadDataBytes, &dwNumDataBytesRead);

  if (Status == FTC_SUCCESS)
  {
    // adjust last 2 bytes
    if (dwNumRemainingDataBits < 8)
    {
      InputBuffer[dwNumReadDataBytes - 2] = (InputBuffer[dwNumReadDataBytes - 2] >> dwNumRemainingDataBits);
      LastDataBit = (InputBuffer[dwNumReadDataBytes - 1] << (dwNumTmsClocks - 1));
      LastDataBit = (LastDataBit & '\x80'); // strip the rest
      InputBuffer[dwNumReadDataBytes - 2] = (InputBuffer[dwNumReadDataBytes - 2] | (LastDataBit >> (dwNumRemainingDataBits - 1)));

      dwNumReadDataBytes = (dwNumReadDataBytes - 1);

      for (dwBytesReadIndex = 0 ; dwBytesReadIndex < dwNumReadDataBytes; dwBytesReadIndex++)
        (*pReadDataBuffer)[dwBytesReadIndex] = InputBuffer[dwBytesReadIndex];
    }
    else // case for 0 bit shift in data + TMS read bit
    {
      LastDataBit = (InputBuffer[dwNumReadDataBytes - 1] << (dwNumTmsClocks - 1));
      LastDataBit = (LastDataBit >> 7); // strip the rest
      InputBuffer[dwNumReadDataBytes - 1] = LastDataBit;

      for (dwBytesReadIndex = 0 ; dwBytesReadIndex < dwNumReadDataBytes; dwBytesReadIndex++)
        (*pReadDataBuffer)[dwBytesReadIndex] = InputBuffer[dwBytesReadIndex];
    }

    *lpdwNumBytesReturned = dwNumReadDataBytes;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::GenerateTCKClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwTotalNumClockPulsesBytes = 0;
  DWORD dwNumClockPulsesByteBlocks = 0;
  DWORD dwNumClockPulsesByteBlockCntr = 0;
  DWORD dwNumClockPulsesBytes = 0;
  DWORD dwNumRemainingClockPulsesBits = 0;
  //  DWORD dwDataBufferIndex = 0;

  MoveJTAGFromOneStateToAnother(RunTestIdle, NO_LAST_DATA_BIT, FALSE);

  dwTotalNumClockPulsesBytes = (dwNumClockPulses / NUMBITSINBYTE);

  if (dwTotalNumClockPulsesBytes > 0)
  {
    dwNumClockPulsesByteBlocks = (dwTotalNumClockPulsesBytes / NUM_BYTE_CLOCK_PULSES_BLOCK_SIZE);

    do
    {
      if (dwNumClockPulsesByteBlockCntr < dwNumClockPulsesByteBlocks)
        dwNumClockPulsesBytes = NUM_BYTE_CLOCK_PULSES_BLOCK_SIZE;
      else
        dwNumClockPulsesBytes = (dwTotalNumClockPulsesBytes - (dwNumClockPulsesByteBlockCntr * NUM_BYTE_CLOCK_PULSES_BLOCK_SIZE));

      if (dwNumClockPulsesBytes > 0)
      {
        // Number of whole bytes
        dwNumClockPulsesBytes = (dwNumClockPulsesBytes - 1);

        // clk data bytes out on -ve clk LSB
        AddByteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD, false);
        AddByteToOutputBuffer((dwNumClockPulsesBytes & '\xFF'), false);
        AddByteToOutputBuffer(((dwNumClockPulsesBytes / 256) & '\xFF'), false);

        DWORD dwDataBufferIndex = 0;

        // now add the data bytes ie 0 to go out with the clock pulses
        do
        {
          AddByteToOutputBuffer(0, false);
          dwDataBufferIndex = (dwDataBufferIndex + 1);
        }
        while (dwDataBufferIndex < (dwNumClockPulsesBytes + 1));
      }

      Status = FTC_SendBytesToDevice(ftHandle);

      dwNumClockPulsesByteBlockCntr = (dwNumClockPulsesByteBlockCntr + 1);
    }
    while ((dwNumClockPulsesByteBlockCntr <= dwNumClockPulsesByteBlocks) && (Status == FTC_SUCCESS));
  }

  if (Status == FTC_SUCCESS)
  {
    dwNumRemainingClockPulsesBits = (dwNumClockPulses % NUMBITSINBYTE);

    if (dwNumRemainingClockPulsesBits > 0)
    {
      dwNumRemainingClockPulsesBits = (dwNumRemainingClockPulsesBits - 1);

      //clk data bits out on -ve clk LSB
      AddByteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD, false);
      AddByteToOutputBuffer((dwNumRemainingClockPulsesBits & '\xFF'), false);
      AddByteToOutputBuffer('\xFF', false);

      Status = FTC_SendBytesToDevice(ftHandle);
    }
  }

  //MoveJTAGFromOneStateToAnother(RunTestIdle, NO_LAST_DATA_BIT, FALSE);

  //Status = FTC_SendBytesToDevice(ftHandle);

  return Status;
}

void FT2232cMpsseJtag::ProcessReadCommandsSequenceBytes(PInputByteBuffer pInputBuffer, DWORD dwNumBytesRead, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                                        LPDWORD lpdwNumBytesReturned)
{
  DWORD CommandSequenceIndex = 0;
  PReadCommandsSequenceData pReadCommandsSequenceDataBuffer;
  DWORD dwNumReadCommandSequences;
  PReadCommandSequenceData pReadCmdSequenceData;
  DWORD dwNumBitsToRead = 0;
  DWORD dwNumTmsClocks = 0;
  DWORD dwNumReadDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;
  BYTE  LastDataBit = 0;
  DWORD dwBytesReadIndex = 0;
  DWORD dwNumReadBytesProcessed = 0;
  DWORD dwTotalNumBytesRead = 0;
  DWORD dwNumBytesReturned = 0;

  if (iCommandsSequenceDataDeviceIndex != -1)
  {
    pReadCommandsSequenceDataBuffer = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pReadCommandsSequenceDataBuffer;
    dwNumReadCommandSequences = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences;

    for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwNumReadCommandSequences); CommandSequenceIndex++)
    {
      pReadCmdSequenceData = (*pReadCommandsSequenceDataBuffer)[CommandSequenceIndex];
      dwNumBitsToRead = (*pReadCmdSequenceData)[0];
      dwNumTmsClocks = (*pReadCmdSequenceData)[1];

      GetNumDataBytesToRead(dwNumBitsToRead, &dwNumReadDataBytes, &dwNumRemainingDataBits);

      dwNumReadBytesProcessed = (dwNumReadBytesProcessed + dwNumReadDataBytes);

      // adjust last 2 bytes
      if (dwNumRemainingDataBits < 8)
      {
        (*pInputBuffer)[dwNumReadBytesProcessed - 2] = ((*pInputBuffer)[dwNumReadBytesProcessed - 2] >> dwNumRemainingDataBits);
        LastDataBit = ((*pInputBuffer)[dwNumReadBytesProcessed - 1] << (dwNumTmsClocks - 1));
        LastDataBit = (LastDataBit & '\x80'); // strip the rest
        (*pInputBuffer)[dwNumReadBytesProcessed - 2] = ((*pInputBuffer)[dwNumReadBytesProcessed - 2] | (LastDataBit >> (dwNumRemainingDataBits - 1)));

        dwNumReadDataBytes = (dwNumReadDataBytes - 1);

        for (dwBytesReadIndex = 0; dwBytesReadIndex < dwNumReadDataBytes; dwBytesReadIndex++)
          (*pReadCmdSequenceDataBuffer)[(dwBytesReadIndex + dwNumBytesReturned)] = (*pInputBuffer)[(dwBytesReadIndex + dwTotalNumBytesRead)];
      }
      else // case for 0 bit shift in data + TMS read bit
      {
        LastDataBit = ((*pInputBuffer)[dwNumReadBytesProcessed - 1] << (dwNumTmsClocks - 1));
        LastDataBit = (LastDataBit >> 7); // strip the rest
        (*pInputBuffer)[dwNumReadBytesProcessed - 1] = LastDataBit;

        for (dwBytesReadIndex = 0; dwBytesReadIndex < dwNumReadDataBytes; dwBytesReadIndex++)
          (*pReadCmdSequenceDataBuffer)[(dwBytesReadIndex + dwNumBytesReturned)] = (*pInputBuffer)[(dwBytesReadIndex + dwTotalNumBytesRead)];
      }

      dwTotalNumBytesRead = dwNumReadBytesProcessed;

      dwNumBytesReturned = (dwNumBytesReturned + dwNumReadDataBytes);
    }
  }

  *lpdwNumBytesReturned = dwNumBytesReturned;
}

DWORD FT2232cMpsseJtag::GetTotalNumCommandsSequenceDataBytesToRead(void)
{
  DWORD dwTotalNumBytesToBeRead = 0;
  DWORD CommandSequenceIndex = 0;
  PReadCommandsSequenceData pReadCommandsSequenceDataBuffer;
  DWORD dwNumReadCommandSequences = 0;
  PReadCommandSequenceData pReadCmdSequenceData;
  DWORD dwNumBitsToRead = 0;
  DWORD dwNumDataBytesToRead = 0;
  DWORD dwNumRemainingDataBits = 0;

  if (iCommandsSequenceDataDeviceIndex != -1)
  {
    pReadCommandsSequenceDataBuffer = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pReadCommandsSequenceDataBuffer;
    dwNumReadCommandSequences = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences;

    for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwNumReadCommandSequences); CommandSequenceIndex++)
    {
      pReadCmdSequenceData = (*pReadCommandsSequenceDataBuffer)[CommandSequenceIndex];
      dwNumBitsToRead = (*pReadCmdSequenceData)[0];

      GetNumDataBytesToRead(dwNumBitsToRead, &dwNumDataBytesToRead, &dwNumRemainingDataBits);

      dwTotalNumBytesToBeRead = (dwTotalNumBytesToBeRead + dwNumDataBytesToRead);
    }
  }

  return dwTotalNumBytesToBeRead;
}

void FT2232cMpsseJtag::CopyReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pDestinationBuffer, PReadCommandsSequenceData pSourceBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer)
{
  DWORD CommandSequenceIndex = 0;
  PReadCommandSequenceData pReadCmdSequenceData;
  DWORD dwNumBitsToRead = 0;
  DWORD dwNumTmsClocks = 0;

  for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwSizeReadCommandsSequenceDataBuffer); CommandSequenceIndex++)
  {
    pReadCmdSequenceData = (*pSourceBuffer)[CommandSequenceIndex];
    dwNumBitsToRead = (*pReadCmdSequenceData)[0];
    dwNumTmsClocks = (*pReadCmdSequenceData)[1];

    pReadCmdSequenceData = (*pDestinationBuffer)[CommandSequenceIndex];
    (*pReadCmdSequenceData)[0] = dwNumBitsToRead;
    (*pReadCmdSequenceData)[1] = dwNumTmsClocks;
  }
}

FTC_STATUS FT2232cMpsseJtag::AddReadCommandSequenceData(DWORD dwNumBitsToRead, DWORD dwNumTmsClocks)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwSizeReadCommandsSequenceDataBuffer;
  PReadCommandsSequenceData pReadCommandsSequenceDataBuffer;
  DWORD dwNumReadCommandSequences;
  PReadCommandsSequenceData pTmpReadCmdsSequenceDataBuffer;
  PReadCommandSequenceData pReadCmdSequenceData;

  if (iCommandsSequenceDataDeviceIndex != -1)
  {
    dwSizeReadCommandsSequenceDataBuffer = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwSizeReadCommandsSequenceDataBuffer;
    pReadCommandsSequenceDataBuffer = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pReadCommandsSequenceDataBuffer;
    dwNumReadCommandSequences = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences;

    if (dwNumReadCommandSequences > (dwSizeReadCommandsSequenceDataBuffer - 1))
    {
      pTmpReadCmdsSequenceDataBuffer = CreateReadCommandsSequenceDataBuffer(dwSizeReadCommandsSequenceDataBuffer);

      if (pTmpReadCmdsSequenceDataBuffer != NULL)
      {
        // Temporary save the contents of the read commands sequence data buffer
        CopyReadCommandsSequenceDataBuffer(pTmpReadCmdsSequenceDataBuffer, pReadCommandsSequenceDataBuffer, dwSizeReadCommandsSequenceDataBuffer);

        DeleteReadCommandsSequenceDataBuffer(pReadCommandsSequenceDataBuffer, dwSizeReadCommandsSequenceDataBuffer);

        // Increase the size of the read commands sequence data buffer
        pReadCommandsSequenceDataBuffer = CreateReadCommandsSequenceDataBuffer((dwSizeReadCommandsSequenceDataBuffer + COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE_INCREMENT));
  
        if (pReadCommandsSequenceDataBuffer != NULL)
        {
          CopyReadCommandsSequenceDataBuffer(pReadCommandsSequenceDataBuffer, pTmpReadCmdsSequenceDataBuffer, dwSizeReadCommandsSequenceDataBuffer);

          DeleteReadCommandsSequenceDataBuffer(pTmpReadCmdsSequenceDataBuffer, dwSizeReadCommandsSequenceDataBuffer);

          dwSizeReadCommandsSequenceDataBuffer = (dwSizeReadCommandsSequenceDataBuffer + COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE_INCREMENT);

          OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwSizeReadCommandsSequenceDataBuffer = dwSizeReadCommandsSequenceDataBuffer;
          OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pReadCommandsSequenceDataBuffer = pReadCommandsSequenceDataBuffer;
        }
        else
          Status = FTC_INSUFFICIENT_RESOURCES;
      }
      else
        Status = FTC_INSUFFICIENT_RESOURCES;
    }

    if (Status == FTC_SUCCESS)
    {
      if (dwNumReadCommandSequences > 0)
        pReadCmdSequenceData = (*pReadCommandsSequenceDataBuffer)[(dwNumReadCommandSequences - 1)];

      pReadCmdSequenceData = (*pReadCommandsSequenceDataBuffer)[dwNumReadCommandSequences];

      (*pReadCmdSequenceData)[0] = dwNumBitsToRead;
      (*pReadCmdSequenceData)[1] = dwNumTmsClocks;

      dwNumReadCommandSequences = (dwNumReadCommandSequences + 1);

      OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences = dwNumReadCommandSequences;
    }
  }

  return Status;
}

PReadCommandsSequenceData FT2232cMpsseJtag::CreateReadCommandsSequenceDataBuffer(DWORD dwSizeReadCmdsSequenceDataBuffer)
{
  PReadCommandsSequenceData pReadCmdsSequenceDataBuffer;
  DWORD CommandSequenceIndex = 0;
  PReadCommandSequenceData pReadCmdSequenceData;

  pReadCmdsSequenceDataBuffer = PReadCommandsSequenceData(new ReadCommandsSequenceData[dwSizeReadCmdsSequenceDataBuffer]);

  if (pReadCmdsSequenceDataBuffer != NULL)
  {
    for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwSizeReadCmdsSequenceDataBuffer); CommandSequenceIndex++)
    {
      pReadCmdSequenceData = PReadCommandSequenceData(new ReadCommandSequenceData);

      (*pReadCmdsSequenceDataBuffer)[CommandSequenceIndex] = pReadCmdSequenceData;
    }
  }

  return pReadCmdsSequenceDataBuffer;
}

void FT2232cMpsseJtag::DeleteReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pReadCmdsSequenceDataBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer)
{
  DWORD CommandSequenceIndex = 0;
  PReadCommandSequenceData pReadCmdSequenceData;

  for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwSizeReadCommandsSequenceDataBuffer); CommandSequenceIndex++)
  {
    pReadCmdSequenceData = (*pReadCmdsSequenceDataBuffer)[CommandSequenceIndex];

    delete [] pReadCmdSequenceData;
  }

  delete [] pReadCmdsSequenceDataBuffer;
}

FTC_STATUS FT2232cMpsseJtag::CreateDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwDeviceIndex = 0;
  BOOLEAN bDeviceDataBuffersCreated = false;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceDataBuffersCreated); dwDeviceIndex++)
  {
    if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == 0)
    {
      bDeviceDataBuffersCreated = true;

      OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer = POutputByteBuffer(new OutputByteBuffer);

      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer != NULL)
      {
        OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer = CreateReadCommandsSequenceDataBuffer(INIT_COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE);

        if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer != NULL)
        {
          OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice = ftHandle;
          OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumBytesToSend = 0;
          OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwSizeReadCommandsSequenceDataBuffer = INIT_COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE;
          OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumReadCommandSequences = 0;
        }
        else
        {
          delete [] OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer;

          Status = FTC_INSUFFICIENT_RESOURCES;
        }
      }
      else
        Status = FTC_INSUFFICIENT_RESOURCES;
    }
  }

  if ((Status == FTC_SUCCESS) && (bDeviceDataBuffersCreated == true))
    dwNumOpenedDevices = dwNumOpenedDevices + 1;

  return Status;
}

void FT2232cMpsseJtag::ClearDeviceCommandSequenceData(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOLEAN bDeviceHandleFound = false;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = true;

        OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumBytesToSend = 0;
        OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumReadCommandSequences = 0;
      }
    }
  }
  else
  {
    // This code is executed if there is only one device connected to the system, this code is here just in case
    // that a device was unplugged from the system, while the system was still running
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice != 0)
      {
        bDeviceHandleFound = true;

        OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumBytesToSend = 0;
        OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumReadCommandSequences = 0;
      }
    }
  }
}

DWORD FT2232cMpsseJtag::GetNumBytesInCommandsSequenceDataBuffer(void)
{
  DWORD dwNumBytesToSend_temp = 0;

  if (iCommandsSequenceDataDeviceIndex != -1)
    // Get the number commands to be executed in sequence ie write, read and write/read
    dwNumBytesToSend_temp = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend;

  return dwNumBytesToSend_temp;
}

DWORD FT2232cMpsseJtag::GetCommandsSequenceDataDeviceIndex(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOLEAN bDeviceHandleFound = false;
  INT iCmdsSequenceDataDeviceIndex = 0;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = true;

        iCmdsSequenceDataDeviceIndex = dwDeviceIndex;
      }
    }
  }
  else
  {
    // This code is executed if there is only one device connected to the system, this code is here just in case
    // that a device was unplugged from the system, while the system was still running
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice != 0)
      {
        bDeviceHandleFound = true;

        iCmdsSequenceDataDeviceIndex = dwDeviceIndex;
      }
    }
  }

  return iCmdsSequenceDataDeviceIndex;
}

void FT2232cMpsseJtag::DeleteDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOLEAN bDeviceHandleFound = false;
  POutputByteBuffer pCmdsSequenceDataOutPutBuffer;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
  {
    if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == ftHandle)
    {
      bDeviceHandleFound = true;

      OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice = 0;
      OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumBytesToSend = 0;
      pCmdsSequenceDataOutPutBuffer = OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer;
      delete [] pCmdsSequenceDataOutPutBuffer;
      OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer = NULL;
      DeleteReadCommandsSequenceDataBuffer(OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer,
                                           OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwSizeReadCommandsSequenceDataBuffer);
      OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer = NULL;
    }
  }

  if ((dwNumOpenedDevices > 0) && bDeviceHandleFound)
    dwNumOpenedDevices = dwNumOpenedDevices - 1;
}

FTC_STATUS FT2232cMpsseJtag::AddDeviceWriteCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                                   PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                   DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumCommandDataBytes = 0;

  if (pWriteDataBuffer != NULL)
  {
    Status = CheckWriteDataToExternalDeviceBitsBytesParameters(dwNumBitsToWrite, dwNumBytesToWrite);

    if (Status == FTC_SUCCESS)
    {
      if ((dwTapControllerState >= TEST_LOGIC_STATE) && (dwTapControllerState <= SHIFT_INSTRUCTION_REGISTER_STATE))
      {
        dwNumCommandDataBytes = (NUM_WRITE_COMMAND_BYTES + dwNumBytesToWrite);

        iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

        if ((GetNumBytesInCommandsSequenceDataBuffer() + dwNumCommandDataBytes) < OUTPUT_BUFFER_SIZE)
          AddWriteCommandDataToOutPutBuffer(bInstructionTestData, dwNumBitsToWrite, pWriteDataBuffer,
                                            dwNumBytesToWrite, dwTapControllerState);
        else
          Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;

        // Reset to indicate that you are not building up a sequence of commands
        iCommandsSequenceDataDeviceIndex = -1;
      }
      else
        Status = FTC_INVALID_TAP_CONTROLLER_STATE;
    }
  }
  else
    Status = FTC_NULL_WRITE_DATA_BUFFER_POINTER;

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::AddDeviceReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumTmsClocks = 0;

  if ((dwNumBitsToRead >= MIN_NUM_BITS) && (dwNumBitsToRead <= MAX_NUM_BITS))
  {
    if ((dwTapControllerState >= TEST_LOGIC_STATE) && (dwTapControllerState <= SHIFT_INSTRUCTION_REGISTER_STATE))
    {
      iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

      if ((GetNumBytesInCommandsSequenceDataBuffer() + NUM_READ_COMMAND_BYTES) < OUTPUT_BUFFER_SIZE)
      {
        dwNumTmsClocks = AddReadCommandToOutputBuffer(bInstructionTestData, dwNumBitsToRead, dwTapControllerState);

        Status = AddReadCommandSequenceData(dwNumBitsToRead, dwNumTmsClocks);
      }
      else
        Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;

      // Reset to indicate that you are not building up a sequence of commands
      iCommandsSequenceDataDeviceIndex = -1;
    }
    else
      Status = FTC_INVALID_TAP_CONTROLLER_STATE;
  }
  else
    Status = FTC_INVALID_NUMBER_BITS;

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::AddDeviceWriteReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                       DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumCommandDataBytes = 0;
  DWORD dwNumTmsClocks = 0;

  if (pWriteDataBuffer != NULL)
  {
    Status = CheckWriteDataToExternalDeviceBitsBytesParameters(dwNumBitsToWriteRead, dwNumBytesToWrite);

    if (Status == FTC_SUCCESS)
    {
      if ((dwTapControllerState >= TEST_LOGIC_STATE) && (dwTapControllerState <= SHIFT_INSTRUCTION_REGISTER_STATE))
      {
        dwNumCommandDataBytes = (NUM_WRITE_READ_COMMAND_BYTES + dwNumBytesToWrite);

        iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

        if ((GetNumBytesInCommandsSequenceDataBuffer() + dwNumCommandDataBytes) < OUTPUT_BUFFER_SIZE)
        {
          dwNumTmsClocks = AddWriteReadCommandDataToOutPutBuffer(bInstructionTestData, dwNumBitsToWriteRead, pWriteDataBuffer,
                                                                   dwNumBytesToWrite, dwTapControllerState);
 
          Status = AddReadCommandSequenceData(dwNumBitsToWriteRead, dwNumTmsClocks);
        }
        else
          Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;

        // Reset to indicate that you are not building up a sequence of commands
        iCommandsSequenceDataDeviceIndex = -1;
      }
      else
        Status = FTC_INVALID_TAP_CONTROLLER_STATE;
    }
  }
  else
    Status = FTC_NULL_WRITE_DATA_BUFFER_POINTER;

  return Status;
}

FT2232cMpsseJtag::FT2232cMpsseJtag(void)
{
  DWORD dwDeviceIndex = 0;

  CurrentJtagState = Undefined;

  dwSavedLowPinsDirection = 0;
  dwSavedLowPinsValue = 0;

  dwNumOpenedDevices = 0;

  for (dwDeviceIndex = 0; (dwDeviceIndex < MAX_NUM_DEVICES); dwDeviceIndex++)
    OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice = 0;

  iCommandsSequenceDataDeviceIndex = -1;
}

FT2232cMpsseJtag::~FT2232cMpsseJtag(void)
{
  DWORD dwDeviceIndex = 0;
  POutputByteBuffer pCmdsSequenceDataOutPutBuffer;

  if (dwNumOpenedDevices > 0)
  {
    for (dwDeviceIndex = 0; (dwDeviceIndex < MAX_NUM_DEVICES); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice != 0)
      {
        OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice = 0;

        pCmdsSequenceDataOutPutBuffer = OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer;

        if (pCmdsSequenceDataOutPutBuffer != NULL)
          delete [] pCmdsSequenceDataOutPutBuffer;

        OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer = NULL;

        if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer != NULL)
          DeleteReadCommandsSequenceDataBuffer(OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer,
                                               OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwSizeReadCommandsSequenceDataBuffer);

        OpenedDevicesCommandsSequenceData[dwDeviceIndex].pReadCommandsSequenceDataBuffer = NULL;
      }
    }
  }
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GetNumDevices(LPDWORD lpdwNumDevices)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FT2232CDeviceIndexes FT2232CIndexes;

  *lpdwNumDevices = 0;

  Status = FTC_GetNumDevices(lpdwNumDevices, &FT2232CIndexes);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GetDeviceNameLocationID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
//FTC_STATUS FT2232cMpsseJtag::JTAG_GetDeviceNameLocID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
{
  return FTC_GetDeviceNameLocationID(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID);
}

FTC_STATUS FT2232cMpsseJtag::JTAG_OpenSpecifiedDevice(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
//FTC_STATUS FT2232cMpsseJtag::JTAG_OpenEx(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_OpenSpecifiedDevice(lpDeviceName, dwLocationID, pftHandle);
  
  if (Status == FTC_SUCCESS)
  {
    Status = CreateDeviceCommandsSequenceDataBuffers(*pftHandle);

    if (Status != FTC_SUCCESS)
    {
      FTC_CloseDevice(*pftHandle);

      *pftHandle = 0;
    }
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_OpenDevice(FTC_HANDLE *pftHandle)
//FTC_STATUS FT2232cMpsseJtag::JTAG_Open(FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_OpenDevice(pftHandle);

  if (Status == FTC_SUCCESS)
  {
    Status = CreateDeviceCommandsSequenceDataBuffers(*pftHandle);

    if (Status != FTC_SUCCESS)
    {
      //      FTC_CloseDevice(*pftHandle);
      FTC_CloseDevice(*pftHandle);

      *pftHandle = 0;
    }
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_CloseDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_CloseDevice(ftHandle);

  DeleteDeviceCommandsSequenceDataBuffers(ftHandle);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    {
      Status = FTC_ResetUSBDevicePurgeUSBInputBuffer(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceUSBBufferSizes(ftHandle, USB_INPUT_BUFFER_SIZE, USB_OUTPUT_BUFFER_SIZE);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceSpecialCharacters(ftHandle, false, FT_EVENT_VALUE, false, FT_ERROR_VALUE);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetReadWriteDeviceTimeouts(ftHandle, DEVICE_READ_TIMEOUT_INFINITE, DEVICE_WRITE_TIMEOUT);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceLatencyTimer(ftHandle, DEVICE_LATENCY_TIMER_VALUE);

      if (Status == FTC_SUCCESS)
        Status = FTC_ResetMPSSEInterface(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_EnableMPSSEInterface(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_SynchronizeMPSSEInterface(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_ResetUSBDevicePurgeUSBInputBuffer(ftHandle);

      if (Status == FTC_SUCCESS)
        //Sleep(20); // wait for all the USB stuff to complete
	// Removed Sleep() call and replaced with linux sleep to give up 
	// timeslice - sched_yield() -- not sure is best idea! -- Julius
	sched_yield();

      if (Status == FTC_SUCCESS)
        Status = InitDataInOutClockFrequency(ftHandle, dwClockDivisor);

      if (Status == FTC_SUCCESS)
	//        Status = FTC_SetDeviceLoopbackState(ftHandle, false);
        Status = FTC_SetDeviceLoopbackState(ftHandle, false);

      if (Status == FTC_SUCCESS)
        //Sleep(20); // wait for all the USB stuff to complete
	// Removed Sleep() call and replaced with linux sleep to give up 
	// timeslice - sched_yield() -- not sure is best idea! -- Julius
	sched_yield();

      if (Status == FTC_SUCCESS)
        Status = FTC_ResetUSBDevicePurgeUSBInputBuffer(ftHandle);

      if (Status == FTC_SUCCESS)
        //Sleep(20); // wait for all the USB stuff to complete
	// Removed Sleep() call and replaced with linux sleep to give up 
	// timeslice - sched_yield() -- not sure is best idea! -- Julius
	sched_yield();

      if (Status == FTC_SUCCESS)
        Status = ResetTAPContollerExternalDeviceSetToTestIdleMode(ftHandle);
    }
    else
      Status = FTC_INVALID_CLOCK_DIVISOR;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    FTC_GetClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    {
      Status = SetDataInOutClockFrequency(ftHandle, dwClockDivisor);

      if (Status == FTC_SUCCESS)
        FTC_GetClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
    }
    else
      Status = FTC_INVALID_CLOCK_DIVISOR;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_SetDeviceLoopbackState(FTC_HANDLE ftHandle, BOOL bLoopbackState)
//FTC_STATUS FT2232cMpsseJtag::JTAG_SetLoopback(FTC_HANDLE ftHandle, BOOL bLoopbackState)
{
  return FTC_SetDeviceLoopbackState(ftHandle, bLoopbackState);
}

FTC_STATUS FT2232cMpsseJtag::JTAG_SetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
//FTC_STATUS FT2232cMpsseJtag::JTAG_SetGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                                                   PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,
                                                                   BOOL bControlHighInputOutputPins,
                                                                   PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLowPinsDirection = 0;
  DWORD dwLowPinsValue = 0;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((pLowInputOutputPinsData != NULL) && (pHighInputOutputPinsData != NULL))
    {
      if (pLowInputOutputPinsData->bPin1InputOutputState != false)
        dwLowPinsDirection = (dwLowPinsDirection | '\x01');
      if (pLowInputOutputPinsData->bPin2InputOutputState != false)
        dwLowPinsDirection = (dwLowPinsDirection | '\x02');
      if (pLowInputOutputPinsData->bPin3InputOutputState != false)
        dwLowPinsDirection = (dwLowPinsDirection | '\x04');
      if (pLowInputOutputPinsData->bPin4InputOutputState != false)
        dwLowPinsDirection = (dwLowPinsDirection | '\x08');
      if (pLowInputOutputPinsData->bPin1LowHighState != false)
        dwLowPinsValue = (dwLowPinsValue | '\x01');
      if (pLowInputOutputPinsData->bPin2LowHighState != false)
        dwLowPinsValue = (dwLowPinsValue | '\x02');
      if (pLowInputOutputPinsData->bPin3LowHighState != false)
        dwLowPinsValue = (dwLowPinsValue | '\x04');
      if (pLowInputOutputPinsData->bPin4LowHighState != false)
        dwLowPinsValue = (dwLowPinsValue | '\x08');

      if (pHighInputOutputPinsData->bPin1InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x01');
      if (pHighInputOutputPinsData->bPin2InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x02');
      if (pHighInputOutputPinsData->bPin3InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x04');
      if (pHighInputOutputPinsData->bPin4InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x08');
      if (pHighInputOutputPinsData->bPin1LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x01');
      if (pHighInputOutputPinsData->bPin2LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x02');
      if (pHighInputOutputPinsData->bPin3LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x04');
      if (pHighInputOutputPinsData->bPin4LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x08');

      Status = SetGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
                                                dwLowPinsDirection, dwLowPinsValue,
                                                bControlHighInputOutputPins,
                                                dwHighPinsDirection, dwHighPinsValue);
    }
    else
      Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
//FTC_STATUS FT2232cMpsseJtag::JTAG_GetGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                                                   PFTC_LOW_HIGH_PINS pLowPinsInputData,
                                                                   BOOL bControlHighInputOutputPins,
                                                                   PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((pLowPinsInputData != NULL) && (pHighPinsInputData != NULL))
      Status = GetGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins, pLowPinsInputData,
                                                bControlHighInputOutputPins, pHighPinsInputData);
    else
      Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_WriteDataToExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
//FTC_STATUS FT2232cMpsseJtag::JTAG_Write(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                                            PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                            DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pWriteDataBuffer != NULL)
    {
      Status = CheckWriteDataToExternalDeviceBitsBytesParameters(dwNumBitsToWrite, dwNumBytesToWrite);

      if (Status == FTC_SUCCESS)
      {
        if ((dwTapControllerState >= TEST_LOGIC_STATE) && (dwTapControllerState <= SHIFT_INSTRUCTION_REGISTER_STATE))
          Status = WriteDataToExternalDevice(ftHandle, bInstructionTestData, dwNumBitsToWrite, pWriteDataBuffer,
                                             dwNumBytesToWrite, dwTapControllerState);
        else
          Status = FTC_INVALID_TAP_CONTROLLER_STATE;
      }
    }
    else
      Status = FTC_NULL_WRITE_DATA_BUFFER_POINTER;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
//FTC_STATUS FT2232cMpsseJtag::JTAG_Read(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
                                                             PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                                             DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pReadDataBuffer != NULL)
    {
      if ((dwNumBitsToRead >= MIN_NUM_BITS) && (dwNumBitsToRead <= MAX_NUM_BITS))
      {
        if ((dwTapControllerState >= TEST_LOGIC_STATE) && (dwTapControllerState <= SHIFT_INSTRUCTION_REGISTER_STATE))
          Status = ReadDataFromExternalDevice(ftHandle, bInstructionTestData, dwNumBitsToRead, pReadDataBuffer,
                                              lpdwNumBytesReturned, dwTapControllerState);
        else
          Status = FTC_INVALID_TAP_CONTROLLER_STATE;
      }
      else
        Status = FTC_INVALID_NUMBER_BITS;
    }
    else
      Status = FTC_NULL_READ_DATA_BUFFER_POINTER;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_WriteReadDataToFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
//FTC_STATUS FT2232cMpsseJtag::JTAG_WriteRead(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                                    PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                                    PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                                                    DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((pWriteDataBuffer != NULL) && (pReadDataBuffer != NULL))
    {
      Status = CheckWriteDataToExternalDeviceBitsBytesParameters(dwNumBitsToWriteRead, dwNumBytesToWrite);

      if (Status == FTC_SUCCESS)
      {
        if ((dwTapControllerState >= TEST_LOGIC_STATE) && (dwTapControllerState <= SHIFT_INSTRUCTION_REGISTER_STATE))
          Status = WriteReadDataToFromExternalDevice(ftHandle, bInstructionTestData, dwNumBitsToWriteRead,
                                                     pWriteDataBuffer, dwNumBytesToWrite, pReadDataBuffer,
                                                     lpdwNumBytesReturned, dwTapControllerState);
        else
          Status = FTC_INVALID_TAP_CONTROLLER_STATE;
      }
    }
    else
    {
      if (pWriteDataBuffer == NULL)
        Status = FTC_NULL_WRITE_DATA_BUFFER_POINTER;
      else
        Status = FTC_NULL_READ_DATA_BUFFER_POINTER;
    }
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GenerateTCKClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses)
//FTC_STATUS FT2232cMpsseJtag::JTAG_GenerateClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((dwNumClockPulses >= MIN_NUM_CLOCK_PULSES) && (dwNumClockPulses <= MAX_NUM_CLOCK_PULSES))
      Status = GenerateTCKClockPulses(ftHandle, dwNumClockPulses);
    else
      Status = FTC_INVALID_NUMBER_CLOCK_PULSES;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_ClearCommandSequence(void)
//FTC_STATUS FT2232cMpsseJtag::JTAG_ClearCmdSequence(void)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDevices;
  FT2232CDeviceIndexes FT2232CIndexes;

  Status = FTC_GetNumDevices(&dwNumDevices, &FT2232CIndexes);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumDevices == 1)
      ClearDeviceCommandSequenceData(0);
    else
      Status = FTC_TOO_MANY_DEVICES;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_AddWriteCommand(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
//FTC_STATUS FT2232cMpsseJtag::JTAG_AddWriteCmd(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                                  PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                  DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  //DWORD dwNumCommandDataBytes = 0;
  DWORD dwNumDevices;
  FT2232CDeviceIndexes FT2232CIndexes;

  Status = FTC_GetNumDevices(&dwNumDevices, &FT2232CIndexes);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumDevices == 1)
      // ftHandle parameter set to 0 to indicate only one device present in the system
      Status = AddDeviceWriteCommand(0, bInstructionTestData, dwNumBitsToWrite, pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);
    else
      Status = FTC_TOO_MANY_DEVICES;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_AddReadCommand(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
//FTC_STATUS FT2232cMpsseJtag::JTAG_AddReadCmd(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  //DWORD dwNumTmsClocks = 0;
  DWORD dwNumDevices;
  FT2232CDeviceIndexes FT2232CIndexes;

  Status = FTC_GetNumDevices(&dwNumDevices, &FT2232CIndexes);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumDevices == 1)
      // ftHandle parameter set to 0 to indicate only one device present in the system
      Status = AddDeviceReadCommand(0, bInstructionTestData, dwNumBitsToRead, dwTapControllerState);
    else
      Status = FTC_TOO_MANY_DEVICES;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_AddWriteReadCommand(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
//FTC_STATUS FT2232cMpsseJtag::JTAG_AddWriteReadCmd(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                      PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                      DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;
  //DWORD dwNumCommandDataBytes = 0;
  //DWORD dwNumTmsClocks = 0;
  DWORD dwNumDevices;
  FT2232CDeviceIndexes FT2232CIndexes;

  Status = FTC_GetNumDevices(&dwNumDevices, &FT2232CIndexes);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumDevices == 1)
      // ftHandle parameter set to 0 to indicate only one device present in the system
      Status = AddDeviceWriteReadCommand(0, bInstructionTestData, dwNumBitsToWriteRead, pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);
    else
      Status = FTC_TOO_MANY_DEVICES;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_ClearDeviceCommandSequence(FTC_HANDLE ftHandle)
//FTC_STATUS FT2232cMpsseJtag::JTAG_ClearDeviceCmdSequence(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
    ClearDeviceCommandSequenceData(ftHandle);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_AddDeviceWriteCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
//FTC_STATUS FT2232cMpsseJtag::JTAG_AddDeviceWriteCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                                        PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                        DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
    Status = AddDeviceWriteCommand(ftHandle, bInstructionTestData, dwNumBitsToWrite, pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_AddDeviceReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
//FTC_STATUS FT2232cMpsseJtag::JTAG_AddDeviceReadCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
    Status = AddDeviceReadCommand(ftHandle, bInstructionTestData, dwNumBitsToRead, dwTapControllerState);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_AddDeviceWriteReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
//FTC_STATUS FT2232cMpsseJtag::JTAG_AddDeviceWriteReadCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                            PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                                            DWORD dwTapControllerState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
    Status = AddDeviceWriteReadCommand(ftHandle, bInstructionTestData, dwNumBitsToWriteRead, pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_ExecuteCommandSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
//FTC_STATUS FT2232cMpsseJtag::JTAG_ExecuteCmdSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                                         LPDWORD lpdwNumBytesReturned)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumCmdSequenceBytes = 0;
  DWORD dwCmdSequenceByteIndex = 0;
  InputByteBuffer InputBuffer;
  DWORD dwTotalNumBytesToBeRead = 0;
  DWORD dwNumBytesRead = 0;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pReadCmdSequenceDataBuffer != NULL)
    {
      iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

      dwNumCmdSequenceBytes = GetNumBytesInCommandsSequenceDataBuffer();

      if (dwNumCmdSequenceBytes > 0)
      {
        AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);

        FTC_ClearOutputBuffer();

        dwNumCmdSequenceBytes = GetNumBytesInCommandsSequenceDataBuffer();
          
        // Transfer sequence of commands for specified device to output buffer for transmission to device
        for (dwCmdSequenceByteIndex = 0; (dwCmdSequenceByteIndex < dwNumCmdSequenceBytes); dwCmdSequenceByteIndex++)
          FTC_AddByteToOutputBuffer((*OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pCommandsSequenceDataOutPutBuffer)[dwCmdSequenceByteIndex], false);

        OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend = 0;

        Status = FTC_SendCommandsSequenceToDevice(ftHandle);

        if (Status == FTC_SUCCESS)
        {
          if (OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences > 0)
          {
            // Calculate the total number of bytes to be read, as a result of a command sequence
            dwTotalNumBytesToBeRead = GetTotalNumCommandsSequenceDataBytesToRead();

            Status = FTC_ReadCommandsSequenceBytesFromDevice(ftHandle, &InputBuffer, dwTotalNumBytesToBeRead, &dwNumBytesRead);
        
            if (Status == FTC_SUCCESS)
            {
              // Process all bytes received and return them in the read data buffer
              ProcessReadCommandsSequenceBytes(&InputBuffer, dwNumBytesRead, pReadCmdSequenceDataBuffer, lpdwNumBytesReturned);
            }
          }
        }

        OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences = 0;
      }
      else
        Status = FTC_NO_COMMAND_SEQUENCE;

      iCommandsSequenceDataDeviceIndex = -1;
    }
    else
      Status = FTC_NULL_READ_CMDS_DATA_BUFFER_POINTER;
  }

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if (lpDllVersionBuffer != NULL)
  {
    if (dwBufferSize > strlen(DLL_VERSION_NUM))
      strcpy(lpDllVersionBuffer, DLL_VERSION_NUM);
    else
      Status = FTC_DLL_VERSION_BUFFER_TOO_SMALL;
  }
  else
    Status = FTC_NULL_DLL_VERSION_BUFFER_POINTER;

  return Status;
}

FTC_STATUS FT2232cMpsseJtag::JTAG_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode,
                                                     LPSTR lpErrorMessageBuffer, DWORD dwBufferSize)
{
  FTC_STATUS Status = FTC_SUCCESS;
  CHAR szErrorMsg[MAX_ERROR_MSG_SIZE];
  INT iCharCntr = 0;

  if ((lpLanguage != NULL) && (lpErrorMessageBuffer != NULL))
  {
    for (iCharCntr = 0; (iCharCntr < MAX_ERROR_MSG_SIZE); iCharCntr++)
      szErrorMsg[iCharCntr] = '\0';

    if (((StatusCode >= FTC_SUCCESS) && (StatusCode <= FTC_INSUFFICIENT_RESOURCES)) ||
        ((StatusCode >= FTC_FAILED_TO_COMPLETE_COMMAND) && (StatusCode <= FTC_INVALID_STATUS_CODE)))
    {
      if (strcmp(lpLanguage, ENGLISH) == 0)
      {
        if ((StatusCode >= FTC_SUCCESS) && (StatusCode <= FTC_INSUFFICIENT_RESOURCES))
          strcpy(szErrorMsg, EN_Common_Errors[StatusCode]);
        else
          strcpy(szErrorMsg, EN_New_Errors[(StatusCode - FTC_FAILED_TO_COMPLETE_COMMAND)]);
      }
      else
      {
        strcpy(szErrorMsg, EN_New_Errors[FTC_INVALID_LANGUAGE_CODE - FTC_FAILED_TO_COMPLETE_COMMAND]);

        Status = FTC_INVALID_LANGUAGE_CODE;
      }
    }
    else
    {
      sprintf(szErrorMsg, "%s%d", EN_New_Errors[FTC_INVALID_STATUS_CODE - FTC_FAILED_TO_COMPLETE_COMMAND], (int)StatusCode);

      Status = FTC_INVALID_STATUS_CODE;
    }

    if (dwBufferSize > strlen(szErrorMsg))
      strcpy(lpErrorMessageBuffer, szErrorMsg);
    else
      Status = FTC_ERROR_MESSAGE_BUFFER_TOO_SMALL;
  }
  else
  {
    if (lpLanguage == NULL)
      Status = FTC_NULL_LANGUAGE_CODE_BUFFER_POINTER;
    else
      Status = FTC_NULL_ERROR_MESSAGE_BUFFER_POINTER;
  }

  return Status;
}
