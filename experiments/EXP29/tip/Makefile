#CC=/LinuxPPC/CDK/bin/powerpc-linux-gcc
CC=/opt/host/armv4l/bin/armv4l-unknown-linux-gcc

EXEC = tip
OBJS = tip.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) --static

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

