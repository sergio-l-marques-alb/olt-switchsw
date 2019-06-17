/*
* $Id$
* $Copyright: (c) 2018 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*
*/

#ifndef _SOC_DNX_DEBUG_H
#define _SOC_DNX_DEBUG_H

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

#include <soc/dnx/legacy/ARAD/arad_debug.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_error.h>

shr_error_e soc_dnx_dbg_egress_shaping_enable_get(const unsigned int unit, uint8 *enable);
shr_error_e soc_dnx_dbg_egress_shaping_enable_set(const unsigned int unit, const uint8 enable);
shr_error_e soc_dnx_compilation_vendor_valid(const unsigned int unit, const unsigned int val);

#ifndef _KERNEL_ 
#endif /*_KERNEL_*/
#endif /*_SOC_DNX_DEBUG_H*/
