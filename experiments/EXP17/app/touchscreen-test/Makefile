#test for MXL

INCLUDE = .
CFLAGS = -I. -I$(INCLUDE) -I/HHARMMXL-INTEG/linux-2.4.18-rmk4/include
CFLAGS += -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes

OUTPUT = .

$(OUTPUT)/asp_test: asp_test.c $(INCLUDE)/asp.h
	arm-elf-linux-gcc $(CFLAGS) -o $@ $<
clean:
	rm -f $(OUTPUT)/asp_test
