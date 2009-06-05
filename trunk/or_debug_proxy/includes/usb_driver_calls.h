/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : usb_functions.h
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

/*$$CHANGE HISTORY*/
/******************************************************************************/
/*                                                                            */
/*                         C H A N G E  H I S T O R Y                         */
/*                                                                            */
/******************************************************************************/

// Date		Version	Description
//------------------------------------------------------------------------
// 090301	1.0	Generic header file for modules containing the 
//                      platform specific D2XX MPSSE JTAG driver calls. jb
// 

#ifndef CYGWIN_COMPILE
#include "FT2232cMpsseJtag.h"
#else
#include "win_FTCJTAG.h"
#endif

// Function prototypes
FTC_STATUS 
FT2232_USB_JTAG_WriteDataToExternalDevice(
					  //FTC_HANDLE ftHandle, 
					  BOOL bInstructionTestData, 
					  DWORD dwNumBitsToWrite, 
					  PWriteDataByteBuffer pWriteDataBuffer, 
					  DWORD dwNumBytesToWrite, 
					  DWORD dwTapControllerState);


FTC_STATUS 
FT2232_USB_JTAG_ReadDataFromExternalDevice(
					   //FTC_HANDLE ftHandle, 
					    BOOL bInstructionTestData, 
					    DWORD dwNumBitsToRead, 
					    PReadDataByteBuffer pReadDataBuffer, 
					    LPDWORD lpdwNumBytesReturned,
					    DWORD dwTapControllerState);
FTC_STATUS 
FT2232_USB_JTAG_WriteReadDataToFromExternalDevice(
						  //FTC_HANDLE ftHandle, 
						  BOOL bInstructionTestData, 
						  DWORD dwNumBitsToWriteRead, 
						  PWriteDataByteBuffer pWriteDataBuffer, 
						  DWORD dwNumBytesToWrite, 
						  PReadDataByteBuffer pReadDataBuffer, 
						  LPDWORD lpdwNumBytesReturned,
						  DWORD dwTapControllerState);

FTC_STATUS 
FT2232_USB_JTAG_CloseDevice();

int init_usb_jtag();


