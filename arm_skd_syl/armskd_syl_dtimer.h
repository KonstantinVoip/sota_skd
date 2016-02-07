/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                            *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_dtimer.c
*
* Description :
*
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

#ifndef ARMSKDSYLDTIMER_H
#define ARMSKDSYLDTIMER_H

//Функции инициализации таймеров
int init_syl_dtimer();

extern inline void dmtimer3_2_microsecond (unsigned int microsec);

//тут должны быть задержки для нашего таймера
/*
extern inline dmtimer3_1_microsecond (unsigned int microsec);
extern inline dmtimer3_2_microsecond (unsigned int microsec)
extern inline dmtimer3_4_microsecond (unsigned int microsec)
extern inline dmtimer3_6_microsecond (unsigned int microsec)
extern inline dmtimer3_8microsecond (unsigned int microsec)
extern inline dmtimer3_10_microsecond (unsigned int microsec)
*/

/*
extern inline dmtimer3_del_nanosecond  (unsigned int nanosec);
extern inline dmtimer3_del_microsecond (unsigned int microsec);
extern inline dmtimer3_del_millisecond (unsigned int millisec);
extern inline dmtimer3_del_second      (unsigned int second);
*/

/*Интерфейс для работы с PERFOMANCE TIMER используеться DTIMER7 */

extern inline void         dmtimer7_SysPerfTimerSetup(void);
extern inline void         dmtimer7_PerfTimerStart(unsigned short);          //запускаем таймер
extern inline unsigned int dmtimer7_PerfTimerStop(unsigned short);           //возвращаем количество тиков


#endif /* ARMSKDSYLDTIMER_H */
