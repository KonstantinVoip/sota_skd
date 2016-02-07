/****************************************************************************/
/*  AM335x_PRU.cmd                                                          */
/*  Copyright (c) 2014  Texas Instruments Incorporated                      */
/*                                                                          */
/*    Description: This file is a linker command file that can be used for  */
/*                 linking PRU programs built with the C compiler and       */
/*                 the resulting .out file on an AM335x device.             */
/****************************************************************************/

-cr // LINK USING C CONVENTIONS
-stack 0x100 // SOFTWARE STACK SIZE
-heap  0x100 // HEAP AREA SIZE
//--args 0x100
// SPECIFY THE SYSTEM MEMORY MAP


//������������� ����� PAGE 0: all executable sections should be allocated on page 0
//������������� ����� PAGE 1: All data sections should be allocated on page 1


MEMORY
{
	PAGE 0:
	P_MEM    : org =  0x00000000 len = 0x00002000   //Instruction_ PRU_1  MEM 8 KB
	PAGE 1:
	NEAR_MEM : org =  0x00000000 len = 0x00002000   //DATA_ PRU_1  MEM 8 KB
/*	FAR_MEM  : org =  0x00010000 len = 0x00003000 */  //DATA_ Shared MEM 12 KB
}


// SPECIFY THE SECTIONS ALLOCATION INTO MEMORY
SECTIONS
{
.text:_c_int00*	>  0x0, PAGE 0

/*
.bss :     {} > NEAR_MEM, PAGE 1
.data :    {} > NEAR_MEM, PAGE 1 palign=2
.rodata :  {} > NEAR_MEM, PAGE 1

.farbss 	: {} > FAR_MEM, PAGE  1
.fardata    : {} > FAR_MEM, PAGE  1                        //pruhv7a p.100  Sections
.rofardata  : {} > FAR_MEM, PAGE  1
.sysmem 	: {} > FAR_MEM, PAGE  1
.stack 		: {} > FAR_MEM, PAGE  1
.init_array : {} > FAR_MEM, PAGE  1
.cinit 		: {} > FAR_MEM, PAGE  1
.args 		: {} > NEAR_MEM,PAGE  1
*/
.data :  {} > NEAR_MEM, PAGE  1 palign=2
.stack : {} > NEAR_MEM, PAGE  1
.text  : {} > P_MEM, PAGE 0    //������ ����������� ���
}








//���������� ������� ����������� ����
/*
MEMORY
{
	PAGE 0:
	P_MEM    : org =  0x00000000 len = 0x00002000   //Instruction_ PRU_1  MEM 8 KB
	PAGE 1:
	NEAR_MEM : org =  0x00000000 len = 0x00002000   //DATA_ PRU_1  MEM 8 KB
	FAR_MEM  : org =  0x00010000 len = 0x00003000   //DATA_ Shared MEM 12 KB
}


// SPECIFY THE SECTIONS ALLOCATION INTO MEMORY
SECTIONS
{
.bss :     {} > NEAR_MEM, PAGE 1
.data :    {} > NEAR_MEM, PAGE 1 palign=2
.rodata :  {} > NEAR_MEM, PAGE 1

.farbss 	: {} > FAR_MEM, PAGE  1
.fardata    : {} > FAR_MEM, PAGE  1                        //pruhv7a p.100  Sections
.rofardata  : {} > FAR_MEM, PAGE  1
.sysmem 	: {} > FAR_MEM, PAGE  1
.stack 		: {} > FAR_MEM, PAGE  1
.init_array : {} > FAR_MEM, PAGE  1
.cinit 		: {} > FAR_MEM, PAGE  1
.args 		: {} > NEAR_MEM,PAGE 1
.text : {} > P_MEM, PAGE 0    //������ ����������� ���
}


*/











//��� ���� ������� ���������� ������
/*
MEMORY
{
	PAGE 0:
	//P_MEM : org = 	 0x00000008 len = 0x0003FFF8
	P_MEM : org = 	 0x00000000 len = 0x00002000
	PAGE 1:
	NEAR_MEM : org = 0x00000008 len = 0x0000FFF8
	FAR_MEM : org =  0x00010000 len = 0x80000000
}
// SPECIFY THE SECTIONS ALLOCATION INTO MEMORY
SECTIONS
{
.bss :     {} > NEAR_MEM, PAGE 1
.data :    {} > NEAR_MEM, PAGE 1 palign=2
.rodata :  {} > NEAR_MEM, PAGE 1
.farbss :  {} > FAR_MEM, PAGE 1
.fardata : {} > FAR_MEM, PAGE 1
.rofardata : {} > FAR_MEM, PAGE 1
// In far memory for validation purposes
.sysmem : {} > FAR_MEM, PAGE 1
.stack : {} > FAR_MEM, PAGE 1
.init_array : {} > FAR_MEM, PAGE 1
.cinit : {} > FAR_MEM, PAGE 1
.args : {} > NEAR_MEM, PAGE 1
.text : {} > P_MEM, PAGE 0
}
*/
















/*
-cr								// Link using C conventions

// Specify the System Memory Map
MEMORY
{
      PAGE 0:
	PRU_IMEM		: org = 0x00000000 len = 0x00002000  // 8kB PRU0 Instruction RAM

      PAGE 1:

	// RAM

	PRU_DMEM_0_1	: org = 0x00000000 len = 0x00002000 CREGISTER=24 // 8kB PRU Data RAM 0_1
	PRU_DMEM_1_0	: org = 0x00002000 len = 0x00002000	CREGISTER=25 // 8kB PRU Data RAM 1_0
	PRU_SHAREDMEM	: org = 0x00010000 len = 0x00003000 CREGISTER=28 // 12kB Shared RAM

	DDR			    : org = 0x80000000 len = 0x00000100	CREGISTER=31
	L3OCMC			: org = 0x40000000 len = 0x00010000	CREGISTER=30


	// Peripherals

	PRU_CFG			: org = 0x00026000 len = 0x00000100	CREGISTER=4
	PRU_ECAP		: org = 0x00030000 len = 0x00000100	CREGISTER=3
	PRU_IEP			: org = 0x0002E000 len = 0x0000031C	CREGISTER=26
	PRU_INTC		: org = 0x00020000 len = 0x00001504	CREGISTER=0
	PRU_UART		: org = 0x00028000 len = 0x00000100	CREGISTER=7

	DCAN0			: org = 0x481CC000 len = 0x00000100	CREGISTER=14
	DCAN1			: org = 0x481D0000 len = 0x00000100	CREGISTER=15
	DMTIMER2		: org = 0x48040000 len = 0x00000100	CREGISTER=1
	EHRPWM1			: org = 0x48300000 len = 0x00000100	CREGISTER=18
	EHRPWM2			: org = 0x48302000 len = 0x00000100	CREGISTER=19
	EHRPWM3			: org = 0x48304000 len = 0x00000100	CREGISTER=20
	GEMAC			: org = 0x4A100000 len = 0x00000100	CREGISTER=9
	I2C1			: org = 0x4802A000 len = 0x00000100	CREGISTER=2
	I2C2			: org = 0x4819C000 len = 0x00000100	CREGISTER=17
	MBX0			: org = 0x480C8000 len = 0x00000100	CREGISTER=22
	MCASP0_DMA		: org = 0x46000000 len = 0x00000100	CREGISTER=8
	MCSPI0			: org = 0x48030000 len = 0x00000100	CREGISTER=6
	MCSPI1			: org = 0x481A0000 len = 0x00000100	CREGISTER=16
	MDIO			: org = 0x00032400 len = 0x00000100	CREGISTER=21
	MMCHS0			: org = 0x48060000 len = 0x00000100	CREGISTER=5
	SPINLOCK		: org = 0x480CA000 len = 0x00000100	CREGISTER=23
	TPCC			: org = 0x49000000 len = 0x000010A0	CREGISTER=29
	UART1			: org = 0x48022000 len = 0x00000100	CREGISTER=11
	UART2			: org = 0x48024000 len = 0x00000100	CREGISTER=12

	RSVD10			: org = 0x48318000 len = 0x00000100	CREGISTER=10
	RSVD13			: org = 0x48310000 len = 0x00000100	CREGISTER=13
	RSVD27			: org = 0x00032000 len = 0x00000100	CREGISTER=27

}

// Specify the sections allocation into memory
SECTIONS {
	// Forces _c_int00 to the start of PRU IRAM. Not necessary when loading
	//   an ELF file, but useful when loading a binary
	.text:_c_int00*	>  0x0, PAGE 0

	.text		>  PRU_IMEM, PAGE 0
	.stack		>  PRU_DMEM_0_1, PAGE 1
	.bss		>  PRU_DMEM_0_1, PAGE 1
	.cio		>  PRU_DMEM_0_1, PAGE 1
	.data		>  PRU_DMEM_0_1, PAGE 1
	.switch		>  PRU_DMEM_0_1, PAGE 1
	.sysmem		>  PRU_DMEM_0_1, PAGE 1
	.cinit		>  PRU_DMEM_0_1, PAGE 1
	.rodata		>  PRU_DMEM_0_1, PAGE 1
	.rofardata	>  PRU_DMEM_0_1, PAGE 1
	.farbss		>  PRU_DMEM_0_1, PAGE 1
	.fardata	>  PRU_DMEM_0_1, PAGE 1
	.resource_table > PRU_DMEM_0_1, PAGE 1
}
*/
