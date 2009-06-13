#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nsblueapi.h"
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <popt.h>

struct bt_config_options_t {
	int inquiry;
	int remote_device_name;
	int read_local_name;
	int write_local_name;
	int get_fixed_pin;
	int set_fixed_pin;
	int get_pin;
	int read_local_name;
	int get_secuirty_mode;
	int set_secuirty_mode;
	int set_link_policy;
	int get_link_policy;
	int set_port_config;
	int get_port_config;
}

struct bt_config_options_t btCfg;

struct baudmap_t {
        int intbaud;
        speed_t baud;
};

struct baudmap_t baudmap[] = {
	{ 9600, B9600},
	{ 19200, B19200},
	{ 38400, B38400},
	{ 57600, B57600},
	{ 115200, B115200},
	{ 230400, B230400}
};

void * memcpyle(void * dest, const void *src, size_t n) {
	size_t i;
	uint8_t * destination = dest;
	const uint8_t * source = src;
	for ( i = 0 ; i < n; i++) { /* Reverse Order of Bytes */
		destination[i] = source[(n-1)-i];
	}
	return (dest);
}

void usage(poptContext optCon, int exitcode, char *error){
	poptPrintUsage(optCon, stderr, 0);
	if (error)
		fprintf(stderr, "%s", error);
	exit(exitcode);
}

int main(int argc, char ** argv) {
	int Length = 0x0A, NumResponses = 0x00, Mode = 0x00;
	struct nsb_inquire_ind * Responses;
	int numResponses;
	uint8_t status;
	char * DeviceName;
	char BdAddrStr[41];
	uint8_t BdAddr[6];
	int NameLength;
	int i;
	uint16_t LinkTimeout = 0x00;
	uint8_t Filter;
	uint8_t * Version;
	
	speed_t baudRate = B9600; /* Default Baud */
	int intBaudRate = 0; /* Baud Rate in integer format */
	char * serialPort = "/dev/ttyUSB0"; /* Default Serial Port */
	
	int nsb_serfd;
	
	poptContext optCon;
	int c;

	struct poptOption optionsTable[] = {
		{ "baud", 'b', POPT_ARG_INT, &intBaudRate, 'b', "BAUD rate (default: 9600)", "9600|19200|38400|57600|115200" },
		{ "serial-port", 's', POPT_ARG_STRING, &serialPort, 0, "Serial Port", "device" },
		POPT_AUTOHELP POPT_TABLEEND
	};

	optCon = poptGetContext(NULL, argc, (const char**) argv, optionsTable, 0);
	while ((c = poptGetNextOpt(optCon)) >= 0) {
		switch (c) {
			case 'b':
				for (i = 0; i < 6; i++) {
					if (baudmap[i].intbaud == intBaudRate) {
						baudRate = baudmap[i].baud;
						break;
					}
				}
				if (i >= 6)
					printf("Unknow BAUD, defaulting to 9600\n");
				break;
		}
	}
	
	nsb_init_serial_port(&nsb_serfd, serialPort, baudRate);
#if 0
	if (nsb_gap_inquiry(nsb_serfd, Length, NumResponses, Mode, &Responses, &numResponses, (uint8_t *) &status) != RETURN_OK) {
		printf("Error Returning Value\n");
	} else {
		for ( i = 0; i < numResponses; i++) {
			if(nsb_gap_remote_device_name(nsb_serfd, Responses[i].BdAddr, &DeviceName, &NameLength, &status) != RETURN_OK) {
				printf("Error Reading Name i=%d\n", i);
			} else {
				nsb_bdaddr_to_string(&BdAddrStr, Responses[i].BdAddr);
				printf("BdAddr: %s  Name: %s\n", BdAddrStr, DeviceName);
			}
		}
	}
	nsb_inquire_ind_free(Responses);
	

	LinkTimeout = 0x7D00;
	nsb_set_default_link_timeout(nsb_serfd, LinkTimeout, &status);
	LinkTimeout = 0x000;
	if (nsb_get_default_link_timeout(nsb_serfd, &LinkTimeout) != RETURN_OK) {
		printf ("Error Getting Link");
	} else {
		printf( "Link Timeout = %04X\n", (unsigned int) LinkTimeout);
	}
#endif	
#if 0
	nsb_gap_read_local_name(nsb_serfd, &DeviceName, &NameLength, &status);
	printNSBTConfirmError(status);
	
	nsb_gap_read_local_bda(nsb_serfd, BdAddr, &status);
	printNSBTConfirmError(status);
	nsb_bdaddr_to_string(BdAddrStr, BdAddr);
	printf("Old:\nBdAddr: %s  Name: %s\n", BdAddrStr, DeviceName);

	nsb_gap_write_local_name(nsb_serfd, "LTShow", &status);
	printNSBTConfirmError(status);



	nsb_gap_read_local_name(nsb_serfd, &DeviceName, &NameLength, &status);
	printNSBTConfirmError(status);
	
	nsb_gap_read_local_bda(nsb_serfd, BdAddr, &status);
	printNSBTConfirmError(status);
	nsb_bdaddr_to_string(BdAddrStr, BdAddr);
	printf("New:\nBdAddr: %s  Name: %s\n", BdAddrStr, DeviceName);	
	
	nsb_reset(nsb_serfd, &Version);
	printf("Version: %s\n", (char *)Version);
	
#else
		
	nsb_gap_read_local_name(nsb_serfd, &DeviceName, &NameLength, &status);
	printNSBTConfirmError(status);
	
	nsb_gap_read_local_bda(nsb_serfd, BdAddr, &status);
	printNSBTConfirmError(status);
	nsb_bdaddr_to_string(BdAddrStr, BdAddr);
	printf("BdAddr: %s  Name: %s\n", BdAddrStr, DeviceName);
	
#endif	

#if 0	
	nsb_gap_read_local_bda(nsb_serfd, BdAddr, &status);
	printNSBTConfirmError(status);
	nsb_bdaddr_to_string(BdAddrStr, BdAddr);
	printf("BdAddr: %s  Name: %s\n", BdAddrStr, DeviceName);

	nsb_get_event_filter(nsb_serfd, &Filter);
	printf("Filter: 0x%02X\n", Filter);

	nsb_restore_factory_settings(nsb_serfd, &status);
	printNSBTConfirmError(status);

	nsb_reset(&Version);
	printf("Version: %s\n", (char *)Version);
#endif
	nsb_close_serial_port(nsb_serfd);
	return 0;
}
