CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc 
LD = /opt/host/armv4l/bin/armv4l-unknown-linux-ld
CFLAGS = -D__KERNEL__ -I/HHARM9-EDU/kernel/include/linux -I/HHARM9-EDU/kernel/include -Wall -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=s3c2410_testirq -I/opt/host/armv4l/src/linux/include -DMODULE



led.o: led.c 
	$(CC) $(CFLAGS) -c $^ -o $@
#	cp led.o / -f
		
all: led.o 

.PHONY: clean
clean:
	-rm -f *.o

distclean:
	@make clean
	rm -f tags *~

