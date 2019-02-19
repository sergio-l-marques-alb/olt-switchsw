/* $Id: pcp_general.h,v 1.11 Broadcom SDK $
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

#ifndef __SOC_PCP_GENERAL_INCLUDED__
/* { */
#define __SOC_PCP_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_oam_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_PORT_MAX                                         (63)
#define PCP_RIF_ID_MAX                                       (4*1024-1)
#define PCP_VSI_ID_MAX                                       (16*1024-1)
#define PCP_NOF_VRFS                                         (256)
#define PCP_FEC_ID_MAX                                       (16*1024-1)

#define PCP_DEST_ENCODE_TOTAL_IN_BITS_EM                     (16)
#define PCP_DEST_ENCODE_TOTAL_IN_BITS_16                     (16)
#define PCP_DEST_ENCODE_TOTAL_IN_BITS_17                     (17)
/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define PCP_ALLOC(var, type, count, str)                                       \
  {                                                                       \
    if(var != NULL)                                                       \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(PCP_ALLOC_TO_NON_NULL_ERR, PCP_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc((count) * sizeof(type),str);                  \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, PCP_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, PCP_GEN_ERR_NUM_ALLOC, exit);             \
  }
#define PCP_ALLOC_ANY_SIZE(var, type, count, str)                              \
  {                                                                       \
    if(var != NULL)                                                       \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(PCP_ALLOC_TO_NON_NULL_ERR, PCP_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc_any_size((count) * sizeof(type), str);         \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, PCP_GEN_ERR_NUM_ALLOC_ANY_SET, exit);     \
  }



#define PCP_FREE(var)                                                     \
  if (var != NULL)                                                        \
  {                                                                       \
    soc_sand_os_free(var);                                                    \
    var = NULL;                                                           \
  }

#define PCP_FREE_ANY_SIZE(var)                                            \
  if (var != NULL)                                                        \
  {                                                                       \
    res = soc_sand_os_free_any_size(var);                                     \
    SOC_SAND_CHECK_FUNC_RESULT(res, PCP_GEN_ERR_NUM_FREE, exit);              \
    var = NULL;                                                           \
  }

#define PCP_CLEAR(var_ptr, type, count)                                   \
  {                                                                       \
    if (var_ptr == NULL)                                                  \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var_ptr,                                                      \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, PCP_GEN_ERR_NUM_CLEAR, exit);             \
  }

#define PCP_COPY(var_dest_ptr, var_src_ptr, type, count)                  \
  {                                                                       \
    res = soc_sand_os_memcpy(                                                 \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, PCP_GEN_ERR_NUM_CLEAR, exit);             \
  }


#define PCP_DO_NOTHING_AND_EXIT                                           \
  do                                                                      \
  {                                                                       \
    SOC_SAND_IGNORE_UNUSED_VAR(res);                                          \
    goto exit;                                                            \
  } while (0)

#define PCP_STRUCT_VERIFY(type, name, exit_num, exit_place)               \
          do                                                              \
          {                                                               \
            res = type##_verify(                                          \
                    name                                                  \
                  );                                                      \
            SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit_place);            \
          } while(0)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* 
 * Supposed to be in pcp_api_general.h 
 */
/*
 *  Virtual switch instance ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_VSI_ID                                         PCP_VSI_ID;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */

  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_FWD_DECISION_DEST_PREFIX_FIND = PCP_PROC_DESC_BASE_GENERAL_FIRST,
  PCP_FWD_DECISION_DEST_TYPE_FIND,
  PCP_FWD_DECISION_IN_BUFFER_PARSE,
  
    /*
   * Last element. Do no touch.
   */
  PCP_GENERAL_PROCEDURE_DESC_LAST
} PCP_GENERAL_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_TM_PORT_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_GENERAL_FIRST,
  PCP_PORT_OUT_OF_RANGE_ERR,
  PCP_RIF_ID_OUT_OF_RANGE_ERR,
  PCP_GENERAL_TYPE_OUT_OF_RANGE_ERR,
  PCP_GENERAL_VAL_OUT_OF_RANGE_ERR,
  PCP_GENERAL_COMMAND_OUT_OF_RANGE_ERR,
  PCP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR,
  PCP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR,
  PCP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
  PCP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  PCP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   PCP_VSI_ID_OUT_OF_RANGE_ERR,
   PCP_VRF_ID_OUT_OF_RANGE_ERR,
   PCP_FEC_ID_OUT_OF_RANGE_ERR,

   PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_EEI_TYPE_INVALID_ERR,
   PCP_FRWRD_DEST_TRAP_CODE_INVALID_ERR,
   PCP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_TRAP_FWD_INVALID_ERR,
   PCP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR,
   PCP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_EEI_VAL_INVALID_ERR,
   PCP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR,
   PCP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR,
   PCP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_FEC_PTR_INVALID_ERR,
   PCP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_DROP_ILLEGAL_ERR,
   
   PCP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
   
   PCP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR,
   PCP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR,
   PCP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,

   PCP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR,
   PCP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR,

   PCP_FEATURE_NOT_SUPPORTED_ERR,
  /*
   * Last element. Do no touch.
   */
  PCP_GENERAL_ERR_LAST
} PCP_GENERAL_ERR;
   
#define PCP_EEI_TYPE_EMPTY                               SOC_PPC_EEI_TYPE_EMPTY
#define PCP_EEI_TYPE_TRILL                               SOC_PPC_EEI_TYPE_TRILL
#define PCP_EEI_TYPE_MPLS                                SOC_PPC_EEI_TYPE_MPLS
#define PCP_EEI_TYPE_MIM                                 SOC_PPC_EEI_TYPE_MIM
#define PCP_NOF_EEI_TYPES                                SOC_PPC_NOF_EEI_TYPES_PB
typedef SOC_PPC_EEI_TYPE                                     PCP_EEI_TYPE;

#define PCP_OUTLIF_ENCODE_TYPE_NONE                      SOC_PPC_OUTLIF_ENCODE_TYPE_NONE
#define PCP_OUTLIF_ENCODE_TYPE_AC                        SOC_PPC_OUTLIF_ENCODE_TYPE_AC
#define PCP_OUTLIF_ENCODE_TYPE_EEP                       SOC_PPC_OUTLIF_ENCODE_TYPE_EEP
#define PCP_OUTLIF_ENCODE_TYPE_VSI                       SOC_PPC_OUTLIF_ENCODE_TYPE_VSI
#define PCP_NOF_OUTLIF_ENCODE_TYPES                      SOC_PPC_NOF_OUTLIF_ENCODE_TYPES
typedef SOC_PPC_OUTLIF_ENCODE_TYPE                           PCP_OUTLIF_ENCODE_TYPE;

#define PCP_FRWRD_DECISION_TYPE_DROP                     SOC_PPC_FRWRD_DECISION_TYPE_DROP
#define PCP_FRWRD_DECISION_TYPE_UC_FLOW                  SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW
#define PCP_FRWRD_DECISION_TYPE_UC_LAG                   SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG
#define PCP_FRWRD_DECISION_TYPE_UC_PORT                  SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT
#define PCP_FRWRD_DECISION_TYPE_MC                       SOC_PPC_FRWRD_DECISION_TYPE_MC
#define PCP_FRWRD_DECISION_TYPE_FEC                      SOC_PPC_FRWRD_DECISION_TYPE_FEC
#define PCP_FRWRD_DECISION_TYPE_TRAP                     SOC_PPC_FRWRD_DECISION_TYPE_TRAP
#define PCP_NOF_FRWRD_DECISION_TYPES                     SOC_PPC_NOF_FRWRD_DECISION_TYPES
typedef SOC_PPC_FRWRD_DECISION_TYPE                          PCP_FRWRD_DECISION_TYPE;

#define PCP_MPLS_COMMAND_TYPE_PUSH                       SOC_PPC_MPLS_COMMAND_TYPE_PUSH
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM
#define PCP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET          SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET
#define PCP_MPLS_COMMAND_TYPE_SWAP                       SOC_PPC_MPLS_COMMAND_TYPE_SWAP
#define PCP_NOF_MPLS_COMMAND_TYPES                       SOC_PPC_NOF_MPLS_COMMAND_TYPES
typedef SOC_PPC_MPLS_COMMAND_TYPE                            PCP_MPLS_COMMAND_TYPE;

typedef SOC_PPC_TRAP_INFO                                      PCP_TRAP_INFO;
typedef SOC_PPC_OUTLIF                                         PCP_OUTLIF;
typedef SOC_PPC_MPLS_COMMAND                                   PCP_MPLS_COMMAND;
typedef SOC_PPC_EEI_VAL                                        PCP_EEI_VAL;
typedef SOC_PPC_EEI                                            PCP_EEI;
typedef SOC_PPC_FRWRD_DECISION_TYPE_INFO                       PCP_FRWRD_DECISION_TYPE_INFO;
typedef SOC_PPC_FRWRD_DECISION_INFO                            PCP_FRWRD_DECISION_INFO;
typedef SOC_PPC_ACTION_PROFILE                                 PCP_ACTION_PROFILE;

typedef enum
{
  /*
   *  EM encoding type
   */
  PCP_DEST_ENCODE_TYPE_EM = 0,

  /*
   *  16 bit encoding
   */
   PCP_DEST_ENCODE_TYPE_16_BITS = 1,

  /*
   *  17 bit encoding
   */
   PCP_DEST_ENCODE_TYPE_17_BITS = 2,

 /*
  *  Number of encoding types
  */
  PCP_NOF_DEST_ENCODE_TYPES = 3
}PCP_DEST_ENCODE_TYPE;
typedef struct{
  uint32      base;
  uint32      size;
  uint32     wrd_sz;
  const char    *name;
  uint32      mod_id;
}PCP_TBL_PRINT_INFO;

#define PCP_DEST_TYPE_QUEUE                              SOC_TMC_DEST_TYPE_QUEUE
#define PCP_DEST_TYPE_MULTICAST                          SOC_TMC_DEST_TYPE_MULTICAST
#define PCP_DEST_TYPE_SYS_PHY_PORT                       SOC_TMC_DEST_TYPE_SYS_PHY_PORT
#define PCP_DEST_TYPE_LAG                                SOC_TMC_DEST_TYPE_LAG
typedef SOC_TMC_DEST_TYPE                                    PCP_DEST_TYPES;

/*
 * Application type
 *
 * Forwarding decision encoding format varies for different applications.
 * Application type determines destination encoding type (EM, 16 bit, etc.)
 *  and asd format
 */
typedef enum
{
  PCP_FRWRD_DECISION_APPLICATION_TYPE_EMPTY = 0,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_TRAP,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_FEC,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_IP,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_PMF,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_17,
  PCP_FRWRD_DECISION_APPLICATION_TYPE_16,
  PCP_FRWRD_DECISION_APPLICATION_TYPES
} PCP_FRWRD_DECISION_APPLICATION_TYPE;

/* */
typedef struct  
{
  /*
   *  Destination identifier
   */
  uint32 enc_dst;
  /* 
   *  Application Specific Data
   */ 
  uint32 enc_asd;

}PCP_FRWRD_DECISION_ENCODED_INFO;

/* From soc_pb_pp_api_general.h */

/*
 *  Local PP port. Range : 0 - 63.
 */
typedef SOC_PPC_PORT                                           PCP_PORT;

/*
 *  Filtering ID. Range: Soc_petraB: 0 - 16K-1.
 */
typedef SOC_PPC_FID                                            PCP_FID;

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

void
  PCP_TRAP_INFO_clear(
    SOC_SAND_OUT PCP_TRAP_INFO *info
  );

void
  PCP_OUTLIF_clear(
    SOC_SAND_OUT PCP_OUTLIF *info
  );

void
  PCP_MPLS_COMMAND_clear(
    SOC_SAND_OUT PCP_MPLS_COMMAND *info
  );

void
  PCP_EEI_VAL_clear(
    SOC_SAND_OUT PCP_EEI_VAL *info
  );

void
  PCP_EEI_clear(
    SOC_SAND_OUT PCP_EEI *info
  );

void
  PCP_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_DECISION_TYPE_INFO *info
  );

void
  PCP_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO *info
  );

void
  PCP_ACTION_PROFILE_clear(
    SOC_SAND_OUT PCP_ACTION_PROFILE *info
  );

uint32
  pcp_fwd_decision_in_buffer_build(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_IN  uint8                 is_sa_drop,
    SOC_SAND_OUT uint32                   *dest_buffer,
    SOC_SAND_OUT uint32                   *asd_buffer
  );

uint32
  pcp_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  uint32                  dest_buffer,
    SOC_SAND_IN  uint32                  asd_buffer,
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT uint8                 *is_sa_drop
  );

uint32
  PCP_TRAP_INFO_verify(
    SOC_SAND_IN  PCP_TRAP_INFO *info
  );
uint32
  PCP_OUTLIF_verify(
    SOC_SAND_IN  PCP_OUTLIF *info
  );

uint32
  PCP_MPLS_COMMAND_verify(
    SOC_SAND_IN  PCP_MPLS_COMMAND *info
  );

uint32
  PCP_EEI_verify(
    SOC_SAND_IN  PCP_EEI *info
  );

uint32
  PCP_FRWRD_DECISION_TYPE_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_DECISION_TYPE       type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_TYPE_INFO *info
  );

uint32
  PCP_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO *info
  );
uint32
  PCP_ACTION_PROFILE_verify(
    SOC_SAND_IN  PCP_ACTION_PROFILE *info
  );
uint32
  PCP_FRWRD_DECISION_INFO_with_encode_type_verify(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO *info
  );

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_general_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_general_get_procs_ptr(void);

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_general_get_errs_ptr(void);

/* } */

/* ************************* pcp_api_trap_mgmt.h ************************* */

#define PCP_TRAP_CODE_PBP_SA_DROP_0                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_0
#define PCP_TRAP_CODE_PBP_SA_DROP_1                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_1
#define PCP_TRAP_CODE_PBP_SA_DROP_2                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_2
#define PCP_TRAP_CODE_PBP_SA_DROP_3                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_3
#define PCP_TRAP_CODE_PBP_TE_TRANSPLANT                  SOC_PPC_TRAP_CODE_PBP_TE_TRANSPLANT
#define PCP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL              SOC_PPC_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL
#define PCP_TRAP_CODE_PBP_TRANSPLANT                     SOC_PPC_TRAP_CODE_PBP_TRANSPLANT
#define PCP_TRAP_CODE_PBP_LEARN_SNOOP                    SOC_PPC_TRAP_CODE_PBP_LEARN_SNOOP
#define PCP_TRAP_CODE_SA_AUTHENTICATION_FAILED           SOC_PPC_TRAP_CODE_SA_AUTHENTICATION_FAILED
#define PCP_TRAP_CODE_PORT_NOT_PERMITTED                 SOC_PPC_TRAP_CODE_PORT_NOT_PERMITTED
#define PCP_TRAP_CODE_UNEXPECTED_VID                     SOC_PPC_TRAP_CODE_UNEXPECTED_VID
#define PCP_TRAP_CODE_SA_MULTICAST                       SOC_PPC_TRAP_CODE_SA_MULTICAST
#define PCP_TRAP_CODE_SA_EQUALS_DA                       SOC_PPC_TRAP_CODE_SA_EQUALS_DA
#define PCP_TRAP_CODE_8021X                              SOC_PPC_TRAP_CODE_8021X
#define PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP         SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP
#define PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT       SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT
#define PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1     SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1
#define PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2     SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2
#define PCP_TRAP_CODE_MY_ARP                             SOC_PPC_TRAP_CODE_MY_ARP
#define PCP_TRAP_CODE_ARP                                SOC_PPC_TRAP_CODE_ARP
#define PCP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY              SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY
#define PCP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG              SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG
#define PCP_TRAP_CODE_IGMP_UNDEFINED                     SOC_PPC_TRAP_CODE_IGMP_UNDEFINED
#define PCP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY       SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY
#define PCP_TRAP_CODE_RESERVED_MC_0                      SOC_PPC_TRAP_CODE_RESERVED_MC_0
#define PCP_TRAP_CODE_RESERVED_MC_7                      SOC_PPC_TRAP_CODE_RESERVED_MC_7
#define PCP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG         SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG
#define PCP_TRAP_CODE_ICMPV6_MLD_UNDEFINED               SOC_PPC_TRAP_CODE_ICMPV6_MLD_UNDEFINED
#define PCP_TRAP_CODE_DHCP_SERVER                        SOC_PPC_TRAP_CODE_DHCP_SERVER
#define PCP_TRAP_CODE_DHCP_CLIENT                        SOC_PPC_TRAP_CODE_DHCP_CLIENT
#define PCP_TRAP_CODE_DHCPV6_SERVER                      SOC_PPC_TRAP_CODE_DHCPV6_SERVER
#define PCP_TRAP_CODE_DHCPV6_CLIENT                      SOC_PPC_TRAP_CODE_DHCPV6_CLIENT
#define PCP_TRAP_CODE_PROG_TRAP_0                        SOC_PPC_TRAP_CODE_PROG_TRAP_0
#define PCP_TRAP_CODE_PROG_TRAP_1                        SOC_PPC_TRAP_CODE_PROG_TRAP_1
#define PCP_TRAP_CODE_PROG_TRAP_2                        SOC_PPC_TRAP_CODE_PROG_TRAP_2
#define PCP_TRAP_CODE_PROG_TRAP_3                        SOC_PPC_TRAP_CODE_PROG_TRAP_3
#define PCP_TRAP_CODE_PORT_NOT_VLAN_MEMBER               SOC_PPC_TRAP_CODE_PORT_NOT_VLAN_MEMBER
#define PCP_TRAP_CODE_HEADER_SIZE_ERR                    SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR
#define PCP_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS             SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS
#define PCP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL            SOC_PPC_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL
#define PCP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID              SOC_PPC_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID
#define PCP_TRAP_CODE_STP_STATE_BLOCK                    SOC_PPC_TRAP_CODE_STP_STATE_BLOCK
#define PCP_TRAP_CODE_STP_STATE_LEARN                    SOC_PPC_TRAP_CODE_STP_STATE_LEARN
#define PCP_TRAP_CODE_IP_COMP_MC_INVALID_IP              SOC_PPC_TRAP_CODE_IP_COMP_MC_INVALID_IP
#define PCP_TRAP_CODE_MY_MAC_AND_IP_DISABLE              SOC_PPC_TRAP_CODE_MY_MAC_AND_IP_DISABLE
#define PCP_TRAP_CODE_TRILL_VERSION                      SOC_PPC_TRAP_CODE_TRILL_VERSION
#define PCP_TRAP_CODE_TRILL_INVALID_TTL                  SOC_PPC_TRAP_CODE_TRILL_INVALID_TTL
#define PCP_TRAP_CODE_TRILL_CHBH                         SOC_PPC_TRAP_CODE_TRILL_CHBH
#define PCP_TRAP_CODE_TRILL_NO_REVERSE_FEC               SOC_PPC_TRAP_CODE_TRILL_NO_REVERSE_FEC
#define PCP_TRAP_CODE_TRILL_CITE                         SOC_PPC_TRAP_CODE_TRILL_CITE
#define PCP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC             SOC_PPC_TRAP_CODE_TRILL_ILLEGAL_INNER_MC
#define PCP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE            SOC_PPC_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE
#define PCP_TRAP_CODE_MY_MAC_AND_ARP                     SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP
#define PCP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3              SOC_PPC_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3
#define PCP_TRAP_CODE_MPLS_LABEL_VALUE_0                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_0
#define PCP_TRAP_CODE_MPLS_LABEL_VALUE_1                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_1
#define PCP_TRAP_CODE_MPLS_LABEL_VALUE_2                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_2
#define PCP_TRAP_CODE_MPLS_LABEL_VALUE_3                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_3
#define PCP_TRAP_CODE_MPLS_NO_RESOURCES                  SOC_PPC_TRAP_CODE_MPLS_NO_RESOURCES
#define PCP_TRAP_CODE_INVALID_LABEL_IN_RANGE             SOC_PPC_TRAP_CODE_INVALID_LABEL_IN_RANGE
#define PCP_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM          SOC_PPC_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM
#define PCP_TRAP_CODE_MPLS_LSP_BOS                       SOC_PPC_TRAP_CODE_MPLS_LSP_BOS
#define PCP_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14           SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14
#define PCP_TRAP_CODE_MPLS_PWE_NO_BOS                    SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS
#define PCP_TRAP_CODE_MPLS_VRF_NO_BOS                    SOC_PPC_TRAP_CODE_MPLS_VRF_NO_BOS
#define PCP_TRAP_CODE_MPLS_TERM_TTL_0                    SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_0
#define PCP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP        SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP
#define PCP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP        SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP
#define PCP_TRAP_CODE_IPV4_TERM_VERSION_ERROR            SOC_PPC_TRAP_CODE_IPV4_TERM_VERSION_ERROR
#define PCP_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR           SOC_PPC_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR
#define PCP_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR      SOC_PPC_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR
#define PCP_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR       SOC_PPC_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR
#define PCP_TRAP_CODE_IPV4_TERM_TTL0                     SOC_PPC_TRAP_CODE_IPV4_TERM_TTL0
#define PCP_TRAP_CODE_IPV4_TERM_HAS_OPTIONS              SOC_PPC_TRAP_CODE_IPV4_TERM_HAS_OPTIONS
#define PCP_TRAP_CODE_IPV4_TERM_TTL1                     SOC_PPC_TRAP_CODE_IPV4_TERM_TTL1
#define PCP_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP            SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP
#define PCP_TRAP_CODE_IPV4_TERM_DIP_ZERO                 SOC_PPC_TRAP_CODE_IPV4_TERM_DIP_ZERO
#define PCP_TRAP_CODE_IPV4_TERM_SIP_IS_MC                SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_IS_MC
#define PCP_TRAP_CODE_CFM_ACCELERATED_INGRESS            SOC_PPC_TRAP_CODE_CFM_ACCELERATED_INGRESS
#define PCP_TRAP_CODE_ILLEGEL_PFC                        SOC_PPC_TRAP_CODE_ILLEGEL_PFC
#define PCP_TRAP_CODE_SA_DROP_0                          SOC_PPC_TRAP_CODE_SA_DROP_0
#define PCP_TRAP_CODE_SA_DROP_1                          SOC_PPC_TRAP_CODE_SA_DROP_1
#define PCP_TRAP_CODE_SA_DROP_2                          SOC_PPC_TRAP_CODE_SA_DROP_2
#define PCP_TRAP_CODE_SA_DROP_3                          SOC_PPC_TRAP_CODE_SA_DROP_3
#define PCP_TRAP_CODE_SA_NOT_FOUND_0                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0
#define PCP_TRAP_CODE_SA_NOT_FOUND_1                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1
#define PCP_TRAP_CODE_SA_NOT_FOUND_2                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2
#define PCP_TRAP_CODE_SA_NOT_FOUND_3                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3
#define PCP_TRAP_CODE_UNKNOWN_DA_0                       SOC_PPC_TRAP_CODE_UNKNOWN_DA_0
#define PCP_TRAP_CODE_UNKNOWN_DA_7                       SOC_PPC_TRAP_CODE_UNKNOWN_DA_7
#define PCP_TRAP_CODE_ELK_ERROR                          SOC_PPC_TRAP_CODE_ELK_ERROR
#define PCP_TRAP_CODE_DA_NOT_FOUND_0                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_0
#define PCP_TRAP_CODE_DA_NOT_FOUND_1                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_1
#define PCP_TRAP_CODE_DA_NOT_FOUND_2                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_2
#define PCP_TRAP_CODE_DA_NOT_FOUND_3                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_3
#define PCP_TRAP_CODE_P2P_MISCONFIGURATION               SOC_PPC_TRAP_CODE_P2P_MISCONFIGURATION
#define PCP_TRAP_CODE_SAME_INTERFACE                     SOC_PPC_TRAP_CODE_SAME_INTERFACE
#define PCP_TRAP_CODE_TRILL_UNKNOWN_UC                   SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_UC
#define PCP_TRAP_CODE_TRILL_UNKNOWN_MC                   SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_MC
#define PCP_TRAP_CODE_UC_LOOSE_RPF_FAIL                  SOC_PPC_TRAP_CODE_UC_LOOSE_RPF_FAIL
#define PCP_TRAP_CODE_DEFAULT_UCV6                       SOC_PPC_TRAP_CODE_DEFAULT_UCV6
#define PCP_TRAP_CODE_DEFAULT_MCV6                       SOC_PPC_TRAP_CODE_DEFAULT_MCV6
#define PCP_TRAP_CODE_MPLS_P2P_NO_BOS                    SOC_PPC_TRAP_CODE_MPLS_P2P_NO_BOS
#define PCP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP             SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_TRAP
#define PCP_TRAP_CODE_MPLS_CONTROL_WORD_DROP             SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_DROP
#define PCP_TRAP_CODE_MPLS_UNKNOWN_LABEL                 SOC_PPC_TRAP_CODE_MPLS_UNKNOWN_LABEL
#define PCP_TRAP_CODE_MPLS_P2P_MPLSX4                    SOC_PPC_TRAP_CODE_MPLS_P2P_MPLSX4
#define PCP_TRAP_CODE_ETH_L2CP_PEER                      SOC_PPC_TRAP_CODE_ETH_L2CP_PEER
#define PCP_TRAP_CODE_ETH_L2CP_DROP                      SOC_PPC_TRAP_CODE_ETH_L2CP_DROP
#define PCP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY       SOC_PPC_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY
#define PCP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG       SOC_PPC_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG
#define PCP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED              SOC_PPC_TRAP_CODE_ETH_FL_IGMP_UNDEFINED
#define PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY
#define PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE      SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE
#define PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED        SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED
#define PCP_TRAP_CODE_IPV4_VERSION_ERROR                 SOC_PPC_TRAP_CODE_IPV4_VERSION_ERROR
#define PCP_TRAP_CODE_IPV4_CHECKSUM_ERROR                SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR
#define PCP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR           SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR
#define PCP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR            SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR
#define PCP_TRAP_CODE_IPV4_TTL0                          SOC_PPC_TRAP_CODE_IPV4_TTL0
#define PCP_TRAP_CODE_IPV4_HAS_OPTIONS                   SOC_PPC_TRAP_CODE_IPV4_HAS_OPTIONS
#define PCP_TRAP_CODE_IPV4_TTL1                          SOC_PPC_TRAP_CODE_IPV4_TTL1
#define PCP_TRAP_CODE_IPV4_SIP_EQUAL_DIP                 SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP
#define PCP_TRAP_CODE_IPV4_DIP_ZERO                      SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO
#define PCP_TRAP_CODE_IPV4_SIP_IS_MC                     SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC
#define PCP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED SOC_PPC_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED
#define PCP_TRAP_CODE_IPV6_VERSION_ERROR                 SOC_PPC_TRAP_CODE_IPV6_VERSION_ERROR
#define PCP_TRAP_CODE_IPV6_HOP_COUNT0                    SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT0
#define PCP_TRAP_CODE_IPV6_HOP_COUNT1                    SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1
#define PCP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION       SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION
#define PCP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS              SOC_PPC_TRAP_CODE_IPV6_LOOPBACK_ADDRESS
#define PCP_TRAP_CODE_IPV6_MULTICAST_SOURCE              SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE
#define PCP_TRAP_CODE_IPV6_NEXT_HEADER_NULL              SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL
#define PCP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE            SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE
#define PCP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION        SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION
#define PCP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION        SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION
#define PCP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE             SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE
#define PCP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE             SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE
#define PCP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION   SOC_PPC_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION
#define PCP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION       SOC_PPC_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION
#define PCP_TRAP_CODE_IPV6_MULTICAST_DESTINATION         SOC_PPC_TRAP_CODE_IPV6_MULTICAST_DESTINATION
#define PCP_TRAP_CODE_MPLS_TTL0                          SOC_PPC_TRAP_CODE_MPLS_TTL0
#define PCP_TRAP_CODE_MPLS_TTL1                          SOC_PPC_TRAP_CODE_MPLS_TTL1
#define PCP_TRAP_CODE_TCP_SN_FLAGS_ZERO                  SOC_PPC_TRAP_CODE_TCP_SN_FLAGS_ZERO
#define PCP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET              SOC_PPC_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET
#define PCP_TRAP_CODE_TCP_SYN_FIN                        SOC_PPC_TRAP_CODE_TCP_SYN_FIN
#define PCP_TRAP_CODE_TCP_EQUAL_PORTS                    SOC_PPC_TRAP_CODE_TCP_EQUAL_PORTS
#define PCP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER     SOC_PPC_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER
#define PCP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8            SOC_PPC_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8
#define PCP_TRAP_CODE_UDP_EQUAL_PORTS                    SOC_PPC_TRAP_CODE_UDP_EQUAL_PORTS
#define PCP_TRAP_CODE_ICMP_DATA_GT_576                   SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576
#define PCP_TRAP_CODE_ICMP_FRAGMENTED                    SOC_PPC_TRAP_CODE_ICMP_FRAGMENTED
#define PCP_TRAP_CODE_GENERAL                            SOC_PPC_TRAP_CODE_GENERAL
#define PCP_TRAP_CODE_FACILITY_INVALID                   SOC_PPC_TRAP_CODE_FACILITY_INVALID
#define PCP_TRAP_CODE_FEC_ENTRY_ACCESSED                 SOC_PPC_TRAP_CODE_FEC_ENTRY_ACCESSED
#define PCP_TRAP_CODE_UC_STRICT_RPF_FAIL                 SOC_PPC_TRAP_CODE_UC_STRICT_RPF_FAIL
#define PCP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL               SOC_PPC_TRAP_CODE_MC_EXPLICIT_RPF_FAIL
#define PCP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL          SOC_PPC_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL
#define PCP_TRAP_CODE_MC_USE_SIP_RPF_FAIL                SOC_PPC_TRAP_CODE_MC_USE_SIP_RPF_FAIL
#define PCP_TRAP_CODE_MC_USE_SIP_ECMP                    SOC_PPC_TRAP_CODE_MC_USE_SIP_ECMP
#define PCP_TRAP_CODE_ICMP_REDIRECT                      SOC_PPC_TRAP_CODE_ICMP_REDIRECT
#define PCP_TRAP_CODE_USER_OAMP                          SOC_PPC_TRAP_CODE_USER_OAMP
#define PCP_TRAP_CODE_USER_ETH_OAM_ACCELERATED           SOC_PPC_TRAP_CODE_USER_ETH_OAM_ACCELERATED
#define PCP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED          SOC_PPC_TRAP_CODE_USER_MPLS_OAM_ACCELERATED
#define PCP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED SOC_PPC_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED
#define PCP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED       SOC_PPC_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED
#define PCP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED        SOC_PPC_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED
#define PCP_TRAP_CODE_USER_DEFINED_0                     SOC_PPC_TRAP_CODE_USER_DEFINED_0
#define PCP_TRAP_CODE_USER_DEFINED_5                     SOC_PPC_TRAP_CODE_USER_DEFINED_5
#define PCP_TRAP_CODE_USER_DEFINED_6                     SOC_PPC_TRAP_CODE_USER_DEFINED_6
#define PCP_TRAP_CODE_USER_DEFINED_7                     SOC_PPC_TRAP_CODE_USER_DEFINED_7
#define PCP_TRAP_CODE_USER_DEFINED_8                     SOC_PPC_TRAP_CODE_USER_DEFINED_8
#define PCP_TRAP_CODE_USER_DEFINED_10                    SOC_PPC_TRAP_CODE_USER_DEFINED_10
#define PCP_TRAP_CODE_USER_DEFINED_11                    SOC_PPC_TRAP_CODE_USER_DEFINED_11
#define PCP_TRAP_CODE_USER_DEFINED_12                    SOC_PPC_TRAP_CODE_USER_DEFINED_12
#define PCP_TRAP_CODE_USER_DEFINED_13                    SOC_PPC_TRAP_CODE_USER_DEFINED_13
#define PCP_TRAP_CODE_USER_DEFINED_27                    SOC_PPC_TRAP_CODE_USER_DEFINED_27
#define PCP_TRAP_CODE_USER_DEFINED_28                    SOC_PPC_TRAP_CODE_USER_DEFINED_28
#define PCP_TRAP_CODE_USER_DEFINED_35                    SOC_PPC_TRAP_CODE_USER_DEFINED_35
#define PCP_TRAP_CODE_USER_DEFINED_36                    SOC_PPC_TRAP_CODE_USER_DEFINED_36
#define PCP_TRAP_CODE_USER_DEFINED_51                    SOC_PPC_TRAP_CODE_USER_DEFINED_51
#define PCP_TRAP_CODE_USER_DEFINED_52                    SOC_PPC_TRAP_CODE_USER_DEFINED_52
#define PCP_TRAP_CODE_USER_DEFINED_59                    SOC_PPC_TRAP_CODE_USER_DEFINED_59
#define PCP_NOF_TRAP_CODES                               SOC_PPC_NOF_TRAP_CODES
typedef SOC_PPC_TRAP_CODE                                      PCP_TRAP_CODE;

/* ************************* pcp_trap_mgmt.h ************************* */

/*  Prefixes */
#define PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX  (0x0<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX  (0x1<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX  (0x2<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX  (0x3<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX  (0x4<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX  (0x5<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX  (0x6<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX  (0x7<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX  (0x8<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX  (0x9<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX  (0xa<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX   (0xb<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX   (0xc<<4)
#define PCP_TRAP_CODE_INTERNAL_IHP_TIMNA_PREFIX (0xd<<4)
#define PCP_TRAP_CODE_INTERNAL_USER_PREFIX      (0xf<<4)

typedef enum
{
  /*  Link-Layer Cpu-Trap-Codes */
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0                         = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1                         = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2                         = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3                         = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT                    = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL                = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT                       = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP                      = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED             = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED                   = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID                       = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST                         = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA                         = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xc,
  PCP_TRAP_CODE_INTERNAL_LLR_8021X                                = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xd,
  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0               = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xe,
  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1               = PCP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xf,

  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2               = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3               = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_LLR_MY_ARP                               = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_LLR_ARP                                  = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY                = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG                = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED                       = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY         = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX                   = PCP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x8, /*  prefix */

  PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG           = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED                 = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER                          = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT                          = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER                        = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT                        = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0                        = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1                        = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2                        = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3                        = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x9,

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5               = PCP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0xa, /* SW prefix */

  /*  VTT Cpu-Trap-Codes */
  PCP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER                 = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS               = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL              = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID                = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE                = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE              = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP                       = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3                = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP                = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION                        = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL                    = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH                           = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xc,
  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC                 = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xd,
  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE                           = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xe,
  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC               = PCP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xf,

  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0                    = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1                    = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2                    = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3                    = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES                    = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE          = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM            = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS                         = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14             = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0                           = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP               = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xc,
  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP               = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xd,
  PCP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xe,
  PCP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xf,

  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR                     = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR                = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR                 = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0                               = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS                        = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1                               = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP                      = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO                           = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC                          = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED  = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS                 = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC                             = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xc,

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7               = PCP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xd, /* SW prefix */

  /*  Forwarding-Lookup Cpu-Trap-Codes */
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY         = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG         = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED                = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY  = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE        = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED          = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR                   = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR                  = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR             = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR              = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0                            = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xc,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS                     = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xd,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1                            = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xe,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP                   = PCP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xf,

  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC                       = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR                   = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION         = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS                = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE                = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL                = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE              = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION          = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION          = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE               = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xc,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE               = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xd,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION     = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xe,
  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION         = PCP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xf,

  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION           = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0                            = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1                            = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO                    = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET                = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN                          = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER       = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8              = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576                     = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP0                             = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xc,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP1                             = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xd,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP2                             = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xe,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP3                             = PCP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xf,

  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND0                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND1                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND2                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND3                        = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX                    = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x8, /*  prefix */

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10              = PCP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0xc, /* SW prefix */

  PCP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR                            = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x0,
  PCP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION                 = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x1,
  PCP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE                       = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x2,
  PCP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC                     = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x3,
  PCP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC                     = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x4,
  PCP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL                    = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x5,
  PCP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6                         = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x6,
  PCP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6                         = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x7,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x8,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP               = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x9,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP               = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xa,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL                   = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xb,
  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4                      = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xc,

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12             = PCP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xd, /* SW prefix */

  /*  PMF Cpu-Trap-Codes */
  PCP_TRAP_CODE_INTERNAL_PMF_GENERAL                              = PCP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX + 0x0,

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27             = PCP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX + 0x1, /* SW prefix */

  /*  Fec-Resolution Cpu-Trap-Codes */
  PCP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID                     = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x0,
  PCP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED                   = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x1,
  PCP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL                   = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x2,
  PCP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL                 = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x3,
  PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL            = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x4,
  PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL                  = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x5,
  PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP                      = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x6,
  PCP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT                        = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x7,

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35             = PCP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x8, /* SW prefix */

  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51             = PCP_TRAP_CODE_INTERNAL_IHP_TIMNA_PREFIX + 0x0, /* SW prefix */
  PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_67             = PCP_TRAP_CODE_INTERNAL_USER_PREFIX + 0x0, /* SW prefix */


  /*  External device */
  PCP_TRAP_CODE_INTERNAL_USER_OAMP                                = PCP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x0,
  PCP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED            = PCP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x1,
  PCP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED                = PCP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x2,
  PCP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED       = PCP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x3,
  PCP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED             = PCP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x4,
  PCP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED         = PCP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x5

} PCP_TRAP_CODE_INTERNAL;

uint32
  pcp_trap_mgmt_trap_code_to_internal(
    SOC_SAND_IN  PCP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT PCP_TRAP_CODE_INTERNAL                  *trap_code_internal
  );

uint32
  pcp_trap_cpu_trap_code_from_internal_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int32                                 trap_code_internal,
    SOC_SAND_OUT PCP_TRAP_CODE                          *trap_code
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_GENERAL_INCLUDED__*/
#endif

