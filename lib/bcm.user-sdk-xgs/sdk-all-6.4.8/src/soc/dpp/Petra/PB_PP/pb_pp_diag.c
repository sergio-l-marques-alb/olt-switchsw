/* $Id: pb_pp_diag.c,v 1.18 Broadcom SDK $
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
*/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_diag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ilm.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_rif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_extend_p2p.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_sa_auth.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_trill.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_encap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mpls_term.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_diagnostics.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_port.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_port.h>
#include <soc/dpp/Petra/petra_packet.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>


#include <soc/dpp/TMC/tmc_api_packet.h>


#include <soc/dpp/Petra/petra_ports.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_DIAG_PKT_TRACE_MAX                               (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_MAX_SIZE_MAX                                (2000)
#define SOC_PB_PP_DIAG_BUFF_MAX                                    (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_BUFF_LEN_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_FLAVOR_MAX                                  (SOC_PB_PP_NOF_DIAG_FLAVORS-1)
#define SOC_PB_PP_DIAG_LKUP_NUM_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_LKUP_USAGE_MAX                              (SOC_PB_PP_NOF_DIAG_TCAM_USAGES-1)
#define SOC_PB_PP_DIAG_IN_TM_PORT_MAX                              (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_DIAG_PP_CONTEXT_MAX                              (SOC_TMC_PORT_NOF_HEADER_TYPES-1)
#define SOC_PB_PP_DIAG_PACKET_QUAL_MAX                             (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_DIAG_CODE_MAX                                    (SOC_PB_PP_NOF_TRAP_CODES-1)
#define SOC_PB_PP_DIAG_CPU_DEST_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_IP_MAX                                      (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_TYPE_MAX                                    (SOC_PB_PP_NOF_DIAG_LIF_LKUP_TYPES-1)
#define SOC_PB_PP_DIAG_BASE_INDEX_MAX                              (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_OPCODE_ID_MAX                               (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_LENGTH_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_FEC_PTR_MAX                                 (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_ENCAP_TYPE_MAX                              (SOC_SAND_PP_NOF_ETH_ENCAP_TYPES-1)
#define SOC_PB_PP_DIAG_VLAN_TAG_FORMAT_MAX                         (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define SOC_PB_PP_DIAG_NEXT_PRTCL_MAX                              (SOC_PB_PP_NOF_L2_NEXT_PRTCL_TYPES-1)
#define SOC_PB_PP_DIAG_HDR_TYPE_MAX                                (SOC_PB_PP_NOF_PKT_HDR_TYPES-1)
#define SOC_PB_PP_DIAG_HDR_OFFSET_MAX                              (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_RANGE_INDEX_MAX                             (SOC_SAND_U8_MAX)
#define SOC_PB_PP_DIAG_TERM_TYPE_MAX                               (SOC_PB_PP_NOF_DIAG_MPLS_TERM_TYPES-1)
#define SOC_PB_PP_DIAG_LABEL_MAX                                   (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_RANGE_BIT_MAX                               (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_FRWRD_TYPE_MAX                              (SOC_PB_PP_NOF_PKT_FRWRD_TYPES-1)
#define SOC_PB_PP_DIAG_VRF_MAX                                     (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_TRILL_UC_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_FRWRD_HDR_INDEX_MAX                         (SOC_SAND_U8_MAX)
#define SOC_PB_PP_DIAG_VALID_FIELDS_MAX                            (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_METER1_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_METER2_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_DP_METER_CMD_MAX                            (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_COUNTER1_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_COUNTER2_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_CUD_MAX                                     (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_EEP_MAX                                     (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_ETHER_TYPE_MAX                              (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_DIAG_TOTAL_SIZE_MAX                              (SOC_SAND_U32_MAX)
#define SOC_PB_PP_DIAG_NOF_PACKETS_MAX                             (SOC_SAND_U32_MAX)

/* size of header qual in bits*/
#define SOC_PB_PP_DIAG_HDR_QUAL_SIZE      (7)

/* size of header index in bits*/
#define SOC_PB_PP_DIAG_HDR_INDEX_SIZE      (7)

/* size of header index in bytes */
#define SOC_PB_PP_DIAG_HDR_MPLS_BYTE_SIZE      (4)

#define SOC_PB_PP_DIAG_FLD_LSB_MIN (0)
#define SOC_PB_PP_DIAG_FLD_MSB_MAX (255)
#define SOC_PB_PP_DIAG_FLD_LEN (256)

#define SOC_PB_PP_DIAG_NOF_TRAPS (256)
/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define SOC_PB_PP_DIAG_FLD_FILL(prm_fld,prm_addr_msb, prm_addr_lsb,prm_fld_msb,prm_fld_lsb)  \
          (prm_fld)->addr.base = (prm_addr_msb << 16) + prm_addr_lsb;  \
          (prm_fld)->msb = prm_fld_msb;  \
          (prm_fld)->lsb= prm_fld_lsb;

#define SOC_PB_PP_DIAG_FLD_READ(prm_fld,prm_blk, prm_addr_msb, prm_addr_lsb,prm_fld_msb,prm_fld_lsb,prm_err_num)  \
  SOC_PB_PP_DIAG_FLD_FILL(prm_fld,prm_addr_msb, prm_addr_lsb,prm_fld_msb,prm_fld_lsb);    \
  res = soc_pb_pp_diag_dbg_val_get_unsafe(      \
          unit,      \
          prm_blk,      \
          prm_fld,      \
          regs_val      \
        );              \
  SOC_SAND_CHECK_FUNC_RESULT(res, prm_err_num, exit);


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_diag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_MODE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_MODE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_MODE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_MODE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_TRACE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_TRACE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_TRACE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PARSING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PARSING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PARSING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TERMINATION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TERMINATION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TERMINATION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_LEARNING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_LEARNING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_LEARNING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_ENCAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_ENCAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_ENCAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_EG_DROP_LOG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_EG_DROP_LOG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_EG_DROP_LOG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_SEND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_SEND_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_SEND_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_DBG_VAL_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_LIF_DB_ID_TO_DB_TYPE_MAP_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_DIAG_PKT_ETH_HEADER_BUILD),


  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_diag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR",
    "The parameter 'pkt_trace' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DIAG_PKT_TRACES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DIAG_LEM_LKUP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_VALID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_VALID_OUT_OF_RANGE_ERR",
    "The parameter 'valid' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DIAG_LEM_LKUP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_BUFF_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR",
    "The parameter 'flavor' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DIAG_FLAVORS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR",
    "The parameter 'lkup_num' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR",
    "The parameter 'lkup_usage' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DIAG_TCAM_USAGES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'in_tm_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_PP_CONTEXT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_PP_CONTEXT_OUT_OF_RANGE_ERR",
    "The parameter 'pp_context' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_PORT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR",
    "The parameter 'packet_qual' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_CODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'code' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_dest' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_IP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_IP_OUT_OF_RANGE_ERR",
    "The parameter 'ip' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'base_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'length' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR",
    "The parameter 'fec_ptr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'encap_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETH_ENCAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR",
    "The parameter 'next_prtcl' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_NEXT_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_PKT_HDR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_offset' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'range_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'term_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DIAG_MPLS_TERM_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR",
    "The parameter 'range_bit' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_PKT_FRWRD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_VRF_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_VRF_OUT_OF_RANGE_ERR",
    "The parameter 'vrf' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR",
    "The parameter 'trill_uc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_hdr_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR",
    "The parameter 'valid_fields' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_METER1_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_METER1_OUT_OF_RANGE_ERR",
    "The parameter 'meter1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_METER2_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_METER2_OUT_OF_RANGE_ERR",
    "The parameter 'meter2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR",
    "The parameter 'dp_meter_cmd' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR",
    "The parameter 'counter1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR",
    "The parameter 'counter2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_CUD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_CUD_OUT_OF_RANGE_ERR",
    "The parameter 'cud' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_EEP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR",
    "The parameter 'drop_log' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'ether_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'total_size' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_packets' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_DIAG_RESTORE_NOT_SAVED_ERR,
    "SOC_PB_PP_DIAG_RESTORE_NOT_SAVED_ERR",
    "try to restore traps status when not saved.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR",
    "LIF DB ID is not used. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },



  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};


/* internal debug enable */
#ifdef SAND_LOW_LEVEL_SIMULATION
#define SOC_PB_DEBUG_INTERNAL (1)
#else
#define SOC_PB_DEBUG_INTERNAL (0)
#endif
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
 *     read diagnostic value
 *********************************************************************/

#if SOC_PB_DEBUG_INTERNAL
 STATIC uint32
   soc_pb_pp_diag_dbg_sim_fil_val(
   SOC_SAND_IN  uint32               sample_id,
   SOC_SAND_IN  uint32               blk,
   SOC_SAND_IN  uint32               addr,
   SOC_SAND_OUT uint32               regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN]
 )
 {
   return 0;
 }
#endif

uint32
  soc_pb_pp_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32               blk,
    SOC_SAND_IN  SOC_PB_PP_DIAG_REG_FIELD   *fld,
    SOC_SAND_OUT uint32               val[SOC_PB_PP_DIAG_DBG_VAL_LEN]
  )
{
  uint32
  reg_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
  tbl_entry_indx,
  reg_indx,
  entry_start,
  entry_end,
  bit_lsb = 0,
  bit_msb = 0;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA
    erpp_debug_tbl_data;
  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA
    epni_etpp_debug_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DBG_VAL_GET_UNSAFE);
  SOC_PB_PP_CLEAR(val,uint32,SOC_PB_PP_DIAG_DBG_VAL_LEN);
  SOC_PB_PP_CLEAR(reg_val,uint32,SOC_PB_PP_DIAG_DBG_VAL_LEN);

  regs = soc_petra_regs();
  pp_regs = soc_pb_pp_regs();

  if (blk == SOC_PB_IHP_ID)
  {
    SOC_PB_FLD_SET(regs->ihp.ihp_debug_reg.dbg_addr,fld->addr.base,10,exit);
    res = soc_petra_read_reg_buffer_unsafe(
            unit,
            SOC_PB_REG_DB_ACC_REF(regs->ihp.ihp_debug_res_reg.addr),
            SOC_TMC_DEFAULT_INSTANCE,
            SOC_PB_PP_DIAG_DBG_VAL_LEN,
            reg_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (blk == SOC_PB_IHB_ID)
  {
    SOC_PB_FLD_SET(regs->ihb.ihb_debug_reg.dbg_addr,fld->addr.base,20,exit);
    res = soc_petra_read_reg_buffer_unsafe(
            unit,
            SOC_PB_REG_DB_ACC_REF(regs->ihb.ihb_debug_res_reg.addr),
            SOC_TMC_DEFAULT_INSTANCE,
            SOC_PB_PP_DIAG_DBG_VAL_LEN,
            reg_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (blk == SOC_PB_EGQ_ID)
  {
    SOC_PB_PP_FLD_SET(pp_regs->egq.erpp_debug_configuration_reg.debug_select,fld->addr.base,30,exit);
    res = soc_pb_egq_erpp_debug_tbl_get_unsafe(
            unit,
            0,
            &erpp_debug_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    SOC_PB_PP_COPY(reg_val,erpp_debug_tbl_data.erpp_debug,uint32,8);
  }
  else if (blk == SOC_PB_EPNI_ID)
  {
    SOC_PB_PP_FLD_SET(pp_regs->epni.etpp_debug_configuration_reg.debug_select,fld->addr.base,40,exit);

    entry_start = fld->lsb /32;
    entry_end = fld->msb /32;
    bit_lsb = fld->lsb - entry_start*32;
    bit_msb = fld->msb - entry_start*32;
    reg_indx = 0;
    for (tbl_entry_indx=entry_start; tbl_entry_indx<=entry_end; ++tbl_entry_indx)
    {
      res = soc_pb_pp_epni_etpp_debug_tbl_get_unsafe(
              unit,
              tbl_entry_indx,
              &epni_etpp_debug_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      reg_val[reg_indx++] = epni_etpp_debug_tbl_data.etpp_debug;
    }
  }



  if (blk != SOC_PB_EPNI_ID)
  {
#if SOC_PB_DEBUG_INTERNAL
      soc_pb_pp_diag_dbg_sim_fil_val(1,blk, fld->addr.base,reg_val);
#endif
    res = soc_sand_bitstream_get_any_field(reg_val,SOC_PETRA_FLD_LSB(*fld),SOC_PETRA_FLD_NOF_BITS(*fld),val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
      reg_indx = 0;
#if SOC_PB_DEBUG_INTERNAL
      soc_pb_pp_diag_dbg_sim_fil_val(1,blk, fld->addr.base,reg_val);
      reg_indx = entry_start;
#endif
      res = soc_sand_bitstream_get_any_field(reg_val + reg_indx,bit_lsb,bit_msb-bit_lsb+1,val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_dbg_val_get_unsafe()", 0, 0);
}

 
/*********************************************************************
 *     given termination code returns the place of the i-th MPLS label
 *********************************************************************/
STATIC uint32
  soc_pb_pp_diag_term_code_to_header_index(
    SOC_SAND_IN  SOC_PB_PP_PKT_TERM_TYPE    term_type,
    SOC_SAND_IN  uint32               lbl_index
  )
{
  uint32
    nof_labels=0;
  switch(term_type)
  {
  case SOC_PB_PP_PKT_TERM_TYPE_MPLS_ETH:
  case SOC_PB_PP_PKT_TERM_TYPE_CW_MPLS_ETH:
    nof_labels = 1;
    break;
  case SOC_PB_PP_PKT_TERM_TYPE_MPLS2_ETH:
  case SOC_PB_PP_PKT_TERM_TYPE_CW_MPLS2_ETH:
    nof_labels = 2;
    break;
  case SOC_PB_PP_PKT_TERM_TYPE_MPLS3_ETH:
  case SOC_PB_PP_PKT_TERM_TYPE_CW_MPLS3_ETH:
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

uint32
  soc_pb_pp_diag_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE app_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE signal_type,
    SOC_SAND_IN  uint32                  dest_buffer,
    SOC_SAND_IN  uint32                  asd_buffer,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT uint8                 *is_sa_drop
  )
{
  SOC_PB_PP_FRWRD_DECISION_INFO
    fwd_decision_tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  /* treat trap destination */
 res = soc_pb_pp_fwd_decision_in_buffer_parse(
          app_type,
          dest_buffer,
          asd_buffer,
          fwd_decision,
          is_sa_drop
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  if (fwd_decision->type == SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP)
  {
    goto exit;
  }

  res = soc_pb_pp_fwd_decision_in_buffer_parse(
          signal_type,
          dest_buffer,
          asd_buffer,
          fwd_decision,
          is_sa_drop
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  res = soc_pb_pp_fwd_decision_in_buffer_parse(
          app_type,
          dest_buffer,
          asd_buffer,
          &(fwd_decision_tmp),
          is_sa_drop
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  SOC_PB_PP_COPY(&(fwd_decision->additional_info), &(fwd_decision_tmp.additional_info),SOC_PB_PP_FRWRD_DECISION_TYPE_INFO,1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_fwd_decision_in_buffer_parse()", 0, 0);

}




/*********************************************************************
 *     given forwarding code returns the place of the i-th MPLS label
 *********************************************************************/
STATIC SOC_PB_PP_PKT_HDR_TYPE
  soc_pb_pp_diag_hdr_at_index(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_STK_TYPE   pkt_type,
    SOC_SAND_IN  uint32                 hdr_index
  )
{
  SOC_PB_PP_PKT_HDR_TYPE
    hdr_type;

  hdr_type = SOC_SAND_GET_BITS_RANGE(pkt_type,4*hdr_index + 3,4*hdr_index);
  return hdr_type;
}

STATIC SOC_PB_PP_DIAG_FWD_LKUP_TYPE
  soc_pb_pp_diag_frwrd_type_to_lkup_type_map(
    SOC_SAND_IN  SOC_PB_PP_PKT_FRWRD_TYPE   frwrd_type
  )
{
  switch(frwrd_type)
  {
  case SOC_PB_PP_PKT_FRWRD_TYPE_BRIDGE:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_MACT;
  case SOC_PB_PP_PKT_FRWRD_TYPE_IPV4_UC:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC;
  case SOC_PB_PP_PKT_FRWRD_TYPE_IPV4_MC:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC;
  case SOC_PB_PP_PKT_FRWRD_TYPE_IPV6_UC:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC;
  case SOC_PB_PP_PKT_FRWRD_TYPE_IPV6_MC:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC;
  case SOC_PB_PP_PKT_FRWRD_TYPE_MPLS:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_ILM;
  case SOC_PB_PP_PKT_FRWRD_TYPE_TRILL:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC;
  case SOC_PB_PP_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_MACT;
  case SOC_PB_PP_PKT_FRWRD_TYPE_CPU_TRAP:
  default:
    return SOC_PB_PP_DIAG_FWD_LKUP_TYPE_NONE;
  }
}

uint32
  soc_pb_pp_diag_traps_range_info_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                            regs_val[2],
    SOC_SAND_IN  uint32                            rng_id,
    SOC_SAND_IN  uint32                            nof_traps,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TRAPS_INFO               *traps_info
  )
{
  uint32
    indx;
  SOC_PB_PP_TRAP_CODE
    trap_code;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  for (indx = 0; indx < nof_traps; ++indx)
  {
    if (soc_sand_bitstream_test_bit(regs_val,indx))
    {
      res = soc_pb_pp_trap_sb_to_trap_code_map_get(
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
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_range_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE);
  
  res = soc_pb_diag_sample_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_sample_enable_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_sample_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_sample_enable_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_diag_sample_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_sample_enable_get_verify()", 0, 0);
}

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = soc_pb_diag_sample_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_sample_enable_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the mode configuration for diag module, including
 *     diag-flavor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_MODE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  res = soc_pb_pp_sw_db_diag_mode_set(
          unit,
          mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_mode_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_MODE_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_MODE_INFO, mode_info, 10, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_mode_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_diag_mode_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_MODE_INFO_GET_VERIFY);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_mode_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the mode configuration for diag module, including
 *     diag-flavor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  SOC_PB_PP_DIAG_MODE_INFO_clear(mode_info);

  res = soc_pb_pp_sw_db_diag_mode_get(
          unit,
          mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_mode_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Clear the trace of transmitted packet, so next trace
 *     info will relate to next packets to transmit
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_pkt_trace_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  )
{
  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA
    ihp_sem_result_accessed_tbl_data;
  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA
    ihp_mpls_label_range_encountered_tbl_data;
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL
    *fec_accessed_tbl;
  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL
    *sem_result_accessed_tbl;
  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL
    *mpls_label_range_encountered_tbl;
  uint32
    clear_val = 0xFFFFFFFF;
  SOC_PB_PP_REGS
    *regs;
  SOC_PB_REGS
    *tm_regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_TRACE_CLEAR_UNSAFE);
  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();


  if (pkt_trace & SOC_PB_PP_DIAG_PKT_TRACE_FEC)
  {
    fec_accessed_tbl = &(tables->ihb.fec_entry_accessed_tbl);
    ihb_fec_entry_accessed_tbl_data.fec_entry_accessed = 0;
    res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, fec_accessed_tbl->addr.size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_pb_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_fec_entry_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  if (pkt_trace & SOC_PB_PP_DIAG_PKT_TRACE_LIF)
  {
    sem_result_accessed_tbl = &(tables->ihp.sem_result_accessed_tbl);
    ihp_sem_result_accessed_tbl_data.sem_result_accessed = 0;
    res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, sem_result_accessed_tbl->addr.size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    res = soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_sem_result_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  if (pkt_trace & SOC_PB_PP_DIAG_PKT_TRACE_TUNNEL_RNG)
  {
    mpls_label_range_encountered_tbl = &(tables->ihp.mpls_label_range_encountered_tbl);
    ihp_mpls_label_range_encountered_tbl_data.mpls_label_range_encountered = 0;
    res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, mpls_label_range_encountered_tbl->addr.size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_pb_pp_ihp_mpls_label_range_encountered_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_mpls_label_range_encountered_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  /* Traps clear*/
  if (pkt_trace & SOC_PB_PP_DIAG_PKT_TRACE_TRAP)
  {
    /* LLR traps */
    SOC_PB_PP_REG_SET(regs->ihp.dbg_llr_trap0_reg,clear_val,110,exit);
    SOC_PB_PP_REG_SET(regs->ihp.dbg_llr_trap1_reg,clear_val,120,exit);
    /* VTT traps */
    SOC_PB_PP_REG_SET(regs->ihp.vtt_trap_encountered1_reg,clear_val,140,exit);
    SOC_PB_PP_REG_SET(regs->ihp.vtt_trap_encountered2_reg,clear_val,150,exit);
    /* FLP traps */
    SOC_PB_PP_REG_SET(regs->ihb.dbg_flp_trap0_reg,clear_val,170,exit);
    SOC_PB_PP_REG_SET(regs->ihb.dbg_flp_trap1_reg,clear_val,180,exit);
    /* FER traps */
    SOC_PB_PP_REG_SET(regs->ihb.dbg_fer_trap_reg,clear_val,190,exit);
  }

  if (pkt_trace & SOC_PB_PP_DIAG_PKT_TRACE_EG_DROP_LOG)
  {
    SOC_PB_PP_REG_SET(tm_regs->egq.erpp_discard_interrupt_reg,clear_val,200,exit);
  }
  
exit:
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, 0);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_pkt_trace_clear_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_pkt_trace_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_TRACE_CLEAR_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pkt_trace, SOC_PB_PP_DIAG_PKT_TRACE_MAX, SOC_PB_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_pkt_trace_clear_verify()", 0, 0);
}

/*********************************************************************
*     Returns the header of last received packet entered the
 *     device and the its association to TM/system/PP ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_received_packet_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    indx,
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    dummy,
    cur_lsb=0,
    read_size,
    tmp=0;
  uint8
    is_lag;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rcvd_pkt_info);

  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_clear(rcvd_pkt_info);

  /* TM-port*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,0,SOC_PB_PP_DIAG_FLD_MSB_MAX,SOC_PB_PP_DIAG_FLD_LSB_MIN,15);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          regs_val,
          0,
          7,
          &(tmp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  rcvd_pkt_info->in_tm_port = tmp;
  /* pp-context*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          regs_val,
          7,
          3,
          &(tmp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  switch(tmp) {
  case SOC_PB_PARSER_PP_CONTEXT_RAW:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_RAW;
    break;

  case SOC_PB_PARSER_PP_CONTEXT_ITMH:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_TM;
    break;

  case SOC_PB_PARSER_PP_CONTEXT_ETH:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_ETH;
    break;

  case SOC_PB_PARSER_PP_CONTEXT_PROG:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_PROG;
    break;

  case SOC_PB_PARSER_PP_CONTEXT_FTMH:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_STACKING;
    break;

  case SOC_PB_PARSER_PP_CONTEXT_INJECTED_EXT_KEY:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_INJECTED;
    break;

  default:
    rcvd_pkt_info->pp_context = SOC_PETRA_PORT_HEADER_TYPE_NONE;
  }

  res = soc_sand_bitstream_get_any_field(
          regs_val,
          21,
          7,
          &(rcvd_pkt_info->packet_header.buff_len)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* hw has size - 1 */
  rcvd_pkt_info->packet_header.buff_len += 1;

  /* header-0*/
  read_size = SOC_PB_PP_DIAG_FLD_LEN - 28;
  res = soc_sand_bitstream_get_any_field(
          regs_val,
          28,
          read_size,
          rcvd_pkt_info->packet_header.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  cur_lsb = read_size;
  /* header-1/2/3*/
  read_size = SOC_PB_PP_DIAG_FLD_LEN;
  for (indx = 1; indx <= 3; ++indx)
  {
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,indx,SOC_PB_PP_DIAG_FLD_MSB_MAX,SOC_PB_PP_DIAG_FLD_LSB_MIN,45);
    res = soc_sand_bitstream_set_any_field(
            regs_val,
            cur_lsb,
            read_size,
            rcvd_pkt_info->packet_header.buff
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    cur_lsb += read_size;
  }
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,4,SOC_PB_PP_DIAG_FLD_MSB_MAX,SOC_PB_PP_DIAG_FLD_LSB_MIN,55);
  res = soc_sand_bitstream_set_any_field(
          regs_val,
          cur_lsb,
          1023-cur_lsb+1,
          rcvd_pkt_info->packet_header.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,5,105,93,65);
  tmp = regs_val[0];
  res = soc_petra_ports_logical_sys_id_parse(
          tmp,
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
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,5,111,106,75);
  rcvd_pkt_info->in_pp_port = regs_val[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_received_packet_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_received_packet_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_received_packet_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns network headers qualified on packet upon
 *     parsing, including packet format, L2 headers fields,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_parsing_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_PARSING_INFO                   *pars_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    act_hdr_index,
    hdr_indx,
    base,
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
    found;
  SOC_PB_PP_PORT_INFO
    port_info;
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO
    tpid_profile_info;
  SOC_PB_PP_LLP_PARSE_TPID_VALUES
    tpid_vals;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PARSING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pars_info);

  SOC_PB_PP_DIAG_PARSING_INFO_clear(pars_info);

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,0,213,208,5);
  tmp = regs_val[0];
  res = soc_pb_pp_pkt_hdr_interanl_val_to_type_map(
          tmp,
          &(pars_info->hdr_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,4,69,35,15);
  qual[0]= regs_val[0];
  qual[1]= regs_val[1];

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,4,112,78,15);
  for (act_hdr_index = 0, hdr_indx = 0 ; hdr_indx < SOC_PB_PP_DIAG_MAX_NOF_HDRS; ++hdr_indx,++act_hdr_index)
  {
    /* set header type */
    pars_info->hdrs_stack[hdr_indx].hdr_type = soc_pb_pp_diag_hdr_at_index(pars_info->hdr_type,hdr_indx);
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_ETH && outer_eth_vld == FALSE)
    {
      outer_eth_indx = hdr_indx;
      outer_eth_vld = TRUE;
    }
    else if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_ETH)
    {
      inner_eth_indx = hdr_indx;
      inner_eth_vld = TRUE;
    }

    /* no more network headers */
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_NONE)
    {
      break;
    }

    /* if not first MPLS */
    if (prev_mpls && pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_MPLS)
    {
      /* previous index + previous (MPLS) header size, which cannot include CW*/
      pars_info->hdrs_stack[hdr_indx].hdr_offset = pars_info->hdrs_stack[hdr_indx-1].hdr_offset + SOC_PB_PP_DIAG_HDR_MPLS_BYTE_SIZE;
      /* actually header index was not used, next time you can use it */
      --act_hdr_index;
    }
    else
    {
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              SOC_PB_PP_DIAG_HDR_INDEX_SIZE*act_hdr_index,
              SOC_PB_PP_DIAG_HDR_INDEX_SIZE,
              &(pars_info->hdrs_stack[hdr_indx].hdr_offset)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      hdr_qual = 0;
      res = soc_sand_bitstream_get_any_field(
              qual,
              SOC_PB_PP_DIAG_HDR_QUAL_SIZE*act_hdr_index,
              SOC_PB_PP_DIAG_HDR_QUAL_SIZE,
              &hdr_qual
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (
          (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_IPV4) ||
          (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_IPV6)
         )
      {
        pars_info->hdrs_stack[hdr_indx].ip.hdr_err = SOC_SAND_GET_BIT(hdr_qual,0);
        pars_info->hdrs_stack[hdr_indx].ip.next_prtcl = SOC_SAND_GET_BITS_RANGE(hdr_qual,4,1);
        pars_info->hdrs_stack[hdr_indx].ip.is_mc = SOC_SAND_GET_BIT(hdr_qual,5);
        pars_info->hdrs_stack[hdr_indx].ip.is_fragmented = SOC_SAND_GET_BIT(hdr_qual,6);
      }
      else if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_MPLS)
      {
        inc_bos = (uint8)SOC_SAND_GET_BIT(hdr_qual,0);
      }
      else if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_ETH)
      {
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.inner_tpid = SOC_SAND_GET_BITS_RANGE(hdr_qual,1,0);
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.is_outer_prio = (uint8)SOC_SAND_GET_BITS_RANGE(hdr_qual,2,2);
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.outer_tpid = SOC_SAND_GET_BITS_RANGE(hdr_qual,4,3);
        pars_info->hdrs_stack[hdr_indx].eth.encap_type = (SOC_SAND_PP_ETH_ENCAP_TYPE)SOC_SAND_GET_BITS_RANGE(hdr_qual,6,5);
      }
    }

    /* set last mpls header index */
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PB_PP_PKT_HDR_TYPE_MPLS)
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
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,4,31,28,15);
    res = soc_pb_pp_l2_next_prtcl_interanl_val_to_type_map(
            unit,
            regs_val[0],
            &(pars_info->hdrs_stack[outer_eth_indx].eth.next_prtcl),
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
     
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,1,53,50,15);
    pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tag_format = (SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT)regs_val[0];

    base = 13;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,2,0,37,13,15);
    res = soc_sand_bitstream_get_any_field(regs_val,13-base,12,&pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].vid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_bitstream_get_any_field(regs_val,26-base,12,&pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].vid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
 /*
  * get TPID info, using driver calls
  */
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,5,111,106,15);
  in_port = regs_val[0];
  
  res = soc_pb_pp_port_info_get_unsafe(
          unit,
          in_port,
          &port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_llp_parse_tpid_profile_info_get_unsafe(
          unit,
          port_info.tpid_profile,
          &tpid_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_llp_parse_tpid_values_get_unsafe(
          unit,
          &tpid_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  if (outer_eth_vld && pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid != SOC_PB_PP_LLP_PARSE_TPID_INDEX_NONE)
  {
    if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_TPID1)
    {
      tmp = tpid_profile_info.tpid1.index;
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tmp];
    }
    else if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_TPID2)
    {
      tmp = tpid_profile_info.tpid2.index;
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tmp];
    }
    else
    {
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[4];
    }
    
  }
  if (inner_eth_vld && pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid != SOC_PB_PP_LLP_PARSE_TPID_INDEX_NONE)
  {
    if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_TPID1)
    {
      tmp = tpid_profile_info.tpid1.index;
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tmp];
    }
    else if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_TPID2)
    {
      tmp = tpid_profile_info.tpid2.index;
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tmp];
    }
    else
    {
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[4];
    }
  }

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_parsing_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_parsing_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PARSING_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_parsing_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns information obtained by termination including
 *     terminated headers
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_termination_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TERM_INFO                   *term_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    lbl_indx,
    frwrd_code = 0,
    qual,
    hdr_index;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TERMINATION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_info);

  SOC_PB_PP_DIAG_TERM_INFO_clear(term_info);

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,0,34,31,10);
  term_info->term_type = (SOC_PB_PP_PKT_TERM_TYPE)regs_val[0];

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,0,4,69,35,20);

  /* check if i-th label was terminated */
  for (lbl_indx=0; lbl_indx <3; ++lbl_indx)
  {
    hdr_index = soc_pb_pp_diag_term_code_to_header_index(term_info->term_type,0);
    if (hdr_index == 0)
    {
      break;
    }
    /* get qualifier at place hdr_index*/
    qual = 0;
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            hdr_index*SOC_PB_PP_DIAG_HDR_QUAL_SIZE,
            SOC_PB_PP_DIAG_HDR_QUAL_SIZE,
            &qual
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
  }

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,1,142,134,5);
  res = soc_sand_bitstream_get_any_field(regs_val,5,4,&frwrd_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  term_info->frwrd_type = frwrd_code;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_termination_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_termination_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TERMINATION_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_termination_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Simulate IP lookup in the device tables and return
 *     FEC-pointer
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_frwrd_lpm_lkup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    reg_val[2];
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lpm_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_ptr);
  SOC_SAND_CHECK_NULL_INPUT(found);


  regs = soc_pb_pp_regs();
  reg_val[0] = lpm_key->key.subnet.ip_address;
  reg_val[1] = lpm_key->vrf;


  SOC_PB_PP_REG_SET(regs->ihb.query_key_reg_0, reg_val[0],20,exit);
  SOC_PB_PP_REG_SET(regs->ihb.query_key_reg_1, reg_val[1],20,exit);

  reg_val[0] = 1;
  SOC_PB_PP_FLD_SET(regs->ihb.query_payload_reg.query_trigger, reg_val[0],20,exit);

  SOC_PB_PP_FLD_GET(regs->ihb.query_payload_reg.query_payload, reg_val[0],20,exit);

  *found = TRUE;
  *fec_ptr = reg_val[0];
 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_frwrd_lpm_lkup_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_frwrd_lpm_lkup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY              *lpm_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_IPV4_VPN_KEY, lpm_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_frwrd_lpm_lkup_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the forwarding lookup performed including:
 *     forwarding type (bridging, routing, ILM, ...), the key
 *     used for the lookup and the result of the lookup
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_frwrd_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_FRWRD_LKUP_INFO                *frwrd_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    dest=0,
    ofst_index=0,
    lbl_index=0,
    frwrd_code=0,
    base,
    vrf,
    mac_long[2],
    asd=0,
    tmp=0,
    diag_flavor;
  SOC_PB_PP_DIAG_MODE_INFO
    diag_mode;
  SOC_PB_PP_FRWRD_ILM_GLBL_INFO
    ilm_glbl;
  uint8
    dummy;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(frwrd_info);

  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_clear(frwrd_info);

  base = 134;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,1,142,134,5);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,139-base,4,&frwrd_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
   
    /* if raw */
  res = soc_pb_pp_sw_db_diag_mode_get(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  diag_flavor = diag_mode.flavor;

  /* if not raw mode get type */
  if ((diag_flavor & SOC_PB_PP_DIAG_FLAVOR_RAW) == 0)
  {
    frwrd_info->frwrd_type = soc_pb_pp_diag_frwrd_type_to_lkup_type_map(frwrd_code);
  }

  res = soc_sand_bitstream_get_any_field(regs_val,134-base,3,&ofst_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_get_any_field(regs_val,137-base,2,&lbl_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  frwrd_info->frwrd_hdr_index = (uint8)(ofst_index + lbl_index);

  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,0,21,14,5);
  vrf = regs_val[0];

  if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC || frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC)
  {
    if (vrf != 0)
    {
      frwrd_info->frwrd_type = SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN;
    }
  }
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,8,0,76,13,5);

  base = 13;
  if (diag_flavor & SOC_PB_PP_DIAG_FLAVOR_RAW)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,13-base,63,frwrd_info->lkup_key.raw);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_MACT)
  {
    frwrd_info->lkup_key.mact.key_type = SOC_PB_PP_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    mac_long[0] = 0;
    mac_long[1] = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,0,48,mac_long);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    res = soc_sand_pp_mac_address_long_to_struct(
            mac_long,
            &(frwrd_info->lkup_key.mact.key_val.mac.mac)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    /* vsi */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,48,14,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    frwrd_info->lkup_key.mact.key_val.mac.fid = tmp;
  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC || frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,32,&frwrd_info->lkup_key.ipv4_uc.key.subnet.ip_address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    frwrd_info->lkup_key.ipv4_uc.key.subnet.prefix_len = 32;
    res = soc_sand_bitstream_get_any_field(regs_val,32,8,&frwrd_info->lkup_key.ipv4_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC)
  {
    base = 0;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,2,0,148,5,5);
    res = soc_sand_bitstream_get_any_field(regs_val,0,28,&frwrd_info->lkup_key.ipv4_mc.group);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    frwrd_info->lkup_key.ipv4_mc.group |= 0xe << 28;
    res = soc_sand_bitstream_get_any_field(regs_val,28,32,&frwrd_info->lkup_key.ipv4_mc.source.ip_address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    frwrd_info->lkup_key.ipv4_mc.source.prefix_len = 32;
    res = soc_sand_bitstream_get_any_field(regs_val,60,12,&frwrd_info->lkup_key.ipv4_mc.inrif);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    frwrd_info->lkup_key.ipv4_mc.inrif_valid = TRUE;
  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC || frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_VPN)
  {
    base = 0;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,2,0,148,5,5);
    res = soc_sand_bitstream_get_any_field(regs_val,0,128,frwrd_info->lkup_key.ipv6_uc.key.subnet.ipv6_address.address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    frwrd_info->lkup_key.ipv6_uc.key.subnet.prefix_len = 132;
    res = soc_sand_bitstream_get_any_field(regs_val,128,8,&frwrd_info->lkup_key.ipv6_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC)
  {
    base = 0;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,2,0,148,5,5);
    res = soc_sand_bitstream_get_any_field(regs_val,0,120,frwrd_info->lkup_key.ipv6_mc.group.address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    /* set MSB to be 0xFF */
    tmp = 0xFF;
    res = soc_sand_bitstream_set_any_field(&tmp,120,8,frwrd_info->lkup_key.ipv6_uc.key.subnet.ipv6_address.address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    /* in-RIF*/
    res = soc_sand_bitstream_get_any_field(regs_val,128,8,&frwrd_info->lkup_key.ipv6_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_ILM)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.in_label);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_bitstream_get_any_field(regs_val,SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM,SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    frwrd_info->lkup_key.ilm.mapped_exp =(uint8)tmp;

    res = soc_pb_pp_frwrd_ilm_glbl_info_get_unsafe(unit,&ilm_glbl);
    if (!ilm_glbl.key_info.mask_port)
    {
      tmp = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM+SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
      res = soc_sand_bitstream_get_any_field(regs_val,tmp,SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.in_local_port);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      tmp += SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
    }
    else
    {
      /* port not in the key, then this is duplication of the label */
      frwrd_info->lkup_key.ilm.in_local_port = 0;
      tmp = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM+SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM+SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
    }

    if (!ilm_glbl.key_info.mask_inrif)
    {
      res = soc_sand_bitstream_get_any_field(regs_val,tmp,SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.inrif);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      frwrd_info->lkup_key.ilm.inrif = 0;
    }
    tmp += SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
  }
  else if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&frwrd_info->lkup_key.trill_uc);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  base = 0;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,9,0,41,0,5);
  /* if raw */
  if (diag_flavor & SOC_PB_PP_DIAG_FLAVOR_RAW)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0-base,41,frwrd_info->lkup_res.raw);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (
            frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC ||
            frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_VPN ||
            frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC ||
            frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC
          )
  {
    base = 0;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,3,0,16,0,5);
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      frwrd_info->key_found = TRUE;

      if (SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC||SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC)
      {
        res = soc_sand_bitstream_get_any_field(regs_val,1,16,&dest);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        asd = 0;

        if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_MACT || frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_BMACT)
        {
          res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
                  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_IP,
                  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
                  dest,
                  asd,
                  &(frwrd_info->lkup_res.mact.frwrd_info.forward_decision),
                  &(frwrd_info->lkup_res.mact.frwrd_info.drop_when_sa_is_known)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        }
      }
      else
      {
        frwrd_info->lkup_res.frwrd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC;
        frwrd_info->lkup_res.frwrd_decision.additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;
        res = soc_sand_bitstream_get_any_field(regs_val,1,16,&frwrd_info->lkup_res.frwrd_decision.dest_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }
    else
    {
      frwrd_info->key_found = FALSE;
    }
  }
    /* lookup in LPM and result is FEC */
  else if (
            frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC ||
            frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN
          )
  {
    /* check maybe host has the answer */

    base = 0;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,9,0,42,0,5);
    /*was match*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,42,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      frwrd_info->key_found = TRUE;
      res = soc_sand_bitstream_get_any_field(regs_val,0,16,&dest);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = soc_sand_bitstream_get_any_field(regs_val,16,24,&asd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = soc_pb_pp_frwrd_em_dest_to_fec(dest,asd,&(frwrd_info->lkup_res.host_info));
      frwrd_info->frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST;
    }
    else
    {
      base = 0;
      SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,5,0,13,0,5);
      /*was match*/
      frwrd_info->key_found = TRUE;
      frwrd_info->lkup_res.frwrd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC;
      frwrd_info->lkup_res.frwrd_decision.additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;
      frwrd_info->lkup_res.frwrd_decision.dest_id = regs_val[0];
    }
  }
  /* lookup in LEM*/
  else
  {
    base = 0;
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,9,0,42,0,5);
    /*was match*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,42,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      frwrd_info->key_found = TRUE;
      res = soc_sand_bitstream_get_any_field(regs_val,0,16,&dest);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = soc_sand_bitstream_get_any_field(regs_val,16,24,&asd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_MACT || frwrd_info->frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_BMACT)
      {
        res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
                SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
                SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
                dest,
                asd,
                &(frwrd_info->lkup_res.mact.frwrd_info.forward_decision),
                &(frwrd_info->lkup_res.mact.frwrd_info.drop_when_sa_is_known)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
      }
      else
      {
        res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
                SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
                SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
                dest,
                asd,
                &(frwrd_info->lkup_res.frwrd_decision),
                &(dummy)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);
      }
    }
    else
    {
      frwrd_info->key_found = FALSE;
    }
  }
  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_frwrd_lkup_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_frwrd_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_frwrd_lkup_info_get_verify()", 0, 0);
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
  soc_pb_pp_diag_traps_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TRAPS_INFO                     *traps_info
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    regs_val[3],
    reg_val,
    tmp,
    nof_bits;
  SOC_PB_PP_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_DIAG_TRAPS_INFO_clear(traps_info);
  
  /* LLR traps */
  SOC_PB_PP_REG_GET(regs->ihp.dbg_llr_trap0_reg,regs_val[0],10,exit);
  SOC_PB_PP_REG_GET(regs->ihp.dbg_llr_trap1_reg,regs_val[1],20,exit);

  nof_bits = SOC_PETRA_FLD_NOF_BITS(regs->ihp.dbg_llr_trap0_reg.dbg_llr_trap0) +
             SOC_PETRA_FLD_NOF_BITS(regs->ihp.dbg_llr_trap1_reg.dbg_llr_trap1);

  res = soc_pb_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          0,
          nof_bits,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* VTT traps */
  SOC_PB_PP_REG_GET(regs->ihp.vtt_trap_encountered1_reg,regs_val[0],40,exit);
  SOC_PB_PP_REG_GET(regs->ihp.vtt_trap_encountered2_reg,regs_val[1],50,exit);

  nof_bits = SOC_PETRA_FLD_NOF_BITS(regs->ihp.vtt_trap_encountered1_reg.vtt_traps_encountered1) +
             SOC_PETRA_FLD_NOF_BITS(regs->ihp.vtt_trap_encountered2_reg.vtt_traps_encountered2);

  res = soc_pb_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          1,
          nof_bits,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  /* FLP traps */
  SOC_PB_PP_REG_GET(regs->ihb.dbg_flp_trap0_reg,regs_val[0],70,exit);
  SOC_PB_PP_REG_GET(regs->ihb.dbg_flp_trap1_reg,regs_val[1],80,exit);
  SOC_PB_PP_REG_GET(regs->ihb.dbg_flp_trap1_reg,regs_val[2],85,exit);

  nof_bits = SOC_PETRA_FLD_NOF_BITS(regs->ihb.dbg_flp_trap0_reg.dbg_flp_trap0) +
             SOC_PETRA_FLD_NOF_BITS(regs->ihb.dbg_flp_trap1_reg.dbg_flp_trap1) +
             SOC_PETRA_FLD_NOF_BITS(regs->ihb.dbg_flp_trap2_reg.dbg_flp_trap2);

  res = soc_pb_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          2,
          nof_bits,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  /* FER traps */
  SOC_PB_PP_REG_GET(regs->ihb.dbg_fer_trap_reg,regs_val[0],100,exit);
  nof_bits = SOC_PETRA_FLD_NOF_BITS(regs->ihb.dbg_fer_trap_reg.dbg_fer_trap);

  res = soc_pb_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          3,
          nof_bits,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

 /*
  * committed Trap
  */
  SOC_PB_PP_REG_GET(regs->ihb.dbg_last_resolved_trap_reg,reg_val,120,exit);
  
  /* is valid */
  tmp = 0;
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_valid,tmp,reg_val,130,exit);
  traps_info->committed_trap.is_pkt_trapped = SOC_SAND_NUM2BOOL(tmp);
  /* get trap Code */
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_cpu_trap_code,tmp,reg_val,140,exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&traps_info->committed_trap.code);
  res = soc_pb_pp_trap_frwrd_profile_info_get_unsafe(
          unit,
          traps_info->committed_trap.code,
          &(traps_info->committed_trap.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  /* update trap strength according HW val */
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_cpu_trap_code_strength,tmp,reg_val,160,exit);
  traps_info->committed_trap.info.strength = tmp;

  /* get snoop Code */
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_snoop_code,tmp,reg_val,170,exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&traps_info->committed_snoop.code);
  res = soc_pb_pp_trap_snoop_profile_info_get_unsafe(
          unit,
          traps_info->committed_snoop.code,
          &(traps_info->committed_snoop.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  /* update snoop strength according HW val */
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_snoop_code_strength,tmp,reg_val,190,exit);
  traps_info->committed_snoop.info.strength = tmp;

 /*
  * committed Trap changed destination
  */
  SOC_PB_PP_REG_GET(regs->ihb.dbg_last_trap_change_destination_reg,reg_val,200,exit);
  
  /* is valid */
  tmp = 0;
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_trap_change_destination_reg.dbg_last_trap_change_destination_valid,tmp,reg_val,210,exit);
  traps_info->trap_updated_dest.is_pkt_trapped = SOC_SAND_NUM2BOOL(tmp);
  /* get trap Code */
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_trap_change_destination_reg.dbg_last_trap_change_destination,tmp,reg_val,220,exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&traps_info->trap_updated_dest.code);
  res = soc_pb_pp_trap_frwrd_profile_info_get_unsafe(
          unit,
          traps_info->trap_updated_dest.code,
          &(traps_info->trap_updated_dest.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  /* clear if this is the mode */
  res = soc_pb_pp_sw_db_diag_mode_get(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

  if (diag_mode.flavor & SOC_PB_PP_DIAG_FLAVOR_CLEAR_ON_GET)
  {
    res = soc_pb_pp_diag_pkt_trace_clear_unsafe(
            unit,
            SOC_PB_PP_DIAG_PKT_TRACE_TRAP
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_traps_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns packet trapped to CPU with additional
 *     information: identifying the reason of trapping (if
 *     any), the source system port, and pointer to the packet
 *     payload.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_trapped_packet_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT uint8                            *buff,
    SOC_SAND_IN  uint32                             max_size,
    SOC_SAND_OUT uint32                             *buff_len,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TRAP_PACKET_INFO    *packet_info
  )
{
  SOC_PETRA_PKT_RX_PACKET_INFO
    *rx_packet=NULL;
  SOC_PETRA_PKT_PACKET_RECV_MODE
    recv_mode;
  uint32
    pkt_start;
  uint32
    res = SOC_SAND_OK;
  soc_pkt_t dnx_pkt;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPPED_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(buff);
  SOC_SAND_CHECK_NULL_INPUT(buff_len);
  SOC_SAND_CHECK_NULL_INPUT(packet_info);

  SOC_PB_PP_DIAG_TRAP_PACKET_INFO_clear(packet_info);

  SOC_PETRA_ALLOC_ANY_SIZE(rx_packet,SOC_PETRA_PKT_RX_PACKET_INFO,1);

  soc_petra_pkt_packet_receive_mode_get_unsafe(&recv_mode);
  if (recv_mode == SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB)
  {
    recv_mode = SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB;
  }
  else
  {
    recv_mode = SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB;
  }
  soc_sand_os_printf("recv_mode: %u\n\r",recv_mode);

  soc_petra_pkt_packet_receive_mode_set_unsafe(recv_mode);
  
  res = soc_petra_pkt_packet_recv_unsafe(
          unit,
          max_size,
          rx_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *buff_len = SOC_SAND_MIN(rx_packet->packet.data_byte_size,max_size);
  if (recv_mode == SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
  {
    pkt_start = SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE - *buff_len;
  }
  else
  {
    pkt_start = 0;
  }
  SOC_PETRA_COPY(buff,rx_packet->packet.data+pkt_start,uint8,*buff_len);


  res = soc_ppd_trap_packet_parse(
          unit,
          buff,
          *buff_len,
          &(packet_info->pkt_info),
          &dnx_pkt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  if (rx_packet != NULL)
  {
    SOC_PETRA_FREE_ANY_SIZE(rx_packet);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_trapped_packet_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_trapped_packet_info_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT uint8                              *buff,
    SOC_SAND_IN  uint32                             max_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPPED_PACKET_INFO_GET_VERIFY);

  /* SOC_PB_PP_STRUCT_VERIFY(uint8, buff, 10, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(max_size, SOC_PB_PP_DIAG_MAX_SIZE_MAX, SOC_PB_PP_DIAG_MAX_SIZE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_trapped_packet_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the destination of all traps to be CPU, so trapped
 *     packet can be captured and examined by CPU
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_traps_all_to_cpu_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO               *trap_info
  )
{
  uint32
    indx,
    dest,
    asd;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  SOC_PB_PP_FRWRD_DECISION_INFO
    frwrd_info;
  uint8
    saved;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(trap_info);
  /* check if there is a need to save status */
  res = soc_pb_pp_sw_db_diag_trap_save_status_get(
          unit,
          &saved
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /* if not already saved, then save now*/
  if (!saved)
  {
    for (indx = 0; indx < SOC_PB_PP_DIAG_NOF_TRAPS; ++ indx)
    {
    /* get table configuration */
      res = soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
              unit,
              indx,
              &fwd_act_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      /* save in SW DB */
      res = soc_pb_pp_sw_db_diag_trap_dest_set(
              unit,
              indx,
              &fwd_act_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&frwrd_info);
  frwrd_info.type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT;
  frwrd_info.dest_id = trap_info->cpu_dest;
  frwrd_info.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;

  res = soc_pb_pp_fwd_decision_in_buffer_build(
          SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS,
          &frwrd_info,
          FALSE,
          &dest,
          &asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* overwrite configuration to send to CPU */
    for (indx = 0; indx < SOC_PB_PP_DIAG_NOF_TRAPS; ++ indx)
    {
    /* get table configuration */
      res = soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
              unit,
              indx,
              &fwd_act_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      fwd_act_profile_tbl_data.fwd_act_destination = dest;
      fwd_act_profile_tbl_data.fwd_act_destination_add_vsi = 0;
      fwd_act_profile_tbl_data.fwd_act_destination_add_vsi_shift =  0;
      fwd_act_profile_tbl_data.fwd_act_destination_valid = 1;

      res = soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
              unit,
              indx,
              &fwd_act_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_all_to_cpu_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_traps_all_to_cpu_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO               *trap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO, trap_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_all_to_cpu_verify()", 0, 0);
}

/*********************************************************************
*     Restore the configuration of all traps, to be as
 *     configured before last calling for
 *     soc_ppd_diag_traps_all_to_cpu()
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_traps_stat_restore_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    indx;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  uint8
    saved;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE_UNSAFE);

  /* check if some status already saved*/
  res = soc_pb_pp_sw_db_diag_trap_save_status_get(
          unit,
          &saved
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /* if not already saved, then save now*/
  if (!saved)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_DIAG_RESTORE_NOT_SAVED_ERR, 20, exit);
  }

  for (indx = 0; indx < SOC_PB_PP_DIAG_NOF_TRAPS; ++ indx)
  {
    /* get table configuration */
    res = soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
            unit,
            indx,
            &fwd_act_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* read from SW DB saved fields*/
    res = soc_pb_pp_sw_db_diag_trap_dest_get(
            unit,
            indx,
            &fwd_act_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
            unit,
            indx,
            &fwd_act_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_stat_restore_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_traps_stat_restore_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_traps_stat_restore_verify()", 0, 0);
}

/*********************************************************************
*     Return the trace (changes) for forwarding decision for
 *     last packet in several phases in processing
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_frwrd_decision_trace_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    dest,
    base,
    out_lif_val=0,
    asd=0,
    frwrd_code=0,
    dest_is_valid=0,
    tmp=0;
  SOC_PB_PP_REGS
    *regs;
  uint8
    dummy;
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE
    app_type;
  SOC_PB_PP_DIAG_FWD_LKUP_TYPE
    frwrd_type;
  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO
    trap_info;
  SOC_PB_PP_TRAP_CODE
    trap_code;
  uint8
    is_fec=FALSE;
  uint32
    reg_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(frwrd_trace_info);

  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO_clear(frwrd_trace_info);
  regs = soc_pb_pp_regs();

  base = 134;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,1,142,134,5);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,139-base,4,&frwrd_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  frwrd_type = soc_pb_pp_diag_frwrd_type_to_lkup_type_map(frwrd_code);

  if (frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_MACT)
  {
    app_type = SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT;
  }
  else if (frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_ILM)
  {
    app_type = SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM;
  }
  else if (frwrd_type == SOC_PB_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC)
  {
    app_type = SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_IP;
  }
  else
  {
    app_type = SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT;
  }

  base = 62;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,2,0,149,62,5);
  frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].is_valid = FALSE;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,62-base,2,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.snoop_action_strength = tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,122-base,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.frwrd_action_strength = tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,142-base,8,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&trap_code);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = trap_code;

  /*LIF forwarding decision*/
  base = 143;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,5,1,210,143,5);
  /* trap */
  tmp = 0;
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.snoop_action_strength = 0;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,143-base,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.frwrd_action_strength = tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,178-base,8,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&trap_code);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.trap_code = trap_code;

  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,146-base,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  dest_is_valid = tmp;
  if (dest_is_valid)
  {
    frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LIF].is_valid = (uint8)tmp;
    dest = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,147-base,17,&dest);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    asd = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,186-base,25,&asd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P,
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
            dest,
            asd,
            &(frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision),
            &(dummy)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
    if (frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision.type == SOC_PB_PP_FRWRD_DECISION_TYPE_FEC)
    {
      is_fec = TRUE;
    }
  }
  /*forwarding lookup forwarding decision*/
  base = 51;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,6,6,163,51,5);
  tmp = 0;
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.snoop_action_strength = 0;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,51-base,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,89-base,8,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&trap_code);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = trap_code;


  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,57-base,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  dest_is_valid |= tmp;
  if (dest_is_valid)
  {
    frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].is_valid = (uint8)tmp;

    dest = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,58-base,17,&dest);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    asd = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,139-base,25,&asd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    tmp = 0;
    res = soc_sand_bitstream_get_any_field(&asd,16,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp ==1 && ((SOC_SAND_GET_BIT(asd,25)==0))) /*outlif-valid*/
    {
       /* Petra b code. Almost not in use. Ignore coverity defects */
      /* coverity[returned_value] */
      res = soc_sand_bitstream_get_any_field(&asd,0,16,&out_lif_val);
      asd = (3<<18) | out_lif_val; /* convert outlif-or-eei to ASD */
    }

    /* check if this is ILM or MACT */
    res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
            app_type,
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
            dest,
            asd,
            &(frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision),
            &(dummy)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
    if (frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.type == SOC_PB_PP_FRWRD_DECISION_TYPE_FEC)
    {
      is_fec = TRUE;
    }
  }
  /*PMF forwarding decision*/
  base = 16;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,8,10,147,16,5);
  /* trap */
  tmp = 0;
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_PMF].action_profile.snoop_action_strength = 0;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_PMF].action_profile.frwrd_action_strength = tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,85,8,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&trap_code);
  frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_PMF].action_profile.trap_code = trap_code;

  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,3,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  dest_is_valid |= tmp;
  if (dest_is_valid)
  {
    frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_PMF].is_valid = (uint8)tmp;

    dest = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,4,17,&dest);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    asd = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,124-base,24,&asd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
            app_type,
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
            dest,
            asd,
            &(frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision),
            &(dummy)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
    if (frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.type == SOC_PB_PP_FRWRD_DECISION_TYPE_FEC)
    {
      is_fec = TRUE;
    }
  }

  /*Trap forwarding decision*/
  /* is valid */
  tmp = 0;
  SOC_PB_PP_REG_GET(regs->ihb.dbg_last_trap_change_destination_reg,reg_val,115,exit);
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_trap_change_destination_reg.dbg_last_trap_change_destination_valid,tmp,reg_val,210,exit);
  dest_is_valid |= tmp;
  if (dest_is_valid)
  {
    frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_TRAP].is_valid = (uint8)tmp;
    /* get trap Code */
    if (tmp)
    {
      SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_trap_change_destination_reg.dbg_last_trap_change_destination,tmp,reg_val,220,exit);
    }
    else
    {
      SOC_PB_PP_REG_GET(regs->ihb.dbg_last_resolved_trap_reg,reg_val,115,exit);
      SOC_PB_PP_FLD_FROM_REG(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_cpu_trap_code,tmp,reg_val,210,exit);
    }

    soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(unit,tmp,&trap_code);
    frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_TRAP].action_profile.trap_code = trap_code;
    res = soc_pb_pp_trap_frwrd_profile_info_get_unsafe(
            unit,
            trap_code,
            &(trap_info)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

    SOC_PB_PP_COPY(
      &(frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision),
      &(trap_info.dest_info.frwrd_dest),
      SOC_PB_PP_FRWRD_DECISION_INFO,
      1
    );

    SOC_PB_PP_FLD_GET(regs->ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_cpu_trap_code_strength,reg_val,115,exit);
    frwrd_trace_info->trap[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_TRAP].action_profile.frwrd_action_strength = reg_val;
    
  }

  /*FEC forwarding decision*/
  base = 145;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,12,1,255,145,5);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,3,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  dest_is_valid |= tmp;
  if (dest_is_valid)
  {
    frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_FEC].is_valid = (uint8)tmp;

    dest = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,4,17,&dest);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    asd = 0;
    /* outlif */
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,12,0,153,138,5);
    asd = regs_val[0] ;

    res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
            (is_fec)?SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC:app_type,
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
            dest,
            asd,
            &(frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_FEC].frwrd_decision),
            &(dummy)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);
  }


  /* last_resolved Destination*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,14,0,247,0,5);

  /* destination*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,87,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  dest_is_valid |= tmp;
  if (dest_is_valid)
  {
    frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].is_valid = (uint8)tmp;
 
    /* parse dest*/
    dest = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,101,16,&dest);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    asd = 0;
    res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_16,
            dest,
            asd,
            &(frwrd_trace_info->frwrd[SOC_PB_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision),
            &(dummy)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_frwrd_decision_trace_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_frwrd_decision_trace_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_frwrd_decision_trace_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the information to be learned for the incoming
 *     packet. This is the information that the processing
 *     determine to be learned, the MACT supposed to learn this
 *     information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_learning_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEARN_INFO                     *learn_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    dest,
    asd,
    mac_long[2],
    tmp=0;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_LEARNING_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(learn_info);

  SOC_PB_PP_DIAG_LEARN_INFO_clear(learn_info);


  /* learning-info*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,12,0,255,160,5);

  /* learn ingress/egress*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp)
  {
    learn_info->ingress = TRUE;
  }
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,1,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
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
  /* sa */
  mac_long[0] = 0;
  mac_long[1] = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,2,48,mac_long);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  learn_info->key.key_type = SOC_PB_PP_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          mac_long,
          &(learn_info->key.key_val.mac.mac)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* vsi */
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,50,14,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  learn_info->key.key_val.mac.fid = tmp;
  /* destination */
  dest = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,65,16,&dest);/* 64 for DB-ID*/
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* ASD */
  asd = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,81,15,&asd);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* learning-info-rest*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,12,1,9,0,5);
  res = soc_sand_bitstream_get_any_field(regs_val,0,9,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  asd = asd | (tmp<<15);
  /*
  if (asd != 0)
  {
    reg_val = 0;
    SOC_PB_PP_REG_GET(pp_regs->ihp.asd_ac_prefix_reg, reg_val, 10, exit);
    asd |= SOC_SAND_SET_BITS_RANGE(reg_val,23,14)  ;
  }
*/
  res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP,
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
          dest,
          asd,
          &(learn_info->value.frwrd_info.forward_decision),
          &(learn_info->value.frwrd_info.drop_when_sa_is_known)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);
  learn_info->value.aging_info.is_dynamic = TRUE;
  learn_info->value.aging_info.age_status = 6;

  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,9,1,&tmp);
  learn_info->is_new_key = SOC_SAND_NUM2BOOL(!tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_learning_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_learning_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_LEARNING_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_learning_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the result of ingress vlan editing,
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_ing_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_VLAN_EDIT_RES                  *vec_res
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vec_res);

  SOC_PB_PP_DIAG_VLAN_EDIT_RES_clear(vec_res);

  /* vlan-edit-info*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,12,0,35,14,5);
  /* AC-tag info*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,12,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->ac_tag.vid = (SOC_SAND_PP_VLAN_ID)tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,12,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->ac_tag.dei = (SOC_SAND_PP_DEI_CFI)tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,13,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->ac_tag.pcp = (SOC_SAND_PP_PCP_UP)tmp;
  /* edit id*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,16,6,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->cmd_id = tmp;
  /* edit info*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,16,6,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->cmd_id = tmp;

  res = soc_pb_pp_lif_ing_vlan_edit_command_info_get_unsafe(
          unit,
          vec_res->cmd_id,
          &vec_res->cmd_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_ing_vlan_edit_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_ing_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_ing_vlan_edit_info_get_verify()", 0, 0);
}

/*********************************************************************
*     The Traffic management information associated with the
 *     packet including meter, DP, TC, etc...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_pkt_associated_tm_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_PKT_TM_INFO                    *pkt_tm_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    dest,
    asd=0,
    tmp=0;
  uint8
    dummy;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pkt_tm_info);

  SOC_PB_PP_DIAG_PKT_TM_INFO_clear(pkt_tm_info);


  /* tm-forwarding-info*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHB_ID,14,0,247,0,5);

  /* destination*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,87,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_DEST;
    /* parse dest*/
    dest = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,101,16,&dest);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    asd = 0;
    res = soc_pb_pp_diag_fwd_decision_in_buffer_parse(
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_16,
            dest,
            asd,
            &(pkt_tm_info->frwrd_decision),
            &(dummy)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  /*TC*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_TC;
    /* parse tc*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,98,3,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    pkt_tm_info->tc = (SOC_SAND_PP_TC)tmp;
  }

  /*DP*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_DP;
    /* parse dp*/
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,96,2,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    pkt_tm_info->dp = (SOC_SAND_PP_DP)tmp;
  }

  /*METER1*/
  tmp = 0;
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_METER1;
    res = soc_sand_bitstream_get_any_field(regs_val,164,13,&pkt_tm_info->meter1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  /*METER2*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_METER2;
    res = soc_sand_bitstream_get_any_field(regs_val,177,13,&pkt_tm_info->meter2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  /*METER-command*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_MTR_CMD;
    res = soc_sand_bitstream_get_any_field(regs_val,244,2,&pkt_tm_info->dp_meter_cmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  /*COUNTER1*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_COUNTER1;
    res = soc_sand_bitstream_get_any_field(regs_val,190,12,&pkt_tm_info->counter1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  /*COUNTER2*/
  tmp = 1;/*always valid*/
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PB_PP_DIAG_PKT_TM_FIELD_COUNTER2;
    res = soc_sand_bitstream_get_any_field(regs_val,202,12,&pkt_tm_info->counter2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_pkt_associated_tm_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_pkt_associated_tm_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_pkt_associated_tm_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the encapsulation and editing information applied to
 *     last packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_encap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_ENCAP_INFO                     *encap_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    tmp=0,
    base,
    tmp2=0,
    tmp3;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;
  SOC_PB_PP_EG_ENCAP_EEP_TYPE
    eep_type;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_ENCAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  SOC_PB_PP_DIAG_ENCAP_INFO_clear(encap_info);

  base = 188;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_EPNI_ID,0,5,227,188,5);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,227-base,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp)
  {
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,202-base,24,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_bitstream_get_any_field(&tmp,0,20,&encap_info->mpls_cmd.label);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_bitstream_get_any_field(&tmp,20,4,&tmp2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    encap_info->mpls_cmd.command = (SOC_PB_PP_MPLS_COMMAND_TYPE)tmp2;

    if (encap_info->mpls_cmd.command < SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE)
    {
      encap_info->mpls_cmd.push_profile = encap_info->mpls_cmd.command;
      encap_info->mpls_cmd.command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;
    }
  }
  res = soc_sand_bitstream_get_any_field(regs_val,226-base,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  base = 198;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_EGQ_ID,0,0,213,198,5);

  tmp =SOC_SAND_GET_BITS_RANGE(regs_val[0],15,14);
  tmp2 =SOC_SAND_GET_BITS_RANGE(regs_val[0],13,0);
  switch(tmp)
  {
  case 2:/* out-AC */
    encap_info->out_ac = tmp2;
  break;
  case 0: /* out-EEP */
    encap_info->eep[0] = tmp2;
  break;
  default:
   break;
  }
  /* get range */
  res = soc_pb_pp_eg_encap_range_info_get(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (range_info.ll_limit >= encap_info->eep[0])
  {
    eep_type = SOC_PB_PP_EG_ENCAP_EEP_TYPE_LL;
  }
  else if (range_info.ip_tnl_limit >= encap_info->eep[0])
  {
    eep_type = SOC_PB_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP;
  }
  else
  {
    eep_type = SOC_PB_PP_EG_ENCAP_EEP_TYPE_LIF_EEP;
  }

  res = soc_pb_pp_eg_encap_entry_get(
          unit,
          eep_type,
          encap_info->eep[0],
          2,
          encap_info->encap_info,
          &(encap_info->eep[1]),
          &tmp2
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  encap_info->nof_eeps = tmp2;
 

  base = 29;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_EPNI_ID,0,7,58,29,5);
  tmp = 0;

  res = soc_sand_bitstream_get_any_field(regs_val,45-base,14,&tmp);
  encap_info->ll_vsi = tmp;

  res = soc_sand_bitstream_get_any_field(regs_val,29-base,16,&tmp3);
  tmp =SOC_SAND_GET_BITS_RANGE(tmp3,15,14);
  tmp2 =SOC_SAND_GET_BITS_RANGE(tmp3,13,0);
  switch(tmp)
  {
  case 2:/* out-AC */
    encap_info->out_ac = tmp2;
    break;
  case 0: /* out-EEP */
  default:
    break;
  }
  /* out ports */
  base = 499;
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_EPNI_ID,0,6,511,499,5);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,505-base,7,&tmp);
  encap_info->tm_port = tmp;

  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,499-base,6,&tmp);
  encap_info->pp_port = tmp;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_encap_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_encap_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_ENCAP_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_encap_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the reason for packet discard
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_eg_drop_log_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  )
{
  uint32
    reg_val;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_EG_DROP_LOG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_drop_log);

  SOC_PB_PP_DIAG_EG_DROP_LOG_INFO_clear(eg_drop_log);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->egq.erpp_discard_interrupt_reg,reg_val,10,exit);
  /* as assuming same order for enum and register */
  eg_drop_log->drop_log[0] = reg_val;

  /* check if to clear after read */
  res = soc_pb_pp_sw_db_diag_mode_get(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & SOC_PB_PP_DIAG_FLAVOR_CLEAR_ON_GET)
  {
    SOC_PB_REG_SET(regs->egq.erpp_discard_interrupt_reg,0xFFFFF,20,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_eg_drop_log_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_eg_drop_log_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_EG_DROP_LOG_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_eg_drop_log_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the LIF DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_db_lif_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LIF_LKUP_INFO                  *lkup_info
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    /*dest=0,*/
    /*ofst_index=0,*/
    /*lbl_index=0,*/
    /*frwrd_code=0,*/
    db_id,
    /*vrf,*/
    /*mac_long[2],*/
    raw_key[6]={0},
    lif_index,
    /*asd=0,*/
    tmp=0;
  SOC_PB_PP_DIAG_MODE_INFO
    diag_mode;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_res;
  /*uint8
    dummy;*/
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA
    ihp_sem_result_table_tbl;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA
    ihp_sem_result_table_label_vrl_tbl;
  uint8
    found;
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO          pwe_additional_info; /* TO DO: pass this as an argument */
  SOC_PPC_RIF_INFO dummy;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE);

  SOC_PPD_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  SOC_PB_PP_DIAG_LIF_LKUP_INFO_clear(lkup_info);

  /*key*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,6,0,67,0,5);

  /* raw-key and data-base ID/Type */
  if (db_info->lkup_num == 1)/*second*/
  {
    res = soc_sand_bitstream_get_any_field(regs_val,30,4,&db_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_bitstream_get_any_field(regs_val,0,34,raw_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else/*first*/
  {
    res = soc_sand_bitstream_get_any_field(regs_val,64,4,&db_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_bitstream_get_any_field(regs_val,34,34,raw_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  /* key for lookup*/

  /* diag mode */
  res = soc_pb_pp_sw_db_diag_mode_get(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & SOC_PB_PP_DIAG_FLAVOR_RAW)
  {
    SOC_PB_PP_COPY(lkup_info->key.raw,raw_key,uint32,2);
  }
  else
  {
    /* fill key as struct from buffer */
    res = soc_pb_pp_isem_access_key_from_buffer(
            unit,
            raw_key,
            &isem_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    switch(isem_key.key_type)
    {
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC;
      lkup_info->key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      lkup_info->key.ac.key_type = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT;
      lkup_info->key.ac.outer_vid = SOC_PB_PP_LIF_IGNORE_OUTER_VID;
      lkup_info->key.ac.inner_vid = SOC_PB_PP_LIF_IGNORE_INNER_VID;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC;
      lkup_info->key.ac.key_type = (isem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID)?SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN:SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
      lkup_info->key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      lkup_info->key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      lkup_info->key.ac.inner_vid = SOC_PB_PP_LIF_IGNORE_INNER_VID;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC;
      lkup_info->key.ac.key_type = (isem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID)?SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN:SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN;;
      lkup_info->key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      lkup_info->key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      lkup_info->key.ac.inner_vid = isem_key.key_info.l2_eth.inner_vid;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID;
      lkup_info->key.mim.isid_id = isem_key.key_info.pbb.isid;
      lkup_info->key.mim.isid_domain = isem_key.key_info.pbb.isid_domain;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      lkup_info->key.mpls.label_id = isem_key.key_info.mpls.label;
      lkup_info->key.mpls.vsid = isem_key.key_info.mpls.in_rif;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      lkup_info->key.mpls.label_id = isem_key.key_info.mpls.label;
      lkup_info->key.mpls.vsid = 0;
/* $Id: pb_pp_diag.c,v 1.18 Broadcom SDK $
case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_PWE;
lkup_info->key.pwe = isem_key.key_info.mpls.label;
*/
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL;
      lkup_info->key.ip_tunnel.dip = isem_key.key_info.ip_tunnel.dip;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL:
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_TRILL;
      lkup_info->key.nick_name = isem_key.key_info.trill.nick_name;
    break;
    default:
      break;
    }

  }

  /* now get lookup result */
  if (db_info->lkup_num == 1)/*second*/
  {
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,7,0,16,0,5);
  }
  else
  {
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,7,0,33,17,5);
  }

  /* key lookup found?*/
  res = soc_sand_bitstream_get_any_field(regs_val,16,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  lkup_info->found = SOC_SAND_NUM2BOOL(tmp);
  if (!lkup_info->found)
  {
    goto exit;
  }

  res = soc_pb_pp_isem_access_entry_from_buffer(
          regs_val,
          &isem_res
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* get result: index and opcode */
  lkup_info->opcode_id = isem_res.sem_op_code;
  lkup_info->base_index = isem_res.sem_result_ndx;

  /* get result val*/
  if (diag_mode.flavor & SOC_PB_PP_DIAG_FLAVOR_RAW)
  {
    res = soc_pb_pp_ihp_sem_result_table_tbl_get_unsafe(
            unit,
            lkup_info->base_index,
            &ihp_sem_result_table_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PB_PP_COPY(lkup_info->value.raw,ihp_sem_result_table_tbl.sem_result_table,uint32,2);
  }
  else
  {
   /*
    * check whether this is mpls termination or pwe
    */
    res = soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_get_unsafe(
            unit,
            lkup_info->base_index,
            &ihp_sem_result_table_label_vrl_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (
        (ihp_sem_result_table_label_vrl_tbl.type == 0x0 || /* PWE P2P*/
        ihp_sem_result_table_label_vrl_tbl.type == 0x1) && /* PWE MP */
        (lkup_info->type == SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL) /* thought as MPLS */
      )
    {
      lkup_info->type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_PWE;
      lkup_info->key.pwe = lkup_info->key.mpls.label_id;
    }
    switch(lkup_info->type)
    {
    case SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC:
      res = soc_pb_pp_l2_lif_ac_get_unsafe(
              unit,
              &(lkup_info->key.ac),
              &lif_index,
              &(lkup_info->value.ac),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_DIAG_LIF_LKUP_TYPE_PWE:
      res = soc_pb_pp_l2_lif_pwe_get_unsafe(
              unit,
              lkup_info->key.pwe,
              &lif_index,
			  &pwe_additional_info,
              &(lkup_info->value.pwe),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
      res = soc_pb_pp_rif_mpls_label_map_get_unsafe(
              unit,
              &(lkup_info->key.mpls),
              &lif_index,
              &(lkup_info->value.mpls),
              &dummy,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
      res = soc_pb_pp_rif_ip_tunnel_map_get_unsafe(
              unit,
              lkup_info->key.ip_tunnel.dip,
              &lif_index,
              &(lkup_info->value.ip),
              &dummy,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_DIAG_LIF_LKUP_TYPE_TRILL:
    case SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID:
      
    default:
      break;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_db_lif_lkup_info_get_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_diag_db_lif_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_DB_USE_INFO, db_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_db_lif_lkup_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the LEM DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_db_lem_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                                 *valid
  )
{
  SOC_PB_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  SOC_PB_PP_DIAG_MODE_INFO
    diag_mode;
  SOC_PB_PP_LEM_ACCESS_KEY
    lem_key_str;
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED
    lem_key_buf;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    lem_payload_str;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  uint32
    key_type=0;
  uint32
    lem_payload_buf[SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S] = {0};
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);

  SOC_PB_PP_DIAG_LEM_KEY_clear(key);
  SOC_PB_PP_DIAG_LEM_VALUE_clear(val);
  SOC_PB_PP_LEM_ACCESS_KEY_clear(&lem_key_str);

  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key_buf);

  /*key*/
  SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,8,0,153,0,5);
  
  *valid = FALSE;

  if (db_info->lkup_num == 0)
  {
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            13,
            SOC_DPP_LEM_WIDTH_PETRAB,
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* is found*/
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,9,0,85,0,5);
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,42,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      *valid = TRUE;
      /* payload */
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              0,
              41,
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  else if (db_info->lkup_num == 1)
  {
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            90,
            SOC_DPP_LEM_WIDTH_PETRAB,
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    /* is found*/
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,9,0,85,0,5);
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,85,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      *valid = TRUE;
      /* payload */
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              43,
              41,
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  else if (db_info->lkup_num == 2)
  {
    /*key*/
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,3,0,75,0,5);
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            13,/*llr2mact_key*/
            SOC_DPP_LEM_WIDTH_PETRAB,
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* is found*/
    SOC_PB_PP_DIAG_FLD_READ(&fld,SOC_PB_IHP_ID,4,0,42,0,5);
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,42,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      *valid = TRUE;
      /* payload */
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              0,/*mact2llr_payload*/
              41,
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

  }


 /*
  * if raw
  */
  res = soc_pb_pp_sw_db_diag_mode_get(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & SOC_PB_PP_DIAG_FLAVOR_RAW)
  {
    SOC_PB_PP_COPY(key->raw, lem_key_buf.buffer, uint32, SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S);
    SOC_PB_PP_COPY(val->raw, lem_payload_buf, uint32, SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
    goto exit;
  }

  res = soc_pb_pp_lem_key_encoded_parse(
          unit,
          &lem_key_buf,
          &lem_key_str
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*valid)
  {
    res = soc_pb_pp_lem_access_payload_parse(
            unit,
            lem_payload_buf,
            &lem_payload_str
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (lem_key_str.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC)
  {
    *type = SOC_PB_PP_DIAG_LEM_LKUP_TYPE_MACT;
    res = soc_pb_pp_frwrd_mact_key_parse(
            unit,
            &lem_key_str,
            &(key->mact)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }
    res = soc_pb_pp_frwrd_mact_payload_convert(
            unit,
            &lem_payload_str,
            &(val->mact)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM)
  {
    *type = SOC_PB_PP_DIAG_LEM_LKUP_TYPE_ILM;
    /*res = */soc_pb_pp_frwrd_ilm_lem_key_parse(
            &lem_key_str,
            &(key->ilm)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }

    /*res = */soc_pb_pp_frwrd_ilm_lem_payload_parse(
            &lem_payload_str,
            &(val->ilm)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH)
  {
    *type  = SOC_PB_PP_DIAG_LEM_LKUP_TYPE_SA_AUTH;
    res = soc_pb_pp_sa_based_key_from_lem_key_map(
            unit,
            &lem_key_str,
            &(key->sa_auth)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }
    res = soc_pb_pp_sa_based_payload_from_lem_payload_map(
            unit,
            &lem_payload_str,
            &(val->sa_auth)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* get VID assignment info */
    res = soc_pb_pp_lem_access_sa_based_asd_parse(
          unit,
          lem_payload_str.asd,
          NULL,
          &(val->sa_vid_assign)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else if (lem_key_str.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST)
  {
    *type  = SOC_PB_PP_DIAG_LEM_LKUP_TYPE_HOST;

    key->host.subnet.subnet.ip_address = lem_key_str.param[0].value[0];
    key->host.subnet.subnet.prefix_len = 32;
    key->host.vrf_id = lem_key_str.param[1].value[0];

    if (!*valid)
    {
      goto exit;
    }

    /*res = */soc_pb_pp_frwrd_em_dest_to_fec(
            lem_payload_str.dest,
            lem_payload_str.asd,
            &(val->host)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED)
  {
    *type  = SOC_PB_PP_DIAG_LEM_LKUP_TYPE_EXTEND_P2P;
    res = soc_pb_pp_isem_access_key_from_buffer(unit,lem_key_str.param[0].value,&isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    switch(isem_key.key_type)
    {
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.key_type = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT;
      key->extend_p2p_key.key.ac.outer_vid = SOC_PB_PP_LIF_IGNORE_OUTER_VID;
      key->extend_p2p_key.key.ac.inner_vid = SOC_PB_PP_LIF_IGNORE_INNER_VID;
      key_type = SOC_PB_PP_FRWRD_P2P_KEY_TYPE_AC;
      break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.key_type = (isem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID)?SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN:SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      key->extend_p2p_key.key.ac.inner_vid = SOC_PB_PP_LIF_IGNORE_INNER_VID;
      key_type = SOC_PB_PP_FRWRD_P2P_KEY_TYPE_AC;
      break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.key_type = (isem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID)?SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN:SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN;;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      key->extend_p2p_key.key.ac.inner_vid = isem_key.key_info.l2_eth.inner_vid;
      key_type = SOC_PB_PP_FRWRD_P2P_KEY_TYPE_AC;
    break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID;
      key->extend_p2p_key.key.mim.isid_id = isem_key.key_info.pbb.isid;
      key->extend_p2p_key.key.mim.isid_domain = isem_key.key_info.pbb.isid_domain;
      break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      key->extend_p2p_key.key.mpls.label_id = isem_key.key_info.mpls.label;
      key->extend_p2p_key.key.mpls.vsid = isem_key.key_info.mpls.in_rif;
      key_type = SOC_PB_PP_FRWRD_P2P_KEY_TYPE_MIM;
      break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      key->extend_p2p_key.key.mpls.label_id = isem_key.key_info.mpls.label;
      key->extend_p2p_key.key.mpls.vsid = 0;
      key_type = SOC_PB_PP_FRWRD_P2P_KEY_TYPE_PWE;
      break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL;
      key->extend_p2p_key.key.ip_tunnel.dip = isem_key.key_info.ip_tunnel.dip;
      break;
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL:
      key->extend_p2p_key.type = SOC_PB_PP_DIAG_LIF_LKUP_TYPE_TRILL;
      key->extend_p2p_key.key.nick_name = isem_key.key_info.trill.nick_name;
      break;
    default:
      break;
    }
    if (!*valid)
    {
      goto exit;
    }

    res = soc_pb_pp_frwrd_p2p_info_from_lem_buffer(
      unit,
      key_type,
      &(val->extend_p2p),
      &lem_payload_str
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC)
  {
	  *type = SOC_PB_PP_DIAG_LEM_LKUP_TYPE_BMACT;
	  res = soc_pb_pp_frwrd_bmact_key_parse(
		  unit,
		  &lem_key_str,
		  &(key->bmact)
		  );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  if (!*valid)
	  {
		  goto exit;
	  }
	  res = soc_pb_pp_frwrd_bmact_payload_convert(
		  unit,
		  &lem_payload_str,
		  &(val->bmact)
		  );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    SOC_PB_PP_COPY(key->raw, lem_key_buf.buffer, uint32, SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S);
    SOC_PB_PP_COPY(val->raw, lem_payload_buf, uint32, SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
  }


  
      

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_db_lem_lkup_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_db_lem_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_DB_USE_INFO, db_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_db_lem_lkup_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the TCAM DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_db_tcam_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO                  *use_info,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TCAM_KEY                       *key,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TCAM_VALUE                     *val,
    SOC_SAND_OUT uint8                                 *valid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(use_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);

  SOC_PB_PP_DIAG_TCAM_KEY_clear(key);
  SOC_PB_PP_DIAG_TCAM_VALUE_clear(val);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_db_tcam_lkup_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_db_tcam_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO                  *use_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_TCAM_USE_INFO, use_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_db_tcam_lkup_info_get_verify()", 0, 0);
}

#define SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_BITS (2*SOC_SAND_PP_MAC_ADDRESS_NOF_BITS + SOC_SAND_PP_TPID_NOF_BITS + SOC_SAND_PP_UP_NOF_BITS + SOC_SAND_PP_VID_NOF_BITS + SOC_SAND_PP_CFI_NOF_BITS + SOC_SAND_PP_ETHER_TYPE_NOF_BITS)
#define SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_U32  SOC_SAND_DIV_ROUND_UP(SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_BITS,32)

STATIC uint32
  soc_pb_pp_diag_pkt_eth_header_build(
    SOC_SAND_IN   SOC_PB_PP_DIAG_ETH_PACKET_INFO     *header_info,
    SOC_SAND_IN   uint32                       start_bit_msb,
    SOC_SAND_OUT  uint8                        *header_buffer

  )
{
  uint32
    long_buf[SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_U32];
  uint32
    mac_long[2],
    copied_bits = 0,
    tmp,
    start_bit;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_ETH_HEADER_BUILD);

  SOC_PB_PP_CLEAR(long_buf,uint32,SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_U32);
  
  start_bit = SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_U32 * 32;
  copied_bits = start_bit;

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          &(header_info->da),
          mac_long
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  start_bit -= SOC_SAND_PP_MAC_ADDRESS_NOF_BITS;
  res = soc_sand_bitstream_set_any_field(
          mac_long,
          start_bit,
          SOC_SAND_PP_MAC_ADDRESS_NOF_BITS,
          long_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          &(header_info->sa),
          mac_long
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  start_bit -= SOC_SAND_PP_MAC_ADDRESS_NOF_BITS;
  res = soc_sand_bitstream_set_any_field(
          mac_long,
          start_bit,
          SOC_SAND_PP_MAC_ADDRESS_NOF_BITS,
          long_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

 /*
  * if Ethernet header include VLAN tag then set it.
  */
  if (header_info->is_tagged)
  {
    start_bit -= SOC_SAND_PP_TPID_NOF_BITS;
    tmp = header_info->tag.tpid;
    res = soc_sand_bitstream_set_any_field(
            &tmp,
            start_bit,
            SOC_SAND_PP_TPID_NOF_BITS,
            long_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    tmp = header_info->tag.pcp;

    start_bit -= SOC_SAND_PP_UP_NOF_BITS;
    res = soc_sand_bitstream_set_any_field(
            &tmp,
            start_bit,
            SOC_SAND_PP_UP_NOF_BITS,
            long_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);


    tmp = header_info->tag.dei;
    start_bit -= SOC_SAND_PP_CFI_NOF_BITS;
    res = soc_sand_bitstream_set_any_field(
            &tmp,
            start_bit,
            SOC_SAND_PP_CFI_NOF_BITS,
            long_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    tmp = header_info->tag.vid;
    start_bit -= SOC_SAND_PP_VID_NOF_BITS;
    res = soc_sand_bitstream_set_any_field(
            &tmp,
            start_bit,
            SOC_SAND_PP_VID_NOF_BITS,
            long_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

  tmp = header_info->ether_type;
  start_bit -= SOC_SAND_PP_ETHER_TYPE_NOF_BITS;
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          start_bit,
          SOC_SAND_PP_ETHER_TYPE_NOF_BITS,
          long_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  copied_bits -= start_bit;

  soc_sand_U32_to_U8(
    long_buf,
    SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_U32 * 4,
    header_buffer + (start_bit_msb/8) - (SOC_PB_PP_DIAG_ETH_HDR_MAX_NOF_U32 * 4)
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_diag_pkt_eth_header_build()",0,0);
}



/*********************************************************************
*     Inject packets from CPU port to device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_diag_pkt_send_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO                    *pkt_info
  )
{
  SOC_PETRA_PKT_TX_PACKET_INFO
    *tx_packet = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_SEND_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pkt_info);

  SOC_PETRA_ALLOC(tx_packet,SOC_PETRA_PKT_TX_PACKET_INFO,1);
  SOC_PETRA_CLEAR(tx_packet->packet.data,uint8,SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE);
  
  tx_packet->packet.data_byte_size = pkt_info->total_size;
  tx_packet->path_type = SOC_TMC_PACKET_SEND_PATH_TYPE_INGRESS;

  res = soc_pb_pp_diag_pkt_eth_header_build(
            &(pkt_info->ether_info),
            SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE * SOC_SAND_NOF_BITS_IN_BYTE,
            tx_packet->packet.data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_pkt_packet_send_unsafe(
          unit,
          tx_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  if (tx_packet != NULL)
  {
    SOC_PETRA_FREE(tx_packet);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_pkt_send_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_diag_pkt_send_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO                    *pkt_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_DIAG_PKT_SEND_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_PACKET_INFO, pkt_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_diag_pkt_send_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_diag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_diag_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_diag;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_diag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_diag_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_diag;
}
uint32
  SOC_PB_PP_DIAG_BUFFER_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_DIAG_BUFF_MAX_SIZE; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->buff[ind], SOC_PB_PP_DIAG_BUFF_MAX, SOC_PB_PP_DIAG_BUFF_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->buff_len, SOC_PB_PP_DIAG_BUFF_LEN_MAX, SOC_PB_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_BUFFER_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->flavor, SOC_PB_PP_DIAG_FLAVOR_MAX, SOC_PB_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_MODE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_DB_USE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->lkup_num, SOC_PB_PP_DIAG_LKUP_NUM_MAX, SOC_PB_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_DB_USE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TCAM_USE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->lkup_num, SOC_PB_PP_DIAG_LKUP_NUM_MAX, SOC_PB_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lkup_usage, SOC_PB_PP_DIAG_LKUP_USAGE_MAX, SOC_PB_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TCAM_USE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_BUFFER, &(info->packet_header), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_tm_port, SOC_PB_PP_DIAG_IN_TM_PORT_MAX, SOC_PB_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_pp_port, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pp_context, SOC_PB_PP_DIAG_PP_CONTEXT_MAX, SOC_PB_PP_DIAG_PP_CONTEXT_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->packet_qual, SOC_PB_PP_DIAG_PACKET_QUAL_MAX, SOC_PB_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TRAP_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_PACKET_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_PACKET_INFO, &(info->pkt_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TRAP_PACKET_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TRAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->code, SOC_PB_PP_DIAG_CODE_MAX, SOC_PB_PP_DIAG_CODE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO, &(info->info), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TRAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_SNOOP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_SNOOP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->code, SOC_PB_PP_DIAG_CODE_MAX, SOC_PB_PP_DIAG_CODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO, &(info->info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_SNOOP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TRAPS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAPS_INFO *info
  )
{


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TRAPS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_dest, SOC_PB_PP_DIAG_CPU_DEST_MAX, SOC_PB_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_LEM_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LEM_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_KEY, &(info->mact), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_KEY, &(info->bmact), 11, exit);
  /*SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_IPV4_VPN_ROUTE_KEY, &(info->host), 12, exit);*/
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_ILM_KEY, &(info->ilm), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_LEM_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_LIF_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LIF_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY, &(info->ac), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pwe, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_LABEL_RIF_KEY, &(info->mpls), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ip_tunnel.dip, SOC_PB_PP_DIAG_IP_MAX, SOC_PB_PP_DIAG_IP_OUT_OF_RANGE_ERR, 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_KEY, &(info->mim), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_LIF_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_LIF_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LIF_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_INFO, &(info->ac), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_PWE_INFO, &(info->pwe), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_INFO, &(info->mpls), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_IP_TERM_INFO, &(info->ip), 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_INFO, &(info->mim), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_LIF_VALUE_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_LIF_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LIF_LKUP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_DIAG_TYPE_MAX, SOC_PB_PP_DIAG_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_LIF_KEY, &(info->key), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->base_index, SOC_PB_PP_DIAG_BASE_INDEX_MAX, SOC_PB_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->opcode_id, SOC_PB_PP_DIAG_OPCODE_ID_MAX, SOC_PB_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR, 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_LIF_VALUE, &(info->value), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_LIF_LKUP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TCAM_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_KEY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_DIAG_TCAM_KEY_NOF_UINT32S_MAX; ++ind)
  {
  }
  for (ind = 0; ind < SOC_PB_PP_DIAG_TCAM_KEY_NOF_UINT32S_MAX; ++ind)
  {
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->length, SOC_PB_PP_DIAG_LENGTH_MAX, SOC_PB_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TCAM_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TCAM_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fec_ptr, SOC_PB_PP_DIAG_FEC_PTR_MAX, SOC_PB_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TCAM_VALUE_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_LEM_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LEM_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE, &(info->mact), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_INFO, &(info->bmact), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->host.fec_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->ilm), 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_SA_AUTH_MAC_INFO, &(info->sa_auth), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_LEM_VALUE_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PARSING_L2_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_L2_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->encap_type, SOC_PB_PP_DIAG_ENCAP_TYPE_MAX, SOC_PB_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_INFO, &(info->tag_fromat), 11, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_tag_format, SOC_PB_PP_DIAG_VLAN_TAG_FORMAT_MAX, SOC_PB_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->next_prtcl, SOC_PB_PP_DIAG_NEXT_PRTCL_MAX, SOC_PB_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PARSING_L2_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PARSING_MPLS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PARSING_MPLS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PARSING_IP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->next_prtcl, SOC_PB_PP_DIAG_NEXT_PRTCL_MAX, SOC_PB_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PARSING_IP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PARSING_HEADER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_HEADER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->hdr_type, SOC_PB_PP_DIAG_HDR_TYPE_MAX, SOC_PB_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->hdr_offset, SOC_PB_PP_DIAG_HDR_OFFSET_MAX, SOC_PB_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_PARSING_L2_INFO, &(info->eth), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_PARSING_IP_INFO, &(info->ip), 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_PARSING_MPLS_INFO, &(info->mpls), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PARSING_HEADER_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PARSING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->hdr_type, SOC_PB_PP_DIAG_HDR_TYPE_MAX, SOC_PB_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PB_PP_DIAG_MAX_NOF_HDRS; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_PARSING_HEADER_INFO, &(info->hdrs_stack[ind]), 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PARSING_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TERM_MPLS_LABEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERM_MPLS_LABEL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->range_index, SOC_PB_PP_DIAG_RANGE_INDEX_MAX, SOC_PB_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR, 10, exit); */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LABEL_RANGE, &(info->range), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TERM_MPLS_LABEL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TERMINATED_MPLS_LABEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERMINATED_MPLS_LABEL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->term_type, SOC_PB_PP_DIAG_TERM_TYPE_MAX, SOC_PB_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->label, SOC_PB_PP_DIAG_LABEL_MAX, SOC_PB_PP_DIAG_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_TERM_MPLS_LABEL_INFO, &(info->range_match), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->range_bit, SOC_PB_PP_DIAG_RANGE_BIT_MAX, SOC_PB_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TERMINATED_MPLS_LABEL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TERM_MPLS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERM_MPLS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < 3; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_TERMINATED_MPLS_LABEL_INFO, &(info->lbls_info[ind]), 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TERM_MPLS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_TERM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERM_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->term_type, SOC_PB_PP_DIAG_TERM_TYPE_MAX, SOC_PB_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_TERM_MPLS_INFO, &(info->mpls), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_type, SOC_PB_PP_DIAG_FRWRD_TYPE_MAX, SOC_PB_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_TERM_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_IPV4_VPN_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vrf, SOC_PB_PP_DIAG_VRF_MAX, SOC_PB_PP_DIAG_VRF_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY, &(info->key), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_IPV4_VPN_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_IPV6_VPN_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV6_VPN_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vrf, SOC_PB_PP_DIAG_VRF_MAX, SOC_PB_PP_DIAG_VRF_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY, &(info->key), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_IPV6_VPN_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_FRWRD_LKUP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_LKUP_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_KEY, &(info->mact), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_KEY, &(info->bmact), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_IPV4_VPN_KEY, &(info->ipv4_uc), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY, &(info->ipv4_mc), 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_IPV6_VPN_KEY, &(info->ipv6_uc), 14, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY, &(info->ipv6_mc), 15, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_ILM_KEY, &(info->ilm), 16, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->trill_uc, SOC_PB_PP_DIAG_TRILL_UC_MAX, SOC_PB_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR, 17, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRILL_MC_ROUTE_KEY, &(info->trill_mc), 18, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_FRWRD_LKUP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_FRWRD_LKUP_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_LKUP_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE, &(info->mact), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_INFO, &(info->bmact), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->frwrd_decision), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_FRWRD_LKUP_VALUE_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_type, SOC_PB_PP_DIAG_FRWRD_TYPE_MAX, SOC_PB_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_hdr_index, SOC_PB_PP_DIAG_FRWRD_HDR_INDEX_MAX, SOC_PB_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR, 11, exit); */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_FRWRD_LKUP_KEY, &(info->lkup_key), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_FRWRD_LKUP_VALUE, &(info->lkup_res), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_LEARN_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LEARN_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_KEY, &(info->key), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE, &(info->value), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_LEARN_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_VLAN_EDIT_RES_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_VLAN_EDIT_RES *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO, &(info->cmd_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_VLAN_EDIT_RES_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_DECISION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->frwrd_decision), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_FRWRD_DECISION_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_NOF_DIAG_FRWRD_DECISION_PHASES; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_FRWRD_DECISION_INFO, &(info->frwrd[ind]), 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PKT_TM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PKT_TM_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->valid_fields, SOC_PB_PP_DIAG_VALID_FIELDS_MAX, SOC_PB_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->frwrd_decision), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter1, SOC_PB_PP_DIAG_METER1_MAX, SOC_PB_PP_DIAG_METER1_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter2, SOC_PB_PP_DIAG_METER2_MAX, SOC_PB_PP_DIAG_METER2_OUT_OF_RANGE_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp_meter_cmd, SOC_PB_PP_DIAG_DP_METER_CMD_MAX, SOC_PB_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR, 16, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->counter1, SOC_PB_PP_DIAG_COUNTER1_MAX, SOC_PB_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR, 17, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->counter2, SOC_PB_PP_DIAG_COUNTER2_MAX, SOC_PB_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR, 18, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cud, SOC_PB_PP_DIAG_CUD_MAX, SOC_PB_PP_DIAG_CUD_OUT_OF_RANGE_ERR, 22, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PKT_TM_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_COMMAND, &(info->mpls_cmd), 10, exit);
  for (ind = 0; ind < SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_ENTRY_INFO, &(info->encap_info[ind]), 11, exit);
  }
  for (ind = 0; ind < SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->eep[ind], SOC_PB_PP_DIAG_EEP_MAX, SOC_PB_PP_DIAG_EEP_OUT_OF_RANGE_ERR, 12, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ll_vsi, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_ac, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_EG_DROP_LOG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_EG_DROP_LOG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_EG_DROP_LOG_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_ETH_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_ETH_PACKET_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ether_type, SOC_PB_PP_DIAG_ETHER_TYPE_MAX, SOC_PB_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_ETH_PACKET_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_DIAG_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_DIAG_TYPE_MAX, SOC_PB_PP_DIAG_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_tm_port, SOC_PB_PP_DIAG_IN_TM_PORT_MAX, SOC_PB_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->total_size, SOC_PB_PP_DIAG_TOTAL_SIZE_MAX, SOC_PB_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_packets, SOC_PB_PP_DIAG_NOF_PACKETS_MAX, SOC_PB_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR, 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_DIAG_ETH_PACKET_INFO, &(info->ether_info), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_DIAG_PACKET_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

