/* $Id: pb_pp_fp_key.c,v 1.15 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_key.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



#define SOC_PB_PP_FP_KEY_DA_MSB                            (0)
#define SOC_PB_PP_FP_KEY_DA_LSB                            (47)
#define SOC_PB_PP_FP_KEY_SA_MSB                            (48)
#define SOC_PB_PP_FP_KEY_SA_LSB                            (95)
#define SOC_PB_PP_FP_KEY_VLAN_TAG_1ST_MSB                  (112)
#define SOC_PB_PP_FP_KEY_VLAN_TAG_1ST_LSB                  (127)
#define SOC_PB_PP_FP_KEY_VLAN_TAG_2ND_MSB                  (144)
#define SOC_PB_PP_FP_KEY_VLAN_TAG_2ND_LSB                  (159)
#define SOC_PB_PP_FP_KEY_ETHERTYPE_MSB                     (96) /* Assumption: untagged packets */
#define SOC_PB_PP_FP_KEY_ETHERTYPE_LSB                     (111)
#define SOC_PB_PP_FP_KEY_ETHERTYPE_H2_MSB                  (1008) /* From Header 2 */
#define SOC_PB_PP_FP_KEY_ETHERTYPE_H2_LSB                  (1023)


#define SOC_PB_PP_FP_KEY_IPV4_NEXT_PRTCL_MSB										(72)
#define SOC_PB_PP_FP_KEY_IPV4_NEXT_PRTCL_LSB										(79)
#define SOC_PB_PP_FP_KEY_IPV4_DF_MSB                            (49)
#define SOC_PB_PP_FP_KEY_IPV4_DF_LSB                            (49)
#define SOC_PB_PP_FP_KEY_IPV4_MF_MSB                            (50)
#define SOC_PB_PP_FP_KEY_IPV4_MF_LSB                            (50)
#define SOC_PB_PP_FP_KEY_IPV4_SIP_MSB                           (96)
#define SOC_PB_PP_FP_KEY_IPV4_SIP_LSB                           (127)
#define SOC_PB_PP_FP_KEY_IPV4_DIP_MSB                           (128)
#define SOC_PB_PP_FP_KEY_IPV4_DIP_LSB                           (159)
#define SOC_PB_PP_FP_KEY_IPV4_SRC_PORT_MSB                      (0)
#define SOC_PB_PP_FP_KEY_IPV4_SRC_PORT_LSB                      (15)
#define SOC_PB_PP_FP_KEY_IPV4_DEST_PORT_MSB                     (16)
#define SOC_PB_PP_FP_KEY_IPV4_DEST_PORT_LSB                     (31)
#define SOC_PB_PP_FP_KEY_IPV4_TOS_MSB                           (8)
#define SOC_PB_PP_FP_KEY_IPV4_TOS_LSB                           (15)
#define SOC_PB_PP_FP_KEY_IPV4_TTL_MSB                           (64)
#define SOC_PB_PP_FP_KEY_IPV4_TTL_LSB                           (71)
#define SOC_PB_PP_FP_KEY_IPV4_TCP_CTL_MSB                       (266)
#define SOC_PB_PP_FP_KEY_IPV4_TCP_CTL_LSB                       (271)

#define SOC_PB_PP_FP_QUAL_HDR_IPV6_TC_MSB                           (4)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_TC_LSB                           (11)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_FLOW_LABEL_MSB                   (12)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_FLOW_LABEL_LSB                   (31)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_HOP_LIMIT_MSB                    (56)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_HOP_LIMIT_LSB                    (63)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH_MSB                     (64)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH_LSB                     (127)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW_MSB                      (128)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW_LSB                      (191)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH_MSB                     (192)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH_LSB                     (255)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW_MSB                      (256)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW_LSB                      (319)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL_MSB                   (48)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL_LSB                   (55)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL_MSB                      (426)
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL_LSB                      (431)

#define SOC_PB_PP_FP_KEY_MPLS_LABEL_MSB                          (0)
#define SOC_PB_PP_FP_KEY_MPLS_LABEL_LSB                          (31)
#define SOC_PB_PP_FP_KEY_MPLS_LABEL_ID_MSB                       (0)
#define SOC_PB_PP_FP_KEY_MPLS_LABEL_ID_LSB                       (19)
#define SOC_PB_PP_FP_KEY_MPLS_EXP_MSB                            (20)
#define SOC_PB_PP_FP_KEY_MPLS_EXP_LSB                            (22)
#define SOC_PB_PP_FP_KEY_MPLS_TTL_MSB                            (24)
#define SOC_PB_PP_FP_KEY_MPLS_TTL_LSB                            (31)
#define SOC_PB_PP_FP_KEY_MPLS_BOS_MSB                            (23)
#define SOC_PB_PP_FP_KEY_MPLS_BOS_LSB                            (23)

#define SOC_PB_PP_FP_KEY_MPLS_LABEL_SIZE_IN_BITS                 (32)
#define SOC_PB_PP_FP_KEY_MPLS_2_OFFSET                           (SOC_PB_PP_FP_KEY_MPLS_LABEL_SIZE_IN_BITS)
#define SOC_PB_PP_FP_KEY_MPLS_3_OFFSET                           (SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_SIZE_IN_BITS)
#define SOC_PB_PP_FP_KEY_MPLS_4_OFFSET                           (SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_SIZE_IN_BITS)

#define SOC_PB_PP_FP_KEY_BUFFER_SIZE                               (175)

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

typedef struct
{
  SOC_PB_PP_FP_KEY_CONSTRAINT type;
  uint32               buffer;
  uint32               msb;
  uint32               lsb;
  uint32               key_src;
} SOC_PB_PP_FP_KEY_CONSTRAINT_INFO;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_fp_key[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_LENGTH_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_LENGTH_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_LENGTH_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_LENGTH_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_ALLOC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_ALLOC_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_ALLOC_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_ALLOC_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_FREE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_FREE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_FREE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_SPACE_FREE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_DESC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_DESC_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_DESC_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_DESC_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_HEADER_NDX_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_NOF_VLAN_TAGS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_TOTAL_SIZE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_CE_INSTR_BOUNDS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_CE_INSTR_INSTALL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_NEW_DB_POSSIBLE_CONFS_GET_UNSAFE),



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_fp_key[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg' is out of range. \n\r "
    "The range is: No min - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_KEY_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_CYCLE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_KEY_CYCLE_OUT_OF_RANGE_ERR",
    "The parameter 'cycle' is out of range. \n\r "
    "The range is: No min - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_KEY_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'lsb' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_LENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_KEY_LENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'length' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_KEY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_KEY_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'key' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_PMF_KEYS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_UNKNOWN_QUAL_ERR,
    "SOC_PB_PP_FP_KEY_UNKNOWN_QUAL_ERR",
    "The requested qualifier is not supported.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_BAD_PADDING_ERR,
    "SOC_PB_PP_FP_KEY_BAD_PADDING_ERR",
    "The hardware cannot add the requested amount of padding bits.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_COPY_OVERFLOW_ERR,
    "SOC_PB_PP_FP_KEY_COPY_OVERFLOW_ERR",
    "Attempted read beyond the end of the requested qualifier.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_KEY_TOO_MANY_BITS_ERR,
    "SOC_PB_PP_FP_KEY_TOO_MANY_BITS_ERR",
    "The hardware cannot copy the requested amount of bits.\n\r"
    "The maximum number of bits per copy instruction is 32.\n\r",
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

/*
 *  This table contains the locations of the qualifiers on the device's internal data buses. The MSB
 *  of the bus is bit 0 (i.e. the MSB of the first byte of the packet header is bit 0, the LSB of
 *  the first byte of the packet header is 7, etc.)
 *  For qualifiers extracted from the packet headers, the location is relative to the beginning of
 *  the appropriate header (e.g. the MSB of the destination address in an Ethernet header is always
 *  0, regardless of the header's place in the header stack). Further calculation (and information
 *  about the packet) is needed in order to determine the qualifier's exact location on the bus.
 */
 static const SOC_PB_PP_FP_KEY_QUAL_INFO
   Soc_pb_pp_fp_key_qual_info[] =
 {
   {SOC_PB_PP_FP_QUAL_HDR_INNER_VLAN_TAG,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_VLAN_TAG_1ST_MSB, SOC_PB_PP_FP_KEY_VLAN_TAG_1ST_LSB,                                         SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_SA,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_SA_MSB, SOC_PB_PP_FP_KEY_SA_LSB,                                                             SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_DA,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_DA_MSB,  SOC_PB_PP_FP_KEY_DA_LSB,                                                            SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_ETHERTYPE,        SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_ETHERTYPE_MSB,  SOC_PB_PP_FP_KEY_ETHERTYPE_LSB,                                              SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_2ND_VLAN_TAG,     SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_VLAN_TAG_2ND_MSB,SOC_PB_PP_FP_KEY_VLAN_TAG_2ND_LSB,                                          SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_VLAN_FORMAT,            SOC_PB_PP_FP_KEY_SRC_IRPP,    844,847,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_VLAN_TAG,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_VLAN_TAG_1ST_MSB,SOC_PB_PP_FP_KEY_VLAN_TAG_1ST_LSB,                                          SOC_PB_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_SA,                     SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_SA_MSB,SOC_PB_PP_FP_KEY_SA_LSB,                                                              SOC_PB_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_DA,                     SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_DA_MSB, SOC_PB_PP_FP_KEY_DA_LSB,                                                             SOC_PB_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_ETHERTYPE,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_ETHERTYPE_H2_MSB, SOC_PB_PP_FP_KEY_ETHERTYPE_H2_LSB,                                         SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_2ND_VLAN_TAG,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_VLAN_TAG_2ND_MSB,SOC_PB_PP_FP_KEY_VLAN_TAG_2ND_LSB,                                          SOC_PB_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL,  SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_NEXT_PRTCL_MSB,  SOC_PB_PP_FP_KEY_IPV4_NEXT_PRTCL_LSB,                                  SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_DF,          SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_DF_MSB, SOC_PB_PP_FP_KEY_IPV4_DF_LSB,                                                   SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_MF,          SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_MF_MSB,  SOC_PB_PP_FP_KEY_IPV4_MF_LSB,                                                  SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_SIP,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_SIP_MSB, SOC_PB_PP_FP_KEY_IPV4_SIP_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_DIP,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_DIP_MSB, SOC_PB_PP_FP_KEY_IPV4_DIP_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_SRC_PORT,    SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_SRC_PORT_MSB, SOC_PB_PP_FP_KEY_IPV4_SRC_PORT_LSB,                                       SOC_PB_PMF_CE_SUB_HEADER_4,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_DEST_PORT,   SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_DEST_PORT_MSB, SOC_PB_PP_FP_KEY_IPV4_DEST_PORT_LSB,                                     SOC_PB_PMF_CE_SUB_HEADER_4,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_TOS,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_TOS_MSB, SOC_PB_PP_FP_KEY_IPV4_TOS_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_TTL,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_TTL_MSB, SOC_PB_PP_FP_KEY_IPV4_TTL_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_TCP_CTL,     SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_TCP_CTL_MSB, SOC_PB_PP_FP_KEY_IPV4_TCP_CTL_LSB,                                         SOC_PB_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_NEXT_PRTCL,        SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_NEXT_PRTCL_MSB, SOC_PB_PP_FP_KEY_IPV4_NEXT_PRTCL_LSB,                                   SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_DF,                SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_DF_MSB, SOC_PB_PP_FP_KEY_IPV4_DF_LSB,                                                   SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_MF,                SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_MF_MSB, SOC_PB_PP_FP_KEY_IPV4_MF_LSB,                                                   SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_SIP,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_SIP_MSB, SOC_PB_PP_FP_KEY_IPV4_SIP_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_DIP,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_DIP_MSB, SOC_PB_PP_FP_KEY_IPV4_DIP_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_SRC_PORT,          SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_SRC_PORT_MSB, SOC_PB_PP_FP_KEY_IPV4_SRC_PORT_LSB,                                       SOC_PB_PMF_CE_SUB_HEADER_3,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_DEST_PORT,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_DEST_PORT_MSB, SOC_PB_PP_FP_KEY_IPV4_DEST_PORT_LSB,                                     SOC_PB_PMF_CE_SUB_HEADER_3,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_TOS,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_TOS_MSB, SOC_PB_PP_FP_KEY_IPV4_TOS_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_TTL,               SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_TTL_MSB, SOC_PB_PP_FP_KEY_IPV4_TTL_LSB,                                                 SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_TCP_CTL,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_IPV4_TCP_CTL_MSB, SOC_PB_PP_FP_KEY_IPV4_TCP_CTL_LSB,                                         SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_HI,          SOC_PB_PP_FP_KEY_SRC_IRPP, 900, 907,                                                0,  0},
   {SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_LOW,         SOC_PB_PP_FP_KEY_SRC_IRPP, 908, 923,                                                0,  0},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH,          SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH_LSB,                             SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW_LSB,                               SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH,          SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH_LSB,                             SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW_LSB,                               SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL,        SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL_LSB,                         SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL_LSB,                               SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_L4OPS,             SOC_PB_PP_FP_KEY_SRC_IRPP, 900, 923,                                                0,  0},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_TC,                SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_TC_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_TC_LSB,                                         SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_FLOW_LABEL,        SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_FLOW_LABEL_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_FLOW_LABEL_LSB,                         SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_IPV6_HOP_LIMIT,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_QUAL_HDR_IPV6_HOP_LIMIT_MSB, SOC_PB_PP_FP_QUAL_HDR_IPV6_HOP_LIMIT_LSB,                           SOC_PB_PMF_CE_SUB_HEADER_2,  SOC_PB_PMF_CE_SUB_HEADER_3},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_FWD,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_LABEL_MSB, SOC_PB_PP_FP_KEY_MPLS_LABEL_LSB,                                            SOC_PB_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_ID_FWD,         SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_LABEL_ID_MSB, SOC_PB_PP_FP_KEY_MPLS_LABEL_ID_LSB,                                            SOC_PB_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP_FWD,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_EXP_MSB, SOC_PB_PP_FP_KEY_MPLS_EXP_LSB,                                                SOC_PB_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL_FWD,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_TTL_MSB, SOC_PB_PP_FP_KEY_MPLS_TTL_LSB,                                                SOC_PB_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS_FWD,           SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_BOS_MSB, SOC_PB_PP_FP_KEY_MPLS_BOS_LSB,                                                SOC_PB_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL1,            SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_LABEL_MSB, SOC_PB_PP_FP_KEY_MPLS_LABEL_LSB,                                            SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP1,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_EXP_MSB, SOC_PB_PP_FP_KEY_MPLS_EXP_LSB,                                                SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL1,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_TTL_MSB, SOC_PB_PP_FP_KEY_MPLS_TTL_LSB,                                                SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS1,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_BOS_MSB, SOC_PB_PP_FP_KEY_MPLS_BOS_LSB,                                                SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL2,            SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_MSB, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_LSB,  SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP2,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_EXP_MSB, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_EXP_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL2,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_TTL_MSB, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_TTL_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS2,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_BOS_MSB, SOC_PB_PP_FP_KEY_MPLS_2_OFFSET + SOC_PB_PP_FP_KEY_MPLS_BOS_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL3,            SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_MSB, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_LSB,  SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP3,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_EXP_MSB, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_EXP_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL3,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_TTL_MSB, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_TTL_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS3,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_BOS_MSB, SOC_PB_PP_FP_KEY_MPLS_3_OFFSET + SOC_PB_PP_FP_KEY_MPLS_BOS_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL4,            SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_MSB, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_LABEL_LSB,  SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP4,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_EXP_MSB, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_EXP_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL4,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_TTL_MSB, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_TTL_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS4,              SOC_PB_PP_FP_KEY_SRC_PKT_HDR, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_BOS_MSB, SOC_PB_PP_FP_KEY_MPLS_4_OFFSET + SOC_PB_PP_FP_KEY_MPLS_BOS_LSB,      SOC_PB_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_SRC_TM_PORT,           SOC_PB_PP_FP_KEY_SRC_IRPP,    73,                                                      79,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_SRC_SYST_PORT,         SOC_PB_PP_FP_KEY_SRC_IRPP,    195,                                                    207,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT,           SOC_PB_PP_FP_KEY_SRC_IRPP,    66,                                                      71,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_PROCESSING_TYPE,       SOC_PB_PP_FP_KEY_SRC_IRPP,    209,                                                    211,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_PKT_HDR_TYPE,          SOC_PB_PP_FP_KEY_SRC_IRPP,    138,                                                    143,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_ETH_TAG_FORMAT,        SOC_PB_PP_FP_KEY_SRC_IRPP,    844,                                                    847,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_DEST,          SOC_PB_PP_FP_KEY_SRC_IRPP,    218,                                                    235,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_TC,            SOC_PB_PP_FP_KEY_SRC_IRPP,    236,                                                    239,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_DP,            SOC_PB_PP_FP_KEY_SRC_IRPP,    241,                                                    243,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE, SOC_PB_PP_FP_KEY_SRC_IRPP,    260,                                                    267,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL, SOC_PB_PP_FP_KEY_SRC_IRPP,    246,                                                    259,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_UP,                    SOC_PB_PP_FP_KEY_SRC_IRPP,    269,                                                    271,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_SNOOP_CODE,            SOC_PB_PP_FP_KEY_SRC_IRPP,    276,                                                    283,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_LEARN_DECISION_DEST,   SOC_PB_PP_FP_KEY_SRC_IRPP,    580,                                                    595,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_LEARN_ADD_INFO,        SOC_PB_PP_FP_KEY_SRC_IRPP,    596,                                                    619,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_IN_LIF,                SOC_PB_PP_FP_KEY_SRC_IRPP,    924,                                                    937,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_LL_MIRROR_CMD,         SOC_PB_PP_FP_KEY_SRC_IRPP,    284,                                                    287,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_SYSTEM_VSI,            SOC_PB_PP_FP_KEY_SRC_IRPP,    320,                                                    335,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_ORIENTATION_IS_HUB,    SOC_PB_PP_FP_KEY_SRC_IRPP,    339,                                                    339,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_VLAN_ID,               SOC_PB_PP_FP_KEY_SRC_IRPP,    352,                                                    363,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_VLAN_PCP,              SOC_PB_PP_FP_KEY_SRC_IRPP,    348,                                                    350,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_VLAN_DEI,              SOC_PB_PP_FP_KEY_SRC_IRPP,    351,                                                    351,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_STP_STATE,             SOC_PB_PP_FP_KEY_SRC_IRPP,    366,                                                    367,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_FWD_TYPE,              SOC_PB_PP_FP_KEY_SRC_IRPP,    380,                                                    383,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_SUB_HEADER_NDX,        SOC_PB_PP_FP_KEY_SRC_IRPP,    377,                                                    379,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_KEY_CHANGED,           SOC_PB_PP_FP_KEY_SRC_IRPP,      0,                                                     11,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_IN_RIF,                SOC_PB_PP_FP_KEY_SRC_IRPP,    416,                                                    427,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_VRF,                   SOC_PB_PP_FP_KEY_SRC_IRPP,    428,                                                    435,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_PCKT_IS_COMP_MC,       SOC_PB_PP_FP_KEY_SRC_IRPP,    439,                                                    439,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_MY_BMAC,               SOC_PB_PP_FP_KEY_SRC_IRPP,    371,                                                    371,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_IN_TTL,                SOC_PB_PP_FP_KEY_SRC_IRPP,    440,                                                    447,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_IN_DSCP_EXP,           SOC_PB_PP_FP_KEY_SRC_IRPP,    448,                                                    455,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_PACKET_SIZE_RANGE,     SOC_PB_PP_FP_KEY_SRC_IRPP,    898,                                                    899,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_TERM_TYPE,             SOC_PB_PP_FP_KEY_SRC_IRPP,    384,                                                    387,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_OUT_LIF,                    SOC_PB_PP_FP_KEY_SRC_IRPP,    415-16+1, /*388 in theory for EEI */                    415,                                                0,                       0},
   {SOC_PB_PP_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD,      SOC_PB_PP_FP_KEY_SRC_IRPP,    710,                                                    735,                                                0,                       0}
 };

static const uint32
  Soc_pb_pp_fp_key_qual_info_size = sizeof(Soc_pb_pp_fp_key_qual_info) / sizeof(SOC_PB_PP_FP_KEY_QUAL_INFO);

static const SOC_PB_PP_FP_KEY_CONSTRAINT_INFO
  Soc_pb_pp_fp_key_constraints_info[] =
{
  {SOC_PB_PP_FP_KEY_CONS_TCAM_72B,  0, 71,  0,  SOC_PB_PMF_TCAM_KEY_SRC_A_71_0       },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_72B,  0, 103, 32, SOC_PB_PMF_TCAM_KEY_SRC_A_103_32     },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_144B, 0, 143, 0,  SOC_PB_PMF_TCAM_KEY_SRC_A_143_0      },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_144B, 0, 175, 32, SOC_PB_PMF_TCAM_KEY_SRC_A_175_32     },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_72B,  1, 71,  0,  SOC_PB_PMF_TCAM_KEY_SRC_B_71_0       },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_72B,  1, 103, 32, SOC_PB_PMF_TCAM_KEY_SRC_B_103_32     },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_144B, 1, 143, 0,  SOC_PB_PMF_TCAM_KEY_SRC_B_143_0      },
  {SOC_PB_PP_FP_KEY_CONS_TCAM_144B, 1, 175, 32, SOC_PB_PMF_TCAM_KEY_SRC_B_175_32     },
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   0, 9,   0,  SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0  },
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   0, 19,  10, SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10},
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   0, 29,  20, SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20},
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   0, 39,  30, SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30},
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   1, 9,   0,  SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0  },
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   1, 19,  10, SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10},
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   1, 29,  20, SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20},
  {SOC_PB_PP_FP_KEY_CONS_DIR_TBL,   1, 39,  30, SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30},
  {SOC_PB_PP_FP_KEY_CONS_FEM,       0, 31,  0,  SOC_PB_PMF_FEM_INPUT_SRC_A_31_0      },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       0, 47,  16, SOC_PB_PMF_FEM_INPUT_SRC_A_47_16     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       0, 63,  32, SOC_PB_PMF_FEM_INPUT_SRC_A_63_32     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       0, 79,  48, SOC_PB_PMF_FEM_INPUT_SRC_A_79_48     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       0, 95,  64, SOC_PB_PMF_FEM_INPUT_SRC_A_95_64     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       1, 31,  0,  SOC_PB_PMF_FEM_INPUT_SRC_B_31_0      },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       1, 47,  16, SOC_PB_PMF_FEM_INPUT_SRC_B_47_16     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       1, 63,  32, SOC_PB_PMF_FEM_INPUT_SRC_B_63_32     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       1, 79,  48, SOC_PB_PMF_FEM_INPUT_SRC_B_79_48     },
  {SOC_PB_PP_FP_KEY_CONS_FEM,       1, 95,  64, SOC_PB_PMF_FEM_INPUT_SRC_B_95_64     },
  {SOC_PB_PP_FP_KEY_CONS_TAG,       0, 29,  0,  SOC_PB_PMF_TAG_VAL_SRC_A_29_0        },
  {SOC_PB_PP_FP_KEY_CONS_TAG,       0, 61,  32, SOC_PB_PMF_TAG_VAL_SRC_A_61_32       },
  {SOC_PB_PP_FP_KEY_CONS_TAG,       1, 29,  0,  SOC_PB_PMF_TAG_VAL_SRC_B_29_0        },
  {SOC_PB_PP_FP_KEY_CONS_TAG,       1, 61,  32, SOC_PB_PMF_TAG_VAL_SRC_B_61_32       }
};

static const uint32
  Soc_pb_pp_fp_key_constraints_info_size = sizeof(Soc_pb_pp_fp_key_constraints_info) / sizeof(SOC_PB_PP_FP_KEY_CONSTRAINT_INFO);

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


uint8
  soc_pb_pp_fp_key_qual_info_find(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE     qual_type,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_QUAL_INFO *qual_info
  )
{
  uint32
    idx;
  uint8
    found = FALSE;

  if (SOC_PB_PP_FP_QUAL_HDR_USER_DEF_0 <= qual_type && qual_type <= SOC_PB_PP_FP_QUAL_HDR_USER_DEF_9)
  {
    soc_pb_pp_sw_db_fp_db_udf_get(
      unit,
      qual_type - SOC_PB_PP_FP_QUAL_HDR_USER_DEF_0,
      qual_info
    );
    found = TRUE;
  }
  else
  {
    for (idx = 0; !found && idx < Soc_pb_pp_fp_key_qual_info_size; ++idx)
    {
      if (Soc_pb_pp_fp_key_qual_info[idx].qual_type == qual_type)
      {
        *qual_info = Soc_pb_pp_fp_key_qual_info[idx];
        found = TRUE;
      }
    }
  }

  if (qual_type == SOC_PB_PP_FP_QUAL_IRPP_KEY_CHANGED)
  {
    qual_info->lsb = soc_pb_pp_sw_db_fp_key_change_size_get(unit) - 1;
  }

  return found;
}

/*
 * Find the Header index per PFG and qualifier type
 */
STATIC
 uint32
  soc_pb_pp_fp_key_header_ndx_find(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE   qual_type,
    SOC_SAND_OUT SOC_PB_PMF_CE_SUB_HEADER *header_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    qual_info_found;
  SOC_PB_PP_FP_PFG_INFO
    pfg_info;
  SOC_PB_PMF_CE_SUB_HEADER
    header_ndx_lcl;
  SOC_PB_PP_FP_KEY_QUAL_INFO
    qual_info;
  SOC_PB_PP_FP_PKT_HDR_TYPE
    pkt_hdr_type;
  uint32
    pkt_hdr_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_HEADER_NDX_FIND);

  /*
   *  Get the PFG structure
   */
  SOC_PB_PP_FP_PFG_INFO_clear(&pfg_info);
  res = soc_pb_pp_fp_packet_format_group_get_unsafe(
          unit,
          pfg_ndx,
          &pfg_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Get the Qualifier information
   */
  qual_info_found = soc_pb_pp_fp_key_qual_info_find(
                      unit,
                      qual_type,
                      &qual_info
                    );
  if (!qual_info_found)
  {
    /* This is an internal error, it should never happen */
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_KEY_UNKNOWN_QUAL_ERR, 20, exit);
  }

  /*
   * Set the first header by default, and find if it should not be the case
   */
  header_ndx_lcl = qual_info.header_ndx_0;

  switch (qual_type)
  {
  case SOC_PB_PP_FP_QUAL_HDR_INNER_VLAN_TAG:
  case SOC_PB_PP_FP_QUAL_HDR_INNER_SA:
  case SOC_PB_PP_FP_QUAL_HDR_INNER_DA:
  case SOC_PB_PP_FP_QUAL_HDR_INNER_ETHERTYPE:
  case SOC_PB_PP_FP_QUAL_HDR_INNER_2ND_VLAN_TAG:
    /*
     * Find the inner Ethernet location, with the
     * assumption it is in the second header by default
     */
    for (pkt_hdr_type_ndx = 0; pkt_hdr_type_ndx < SOC_PB_PP_NOF_FP_PKT_HDR_TYPES; pkt_hdr_type_ndx++)
    {
      pkt_hdr_type = 1 << pkt_hdr_type_ndx;
      if ((pfg_info.hdr_format_bmp & pkt_hdr_type) == FALSE)
      {
        continue;
      }

      if ((pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_TRILL_ETH))
      {
        header_ndx_lcl = qual_info.header_ndx_1;
      }
    }
  	break;

  case SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH:
  case SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW:
  case SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH:
  case SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW:
  case SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL:
  case SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL:
    /*
     * Find the IPv6 location, with the
     * assumption it is in the second header by default
     */
    for (pkt_hdr_type_ndx = 0; pkt_hdr_type_ndx < SOC_PB_PP_NOF_FP_PKT_HDR_TYPES; pkt_hdr_type_ndx++)
    {
      pkt_hdr_type = 1 << pkt_hdr_type_ndx;
      if ((pfg_info.hdr_format_bmp & pkt_hdr_type) == FALSE)
      {
        continue;
      }

      if ((pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH))
      {
        header_ndx_lcl = qual_info.header_ndx_1;
      }
    }
  	break;

  case SOC_PB_PP_FP_QUAL_HDR_IPV4_NEXT_PRTCL:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_DF:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_MF:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_SIP:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_DIP:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_SRC_PORT:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_DEST_PORT:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_TOS:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_TTL:
  case SOC_PB_PP_FP_QUAL_HDR_IPV4_TCP_CTL:
    /*
     * Find the outer IPv4 location, with the
     * assumption it is in the second header by default
     */
    for (pkt_hdr_type_ndx = 0; pkt_hdr_type_ndx < SOC_PB_PP_NOF_FP_PKT_HDR_TYPES; pkt_hdr_type_ndx++)
    {
      pkt_hdr_type = 1 << pkt_hdr_type_ndx;
      if ((pfg_info.hdr_format_bmp & pkt_hdr_type) == FALSE)
      {
        continue;
      }

      if ((pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH)
          || (pkt_hdr_type == SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH))
      {
        header_ndx_lcl = qual_info.header_ndx_1;
      }
    }
  	break;

  default:
    break;
  }

  *header_ndx = header_ndx_lcl;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_header_ndx_find()", 0, 0);
}



/*
 * Find the Header index per PFG and qualifier type
 */
STATIC
 uint32
  soc_pb_pp_fp_nof_vlan_tags_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            pfg_ndx,
    SOC_SAND_IN  uint8            is_first_header,
    SOC_SAND_OUT uint32            *nof_tags
  )
{
  uint32
    nof_tags_lcl,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_PFG_INFO
    pfg_info;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    vlan_format_ndx;
  SOC_PB_PP_FP_CONTROL_INDEX
    ctrl_ndx;
  SOC_PB_PP_FP_CONTROL_INFO
    ctrl_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_NOF_VLAN_TAGS_GET);

  SOC_PB_PP_FP_CONTROL_INDEX_clear(&ctrl_ndx);
  SOC_PB_PP_FP_CONTROL_INFO_clear(&ctrl_info);

  /*
   *  Get the PFG structure
   */
  SOC_PB_PP_FP_PFG_INFO_clear(&pfg_info);
  res = soc_pb_pp_fp_packet_format_group_get_unsafe(
          unit,
          pfg_ndx,
          &pfg_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Get the number of tags:
   *  1. If first header, according to the PFG
   *  2. If afterwards, then according to the control API
   */
  nof_tags_lcl = 0;
  if (is_first_header == TRUE)
  {
    for (vlan_format_ndx = 0; vlan_format_ndx < SOC_PB_NOF_ETHERNET_FRAME_VLAN_FORMATS_HW; vlan_format_ndx++)
    {
      if ((pfg_info.vlan_tag_structure_bmp & (1 << vlan_format_ndx)) != 0)
      {
        /* A VLAN format has been found, set according to it*/
        nof_tags_lcl = SOC_SAND_PP_NOF_TAGS_IN_VLAN_FORMAT(vlan_format_ndx);
        break;
      }
    }
  }
  else
  {
    /*
     * Get the number of tags according to the control API
     */
    ctrl_ndx.type    = SOC_PB_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS;
    ctrl_ndx.val_ndx = pfg_ndx;
    res = soc_pb_pp_fp_control_get_unsafe(
            unit,
            &ctrl_ndx,
            &ctrl_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    nof_tags_lcl = ctrl_info.val[0];
  }

  *nof_tags = nof_tags_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_nof_vlan_tags_get()", 0, 0);
}


STATIC uint32
  soc_pb_pp_fp_key_qual_net_length_get(
    SOC_SAND_IN uint32 msb,
    SOC_SAND_IN uint32 lsb
  )
{
  return lsb - msb + 1;
}

STATIC uint32
  soc_pb_pp_fp_key_nibble_offset_get(
    SOC_SAND_IN uint32 lsb
  )
{
  uint32
    ret_val = lsb / 4;

  if (lsb % 4 != 0)
  {
    ++ret_val;
  }

  return ret_val - 8;
}

STATIC uint32
  soc_pb_pp_fp_key_qual_padded_length_get(
    SOC_SAND_IN uint32 msb,
    SOC_SAND_IN uint32 lsb
  )
{
  uint32
    nibble_offset,
    padded_lsb,
    padded_length;

  nibble_offset = soc_pb_pp_fp_key_nibble_offset_get(
                    lsb
                  );
  padded_lsb    = (nibble_offset + 8) * 4 - 1;
  padded_length = soc_pb_pp_fp_key_qual_net_length_get(
                    msb,
                    padded_lsb
                  );

  return padded_length;
}

STATIC uint32
  soc_pb_pp_fp_key_total_size_get(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *db_info,
    SOC_SAND_OUT uint32              *nof_quals,
    SOC_SAND_OUT uint32              *nof_instrs,
    SOC_SAND_OUT uint32              *nof_bits
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    length,
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_TOTAL_SIZE_GET);
  
  *nof_instrs = 0;
  *nof_quals  = 0;
  *nof_bits   = 0;
  for (idx = 0; idx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++idx)
  {
    if (db_info->qual_types[idx] != BCM_FIELD_ENTRY_INVALID)
    {
      res = soc_pb_pp_fp_key_length_get_unsafe(
              unit,
              db_info->qual_types[idx],
              TRUE,
              &length
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      ++*nof_quals;
      ++*nof_instrs;
      *nof_bits += length;
      if (length > 32)
      {
        ++*nof_instrs;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_total_size_get()", 0, 0);
}

STATIC uint32
  soc_pb_pp_fp_key_free_segments_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            pfg_ndx,
    SOC_SAND_IN  uint32            start_instr,
    SOC_SAND_IN  uint32            end_instr,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_SEGMENT free_segments[SOC_PB_PP_FP_KEY_NOF_FREE_SEGMENTS_IN_BUFFER_MAX * 2]
  )
{
  uint32
    buffer,
    idx,
    la_idx,
    instr_id = start_instr,
    la_instr_id,
    lsb,
    nof_free_segments;
  uint8
    in_free_seg;
  SOC_PB_SW_DB_PMF_CE
    instr,
    la_instr;

  nof_free_segments = 0;
  /*
   *  Loop over both buffers
   */
  for (buffer = 0; buffer < 2; ++buffer)
  {
    /*
     *  Loop over the copy engine instructions in the given range
     */
    in_free_seg = FALSE;
    lsb         = 0;
    for (idx = 0; idx < start_instr - end_instr + 1; ++ idx)
    {
      instr_id = start_instr - idx;
      soc_pb_sw_db_pgm_ce_instr_get(
        unit,
        pfg_ndx,
        buffer,
        instr_id,
        &instr
      );

      if (in_free_seg)
      {
        if (instr.is_used)
        {
          /*
           *  Entering an allocated segment
           */
          free_segments[nof_free_segments].end_instr    = instr_id + 1;
          free_segments[nof_free_segments].msb          = lsb;
          free_segments[nof_free_segments].const_length = (instr_id > 3) ? TRUE : FALSE;
          in_free_seg                                   = FALSE;

          if (instr_id <= 3)
          {
            for (la_idx = 0; la_idx < instr_id + 1; ++la_idx)
            {
              la_instr_id = instr_id - la_idx;
              soc_pb_sw_db_pgm_ce_instr_get(
                unit,
                pfg_ndx,
                buffer,
                la_instr_id,
                &la_instr
              );

              if (la_instr.is_used && la_instr.is_used_in_cycle_1)
              {
                free_segments[nof_free_segments].const_length = TRUE;
              }
            }

            if (!free_segments[nof_free_segments].const_length)
            {
              free_segments[nof_free_segments].msb = SOC_PB_PP_FP_KEY_BUFFER_SIZE;
            }
          }

          ++nof_free_segments;
        }
      }
      else
      {
        if (!instr.is_used)
        {
          /*
           *  Entering a free segment
           */
          in_free_seg                                  = TRUE;
          free_segments[nof_free_segments].start_instr = instr_id;
          free_segments[nof_free_segments].lsb         = lsb;
          free_segments[nof_free_segments].key         = buffer;
        }
      }

      if (instr.is_valid)
      {
        lsb += instr.bit_count + 1;
      }
    }

    /*
     *  Handle an open free segment at the end of the buffer
     */
    if (in_free_seg)
    {
      free_segments[nof_free_segments].end_instr    = instr_id;
      free_segments[nof_free_segments].msb          = SOC_PB_PP_FP_KEY_BUFFER_SIZE;
      free_segments[nof_free_segments].const_length = FALSE;
      ++nof_free_segments;
    }
  }

  return nof_free_segments;
}

STATIC uint8
  soc_pb_pp_fp_key_in_segment_place(
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_CONSTRAINT constraint,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_SEGMENT    *segment,
    SOC_SAND_OUT uint32               *msb,
    SOC_SAND_OUT uint32               *lsb,
    SOC_SAND_OUT uint32               *frame_msb,
    SOC_SAND_OUT uint32               *frame_lsb,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_LOCATION   *key_src
  )
{
  uint32
    idx,
    curr_msb,
    curr_lsb,
    curr_length,
    best_length;

  best_length = 0;
  for (idx = 0; idx < Soc_pb_pp_fp_key_constraints_info_size; ++idx)
  {
    if (Soc_pb_pp_fp_key_constraints_info[idx].type == constraint
          && Soc_pb_pp_fp_key_constraints_info[idx].buffer == segment->key)
    {
      curr_msb    = SOC_SAND_MIN(Soc_pb_pp_fp_key_constraints_info[idx].msb, segment->msb);
      curr_lsb    = SOC_SAND_MAX(Soc_pb_pp_fp_key_constraints_info[idx].lsb, segment->lsb);
      curr_length = (curr_msb > curr_lsb) ? curr_msb - curr_lsb + 1 : 0;
      if (curr_length > best_length)
      {
        *msb        = curr_msb;
        *lsb        = curr_lsb;
        *frame_msb  = Soc_pb_pp_fp_key_constraints_info[idx].msb;
        *frame_lsb  = Soc_pb_pp_fp_key_constraints_info[idx].lsb;
        key_src->fem_src = Soc_pb_pp_fp_key_constraints_info[idx].key_src;
        best_length = curr_length;
      }
    }
  }

  return (best_length > 0) ? TRUE : FALSE;
}

/*********************************************************************
*     Returns the number of bits required to hold a qualifier
 *     of type qual_type. If with_padding is set then the
 *     returned value is the actual number of bits required
 *     (with extra padding due to hardware limitations),
 *     otherwise the returned value is the exact number of bits
 *     required to store the qualifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_key_length_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE     qual_type,
    SOC_SAND_IN  uint8              with_padding,
    SOC_SAND_OUT uint32              *length
  )
{
  uint8
    qual_info_found;
  SOC_PB_PP_FP_KEY_QUAL_INFO
    qual_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_LENGTH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(length);

  qual_info_found = soc_pb_pp_fp_key_qual_info_find(
                      unit,
                      qual_type,
                      &qual_info
                    );
  if (!qual_info_found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_KEY_UNKNOWN_QUAL_ERR, 10, exit);
  }

  if (!with_padding)
  {
    *length = soc_pb_pp_fp_key_qual_net_length_get(
                qual_info.msb,
                qual_info.lsb
              );
  }
  else
  {
    *length = soc_pb_pp_fp_key_qual_padded_length_get(
                qual_info.msb,
                qual_info.lsb
              );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_length_get_unsafe()", 0, 0);
}

STATIC uint32
  soc_pb_pp_fp_key_start_instr_get(
    SOC_SAND_IN uint32 cycle,
    SOC_SAND_IN SOC_PB_PP_FP_KEY_CONSTRAINT key_cons
  )
{
  return ((cycle == 1) || (key_cons == SOC_PB_PP_FP_KEY_CONS_FEM) || (key_cons == SOC_PB_PP_FP_KEY_CONS_TAG)) ? 7 : 3;
}

STATIC uint8
  soc_pb_pp_fp_key_location_determine(
    SOC_SAND_IN    int               unit,
    SOC_SAND_IN    uint32               pfg_ndx,
    SOC_SAND_IN    uint32               cycle,
    SOC_SAND_IN    SOC_PB_PP_FP_KEY_CONSTRAINT key_cons,
    SOC_SAND_IN    uint32               key_length,
    SOC_SAND_IN    uint32               nof_instrs,
    SOC_SAND_INOUT uint32               *lsb,
    SOC_SAND_INOUT uint32               *prefix_size,
    SOC_SAND_OUT   SOC_PB_PP_FP_KEY_SEGMENT    *key_seg,
    SOC_SAND_OUT   SOC_PB_PP_FP_KEY_LOCATION   *key_src,
    SOC_SAND_OUT   uint32               *frame_lsb
  )
{
  uint32
    start_instr,
    nof_free_segs,
    idx,
    seg_msb=0,
    seg_lsb=0,
    frame_msb=0,
    req_instrs;
  SOC_PB_PP_FP_KEY_SEGMENT
    free_segments[SOC_PB_PP_FP_KEY_NOF_FREE_SEGMENTS_IN_BUFFER_MAX * 2];
  uint8
    found_place,
    seg_valid;

  /*
   *  Make a list of all the free buffer segments
   */
  start_instr   = soc_pb_pp_fp_key_start_instr_get(
                    cycle,
                    key_cons
                  );
  nof_free_segs = soc_pb_pp_fp_key_free_segments_get(
                    unit,
                    pfg_ndx,
                    start_instr,
                    0,
                    free_segments
                  );
  
  found_place = FALSE;
  for (idx = 0; !found_place && idx < nof_free_segs; ++idx)
  {
    seg_valid = soc_pb_pp_fp_key_in_segment_place(
                  key_cons,
                  &free_segments[idx],
                  &seg_msb,
                  &seg_lsb,
                  &frame_msb,
                  frame_lsb,
                  key_src
                );

    if (seg_valid)
    {
      /*
       *  If we can position the LSB in its proper place (or we just don't care)...
       */
      if (*lsb == SOC_SAND_UINT_MAX)
      {
        *lsb = seg_lsb - *frame_lsb;
      }
      if (*lsb >= seg_lsb - *frame_lsb)
      {
        /*
         *  ... and if there's enough space in this segment...
         */
        req_instrs = (*lsb > seg_lsb - *frame_lsb) ? nof_instrs + SOC_SAND_DIV_ROUND_UP((*lsb - (seg_lsb - *frame_lsb)), SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX) : nof_instrs;
        if (((seg_msb - seg_lsb + 1) >= key_length)
              && (req_instrs <= free_segments[idx].start_instr - free_segments[idx].end_instr + 1))
        {
          /*
           *  ... and if there's enough place for the prefix (or we just don't care)...
           */
          if (*prefix_size == SOC_SAND_UINT_MAX)
          {
            *prefix_size = SOC_SAND_MIN(frame_msb - *frame_lsb - *lsb - key_length + 1, 4);
          }
          if (*prefix_size <= SOC_SAND_MIN(frame_msb - *frame_lsb - *lsb - key_length + 1, 4))
          {
            /*
             *  ... then we finally found a place for this key!
             */
            found_place = TRUE;
            *key_seg    = free_segments[idx];
          }
        }
      }
    }
  }

  return found_place;
}

STATIC uint32
  soc_pb_pp_fp_key_ce_instr_set(
    SOC_SAND_IN    int          unit,
    SOC_SAND_IN    uint32          db_id_ndx,
    SOC_SAND_IN    uint32          pfg_ndx,
    SOC_SAND_IN    uint32          key,
    SOC_SAND_IN    SOC_PB_PP_FP_QUAL_TYPE qual_type,
    SOC_SAND_IN    uint8          is_used_in_cycle_1,
    SOC_SAND_INOUT uint32          *instr
  )
{
  uint32
    res = SOC_SAND_OK,
    nof_vlan_tags = 0;
  uint32
    nibble_offset,
    padded_length;
  uint8
    qual_info_found;
  SOC_PB_PP_FP_KEY_QUAL_INFO
    qual_info;
  SOC_PB_SW_DB_PMF_CE
    ce_instr;
  SOC_PB_PMF_CE_SUB_HEADER
    header_ndx = SOC_PB_PMF_CE_SUB_HEADER_0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  qual_info_found = soc_pb_pp_fp_key_qual_info_find(
                      unit,
                      qual_type,
                      &qual_info
                    );
  if (!qual_info_found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_KEY_UNKNOWN_QUAL_ERR, 10, exit);
  }

  if (qual_info.data_src == SOC_PB_PP_FP_KEY_SRC_PKT_HDR)
  {
    /* Petra b code. Almost not in use. Ignore coverity defects */
    /* coverity[returned_value] */
    res = soc_pb_pp_fp_key_header_ndx_find(
            unit,
            pfg_ndx,
            qual_type,
            &header_ndx
          );
  }

  nibble_offset = soc_pb_pp_fp_key_nibble_offset_get(
                    qual_info.lsb
                  );
  padded_length = soc_pb_pp_fp_key_qual_padded_length_get(
                    qual_info.msb,
                    qual_info.lsb
                  );

  ce_instr.is_used              = TRUE;
  ce_instr.is_valid             = TRUE;
  ce_instr.is_used_in_cycle_1   = is_used_in_cycle_1;
  ce_instr.is_irpp              = qual_info.data_src == SOC_PB_PP_FP_KEY_SRC_IRPP ? TRUE : FALSE;
  ce_instr.header_offset_select = header_ndx;
  ce_instr.nibble_field_offset  = nibble_offset & 0xff; /* Trim to accommodate negative values */
  ce_instr.bit_count            = SOC_SAND_MIN(padded_length - 1, 31);
  ce_instr.db_id_ndx            = db_id_ndx;

  if (qual_type == SOC_PB_PP_FP_QUAL_HDR_INNER_ETHERTYPE)
  {
    res = soc_pb_pp_fp_nof_vlan_tags_get(
            unit,
            pfg_ndx,
            FALSE,
            &nof_vlan_tags
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    ce_instr.nibble_field_offset += nof_vlan_tags * 8;
  }

  soc_pb_sw_db_pgm_ce_instr_set(
    unit,
    pfg_ndx,
    key,
    *instr,
    &ce_instr
  );

  if (qual_info.lsb - qual_info.msb > 31)
  {
    ce_instr.nibble_field_offset = (nibble_offset - 8) & 0xff;
    ce_instr.bit_count           = (padded_length - 1) - 32;

    soc_pb_sw_db_pgm_ce_instr_set(
      unit,
      pfg_ndx,
      key,
      --*instr,
      &ce_instr
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_ce_instr_set()", 0, 0);
}

/*********************************************************************
*     Allocates a place in one of the PMF custom keys for the
 *     qualifiers of a field processor database, and configures
 *     the relevant copy engines (in the software database) to
 *     copy the qualifiers to that position.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_key_space_alloc_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               pfg_ndx,
    SOC_SAND_IN  uint32               db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO  *db_info,
    SOC_SAND_IN  uint32               cycle,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_CONSTRAINT key_cons,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    empty_instr_size,
    nof_quals,
    nof_instrs,
    nof_bits,
    idx,
    qual_idx,
    instr_idx,
    lsb = SOC_SAND_UINT_MAX,
    prefix_size = 0,
    curr_lsb,
    padding,
    length,
    frame_lsb = 0;
  uint8
    db_has_format,
    found_place;
  SOC_PB_PP_FP_KEY_SEGMENT
    key_seg;
  SOC_PB_PP_FP_KEY_DESC
    key_desc;
  SOC_PB_SW_DB_PMF_CE
    ce_instr;
  SOC_PB_PP_FP_KEY_LOCATION
    key_src;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_SPACE_ALLOC_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  sal_memset(&key_seg, 0x0, sizeof(SOC_PB_PP_FP_KEY_SEGMENT));

  db_has_format = soc_pb_pp_sw_db_fp_db_has_key_format_get(
                    unit,
                    db_id_ndx
                  );
  if (db_has_format)
  {
    soc_pb_pp_sw_db_fp_key_desc_get(
      unit,
      db_id_ndx,
      &key_desc
    );
    for (qual_idx = 0 ; (qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (db_info->qual_types[qual_idx] == BCM_FIELD_ENTRY_INVALID); ++qual_idx);
    /* Petra code. Not in use. Ignore coverity defects */
    /* coverity[overrun-local : FALSE] */
    lsb = key_desc.fields[qual_idx].lsb;
  }
  res = soc_pb_pp_fp_key_total_size_get(
          unit,
          db_info,
          &nof_quals,
          &nof_instrs,
          &nof_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  found_place = soc_pb_pp_fp_key_location_determine(
                  unit,
                  pfg_ndx,
                  cycle,
                  key_cons,
                  nof_bits,
                  nof_instrs,
                  &lsb,
                  &prefix_size,
                  &key_seg,
                  &key_src,
                  &frame_lsb
                );

  if (!found_place)
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   *  Check whether the first qualifier is the result of a previous action
   */
  for (qual_idx = 0; qual_idx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; qual_idx++)
  {
    if (db_info->qual_types[qual_idx] == BCM_FIELD_ENTRY_INVALID)
    {
      break;
    }
    if (db_info->qual_types[qual_idx] == SOC_PB_PP_FP_QUAL_IRPP_KEY_CHANGED)
    {
      if (cycle == 0)
      {
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        SOC_PB_PP_DO_NOTHING_AND_EXIT;
      }

      if (key_seg.lsb + lsb == 0)
      {
        soc_pb_pp_sw_db_fp_loc_key_changed_set(
          unit,
          pfg_ndx,
          key_seg.key == 0 ? SOC_PB_PP_FP_KEY_CHANGE_LOCATION_A_11_0 : SOC_PB_PP_FP_KEY_CHANGE_LOCATION_B_11_0
        );
      }
      else if (key_seg.lsb + lsb == 32)
      {
        soc_pb_pp_sw_db_fp_loc_key_changed_set(
          unit,
          pfg_ndx,
          key_seg.key == 0 ? SOC_PB_PP_FP_KEY_CHANGE_LOCATION_A_43_32 : SOC_PB_PP_FP_KEY_CHANGE_LOCATION_B_43_32
        );
      }
      else
      {
        /*
         *  We don't support other locations
         */
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        SOC_PB_PP_DO_NOTHING_AND_EXIT;
      }
    }
  }

  soc_pb_pp_sw_db_fp_db_key_location_set(
    unit,
    db_id_ndx,
    pfg_ndx,
    &key_src
  );

  instr_idx = key_seg.start_instr;
  if (frame_lsb + lsb > key_seg.lsb)
  {
    empty_instr_size = frame_lsb + lsb - key_seg.lsb;
    while(empty_instr_size)
    {
      ce_instr.is_used              = TRUE;
      ce_instr.is_valid             = TRUE;
      ce_instr.is_used_in_cycle_1   = (cycle == 1) ? TRUE : FALSE;
      ce_instr.is_irpp              = TRUE;
      ce_instr.header_offset_select = 0;
      ce_instr.nibble_field_offset  = 0;
      ce_instr.bit_count            = SOC_SAND_MIN(empty_instr_size, SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX) - 1;
      ce_instr.db_id_ndx            = db_id_ndx;
      soc_pb_sw_db_pgm_ce_instr_set(
        unit,
        pfg_ndx,
        key_seg.key,
        instr_idx,
        &ce_instr
      );
      --instr_idx;
      empty_instr_size = (empty_instr_size > SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX)? empty_instr_size - SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX : 0;
    }
  }
  SOC_PB_PP_FP_KEY_DESC_clear(&key_desc);
  curr_lsb = lsb;
  qual_idx = 0;
  for (idx = 0; idx < nof_quals; ++idx)
  {
    for ( ; (qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (db_info->qual_types[qual_idx] == BCM_FIELD_ENTRY_INVALID); ++qual_idx);
    res = soc_pb_pp_fp_key_ce_instr_set(
            unit,
            db_id_ndx,
            pfg_ndx,
            key_seg.key,
            db_info->qual_types[qual_idx],
            (cycle == 1) ? TRUE : FALSE,
            &instr_idx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    --instr_idx;

    res = soc_pb_pp_fp_key_length_get_unsafe(
            unit,
            db_info->qual_types[qual_idx],
            TRUE,
            &length
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    res = soc_pb_pp_fp_key_length_get_unsafe(
            unit,
            db_info->qual_types[qual_idx],
            FALSE,
            &key_desc.fields[qual_idx].length
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    padding = length - key_desc.fields[qual_idx].length;
    key_desc.fields[qual_idx].lsb = curr_lsb + padding;
    curr_lsb += length;
    ++qual_idx;
  }

  if (!db_has_format)
  {
    soc_pb_pp_sw_db_fp_key_desc_set(
      unit,
      db_id_ndx,
      &key_desc
    );
    soc_pb_pp_sw_db_fp_db_has_key_format_set(
      unit,
      db_id_ndx,
      TRUE
    );
  }

  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_space_alloc_unsafe()", db_id_ndx, 0);
}

/*********************************************************************
*     Frees the PMF key space allocated to a database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_key_space_free_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    pfg_ndx,
    buffer,
    instr;
  SOC_PB_SW_DB_PMF_CE
    ce_instr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_SPACE_FREE_UNSAFE);

  for (pfg_ndx = 0; pfg_ndx < 5; ++pfg_ndx)
  {
    for (buffer = 0; buffer < 2; ++buffer)
    {
      for (instr = 0; instr < 8; ++instr)
      {
        soc_pb_sw_db_pgm_ce_instr_get(
          unit,
          pfg_ndx,
          buffer,
          instr,
          &ce_instr
        );
        if (ce_instr.db_id_ndx == db_id_ndx)
        {
          ce_instr.is_used = FALSE;
          soc_pb_sw_db_pgm_ce_instr_set(
            unit,
            pfg_ndx,
            buffer,
            instr,
            &ce_instr
          );
        }
      }
    }
  }

  soc_pb_pp_sw_db_fp_db_has_key_format_set(
    unit,
    db_id_ndx,
    FALSE
  );

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_space_free_unsafe()", db_id_ndx, 0);
}


uint32
  soc_pb_pp_fp_qual_length_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE         qual_type,
    SOC_SAND_OUT uint32                     *found,
    SOC_SAND_OUT uint32                     *length_padded,
    SOC_SAND_OUT uint32                     *length_logical
  )
{
  uint32
    total_length,
    res;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;
  uint8
      is_for_egress,
    qual_info_found = FALSE;
  SOC_PB_PP_FP_KEY_QUAL_INFO
    qual_info;
  SOC_PB_PP_FP_QUAL_TYPE         
      qual_type_arr[SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX] = {0};


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CE_KEY_LENGTH_MINIMAL_GET);

  SOC_SAND_CHECK_NULL_INPUT(length_padded);
  SOC_SAND_CHECK_NULL_INPUT(length_logical);
  SOC_SAND_CHECK_NULL_INPUT(found);

  /* Find this qualifier in the non-predfined qualifiers */
  if (stage == SOC_PB_PP_FP_DATABASE_STAGE_INGRESS_PMF) {
      qual_info_found = soc_pb_pp_fp_key_qual_info_find(
                          unit,
                          qual_type,
                          &qual_info
                        );
      if (qual_info_found)
      {
          *found = TRUE;
          *length_logical = soc_pb_pp_fp_key_qual_net_length_get(
                                qual_info.msb,
                                qual_info.lsb
                              );
          *length_padded = soc_pb_pp_fp_key_qual_padded_length_get(
                                qual_info.msb,
                                qual_info.lsb
                              );
          SOC_PB_PP_DO_NOTHING_AND_EXIT;
      }
  }


    /*
     *  Check whether it is in a predefined key
     */
    is_for_egress = (stage == SOC_PB_PP_FP_DATABASE_STAGE_EGRESS)? TRUE: FALSE;
    qual_type_arr[0] = qual_type;
    qual_type_arr[1] = BCM_FIELD_ENTRY_INVALID;
    res = soc_pb_pp_fp_key_get(
            unit,
            is_for_egress,
            qual_type_arr,
            &predefined_acl_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_pp_fp_qual_predefined_size_get(
            unit,
            predefined_acl_key,
            qual_type,
            &qual_info_found,
            &total_length
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (qual_info_found)
    {
        *found = TRUE;
        *length_logical = total_length;
        *length_padded = total_length;
        SOC_PB_PP_DO_NOTHING_AND_EXIT;
    }


    *found = FALSE;
    *length_logical = 0;
    *length_padded = 0;
    
    SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_length_get_unsafe()", 0, 0);
}



/*********************************************************************
*     Returns the description of a database's key structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_key_desc_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC      *key_desc
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_DESC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(key_desc);

  SOC_PB_PP_FP_KEY_DESC_clear(key_desc);

  soc_pb_pp_sw_db_fp_key_desc_get(
    unit,
    db_id_ndx,
    key_desc
  );

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_desc_get_unsafe()", db_id_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_fp_key module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_key_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_fp_key;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_fp_key module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_key_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_fp_key;
}

void
  SOC_PB_PP_FP_KEY_LOCATION_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FP_KEY_LOCATION));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_KEY_DESC_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FP_KEY_DESC));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
