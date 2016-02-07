/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_copy_to_ddr.h
*
* Description : this copy tfa table and tyr table  from eMMC flash to DDR
*
* Author      : Konstantin Shiluaev
*
******************************************************************************/
#ifndef ARMSKDSYLCOPYTODDR_H
#define ARMSKDSYLCOPYTODDR_H


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

void read_from_emmc_to_DDR();



#endif /* ABARMFIFODRV_H */
