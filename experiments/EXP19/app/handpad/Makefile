CROSS = /opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
#LDFLAGS += --static

EXEC = handpad 
OBJS = handpad.o graphic.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBM) $(LDLIBS) $(LIBGCC)
#	cp -f handpad ../../module

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
