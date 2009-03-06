#include <stdio.h>
#include <ncurses.h>
#include <popt.h>
#include <unistd.h>
#include <stdlib.h>
#include "nsblueoptions.h"
#include "nsbluecommander.h"

extern int (*print)(const char *, ...);

struct baudmap_t baudmap[] = {
		{ 9600, B9600},
		{ 19200, B19200},
		{ 38400, B38400},
		{ 57600, B57600},
		{ 115200, B115200}
};

void usage(poptContext optCon, int exitcode, char *error) {
	poptPrintUsage(optCon, stderr, 0);
	if (error)
		fprintf(stderr, "%s", error);
	exit(exitcode);
}

int parseOptions(int argc, char ** argv, Options * ops) {
	int i;
	poptContext optCon;
	int c;
	struct poptOption optionsTable[] = {
			{ "baud", 'b', POPT_ARG_INT, &ops->baud, 'b', "BAUD rate (default: 9600)", "9600|19200|38400|57600|115200" },
			{ "serial-port", 's', POPT_ARG_STRING, &ops->sport, 0, "Serial Port", "device" },
			{ "set-name", 'n', POPT_ARG_STRING, &ops->sname, 0, "Get Bluetooth Name", "name"},
			{ "get-name", '\0', POPT_ARG_NONE, &ops->gname, 0, "Set Bluetooth Name", NULL},
			{ "get-secmode", '\0', POPT_ARG_NONE, &ops->gsecmode, 0, "Get Security Mode", NULL},
			{ "set-secmode", '\0', POPT_ARG_INT, &ops->secmode, 'S', "Set Secuirty Mode", "1-4"},
			{ "interactive", 'i', POPT_ARG_NONE, &ops->interactive, 0, "Interactive Mode", NULL},
			{ "surpress-errors", '\0', POPT_ARG_VAL, &ops->printErrors, 0, "Supress Error", NULL},
			{ "display-errors", 'e', POPT_ARG_VAL, &ops->printErrors, 1, "Supress Error", NULL},
			{ "verbose", 'v', POPT_ARG_NONE, &ops->verbose, 0, "Verbose Mode", NULL},
			POPT_AUTOHELP POPT_TABLEEND
	};

	optCon = poptGetContext(NULL, argc, (const char**) argv, optionsTable, 0);

	while ((c = poptGetNextOpt(optCon)) >= 0) {
		switch (c) {
			case 'b':
				for (i = 0; i < 5; i++) {
					if (baudmap[i].intbaud == ops->baud) {
						ops->speed = baudmap[i].baud;
						i = -1;
						break;
					}
				}
				if (i != -1)
					printf("Unknow BAUD, defaulting to 9600\n");
				break;
			case 'S':
				if (ops->secmode < 1 || ops->secmode > 4) {
					printf("%d is an unsupported security mode\n", ops->secmode);
					return -1;
				}
				break;
		}
	}
	if (ops->sport == NULL)
		usage(optCon, -1, "\nMust specify a serial port: e.g. -s /dev/ttyUSB0\n\n");

	if (init_serial_port(ops->sport, ops->speed) == -1) {
			printf("Error setting up Serial Port, Exiting\n");
			return -1;
	}
	if(!ops->interactive) {
		print = printf;
	} else {
		print = printw;
	}
	
	return 0;
}

