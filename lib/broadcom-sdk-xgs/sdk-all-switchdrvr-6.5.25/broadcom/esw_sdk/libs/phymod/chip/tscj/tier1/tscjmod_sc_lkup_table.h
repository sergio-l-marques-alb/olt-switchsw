/*----------------------------------------------------------------------
 * $Id: tscjmod_sc_lkup_table.c, $
 * $Copyright: (c) 2021 Broadcom.
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
#ifndef tscjmod_sc_lkup_table_H_
#define tscjmod_sc_lkup_table_H_ 

#include "tsco/tier1/tscomod.h"

#include <phymod/phymod.h>

#define TSCJMOD_HW_SPEED_ID_TABLE_SIZE   64
#define TSCJMOD_HW_AM_TABLE_SIZE    64
#define TSCJMOD_HW_UM_TABLE_SIZE    64


#define TSCJMOD_SPEED_ID_TABLE_SIZE  97
#define TSCJMOD_SPEED_ID_ENTRY_SIZE  5
#define TSCJMOD_AM_TABLE_SIZE  64
#define TSCJMOD_AM_ENTRY_SIZE  3
#define TSCJMOD_UM_TABLE_SIZE  64
#define TSCJMOD_UM_ENTRY_SIZE  2

#define TSCJMOD_SPEED_PRIORITY_MAPPING_TABLE_SIZE 1
#define TSCJMOD_SPEED_PRIORITY_MAPPING_ENTRY_SIZE 9


extern int tscomod_get_mapped_speed(tscomod_spd_intfc_type_t spd_intf, int *speed);
extern uint32_t* tscj_spd_id_entry_53_get(void);
extern uint32_t* tscj_am_table_entry_get(void);
extern uint32_t* tscj_um_table_entry_get(void);
extern uint32_t* tscj_speed_priority_mapping_table_get(void);
extern uint32_t* tscj_spd_id_entry_53_gsh_get(void);
extern uint32_t*  tscj_spd_id_entry_100g_4lane_no_fec_53_get(void);
#endif
