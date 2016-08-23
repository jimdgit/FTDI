/*++

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    FT2232cMpsseSpi.cpp

Abstract:

    FT2232H and FT4232H Hi-Speed Dual Device and FT2232D Dual Device Class Declaration/Definition.

Environment:

    kernel & user mode

Revision History:

    04/07/05    kra     Created.
    03/06/08    kra     Added new method ie WriteGeneralPurposeHighInputOutputPins to control the output
                        states ie high(true) or low(false) of the 4 general purpose higher input/output
                        (ACBUS 0-3/GPIOH 0-3) pins, for the SPI_Write and SPI_AddDeviceWriteCmd functions.
    04/06/08    kra     Modified AddWriteCommandDataToOutPutBuffer method, to accomadate the fact that some
                        SPI devices do not require any control bits written to them, prior to a read operation.
    04/06/08    kra     Added new method ie ReadGeneralPurposeHighInputOutputPins, to read the input states of the
                        4 general purpose higher input/output (ACBUS 0-3/GPIOH 0-3) pins that have been previously
                        configured as input pin(s). Modified ReadDataFromExternalDevice method to call the new
                        ReadGeneralPurposeHighInputOutputPins method.
    21/08/08    kra     Added new functions for FT2232H and FT4232H hi-speed devices.
    04/09/08    kra     Added critical sections to every public method, to ensure that only one public method in the
                        DLL will be executed at a time, when a process/application has multiple threads running.

--*/

#define WIO_DEFINED

#include "stdafx.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "FT2232cMpsseSpi.h"

BOOL FT2232cMpsseSpi::IsClockCorrectInitialOutputState(PFTC_INIT_CONDITION pWriteReadStartCondition, BOOL bClockInOutDataBitsPosEdge)
{
  BOOL bClockCorrectInitialOutputState = TRUE;

  // Check that the clock initial state of high is not equal to the data being clocked out on a negative edge or
  // the clock initial state of low is not equal to the data being clocked out on a positive edge
  //if ((*pWriteReadStartCondition).bClockPinStateAfterChipSelect != bClockInOutDataBitsPosEdge)
  if ((*pWriteReadStartCondition).bClockPinState != bClockInOutDataBitsPosEdge)
    bClockCorrectInitialOutputState = FALSE;
  
  return bClockCorrectInitialOutputState;
}

// This function checks that the specified general purpose higher pins(GPIOH1 - GPIOH4) has been configured as outputs
// for a FT2232D dual device
BOOL FT2232cMpsseSpi::IsGeneralPurposeHighPinsGPIOH1To4CorrectOutputStates(FTC_HANDLE ftHandle, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteReadActiveStates)
{
  BOOL bHighPinsCorrectStates = TRUE;
  INT iCmdsSequenceDataDeviceIndex = 0;
  DWORD dwHighPinsDirection = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  // Checks that the specified general purpose higher pins(GPIOH1 - GPIOH4) have been configured as outputs
  dwHighPinsDirection = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection;

  if ((*pHighPinsWriteReadActiveStates).bPin1State != FALSE)
  {
    if ((dwHighPinsDirection & PIN1_HIGH_VALUE) != PIN1_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin2State != FALSE))
  {
    if ((dwHighPinsDirection & PIN2_HIGH_VALUE) != PIN2_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin3State != FALSE))
  {
    if ((dwHighPinsDirection & PIN3_HIGH_VALUE) != PIN3_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }
  
  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin4State != FALSE))
  {
    if ((dwHighPinsDirection & PIN4_HIGH_VALUE) != PIN4_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  return bHighPinsCorrectStates;
}

// This function checks that the specified general purpose higher pins(GPIOH1 - GPIOH8) has been configured as outputs
// for a FT2232H or FT4323H hi-speed device
BOOL FT2232cMpsseSpi::IsGeneralPurposeHighPinsGPIOH1To8CorrectOutputStates(FTC_HANDLE ftHandle, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteReadActiveStates)
{
  BOOL bHighPinsCorrectStates = TRUE;
  INT iCmdsSequenceDataDeviceIndex = 0;
  DWORD dwHighPinsDirection = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  // Checks that the specified general purpose higher pins(GPIOH1 - GPIOH8) have been configured as outputs
  dwHighPinsDirection = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection;

  if ((*pHighPinsWriteReadActiveStates).bPin1State != FALSE)
  {
    if ((dwHighPinsDirection & PIN1_HIGH_VALUE) != PIN1_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin2State != FALSE))
  {
    if ((dwHighPinsDirection & PIN2_HIGH_VALUE) != PIN2_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin3State != FALSE))
  {
    if ((dwHighPinsDirection & PIN3_HIGH_VALUE) != PIN3_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }
  
  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin4State != FALSE))
  {
    if ((dwHighPinsDirection & PIN4_HIGH_VALUE) != PIN4_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin5State != FALSE))
  {
    if ((dwHighPinsDirection & PIN5_HIGH_VALUE) != PIN5_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin6State != FALSE))
  {
    if ((dwHighPinsDirection & PIN6_HIGH_VALUE) != PIN6_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin7State != FALSE))
  {
    if ((dwHighPinsDirection & PIN7_HIGH_VALUE) != PIN7_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  if ((bHighPinsCorrectStates != FALSE) && ((*pHighPinsWriteReadActiveStates).bPin8State != FALSE))
  {
    if ((dwHighPinsDirection & PIN8_HIGH_VALUE) != PIN8_HIGH_VALUE)
      bHighPinsCorrectStates = FALSE;
  }

  return bHighPinsCorrectStates;
}

// This function checks that the specified general purpose higher pin(GPIOH1 - GPIOH4) has been configured as an input
// for a FT2232D dual device
BOOL FT2232cMpsseSpi::IsGeneralPurposeGPIOH1To4PinsCorrectInputState(FTC_HANDLE ftHandle, DWORD dwWaitDataWritePin)
{
  BOOL bHighPinCorrectInputState = TRUE;
  INT iCmdsSequenceDataDeviceIndex = 0;
  DWORD dwHighPinsDirection = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  // Checks that the specified general purpose higher pin(GPIOH1 - GPIOH4) has been configured as an input
  dwHighPinsDirection = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection;

  switch (dwWaitDataWritePin)
  {
    case ACBUS0GPIOH1:
      if ((dwHighPinsDirection & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS1GPIOH2:
      if ((dwHighPinsDirection & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS2GPIOH3:
      if ((dwHighPinsDirection & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS3GPIOH4:
      if ((dwHighPinsDirection & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    default:
      break;
  }
  
  return bHighPinCorrectInputState;
}

// This function checks that the specified general purpose higher pin(GPIOH1 - GPIOH8) has been configured as an input
// for a FT2232H or FT4323H hi-speed device
BOOL FT2232cMpsseSpi::IsGeneralPurposeGPIOH1To8PinsCorrectInputState(FTC_HANDLE ftHandle, DWORD dwWaitDataWritePin)
{
  BOOL bHighPinCorrectInputState = TRUE;
  INT iCmdsSequenceDataDeviceIndex = 0;
  DWORD dwHighPinsDirection = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  // Checks that the specified general purpose higher pin(GPIOH1 - GPIOH8) has been configured as an input
  dwHighPinsDirection = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection;

  switch (dwWaitDataWritePin)
  {
    case ACBUS0GPIOH1:
      if ((dwHighPinsDirection & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS1GPIOH2:
      if ((dwHighPinsDirection & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS2GPIOH3:
      if ((dwHighPinsDirection & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS3GPIOH4:
      if ((dwHighPinsDirection & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS4GPIOH5:
      if ((dwHighPinsDirection & PIN5_HIGH_VALUE) == PIN5_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS5GPIOH6:
      if ((dwHighPinsDirection & PIN6_HIGH_VALUE) == PIN6_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS6GPIOH7:
      if ((dwHighPinsDirection & PIN7_HIGH_VALUE) == PIN7_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    case ACBUS7GPIOH8:
      if ((dwHighPinsDirection & PIN8_HIGH_VALUE) == PIN8_HIGH_VALUE)
        bHighPinCorrectInputState = FALSE;
      break;
    default:
      break;
  }
  
  return bHighPinCorrectInputState;
}

FTC_STATUS FT2232cMpsseSpi::CheckWriteDataToExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsPosEdge,
                                                                     DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                                     DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits,
                                                                     DWORD dwNumDataBitsToWrite, PWriteDataByteBuffer pWriteDataBuffer,
                                                                     DWORD dwNumDataBytesToWrite)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumControlBytesToBeWritten = 0;
  DWORD dwNumRemainingControlBits = 0;
  DWORD dwNumDataBytesToBeWritten = 0;
  DWORD dwNumRemainingDataBits = 0;

  if ((pWriteStartCondition != NULL) && (pWriteControlBuffer != NULL))
  {
    if ((*pWriteStartCondition).dwChipSelectPin <= ADBUS7GPIOL4)
    {
      if (IsClockCorrectInitialOutputState(pWriteStartCondition, bClockOutDataBitsPosEdge) == TRUE)
      {
        if ((dwNumControlBitsToWrite >= MIN_NUM_CONTROL_BITS) && (dwNumControlBitsToWrite <= MAX_NUM_CONTROL_BITS))
        {
          if ((dwNumControlBytesToWrite >= MIN_NUM_CONTROL_BYTES) && (dwNumControlBytesToWrite <= MAX_NUM_CONTROL_BYTES))
          {
            dwNumControlBytesToBeWritten = (dwNumControlBitsToWrite / NUMBITSINBYTE);

            dwNumRemainingControlBits = (dwNumControlBitsToWrite % 8);

            if (dwNumRemainingControlBits > 0)
              dwNumControlBytesToBeWritten = (dwNumControlBytesToBeWritten + 1);

            if (dwNumControlBytesToWrite >= dwNumControlBytesToBeWritten)
            {
              if (bWriteDataBits != FALSE)
              {
                if (pWriteDataBuffer != NULL)
                {
                  if ((dwNumDataBitsToWrite >= MIN_NUM_WRITE_DATA_BITS) && (dwNumDataBitsToWrite <= MAX_NUM_WRITE_DATA_BITS))
                  {
                    if ((dwNumDataBytesToWrite >= MIN_NUM_WRITE_DATA_BYTES) && (dwNumDataBytesToWrite <= MAX_NUM_WRITE_DATA_BYTES))
                    {
                      dwNumDataBytesToBeWritten = (dwNumDataBitsToWrite / NUMBITSINBYTE);

                      dwNumRemainingDataBits = (dwNumDataBitsToWrite % 8);

                      if (dwNumRemainingDataBits > 0)
                        dwNumDataBytesToBeWritten = (dwNumDataBytesToBeWritten + 1);

                      if (dwNumDataBytesToWrite < dwNumDataBytesToBeWritten)
                        Status = FTC_NUMBER_WRITE_DATA_BYTES_TOO_SMALL;
                    }
                    else
                      Status = FTC_INVALID_NUMBER_WRITE_DATA_BYTES;
                  }
                  else
                    Status = FTC_INVALID_NUMBER_WRITE_DATA_BITS;
                }
                else
                  Status = FTC_NULL_WRITE_DATA_BUFFER_POINTER;
              }
            }
            else
              Status = FTC_NUMBER_CONTROL_BYTES_TOO_SMALL;
          }
          else
            Status = FTC_INVALID_NUMBER_CONTROL_BYTES;
        }
        else
          Status = FTC_INVALID_NUMBER_CONTROL_BITS;
      }
      else
        Status = FTC_INVALID_INIT_CLOCK_PIN_STATE;
    }
    else
      Status = FTC_INVALID_FT2232C_CHIP_SELECT_PIN;
  }
  else
  {
    if (pWriteStartCondition == NULL)
      Status = FTC_NULL_INITIAL_CONDITION_BUFFER_POINTER;
    else
      Status = FTC_NULL_WRITE_CONTROL_BUFFER_POINTER;
  }

  return Status;
}

// This function checks the write data parameters specific to a device connected to FT2232D dual device
FTC_STATUS FT2232cMpsseSpi::CheckWriteDataToExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if (pHighPinsWriteActiveStates != NULL)
  {
    if (IsGeneralPurposeHighPinsGPIOH1To4CorrectOutputStates(ftHandle, pHighPinsWriteActiveStates) == TRUE)
    {
      if (pWaitDataWriteComplete != NULL)
      {
        if ((*pWaitDataWriteComplete).bWaitDataWriteComplete != FALSE)
        {
          if ((*pWaitDataWriteComplete).dwWaitDataWritePin <= ACBUS3GPIOH4)
          {
            if ((*pWaitDataWriteComplete).dwWaitDataWritePin > ADBUS2DataIn)
            {
              if (IsGeneralPurposeGPIOH1To4PinsCorrectInputState(ftHandle, (*pWaitDataWriteComplete).dwWaitDataWritePin) == FALSE)
                Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
            }
          }
          else
            Status = FTC_INVALID_FT2232C_DATA_WRITE_COMPLETE_PIN;
        }
      }
      else
        Status = FTC_NULL_WAIT_DATA_WRITE_BUFFER_POINTER;
    }
    else
      Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
  }
  else
    Status = FTC_NULL_OUTPUT_PINS_BUFFER_POINTER;

  return Status;
}

// This function checks the write data parameters specific to a device connected to FT2232H or FT4323H hi-speed device
FTC_STATUS FT2232cMpsseSpi::CheckWriteDataToExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if (pHighPinsWriteActiveStates != NULL)
  {
    if (IsGeneralPurposeHighPinsGPIOH1To8CorrectOutputStates(ftHandle, pHighPinsWriteActiveStates) == TRUE)
    {
      if (pWaitDataWriteComplete != NULL)
      {
        if ((*pWaitDataWriteComplete).bWaitDataWriteComplete != FALSE)
        {
          if ((*pWaitDataWriteComplete).dwWaitDataWritePin <= ACBUS3GPIOH4)
          {
            if ((*pWaitDataWriteComplete).dwWaitDataWritePin > ADBUS2DataIn)
            {
              if (IsGeneralPurposeGPIOH1To8PinsCorrectInputState(ftHandle, (*pWaitDataWriteComplete).dwWaitDataWritePin) == FALSE)
                Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
            }
          }
          else
            Status = FTC_INVALID_FT2232C_DATA_WRITE_COMPLETE_PIN;
        }
      }
      else
        Status = FTC_NULL_WAIT_DATA_WRITE_BUFFER_POINTER;
    }
    else
      Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
  }
  else
    Status = FTC_NULL_OUTPUT_PINS_BUFFER_POINTER;

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::CheckReadDataFromExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition,
                                                                      BOOL bClockInDataBitsPosEdge, DWORD dwNumControlBitsToWrite,
                                                                      PWriteControlByteBuffer pWriteControlBuffer, DWORD dwNumControlBytesToWrite, 
                                                                      DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumControlBytesToBeWritten = 0;
  DWORD dwNumRemainingControlBits = 0;

  if ((pReadStartCondition != NULL) && (pWriteControlBuffer != NULL))
  {
    if ((*pReadStartCondition).dwChipSelectPin <= ADBUS7GPIOL4)
    {
      if (IsClockCorrectInitialOutputState(pReadStartCondition, bClockInDataBitsPosEdge) == TRUE)
      {
        if ((dwNumControlBitsToWrite >= MIN_NUM_CONTROL_BITS) && (dwNumControlBitsToWrite <= MAX_NUM_CONTROL_BITS))
        {
          if ((dwNumControlBytesToWrite >= MIN_NUM_CONTROL_BYTES) && (dwNumControlBytesToWrite <= MAX_NUM_CONTROL_BYTES))
          {
            dwNumControlBytesToBeWritten = (dwNumControlBitsToWrite / NUMBITSINBYTE);

            dwNumRemainingControlBits = (dwNumControlBitsToWrite % 8);

            if (dwNumRemainingControlBits > 0)
              dwNumControlBytesToBeWritten = (dwNumControlBytesToBeWritten + 1);

            if (dwNumControlBytesToWrite >= dwNumControlBytesToBeWritten)
            {
              if (pReadDataBuffer != NULL)
              {
                if ((dwNumDataBitsToRead < MIN_NUM_READ_DATA_BITS) || (dwNumDataBitsToRead > MAX_NUM_READ_DATA_BITS))
                  Status = FTC_INVALID_NUMBER_READ_DATA_BITS;
              }
              else
                Status = FTC_NULL_READ_DATA_BUFFER_POINTER;
            }
            else
              Status = FTC_NUMBER_CONTROL_BYTES_TOO_SMALL;
          }
          else
            Status = FTC_INVALID_NUMBER_CONTROL_BYTES;
        }
        else
          Status = FTC_INVALID_NUMBER_CONTROL_BITS;
      }
      else
        Status = FTC_INVALID_INIT_CLOCK_PIN_STATE;
    }
    else
      Status = FTC_INVALID_FT2232C_CHIP_SELECT_PIN;
  }
  else
  {
    if (pReadStartCondition == NULL)
      Status = FTC_NULL_INITIAL_CONDITION_BUFFER_POINTER;
    else
      Status = FTC_NULL_WRITE_CONTROL_BUFFER_POINTER;
  }

  return Status;
}

void FT2232cMpsseSpi::AddByteToOutputBuffer(DWORD dwOutputByte, BOOL bClearOutputBuffer)
{
  DWORD dwNumBytesToSend = 0;

  if (iCommandsSequenceDataDeviceIndex == -1)
    FTC_AddByteToOutputBuffer(dwOutputByte, bClearOutputBuffer);
  else
  {
    // This is used when you are building up a sequence of commands ie write and read
    dwNumBytesToSend = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend;

    (*OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pCommandsSequenceDataOutPutBuffer)[dwNumBytesToSend] = (dwOutputByte & '\xFF');

    dwNumBytesToSend = dwNumBytesToSend + 1;

    OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend = dwNumBytesToSend;
  }
}

FTC_STATUS FT2232cMpsseSpi::SetTCKTDITMSPinsCloseState(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwSavedLowPinsValue = 0;

  dwSavedLowPinsValue = GetDeviceSavedLowPinsValue(ftHandle);

  if ((pCloseFinalStatePinsData->bTCKPinState != FALSE) ||  
      (pCloseFinalStatePinsData->bTDIPinState != FALSE) ||
      (pCloseFinalStatePinsData->bTMSPinState != FALSE)) {
    AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, TRUE);

    if (pCloseFinalStatePinsData->bTCKPinState != FALSE) {
      if (pCloseFinalStatePinsData->bTCKPinActiveState != FALSE)
        dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x01'); // Set TCK pin high
      else
        dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFE'); // Set TCK pin low
    }

    if (pCloseFinalStatePinsData->bTDIPinState != FALSE) {
      if (pCloseFinalStatePinsData->bTDIPinActiveState != FALSE)
        dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x02'); // Set TDI pin high
      else
        dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFD'); // Set TDI pin low
    }

    if (pCloseFinalStatePinsData->bTMSPinState != FALSE) {
      if (pCloseFinalStatePinsData->bTMSPinActiveState != FALSE)
        dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x08'); // Set TMS pin high
      else
        dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xF7'); // Set TMS pin low
    }
    
    AddByteToOutputBuffer(dwSavedLowPinsValue, FALSE);
    // set TCK/SK, TDI/DO, TMS/CS and GPIOL1-4 as output, set TD0/D1 ie ADBUS2 as input
    AddByteToOutputBuffer('\xFB', FALSE);

    Status = FTC_SendBytesToDevice(ftHandle);
  }

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

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
      Sleep(20); // wait for all the USB stuff to complete

    if (Status == FTC_SUCCESS)
      InitGeneralPurposeInputOutputPins(ftHandle);

    if (Status == FTC_SUCCESS)
      Status = InitDataInOutClockFrequency(ftHandle, dwClockDivisor);

    if (Status == FTC_SUCCESS)
      Status = FTC_SetDeviceLoopbackState(ftHandle, FALSE);

    if (Status == FTC_SUCCESS)
      Status = EmptyDeviceInputBuffer(ftHandle);

    if (Status == FTC_SUCCESS)
      Sleep(30); // wait for all the USB stuff to complete
  }
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SetDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;
  
  AddByteToOutputBuffer(SET_CLOCK_FREQUENCY_CMD, TRUE);
  AddByteToOutputBuffer(dwClockDivisor, FALSE);
  AddByteToOutputBuffer((dwClockDivisor >> 8), FALSE);

  Status = FTC_SendBytesToDevice(ftHandle);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::InitDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  //Sleep(0); // give up timeslice

  SetDataInOutClockFrequency(ftHandle, dwClockDivisor);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::EmptyDeviceInputBuffer(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  DWORD dwNumBytesRead = 0;
  InputByteBuffer InputBuffer;

  // Get the number of bytes in the device input buffer
  Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

  if ((Status == FTC_SUCCESS) && (dwNumBytesDeviceInputBuffer > 0))
     FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

  return Status;
}

void FT2232cMpsseSpi::SaveGeneralPurposeHighPinsInputOutputStates(FTC_HANDLE ftHandle, DWORD dwHighPinsDirection, DWORD dwHighPinsValue)
{
  INT iCmdsSequenceDataDeviceIndex = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection = dwHighPinsDirection;
  OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsValue = dwHighPinsValue;
}

FTC_STATUS FT2232cMpsseSpi::SetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, DWORD dwHighPinsDirection, DWORD dwHighPinsValue)
{
  FTC_STATUS Status = FTC_SUCCESS;

  // output on the general purpose I/O high pins 1-4 or 1-8
  AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, TRUE);

  if (FTC_IsDeviceHiSpeedType(ftHandle) == FALSE) {
    dwHighPinsValue = (dwHighPinsValue & '\x0F');

    dwHighPinsDirection = (dwHighPinsDirection & '\x0F');
  } else {
    dwHighPinsValue = (dwHighPinsValue & '\xFF');

    dwHighPinsDirection = (dwHighPinsDirection & '\xFF');
  }

  AddByteToOutputBuffer(dwHighPinsValue, FALSE);
  AddByteToOutputBuffer(dwHighPinsDirection, FALSE);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS)
    SaveGeneralPurposeHighPinsInputOutputStates(ftHandle, dwHighPinsDirection, dwHighPinsValue);

  return Status;
}

// This function sets the initial input/output states of the 4 general purpose higher pins(GPIOH1 - GPIOH4)
// for a FT2232D dual device
void FT2232cMpsseSpi::WriteGeneralPurposeHighInputOutputGPIOH1To4Pins(FTC_HANDLE ftHandle, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  INT iCmdsSequenceDataDeviceIndex = 0;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  dwHighPinsDirection = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection;
  dwHighPinsValue = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsValue;

  AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, FALSE);

  if ((*pHighPinsWriteActiveStates).bPin1State != FALSE)
  {
    // Pin 1(ACBUS0/GPIOH0) of the 4 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin1ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH1_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xFE');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin2State != FALSE)
  {
    // Pin 2(ACBUS1/GPIOH1) of the 4 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin2ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH2_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xFD');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin3State != FALSE)
  {
    // Pin 3(ACBUS2/GPIOH2) of the 4 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin3ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH3_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xFB');
    }
  }
  
  if ((*pHighPinsWriteActiveStates).bPin4State != FALSE)
  {
    // Pin 4(ACBUS3/GPIOH3) of the 4 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin4ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH4_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xF7');
    }
  }

  dwHighPinsValue = (dwHighPinsValue & '\x0F');
  AddByteToOutputBuffer(dwHighPinsValue, FALSE);

  dwHighPinsDirection = (dwHighPinsDirection & '\x0F');
  AddByteToOutputBuffer(dwHighPinsDirection, FALSE);

  SaveGeneralPurposeHighPinsInputOutputStates(ftHandle, dwHighPinsDirection, dwHighPinsValue);
}

// This function sets the initial input/output states of the 8 general purpose higher pins(GPIOH1 - GPIOH8)
// for a FT2232H or FT4323H hi-speed device
void FT2232cMpsseSpi::WriteGeneralPurposeHighInputOutputGPIOH1To8Pins(FTC_HANDLE ftHandle, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  INT iCmdsSequenceDataDeviceIndex = 0;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  iCmdsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  dwHighPinsDirection = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsDirection;
  dwHighPinsValue = OpenedDevicesCommandsSequenceData[iCmdsSequenceDataDeviceIndex].dwSavedHighPinsValue;

  AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, FALSE);

  if ((*pHighPinsWriteActiveStates).bPin1State != FALSE)
  {
    // Pin 1(ACBUS0/GPIOH0) of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin1ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH1_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xFE');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin2State != FALSE)
  {
    // Pin 2(ACBUS1/GPIOH1) of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin2ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH2_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xFD');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin3State != FALSE)
  {
    // Pin 3(ACBUS2/GPIOH2) of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin3ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH3_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xFB');
    }
  }
  
  if ((*pHighPinsWriteActiveStates).bPin4State != FALSE)
  {
    // Pin 4(ACBUS3/GPIOH3) of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin4ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH4_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xF7');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin5State != FALSE)
  {
    // Pin 5(ACBUS4/GPIOH4 of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin5ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH5_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xEF');
    }
  }
  
  if ((*pHighPinsWriteActiveStates).bPin6State != FALSE)
  {
    // Pin 6(ACBUS5/GPIOH5 of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin6ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH6_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xDF');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin7State != FALSE)
  {
    // Pin 7(ACBUS6/GPIOH6 of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin7ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH7_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\xBF');
    }
  }

  if ((*pHighPinsWriteActiveStates).bPin8State != FALSE)
  {
    // Pin 8(ACBUS7/GPIOH7 of the 8 general purpose higher input/output pins has been set to an output, set
    // its active state to high(true) or low(false)
    if ((*pHighPinsWriteActiveStates).bPin8ActiveState != FALSE) {
      dwHighPinsValue = (dwHighPinsValue | GPIOH8_PIN_HIGH_VALUE);
    } else {
      dwHighPinsValue = (dwHighPinsValue & '\x7F');
    }
  }

  dwHighPinsValue = (dwHighPinsValue & '\xFF');
  AddByteToOutputBuffer(dwHighPinsValue, FALSE);

  dwHighPinsDirection = (dwHighPinsDirection & '\xFF');
  AddByteToOutputBuffer(dwHighPinsDirection, FALSE);

  SaveGeneralPurposeHighPinsInputOutputStates(ftHandle, dwHighPinsDirection, dwHighPinsValue);
}

FTC_STATUS FT2232cMpsseSpi::SetChipSelectPinsDisableStates(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLowPinsValue = 0;

  dwLowPinsValue = GetDeviceSavedLowPinsValue(ftHandle);

  AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, TRUE);

  if (pChipSelectsDisableStates->bADBUS3ChipSelectPinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | CHIP_SELECT_PIN_HIGH_VALUE); // set CS to high

  if (pChipSelectsDisableStates->bADBUS4GPIOL1PinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | GPIOL1_PIN_HIGH_VALUE); // set GPIOL1 to high

  if (pChipSelectsDisableStates->bADBUS5GPIOL2PinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | GPIOL2_PIN_HIGH_VALUE); // set GPIOL2 to high

  if (pChipSelectsDisableStates->bADBUS6GPIOL3PinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | GPIOL3_PIN_HIGH_VALUE); // set GPIOL3 to high

  if (pChipSelectsDisableStates->bADBUS7GPIOL4PinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | GPIOL4_PIN_HIGH_VALUE); // set GPIOL4 to high

  // set SK and DO to high, set CS and GPIOL1-4 to low
  AddByteToOutputBuffer(dwLowPinsValue, FALSE);

  // set SK, DO, CS and GPIOL1-4 as output, set D1 ie ADBUS2 as input
  AddByteToOutputBuffer('\xFB', FALSE);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS)
    InsertDeviceSavedLowPinsValue(ftHandle, dwLowPinsValue);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::InitGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, TRUE);

  // set SK and DO to low, set CS and GPIOL1-4 to low
  AddByteToOutputBuffer('\x00', FALSE);

  // set SK, DO, CS and GPIOL1-4 as output, set D1 ie ADBUS2 as input
  AddByteToOutputBuffer('\xFB', FALSE);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    // set SK and DO to low, set CS and GPIOL1-4 to low
    InsertDeviceSavedLowPinsValue(ftHandle, '\x00');

    if (FTC_IsDeviceHiSpeedType(ftHandle) == FALSE) {
      // set GPIOH1-4 as outputs and high state
      Status = SetGeneralPurposeHighInputOutputPins(ftHandle, '\x0F', '\x0F');
    } else {
      // set GPIOH1-8 as outputs and high state
      Status = SetGeneralPurposeHighInputOutputPins(ftHandle, '\xFF', '\xFF');
    }
  }

  return Status;
}

// This function gets the input states of the 4 general purpose higher input/output (ACBUS 0-3/GPIOH 0-3) pins
void FT2232cMpsseSpi::GetGeneralPurposeHighInputOutputGPIOH1To4PinsInputStates(DWORD dwInputStatesReturnedValue, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  if ((dwInputStatesReturnedValue & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
    pHighPinsInputData->bPin1LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
    pHighPinsInputData->bPin2LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
    pHighPinsInputData->bPin3LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
    pHighPinsInputData->bPin4LowHighState = TRUE;
}

// This function gets the states of the 4 general purpose higher input/output (ACBUS 0-3/GPIOH 0-3) pins
FTC_STATUS FT2232cMpsseSpi::GetGeneralPurposeHighInputOutputGPIOH1To4Pins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;

  pHighPinsInputData->bPin1LowHighState = FALSE;
  pHighPinsInputData->bPin2LowHighState = FALSE;
  pHighPinsInputData->bPin3LowHighState = FALSE;
  pHighPinsInputData->bPin4LowHighState = FALSE;

  // Put in this small delay incase the application programmer does a get GPIOs immediately after a set GPIOs
  Sleep(5);

  // Get the number of bytes in the device input buffer
  Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumBytesDeviceInputBuffer > 0)
      Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

    if (Status == FTC_SUCCESS)
    {
      // get the states of the general purpose I/O high pins 1-4
      AddByteToOutputBuffer(GET_HIGH_BYTE_DATA_BITS_CMD, TRUE);
      AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);
      Status = FTC_SendBytesToDevice(ftHandle);

      if (Status == FTC_SUCCESS)
      {
        Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer);

        if (Status == FTC_SUCCESS)
        {
          Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

          if (Status == FTC_SUCCESS)
            GetGeneralPurposeHighInputOutputGPIOH1To4PinsInputStates(InputBuffer[0], pHighPinsInputData);
        }
      }
    }
  }

  return Status;
}

void  FT2232cMpsseSpi::GetHiSpeedDeviceGeneralPurposeInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTH_LOW_HIGH_PINS pPinsInputData)
{
  if ((dwInputStatesReturnedValue & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
    pPinsInputData->bPin1LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
    pPinsInputData->bPin2LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
    pPinsInputData->bPin3LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
    pPinsInputData->bPin4LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN5_HIGH_VALUE) == PIN5_HIGH_VALUE)
    pPinsInputData->bPin5LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN6_HIGH_VALUE) == PIN6_HIGH_VALUE)
    pPinsInputData->bPin6LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN7_HIGH_VALUE) == PIN7_HIGH_VALUE)
    pPinsInputData->bPin7LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN8_HIGH_VALUE) == PIN8_HIGH_VALUE)
    pPinsInputData->bPin8LowHighState = TRUE;
}

FTC_STATUS FT2232cMpsseSpi::GetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, PFTH_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;
  BOOL bHiSpeedFT2232HTDeviceype = FALSE;

  pHighPinsInputData->bPin1LowHighState = FALSE;
  pHighPinsInputData->bPin2LowHighState = FALSE;
  pHighPinsInputData->bPin3LowHighState = FALSE;
  pHighPinsInputData->bPin4LowHighState = FALSE;
  pHighPinsInputData->bPin5LowHighState = FALSE;
  pHighPinsInputData->bPin6LowHighState = FALSE;
  pHighPinsInputData->bPin7LowHighState = FALSE;
  pHighPinsInputData->bPin8LowHighState = FALSE;

  // Put in this small delay incase the application programmer does a get GPIOs immediately after a set GPIOs
  Sleep(5);

  if ((Status = FTC_IsDeviceHiSpeedFT2232HType(ftHandle, &bHiSpeedFT2232HTDeviceype)) == FTC_SUCCESS)
  {
    // If the device is a FT2232H hi-speed device
    if (bHiSpeedFT2232HTDeviceype == TRUE)
    {
      // Get the number of bytes in the device input buffer
      if ((Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer)) == FTC_SUCCESS)
      {
        if (dwNumBytesDeviceInputBuffer > 0)
          Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

        if (Status == FTC_SUCCESS)
        {
          // get the states of the general purpose I/O high pins 1-8
          AddByteToOutputBuffer(GET_HIGH_BYTE_DATA_BITS_CMD, TRUE);
          AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);
          Status = FTC_SendBytesToDevice(ftHandle);

          if (Status == FTC_SUCCESS)
          {
            if ((Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer)) == FTC_SUCCESS)
            {
              if ((Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead)) == FTC_SUCCESS)
                GetHiSpeedDeviceGeneralPurposeInputOutputPinsInputStates(InputBuffer[0], pHighPinsInputData);
            }
          }
        }
      }
    }
  }

  return Status;
}

// This function reads the input states of the 4 general purpose higher input/output (ACBUS 0-3/GPIOH 0-3) pins
// that have been previously configured as input pin(s)
FTC_STATUS FT2232cMpsseSpi::ReadGeneralPurposeHighInputOutputGPIOH1To4Pins(FTC_HANDLE ftHandle, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FTC_LOW_HIGH_PINS HighPinsInputData;

  Status = GetGeneralPurposeHighInputOutputGPIOH1To4Pins(ftHandle, &HighPinsInputData);

  if (Status == FTC_SUCCESS)
  {
    if ((*pHighPinsReadActiveStates).bPin1State != FALSE)
    {
      // Pin 1(ACBUS0/GPIOH0) of the 4 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin1ActiveState = HighPinsInputData.bPin1LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin2State != FALSE)
    {
      // Pin 2(ACBUS1/GPIOH1) of the 4 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin2ActiveState = HighPinsInputData.bPin2LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin3State != FALSE)
    {
      // Pin 3(ACBUS2/GPIOH2) of the 4 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin3ActiveState = HighPinsInputData.bPin3LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin4State != FALSE)
    {
      // Pin 4(ACBUS3/GPIOH3) of the 4 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin4ActiveState = HighPinsInputData.bPin4LowHighState;
    }
  }

  return Status;
}

// This function reads the input states of the 8 general purpose higher input/output (ACBUS 0-7/GPIOH 0-7) pins
// that have been previously configured as input pin(s)
FTC_STATUS FT2232cMpsseSpi::ReadGeneralPurposeHighInputOutputGPIOH1To8Pins(FTC_HANDLE ftHandle, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FTH_LOW_HIGH_PINS HighPinsInputData;

  (*pHighPinsReadActiveStates).bPin1ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin2ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin3ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin4ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin5ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin6ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin7ActiveState = FALSE;
  (*pHighPinsReadActiveStates).bPin8ActiveState = FALSE;

  Status = GetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, &HighPinsInputData);

  if (Status == FTC_SUCCESS)
  {
    if ((*pHighPinsReadActiveStates).bPin1State != FALSE)
    {
      // Pin 1(ACBUS0/GPIOH0) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin1ActiveState = HighPinsInputData.bPin1LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin2State != FALSE)
    {
      // Pin 2(ACBUS1/GPIOH1) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin2ActiveState = HighPinsInputData.bPin2LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin3State != FALSE)
    {
      // Pin 3(ACBUS2/GPIOH2) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin3ActiveState = HighPinsInputData.bPin3LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin4State != FALSE)
    {
      // Pin 4(ACBUS3/GPIOH3) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin4ActiveState = HighPinsInputData.bPin4LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin5State != FALSE)
    {
      // Pin 5(ACBUS4/GPIOH4) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin5ActiveState = HighPinsInputData.bPin5LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin6State != FALSE)
    {
      // Pin 6(ACBUS5/GPIOH5) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin6ActiveState = HighPinsInputData.bPin6LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin7State != FALSE)
    {
      // Pin 7(ACBUS6/GPIOH6) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin7ActiveState = HighPinsInputData.bPin7LowHighState;
    }

    if ((*pHighPinsReadActiveStates).bPin8State != FALSE)
    {
      // Pin 8(ACBUS7/GPIOH7) of the 8 general purpose higher input/output pins has been set to an input, get
      // its active state ie high(true) or low(false)
      (*pHighPinsReadActiveStates).bPin8ActiveState = HighPinsInputData.bPin8LowHighState;
    }
  }

  return Status;
}

void FT2232cMpsseSpi::InsertDeviceSavedLowPinsValue(FTC_HANDLE ftHandle, DWORD dwLowPinsValue)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceHandleFound = FALSE;
  DWORD dwDeviceFoundIndex = 0;
  BOOL bDeviceLowPinsValueInserted = FALSE;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = TRUE;

        dwDeviceFoundIndex = dwDeviceIndex;
      }
    }
  }
  else
  {
    // This code is executed if there is only one device connected to the system, this code is here just in case
    // that a device was unplugged from the system, while the system was still running
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice != 0)
      {
        bDeviceHandleFound = TRUE;

        dwDeviceFoundIndex = dwDeviceIndex;
      }
    }
  }

  if (bDeviceHandleFound != FALSE)
    OpenedDevicesSavedLowPinsValues[dwDeviceFoundIndex].dwSavedLowPinsValue = dwLowPinsValue; 
  else
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceLowPinsValueInserted); dwDeviceIndex++)
    {
      if (OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice == 0)
      {
        OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice = ftHandle;

        OpenedDevicesSavedLowPinsValues[dwDeviceIndex].dwSavedLowPinsValue = dwLowPinsValue;

        bDeviceLowPinsValueInserted = TRUE;
      }
    }
  }
}

DWORD FT2232cMpsseSpi::GetDeviceSavedLowPinsValue(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceHandleFound = FALSE;
  DWORD dwSavedLowPinsValue = 0;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = TRUE;

        dwSavedLowPinsValue = OpenedDevicesSavedLowPinsValues[dwDeviceIndex].dwSavedLowPinsValue;
      }
    }
  }
  else
  {
    // This code is executed if there is only one device connected to the system, this code is here just in case
    // that a device was unplugged from the system, while the system was still running
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice != 0)
      {
        bDeviceHandleFound = TRUE;

        dwSavedLowPinsValue = OpenedDevicesSavedLowPinsValues[dwDeviceIndex].dwSavedLowPinsValue;
      }
    }
  }

  return dwSavedLowPinsValue;
}

void FT2232cMpsseSpi::DeleteDeviceSavedLowPinsValue(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceHandleFound = FALSE;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
  {
    if (OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice == ftHandle)
    {
      bDeviceHandleFound = TRUE;

      OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice = 0;
    }
  }
}

DWORD FT2232cMpsseSpi::SetStartCondition(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pStartCondition)
{
  DWORD dwLowPinsValue = 0;

  dwLowPinsValue = GetDeviceSavedLowPinsValue(ftHandle);

  //AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);

  //if (pStartCondition->bClockPinStateBeforeChipSelect != FALSE)
  if (pStartCondition->bClockPinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | CLOCK_SIGNAL_PIN_HIGH_VALUE); // set SK to high

  if (pStartCondition->bDataOutPinState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | DATA_OUT_PIN_HIGH_VALUE); // set DO to high

  // Set chip select pin to disable value 
  switch (pStartCondition->dwChipSelectPin)
  {
    case ADBUS3ChipSelect:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | CHIP_SELECT_PIN_HIGH_VALUE); // set CS to high
      break;
    case ADBUS4GPIOL1:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL1_PIN_HIGH_VALUE); // set GPIOL1 to high
      break;
    case ADBUS5GPIOL2:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL2_PIN_HIGH_VALUE); // set GPIOL2 to high
      break;
    case ADBUS6GPIOL3:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL3_PIN_HIGH_VALUE); // set GPIOL3 to high
      break;
    case ADBUS7GPIOL4:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL4_PIN_HIGH_VALUE); // set GPIOL4 to high
      break;
    default:
      break;
  }

  //AddByteToOutputBuffer(dwLowPinsValue, false);

  // set SK, DO, CS and GPIOL1-4 as output, set D1 as input
  //AddByteToOutputBuffer('\xFB', false);

  return dwLowPinsValue;
}

void FT2232cMpsseSpi::GetClockDataOutCommands(BOOL bClockOutDataBitsMSBFirst, BOOL bClockOutDataBitsPosEdge,
                                              LPDWORD lpdwDataWriteBytesCommand, LPDWORD lpdwDataWriteBitsCommand)
{
  if (bClockOutDataBitsMSBFirst != FALSE)
  {
    if (bClockOutDataBitsPosEdge != FALSE)
    {
      *lpdwDataWriteBytesCommand = CLK_DATA_BYTES_OUT_ON_POS_CLK_MSB_FIRST_CMD;
      *lpdwDataWriteBitsCommand = CLK_DATA_BITS_OUT_ON_POS_CLK_MSB_FIRST_CMD;
    }
    else
    {
      *lpdwDataWriteBytesCommand = CLK_DATA_BYTES_OUT_ON_NEG_CLK_MSB_FIRST_CMD;
      *lpdwDataWriteBitsCommand = CLK_DATA_BITS_OUT_ON_NEG_CLK_MSB_FIRST_CMD;
    }
  }
  else
  {
    if (bClockOutDataBitsPosEdge != FALSE)
    {
      *lpdwDataWriteBytesCommand = CLK_DATA_BYTES_OUT_ON_POS_CLK_LSB_FIRST_CMD;
      *lpdwDataWriteBitsCommand = CLK_DATA_BITS_OUT_ON_POS_CLK_LSB_FIRST_CMD;
    }
    else
    {
      *lpdwDataWriteBytesCommand = CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD;
      *lpdwDataWriteBitsCommand = CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD;
    }
  }
}

void FT2232cMpsseSpi::EnableSPIChip(PFTC_INIT_CONDITION pStartCondition, DWORD dwLowPinsValue)
{
  AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, FALSE);

  // Set chip select pin to enable value 
  switch (pStartCondition->dwChipSelectPin)
  {
    case ADBUS3ChipSelect:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue & CHIP_SELECT_PIN_LOW_VALUE); // set CS to low
      else
        dwLowPinsValue = (dwLowPinsValue | CHIP_SELECT_PIN_HIGH_VALUE); // set CS to high
      break;
    case ADBUS4GPIOL1:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue & GPIOL1_PIN_LOW_VALUE); // set GPIOL1 to low
      else
        dwLowPinsValue = (dwLowPinsValue | GPIOL1_PIN_HIGH_VALUE); // set GPIOL1 to high
      break;
    case ADBUS5GPIOL2:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue & GPIOL2_PIN_LOW_VALUE); // set GPIOL2 to low
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL2_PIN_HIGH_VALUE); // set GPIOL2 to high
      break;
    case ADBUS6GPIOL3:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue & GPIOL3_PIN_LOW_VALUE); // set GPIOL3 to low
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL3_PIN_HIGH_VALUE); // set GPIOL3 to high
      break;
    case ADBUS7GPIOL4:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue & GPIOL4_PIN_LOW_VALUE); // set GPIOL4 to low
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL4_PIN_HIGH_VALUE); // set GPIOL4 to high
      break;
    default:
      break;
  }

  AddByteToOutputBuffer(dwLowPinsValue, FALSE);

  // set SK, DO, CS and GPIOL1-4 as output, set D1 as input
  AddByteToOutputBuffer('\xFB', FALSE);

  //AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);

  //if (pStartCondition->bClockPinStateAfterChipSelect != FALSE)
  //  dwLowPinsValue = (dwLowPinsValue | CLOCK_SIGNAL_PIN_HIGH_VALUE); // set SK to high
  //else
  //  dwLowPinsValue = (dwLowPinsValue & CLOCK_SIGNAL_PIN_LOW_VALUE); // set SK to low

  //AddByteToOutputBuffer(dwLowPinsValue, false);

  // set SK, DO, CS and GPIOL1-4 as output, set D1 as input
  //AddByteToOutputBuffer('\xFB', false);
}

void FT2232cMpsseSpi::DisableSPIChip(PFTC_INIT_CONDITION pStartCondition, DWORD dwLowPinsValue)
{
  AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, FALSE);

  // Set chip select pin to disable value 
  switch (pStartCondition->dwChipSelectPin)
  {
    case ADBUS3ChipSelect:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | CHIP_SELECT_PIN_HIGH_VALUE); // set CS to high
      else
        dwLowPinsValue = (dwLowPinsValue & CHIP_SELECT_PIN_LOW_VALUE); // set CS to low
      break;
    case ADBUS4GPIOL1:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL1_PIN_HIGH_VALUE); // set GPIOL1 to high
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL1_PIN_LOW_VALUE); // set GPIOL1 to low
      break;
    case ADBUS5GPIOL2:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL2_PIN_HIGH_VALUE); // set GPIOL2 to high
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL2_PIN_LOW_VALUE); // set GPIOL2 to low
      break;
    case ADBUS6GPIOL3:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL3_PIN_HIGH_VALUE); // set GPIOL3 to high
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL3_PIN_LOW_VALUE); // set GPIOL3 to low
      break;
    case ADBUS7GPIOL4:
      if (pStartCondition->bChipSelectPinState != FALSE)
        dwLowPinsValue = (dwLowPinsValue | GPIOL4_PIN_HIGH_VALUE); // set GPIOL4 to high
      else
        dwLowPinsValue = (dwLowPinsValue & GPIOL4_PIN_LOW_VALUE); // set GPIOL4 to low
      break;
    default:
      break;
  }

  AddByteToOutputBuffer(dwLowPinsValue, FALSE);

  // set SK, DO, CS and GPIOL1-4 as output, set D1 as input
  AddByteToOutputBuffer('\xFB', FALSE);
}

void FT2232cMpsseSpi::AddWriteCommandDataToOutPutBuffer(PFTC_INIT_CONDITION pWriteStartCondition, DWORD dwNumControlBitsToWrite,
                                                        PWriteControlByteBuffer pWriteControlBuffer, BOOL bWriteDataBits,
                                                        DWORD dwNumDataBitsToWrite, PWriteDataByteBuffer pWriteDataBuffer,
                                                        DWORD dwDataWriteBytesCommand, DWORD dwDataWriteBitsCommand)
{
  DWORD dwModNumControlBitsToWrite = 0;
  DWORD dwControlBufferIndex = 0;
  DWORD dwNumControlBytes = 0;
  DWORD dwNumRemainingControlBits = 0;
  DWORD dwModNumDataBitsToWrite = 0;
  DWORD dwDataBufferIndex = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;

  // kra - 040608, added test for number of control bits to write, because for SPI only, the number of control
  // bits to write can be 0 on some SPI devices, before a read operation is performed
  if (dwNumControlBitsToWrite > 1)
  {
    // adjust for bit count of 1 less than no of bits
    dwModNumControlBitsToWrite = (dwNumControlBitsToWrite - 1);

    // Number of control bytes is greater than 0, only if the minimum number of control bits is 8
    dwNumControlBytes = (dwModNumControlBitsToWrite / 8);

    if (dwNumControlBytes > 0)
    {
      // Number of whole bytes
      dwNumControlBytes = (dwNumControlBytes - 1);

      // clk data bytes out
      AddByteToOutputBuffer(dwDataWriteBytesCommand, FALSE);
      AddByteToOutputBuffer((dwNumControlBytes & '\xFF'), FALSE);
      AddByteToOutputBuffer(((dwNumControlBytes / 256) & '\xFF'), FALSE);

      // now add the data bytes to go out
      do
      {
        AddByteToOutputBuffer((*pWriteControlBuffer)[dwControlBufferIndex], FALSE);
        dwControlBufferIndex = (dwControlBufferIndex + 1);
      }
      while (dwControlBufferIndex < (dwNumControlBytes + 1));
    }

    dwNumRemainingControlBits = (dwModNumControlBitsToWrite % 8);

    // do remaining bits
    if (dwNumRemainingControlBits > 0)
    {
      // clk data bits out
      AddByteToOutputBuffer(dwDataWriteBitsCommand, FALSE);
      AddByteToOutputBuffer((dwNumRemainingControlBits & '\xFF'), FALSE);
      AddByteToOutputBuffer((*pWriteControlBuffer)[dwControlBufferIndex], FALSE);
    }
  }

  if ((bWriteDataBits != FALSE) && (dwNumDataBitsToWrite > 0))
  {
    // adjust for bit count of 1 less than no of bits
    dwModNumDataBitsToWrite = (dwNumDataBitsToWrite - 1);

    dwNumDataBytes = (dwModNumDataBitsToWrite / 8);

    if (dwNumDataBytes > 0)
    {
      // Number of whole bytes
      dwNumDataBytes = (dwNumDataBytes - 1);

      // clk data bytes out
      AddByteToOutputBuffer(dwDataWriteBytesCommand, FALSE);
      AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), FALSE);
      AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), FALSE);

      // now add the data bytes to go out
      do
      {
        AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], FALSE);
        dwDataBufferIndex = (dwDataBufferIndex + 1);
      }
      while (dwDataBufferIndex < (dwNumDataBytes + 1));
    }

    dwNumRemainingDataBits = (dwModNumDataBitsToWrite % 8);

    // do remaining bits
    if (dwNumRemainingDataBits > 0)
    {
      // clk data bits out
      AddByteToOutputBuffer(dwDataWriteBitsCommand, FALSE);
      AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), FALSE);
      AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], FALSE);
    }
  }
}

void FT2232cMpsseSpi::SavedAddWriteCommandDataToOutPutBuffer(PFTC_INIT_CONDITION pWriteStartCondition, DWORD dwNumControlBitsToWrite,
                                                        PWriteControlByteBuffer pWriteControlBuffer, BOOL bWriteDataBits,
                                                        DWORD dwNumDataBitsToWrite, PWriteDataByteBuffer pWriteDataBuffer,
                                                        DWORD dwDataWriteBytesCommand, DWORD dwDataWriteBitsCommand)
{
  DWORD dwControlBufferIndex = 0;
  DWORD dwNumControlBytes = 0;
  DWORD dwNumRemainingControlBits = 0;
  DWORD dwModNumDataBitsToWrite = 0;
  DWORD dwDataBufferIndex = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;

  dwNumControlBytes = (dwNumControlBitsToWrite / 8);

  if (dwNumControlBytes > 0)
  {
    // Number of whole bytes
    dwNumControlBytes = (dwNumControlBytes - 1);

    // clk data bytes out
    AddByteToOutputBuffer(dwDataWriteBytesCommand, FALSE);
    AddByteToOutputBuffer((dwNumControlBytes & '\xFF'), FALSE);
    AddByteToOutputBuffer(((dwNumControlBytes / 256) & '\xFF'), FALSE);

    // now add the data bytes to go out
    do
    {
      AddByteToOutputBuffer((*pWriteControlBuffer)[dwControlBufferIndex], FALSE);
      dwControlBufferIndex = (dwControlBufferIndex + 1);
    }
    while (dwControlBufferIndex < (dwNumControlBytes + 1));
  }

  dwNumRemainingControlBits = (dwNumControlBitsToWrite % 8);

  // do remaining bits
  if (dwNumRemainingControlBits > 0)
  {
    dwNumRemainingControlBits = (dwNumRemainingControlBits - 1);

    // clk data bits out
    AddByteToOutputBuffer(dwDataWriteBitsCommand, FALSE);
    AddByteToOutputBuffer((dwNumRemainingControlBits & '\xFF'), FALSE);
    AddByteToOutputBuffer((*pWriteControlBuffer)[dwControlBufferIndex], FALSE);
  }

  if ((bWriteDataBits != FALSE) && (dwNumDataBitsToWrite > 0))
  {
    dwNumDataBytes = (dwNumDataBitsToWrite / 8);

    if (dwNumDataBytes > 0)
    {
      // Number of whole bytes
      dwNumDataBytes = (dwNumDataBytes - 1);

      // clk data bytes out
      AddByteToOutputBuffer(dwDataWriteBytesCommand, FALSE);
      AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), FALSE);
      AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), FALSE);

      // now add the data bytes to go out
      do
      {
        AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], FALSE);
        dwDataBufferIndex = (dwDataBufferIndex + 1);
      }
      while (dwDataBufferIndex < (dwNumDataBytes + 1));
    }

    dwNumRemainingDataBits = (dwNumDataBitsToWrite % 8);

    // do remaining bits
    if (dwNumRemainingDataBits > 0)
    {
      dwNumRemainingControlBits = (dwNumRemainingControlBits - 1);

      // clk data bits out
      AddByteToOutputBuffer(dwDataWriteBitsCommand, FALSE);
      AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), FALSE);
      AddByteToOutputBuffer((*pWriteDataBuffer)[dwDataBufferIndex], FALSE);
    }
  }
}

void FT2232cMpsseSpi::GetDataWriteCompletePinState(PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, LPDWORD lpdwDataWriteCompletePin, LPDWORD lpdwDataWriteCompleteStateValue)
{
  *lpdwDataWriteCompletePin = 0;
  *lpdwDataWriteCompleteStateValue = 0;

  // Set pin to detect when all bytes have been written to an external device 
  switch (pWaitDataWriteComplete->dwWaitDataWritePin)
  {
    case ADBUS2DataIn:
      *lpdwDataWriteCompletePin = SERIAL_DATA_IN_PIN_HIGH_VALUE;

      if (pWaitDataWriteComplete->bDataWriteCompleteState != FALSE)
        *lpdwDataWriteCompleteStateValue = SERIAL_DATA_IN_PIN_HIGH_VALUE; // to check if SDI is high
      break;
    case ACBUS0GPIOH1:
      *lpdwDataWriteCompletePin = GPIOH1_PIN_HIGH_VALUE;

      if (pWaitDataWriteComplete->bDataWriteCompleteState != FALSE)
        *lpdwDataWriteCompleteStateValue = GPIOH1_PIN_HIGH_VALUE; // to check if GPIOH1 is high
      break;
    case ACBUS1GPIOH2:
      *lpdwDataWriteCompletePin = GPIOH2_PIN_HIGH_VALUE;

      if (pWaitDataWriteComplete->bDataWriteCompleteState != FALSE)
        *lpdwDataWriteCompleteStateValue = GPIOH2_PIN_HIGH_VALUE; // to check if GPIOH2 is high
      break;
    case ACBUS2GPIOH3:
      *lpdwDataWriteCompletePin = GPIOH3_PIN_HIGH_VALUE;

      if (pWaitDataWriteComplete->bDataWriteCompleteState != FALSE)
        *lpdwDataWriteCompleteStateValue = GPIOH3_PIN_HIGH_VALUE; // to check if GPIOH3 is high
      break;
    case ACBUS3GPIOH4:
      *lpdwDataWriteCompletePin = GPIOH4_PIN_HIGH_VALUE;

      if (pWaitDataWriteComplete->bDataWriteCompleteState != FALSE)
        *lpdwDataWriteCompleteStateValue = GPIOH4_PIN_HIGH_VALUE; // to check if GPIOH4 is high
      break;
    default:
      break;
  }
}

FTC_STATUS FT2232cMpsseSpi::CheckDataWrittenToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLowPinsValue = 0;
  BOOL bFirstTimeRoundLoop = TRUE;
  SYSTEMTIME StartTime;
  DWORD dwNumDataBytesToRead = 0;
  DWORD dwDataWriteCompletePin = 0;
  DWORD dwDataWriteCompleteStateValue = 0;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  DWORD dwNumDataBytesRead = 0;
  InputByteBuffer InputBuffer;
  BOOL bDataWriteComplete = FALSE;
  DWORD dwReadValue = 0;

  FTC_ClearOutputBuffer();

  dwLowPinsValue = SetStartCondition(ftHandle, pWriteStartCondition);

  EnableSPIChip(pWriteStartCondition, dwLowPinsValue);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    GetLocalTime(&StartTime);

    GetNumDataBytesToRead(8, &dwNumDataBytesToRead);

    GetDataWriteCompletePinState(pWaitDataWriteComplete, &dwDataWriteCompletePin, &dwDataWriteCompleteStateValue);

    do
    {
      Sleep(0);

      // Get the number of bytes in the device input buffer
      Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

      if (Status == FTC_SUCCESS)
      {
        if (dwNumBytesDeviceInputBuffer > 0)
          FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumDataBytesRead);
      }
    }
    while ((dwNumBytesDeviceInputBuffer > 0) && (Status == FTC_SUCCESS));

    do
    {
      if (bFirstTimeRoundLoop == TRUE)
        bFirstTimeRoundLoop = FALSE;
      else
        FTC_ClearOutputBuffer();

      if (pWaitDataWriteComplete->dwWaitDataWritePin == ADBUS2DataIn)
        AddByteToOutputBuffer(GET_LOW_BYTE_DATA_BITS_CMD, FALSE);
      else
        AddByteToOutputBuffer(GET_HIGH_BYTE_DATA_BITS_CMD, FALSE);

      AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);

      Status = FTC_SendBytesToDevice(ftHandle);

      if (Status == FTC_SUCCESS)
      {
        // Get the number of bytes in the device input buffer
        Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

        if (Status == FTC_SUCCESS)
        {
          //Sleep(0);  // give up timeslice

          if (dwNumBytesDeviceInputBuffer > 0)
            Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumDataBytesRead);
          else
            dwNumDataBytesRead = 0;

          if (Status == FTC_SUCCESS)
          {
            if (dwNumDataBytesRead > 0)
            {
              dwReadValue = (InputBuffer[0] & dwDataWriteCompletePin);

              if (dwReadValue == dwDataWriteCompleteStateValue)
                bDataWriteComplete = TRUE;
            }

            if (bDataWriteComplete == FALSE)
            {
              Sleep(0);  // give up timeslice
              if (FTC_Timeout(StartTime, pWaitDataWriteComplete->dwDataWriteTimeoutmSecs))
                Status = FTC_DATA_WRITE_COMPLETE_TIMEOUT;
            }
          }
        }
      }
    }
    while ((bDataWriteComplete == FALSE) && (Status == FTC_SUCCESS));

    if (Status == FTC_SUCCESS)
    {
      FTC_ClearOutputBuffer();

      DisableSPIChip(pWriteStartCondition, dwLowPinsValue);

      Status = FTC_SendBytesToDevice(ftHandle);
    }
  }

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::WriteDataToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                      BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                      DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                      PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                                      PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLowPinsValue = 0;
  DWORD dwDataWriteBytesCommand;
  DWORD dwDataWriteBitsCommand;

  dwLowPinsValue = SetStartCondition(ftHandle, pWriteStartCondition);

  GetClockDataOutCommands(bClockOutDataBitsMSBFirst, bClockOutDataBitsPosEdge, &dwDataWriteBytesCommand, &dwDataWriteBitsCommand);

  EnableSPIChip(pWriteStartCondition, dwLowPinsValue);

  AddWriteCommandDataToOutPutBuffer(pWriteStartCondition, dwNumControlBitsToWrite, pWriteControlBuffer, bWriteDataBits,
                                    dwNumDataBitsToWrite, pWriteDataBuffer, dwDataWriteBytesCommand, dwDataWriteBitsCommand);

  DisableSPIChip(pWriteStartCondition, dwLowPinsValue);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((bWriteDataBits != FALSE) && (pWaitDataWriteComplete->bWaitDataWriteComplete != FALSE))
      Status = CheckDataWrittenToExternalDevice(ftHandle, pWriteStartCondition, pWaitDataWriteComplete);
  }

  return Status;
}

void FT2232cMpsseSpi::GetNumDataBytesToRead(DWORD dwNumDataBitsToRead, LPDWORD lpdwNumDataBytesToRead)
{
  DWORD dwModNumDataBitsToRead = 0;
  DWORD dwNumDataBytesToRead = 0;
  DWORD dwNumRemainingDataBits = 0;

  // adjust for bit count of 1 less than no of bits
  dwModNumDataBitsToRead = (dwNumDataBitsToRead - 1);

  // Number of whole bytes to read
  dwNumDataBytesToRead = (dwModNumDataBitsToRead / 8);

  // number of remaining bits
  dwNumRemainingDataBits = (dwModNumDataBitsToRead % 8);

  // increase the number of whole bytes if bits left over
  if (dwNumRemainingDataBits > 0)
    dwNumDataBytesToRead = (dwNumDataBytesToRead + 1);

  *lpdwNumDataBytesToRead = dwNumDataBytesToRead;
}

// This will work out the number of whole bytes to read
FTC_STATUS FT2232cMpsseSpi::GetDataFromExternalDevice(FTC_HANDLE ftHandle, DWORD dwNumDataBitsToRead,
                                                      PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumDataBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  InputByteBuffer InputBuffer;
  DWORD dwBytesReadIndex = 0;

  GetNumDataBytesToRead(dwNumDataBitsToRead, &dwNumDataBytes);

  Status = FTC_ReadFixedNumBytesFromDevice(ftHandle, &InputBuffer, dwNumDataBytes, &dwNumDataBytesRead);

  if (Status == FTC_SUCCESS)
  {
    for (dwBytesReadIndex = 0 ; dwBytesReadIndex < dwNumDataBytes; dwBytesReadIndex++)
      (*pReadDataBuffer)[dwBytesReadIndex] = InputBuffer[dwBytesReadIndex];

    *lpdwNumBytesReturned = dwNumDataBytes;
  }

  return Status;
}

void FT2232cMpsseSpi::AddReadCommandToOutputBuffer(PFTC_INIT_CONDITION pReadStartCondition, DWORD dwNumDataBitsToRead,
                                                   DWORD dwDataReadBytesCommand, DWORD dwDataReadBitsCommand)
{
  DWORD dwModNumBitsToRead = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;

  // adjust for bit count of 1 less than no of bits
  dwModNumBitsToRead = (dwNumDataBitsToRead - 1);

  dwNumDataBytes = (dwModNumBitsToRead / 8);

  if (dwNumDataBytes > 0)
  {
    // Number of whole bytes
    dwNumDataBytes = (dwNumDataBytes - 1);

    // clk data bytes out
    AddByteToOutputBuffer(dwDataReadBytesCommand, FALSE);
    AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), FALSE);
    AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), FALSE);
  }

  // number of remaining bits
  dwNumRemainingDataBits = (dwModNumBitsToRead % 8);

  if (dwNumRemainingDataBits > 0)
  {
    // clk data bits out
    AddByteToOutputBuffer(dwDataReadBitsCommand, FALSE);
    AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), FALSE);
  }
}

void FT2232cMpsseSpi::GetClockDataInCommands(BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                             LPDWORD lpdwDataReadBytesCommand, LPDWORD lpdwDataReadBitsCommand)
{
  if (bClockInDataBitsMSBFirst != FALSE)
  {
    if (bClockInDataBitsPosEdge != FALSE)
    {
      *lpdwDataReadBytesCommand = CLK_DATA_BYTES_IN_ON_POS_CLK_MSB_FIRST_CMD;
      *lpdwDataReadBitsCommand = CLK_DATA_BITS_IN_ON_POS_CLK_MSB_FIRST_CMD;
    }
    else
    {
      *lpdwDataReadBytesCommand = CLK_DATA_BYTES_IN_ON_NEG_CLK_MSB_FIRST_CMD;
      *lpdwDataReadBitsCommand = CLK_DATA_BITS_IN_ON_NEG_CLK_MSB_FIRST_CMD;
    }
  }
  else
  {
    if (bClockInDataBitsPosEdge != FALSE)
    {
      *lpdwDataReadBytesCommand = CLK_DATA_BYTES_IN_ON_POS_CLK_LSB_FIRST_CMD;
      *lpdwDataReadBitsCommand = CLK_DATA_BITS_IN_ON_POS_CLK_LSB_FIRST_CMD;
    }
    else
    {
      *lpdwDataReadBytesCommand = CLK_DATA_BYTES_IN_ON_NEG_CLK_LSB_FIRST_CMD;
      *lpdwDataReadBitsCommand = CLK_DATA_BITS_IN_ON_NEG_CLK_LSB_FIRST_CMD;
    }
  }
}

FTC_STATUS FT2232cMpsseSpi::ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition,
                                                       BOOL bClockOutControlBitsMSBFirst, BOOL bClockOutControlBitsPosEdge,
                                                       DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                       DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                                       DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer,
                                                       LPDWORD lpdwNumDataBytesReturned)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLowPinsValue = 0;
  DWORD dwDataWriteBytesCommand;
  DWORD dwDataWriteBitsCommand;
  DWORD dwDataReadBytesCommand;
  DWORD dwDataReadBitsCommand;

  *lpdwNumDataBytesReturned = 0;

  if (dwNumDataBitsToRead > 0)
  {
    dwLowPinsValue = SetStartCondition(ftHandle, pReadStartCondition);

    GetClockDataOutCommands(bClockOutControlBitsMSBFirst, bClockOutControlBitsPosEdge, &dwDataWriteBytesCommand, &dwDataWriteBitsCommand);

    if (Status == FTC_SUCCESS)
    {
      EnableSPIChip(pReadStartCondition, dwLowPinsValue);

      AddWriteCommandDataToOutPutBuffer(pReadStartCondition, dwNumControlBitsToWrite, pWriteControlBuffer,
                                        FALSE, 0, NULL, dwDataWriteBytesCommand, dwDataWriteBitsCommand);

      GetClockDataInCommands(bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge, &dwDataReadBytesCommand, &dwDataReadBitsCommand);

      AddReadCommandToOutputBuffer(pReadStartCondition, dwNumDataBitsToRead, dwDataReadBytesCommand, dwDataReadBitsCommand);

      DisableSPIChip(pReadStartCondition, dwLowPinsValue);

      AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);

      Status = FTC_SendBytesToDevice(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = GetDataFromExternalDevice(ftHandle, dwNumDataBitsToRead, pReadDataBuffer, lpdwNumDataBytesReturned);
    }
  }

  return Status;
}

DWORD FT2232cMpsseSpi::GetTotalNumCommandsSequenceDataBytesToRead(void)
{
  DWORD dwTotalNumBytesToBeRead = 0;
  DWORD CommandSequenceIndex = 0;
  PReadCommandsSequenceData pReadCommandsSequenceDataBuffer;
  DWORD dwNumReadCommandSequences = 0;
  PReadCommandSequenceData pReadCmdSequenceData;
  DWORD dwNumBitsToRead = 0;
  DWORD dwNumDataBytesToRead = 0;

  if (iCommandsSequenceDataDeviceIndex != -1)
  {
    pReadCommandsSequenceDataBuffer = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].pReadCommandsSequenceDataBuffer;
    dwNumReadCommandSequences = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences;

    for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwNumReadCommandSequences); CommandSequenceIndex++)
    {
      pReadCmdSequenceData = (*pReadCommandsSequenceDataBuffer)[CommandSequenceIndex];
      dwNumBitsToRead = (*pReadCmdSequenceData)[0];

      GetNumDataBytesToRead(dwNumBitsToRead, &dwNumDataBytesToRead);

      dwTotalNumBytesToBeRead = (dwTotalNumBytesToBeRead + dwNumDataBytesToRead);
    }
  }

  return dwTotalNumBytesToBeRead;
}

void FT2232cMpsseSpi::CopyReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pDestinationBuffer, PReadCommandsSequenceData pSourceBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer)
{
  DWORD CommandSequenceIndex = 0;
  PReadCommandSequenceData pReadCmdSequenceData;
  DWORD dwNumBitsToRead = 0;

  for (CommandSequenceIndex = 0; (CommandSequenceIndex < dwSizeReadCommandsSequenceDataBuffer); CommandSequenceIndex++)
  {
    pReadCmdSequenceData = (*pSourceBuffer)[CommandSequenceIndex];
    dwNumBitsToRead = (*pReadCmdSequenceData)[0];

    pReadCmdSequenceData = (*pDestinationBuffer)[CommandSequenceIndex];
    (*pReadCmdSequenceData)[0] = dwNumBitsToRead;
  }
}

FTC_STATUS FT2232cMpsseSpi::AddReadCommandSequenceData(DWORD dwNumDataBitsToRead)
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

      (*pReadCmdSequenceData)[0] = dwNumDataBitsToRead;

      dwNumReadCommandSequences = (dwNumReadCommandSequences + 1);

      OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumReadCommandSequences = dwNumReadCommandSequences;
    }
  }

  return Status;
}

PReadCommandsSequenceData FT2232cMpsseSpi::CreateReadCommandsSequenceDataBuffer(DWORD dwSizeReadCmdsSequenceDataBuffer)
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

void FT2232cMpsseSpi::DeleteReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pReadCmdsSequenceDataBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer)
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

FTC_STATUS FT2232cMpsseSpi::CreateDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceDataBuffersCreated = FALSE;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceDataBuffersCreated); dwDeviceIndex++)
  {
    if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == 0)
    {
      bDeviceDataBuffersCreated = TRUE;

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

  if ((Status == FTC_SUCCESS) && (bDeviceDataBuffersCreated == TRUE))
    dwNumOpenedDevices = dwNumOpenedDevices + 1;

  return Status;
}

void FT2232cMpsseSpi::ClearDeviceCommandSequenceData(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceHandleFound = FALSE;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = TRUE;

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
        bDeviceHandleFound = TRUE;

        OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumBytesToSend = 0;
        OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumReadCommandSequences = 0;
      }
    }
  }
}

DWORD FT2232cMpsseSpi::GetNumBytesInCommandsSequenceDataBuffer(void)
{
  DWORD dwNumBytesToSend = 0;

  if (iCommandsSequenceDataDeviceIndex != -1)
    // Get the number commands to be executed in sequence ie write, read and write/read
    dwNumBytesToSend = OpenedDevicesCommandsSequenceData[iCommandsSequenceDataDeviceIndex].dwNumBytesToSend;

  return dwNumBytesToSend;
}

DWORD FT2232cMpsseSpi::GetCommandsSequenceDataDeviceIndex(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceHandleFound = FALSE;
  INT iCmdsSequenceDataDeviceIndex = 0;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = TRUE;

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
        bDeviceHandleFound = TRUE;

        iCmdsSequenceDataDeviceIndex = dwDeviceIndex;
      }
    }
  }

  return iCmdsSequenceDataDeviceIndex;
}

void FT2232cMpsseSpi::DeleteDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceHandleFound = FALSE;
  POutputByteBuffer pCmdsSequenceDataOutPutBuffer;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
  {
    if (OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice == ftHandle)
    {
      bDeviceHandleFound = TRUE;

      OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice = 0;
      OpenedDevicesCommandsSequenceData[dwDeviceIndex].dwNumBytesToSend = 0;
      pCmdsSequenceDataOutPutBuffer = OpenedDevicesCommandsSequenceData[dwDeviceIndex].pCommandsSequenceDataOutPutBuffer;

      if (pCmdsSequenceDataOutPutBuffer != NULL)
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

FTC_STATUS FT2232cMpsseSpi::AddDeviceWriteCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                  BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                  DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                  PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumCommandDataBytes = 0;
  DWORD dwLowPinsValue = 0;
  DWORD dwDataWriteBytesCommand = 0;
  DWORD dwDataWriteBitsCommand = 0;
  //PFTC_WAIT_DATA_WRITE pWriteWaitDataComplete;

  dwNumCommandDataBytes = (NUM_WRITE_COMMAND_BYTES + dwNumControlBytesToWrite);

  if ((bWriteDataBits != FALSE) && (dwNumDataBitsToWrite > 0))
    dwNumCommandDataBytes = dwNumCommandDataBytes + (NUM_READ_COMMAND_BYTES + dwNumDataBytesToWrite);

  iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  if ((GetNumBytesInCommandsSequenceDataBuffer() + dwNumCommandDataBytes) < OUTPUT_BUFFER_SIZE)
  {
    dwLowPinsValue = SetStartCondition(ftHandle, pWriteStartCondition);

    GetClockDataOutCommands(bClockOutDataBitsMSBFirst, bClockOutDataBitsPosEdge, &dwDataWriteBytesCommand, &dwDataWriteBitsCommand);

    EnableSPIChip(pWriteStartCondition, dwLowPinsValue);

    AddWriteCommandDataToOutPutBuffer(pWriteStartCondition, dwNumControlBitsToWrite, pWriteControlBuffer, bWriteDataBits,
                                      dwNumDataBitsToWrite, pWriteDataBuffer, dwDataWriteBytesCommand, dwDataWriteBitsCommand);

    DisableSPIChip(pWriteStartCondition, dwLowPinsValue);

    //if (((bWriteDataBits != false) && (dwNumDataBitsToWrite > 0)) && (pWaitDataWriteComplete->bWaitDataWriteComplete != false))
    //  Status = CheckDataWrittenToExternalDevice(ftHandle, pWriteStartCondition, pWaitDataWriteComplete);
  }
  else
    Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;

  // Reset to indicate that you are not building up a sequence of commands
  iCommandsSequenceDataDeviceIndex = -1;

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::AddDeviceReadCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                                 BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                 DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                                 DWORD dwNumDataBitsToRead)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLowPinsValue = 0;
  DWORD dwDataWriteBytesCommand = 0;
  DWORD dwDataWriteBitsCommand = 0;
  DWORD dwDataReadBytesCommand = 0;
  DWORD dwDataReadBitsCommand = 0;

  iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

  if ((GetNumBytesInCommandsSequenceDataBuffer() + NUM_READ_COMMAND_BYTES) < OUTPUT_BUFFER_SIZE)
  {
    dwLowPinsValue = SetStartCondition(ftHandle, pReadStartCondition);

    GetClockDataOutCommands(bClockOutControlBitsMSBFirst, bClockOutControlBitsPosEdge, &dwDataWriteBytesCommand, &dwDataWriteBitsCommand);

    EnableSPIChip(pReadStartCondition, dwLowPinsValue);

    AddWriteCommandDataToOutPutBuffer(pReadStartCondition, dwNumControlBitsToWrite, pWriteControlBuffer,
                                      FALSE, 0, NULL, dwDataWriteBytesCommand, dwDataWriteBitsCommand);

    GetClockDataInCommands(bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge, &dwDataReadBytesCommand, &dwDataReadBitsCommand);

    AddReadCommandToOutputBuffer(pReadStartCondition, dwNumDataBitsToRead, dwDataReadBytesCommand, dwDataReadBitsCommand);

    DisableSPIChip(pReadStartCondition, dwLowPinsValue);

    Status = AddReadCommandSequenceData(dwNumDataBitsToRead);
  }
  else
    Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;

  // Reset to indicate that you are not building up a sequence of commands
  iCommandsSequenceDataDeviceIndex = -1;

  return Status;
}

FT2232cMpsseSpi::FT2232cMpsseSpi(void)
{
  DWORD dwDeviceIndex = 0;

  dwNumOpenedDevices = 0;

  for (dwDeviceIndex = 0; (dwDeviceIndex < MAX_NUM_DEVICES); dwDeviceIndex++)
  {
    OpenedDevicesSavedLowPinsValues[dwDeviceIndex].hDevice = 0;

    OpenedDevicesCommandsSequenceData[dwDeviceIndex].hDevice = 0;
  }

  iCommandsSequenceDataDeviceIndex = -1;

  InitializeCriticalSection(&threadAccess);
}

FT2232cMpsseSpi::~FT2232cMpsseSpi(void)
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

  DeleteCriticalSection(&threadAccess);
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetNumDevices(LPDWORD lpdwNumDevices)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FT2232CDeviceIndexes FT2232CIndexes;

  EnterCriticalSection(&threadAccess);

  *lpdwNumDevices = 0;

  Status = FTC_GetNumDevices(lpdwNumDevices, &FT2232CIndexes);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetNumHiSpeedDevices(LPDWORD lpdwNumHiSpeedDevices)
{
  FTC_STATUS Status = FTC_SUCCESS;
  HiSpeedDeviceIndexes HiSpeedIndexes;

  EnterCriticalSection(&threadAccess);

  *lpdwNumHiSpeedDevices = 0;

  Status = FTC_GetNumHiSpeedDevices(lpdwNumHiSpeedDevices, &HiSpeedIndexes);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetDeviceNameLocationID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_GetDeviceNameLocationID(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetHiSpeedDeviceNameLocIDChannel(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID, LPSTR lpChannel, DWORD dwChannelBufferSize, LPDWORD lpdwHiSpeedDeviceType)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwDeviceType = 0;

  EnterCriticalSection(&threadAccess);

  *lpdwHiSpeedDeviceType = 0;

  Status = FTC_GetHiSpeedDeviceNameLocationIDChannel(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID, lpChannel, dwChannelBufferSize, &dwDeviceType);

  if (Status == FTC_SUCCESS)
  {
    if ((dwDeviceType == FT_DEVICE_2232H) || (dwDeviceType == FT_DEVICE_4232H))
    {
      if (dwDeviceType == FT_DEVICE_2232H)
        *lpdwHiSpeedDeviceType = FT2232H_DEVICE_TYPE;
      else
        *lpdwHiSpeedDeviceType = FT4232H_DEVICE_TYPE;
    }
    else
      Status = FTC_DEVICE_NOT_FOUND;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_OpenDevice(FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_OpenDevice(pftHandle);

  if (Status == FTC_SUCCESS)
    Status = CreateDeviceCommandsSequenceDataBuffers(*pftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_OpenSpecifiedDevice(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_OpenSpecifiedDevice(lpDeviceName, dwLocationID, pftHandle);

  if (Status == FTC_SUCCESS)
    Status = CreateDeviceCommandsSequenceDataBuffers(*pftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_OpenSpecifiedHiSpeedDevice(LPSTR lpDeviceName, DWORD dwLocationID, LPSTR lpChannel, FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_OpenSpecifiedHiSpeedDevice(lpDeviceName, dwLocationID, lpChannel, pftHandle);

  if (Status == FTC_SUCCESS)
  {
    Status = CreateDeviceCommandsSequenceDataBuffers(*pftHandle);

    if (Status != FTC_SUCCESS)
    {
      FTC_CloseDevice(*pftHandle);

      *pftHandle = 0;
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetHiSpeedDeviceType(FTC_HANDLE ftHandle, LPDWORD lpdwHiSpeedDeviceType)
{
  FTC_STATUS Status = FTC_SUCCESS;
  BOOL bHiSpeedFT2232HTDeviceType = FALSE;

  EnterCriticalSection(&threadAccess);

  *lpdwHiSpeedDeviceType = 0;

  Status = FTC_GetHiSpeedDeviceType(ftHandle, &bHiSpeedFT2232HTDeviceType);

  if (Status == FTC_SUCCESS)
  {
    // Is the device a FT2232H hi-speed device
    if (bHiSpeedFT2232HTDeviceType == TRUE)
      *lpdwHiSpeedDeviceType = FT2232H_DEVICE_TYPE;
    else
      *lpdwHiSpeedDeviceType = FT4232H_DEVICE_TYPE;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_CloseDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_CloseDevice(ftHandle);

  DeleteDeviceSavedLowPinsValue(ftHandle);

  DeleteDeviceCommandsSequenceDataBuffers(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS WINAPI FT2232cMpsseSpi::SPI_CloseDevice(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if (pCloseFinalStatePinsData != NULL)
    {
      if ((Status = SetTCKTDITMSPinsCloseState(ftHandle, pCloseFinalStatePinsData)) == FTC_SUCCESS)
        Status = SPI_CloseDevice(ftHandle);
    }
    else
      Status = FTC_NULL_CLOSE_FINAL_STATE_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if ((Status = FTC_InitHiSpeedDevice(ftHandle)) == FTC_SUCCESS)
    {
      Status = InitDevice(ftHandle, dwClockDivisor);
    }
  }

  if (Status == FTC_INVALID_HANDLE)
  {
    if ((Status = FT2232c::FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
    {
      Status = InitDevice(ftHandle, dwClockDivisor);
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_TurnOnDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_TurnOnDivideByFiveClockingHiSpeedDevice(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_TurnOffDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_TurnOffDivideByFiveClockingHiSpeedDevice(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE LatencyTimermSec)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_SetDeviceLatencyTimer(ftHandle, LatencyTimermSec);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetDeviceLatencyTimer(FTC_HANDLE ftHandle, LPBYTE lpLatencyTimermSec)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_GetDeviceLatencyTimer(ftHandle, lpLatencyTimermSec);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    FTC_GetClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetHiSpeedDeviceClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    FTC_GetHiSpeedDeviceClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
  {
    if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
    {
      FTC_GetHiSpeedDeviceClockFrequencyValues(ftHandle, dwClockDivisor, lpdwClockFrequencyHz);
    }

    if (Status == FTC_INVALID_HANDLE)
    {
      if ((Status = FT2232c::FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
      {
        FTC_GetClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
      }
    }

    if (Status == FTC_SUCCESS)
    {
      Status = SetDataInOutClockFrequency(ftHandle, dwClockDivisor);
    }
  }
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_SetDeviceLoopbackState(FTC_HANDLE ftHandle, BOOL bLoopbackState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS) {
    Status = FTC_SetDeviceLoopbackState(ftHandle, bLoopbackState);
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_SetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates,
                                                                     PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  EnterCriticalSection(&threadAccess);

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if ((pChipSelectsDisableStates != NULL) && (pHighInputOutputPinsData != NULL))
    {
      Status = SetChipSelectPinsDisableStates(ftHandle, pChipSelectsDisableStates);

      if (Status == FTC_SUCCESS)
      {
        if (pHighInputOutputPinsData->bPin1InputOutputState != FALSE)
          dwHighPinsDirection = (dwHighPinsDirection | '\x01');
        if (pHighInputOutputPinsData->bPin2InputOutputState != FALSE)
          dwHighPinsDirection = (dwHighPinsDirection | '\x02');
        if (pHighInputOutputPinsData->bPin3InputOutputState != FALSE)
          dwHighPinsDirection = (dwHighPinsDirection | '\x04');
        if (pHighInputOutputPinsData->bPin4InputOutputState != FALSE)
          dwHighPinsDirection = (dwHighPinsDirection | '\x08');
        if (pHighInputOutputPinsData->bPin1LowHighState != FALSE)
          dwHighPinsValue = (dwHighPinsValue | '\x01');
        if (pHighInputOutputPinsData->bPin2LowHighState != FALSE)
          dwHighPinsValue = (dwHighPinsValue | '\x02');
        if (pHighInputOutputPinsData->bPin3LowHighState != FALSE)
          dwHighPinsValue = (dwHighPinsValue | '\x04');
        if (pHighInputOutputPinsData->bPin4LowHighState != FALSE)
          dwHighPinsValue = (dwHighPinsValue | '\x08');

        Status = SetGeneralPurposeHighInputOutputPins(ftHandle, dwHighPinsDirection, dwHighPinsValue);
      }
    }
    else
    {
      if (pChipSelectsDisableStates == NULL)
        Status = FTC_NULL_CHIP_SELECT_BUFFER_POINTER;
      else
        Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_SetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates,
                                                                              PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  BOOL bHiSpeedFT2232HTDeviceType = FALSE;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if ((pChipSelectsDisableStates != NULL) && (pHighInputOutputPinsData != NULL))
    {
      Status = SetChipSelectPinsDisableStates(ftHandle, pChipSelectsDisableStates);

      if (Status == FTC_SUCCESS)
      {
        if ((Status = FTC_IsDeviceHiSpeedFT2232HType(ftHandle, &bHiSpeedFT2232HTDeviceType)) == FTC_SUCCESS)
        {
          // If the device is a FT2232H hi-speed device
          if (bHiSpeedFT2232HTDeviceType == TRUE)
          {
            if (pHighInputOutputPinsData->bPin1InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x01');
            if (pHighInputOutputPinsData->bPin2InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x02');
            if (pHighInputOutputPinsData->bPin3InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x04');
            if (pHighInputOutputPinsData->bPin4InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x08');
            if (pHighInputOutputPinsData->bPin5InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x10');
            if (pHighInputOutputPinsData->bPin6InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x20');
            if (pHighInputOutputPinsData->bPin7InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x40');
            if (pHighInputOutputPinsData->bPin8InputOutputState != FALSE)
              dwHighPinsDirection = (dwHighPinsDirection | '\x80');

            if (pHighInputOutputPinsData->bPin1LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x01');
            if (pHighInputOutputPinsData->bPin2LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x02');
            if (pHighInputOutputPinsData->bPin3LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x04');
            if (pHighInputOutputPinsData->bPin4LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x08');
            if (pHighInputOutputPinsData->bPin5LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x10');
            if (pHighInputOutputPinsData->bPin6LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x20');
            if (pHighInputOutputPinsData->bPin7LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x40');
            if (pHighInputOutputPinsData->bPin8LowHighState != FALSE)
              dwHighPinsValue = (dwHighPinsValue | '\x80');

            Status = SetGeneralPurposeHighInputOutputPins(ftHandle, dwHighPinsDirection, dwHighPinsValue);
          }
        }
      }
    }
    else
    {
      if (pChipSelectsDisableStates == NULL)
        Status = FTC_NULL_CHIP_SELECT_BUFFER_POINTER;
      else
        Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pHighPinsInputData != NULL)
      Status = GetGeneralPurposeHighInputOutputGPIOH1To4Pins(ftHandle, pHighPinsInputData);
    else
      Status = FTC_NULL_INPUT_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, PFTH_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if (pHighPinsInputData != NULL)
      Status = GetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, pHighPinsInputData);
    else
      Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function writes data to a device connected to FT2232D dual device
FTC_STATUS FT2232cMpsseSpi::SPI_WriteDataToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                          BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                          DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                          PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                                          PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    Status = CheckWriteDataToExternalDeviceParameters(ftHandle, pWriteStartCondition, bClockOutDataBitsPosEdge, dwNumControlBitsToWrite,
                                                      pWriteControlBuffer, dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite,
                                                      pWriteDataBuffer, dwNumDataBytesToWrite);

    if (Status == FTC_SUCCESS) {
      if ((Status = CheckWriteDataToExternalDeviceParameters(ftHandle, pWaitDataWriteComplete, pHighPinsWriteActiveStates)) == FTC_SUCCESS) {
        FTC_ClearOutputBuffer();

        // kra - 03/06/08, added to control the (ACBUS 0-3/GPIOH 0-3) pins
        WriteGeneralPurposeHighInputOutputGPIOH1To4Pins(ftHandle, pHighPinsWriteActiveStates);

        Status = WriteDataToExternalDevice(ftHandle, pWriteStartCondition,  bClockOutDataBitsMSBFirst, bClockOutDataBitsPosEdge,
                                           dwNumControlBitsToWrite, pWriteControlBuffer, dwNumControlBytesToWrite,
                                           bWriteDataBits, dwNumDataBitsToWrite, pWriteDataBuffer, dwNumDataBytesToWrite,
                                           pWaitDataWriteComplete);
      }
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function writes data to a device connected to FT2232H or FT4232H hi-speed device
FTC_STATUS FT2232cMpsseSpi::SPI_WriteDataToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                          BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                          DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                          PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                                          PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    Status = CheckWriteDataToExternalDeviceParameters(ftHandle, pWriteStartCondition, bClockOutDataBitsPosEdge, dwNumControlBitsToWrite,
                                                      pWriteControlBuffer, dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite,
                                                      pWriteDataBuffer, dwNumDataBytesToWrite);

    if (Status == FTC_SUCCESS) {
      if ((Status = CheckWriteDataToExternalDeviceParameters(ftHandle, pWaitDataWriteComplete, pHighPinsWriteActiveStates)) == FTC_SUCCESS) {
        FTC_ClearOutputBuffer();

        // Control the (ACBUS 0-7/GPIOH 0-7) pins
        WriteGeneralPurposeHighInputOutputGPIOH1To8Pins(ftHandle, pHighPinsWriteActiveStates);

        Status = WriteDataToExternalDevice(ftHandle, pWriteStartCondition,  bClockOutDataBitsMSBFirst, bClockOutDataBitsPosEdge,
                                           dwNumControlBitsToWrite, pWriteControlBuffer, dwNumControlBytesToWrite,
                                           bWriteDataBits, dwNumDataBitsToWrite, pWriteDataBuffer, dwNumDataBytesToWrite,
                                           pWaitDataWriteComplete);
      }
    }
  }

  LeaveCriticalSection(&threadAccess);
  
  return Status;
}

// This function reads data to a device connected to FT2232D dual device
FTC_STATUS FT2232cMpsseSpi::SPI_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition,
                                                           BOOL bClockOutControlBitsMSBFirst, BOOL bClockOutControlBitsPosEdge,
                                                           DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                           DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst,
                                                           BOOL bClockInDataBitsPosEdge, DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer,
                                                           LPDWORD lpdwNumDataBytesReturned, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    Status = CheckReadDataFromExternalDeviceParameters(ftHandle, pReadStartCondition, bClockOutControlBitsPosEdge, dwNumControlBitsToWrite,
                                                       pWriteControlBuffer, dwNumControlBytesToWrite, dwNumDataBitsToRead,
                                                       pReadDataBuffer);

    if (Status == FTC_SUCCESS) {
      if (pHighPinsReadActiveStates != NULL)
      {
        if (IsGeneralPurposeHighPinsGPIOH1To4CorrectOutputStates(ftHandle, pHighPinsReadActiveStates) == FALSE)
          Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
      }
      else
        Status = FTC_NULL_OUTPUT_PINS_BUFFER_POINTER;

      if (Status == FTC_SUCCESS) {
        FTC_ClearOutputBuffer();

        // kra - 04/06/08, Added ReadGeneralPurposeHighInputOutputPins method, to read the input states of the 4 general
        // purpose higher input/output (ACBUS 0-3/GPIOH 0-3) pins that have been previously configured as input pin(s).
        if ((Status = ReadGeneralPurposeHighInputOutputGPIOH1To4Pins(ftHandle, pHighPinsReadActiveStates)) == FTC_SUCCESS)
          Status = ReadDataFromExternalDevice(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst, bClockOutControlBitsPosEdge,
                                              dwNumControlBitsToWrite, pWriteControlBuffer, dwNumControlBytesToWrite,
                                              bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge, dwNumDataBitsToRead, pReadDataBuffer,
                                              lpdwNumDataBytesReturned);
      }
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function reads data to a device connected to FT2232H or FT4232H hi-speed device
FTC_STATUS FT2232cMpsseSpi::SPI_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition,
                                                           BOOL bClockOutControlBitsMSBFirst, BOOL bClockOutControlBitsPosEdge,
                                                           DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                           DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst,
                                                           BOOL bClockInDataBitsPosEdge, DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer,
                                                           LPDWORD lpdwNumDataBytesReturned, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    Status = CheckReadDataFromExternalDeviceParameters(ftHandle, pReadStartCondition, bClockOutControlBitsPosEdge, dwNumControlBitsToWrite,
                                                       pWriteControlBuffer, dwNumControlBytesToWrite, dwNumDataBitsToRead,
                                                       pReadDataBuffer);

    if (Status == FTC_SUCCESS) {
      if (pHighPinsReadActiveStates != NULL)
      {
        if (IsGeneralPurposeHighPinsGPIOH1To8CorrectOutputStates(ftHandle, pHighPinsReadActiveStates) == FALSE)
          Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
      }
      else
        Status = FTC_NULL_OUTPUT_PINS_BUFFER_POINTER;

      if (Status == FTC_SUCCESS) {
        FTC_ClearOutputBuffer();

        // Read the input states of the 8 general purpose higher input/output (ACBUS 0-7/GPIOH 0-7) pins that
        // have been previously configured as input pin(s).
        if ((Status = ReadGeneralPurposeHighInputOutputGPIOH1To8Pins(ftHandle, pHighPinsReadActiveStates)) == FTC_SUCCESS)
          Status = ReadDataFromExternalDevice(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst, bClockOutControlBitsPosEdge,
                                              dwNumControlBitsToWrite, pWriteControlBuffer, dwNumControlBytesToWrite,
                                              bClockInDataBitsMSBFirst, bClockInDataBitsPosEdge, dwNumDataBitsToRead, pReadDataBuffer,
                                              lpdwNumDataBytesReturned);
      }
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_ClearDeviceCommandSequence(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
    ClearDeviceCommandSequenceData(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function adds a write data command to the command sequence buffer to write data to a device connected
// to a FT2232D dual device
FTC_STATUS FT2232cMpsseSpi::SPI_AddDeviceWriteCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                      BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                      DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                      PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                                      PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FTC_WAIT_DATA_WRITE WaitDataWriteComplete;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    WaitDataWriteComplete.bWaitDataWriteComplete = FALSE;
    WaitDataWriteComplete.dwWaitDataWritePin = ADBUS2DataIn;

    Status = CheckWriteDataToExternalDeviceParameters(ftHandle, pWriteStartCondition, bClockOutDataBitsPosEdge, dwNumControlBitsToWrite,
                                                      pWriteControlBuffer, dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite,
                                                      pWriteDataBuffer, dwNumDataBytesToWrite);

    if (Status == FTC_SUCCESS) {
      if ((Status = CheckWriteDataToExternalDeviceParameters(ftHandle, &WaitDataWriteComplete, pHighPinsWriteActiveStates)) == FTC_SUCCESS) {
        if ((GetNumBytesInCommandsSequenceDataBuffer() + NUM_GPIO_WRITE_COMMAND_BYTES) < OUTPUT_BUFFER_SIZE)
        {
          // kra - 03/06/08, added to control the (ACBUS 0-3/GPIOH 0-3) pins
          WriteGeneralPurposeHighInputOutputGPIOH1To4Pins(ftHandle, pHighPinsWriteActiveStates);

          AddDeviceWriteCommand(ftHandle, pWriteStartCondition, bClockOutDataBitsMSBFirst, bClockOutDataBitsPosEdge, dwNumControlBitsToWrite,
                                pWriteControlBuffer, dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite, pWriteDataBuffer,
                                dwNumDataBytesToWrite);
        }
        else
          Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;
      }
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function adds a write data command to the command sequence buffer to write data to a device connected
// to a FT2232H or FT4232H hi-speed device
FTC_STATUS FT2232cMpsseSpi::SPI_AddDeviceWriteCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                      BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                      DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                      PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                                      PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FTC_WAIT_DATA_WRITE WaitDataWriteComplete;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    WaitDataWriteComplete.bWaitDataWriteComplete = FALSE;
    WaitDataWriteComplete.dwWaitDataWritePin = ADBUS2DataIn;

    Status = CheckWriteDataToExternalDeviceParameters(ftHandle, pWriteStartCondition, bClockOutDataBitsPosEdge, dwNumControlBitsToWrite,
                                                      pWriteControlBuffer, dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite,
                                                      pWriteDataBuffer, dwNumDataBytesToWrite);

    if (Status == FTC_SUCCESS) {
      if ((Status = CheckWriteDataToExternalDeviceParameters(ftHandle, &WaitDataWriteComplete, pHighPinsWriteActiveStates)) == FTC_SUCCESS) {
        if ((GetNumBytesInCommandsSequenceDataBuffer() + NUM_GPIO_WRITE_COMMAND_BYTES) < OUTPUT_BUFFER_SIZE)
        {
          // Control the (ACBUS 0-7/GPIOH 0-7) pins
          WriteGeneralPurposeHighInputOutputGPIOH1To8Pins(ftHandle, pHighPinsWriteActiveStates);

          AddDeviceWriteCommand(ftHandle, pWriteStartCondition, bClockOutDataBitsMSBFirst, bClockOutDataBitsPosEdge, dwNumControlBitsToWrite,
                                pWriteControlBuffer, dwNumControlBytesToWrite, bWriteDataBits, dwNumDataBitsToWrite, pWriteDataBuffer,
                                dwNumDataBytesToWrite);
        }
        else
          Status = FTC_COMMAND_SEQUENCE_BUFFER_FULL;
      }
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function adds a read data command to the command sequence buffer to read data from a device connected
// to a FT2232D dual device
FTC_STATUS FT2232cMpsseSpi::SPI_AddDeviceReadCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                                     BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                     DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                                     DWORD dwNumDataBitsToRead, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  ReadDataByteBuffer ReadDataBuffer;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    Status = CheckReadDataFromExternalDeviceParameters(ftHandle, pReadStartCondition, bClockInDataBitsPosEdge, dwNumControlBitsToWrite,
                                                       pWriteControlBuffer, dwNumControlBytesToWrite, dwNumDataBitsToRead,
                                                       &ReadDataBuffer);

    if (Status == FTC_SUCCESS) {
      if (pHighPinsReadActiveStates != NULL)
      {
        if (IsGeneralPurposeHighPinsGPIOH1To4CorrectOutputStates(ftHandle, pHighPinsReadActiveStates) == FALSE)
          Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
      }
      else
        Status = FTC_NULL_OUTPUT_PINS_BUFFER_POINTER;

      if (Status == FTC_SUCCESS)
        Status = AddDeviceReadCommand(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst, bClockOutControlBitsPosEdge,
                                      dwNumControlBitsToWrite, pWriteControlBuffer, dwNumControlBytesToWrite, bClockInDataBitsMSBFirst,
                                      bClockInDataBitsPosEdge, dwNumDataBitsToRead);
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

// This function adds a read data command to the command sequence buffer to read data from a device connected
// to a FT2232H or FT4232H hi-speed device
FTC_STATUS FT2232cMpsseSpi::SPI_AddDeviceReadCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                                     BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                     DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                                     DWORD dwNumDataBitsToRead, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates)
{
  FTC_STATUS Status = FTC_SUCCESS;
  ReadDataByteBuffer ReadDataBuffer;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    Status = CheckReadDataFromExternalDeviceParameters(ftHandle, pReadStartCondition, bClockInDataBitsPosEdge, dwNumControlBitsToWrite,
                                                       pWriteControlBuffer, dwNumControlBytesToWrite, dwNumDataBitsToRead,
                                                       &ReadDataBuffer);

    if (Status == FTC_SUCCESS) {
      if (pHighPinsReadActiveStates != NULL)
      {
        if (IsGeneralPurposeHighPinsGPIOH1To8CorrectOutputStates(ftHandle, pHighPinsReadActiveStates) == FALSE)
          Status = FTC_INVALID_CONFIGURATION_HIGHER_GPIO_PIN;
      }
      else
        Status = FTC_NULL_OUTPUT_PINS_BUFFER_POINTER;

      if (Status == FTC_SUCCESS)
        Status = AddDeviceReadCommand(ftHandle, pReadStartCondition, bClockOutControlBitsMSBFirst, bClockOutControlBitsPosEdge,
                                      dwNumControlBitsToWrite, pWriteControlBuffer, dwNumControlBytesToWrite, bClockInDataBitsMSBFirst,
                                      bClockInDataBitsPosEdge, dwNumDataBitsToRead);
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_ExecuteDeviceCommandSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                                             LPDWORD lpdwNumBytesReturned)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumCmdSequenceBytes = 0;
  DWORD dwCmdSequenceByteIndex = 0;
  InputByteBuffer InputBuffer;
  DWORD dwTotalNumBytesToBeRead = 0;
  DWORD dwNumBytesRead = 0;
  DWORD dwBytesReadIndex = 0;

  EnterCriticalSection(&threadAccess);

  *lpdwNumBytesReturned = 0;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pReadCmdSequenceDataBuffer != NULL)
    {
      iCommandsSequenceDataDeviceIndex = GetCommandsSequenceDataDeviceIndex(ftHandle);

      dwNumCmdSequenceBytes = GetNumBytesInCommandsSequenceDataBuffer();

      if (dwNumCmdSequenceBytes > 0)
      {
        AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);

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
              *lpdwNumBytesReturned = dwNumBytesRead;

              // Process all bytes received and return them in the read data buffer
              for (dwBytesReadIndex = 0; dwBytesReadIndex < dwNumBytesRead; dwBytesReadIndex++)
                (*pReadCmdSequenceDataBuffer)[dwBytesReadIndex] = InputBuffer[dwBytesReadIndex];
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

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if (lpDllVersionBuffer != NULL)
  {
    if (dwBufferSize > strlen(DLL_VERSION_NUM))
      strcpy(lpDllVersionBuffer, DLL_VERSION_NUM);
    else
      Status = FTC_DLL_VERSION_BUFFER_TOO_SMALL;
  }
  else
    Status = FTC_NULL_DLL_VERSION_BUFFER_POINTER;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232cMpsseSpi::SPI_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode,
                                                   LPSTR lpErrorMessageBuffer, DWORD dwBufferSize)
{
  FTC_STATUS Status = FTC_SUCCESS;
  CHAR szErrorMsg[MAX_ERROR_MSG_SIZE];
  INT iCharCntr = 0;

  EnterCriticalSection(&threadAccess);

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
      sprintf(szErrorMsg, "%s%d", EN_New_Errors[FTC_INVALID_STATUS_CODE - FTC_FAILED_TO_COMPLETE_COMMAND], StatusCode);

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

  LeaveCriticalSection(&threadAccess);

  return Status;
}
