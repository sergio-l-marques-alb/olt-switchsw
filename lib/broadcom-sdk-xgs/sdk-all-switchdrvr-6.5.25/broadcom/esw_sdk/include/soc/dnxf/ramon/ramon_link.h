/*
 * $Id: ramon_link.h,v 1.1.2.1 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RAMON STAT H
 */

#ifndef _SOC_RAMON_LINK_H_
#define _SOC_RAMON_LINK_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 (Ramon) family only!"
#endif

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/error.h>
#include <soc/dnxc/error.h>

int soc_ramon_linkctrl_init(
    int unit);

#endif
