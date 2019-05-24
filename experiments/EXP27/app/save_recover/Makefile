#CROSS = /home/hhcn/toolchain/gcc-3.2.2/toolchain/bin/powerpc-uclibc-
CROSS =/opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
#LDFLAGS += --static
EXEC_PUT = recover
OBJS_PUT = recover.o iomap.o
EXEC_GET = save
OBJS_GET = save.o iomap.o
all: $(EXEC_PUT) $(EXEC_GET) 

$(EXEC_PUT): $(OBJS_PUT)
	$(CC) $(LDFLAGS) -Wall -o $@ $(OBJS_PUT) $(LDLIBS)
	cp -f $(EXEC_PUT) /

$(EXEC_GET): $(OBJS_GET)
	$(CC) $(LDFLAGS) -Wall -o $@ $(OBJS_GET) $(LDLIBS)
	cp -f $(EXEC_GET) /
clean:
	-rm -f $(EXEC_PUT) $(EXEC_GET) *.elf *.gdb *.o *~
