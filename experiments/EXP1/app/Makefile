
CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc
CFLAGS = -DDEBUG -D__Linux__ -g

all:hello

hello: hello.o
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o hello
