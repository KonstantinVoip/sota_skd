/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_fifodrv.h
*
* Description : �������� ������� ������ ������ ����
*               1 ����� ->������ �������� ������ ������ ���.
*               2 ����� ->���������� ��������� � ������� UART ������
* Author      : Konstantin Shiluaev
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source: 
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2014/09/21 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrv.h $
* --------- Initial revision
******************************************************************************/
#ifndef ARMSKDSYL_MAIN_H_
#define ARMSKDSYL_MAIN_H_



//#include "hw_types.h"
//#include "soc_AM335x.h"
//#include "hw_control_AM335x.h"
//#include "hw_cm_per.h"
//#include "hw_prm_per.h"
//#include "consoleUtils.h"
//#include "uartStdio.h"




/*������  PRU0    ->4�/����*/
//extern const unsigned char PRU0_SKDSYL_DATA[0x1000];
/*���������� PRU0 ->4�/����*/
//extern const unsigned char PRU0_SKDSYL_INST[0x1000];

/*������ PRU1     ->4�/����*/
extern const unsigned char PRU1_SKDSYL_DATA[0x1000];
/*���������� PRU1 ->4�/���� */
extern const unsigned char PRU1_SKDSYL_INST[0x1000];

//�������� �������
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

/* ��������.
			unsigned int buf_from=0x11223344;
			unsigned int buf_dest=0x00000000;
			memcpy(&buf_dest,&buf_from,sizeof(buf_dest));
*/


    //��������.
    /*
	asm("    mov   r0, #3\n\t"
        "    mov   r1, #2\n\t"
        "    add   r2, r1, r0"
    		);
   */



  //�������� 32 ���
  /*
	 //��������� 32 ����
		   asm("    movw   r2, #8000\n\t"  //������� ������ 0-15  ���
	    	   "    movt   r2, #0001\n\t"  //������� ������ 16-32 ���
		   );
  */


	//�������� ��� ������ L1_data ,L1_instruction and L2 Uni fied cache
    //CacheEnable(CACHE_ALL);

	//�������� ��������� ��� �� ���������� ��������� � �������
    /*Enabling functional clocks for GPIO1 instance. */
#if 0
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
#endif


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
    //������������� SPI ���� ���������.
    //������ �������� � SPI ����� �������� � PPM ������ ����� GPIO ����
   // gpio_spi_interface_to_ppm_block();
    //������ �������� � �������������� I2C
    //  init_syl_i2c_bus();

    //test_data_types();
    //����� ������ ���� :���� ��� �����
    //ConsoleUtilsPrintf("appl:Type_of_block=%s+\n\r",REVISION_APPLICATION);
    //����� �� ��� ��� ������ � ���������� ����� �����.                                     //��������� ������ � ���� I2C <->���������� � ����
    //ConsoleUtilsPrintf("appl:Type=%s,number_of_block=%d\n\r",REVISION_APPLICATION);

    //read_from_emmc_to_DDR();   //������ � ����������� ������� �� eMMC � DDR ������.
    //����� ������                                                              hfkk            //�������� ������ �� eMMC ������ � DDR
    //ConsoleUtilsPrintf("appl:tfa_tab=[%d][%d][%d][%d],tyr_tab=[%d][%d][%d][%d]\n\r");

    /*�������� ���� ��� ������������� �������� ����������������� ����� ����. */


    //������� ������ ������[TYR/TFA] � eMMC ������ � DDR ������.
    //����� ������ ���������� ��������� ��� �������
    //1.
    // read_from_emmc_to_DDR();   //������ � ����������� ������� �� eMMC � DDR ������.

     //�������� ������� ����� ������ �� XMODEM RECIEVE
     //test_main_xmodem_recieve_functions();


    //������������� ���� ��� ����� ����������� ������ PPM ������
	//init_clock_pll_mpu_and_config();  //clock and pll
    //������ �� ���������
    //reg=(HWREG(SOC_DMTIMER_2_REGS + 0x0));
    //syl_Sysdelay(50);
    //������ � ��������
    //HWREG(GPIO_INSTANCE_ADDRESS + GPIO_SETDATAOUT)   = (1 << GPIO_INSTANCE_PIN_NUMBER);
   //test_functions();
   //init_syl_dtimer();
    //init_syl_i2c_bus();
	  // init_syl_gpio();
	 // init_syl_spi_bus();
	//sets pin mux for PRU cape






















/*****************************************************************************
**             Function definitions & Global variables
*****************************************************************************/
//void PRUCapePinmux(void);

//void PRU_skd_syl_test(void);
//void MainMenu(void);
//int LEDTest(void);
//int SwitchTest(void);
//int AudioTest(void);
//int HDQTest(void);
//int UARTTest(void);
//void UARTInit(void);
//void PRUCapePinmux(void);

#endif
