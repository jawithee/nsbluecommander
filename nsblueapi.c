#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nsbluecommands.h"
#include "nsblueapi.h"
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

nsb_options nsb_global_options = {{1,0}};

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

RETURN_VALUE_T printNSBTConfirmError(uint8_t error) {
	if(error > 0 && error < (sizeof(NSBTERRORS)/sizeof(char **))) {
		printf("Error(0x%02X): %s\n", (int) error, NSBTERRORS[error]);
	}
	return RETURN_OK;
}

RETURN_VALUE_T sendPacket(int nsb_serfd, uint8_t type, uint8_t opcode, uint8_t * data, uint16_t dataLen) {
	uint8_t header[6];
	
	/* Create Header */
	header[0] = STX; /* Start Delimiter */
	header[1] = type;
	header[2] = opcode;
	header[3] = (dataLen) & 0xFF;
	header[4] = (dataLen >> 8) & 0xFF;
	header[5] = (header[1]+header[2]+header[3]+header[4]) & 0xFF;
	/* Send Header */
	write(nsb_serfd, header, 6);
	/* Send Data */
	write(nsb_serfd, data, dataLen);
	header[0] = ETX;
	/* Send End Delimiter */
	write(nsb_serfd, header, 1);
	return RETURN_OK;
}

RETURN_VALUE_T getPacket(int nsb_serfd, uint8_t * type, uint8_t * opcode, uint8_t * data, uint16_t * dataLen) {
	int readCount;
	int retVal = 0;
	uint8_t header[6];
	/* Get Header */
	readCount = 6;
	
	while(readCount > 0) {
		retVal = read(nsb_serfd, &header[6-readCount], readCount);
		if(retVal < 0) return RETURN_READ_ERROR; /* Error Reading */
		readCount -= retVal;
	}
	
	*type = header[1];
	*opcode = header[2];
	*dataLen = (header[4] << 8) | header[3];
	/* Read the data */
	readCount = *dataLen;
	while(readCount > 0) {
		retVal = read(nsb_serfd, &data[*dataLen-readCount], readCount);
		if(retVal < 0) return RETURN_READ_ERROR; /* Error Reading */
		readCount -= retVal;
	}
	/* Get end delimiter */
	readCount = 1;
	while(readCount > 0) {
		retVal = read(nsb_serfd, &header[1-readCount], readCount);
		if(retVal < 0) return RETURN_READ_ERROR; /* Error Reading */
		readCount -= retVal;
	}
	return RETURN_OK;
}

RETURN_VALUE_T nsb_gap_inquiry(int nsb_serfd, uint8_t Length, uint8_t NumResponses, uint8_t Mode, struct nsb_inquire_ind ** response, int * responseNumResponses, uint8_t * status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	struct nsb_inquire_ind * IndResponse = NULL;
	
	data[0] = Length;
	data[1] = NumResponses;
	data[2] = Mode;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_INQUIRY, data, 3);
	*responseNumResponses = 0;
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_DEVICE_FOUND && opcode != GAP_INQUIRY) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}
		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else if (type == PACKET_TYPE_INDICATION) {
			IndResponse = (struct nsb_inquire_ind *) realloc(IndResponse, ++(*responseNumResponses));
			memcpy(IndResponse[*responseNumResponses-1].BdAddr, data, sizeof(uint8_t)*6);
			memcpy(IndResponse[*responseNumResponses-1].DeviceClass, &data[6], sizeof(uint8_t)*3);
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	
	*response = IndResponse;
	return (retVal);
}

RETURN_VALUE_T nsb_inquire_ind_free(struct nsb_inquire_ind *response) {
	free(response);
	return (RETURN_OK);
};

RETURN_VALUE_T nsb_gap_remote_device_name(int nsb_serfd, uint8_t * BdAddr, char ** DeviceName, int * NameLength, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	*DeviceName = NULL;
	
	memcpy(data, BdAddr, sizeof(uint8_t)*6);
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_REMOTE_DEVICE_NAME, data, 6);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_REMOTE_DEVICE_NAME) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			if (*status == ERROR_OK) {
				/* data[1-6] is BDADDR */
				*NameLength = data[7];
				*DeviceName = (char *) malloc(sizeof(char) * (*NameLength));
				memcpy(*DeviceName, &data[8], *NameLength);
			} else {
				*DeviceName = (char *) malloc(sizeof(char));
				(*DeviceName)[0] = '\0';
				*NameLength = 0;
			}
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_set_default_link_timeout(int nsb_serfd, uint16_t LinkTimeout, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, &LinkTimeout, sizeof(uint8_t)*2);
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SET_DEFAULT_LINK_TIMEOUT, data, 2);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SET_DEFAULT_LINK_TIMEOUT) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_get_default_link_timeout(int nsb_serfd, uint16_t *LinkTimeout) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
		
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GET_DEFAULT_LINK_TIMEOUT, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GET_DEFAULT_LINK_TIMEOUT) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			memcpy(LinkTimeout, data, 2);
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_set_link_timeout(int nsb_serfd, uint8_t * BdAddr, uint16_t LinkTimeout, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, BdAddr, sizeof(uint8_t)*6);
	memcpy(&data[6], &LinkTimeout, sizeof(uint8_t)*2);
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_SET_LINK_TIMEOUT, data, 8);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_SET_LINK_TIMEOUT) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			/* data[1-6] are BdAddr */
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_get_link_timeout(int nsb_serfd, uint8_t * BdAddr, uint16_t *LinkTimeout, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, BdAddr, sizeof(uint8_t)*6);
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_GET_LINK_TIMEOUT, data, 6);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_GET_LINK_TIMEOUT) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			/* data[1-6] = BdAddr */
			memcpy(LinkTimeout, &data[7], 2);
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_store_default_con(int nsb_serfd, uint8_t Index, uint8_t LocalPort, uint8_t RemotePort, uint8_t * BdAddr, uint8_t TransportMode, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = Index;
	data[1] = LocalPort;
	data[2] = RemotePort;
	memcpy(&data[3], BdAddr, sizeof(uint8_t)*6);
	data[9] = TransportMode;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_STORE_DEFAULT_CON, data, 10);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_STORE_DEFAULT_CON) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_connect_default_con(int nsb_serfd, uint8_t Index, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = Index;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_CONNECT_DEFAULT_CON, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_CONNECT_DEFAULT_CON) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_get_list_default_con(int nsb_serfd, uint8_t Index, uint8_t *EntryStatus, uint8_t * RemoteBdAddress, uint8_t * LocalPort, uint8_t *RemotePort, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = Index;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_GET_LIST_DEFAULT_CON, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_GET_LIST_DEFAULT_CON) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			/* data[1] = index */
			*EntryStatus = data[2];
			memcpy(RemoteBdAddress, &data[3], sizeof(uint8_t)*6);
			*LocalPort = data[9];
			*RemotePort = data[10];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_delete_default_con(int nsb_serfd, uint8_t Index, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = Index;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_DELETE_DEFAULT_CON, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_DELETE_DEFAULT_CON) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			/* data[0] = index */
			*status = data[1];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_set_port_config(int nsb_serfd, uint8_t LocalPort, uint8_t BaudRate, uint8_t Portsettings, uint8_t FlowControl, uint8_t XonChar, uint8_t XoffChar, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = LocalPort;
	data[1] = BaudRate;
	data[2] = Portsettings;
	data[3] = FlowControl;
	data[4] = XonChar;
	data[5] = XoffChar;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_SET_PORT_CONFIG, data, 6);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_SET_PORT_CONFIG) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			/* data[1] = LocalPort */
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_spp_get_port_config(int nsb_serfd, uint8_t LocalPort, uint8_t * BaudRate, uint8_t * PortSettings, uint8_t * FlowControl, uint8_t * XonChar, uint8_t * XoffChar, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = LocalPort;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SPP_SET_PORT_CONFIG, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SPP_GET_PORT_CONFIG) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			/* Local Port = data[1] */
			*BaudRate = data[2];
			*PortSettings = data[3];
			*FlowControl = data[4];
			*XonChar = data[5];
			*XoffChar = data[6];
			
			/* data[1] = LocalPort */
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_read_local_name(int nsb_serfd, char ** DeviceName, int * NameLength, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	*DeviceName = NULL;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_READ_LOCAL_NAME, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_READ_LOCAL_NAME) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			*NameLength = data[1];
			*DeviceName = (char *) malloc(sizeof(char) * (*NameLength+1));
			memcpy(*DeviceName, &data[2], *NameLength+1);
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_write_local_name(int nsb_serfd, char * DeviceName, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;

	dataLen = strlen(DeviceName);
	if (dataLen > 39) dataLen = 39;
	memcpy(&data[1], DeviceName, dataLen);
	
	data[1+dataLen++] = '\0'; /* Null Terminate Data */
	data[0] = dataLen;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_WRITE_LOCAL_NAME, data, dataLen+1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_WRITE_LOCAL_NAME) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_read_local_bda(int nsb_serfd, uint8_t * BdAddr, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_READ_LOCAL_BDA, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_READ_LOCAL_BDA) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			memcpy(BdAddr, &data[1], sizeof(uint8_t)*6);
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_change_local_bdaddress(int nsb_serfd, uint8_t * BdAddr, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, BdAddr, 6);
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, CHANGE_LOCAL_BDADDRESS, data, 6);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != CHANGE_LOCAL_BDADDRESS) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_set_scanmode(int nsb_serfd, uint8_t Connectability, uint8_t Discoverability, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = Connectability;
	data[1] = Discoverability;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_SET_SCANMODE, data, 2);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_SET_SCANMODE) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_read_operation_mode(int nsb_serfd, uint8_t *mode, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, READ_OPERATION_MODE, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != READ_OPERATION_MODE) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			*mode = data[1];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_write_operation_mode(int nsb_serfd, uint8_t mode, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = mode;
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, WRITE_OPERATION_MODE, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != WRITE_OPERATION_MODE) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_set_default_link_latency(int nsb_serfd, uint16_t latency, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, &latency, 2);
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SET_DEFAULT_LINK_LATENCY, data, 2);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SET_DEFAULT_LINK_LATENCY) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_get_default_link_latency(int nsb_serfd, uint16_t *latency) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GET_DEFAULT_LINK_LATENCY, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GET_DEFAULT_LINK_LATENCY) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			memcpy(&latency, data, 2);
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_get_security_mode(int nsb_serfd, uint8_t *mode, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_GET_SECURITY_MODE, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_GET_SECURITY_MODE) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			*mode = data[1];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_set_security_mode(int nsb_serfd, uint8_t mode, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[1] = mode;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_SET_SECURITY_MODE, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_SET_SECURITY_MODE) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_remove_pairing(int nsb_serfd, uint8_t *BdAddr, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, BdAddr, 6);
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_REMOVE_PAIRING, data, 6);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_REMOVE_PAIRING) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_list_paired_devices(int nsb_serfd, uint8_t **BdAddrList, uint8_t * DeviceCount, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_LIST_PAIRED_DEVICES, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_LIST_PAIRED_DEVICES) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			*DeviceCount = data[1];
			*BdAddrList = (uint8_t *) malloc(sizeof(uint8_t)*6*(*DeviceCount));
			memcpy(*BdAddrList, &data[2], 6*(*DeviceCount));
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_get_fixed_pin(int nsb_serfd, uint8_t ** Pincode, uint8_t * PinLength, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_GET_FIXED_PIN, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_GET_FIXED_PIN) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			*PinLength = data[1];
			*Pincode = (uint8_t*) malloc(sizeof(uint8_t)*(*PinLength));
			memcpy(*Pincode, &data[2], *PinLength);
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_gap_set_fixed_pin(int nsb_serfd, uint8_t * Pincode, uint8_t PinLength, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	memcpy(data, Pincode, PinLength);
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GAP_SET_FIXED_PIN, data, PinLength);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GAP_SET_FIXED_PIN) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_set_event_filter(int nsb_serfd, uint8_t Filter, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = Filter;
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, SET_EVENT_FILTER, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != SET_EVENT_FILTER) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_get_event_filter(int nsb_serfd, uint8_t * Filter) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, GET_EVENT_FILTER, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != GET_EVENT_FILTER) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*Filter = data[0];
			
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_reset(int nsb_serfd, uint8_t ** Version) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, RESET, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != DEVICE_READY) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_INDICATION ) {
			*Version = (uint8_t *) malloc( sizeof(uint8_t) * data[0]+1);
			memcpy(*Version, &data[1], data[0]+1);
			Version[data[0]] = '\0';			
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_restore_factory_settings(int nsb_serfd, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, RESTORE_FACTORY_SETTINGS, data, 0);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != RESTORE_FACTORY_SETTINGS) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_change_uart_speed(int nsb_serfd, uint8_t UartSpeed, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = UartSpeed;
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, CHANGE_NVS_UART_SPEED, data, 1);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != CHANGE_NVS_UART_SPEED) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_change_uart_settings(int nsb_serfd, uint8_t ParityType, uint8_t StopBits, uint8_t *status) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN];
	uint16_t dataLen;
	int endRec = 0;
	int retVal = 0;
	
	data[0] = ParityType;
	data[1] = StopBits;
	sendPacket(nsb_serfd, PACKET_TYPE_REQUEST, CHANGE_UART_SETTINGS, data, 2);
	
	while (!endRec) {
		retVal = getPacket(nsb_serfd, &type, &opcode, data, &dataLen);
		if (retVal != RETURN_OK)
			break;
		if (opcode != CHANGE_UART_SETTINGS) {
			retVal = RETURN_INCORRECT_OPCODE; /* WRONG_OPCODE */
			break;
		}

		if ( type == PACKET_TYPE_CONFIRM ) {
			*status = data[0];
			break;
		} else {
			retVal = RETURN_INCORRECT_TYPE; /* WRONG_TYPE */
			break;
		}
	}
	return (retVal);
}

RETURN_VALUE_T nsb_bdaddr_to_string (char * BdAddrStr, uint8_t * BdAddr) {
//	*BdAddrStr = (char*) malloc(sizeof(char)*18);
	snprintf(BdAddrStr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", BdAddr[5], BdAddr[4], BdAddr[3], BdAddr[2], BdAddr[1], BdAddr[0]);
	return RETURN_OK;
}

RETURN_VALUE_T nsb_string_to_bdaddr (uint8_t * BdAddr, char * BdAddrStr) {
	unsigned int b[6];
//	*BdAddr = (uint8_t *) malloc(sizeof(uint8_t)*6);
	sscanf(BdAddrStr,"%02X:%02X:%02X:%02X:%02X:%02X", &b[5], &b[4], &b[3], &b[2], &b[1], &b[0]);
	(BdAddr)[0] = b[0];
	(BdAddr)[1] = b[1];
	(BdAddr)[2] = b[2];
	(BdAddr)[3] = b[3];
	(BdAddr)[4] = b[4];
	(BdAddr)[5] = b[5];
	return RETURN_OK;
}

RETURN_VALUE_T nsb_init_serial_port(int * nsb_serfd, const char* port, speed_t speed) {
	struct termios term;
	
	if ((*nsb_serfd = open(port, O_RDWR)) == -1)
		return RETURN_SERIAL_PORT_OPEN_ERROR;
	if (tcgetattr(*nsb_serfd, &term) == -1)
		return -1;
	cfmakeraw(&term);
	if (cfsetospeed(&term, speed) == -1)
		return RETURN_SERIAL_PORT_SET_BAUD_ERROR;
	if (tcsetattr(*nsb_serfd, TCSANOW, &term) == -1)
		return RETURN_SERIAL_PORT_ERROR;	
	tcflush(*nsb_serfd, TCIOFLUSH);
	tcsendbreak(*nsb_serfd, 0);

	return 0;
}

RETURN_VALUE_T nsb_close_serial_port(int nsb_serfd) {
	close(nsb_serfd);
	return 0;
}
