/*
 * $Id: util.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        util.c
 *
 * Provides:
 *     pcid_x2i
 *     pcid_type_name
 *
 * Requires: types and memory type enumeration from verinet.h
 *     
 */

#include <sys/types.h>
#include <soc/types.h>
#include <bde/pli/verinet.h>
#include <string.h>

int
pcid_x2i(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}


/*
 * type_name
 */

void pcid_type_name(char *buffer, uint32 type)
{
    switch (type) {
    case PCI_CONFIG:
	strcpy(buffer, "PCI CONFIG");
	break;
    case PCI_MEMORY:
	strcpy(buffer, "PCI MEMORY");
	break;
    case I2C_CONFIG:
	strcpy(buffer, "I2C CONFIG");
	break;
    case PLI_CONFIG:
	strcpy(buffer, "PLI CONFIG");
	break;
    case JTAG_CONFIG:
	strcpy(buffer, "JTAG");
	break;
    default:
	strcpy(buffer, "UNKNOWN");
	break;
    }
}
