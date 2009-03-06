#ifndef __NSBLUEOPTIONS__H_
#define __NSBLUEOPTIONS__H_
#include <termios.h>
#include <unistd.h>
#include <popt.h>

struct baudmap_t {
	int intbaud;
	speed_t baud;
};

typedef struct {
	speed_t speed;
	int baud;
	char * sport;
	char * sname;
	int gname; 
	int gsecmode;
	int secmode;
	int interactive;
	int printErrors;
	int verbose;
} Options;

void usage(poptContext optCon, int exitcode, char *error);
int parseOptions(int argc, char ** argv, Options * ops);

#endif
