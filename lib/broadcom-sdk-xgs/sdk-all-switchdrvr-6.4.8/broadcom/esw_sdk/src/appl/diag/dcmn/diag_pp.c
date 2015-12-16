/* 
 * $Id: diag_pp.c,v 1.15 Broadcom SDK $
 *
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
 * File:        diag.c
 * Purpose:     Device diagnostics commands.
 *
 */






#include "appl/dpp/FecAllocation/fec_allocation.h"

#include <shared/bsl.h>

#include <appl/diag/diag.h>
#include <appl/diag/diag_pp.h>
#include <appl/diag/diag_alloc.h>

#include <soc/drv.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <bcm/vswitch.h>
#include <bcm/policer.h>

#include <appl/diag/diag.h>
#include <appl/diag/diag_pp.h>
#include <sal/appl/sal.h>
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/system.h>

#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPC/ppc_api_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/petra_dispatch.h>
#include <soc/dpp/mbcm_pp.h>
#endif /* BCM_DPP_SUPPORT */

#include <soc/hwstate/hw_log.h>


#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_diag.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_diag.h>
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
  #include <soc/dpp/mbcm_pp.h>
#endif

#ifdef DATAIO_SUPPORT
#include <appl/diag/dcmn/diag_pp_data.h>
#endif

/*************
 * DEFINES   *
 *************/
#define DIAG_PP_GET_BLOCK_NOF_ENTRIES 100

#ifdef BCM_DPP_SUPPORT
cmd_result_t cmd_ppd_api_diag_frwrd_decision_trace_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_received_packet_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_parsing_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_db_lif_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_termination_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_traps_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_trapped_packet_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_encap_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_frwrd_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_frwrd_lpm_lkup_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_ing_vlan_edit_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_learning_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_mode_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_mode_info_set(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_pkt_associated_tm_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_pkt_trace_clear(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_db_lem_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_vsi_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_rif_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_vtt_dump_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_flp_dump_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_egress_vlan_edit_info_get(int unit, args_t* a); 
cmd_result_t cmd_ppd_api_diag_mtr_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_gport_info_get(int unit, args_t* a);
cmd_result_t cmd_bcm_cross_connect_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_ipv4_mc_route_get(int unit, args_t* a);
cmd_result_t cmd_diag_occupation_profile_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_egress_drop(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_lif_show(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_pp_last(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_tpid_globals_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_tpid_profiles_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_tpid_port_profile_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_db_glem_lkup_info_get(int unit, args_t* a);
#ifdef BCM_ARAD_SUPPORT
cmd_result_t cmd_diag_fec_allocation(int unit, args_t* a);
cmd_result_t cmd_diag_occ_mgmt(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_cos_get(int unit, args_t* a);
#endif
#ifdef BCM_JERICHO_SUPPORT
cmd_result_t cmd_diag_kaps_lkup_info_get(int unit, args_t* a);
#endif /* BCM_JERICHO_SUPPORT */
cmd_result_t cmd_diag_hw_log_full_test(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_mem_test(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_mem_test1(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_reg_test(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_show(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_commit(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_purge(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_diag(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_set_immediate_hw_access(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_restore_hw_access(int unit, args_t* a);
static int hw_access;


/*extern int soc_crash_rec_test(int unit, soc_mem_t mem, void* data);
extern void soc_crash_rec_print_list(int unit);
extern void soc_hw_log_commit(int unit);*/

const DIAG_PP_COMMAND_DEF diag_pp_command_def[] =
{
/*     --------------------------------------------------------------------------------
      |        Long          |   Short    |             Print                         |
      |      Command Name    |Command Name|            Function                       |
       -------------------------------------------------------------------------------- */
    { "Frwrd_Decision_Trace"  , "FDT"    , cmd_ppd_api_diag_frwrd_decision_trace_get  },
    { "Received_Packet_Info"  , "RPI"    , cmd_ppd_api_diag_received_packet_info_get  },
    { "Parsing_Info"          , "PI"     , cmd_ppd_api_diag_parsing_info_get          },
    { "DB_LIF_lkup_info"      , "DBLIF"  , cmd_ppd_api_diag_db_lif_lkup_info_get      },
    { "TERMination_Info"      , "TERMI"  , cmd_ppd_api_diag_termination_info_get      },
    { "TRAPS_Info"            , "TRAPSI" , cmd_ppd_api_diag_traps_info_get            },
    { "Trapped_Packet_Info"   , "TPI"    , cmd_ppd_api_diag_trapped_packet_info_get   },
    { "ENCAP_info"            , "ENCAP"  , cmd_ppd_api_diag_encap_info_get            },
    { "Frwrd_Lkup_Info"       , "FLI"    , cmd_ppd_api_diag_frwrd_lkup_info_get       },
    { "Frwrd_LPM_lkup"        , "FLPM"   , cmd_ppd_api_diag_frwrd_lpm_lkup_get        },
    { "Ing_Vlan_Edit_info"    , "IVE"    , cmd_ppd_api_diag_ing_vlan_edit_info_get    },
    { "LeaRNing_info"         , "LRN"    , cmd_ppd_api_diag_learning_info_get         },
    { "MODE_info_Get"         , "MODEG"  , cmd_ppd_api_diag_mode_info_get             },
    { "MODE_info_Set"         , "MODES"  , cmd_ppd_api_diag_mode_info_set             },
    { "PKT_associated_TM_info", "PKTTM"  , cmd_ppd_api_diag_pkt_associated_tm_info_get},
    { "PKT_TRace_Clear"       , "PKTTRC" , cmd_ppd_api_diag_pkt_trace_clear           },
    { "MeTeR_info_get"        , "MTR"    , cmd_ppd_api_diag_mtr_info_get              },
    { "DB_LEM_lkup_info"      , "DBLEM"  , cmd_ppd_api_diag_db_lem_lkup_info_get      },
    { "Egress_DROP"           , "EDROP"  , cmd_diag_egress_drop                       },
    { "LIF_show"              , "LIF"    , cmd_ppd_api_diag_lif_show                  },
    { "LAST"                  , "LAST"   , cmd_ppd_api_diag_pp_last                   },
    { "TPID_Globals_show"     , "TPIDG"  , cmd_ppd_api_diag_tpid_globals_get          },
    { "TPID_Profiles_show"    , "TPIDPR" , cmd_ppd_api_diag_tpid_profiles_get         },
    { "TPID_Port_Profile_show", "TPIDPO" , cmd_ppd_api_diag_tpid_port_profile_get     }
#ifdef  BCM_ARAD_SUPPORT
                                                                                      ,
    { "VTT_DUMP"              , "VTT"    , cmd_ppd_api_diag_vtt_dump_get              },
    { "FLP_DUMP"              , "FLP"    , cmd_ppd_api_diag_flp_dump_get              },
#ifdef DATAIO_SUPPORT
    { "DUMP"                  , "DUMP"   , cmd_ppd_api_diag_pp_dump_get               },
    { "Signals"               , "SIG"    , cmd_ppd_api_diag_pp_signals_get            },
#endif
    { "Egr_Vlan_Edit_Info"    , "EVE"    , cmd_ppd_api_diag_egress_vlan_edit_info_get },
    { "GPort"                 , "GP"     , cmd_diag_gport_info_get                    },
    { "Cross-Connect-traverse", "CC"     , cmd_bcm_cross_connect_get                  },
    { "IPv4 MC Route Table"   , "IPv4_MC", cmd_ppd_api_diag_ipv4_mc_route_get         },
    { "VSI_decision"          , "VSI"    , cmd_ppd_api_diag_vsi_info_get              },
    { "RIF_profile_info"      , "RIF"    , cmd_ppd_api_diag_rif_info_get              },    
    { "If_PROFILE_Status"     , "IPROFILE" , cmd_diag_occupation_profile_info_get     },
    { "HW_Log_Full_Test"      , "HWLOGFULLTEST",    cmd_diag_hw_log_full_test         },
    { "HW_Log_Single_Mem_Test", "HWLOGTESTMEM",     cmd_diag_hw_log_mem_test          },
    { "HW_Log_Single1_Mem_Test","HWLOGTESTMEM1",    cmd_diag_hw_log_mem_test1         },
    { "HW_Log_Single_Test_Reg", "HWLOGTESTREG",     cmd_diag_hw_log_reg_test          },
    { "HW_Log_Show"           , "HWLOGSHOW",        cmd_diag_hw_log_show              },
    { "HW_Log_Commit"         , "HWLOGCOMMIT",      cmd_diag_hw_log_commit            },
    { "HW_Log_Purge"          , "HWLOGPURGE",       cmd_diag_hw_log_purge             },
    { "HW_Log_Diagnostics"    , "HWLOGDIAG",        cmd_diag_hw_log_diag              },
    { "HW_Log_Set_Immediate_Hw_Access", "HWLOGIM",       cmd_diag_hw_log_set_immediate_hw_access},
    { "HW_Log_Restore_Hw_Access"      , "HWLOGRESTORE",  cmd_diag_hw_log_restore_hw_access},
    { "Fec_Allocation_Status" , "FECALLOCATION",    cmd_diag_fec_allocation           },
    { "OCCupation_mgmt_show"  , "OCC",              cmd_diag_occ_mgmt                 },
    { "Class_Of_Service"      , "COS" ,   cmd_ppd_api_diag_cos_get                    },
    { "DB_GLEM_lkup_info"     , "DBGLEM", cmd_ppd_api_diag_db_glem_lkup_info_get      }
#endif
#ifdef  BCM_JERICHO_SUPPORT
                                                                                      ,
    { "Kaps_LKuP_info_get"    , "KLKP",    cmd_diag_kaps_lkup_info_get            }
#endif /* BCM_JERICHO_SUPPORT */
        };

cmd_result_t cmd_diag_hw_log_full_test(int unit, args_t* a)
{
    bcm_error_t rv;
    rv = soc_mem_iterate(unit, soc_hw_log_mem_test, NULL);
    if( rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_mem_test(int unit, args_t* a)
{
    soc_hw_log_mem_test(unit, 188, NULL);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_mem_test1(int unit, args_t* a)
{
    soc_hw_log_mem_test(unit, 689, NULL);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_reg_test(int unit, args_t* a)
{
    soc_hw_log_reg_test(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_show(int unit, args_t* a)
{
    soc_hw_log_print_list(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_commit(int unit, args_t* a)
{
    soc_hw_log_commit(unit);
    return CMD_OK;
}
cmd_result_t cmd_diag_hw_log_purge(int unit, args_t* a)
{
    soc_hw_log_purge(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_diag(int unit, args_t* a)
{
    soc_hw_log_diagshow(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_set_immediate_hw_access(int unit, args_t* a)
{
    soc_hw_set_immediate_hw_access(unit, &hw_access);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_restore_hw_access(int unit, args_t* a)
{
    soc_hw_restore_immediate_hw_access(unit, hw_access);
    return CMD_OK;
}


/******************************************************************** 
 *  Function handler:  frwrd_decision_trace_get (section diag)
 ********************************************************************/

cmd_result_t
  cmd_ppd_api_diag_frwrd_decision_trace_get(int unit, args_t* a) 
{   
  uint32 
    ret; 
  uint32
    soc_sand_dev_id; 
  parse_table_t    pt;
  int              prm_core=0;

  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO   
    prm_frwrd_trace_info;
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_frwrd_decision_trace_get(
          soc_sand_dev_id,
          prm_core,
          &prm_frwrd_trace_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    return CMD_FAIL;
  } 
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO_print(&prm_frwrd_trace_info);
  return CMD_OK;
} 


/******************************************************************** 
 *  Function handler: received_packet_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_received_packet_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO   
    prm_rcvd_pkt_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=0;

  soc_sand_dev_id = (unit);
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_clear(&prm_rcvd_pkt_info);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_received_packet_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_rcvd_pkt_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
      return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_print(&prm_rcvd_pkt_info);

  return CMD_OK;
} 
  
/******************************************************************** 
 *  Function handler: parsing_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_parsing_info_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_PARSING_INFO   
    prm_pars_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core = 0;
  
  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_PARSING_INFO_clear(&prm_pars_info);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_parsing_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_pars_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
      return CMD_FAIL;     
  } 

  SOC_PPD_DIAG_PARSING_INFO_print(&prm_pars_info);

  return CMD_OK; 
} 

cmd_result_t 
  cmd_ppd_api_diag_db_lif_lkup_info_specific_get(int unit, int core_id, SOC_PPD_DIAG_DB_USE_INFO* prm_db_info)  
{      
    SOC_PPD_DIAG_LIF_LKUP_INFO
      prm_lkup_info;
    uint32
      ret;
    uint32
      soc_sand_dev_id;
    int
      lif_profile;

    SOC_PPD_DIAG_LIF_LKUP_INFO_clear(&prm_lkup_info);
    soc_sand_dev_id = (unit);
    

    /* Call function */
    ret = soc_ppd_diag_db_lif_lkup_info_get(
            soc_sand_dev_id,
            core_id,
            prm_db_info,
            &prm_lkup_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    } 

    if(prm_lkup_info.found)
    {
        SOC_PPC_DIAG_DB_USE_INFO_print(prm_db_info);
        if(prm_db_info->bank_id == 3)
            cli_out("Port %u\r\n", prm_lkup_info.key.ac.raw_key);
        SOC_PPD_DIAG_LIF_LKUP_INFO_print(&prm_lkup_info);
    }

    if (prm_lkup_info.found) {
        /* Check if profile assignment printing is necessary */
        switch (prm_lkup_info.type) {
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
            lif_profile = prm_lkup_info.value.ac.lif_profile;
            break;
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
            lif_profile = prm_lkup_info.value.pwe.lif_profile;
            break;
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
            lif_profile = prm_lkup_info.value.mpls.lif_profile;
            break;
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
            lif_profile = prm_lkup_info.value.ip.lif_profile;
            break;
        default: /* No profile usage */
            lif_profile = -1;
            break;
        }
        if (lif_profile > -1) {
            ret = SOC_PPC_OCC_PROFILE_USAGE_print(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_SIZE);
            if (soc_sand_get_error_code_from_error_word(ret) != CMD_OK) 
            { 
                return CMD_FAIL; 
            } 
        }
    }

    cli_out("\n");

    return CMD_OK;
}
/******************************************************************** 
 *  Function handler: db_lif_lkup_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_db_lif_lkup_info_get(int unit, args_t* a)  
{   
  SOC_PPD_DIAG_DB_USE_INFO
    prm_db_info;
  
  parse_table_t    pt;
  uint32              default_val = 4;
  uint32              bank_id=4, bank_id_tmp;
  uint32              lkup_num=4, lkup_num_tmp;
  int                 prm_core=0; 


  SOC_PPD_DIAG_DB_USE_INFO_clear(&prm_db_info);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "bank_id", PQ_DFL|PQ_INT, &default_val,  &bank_id, NULL);
  parse_table_add(&pt, "lkup_num", PQ_DFL|PQ_INT, &default_val, &lkup_num, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 
  
  if (lkup_num == default_val) {
      for (lkup_num_tmp = 0; lkup_num_tmp < 2; lkup_num_tmp++) {
          prm_db_info.lkup_num = lkup_num_tmp;           
          if (bank_id == default_val) {
              for (bank_id_tmp = 0; bank_id_tmp < ((lkup_num_tmp == 0) ? 4 : 3); bank_id_tmp++) {
                  prm_db_info.bank_id = bank_id_tmp;
                  if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
                  {
                      return CMD_FAIL;
                  }
              }
          } else {
              prm_db_info.bank_id = bank_id;
              if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
              {
                  return CMD_FAIL;
              }
          }
      }
  } else {
      prm_db_info.lkup_num = lkup_num;      
      if (bank_id == default_val) {
          for (bank_id_tmp = 0; bank_id_tmp < ((lkup_num == 0 ) ? 4 : 3); bank_id_tmp++) {
              prm_db_info.bank_id = bank_id_tmp;
              if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
              {
                  return CMD_FAIL;
              }
          }
      } else {
          prm_db_info.bank_id = bank_id;
          if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
          {
              return CMD_FAIL;
          }
      }
  }

  
 
  return CMD_OK;
} 

/******************************************************************** 
 *  Function handler: termination_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_termination_info_get(int unit, args_t* a)   
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_TERM_INFO   
    prm_term_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=0;

  SOC_PPD_DIAG_TERM_INFO_clear(&prm_term_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_termination_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_term_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
      return CMD_FAIL;  
  } 

  SOC_PPD_DIAG_TERM_INFO_print(&prm_term_info);
  cli_out("\n");
  return CMD_OK; 
} 


/******************************************************************** 
 *  Function handler: traps_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_traps_info_get(int unit, args_t* a)    
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_TRAPS_INFO   
    prm_traps_info;
  uint32
    soc_sand_dev_id;
  int prm_core =  0;
  parse_table_t    pt;
 
  SOC_PPD_DIAG_TRAPS_INFO_clear(&prm_traps_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_traps_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_traps_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
      return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_TRAPS_INFO_print(&prm_traps_info);
  return CMD_OK; 
} 
  

/******************************************************************** 
 *  Function handler: trapped_packet_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_trapped_packet_info_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  uint8   
    prm_buff[400];
  uint32   
    prm_max_size=400,
    ind;
  uint32   
    prm_buff_len;
  SOC_PPD_DIAG_TRAP_PACKET_INFO   
    prm_packet_info;
  uint32
    prm_parse_learn = 0;
/*  SOC_PPD_FRWRD_MACT_LEARN_MSG
    learn_msg;
  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO
    learn_events; */
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;

  SOC_PPD_DIAG_TRAP_PACKET_INFO_clear(&prm_packet_info);
  soc_sand_dev_id = (unit);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "max_size", PQ_DFL|PQ_INT, 0, &prm_max_size, NULL);
  /* prm_parse_learn is not supported */
/*  parse_table_add(&pt, "prm_parse_learn", PQ_DFL|PQ_INT, 0, &prm_parse_learn, NULL);*/

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Call function */
  ret = soc_ppd_diag_trapped_packet_info_get(
          soc_sand_dev_id,
          prm_buff,
          prm_max_size,
          &prm_buff_len,
          &prm_packet_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
      return CMD_FAIL;
  } 

  if (prm_parse_learn != 3)
  {
    for (ind=0; ind < SOC_SAND_MIN(prm_buff_len,300); ++ind)
    {
      cli_out("Data[%u]:       %02x\n\r", ind,prm_buff[ind]);
    }

    cli_out("buff_len: %u\n\r", prm_buff_len);

    SOC_PPD_DIAG_TRAP_PACKET_INFO_print(&prm_packet_info);
  }

  /*
  if (prm_parse_learn != 0)
  {
    learn_msg.max_nof_events = 8;
    learn_events.events = soc_sand_os_malloc(sizeof(SOC_PPC_FRWRD_MACT_EVENT_INFO)*learn_msg.max_nof_events,"learn_events.events");
    
    learn_msg.msg_len = prm_buff_len;
    learn_msg.msg_buffer = soc_sand_os_malloc(sizeof(uint8)* prm_buff_len, "learn_msg.msg_buffer");
    soc_sand_os_memcpy(learn_msg.msg_buffer,prm_buff, prm_buff_len);
    
    ret = soc_ppd_frwrd_mact_learn_msg_parse(soc_sand_dev_id,&learn_msg,&learn_events);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL;
    } 

    SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(&learn_events);

    soc_sand_os_free(learn_events.events);
    soc_sand_os_free(learn_msg.msg_buffer);

  }
  */

  return CMD_OK;
} 

/******************************************************************** 
 *  Function handler: encap_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_encap_info_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_ENCAP_INFO   
    prm_encap_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=0;

  SOC_PPD_DIAG_ENCAP_INFO_clear(&prm_encap_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Call function */
  ret = soc_ppd_diag_encap_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_encap_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_ENCAP_INFO_print(unit, &prm_encap_info);

  if (SOC_IS_JERICHO(unit)) {
      ret = SOC_PPC_OCC_PROFILE_USAGE_print(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_SIZE(unit));
      if (soc_sand_get_error_code_from_error_word(ret) != CMD_OK) 
      { 
          return CMD_FAIL; 
      }
  }
  return CMD_OK; 
} 



/******************************************************************** 
 *  Function handler: frwrd_lkup_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_frwrd_lkup_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_FRWRD_LKUP_INFO   
    prm_frwrd_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=0;
  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_FRWRD_LKUP_INFO_clear(&prm_frwrd_info);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_frwrd_lkup_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_frwrd_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_FRWRD_LKUP_INFO_print(&prm_frwrd_info);
  return CMD_OK;  
} 
  
  
/******************************************************************** 
 *  Function handler: frwrd_lpm_lkup_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_frwrd_lpm_lkup_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_IPV4_VPN_KEY   
    prm_lpm_key;
  uint32   
    prm_fec_ptr;
  uint8   
    prm_found;
  uint32
    soc_sand_dev_id;
  ip_addr_t   ip_addr;
  parse_table_t    pt;
  int              prm_core=0;
   
  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_IPV4_VPN_KEY_clear(&prm_lpm_key);
 
  /* Get parameters */ 

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
  parse_table_add(&pt, "prefix_len", PQ_DFL|PQ_INT, 0, &prm_lpm_key.key.subnet.prefix_len, NULL);
  parse_table_add(&pt, "vrf", PQ_DFL|PQ_INT, 0, &prm_lpm_key.vrf, NULL);
  parse_table_add(&pt, "ip_address", PQ_DFL|PQ_IP, 0, &ip_addr, NULL);

  prm_lpm_key.vrf = (uint32)-1;

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if (prm_lpm_key.vrf == (uint32)-1) {
      cli_out(" *** SW error - expecting parameter lpm_key after frwrd_lpm_lkup_get***\n\r");
      return CMD_FAIL;
  }
  prm_lpm_key.key.subnet.ip_address = ip_addr;

  /* Call function */
  ret = soc_ppd_diag_frwrd_lpm_lkup_get(
          soc_sand_dev_id,
          prm_core,
          &prm_lpm_key,
          &prm_fec_ptr,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL;
  } 

  cli_out("fec_ptr: %u\n\r",prm_fec_ptr);

  cli_out("found: %u\n\r",prm_found);

  return CMD_OK;  
} 
  


/******************************************************************** 
 *  Function handler: ing_vlan_edit_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_ing_vlan_edit_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_VLAN_EDIT_RES   
    prm_vec_res;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=0;

  soc_sand_dev_id = (unit);
  SOC_PPD_DIAG_VLAN_EDIT_RES_clear(&prm_vec_res);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  /* Call function */
  ret = soc_ppd_diag_ing_vlan_edit_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_vec_res
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_VLAN_EDIT_RES_print(&prm_vec_res);
  return CMD_OK;  
} 
  


/******************************************************************** 
 *  Function handler: learning_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_learning_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_LEARN_INFO   
    prm_learn_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;

  int prm_core=0;

  soc_sand_dev_id = (unit);
  SOC_PPD_DIAG_LEARN_INFO_clear(&prm_learn_info);
 
 /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }
  
  /* Call function */
  ret = soc_ppd_diag_learning_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_learn_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_LEARN_INFO_print(&prm_learn_info);
  return CMD_OK;  
} 



/******************************************************************** 
 *  Function handler: mode_info_get (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_mode_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_MODE_INFO   
    prm_mode_info;
  uint32
    soc_sand_dev_id;

  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_MODE_INFO_clear(&prm_mode_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_diag_mode_info_get(
          soc_sand_dev_id,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_MODE_INFO_print(&prm_mode_info);
  return CMD_OK;  
} 
  
/******************************************************************** 
 *  Function handler: mode_info_set (section diag)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_diag_mode_info_set(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_MODE_INFO   
    prm_mode_info;
  uint32
    soc_sand_dev_id;
  char* flavor;
  parse_table_t    pt;
  int32 params_num;

  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_MODE_INFO_clear(&prm_mode_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_diag_mode_info_get(
          soc_sand_dev_id,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "flavor", PQ_DFL|PQ_STRING, 0, (void *)&flavor, NULL);
  prm_mode_info.flavor = SOC_PPD_DIAG_FLAVOR_NONE;

  params_num = parse_arg_eq(a, &pt);
  if (0 > params_num) {
      return CMD_USAGE;
  }

  if ((params_num > 0) && (! sal_strncasecmp(flavor, "RAW", strlen(flavor)))) {
      prm_mode_info.flavor |= 1;
  } else {
      prm_mode_info.flavor = 0;
  }

  /* Call function */
  ret = soc_ppd_diag_mode_info_set(
          soc_sand_dev_id,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL;
  } 

  return CMD_OK; 
} 

/******************************************************************** 
 *  Function handler: pkt_associated_tm_info_get (section diag)
 ********************************************************************/
int 
  cmd_ppd_api_diag_pkt_associated_tm_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_PKT_TM_INFO   
    prm_pkt_tm_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=0;

  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_PKT_TM_INFO_clear(&prm_pkt_tm_info);
 
  /* Get parameters */ 

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
   
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Call function */
  ret = soc_ppd_diag_pkt_associated_tm_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_pkt_tm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_PKT_TM_INFO_print(&prm_pkt_tm_info);
  return CMD_OK;  
} 


/******************************************************************** 
 *  Function handler: pkt_associated_tm_info_get (section diag)
 ********************************************************************/
int 
  cmd_ppd_api_diag_mtr_info_get(int unit, args_t* a) 
{   
  uint32 
    ret, meter_core, meter_group, core_id;   
  bcm_policer_config_t
    policer_cfg;
  bcm_policer_t
    policer_id;
  parse_table_t    
	pt;
  int 
	cbl, ebl;
  SOC_PPC_MTR_METER_ID
	meter_id;

  bcm_policer_config_t_init(&policer_cfg);
  meter_core = meter_group = 0;
  cbl = ebl = 0;
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "id", PQ_INT, 0, &policer_id, NULL);
  parse_table_add(&pt, "group", PQ_DFL|PQ_INT, 0, &meter_group, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &meter_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* if core/group explicity requested, update policer_id*/
  if (meter_group != 0 || meter_core != 0) {
	  if (meter_core > _DPP_POLICER_METER_MAX_CORE_ID(unit) || meter_group > 1) {
		  return CMD_FAIL; 
	  }

	  policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, policer_id, meter_group, meter_core); 
  }

  /* Get meter config */
  ret = bcm_petra_policer_get(unit, policer_id, &policer_cfg); 
  if (ret != BCM_E_NONE) { 
        return CMD_FAIL; 
  } 

  /* Get bucket levels*/
  SOC_PPC_MTR_METER_ID_clear(&meter_id);
  core_id = _DPP_POLICER_ID_TO_CORE(unit, policer_id);
  meter_id.group = _DPP_POLICER_ID_TO_GROUP(unit, policer_id);
  meter_id.id = _DPP_POLICER_ID_TO_METER(unit, policer_id);

  ret = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_meter_ins_bucket_get,(unit, core_id, &meter_id, &cbl, &ebl));
  if(SOC_SAND_FAILURE(ret)) {
        return CMD_FAIL;
  }

  /* Print*/
  ARAD_PP_DIAG_MTR_INFO_print(unit, policer_id, &policer_cfg, cbl, ebl);

  return CMD_OK;  
} 

  
/******************************************************************** 
 *  Function handler: pkt_trace_clear (section diag)
 ********************************************************************/
int 
  cmd_ppd_api_diag_pkt_trace_clear(int unit, args_t* a) 
{   
  uint32 
    ret;   
  uint32   
    prm_pkt_trace = SOC_PPC_DIAG_PKT_TRACE_ALL;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core = 0;
   
  soc_sand_dev_id = (unit); 
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Call function */
  ret = soc_ppd_diag_pkt_trace_clear(
          soc_sand_dev_id,
          prm_core,
          prm_pkt_trace
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL;
  } 

  return CMD_OK;  
} 


/******************************************************************** 
 *  Function handler: db_lem_lkup_info_get (section diag)
 ********************************************************************/
int 
  cmd_ppd_api_diag_db_lem_lkup_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_DB_USE_INFO   
    prm_db_info;
  SOC_PPD_DIAG_LEM_LKUP_TYPE   
    prm_type;
  SOC_PPD_DIAG_LEM_KEY   
    prm_key;
  SOC_PPD_DIAG_LEM_VALUE   
    prm_val;
  uint8   
    prm_valid;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core = 0;

  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_DB_USE_INFO_clear(&prm_db_info);
  SOC_PPD_DIAG_LEM_KEY_clear(&prm_key);
  SOC_PPD_DIAG_LEM_VALUE_clear(&prm_val);
 
  /* Get parameters */
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "lkup_num", PQ_DFL|PQ_INT, 0, &prm_db_info.lkup_num, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Call function */
  ret = soc_ppd_diag_db_lem_lkup_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_db_info,
          &prm_type,
          &prm_key,
          &prm_val,
          &prm_valid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL;
  } 

  cli_out("type: %s\n\r",SOC_PPD_DIAG_LEM_LKUP_TYPE_to_string(prm_type));

  cli_out("key:\n\r");
  SOC_PPD_DIAG_LEM_KEY_print(&prm_key,prm_type);
  if (prm_valid)
  {
    cli_out("value:\n\r");
    SOC_PPD_DIAG_LEM_VALUE_print(&prm_val,prm_type);
  }
  cli_out("valid: %u\n\r",prm_valid);

  return CMD_OK; 
} 

/******************************************************************** 
 *  Function handler: vsi_info_get (section diag)
 ********************************************************************/
cmd_result_t
  cmd_ppd_api_diag_vsi_info_get(int unit, args_t* a)
{
  uint32
    ret;
  SOC_PPD_DIAG_VSI_INFO
    prm_vsi_info;
  uint32
    soc_sand_dev_id;
  int              prm_core = 0;
  parse_table_t    pt;

  soc_sand_dev_id = (unit);
  SOC_PPD_DIAG_VSI_INFO_clear(&prm_vsi_info);

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);

  /* Get parameters */
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Call function */
  ret = soc_ppd_diag_vsi_info_get(
          soc_sand_dev_id,
          prm_core,
          &prm_vsi_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    return CMD_FAIL;
  }

  SOC_PPD_DIAG_VSI_INFO_print(&prm_vsi_info);
  return CMD_OK;
}

/******************************************************************** 
 *  Function handler: rif_info_get (section diag)
 ********************************************************************/
cmd_result_t
    cmd_ppd_api_diag_rif_info_get(int unit, args_t* a)
{
  uint32 ret;
  SOC_PPD_RIF_INFO rif_info;
  uint32 soc_sand_dev_id, rif_index, rif_profile_index;
  int size_of_signal_in_bits;
  uint32 val[24];
  int              prm_core = 0;
  parse_table_t    pt;

  soc_sand_dev_id = (unit);
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  SOC_PPD_RIF_INFO_clear(&rif_info);

  /* get last rif */
  arad_pp_signal_mngr_signal_get(soc_sand_dev_id, prm_core, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF, val, &size_of_signal_in_bits);

  rif_index = val[0];

  /* Get info */
  ret = soc_ppd_rif_info_get(soc_sand_dev_id, rif_index, &rif_info);
  
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK){
      return CMD_FAIL;
  }

  LOG_CLI((BSL_META_U(unit, "\nLast packet RIF used: %u\n\r"),rif_index));
  LOG_CLI((BSL_META_U(unit, "RIF Information: \n\r")));
  LOG_CLI((BSL_META_U(unit, "---------------: \n\r")));
  SOC_PPD_RIF_INFO_print(&rif_info);

  if (SOC_IS_JERICHO(unit)) {      
      arad_pp_signal_mngr_signal_get(soc_sand_dev_id, prm_core, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF_PROFILE, val, &size_of_signal_in_bits);
      rif_profile_index = val[0];
      if(rif_info.routing_enablers_bm_id != rif_profile_index) {
          LOG_CLI((BSL_META_U(unit,"\t WARNING!!! rif profile signal(%d) != rif profile used(%d)!!!! \n"), rif_profile_index, rif_info.routing_enablers_bm_id));
      }
  }
  
  return CMD_OK;
}
/********************************************************************
 *  Function handler: ipv4_mc_route_get (section diag)
 ********************************************************************/
int 
cmd_ppd_api_diag_ipv4_mc_route_get(int unit, args_t* a) 
{
    int i;
    uint32   ret;
    SOC_PPD_IP_ROUTING_TABLE_RANGE      prm_block_range_key;
    SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY     prm_keys[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO    prm_routes_info[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPD_FRWRD_IP_ROUTE_STATUS       prm_routes_status[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPD_FRWRD_IP_ROUTE_LOCATION     prm_routes_location[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    uint32                              prm_nof_entries=0;
    char decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
    char decimal_src_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];

    SOC_PPD_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range_key);
    prm_block_range_key.start.type = SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED;

    prm_block_range_key.entries_to_act =  DIAG_PP_GET_BLOCK_NOF_ENTRIES;
    prm_block_range_key.entries_to_scan = 32*1024;

    while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range_key.start.payload))
    {
        /* Call function */
        ret = soc_ppd_frwrd_ipv4_mc_route_get_block(unit,
                                                   &prm_block_range_key,
                                                    prm_keys,
                                                    prm_routes_info,
                                                    prm_routes_status,
                                                    prm_routes_location,
                                                   &prm_nof_entries);

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL;
        } 

        if (prm_nof_entries>0) {
            cli_out(" ----------------------------------------------------------------------------------\n\r"
                    "|                                    Routing Table                                 |\n\r"
                    "|----------------------------------------------------------------------------------|\n\r"
                    "|     Group       | Inrif |      Source        |  Fec   |   Status    |  Location  |\n\r"
                    "|                 |       |                    |        |             |            |\n\r");
            cli_out(" ---------------------------------------------------------------------------------- \n\r");
        }
        else
        {
            cli_out("Multicast routing table is empty.\n\r");
            return CMD_OK;
        }

        for(i = 0; i < prm_nof_entries; i++)
        {
            soc_sand_pp_ip_long_to_string(prm_keys[i].group, 1,decimal_ip);
            cli_out("| %-16s", decimal_ip);

            if(prm_keys[i].inrif_valid)
                cli_out("| %-6d", prm_keys[i].inrif);
            else
                cli_out("| %-6c", '-');

            soc_sand_pp_ip_long_to_string(prm_keys[i].source.ip_address, 1,decimal_src_ip);
            sal_sprintf(decimal_src_ip,"%1s/%-1u",decimal_src_ip,prm_keys[i].source.prefix_len);

            cli_out("| %-18s | %-6d | %-11s | %7s    |\n\r",
                    decimal_src_ip, prm_routes_info[i].dest_id.dest_val,
                    SOC_PPD_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]),
                    SOC_PPD_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));
        }
    }
    cli_out(" ---------------------------------------------------------------------------------- \n\r");
    return CMD_OK;
} 

cmd_result_t
    cmd_ppd_api_diag_lif_show(int unit, args_t* a)
{
#ifndef __KERNEL__
    char *lif_type;
    parse_table_t    pt;
    int lif_id = -1;
    int global_flag = 0;
    int internal_offset, bank_id;
    int32 params_num;
    int llif_id, glif_id = -1;
    uint32 ret;
    SOC_PPD_LIF_ENTRY_INFO lif_entry_info;
    int mem=0, copyno;
    char *table_type = NULL;

    parse_table_init(unit, &pt);
    /* User may omit type - "in" is default */
    parse_table_add(&pt, "type", PQ_STRING, (void *) "in", (void *)&lif_type, NULL);
    parse_table_add(&pt, "id",   PQ_DFL | PQ_INT, &lif_id,      &lif_id,      NULL);
    parse_table_add(&pt, "gl",   PQ_DFL | PQ_INT, &global_flag, &global_flag, NULL);

    params_num = parse_arg_eq(a, &pt);
    if (0 > params_num) {
        return CMD_USAGE;
    }

    /* No default for lif id */
    if(lif_id == -1) {
        cli_out("lif id was not specified\n");
        return CMD_OK;
    }

    if(!sal_strncasecmp(lif_type, "in", strlen(lif_type))) {
        /* Normalize string for below print */
        lif_type = "In";
        if(global_flag == 1) {
            glif_id = lif_id;
            if((ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                          _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, lif_id, &llif_id)) != BCM_E_NONE) {
                cli_out("Global %s_LIF:0x%x does not exist\n", lif_type, lif_id);
                return ret;
            }
        }
        else
            llif_id = lif_id;

        if((llif_id < 0) || (llif_id >= SOC_DPP_DEFS_GET(unit, nof_local_lifs))) {
            cli_out("Local %s_LIF:%d is out of range\n", lif_type, llif_id);
            return CMD_OK;
        }

        internal_offset = ARAD_PP_IN_LIF_TO_OFFSET_IN_BANK(unit, llif_id);
        bank_id         = ARAD_PP_IN_LIF_TO_BANK_ID(unit, llif_id);

        ret = soc_ppd_lif_table_entry_get(unit, llif_id, &lif_entry_info);
        if(SOC_SAND_FAILURE(ret)) {
            return CMD_FAIL;
        }

        /* Look for table and present appropriately */
        switch(lif_entry_info.type) {
            case SOC_PPC_LIF_ENTRY_TYPE_EMPTY:
                mem = PPDB_B_LIF_TABLEm;
                table_type = "BASIC";
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_AC:
            case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
            case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
                switch(lif_entry_info.value.ac.service_type) {
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP:
                        mem = PPDB_B_LIF_TABLE_AC_MPm;
                        table_type = "AC_MP";
                        break;
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC:
                        mem = PPDB_B_LIF_TABLE_AC_2_OUT_LIFm;
                        table_type = "AC_2_OUT_LIF";
                        break;
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE:
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB:
                        mem = PPDB_B_LIF_TABLE_AC_2_EEIm;
                        table_type = "AC_2_EEI";
                        break;
                    default:
                        cli_out("Illegal AC Service Type:%d", lif_entry_info.value.ac.service_type);
                        break;
                }
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_PWE:
                if(lif_entry_info.value.pwe.service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP) {
                    mem = PPDB_B_LIF_TABLE_LABEL_PWE_MPm;
                    table_type = "LABEL_PWE_MP";
                }
                else if(lif_entry_info.value.pwe.service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P) {
                    mem = PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm;
                    table_type = "LABEL_PWE_P2P";
                }
                else
                    cli_out("Illegal PWE Service Type:%d", lif_entry_info.value.pwe.service_type);
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_ISID:
                if(lif_entry_info.value.isid.service_type == SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP) {
                    mem = PPDB_B_LIF_TABLE_ISID_MPm;
                    table_type = "ISID_MP";
                }
                else if(lif_entry_info.value.isid.service_type == SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P) {
                    mem = PPDB_B_LIF_TABLE_ISID_P2Pm;
                    table_type = "ISID_P2P";
                }
                else
                    cli_out("Illegal ISID Service Type:%d", lif_entry_info.value.isid.service_type);
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
                mem = PPDB_B_LIF_TABLE_IP_TTm;
                table_type = "IP_TT";
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
                mem = PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm;
                table_type = "LABEL_PROTOCOL_OR_LSP";
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
                mem = PPDB_B_LIF_TABLE_TRILLm;
                table_type = "TRILL";
                break;
            case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
                switch(lif_entry_info.value.extender.service_type) {
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP:
                        mem = PPDB_B_LIF_TABLE_AC_MPm;
                        table_type = "AC_MP";
                        break;
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC:
                        mem = PPDB_B_LIF_TABLE_AC_2_OUT_LIFm;
                        table_type = "AC_2_OUT_LIF";
                        break;
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE:
                    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB:
                    default:
                        cli_out("Illegal Extender Service Type:%d", lif_entry_info.value.extender.service_type);
                        break;
                }
                break;

            default:
                cli_out("Unsupported LIF type:%d - Using default\n", lif_entry_info.type);
                mem = PPDB_B_LIF_TABLEm;
                table_type = "BASIC";
                break;
        }
        if(!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Memory:%d does not exist defaulting to PPDB_B_LIF_TABLE\n", mem);
            mem = PPDB_B_LIF_TABLEm;
        }
    }
    else if(!sal_strncasecmp(lif_type, "out", strlen(lif_type))) {
        int entry;
        ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE access_type;

        /* Normalize string for below print */
        lif_type = "Out";
        if(global_flag == 1) {
            glif_id = lif_id;
            if((ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                          _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, lif_id, &llif_id)) != BCM_E_NONE) {
                cli_out("Global %s_LIF:0x%x does not exist\n", lif_type, lif_id);
                return ret;
            }
        }
        else
            llif_id = lif_id;

        if((llif_id < 0) || (lif_id >= SOC_DPP_DEFS_GET(unit, nof_out_lifs))) {
            cli_out("Local %s_LIF:%d is out of range\n", lif_type, llif_id);
            return CMD_OK;
        }

        entry           = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(llif_id);
        internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry);
        bank_id         = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry);

        ret = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, llif_id, &access_type);
        if(SOC_SAND_FAILURE(ret)) {
            return CMD_FAIL;
        }

        switch(access_type) {
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE:
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA:
                mem = EPNI_DATA_FORMATm;
                table_type = "DATA_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER:
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_ARP_OVERLAY:
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER:
                mem  = EPNI_LINK_LAYER_OR_ARP_FORMATm;
                table_type = "LINK_LAYER_OR_ARP_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP:
                mem = EPNI_IPV4_TUNNEL_FORMATm;
                table_type = "IPV4_TUNNEL_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL:
                mem = EPNI_TRILL_FORMATm;
                table_type = "TRILL_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF:
                mem = EPNI_OUT_RIF_FORMATm;
                table_type = "OUT_RIF_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC:
                mem = EPNI_AC_FORMATm;
                table_type = "AC_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL:
                mem = EPNI_MPLS_PUSH_FORMATm;
                table_type = "MPLS_PUSH_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP:
                mem = EPNI_MPLS_SWAP_FORMATm;
                table_type = "MPLS_SWAP_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP:
                mem = EPNI_MPLS_POP_FORMATm;
                table_type = "MPLS_POP_FORMAT";
                break;
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ISID:
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6:
            case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS:          /* No such table - see details for specific one below */
            default:
                cli_out("Unsupported EEDB access type:%d - Using default\n", access_type);
                mem = EPNI_DATA_FORMATm;
                table_type = "DATA_FORMAT";
                break;
        }
        if(!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Memory:%d does not exist defaulting to EPNI_DATA_FORMAT\n", mem);
            mem = EPNI_DATA_FORMATm;
        }
    }
    else {
        cli_out("No LIF type:\"%s\"\n", lif_type);
        return CMD_OK;
    }

    if(glif_id != -1)
        cli_out("Global %s_LIF:%d -> ", lif_type, glif_id);
    cli_out("Local %s_LIF:%d -> Type:%s Bank:%d Offset:%d\n\n", lif_type, llif_id, table_type, bank_id, internal_offset);

    copyno = SOC_MEM_BLOCK_MIN(unit, mem);
    ret = dpp_do_dump_table(unit, mem, bank_id, copyno, internal_offset, 1, 0);
    return ret;

#else
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_OK;
#endif /* __KERNEL__ */
}

#define CMD_DIAG_PP_DEST_VALID 0x1L
#define CMD_DIAG_PP_TC_VALID 0x2L
#define CMD_DIAG_PP_DP_VALID 0x4L
#define CMD_DIAG_PP_METER_0_VALID 0x8L
#define CMD_DIAG_PP_METER_1_VALID 0x10L
#define CMD_DIAG_PP_DP_METER_CMD_VALID 0x20L
#define CMD_DIAG_PP_COUNTER_0_VALID 0x40L
#define CMD_DIAG_PP_COUNTER_1_VALID 0x80L
#define CMD_DIAG_PP_SNOOP_VALID 0x100L
#define CMD_DIAG_PP_MIRROR_VALID 0x200L
#define CMD_DIAG_PP_TRAP_VALID 0x400L

#define CMD_DIAG_PP_PACKET_ACTION_NUM_FIELDS 11

#define DECLARE_PACKET_ACTION_DESC() \
    char *packet_action_desc[] = { \
        "destination", \
        "TC", \
        "DP", \
        "Meter 0", \
        "Meter 1", \
        "DP meter cmd", \
        "Counter 0", \
        "Counter 1", \
        "Snoop action", \
        "Mirror profile", \
        "Trap code" }

#define PACKET_ACTION_DESC(i) packet_action_desc[i]

typedef struct cmd_diag_pp_packet_action_e {
    uint32 dest;
    uint32 tc;
    uint32 dp;
    uint32 meter0;
    uint32 meter1;
    uint32 dp_meter_cmd;
    uint32 counter0;
    uint32 counter1;
    uint32 snoop;
    uint32 mirror;
    uint32 trap;
    uint32 flags;
} cmd_diag_pp_packet_action_t;

#define DECLARE_IRPP_STAGE_DESC() \
    char *irpp_stage_desc[] = { \
        "port termination", \
        "parser", \
        "vtt", \
        "vtt", \
        "flp", \
        "pmf", \
        "fec resolution", \
        "tm" }

#define IRPP_STAGE_DESC(i) irpp_stage_desc[i]

typedef enum {
    diag_irpp_stage_port_termination,  
    diag_irpp_stage_parser,  
    diag_irpp_stage_vt,  
    diag_irpp_stage_tt,  
    diag_irpp_stage_flp,  
    diag_irpp_stage_pmf,  
    diag_irpp_stage_fec,  
    diag_irpp_stage_tm,  
    diag_irpp_stages  
} cmd_diag_pp_irpp_stage_e;

#define CMD_DIAG_PP_IRPP_STAGES 8

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_port_termination(int unit, int core)
{
    uint32 ret;
    
    SOC_PPD_DIAG_RECEIVED_PACKET_INFO    
      prm_rcvd_pkt_info;

    ret = soc_ppd_diag_received_packet_info_get(
            unit,
            core,
            &prm_rcvd_pkt_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    } 
    
    /* port termination */
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "TM port", prm_rcvd_pkt_info.in_tm_port));
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "PP port", prm_rcvd_pkt_info.in_pp_port));
    
    if (prm_rcvd_pkt_info.src_sys_port.sys_port_type != SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT)
    {
      LOG_CLI((BSL_META_U(unit, "%-16s: %d(%s)\n\r"), "System port", prm_rcvd_pkt_info.src_sys_port.sys_id,
        soc_sand_SAND_PP_SYS_PORT_TYPE_to_string(prm_rcvd_pkt_info.src_sys_port.sys_port_type)));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit, "%-16s: %d(0x%x)\n\r"), "System port", prm_rcvd_pkt_info.src_sys_port.sys_id, 
        prm_rcvd_pkt_info.src_sys_port.sys_id));
    }

    return CMD_OK;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_parser(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution)
{
    uint32 ret;

    SOC_PPD_DIAG_PARSING_INFO    
      prm_pars_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_parsing_info_get(
            unit,
            core,
            &prm_pars_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL;     
    } 

    {
      const char* str = NULL;
      switch(prm_pars_info.hdr_type)
      {
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH:
        str = "ETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH_ETH:
        str = "ETHoETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV4_ETH:
        str = "IPv4oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH_IPV4_ETH:
        str = "EHToIPv4oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV6_ETH:
        str = "IPv6oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_MPLS1_ETH:
        str = "MPLSx1oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_FC_ENCAP_ETH:
        str = "FCwithENCAPoETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_FC_STD_ETH:
        str = "FCoETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_MPLS2_ETH:
        str = "MPLSx2oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_MPLS3_ETH:
        str = "MPLSx3oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH_TRILL_ETH:
        str = "ETHoTRILLoETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH:
        str = "ETHoMPLSx1oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH:
        str = "ETHoMPLSx2oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH:
        str = "ETHoMPLSx3oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH:
        str = "IPv4oIPv4oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH:
        str = "IPv4oMPLSx1oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH:
        str = "IPv4oMPLSx2oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH:
        str = "IPv4oMPLSx3oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH:
        str = "IPv6oIPv4oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH:
        str = "IPv6oMPLSx1oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH:
        str = "IPv6oMPLSx2oETH";
      break;
      case SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH:
        str = "IPv6oMPLSx3oETH";
      break;
      default:
        str = "UNKNOWN";
      }
      
      LOG_CLI((BSL_META_U(unit, "%-16s: %s\n\r"), "Pkt format", str));
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Initial VID", prm_pars_info.initial_vid));
    
    /* Initial TC */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,117,115,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,102,100,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Initial TC", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = regs_val[0];
    
    /* Initial DP */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,122,121,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,107,106,20);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Initial DP", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = regs_val[0];

    /* trap */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,146,139,30);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,131,124,30);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Trap code", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_TRAP_VALID;
    action_resolution->trap = regs_val[0];

    /* mirror */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,68,65,40);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,55,52,40);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Mirror profile", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = regs_val[0];

    /* snoop */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,63,56,50);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,50,43,50);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop code", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop = regs_val[0];
    
    return CMD_OK;
exit:
  return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_vtt(int unit, int core)
{
    uint32 ret;

    SOC_PPD_DIAG_VSI_INFO
      prm_vsi_info;
    
    SOC_PPD_DIAG_VLAN_EDIT_RES     
      prm_vec_res;
    
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    /* local LIF ID */
    if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,1,140,124,30);
      LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Local LIF ID", regs_val[0]));
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,1,245,230,30);
        LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "LIF ID", regs_val[0]));
    }
      
    /* System LIF ID */
    if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,40,23,40);
      LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "System LIF ID", regs_val[0]));
    }
    
    /* VSI */
    ret = soc_ppd_diag_vsi_info_get(
            unit,
            core,
            &prm_vsi_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      return CMD_FAIL;
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "VSI", prm_vsi_info.vsi));
    
    ret = soc_ppd_diag_ing_vlan_edit_info_get(
            unit,
            core,
            &prm_vec_res
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
          return CMD_FAIL; 
    } 
    
    if ( prm_vec_res.cmd_info.tags_to_remove || prm_vec_res.cmd_info.outer_tag.vid_source || prm_vec_res.cmd_info.inner_tag.vid_source )
    {
      LOG_CLI((BSL_META_U(unit, "%-16s: %-16s|%-16s|%-16s\n\r"), "IVE", "Command ID", "VID 0", "VID 1"));
      LOG_CLI((BSL_META_U(unit, "%-16s  %-16u %-16u %-16u\n\r"), "", prm_vec_res.cmd_id, prm_vec_res.ac_tag.vid, prm_vec_res.ac_tag2.vid));
      LOG_CLI((BSL_META_U(unit, "%-16s: %-16s|%-16s\n\r"), "", "Tags to remove", "Tpid profile"));
      LOG_CLI((BSL_META_U(unit, "%-16s  %-16u %-16u\n\r"), "", prm_vec_res.cmd_info.tags_to_remove, prm_vec_res.cmd_info.tpid_profile));
    
      if (prm_vec_res.cmd_info.outer_tag.vid_source)
      {
        LOG_CLI((BSL_META_U(unit, "%-16s: %-16s|%-16s|%-16s\n\r"), "", "Outer TPID index", "  VID src", "  PCP-DEI src"));
        LOG_CLI((BSL_META_U(unit, "%-16s  %-16u %-16s %-16s\n\r"), "", prm_vec_res.cmd_info.outer_tag.tpid_index,
             SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(prm_vec_res.cmd_info.outer_tag.vid_source),
             SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(prm_vec_res.cmd_info.outer_tag.pcp_dei_source)));
      }
      
      if (prm_vec_res.cmd_info.inner_tag.vid_source)
      {
        LOG_CLI((BSL_META_U(unit, "%-16s: %-16s|%-16s|%-16s\n\r"), "", "Inner TPID index", "  VID src", "  PCP-DEI src"));
        LOG_CLI((BSL_META_U(unit, "%-16s  %-16u %-16s %-16s\n\r"), "", prm_vec_res.cmd_info.inner_tag.tpid_index,
             SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(prm_vec_res.cmd_info.inner_tag.vid_source),
             SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(prm_vec_res.cmd_info.inner_tag.pcp_dei_source)));
      }
    }
    else
      {
        if (prm_vec_res.cmd_info.tpid_profile)
        {
          LOG_CLI((BSL_META_U(unit, "%-16s: %s%d\n\r"), "IVE", "tpid profile ", prm_vec_res.cmd_info.tpid_profile));
        }
      }

    return CMD_OK;
    
exit:
      return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_tt(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution)
{
    uint32 ret;

    SOC_PPD_DIAG_TERM_INFO     
      prm_term_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_termination_info_get(
            unit,
            core,
            &prm_term_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL;  
    }
    
    if (prm_term_info.term_type != SOC_PPC_PKT_TERM_TYPE_NONE)
    {
      LOG_CLI((BSL_META_U(unit, "%-16s: %s\n\r"), "Hdr terminated", SOC_PPC_PKT_TERM_TYPE_to_string(prm_term_info.term_type)));
    }

    /* destination */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,86,68,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,2,35,17,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "FWD destination", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_DEST_VALID;
    action_resolution->dest = regs_val[0];

    /* TC */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,58,56,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,0,58,56,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "tc", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = regs_val[0];

    /* DP */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,62,61,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,0,62,61,20);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "dp", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = regs_val[0];

    /* TRAP */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,110,103,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,2,59,52,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Trap code", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_TRAP_VALID;
    action_resolution->trap = regs_val[0];

    /* mirror */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,1,144,141,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,55,52,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Mirror profile", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = regs_val[0];

    /* snoop */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,1,82,75,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,1,163,160,20);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop code", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop = regs_val[0];

    return CMD_OK;
        
exit:
    return CMD_FAIL;
}


STATIC void
  cmd_ppd_api_FRWRD_LKUP_KEY_print(
    int unit,
    SOC_PPC_DIAG_FRWRD_LKUP_KEY *info,
    SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type,
    uint8                      is_kbp
  )
{
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(frwrd_type)
  {
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
    {
        SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *key;

        key = &(info->mact.key_val.mac);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { DA, FID }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  %s"), "", "{ "));
        soc_sand_SAND_PP_MAC_ADDRESS_print(&(key->mac));
        LOG_CLI((BSL_META_U(unit,
                            ", %d }\n\r"), key->fid));
    }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
    {
        SOC_PPC_BMACT_ENTRY_KEY *key;
    
        key = &(info->bmact);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { BMAC, BVID }"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { "), ""));
        soc_sand_SAND_PP_MAC_ADDRESS_print(&(key->b_mac_addr));
        LOG_CLI((BSL_META_U(unit,
                            ", %d}\n\r"), key->b_vid));
    }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        SOC_PPC_DIAG_IPV4_UNICAST_RPF *key;

        key = &(info->kbp_ipv4_unicast_rpf);
        
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { VRF, SIP, DIP }, { VRF, SIP }, { VRF, DIP }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %s, %s }\n\r"), "", key->vrf, 
                            soc_sand_pp_ip_long_to_string(key->sip,1,decimal_ip),
                            soc_sand_pp_ip_long_to_string(key->dip,1,decimal_ip)));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %s }\n\r"), "", key->vrf, 
                            soc_sand_pp_ip_long_to_string(key->sip,1,decimal_ip)));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %s }\n\r"), "", key->vrf, 
                            soc_sand_pp_ip_long_to_string(key->dip,1,decimal_ip)));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      { 
        SOC_PPC_DIAG_IPV4_VPN_KEY *key;

        key = &(info->ipv4_uc);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { VRF, DIP }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %s }\n\r"), "", key->vrf, 
                            soc_sand_pp_ip_long_to_string(key->key.subnet.ip_address,1,decimal_ip)));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
        { 
          SOC_PPC_DIAG_IPV4_VPN_KEY *key;
        
          key = &(info->ipv4_uc);
        
          LOG_CLI((BSL_META_U(unit,
                            "%-16s: { VRF, DIP }\n\r"), "fwd lookup key"));
          LOG_CLI((BSL_META_U(unit,
                              "%-16s  { %d, %s }\n\r"), "", key->vrf, 
                              soc_sand_pp_ip_long_to_string(key->key.subnet.ip_address,1,decimal_ip)));
        }
      break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
          SOC_PPC_DIAG_IPV4_MULTICAST *key;
          
          key = &(info->kbp_ipv4_multicast);
          
          LOG_CLI((BSL_META_U(unit,
                              "%-16s: { VRF, InRIF, SIP, DIP }, { InRif, DIP }, { VRF, SIP }\n\r"), "fwd lookup key"));
          LOG_CLI((BSL_META_U(unit,
                              "%-16s  { %d, %d, %s, %s }\n\r"), "", key->vrf, key->in_rif,
                              soc_sand_pp_ip_long_to_string(key->sip,1,decimal_ip),
                              soc_sand_pp_ip_long_to_string(key->dip,1,decimal_ip)));
          LOG_CLI((BSL_META_U(unit,
                              "%-16s  { %d, %s }\n\r"), "", key->in_rif, 
                              soc_sand_pp_ip_long_to_string(key->dip,1,decimal_ip)));
          LOG_CLI((BSL_META_U(unit,
                              "%-16s  { %d, %s }\n\r"), "", key->vrf, 
                              soc_sand_pp_ip_long_to_string(key->sip,1,decimal_ip)));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
          SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY *key;

          key = &(info->ipv4_mc);
          LOG_CLI((BSL_META_U(unit,
                              "%-16s: { group, InRIF(Valid?), DIP }\n\r"), "fwd lookup key"));
          LOG_CLI((BSL_META_U(unit,
                              "%-16s  { %d, %d(%s), %s }\n\r"), "", key->group, key->inrif, key->inrif_valid?"V":"X",
                              soc_sand_pp_ip_long_to_string(key->source.ip_address,1,decimal_ip)));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
          SOC_PPC_DIAG_IPV6_UNICAST_RPF *key;

          key = &(info->kbp_ipv6_unicast_rpf);
          LOG_CLI((BSL_META_U(unit,
                              "%-16s: { VRF, SIP, DIP }, { VRF, DIP }, { VRF, SIP }\n\r"), "fwd lookup key"));
          LOG_CLI((BSL_META_U(unit,
                              "%-16s  { %d, "), "", key->vrf));
          soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
          LOG_CLI((BSL_META_U(unit,
                              ", ")));
          soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->dip);
          LOG_CLI((BSL_META_U(unit,
                              "}\n\r")));

          LOG_CLI((BSL_META_U(unit,
                              "%-16s  {%d, "), "", key->vrf));
          soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->dip);
          LOG_CLI((BSL_META_U(unit,
                              "}\n\r")));

          LOG_CLI((BSL_META_U(unit,
                              "%-16s  {%d, "), "", key->vrf));
          soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
          LOG_CLI((BSL_META_U(unit,
                              "}\n\r")));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
      SOC_PPC_DIAG_IPV6_VPN_KEY *key;

      key = &(info->ipv6_uc);
      
      LOG_CLI((BSL_META_U(unit,
                          "%-16s: { VRF, DIP(prefix) }\n\r"), "fwd lookup key"));
      LOG_CLI((BSL_META_U(unit,
                          "%-16s  { %d, "), "", key->vrf));
      soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->key.subnet.ipv6_address);
      LOG_CLI((BSL_META_U(unit,
                        "(%u) }\n\r"), key->key.subnet.prefix_len));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        SOC_PPC_DIAG_IPV6_MULTICAST *key;
        
        key = &(info->kbp_ipv6_multicast);

        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { VRF, InRIF, SIP, DIP }, { InRIF, SIP, DIP }, { VRF, SIP }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %d, "), "", key->vrf, key->in_rif));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
        LOG_CLI((BSL_META_U(unit,
                            ", ")));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->dip);
        LOG_CLI((BSL_META_U(unit,
                            " }\n\r")));

        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, "), "", key->in_rif));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
        LOG_CLI((BSL_META_U(unit,
                            ", ")));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->dip);
        LOG_CLI((BSL_META_U(unit,
                            " }\n\r")));

        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, "), "", key->vrf));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
        LOG_CLI((BSL_META_U(unit,
                            " }\n\r")));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *key;

        key = &(info->ipv6_mc);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { DIP }\n\r"), "fwd lookup key"));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&(key->group));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { "), ""));
        soc_sand_SAND_PP_IPV6_ADDRESS_print(&(key->group));
        LOG_CLI((BSL_META_U(unit,
                            " }\n\r")));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        SOC_PPC_DIAG_MPLS *key;

        key = &(info->kbp_mpls);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { InRIF, InPort, exp, label }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %d, %d, %d }\n\r"), "", key->in_rif, key->in_port, key->exp, key->mpls_label));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
      SOC_PPC_FRWRD_ILM_KEY *key;

      key = &(info->ilm);
      LOG_CLI((BSL_META_U(unit,
                          "%-16s: { InRIF, InPort, exp, label }\n\r"), "fwd lookup key"));
      LOG_CLI((BSL_META_U(unit,
                          "%-16s  { %d, %d, %d, %d }\n\r"), "key", key->inrif, key->in_local_port, key->mapped_exp, key->in_label));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        SOC_PPC_DIAG_TRILL_UNICAST *key;

        key = &(info->kbp_trill_unicast);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { egress nick(egress nick hexadecimal)}\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s { %d(0x%x) }\n\r"), "", key->egress_nick, key->egress_nick));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { Trill UC(Trill UC hexadecimal) }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d(0x%x) }\n\r"), "", info->trill_uc, info->trill_uc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        SOC_PPC_DIAG_TRILL_MULTICAST *key;

        key = &(info->kbp_trill_multicast);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { Esdai, FID, Dist Tree Nick }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %d, %d, %d }\n\r"), "key", key->esdai, key->fid_vsi, key->dist_tree_nick));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
      SOC_PPC_TRILL_MC_ROUTE_KEY *key;

      key = &(info->trill_mc);
      LOG_CLI((BSL_META_U(unit,
                          "%-16s: { Esadi, FID, Tree Nick, Ing Nick, Adj EEP, TTS }\n\r"), " fwd lookup key"));
      LOG_CLI((BSL_META_U(unit,
                          "%-16s  { %u, %u, %u, %u, %u, %u }\n\r"), "key", key->esadi, key->fid, key->tree_nick, key->ing_nick, key->adjacent_eep, key->tts));
      }
    break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF:
    {
        SOC_PPC_FRWRD_FCF_ROUTE_KEY *key;

        key = &(info->fcf);
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: { flags, VFI, DID, prefix length }\n\r"), "fwd lookup key"));
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  { %u, %u, %u, %u }\n\r"), "", key->flags, key->vfi, key->d_id, key->prefix_len));
    }
  break;
  default:
    LOG_CLI((BSL_META_U(unit,
                        "%-16s: { 0x%08x %08x }\n\r"),"key",info->raw[0], info->raw[1]));
    break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC void
cmd_ppd_api_FRWRD_DECISION_INFO_print(int unit, int add_leading_space, SOC_PPC_FRWRD_DECISION_INFO *info)
{
    if (add_leading_space) {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  %s %d\n\r"), "", 
                            SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                            info->dest_id));
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                            "%s %d\n\r"),  
                            SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                            info->dest_id));
    }
    
    switch(info->type)
    {
    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
    case SOC_PPC_FRWRD_DECISION_TYPE_MC:
        {
            SOC_PPC_EEI *eei;
            
            eei = &(info->additional_info.eei);
            switch(eei->type)
            {
            case SOC_PPC_EEI_TYPE_MPLS:
                {
                    SOC_PPC_MPLS_COMMAND *mpls_cmd;
    
                    mpls_cmd = &(eei->val.mpls_command);
                    LOG_CLI((BSL_META_U(unit,
                                        "%-16s  %s %s label %u profile %u\n\r"), "", "eei - mpls", 
                                        SOC_PPC_MPLS_COMMAND_TYPE_to_string(mpls_cmd->command),
                                        mpls_cmd->label,
                                        mpls_cmd->push_profile));
                }
                break;
            case SOC_PPC_EEI_TYPE_TRILL:
                {
                    SOC_SAND_PP_TRILL_DEST *trill_dest;
    
                    trill_dest = &(eei->val.trill_dest);
                    LOG_CLI((BSL_META_U(unit,
                                        "%-16s  %s%s dest nick %u\n\r"), "", "eei - trill", 
                                        (trill_dest->is_multicast)?" multicast":"",
                                        trill_dest->dest_nick));
                }
                break;
            case SOC_PPC_EEI_TYPE_MIM:
                LOG_CLI((BSL_META_U(unit,
                              "%-16s  %s %u\n\r"),"", "eei - MiM ISID", eei->val.isid));
                break;
            default:
                break;
            }
        }
        break;
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
        LOG_CLI((BSL_META_U(unit,
                            "%-16s  %s %d\n\r"), "", 
                            "outlif",
                            info->additional_info.outlif.val));
        break;
    case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
        {
            SOC_PPC_TRAP_INFO *trap_info;
    
            trap_info = &(info->additional_info.trap_info);
                
            LOG_CLI((BSL_META_U(unit,
                            "%-16s  %s %d fwd act strength %u snoop act strength %u\n\r"), "", 
                            "trap - trap code",
                            trap_info->action_profile.trap_code,
                            trap_info->action_profile.frwrd_action_strength,
                            trap_info->action_profile.snoop_action_strength));
        }
        break;
    default:
        break;
    }
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_flp(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution)
{
    uint32 ret;

    SOC_PPD_DIAG_FRWRD_LKUP_INFO   
      prm_frwrd_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32 snoop_code;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_frwrd_lkup_info_get(
              unit,
              core,
              &prm_frwrd_info
            );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    } 
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "fwd hdr offset", prm_frwrd_info.frwrd_hdr_index));
    
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
    LOG_CLI((BSL_META_U(unit, "%-16s: %s\n\r"), "fwd lookup in", prm_frwrd_info.is_kbp?"KBP":"LEM"));
    LOG_CLI((BSL_META_U(unit, "%-16s: %s\n\r"), "fwd lookup type", SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(prm_frwrd_info.frwrd_type)));
    cmd_ppd_api_FRWRD_LKUP_KEY_print(unit, &(prm_frwrd_info.lkup_key),prm_frwrd_info.frwrd_type, prm_frwrd_info.is_kbp);
#else
    LOG_CLI((BSL_META_U(unit, "%-16s: %s\n\r"), "fwd lookup in", "LEM"));
    LOG_CLI((BSL_META_U(unit, "%-16s: %s\n\r"), "fwd lookup type", SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(prm_frwrd_info.frwrd_type)));
    cmd_ppd_api_FRWRD_LKUP_KEY_print(unit, &(prm_frwrd_info.lkup_key),prm_frwrd_info.frwrd_type, FALSE);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
    
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
    if(prm_frwrd_info.is_kbp) {
        SOC_PPC_DIAG_IP_REPLY_RECORD *result;

        result = &(prm_frwrd_info.ip_reply_record);

        if (result->match_status)
        {
            LOG_CLI((BSL_META_U(unit,
                                "%-16s: [%s|%s]\n\r"), "result 0", result->is_synamic?"D":"S", result->p2p_service?"P":"M"));

            switch (result->identifier)
            {
            case 0:
                cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, 1, &result->destination);
                if (result->out_lif_valid) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%-16s  %s %d\n\r"), "", 
                                        "outlif",
                                        result->out_lif));
                }
                break;
            case 1:
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s  FEC %d EEI %d\n\r"), "", 
                                    result->fec_ptr,
                                    result->eei));
                break;
            default:
                break;
            }
        }
        else
        {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: not found\n\r"), "result 0"));
        }

        {
            SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD *info;

            info = &(prm_frwrd_info.second_ip_reply_result);
            if(info->match_status) {
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: FEC %d\n\r"), "result 1", info->dest_id));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: not found\n\r"), "result 1"));
            }
        }
    } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
    {
        if (prm_frwrd_info.key_found)
        {
            switch(prm_frwrd_info.frwrd_type)
            {
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
              {
                SOC_PPC_FRWRD_MACT_ENTRY_VALUE *info;

                info = &(prm_frwrd_info.lkup_res.mact);
                
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: "), "result"));
                cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, 0, &(info->frwrd_info.forward_decision));

                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: "), "flags"));

                if (info->frwrd_info.drop_when_sa_is_known)
                {
                LOG_CLI((BSL_META_U(unit,
                                    "drop when SA is known |")));
                }

                LOG_CLI((BSL_META_U(unit,
                                    "%s|"), (info->aging_info.is_dynamic)?"D":"S"));
                LOG_CLI((BSL_META_U(unit,
                                    "%u|"), info->aging_info.age_status));

                if (info->accessed)
                {
                    LOG_CLI((BSL_META_U(unit,
                                        "A|")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%u\n\r"),info->group));
              
#ifdef BCM_88660_A0
                {
                SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB *slb;

                slb = &(info->slb_info);
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: FEC entries %d|LAG entries %d|LAG&member 0x%08x|FEC 0x%08x\n\r"), "slb",
                                    slb->match_fec_entries,
                                    slb->match_lag_entries,
                                    slb->lag_group_and_member,
                                    slb->fec));
                }
#endif /* BCM_88660_A0 */
              }
            break;
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
              {
                SOC_PPC_BMACT_ENTRY_INFO *bmact;

                bmact = &(prm_frwrd_info.lkup_res.bmact);
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: system port %u|ISID %u|SA learn FEC %u|drop SA %u\n\r"), "result",
                                    bmact->sys_port_id,
                                    bmact->i_sid_domain,
                                    bmact->sa_learn_fec_id,
                                    bmact->drop_sa));
              }
            break;
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
                LOG_CLI((BSL_META_U(unit,
                                    "%-16s: "), "result"));
                cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, 0, &(prm_frwrd_info.lkup_res.frwrd_decision));
            break;
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
                {
                    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *info;

                    info = &(prm_frwrd_info.lkup_res.host_info);

                    if (info->eep != SOC_PPC_EEP_NULL)
                    {
                        LOG_CLI((BSL_META_U(unit,
                                            "%-16s: FEC %u EEP %u\n\r"), "result", info->fec_id, info->eep));
                    }
                    else
                    {
                        LOG_CLI((BSL_META_U(unit,
                                            "%-16s: FEC %u\n\r"), "result", info->fec_id));
                    }
                    cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, 1, &(info->frwrd_decision));
                }
            break;
            default:
              LOG_CLI((BSL_META_U(unit,
                                  "%-16s: 0x%08x 0x%08x\n\r"),
                                  "result",
                                  prm_frwrd_info.lkup_res.raw[0],
                                  prm_frwrd_info.lkup_res.raw[1]));
              break;
            }
        }
        else
        {
            LOG_CLI((BSL_META_U(unit,
                                "%-16s: not found\n\r"), "result"));
        }
    }

    /* TC */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,8,101,99,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,7,158,156,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "tc", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = regs_val[0];

    /* DP */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,8,108,107,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,7,165, 164,20);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "dp", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = regs_val[0];

    /* counter 0 */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,194,194,20);

        if (regs_val[0] != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,236,216,20);

            LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "counter 0", regs_val[0]));

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_0_VALID;
            action_resolution->counter0 = regs_val[0];            
        }
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,8,123,123,20);

        if (regs_val[0] != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,0,8,165,145,20);

            LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "counter 0", regs_val[0]));

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_0_VALID;
            action_resolution->counter0 = regs_val[0];            
        }
    }   

    /* counter 1 */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,193,193,20);

        if (regs_val[0] != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,215,195,20);

            LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "counter 1", regs_val[0]));

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_1_VALID;
            action_resolution->counter1 = regs_val[0];            
        }
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,8,122,122,20);

        if (regs_val[0] != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,0,8,144,124,20);

            LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "counter 1", regs_val[0]));

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_1_VALID;
            action_resolution->counter1 = regs_val[0];            
        }
    }
    
    /* TRAP */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,8,151,144,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,7,208,201,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Trap code", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_TRAP_VALID;
    action_resolution->trap = regs_val[0];

    /* mirror */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,6,226,223,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,6,101,98,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Mirror profile", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = regs_val[0];

    /* snoop */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,1,255,252,20);
        snoop_code = regs_val[0];        
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,2,3,0,20);
        snoop_code |= (regs_val[0]) << 4;        
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,1,180,173,20);
        snoop_code = regs_val[0];        
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop code", snoop_code));

    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop = regs_val[0];

    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_pmf(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution)
{
    uint32 ret;
    uint32
      ind2,
      ind;

    SOC_PPD_FP_PACKET_DIAG_INFO   
      *prm_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32 mirror_profile;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    prm_info = sal_alloc(sizeof(SOC_PPD_FP_PACKET_DIAG_INFO), "cmd_ppd_api_fp_packet_diag_get.prm_info");
    if(!prm_info) {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    SOC_PPD_FP_PACKET_DIAG_INFO_clear(prm_info);
    
    ret = soc_ppd_fp_packet_diag_get(
            unit,
            core,
            prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        sal_free(prm_info);
        return CMD_FAIL; 
    } 
    
    LOG_CLI((BSL_META_U(unit,
                        "%-16s: [db id] type val\n\r"),
                        "PMF actions"));

    for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
    {
        for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACRO_SIMPLES; ++ind2)
        {
          SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE *macro;

          macro = &(prm_info->macro_simple[ind][ind2]);
          if ((macro->action.type == SOC_PPC_FP_ACTION_TYPE_INVALID) ||
            (macro->action.type == SOC_PPC_FP_ACTION_TYPE_NOP))
          {
            continue;
          }

          LOG_CLI((BSL_META_U(unit,
                              "%-16s  [%u] %s %u(0x%x)\n\r"), "", macro->db_id,
                              SOC_PPC_FP_ACTION_TYPE_to_string(macro->action.type),
                              macro->action.val,
                              macro->action.val));
        }
    
      for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACROS; ++ind2)
      {
        SOC_PPC_FP_PACKET_DIAG_MACRO *macro;

        macro = &(prm_info->macro[ind][ind2]);
        if ((macro->action.type == SOC_PPC_FP_ACTION_TYPE_INVALID) ||
            (macro->action.type == SOC_PPC_FP_ACTION_TYPE_NOP))
        {
          continue;
        }

        LOG_CLI((BSL_META_U(unit,
                            "%-16s  [%u/%u] %s %u(0x%x)"), "", macro->db_id,
                            macro->entry_id,
                            SOC_PPC_FP_ACTION_TYPE_to_string(macro->action.type),
                            macro->action.val,
                            macro->action.val));

        if (macro->qual_mask.type != SOC_PPC_NOF_FP_QUAL_TYPES)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%s 0x%x 0x%x\n\r"),
                              SOC_PPC_FP_QUAL_TYPE_to_string(macro->qual_mask.type),
                              macro->qual_mask.val[0],
                              macro->qual_mask.val[1]));
        }
        else
        {
            LOG_CLI((BSL_META_U(unit, "\n\r")));
        }
      }
    }
    
    sal_free(prm_info);

    /* mirror */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,9,255,254,10);
        mirror_profile = regs_val[0];
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,10,1,0,10);
        mirror_profile |= regs_val[0] << 2;
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,9,9,233,230,10);
        mirror_profile = regs_val[0];
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Mirror profile", mirror_profile));

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = regs_val[0];

    /* snoop */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,0,9,144,137,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,9,9,141,134,20);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop code", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop = regs_val[0];

    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_fec(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution)
{
    uint32 ret;
    SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO     
      prm_frwrd_trace_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_frwrd_decision_trace_get(
            unit,
            core,
            &prm_frwrd_trace_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      return CMD_FAIL;
    } 
    
    /* FEC */
    if (prm_frwrd_trace_info.frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.type == ARAD_PP_FRWRD_DECISION_TYPE_FEC)
    {
        LOG_CLI((BSL_META_U(unit, "%-16s: %d accessed\n\r"),"FEC",
            prm_frwrd_trace_info.frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.dest_id));
    }

    /* mirror */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,2,1,93,90,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,10,1,76,73,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Mirror profile", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = regs_val[0];

    /* snoop */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,2,1,20,13,10);
        LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop code", regs_val[0]));
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,2,1,24,21,10);
        LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop action", regs_val[0]));
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,10,1,12,9,20);
        LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Snoop action", regs_val[0]));
    }  

    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop = regs_val[0];

    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_tm(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution)
{
    uint32 ret;

    SOC_PPD_DIAG_PKT_TM_INFO   
      prm_pkt_tm_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_pkt_associated_tm_info_get(
            unit,
            core,
            &prm_pkt_tm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
          return CMD_FAIL; 
    } 
    
    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DEST)
    {
      LOG_CLI((BSL_META_U(unit,
                            "%-16s: "), "FWD destination"));
      cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, 0, &(prm_pkt_tm_info.frwrd_decision));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_TC)
    {
      LOG_CLI((BSL_META_U(unit,
                          "%-16s: %u\n\r"), "tc", prm_pkt_tm_info.tc));
    }

    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = prm_pkt_tm_info.tc;

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DP)
    {
      LOG_CLI((BSL_META_U(unit,
                          "%-16s: %u\n\r"), "dp", prm_pkt_tm_info.dp));
    }

    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = prm_pkt_tm_info.dp;

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "meter 0", prm_pkt_tm_info.meter1));

        action_resolution->flags |= CMD_DIAG_PP_METER_0_VALID;
        action_resolution->meter0 = prm_pkt_tm_info.meter1;
    }
    else
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: N/A\n\r"), "meter 0"));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "meter 1", prm_pkt_tm_info.meter2));

        action_resolution->flags |= CMD_DIAG_PP_METER_1_VALID;
        action_resolution->meter1 = prm_pkt_tm_info.meter2;
    }
    else
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: N/A\n\r"), "meter 1"));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "dp meter cmd", prm_pkt_tm_info.dp_meter_cmd));
        action_resolution->flags |= CMD_DIAG_PP_DP_METER_CMD_VALID;
        action_resolution->dp_meter_cmd = prm_pkt_tm_info.dp_meter_cmd;
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "counter 0", prm_pkt_tm_info.counter1));
        action_resolution->flags |= CMD_DIAG_PP_COUNTER_0_VALID;
        action_resolution->counter0 = prm_pkt_tm_info.counter1;
    }
    else
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: N/A\n\r"), "counter 0"));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "counter 1", prm_pkt_tm_info.counter2));
        action_resolution->flags |= CMD_DIAG_PP_COUNTER_1_VALID;
        action_resolution->counter1 = prm_pkt_tm_info.counter2;
    }
    else
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: N/A\n\r"), "counter 1"));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CUD)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "cud", prm_pkt_tm_info.cud));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "ETH policer", prm_pkt_tm_info.eth_meter_ptr));
    }
    else
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: N/A\n\r"), "ETH policer"));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "ingress shp dest", prm_pkt_tm_info.ingress_shaping_da));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "ECN capable", prm_pkt_tm_info.ecn_capable));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CNI)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "CNI", prm_pkt_tm_info.cni));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE)
    {
        const char* str = NULL;
        switch(prm_pkt_tm_info.da_type) 
        {
        case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC:
          str = "UC";
        break;
        case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC:
          str = "MC";
        break;
        case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC:
          str = "BC";
        break;
        default:
          str = "Unknown";
        }

        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %s\n\r"), "DA type", str));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "ST VSQ", prm_pkt_tm_info.st_vsq_ptr));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "LAG LB key", prm_pkt_tm_info.lag_lb_key));
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "ignore CP", prm_pkt_tm_info.ignore_cp));
    }

    /* mirror */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,4,0,126,123,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,12,0,124,121,10);
    }
    
    LOG_CLI((BSL_META_U(unit, "%-16s: %d\n\r"), "Mirror profile", regs_val[0]));

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = regs_val[0];


    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,4,1,62,55,20);
    }
    else {
        
    }

    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop = regs_val[0];

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-16s: %u\n\r"), "Snoop action", prm_pkt_tm_info.snoop_id));
    }

    return CMD_OK;

exit:
    return CMD_FAIL;
}

cmd_result_t
    cmd_ppd_api_diag_pp_last(int unit, args_t* a)
{
  parse_table_t    pt;
  int prm_core = 0;
  int32 params_num;
  cmd_result_t cmd_result;
  cmd_diag_pp_packet_action_t packet_action[CMD_DIAG_PP_IRPP_STAGES];

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0,      &prm_core,      NULL);

  params_num = parse_arg_eq(a, &pt);
  if (0 > params_num) {
    return CMD_USAGE;
  }

  sal_memset(packet_action, 0, sizeof(packet_action));

  LOG_CLI((BSL_META_U(unit, "        \033[1m\033[4mLast packet IRPP information:\033[0m \n\r")));

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mPort termination information\033[0m \n\r"))); */
  /* port termination */
  if ((cmd_result = cmd_ppd_api_diag_pp_last_port_termination(unit, prm_core)) != CMD_OK) {
      return cmd_result;
  }    

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mParser information\033[0m \n\r"))); */

  /* parser */
  if ((cmd_result = cmd_ppd_api_diag_pp_last_parser(unit, prm_core, &packet_action[diag_irpp_stage_parser])) != CMD_OK) {
      return cmd_result;
  }    

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mVLAN translation information\033[0m \n\r"))); */

  /* VTT */
  if ((cmd_result = cmd_ppd_api_diag_pp_last_vtt(unit, prm_core)) != CMD_OK) {
      return cmd_result;
  }     

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mTunnel termination information\033[0m \n\r"))); */

  /* TT */

  /* Is_My_MAC */

  /* Tunnel Termination information */
  if ((cmd_result = cmd_ppd_api_diag_pp_last_tt(unit, prm_core, &packet_action[diag_irpp_stage_tt])) != CMD_OK) {
      return cmd_result;
  }     

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mForward lookup processing information\033[0m \n\r"))); */

  /* FLP */
  if ((cmd_result = cmd_ppd_api_diag_pp_last_flp(unit, prm_core, &packet_action[diag_irpp_stage_flp])) != CMD_OK) {
      return cmd_result;
  }     

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mPMF information\033[0m \n\r"))); */

  /* PMF */
  if ((cmd_result = cmd_ppd_api_diag_pp_last_pmf(unit, prm_core, &packet_action[diag_irpp_stage_pmf])) != CMD_OK) {
      return cmd_result;
  }     

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mFEC resolution information\033[0m \n\r"))); */

  /* FEC */ 
  if ((cmd_result = cmd_ppd_api_diag_pp_last_fec(unit, prm_core, &packet_action[diag_irpp_stage_fec])) != CMD_OK) {
      return cmd_result;
  }     

  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  /* LOG_CLI((BSL_META_U(unit, "\033[4mTM resolution information\033[0m \n\r"))); */

  /* TM resolution */  
  if ((cmd_result = cmd_ppd_api_diag_pp_last_tm(unit, prm_core, &packet_action[diag_irpp_stage_tm])) != CMD_OK) {
      return cmd_result;
  }     

  /* Summary */
  LOG_CLI((BSL_META_U(unit, "------------------------------------------------\n\r")));

  {   
      DECLARE_PACKET_ACTION_DESC();
      DECLARE_IRPP_STAGE_DESC();

      cmd_diag_pp_irpp_stage_e stage;

      for (stage=diag_irpp_stage_port_termination; stage<diag_irpp_stage_tm; ++stage) {
        int field;
        
        for (field=0; field<CMD_DIAG_PP_PACKET_ACTION_NUM_FIELDS; ++field) {
            if ((packet_action[stage].flags & (0x1L << field)) && (packet_action[diag_irpp_stage_tm].flags & (0x1L << field))) {
                if (((uint32 *)&packet_action[stage])[field] == ((uint32 *)&packet_action[diag_irpp_stage_tm])[field]) {
                    LOG_CLI((BSL_META_U(unit, "%-16s  was first assigned at \033[4m%s\033[0m stage\n\r"), PACKET_ACTION_DESC(field), IRPP_STAGE_DESC(stage)));
                    packet_action[diag_irpp_stage_tm].flags &= ~((0x1L) << field);
                }
            }
        }
      }
  }

  return CMD_OK;
}

/********************************************************************* 
 *  Function handler:  tpid_globals_get (section diag)
 ********************************************************************/

cmd_result_t
    cmd_ppd_api_diag_tpid_globals_get(int unit, args_t* a) 
{   
  uint32
      ret; 

  SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
  ret = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    return CMD_FAIL;
  } 

  SOC_PPD_LLP_PARSE_TPID_VALUES_print(&tpid_vals);
 
  return CMD_OK;

} 

/********************************************************************* 
 *  Function handler:  tpid_profiles_get (section diag)
 ********************************************************************/
cmd_result_t
    cmd_ppd_api_diag_tpid_profiles_get(int unit, args_t* a)
{   
  uint32 
      ret; 
  uint32
      profile;

  for (profile=0; profile<_BCM_PORT_NOF_TPID_PROFILES; profile++) {

      SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO tpid_profile_info;

      ret = soc_ppd_llp_parse_tpid_profile_info_get(unit, profile, &tpid_profile_info);

      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
        return CMD_FAIL;
      } 

      LOG_CLI((BSL_META_U(unit, "\nprofile %u:\n"), profile));
      SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_print(&tpid_profile_info);
      LOG_CLI((BSL_META_U(unit, "\n")));
  }
 
  return CMD_OK;
} 
  
/********************************************************************* 
 *  Function handler:  tpid_port_profile_get (section diag)
 ********************************************************************/
  
cmd_result_t 
    cmd_ppd_api_diag_tpid_port_profile_get(int unit, args_t* a)
{
    SOC_PPD_PORT soc_ppd_port_i;
    SOC_PPD_PORT_INFO port_info;

    int              port_id = -1;
    int              prm_core = 0;
    parse_table_t    pt; 

    uint32 
      ret;

    SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO tpid_profile_info;
    SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;

    /* Get parameters */ 

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "port_id", PQ_DFL|PQ_IP, &port_id, &port_id, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);


    if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
    }

    if (port_id <= -1) 
    {
        /* If port not specified, return profiles of all ports*/
        int port;
        for (port = 0; port< SOC_DPP_DEFS_GET(unit,nof_pp_ports); port++) {
            soc_ppd_port_i = port;
            soc_ppd_port_info_get(unit,prm_core,soc_ppd_port_i, &port_info);
            LOG_CLI((BSL_META_U(unit, "port: %u : "), port));
            LOG_CLI((BSL_META_U(unit, "tpid_profile: %u\n"), port_info.tpid_profile));
        }
    }
    else
    {
        LOG_CLI((BSL_META_U(unit, "port: %u\n"), port_id));

        /* Profile index */
        soc_ppd_port_i = port_id;
        soc_ppd_port_info_get(unit,prm_core,soc_ppd_port_i, &port_info);
        LOG_CLI((BSL_META_U(unit, "tpid_profile: %u\n\n"), port_info.tpid_profile));

        /* Profile */
        ret = soc_ppd_llp_parse_tpid_profile_info_get(unit, port_info.tpid_profile, &tpid_profile_info);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL;
        }
        SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_print(&tpid_profile_info);

        /* Tpid vals of profile */
        ret = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL;
        }
        LOG_CLI((BSL_META_U(unit, "\n\ntpid index %u: %u\n"), tpid_profile_info.tpid1.index, tpid_vals.tpid_vals[tpid_profile_info.tpid1.index]));
        LOG_CLI((BSL_META_U(unit, "tpid index %u: %u\n"), tpid_profile_info.tpid2.index, tpid_vals.tpid_vals[tpid_profile_info.tpid2.index]));
    }
    return CMD_OK;
}

#ifdef  BCM_ARAD_SUPPORT
int
cmd_ppd_api_diag_vtt_dump_get(int unit, args_t* a) 
{ 
    uint32         soc_sand_dev_id; 
    parse_table_t  pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;
    int prog_id_vt, prog_id_tt, num_of_progs_vt, num_of_progs_tt;
    int             prm_core=0;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
    }

    soc_sand_dev_id = (unit); 

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_isem_access_print_last_vtt_program_data(soc_sand_dev_id, prm_core, 1, &prog_id_vt, &prog_id_tt, &num_of_progs_vt, &num_of_progs_tt);
        /* check if previous function return an error value */
        if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) { 
            return CMD_FAIL; 
        }
    } else {
        arad_pp_isem_access_print_all_programs_data(soc_sand_dev_id);
    }

    return CMD_OK;
}

int
cmd_ppd_api_diag_flp_dump_get(int unit, args_t* a) 
{ 
    uint32
        soc_sand_dev_id; 
    parse_table_t  pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;
    int prog_id, num_of_progs;
    int            prm_core=0;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
    }

    soc_sand_dev_id = (unit); 

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_flp_access_print_last_programs_data(soc_sand_dev_id, prm_core, 1, &prog_id, &num_of_progs);
        /* check if previous function return an error value */
        if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) { 
            return CMD_FAIL; 
        }
    } else {
        arad_pp_flp_access_print_all_programs_data(soc_sand_dev_id);
    }
  
  return CMD_OK;
}

/******************************************************************** 
 *  Function handler: egress_vlan_edit_info_get (section diag)
 ********************************************************************/
int 
cmd_ppd_api_diag_egress_vlan_edit_info_get(int unit, args_t* a) 
{
  uint32
    soc_sand_dev_id; 
  uint32 
    ret; 
  SOC_PPD_DIAG_EGRESS_VLAN_EDIT_RES
    prm_vec_res;
  int              prm_core = 0;
  parse_table_t    pt;


  soc_sand_dev_id = (unit); 
  SOC_PPD_DIAG_EGRESS_VLAN_EDIT_INFO_clear(&prm_vec_res);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }


  /* Call function */
  ret = soc_ppd_diag_egress_vlan_edit_info_get(soc_sand_dev_id, prm_core, &prm_vec_res);

  /* check if previous function return an error value */
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPD_DIAG_EGRESS_VLAN_EDIT_INFO_print(&prm_vec_res);

  return CMD_OK;

}

/******************************************************************** 
 *  Function handler: cmd_diag_gport_info_get (section diag)
 ********************************************************************/
int 
cmd_diag_gport_info_get(int unit, args_t* a) 
{
  uint32 
    ret; 
  parse_table_t    
    pt;
  DIAG_ALLOC_GPORT_PARAMS 
    gport_params;
  DIAG_ALLOC_GPORT_INFO
    gport_info;


  /* init gport params and infos */
  sal_memset(&gport_params, 0x0, sizeof(DIAG_ALLOC_GPORT_PARAMS));
  sal_memset(&gport_info, 0x0, sizeof(DIAG_ALLOC_GPORT_INFO));

  /* by default options for display information are enabled */
  gport_params.forwarding_database = 1;
  gport_params.lif_database = 1;
  gport_params.resource = 1;


  parse_table_init(unit, &pt);
  parse_table_add(&pt, "id", PQ_DFL|PQ_INT, 0, &gport_params.gport_id, NULL);
  parse_table_add(&pt, "RES", PQ_DFL|PQ_INT, 0, &gport_params.resource, NULL);
  parse_table_add(&pt, "FW_DB", PQ_DFL|PQ_INT, 0, &gport_params.forwarding_database, NULL);
  parse_table_add(&pt, "LIF_DB", PQ_DFL|PQ_INT, 0, &gport_params.lif_database, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  if (gport_params.gport_id <=0) {
      return CMD_USAGE;
  }

  /* Call function */
  gport_info.forwarding_info_data = sal_alloc(sizeof(_BCM_GPORT_PHY_PORT_INFO), "tmp gport");
  ret = diag_alloc_gport_info_get(unit, &gport_params, &gport_info);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  diag_alloc_gport_info_print(unit, &gport_params, &gport_info);

  sal_free(gport_info.forwarding_info_data);

  return CMD_OK;
}




/******************************************************************** 
 *  Function handler: cmd_bcm_cross_connect_get (section diag)
 ********************************************************************/

/* helper function for cmd_bcm_cross_connect_print_cb, print gport and encap value */
void cmd_bcm_cross_connect_gport_encap_print_cb(int unit, bcm_gport_t gport,  int encap) {
    cmd_result_t result;
    bcm_error_t rv;
    SOC_PPD_FRWRD_DECISION_INFO fwd_decision;
    DIAG_ALLOC_GPORT_PARAMS     gport_params;
    DIAG_ALLOC_GPORT_INFO       gport_info;

    if (diag_alloc_gport_type_is_supported(gport)) {
        /* print encoding and id like in diag pp GP */
        /* init gport params and infos */
        diag_alloc_gport_param_init(&gport_params);
        diag_alloc_gport_info_init(&gport_info);

        gport_params.gport_id = gport;

        /* Call function */
        result = diag_alloc_gport_info_get(unit, &gport_params, &gport_info);

        if (soc_sand_get_error_code_from_error_word(result) != SOC_SAND_OK) { 
            return; 
        } 
         cli_out("   ");
        diag_alloc_gport_info_print(unit, &gport_params, &gport_info);
    } else {
        if (encap != 0 || BCM_GPORT_IS_FORWARD_PORT(gport)) {
            cli_out("   phy gport: 0x%-10x\n", gport);
            cli_out("   encap:     0x%-10x\n", encap);
        } else {
            SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);
            /* get frwrd decision */
            rv = _bcm_dpp_port_encap_to_fwd_decision(unit, gport, encap, &fwd_decision);
            if ( rv != BCM_E_NONE) {
                cli_out("error in _bcm_dpp_port_encap_to_fwd_decision\n");
                return;
            }
            cli_out("   phy gport: <%s,%d>\n", SOC_PPC_FRWRD_DECISION_TYPE_to_string(fwd_decision.type), fwd_decision.dest_id);
        }
    }

}

/* print gport and encap for both gports in cross connect */
int cmd_bcm_cross_connect_print_cb(int unit, 
    bcm_vswitch_cross_connect_t *gports, 
    void *user_data) {
    
    cli_out("gport1\n");
    cmd_bcm_cross_connect_gport_encap_print_cb(unit, gports->port1, gports->encap1);
    cli_out("gport2\n");
    cmd_bcm_cross_connect_gport_encap_print_cb(unit, gports->port2, gports->encap2);
    cli_out("\n");

    return BCM_E_NONE;
}


int 
cmd_bcm_cross_connect_get(int unit, args_t* a) 
{
  uint32 
    ret;
  int
    user_data; 

  /* call function */
  ret = bcm_vswitch_cross_connect_traverse(unit, cmd_bcm_cross_connect_print_cb, &user_data);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
       return CMD_FAIL;
  }
  return CMD_OK;
}


/******************************************************************** 
 *  Function handler: cmd_diag_occupation_profile_info_get (section diag)
 ********************************************************************/


#ifdef BCM_ARAD_SUPPORT

cmd_result_t
cmd_diag_egress_drop(int unit, args_t* a){
    uint32 
        ret;   
    SOC_PPD_DIAG_EG_DROP_LOG_INFO   
        prm_eg_drop_log;
   
    SOC_PPD_DIAG_EG_DROP_LOG_INFO_clear(&prm_eg_drop_log);

    /* Call function */
    ret = soc_ppd_diag_eg_drop_log_get(unit, &prm_eg_drop_log);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) { 
        cli_out(" *** soc_ppd_diag_eg_drop_log_get - FAIL"); 
        return CMD_FAIL;
    } 

    SOC_PPD_DIAG_EG_DROP_LOG_INFO_print(&prm_eg_drop_log);

    return CMD_OK;
}

cmd_result_t 
cmd_diag_fec_allocation(int unit, args_t* a) {



    if (fec_allocation_main(unit) != BCM_E_NONE)
    {
           return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_result_t 
cmd_diag_occ_mgmt(int unit, args_t* a) {

    if (SOC_PPC_OCC_ALL_PROFILES_print(unit) != SOC_SAND_OK)
    {
           return CMD_FAIL;
    }

    return CMD_OK;
}


#endif



cmd_result_t 
cmd_diag_occupation_profile_info_get(int unit, args_t* a) {
    uint32 
        ret;

    ret = SOC_PPC_OCC_ALL_PROFILES_print(unit);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    }

    return CMD_OK;
}

cmd_result_t 
cmd_ppd_api_diag_cos_get(int unit, args_t* a) {
    uint32 
        ret;
    int              prm_core = 0;
    parse_table_t    pt;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    ret = arad_pp_diag_cos_info_get_unsafe(unit, prm_core);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    } 

    return CMD_OK;
}

int 
  cmd_ppd_api_diag_db_glem_lkup_info_get(int unit, args_t* a) 
{   
  uint32            rv;   
  int               global_lif = 0;
  int               gport = 0;
  int               prm_core = 0;
  int               i;
  parse_table_t     pt;
  uint8             nof_printed_lifs;
  _bcm_dpp_gport_hw_resources hw_resources;
  soc_ppc_diag_glem_signals_t signals_info;
  soc_ppc_diag_glem_outlif_t *outlif_info;

  /* Get parameters */
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "gport", PQ_DFL|PQ_INT, 0, &gport, NULL);
  parse_table_add(&pt, "gl", PQ_DFL|PQ_INT, 0, &global_lif, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
  }

  /* Input validation */
  cli_out("\n\n");
  if (gport != 0 && global_lif != 0) {
      cli_out("Error: Can't set both gl and gport\n");
      return CMD_FAIL;
  }

  if(prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores) || prm_core < 0){
       cli_out("Error: Illegal core ID for device: %d\n", prm_core);
       return CMD_FAIL;
  }

  sal_memset(&signals_info, 0, sizeof(signals_info));

  /* Call functions */

  if (gport) {
      /* Get the global lif from the gport, and then perform the regular global lif lookup. */
      rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, &hw_resources);
      if (rv == BCM_E_NOT_FOUND || rv == BCM_E_PORT) {
          cli_out("Error: Gport 0x%x doesn't exist.\n", gport);
          return CMD_FAIL;
      } else if (rv) {
          cli_out("Error in sw state access\n");
          return CMD_FAIL;
      }

      /* Make sure we got a legal global outlif. */
      if (hw_resources.global_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
          cli_out("Error: No egress global lif found for gport 0x%x.\n", gport);
          return CMD_FAIL;
      }
      
      /* Global outlif found. */
      global_lif = hw_resources.global_out_lif;
      cli_out("For gport 0x%x:\n", global_lif);
  }

  if (global_lif) {
      outlif_info = &signals_info.outlifs[0];
      /* First, check if the lif is mapped or unmapped*/
      /* Perform a GLEM lookup. */
      rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_by_key_get, (unit, global_lif, &outlif_info->local_outlif, &outlif_info->accessed, &outlif_info->found));
      if (rv != BCM_E_NONE) {
          return CMD_FAIL;
      }
      outlif_info->global_outlif = global_lif;
      outlif_info->source = soc_ppc_diag_glem_outlif_source_user;

  } else {
      /* Read signals. */
      rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_diag_glem_signals_get, (unit, prm_core, &signals_info));
      if (rv != BCM_E_NONE) {
          return CMD_FAIL;
      }

  }

  nof_printed_lifs = 0;
  for (i = 0 ; i < 2 ; i++) {
      outlif_info = &signals_info.outlifs[i];
      if (outlif_info->global_outlif) {
          /* Only print No point in displaying empty searches. */
          nof_printed_lifs++;
          
          if (!outlif_info->found) {
              /* 
               *  Even if the GLEM lookup failed, it's possible that the lif is still valid because it's a direct lif.
               *  Check if this is the case, and update accordingly.
               */
              rv = _bcm_dpp_lif_id_is_mapped(unit, FALSE, outlif_info->global_outlif, &outlif_info->mapped);
              if (rv != BCM_E_NONE) {
                  return CMD_FAIL;
              }

              if (!outlif_info->mapped) {
                  /* Direct lif. */
                  outlif_info->found = TRUE;
                  outlif_info->local_outlif = outlif_info->global_outlif;
                  outlif_info->mapped = FALSE;
              }
          } else {
              /* If lif was found in the GLEM, it means it's a mapped lif. */
              outlif_info->mapped = TRUE;
          }
          soc_ppc_diag_glem_outlif_print(outlif_info);
      }
  }

  if (!nof_printed_lifs) {
      cli_out("No GLEM lookups were performed.\n\n\n");
  }

  return CMD_OK; 
} 

#endif /* BCM_ARAD_SUPPORT */


#ifdef BCM_JERICHO_SUPPORT


cmd_result_t
cmd_diag_kaps_lkup_info_get(int unit, args_t* a) {
    uint32 soc_rc;
    int              prm_core=0;
    parse_table_t    pt;

 
    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
    if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
    }

    soc_rc = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_diag_kaps_lkup_info_get, (unit, prm_core)));
    if(SOC_SAND_FAILURE(soc_rc)) {
        return CMD_FAIL;
    }
    return CMD_OK;
}


#endif /* BCM_JERICHO_SUPPORT */

cmd_result_t
cmd_dpp_diag_pp(int unit, args_t* a)
{
    int i;
    char      *function;

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    } 

    for (i=0; i<sizeof(diag_pp_command_def)/sizeof(DIAG_PP_COMMAND_DEF); i++ ) {
        if (DIAG_FUNC_STR_MATCH(function, diag_pp_command_def[i].short_name, diag_pp_command_def[i].long_name)) {
            return (diag_pp_command_def[i].cmd_diag_pp_function(unit,a));
        }
    } 

    return CMD_USAGE;
}

void
print_pp_usage(int unit)
{
    char cmd_dpp_diag_pp_usage[] =
    "Usage (DIAG pp):"
    "\n\tDIAGnotsics pp commands\n\t"
    "Usages:\n\t"
    "DIAG pp [OPTION] <parameters> ..."
#ifdef __PEDANTIC__
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
    "OPTION can be:"
    "\nFrwrd_Decision_Trace - returns the forwarding decision (destination + trap) for each phase in processing port,LIF, forwarding lookup, FEC resultion, TRAP"
    "\n\t\tcore - Core ID (0/1)"
    "\nReceived_Packet_Info - dump first 128 bytes from last received packet and return its association port information"
    "\n\t\tcore - Core ID (0/1)."
    "\nParsing_Info - returns packet format e.g. ipv4oEth"
    "\n\t\tcore - Core ID (0/1)"
    "\nDB_LIF_lkup_info - return the lookup keys and result from SEM DB"
    "\n\t\tbank_id - Bank number. For SEM: 0 (SEM-A), 1 (SEM-B), 2 (TCAM)."
    "\n\t\tlkup_num - Lookup number, some DB has more than one lookup"
    "\n\t\tcore - Core ID (0/1)"
    "\nTERMination_Info - retuns terminated headers and the forwarding header types" 
    "\n\t\tcore - Core ID (0/1)"
    "\nTRAPS_Info - returns wether packet was trapped, and trap-code" 
    "\n\t\tcore - Core ID (0/1)"
    "\nTrapped_Packet_Info - parse packet recived by CPU return packet size, start of network header and other proccessing attributes"  
    "\n\t\tmax_size - Maximum number of bytes to read"
    "\nENCAP_info - returns the encapsulation info in egress DB"
    "\n\t\tcore - Core ID (0/1)"
    "\nFrwrd_Lkup_Info - returns the forwarding enginetype  (IP routing, bridging, TRILL,..)"       
    "\n\t\tcore - Core ID (0/1)"
    "\nFrwrd_LPM_lkup - diag query into LPM DB. Given VRF,DIP return FECs"
    "\n\t\tip_address - Ipv4 address"
    "\n\t\tprefix_len - Number of bits to consider in the IP address starting from the msb. Range: 0-32."
    "\n\t\t\tExample for key ip_address 192.168.1.0 and prefix_len 24 would match any IP Address of the form 192.168.1.x"
    "\n\t\tvrf - VRF"
    "\n\t\tcore - Core ID (0/1)"
    "\nIng_Vlan_Edit_info - return ingress/egress vlan editing information"
    "\n\t\tcore - Core ID (0/1)"
    "\nLeaRNing_info - returns learning info of last packet"
    "\nMODE_info_Get - Gets the configuration set by the mode_info_set API."
    "\nMODE_info_Set - when setting mode to raw: rest of diag CLIs will return information (keys and payload) in hex without parsing"
    "\n\t\tflavor - Flavor indicates/affects the way the diagnostic APIs work:"
    "\n\t\t\tRAW to return a raw bit-stream i.e. without parsing into structure"
    "\n\t\t\tNONE for no flavor"
    "\nPKT_associated_TM_info - returns  the resolved destination, meter-id, and other info to be processed/used by TM blocks"
    "\n\t\tcore - Core ID (0/1)"
	"\nMeTeRing- returns the current bucket status for a given meter"
	"\n\t\tid - Meter ID"
    "\n\t\tcore - Meter core id (0/1), optional"
	"\n\t\tgroup - Meter group id (0/1), optional"
    "\nPKT_TRace_Clear - clear sticky status in HW"
    "\n\t\tcore - Core ID (0/1)"
    "\nDB_LEM_lkup_info - return the lookup keys and result from LEM DB. Used for forwarding lookup. E.g. MACT, ILM, host lookup"
    "\n\t\tlkup_num - Lookup number, some DB has more than one lookup"
    "\nEgress_DROP - display filters at egress"
    "\nLIF_show - display bank ID and offset inside bank"
    "\n\ttype - lif type:in/out"
    "\n\tid   - lif id (interpreted as local id by default or as global when gl=1"
    "\n\tgl   - set to 1 in order to have lif id interpreted as global one"
    "\ntpidg - display globals tpid"
    "\ntpidpr - display tpid profiles" 
    "\ntpidpo port - display profile associate with port"
    "\n\tport - port number, optional"
    "\n\tcore - core id (0/1), optional"
#ifdef  BCM_ARAD_SUPPORT
    "\nVTT_Dump [last=0]  - return the valid VTT programs"
    "\nVTT_Dump [last=1]  - return last invoked VTT program"
    "\nFLP_Dump [last=0]  - return the valid FLP programs"
    "\nFLP_Dump [last=1]  - return last invoked FLP program"
    "\nEgr_Vlan_Edit_info - return egress vlan editing information"
    "\nGPort              - return information for the gport"
    "\n\tid     - gport id"
    "\n\tRES    - Display resource"
    "\n\tFW_DB  - Display forward database info"
    "\n\tLIF_DB - Display lif database info"
    "\nCross-Connect-traverse - return the cross connection traverse information"
    "\nIPv4_MC            - displays mc routing table"
    "\nVSI_decision       - VSI information"
    "\nIf_PROFILE_Status  - Interface profile allocation"
    "\nRIF_profile_info   - return information about last packet profile"
    "\nOCCupation_mgmt_show - display occupation maps status"
    "\nClass_Of_Service   - display cos mapping"
    "\nDB_GLEM_lkup_info  - returns the last lookup key(s) and result(s) in the Global Lif Exact Match"
    "\n\t\tgport=[value]  -    perform a GLEM lookup for gport [value]"
    "\n\t\tgl=[value]     -    perform a GLEM lookup for [value]"
    "\n\t\tcore           -    core id (optional, default 0)"
#endif
#ifdef BCM_JERICHO_SUPPORT
    "\nKaps_LKuP_info_get - return the lookup keys and results from kaps"
#endif /* BCM_JERICHO_SUPPORT */
#ifdef DATAIO_SUPPORT
    "\ndiag pp sig [name=Signal] [from=Stage] [to=Stage] [show=stage/struct/detail/expand/full] [order=little/big]"
    "\n\tshow options"
    "\n\t\tstage  - presents all PP blocks and stages that may be used in to and from options"
    "\n\t\tstruct - presents available expansions for signals. May be used with name option to limit the output for desired expansions only"
    "\n\t\tdetail - provides HW(ASIC) name for specific signal and address ranges"
    "\n\t\texpand - provides signal itself and its expansion"
    "\n\t\tfull   - provides full value fo signal (without it is limited to 256 first bits"
    "\n\tname, from, to"
    "\n\t\t       - filtering fields for signal representation"
    "\n\t\tSignal - name of specific signal from specifications, may be multiple signals from different stages"
    "\n\t\tStage  - use in from or to, filters signals that were fetched in specific place"
    "\n\torder"
    "\n\t\tbig    - presents MSB first, useful for variable representation [default value - no need to use explicitly]"
    "\n\t\tlittle - presents LSB first, useful for packet/memory signals"
#endif /* DATAIO_SUPPORT */
    "\n";
#endif   /*COMPILER_STRING_CONST_LIMIT*/

    cli_out(cmd_dpp_diag_pp_usage);
}
#endif /* BCM_DPP_SUPPORT */

