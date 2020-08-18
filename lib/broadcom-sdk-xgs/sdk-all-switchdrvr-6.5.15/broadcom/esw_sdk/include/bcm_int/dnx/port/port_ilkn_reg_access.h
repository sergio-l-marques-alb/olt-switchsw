/*
 * $Id: stat.h,v 1.2 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file contains STAT definitions internal to the BCM library.
 */

#ifndef __JER2_PORT_ILKN_REG_ACCESS_INCLUDED__
#define __JER2_PORT_ILKN_REG_ACCESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jer2) family only!"
#endif

int soc_jer2_ilkn_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data);
int soc_jer2_ilkn_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data);

int soc_jer2_ilkn_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data);
int soc_jer2_ilkn_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data);

int soc_jer2_ilkn_reg_above_64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);
int soc_jer2_ilkn_reg_above_64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);

#endif /* __JER2_JER_REG_ACCESS_INCLUDED__ */
