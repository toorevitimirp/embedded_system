CC = /opt/host/armv4l/bin/armv4l-unknown-linux-gcc 
LD = /opt/host/armv4l/bin/armv4l-unknown-linux-ld
CFLAGS = -D__KERNEL__ -I/HHARM9-EDU/kernel/include/linux -I/HHARM9-EDU/kernel/include -Wall -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=s3c2410_testirq -I/opt/host/armv4l/src/linux/include -DMODULE

all:2410audio.o 2410audio_play.o 2410audio_rec.o 2410audio_play_rec.o

2410audio.o: audio.o 2410iic.o
	$(LD) -r $^ -o $@ 

2410audio_play.o: audio_play.o 2410iic.o
	$(LD) -r $^ -o $@ 

2410audio_rec.o: audio_rec.o 2410iic.o
	$(LD) -r $^ -o $@ 

2410audio_play_rec.o: audio_play_rec.o 2410iic.o
	$(LD) -r $^ -o $@ 

audio.o: 2410audio.c 
	$(CC) $(CFLAGS) -c $^ -o $@

audio_play.o: 2410audio_play.c 
	$(CC) $(CFLAGS) -c $^ -o $@

audio_rec.o: 2410audio_rec.c 
	$(CC) $(CFLAGS) -c $^ -o $@

audio_play_rec.o: 2410audio_play_rec.c 
	$(CC) $(CFLAGS) -c $^ -o $@

2410iic.o: 2410iic.c
	$(CC) $(CFLAGS) -c $^ 

.PHONY: clean
clean:
	-rm -f *.o

distclean:
	@make clean
	rm -f tags *~

