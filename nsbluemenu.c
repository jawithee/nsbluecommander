#include <ncurses.h>
#include <string.h>
#include "nsbluemenu.h"
#include "nsbluecommands.h"
#include "nsblueoptions.h"

extern Options ops;

packetArguments packetArgs[NUMOPCODE];

/* Global Menu Options */
arguments argsBlueAddr[] = {{"BLuetooth Address:", BDADDR, 0}};
arguments argsStRmBt[] = {{"Status:", STATUS, 0}, {"Remote BDA:", BDADDR, 0}, {"BT Name:", STRLENSTRING, 0}};
arguments argsSt[] = {{"Status:", STATUS, 0}};
arguments argsBrGPID []= {{"Browser Group ID:", UUID, 0}};
arguments argsStSv[] = {{"Status:", STATUS, 0}, {"Service List:", SERVICELIST, 0}};
arguments argsSchAtt[] = {{"Search Patterns:", SEARCHPATTERN, 86}, {"Attributes:", ATTRIBUTES, 86}};
arguments argsSchres[] = {{"Search Results:", SEARCHRESULTS, 330}};
arguments argsLpBaRp[] = {{"Local Port:", INTBYTE1, 30}, {"Bluetooth Address:", BDADDR, 0}, {"Remote Port Number:", INTBYTE1, 30}};
arguments argsStLp[] = {{"Status:", STATUS, 0}, {"Local Port:", INTBYTE1, 0}};
arguments argsLp[] = {{"Local Port:", INTBYTE1, 30}};
arguments argsBaLt[] = {{"BLuetooth Address:", BDADDR, 0}, {"Link Timeout:", HEXBYTE2, 0}};
arguments argsStBa[] = {{"Status:", STATUS, 0}, {"BLuetooth Address:", BDADDR, 0}};
arguments argsBa[] = {{"BLuetooth Address:", BDADDR, 0}};
arguments argsStBaLt[] = {{"Status:", STATUS, 0}, {"BLuetooth Address:", BDADDR, 0}, {"Link Timeout:", HEXBYTE2, 0}};
arguments argsLpBrPsFcXnXf[] = {{"Local Port:", INTBYTE1, 30}, {"Baud Rate:", HEXBYTE1, 0}, {"Port Settings:", HEXBYTE1, 0}, {"Flow Control:", HEXBYTE1, 0}, {"Xon Char:", HEXBYTE1, 0}, {"Xoff Char:", HEXBYTE1, 0}};
arguments argsStLpBrPsFcXnXf[] = {{"Status:", STATUS, 0}, {"Local Port:", INTBYTE1, 30}, {"Baud Rate:", HEXBYTE1, 0}, {"Port Settings:", HEXBYTE1, 0}, {"Flow Control:", HEXBYTE1, 0}, {"Xon Char:", HEXBYTE1, 0}, {"Xoff Char:", HEXBYTE1, 0}};
arguments argsStLpPsBr[] = {{"Status:", STATUS, 0}, {"Local Port:", INTBYTE1, 30}, {"Port Status:", HEXBYTE1, 0}, {"Break Length:", HEXBYTE2, 0}};
arguments argsLpSa[] = {{"Local Port:", INTBYTE1, 30}, {"State (1/0):", INTBYTE1, 0}};
arguments argsLpBl[] = {{"Local Port:", INTBYTE1, 30}, {"Break Length (ms):", INTBYTE2, 0}};
arguments argsStNa[] = {{"Status:", STATUS, 0}, {"Name:", STRLENSTRING, 0}};
arguments argsNa[] = {{"Name:", STRLENSTRING, 40}};
arguments argsStMo[] = {{"Status:", STATUS, 0},{"Mode:", HEXBYTE1, 0}};
arguments argsMo[] = {{"Mode:", HEXBYTE2, 0}};
arguments argsLt[] = {{"Link Timeout:", HEXBYTE2, 0}};
arguments argsStRS[] = {{"Status:", STATUS, 0}, {"RSSI:", HEXBYTE1, 0}};
arguments argsLy[] = {{"Link Policy:", HEXBYTE2, 0}};
arguments argsStLy[] = {{"Status:", STATUS, 0},{"Link Policy:", HEXBYTE2, 0}};
arguments argsBaLy[] = {{"Bluetooth Address:", BDADDR, 0},{"Link Policy:", HEXBYTE2, 0}};
arguments argsBaSaiSMiSatSto[] = {{"Bluetooth Address:", BDADDR, 0}, {"Sniff Max Interval:", INTBYTE2, 0}, {"Sniff Min Interval:", INTBYTE2, 0}, {"Sniff Attempt:", INTBYTE2, 0}, {"Sniff Timeout:", INTBYTE2, 0}};
	

menuitem gapMenuItems[] = {{"GAP_READ_LOCAL_NAME", NULL, GAP_READ_LOCAL_NAME},
							{"GAP_WRITE_LOCAL_NAME", NULL, GAP_WRITE_LOCAL_NAME},
							{"GAP_GET_SECURITY_MODE", NULL, GAP_GET_SECURITY_MODE},
							{"GAP_SET_SECURITY_MODE", NULL, GAP_SET_SECURITY_MODE},
							{"GAP_GET_LINK_POLICY", NULL, GAP_GET_SECURITY_MODE},
							{"GAP_SET_LINK_POLICY", NULL, GAP_SET_LINK_POLICY},
							{"GAP_REMOTE_DEVICE_NAME", NULL, GAP_REMOTE_DEVICE_NAME},
							{"GAP_READ_LOCAL_BDA", NULL, GAP_READ_LOCAL_BDA},
							{"GAP_ENTER_SNIFF_MODE", NULL, GAP_GET_SECURITY_MODE},
							{"GAP_EXIT_SNIFF_MODE", NULL, GAP_GET_SECURITY_MODE}
};
menu gapMenu = {"GAP MENU", gapMenuItems, NULL, sizeof(gapMenuItems)/sizeof(menuitem)};

menuitem sppMenuItems[] = {{"SPP_ESTABLISH_LINK", NULL, SPP_ESTABLISH_LINK},
							{"SPP_RELEASE_LINK", NULL, SPP_RELEASE_LINK},
							{"SPP_TRANSPARENT_MODE", NULL, SPP_TRANSPARENT_MODE},
							{"SPP_GET_LINK_TIMEOUT", NULL, SPP_GET_LINK_TIMEOUT},
							{"SPP_SET_LINK_TIMEOUT", NULL, SPP_SET_LINK_TIMEOUT},
							{"SPP_GET_PORT_CONFIG", NULL, SPP_GET_PORT_CONFIG},
							{"SPP_SET_PORT_CONFIG", NULL, SPP_SET_PORT_CONFIG},
							{"SPP_GET_PORT_STATUS", NULL, SPP_GET_PORT_STATUS},
							{"SPP_PORT_SET_DTR", NULL, SPP_PORT_SET_DTR},
							{"SPP_PORT_SET_RTS", NULL, SPP_PORT_SET_RTS},
							{"SPP_PORT_BREAK", NULL, SPP_PORT_BREAK}
};
menu sppMenu = {"SPP MENU", sppMenuItems, NULL, sizeof(sppMenuItems)/sizeof(menuitem)};

menuitem miscMenuItems[] = {{"SET_DEFAULT_LINK_TIMEOUT", NULL, SET_DEFAULT_LINK_TIMEOUT},
							{"GET_DEFAULT_LINK_TIMEOUT", NULL, GET_DEFAULT_LINK_TIMEOUT},
							{"READ_RSSI", NULL, READ_RSSI},
							{"SET_DEFAULT_LINK_POLICY", NULL, SET_DEFAULT_LINK_POLICY},
							{"GET_DEFAULT_LINK_POLICY", NULL, GET_DEFAULT_LINK_POLICY}
};
menu miscMenu = {"MISC MENU", miscMenuItems, NULL, sizeof(miscMenuItems)/sizeof(menuitem)};

menuitem sdapMenuItems[] = {{"SET_DEFAULT_LINK_TIMEOUT", NULL, NOOPCODE},
							{"GET_DEFAULT_LINK_TIMEOUT", NULL, NOOPCODE}
};
menu sdapMenu = {"SDAP MENU", sdapMenuItems, NULL, sizeof(sdapMenuItems)/sizeof(menuitem)};

menuitem mainMenuItems[] = {{"GAP", &gapMenu, NOOPCODE},
							{"SPP", &sppMenu, NOOPCODE},
							{"SDAP", &sppMenu, NOOPCODE},
							{"MISC", &miscMenu, NOOPCODE}
};
menu mainMenu = {"MAIN MENU", mainMenuItems, NULL, sizeof(mainMenuItems)/sizeof(menuitem)}; 

menu * curMenu = &mainMenu;

/* End Global Menu Options */
int interactiveMode(void) {	
	int highlight = 0;
	int c;
	int startx = 0;
	int starty = 0;
	
	initMenu();

	initscr();
	start_color();
	init_pair(1, COLOR_RED, COLOR_WHITE);

	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
	getmaxyx(stdscr, starty, startx);

	/*menu_win = newwin(HEIGHT, WIDTH, starty, startx);*/
	keypad(stdscr, TRUE);
	curs_set(0);
	refresh();
	print_menu(stdscr, highlight);
	while (1) {
		c = wgetch(stdscr);
		switch(c)
		{	case KEY_UP:
				if(highlight == 0)
					highlight = curMenu->size-1;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == curMenu->size-1)
					highlight = 0;
				else 
					++highlight;
				break;
			case KEY_BACKSPACE:
				if(curMenu->prev != NULL) {
					curMenu=curMenu->prev;
					highlight = 0;
				}
				break;
			case KEY_RIGHT:
			case KEY_ENTER:
			case 10:
				fprintf(stderr, "Entered Key\n");
				if(curMenu->items[highlight].nextMenu != NULL) {
					curMenu->items[highlight].nextMenu->prev = curMenu;
					curMenu = curMenu->items[highlight].nextMenu;
				} else if(curMenu->items[highlight].opcode != NOOPCODE) {
					fprintf(stderr, "In the OOPCODE LOOP\n");
					cursesSendCommand(PACKET_TYPE_REQUEST, curMenu->items[highlight].opcode);
					fprintf(stderr, "PrintResponse\n");
					cursesPrintConfirm();
					fprintf(stderr, "Received Response\n");
				}
				
				highlight = 0;
				break;
			default:
				mvprintw(starty-1, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		if(c >= 'a' && c <= 'z' && (c-'a' < curMenu->size)){ /* Allow Quick Keys */
			highlight = c - 'a';
			ungetch(KEY_ENTER); 
		}
		if(c >= 'A' && c <= 'Z' && (c-'A' < curMenu->size)) { /* Allow Quick Keys */
			highlight = c - 'A';					
			ungetch(KEY_ENTER);
		}
		if(c == 27) break; /* Exit Program ESC-Key*/
		clear();
		print_menu(stdscr, highlight);
	}
	clrtoeol();
	refresh();
	endwin();
	return 0;
}

void print_menu(WINDOW *menu_win, int highlight) {
	int x, y, i;	
	x = 1;
	y = 0;
	int winx, winy;
	getmaxyx(menu_win, winy, winx);
	
	wattron(menu_win, A_BOLD|COLOR_PAIR(1)); 
	mvwprintw(menu_win, y, x, "%s", curMenu->menuName);
	wattroff(menu_win, A_BOLD|COLOR_PAIR(1));
	y++;
	/*box(menu_win, 0, 0);*/
	for(i = 0; i < curMenu->size; ++i) {
		if(highlight == i) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%c) %s", 'a'+i , curMenu->items[i].name);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%c) %s", 'a'+i, curMenu->items[i].name);
		++y;
	}
	mvwprintw(menu_win, winy-2, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	mvwprintw(menu_win, winy-1, 0, "Or press the associated character. Press ESC to exit");
	wrefresh(menu_win);
}

int cursesSendCommand(uint8_t type, uint8_t opcode) {
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	arguments * reqArgs = packetArgs[opcode].reqArgs;
	int NumArgs = packetArgs[opcode].numReqArgs;
	int i=0;
	int temp, baddr[6];
	int dataLen = 0;
	clear();
	echo();

	while(i < NumArgs) {
		switch(reqArgs[i].argsType) {
			case STRLENSTRING:
				printw("%s", reqArgs[i].printOptions);
				scanw("%[^\n]", (char *) &data[dataLen+1]);
				if(reqArgs[i].argOptions > 0) 
					data[i+1+reqArgs[i].argOptions] = '\0'; /* Limit String Length*/
				data[dataLen] = (strlen((const char *) &data[dataLen+1]) + 1) & 0xFF; /* Get the String Length and Store it into Previous Byte */
				dataLen += data[dataLen] + 1; /* Length is increased to size of string plus 1 size byte */
				break;
			case HEXBYTE1:
				printw("%s", reqArgs[i].printOptions);
				scanw("0x%X", &temp); /* Get Value */
				data[dataLen] = 0xFF & (temp); /* Get lowest Byte */
				dataLen += 1; /* Added one more byte to data */
			case HEXBYTE2:
				printw("%s", reqArgs[i].printOptions);
				scanw("0x%X", &temp); /* Get Value */
				data[dataLen] = 0xFF & (temp); /* Get lowest Byte */
				data[dataLen+1] = 0xFF & (temp >> 8); /* Get Upper Byte */
				dataLen += 2; /* Added two more byte to data */	
			case INTBYTE1:
				printw("%s", reqArgs[i].printOptions);
				scanw("%d", &temp); /* Get Value */
				data[dataLen] = 0xFF & (temp); /* Get lowest Byte */
				dataLen += 1; /* Added one more byte to data */
			case INTBYTE2:
				printw("%s", reqArgs[i].printOptions);
				scanw("%d", &temp); /* Get Value */
				data[dataLen] = 0xFF & (temp); /* Get lowest Byte */
				data[dataLen+1] = 0xFF & (temp >> 8); /* Get Upper Byte */
				dataLen += 2; /* Added two more byte to data */
			case BDADDR:
				printw("%s", reqArgs[i].printOptions);
				scanw("%X:%X:%X:%X:%X:%X", baddr[0], baddr[1], baddr[2], baddr[3], baddr[4], baddr[5]); /* Get Value */
				data[dataLen+5] = 0xFF & baddr[0];
				data[dataLen+4] = 0xFF & baddr[1]; 
				data[dataLen+3] = 0xFF & baddr[2];
				data[dataLen+2] = 0xFF & baddr[3];
				data[dataLen+1] = 0xFF & baddr[4];
				data[dataLen] = 0xFF & baddr[5];				
				dataLen += 6; /* Added two more byte to data */
			case UUID:
				printw("%s", reqArgs[i].printOptions);
				scanw("0x%X", &temp); /* Get Value */
				data[dataLen] = 0xFF & (temp); /* Get lowest Byte */
				data[dataLen+1] = 0xFF & (temp >> 8); /* Get Upper Byte */
				dataLen += 2; /* Added two more byte to data */
			default:
				return -1; /* Unknown Argument */
		}
		i++;
	}
	fprintf(stderr, "Type: 0x%2X\nOpcode: 0x%2X\n DataLen: %d\n", (int) type, (int) opcode, dataLen);
	sendPacket(type, opcode, data, dataLen);
	
	return 0;
}
#if 0
void cursesPrintConfirm(void) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	int dataLen;
	
	int i;

	if(getPacket(&type, &opcode, data, &dataLen) < 0)
		printw("Error Reading\n");
	printw("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
	printw("Data: ");
	for(i = 0; i < dataLen; i++)
		printw("0x%X ", (int) data[i]);
		
	refresh();
	while(wgetch(stdscr) != 'q');
	clear();
}
#endif

int cursesPrintConfirm(void) {
	uint8_t type, opcode;
	uint8_t data[BLUEMAXDATALEN]; /* Maximum Allowed Data */
	arguments * resArgs;
	int NumArgs;
	int winx, winy;
	
	int i=0, temp;
	int dataLen, dataIndex = 0;
	
	getmaxyx(stdscr, winy, winx);
	clear();
	echo();
	if(getPacket(&type, &opcode, data, &dataLen) < 0) {
		printw("Error Reading\n");
		return -1;
	}
	resArgs = packetArgs[opcode].confirmArgs;
	NumArgs = packetArgs[opcode].numConfirmArgs;
	
	/* If Verbose */
	if(ops.verbose) printw("Type: 0x%x\nOp: 0x%x\ndataLen: %d\n", (int) type, (int) opcode, (int) dataLen);
	
	while(i < NumArgs) {
		switch(resArgs[i].argsType) {
			case STATUS:
				printNSBTConfirmError(data[dataIndex]);
			//	printw("%s 0x%02X\n", resArgs[i].printOptions, (int) data[dataIndex]);
				dataIndex += 1; /* One Byte Read */
				break;
			case STRLENSTRING:
				printw("Data Len: %d\n", (int) data[dataIndex]);
				printw("%s %s\n", resArgs[i].printOptions,  (char *) &data[dataIndex+1]);
				dataIndex += data[dataIndex] + 1; /* String Length + Byte for String Length */
				break;
			case HEXBYTE1:
				printw("%s 0x%02X\n", resArgs[i].printOptions, (int) data[dataIndex]);
				dataIndex += 1; /* Added one more byte to data */
				break;
			case HEXBYTE2:
				temp = ((data[dataIndex+1] << 8) & 0xFF)| data[dataIndex];
				printw("%s 0x%04X\n", resArgs[i].printOptions, temp);
				dataIndex += 2; /* Added one more byte to data */
				break;
			case INTBYTE1:
				printw("%s %d\n", resArgs[i].printOptions, (int) data[dataIndex]);
				dataIndex += 1; /* Added one more byte to data */
				break;
			case INTBYTE2:
				temp = ((data[dataIndex+1] << 8) & 0xFF)| data[dataIndex];
				printw("%s %d\n", resArgs[i].printOptions, temp);
				dataIndex += 2; /* Added one more byte to data */
				break;	
			case BDADDR:
				printw("%s %02X:%02X:%02X:%02X:%02X:%02X\n", resArgs[i].printOptions, (int) data[dataIndex+5], (int) data[dataIndex+4], (int) data[dataIndex+3], (int) data[dataIndex+2], (int) data[dataIndex+1], (int) data[dataIndex]);
				dataIndex += 6; /* Added one more byte to data */
				break;					
			case UUID:
				temp = ((data[dataIndex+1] << 8) & 0xFF)| data[dataIndex];
				printw("%s 0x%04X\n", resArgs[i].printOptions, temp);
				dataIndex += 2; /* Added one more byte to data */
				break;
			default:
				return -1; /* Unknown Argument */		
		}
		i++;
	}
	mvprintw(winy-1, 0, "Press \'q\' to go back");
	refresh();
	while(wgetch(stdscr) != 'q');
	clear();

	return 0;
}

int initMenu(void) {
	int i;
	for( i = 0; i < NUMOPCODE; i++) {
		packetArgs[i].reqArgs= NULL;
		packetArgs[i].numReqArgs = 0;
		packetArgs[i].confirmArgs = NULL;
		packetArgs[i].numConfirmArgs = 0;
	}
//return 0;
//}
#if 1

	packetArgs[GAP_REMOTE_DEVICE_NAME].reqArgs= argsBlueAddr;
	packetArgs[GAP_REMOTE_DEVICE_NAME].numReqArgs = 1;
	packetArgs[GAP_REMOTE_DEVICE_NAME].confirmArgs = argsStRmBt;
	packetArgs[GAP_REMOTE_DEVICE_NAME].numConfirmArgs = 3;

	/* SDAP */	
	packetArgs[SDAP_CONNECT].reqArgs= argsBlueAddr;
	packetArgs[SDAP_CONNECT].numReqArgs = 1;
	packetArgs[SDAP_CONNECT].confirmArgs = argsSt;
	packetArgs[SDAP_CONNECT].numConfirmArgs = 1;
	
	packetArgs[SDAP_DISCONNECT].confirmArgs = argsSt;
	packetArgs[SDAP_DISCONNECT].numConfirmArgs = 1;
	
	packetArgs[SDAP_SERVICE_BROWSE].reqArgs= argsBrGPID;
	packetArgs[SDAP_SERVICE_BROWSE].numReqArgs = 1;
	packetArgs[SDAP_SERVICE_BROWSE].confirmArgs = argsStSv;
	packetArgs[SDAP_SERVICE_BROWSE].numConfirmArgs = 2;
	
	packetArgs[SDAP_SERVICE_SEARCH].reqArgs = argsSchAtt; 
	packetArgs[SDAP_SERVICE_SEARCH].numReqArgs = 2;
	packetArgs[SDAP_SERVICE_SEARCH].confirmArgs = argsSchres;
	packetArgs[SDAP_SERVICE_SEARCH].numConfirmArgs = 1;
	
	/* MORE SDAP ...*/

	/* SPP */
	packetArgs[SPP_ESTABLISH_LINK].reqArgs = argsLpBaRp;
	packetArgs[SPP_ESTABLISH_LINK].numReqArgs = 3;
	packetArgs[SPP_ESTABLISH_LINK].confirmArgs = argsStLp;
	packetArgs[SPP_ESTABLISH_LINK].numConfirmArgs = 2;
	
	packetArgs[SPP_RELEASE_LINK].reqArgs = argsLp;
	packetArgs[SPP_RELEASE_LINK].numReqArgs = 1;
	packetArgs[SPP_RELEASE_LINK].confirmArgs = argsStLp;
	packetArgs[SPP_RELEASE_LINK].numConfirmArgs = 2;
	
	packetArgs[SPP_TRANSPARENT_MODE].reqArgs = argsLp;
	packetArgs[SPP_TRANSPARENT_MODE].numReqArgs = 1;
	packetArgs[SPP_TRANSPARENT_MODE].confirmArgs = argsStLp;
	packetArgs[SPP_TRANSPARENT_MODE].numConfirmArgs = 2;
	
	packetArgs[SPP_SET_LINK_TIMEOUT].reqArgs = argsBaLt;
	packetArgs[SPP_SET_LINK_TIMEOUT].numReqArgs = 2;
	packetArgs[SPP_SET_LINK_TIMEOUT].confirmArgs = argsStBa;
	packetArgs[SPP_SET_LINK_TIMEOUT].numConfirmArgs = 2;
	
	packetArgs[SPP_GET_LINK_TIMEOUT].reqArgs = argsBa;
	packetArgs[SPP_GET_LINK_TIMEOUT].numReqArgs = 2;
	packetArgs[SPP_GET_LINK_TIMEOUT].confirmArgs = argsStBaLt;
	packetArgs[SPP_GET_LINK_TIMEOUT].numConfirmArgs = 3;
	
	/*****************/
	
	packetArgs[SPP_SET_PORT_CONFIG].reqArgs = argsLpBrPsFcXnXf;
	packetArgs[SPP_SET_PORT_CONFIG].numReqArgs = 6;
	packetArgs[SPP_SET_PORT_CONFIG].confirmArgs = argsStLp;
	packetArgs[SPP_SET_PORT_CONFIG].numConfirmArgs = 2;
	
	packetArgs[SPP_GET_PORT_CONFIG].reqArgs = argsLp;
	packetArgs[SPP_GET_PORT_CONFIG].numReqArgs = 1;
	packetArgs[SPP_GET_PORT_CONFIG].confirmArgs = argsStLpBrPsFcXnXf;
	packetArgs[SPP_GET_PORT_CONFIG].numConfirmArgs = 7;
	
	packetArgs[SPP_GET_PORT_STATUS].reqArgs= argsLp;
	packetArgs[SPP_GET_PORT_STATUS].numReqArgs = 1;
	packetArgs[SPP_GET_PORT_STATUS].confirmArgs = argsStLpPsBr;
	packetArgs[SPP_GET_PORT_STATUS].numConfirmArgs = 4;
	
	packetArgs[SPP_PORT_SET_DTR].reqArgs = argsLpSa;
	packetArgs[SPP_PORT_SET_DTR].numReqArgs = 2;
	packetArgs[SPP_PORT_SET_DTR].confirmArgs = argsStLp;
	packetArgs[SPP_PORT_SET_DTR].numConfirmArgs = 2;
	
	packetArgs[SPP_PORT_SET_RTS].reqArgs = argsLpSa;
	packetArgs[SPP_PORT_SET_RTS].numReqArgs = 2;
	packetArgs[SPP_PORT_SET_RTS].confirmArgs = argsStLp;
	packetArgs[SPP_PORT_SET_RTS].numConfirmArgs = 2;
	
	packetArgs[SPP_PORT_BREAK].reqArgs= argsLpBl;
	packetArgs[SPP_PORT_BREAK].numReqArgs = 2;
	packetArgs[SPP_PORT_BREAK].confirmArgs = argsStLp;
	packetArgs[SPP_PORT_BREAK].numConfirmArgs = 2;
	
	packetArgs[GAP_READ_LOCAL_NAME].reqArgs= NULL;
	packetArgs[GAP_READ_LOCAL_NAME].numReqArgs = 0;
	packetArgs[GAP_READ_LOCAL_NAME].confirmArgs = argsStNa;
	packetArgs[GAP_READ_LOCAL_NAME].numConfirmArgs = 2;
	
	packetArgs[GAP_WRITE_LOCAL_NAME].reqArgs= argsNa;
	packetArgs[GAP_WRITE_LOCAL_NAME].numReqArgs = 1;
	packetArgs[GAP_WRITE_LOCAL_NAME].confirmArgs = argsSt;
	packetArgs[GAP_WRITE_LOCAL_NAME].numConfirmArgs = 1;
	
	packetArgs[GAP_READ_LOCAL_BDA].reqArgs= NULL;
	packetArgs[GAP_READ_LOCAL_BDA].numReqArgs = 0;
	packetArgs[GAP_READ_LOCAL_BDA].confirmArgs = argsStBa;
	packetArgs[GAP_READ_LOCAL_BDA].numConfirmArgs = 2;
	
	packetArgs[GAP_GET_SECURITY_MODE].reqArgs= NULL;
	packetArgs[GAP_GET_SECURITY_MODE].numReqArgs = 0;
	packetArgs[GAP_GET_SECURITY_MODE].confirmArgs = argsStMo;
	packetArgs[GAP_GET_SECURITY_MODE].numConfirmArgs = 2;
	
	packetArgs[GAP_SET_SECURITY_MODE].reqArgs = argsMo;
	packetArgs[GAP_SET_SECURITY_MODE].numReqArgs = 1;
	packetArgs[GAP_SET_SECURITY_MODE].confirmArgs = argsSt;
	packetArgs[GAP_SET_SECURITY_MODE].numConfirmArgs = 1;
	
	/* Misc */	
	
	packetArgs[SET_DEFAULT_LINK_TIMEOUT].reqArgs = argsLt;
	packetArgs[SET_DEFAULT_LINK_TIMEOUT].numReqArgs = 1;
	packetArgs[SET_DEFAULT_LINK_TIMEOUT].confirmArgs = argsSt;
	packetArgs[SET_DEFAULT_LINK_TIMEOUT].numConfirmArgs = 1;
	
	packetArgs[GET_DEFAULT_LINK_TIMEOUT].reqArgs= NULL;
	packetArgs[GET_DEFAULT_LINK_TIMEOUT].numReqArgs = 0;
	packetArgs[GET_DEFAULT_LINK_TIMEOUT].confirmArgs = argsLt;
	packetArgs[GET_DEFAULT_LINK_TIMEOUT].numConfirmArgs = 1;
	
	packetArgs[READ_RSSI].reqArgs = argsBa;
	packetArgs[READ_RSSI].numReqArgs = 1;
	packetArgs[READ_RSSI].confirmArgs = argsStRS;
	packetArgs[READ_RSSI].numConfirmArgs = 2;
	
	packetArgs[SET_DEFAULT_LINK_POLICY].reqArgs = argsLy;
	packetArgs[SET_DEFAULT_LINK_POLICY].numReqArgs = 1;
	packetArgs[SET_DEFAULT_LINK_POLICY].confirmArgs = argsSt;
	packetArgs[SET_DEFAULT_LINK_POLICY].numConfirmArgs = 1;
	
	packetArgs[GET_DEFAULT_LINK_POLICY].reqArgs= NULL;
	packetArgs[GET_DEFAULT_LINK_POLICY].numReqArgs = 0;
	packetArgs[GET_DEFAULT_LINK_POLICY].confirmArgs = argsLy;
	packetArgs[GET_DEFAULT_LINK_POLICY].numConfirmArgs = 1;
	
	/* GAP */
	
	packetArgs[GAP_GET_LINK_POLICY].reqArgs= argsBa;
	packetArgs[GAP_GET_LINK_POLICY].numReqArgs = 1;
	packetArgs[GAP_GET_LINK_POLICY].confirmArgs = argsStLy;
	packetArgs[GAP_GET_LINK_POLICY].numConfirmArgs = 2;
	
	packetArgs[GAP_SET_LINK_POLICY].reqArgs = argsBaLy;
	packetArgs[GAP_SET_LINK_POLICY].numReqArgs = 2;
	packetArgs[GAP_SET_LINK_POLICY].confirmArgs = argsSt;
	packetArgs[GAP_SET_LINK_POLICY].numConfirmArgs = 1;
		
	packetArgs[GAP_ENTER_SNIFF_MODE].reqArgs = argsBaSaiSMiSatSto; 
	packetArgs[GAP_ENTER_SNIFF_MODE].numReqArgs = 5;
	packetArgs[GAP_ENTER_SNIFF_MODE].confirmArgs = argsSt;
	packetArgs[GAP_ENTER_SNIFF_MODE].numConfirmArgs = 1;
	
	packetArgs[GAP_EXIT_SNIFF_MODE].reqArgs= argsBa;
	packetArgs[GAP_EXIT_SNIFF_MODE].numReqArgs = 1;
	packetArgs[GAP_EXIT_SNIFF_MODE].confirmArgs = argsSt;
	packetArgs[GAP_EXIT_SNIFF_MODE].numConfirmArgs = 1;	
	
	return 0;
}
#endif
