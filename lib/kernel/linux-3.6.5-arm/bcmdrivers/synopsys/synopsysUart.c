/*
 * synopsys.c
 *
 *  Created on: 09-Apr-2013
 *      Author: Chandra Mohan C 
 *      Email : chandra@broadcom.com
 */


#include "synopsysUart.h"

//#define serial_out(x, y)	writel(x, y)
//#define serial_in(y)		readl(y)
#ifdef CONFIG_MACH_CYGNUS
	#define serial_out(x, y)	(*((unsigned long*)y))= x
	#define serial_in(y)		(*((unsigned long*)y))
#else
	#define serial_out(x, y)	(*((unsigned char*)y))= x
	#define serial_in(y)		(*((unsigned char*)y))
#endif

#ifndef CONFIG_SYS_SYNOPSYS_UART_IER
#define CONFIG_SYS_SYNOPSYS_UART_IER  0x00
#endif /* CONFIG_SYS_SysnopsysUart_IER */

#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */
#define UART_FCRVAL (UART_FCR_FIFO_EN |	\
		     UART_FCR_RXSR |	\
		     UART_FCR_TXSR)		/* Clear & enable FIFOs */

void synopsysUart_init(SynopsysUart_t com_port, int baud_divisor)
{
	serial_out(CONFIG_SYS_SYNOPSYS_UART_IER, &com_port->cd2.ier);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, (unsigned long)&com_port->lcr);
	serial_out(0, &com_port->cd1.dll);
	serial_out(0, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
	serial_out(UART_MCRVAL, &com_port->mcr);
	serial_out(UART_FCRVAL, &com_port->cd3.fcr);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, &com_port->lcr);
	serial_out(baud_divisor & 0xff, &com_port->cd1.dll);
	serial_out((baud_divisor >> 8) & 0xff, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
}

void synopsysUart_putc(const char c, SynopsysUart_t com_port)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_THRE) == 0);

	serial_out(c, &com_port->cd1.thr);
}

#ifndef CONFIG_SYNOPSYS_UART_MIN_FUNCTIONS
char synopsysUart_getc(SynopsysUart_t com_port)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_DR) == 0);

	return serial_in(&com_port->cd1.rbr);
}

int synopsysUart_tstc(SynopsysUart_t com_port)
{
	return (serial_in(&com_port->lsr) & UART_LSR_DR) != 0;
}

void synopsysUart_enterLoopback(SynopsysUart_t com_port)
{
	serial_out((serial_in(&com_port->mcr) |UART_MCR_LOOPBACK ), &com_port->mcr);
	serial_out(UART_FCRVAL, &com_port->cd3.fcr);

}

void synopsysUart_exitLoopback(SynopsysUart_t com_port)
{
	serial_out((serial_in(&com_port->mcr) & (~UART_MCR_LOOPBACK)) ,&com_port->mcr);
	serial_out(UART_FCRVAL, &com_port->cd3.fcr);
}

#endif /* CONFIG_SYNOPSYS_UART_MIN_FUNCTIONS */
