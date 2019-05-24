CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc

all: setmac readmac

setmac:setmac.o
	$(CC) -o $@ $^

readmac:readmac.o
	$(CC) -o $@ $^	

clean:
	-rm -f  *.o *~  setmac readmac

