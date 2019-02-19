/*
 * $Id: socdiag.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2006, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 */
#include <sal/types.h>
#include "../../bde/ecos/ecosbde.h"

ibde_t *bde;

int
bde_create(void)
{	
    ecosbde_bus_t bus; 
    bus.base_addr_start = 0xA8000000;
    bus.int_line = 3;
#ifdef LE_HOST
    bus.be_pio = 0;
    bus.be_packet = 0;
    bus.be_other = 0;
#else
    bus.be_pio = 0;
    bus.be_packet = 1;
    bus.be_other = 0;
#endif
    return ecosbde_create(&bus, &bde);
}
