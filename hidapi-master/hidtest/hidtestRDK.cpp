/*******************************************************
 Windows HID simplification

 Hacked for Sonavation

********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"

unsigned char echocmd[] = {
    0x66, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x59, 0xaf, 0xac, 0xfb, 0x45, 0x18, 0xb6, 0x10, 0xcc, 0xbd, 0x43, 0x8b, 0x31, 0xb7, 0xa7, 0x9b,
    0xfd, 0xd4, 0x4a, 0x09, 0x36, 0x3a, 0xc6, 0xd9, 0xcb, 0xec, 0x3f, 0xa3, 0x90, 0x0f, 0x5b, 0x7b, 0x00, 0x00, 0x16,
    0x00, 0x00, 0x00, 0x22, 0x0f, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif
/**
*  Frame sizes.
*/
#define AES_IV_LEN                  16
#define SPP_TAG_PREFIX_SIZE         1
#define SPP_TAG_CATAGORY_SIZE       1
#define SPP_TAG_SIZE_SIZE           4
#define SPP_CMD_SIZE                (SPP_TAG_PREFIX_SIZE + SPP_TAG_CATAGORY_SIZE + SPP_TAG_SIZE_SIZE)
#define SPP_HMAC_SIZE               32
#define SPP_FRAME_SIZE_SIZE         4
#define SPP_KEYID_SIZE              2
#define SPP_KEYID_OFFSET            (SPP_FRAME_SIZE_SIZE + 2)
#define SPP_PACKET_TYPE_OFFSET      (SPP_FRAME_SIZE_SIZE + 1)
#define SPP_HEADER_SIZE             (SPP_FRAME_SIZE_SIZE  + SPP_KEYID_SIZE + AES_IV_LEN + SPP_HMAC_SIZE)
#define AES_KEY_LEN                 16
int SendHIDCommand(hid_device *handle,unsigned char *cmd)
{
    int len = *((unsigned int*)cmd);
    unsigned char buffer[65];
    int sent = 0;
    int res = 0;
    buffer[0] = 0;

    while (sent  < len)
    {
        memcpy(&buffer[1], &cmd[sent], 64);
        res = hid_write(handle, &buffer[0], 65);
        if (res < 0) {
            printf("Unable to write()\n");
            printf("Error: %ls\n", hid_error(handle));
        }
        sent += 64;
    }
    return 0;
}
int readHIDResponse(hid_device *handle, unsigned char **resp,int timeout)
{
    unsigned char readbuffer[65];
    int readBytes = 0;
    int i = 0;
    *resp = 0;
    readBytes = hid_read_timeout(handle, readbuffer, 64, timeout);
    if (!readBytes)
        return -1; // Timed out.
    // First byte is length
    int len = *((unsigned int*)readbuffer) + 4;
    // we are going to grab 64 byte chunks, so we need multiples of 64
    *resp = (unsigned char *) malloc((len / 64 + 1) * 64 );
    memcpy(*resp, readbuffer,64);
    len -= readBytes;
    ++i;
    while (len > 0)
    {
        readBytes = hid_read_timeout(handle, readbuffer, 64, timeout);
        if (readBytes)
        {
            memcpy((*resp) +(i*64), readbuffer, 64);
        }
        len -= readBytes;
        ++i;

    }
    return 0;
}
int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	struct hid_device_info *devs, *cur_dev;
	
	if (hid_init())
		return -1;

	devs = hid_enumerate(0x1c37, 0xf1d0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
        printf("  Usage Page:   0x%x\n", cur_dev->usage_page);
        printf("  Usage:        %d\n", cur_dev->usage);
        printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;
	

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
    //  1c37 f1d0
	handle = hid_open(0x1c37, 0xf1d0, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	

    while (1)
    {
        unsigned char* resp;
        unsigned char* data;
        int datalen;
        SendHIDCommand(handle,echocmd);
        readHIDResponse(handle, &resp,1000);
        if (resp)
        {
            int len = *((unsigned int*)resp) ;
            printf("Received: %d bytes ", len);
            data = resp + SPP_HEADER_SIZE;
            datalen = *((int*)(data + 8));
            if (memcmp((data+12), (echocmd + SPP_HEADER_SIZE + 12), datalen))
            {
                printf("Error in response\n");
            }
            else
            {
                printf("echo response OK\n");

            }
            free(resp);
        }
        else
        {
            printf("Got no response\n");
        }
       
    }

    hid_set_nonblocking(handle, 1);
	// Try to read from the device. There shoud be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);

	// Send a Feature Report to the device
	buf[0] = 0x2;
	buf[1] = 0xa0;
	buf[2] = 0x0a;
	buf[3] = 0x00;
	buf[4] = 0x00;
	res = hid_send_feature_report(handle, buf, 17);
	if (res < 0) {
		printf("Unable to send a feature report.\n");
	}

	memset(buf,0,sizeof(buf));

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		printf("Unable to get a feature report.\n");
		printf("%ls", hid_error(handle));
	}
	else {
		// Print out the returned buffer.
		printf("Feature Report\n   ");
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}

	memset(buf,0,sizeof(buf));

	// Toggle LED (cmd 0x80). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 17);
	if (res < 0) {
		printf("Unable to write()\n");
		printf("Error: %ls\n", hid_error(handle));
	}
	

	// Request state (cmd 0x81). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x81;
	hid_write(handle, buf, 17);
	if (res < 0)
		printf("Unable to write() (2)\n");

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	res = 0;
	while (res == 0) {
		res = hid_read(handle, buf, sizeof(buf));
		if (res == 0)
			printf("waiting...\n");
		if (res < 0)
			printf("Unable to read()\n");
		#ifdef WIN32
		Sleep(500);
		#else
		usleep(500*1000);
		#endif
	}

	printf("Data read:\n   ");
	// Print out the returned buffer.
	for (i = 0; i < res; i++)
		printf("%02hhx ", buf[i]);
	printf("\n");

	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	system("pause");
#endif

	return 0;
}
