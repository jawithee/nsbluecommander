#ifndef __NSBLUEMENU__H_
#define __NSBLUEMENU__H_
#include <stdint.h>

typedef enum {
	HEXBYTE1 = 0,
	HEXBYTE2,
	INTBYTE1,
	INTBYTE2,
	BDADDR,
	STRING,
	STRLENSTRING,
	STATUS,
	UUID,
	SERVICELIST,
	SEARCHPATTERN,
	SEARCHRESULTS,
	ATTRIBUTES,
} argumentType;

typedef struct {
	char * printOptions;
	argumentType argsType;
	int argOptions;
} arguments;

typedef struct menus {
	char * menuName;
	struct menuitems * items;
	struct menus * prev;
	int size;
} menu;

typedef struct menuitems {
	char * name;
	menu * nextMenu;
/*	void (*func)(void); */
	uint8_t opcode;
} menuitem;

typedef struct{
	int numReqArgs;
	arguments * reqArgs;
	int numConfirmArgs;
	arguments * confirmArgs;
} packetArguments;


void print_menu(WINDOW *menu_win, int highlight);
int interactiveMode(void);

int cursesSendCommand(uint8_t type, uint8_t opcode);
/* void cursesPrintConfirm(void); */
int cursesPrintConfirm(void);
int initMenu(void);

#endif
