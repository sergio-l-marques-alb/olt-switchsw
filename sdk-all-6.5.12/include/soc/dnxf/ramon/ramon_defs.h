/*
 * $Id: ramon_defs.h,v 1.1.2.2 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RAMON DEFS H
 */
 
#ifndef _SOC_RAMON_DEFS_H_
#define _SOC_RAMON_DEFS_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnxf/ramon/ramon_intr.h>


#define SOC_RAMON_MAX_NUM_OF_PRIORITIES        (4)
    
#define SOC_RAMON_NOF_BLK                      128

#endif

