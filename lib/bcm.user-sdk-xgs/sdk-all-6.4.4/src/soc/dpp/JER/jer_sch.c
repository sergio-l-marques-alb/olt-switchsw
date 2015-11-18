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
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_scheduler_flow_converts.h>

#include <soc/dpp/JER/jer_sch.h>
#include <soc/dpp/JER/jer_tbls.h>

#define CAL_0_1_SIZE  1024
#define CAL_2_15_SIZE 256
#define JER_SCH_HR_PRIORITY_LOW_VAL  (0)
#define JER_SCH_HR_PRIORITY_HIGH_VAL (15)

int
  soc_jer_sch_init(
    SOC_SAND_IN   int                    unit
    )
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 reg_val, flags, rate_kbps, slow_level, slow_type, mem_val32;
    int core;
    uint32 slow_rate_max_level;
    char* slow_rate_max_level_str;

    SOCDNX_INIT_FUNC_DEFS;

    /* allocate egq to e2e interface mapping */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
        if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {
            SOCDNX_IF_ERR_EXIT(soc_jer_sch_e2e_interface_allocate(unit, port_i));
        }
    }

    /* init e2e registers */
    SOCDNX_SAND_IF_ERR_EXIT(arad_scheduler_end2end_regs_init(unit));


    /* Set default values for priority propegation */
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) 
    {
        /* 0 for low prio, 1 for high prio*/
        SOCDNX_IF_ERR_EXIT(READ_SCH_HR_PRIORITY_MASKr(unit, core, 0, &reg_val));
        soc_reg_field_set(unit, SCH_HR_PRIORITY_MASKr, &reg_val, HR_PRIORITY_MASK_Nf, JER_SCH_HR_PRIORITY_LOW_VAL);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_HR_PRIORITY_MASKr(unit, core, 0, reg_val));
        
        SOCDNX_IF_ERR_EXIT(READ_SCH_HR_PRIORITY_MASKr(unit, core, 1, &reg_val));
        soc_reg_field_set(unit, SCH_HR_PRIORITY_MASKr, &reg_val, HR_PRIORITY_MASK_Nf, JER_SCH_HR_PRIORITY_HIGH_VAL);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_HR_PRIORITY_MASKr(unit, core, 1, reg_val)); 

    }

    /* Enable slow factor */
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &reg_val, SLOW_FACTOR_ENABLEf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, reg_val));
    }

    slow_rate_max_level_str = soc_property_get_str(unit, spn_SLOW_MAX_RATE_LEVEL);

    if ( (slow_rate_max_level_str == NULL) || (sal_strcmp(slow_rate_max_level_str, "LOW")==0) )
    {
        slow_rate_max_level = 1;
    } 
    else if (sal_strcmp(slow_rate_max_level_str, "NORMAL") == 0)
    {
        slow_rate_max_level = 2;
    } 
    else if (sal_strcmp(slow_rate_max_level_str, "HIGH") == 0)
    {
        slow_rate_max_level = 3;
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Unsupported properties: slow_rate_max_level should be LOW/NORMAL/HIGH")));
    }

    /* Write slow rate max level */
    SOCDNX_IF_ERR_EXIT(READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, &reg_val));
    soc_reg_field_set(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, &reg_val, SLOW_MAX_BUCKET_WIDTHf, slow_rate_max_level);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, reg_val));
    
    mem_val32 = 0;
    soc_mem_field_set(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, MAX_BUCKETf, &slow_rate_max_level);

    for(slow_level=0 ; slow_level<8 ; slow_level++) {
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), slow_level, &mem_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), slow_level, &mem_val32));
    }
 
    for(slow_level=0 ; slow_level<8 ; slow_level++) {

        rate_kbps = (100000000*(slow_level+1))/8;

        for(slow_type=1 ; slow_type<=2 ; slow_type++) {
            SOCDNX_IF_ERR_EXIT(jer_sch_slow_max_rates_per_level_set(unit, SOC_CORE_ALL, slow_level, slow_type, rate_kbps));
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}
/*********************************************************************
*     This function gets an entry in the device rate table, per core.
*     This function gets an entry in the device rate table.
*     Each entry contains a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links.
*     This procedure is only useful for multi-core architectures.
*     The driver reads from the following tables:
*     Device Rate Memory (DRM)
*     For Jericho, this table is SCH_DEVICE_RATE_MEMORY_DRMm
*     Details: in the H file. (search for prototype)
*     Note that validity of input parameters is assumed to have
*     been checked by calling procedures.
* See also:
*   arad_sch_device_rate_entry_get_unsafe()
*********************************************************************/
uint32
  jer_sch_device_rate_entry_core_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_OUT uint32              *rate
  )
{
  uint32
    interval_in_clock_128_th,
    calc,
    offset,
  credit_worth,
    res;
  ARAD_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_RATE_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rate);

  /*
   * Read from DRM table
   */
  offset = (nof_active_links_ndx * SOC_DPP_DEFS_GET(unit, nof_rci_levels)) + rci_level_ndx;
  res = jer_sch_drm_tbl_get_unsafe(
          unit, core,
          offset,
          &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  interval_in_clock_128_th = drm_tbl_data.device_rate;
  /*
   *  Calculate Device Credit Rate Generation (in Mbits/secs) according to:
   *
   *
   *                       Credit [bits] * Num_of_Mega_clocks_128th_in_sec [(M * clocks)/(128 * sec)]
   *  Rate [Mbits/Sec] =   -----------------------------------------------------------------------
   *                          interval_between_credits_in_clock_128th [clocks/128]
   */
  if (0 == interval_in_clock_128_th)
  {
    *rate = 0;
  }
  else
  {
    /*
     * Get 'credit worth' value using device driver specific procedure.
     */
    res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, &credit_worth))) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (arad_chip_mega_ticks_per_sec_get(unit) * ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, interval_in_clock_128_th);
    *rate = calc;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_sch_device_rate_entry_core_get_unsafe()",0,0);
}
/*********************************************************************
*     This function sets an entry in the device rate table, per core.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links.
*     This procedure is only useful for multi-core architectures.
*     The driver writes to the following tables:
*     Device Rate Memory (DRM)
*     For Jericho, this table is SCH_DEVICE_RATE_MEMORY_DRMm
*     Details: in the H file. (search for prototype)
*     Note that validity of input parameters is assumed to have
*     been checked by calling procedures.
*
* See also:
*   arad_sch_device_rate_entry_set_unsafe
*********************************************************************/
uint32
  jer_sch_device_rate_entry_core_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_IN  uint32              rate
  )
{
  uint32
    interval_in_clock_128_th,
    calc,
    offset,
    credit_worth,
    res;
  ARAD_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_RATE_ENTRY_SET_UNSAFE);
  /*
   * Get 'credit worth' value using device driver specific procedure.
   */
  res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, &credit_worth))) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);
  /*
   *  Calculate interval between credits (in Clocks/64) according to:
   *
   *
   *                       Credit [bits] * Num_of_Mega_clocks_64th_in_sec [(M * clocks)/(64 * sec)]
   *  Rate [Mbits/Sec] =   -----------------------------------------------------------------------
   *                          interval_between_credits_in_clock_64th [clocks/64]
   */
  if (0 == rate)
  {
    interval_in_clock_128_th = 0;
  }
  else
  {
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (arad_chip_mega_ticks_per_sec_get(unit) * ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, rate);
    interval_in_clock_128_th = calc;
  }
  SOC_SAND_LIMIT_FROM_ABOVE(interval_in_clock_128_th, ARAD_SCH_DEVICE_RATE_INTERVAL_MAX);
  if (interval_in_clock_128_th != 0)
  {
    SOC_SAND_LIMIT_FROM_BELOW(interval_in_clock_128_th, ARAD_SCH_DEVICE_RATE_INTERVAL_MIN);
  }
  offset = (nof_active_links_ndx * SOC_DPP_DEFS_GET(unit, nof_rci_levels)) + rci_level_ndx;
  drm_tbl_data.device_rate = interval_in_clock_128_th;
  /*
   * Write indirect to DRM table, single entry
   */
  res = 0 ;
  res = jer_sch_drm_tbl_set_unsafe(
          unit, core,
          offset, &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_sch_device_rate_entry_core_set_unsafe()",0,0);
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

            /* The core can't be negative... */
            /* coverity[negative_returns:FALSE] */
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

                if (!ARAD_SCH_IS_CHNIF_ID(unit, e2e_if)) {
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
            {
                uint32 array_index, bit_index, sizeof_element ;

                /*
                 * Update bit 'e2e_if' to '0' (open nif, do not force pause)
                 */
                sizeof_element = 32 ;
                array_index = e2e_if / sizeof_element ;
                bit_index = e2e_if % sizeof_element ;
                SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_NIF_CONFIGr(unit, core, array_index, &data_32)) ;
                data_32 &= SOC_SAND_RBIT(bit_index) ;
                WRITE_SCH_DVS_NIF_CONFIGr(unit, core, array_index, data_32) ;
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
    uint32          e2e_if, tm_port, data_32, egress_offset, non_channelized_port_offset, is_master;
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

        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        end = 1;

        for (start = 0; start < end; start++) {

            /* The core can't be negative... */
            /* coverity[negative_returns:FALSE] */
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

            if (ARAD_SCH_IS_CHNIF_ID(unit, e2e_if)) {
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
            {
                uint32 array_index, bit_index, sizeof_element ;

                /*
                 * update bit 'e2e_if' to '1' (close nif, force pause)
                 */
                sizeof_element = 32 ;
                array_index = e2e_if / sizeof_element ;
                bit_index = e2e_if % sizeof_element ;
                SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_NIF_CONFIGr(unit, core, array_index, &data_32)) ;
                data_32 |= SOC_SAND_BIT(bit_index) ;
                WRITE_SCH_DVS_NIF_CONFIGr(unit, core, array_index, data_32) ;
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
        cal_internal_select,
        cal_size,
        slot,
        entry;
    soc_mem_t
        memory;

    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2; /* selects the right dual calendar memory */
    cal_internal_select = sch_offset % 2; /* selects the right calendar within the dual calendar memory */

    if(cal_select == 0 || cal_select == 1) {
        cal_size = CAL_0_1_SIZE;
    } else {
        cal_size = CAL_2_15_SIZE;
    }

    if(slots_count > cal_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Slots count is out of range")));
    }

    /* Each table 0-15 contain 2 active calenders and two passive calendarts */
    cal_offset = cal_internal_select * (2 * cal_size) + sch_to_set * cal_size;

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
        cal_internal_select,
        cal_size,
        slot,
        entry;
    soc_mem_t
        memory;

    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2; /* selects the right dual calendar memory */
    cal_internal_select = sch_offset % 2; /* selects the right calendar within the dual calendar memory */

    if(cal_select == 0 || cal_select == 1) {
        cal_size = CAL_0_1_SIZE;
    } else {
        cal_size = CAL_2_15_SIZE;
    }

    if(slots_count > cal_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Slots count is out of range")));
    }

    /* Each table 0-15 contain 2 active calenders and two passive calendarts */
    cal_offset = cal_internal_select * (2 * cal_size) + sch_to_set * cal_size;

    /* Read memory*/
    memory = cal_memories[cal_select];
    for (slot=0; slot < slots_count; slot++) {
        entry = 0;
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, memory, SCH_BLOCK(unit, core_id), cal_offset+slot, &entry));
        soc_mem_field_get(unit, memory, &entry, PORT_SELf, &(slots[slot]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_enable_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int enable
    )
{
    uint32 reg_val;
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    /* for now - write the same value to cores 0/1 */
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) 
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &reg_val, INTERFACE_PRIORITY_PROPAGATION_ENABLEf, enable? 1:0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_enable_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  int* enable
    )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    /* for now - get value from core 0 */

    SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, 0, &reg_val));
    *enable = soc_reg_field_get(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, reg_val, INTERFACE_PRIORITY_PROPAGATION_ENABLEf);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_port_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_IN  int is_high_prio
   )
{
    uint32 mem_val, base_q_pair, tm_port;
    int core, offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    offset = base_q_pair + cosq;
    if (offset < 0 || offset > 255)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid offset for port hr %d \n"), offset));
    }

    SOCDNX_IF_ERR_EXIT(READ_SCH_HR_SCHEDULER_CONFIGURATION_SHCm(unit, SCH_BLOCK(unit, core), offset, &mem_val));
    soc_mem_field32_set(unit, SCH_HR_SCHEDULER_CONFIGURATION_SHCm, &mem_val,HR_PRIORITY_MASK_SELECTf, is_high_prio);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_HR_SCHEDULER_CONFIGURATION_SHCm(unit, SCH_BLOCK(unit, core), offset, &mem_val)); 


exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_OUT  int *is_high_prio
   )
{
    uint32 mem_val, base_q_pair, field_val, tm_port;
    int offset, core;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    offset = base_q_pair + cosq;
    if (offset < 0 || offset > 255)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid offset for port hr %d \n"), offset));
    }
       
    SOCDNX_IF_ERR_EXIT(READ_SCH_HR_SCHEDULER_CONFIGURATION_SHCm(unit, SCH_BLOCK(unit, core), offset, &mem_val));
    soc_mem_field_get(unit, SCH_HR_SCHEDULER_CONFIGURATION_SHCm, &mem_val, HR_PRIORITY_MASK_SELECTf, &field_val);

    *is_high_prio = field_val;
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_sch_slow_max_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Legacy API */
    SOCDNX_IF_ERR_EXIT(jer_sch_slow_max_rates_per_level_set(unit, SOC_CORE_ALL, 0, slow_rate_type, slow_rate_val));

exit:
    SOCDNX_FUNC_RETURN; 
   
}

uint32
jer_sch_slow_max_rates_per_level_set(
   SOC_SAND_IN int unit, 
   SOC_SAND_IN int core, 
   SOC_SAND_IN int level , 
   SOC_SAND_IN int slow_rate_type, 
   SOC_SAND_IN int slow_rate_val)
{
    soc_field_info_t peak_rate_man_fld, peak_rate_exp_fld;
    ARAD_SCH_SUBFLOW sub_flow;
    ARAD_SCH_INTERNAL_SUB_FLOW_DESC internal_sub_flow;
    uint32 mem_val32, slow_fld_val;

    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_SCH_SUBFLOW_clear(&sub_flow);

    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

    sub_flow.shaper.max_rate = slow_rate_val;

    SOCDNX_SAND_IF_ERR_EXIT(arad_sch_to_internal_subflow_shaper_convert(unit, &sub_flow, &internal_sub_flow, TRUE));

    slow_fld_val = 0;
    slow_fld_val |= ARAD_FLD_IN_PLACE(internal_sub_flow.peak_rate_exp, peak_rate_exp_fld);
    slow_fld_val |= ARAD_FLD_IN_PLACE(internal_sub_flow.peak_rate_man, peak_rate_man_fld);

    if (slow_rate_type == 1)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_set(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, SLOW_RATEf, &slow_fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_set(unit, SCH_SLOW_SCALE_B_SSBm, &mem_val32, SLOW_RATEf, &slow_fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
    }
    
exit:
    SOCDNX_FUNC_RETURN;

}




uint32
  jer_sch_slow_max_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int      *slow_rate_val
  )
{
   
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_sch_slow_max_rates_per_level_get(unit, 0, 0, slow_rate_type, slow_rate_val));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_sch_slow_max_rates_per_level_get(
   SOC_SAND_IN  int   unit,
   SOC_SAND_IN  int   core,
   SOC_SAND_IN  int   level,
   SOC_SAND_IN  int   slow_rate_type,
   SOC_SAND_OUT int*  slow_rate_val)
{
    uint32 slow_fld_val, mem_val32;
    ARAD_SCH_SUBFLOW sub_flow;
    ARAD_SCH_INTERNAL_SUB_FLOW_DESC internal_sub_flow;
    soc_field_info_t peak_rate_man_fld, peak_rate_exp_fld;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(slow_rate_val);

    /* These values are accesed but have no influence on the max rates*/
    internal_sub_flow.max_burst = 0;
    internal_sub_flow.slow_rate_index = 0;

    /*
    * The rate register value is interpreted like \{PeakRateExp,
    * PeakRateMan\} in the SHDS table.
    * Get the fields database for the interpretation.
    */
    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

    if (slow_rate_type == 1)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_get(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, SLOW_RATEf, &slow_fld_val);
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_get(unit, SCH_SLOW_SCALE_B_SSBm, &mem_val32, SLOW_RATEf, &slow_fld_val);
    }

    internal_sub_flow.peak_rate_exp = ARAD_FLD_FROM_PLACE(slow_fld_val, peak_rate_exp_fld);
    internal_sub_flow.peak_rate_man = ARAD_FLD_FROM_PLACE(slow_fld_val, peak_rate_man_fld);

    /*
    * The slow setting is equivalent to the SHDS setting.
    */
    SOCDNX_SAND_IF_ERR_EXIT(arad_sch_from_internal_subflow_shaper_convert(unit, &internal_sub_flow, &sub_flow));

    *slow_rate_val = sub_flow.shaper.max_rate;

exit:
    SOCDNX_FUNC_RETURN;

}


#undef _ERR_MSG_MODULE_NAME

