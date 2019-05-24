CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc
EXEC = config.cgi
OBJS = config.o myhdr.o cgihead.o
FLTFLAGS= -s 64000
CFLAGS += -DHHTECH_CGI_CHINESE
all: $(EXEC) macset play setmac
$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) -lcrypt

macset:macset.o
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)

play:playmtv.o
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)

setmac:eeprom.o
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)
	

clean:
	-rm -f *.cgi *.elf *.gdb *.o *~ setmac ree timer update macset play setmac

