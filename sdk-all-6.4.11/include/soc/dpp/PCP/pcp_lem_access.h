/* $Id: pcp_lem_access.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PCP_LEM_ACCESS_INCLUDED__
/* { */
#define __SOC_PCP_LEM_ACCESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/PCP/pcp_api_frwrd_mact.h>
#include <soc/dpp/PCP/pcp_api_frwrd_ilm.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* 
 *  encoding:
 *    21-20: table coding. h0=0x0, h1=0x1, cam=0x2
 *    19-0: table entry offset
 *  current PCP_LEM_NOF_ENTRIES=(2*1024*1024+32) will search cam + h1 + h0:
 *    21-20: 0x2 (cam encode)
 *    19-0: 32   (cam max size)
 */
#define PCP_LEM_NOF_ENTRIES                             (2*1024*1024+32)

#define PCP_LEM_KEY_PARAM_MAX_IN_UINT32S                  (3)
#define PCP_LEM_KEY_MAX_NOF_PARAMS                      (5)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_EXTENDED      (1)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC           (2)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_BACKBONE_MAC  (2)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC       (2)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IP_HOST       (2)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH       (1)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM           (4)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC      (1)
#define PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC      (5)


/*
 *	Prefix characteristic: value and number of significant msb bits.
 *  For example, Backbone MAC keys have a '000' in their msbs:
 *  value is 0 ('000'), size (n bits) is 3. For MAC addresses, fix a number of bits of 1
 *  and a key prefix of 0
 */
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_MAC                  (1)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC     (1)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC                (0)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC   (3)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC              (2)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST              (5)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH              (7)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_ILM                  (6)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED             (3)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM     (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED     (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC              (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC (4)
#define PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC              (0)
#define PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC (4)


#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC          (48) 
#define PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC          (14) /* Consider FID and not System-VSI */
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC (48)
#define PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC (12)
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC      (28)
#define PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC      (14)
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST      (32)
#define PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST      (8)
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SA_AUTH      (48)
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM          (20)
#define PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM          (3)
#define PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM          (6)
#define PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM          (12)
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC     (16)
#define PCP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC     (16)
#define PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC     (16)
#define PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC     (12)
#define PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC     (13)
#define PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC     (1)



#define PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS   (63)
#define PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S  (PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS / SOC_SAND_NOF_BITS_IN_UINT32 + 1)

#define PCP_LEM_ACCESS_ASD_NOF_BITS           (24)
#define PCP_LEM_ACCESS_DEST_NOF_BITS          (16)
#define PCP_LEM_ACCESS_PAYLOAD_NOF_BITS       (PCP_LEM_ACCESS_ASD_NOF_BITS + PCP_LEM_ACCESS_DEST_NOF_BITS + 1)
#define PCP_LEM_ACCESS_PAYLOAD_IN_UINT32S       (PCP_LEM_ACCESS_PAYLOAD_NOF_BITS / SOC_SAND_NOF_BITS_IN_UINT32 + 1)

/*
 *	Payload composition
 */
#define PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S                  (2)
#define PCP_LEM_ACCESS_ASD_FIRST_REG_LSB                 (SOC_SAND_REG_SIZE_BITS - PCP_LEM_ACCESS_DEST_NOF_BITS)
#define PCP_LEM_ACCESS_ASD_FIRST_REG_MSB                 (SOC_SAND_REG_MAX_BIT)
#define PCP_LEM_ACCESS_ASD_FIRST_REG_NOF_BITS            \
  (PCP_LEM_ACCESS_ASD_FIRST_REG_MSB - PCP_LEM_ACCESS_ASD_FIRST_REG_LSB + 1)
#define PCP_LEM_ACCESS_ASD_SCND_REG_LSB                  (0)
#define PCP_LEM_ACCESS_ASD_SCND_REG_MSB                   \
  (PCP_LEM_ACCESS_ASD_NOF_BITS - PCP_LEM_ACCESS_ASD_FIRST_REG_NOF_BITS - 1)
#define PCP_LEM_ACCESS_IS_DYN_LSB                         (PCP_LEM_ACCESS_ASD_SCND_REG_MSB + 1)

/*
 *	Field values
 */
#define PCP_LEM_ACCESS_CMD_DELETE_FLD_VAL      0
#define PCP_LEM_ACCESS_CMD_INSERT_FLD_VAL      1
#define PCP_LEM_ACCESS_CMD_REFRESH_FLD_VAL     2
#define PCP_LEM_ACCESS_CMD_LEARN_FLD_VAL       3
#define PCP_LEM_ACCESS_CMD_DEFRAG_FLD_VAL      4
#define PCP_LEM_ACCESS_CMD_ACK_FLD_VAL         5
#define PCP_LEM_ACCESS_CMD_TRANSPLANT_FLD_VAL  6
#define PCP_LEM_ACCESS_CMD_ERROR_FLD_VAL       7


/*
* ASD formatting for different applications
*/

/*
 * ASD formatting for TRILL-SA-authentication
 */




/*
 * ASD formatting for SA-authentication
 */







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
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0000 = 0, /* TRILL MC */
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0001 = 1, /*b-mac*/
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0010 = 2, /* ipv4-mc comp*/
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0011 = 3,  /* extended */
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0100 = 4, /* TRILL UC */
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0101 = 5, /*IP-host*/
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0110 = 6, /*ILM */
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0111 = 7,/* SA-AUTH*/
  PCP_LEM_ACCESS_KEY_TYPE_PREFIX_1    = 8
}PCP_LEM_ACCESS_KEY_TYPE_PREFIX;



typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_LEM_ACCESS_GET_PROCS_PTR = PCP_PROC_DESC_BASE_LEM_ACCESS_FIRST,
  PCP_LEM_ACCESS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_LEM_KEY_ENCODED_BUILD,
  PCP_LEM_ACCESS_PAYLOAD_BUILD,
  PCP_LEM_KEY_ENCODED_PARSE,
  PCP_LEM_ACCESS_PARSE,
  PCP_LEM_REQUEST_SEND,
  PCP_LEM_REQUEST_ANSWER_RECEIVE,
  PCP_LEM_ACCESS_ENTRY_ADD_UNSAFE,
  PCP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE,
  PCP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE,
  PCP_LEM_ACCESS_ENTRY_BY_INDEX_GET_UNSAFE,
  PCP_LEM_ACCESS_AGE_FLD_SET,
  PCP_LEM_ACCESS_AGE_FLD_GET,
  PCP_LEM_ACCESS_SA_BASED_ASD_BUILD,
  PCP_LEM_ACCESS_SA_BASED_ASD_PARSE,
  PCP_LEM_ILM_KEY_BUILD_SET,
  PCP_LEM_ACCESS_FLP_PROGRAM_MAP_ENTRY_GET,
  PCP_LEM_ACCESS_TCAM_BANK_BITMAP_SET,
  PCP_LEM_ACCESS_TCAM_BANK_BITMAP_GET,
  PCP_LEM_ACCESS_IPV6_BANK_BITMAP_SET,
  PCP_LEM_ACCESS_IPV6_BANK_BITMAP_GET,
  PCP_LEM_ACCESS_TCAM_PREFIX_SET,
  PCP_LEM_ACCESS_TCAM_PREFIX_GET,
  PCP_LEM_ACCESS_IPV6_TCAM_PREFIX_SET,
  PCP_LEM_ACCESS_IPV6_TCAM_PREFIX_GET,

  /*
   * Last element. Do no touch.
   */
  PCP_LEM_ACCESS_PROCEDURE_DESC_LAST
} PCP_LEM_ACCESS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_LEM_ACCESS_FIRST,
  PCP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
  PCP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR,
  PCP_LEM_STAMP_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR,
  PCP_LEM_ASD_OUT_OF_RANGE_ERR,
  PCP_LEM_AGE_OUT_OF_RANGE_ERR,
  PCP_LEM_DEST_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR,
  PCP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR,
  PCP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR,
  PCP_LEM_ACCESS_PROGRAM_NOT_FOUND,
  PCP_LEM_ACCESS_PREFIX_TOO_LONG,
  PCP_LEM_ACCESS_MALFORMED_PREFIX,
  PCP_LEM_ACCESS_KEYS_DONT_MATCH,

  /*
   * Last element. Do no touch.
   */
  PCP_LEM_ACCESS_ERR_LAST
} PCP_LEM_ACCESS_ERR;

typedef enum
{
  /*
   *  Delete an entry from the LEM DB.
   */
  PCP_LEM_ACCESS_CMD_DELETE = 0,
  /*
   *  Insert an entry to the DB
   */
  PCP_LEM_ACCESS_CMD_INSERT = 1,
  /*
   *  Refresh an entry
   */
   PCP_LEM_ACCESS_CMD_REFRESH = 2,
  /*
   *  Simulate a learned entry
   */
  PCP_LEM_ACCESS_CMD_LEARN = 3,
  /*
   *  Defrag command (i.e., transfer entries from CAM to LEM DB)
   */
  PCP_LEM_ACCESS_CMD_DEFRAG = 4,
  /*
   *  ACK to receive an Ack
   */
  PCP_LEM_ACCESS_CMD_ACK = 5,
  /*
   *  Transplant an entry
   */
  PCP_LEM_ACCESS_CMD_TRANSPLANT = 6,
  /*
   *  Error on a request
   */
  PCP_LEM_ACCESS_CMD_ERROR = 7,
  /*
   *  Number of request commands
   */
  PCP_LEM_ACCESS_NOF_CMDS = 8
}PCP_LEM_ACCESS_CMD;

typedef enum
{
  /*
   *  CPU
   */
  PCP_LEM_ACCCESS_REQ_ORIGIN_CPU = 0,
  /*
   *  OLP
   */
  PCP_LEM_ACCCESS_REQ_ORIGIN_OLP = 1,
  /*
   *  Number of request origins
   */
  PCP_LEM_ACCCESS_NOF_REQ_ORIGINS = 2
}PCP_LEM_ACCCESS_REQ_ORIGIN;

typedef enum
{
  /*
   *  MAC address (FID + MAC)
   */
  PCP_LEM_ACCESS_KEY_TYPE_MAC = 0,
  /*
   *  Backbone MAC (B-FID + B-MAC)
   */
  PCP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC = 1,
  /*
   *  IPv4 Compatible Multicast
   */
   PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC = 2,
  /*
   *  IPv4 Compatible Multicast
   */
   PCP_LEM_ACCESS_KEY_TYPE_IP_HOST = 3,
  /*
   *  SA authentication (SA-MAC)
   */
   PCP_LEM_ACCESS_KEY_TYPE_SA_AUTH = 4,
   /*
   *  ILM
   */
   PCP_LEM_ACCESS_KEY_TYPE_ILM = 5,
  /*
   *  extended p2p
   */
   PCP_LEM_ACCESS_KEY_TYPE_EXTENDED = 6,
  /*
   *  TRILL UC
   */
   PCP_LEM_ACCESS_KEY_TYPE_TRILL_UC = 7,
   /*
   *  TRILL MC
   */
   PCP_LEM_ACCESS_KEY_TYPE_TRILL_MC = 8,
  /*
   *  Number of key types
   */
   PCP_LEM_ACCESS_NOF_KEY_TYPES = 9
}PCP_LEM_ACCESS_KEY_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Contents (span exactly 4 registers)
   */
  uint32 data[4];
} PCP_LEM_ACCESS_BUFFER;

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

} PCP_LEM_ACCESS_KEY_MAC;

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

} PCP_LEM_ACCESS_KEY_BMAC;

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

} PCP_LEM_ACCESS_KEY_IPV4_MC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

 /*
  *  Value
  */
  uint32 value[PCP_LEM_KEY_PARAM_MAX_IN_UINT32S];

 /*
  *	Number of significant bits in the value
  */
  uint8 nof_bits;

} PCP_LEM_ACCESS_KEY_PARAM;

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

} PCP_LEM_ACCESS_KEY_PREFIX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Type
   */
   PCP_LEM_ACCESS_KEY_TYPE type;

  /*
   *	Parameters of the key: at the most 5.
   *  For MAC addresses: param[0] is MAC (48b), param[1] is FID (14b)
   *  For Backbone MAC addresses: param[0] is B-MAC (48b), param[1] is B-FID (12b)
   *  For IPv4 Compatible Multicast addresses: param[0] is DIP (28b), param[1] is FID (14b)
   *  For IP Host addresses: param[0] is DIP (32b), param[1] is VRF (8b)
   *  For SA AUTH addresses: param[0] is SA-MAC (48b)
   *  For ILM: param[0] is DIP (32b), param[1] is VRF (8b)
   */
  PCP_LEM_ACCESS_KEY_PARAM param[PCP_LEM_KEY_MAX_NOF_PARAMS];

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
  PCP_LEM_ACCESS_KEY_PREFIX prefix;

} PCP_LEM_ACCESS_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  Key for the request
   */
   PCP_LEM_ACCESS_KEY key;

  /*
   *	Request command
   */
  PCP_LEM_ACCESS_CMD command;

  /*
   *	Request stamp (for the ACK).
   *  Configured internally.
   */
  uint32 stamp;

} PCP_LEM_ACCESS_REQUEST;

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

} PCP_LEM_ACCESS_PAYLOAD;

typedef enum
{
  /*
   *  FID limit
   */
  PCP_LEM_ACCESS_FAIL_REASON_FID_LIMIT = 0,
  /*
   *  MACT Full
   */
   PCP_LEM_ACCESS_FAIL_REASON_MACT_FULL,
  /*
   *  CAM Full
   */
   PCP_LEM_ACCESS_FAIL_REASON_CAM_FULL,
  /*
   *  Delete unknown key
   */
   PCP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN,
  /*
   *  Wrong stamp - not sure the reply corresponds to the
   *  original request. Not supposed to happen.
   */
   PCP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP,
  /*
   *  Request not sent due to no open slots for new CPU requests
   *  (the polling failed) - for the lookup, the polling has not finished
   */
   PCP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT,
  /*
   *  Unknown FID
   */
  PCP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN,
  /*
   *  Request of type learn for a static entry
   */
  PCP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC,
  /*
   *  Change a static entry
   */
  PCP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC,
  /*
   *  EMC problem, should probe further to find the cause
   */
  PCP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM,
  /*
   *  Unknown Reason
   */
  PCP_LEM_ACCESS_FAIL_REASON_UNKNOWN,
 /*
   *  Number of fail reasons
   */
   PCP_LEM_ACCESS_NOF_FAIL_REASONS
}PCP_LEM_ACCESS_FAIL_REASON;

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
  PCP_LEM_ACCESS_FAIL_REASON reason;

} PCP_LEM_ACCESS_ACK_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  The original request
   */
   PCP_LEM_ACCESS_REQUEST request;

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
  PCP_LEM_ACCESS_PAYLOAD payload;

  /*
   *	Origin of the entry insertion: if True, this device, otherwise
   *  another device learned it.
   */
  uint8 is_learned_first_by_me;

  /*
   *	Request origin (OLP or CPU)
   */
  PCP_LEM_ACCCESS_REQ_ORIGIN req_origin;

} PCP_LEM_ACCESS_OUTPUT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
  *	Encoding of the key in bits
  */
  uint32 buffer[PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S];

} PCP_LEM_ACCESS_KEY_ENCODED;


typedef struct
{
  uint32 or_value;
  uint32 and_value;
  uint32 valid;
}PCP_LEM_ACCESS_MASK_INFO;

typedef struct
{
  uint8 valid;
  uint32 prog_idx;
  uint32 data_processing_profile;
  PCP_LEM_ACCESS_MASK_INFO tcam;
  uint32 tcam_key_size;
  PCP_LEM_ACCESS_MASK_INFO lpm_2nd_lkp;
  PCP_LEM_ACCESS_MASK_INFO lpm_1st_lkp;
  PCP_LEM_ACCESS_MASK_INFO lem_2nd_lkp;
  uint32 lem_2nd_lkp_key_select;
  PCP_LEM_ACCESS_MASK_INFO lem_1st_lkp;
  uint32 lem_1st_lkp_key_type;
  uint32 lem_1st_lkp_key_select;
  uint32 elk_lkp_valid;
}PCP_LEM_ACCESS_INFO;


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
  pcp_lem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*
 *	Conversion functions for key construction / parsing
 */
uint32
  pcp_lem_key_encoded_parse(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  PCP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer,
     SOC_SAND_OUT PCP_LEM_ACCESS_KEY             *key
  );

uint32
  pcp_lem_key_encoded_build(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  PCP_LEM_ACCESS_KEY             *key,
     SOC_SAND_OUT PCP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer
  );

uint32
  pcp_lem_access_parse(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_BUFFER     *buffer,
    SOC_SAND_OUT PCP_LEM_ACCESS_OUTPUT     *request_all,
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS *ack_status
  );

uint32
  pcp_lem_access_payload_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD   *payload,
    SOC_SAND_OUT uint32                    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S]
  );

uint32
  pcp_lem_access_payload_parse(
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   uint32                    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT  PCP_LEM_ACCESS_PAYLOAD   *payload
   );
/*********************************************************************
 *     Add an entry to the Exact match table.
 *********************************************************************/
uint32
  pcp_lem_access_entry_add_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS      *ack_status
  );

/*********************************************************************
 *     Remove an entry to the Exact match table.
 *********************************************************************/
uint32
  pcp_lem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS      *ack_status
  );

/*********************************************************************
 *     Get an entry in the Exact match table according to its key
 *********************************************************************/
uint32
  pcp_lem_access_entry_by_key_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT uint8                        *is_found
  );

/*********************************************************************
 *     Get an entry in the Exact match table according to its index
 *********************************************************************/
uint32
  pcp_lem_access_entry_by_index_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        entry_ndx,
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT uint8                        *is_valid
  );

/*
 *	Clear functions
 */
void
  PCP_LEM_ACCESS_KEY_ENCODED_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY_ENCODED *info
  );

void
  PCP_LEM_ACCESS_KEY_PARAM_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY_PARAM *info
  );

void
  PCP_LEM_ACCESS_KEY_PREFIX_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY_PREFIX *info
  );

void
  PCP_LEM_ACCESS_KEY_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY *info
  );

void
  PCP_LEM_ACCESS_REQUEST_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_REQUEST *info
  );

void
  PCP_LEM_ACCESS_PAYLOAD_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD *info
  );

void
  PCP_LEM_ACCESS_OUTPUT_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_OUTPUT  *info
  );

void
  PCP_LEM_ACCESS_ACK_STATUS_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS  *info
  );

void
  PCP_LEM_ACCESS_BUFFER_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_BUFFER *info
  );

uint32
  PCP_LEM_ACCESS_KEY_PARAM_verify(
    SOC_SAND_IN PCP_LEM_ACCESS_KEY_TYPE   type,
    SOC_SAND_IN PCP_LEM_ACCESS_KEY_PARAM *info,
    SOC_SAND_IN uint32                   params_ndx
  );

uint32
  PCP_LEM_ACCESS_KEY_PREFIX_verify(
    SOC_SAND_IN PCP_LEM_ACCESS_KEY_TYPE type,
    SOC_SAND_IN  PCP_LEM_ACCESS_KEY_PREFIX *info
  );

uint32
  PCP_LEM_ACCESS_KEY_verify(
    SOC_SAND_IN  PCP_LEM_ACCESS_KEY *info
  );

uint32
  PCP_LEM_ACCESS_REQUEST_verify(
    SOC_SAND_IN  PCP_LEM_ACCESS_REQUEST *info
  );

uint32
  PCP_LEM_ACCESS_PAYLOAD_verify(
    SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD *info
  );

/*********************************************************************
* NAME:
 *   pcp_lem_access_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_api_lem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_lem_access_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_lem_access_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_api_lem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_lem_access_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_LEM_ACCESS_INCLUDED__*/
#endif
