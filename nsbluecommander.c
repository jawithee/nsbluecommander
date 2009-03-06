/*
 * 							COPYRIGHT
 *
 * National Semiconductor LMX3898 Bluetooth 
 * Copyright (C) 2008 Jason Withee
 * jwithee (at) eece (dot) maine (dot) edu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCES
#define _BSD_SOURCE /* cfmakeraw requirement */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <popt.h>
#include <ncurses.h>
#include "nsbluecommander.h"
#include "nsblueoptions.h"
#include "nsbluemenu.h"
#include "nsbluecommands.h"

#define BLUEMAXDATALEN 333

int serfd;
int (*print)(const char *, ...);
Options ops = {B9600, 0, NULL, NULL, 0, 0, 0, 0, 0, 0};

int main(int argc, char* argv[]) {
	int i;
	
	/* Bluetooth Commands */
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	int dataLen;

	if(parseOptions(argc, argv, &ops) < 0)
		return -1;
	
	if(ops.interactive) {
		interactiveMode();

	} else if(ops.gname) {
		gapReadLocalName();
#if 0
		sendPacket(PACKET_TYPE_REQUEST, GAP_READ_LOCAL_NAME, data, 0);
		if(getPacket(&type, &opcode, data, &dataLen) < 0)
			printf("Error Reading\n");
		printf("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
		printf("Data: ");
		for(i = 0; i < 2; i++)
			printf("0x%X ", (int) data[i]);
		printf("%s\n", (char *) &data[2]);
#endif
	}  else if(ops.sname) {
		strncpy((char *) &data[1], ops.sname, 40);
		data[40] = '\0'; /* Make sure null terminated */
		data[0] = (strlen(ops.sname) + 1) & 0xFF;
		dataLen = data[0] + 1;
		sendPacket(PACKET_TYPE_REQUEST, GAP_WRITE_LOCAL_NAME, data, dataLen);
		if(getPacket(&type, &opcode, data, &dataLen) < 0)
			printf("Error Reading\n");
		printf("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
		printf("Data: ");
		for(i = 0; i < dataLen; i++)
			printf("0x%X ", (int) data[i]);
		printf("\n");
	} else if (ops.gsecmode) {
		sendPacket(PACKET_TYPE_REQUEST, GAP_GET_SECURITY_MODE, data, 0);
		if(getPacket(&type, &opcode, data, &dataLen) < 0)
			printf("Error Reading\n");
		
		printf("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
		printf("Data: ");
		for (i = 0; i < 2; i++)
			printf("0x%X ", (int) data[i]);
		printf("%s\n", (char *) &data[2]);
	} else if (ops.secmode) {
		data[0] = ops.secmode;
		sendPacket(PACKET_TYPE_REQUEST, GAP_SET_SECURITY_MODE, data, 1);
		if(getPacket(&type, &opcode, data, &dataLen) < 0)
			printf("Error Reading\n");
		printf("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
		printf("Data: ");
		for(i = 0; i < dataLen; i++)
			printf("0x%X ", (int) data[i]);
		printf("\n");
	}

	fflush(NULL);
	close(serfd);

	return 0;
}

int init_serial_port(const char* port, speed_t speed) {
	struct termios term;
	
	if ((serfd = open(port, O_RDWR)) == -1)
		return -1;
	if (tcgetattr(serfd, &term) == -1)
		return -1;
	cfmakeraw(&term);
	if (cfsetospeed(&term, speed) == -1)
		return -1;
	if (tcsetattr(serfd, TCSANOW, &term) == -1)
		return -1;	
	tcflush(serfd, TCIOFLUSH);
	tcsendbreak(serfd, 0);
	
	return 0;
}


