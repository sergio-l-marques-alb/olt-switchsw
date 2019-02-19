/*
 * $Id$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF INTERRUPT H
 */
 
#ifndef _SOC_DNXF_INTERRUPT_H_
#define _SOC_DNXF_INTERRUPT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

void soc_dnxf_block_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4);


#endif /*_SOC_DNXF_INTERRUPT_H_*/
