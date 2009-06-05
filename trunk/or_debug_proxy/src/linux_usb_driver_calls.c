/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : linux_usb_driver_calls.c
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

#include "WinTypes.h"

#include "usb_driver_calls.h"

#include "usb_functions.h"
#include "or_debug_proxy.h"

#include "FT2232cMpsseJtag.h"

static FT2232cMpsseJtag *pFT2232cMpsseJtag = NULL;

// Global USB JTAG device handle
FTC_HANDLE ftHandle;

// Write data to external device:
//FTC_STATUS FT2232cMpsseJtag::JTAG_WriteDataToExternalDevice(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,

FTC_STATUS
FT2232_USB_JTAG_WriteDataToExternalDevice(
					  //FTC_HANDLE ftHandle, 
					  BOOL bInstructionTestData, 
					  DWORD dwNumBitsToWrite, 
					  PWriteDataByteBuffer pWriteDataBuffer, 
					  DWORD dwNumBytesToWrite, 
					  DWORD dwTapControllerState)
{
  // Call the driver from the libary compiled in Linux
  return pFT2232cMpsseJtag->JTAG_WriteDataToExternalDevice(
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
  return pFT2232cMpsseJtag->JTAG_ReadDataFromExternalDevice(
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

 return pFT2232cMpsseJtag->JTAG_WriteReadDataToFromExternalDevice(
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
  return pFT2232cMpsseJtag->JTAG_CloseDevice(ftHandle);
}



// Set clock frequency
// Frequency = 12Mhz/((1+divisor)*2), 
// divisor=1000 => Freq=5995Hz~=6kHz, divisor=500=>Freq=12kHz
#define USB_JTAG_CLK_DIVIDER 0

int init_usb_jtag()
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDevices = 0;
  //char szDeviceName[100];
  //char szDeviceName[] = "Dual RS232 A"; // Original, unmodified FT2232 device name
  // We now open the device by its name
  char szDeviceName[] = "ORSoC OpenRISC debug cable A\0"; // ORSoC debug cable UART A name
  DWORD dwLocationID = 0;
  DWORD dwClockFrequencyHz = 0;
  FTC_INPUT_OUTPUT_PINS LowInputOutputPinsData;
  FTC_INPUT_OUTPUT_PINS HighInputOutputPinsData;
  FTC_LOW_HIGH_PINS LowPinsInputData;
  FTC_LOW_HIGH_PINS HighPinsInputData;

  //char szDllVersion[10];

  // Has been changed to hardcode load device named "ORSoC OpenRISC debug cable A"; - jb 090301
  
  /*
  //Status = JTAG_GetNumDevices(&dwNumDevices);
  Status = pFT2232cMpsseJtag->JTAG_GetNumDevices(&dwNumDevices);
  if (DEBUG_USB_DRVR_FUNCS) 
    {
      if (Status == FTC_SUCCESS)
	printf("JTAG_GetNumDevices detected %ld available FT2232x device(s) connected to the system.\n", dwNumDevices);
      else 
	{
	  printf("GetNumDevices failed with status code 0x%lx\n", Status);
	  exit(-1);
	}
    }

  if (dwNumDevices == 0)
    {
      printf("Error: USB Debugger device not detected\n");
      exit(-1);
    }
  
  Status = pFT2232cMpsseJtag->JTAG_GetDllVersion(szDllVersion, 10);
  
  if (DEBUG_USB_DRVR_FUNCS) 
    printf("JTAG_GetDLLVersion returned Status: 0x%lx and version %s\n", Status, szDllVersion);

  */
  dwNumDevices = 1;

  if (Status == FTC_SUCCESS)
    {
      
      
      if (dwNumDevices == 1)
	{
	  /*
	  Status = pFT2232cMpsseJtag->JTAG_GetDeviceNameLocationID(0,szDeviceName,50, &dwLocationID); 
	  
	  if (DEBUG_USB_DRVR_FUNCS) 
	    // In the Windows version it shows Dual RS323 A at: 0x321
	    printf("JTAG_GetDeviceNameLocID: %s at: 0x%lx\n", szDeviceName, dwLocationID);      
	  */
	  dwLocationID = 0;
	  
	  if (Status == FTC_SUCCESS)
	    {
	      Status = pFT2232cMpsseJtag->JTAG_OpenSpecifiedDevice(szDeviceName,dwLocationID, &ftHandle);
	    }
	}
      else 
	// When there's more than 1 device, will just open first device. Perhaps implement selection menu for
	// users in event that there's more than 1, but for now just hard code this to open the same device
	{
	  //if (dwNumDevices == 2)
	  //{
	  Status=pFT2232cMpsseJtag->JTAG_GetDeviceNameLocationID(1,szDeviceName,50,&dwLocationID);
	  
	  if (Status == FTC_SUCCESS)
	    {
	      Status = pFT2232cMpsseJtag->JTAG_OpenSpecifiedDevice(szDeviceName, dwLocationID, &ftHandle);
	    }
	  //}
	}

      // Try initialising and obtaining a handle to a specific device
      if (Status == FTC_SUCCESS)
	{
	  printf("Initialising USB JTAG interface\n");
	  Status = pFT2232cMpsseJtag->JTAG_InitDevice(ftHandle, 0);
	  
	  // Set clock frequency
	  // Frequency = 12Mhz/((1+divisor)*2), 
	  // divisor=1000 => Freq=5995Hz~=6kHz, divisor=500=>Freq=12kHz
	  if (DEBUG_USB_DRVR_FUNCS)
	    printf("Setting JTAG clock frequency.  Divisor = %d, TCK ~= %dkHz.\n", USB_JTAG_CLK_DIVIDER, (12000000/((USB_JTAG_CLK_DIVIDER+1)*2))/1000);
	  if (Status == FTC_SUCCESS)
	    Status = pFT2232cMpsseJtag->JTAG_SetClock(ftHandle, USB_JTAG_CLK_DIVIDER, &dwClockFrequencyHz);
 
	  if (Status == FTC_SUCCESS){
	    
	    Status = pFT2232cMpsseJtag->JTAG_GetClock(5, &dwClockFrequencyHz);
	  }	    
	  
	}
      
      //if (Status == FTC_SUCCESS)
      //  Status = JTAG_SetLoopback(ftHandle, true);	    
      if (Status == FTC_SUCCESS)
	{ 
	  if (DEBUG_USB_DRVR_FUNCS) // Let LED blink
	    {
	      // true=output, false=input
	      LowInputOutputPinsData.bPin1InputOutputState = true;  // ADBUS4
	      LowInputOutputPinsData.bPin2InputOutputState = true;  // ADBUS5
	      LowInputOutputPinsData.bPin3InputOutputState = true;  // ADBUS6
	      LowInputOutputPinsData.bPin4InputOutputState = true;  // ADBUS7
	      LowInputOutputPinsData.bPin1LowHighState = false;     // ADBUS4
	      LowInputOutputPinsData.bPin2LowHighState = false;     // ADBUS5
	      LowInputOutputPinsData.bPin3LowHighState = true;      // ADBUS6
	      LowInputOutputPinsData.bPin4LowHighState = true;      // ADBUS7
	      
	      HighInputOutputPinsData.bPin1InputOutputState = true; // ACBUS0 Output
	      HighInputOutputPinsData.bPin2InputOutputState = true; // ACBUS1 Output
	      HighInputOutputPinsData.bPin3InputOutputState = true; // ACBUS2 Output
	      HighInputOutputPinsData.bPin4InputOutputState = true; // ACBUS3 Output
	      HighInputOutputPinsData.bPin1LowHighState = true;     // ACBUS0
	      HighInputOutputPinsData.bPin2LowHighState = true;     // ACBUS1
	      HighInputOutputPinsData.bPin3LowHighState = true;     // ACBUS2  LED RXLEDA A1 OFF 
	      HighInputOutputPinsData.bPin4LowHighState = false;    // ACBUS3  LED TXLEDA A2 ON
	      
	      Status = pFT2232cMpsseJtag->JTAG_SetGeneralPurposeInputOutputPins(ftHandle, 
										true, 
										&LowInputOutputPinsData, 
										true, 
										&HighInputOutputPinsData);
		  
	      LowInputOutputPinsData.bPin1InputOutputState = true;  // ADBUS4 
	      LowInputOutputPinsData.bPin2InputOutputState = true;  // ADBUS5
	      LowInputOutputPinsData.bPin3InputOutputState = true;  // ADBUS6
	      LowInputOutputPinsData.bPin4InputOutputState = true;  // ADBUS7
	      LowInputOutputPinsData.bPin1LowHighState = false;     // ADBUS4
	      LowInputOutputPinsData.bPin2LowHighState = false;     // ADBUS5
	      LowInputOutputPinsData.bPin3LowHighState = true;      // ADBUS6
	      LowInputOutputPinsData.bPin4LowHighState = true;      // ADBUS7
	      
	      HighInputOutputPinsData.bPin1InputOutputState = true; // ACBUS0
	      HighInputOutputPinsData.bPin2InputOutputState = true; // ACBUS1
	      HighInputOutputPinsData.bPin3InputOutputState = true; // ACBUS2
	      HighInputOutputPinsData.bPin4InputOutputState = true; // ACBUS3
	      HighInputOutputPinsData.bPin1LowHighState = true;     // ACBUS0
	      HighInputOutputPinsData.bPin2LowHighState = true;     // ACBUS1
	      HighInputOutputPinsData.bPin3LowHighState = false;    // ACBUS2 LED RXLEDA A1 ON
	      HighInputOutputPinsData.bPin4LowHighState = true;     // ACBUS3 LED TXLEDA A2 OFF
	      
	      Status = pFT2232cMpsseJtag->JTAG_SetGeneralPurposeInputOutputPins(ftHandle, 
										true, 
										&LowInputOutputPinsData, 
										true, 
										&HighInputOutputPinsData);
	      
	      LowInputOutputPinsData.bPin1InputOutputState = true;  // ADBUS4 - 
	      LowInputOutputPinsData.bPin2InputOutputState = true;  // ADBUS5
	      LowInputOutputPinsData.bPin3InputOutputState = true;  // ADBUS6
	      LowInputOutputPinsData.bPin4InputOutputState = true;  // ADBUS7
	      LowInputOutputPinsData.bPin1LowHighState = false;     // ADBUS4
	      LowInputOutputPinsData.bPin2LowHighState = false;     // ADBUS5
	      LowInputOutputPinsData.bPin3LowHighState = true;      // ADBUS6
	      LowInputOutputPinsData.bPin4LowHighState = true;      // ADBUS7
	      
	      HighInputOutputPinsData.bPin1InputOutputState = true; // ACBUS0
	      HighInputOutputPinsData.bPin2InputOutputState = true; // ACBUS1
	      HighInputOutputPinsData.bPin3InputOutputState = true; // ACBUS2
	      HighInputOutputPinsData.bPin4InputOutputState = true; // ACBUS3
	      HighInputOutputPinsData.bPin1LowHighState = true;     // ACBUS0
	      HighInputOutputPinsData.bPin2LowHighState = true;     // ACBUS1
	      HighInputOutputPinsData.bPin3LowHighState = true;     // ACBUS2 LED RXLEDA A1 OFF
	      HighInputOutputPinsData.bPin4LowHighState = true;     // ACBUS3 LED TXLEDA A2 OFF
	      
	      Status = pFT2232cMpsseJtag->JTAG_SetGeneralPurposeInputOutputPins(ftHandle, 
										true, 
										&LowInputOutputPinsData, 
										true, 
										&HighInputOutputPinsData);
	      
	      LowInputOutputPinsData.bPin1InputOutputState = true;  // ADBUS4 - 
	      LowInputOutputPinsData.bPin2InputOutputState = true;  // ADBUS5
	      LowInputOutputPinsData.bPin3InputOutputState = true;  // ADBUS6
	      LowInputOutputPinsData.bPin4InputOutputState = true;  // ADBUS7
	      LowInputOutputPinsData.bPin1LowHighState = false;     // ADBUS4
	      LowInputOutputPinsData.bPin2LowHighState = false;     // ADBUS5
	      LowInputOutputPinsData.bPin3LowHighState = true;      // ADBUS6
	      LowInputOutputPinsData.bPin4LowHighState = true;      // ADBUS7
	      
	      HighInputOutputPinsData.bPin1InputOutputState = true; // ACBUS0
	      HighInputOutputPinsData.bPin2InputOutputState = true; // ACBUS1
	      HighInputOutputPinsData.bPin3InputOutputState = true; // ACBUS2
	      HighInputOutputPinsData.bPin4InputOutputState = true; // ACBUS3
	      HighInputOutputPinsData.bPin1LowHighState = true;     // ACBUS0
	      HighInputOutputPinsData.bPin2LowHighState = true;     // ACBUS1
	      HighInputOutputPinsData.bPin3LowHighState = false;    // ACBUS2 LED RXLEDA A1 ON
	      HighInputOutputPinsData.bPin4LowHighState = false;    // ACBUS3 LED TXLEDA A2 ON
	      
	      Status = pFT2232cMpsseJtag->JTAG_SetGeneralPurposeInputOutputPins(ftHandle, 
										true, 
										&LowInputOutputPinsData, 
										true, 
										&HighInputOutputPinsData);
	    }
	  
	  // Setup GPIO pins for JTAG
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
	  HighInputOutputPinsData.bPin3LowHighState = false; // ACBUS2 LED RXLEDA A1 ON
	  HighInputOutputPinsData.bPin4LowHighState = true; // ACBUS3
	  
	  Status = pFT2232cMpsseJtag->JTAG_SetGeneralPurposeInputOutputPins(ftHandle, 
									    true, 
									    &LowInputOutputPinsData, 
									    true, 
									    &HighInputOutputPinsData);
	  
	  if (Status == FTC_SUCCESS)
	    Status = pFT2232cMpsseJtag->JTAG_GetGeneralPurposeInputOutputPins(ftHandle, true, &LowPinsInputData, 
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
