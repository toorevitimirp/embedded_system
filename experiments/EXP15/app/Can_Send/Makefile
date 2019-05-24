CROSS = /opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
//LDFLAGS += --static


EXEC = can_send
OBJS = send.o 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBM) $(LDLIBS) $(LIBGCC) -lm

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
