/* $Id: arad_pp_general.h,v 1.40 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifndef __ARAD_PP_GENERAL_INCLUDED__
/* { */
#define __ARAD_PP_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_TM_PORT_MAX                                      (ARAD_NOF_FAP_PORTS - 1)
#define ARAD_PP_PORT_MAX                                         (255)
#define ARAD_PP_NOF_PORTS                                        (ARAD_PP_PORT_MAX + 1)
#define ARAD_PP_VSI_ID_MIN                                       (1)
#define ARAD_PP_VSI_ID_MAX                                       (64*1024-1)
#define ARAD_PP_SYS_VSI_ID_MAX                                   (64*1024-1)
#define ARAD_PP_FEC_ID_MAX                                       (SOC_DPP_NOF_FECS_ARAD - 1)
#define ARAD_PP_VRF_ID_MIN                                       (1)
#define ARAD_PP_ISID_DOMAIN_MAX                                  (31)
#define ARAD_PP_ISID_ID_MAX                                      (24*1024-1) 
#define ARAD_PP_VRRP_NOF_MAC_ADDRESSES                           (4*1024)

#define ARAD_PP_MAX_MAC_LIMIT_VAL_PER_TUNNEL                     (0x7FFF)
#define ARAD_PP_MAC_NOF_LIMIT_TUNNEL                             (2048)
#define ARAD_PP_MAC_LIMIT_NOF_PON_PORT                           (8)
#define ARAD_PP_LIMIT_NUM_MAX                                    (ARAD_PP_MAC_NOF_LIMIT_TUNNEL*ARAD_PP_MAC_LIMIT_NOF_PON_PORT)

#define ARAD_PP_MP_LEVEL_MAX                                     (7)
#define ARAD_PP_SYS_PORT_MAX                                     (4095)


#define ARAD_PP_ASD_NOF_BITS_TRILL_NICK                          (16)
#define ARAD_PP_EEI_ISID_NOF_BITS                                (24)
#define ARAD_PP_EEI_NOF_BITS                                     (24)

#define ARAD_PP_ASD_BIT_POSITION_IDENTIFIER                      (20)
#define ARAD_PP_EEI_IDENTIFIER_NOF_BITDS                         (4)
#define ARAD_PP_ASD_BIT_POSITION_IDENTIFIER_2_MSBS               (ARAD_PP_ASD_BIT_POSITION_IDENTIFIER + ARAD_PP_EEI_IDENTIFIER_NOF_BITDS -2)

#define ARAD_PP_EEI_IDENTIFIER_POP_VAL                       (8)
#define ARAD_PP_EEI_IDENTIFIER_SWAP_VAL                      (9)
#define ARAD_PP_EEI_IDENTIFIER_TRILL_VAL                     (10)
#define ARAD_PP_EEI_IDENTIFIER_ENCAP_VAL                     (15)
#define ARAD_PP_EEI_IDENTIFIER_ENCAP_VAL_2_MSBS               (3)


#define ARAD_PP_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_MAX         (7)
#define ARAD_PP_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_MAX         (3)

#define ARAD_PP_DEST_ENCODE_TOTAL_IN_BITS_19                     (19)
#define ARAD_PP_DEST_ENCODE_TOTAL_IN_BITS_12                     (12)  /* destination encoded in 12 bits */

#define ARAD_PP_PCP_PROFILE_NDX_MAX                              (15)

#define ARAD_PP_RIF_ISEM_RES_SERVICE_TYPE                        (3)

#define ARAD_PP_LIF_OPCODE_NO_COS                                (3)


#define ARAD_PP_RIF_NULL_VAL                                     (0)

#define ARAD_PP_MTU_MAX                                          (0x3FFF)

#define ARAD_PP_VXLAN_DST_PORT                                   (0x5555)



#define ARAD_PP_NOF_USER_DEFINED_ETHER_TYPES   (7)


typedef enum
{
  ARAD_PP_L3_NEXT_PRTCL_NDX_NONE = 0,
  ARAD_PP_L3_NEXT_PRTCL_NDX_TCP = 8,
  ARAD_PP_L3_NEXT_PRTCL_NDX_UDP = 9,
  ARAD_PP_L3_NEXT_PRTCL_NDX_IGMP = 10,
  ARAD_PP_L3_NEXT_PRTCL_NDX_ICMP = 11,
  ARAD_PP_L3_NEXT_PRTCL_NDX_ICMPV6 = 12,
  ARAD_PP_L3_NEXT_PRTCL_NDX_IPV4 = 13,
  ARAD_PP_L3_NEXT_PRTCL_NDX_IPV6 = 14,
  ARAD_PP_L3_NEXT_PRTCL_NDX_MPLS = 15,
  ARAD_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED = 0xff
} ARAD_PP_L3_NEXT_PRTCL_NDX;





/* forward code */
#define ARAD_PP_FWD_CODE_ETHERNET                   (0x0)
#define ARAD_PP_FWD_CODE_IPV4_UC                    (0x1)
#define ARAD_PP_FWD_CODE_IPV4_MC                    (0x2)
#define ARAD_PP_FWD_CODE_IPV6_UC                    (0x3)
#define ARAD_PP_FWD_CODE_IPV6_MC                    (0x4)
#define ARAD_PP_FWD_CODE_MPLS                       (0x5)
#define ARAD_PP_FWD_CODE_TRILL                      (0x6)
#define ARAD_PP_FWD_CODE_CPU_TRAP                   (0x7)
#define ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION (0x8)
#define ARAD_PP_FWD_CODE_CUSTOM1                    (0x9)
#define ARAD_PP_FWD_CODE_CUSTOM2                    (0xA)
#define ARAD_PP_FWD_CODE_TM                         (0xE)
/* nof fwd code */
#define ARAD_PP_FWD_CODE_NOF_FWD_CODE               (16) 


/* system headers mode */
#define ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO         (0x0)
#define ARAD_PP_SYSTEM_HEADERS_MODE_PETRAB          (0x1)
#define ARAD_PP_SYSTEM_HEADERS_MODE_ARAD            (0x2)
#define ARAD_PP_SYSTEM_HEADERS_MODE_OTHER           (0x3)

/* FHEI fwd size */
#define ARAD_IHB_PPH_FHEI_FWD_SIZE_0B               (0)
#define ARAD_IHB_PPH_FHEI_FWD_SIZE_3B               (1)
#define ARAD_IHB_PPH_FHEI_FWD_SIZE_5B               (2)
#define ARAD_IHB_PPH_FHEI_FWD_SIZE_8B               (3)
#define ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS         (2)

/* } */
/*************
 * MACROS    *
 *************/
/* { */


#define ARAD_PP_COPY(var_dest_ptr, var_src_ptr, type, count)                \
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

#define ARAD_PP_CLEAR                                                       \
          ARAD_CLEAR

#define ARAD_PP_PCP_DEI_TO_FLD_VAL(pcp, dei)                                \
          ((pcp<<1) + SOC_SAND_BOOL2NUM(dei))

#define ARAD_PP_PCP_DEI_FROM_FLD_VAL(fld_val, pcp, dei)                     \
          pcp = (uint8)SOC_SAND_GET_BITS_RANGE(fld_val, 3, 1);              \
          dei = (uint8)SOC_SAND_GET_BIT(fld_val, 0);

/* Generate offset (tables, registers) from value and nof_bits */
#define ARAD_PP_FLDS_TO_BUFF_2(val1, nof_bits1, val2, nof_bits2)            \
  ((val1 << nof_bits2) + val2)

#define ARAD_PP_FLDS_TO_BUFF_3(val1, nof_bits1, val2, nof_bits2, val3, nof_bits3)  \
  ((ARAD_PP_FLDS_TO_BUFF_2(val1, nof_bits1, val2, nof_bits2) << nof_bits3) + val3)

#define ARAD_PP_FLDS_TO_BUFF_4(val1, nof_bits1, val2, nof_bits2, val3, nof_bits3, val4, nof_bits4)  \
  ((ARAD_PP_FLDS_TO_BUFF_3(val1, nof_bits1, val2, nof_bits2, val3, nof_bits3) << nof_bits4) + val4)

#define ARAD_PP_FLDS_FROM_BUFF_2(buff, val1, nof_bits1, val2, nof_bits2)     \
  do {                                                                   \
    val1 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits1)+(nof_bits2)-1), nof_bits2);  \
    val2 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits2)-1), 0);              \
  } while (0)
#define ARAD_PP_SYS_PORT_ENCODE(is_lag, port_val)  \
  ((is_lag))?SOC_SAND_BIT(15)|(port_val):(port_val);

#define ARAD_PP_FLDS_FROM_BUFF_3(buff, val1, nof_bits1, val2, nof_bits2, val3, nof_bits3)  \
  do {                                                                   \
    ARAD_PP_FLDS_FROM_BUFF_2(buff, val2, nof_bits2, val3, nof_bits3);      \
    val1 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits1)+(nof_bits2)+(nof_bits3)-1), nof_bits2 + nof_bits3);  \
  } while (0)

#define ARAD_PP_FLDS_FROM_BUFF_4(buff, val1, nof_bits1, val2, nof_bits2, val3, nof_bits3, val4, nof_bits4)  \
  do {                                                                   \
    ARAD_PP_FLDS_FROM_BUFF_3(buff, val2, nof_bits2, val3, nof_bits3, val4, nof_bits4);  \
    val1 = SOC_SAND_GET_BITS_RANGE(buff, ((nof_bits1)+(nof_bits2)+(nof_bits3)+(nof_bits4)-1), nof_bits2+nof_bits3+nof_bits4);  \
  } while (0)

#define ARAD_PP_MASK_IS_ON(bitmap, mask) \
  (SOC_SAND_NUM2BOOL((bitmap) & (mask)))

#define ARAD_PP_SAND_SYS_PORT_ENCODE(sys_port)\
  ARAD_PP_SYS_PORT_ENCODE(((sys_port)->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG),sys_port->sys_id);
  
#define ARAD_PP_SYS_PORT_DECODE(val, sys_port)\
  (sys_port)->sys_port_type = (SOC_SAND_GET_BIT(val,12))==1)? SOC_SAND_PP_SYS_PORT_TYPE_LAG:SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT; \
  (sys_port)->sys_id = SOC_SAND_GET_BITS_RANGE(val,11,0);

#define ARAD_PP_IP_PROTOCOL_UD_NDX_TO_FIELD_VAL(ndx) (ndx+1)
#define ARAD_PP_IP_FIELD_VAL_TO_PROTOCOL_UD_NDX(ndx) (ndx-1)


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
  ARAD_PP_GENERAL_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_GENERAL_FIRST,
  ARAD_PP_GENERAL_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  ARAD_PP_FWD_DECISION_DEST_PREFIX_FIND,
  ARAD_PP_FWD_DECISION_DEST_TYPE_FIND,
  ARAD_PP_FWD_DECISION_EEI_TYPE_FIND,
  ARAD_PP_FWD_DECISION_IN_BUFFER_BUILD,
  ARAD_PP_FWD_DECISION_IN_BUFFER_PARSE,
  ARAD_PP_FWD_DECISION_BUILD,
  ARAD_PP_FWD_DECISION_PARSE,
  ARAD_PP_PKT_HDR_TYPE_TO_INTERANL_VAL_MAP,
  ARAD_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP,
  ARAD_PP_L2_NEXT_PRTCL_TYPE_TO_INTERANL_VAL_MAP,
  ARAD_PP_L2_NEXT_PRTCL_INTERANL_VAL_TO_TYPE_MAP,
  ARAD_PP_L2_NEXT_PRTCL_TYPE_ALLOCATE,
  ARAD_PP_L2_NEXT_PRTCL_TYPE_DEALLOCATE,
  ARAD_PP_L2_NEXT_PRTCL_TYPE_FIND,
  ARAD_PP_L2_NEXT_PRTCL_TYPE_FROM_INTERNAL_FIND,
  ARAD_PP_L3_NEXT_PROTOCOL_ADD,
  ARAD_PP_L3_NEXT_PROTOCOL_REMOVE,
  ARAD_PP_L3_NEXT_PROTOCOL_FIND,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_GENERAL_PROCEDURE_DESC_LAST
} ARAD_PP_GENERAL_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_TM_PORT_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_GENERAL_FIRST,
  ARAD_PP_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_FID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FEC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VRF_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_AC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_MP_LEVEL_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
  ARAD_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

   ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_EEI_TYPE_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_CODE_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_FWD_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_EEI_VAL_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MPLS_TPID_PROFILE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MPLS_POP_MODEL_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_MPLS_POP_NEXT_HEADER_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR,
   ARAD_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_DROP_ILLEGAL_ERR,

   ARAD_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,

   ARAD_PP_EG_ENCAP_NDX_ILLEGAL_ERR,
   ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_OUT_OF_RANGE_ERR,
   
   ARAD_PP_FEATURE_NOT_SUPPORTED_ERR,
   ARAD_PP_GEN_NUM_CLEAR_ERR,
   ARAD_PP_MAX_BELOW_MIN_ERR,

   ARAD_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,

   ARAD_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR,

   ARAD_PP_PKT_HDR_TYPE_NOT_SUPPORTED_ERR,
   ARAD_PP_L2_NEXT_PRTCL_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR,
   ARAD_PP_FRWRD_DECISION_EEI_AND_OUTLIF_VALID_ERR,

  ARAD_PP_IP_ITER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_DEST_ISID_VAL_INVALID_ERR,
  ARAD_PP_FRWRD_DEST_RAW_VAL_INVALID_ERR,

    ARAD_PP_EG_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR,

   ARAD_PP_VLAN_TAGS_OUT_OF_RANGE_ERR,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_GENERAL_ERR_LAST
} ARAD_PP_GENERAL_ERR;

typedef enum
{
  /*
   *  19 bit encoding
   */
   ARAD_PP_DEST_ENCODE_TYPE_19_BITS = 0,
  /*
   *  Number of encoding types
   */
   ARAD_PP_NOF_DEST_ENCODE_TYPES = 1
}ARAD_PP_DEST_ENCODE_TYPE;

/* Copy engine instructions */
typedef struct
{
  uint32 bitcount;
  uint32 niblle_field_offset;
  uint32 header_offset_select;
  uint32 source_select;
}ARAD_PP_CE_INSTRUCTION;

/* Copy engine has two types of instrutions: 16 and 32 bits */
typedef ARAD_PP_CE_INSTRUCTION ARAD_PP_CE_16B_INSTRUCTION;
typedef ARAD_PP_CE_INSTRUCTION ARAD_PP_CE_32B_INSTRUCTION;

/*
 * Application type
 *
 * Forwarding decision encoding format varies for different applications.
 * Application type determines destination encoding type (EM, 16 bit, etc.)
 *  and asd format
 */
typedef enum
{
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT = 0,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TRAP,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_IP,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_PMF,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_16,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPES
} ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE;

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
/*********************************************************************
*     Convert a forwarding decision to a buffer according to the
*     encoding type and the SA drop bit (for the ASD encoding)
*********************************************************************/

uint32
  arad_pp_fwd_decision_in_buffer_build(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE app_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT uint32                   *dest_buffer,
    SOC_SAND_OUT uint32                   *asd_buffer
  );

/*********************************************************************
*     Parse an ASD and a destination buffer to get the forward
*     decision and the SA drop (from the ASD msb)
*********************************************************************/

#define ARAD_PP_FWD_DECISION_PARSE_LEGACY   (0x1)
#define ARAD_PP_FWD_DECISION_PARSE_DEST     (0x2)
#define ARAD_PP_FWD_DECISION_PARSE_EEI      (0x4)
#define ARAD_PP_FWD_DECISION_PARSE_OULTIF   (0x8)
#define ARAD_PP_FWD_DECISION_PARSE_EEI_MIM   (0x10) /* If set, indicates the EEI type is MIM */
#define ARAD_PP_FWD_DECISION_PARSE_FORMAT_3 (0x20)
#define ARAD_PP_FWD_DECISION_PARSE_OULTIF_INVALID   (0x40)
#define ARAD_PP_FWD_DECISION_PARSE_FORMAT_3B   (0x80) /* format3B arad+: 1b'1' Native-VSI(12),  HI(4), outLIF(14), FEC(12) 
                                                         format3B jer:   1b'1' Native-VSI(13),  HI(4), outLIF(15), FEC(12) */
/* must point to last define*/
#define ARAD_PP_FWD_DECISION_PARSE_LAST (ARAD_PP_FWD_DECISION_PARSE_FORMAT_3B)

uint32
  arad_pp_fwd_decision_in_buffer_parse(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
      SOC_SAND_IN  uint32                  dest_buffer,
      SOC_SAND_IN  uint32                  asd_buffer,
      SOC_SAND_IN  uint32                  flags,
      SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO *fwd_decision
  );



 /*********************************************************************	 
 *     map from header stack type to HW internal value	 
 *********************************************************************/	 
 uint32	 
   arad_pp_pkt_hdr_type_to_interanl_val_map(	 
     SOC_SAND_IN  ARAD_PP_PKT_HDR_STK_TYPE    pkt_hdr_type,	 
     SOC_SAND_OUT uint32                  *internal_val	 
   );	 
 	 
 /*********************************************************************	 
 *     map from HW internal value to header stack type	 
 *********************************************************************/	 
 uint32	 
   arad_pp_pkt_hdr_interanl_val_to_type_map(	 
     SOC_SAND_IN  uint32                  internal_val,	 
     SOC_SAND_OUT ARAD_PP_PKT_HDR_STK_TYPE    *pkt_hdr_type	 
   );

/*********************************************************************
*    map from HW internal value to L2 next protocol type
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_to_interanl_val_map(
    SOC_SAND_IN  ARAD_PP_L2_NEXT_PRTCL_TYPE    l2_next_prtcl_type,
    SOC_SAND_OUT uint32                    *internal_val,
    SOC_SAND_OUT uint8                   *found
  );

/*********************************************************************
*    map from HW internal value to L2 next protocol type
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_interanl_val_to_type_map(
    SOC_SAND_IN  uint32                  internal_val,
    SOC_SAND_OUT ARAD_PP_L2_NEXT_PRTCL_TYPE    *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                   *found
  );

/*********************************************************************
*    Allocate l2 protocol type:
*    If l2_next_prtcl_type is one of ARAD_PP_L2_NEXT_PRTCL_TYPE, returns
*    its internal value. Else, tries to allocate one of 7 custom entries.
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_allocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE      *success
  );

/*********************************************************************
*    Allocate l2 protocol type:
*    If l2_next_prtcl_type is one of ARAD_PP_L2_NEXT_PRTCL_TYPE, returns
*    its internal value. Else, tries to allocate one of 7 custom entries.
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT uint8                 *found
  );

uint32
  arad_pp_l2_next_prtcl_type_from_internal_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  internal_ndx,
    SOC_SAND_OUT uint32                  *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                 *found
  );

/*********************************************************************
*    Deallocate l2 protocol type:
*    If l2_next_prtcl_type is one of ARAD_PP_L2_NEXT_PRTCL_TYPE, does nothing.
*    Else, tries to deallocate from the 7 custom entries.
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_deallocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type
  );

/*********************************************************************
*  Checks whether l2_next_prtcl_type can be successfully allocated.
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_allocate_test_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  );

uint32
  arad_pp_l2_next_prtcl_type_allocate_test_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  );


/*
 * L3 Next-Protocol Management
 */
uint32
  arad_pp_l3_prtcl_to_ndx(
    SOC_SAND_IN  ARAD_PP_L3_NEXT_PRTCL_TYPE prtcl_type,
    SOC_SAND_OUT uint8 *ndx
  );

uint32
  arad_pp_ndx_to_l3_prtcl(
    SOC_SAND_IN  uint8 ndx,
    SOC_SAND_OUT ARAD_PP_L3_NEXT_PRTCL_TYPE *prtcl_type,
    SOC_SAND_OUT uint8 *is_found
  );

uint32
  arad_pp_l3_next_protocol_add(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8              *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );

uint32
  arad_pp_l3_next_protocol_remove(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx
  );

uint32
  arad_pp_l3_next_protocol_find(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8              *internal_ndx,
    SOC_SAND_OUT uint8             *is_found
  );


/*********************************************************************
* NAME:
 *   arad_pp_general_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_general_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_general_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_general_get_errs_ptr(void);

uint32
  ARAD_PP_TRAP_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_INFO *info
  );

uint32
  ARAD_PP_OUTLIF_verify(
    SOC_SAND_IN  ARAD_PP_OUTLIF *info
  );

uint32
  ARAD_PP_MPLS_COMMAND_verify(
    SOC_SAND_IN  ARAD_PP_MPLS_COMMAND *info
  );

uint32
  ARAD_PP_EEI_verify(
    SOC_SAND_IN  ARAD_PP_EEI *info
  );

uint32
  ARAD_PP_FRWRD_DECISION_TYPE_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE       type,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE_INFO *info
  );

uint32
  ARAD_PP_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO *info
  );

uint32
  ARAD_PP_ACTION_PROFILE_verify(
    SOC_SAND_IN  ARAD_PP_ACTION_PROFILE *info
  );

uint32
  ARAD_PP_TPID_PROFILE_verify(
    SOC_SAND_IN  ARAD_PP_TPID_PROFILE *info
  );

uint32
  ARAD_PP_PEP_KEY_verify(
    SOC_SAND_IN  ARAD_PP_PEP_KEY *info
  );

uint32
  ARAD_PP_IP_ROUTING_TABLE_ITER_verify(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_ITER *info
  );

uint32
  ARAD_PP_IP_ROUTING_TABLE_RANGE_verify(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_RANGE *info
  );

uint32
  ARAD_PP_FRWRD_DECISION_INFO_with_encode_type_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO *info
  );

uint32
  arad_pp_SAND_PP_SYS_PORT_ID_verify(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID *info
  );

const char*
  ARAD_PP_DEST_ENCODE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DEST_ENCODE_TYPE enum_val
  );

/* 
 *  These functions stay the same for Jericho because learning data
 *  size in lif table is similar to the size in Arad.
 *  As a result we can't learn all possible forwarding information.
 */
uint32
  arad_pp_fwd_decision_to_learn_buffer(
        SOC_SAND_IN  int                         unit,
        SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE    fwd_decision_type,
        SOC_SAND_IN  uint32                         dest_id,
        SOC_SAND_OUT uint32                         *learn_buffer,
        SOC_SAND_OUT uint32                         *additional_info
  );
uint32
  arad_pp_fwd_decision_from_learn_buffer(
        SOC_SAND_IN  int                      unit,
        SOC_SAND_IN  uint32                      learn_buffer,
        SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO *fwd_decision
  );
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_GENERAL_INCLUDED__*/
#endif


