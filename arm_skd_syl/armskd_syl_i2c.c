/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                     *
*                                            All rights reserved.                                                *
***********************************************************************************************************************
* Module      : armskdsyl_i2c.c
*
* Description : I2C bus interface to FPGA PLIS
*
* Author      : Konstantin Shiluaev.
*
*
* Через Шину I2C мы определяем порядковый номер СОТЫ от 1 до 48
* Ориентация соты с Лева направо как в документации нужно здесь будет сделать
* стык СОТА <-> ПЛИС в будущем. и определить у нас модуль только ПМ(приём)
* или  ППМ (приём-передача).ПЛАТА СОТА.
*
******************************************************************************/
#include "soc_AM335x.h"
#include "hsi2c.h"
#include "interrupt.h"
#include "beaglebone.h"
#include "clock.h"
#include "consoleUtils.h"


//device который висит на шине I2C упаравляет как раз Питанием
#include "hw_tps65217.h"

#define I2C_INSTANCE	0x03
#define I2C_INTERRUPT_FLAG_TO_CLR         (0x7FF)


/*	I2C instance	*/
#define  I2C_0					         	(0x0u)
#define  I2C_1					         	(0x1u)
#define  I2C_2					         	(0x2u)


/*	System clock fed to I2C module - 48Mhz	*/
#define  I2C_SYSTEM_CLOCK					(48000000u)
/*	Internal clock used by I2C module - 12Mhz	*/
#define  I2C_INTERNAL_CLOCK					(12000000u)
/*	I2C bus speed or frequency - 100Khz	*/
#define	 I2C_OUTPUT_CLOCK					(100000u)


static  unsigned char syl_dataFromSlave[2];
static  unsigned char syl_dataToSlave[3];
static  unsigned int  syl_tCount;
static  unsigned int  syl_rCount;



//static void syl_TPS65217RegRead(unsigned char regOffset, unsigned char* dest);
static void syl_TPS65217RegWrite(unsigned char port_level, unsigned char regOffset,unsigned char dest_val, unsigned char mask);


static void syl_CleanupInterrupts(void);
static void syl_SetupReception  (unsigned int syl_dcount);
static void syl_SetupI2CTransmit(unsigned int syl_dcount);



int init_syl_i2c_bus()
{
    I2C0ModuleClkConfig();

    I2CPinMuxSetup(I2C_0);

    /* Put i2c in reset/disabled state */
    I2CMasterDisable(SOC_I2C_0_REGS);

    I2CSoftReset(SOC_I2C_0_REGS);

    /* Disable auto Idle functionality */
    I2CAutoIdleDisable(SOC_I2C_0_REGS);

    /* Configure i2c bus speed to 100khz */
    I2CMasterInitExpClk(SOC_I2C_0_REGS, I2C_SYSTEM_CLOCK, I2C_INTERNAL_CLOCK,I2C_OUTPUT_CLOCK);
    I2CMasterEnable(SOC_I2C_0_REGS);

    while(!I2CSystemStatusGet(SOC_I2C_0_REGS));



    //Устанавливаем SLAVE адрес на шине
    /* Configure PMIC slave address */
    I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, PMIC_TPS65217_I2C_SLAVE_ADDR);



 return 1;
}



/*
 * \brief Clear the status of all interrupts
 *
 * \param  none.
 *
 * \return none
 */

void syl_CleanupInterrupts(void)
{
    I2CMasterIntClearEx(SOC_I2C_0_REGS,  I2C_INTERRUPT_FLAG_TO_CLR);
}




/*
 * \brief Receives data over I2C0 bus
 *
 * \param  dcount - Number of bytes to receive.
 *
 * \return none
 */

void syl_SetupReception(unsigned int syl_dcount)
{
    I2CSetDataCount(SOC_I2C_0_REGS, 1);

    syl_CleanupInterrupts();

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX);

    I2CMasterStart(SOC_I2C_0_REGS);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0);

    I2CMasterDataPut(SOC_I2C_0_REGS, syl_dataToSlave[syl_tCount]);

    I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY);

    while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_ADRR_READY_ACESS));

    I2CSetDataCount(SOC_I2C_0_REGS, syl_dcount);

    syl_CleanupInterrupts();

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_RX);

    I2CMasterStart(SOC_I2C_0_REGS);

    /* Wait till the bus if free */
    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0);

    /* Read the data from slave of dcount */
    while((syl_dcount--))
    {
        while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_RECV_READY));

        syl_dataFromSlave[syl_rCount++] = I2CMasterDataGet(SOC_I2C_0_REGS);

        I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_RECV_READY);
    }

    I2CMasterStop(SOC_I2C_0_REGS);

    while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_STOP_CONDITION));

    I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_STOP_CONDITION);
}

/*
 * \brief Transmits data over I2C0 bus
 *
 * \param  none
 *
 * \return none
 */
void syl_SetupI2CTransmit(unsigned int syl_dcount)
{
    I2CSetDataCount(SOC_I2C_0_REGS, syl_dcount);

    syl_CleanupInterrupts();

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX);

    I2CMasterStart(SOC_I2C_0_REGS);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0);

    while((I2C_INT_TRANSMIT_READY == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_TRANSMIT_READY)) && syl_dcount--)
    {
        I2CMasterDataPut(SOC_I2C_0_REGS, syl_dataToSlave[syl_tCount++]);

        I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY);
    }

    I2CMasterStop(SOC_I2C_0_REGS);

    while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_STOP_CONDITION));

    I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_STOP_CONDITION);

}






/*
void syl_TPS65217RegRead(unsigned char regOffset, unsigned char* dest)
{
    syl_dataToSlave[0] = regOffset;
    syl_tCount = 0;

    syl_SetupReception(1);

    *dest = syl_dataFromSlave[0];
}
*/

/**
 *  \brief            - Generic function that can write a TPS65217 PMIC
 *                      register or bit field regardless of protection
 *                      level.
 *
 * \param prot_level  - Register password protection.
 *                      use PROT_LEVEL_NONE, PROT_LEVEL_1, or PROT_LEVEL_2
 * \param regOffset:  - Register address to write.
 *
 * \param dest_val    - Value to write.
 *
 * \param mask        - Bit mask (8 bits) to be applied.  Function will only
 *                      change bits that are set in the bit mask.
 *
 * \return:            None.
 */

/*
void syl_TPS65217RegWrite(unsigned char port_level, unsigned char regOffset,unsigned char dest_val, unsigned char mask)
{
    unsigned char read_val;
    unsigned xor_reg;

    syl_dataToSlave[0] = regOffset;
    syl_tCount = 0;
    syl_rCount = 0;

    if(mask != MASK_ALL_BITS)
    {
    	syl_SetupReception(1);

         read_val = syl_dataFromSlave[0];
         read_val &= (~mask);
         read_val |= (dest_val & mask);
         dest_val = read_val;
    }

    if(port_level > 0)
    {
         xor_reg = regOffset ^ PASSWORD_UNLOCK;

         syl_dataToSlave[0] = PASSWORD;
         syl_dataToSlave[1] = xor_reg;
         syl_tCount = 0;

         syl_SetupI2CTransmit(2);
    }

    syl_dataToSlave[0] = regOffset;
    syl_dataToSlave[1] = dest_val;
    syl_tCount = 0;

    syl_SetupI2CTransmit(2);

    if(port_level == PROT_LEVEL_2)
    {
         syl_dataToSlave[0] = PASSWORD;
         syl_dataToSlave[1] = xor_reg;
         syl_tCount = 0;

         syl_SetupI2CTransmit(2);

         syl_dataToSlave[0] = regOffset;
         syl_dataToSlave[1] = dest_val;
         syl_tCount = 0;

         syl_SetupI2CTransmit(2);
    }
}
*/















