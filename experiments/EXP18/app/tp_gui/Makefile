CROSS = /opt/host/armv4l/bin/armv4l-unknown-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip
#LDFLAGS += --static
LIBGCC += -lm

EXEC = tp_gui
OBJS = graphic.o main.o


all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBM) $(LDLIBS) $(LIBGCC) 
	
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
