#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_diag.c,v 1.93 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG

#include <shared/bsl.h>
/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/swstate/access/sw_state_access.h>
#include <bcm/policer.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_ports.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fcf.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_extend_p2p.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_sa_auth.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>
#include <soc/dpp/PPD/ppd_api_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_test.h>

#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_diag.h>

#include <soc/dpp/TMC/tmc_api_packet.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_ac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>
#include <soc/hwstate/hw_log.h>

#include <soc/dpp/port_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_DIAG_PKT_TRACE_MAX                               (SOC_SAND_U32_MAX)
#define ARAD_PP_DIAG_FLAVOR_MAX                                  (ARAD_PP_NOF_DIAG_FLAVORS-1)
#define ARAD_PP_DIAG_LKUP_NUM_MAX                                (SOC_SAND_U32_MAX)
#define ARAD_PP_DIAG_BANK_ID_MAX                                 (SOC_SAND_U32_MAX)
/* size of header qual in bits*/
#define ARAD_PP_DIAG_HDR_QUAL_SIZE      (11)

/* size of header index in bits*/
#define ARAD_PP_DIAG_HDR_INDEX_SIZE      (7)

/* size of header index in bytes */
#define ARAD_PP_DIAG_HDR_MPLS_BYTE_SIZE      (4)

#define ARAD_PP_DIAG_FLD_LSB_MIN (0)
#define ARAD_PP_DIAG_FLD_MSB_MAX (255)
#define ARAD_PP_DIAG_FLD_LEN (256)

#define ARAD_PP_FIRST_TBL_ENTRY (0)

/* Encoding of the Destination in TM-Command */
#define ARAD_PP_DIAG_FRWRD_DECISION_DEST_DROP_CODE  (SOC_IS_JERICHO(unit)? 0x7FFFF: 0x3FFFF)
#define ARAD_PP_DIAG_FRWRD_DECISION_TYPE_UC_FLOW    (SOC_IS_JERICHO(unit)? 3: 1)
#define ARAD_PP_DIAG_FRWRD_DECISION_TYPE_MC         (0x1) /* Ingress Multicast in Jericho */
#define ARAD_PP_DIAG_FRWRD_DECISION_TYPE_EGRESS_MC  (0x2) /* Fabric-egress Multicast in Jericho */

#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
/* The maximal number of parameters per forwarding type */
#define ARAD_PP_DIAG_FORWARDING_TYPE_MAX_PARAMETERS     4
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_CHECK_DIAG_LAST_PACKET_VALIDITY(func_name, core)                           \
  res = arad_pp_diag_is_valid(unit, core, func_name, &ret_val);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 999, exit);                            \
  if (!ret_val) {                                                                \
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR, 999, exit);  \
  }   

/*calcute x/y in percent%*/
#define ARAD_PP_DIAG_PERCENT(x,y) (((100 * (x)) + ((y) / 2)) / (y))

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
typedef struct{
/*
 * name
 */
    CONST char *name;
/*
 * msb
 */
    uint32 msb;
/*
 * lsb
 */
    uint32 lsb;
/*
 * print function
 */
    void (*print_function)(CONST char *param_name, uint32 msb, uint32 lsb, CONST void *param);
}ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER;

STATIC void _uint8_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35d|\n\r"), param_name, msb, lsb, *((uint8*)param)));
} 

STATIC void _uint16_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35d|\n\r"), param_name, msb, lsb, *((uint16*)param)));
} 

STATIC void _uint32_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35d|\n\r"), param_name, msb, lsb, *((uint32*)param)));
}

STATIC void _ipv4_address_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    char
        decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35s|\n\r"), param_name, msb, lsb, soc_sand_pp_ip_long_to_string(*((uint32*)param), 1 ,decimal_ip)));
}

STATIC void _ipv6_address_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|"), param_name, msb, lsb));
    soc_sand_SAND_PP_IPV6_ADDRESS_print((SOC_SAND_PP_IPV6_ADDRESS*)param);
    LOG_CLI((BSL_META("|\n\r")));
}

typedef struct{
/*
 * Size in bytes
 */
uint32 size_in_bytes;
/*
 * number of parameters
 */
uint32 nof_params;
/*
 * parameters
 */
ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER params[ARAD_PP_DIAG_FORWARDING_TYPE_MAX_PARAMETERS];
}ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETERS;

CONST static ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETERS forwarding_type_parameters[ARAD_KBP_FRWRD_IP_NOF_TABLES] = {
/* Ipv4 Unicast Rpf 0 */
    {   10,
        3,
        {
            {"vrf", 75, 64, _uint16_print},
            {"SIP", 63, 32, _ipv4_address_print},
            {"DIP", 31, 0, _ipv4_address_print},
            {"", 0, 0, NULL}
        }
    },
/* Ipv4 Unicast Rpf 1 - undefined */
    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },
/* Ipv4 Multicast */
    {   12,
        4,
        {
            {"vrf", 91, 80, _uint16_print},
            {"in_rif", 75, 64, _uint16_print},
            {"SIP", 63, 32, _ipv4_address_print},
            {"DIP", 27, 0, _ipv4_address_print}
        }
    },
/* Ipv6 Unicast Rpf 0 */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
    {   34,
        3,
        {
            {"vrf", 267, 256, _uint16_print},
            {"SIP", 255, 128, _ipv6_address_print},
            {"DIP", 127, 0, _ipv6_address_print},
            {"", 0, 0, NULL}
        }
    },
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
    {   18,
        2,
        {
            {"vrf", 139, 128, _uint16_print},
            {"DIP", 127, 0, _ipv6_address_print},
            {"", 0, 0, NULL}
        }
    },
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
/* Ipv6 Unicast Rpf 1 - undefined */
    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },
/* Ipv6 Multicat */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
    {   35,
        4,
        {
            {"vrf", 275, 264, _uint16_print},
            {"in_rif", 259, 248, _uint16_print},
            {"SIP", 247, 120, _ipv6_address_print},
            {"DIP", 119, 0, _ipv6_address_print}
        }
    },
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
    {   19,
        3,
        {
            {"vrf", 147, 136, _uint16_print},
            {"in_rif", 131, 120, _uint16_print},
            {"DIP", 119, 0, _ipv6_address_print}
        }
    },
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
/* MPLS */
    {   6,
        4,
        {
            {"in_rif", 42, 31, _uint16_print},
            {"in_port", 30, 23, _uint8_print},
            {"exp", 22, 20, _uint8_print},
            {"mpls_label", 19, 0, _uint32_print}
        }
    },
/* Trill Unicast */
    {   2,
        1,
        {
            {"trill", 15, 0, _uint16_print},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },
/* Trill Multiocast */
    {   4,
        3,
        {
            {"esdai", 31, 31, _uint8_print},
            {"fid_vsi", 30, 16, _uint16_print},
            {"dis_tree_nick", 15, 0, _uint16_print},
            {"", 0, 0, NULL}
        }
    },
/* Dummy 0 - undefined */
    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },
/* Dummy 1 - undefined */
    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },
/* Dummy 2 - undefined */
    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },
/* Dummy 3 - undefined */
    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    }
};
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_diag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_MODE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_MODE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_MODE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_MODE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_TRACE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_TRACE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_TRACE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_RECEIVED_PACKET_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PARSING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PARSING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PARSING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TERMINATION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TERMINATION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TERMINATION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_ALL_TO_CPU),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_ALL_TO_CPU_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_ALL_TO_CPU_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_STAT_RESTORE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_STAT_RESTORE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_STAT_RESTORE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LEARNING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LEARNING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LEARNING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ENCAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ENCAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ENCAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_EG_DROP_LOG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_EG_DROP_LOG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_EG_DROP_LOG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_SEND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_SEND_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_SEND_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DBG_VAL_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LIF_DB_ID_TO_DB_TYPE_MAP_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ETH_HEADER_BUILD),


  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_diag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR",
    "The parameter 'pkt_trace' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DIAG_PKT_TRACES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DIAG_LEM_LKUP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VALID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VALID_OUT_OF_RANGE_ERR",
    "The parameter 'valid' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DIAG_LEM_LKUP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BUFF_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR",
    "The parameter 'flavor' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DIAG_FLAVORS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR",
    "The parameter 'lkup_num' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR",
    "The parameter 'bank_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR",
    "The parameter 'lkup_usage' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DIAG_TCAM_USAGES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'in_tm_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_PARSER_PROGRAM_POINTER_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_PARSER_PROGRAM_POINTER_OUT_OF_RANGE_ERR",
    "The parameter 'parser_program_pointer' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_PORT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR",
    "The parameter 'packet_qual' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'code' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_dest' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_IP_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_IP_OUT_OF_RANGE_ERR",
    "The parameter 'ip' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'base_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'length' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR",
    "The parameter 'fec_ptr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'encap_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETH_ENCAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR",
    "The parameter 'next_prtcl' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_L2_NEXT_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_PKT_HDR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_offset' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'range_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'term_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DIAG_MPLS_TERM_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR",
    "The parameter 'range_bit' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_PKT_FRWRD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR",
    "The parameter 'vrf' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR",
    "The parameter 'trill_uc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_hdr_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR",
    "The parameter 'valid_fields' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_METER1_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_METER1_OUT_OF_RANGE_ERR",
    "The parameter 'meter1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_METER2_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_METER2_OUT_OF_RANGE_ERR",
    "The parameter 'meter2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR",
    "The parameter 'dp_meter_cmd' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR",
    "The parameter 'counter1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR",
    "The parameter 'counter2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_CUD_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_CUD_OUT_OF_RANGE_ERR",
    "The parameter 'cud' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR",
    "The parameter 'drop_log' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'ether_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'total_size' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_packets' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_INVALID_TM_PORT_ERR,
    "ARAD_PP_DIAG_INVALID_TM_PORT_ERR",
    "Egress information provided invalid tm port.\n\r ",    
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    ARAD_PP_DIAG_RESTORE_NOT_SAVED_ERR,
    "ARAD_PP_DIAG_RESTORE_NOT_SAVED_ERR",
    "try to restore traps status when not saved.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR",
    "LIF DB ID is not used. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR,
    "ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR",
    "No relevant packet sent for diagnostics \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  /*
   * Last element. Do no touch.
   */
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

#define ARAD_PP_DIAG_FILTER_NOF_ETPP_FILTERS (18)
#define ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_1 (32)
#define ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_2 (9)

#define ARAD_PP_DIAG_FILTER_TO_LOG(__trap_log,__trap_name)  \
    __trap_log[((__trap_name) >> 31) & 1] |= ((__trap_name) & 0x7FFFFFFF)

static uint32 arad_pp_diag_filter_bit_to_enum[] = 
{
    ARAD_PP_DIAG_EG_DROP_REASON_ILLEGAL_EEI,
    ARAD_PP_DIAG_EG_DROP_REASON_PHP_CONFLICT,
    ARAD_PP_DIAG_EG_DROP_REASON_POP_IP_VERSION_ERR,
    0,
    ARAD_PP_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET,
    0,
    ARAD_PP_DIAG_EG_DROP_REASON_IP_TUNNEL_SNOOPED_PACKET,
    0,
    ARAD_PP_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET,
    0,
    ARAD_PP_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET,
    ARAD_PP_DIAG_EG_DROP_REASON_EEDB_LINK_LAYER_ENTRY_NEEDED,
    ARAD_PP_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP,
    ARAD_PP_DIAG_EG_DROP_REASON_STP_BLOCK,
    ARAD_PP_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE,
    ARAD_PP_DIAG_EG_DROP_REASON_OUT_LIF_WITH_DROP,
    ARAD_PP_DIAG_EG_DROP_REASON_MTU_VIOLATION,
    ARAD_PP_DIAG_EG_DROP_REASON_EEDB_LAST_ENTRY_NOT_AC,
    ARAD_PP_DIAG_EG_DROP_REASON_BOUNCE_BACK,
    ARAD_PP_DIAG_EG_DROP_REASON_OTM_INVALID,
    ARAD_PP_DIAG_EG_DROP_REASON_DSS_STACKING,
    ARAD_PP_DIAG_EG_DROP_REASON_EXCLUDE_SRC,
    ARAD_PP_DIAG_EG_DROP_REASON_LAG_MULTICAST,
    ARAD_PP_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP,
    ARAD_PP_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE,
    ARAD_PP_DIAG_EG_DROP_REASON_SRC_EQUAL_DEST,
    ARAD_PP_DIAG_EG_DROP_REASON_UNKNOWN_DA,
    ARAD_PP_DIAG_EG_DROP_REASON_SPLIT_HORIZON,
    ARAD_PP_DIAG_EG_DROP_REASON_PRIVATE_VLAN,
    ARAD_PP_DIAG_EG_DROP_REASON_TTL_SCOPE,
    ARAD_PP_DIAG_EG_DROP_REASON_MTU_VIOLATION,
    ARAD_PP_DIAG_EG_DROP_REASON_TRILL_TTL_ZERO,
    ARAD_PP_DIAG_EG_DROP_REASON_TRILL_SAME_INTERFACE,
    ARAD_PP_DIAG_EG_DROP_REASON_CNM,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_VERSION_ERR,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_VERSION_ERR,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_CHECKSUM_ERR,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_IHL_SHORT,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_TOTAL_LEGNTH,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_TTL_1,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_TTL_1,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_WITH_OPTIONS,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_TTL_0,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_TTL_0,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_SIP_EQUAL_DIP,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_DIP_IS_ZERO,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV4_SIP_IS_MC,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SIP_IS_MC,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_DIP_UNSPECIFIED,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SIP_UNSPECIFIED,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_LOOPBACK,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_HOP_BY_HOP,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_DEST,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_DEST,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_SRC,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_SRC,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_IPV4_COMPATIBLE,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_IPV4_MAPPED,
    ARAD_PP_DIAG_EG_DROP_REASON_IPV6_DEST_MC,
};

/* internal debug enable */
#ifdef SAND_LOW_LEVEL_SIMULATION
#define ARAD_DEBUG_INTERNAL (1)
#endif
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
 *     init diagnostic registers
 *********************************************************************/
uint32
  arad_pp_diag_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id

)
{
  return arad_pp_diag_pkt_trace_clear_unsafe(unit, core_id, SOC_PPC_DIAG_PKT_TRACE_ALL);
}


uint32
  arad_pp_diag_is_valid(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32             func_name,
    SOC_SAND_OUT uint32	*           ret_val
  )
{
  uint32 reg_val;
  uint32
    res = SOC_SAND_OK;
  uint64 reg_val64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!SOC_IS_ARDON(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core_id, &reg_val64));


      if (!COMPILER_64_IS_ZERO(reg_val64)) {
          *(ret_val) = 1;
          goto exit;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_DBG_FLP_PROGRAM_SELECTION_CAM_LINEr(unit, core_id, &reg_val64));
      if (!COMPILER_64_IS_ZERO(reg_val64)) {
          *(ret_val) = 1;
          goto exit;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_DBG_FLP_SELECTED_PROGRAMr(unit, core_id, &reg_val));
      if (reg_val != 0) {
          *(ret_val) = 1;
          goto exit;
      }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_DBG_PMF_SELECTED_CAM_LINE_0r(unit, core_id, &reg_val));
  if (reg_val != 0) {
      *(ret_val) = 1;
      goto exit;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHB_DBG_PMF_SELECTED_CAM_LINE_1r(unit, core_id,&reg_val));
  if (reg_val != 0) {
      *(ret_val) = 1;
      goto exit;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHB_DBG_PMF_SELECTED_PROGRAMr(unit, core_id,&reg_val));
  if (reg_val != 0) {
      *(ret_val) = 1;
      goto exit;
  }

  *(ret_val) = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_is_valid()", 0, 0);
}


/*********************************************************************
 *     read diagnostic value
 *********************************************************************/

#if defined(ARAD_DEBUG_INTERNAL) && ARAD_DEBUG_INTERNAL
 STATIC uint32
   arad_pp_diag_dbg_sim_fil_val(
   SOC_SAND_IN  uint32               sample_id,
   SOC_SAND_IN  uint32               blk,
   SOC_SAND_IN  uint32               addr,
   SOC_SAND_OUT uint32               regs_val[ARAD_PP_DIAG_DBG_VAL_LEN]
 )
 {
   return 0;
 }
#endif

uint32
  arad_pp_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core_id,
    SOC_SAND_IN  uint32               blk,
    SOC_SAND_IN  ARAD_PP_DIAG_REG_FIELD  *fld,
    SOC_SAND_OUT uint32               val[ARAD_PP_DIAG_DBG_VAL_LEN]
  )
{
  uint32
    reg32 = 0,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    data_above_64,
    field_above_64;
  soc_reg_t
      reg_00e0r, reg_00e1r, 
      ihp_00e0r = SOC_IS_JERICHO(unit)? IHP_REG_010Cr: IHP_REG_00E0r,
      ihp_00e1r  = SOC_IS_JERICHO(unit)? IHP_REG_010Dr: IHP_REG_00E1r,
      ihb_00e0r = SOC_IS_JERICHO(unit)? IHB_REG_00FEr: IHB_REG_00E0r,
      ihb_00e1r  = SOC_IS_JERICHO(unit)? IHB_REG_00FFr: IHB_REG_00E1r;
      int hw_access;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_PP_CLEAR(val,uint32,ARAD_PP_DIAG_DBG_VAL_LEN); 

  soc_hw_set_immediate_hw_access(unit, &hw_access);


  if ((blk == ARAD_IHP_ID) || (blk == ARAD_IHB_ID))
  {
    reg_00e0r = (blk == ARAD_IHP_ID)? ihp_00e0r: ihb_00e0r;
    reg_00e1r = (blk == ARAD_IHP_ID)? ihp_00e1r: ihb_00e1r;
    ARAD_FLD_TO_REG(reg_00e0r, FIELD_0_30f, fld->base, reg32, 11, exit);
    ARAD_FLD_TO_REG(reg_00e0r, FIELD_31_31f, 1, reg32, 16, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, reg_00e0r, core_id, 0, reg32));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, soc_reg_above_64_get(unit, reg_00e1r, core_id, 0, data_above_64));
    soc_reg_above_64_field_get(unit, reg_00e1r , data_above_64, FIELD_0_255f, field_above_64);    
  }
  else if (blk == ARAD_EGQ_ID)
  {
      ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_ADDRESSf, fld->base, reg32, 10, exit);
      ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, 1, reg32, 15, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_DBG_COMMANDr(unit, core_id,  reg32));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EGQ_DBG_DATAr(unit, core_id, data_above_64));
      soc_reg_above_64_field_get(unit, EGQ_DBG_DATAr, data_above_64, DBG_DATAf, field_above_64);    
  }
  else if (blk == ARAD_EPNI_ID)
  {
      ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_ADDRESSf, fld->base, reg32, 10, exit);
      ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, 1, reg32, 15, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_DBG_COMMANDr(unit, core_id,  reg32));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_DBG_DATAr(unit, core_id, data_above_64));
      soc_reg_above_64_field_get(unit, EPNI_DBG_DATAr, data_above_64, DBG_DATAf, field_above_64);    
  }

#if defined(ARAD_DEBUG_INTERNAL) && ARAD_DEBUG_INTERNAL
  arad_pp_diag_dbg_sim_fil_val(1,blk, fld->base,field_above_64);
#endif


  res = soc_sand_bitstream_get_any_field(field_above_64,fld->lsb,(fld->msb - fld->lsb +1),val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    soc_hw_restore_immediate_hw_access(unit, hw_access);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_dbg_val_get_unsafe()", 0, 0);
}
 
/*********************************************************************
 *     given termination code returns the place of the i-th MPLS label
 *********************************************************************/
STATIC uint32
  arad_pp_diag_term_code_to_header_index(
    SOC_SAND_IN  ARAD_PP_PKT_TERM_TYPE    term_type,
    SOC_SAND_IN  uint32                   lbl_index
  )
{
  uint32
    nof_labels=0;

  switch(term_type)
  {
  case ARAD_PP_PKT_TERM_TYPE_MPLS_ETH:
  case ARAD_PP_PKT_TERM_TYPE_CW_MPLS_ETH:
    nof_labels = 1;
    break;
  case ARAD_PP_PKT_TERM_TYPE_MPLS2_ETH:
  case ARAD_PP_PKT_TERM_TYPE_CW_MPLS2_ETH:
    nof_labels = 2;
    break;
  case ARAD_PP_PKT_TERM_TYPE_MPLS3_ETH:
  case ARAD_PP_PKT_TERM_TYPE_CW_MPLS3_ETH:
    nof_labels = 3;
    break;
  default:
    nof_labels = 0;
  break;
  }
  /* if there is label less than label index return 0 */
  if (lbl_index >= nof_labels)
  {
    return 0;/* indicate no such label in such level 'label-index' */
  }

  /* 1 for Ethernet, and header according to label index*/
  return 1 + lbl_index;
}

/* given trap code return 1 if it's for trap reason */
STATIC uint32
  arad_pp_diag_is_trap_code(
    SOC_SAND_IN  uint32                                 trap_code
  )
{
    if(trap_code >= ARAD_PP_TRAP_CODE_INTERNAL_BLOCK_PREFIX && 
       trap_code <= ARAD_PP_TRAP_CODE_INTERNAL_BLOCK_PREFIX + 4
       ) {
        return 0;
    }
    return 1;
}



uint32
  arad_pp_diag_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE app_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE signal_type,
    SOC_SAND_IN  uint32                                  dest_buffer,
    SOC_SAND_IN  uint32                                  asd_buffer,
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO             *fwd_decision
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* treat trap destination */
  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,
          app_type,
          dest_buffer,
          asd_buffer,
          ARAD_PP_FWD_DECISION_PARSE_DEST|ARAD_PP_FWD_DECISION_PARSE_EEI,
          fwd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_fwd_decision_in_buffer_parse()", 0, 0);

}

/*********************************************************************
 *     given forwarding code returns the place of the i-th MPLS label
 *********************************************************************/
STATIC ARAD_PP_PKT_HDR_TYPE
  arad_pp_diag_hdr_at_index(
    SOC_SAND_IN  ARAD_PP_PKT_HDR_STK_TYPE   pkt_type,
    SOC_SAND_IN  uint32                     hdr_index
  )
{
  ARAD_PP_PKT_HDR_TYPE
    hdr_type;

  hdr_type = SOC_SAND_GET_BITS_RANGE(pkt_type, 4*hdr_index + 3, 4*hdr_index);
  return hdr_type;
}

STATIC ARAD_PP_DIAG_FWD_LKUP_TYPE
  arad_pp_diag_frwrd_type_to_lkup_type_map(
    SOC_SAND_IN  ARAD_PP_PKT_FRWRD_TYPE   frwrd_type
  )
{
  switch(frwrd_type)
  {
  case ARAD_PP_PKT_FRWRD_TYPE_BRIDGE:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT;
  case ARAD_PP_PKT_FRWRD_TYPE_IPV4_UC:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC;
  case ARAD_PP_PKT_FRWRD_TYPE_IPV4_MC:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC;
  case ARAD_PP_PKT_FRWRD_TYPE_IPV6_UC:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC;
  case ARAD_PP_PKT_FRWRD_TYPE_IPV6_MC:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC;
  case ARAD_PP_PKT_FRWRD_TYPE_MPLS:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_ILM;
  case ARAD_PP_PKT_FRWRD_TYPE_TRILL:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC;
  case ARAD_PP_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT;
  case ARAD_PP_PKT_FRWRD_TYPE_CUSTOM1:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_FCF;
  case ARAD_PP_PKT_FRWRD_TYPE_CPU_TRAP:
  default:
    return ARAD_PP_DIAG_FWD_LKUP_TYPE_NONE;
  }
}

uint32
  arad_pp_diag_traps_range_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            regs_val[2],
    SOC_SAND_IN  uint32                            rng_id,
    SOC_SAND_IN  uint32                            nof_traps,
    SOC_SAND_OUT ARAD_PP_DIAG_TRAPS_INFO           *traps_info
  )
{
  uint32
    indx;
  ARAD_PP_TRAP_CODE
    trap_code;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  for (indx = 0; indx < nof_traps; ++indx)
  {
    if (soc_sand_bitstream_test_bit(regs_val, indx))
    {
      res = arad_pp_trap_sb_to_trap_code_map_get(
              unit,
              rng_id,
              indx,
              &trap_code
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_sand_bitstream_set_bit(
              traps_info->trap_stack,
              trap_code
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_traps_range_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE);
  
  res = arad_diag_sample_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_set_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_sample_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_set_verify()", 0, 0);
}

uint32
  arad_pp_diag_sample_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_get_verify()", 0, 0);
}

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_diag_sample_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the mode configuration for diag module, including
 *     diag-flavor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_DIAG_MODE_INFO                 *mode_info
  )
{
  soc_error_t
    rv = SOC_E_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_MODE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  rv = sw_state_access[unit].dpp.soc.arad.pp.diag.mode_info.set(
          unit,
          *mode_info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_MODE_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_DIAG_MODE_INFO, mode_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_set_verify()", 0, 0);
}

uint32
  arad_pp_diag_mode_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_MODE_INFO_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the mode configuration for diag module, including
 *     diag-flavor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_DIAG_MODE_INFO                      *mode_info
  )
{
  soc_error_t
    rv = SOC_E_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode_info);


  rv = sw_state_access[unit].dpp.soc.arad.pp.diag.mode_info.get(
          unit,
          mode_info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_VSI_INFO_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_mode_info_get_verify()", 0, 0);
}

/*********************************************************************
 *     Returns VSI information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_VSI_INFO                      *vsi_info
  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  ARAD_PP_DIAG_VSI_INFO_clear(vsi_info);

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld, core_id, ARAD_IHP_ID,3,0,208,193,20);
  }
  else {
      ARAD_PP_DIAG_FLD_READ(&fld, core_id, ARAD_IHP_ID,4,0,192,177,20);
  }
  vsi_info->vsi = regs_val[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     Stores an EPNI raw signal data to regs_val
 *********************************************************************/
uint32
  arad_pp_diag_get_raw_signal(
      int core_id,
      ARAD_MODULE_ID prm_blk,
      int prm_addr_msb,
      int prm_addr_lsb,
      int prm_fld_msb,
      int prm_fld_lsb,
      uint32 *regs_val)
{
    ARAD_PP_DIAG_REG_FIELD 
        fld;

    uint32
         unit = 0,
         res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_GET_EPNI_RAW_SIGNAL);

    if (regs_val == NULL) {
        res = SOC_SAND_ERR;
        goto exit;
    }
    /*This macro writes data to regs_val - a parameter of the using function*/
    ARAD_PP_DIAG_FLD_READ(&fld, core_id, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb, 20);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_epni_raw_signal()", 0, 0);

}

/*********************************************************************
*     Clear the trace of transmitted packet, so next trace
 *     info will relate to next packets to transmit
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_pkt_trace_clear_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                  core_id,
    SOC_SAND_IN  uint32                               pkt_trace
  )
{
  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
  uint64
    clear_val_64 = COMPILER_64_INIT(0, 0);
  uint32
    clear_val = 0xFFFFFFFF;
  uint32
    res = SOC_SAND_OK;
  uint64
	field64;
  uint32 
      nof_flp_program_selection_lines;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_TRACE_CLEAR_UNSAFE);

  if (pkt_trace & ARAD_PP_DIAG_PKT_TRACE_FEC)
  {
    ihb_fec_entry_accessed_tbl_data.fec_entry_accessed = 0;
    res = arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
            unit,
            ARAD_PP_FIRST_TBL_ENTRY,
            &ihb_fec_entry_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  if (pkt_trace & ARAD_PP_DIAG_PKT_TRACE_LIF)
  {
      COMPILER_64_SET(clear_val_64,  0, SOC_SAND_U32_MAX );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core_id, clear_val_64));

    
  }
  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_FLP)
  {
      nof_flp_program_selection_lines = SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines);

      /* This verification is needed for further work */
      if(nof_flp_program_selection_lines > 64) {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
      }
      if(nof_flp_program_selection_lines < 64) {
          COMPILER_64_SET(clear_val_64, 0, 1);
          COMPILER_64_SHL(clear_val_64, nof_flp_program_selection_lines);
          COMPILER_64_SUB_32(clear_val_64, 1);
      } else {
          COMPILER_64_SET(clear_val_64, SOC_SAND_U32_MAX , SOC_SAND_U32_MAX );
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_PROGRAM_SELECTION_CAM_LINEr(unit, core_id, clear_val_64));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_SELECTED_PROGRAMr(unit, core_id, clear_val));
  }

  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_PMF)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_PMF_SELECTED_CAM_LINE_0r(unit, core_id,  clear_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  47,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_PMF_SELECTED_CAM_LINE_1r(unit, core_id,  clear_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  48,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_PMF_SELECTED_PROGRAMr(unit, core_id,  clear_val));  
  }

  if (pkt_trace & ARAD_PP_DIAG_PKT_TRACE_TUNNEL_RNG)
  {
    
  }
  /* Traps clear*/
  if (pkt_trace & ARAD_PP_DIAG_PKT_TRACE_TRAP)
  {
    /* LLR traps */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_LLR_TRAP_0r(unit, core_id,  clear_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_LLR_TRAP_1r(unit, core_id,  clear_val));
    /* VTT traps */
    COMPILER_64_SET(field64,0,clear_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_VTT_TRAPS_ENCOUNTEREDr(unit, core_id,  &field64));
    /* FLP traps */
    COMPILER_64_SET(field64,0,clear_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_TRAP_0r(unit, core_id, clear_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_TRAP_1r(unit, core_id, clear_val));
    /* FER traps */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_FER_TRAPr(unit, core_id,  clear_val));
  }

  if (pkt_trace & ARAD_PP_DIAG_PKT_TRACE_EG_DROP_LOG)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_PP_INT_REGr(unit, core_id,  clear_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr(unit, core_id, clear_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTER_2r(unit, core_id, clear_val));
  }
  
exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_trace_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_pkt_trace_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_TRACE_CLEAR_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pkt_trace, ARAD_PP_DIAG_PKT_TRACE_MAX, ARAD_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_trace_clear_verify()", 0, 0);
}

/*********************************************************************
*     Returns the header of last received packet entered the
 *     device and the its association to TM/system/PP ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_received_packet_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_RECEIVED_PACKET_INFO      *rcvd_pkt_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    indx,
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    dummy,
    cur_lsb=0,
    read_size,
    tmp=0;
  uint8
    is_lag;
  uint32
    res = SOC_SAND_OK, valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rcvd_pkt_info);

  ARAD_PP_DIAG_RECEIVED_PACKET_INFO_clear(rcvd_pkt_info);

  *ret_val = SOC_PPC_DIAG_OK; 

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_RECEIVED_PACKET_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   


  /* TM-port */
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,ARAD_PP_DIAG_FLD_MSB_MAX,ARAD_PP_DIAG_FLD_LSB_MIN,15); 
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          regs_val,
          211,
          8,
          &(tmp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  rcvd_pkt_info->in_tm_port = tmp;
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,2,ARAD_PP_DIAG_FLD_MSB_MAX,ARAD_PP_DIAG_FLD_LSB_MIN,15);
  /* pp-context */
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          regs_val,
          SOC_IS_JERICHO(unit)? 67: 68,
          3,
          &(tmp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  switch(tmp) {
  case ARAD_PARSER_PARSER_PROGRAM_POINTER_RAW:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_RAW;
    break;

  case ARAD_PARSER_PARSER_PROGRAM_POINTER_ITMH:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_TM;
    break;

  case ARAD_PARSER_PARSER_PROGRAM_POINTER_ETH:
  case ARAD_PARSER_PARSER_PROGRAM_POINTER_RAW_MPLS:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_ETH;
    break;

  case ARAD_PARSER_PARSER_PROGRAM_POINTER_FTMH:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_STACKING;
    break;

  default:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_NONE;
  }

  res = soc_sand_bitstream_get_any_field(
          regs_val,
          SOC_IS_JERICHO(unit)? 82: 83,
          7,
          &(rcvd_pkt_info->packet_header.buff_len)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* hw has size - 1 */
  rcvd_pkt_info->packet_header.buff_len += 1; 

  read_size = ARAD_PP_DIAG_FLD_LEN - (SOC_IS_JERICHO(unit)? 89: 90); 
  res = soc_sand_bitstream_get_any_field( 
          regs_val,
          (SOC_IS_JERICHO(unit)? 89: 90),
          read_size,
          rcvd_pkt_info->packet_header.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  cur_lsb = read_size;
  read_size = ARAD_PP_DIAG_FLD_LEN;
  for (indx = 1; indx <= 3; ++indx)
  {
    ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,indx+2,ARAD_PP_DIAG_FLD_MSB_MAX,ARAD_PP_DIAG_FLD_LSB_MIN,45);
    res = soc_sand_bitstream_set_any_field(
            regs_val,
            cur_lsb,
            read_size,
            rcvd_pkt_info->packet_header.buff
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    cur_lsb += read_size;
  }
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,ARAD_PP_DIAG_FLD_MSB_MAX,ARAD_PP_DIAG_FLD_LSB_MIN,55);
  res = soc_sand_bitstream_set_any_field(
          regs_val,
          cur_lsb,
          1023-cur_lsb+1,
          rcvd_pkt_info->packet_header.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,206,191,65);
  tmp = regs_val[0];
  res = arad_ports_logical_sys_id_parse(
          unit,
          tmp, /*sys_logic_port_id*/
          &is_lag,
          &(rcvd_pkt_info->src_sys_port.sys_id),
          &dummy,
          &(rcvd_pkt_info->src_sys_port.sys_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  if (is_lag)
  {
    rcvd_pkt_info->src_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
  }
  else
  {
    rcvd_pkt_info->src_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
  }
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,226,219,75);
  rcvd_pkt_info->in_pp_port = regs_val[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_received_packet_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_received_packet_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_received_packet_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns network headers qualified on packet upon
 *     parsing, including packet format, L2 headers fields,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_parsing_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_INFO              *pars_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    act_hdr_index,
    hdr_indx,
    inner_eth_indx=0,
    outer_eth_indx=0,
    qual[2],
    in_port,
    hdr_qual,
    lst_mpls_header=0,
    tmp=0;
  uint8
    inc_bos=FALSE,
    outer_eth_vld = FALSE,
    inner_eth_vld = FALSE,
    prev_mpls=FALSE,
    found,
    l3_next_prtcl_ndx;
  ARAD_PP_PORT_INFO
    port_info;
  ARAD_PP_LLP_PARSE_TPID_PROFILE_INFO
    tpid_profile_info;
  ARAD_PP_LLP_PARSE_TPID_VALUES
    tpid_vals;
  uint32
    res = SOC_SAND_OK;
  uint32
      inner_vid = 0, valid;
  ARAD_PP_L3_NEXT_PRTCL_TYPE l3_next_prtcl_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PARSING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pars_info);

  ARAD_PP_DIAG_PARSING_INFO_clear(pars_info);

  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_PARSING_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,1,104,99,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,1,141,136,5);
  }
  tmp = regs_val[0];
  arad_pp_pkt_hdr_interanl_val_to_type_map(tmp,&pars_info->hdr_type);

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,146,92,15);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,147,93,15);
  }
  qual[0]= regs_val[0];
  qual[1]= regs_val[1];

  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,190,156,15);
  for (act_hdr_index = 0, hdr_indx = 0 ; hdr_indx < ARAD_PP_DIAG_MAX_NOF_HDRS; ++hdr_indx, ++act_hdr_index)
  {
    /* set header type */
    pars_info->hdrs_stack[hdr_indx].hdr_type = arad_pp_diag_hdr_at_index(pars_info->hdr_type,hdr_indx);
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_ETH && outer_eth_vld == FALSE)
    {
      outer_eth_indx = hdr_indx;
      outer_eth_vld = TRUE;
    }
    else if (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_ETH)
    {
      inner_eth_indx = hdr_indx;
      inner_eth_vld = TRUE;
    }

    /* no more network headers */
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_NONE)
    {
      break;
    }

    /* if not first MPLS */
    if (prev_mpls && pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_MPLS)
    {
      /* previous index + previous (MPLS) header size, which cannot include CW*/
      pars_info->hdrs_stack[hdr_indx].hdr_offset = pars_info->hdrs_stack[hdr_indx-1].hdr_offset + ARAD_PP_DIAG_HDR_MPLS_BYTE_SIZE;
      /* actually header index was not used, next time you can use it */
      --act_hdr_index;
    }
    else
    {
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              ARAD_PP_DIAG_HDR_INDEX_SIZE*act_hdr_index,
              ARAD_PP_DIAG_HDR_INDEX_SIZE,
              &(pars_info->hdrs_stack[hdr_indx].hdr_offset)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      hdr_qual = 0;
      res = soc_sand_bitstream_get_any_field(
              qual,
              ARAD_PP_DIAG_HDR_QUAL_SIZE*act_hdr_index,
              ARAD_PP_DIAG_HDR_QUAL_SIZE,
              &hdr_qual
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (
          (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_IPV4) ||
          (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_IPV6)
         )
      {
        l3_next_prtcl_ndx = SOC_SAND_GET_BITS_RANGE(hdr_qual,10,7);
        res = arad_pp_ndx_to_l3_prtcl(l3_next_prtcl_ndx, &l3_next_prtcl_type, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        pars_info->hdrs_stack[hdr_indx].ip.next_prtcl = l3_next_prtcl_type;
        pars_info->hdrs_stack[hdr_indx].ip.hdr_err = SOC_SAND_GET_BIT(hdr_qual,0);
        pars_info->hdrs_stack[hdr_indx].ip.is_mc = SOC_SAND_GET_BIT(hdr_qual,5);
        pars_info->hdrs_stack[hdr_indx].ip.is_fragmented = SOC_SAND_GET_BIT(hdr_qual,6);
      }
      else if (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_MPLS)
      {
        inc_bos = (uint8)SOC_SAND_GET_BIT(hdr_qual,0);
      }
      else if (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_ETH)
      {
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.inner_tpid = SOC_SAND_GET_BITS_RANGE(hdr_qual,1,0);
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.is_outer_prio = (uint8)SOC_SAND_GET_BITS_RANGE(hdr_qual,2,2);
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.outer_tpid = SOC_SAND_GET_BITS_RANGE(hdr_qual,4,3);
        pars_info->hdrs_stack[hdr_indx].eth.encap_type = (SOC_SAND_PP_ETH_ENCAP_TYPE)SOC_SAND_GET_BITS_RANGE(hdr_qual,6,5);
      }
    }

    /* set last mpls header index */
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == ARAD_PP_PKT_HDR_TYPE_MPLS)
    {
      lst_mpls_header = hdr_indx;
      prev_mpls = TRUE;
    }
  }
  /* if there is MPLS label in header*/
  if (lst_mpls_header != 0)
  {
    pars_info->hdrs_stack[lst_mpls_header].mpls.bos = inc_bos;
  }
  /* further information for outer Ethernet*/

  /*l2 next header */
  if (outer_eth_vld)
  {
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,102,99,15);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,103,100,15);
      }
    res = arad_pp_l2_next_prtcl_interanl_val_to_type_map(
            regs_val[0],
            &(pars_info->hdrs_stack[outer_eth_indx].eth.next_prtcl),
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,1,194,191,15);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,1,212,209,15);
    }
    pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tag_format = (SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT)regs_val[0];

    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2,4,255,251,15);
        tmp = regs_val[0] & 0x1F;
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2,5,19,0,16);
        tmp += (regs_val[0] << (255 - 251 + 1));
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,4,255,236,15);
        tmp = regs_val[0];
    }
    res = soc_sand_bitstream_get_any_field(&tmp,0,12,&pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].vid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_bitstream_get_any_field(&tmp, 13, SOC_IS_JERICHO(unit)? 12: 7, &inner_vid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,5,4,0,15);
        inner_vid |= (regs_val[0] & 0x1F) << 7;
    }
    res = soc_sand_bitstream_get_any_field(&inner_vid,0,12,&pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].vid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2,0,24,13,15);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,0,11,0,15);
  }
    pars_info->initial_vid = regs_val[0];

 /*
  * get TPID info, using driver calls
  */
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,226,219,15);
  in_port = regs_val[0];
  
  res = arad_pp_port_info_get_unsafe(
          unit,
          SOC_CORE_DEFAULT,
          in_port,
          &port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_llp_parse_tpid_profile_info_get_unsafe(
          unit,
          port_info.tpid_profile,
          &tpid_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_llp_parse_tpid_values_get_unsafe(
          unit,
          &tpid_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  /* Get outer TPID in outer Ethernet header */
  if (outer_eth_vld && pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid != ARAD_PP_LLP_PARSE_TPID_INDEX_NONE)
  {
    /* Get TPID value according to tpid profile info */
    if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID1)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
    else if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID2)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];  
  }
  else
  {
      /* When there is no valid TPID, set vid value to 0 */
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].vid = 0;
  }

  /* Get inner TPID in outer Ethernet header */
  if (outer_eth_vld && pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.inner_tpid != ARAD_PP_LLP_PARSE_TPID_INDEX_NONE)
  {
    /* Get TPID value according to tpid profile info */
    if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.inner_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID1)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
    else if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.inner_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID2)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];  
  }
  else
  {
      /* When there is no valid TPID, set vid value to 0 */
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].vid = 0;
  }

  if (inner_eth_vld)
  {
    /* Get outer TPID in inner Ethernet header */
    if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.outer_tpid != ARAD_PP_LLP_PARSE_TPID_INDEX_NONE)
    {
	  /* Get TPID value according to tpid profile info */
	  if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.outer_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID1)
	    pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
	  else if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.outer_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID2)
	    pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];
    }
    else
    {
      /* When there is no valid TPID, set vid value to 0 */
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].vid = 0;
    }  
  
    /* Get inner TPID in inner Ethernet header */
    if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid != ARAD_PP_LLP_PARSE_TPID_INDEX_NONE)
    {
	  /* Get TPID value according to tpid profile info */
	  if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID1)
	    pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
	  else if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid == ARAD_PP_LLP_PARSE_TPID_INDEX_TPID2)
	    pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];
    }
    else
    {
      /* When there is no valid TPID, set vid value to 0 */
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[1].vid = 0;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_parsing_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_parsing_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PARSING_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_parsing_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns information obtained by termination including
 *     terminated headers
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_termination_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_TERM_INFO                 *term_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    lbl_indx,
    frwrd_code = 0,
    frwrd_offset = 0,
    qual,
    hdr_index;
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_DIAG_PARSING_INFO
    pars_info;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TERMINATION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_info);


  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_TERMINATION_INFO_GET, &valid);              
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  *ret_val = SOC_PPC_DIAG_OK; 
  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_TERM_INFO_clear(term_info);

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,0,229,226,10);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,0,211,208,10);
  }
  term_info->term_type = (ARAD_PP_PKT_TERM_TYPE)regs_val[0];

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,146,92,20);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,1,6,147,93,20);
  }

  /* check if i-th label was terminated */
  for (lbl_indx=0; lbl_indx <3; ++lbl_indx)
  {
    hdr_index = arad_pp_diag_term_code_to_header_index(term_info->term_type,0);
    if (hdr_index == 0)
    {
      break;
    }
    /* get qualifier at place hdr_index*/
    qual = 0;
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            hdr_index*ARAD_PP_DIAG_HDR_QUAL_SIZE, 
            ARAD_PP_DIAG_HDR_QUAL_SIZE,
            &qual
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
  }

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,7,73,65,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,6,219,211,5);
  }
  res = soc_sand_bitstream_get_any_field(regs_val,5,4,&frwrd_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  term_info->frwrd_type = frwrd_code;

  /* forwarding offset */
  res = soc_sand_bitstream_get_any_field(regs_val,0,3,&frwrd_offset);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* parsing info */
  res = arad_pp_diag_parsing_info_get_unsafe(unit,core_id,&pars_info, ret_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
     2nd myMAC:
     - fwd-offset  > 4
     - fwd-offset == 4 and packet is not IPoMPLSoIPoE (identified by parsing info: 2nd header == IPv4/IPv6 && next_prtcl == MPLS)
     - fwd-offset == 3 and packet is EoE
  */
  term_info->is_scnd_my_mac =  ((frwrd_offset >  4) ||
                               ((frwrd_offset == 4) &&
                                    !(((pars_info.hdrs_stack[1].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV4) ||
                                       (pars_info.hdrs_stack[1].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV6)) &&
                                       (pars_info.hdrs_stack[1].ip.next_prtcl == SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS))) ||
                               ((frwrd_offset == 3) && (pars_info.hdr_type == SOC_PPC_PKT_HDR_STK_TYPE_ETH_ETH)));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_termination_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_termination_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TERMINATION_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_termination_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Simulate IP lookup in the device tables and return
 *     FEC-pointer
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_frwrd_lpm_lkup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT              *ret_val

  )
{
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY
      test_lpm_key;
   
  uint32
    res = SOC_SAND_OK;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lpm_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_ptr);
  SOC_SAND_CHECK_NULL_INPUT(found);

  *ret_val = SOC_PPC_DIAG_OK;


  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_clear(&test_lpm_key);
  sal_memcpy(&test_lpm_key.key,&lpm_key->key,sizeof(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY));
  test_lpm_key.vrf = lpm_key->vrf;

  res = arad_pp_frwrd_ipv4_test_lpm_lkup_get(unit,&test_lpm_key,fec_ptr,found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lpm_lkup_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_frwrd_lpm_lkup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_VPN_KEY              *lpm_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY);

  res = ARAD_PP_DIAG_IPV4_VPN_KEY_verify(unit, lpm_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lpm_lkup_get_verify()", 0, 0);
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
STATIC void
    arad_pp_diag_request_to_structure(
        SOC_SAND_IN  uint32 *request_buffer,
        SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_OUT void **params)
{
    uint32
        nof_params = forwarding_type_parameters[type].nof_params;
    uint32
        param_ndx;
    uint32
        tmp;
    CONST ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER
        *cur_param;
    for(param_ndx = 0; param_ndx < nof_params; ++param_ndx) {
        cur_param = &forwarding_type_parameters[type].params[param_ndx];
        /* we assume that if the length of the parameter is > 16 bits it is a uint32 bits array, otherwise we will use the tmp */
        if(cur_param->msb - cur_param->lsb + 1 > 16) {
                SHR_BITCOPY_RANGE((uint32*) params[param_ndx],
                                  0,
                                  request_buffer,
                                  cur_param->lsb,
                                  cur_param->msb - cur_param->lsb + 1);
        } else {
            tmp = 0;
            SHR_BITCOPY_RANGE(&tmp,
                              0,
                              request_buffer,
                              cur_param->lsb,
                              cur_param->msb - cur_param->lsb + 1);
            if(cur_param->msb - cur_param->lsb + 1 <= 8) {
                *((uint8*)params[param_ndx]) = tmp & 0xff;
            } else {
                *((uint16*)params[param_ndx]) = tmp & 0xffff;
            }
        }
    }
}

STATIC ARAD_PP_DIAG_IPV4_UNICAST_RPF
  arad_pp_diag_request_to_ipv4_unicast_rpf(uint32 *request_buffer)
{
  void
    *params[3];
  ARAD_PP_DIAG_IPV4_UNICAST_RPF
    res;
  ARAD_PP_DIAG_IPV4_UNICAST_RPF_clear(&res);
  params[0] = &res.vrf;
  params[1] = &res.sip;
  params[2] = &res.dip;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, params);
  
  return res;
}
  
STATIC ARAD_PP_DIAG_IPV4_MULTICAST
  arad_pp_diag_request_to_ipv4_multicast(uint32 *request_buffer)
{
  void
    *params[5];
  ARAD_PP_DIAG_IPV4_MULTICAST
    res;

  ARAD_PP_DIAG_IPV4_MULTICAST_clear(&res);
  params[0] = &res.vrf;
  params[1] = &res.in_rif;
  params[2] = &res.sip;
  params[3] = &res.dip;
  params[4] = &res.dip;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, params);
    
  return res;
}

STATIC ARAD_PP_DIAG_IPV6_UNICAST_RPF
  arad_pp_diag_request_to_ipv6_unicast_rpf(uint32 *request_buffer)
{
  void
    *params[3];
  ARAD_PP_DIAG_IPV6_UNICAST_RPF
    res;

  ARAD_PP_DIAG_IPV6_UNICAST_RPF_clear(&res);
  params[0] = &res.vrf;
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
  params[1] = res.sip.address;
  params[2] = res.dip.address;
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  params[1] = res.dip.address;
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, params);
  
  return res;
}

STATIC ARAD_PP_DIAG_IPV6_MULTICAST
  arad_pp_diag_request_to_ipv6_multicast(uint32 *request_buffer)
{
  void
    *params[5];
  ARAD_PP_DIAG_IPV6_MULTICAST
    res;

  ARAD_PP_DIAG_IPV6_MULTICAST_clear(&res);
  params[0] = &res.vrf;
  params[1] = &res.in_rif;
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
  params[2] = res.sip.address;
  params[3] = res.dip.address;
  params[4] = res.dip.address;
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  params[2] = res.dip.address;
  params[3] = res.dip.address;
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, params);

  return res;
}

STATIC ARAD_PP_DIAG_MPLS
  arad_pp_diag_request_to_mpls(uint32 *request_buffer)
{
  void
    *params[4];
  ARAD_PP_DIAG_MPLS
    res;

  ARAD_PP_DIAG_MPLS_clear(&res);
  params[0] = &res.in_rif;
  params[1] = &res.in_port;
  params[2] = &res.exp;
  params[3] = &res.mpls_label;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_LSR, params);
  
  return res;
}

STATIC ARAD_PP_DIAG_TRILL_UNICAST
  arad_pp_diag_request_to_trill_unicast(uint32 *request_buffer)
{
  void
    *param;
  ARAD_PP_DIAG_TRILL_UNICAST
    res;

  ARAD_PP_DIAG_TRILL_UNICAST_clear(&res);
  param = &res.egress_nick;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, &param);
  
  return res;
};

STATIC ARAD_PP_DIAG_TRILL_MULTICAST
  arad_pp_diag_request_to_trill_multicast(uint32 *request_buffer)
{
  void
    *params[3];
  ARAD_PP_DIAG_TRILL_MULTICAST
    res;

  ARAD_PP_DIAG_TRILL_MULTICAST_clear(&res);
  params[0] = &res.esdai;
  params[1] = &res.fid_vsi;
  params[2] = &res.dist_tree_nick;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, params);
  
  return res;
}

STATIC uint32
  arad_pp_diag_reply_to_ip_reply_record(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 *reply,
    SOC_SAND_OUT ARAD_PP_DIAG_IP_REPLY_RECORD* ip_reply_record_rpf
  )
{
  uint32
    tmp,
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(ip_reply_record_rpf);

  /* Match Status [55] */
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 55, 1);
  ip_reply_record_rpf->match_status = tmp & 0xff;

  /* is dynamic [42] */
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 42, 1);
  ip_reply_record_rpf->is_synamic = tmp & 0xff;

  /* P2P Service [41] */
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 41, 1);
  ip_reply_record_rpf->p2p_service = tmp & 0xff;

  /* Identifier [40:39] */
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 39, 2);
  ip_reply_record_rpf->identifier = tmp & 0xff;

  if(ip_reply_record_rpf->identifier == 0x0) {
    /* Out LIF Valid [35] */
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 35, 2);
    ip_reply_record_rpf->out_lif_valid = tmp & 0xff;

    /* Out LIF [34:19] */
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 19, 16);
    ip_reply_record_rpf->out_lif = tmp & 0xffff;

    /* Destination [18:0] */
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 0, 19);
    
    res = arad_pp_fwd_decision_in_buffer_parse(
            unit,
            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
            tmp,
            0,
            ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_LEGACY,
            &ip_reply_record_rpf->destination
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  } else if(ip_reply_record_rpf->identifier == 0x1) {
    /* EEI [38:16] */
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 16, 23);
    ip_reply_record_rpf->eei = tmp;
    
    /* FEC-Ptr [14:0] */
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 0, 15);
    ip_reply_record_rpf->fec_ptr =tmp & 0xff;
  } else {
  /* Invalid identifier */
    SOC_SAND_SET_ERROR_CODE(0, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_reply_to_ip_reply_record_rpf()",0,0);
}    

STATIC void
    ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(
        SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_IN void **params,
        SOC_SAND_IN uint32 offset
    )
{
    uint32
        nof_params = forwarding_type_parameters[type].nof_params;
    uint32
        param_ndx;
    CONST ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER
        *cur_param;
    uint32
        offset_lcl = offset - 8*forwarding_type_parameters[type].size_in_bytes;

    LOG_CLI((BSL_META("Master Key:\n\r")));
    LOG_CLI((BSL_META("\n\r")));
    LOG_CLI((BSL_META("-------------------------------------------------------------------\n\r")));
    LOG_CLI((BSL_META("%-20s|%-4s|%-4s|%-35s|\n\r"), "Field name", "MSB", "LSB", "Last-packet Value"));
    LOG_CLI((BSL_META("-------------------------------------------------------------------\n\r")));
    for(param_ndx = 0; param_ndx < nof_params; ++param_ndx) {
        cur_param = &forwarding_type_parameters[type].params[param_ndx];
        cur_param->print_function(cur_param->name, cur_param->msb + offset_lcl, cur_param->lsb + offset_lcl, params[param_ndx]);
    }
}
 
uint32
    arad_pp_diag_get_frwrd_type_size(
        SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_OUT uint32                   *size
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(size);

    SOC_SAND_ERR_IF_ABOVE_NOF(type, ARAD_KBP_FRWRD_IP_NOF_TABLES, 1, 10, exit);
    
    *size = forwarding_type_parameters[type].size_in_bytes;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_frwrd_type_size()",0,0);
}

ARAD_KBP_FRWRD_IP_TBL_ID
    arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(
        SOC_SAND_IN ARAD_PP_DIAG_FWD_LKUP_TYPE type
    )
{
    switch (type) {
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_NONE:
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT:
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_BMACT:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV4_MC;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV6_MC;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_VPN:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_ILM:
            return ARAD_KBP_FRWRD_TBL_ID_LSR;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC:
            return ARAD_KBP_FRWRD_TBL_ID_TRILL_UC;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC:
            return ARAD_KBP_FRWRD_TBL_ID_TRILL_MC;
        case ARAD_PP_DIAG_FWD_LKUP_TYPE_FCF:
        case ARAD_PP_NOF_DIAG_FWD_LKUP_TYPES:
        default:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
    }
}
 
void
  ARAD_PP_DIAG_IPV4_UNICAST_RPF_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_IPV4_UNICAST_RPF *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[3];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->sip;
  params[2] = &info->dip;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

void
  ARAD_PP_DIAG_IPV4_MULTICAST_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_IPV4_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[4];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->in_rif;
  params[2] = &info->sip;
  params[3] = &info->dip;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

void
  ARAD_PP_DIAG_IPV6_UNICAST_RPF_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_IPV6_UNICAST_RPF *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[3];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
  params[1] = &info->sip;
  params[2] = &info->dip;
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  params[1] = &info->dip;
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

void
  ARAD_PP_DIAG_IPV6_MULTICAST_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_IPV6_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[4];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->in_rif;
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
  params[2] = &info->sip;
  params[3] = &info->dip;
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  params[2] = &info->dip;
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

void
  ARAD_PP_DIAG_MPLS_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_MPLS *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[4];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->in_rif;
  params[1] = &info->in_port;
  params[2] = &info->exp;
  params[3] = &info->mpls_label;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_LSR, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

void
  ARAD_PP_DIAG_TRILL_UNICAST_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_TRILL_UNICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void
    *param;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  param = &info->egress_nick;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, &param, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

void
  ARAD_PP_DIAG_TRILL_MULTICAST_print_with_offsets(
    SOC_SAND_IN ARAD_PP_DIAG_TRILL_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[3];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->esdai;
  params[1] = &info->fid_vsi;
  params[2] = &info->dist_tree_nick;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
};

uint32
  arad_pp_diag_get_request(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  int                     core_id,
    SOC_SAND_IN  uint32                     nof_bytes,
    SOC_SAND_OUT uint32                     *buffer
  )
{
  uint32
    res;
  uint32
    tmp_buf[ARAD_PP_DIAG_DBG_VAL_LEN];
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    nof_bytes_lcl = nof_bytes;
  int32
    offset;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(buffer);

  if(nof_bytes_lcl > 0) {
    /* getting the signals of the request */
    fld.base = 196612;
    fld.msb = 7;
    fld.lsb = 0;

    res = arad_pp_diag_dbg_val_get_unsafe(
            unit,
            core_id,
            21,
            &fld,
            tmp_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SHR_BITCOPY_RANGE(buffer, (nof_bytes_lcl - 1)* 8, tmp_buf, 0, 7);
    --nof_bytes_lcl;
  }
  
  while(nof_bytes_lcl > 0) {
    --fld.base;
    fld.msb = 255;
    fld.lsb = 0;

    res = arad_pp_diag_dbg_val_get_unsafe(
            unit,
            core_id,
            21,
            &fld,
            tmp_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    offset = 7;
    while(nof_bytes_lcl > 4 && offset >= 0) {
      SHR_BITCOPY_RANGE(buffer, (nof_bytes_lcl - 4) * 8, tmp_buf + offset, 0, 32);
      nof_bytes_lcl -= 4;
      --offset;
    }
    if(nof_bytes_lcl > 0 && offset >= 0) {
      SHR_BITCOPY_RANGE(buffer, 0, tmp_buf + offset, (4 - nof_bytes_lcl) * 8, nof_bytes_lcl*8);
      nof_bytes_lcl = 0;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_request()",0,0);
}

STATIC uint32
  arad_pp_diag_get_replies(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_IP_REPLY_RECORD         *ip_reply_record,
    SOC_SAND_OUT ARAD_PP_DIAG_SECOND_IP_REPLY_RECORD  *second_ip_reply_record
  )
{
  uint32
    res;
  uint32
    buf[2],
    tmp_buf[ARAD_PP_DIAG_DBG_VAL_LEN];
  ARAD_PP_DIAG_REG_FIELD
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(ip_reply_record);
  SOC_SAND_CHECK_NULL_INPUT(second_ip_reply_record);

  /* getting the signals of the reply */
  fld.base = 262144;
  fld.msb = 128;
  fld.lsb = 1;
      
  res = arad_pp_diag_dbg_val_get_unsafe(
          unit,
          core_id,
          21,
          &fld,
          tmp_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SHR_BITCOPY_RANGE(buf, 24, tmp_buf + 3, 0, 32);
  SHR_BITCOPY_RANGE(buf, 0, tmp_buf + 2, 8, 24);
      
  /* parsing the reply - first search */
  res = arad_pp_diag_reply_to_ip_reply_record(
          unit,
          buf,
          ip_reply_record
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  /* second search */
  second_ip_reply_record->match_status = (SHR_BITGET(tmp_buf + 3, 30) != 0);
  if(second_ip_reply_record->match_status) {
    buf[0] = 0;
    SHR_BITCOPY_RANGE(buf, 8, tmp_buf + 2, 0, 8);
    SHR_BITCOPY_RANGE(buf, 0, tmp_buf + 1, 24, 8);
    second_ip_reply_record->dest_id = buf[0] & 0xffff;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_replies()",0,0);
}
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */

void
  ARAD_PP_DIAG_MTR_INFO_print(
	SOC_SAND_IN			int unit,
	SOC_SAND_IN			bcm_policer_t policer_id, 
	SOC_SAND_IN			bcm_policer_config_t *policer_cfg, 
	SOC_SAND_IN			int cbl, 
	SOC_SAND_IN			int ebl)
{
  int
	  display_cbl, display_ebl, cbl_level, ebl_level, i, j, ast_count;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(policer_cfg);


  /*cbl and ebl are in bytes*/
  display_cbl = cbl * 8 / 1000;
  display_ebl = ebl * 8 / 1000;

  cbl_level = ebl_level = 0;
  if (display_cbl > 0 && policer_cfg->ckbits_burst != 0) {
	  cbl_level = ARAD_PP_DIAG_PERCENT(display_cbl, policer_cfg->ckbits_burst);
  }
  if (display_ebl > 0 && policer_cfg->pkbits_burst != 0) {
	  ebl_level = ARAD_PP_DIAG_PERCENT(display_ebl, policer_cfg->pkbits_burst);
  }
  

  LOG_CLI((BSL_META("Meter id: %d\n\r"), policer_id));
  LOG_CLI((BSL_META("Mode: %s\n\r"), SOC_PPD_MTR_MODE_to_string(policer_cfg->mode)));

  LOG_CLI((BSL_META("-------------------------------------------------------------------------\n\r")));
  LOG_CLI((BSL_META("Bucket Name  |Configured Rate(kbps)|Size(kb)          |Bucket Level      \n\r")));
  LOG_CLI((BSL_META("Committed    |")));
  LOG_CLI((BSL_META("%-21d|"), policer_cfg->ckbits_sec));
  LOG_CLI((BSL_META("%-18d|"), policer_cfg->ckbits_burst)); 
  LOG_CLI((BSL_META("%d\n\r"), display_cbl));
  if (policer_cfg->mode != bcmPolicerModeCommitted) {
	  LOG_CLI((BSL_META("Excess       |")));
	  LOG_CLI((BSL_META("%-21d|"), policer_cfg->pkbits_sec));
	  LOG_CLI((BSL_META("%-18d|"), policer_cfg->pkbits_burst)); 
	  LOG_CLI((BSL_META("%d\n\r"), display_ebl));
  }

  /* no excess bucket in committed mode*/
  LOG_CLI((BSL_META("\r\n  CBL     ")));
  if (policer_cfg->mode != bcmPolicerModeCommitted) {
	  LOG_CLI((BSL_META("   EBL     \r\n")));
  }

  else{
	  LOG_CLI((BSL_META("\r\n")));
  }
  for (i = 80; i >= 0; i -= 20) {
	  LOG_CLI((BSL_META("|")));


	  if (cbl_level == 0 || i > cbl_level) {
		  LOG_CLI((BSL_META("     ")));
	  } else if (i < cbl_level - (cbl_level % 20)) {
		  LOG_CLI((BSL_META("*****")));
	  } else {
		  ast_count = (cbl_level % 20) / 5;
		  for (j = 0; j < ast_count; j++) {
			   LOG_CLI((BSL_META("*")));
		  }
		  for (j = 0; j < 5 - ast_count; j++) {
			   LOG_CLI((BSL_META(" ")));
		  }
	  }

	  LOG_CLI((BSL_META("|")));
	  if (policer_cfg->mode != bcmPolicerModeCommitted) {
		  LOG_CLI((BSL_META("     |")));

		  if (ebl_level == 0 || i > ebl_level) {
			  LOG_CLI((BSL_META("     ")));
		  } else if (i < ebl_level - (ebl_level % 20)) {
			  LOG_CLI((BSL_META("*****")));
		  } else {
			  ast_count = (ebl_level % 20) / 5;
			  for (j = 0; j < ast_count; j++) {
				 LOG_CLI((BSL_META("*")));
			  }
			  for (j = 0; j < 5 - ast_count; j++) {
				 LOG_CLI((BSL_META(" ")));
			  }
		  }

		  LOG_CLI((BSL_META("|\r\n")));
	  } else {
		  LOG_CLI((BSL_META("\r\n")));
	  }
  }

  LOG_CLI((BSL_META("-------    ")));
  if (policer_cfg->mode != bcmPolicerModeCommitted) {
	  LOG_CLI((BSL_META(" -------\r\n")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_DIAG_ETH_POLICER_INFO_print(
	SOC_SAND_IN		int 						unit,
	SOC_SAND_IN		bcm_port_t 					port,
	SOC_SAND_IN		SOC_PPD_MTR_BW_PROFILE_INFO *policer_cfg, 
	SOC_SAND_IN		int 						*bucket_lvl,
    SOC_SAND_IN		uint32 						agg_policer_valid,
    SOC_SAND_IN		uint32 						agg_policer_id)
{
	int i, bucket;
	char *dataType_name[5] = {" UNK_UCAST ","   UCAST   "," UNK_MCAST ","   MCAST   ","   BCAST   "};
	char *disable_cir;
	char *packet_mode;
	char *color_blind;
	char *packet_adj;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	SOC_SAND_CHECK_NULL_INPUT(policer_cfg);
	SOC_SAND_CHECK_NULL_INPUT(bucket_lvl);

	LOG_CLI((BSL_META("|--------------------------------------------PORT %3d-------------------------------------------------|\n\r"),port));
	if (agg_policer_valid == 1) {
        LOG_CLI((BSL_META("|                                aggregate policer index = %d                                         |\n\r"),agg_policer_id));
	}
	LOG_CLI((BSL_META("|-------------------------------------------------------------------------------------------------------------------|\n\r")));
	LOG_CLI((BSL_META("| Data Type |   Rate(kbps)  |  Burst(kbps)  |  Bucket(kbps) | Cir Disable | Color Blind | Packet Mode | Packet Adj  |\n\r")));
	LOG_CLI((BSL_META("|-------------------------------------------------------------------------------------------------------------------|\n\r")));
	
	for (i=0;i<SOC_PPC_NOF_MTR_ETH_TYPES;i++) {
		/* bucket level is a 23 bits - 2's compliment expression
		   max positive num: (2^(23-1)-1) = 0x400000-1
		   translation of "positive" to negative : num - 2^23 = num - 0x800000
		 
		   In addition, translation froom Bytes to kbits. dividing by 125
		*/
		bucket = bucket_lvl[i] < 0x400000 ? bucket_lvl[i] : bucket_lvl[i] - 0x800000;
		bucket = bucket/125;
		disable_cir = policer_cfg[i].disable_cir    == 1 ? "TRUE " : "FALSE";
		packet_mode = policer_cfg[i].is_packet_mode == 1 ? "TRUE " : "FALSE";
		color_blind = policer_cfg[i].color_mode     == 0 ? "TRUE " : "FALSE";
		packet_adj = policer_cfg[i].is_pkt_truncate == 1 ? "TRUE " : "FALSE";
		LOG_CLI((BSL_META("|%s|  %11d  |    %7d    |    %7d    |    %s    |    %s    |    %s    |    %s    |\n\r"), 
				 dataType_name[i],policer_cfg[i].cir, policer_cfg[i].cbs, bucket,
				 disable_cir,color_blind,packet_mode,packet_adj));
		LOG_CLI((BSL_META("|-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - - - - - - - - |\n\r")));
	}
	LOG_CLI((BSL_META("|-------------------------------------------------------------------------------------------------------------------|\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_DIAG_AGGREGATE_ETH_POLICER_INFO_print(
	SOC_SAND_IN		int 						unit,
	SOC_SAND_IN		bcm_policer_t 	            policer_id,
	SOC_SAND_IN		int 	                    nom_of_policers,
    SOC_SAND_IN		SOC_PPD_MTR_BW_PROFILE_INFO *policer_cfg, 
	SOC_SAND_IN		int 						*bucket_lvl)
{
	int i, j, bucket;
	char *dataType_name[4][4] = {{" ALL TRAFFIC ",      ""       ,      ""       ,      ""       },
                                 {"  UNK_UC/MC  ","  MC/UC/BC   ",       ""      ,      ""       },
                                 {"  UC/UNK_UC  ","  MC/UNK_MC  ","    BCAST    ",      ""       },
                                 {"   UNK_UC    ","      UC     ","  UNK_UC/MC  ","    BCAST    "}
                                };
	char *disable_cir;
	char *packet_mode;
	char *color_blind;
	char* packet_adj;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	SOC_SAND_CHECK_NULL_INPUT(policer_cfg);
	SOC_SAND_CHECK_NULL_INPUT(bucket_lvl);

    /* first Index of the 2D data type array*/
    i = nom_of_policers - 1;

	LOG_CLI((BSL_META("----------------------------------------------POLICER ID: 0x%x---------------------------------------------------\n\r"),policer_id));
	LOG_CLI((BSL_META("|---------------------------------------------------------------------------------------------------------------------|\n\r")));
	LOG_CLI((BSL_META("|  Data Type  |   Rate(kbps)  |  Burst(kbps)  |  Bucket(kbps) | Cir Disable | Color Blind | Packet Mode | Packet Adj  |\n\r")));
	LOG_CLI((BSL_META("|---------------------------------------------------------------------------------------------------------------------|\n\r")));
	
	for (j=0;j<nom_of_policers;j++) {
		/* bucket level is a 23 bits - 2's compliment expression
		   max positive num: (2^(23-1)-1) = 0x400000-1
		   translation of "positive" to negative : num - 2^23 = num - 0x800000
		 
		   In addition, translation froom Bytes to kbits. dividing by 125
		*/
		bucket = bucket_lvl[j] < 0x400000 ? bucket_lvl[j] : bucket_lvl[j] - 0x800000;
		bucket = bucket/125;
		disable_cir = policer_cfg[j].disable_cir    == 0 ? "FALSE" : "TRUE ";
		packet_mode = policer_cfg[j].is_packet_mode == 0 ? "FALSE" : "TRUE ";
		color_blind = policer_cfg[j].color_mode     == 0 ? "TRUE " : "FALSE";
		packet_adj  = policer_cfg[j].is_pkt_truncate== 0 ? "FALSE" : "TRUE ";
		LOG_CLI((BSL_META("|%s|  %11d  |    %7d    |    %7d    |    %s    |    %s    |    %s    |    %s    |\n\r"), 
				 dataType_name[i][j],policer_cfg[j].cir, policer_cfg[j].cbs, bucket,
				 disable_cir,color_blind,packet_mode,packet_adj));
		LOG_CLI((BSL_META("|-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - - - - - - - -  -  -  -|\n\r")));
	}
	LOG_CLI((BSL_META("|---------------------------------------------------------------------------------------------------------------------|\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*********************************************************************
*     Returns the forwarding lookup performed including:
 *     forwarding type (bridging, routing, ILM, ...), the key
 *     used for the lookup and the result of the lookup
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_frwrd_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_LKUP_INFO           *frwrd_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    dest=0,
    ofst_index=0,
    lbl_index=0,
    frwrd_code=0,
    vrf,
    mac_long[2],
    tmp=0,
    diag_flavor;
  ARAD_PP_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_KEY_ENCODED
    lem_key;
  ARAD_PP_LEM_ACCESS_KEY
    lem_access_key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    lem_payload_str;
  uint32
    lem_payload_buf[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S] = {0};
  uint32 fid_nof_bits;
  uint32  valid;

#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
  uint32
    buf[9];
  uint32
    size_in_bytes;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(frwrd_info);

  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_LKUP_INFO_GET, &valid);                  
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_FRWRD_LKUP_INFO_clear(frwrd_info);


  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);


  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,7,73,65,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,6,219,211,5);
  }
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,5,4,&frwrd_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
   
  /* if raw */
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  diag_flavor = diag_mode.flavor;

  /* if not raw mode get type */
  if ((diag_flavor & ARAD_PP_DIAG_FLAVOR_RAW) == 0)
  {
    frwrd_info->frwrd_type = arad_pp_diag_frwrd_type_to_lkup_type_map(frwrd_code);

  }

  res = soc_sand_bitstream_get_any_field(regs_val,0,3,&ofst_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_bitstream_get_any_field(regs_val,3,2,&lbl_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  frwrd_info->frwrd_hdr_index = (uint8)(ofst_index + lbl_index);

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,0,222,209,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,0,204,193,5);
  }
  vrf = regs_val[0];

  if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC || frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC)
  {    
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
  if(frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC && ARAD_KBP_ENABLE_IPV4_UC) {
    /* gettingh the request */
    res = arad_pp_diag_get_frwrd_type_size(
            arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC),
            &size_in_bytes
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    res = arad_pp_diag_get_request(
            unit,
            core_id,
            size_in_bytes,
            buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* parsing the request */
    frwrd_info->lkup_key.kbp_ipv4_unicast_rpf = arad_pp_diag_request_to_ipv4_unicast_rpf(buf);

    /* getting the reply */
    res = arad_pp_diag_get_replies(
            unit,
            core_id,
            &frwrd_info->ip_reply_record,
            &frwrd_info->second_ip_reply_result
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    frwrd_info->is_kbp = TRUE;
  } else if(frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC && ARAD_KBP_ENABLE_IPV4_MC) {
    /* gettingh the request */
    res = arad_pp_diag_get_frwrd_type_size(
            arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC),
            &size_in_bytes
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

    res = arad_pp_diag_get_request(
            unit,
            core_id,
            size_in_bytes,
            buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    /* parsing the request */
    frwrd_info->lkup_key.kbp_ipv4_multicast = arad_pp_diag_request_to_ipv4_multicast(buf);

    /* getting the reply */
    res = arad_pp_diag_get_replies(
            unit,
            core_id,
            &frwrd_info->ip_reply_record,
            &frwrd_info->second_ip_reply_result
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    frwrd_info->is_kbp = TRUE;
  }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
  }

  if (vrf != 0)
    {
      frwrd_info->frwrd_type = ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN;
    }
  if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC) {
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,1,54,54,5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,1,34,34,5);
      }
      res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      if (tmp == 1) {
          frwrd_info->frwrd_type = ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC;
      }
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC && ARAD_KBP_ENABLE_TRILL_UC) {
        /* gettingh the request */
        res = arad_pp_diag_get_frwrd_type_size(
                arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC),
                &size_in_bytes
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

        res = arad_pp_diag_get_request(
                unit,
                core_id,
                size_in_bytes,
                buf
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        /* parsing the request */
        frwrd_info->lkup_key.kbp_trill_unicast = arad_pp_diag_request_to_trill_unicast(buf);

        /* getting the reply */
        res = arad_pp_diag_get_replies(
                unit,
                core_id,
                &frwrd_info->ip_reply_record,
                &frwrd_info->second_ip_reply_result
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
        frwrd_info->is_kbp = TRUE;
      } else if(frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC && ARAD_KBP_ENABLE_TRILL_MC) {
        /* gettingh the request */
        res = arad_pp_diag_get_frwrd_type_size(
                arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC),
                &size_in_bytes
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

        res = arad_pp_diag_get_request(
                unit,
                core_id,
                size_in_bytes,
                buf
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        /* parsing the request */
        frwrd_info->lkup_key.kbp_trill_multicast = arad_pp_diag_request_to_trill_multicast(buf);

        /* getting the reply */
        res = arad_pp_diag_get_replies(
                unit,
                core_id,
                &frwrd_info->ip_reply_record,
                &frwrd_info->second_ip_reply_result
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
        frwrd_info->is_kbp = TRUE;
      } 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
  }

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,26, 0, 79, 0, 101);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,22, 0 , 116, 43, 101);
  }

  if (diag_flavor & ARAD_PP_DIAG_FLAVOR_RAW)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,74,frwrd_info->lkup_key.raw);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT)
  {
    frwrd_info->lkup_key.mact.key_type = ARAD_PP_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    mac_long[0] = 0;
    mac_long[1] = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,0,48,mac_long);
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
    /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    res = soc_sand_pp_mac_address_long_to_struct(
            mac_long,
            &(frwrd_info->lkup_key.mact.key_val.mac.mac)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    /* vsi */
    tmp = 0;
    fid_nof_bits = SOC_DPP_DEFS_GET(unit, fid_nof_bits);
    res = soc_sand_bitstream_get_any_field(regs_val,48,fid_nof_bits,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    frwrd_info->lkup_key.mact.key_val.mac.fid = tmp;
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC || frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,32,&frwrd_info->lkup_key.ipv4_uc.key.subnet.ip_address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    frwrd_info->lkup_key.ipv4_uc.key.subnet.prefix_len = 32;
    res = soc_sand_bitstream_get_any_field(regs_val,32,12,&frwrd_info->lkup_key.ipv4_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC)
  {
      
    ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,15,1,235,76,5);
    res = soc_sand_bitstream_get_any_field(regs_val,0,28,&frwrd_info->lkup_key.ipv4_mc.group);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    frwrd_info->lkup_key.ipv4_mc.group |= 0xe << 28;
    res = soc_sand_bitstream_get_any_field(regs_val,28,32,&frwrd_info->lkup_key.ipv4_mc.source.ip_address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
    frwrd_info->lkup_key.ipv4_mc.source.prefix_len = 32;
    res = soc_sand_bitstream_get_any_field(regs_val,60,12,&frwrd_info->lkup_key.ipv4_mc.inrif);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    frwrd_info->lkup_key.ipv4_mc.inrif_valid = TRUE;
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC || frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_VPN)
  {
    
    ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,15,1,235,76,5);
    res = soc_sand_bitstream_get_any_field(regs_val,0,128,frwrd_info->lkup_key.ipv6_uc.key.subnet.ipv6_address.address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
    frwrd_info->lkup_key.ipv6_uc.key.subnet.prefix_len = 132;
    res = soc_sand_bitstream_get_any_field(regs_val,128,12,&frwrd_info->lkup_key.ipv6_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
    if(frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC && ARAD_KBP_ENABLE_IPV6_UC) {
      /* gettingh the request */
      res = arad_pp_diag_get_frwrd_type_size(
              arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC),
              &size_in_bytes
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);

      res = arad_pp_diag_get_request(
              unit,
              core_id,
              size_in_bytes,
              buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

      /* parsing the request */
      frwrd_info->lkup_key.kbp_ipv6_unicast_rpf = arad_pp_diag_request_to_ipv6_unicast_rpf(buf);

      /* getting the reply */
      res = arad_pp_diag_get_replies(
              unit,
              core_id,
              &frwrd_info->ip_reply_record,
              &frwrd_info->second_ip_reply_result
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
      frwrd_info->is_kbp = TRUE;
    }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC)
  {
      
    ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,15,1,235,76,5);
    res = soc_sand_bitstream_get_any_field(regs_val,0,120,frwrd_info->lkup_key.ipv6_mc.group.address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
    /* set MSB to be 0xFF */
    tmp = 0xFF;
    res = soc_sand_bitstream_set_any_field(&tmp,120,8,frwrd_info->lkup_key.ipv6_uc.key.subnet.ipv6_address.address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
    /* in-RIF*/
    res = soc_sand_bitstream_get_any_field(regs_val,128,12,&frwrd_info->lkup_key.ipv6_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
    if(ARAD_KBP_ENABLE_IPV6_MC) {
      /* gettingh the request */
      res = arad_pp_diag_get_frwrd_type_size(
              arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC),
              &size_in_bytes
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 275, exit);

      res = arad_pp_diag_get_request(
              unit,
              core_id,
              size_in_bytes,
              buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

      /* parsing the request */
      frwrd_info->lkup_key.kbp_ipv6_multicast = arad_pp_diag_request_to_ipv6_multicast(buf);

      /* getting the reply */
      res = arad_pp_diag_get_replies(
              unit,
              core_id,
              &frwrd_info->ip_reply_record,
              &frwrd_info->second_ip_reply_result
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
      frwrd_info->is_kbp = TRUE;
    }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_ILM)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.in_label);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
    res = soc_sand_bitstream_get_any_field(regs_val,ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM,ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

    frwrd_info->lkup_key.ilm.mapped_exp =(uint8)tmp;

    tmp = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM+ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
    if (SOC_IS_JERICHO(unit)) {
        /* Core is part of the key */
        uint32 port_and_core;

        res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM_JERICHO,&port_and_core);
        SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

        frwrd_info->lkup_key.ilm.in_local_port = port_and_core & ((1 << ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM) -1);
        frwrd_info->lkup_key.ilm.in_core = (port_and_core >> ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM);

        tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM_JERICHO;
    }
    else {
        res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.in_local_port);
        SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

        tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
    }
    res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.inrif);
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
    tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
    if(ARAD_KBP_ENABLE_MPLS) {
      /* gettingh the request */
      res = arad_pp_diag_get_frwrd_type_size(
              arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(ARAD_PP_DIAG_FWD_LKUP_TYPE_ILM),
              &size_in_bytes
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 335, exit);

      res = arad_pp_diag_get_request(
              unit,
              core_id,
              size_in_bytes,
              buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);

      /* parsing the request */
      frwrd_info->lkup_key.kbp_mpls = arad_pp_diag_request_to_mpls(buf);

      /* getting the reply */
      res = arad_pp_diag_get_replies(
              unit,
              core_id,
              &frwrd_info->ip_reply_record,
              &frwrd_info->second_ip_reply_result
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);
      frwrd_info->is_kbp = TRUE;
    }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&frwrd_info->lkup_key.trill_uc);
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,1,54,54,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,1,34,34,5);
    }
    res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);


  }
  else if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&frwrd_info->lkup_key.trill_mc.tree_nick);
    SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);

    res = soc_sand_bitstream_get_any_field(regs_val,17,15,&frwrd_info->lkup_key.trill_mc.fid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);

  }

  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF)
  {

    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&lem_access_key);

    sal_memcpy(&lem_key.buffer,regs_val,sizeof(uint32)*SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));

    res = arad_pp_lem_key_encoded_parse(unit,&lem_key,&lem_access_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);

    arad_pp_fcf_route_key_from_lem_key(&lem_access_key,&frwrd_info->lkup_key.fcf);
  }

  
  /* if raw */
  if (diag_flavor & ARAD_PP_DIAG_FLAVOR_RAW)
  {
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,7,0,44,0, 5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,21,0,45,1,5); 
      }
    res = soc_sand_bitstream_get_any_field(regs_val,0,SOC_IS_JERICHO(unit)? 45: 43,frwrd_info->lkup_res.raw);
    SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);
  }
  else if (
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC ||
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_VPN ||
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC ||
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC  ||
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_FCF
          )
  {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,16,0,125,85,5);
        tmp = 0;
        res = soc_sand_bitstream_get_any_field(regs_val,40,1,&tmp);
        SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);
        if (tmp)
        {
          frwrd_info->key_found = TRUE;

          if (ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC||ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC)
          {
            res = soc_sand_bitstream_get_any_field(regs_val,0,SOC_DPP_DEFS_GET(unit, fec_nof_bits),&dest);
            SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);
            payload.asd = 0;

            if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT || frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_BMACT)
            {
              res = arad_pp_diag_fwd_decision_in_buffer_parse(
                      unit,
                      ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                      ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
                      dest,
                      payload.asd,
                      &(frwrd_info->lkup_res.mact.frwrd_info.forward_decision)
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);
            }
          }
          else
          {
            frwrd_info->lkup_res.frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_FEC;
            frwrd_info->lkup_res.frwrd_decision.additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY;
            res = soc_sand_bitstream_get_any_field(regs_val, 0, SOC_DPP_DEFS_GET(unit, fec_nof_bits), &frwrd_info->lkup_res.frwrd_decision.dest_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);
          }
        }
        else
        {
          frwrd_info->key_found = FALSE;
        }
      }
  }
    /* lookup in LPM and result is FEC */
  else if (
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC ||
            frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN
          )
  {
    /* check maybe host has the answer */

    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,7,0,46,0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,90,0,5);
    }
    /*was match*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 46: 45, 1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);
    if (tmp)
    {
      frwrd_info->key_found = tmp;
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              0, /* Not clear why not 2 in Arad */
              SOC_IS_JERICHO(unit)? 45: 43,
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 470, exit);

      res = arad_pp_lem_access_payload_parse(
              unit,
              ARAD_PP_LEM_ENTRY_TYPE_UNKNOWN,
              lem_payload_buf,
              &lem_payload_str
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 480, exit);

      res = arad_pp_frwrd_em_dest_to_fec(
              unit,
              &lem_payload_str,
              &(frwrd_info->lkup_res.host_info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);

    }
    else
    {
        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /*lpm2flp_second_payload(PB) -> lpm2flp_payload(ARAD)*/
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,14,0,15,1,5); 
          /*was match*/
          frwrd_info->key_found = TRUE;
          frwrd_info->lkup_res.frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_FEC;
          frwrd_info->lkup_res.frwrd_decision.additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY;
          frwrd_info->lkup_res.frwrd_decision.dest_id = regs_val[0];
        }
    }
  }
  /* lookup in LEM*/
  else
  {


    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,47,0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,45,0,5); 
    }
    /*was match*/
    tmp = 0;

    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 47: 45,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);
    if (tmp)
    {
      frwrd_info->key_found = TRUE;

      if (SOC_IS_JERICHO(unit)) {
          res = soc_sand_bitstream_get_any_field(regs_val, 0, 45,lem_payload_buf);
      }
      else {
          res = soc_sand_bitstream_get_any_field(regs_val, 0, 43,lem_payload_buf);
      }
      SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);

      res = arad_pp_lem_access_payload_parse(
              unit, 
              ARAD_PP_LEM_ENTRY_TYPE_UNKNOWN,
              lem_payload_buf, 
              &payload
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 505, exit);

      if (frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT || frwrd_info->frwrd_type == ARAD_PP_DIAG_FWD_LKUP_TYPE_BMACT)
      {
        res = arad_pp_fwd_decision_in_buffer_parse(
                unit,
                ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                payload.dest,
                payload.asd,
                payload.flags,
                &(frwrd_info->lkup_res.mact.frwrd_info.forward_decision)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 530, exit);
      }
      else
      {
        res = arad_pp_diag_fwd_decision_in_buffer_parse(
                unit,
                ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
                payload.dest,
                payload.asd,
                &(frwrd_info->lkup_res.frwrd_decision)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 540, exit);
      }
    }
    else
    {
      frwrd_info->key_found = FALSE;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lkup_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_frwrd_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LKUP_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lkup_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns information regard packet trapping/snooping,
 *     including which traps/snoops were fulfilled, which
 *     trap/snoop was committed, and whether packet was
 *     forwarded/processed according to trap or according to
 *     normal packet processing flow.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_traps_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_TRAPS_INFO                *traps_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  )
{
  uint32
    regs_val[3],
    tmp_regs_val[3],
    reg_val,
    is_trapped,
    tmp;
  ARAD_PP_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;
  uint64
    reg64_val;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TRAPS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  *ret_val = SOC_PPC_DIAG_OK; 
 
  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_TRAPS_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_TRAPS_INFO_clear(traps_info);
  
  /* LLR traps */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_LLR_TRAP_0r(unit, core_id, &regs_val[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_LLR_TRAP_1r(unit, core_id, &regs_val[1]));

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          0,
          42,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* VTT traps */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_VTT_TRAPS_ENCOUNTEREDr(unit, core_id, &reg64_val));
  regs_val[0] = COMPILER_64_LO(reg64_val);
  regs_val[1] = COMPILER_64_HI(reg64_val);

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          1,
          55,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* FLP traps */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_FLP_TRAP_0r(unit, core_id, &tmp_regs_val[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_FLP_TRAP_1r(unit, core_id, &tmp_regs_val[1]));

  regs_val[0] = tmp_regs_val[0] | ((tmp_regs_val[1] & 0x3FFF) << 18);
  regs_val[1] = (tmp_regs_val[1] & 0xFFFFC000) >> 14;

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          2,
          44,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  /* FER traps */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_DBG_FER_TRAPr(unit, core_id, &regs_val[0]));

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          3,
          8,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

 /*
  * committed Trap
  */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_DBG_LAST_RESOLVED_TRAPr(unit, core_id, &reg_val));
  
  /* is valid */
  tmp = 0;
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_VALIDf);
  traps_info->committed_trap.is_pkt_trapped = SOC_SAND_NUM2BOOL(tmp);

  /* get trap Code */
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODEf);
  is_trapped = arad_pp_diag_is_trap_code(tmp);

  traps_info->committed_trap.is_pkt_trapped = traps_info->committed_trap.is_pkt_trapped && is_trapped;

  if(traps_info->committed_trap.is_pkt_trapped) {

      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, tmp, &traps_info->committed_trap.code);
      res = arad_pp_trap_frwrd_profile_info_get_unsafe(
              unit,
              traps_info->committed_trap.code,
              &(traps_info->committed_trap.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }

  /* update trap strength according HW val */
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODE_STRENGTHf);
  traps_info->committed_trap.info.strength = tmp;

  /* get snoop Code */
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_SNOOP_CODEf);
  arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, tmp, &traps_info->committed_snoop.code);
  res = arad_pp_trap_snoop_profile_info_get_unsafe(
          unit,
          traps_info->committed_snoop.code,
          &(traps_info->committed_snoop.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  /* update snoop strength according HW val */
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_SNOOP_CODE_STRENGTHf);
  traps_info->committed_snoop.info.strength = tmp;

 /*
  * committed Trap changed destination
  */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr(unit, core_id, &reg_val));
  
  /* is valid */
  tmp = 0;
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr, reg_val, DBG_LAST_TRAP_CHANGE_DESTINATION_VALIDf);
  traps_info->trap_updated_dest.is_pkt_trapped = SOC_SAND_NUM2BOOL(tmp);

  /* get trap Code */
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr, reg_val, DBG_LAST_TRAP_CHANGE_DESTINATIONf);
  is_trapped = arad_pp_diag_is_trap_code(tmp);
  traps_info->trap_updated_dest.is_pkt_trapped = traps_info->trap_updated_dest.is_pkt_trapped && is_trapped;

  if(traps_info->trap_updated_dest.is_pkt_trapped) {
      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, tmp, &traps_info->trap_updated_dest.code);
      res = arad_pp_trap_frwrd_profile_info_get_unsafe(
              unit,
              traps_info->trap_updated_dest.code,
              &(traps_info->trap_updated_dest.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  }

  /* clear if this is the mode */
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  if (diag_mode.flavor & ARAD_PP_DIAG_FLAVOR_CLEAR_ON_GET)
  {
    res = arad_pp_diag_pkt_trace_clear_unsafe(
            unit,
            core_id,
            ARAD_PP_DIAG_PKT_TRACE_TRAP
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_traps_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_traps_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TRAPS_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_traps_info_get_verify()", 0, 0);
}
/*********************************************************************
*     Return the trace (changes) for forwarding decision for
 *     last packet in several phases in processing
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_frwrd_decision_trace_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    dest,
    trap,
    fld_val,
    reg_val,
    eei=0;
  ARAD_PP_TRAP_CODE
    trap_code;
  uint32
    res = SOC_SAND_OK;
  uint32 tmp, valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(frwrd_trace_info);

  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO_clear(frwrd_trace_info);
  
  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET, &valid);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                          
  if (!valid) {        
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                       
      SOC_SAND_EXIT_NO_ERROR;
  }   

  /* port trap */

  /*dbg: llrvtt_4eq5_snoop_strength [1 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 2, 0, 55, 54, 101);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 3, 0 , 42, 41, 101);
  }
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.snoop_action_strength = regs_val[0];

  /*dbg: llrvtt_4eq5_forwarding_action_strength [2 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 2, 0, 120, 118, 102);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 3, 0, 105, 103, 102);
  }
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.frwrd_action_strength = regs_val[0];
  
  /*dbg: llrvtt_4eq5_forwarding_action_cpu_trap_code [7 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2, 0 , 146,  139,103);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3, 0 , 131,  124,103);
  }
  arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,regs_val[0],&trap_code);
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = trap_code;

  /*llrvtt_4eq5_forwarding_action_cpu_trap_qual [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2, 0 , 138,  123,104);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3, 0 , 123,  108,104);
  }
  frwrd_trace_info->trap_qual[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT] = regs_val[0];


  /* LIF trap */
  
  /*dbg: ihp2ihb_vtt2flp_2eq3_snoop_strength [1 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3, 1 , 74, 73, 105);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 0 , 23 , 22,105);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 1 , 54 , 53,105);
  }
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.snoop_action_strength = regs_val[0];
  
  /*dbg: ihp2ihb_vtt2flp_2eq3_forwarding_action_strength [2 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3, 2, 67, 65, 106);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 0 , 113 , 111,106);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 2 , 16 , 14,106);
  }
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.frwrd_action_strength = regs_val[0];

  /*dbg: ihp2ihb_vtt2flp_2eq3_forwarding_action_cpu_trap_code [7 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3, 2, 110, 103, 107);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 0, 156, 149, 107);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 2, 59, 52, 107);
  }
  arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,regs_val[0],&trap_code);
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.trap_code = trap_code;

  /*dbg: ihp2ihb_vtt2flp_2eq3_forwarding_action_cpu_trap_qual [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3, 2 , 102, 87, 108);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 0 , 148, 133,108);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4, 2 , 51, 36,108);
  }
  frwrd_trace_info->trap_qual[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF] = regs_val[0];


  /*LIF forwarding decision*/

  /*dbg: ihp2ihb_vtt2flp_2eq3_forwarding_action_destination [18 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,  3, 2, 86, 68, 102);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,  4, 0, 132, 114, 102);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 2, 35, 17, 109);
  }
   dest = regs_val[0];
   /*dbg: ihp2ihb_vtt2flp_2eq3_eei [23 : 0] */
   if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 3, 2 , 135, 112, 110);
   } else if (SOC_IS_ARDON(unit)) {
       regs_val[0] = 0;
   } else {
       ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 2 , 83 , 60,110);
   }
   eei = regs_val[0];

   res = arad_pp_diag_fwd_decision_in_buffer_parse(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          dest,
          eei,
          &(frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  /*dbg: ihp2ihb_vtt2flp_2eq3_out_lif [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 3, 1, 122, 105, 111);
  } else if (SOC_IS_ARDON(unit)) {
      regs_val[0] = 0;
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 1, 157, 142, 111);
  }
  frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF].outlif = regs_val[0];
  
 /* forwarding lookup TRAP */
      
  /*dbg: flp2pmf_2eq3_snoop_strength [1 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 1, 251, 250, 112);
  } else if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 1 , 34, 33, 1121);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 1 , 172, 171, 1121);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 1 , 34, 33,112);
  }
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.snoop_action_strength = regs_val[0];

  /*dbg: flp2pmf_2eq3_forwarding_action_strength [2 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 8, 104, 102, 113);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 6 , 229, 227,1131);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 161, 159,1131);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 12, 10,113);
  }
  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = regs_val[0];

  /*dbg: flp2pmf_2eq3_forwarding_action_cpu_trap_code [7 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 8, 151, 144, 114);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 20, 13,1141);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 208, 201,1141);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 59, 52,114);
  }
  if(arad_pp_diag_is_trap_code(regs_val[0])){
      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,regs_val[0],&trap_code);
      frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = trap_code;
  }
  else{
    if(frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength > 0)
    {
      frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    }
  }

  /*dbg: flp2pmf_2eq3_forwarding_action_cpu_trap_qual [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 8, 143, 128, 115);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,  0, 7 , 12,  0,1151);
          tmp = regs_val[0] << 3;
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,  0, 6 , 255,  253,1151);
          regs_val[0] += tmp;
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 200, 185,1151);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 51, 36,115);
  }
  frwrd_trace_info->trap_qual[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND] = regs_val[0];


 /* forwarding lookup forwarding decision*/

  /*dbg: flp2pmf_2eq3_forwarding_action_destination [18 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 8, 127, 109, 116);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 6 , 252, 234, 1161);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 184, 166, 1161);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 35, 17,116);
  }
  dest = regs_val[0];

  /*dbg: flp2pmf_2eq3_eei [23 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 9, 192, 169, 117);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7, 189, 166, 1171);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 8, 121, 98, 1171);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7 , 228, 205,117);
  }
  eei = regs_val[0];

  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  /*dbg: flp2pmf_1eq2_out_lif [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,  4, 10, 8,  0, 1181);
      tmp = regs_val[0] << (255-247+1);
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,  4, 9, 255,  247, 1182);
      regs_val[0] += tmp;
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 7, 255, 240, 1181);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 8, 191, 176, 1181);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0, 8 , 38, 23,118);
  }
  frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].outlif = regs_val[0];

  /*PMF forwarding decision*/

  /*dbg: pmf2fer_38eq39_forwarding_action [21 : 3] Destination in forwarding action */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,  0, 11, 6, 0, 1191);
      tmp = regs_val[0] << (255 - (241+3) +1);
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,  0, 10, 255,  241+3, 1192);
      regs_val[0] += tmp;
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,2, 10 , 192, 174, 1191);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,9, 10 , 227, 209, 1191);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,9, 10 , 194, 176,119);
  }
  dest = regs_val[0];

  /*dbg: pmf2fer_38eq39_eei [23 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 0, 11, 161, 138, 120);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,2, 11 , 69, 46, 1201);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,9, 11 , 120, 97, 1201);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,9, 11 , 71, 48,120);
  }
  eei = regs_val[0];
  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  /*dbg: pmf2fer_38eq39_out_lif [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 0, 9, 219, 202, 121);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,2, 9 , 181, 166, 1211);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,9, 9 , 198, 183, 1211);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,9, 9 , 181, 166,121);
  }

  frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_PMF].outlif = regs_val[0];

  /* Trap forwarding decision */

  /* dbg: fer2lbp_eq2_forwarding_action_destination [21 : 3] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 2, 158+21, 158+3, 122);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,3, 2 , 120, 102, 1221);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 2 , 154, 136, 1221);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 2 , 120, 102,122);
  }

  dest = regs_val[0];

  /*dbg: fer2lbp_eq2_eei [23 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 3, 86, 63, 123);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 3, 2 , 253, 230,1231);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 3, 48, 25, 1231);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 2 , 253, 230,123);
  }
  eei = regs_val[0];

  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  /*dbg: fer2lbp_eq2_out_lif [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 1, 62, 45, 124);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit))
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,3, 1 , 15, 30, 1241);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 48, 33, 1241);
      }
  }
  else
  {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 30, 15,124);
  }
  frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_TRAP].outlif = regs_val[0];

  /*FEC forwarding decision*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_IHB_DBG_LAST_RESOLVED_TRAPr(unit, core_id, &reg_val));

  trap = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODEf);

  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength = 
      soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODE_STRENGTHf);

  if(arad_pp_diag_is_trap_code(trap)){
      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,trap,&trap_code);
      frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.trap_code = trap_code;
  }
  else{
    if(frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength > 0)
    {
      frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.trap_code = ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    }
  }


  frwrd_trace_info->trap[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.snoop_action_strength = 
      soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_SNOOP_CODE_STRENGTHf);

  fld_val = 
      soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_VALIDf);

  frwrd_trace_info->trap_qual[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC] = fld_val;
  
  /* last_resolved Destination */
  /*dbg: lbp2dbg_ire_eq_tm_cmd [147 : 130] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 4, 0, 150, 132, 125);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,5, 0 , 147, 130,1251);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,12, 0 , 147, 130,1251);
  }
  dest = regs_val[0];
  /* fer2lbp_eq2_eei [23 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 3, 86, 63, 125);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 3, 2, 253, 230, 125);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 10, 3, 48, 25, 125);
      }
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 2 , 253, 230,125);
  }

  eei = regs_val[0];
  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  /*dbg: fer2lbp_eq2_out_lif [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 1, 62, 45, 124);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,3, 1 , 30, 15, 1241);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 48, 33, 1241);
      }
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 30, 15,124);
  }
  frwrd_trace_info->frwrd[ARAD_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].outlif = regs_val[0];;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_decision_trace_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_frwrd_decision_trace_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_decision_trace_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the information to be learned for the incoming
 *     packet. This is the information that the processing
 *     determine to be learned, the MACT supposed to learn this
 *     information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_learning_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_LEARN_INFO              *learn_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                  *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    lem_payload;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    lem_key;
  ARAD_PP_LEM_ACCESS_KEY
    lem_access_key;
  uint32
    res = SOC_SAND_OK;
  uint32  valid;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_LEARNING_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(learn_info);

  *ret_val = SOC_PPC_DIAG_OK;   

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_LEARNING_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                          
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_LEARN_INFO_clear(learn_info);
  


  
  /* learn ingress/egress*/

  /*dbg: fer2lbp_eq2_ingress_learn_enable [0 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 1, 194, 194, 101);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 177, 177,1011);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 147, 147,101);
  }

  if (regs_val[0])
  {
    learn_info->ingress = TRUE;
  }

  /*dbg: fer2lbp_eq2_egress_learn_enable [0 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 3, 62, 62, 101);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 3, 24, 24,1012);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 2 , 229, 229,101);
  }

  tmp = regs_val[0];
  /* if ingress or egress learning, then this information is relevant
   * otherwise it not relevant and leave */
  if (tmp || learn_info->ingress)
  {
    learn_info->valid = TRUE;
  }
  else
  {
    goto exit;
  }

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&lem_access_key);


  /*dbg: fer2lbp_eq2_learn_or_transplant [0 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 1, 95, 95, 101);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 78, 78,1013);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 48, 48,101);
  }
  learn_info->is_new_key = !regs_val[0];

  /* payload */

  /*dbg: fer2lbp_eq_learn_data [39 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 0, 59, 20, 101);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 10, 0 , 59, 20,101);
  }

  /* buffer to payload */
  res = arad_pp_lem_access_payload_parse(unit,ARAD_PP_LEM_ENTRY_TYPE_UNKNOWN,regs_val,&lem_payload);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* payload to MACT info */
  res = arad_pp_frwrd_mact_payload_convert(unit,&lem_payload,&learn_info->value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  /* key */
  /*dbg: fer2lbp_eq2_learn_key [62 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 1, 158, 96, 101);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 141, 79,1014);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10, 1 , 111, 49,101);
  }

  sal_memcpy(&lem_key.buffer,regs_val,sizeof(uint32)*SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));

  res = arad_pp_lem_key_encoded_parse(unit,&lem_key,&lem_access_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  res = arad_pp_frwrd_mact_key_parse(unit,&lem_access_key,&learn_info->key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_learning_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_learning_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_LEARNING_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_learning_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the result of ingress vlan editing,
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_ing_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_VLAN_EDIT_RES           *vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                  *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  uint32
    res = SOC_SAND_OK;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vec_res);

  *ret_val = SOC_PPC_DIAG_OK;    

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET, &valid);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                         
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_VLAN_EDIT_RES_clear(vec_res);

  /* vlan-edit-info*/
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,2,0,149,116,5);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, IHP_VTT_GENERAL_CONFIGS_0r, SOC_CORE_ALL, 0, FORCE_VEC_FROM_LIFf, &vec_res->adv_mode));
  }
  else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10,0,145,112,5);
  }

  /* AC-tag info*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,12,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->ac_tag.vid = (SOC_SAND_PP_VLAN_ID)tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,24,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  vec_res->ac_tag.dei = (SOC_SAND_PP_DEI_CFI)tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,25,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  vec_res->ac_tag.pcp = (SOC_SAND_PP_PCP_UP)tmp;

  /* AC-tag2 info*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,12,12,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  vec_res->ac_tag2.vid = (SOC_SAND_PP_VLAN_ID)tmp;

  /* edit id*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,28,6,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  vec_res->cmd_id = tmp;
  
  res = arad_pp_lif_ing_vlan_edit_command_info_get_unsafe(
          unit,
          vec_res->cmd_id,
          &vec_res->cmd_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_ing_vlan_edit_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_ing_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_ing_vlan_edit_info_get_verify()", 0, 0);
}

/*********************************************************************
*     The Traffic management information associated with the
 *     packet including meter, DP, TC, etc...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_pkt_associated_tm_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_PKT_TM_INFO             *pkt_tm_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                  *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
	regs_val_part2[ARAD_PP_DIAG_DBG_VAL_LEN],
    dest,
    type=0,
    dest_id=0,
    sys_logic_port_id,
    lag_id,
    lag_member_id,
    sys_phys_port_id,
	counter2_part1=0,
    counter2_part2=0,
    tmp=0;
  uint32
    res = SOC_SAND_OK;
  uint8 is_lag_not_phys;
  uint32  valid;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pkt_tm_info);

  *ret_val = SOC_PPC_DIAG_OK;                                                           

 
  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                           
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_PKT_TM_INFO_clear(pkt_tm_info);

  /* tm-forwarding-info*/
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,4,1,66,0,5);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,5,1,53,0,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,12,1,53,0,5);
  }
  sal_memcpy(regs_val_part2, regs_val, sizeof(regs_val));
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,4,0,255,0,5);
  } else if (SOC_IS_ARDON(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,5,0,255,0,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,12,0,255,0,5);
  }

  /* destination*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 118: 116,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp)
  {
      pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_DEST;
      /* parse dest*/
      /* 
       * Convert the Destination in the TM-Command to the Forward decision. 
       * Encoding: 
       * ARAD: 
       *    All ones = drop 
       *    Bit 17=1 -> ING_DESTINATION_FLOW_ID 
       *         16:0 = Flow-ID 
       *    Bit 16:17=01 -> ING_DESTINATION_MULTICAST_ID 
       *         15:0 = Multicast Flow-ID
       *    Bit 16:17=00 -> destination system port
       * JERICHO: 
      *    All ones = drop 
      *    Bit 18:17=11 -> ING_DESTINATION_FLOW_ID 
      *         16:0 = Flow-ID 
      *    Bit 18:17=10 -> EG/FABRIC_DESTINATION_MULTICAST_ID 
      *         16:0 = Multicast Flow-ID
      *    Bit 18:17=01 -> ING_DESTINATION_MULTICAST_ID 
      *         16:0 = Multicast Flow-ID
      *    Bit 18:17=001 -> destination system port
       */
      dest = 0;
      /* Destination located in 147:130 in Arad TM command, and in 150:132 In Jericho */
      res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 132: 130, SOC_IS_JERICHO(unit)? 19: 18, &dest);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (dest==ARAD_PP_DIAG_FRWRD_DECISION_DEST_DROP_CODE) {
          LOG_INFO(BSL_LS_SOC_DIAG,
                   (BSL_META_U(unit,
                               "0")));
          pkt_tm_info->frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_DROP;
      } else {
          type = 0;
          res = soc_sand_bitstream_get_any_field(&dest, 17, SOC_IS_JERICHO(unit)? 2: 1, &type);
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          if (type == ARAD_PP_DIAG_FRWRD_DECISION_TYPE_UC_FLOW){
              pkt_tm_info->frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_UC_FLOW;
              res = soc_sand_bitstream_get_any_field(&dest, 0, 17, &dest_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          } else {;
              type=0;
              res = soc_sand_bitstream_get_any_field(&dest, SOC_IS_JERICHO(unit)? 17: 16, 2, &type);
              SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
              if (type == ARAD_PP_DIAG_FRWRD_DECISION_TYPE_MC) {
                  pkt_tm_info->frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_MC;
                  res = soc_sand_bitstream_get_any_field(&dest,0, SOC_IS_JERICHO(unit)? 17: 16, &dest_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
              } else if (SOC_IS_JERICHO(unit) && (type == ARAD_PP_DIAG_FRWRD_DECISION_TYPE_EGRESS_MC)) {
                  pkt_tm_info->frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_MC;
                  res = soc_sand_bitstream_get_any_field(&dest,0, 17, &dest_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
              } else {
                  /* LAG/Sys-Port */
                  /* No check in Jericho that bit 16 is up */
                  sys_logic_port_id=0;
                  res = soc_sand_bitstream_get_any_field(&dest, 0, 16, &sys_logic_port_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
                  sys_phys_port_id=0;
                  res = arad_ports_logical_sys_id_parse(unit, sys_logic_port_id, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
                  if (is_lag_not_phys){
                      pkt_tm_info->frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG;
                      dest_id = lag_id;
                  } else {
                      pkt_tm_info->frwrd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT;
                      dest_id = sys_phys_port_id;
                  }
              }
          }
      }
      pkt_tm_info->frwrd_decision.dest_id = dest_id;
  }

  /*TC*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_TC;
    /* parse tc*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 129: 127, 3,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    pkt_tm_info->tc = (SOC_SAND_PP_TC)tmp;
  }

  /*DP*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_DP;
    /* parse dp*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 127: 125, 2, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    pkt_tm_info->dp = (SOC_SAND_PP_DP)tmp;
  }

  /* On Arad, Meter PTR 0 is always invalid, unless specified otherwise by SOC property. */
  /* The case where meter PTR 0 is not invalid is ignored. */
  /* On Jericho, a valid bit was added */
  /*METER0*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit) ? 204 : 196, SOC_IS_JERICHO(unit) ? 1 : SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &tmp); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_METER1;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 187: 196, SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &pkt_tm_info->meter1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  /*METER1*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 222: 211, SOC_IS_JERICHO(unit) ? 1 : SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_METER2;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 205: 211, SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &pkt_tm_info->meter2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }
  /*METER-command*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_MTR_CMD;
    res = soc_sand_bitstream_get_any_field(regs_val_part2, SOC_IS_JERICHO(unit)? 27: 42, 2, &pkt_tm_info->dp_meter_cmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  }
  /*COUNTER1*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 244: 242, 1, &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_COUNTER1;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 223: 226, SOC_IS_JERICHO(unit)? 21: 16, &pkt_tm_info->counter1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  }
  /*COUNTER2*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val_part2, SOC_IS_JERICHO(unit)? 10: 3, 1, &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_COUNTER2;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 245: 243, SOC_IS_JERICHO(unit)? 11: 13, &counter2_part1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    res = soc_sand_bitstream_get_any_field(regs_val_part2, 0, SOC_IS_JERICHO(unit)? 10: 3, &counter2_part2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);
	pkt_tm_info->counter2 = counter2_part1 + (counter2_part2 << (SOC_IS_JERICHO(unit)? 11: 13));
  }

  /* Ethernet Meter Pointer */
  tmp = 1;  /* always valid */
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_ETH_METER_PTR;
    res = soc_sand_bitstream_get_any_field(regs_val,0,11,&pkt_tm_info->eth_meter_ptr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  }
  /* Ingress Shaping Destination */
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 151: 148, 1, &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_ING_SHAPING_DA;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 152: 149, 17, &pkt_tm_info->ingress_shaping_da);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  }
  /* ECN Capable */
  tmp = 1;  /* always valid */
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_ECN_CAPABLE;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 169: 166, 1, &pkt_tm_info->ecn_capable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  }
  /* CNI */
  tmp = 1;  /* always valid */
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_CNI;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 170: 167, 1, &pkt_tm_info->cni);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
  }
  /* DA Type - before Jericho */
  res = soc_sand_bitstream_get_any_field(regs_val,168,2,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  if (SOC_IS_ARADPLUS_AND_BELOW(unit) && tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_DA_TYPE;
    switch (tmp) {
    case 1:
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
      break;
    case 2:
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC;
      break;
    case 3:
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 235, exit);
      break;
    }
  }
  /* ST VSQ pointer */
  tmp = 1;  /* always valid */
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_ST_VSQ_PTR;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 179: 188, 8, &pkt_tm_info->st_vsq_ptr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  }
  /* LAG LB Key */
  tmp = 1;  /* always valid */
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_LAG_LB_KEY;
    res = soc_sand_bitstream_get_any_field(regs_val_part2, SOC_IS_JERICHO(unit)? 11: 26, 16, &pkt_tm_info->lag_lb_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  }
  /* Ignore Congestion Point */
  tmp = 1;  /* always valid */
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_IGNORE_CP;
    res = soc_sand_bitstream_get_any_field(regs_val_part2, SOC_IS_JERICHO(unit)? 29: 44, 1, &pkt_tm_info->ignore_cp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
  }
  /*CUD*/
  /*dbg: fer2lbp_eq2_out_lif [15 : 0] */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,2,1,62,45,1751);
  } else  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARDON(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,3,1,30,15,1751);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10,1,48,33,1751);
      }
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,10,1,30,15,175);
  }
  tmp = 1; /*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_CUD;
    res = soc_sand_bitstream_get_any_field(regs_val,0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &pkt_tm_info->cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
  }
  /*Snoop id*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= ARAD_PP_DIAG_PKT_TM_FIELD_SNOOP_ID;
    /* parse dp*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 119: 117, 4, &pkt_tm_info->snoop_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_associated_tm_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_pkt_associated_tm_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_associated_tm_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the encapsulation and editing information applied to
 *     last packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_diag_encap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_ENCAP_INFO             *encap_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                 *ret_val
  )
{
  ARAD_PP_DIAG_REG_FIELD
      fld;
  uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
      tmp=0,
      otm_port,
      tmp2=0;
  uint32
      res = SOC_SAND_OK;
  uint32
      curr_eep_index = 0;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
      cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  uint32
      base_q_pair,
      nof_priorities,
      found,
      flags;
  soc_pbmp_t 
      pbmp;
  soc_port_t
      port_i;
  int
      core;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ENCAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);
  *ret_val = SOC_PPC_DIAG_OK;                                                           


  ARAD_PP_DIAG_ENCAP_INFO_clear(encap_info);

  /* 
   * Encapsulation information. 
   */
  
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 2, SOC_IS_JERICHO(unit)? 255: 104, SOC_IS_JERICHO(unit)? 228: 76, 5);

  tmp = 0;
  /* action0_valid*/
  res = soc_sand_bitstream_get_any_field(regs_val, 0, 1, &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp)
  {
    /* PPH EEI extension (24bits) Command(4), Data(20) */

    /* Command (4) */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, 21, 4, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (tmp <= 7) {
       /* MPLS command */
       encap_info->mpls_cmd.command = ARAD_PP_MPLS_COMMAND_TYPE_PUSH;
       encap_info->mpls_cmd.push_profile = tmp;
       /* Data = label */
       soc_sand_bitstream_get_any_field(regs_val, 1, 20, &encap_info->mpls_cmd.label); 
    } else if (tmp == 10) {
       /* 
        * TRILL
        * Data = {'000', Multi-Destination, Egress-RBridge-Nickname [15:0]}
        */  
    }
  }

  /* Not EEI then maybe FHEI ? */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 9, 2, 183, 182, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 2, 0, 157, 156, 5);
  }
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, 0, 2, &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 656, exit);

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 1, 86, 83, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 2, 0, 155, 152, 5);
  }
  tmp2 = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, 0, 4, &tmp2);
  SOC_SAND_CHECK_FUNC_RESULT(res, 659, exit);

  if (tmp == 1 && tmp2 == 5) {
      /* FHEI 3B MPLS POP */
      encap_info->mpls_cmd.command = ARAD_PP_MPLS_COMMAND_TYPE_POP;
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 2, 0, 64 + 24 - 1, 64, 5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 0, 44, 21, 5);
      }

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,14,1,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 660, exit);

      encap_info->mpls_cmd.has_cw = tmp;

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,15,1,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 665, exit);

      encap_info->mpls_cmd.model = (tmp == 1) ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE:SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,16,4,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 670, exit);

      encap_info->mpls_cmd.pop_next_header = tmp;

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,20,2,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 675, exit);

      encap_info->mpls_cmd.tpid_profile = tmp;
            
  } else if (tmp == 2 && tmp2 == 5) {
      /* FHEI 5B MPLS SWAP */
      encap_info->mpls_cmd.command = ARAD_PP_MPLS_COMMAND_TYPE_SWAP;
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 2, 0, 64 + 40 - 1, 64, 5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 0, 60, 21, 5);
      }

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,20,20,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 680, exit);

      encap_info->mpls_cmd.label = tmp;
  }

  curr_eep_index = 0;
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 2, 227, 96, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 1, 254, 234, 5);
  }
  tmp = 0;    
  /* action1_valid */
  res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp) {
    tmp2 = 0;
    /* action1_outlif */
    res = soc_sand_bitstream_get_any_field(regs_val,5,SOC_DPP_DEFS_GET(unit, out_lif_nof_bits),&tmp2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* set outlif according to information */
    encap_info->eep[curr_eep_index] = tmp2;

    res = arad_pp_eg_encap_entry_get_unsafe(
              unit,
              ARAD_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,/* ARAD_PP_EG_ENCAP_EEP_TYPE_LIF_EEP for PWE */
              encap_info->eep[curr_eep_index],
              1,
              &encap_info->encap_info[curr_eep_index],
              &(encap_info->eep[(curr_eep_index+1)]),
              &tmp2
           );
    curr_eep_index++;
    encap_info->nof_eeps = curr_eep_index;
  }

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 1, 255, 220, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 1, 156, 136, 5);
  }
  tmp = 0;
  
  /* action2_valid */
  res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp) {
    tmp2 = 0;
    /* action2_outlif */
    res = soc_sand_bitstream_get_any_field(regs_val,5,SOC_DPP_DEFS_GET(unit, out_lif_nof_bits),&tmp2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {/* This condition holds for arad plus and below. For jericho and above, next eep might be 0*/
        if(curr_eep_index != 0 && encap_info->eep[curr_eep_index] != SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID) {
            SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(tmp2,encap_info->eep[curr_eep_index],SOC_E_INTERNAL,36,exit);
        }
    }
    /* set outlif according to information */
    encap_info->eep[curr_eep_index] = tmp2;
    

    res = arad_pp_eg_encap_entry_get_unsafe(
      unit,
      ARAD_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,/* ARAD_PP_EG_ENCAP_EEP_TYPE_LIF_EEP for PWE */
      encap_info->eep[curr_eep_index],
      1,
      &encap_info->encap_info[curr_eep_index],
      &(encap_info->eep[(curr_eep_index+1)]),
      &tmp2
    );
    curr_eep_index++;
    encap_info->nof_eeps = curr_eep_index;
   }

  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 1, 219, 88, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 1, 58, 38, 5);
  }
  tmp = 0;
  
  /* action3_valid */
  res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp) {
    tmp2 = 0;
    /* action3_outlif */
    res = soc_sand_bitstream_get_any_field(regs_val,5,SOC_DPP_DEFS_GET(unit, out_lif_nof_bits),&tmp2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {/* This condition holds for arad plus and below. For jericho and above, next eep might be 0*/
        if(curr_eep_index != 0 && encap_info->eep[curr_eep_index] != SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID) {
            SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(tmp2,encap_info->eep[curr_eep_index],SOC_E_INTERNAL,37,exit);
        }
    }
    /* set outlif according to information */
    encap_info->eep[curr_eep_index] = tmp2;

    res = arad_pp_eg_encap_entry_get_unsafe(
      unit,
      ARAD_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,/* ARAD_PP_EG_ENCAP_EEP_TYPE_LIF_EEP for PWE */
      encap_info->eep[curr_eep_index],
      1,
      &encap_info->encap_info[curr_eep_index],
      &(encap_info->eep[(curr_eep_index+1)]),
      &tmp2
    );
    curr_eep_index++;
    encap_info->nof_eeps = curr_eep_index;
  }

  /* VSI, Out-AC */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 8, 0, 77, 60, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 8, 0, 65, 50, 5);
  }
  /* out_lif before ESEM is out-AC ?*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, tmp, &cur_eep_type);
  if (cur_eep_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {
    /* out_lif before ESEM is not out-AC */
    /* VSI is taken from ESEM key */
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 11, 0, 189, 174, 5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 11, 0, 128, 113, 5);
      }
    /* vsi */
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&encap_info->ll_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

    /* Read lookup found */        
    tmp = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  104,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_LAST_LOOKUP_RESULT_FOUNDf, &tmp));

    if (tmp) {
    /* ESEM last lookup found */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_LAST_LOOKUP_RESULTf, &tmp));
      /* Out-AC */
      encap_info->out_ac = tmp;
    } else {
      /* Take from Port default */
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 0, 89+22, 89+6, 5);
        } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 4, 0, 63+21, 63+6, 5);
        }
      /* Port-default-Out-AC */
      res = soc_sand_bitstream_get_any_field(regs_val, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &encap_info->out_ac);
      SOC_SAND_CHECK_FUNC_RESULT(res, 335, exit);
    }
  } else {
    /* VSI is taken from LinkLayer */
    ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 8, 0, SOC_IS_JERICHO(unit)? 25: 15, SOC_IS_JERICHO(unit)? 10: 0, 5);
    /* vsi */
    res = soc_sand_bitstream_get_any_field(regs_val, 0, 16, &encap_info->ll_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 337, exit);

    /* Out-AC is found */
    encap_info->out_ac = tmp;    
  }


  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,7,0,42,35,5);
  /* Out-Base-TM-Port */
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,8,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);

  /* Convert Q-pair to TM port */
  res = soc_port_sw_db_valid_ports_get(unit, 0, &pbmp);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

  found = 0;
  SOC_PBMP_ITER(pbmp, port_i) {

      res = soc_port_sw_db_flags_get(unit, port_i, &flags);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 155, exit);

      if (SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags)) {
          continue;
      }

      res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);

      res = soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &nof_priorities);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 170, exit);


      if (tmp >= base_q_pair && tmp < (base_q_pair + nof_priorities)) {
          res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &otm_port, &core);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 180, exit);
          encap_info->tm_port = otm_port;
          found = 1;
          break;
      }
  }  

  if (found == 0) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_DIAG_INVALID_TM_PORT_ERR, 4001, exit);
  }

  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,7,0,50,43,5);
  /* Out-PP-Port */
  res = soc_sand_bitstream_get_any_field(regs_val,0,8,&encap_info->pp_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_encap_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_encap_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ENCAP_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_encap_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the native out-RIF
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_out_rif_get_unsafe(
    SOC_SAND_IN     int     unit,
    SOC_SAND_IN     int     core_id,
    SOC_SAND_OUT    uint8   *rif_is_valid,
    SOC_SAND_OUT    uint32  *out_rif
  )
{
  ARAD_PP_DIAG_REG_FIELD
      fld;
  uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
  uint32
      tmp,
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(rif_is_valid);
  SOC_SAND_CHECK_NULL_INPUT(out_rif);

  *rif_is_valid = 0;
  *out_rif = 0;

  /* Stamp native VSI */
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 0, 244, 244, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 0, 1, 154, 154, 5);
  }

  res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Out-RIF profile */
  if (tmp) {
      *rif_is_valid = tmp;
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 2, 0, 137, 122, 5);
      res = soc_sand_bitstream_get_any_field(regs_val,0,16,out_rif);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_out_rif_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the reason for packet discard
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_eg_drop_log_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  )
{
  uint32
    reg_val,
    trap_val,
    offset = 0,
    bit_indx;
  ARAD_PP_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EG_DROP_LOG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_drop_log);

  ARAD_PP_DIAG_EG_DROP_LOG_INFO_clear(eg_drop_log);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,READ_EPNI_PP_INT_REGr(unit, SOC_CORE_ALL, &reg_val));
  /* map from register to PPD enum */

  
  for(bit_indx = 0; bit_indx < ARAD_PP_DIAG_FILTER_NOF_ETPP_FILTERS; ++bit_indx) {
      if(reg_val & SOC_SAND_BIT(bit_indx)){
          trap_val = arad_pp_diag_filter_bit_to_enum[bit_indx];
          ARAD_PP_DIAG_FILTER_TO_LOG(eg_drop_log->drop_log,trap_val);
      }
  }
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,READ_EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr(unit, SOC_CORE_ALL, &reg_val));
  offset = ARAD_PP_DIAG_FILTER_NOF_ETPP_FILTERS;

  for(bit_indx = 0; bit_indx < ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_1; ++bit_indx) {
      if(reg_val & SOC_SAND_BIT(bit_indx)){
          trap_val = arad_pp_diag_filter_bit_to_enum[bit_indx+offset];
          ARAD_PP_DIAG_FILTER_TO_LOG(eg_drop_log->drop_log,trap_val);
      }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,READ_EGQ_ERPP_DISCARD_INTERRUPT_REGISTER_2r(unit, SOC_CORE_ALL, &reg_val));
  offset += ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_1;

  for(bit_indx = 0; bit_indx < ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_2; ++bit_indx) {
      if(reg_val & SOC_SAND_BIT(bit_indx)){
          trap_val = arad_pp_diag_filter_bit_to_enum[bit_indx+offset];
          ARAD_PP_DIAG_FILTER_TO_LOG(eg_drop_log->drop_log,trap_val);
      }
  }

  /* check if to clear after read */
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & ARAD_PP_DIAG_FLAVOR_CLEAR_ON_GET)
  {
    reg_val = 0xFFFFFFFF;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_PP_INT_REGr(unit, SOC_CORE_ALL, reg_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr(unit, SOC_CORE_ALL, reg_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTER_2r(unit, SOC_CORE_ALL, reg_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_eg_drop_log_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_eg_drop_log_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EG_DROP_LOG_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_eg_drop_log_get_verify()", 0, 0);
}

STATIC
uint32
  arad_pp_diag_isem_access_key_to_lookup_type(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY_TYPE            key_type,
    SOC_SAND_OUT ARAD_PP_DIAG_LIF_LKUP_TYPE              *lookup_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (key_type) {
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:                              
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID:                    
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID: 
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_DESIGNATED_VID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_AC_TST2:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST2_TCAM:
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_AC;
    break; 
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3:                             
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_COUPLING:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_FRR:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_ELI:                           
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_ELI:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2_ELI:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF_ELI:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_PWE_L2_GAL:
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB:
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP:  
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_APPOINTED_FORWARDER:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NATIVE_INNER_TPID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_VSI:
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_TRILL;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI:   
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_VXLAN;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE:   
    *lookup_type = ARAD_PP_DIAG_LIF_LKUP_TYPE_L2GRE;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_FCOE:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV4:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV6:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_EoIP:    
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_RPA:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_TT:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISA:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISB:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_VT:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_VT:
  default:
    break;
  } 

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_isem_access_key_to_lookup_type()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the LIF DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_db_lif_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO            *db_info,
    SOC_SAND_OUT ARAD_PP_DIAG_LIF_LKUP_INFO          *lkup_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                 *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    prefix,
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    raw_key[6]={0},
    tmp=0,
    port=0;
  ARAD_PP_DIAG_MODE_INFO
    diag_mode;
  ARAD_PP_ISEM_ACCESS_ENTRY
    isem_res;
  ARAD_PP_ISEM_ACCESS_KEY
    isem_key;
  ARAD_PP_IHP_LIF_TABLE_TBL_DATA
    ihp_lif_table_data;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_port_config_tbl;
  uint8
    found;
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_L2_LIF_PWE_ADDITIONAL_INFO	pwe_additional_info; /* to do: pass this as an argument to this function*/

  SOC_PPC_RIF_INFO dummy;
  ARAD_PP_MPLS_TERM_PROFILE_INFO mpls_term_profile; 
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE);

  SOC_PPD_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  *ret_val = SOC_PPC_DIAG_OK;  

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                           
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_LIF_LKUP_INFO_clear(lkup_info);
  ARAD_PP_DIAG_LIF_KEY_clear(&lkup_info->key);



  /* get lookup result */
  if (db_info->bank_id == 0)
  {
      if (db_info->lkup_num == 1) /*second*/
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 21: 14), 0, (SOC_IS_JERICHO(unit)? 18: 17), 0, 5);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 21: 14), 0, (SOC_IS_JERICHO(unit)? 37: 35), (SOC_IS_JERICHO(unit)? 19: 18), 5);
      }
  }
  else if (db_info->bank_id == 1) /* bank_id == 1 */
  {
      if (db_info->lkup_num == 1) /*second*/
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 23: 16), 0, (SOC_IS_JERICHO(unit)? 18: 17), 0, 5);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 23: 16), 0, (SOC_IS_JERICHO(unit)? 37: 35), (SOC_IS_JERICHO(unit)? 19: 18), 5);
      }
  }
  else if(db_info->bank_id == 2)/* bank_id == 2 */
  {
      if (db_info->lkup_num == 1) /*second*/
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 25: 12), 0, (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) + 1, 1, 5);
      }
      else
      {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 25: 12), 0, (4 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) + 3, (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) + 3, 5);

      }
  }
  else /*bank_id == 3 (PORT)*/
  {
      if (db_info->lkup_num == 1)
          goto exit;
      lkup_info->found = TRUE;
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 2: 3), (SOC_IS_JERICHO(unit) ? 5 : 5), (SOC_IS_JERICHO(unit) ? 35 : 16), (SOC_IS_JERICHO(unit)? 28: 9), 5);

      res = soc_sand_bitstream_get_any_field(regs_val, 0, 8, &port);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
                   unit,
                   core_id,
                   port,
                   &vtt_port_config_tbl
                 );
      SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

      lkup_info->key.ac.raw_key = port;
      lkup_info->base_index = vtt_port_config_tbl.default_port_lif;


  }

  /* key lookup found?*/
  if(db_info->bank_id != 3)
  {
      if (db_info->bank_id == 2)
      {
          res = soc_sand_bitstream_get_any_field(regs_val, (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width)), 1, &tmp);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1225, exit);
          lkup_info->found = SOC_SAND_NUM2BOOL(tmp);
      }
      else
      {
          res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 18: 17, 1, &tmp);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1220, exit);
          lkup_info->found = SOC_SAND_NUM2BOOL(tmp);
      }
      if (!lkup_info->found)
      {
          goto exit;
      }
  }

    res = arad_pp_isem_access_entry_from_buffer( unit, regs_val, &isem_res);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  /*get key*/
  if (db_info->bank_id == 0 || db_info->bank_id == 1) { /* ISEM-A & B */
     if (db_info->bank_id == 0) {
         ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit) ? 20:13), 0, 2 * SOC_DPP_DEFS_GET(unit, isem_width) - 1, 0, 5);
     }
     else  { /* bank_id == 1 */
        
         ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 22: 15), 0, 2 * SOC_DPP_DEFS_GET(unit, isem_width) - 1, 0, 5);
     }

     /* key for lookup: raw-key and data-base ID/Type */
     if (db_info->lkup_num == 1) /*second*/
     {
        res = soc_sand_bitstream_get_any_field(regs_val, 0, SOC_DPP_DEFS_GET(unit, isem_width), raw_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
     }
     else /*first*/
     {
        res = soc_sand_bitstream_get_any_field(regs_val, SOC_DPP_DEFS_GET(unit, isem_width), SOC_DPP_DEFS_GET(unit, isem_width), raw_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
     }
  } else if(db_info->bank_id == 2){ /* bank_id == 2 TCAM*/
      {
          if (db_info->lkup_num == 1) /*second*/
          {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, (SOC_IS_JERICHO(unit)? 24: 11), 0, 165, 0, 10);
              res = soc_sand_bitstream_get_any_field(regs_val, 6, 160, raw_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

              res = soc_sand_bitstream_set_any_field(regs_val, 161, 5, raw_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
          }
          else /* first */
          {
              if (SOC_IS_JERICHO(unit)) {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 24, 0, 255, 172, 12);
                  tmp = 256-172;
              } else {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 11, 0, 255, 166, 12);
                  tmp = 256-166;
              }
              res = soc_sand_bitstream_set_any_field(regs_val, 0, tmp, raw_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

              if (SOC_IS_JERICHO(unit)) {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 24, 1, 75, 0, 14);
              } else {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 11, 1, 69, 0, 14);
              }
              res = soc_sand_bitstream_set_any_field(regs_val, tmp + 1, SOC_IS_JERICHO(unit)? 76: 70, raw_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

              if (SOC_IS_JERICHO(unit)) {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 24, 0, 171, 166, 13);
              } else {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 11, 1, 75, 70, 13);
              }
              res = soc_sand_bitstream_set_any_field(regs_val, 161, 6, raw_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

          }
      }
  }


    if(db_info->bank_id != 3)
    {
        /* diag mode */
        res = arad_pp_diag_mode_info_get_unsafe(
                unit,
                &diag_mode
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        /* fill key as struct from buffer */
        res = arad_pp_isem_access_key_from_buffer(
                unit,
                raw_key,
                db_info->lkup_num,
                db_info->bank_id,
                &isem_key
        );

        res = arad_pp_isem_prefix_from_buffer(unit,raw_key,db_info->lkup_num,db_info->bank_id, &prefix);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

        res = arad_pp_isem_access_prefix_to_key_type(unit, prefix, db_info->lkup_num,db_info->bank_id, raw_key, &(isem_key.key_type));
        SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);



        if (diag_mode.flavor & ARAD_PP_DIAG_FLAVOR_RAW)
        {
            ARAD_PP_COPY(lkup_info->key.raw,raw_key,uint32,6);
        }
        else
        {
            res = arad_pp_diag_isem_access_key_to_lookup_type(
                    unit,
                    isem_key.key_type,
                    &lkup_info->type
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
        }

        /* get result: index only */
        lkup_info->base_index = isem_res.sem_result_ndx;

        /* get result val*/
        if (diag_mode.flavor & ARAD_PP_DIAG_FLAVOR_RAW)
        {
            res = arad_pp_ihp_lif_table_tbl_get_unsafe(
                    unit,
                    lkup_info->base_index,
                    &ihp_lif_table_data
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            ARAD_PP_COPY(lkup_info->value.raw,ihp_lif_table_data.lif_table_entry,uint32,2);
        }
        else
        {
            /*
             * check whether this is mpls termination or pwe
             */
            res = arad_pp_ihp_lif_table_tbl_get_unsafe(unit, lkup_info->base_index, &ihp_lif_table_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            if ((ihp_lif_table_data.type == 0x0 || /* PWE P2P*/
                    ihp_lif_table_data.type == 0x1) && /* PWE MP */
                    (lkup_info->type == ARAD_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL) /* thought as MPLS */)
            {
                lkup_info->type = ARAD_PP_DIAG_LIF_LKUP_TYPE_PWE;
            }
        }
    }
    else /* PORT */
        lkup_info->type = ARAD_PP_DIAG_LIF_LKUP_TYPE_AC;

    switch(lkup_info->type)
    {
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_AC:
        res = arad_pp_l2_lif_ac_get_internal_unsafe(unit, NULL, TRUE,
                                                    &lkup_info->base_index,
                                                    &(lkup_info->value.ac),
                                                    &found);

        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_PWE:
        res = arad_pp_l2_lif_pwe_get_internal_unsafe(unit, 0, TRUE,
                                                     &lkup_info->base_index,
			  										 &pwe_additional_info,
                                                     &(lkup_info->value.pwe),
                                                     &found);

      res = arad_pp_mpls_term_profile_info_get_unsafe(unit, lkup_info->value.pwe.term_profile,&mpls_term_profile);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      lkup_info->value.pwe.has_cw = ((mpls_term_profile.flags & SOC_PPD_MPLS_TERM_HAS_CW) != 0);
    break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
      res = arad_pp_rif_mpls_label_map_get_internal_unsafe(unit, NULL, TRUE,
                                                           &lkup_info->base_index,
                                                           &(lkup_info->value.mpls),
                                                           &dummy,
                                                           &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
      res = arad_pp_rif_ip_tunnel_term_get_internal_unsafe(unit, NULL, TRUE,
                                                           &lkup_info->base_index,
                                                           &(lkup_info->value.ip),
                                                           &dummy,
                                                           &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_TRILL:
        res = arad_pp_l2_lif_trill_uc_get_internal_unsafe(unit, 0, TRUE,
                                                          &lkup_info->base_index,
                                                          &(lkup_info->value.trill),
                                                          &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID:

      res = arad_pp_l2_lif_isid_key_parse_unsafe(
              unit,
              &isem_key,
              &(lkup_info->key.mim)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      
      break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_L2GRE:
      res = arad_pp_l2_lif_gre_get_internal_unsafe(unit, NULL, TRUE,
                                                   &lkup_info->base_index,
                                                   &(lkup_info->value.l2gre),
                                                   &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    case ARAD_PP_DIAG_LIF_LKUP_TYPE_VXLAN: 
      arad_pp_l2_lif_vxlan_get_internal_unsafe(unit, NULL, TRUE,
                                               &lkup_info->base_index,
                                               &(lkup_info->value.vxlan),
                                               &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    default:
      break;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lif_lkup_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_db_lif_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO                    *db_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_DIAG_DB_USE_INFO, db_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lif_lkup_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the LEM DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_db_lem_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO              *db_info,
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_LKUP_TYPE            *type,
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_KEY                  *key,
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_VALUE                *val,
    SOC_SAND_OUT uint8                                 *valid
  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  ARAD_PP_DIAG_MODE_INFO
    diag_mode;
  ARAD_PP_LEM_ACCESS_KEY
    lem_key_str;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    lem_key_buf;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    lem_payload_str;
  ARAD_PP_ISEM_ACCESS_KEY
    isem_key;
  uint32
    lem_payload_buf[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S] = {0};
  uint32
    res = SOC_SAND_OK;
  uint32  valid_diag;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);


  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET, &valid_diag);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid_diag) {   
      *valid = 0;
      goto exit;                                                     
  }   

  ARAD_PP_DIAG_LEM_KEY_clear(key);
  ARAD_PP_DIAG_LEM_VALUE_clear(val);
  ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key_str);  

  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key_buf);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload_str);

  /*key*/
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 26, 0, 205, 0, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 22, 0, 234, 0, 5);
  }
  
  *valid = FALSE;

  if (db_info->lkup_num == 0)
  {
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            SOC_IS_JERICHO(unit)? 0: 43,
            SOC_DPP_DEFS_GET(unit, lem_width),
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* is found*/
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 27, 0, 95, 0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 21, 0, 90, 0, 5);
    }
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_DPP_DEFS_GET(unit, lem_payload_width) + 2, 1, &tmp); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (tmp)
    {
      *valid = TRUE;
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              0,
              SOC_DPP_DEFS_GET(unit, lem_payload_width),
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  else if (db_info->lkup_num == 1)
  {
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            SOC_IS_JERICHO(unit)? 126: 161,
            SOC_DPP_DEFS_GET(unit, lem_width),
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    /* is found*/
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 27, 0, 95, 0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 21, 0, 90, 0, 5);
    }

    /* flp2pmf_2eq3_lem_1st_lookup_found - get it from flp2pmf */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 4, 7, 62, 62, 51);
    } else  if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 0, 6, 189, 189, 51);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 0, 6, 40, 40, 5);
    }
    tmp = regs_val[0];

    /* back to original singals */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 27, 0, 95, 0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 21, 0, 90, 0, 5);
    }

    if (tmp)
    {
      *valid = TRUE;
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              SOC_DPP_DEFS_GET(unit, lem_payload_width) + 3,
              SOC_DPP_DEFS_GET(unit, lem_payload_width),
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  else if (db_info->lkup_num == 2)
  {
    /*key*/
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 6, 0, 95, 0, 5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 22, 0, 89, 0, 5);
      }
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            SOC_IS_JERICHO(unit)? 0: 16,
            SOC_DPP_DEFS_GET(unit, lem_width),
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* is found */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 2, 0, 255, 0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID, 21, 0, 45, 0, 5);
    }
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 114: 45, 1, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      *valid = TRUE;
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              SOC_IS_JERICHO(unit)? 69: 2,
              SOC_DPP_DEFS_GET(unit, lem_payload_width),
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

 /*
  * if raw
  */
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & ARAD_PP_DIAG_FLAVOR_RAW)
  {
    ARAD_PP_COPY(key->raw, lem_key_buf.buffer, uint32, SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));
    ARAD_PP_COPY(val->raw, lem_payload_buf, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
    goto exit;
  }

  res = arad_pp_lem_key_encoded_parse(
          unit,
          &lem_key_buf,
          &lem_key_str
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*valid)
  {
    res = arad_pp_lem_access_payload_parse(
            unit,
            ARAD_PP_LEM_ENTRY_TYPE_UNKNOWN,
            lem_payload_buf,
            &lem_payload_str
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC)
  {
    *type = ARAD_PP_DIAG_LEM_LKUP_TYPE_MACT;
    res = arad_pp_frwrd_mact_key_parse(
            unit,
            &lem_key_str,
            &(key->mact)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ARAD_PP_FRWRD_MACT_ENTRY_KEY_print(&(key->mact));
    if (!*valid)
    {
      goto exit;
    }
    res = arad_pp_frwrd_mact_payload_convert(
            unit,
            &lem_payload_str,
            &(val->mact)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM)
  {
    *type = ARAD_PP_DIAG_LEM_LKUP_TYPE_ILM;
    
    arad_pp_frwrd_ilm_lem_key_parse(
            &lem_key_str,
            &(key->ilm)
          );
    
    if (!*valid)
    {
      goto exit;
    }
    
    arad_pp_frwrd_ilm_lem_payload_parse(
            unit,
            &lem_payload_str,
            &(val->ilm)
          );
    
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH)
  {
    *type  = ARAD_PP_DIAG_LEM_LKUP_TYPE_SA_AUTH;
    res = arad_pp_sa_based_key_from_lem_key_map(
            unit,
            &lem_key_str,
            &(key->sa_auth)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }
    res = arad_pp_sa_based_payload_from_lem_payload_map(
            unit,
            &lem_payload_str,
            &(val->sa_auth)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* get VID assignment info */
    
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST)
  {
    *type  = ARAD_PP_DIAG_LEM_LKUP_TYPE_HOST;

    key->host.subnet.subnet.ip_address = lem_key_str.param[0].value[0];
    key->host.subnet.subnet.prefix_len = 32;
    key->host.vrf_id = lem_key_str.param[1].value[0];

    if (!*valid)
    {
      goto exit;
    }

    res = arad_pp_frwrd_em_dest_to_fec(
            unit,
            &lem_payload_str,
            &(val->host)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_EXTENDED)
  {
    *type  = ARAD_PP_DIAG_LEM_LKUP_TYPE_EXTEND_P2P;
    res = arad_pp_isem_access_key_from_buffer(unit,lem_key_str.param[0].value,db_info->lkup_num, db_info->bank_id, &isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    switch(isem_key.key_type)
    {
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD:
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.key_type = ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT;
      key->extend_p2p_key.key.ac.outer_vid = ARAD_PP_LIF_IGNORE_OUTER_VID;
      key->extend_p2p_key.key.ac.inner_vid = ARAD_PP_LIF_IGNORE_INNER_VID;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:    
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.key_type = (isem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID)?ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN:ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      key->extend_p2p_key.key.ac.inner_vid = ARAD_PP_LIF_IGNORE_INNER_VID;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:    
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.key_type = (isem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID)?ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN:ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN;;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      key->extend_p2p_key.key.ac.inner_vid = isem_key.key_info.l2_eth.inner_vid;
    break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB:
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID;
      key->extend_p2p_key.key.mim.isid_id = isem_key.key_info.pbb.isid;
      key->extend_p2p_key.key.mim.isid_domain = isem_key.key_info.pbb.isid_domain;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF:
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      key->extend_p2p_key.key.mpls.label_id = isem_key.key_info.mpls.label;
      key->extend_p2p_key.key.mpls.vsid = isem_key.key_info.mpls.in_rif;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      key->extend_p2p_key.key.mpls.label_id = isem_key.key_info.mpls.label;
      key->extend_p2p_key.key.mpls.vsid = 0;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP:
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL;
      key->extend_p2p_key.key.ip_tunnel.dip = isem_key.key_info.ip_tunnel.dip;
      key->extend_p2p_key.key.ip_tunnel.sip = isem_key.key_info.ip_tunnel.sip;
      key->extend_p2p_key.key.ip_tunnel.dip_prefix_len = isem_key.key_info.ip_tunnel.dip_prefix_len;
      key->extend_p2p_key.key.ip_tunnel.sip_prefix_len = isem_key.key_info.ip_tunnel.sip_prefix_len;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK:
      key->extend_p2p_key.type = ARAD_PP_DIAG_LIF_LKUP_TYPE_TRILL;
      key->extend_p2p_key.key.nick_name = isem_key.key_info.trill.nick_name;
      break;
    default:
      break;
    }
    if (!*valid)
    {
      goto exit;
    }

    
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_ADJ)
  {
    *type  = ARAD_PP_DIAG_LEM_LKUP_TYPE_TRILL_ADJ;
    res = arad_pp_sa_based_trill_key_from_lem_key_map(
            unit,
            &lem_key_str,
            &(key->trill_adj.sys_port_id),
            &(key->trill_adj.mac_key)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }
    res = arad_pp_sa_based_payload_from_lem_payload_map(
            unit,
            &lem_payload_str,
            &(val->sa_auth)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }
  else
  {
    ARAD_PP_COPY(key->raw, lem_key_buf.buffer, uint32, SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));
    ARAD_PP_COPY(val->raw, lem_payload_buf, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lem_lkup_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_db_lem_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO                    *db_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_DIAG_DB_USE_INFO, db_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lem_lkup_info_get_verify()", 0, 0);
}


/* ******************************************************************** 
 * Return the egress vlan edit command information
 * ********************************************************************/

uint32
  arad_pp_diag_egress_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_egress_vlan_edit_info_get_verify()", 0, 0);
}

uint32
  arad_pp_diag_egress_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES      *prm_vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  ) 
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
  ARAD_PP_DIAG_ENCAP_INFO 
    encap_info;
  ARAD_PP_EG_AC_INFO 
    ac_fields;
  uint32
    res = SOC_SAND_OK;
  uint32
    tag_format_int = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);


  ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES_clear(prm_vec_res);

  ARAD_PP_DIAG_ENCAP_INFO_clear(&encap_info);
  *ret_val = SOC_PPC_DIAG_OK;
  /* get out ac id from encap info */

  arad_pp_diag_encap_info_get_unsafe(unit, core_id, &encap_info, ret_val);
  if (*ret_val != SOC_PPC_DIAG_OK) {
      goto exit;
  }

  /* get ac_tag, ac_tag2 and edit profile  from out ac fields */
  
  ARAD_PP_EG_AC_INFO_clear(&ac_fields);

  arad_pp_eg_ac_info_get_unsafe(unit, encap_info.out_ac, &ac_fields);

  prm_vec_res->key.edit_profile = ac_fields.edit_info.edit_profile;

  prm_vec_res->ac_tag.vid = ac_fields.edit_info.vlan_tags[0].vid;

  prm_vec_res->ac_tag2.vid = ac_fields.edit_info.vlan_tags[1].vid;

  /* get tag format using signal */
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 11, 0, (SOC_IS_JERICHO(unit)? 149: 88), (SOC_IS_JERICHO(unit)? 141: 81), 5);

  res = soc_sand_bitstream_get_any_field(
          regs_val,
          SOC_IS_JERICHO(unit)? 4: 3,
          4,
          &tag_format_int
        );

  (prm_vec_res->key).tag_format = tag_format_int;
  /* get command informations using arad_pp_eg_vlan_edit_command_info_get_unsafe */
  arad_pp_eg_vlan_edit_command_info_get_unsafe(
          unit, 
          &(prm_vec_res->key), 
          &(prm_vec_res->cmd_info)
  );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_egress_vlan_edit_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Returns the cos info for each stage signal.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_diag_cos_info_get_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 core_id 

  )
{
    uint32 res = SOC_SAND_OK;
    uint32 fld_val[4];
    uint32 eg_cos_aft_fwd_header_code; 
    uint32 eg_cos_aft_fwd_dscp_remark; 
    uint32 eg_cos_aft_fwd_exp_remark;     

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    if (SOC_IS_ARAD(unit)) {
        /* 1. Ingress Initial COS assignment: {TC, DP}  */  
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {  
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 3, 0, 102, 100, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 3, 0, 107, 106, fld_val[1]); /* DP */
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 3, 0, 117, 115, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 3, 0, 122, 121, fld_val[1]); /* DP */
        }
        LOG_CLI((BSL_META_U(unit,
                            "Ingress Initial COS assignment: {TC(%d), DP(%d)}\n"), 
                            fld_val[0], fld_val[1]));
                            
        /* 2. Ingress L2/L3 (LIF) COS: {COS-profile, TC, DP, Terminated-DSCP-EXP} */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 2, 133, 128, fld_val[0]); /* COS-profile */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 0, 58, 56, fld_val[1]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 0, 62, 61, fld_val[2]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 2, 152, 145, fld_val[3]); /* Terminated-DSCP-EXP */
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 3, 2, 185, 180, fld_val[0]); /* COS-profile */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 2, 0, 117, 115, fld_val[1]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 2, 0, 122, 121, fld_val[2]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 3, 2, 204, 197, fld_val[3]); /* Terminated-DSCP-EXP */
        }
        LOG_CLI((BSL_META_U(unit,
                            "Ingress L2/L3 (LIF) COS: {COS-profile(%d), TC(%d), DP(%d), Terminated-DSCP-EXP(%d)}\n"), 
                            fld_val[0], fld_val[1],fld_val[2], fld_val[3]));                        

        /* 3. Ingress FWD COS: {TC, DP, In-DSCP-EXP} */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 7, 9, 7, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 7, 16, 15, fld_val[1]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 6, 197, 190, fld_val[2]); /* In-DSCP-EXP */
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 7, 158, 156, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 7, 165, 164, fld_val[1]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 6, 90, 83, fld_val[2]); /* In-DSCP-EXP */
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 8, 101, 99, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 8, 108, 107, fld_val[1]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID, core_id, 4, 8, 33, 26, fld_val[2]); /* In-DSCP-EXP */
        }
        LOG_CLI((BSL_META_U(unit,
                            "Ingress FWD COS: {TC(%d), DP(%d), In-DSCP-EXP(%d)}\n"), 
                            fld_val[0], fld_val[1],fld_val[2])); 

        /* 4. Ingress L2/L3 COS after FP: {TC, DP, In-DSCP-EXP} */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {   
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 9, 10, 197, 195, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 9, 10, 199, 198, fld_val[1]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 9, 10, 145, 138, fld_val[2]); /* In-DSCP-EXP */
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 9, 10, 230, 228, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 9, 10, 232, 231, fld_val[1]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 9, 10, 178, 171, fld_val[2]); /* In-DSCP-EXP */
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 11, 9, 7, fld_val[0]); /* TC */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 11, 11, 10, fld_val[1]); /* DP */
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 0, 10, 213, 206, fld_val[2]); /* In-DSCP-EXP */
        }
        LOG_CLI((BSL_META_U(unit,
                            "Ingress L2/L3 COS after FP: {TC(%d), DP(%d), In-DSCP-EXP(%d)}\n"), 
                            fld_val[0], fld_val[1],fld_val[2]));  
                            

        /* 5. Ingress L2/L3 COS to Ingress TM: {TC, DP} */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 12, 0, 129, 127, fld_val[0]); /* TC */        
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 12, 0, 126, 125, fld_val[1]); /* DP */            
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 10, 2, 157, 155, fld_val[0]); /* TC */        
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 10, 2, 159, 158, fld_val[1]); /* DP */
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 2, 2, 182, 180, fld_val[0]); /* TC */        
            ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID, core_id, 2, 2, 184, 183, fld_val[1]); /* DP */  
        }
        LOG_CLI((BSL_META_U(unit,
                            "Ingress L2/L3 COS to Ingress TM: {TC(%d), DP(%d)}\n"), 
                            fld_val[0], fld_val[1]));                       

        /* 6. Egress COS before Egress TM: {TC, DP} */
        ARAD_PP_DIAG_FLD_GET(ARAD_EGQ_ID, core_id, 4, 1, 50, 48, fld_val[0]); /* TC */
        ARAD_PP_DIAG_FLD_GET(ARAD_EGQ_ID, core_id, 4, 1, 23, 22, fld_val[1]); /* DP */
        LOG_CLI((BSL_META_U(unit,
                            "Egress COS before Egress TM: {TC(%d), DP(%d)}\n"), 
                            fld_val[0], fld_val[1]));    
                          
        
        /* 7. Egress COS after Egress TM: {TC, DP, In-DSCP-EXP} */        
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 2, 1, 49, 47, fld_val[0]); /* TC */
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 2, 1, 22, 21, fld_val[1]); /* DP */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 5, 0, 178, 171, fld_val[2]); /* In-DSCP-EXP */    
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 5, 0, 180, 173, fld_val[2]); /* In-DSCP-EXP */
        }        
        LOG_CLI((BSL_META_U(unit,
                            "Egress COS after Egress TM: {TC(%d), DP(%d), In-DSCP-EXP(%d)}\n"), 
                            fld_val[0], fld_val[1],fld_val[2]));  

        /* 8. Egress COS after FWD: {In-DSCP-EXP, Remark-profile, DP, Out-DSCP-EXP} */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 5, 0, 178, 171, fld_val[0]); /* In-DSCP-EXP */        
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 5, 0, 182, 179, eg_cos_aft_fwd_header_code); 
            if ((eg_cos_aft_fwd_header_code == 0) || 
                (eg_cos_aft_fwd_header_code == 1) || 
                (eg_cos_aft_fwd_header_code == 2)) { /* Bridge, IPV4 */
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 11, 0, 247, 235, eg_cos_aft_fwd_dscp_remark); 
                fld_val[1] = 0;
                SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_dscp_remark, 6, 4);
                fld_val[2] = 0;
                SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_dscp_remark, 10, 2);
            } else if (eg_cos_aft_fwd_header_code == 5) { /* MPLS */
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 11, 0, 254, 248, eg_cos_aft_fwd_exp_remark);
                fld_val[1] = 0;
                SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_exp_remark, 1, 4);
                fld_val[2] = 0;
                SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_exp_remark, 5, 2);
            } else { /* INVALID */
                fld_val[1] = -1;
                fld_val[2] = -1;
            }           
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 7, 0, 74, 67, fld_val[3]); /* Out-DSCP-EXP */
        } else {            
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 5, 0, 180, 173, fld_val[0]); /* In-DSCP-EXP */        
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 5, 0, 184, 181, eg_cos_aft_fwd_header_code);   
            if ((eg_cos_aft_fwd_header_code == 0) || 
                (eg_cos_aft_fwd_header_code == 1) || 
                (eg_cos_aft_fwd_header_code == 2)) { /* Bridge, IPV4 */
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 11, 1, 53, 41, eg_cos_aft_fwd_dscp_remark); 
                fld_val[1] = 0;
                SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_dscp_remark, 6, 4);
                fld_val[2] = 0;
                SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_dscp_remark, 10, 2);
            } else if (eg_cos_aft_fwd_header_code == 5) { /* MPLS */
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 11, 1, 60, 54, eg_cos_aft_fwd_exp_remark);
                fld_val[1] = 0;
                SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_exp_remark, 1, 4);
                fld_val[2] = 0;
                SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_exp_remark, 5, 2);
            } else { /* INVALID */
                fld_val[1] = -1;
                fld_val[2] = -1;
            }                    
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 7, 0, 76, 69, fld_val[3]); /* Out-DSCP-EXP */
        }
        LOG_CLI((BSL_META_U(unit,
                            "Egress COS after FWD: {In-DSCP-EXP(%d), Remark-profile(%d), DP(%d), Out-DSCP-EXP(%d)}\n"), 
                            fld_val[0], fld_val[1],fld_val[2], fld_val[3]));
        
        /* 9. Egress COS after Encap: {Out-DSCP-EXP} */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 8, 0, 73, 66, fld_val[0]); /* Out-DSCP-EXP */
        } else {
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID, core_id, 8, 0, 85, 78, fld_val[0]); /* Out-DSCP-EXP */
        }
        LOG_CLI((BSL_META_U(unit,
                            "Egress COS after Encap: {Out-DSCP-EXP(%d)}\n"), 
                            fld_val[0]));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sig_cos_print_cos_data()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_diag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_diag_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_diag;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_diag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_diag_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_diag;
}
uint32
  ARAD_PP_DIAG_MODE_INFO_verify(
    SOC_SAND_IN  ARAD_PP_DIAG_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->flavor, ARAD_PP_DIAG_FLAVOR_MAX, ARAD_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_DIAG_MODE_INFO_verify()",0,0);
}

uint32
  ARAD_PP_DIAG_DB_USE_INFO_verify(
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->lkup_num, ARAD_PP_DIAG_LKUP_NUM_MAX, ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->bank_id, ARAD_PP_DIAG_BANK_ID_MAX, ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_DIAG_DB_USE_INFO_verify()",0,0);
}
uint32
  ARAD_PP_DIAG_IPV4_VPN_KEY_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_VPN_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->vrf, SOC_DPP_DEFS_GET(unit, nof_vrfs), ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_IPV4_VPN_ROUTE_KEY, &(info->key), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_DIAG_IPV4_VPN_KEY_verify()",0,0);
}


/* Returns the 1st (odd) instruction entry used */
uint32
  arad_pp_diag_prge_first_instr_get(int unit, int core_id, uint32 *first_instruction) {
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32 res;

    fld.base = (11 << 16) + 0;
    if (SOC_IS_JERICHO(unit))
    {
        fld.msb = 86;
        fld.lsb = 80;
    }
    else
    {
        fld.msb = 29;
        fld.lsb = 24;
    }

    res = arad_pp_diag_dbg_val_get_unsafe(unit, core_id, ARAD_EPNI_ID, &fld, regs_val);
    if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
    {
        return -1;
    }

    res = soc_sand_bitstream_get_any_field(regs_val,0,5,first_instruction);
    if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
    {
        return -1;
    }

    return 0;
}




/*
 * Function:     arad_pp_diag_ftmh_cfg_get
 * Purpose:      Call soc_ftmh_cfg_get
 */

int
arad_pp_diag_ftmh_cfg_get(int unit, int * p_cfg_ftmh_lb_key_ext_en, int * p_cfg_ftmh_stacking_ext_enable)
{

        int
            rv = SOC_E_NONE;

        rv = soc_ftmh_cfg_get(unit,p_cfg_ftmh_lb_key_ext_en, p_cfg_ftmh_stacking_ext_enable);
        return rv;
}


/*
 * Function:     arad_pp_diag_epni_prge_program_tbl_get
 * Purpose:      Call arad_pp_epni_prge_program_tbl_get_unsafe
 */

uint32
arad_pp_diag_epni_prge_program_tbl_get(int unit, uint32 offset, ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data)
{

        uint32
            rv = SOC_E_NONE;

        rv = arad_pp_epni_prge_program_tbl_get_unsafe( unit, offset, tbl_data);

        return rv;
}


/*
 * Function:     arad_pp_diag_mem_read
 * Purpose:      Call soc_mem_read
 */

int
arad_pp_diag_mem_read(int unit,
                        soc_mem_t mem,
                        int copyno,
                        int in_line,
                        void* val){

        int
            rv = SOC_E_NONE;

        if (copyno == -1) {
            rv = soc_mem_read(unit, mem, EPNI_BLOCK(unit, SOC_CORE_ALL), in_line, val); 
        }

        else{
            rv = soc_mem_read(unit, mem, copyno, in_line, val); 
        }

        return rv;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */
