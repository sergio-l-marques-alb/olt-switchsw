/*
 * $Id: rx.h,v 1.2 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF FABRIC H
 */
#ifndef _BCM_INT_DNXF_RX_H_
#define _BCM_INT_DNXF_RX_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <sal/types.h>
#include <bcm/types.h>

int dnxf_rx_init(
    int unit);

int dnxf_rx_deinit(
    int unit);

#endif /*_BCM_INT_DNXF_RX_H_*/
