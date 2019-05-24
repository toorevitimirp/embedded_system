#	$Id: Makefile.linux,v 1.9 1999/08/13 01:54:32 paulus Exp $
#CROSS = /home/hhcn/toolchain/gcc-3.2.2/toolchain/bin/powerpc-uclibc-
CROSS = /opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
LDFLAGS += --static

CDEF1=	-DTERMIOS			# Use the termios structure
CDEF2=	-DSIGTYPE=void			# Standard definition
CDEF3=	-UNO_SLEEP			# Use the usleep function
CDEF4=	-DFNDELAY=O_NDELAY		# Old name value
CDEFS=	$(CDEF1) $(CDEF2) $(CDEF3) $(CDEF4)

CFLAGS=	-O2 -g -pipe $(CDEFS)

INSTALL= install

all:	chat

chat:	chat.o
	$(CC) $(LDFLAGS) -o chat chat.o

chat.o:	chat.c
	$(CC) -c $(CFLAGS) -o chat.o chat.c

install: chat
	mkdir -p $(BINDIR)
	$(INSTALL) -s -c chat $(BINDIR)
	$(INSTALL) -c -m 644 chat.8 $(MANDIR)/man8

clean:
	rm -f chat.o chat *~
