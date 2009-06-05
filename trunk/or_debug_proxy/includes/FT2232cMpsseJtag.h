/*++

FTC MPSSE Interface DLLs - Copyright © FTDI Ltd. 2009


The source code to the FTCI2C, FTCJTAG and FTCSPI DLLs is provided as-is and no warranty is made as to its function or reliability.  

This source code may be freely modified and redistributed, provided that the FTDI Ltd. copyright notice remains intact.

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    ft2232cmpssejtag.h

Abstract:

    FT2232C Dual Device Device Class Declaration/Definition.

Environment:

    kernel & user mode

Revision History:

    07/02/05    kra           Created.
    24/08/05    kra           Added new function JTAG_GenerateClockPulses and new error code FTC_INVALID_NUMBER_CLOCK_PULSES
    16/09/05    kra           Version 1.50 - Added break statements after DLL_THREAD_ATTACH and DLL_THREAD_DETACH for multiple threaded applications
	08/03/06	ana		      Version 1.08 - fix byte boundry mising bit.
	19/11/08	Rene Baumann  Port FTCJTAG to Linux.
	
--*/

#ifndef FT2232cMpsseJtag_H
#define FT2232cMpsseJtag_H

//#include <windows.h>

#include "FT2232c.h"
//#include "FTCJTAG.h" -- All required bits now included in this file. WinAPI things in there not needed here


// Defines from FTCJTAG.h

typedef DWORD FTC_HANDLE;
typedef ULONG FTC_STATUS;

#define TEST_LOGIC_STATE 1
#define RUN_TEST_IDLE_STATE 2
#define PAUSE_TEST_DATA_REGISTER_STATE 3
#define PAUSE_INSTRUCTION_REGISTER_STATE 4
#define SHIFT_TEST_DATA_REGISTER_STATE 5
#define SHIFT_INSTRUCTION_REGISTER_STATE 6

#define FTC_SUCCESS 0 // FTC_OK
#define FTC_INVALID_HANDLE 1 // FTC_INVALID_HANDLE
#define FTC_DEVICE_NOT_FOUND 2 //FTC_DEVICE_NOT_FOUND
#define FTC_DEVICE_NOT_OPENED 3 //FTC_DEVICE_NOT_OPENED
#define FTC_IO_ERROR 4 //FTC_IO_ERROR
#define FTC_INSUFFICIENT_RESOURCES 5 // FTC_INSUFFICIENT_RESOURCES

#define FTC_FAILED_TO_COMPLETE_COMMAND 20          // cannot change, error code mapped from FT2232c classes
#define FTC_FAILED_TO_SYNCHRONIZE_DEVICE_MPSSE 21  // cannot change, error code mapped from FT2232c classes
#define FTC_INVALID_DEVICE_NAME_INDEX 22           // cannot change, error code mapped from FT2232c classes
#define FTC_NULL_DEVICE_NAME_BUFFER_POINTER 23     // cannot change, error code mapped from FT2232c classes 
#define FTC_DEVICE_NAME_BUFFER_TOO_SMALL 24        // cannot change, error code mapped from FT2232c classes
#define FTC_INVALID_DEVICE_NAME 25                 // cannot change, error code mapped from FT2232c classes
#define FTC_INVALID_LOCATION_ID 26                 // cannot change, error code mapped from FT2232c classes
#define FTC_DEVICE_IN_USE 27                       // cannot change, error code mapped from FT2232c classes
#define FTC_TOO_MANY_DEVICES 28                    // cannot change, error code mapped from FT2232c classes
#define FTC_INVALID_CLOCK_DIVISOR 29
#define FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER 30
#define FTC_INVALID_NUMBER_BITS 31
#define FTC_NULL_WRITE_DATA_BUFFER_POINTER 32
#define FTC_INVALID_NUMBER_BYTES 33
#define FTC_NUMBER_BYTES_TOO_SMALL 34
#define FTC_INVALID_TAP_CONTROLLER_STATE 35
#define FTC_NULL_READ_DATA_BUFFER_POINTER 36
#define FTC_COMMAND_SEQUENCE_BUFFER_FULL 37
#define FTC_NULL_READ_CMDS_DATA_BUFFER_POINTER 38
#define FTC_NO_COMMAND_SEQUENCE 39
#define FTC_INVALID_NUMBER_CLOCK_PULSES 40
#define FTC_NULL_DLL_VERSION_BUFFER_POINTER 41
#define FTC_DLL_VERSION_BUFFER_TOO_SMALL 42
#define FTC_NULL_LANGUAGE_CODE_BUFFER_POINTER 43
#define FTC_NULL_ERROR_MESSAGE_BUFFER_POINTER 44
#define FTC_ERROR_MESSAGE_BUFFER_TOO_SMALL 45
#define FTC_INVALID_LANGUAGE_CODE 46
#define FTC_INVALID_STATUS_CODE 47


typedef struct Ft_Input_Output_Pins{
  BOOL  bPin1InputOutputState;
  BOOL  bPin1LowHighState;
  BOOL  bPin2InputOutputState;
  BOOL  bPin2LowHighState;
  BOOL  bPin3InputOutputState;
  BOOL  bPin3LowHighState;
  BOOL  bPin4InputOutputState;
  BOOL  bPin4LowHighState;
}FTC_INPUT_OUTPUT_PINS, *PFTC_INPUT_OUTPUT_PINS;
typedef struct Ft_Low_High_Pins{
  BOOL  bPin1LowHighState;
  BOOL  bPin2LowHighState;
  BOOL  bPin3LowHighState;
  BOOL  bPin4LowHighState;
}FTC_LOW_HIGH_PINS, *PFTC_LOW_HIGH_PINS;
#define MAX_WRITE_DATA_BYTES_BUFFER_SIZE 65536    // 64k bytes

typedef BYTE WriteDataByteBuffer[MAX_WRITE_DATA_BYTES_BUFFER_SIZE];
typedef WriteDataByteBuffer *PWriteDataByteBuffer;


#define MAX_READ_DATA_BYTES_BUFFER_SIZE 65536    // 64k bytes

typedef BYTE ReadDataByteBuffer[MAX_READ_DATA_BYTES_BUFFER_SIZE];
typedef ReadDataByteBuffer *PReadDataByteBuffer;

#define MAX_READ_CMDS_DATA_BYTES_BUFFER_SIZE 131071  // 128K bytes 

typedef BYTE ReadCmdSequenceDataByteBuffer[MAX_READ_CMDS_DATA_BYTES_BUFFER_SIZE];
typedef ReadCmdSequenceDataByteBuffer *PReadCmdSequenceDataByteBuffer;


// end defines from FTCJTAG.h


#define DEVICE_CHANNEL_A " A"
#define DEVICE_CHANNEL_B " B"

#define DLL_VERSION_NUM "1.8"

#define USB_INPUT_BUFFER_SIZE 65536   // 64K
#define USB_OUTPUT_BUFFER_SIZE 65536  // 64K

const BYTE FT_EVENT_VALUE = 0;
const BYTE FT_ERROR_VALUE = 0;

#define DEVICE_READ_TIMEOUT_INFINITE 0
#define DEVICE_WRITE_TIMEOUT 5000 // 5 seconds

#define MIN_CLOCK_DIVISOR 0     // equivalent to 6MHz
#define MAX_CLOCK_DIVISOR 65535 // equivalent to 91Hz

#define MIN_NUM_BITS 2       // specifies the minimum number of bits that can be written or read to/from an external device
#define MAX_NUM_BITS 524280  // specifies the maximum number of bits that can be written or read to/from an external device
#define MIN_NUM_BYTES 1      // specifies the minimum number of bytes that can be written to an external device
#define MAX_NUM_BYTES 65535  // specifies the maximum number of bytes that can be written to an external device

#define NUMBITSINBYTE 8

#define MIN_NUM_CLOCK_PULSES 1          // specifies the minimum number of clock pulses that a FT2232C dual device can generate
#define MAX_NUM_CLOCK_PULSES 2000000000 // specifies the maximum number of clock pulses that a FT2232C dual device can generate

#define NUM_BYTE_CLOCK_PULSES_BLOCK_SIZE 32000 //4000

#define PIN1_HIGH_VALUE  1
#define PIN2_HIGH_VALUE  2
#define PIN3_HIGH_VALUE  4
#define PIN4_HIGH_VALUE  8

#define NUM_WRITE_COMMAND_BYTES 18
#define NUM_READ_COMMAND_BYTES 18
#define NUM_WRITE_READ_COMMAND_BYTES 19

#define MAX_ERROR_MSG_SIZE 250

const CHAR ENGLISH[3] = "EN";

const CHAR EN_Common_Errors[FTC_INSUFFICIENT_RESOURCES + 1][MAX_ERROR_MSG_SIZE] = {
    "",
    "Invalid device handle.",
    "Device not found.",
    "Device not opened.",
    "General device IO error.",
    "Insufficient resources available to execute function."};

const CHAR EN_New_Errors[(FTC_INVALID_STATUS_CODE - FTC_FAILED_TO_COMPLETE_COMMAND) + 1][MAX_ERROR_MSG_SIZE] = {
    "Failed to complete command.",
    "Failed to synchronize the device MPSSE interface.",
    "Invalid device name index.",
    "Pointer to device name buffer is null.",
    "Buffer to contain device name is too small.",
    "Invalid device name.",
    "Invalid device location identifier.",
    "Device already in use by another application.",
    "More than one device detected.",
    "Invalid clock divisor. Valid range is 0 - 65535.",
    "Pointer to input output buffer is null.",
    "Invalid number of bits. Valid range 2 to 524280. 524280 bits is equivalent to 64K bytes",
    "Pointer to write data buffer is null.",
    "Invalid size of write data buffer. Valid range is 1 - 65535",
    "Buffer to contain number of bits is too small.",
    "Invalid Test Access Port(TAP) controller state.",
    "Pointer to read data buffer is null.",
    "Command sequence buffer is full. Valid range is 1 - 131070 ie 128K bytes.",
    "Pointer to read command sequence data buffer is null.",
    "No command sequence found.",
    "Invalid number of clock pulses. Valid range is 1 - 2000,000,000.",
    "Pointer to dll version number buffer is null.",
    "Buffer to contain dll version number is too small.",
    "Pointer to language code buffer is null.",
    "Pointer to error message buffer is null.",
    "Buffer to contain error message is too small.",
    "Unsupported language code.",
    "Unknown status code = "};

const BYTE CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD = '\x19';
const BYTE CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD = '\x1B';
const BYTE CLK_DATA_BYTES_IN_ON_POS_CLK_LSB_FIRST_CMD = '\x28';
const BYTE CLK_DATA_BITS_IN_ON_POS_CLK_LSB_FIRST_CMD = '\x2A';
const BYTE CLK_DATA_BYTES_OUT_ON_NEG_CLK_IN_ON_POS_CLK_LSB_FIRST_CMD = '\x39';
const BYTE CLK_DATA_BITS_OUT_ON_NEG_CLK_IN_ON_POS_CLK_LSB_FIRST_CMD = '\x3B';

const BYTE CLK_DATA_TMS_NO_READ_CMD = '\x4B';
const BYTE CLK_DATA_TMS_READ_CMD = '\x6B';

const BYTE SET_LOW_BYTE_DATA_BITS_CMD = '\x80';
const BYTE GET_LOW_BYTE_DATA_BITS_CMD = '\x81';
const BYTE SET_HIGH_BYTE_DATA_BITS_CMD = '\x82';
const BYTE GET_HIGH_BYTE_DATA_BITS_CMD = '\x83';
const BYTE SET_CLOCK_FREQUENCY_CMD = '\x86';
const BYTE SEND_ANSWER_BACK_IMMEDIATELY_CMD = '\x87';

enum JtagStates {TestLogicReset, RunTestIdle, PauseDataRegister, PauseInstructionRegister, ShiftDataRegister, ShiftInstructionRegister, Undefined};

#define NUM_JTAG_TMS_STATES 6

// go from current JTAG state to new JTAG state ->                    tlr     rti     pdr     pir     sdr     sir
const BYTE TestLogicResetToNewJTAGState[NUM_JTAG_TMS_STATES]      = {'\x01', '\x00', '\x0A', '\x16', '\x02', '\x06'};
const BYTE RunTestIdleToNewJTAGState[NUM_JTAG_TMS_STATES]         = {'\x07', '\x00', '\x05', '\x0B', '\x01', '\x03'};
const BYTE PauseDataRegToNewJTAGState[NUM_JTAG_TMS_STATES]        = {'\x1F', '\x03', '\x17', '\x2F', '\x01', '\x0F'};
const BYTE PauseInstructionRegToNewJTAGState[NUM_JTAG_TMS_STATES] = {'\x1F', '\x03', '\x17', '\x2F', '\x07', '\x01'};
const BYTE ShiftDataRegToNewJTAGState[NUM_JTAG_TMS_STATES]        = {'\x1F', '\x03', '\x01', '\x2F', '\x00', '\x00'};
const BYTE ShiftInstructionRegToNewJTAGState[NUM_JTAG_TMS_STATES] = {'\x1F', '\x03', '\x17', '\x01', '\x00', '\x00'};

// number of TMS clocks to go from current JTAG state to new JTAG state ->       tlr rti pdr pir sdr sir
const BYTE TestLogicResetToNewJTAGStateNumTMSClocks[NUM_JTAG_TMS_STATES]      = {1,  1,  5,  6,  4,  5};
const BYTE RunTestIdleToNewJTAGStateNumTMSClocks[NUM_JTAG_TMS_STATES]         = {3,  5,  4,  5,  3,  4};
const BYTE PauseDataRegToNewJTAGStateNumTMSClocks[NUM_JTAG_TMS_STATES]        = {5,  3,  6,  7,  2,  6};
const BYTE PauseInstructionRegToNewJTAGStateNumTMSClocks[NUM_JTAG_TMS_STATES] = {5,  3,  6,  7,  5,  2};
const BYTE ShiftDataRegToNewJTAGStateNumTMSClocks[NUM_JTAG_TMS_STATES]        = {5,  3,  2,  7,  0,  0};
const BYTE ShiftInstructionRegToNewJTAGStateNumTMSClocks[NUM_JTAG_TMS_STATES] = {5,  4,  6,  2,  0,  0};

#define NO_LAST_DATA_BIT 0

#define NUM_COMMAND_SEQUENCE_READ_DATA_BYTES 2
#define INIT_COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE 100
#define COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE_INCREMENT 10

typedef DWORD ReadCommandSequenceData[NUM_COMMAND_SEQUENCE_READ_DATA_BYTES];
typedef ReadCommandSequenceData *PReadCommandSequenceData;

typedef PReadCommandSequenceData ReadCommandsSequenceData[1];
typedef ReadCommandsSequenceData *PReadCommandsSequenceData;

typedef struct Ft_Device_Cmd_Sequence_Data{
  DWORD hDevice;                                    // handle to the opened and initialized FT2232C dual type device
  DWORD dwNumBytesToSend;
  POutputByteBuffer pCommandsSequenceDataOutPutBuffer;
  DWORD dwSizeReadCommandsSequenceDataBuffer;
  PReadCommandsSequenceData pReadCommandsSequenceDataBuffer;
  DWORD dwNumReadCommandSequences;
}FTC_DEVICE_CMD_SEQUENCE_DATA, *PFTC_DEVICE_CMD_SEQUENCE_DATA;


static DWORD dwSavedLowPinsDirection = 0;												   // Removed from FT2232cMpsseJtag class to avoid segmentation fault. Rene 
static DWORD dwSavedLowPinsValue = 0;													   // Removed from FT2232cMpsseJtag class to avoid segmentation fault. Rene 
static JtagStates CurrentJtagState = Undefined;								           // Removed from FT2232cMpsseJtag class to avoid segmentation fault. Rene 
static DWORD dwNumOpenedDevices = 0; 													   // Removed from FT2232cMpsseJtag class to avoid segmentation fault. Rene 
static FTC_DEVICE_CMD_SEQUENCE_DATA OpenedDevicesCommandsSequenceData[MAX_NUM_DEVICES];  // Removed from FT2232cMpsseJtag class to avoid segmentation fault. Rene
static INT iCommandsSequenceDataDeviceIndex = -1;										   // Removed from FT2232cMpsseJtag class to avoid segmentation fault. Rene 


//----------------------------------------------------------------------------
class FT2232cMpsseJtag : private FT2232c
{
private:
  // Moved out from this class to avoid segmentation fault. --> DWORD dwSavedLowPinsDirection;	                                                  // Rene
  // Moved out from this class to avoid segmentation fault. --> DWORD dwSavedLowPinsValue;														  // Rene
  // Moved out from this class to avoid segmentation fault. --> JtagStates CurrentJtagState;													  // Rene
  // Moved out from this class to avoid segmentation fault. --> DWORD dwNumOpenedDevices;														  // Rene
  // Moved out from this class to avoid segmentation fault. --> FTC_DEVICE_CMD_SEQUENCE_DATA OpenedDevicesCommandsSequenceData[MAX_NUM_DEVICES];  // Rene
  // Moved out from this class to avoid segmentation fault. --> INT iCommandsSequenceDataDeviceIndex;											  // Rene

  FTC_STATUS CheckWriteDataToExternalDeviceBitsBytesParameters(DWORD dwNumBitsToWrite, DWORD dwNumBytesToWrite);

  void       AddByteToOutputBuffer(DWORD dwOutputByte, BOOL bClearOutputBuffer);
  FTC_STATUS SetDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
  FTC_STATUS InitDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
  void       SetJTAGToNewState(DWORD dwNewJtagState, DWORD dwNumTmsClocks, BOOL bDoReadOperation);
  DWORD      MoveJTAGFromOneStateToAnother(JtagStates NewJtagState, DWORD dwLastDataBit, BOOL bDoReadOperation);
  FTC_STATUS ResetTAPContollerExternalDeviceSetToTestIdleMode(FTC_HANDLE ftHandle);
  FTC_STATUS SetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                              DWORD dwLowPinsDirection, DWORD dwLowPinsValue,
                                              BOOL bControlHighInputOutputPins,
                                              DWORD dwHighPinsDirection, DWORD dwHighPinsValue);
  void       GetGeneralPurposeInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTC_LOW_HIGH_PINS pPinsInputData);
  FTC_STATUS GetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                              PFTC_LOW_HIGH_PINS pLowPinsInputData,
                                              BOOL bControlHighInputOutputPins,
                                              PFTC_LOW_HIGH_PINS pHighPinsInputData);
  void       AddWriteCommandDataToOutPutBuffer(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                               PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                               DWORD dwTapControllerState);
  FTC_STATUS WriteDataToExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionData, DWORD dwNumBitsToWrite,
                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                       DWORD dwTapControllerState);
  void       GetNumDataBytesToRead(DWORD dwNumBitsToRead, LPDWORD lpdwNumDataBytesToRead, LPDWORD lpdwNumRemainingDataBits);
  FTC_STATUS GetDataFromExternalDevice(FTC_HANDLE ftHandle, DWORD dwNumBitsToRead, DWORD dwNumTmsClocks,
                                       PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned);
  DWORD      AddReadCommandToOutputBuffer(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
  FTC_STATUS ReadDataFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
                                        PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                        DWORD dwTapControllerState);
  DWORD      AddWriteReadCommandDataToOutPutBuffer(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                                   PWriteDataByteBuffer pWriteDataBuffer,
                                                   DWORD dwNumBytesToWrite, DWORD dwTapControllerState);
  FTC_STATUS WriteReadDataToFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                               PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                               PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                               DWORD dwTapControllerState);
  FTC_STATUS GenerateTCKClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses);

  void       ProcessReadCommandsSequenceBytes(PInputByteBuffer pInputBuffer, DWORD dwNumBytesRead, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                              LPDWORD lpdwNumBytesReturned);
  DWORD      GetTotalNumCommandsSequenceDataBytesToRead (void);
  void       CopyReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pDestinationBuffer, PReadCommandsSequenceData pSourceBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer);
  FTC_STATUS AddReadCommandSequenceData(DWORD dwNumBitsToRead, DWORD dwNumTmsClocks);
  void       CreateReadCommandsSequenceDataBuffer(void);
  PReadCommandsSequenceData CreateReadCommandsSequenceDataBuffer(DWORD dwSizeReadCmdsSequenceDataBuffer);
  void       DeleteReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pReadCmdsSequenceDataBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer);

  FTC_STATUS CreateDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle);
  void       ClearDeviceCommandSequenceData(FTC_HANDLE ftHandle);
  DWORD      GetNumBytesInCommandsSequenceDataBuffer(void);
  DWORD      GetCommandsSequenceDataDeviceIndex(FTC_HANDLE ftHandle);
  void       DeleteDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle);

  FTC_STATUS AddDeviceWriteCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                   PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                   DWORD dwTapControllerState);
  FTC_STATUS AddDeviceReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
  FTC_STATUS AddDeviceWriteReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                       DWORD dwTapControllerState);

public:
  FT2232cMpsseJtag(void);
  ~FT2232cMpsseJtag(void);


  FTC_STATUS JTAG_GetNumDevices(LPDWORD lpdwNumDevices);
  FTC_STATUS JTAG_GetDeviceNameLocationID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID);
  //FTC_STATUS JTAG_GetDeviceNameLocID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID);
  FTC_STATUS JTAG_OpenSpecifiedDevice(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle);
  //FTC_STATUS JTAG_OpenEx(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle);
  FTC_STATUS JTAG_OpenDevice(FTC_HANDLE *pftHandle);
  //FTC_STATUS JTAG_Open(FTC_HANDLE *pftHandle);
  FTC_STATUS JTAG_CloseDevice(FTC_HANDLE ftHandle);
  //FTC_STATUS JTAG_Close(FTC_HANDLE ftHandle);
  FTC_STATUS JTAG_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockFrequencyValue);
  FTC_STATUS JTAG_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
  FTC_STATUS JTAG_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
  FTC_STATUS JTAG_SetDeviceLoopbackState(FTC_HANDLE ftHandle, BOOL bLoopbackState);
  //FTC_STATUS JTAG_SetLoopback(FTC_HANDLE ftHandle, BOOL bLoopbackState);
  FTC_STATUS JTAG_SetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
							  //FTC_STATUS JTAG_SetGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
				  PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,
				  BOOL bControlHighInputOutputPins,
				  PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData);
  FTC_STATUS JTAG_GetGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
							  //FTC_STATUS JTAG_GetGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
				  PFTC_LOW_HIGH_PINS pLowPinsInputData,
				  BOOL bControlHighInputOutputPins,
				  PFTC_LOW_HIGH_PINS pHighPinsInputData);
  FTC_STATUS JTAG_WriteDataToExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
						   //FTC_STATUS JTAG_Write(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
			       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
			       DWORD dwTapControllerState);
  FTC_STATUS JTAG_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
						    //FTC_STATUS JTAG_Read(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
			      PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
			      DWORD dwTapControllerState);
  FTC_STATUS JTAG_WriteReadDataToFromExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
							   //FTC_STATUS JTAG_WriteRead(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
				   PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
				   PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
				   DWORD dwTapControllerState);
  FTC_STATUS JTAG_GenerateTCKClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses);
  //FTC_STATUS JTAG_GenerateClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses);
  FTC_STATUS JTAG_ClearCommandSequence(void);
  //FTC_STATUS JTAG_ClearCmdSequence(void);
  FTC_STATUS JTAG_AddWriteCommand(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
					 //FTC_STATUS JTAG_AddWriteCmd(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
				     PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
				     DWORD dwTapControllerState);
  FTC_STATUS JTAG_AddReadCommand(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
  //FTC_STATUS JTAG_AddReadCmd(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
  FTC_STATUS JTAG_AddWriteReadCommand(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
					     //FTC_STATUS JTAG_AddWriteReadCmd(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
					 PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
					 DWORD dwTapControllerState);
  FTC_STATUS JTAG_ClearDeviceCommandSequence(FTC_HANDLE ftHandle);
  //FTC_STATUS JTAG_ClearDeviceCmdSequence(FTC_HANDLE ftHandle);
  FTC_STATUS JTAG_AddDeviceWriteCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
					       //FTC_STATUS JTAG_AddDeviceWriteCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
					   PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
					   DWORD dwTapControllerState);
  FTC_STATUS JTAG_AddDeviceReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
  //FTC_STATUS JTAG_AddDeviceReadCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
  FTC_STATUS JTAG_AddDeviceWriteReadCommand(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
						   //FTC_STATUS JTAG_AddDeviceWriteReadCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
					       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
					       DWORD dwTapControllerState);
  FTC_STATUS JTAG_ExecuteCommandSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
						//FTC_STATUS JTAG_ExecuteCmdSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
					    LPDWORD lpdwNumBytesReturned);
  FTC_STATUS JTAG_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize);
  FTC_STATUS JTAG_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode,
                                            LPSTR lpErrorMessageBuffer, DWORD dwBufferSize);
};

#endif  /* FT2232cMpsseJtag_H */
