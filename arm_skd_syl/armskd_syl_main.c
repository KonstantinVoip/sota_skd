/******************************************************************************************************
*                                    (c) COPYRIGHT by ZAO NPK PELENGATOR                              *
*                                            All rights reserved.                                     *
*******************************************************************************************************
* Module      : armskd_syl_main.c
*
* Description : Main File
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "consoleUtils.h"
#include "hw_types.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "beaglebone.h"
#include "hw_cm_per.h"
#include "hw_prm_per.h"
#include "gpio_v2.h"

#include "mmu.h"
#include "cache.h"

#include "armskd_syl_main.h"
#include "pru.h"


//Extern Defenition for PRIMER
#include "armskd_syl_dtimer.h"     //This system config
#include "armskd_syl_boardcfg.h"   //This board  config

#include "armskd_syl_spi.h"
#include "armskd_syl_i2c.h"
#include "armskd_syl_copy_to_ddr.h"
#include "armskd_syl_algoritm.h"


#include "armskd_syl_uart.h"


#include "armskd_syl_xmodem.h"
#include "armskd_syl_menu.h"


//#include "pru_cape_demo.h"
//#include "list.h"
//#include "byteorder.h"
#define START_ADDR_DDR                  (0x80000000u)
#define START_ADDR_DEV                  (0x44000000u)
#define START_ADDR_OCMC                 (0x40300000u)
#define NUM_SECTIONS_DDR                (512u)
#define NUM_SECTIONS_DEV                (960u)
#define NUM_SECTIONS_OCMC               (1u)


/*
#pragma DATA_SECTION(bufferB, "application")
unsigned char bufferB[131072];
*/

//Включаем поддержку сборки целого образ с PRU сопроцессорами.
//#define PRU1_IMAGE_ENABLE 1


//Включаем выключаем использование меню
//#define ENABLE_MENU   1



//Надо условиться что в режиме отладки приложения у нас будет версия 0.0.1 приложения application
//в ней будет множество отладочных изменяемых файлов.в репозитории Git нужно постоянно класть там контролировать
//изменяемые файлы.



#define REVISION_APPLICATION  210815

/*GPIO START*/
//#define GPIO_INSTANCE_ADDRESS           (SOC_GPIO_1_REGS)
//#define GPIO_INSTANCE_PIN_NUMBER        (23)
//#define GPIO_INSTANCE_PIN_NUMBER_22     (22)


//#pragma DATA_ALIGN(PageTable, MMU_PAGETABLE_ALIGN_SIZE);
//static volatile unsigned int PageTable[MMU_PAGETABLE_NUM_ENTRY];

/*
#pragma DATA_SECTION(buf_tab_1, "tab1")
unsigned char  buf_tab_1[16777216];           //16 М/байт

#pragma DATA_SECTION(buf_tab_2, "tab2")       //16 М/байт
unsigned char  buf_tab_2[16777216];

#pragma DATA_SECTION(buf_tab_3, "tab3")       //16 М/байт
unsigned char  buf_tab_3[16777216];

#pragma DATA_SECTION(buf_tab_4, "tab4")       //16 М/байт
unsigned char  buf_tab_4[16777216];
*/


/******************************************************************************
**              FUNCTION DEFINITIONS
******************************************************************************/

//******************************************************************************
//    UARTInit
//      This function initializes the UART console.
//******************************************************************************
void UARTInit(void)
{
	ConsoleUtilsInit();
	//ConsoleUtilsSetType(CONSOLE_UART);
}

/*Типы данных   64 bit
* long long,signed long long           -9 223 372 036 854 775 808 <----> 9 223 372 036 854 775 807
* unsigned long long                                             0 <----> 18 446 744 073 709 551 615
*
*Типы данных   32 bit
 *unsigned int                                     0   <-------->    4 294 967 295
 *int, signed int                       -2 147 483 648 <-------->    2 147 483 647
 * Типы данных   16 bit
 * short,   signed short 16 bits 2s complement -32 768 <---------> 32 767
 * unsigned short, wchar_t (2) 16 bits Binary      0   <---------> 65 535
 *Типы данных   8 bit
 * signed char                                   -128  <----------> 127
 * char                                            0   <----------> 255
 *  */
static void test_data_types()
{

	/*
	unsigned int
	signed   int
	unsigned short
	signed   short
	signed char
	unsigned char
	float*/
	unsigned long long   a=0x1122334455667788;
//	signed long long     b;
//	float                c=2.1453218;
	unsigned int         d=0x11112222;
//	signed   int         e;
	unsigned short       f=0x3344;
//	signed   short       g;
//	signed   char        h;
	unsigned char        m=0x51;

	ConsoleUtilsPrintf("unsigned_long=%d,0x%x\n\r",sizeof(a),a);  //8 байт,64 бит
//	ConsoleUtilsPrintf("signed_long=%d\n\r",sizeof(b));    //8 байт,64 бит

//	ConsoleUtilsPrintf("float=%d\n\r",sizeof(c));  		   //4 байт,32 бит

//	ConsoleUtilsPrintf("signed_int =%d\n\r",sizeof(e));    //4 байт,32 бит
	ConsoleUtilsPrintf("unsigned_int =%d,0x%x\n\r",sizeof(d),d);  //4 байт,32 бит


//	ConsoleUtilsPrintf("signed_short=%d\n\r",sizeof(g));    //2 байт,16 бит
	ConsoleUtilsPrintf("unsigned_short=%d,0x%x\n\r",sizeof(f),f);  //2 байт,16 бит


//	ConsoleUtilsPrintf("unsigned_char=%d\n\r",sizeof(h));  //1 байт,8 бит
	ConsoleUtilsPrintf("signed_char=%d,0x%x\n\r",sizeof(m),m);    //1 байт,8 бит
}


//******************************************************************************
//    Main
//******************************************************************************
int main()
{

//Включить меню вывода на экран.
#ifdef  ENABLE_MENU
    syl_MainMenu();
#endif

#ifdef PRU1_IMAGE_ENABLE
     PRU_skd_syl_test() ;
#endif

}


/* Видимо механизм должен быть таким,я принял SIP пакет из сети ethernet
 * с нужным данными SIP сигнализации,установил соединение с нужным номером из промежуточной
 * таблицы к удалённому телефону абоненту ,потом повалились RTP Пакеты. нужно их быстро очищать от заголовка
 * и пересылать в DMA канал MCASP
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *








































//Задел под PRU будущее.

#ifdef PRU1_IMAGE_ENABLE

void  PRU_skd_syl_test()
{

    //Рабочий кусок нашего массива для PRU модуля
    PRUCapePinmux(); //можно сделать pin_mux
    PRUICSSInit();
   // PRUICSSReset();
	ConsoleUtilsPrintf("\nLoading PRU1 Instructions and Data...\n");
	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(PRU1_SKDSYL_INST), (unsigned int*)PRU1_SKDSYL_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(PRU1_SKDSYL_DATA), (unsigned int*)PRU1_SKDSYL_DATA);

	ConsoleUtilsPrintf("\nRunning PRUs...\n");
	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nDo you see all 7 LEDs flashing? y/n\n");

}
#endif






