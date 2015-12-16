/* $Id: pb_pp_eg_encap.c,v 1.14 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_eg_encap.c
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_encap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX                             (SOC_DPP_NOF_OUT_LIFS_PETRAB - 1)
#define SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_MAX                        (SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES-1)
#define SOC_PB_PP_EG_ENCAP_DEPTH_MAX                               (SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES-1)
#define SOC_PB_PP_EG_ENCAP_PROFILE_NDX_MAX                         (7)
#define SOC_PB_PP_EG_ENCAP_SRC_IP_MAX                              (15)
#define SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_MIN                        (0) /* ((ll_limit + 1)) */
#define SOC_PB_PP_EG_ENCAP_SWAP_LABEL_MAX                          (1*1024*1024-1)
#define SOC_PB_PP_EG_ENCAP_LABEL_MAX                               (1*1024*1024-1)
#define SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_MAX                        (7)
#define SOC_PB_PP_EG_ENCAP_OUT_VSI_MAX                             (4*1024-1)
#define SOC_PB_PP_EG_ENCAP_TPID_PROFILE_MAX                        (3)
#define SOC_PB_PP_EG_ENCAP_TUNNEL_LABEL_MAX                        (SOC_SAND_U32_MAX)
#define SOC_PB_PP_EG_ENCAP_NOF_TUNNELS_MAX                         (2)
#define SOC_PB_PP_EG_ENCAP_ORIENTATION_MAX                         (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define SOC_PB_PP_EG_ENCAP_DEST_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_EG_ENCAP_SRC_INDEX_MAX                           (15)
#define SOC_PB_PP_EG_ENCAP_TTL_INDEX_MAX                           (3)
#define SOC_PB_PP_EG_ENCAP_TOS_INDEX_MAX                           (15)
#define SOC_PB_PP_EG_ENCAP_PCP_DEI_MAX                             (SOC_SAND_U8_MAX)
#define SOC_PB_PP_EG_ENCAP_TPID_INDEX_MAX                          (3)
#define SOC_PB_PP_EG_ENCAP_MODEL_MAX                               (SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1)
#define SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_MAX                       (SOC_PB_PP_NOF_EG_ENCAP_EXP_MARK_MODES-1)
#define SOC_PB_PP_EG_ENCAP_CW_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_MAX                          (SOC_PB_PP_NOF_EG_ENCAP_ENTRY_TYPES-1)

#define SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS                            (8)
#define SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS                            (8)
#define SOC_PB_PP_EG_ENCAP_SIF_NOF_BITS                            (32)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_CW_NOF_BITS                         (1)
#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_EXP_NOF_BITS                        (3)
#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_TTL_NOF_BITS                        (8)
#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PIPE_NOF_BITS                       (1)

#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_NOF_BITS                 \
  (SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_CW_NOF_BITS  +                        \
   SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_EXP_NOF_BITS +                        \
   SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_TTL_NOF_BITS +                        \
   SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PIPE_NOF_BITS)

#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_BUILD(pipe, ttl, exp, cw)  \
  SOC_PB_PP_FLDS_TO_BUFF_4(                                                  \
    cw, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_CW_NOF_BITS,                     \
    exp, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_EXP_NOF_BITS,                   \
    ttl, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_TTL_NOF_BITS,                   \
    pipe, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PIPE_NOF_BITS)


#define SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_PARSE(buff, pipe, ttl, exp, cw) \
    SOC_PB_PP_FLDS_FROM_BUFF_4(                                              \
    buff,                                                                \
    cw, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_CW_NOF_BITS,                     \
    exp, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_EXP_NOF_BITS,                   \
    ttl, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_TTL_NOF_BITS,                   \
    pipe, SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PIPE_NOF_BITS)

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

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_eg_encap[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_LL_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_TUNNEL_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_ENCAP_LIF_ENTRY_GET_UNSAFE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_eg_encap[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'lif_eep_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'next_eep' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'tunnel_eep_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'll_eep' is out of range. \n\r "
    "The range is: 0 to range_info->ll_limit.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'll_eep_ndx' is out of range. \n\r "
    "The range is: 0 to range_info->ll_limit.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eep_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eep_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR",
    "The parameter 'depth' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR",
    "The parameter 'src_ip' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR",
    "The parameter 'll_limit' is out of range. \n\r "
    "The range is: 0 - 4*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR",
    "The parameter 'ip_tnl_limit' is out of range. \n\r "
    "The range is: (ll_limit + 1) - (12K-1).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_MPLS_TNL_LIMIT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_MPLS_TNL_LIMIT_OUT_OF_RANGE_ERR",
    "The parameter 'mpls_tnl_limit' is out of range. \n\r "
    "The range is: ip_tnl_limit+1 - 12*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'swap_label' is out of range. \n\r "
    "The range is: 0 - 1*1024*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'label' is out of range. \n\r "
    "The range is: 0 - 1*1024*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'push_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR",
    "The parameter 'out_vsi' is out of range. \n\r "
    "The range is: 0 - 4K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'pop_type' is out of range. \n\r "
    "Value should be an mpls pop command (Range: 8-14).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'tunnel_label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_tunnels' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR",
    "The parameter 'dest' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'src_index' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ttl_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tos_index' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_dei' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR",
    "The parameter 'model' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'exp_mark_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_ENCAP_EXP_MARK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR",
    "The parameter 'cw' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'entry_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_ENCAP_ENTRY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_AC_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_AC_OUT_OF_RANGE_ERR",
    "'lif_eep_ndx' is out of range. The range is: 0 to range_info->ll_limit,\n\r "
    "as configured using soc_pb_pp_eg_encap_range_info_set().\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR",
    "'lif_eep_ndx' is out of range. The range is: range_info->ip_tunnel_limit+1\n\r "
    "to 12K-1, as configured using soc_pb_pp_eg_encap_range_info_set().\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LIF_EEP_AND_AC_ID_NOT_EQUAL_ERR,
    "SOC_PB_PP_EG_ENCAP_LIF_EEP_AND_AC_ID_NOT_EQUAL_ERR",
    "Out ac should be equal to lif_eep.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR",
    "'lif_eep_ndx' is out of range. \n\r"
    "The range is: MAX(range_info->ip_tunnel_limit+1,8K) to 12K-1 .\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR",
    "'PARAM_NAME ' is out of range. \n\r"
    "The range is: range_info->ll_limit+1, range_info->ip_tnl_limit, \n\r"
    "as configured using soc_pb_pp_eg_encap_range_info_set().\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
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
  soc_pb_pp_eg_encap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->epni.eep_limit_reg, reg_val, 10, exit);
  SOC_PB_PP_FLD_TO_REG(regs->epni.eep_limit_reg.eep_ip_tunnel_limit, range_info->ip_tnl_limit, reg_val, 20, exit);
  SOC_PB_PP_FLD_TO_REG(regs->epni.eep_limit_reg.eep_link_layer_limit, range_info->ll_limit, reg_val, 30, exit);
  SOC_PB_PP_REG_SET(regs->epni.eep_limit_reg, reg_val, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_range_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_RANGE_INFO, range_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_range_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_range_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_VERIFY);

  /* Nothing to verify ... */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_range_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PB_PP_EG_ENCAP_RANGE_INFO_clear(range_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->epni.eep_limit_reg, reg_val, 10, exit);
  SOC_PB_PP_FLD_FROM_REG(regs->epni.eep_limit_reg.eep_ip_tunnel_limit, range_info->ip_tnl_limit, reg_val, 20, exit);
  SOC_PB_PP_FLD_FROM_REG(regs->epni.eep_limit_reg.eep_link_layer_limit, range_info->ll_limit, reg_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_range_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set LIF Editing entry to be NULL Entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_null_lif_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_null_lif_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_null_lif_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_null_lif_entry_add_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold AC ID. Actually maps from
 *     CUD to AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ac_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ac_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_ac_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ac_entry_add_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold MPLS LSR SWAP label.
 *     Needed for MPLS multicast services.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_swap_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(swap_info);

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.mpls1_command = SOC_PB_PP_MPLS_COMMAND_TYPE_SWAP;
  tbl_data.mpls1_label = swap_info->swap_label;
  tbl_data.next_eep = next_eep;
  tbl_data.next_vsi_lsb = swap_info->out_vsi;

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_swap_command_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_swap_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_VERIFY);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(lif_eep_ndx, range_info.ip_tnl_limit+1, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_SWAP_INFO, swap_info, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(next_eep, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_swap_command_entry_add_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold PWE info (VC label and
 *     push profile).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    tbl_data;
  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA
    eep_orientation_tbl_data;
  SOC_PB_PP_TBLS
    *tbls = NULL;
  uint32
    tbl_offset,
    bit_offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Push commands in HW implementations is 0 + push_profile */
  tbl_data.mpls1_command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH + pwe_info->push_profile;
  tbl_data.mpls1_label = pwe_info->label;
  tbl_data.next_eep = next_eep;

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  /*
   * Orientation
   */
  tbls = soc_pb_pp_tbls();

  tbl_offset = lif_eep_ndx / tbls->egq.eep_orientation_tbl.addr.width_bits;
  bit_offset = lif_eep_ndx % tbls->egq.eep_orientation_tbl.addr.width_bits;

  res = soc_pb_pp_egq_eep_orientation_tbl_get_unsafe(
          unit,
          tbl_offset,
          &eep_orientation_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_bitstream_set(
          &eep_orientation_tbl_data.eep_orientation,
          bit_offset,
          (pwe_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 0x1 : 0x0)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_egq_eep_orientation_tbl_set_unsafe(
          unit,
          tbl_offset,
          &eep_orientation_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_pwe_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_VERIFY);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
    unit,
    &range_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(
    lif_eep_ndx,
    SOC_SAND_MAX((range_info.ip_tnl_limit+1), SOC_PB_PP_EG_ENCAP_UPPER_BANK_MIN_ENTRY),
    SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR, 10, exit
    );

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_PWE_INFO, pwe_info, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(next_eep, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_entry_add_verify()", lif_eep_ndx, next_eep);
}

/*********************************************************************
*     Set LIF Editing entry to hold MPLS LSR POP command.
 *     Needed for MPLS multicast services.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pop_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pop_info);

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.mpls1_command = pop_info->pop_type;
  tbl_data.next_eep = next_eep;
  tbl_data.next_vsi_lsb = pop_info->ethernet_info.out_vsi;
  if (pop_info->pop_type == SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET)
  {
    tbl_data.has_cw = SOC_SAND_BOOL2NUM(pop_info->ethernet_info.has_cw);
    tbl_data.tpid_profile = pop_info->ethernet_info.tpid_profile;
  }

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pop_command_entry_add_unsafe()", lif_eep_ndx, next_eep);
}

uint32
  soc_pb_pp_eg_encap_pop_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_eep_ndx, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_POP_INFO, pop_info, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(next_eep, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pop_command_entry_add_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
 * invalid for Soc_petraB.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_vsi_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,   
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep  
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_vsi_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_vsi_entry_add_verify(
    SOC_SAND_IN  int                                 unit,   
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_vsi_entry_add_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Add MPLS tunnels encapsulation entry to the Tunnels
 *     Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_mpls_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    bit_offset;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    ee_entry_tbl_data;
  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA
    eep_orientation_tbl_data;
  SOC_PB_PP_TBLS
    *tbls = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_encap_info);

  tbls = soc_pb_pp_tbls();

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
          unit,
          tunnel_eep_ndx,
          &ee_entry_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ee_entry_tbl_data.next_vsi_lsb = mpls_encap_info->out_vsi;
  ee_entry_tbl_data.next_eep = ll_eep;

  if (mpls_encap_info->nof_tunnels > 0)
  {
    /* 1st encapsulation */
    ee_entry_tbl_data.mpls1_command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH + mpls_encap_info->tunnels[0].push_profile;
    ee_entry_tbl_data.mpls1_label = mpls_encap_info->tunnels[0].tunnel_label;
  }
  else
  {
    ee_entry_tbl_data.mpls1_command = 0;
    ee_entry_tbl_data.mpls1_label = 0;
  }

  if (mpls_encap_info->nof_tunnels > 1)
  {
    /* 2nd encapsulation */
    ee_entry_tbl_data.mpls2_command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH + mpls_encap_info->tunnels[1].push_profile;
    ee_entry_tbl_data.mpls2_label = mpls_encap_info->tunnels[1].tunnel_label;
  }
  else /* no second encap */
  {
    ee_entry_tbl_data.mpls2_command = 0;
    ee_entry_tbl_data.mpls2_label = 0;
  }

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          tunnel_eep_ndx,
          &ee_entry_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  /*
   * Orientation
   */
  tbl_offset = tunnel_eep_ndx / tbls->egq.eep_orientation_tbl.addr.width_bits;
  bit_offset = tunnel_eep_ndx % tbls->egq.eep_orientation_tbl.addr.width_bits;

  res = soc_pb_pp_egq_eep_orientation_tbl_get_unsafe(
          unit,
          tbl_offset,
          &eep_orientation_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_bitstream_set(
          &eep_orientation_tbl_data.eep_orientation,
          bit_offset,
          (mpls_encap_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 0x1 : 0x0)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_egq_eep_orientation_tbl_set_unsafe(
    unit,
    tbl_offset,
    &eep_orientation_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_mpls_entry_add_unsafe()", tunnel_eep_ndx, ll_eep);
}

uint32
  soc_pb_pp_eg_encap_mpls_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_VERIFY);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(tunnel_eep_ndx, range_info.ip_tnl_limit+1, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO, mpls_encap_info, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ll_eep, range_info.ll_limit, SOC_PB_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_mpls_entry_add_verify()", tunnel_eep_ndx, 0);
}

/*********************************************************************
*     Add IPv4 tunnels encapsulation entry to the Egress
 *     Encapsulation Tunnels Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ipv4_encap_info);

  res = soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_get_unsafe(
          unit,
          tunnel_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.gre_enable = SOC_SAND_BOOL2NUM(ipv4_encap_info->dest.enable_gre);
  tbl_data.ipv4_dst = ipv4_encap_info->dest.dest;
  tbl_data.ipv4_src_index = ipv4_encap_info->dest.src_index;
  tbl_data.ipv4_tos_index = ipv4_encap_info->dest.tos_index;
  tbl_data.ipv4_ttl_index = ipv4_encap_info->dest.ttl_index;
  tbl_data.next_vsi_lsb = ipv4_encap_info->out_vsi;
  tbl_data.next_eep = ll_eep;
  
  res = soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_set_unsafe(
          unit,
          tunnel_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_entry_add_unsafe()", tunnel_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_ipv4_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_VERIFY);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(tunnel_eep_ndx, range_info.ll_limit+1, range_info.ip_tnl_limit, SOC_PB_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO, ipv4_encap_info, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ll_eep, range_info.ll_limit, SOC_PB_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_entry_add_verify()", tunnel_eep_ndx, 0);
}

/*********************************************************************
*     Add LL encapsulation entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ll_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                    *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  res = soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_get_unsafe(
          unit,
          ll_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          &ll_encap_info->dest_mac,
          tbl_data.dest_mac
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  tbl_data.vid = ll_encap_info->out_vid;
  tbl_data.vid_valid = ll_encap_info->out_vid_valid;

  res = soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_set_unsafe(
          unit,
          ll_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_entry_add_unsafe()", ll_eep_ndx, 0);
}


uint32
  soc_pb_pp_eg_encap_ll_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                    *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_VERIFY);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(ll_eep_ndx, range_info.ll_limit, SOC_PB_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_LL_INFO, ll_encap_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ll_entry_add_verify()", ll_eep_ndx, 0);
}

/*********************************************************************
*     Remove entry from the encapsulation Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_UNSAFE);

  sal_memset(&tbl_data, 0x0, sizeof(tbl_data));

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_entry_remove_unsafe()", eep_type_ndx, eep_ndx);
}


uint32
  soc_pb_pp_eg_encap_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eep_type_ndx, SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_MAX, SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  switch (eep_type_ndx)
  {
  case SOC_PB_PP_EG_ENCAP_EEP_TYPE_LL:
    SOC_SAND_ERR_IF_ABOVE_MAX(eep_ndx, range_info.ll_limit, SOC_PB_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR, 20, exit);
  	break;
  case SOC_PB_PP_EG_ENCAP_EEP_TYPE_LIF_EEP:
  case SOC_PB_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP:
    
    SOC_SAND_ERR_IF_ABOVE_MAX(eep_ndx, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR, 30, exit);
  	break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 50, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_entry_remove_verify()", 0, eep_ndx);
}

/*********************************************************************
 *     Get LL encapsulation entry.
 *********************************************************************/
STATIC uint32
  soc_pb_pp_eg_encap_ll_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                 *encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_LL_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  res = soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_get_unsafe(
          unit,
          ll_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          tbl_data.dest_mac,
          &encap_info->entry_val.ll_info.dest_mac
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  encap_info->entry_val.ll_info.out_vid = tbl_data.vid;
  encap_info->entry_val.ll_info.out_vid_valid = SOC_SAND_NUM2BOOL(tbl_data.vid_valid);

  encap_info->entry_type = SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_LL_ENCAP;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_entry_add_unsafe()", ll_eep_ndx, 0);
}

/*********************************************************************
 *     Get LIF encapsulation entry.
 *********************************************************************/
STATIC uint32
  soc_pb_pp_eg_encap_lif_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint32                                  *next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_LIF_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);
  SOC_SAND_CHECK_NULL_INPUT(next_eep);

  res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
          unit,
          eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (tbl_data.mpls1_command == SOC_PB_PP_MPLS_COMMAND_TYPE_SWAP)
  {
    /* SWAP */
    encap_info->entry_type = SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_SWAP_CMND;
    SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(encap_info);
    encap_info->entry_val.swap_info.swap_label = tbl_data.mpls1_label;
    encap_info->entry_val.swap_info.out_vsi = tbl_data.next_vsi_lsb;
  }
  else if ((tbl_data.mpls1_command >= SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE) &&
           (tbl_data.mpls1_command <= SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET))
  {
    /* POP */
    encap_info->entry_type = SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_POP_CMND;
    SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(encap_info);
    encap_info->entry_val.pop_info.pop_type = tbl_data.mpls1_command;
    if (encap_info->entry_val.pop_info.pop_type == SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET)
    {
      encap_info->entry_val.pop_info.ethernet_info.has_cw = SOC_SAND_NUM2BOOL(tbl_data.has_cw);
      encap_info->entry_val.pop_info.ethernet_info.tpid_profile = tbl_data.tpid_profile;
      encap_info->entry_val.pop_info.ethernet_info.out_vsi = tbl_data.next_vsi_lsb;
    }
  }
  else
  {
    /* PWE */
    encap_info->entry_type = SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_PWE;
    SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(encap_info);

    /* Push commands in HW implementations is 0 + push_profile */
    encap_info->entry_val.pwe_info.push_profile = tbl_data.mpls1_command - SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;
    encap_info->entry_val.pwe_info.label = tbl_data.mpls1_label;
  }

  *next_eep = tbl_data.next_eep;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_tunnel_entry_get_unsafe()", eep_ndx, 0);
}

/*********************************************************************
 *     Get tunnel encapsulation entry.
 *********************************************************************/
STATIC uint32
  soc_pb_pp_eg_encap_tunnel_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint32                                  *next_eep
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    bit_offset,
    orientation_val;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
    ee_entry_tbl_data;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA
    ip_tunnel_format_tbl_data;
  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA
    eep_orientation_tbl_data;
  SOC_PB_PP_TBLS
    *tbls = NULL;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_TUNNEL_ENTRY_GET_UNSAFE);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (eep_ndx <= range_info.ip_tnl_limit)
  {
    /* IP Tunnel */
    encap_info->entry_type = SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP;
    SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(encap_info);
    res = soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_get_unsafe(
            unit,
            eep_ndx,
            &ip_tunnel_format_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    encap_info->entry_val.ipv4_encap_info.dest.enable_gre = SOC_SAND_NUM2BOOL(ip_tunnel_format_tbl_data.gre_enable);
    encap_info->entry_val.ipv4_encap_info.dest.dest = ip_tunnel_format_tbl_data.ipv4_dst;
    encap_info->entry_val.ipv4_encap_info.dest.src_index = ip_tunnel_format_tbl_data.ipv4_src_index;
    encap_info->entry_val.ipv4_encap_info.dest.tos_index = (uint8)ip_tunnel_format_tbl_data.ipv4_tos_index;
    encap_info->entry_val.ipv4_encap_info.dest.ttl_index = (uint8)ip_tunnel_format_tbl_data.ipv4_ttl_index;
    encap_info->entry_val.ipv4_encap_info.out_vsi = ip_tunnel_format_tbl_data.next_vsi_lsb;
    *next_eep = ip_tunnel_format_tbl_data.next_eep;

  }
  else
  {
    /* parse it as LIF, to check if it pop or swap */
    res = soc_pb_pp_eg_encap_lif_entry_get_unsafe(
            unit,
            eep_ndx,
            encap_info,
            next_eep
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* not tunnel encapsulation but swap/pop, then done */
    if(encap_info->entry_type != SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_PWE) {
        goto exit;
    }
    
    /* MPLS tunnel */
    tbls = soc_pb_pp_tbls();

    res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
            unit,
            eep_ndx,
            &ee_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    encap_info->entry_type = SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
    SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(encap_info);

    encap_info->entry_val.mpls_encap_info.out_vsi = ee_entry_tbl_data.next_vsi_lsb;
    *next_eep = ee_entry_tbl_data.next_eep;

    encap_info->entry_val.mpls_encap_info.nof_tunnels = 0;

    /* 1st encapsulation */
    encap_info->entry_val.mpls_encap_info.tunnels[0].push_profile =
      ee_entry_tbl_data.mpls1_command - SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;
    encap_info->entry_val.mpls_encap_info.tunnels[0].tunnel_label =
      ee_entry_tbl_data.mpls1_label;

    /* 2nd encapsulation */
    encap_info->entry_val.mpls_encap_info.tunnels[1].push_profile =
      ee_entry_tbl_data.mpls2_command - SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;
    encap_info->entry_val.mpls_encap_info.tunnels[1].tunnel_label =
      ee_entry_tbl_data.mpls2_label;

    /* Nof tunnels */
    if (encap_info->entry_val.mpls_encap_info.tunnels[0].tunnel_label != 0)
    {
      ++encap_info->entry_val.mpls_encap_info.nof_tunnels;
    }
    if (encap_info->entry_val.mpls_encap_info.tunnels[1].tunnel_label != 0)
    {
      ++encap_info->entry_val.mpls_encap_info.nof_tunnels;
    }

    /*
     * Orientation
     */
    tbl_offset = eep_ndx / tbls->egq.eep_orientation_tbl.addr.width_bits;
    bit_offset = eep_ndx % tbls->egq.eep_orientation_tbl.addr.width_bits;

    res = soc_pb_pp_egq_eep_orientation_tbl_get_unsafe(
            unit,
            tbl_offset,
            &eep_orientation_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    orientation_val = SOC_SAND_GET_BIT(eep_orientation_tbl_data.eep_orientation, bit_offset);
    
    encap_info->entry_val.mpls_encap_info.orientation =
      orientation_val ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_tunnel_entry_get_unsafe()", eep_ndx, 0);
}

/*********************************************************************
*     Get entry information from the Egress encapsulation
 *     tables.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_IN  uint32                                  depth,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                 encap_entry_info[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                  next_eep[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    depth_i,
    cur_eep_ndx;
  SOC_PB_PP_EG_ENCAP_EEP_TYPE
    cur_eep_type;
  SOC_PB_PP_EG_ENCAP_RANGE_INFO
    range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_SAND_ERR_IF_ABOVE_MAX(depth, SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES - 1, SOC_PB_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR, 3, exit);

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          &range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* Eep entry to get on current iteration */
  cur_eep_type = eep_type_ndx;
  cur_eep_ndx = eep_ndx;
  *nof_entries = 0;

  for (depth_i = 0; depth_i <= depth; ++depth_i)
  {
    SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(&encap_entry_info[depth_i]);
    next_eep[depth_i] = 0xffffffff;

    switch (cur_eep_type)
    {
      case SOC_PB_PP_EG_ENCAP_EEP_TYPE_LIF_EEP:
        res = soc_pb_pp_eg_encap_lif_entry_get_unsafe(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i],
                &next_eep[depth_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        break;

      case SOC_PB_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP:
        res = soc_pb_pp_eg_encap_tunnel_entry_get_unsafe(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i],
                &next_eep[depth_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        break;

      case SOC_PB_PP_EG_ENCAP_EEP_TYPE_LL:
        res = soc_pb_pp_eg_encap_ll_entry_get_unsafe(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        
        /* Last encap. Break the loop. */
        depth_i = SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 40, exit);
    }

    if (depth_i != SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES)
    {
      /* If not done, update current eep for next iteration */
      cur_eep_ndx = next_eep[depth_i];

      /* We're not in the 1st iteration, so entry is not LIF (so ll/tunnel) */
      if (cur_eep_ndx <= range_info.ll_limit)
      {
        cur_eep_type = SOC_PB_PP_EG_ENCAP_EEP_TYPE_LL;
      }
      else
      {
        cur_eep_type = SOC_PB_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP;
      }
    }

    ++(*nof_entries);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_entry_get_unsafe()", 0, eep_ndx);
}

uint32
  soc_pb_pp_eg_encap_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_IN  uint32                                  depth
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eep_type_ndx, SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_MAX, SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eep_ndx, SOC_PB_PP_EG_ENCAP_EEP_NDX_MAX, SOC_PB_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(depth, SOC_PB_PP_EG_ENCAP_DEPTH_MAX, SOC_PB_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_entry_get_verify()", 0, eep_ndx);
}

/*********************************************************************
*     Setting the push profile info - specifying how to build
 *     the label header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_buff[SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS],
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_BUFFER_GET(
    regs->epni.mpls_profile_reg,
    SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS,
    reg_buff,
    10,
    exit);

  fld_val = SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_BUILD(
    (profile_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE ? 0x1 : 0x0),
    profile_info->ttl,
    profile_info->exp,
    SOC_SAND_BOOL2NUM(profile_info->has_cw)
    );

  res = soc_sand_bitstream_set_any_field(
          &fld_val,
          SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_NOF_BITS * profile_ndx,
          SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_NOF_BITS,
          reg_buff);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PB_PP_REG_BUFFER_SET(
    regs->epni.mpls_profile_reg,
    SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS,
    reg_buff,
    30,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_push_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PP_EG_ENCAP_PROFILE_NDX_MAX, SOC_PB_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_push_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PP_EG_ENCAP_PROFILE_NDX_MAX, SOC_PB_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_profile_info_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Setting the push profile info - specifying how to build
 *     the label header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_buff[SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS],
    fld_val,
    model,
    ttl,
    exp,
    has_cw;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_clear(profile_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_BUFFER_GET(
    regs->epni.mpls_profile_reg,
    SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS,
    reg_buff,
    10,
    exit);

  res = soc_sand_bitstream_get_any_field(
          reg_buff,
          SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_NOF_BITS * profile_ndx,
          SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PB_PP_EG_ENCAP_MPLS_PROFILE_REG_PROFILE_PARSE(
    fld_val,
    model,
    ttl,
    exp,
    has_cw);

  profile_info->ttl = (SOC_SAND_PP_IP_TTL)ttl;
  profile_info->exp = (SOC_SAND_PP_MPLS_EXP)exp;
  profile_info->model = (model ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE : SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM);
  profile_info->has_cw = SOC_SAND_NUM2BOOL(has_cw);
  profile_info->exp_mark_mode = SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_profile_info_get_unsafe()", profile_ndx, 0);
}


/*********************************************************************
*     Set the EXP value of the pushed label as mapping of the
 *     TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exp_key);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_exp_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_push_exp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_exp_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_push_exp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_exp_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the EXP value of the pushed label as mapping of the
 *     TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exp_key);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_exp_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_SET(regs->epni.mpls_control_word_reg.mpls_control_word, glbl_info->cw, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_glbl_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_clear(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.mpls_control_word_reg.mpls_control_word, glbl_info->cw, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set source IP address for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  reg_offset = entry_ndx / (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_SIF_NOF_BITS);

  SOC_PB_PP_REG_SET(regs->epni.ipv4_sip_reg[reg_offset], src_ip, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_EG_ENCAP_SRC_IP_MAX, SOC_PB_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(src_ip, SOC_PB_PP_EG_ENCAP_SRC_IP_MAX, SOC_PB_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR, 20, exit); */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_EG_ENCAP_SRC_IP_MAX, SOC_PB_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify()", entry_ndx, 0);
}

/*********************************************************************
*     Set source IP address for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT uint32                                  *src_ip
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_UNSAFE);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.ipv4_sip_reg[entry_ndx].ipv4_sip, (*src_ip), 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set TTL for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    fld_offset,
    fld_val,
    ttl_fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  reg_offset = entry_ndx / (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS);
  fld_offset = (entry_ndx % (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS)) * SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS;

  SOC_PB_PP_REG_GET(regs->epni.ipv4_ttl_reg[reg_offset], fld_val, 10, exit);

  ttl_fld_val = (uint32)ttl;
  res = soc_sand_bitstream_set_any_field(
          &ttl_fld_val,
          fld_offset,
          SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_PP_REG_SET(regs->epni.ipv4_ttl_reg[reg_offset], fld_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe()", entry_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_EG_ENCAP_TTL_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(ttl, SOC_SAND_PP_IP_TTL_MAX, SOC_SAND_PP_IP_TTL_OUT_OF_RANGE_ERR, 20, exit); */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_EG_ENCAP_TTL_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify()", entry_ndx, 0);
}


/*********************************************************************
*     Set TTL for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    fld_offset,
    fld_val,
    ttl_fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ttl);

  regs = soc_pb_pp_regs();

  reg_offset = entry_ndx / (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS);
  fld_offset = (entry_ndx % (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS)) * SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS;

  SOC_PB_PP_REG_GET(regs->epni.ipv4_ttl_reg[reg_offset], fld_val, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &fld_val,
          fld_offset,
          SOC_PB_PP_EG_ENCAP_TTL_NOF_BITS,
          &ttl_fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *ttl = (SOC_SAND_PP_IP_TTL)ttl_fld_val ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe()", entry_ndx, 0);
}

/*********************************************************************
*     Set TOS for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    fld_offset,
    fld_val,
    tos_fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  reg_offset = entry_ndx / (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS);
  fld_offset = (entry_ndx % (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS)) * SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS;

  SOC_PB_PP_REG_GET(regs->epni.ipv4_tos_reg[reg_offset], fld_val, 10, exit);

  tos_fld_val = (uint32)tos;
  res = soc_sand_bitstream_set_any_field(
          &tos_fld_val,
          fld_offset,
          SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_PP_REG_SET(regs->epni.ipv4_tos_reg[reg_offset], fld_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe()", entry_ndx, 0);
}


uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_EG_ENCAP_TOS_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(tos, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 20, exit); */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_EG_ENCAP_TOS_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify()", entry_ndx, 0);
}

/*********************************************************************
*     Set TOS for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  )
  {
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    fld_offset,
    fld_val,
    tos_fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tos);

  regs = soc_pb_pp_regs();

  reg_offset = entry_ndx / (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS);
  fld_offset = (entry_ndx % (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS)) * SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS;

  SOC_PB_PP_REG_GET(regs->epni.ipv4_tos_reg[reg_offset], fld_val, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &fld_val,
          fld_offset,
          SOC_PB_PP_EG_ENCAP_TOS_NOF_BITS,
          &tos_fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *tos = (SOC_SAND_PP_IPV4_TOS)tos_fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe()", entry_ndx, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_eg_encap module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_encap_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_eg_encap;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_eg_encap module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_encap_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_eg_encap;
}
uint32
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ll_limit, (16*1024-1), SOC_PB_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_MIN may be changed and be more thean 0 */
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->ip_tnl_limit, SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_MIN, (16*1024-1), SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_RANGE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_SWAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->swap_label, SOC_PB_PP_EG_ENCAP_SWAP_LABEL_MAX, SOC_PB_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_PB_PP_EG_ENCAP_OUT_VSI_MAX, SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_SWAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_PWE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->label, SOC_PB_PP_EG_ENCAP_LABEL_MAX, SOC_PB_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_MAX, SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_PWE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_PB_PP_EG_ENCAP_OUT_VSI_MAX, SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, SOC_PB_PP_EG_ENCAP_TPID_PROFILE_MAX, SOC_PB_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_POP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->pop_type, SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE, SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET, SOC_PB_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO, &(info->ethernet_info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_POP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tunnel_label, SOC_PB_PP_EG_ENCAP_TUNNEL_LABEL_MAX, SOC_PB_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_MAX, SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_EG_ENCAP_MPLS_MAX_NOF_TUNNELS; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO, &(info->tunnels[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_tunnels, SOC_PB_PP_EG_ENCAP_NOF_TUNNELS_MAX, SOC_PB_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PB_PP_EG_ENCAP_ORIENTATION_MAX, SOC_PB_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_PB_PP_EG_ENCAP_OUT_VSI_MAX, SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Nothing to verify in dest */
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->dest, SOC_PB_PP_EG_ENCAP_DEST_MAX, SOC_PB_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR, 10, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_index, SOC_PB_PP_EG_ENCAP_SRC_INDEX_MAX, SOC_PB_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl_index, SOC_PB_PP_EG_ENCAP_TTL_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tos_index, SOC_PB_PP_EG_ENCAP_TOS_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO, &(info->dest), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_PB_PP_EG_ENCAP_OUT_VSI_MAX, SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_LL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_dei, SOC_PB_PP_EG_ENCAP_PCP_DEI_MAX, SOC_PB_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR, 14, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_index, SOC_PB_PP_EG_ENCAP_TPID_INDEX_MAX, SOC_PB_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_LL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->model, SOC_PB_PP_EG_ENCAP_MODEL_MAX, SOC_PB_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, SOC_SAND_PP_IP_TTL_MAX, SOC_SAND_PP_IP_TTL_OUT_OF_RANGE_ERR, 12, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_mark_mode, SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_MAX, SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_MAX, SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cw, SOC_PB_PP_EG_ENCAP_CW_MAX, SOC_PB_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_ac, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_SWAP_INFO, &(info->swap_info), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_PWE_INFO, &(info->pwe_info), 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_POP_INFO, &(info->pop_info), 14, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO, &(info->mpls_encap_info), 15, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO, &(info->ipv4_encap_info), 16, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_LL_INFO, &(info->ll_info), 17, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_ENCAP_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_type, SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_MAX, SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_ENCAP_ENTRY_VALUE, &(info->entry_val), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_ENCAP_ENTRY_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

