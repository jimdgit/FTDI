/*++

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    ft2232cmpssespi.h

Abstract:

    FT2232H and FT4232H Hi-Speed Dual Device and FT2232D Dual Device Class Declaration/Definition.

Environment:

    kernel & user mode

Revision History:

    04/07/05    kra     Created.
    29/05/08    kra     Version 1.10 - Modified FT2232.cpp to fix two bugs, if more than one device
                        is connected with the same device name, an invalid location id error is reported and changed
                        Sleep(0) to Sleep(1) to solve intermittent problem with FTC_Timeout failure
    03/06/08    kra     Version 1.20 - Added new method ie WriteGeneralPurposeHighInputOutputPins to control the 
                        output states ie high(true) or low(false) of the 4 general purpose higher input/output
                        (ACBUS 0-3/GPIOH 0-3) pins, for the SPI_Write and SPI_AddDeviceWriteCmd functions.
    04/06/08    kra     Version 1.20 - Modified AddWriteCommandDataToOutPutBuffer method, to accomadate the fact that
                        some SPI devices do not require any control bits written to them, prior to a read operation
                        and changed MIN_NUM_CONTROL_BITS constant from 2 to 0. Changed number of write control bits
                        error message from "Invalid number of write control bits. Valid range 2 to 2040." to 
                        "Invalid number of write control bits. Valid range 0 to 2040."
    04/06/08    kra     Added new function ie ReadGeneralPurposeHighInputOutputPins, to read the input states of the
                        4 general purpose higher input/output (ACBUS 0-3/GPIOH 0-3) pins that have been previously
                        configured as an input pin(s).
    21/08/08    kra     Version 2.0 - Added new functions for FT2232H and FT4232H hi-speed devices.
    04/09/08    kra     Added CRITICAL_SECTION object.

	
--*/

#ifndef FT2232cMpsseSpi_H
#define FT2232cMpsseSpi_H

#include <windows.h>

#include "ftcspi.h"

#include "FT2232h.h"

#define DEVICE_CHANNEL_A " A"
#define DEVICE_CHANNEL_B " B"

#define DLL_VERSION_NUM "2.0"

#define USB_INPUT_BUFFER_SIZE 65536  // 64K
#define USB_OUTPUT_BUFFER_SIZE 65536  // 64K

const BYTE FT_EVENT_VALUE = 0;
const BYTE FT_ERROR_VALUE = 0;

#define DEVICE_READ_TIMEOUT_INFINITE 0
#define DEVICE_WRITE_TIMEOUT 5000 // 5 seconds

#define MIN_CLOCK_DIVISOR 0     // equivalent to 6MHz
#define MAX_CLOCK_DIVISOR 65535 // equivalent to 91Hz

// kra - 04/06/08, to accomadate the fact that some SPI devices do not require any control bits written to
// them, prior to a read operation, changed MIN_NUM_CONTROL_BITS constant from 2 to 0
#define MIN_NUM_CONTROL_BITS 0 //2       // specifies the minimum number of control bits that can be written to an external device
#define MAX_NUM_CONTROL_BITS 2040        // specifies the maximum number of control bits that can be written to an external device
#define MIN_NUM_CONTROL_BYTES 1          // specifies the minimum number of control bytes that can be written to an external device
#define MAX_NUM_CONTROL_BYTES 255        // specifies the maximum number of control bytes that can be written to an external device
#define MIN_NUM_WRITE_DATA_BITS 2        // specifies the minimum number of data bits that can be written to an external device
#define MAX_NUM_WRITE_DATA_BITS 524280   // specifies the maximum number of data bits that can be written to an external device
#define MIN_NUM_WRITE_DATA_BYTES 1       // specifies the minimum number of data bytes that can be written to an external device
#define MAX_NUM_WRITE_DATA_BYTES 65535   // specifies the maximum number of data bytes that can be written to an external device
#define MIN_NUM_READ_DATA_BITS 2         // specifies the minimum number of data bits that can be read from an external device
#define MAX_NUM_READ_DATA_BITS 524280    // specifies the maximum number of data bits that can be read from an external device
#define MIN_NUM_READ_DATA_BYTES 1        // specifies the minimum number of data bytes that can be read from an external device
#define MAX_NUM_READ_DATA_BYTES 65535    // specifies the maximum number of data bytes that can be read from an external device

#define NUMBITSINBYTE 8

#define CLOCK_SIGNAL_PIN_HIGH_VALUE '\x01'
#define CLOCK_SIGNAL_PIN_LOW_VALUE '\xFE'
#define DATA_OUT_PIN_HIGH_VALUE '\x02'

#define CHIP_SELECT_PIN_HIGH_VALUE '\x08'
#define GPIOL1_PIN_HIGH_VALUE '\x10'
#define GPIOL2_PIN_HIGH_VALUE '\x20'
#define GPIOL3_PIN_HIGH_VALUE '\x40'
#define GPIOL4_PIN_HIGH_VALUE '\x80'

#define CHIP_SELECT_PIN_LOW_VALUE '\xF7'
#define GPIOL1_PIN_LOW_VALUE '\xEF'
#define GPIOL2_PIN_LOW_VALUE '\xDF'
#define GPIOL3_PIN_LOW_VALUE '\xBF'
#define GPIOL4_PIN_LOW_VALUE '\x7F'

#define SERIAL_DATA_IN_PIN_HIGH_VALUE '\x04'
#define GPIOH1_PIN_HIGH_VALUE '\x01'
#define GPIOH2_PIN_HIGH_VALUE '\x02'
#define GPIOH3_PIN_HIGH_VALUE '\x04'
#define GPIOH4_PIN_HIGH_VALUE '\x08'
#define GPIOH5_PIN_HIGH_VALUE '\x10'
#define GPIOH6_PIN_HIGH_VALUE '\x20'
#define GPIOH7_PIN_HIGH_VALUE '\x40'
#define GPIOH8_PIN_HIGH_VALUE '\x80'

#define PIN1_HIGH_VALUE  1
#define PIN2_HIGH_VALUE  2
#define PIN3_HIGH_VALUE  4
#define PIN4_HIGH_VALUE  8
#define PIN5_HIGH_VALUE  16
#define PIN6_HIGH_VALUE  32
#define PIN7_HIGH_VALUE  64
#define PIN8_HIGH_VALUE  128

#define NUM_WRITE_COMMAND_BYTES 11
#define NUM_READ_COMMAND_BYTES 5

#define NUM_GPIO_WRITE_COMMAND_BYTES 3

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

    "Pointer to channel buffer is null.",
    "Buffer to contain channel is too small.",
    "Invalid device channel. Valid values are A and B.",
    "Invalid latency timer value. Valid range is 2 - 255 milliseconds.",

    "Invalid clock divisor. Valid range is 0 - 65535.",
    "Pointer to input buffer is null.",
    "Pointer to chip select buffer is null.",
    "Pointer to input output buffer is null.",
    "Pointer to output pins buffer is null.",
    "Pointer to initial conditions buffer is null.",
    "Pointer to write control buffer is null.",
    "Pointer to write data buffer is null.",
    "Pointer to wait write data buffer is null.",
    "Pointer to read data buffer is null.",
    "Pointer to read command sequence data buffer is null.",
    "Invalid number of write control bits. Valid range 0 to 2040. 2040 bits is equivalent to 255 bytes",
    "Invalid size of write control buffer. Valid range is 1 - 255",
    "Buffer to contain number of control bits is too small.",
    "Invalid number of write data bits. Valid range 2 to 524280. 524280 bits is equivalent to 64K bytes",
    "Invalid size of write data buffer. Valid range is 1 - 65535",
    "Buffer to contain number of data bits is too small.",
    "Invalid number of read data bits. Valid range 2 to 524280. 524280 bits is equivalent to 64K bytes",
    "Invalid initial clock state ie for clocking data in/out on positive edge, clock initial state must be low.",
    "Invalid chip select pin.",
    "Invalid data write complete pin.",
    "Timeout expired waiting for data bytes to be written to external device.",
    "Invalid configuration of a general purpose higher input/output pin ie one of the pins is configured as an input instead of an output.",
    "Command sequence buffer is full. Valid range is 1 - 131070 ie 128K bytes.",
    "No command sequence found.",
    "Pointer to final state buffer is null.",
    "Pointer to dll version number buffer is null.",
    "Buffer to contain dll version number is too small.",
    "Pointer to language code buffer is null.",
    "Pointer to error message buffer is null.",
    "Buffer to contain error message is too small.",
    "Unsupported language code.",
    "Unknown status code = "};

const BYTE CLK_DATA_BYTES_OUT_ON_POS_CLK_MSB_FIRST_CMD = '\x10';
const BYTE CLK_DATA_BYTES_OUT_ON_NEG_CLK_MSB_FIRST_CMD = '\x11';
const BYTE CLK_DATA_BITS_OUT_ON_POS_CLK_MSB_FIRST_CMD = '\x12';
const BYTE CLK_DATA_BITS_OUT_ON_NEG_CLK_MSB_FIRST_CMD = '\x13';

const BYTE CLK_DATA_BYTES_IN_ON_POS_CLK_MSB_FIRST_CMD = '\x20';
const BYTE CLK_DATA_BYTES_IN_ON_NEG_CLK_MSB_FIRST_CMD = '\x25';
const BYTE CLK_DATA_BITS_IN_ON_POS_CLK_MSB_FIRST_CMD = '\x22';
const BYTE CLK_DATA_BITS_IN_ON_NEG_CLK_MSB_FIRST_CMD = '\x27';

const BYTE CLK_DATA_BYTES_OUT_ON_POS_CLK_LSB_FIRST_CMD = '\x18';
const BYTE CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD = '\x19';
const BYTE CLK_DATA_BITS_OUT_ON_POS_CLK_LSB_FIRST_CMD = '\x1A';
const BYTE CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD = '\x1B';

const BYTE CLK_DATA_BYTES_IN_ON_POS_CLK_LSB_FIRST_CMD = '\x28';
const BYTE CLK_DATA_BYTES_IN_ON_NEG_CLK_LSB_FIRST_CMD = '\x2D';
const BYTE CLK_DATA_BITS_IN_ON_POS_CLK_LSB_FIRST_CMD = '\x2A';
const BYTE CLK_DATA_BITS_IN_ON_NEG_CLK_LSB_FIRST_CMD = '\x2F';

const BYTE CLK_DATA_TMS_NO_READ_CMD = '\x4B';
const BYTE CLK_DATA_TMS_READ_CMD = '\x6B';

const BYTE SET_LOW_BYTE_DATA_BITS_CMD = '\x80';
const BYTE GET_LOW_BYTE_DATA_BITS_CMD = '\x81';
const BYTE SET_HIGH_BYTE_DATA_BITS_CMD = '\x82';
const BYTE GET_HIGH_BYTE_DATA_BITS_CMD = '\x83';
const BYTE SET_CLOCK_FREQUENCY_CMD = '\x86';
const BYTE SEND_ANSWER_BACK_IMMEDIATELY_CMD = '\x87';

#define WRITE_WAIT_COMMAND_SEQUENCE_CODE = 65535 // '\xFFFF'

#define INIT_COMMAND_SEQUENCE_WRITE_WAIT_DATA_BUFFER_SIZE 100
#define COMMAND_SEQUENCE_WRITE_WAIT_DATA_BUFFER_SIZE_INCREMENT 10

#define NUM_COMMAND_SEQUENCE_READ_DATA_BYTES 2
#define INIT_COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE 100
#define COMMAND_SEQUENCE_READ_DATA_BUFFER_SIZE_INCREMENT 10

typedef WORD OutputWordBuffer[OUTPUT_BUFFER_SIZE];
typedef OutputWordBuffer *POutputWordBuffer;

typedef DWORD ReadCommandSequenceData[NUM_COMMAND_SEQUENCE_READ_DATA_BYTES];
typedef ReadCommandSequenceData *PReadCommandSequenceData;

typedef PReadCommandSequenceData ReadCommandsSequenceData[1];
typedef ReadCommandsSequenceData *PReadCommandsSequenceData;

typedef struct Ft_Device_Low_Pins_Data{
  DWORD hDevice;                            // handle to the opened and initialized FT2232C dual type device
  DWORD dwSavedLowPinsValue;
} FTC_DEVICE_LOW_PINS_DATA, *PFTC_DEVICE_LOW_PINS_DATA;

typedef struct Ft_Device_Cmd_Sequence_Data{
  DWORD hDevice;                            // handle to the opened and initialized FT2232C dual type device
  DWORD dwSavedLowPinsValue;
  DWORD dwSavedHighPinsDirection;
  DWORD dwSavedHighPinsValue;
  DWORD dwNumBytesToSend;
  POutputByteBuffer pCommandsSequenceDataOutPutBuffer;
  DWORD dwNumWriteWaitCommandSequences;
  DWORD dwSizeReadCommandsSequenceDataBuffer;
  PReadCommandsSequenceData pReadCommandsSequenceDataBuffer;
  DWORD dwNumReadCommandSequences;
}FTC_DEVICE_CMD_SEQUENCE_DATA, *PFTC_DEVICE_CMD_SEQUENCE_DATA;


//----------------------------------------------------------------------------
class FT2232cMpsseSpi : private FT2232h
{
private:
  // This object is used to restricted access to one thread, when a process/application has multiple 
  // threads running. The critical section object will ensure that only one public method in the DLL 
  // will be executed at a time.
  CRITICAL_SECTION threadAccess;

  DWORD dwNumOpenedDevices;
  FTC_DEVICE_LOW_PINS_DATA OpenedDevicesSavedLowPinsValues[MAX_NUM_DEVICES];
  FTC_DEVICE_CMD_SEQUENCE_DATA OpenedDevicesCommandsSequenceData[MAX_NUM_DEVICES];
  INT iCommandsSequenceDataDeviceIndex;

  BOOL       IsClockCorrectInitialOutputState(PFTC_INIT_CONDITION pWriteReadStartCondition, BOOL bClockInOutDataBitsPosEdge);
  BOOL       IsGeneralPurposeHighPinsGPIOH1To4CorrectOutputStates(FTC_HANDLE ftHandle, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteReadActiveStates);
  BOOL       IsGeneralPurposeHighPinsGPIOH1To8CorrectOutputStates(FTC_HANDLE ftHandle, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteReadActiveStates);
  BOOL       IsGeneralPurposeGPIOH1To4PinsCorrectInputState(FTC_HANDLE ftHandle, DWORD dwWaitDataWritePin);
  BOOL       IsGeneralPurposeGPIOH1To8PinsCorrectInputState(FTC_HANDLE ftHandle, DWORD dwWaitDataWritePin);

  FTC_STATUS CheckWriteDataToExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsPosEdge,
                                                      DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                      DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                      PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite);
  FTC_STATUS CheckWriteDataToExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS CheckWriteDataToExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS CheckReadDataFromExternalDeviceParameters(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition,
                                                       BOOL bClockInDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                       DWORD dwNumControlBytesToWrite, DWORD dwNumDataBitsToRead,
                                                       PReadDataByteBuffer pReadDataBuffer);

  void       AddByteToOutputBuffer(DWORD dwOutputByte, BOOL bClearOutputBuffer);

  FTC_STATUS SetTCKTDITMSPinsCloseState(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData);
  FTC_STATUS InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
  FTC_STATUS SetDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
  FTC_STATUS InitDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
  FTC_STATUS EmptyDeviceInputBuffer(FTC_HANDLE ftHandle);

  void       SaveGeneralPurposeHighPinsInputOutputStates(FTC_HANDLE ftHandle, DWORD dwHighPinsDirection, DWORD dwHighPinsValue);
  FTC_STATUS SetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, DWORD dwHighPinsDirection, DWORD dwHighPinsValue);
  void       WriteGeneralPurposeHighInputOutputGPIOH1To4Pins(FTC_HANDLE ftHandle, PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  void       WriteGeneralPurposeHighInputOutputGPIOH1To8Pins(FTC_HANDLE ftHandle, PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS SetChipSelectPinsDisableStates(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates);
  FTC_STATUS InitGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle);
  void       GetGeneralPurposeHighInputOutputGPIOH1To4PinsInputStates(DWORD dwInputStatesReturnedValue, PFTC_LOW_HIGH_PINS pPinsInputData);
  FTC_STATUS GetGeneralPurposeHighInputOutputGPIOH1To4Pins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData);
  void       GetHiSpeedDeviceGeneralPurposeInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTH_LOW_HIGH_PINS pPinsInputData);
  FTC_STATUS GetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, PFTH_LOW_HIGH_PINS pHighPinsInputData);
  FTC_STATUS ReadGeneralPurposeHighInputOutputGPIOH1To4Pins(FTC_HANDLE ftHandle, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates);
  FTC_STATUS ReadGeneralPurposeHighInputOutputGPIOH1To8Pins(FTC_HANDLE ftHandle, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates);

  void       InsertDeviceSavedLowPinsValue(FTC_HANDLE ftHandle, DWORD dwLowPinsValue);
  DWORD      GetDeviceSavedLowPinsValue(FTC_HANDLE ftHandle);
  void       DeleteDeviceSavedLowPinsValue(FTC_HANDLE ftHandle);


  DWORD      SetStartCondition(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pStartCondition);
  void       GetClockDataOutCommands(BOOL bClockOutDataBitsMSBFirst, BOOL bClockOutDataBitsPosEdge,
                                     LPDWORD lpdwDataWriteBytesCommand, LPDWORD lpdwDataWriteBitsCommand);
  void       EnableSPIChip(PFTC_INIT_CONDITION pStartCondition, DWORD dwLowPinsValue);
  void       DisableSPIChip(PFTC_INIT_CONDITION pStartCondition, DWORD dwLowPinsValue);
  void       SavedAddWriteCommandDataToOutPutBuffer(PFTC_INIT_CONDITION pWriteStartCondition, DWORD dwNumControlBitsToWrite,
                                               PWriteControlByteBuffer pWriteControlBuffer, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                               PWriteDataByteBuffer pWriteDataBuffer, DWORD dwDataWriteBytesCommand, 
                                               DWORD dwDataWriteBitsCommand);
  void       AddWriteCommandDataToOutPutBuffer(PFTC_INIT_CONDITION pWriteStartCondition, DWORD dwNumControlBitsToWrite,
                                               PWriteControlByteBuffer pWriteControlBuffer, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                               PWriteDataByteBuffer pWriteDataBuffer, DWORD dwDataWriteBytesCommand, 
                                               DWORD dwDataWriteBitsCommand);
  void       GetDataWriteCompletePinState(PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete, LPDWORD lpdwDataWriteCompletePin, LPDWORD lpdwDataWriteCompleteStateValue);
  FTC_STATUS CheckDataWrittenToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete);
  FTC_STATUS WriteDataToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                       BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                       DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete);
  void       GetNumDataBytesToRead(DWORD dwNumDataBitsToRead, LPDWORD lpdwNumDataBytesToRead);
  FTC_STATUS GetDataFromExternalDevice(FTC_HANDLE ftHandle, DWORD dwNumDataBitsToRead,
                                       PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned);
  void       AddReadCommandToOutputBuffer(PFTC_INIT_CONDITION pReadStartCondition, DWORD dwNumDataBitsToRead,
                                          DWORD dwDataReadBytesCommand, DWORD dwDataReadBitsCommand);
  void       GetClockDataInCommands(BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                    LPDWORD lpdwDataReadBytesCommand, LPDWORD lpdwDataReadBitsCommand);
  FTC_STATUS ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                        BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                        DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                        DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumDataBytesReturned);

  DWORD      GetTotalNumCommandsSequenceDataBytesToRead (void);
  void       CopyReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pDestinationBuffer, PReadCommandsSequenceData pSourceBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer);
  FTC_STATUS AddReadCommandSequenceData(DWORD dwNumDataBitsToRead);
  void       CreateReadCommandsSequenceDataBuffer(void);
  PReadCommandsSequenceData CreateReadCommandsSequenceDataBuffer(DWORD dwSizeReadCmdsSequenceDataBuffer);
  void       DeleteReadCommandsSequenceDataBuffer(PReadCommandsSequenceData pReadCmdsSequenceDataBuffer, DWORD dwSizeReadCommandsSequenceDataBuffer);

  FTC_STATUS CreateDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle);
  void       ClearDeviceCommandSequenceData(FTC_HANDLE ftHandle);
  DWORD      GetNumBytesInCommandsSequenceDataBuffer(void);
  DWORD      GetCommandsSequenceDataDeviceIndex(FTC_HANDLE ftHandle);
  void       DeleteDeviceCommandsSequenceDataBuffers(FTC_HANDLE ftHandle);

  FTC_STATUS AddDeviceWriteCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                   BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                   DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                   PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite);
  FTC_STATUS AddDeviceReadCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                  BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                  DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                  DWORD dwNumDataBitsToRead);

public:
  FT2232cMpsseSpi(void);
  ~FT2232cMpsseSpi(void);

  FTC_STATUS WINAPI SPI_GetNumDevices(LPDWORD lpdwNumDevices);
  FTC_STATUS WINAPI SPI_GetNumHiSpeedDevices(LPDWORD lpdwNumHiSpeedDevices);
  FTC_STATUS WINAPI SPI_GetDeviceNameLocationID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID);
  FTC_STATUS WINAPI SPI_GetHiSpeedDeviceNameLocIDChannel(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID, LPSTR lpChannel, DWORD dwChannelBufferSize, LPDWORD lpdwHiSpeedDeviceType);
  FTC_STATUS WINAPI SPI_OpenDevice(FTC_HANDLE *pftHandle);
  FTC_STATUS WINAPI SPI_OpenSpecifiedDevice(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle);
  FTC_STATUS WINAPI SPI_OpenSpecifiedHiSpeedDevice(LPSTR lpDeviceName, DWORD dwLocationID, LPSTR lpChannel, FTC_HANDLE *pftHandle);
  FTC_STATUS WINAPI SPI_GetHiSpeedDeviceType(FTC_HANDLE ftHandle, LPDWORD lpdwHiSpeedDeviceType);
  FTC_STATUS WINAPI SPI_CloseDevice(FTC_HANDLE ftHandle);
  FTC_STATUS WINAPI SPI_CloseDevice(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData);
  FTC_STATUS WINAPI SPI_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor);
  FTC_STATUS WINAPI SPI_TurnOnDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle);
  FTC_STATUS WINAPI SPI_TurnOffDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle);
  FTC_STATUS WINAPI SPI_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE LatencyTimermSec);
  FTC_STATUS WINAPI SPI_GetDeviceLatencyTimer(FTC_HANDLE ftHandle, LPBYTE lpLatencyTimermSec);
  FTC_STATUS WINAPI SPI_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
  FTC_STATUS WINAPI SPI_GetHiSpeedDeviceClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
  FTC_STATUS WINAPI SPI_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz);
  FTC_STATUS WINAPI SPI_SetDeviceLoopbackState(FTC_HANDLE ftHandle, BOOL bLoopbackState);
  FTC_STATUS WINAPI SPI_SetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates, PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData);
  FTC_STATUS WINAPI SPI_SetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, PFTC_CHIP_SELECT_PINS pChipSelectsDisableStates,
                                                                      PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData);
  FTC_STATUS WINAPI SPI_GetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData);
  FTC_STATUS WINAPI SPI_GetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, PFTH_LOW_HIGH_PINS pHighPinsInputData);
  FTC_STATUS WINAPI SPI_WriteDataToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                  BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                  DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                  PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete,
                                                  PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS WINAPI SPI_WriteDataToExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                                  BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                  DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                                  PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite, PFTC_WAIT_DATA_WRITE pWaitDataWriteComplete,
                                                  PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS WINAPI SPI_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                                   BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                   DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                                   DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer,
                                                   LPDWORD lpdwNumDataBytesReturned, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates);
  FTC_STATUS WINAPI SPI_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                                   BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                                   DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                                   DWORD dwNumDataBitsToRead, PReadDataByteBuffer pReadDataBuffer,
                                                   LPDWORD lpdwNumDataBytesReturned, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates);
  FTC_STATUS WINAPI SPI_ClearDeviceCommandSequence(FTC_HANDLE ftHandle);
  FTC_STATUS WINAPI SPI_AddDeviceWriteCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                              BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                              DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                              PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                              PFTC_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS WINAPI SPI_AddDeviceWriteCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pWriteStartCondition, BOOL bClockOutDataBitsMSBFirst,
                                              BOOL bClockOutDataBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                              DWORD dwNumControlBytesToWrite, BOOL bWriteDataBits, DWORD dwNumDataBitsToWrite,
                                              PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
                                              PFTH_HIGHER_OUTPUT_PINS pHighPinsWriteActiveStates);
  FTC_STATUS WINAPI SPI_AddDeviceReadCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                             BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                             DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                             DWORD dwNumDataBitsToRead, PFTC_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates);
  FTC_STATUS WINAPI SPI_AddDeviceReadCommand(FTC_HANDLE ftHandle, PFTC_INIT_CONDITION pReadStartCondition, BOOL bClockOutControlBitsMSBFirst,
                                             BOOL bClockOutControlBitsPosEdge, DWORD dwNumControlBitsToWrite, PWriteControlByteBuffer pWriteControlBuffer,
                                             DWORD dwNumControlBytesToWrite, BOOL bClockInDataBitsMSBFirst, BOOL bClockInDataBitsPosEdge,
                                             DWORD dwNumDataBitsToRead, PFTH_HIGHER_OUTPUT_PINS pHighPinsReadActiveStates);
  FTC_STATUS WINAPI SPI_ExecuteDeviceCommandSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                                     LPDWORD lpdwNumBytesReturned);
  FTC_STATUS WINAPI SPI_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize);
  FTC_STATUS WINAPI SPI_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode,
                                           LPSTR lpErrorMessageBuffer, DWORD dwBufferSize);
};

#endif  /* FT2232cMpsseSpi_H */
