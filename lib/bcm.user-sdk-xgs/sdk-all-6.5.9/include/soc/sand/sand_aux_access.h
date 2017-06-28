/*
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    access_pack.h
 * Purpose: Miscellaneous routine for device db access
 */

#ifndef   _SOC_SAND_AUX_ACCESS_H_
#define   _SOC_SAND_AUX_ACCESS_H_

#include <shared/utilex/utilex_rhlist.h>

#define ACC_NO_READ     0x01
#define ACC_NO_WB       0x02

typedef struct
{
    int flags;
} shr_reg_data_t;

typedef struct
{
    int flags;
} shr_mem_data_t;

typedef struct
{
    rhentry_t   entry;
    int         opcode;
    uint32      address;
    int         offset;
    int         cmic_block_id;
    int         block_id;
    int         count;
    char        block_n[RHNAME_MAX_SIZE];
} shr_hit_entry_t;

char*
shr_access_get_sand_name(
        int unit);

shr_error_e
shr_access_device_init(
    int unit);

shr_error_e
shr_access_device_deinit(
    int unit);

int
shr_access_reg_no_read_get(
        int         unit,
        soc_reg_t   reg);

int
shr_access_mem_no_read_get(
        int         unit,
        soc_mem_t   reg);

int
shr_access_reg_no_wb_get(
        int         unit,
        soc_reg_t   reg);

int
shr_access_mem_no_wb_get(
        int         unit,
        soc_mem_t   mem);

shr_error_e
shr_access_mem_present_in_block(
    int                unit,
    int                cmic_block,
    soc_mem_t          mem,
    int*               block_id_p);

shr_error_e
shr_access_reg_present_in_block(
    int                unit,
    int                cmic_block,
    soc_block_types_t  reg_block_list,
    int*               block_id_p);

#define HIT_AVOID_REPEAT         0x01

shr_error_e
shr_access_hit_get(
    int unit,
    rhlist_t ** hit_list_p,
    int flags);

char *shr_access_hit_opcode_name(
    int opcode);

#endif /* _SOC_SAND_AUX_ACCESS_H_ */
