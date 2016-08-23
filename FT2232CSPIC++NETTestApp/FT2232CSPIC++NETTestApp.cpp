// FT2232CSPIC++NETTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

#include "FTCSPI.h"

#define MAX_SPI_93LC56B_CHIP_SIZE_IN_WORDS 128

#define MAX_FREQ_93LC56B_CLOCK_DIVISOR 3   // equivalent to 1.5MHz

const unsigned char  SPI_EWEN_CMD = '\x9F'; // set up write enable command
const unsigned char SPI_EWDS_CMD = '\x87'; // set up write disable command
const  unsigned char  SPI_ERAL_CMD = '\x97'; // set up erase all command

#define NUM_93LC56B_CMD_CONTOL_BITS 11
#define NUM_93LC56B_CMD_CONTOL_BYTES 2

#define NUM_93LC56B_CMD_DATA_BITS 16
#define NUM_93LC56B_CMD_DATA_BYTES 2

#define MAX_READ_DATA_WORDS_BUFFER_SIZE 65536    // 64k bytes

typedef WORD ReadDataWordBuffer[MAX_READ_DATA_WORDS_BUFFER_SIZE];
typedef ReadDataWordBuffer *PReadDataWordBuffer;

int _tmain(int argc, _TCHAR* argv[])
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDevices = 0;
  char szDeviceName[100];
  DWORD dwLocationID = 0;
  FTC_HANDLE ftHandle;
  BOOL bPerformCommandSequence = false;
  DWORD dwClockFrequencyHz = 0;
  FTC_CHIP_SELECT_PINS ChipSelectsDisableStates;
  FTC_INPUT_OUTPUT_PINS HighInputOutputPins;
  FTC_INIT_CONDITION WriteStartCondition;
  WriteControlByteBuffer WriteControlBuffer;
  WriteDataByteBuffer WriteDataBuffer;
  DWORD dwNumDataBytesToWrite = 0;
  FTC_WAIT_DATA_WRITE WaitDataWriteComplete;
  FTC_HIGHER_OUTPUT_PINS HighPinsWriteActiveStates;
  DWORD dwDataWordInitValue = 0;
  DWORD dwDataWordValue = 0;
  DWORD dwWriteDataWordAddress = 0;
  DWORD dwControlLocAddress1 = 0;
  DWORD dwControlLocAddress2 = 0;
  FTC_INIT_CONDITION ReadStartCondition;
  DWORD dwReadDataWordAddress = 0;
  DWORD dwReadWordValue = 0;
  ReadDataWordBuffer ReadWordData;
  DWORD dwNumDataBytesReturned = 0;
  DWORD dwDataWordWritten = 0;
  DWORD dwCharCntr = 0;
  ReadDataByteBuffer ReadDataBuffer;
  DWORD dwReadDataIndex = 0;
  ReadCmdSequenceDataByteBuffer ReadCmdSequenceDataBuffer;
  int MsgBoxKeyPressed = 0;
  DWORD dwLoopCntr = 0;
  char szDllVersion[10];
  char szTitleErrorMessage[100];
  char szErrorMessage[200];

  Status = SPI_GetNumDevices(&dwNumDevices);

  //Status = SPI_GetDllVersion(szDllVersion, 10);

  if ((Status == FTC_SUCCESS) && (dwNumDevices > 0))
  {
    if (dwNumDevices == 1)
    {
      Status = SPI_GetDeviceNameLocID(0, szDeviceName, 100, &dwLocationID);

      if (Status == FTC_SUCCESS)
      {
        Status = SPI_OpenEx(szDeviceName, dwLocationID, &ftHandle);
        //Status = SPI_Open(&ftHandle);
      }
    }
    else
    {
      if (dwNumDevices == 2)
      {
        Status = SPI_GetDeviceNameLocID(1, szDeviceName, 100, &dwLocationID);

        if (Status == FTC_SUCCESS)
        {
          Status = SPI_OpenEx(szDeviceName, dwLocationID, &ftHandle);
          //Status = SPI_Open(&ftHandle);
        }
      }
    }
  }

  if ((Status == FTC_SUCCESS) && (dwNumDevices > 0))
  {
    if (Status == FTC_SUCCESS)
      Status = SPI_GetClock(MAX_FREQ_93LC56B_CLOCK_DIVISOR, &dwClockFrequencyHz);

    //if (Status == FTC_SUCCESS)
    //  Status = SPI_SetLoopback(ftHandle, true);

    if (Status == FTC_SUCCESS)
    {
      //if (Status == FTC_SUCCESS)
        //Status = SPI_SetClock(ftHandle, 0, &dwClockFrequencyHz);

      if (Status == FTC_SUCCESS)
      {
        Status = SPI_InitDevice(ftHandle, MAX_FREQ_93LC56B_CLOCK_DIVISOR); //65535

        bPerformCommandSequence = true;

        if (bPerformCommandSequence == true)
        {
            if (Status == FTC_SUCCESS)
              Status = SPI_ClearDeviceCmdSequence(ftHandle);
        }

        if (Status == FTC_SUCCESS)
        {
          // Must set the chip select disable states for all the SPI devices connected to the FT2232C dual device
          ChipSelectsDisableStates.bADBUS3ChipSelectPinState = false;
          ChipSelectsDisableStates.bADBUS4GPIOL1PinState = false;
          ChipSelectsDisableStates.bADBUS5GPIOL2PinState = false;
          ChipSelectsDisableStates.bADBUS6GPIOL3PinState = false;
          ChipSelectsDisableStates.bADBUS7GPIOL4PinState = false;

          HighInputOutputPins.bPin1InputOutputState = true;
          HighInputOutputPins.bPin1LowHighState = false;
          HighInputOutputPins.bPin2InputOutputState = true;
          HighInputOutputPins.bPin2LowHighState = false;
          HighInputOutputPins.bPin3InputOutputState = true;
          HighInputOutputPins.bPin3LowHighState = false;
          HighInputOutputPins.bPin4InputOutputState = true;
          HighInputOutputPins.bPin4LowHighState = false;

          Status = SPI_SetGPIOs(ftHandle, &ChipSelectsDisableStates, &HighInputOutputPins);
        }

        do
        {
          if (Status == FTC_SUCCESS)
          {
            WriteStartCondition.bClockPinState = false;
            WriteStartCondition.bDataOutPinState = false;
            WriteStartCondition.bChipSelectPinState = false;
            WriteStartCondition.dwChipSelectPin = ADBUS3ChipSelect;

            WaitDataWriteComplete.bWaitDataWriteComplete = false;

            HighPinsWriteActiveStates.bPin1ActiveState = false;
            HighPinsWriteActiveStates.bPin1State = false;
            HighPinsWriteActiveStates.bPin2ActiveState = false;
            HighPinsWriteActiveStates.bPin2State = false;
            HighPinsWriteActiveStates.bPin3ActiveState = false;
            HighPinsWriteActiveStates.bPin3State = false;
            HighPinsWriteActiveStates.bPin4ActiveState = false;
            HighPinsWriteActiveStates.bPin4State = false;

            WriteControlBuffer[0] = SPI_EWEN_CMD;
            WriteControlBuffer[1] = '\xFF';

            // enable writing
            Status = SPI_Write(ftHandle, &WriteStartCondition, true, false,
                               NUM_93LC56B_CMD_CONTOL_BITS, &WriteControlBuffer, NUM_93LC56B_CMD_CONTOL_BYTES,
                               false, 0, &WriteDataBuffer, 0, &WaitDataWriteComplete, &HighPinsWriteActiveStates);

            if (Status == FTC_SUCCESS)
            {
              WriteControlBuffer[0] = SPI_ERAL_CMD;

              // erase all
              Status = SPI_Write(ftHandle, &WriteStartCondition, true, false,
                                 NUM_93LC56B_CMD_CONTOL_BITS, &WriteControlBuffer, NUM_93LC56B_CMD_CONTOL_BYTES,
                                 false, 0, &WriteDataBuffer, 0, &WaitDataWriteComplete, &HighPinsWriteActiveStates);

              if (Status == FTC_SUCCESS)
              {
                WriteControlBuffer[0] = SPI_EWDS_CMD;

                // disable writing
                Status = SPI_Write(ftHandle, &WriteStartCondition, true, false,
                                   NUM_93LC56B_CMD_CONTOL_BITS, &WriteControlBuffer, NUM_93LC56B_CMD_CONTOL_BYTES,
                                  false, 0, &WriteDataBuffer, 0, &WaitDataWriteComplete, &HighPinsWriteActiveStates);
              }
            }
          }

          Sleep(200);

          if ((dwLoopCntr % 2) == 0)
            dwDataWordInitValue = 0;
          else
            dwDataWordInitValue = 30583; //'\x7777';

          if (Status == FTC_SUCCESS)
          {
            WaitDataWriteComplete.bWaitDataWriteComplete = true;
            WaitDataWriteComplete.dwWaitDataWritePin = ADBUS2DataIn;
            WaitDataWriteComplete.bDataWriteCompleteState = true;
            WaitDataWriteComplete.dwDataWriteTimeoutmSecs = 5000;

            dwWriteDataWordAddress = 0;

            do
            {
              // set up write command and address
              dwControlLocAddress1 = 160; //'\xA0';
              dwControlLocAddress1 = (dwControlLocAddress1 | ((dwWriteDataWordAddress / 8) & '\x0F'));

              // shift left 5 bits ie make bottom 3 bits the 3 MSB's
              dwControlLocAddress2 = ((dwWriteDataWordAddress & '\x07') * 32);

              WriteControlBuffer[0] = (dwControlLocAddress1 & '\xFF');
              WriteControlBuffer[1] = (dwControlLocAddress2 & '\xFF');

              if (dwDataWordInitValue == 0)
                dwDataWordValue = dwWriteDataWordAddress;
              else
                dwDataWordValue = dwDataWordInitValue;

              // write data
              WriteDataBuffer[0] = (dwDataWordValue & '\xFF');
              WriteDataBuffer[1] = ((dwDataWordValue / 256) & '\xFF');

              Status = SPI_Write(ftHandle, &WriteStartCondition, true, false,
                                 NUM_93LC56B_CMD_CONTOL_BITS, &WriteControlBuffer, NUM_93LC56B_CMD_CONTOL_BYTES,
                                 true, NUM_93LC56B_CMD_DATA_BITS, &WriteDataBuffer, NUM_93LC56B_CMD_DATA_BYTES,
                                 &WaitDataWriteComplete, &HighPinsWriteActiveStates);

              dwWriteDataWordAddress++;
            }
            while ((dwWriteDataWordAddress < MAX_SPI_93LC56B_CHIP_SIZE_IN_WORDS) && (Status == FTC_SUCCESS));

            Sleep(200);

            if (Status == FTC_SUCCESS)
              Status = SPI_InitDevice(ftHandle, MAX_FREQ_93LC56B_CLOCK_DIVISOR); //65535

            if (Status == FTC_SUCCESS)
            {
              ReadStartCondition.bClockPinState = false;
              ReadStartCondition.bDataOutPinState = false;
              ReadStartCondition.bChipSelectPinState = false;
              ReadStartCondition.dwChipSelectPin = ADBUS3ChipSelect;

              dwReadDataWordAddress = 0;

              do
              {
                // set up read command and address
                dwControlLocAddress1 = 192; //'\xC0';
                dwControlLocAddress1 = (dwControlLocAddress1 | ((dwReadDataWordAddress / 8) & '\x0F'));

                // shift left 5 bits ie make bottom 3 bits the 3 MSB's
                dwControlLocAddress2 = ((dwReadDataWordAddress & '\x07') * 32);

                WriteControlBuffer[0] = (dwControlLocAddress1 & '\xFF');
                WriteControlBuffer[1] = (dwControlLocAddress2 & '\xFF');

                if (bPerformCommandSequence == false)
                {
                  Status = SPI_Read(ftHandle, &ReadStartCondition, true, false, NUM_93LC56B_CMD_CONTOL_BITS,
                                    &WriteControlBuffer, NUM_93LC56B_CMD_CONTOL_BYTES, true, false, NUM_93LC56B_CMD_DATA_BITS,
                                    &ReadDataBuffer, &dwNumDataBytesReturned, &HighPinsWriteActiveStates);

                  if (Status == FTC_SUCCESS)
                  {
                    dwReadWordValue = ((ReadDataBuffer[1] * 256) | ReadDataBuffer[0]);

                    ReadWordData[dwReadDataWordAddress] = dwReadWordValue;
                  }
                }
                else
                  Status = SPI_AddDeviceReadCmd(ftHandle, &ReadStartCondition, true, false, NUM_93LC56B_CMD_CONTOL_BITS,
                                                &WriteControlBuffer, NUM_93LC56B_CMD_CONTOL_BYTES, true, false, NUM_93LC56B_CMD_DATA_BITS,
                                                &HighPinsWriteActiveStates);

                dwReadDataWordAddress++;
              }
              while ((dwReadDataWordAddress < MAX_SPI_93LC56B_CHIP_SIZE_IN_WORDS) && (Status == FTC_SUCCESS));

              MsgBoxKeyPressed = IDOK;

              if (Status == FTC_SUCCESS)
              {
                if (bPerformCommandSequence == true)
                {
                  Status = SPI_ExecuteDeviceCmdSequence(ftHandle, &ReadCmdSequenceDataBuffer, &dwNumDataBytesReturned);

                  if (Status == FTC_SUCCESS)
                  {
                    if (dwNumDataBytesReturned > 0)
                    {
                      dwReadDataWordAddress = 0;

                      dwReadDataIndex = 0;

                      do
                      {
                        dwReadWordValue = ((ReadCmdSequenceDataBuffer[(dwReadDataIndex + 1)] * 256) | ReadCmdSequenceDataBuffer[dwReadDataIndex]);

                        ReadWordData[dwReadDataWordAddress] = dwReadWordValue;

                        dwReadDataIndex = (dwReadDataIndex + 2);

                        dwReadDataWordAddress++;
                      }
                      while (dwReadDataIndex < dwNumDataBytesReturned);
                    }
                    else
                      MsgBoxKeyPressed = MessageBox(NULL, "No data returned.", "SPI Execute Command Sequence", MB_OK | MB_OKCANCEL);
                  }
                }

                if ((Status == FTC_SUCCESS) && (MsgBoxKeyPressed == IDOK))
                {
                  for (dwReadDataIndex = 0; ((dwReadDataIndex < MAX_SPI_93LC56B_CHIP_SIZE_IN_WORDS) && (MsgBoxKeyPressed == IDOK)); dwReadDataIndex++)
                  {
                    if (dwDataWordInitValue != 0)
                      dwDataWordWritten = dwDataWordInitValue;
                    else
                      dwDataWordWritten = (dwReadDataIndex & '\xFF');

                    if (((ReadWordData[dwReadDataIndex] != dwDataWordWritten) && (dwDataWordValue != 0)) ||
                        ((ReadWordData[dwReadDataIndex] != dwDataWordWritten) && (dwDataWordValue == 0)))
                    {
                      for (dwCharCntr = 0; (dwCharCntr < 100); dwCharCntr++)
                        szErrorMessage[dwCharCntr] = '\0';

                      sprintf(szErrorMessage, "SPI 93LC56B Read/Write mismatch, Address = %i, Write Value = %X, Read Value = %X", dwReadDataIndex, dwDataWordWritten, ReadWordData[dwReadDataIndex]);
                      MsgBoxKeyPressed = MessageBox(NULL, szErrorMessage, "SPI Read Error Message", MB_OK | MB_OKCANCEL);
                    }
                  }
                }
              }
            }
          }

          dwLoopCntr++;
        }
        while ((dwLoopCntr < 10) && (Status == FTC_SUCCESS));
      }
    }

    SPI_Close(ftHandle);
  }

  if (Status != FTC_SUCCESS)
  {
    Status = SPI_GetErrorCodeString("EN", Status, szErrorMessage, 200);

    sprintf(szTitleErrorMessage, "%s%d", "SPI Error Status Message, Loop No = ", dwLoopCntr);

    MessageBox(NULL, szErrorMessage, "SPI Error Status Message", MB_OK);
  }
  else
  {
    Status = SPI_GetDllVersion(szDllVersion, 10);

    //if (Status == FTC_SUCCESS)
    //  MessageBox(NULL, szDllVersion, "SPI DLL Version", MB_OK);
    //else
    //{
      //Status = SPI_GetErrorCodeString("EN", Status, szErrorMessage, 100);

      //MessageBox(NULL, szErrorMessage, "SPI Error Status Message", MB_OK);
    //}
  }

	return 0;
}

