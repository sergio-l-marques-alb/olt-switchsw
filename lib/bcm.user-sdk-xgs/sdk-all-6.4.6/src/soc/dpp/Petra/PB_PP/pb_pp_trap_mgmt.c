/* $Id: pb_pp_trap_mgmt.c,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_trap_mgmt.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#include <soc/dpp/Petra/petra_packet.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX                      (SOC_PB_PP_NOF_TRAP_CODES-1)
#define SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN                  (1)
#define SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX                  (7)
#define SOC_PB_PP_TRAP_MGMT_BUFF_LEN_MAX                           (SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE)
#define SOC_PB_PP_TRAP_MGMT_VSI_SHIFT_MAX                          (3)
#define SOC_PB_PP_TRAP_MGMT_COUNTER_SELECT_MAX                     (1)
#define SOC_PB_PP_TRAP_MGMT_COUNTER_ID_MAX                         (4095)
#define SOC_PB_PP_TRAP_MGMT_METER_SELECT_MAX                       (1)
#define SOC_PB_PP_TRAP_MGMT_METER_ID_MAX                           ((1 << SOC_DPP_METER_POINTER_NOF_BITS_PETRAB) - 1)
#define SOC_PB_PP_TRAP_MGMT_METER_COMMAND_MAX                      (3)
#define SOC_PB_PP_TRAP_MGMT_ETHERNET_POLICE_ID_MAX                 (511)
#define SOC_PB_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_MAX                 (7)
#define SOC_PB_PP_TRAP_MGMT_STRENGTH_MAX                           (7)
#define SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_MAX                        (SOC_SAND_U32_MAX)
#define SOC_PB_PP_TRAP_MGMT_SNOOP_CMND_MAX                         (15)
#define SOC_PB_PP_TRAP_MGMT_CUD_MAX                                (0xFFFF)
#define SOC_PB_PP_TRAP_MGMT_TYPE_MAX                               (SOC_PB_PP_NOF_TRAP_MACT_EVENT_TYPES-1)
#define SOC_PB_PP_TRAP_MGMT_CPU_TRAP_CODE_MAX                      (SOC_PB_PP_NOF_TRAP_CODES-1)
#define SOC_PB_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_MAX                 (4095)
#define SOC_PB_PP_TRAP_MGMT_SRC_SYS_PORT_MAX                       (SOC_SAND_U32_MAX)
#define SOC_PB_PP_TRAP_MGMT_LL_HEADER_PTR_MAX                      (SOC_SAND_U32_MAX)

#define SOC_PB_PP_TRAP_MGMT_SNP_STRENGTH_MAX                       (3)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile) \
  (valid)?(SOC_PB_PP_FLDS_TO_BUFF_2(valid, 1, eg_action_profile, 3)):(0)

#define SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile) \
  do { \
    valid = SOC_SAND_GET_BIT(fld_val, 3); \
    eg_action_profile = (valid)?SOC_SAND_GET_BITS_RANGE(fld_val, 2, 0):0; \
  } while (0)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint32 trap_code;
  
  struct
  {
    uint32 strength;

    struct
    {
      uint8 is_oam_dest;
      /*SOC_PB_PP_FRWRD_DECISION_INFO frwrd_dest;*/
      uint8 add_vsi;
      uint32  vsi_shift;
    } info;
  } frwrd;

  struct
  {
    uint32 strength;

    struct
    {
      uint32 snoop_cmnd;
    } info;
  } snoop;
}SOC_PB_PP_TRAP_MGMT_ACTION;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PB_PP_TRAP_MGMT_ACTION
  Soc_pb_pp_trap_mgmt_action[] =
{
  {SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_0, {0, {FALSE, FALSE, 0}}, {0, {0}}}
};

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_trap_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_PARSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MACT_EVENT_PARSE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MGMT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_MGMT_TRAP_CODE_TO_INTERNAL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_CPU_TRAP_CODE_FROM_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_PACKET_PARSE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SB_TO_TRAP_CODE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_TRAP_SB_VTT_TO_INTERNAL_TRAP_CODE_MAP_GET),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_trap_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_TRAP_EG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'eg_action_profile' is out of range. \n\r "
    "The range is: 1 - 7 or 0xffffffff (SOC_PB_PP_TRAP_EG_NO_ACTION).\n\r "
    "Note 0 also is out of range as it indicates no action",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 1 - 7.\n\r "
    "Note 0 also is out of range as it indicates no action",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_BUFF_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 4 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR",
    "The parameter 'vsi_shift' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR",
    "The parameter 'counter_select' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR",
    "The parameter 'counter_id' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR",
    "The parameter 'meter_select' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR",
    "The parameter 'meter_id' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'meter_command' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'ethernet_police_id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_offset_index' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'strength' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'bitmap_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_SIZE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'size' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_TRAP_SNOOP_ACTION_SIZES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_SAMPLING_PROBABILITY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_SAMPLING_PROBABILITY_OUT_OF_RANGE_ERR",
    "The parameter 'sampling_probability' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_cmnd' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR",
    "The parameter 'cud' is out of range. \n\r "
    "The range is: 0 - 0xFFFF.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_TRAP_MACT_EVENT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_trap_code' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_trap_qualifier' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'src_sys_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR",
    "The parameter 'll_header_ptr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR,
    "SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR",
    "Trap code is not supported. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR",
    "'strength' is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR,
    "SOC_PB_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR",
    "More than one bit in bitmap is on.",
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
  soc_pb_pp_trap_mgmt_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;
  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO
    frwrd_action_profile_info;
  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO
    snoop_action_profile_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (idx = 0; idx < sizeof(Soc_pb_pp_trap_mgmt_action) / sizeof(SOC_PB_PP_TRAP_MGMT_ACTION); ++idx)
  {
    SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&frwrd_action_profile_info);
    frwrd_action_profile_info.bitmap_mask = SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    frwrd_action_profile_info.strength = Soc_pb_pp_trap_mgmt_action[idx].frwrd.strength;
    SOC_PB_PP_FRWRD_DECISION_DROP_SET(unit,&frwrd_action_profile_info.dest_info.frwrd_dest);
    frwrd_action_profile_info.dest_info.add_vsi = Soc_pb_pp_trap_mgmt_action[idx].frwrd.info.add_vsi;
    frwrd_action_profile_info.dest_info.is_oam_dest = Soc_pb_pp_trap_mgmt_action[idx].frwrd.info.is_oam_dest;
    frwrd_action_profile_info.dest_info.vsi_shift = Soc_pb_pp_trap_mgmt_action[idx].frwrd.info.vsi_shift;
    res = soc_pb_pp_trap_frwrd_profile_info_set_unsafe(
            unit,
            Soc_pb_pp_trap_mgmt_action[idx].trap_code,
            &frwrd_action_profile_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&frwrd_action_profile_info);
    snoop_action_profile_info.strength = Soc_pb_pp_trap_mgmt_action[idx].snoop.strength;
    snoop_action_profile_info.snoop_cmnd = Soc_pb_pp_trap_mgmt_action[idx].snoop.info.snoop_cmnd;
    res = soc_pb_pp_trap_snoop_profile_info_set_unsafe(
            unit,
            Soc_pb_pp_trap_mgmt_action[idx].trap_code,
            &snoop_action_profile_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mgmt_init_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_trap_mgmt_trap_code_to_internal(
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_CODE_INTERNAL                  *trap_code_internal,
    SOC_SAND_OUT SOC_PETRA_REG_FIELD                           **strength_fld_fwd,
    SOC_SAND_OUT SOC_PETRA_REG_FIELD                           **strength_fld_snp
  )
{
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PETRA_REG_FIELD
    *DUMMY_FIELD = NULL;
  int32
    diff;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_MGMT_TRAP_CODE_TO_INTERNAL);

  regs = soc_pb_pp_regs();

  switch (trap_code_ndx)
  {
  case  SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0;
    *strength_fld_fwd = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop0_fwd;
    *strength_fld_snp = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1;
    *strength_fld_fwd = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop1_fwd;
    *strength_fld_snp = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop1_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2;
    *strength_fld_fwd = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop2_fwd;
    *strength_fld_snp = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop2_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3;
    *strength_fld_fwd = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop3_fwd;
    *strength_fld_snp = &regs->ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop3_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_TE_TRANSPLANT:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT;
    *strength_fld_fwd = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_te_transplant_fwd;
    *strength_fld_snp = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_te_transplant_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL;
    *strength_fld_fwd = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_te_unknown_tunnel_fwd;
    *strength_fld_snp = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_te_unknown_tunnel_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_TRANSPLANT:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT;
    *strength_fld_fwd = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_transplant_fwd;
    *strength_fld_snp = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_transplant_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PBP_LEARN_SNOOP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP;
    *strength_fld_fwd = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_learn_snoop_fwd;
    *strength_fld_snp = &regs->ihp.pbp_action_profiles_reg.action_profile_pbp_learn_snoop_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_AUTHENTICATION_FAILED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED;
    *strength_fld_fwd = &regs->ihp.initial_vlan_action_profiles_reg.action_profile_sa_authentication_failed_fwd;
    *strength_fld_snp = &regs->ihp.initial_vlan_action_profiles_reg.action_profile_sa_authentication_failed_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PORT_NOT_PERMITTED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED;
    *strength_fld_fwd = &regs->ihp.initial_vlan_action_profiles_reg.action_profile_port_not_permitted_fwd;
    *strength_fld_snp = &regs->ihp.initial_vlan_action_profiles_reg.action_profile_port_not_permitted_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_UNEXPECTED_VID:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID;
    *strength_fld_fwd = &regs->ihp.initial_vlan_action_profiles_reg.action_profile_unexpected_vid_fwd;
    *strength_fld_snp = &regs->ihp.initial_vlan_action_profiles_reg.action_profile_unexpected_vid_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_MULTICAST:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST;
    *strength_fld_fwd = &regs->ihp.filtering_action_profiles_reg.action_profile_sa_multicast_fwd;
    *strength_fld_snp = &regs->ihp.filtering_action_profiles_reg.action_profile_sa_multicast_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_EQUALS_DA:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA;
    *strength_fld_fwd = &regs->ihp.filtering_action_profiles_reg.action_profile_sa_equals_da_fwd;
    *strength_fld_snp = &regs->ihp.filtering_action_profiles_reg.action_profile_sa_equals_da_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_8021X:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_8021X;
    *strength_fld_fwd = &regs->ihp.filtering_action_profiles_reg.action_profile_8021x_fwd;
    *strength_fld_snp = &regs->ihp.filtering_action_profiles_reg.action_profile_8021x_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0;
    *strength_fld_fwd = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_fwd[0];
    *strength_fld_snp = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_snp[0];
    break;
  case  SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1;
    *strength_fld_fwd = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_fwd[1];
    *strength_fld_snp = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_snp[1];
    break;
  case  SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2;
    *strength_fld_fwd = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_fwd[2];
    *strength_fld_snp = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_snp[2];
    break;
  case  SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3;
    *strength_fld_fwd = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_fwd[3];
    *strength_fld_snp = &regs->ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_snp[3];
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_ARP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_MY_ARP;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_arp_reg.action_profile_my_arp_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_arp_reg.action_profile_my_arp_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ARP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ARP;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_arp_reg.action_profile_arp_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_arp_reg.action_profile_arp_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_igmp_reg.action_profile_igmp_membership_query_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_igmp_reg.action_profile_igmp_membership_query_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_igmp_reg.action_profile_igmp_report_leave_msg_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_igmp_reg.action_profile_igmp_report_leave_msg_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IGMP_UNDEFINED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_igmp_reg.action_profile_igmp_undefined_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_igmp_reg.action_profile_igmp_undefined_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_mc_listener_query_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_mc_listener_query_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_report_done_msg_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_report_done_msg_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_UNDEFINED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_undefined_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_undefined_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DHCP_SERVER:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_server_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_server_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DHCP_CLIENT:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_client_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_client_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DHCPV6_SERVER:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_server_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_server_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DHCPV6_CLIENT:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT;
    *strength_fld_fwd = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_client_fwd;
    *strength_fld_snp = &regs->ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_client_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PROG_TRAP_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0;
    *strength_fld_fwd = &regs->ihp.general_trap_reg_4[0].general_trap_action_profile_fwd;
    *strength_fld_snp = &regs->ihp.general_trap_reg_4[0].general_trap_action_profile_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PROG_TRAP_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1;
    *strength_fld_fwd = &regs->ihp.general_trap_reg_4[1].general_trap_action_profile_fwd;
    *strength_fld_snp = &regs->ihp.general_trap_reg_4[1].general_trap_action_profile_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PROG_TRAP_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2;
    *strength_fld_fwd = &regs->ihp.general_trap_reg_4[2].general_trap_action_profile_fwd;
    *strength_fld_snp = &regs->ihp.general_trap_reg_4[2].general_trap_action_profile_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PROG_TRAP_3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3;
    *strength_fld_fwd = &regs->ihp.general_trap_reg_4[3].general_trap_action_profile_fwd;
    *strength_fld_snp = &regs->ihp.general_trap_reg_4[3].general_trap_action_profile_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_PORT_NOT_VLAN_MEMBER:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths0_reg.port_not_vlan_member_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths0_reg.port_not_vlan_member_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_HEADER_SIZE_ERR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths0_reg.header_size_err_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths0_reg.header_size_err_snp;
    break;
  case SOC_PB_PP_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths11_reg.header_size_err_over_mpls_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths11_reg.header_size_err_over_mpls_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths0_reg.my_bmac_and_learn_null_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths0_reg.my_bmac_and_learn_null_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths0_reg.my_bmac_unknown_isid_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths0_reg.my_bmac_unknown_isid_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_STP_STATE_BLOCK:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths1_reg.stp_state_block_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths1_reg.stp_state_block_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_STP_STATE_LEARN:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths1_reg.stp_state_learn_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths1_reg.stp_state_learn_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IP_COMP_MC_INVALID_IP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths2_reg.ip_comp_mc_invalid_ip_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths2_reg.ip_comp_mc_invalid_ip_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_MAC_AND_IP_DISABLE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths2_reg.my_mac_and_ip_disable_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths2_reg.my_mac_and_ip_disable_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_VERSION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths2_reg.trill_version_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths2_reg.trill_version_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_INVALID_TTL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths2_reg.trill_invalid_ttl_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths2_reg.trill_invalid_ttl_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_CHBH:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths3_reg.trill_chbh_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths3_reg.trill_chbh_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_NO_REVERSE_FEC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths3_reg.trill_no_reverse_fec_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths3_reg.trill_no_reverse_fec_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_CITE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths3_reg.trill_cite_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths3_reg.trill_cite_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths3_reg.trill_illegel_inner_mc_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths3_reg.trill_illegel_inner_mc_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths4_reg.my_mac_and_mpls_disable_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths4_reg.my_mac_and_mpls_disable_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_MAC_AND_ARP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths4_reg.my_mac_and_arp_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths4_reg.my_mac_and_arp_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths4_reg.my_mac_and_unknown_l3_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths4_reg.my_mac_and_unknown_l3_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value0_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value1_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value1_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value2_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value2_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value3_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths7_reg.mpls_label_value3_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_NO_RESOURCES:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths4_reg.mpls_no_resources_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths4_reg.mpls_no_resources_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_INVALID_LABEL_IN_RANGE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths5_reg.mpls_invalid_label_in_range_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths5_reg.mpls_invalid_label_in_range_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths8_reg.mpls_invalid_label_in_sem_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths8_reg.mpls_invalid_label_in_sem_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_LSP_BOS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths6_reg.mpls_lsp_bos_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths6_reg.mpls_lsp_bos_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_label14_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_label14_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_PWE_NO_BOS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_VRF_NO_BOS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths6_reg.mpls_vrf_no_bos_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths6_reg.mpls_vrf_no_bos_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_TERM_TTL_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths5_reg.mpls_ttl0_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths5_reg.mpls_ttl0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths6_reg.mpls_control_word_trap_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths6_reg.mpls_control_word_trap_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths6_reg.mpls_control_word_drop_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths6_reg.mpls_control_word_drop_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_VERSION_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths8_reg.ipv4_version_error_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths8_reg.ipv4_version_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths8_reg.ipv4_checksum_error_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths8_reg.ipv4_checksum_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths8_reg.ipv4_header_length_error_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths8_reg.ipv4_header_length_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths9_reg.ipv4_total_length_error_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths9_reg.ipv4_total_length_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_TTL0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths9_reg.ipv4_ttl0_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths9_reg.ipv4_ttl0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_HAS_OPTIONS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths9_reg.ipv4_has_options_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths9_reg.ipv4_has_options_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_TTL1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths9_reg.ipv4_ttl1_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths9_reg.ipv4_ttl1_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths10_reg.ipv4_sip_equal_dip_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths10_reg.ipv4_sip_equal_dip_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_DIP_ZERO:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths10_reg.ipv4_dip_zero_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths10_reg.ipv4_dip_zero_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths10_reg.ipv4_fragmented_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths10_reg.ipv4_fragmented_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TERM_SIP_IS_MC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths10_reg.ipv4_sip_is_mc_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths10_reg.ipv4_sip_is_mc_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_CFM_ACCELERATED_INGRESS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths1_reg.cfm_accelarated_ingress_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths1_reg.cfm_accelarated_ingress_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ILLEGEL_PFC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC;
    *strength_fld_fwd = &regs->ihp.vtt_trap_strengths1_reg.illegal_pfc_fwd;
    *strength_fld_snp = &regs->ihp.vtt_trap_strengths1_reg.illegal_pfc_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_DROP_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP0;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[0];
    *strength_fld_snp = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[0];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_DROP_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP1;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[1];
    *strength_fld_snp = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[1];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_DROP_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP2;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[2];
    *strength_fld_snp = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[2];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_DROP_3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP3;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[3];
    *strength_fld_snp = &regs->ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[3];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[0];
    *strength_fld_snp = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[0];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[1];
    *strength_fld_snp = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[1];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[2];
    *strength_fld_snp = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[2];
    break;
  case  SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3;
    *strength_fld_fwd = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[3];
    *strength_fld_snp = &regs->ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[3];
    break;
  case  SOC_PB_PP_TRAP_CODE_ELK_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR;
    *strength_fld_fwd = &regs->ihb.flp_general_cfg_reg.action_profile_elk_error_fwd;
    *strength_fld_snp = &regs->ihb.flp_general_cfg_reg.action_profile_elk_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND0;
    *strength_fld_fwd = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[0];
    *strength_fld_snp = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[0];
    break;
  case  SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND1;
    *strength_fld_fwd = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[1];
    *strength_fld_snp = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[1];
    break;
  case  SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_2:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND2;
    *strength_fld_fwd = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[2];
    *strength_fld_snp = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[2];
    break;
  case  SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_3:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND3;
    *strength_fld_fwd = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[3];
    *strength_fld_snp = &regs->ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[3];
    break;
  case  SOC_PB_PP_TRAP_CODE_P2P_MISCONFIGURATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION;
    *strength_fld_fwd = &regs->ihb.p2p_cfg_reg.action_profile_p2p_misconfiguration_fwd;
    *strength_fld_snp = &regs->ihb.p2p_cfg_reg.action_profile_p2p_misconfiguration_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_SAME_INTERFACE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE;
    *strength_fld_fwd = &regs->ihb.action_profile_same_interface_reg.action_profile_same_interface_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_same_interface_reg.action_profile_same_interface_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_UNKNOWN_UC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC;
    *strength_fld_fwd = &regs->ihb.trill_unknown_reg.action_profile_trill_unknown_uc_fwd;
    *strength_fld_snp = &regs->ihb.trill_unknown_reg.action_profile_trill_unknown_uc_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TRILL_UNKNOWN_MC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC;
    *strength_fld_fwd = &regs->ihb.trill_unknown_reg.action_profile_trill_unknown_mc_fwd;
    *strength_fld_snp = &regs->ihb.trill_unknown_reg.action_profile_trill_unknown_mc_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_UC_LOOSE_RPF_FAIL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL;
    *strength_fld_fwd = &regs->ihb.ipv4_cfg_reg.action_profile_uc_loose_rpf_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_cfg_reg.action_profile_uc_loose_rpf_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DEFAULT_UCV6:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
    *strength_fld_fwd = &regs->ihb.ipv6_cfg_reg.action_profile_default_ucv6_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_cfg_reg.action_profile_default_ucv6_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_DEFAULT_MCV6:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
    *strength_fld_fwd = &regs->ihb.ipv6_cfg_reg.action_profile_default_mcv6_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_cfg_reg.action_profile_default_mcv6_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_P2P_NO_BOS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS;
    *strength_fld_fwd = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_p2p_no_bos_fwd;
    *strength_fld_snp = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_p2p_no_bos_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP;
    *strength_fld_fwd = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_control_word_trap_fwd;
    *strength_fld_snp = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_control_word_trap_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_CONTROL_WORD_DROP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP;
    *strength_fld_fwd = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_control_word_drop_fwd;
    *strength_fld_snp = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_control_word_drop_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_UNKNOWN_LABEL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL;
    *strength_fld_fwd = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_unknown_label_fwd;
    *strength_fld_snp = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_unknown_label_snp;
    break;
    case  SOC_PB_PP_TRAP_CODE_MPLS_P2P_MPLSX4:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4;
    *strength_fld_fwd = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_p2p_mplsx4_fwd;
    *strength_fld_snp = &regs->ihb.lsr_action_profile_reg.action_profile_mpls_p2p_mplsx4_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_L2CP_PEER:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_peer_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_peer_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_L2CP_DROP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_drop_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_drop_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles1_reg.igmp_membership_query_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles1_reg.igmp_membership_query_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles1_reg.igmp_report_leave_msg_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles1_reg.igmp_report_leave_msg_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles1_reg.igmp_undefined_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles1_reg.igmp_undefined_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles1_reg.icmpv6_mld_mc_listener_query_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles1_reg.icmpv6_mld_mc_listener_query_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles1_reg.icmpv6_mld_report_done_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles1_reg.icmpv6_mld_report_done_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED;
    *strength_fld_fwd = &regs->ihb.ethernet_action_profiles1_reg.icmpv6_mld_undefined_fwd;
    *strength_fld_snp = &regs->ihb.ethernet_action_profiles1_reg.icmpv6_mld_undefined_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_VERSION_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_version_error_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_version_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_CHECKSUM_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_checksum_version_error_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_checksum_version_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_header_length_error_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_header_length_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_total_length_error_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_total_length_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TTL0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_ttl0_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_ttl0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_HAS_OPTIONS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_has_options_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles0_reg.action_profile_ipv4_has_options_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_TTL1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_ttl1_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_ttl1_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_SIP_EQUAL_DIP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_equal_dip_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_equal_dip_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_DIP_ZERO:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_dip_zero_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_dip_zero_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV4_SIP_IS_MC:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC;
    *strength_fld_fwd = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_is_mc_fwd;
    *strength_fld_snp = &regs->ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_is_mc_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_VERSION_ERROR:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_version_error_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_version_error_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_HOP_COUNT0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count0_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_HOP_COUNT1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count1_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count1_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_unspecified_destination_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_unspecified_destination_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_loopback_address_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_loopback_address_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_MULTICAST_SOURCE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_multicast_source_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles0_reg.action_profile_ipv6_multicast_source_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_NEXT_HEADER_NULL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_next_header_null_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_next_header_null_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_unspecified_source_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_unspecified_source_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_destination_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_destination_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_destination_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_destination_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_source_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_source_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_source_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_source_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_compatible_destination_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_compatible_destination_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_mapped_destination_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_mapped_destination_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_IPV6_MULTICAST_DESTINATION:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION;
    *strength_fld_fwd = &regs->ihb.ipv6_action_profiles2_reg.action_profile_ipv6_multicast_destination_fwd;
    *strength_fld_snp = &regs->ihb.ipv6_action_profiles2_reg.action_profile_ipv6_multicast_destination_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_TTL0:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0;
    *strength_fld_fwd = &regs->ihb.mpls_action_profiles_reg.action_profile_mpls_ttl0_fwd;
    *strength_fld_snp = &regs->ihb.mpls_action_profiles_reg.action_profile_mpls_ttl0_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MPLS_TTL1:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1;
    *strength_fld_fwd = &regs->ihb.mpls_action_profiles_reg.action_profile_mpls_ttl1_fwd;
    *strength_fld_snp = &regs->ihb.mpls_action_profiles_reg.action_profile_mpls_ttl1_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TCP_SN_FLAGS_ZERO:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO;
    *strength_fld_fwd = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_sn_flags_zero_fwd;
    *strength_fld_snp = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_sn_flags_zero_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET;
    *strength_fld_fwd = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_sn_zero_flags_set_fwd;
    *strength_fld_snp = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_sn_zero_flags_set_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TCP_SYN_FIN:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN;
    *strength_fld_fwd = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_syn_fin_fwd;
    *strength_fld_snp = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_syn_fin_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TCP_EQUAL_PORTS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS;
    *strength_fld_fwd = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_equal_ports_fwd;
    *strength_fld_snp = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_equal_ports_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER;
    *strength_fld_fwd = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_incomplete_header_fwd;
    *strength_fld_snp = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_incomplete_header_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8;
    *strength_fld_fwd = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_offset_lt8_fwd;
    *strength_fld_snp = &regs->ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_offset_lt8_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_UDP_EQUAL_PORTS:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS;
    *strength_fld_fwd = &regs->ihb.udp_action_profiles_reg.action_profile_udp_equal_ports_fwd;
    *strength_fld_snp = &regs->ihb.udp_action_profiles_reg.action_profile_udp_equal_ports_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ICMP_DATA_GT_576:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576;
    *strength_fld_fwd = &regs->ihb.icmp_action_profiles_reg.action_profile_icmp_data_gt_576_fwd;
    *strength_fld_snp = &regs->ihb.icmp_action_profiles_reg.action_profile_icmp_data_gt_576_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ICMP_FRAGMENTED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED;
    *strength_fld_fwd = &regs->ihb.icmp_action_profiles_reg.action_profile_icmp_fragmented_fwd;
    *strength_fld_snp = &regs->ihb.icmp_action_profiles_reg.action_profile_icmp_fragmented_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_GENERAL: 
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_PMF_GENERAL;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;
  case  SOC_PB_PP_TRAP_CODE_FACILITY_INVALID:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID;
    *strength_fld_fwd = &regs->ihb.action_profile_general_reg.action_profile_facility_invalid_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_general_reg.action_profile_facility_invalid_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_FEC_ENTRY_ACCESSED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED;
    *strength_fld_fwd = &regs->ihb.action_profile_general_reg.action_profile_fec_entry_accessed_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_general_reg.action_profile_fec_entry_accessed_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_UC_STRICT_RPF_FAIL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL;
    *strength_fld_fwd = &regs->ihb.action_profile_uc_rpf_reg.action_profile_uc_strict_rpf_fail_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_uc_rpf_reg.action_profile_uc_strict_rpf_fail_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL;
    *strength_fld_fwd = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_explicit_rpf_fail_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_explicit_rpf_fail_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL;
    *strength_fld_fwd = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_as_is_rpf_fail_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_as_is_rpf_fail_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL;
    *strength_fld_fwd = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_rpf_fail_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_rpf_fail_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_MC_USE_SIP_ECMP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP;
    *strength_fld_fwd = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_ecmp_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_ecmp_snp;
    break;
  case  SOC_PB_PP_TRAP_CODE_ICMP_REDIRECT:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT;
    *strength_fld_fwd = &regs->ihb.action_profile_general_reg.action_profile_icmp_redirect_fwd;
    *strength_fld_snp = &regs->ihb.action_profile_general_reg.action_profile_icmp_redirect_snp;
    break;

  case  SOC_PB_PP_TRAP_CODE_USER_OAMP:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_OAMP;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;
  case  SOC_PB_PP_TRAP_CODE_USER_ETH_OAM_ACCELERATED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;
  case  SOC_PB_PP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;
  case  SOC_PB_PP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;
  case  SOC_PB_PP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;
  case  SOC_PB_PP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED:
    *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED;
    *strength_fld_fwd = DUMMY_FIELD;
    *strength_fld_snp = DUMMY_FIELD;
    break;

  default:
    /* SOC_PB_PP_TRAP_CODE_RESERVED_MC_0-7 */
    if ((trap_code_ndx >= SOC_PB_PP_TRAP_CODE_RESERVED_MC_0) && (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_RESERVED_MC_7))
    {
      diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_RESERVED_MC_0);
      *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX + diff;
      
      /* fwd/snp strength for reserved_mc is not per trap_code_ndx,
         but per reserved_mc DA and trap profile (see reserved mc table) */
      *strength_fld_fwd = NULL;
      *strength_fld_snp = NULL;
    }
    /* SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0-7 */
    else if ((trap_code_ndx >= SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0) && (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_7))
    {
      diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0);
      *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX + diff;
        
      *strength_fld_fwd = NULL;
      *strength_fld_snp = NULL;
    }
    /* SOC_PB_PP_TRAP_CODE_USER_DEFINED_0-63 */
    else if ((trap_code_ndx >= SOC_PB_PP_TRAP_CODE_USER_DEFINED_0) && (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_59))
    {
      *strength_fld_fwd = NULL;
      *strength_fld_snp = NULL;

      if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_5)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_0);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_7)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_6);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_10)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_8);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_12)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_11);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_27)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_13);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_35)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_28);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_51)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_36);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51 + diff;
      }
      else if (trap_code_ndx <= SOC_PB_PP_TRAP_CODE_USER_DEFINED_59)
      {
        diff = (trap_code_ndx - SOC_PB_PP_TRAP_CODE_USER_DEFINED_52);
        *trap_code_internal = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_60 + diff;
      }
      else
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR, 95, exit);
      }
    }
    else /* Unknown */
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR, 100, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mgmt_trap_code_to_internal()", trap_code_ndx, 0);
}

/************************************************************************
*   Convert CPU trap code as received on packet to API enumerator.     *
************************************************************************/
uint32
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int32                                 trap_code_internal,
    SOC_SAND_OUT SOC_PB_PP_TRAP_CODE                          *trap_code
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_CPU_TRAP_CODE_FROM_INTERNAL_UNSAFE);

  switch (trap_code_internal)
  {
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_TE_TRANSPLANT;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_TRANSPLANT;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP:
    *trap_code = SOC_PB_PP_TRAP_CODE_PBP_LEARN_SNOOP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_AUTHENTICATION_FAILED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED:
    *trap_code = SOC_PB_PP_TRAP_CODE_PORT_NOT_PERMITTED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID:
    *trap_code = SOC_PB_PP_TRAP_CODE_UNEXPECTED_VID;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_MULTICAST;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_EQUALS_DA;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_8021X:
    *trap_code = SOC_PB_PP_TRAP_CODE_8021X;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0:
    *trap_code = SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1:
    *trap_code = SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2:
    *trap_code = SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3:
    *trap_code = SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_MY_ARP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_ARP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ARP:
    *trap_code = SOC_PB_PP_TRAP_CODE_ARP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY:
    *trap_code = SOC_PB_PP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG:
    *trap_code = SOC_PB_PP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED:
    *trap_code = SOC_PB_PP_TRAP_CODE_IGMP_UNDEFINED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code = SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG:
    *trap_code = SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED:
    *trap_code = SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_UNDEFINED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER:
    *trap_code = SOC_PB_PP_TRAP_CODE_DHCP_SERVER;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT:
    *trap_code = SOC_PB_PP_TRAP_CODE_DHCP_CLIENT;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER:
    *trap_code = SOC_PB_PP_TRAP_CODE_DHCPV6_SERVER;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT:
    *trap_code = SOC_PB_PP_TRAP_CODE_DHCPV6_CLIENT;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0:
    *trap_code = SOC_PB_PP_TRAP_CODE_PROG_TRAP_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1:
    *trap_code = SOC_PB_PP_TRAP_CODE_PROG_TRAP_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2:
    *trap_code = SOC_PB_PP_TRAP_CODE_PROG_TRAP_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3:
    *trap_code = SOC_PB_PP_TRAP_CODE_PROG_TRAP_3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER:
    *trap_code = SOC_PB_PP_TRAP_CODE_PORT_NOT_VLAN_MEMBER;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR:
    *trap_code = SOC_PB_PP_TRAP_CODE_HEADER_SIZE_ERR;
    break;
  case SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS:
    *trap_code = SOC_PB_PP_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK:
    *trap_code = SOC_PB_PP_TRAP_CODE_STP_STATE_BLOCK;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN:
    *trap_code = SOC_PB_PP_TRAP_CODE_STP_STATE_LEARN;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP:
    *trap_code = SOC_PB_PP_TRAP_CODE_IP_COMP_MC_INVALID_IP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_MAC_AND_IP_DISABLE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_VERSION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_INVALID_TTL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_CHBH;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_NO_REVERSE_FEC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_CITE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_MAC_AND_ARP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3:
    *trap_code = SOC_PB_PP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_NO_RESOURCES;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE:
    *trap_code = SOC_PB_PP_TRAP_CODE_INVALID_LABEL_IN_RANGE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LSP_BOS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_PWE_NO_BOS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_VRF_NO_BOS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_TERM_TTL_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_VERSION_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_TTL0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_HAS_OPTIONS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_TTL1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_DIP_ZERO;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TERM_SIP_IS_MC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS:
    *trap_code = SOC_PB_PP_TRAP_CODE_CFM_ACCELERATED_INGRESS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC:
    *trap_code = SOC_PB_PP_TRAP_CODE_ILLEGEL_PFC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP0:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_DROP_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP1:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_DROP_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP2:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_DROP_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP3:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_DROP_3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3:
    *trap_code = SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_ELK_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND0:
    *trap_code = SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND1:
    *trap_code = SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND2:
    *trap_code = SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_2;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND3:
    *trap_code = SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_3;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_P2P_MISCONFIGURATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE:
    *trap_code = SOC_PB_PP_TRAP_CODE_SAME_INTERFACE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_UNKNOWN_UC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC:
    *trap_code = SOC_PB_PP_TRAP_CODE_TRILL_UNKNOWN_MC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL:
    *trap_code = SOC_PB_PP_TRAP_CODE_UC_LOOSE_RPF_FAIL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6:
    *trap_code = SOC_PB_PP_TRAP_CODE_DEFAULT_UCV6;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6:
    *trap_code = SOC_PB_PP_TRAP_CODE_DEFAULT_MCV6;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_P2P_NO_BOS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_CONTROL_WORD_DROP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_UNKNOWN_LABEL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_P2P_MPLSX4;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_L2CP_PEER;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_L2CP_DROP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED:
    *trap_code = SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_VERSION_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_CHECKSUM_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TTL0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_HAS_OPTIONS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_TTL1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_SIP_EQUAL_DIP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_DIP_ZERO;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV4_SIP_IS_MC;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_VERSION_ERROR;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_HOP_COUNT0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_HOP_COUNT1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_MULTICAST_SOURCE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_NEXT_HEADER_NULL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION:
    *trap_code = SOC_PB_PP_TRAP_CODE_IPV6_MULTICAST_DESTINATION;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_TTL0;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1:
    *trap_code = SOC_PB_PP_TRAP_CODE_MPLS_TTL1;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO:
    *trap_code = SOC_PB_PP_TRAP_CODE_TCP_SN_FLAGS_ZERO;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET:
    *trap_code = SOC_PB_PP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN:
    *trap_code = SOC_PB_PP_TRAP_CODE_TCP_SYN_FIN;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS:
    *trap_code = SOC_PB_PP_TRAP_CODE_TCP_EQUAL_PORTS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER:
    *trap_code = SOC_PB_PP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8:
    *trap_code = SOC_PB_PP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS:
    *trap_code = SOC_PB_PP_TRAP_CODE_UDP_EQUAL_PORTS;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576:
    *trap_code = SOC_PB_PP_TRAP_CODE_ICMP_DATA_GT_576;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED:
    *trap_code = SOC_PB_PP_TRAP_CODE_ICMP_FRAGMENTED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_PMF_GENERAL:
    *trap_code = SOC_PB_PP_TRAP_CODE_GENERAL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID:
    *trap_code = SOC_PB_PP_TRAP_CODE_FACILITY_INVALID;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED:
    *trap_code = SOC_PB_PP_TRAP_CODE_FEC_ENTRY_ACCESSED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL:
    *trap_code = SOC_PB_PP_TRAP_CODE_UC_STRICT_RPF_FAIL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL:
    *trap_code = SOC_PB_PP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL:
    *trap_code = SOC_PB_PP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL:
    *trap_code = SOC_PB_PP_TRAP_CODE_MC_USE_SIP_RPF_FAIL;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP:
    *trap_code = SOC_PB_PP_TRAP_CODE_MC_USE_SIP_ECMP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT:
    *trap_code = SOC_PB_PP_TRAP_CODE_ICMP_REDIRECT;
    break;

  case  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_OAMP:
    *trap_code = SOC_PB_PP_TRAP_CODE_USER_OAMP;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED:
    *trap_code = SOC_PB_PP_TRAP_CODE_USER_ETH_OAM_ACCELERATED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED:
    *trap_code = SOC_PB_PP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    *trap_code = SOC_PB_PP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED:
    *trap_code = SOC_PB_PP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED;
    break;
  case  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED:
    *trap_code = SOC_PB_PP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED;
    break;

  default:
    /* SOC_PB_PP_TRAP_CODE_RESERVED_MC_0-7 */
    if ((SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX <= trap_code_internal) &&
        (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX + SOC_PB_PP_TRAP_CODE_RESERVED_MC_7 - SOC_PB_PP_TRAP_CODE_RESERVED_MC_0))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_RESERVED_MC_0 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX;
    }
    /* SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0-7 */
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX + 7))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX;
    }
    /* SOC_PB_PP_TRAP_CODE_USER_DEFINED_0-63 */
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_5 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_0))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5;
    }
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_7 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_6))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_6 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7;
    }

    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_10 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_8))
    {
      /* Petra b code. Almost not in use. Ignore coverity defects */
      /* coverity[dead_error_line] */
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_8 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10;
    }
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_12 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_11))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_11 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12;
    }
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_27 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_13))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_13 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27;
    }
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_35 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_28))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_28 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35;
    }
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51 + SOC_PB_PP_TRAP_CODE_USER_DEFINED_51 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_36))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_36 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51;
    }
    else if ((SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_60 <= trap_code_internal) &&
      (trap_code_internal <= SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_60 + 15 /* SOC_PB_PP_TRAP_CODE_USER_DEFINED_67 - SOC_PB_PP_TRAP_CODE_USER_DEFINED_52 */))
    {
      *trap_code = SOC_PB_PP_TRAP_CODE_USER_DEFINED_52 + trap_code_internal - SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_60;
    }
    /* Unknown */
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR, 100, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mgmt_trap_code_from_internal()", trap_code_internal, 0);
}


uint32
  soc_pb_pp_trap_sb_vtt_to_internal_trap_code_map_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN   uint32            sb_trap_indx,
    SOC_SAND_OUT  uint32            *internal_trap_code
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_SB_VTT_TO_INTERNAL_TRAP_CODE_MAP_GET);
  SOC_SAND_CHECK_NULL_INPUT(internal_trap_code);
  
  switch(sb_trap_indx)
  {
  case 0:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR;
    break;
  case 1:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC;
    break;
  case 2:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER;
    break;
  case 3:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS    /* lookup is trap! */;
    break;
  case 4:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID;
    break;
  case 5:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL;
    break;
  case 6:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK;
    break;
  case 7:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN;
    break;
  case 8:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS;
    break;
  case 9:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION;
    break;
  case 10:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL;
    break;
  case 11:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH;
    break;
  case 12:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC;
    break;
  case 13:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE;
    break;
  case 14:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC;
    break;
  case 15:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP;
    break;
  case 16:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE;
    break;
  case 17:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE;
    break;
  case 18:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP;
    break;
  case 19:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3;
    break;
  case 20:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0;
    break;
  case 21:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1;
    break;
  case 22:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2;
    break;
  case 23:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3;
    break;
  case 24:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES;
    break;
  case 25:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE;
    break;
  case 26:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM;
    break;
  case 27:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS;
    break;
  case 28:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14;
    break;
  case 29:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS;
    break;
  case 30:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS;
    break;
  case 31:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0;
    break;
  case 32:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP;
    break;
  case 33:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP;
    break;
  case 34:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR;
    break;
  case 35:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR;
    break;
  case 36:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR;
    break;
  case 37:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR;
    break;
  case 38:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0;
    break;
  case 39:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1;
    break;
  case 40:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS;
    break;
  case 41:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP;
    break;
  case 42:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO;
    break;
  case 43:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC;
    break;
  case 44:
    *internal_trap_code = SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    break;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_trap_sb_vtt_to_internal_trap_code_map_get()",0,0);
}

uint32
  soc_pb_pp_trap_sb_to_trap_code_map_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN   uint32                   rng_num,
    SOC_SAND_IN   uint32                   sb_trap_indx,
    SOC_SAND_OUT  SOC_PB_PP_TRAP_CODE            *trap_code
  )
{
  uint32
    res;
  uint32
    inter_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_SB_TO_TRAP_CODE_MAP_GET);
  SOC_SAND_CHECK_NULL_INPUT(trap_code);

  switch(rng_num)
  {
    case 0:/* llr*/
      inter_val = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + sb_trap_indx;
    break;
    case 1:
      res = soc_pb_pp_trap_sb_vtt_to_internal_trap_code_map_get(unit,sb_trap_indx,&inter_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
    case 2:
      inter_val = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + sb_trap_indx;
    break;
    default:
    case 3:
      inter_val = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX + sb_trap_indx;
    break;
  }
  res = soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(
          unit,
          inter_val,
          trap_code
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_trap_sb_to_trap_code_map_get()",0,0);
}
/*********************************************************************
*     Fill information required to parse packet arriving to CPU.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_pb_pp_trap_packet_parse_info_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO  *packet_parse_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_MGMT_OPERATION_MODE
    op_mode;
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    reg_val,
    ftmh_ext_outlif,
    ftmh_lb_key_enable,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_PACKET_PARSE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(packet_parse_info);

  res = soc_pb_mgmt_operation_mode_get_unsafe(
          unit,
          &op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  pp_regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(pp_regs->eci.system_headers_configurations0_reg, reg_val, 35, exit);
  SOC_PB_PP_FLD_FROM_REG(pp_regs->eci.system_headers_configurations0_reg.ftmh_ext,ftmh_ext_outlif, reg_val, 40, exit);

  packet_parse_info->dest_ext_mode =
    (ftmh_ext_outlif == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_ALWAYS ?
       SOC_PPC_TRAP_MGMT_FTMH_DEST_EXT_MODE_ALWAYS :
       ftmh_ext_outlif == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC ?
       SOC_PPC_TRAP_MGMT_FTMH_DEST_EXT_MODE_MC :
       SOC_PPC_TRAP_MGMT_FTMH_DEST_EXT_MODE_NEVER);

  SOC_PB_PP_FLD_FROM_REG(pp_regs->eci.system_headers_configurations0_reg.ftmh_lb_key_ext_enable, ftmh_lb_key_enable, reg_val, 40, exit);
  packet_parse_info->lb_ext_en =(uint8)ftmh_lb_key_enable;

  /* if eep extension exist, then soc_petra-B, note soc_petra-b is used to know if there is EEP extension! */
  SOC_PB_PP_FLD_FROM_REG(pp_regs->eci.system_headers_configurations0_reg.add_pph_eep_ext, fld_val, reg_val, 40, exit);
  packet_parse_info->pph_eep_ext = (uint8)fld_val;

  res = soc_petra_pkt_packet_receive_mode_get_unsafe(&(packet_parse_info->recv_mode));
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in trap_packet_parse_info_get_unsafe()",0,0);
}

/*********************************************************************
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_frwrd_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fwd_dest_buff,
    fwd_asd_buff;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
          trap_code_ndx,
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_fwd_decision_in_buffer_build(
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRAP,
          &profile_info->dest_info.frwrd_dest,
          FALSE,
          &fwd_dest_buff,
          &fwd_asd_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  fwd_act_profile_tbl_data.fwd_act_trap =
    SOC_SAND_BOOL2NUM(profile_info->processing_info.is_trap);
  fwd_act_profile_tbl_data.fwd_act_control =
    SOC_SAND_BOOL2NUM(profile_info->processing_info.is_control);
  fwd_act_profile_tbl_data.fwd_act_destination =
    fwd_dest_buff;
  fwd_act_profile_tbl_data.fwd_act_destination_valid =
    ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_destination_add_vsi =
    SOC_SAND_BOOL2NUM(profile_info->dest_info.add_vsi);
  fwd_act_profile_tbl_data.fwd_act_destination_add_vsi_shift =
    profile_info->dest_info.vsi_shift;
  fwd_act_profile_tbl_data.fwd_act_traffic_class =
    profile_info->cos_info.tc;
  fwd_act_profile_tbl_data.fwd_act_traffic_class_valid =
    ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_drop_precedence =
    profile_info->cos_info.dp;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_valid =
    ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_meter_pointer =
    profile_info->meter_info.meter_id;
  fwd_act_profile_tbl_data.fwd_act_meter_pointer_selector =
    profile_info->meter_info.meter_select;
  fwd_act_profile_tbl_data.fwd_act_counter_pointer =
    profile_info->count_info.counter_id;
  fwd_act_profile_tbl_data.fwd_act_counter_pointer_selector =
    profile_info->count_info.counter_select;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command =
    profile_info->meter_info.meter_command;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command_valid =
    ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_fwd_offset_index =
    profile_info->processing_info.frwrd_offset_index;
  fwd_act_profile_tbl_data.fwd_act_fwd_offset_index_valid =
    ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer =
    profile_info->policing_info.ethernet_police_id;
  fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer_valid =
    ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_POLICER) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_learn_disable =
    SOC_SAND_BOOL2NUM_INVERSE(profile_info->processing_info.enable_learning);
  
  res = soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
          unit,
          trap_code_internal,
          &fwd_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (strength_fld_fwd)
  {
    SOC_PB_PP_FLD_SET(*strength_fld_fwd, profile_info->strength, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_frwrd_profile_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_trap_frwrd_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_frwrd_profile_info_set_verify()", trap_code_ndx, 0);
}

uint32
  soc_pb_pp_trap_frwrd_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_frwrd_profile_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_frwrd_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;
  uint8
    is_sa_drop;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(profile_info);

  res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
    trap_code_ndx,
    &trap_code_internal,
    &strength_fld_fwd,
    &strength_fld_snp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
    unit,
    trap_code_internal,
    &fwd_act_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  fld_val = 0;
  if (strength_fld_fwd)
  {
    SOC_PB_PP_FLD_GET(*strength_fld_fwd, fld_val, 30, exit);
  }

  profile_info->strength = fld_val;

  res = soc_pb_pp_fwd_decision_in_buffer_parse(
    SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRAP,
    fwd_act_profile_tbl_data.fwd_act_destination,
    0,
    &profile_info->dest_info.frwrd_dest,
    &is_sa_drop
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  profile_info->bitmap_mask = 0;

  profile_info->processing_info.is_trap =
    SOC_SAND_NUM2BOOL(fwd_act_profile_tbl_data.fwd_act_trap);
  profile_info->processing_info.is_control =
    SOC_SAND_NUM2BOOL(fwd_act_profile_tbl_data.fwd_act_control);
  if (fwd_act_profile_tbl_data.fwd_act_destination_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  }
  profile_info->dest_info.add_vsi =
    SOC_SAND_NUM2BOOL(fwd_act_profile_tbl_data.fwd_act_destination_add_vsi);
  profile_info->dest_info.vsi_shift =
    fwd_act_profile_tbl_data.fwd_act_destination_add_vsi_shift;
  profile_info->cos_info.tc =
    (SOC_SAND_PP_TC)fwd_act_profile_tbl_data.fwd_act_traffic_class;
  if (fwd_act_profile_tbl_data.fwd_act_traffic_class_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC;
  }
  profile_info->cos_info.dp =
    (SOC_SAND_PP_DP)fwd_act_profile_tbl_data.fwd_act_drop_precedence;
  if (fwd_act_profile_tbl_data.fwd_act_drop_precedence_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP;
  }
  profile_info->meter_info.meter_id =
    fwd_act_profile_tbl_data.fwd_act_meter_pointer;
  profile_info->meter_info.meter_select =
    fwd_act_profile_tbl_data.fwd_act_meter_pointer_selector;
  profile_info->count_info.counter_id =
    fwd_act_profile_tbl_data.fwd_act_counter_pointer;
  profile_info->count_info.counter_select =
    fwd_act_profile_tbl_data.fwd_act_counter_pointer_selector;
  profile_info->meter_info.meter_command =
    fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command;
  if (fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD;
  }
  profile_info->processing_info.frwrd_offset_index =
    fwd_act_profile_tbl_data.fwd_act_fwd_offset_index;
  if (fwd_act_profile_tbl_data.fwd_act_fwd_offset_index_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;
  }
  profile_info->policing_info.ethernet_police_id =
    fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer;
  if (fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_POLICER;
  }
  profile_info->processing_info.enable_learning =
    SOC_SAND_NUM2BOOL_INVERSE(fwd_act_profile_tbl_data.fwd_act_learn_disable);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_frwrd_profile_info_get_unsafe()", trap_code_ndx, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_snoop_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA
    tbl_data;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
    trap_code_ndx,
    &trap_code_internal,
    &strength_fld_fwd,
    &strength_fld_snp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.snoop_action = profile_info->snoop_cmnd;

  res = soc_pb_pp_ihb_snoop_action_tbl_set_unsafe(
    unit,
    trap_code_internal,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (strength_fld_snp)
  {
    SOC_PB_PP_FLD_SET(*strength_fld_snp, profile_info->strength, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_snoop_profile_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_trap_snoop_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_snoop_profile_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_trap_snoop_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_snoop_profile_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_snoop_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA
    tbl_data;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(profile_info);

  res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
    trap_code_ndx,
    &trap_code_internal,
    &strength_fld_fwd,
    &strength_fld_snp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihb_snoop_action_tbl_get_unsafe(
    unit,
    trap_code_internal,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  profile_info->snoop_cmnd = tbl_data.snoop_action;

  fld_val = 0;
  if (strength_fld_snp)
  {
    SOC_PB_PP_FLD_GET(*strength_fld_snp, fld_val, 30, exit);
  }

  profile_info->strength = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_snoop_profile_info_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_to_eg_action_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                  eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    valid,
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_REGS
    *tm_regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_UNSAFE);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();


  valid = (eg_action_profile == SOC_PB_PP_TRAP_EG_NO_ACTION) ? 0 : 1;

  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_NO_VSI_TRANSLATION)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_no_vsi_translation, fld_val, 10, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_VSI_MEMBERSHIP)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_vlan_membership, fld_val, 20, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_ACC_FRM)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_unacceptable_frame_type, fld_val, 30, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_HAIR_PIN)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_src_equal_dest, fld_val, 40, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_UNKNOWN_DA)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_unknown_da, fld_val, 50, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_SPLIT_HORIZON)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_split_horizon, fld_val, 60, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_PRIVATE_VLAN)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_private_vlan, fld_val, 70, exit);
  }

  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TTL_SCOPE)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_ttl_scope, fld_val, 80, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_MTU_VIOLATION)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_mtu_violation, fld_val, 90, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TRILL_TTL_0)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_trill_ttl_zero, fld_val, 100, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TRILL_SAME_INTERFACE)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile2_reg.action_profile_trill_same_interface, fld_val, 110, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TRILL_BOUNCE_BACK)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile3_reg.action_profile_bounce_back, fld_val, 120, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_DSS_STACKING)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_dss_stacking , fld_val, 130, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_LAG_MULTICAST)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_lag_multicast, fld_val, 140, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_EXCLUDE_SRC)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_exclude_src, fld_val, 150, exit);
  }
  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_CFM_TRAP)
  {
    fld_val = SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->egq.cnm_general_config_reg.action_profile_cfm_trap, fld_val, 106, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_to_eg_action_map_set_unsafe()", trap_type_bitmap_ndx, eg_action_profile);
}

uint32
  soc_pb_pp_trap_to_eg_action_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                  eg_action_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_VERIFY);

  if (eg_action_profile != SOC_PB_PP_TRAP_EG_NO_ACTION)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(eg_action_profile, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_to_eg_action_map_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_trap_to_eg_action_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_VERIFY);

  if (soc_sand_bitstream_get_nof_on_bits(&trap_type_bitmap_ndx, 1) != 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_to_eg_action_map_get_verify()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_to_eg_action_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                  *eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    valid,
    fld_val,
    eg_action_profile_from_fld;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_REGS
    *tm_regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_UNSAFE);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();

  valid = 0;
  eg_action_profile_from_fld = 0;

  if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_NO_VSI_TRANSLATION)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile1_reg.action_profile_no_vsi_translation, fld_val, 10, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_VSI_MEMBERSHIP)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile1_reg.action_profile_vlan_membership, fld_val, 20, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_ACC_FRM)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile1_reg.action_profile_unacceptable_frame_type, fld_val, 30, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_HAIR_PIN)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_src_equal_dest, fld_val, 40, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_UNKNOWN_DA)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_unknown_da, fld_val, 50, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_SPLIT_HORIZON)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_split_horizon, fld_val, 60, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_PRIVATE_VLAN)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_private_vlan, fld_val, 70, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TTL_SCOPE)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_ttl_scope, fld_val, 80, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_MTU_VIOLATION)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_mtu_violation, fld_val, 90, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TRILL_TTL_0)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_trill_ttl_zero, fld_val, 100, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TRILL_SAME_INTERFACE)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile2_reg.action_profile_trill_same_interface, fld_val, 110, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_TRILL_BOUNCE_BACK)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile3_reg.action_profile_bounce_back, fld_val, 120, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_DSS_STACKING)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile1_reg.action_profile_dss_stacking , fld_val, 130, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_LAG_MULTICAST)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile1_reg.action_profile_lag_multicast, fld_val, 140, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if  (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_EXCLUDE_SRC)
  {
    SOC_PB_PP_FLD_GET(regs->egq.action_profile1_reg.action_profile_exclude_src, fld_val, 150, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }
  else if (trap_type_bitmap_ndx & SOC_PB_PP_TRAP_EG_TYPE_CFM_TRAP)
  {
    SOC_PB_IMPLICIT_FLD_GET(tm_regs->egq.cnm_general_config_reg.action_profile_cfm_trap, fld_val, 106, exit);
    SOC_PB_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  }

  *eg_action_profile = (valid ? eg_action_profile_from_fld : SOC_PB_PP_TRAP_EG_NO_ACTION);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_to_eg_action_map_get_unsafe()", trap_type_bitmap_ndx, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_eg_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  tbl_data.otm_valid =
   ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST) ? 0x1 : 0x0);
  tbl_data.otm = (profile_info->out_tm_port == SOC_PB_PP_TRAP_ACTION_PKT_DISCARD_ID ? 0x0 : profile_info->out_tm_port);
  tbl_data.discard = (profile_info->out_tm_port == SOC_PB_PP_TRAP_ACTION_PKT_DISCARD_ID ? 0x1 : 0x0);
  tbl_data.tc = profile_info->cos_info.tc;
  tbl_data.tc_valid = ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC) ? 0x1 : 0x0);
  tbl_data.dp = profile_info->cos_info.dp;
  tbl_data.dp_valid = ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP) ? 0x1 : 0x0);
  tbl_data.cud = profile_info->cud;
  tbl_data.cud_valid = ((profile_info->bitmap_mask & SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_CUD) ? 0x1 : 0x0);

  res = soc_pb_pp_egq_action_profile_table_tbl_set_unsafe(
          unit,
          profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_eg_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  soc_pb_pp_trap_eg_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(profile_ndx, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX, SOC_PB_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_eg_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  soc_pb_pp_trap_eg_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(profile_ndx, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN, SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX, SOC_PB_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_eg_profile_info_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_eg_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO_clear(profile_info);

  res = soc_pb_pp_egq_action_profile_table_tbl_get_unsafe(
    unit,
    profile_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  profile_info->bitmap_mask = 0;
  if (tbl_data.otm_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  }
  if (tbl_data.discard)
  {
    profile_info->out_tm_port = SOC_PB_PP_TRAP_ACTION_PKT_DISCARD_ID;
  }
  else
  {
    profile_info->out_tm_port = tbl_data.otm;
  }
  profile_info->cos_info.tc = (SOC_SAND_PP_TC)tbl_data.tc;
  if (tbl_data.tc_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC;
  }
  profile_info->cos_info.dp = (SOC_SAND_PP_DP)tbl_data.dp;
  if (tbl_data.dp_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP;
  }
  profile_info->cud = tbl_data.cud;
  if (tbl_data.cud_valid)
  {
    profile_info->bitmap_mask |= SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_CUD;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_eg_profile_info_get_unsafe()", profile_ndx, 0);
}

/*********************************************************************
*     Read MACT event from the events FIFO into buffer.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_mact_event_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                  buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                  *buff_len
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_MACT_EVENT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(buff_len);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->ihp.large_em_interrupt_reg.large_em_event_ready, fld_val, 10, exit);
  if (fld_val)
  {
    /* Event is ready in FIFO */
    
    /* Read large_em_event_reg.reg_0 last, since lsb read advances the FIFO */
    SOC_PB_PP_REG_GET(regs->ihp.large_em_event_reg.reg_3, buff[3], 23, exit);
    SOC_PB_PP_REG_GET(regs->ihp.large_em_event_reg.reg_2, buff[2], 22, exit);
    SOC_PB_PP_REG_GET(regs->ihp.large_em_event_reg.reg_1, buff[1], 21, exit);
    SOC_PB_PP_REG_GET(regs->ihp.large_em_event_reg.reg_0, buff[0], 20, exit);
    *buff_len = 4;
  }
  else
  {
    /* Event FIFO is empty */
    *buff_len = 0;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mact_event_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_trap_mact_event_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_MACT_EVENT_GET_VERIFY);

  /* Nothing to verify */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mact_event_get_verify()", 0, 0);
}

/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_trap_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                  buff_len,
    SOC_SAND_OUT SOC_PB_PP_TRAP_MACT_EVENT_INFO                *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_MACT_EVENT_PARSE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  SOC_PB_PP_TRAP_MACT_EVENT_INFO_clear(mact_event);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
  SOC_SAND_TODO_IMPLEMENT_WARNING;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mact_event_parse_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_trap_mact_event_parse_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                  buff_len
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TRAP_MACT_EVENT_PARSE_VERIFY);

  if (buff_len != SOC_PB_PP_TRAP_MGMT_BUFF_LEN_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_trap_mact_event_parse_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_trap_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_trap_mgmt_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_trap_mgmt;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_trap_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_trap_mgmt_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_trap_mgmt;
}
uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->frwrd_dest), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_shift, SOC_PB_PP_TRAP_MGMT_VSI_SHIFT_MAX, SOC_PB_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->counter_select, SOC_PB_PP_TRAP_MGMT_COUNTER_SELECT_MAX, SOC_PB_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->counter_id, SOC_PB_PP_TRAP_MGMT_COUNTER_ID_MAX, SOC_PB_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter_select, SOC_PB_PP_TRAP_MGMT_METER_SELECT_MAX, SOC_PB_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter_id, SOC_PB_PP_TRAP_MGMT_METER_ID_MAX, SOC_PB_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter_command, SOC_PB_PP_TRAP_MGMT_METER_COMMAND_MAX, SOC_PB_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ethernet_police_id, SOC_PB_PP_TRAP_MGMT_ETHERNET_POLICE_ID_MAX, SOC_PB_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_offset_index, SOC_PB_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_MAX, SOC_PB_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, SOC_PB_PP_TRAP_MGMT_STRENGTH_MAX, SOC_PB_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->bitmap_mask, SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_MAX, SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO, &(info->dest_info), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO, &(info->cos_info), 13, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO, &(info->count_info), 14, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO, &(info->meter_info), 15, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO, &(info->policing_info), 16, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO, &(info->processing_info), 17, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, SOC_PB_PP_TRAP_MGMT_SNP_STRENGTH_MAX, SOC_PB_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->snoop_cmnd, SOC_PB_PP_TRAP_MGMT_SNOOP_CMND_MAX, SOC_PB_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->bitmap_mask, SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_MAX, SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR, 10, exit);
  
  if (info->out_tm_port != SOC_PB_PP_TRAP_ACTION_PKT_DISCARD_ID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->out_tm_port, SOC_PB_PP_TM_PORT_MAX, SOC_PB_PP_TM_PORT_OUT_OF_RANGE_ERR, 11, exit);
  }
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO, &(info->cos_info), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cud, SOC_PB_PP_TRAP_MGMT_CUD_MAX, SOC_PB_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_MACT_EVENT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_TRAP_MGMT_TYPE_MAX, SOC_PB_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_KEY, &(info->key), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE, &(info->val), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_MACT_EVENT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_TRAP_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_trap_code, SOC_PB_PP_TRAP_MGMT_CPU_TRAP_CODE_MAX, SOC_PB_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_trap_qualifier, SOC_PB_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_MAX, SOC_PB_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_sys_port, SOC_PB_PP_TRAP_MGMT_SRC_SYS_PORT_MAX, SOC_PB_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ntwrk_header_ptr, SOC_PB_PP_TRAP_MGMT_LL_HEADER_PTR_MAX, SOC_PB_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_PACKET_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

