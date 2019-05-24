CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc 
CFLAGS = -march=armv4 -O2 
CFLAGS +=-Wall
#CFLAGS +=-Wcast-align
LDFLAGS = 

all: recorder recorder_play

recorder:  recorder.o
	$(CC) -o $@ $^ 
#	cp -pf recorder /

recorder_play:recorder_play.o
	$(CC) -o $@ $^

clean:
	rm -f recorder recorder_play *.o

distclean:
	make clean
	rm -f tags *~

