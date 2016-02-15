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

#define REVISION_APPLICATION  210815

/*GPIO START*/
#define GPIO_INSTANCE_ADDRESS           (SOC_GPIO_1_REGS)
#define GPIO_INSTANCE_PIN_NUMBER        (23)
#define GPIO_INSTANCE_PIN_NUMBER_22     (22)


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

//Работает функция
/*
	unsigned char buf_from[4]={0x11,0x22,0x33,0x44};
	unsigned char buf_dest[4]={0x00,0x00,0x00,0x00};
	memcpy(&buf_dest,&buf_from,sizeof(buf_dest));
*/

/*
		unsigned int buf_from[4]={0x11223344,0x22556677,0x33889900,0x44112233};
		unsigned int buf_dest[4]={0x00000000,0x00000000,0x00000000,0x00000000};
		memcpy(&buf_dest,&buf_from,sizeof(buf_dest));
*/

/* Работает.
			unsigned int buf_from=0x11223344;
			unsigned int buf_dest=0x00000000;
			memcpy(&buf_dest,&buf_from,sizeof(buf_dest));
*/


    //Работает.
    /*
	asm("    mov   r0, #3\n\t"
        "    mov   r1, #2\n\t"
        "    add   r2, r1, r0"
    		);
   */



  //Работает 32 бит
  /*
	 //размещаем 32 бита
		   asm("    movw   r2, #8000\n\t"  //младший разряд 0-15  бит
	    	   "    movt   r2, #0001\n\t"  //страший разряд 16-32 бит
		   );
  */


	//Включаем КЭШ память L1_data ,L1_instruction and L2 Uni fied cache
    //CacheEnable(CACHE_ALL);

	//Зажигаем Светодиод что мы стартовали нормально в функции
    /*Enabling functional clocks for GPIO1 instance. */
    GPIO1ModuleClkConfig();
    /* Selecting GPIO1[23] pin for use. */
    GPIO1Pin23PinMuxSetup();
    /* Enabling the GPIO module. */
    GPIOModuleEnable(GPIO_INSTANCE_ADDRESS);
    /* Resetting the GPIO module. */
    GPIOModuleReset(GPIO_INSTANCE_ADDRESS);
    /* Setting the GPIO pin as an output pin. */
    GPIODirModeSet(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER_22,GPIO_DIR_OUTPUT);
    GPIOPinWrite(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER_22,GPIO_PIN_HIGH);
    ConsoleUtilsPrintf("+appl:PELENGATOR SOTA_START_OK_rev=%d+\n\r",REVISION_APPLICATION);



    //Perfomance Timer
    /*
    int a[200],b[200],c[200];
    int i;

    dmtimer7_SysPerfTimerSetup();
    dmtimer7_PerfTimerStart(0);
    for (i = 0; i < 200; i++)
     {
        a[i]= b[i]=i+1;
     }
    for (i = 0; i < 200; i++)
     {
         c[i]= a[i] * b[i];
     }
    dmtimer7_PerfTimerStop(0);
    */

    //init_syl_uart();

    //syl_dtimer3_init();

     //init_syl_dtimer();
     //gpio_spi_interface_to_ppm_block();
     //start_poisk_table_algoritm();
     // init_syl_uart();
    //Инициализация SPI шины выполняем.
    //Теперь работаем с SPI шиной стыковка с PPM Блоком через GPIO шину
   // gpio_spi_interface_to_ppm_block();
    //Теперь работаем с инициализацией I2C
    //  init_syl_i2c_bus();

    //test_data_types();
    //Здесь должно быть :СОТА или КРОСС
    //ConsoleUtilsPrintf("appl:Type_of_block=%s+\n\r",REVISION_APPLICATION);
    //Здесь ПМ или ППМ модуль и порядковый номер блока.                                     //Прочитали данные с шины I2C <->соединённой с ПЛИС
    //ConsoleUtilsPrintf("appl:Type=%s,number_of_block=%d\n\r",REVISION_APPLICATION);

    //read_from_emmc_to_DDR();   //читаем и распологаем Таблицы из eMMC в DDR память.
    //Здесь должна                                                              hfkk            //Загрузка таблиц из eMMC флэшки в DDR
    //ConsoleUtilsPrintf("appl:tfa_tab=[%d][%d][%d][%d],tyr_tab=[%d][%d][%d][%d]\n\r");

    /*Тестовое меню для осуществления Проверки работоспосоьности блока сота. */
#ifdef  ENABLE_MENU
    syl_MainMenu();
#endif

#ifdef PRU1_IMAGE_ENABLE
     PRU_skd_syl_test() ;
#endif

    //Функция Чтения таблиц[TYR/TFA] с eMMC флэшки в DDR паямть.
    //После Старта приложения выполняем эту функцию
    //1.
    // read_from_emmc_to_DDR();   //читаем и распологаем Таблицы из eMMC в DDR память.

     //работает функция приёма данных по XMODEM RECIEVE
     //test_main_xmodem_recieve_functions();


    //Инициализация меню для ввода управляющих данных PPM модуля
	//init_clock_pll_mpu_and_config();  //clock and pll
    //Чтение из регистров
    //reg=(HWREG(SOC_DMTIMER_2_REGS + 0x0));
    //syl_Sysdelay(50);
    //Запись в регистры
    //HWREG(GPIO_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO_INSTANCE_PIN_NUMBER);
   //test_functions();
   //init_syl_dtimer();
    //init_syl_i2c_bus();
	  // init_syl_gpio();
	 // init_syl_spi_bus();
	//sets pin mux for PRU cape

}


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





#if 0

//******************************************************************************
//    LED Test
//      This function tests the 7 LEDs on the PRU Cape.
//******************************************************************************
int LEDTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 LED TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU0 Instructions and Data...\n");

	PRUMemLoad(PRU_ICSS1, PRU0_IRAM, 0, sizeof(LED0_INST), (unsigned int*)LED0_INST);
	PRUMemLoad(PRU_ICSS1, PRU0_DRAM, 0, sizeof(LED0_DATA), (unsigned int*)LED0_DATA);

	ConsoleUtilsPrintf("\nLoading PRU1 Instructions and Data...\n");
	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(LED1_INST), (unsigned int*)LED1_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(LED1_DATA), (unsigned int*)LED1_DATA);

	ConsoleUtilsPrintf("\nRunning PRUs...\n");
	PRUEnable(PRU_ICSS1, PRU0);
	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nDo you see all 7 LEDs flashing? y/n\n");
	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU0);
	PRUHalt(PRU_ICSS1, PRU1);

	return(answer[0]);
}

//******************************************************************************
//    Switch Test
//      This function tests the Push Button switches on the PRU Cape.
//      Pressing each switch will toggle an LED on the PRU Cape.
//******************************************************************************
int SwitchTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 SWITCH TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");
	PRUMemLoad(PRU_ICSS1, PRU0_IRAM, 0, sizeof(SW_INST), (unsigned int*)SW_INST);
	PRUMemLoad(PRU_ICSS1, PRU0_DRAM, 0, sizeof(SW_DATA), (unsigned int*)SW_DATA);

	ConsoleUtilsPrintf("\nRunning PRU0...\n");
	PRUEnable(PRU_ICSS1, PRU0);

	ConsoleUtilsPrintf("\nDo the switches turn on LEDs? y/n\n");
	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU0);

	return(answer[0]);
}

//******************************************************************************
//    Temp Sensor Test
//      This function tests the temp sensor on the PRU Cape. If the temperature
//      rises, a red LED will turn on.  If the temperature falls, a blue LED
//      will turn on.
//******************************************************************************
int HDQTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 TEMP SENSOR TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");

	PRUMemLoad(PRU_ICSS1, PRU0_IRAM, 0, sizeof(SLAVE_INST), (unsigned int*)SLAVE_INST);
	PRUMemLoad(PRU_ICSS1, PRU0_DRAM, 0, sizeof(SLAVE_DATA), (unsigned int*)SLAVE_DATA);

	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(MASTER_INST), (unsigned int*)MASTER_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(MASTER_DATA), (unsigned int*)MASTER_DATA);

	PRUEnable(PRU_ICSS1, PRU1);
	PRUEnable(PRU_ICSS1, PRU0);

	ConsoleUtilsPrintf("\nDoes the red and blue LEDs light up during heating and \n cooling the temperature sensor? y/n\n");

	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU0);
	PRUHalt(PRU_ICSS1, PRU1);

	return(answer[0]);
}

//******************************************************************************
//    Audio Test
//      This function tests the audio output on the PRU Cape by playing a
//      single tone.
//******************************************************************************

#if 0

int AudioTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 AUDIO TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");

	PRUHalt(PRU_ICSS1, PRU1);

	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(AUDIO_INST), (unsigned int*)AUDIO_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(AUDIO_DATA), (unsigned int*)AUDIO_DATA);

	ConsoleUtilsPrintf("\nRunning PRU1...\n");

	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nAre the speakers making a tone? y/n\n");

	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU1);

	return(answer[0]);
}
#endif


//******************************************************************************
//    UART Test
//      This function tests the RS-232 UART port on the PRU Cape by requesting
//      the user type 5 characters in a console and echoing the characters back.
//******************************************************************************
int UARTTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 UART TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");

	PRUHalt(PRU_ICSS1, PRU1);

	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(UART_INST), (unsigned int*)UART_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(UART_DATA), (unsigned int*)UART_DATA);

	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nDid the UART work? y/n\n");

	ConsoleUtilsGets(answer, 3);
	return(answer[0]);
}


#endif
