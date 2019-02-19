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
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>

#include <soc/dpp/JER/jer_sch.h>

#define CAL_0_1_SIZE  1024
#define CAL_2_15_SIZE 256

int
  soc_jer_sch_init(
    SOC_SAND_IN   int                    unit
    )
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_jer_sch_e2e_interface_allocate(unit, port_i));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_sch_e2e_interface_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    )
{
    uint8           found = FALSE;
    uint32          num_of_big_calendars, num_of_channelized_interfaces, num_of_interfaces, e2e_if, tm_port, data_32, egress_offset, base_q_pair,
                    non_channelized_port_offset, is_master;
    int             is_channelized, core, start, end;
    soc_port_if_t   interface_type;
    soc_reg_above_64_val_t tbl_data;
    SOC_SAND_OCC_BM_PTR ifs_occ;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));

    if (is_master) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized)); 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

        num_of_channelized_interfaces = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces);
        num_of_big_calendars = SOC_DPP_DEFS_GET(unit, nof_big_channelized_calendars);
        num_of_interfaces = SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);

        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        end = 1;


        for (start = 0; start < end; start++) {
            ifs_occ = arad_sw_db_egr_e2e_interfaces_occ_get(unit, core);
            if (is_channelized) {
                /* Try to allocate big calendars for ILKN interface */
                if (interface_type == SOC_PORT_IF_ILKN) {
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(ifs_occ, 0, (num_of_big_calendars-1), TRUE, &e2e_if, &found));
                }

                if (!found) {
                    /* try to allocate all channelized if's ID's*/
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(ifs_occ, num_of_big_calendars, (num_of_channelized_interfaces-1), TRUE, &e2e_if, &found));
                }

                if (!found && interface_type != SOC_PORT_IF_ILKN) {
                    /* try to allocate all channelized if's ID's*/
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(ifs_occ, 0, (num_of_channelized_interfaces-1), TRUE, &e2e_if, &found));
                }
            } else { /* non channelized interface */

                /* try to allocate non channelized e2e if's ID's*/
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(ifs_occ, num_of_channelized_interfaces, (num_of_interfaces-1), TRUE, &e2e_if, &found));

                if (!found) {
                    /* try to allocate small channelized e2e if's ID's*/
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(ifs_occ, num_of_big_calendars, (num_of_interfaces-1), TRUE, &e2e_if, &found));
                }

                if (!found) {
                    /* try to allocate big channelized e2e if's ID's*/
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(ifs_occ, 0, (num_of_interfaces-1), TRUE, &e2e_if, &found));
                }

            }

            if (!found) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("No e2e interfaces are left for port %d \n"), port));
            } else {
                /* mark e2e interface as used */
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(ifs_occ, e2e_if, TRUE));

                /* Set HW SCH offset */
                data_32 = 0;
                SOC_REG_ABOVE_64_CLEAR(tbl_data);

                SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data_32));
                soc_SCH_FC_MAP_FCMm_field32_set(unit, &data_32 ,FC_MAP_FCMf, e2e_if);
                SOCDNX_IF_ERR_EXIT(WRITE_SCH_FC_MAP_FCMm(unit,SCH_BLOCK(unit, core), egress_offset, &data_32));

                if (e2e_if > num_of_channelized_interfaces) {
                    /* In case of non channelized port set also Port ID */
                    non_channelized_port_offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, e2e_if);
                    SOCDNX_IF_ERR_EXIT(READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));
                    soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_IDf,base_q_pair);
                    SOCDNX_IF_ERR_EXIT(WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));    
                }
            }

            /* handle ILKN dedicated mode */
            if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN && start == 0) {
                if (IS_TDM_PORT(unit, port)) {
                    egress_offset++;
                } else {
                    egress_offset--;
                }
                end++;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}


int 
soc_jer_sch_e2e_interface_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    )
{
    uint32          e2e_if, tm_port, data_32, egress_offset, non_channelized_port_offset, is_master, 
                    num_of_channelized_interfaces;
    int             is_channelized, core, start, end;
    soc_port_if_t   interface_type;
    soc_reg_above_64_val_t tbl_data;
    SOC_SAND_OCC_BM_PTR ifs_occ;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));

    if (is_master) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized)); 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

        num_of_channelized_interfaces = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces);
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        end = 1;

        for (start = 0; start < end; start++) {
            ifs_occ = arad_sw_db_egr_e2e_interfaces_occ_get(unit, core);

            data_32 = 0;
            SOC_REG_ABOVE_64_CLEAR(tbl_data);
            SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data_32));
            /* get e2e interface*/
            e2e_if = soc_SCH_FC_MAP_FCMm_field32_get(unit, &data_32, FC_MAP_FCMf);

            /* set HW e2e interface to 0 */
            soc_SCH_FC_MAP_FCMm_field32_set(unit, &data_32 ,FC_MAP_FCMf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_FC_MAP_FCMm(unit,SCH_BLOCK(unit, core), egress_offset, &data_32));

            /* mark e2e interface as free */
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(ifs_occ, e2e_if, FALSE));

            if (e2e_if > num_of_channelized_interfaces) {
                /* In case of non channelized port set also Port ID to 0 */
                non_channelized_port_offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, e2e_if);
                SOCDNX_IF_ERR_EXIT(READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));
                soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_IDf,0);
                SOCDNX_IF_ERR_EXIT(WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));    
            }

            /* handle ILKN dedicated mode */
            if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN && start == 0) {
                if (IS_TDM_PORT(unit, port)) {
                    egress_offset++;
                } else {
                    egress_offset--;
                }
                end++;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}

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
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
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
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, memory, SCH_BLOCK(unit, core_id), cal_offset+slot, &entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
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
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, memory, SCH_BLOCK(unit, core_id), cal_offset+slot, &entry));
        soc_mem_field_get(unit, memory, &entry, PORT_SELf, &(slots[slot]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

