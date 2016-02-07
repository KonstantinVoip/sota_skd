/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                                *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_first_emmc_flash.c
*
* Description : Function for first flashing eMMC -> MLO, APLLICATION and TFA/TYR table
*
* Author      : Konstantin Shiluaev
*
******************************************************************************/
#include "stdlib.h"
#include "soc_AM335x.h"
#include "armskd_syl_menu.h"
#include "armskd_syl_spi.h"
#include "ConsoleUtils.h"

#include "cmdLine.h"   //Здесь находиться функция  CmdLineProcess


/* SD Controller info structure */

/*****************************************************************************
Defines the size of the buffers that hold the path, or temporary data from
the memory card.  There are two buffers allocated of this size.  The buffer
size must be large enough to hold the longest expected full path name,
including the file name, and a trailing null character.
******************************************************************************/
#define PATH_BUF_SIZE   512
/* Defines size of the buffers that hold temporary data. */
#define DATA_BUF_SIZE   64 * (2 * 512)
/*****************************************************************************
Defines the size of the buffer that holds the command line.
******************************************************************************/
#define CMD_BUF_SIZE    512
/*****************************************************************************
The buffer that holds the command line.
******************************************************************************/
static char g_cCmdBuf[CMD_BUF_SIZE];
volatile unsigned int g_sPState = 0;
volatile unsigned int g_sCState = 0;

/*****************************************************************************
This buffer holds the full path to the current working directory.  Initially
it is root ("/").
******************************************************************************/
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

/*****************************************************************************
A temporary data buffer used for write file paths
******************************************************************************/
static char g_cWrBuf[PATH_BUF_SIZE] = "/";

//Описание Команд
int Cmd_help(int argc, char *argv[]);

int Cmd_ppm_ch1(int argc, char *argv[]);
int Cmd_ppm_ch2(int argc, char *argv[]);
int Cmd_ppm_ch3(int argc, char *argv[]);
int Cmd_ppm_ch4(int argc, char *argv[]);
int Cmd_start(int argc, char *argv[]);
int Cmd_stop (int argc, char *argv[]);

//Команды Обновления ПО
int Cmd_update_boot (int argc, char *argv[]);
int Cmd_update_application (int argc, char *argv[]);
int Cmd_update_table (int argc, char *argv[]);


void HSMMCSDFsProcessCmdLine(void);
/*******************************************************************************
**
** This is the table that holds the command names, implementing functions, and
** brief description.
**
*******************************************************************************/
tCmdLineEntry g_sCmdTable[] =
{
     { "help" ,Cmd_help,  			 ":  0_bit[0-1] 1-6_bit[0-3F(63)]  7-12_bit[0-3F(63)]" },


#ifdef TYPE_MODULE_PPM
	  { "ch1"  ,Cmd_ppm_ch1,	 		 ": [RX_stby_1][TX_ph_6][RX_ph_6][TX_at_6][RX_at_6][TX_stby_1]" },
      { "ch2"  ,Cmd_ppm_ch2,		     ": [RX_stby_1][TX_ph_6][RX_ph_6][TX_at_6][RX_at_6][TX_stby_1]" },
      { "ch3"  ,Cmd_ppm_ch3,		     ": [RX_stby_1][TX_ph_6][RX_ph_6][TX_at_6][RX_at_6][TX_stby_1]" },
      { "ch4"  ,Cmd_ppm_ch4,		     ": [RX_stby_1][TX_ph_6][RX_ph_6][TX_at_6][RX_at_6][TX_stby_1]" },
#endif

#ifdef TYPE_MODULE_PM
	  { "ch1"  ,Cmd_ppm_ch1,	 		 ": [RX_stby_1bit]  [RX_ph_6bit]  [RX_at_6bit]" },
      { "ch2"  ,Cmd_ppm_ch2,		     ": [RX_stby_1bit]  [RX_ph_6bit]  [RX_at_6bit]" },
      { "ch3"  ,Cmd_ppm_ch3,		     ": [RX_stby_1bit]  [RX_ph_6bit]  [RX_at_6bit]" },
      { "ch4"  ,Cmd_ppm_ch4,		     ": [RX_stby_1bit]  [RX_ph_6bit]  [RX_at_6bit]" },
#endif


	  { "start",Cmd_start  ,  		     ": [num_of_iter][num_of_ch_1,2,3,4]" },
//    { "stop" ,Cmd_stop   ,   		     ": [stop transmit to ppm all ch]" },
      { "sw_update_boot",Cmd_update_boot,": [update MLO first loader-xmodem]" },
	  { "sw_update_application",Cmd_update_application,": [update Application-xmodem]" },
      { "sw_update_tab",    	Cmd_update_table,": [update tfa_tab,tyr_tab-xmodem]" },
    { 0, 0, 0 }
};


/*****************************************************************************
This function returns a string representation of an error code that was
returned from a function call to FatFs.  It can be used for printing human
readable error messages.
*****************************************************************************/

#if 0
const char * StringFromFresult(FRESULT fresult)
{
    unsigned int uIdx;

    /* Enter a loop to search the error code table for a matching error code. */
    for(uIdx = 0; uIdx < NUM_FRESULT_CODES; uIdx++)
    {
        /*
        ** If a match is found, then return the string name of the error code.
        */
        if(g_sFresultStrings[uIdx].fresult == fresult)
        {
            return(g_sFresultStrings[uIdx].pcResultStr);
        }
    }

    /*
    ** At this point no matching code was found, so return a string indicating
    ** unknown error.
    */
    return("UNKNOWN ERROR CODE");
}
#endif




//******************************************************************************
//    Main Menu
//******************************************************************************
void syl_MainMenu(void)
{

   while(1)
   {

	   HSMMCSDFsProcessCmdLine();


   }


#if 0




	char choice[2];
	 char* ret_buf;

	 unsigned char ppm_parametr[5];   //0-255
	 char data[5];

	//char LED;
	//char UART;
	//char Audio;
	//char Switch;
	//char TempSensor;
//#if 0

	//unsigned int validChoice = 0;

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("*                  PELENGATOR SOTA TEST BLOCK                  *\n");
	ConsoleUtilsPrintf("****************************************************************\n");


	////Выбираем канал который будем тестировать
	ConsoleUtilsPrintf("1.Select_X01_sel12_00\n\r");
	//Выбираем канал который будем тестировать
	ConsoleUtilsPrintf("2.Select_X02_sel12_01\n\r");
	//Выбираем канал который будем тестировать
	ConsoleUtilsPrintf("3.Select_X03_sel12_10\n\r");
	//Выбираем канал который будем тестировать
	ConsoleUtilsPrintf("4.Select_X04_sel12_11\n\r");

	//memset(&choice,0x00,sizeof(choice));   //DDR  256 М/байт    0-256  М/байт

	ret_buf=ConsoleUtilsGets(choice, 2);


	 CmdLineProcess(choice);

	switch(choice[0])
	{
	case '1':
		ConsoleUtilsPrintf("Channel_1:\n\r");
	break;

	case '2':
		ConsoleUtilsPrintf("Channel_2:\n\r");
	break;

	case '3':
		ConsoleUtilsPrintf("Channel_3:\n\r");
	break;

	default:
		ConsoleUtilsPrintf("Channel_4:\n\r");
	break;
	}

#if  0
	switch(choice[0])
	{
	case '1':
		ConsoleUtilsPrintf("Channel_1:\n\r");
     	ConsoleUtilsPrintf("get_Rx Standby Control\n\r");
		ConsoleUtilsGets(&data[0], 1);
		ConsoleUtilsPrintf("get_Tx Phase Shifting Control\n\r");
		ConsoleUtilsGets(&data[1], 1);
		ConsoleUtilsPrintf("get_Rx Phase Shifting Control\n\r");
		ConsoleUtilsGets(&data[2], 1);
		ConsoleUtilsPrintf("get_Tx Attenuation Control\n\r");
		ConsoleUtilsGets(&data[3], 1);
		ConsoleUtilsPrintf("get_Rx Attenuation Control\n\r");
		ConsoleUtilsGets(&data[4], 1);
		ConsoleUtilsPrintf("get_Tx Standby Control\n\r");
		ConsoleUtilsGets(&data[5], 1);

	break;

	case '2':
		ConsoleUtilsPrintf("->2.\n\r");
	break;

	case '3':
		ConsoleUtilsPrintf("->3.\n\r");
	break;

	case '4':
		ConsoleUtilsPrintf("->4.\n\r");
	break;

	default:
		ConsoleUtilsPrintf("default->\n\r");
	break;
	}

#endif

#if 0
	while(!validChoice)
	{

		ConsoleUtilsPrintf("1. SPI_PPM_TEST\n");
		/*ConsoleUtilsPrintf("2. Switches\n");
		ConsoleUtilsPrintf("3. Audio\n");
		ConsoleUtilsPrintf("4. Uart\n");
		ConsoleUtilsPrintf("5. Temp sensor\n");
		ConsoleUtilsPrintf("6. All\n");*/

		ConsoleUtilsGets(choice, 5);


		switch(choice[0])
		{
		case '1':
		    //Test_SPI_to_PPM_block


			////Выбираем канал который будем тестировать
			//ConsoleUtilsPrintf("Select_X01_sel12_00\n\r");

			//Выбираем канал который будем тестировать
			//ConsoleUtilsPrintf("Select_X02_sel12_01\n\r");

			//Выбираем канал который будем тестировать
			//ConsoleUtilsPrintf("Select_X03_sel12_10\n\r");

			//Выбираем канал который будем тестировать
			//ConsoleUtilsPrintf("Select_X04_sel12_11\n\r");
			//Здесь должен быть ущё сигналы T и R (transmit/recieve)
			//send_to_ppm_channel_1(0x0A0A0A0A);
			//send_to_ppm_channel_2(0x0B0B0B0B);
			//send_to_ppm_channel_3(0x0C0C0C0C);
			//send_to_ppm_channel_4(0x0C0C0C0C);
	    choice[0]=255; //выход из меню нашего получаеться
		break;



		default:
		//ConsoleUtilsPrintf("\nInvalid input.\n\n");
		break;

		}
	}

#endif

#endif

}



/*******************************************************************************
**
** void ReadLine(void)
**
*******************************************************************************/
void ReadLine(void)
{
    unsigned long ulIdx;
    unsigned char ucChar;

    /*
    ** Start reading at the beginning of the command buffer and print a prompt.
    */
    g_cCmdBuf[0] = '\0';
    ulIdx = 0;

    /*
    ** Loop forever.  This loop will be explicitly broken out of when the line
    ** has been fully read.
    */
    while(1)
    {
        /*
        ** Attempt to open the directory.
        */

#if 0
    	if((HSMMCSDCardPresent(&ctrlInfo)) == 1)
        {
            if(g_sCState == 0)
            {

            	if(f_opendir(&g_sDirObject, g_cCwdBuf) != FR_OK)
                {
                    ConsoleUtilsPrintf("\nFailed to open directory.\n");
                    g_sCState = 0;
                    g_sPState = 0;
                    g_cCmdBuf[0] = '\0';
                    return;
                }
                else
                {
                    g_sCState = 1;
                }

            }

            g_sCState = 1;

            if (g_sCState != g_sPState)
            {
                if (g_sCState == 0)
                {
                    ConsoleUtilsPrintf("%s>", "UNKNOWN");
                    g_sPState = 0;

                    return;
                }
                else
                {
                    ConsoleUtilsPrintf("%s> %s", g_cCwdBuf, g_cCmdBuf);
                    g_sPState = 1;
                }
            }
        }//end HSMMCSDCardPresent
        else
        {
            g_sCState = 0;
            g_sPState = 0;
            g_cCmdBuf[0] = '\0';
            return;
        }//end HSMMCSDCardPresent
#endif



    	////////////////////////////////////////////////////////////////////////////////////
        /*
        ** Loop while there are characters that have been received from the
        ** UART.
        */
        while(UARTCharsAvail(SOC_UART_0_REGS))
        {
            /*
            ** Read the next character from the UART.
            */
            ucChar = UARTGetc();

            /*
            ** See if this character is a backspace and there is at least one
            ** character in the input line.
            */
            if((ucChar == '\b') && (ulIdx != 0))
            {
                /*
                ** Erase the lsat character from the input line.
                */
                ConsoleUtilsPrintf("\b \b");
                ulIdx--;
                g_cCmdBuf[ulIdx] = '\0';
            }

            /*
            ** See if this character is a newline.
            */
            else if((ucChar == '\r') || (ucChar == '\n'))
            {
                /*
                ** Return to the caller.
                */
                ConsoleUtilsPrintf("\n");
                return;
            }

            /*
            ** See if this character is an escape or Ctrl-U.
            */
            else if((ucChar == 0x1b) || (ucChar == 0x15))
            {
                /*
                ** Erase all characters in the input buffer.
                */
                while(ulIdx)
                {
                    ConsoleUtilsPrintf("\b \b");
                    ulIdx--;
                }
                g_cCmdBuf[0] = '\0';
            }

            /*
            ** See if this is a printable ASCII character.
            */
            else if((ucChar >= ' ') && (ucChar <= '~') && (ulIdx < (sizeof(g_cCmdBuf) - 1)))
            {
                /*
                ** Add this character to the input buffer.
                */
                g_cCmdBuf[ulIdx++] = ucChar;
                g_cCmdBuf[ulIdx] = '\0';
                ConsoleUtilsPrintf("%c", ucChar);
            }
        }
    } //end while 1
}
/*******************************************************************************
**
** void HSMMCSDFsProcessCmdLine(void)
**
*******************************************************************************/
void HSMMCSDFsProcessCmdLine(void)
{
    int iStatus;
    //
    // Enter an infinite loop for reading and processing commands from the
    // user.
    //
    //
    // Get a line of text from the user.
    //
    ReadLine();
    if(g_cCmdBuf[0] == '\0')
    {
          return;
    }

    //
    // Pass the line from the user to the command processor.
    // It will be parsed and valid commands executed.
    //
    iStatus = CmdLineProcess(g_cCmdBuf);  //include cmdline process

    //
    // Handle the case of bad command.
    //
    if(iStatus == CMDLINE_BAD_CMD)
    {
        ConsoleUtilsPrintf("Bad command!\n");
    }

    //
    // Handle the case of too many arguments.
    //
    else if(iStatus == CMDLINE_TOO_MANY_ARGS)
    {
        ConsoleUtilsPrintf("Too many arguments for command processor!\n");
    }

    //
    // Otherwise the command was executed.  Print the error
    // code if one was returned.
    //
    else if(iStatus != 0)
    {
        //ConsoleUtilsPrintf("Command returned error code %s\n", StringFromFresult((FRESULT)iStatus));
    }

    g_cCmdBuf[0] = '\0';
    ConsoleUtilsPrintf("%s> %s", g_cCwdBuf, g_cCmdBuf);
}


/*******************************************************************************
**
** This function implements the "help" command.  It prints a simple list of the
** available commands with a brief description.
**
*******************************************************************************/
int Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    /*
    ** Print some header text.
    */
    ConsoleUtilsPrintf("\nAvailable commands\n");
    ConsoleUtilsPrintf("------------------\n");

    /*
    ** Point at the beginning of the command table.
    */
    pEntry = &g_sCmdTable[0];

    /*
    ** Enter a loop to read each entry from the command table.  The end of the
    ** table has been reached when the command name is NULL.
    */
    while(pEntry->pcCmd)
    {
        /*
        ** Print the command name and the brief description.
        */
        ConsoleUtilsPrintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

        /*
        ** Advance to the next entry in the table.
        */
        pEntry++;
    }

    /*
    ** Return success.
    */
    return(0);
}




/**************************************************************************************************
Parameters:         int Cmd_ppm_ch1(int argc, char *argv[])
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/
int Cmd_ppm_ch1(int argc, char *argv[])
{

	//unsigned short i=0;
    //unsigned char todo[3];
	//unsigned char* todo=0;
	  //Для ППМ модуля
    unsigned char ppm_Rx_stby=0x1;                //по умолчанию включен на приём Recieve
    unsigned char ppm_Rx_phase_shifting=0x1;      //по умолчания -90[град]
    unsigned char ppm_Rx_attenuation_control=0x1; //по умолчания -16[дБ]



#ifdef TYPE_MODULE_PPM
    unsigned char ppm_Tx_attenuation_control=0x1; //по умолчания -16[дБ]
    unsigned char ppm_Tx_phase_shifting=0x1;      //по умолчания -90[град]
    unsigned char ppm_Tx_stby=0x0;                //Tx выключена на облучение
#endif


	//Теперь нужно получить параметры параметры:
	if(argc==0)
	{
		ConsoleUtilsPrintf("error_num_of_argument==0\n\r");
		return 0;
	}

    //6 аргументов в консоли

#ifdef TYPE_MODULE_PPM
	if(argc==7)
	{
	ppm_Rx_stby=atoi(argv[1]);
	ConsoleUtilsPrintf("argv_1=%d|char=%c|hex1=0x%x\n\r",ppm_Rx_stby,ppm_Rx_stby,ppm_Rx_stby);

	ppm_Tx_phase_shifting=atoi(argv[2]);
	ConsoleUtilsPrintf("argv_2=%d|char=%c|hex1=0x%x\n\r",ppm_Tx_phase_shifting,ppm_Tx_phase_shifting,ppm_Tx_phase_shifting);

	ppm_Rx_phase_shifting=atoi(argv[3]);
	ConsoleUtilsPrintf("argv_3=%d|char=%c|hex1=0x%x\n\r",ppm_Rx_phase_shifting,ppm_Rx_phase_shifting,ppm_Rx_phase_shifting);

	ppm_Tx_attenuation_control=atoi(argv[4]);
	ConsoleUtilsPrintf("argv_4=%d|char=%c|hex1=0x%x\n\r",ppm_Tx_attenuation_control,ppm_Tx_attenuation_control,ppm_Tx_attenuation_control);

	ppm_Rx_attenuation_control=atoi(argv[5]);
	ConsoleUtilsPrintf("argv_5=%d|char=%c|hex1=0x%x\n\r",ppm_Rx_attenuation_control,ppm_Rx_attenuation_control,ppm_Rx_attenuation_control);

	ppm_Tx_stby=atoi(argv[6]);
	ConsoleUtilsPrintf("argv_6=%d|char=%c|hex1=0x%x\n\r",ppm_Tx_stby,ppm_Tx_stby,ppm_Tx_stby);

    ppm_module_ch1.tx_standy				 = ppm_Tx_stby; //25 бит
	ppm_module_ch1.rx_attenuation_control    = ppm_Rx_attenuation_control; //19-24 бит
	ppm_module_ch1.tx_attenuation_control    = ppm_Tx_attenuation_control; //13-18 бит
	ppm_module_ch1.rx_phase_shifting_control = ppm_Rx_phase_shifting; //7-12  бит
	ppm_module_ch1.tx_phase_shifting_control = ppm_Tx_phase_shifting; //1-6   бит
	ppm_module_ch1.rx_standy                 = ppm_Rx_stby; // 0    бит

	memcpy(&buf_dest_ppm.buf_dest_ch1,&ppm_module_ch1,sizeof(buf_dest_ppm.buf_dest_ch1));
	}
	else
	{
		ConsoleUtilsPrintf("no valid argument number for PPM =%d\n\r",argc);
		return 0;
	}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TYPE_MODULE_PM
	  if(argc==4)
		{
	    ppm_Rx_stby=atoi(argv[1]);
		ConsoleUtilsPrintf("argv_1=%d|hex1=0x%x\n\r",ppm_Rx_stby,ppm_Rx_stby);

		ppm_Rx_phase_shifting=atoi(argv[2]);
		ConsoleUtilsPrintf("argv_2=%d|hex1=0x%x\n\r",ppm_Rx_phase_shifting,ppm_Rx_phase_shifting);

		ppm_Rx_attenuation_control=atoi(argv[3]);
		ConsoleUtilsPrintf("argv_3=%d|hex1=0x%x\n\r",ppm_Rx_attenuation_control,ppm_Rx_attenuation_control);

		pm_module_ch1.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
		pm_module_ch1.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
		pm_module_ch1.rx_standy                 = ppm_Rx_stby; // 0    бит
		memcpy(&buf_dest_ppm.buf_dest_ch1,&pm_module_ch1,sizeof(buf_dest_ppm.buf_dest_ch1));

		}
	  else
	  {
		  ConsoleUtilsPrintf("no valid argument number for PM =%d\n\r",argc);
		  return 0;

	  }

#endif


    /*
    ** Return success.
    */
 return(0);
}



/**************************************************************************************************
Parameters:         int Cmd_ppm_ch2(int argc, char *argv[])
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/
int Cmd_ppm_ch2(int argc, char *argv[])
{
	  //Для ППМ модуля
unsigned char ppm_Rx_stby=0x1;                //по умолчанию включен на приём Recieve
unsigned char ppm_Rx_phase_shifting=0x1;      //по умолчания -90[град]
unsigned char ppm_Rx_attenuation_control=0x1; //по умолчания -16[дБ]



#ifdef TYPE_MODULE_PPM
  unsigned char ppm_Tx_attenuation_control=0x1; //по умолчания -16[дБ]
  unsigned char ppm_Tx_phase_shifting=0x1;      //по умолчания -90[град]
  unsigned char ppm_Tx_stby=0x0;                //Tx выключена на облучение
#endif

	//Теперь нужно получить параметры параметры:
	if(argc==0)
	{
		ConsoleUtilsPrintf("ch2error_num_of_argument==0\n\r");
		return 0;
	}

#ifdef TYPE_MODULE_PM
	  if(argc==4)
		{
	    ppm_Rx_stby=atoi(argv[1]);
		ConsoleUtilsPrintf("ch2argv_1=%d|hex1=0x%x\n\r",ppm_Rx_stby,ppm_Rx_stby);

		ppm_Rx_phase_shifting=atoi(argv[2]);
		ConsoleUtilsPrintf("ch2argv_2=%d|hex1=0x%x\n\r",ppm_Rx_phase_shifting,ppm_Rx_phase_shifting);

		ppm_Rx_attenuation_control=atoi(argv[3]);
		ConsoleUtilsPrintf("ch2argv_3=%d|hex1=0x%x\n\r",ppm_Rx_attenuation_control,ppm_Rx_attenuation_control);

		pm_module_ch2.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
		pm_module_ch2.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
		pm_module_ch2.rx_standy                 = ppm_Rx_stby; // 0    бит
		memcpy(&buf_dest_ppm.buf_dest_ch2,&pm_module_ch2,sizeof(buf_dest_ppm.buf_dest_ch2));

		}
	  else
	  {
		  ConsoleUtilsPrintf("ch2 no valid argument number for PM =%d\n\r",argc);
		  return 0;

	  }

#endif
	  /*
	    ** Return success.
	    */
	 return(0);
}


/**************************************************************************************************
Parameters:         int Cmd_ppm_ch3(int argc, char *argv[])
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/
int Cmd_ppm_ch3(int argc, char *argv[])
{


//Для ППМ модуля
unsigned char ppm_Rx_stby=0x1;                //по умолчанию включен на приём Recieve
unsigned char ppm_Rx_phase_shifting=0x1;      //по умолчания -90[град]
unsigned char ppm_Rx_attenuation_control=0x1; //по умолчания -16[дБ]



#ifdef TYPE_MODULE_PPM
unsigned char ppm_Tx_attenuation_control=0x1; //по умолчания -16[дБ]
unsigned char ppm_Tx_phase_shifting=0x1;      //по умолчания -90[град]
unsigned char ppm_Tx_stby=0x0;                //Tx выключена на облучение
#endif




		//Теперь нужно получить параметры параметры:
		if(argc==0)
		{
	    ConsoleUtilsPrintf("ch3error_num_of_argument==0\n\r");
	    return 0;
        }


#ifdef TYPE_MODULE_PM
	  if(argc==4)
		{
	    ppm_Rx_stby=atoi(argv[1]);
		ConsoleUtilsPrintf("ch3argv_1=%d|hex1=0x%x\n\r",ppm_Rx_stby,ppm_Rx_stby);

		ppm_Rx_phase_shifting=atoi(argv[2]);
		ConsoleUtilsPrintf("ch3argv_2=%d|hex1=0x%x\n\r",ppm_Rx_phase_shifting,ppm_Rx_phase_shifting);

		ppm_Rx_attenuation_control=atoi(argv[3]);
		ConsoleUtilsPrintf("ch3argv_3=%d|hex1=0x%x\n\r",ppm_Rx_attenuation_control,ppm_Rx_attenuation_control);

		pm_module_ch3.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
		pm_module_ch3.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
		pm_module_ch3.rx_standy                 = ppm_Rx_stby; // 0    бит
		memcpy(&buf_dest_ppm.buf_dest_ch3,&pm_module_ch3,sizeof(buf_dest_ppm.buf_dest_ch3));

		}
	  else
	  {
		  ConsoleUtilsPrintf("ch3no valid argument number for PM =%d\n\r",argc);
		  return 0;

	  }

#endif

	    /*
	    ** Return success.
	    */
 return(0);

}



/**************************************************************************************************
Parameters:         int Cmd_ppm_ch4(int argc, char *argv[])
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/
int Cmd_ppm_ch4(int argc, char *argv[])
{

//Для ППМ модуля
unsigned char ppm_Rx_stby=0x1;                //по умолчанию включен на приём Recieve
unsigned char ppm_Rx_phase_shifting=0x1;      //по умолчания -90[град]
unsigned char ppm_Rx_attenuation_control=0x1; //по умолчания -16[дБ]



#ifdef TYPE_MODULE_PPM
unsigned char ppm_Tx_attenuation_control=0x1; //по умолчания -16[дБ]
unsigned char ppm_Tx_phase_shifting=0x1;      //по умолчания -90[град]
unsigned char ppm_Tx_stby=0x0;                //Tx выключена на облучение
#endif


		//Теперь нужно получить параметры параметры:
		if(argc==0)
		{
        ConsoleUtilsPrintf("ch4error_num_of_argument==0\n\r");
	    return 0;
		}

#ifdef TYPE_MODULE_PM
	  if(argc==4)
		{
	    ppm_Rx_stby=atoi(argv[1]);
		ConsoleUtilsPrintf("ch4argv_1=%d|hex1=0x%x\n\r",ppm_Rx_stby,ppm_Rx_stby);

		ppm_Rx_phase_shifting=atoi(argv[2]);
		ConsoleUtilsPrintf("ch4argv_2=%d|hex1=0x%x\n\r",ppm_Rx_phase_shifting,ppm_Rx_phase_shifting);

		ppm_Rx_attenuation_control=atoi(argv[3]);
		ConsoleUtilsPrintf("ch4argv_3=%d|hex1=0x%x\n\r",ppm_Rx_attenuation_control,ppm_Rx_attenuation_control);

		pm_module_ch4.rx_attenuation_control    = ppm_Rx_attenuation_control; //7-12 бит
		pm_module_ch4.rx_phase_shifting_control = ppm_Rx_phase_shifting; //1-6  бит
		pm_module_ch4.rx_standy                 = ppm_Rx_stby; // 0    бит
		memcpy(&buf_dest_ppm.buf_dest_ch4,&pm_module_ch4,sizeof(buf_dest_ppm.buf_dest_ch4));

		}
	  else
	  {
		  ConsoleUtilsPrintf("ch4no valid argument number for PM =%d\n\r",argc);
		  return 0;

	  }

#endif

	  /*
	    ** Return success.
	    */
return(0);
}





/**************************************************************************************************
Parameters:         int Cmd_start(int argc, char *argv[])
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/
int Cmd_start(int argc, char *argv[])
{
	unsigned int num_of_iterartion=0;
	unsigned int num_of_channel=0;
    unsigned int i=0;


	//Теперь нужно получить параметры параметры:
	if(argc>3)
	{
		ConsoleUtilsPrintf("error_num_of_argument>2\n\r");
		return 0;
	}

	if(argc==0)
	{
		ConsoleUtilsPrintf("error_num_of_argument==0\n\r");
		return 0;
	}

	if(argc==1)
	{
	   ConsoleUtilsPrintf("error_num_of_argument==1\n\r");
	   return 0;
	}


   if(argc==3)
   {
	num_of_iterartion=atoi(argv[1]);
	ConsoleUtilsPrintf("num_of_iterartion=%d|num_of_iterartion=0x%x\n\r",num_of_iterartion,num_of_iterartion);
	num_of_channel=atoi(argv[2]);
	ConsoleUtilsPrintf("num_of_iterartion=%d|num_of_iterartion=0x%x\n\r",num_of_channel,num_of_channel);

	 if(num_of_channel==1)
	 {
			 for(i=0;i<=num_of_iterartion ;i++)
			 {
			 	send_to_ppm_channel_1(buf_dest_ppm.buf_dest_ch1);
			 }

	 ConsoleUtilsPrintf("STOP TRANSMISSION_1ch\n\r");
	 return 0;
	 }

	 if(num_of_channel==2)
	 {
		 for(i=0;i<=num_of_iterartion ;i++)
		 {
		 	send_to_ppm_channel_1(buf_dest_ppm.buf_dest_ch1);
		 	send_to_ppm_channel_2(buf_dest_ppm.buf_dest_ch2);
		 }

	 ConsoleUtilsPrintf("STOP TRANSMISSION_2ch\n\r");
	 return 0;
	 }

	 if(num_of_channel==3)
	 {
		for(i=0;i<=num_of_iterartion ;i++)
		{
		send_to_ppm_channel_1(buf_dest_ppm.buf_dest_ch1);
		send_to_ppm_channel_2(buf_dest_ppm.buf_dest_ch2);
		send_to_ppm_channel_3(buf_dest_ppm.buf_dest_ch3);
		}

	  ConsoleUtilsPrintf("STOP TRANSMISSION_3ch\n\r");
	  return 0;
	 }

	 if(num_of_channel==4)
	 {
	 	for(i=0;i<=num_of_iterartion ;i++)
	 	{
	 	send_to_ppm_channel_1(buf_dest_ppm.buf_dest_ch1);
	 	send_to_ppm_channel_2(buf_dest_ppm.buf_dest_ch2);
	 	send_to_ppm_channel_3(buf_dest_ppm.buf_dest_ch3);
	 	send_to_ppm_channel_4(buf_dest_ppm.buf_dest_ch4);
	 	}
	 ConsoleUtilsPrintf("STOP TRANSMISSION_4ch\n\r");
	 return 0;
	 }

   }
   else
   {
	   ConsoleUtilsPrintf("no valid argument number for start_PPM =%d\n\r",argc);
	   return 0;
   }

 return 0;
}







/**************************************************************************************************
Parameters:         void sys_timer_set_millisecond_delay(uint16_t  millisecond)
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/

int Cmd_stop (int argc, char *argv[])
{

	//Cmd_start(int argc, char *argv[])
	//Cmd_start(,);
return 0;
}



/**************************************************************************************************
Parameters:         void sys_timer_set_millisecond_delay(uint16_t  millisecond)
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/

int Cmd_update_boot (int argc, char *argv[])
{

	ConsoleUtilsPrintf("UPDATE MLO BOOT LOADER\n\r");
	return 0;
}


/**************************************************************************************************
Parameters:         void sys_timer_set_millisecond_delay(uint16_t  millisecond)
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/

int Cmd_update_application (int argc, char *argv[])
{

	ConsoleUtilsPrintf("UPDATE APPLICATION\n\r");

	return 0;
}


/**************************************************************************************************
Parameters:         void sys_timer_set_millisecond_delay(uint16_t  millisecond)
Remarks:            устанавливаем задержку таймера в миллисеекундах [1-1000]
***************************************************************************************************/
int Cmd_update_table (int argc, char *argv[])
{


	ConsoleUtilsPrintf("UPDATE TABLE TFA TYR\n\r");
	return 0;
}











