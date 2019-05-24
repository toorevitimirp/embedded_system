#ifndef _MXLSOUND_H_
#define _MXLSOUND_H_

/*IOCTL COMMANDS */
#define     DAC_OFF             0x10
#define     MONO                0x11
#define     STERO               0x12
#define     LR_MUTE             0x20
#define     LEFT_MUTE	        0x21
#define     RIGHT_MUTE	        0x22
#define	    LEFT_FULL	        0x23
#define	    LEFT_LOW	        0x24
#define     RIGHT_FULL	        0x25
#define     RIGHT_LOW	        0x26
#define 	LINEIN				0x27
#define 	MIC					0x28

/* HHTech add, uda1380 i2c commands */
unsigned int I2C_test(void);
unsigned int I2C_read(unsigned int reg, unsigned int *_data);
unsigned int I2C_write(unsigned int reg, unsigned int data);

#define EVMODE 		0x00
#define I2SIO  		0x01
#define POWCON  	0x02
#define ANAMIX  	0x03
#define HPAM 		0x04
#define MASVOLCON 	0x10
#define MIXVOLCON 	0x11
#define MODSEL 		0x12
#define MASMUT 		0x13
#define MIXSD 		0x14
#define DECVOLCON 	0x20
#define PGAMUT 		0x21
#define ADCCON 		0x22
#define AGCCON 		0x23
#define RESL3 		0x7F
#define DECSTA 		0x28
#define INTFILSTA 	0x18

#ifndef     IO_BASE
#define     IO_BASE	            0xf0000000
#endif

#define     SSI_STX             IO_BASE+ 0x218000
#define     SSI_SRX             IO_BASE+ 0x218004
#define     SSI_SCSR            IO_BASE+ 0x218008
#define     SSI_STCR            IO_BASE+ 0x21800C
#define     SSI_SRCR            IO_BASE+ 0x218010
#define     SSI_STCCR           IO_BASE+ 0x218014
#define     SSI_SRCCR           IO_BASE+ 0x218018
#define     SSI_STSR            IO_BASE+ 0x21801C
#define     SSI_SFCSR           IO_BASE+ 0x218020
#define     SSI_STR             IO_BASE+ 0x218024
#define     SSI_SOR             IO_BASE+ 0x218028
#define     PCLKDIV3    		IO_BASE+ 0x21b020
#define     UPCTL0				IO_BASE+ 0x21b00c
#define     UPCTL1	    		IO_BASE+ 0x21b010
#define     PCDR	    		IO_BASE+ 0x21b020
#define     CSCR	    		IO_BASE+ 0x21b000

#define     FMCR            	0xf021b808

#define     PT	                0xf021c200	//portc
#define     DDIR            	PT
#define     OCR1            	PT+ 0x4
#define     OCR2				PT+ 0x8
#define     DR	                PT+ 0x1c
#define     GIUS            	PT+ 0x20
#define     GPR	                PT+ 0x38
#define     PUEN            	PT+ 0x40

//DMA Channle allocation
#define _reg_DMA_SAR(channum)       (*((volatile unsigned long *)(IO_ADDRESS((0x209080 + channum * 0x40)))))

#define _reg_DMA_DAR(channum)       (*((volatile unsigned long *)(IO_ADDRESS((0x209084 + channum * 0x40)))))


#define _reg_DMA_CNTR(channum)      (*((volatile unsigned long *)(IO_ADDRESS((0x209088 + channum * 0x40)))))



#define _reg_DMA_CCR(channum)    (*((volatile unsigned long *)(IO_ADDRESS((0x20908C + channum * 0x40)))))

#define _reg_DMA_RSSR(channum)   (*((volatile unsigned long *)(IO_ADDRESS((0x209090 + channum * 0x40)))))

#define _reg_DMA_BLR(channum)    (*((volatile unsigned long *)(IO_ADDRESS((0x209094 + channum * 0x40)))))

#define _reg_DMA_RTOR(channum)   (*((volatile unsigned long *)(IO_ADDRESS((0x209098 + channum * 0x40)))))

#define _reg_DMA_BUCR(channum)   (*((volatile unsigned long *)(IO_ADDRESS((0x209098 + channum * 0x40)))))
//end DMA

/*Sample rate define */
#define     k8                  8000
#define     k11025              11025
#define     k16                 16000
#define     k2205               22050
#define     k32                 32000
#define     k441                44100
#define     k48                 48000

#define     SR_REG              0x01
#define     AVOL                0x02
#define     GCFG                0x03

#define	reg32p(r)	((volatile unsigned long *) (r))
#define	reg16p(r)	((volatile unsigned short *) (r))
#define	reg8p(r)	((volatile unsigned char *) (r))

#endif
