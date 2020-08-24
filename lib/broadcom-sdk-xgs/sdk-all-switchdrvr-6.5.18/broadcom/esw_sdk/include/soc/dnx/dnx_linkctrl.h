/*
 * $Id: dnx_linkctrl.h,v 1.1.2.1 2013/09/10 07:35:39 Exp $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNX LINKCTRL H
 */

#ifndef _SOC_DNX_LINKCTRL_H_
#define _SOC_DNX_LINKCTRL_H_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

#include <soc/error.h>

extern int soc_dnx_linkctrl_init(
    int unit);

#endif
