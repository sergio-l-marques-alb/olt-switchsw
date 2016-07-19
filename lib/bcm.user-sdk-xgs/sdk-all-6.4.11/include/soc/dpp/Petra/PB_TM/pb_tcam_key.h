/* $Id: soc_pb_tcam_key.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_TCAM_KEY_INCLUDED__
/* { */
#define __SOC_PB_TCAM_KEY_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>

#include <soc/dpp/TMC/tmc_api_tcam_key.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum size in uint32s of a TCAM entry                   */
#define  SOC_PB_TCAM_RULE_NOF_UINT32S_MAX (SOC_TMC_TCAM_RULE_NOF_UINT32S_MAX)

/*     Maximum field value size in uint32s.                      */
#define  SOC_PB_TCAM_KEY_FLD_NOF_UINT32S_MAX (SOC_TMC_TCAM_KEY_FLD_NOF_UINT32S_MAX)



#define SOC_PB_EGR_ACL_DB_TYPE_ETH                             SOC_TMC_EGR_ACL_DB_TYPE_ETH
#define SOC_PB_EGR_ACL_DB_TYPE_IPV4                            SOC_TMC_EGR_ACL_DB_TYPE_IPV4
#define SOC_PB_EGR_ACL_DB_TYPE_MPLS                            SOC_TMC_EGR_ACL_DB_TYPE_MPLS
#define SOC_PB_EGR_ACL_DB_TYPE_TM                              SOC_TMC_EGR_ACL_DB_TYPE_TM
#define SOC_PB_EGR_NOF_ACL_DB_TYPES                            SOC_TMC_EGR_NOF_ACL_DB_TYPES
typedef SOC_TMC_EGR_ACL_DB_TYPE                                SOC_PB_EGR_ACL_DB_TYPE;

#define SOC_PB_PMF_TCAM_KEY_SRC_L2                          SOC_TMC_PMF_TCAM_KEY_SRC_L2
#define SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4                     SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV4
#define SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6                     SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV6
#define SOC_PB_PMF_TCAM_KEY_SRC_B_A                         SOC_TMC_PMF_TCAM_KEY_SRC_B_A
#define SOC_PB_PMF_TCAM_KEY_SRC_A_71_0                      SOC_TMC_PMF_TCAM_KEY_SRC_A_71_0
#define SOC_PB_PMF_TCAM_KEY_SRC_A_103_32                    SOC_TMC_PMF_TCAM_KEY_SRC_A_103_32
#define SOC_PB_PMF_TCAM_KEY_SRC_A_143_0                     SOC_TMC_PMF_TCAM_KEY_SRC_A_143_0
#define SOC_PB_PMF_TCAM_KEY_SRC_A_175_32                    SOC_TMC_PMF_TCAM_KEY_SRC_A_175_32
#define SOC_PB_PMF_TCAM_KEY_SRC_B_71_0                      SOC_TMC_PMF_TCAM_KEY_SRC_B_71_0
#define SOC_PB_PMF_TCAM_KEY_SRC_B_103_32                    SOC_TMC_PMF_TCAM_KEY_SRC_B_103_32
#define SOC_PB_PMF_TCAM_KEY_SRC_B_143_0                     SOC_TMC_PMF_TCAM_KEY_SRC_B_143_0
#define SOC_PB_PMF_TCAM_KEY_SRC_B_175_32                    SOC_TMC_PMF_TCAM_KEY_SRC_B_175_32
#define SOC_PB_NOF_PMF_TCAM_KEY_SRCS                        SOC_TMC_NOF_PMF_TCAM_KEY_SRCS
typedef SOC_TMC_PMF_TCAM_KEY_SRC                               SOC_PB_PMF_TCAM_KEY_SRC;

#define SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF                        SOC_TMC_TCAM_KEY_FORMAT_TYPE_PMF
#define SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL                    SOC_TMC_TCAM_KEY_FORMAT_TYPE_EGR_ACL
#define SOC_PB_TCAM_NOF_KEY_FORMAT_TYPES                       SOC_TMC_TCAM_NOF_KEY_FORMAT_TYPES
typedef SOC_TMC_TCAM_KEY_FORMAT_TYPE                           SOC_PB_TCAM_KEY_FORMAT_TYPE;

#define SOC_PB_PMF_TCAM_FLD_L2_LLVP                         SOC_TMC_PMF_TCAM_FLD_L2_LLVP
#define SOC_PB_PMF_TCAM_FLD_L2_STAG                         SOC_TMC_PMF_TCAM_FLD_L2_STAG
#define SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC                   SOC_TMC_PMF_TCAM_FLD_L2_CTAG_IN_AC
#define SOC_PB_PMF_TCAM_FLD_L2_SA                           SOC_TMC_PMF_TCAM_FLD_L2_SA
#define SOC_PB_PMF_TCAM_FLD_L2_DA                           SOC_TMC_PMF_TCAM_FLD_L2_DA
#define SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE                    SOC_TMC_PMF_TCAM_FLD_L2_ETHERTYPE
#define SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT                   SOC_TMC_PMF_TCAM_FLD_L2_IN_PP_PORT
#define SOC_PB_NOF_PMF_TCAM_FLD_L2S                         SOC_TMC_NOF_PMF_TCAM_FLD_L2S
typedef SOC_TMC_PMF_TCAM_FLD_L2                                SOC_PB_PMF_TCAM_FLD_L2;

#define SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV4                    SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV4
#define SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV6                    SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV6
#define SOC_PB_NOF_PMF_TCAM_FLD_L3_MODES                    SOC_TMC_NOF_PMF_TCAM_FLD_L3_MODES
typedef SOC_TMC_PMF_TCAM_FLD_L3_MODE                           SOC_PB_PMF_TCAM_FLD_L3_MODE;

#define SOC_PB_PMF_FLD_IPV4_L4OPS                           SOC_TMC_PMF_FLD_IPV4_L4OPS
#define SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL                      SOC_TMC_PMF_FLD_IPV4_NEXT_PRTCL
#define SOC_PB_PMF_FLD_IPV4_DF                              SOC_TMC_PMF_FLD_IPV4_DF
#define SOC_PB_PMF_FLD_IPV4_MF                              SOC_TMC_PMF_FLD_IPV4_MF
#define SOC_PB_PMF_FLD_IPV4_FRAG_NON_0                      SOC_TMC_PMF_FLD_IPV4_FRAG_NON_0
#define SOC_PB_PMF_FLD_IPV4_L4OPS_FLAG                      SOC_TMC_PMF_FLD_IPV4_L4OPS_FLAG
#define SOC_PB_PMF_FLD_IPV4_SIP                             SOC_TMC_PMF_FLD_IPV4_SIP
#define SOC_PB_PMF_FLD_IPV4_DIP                             SOC_TMC_PMF_FLD_IPV4_DIP
#define SOC_PB_PMF_FLD_IPV4_L4OPS_OPT                       SOC_TMC_PMF_FLD_IPV4_L4OPS_OPT
#define SOC_PB_PMF_FLD_IPV4_SRC_PORT                        SOC_TMC_PMF_FLD_IPV4_SRC_PORT
#define SOC_PB_PMF_FLD_IPV4_DEST_PORT                       SOC_TMC_PMF_FLD_IPV4_DEST_PORT
#define SOC_PB_PMF_FLD_IPV4_TOS                             SOC_TMC_PMF_FLD_IPV4_TOS
#define SOC_PB_PMF_FLD_IPV4_TCP_CTL                         SOC_TMC_PMF_FLD_IPV4_TCP_CTL
#define SOC_PB_PMF_FLD_IPV4_IN_AC_VRF                       SOC_TMC_PMF_FLD_IPV4_IN_AC_VRF
#define SOC_PB_PMF_FLD_IPV4_IN_PP_PORT                      SOC_TMC_PMF_FLD_IPV4_IN_PP_PORT
#define SOC_PB_PMF_FLD_IPV4_IN_VID                          SOC_TMC_PMF_FLD_IPV4_IN_VID
#define SOC_PB_NOF_PMF_FLD_IPV4S                            SOC_TMC_NOF_PMF_FLD_IPV4S
typedef SOC_TMC_PMF_FLD_IPV4                                   SOC_PB_PMF_FLD_IPV4;

#define SOC_PB_PMF_FLD_IPV6_L4OPS                           SOC_TMC_PMF_FLD_IPV6_L4OPS
#define SOC_PB_PMF_FLD_IPV6_SIP_HIGH                        SOC_TMC_PMF_FLD_IPV6_SIP_HIGH
#define SOC_PB_PMF_FLD_IPV6_SIP_LOW                         SOC_TMC_PMF_FLD_IPV6_SIP_LOW
#define SOC_PB_PMF_FLD_IPV6_DIP_HIGH                        SOC_TMC_PMF_FLD_IPV6_DIP_HIGH
#define SOC_PB_PMF_FLD_IPV6_DIP_LOW                         SOC_TMC_PMF_FLD_IPV6_DIP_LOW
#define SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL                      SOC_TMC_PMF_FLD_IPV6_NEXT_PRTCL
#define SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL                 SOC_TMC_PMF_FLD_IPV6_PP_PORT_TCP_CTL
#define SOC_PB_PMF_FLD_IPV6_IN_AC_VRF                       SOC_TMC_PMF_FLD_IPV6_IN_AC_VRF
#define SOC_PB_NOF_PMF_FLD_IPV6S                            SOC_TMC_NOF_PMF_FLD_IPV6S
typedef SOC_TMC_PMF_FLD_IPV6                                   SOC_PB_PMF_FLD_IPV6;

#define SOC_PB_PMF_TCAM_FLD_A_B_LOC_IRPP                    SOC_TMC_PMF_TCAM_FLD_A_B_LOC_IRPP
#define SOC_PB_PMF_TCAM_FLD_A_B_LOC_HDR                     SOC_TMC_PMF_TCAM_FLD_A_B_LOC_HDR
#define SOC_PB_NOF_PMF_TCAM_FLD_A_B_LOCS                    SOC_TMC_NOF_PMF_TCAM_FLD_A_B_LOCS
typedef SOC_TMC_PMF_TCAM_FLD_A_B_LOC                           SOC_PB_PMF_TCAM_FLD_A_B_LOC;

#define SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE               SOC_TMC_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT              SOC_TMC_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG                   SOC_TMC_EGR_ACL_TCAM_FLD_L2_OUTER_TAG
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG                   SOC_TMC_EGR_ACL_TCAM_FLD_L2_INNER_TAG
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_SA                          SOC_TMC_EGR_ACL_TCAM_FLD_L2_SA
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_DA                          SOC_TMC_EGR_ACL_TCAM_FLD_L2_DA
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA        SOC_TMC_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_L2S                        SOC_TMC_EGR_NOF_ACL_TCAM_FLD_L2S
typedef SOC_TMC_EGR_ACL_TCAM_FLD_L2                            SOC_PB_EGR_ACL_TCAM_FLD_L2;

#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS                       SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_TOS
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE               SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP                       SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_SIP
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP                       SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_DIP
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID                 SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA      SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_IPV4S                      SOC_TMC_EGR_NOF_ACL_TCAM_FLD_IPV4S
typedef SOC_TMC_EGR_ACL_TCAM_FLD_IPV4                          SOC_PB_EGR_ACL_TCAM_FLD_IPV4;

#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH                      SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_FTMH
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA                  SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL                     SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_LABEL
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP                       SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_EXP
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL                       SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_TTL
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA      SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_MPLSS                      SOC_TMC_EGR_NOF_ACL_TCAM_FLD_MPLSS
typedef SOC_TMC_EGR_ACL_TCAM_FLD_MPLS                          SOC_PB_EGR_ACL_TCAM_FLD_MPLS;

#define SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH                        SOC_TMC_EGR_ACL_TCAM_FLD_TM_FTMH
#define SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA                    SOC_TMC_EGR_ACL_TCAM_FLD_TM_HDR_DATA
#define SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA        SOC_TMC_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_TMS                        SOC_TMC_EGR_NOF_ACL_TCAM_FLD_TMS
typedef SOC_TMC_EGR_ACL_TCAM_FLD_TM                            SOC_PB_EGR_ACL_TCAM_FLD_TM;

typedef SOC_TMC_TCAM_RULE_INFO                                 SOC_PB_TCAM_RULE_INFO;
typedef SOC_TMC_TCAM_KEY_FORMAT                                SOC_PB_TCAM_KEY_FORMAT;
typedef SOC_TMC_TCAM_KEY                                       SOC_PB_TCAM_KEY;
typedef SOC_TMC_TCAM_OUTPUT                                    SOC_PB_TCAM_OUTPUT;
typedef SOC_TMC_PMF_TCAM_FLD_L3                                SOC_PB_PMF_TCAM_FLD_L3;
typedef SOC_TMC_PMF_TCAM_FLD_A_B                               SOC_PB_PMF_TCAM_FLD_A_B;
typedef SOC_TMC_TCAM_KEY_FLD_TYPE                          SOC_PB_TCAM_KEY_FLD_TYPE;
typedef SOC_TMC_TCAM_KEY_FLD_VAL                           SOC_PB_TCAM_KEY_FLD_VAL;

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
/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_tcam_key_size_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT                   *key_format,
    SOC_SAND_OUT SOC_PB_TCAM_BANK_ENTRY_SIZE              *key_size,
    SOC_SAND_OUT uint32                             *key_size_in_bits
  );


/*********************************************************************
* NAME:
 *   soc_pb_tcam_key_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the key format and its size (according to the
 *   'key_format' parameter) and mask all the fields.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SRC               key_format -
 *     TCAM Key format. Must be set accordingly to the Key
 *     format expected in the bank the key will be inserted in.
 *   SOC_SAND_OUT SOC_PB_TCAM_KEY                   *tcam_key -
 *     TCAM key with raw bitmap presentation.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_key_clear_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT            *key_format,
    SOC_SAND_OUT SOC_PB_TCAM_KEY                   *tcam_key
  );

uint32
  soc_pb_tcam_key_clear_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT             *key_format
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_key_val_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Write inside the key the field value in the place set
 *   according to the field type and the key format. Unmask
 *   the corresponding bits in the key.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key -
 *     TCAM key with raw bitmap presentation.
 *   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx -
 *     Type of the field to insert in the key. The type is
 *     selected according to the key format.
 *   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val -
 *     Value of the field to insert into the key. The size (in
 *     bits) of the valid value is determined according to the
 *     field type.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_key_val_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val
  );

uint32
  soc_pb_tcam_key_val_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val
  );

uint32
  soc_pb_tcam_key_val_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tcam_key_val_set_unsafe" API.
 *     Refer to "soc_pb_tcam_key_val_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_tcam_key_val_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL           *fld_val
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_key_masked_val_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Write inside the key the field value in the place set
 *   according to the field type and the key format. Unmask
 *   in the key the bits according to the mask value.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key -
 *     TCAM key with raw bitmap presentation.
 *   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx -
 *     Type of the field to insert in the key. The type is
 *     selected according to the key format.
 *   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val -
 *     Value of the field to insert into the key. The size (in
 *     bits) of the valid value is determined according to the
 *     field type.
 *   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *mask_val -
 *     Mask value of the field to insert into the key. For each
 *     meaningful bit the mask bit must be set. Otherwise, this
 *     bit is ignored.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_key_masked_val_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *mask_val
  );

uint32
  soc_pb_tcam_key_masked_val_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *mask_val
  );

uint32
  soc_pb_tcam_key_masked_val_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tcam_key_masked_val_set_unsafe" API.
 *     Refer to "soc_pb_tcam_key_masked_val_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_tcam_key_masked_val_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL           *mask_val
  );


uint32
  SOC_PB_TCAM_RULE_INFO_verify(
    SOC_SAND_IN  SOC_PB_TCAM_RULE_INFO *info
  );

uint32
  SOC_PB_TCAM_KEY_FORMAT_verify(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT *info
  );

uint32
  SOC_PB_TCAM_KEY_verify(
    SOC_SAND_IN  SOC_PB_TCAM_KEY *info
  );

uint32
  SOC_PB_TCAM_OUTPUT_verify(
    SOC_SAND_IN  SOC_PB_TCAM_OUTPUT *info
  );

uint32
  SOC_PB_TCAM_KEY_FLD_TYPE_verify(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE *info
  );

uint32
  SOC_PB_PMF_TCAM_FLD_L3_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L3 *info
  );

uint32
  SOC_PB_PMF_TCAM_FLD_A_B_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B *info
  );

uint32
  SOC_PB_TCAM_KEY_FLD_VAL_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE *fld_type,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL  *info
  );


void
  SOC_PB_TCAM_RULE_INFO_clear(
    SOC_SAND_OUT SOC_PB_TCAM_RULE_INFO *info
  );

void
  SOC_PB_TCAM_KEY_FORMAT_clear(
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FORMAT *info
  );

void
  SOC_PB_TCAM_KEY_clear(
    SOC_SAND_OUT SOC_PB_TCAM_KEY *info
  );

void
  SOC_PB_TCAM_OUTPUT_clear(
    SOC_SAND_OUT SOC_PB_TCAM_OUTPUT *info
  );

void
  SOC_PB_TCAM_KEY_FLD_TYPE_clear(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_TYPE *info
  );

void
  SOC_PB_PMF_TCAM_FLD_L3_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_FLD_L3 *info
  );

void
  SOC_PB_PMF_TCAM_FLD_A_B_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_FLD_A_B *info
  );

void
  SOC_PB_TCAM_KEY_FLD_VAL_clear(
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL *info
  );


#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_EGR_ACL_DB_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE enum_val
  );

const char*
  SOC_PB_PMF_TCAM_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SRC enum_val
  );

const char*
  SOC_PB_TCAM_KEY_FORMAT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT_TYPE enum_val
  );

const char*
  SOC_PB_PMF_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L2 enum_val
  );

const char*
  SOC_PB_PMF_TCAM_FLD_L3_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L3_MODE enum_val
  );

const char*
  SOC_PB_PMF_FLD_IPV4_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FLD_IPV4 enum_val
  );

const char*
  SOC_PB_PMF_FLD_IPV6_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FLD_IPV6 enum_val
  );

const char*
  SOC_PB_PMF_TCAM_FLD_A_B_LOC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B_LOC enum_val
  );

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_L2 enum_val
  );

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_IPV4_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_IPV4 enum_val
  );

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_MPLS_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_MPLS enum_val
  );

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_TM_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_TM enum_val
  );

void
  SOC_PB_TCAM_RULE_INFO_print(
    SOC_SAND_IN  SOC_PB_TCAM_RULE_INFO *info
  );

void
  SOC_PB_TCAM_KEY_FORMAT_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT *info
  );

void
  SOC_PB_TCAM_KEY_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY *info
  );

void
  SOC_PB_TCAM_OUTPUT_print(
    SOC_SAND_IN  SOC_PB_TCAM_OUTPUT *info
  );

void
  SOC_PB_TCAM_KEY_FLD_TYPE_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE *info
  );

void
  SOC_PB_PMF_TCAM_FLD_L3_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L3 *info
  );

void
  SOC_PB_PMF_TCAM_FLD_A_B_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B *info
  );

void
  SOC_PB_TCAM_KEY_FLD_VAL_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL *info
  );


#endif

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_TCAM_KEY_INCLUDED__*/
#endif

