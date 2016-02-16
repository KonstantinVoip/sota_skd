/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                     *
*                                            All rights reserved.                                                *
***********************************************************************************************************************
* Module      : armskd_syl_dtimer.c
*
* Description :
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************/
/**
 * \file   dmtimerCounter.c
 *
 * \brief  Sample application for DMTimer. The application will
 *         count down from 9-0.
 *
 *         Application Configuration:
 *
 *             Modules Used:
 *                 DMTimer2
 *                 UART0
 *
 *             Configurable parameters:
 *                 None.
 *
 *             Hard-coded configuration of other parameters:
 *                 Mode of Timer - Timer mode(Auto reload)
 *
 *         Application Use Case:
 *             The application demonstrates DMTimer in Autoreload mode
 *             of operation. In the example for every overflow of DMTimer
 *             the counter register is reloaded with contents of overflow
 *             register. This sequence is continued 10 times and at each
 *             overflow a decrementing value is printed on the serial
 *             console showcasing the DMTimer as a down counter.
 *
 *         Running the example:
 *             On execution, the example will count down from 9 - 0 and stop.
 *             The time interval between each count is approximate to 700ms.
 *
 */

/*
* ТАЙМЕР до 0 не считает  срабатывает только при 0xFFFFFFFF (считает только вперёд)
* Для 32768 КгЦ
* Расчёт значений которые идут в TLDR  при таймере 32768 [кГц]
* цена 1 тика = 31.25[мкс]
* time_clk_period = 1/32768  = 0.00003051[с] = 30.51 [мкс]
*
* хотим задержку  ~2 секунды равно 65535 * 0.00003051 тиков =~ 1.99947285[с]
* TLDR = 0xFFFFFFFF - 0xFFFF = 0xFFFF0000;
* время_прерывания[с] = (0xFFFFFFFF -TLDR+1) * time_clk_period*PS(1)
*
* Для 24Мгц  = 24000000 [Гц]
* time_clk_period = 1/24 000 000  = 4.16666666667 * 10 -8  [41.6] [нс]
*
*1 count at 32.768 KHz takes 31.25us
*0x1900 counts takes 200ms  */

/*В этом Примере используем DMTIMER3 при частоте главного осцилятора 24[мгц] соответсвенно и делители
 * будут у нас такие */

#include "consoleUtils.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h" //skd_add_for_pin
#include "hw_types.h"          //skd_add_for_pin
#include "beaglebone.h"
#include "interrupt.h"
#include "dmtimer.h"
#include "gpio_v2.h"
#include "error.h"
#include "perf.h"
#include "clock.h"
#include "mmu.h"
#include "cache.h"

#include "armskd_syl_dtimer.h"


#define DEBUG_TIMER_FROM_GPIO_PIN  1

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS GPIO3
*****************************************************************************/
#define GPIO3_INSTANCE_ADDRESS           (SOC_GPIO_3_REGS)
#define GPIO3_INSTANCE_PIN_NUMBER_14        (14)
#define GPIO3_INSTANCE_PIN_NUMBER_15        (15)
#define GPIO3_INSTANCE_PIN_NUMBER_16        (16)
#define GPIO3_INSTANCE_PIN_NUMBER_17        (17)


/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/

/******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

/*Добавки работы с MMU */
#define START_ADDR_DDR                     (0x80000000)
#define START_ADDR_DEV                     (0x44000000)
#define START_ADDR_OCMC                    (0x40300000)
#define NUM_SECTIONS_DDR                   (512)
#define NUM_SECTIONS_DEV                   (960)
#define NUM_SECTIONS_OCMC                  (1)




/******DTIMER3 DEFENITION FOR WORKSPACE*****/

//#define TIMER_INITIAL_COUNT                     (0xFFFF0000u)    //вот как нужно получить 65535 тиков  32768 [Кгц] 1 прерывание = 2 секунды.
//#define TIMER_RLD_COUNT                         (0xFFFF0000u)    //вот как нужно получить 65535 тиков  32768 [Кгц] 1 прерывание = 2 секунды.

//#define TIMER_INITIAL_COUNT                   (0xFFFE0001u)    //вот как нужно получить 65535 тиков  32768 [Кгц] 1 прерывание = 4 секунды.
//#define TIMER_RLD_COUNT                       (0xFFFE0001u)    //вот как нужно получить 65535 тиков  32768 [Кгц] 1 прерывание = 4 секунды.

//пример для MMU
#define TIMER_INITIAL_COUNT             (0xFFFFFFFFu-12)
#define TIMER_RLD_COUNT                 (0xFFFFFFFFu-12)


//Меряем Производительность
void         dmtimer7_SysPerfTimerSetup(void);
void         dmtimer7_PerfTimerStart(unsigned short);          //запускаем таймер
unsigned int dmtimer7_PerfTimerStop(unsigned short);           //возвращаем количество тиков


//#define TIMER_INITIAL_COUNT             (0xFFFFA23Fu)
#define TIMER_PERF_BASE                 (SOC_DMTIMER_7_REGS)

//static inline void dtimer3_delay();   //здесь должен быть таймер с задержкой 1 мкс
static void DMTimerSetUp(void);
//static void DMTimerAintcConfigure(void);
static void DMTimerSetUp(void);
//static void DMTimerIsr(void);

static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;
unsigned int cnt=0;


#define TIME_GRANULARITY                (1000000u)

#pragma DATA_ALIGN(pageTable, 16384);
static volatile unsigned int pageTable[4*1024];


//Меряем Производительность
void  inline  dmtimer7_SysPerfTimerSetup(void)
{
    DMTimer7ModuleClkConfig();
    DMTimerCounterSet(TIMER_PERF_BASE, 0);
}

//запускаем  таймер   //flag =1
void inline  dmtimer7_PerfTimerStart(unsigned short flag)
{

     // DMTimerCounterSet(TIMER_PERF_BASE, 0);
	  HWREG(TIMER_PERF_BASE + DMTIMER_TCRR) = 0;

	  //DMTimerEnable(TIMER_PERF_BASE);
	   /* Start the timer */
	   HWREG(TIMER_PERF_BASE + DMTIMER_TCLR) |= DMTIMER_TCLR_ST;


}

//останавливаем  таймер   //flag =0 выводим на печать сколько получили
unsigned int inline dmtimer7_PerfTimerStop(unsigned short flag)
{
    unsigned int timeInTicks = 0;
    unsigned int time=0;
    //DMTimerDisable(TIMER_PERF_BASE);
    HWREG(TIMER_PERF_BASE + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

    timeInTicks = DMTimerCounterGet(TIMER_PERF_BASE);

    /* Get the time taken in from the ticks read from the Timer */
    time = (timeInTicks/ (CLK_EXT_CRYSTAL_SPEED/TIME_GRANULARITY));
    ConsoleUtilsPrintf("-takes %u [microsec] %u[nano]  %u [ticks]-\r\n",time,timeInTicks*41,timeInTicks);

 //возвращаем количество тиков
 return timeInTicks;
}


static inline void test_perfomance()
{
//unsigned int val;
//val = cnt % 2
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
	  /*
      switch(cnt % 2)
      {
      case 0:
    	  HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
      break;


      default:
    	  HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
      break;
      }
      */

	//if (cnt % 2==0) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    //один pin GPIO16
   // else HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // cnt++;
	//asm("NOP");
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    //HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
}


/*
static inline void test_perfomance()
{
	asm("NOP");

	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
}
*/

void inline ggle_up(unsigned int val1)
{
       //размещаем 32 бита
	   asm("    movw   r2, #0x0000\n\t"  //младший разряд 0-15  бит  //GPIO PIN15 LOAD_BUF
    	   "    movt   r2, #0x0001\n\t"  //страший разряд 16-32 бит  //GPIO PIN15 LOAD_BUF
	       "str r2,[r0]" "\n\t"
	   //    "str r2,[r1]" "\n\t"
	   );
}


void inline ggle_down(unsigned int val1)
{
       //размещаем 32 бита
	   asm("    movw   r2, #0x0000\n\t"  //младший разряд 0-15  бит  //GPIO PIN15 LOAD_BUF
    	   "    movt   r2, #0x0001\n\t"  //страший разряд 16-32 бит  //GPIO PIN15 LOAD_BUF
	       "str r2,[r0]" "\n\t"
	     //  "str r2,[r1]" "\n\t"
	   );
}



void inline Toggle(unsigned int val1 ,unsigned int val2)
{
       //размещаем 32 бита
	   asm("    movw   r2, #0x0000\n\t"  //младший разряд 0-15  бит  //GPIO PIN15 LOAD_BUF
    	   "    movt   r2, #0x0001\n\t"  //страший разряд 16-32 бит  //GPIO PIN15 LOAD_BUF
		   "str r2,[r1]" "\n\t"
		   "str r2,[r0]" "\n\t"
	   );
}




/**************************************************************************************************
Parameters:         int init_syl_dtimer(void)
Remarks:            устанавливаем задержку таймера в секундах [1-60]
***************************************************************************************************/
int init_syl_dtimer(void)
{

 //unsigned int timerTicks=0;
 //unsigned int time=0;
 unsigned int i=0;
 unsigned int cnt=0;

 //Инициализируем MMU
 //MMUConfigAndEnable();
 //CacheEnable(CACHE_ALL);



//Инициализируем GPIO для Проверки Таймеров
//Используем отладку для GPIO возьмём один GPIO
#ifdef DEBUG_TIMER_FROM_GPIO_PIN

	GPIO3ModuleClkConfig();
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AXR0)   = CONTROL_CONF_MUXMODE(7); // GPIO3_16   //30 pin
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKR) = CONTROL_CONF_MUXMODE(7); // GPIO3_17   //28 pin
    GPIOModuleEnable(GPIO3_INSTANCE_ADDRESS);
    GPIOModuleReset (GPIO3_INSTANCE_ADDRESS);
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_16,GPIO_DIR_OUTPUT); //SEL_1
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_17,GPIO_DIR_OUTPUT); //SEL_2

#endif

    DMTimer3ModuleClkConfig();
    DMTimerSetUp();


    while(1)
    {
    if (cnt % 2==0) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);   //один pin GPIO16
       else HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);   //один pin GPIO16
   	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
       //if (cnt==0) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);                            //другой pin GPIO15
       //else if (cnt==200000) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
       //else if (cnt==400000) cnt=-1;
       cnt++;
    }







#if 0
    DMTimer3ModuleClkConfig();
    /* Enable IRQ in CPSR */
    IntMasterIRQEnable();
    /* Register DMTimer2 interrupts on to AINTC */
    DMTimerAintcConfigure();
    DMTimerSetUp();
    DMTimerIntEnable(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_EN_FLAG);
    DMTimerEnable(SOC_DMTIMER_3_REGS);
#endif


#if 0 //выполняем perfomance counter
    /* Setup the Timer for Performance measurement */
      dmtimer7_SysPerfTimerSetup();
    /* Start the Timer for Performance Measurement */
      dmtimer7_PerfTimerStart(0);
     // for(i=0;i<1000000;i++)
      for(i=0;i<1;i++)
      {
     // test_perfomance();
      dtimer3_delay();

      }
      dmtimer7_PerfTimerStop(1);

//#endif


     while(1);  //Конуц нашей программы должен заканчиваться на бесконечном выполнении инструкций.

    //Основное Приложение по которому проверяем скорость поднимание и опискание фронтов
#endif

#if 0
    while(1)
    {

    //test_perfomance();
    //Это реально мощь ядра сравним времена для 300[Мгц] и 600[Мгц]
    //  Toggle(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT ,GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);


    //ggle_up(GPIO3_INSTANCE_ADDRESS+ GPIO_SETDATAOUT);
    //ggle_down(GPIO3_INSTANCE_ADDRESS+GPIO_CLEARDATAOUT);
    //dtimer3_delay();
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
   // HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    //dtimer3_delay();
    }

#endif

}


/*
static void DMTimerAintcConfigure(void)
{
    IntAINTCInit();
    IntRegister(SYS_INT_TINT3, DMTimerIsr);
    IntPrioritySet(SYS_INT_TINT3, 0, AINTC_HOSTINT_ROUTE_IRQ);
    IntSystemEnable(SYS_INT_TINT3);
}
*/

static void DMTimerSetUp(void)
{
	DMTimerPreScalerClkDisable(SOC_DMTIMER_3_REGS);
    DMTimerReset(SOC_DMTIMER_3_REGS);
    DMTimerCounterSet(SOC_DMTIMER_3_REGS, TIMER_INITIAL_COUNT);
    DMTimerReloadSet(SOC_DMTIMER_3_REGS, TIMER_RLD_COUNT);
    DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
    DMTimerPostedModeConfig(SOC_DMTIMER_3_REGS,DMTIMER_POSTED);
}

/*
static inline void DMTimerIsr(void)
{
   // DMTimerIntStatusClear(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_IT_FLAG);

    if (cnt % 2==0) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);   //один pin GPIO16
    else HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);   //один pin GPIO16
	//HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_16);
    //if (cnt==0) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_SETDATAOUT)     = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);                            //другой pin GPIO15
    //else if (cnt==200000) HWREG(GPIO3_INSTANCE_ADDRESS + GPIO_CLEARDATAOUT)   = (1 << GPIO3_INSTANCE_PIN_NUMBER_17);
    //else if (cnt==400000) cnt=-1;
    cnt++;
}
*/




/**************************************************************************************************
Parameters:         static void DMTimerSetUp(void)
Remarks:            устанавливаем задержку таймера в секундах [1-60]
***************************************************************************************************/
#if 0

static void DMTimerSetUp(void)
{

    /* Load the counter with the initial count value */
   // DMTimerCounterSet(SOC_DMTIMER_3_REGS, TIMER_INITIAL_COUNT);

	  HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) = TIMER_INITIAL_COUNT;

    /* Load the load register with the reload count value */
  //  DMTimerReloadSet(SOC_DMTIMER_3_REGS, TIMER_RLD_COUNT);
	  HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TLDR) = TIMER_RLD_COUNT;

    /* Configure the DMTimer for Auto-reload mode */
  //  DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
	  /*Устанавливаем нужный нам режим работы
	 //AR=1 ;PRE =0;   //всё правильно reload*/
	  HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) = 0x00000002;

}
#endif



#if 0
static inline void dtimer3_delay()
{
        //DMTimerCounterSet(SOC_DMTIMER_3_REGS, 0);
	   HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR)=0;
		//DMTimerEnable(SOC_DMTIMER_3_REGS);
       HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) |= DMTIMER_TCLR_ST;
       //DMTimerDisable(SOC_DMTIMER_3_REGS);
       //HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;


        //while(DMTimerCounterGet(SOC_DMTIMER_3_REGS) < 0x5DC0); //1   [мс]
        //while(DMTimerCounterGet(SOC_DMTIMER_3_REGS) < 0x2EE0);   //500 [мкс]
        //while(DMTimerCounterGet(SOC_DMTIMER_3_REGS) < 0x1770); //250 [мкс]

         //while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x1770); //250 [мкс]
      // while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x00F0); //250 [мкс]

       //while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x0078);
       //HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

     // while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x003C); // 6 мкс
     // HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

       //while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x0014); // 4 мкс
       //HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

       //while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x0008); // 2 мкс
       //HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;


}
#endif


//Пример для разбора функций.
void dmtimer3_2_microsecond (unsigned int microsec)
{
	   HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR)=0;
		//DMTimerEnable(SOC_DMTIMER_3_REGS);
       HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) |= DMTIMER_TCLR_ST;
       while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x0008); // 2 мкс
       HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

}


//Пример по Умолчанию Который Использует Прерывания,оставляем для Тестирования функций
//Пока делаем #IF0
#if 0

#define TIMER_INITIAL_COUNT             (0xFF000000u)
#define TIMER_RLD_COUNT                 (0xFF000000u)

/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
static void DMTimerAintcConfigure(void);
static void DMTimerSetUp(void);
static void DMTimerIsr(void);

/******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;

init_syl_dtimer(void)
{

    /***********************************************************************************************/
	ConsoleUtilsPrintf("_armskd_syl_dtimer_init_OK_\n\r ");
	/* This function will enable clocks for the DMTimer2 instance */
	  DMTimer3ModuleClkConfig();

    /* Initialize the UART console */
    ConsoleUtilsInit();

    /* Select the console type based on compile time check */
    ConsoleUtilsSetType(CONSOLE_UART);

    /* Enable IRQ in CPSR */
    IntMasterIRQEnable();

    /* Register DMTimer2 interrupts on to AINTC */
    DMTimerAintcConfigure();

    /* Perform the necessary configurations for DMTimer */
    DMTimerSetUp();

    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_EN_FLAG);

    ConsoleUtilsPrintf("Tencounter: ");

    /* Start the DMTimer */
    DMTimerEnable(SOC_DMTIMER_3_REGS);

    while(cntValue)
    {
        if(flagIsr == 1)
        {
            ConsoleUtilsPrintf("\b%d",(cntValue - 1));
            cntValue--;
            flagIsr = 0;
        }
    }

    /* Stop the DMTimer */
    DMTimerDisable(SOC_DMTIMER_3_REGS);

    PRINT_STATUS(S_PASS);

    /* Halt the program */
    while(1);
}



/*
** Do the necessary DMTimer configurations on to AINTC.
*/
static void DMTimerAintcConfigure(void)
{
    /* Initialize the ARM interrupt control */
    IntAINTCInit();

    /* Registering DMTimerIsr */
    IntRegister(SYS_INT_TINT3, DMTimerIsr);

    /* Set the priority */
    IntPrioritySet(SYS_INT_TINT3, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_TINT3);
}

/*
** Setup the timer for one-shot and compare mode.
*/
static void DMTimerSetUp(void)
{
    /* Load the counter with the initial count value */
    DMTimerCounterSet(SOC_DMTIMER_3_REGS, TIMER_INITIAL_COUNT);

    /* Load the load register with the reload count value */
    DMTimerReloadSet(SOC_DMTIMER_3_REGS, TIMER_RLD_COUNT);

    /* Configure the DMTimer for Auto-reload and compare mode */
    DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
}

/*
** DMTimer interrupt service routine. This will send a character to serial
** console.
*/
static void DMTimerIsr(void)
{
    /* Disable the DMTimer interrupts */
    DMTimerIntDisable(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_EN_FLAG);

    /* Clear the status of the interrupt flags */
    DMTimerIntStatusClear(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_IT_FLAG);

    flagIsr = 1;

    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_EN_FLAG);
}

#endif











