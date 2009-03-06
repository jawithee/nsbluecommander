CFLAGS  =   -g -Wall
CC      =   gcc
LIBS    =   -lncurses -lpopt
INCLUDES = 
OBJS = nsbluecommands.o  nsbluemenu.o  nsblueoptions.o nsbluecommander.o
SRCS = nsbluecommands.c  nsbluemenu.c  nsblueoptions.c nsbluecommander.c

all: nsbluecommander

# The variable $@ has the value of the target. In this case $@ = psort
nsbluecommander: nsbluecommander.o ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

depend: 
	makedepend ${SRCS}

clean:
	rm *.o core *~

# DO NOT DELETE

