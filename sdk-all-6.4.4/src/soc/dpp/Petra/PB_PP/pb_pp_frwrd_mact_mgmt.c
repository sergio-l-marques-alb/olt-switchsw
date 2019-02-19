/* $Id: pb_pp_frwrd_mact_mgmt.c,v 1.27 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_packet.h>
#include <soc/dpp/Petra/petra_header_parsing_utils.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN              (0)
#define SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MAX              (6)
#define SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX        (7)
#define SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_MAX         (1)
#define SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_MAX                (SOC_PB_PP_NOF_FRWRD_MACT_TRAP_TYPES-1)
#define SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_MAX             (3)
#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_MAX                   (SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REQ_FAIL)
#define SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_MAX                  (SOC_PB_PP_NOF_FRWRD_MACT_MSG_HDR_TYPES-1)
#define SOC_PB_PP_FRWRD_MACT_TYPE_MAX                         (SOC_PB_PP_NOF_FRWRD_MACT_EVENT_PATH_TYPES-1)
#define SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_MAX                (SOC_PB_PP_NOF_FRWRD_MACT_LEARNING_MODES-1)
#define SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_MAX                  (SOC_PB_PP_NOF_FRWRD_MACT_SHADOW_MODES-1)
#define SOC_PB_PP_FRWRD_MACT_NOF_ENTRIES_MAX                  (0x7FFE)
#define SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_MAX           (SOC_PB_PP_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES-1)
#define SOC_PB_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_MAX       (3)
#define SOC_PB_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_MAX    (3)
#define SOC_PB_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_MAX    (3)
#define SOC_PB_PP_FRWRD_MACT_ID_MAX                           (255)

#define SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_MAX                         (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_LEN_MAX                     (SOC_SAND_U32_MAX)

#define SOC_PB_PP_FRWRD_MACT_FID_FAIL_MAX                     (0x7ffe)
   /* limit check is loose check, actual limit will be check according to set function */
#define SOC_PB_PP_FRWRD_MACT_TIME_SEC_MAX                               (1000)

#define SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_ENTRY_UNLIMITED          (0x7FFF)

#define SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS                          (2)

#define SOC_PB_PP_FRWRD_MACT_16_CLOCKS_RESOLUTION                            (16)
#define SOC_PB_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC                       (0x1)
#define SOC_PB_PP_FRWRD_MACT_AGE_NOF_BITS                               (3)
#define SOC_PB_PP_FRWRD_MACT_NOF_AGE_KEYS                                    (2)

#define SOC_PB_PP_FRWRD_MACT_LEM_DB_MACT_ID                     (1)

#define SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_MAC_IN_MAC_FLD_VAL               (0x0)
#define SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_AUTHENTIFICATION_FLD_VAL         (0x1)
#define SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SA_LOOKUP_FLD_VAL                (0x2)

#define SOC_PB_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_FLD_VAL                  (0x2)
#define SOC_PB_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_AND_REFRESH_FLD_VAL      (0x3)

#define SOC_PB_PP_FRWRD_MACT_FIFO_FULL_THRESHOLD                             (127)

#define SOC_PB_PP_FRWRD_MACT_ALL_FORWARDING_CODE_ENABLE                 (0xFFFF)

#define SOC_PB_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE                                   (8)
/*
 * Ethernet Type (In Bytes)
 */
#define SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE                                    (2)
/*
 * Ethernet Header Size (In Bytes)
 */
#define SOC_PB_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE                                  (14)
/*
 * ITMH Header Size (In Bytes)
 */
#define SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE                                      (4)
#define SOC_PB_PP_FRWRD_MACT_OTMH_HEADER_SIZE                                      (2)
/*
 * HW DSP Header buffer size in uint32s
 */
#define SOC_PB_PP_FRWRD_MACT_DSP_HEADER_SIZE                                       (8)
#define SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_DISABLED                  (0)
#define SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_SINGLE                    (1)
#define SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_AGGR                      (8)
#define SOC_PB_PP_FRWRD_MACT_DSP_MSG_MIN_SIZE                                      (64)
/*
 * This type to verify the packet was received by the OLP
 * is DSP packet. It cannot conflict with normal
 * traffic.
 */
#define  SOC_PB_PP_FRWRD_MACT_MESSAGE_DSP_TYPE                                      (0XAB00)

#define  SOC_PB_PP_FRWRD_MACT_LEARN_FILTER_TTL_LOOPBACK                       (12)
#define  SOC_PB_PP_FRWRD_MACT_LEARN_FILTER_TTL_MSGS                           (48)

/*
 * default profile for MAC limit, used to set no limit
 */
#define  SOC_PB_PP_FRWRD_MACT_LEARN_LIMIT_DFLT_PROFILE_ID (0)



#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EVENT_LEN        16
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_NUM_OF_EVENTS    8
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EXT_LEN          2
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAX_LEN         ((SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EVENT_LEN + SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EXT_LEN) * SOC_PB_PP_FRWRD_MACT_LEARN_MSG_NUM_OF_EVENTS + SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE) 

#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_VALID_INDEX    0
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_VALID_LEN      1
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_TYPE_INDEX     1
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_TYPE_LEN       3
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FAP_INDEX      21
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FAP_LEN        11
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FID_INDEX      35
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FID_LEN        12
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAC_INDEX      47 
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAC_LEN        48
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_FID_INDEX   67
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_FID_LEN     14
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_DIP_INDEX   95
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_DIP_LEN     28
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_1_INDEX      128
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_1_LEN        16
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_2_INDEX      5
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_2_LEN        7
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_EXT_INDEX     96
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_EXT_LEN       1
#define SOC_PB_PP_FRWRD_MACT_LEARN_DEST_INDEX           100
#define SOC_PB_PP_FRWRD_MACT_LEARN_DEST_LEN             16
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_INDEX   20
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_LEN     1
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_LAG_INDEX          12
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_LAG_LEN            8
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_DYNAMIC_INDEX   116
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_DYNAMIC_LEN     1
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_SA_KNOWN_INDEX     120
#define SOC_PB_PP_FRWRD_MACT_LEARN_MSG_SA_KNOWN_LEN       1


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
  /*
  *  FIFO Learning
  */
  SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING = 0,
  /*
  *  FIFO Shadow
  */
  SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW = 1,
 /*
  *  Number of types in SOC_PB_PP_FRWRD_MACT_FIFO
  */
  SOC_PB_PP_FRWRD_MACT_NOF_FIFOS = 2
}SOC_PB_PP_FRWRD_MACT_FIFO;

typedef enum
{
 /*
  *  Send to loopback
  */
  SOC_PB_PP_FRWRD_MACT_POSITION_LOOPBACK = 0,
 /*
  *  Send to FIFO Learning
  */
  SOC_PB_PP_FRWRD_MACT_POSITION_LEARNING = 1,
 /*
  *  Send to FIFO Shadow
  */
  SOC_PB_PP_FRWRD_MACT_POSITION_SHADOW = 2,
 /*
  *  Number of types in SOC_PB_PP_FRWRD_MACT_POSITION
  */
  SOC_PB_PP_FRWRD_MACT_NOF_POSITIONS = 3
}SOC_PB_PP_FRWRD_MACT_POSITION;


/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_frwrd_mact_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MGMT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_MAX_AGE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGE_CONF_DEFAULT_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGE_CONF_WRITE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGE_MODIFY_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_AGE_CONF_READ),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_KEY_INDEX_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_REGS_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_IS_INGRESS_LEARNING_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_UNSAFE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_frwrd_mact_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'fid_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 6.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mac_learn_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'event_handle_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_TRAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_TRAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'event_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_EVENT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'vsi_event_handle_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'header_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_MSG_HDR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_EVENT_PATH_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'learning_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_LEARNING_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'shadow_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_SHADOW_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MGMT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MGMT_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - 0x7FFE.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR",
    "The parameter 'action_when_exceed' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'sa_drop_action_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'sa_unknown_action_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'da_unknown_action_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR",
    "The parameter 'id' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR",
    "The parameter 'sec' is out of range. \n\r "
    "The range is: 0 - 660.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR",
    "The parameter 'fid_base' is out of range. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR",
    "The parameter 'generate_event' is out of range. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR",
    "The parameter 'is_lag' is out of range. \n\r "
    "The range is: 0 - 0 for the ingress learning.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR,
    "SOC_PB_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR",
    "The parameter 'trap_code' is not valid (useless for this API). \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to 0xfffe.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR",
    "The parameter 'distribution_fifo' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MSG_LEN_ERR,
    "SOC_PB_PP_FRWRD_MACT_MSG_LEN_ERR",
    "learning message is too small (unexpected size). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR,
    "SOC_PB_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR",
    "message passed to soc_pb_pp_frwrd_mact_learn_msg_parse is not a learning message\n\r ",
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

STATIC uint32
  soc_pb_pp_frwrd_mact_event_type_to_event_val(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE                     event_type_ndx
  )
{

  switch(event_type_ndx)
  {
  case SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT:
    return 0;
  case SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN:
    return 3;
  case SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT:
    return 6;
  case SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REFRESH:
    return 2;
  case SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_ACK:
    return 5;
  case SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REQ_FAIL:
    return 7;
  default:
    return 1;
  }
}

/*
 *  Get the right register and field indexes of the
 *  DSP event table
 */

/*
 *	Get the maximal age before delete according to the
 *  device configuration
 */
STATIC uint32
  soc_pb_pp_frwrd_mact_max_age_default_get(
      SOC_SAND_IN  int  unit,
      SOC_SAND_OUT uint32  *max_age
    )
{
  uint32
    res;
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    is_high_res,
    pph_petra_a_compatible;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_MAX_AGE_GET);

  /*
   *	Get the device configuration
   */
  learning_mode = soc_pb_pp_sw_db_learning_mode_get(unit);
  res = soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
          unit,
          &is_high_res
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        
  pph_petra_a_compatible = !is_high_res || soc_pb_pp_sw_db_is_petra_a_compatible_get(unit);

  if (pph_petra_a_compatible == TRUE)
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      *max_age = 2; /* Difference between age 3 and 1 */
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }
  else
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      *max_age = 4; /* Difference between age 6 and 2 */
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      *max_age = 6; /* Difference between age 6 and 0 */
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_max_age_default_get()", 0, 0);
}

/*
 *	Get the default age configuration according to the
 *  device configuration and the MACT spec
 */
STATIC uint32
  soc_pb_pp_frwrd_mact_age_conf_default_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE  *default_conf
    )
{
  uint32
    res;
  uint8
    is_owned;
  uint32
    age_ndx;
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    is_high_res,
    pph_petra_a_compatible;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGE_CONF_DEFAULT_GET);

  /*
   *	Get the device configuration
   */
  learning_mode = soc_pb_pp_sw_db_learning_mode_get(unit);
  res = soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
          unit,
          &is_high_res
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  pph_petra_a_compatible = !is_high_res || soc_pb_pp_sw_db_is_petra_a_compatible_get(unit);
  /*
   *	Get the default configuration (according to the MACT spec)
   *  The age age_ndx is {Age, RBD}
   */
  if (pph_petra_a_compatible == TRUE)
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      /*
       *	At 3, refresh, at 2 delete if is-mine, at 1 delete entry
       */
      for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 2)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 3)
          {
            if (is_owned == FALSE)
            {
              default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
              default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
              default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
            }
            else /* is_owned == TRUE */
            {
              default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
              default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
              default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
            }
          }
          else if (age_ndx == 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = TRUE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      /*
       *	At 1 aged-out
       */
      for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx < 2)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 4)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = TRUE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      /*
       *	At 0 delete entry
       */
      for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }
  else
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      /*
       *	At 6, refresh, at 4 delete if is-mine, at 2 delete
       */
      for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx < 2)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 2)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 3)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 4)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = is_owned;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = TRUE;
          }
          else if (age_ndx == 7)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      /*
       *	At 2 aged-out if is-mine, at 0 aged-out if is-mine
       */
      for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if ((age_ndx == 0) || (age_ndx == 2))
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = is_owned;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      /*
       *	At 0 delete entry
       */
      for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_age_conf_default_get()", 0, 0);
}

/*
 *	Write the aging configuration
 */
STATIC uint32
  soc_pb_pp_frwrd_mact_age_conf_write(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE  *conf
    )
{
  uint32
    res;
  uint8
    is_owned;
  uint32
    base_offset,
    entry_offset,
    age_ndx;
  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGE_CONF_WRITE);

  /*
   *	The Aging configuration pointer is set as ONE for
   *  the MAC dynamic configuration
   */
  base_offset = SOC_PB_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC << SOC_PB_PP_FRWRD_MACT_AGE_NOF_BITS;
  
  for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
  {
    entry_offset = base_offset + age_ndx;

    for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
      tbl_data.delete_entry[is_owned] = conf->age_action[age_ndx][is_owned].deleted;
      tbl_data.create_aged_out_event[is_owned] = conf->age_action[age_ndx][is_owned].aged_out;
      tbl_data.create_refresh_event[is_owned] = conf->age_action[age_ndx][is_owned].refreshed;
    }

    res = soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10 + age_ndx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_age_conf_write()", age_ndx, 0);
}

/*
 *	Read the aging configuration
 */
STATIC uint32
  soc_pb_pp_frwrd_mact_age_conf_read(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE  *conf
    )
{
  uint32
    res;
  uint8
    is_owned;
  uint32
    base_offset,
    entry_offset,
    age_ndx;
  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGE_CONF_READ);

  SOC_SAND_CHECK_NULL_INPUT(conf);

  /*
   *	The Aging configuration pointer is set as ONE for
   *  the MAC dynamic configuration
   */
  base_offset = SOC_PB_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC << SOC_PB_PP_FRWRD_MACT_AGE_NOF_BITS;

  for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
  {
    entry_offset = base_offset + age_ndx;

    res = soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_get_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10 + age_ndx, exit);

    for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
      conf->age_action[age_ndx][is_owned].deleted = SOC_SAND_NUM2BOOL(tbl_data.delete_entry[is_owned]);
      conf->age_action[age_ndx][is_owned].aged_out = SOC_SAND_NUM2BOOL(tbl_data.create_aged_out_event[is_owned]);
      conf->age_action[age_ndx][is_owned].refreshed = SOC_SAND_NUM2BOOL(tbl_data.create_refresh_event[is_owned]);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_age_conf_read()", 0, 0);
}

/*
 *	Get the age to insert a new action (aged-out, refresh, delete)
 *  according to the device configuration
 */
STATIC uint32
  soc_pb_pp_frwrd_mact_age_modify_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION  *age_modif
    )
{
  uint32
    res;
  uint8
    is_owned;
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    is_high_res,
    pph_petra_a_compatible;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGE_MODIFY_GET);

  /*
   *	Get the device configuration
   */
  learning_mode = soc_pb_pp_sw_db_learning_mode_get(unit);
  res = soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
          unit,
          &is_high_res
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  pph_petra_a_compatible = !is_high_res || soc_pb_pp_sw_db_is_petra_a_compatible_get(unit);

  /*
   *	Get the default configuration
   *  The age format is {Age, RBD}
   */
  if (pph_petra_a_compatible == TRUE)
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      /*
       *	At 3, refresh, at 2 aged-out if not-mine,
       *  3 if is-mine, at 1 delete entry for not mine, 2 for mine
       */
      age_modif->age_delete[FALSE] = 1;
      age_modif->age_delete[TRUE] = 2;
      age_modif->age_aged_out[FALSE] = 2;
      age_modif->age_aged_out[TRUE] = 3;
      age_modif->age_refresh[FALSE] = 3;
      age_modif->age_refresh[TRUE] = 3;
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      /*
       *	At 2, refresh, at 1 aged-out
       *  at 0 delete entry
       */
      for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = 0;
        age_modif->age_aged_out[is_owned] = 1;
        age_modif->age_refresh[is_owned] = 2;
      }
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      /*
       *	At 2, refresh, at 1 aged-out
       *  at 0 delete entry
       */
      for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = 0;
        age_modif->age_aged_out[is_owned] = 1;
        age_modif->age_refresh[is_owned] = 2;
      }
       break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
    /*
     *	Set in the standard age format (Age, RBD)
     */
      for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = age_modif->age_delete[is_owned] << 1;
        age_modif->age_aged_out[is_owned] = age_modif->age_aged_out[is_owned] << 1;
        age_modif->age_refresh[is_owned] = age_modif->age_refresh[is_owned] << 1;
      }
    }
  else
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      /*
       *	At 6, refresh if is-mine, 4 if not, at 3 aged-out if not-mine,
       *  5 if is-mine, at 2 delete entry for not mine, 4 for mine
       */
      age_modif->age_delete[FALSE] = 2;
      age_modif->age_delete[TRUE] = 4;
      age_modif->age_aged_out[FALSE] = 3;
      age_modif->age_aged_out[TRUE] = 5;
      age_modif->age_refresh[FALSE] = 4;
      age_modif->age_refresh[TRUE] = 6;
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      /*
       *	At 3, refresh if is-mine, 2 if not, at 1 aged-out if not-mine,
       *  2 if is-mine, at 0 delete entry
       */
      age_modif->age_delete[FALSE] = 0;
      age_modif->age_delete[TRUE] = 0;
      age_modif->age_aged_out[FALSE] = 1;
      age_modif->age_aged_out[TRUE] = 2;
      age_modif->age_refresh[FALSE] = 2;
      age_modif->age_refresh[TRUE] = 3;
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      /*
       *	At 2, refresh, at 1 aged-out
       *  at 0 delete entry
       */
      for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = 0;
        age_modif->age_aged_out[is_owned] = 1;
        age_modif->age_refresh[is_owned] = 2;
      }
       break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_age_modify_get()", 0, 0);
}

/*********************************************************************
* NAME:
*     soc_pb_pp_frwrd_mact_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  soc_pb_pp_frwrd_mact_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  uint32
    fap_id;
  SOC_PB_PP_MGMT_OPERATION_MODE
    oper_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_REGS_INIT);

  regs = soc_pb_pp_regs();

  res = soc_pb_pp_mgmt_operation_mode_get_unsafe(
          unit,
          &oper_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  /*
   * Save the SA authentication in SW DB
   */
  soc_pb_pp_sw_db_authentication_enable_set(
    unit,
    oper_mode.authentication_enable
  );

  /*
   *	Enable the management unit
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_management_unit_configuration_reg.large_em_mngmnt_unit_enable, fld_val, 10, exit);

  /*
   *	Enable the Defrag machine
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_emcdefrag_configuration_reg.large_em_defrag_enable, fld_val, 20, exit);

  /*
   *	Enable an access of the VSI database when learning a MAC address
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_access_vsi_db_on_service_mac, fld_val, 30, exit);

  /*
   *	Enable the event mechanism
   */
  fld_val = SOC_PB_PP_FRWRD_MACT_FIFO_FULL_THRESHOLD;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_full_threshold, fld_val, 40, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_reply_fifo_configuration_reg.large_em_reply_fifo_full_threshold, fld_val, 50, exit);

  /*
   *	Use System-VSI (according to Operation mode) and FID mapping
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_access_fid_db, fld_val, 60, exit);
  fld_val = soc_pb_pp_sw_db_system_vsi_enable_get(unit);
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_access_vsi_db, fld_val, 70, exit);

  /*
   *	Take the FAP_id for the source_device
   */
  res = soc_petra_mgmt_system_fap_id_get(
          unit,
          &fap_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);

  fld_val = fap_id;
  SOC_PB_PP_FLD_SET(regs->olp.general_configuration1_reg.source_device, fld_val, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_mact_regs_init()",0,0);
}

uint8
  soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                   *is_high_res
  )
{
  uint32
    fld_val = 0x1,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(is_high_res);
  
  /* Verify above B0 if the high resolution is set */
  if (SOC_PB_REV_A1_OR_BELOW == FALSE)
  {
    regs = soc_pb_pp_regs();
    SOC_PB_PP_FLD_GET(regs->ihp.soc_petra_c_bugfix_reg.mact_aging_high_res_chicken_bit, fld_val, 10, exit);
  }

  *is_high_res = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution()",0,0);
}


/*********************************************************************
* NAME:
*     soc_pb_pp_frwrd_mact_mgmt_init_unsafe
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mgmt_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint8
    mac_in_mac_enabled,
    authentication_enable;
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO
    mac_limit_dflt_profile;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_MGMT_INIT);

  regs = soc_pb_pp_regs();

  res = soc_pb_pp_frwrd_mact_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set according to the operation mode the use of an
   *  opportunistic learning
   */
  authentication_enable = soc_pb_pp_sw_db_authentication_enable_get(unit);

  res = soc_pb_pp_is_mac_in_mac_enabled(
      unit,
      &mac_in_mac_enabled
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  fld_val = authentication_enable || mac_in_mac_enabled;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_ingress_learn_type_reg.large_em_ingress_learn_type, fld_val, 20, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.learning_cfg_reg.opportunistic_learning, fld_val, 30, exit);

  /*
   *	Enable the learning configuration for all forwarding codes
   */
  fld_val = SOC_PB_PP_FRWRD_MACT_ALL_FORWARDING_CODE_ENABLE;
  SOC_PB_PP_FLD_SET(regs->ihb.learning_cfg_reg.learn_enable_map, fld_val, 50, exit);

  /*
   *set bandwith for CPU access to LEM database
   */
  fld_val = 0x12c;
  SOC_PB_PP_FLD_SET(regs->ihp.sync_counter_reg.sync_counter, fld_val, 55, exit);

 /*
  * set profile 0: for "no-learn-limit"
  */
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_clear(&mac_limit_dflt_profile);
  mac_limit_dflt_profile.is_limited = FALSE;
  mac_limit_dflt_profile.action_when_exceed = SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE;

  res = soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
          unit,
          SOC_PB_PP_FRWRD_MACT_LEARN_LIMIT_DFLT_PROFILE_ID,
          &mac_limit_dflt_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_mact_mgmt_init_unsafe()",0,0);
}

STATIC uint32
  soc_pb_pp_frwrd_mact_is_ingress_learning_get(
        SOC_SAND_IN  int                      unit,
        SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE learning_mode,
        SOC_SAND_OUT uint8                      *is_ingress_learning
      )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_IS_INGRESS_LEARNING_GET);

  switch(learning_mode)
  {
  case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
  case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    *is_ingress_learning = TRUE;
    break;

  case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
  case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
  case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
    *is_ingress_learning = FALSE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 140, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_mact_is_ingress_learning_get()", learning_mode,0);
}
/*********************************************************************
*     Sets the mode of the MACT, including - ingress vs.
 *     egress learning- how each device responds internally to
 *     events (learn/aged-out/refresh) - which events to inform
 *     other devices.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    sa_lookup_type,
    reg_val,
    dsp_generation_en,
    dsp_delay,
    dsp_nof_cmds,
    fld_val,
    age_entry_key[SOC_PB_PP_FRWRD_MACT_NOF_AGE_KEYS] = {0x5, 0x7}, /* Set the Dynamic MAC addresses */
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    mac_in_mac_enabled,
    authentication_enable,
    is_centralized,
    is_egr_independent,
    is_ingress_learning,
    is_high_res,
    sys_phy_port,
    sys_lag_port,
    pph_petra_a_compatible;
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE
    default_conf;
  SOC_PB_PP_REGS
    *regs;
  uint32
    pp_port_ndx,
    is_lag_ndx,
    handle_profile_ndx,
    reg_ndx,
    fld_ndx,
    age_key_ndx;
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY
    event_key,
    event_key_no_mac;
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO
    handle_info,
    handle_info_no_mac;
  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA
    tbl_data;
  SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE
    msg_type[SOC_PB_PP_FRWRD_MACT_NOF_FIFOS];
  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;
  SOC_PB_REGS
    *tm_regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();

  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&default_conf);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key_no_mac);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info_no_mac);

  /*
   * Set the Software Database for future use
   */
  learning_mode = oper_mode_info->learning_mode;
  soc_pb_pp_sw_db_learning_mode_set(unit,learning_mode);
  pph_petra_a_compatible = oper_mode_info->soc_petra_a_compatible;
  soc_pb_pp_sw_db_is_petra_a_compatible_set(unit,pph_petra_a_compatible);

  /*
   * Set the age resolution
   */
  res = soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
          unit,
          &is_high_res
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  fld_val = !is_high_res || pph_petra_a_compatible;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_age_aging_resolution_reg.large_em_age_aging_resolution, !fld_val, 5, exit);

  /*
   * Set the age configuration by getting the default configuration
   */
  res = soc_pb_pp_frwrd_mact_age_conf_default_get(
          unit,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_frwrd_mact_age_conf_write(
          unit,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Set the aging mode
   */
  sys_phy_port = TRUE;
  if (pph_petra_a_compatible == TRUE)
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      sys_lag_port = FALSE;
      is_centralized = FALSE;
      is_egr_independent = FALSE;
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      sys_lag_port = FALSE;
      is_centralized = TRUE;
      is_egr_independent = FALSE;
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 30, exit);
    }
  }
  else
  {
    switch(learning_mode)
    {
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      sys_lag_port = TRUE;
      is_centralized = FALSE;
      is_egr_independent = FALSE;
      break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      sys_lag_port = TRUE;
      is_centralized = TRUE;
      is_egr_independent = FALSE;
    break;

    case SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      sys_lag_port = TRUE;
      is_centralized = FALSE;
      is_egr_independent = TRUE;
    break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 40, exit);
    }
  }

  SOC_PB_PP_REG_GET(regs->ihp.large_em_age_aging_mode_reg, reg_val, 30, exit);
  for (age_key_ndx = 0; age_key_ndx < SOC_PB_PP_FRWRD_MACT_NOF_AGE_KEYS; ++age_key_ndx)
  {
    fld_ndx = age_entry_key[age_key_ndx];
    fld_val = SOC_PB_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC;
    SOC_PB_PP_FLD_TO_REG(regs->ihp.large_em_age_aging_mode_reg.large_em_age_aging_cfg_ptr[fld_ndx], fld_val, reg_val,  50, exit);
    fld_val = sys_phy_port;
    SOC_PB_PP_FLD_TO_REG(regs->ihp.large_em_age_aging_mode_reg.large_em_age_own_system_physical_ports[fld_ndx], fld_val, reg_val,  60, exit);
    fld_val = sys_lag_port;
    SOC_PB_PP_FLD_TO_REG(regs->ihp.large_em_age_aging_mode_reg.large_em_age_own_system_lag_ports[fld_ndx], fld_val, reg_val,  70, exit);
  }
  SOC_PB_PP_REG_SET(regs->ihp.large_em_age_aging_mode_reg, reg_val, 80, exit);

  /*
   * Enable the learn and lookup filters
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_drop_duplicate, fld_val, 90, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_drop_duplicate, fld_val, 100, exit);

  /*
   * Set different Learn Filter TTLs if the loopback FIFO only is used
   */
  if (is_egr_independent == TRUE)
  {
    fld_val = SOC_PB_PP_FRWRD_MACT_LEARN_FILTER_TTL_LOOPBACK;
  }
  else
  {
    fld_val = SOC_PB_PP_FRWRD_MACT_LEARN_FILTER_TTL_MSGS;
  }
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_entry_ttl, fld_val, 105, exit);
  fld_val = 0x0;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_res, fld_val, 107, exit);

  /*
   * Enable the learn message configuration
   */
  msg_type[SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING] = oper_mode_info->learn_msgs_info.type;
  msg_type[SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW] = oper_mode_info->shadow_msgs_info.type;

  /*
   *  Soft reset of the OLP in case of direct access to prevent the OLP FIFO
   *  to get the first events
   */
  if (oper_mode_info->learn_msgs_info.type == SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS)
  {
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->eci.soc_petra_soft_reset_reg.olp_reset, 0x1, 108, exit);
  }
  else
  {
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->eci.soc_petra_soft_reset_reg.olp_reset, 0x0, 109, exit);
  }

  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_REG_GET(regs->olp.dsp_engine_configuration_reg[reg_ndx], reg_val, 110, exit);
    switch(msg_type[reg_ndx])
    {
    case SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE:
      dsp_generation_en = 0x1;
      dsp_nof_cmds = SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_SINGLE;
      dsp_delay = 0x20;
      break;

    case SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR:
      dsp_generation_en = 0x1;
      dsp_nof_cmds = SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_AGGR;
      dsp_delay = 0x3F;
      break;

    case SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS:
    default:
      dsp_generation_en = 0x0;
      dsp_nof_cmds = 1; /*Do -1 afterwards SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_DISABLED; */
      dsp_delay = 0;
      break;
    }
    dsp_nof_cmds = dsp_nof_cmds - 1;
    SOC_PB_PP_FLD_TO_REG(regs->olp.dsp_engine_configuration_reg[reg_ndx].dsp_generation_en, dsp_generation_en, reg_val, 130, exit);
    SOC_PB_PP_FLD_TO_REG(regs->olp.dsp_engine_configuration_reg[reg_ndx].max_cmd_delay, dsp_delay, reg_val, 133, exit);
    SOC_PB_PP_FLD_TO_REG(regs->olp.dsp_engine_configuration_reg[reg_ndx].max_dsp_cmd, dsp_nof_cmds, reg_val, 137, exit);
    SOC_PB_PP_REG_SET(regs->olp.dsp_engine_configuration_reg[reg_ndx], reg_val, 130, exit);

    if ((dsp_generation_en == TRUE) && (reg_ndx == SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING))
    {
      res = soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
              unit,
              &(oper_mode_info->learn_msgs_info.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    }
    else if ((dsp_generation_en == TRUE) && (reg_ndx == SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW))
    {
      /*
       * Enable the shadow message configuration
       */
      res = soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
              unit,
              &(oper_mode_info->shadow_msgs_info.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }
  }

  /*
   * Enable the OLP message engine generation
   */
  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_REG_GET(regs->olp.dsp_engine_configuration_reg[reg_ndx], reg_val, 165, exit);
    fld_val = SOC_PB_PP_FRWRD_MACT_DSP_MSG_MIN_SIZE;
    SOC_PB_PP_FLD_TO_REG(regs->olp.dsp_engine_configuration_reg[reg_ndx].min_dsp, fld_val, reg_val, 170, exit);
    SOC_PB_PP_REG_SET(regs->olp.dsp_engine_configuration_reg[reg_ndx], reg_val, 175, exit);
  }
  /*
   * Set the lookup type
   */
  res = soc_pb_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

   /*
    * check if mac in mac is enabled. 
    */
    res = soc_pb_pp_is_mac_in_mac_enabled(
              unit,
              &mac_in_mac_enabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    authentication_enable = soc_pb_pp_sw_db_authentication_enable_get(unit);

  if (is_ingress_learning == TRUE)
  {
    /*
     * Get if the device is set in an opportunistic SA learning mode
     */
    SOC_PB_PP_FLD_GET(regs->ihb.learning_cfg_reg.opportunistic_learning, fld_val, 145, exit);
    if (fld_val == 0x1)
    {
        if (mac_in_mac_enabled)
        {
            sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_MAC_IN_MAC_FLD_VAL;
        }
        else if (authentication_enable) 
        {
            sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_AUTHENTIFICATION_FLD_VAL;
        }
        else {
            sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SA_LOOKUP_FLD_VAL;
        }
    }
    else
    {
      sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SA_LOOKUP_FLD_VAL;
    }
  }
  else /* egress_learning */
  {
    if (mac_in_mac_enabled)
    {
      sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_MAC_IN_MAC_FLD_VAL;
    }
    else if (authentication_enable) 
    {
        sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_AUTHENTIFICATION_FLD_VAL;
    }
    else
    {
      sa_lookup_type = SOC_PB_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SA_LOOKUP_FLD_VAL;
    }

    /*
     * to get egress learning must be set to opportunistic learning
     */
    SOC_PB_PP_FLD_SET(regs->ihb.learning_cfg_reg.opportunistic_learning, 1, 147, exit);
  }
  SOC_PB_PP_FLD_SET(regs->eci.sa_lookup_type_reg.sa_lookup_type, sa_lookup_type, 150, exit);

  fld_val = is_ingress_learning;
  SOC_PB_PP_FLD_SET(regs->ihb.learning_cfg_reg.ingress_learning, fld_val, 160, exit);

  /*
   * Set the general event mode do not allow refresh for ingress learning
   */
  SOC_PB_PP_REG_GET(regs->ihp.large_em_lookup_lookup_mode_reg, reg_val, 170, exit);
  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES; ++fld_ndx)
  {
    if (fld_ndx == SOC_PB_PP_FRWRD_MACT_LEM_DB_MACT_ID)
    {
      if (is_ingress_learning == TRUE)
      {
        fld_val = SOC_PB_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_FLD_VAL;
      }
      else
      {
        fld_val = SOC_PB_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_AND_REFRESH_FLD_VAL;
      }
      SOC_PB_PP_FLD_TO_REG(regs->ihp.large_em_lookup_lookup_mode_reg.large_em_lookup_allowed_events_dynamic[fld_ndx], fld_val, reg_val, 180, exit);
      fld_val = 0;
      SOC_PB_PP_FLD_TO_REG(regs->ihp.large_em_lookup_lookup_mode_reg.large_em_lookup_allowed_events_static[fld_ndx], fld_val, reg_val, 190, exit);
      fld_val = 0x1;
      SOC_PB_PP_FLD_TO_REG(regs->ihp.large_em_lookup_lookup_mode_reg.large_em_lookup_allowed_events_learn[fld_ndx], fld_val, reg_val, 200, exit);
    }
  }
  SOC_PB_PP_REG_SET(regs->ihp.large_em_lookup_lookup_mode_reg, reg_val, 210, exit);

  /*
   *	Set at coarse granularity the event handling according to the learning
   *  mode
   */
  /*
   *	Building the key
   */
  /* config only profile zero */
  for (handle_profile_ndx = 0; handle_profile_ndx < 1; ++handle_profile_ndx)
  {
    event_key.vsi_event_handle_profile = handle_profile_ndx;
    for (is_lag_ndx = 0; is_lag_ndx < SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS; ++is_lag_ndx)
    {
      event_key.is_lag = SOC_SAND_NUM2BOOL(is_lag_ndx);
      if (is_centralized == TRUE)
      {
        /*
         *	For centralized, send all to the learning
         */
        event_key.event_type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN
          + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REQ_FAIL + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_ACK + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        handle_info.self_learning = FALSE;
        handle_info.send_to_learning_fifo = TRUE;
        handle_info.send_to_shadow_fifo = FALSE;
        res = soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
      }
      else if (is_egr_independent == TRUE)
      {
        /*
         *	For egress independent, all in loopback + all to the learning in case of lag
         */
        event_key.event_type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN;
        if (is_lag_ndx)
        {
          event_key.event_type += SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REFRESH + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        }
        handle_info.self_learning = TRUE;
        handle_info.send_to_learning_fifo = SOC_SAND_NUM2BOOL(is_lag_ndx);
        handle_info.send_to_shadow_fifo = FALSE;
        res = soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
      }
      else /* Distributed mode */
      {
        /*
         *	In Command, for transplant, learn, refresh, and delete (aged-out), if is-ll (self bit),
         *  learn message , otherwise (router must be informed) both.
         *  For the other commands (Ack, insert, defrag, change-fail), send to CPU.
         */
        event_key.event_type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN
          + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REFRESH + SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        handle_info.self_learning = FALSE;
        handle_info.send_to_learning_fifo = TRUE;
        handle_info.send_to_shadow_fifo = FALSE;
        res = soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
      }
    }
  }
  /*
   *	Set the shadow mode by a get-modify-set logic on the shadow bit
   */
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key);
  for (handle_profile_ndx = 0; handle_profile_ndx < 1; ++handle_profile_ndx)
  {
    event_key.vsi_event_handle_profile = handle_profile_ndx;
    for (is_lag_ndx = 0; is_lag_ndx < SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS; ++is_lag_ndx)
    {
      event_key.is_lag = SOC_SAND_NUM2BOOL(is_lag_ndx);
      for (event_type_ndx = 0; event_type_ndx < SOC_PB_PP_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
      {
        if ((event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
           ||(event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT)
           ||(event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN)
           )
        {
          event_key.event_type = event_type_ndx;
          res = soc_pb_pp_frwrd_mact_event_handle_info_get_unsafe(
                  unit,
                  &event_key,
                  &handle_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

          /*
           *	Allow for all these events to send to the shadow in general
           *  Two exceptions: not if is not lag in the lag mode, and the learn event
           *  in the arp mode
           */
          if ((oper_mode_info->shadow_mode == SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_NONE)
              || ((oper_mode_info->shadow_mode == SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_LAG) && (is_lag_ndx == 0)))
          {
            handle_info.send_to_shadow_fifo = FALSE;
          }
          else
          {
            handle_info.send_to_shadow_fifo = TRUE;
          }
          if ((event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN)
            && (oper_mode_info->shadow_mode == SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_ARP))
          {
            handle_info.send_to_shadow_fifo = FALSE;
          }

          res = soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe(
                  unit,
                  &event_key,
                  &handle_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
        }
      }
    }
  }

  /*
   *	Enable the learn per PP port in FLP
   */
  for (pp_port_ndx = 0; pp_port_ndx <= SOC_PB_PP_PORT_MAX; ++pp_port_ndx)
  {
    res = soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(
            unit,
            pp_port_ndx,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
    tbl_data.learn_enable = TRUE;
    tbl_data.sa_lookup_enable = TRUE;
    res = soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe(
            unit,
            pp_port_ndx,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_oper_mode_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO, oper_mode_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_oper_mode_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Sets the mode of the MACT, including - ingress vs.
 *     egress learning- how each device responds internally to
 *     events (learn/aged-out/refresh) - which events to inform
 *     other devices.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    dsp_generation_en,
    dsp_nof_cmds,
    res = SOC_SAND_OK;
  uint32
    reg_ndx,
    is_lag_ndx,
    handle_profile_ndx;
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY
    event_key;
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO
    handle_info;
  uint8
    is_shadow_always_false,
    is_shadow_always_false_for_other_events,
    is_shadow_aged_out_transplant_always_true,
    is_shadow_aged_out_transplant_lag_always_true,
    is_shadow_aged_out_transplant_not_lag_always_false,
    is_shadow_learned_always_false,
    is_shadow_learned_always_true,
    is_shadow_learned_not_lag_always_false,
    is_shadow_learned_lag_always_true,
    is_ingress_learning;
  SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE
    msg_type[SOC_PB_PP_FRWRD_MACT_NOF_FIFOS];
  SOC_PB_PP_REGS
    *regs;
  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_clear(oper_mode_info);

  regs = soc_pb_pp_regs();

  /*
   *	Get from the Software Database the device general configuration
   */
  oper_mode_info->learning_mode = soc_pb_pp_sw_db_learning_mode_get(unit);
  oper_mode_info->soc_petra_a_compatible = soc_pb_pp_sw_db_is_petra_a_compatible_get(unit);

  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_FLD_GET(regs->olp.dsp_engine_configuration_reg[reg_ndx].dsp_generation_en, dsp_generation_en, 9, exit);
    SOC_PB_PP_FLD_GET(regs->olp.dsp_engine_configuration_reg[reg_ndx].max_dsp_cmd, dsp_nof_cmds, 10, exit);
    if (dsp_generation_en == 0x0)
    {
      msg_type[reg_ndx] = SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS;
    }
    else
    {
      switch(dsp_nof_cmds)
      {
      case SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_SINGLE - 1:
        msg_type[reg_ndx] = SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE;
        break;

      case SOC_PB_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_AGGR - 1:
        msg_type[reg_ndx] = SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR;
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 15, exit);
      }
    }
  }

  oper_mode_info->learn_msgs_info.type = msg_type[SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING];
  oper_mode_info->shadow_msgs_info.type = msg_type[SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW];

  /*
   *	Get the learn message configuration
   */
  if (msg_type[SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING] != SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS)
  {
    res = soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
            unit,
            &(oper_mode_info->learn_msgs_info.info)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  /*
   *	Get the shadow message configuration
   */
  if (msg_type[SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW] != SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS)
  {
    res = soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
            unit,
            &(oper_mode_info->shadow_msgs_info.info)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

  /*
   *	To get the shadow mode, scan all the entries and see if there is a deviation
   *  from the mode of the first entry
   */
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key);
  /*
   *	Set the lookup type
   */
  res = soc_pb_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          soc_pb_pp_sw_db_learning_mode_get(unit),
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Check that for unused event types, nothing was configured.
   */

  is_shadow_always_false = TRUE;
  is_shadow_always_false_for_other_events = TRUE;
  is_shadow_aged_out_transplant_always_true = TRUE;
  is_shadow_aged_out_transplant_lag_always_true = TRUE;
  is_shadow_aged_out_transplant_not_lag_always_false = TRUE;
  is_shadow_learned_always_false = TRUE;
  is_shadow_learned_always_true = TRUE;
  is_shadow_learned_not_lag_always_false = TRUE;
  is_shadow_learned_lag_always_true = TRUE;

  for (handle_profile_ndx = 0; handle_profile_ndx < 1; ++handle_profile_ndx)
  {
    event_key.vsi_event_handle_profile = handle_profile_ndx;
    for (is_lag_ndx = 0; is_lag_ndx < SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS; ++is_lag_ndx)
    {
      event_key.is_lag = SOC_SAND_NUM2BOOL(is_lag_ndx);
      for (event_type_ndx = 0; event_type_ndx < SOC_PB_PP_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
      {
        if ((event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
          ||(event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT)
          ||(event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN)
          )
        {
          event_key.event_type = event_type_ndx;
          res = soc_pb_pp_frwrd_mact_event_handle_info_get_unsafe(
                  unit,
                  &event_key,
                  &handle_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

          if ((event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
             ||(event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT)
             )
          {
            if (handle_info.send_to_shadow_fifo == FALSE)
            {
              is_shadow_aged_out_transplant_always_true = FALSE;

            }
            if ((handle_info.send_to_shadow_fifo == FALSE) && (is_lag_ndx == TRUE))
            {
              is_shadow_aged_out_transplant_lag_always_true = FALSE;

            }
            else if ((handle_info.send_to_shadow_fifo == TRUE) && (is_lag_ndx == FALSE))
            {
              is_shadow_aged_out_transplant_not_lag_always_false = FALSE;
            }
          }
          else if (event_type_ndx == SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN)
          {
            if ((handle_info.send_to_shadow_fifo == FALSE) && ((is_lag_ndx == TRUE)))
            {
              is_shadow_learned_lag_always_true = FALSE;

            }
            else if ((handle_info.send_to_shadow_fifo == TRUE) && ((is_lag_ndx == FALSE)))
            {
              is_shadow_learned_not_lag_always_false = FALSE;
            }
            if (handle_info.send_to_shadow_fifo == TRUE)
            {
              is_shadow_learned_always_false = FALSE;

            }
            else
            {
              is_shadow_learned_always_true = FALSE;
            }
          }

          if (handle_info.send_to_shadow_fifo == TRUE)
          {
            is_shadow_always_false = FALSE;
          }
        }
      }
    }
  }

  /*
   *	Determine if the shadow is customized or its type
   */
  if (is_shadow_always_false == TRUE)
  {
    oper_mode_info->shadow_mode = SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_NONE;
  }
  else if (is_shadow_always_false_for_other_events && is_shadow_aged_out_transplant_always_true
           &&  is_shadow_learned_always_false)
  {
    oper_mode_info->shadow_mode = SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_ARP;
  }
  else if (is_shadow_always_false_for_other_events
            && is_shadow_aged_out_transplant_lag_always_true
            &&  is_shadow_aged_out_transplant_not_lag_always_false
            && is_shadow_learned_lag_always_true
            && is_shadow_learned_not_lag_always_false)
  {
    oper_mode_info->shadow_mode = SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_LAG;
  }
  else if (is_shadow_always_false_for_other_events
            && is_shadow_aged_out_transplant_always_true
            && is_shadow_learned_always_true)
  {
    oper_mode_info->shadow_mode = SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_ALL;
  }
  else
  {
    /*
     *	Not standard shadow mode: nof for customized
     */
    oper_mode_info->shadow_mode = SOC_PB_PP_NOF_FRWRD_MACT_SHADOW_MODES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_oper_mode_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    fld_val,
    ticks_per_microsec,
    entry_decrease_in_microsec,
    entry_decrease_in_16_clks,
    max_age,
    res = SOC_SAND_OK;
  SOC_SAND_U64
    tmp_result;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  regs = soc_pb_pp_regs();

  /*
   *	Number of ticks per micro sec
   */
  ticks_per_microsec = soc_petra_chip_mega_ticks_per_sec_get(unit);

 /*
   *	Get the maximal age (by default)
   */
  res = soc_pb_pp_frwrd_mact_max_age_default_get(
          unit,
          &max_age
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Compute the time for the machine to decrease one entry in clocks
   *  The time to go from an entry to the next plus the meta-cycle time between a loop
   *  and the next one
   */
  entry_decrease_in_microsec = (aging_info->aging_time.mili_sec
    + aging_info->aging_time.sec * SOC_PB_PP_FRWRD_MACT_NOF_MS_IN_SEC)
    * SOC_PB_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS / max_age;
	

  entry_decrease_in_16_clks = (entry_decrease_in_microsec / SOC_PB_PP_FRWRD_MACT_16_CLOCKS_RESOLUTION);

  soc_sand_u64_clear(&tmp_result);

  soc_sand_u64_multiply_longs(entry_decrease_in_16_clks, ticks_per_microsec, &tmp_result);

  if(tmp_result.arr[1] != 0) {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR, 15, exit);
  }

  entry_decrease_in_16_clks = tmp_result.arr[0];

  SOC_PB_PP_FLD_SET(regs->ihp.large_em_age_machine_meta_cycle_reg.large_em_age_machine_meta_cycle, entry_decrease_in_16_clks, 20, exit);

  /*
   * Set the shaper to 64 clocks
   */
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_age_machine_configuration_reg.large_em_age_machine_access_shaper, 64, 25, exit);

  /*
   *	Enable the aging machine
   */
  if (aging_info->enable_aging == TRUE)
  {
    fld_val = 0x1;
  }
  else
  {
    fld_val = 0x0;
  }
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_age_machine_configuration_reg.large_em_age_aging_enable, fld_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_aging_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_aging_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_AGING_INFO, aging_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_aging_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    fld_val,
    ticks_per_microsec,
    entry_decrease_in_microsec,
    entry_decrease_in_16_clks,
    max_age,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_PB_PP_FRWRD_MACT_AGING_INFO_clear(aging_info);

  regs = soc_pb_pp_regs();

  /*
   *	Number of ticks per ms
   */
  ticks_per_microsec = soc_petra_chip_mega_ticks_per_sec_get(unit);

 /*
   *	Get the maximal age (by default)
   */
  res = soc_pb_pp_frwrd_mact_max_age_default_get(
          unit,
          &max_age
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Compute the time for the machine to decrease one entry in clocks:
   *  The time to go from an entry to the next plus the meta-cycle time between a loop
   *  and the next one
   */
  SOC_PB_PP_FLD_GET(regs->ihp.large_em_age_machine_meta_cycle_reg.large_em_age_machine_meta_cycle, entry_decrease_in_16_clks, 20, exit);
  entry_decrease_in_microsec =
    (entry_decrease_in_16_clks / ticks_per_microsec) * SOC_PB_PP_FRWRD_MACT_16_CLOCKS_RESOLUTION;
  aging_info->aging_time.sec = (entry_decrease_in_microsec * max_age)
    / (SOC_PB_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS * SOC_PB_PP_FRWRD_MACT_NOF_MS_IN_SEC);
  aging_info->aging_time.mili_sec = ((entry_decrease_in_microsec * max_age) / SOC_PB_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS)
    % SOC_PB_PP_FRWRD_MACT_NOF_MS_IN_SEC;

  /*
   *	Enable the aging machine
   */
  SOC_PB_PP_FLD_GET(regs->ihp.large_em_age_machine_configuration_reg.large_em_age_aging_enable, fld_val, 30, exit);
  if (fld_val == TRUE)
  {
    aging_info->enable_aging = TRUE;
  }
  else
  {
    aging_info->enable_aging = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_aging_info_get_unsafe()", 0, 0);
}
/*********************************************************************
*     Sets the device action upon events invoked by the aging
 *     process: - Whether the device deletes aged-out entries
 *     internally - Whether the device generates an event for
 *     aged-out entries - Whether the device generates an event
 *     for refreshed entries
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    age_ndx,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION
    age_modif;
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE
    default_conf;
  uint8
    pph_petra_a_compatible,
    is_high_res,
    is_owned;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION_clear(&age_modif);
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&default_conf);

  /*
   *	Stamp the events to indicate aged-out entries
   */
  if (aging_info->event_when_aged_out == TRUE)
  {
    fld_val = 0x0;
    SOC_PB_PP_FLD_SET(regs->ihp.large_em_age_machine_configuration_reg.large_em_age_stamp, fld_val, 10, exit);
  }

  /*
   *	Set the Aging configuration table according to:
   *    1. The general configuration (given by the Software Database)
   *    2. If no delete, insert the delete at age 0
   *    3. If no aged-out, insert the aged-out one before the delete event
   *    4. If no refresh, insert the refresh one before the aged-out event
   *       or two before the delete event
   */
  res = soc_pb_pp_frwrd_mact_age_modify_get(
          unit,
          &age_modif
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_frwrd_mact_age_conf_default_get(
          unit,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
          unit,
          &is_high_res
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  
  pph_petra_a_compatible = !is_high_res || soc_pb_pp_sw_db_is_petra_a_compatible_get(unit);

  /*
   *	Modify the table according to the rule:
   *  if False, unset, if True, set at this age
   *  (Twice if the device is compatible with Soc_petra-A)
   */
  for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
  {
    age_ndx = age_modif.age_delete[is_owned];
    default_conf.age_action[age_ndx][is_owned].deleted = aging_info->delete_internally;
    age_ndx = age_modif.age_aged_out[is_owned];
    default_conf.age_action[age_ndx][is_owned].aged_out = aging_info->event_when_aged_out;
    age_ndx = age_modif.age_refresh[is_owned];
    default_conf.age_action[age_ndx][is_owned].refreshed = aging_info->event_when_refreshed;
    if (pph_petra_a_compatible)
    {
      age_ndx = age_modif.age_delete[is_owned] + 1;
      default_conf.age_action[age_ndx][is_owned].deleted = aging_info->delete_internally;
      age_ndx = age_modif.age_aged_out[is_owned] + 1;
      default_conf.age_action[age_ndx][is_owned].aged_out = aging_info->event_when_aged_out;
      age_ndx = age_modif.age_refresh[is_owned] + 1;
      default_conf.age_action[age_ndx][is_owned].refreshed = aging_info->event_when_refreshed;
    }
  }

  /*
   *	Write the new configuration
   */
  res = soc_pb_pp_frwrd_mact_age_conf_write(
          unit,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_aging_events_handle_info_set_unsafe()", 0, 0);
}
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE, aging_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_aging_events_handle_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Sets the device action upon events invoked by the aging
 *     process: - Whether the device deletes aged-out entries
 *     internally - Whether the device generates an event for
 *     aged-out entries - Whether the device generates an event
 *     for refreshed entries
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    age_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE
    conf;
  SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION
    age_modif;
  uint8
    is_owned;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(aging_info);
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(aging_info);

  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&conf);
  SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION_clear(&age_modif);

  /*
   *	Get the Hardware configuration
   */
  res = soc_pb_pp_frwrd_mact_age_conf_read(
          unit,
          &conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the ages to modify
   */
  res = soc_pb_pp_frwrd_mact_age_modify_get(
          unit,
          &age_modif
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	Get the setting according to is-not-mine
   */
  is_owned = FALSE;
  age_ndx = age_modif.age_delete[is_owned];
  aging_info->delete_internally = conf.age_action[age_ndx][is_owned].deleted;
  age_ndx = age_modif.age_aged_out[is_owned];
  aging_info->event_when_aged_out = conf.age_action[age_ndx][is_owned].aged_out;
  age_ndx = age_modif.age_refresh[is_owned];
  aging_info->event_when_refreshed = conf.age_action[age_ndx][is_owned].refreshed;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_aging_events_handle_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Maps FID-Profile to FID, for shared learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FID                                fid
  )
{
  uint32
    entry_offset,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE);

  /*
   *	The offset is from 0 to 6 since the FID profile 0 is reserved (= VID)
   */
  entry_offset = fid_profile_ndx;
  tbl_data.fid = fid;

  res = soc_pb_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe()", fid_profile_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FID                                fid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY);

  /* SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(fid_profile_ndx, SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN, SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fid, SOC_PB_PP_FID_MAX, SOC_PB_PP_FID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_verify()", fid_profile_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY);

  /* SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(fid_profile_ndx, SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN, SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get_verify()", fid_profile_ndx, 0);
}

/*********************************************************************
*     Maps FID-Profile to FID, for shared learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_FID                                 *fid
  )
{
  uint32
    entry_offset,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fid);

  /*
   *	The offset is from 0 to 6 since the FID profile 0 is reserved (= VID)
   */
  entry_offset = fid_profile_ndx;

  res = soc_pb_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *fid = tbl_data.fid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe()", fid_profile_ndx, 0);
}

/*********************************************************************
*     Enable the MAC limit feature, which limits per fid the
 *     maximum number of entries allowed to be in the MAC
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  regs = soc_pb_pp_regs();

  /*
   *	Enable the MAC limit FID mechanism
   */
  fld_val = limit_info->enable;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_fid_db_configuration_reg.large_em_fid_db_check_fid_limit, fld_val, 10, exit);

  /*
   *	Allow static entries to be inserted despite a FID limit reached
   */
  fld_val = limit_info->static_may_exceed;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_mngmnt_req_configuration_reg.large_em_mngmnt_req_allow_static_exceed, fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO, limit_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Enable the MAC limit feature, which limits per fid the
 *     maximum number of entries allowed to be in the MAC
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(limit_info);

  regs = soc_pb_pp_regs();

  /*
   *	Enable the MAC limit FID mechanism
   */
  SOC_PB_PP_FLD_GET(regs->ihp.large_em_fid_db_configuration_reg.large_em_fid_db_check_fid_limit, fld_val, 10, exit);
  limit_info->enable = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	Allow static entries to be inserted despite a FID limit reached
   */
  SOC_PB_PP_FLD_GET(regs->ihp.large_em_mngmnt_req_configuration_reg.large_em_mngmnt_req_allow_static_exceed, fld_val, 20, exit);
  limit_info->static_may_exceed = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries an FID can hold in the MAC
 *     Table), and the notification action if the configured
 *     limit is exceeded.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  /*
   *	Read-Modify-Write the configuration table
   */
  res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (limit_info->is_limited == TRUE)
  {
    tbl_data.limit = limit_info->nof_entries;
  }
  else
  {
    tbl_data.limit = SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_ENTRY_UNLIMITED;
  }

  switch(limit_info->action_when_exceed)
  {
  case SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE:
    tbl_data.interrupt_en = FALSE;
    tbl_data.message_en = FALSE;
    break;

  case SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT:
    tbl_data.interrupt_en = TRUE;
    tbl_data.message_en = FALSE;
    break;

  case SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG:
    tbl_data.interrupt_en = FALSE;
    tbl_data.message_en = TRUE;
    break;
  
  case SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT:
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR, 20, exit);
  }

  res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe()", mac_learn_profile_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO, limit_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_verify()", mac_learn_profile_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_profile_limit_info_get_verify()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries an FID can hold in the MAC
 *     Table), and the notification action if the configured
 *     limit is exceeded.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_clear(limit_info);

  /*
   *	Read the configuration table
   */
  res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (tbl_data.limit == SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_ENTRY_UNLIMITED)
  {
    limit_info->is_limited = FALSE;
    limit_info->nof_entries = 0;
  }
  else
  {
    limit_info->is_limited = TRUE;
    limit_info->nof_entries = tbl_data.limit;
  }

  if (tbl_data.interrupt_en == TRUE)
  {
    limit_info->action_when_exceed = SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT;
  }
  else if (tbl_data.message_en == TRUE)
  {
    limit_info->action_when_exceed = SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG;
  }
  else
  {
    limit_info->action_when_exceed = SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_profile_limit_info_get_unsafe()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the event-handle profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE);

  /*
   *	Read-Modify-Write the configuration table
   */
  res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.is_link_layer_fid = event_handle_profile;

  res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_profile_set_unsafe()", mac_learn_profile_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(event_handle_profile, SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_MAX, SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_profile_set_verify()", mac_learn_profile_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_profile_get_verify()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the event-handle profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_handle_profile);

  /*
   *	Read the configuration table
   */
  res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *event_handle_profile = tbl_data.is_link_layer_fid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_profile_get_unsafe()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Get the last event parameters for the direct access mode
 *     (i.e., if no OLP messages are sent in case of event)
 *     from the Event FIFO.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *soc_pb_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_buf);

  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER_clear(event_buf);

  regs = soc_petra_regs();
  soc_pb_regs = soc_pb_pp_regs();

  /*
   *	Verify an event is present in the Event FIFO via its interrupt
   */
  SOC_PB_FLD_GET(regs->ihp.mact_interrupt_reg.mact_event_ready, fld_val, 10, exit);
  if (fld_val == 0x0)
  {
    event_buf->buff_len = 0;
  }
  else
  {
    /*
     * Reading the LSB of register 0 extracts the event from the FIFO
     */
    SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_event_reg.reg_3, event_buf->buff[3], 20, exit);
    SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_event_reg.reg_2, event_buf->buff[2], 30, exit);
    SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_event_reg.reg_1, event_buf->buff[1], 40, exit);
    SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_event_reg.reg_0, event_buf->buff[0], 50, exit);
    event_buf->buff_len = 4;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set how to handle an event according to the event key
 *     parameters (event-type,vsi-handle-profile,is-lag)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res,
    build_val,
    reg_vals[SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS],
    start_write_bit,
    indx;
  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;
  SOC_PB_PP_REGS
    *regs;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);


  
  regs = soc_pb_pp_regs();

  start_write_bit = 0;

  start_write_bit = (1 << 5) | (event_key->vsi_event_handle_profile) | (event_key->is_lag << 4);

  res = soc_sand_mem_read_unsafe(
             unit,
             reg_vals,
             regs->olp.dsp_event_table_raw_reg[0].addr.base,
             SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS*4,
             FALSE
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  build_val = (handle_info->self_learning) | (handle_info->send_to_learning_fifo << 2) | (handle_info->send_to_shadow_fifo << 1);

 /*
  *	Command type
  */
  for (event_type_ndx = 0; event_type_ndx < SOC_PB_PP_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
  {
    if (SOC_SAND_GET_BIT(event_key->event_type, event_type_ndx) == 0x1)
    {
      indx = start_write_bit + (soc_pb_pp_frwrd_mact_event_type_to_event_val(SOC_SAND_BIT(event_type_ndx)) << 1);
      indx *= 3;

      res = soc_sand_bitstream_set_any_field(
              &build_val,
              indx,
              3,
              reg_vals
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

   res = soc_sand_mem_write_unsafe(
           unit,
           reg_vals,
           regs->olp.dsp_event_table_raw_reg[0].addr.base,
           SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS*4,
           FALSE
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_frwrd_mact_event_handle_info_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
    )
{
  uint32
    res;
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    is_ingress_learning;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY);

  /*
   *	In ingress learning, the is_lag parameter must be false
   */
  learning_mode = soc_pb_pp_sw_db_learning_mode_get(unit);
  /*
   *	Get if it is set in ingress larning
   */
 res = soc_pb_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  if ((is_ingress_learning == TRUE) && (event_key->is_lag == TRUE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_event_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY, event_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO, handle_info, 20, exit);

  res = soc_pb_pp_frwrd_mact_event_handle_info_verify(
          unit,
          event_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_event_handle_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY, event_key, 10, exit);

  res = soc_pb_pp_frwrd_mact_event_handle_info_verify(
          unit,
          event_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set how to handle an event according to the event key
 *     parameters (event-type,vsi-handle-profile,is-lag)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res,
    build_val=0xffffffff, /* Assigning all 1s, in case we accidently access it without assigning real value */
    reg_vals[SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS],
    start_write_bit,
    indx;
  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(handle_info);

  regs = soc_pb_pp_regs();
  
  start_write_bit = (1 << 5) | (event_key->vsi_event_handle_profile) | (event_key->is_lag << 4);

  res = soc_sand_mem_read_unsafe(
             unit,
             reg_vals,
             regs->olp.dsp_event_table_raw_reg[0].addr.base,
             SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS*4,
             FALSE
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  *	Command type
  */
  for (event_type_ndx = 0; event_type_ndx < SOC_PB_PP_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
  {
    if (SOC_SAND_GET_BIT(event_key->event_type, event_type_ndx) == 0x1)
    {
      indx = start_write_bit + (soc_pb_pp_frwrd_mact_event_type_to_event_val(SOC_SAND_BIT(event_type_ndx)) << 1);
      indx *= 3;

      build_val = 0;

      res = soc_sand_bitstream_get_any_field(
              reg_vals,
              indx,
              3,
              &build_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      break;
    }
  }

  handle_info->self_learning = SOC_SAND_GET_BIT(build_val,0);
  handle_info->send_to_learning_fifo = SOC_SAND_GET_BIT(build_val,2);
  handle_info->send_to_shadow_fifo = SOC_SAND_GET_BIT(build_val,1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_handle_info_get_unsafe()", 0, 0);
}

/*
 *	OLP message configuration
 */
STATIC
  uint32
    soc_pb_pp_frwrd_mact_olp_msg_set(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_FIFO       distribution_fifo,
      SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO  *distribution_info
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN
    otmh_extenstion;
  uint32
    header_size,
    dsp_header_type = 0,
    to_skip,
    header_type_lsb = 0,
    indx,
    header[SOC_PB_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE],
    otmh_size=0;
  SOC_TMC_HPU_ITMH_HDR
      itmh_hdr;
  SOC_PETRA_PORTS_ITMH
      itm_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  regs = soc_pb_pp_regs();

  if (distribution_info->header_type == SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH ||
      distribution_info->header_type == SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ITMH
     )
  {
     res = soc_petra_ports_otmh_extension_get_unsafe(
            unit,
            SOC_PETRA_OLP_PORT_ID,
            &otmh_extenstion
         );
     SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

     /* basic OTMH */
     otmh_size = SOC_PB_PP_FRWRD_MACT_OTMH_HEADER_SIZE;

     if (otmh_extenstion.dest_ext_en)
     {
       otmh_size += 2;
     }
     if (otmh_extenstion.outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS)
     {
       otmh_size += 2;
     }
     else if (otmh_extenstion.outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC)
     {
       /* check if MC */
       SOC_TMC_HPU_ITMH_HDR_clear(&itmh_hdr);
       itmh_hdr.base = distribution_info->header[SOC_PB_PP_FRWRD_MACT_MSG_HDR_SIZE - 1];
       res = soc_petra_hpu_itmh_parse(&itmh_hdr,&itm_info);
       SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
       if(itm_info.base.destination.type == SOC_TMC_DEST_TYPE_MULTICAST)
       {
         otmh_size += 2;
       }
     }
     if (otmh_extenstion.src_ext_en)
     {
       otmh_size += 2;
     }
  }


  /*
   *	Verify the FIFO
   */
  SOC_SAND_ERR_IF_ABOVE_MAX(distribution_fifo, SOC_PB_PP_FRWRD_MACT_NOF_FIFOS-1, SOC_PB_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR, 5, exit);

 /*
   *	Set the EtherType for the OLP messages (similar to Soc_petra-A)
   */
  to_skip = 0;
  switch(distribution_info->header_type)
  {
  case SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_RAW:
    header_size = SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
    header_type_lsb = 0;
    to_skip = 0;
    break;

  case SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ITMH:
    header_size = SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE + SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
    header_type_lsb = SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE;
   /*
    * in the receiver the ITMH will be replaced with OTMH.
    */
    to_skip = otmh_size;
    break;

  case SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH:
    to_skip = SOC_PB_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE + otmh_size;
    header_type_lsb = SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE + SOC_PB_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
    header_size = SOC_PB_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE + SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_PB_PP_FLD_SET(regs->olp.general_configuration0_reg.bytes_to_skip, to_skip, 15, exit);

 /*
  *  Copy the header
  */
  sal_memcpy(
    header,
    distribution_info->header,
    sizeof(uint32) * SOC_PB_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE
  );

  header_type_lsb = (SOC_PB_PP_FRWRD_MACT_DSP_HEADER_SIZE * SOC_SAND_NOF_BITS_IN_UINT32) -
    ((header_type_lsb + SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE) * SOC_SAND_NOF_BITS_IN_CHAR);

 /*
  * If the given header is Ethernet, then it should have the Ethernet type for learning.
  * Otherwise set DSP Type, so the receiver OLP will identify that this packet is valid as
  * learning message.
  */
  if (distribution_info->header_type != SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH)
  {
    dsp_header_type = SOC_PB_PP_FRWRD_MACT_MESSAGE_DSP_TYPE;
  }
  else
  {
    res = soc_sand_bitstream_get_any_field(
            distribution_info->header,
            header_type_lsb,
            SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE * SOC_SAND_NOF_BITS_IN_CHAR,
            &dsp_header_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          &dsp_header_type,
          header_type_lsb,
          SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE * SOC_SAND_NOF_BITS_IN_CHAR,
          header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
  * Configure registers that deduced (indirectly) from the
  * user parameters (EtherType, toSkip and EtherSize)
  * only Ethernet and ITMH over Ethernet Headers are supported
  * for other configuration (to skip already is set up).
  */

 /*
  * Set the header size (in Bytes)
  */
  SOC_PB_PP_FLD_SET(regs->olp.dsp_engine_configuration_reg[distribution_fifo].dsp_header_size, header_size, 50, exit);

  /*
   * Set Ethernet Type to be looked for in received packets.
   * get it from the supplied Header.
   */
  SOC_PB_PP_FLD_SET(regs->olp.general_configuration0_reg.dsp_eth_type, dsp_header_type, 60, exit);

  for (indx = 0; indx < SOC_PB_PP_FRWRD_MACT_DSP_HEADER_SIZE; ++indx)
  {
    SOC_PB_PP_REG_SET(regs->olp.dsp_header_reg[distribution_fifo][indx], header[indx], 20 + indx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_olp_msg_set()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_frwrd_mact_olp_msg_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_FIFO       distribution_fifo,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO  *distribution_info
    )
{
  uint32
    reg_val,
    res = SOC_SAND_OK,
    indx,
    header_size;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_clear(distribution_info);

  regs = soc_pb_pp_regs();

 /*
   *	Verify the FIFO
   */
  SOC_SAND_ERR_IF_ABOVE_MAX(distribution_fifo, SOC_PB_PP_FRWRD_MACT_NOF_FIFOS-1, SOC_PB_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR, 5, exit);

 /*
  * Get the header
  */

 /*
  * First, Get the header size (in Bytes)
  */
  SOC_PB_PP_FLD_GET(regs->olp.dsp_engine_configuration_reg[distribution_fifo].dsp_header_size, header_size, 10, exit);

 /*
  * Determine the Header type according to its length
  */
  if (header_size == SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE)
  {
    distribution_info->header_type = SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_RAW;
  }
  else if (header_size == SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE + SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE)
  {
    distribution_info->header_type = SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ITMH;
  }
  else if (header_size == SOC_PB_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE + SOC_PB_PP_FRWRD_MACT_ITMH_HEADER_SIZE)
  {
    distribution_info->header_type = SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH;
  }

 /*
  *  Copy header
  */
  for (indx = SOC_PB_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE; indx > 0; --indx)
  {
    SOC_PB_PP_REG_GET(regs->olp.dsp_header_reg[distribution_fifo][indx-1], reg_val, 20 + indx, exit);
    distribution_info->header[indx-1] = reg_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_olp_msg_get()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the learn messages to other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = soc_pb_pp_frwrd_mact_olp_msg_set(
          unit,
          SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe()", 0, 0);
}

uint32
soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO, distribution_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the learn messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_clear(distribution_info);

  res = soc_pb_pp_frwrd_mact_olp_msg_get(
          unit,
          SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the shadow messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                     *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = soc_pb_pp_frwrd_mact_olp_msg_set(
          unit,
          SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                     *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO, distribution_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the shadow messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                     *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_clear(distribution_info);

  res = soc_pb_pp_frwrd_mact_olp_msg_get(
          unit,
          SOC_PB_PP_FRWRD_MACT_FIFO_SHADOW,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Gets the information if the MAC limitation is exceeded,
 *     i.e. when a MAC Table entry is tried to be inserted and
 *     exceeds the limitation set per FID. This insertion can
 *     be triggered by CPU or after a packet learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exceed_info);

  regs = soc_pb_pp_regs();

  /*
   *	Read the last FIDs
   */
  SOC_PB_PP_FLD_GET(regs->ihp.large_em_mngmnt_req_exceed_limit_fid_reg.large_em_mngmnt_req_exceed_limit_fid, fld_val, 10, exit);
  exceed_info->fid_fail = fld_val;

  SOC_PB_PP_FLD_GET(regs->ihp.large_em_mngmnt_req_exceed_limit_static_allowed_fid_reg.large_em_mngmnt_req_exceed_limit_static_allowed_fid, fld_val, 20, exit);
  exceed_info->fid_allowed = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set per port the MACT management information, including
 *     which profile to activate when SA is known in this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  /*
   *	Read-modify-write in the configuration table
   */
  res = soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(
          unit,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.action_profile_sa_not_found_index = port_info->sa_unknown_action_profile;
  tbl_data.action_profile_da_not_found_index = port_info->da_unknown_action_profile;
  tbl_data.action_profile_sa_drop_index = port_info->sa_drop_action_profile;

  res = soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe(
          unit,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_mact_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Set per port MACT management information including which
 *     profile to activate when SA is known in this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PB_PP_FRWRD_MACT_PORT_INFO_clear(port_info);

  /*
   *	Read the configuration table
   */
  res = soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(
          unit,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_info->sa_unknown_action_profile = tbl_data.action_profile_sa_not_found_index;
  port_info->da_unknown_action_profile = tbl_data.action_profile_da_not_found_index;
  port_info->sa_drop_action_profile = tbl_data.action_profile_sa_drop_index;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_port_info_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     For each trap type, set the action profile. Different
 *     actions may be assigned to the same trap type according
 *     to the port-profile (4 possibilities).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_trap_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  regs = soc_pb_pp_regs();

  /*
   *	Set the action profile strengths
   */
  switch(trap_type_ndx)
  {
  case SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_DROP:
    SOC_PB_PP_FLD_SET(regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[port_profile_ndx], action_profile->frwrd_action_strength, 20, exit);
    SOC_PB_PP_FLD_SET(regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[port_profile_ndx], action_profile->snoop_action_strength, 30, exit);
    break;

  case SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN:
    SOC_PB_PP_FLD_SET(regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[port_profile_ndx], action_profile->frwrd_action_strength, 40, exit);
    SOC_PB_PP_FLD_SET(regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[port_profile_ndx], action_profile->snoop_action_strength, 50, exit);
    break;

  case SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN:
    SOC_PB_PP_FLD_SET(regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[port_profile_ndx], action_profile->frwrd_action_strength, 60, exit);
    SOC_PB_PP_FLD_SET(regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[port_profile_ndx], action_profile->snoop_action_strength, 70, exit);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_trap_info_set_unsafe()", 0, port_profile_ndx);
}

uint32
  soc_pb_pp_frwrd_mact_trap_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type_ndx, SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, action_profile, 30, exit);

  if (trap_type_ndx == SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN &&
      (action_profile->trap_code != (SOC_PB_PP_TRAP_CODE)(SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_0 + port_profile_ndx)))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR, 40, exit);
  }

  if (trap_type_ndx == SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN &&
    (action_profile->trap_code != (SOC_PB_PP_TRAP_CODE)(SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_0 + port_profile_ndx)))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR, 50, exit);
  }

  if (trap_type_ndx == SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_DROP &&
    (action_profile->trap_code != (SOC_PB_PP_TRAP_CODE)(SOC_PB_PP_TRAP_CODE_SA_DROP_0 + port_profile_ndx)))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_trap_info_set_verify()", 0, port_profile_ndx);
}

uint32
  soc_pb_pp_frwrd_mact_trap_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type_ndx, SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_trap_info_get_verify()", 0, port_profile_ndx);
}

/*********************************************************************
*     For each trap type, set the action profile. Different
 *     actions may be assigned to the same trap type according
 *     to the port-profile (4 possibilities).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_trap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PB_PP_ACTION_PROFILE_clear(action_profile);

  regs = soc_pb_pp_regs();

  /*
   *	Set the action profile strengths
   */
  switch(trap_type_ndx)
  {
  case SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_DROP:
    SOC_PB_PP_FLD_GET(regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[port_profile_ndx], action_profile->frwrd_action_strength, 20, exit);
    SOC_PB_PP_FLD_GET(regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[port_profile_ndx], action_profile->snoop_action_strength, 30, exit);
    break;

  case SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN:
    SOC_PB_PP_FLD_GET(regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[port_profile_ndx], action_profile->frwrd_action_strength, 40, exit);
    SOC_PB_PP_FLD_GET(regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[port_profile_ndx], action_profile->snoop_action_strength, 50, exit);
    break;

  case SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN:
    SOC_PB_PP_FLD_GET(regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[port_profile_ndx], action_profile->frwrd_action_strength, 60, exit);
    SOC_PB_PP_FLD_GET(regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[port_profile_ndx], action_profile->snoop_action_strength, 70, exit);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 80, exit);
  }

  if (trap_type_ndx == SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN)
  {
      action_profile->trap_code = SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_0 + port_profile_ndx;
  }

  if (trap_type_ndx == SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN)
  {
    action_profile->trap_code = SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_0 + port_profile_ndx;
  }

  if (trap_type_ndx == SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_DROP)
  {
    action_profile->trap_code = SOC_PB_PP_TRAP_CODE_SA_DROP_0 + port_profile_ndx;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_trap_info_get_unsafe()", 0, port_profile_ndx);
}

/*********************************************************************
*     Sets the information for bridging compatible Multicast
 *     MAC addresses.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Use of micro program
   */
  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_verify()", 0, 0);
}

/*********************************************************************
*     Sets the information for bridging compatible Multicast
 *     MAC addresses.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(info);

  /*
   *	Use of micro program
   */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe()", 0, 0);
}


/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO              *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    buff_ndx;
  SOC_PB_PP_LEM_ACCESS_BUFFER
    buffer;
  SOC_PB_PP_LEM_ACCESS_OUTPUT
    output;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_buf);
  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  SOC_PB_PP_LEM_ACCESS_BUFFER_clear(&buffer);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);
  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(&output);

  /*
   * Parse LEM access buffer
   */
  for (buff_ndx = 0; buff_ndx < event_buf->buff_len; ++buff_ndx)
  {
    buffer.data[buff_ndx] = event_buf->buff[buff_ndx];
  }
  res = soc_pb_pp_lem_access_parse(
          unit,
          &buffer,
          &output,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * MACT event type
   */
  switch (output.request.command)
  {
  case SOC_PB_PP_LEM_ACCESS_CMD_DELETE:
    mact_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
    break;

  case SOC_PB_PP_LEM_ACCESS_CMD_LEARN:
    mact_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN;
    break;

  case SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT:
    mact_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT;
    break;

  case SOC_PB_PP_LEM_ACCESS_CMD_REFRESH:
    mact_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REFRESH;
    break;

  case SOC_PB_PP_LEM_ACCESS_CMD_ACK:
    mact_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_ACK;
    break;

  case SOC_PB_PP_LEM_ACCESS_CMD_ERROR:
    mact_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 15, exit);
  }

  /*
   * Key
   */
  res = soc_pb_pp_frwrd_mact_key_parse(
          unit,
          &output.request.key,
          &mact_event->key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Value
   */
  res = soc_pb_pp_frwrd_mact_payload_convert(
          unit,
          &output.payload,
          &mact_event->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   * LAG
   */
  mact_event->lag.is_lag = output.is_part_of_lag;
  mact_event->lag.id = output.stamp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_parse_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_event_parse_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER, event_buf, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_event_parse_verify()", 0, 0);
}
void
  SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION  *info
  )
{
  uint8
    is_owned;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION));
  for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
  {
    info->age_delete[is_owned] = 8;
    info->age_aged_out[is_owned] = 8;
    info->age_refresh[is_owned] = 8;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC void
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_clear(
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENT  *info
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FRWRD_MACT_AGING_EVENT));
  info->deleted = 8;
  info->aged_out = 8;
  info->refreshed = 8;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE  *info
  )
{
  uint8
    is_owned;
  uint32
    age_ndx;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE));
  for (age_ndx = 0; age_ndx < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
  {
    for (is_owned = 0; is_owned < SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
      SOC_PB_PP_FRWRD_MACT_AGING_EVENT_clear(&(info->age_action[age_ndx][is_owned]));
    }
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
soc_pb_pp_frwrd_mact_learn_msg_conf_get_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG          *learn_msg,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF     *learn_msg_conf
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO
    distr_info;
  SOC_PPD_TRAP_PACKET_INFO
    pkt_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* parse packet header */
  res = soc_ppd_trap_packet_parse(unit, learn_msg->msg_buffer,learn_msg->msg_len,&pkt_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  learn_msg_conf->header_size = pkt_info.ntwrk_header_ptr;

  /* Get Header Type */
  res = soc_pb_pp_frwrd_mact_olp_msg_get(
          unit,
          SOC_PB_PP_FRWRD_MACT_FIFO_LEARNING,
          &distr_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (distr_info.header_type == SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH)
  {
    learn_msg_conf->header_size += SOC_PB_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
  }
  
  /* Get Receive Mode */
  res = soc_petra_pkt_packet_receive_mode_get_unsafe(&(learn_msg_conf->recv_mode));
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_msg_conf_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_mact_learn_msg_parse_unsafe(
      SOC_SAND_IN   int                                             unit,
      SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG                  *learn_msg,
      SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF             *learn_msg_conf,
      SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO       *learn_events
    )
{
  uint32
    res = SOC_SAND_OK,
    i,
    event_index,
    skip_bytes,
    offset,
    fld_val;
  uint8
    msg_buffer[SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAX_LEN],
    *buff;
  SOC_PB_PP_FRWRD_MACT_EVENT_INFO
    *c_event;
  uint32
    fld_array[2] = {0},
    asd = 0,
    ext_skip = 0,
    dest = 0,
    skipped_events = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(learn_msg);
  SOC_SAND_CHECK_NULL_INPUT(learn_msg_conf);
  SOC_SAND_CHECK_NULL_INPUT(learn_events);

  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(learn_events);

  sal_memset(msg_buffer, 0, sizeof(msg_buffer));

  /* Calculate skip_bytes according to the header type */

  skip_bytes = learn_msg_conf->header_size;

  /* Handle MSB_TO_LSB or LSB_TO_MSB buffer type */
  if (learn_msg_conf->recv_mode == SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
  {
    for (i = 0; i < learn_msg->msg_len - skip_bytes; i++)
    {
      msg_buffer[i] = learn_msg->msg_buffer[learn_msg->msg_len - skip_bytes - i - 1];
    }
  }
  else
  {
    if(learn_msg->msg_len - skip_bytes > SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAX_LEN)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_MSG_LEN_ERR, 20, exit);
    }
    soc_sand_os_memcpy(msg_buffer,learn_msg->msg_buffer + skip_bytes,learn_msg->msg_len - skip_bytes);
  }

  
  /* check this is indeed learn packet */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            msg_buffer,
            0,
            SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE*8,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (fld_val != SOC_PB_PP_FRWRD_MACT_MESSAGE_DSP_TYPE)
    
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR, 15, exit);
  }
  fld_val = 0;

  /* Start event loop, each iteration parses one event */
  learn_events->nof_events_in_msg = 0;
  /* now skip ether-type */
  offset = SOC_PB_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
  for(event_index = 0; event_index < learn_msg->max_nof_events ; event_index++)
  {
    offset += (event_index * SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EVENT_LEN) + ext_skip;
    if(offset > learn_msg->msg_len)
    {
      /* Finished parsing the message */
      break;
    }

    /* Buff should point to the start of the event */
    buff = msg_buffer + offset;

    /* Check valid bit */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_VALID_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_VALID_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if(fld_val != 1)
    {
      /* Event is not valid, skip */
      skipped_events++;
      continue;
    }

    learn_events->nof_events_in_msg++;
    /* c_event will point to the output struct */
    c_event = &learn_events->events[learn_events->nof_parsed_events];
    learn_events->nof_parsed_events++;

    /* Check for extension */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_EXT_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_EXT_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

    if(fld_val == 1)
    {
      ext_skip += SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EXT_LEN;

      /* Construct ASD out of extension payload */
      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
        unit,
        buff,
        SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_1_INDEX,
        SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_1_LEN,
        &fld_val
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

      asd = fld_val;

      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
        unit,
        buff,
        SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_2_INDEX,
        SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_2_LEN,
        &fld_val
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

      asd |= (fld_val << SOC_PB_PP_FRWRD_MACT_LEARN_MSG_ASD_1_LEN);
    }

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      buff,
      SOC_PB_PP_FRWRD_MACT_LEARN_DEST_INDEX,
      SOC_PB_PP_FRWRD_MACT_LEARN_DEST_LEN,
      &dest
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);    

      res = soc_pb_pp_fwd_decision_in_buffer_parse(
        SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
        dest,
        asd,
        &(c_event->value.frwrd_info.forward_decision),
        &(c_event->value.frwrd_info.drop_when_sa_is_known)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);


    /* Type */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_TYPE_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_TYPE_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    switch (fld_val)
    {
      case SOC_PB_PP_LEM_ACCESS_CMD_DELETE:
        c_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        break;

      case SOC_PB_PP_LEM_ACCESS_CMD_LEARN:
        c_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN;
        break;

      case SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT:
        c_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT;
        break;

      case SOC_PB_PP_LEM_ACCESS_CMD_REFRESH:
        c_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REFRESH;
        break;

      case SOC_PB_PP_LEM_ACCESS_CMD_ACK:
        c_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_ACK;
        break;

      case SOC_PB_PP_LEM_ACCESS_CMD_ERROR:
        c_event->type = SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
        break;

      default:
        skipped_events++;
        continue;
    }

    /* MFF Key */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if(fld_val == 1)
    {
      /* MFF IS KEY */
      c_event->key.key_type = SOC_PB_PP_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
              unit,
              buff,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FID_INDEX,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FID_LEN,
              &fld_val
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      c_event->key.key_val.mac.fid = fld_val;

      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
              unit,
              buff,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAC_INDEX + 16,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAC_LEN - 16,
              &fld_val
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      fld_array[0] = fld_val;
      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
              unit,
              buff,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAC_INDEX,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_MAC_LEN - 32,
              &fld_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      fld_array[1] = fld_val;
      res = soc_sand_pp_mac_address_long_to_struct(fld_array, &c_event->key.key_val.mac.mac);

    }
    else
    {
      /* MFF IS NOT KEY */
      c_event->key.key_type = SOC_PB_PP_FRWRD_MACT_KEY_TYPE_IPV4_MC;

      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
              unit,
              buff,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_FID_INDEX,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_FID_LEN,
              &fld_val
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      c_event->key.key_val.ipv4_mc.fid = fld_val;

      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
              unit,
              buff,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_DIP_INDEX,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IPV4_DIP_LEN,
              &fld_val
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      c_event->key.key_val.ipv4_mc.dip = fld_val;     
    }

    /* Lag */
    if(c_event->type != SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_ACK)
    {
      res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
              unit,
              buff,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_LAG_INDEX,
              SOC_PB_PP_FRWRD_MACT_LEARN_MSG_LAG_LEN,
              &fld_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      c_event->lag.id = fld_val;
      c_event->lag.is_lag = (uint8)(c_event->lag.id != 0);
    }
    else
    {
      c_event->lag.is_lag = 0;
      c_event->lag.id = 0;
    }

    /* Is Dynamic */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_DYNAMIC_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_DYNAMIC_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    c_event->value.aging_info.is_dynamic = SOC_SAND_NUM2BOOL(fld_val);
    c_event->value.aging_info.age_status = 0;
    
    /* SA is Known */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_SA_KNOWN_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_SA_KNOWN_LEN,
            &fld_val
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
    c_event->value.frwrd_info.drop_when_sa_is_known = SOC_SAND_NUM2BOOL(fld_val);

    /* Source FAP */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FAP_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_FAP_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    learn_events->source_fap = fld_val;

  }/* End of event loop */

  /* Check for more events (without parsing) */
  for(; ; event_index++)
  {
    offset += (event_index * SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EVENT_LEN) + ext_skip;
    if(offset > learn_msg->msg_len)
    {
      /* Finished parsing the message */
      break;
    }
    learn_events->nof_events_in_msg++;

    /* Buff should point to the start of the event */
    buff = msg_buffer + offset;

    /* Check for extension */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_EXT_INDEX,
            SOC_PB_PP_FRWRD_MACT_LEARN_MSG_IS_EXT_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

    if(fld_val == 1)
    {
      ext_skip += SOC_PB_PP_FRWRD_MACT_LEARN_MSG_EXT_LEN;
    }
  }

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_learn_msg_parse()", 0, 0);
}



/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_frwrd_mact_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_mact_mgmt_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_frwrd_mact_mgmt;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_frwrd_mact_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_mact_mgmt_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_frwrd_mact_mgmt;
}
uint32
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_TIME, &(info->aging_time), 11, exit);
   /* limit check is loose check, actual limit will be check according to set function */
    SOC_SAND_ERR_IF_ABOVE_MAX(info->aging_time.sec, SOC_PB_PP_FRWRD_MACT_TIME_SEC_MAX, SOC_PB_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_AGING_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->event_type, SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_MAX, SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_event_handle_profile, SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_MAX, SOC_PB_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_verify()",0,0);
}
uint32
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_MAX, SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
 
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_MACT_TYPE_MAX, SOC_PB_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO, &(info->info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->learning_mode, SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_MAX, SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO, &(info->learn_msgs_info), 12, exit);
  if (info->shadow_mode != SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_ALL)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->shadow_mode, SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_MAX, SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR, 13, exit);
  }
  if (info->shadow_mode != SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_NONE)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO, &(info->shadow_msgs_info), 14, exit);
  }

  if ((info->learning_mode == SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT) && (info->soc_petra_a_compatible == TRUE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_verify()",0,0);
}
uint32
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, SOC_PB_PP_FRWRD_MACT_NOF_ENTRIES_MAX, SOC_PB_PP_FRWRD_MACT_MGMT_NOF_ENTRIES_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_when_exceed, SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_MAX, SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR, 12, exit);

  if (info->is_limited == FALSE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, 0, SOC_PB_PP_FRWRD_MACT_MGMT_NOF_ENTRIES_OUT_OF_RANGE_ERR, 30, exit);
  }

  if (info->action_when_exceed == SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR, 40, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_verify()",0,0);
}
uint32
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_fail, SOC_PB_PP_FRWRD_MACT_FID_FAIL_MAX, SOC_PB_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_allowed, SOC_PB_PP_FID_MAX, SOC_PB_PP_FID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->fid_base)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_base, SOC_PB_PP_FID_MAX, SOC_PB_PP_FID_OUT_OF_RANGE_ERR, 13, exit);
  }

  if (info->generate_event == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify()",0,0);
}
uint32
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sa_drop_action_profile, SOC_PB_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_MAX, SOC_PB_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->sa_unknown_action_profile, SOC_PB_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_MAX, SOC_PB_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_unknown_action_profile, SOC_PB_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_MAX, SOC_PB_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, SOC_PB_PP_FRWRD_MACT_ID_MAX, SOC_PB_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_MACT_TYPE_MAX, SOC_PB_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_KEY, &(info->key), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE, &(info->value), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO, &(info->lag), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_EVENT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_FRWRD_MACT_EVENT_BUFF_MAX_SIZE; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->buff[ind], SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_MAX, SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->buff_len, SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_LEN_MAX, SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

