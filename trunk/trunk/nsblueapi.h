#ifndef NSBLUEAPI__H_
#define NSBLUEAPI__H_
#include <stdint.h>
#include <sys/time.h>
#include <termios.h>

typedef struct {
	struct timeval read_timeout;
} nsb_options;

struct nsb_inquire_ind {
	uint8_t BdAddr[6];
	uint8_t DeviceClass[3];
};

typedef enum {
	STATUS_OK = 0,
	STATUS_ACK,
	STATUS_NAK,
	STATUS_UNEXPECTED_RESPONSE,
	STATUS_READ_TIMEOUT,
	STATUS_SERIAL_PORT_ERROR,
	STATUS_OPEN_FILE_FAILED,
	STATUS_COMMAND_ERROR,
	STATUS_PROGRAM_TOO_BIG,
	STATUS_SELECT_ERROR,
	STATUS_RET_SUCCESS,
	STATUS_RET_UNKNOWN_CMD,
	STATUS_RET_INVALID_CMD,
	STATUS_RET_INVALID_ADDR,
	STATUS_RET_FLASH_FAIL,
	STATUS_FTDI_READ_FAIL,
	STATUS_FTDI_WRITE_FAIL
} STATUS_T;

typedef enum {
	RETURN_OK = 0,
	RETURN_READ_ERROR,
	RETURN_HEADER_ERROR,
	RETURN_START_DELIMITER_ERROR,
	RETURN_END_DELIMITER_ERROR,
	RETURN_INCORRECT_OPCODE,
	RETURN_INCORRECT_TYPE,
	RETURN_ERROR,
	RETURN_SERIAL_PORT_OPEN_ERROR,
	RETURN_SERIAL_PORT_SET_BAUD_ERROR,
	RETURN_SERIAL_PORT_ERROR
} RETURN_VALUE_T;


RETURN_VALUE_T nsb_gap_inquiry(int nsb_serfd, uint8_t Length, uint8_t NumResponses, uint8_t Mode, struct nsb_inquire_ind ** response, int * responseNumResponses, uint8_t * status);
RETURN_VALUE_T nsb_inquire_ind_free(struct nsb_inquire_ind *response);
RETURN_VALUE_T nsb_gap_remote_device_name(int nsb_serfd, uint8_t * BdAddr, char ** DeviceName, int * NameLength, uint8_t *status);
RETURN_VALUE_T nsb_set_default_link_timeout(int nsb_serfd, uint16_t LinkTimeout, uint8_t *status);
RETURN_VALUE_T nsb_get_default_link_timeout(int nsb_serfd, uint16_t *LinkTimeout);
RETURN_VALUE_T nsb_spp_set_link_timeout(int nsb_serfd, uint8_t * BdAddr, uint16_t LinkTimeout, uint8_t *status);
RETURN_VALUE_T nsb_spp_get_link_timeout(int nsb_serfd, uint8_t * BdAddr, uint16_t *LinkTimeout, uint8_t *status);
RETURN_VALUE_T nsb_spp_store_default_con(int nsb_serfd, uint8_t Index, uint8_t LocalPort, uint8_t RemotePort, uint8_t * BdAddr, uint8_t TransportMode, uint8_t *status);
RETURN_VALUE_T nsb_spp_connect_default_con(int nsb_serfd, uint8_t Index, uint8_t *status);
RETURN_VALUE_T nsb_spp_get_list_default_con(int nsb_serfd, uint8_t Index, uint8_t *EntryStatus, uint8_t * RemoteBdAddress, uint8_t * LocalPort, uint8_t *RemotePort, uint8_t *status);
RETURN_VALUE_T nsb_spp_delete_default_con(int nsb_serfd, uint8_t Index, uint8_t *status);
RETURN_VALUE_T nsb_spp_set_port_config(int nsb_serfd, uint8_t LocalPort, uint8_t BaudRate, uint8_t Portsettings, uint8_t FlowControl, uint8_t XonChar, uint8_t XoffChar, uint8_t *status);
RETURN_VALUE_T nsb_spp_get_port_config(int nsb_serfd, uint8_t LocalPort, uint8_t * BaudRate, uint8_t * PortSettings, uint8_t * FlowControl, uint8_t * XonChar, uint8_t * XoffChar, uint8_t *status);
RETURN_VALUE_T nsb_gap_read_local_name(int nsb_serfd, char ** DeviceName, int * NameLength, uint8_t *status);
RETURN_VALUE_T nsb_gap_write_local_name(int nsb_serfd, char * DeviceName, uint8_t *status);
RETURN_VALUE_T nsb_gap_read_local_bda(int nsb_serfd, uint8_t * BdAddr, uint8_t *status);
RETURN_VALUE_T nsb_change_local_bdaddress(int nsb_serfd, uint8_t * BdAddr, uint8_t *status);
RETURN_VALUE_T nsb_gap_set_scanmode(int nsb_serfd, uint8_t Connectability, uint8_t Discoverability, uint8_t *status);
RETURN_VALUE_T nsb_read_operation_mode(int nsb_serfd, uint8_t *mode, uint8_t *status);
RETURN_VALUE_T nsb_write_operation_mode(int nsb_serfd, uint8_t mode, uint8_t *status);
RETURN_VALUE_T nsb_set_default_link_latency(int nsb_serfd, uint16_t latency, uint8_t *status);
RETURN_VALUE_T nsb_get_default_link_latency(int nsb_serfd, uint16_t *latency);
RETURN_VALUE_T nsb_gap_get_security_mode(int nsb_serfd, uint8_t *mode, uint8_t *status);
RETURN_VALUE_T nsb_gap_set_security_mode(int nsb_serfd, uint8_t mode, uint8_t *status);
RETURN_VALUE_T nsb_gap_remove_pairing(int nsb_serfd, uint8_t *BdAddr, uint8_t *status);
RETURN_VALUE_T nsb_gap_list_paired_devices(int nsb_serfd, uint8_t **BdAddrList, uint8_t * DeviceCount, uint8_t *status);
RETURN_VALUE_T nsb_gap_get_fixed_pin(int nsb_serfd, uint8_t ** Pincode, uint8_t * PinLength, uint8_t *status);
RETURN_VALUE_T nsb_gap_set_fixed_pin(int nsb_serfd, uint8_t * Pincode, uint8_t PinLength, uint8_t *status);
RETURN_VALUE_T nsb_set_event_filter(int nsb_serfd, uint8_t Filter, uint8_t *status);
RETURN_VALUE_T nsb_get_event_filter(int nsb_serfd, uint8_t * Filter);
RETURN_VALUE_T nsb_reset(int nsb_serfd, uint8_t ** Version);
RETURN_VALUE_T nsb_restore_factory_settings(int nsb_serfd, uint8_t *status);
RETURN_VALUE_T nsb_change_uart_speed(int nsb_serfd, uint8_t UartSpeed, uint8_t *status);
RETURN_VALUE_T nsb_change_uart_settings(int nsb_serfd, uint8_t ParityType, uint8_t StopBits, uint8_t *status);
RETURN_VALUE_T nsb_bdaddr_to_string (char * BdAddrStr, uint8_t * BdAddr);
RETURN_VALUE_T nsb_string_to_bdaddr (uint8_t * BdAddr, char * BdAddrStr);

RETURN_VALUE_T nsb_init_serial_port(int * nsb_serfd, const char* port, speed_t speed);
RETURN_VALUE_T nsb_close_serial_port(int nsb_serfd);

RETURN_VALUE_T printNSBTConfirmError(uint8_t error);

#endif
