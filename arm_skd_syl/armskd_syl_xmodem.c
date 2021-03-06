/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                     *
*                                            All rights reserved.                                                *
***********************************************************************************************************************
* Module      : armskd_syl_xmodem.c
*
* Description : XMODEM realization
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************/
#include "uart_irda_cir.h"
#include "soc_AM335x.h"
#include "crc16.h"
#include <string.h>
#include "uartStdio.h"


#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A
#define DLY_1S 1000
#define MAXRETRANS 25


int _inbyte(unsigned int timeout) // msec timeout
{

    return(UARTCharGetTimeout(SOC_UART_0_REGS, timeout));

}
void _outbyte(int c)
{
    UARTPutc(c);
}



static int check(int crc, const unsigned char *buf, int sz)
{
	if (crc) {
		unsigned short crc = crc16_ccitt(buf, sz);
		unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
			return 1;
	}
	else {
		int i;
		unsigned char cks = 0;
		for (i = 0; i < sz; ++i) {
			cks += buf[i];
		}
		if (cks == buf[sz])
		return 1;
	}

	return 0;
}


static void flushinput(void)
{
	while (_inbyte(((DLY_1S)*3)>>1) >= 0)
		;
}



int xmodemReceive(unsigned char *dest, int destsz)
{
	unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	unsigned char *p;
	int bufsz, crc = 0;
	unsigned char trychar = 'C';
	unsigned char packetno = 1;
    int c;
	int i, len = 0;
	int retry;
	int retrans = MAXRETRANS;

	for(;;)
	{
		for( retry = 1; retry; ++retry)
		{
			if (trychar)
			{
				_outbyte(trychar);
			}
			if ((c = _inbyte((DLY_1S)<<10)) >= 0)
			{
				switch (c)
				{
					case SOH:
						bufsz = 128;
						goto start_recv;
					case STX:
						bufsz = 1024;
						goto start_recv;
					case EOT:
	//					flushinput();
						_outbyte(ACK);
						return len; /* normal end */
					case CAN:
						if ((c = _inbyte(DLY_1S)) == CAN)
						{
							flushinput();
							_outbyte(ACK);
							return -1; /* canceled by remote */
						}
						break;
					default:
						break;
				}
			}
		}
		if (trychar == 'C') { trychar = NAK; continue; }
		flushinput();
		_outbyte(CAN);
		_outbyte(CAN);
		_outbyte(CAN);
		return -2; /* sync error */

	start_recv:
		if (trychar == 'C') crc = 1;
		trychar = 0;
		p = xbuff;
		*p++ = c;
		for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
			if ((c = _inbyte(DLY_1S)) < 0) goto reject;
			*p++ = c;
		}

		if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
			(xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
			check(crc, &xbuff[3], bufsz)) {
			if (xbuff[1] == packetno)	{
				register int count = destsz - len;
				if (count > bufsz) count = bufsz;
				if (count > 0) {
					memcpy (&dest[len], &xbuff[3], count);
					len += count;
				}
				++packetno;
				retrans = MAXRETRANS+1;
			}
			if (--retrans <= 0) {
				flushinput();
				_outbyte(CAN);
				_outbyte(CAN);
				_outbyte(CAN);
				return -3; /* too many retry error */
			}
			_outbyte(ACK);
			continue;
		}
	reject:
		flushinput();
		_outbyte(NAK);
	}
}



void test_main_xmodem_recieve_functions()
{
    unsigned int i=0;
	int st;

	printf ("Send data using the xmodem protocol from your terminal emulator now...\n");
	/* the following should be changed for your environment:
	   0x30000 is the download address,
	   65536 is the maximum size to be written at this address
	 */


  //st = xmodemReceive((char *)0x30000, 65536);
  //	st = xmodemReceive((char *)0x81000000, 16777216);   //������������ ������ 64  �/����
	st = xmodemReceive((unsigned char *)0x81000000, 16777216);   //������������ ������ 64  �/����


	if (st < 0)
	{
		printf ("Xmodem receive error: status: %d\n", st);
	}
	else
	{
		printf ("Xmodem successfully received %d bytes\n", st);
	}


}
