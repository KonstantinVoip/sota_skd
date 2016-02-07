/*
 * arm_skd_syl.cmd
 *
 * Linker command file.
 *
 * Copyright (C) ZAO NPK_PELENGATOR Konstantin Shiluaev 2015
 *
*/

/****************************************************************************/
//-stack  0x1000                             /* SOFTWARE STACK SIZE           */

-stack  0x0008
-heap   0x2000                             /* HEAP AREA SIZE                */
-e Entry

/* Since we used 'Entry' as the entry-point symbol the compiler issues a    */
/* warning (#10063-D: entry-point symbol other than "_c_int00" specified:   */
/* "Entry"). The CCS Version (5.1.0.08000) stops building from command      */
/* line when there is a warning. So this warning is suppressed with the     */
/* below flag. */



//�������� ��������� ������ ������ ��� PRU ����������

//#define PRU_ADD 1


--diag_suppress=10063
//--retain=pru0_skd_syl_image.obj(*)
//--retain=pru1_skd_syl_image.obj(*)

//--retain=(*)pru1_skd_syl_image.obj

//--retain=pru1_skd_syl_image.obj(.text)
//--retain=pru1_skd_syl_image.obj(.data)
//--retain=pru1_skd_syl_image.obj(.strtab)

//rts32eabi.lib<printf.obj>(.text)
/* SPECIFY THE SYSTEM MEMORY MAP */
//--retain=PRU_LED1_image.obj(*)


#ifdef PRU_ADD

--retain=pru1_skd_syl_image.obj(*)

#endif


/*
MEMORY
{
        //������ ��������
        DDR_MEM_0        : org = 0x80000000  len = 0x10000000                    // 256 Mb RAM
        //����� ����� ������ ���� ������� ��� ��� � ���
        DDR_MEM_1        : org = 0x90000000  len = 0x10000000                    // 256 Mb RAM
        //������ � ���
        SRAM		   : org = 0x402F0400    len = 0x0000FC00           // 64kB internal SRAM very fast
        L3OCMC0		   : org = 0x40300000    len = 0x00010000           // 64kB L3 OCMC SRAM  L3 interconnect
}
*/


























/*���� ���� ��������� ������ ������������ ��� ������ ��� ������ ��� � ����  */


MEMORY
{
        //������ ��������
        DDR_MEM_0        : org = 0x80000000  len = 0x00040000                    // 128 K/bait
        //����� ����� ������ ��� ����� ����������.CONST
        DDR_MEM_1        : org = 0x80040000  len = 0x00040000                    // 128 K/bait
        //����� ����� ������ ������� � ������� 64 �/����
        PPM_TAB1_DDR     : org = 0x81000000  len = 0x01000000                 	//������� 1  ������� ������� 16 �/����
        PPM_TAB2_DDR     : org = 0x82000000  len = 0x01000000                 	//������� 2  ������� ������� 16 �/����
        PPM_TAB3_DDR     : org = 0x83000000  len = 0x01000000                 	//������� 3  ������� ������� 16 �/����
        PPM_TAB4_DDR     : org = 0x84000000  len = 0x01000000                 	//������� 4  ������� ������� 16 �/����
       //������ � ��� ���� ������������ � �������
       //SRAM		     : org = 0x402F0400    len = 0x0000FC00           // 64kB internal SRAM very fast
       //L3OCMC0		 : org = 0x40300000    len = 0x00010000           // 64kB L3 OCMC SRAM  L3 interconnect
}


SECTIONS
{
    .text:Entry : load > 0x80000000
    .text    : load >  DDR_MEM_0 ///����������� ����������� ���
    .data    : load >  DDR_MEM_0 // initialized data
    .bss     : load >  DDR_MEM_0 //���������� � ����������� ���������� ���������������������
     RUN_START(bss_start)
     RUN_END(bss_end)
    .cinit   : load >  DDR_MEM_0  //��� ������������� ���������� � ����������� ����������
    .switch  : load >  DDR_MEM_0  // switch table info
    .sysmem  : load >  DDR_MEM_0  //������������ ������ (����) such as malloc, calloc, realloc, or new.
    //.stack   : load >  DDR_MEM_0  //������� �����
   .stack   : load > DDR_MEM_1 HIGH  // SOFTWARE SYSTEM STACK


    .const   : load >  DDR_MEM_0  //���������� � ����������� ��������� ���������������������
    .cio     : load >  DDR_MEM_0  // C I/O
    //����� ��������� ��� ��� PRU CAPE ������
   // pru1_skd_syl_text: {PRU_LED1_image.obj(.text)} load > DDR_MEM_1 run_start(PRU1_SKDSYL_INST)
   // pru1_skd_syl_data: {PRU_LED1_image.obj(.data)} load > DDR_MEM_1 run_start(PRU1_SKDSYL_DATA)

   //pru0_skd_syl_text: {pru0_skd_syl_image.obj(.text)} load > DDR_MEM_1 run_start(PRU0_SKDSYL_INST)
   //pru0_skd_syl_data: {pru1_skd_syl_image.obj(.data)} load > DDR_MEM_1 run_start(PRU0_SKDSYL_DATA)
   #ifdef PRU_ADD
   pru1_skd_syl_text: {pru1_skd_syl_image.obj(.text)}  load  > DDR_MEM_1 run_start(PRU1_SKDSYL_INST)
   pru1_skd_syl_data: {pru1_skd_syl_image.obj(.data)}  load  > DDR_MEM_1 run_start(PRU1_SKDSYL_DATA)
   #endif
   //��������� ������� ��� ������� ������.
    //��������� ������ ����� ����������� ������� ��� �������� application
    tab1  : load > PPM_TAB1_DDR  //1 -� 16 �/����
    tab2  : load > PPM_TAB2_DDR  //2 -� 16 �/����
    tab3  : load > PPM_TAB3_DDR  //3 -� 16 �/����
    tab4  : load > PPM_TAB4_DDR  //4 -� 16 �/����
}


/*
// SPECIFY THE SECTIONS ALLOCATION INTO MEMORY

SECTIONS
{
    .text:Entry : load > 0x80000000

    .text    : load > DDR_MEM              // CODE
    .data    : load > DDR_MEM              // INITIALIZED GLOBAL AND STATIC VARIABLES
    .bss     : load > DDR_MEM              // UNINITIALIZED OR ZERO INITIALIZED
                                           // GLOBAL & STATIC VARIABLES
                    					   // RUN_START(bss_start)
                   						   // RUN_END(bss_end)
    .const   : load > DDR_MEM              // GLOBAL CONSTANTS
    .cinit	 : load > DDR_MEM
    .stack   : load > DDR_MEM HIGH //0x87FFF000           // SOFTWARE SYSTEM STACK
 // LED0_text: {PRU_LED0_image.obj(.text)} load > DDR_MEM run_start(LED0_INST)
 // LED0_data: {PRU_LED0_image.obj(.data)} load > DDR_MEM run_start(LED0_DATA)
 // LED1_text: {PRU_LED1_image.obj(.text)} load > DDR_MEM run_start(LED1_INST)
 // LED1_data: {PRU_LED1_image.obj(.data)} load > DDR_MEM run_start(LED1_DATA)
 // SW_text: {PRU_Switch_image.obj(.text)} load > DDR_MEM run_start(SW_INST)
 // SW_data: {PRU_Switch_image.obj(.data)} load > DDR_MEM run_start(SW_DATA)
 // AUDIO_text: {PRU_Audio_image.obj(.text)} load > DDR_MEM run_start(AUDIO_INST)
 // AUDIO_data: {PRU_Audio_image.obj(.data)} load > DDR_MEM run_start(AUDIO_DATA)
 // HW_UART_text: {PRU_Hardware_UART_image.obj(.text)} load > DDR_MEM run_start(UART_INST)
//	HW_UART_data: {PRU_Hardware_UART_image.obj(.data)} load > DDR_MEM run_start(UART_DATA)
//	TEMPSENSOR0_text: {PRU_HDQ_TempSensor0_image.obj(.text)} load > DDR_MEM run_start(SLAVE_INST)
//  TEMPSENSOR0_data: {PRU_HDQ_TempSensor0_image.obj(.data)} load > DDR_MEM run_start(SLAVE_DATA)
//  TEMPSENSOR1_text: {PRU_HDQ_TempSensor1_image.obj(.text)} load > DDR_MEM run_start(MASTER_INST)
//  TEMPSENSOR1_data: {PRU_HDQ_TempSensor1_image.obj(.data)} load > DDR_MEM run_start(MASTER_DATA)
}

*/

