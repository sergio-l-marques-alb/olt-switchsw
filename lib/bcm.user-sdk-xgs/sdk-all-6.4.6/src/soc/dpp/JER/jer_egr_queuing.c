/*
 * $Id: $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS
#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>


typedef struct soc_jer_egr_if_s {
    uint32 egr_if_in_use[SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)];
} soc_jer_egr_if_t;

STATIC soc_jer_egr_if_t jer_egr_if[SOC_MAX_NUM_DEVICES][SOC_DPP_DEFS_MAX(NOF_CORES)];

int 
soc_jer_egr_interface_init(int unit) 
{
    soc_pbmp_t ports_bm;
    int core;
    uint32 egr_if;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        for(egr_if=0 ; egr_if<SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES) ; egr_if++) {
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = 0;
        }
    }

    if (SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

        SOC_PBMP_ITER(ports_bm, port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, port, &egr_if));

            if(egr_if == INVALID_EGR_INTERFACE) {
                continue;
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = jer_egr_if[unit][core].egr_if_in_use[egr_if] + 1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_egr_interface_alloc(int unit, soc_port_t port) 
{
    int core, is_channelized, rate_100g_or_above;
    uint32 rate_mps, egr_if = 0, other_egr_if = 0;
    soc_port_if_t interface_type;
    soc_port_t master_port;
    int start, end, i;
    uint32     offset;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    if(master_port == port) { /* First port in interface */
         /* Allocate egress interface */
         switch(interface_type) {
            case SOC_PORT_IF_CPU:
                egr_if = SOC_JER_EGR_IF_CPU;
                break;

            case SOC_PORT_IF_OLP:
                egr_if = SOC_JER_EGR_IF_OLP;
                break;

            case SOC_PORT_IF_OAMP:
                egr_if = SOC_JER_EGR_IF_OAMP;
                break;

            case SOC_PORT_IF_RCY:
                egr_if = SOC_JER_EGR_IF_RCY;
                break;

            case SOC_PORT_IF_ILKN:
                /* ILKN dedicated mode requiries 2 egress interfaces per ILKN - validate both are free */
                other_egr_if = 0;
                if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
                    if (IS_TDM_PORT(unit, port)) {
                        if (offset == 0) {
                            egr_if = 0;
                            other_egr_if = 1;
                        } else if (offset == 1) {
                            egr_if = 16;
                            other_egr_if = 17;
                        } else {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface, only 2 ILKN interfaces are supported per core in ILKN dedicated mode")));
                        }
                    } else {
                        if (offset == 0){
                            egr_if = 1;
                            other_egr_if = 0;
                        } else if (offset == 1) {
                            egr_if = 17;
                            other_egr_if = 16;
                        } else {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface, only 2 ILKN interfaces are supported per core in ILKN dedicated mode")));
                        }
                    }

                    if (jer_egr_if[unit][core].egr_if_in_use[other_egr_if] != 0) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface, 2 interfaces are needed in ILKN dedicated mode")));
                    }
                }
             break;

             default:
                egr_if = INVALID_EGR_INTERFACE;
                break;
        }

        if(egr_if == INVALID_EGR_INTERFACE) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &rate_mps));
            rate_100g_or_above = (rate_mps >= 100000 ? 1 : 0);

            /* First search in non-channalized\low-speed range*/
            if(!rate_100g_or_above && !is_channelized) {
                start = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces);
                end = SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);

                for(i=start ; i<end ; i++) {
                    if(jer_egr_if[unit][core].egr_if_in_use[i] == 0) {
                        egr_if = i;
                        break;
                    }
                }
            }

            /* if not found search in channalized\high speed range*/
            if(egr_if == INVALID_EGR_INTERFACE) {
                start = 0;
                end = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces) - SOC_DPP_IMP_DEFS_GET(unit, nof_special_interfaces);

                for(i=start ; i<end ; i++) {
                    /* reserve these egr_if id's for ILKN port */
                    if ((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && (i == 0 || i == 1 || i == 16 || i== 17)) {
                        continue;
                    }
                    if(jer_egr_if[unit][core].egr_if_in_use[i] == 0) {
                        egr_if = i;
                        break;
                    }
                }
            }

            if(egr_if == INVALID_EGR_INTERFACE) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface for port %d"), port));
            }
        }
    } else {
        /* channalized interface - get egr_if of master port */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, master_port, &egr_if));
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN) {
            if (IS_TDM_PORT(unit, master_port) && !IS_TDM_PORT(unit, port)) {
                egr_if++;
            }
            if (!IS_TDM_PORT(unit, master_port) && IS_TDM_PORT(unit, port)) {
                egr_if--;
            }
        }
    }

    jer_egr_if[unit][core].egr_if_in_use[egr_if] = jer_egr_if[unit][core].egr_if_in_use[egr_if] + 1;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_set(unit, port, egr_if));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_egr_interface_free(int unit, soc_port_t port) 
{
    uint32 egr_if;
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, port, &egr_if));

    if(egr_if != INVALID_EGR_INTERFACE) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        jer_egr_if[unit][core].egr_if_in_use[egr_if] = jer_egr_if[unit][core].egr_if_in_use[egr_if] - 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_egr_port2egress_offset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *egr_if
  )
{
    soc_port_t
        port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, port, egr_if));

exit:
    SOCDNX_FUNC_RETURN;
}

int
    soc_jer_egr_is_channelized(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_OUT uint32                *is_channalzied)
{
    uint32 reg_val[1], egr_if;
    SOCDNX_INIT_FUNC_DEFS;

    /*Get channel arbiter*/
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));

    /*Only first 32 interfaces might be channelized*/
    if(egr_if < 32 /*Doron TBD - replace with defines tool*/) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_IFC_IS_CHANNELIZEDr(unit, core, reg_val));
        if(SHR_BITGET(reg_val, egr_if)) {
            *is_channalzied = 1;
        } else {
            *is_channalzied = 0;
        }
    } else {
        *is_channalzied = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_egr_nrdy_th_profile_data_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 profile_data)
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    data = 0;
    
    if (profile <= 2) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_0_2r(unit, core, &data));
        if (profile == 0) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_0f, profile_data);
        } else if (profile == 1) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_1f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_2f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_0_2r(unit, core, data));
    }

    else if (profile >= 3 && profile <= 5) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_3_5r(unit, core, &data));
        if (profile == 3) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_3f, profile_data);
        } else if (profile == 4) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_4f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_5f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_3_5r(unit, core, data));
    }

    else if (profile >= 6 && profile <= 8) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_6_8r(unit, core, &data));
        if (profile == 6) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_8r, &data, NRDY_TH_6f, profile_data);
        } else if (profile == 7) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_8r, &data, NRDY_TH_7f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_8r, &data, NRDY_TH_8f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_6_8r(unit, core, data));
    }

    else if (profile >= 9 && profile <= 11) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_9_11r(unit, core, &data));
        if (profile == 9) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_9_11r, &data, NRDY_TH_9f, profile_data);
        } else if (profile == 10) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_9_11r, &data, NRDY_TH_10f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_9_11r, &data, NRDY_TH_11f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_9_11r(unit, core, data));
    }

    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Invalid profile id %d "), profile));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_egr_nrdy_th_profile_data_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_OUT uint32 *profile_data)
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    data = 0;
    
    if (profile <= 2) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_0_2r(unit, core, &data));
        if (profile == 0) {
            *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_0_2r, data, NRDY_TH_0f);
        } else if (profile == 1) {
            *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_0_2r, data, NRDY_TH_1f);
        } else {
            *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_0_2r, data, NRDY_TH_2f);
        }
    }

    else if (profile >= 3 && profile <= 5) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_3_5r(unit, core, &data));
        if (profile == 3) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_3_5r, data, NRDY_TH_3f);
        } else if (profile == 4) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_3_5r, data, NRDY_TH_4f);
        } else {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_3_5r, data, NRDY_TH_5f);
        }
    }

    else if (profile >= 6 && profile <= 8) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_6_8r(unit, core, &data));
        if (profile == 6) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_6_8r, data, NRDY_TH_6f);
        } else if (profile == 7) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_6_8r, data, NRDY_TH_7f);
        } else {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_6_8r, data, NRDY_TH_8f);
        }
    }

    else if (profile >= 9 && profile <= 11) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_9_11r(unit, core, &data));
        if (profile == 9) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_9_11r, data, NRDY_TH_9f);
        } else if (profile == 10) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_9_11r, data, NRDY_TH_10f);
        } else {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_9_11r, data, NRDY_TH_11f);
        }
    }

    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Invalid profile id %d "), profile));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_egr_tm_init(int unit)
{   
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 tm_port, base_q_pair, priority_mode, is_master, tdm_egress_priority;
    uint32 reg_val, fld_val, flags;
    SOC_TMC_EGR_PORT_SHAPER_MODE shaper_mode;
    int core;
    soc_port_if_t interface_type;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_reg_above_64_val_t reg_val_above_64, fld_val_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    /* Regs init */
    reg_val = 0;
    soc_reg_field_set(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, &reg_val, PQP_CALENDER_SWITCH_ENf, 1);
    soc_reg_field_set(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, &reg_val, FQP_CALENDER_SWITCH_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PQP_AND_FQP_CALENDER_SETTINGr(unit, SOC_CORE_ALL, reg_val));

    SOCDNX_IF_ERR_EXIT(arad_egr_queuing_init(unit, SOC_DPP_CONFIG(unit)->arad->init.eg_cgm_scheme));

    /* In case ERP port is enabled, search for unoccupied NIF interface */
    /*SOCDNX_IF_ERR_EXIT(arad_ports_init_interfaces_erp_setting(unit, &(SOC_DPP_CONFIG(unit)->arad->init.ports));*/

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

    SOC_PBMP_ITER(pbmp, port_i) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
        if (SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags)) {
            continue;
        }
       
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port_i, &base_q_pair));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &priority_mode));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_shaper_mode_get(unit, port_i, &shaper_mode););
        

        /* Map local port to base_q_pair */
        SOCDNX_IF_ERR_EXIT(arad_egr_dsp_pp_to_base_q_pair_set(unit, core,  tm_port, base_q_pair));

        /* Set port priorities */
        SOCDNX_IF_ERR_EXIT(arad_egr_dsp_pp_priorities_mode_set(unit,core, tm_port, priority_mode));

        /* set port shaper mode*/
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_dsp_pp_shaper_mode_set_unsafe(unit, core, tm_port, shaper_mode));

        /* Map port to interface */
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_map_set(unit, port_i, 0));

        /*Fast port enable*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port_i, &is_master));
        if (is_master) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface_type));
            if (interface_type == SOC_PORT_IF_ILKN) { 
                SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fast_port_set(unit, port_i, 1, 1));
            } else if (interface_type == SOC_PORT_IF_CAUI) {
                SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fast_port_set(unit, port_i, 0, 1));
            } else {
                SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fast_port_set(unit, port_i, 0, 0));
            }
        }
    }

    /* per core Init */
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
    tdm_egress_priority = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_priority;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {

        /* TDM SP mode configuration */
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            /*Enable ilkn_tdm_dedicated_queueing*/
            fld_val = 0x1;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, core, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, core, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));

            /* enable TDM traffic in EGQ */
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, EGRESS_TDM_MODEf,  fld_val));
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_PKT_TCf, tdm_egress_priority));

    }

    /* By default set disable fragmentation for all interfaces */
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITSET_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, NIF_NO_FRAG_Lf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,NIF_NO_FRAG_Lf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITSET_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, NIF_NO_FRAG_Hf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,NIF_NO_FRAG_Hf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, CPU_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,CPU_NO_FRAGf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, OLP_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,OLP_NO_FRAGf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, OAM_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,OAM_NO_FRAGf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, RCY_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,RCY_NO_FRAGf,fld_val_above_64);

        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_val_above_64));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

#define SOC_JER_CAL_BW_OTHERS   40 /*Gbps*/
#define SOC_JER_CAL_BW_OLP      1  /*Gbps*/
#define SOC_JER_CAL_BW_OAMP     1  /*Gbps*/
#define SOC_JER_CAL_BW_CPU      1  /*Gbps*/
#define SOC_JER_CAL_BW_RCY      1  /*Gbps*/

#define SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED   28
#define SOC_JER_FQP_CAL_IF_OTHERS_GIVE                29
#define SOC_JER_FQP_CAL_IF_1_GIG_NIFS                 30
#define SOC_JER_FQP_CAL_IF_INVALID                    31

#define SOC_JER_PQP_CAL_IF_INVALID                    128

#define SOC_JER_CAL_ILLEGAL_NIF                       0xfff

#define SOC_JER_MAX_CAL_LENGTH                        256

int
  soc_jer_egr_q_nif_cal_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN soc_jer_egr_cal_type_t cal_to_update
  )
{
    soc_port_t port;
    uint32 egr_if, flags, is_master, nof_nifs, last_port_entry, other_pqp_to_fill;
    uint32 if_rate_mbps, if_rate_gbps, total_gig_sum = 0;
    soc_pbmp_t port_bm;
    uint32  calendar_length;
    int i;
    soc_field_t cal_set_field;
    soc_mem_t cal_mem;
    uint32 reg_val, current_cal, next_cal, next_cal_offset, protocol_offset;
    uint32 rates[SOC_PBMP_PORT_MAX] = {0};
    ARAD_OFP_RATES_CAL_SCH *calendar = NULL;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_port_if_t interface_type;
    uint32 *temp_buff = NULL;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(calendar, ARAD_OFP_RATES_CAL_SCH, 1,"calendar egr_q_nif_cal");
    SOCDNX_ALLOC(temp_buff, uint32, SOC_JER_MAX_CAL_LENGTH, "temp_buff");

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));

    SOC_PBMP_ITER(port_bm, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

        if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
            /* Statistics interface is not required for Egress */
            continue;
        }
        if (SOC_PORT_IS_ELK_INTERFACE(flags)){
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
        if(!is_master) {
            continue;
        }
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, port, &egr_if));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));

        /*TDM SP MODE CONFIGURATION*/
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;;
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
            if (interface_type == SOC_PORT_IF_ILKN) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset));
                if (protocol_offset == 0) {
                   egr_if = 0;
                } else {
                   egr_if = 16;
                }
            }
        }
        
        /* set interface rate in gbps */
        if (if_rate_mbps <= 1000) {
            if_rate_gbps = 1;
        } else {
            if_rate_gbps = if_rate_mbps / 1000;
        }   

        total_gig_sum += if_rate_gbps;
        rates[egr_if] = if_rate_gbps;
    }

    /* handle special interfaces */
    if(cal_to_update == socJerCalTypeFQP) { /*FQP*/
        total_gig_sum += SOC_JER_CAL_BW_OTHERS;
        rates[SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED] = SOC_JER_CAL_BW_OTHERS;
    } else /*PQP*/ {
        
        total_gig_sum += SOC_JER_CAL_BW_CPU;
        rates[SOC_JER_EGR_IF_CPU] = SOC_JER_CAL_BW_CPU;

        total_gig_sum += SOC_JER_CAL_BW_OLP;
        rates[SOC_JER_EGR_IF_OLP] = SOC_JER_CAL_BW_OLP;

        total_gig_sum += SOC_JER_CAL_BW_OAMP;
        rates[SOC_JER_EGR_IF_OAMP] = SOC_JER_CAL_BW_OAMP;

        total_gig_sum += SOC_JER_CAL_BW_RCY;
        rates[SOC_JER_EGR_IF_RCY] = SOC_JER_CAL_BW_RCY;
    }

    /* build calendar from rates */
    nof_nifs = SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_from_rates_to_calendar(
       unit,
       rates,
       nof_nifs,
       total_gig_sum,
       SOC_JER_MAX_CAL_LENGTH,
       calendar,
       &calendar_length
       ));

    /* space calendar */
    last_port_entry = SOC_JER_CAL_ILLEGAL_NIF;
    other_pqp_to_fill = SOC_JER_EGR_IF_CPU;
    for (i = 0; i < calendar_length; i++)
    {
        if (last_port_entry == calendar->slots[i])
        {
            if (calendar_length == SOC_JER_MAX_CAL_LENGTH) {
                break;
            }

            /* Space the calendar, inserting dummies between duplicate entries */
            SOC_COPY(temp_buff, calendar->slots, uint32, SOC_JER_MAX_CAL_LENGTH);
            SOC_COPY(temp_buff + i + 1, calendar->slots + i, uint32, calendar_length - i);
            SOC_COPY(calendar->slots, temp_buff, uint32, SOC_JER_MAX_CAL_LENGTH);
            if (cal_to_update == socJerCalTypeFQP) { /*FQP*/
                calendar->slots[i] = SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED;
            } else { /*PQP, space with special interfaces(each time different special interface)*/
                calendar->slots[i] = other_pqp_to_fill;
                other_pqp_to_fill = (other_pqp_to_fill == SOC_JER_EGR_IF_RCY) ? SOC_JER_EGR_IF_CPU : other_pqp_to_fill+1;
            }

            /* Increase the calendar length by 1 for each dummy inserted */
            calendar_length++;
        }
        last_port_entry = calendar->slots[i];
    }

    /* Insert dummy tail at the end of calendar */
    if ((calendar_length < SOC_JER_MAX_CAL_LENGTH) && (calendar->slots[calendar_length-1] == calendar->slots[0]))
    {
        if (cal_to_update == socJerCalTypeFQP) { /*FQP*/
            calendar->slots[calendar_length] = SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED;
        } else { /*PQP*/
            calendar->slots[calendar_length] = other_pqp_to_fill;
        }
        calendar_length++;
    }
    /* set rest of celendatr entries to not valid */
    for (i=calendar_length; i<SOC_JER_MAX_CAL_LENGTH; i++) {
        if (cal_to_update == socJerCalTypePQP) {
            calendar->slots[i] = SOC_JER_PQP_CAL_IF_INVALID; 
        } else {
            calendar->slots[i] = SOC_JER_FQP_CAL_IF_INVALID;
        }
    }

    /* configure calendar */
    if (cal_to_update == socJerCalTypeFQP) { /* PQP*/
        cal_set_field = FQP_CALENDER_SET_SELf;
        cal_mem = EGQ_FQP_NIF_PORT_MUXm;
    } else /*PQP*/ {
        cal_set_field = PQP_CALENDER_SET_SELf;
        cal_mem = EGQ_PQP_NIF_PORT_MUXm;
    }

    SOCDNX_IF_ERR_EXIT(READ_EGQ_PQP_AND_FQP_CALENDER_SETTINGr(unit, core, &reg_val));
    current_cal = soc_reg_field_get(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, reg_val, cal_set_field);
    next_cal = current_cal ? 0 : 1;
    next_cal_offset = next_cal ? SOC_JER_MAX_CAL_LENGTH : 0;

    for (i=0; i<SOC_JER_MAX_CAL_LENGTH; i++) {
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, cal_mem, EGQ_BLOCK(unit, core), i+next_cal_offset, &(calendar->slots[i])));
    }

    /* switch calendar A<->B sets */
    soc_reg_field_set(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, &reg_val, cal_set_field, next_cal);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PQP_AND_FQP_CALENDER_SETTINGr(unit, core, reg_val));

exit:
    SOC_FREE(temp_buff);
    SOC_FREE(calendar);
    SOCDNX_FUNC_RETURN;
}

int
  soc_jer_egr_q_fqp_scheduler_config(
    SOC_SAND_IN int unit
  )
{
    soc_pbmp_t port_bm;
    uint32 flags, is_master, if_rate_mbps, egr_if, field_val;
    soc_port_t port;
    int core;
    soc_reg_above_64_val_t data;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_port_if_t interface_type;

    SOCDNX_INIT_FUNC_DEFS;
  
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        /*Bandwidth normal requests by others*/
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_RCY_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OAM_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OLP_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_DEL_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_CPU_REQ_ENf,  0x1));

        /*Bandwidth Higher requests by others*/
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_RCY_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OAM_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OLP_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_DEL_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_CPU_HIGHER_ENf,  0x0));
    }

    /*Disable higher requests on account of 100g ports or higher*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));

    SOC_PBMP_ITER(port_bm, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

        if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
            /* Statistics interface is not required for Egress */
            continue;
        }
        if (SOC_PORT_IS_ELK_INTERFACE(flags)){
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
        if(!is_master) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, port, &egr_if));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));

        /* set interface from higer request bitmap*/
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

        field_val = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS) ? 0 : 1; 
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, HIGHER_REQ_EN_PER_MALf, &field_val);

        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));


        /* handle ILKN dedicated mode */
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN) {
            if (IS_TDM_PORT(unit, port)) {
                egr_if++;
            } else {
                egr_if--;
            }

            /* set interface from higer request bitmap*/
            SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

            field_val = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS) ? 0 : 1; 
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, HIGHER_REQ_EN_PER_MALf, &field_val);

            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;

}

int
  soc_jer_egr_q_nif_cal_set_all(
    SOC_SAND_IN int unit
  )
{
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fqp_scheduler_config(unit));
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_nif_cal_set(unit, core, socJerCalTypeFQP));
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_nif_cal_set(unit, core, socJerCalTypePQP));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int 
soc_jer_egr_q_fast_port_set(
        SOC_SAND_IN  int unit, 
        SOC_SAND_IN  soc_port_t port, 
        SOC_SAND_IN  int is_ilkn, 
        SOC_SAND_IN  int turn_on
        )
{
    uint32 egress_offset, data, fld_val, tm_port;
    int core;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core)); 

    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egress_offset));

    /* read */
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));

    /* modify Fast port enable */
    fld_val = (turn_on ? 1 : 0);
    soc_EGQ_PER_IFC_CFGm_field32_set(unit, &data, FAST_PORT_ENf, fld_val);

    /* write */
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));

    /* handle ILKN dedicated mode */
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
    if (is_ilkn && ilkn_tdm_dedicated_queuing) {
        if (IS_TDM_PORT(unit, port)) {
            egress_offset++;
        } else {
            egress_offset--;
        }

        /* read */
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));

        /* modify Fast port enable */
        soc_EGQ_PER_IFC_CFGm_field32_set(unit, &data, FAST_PORT_ENf, fld_val);

        /* write */
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

uint32
  soc_jer_egr_queuing_sp_tc_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
    soc_reg_above_64_val_t reg;

    SOCDNX_INIT_FUNC_DEFS;
    if (threshold_type == soc_dpp_cosq_threshold_data_buffers)
    {
        /* Legacy, write max=min, alpha = 1*/
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers_max, threshold_value, drop_type));
        /* write the same value to min */
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers_min, threshold_value, drop_type));
        /* write 1 to alpha */
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_thereshold_data_buffers_alpha, 1, drop_type));
    }
    else if (threshold_type == soc_dpp_cosq_threshold_packet_descriptors || threshold_type == soc_dpp_cosq_threshold_available_data_buffers)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, threshold_type, threshold_value, drop_type));
    }
  
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_max)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers, threshold_value, drop_type));
    }
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_min)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_0_MINIMUMLIMITf, threshold_value);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_1_MINIMUMLIMITf, threshold_value);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_2_MINIMUMLIMITf, threshold_value);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_3_MINIMUMLIMITf, threshold_value);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_4_MINIMUMLIMITf, threshold_value);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_5_MINIMUMLIMITf, threshold_value);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_6_MINIMUMLIMITf, threshold_value);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_7_MINIMUMLIMITf, threshold_value);
                    break;
            }
        }
        else /* SP 1*/ 
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_0_MINIMUMLIMITf, threshold_value);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_1_MINIMUMLIMITf, threshold_value);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_2_MINIMUMLIMITf, threshold_value);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_3_MINIMUMLIMITf, threshold_value);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_4_MINIMUMLIMITf, threshold_value);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_5_MINIMUMLIMITf, threshold_value);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_6_MINIMUMLIMITf, threshold_value);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_7_MINIMUMLIMITf, threshold_value);
                    break;              
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
    }
    else if (threshold_type == soc_dpp_cosq_thereshold_data_buffers_alpha)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr(unit, core, reg));

        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_0_ALPHAf, threshold_value);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_1_ALPHAf, threshold_value);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_2_ALPHAf, threshold_value);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_3_ALPHAf, threshold_value);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_4_ALPHAf, threshold_value);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_5_ALPHAf, threshold_value);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_6_ALPHAf, threshold_value);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_7_ALPHAf, threshold_value);
                    break;
            }
        }
        else /* SP 1*/
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_0_ALPHAf, threshold_value);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_1_ALPHAf, threshold_value);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_2_ALPHAf, threshold_value);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_3_ALPHAf, threshold_value);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_4_ALPHAf, threshold_value);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_5_ALPHAf, threshold_value);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_6_ALPHAf, threshold_value);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_7_ALPHAf, threshold_value);
                    break;
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr(unit, core, reg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_egr_queuing_sp_tc_drop_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
    soc_reg_above_64_val_t reg;

    SOCDNX_INIT_FUNC_DEFS;
    if (threshold_type == soc_dpp_cosq_threshold_data_buffers)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_get_unsafe(unit, core, tc, threshold_type, threshold_value, drop_type));  
    }
    else if (threshold_type == soc_dpp_cosq_threshold_packet_descriptors || threshold_type == soc_dpp_cosq_threshold_available_data_buffers)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_get_unsafe(unit, core, tc, threshold_type, threshold_value, drop_type));
    }
  
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_max)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_get_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers, threshold_value, drop_type));
    }
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_min)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_0_MINIMUMLIMITf);
                    break;
                case 1:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_1_MINIMUMLIMITf);
                    break;
                case 2:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_2_MINIMUMLIMITf);
                    break;
                case 3:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_3_MINIMUMLIMITf);
                    break;
                case 4:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_4_MINIMUMLIMITf);
                    break;
                case 5:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_5_MINIMUMLIMITf);
                    break;
                case 6:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_6_MINIMUMLIMITf);
                    break;
                case 7:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_7_MINIMUMLIMITf);
                    break;
            }
        }
        else /* SP 1*/ 
        {
            switch (tc)
            {
                case 0:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_0_MINIMUMLIMITf);
                    break;
                case 1:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_1_MINIMUMLIMITf);
                    break;
                case 2:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_2_MINIMUMLIMITf);
                    break;
                case 3:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_3_MINIMUMLIMITf);
                    break;
                case 4:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_4_MINIMUMLIMITf);
                    break;
                case 5:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_5_MINIMUMLIMITf);
                    break;
                case 6:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_6_MINIMUMLIMITf);
                    break;
                case 7:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_7_MINIMUMLIMITf);
                    break;              
            }
        }
    }
    else if (threshold_type == soc_dpp_cosq_thereshold_data_buffers_alpha)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr(unit, core, reg));

        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_0_ALPHAf);
                    break;
                case 1:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_1_ALPHAf);
                    break;
                case 2:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_2_ALPHAf);
                    break;
                case 3:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_3_ALPHAf);
                    break;
                case 4:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_4_ALPHAf);
                    break;
                case 5:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_5_ALPHAf);
                    break;
                case 6:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_6_ALPHAf);
                    break;
                case 7:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_7_ALPHAf);
                    break;
            }
        }
        else /* SP 1*/
        {
            switch (tc)
            {
                case 0:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_0_ALPHAf);
                    break;
                case 1:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_1_ALPHAf);
                    break;
                case 2:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_2_ALPHAf);
                    break;
                case 3:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_3_ALPHAf);
                    break;
                case 4:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_4_ALPHAf);
                    break;
                case 5:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_5_ALPHAf);
                    break;
                case 6:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_6_ALPHAf);
                    break;
                case 7:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_7_ALPHAf);
                    break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
  soc_jer_egr_queuing_sch_unsch_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 field_32;

    SOCDNX_INIT_FUNC_DEFS;

    /* Write to jericho-relevant only fields */
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    field_32 = dev_thresh->thresh_type[threshold_type].mc_shared.descriptors_min;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_MC_PD_SHARED_MIN_THf, &field_32);
    field_32 = dev_thresh->thresh_type[threshold_type].mc_shared.descriptors_alpha;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_MC_PD_SHARED_ALPHAf, &field_32);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));

    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sch_unsch_drop_set_unsafe(unit, core, threshold_type, dev_thresh));
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_egr_threshold_types_verify(
   SOC_SAND_IN    int   unit,
   SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (threshold_type)
    {
        
        case soc_dpp_cosq_threshold_bytes:
        case soc_dpp_cosq_threshold_packet_descriptors:
        case soc_dpp_cosq_threshold_packets:
        case soc_dpp_cosq_threshold_data_buffers:
        case soc_dpp_cosq_threshold_available_packet_descriptors:
        case soc_dpp_cosq_threshold_available_data_buffers:
        case soc_dpp_cosq_threshold_buffer_descriptor_buffers:
        case soc_dpp_cosq_threshold_buffer_descriptors:
        case soc_dpp_cosq_threshold_dbuffs:
        case soc_dpp_cosq_threshold_full_dbuffs:
        case soc_dpp_cosq_threshold_mini_dbuffs:
        case soc_dpp_cosq_threshold_dynamic_weight:
        case soc_dpp_cosq_threshold_packet_descriptors_min:
        case soc_dpp_cosq_threshold_packet_descriptors_max:
        case soc_dpp_cosq_threshold_packet_descriptors_alpha:
        case soc_dpp_cosq_threshold_data_buffers_min:
        case soc_dpp_cosq_threshold_data_buffers_max:
        case soc_dpp_cosq_thereshold_data_buffers_alpha:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Invalid threshold type %d "), threshold_type));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  uint32
    soc_jer_egr_unsched_drop_q_pair_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_IN  uint32                drop_precedence,
    SOC_SAND_IN  ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  uint32
    offset;
  soc_reg_above_64_val_t data;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(thresh_info);

  if(thresh_info->packet_descriptors > EGQ_QDCT_PD_MAX_VAL)
  {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("packet descriptors is above max value")));
  }

  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
 
  /* Write PD thresholdS - min and alpha only */
  SOCDNX_IF_ERR_EXIT(READ_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_MC_PD_MIN_TH_DP_0f + drop_precedence, thresh_info->packet_descriptors_min);
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_MC_PD_ALPHA_DP_0f + drop_precedence, thresh_info->packet_descriptors_alpha);
  SOCDNX_IF_ERR_EXIT(WRITE_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));


exit:
  SOCDNX_FUNC_RETURN;
}



uint32
  soc_jer_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
    uint32 egress_tc;
    ARAD_EGR_THRESH_INFO thresh_info;

    SOCDNX_INIT_FUNC_DEFS;

    egress_tc = prio_ndx;

    /* Use arad function to write values common to arad and jericho */
    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_unsched_drop_set_unsafe(unit, core, profile ,prio_ndx, dp_ndx, thresh, exact_thresh));

    /* Write jericho-relevant only values */
   
    arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);
    thresh_info.packet_descriptors_min = thresh->queue_pkts_consumed_min[profile];
    thresh_info.packet_descriptors_alpha = thresh->queue_pkts_consumed_alpha[profile];
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_unsched_drop_q_pair_thresh_set_unsafe(
               unit,
               core,
               egress_tc,
               profile,
               dp_ndx,
               &thresh_info
           ));
       

    exact_thresh->queue_pkts_consumed_min[profile] = thresh_info.packet_descriptors_min;
    exact_thresh->queue_pkts_consumed_alpha[profile] = thresh_info.packet_descriptors_alpha;
  

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_egr_sched_port_fc_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 field_32;
    SOCDNX_INIT_FUNC_DEFS;

    /* Write to jericho-relevant only fields  - PDCT */
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    field_32 = thresh->packet_descriptors_min;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_UC_PD_MIN_FC_THf, &field_32);
    field_32 = thresh->packet_descriptors_alpha;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_UC_PD_ALPHAf, &field_32);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    /* PQST */
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PQST_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    field_32 = thresh->data_buffers_min;
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_UC_DB_MIN_FC_THf, &field_32);
    field_32 = thresh->data_buffers_alpha;
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_UC_DB_ALPHAf, &field_32);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PQST_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));

    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_sched_port_fc_thresh_set_unsafe(unit, core, threshold_type, thresh));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_egr_sched_q_fc_thresh_set_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 offset;
    ARAD_EGR_THRESH_INFO thresh_info;
    SOCDNX_INIT_FUNC_DEFS;

    offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type, prio);

    /* Write PD threshold - min and alpha only */
    SOCDNX_IF_ERR_EXIT(READ_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));
    soc_EGQ_QDCT_TABLEm_field32_set(unit, mem, QUEUE_UC_PD_MIN_FC_THf, thresh->packet_descriptors_min);
    soc_EGQ_QDCT_TABLEm_field32_set(unit, mem, QUEUE_UC_PD_ALPHAf, thresh->packet_descriptors_alpha);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));

    /* Write DB thresholds*/
    SOCDNX_IF_ERR_EXIT(READ_EGQ_QQST_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));
    soc_EGQ_QQST_TABLEm_field32_set(unit, mem, QUEUE_UC_DB_MIN_FC_THf, thresh->data_buffers_min);
    soc_EGQ_QQST_TABLEm_field32_set(unit, mem, QUEUE_UC_DB_ALPHAf, thresh->data_buffers_alpha);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_QQST_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));

    /* CALL THE ARAD FUNCTION */
    thresh_info.dbuff = thresh->data_buffers;/* queue_words_consumed parameter used for Data buffers in Arad */
    thresh_info.packet_descriptors = thresh->packet_descriptors;

    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
               unit,
               core,
               prio,
               threshold_type,
               &thresh_info));

exit:
    SOCDNX_FUNC_RETURN;

}

uint32	
  soc_jer_egr_queuing_if_fc_uc_set_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  )
{
    soc_reg_above_64_val_t reg_pd_min, reg_db_min;
    uint32 reg_pd_alpha, reg_db_alpha;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr(unit, core, reg_pd_min));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr(unit, core, &reg_pd_alpha));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr(unit, core, reg_db_min));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr(unit, core, &reg_db_alpha));

    switch(uc_if_profile_ndx)
    {
        case 0:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_0f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_0f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_0f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_0f, info->size256_th_alpha);
            break;
        case 1:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_1f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_1f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_1f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_1f, info->size256_th_alpha);
            break;
        case 2:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_2f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_2f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_2f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_2f, info->size256_th_alpha);
            break;
        case 3:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_3f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_3f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_3f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_3f, info->size256_th_alpha);
            break;
        case 4:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_4f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_4f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_4f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_4f, info->size256_th_alpha);
            break;
        case 5:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_5f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_5f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_5f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_5f, info->size256_th_alpha);
            break;
        case 6:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_6f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_6f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_6f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_6f, info->size256_th_alpha);
            break;
        case 7:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_7f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_7f, info->pd_th_alpha);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_7f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_7f, info->size256_th_alpha);
            break;
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr(unit, core, reg_pd_min));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr(unit, core, reg_pd_alpha));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr(unit, core, reg_db_min));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr(unit, core, reg_db_alpha));

    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_if_fc_uc_set_unsafe(unit, core, uc_if_profile_ndx, info));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_egr_queuing_init_thresholds(int unit, int port_rate, int nof_priorities, int nof_channels, SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS* cgm_init_thresholds)
{
    SOCDNX_INIT_FUNC_DEFS;

    cgm_init_thresholds->mc_reserved_pds  = 40; /* Default value - just in case */

    if(port_rate <= 2500) 
    { /* 1G */
        cgm_init_thresholds->threshold_port = 31;
        cgm_init_thresholds->threshold_queue = 31;
        cgm_init_thresholds->port_mc_drop_pds = 135;
        cgm_init_thresholds->port_mc_drop_dbs = 1350;
        cgm_init_thresholds->mc_reserved_pds = 40;
    } 
    else if(port_rate <= 12500) /* 10G */
    { 
        cgm_init_thresholds->threshold_port = 167;
        cgm_init_thresholds->threshold_queue = 84;
        cgm_init_thresholds->port_mc_drop_pds = 722;
        cgm_init_thresholds->port_mc_drop_dbs = 7220;
        switch(nof_priorities)
        {
            case 1:
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 160;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 40;
                break;
            default:
                break;      
        }
    }
    else if (port_rate <= 30000) /* 25G */
    {
        cgm_init_thresholds->threshold_port = 500;
        cgm_init_thresholds->threshold_queue = 250;
        cgm_init_thresholds->port_mc_drop_pds = 2167;
        cgm_init_thresholds->port_mc_drop_dbs = 21670;
        switch (nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 500;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 400;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds  = 100;
                break;
            default:
                break;
        }
    } 
    else if(port_rate <= 48000) /* 40G */
    { 
        cgm_init_thresholds->threshold_port = 500;
        cgm_init_thresholds->threshold_queue = 250;
        cgm_init_thresholds->port_mc_drop_pds = 2167;
        cgm_init_thresholds->port_mc_drop_dbs = 21670;
        switch(nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 500;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 640;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 160;
                break;
            default:
                break;
        }
    } 
    else if(port_rate <= 127000) /* 100G , 120G */
    { 
        cgm_init_thresholds->threshold_port = 2000;
        cgm_init_thresholds->threshold_queue = 1000;
        cgm_init_thresholds->port_mc_drop_pds = 8667;
        cgm_init_thresholds->port_mc_drop_dbs = 86670;
        switch(nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 2000;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 1600;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 400;
                break;
            default:
                break;
        }
    } 
    else  /* 300G */
    { 
        cgm_init_thresholds->threshold_port = 3000;
        cgm_init_thresholds->threshold_port = 1500;
        cgm_init_thresholds->port_mc_drop_pds = 13000;
        cgm_init_thresholds->port_mc_drop_dbs = 130000;
        switch(nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 3000;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 1600;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 1200;
                break;
            default:
                break;
        }
    }

    cgm_init_thresholds->mc_reserved_pds = cgm_init_thresholds->mc_reserved_pds / nof_channels;
    cgm_init_thresholds->drop_pds_th = 6000;
    cgm_init_thresholds->drop_dbs_th = 6000;
    cgm_init_thresholds->drop_pds_th_tc = 4000;

    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

