/*
 * $Id: port.h,v 1.2.10.11.2.8 Broadcom SDK $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        stat.h
 * Purpose:     STAT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_STAT_H_
#define   _BCM_INT_DNXF_STAT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

int dnxf_stat_init(int unit);
int dnxf_stat_deinit(int unit);

#endif /*_BCM_INT_DNXF_STAT_H_*/
