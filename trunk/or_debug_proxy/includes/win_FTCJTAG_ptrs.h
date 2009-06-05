#ifndef _WIN_FTCJTAG_PTRS_H_
#define _WIN_FTCJTAG_PTRS_H_
	

typedef FTC_STATUS _stdcall (*jtagGetNumDevicesPtr)(LPDWORD lpdwNumDevices);
jtagGetNumDevicesPtr jtagGetNumDevices;

typedef FTC_STATUS _stdcall (*jtagInitDevicePtr)(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
jtagInitDevicePtr jtagInitDevice;

typedef FTC_STATUS _stdcall (*jtagOpenPtr)(FTC_HANDLE *pftHandle);
jtagOpenPtr jtagOpen;

typedef FTC_STATUS _stdcall (*jtagOpenExPtr)(LPSTR lpDeviceName, DWORD dwLocationIO, FTC_HANDLE *pftHandle);
jtagOpenExPtr jtagOpenEx;

typedef FTC_STATUS _stdcall (*jtagGetDeviceNameLocIDPtr)(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID);
jtagGetDeviceNameLocIDPtr jtagGetDeviceNameLocID;

typedef FTC_STATUS _stdcall (*jtagClosePtr)(FTC_HANDLE ftHandle);
jtagClosePtr jtagClose;


typedef FTC_STATUS _stdcall (*jtagGetClockPtr)(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
jtagGetClockPtr jtagGetClock;


typedef FTC_STATUS _stdcall (*jtagSetClockPtr)(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
jtagSetClockPtr jtagSetClock;

typedef FTC_STATUS _stdcall (*jtagSetLoopbackPtr)(FTC_HANDLE ftHandle, BOOL bLoopbackState);
jtagSetLoopbackPtr jtagSetLoopback;


typedef FTC_STATUS _stdcall (*jtagSetGPIOsPtr)(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,                                
					       PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,                                
					       BOOL bControlHighInputOutputPins,                                
					       PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData);
jtagSetGPIOsPtr jtagSetGPIOs;


typedef FTC_STATUS _stdcall (*jtagGetGPIOsPtr)(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
					       PFTC_LOW_HIGH_PINS pLowPinsInputData,
					       BOOL bControlHighInputOutputPins,
					       PFTC_LOW_HIGH_PINS pHighPinsInputData);
jtagGetGPIOsPtr jtagGetGPIOs;


typedef FTC_STATUS _stdcall (*jtagWritePtr)(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
					    PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
					    DWORD dwTapControllerState);
jtagWritePtr jtagWrite;


typedef FTC_STATUS _stdcall (*jtagReadPtr)(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
					   PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
					   DWORD dwTapControllerState);
jtagReadPtr jtagRead;


typedef FTC_STATUS _stdcall (*jtagWriteReadPtr)(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
						PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
						PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
						DWORD dwTapControllerState);
jtagWriteReadPtr jtagWriteRead;


typedef FTC_STATUS _stdcall (*jtagAddWriteCmdPtr)(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
						  PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
						  DWORD dwTapControllerState);
jtagAddWriteCmdPtr jtagAddWriteCmd;



typedef FTC_STATUS _stdcall (*jtagAddReadCmdPtr)(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
jtagAddReadCmdPtr jtagAddReadCmd;



typedef FTC_STATUS _stdcall (*jtagAddWriteReadCmdPtr)(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                       DWORD dwTapControllerState);
jtagAddWriteReadCmdPtr jtagAddWriteReadCmd;




typedef FTC_STATUS _stdcall (*jtagClearDeviceCmdSequencePtr)(FTC_HANDLE ftHandle);
jtagClearDeviceCmdSequencePtr jtagClearDeviceCmdSequence;



typedef FTC_STATUS _stdcall (*jtagAddDeviceWriteCmdPtr)(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
							PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
							DWORD dwTapControllerState);
							jtagAddDeviceWriteCmdPtr jtagAddDeviceWriteCmd;



typedef FTC_STATUS _stdcall (*jtagAddDeviceReadCmdPtr)(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState);
jtagAddDeviceReadCmdPtr jtagAddDeviceReadCmd;



typedef FTC_STATUS _stdcall (*jtagAddDeviceWriteReadCmdPtr)(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
							    PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
							    DWORD dwTapControllerState);
jtagAddDeviceWriteReadCmdPtr jtagAddDeviceWriteReadCmd;



typedef FTC_STATUS _stdcall (*jtagExecuteCmdSequencePtr)(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
							 LPDWORD lpdwNumBytesReturned);
jtagExecuteCmdSequencePtr jtagExecuteCmdSequence;



typedef FTC_STATUS _stdcall (*jtagGetDllVersionPtr)(LPSTR lpDllVersionBuffer, DWORD dwBufferSize);
jtagGetDllVersionPtr jtagGetDllVersion;



typedef FTC_STATUS _stdcall (*jtagGetErrorCodeStringPtr)(LPSTR lpLanguage, FTC_STATUS StatusCode,
							 LPSTR lpErrorMessageBuffer, DWORD dwBufferSize);
jtagGetErrorCodeStringPtr jtagGetErrorCodeString;


int getFTDIJTAGFunctions ();


#endif /* _WIN_FTCJTAG_PTRS_H_ */
