/*
 * $Id: $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ
#include <shared/bsl.h>
#include <soc/dpp/drv.h>

#define CAL_0_1_SIZE  1024
#define CAL_2_15_SIZE 256

int
  soc_jer_sch_cal_max_size_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   )
{
    uint32 cal_select;
    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2;

    if(cal_select == 0 || cal_select == 1) {
        *max_cal_size = CAL_0_1_SIZE;
    } else {
        *max_cal_size = CAL_2_15_SIZE;
    }

    SOCDNX_FUNC_RETURN;
}

static soc_mem_t cal_memories[] = {
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_0m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_1m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_2m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_3m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_4m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_5m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_6m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_7m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_8m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_9m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_10m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_11m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_12m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_13m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_14m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_15m
};

int
  soc_jer_sch_cal_tbl_set(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   int                    core,
    SOC_SAND_IN   uint32                 sch_to_set /*A (0) or B (1)*/,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
  )
{
    uint32
        cal_offset,
        cal_select,
        cal_size,
        slot,
        entry;
    soc_mem_t
        memory;

    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2;

    if(cal_select == 0 || cal_select == 1) {
        cal_size = CAL_0_1_SIZE;
    } else {
        cal_size = CAL_2_15_SIZE;
    }

    if(slots_count > cal_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Slots count is out of range")));
    }

    /* Each table 0-15 contain 2 active calenders and two passive calendarts */
    cal_offset = cal_select * (2 * cal_size) + sch_to_set * cal_size;

    /* Write memory*/
    memory = cal_memories[cal_select];
    for (slot=0; slot < slots_count; slot++) {
        entry = 0;
        soc_mem_field_set(unit, memory, &entry, PORT_SELf, &(slots[slot]));
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, memory, SCH_BLOCK(unit, core), cal_offset+slot, &entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   int                    core,
    SOC_SAND_IN   uint32                 sch_to_set /*A (0) or B (1)*/,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
  )
{
    uint32
        cal_offset,
        cal_select,
        cal_size,
        slot,
        entry;
    soc_mem_t
        memory;

    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2;

    if(cal_select == 0 || cal_select == 1) {
        cal_size = CAL_0_1_SIZE;
    } else {
        cal_size = CAL_2_15_SIZE;
    }

    if(slots_count > cal_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Slots count is out of range")));
    }

    /* Each table 0-15 contain 2 active calenders and two passive calendarts */
    cal_offset = cal_select * (2 * cal_size) + sch_to_set * cal_size;

    /* Write memory*/
    memory = cal_memories[cal_select];
    for (slot=0; slot < slots_count; slot++) {
        entry = 0;
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, memory, SCH_BLOCK(unit, core), cal_offset+slot, &entry));
        soc_mem_field_get(unit, memory, &entry, PORT_SELf, &(slots[slot]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

