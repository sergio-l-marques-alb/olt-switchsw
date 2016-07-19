/* $Id$
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

#ifndef __SOC_PB_PP_LEM_ACCESS_INCLUDED__
/* { */
#define __SOC_PB_PP_LEM_ACCESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_sa_auth.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_trill.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ilm.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_vid_assign.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LEM_NOF_ENTRIES                             (SOC_DPP_NOF_LEM_LINES_PETRAB + 32)

#define SOC_PB_PP_LEM_KEY_PARAM_MAX_IN_UINT32S                  (3)
#define SOC_PB_PP_LEM_KEY_MAX_NOF_PARAMS                      (5)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_EXTENDED      (1)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC           (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_BACKBONE_MAC  (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC       (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IP_HOST       (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH       (1)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM           (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC      (1)
#define SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC      (5)


/* $Id$
 *	Prefix characteristic: value and number of significant msb bits.
 *  For example, Backbone MAC keys have a '000' in their msbs:
 *  value is 0 ('000'), size (n bits) is 3. For MAC addresses, fix a number of bits of 1
 *  and a key prefix of 0
 */
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_MAC                  (1)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC     (1)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC                (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC   (3)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC              (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST              (5)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH              (7)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_ILM                  (6)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED             (3)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM     (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED     (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC              (1)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC (4)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC              (0)
#define SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC (4)
#define SOC_PB_PP_EXTENDED_KEY_PREFIX_NOF_BITS                   (4)


#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC          (48)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC          (14) /* Consider FID and not System-VSI */
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC (48)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC (12)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC      (28)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC      (14)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST      (32)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST      (8)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SA_AUTH      (48)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM          (20)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM          (3)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM          (6)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM          (12)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC     (16)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC     (16)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC     (16)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC     (12)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC     (13)
#define SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC     (1)



#define SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S  (SOC_DPP_LEM_WIDTH_PETRAB / SOC_SAND_NOF_BITS_IN_UINT32 + 1)
#define SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS           (24)
#define SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS          (16)
#define SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_BITS       (SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS + SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS + 1)
#define SOC_PB_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S       (SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_BITS / SOC_SAND_NOF_BITS_IN_UINT32 + 1)

/*
 *	Payload composition
 */
#define SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S                  (2)
#define SOC_PB_PP_LEM_ACCESS_ASD_FIRST_REG_LSB                 (SOC_SAND_REG_SIZE_BITS - SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS)
#define SOC_PB_PP_LEM_ACCESS_ASD_FIRST_REG_MSB                 (SOC_SAND_REG_MAX_BIT)
#define SOC_PB_PP_LEM_ACCESS_ASD_FIRST_REG_NOF_BITS            \
  (SOC_PB_PP_LEM_ACCESS_ASD_FIRST_REG_MSB - SOC_PB_PP_LEM_ACCESS_ASD_FIRST_REG_LSB + 1)
#define SOC_PB_PP_LEM_ACCESS_ASD_SCND_REG_LSB                  (0)
#define SOC_PB_PP_LEM_ACCESS_ASD_SCND_REG_MSB                   \
  (SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS - SOC_PB_PP_LEM_ACCESS_ASD_FIRST_REG_NOF_BITS - 1)
#define SOC_PB_PP_LEM_ACCESS_IS_DYN_LSB                         (SOC_PB_PP_LEM_ACCESS_ASD_SCND_REG_MSB + 1)

/*
 *	Field values
 */
#define SOC_PB_PP_LEM_ACCESS_CMD_DELETE_FLD_VAL      0
#define SOC_PB_PP_LEM_ACCESS_CMD_INSERT_FLD_VAL      1
#define SOC_PB_PP_LEM_ACCESS_CMD_REFRESH_FLD_VAL     2
#define SOC_PB_PP_LEM_ACCESS_CMD_LEARN_FLD_VAL       3
#define SOC_PB_PP_LEM_ACCESS_CMD_DEFRAG_FLD_VAL      4
#define SOC_PB_PP_LEM_ACCESS_CMD_ACK_FLD_VAL         5
#define SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT_FLD_VAL  6
#define SOC_PB_PP_LEM_ACCESS_CMD_ERROR_FLD_VAL       7


/*
* ASD formatting for different applications
*/

/*
 * ASD formatting for TRILL-SA-authentication
 */
#define SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LSB       (0)
#define SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_MSB       (12)
#define SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LEN       (SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_MSB - SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LSB + 1)


#define SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LSB     (21)
#define SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_MSB     (21)
#define SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_MSB - SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LSB + 1)


/*
 * ASD formatting for SA-authentication
 */
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB       (0)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_MSB       (11)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LEN       (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB + 1)


#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LSB     (19)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_MSB     (19)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LSB + 1)

#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LSB     (20)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_MSB     (20)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LSB + 1)

#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LSB     (21)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_MSB     (21)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LSB + 1)

#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB     (22)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_MSB     (22)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB + 1)

#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LSB     (23)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_MSB     (23)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LSB + 1)

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
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0000 = 0, /* TRILL MC */
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0001 = 4, /*b-mac*/
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0010 = 2, /* ipv4-mc comp*/
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0011 = 3,  /* extended */
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0100 = 1, /* TRILL UC */
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0101 = 5, /*IP-host*/
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0110 = 6, /*ILM */
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0111 = 7,/* SA-AUTH*/
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_1    = 8
}SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX;



typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LEM_ACCESS_GET_PROCS_PTR = SOC_PB_PP_PROC_DESC_BASE_LEM_ACCESS_FIRST,
  SOC_PB_PP_LEM_ACCESS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_LEM_KEY_ENCODED_BUILD,
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_BUILD,
  SOC_PB_PP_LEM_KEY_ENCODED_PARSE,
  SOC_PB_PP_LEM_ACCESS_PARSE,
  SOC_PB_PP_LEM_REQUEST_SEND,
  SOC_PB_PP_LEM_REQUEST_ANSWER_RECEIVE,
  SOC_PB_PP_LEM_ACCESS_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE,
  SOC_PB_PP_LEM_ACCESS_ENTRY_BY_INDEX_GET_UNSAFE,
  SOC_PB_PP_LEM_ACCESS_AGE_FLD_SET,
  SOC_PB_PP_LEM_ACCESS_AGE_FLD_GET,
  SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_BUILD,
  SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_PARSE,
  SOC_PB_PP_LEM_ILM_KEY_BUILD_SET,
  SOC_PB_PP_LEM_ACCESS_FLP_PROGRAM_MAP_ENTRY_GET,
  SOC_PB_PP_LEM_ACCESS_TCAM_BANK_BITMAP_SET,
  SOC_PB_PP_LEM_ACCESS_TCAM_BANK_BITMAP_GET,
  SOC_PB_PP_LEM_ACCESS_IPV6_BANK_BITMAP_SET,
  SOC_PB_PP_LEM_ACCESS_IPV6_BANK_BITMAP_GET,
  SOC_PB_PP_LEM_ACCESS_TCAM_PREFIX_SET,
  SOC_PB_PP_LEM_ACCESS_TCAM_PREFIX_GET,
  SOC_PB_PP_LEM_ACCESS_IPV6_TCAM_PREFIX_SET,
  SOC_PB_PP_LEM_ACCESS_IPV6_TCAM_PREFIX_GET,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LEM_ACCESS_PROCEDURE_DESC_LAST
} SOC_PB_PP_LEM_ACCESS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LEM_ACCESS_FIRST,
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
  SOC_PB_PP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_STAMP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ASD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_AGE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_DEST_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR,
  SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR,
  SOC_PB_PP_LEM_ACCESS_PROGRAM_NOT_FOUND,
  SOC_PB_PP_LEM_ACCESS_PREFIX_TOO_LONG,
  SOC_PB_PP_LEM_ACCESS_MALFORMED_PREFIX,
  SOC_PB_PP_LEM_ACCESS_KEYS_DONT_MATCH,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LEM_ACCESS_ERR_LAST
} SOC_PB_PP_LEM_ACCESS_ERR;

typedef enum
{
  /*
   *  Delete an entry from the LEM DB.
   */
  SOC_PB_PP_LEM_ACCESS_CMD_DELETE = 0,
  /*
   *  Insert an entry to the DB
   */
  SOC_PB_PP_LEM_ACCESS_CMD_INSERT = 1,
  /*
   *  Refresh an entry
   */
   SOC_PB_PP_LEM_ACCESS_CMD_REFRESH = 2,
  /*
   *  Simulate a learned entry
   */
  SOC_PB_PP_LEM_ACCESS_CMD_LEARN = 3,
  /*
   *  Defrag command (i.e., transfer entries from CAM to LEM DB)
   */
  SOC_PB_PP_LEM_ACCESS_CMD_DEFRAG = 4,
  /*
   *  ACK to receive an Ack
   */
  SOC_PB_PP_LEM_ACCESS_CMD_ACK = 5,
  /*
   *  Transplant an entry
   */
  SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT = 6,
  /*
   *  Error on a request
   */
  SOC_PB_PP_LEM_ACCESS_CMD_ERROR = 7,
  /*
   *  Number of request commands
   */
  SOC_PB_PP_LEM_ACCESS_NOF_CMDS = 8
}SOC_PB_PP_LEM_ACCESS_CMD;

typedef enum
{
  /*
   *  CPU
   */
  SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU = 0,
  /*
   *  OLP
   */
  SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OLP = 1,
  /*
   *  Number of request origins
   */
  SOC_PB_PP_LEM_ACCCESS_NOF_REQ_ORIGINS = 2
}SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN;

typedef enum
{
  /*
   *  MAC address (FID + MAC)
   */
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC = 0,
  /*
   *  Backbone MAC (B-FID + B-MAC)
   */
  SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC = 1,
  /*
   *  IPv4 Compatible Multicast
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC = 2,
  /*
   *  IPv4 Compatible Multicast
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST = 3,
  /*
   *  SA authentication (SA-MAC)
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH = 4,
   /*
   *  ILM
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM = 5,
  /*
   *  extended p2p
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED = 6,
  /*
   *  TRILL UC
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC = 7,
   /*
   *  TRILL MC
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC = 8,
  /*
   *  Number of key types
   */
   SOC_PB_PP_LEM_ACCESS_NOF_KEY_TYPES = 9
}SOC_PB_PP_LEM_ACCESS_KEY_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Contents (span exactly 4 registers)
   */
  uint32 data[4];
} SOC_PB_PP_LEM_ACCESS_BUFFER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *	FID
   */
   uint32 fid;

  /*
   *	MAC address
   */
  SOC_SAND_PP_MAC_ADDRESS  mac;

} SOC_PB_PP_LEM_ACCESS_KEY_MAC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

 /*
  *	FID
  */
  uint32 bfid;

  /*
   *	MAC address
   */
  SOC_SAND_PP_MAC_ADDRESS  bmac;

} SOC_PB_PP_LEM_ACCESS_KEY_BMAC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

 /*
  *	FID
  */
  uint32 fid;

  /*
   *	DIP
   */
  uint32 dip;

} SOC_PB_PP_LEM_ACCESS_KEY_IPV4_MC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

 /*
  *  Value
  */
  uint32 value[SOC_PB_PP_LEM_KEY_PARAM_MAX_IN_UINT32S];

 /*
  *	Number of significant bits in the value
  */
  uint8 nof_bits;

} SOC_PB_PP_LEM_ACCESS_KEY_PARAM;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

 /*
  *  Value
  */
  uint32 value;

 /*
  *	Number of significant bits in the value
  */
  uint8 nof_bits;

} SOC_PB_PP_LEM_ACCESS_KEY_PREFIX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Type
   */
   SOC_PB_PP_LEM_ACCESS_KEY_TYPE type;

  /*
   *	Parameters of the key: at the most 5.
   *  For MAC addresses: param[0] is MAC (48b), param[1] is FID (14b)
   *  For Backbone MAC addresses: param[0] is B-MAC (48b), param[1] is B-FID (12b)
   *  For IPv4 Compatible Multicast addresses: param[0] is DIP (28b), param[1] is FID (14b)
   *  For IP Host addresses: param[0] is DIP (32b), param[1] is VRF (8b)
   *  For SA AUTH addresses: param[0] is SA-MAC (48b)
   *  For ILM: param[0] is DIP (32b), param[1] is VRF (8b)
   */
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM param[SOC_PB_PP_LEM_KEY_MAX_NOF_PARAMS];

  /*
   *	Number of effective parameters (an additional verification is done on the nof_bits for each parameter)
   *  For MAC addresses: 2
   *  For Backbone MAC addresses: 2
   *  For IPv4 Compatible Multicast addresses: 2
   *  For IP Host addresses: 2
   *  For SA AUTH: 1
   */
  uint8 nof_params;

  /*
   *	Prefix of the key
   *  For MAC addresses: '1'
   *  For Backbone MAC addresses: '000'
   *  For IPv4 Compatible Multicast addresses: '0010'
   *  For IPv6 Compatible Multicast addresses: '0011'
   *  For TP2PS addresses: '0100'
   *  For IPHT addresses: '0101'
   *  For ILMT addresses: '0110'
   *  For Trill (Sabva, Sabeep) addresses: '0111'
   */
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX prefix;

} SOC_PB_PP_LEM_ACCESS_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Key for the request
   */
   SOC_PB_PP_LEM_ACCESS_KEY key;

  /*
   *	Request command
   */
  SOC_PB_PP_LEM_ACCESS_CMD command;

  /*
   *	Request stamp (for the ACK).
   *  Configured internally.
   */
  uint32 stamp;

} SOC_PB_PP_LEM_ACCESS_REQUEST;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Destination buffer
   */
   uint32 dest;

  /*
   *	Application specific data
   */
  uint32 asd;

  /*
   *	If True, then the entry is marked as dynamic (otherwise static)
   */
  uint8 is_dynamic;

  /*
   *	Entry age
   */
  uint32 age;

} SOC_PB_PP_LEM_ACCESS_PAYLOAD;

typedef enum
{
  /*
   *  FID limit
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT = 0,
  /*
   *  MACT Full
   */
   SOC_PB_PP_LEM_ACCESS_FAIL_REASON_MACT_FULL,
  /*
   *  CAM Full
   */
   SOC_PB_PP_LEM_ACCESS_FAIL_REASON_CAM_FULL,
  /*
   *  Delete unknown key
   */
   SOC_PB_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN,
  /*
   *  Wrong stamp - not sure the reply corresponds to the
   *  original request. Not supposed to happen.
   */
   SOC_PB_PP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP,
  /*
   *  Request not sent due to no open slots for new CPU requests
   *  (the polling failed) - for the lookup, the polling has not finished
   */
   SOC_PB_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT,
  /*
   *  Unknown FID
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN,
  /*
   *  Request of type learn for a static entry
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC,
  /*
   *  Change a static entry
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC,
  /*
   *  EMC problem, should probe further to find the cause
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM,
  /*
   *  Unknown Reason
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN,
 /*
   *  Number of fail reasons
   */
   SOC_PB_PP_LEM_ACCESS_NOF_FAIL_REASONS
}SOC_PB_PP_LEM_ACCESS_FAIL_REASON;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Success for the request operation
   */
   uint8 is_success;

  /*
   *	If failure for 'is_success', describe the reason
   */
  SOC_PB_PP_LEM_ACCESS_FAIL_REASON reason;

} SOC_PB_PP_LEM_ACCESS_ACK_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  The original request
   */
   SOC_PB_PP_LEM_ACCESS_REQUEST request;

  /*
   * Is part of LAG
   */
  uint8 is_part_of_lag;

  /*
   *  The request stamp
   */
  uint32 stamp;

  /*
   *	The payload
   */
  SOC_PB_PP_LEM_ACCESS_PAYLOAD payload;

  /*
   *	Origin of the entry insertion: if True, this device, otherwise
   *  another device learned it.
   */
  uint8 is_learned_first_by_me;

  /*
   *	Request origin (OLP or CPU)
   */
  SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN req_origin;

} SOC_PB_PP_LEM_ACCESS_OUTPUT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
  *	Encoding of the key in bits
  */
  uint32 buffer[SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S];

} SOC_PB_PP_LEM_ACCESS_KEY_ENCODED;


typedef struct
{
  uint32 or_value;
  uint32 and_value;
  uint32 valid;
}SOC_PB_PP_LEM_ACCESS_MASK_INFO;

typedef struct
{
  uint8 valid;
  uint32 prog_idx;
  uint32 data_processing_profile;
  SOC_PB_PP_LEM_ACCESS_MASK_INFO tcam;
  uint32 tcam_key_size;
  SOC_PB_PP_LEM_ACCESS_MASK_INFO lpm_2nd_lkp;
  SOC_PB_PP_LEM_ACCESS_MASK_INFO lpm_1st_lkp;
  SOC_PB_PP_LEM_ACCESS_MASK_INFO lem_2nd_lkp;
  uint32 lem_2nd_lkp_key_select;
  SOC_PB_PP_LEM_ACCESS_MASK_INFO lem_1st_lkp;
  uint32 lem_1st_lkp_key_type;
  uint32 lem_1st_lkp_key_select;
  uint32 elk_lkp_valid;
  SOC_PB_PP_CE_INSTRUCTION key_b_instruction3;
  SOC_PB_PP_CE_INSTRUCTION key_b_instruction2;
  SOC_PB_PP_CE_INSTRUCTION key_b_instruction1;
  SOC_PB_PP_CE_INSTRUCTION key_b_instruction0;
  SOC_PB_PP_CE_INSTRUCTION key_a_instruction1;
  SOC_PB_PP_CE_INSTRUCTION key_a_instruction0;
}SOC_PB_PP_LEM_ACCESS_INFO;

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
  soc_pb_pp_lem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*
 *	Conversion functions for key construction / parsing
 */
uint32
  soc_pb_pp_lem_key_encoded_parse(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer,
     SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY             *key
  );

uint32
  soc_pb_pp_lem_key_encoded_build(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY             *key,
     SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer
  );

uint32
  soc_pb_pp_lem_access_parse(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_BUFFER     *buffer,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_OUTPUT     *request_all,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS *ack_status
  );

uint32
  soc_pb_pp_lem_access_payload_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD   *payload,
    SOC_SAND_OUT uint32                    payload_data[SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S]
  );

uint32
  soc_pb_pp_lem_access_payload_parse(
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   uint32                    payload_data[SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT  SOC_PB_PP_LEM_ACCESS_PAYLOAD   *payload
   );
/*********************************************************************
 *     Add an entry to the Exact match table.
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_add_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS      *ack_status
  );

/*********************************************************************
 *     Remove an entry to the Exact match table.
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS      *ack_status
  );

/*********************************************************************
 *     Get an entry in the Exact match table according to its key
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_by_key_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT uint8                        *is_found
  );

/*********************************************************************
 *     Get an entry in the Exact match table according to its index
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_by_index_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        entry_ndx,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT uint8                        *is_valid
  );

/*********************************************************************
 *     parse/build ASD field for SA-Atuh usages
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_sa_based_asd_build(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_SA_AUTH_MAC_INFO      *auth_info,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO   *vid_assign_info,
    SOC_SAND_OUT uint32                        *asd
  );

/*********************************************************************
 *     parse/build ASD field for Trill-SA-Atuh usages
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_trill_sa_based_asd_build(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO              *auth_info,
    SOC_SAND_OUT uint32                          *asd
  );

/* assumed key already cleared */
uint32
  soc_pb_pp_lem_access_sa_based_asd_parse(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        asd,
    SOC_SAND_OUT SOC_PB_PP_LLP_SA_AUTH_MAC_INFO      *auth_info,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO   *vid_assign_info
  );

uint32
  soc_pb_pp_lem_access_trill_sa_based_asd_parse(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        asd,
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO            *auth_info
  );

/*********************************************************************
 *     Set/get valid TCAM banks
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_ipv6_bank_bitmap_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER                    type,
    SOC_SAND_IN  uint32                        bitmap
  );

uint32
  soc_pb_pp_lem_access_ipv6_bank_bitmap_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER                    type,
    SOC_SAND_OUT uint32                        *bitmap
  );

uint32
  soc_pb_pp_lem_access_ipv4_mc_bank_bitmap_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  bitmap
  );

/*********************************************************************
 *     Set/get TCAM database prefixes
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_ipv6_tcam_prefix_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN SOC_PB_TCAM_USER type,
    SOC_SAND_IN uint32     prefix,
    SOC_SAND_IN uint32    nof_bits_in_prefix
  );

uint32
  soc_pb_pp_lem_access_ipv6_tcam_prefix_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER type,
    SOC_SAND_OUT uint32     *prefix,
    SOC_SAND_OUT uint32    *nof_bits_in_prefix
  );

uint32
  soc_pb_pp_lem_access_ipv4_mc_tcam_prefix_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  prefix,
    SOC_SAND_IN uint32 nof_bits_in_prefix
  );

uint32
  soc_pb_pp_lem_ilm_key_build_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_INFO       *glbl_info
  );

uint32
  soc_pb_pp_lem_trill_mc_key_build_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint8           mask_adjacent_nickname,
    SOC_SAND_IN  uint8           mask_fid,
    SOC_SAND_IN  uint8           mask_adjacent_eep
  );

uint32
  soc_pb_pp_lem_trill_mc_key_build_get(
    SOC_SAND_IN   int           unit,
    SOC_SAND_OUT  uint8           *mask_adjacent_nickname,
    SOC_SAND_OUT  uint8           *mask_fid,
    SOC_SAND_OUT  uint8           *mask_adjacent_eep
  );

/*
 *	Clear functions
 */
void
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_ENCODED *info
  );

void
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_PARAM *info
  );

void
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_PREFIX *info
  );

void
  SOC_PB_PP_LEM_ACCESS_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY *info
  );

void
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_REQUEST *info
  );

void
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD *info
  );

void
  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_OUTPUT  *info
  );

void
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS  *info
  );

void
  SOC_PB_PP_LEM_ACCESS_BUFFER_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_BUFFER *info
  );

uint32
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_verify(
    SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_TYPE   type,
    SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_PARAM *info,
    SOC_SAND_IN uint32                   params_ndx
  );

uint32
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_verify(
    SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_TYPE type,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX *info
  );

uint32
  SOC_PB_PP_LEM_ACCESS_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY *info
  );

uint32
  SOC_PB_PP_LEM_ACCESS_REQUEST_verify(
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_REQUEST *info
  );

uint32
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_verify(
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_lem_access_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_lem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lem_access_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_lem_access_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_lem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lem_access_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LEM_ACCESS_INCLUDED__*/
#endif
