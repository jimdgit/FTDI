/*++

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    ftcspi.cpp

Abstract:

    API DLL for FT2232H and FT4232H Hi-Speed Dual Device and FT2232D Dual Device setup to simulate the
    Serial Peripheral Interface(SPI) synchronous serial protocol.
	  Defines the entry point for the DLL application.

Environment:

    kernel & user mode

Revision History:

    29/06/05    kra     Created.
    21/08/08    kra     Added new functions for FT2232H and FT4232H hi-speed devices.
	
--*/

#include "stdafx.h"

#include <windows.h>

#include "ftcspi.h"

#include "ft2232cmpssespi.h"

static FT2232cMpsseSpi *pFT2232cMpsseSpi = NULL;

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
  	{
		case DLL_PROCESS_ATTACH:
      if (pFT2232cMpsseSpi == NULL)
        pFT2232cMpsseSpi = new FT2232cMpsseSpi();
      break;
		case DLL_THREAD_ATTACH:
      break;
		case DLL_THREAD_DETACH:
      break;
		case DLL_PROCESS_DETACH:
      pFT2232cMpsseSpi->~FT2232cMpsseSpi();
		  break;
    }

    return TRUE;
}

//---------------------------------------------------------------------------

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetNumDevices(LPDWORD lpdwNumDevices)
{
  return pFT2232cMpsseSpi->SPI_GetNumDevices(lpdwNumDevices);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetNumHiSpeedDevices(LPDWORD lpdwNumHiSpeedDevices)
{
  return pFT2232cMpsseSpi->SPI_GetNumHiSpeedDevices(lpdwNumHiSpeedDevices);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetDeviceNameLocID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
{
  return pFT2232cMpsseSpi->SPI_GetDeviceNameLocationID(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetHiSpeedDeviceNameLocIDChannel(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID, LPSTR lpChannel, DWORD dwChannelBufferSize, LPDWORD lpdwHiSpeedDeviceType)
{
  return pFT2232cMpsseSpi->SPI_GetHiSpeedDeviceNameLocIDChannel(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID, lpChannel, dwChannelBufferSize, lpdwHiSpeedDeviceType);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_Open(FTC_HANDLE *pftHandle)
{
  return pFT2232cMpsseSpi->SPI_OpenDevice(pftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_OpenEx(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
  return pFT2232cMpsseSpi->SPI_OpenSpecifiedDevice(lpDeviceName, dwLocationID, pftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_OpenHiSpeedDevice(LPSTR lpDeviceName, DWORD dwLocationID, LPSTR lpChannel, FTC_HANDLE *pftHandle)
{
  return pFT2232cMpsseSpi->SPI_OpenSpecifiedHiSpeedDevice(lpDeviceName, dwLocationID, lpChannel, pftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetHiSpeedDeviceType(FTC_HANDLE ftHandle, LPDWORD lpdwHiSpeedDeviceType)
{
  return pFT2232cMpsseSpi->SPI_GetHiSpeedDeviceType(ftHandle, lpdwHiSpeedDeviceType);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_Close(FTC_HANDLE ftHandle)
{
  return pFT2232cMpsseSpi->SPI_CloseDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_CloseDevice(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
{
  return pFT2232cMpsseSpi->SPI_CloseDevice(ftHandle, pCloseFinalStatePinsData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  return pFT2232cMpsseSpi->SPI_InitDevice(ftHandle, dwClockDivisor);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_TurnOnDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  return pFT2232cMpsseSpi->SPI_TurnOnDivideByFiveClockingHiSpeedDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_TurnOffDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  return pFT2232cMpsseSpi->SPI_TurnOffDivideByFiveClockingHiSpeedDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE timerValue)
{
  return pFT2232cMpsseSpi->SPI_SetDeviceLatencyTimer(ftHandle, timerValue);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetDeviceLatencyTimer(FTC_HANDLE ftHandle, LPBYTE lpTimerValue)
{
  return pFT2232cMpsseSpi->SPI_GetDeviceLatencyTimer(ftHandle, lpTimerValue);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  return pFT2232cMpsseSpi->SPI_GetClock(dwClockDivisor, lpdwClockFrequencyHz);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetHiSpeedDeviceClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  return pFT2232cMpsseSpi->SPI_GetHiSpeedDeviceClock(dwClockDivisor, lpdwClockFrequencyHz);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  return pFT2232cMpsseSpi->SPI_SetClock(ftHandle, dwClockDivisor, lpdwClockFrequencyHz);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_SetLoopback(FTC_HANDLE ftHandle, BOOL bLoopbackState)
{
  return pFT2232cMpsseSpi->SPI_SetDeviceLoopbackState(ftHandle, bLoopbackState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_SetGPIOs(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates, PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  return pFT2232cMpsseSpi->SPI_SetGeneralPurposeHighInputOutputPins(ftHandle, pChipSelectsDisableStates, pHighInputOutputPinsData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_SetHiSpeedDeviceGPIOs(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates,
                                            PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  return pFT2232cMpsseSpi->SPI_SetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, pChipSelectsDisableStates, pHighInputOutputPinsData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetGPIOs(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  return pFT2232cMpsseSpi->SPI_GetGeneralPurposeHighInputOutputPins(ftHandle, pHighPinsInputData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetHiSpeedDeviceGPIOs(FTC_HANDLE ftHandle, PFTH_LOW_HIGH_PINS pHighPinsInputData)
{
  return pFT2232cMpsseSpi->SPI_GetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, pHighPinsInputData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_Write(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                            BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                            DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite, PWriteDataByteBuffer pWriteDataBuffer,
                            DWORD dwNumDataBytesToWrite, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  return pFT2232cMpsseSpi->SPI_WriteDataToExternalDevice(ftHandle, pWriteStartCondition, bClockOutDataBitsMSBFirst,
                                                         bClockOutDataBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                         dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite,
                                                         pWriteDataBuffer, dwNumDataBytesToWrite, pWaitDataWriteComplete,
                                                         pHighPinsWriteActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_WriteHiSpeedDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                         BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                         DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                         PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete,
                                         PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  return pFT2232cMpsseSpi->SPI_WriteDataToExternalDevice(ftHandle, pWriteStartCondition, bClockOutDataBitsMSBFirst,
                                                         bClockOutDataBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                         dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite,
                                                         pWriteDataBuffer, dwNumDataBytesToWrite, pWaitDataWriteComplete,
                                                         pHighPinsWriteActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_Read(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                           BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                           DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                           DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumDataBytesReturned,
                           PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  return pFT2232cMpsseSpi->SPI_ReadDataFromExternalDevice(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst,
                                                          bClockOutControlBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                          dwNumControlBytesToWrite, bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge,
                                                          dwNumDataBitsToRead, pReadDataBuffer, lpdwNumDataBytesReturned,
                                                          pHighPinsReadActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_ReadHiSpeedDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                        BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                        DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                        DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumDataBytesReturned,
                                        PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  return pFT2232cMpsseSpi->SPI_ReadDataFromExternalDevice(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst,
                                                          bClockOutControlBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                          dwNumControlBytesToWrite, bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge,
                                                          dwNumDataBitsToRead, pReadDataBuffer, lpdwNumDataBytesReturned,
                                                          pHighPinsReadActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_ClearDeviceCmdSequence(FTC_HANDLE ftHandle)
{
  return pFT2232cMpsseSpi->SPI_ClearDeviceCommandSequence(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_AddDeviceWriteCmd(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                        BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                        DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite, PWriteDataByteBuffer pWriteDataBuffer,
                                        DWORD dwNumDataBytesToWrite, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  return pFT2232cMpsseSpi->SPI_AddDeviceWriteCommand(ftHandle, pWriteStartCondition, bClockOutDataBitsMSBFirst,
                                                     bClockOutDataBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                     dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite, pWriteDataBuffer,
                                                     dwNumDataBytesToWrite, pHighPinsWriteActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_AddHiSpeedDeviceWriteCmd(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                               BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                               DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                               PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                               PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  return pFT2232cMpsseSpi->SPI_AddDeviceWriteCommand(ftHandle, pWriteStartCondition, bClockOutDataBitsMSBFirst,
                                                     bClockOutDataBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                     dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite, pWriteDataBuffer,
                                                     dwNumDataBytesToWrite, pHighPinsWriteActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_AddDeviceReadCmd(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                       BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                       DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                       DWORD dwNumDataBitsToRead, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  return pFT2232cMpsseSpi->SPI_AddDeviceReadCommand(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst,
                                                    bClockOutControlBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                    dwNumControlBytesToWrite, bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge,
                                                    dwNumDataBitsToRead, pHighPinsReadActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_AddHiSpeedDeviceReadCmd(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                              BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                              DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                              DWORD dwNumDataBitsToRead, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  return pFT2232cMpsseSpi->SPI_AddDeviceReadCommand(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst,
                                                    bClockOutControlBitsPosEdge, dwNumControlBitsToWrite, pWriteControlBuffer,
                                                    dwNumControlBytesToWrite, bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge,
                                                    dwNumDataBitsToRead, pHighPinsReadActiveStates);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_ExecuteDeviceCmdSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                               LPDWORD lpdwNumBytesReturned)
{
  return pFT2232cMpsseSpi->SPI_ExecuteDeviceCommandSequence(ftHandle, pReadCmdSequenceDataBuffer, lpdwNumBytesReturned);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize)
{
  return pFT2232cMpsseSpi->SPI_GetDllVersion(lpDllVersionBuffer, dwBufferSize);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI SPI_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode, LPSTR lpErrorMessageBuffer, DWORD dwBufferSize)
{
  return pFT2232cMpsseSpi->SPI_GetErrorCodeString(lpLanguage, StatusCode, lpErrorMessageBuffer, dwBufferSize);
}
//---------------------------------------------------------------------------

