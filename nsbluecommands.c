#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nsbluecommands.h"
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

void * memcpyle(void * dest, const void *src, size_t n) {
	size_t i;
	uint8_t * destination = dest;
	const uint8_t * source = src;
	for ( i = 0 ; i < n; i++) { /* Reverse Order of Bytes */
		destination[i] = source[(n-1)-i];
	}
	return (dest);
}

int main() {
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
	
	int nsb_serfd;
	
	nsb_init_serial_port(&nsb_serfd, "/dev/ttyUSB0", B9600);
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
