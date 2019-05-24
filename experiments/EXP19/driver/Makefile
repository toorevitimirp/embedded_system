CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc
CFLAGS = -D__KERNEL__ -I/HHARM9-EDU/kernel/include -Wall -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float  -DUTS_MACHINE='"arm"' -DKBUILD_BASENAME=digi -DMODULE -DMODVERSIONS
#CFLAGS = -D__KERNEL__ -I. -I$(INCLUDE) -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=digi -DMODULE -DMODVERSIONS

OUTPUT = .

$(OUTPUT)/digi.o: digi.c digi.h
	$(CC) $(CFLAGS) -o $@ -c $<
#	cp -f $@ /var/ftp
	cp -f $@ /

clean:
	rm -f $(OUTPUT)/digi.o

#arm-elf-linux-gcc -D__KERNEL__ -I/HHARMMXL-INTEG/linux-2.4.18-rmk4/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float   -DKBUILD_BASENAME=dbmx1fb   -c -o dbmx1fb.o dbmx1fb.c
#rm -f video.o

