/*----------------------------------------------------------------------
 * $Id: tscomod_sc_lkup_table.c, $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
#ifndef tscomod_sc_lkup_table_H_
#define tscomod_sc_lkup_table_H_ 

#include "tscomod.h"
#include <phymod/phymod.h>

#define TSCOMOD_HW_SPEED_ID_TABLE_SIZE   64
#define TSCOMOD_HW_AM_TABLE_SIZE    64
#define TSCOMOD_HW_UM_TABLE_SIZE    64


#define TSCOMOD_SPEED_ID_TABLE_SIZE  97
#define TSCOMOD_SPEED_ID_ENTRY_SIZE  5
#define TSCOMOD_AM_TABLE_SIZE  64
#define TSCOMOD_AM_ENTRY_SIZE  3
#define TSCOMOD_UM_TABLE_SIZE  64
#define TSCOMOD_UM_ENTRY_SIZE  2

#define TSCOMOD_SPEED_PRIORITY_MAPPING_TABLE_SIZE 1
#define TSCOMOD_SPEED_PRIORITY_MAPPING_ENTRY_SIZE 9

typedef struct tscomod_sc_pmd_entry_t {
    int t_pma_os_mode;
    int pll_mode;
} tscomod_sc_pmd_entry_st;

extern const tscomod_sc_pmd_entry_st tscomod_sc_pmd_entry[];

extern int tscomod_get_mapped_speed(tscomod_spd_intfc_type_t spd_intf, int *speed);


#endif
