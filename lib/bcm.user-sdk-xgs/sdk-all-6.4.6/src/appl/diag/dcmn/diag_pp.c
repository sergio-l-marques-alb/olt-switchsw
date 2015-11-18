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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#endif /* BCM_DPP_SUPPORT */

#include <soc/hwstate/hw_log.h>

#include <soc/dpp/mbcm_pp.h>

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
cmd_result_t cmd_diag_gport_info_get(int unit, args_t* a);
cmd_result_t cmd_bcm_cross_connect_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_ipv4_mc_route_get(int unit, args_t* a);
cmd_result_t cmd_diag_occupation_profile_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_egress_drop(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_lif_show(int unit, args_t* a);
#ifdef BCM_ARAD_SUPPORT
cmd_result_t cmd_diag_fec_allocation(int unit, args_t* a);
cmd_result_t cmd_diag_occ_mgmt(int unit, args_t* a);
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
    { "DB_LEM_lkup_info"      , "DBLEM"  , cmd_ppd_api_diag_db_lem_lkup_info_get  },
    { "Egress_DROP"           , "EDROP"  , cmd_diag_egress_drop                       },
    { "LIF_show"              , "LIF"    , cmd_ppd_api_diag_lif_show                  }
#ifdef  BCM_ARAD_SUPPORT
                                                                                      ,
    { "VTT_DUMP"              , "VTT"    , cmd_ppd_api_diag_vtt_dump_get              },
    { "FLP_DUMP"              , "FLP"    , cmd_ppd_api_diag_flp_dump_get              },
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
    { "OCCupation_mgmt_show"  , "OCC",              cmd_diag_occ_mgmt                 }
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


    SOC_PPC_DIAG_DB_USE_INFO_print(prm_db_info);  
    SOC_PPD_DIAG_LIF_LKUP_INFO_print(&prm_lkup_info);

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
  uint32              default_val = 3;
  uint32              bank_id=3, bank_id_tmp;
  uint32              lkup_num=3, lkup_num_tmp;
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
  
  

  if (lkup_num == default_val) {
      for (lkup_num_tmp = 0; lkup_num_tmp < 2; lkup_num_tmp++) {
          prm_db_info.lkup_num = lkup_num_tmp;           
          if (bank_id == default_val) {
              for (bank_id_tmp = 0; bank_id_tmp < 3; bank_id_tmp++) {
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
          for (bank_id_tmp = 0; bank_id_tmp < 3; bank_id_tmp++) {
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

  SOC_PPD_DIAG_ENCAP_INFO_print(&prm_encap_info);

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
/* For Jericho only - SW mapping below should work for all
            uint8 is_found;
            ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_by_key_get, (unit, lif_id, &llif_id, NULL, &is_found));
            if(is_found)
                cli_out("Global Out LIF:0x%x mapped to Local LIF:0x%x\n", lif_id, llif_id);
            else {
                cli_out("Global Out LIF:0x%x does not exist in HW\n", lif_id);
                return ret;
            }
*/
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

#ifdef  BCM_ARAD_SUPPORT
int
cmd_ppd_api_diag_vtt_dump_get(int unit, args_t* a) 
{ 
    uint32         soc_sand_dev_id; 
    parse_table_t  pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;
    int prog_id_vt, prog_id_tt;
    int             prm_core=0;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    soc_sand_dev_id = (unit); 

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_isem_access_print_last_vtt_program_data(soc_sand_dev_id, prm_core, 1, &prog_id_vt, &prog_id_tt);
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
    int prog_id;
    int            prm_core=0;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    soc_sand_dev_id = (unit); 

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_flp_access_print_last_programs_data(soc_sand_dev_id, prm_core, 1, &prog_id);
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
  ret = diag_alloc_gport_info_get(unit, &gport_params, &gport_info);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  diag_alloc_gport_info_print(unit, &gport_params, &gport_info);

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
    "\nReceived_Packet_Info - dump first 128 bytes from last received packet and return its association port information"
    "\nParsing_Info - returns packet format e.g. ipv4oEth"
    "\nDB_LIF_lkup_info - return the lookup keys and result from SEM DB"
    "\n\t\tbank_id - Bank number. For SEM: 0 (SEM-A), 1 (SEM-B), 2 (TCAM)."
    "\n\t\tlkup_num - Lookup number, some DB has more than one lookup"
    "\nTERMination_Info - retuns terminated headers and the forwarding header types" 
    "\nTRAPS_Info - returns wether packet was trapped, and trap-code" 
    "\nTrapped_Packet_Info - parse packet recived by CPU return packet size, start of network header and other proccessing attributes"  
    "\n\t\tmax_size - Maximum number of bytes to read"
    "\nENCAP_info - returns the encapsulation info in egress DB"
    "\nFrwrd_Lkup_Info - returns the forwarding enginetype  (IP routing, bridging, TRILL,..)"       
    "\nFrwrd_LPM_lkup - diag query into LPM DB. Given VRF,DIP return FECs"
    "\n\t\tip_address - Ipv4 address"
    "\n\t\tprefix_len - Number of bits to consider in the IP address starting from the msb. Range: 0-32."
    "\n\t\t\tExample for key ip_address 192.168.1.0 and prefix_len 24 would match any IP Address of the form 192.168.1.x"
    "\n\t\tvrf - VRF"
    "\nIng_Vlan_Edit_info - return ingress/egress vlan editing information"
    "\nLeaRNing_info - returns learning info of last packet"
    "\nMODE_info_Get - Gets the configuration set by the mode_info_set API."
    "\nMODE_info_Set - when setting mode to raw: rest of diag CLIs will return information (keys and payload) in hex without parsing"
    "\n\t\tflavor - Flavor indicates/affects the way the diagnostic APIs work:"
    "\n\t\t\tRAW to return a raw bit-stream i.e. without parsing into structure"
    "\n\t\t\tNONE for no flavor"
    "\nPKT_associated_TM_info - returns  the resolved destination, meter-id, and other info to be processed/used by TM blocks"
    "\nPKT_TRace_Clear - clear sticky status in HW"
    "\nDB_LEM_lkup_info - return the lookup keys and result from LEM DB. Used for forwarding lookup. E.g. MACT, ILM, host lookup"
    "\n\t\tlkup_num - Lookup number, some DB has more than one lookup"
    "\nEgress_DROP - display filters at egress"
    "\nLIF_show - display bank ID and offset inside bank"
    "\n\ttype - lif type:in/out"
    "\n\tid   - lif id"
    "\n\tgl   - set to 1 in order to have global lif display, default is local lif"
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
#endif
#ifdef BCM_JERICHO_SUPPORT
    "\nKaps_LKuP_info_get - return the lookup keys and results from kaps"
#endif /* BCM_JERICHO_SUPPORT */
    "\n";
#endif   /*COMPILER_STRING_CONST_LIMIT*/

    cli_out(cmd_dpp_diag_pp_usage);
}
#endif /* BCM_DPP_SUPPORT */
