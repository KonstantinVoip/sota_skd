/**********************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                  *
*                                            All rights reserved.                            *
**********************************************************************************************
* Module      : arm_skd_syl_copy_to_ddr.c
*
* Description : This Module COPY TFA and TYR table from eMMC flash to DDR memory
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************************/
#include "armskd_syl_copy_to_ddr.h"
#include "syl_mmcsd_proto.h"

/*********************************************
 * ��������������� ��������� ������
 * *  ------------------------------------------------------------------------------------------------------
*                              mem        (32768)       1 ���� = 512 ���� *32768 = 16 772 16 (16 �/����)
*   tab_1   				0x01000000  blk=0x8000 ->    tab1_������ �������            16 �/����
*                                         (65536)
*                                       blk=0x10000      ����� �������_1
*   -----------------------------------------------------------------------------------------------
*   tab_2 								  				-tab2_������ �������            16 �/����
*	                                      (65536)
*									    blk=0x10000  	 tab2_������ �������            16 �/����
*                                         (98304)
*                                       blk=0x18000
*   ----------------------------------------------------------------------------------------------
*   tab_3												-tab3_������ �������            16 �/����
*                                         (98304)
*                                       blk=0x18000
*
                                         (131072)
*                                       blk=0x20000
*   ----------------------------------------------------------------------------------------------
*   tab_4												-tab4_�������� �������         16 �/����
*                                         (131072)
*                                       blk=0x20000
*
*                                         (163840)
*                                       blk=0x28000
*
*/
 //��������� ������� ��� ������� ������.
#pragma DATA_SECTION(buf_tab_1, "tab1")
unsigned char  buf_tab_1[16777216];           //16 �/����

#pragma DATA_SECTION(buf_tab_2, "tab2")       //16 �/����
unsigned char  buf_tab_2[16777216];

#pragma DATA_SECTION(buf_tab_3, "tab3")       //16 �/����
unsigned char  buf_tab_3[16777216];

#pragma DATA_SECTION(buf_tab_4, "tab4")       //16 �/����
unsigned char  buf_tab_4[16777216];



/*�������� ������ ������ ���������� ������� */
unsigned int ppm_tab1_blk  =   0x8000;       //start block address
unsigned int ppm_tab1_cnt  =   0x8000;       ////32768 blocks  num of block size 512 bait  * 0x8000(32768)  =   = 16777216 �/����

unsigned int ppm_tab2_blk  =   0x10000;
unsigned int ppm_tab2_cnt  =   0x8000;       ////32768 blocks  num of block size 512 bait  * 0x8000(32768)  =   = 16777216 �/����

unsigned int ppm_tab3_blk  =   0x18000;
unsigned int ppm_tab3_cnt  =   0x8000;       ////32768 blocks  num of block size 512 bait  * 0x8000(32768)  =   = 16777216 �/����

unsigned int ppm_tab4_blk  =   0x20000;
unsigned int ppm_tab4_cnt  =   0x8000;       ////32768 blocks  num of block size 512 bait  * 0x8000(32768)  =   = 16777216 �/����


void read_from_emmc_to_DDR()
{

    struct mmc *mmc_emmc=0;
    unsigned int dev_mmc=1;   //dev =1 eMMC card
    unsigned int n=0;

	/*������ ������ ���������� APP(app) � ����� */  /*������ � ������� ������� 64 �/���� ������ */
 	unsigned int blk=0x1;                            //����� ���������� � 0x400 (1024) �����  524288[����] = 512 [�/����]
 	unsigned int cnt=0x8000;                         //������ ���������� 256 �/���� ������� ������ ������ ������ ����������.


    //�������������� EMMC ������
    mmc_initialize(dev_mmc);
    mmc_emmc=find_mmc_device(dev_mmc);
    mmc_init( mmc_emmc);


    //������ ������� 1 ��������� � DDR ������
    n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt,buf_tab_1);

    //������ ������� 2 ��������� � DDR ������
    n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt,buf_tab_2);

    //������ ������� 3 ��������� � DDR ������
    n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt,buf_tab_3);

    //������ ������� 4 ��������� � DDR ������
    n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt,buf_tab_4);



/*
	//������ ������� � eMMC ������
    //////////////////////////////////////////////READ from eMMC flash
    printf("\eMMC read: dev # %d, block # %d, count %d ... ",dev_mmc, blk, cnt);
    //n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt, 0x90000000);
    n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt,buf_tab_1);
    printf("%d blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");

*/


}
