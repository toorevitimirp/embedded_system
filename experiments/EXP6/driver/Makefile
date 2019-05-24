CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc 
CFLAGS = -D__KERNEL__ -I/HHARM9-EDU/kernel/include -Wall -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=s3c2410_testirq


MODCFLAGS = -DMODULE  


testirq.o: testirq.c
	$(CC) $(MODCFLAGS) $(CFLAGS) -c -o $@ testirq.c
#	cp testirq.o / -f

clean:
	rm -f *.o

