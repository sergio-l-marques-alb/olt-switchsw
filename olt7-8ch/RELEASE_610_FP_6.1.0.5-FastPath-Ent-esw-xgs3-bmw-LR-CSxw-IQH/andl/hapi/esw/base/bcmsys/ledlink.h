/*
 * $Id: ledlink.h,v 1.1 2011/04/18 17:10:17 mruas Exp $
 * $Copyright: (c) 2002, 2003 Broadcom Corp.
 * All Rights Reserved.$
 */

#ifndef _BCM_SYS_LEDLINK_H
#define _BCM_SYS_LEDLINK_H

#include <soc/drv.h>
#include <bcm/port.h>
#include <bcm/link.h>

extern void hpc_default_led_linkscan_cb(int unit, soc_port_t port, 
                                        bcm_port_info_t *info);

#endif /* _BCM_SYS_LEDLINK_H */
