CROSS =/opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
LDFLAGS += --static 

EXEC = ttytest_char
OBJS = ttytest_char.o 

CFLAGS += -DPARANOID_TTYS -DUSE_TERMIO -DKLUDGELINEMODE -D_GNU_SOURCE

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS$(LDLIBS-$(@)))
	cp -f $(EXEC) /
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

