// I2CTEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>


//============================================================================
//  Use of FTDI D2XX library:
//----------------------------------------------------------------------------
//  Include the following 2 lines in your header-file
#pragma comment(lib, "FTD2XX.lib")
#include "FTD2XX.h"
//============================================================================


#include <stdlib.h>
	const BYTE AA_ECHO_CMD_1 = '\xAA';
		const BYTE AB_ECHO_CMD_2 = '\xAB';
		const BYTE BAD_COMMAND_RESPONSE = '\xFA';

		const BYTE MSB_VEDGE_CLOCK_IN_BIT = '\x22';
		const BYTE MSB_EDGE_CLOCK_OUT_BYTE = '\x11';
		const BYTE MSB_EDGE_CLOCK_IN_BYTE = '\x24';



		const BYTE MSB_FALLING_EDGE_CLOCK_BYTE_IN = '\x24';
		const BYTE MSB_FALLING_EDGE_CLOCK_BYTE_OUT = '\x11';
		const BYTE MSB_DOWN_EDGE_CLOCK_BIT_IN = '\x26';
		const BYTE MSB_UP_EDGE_CLOCK_BYTE_IN = '\x20';
		const BYTE MSB_UP_EDGE_CLOCK_BYTE_OUT = '\x10';
		const BYTE MSB_RISING_EDGE_CLOCK_BIT_IN = '\x22';

	FT_STATUS ftStatus;			//Status defined in D2XX to indicate operation result
	FT_HANDLE ftHandle;			//Handle of FT2232H device port 
	BYTE OutputBuffer[1024];		//Buffer to hold MPSSE commands and data to be sent to FT2232H
	BYTE InputBuffer[1024];		//Buffer to hold Data bytes to be read from FT2232H
	//DWORD dwClockDivisor = 0x004A;  	//Value of clock divisor, SCL Frequency = 60/((1+0x004A)*2) (MHz) = 400khz
	DWORD dwClockDivisor = 0x0095;   //200khz
	DWORD dwNumBytesToSend = 0; 	//Index of output buffer
	DWORD dwNumBytesSent = 0, 	dwNumBytesRead = 0, dwNumInputBuffer = 0;

	BYTE ByteDataRead;//ByteAddress
	BYTE ByteAddresshigh = 0x00, ByteAddresslow = 0x80;		//EEPROM address is '0x0080'
	BYTE ByteDataToBeSend = 0x5A;							//data programmed and read


void HighSpeedSetI2CStart(void)
{
	DWORD dwCount;
	for(dwCount=0; dwCount < 4; dwCount++)	// Repeat commands to ensure the minimum period of the start hold time ie 600ns is achieved
	{
		OutputBuffer[dwNumBytesToSend++] = '\x80';	//Command to set directions of lower 8 pins and force value on bits set as output
		OutputBuffer[dwNumBytesToSend++] = '\x03';  //Set SDA, SCL high, WP disabled by SK, DO at bit ¡®1¡¯, GPIOL0 at bit ¡®0¡¯
		OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
	}
	for(dwCount=0; dwCount < 4; dwCount++)	// Repeat commands to ensure the minimum period of the start setup time ie 600ns is achieved
	{
		OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
		OutputBuffer[dwNumBytesToSend++] = '\x01'; 	//Set SDA low, SCL high, WP disabled by SK at bit ¡®1¡¯, DO, GPIOL0 at bit ¡®0¡¯
		OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
	}
	OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
	OutputBuffer[dwNumBytesToSend++] = '\x00'; 	//Set SDA, SCL low high, WP disabled by SK, DO, GPIOL0 at bit ¡®0¡¯
	OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
}

void HighSpeedSetI2CStop(void)
{
	DWORD dwCount;
	for(dwCount=0; dwCount<4; dwCount++)	// Repeat commands to ensure the minimum period of the stop setup time ie 600ns is achieved
	{
		OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
		OutputBuffer[dwNumBytesToSend++] = '\x01'; 	//Set SDA low, SCL high, WP disabled by SK at bit ¡®1¡¯, DO, GPIOL0 at bit ¡®0¡¯
		OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
	}
	for(dwCount=0; dwCount<4; dwCount++)	// Repeat commands to ensure the minimum period of the stop hold time ie 600ns is achieved
	{
		OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
		OutputBuffer[dwNumBytesToSend++] = '\x03'; 	//Set SDA, SCL high, WP disabled by SK, DO at bit ¡®1¡¯, GPIOL0 at bit ¡®0¡¯
		OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
	}
	//Tristate the SCL, SDA pins
	OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
	OutputBuffer[dwNumBytesToSend++] = '\x00'; 	//Set WP disabled by GPIOL0 at bit 0¡¯
	OutputBuffer[dwNumBytesToSend++] = '\x10';	//Set GPIOL0 pins as output with bit ¡¯1¡¯, SK, DO and other pins as input with bit ¡®0¡¯
}

BOOL SendByteAndCheckACK(BYTE dwDataSend)
{
	FT_STATUS ftStatus = FT_OK;
	OutputBuffer[dwNumBytesToSend++] = MSB_FALLING_EDGE_CLOCK_BYTE_OUT; 	//clock data byte out on ¨Cve Clock Edge MSB first
	OutputBuffer[dwNumBytesToSend++] = '\x00';
	OutputBuffer[dwNumBytesToSend++] = '\x00';	//Data length of 0x0000 means 1 byte data to clock out
	OutputBuffer[dwNumBytesToSend++] = dwDataSend;	//Set control byte, bit 4-7 of ¡®1010¡¯ as 24LC02 control code, bit 1-3 as block select bits  //which is don¡¯t care here, bit 0 of ¡®0¡¯ represent Write operation
	//Get Acknowledge bit from EEPROM
	OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
	OutputBuffer[dwNumBytesToSend++] = '\x00'; 	//Set SCL low, WP disabled by SK, GPIOL0 at bit ¡®0¡¯
	OutputBuffer[dwNumBytesToSend++] = '\x11';	//Set SK, GPIOL0 pins as output with bit ¡¯1¡¯, DO and other pins as input with bit ¡®0¡¯
	OutputBuffer[dwNumBytesToSend++] = MSB_RISING_EDGE_CLOCK_BIT_IN; 	//Command to scan in acknowledge bit , -ve clock Edge MSB first
	OutputBuffer[dwNumBytesToSend++] = '\x0';	//Length of 0x0 means to scan in 1 bit

	OutputBuffer[dwNumBytesToSend++] = '\x87';	//Send answer back immediate command
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);		//Send off the commands
	dwNumBytesToSend = 0;			//Clear output buffer
	//Check if ACK bit received, may need to read more times to get ACK bit or fail if timeout
	ftStatus = FT_Read(ftHandle, InputBuffer, 1, &dwNumBytesRead);  	//Read one byte from device receive buffer
	if ((ftStatus != FT_OK) || (dwNumBytesRead == 0))
	{
		printf("fail to get ACK when send control byte 1 [Program Section] \n");
			return FALSE; //Error, can't get the ACK bit from EEPROM
	}
	else 
	{
		if (((InputBuffer[0] & BYTE('\x1'))  != BYTE('\x0'))	)	//Check ACK bit 0 on data byte read out
		{	
			printf("fail to get ACK when send control byte 2 [Program Section] \n");
			return FALSE; //Error, can't get the ACK bit from EEPROM
		}
	}
	
	OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
	OutputBuffer[dwNumBytesToSend++] = '\x02'; 	//Set SDA high, SCL low, WP disabled by SK at bit '0', DO, GPIOL0 at bit '1'
	OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//////////////////////////////////////////////////////////
	/// INITIALIZE SECTION
	///////////////////////////////

	//////////////////////////////////////////////////////////////////
	// Define the variables used 
	//////////////////////////////////////////////////////////////////
	

	DWORD dwCount;
	//BYTE I2CConvertDes[] = "USB Serial Converter A";     //Device port description

	DWORD devIndex = 0;
	char Buf[64];
	DWORD numDevs;


	
	/////////////////////////////////////////////////////
	long locIdBuf[16];
	FT_DEVICE_LIST_INFO_NODE *devInfo;
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (numDevs > 0) {
		// allocate storage for list based on numDevs
		devInfo =
			(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
		if (ftStatus == FT_OK) {
			for (int i = 0; i < numDevs; i++) {
				printf("Dev %d:\n", i);
				printf(" Flags=0x%x\n", devInfo[i].Flags);
				printf(" Type=0x%x\n", devInfo[i].Type);
				printf(" ID=0x%x\n", devInfo[i].ID);
				printf(" LocId=0x%x\n", devInfo[i].LocId);
				printf(" SerialNumber=%s\n", devInfo[i].SerialNumber);
				printf(" Description=%s\n", devInfo[i].Description);
				printf(" ftHandle=0x%x\n", devInfo[i].ftHandle);
			}
		}
	}
		ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
ftStatus = FT_ListDevices(locIdBuf,&numDevs,FT_LIST_ALL|FT_OPEN_BY_LOCATION);
if (ftStatus == FT_OK) {
// FT_ListDevices OK, location IDs are in locIdBuf, and
// numDevs contains the number of devices connected
	printf("FT List devices OK!\r\n");
	for (int i =0; i < numDevs; i++)
	{
		printf("Device %d LocID: %x \r\n", i,locIdBuf[i]);
	}
}
else {
// FT_ListDevices failed
}
ftStatus = FT_OpenEx((PVOID)locIdBuf[2], FT_OPEN_BY_LOCATION, &ftHandle);
//ftStatus = FT_Open(2,&ftHandle);
if (ftStatus == FT_OK) {

	printf("FT Open devices OK!\r\n");
}
else {
// FT_ListDevices failed
}
ftStatus = FT_ListDevices(locIdBuf,&numDevs,FT_LIST_ALL|FT_OPEN_BY_LOCATION);
if (ftStatus == FT_OK) {
// FT_ListDevices OK, location IDs are in locIdBuf, and
// numDevs contains the number of devices connected
	printf("FT List devices OK!\r\n");
	for (int i =0; i < numDevs; i++)
	{
		printf("Device %d LocID: %x \r\n", i,locIdBuf[i]);
	}
}
else {
// FT_ListDevices failed
	printf("FT List device failed!\r\n");
}
///////////////////////////////////////////////////

	ftStatus = FT_ListDevices(&numDevs,NULL, FT_LIST_NUMBER_ONLY);
	ftStatus = FT_ListDevices((PVOID)devIndex,&Buf, FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER);
	//ftStatus = FT_OpenEx((PVOID)Buf, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
	ftStatus = FT_Open(0,&ftHandle);


	if (ftStatus != FT_OK)
	{
		printf("Can't open FT2232H device! \n");
		return 1;
	}
	else
    {      // Port opened successfully
		printf("Successfully open FT2232H device! \n");
		///////////////////////////////////////////////////////
			FT_HANDLE ftHandle2;
	FT_STATUS ftStatus2;
	ftStatus2 = FT_Open(1,&ftHandle2);
	if (ftStatus2 != FT_OK)
	{
		printf("Can't open FT2232H device %d! \n", 1);
		return 1;
	}

		/*
FT_PROGRAM_DATA ftData;

		char ManufacturerBuf[32];
	char ManufacturerIdBuf[16];
	char DescriptionBuf[64];
	char SerialNumberBuf[16]; 
	char Desstr[16] = "USB test2";

	ftData.Signature1 = 0x00000000; 
	ftData.Signature2 = 0xffffffff; 
	ftData.Version = 0x00000001; // EEPROM structure with FT4232H extensions 
	ftData.Manufacturer = ManufacturerBuf; 
	ftData.ManufacturerId = ManufacturerIdBuf; 
	ftData.Description = DescriptionBuf; 
	ftData.SerialNumber = SerialNumberBuf;
	ftStatus = FT_EE_Read(ftHandle,&ftData); 

	//strcpy(ftData.Description,Desstr); 

	//ftData.Description = "USB test";
	//ftData.IFAIsFifo = 0x1;

	ftStatus = FT_EE_Program(ftHandle, &ftData);
	Sleep(100);
	ftStatus = FT_EE_Read(ftHandle,&ftData); 

	ftStatus |= FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);	 // Get the number of bytes in the FT2232H receive buffer
	while(1);
	*/
FT_PROGRAM_DATA ftData1, ftData2;

	char ManufacturerBuf[32];
	char ManufacturerIdBuf[16];
	char DescriptionBuf[64];
	char SerialNumberBuf[16]; 

	char ManufacturerBuf2[32];
	char ManufacturerIdBuf2[16];
	char DescriptionBuf2[64];
	char SerialNumberBuf2[16]; 
	char Desstr1[16] = "USB test1";
	char Desstr2[16] = "USB test2";

	ftData1.Signature1 = 0x00000000; 
	ftData1.Signature2 = 0xffffffff; 
	ftData1.Version = 0x00000001; // EEPROM structure with FT4232H extensions 
	ftData1.Manufacturer = ManufacturerBuf; 
	ftData1.ManufacturerId = ManufacturerIdBuf; 
	ftData1.Description = DescriptionBuf; 
	ftData1.SerialNumber = SerialNumberBuf;
	

	ftData2.Signature1 = 0x00000000; 
	ftData2.Signature2 = 0xffffffff; 
	ftData2.Version = 0x00000001; // EEPROM structure with FT4232H extensions 
	ftData2.Manufacturer = ManufacturerBuf2; 
	ftData2.ManufacturerId = ManufacturerIdBuf2; 
	ftData2.Description = DescriptionBuf2; 
	ftData2.SerialNumber = SerialNumberBuf2;
	//ftStatus2 = FT_EE_Read(ftHandle,&ftData2); 

	ftStatus = FT_EraseEE(ftHandle);
	Sleep(100);
	ftStatus = FT_EE_Read(ftHandle,&ftData1); 

	ftStatus = FT_EraseEE(ftHandle2);
	Sleep(100);
	ftStatus2 = FT_EE_Read(ftHandle2,&ftData2); 

	strcpy(ftData1.Description,Desstr1); 

	//ftData.Description = "USB test";
	//ftData.IFAIsFifo = 0x1;

	ftStatus = FT_EE_Program(ftHandle, &ftData1);
	Sleep(100);
	ftStatus = FT_EE_Read(ftHandle,&ftData1); 

	ftStatus |= FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);	 // Get the number of bytes in the FT2232H receive buffer
	while(1);
/////////////////////////////////////////////////////////////


		ftStatus |= FT_ResetDevice(ftHandle); 	//Reset USB device
		//Purge USB receive buffer first by reading out all old data from FT2232H receive buffer
		ftStatus |= FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);	 // Get the number of bytes in the FT2232H receive buffer
		if ((ftStatus == FT_OK) && (dwNumInputBuffer > 0))
				FT_Read(ftHandle, &InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  	//Read out the data from FT2232H receive buffer
		ftStatus |= FT_SetUSBParameters(ftHandle, 65536, 65535);	//Set USB request transfer size
		ftStatus |= FT_SetChars(ftHandle, false, 0, false, 0);	 //Disable event and error characters
		ftStatus |= FT_SetTimeouts(ftHandle, 0, 5000);		//Sets the read and write timeouts in milliseconds for the FT2232H
		ftStatus |= FT_SetLatencyTimer(ftHandle, 16);		//Set the latency timer
		ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x00); 		//Reset controller
		ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x02);	 	//Enable MPSSE mode

		if (ftStatus != FT_OK)
		{
			printf("fail on initialize FT2232H device 1 ! \n");
			return 1;
		}
		Sleep(50);	// Wait for all the USB stuff to complete and work

	
		
		//////////////////////////////////////////////////////////////////
		// Synchronize the MPSSE interface by sending bad command ¡®0xAA¡¯
		//////////////////////////////////////////////////////////////////
		OutputBuffer[dwNumBytesToSend++] = '\xAA';		//Add BAD command ¡®0xAA¡¯
		ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	// Send off the BAD commands
		dwNumBytesToSend = 0;			//Clear output buffer
		do{
			ftStatus = FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);	 // Get the number of bytes in the device input buffer
		}while ((dwNumInputBuffer == 0) && (ftStatus == FT_OK));   	//or Timeout
		
		bool bCommandEchod = false;
		ftStatus = FT_Read(ftHandle, &InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  //Read out the data from input buffer
		for (dwCount = 0; dwCount < dwNumBytesRead - 1; dwCount++)	//Check if Bad command and echo command received
		{
			if ((InputBuffer[dwCount] == BYTE('\xFA')) && (InputBuffer[dwCount+1] == BYTE('\xAA')))
			{
				bCommandEchod = true;
				break;
			}
		}
		if (bCommandEchod == false) 
		{	
			printf("fail to synchronize MPSSE with command '0xAA' \n");
			return 1; /*Error, can¡¯t receive echo command , fail to synchronize MPSSE interface;*/ 
		}
		
		//////////////////////////////////////////////////////////////////
		// Synchronize the MPSSE interface by sending bad command ¡®0xAB¡¯
		//////////////////////////////////////////////////////////////////
		//dwNumBytesToSend = 0;			//Clear output buffer
		OutputBuffer[dwNumBytesToSend++] = '\xAB';	//Send BAD command ¡®0xAB¡¯
		ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	// Send off the BAD commands
		dwNumBytesToSend = 0;			//Clear output buffer
		do{
			ftStatus = FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);	//Get the number of bytes in the device input buffer
		}while ((dwNumInputBuffer == 0) && (ftStatus == FT_OK));   //or Timeout
		bCommandEchod = false;
		ftStatus = FT_Read(ftHandle, &InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  //Read out the data from input buffer
		for (dwCount = 0;dwCount < dwNumBytesRead - 1; dwCount++)	//Check if Bad command and echo command received
		{
			if ((InputBuffer[dwCount] == BYTE('\xFA')) && (InputBuffer[dwCount+1] == BYTE( '\xAB')))
			{
				bCommandEchod = true;
				break;
			}
		}
		if (bCommandEchod == false)  
		{	
			printf("fail to synchronize MPSSE with command '0xAB' \n");
			return 1; 
			/*Error, can¡¯t receive echo command , fail to synchronize MPSSE interface;*/ 
		}

		printf("MPSSE synchronized with BAD command \n");

		////////////////////////////////////////////////////////////////////
		//Configure the MPSSE for I2C communication with 24LC02B
		//////////////////////////////////////////////////////////////////
		OutputBuffer[dwNumBytesToSend++] = '\x8A'; 	//Ensure disable clock divide by 5 for 60Mhz master clock
		OutputBuffer[dwNumBytesToSend++] = '\x97';	 //Ensure turn off adaptive clocking
		OutputBuffer[dwNumBytesToSend++] = '\x8D'; 	//Enable 3 phase data clock, used by I2C to allow data on both clock edges
		ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	// Send off the commands
		dwNumBytesToSend = 0;			//Clear output buffer
		OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output 
		OutputBuffer[dwNumBytesToSend++] = '\x03'; 	//Set SDA, SCL high, WP disabled by SK, DO at bit ¡®1¡¯, GPIOL0 at bit ¡®0¡¯
		OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
		// The SK clock frequency can be worked out by below algorithm with divide by 5 set as off
		// SK frequency  = 60MHz /((1 +  [(1 +0xValueH*256) OR 0xValueL])*2)
		OutputBuffer[dwNumBytesToSend++] = '\x86'; 			//Command to set clock divisor
		OutputBuffer[dwNumBytesToSend++] = dwClockDivisor & '\xFF';	//Set 0xValueL of clock divisor
		OutputBuffer[dwNumBytesToSend++] = (dwClockDivisor >> 8) & '\xFF';	//Set 0xValueH of clock divisor
		ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	// Send off the commands
		dwNumBytesToSend = 0;			//Clear output buffer
		Sleep(20);		//Delay for a while

		//Turn off loop back in case
		OutputBuffer[dwNumBytesToSend++] = '\x85';		//Command to turn off loop back of TDI/TDO connection
		ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	// Send off the commands
		dwNumBytesToSend = 0;			//Clear output buffer
		Sleep(30);		//Delay for a while
	}

BOOL bSucceed = TRUE;

	//////////////////////////////////////////////////////////

	// PROGRAM SECTION
	///////////////////////////////
	

	////////////////////////////////////////////////////////
	//Set I2C Start Condition
	////////////////////////////////////////////////////////

	HighSpeedSetI2CStart();
	
 
	////////////////////////////////////////////////////////////////////////
	//Send control byte and check the ACK bit
	////////////////////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(0xAE);

	
	////////////////////////////////////////////////////////////////////////
	// Send word high address byte and check the ACK bit
	////////////////////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(ByteAddresshigh);

	
	////////////////////////////////////////////////////////////////////////
	// Send word low address byte and check the ACK bit
	////////////////////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(ByteAddresslow);
		
	////////////////////////////////////////////////////////////////////////
	// Send data byte and check the ACK bit
	////////////////////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(ByteDataToBeSend);
		

	////////////////////////////////////////////////////////////////////////
	// Set I2C Stop Condition
	////////////////////////////////////////////////////////////////////////
	HighSpeedSetI2CStop();

	//Send off the commands
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	dwNumBytesToSend = 0;			//Clear output buffer

	Sleep(50);

	//////////////////
	///READ SECTION
	///////////////////

	//Purge USB receive buffer first before read operation
	ftStatus = FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);	 // Get the number of bytes in the device receive buffer
	if ((ftStatus == FT_OK) && (dwNumInputBuffer > 0))
			FT_Read(ftHandle, &InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  //Read out all the data from receive buffer

	//////////////////////////////////////////////////////////
	// Set I2C start condition
	//////////////////////////////////////////////////////////
	HighSpeedSetI2CStart();


	//////////////////////////////////////////////////////////
	// Send control byte and check ACK bit
	//////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(0xAE);

	//////////////////////////////////////////////////////////
	// Send high address byte and check ACK bit
	//////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(ByteAddresshigh);

	//////////////////////////////////////////////////////////
	// Send low address byte and check ACK bit
	//////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(ByteAddresslow);

	//////////////////////////////////////////////////////////
	// Set I2C start condition
	//////////////////////////////////////////////////////////
	HighSpeedSetI2CStart();
	 

	//////////////////////////////////////////////////////////
	// Send control byte and check ACK bit again
	//////////////////////////////////////////////////////////
	bSucceed = SendByteAndCheckACK(0xAF);

	//////////////////////////////////////////////////////////
	// Read the data from 24LC02B with no ACK bit check
	//////////////////////////////////////////////////////////
	//dwNumBytesToSend = 0;			//Clear output buffer
	OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
	OutputBuffer[dwNumBytesToSend++] = '\x00'; 	//Set SCL low, WP disabled by SK, GPIOL0 at bit ¡®0¡¯
	OutputBuffer[dwNumBytesToSend++] = '\x11';	//Set SK, GPIOL0 pins as output with bit ¡¯1¡¯, DO and other pins as input with bit ¡®0¡¯
	OutputBuffer[dwNumBytesToSend++] = MSB_FALLING_EDGE_CLOCK_BYTE_IN; 	//Command to clock data byte in on ¨Cve Clock Edge MSB first
	OutputBuffer[dwNumBytesToSend++] = '\x00';
	OutputBuffer[dwNumBytesToSend++] = '\x00';	//Data length of 0x0000 means 1 byte data to clock in
	//OutputBuffer[dwNumBytesToSend++] = '\x87';	//Send answer back immediate command

	OutputBuffer[dwNumBytesToSend++] = MSB_RISING_EDGE_CLOCK_BIT_IN; 	//Command to scan in acknowledge bit , -ve clock Edge MSB first
	OutputBuffer[dwNumBytesToSend++] = '\x0';	//Length of 0 means to scan in 1 bit
	OutputBuffer[dwNumBytesToSend++] = '\x87';	//Send answer back immediate command
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);		//Send off the commands
	dwNumBytesToSend = 0;			//Clear output buffer
	ftStatus = FT_Read(ftHandle, InputBuffer, 2, &dwNumBytesRead);//Read one byte from device receive buffer, may need try more times

	printf("Number of bytes read from EEPROM: %x\n", dwNumBytesRead);
	ByteDataRead = InputBuffer[0];		//return the data read from EEPROM
	printf("Success to read data byte [Read Section], value: %x\n", ByteDataRead);

	
	OutputBuffer[dwNumBytesToSend++] = '\x80'; 	//Command to set directions of lower 8 pins and force value on bits set as output
	OutputBuffer[dwNumBytesToSend++] = '\x02'; 	//Set SDA high, SCL low, WP disabled by SK at bit '0', DO, GPIOL0 at bit '1'
	OutputBuffer[dwNumBytesToSend++] = '\x13';	//Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯


	//////////////////////////////////////////////////////////
	// Set I2C stop condition and tristate the pins
	//////////////////////////////////////////////////////////
	HighSpeedSetI2CStop();

	//Send off the commands
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	dwNumBytesToSend = 0;			//Clear output buffer

	FT_Close(ftHandle);

	//printf("failed to read data byte [Read Section]!\n");
	while(1);
	return 0;
}

