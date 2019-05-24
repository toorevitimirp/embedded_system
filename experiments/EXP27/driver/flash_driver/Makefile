CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc 
CFLAGS = -D__KERNEL__ -I/HHARM9-EDU/kernel/include -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=ppcflash


#MODCFLAGS = -O0 -Wall -DCONFIG_KERNELD -DMODULE -D__KERNEL__ -DLinux -nostdinc -I- 
MODCFLAGS = -DMODULE 
flash_drv.o: flash_drv.c
	$(CC) $(MODCFLAGS) $(CFLAGS) -c -o $@ $<
	cp -f $@ /


clean:
	rm -f *.o

