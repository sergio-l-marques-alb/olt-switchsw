/* $Id: pb_pp_port.c,v 1.14 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
*/
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_port.c
*
* MODULE PREFIX:  soc_pb_pp
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_port.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>

#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_MAX                         (SOC_DPP_NOF_TOPOLOGY_IDS_PETRAB - 1)
#define SOC_PB_PP_PORT_STP_STATE_MAX                               (SOC_PB_PP_NOF_PORT_STP_STATES-1)
#define SOC_PB_PP_PORT_DIRECTION_NDX_MAX                           (SOC_PB_PP_PORT_NOF_DIRECTIONS-1)
#define SOC_PB_PP_PORT_LEARN_DEST_TYPE_MAX                         (SOC_SAND_PP_NOF_DEST_TYPES-1)
#define SOC_PB_PP_PORT_PORT_PROFILE_MAX                            (7)
#define SOC_PB_PP_PORT_TPID_PROFILE_MAX                            (3)
#define SOC_PB_PP_PORT_VLAN_DOMAIN_MAX                             (SOC_DPP_NOF_VLAN_DOMAINS_PETRAB - 1)
#define SOC_PB_PP_PORT_PORT_TYPE_MAX                               (SOC_SAND_PP_NOF_PORT_L2_TYPES-1)
#define SOC_PB_PP_PORT_ORIENTATION_MAX                             (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define SOC_PB_PP_PORT_DA_NOT_FOUND_PROFILE_MAX                    (0)
#define SOC_PB_PP_PORT_ETHER_TYPE_BASED_PROFILE_MAX                (7)
#define SOC_PB_PP_PORT_MTU_MAX                                     (SOC_SAND_U32_MAX)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef SOC_PPC_PORT_STP_STATE_FLD_VAL                         SOC_PB_PP_PORT_STP_STATE_FLD_VAL;
#define SOC_PB_PP_PORT_STP_STATE_FLD_VAL_BLOCK               SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK
#define SOC_PB_PP_PORT_STP_STATE_FLD_VAL_LEARN               SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN
#define SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD             SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD

typedef enum
{
  SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_BLOCK = 1,
  SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_LEARN = 1,/*actually block*/
  SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_FORWARD = 0
} SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_port[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_STP_STATE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORT_GET_ERRS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORTS_REGS_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PORTS_INIT),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_port[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'topology_id_ndx' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_STP_STATE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_STP_STATE_OUT_OF_RANGE_ERR",
    "The parameter 'stp_state' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_PORT_STP_STATES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR",
    "The parameter 'sys_phy_port_id' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'direction_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_PORT_DIRECTIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'learn_dest_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_DEST_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_PORT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_PORT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'port_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_PORT_L2_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'ether_type_based_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_MTU_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_MTU_OUT_OF_RANGE_ERR",
    "The parameter 'mtu' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR",
    "The parameter 'da_not_found_profile' not valid for Soc_petra-B. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_port_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;
  SOC_PB_PP_IHP_STP_TABLE_TBL_DATA
    ihp_stp_table_tbl_data;
  SOC_PB_PP_EPNI_STP_TBL_DATA
    epni_stp_tbl_data;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* Forward everything */
  ihp_stp_table_tbl_data.per_port_stp_state[0] = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD;
  ihp_stp_table_tbl_data.per_port_stp_state[1] = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD;
  ihp_stp_table_tbl_data.per_port_stp_state[2] = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD;
  ihp_stp_table_tbl_data.per_port_stp_state[3] = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihp_stp_table_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_stp_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  epni_stp_tbl_data.egress_stp_state[0] = SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_FORWARD;
  epni_stp_tbl_data.egress_stp_state[1] = SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_FORWARD;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_epni_stp_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &epni_stp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &egq_pp_ppct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  egq_pp_ppct_tbl_data.disable_learning = 0;
  for (idx = 0; idx < SOC_PB_PP_NOF_PORTS; ++idx)
  {
    res = soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(
            unit,
            idx,
            &egq_pp_ppct_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_init_unsafe()", 0, 0);
}


STATIC void
  soc_pb_pp_port_orientation2intern(
    SOC_SAND_IN SOC_SAND_PP_HUB_SPOKE_ORIENTATION port_orientation,
    SOC_SAND_OUT uint32                     *egq_ppct_intern
  )
{
  uint32
    port_o_intern;
  
  switch(port_orientation) {
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:
    port_o_intern = 0x1;
  	break;
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE:
    port_o_intern = 0x0;
  	break;
  default:
    /* Undefined */
    port_o_intern = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  }

  *egq_ppct_intern =  port_o_intern;
}

STATIC void
  soc_pb_pp_port_intern2orientation(
    SOC_SAND_IN uint32                        egq_ppct_intern,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION  *port_orientation
  )
{
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION
    port_o_intern;
  
  switch(egq_ppct_intern) {
  case 0x1:
    port_o_intern = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
  	break;
  case 0x0:
    port_o_intern = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
  	break;
  default:
    /* Undefined */
    port_o_intern = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  }

  *port_orientation =  port_o_intern;
}

/*********************************************************************
*     Set main attributes of the port. Generally, these
 *     attributes identify the port and may have use in more
 *     than one module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA
    pp_port_tbl;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_conf_tbl;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA
    pinfo_flp_tbl;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_tbl;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  uint32
    port_tm_id,
    pp_port_in,
    port_pp_id;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;
  uint8
	  mac_in_mac_enabled,
	  vid_sa_base_enabled;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  /************************************************************************/
  /* Get current relevant information                                     */
  /************************************************************************/

  res = soc_pb_pp_ihp_pp_port_info_tbl_get_unsafe(unit, local_port_ndx, &pp_port_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, local_port_ndx, &vtt_in_pp_port_conf_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(unit, local_port_ndx, &pinfo_llr_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  res = soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(unit, local_port_ndx, &pinfo_flp_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(unit, local_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_egq_pct_tbl_get_unsafe(unit, local_port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, local_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  /************************************************************************/
  /* Updated the requested fields                                         */
  /************************************************************************/
  
  /*
   *	Port Profile.
   *  Defines the following:
   *  - Acceptable frame types (VLAN Tag Structure).
   *  - AC map key, (port, portxVlan, PortxVlanxVlan).
   */


  pinfo_llr_tbl.vlan_classification_profile = port_info->port_profile;

  /* 
   * Special configurations for mac-in-mac
   */
  res = soc_pb_pp_is_mac_in_mac_enabled(unit, &mac_in_mac_enabled);
  if (res != SOC_SAND_OK)
	  goto exit;
  if (mac_in_mac_enabled)
  {
	  res = soc_pb_pp_sw_db_llp_vid_assign_port_sa_get(
			  unit,
			  local_port_ndx,
			  &vid_sa_base_enabled
            );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

	  /* Enable b-sa lookup on port */
	  pinfo_llr_tbl.sa_lookup_enable = port_info->is_pbp || vid_sa_base_enabled || pinfo_llr_tbl.enable_sa_authentication;
	  pinfo_flp_tbl.program_translation_profile = port_info->is_pbp;
  }
  vtt_in_pp_port_conf_tbl.small_em_key_profile = port_info->port_profile;
  epni_pp_pct_tbl.llvp_profile = port_info->port_profile;

  /*
   *	TPID Profile
   */
  pp_port_tbl.tpid_profile = port_info->tpid_profile;
  
  /*
   *	VLAN Domain
   */
  vtt_in_pp_port_conf_tbl.vlan_domain = port_info->vlan_domain;
  epni_pp_pct_tbl.vlan_domain = port_info->vlan_domain;
  
  /*
   *	Enable PBP
   */
  epni_pp_pct_tbl.port_is_pbp = SOC_SAND_BOOL2NUM(port_info->is_pbp);
  vtt_in_pp_port_conf_tbl.port_is_pbp = SOC_SAND_BOOL2NUM(port_info->is_pbp);
  
  /*
   *	Enable AC Learning
   */
  vtt_in_pp_port_conf_tbl.learn_ac = SOC_SAND_BOOL2NUM(port_info->is_learn_ac);

  /*
   *	Initial Action Profile
   */
  pinfo_llr_tbl.default_action_profile_fwd = port_info->initial_action_profile.frwrd_action_strength;
  pinfo_llr_tbl.default_action_profile_snp = port_info->initial_action_profile.snoop_action_strength;
  /*
   *	Convert the SW trap code to the HW index
   */
  res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
          port_info->initial_action_profile.trap_code,
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  pinfo_llr_tbl.default_cpu_trap_code = trap_code_internal;

  /*
   *	Enable SA Learning
   */
  pinfo_flp_tbl.learn_enable = SOC_SAND_BOOL2NUM(port_info->enable_learning);
  egq_pp_ppct_tbl.disable_learning = SOC_SAND_BOOL2NUM_INVERSE(port_info->enable_learning);
  /*
   *	same-interfac filtering
   */
  /* ingress */
  pinfo_flp_tbl.enable_unicast_same_interface_filter = SOC_SAND_BOOL2NUM(port_info->enable_same_interfac_filter);
  /* ingress */
  egq_pp_ppct_tbl.enable_src_equal_dst_filter = SOC_SAND_BOOL2NUM(port_info->enable_same_interfac_filter);
  /*
   *  Ether-type-based profile:
   *	Port profile for l2-protocol-based VID and TC assignment.
   */
  pinfo_llr_tbl.protocol_profile = port_info->ether_type_based_profile;

  pinfo_llr_tbl.non_authorized_mode_8021x = SOC_SAND_BOOL2NUM(port_info->not_authorized_802_1x);
  
  /*
   *	Port orientation, HUB or Spoke.
   */
  soc_pb_pp_port_orientation2intern(port_info->orientation, &egq_pp_ppct_tbl.orientation_is_hub);
  
  /*
   *	Port Type
   */
  epni_pp_pct_tbl.cep_c_vlan_edit = (port_info->port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP)?1:0;
     
  /*
   *	Extended P2P (incoming/outgoing)
   */
  pinfo_flp_tbl.transparent_p2p_service_enable = SOC_SAND_BOOL2NUM(port_info->extend_p2p_info.enable_incoming_extend_p2p);

  /*
   *  In TDM mode: port profile is configured to be CPU (no change)
   */
  if (soc_petra_sw_db_tdm_mode_get(unit) != SOC_PETRA_MGMT_TDM_MODE_PACKET)
  {
    pct_tbl.port_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU;
  }
  

  /************************************************************************/
  /* Write the updated configuration to HW                                */
  /************************************************************************/
  
  res = soc_pb_pp_ihp_pp_port_info_tbl_set_unsafe(unit, local_port_ndx, &pp_port_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, local_port_ndx, &vtt_in_pp_port_conf_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(unit, local_port_ndx, &pinfo_llr_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe(unit, local_port_ndx, &pinfo_flp_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_egq_pct_tbl_set_unsafe(unit, local_port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_epni_pp_pct_tbl_set_unsafe(unit, local_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(unit, local_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
 

  /*
   *	MTU
   */
  for (port_tm_id = 0; port_tm_id < SOC_PETRA_NOF_FAP_PORTS; port_tm_id++)
  {
    res = soc_pb_port_to_pp_port_map_get_unsafe(
            unit,
            port_tm_id,
            &pp_port_in,
            &port_pp_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    if (port_pp_id == local_port_ndx)
    {
      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(unit, port_tm_id, &egq_ppct_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      /*
       *	MTU: Maximum Transmission Unit.
       *  Packets with size over this value will be processed
       *  according to action profile assigned to this event.
       */
      egq_ppct_tbl.mtu = port_info->mtu;

      res = soc_pb_pp_egq_ppct_tbl_set_unsafe(unit, port_tm_id, &egq_ppct_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_INFO_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Set main attributes of the port. Generally, these
 *     attributes identify the port and may have use in more
 *     than one module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_tbl;
  SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA
    pp_port_tbl;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_conf_tbl;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA
    pinfo_flp_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PB_PP_PORT_INFO_clear(port_info);

  /************************************************************************/
  /* Get current relevant information                                     */
  /************************************************************************/
  res = soc_pb_egq_pct_tbl_get_unsafe(unit, local_port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihp_pp_port_info_tbl_get_unsafe(unit, local_port_ndx, &pp_port_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, local_port_ndx, &vtt_in_pp_port_conf_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, local_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(unit, local_port_ndx, &pinfo_llr_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(unit, local_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(unit, local_port_ndx, &pinfo_flp_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_pb_pp_egq_ppct_tbl_get_unsafe(unit, local_port_ndx, &egq_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  /************************************************************************/
  /* Retrieve the requested fields                                         */
  /************************************************************************/
  
  /*
   *	Port Profile.
   *  Defines the following:
   *  - Acceptable frame types (VLAN Tag Structure).
   *  - AC map key, (port, portxVlan, PortxVlanxVlan).
   */
  port_info->port_profile = pinfo_llr_tbl.vlan_classification_profile;
  port_info->port_profile = vtt_in_pp_port_conf_tbl.small_em_key_profile;

  /*
   *	TPID Profile
   */
  port_info->tpid_profile = pp_port_tbl.tpid_profile;
  
  /*
   *	VLAN Domain
   */
  port_info->vlan_domain = vtt_in_pp_port_conf_tbl.vlan_domain;
  port_info->vlan_domain = epni_pp_pct_tbl.vlan_domain;
  
  /*
   *	Enable PBP
   */
  port_info->is_pbp = SOC_SAND_NUM2BOOL(epni_pp_pct_tbl.port_is_pbp);
  
  /*
   *	Enable AC Learning
   */
  port_info->is_learn_ac = SOC_SAND_NUM2BOOL(vtt_in_pp_port_conf_tbl.learn_ac);

  /*
   *	Initial Action Profile
   */
  port_info->initial_action_profile.frwrd_action_strength = pinfo_llr_tbl.default_action_profile_fwd;
  port_info->initial_action_profile.snoop_action_strength = pinfo_llr_tbl.default_action_profile_snp;
  res = soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(
          unit,
          pinfo_llr_tbl.default_cpu_trap_code,
          &(port_info->initial_action_profile.trap_code)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);

  /*
   *	Enable SA Learning
   */
  port_info->enable_learning = SOC_SAND_NUM2BOOL(pinfo_flp_tbl.learn_enable) || SOC_SAND_NUM2BOOL_INVERSE(egq_pp_ppct_tbl.disable_learning);

  /*
   *	same-interfac filtering
   */
  /* ingress */
  port_info->enable_same_interfac_filter = SOC_SAND_NUM2BOOL(pinfo_flp_tbl.enable_unicast_same_interface_filter);

  /*
   *  Ether-type-based profile:
   *	Port profile for l2-protocol-based VID and TC assignment.
   */
  port_info->ether_type_based_profile = pinfo_llr_tbl.protocol_profile;

  port_info->not_authorized_802_1x = SOC_SAND_NUM2BOOL(pinfo_llr_tbl.non_authorized_mode_8021x);
  
  /*
   *	Port orientation, HUB or Spoke.
   */
  soc_pb_pp_port_intern2orientation(egq_pp_ppct_tbl.orientation_is_hub, &port_info->orientation);
  
  /*
   *	Port Type
   */
  port_info->port_type = (epni_pp_pct_tbl.cep_c_vlan_edit == 1)?SOC_SAND_PP_PORT_L2_TYPE_CEP:SOC_SAND_PP_PORT_L2_TYPE_VBP;
     
  /*
   *	Extended P2P (incoming/outgoing)x
   */
  port_info->extend_p2p_info.enable_incoming_extend_p2p = SOC_SAND_NUM2BOOL(pinfo_flp_tbl.transparent_p2p_service_enable);

  /*
   *	MTU: Maximum Transmission Unit.
   *  Packets with size over this value will be processed
   *  according to action profile assigned to this event.
   */
  port_info->mtu = egq_ppct_tbl.mtu;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_info_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets the STP state of a port in a specific topology ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_port_stp_state_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                      topology_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_STP_STATE                          stp_state
  )
{
  uint32
    res = SOC_SAND_OK,
    eg_stp_state_bit,
    fld_val;
  SOC_PB_PP_IHP_STP_TABLE_TBL_DATA
    ing_stp_tbl_data;
  SOC_PB_PP_EPNI_STP_TBL_DATA
    eg_stp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_STP_STATE_SET_UNSAFE);

  res = soc_pb_pp_ihp_stp_table_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &ing_stp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch (stp_state)
  {
  case SOC_PB_PP_PORT_STP_STATE_BLOCK:
    fld_val = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_BLOCK;
    break;
  case SOC_PB_PP_PORT_STP_STATE_LEARN:
    fld_val = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_LEARN;
    break;
  case SOC_PB_PP_PORT_STP_STATE_FORWARD:
    fld_val = SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD;
    break;
  default:
    SOC_SAND_ERR_IF_ABOVE_MAX(stp_state, SOC_PB_PP_PORT_STP_STATE_MAX, SOC_PB_PP_PORT_STP_STATE_OUT_OF_RANGE_ERR, 15, exit);
  }

  res = soc_sand_bitstream_set_any_field(
    &fld_val,
    2*topology_id_ndx,
    2,
    ing_stp_tbl_data.per_port_stp_state
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  res = soc_pb_pp_ihp_stp_table_tbl_set_unsafe(
    unit,
    local_port_ndx,
    &ing_stp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  eg_stp_state_bit = (stp_state == SOC_PB_PP_PORT_STP_STATE_FORWARD ? SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_FORWARD : SOC_PB_PP_PORT_STP_STATE_EG_FLD_VAL_BLOCK);

  res = soc_pb_pp_epni_stp_tbl_get_unsafe(
    unit,
    topology_id_ndx,
    &eg_stp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_sand_bitstream_set_any_field(
    &eg_stp_state_bit,
    local_port_ndx,
    1,
    (uint32*)&eg_stp_tbl_data.egress_stp_state
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_epni_stp_tbl_set_unsafe(
    unit,
    topology_id_ndx,
    &eg_stp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_stp_state_set_unsafe()", local_port_ndx, topology_id_ndx);
}

uint32
  soc_pb_pp_port_stp_state_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                      topology_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_STP_STATE                          stp_state
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_STP_STATE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(topology_id_ndx, SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_MAX, SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(stp_state, SOC_PB_PP_PORT_STP_STATE_MAX, SOC_PB_PP_PORT_STP_STATE_OUT_OF_RANGE_ERR, 30, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_stp_state_set_verify()", local_port_ndx, topology_id_ndx);
}

uint32
  soc_pb_pp_port_stp_state_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                      topology_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_STP_STATE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(topology_id_ndx, SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_MAX, SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_stp_state_get_verify()", local_port_ndx, topology_id_ndx);
}

/*********************************************************************
*     Sets the STP state of a port in a specific topology ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_port_stp_state_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                      topology_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT_STP_STATE                          *stp_state
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val=0;
  SOC_PB_PP_IHP_STP_TABLE_TBL_DATA
    ing_stp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_STP_STATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(stp_state);

  res = soc_pb_pp_ihp_stp_table_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &ing_stp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
    ing_stp_tbl_data.per_port_stp_state,
    2*topology_id_ndx,
    2,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch (fld_val)
  {
  case SOC_PB_PP_PORT_STP_STATE_FLD_VAL_BLOCK:
     *stp_state = SOC_PB_PP_PORT_STP_STATE_BLOCK;
    break;
  case SOC_PB_PP_PORT_STP_STATE_FLD_VAL_LEARN:
    *stp_state = SOC_PB_PP_PORT_STP_STATE_LEARN;
    break;
  case SOC_PB_PP_PORT_STP_STATE_FLD_VAL_FORWARD:
    *stp_state = SOC_PB_PP_PORT_STP_STATE_FORWARD;
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_stp_state_get_unsafe()", local_port_ndx, topology_id_ndx);
}

/*********************************************************************
*     Set mapping from TM local port to PP local port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_port_tm_to_pp_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_pp_port
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_tbl;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA
    ihp_pp_port_config_tbl;
  uint8
    is_in = FALSE,
    is_out = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_UNSAFE);

  is_in   = SOC_PPC_IS_DIRECTION_INCOMING(direction_ndx);
  is_out  = SOC_PPC_IS_DIRECTION_OUTGOING(direction_ndx);

  /************************************************************************/
  /* Get current relevant information, change the port mapping            */
  /************************************************************************/

  if (is_in)
  {
    res = soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe(unit, local_tm_port_ndx, &ihp_pp_port_config_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ihp_pp_port_config_tbl.pp_port_offset1 = local_pp_port;
    ihp_pp_port_config_tbl.pp_port_use_offset_directly = 0x1;
  }

  if (is_out)
  {
    res = soc_pb_egq_pct_tbl_get_unsafe(unit, local_tm_port_ndx, &pct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_pp_egq_ppct_tbl_get_unsafe(unit, local_tm_port_ndx, &egq_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

    pct_tbl.out_pp_port = local_pp_port;
    egq_ppct_tbl.out_pp_port = local_pp_port;
  }

  /************************************************************************/
  /* Write the updated configuration to HW                                */
  /************************************************************************/
  if (is_out)
  {
    res = soc_pb_egq_pct_tbl_set_unsafe(unit, local_tm_port_ndx, &pct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_pb_pp_egq_ppct_tbl_set_unsafe(unit, local_tm_port_ndx, &egq_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
  }

  if (is_in)
  {
    res = soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe(unit, local_tm_port_ndx, &ihp_pp_port_config_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_tm_to_pp_map_set_unsafe()", local_tm_port_ndx, 0);
}

uint32
  soc_pb_pp_port_tm_to_pp_map_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_DIRECTION                          direction_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_pp_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_tm_port_ndx, SOC_PB_PP_TM_PORT_MAX, SOC_PB_PP_TM_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(direction_ndx, SOC_PB_PP_PORT_DIRECTION_NDX_MAX, SOC_PB_PP_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_pp_port, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_tm_to_pp_map_set_verify()", local_tm_port_ndx, 0);
}

uint32
  soc_pb_pp_port_tm_to_pp_map_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                                 local_tm_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_tm_port_ndx, SOC_PB_PP_TM_PORT_MAX, SOC_PB_PP_TM_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_tm_to_pp_map_get_verify()", local_tm_port_ndx, 0);
}

/*********************************************************************
*     Set mapping from TM local port to PP local port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_port_tm_to_pp_map_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                                 local_tm_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT                                    *local_pp_port_in,
    SOC_SAND_OUT SOC_PB_PP_PORT                                    *local_pp_port_out
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA
    ihp_pp_port_config_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(local_pp_port_in);
  SOC_SAND_CHECK_NULL_INPUT(local_pp_port_out);

  /*
   *	Incoming
   */
  res = soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe(unit, local_tm_port_ndx, &ihp_pp_port_config_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *local_pp_port_in = ihp_pp_port_config_tbl.pp_port_offset1;

  /*
   *	Outgoing
   */
  res = soc_pb_egq_pct_tbl_get_unsafe(unit, local_tm_port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *local_pp_port_out = pct_tbl.out_pp_port;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_port_tm_to_pp_map_get_unsafe()", local_tm_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_port module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_port_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_port;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_port module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_port_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_port;
}

uint32
  SOC_PB_PP_PORT_EXTEND_P2P_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_PORT_EXTEND_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_PORT_EXTEND_P2P_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_PORT_L2_VPN_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_PORT_L2_VPN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->learn_dest_type, SOC_PB_PP_PORT_LEARN_DEST_TYPE_MAX, SOC_PB_PP_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_PORT_L2_VPN_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_PORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_profile, SOC_PB_PP_PORT_PORT_PROFILE_MAX, SOC_PB_PP_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, SOC_PB_PP_PORT_TPID_PROFILE_MAX, SOC_PB_PP_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_domain, SOC_PB_PP_PORT_VLAN_DOMAIN_MAX, SOC_PB_PP_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_type, SOC_PB_PP_PORT_PORT_TYPE_MAX, SOC_PB_PP_PORT_PORT_TYPE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, &(info->initial_action_profile), 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PB_PP_PORT_ORIENTATION_MAX, SOC_PB_PP_PORT_ORIENTATION_OUT_OF_RANGE_ERR, 19, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ether_type_based_profile, SOC_PB_PP_PORT_ETHER_TYPE_BASED_PROFILE_MAX, SOC_PB_PP_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_PORT_EXTEND_P2P_INFO, &(info->extend_p2p_info), 21, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mtu, SOC_PB_PP_PORT_MTU_MAX, SOC_PB_PP_PORT_MTU_OUT_OF_RANGE_ERR, 24, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_not_found_profile, SOC_PB_PP_PORT_DA_NOT_FOUND_PROFILE_MAX, SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR, 24, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dflt_egress_ac, 0, SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR, 25, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_translation_profile, 0, SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR, 26, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tunnel_termination_profile, 0, SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR, 27, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_PORT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

