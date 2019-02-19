# Makefile for vxWorks version of XMODEM/YMODEM
#

# modification history
# --------------------
# 01b,21apr94,mcm  Changed for vxWorks ver 5.1
# 01a,26feb93,mcm  Merged original Makefile with vxWorks stuff for this

TOOl = gnu
CPU = MC68000
CC=cc68k -mc68000 -msoft-float
LD=ld68k

CFLAGS		= -O -fstrength-reduce \
		-I$(VX_BSP_BASE)/h/ -ansi -pipe -nostdinc -DCPU=$(CPU) 

RM		= rm -f
CP		= cp
SHELL		= /bin/sh


.SUFFIXES : .lint .s .c .g

.s.o :
	@ $(RM) $@
	cat $< > $*.S
	$(CC) $(CFLAGS) -c $*.S
	@ $(RM) $*.S

.c.o :
	@ $(RM) $@
	$(CC) $(CFLAGS) -c $<

.c.g :
	@ $(RM) $@
	$(CC) $(CFLAGS)  -g -c $<




####################### object modules ################################


SRCS = xmodem.c getput.c misc.c send.c receive.c batch.c
OBJECTS = xmodem.o getput.o misc.o send.o receive.o batch.o
# CFLAGS = -O
# MAN = /usr/man/manl/xmodem.l

Xmodem.o: $(OBJECTS)
	$(LD) -o Xmodem.o -r $(OBJECTS) 

$(OBJECTS): xmodem.h

clean:
	rm -f *.o errs a.out 

lint:
	lint xmodem.c getput.c receive.c send.c batch.c misc.c 

shar:
	shar README Makefile xmodem.1 xmodem.h version.h > xmodem.shar.1
	shar update.doc xmodem.c receive.c > xmodem.shar.2
	shar getput.c README.vxWorks rtcTime.fixme.c > xmodem.shar.3
	shar misc.c batch.c send.c > xmodem.shar.4
	shar ymodem.doc > xmodem.shar.5

dist: 
	tar cvf vxXmodem.tar README update.doc Makefile xmodem.1 xmodem.h \
	version.h xmodem.c getput.c receive.c misc.c batch.c \
	send.c ymodem.doc README.vxWorks rtcTime.fixme.c
	compress vxXmodem.tar

xmodem.o: xmodem.c xmodem.h version.h
getput.o: getput.c xmodem.h
misc.o: misc.c xmodem.h
send.o: send.c xmodem.h
receive.o: receive.c xmodem.h
batch.o: batch.c xmodem.h

# end of MakeSkel

