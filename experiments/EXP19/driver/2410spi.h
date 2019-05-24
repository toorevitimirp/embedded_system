#define 2410SPI_BAUD    0x00
#define TRANSFED_READ_FALG   0x01

/*bits*/
#define DCON1_DMD_HS  0x80000000  /*Handshake mode is selected*/
#define DCON1_SYNC    0x40000000  /*DREQ and DACK are synchronized to PCLK*/    #define DCON1_INT     0x20000000  /*Interrupt request is generated*/
#define DCON1_TSZ     0x00000000  /*a unit transfer is performed*/
#define DCON1_SERVMODE 0x00000000 /*Single service mode*/
#define DCON1_HWSRCSEL 0x03000000 /*SPI*/
#define DCON1_SWHW_SEL 0x00800000 /*DMA source selected*/
#define DCON1_RELOAD  0x00400000 /*DMA channel is turned off*/
#define DCON1_DSZ     0x00000000 /*BYTE Will be transferred*/

#define SPSTAO_REDY   0x01     /*Ready to transmit or receive*/


tpyedef struct 2410spi_dev
{
	unsigned int cpol : 1;   //spi clock polarity
	unsigned int cpha : 1; //spi clock phase
}2410spi_dev;
