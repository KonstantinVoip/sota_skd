/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                               *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_spi.c
*
* Description : SPI to PPM module interface defenition
*
* Author      : Konstantin Shiluaev.
*
*
*В Этом Примере будем использовать Таймер3 (DTIMER_3) ,чтобы остальные таймеры
*не трогать.  PIN по которым будут идти данные
* Описание PIN где смотерть на осцилографе наш код   +FEMALE P9 HEADER+
* SPI_CLK         A17                                 -> 22 (pin)               SPI0_SCLK  //оставляем как есть   Pin mux 0
* SPD_DATA        B16                                 -> 18 (pin)               SPI0_D1    //оставляем как есть   pin mux 0
* SPI_SEL1        C12                                 -> 28 (pin)               GPIO3_17   //					  pin mux 7 режим
* SPI_SEL2        D12                                 -> 30 (pin)               GPIO3_16   //					  pin mux 7 режим
* LOAD_BUF        B13                                 -> 29 (pin)               GPIO3_15   //                     pin mux 7 режим
*
* Переключаем ППМ на приём и передачу
* TRANSMIT                                                                                  //включаем на Передачу(облучаем) скорее всего будет выключено.(по началу)
* RECIEVE                                                                                  // включаем на Приём  можно всегда RECIEVE в 1
*
*ПМ Приёмный модуль.
*Bit0 Rx Standby Control Low level standby, Rx OFF；High Level, Rx ON
*Для Примера  Передаём RX Phase Shifting   -45[градусов]
*Bit1 Rx Phase Shifting Control     =   0
*Bit2 Rx Phase Shifting Control     =   0
*Bit3 Rx Phase Shifting Control     = 	1   =	0x4	= 4		6  бит ->   Ph Фаза
*Bit4 Rx Phase Shifting Control     =   0
*Bit5 Rx Phase Shifting Control     =   0
*Bit6 Rx Phase Shifting Control     =   0
*----------------------------------------------------------------------------------
*Для Примера  Передаём Rx Attenuation Control   -1db[децибел]
*Bit7 Rx Attenuation Control        =   0
*Bit8 Rx Attenuation Control        =   0
*Bit9 Rx Attenuation Control        =   0
*Bit10Rx Attenuation Control        =	0   =   0x10 = 16	 6 бит  ->  Att Аттенюация
*Bit11Rx Attenuation Control        =   1
*Bit12Rx Attenuation Control        =   0
*
*!!!!!!!!!!!!!!!!!!!!!!!!!ППМ Приёмно-Передающий модуль!!!!!!!!!!!!!!!!!!!!!!!!
*Bit0 Rx Standby Control           =  1
*Для Примера  Передаём Tx Phase Shifting Control =-11.25[градусов]
*Bit1 Tx Phase Shifting Control    =  0
*Bit2 Tx Phase Shifting Control    =  0
*Bit3 Tx Phase Shifting Control    =  1
*Bit4 Tx Phase Shifting Control    =  0                    6 бит ->  0x4=4
*Bit5 Tx Phase Shifting Control    =  0
*Bit6 Tx Phase Shifting Control    =  0

*Для Примера  Передаём RX Phase Shifting   -180[градусов]
*Bit7 Rx Phase Shifting Control   =  0
*Bit8 Rx Phase Shifting Control   =  0
*Bit9 Rx Phase Shifting Control   =  0                     6 бит -> 0x20 =32
*Bit10 Rx Phase Shifting Control  =  0
*Bit11 Rx Phase Shifting Control  =  0
*Bit12 Rx Phase Shifting Control  =  1
**Для Примера  Передаём Tx Attenuation Control   -16db[децибел]
*Bit13 Tx Attenuation Control     =  1
*Bit14 Tx Attenuation Control     =  0
*Bit15 Tx Attenuation Control     =  0
*Bit16 Tx Attenuation Control     =  0                     6 бит -> 0x1 =1
*Bit17 Tx Attenuation Control     =  0
*Bit18 Tx Attenuation Control     =  0
*Для Примера  Передаём Rx Attenuation Control   -8db[децибел]
*Bit19 Rx Attenuation Control     =  0
*Bit20 Rx Attenuation Control     =  1
*Bit21 Rx Attenuation Control     =  0                     6 бит -> 0x2 =2
*Bit22 Rx Attenuation Control     =  0
*Bit23 Rx Attenuation Control     =  0
*Bit24 Rx Attenuation Control     =  0
**Для Примера  Передаём Rx Attenuation Control   -1db[децибел]
*Bit25 Tx Standby Control         =  1
*
*
*Для обоих модулей действует схема выбора каналов
*Select2 Select1  CS Signal   T/R Module Channel
*0 0 CS1 Low Level Select X01
*0 1 CS2 Low Level Select X02
*1 0 CS3 Low Level Select X03
*1 1 CS4 Low Level Select X04
*
*/
#include "armskd_syl_dtimer.h"
#include "armskd_syl_spi.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h" //skd_add_for_pin
#include "hw_types.h"          //skd_add_for_pin
#include "hw_dmtimer.h"
#include "beaglebone.h"
#include "interrupt.h"
#include "mcspi.h"
#include "gpio_v2.h"
#include "delay.h"
#include "dmtimer.h"
#include "error.h"

#include "stdio.h"
#include "string.h"

extern inline void dmtimer3_2_microsecond (unsigned int microsec);
/*Выбираем тип модуля ПМ ил ППМ */

//Структура при заполнении которой мы вылетаем. для версии 5.5.x работает для 5.1.6

/*
struct ppm_txrx_data {
unsigned  rx_standy 				:1 ;
unsigned  tx_phase_shifting_control :6 ;
unsigned  rx_phase_shifting_control :6 ;
unsigned  tx_attenuation_control    :6 ;
unsigned  rx_attenuation_control    :6 ;
unsigned  tx_standy 				:1 ;
}ppm_module_ch1,ppm_module_ch2,ppm_module_ch3,ppm_module_ch4;    //4 канал для ППМ модуля.
*/




/*
struct ppm_txrx_data {
unsigned  tx_standy 				:1 ;
unsigned  rx_attenuation_control    :6 ;
unsigned  tx_attenuation_control    :6 ;
unsigned  rx_phase_shifting_control :6 ;
unsigned  tx_phase_shifting_control :6 ;
unsigned  rx_standy 				:1 ;
}ppm_module_ch1,ppm_module_ch2,ppm_module_ch3,ppm_module_ch4;    //4 канал для ППМ модуля.




struct pm_rx_data {
unsigned  rx_standy 				:1 ;
unsigned  rx_phase_shifting_control :6 ;
unsigned  rx_attenuation_control    :6 ;
}pm_module;

*/

//SPI CLK какие нам надо передавать
#define MCSPI_OUT_FREQ                   (24000000u) //24 Mhz
#define MCSPI_IN_CLK                     (48000000u) //48 Mhz
#define MCSPI6Mhz_OUT_FREQ               (6000000u)  //6 Mhz
#define MCSPI5Mhz_OUT_FREQ               (5000000u)  //5 Mhz

                                                     //можно ввести ещё тестовую частоту маленькую.

/*GPIO_0  DEFINE */
#define GPIO_0_INSTANCE_ADDRESS           (SOC_GPIO_0_REGS)
#define GPIO_0_INSTANCE_PIN_NUMBER_3          (3)                      //PPM _CS 0
#define GPIO_0_INSTANCE_PIN_NUMBER_5          (5)                      //PPM _CS 1

/*GPIO_3 DEFINE */
#define GPIO3_INSTANCE_ADDRESS            (SOC_GPIO_3_REGS)
#define GPIO3_INSTANCE_PIN_NUMBER_14         (14)
#define GPIO3_INSTANCE_PIN_NUMBER_15         (15)
#define GPIO3_INSTANCE_PIN_NUMBER_16         (16)
#define GPIO3_INSTANCE_PIN_NUMBER_17         (17)


/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
******************************************************************************/
//static unsigned int IsWriteSuccess(void);
//static void McSPI0AintcConfigure(void);
//static void FlashStatusGet(void);
//static void ReadFromFlash(void);
//static void McSPITransfer(void);
//static void WriteToFlash(void);
//static void WriteEnable(void);
//static void SectorErase(void);
//static void IsFlashBusy(void);
//static unsigned int McSPISetUp(void);
//static void VerifyData(void);
//static void McSPIIsr(void);


//static void syl_write_to_spi_bus(void);
static void syl_start_spi_gpio_appl(void);
static unsigned int McSPISetUp(void);
/******************************************************************************
**                      GLOBAL VARIABLE DEFINITIONS
******************************************************************************/
//volatile unsigned int flag = 1;

static unsigned int flag =   1;
static unsigned int chNum =  0;
//static unsigned int length = 0;

//static unsigned char transmit_to_spi[1]={0x0A,0xFF};




//unsigned int data_to_channel1=0x0A0A0A0A;  //
//unsigned int data_to_channel2=0x0B0B0B0B;  //
//unsigned int data_to_channel3=0x0C0C0C0C;  //
//unsigned int data_to_channel4=0x0F0F0F0F;  //
//Должен быть массив по 26 бит данных для PPM

/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/

/******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
void gpio_spi_interface_to_ppm_block()
{
	memset(&ppm_module_ch1,0x00,sizeof(ppm_module_ch1));   //DDR  256 М/байт    0-256  М/байт
	memset(&ppm_module_ch2,0x00,sizeof(ppm_module_ch2));   //DDR  256 М/байт    0-256  М/байт
	memset(&ppm_module_ch3,0x00,sizeof(ppm_module_ch3));   //DDR  256 М/байт    0-256  М/байт
	memset(&ppm_module_ch4,0x00,sizeof(ppm_module_ch4));   //DDR  256 М/байт    0-256  М/байт

	memset(&pm_module_ch1,0x00,sizeof(pm_module_ch1));   //DDR  256 М/байт    0-256  М/байт
	memset(&pm_module_ch2,0x00,sizeof(pm_module_ch2));   //DDR  256 М/байт    0-256  М/байт
	memset(&pm_module_ch3,0x00,sizeof(pm_module_ch3));   //DDR  256 М/байт    0-256  М/байт
	memset(&pm_module_ch4,0x00,sizeof(pm_module_ch4));   //DDR  256 М/байт    0-256  М/байт

	syl_start_spi_gpio_appl();
	/*Заполняем входными данными структуру*/
}




/*Bit Toggle UP and Down*/
void inline BitToggle(unsigned int val1 ,unsigned int val2)
{
       //размещаем 32 бита
 //   HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_15);
//    HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_15);

	  asm("    movw   r2, #0x8000\n\t"  //младший разряд 0-15  бит  //GPIO PIN15 LOAD_BUF
    	   "    movt   r2, #0x0000\n\t"  //страший разряд 16-32 бит  //GPIO PIN15 LOAD_BUF
		   "str r2,[r1]" "\n\t"
		   "str r2,[r0]" "\n\t"
	   );

}


void inline BitToggle_up(unsigned int val1)
{
       //размещаем 32 бита
	   asm("    movw   r2, #0x8000\n\t"  //младший разряд 0-15  бит  //GPIO PIN15 LOAD_BUF
    	   "    movt   r2, #0x0000\n\t"  //страший разряд 16-32 бит  //GPIO PIN15 LOAD_BUF
	       "str r2,[r0]" "\n\t"
	   //    "str r2,[r1]" "\n\t"
	   );
}


void inline BitToggle_down(unsigned int val1)
{
       //размещаем 32 бита
	   asm("    movw   r2, #0x8000\n\t"  //младший разряд 0-15  бит  //GPIO PIN15 LOAD_BUF
    	   "    movt   r2, #0x0000\n\t"  //страший разряд 16-32 бит  //GPIO PIN15 LOAD_BUF
	       "str r2,[r0]" "\n\t"
	     //  "str r2,[r1]" "\n\t"
	   );
}




//26 бит
void syl_start_spi_gpio_appl()
{

//	unsigned int buf_dest_ch1=0x00000000;
//	unsigned int buf_dest_ch2=0x00000000;
//	unsigned int buf_dest_ch3=0x00000000;
//	unsigned int buf_dest_ch4=0x00000000;

	//struct ppm_txrx_data *uk1=0;
	//Заполняем структуру для ППМ Проблема не здесь а при выделении памяти

	//32 битное число по адресу.
	/*
	ppm_module_ch1.rx_standy				 = 0x1; //25 бит
	ppm_module_ch1.tx_phase_shifting_control = 0x1;
	ppm_module_ch1.rx_phase_shifting_control = 0x0;
	ppm_module_ch1.tx_attenuation_control    = 0x0;
	ppm_module_ch1.rx_attenuation_control    = 0x0;
	ppm_module_ch1.tx_standy                 = 0x1; //0 бит
    */


    /*
	ppm_module_ch1.tx_standy				 = 0x1; //25 бит
	ppm_module_ch1.rx_attenuation_control    = 0x0; //19-24 бит
	ppm_module_ch1.tx_attenuation_control    = 0x0; //13-18 бит
	ppm_module_ch1.rx_phase_shifting_control = 0x0; //7-12  бит
	ppm_module_ch1.tx_phase_shifting_control = 0x4; //1-6   бит
	ppm_module_ch1.rx_standy                 = 0x0; // 0    бит
    */

	//i=&ppm_module_ch1;
	//uk1=&ppm_module_ch1;  //получчаем адрес переменной структуры.
	//printf("val_struct=0x%x\n\r",*uk1);
	//buf_dest_ch1=0x02000001;
	//buf_dest_ch1=0x03001001;
	//memcpy(&buf_dest_ch1,&ppm_module_ch1,sizeof(buf_dest_ch1));
    //memset((void *)0x90000000,0x00,65536);   //DDR  256 М/байт    0-256  М/байт
    //memset((void *)0x80000000,0x00,65536);   //DDR  256 М/байт   256-512 М/байт
	//Заполняем структуру для ПМ
    //i=uk1;
	//HWREG(SOC_SPI_0_REGS + (0x138 + (0 * 0x14))) = *uk1;
    //printf("val_struct=0x%x\n\r",*uk1);
    //i= *uk1;

	//Инициализация SPI
//#if 0
    /* Enable the clocks for McSPI0 module.*/
	McSPI0ModuleClkConfig();
	/* Perform Pin-Muxing for SPI0 Instance  set SPI_CLK and SPI_D1(data_output) */
	McSPIPinMuxSetup(0);
    /*SETUP MCSPI CLOCK */
	McSPISetUp();



    //+++++++++++++++++++++++++++++++++++++GPIO3 MODULE ADD++++++++++++++++++++++++++++++++++++++++++++
    GPIO3ModuleClkConfig();
    //pin_mux
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_ACLKX)  = CONTROL_CONF_MUXMODE(7); // GPIO3_14
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_FSX)    = CONTROL_CONF_MUXMODE(7); // GPIO3_15  // LOAD_BUF
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AXR0)   = CONTROL_CONF_MUXMODE(7); // GPIO3_16
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKR) = CONTROL_CONF_MUXMODE(7); // GPIO3_17

    GPIOModuleEnable(GPIO3_INSTANCE_ADDRESS);
    GPIOModuleReset (GPIO3_INSTANCE_ADDRESS);

//  GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_14,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_15,GPIO_DIR_OUTPUT); //LOAD_BUF
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_16,GPIO_DIR_OUTPUT); //SEL_1
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_17,GPIO_DIR_OUTPUT); //SEL_2


   // McSPIChannelEnable(SOC_SPI_0_REGS, chNum);



   //send_to_ppm_channel_1(buf_dest_ch1);
#if 0
    while(1)
    {
    //    BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    //	BitToggle_up(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT);
    //	BitToggle_down(GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
     /*
     HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_15);
     HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_15);
     */
    //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_15);
    //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_15);
    /*
    send_to_ppm_channel_1(buf_dest_ch1);
    send_to_ppm_channel_2(0x0B0B0B0B);
    send_to_ppm_channel_3(0x0C0C0C0C);
	send_to_ppm_channel_4(0x0F0F0F0F);
    */
    }
#endif



}


/**************************************************************************************************
Syntax:      	    unsigned short send_to_ppm_channel_1(unsigned int data_26bit_to_channel1)
Remarks:			This Function wait for complete operations Read/Write.
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
unsigned short send_to_ppm_channel_1(unsigned int data_26bit_to_channel1)
{
    //Шлём на SPI channel
	//dmtimer3_2_microsecond(0);
	// McSPICSAssert(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) |= (MCSPI_CH0CONF_FORCE);

	// Enable the McSPI channel for communication.
    //McSPIChannelEnable(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) |= MCSPI_CH0CTRL_EN_ACTIVE;

	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    while(MCSPI_INT_TX_EMPTY(chNum) != (McSPIIntStatusGet(SOC_SPI_0_REGS) & MCSPI_INT_TX_EMPTY(chNum)));
    {
       //КАНАЛ 0  ->00
    	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);   //SEL_1 0
    	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);   //SEL_2 0
    	//McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(data_26bit_to_channel1), chNum);
    	HWREG(SOC_SPI_0_REGS + (0x138 + (0 * 0x14))) = (unsigned int)(data_26bit_to_channel1);
    }

    HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) &= ~(MCSPI_CH0CONF_FORCE);
   // McSPICSDeAssert(SOC_SPI_0_REGS, chNum);
    /* Disable the McSPI channel.*/
    //BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
  //  McSPIChannelDisable(SOC_SPI_0_REGS, chNum);
   // BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) &= ~MCSPI_CH0CTRL_EN_ACTIVE;
    BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    //КАНАЛ 1  ->11
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
    return 1;

}
/**************************************************************************************************
Syntax:      	    unsigned short  send_to_ppm_channel_2(unsigned int data_26bit_to_channel2)
Remarks:			This Function wait for complete operations Read/Write.
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
unsigned short  send_to_ppm_channel_2(unsigned int data_26bit_to_channel2)
{
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);

	//Шлём на SPI channel
    //McSPICSAssert(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) |= (MCSPI_CH0CONF_FORCE);

	// Enable the McSPI channel for communication.
    //McSPIChannelEnable(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) |= MCSPI_CH0CTRL_EN_ACTIVE;

	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);

    while(MCSPI_INT_TX_EMPTY(chNum) != (McSPIIntStatusGet(SOC_SPI_0_REGS) & MCSPI_INT_TX_EMPTY(chNum)));
    {

    	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   =   (1 << GPIO3_INSTANCE_PIN_NUMBER_16);     //SEL_1  1
    	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);     //SEL_2  0
    	//КАНАЛ 1  ->01
    	// HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
    	//McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(data_26bit_to_channel2), chNum);
    	 HWREG(SOC_SPI_0_REGS + (0x138 + (0 * 0x14))) = (unsigned int)(data_26bit_to_channel2);

    }


    HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) &= ~(MCSPI_CH0CONF_FORCE);
    //McSPICSDeAssert(SOC_SPI_0_REGS, chNum);
    /* Disable the McSPI channel.*/
    //McSPIChannelDisable(SOC_SPI_0_REGS, chNum);
    BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) &= ~MCSPI_CH0CTRL_EN_ACTIVE;

   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
    return 1;
}
/**************************************************************************************************
Syntax:      	    unsigned short  send_to_ppm_channel_3(unsigned int data_26bit_to_channel3)
Remarks:			This Function wait for complete operations Read/Write.
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
unsigned short  send_to_ppm_channel_3(unsigned int data_26bit_to_channel3)
{

	//Шлём на SPI channel
    //McSPICSAssert(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) |= (MCSPI_CH0CONF_FORCE);
	// Enable the McSPI channel for communication.
    //McSPIChannelEnable(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) |= MCSPI_CH0CTRL_EN_ACTIVE;


    while(MCSPI_INT_TX_EMPTY(chNum) != (McSPIIntStatusGet(SOC_SPI_0_REGS) & MCSPI_INT_TX_EMPTY(chNum)));
    {

    	//КАНАЛ 3  ->10
    	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);         //SEL_1  0
        HWREG(GPIO3_INSTANCE_ADDRESS +GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);      		 //SEL_2  1
    	//McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(data_26bit_to_channel3), chNum);
        HWREG(SOC_SPI_0_REGS + (0x138 + (0 * 0x14))) = (unsigned int)(data_26bit_to_channel3);
    }


    HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) &= ~(MCSPI_CH0CONF_FORCE);
    //McSPICSDeAssert(SOC_SPI_0_REGS, chNum);
    /* Disable the McSPI channel.*/
    //McSPIChannelDisable(SOC_SPI_0_REGS, chNum);
    BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) &= ~MCSPI_CH0CTRL_EN_ACTIVE;

  //  HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
  //  HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);

    return 1;

}
/**************************************************************************************************
Syntax:      	    unsigned short  send_to_ppm_channel_4(unsigned int data_26bit_to_channel4)
Remarks:			This Function wait for complete operations Read/Write.
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
unsigned short send_to_ppm_channel_4(unsigned int data_26bit_to_channel4)
{
	//Шлём на SPI channel
    //McSPICSAssert(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) |= (MCSPI_CH0CONF_FORCE);
	// Enable the McSPI channel for communication.
    //McSPIChannelEnable(SOC_SPI_0_REGS, chNum);
	HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) |= MCSPI_CH0CTRL_EN_ACTIVE;


    while(MCSPI_INT_TX_EMPTY(chNum) != (McSPIIntStatusGet(SOC_SPI_0_REGS) & MCSPI_INT_TX_EMPTY(chNum)));
    {

      //КАНАЛ 4  ->11
      HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);            //SEL_1  1
      HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);            //SEL_2  1
      HWREG(SOC_SPI_0_REGS + (0x138 + (0 * 0x14))) = (unsigned int)(data_26bit_to_channel4);
      //McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(0x0A0A0A0A), chNum);
    }


    HWREG(SOC_SPI_0_REGS + MCSPI_CHCONF(chNum)) &= ~(MCSPI_CH0CONF_FORCE);
    //McSPICSDeAssert(SOC_SPI_0_REGS, chNum);
    /* Disable the McSPI channel.*/
    //McSPIChannelDisable(SOC_SPI_0_REGS, chNum);
    BitToggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    HWREG(SOC_SPI_0_REGS + MCSPI_CHCTRL(chNum)) &= ~MCSPI_CH0CTRL_EN_ACTIVE;

  return 1;
}



/**************************************************************************************************
Syntax:      	    static unsigned int McSPISetUp(void)
Remarks:			This Function Initialize McSPI BUs
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
static unsigned int McSPISetUp(void)
{

    static unsigned int ret_val=0;

	// Reset the McSPI instance.
    McSPIReset(SOC_SPI_0_REGS);

    // Enable chip select pin.
    McSPICSEnable(SOC_SPI_0_REGS);

    // Enable master mode of operation.
    McSPIMasterModeEnable(SOC_SPI_0_REGS);

    // Perform the necessary configuration for master mode.
    ret_val=McSPIMasterModeConfig(SOC_SPI_0_REGS, MCSPI_SINGLE_CH,MCSPI_TX_ONLY_MODE, MCSPI_DATA_LINE_COMM_MODE_1,chNum);
    if(!ret_val)
    {
    	ConsoleUtilsPrintf("McSPIMasterModeConfig_ERRROR=%d\r\n",ret_val);
        return 0;
    }

    // Configure the McSPI bus clock depending on clock mode.
   // McSPIClkConfig(SOC_SPI_0_REGS, MCSPI_IN_CLK, MCSPI6Mhz_OUT_FREQ, chNum,MCSPI_CLK_MODE_1);
    McSPIClkConfig(SOC_SPI_0_REGS, MCSPI_IN_CLK, MCSPI5Mhz_OUT_FREQ, chNum,MCSPI_CLK_MODE_1);

    // Configure the word length.
   //McSPIWordLengthSet(SOC_SPI_0_REGS, MCSPI_WORD_LENGTH(8), chNum);


#ifdef TYPE_MODULE_PPM
    McSPIWordLengthSet(SOC_SPI_0_REGS, MCSPI_WORD_LENGTH(26), chNum);
#endif

#ifdef TYPE_MODULE_PM
    McSPIWordLengthSet(SOC_SPI_0_REGS, MCSPI_WORD_LENGTH(13), chNum);
#endif

    // Set polarity of SPIEN to low.
    McSPICSPolarityConfig(SOC_SPI_0_REGS, MCSPI_CS_POL_LOW, chNum);

    // Enable the transmitter FIFO of McSPI peripheral.
    McSPITxFIFOConfig(SOC_SPI_0_REGS, MCSPI_TX_FIFO_ENABLE, chNum);

    // Enable the receiver FIFO of McSPI peripheral.
   // McSPIRxFIFOConfig(SOC_SPI_0_REGS, MCSPI_RX_FIFO_ENABLE, chNum);
    McSPIRxFIFOConfig(SOC_SPI_0_REGS, MCSPI_RX_FIFO_DISABLE, chNum);

 return 1;
}


/*
** McSPI Interrupt Service Routine. This function will clear the status of the
** Tx/Rx interrupts when generated. Will write the Tx data on transmit data
** register and also will put the received data from receive data register to
** a location in memory.
*/
#if 0
static void McSPIIsr(void)
{
    unsigned int intCode = 0;

    //intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);


    intCode = (HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS));


   // (HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS));

    //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // ConsoleUtilsPrintf("McSPIIsr=%d\r\n",intCode);
    while(intCode)
    {
    	 //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    	//dtimer3_delay();
    	if(MCSPI_INT_TX_EMPTY(chNum) == (intCode & MCSPI_INT_TX_EMPTY(chNum)))
        {

        	 //Interrup Status Clear  должен быть именно сначала а не в конце иначе косяк происходит
        	 //McSPIIntStatusClear(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));
        	 //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    		 //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);

        	 /* Clear the SSB bit in the MCSPI_SYST register. */
        	 HWREG(SOC_SPI_0_REGS + MCSPI_SYST) &= ~MCSPI_SET_STATUS_BIT;

        	 /* Clear the interrupt status. */
        	 HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS) = MCSPI_INT_TX_EMPTY(chNum);


         	//Здесь должнобыть управление SEL_1
             //Здесь должно быть управление SEL_2
         	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
            HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);


        	 /////////////////////Наш PPM_SEL1///////////////////////////////////
        	 /////////////////////Наш PPM_SEL2///////////////////////////////////


             /*Загрузка данных и старт Передачи */
        	 /* Load the MCSPI_TX register with the data to be transmitted */
        	// HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
        	 HWREG(SOC_SPI_0_REGS + MCSPI_TX(chNum)) = (unsigned int)(0x0A0A0A0A);


             /////////////////////Наш PPM_SEL1///////////////////////////////////
             /////////////////////Наш PPM_SEL2///////////////////////////////////
             //Disable Interrupt
        	 HWREG(SOC_SPI_0_REGS + MCSPI_IRQENABLE) &= ~ MCSPI_INT_TX_EMPTY(chNum);


        	 /* Clear the SSB bit in the MCSPI_SYST register. */
        	// HWREG(SOC_SPI_0_REGS + MCSPI_SYST) &= ~MCSPI_SET_STATUS_BIT;

        	 /* Clear the interrupt status. */
        	// HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS) = MCSPI_INT_TX_EMPTY(chNum);

           flag = 0;  //add skd_voip
           HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
           HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);

        }

    	intCode = (HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS));
    	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    } //end WHILE
  //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);

}
#endif






//CURENT _OK _VARIANT
#if 0
static void McSPIIsr(void)
{
    unsigned int intCode = 0;
    intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);



    while(intCode)
    {
        if(MCSPI_INT_TX_EMPTY(chNum) == (intCode & MCSPI_INT_TX_EMPTY(chNum)))
        {

        	McSPIIntStatusClear(SOC_SPI_0_REGS,(1 << ((chNum) * 4)));

        	//Здесь должнобыть управление SEL_1
            //Здесь должно быть управление SEL_2
        	HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
            HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
            //Отправляем данные на шину 26 бит
            HWREG(SOC_SPI_0_REGS + (0x138 + (0 * 0x14))) = (unsigned int)(0x0A0A0A0A);

            McSPIIntDisable(SOC_SPI_0_REGS,(1 << ((chNum) * 4)));
            McSPIIntStatusClear(SOC_SPI_0_REGS,(1 << ((chNum) * 4)));


            //Здесь должнобыть управление SEL_1
            //Здесь должно быть управление SEL_2
            HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
            HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
            flag = 0;  //add skd_voip
        }
     intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);
    }

}
#endif

//END CURRENT  OK _VARIANT





//Рабочий вариант передачи данных к ППМ блоку
#if 0
static void McSPIIsr(void)
{
    unsigned int intCode = 0;

    //intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);


    intCode = (HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS));


   // (HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS));


   // ConsoleUtilsPrintf("McSPIIsr=%d\r\n",intCode);
    while(intCode)
    {
        if(MCSPI_INT_TX_EMPTY(chNum) == (intCode & MCSPI_INT_TX_EMPTY(chNum)))
        {


        	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);

        	McSPIIntStatusClear(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));
            //ConsoleUtilsPrintf("MCSPI_INT_TX_EMPTY=%d\r\n",length);
            length--;
            //McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(*p_tx++), chNum);
            McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(0x0A0A0A0A), chNum);


            if(!length)
            {
            	//ConsoleUtilsPrintf("?MCSPI_INT_TX_EMPTY=%d?\r\n",length);
            	//McSPIIntDisable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));

            	  /* Disable the interrupts. */
            	HWREG(SOC_SPI_0_REGS + MCSPI_IRQENABLE) &= ~ MCSPI_INT_TX_EMPTY(chNum);

                //McSPIIntStatusClear(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));
            	flag = 0;  //add skd_voip
                //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT) = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
            }
        }
     //length--;
     //intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);
        intCode = (HWREG(SOC_SPI_0_REGS + MCSPI_IRQSTATUS));

    } //end WHILE
}
#endif






























