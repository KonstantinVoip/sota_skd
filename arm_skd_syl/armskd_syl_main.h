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
