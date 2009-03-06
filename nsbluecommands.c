#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include "nsbluecommands.h"
#include "nsblueoptions.h"

extern int serfd;
extern int (*print)(const char *, ...);
extern Options ops;

char * NSBTERRORS[] = {
	"No error.",
	"The number of bytes in the request does not correspond to the protocol specification",
	"The given duration value is not valid according to the specification.",
	"The selected mode is not valid according to the specification",
	"A timeout occurred.",
	"An unknown error occurred.",
	"The number of bytes in the name string is longer than the maximum specified value.",
	"The given discoverability parameter does not contain a valid value according to the specification.",
	"The given connectability parameter does not contain a valid value according to the specification.",
	"The given security mode is not a valid Bluetooth security mode.",
	"No link key exists for the given Bluetooth address",
	"The connection setup failed due to unknown reasons.",
	"The returned number of services is too large to be handled by the LMX9838. The answer is truncated",
	"The SDP result from the remote device is too large to be handled by the LMX9838 due to ram limitations",
	"It is currently not possible to enter the selected test mode.",
	"The given test mode is not a valid test mode according to the specification",
	"The LMX9838 will change the Bluetooth address to the NSC address.",
	"The selected UART speed value is not valid according to the specification.",
	"The given port value is larger than the maximum specified value.",
	"The given state value is not a valid state according to the specification",
	"The given identifier is larger than the maximum specified value.",
	"The service record is already enabled/disabled.",
	"The given authentication value is not a valid value according to the specification.",
	"The given encryption value is not a valid value according to the specification.",
	"The maximum number of service records, which the LMX9838 is able to store, is reached.",
	"An error occurred while writing to flash. The service record may not be stored.",
	"The given role value is not a valid value according to the specification.",
	"Limits exceeded (Parameter(s) violates limits).",
	"Unexpected at this moment.",
	"Could not send at this moment, no reason specified.",
	"Currently no room in buffer, try again later.",
	"Trying to use an inexistent connection.",
	"Port number out of range.",
	"Port is closed.",
	"Connection establishment on a PORT that has a connection.",
	"Transparent mode attempted while more than 1 connection active.",
	"Trying to store a default connection when a transparent default connection is already stored, or trying to store a transparent default connection when another connection is already stored.",
	"Trying to connection to a default connection, which is not stored.",
	"NS_PROGRESSING Trying to start connecting to default connections when default connection establishment is already progressing.",
	"Other error.",
	"Trying to enable a SDP record which is not stored. Wrong identifier.",
	"Faxclass parameter must be 0 or 1.",
	"The given number of supported formats excesses the specified maximum number of supported formats.",
	"The given number of data stores excess the specified maximum number of data stores.",
	"Attempt to change low power mode failed",
	"The given link policy value is out of range",
	"The pin code length field is too large.",
	"The given parity check is out of range",
	"The given number of stop bits is out of range",
	"The given link timeout value is out of range",
	"The command is not allowed.",
	"The given Audio CODEC type is out of range.",
	"The given Audio Air format is out of range.",
	"The SDP record is too long.",
	"The SDP server failed to create the SDP record.",
	"The selected codec does not support volume control.",
	"The packet type specified in the request is not valid.",
	"The codec (slave) settings is invalid.",
};

int printNSBTConfirmError(uint8_t error) {
	if(error > 0 && error < (sizeof(NSBTERRORS)/sizeof(char **))) {
		print("Error(0x%02X): %s\n", (int) error, NSBTERRORS[error]);
	}
	return 0;
}
	
int sendPacket(uint8_t type, uint8_t opcode, uint8_t * data, int dataLen) {
	uint8_t header[6];
	/* Create Header */
	header[0] = STX; /* Start Delimiter */
	header[1] = type;
	header[2] = opcode;
	header[3] = (dataLen) & 0xFF;
	header[4] = (dataLen >> 8) & 0xFF;
	header[5] = (header[1]+header[2]+header[3]+header[4]) & 0xFF;
	/* Send Header */
	write(serfd, header, 6);
	/* Send Data */
	write(serfd, data, dataLen);
	header[0] = ETX;
	/* Send End Delimiter */
	write(serfd, header, 1);
	return 0;
}

int getPacket(uint8_t * type, uint8_t * opcode, uint8_t * data, int * dataLen) {
	int readCount;
	int retVal = 0;
	uint8_t header[6];
	/* Get Header */
	readCount = 6;
	while(readCount > 0) {
		retVal = read(serfd, &header[6-readCount], readCount);
		if(retVal < 0) return -1; /* Error Reading */
		readCount -= retVal;
	}
	
	*type = header[1];
	*opcode = header[2];
	*dataLen = (header[4] << 8) | header[3];
	/* Read the data */
	readCount = *dataLen;
	while(readCount > 0) {
		retVal = read(serfd, &data[*dataLen-readCount], readCount);
		if(retVal < 0) return -1; /* Error Reading */
		readCount -= retVal;
	}
	/* Get end delimiter */
	readCount = 1;
	while(readCount > 0) {
		retVal = read(serfd, &header[1-readCount], readCount);
		if(retVal < 0) return -1; /* Error Reading */
		readCount -= retVal;
	}	
	return 0;
}

void gapReadLocalName(void) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	int dataLen;
	int i;

	if(ops.interactive) clear();
	sendPacket(PACKET_TYPE_REQUEST, GAP_READ_LOCAL_NAME, data, 0);
		if(getPacket(&type, &opcode, data, &dataLen) < 0)
			print("Error Reading\n");
		print("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
		print("Data: ");
		for(i = 0; i < 2; i++)
			print("0x%X ", (int) data[i]);
		print("%s\n", (char *) &data[2]);
	if(ops.interactive) {
		refresh();
		while(wgetch(stdscr) != 'q');
		clear();
	}
}

void gapWriteLocalName(void) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	int dataLen;
	int i;
	
	if(ops.interactive) { 
		 clear();
		 echo();
	}
	print("Name: ");
	if(ops.interactive) scanw("%s", &data[1]);
	else strncpy((char *) &data[1], ops.sname, 40);
	data[40] = '\0'; /* Make sure null terminated */
	data[0] = (strlen((const char *) &data[1]) + 1) & 0xFF;
	dataLen = data[0] + 1;
		sendPacket(PACKET_TYPE_REQUEST, GAP_WRITE_LOCAL_NAME, data, dataLen);
		if(getPacket(&type, &opcode, data, &dataLen) < 0)
			print("Error Reading\n");
		print("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
		print("Data: ");
		for(i = 0; i < dataLen; i++)
			print("0x%X ", (int) data[i]);
		print("\n");	

	if(ops.interactive) {
		refresh();
		noecho();
		while(wgetch(stdscr) != 'q');
		clear();
	}
}

void gapGetSecurityMode(void) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	int dataLen;
	int i;
	
	if(ops.interactive) clear();
	
	sendPacket(PACKET_TYPE_REQUEST, GAP_GET_SECURITY_MODE, data, 0);
	
	if(getPacket(&type, &opcode, data, &dataLen) < 0)
		print("Error With Response\n");
	
	print("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
	print("Data: ");
	for (i = 0; i < 2; i++)
		print("0x%X ", (int) data[i]);
	print("%s\n", (char *) &data[2]);

	if(ops.interactive)	{
		refresh();
		while(wgetch(stdscr) != 'q');
		clear();
	}
}

