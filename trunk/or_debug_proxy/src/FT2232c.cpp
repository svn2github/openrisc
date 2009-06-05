/*++

FTC MPSSE Interface DLLs - Copyright © FTDI Ltd. 2009


The source code to the FTCI2C, FTCJTAG and FTCSPI DLLs is provided as-is and no warranty is made as to its function or reliability.  

This source code may be freely modified and redistributed, provided that the FTDI Ltd. copyright notice remains intact.

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    FT2232c.cpp

Abstract:

    FT2232C Dual Type Devices Base Class Implementation.

Environment:

    kernel & user mode

Revision History:

    07/02/05    kra           Created.
    25/08/05    kra           Windows 2000 Professional always sets the USB buffer size to 4K ie 4096
	 19/11/08	 Rene Baumann  Port FTCJTAG to Linux.
	
--*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <linux/unistd.h> // -- Possibly not needed, trying without -jb
#include <sched.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <string.h>

#include <ctype.h>
#include <time.h>


#include "FT2232c.h"
#include "WinTypes.h"


//couple of simple string functions
  char* strupr(char *string);
  char* strlwr(char *string);

  static UINT uiNumOpenedDevices = 0;                     // Removed from class FT2232c in FT2232c.h to avoid segmentation fault. Rene
  static FTC_DEVICE_DATA OpenedDevices[MAX_NUM_DEVICES];  // Removed from class FT2232c in FT2232c.h to avoid segmentation fault. Rene
  static OutputByteBuffer OutputBuffer;                   // Removed from class FT2232c in FT2232c.h to avoid segmentation fault. Rene
  static DWORD dwNumBytesToSend = 0;                      // Removed from class FT2232c in FT2232c.h to avoid segmentation fault. Rene
					

BOOLEAN FT2232c::FTC_DeviceInUse(LPSTR lpDeviceName, DWORD dwLocationID)
{
  BOOLEAN bDeviceInUse = false;
  DWORD dwProcessId = 0;
  BOOLEAN bLocationIDFound = false;
  INT iDeviceCntr = 0;

  if (uiNumOpenedDevices > 0)
  {
    //dwProcessId = GetCurrentProcessId();
    dwProcessId = getpid(); //Changed windows call to linux\s getpid()

    for (iDeviceCntr = 0; ((iDeviceCntr < MAX_NUM_DEVICES) && !bLocationIDFound); iDeviceCntr++)
    {
      // Only check device name and location id not the current application
      if (OpenedDevices[iDeviceCntr].dwProcessId != dwProcessId)
      {
        if (strcmp(OpenedDevices[iDeviceCntr].szDeviceName, lpDeviceName) == 0)
        {
          if (OpenedDevices[iDeviceCntr].dwLocationID == dwLocationID)
            bLocationIDFound = true;
        }
      }
    }

    if (bLocationIDFound)
      bDeviceInUse = true;
  }

  return bDeviceInUse;
}

BOOLEAN FT2232c::FTC_DeviceOpened(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
  BOOLEAN bDeviceOpen = false;
  DWORD dwProcessId = 0;
  BOOLEAN bLocationIDFound = false;
  INT iDeviceCntr = 0;

  if (uiNumOpenedDevices > 0)
  {
    //dwProcessId = GetCurrentProcessId();
    dwProcessId = getpid(); //Changed windows call to linux one

    for (iDeviceCntr = 0; ((iDeviceCntr < MAX_NUM_DEVICES) && !bLocationIDFound); iDeviceCntr++)
    {
      if (OpenedDevices[iDeviceCntr].dwProcessId == dwProcessId)
      {
        if (strcmp(OpenedDevices[iDeviceCntr].szDeviceName, lpDeviceName) == 0)
        {
          if (OpenedDevices[iDeviceCntr].dwLocationID == dwLocationID)
          {
            // Device has already been opened by this application, so just return the handle to the device
            *pftHandle = OpenedDevices[iDeviceCntr].hDevice;
            bLocationIDFound = true;
          }
        }
      }
    }

    if (bLocationIDFound)
      bDeviceOpen = true;
  }

  return bDeviceOpen;
}

FTC_STATUS FT2232c::FTC_IsDeviceNameLocationIDValid(LPSTR lpDeviceName, DWORD dwLocationID)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumOfDevices = 0;
  FT2232CDeviceIndexes FT2232CIndexes;
  DWORD dwFlags = 0;
  DWORD dwDeviceType = 0;
  DWORD dwProductVendorID = 0;
  DWORD dwLocID = 0;
  SerialNumber szSerialNumber;
  char szDeviceNameBuffer[DEVICE_STRING_BUFF_SIZE + 1];
  FT_HANDLE ftHandle;
  BOOL bDeviceNameFound = false;
  DWORD dwDeviceIndex = 0;

  // Get the number of devices connected to the system
  Status = FTC_GetNumDevices(&dwNumOfDevices, &FT2232CIndexes);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumOfDevices > 0)
    {
      do
      {
        Status = FT_GetDeviceInfoDetail(FT2232CIndexes[dwDeviceIndex], &dwFlags, &dwDeviceType, &dwProductVendorID,
                                        &dwLocID, szSerialNumber, szDeviceNameBuffer, &ftHandle);

        if (Status == FTC_SUCCESS)
        {
          if (strcmp(szDeviceNameBuffer, lpDeviceName) == 0)
            bDeviceNameFound = true;
        }
        dwDeviceIndex++;
      }
      while ((dwDeviceIndex < dwNumOfDevices) && (Status == FTC_SUCCESS) && (bDeviceNameFound == false));

      if (bDeviceNameFound == TRUE)
      {
        if (dwLocID != dwLocationID)
          Status = FTC_INVALID_LOCATION_ID;
      }
      else
        Status = FTC_INVALID_DEVICE_NAME;
    }
    else
      Status = FTC_DEVICE_NOT_FOUND;
  }

  return Status;
}

FTC_STATUS FT2232c::FTC_IsDeviceFT2232CType(LPSTR lpDeviceName, LPBOOL lpbFT2232CTypeDevice)
{
  FTC_STATUS Status = FTC_SUCCESS;
  LPSTR pszStringSearch;

  *lpbFT2232CTypeDevice = false;

  // Search for the last occurrence of the channel string ie ' A'
  if ((pszStringSearch = strstr(strupr(lpDeviceName), DEVICE_CHANNEL_A)) != NULL)
  {
    // Ensure the last two characters of the device name is ' A' ie channel A
    if (strlen(pszStringSearch) == 2)
    *lpbFT2232CTypeDevice = true; 
  }

  return Status;
}

FT2232c::FT2232c(void)
{
  INT iDeviceCntr = 0;

  uiNumOpenedDevices = 0;

  for (iDeviceCntr = 0; (iDeviceCntr < MAX_NUM_DEVICES); iDeviceCntr++)
    OpenedDevices[iDeviceCntr].dwProcessId = 0;

  dwNumBytesToSend = 0;
}

FT2232c::~FT2232c(void)
{

}

FTC_STATUS FT2232c::FTC_GetNumDevices(LPDWORD lpdwNumDevices, FT2232CDeviceIndexes *FT2232CIndexes)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumOfDevices = 0;
  DWORD dwDeviceIndex = 0;
  DWORD dwFlags = 0;
  DWORD dwDeviceType = 0;
  DWORD dwProductVendorID = 0;
  SerialNumber szSerialNumber;
  char szDeviceNameBuffer[DEVICE_STRING_BUFF_SIZE + 1];
  FT_HANDLE ftHandle;
  DWORD dwLocationID;
  DWORD i;
  BOOL bFT2232CTypeDevice = false;

  *lpdwNumDevices = 0;

  // Get the number of devices connected to the system
  Status = FT_CreateDeviceInfoList(&dwNumOfDevices);

if(DEBUG_LIST_DEVICE){	  
	FT_STATUS ftStatus;
	FT_DEVICE_LIST_INFO_NODE *devInfo;
	DWORD numDevs;
	// create the device information list
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK) {
		    printf("Number of devices is %ld\n",numDevs);
	}
	if (numDevs > 0) {
		    // allocate storage for list based on numDevs
		    devInfo =
	(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
		    // get the device information list
		    ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
		    if (ftStatus == FT_OK) {
		           for ( i = 0; i < numDevs; i++) {
		                  printf("Dev %ld:\n",i);
		                  printf(" Flags=0x%lx\n",devInfo[i].Flags);
		                  printf(" Type=0x%lx\n",devInfo[i].Type);
		                  printf(" ID=0x%lx\n",devInfo[i].ID);
		                  printf(" LocId=0x%lx\n",devInfo[i].LocId);
		                  printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
		                  printf(" Description=%s\n",devInfo[i].Description);
		                  printf(" ftHandle=0x%lx\n",(unsigned long int)devInfo[i].ftHandle);
		           }
		    }
	}
}





  if (Status == FTC_SUCCESS)
  {
    if (dwNumOfDevices > 0)
    {
      do
      {
        // Devices previously opened by another application, results in 0 being returned for dwflags, dwDeviceType,
        // dwProductVendorID, dwLocationID and ftHandle and empty being returned in szSerialNumber and
        // szDeviceNameBuffer. This problem is in the FTD2XX.DLL.
        Status = FT_GetDeviceInfoDetail(dwDeviceIndex, &dwFlags, &dwDeviceType, &dwProductVendorID,
                                        &dwLocationID, szSerialNumber, szDeviceNameBuffer, &ftHandle);
        // The flag value is a 4-byte bit map containing miscellaneous data as defined in the - Type Definitions. 
        // Bit 0 (least significant bit) of this number indicates if the port is open (1) or closed (0). 
        // Bit 1 indicates if the device is enumerated as a high-speed USB device (2) or a full-speed USB device (0). 
        // The remaining bits (2 - 31) are reserved.
        
        // dwDeviceType = 4 --> FT_DEVICE_2232C see FT_DEVICE in ftd2xx.h
        if (Status == FTC_SUCCESS)
        {
          Status = FTC_IsDeviceFT2232CType(szDeviceNameBuffer, &bFT2232CTypeDevice);

          if (Status == FTC_SUCCESS)
          {
            if (bFT2232CTypeDevice == TRUE)
            {
              // The number of devices returned is, not opened devices ie channel A plus devices opened by the
              // calling application. Devices previously opened by another application are not included in this
              // number.
              (*FT2232CIndexes)[*lpdwNumDevices] = dwDeviceIndex;

              *lpdwNumDevices = *lpdwNumDevices + 1;
            }
          }
        }
        else
        {
          // if a device has already been opened by another application, the FT_GetDeviceInfoDetail call will
          // return an INVALID_HANDLE error code, ignore this error code and continue
          if (Status == FTC_INVALID_HANDLE)
            Status = FTC_SUCCESS;
        }

        dwDeviceIndex++;
      }
      while ((dwDeviceIndex < dwNumOfDevices) && (Status == FTC_SUCCESS));
    }
  }

  return Status;
}

FTC_STATUS FT2232c::FTC_GetNumNotOpenedDevices(LPDWORD lpdwNumNotOpenedDevices, FT2232CDeviceIndexes *FT2232CIndexes)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumOfDevices = 0;
  DWORD dwDeviceIndex = 0;
  DWORD dwFlags = 0;
  DWORD dwDeviceType = 0;
  DWORD dwProductVendorID = 0;
  SerialNumber szSerialNumber;
  char szDeviceNameBuffer[DEVICE_STRING_BUFF_SIZE + 1];
  FT_HANDLE ftHandle;
  DWORD dwLocationID;
  BOOL bFT2232CTypeDevice = false;

  *lpdwNumNotOpenedDevices = 0;

  // Get the number of devices connected to the system
  Status = FT_CreateDeviceInfoList(&dwNumOfDevices);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumOfDevices > 0)
    {
      do
      {
        Status = FT_GetDeviceInfoDetail(dwDeviceIndex, &dwFlags, &dwDeviceType, &dwProductVendorID,
                                        &dwLocationID, szSerialNumber, szDeviceNameBuffer, &ftHandle);

        if (Status == FTC_SUCCESS)
        {
          Status = FTC_IsDeviceFT2232CType(szDeviceNameBuffer, &bFT2232CTypeDevice);

          if (Status == FTC_SUCCESS)
          {
            if ((bFT2232CTypeDevice == TRUE) && ((dwFlags & DEVICE_OPENED_FLAG) == 0))
            {
              (*FT2232CIndexes)[*lpdwNumNotOpenedDevices] = dwDeviceIndex;

              *lpdwNumNotOpenedDevices = *lpdwNumNotOpenedDevices + 1;
            }
          }
        }
        else
        {
          // if a device has already been opened by another application, the FT_GetDeviceInfoDetail call will
          // return an INVALID_HANDLE error code, ignore this error code and continue
          if (Status == FTC_INVALID_HANDLE)
            Status = FTC_SUCCESS;
        }

        dwDeviceIndex++;
      }
      while ((dwDeviceIndex < dwNumOfDevices) && (Status == FTC_SUCCESS));
    }
  }

  return Status;
}

FTC_STATUS FT2232c::FTC_GetDeviceNameLocationID(DWORD dwDeviceIndex, LPSTR lpDeviceName, DWORD dwBufferSize, LPDWORD lpdwLocationID)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDevices = 0;
  FT2232CDeviceIndexes FT2232CIndexes;
  DWORD dwFlags = 0;
  DWORD dwDeviceType = 0;
  DWORD dwProductVendorID = 0;
  SerialNumber szSerialNumber;
  char szDeviceNameBuffer[DEVICE_STRING_BUFF_SIZE + 1];
  FT_HANDLE ftHandle;

  if (lpDeviceName != NULL)
  {
    Status = FTC_GetNumDevices(&dwNumDevices, &FT2232CIndexes);

    if (Status == FTC_SUCCESS)
    {
      if (dwNumDevices > 0)
      {
        if (dwDeviceIndex < dwNumDevices)
        {
          Status = FT_GetDeviceInfoDetail(FT2232CIndexes[dwDeviceIndex], &dwFlags, &dwDeviceType, &dwProductVendorID,
                                         lpdwLocationID, szSerialNumber, szDeviceNameBuffer, &ftHandle);

          if (Status == FTC_SUCCESS)
          {
            if (strlen(szDeviceNameBuffer) <= dwBufferSize)
              strcpy(lpDeviceName, szDeviceNameBuffer);
            else
              Status = FTC_DEVICE_NAME_BUFFER_TOO_SMALL;
          }
        }
        else
          Status = FTC_INVALID_DEVICE_NAME_INDEX;
      }
      else
        Status = FTC_DEVICE_NOT_FOUND;
    }
  }
  else
    Status = FTC_NULL_DEVICE_NAME_BUFFER_POINTER;

  return Status;
}

FTC_STATUS FT2232c::FTC_OpenSpecifiedDevice(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FT_HANDLE ftHandle;

  if (lpDeviceName != NULL)
  {
    
    // Caused driver to remove both devices loaded by ftdi_sio in /dev/
    // We wish the second, /dev/ttyUSB1 to remain to connect a terminal 
    // program to it. - jb 090301
    //Status = FTC_IsDeviceNameLocationIDValid(lpDeviceName, dwLocationID);

    if (Status == FTC_SUCCESS)
    {

      if (!FTC_DeviceInUse(lpDeviceName, dwLocationID))
      {

        if (!FTC_DeviceOpened(lpDeviceName, dwLocationID, pftHandle))
        {

          // To open a device in Linux the FT_OPEN_BY_DESCRIPTION	has to be used. Rene	 
          Status = FT_OpenEx((PVOID)lpDeviceName, FT_OPEN_BY_DESCRIPTION, &ftHandle);
          // Linux das not return the LocationID
          // Status = FT_OpenEx((PVOID)dwLocationID, FT_OPEN_BY_LOCATION, &ftHandle);

          if (Status == FTC_SUCCESS)
          {
            *pftHandle = (DWORD)ftHandle;
	    
            FTC_InsertDeviceHandle(lpDeviceName, dwLocationID, *pftHandle);
          }

        }

      }
      else
        Status = FTC_DEVICE_IN_USE;
    }
  }
  else
    Status = FTC_NULL_DEVICE_NAME_BUFFER_POINTER;

  return Status;
}

FTC_STATUS FT2232c::FTC_OpenDevice(FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDevices = 0;
  FT2232CDeviceIndexes FT2232CIndexes;
  char szDeviceName[DEVICE_STRING_BUFF_SIZE + 1];
  DWORD dwLocationID;

  Status = FTC_GetNumDevices(&dwNumDevices, &FT2232CIndexes);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumDevices == 1)
    {
      Status = FTC_GetDeviceNameLocationID(FT2232CIndexes[0], szDeviceName, (DEVICE_STRING_BUFF_SIZE + 1), &dwLocationID);

      if (Status == FTC_SUCCESS)
        Status = FTC_OpenSpecifiedDevice(szDeviceName, dwLocationID, pftHandle);
    }
    else
    {
      if (dwNumDevices == 0)
        Status = FTC_DEVICE_NOT_FOUND;
      else
        Status = FTC_TOO_MANY_DEVICES;
    }
  }

  return Status;
}

FTC_STATUS FT2232c::FTC_CloseDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    Status = FT_Close((FT_HANDLE)ftHandle);

    FTC_RemoveDeviceHandle(ftHandle);
  }

  return Status;
}

void FT2232c::FTC_GetClockFrequencyValues(DWORD dwClockFrequencyValue, LPDWORD lpdwClockFrequencyHz)
{
  *lpdwClockFrequencyHz = (BASE_CLOCK_FREQUENCY_12_MHZ / ((1 + dwClockFrequencyValue) * 2));
}

FTC_STATUS FT2232c::FTC_SetDeviceLoopbackState(FTC_HANDLE ftHandle, BOOL bLoopbackState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (bLoopbackState == false)
      // turn off loopback
      FTC_AddByteToOutputBuffer(TURN_OFF_LOOPBACK_CMD, true);
    else
      // turn on loopback
      FTC_AddByteToOutputBuffer(TURN_ON_LOOPBACK_CMD, true);

    FTC_SendBytesToDevice(ftHandle);
  }

  return Status;
}

void FT2232c::FTC_InsertDeviceHandle(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE ftHandle)
{
  DWORD dwProcessId = 0;
  INT iDeviceCntr = 0;
  BOOLEAN bDeviceftHandleInserted = false;

  if (uiNumOpenedDevices < MAX_NUM_DEVICES)
  {
    //dwProcessId = GetCurrentProcessId();
    dwProcessId = getpid(); //Changed windows call to linux\s getpid()

    for (iDeviceCntr = 0; ((iDeviceCntr < MAX_NUM_DEVICES) && !bDeviceftHandleInserted); iDeviceCntr++)
    {
      if (OpenedDevices[iDeviceCntr].dwProcessId == 0)
      {
        OpenedDevices[iDeviceCntr].dwProcessId = dwProcessId;
        strcpy(OpenedDevices[iDeviceCntr].szDeviceName, lpDeviceName);
        OpenedDevices[iDeviceCntr].dwLocationID = dwLocationID;
        OpenedDevices[iDeviceCntr].hDevice = ftHandle;

        uiNumOpenedDevices = uiNumOpenedDevices + 1;

        bDeviceftHandleInserted = true;
      }
    }
  }
}

FTC_STATUS FT2232c::FTC_IsDeviceHandleValid(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwProcessId = 0;
  INT iDeviceCntr = 0;
  BOOLEAN bDevicempHandleFound = false;

  if ((uiNumOpenedDevices > 0) && (ftHandle > 0))
  {
    //dwProcessId = GetCurrentProcessId();
    dwProcessId = getpid(); //Changed windows call to linux\s getpid()

    for (iDeviceCntr = 0; ((iDeviceCntr < MAX_NUM_DEVICES) && !bDevicempHandleFound); iDeviceCntr++)
    {
      if (OpenedDevices[iDeviceCntr].dwProcessId == dwProcessId)
      {
        if (OpenedDevices[iDeviceCntr].hDevice == ftHandle)
          bDevicempHandleFound = true;
      }
    }

    if (!bDevicempHandleFound)
      Status = FTC_INVALID_HANDLE;
  }
  else
    Status = FTC_INVALID_HANDLE;

  return Status;
}


void FT2232c::FTC_RemoveDeviceHandle(FTC_HANDLE ftHandle)
{
  DWORD dwProcessId = 0;
  INT iDeviceCntr = 0;
  BOOLEAN bDevicempHandleFound = false;

  if (uiNumOpenedDevices > 0)
  {
    //dwProcessId = GetCurrentProcessId();
    dwProcessId = getpid(); //Changed windows call to linux\s getpid()

    for (iDeviceCntr = 0; ((iDeviceCntr < MAX_NUM_DEVICES) && !bDevicempHandleFound); iDeviceCntr++)
    {
      if (OpenedDevices[iDeviceCntr].dwProcessId == dwProcessId)
      {
        if (OpenedDevices[iDeviceCntr].hDevice == ftHandle)
        {
          OpenedDevices[iDeviceCntr].dwProcessId = 0;
          strcpy(OpenedDevices[iDeviceCntr].szDeviceName, "");
          OpenedDevices[iDeviceCntr].dwLocationID = 0;
          OpenedDevices[iDeviceCntr].hDevice = 0;

          uiNumOpenedDevices = uiNumOpenedDevices - 1;

          bDevicempHandleFound = true;
        }
      }
    }
  }
}

FTC_STATUS FT2232c::FTC_ResetUSBDevicePurgeUSBInputBuffer(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;

  Status = FT_ResetDevice((FT_HANDLE)ftHandle);

  if (Status == FTC_SUCCESS)
  {
    // Get the number of bytes in the device input buffer
    Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

    if (Status == FTC_SUCCESS)
    {
      if (dwNumBytesDeviceInputBuffer > 0)
        FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);
    }
  }

  return Status;
}

FTC_STATUS FT2232c::FTC_SetDeviceUSBBufferSizes(FTC_HANDLE ftHandle, DWORD InputBufferSize, DWORD OutputBufferSize)
{
  return FT_SetUSBParameters((FT_HANDLE)ftHandle, InputBufferSize, OutputBufferSize);
}

FTC_STATUS FT2232c::FTC_SetDeviceSpecialCharacters(FTC_HANDLE ftHandle, BOOLEAN bEventEnabled, UCHAR EventCharacter,
                                                   BOOLEAN bErrorEnabled, UCHAR ErrorCharacter)
{
 	UCHAR EventCharEnabled = UCHAR(bEventEnabled);
	UCHAR ErrorCharEnabled = UCHAR(bErrorEnabled);

  // Set the special characters for the device. disable event and error characters
  return FT_SetChars((FT_HANDLE)ftHandle, EventCharacter, EventCharEnabled, ErrorCharacter, ErrorCharEnabled);
}

FTC_STATUS FT2232c::FTC_SetReadWriteDeviceTimeouts(FTC_HANDLE ftHandle, DWORD dwReadTimeoutmSec, DWORD dwWriteTimeoutmSec)
{
  // Sets the read and write timeouts in milli-seconds for the device
  return FT_SetTimeouts((FT_HANDLE)ftHandle, dwReadTimeoutmSec, dwWriteTimeoutmSec);
}

FTC_STATUS FT2232c::FTC_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE LatencyTimermSec)
{
  // Set the device latency timer to a number of milliseconds
  return FT_SetLatencyTimer((FT_HANDLE)ftHandle, LatencyTimermSec);
}

FTC_STATUS FT2232c::FTC_ResetMPSSEInterface(FTC_HANDLE ftHandle)
{
  return FT_SetBitMode((FT_HANDLE)ftHandle, MPSSE_INTERFACE_MASK, RESET_MPSSE_INTERFACE);
}

FTC_STATUS FT2232c::FTC_EnableMPSSEInterface(FTC_HANDLE ftHandle)
{
  return FT_SetBitMode((FT_HANDLE)ftHandle, MPSSE_INTERFACE_MASK, ENABLE_MPSSE_INTERFACE);
}

FTC_STATUS FT2232c::FTC_SendReceiveCommandFromMPSSEInterface(FTC_HANDLE ftHandle, BOOLEAN bSendEchoCommandContinuouslyOnce, BYTE EchoCommand, LPBOOL lpbCommandEchod)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  //SYSTEMTIME StartTime;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwByteCntr = 0;
  BOOL bBadCommandResponse = false;
  
  // New timeout detection variables for linux build -- Julius
  struct timeval tv;
  time_t time_start; // to store number of seconds since unix epoch


  *lpbCommandEchod = false;

  if (!bSendEchoCommandContinuouslyOnce)
  {
    // Causes the device to echo back the command character and wait in command mode
    FTC_AddByteToOutputBuffer(EchoCommand, true);
    FTC_SendBytesToDevice(ftHandle);
  }

  // Windows only time functions - removed for linux library build -- Julius
  //GetLocalTime(&StartTime);
  gettimeofday(&tv, NULL);
  time_start = tv.tv_sec;
  
  do
  {
    // Send the echo command every time round the loop
    if (bSendEchoCommandContinuouslyOnce)
    {
      // Causes the device to echo back the command character and wait in command mode
      FTC_AddByteToOutputBuffer(EchoCommand, true);
      FTC_SendBytesToDevice(ftHandle);
    }

    // Get the number of bytes in the device input buffer
    Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

    if (Status == FTC_SUCCESS)
    {
      //Sleep(0);  // give up timeslice - doesn't work this way in linux, instead do a do a sched_yield():
      sched_yield();
      // This basically checks if the time we took from the first GetLocalTime was bigger than
      // MAX_COMMAND_TIMEOUT_PERIOD
      //if (FTC_Timeout(StartTime, MAX_COMMAND_TIMEOUT_PERIOD))
      gettimeofday(&tv, NULL);
      if ((tv.tv_sec - time_start) > (MAX_COMMAND_TIMEOUT_PERIOD/1000))	
	Status = FTC_FAILED_TO_COMPLETE_COMMAND;
	
      
      
    }

    if (Status == FTC_SUCCESS)
    {
      if (dwNumBytesDeviceInputBuffer > 0)
      {
        FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

        if (dwNumBytesRead > 0)
        {
    		  dwByteCntr = 0;

          do
          {
		        if (dwByteCntr <= (dwNumBytesRead - 1))
            {
              if (InputBuffer[dwByteCntr] == BAD_COMMAND_RESPONSE)
                bBadCommandResponse = true;
              else
              {
                if (bBadCommandResponse == TRUE)
                {
                  if (InputBuffer[dwByteCntr] == EchoCommand)
                    *lpbCommandEchod = true;
                }

                bBadCommandResponse = false;
              }
            }

            dwByteCntr = dwByteCntr + 1;
          }
          while ((dwByteCntr < dwNumBytesRead) && (*lpbCommandEchod == false));
        }
      }
    }
  }
  while ((*lpbCommandEchod == false) && (Status == FTC_SUCCESS));

  return Status;
}

FTC_STATUS FT2232c::FTC_SynchronizeMPSSEInterface(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  BOOL bCommandEchod = false;

  // Get the number of bytes in the device input buffer
  Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumBytesDeviceInputBuffer > 0)
      FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

    Status = FTC_SendReceiveCommandFromMPSSEInterface(ftHandle, TRUE, AA_ECHO_CMD_1, &bCommandEchod);

    if (Status == FTC_SUCCESS)
    {
      if (bCommandEchod == TRUE)
      {
        Status = FTC_SendReceiveCommandFromMPSSEInterface(ftHandle, FALSE, AB_ECHO_CMD_2, &bCommandEchod);

        if (Status == FTC_SUCCESS)
        {
          if (bCommandEchod == false)
            Status = FTC_FAILED_TO_SYNCHRONIZE_DEVICE_MPSSE;
        }
      }
      else
        Status = FTC_FAILED_TO_SYNCHRONIZE_DEVICE_MPSSE;
    }
  }

  return Status;
}

/*
BOOLEAN FT2232c::FTC_Timeout(SYSTEMTIME StartSystemTime, DWORD dwTimeoutmSecs)
{
  BOOLEAN bTimoutExpired = false;
  FILETIME StartFileTime;
  ULARGE_INTEGER StartTime;
  SYSTEMTIME EndSystemTime;
  FILETIME EndFileTime;
  ULARGE_INTEGER EndTime;
  ULONGLONG ulTimeoutmSecs = dwTimeoutmSecs * CONVERT_1MS_TO_100NS; //10000;

  GetLocalTime(&EndSystemTime);

  SystemTimeToFileTime(&StartSystemTime, &StartFileTime);

  StartTime.LowPart = StartFileTime.dwLowDateTime;
  StartTime.HighPart = StartFileTime.dwHighDateTime;

  SystemTimeToFileTime(&EndSystemTime, &EndFileTime);

  EndTime.LowPart = EndFileTime.dwLowDateTime;
  EndTime.HighPart = EndFileTime.dwHighDateTime;

  if ((EndTime.QuadPart - StartTime.QuadPart) > ulTimeoutmSecs)
    bTimoutExpired = true;

  return bTimoutExpired;
  }*/

FTC_STATUS FT2232c::FTC_GetNumberBytesFromDeviceInputBuffer(FTC_HANDLE ftHandle, LPDWORD lpdwNumBytesDeviceInputBuffer)
{
  FTC_STATUS Status = FTC_SUCCESS;
  //SYSTEMTIME StartTime;
  
  // New timeout detection variables for linux build -- Julius
  struct timeval tv;
  time_t time_start; // to store number of seconds since unix epoch
  
  
  // Windows only time functions - removed for linux library build -- Julius
  //GetLocalTime(&StartTime);
  gettimeofday(&tv, NULL);
  time_start = tv.tv_sec;


  do
  {
    // Get the number of bytes in the device input buffer
    Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, lpdwNumBytesDeviceInputBuffer);

    if (Status == FTC_SUCCESS)
    {
      //Sleep(0);  // give up timeslice - doesn't work this way in linux, instead do a do a sched_yield():
      sched_yield();
      // This basically checks if the time we took from the first GetLocalTime was bigger than
      // MAX_COMMAND_TIMEOUT_PERIOD
      //if (FTC_Timeout(StartTime, MAX_COMMAND_TIMEOUT_PERIOD))
      gettimeofday(&tv, NULL);
      if ((tv.tv_sec - time_start) > (MAX_COMMAND_TIMEOUT_PERIOD / 1000))
	Status = FTC_FAILED_TO_COMPLETE_COMMAND;
    }
  }
  while ((*lpdwNumBytesDeviceInputBuffer == 0) && (Status == FTC_SUCCESS));
 
  return Status;
}

void FT2232c::FTC_ClearOutputBuffer(void)
{
  dwNumBytesToSend = 0;
}

void FT2232c::FTC_AddByteToOutputBuffer(DWORD dwOutputByte, BOOL bClearOutputBuffer)
{
  if (bClearOutputBuffer == TRUE)
    dwNumBytesToSend = 0;

  OutputBuffer[dwNumBytesToSend] = (dwOutputByte & '\xFF');

  dwNumBytesToSend = dwNumBytesToSend + 1;
}

DWORD FT2232c::FTC_GetNumBytesInOutputBuffer(void)
{
  return dwNumBytesToSend;
}

FTC_STATUS FT2232c::FTC_SendBytesToDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytesToSend = 0;
  DWORD dwNumBytesSent = 0;
  DWORD dwTotalNumBytesSent = 0;

  if (dwNumBytesToSend > MAX_NUM_BYTES_USB_WRITE)
  {
    do
    {
      // 25/08/05 - Can only use 4096 byte block as Windows 2000 Professional does not allow you to alter the USB buffer size
      // 25/08/05 - Windows 2000 Professional always sets the USB buffer size to 4K ie 4096
      if ((dwTotalNumBytesSent + MAX_NUM_BYTES_USB_WRITE) <= dwNumBytesToSend)
        dwNumDataBytesToSend = MAX_NUM_BYTES_USB_WRITE;
      else
        dwNumDataBytesToSend = (dwNumBytesToSend - dwTotalNumBytesSent);

      // This function sends data to a FT2232C dual type device. The dwNumBytesToSend variable specifies the number of
      // bytes in the output buffer to be sent to a FT2232C dual type device. The dwNumBytesSent variable contains
      // the actual number of bytes sent to a FT2232C dual type device.
      Status = FT_Write((FT_HANDLE)ftHandle, &OutputBuffer[dwTotalNumBytesSent], dwNumDataBytesToSend, &dwNumBytesSent);

      dwTotalNumBytesSent = dwTotalNumBytesSent + dwNumBytesSent;
    }
    while ((dwTotalNumBytesSent < dwNumBytesToSend) && (Status == FTC_SUCCESS)); 
  }
  else
  {
    // This function sends data to a FT2232C dual type device. The dwNumBytesToSend variable specifies the number of
    // bytes in the output buffer to be sent to a FT2232C dual type device. The dwNumBytesSent variable contains
    // the actual number of bytes sent to a FT2232C dual type device.
    Status = FT_Write((FT_HANDLE)ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  }

  
  dwNumBytesToSend = 0;

  return Status;
}

FTC_STATUS FT2232c::FTC_ReadBytesFromDevice(FTC_HANDLE ftHandle, PInputByteBuffer InputBuffer,
                                            DWORD dwNumBytesToRead, LPDWORD lpdwNumBytesRead)
{
  // This function reads data from a FT2232C dual type device. The dwNumBytesToRead variable specifies the maximum
  // number of bytes to be read from a FT2232C dual type device. The lpdwNumBytesRead variable contains the actual
  // number of bytes read from a FT2232C dual type device, which may range from zero to the actual number of bytes
  // requested, depending on how many have been received at the time of the request + the read timeout value.
  // The bytes read from a FT2232C dual type device, will be returned in the input buffer.
  return FT_Read((FT_HANDLE)ftHandle, InputBuffer, dwNumBytesToRead, lpdwNumBytesRead);
}

FTC_STATUS FT2232c::FTC_ReadFixedNumBytesFromDevice(FTC_HANDLE ftHandle, PInputByteBuffer InputBuffer,
                                                    DWORD dwNumBytesToRead, LPDWORD lpdwNumDataBytesRead)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  InputByteBuffer TmpInputByteBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwBytesReadIndex = 0;

  do
  {
    Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer);

    if ((Status == FTC_SUCCESS) && (dwNumBytesDeviceInputBuffer > 0))
    {
      Status = FTC_ReadBytesFromDevice(ftHandle, &TmpInputByteBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

      if (Status == FTC_SUCCESS)
      {
        for (dwBytesReadIndex = 0 ; dwBytesReadIndex < dwNumBytesRead; dwBytesReadIndex++)
        {
          (*InputBuffer)[*lpdwNumDataBytesRead] = TmpInputByteBuffer[dwBytesReadIndex];
          *lpdwNumDataBytesRead = (*lpdwNumDataBytesRead + 1);
        }
      }
    }
  }
  while ((*lpdwNumDataBytesRead < dwNumBytesToRead) && (Status == FTC_SUCCESS));

  return Status;
}

FTC_STATUS FT2232c::FTC_SendReadBytesToFromDevice(FTC_HANDLE ftHandle, PInputByteBuffer InputBuffer,
                                                  DWORD dwNumBytesToRead, LPDWORD lpdwNumBytesRead)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesSent = 0;
  DWORD dwNumControlSendBytes = 0;
  DWORD dwNumDataBytesRead = 0;

  if (dwNumBytesToSend > MAX_NUM_BYTES_USB_WRITE_READ)
  {
    // This function sends data to a FT2232C dual type device. The dwNumBytesToSend variable specifies the number of
    // bytes in the output buffer to be sent to a FT2232C dual type device. The dwNumBytesSent variable contains
    // the actual number of bytes sent to a FT2232C dual type device.
    Status = FT_Write((FT_HANDLE)ftHandle, OutputBuffer, MAX_NUM_BYTES_USB_WRITE_READ, &dwNumBytesSent);

    if (Status == FTC_SUCCESS)
    {
      dwNumControlSendBytes = (dwNumBytesToSend - dwNumBytesToRead);

      Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, InputBuffer, (MAX_NUM_BYTES_USB_WRITE_READ - dwNumControlSendBytes), &dwNumDataBytesRead);

      if (Status == FTC_SUCCESS)
      {
        Status = FT_Write((FT_HANDLE)ftHandle, &OutputBuffer[dwNumBytesSent], (dwNumBytesToSend - dwNumBytesSent), &dwNumBytesSent);

        if (Status == FTC_SUCCESS)
        {
          Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, InputBuffer, (dwNumBytesToRead - dwNumDataBytesRead), &dwNumDataBytesRead);

          if (Status == FTC_SUCCESS)
            *lpdwNumBytesRead = dwNumDataBytesRead;
        }
      }
    }
  }
  else
  {
    // This function sends data to a FT2232C dual type device. The dwNumBytesToSend variable specifies the number of
    // bytes in the output buffer to be sent to a FT2232C dual type device. The dwNumBytesSent variable contains
    // the actual number of bytes sent to a FT2232C dual type device.
    Status = FT_Write((FT_HANDLE)ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);

    if (Status == FTC_SUCCESS)
    {
      Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, InputBuffer, dwNumBytesToRead, &dwNumDataBytesRead);

      if (Status == FTC_SUCCESS)
        *lpdwNumBytesRead = dwNumDataBytesRead;
    }
  }

  dwNumBytesToSend = 0;

  return Status;
}

FTC_STATUS FT2232c::FTC_SendCommandsSequenceToDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytesToSend = 0;
  DWORD dwNumBytesSent = 0;
  DWORD dwTotalNumBytesSent = 0;

  if (dwNumBytesToSend > MAX_NUM_BYTES_USB_WRITE)
  {
    do
    {
      if ((dwTotalNumBytesSent + MAX_NUM_BYTES_USB_WRITE) <= dwNumBytesToSend)
        dwNumDataBytesToSend = MAX_NUM_BYTES_USB_WRITE;
      else
        dwNumDataBytesToSend = (dwNumBytesToSend - dwTotalNumBytesSent);

      // This function sends data to a FT2232C dual type device. The dwNumBytesToSend variable specifies the number of
      // bytes in the output buffer to be sent to a FT2232C dual type device. The dwNumBytesSent variable contains
      // the actual number of bytes sent to a FT2232C dual type device.
      Status = FT_Write((FT_HANDLE)ftHandle, &OutputBuffer[dwTotalNumBytesSent], dwNumDataBytesToSend, &dwNumBytesSent);

      dwTotalNumBytesSent = dwTotalNumBytesSent + dwNumBytesSent;
    }
    while ((dwTotalNumBytesSent < dwNumBytesToSend) && (Status == FTC_SUCCESS)); 
  }
  else
  {
    // This function sends data to a FT2232C dual type device. The dwNumBytesToSend variable specifies the number of
    // bytes in the output buffer to be sent to a FT2232C dual type device. The dwNumBytesSent variable contains
    // the actual number of bytes sent to a FT2232C dual type device.
    Status = FT_Write((FT_HANDLE)ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  }

  dwNumBytesToSend = 0;
  
  return Status;
}

FTC_STATUS FT2232c::FTC_ReadCommandsSequenceBytesFromDevice(FTC_HANDLE ftHandle, PInputByteBuffer InputBuffer,
                                                            DWORD dwNumBytesToRead, LPDWORD lpdwNumBytesRead)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytesToRead = 0;
  DWORD dwNumBytesRead = 0;
  DWORD dwTotalNumBytesRead = 0;

  if (dwNumBytesToRead > MAX_NUM_BYTES_USB_READ)
  {
    do
    {
      if ((dwTotalNumBytesRead + MAX_NUM_BYTES_USB_WRITE_READ) <= dwNumBytesToRead)
        dwNumDataBytesToRead = MAX_NUM_BYTES_USB_WRITE_READ;
      else
        dwNumDataBytesToRead = (dwNumBytesToRead - dwTotalNumBytesRead);

      Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, InputBuffer, dwNumDataBytesToRead, &dwNumBytesRead);

      dwTotalNumBytesRead = dwTotalNumBytesRead + dwNumBytesRead;
    }
    while ((dwTotalNumBytesRead < dwNumBytesToRead) && (Status == FTC_SUCCESS));

    *lpdwNumBytesRead = dwTotalNumBytesRead;
  }
  else
    Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, InputBuffer, dwNumBytesToRead, lpdwNumBytesRead);
  
  return Status;
}


// Define strupr and strlwr - not included in standard gcc libraries


//#if defined(__cplusplus) && __cplusplus
// extern "C" {
//#endif

char* strupr(char *string)
{
      char *s;

      if (string)
      {
            for (s = string; *s; ++s)
                  *s = toupper(*s);
      }
      return string;
} 

char* strlwr(char *string)
{
      char *s;

      if (string)
      {
            for (s = string; *s; ++s)
                  *s = tolower(*s);
      }
      return string;
}

//#if defined(__cplusplus) && __cplusplus
//}
//#endif
