/*
 * serial_synopsys.c
 *
 *  Created on: 09-Apr-2013
 *      Author: Chandra Mohan C 
 *      Email : chandra@broadcom.com
 */

/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include "synopsysUart.h"
#include <asm/pgtable.h>
#include <mach/hardware.h>

#define CONFIG_CONS_INDEX 0
//#define CONFIG_SYS_SYNOPSYS_COM0 HW_IO_PHYS_TO_VIRT(0x18020000)

//#define CONFIG_SYS_SYNOPSYS_COM0 (HW_IO_PHYS_TO_VIRT(IPROC_CCA_UART0_REG_BASE))
#define CONFIG_SYS_SYNOPSYS_COM0 (IPROC_CCA_UART0_REG_BASE)

#define CONFIG_SYS_SYNOPSYS_CLK_DIV 1

#if CONFIG_CONS_INDEX == 0
#error	"Console port 0 defined but not configured."
#elif CONFIG_CONS_INDEX == !defined(CONFIG_SYS_SYNOPSYS_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CONFIG_SYS_SYNOPSYS_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CONFIG_SYS_SYNOPSYS_COM3)
#error	"Console port 3 defined but not configured."
#endif

static SynopsysUart_t serial_ports[4] = {
		(SynopsysUart_t)CONFIG_SYS_SYNOPSYS_COM0
		,0
		};

 inline void
_serial_puts(const char *s, const int port);

void uart0()
{
	//_serial_puts("uart0 from Linux Kernel", 0);
	char c[10]={
			'C'
			,'H'
			,'A'
			,'N'
			,'\n'
			,0
	};

	int i= 0;

	while(c[i])
	{
		synopsysUart_putc(c[i++], CONFIG_SYS_SYNOPSYS_COM0);
	}
}

static int calc_divisor() {
#if CONFIG_SYS_SYNOPSYS_CLK_DIV
	return CONFIG_SYS_SYNOPSYS_CLK_DIV;
#else
	return (CONFIG_SYS_UART_CLK / (gd->baudrate * 16));
#endif
}

int serial_init(void) {
	int clock_divisor;

#ifdef CONFIG_SYS_SYNOPSYS_COM0
	clock_divisor = calc_divisor();
	synopsysUart_init(serial_ports[0], clock_divisor);
#endif

#ifdef CONFIG_SYS_SYNOPSYS_COM1
	clock_divisor = calc_divisor();
	synopsysUart_init(serial_ports[1], clock_divisor);
#endif

#ifdef CONFIG_SYS_SYNOPSYS_COM2
	clock_divisor = calc_divisor();
	synopsysUart_init(serial_ports[2], clock_divisor);
#endif

#ifdef CONFIG_SYS_SYNOPSYS_COM3
	clock_divisor = calc_divisor();
	synopsysUart_init(serial_ports[3], clock_divisor);
#endif

	return (0);
}

 inline void
_serial_putc(const char c, const int port)
{
	if (c == '\n')
	synopsysUart_putc('\r', serial_ports[port]);

	synopsysUart_putc(c, serial_ports[port]);
}

void serial_putc(const char c) {
	if (c == '\n')
		synopsysUart_putc('\r', serial_ports[CONFIG_CONS_INDEX]);

	synopsysUart_putc(c, serial_ports[CONFIG_CONS_INDEX]);
}

 inline void
_serial_putc_raw(const char c, const int port)
{
	synopsysUart_putc(c, serial_ports[port]);
}

void serial_putc_raw(const char c) {
	_serial_putc_raw(c, CONFIG_CONS_INDEX);
}

 inline void
_serial_puts(const char *s, const int port)
{
	while (*s)
	{
		_serial_putc(*s++, port);
	}
}

void serial_puts(const char *s) {
	while (*s) {
		_serial_putc(*s++, CONFIG_CONS_INDEX);
	}
}

 inline int
_serial_getc(const int port)
{
	return synopsysUart_getc(serial_ports[port]);
}

int serial_getc(void) {
	return _serial_getc(CONFIG_CONS_INDEX);
}

 inline int
_serial_tstc(const int port)
{
	return synopsysUart_tstc(serial_ports[port]);
}

int serial_tstc(void) {
	return _serial_tstc(CONFIG_CONS_INDEX);
}
