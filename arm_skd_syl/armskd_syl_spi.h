/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                               *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_spi.h
*
* Description : SPI Interface to PPM module header file
*
* Author      : Konstantin Shiluaev
*
******************************************************************************/
#ifndef ARMSKDSYLSPI_H
#define ARMSKDSYLSPI_H



//Определяем тип блока который используем соединены.

#define TYPE_MODULE_PM           1
//#define TYPE_MODULE_PPM        1








void gpio_spi_interface_to_ppm_block();



struct ppm_txrx_data {
unsigned  tx_standy 				:1 ;
unsigned  rx_attenuation_control    :6 ;
unsigned  tx_attenuation_control    :6 ;
unsigned  rx_phase_shifting_control :6 ;
unsigned  tx_phase_shifting_control :6 ;
unsigned  rx_standy 				:1 ;
}ppm_module_ch1,ppm_module_ch2,ppm_module_ch3,ppm_module_ch4;    //4 канал для ППМ модуля.

/////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pm_rx_data {
unsigned  rx_standy 				:1 ;
unsigned  rx_phase_shifting_control :6 ;
unsigned  rx_attenuation_control    :6 ;
}pm_module_ch1,pm_module_ch2,pm_module_ch3,pm_module_ch4;




struct buf_dest_ppm
{
unsigned int buf_dest_ch1;
unsigned int buf_dest_ch2;
unsigned int buf_dest_ch3;
unsigned int buf_dest_ch4;
}buf_dest_ppm;
















//Внешние функции которые должны в каждый отдельный канал передавать данные.
//Шлём данные в ППМ блок
extern unsigned short  send_to_ppm_channel_1(unsigned int data_26bit_to_channel1);
extern unsigned short  send_to_ppm_channel_2(unsigned int data_26bit_to_channel2);
extern unsigned short  send_to_ppm_channel_3(unsigned int data_26bit_to_channel3);
extern unsigned short  send_to_ppm_channel_4(unsigned int data_26bit_to_channel4);











#endif /* ABARMFIFODRV_H */
