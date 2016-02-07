/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                               *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_algoritm.c
*
* Description : This Module find and sorting TFA and TYR table from DDR memory
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************/
#include "beaglebone.h"


#include "edma_event.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
#include "interrupt.h"
#include "consoleUtils.h"

#include "string.h"
#include "delay.h"
#include "cache.h"
#include "edma.h"
#include "mmu.h"
#include "consoleUtils.h"



#include "armskd_syl_algoritm.h"
#include "armskd_syl_spi.h"   //функции отправки 26 бит к PPM блоку SPI

/*
#pragma DATA_SECTION(bufferB, "my_sect")
unsigned char bufferB[512];
*/



/*Здесь Должны быть адреса DDR памяти где распологаються наши таблицы */

/*Здесь Должны быть адреса в памяти где будет располагаться входные 22 бита откуда я буду делать выборку
 * для таблиц */


/*Здесь должны быть функции готовности данных видимо пересылку данных между блоками лучше
 * делать через КЭШ памть будет быстрее,чем через ОЗУ*/


/*Пока Затычки там где будем получать таблицы */


void start_poisk_table_algoritm()
{
    unsigned char ppm_Rx_stby=0x1;                //по умолчанию включен на приём Recieve
    unsigned char ppm_Rx_phase_shifting=0x1;      //по умолчания -90[град]
    unsigned char ppm_Rx_attenuation_control=0x1; //по умолчания -16[дБ]




    /*1 канал */

    pm_module_ch1.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
    pm_module_ch1.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
    pm_module_ch1.rx_standy                 = ppm_Rx_stby; // 0    бит
    memcpy(&buf_dest_ppm.buf_dest_ch1,&pm_module_ch1,sizeof(buf_dest_ppm.buf_dest_ch1));


    /*2 канал */
    /*
	pm_module_ch2.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
	pm_module_ch2.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
	pm_module_ch2.rx_standy                 = ppm_Rx_stby; // 0    бит
	memcpy(&buf_dest_ppm.buf_dest_ch2,&pm_module_ch2,sizeof(buf_dest_ppm.buf_dest_ch2));
    */

    /*3 канал */
    /*
	pm_module_ch3.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
	pm_module_ch3.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
	pm_module_ch3.rx_standy                 = ppm_Rx_stby; // 0    бит
	memcpy(&buf_dest_ppm.buf_dest_ch3,&pm_module_ch3,sizeof(buf_dest_ppm.buf_dest_ch3));
    */

    /*4 канал */
	/*
	pm_module_ch4.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
	pm_module_ch4.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
	pm_module_ch4.rx_standy                 = ppm_Rx_stby; // 0    бит
	memcpy(&buf_dest_ppm.buf_dest_ch4,&pm_module_ch4,sizeof(buf_dest_ppm.buf_dest_ch4));
	*/


    while(1)
    {
    send_to_ppm_channel_1(buf_dest_ppm.buf_dest_ch1);
    send_to_ppm_channel_2(0x0B0B0B0B);
    send_to_ppm_channel_3(0x0C0C0C0C);
	send_to_ppm_channel_4(0x0F0F0F0F);
    }
    //send_to_ppm_channel_2(0x0B0B0B0B);
    //send_to_ppm_channel_3(0x0C0C0C0C);
	//send_to_ppm_channel_4(0x0F0F0F0F);


}













