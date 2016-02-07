/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_uart.c
*
* Description : UART transmit and Recieve Packet from uart
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************/
#include "uart_irda_cir.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "beaglebone.h"
#include "consoleUtils.h"
#include "hw_types.h"
#include "armskd_syl_uart.h"



/******************************************************************************
**              INTERNAL MACRO DEFINITIONS
******************************************************************************/
#define BAUD_RATE_115200          (115200)
#define UART_MODULE_INPUT_CLK     (48000000)

/*
** The number of data bytes to be transmitted to Transmit FIFO of UART
** per generation of the Transmit Empty interrupt. This can take a maximum
** value of TX Trigger Space which is 'TX FIFO size - TX Threshold Level'.
*/
#define NUM_TX_BYTES_PER_TRANS    (56)

/******************************************************************************
**              INTERNAL FUNCTION PROTOTYPES
******************************************************************************/
static void UartInterruptEnable(void);
static void UART0AINTCConfigure(void);
static void UartFIFOConfigure(void);
static void UartBaudRateSet(void);
static void UARTIsr(void);

/******************************************************************************
**              GLOBAL VARIABLE DEFINITIONS
******************************************************************************/
unsigned char txArray[] = "StarterWare AM335X UART Interrupt application\r\n";

/* A flag used to signify the application to transmit data to UART TX FIFO. */
unsigned int txEmptyFlag = FALSE;

/*
** A variable which holds the number of bytes of the data block transmitted to
** UART TX FIFO until the current instant.
*/
unsigned int currNumTxBytes = 0;




void init_syl_uart()
{
    unsigned int numByteChunks = 0;
    unsigned int remainBytes = 0;
    unsigned int bIndex = 0;

    /* Configuring the system clocks for UART0 instance. */
    UART0ModuleClkConfig();

    /* Performing the Pin Multiplexing for UART0 instance. */
    UARTPinMuxSetup(0);

    /* Performing a module reset. */
    UARTModuleReset(SOC_UART_0_REGS);

    /* Performing FIFO configurations. */
    UartFIFOConfigure();

    /* Performing Baud Rate settings. */
    UartBaudRateSet();

    /* Switching to Configuration Mode B. */
    UARTRegConfigModeEnable(SOC_UART_0_REGS, UART_REG_CONFIG_MODE_B);

    /* Programming the Line Characteristics. */
    UARTLineCharacConfig(SOC_UART_0_REGS, (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1), UART_PARITY_NONE);

    /* Disabling write access to Divisor Latches. */
    UARTDivisorLatchDisable(SOC_UART_0_REGS);

    /* Disabling Break Control. */
    UARTBreakCtl(SOC_UART_0_REGS, UART_BREAK_COND_DISABLE);

    /* Switching to UART16x operating mode. */
    UARTOperatingModeSelect(SOC_UART_0_REGS, UART16x_OPER_MODE);

    /* Select the console type based on compile time check */
    ConsoleUtilsSetType(CONSOLE_UART);

    /* Performing Interrupt configurations. */
    UartInterruptEnable();

    numByteChunks = (sizeof(txArray) - 1) / NUM_TX_BYTES_PER_TRANS;
    remainBytes = (sizeof(txArray) - 1) % NUM_TX_BYTES_PER_TRANS;

    while(1)
    {
        /* This branch is entered if the transmission is not yet complete. */
        if(TRUE == txEmptyFlag)
        {
            if(bIndex < numByteChunks)
            {
                /* Transmitting bytes in chunks of NUM_TX_BYTES_PER_TRANS. */
                currNumTxBytes += UARTFIFOWrite(SOC_UART_0_REGS,&txArray[currNumTxBytes],NUM_TX_BYTES_PER_TRANS);
                bIndex++;
            }

            else
            {
                /* Transmitting remaining data from the data block. */
                currNumTxBytes += UARTFIFOWrite(SOC_UART_0_REGS,&txArray[currNumTxBytes],remainBytes);
            }

            txEmptyFlag = FALSE;

            /*
            ** Re-enables the Transmit Interrupt. This interrupt
            ** was disabled in the Transmit section of the UART ISR.
            */
            UARTIntEnable(SOC_UART_0_REGS, UART_INT_THR);
        }//end if
    }//end while 1

}


/*
** A wrapper function performing FIFO configurations.
*/

static void UartFIFOConfigure(void)
{
    unsigned int fifoConfig = 0;

    /*
    ** - Transmit Trigger Level Granularity is 4
    ** - Receiver Trigger Level Granularity is 1
    ** - Transmit FIFO Space Setting is 56. Hence TX Trigger level
    **   is 8 (64 - 56). The TX FIFO size is 64 bytes.
    ** - The Receiver Trigger Level is 1.
    ** - Clear the Transmit FIFO.
    ** - Clear the Receiver FIFO.
    ** - DMA Mode enabling shall happen through SCR register.
    ** - DMA Mode 0 is enabled. DMA Mode 0 corresponds to No
    **   DMA Mode. Effectively DMA Mode is disabled.
    */
    fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_4,
                                  UART_TRIG_LVL_GRANULARITY_1,
                                  UART_FCR_TX_TRIG_LVL_56,
                                  1,
                                  1,
                                  1,
                                  UART_DMA_EN_PATH_SCR,
                                  UART_DMA_MODE_0_ENABLE);

    /* Configuring the FIFO settings. */
    UARTFIFOConfig(SOC_UART_0_REGS, fifoConfig);
}

/*
** A wrapper function performing Baud Rate settings.
*/

static void UartBaudRateSet(void)
{
    unsigned int divisorValue = 0;

    /* Computing the Divisor Value. */
    divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
                                         BAUD_RATE_115200,
                                         UART16x_OPER_MODE,
                                         UART_MIR_OVERSAMPLING_RATE_42);

    /* Programming the Divisor Latches. */
    UARTDivisorLatchWrite(SOC_UART_0_REGS, divisorValue);
}

/*
** A wrapper function performing Interrupt configurations.
*/

static void UartInterruptEnable(void)
{
    /* Enabling IRQ in CPSR of ARM processor. */
    IntMasterIRQEnable();

    /* Configuring AINTC to receive UART0 interrupts. */
    UART0AINTCConfigure();

    /* Enabling the specified UART interrupts. */
    UARTIntEnable(SOC_UART_0_REGS, (UART_INT_LINE_STAT | UART_INT_THR |UART_INT_RHR_CTI));
}

/*
** Interrupt Service Routine for UART.
*/
static void UARTIsr(void)
{
    unsigned int rxErrorType = 0;
    unsigned char rxByte = 0;
    unsigned int intId = 0;
    unsigned int idx = 0;
    unsigned int i=0;

    unsigned short rx_array[22];

    //unsigned int rx_integer=0,rx_integer1=0;

    /* Checking ths syource of UART interrupt. */
    intId = UARTIntIdentityGet(SOC_UART_0_REGS);
    //printf("intId=0x%x\n\r",intId);
    switch(intId)
    {
        case UART_INTID_TX_THRES_REACH:
        	printf("UART_INTID_TX_THRES_REACH\n\r");
            /*
            ** Checking if the entire transmisssion is complete. If this
            ** condition fails, then the entire transmission has been completed.
            */
            if(currNumTxBytes < (sizeof(txArray) - 1))
            {
                txEmptyFlag = TRUE;
            }

            /*
            ** Disable the THR interrupt. This has to be done even if the
            ** transmission is not complete so as to prevent the Transmit
            ** empty interrupt to be continuously generated.
            */
            UARTIntDisable(SOC_UART_0_REGS, UART_INT_THR);

        break;

        case UART_INTID_RX_THRES_REACH:
        	/*Берём один байт из UART */

        	//printf("UART_INTID_RX_THRES_REACH");
        	rx_array[0]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[1]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[2]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[3]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[4]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[5]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[6]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[7]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[8]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[9]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[10]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[11]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[12]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[13]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[14]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[15]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[16]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[17]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[18]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[19]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[20]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);
        	rx_array[21]=UARTCharGetTimeout(SOC_UART_0_REGS, 100);



            for(i=0;i<22;i++)
            {
            	printf("rx_byte=%X\n\r",rx_array[i]);

            }

        	//rxByte = UARTCharGetNonBlocking(SOC_UART_0_REGS);
        	//printf("rx_byte=%c\n\r",rxByte);
        //	printf("rx_byte=%x,rx_byte1=%x\n\r",rx_integer,rx_integer1);

        	//UARTCharPutNonBlocking(SOC_UART_0_REGS, rxByte);
        break;


        case UART_INTID_RX_LINE_STAT_ERROR:
        	printf("UART_INTID_RX_LINE_STAT_ERROR\n\r");
            rxErrorType = UARTRxErrorGet(SOC_UART_0_REGS);

            /* Check if Overrun Error has occured. */
            if(rxErrorType & UART_LSR_RX_OE)
            {
                ConsoleUtilsPrintf("\r\nUART Overrun Error occured."
                              " Reading and Echoing all data in RX FIFO.\r\n");

                /* Read the entire RX FIFO and the data in RX Shift register. */
                for(idx = 0; idx < (RX_FIFO_SIZE + 1); idx++)
                {
                    rxByte = UARTFIFOCharGet(SOC_UART_0_REGS);
                    UARTFIFOCharPut(SOC_UART_0_REGS, rxByte);
                }

                break;
            }

            /* Check if Break Condition has occured. */
            else if(rxErrorType & UART_LSR_RX_BI)
            {
                ConsoleUtilsPrintf("\r\nUART Break Condition occured.");
            }

            /* Check if Framing Error has occured. */
            else if(rxErrorType & UART_LSR_RX_FE)
            {
                ConsoleUtilsPrintf("\r\nUART Framing Error occured.");
            }

            /* Check if Parity Error has occured. */
            else if(rxErrorType & UART_LSR_RX_PE)
            {
                ConsoleUtilsPrintf("\r\nUART Parity Error occured.");
            }

            ConsoleUtilsPrintf(" Data at the top of RX FIFO is: ");
            rxByte = UARTFIFOCharGet(SOC_UART_0_REGS);
            UARTFIFOCharPut(SOC_UART_0_REGS, rxByte);

        break;


        case UART_INTID_CHAR_TIMEOUT:
        	printf("UART_INTID_CHAR_TIMEOUT\n\r");
            ConsoleUtilsPrintf("\r\nUART Character Timeout Interrupt occured."
                              " Reading and Echoing all data in RX FIFO.\r\n");

            /* Read all the data in RX FIFO. */
            while(TRUE == UARTCharsAvail(SOC_UART_0_REGS))
            {
                rxByte = UARTFIFOCharGet(SOC_UART_0_REGS);
                UARTFIFOCharPut(SOC_UART_0_REGS, rxByte);
            }

        break;

        default:
        printf("default\n\r");
        break;
    }

}



/*
** This function configures the AINTC to receive UART interrupts.
*/
static void UART0AINTCConfigure(void)
{
    /* Initializing the ARM Interrupt Controller. */
  //  IntAINTCInit();

    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_UART0INT, UARTIsr);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_UART0INT, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_UART0INT);
}





















