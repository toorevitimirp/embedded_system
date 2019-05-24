CROSS = /opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
//LDFLAGS += --static


EXEC = key_led
OBJS = key_led.o 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBM) $(LDLIBS) $(LIBGCC) -lm
#	cp key_led / -f

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
