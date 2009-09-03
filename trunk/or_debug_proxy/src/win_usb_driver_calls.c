/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : win_usb_driver_calls.c
// Prepared By                    : jb
// Project Start                  : 2008-10-01

/*$$COPYRIGHT NOTICE*/
/******************************************************************************/
/*                                                                            */
/*                      C O P Y R I G H T   N O T I C E                       */
/*                                                                            */
/******************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; 
  version 2.1 of the License, a copy of which is available from
  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*$$DESCRIPTION*/
/******************************************************************************/
/*                                                                            */
/*                           D E S C R I P T I O N                            */
/*                                                                            */
/******************************************************************************/
//
// Some generically named functions for interfacing to the JTAG driver 
// functions, making the code calling these platform independant. The correct 
// driver calling file (either Cygwin or Linux driver) is included at compile 
// time.
// Also included in this file is the USB-JTAG chip initialisation function.
//


/*$$CHANGE HISTORY*/
/******************************************************************************/
/*                                                                            */
/*                         C H A N G E  H I S T O R Y                         */
/*                                                                            */
/******************************************************************************/

// Date		Version	Description
//------------------------------------------------------------------------
// 081101		First revision           			jb


#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>

#include "win_FTCJTAG.h"

#include "win_FTCJTAG_ptrs.h"	

#include "usb_driver_calls.h"

#include "or_debug_proxy.h"


// Load the pointers and loading functions of the FTCJTAG driver
int getFTDIJTAGFunctions (){
	
  HINSTANCE hLib;

  /* Load the library */
  hLib = LoadLibrary("lib/FTCJTAG.dll");
  
  if (hLib == NULL) {
    printf("Loading of library file \"FTCJTAG.dll\" failed!\n");
    return -1;
  }

  
  /* get the address of the functions */
  jtagGetNumDevices = (jtagGetNumDevicesPtr) GetProcAddress(hLib, "JTAG_GetNumDevices");
  
  if (jtagGetNumDevices == NULL) {
    printf("GetProcAddress for JTAG_GetNumDevices Failed.\n");
    return -1;
  }

  jtagInitDevice = (jtagInitDevicePtr) GetProcAddress(hLib, "JTAG_InitDevice");  
  if (jtagInitDevice == NULL) {
    printf("GetProcAddress for JTAG_InitDevice  Failed.\n");
    return -1;
  }
  
  jtagOpen = (jtagOpenPtr) GetProcAddress(hLib, "JTAG_Open");  
  if (jtagOpen == NULL) {
    printf("GetProcAddress for JTAG_Open  Failed.\n");
    return -1;
  }
  
  jtagOpenEx = (jtagOpenExPtr) GetProcAddress(hLib, "JTAG_OpenEx");  
  if (jtagOpenEx == NULL) {
    printf("GetProcAddress for JTAG_OpenEx  Failed.\n");
    return -1;
  }

  jtagGetDeviceNameLocID = (jtagGetDeviceNameLocIDPtr) GetProcAddress(hLib, "JTAG_GetDeviceNameLocID");  
  if (jtagGetDeviceNameLocID == NULL) {
    printf("GetProcAddress for  Failed JTAG_GetDeviceNameLocID.\n");
    return -1;
  }

  jtagClose = (jtagClosePtr) GetProcAddress(hLib, "JTAG_Close");  
  if (jtagClose == NULL) {
    printf("GetProcAddress for JTAG_Close  Failed.\n");
    return -1;
  }
  
  jtagGetClock = (jtagGetClockPtr) GetProcAddress(hLib, "JTAG_GetClock");  
  if (jtagGetClock == NULL) {
    printf("GetProcAddress for JTAG_GetClock  Failed.\n");
    return -1;
  }

  jtagSetClock = (jtagSetClockPtr) GetProcAddress(hLib, "JTAG_SetClock");  
  if (jtagSetClock == NULL) {
    printf("GetProcAddress for JTAG_SetClock  Failed.\n");
    return -1;
  }

  jtagSetLoopback = (jtagSetLoopbackPtr) GetProcAddress(hLib, "JTAG_SetLoopback");  
  if (jtagSetLoopback == NULL) {
    printf("GetProcAddress for JTAG_SetLoopback  Failed.\n");
    return -1;
  }
  
  jtagSetGPIOs = (jtagSetGPIOsPtr) GetProcAddress(hLib, "JTAG_SetGPIOs");  
  if (jtagSetGPIOs == NULL) {
    printf("GetProcAddress for JTAG_SetGPIOs  Failed.\n");
    return -1;
  }

  jtagGetGPIOs = (jtagGetGPIOsPtr) GetProcAddress(hLib, "JTAG_GetGPIOs");  
  if (jtagGetGPIOs == NULL) {
    printf("GetProcAddress for JTAG_GetGPIOs  Failed.\n");
    return -1;
  }
  
  jtagWrite = (jtagWritePtr) GetProcAddress(hLib, "JTAG_Write");  
  if (jtagWrite == NULL) {
    printf("GetProcAddress for JTAG_Write  Failed.\n");
    return -1;
  }

  jtagRead = (jtagReadPtr) GetProcAddress(hLib, "JTAG_Read");  
  if (jtagRead == NULL) {
    printf("GetProcAddress for JTAG_Read  Failed.\n");
    return -1;
  }

  jtagWriteRead = (jtagWriteReadPtr) GetProcAddress(hLib, "JTAG_WriteRead");  
  if (jtagWriteRead == NULL) {
    printf("GetProcAddress for JTAG_WriteRead  Failed.\n");
    return -1;
  }

  jtagAddWriteCmd = (jtagAddWriteCmdPtr) GetProcAddress(hLib, "JTAG_AddWriteCmd");  
  if (jtagAddWriteCmd == NULL) {
    printf("GetProcAddress for JTAG_AddWriteCmd  Failed.\n");
    return -1;
  }

  jtagAddReadCmd = (jtagAddReadCmdPtr) GetProcAddress(hLib, "JTAG_AddReadCmd");  
  if (jtagAddReadCmd == NULL) {
    printf("GetProcAddress for JTAG_AddReadCmd  Failed.\n");
    return -1;
  }

  jtagAddWriteReadCmd = (jtagAddWriteReadCmdPtr) GetProcAddress(hLib, "JTAG_AddWriteReadCmd");  
  if (jtagAddWriteReadCmd == NULL) {
    printf("GetProcAddress for JTAG_AddWriteReadCmd  Failed.\n");
    return -1;
  }

  jtagClearDeviceCmdSequence = (jtagClearDeviceCmdSequencePtr) GetProcAddress(hLib, "JTAG_ClearDeviceCmdSequence");  
  if (jtagClearDeviceCmdSequence == NULL) {
    printf("GetProcAddress for  Failed.\n JTAG_ClearDeviceCmdSequence");
    return -1;
  }

  jtagAddDeviceReadCmd = (jtagAddDeviceReadCmdPtr) GetProcAddress(hLib, "JTAG_AddDeviceReadCmd");  
  if (jtagAddDeviceReadCmd == NULL) {
    printf("GetProcAddress for  Failed JTAG_AddDeviceReadCmd.\n");
    return -1;
  }

  jtagAddDeviceWriteReadCmd = (jtagAddDeviceWriteReadCmdPtr) GetProcAddress(hLib, "JTAG_AddDeviceWriteReadCmd");  
  if (jtagAddDeviceWriteReadCmd == NULL) {
    printf("GetProcAddress for  Failed.\n JTAG_AddDeviceWriteReadCmd");
    return -1;
  }

  jtagExecuteCmdSequence = (jtagExecuteCmdSequencePtr) GetProcAddress(hLib, "JTAG_ExecuteCmdSequence");  
  if (jtagExecuteCmdSequence == NULL) {
    printf("GetProcAddress for  Failed JTAG_ExecuteCmdSequence.\n");
    return -1;
  }

  jtagGetDllVersion = (jtagGetDllVersionPtr) GetProcAddress(hLib, "JTAG_GetDllVersion");  
  if (jtagGetDllVersion == NULL) {
    printf("GetProcAddress for JTAG_GetDllVersion  Failed.\n");
    return -1;
  }
  
  jtagGetErrorCodeString = (jtagGetErrorCodeStringPtr) GetProcAddress(hLib, "JTAG_GetErrorCodeString");  
  if (jtagGetErrorCodeString == NULL) {
    printf("GetProcAddress for  Failed JTAG_GetErrorCodeString.\n");
    return -1;
  }

  //   = (Ptr) GetProcAddress(hLib, "");  
  //if ( == NULL) {
  //  printf("GetProcAddress for  Failed.\n");
  //  return -1;
  // }
  
  return 0;

}
 

// Global USB JTAG device handle
FTC_HANDLE ftHandle;

FTC_STATUS
FT2232_USB_JTAG_WriteDataToExternalDevice(
					  //FTC_HANDLE ftHandle, 
					  BOOL bInstructionTestData, 
					  DWORD dwNumBitsToWrite, 
					  PWriteDataByteBuffer pWriteDataBuffer, 
					  DWORD dwNumBytesToWrite, 
					  DWORD dwTapControllerState)
{
  // Return the appropritae call to the Windows driver
  return (jtagWrite)(
		     ftHandle, 
		     bInstructionTestData, 
		     dwNumBitsToWrite, 
		     pWriteDataBuffer, 
		     dwNumBytesToWrite, 
		     dwTapControllerState);

}
  

// Read data from external device
FTC_STATUS 
FT2232_USB_JTAG_ReadDataFromExternalDevice(
					   //FTC_HANDLE ftHandle, 
					    BOOL bInstructionTestData, 
					    DWORD dwNumBitsToRead, 
					    PReadDataByteBuffer pReadDataBuffer, 
					    LPDWORD lpdwNumBytesReturned,
					    DWORD dwTapControllerState)

{
  return (jtagRead)(
		    ftHandle, 
		    bInstructionTestData, 
		    dwNumBitsToRead, 
		    pReadDataBuffer, 
		    lpdwNumBytesReturned,
		    dwTapControllerState);
  
}






// Write Read
FTC_STATUS 
FT2232_USB_JTAG_WriteReadDataToFromExternalDevice(
						  //FTC_HANDLE ftHandle, 
						  BOOL bInstructionTestData, 
						  DWORD dwNumBitsToWriteRead, 
						  PWriteDataByteBuffer pWriteDataBuffer, 
						  DWORD dwNumBytesToWrite, 
						  PReadDataByteBuffer pReadDataBuffer, 
						  LPDWORD lpdwNumBytesReturned,
						  DWORD dwTapControllerState)
{

 return (jtagWriteRead)(
			ftHandle, 
			bInstructionTestData, 
			dwNumBitsToWriteRead,
			pWriteDataBuffer, 
			dwNumBytesToWrite,
			pReadDataBuffer, 
			lpdwNumBytesReturned,
			dwTapControllerState);

}



// Close device
FTC_STATUS 
FT2232_USB_JTAG_CloseDevice()
{
  return (jtagClose)(ftHandle);
}

void 
reinit_usb_jtag(void)
{
  FTC_STATUS Status = FTC_SUCCESS;
  if (DEBUG_USB_DRVR_FUNCS) printf("win_usb_driver_calls: reset_usb_jtag() - ");
  Status = (jtagInitDevice)(ftHandle, 0);
  if (DEBUG_USB_DRVR_FUNCS) printf("return Status: %d\n", (int) Status);
}

// Set clock frequency
// Frequency = 12Mhz/((1+divisor)*2), 
// divisor=1000 => Freq=5995Hz~=6kHz, divisor=500=>Freq=12kHz
#define USB_JTAG_CLK_DIVIDER 0

int init_usb_jtag()
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDevices = 0;
  char szDeviceName[100];
  DWORD dwLocationID = 0;
  // Made global: FTC_HANDLE ftHandle;
  DWORD dwClockFrequencyHz = 0;
  FTC_INPUT_OUTPUT_PINS LowInputOutputPinsData;
  FTC_INPUT_OUTPUT_PINS HighInputOutputPinsData;
  FTC_LOW_HIGH_PINS LowPinsInputData;
  FTC_LOW_HIGH_PINS HighPinsInputData;
  BOOL bPerformCommandSequence = false;
  
  DWORD dwLoopCntr = 0;
  char szDllVersion[10];
  
  Status = (jtagGetNumDevices)(&dwNumDevices);
  if (DEBUG_USB_DRVR_FUNCS) 
    printf("JTAG_GetNumDevices returned Status: 0x%x and %d devices\n",
                    Status, dwNumDevices);


  
  if (dwNumDevices == 0)
    {
      printf("Error: USB debug cable not detected\nPlease ensure the device is attached and correctly installed\n\n");
      exit(-1);
    }
  
  
  Status = (jtagGetDllVersion)(szDllVersion, 10);

  if (DEBUG_USB_DRVR_FUNCS) 
    printf("JTAG_GetDLLVersion returned Status: 0x%x and version %s\n",
                     Status, szDllVersion);


  // To do: Iterate through dwNumDevices by index number ,looking for the ORSoC Device, "ORSoC OpenRISC debug cable A\0",
  
  if ((Status == FTC_SUCCESS) && (dwNumDevices > 0))
    {
      if (dwNumDevices == 1)
	{
	  Status = (jtagGetDeviceNameLocID)(0,szDeviceName,50, &dwLocationID); 
	  
	  if (DEBUG_USB_DRVR_FUNCS) 
	    printf("JTAG_GetDeviceNameLocID: %s at: 0x%x\n", 
		   szDeviceName, dwLocationID);
	  
	  if (Status == FTC_SUCCESS)
	    {
	      Status = (jtagOpenEx)(szDeviceName, dwLocationID, &ftHandle);
	      
	    }
	}
      else
	{
	  if (dwNumDevices == 2)
	    {
	      Status=(jtagGetDeviceNameLocID)(1,szDeviceName,50,&dwLocationID);
	      
	      if (Status == FTC_SUCCESS)
		{
		  Status = (jtagOpenEx)(szDeviceName, dwLocationID, &ftHandle);
		}
	    }
	}
    }	
  
  
  if ((Status == FTC_SUCCESS) && (dwNumDevices > 0))
    {
      Status = (jtagInitDevice)(ftHandle, 0);
      
      // Set clock frequency
      // Frequency = 12Mhz/((1+divisor)*2), 
      // divisor=1000 => Freq=5995Hz~=6kHz, divisor=500=>Freq=12kHz
      if (Status == FTC_SUCCESS)
	Status = (jtagSetClock)(ftHandle, USB_JTAG_CLK_DIVIDER, 
				&dwClockFrequencyHz);	
      
      
      if (Status == FTC_SUCCESS){
	printf("USB JTAG interface initialised\n");
	Status = (jtagGetClock)(5, &dwClockFrequencyHz);
      }	    

      if (Status == FTC_SUCCESS)
	{ // true=output, false=input
	  LowInputOutputPinsData.bPin1InputOutputState = true; // ADBUS4 - 
	  // set ADBUS4 to output 0 to enable JTAG buffers on Olimex devices
	  LowInputOutputPinsData.bPin2InputOutputState = true; //ADBUS5
	  LowInputOutputPinsData.bPin3InputOutputState = true;
	  LowInputOutputPinsData.bPin4InputOutputState = true;
	  //ADBUS4 = 0 (enable JTAG buffers on Olimex devices)
	  LowInputOutputPinsData.bPin1LowHighState = false; // ADBUS4
	  LowInputOutputPinsData.bPin2LowHighState = false; // ADBUS5
	  LowInputOutputPinsData.bPin3LowHighState = true; // ADBUS6
	  LowInputOutputPinsData.bPin4LowHighState = true; // ADBUS7
	  
	  HighInputOutputPinsData.bPin1InputOutputState = true; // ACBUS0
	  HighInputOutputPinsData.bPin2InputOutputState = true; // ACBUS1
	  HighInputOutputPinsData.bPin3InputOutputState = true; // ACBUS2
	  HighInputOutputPinsData.bPin4InputOutputState = true; // ACBUS3
	  HighInputOutputPinsData.bPin1LowHighState = true; // ACBUS0
	  HighInputOutputPinsData.bPin2LowHighState = true; // ACBUS1
	  HighInputOutputPinsData.bPin3LowHighState = true; // ACBUS2
	    HighInputOutputPinsData.bPin4LowHighState =true; // ACBUS3
	  
	  Status = (jtagSetGPIOs)(ftHandle, true, &LowInputOutputPinsData, 
					true, &HighInputOutputPinsData);
	  
	  if (Status == FTC_SUCCESS)
	    Status = (jtagGetGPIOs)(ftHandle, true, &LowPinsInputData, 
				    true, &HighPinsInputData);
	  
	  if (DEBUG_USB_DRVR_FUNCS) 
	    printf("JTAG low gpio pins values: (ADBUS7-ADBUS4):%x%x%x%x\n", 
		   (LowPinsInputData.bPin4LowHighState==true), 
		   (LowPinsInputData.bPin3LowHighState==true),
		   (LowPinsInputData.bPin2LowHighState==true),
		   (LowPinsInputData.bPin1LowHighState==true));
	  
	  if (DEBUG_USB_DRVR_FUNCS) 
	    printf("JTAG high gpio pins values: (ACBUS3-ACBUS0): %x%x%x%x\n", 
		   (HighPinsInputData.bPin4LowHighState==true), 
		   (HighPinsInputData.bPin3LowHighState==true),
		   (HighPinsInputData.bPin2LowHighState==true),
		   (HighPinsInputData.bPin1LowHighState==true));
	}	    
    }
  
  if (Status == FTC_SUCCESS)
    return 0;
  else
    return 1;	
  
}
