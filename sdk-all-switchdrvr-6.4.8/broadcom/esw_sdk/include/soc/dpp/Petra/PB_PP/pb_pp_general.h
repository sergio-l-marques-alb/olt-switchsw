/* $Id$
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

#ifndef __SOC_PB_PP_GENERAL_INCLUDED__
/* { */
#define __SOC_PB_PP_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

#include <soc/dpp/soc_dpp_petrab_config_defs.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_TM_PORT_MAX                                      (SOC_PETRA_NOF_FAP_PORTS - 1)
#define SOC_PB_PP_PORT_MAX                                         (63)
#define SOC_PB_PP_NOF_PORTS                                        (SOC_PB_PP_PORT_MAX + 1)
#define SOC_PB_PP_FID_MAX                                          (SOC_DPP_NOF_FIDS_PETRAB - 1)
#define SOC_PB_PP_VSI_ID_MIN                                       (1)
#define SOC_PB_PP_VSI_ID_MAX                                       (16*1024-1)
#define SOC_PB_PP_RIF_VSI_ID_MAX                                   (4*1024-1)
#define SOC_PB_PP_SYS_VSI_ID_MAX                                   (64*1024-1)
#define SOC_PB_PP_FEC_ID_MAX                                       (SOC_DPP_NOF_FECS_PETRAB - 1)
#define SOC_PB_PP_FEC_ID_MIN                                       (0)
#define SOC_PB_PP_VRF_ID_MIN                                       (1)
#define SOC_PB_PP_VRF_ID_MAX                                       (SOC_DPP_NOF_VRFS_PETRAB - 1)
#define SOC_PB_PP_AC_ID_MAX                                        (SOC_DPP_NOF_LOCAL_LIFS_PETRAB - 1)
#define SOC_PB_PP_RIF_ID_MAX                                       (SOC_DPP_NOF_RIFS_PETRAB - 1)
#define SOC_PB_PP_LIF_ID_MAX                                       (SOC_DPP_NOF_LOCAL_LIFS_PETRAB - 1)
#define SOC_PB_PP_ISID_DOMAIN_MAX                                  (63)
#define SOC_PB_PP_ISID_ID_MAX                                      (24*1024-1)

#define SOC_PB_PP_MP_LEVEL_MAX                                     (7)
#define SOC_PB_PP_SYS_PORT_MAX                                     (4095)

#define SOC_PB_PP_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_MAX         (7)
#define SOC_PB_PP_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_MAX         (3)

#define SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_EM                     (16)
#define SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_16                     (16)
#define SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_17                     (17)


#define SOC_PB_PP_RIF_ISEM_RES_SERVICE_TYPE                        (6)

#define SOC_PB_PP_LIF_OPCODE_NO_COS                                (3)


#define SOC_PB_PP_RIF_NULL_VAL                               (0)





typedef enum
{
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_NONE = 0,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_TCP = 8,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_UDP = 9,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_IGMP = 10,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_ICMP = 11,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_ICMPV6 = 12,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_IPV4 = 13,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_IPV6 = 14,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_MPLS = 15,
  SOC_PB_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED = 0xff
} SOC_PB_PP_L3_NEXT_PRTCL_NDX;

/* } */
/*************
 * MACROS    *
 *************/
/* { */


#define SOC_PB_PP_COPY(var_dest_ptr, var_src_ptr, type, count)                \
  do                                                                      \
  {                                                                       \
    if ((var_src_ptr == NULL) || (var_dest_ptr == NULL))                  \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    res = soc_sand_os_memcpy(                                                 \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, 1005, exit);                              \
  }while(0);

#define SOC_PB_PP_CLEAR                                                       \
          SOC_PETRA_CLEAR

#define SOC_PB_PP_PCP_DEI_TO_FLD_VAL(pcp, dei)                                \
          ((pcp<<1) + SOC_SAND_BOOL2NUM(dei))

#define SOC_PB_PP_PCP_DEI_FROM_FLD_VAL(fld_val, pcp, dei)                     \
          pcp = (uint8)SOC_SAND_GET_BITS_RANGE(fld_val, 3, 1);              \
          dei = (uint8)SOC_SAND_GET_BIT(fld_val, 0);

/* Generate offset (tables, registers) from value and nof_bits */
#define SOC_PB_PP_FLDS_TO_BUFF_2(val1, nof_bits1, val2, nof_bits2)            \
  ((val1 << nof_bits2) + val2)

#define SOC_PB_PP_FLDS_TO_BUFF_3(val1, nof_bits1, val2, nof_bits2, val3, nof_bits3)  \
  ((SOC_PB_PP_FLDS_TO_BUFF_2(val1, nof_bits1, val2, nof_bits2) << nof_bits3) + val3)

#define SOC_PB_PP_FLDS_TO_BUFF_4(val1, nof_bits1, val2, nof_bits2, val3, nof_bits3, val4, nof_bits4)  \
  ((SOC_PB_PP_FLDS_TO_BUFF_3(val1, nof_bits1, val2, nof_bits2, val3, nof_bits3) << nof_bits4) + val4)

#define SOC_PB_PP_FLDS_FROM_BUFF_2(buff, val1, nof_bits1, val2, nof_bits2)     \
  do {                                                                   \
    val1 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits1)+(nof_bits2)-1), nof_bits2);  \
    val2 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits2)-1), 0);              \
  } while (0)
#define SOC_PB_PP_SYS_PORT_ENCODE(is_lag, port_val)  \
  ((is_lag))?SOC_SAND_BIT(12)|(port_val):(port_val);

#define SOC_PB_PP_FLDS_FROM_BUFF_3(buff, val1, nof_bits1, val2, nof_bits2, val3, nof_bits3)  \
  do {                                                                   \
    SOC_PB_PP_FLDS_FROM_BUFF_2(buff, val2, nof_bits2, val3, nof_bits3);      \
    val1 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits1)+(nof_bits2)+(nof_bits3)-1), nof_bits2 + nof_bits3);  \
  } while (0)

#define SOC_PB_PP_FLDS_FROM_BUFF_4(buff, val1, nof_bits1, val2, nof_bits2, val3, nof_bits3, val4, nof_bits4)  \
  do {                                                                   \
    SOC_PB_PP_FLDS_FROM_BUFF_3(buff, val2, nof_bits2, val3, nof_bits3, val4, nof_bits4);  \
    val1 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits1)+(nof_bits2)+(nof_bits3)+(nof_bits4)-1), nof_bits2+nof_bits3+nof_bits4);  \
  } while (0)

#define SOC_PB_PP_MASK_IS_ON(bitmap, mask) \
  (SOC_SAND_NUM2BOOL((bitmap) & (mask)))

#define SOC_PB_PP_SAND_SYS_PORT_ENCODE(sys_port)\
  SOC_PB_PP_SYS_PORT_ENCODE(((sys_port)->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG),sys_port->sys_id);
  

#define SOC_PB_PP_IP_PROTOCOL_UD_NDX_TO_FIELD_VAL(ndx) (ndx+1)
#define SOC_PB_PP_IP_FIELD_VAL_TO_PROTOCOL_UD_NDX(ndx) (ndx-1)


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_GENERAL_GET_PROCS_PTR = SOC_PB_PP_PROC_DESC_BASE_GENERAL_FIRST,
  SOC_PB_PP_GENERAL_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_FWD_DECISION_DEST_PREFIX_FIND,
  SOC_PB_PP_FWD_DECISION_DEST_TYPE_FIND,
  SOC_PB_PP_FWD_DECISION_IN_BUFFER_BUILD,
  SOC_PB_PP_FWD_DECISION_IN_BUFFER_PARSE,
  SOC_PB_PP_PKT_HDR_TYPE_TO_INTERANL_VAL_MAP,
  SOC_PB_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP,
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_TO_INTERANL_VAL_MAP,
  SOC_PB_PP_L2_NEXT_PRTCL_INTERANL_VAL_TO_TYPE_MAP,
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_ALLOCATE,
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_DEALLOCATE,
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FIND,
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FROM_INTERNAL_FIND,
  SOC_PB_PP_L3_NEXT_PROTOCOL_ADD,
  SOC_PB_PP_L3_NEXT_PROTOCOL_REMOVE,
  SOC_PB_PP_L3_NEXT_PROTOCOL_FIND,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_GENERAL_PROCEDURE_DESC_LAST
} SOC_PB_PP_GENERAL_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_TM_PORT_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_GENERAL_FIRST,
  SOC_PB_PP_PORT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MP_LEVEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_VAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_ISID_ID_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_EEI_TYPE_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_CODE_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_FWD_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_EEI_VAL_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR,
   SOC_PB_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_DROP_ILLEGAL_ERR,

   SOC_PB_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,

   SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR,
   SOC_PB_PP_GEN_NUM_CLEAR_ERR,
   SOC_PB_PP_MAX_BELOW_MIN_ERR,

   SOC_PB_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,

   SOC_PB_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR,

   SOC_PB_PP_PKT_HDR_TYPE_NOT_SUPPORTED_ERR,
   SOC_PB_PP_L2_NEXT_PRTCL_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_GENERAL_ERR_LAST
} SOC_PB_PP_GENERAL_ERR;

typedef enum
{
  /*
   *  EM encoding type
   */
  SOC_PB_PP_DEST_ENCODE_TYPE_EM = 0,

  /*
   *  16 bit encoding
   */
   SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS = 1,

  /*
   *  17 bit encoding
   */
   SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS = 2,

 /*
  *  Number of encoding types
  */
  SOC_PB_PP_NOF_DEST_ENCODE_TYPES = 3
}SOC_PB_PP_DEST_ENCODE_TYPE;

typedef enum
{
  /*
   *  Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH = 0,
  /*
   *  MAC-in-MAC
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_ETH = 1,
  /*
   *  IPv4 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_ETH = 2,
  /*
   *  IPv6 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_ETH = 3,
  /*
   *  MPLS over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS1_ETH = 5,
  /*
   *  MPLS x 2 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS2_ETH = 6,
  /*
   *  MPLS x 3 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS3_ETH = 7,
  /*
   *  Ethenet over TRILL over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_TRILL_ETH = 12,
  /*
   *  Ethernet over MPLS over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH = 13,
  /*
   *  Ethernet over MPLSx2 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH = 14,
  /*
   *  Ethernet over MPLSx3 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH = 15,
  /*
   *  IPv4 over IPv4 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH = 18,
  /*
   *  IPv4 over MPLS over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH = 21,
  /*
   *  IPv4 over MPLSx2 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH = 22,
  /*
   *  IPv4 over MPLSx3 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH = 23,
  /*
   *  IPv6 over IPv4 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH = 26,
  /*
   *  IPv6 over MPLS over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH = 29,
  /*
   *  IPv6 over MPLSx2 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH = 30,
  /*
   *  IPv6 over MPLSx3 over Ethernet
   */
  SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH = 31,
  /*
   *  Number of types in SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE
   */
  SOC_PB_PP_INTERNAL_NOF_PKT_HDR_STK_TYPES = 19
}SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE;

typedef struct
{
  uint32 bitcount ;
  uint32 niblle_field_offset;
  uint32 header_offset_select;
  uint32 source_select;
  uint32 valid;
}SOC_PB_PP_CE_INSTRUCTION;

/* $Id$
 * Application type
 *
 * Forwarding decision encoding format varies for different applications.
 * Application type determines destination encoding type (EM, 16 bit, etc.)
 *  and asd format
 */
typedef enum
{
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EMPTY = 0,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRAP,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_IP,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_PMF,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_16,
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPES
} SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE;

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
  soc_pb_pp_ce_instruction_build(
    SOC_SAND_IN  SOC_PB_PP_CE_INSTRUCTION         *cek
  );

/*********************************************************************
*     Convert a forwarding decision to a buffer according to the
*     encoding type and the SA drop bit (for the ASD encoding)
*********************************************************************/

uint32
  soc_pb_pp_fwd_decision_in_buffer_build(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE app_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_IN  uint8                 is_sa_drop,
    SOC_SAND_OUT uint32                   *dest_buffer,
    SOC_SAND_OUT uint32                   *asd_buffer
  );

/*********************************************************************
*     Parse an ASD and a destination buffer to get the forward
*     decision and the SA drop (from the ASD msb)
*********************************************************************/
uint32
  soc_pb_pp_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE app_type,
    SOC_SAND_IN  uint32                  dest_buffer,
    SOC_SAND_IN  uint32                  asd_buffer,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT uint8                 *is_sa_drop
  );

/*********************************************************************
*     map from header stack type to HW internal value
*********************************************************************/
uint32
  soc_pb_pp_pkt_hdr_type_to_interanl_val_map(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_STK_TYPE    pkt_hdr_type,
    SOC_SAND_OUT uint32                  *internal_val
  );

/*********************************************************************
*     map from HW internal value to header stack type
*********************************************************************/
uint32
  soc_pb_pp_pkt_hdr_interanl_val_to_type_map(
    SOC_SAND_IN  uint32                  internal_val,
    SOC_SAND_OUT SOC_PB_PP_PKT_HDR_STK_TYPE    *pkt_hdr_type
  );

/*********************************************************************
*    map from HW internal value to L2 next protocol type
*********************************************************************/
uint32
  soc_pb_pp_l2_next_prtcl_type_to_interanl_val_map(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_NEXT_PRTCL_TYPE    l2_next_prtcl_type,
    SOC_SAND_OUT uint32                    *internal_val,
    SOC_SAND_OUT uint8                   *found
  );

/*********************************************************************
*    map from HW internal value to L2 next protocol type
*********************************************************************/
uint32
  soc_pb_pp_l2_next_prtcl_interanl_val_to_type_map(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  internal_val,
    SOC_SAND_OUT SOC_PB_PP_L2_NEXT_PRTCL_TYPE    *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                   *found
  );

/*********************************************************************
*    Allocate l2 protocol type:
*    If l2_next_prtcl_type is one of SOC_PB_PP_L2_NEXT_PRTCL_TYPE, returns
*    its internal value. Else, tries to allocate one of 7 custom entries.
*********************************************************************/
uint32
  soc_pb_pp_l2_next_prtcl_type_allocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE      *success
  );

/*********************************************************************
*    Allocate l2 protocol type:
*    If l2_next_prtcl_type is one of SOC_PB_PP_L2_NEXT_PRTCL_TYPE, returns
*    its internal value. Else, tries to allocate one of 7 custom entries.
*********************************************************************/
uint32
  soc_pb_pp_l2_next_prtcl_type_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT uint8                 *found
  );

uint32
  soc_pb_pp_l2_next_prtcl_type_from_internal_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  internal_ndx,
    SOC_SAND_OUT uint32                  *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                 *found
  );

/*********************************************************************
*    Deallocate l2 protocol type:
*    If l2_next_prtcl_type is one of SOC_PB_PP_L2_NEXT_PRTCL_TYPE, does nothing.
*    Else, tries to deallocate from the 7 custom entries.
*********************************************************************/
uint32
  soc_pb_pp_l2_next_prtcl_type_deallocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type
  );


/*
 * L3 Next-Protocol Management
 */
uint32
  soc_pb_pp_l3_prtcl_to_ndx(
    SOC_SAND_IN  SOC_PB_PP_L3_NEXT_PRTCL_TYPE prtcl_type,
    SOC_SAND_OUT uint8 *ndx
  );

uint32
  soc_pb_pp_llp_trap_ndx_to_l3_prtcl(
    SOC_SAND_IN  uint8 ndx,
    SOC_SAND_OUT SOC_PB_PP_L3_NEXT_PRTCL_TYPE *prtcl_type,
    SOC_SAND_OUT uint8 *is_found
  );

uint32
  soc_pb_pp_l3_next_protocol_add(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8              *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );

uint32
  soc_pb_pp_l3_next_protocol_remove(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx
  );

uint32
  soc_pb_pp_l3_next_protocol_find(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8              *internal_ndx,
    SOC_SAND_OUT uint8             *is_found
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_general_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_general_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_general_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_general_get_errs_ptr(void);

uint32
  SOC_PB_PP_TRAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_INFO *info
  );

uint32
  SOC_PB_PP_OUTLIF_verify(
    SOC_SAND_IN  SOC_PB_PP_OUTLIF *info
  );

uint32
  SOC_PB_PP_MPLS_COMMAND_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_COMMAND *info
  );

uint32
  SOC_PB_PP_EEI_verify(
    SOC_SAND_IN  SOC_PB_PP_EEI *info
  );

uint32
  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE       type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *info
  );

uint32
  SOC_PB_PP_ACTION_PROFILE_verify(
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE *info
  );

uint32
  SOC_PB_PP_TPID_PROFILE_verify(
    SOC_SAND_IN  SOC_PB_PP_TPID_PROFILE *info
  );

uint32
  SOC_PB_PP_PEP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY *info
  );

uint32
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_verify(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_ITER *info
  );

uint32
  SOC_PB_PP_IP_ROUTING_TABLE_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_RANGE *info
  );

uint32
  SOC_PB_PP_FRWRD_DECISION_INFO_with_encode_type_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *info
  );

uint32
  soc_pb_pp_SAND_PP_SYS_PORT_ID_verify(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID *info
  );

const char*
  SOC_PB_PP_DEST_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_DEST_ENCODE_TYPE enum_val
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_GENERAL_INCLUDED__*/
#endif
