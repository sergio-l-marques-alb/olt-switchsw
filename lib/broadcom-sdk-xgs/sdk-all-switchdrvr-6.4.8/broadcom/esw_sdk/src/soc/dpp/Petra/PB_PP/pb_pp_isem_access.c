/* $Id: pb_pp_isem_access.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_vsi.c
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

#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_vsi.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

#ifdef USING_CHIP_SIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_ISEM_ACCESS_DEBUG                                     (1)

#define SOC_PB_PP_ISEM_ACCESS_ADDR                                      (0x00900000)

#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_LSB                        (0)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_MSB                        (2)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_LSB                         (11)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_MSB                         (44)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_PAYLOAD_LSB                     (45)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_PAYLOAD_MSB                     (60)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE                            (SOC_PB_PP_IHP_ISEM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS)

/* Field overlaps other fields. MUST be set first */
#define SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB                              (30)
#define SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MSB                              (33)
#define SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS                         (SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_LSB                        (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_MSB                        (19)
#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_NOF_BITS                   (SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_LSB                        (20)
#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_MSB                        (29)
#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_NOF_BITS                   (SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_LSB                          (20)
#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_MSB                          (31)
#define SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_NOF_BITS                     (SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_LSB            (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_MSB            (5)
#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_NOF_BITS       (SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_LSB              (6)
#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_MSB              (17)
#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_NOF_BITS         (SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_LSB              (18)
#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_MSB              (29)
#define SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_NOF_BITS         (SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_LSB            (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_MSB            (31)
#define SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_NOF_BITS       (SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_LSB                   (24)
#define SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_MSB                   (29)
#define SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_NOF_BITS              (SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_LSB                          (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_MSB                          (23)
#define SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_NOF_BITS                     (SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_LSB                   (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_MSB                   (15)
#define SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_NOF_BITS              (SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_LSB                   (16)
#define SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_MSB                   (29)
#define SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_NOF_BITS              (SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_LSB                       (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_MSB                       (13)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_NOF_BITS                  (SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_LSB             (14)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_MSB             (25)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_NOF_BITS        (SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_LSB               (0)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_MSB               (8)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_NOF_BITS          (SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_LSB         (9)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_MSB         (20)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_NOF_BITS    (SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_LSB         (21)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_MSB         (32)
#define SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_NOF_BITS    (SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_MSB - SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_LSB          (0)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_MSB          (13)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_NOF_BITS     (SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_LSB + 1)

#define SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_LSB             (14)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_MSB             (15)
#define SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_NOF_BITS        (SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_LSB + 1)


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

typedef enum
{
  SOC_PB_PP_ISEM_ACCESS_ACTION_TYPE_DELETE = 0,
  SOC_PB_PP_ISEM_ACCESS_ACTION_TYPE_INSERT = 1,
  SOC_PB_PP_ISEM_ACCESS_ACTION_TYPE_DEFRAG = 4
}SOC_PB_PP_ISEM_ACCESS_ACTION_TYPE;


static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_isem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ISEM_ACCESS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ISEM_ACCESS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_isem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR,
    "SOC_PB_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR",
    "Unknown key type. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ISEM_ACCESS_NOT_READY_ERR,
    "SOC_PB_PP_ISEM_ACCESS_NOT_READY_ERR",
    "Sem is not ready. \n\r ",
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
 * GLOBALS   *
 *************/
/* { */

static SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM
  Soc_pb_pp_isem_access_instruction_1st[] =
{
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_VLAN_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INITIAL_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INITIAL_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_VLAN_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_HDR_INNER_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INITIAL_VID,  SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_VLAN_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_ID_ISID_WO_BTAG, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_WO_BTAG, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_ID_ISID_W_BTAG, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_W_BTAG, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_NO_LOOKUP, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 0, 0, 0},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_AH1_IN_AC_KEY_LSB(12), SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_AH1_IN_AC_KEY, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INITIAL_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_AH1_IN_AC_KEY, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_INNER_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INITIAL_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_AH1_IN_AC_KEY, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 7, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID_VID},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_OUTER_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_OUTER_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_VLAN_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID},
  {SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID_COMP_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_INNER_VID, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_OUTER_VID,  SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_VLAN_DOMAIN, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID}
};

static SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM
  Soc_pb_pp_isem_access_instruction_2nd[] =
{
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_DST_IP, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_DST_IP, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA, 3, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS1, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_2ND_MPLS_IN_RIF, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS1, 3, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS2, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_2ND_MPLS_IN_RIF, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS2, 3, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS3, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_2ND_MPLS_IN_RIF, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS3, 3, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_TRILL_ING_NICK, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_TRILL_ING_NICK, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_TRILL_ING_NICK, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS1, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS1_LSB(10), SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS1, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS2, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS2_LSB(10), SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS2, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS},
  {SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS3, SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS3_LSB(10), SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS3, 0, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS},
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_isem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;
  uint32
    failure;
  SOC_PB_PP_REGS
    *pp_regs = soc_pb_pp_regs();
  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA
    ihp_sem_result_accessed_tbl_data;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA
    ihp_vtt1st_key_construction_tbl_data;
  SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA
    ihp_vtt2nd_key_construction_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_REG_SET(pp_regs->ihp.isem_management_unit_configuration_reg.isem_mngmnt_unit_enable, 0x1, 10, exit);

  /* Clear failure indication of ISEM */
  SOC_PB_PP_REG_GET(pp_regs->ihp.isem_management_unit_failure_reg_0, failure, 42, exit);

  ihp_sem_result_accessed_tbl_data.sem_result_accessed = 0;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_sem_result_accessed_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  for (idx = 0; idx < sizeof(Soc_pb_pp_isem_access_instruction_1st) / sizeof(SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM); ++idx)
  {
    ihp_vtt1st_key_construction_tbl_data.and_mask          = Soc_pb_pp_isem_access_instruction_1st[idx].and_mask;
    ihp_vtt1st_key_construction_tbl_data.or_mask           = Soc_pb_pp_isem_access_instruction_1st[idx].or_mask;
    ihp_vtt1st_key_construction_tbl_data.sem_lookup_enable = Soc_pb_pp_isem_access_instruction_1st[idx].isem_lookup_enable;
    ihp_vtt1st_key_construction_tbl_data.key_inst0         = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_isem_access_instruction_1st[idx].key_inst0);
    ihp_vtt1st_key_construction_tbl_data.key_inst1         = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_isem_access_instruction_1st[idx].key_inst1);
    ihp_vtt1st_key_construction_tbl_data.key_inst2         = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_isem_access_instruction_1st[idx].key_inst2);
    ihp_vtt1st_key_construction_tbl_data.key_inst3         = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_isem_access_instruction_1st[idx].key_inst3);
    res = soc_pb_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(
            unit,
            Soc_pb_pp_isem_access_instruction_1st[idx].prog_idx,
            &ihp_vtt1st_key_construction_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  }

  for (idx = 0; idx < sizeof(Soc_pb_pp_isem_access_instruction_2nd) / sizeof(SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM); ++idx)
  {
    ihp_vtt2nd_key_construction_tbl_data.and_mask          = Soc_pb_pp_isem_access_instruction_2nd[idx].and_mask;
    ihp_vtt2nd_key_construction_tbl_data.or_mask           = Soc_pb_pp_isem_access_instruction_2nd[idx].or_mask;
    ihp_vtt2nd_key_construction_tbl_data.key_inst0         = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_isem_access_instruction_2nd[idx].key_inst0);
    ihp_vtt2nd_key_construction_tbl_data.key_inst1         = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_isem_access_instruction_2nd[idx].key_inst1);
    res = soc_pb_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(
            unit,
            Soc_pb_pp_isem_access_instruction_2nd[idx].prog_idx,
            &ihp_vtt2nd_key_construction_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_access_init_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_sem_key_extend(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY             *key,
     SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_KEY             *extnd_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  /* copy key */
  sal_memcpy(extnd_key,key, sizeof(SOC_PB_PP_ISEM_ACCESS_KEY));

  /* fix according to type */
  switch (key->key_type)
  {
    /* for VD-vlan, copy outer-vlan into inner vlan, msb will be masked*/
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
    extnd_key->key_info.l2_eth.inner_vid = extnd_key->key_info.l2_eth.outer_vid;
  break;
  /* for extended key fill out-vid with 12 lsb of raw key */
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY:
    extnd_key->key_info.l2_eth.outer_vid = SOC_SAND_GET_BITS_RANGE(extnd_key->key_info.raw.raw_val,11,0);
  break;
  default:
    break;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_sem_key_extend()", 0, 0);
}

uint32
  soc_pb_pp_isem_access_key_to_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key_in,
    SOC_SAND_OUT uint32                                  *buffer
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type;
  SOC_PB_PP_ISEM_ACCESS_KEY
    *isem_key, isem_key_s;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ISEM_ACCESS_KEY_SIZE);

  isem_key = &isem_key_s;


  res = soc_pb_pp_sem_key_extend(unit,isem_key_in,isem_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *	Special case for External-Key programs
   */
  if (soc_pb_pp_sw_db_isem_ext_key_enabled_get(unit) == TRUE)
  {
    switch(isem_key->key_type)
    {
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID_VID:
      key_type = isem_key->key_type;
      /* Override of the key type (bits 32:30) by the inner VID */
      res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      res = soc_sand_bitstream_set_any_field(&isem_key->key_info.raw.raw_val, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_VAL_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      res = soc_sand_bitstream_set_any_field(&isem_key->key_info.l2_eth.outer_vid, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = soc_sand_bitstream_set_any_field(&isem_key->key_info.l2_eth.inner_vid, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

      SOC_PB_PP_DO_NOTHING_AND_EXIT;
      break;

    default:
      break;
    }
  }

  switch(isem_key->key_type)
  {
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.mpls.label, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
    /* duplicate 10 lsb of label */
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.mpls.label, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL2_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.mpls.in_rif, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.mpls.label, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.l2_eth.vlan_domain, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.l2_eth.outer_vid, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.l2_eth.inner_vid, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.pbb.isid_domain, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.pbb.isid, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.ip_tunnel.dip, SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.raw.raw_val, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.l2_eth.outer_vid, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL:
    key_type = isem_key->key_type;
    res = soc_sand_bitstream_set_any_field(&key_type, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.trill.nick_name, SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
    res = soc_sand_bitstream_set_any_field(&isem_key->key_info.trill.nick_name, SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME2_NOF_BITS, buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR, 60, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_key_to_buffer()", 0, 0);
}

uint32
  soc_pb_pp_isem_access_key_from_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  *buffer,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key
  )
{
  uint8
    miss = FALSE;
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_CLEAR(isem_key,SOC_PB_PP_ISEM_ACCESS_KEY,1);
  
  res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, &key_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  isem_key->key_type = (SOC_PB_PP_ISEM_ACCESS_KEY_TYPE)key_type;

  switch(key_type)
  {
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_NOF_BITS, &key_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_VLAN_DOMAIN_NOF_BITS, &isem_key->key_info.l2_eth.vlan_domain);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
    if(key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID || key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID){
      res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_INNER_VID_NOF_BITS, &isem_key->key_info.l2_eth.inner_vid);
      SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
    }
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_VD_VID_VID_OUTER_VID_NOF_BITS, &isem_key->key_info.l2_eth.outer_vid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_DOMAIN_NOF_BITS, &isem_key->key_info.pbb.isid_domain);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_PBB_ISID_NOF_BITS, &isem_key->key_info.pbb.isid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_TRILL_NICK_NAME_NOF_BITS, &isem_key->key_info.trill.nick_name);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_NOF_BITS, &isem_key->key_info.mpls.label);
    SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY:
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_NOF_BITS, &isem_key->key_info.raw.raw_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if(key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID)
    {
      res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VID_OUTER_VID_NOF_BITS, &isem_key->key_info.l2_eth.outer_vid);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    }
    break;


  default:
    miss = TRUE;
    break;
  }

  /* if found type then done */
  if (!miss)
  {
    goto exit;
  }
  /* otherwise try masked values*/
  /* zero to bits*/
  key_type = key_type & 0xC;
  isem_key->key_type = (SOC_PB_PP_ISEM_ACCESS_KEY_TYPE)key_type;

  /*
   *	Special case if External-Key is enabled
   */
  if (soc_pb_pp_sw_db_isem_ext_key_enabled_get(unit) == TRUE)
  {
    switch(key_type)
    {
    case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID_VID & 0x8:
      res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW_VAL_NOF_BITS, &isem_key->key_info.raw.raw_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_INNER_VID_NOF_BITS, &isem_key->key_info.l2_eth.inner_vid);
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_EXT_RAW4VID_VID_OUTER_VID_NOF_BITS, &isem_key->key_info.l2_eth.outer_vid);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR, 60, exit);
      break;
    }
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  switch(key_type)
  {
  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF & 0xC:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_VSI_NOF_BITS, &isem_key->key_info.mpls.in_rif);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_MPLS_LABEL_NOF_BITS, &isem_key->key_info.mpls.label);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
    break;

  case SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL & 0xC:
    res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_LSB, SOC_PB_PP_ISEM_ACCESS_KEY_IP_TUNNEL_DIP_NOF_BITS, &isem_key->key_info.ip_tunnel.dip);
    SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR, 60, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_key_from_buffer()", 0, 0);
}


uint32
  soc_pb_pp_isem_access_entry_to_buffer(
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_ENTRY                   *isem_entry,
    SOC_SAND_OUT uint32                                  *buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_CLEAR(buffer, uint32, SOC_DPP_ISEM_PAYLOAD_NOF_UINT32_PETRAB);

  res = soc_sand_bitstream_set_any_field(&isem_entry->sem_result_ndx, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_LSB, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_NOF_BITS, buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  res = soc_sand_bitstream_set_any_field(&isem_entry->sem_op_code, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_LSB, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_NOF_BITS, buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_access_entry_to_buffer()", 0, 0);
}

uint32
  soc_pb_pp_isem_access_entry_from_buffer(
    SOC_SAND_IN  uint32                                  *buffer,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_ENTRY                   *isem_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_CLEAR(isem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);

  res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_LSB, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_RESULT_NDX_NOF_BITS, &isem_entry->sem_result_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_LSB, SOC_PB_PP_ISEM_ACCESS_ENTRY_ISEM_ACCESS_OP_CODE_NOF_BITS, &isem_entry->sem_op_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_access_entry_from_buffer()", 0, 0);
}

uint32
  soc_pb_pp_isem_access_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_ENTRY                   *isem_entry,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();
  uint32
    key_buffer[SOC_PB_PP_ISEM_ACCESS_KEY_SIZE];
  uint32
    entry_buffer[SOC_DPP_ISEM_PAYLOAD_NOF_UINT32_PETRAB];
  uint32
    temp;
  uint32
#ifdef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#else
    is_low_sim_active = FALSE;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(isem_key);
  SOC_SAND_CHECK_NULL_INPUT(isem_entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_isem_access_key_to_buffer(unit, isem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  *success = FALSE;
  if (!is_low_sim_active)
  {
    SOC_PB_PP_REG_BUFFER_SET(regs->ihp.isem_diagnostics_key_reg, SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE, key_buffer, 10, exit);
    SOC_PB_PP_FLD_SET(regs->ihp.isem_diagnostics_reg.isem_diagnostics_lookup, 0x1, 10, exit);
    do
    {
      SOC_PB_PP_FLD_GET(regs->ihp.isem_diagnostics_reg.isem_diagnostics_lookup, temp, 10, exit);
    } while(temp);
    SOC_PB_PP_FLD_GET(regs->ihp.isem_diagnostics_lookup_result_reg.isem_entry_found, temp, 10, exit);
    *success = SOC_SAND_NUM2BOOL(temp);
    SOC_PB_PP_FLD_GET(regs->ihp.isem_diagnostics_lookup_result_reg.isem_entry_payload, entry_buffer[0], 10, exit);
  }
  else
  {
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_get_unsafe(
            unit,
            SOC_PB_PP_ISEM_ACCESS_ADDR,
            key_buffer,
            SOC_PB_PP_ISEM_ACCESS_KEY_SIZE * sizeof(uint32),
            entry_buffer,
            SOC_DPP_ISEM_PAYLOAD_NOF_UINT32_PETRAB,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
#endif
  }

  res = soc_pb_pp_isem_access_entry_from_buffer(entry_buffer, isem_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_access_entry_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_isem_access_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_ENTRY                   *isem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp,
    buffer[SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE];
  uint32
    key_buffer[SOC_PB_PP_ISEM_ACCESS_KEY_SIZE];
  uint32
    failure,
    entry_buffer[SOC_DPP_ISEM_PAYLOAD_NOF_UINT32_PETRAB];
  SOC_PB_PP_REGS
    *pp_regs = soc_pb_pp_regs();
  uint32
#ifdef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#else
    is_low_sim_active = FALSE;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(isem_key);
  SOC_SAND_CHECK_NULL_INPUT(isem_entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_isem_access_key_to_buffer(unit, isem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  res = soc_pb_pp_isem_access_entry_to_buffer(isem_entry, entry_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  if (!is_low_sim_active)
  {
    SOC_PB_PP_FLD_GET(pp_regs->ihp.isem_interrupt_reg.isem_management_completed, temp, 10, exit);
    if (temp)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISEM_ACCESS_NOT_READY_ERR, 10, exit);
    }
  }

  *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;

  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE);

  temp = SOC_PB_PP_ISEM_ACCESS_ACTION_TYPE_INSERT;
  res = soc_sand_bitstream_set_any_field(
          &temp,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_LSB,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_LSB,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  res = soc_sand_bitstream_set_any_field(
          entry_buffer,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_PAYLOAD_LSB,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_PAYLOAD_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_PAYLOAD_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  if (!is_low_sim_active)
  {
    res = soc_sand_tbl_write_unsafe(
            unit,
            buffer,
            SOC_PB_PP_ISEM_ACCESS_ADDR,
            sizeof(buffer),
            SOC_PB_IHP_ID,
            SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

    do
    {
      SOC_PB_PP_FLD_GET(pp_regs->ihp.isem_interrupt_reg.isem_management_completed, temp, 10, exit);
    } while (temp == 0);
    /* clear management_completed by writing 1*/
    temp = 1;
    SOC_PB_PP_FLD_SET(pp_regs->ihp.isem_interrupt_reg.isem_management_completed, temp, 10, exit);

    SOC_PB_PP_FLD_GET(pp_regs->ihp.isem_management_unit_failure_reg_0.isem_mngmnt_unit_failure_valid, failure, 42, exit);
    *success = SOC_SAND_SUCCESS;
    if (failure)
    {
      SOC_PB_PP_FLD_GET(pp_regs->ihp.isem_management_unit_failure_reg_0.isem_mngmnt_unit_failure_reason, failure, 42, exit);

      switch(failure)
      {
      case 0x001:/*Notice: Cam table full*/
      case 0x002:/*Notice: Table coherency             */
      case 0x008:/*Notice: Reached max entry limit     */
      case 0x080:/*Notice: Change-fail non exist       */
      case 0x100:/*Notice: Change request over static  */
      case 0x200:/*Notice: Change non-exist from other */
      case 0x400:/*Notice: Change non-exist from self  */
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
      break;
      default:
      break;
      }
    }
#if SOC_PB_PP_ISEM_ACCESS_DEBUG
    if (failure)
    {
      switch(failure)
      {
      case 0x001:
        soc_sand_os_printf("Notice: Cam table full              ");
        break;
      case 0x002:
        soc_sand_os_printf("Notice: Table coherency             ");
        break;
      case 0x004:
        soc_sand_os_printf("Notice: Delete unknown key          ");
        break;
      case 0x008:
        soc_sand_os_printf("Notice: Reached max entry limit     ");
        break;
      case 0x010:
        soc_sand_os_printf("Notice: Inserted existing           ");
        break;
      case 0x020:
        soc_sand_os_printf("Notice: Learn request over static   ");
        break;
      case 0x040:
        soc_sand_os_printf("Notice: Learn over existing         ");
        break;
      case 0x080:
        soc_sand_os_printf("Notice: Change-fail non exist       ");
        break;
      case 0x100:
        soc_sand_os_printf("Notice: Change request over static  ");
        break;
      case 0x200:
        soc_sand_os_printf("Notice: Change non-exist from other ");
        break;
      case 0x400:
        soc_sand_os_printf("Notice: Change non-exist from self  ");
        break;
      }
    }
#endif
  }
  else
  {
    uint8
      is_success = TRUE;
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_add_unsafe(
            unit,
            SOC_PB_PP_ISEM_ACCESS_ADDR,
            key_buffer,
            SOC_PB_PP_ISEM_ACCESS_KEY_SIZE * sizeof(uint32),
            entry_buffer,
            SOC_DPP_ISEM_PAYLOAD_NOF_UINT32_PETRAB * sizeof(uint32),
            &is_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
#endif
    *success = is_success ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_access_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_isem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp,
    buffer[SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE];
  uint32
    key_buffer[SOC_PB_PP_ISEM_ACCESS_KEY_SIZE];
  SOC_PB_PP_REGS
    *pp_regs = soc_pb_pp_regs();
  uint32
#ifdef SAND_LOW_LEVEL_SIMULATION
    is_low_sim_active = soc_sand_low_is_sim_active_get();
#else
    is_low_sim_active = FALSE;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(isem_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_isem_access_key_to_buffer(unit, isem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE);

  if (!is_low_sim_active)
  {

    SOC_PB_PP_FLD_GET(pp_regs->ihp.isem_interrupt_reg.isem_management_completed, temp, 10, exit);
    if (temp)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISEM_ACCESS_NOT_READY_ERR, 10, exit);
    }
  }

  temp = SOC_PB_PP_ISEM_ACCESS_ACTION_TYPE_DELETE;
  res = soc_sand_bitstream_set_any_field(
          &temp,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_LSB,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_TYPE_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_LSB,
          SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_MSB - SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_KEY_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  if (!is_low_sim_active)
  {
    res = soc_sand_tbl_write_unsafe(
            unit,
            buffer,
            SOC_PB_PP_ISEM_ACCESS_ADDR,
            sizeof(buffer),
            SOC_PB_IHP_ID,
            SOC_PB_PP_ISEM_ACCESS_ENTRY_REQ_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

    do
    {
      SOC_PB_PP_FLD_GET(pp_regs->ihp.isem_interrupt_reg.isem_management_completed, temp, 10, exit);
    } while (temp == 0);
    /* clear management_completed by writing 1*/
    temp = 1;
    SOC_PB_PP_FLD_SET(pp_regs->ihp.isem_interrupt_reg.isem_management_completed, temp, 10, exit);
  }
#ifdef USING_CHIP_SIM
  else
  {
    res = chip_sim_exact_match_entry_remove_unsafe(
            unit,
            SOC_PB_PP_ISEM_ACCESS_ADDR,
            key_buffer,
            SOC_PB_PP_ISEM_ACCESS_KEY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_isem_access_entry_remove_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_isem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_isem_access_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_isem_access;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_isem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_isem_access_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_isem_access;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

