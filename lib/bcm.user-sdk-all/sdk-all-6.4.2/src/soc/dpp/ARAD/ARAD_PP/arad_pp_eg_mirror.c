#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_eg_mirror.c,v 1.21 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_ports.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

#include <soc/dpp/port_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define ARAD_PP_EG_MIRROR_VID_DFLT_NDX                          7 /* means no VLAN, or non matching VLAN */
#define ARAD_PP_EG_MIRROR_VID_NOT_IN_USE                        0 /* VLAN ID value for mapping of vlan index to vlan ID, to signify no mapping */
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_BITS_PER_PROFILE        ((SOC_IS_JERICHO(unit)) ? 10 :4)
/* #define ARAD_PP_EG_MIRROR_PROFILE_MASK                          ((1 << ARAD_PP_EG_MIRROR_PROFILE_TABLE_BITS_PER_PROFILE) - 1) */

#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_PROFILE_SHIFT       8
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS 3
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_ENTRY_MASK     ((1 << ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS) - 1)
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_SHIFT               (ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_PROFILE_SHIFT - ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS)

/* these macros use both port & vid index even if one of them is not needed, to support later mapping changes */
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(port, vid) (((vid)<<ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_SHIFT) | ((port) >> ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS))
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) (ARAD_PP_EG_MIRROR_PROFILE_TABLE_BITS_PER_PROFILE * ((port) & ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_ENTRY_MASK))
/* #define ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(entry, port, vid) (((entry) >> ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid)) & ARAD_PP_EG_MIRROR_PROFILE_MASK)
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(src,dst, port, vid) (SHR_BITCOPY_RANGE(dst,0,src,ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid),ARAD_PP_EG_MIRROR_PROFILE_TABLE_BITS_PER_PROFILE))
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_SET_PROFILE(entry, port, vid, val) (SHR_BITCOPY_RANGE(entry,ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid),val,0,ARAD_PP_EG_MIRROR_PROFILE_TABLE_BITS_PER_PROFILE))
 */
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LEGTH 4
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_START  (0)
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LEGTH 2
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START (ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_START + ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LEGTH)
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_LEGTH 2 
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_START (ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START + ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LEGTH) 
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_LEGTH 1
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_START (ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_START + ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_LEGTH)
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_LEGTH 1
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_START (ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_START + ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_LEGTH)

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

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_mirror[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_mirror[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR",
    "The parameter 'enable_mirror' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 0 - SOC_PPD_NOF_TRAP_CODES.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'recycle_command' is out of range. \n\r "
    "The range is: 0 - DPP_MIRROR_ACTION_NDX_MAX.\n\r ",
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


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


static soc_field_t arad_pp_eg_mirror_vlan_id_flds[ARAD_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES] =
  {MIRROR_VID_0f, MIRROR_VID_1f, MIRROR_VID_2f, MIRROR_VID_3f, MIRROR_VID_4f, MIRROR_VID_5f, MIRROR_VID_6f};


STATIC void copy_if_not_null(uint32 *dst,uint32 dst_start,uint32 *src,uint32 src_start,uint32 len)
{
      if ((NULL==dst || NULL==src)) 
          return;
      SHR_BITCOPY_RANGE(dst,dst_start,src,src_start,len);
}


STATIC 
void 
 epni_mirror_profile_table_entry_get(
     int unit,
     ARAD_PP_PORT port,
     uint32 vid,
     uint32 *entry,
     uint32 *mirror_command,
     uint32 *forward_strength, 
     uint32 *copy_strength,
     uint32 *forward_en, 
     uint32 *mirror_en
     )
{

    uint32 start,length;

    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LEGTH;
    *mirror_command=0;
    copy_if_not_null(mirror_command, 0, entry, start, length);
    if (!SOC_IS_JERICHO(unit)) {
        return;
    }
    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START; 
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START;
    copy_if_not_null(forward_strength, 0, entry, start, length);

    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_LEGTH;
    copy_if_not_null(copy_strength, 0, entry, start, length);

    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_START;
    copy_if_not_null(forward_strength, 0, entry, start, length);


    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_START;
    copy_if_not_null(mirror_en, 0, entry, start, length);

}


STATIC 
void 
 epni_mirror_profile_table_entry_set(
     int unit,
     ARAD_PP_PORT port,
     uint32 vid,
     uint32 *entry,
     uint32 *mirror_command,
     uint32 *forward_strength, 
     uint32 *copy_strength,
     uint32 *forward_en, 
     uint32 *mirror_en
     )
{

    uint32 start,length;

    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LEGTH;
    copy_if_not_null( entry, start, mirror_command, 0,length);
    if (!SOC_IS_JERICHO(unit)) {
        return;
    }
    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START;
    copy_if_not_null(entry, start, forward_strength, 0, length);

    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_COPY_STRENGTH_LEGTH;
    copy_if_not_null(entry, start, copy_strength, 0, length);

    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_FWD_EN_START;
    copy_if_not_null(forward_en, 0, entry, start, length);


    start = ARAD_PP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) + ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_START;
    length = ARAD_PP_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_START;
    copy_if_not_null(mirror_en, 0, entry, start, length);

}
   
   
uint32
  arad_pp_eg_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  int i, res;
  uint32 value;
  soc_reg_above_64_val_t reg_above_64_val = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,
    WRITE_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val)); /* init to empty mapping, no VID index in use */

  /* init EPNI_MIRROR_PROFILE_MAPm to a static 1-1 mapping to recycling commands */
  for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
    value = 0;
    soc_mem_field32_set(unit, EPNI_MIRROR_PROFILE_MAPm, &value, MIRROR_COMMANDf, i);
    if (SOC_DPP_CONFIG(unit)->pp.oam_ccm_2_fhei_enable && i == SOC_DPP_CONFIG(unit)->pp.oam_ccm_2_fhei_eg_default_mirror_profile) {
        soc_mem_field32_set(unit, EPNI_MIRROR_PROFILE_MAPm, &value, FORWARD_DISABLEf, 1);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 100+i, exit, ARAD_REG_ACCESS_ERR,
      WRITE_EPNI_MIRROR_PROFILE_MAPm(unit, MEM_BLOCK_ALL, i, &value));
  }
  /* init the first half of IHP_RECYCLE_COMMANDm to a static 1-1 mapping from recycle commands to inbound mirror action profiles */
  for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
    value = 0; /* other recycling command actions are disabled */
    soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, MIRROR_PROFILEf, i);
    if (i > 0) { /* for all valid outbound mirror profile , set highest strength for forward action packet to drop */      
      soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, FORWARD_STRENGTHf, 7); /* highest strength */
      soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, CPU_TRAP_CODEf, ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0); /* trap code ACCEPTABLE_FRAME_TYPE_DROP */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 100+i, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ALL, i, &value));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_init_unsafe()", 0, 0);
}

/*********************************************************************
 *     Set outbound mirroring for out-port and VLAN, so all
 *     outgoing packets leave from the given port and with the
 *     given VID will be mirrored according to 'enable_mirror'
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  arad_pp_eg_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PP_PORT             out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID      vid,       /* VLAN number */
    SOC_SAND_IN  uint8                    enable_mirror, /* mirror profile */
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
  )
{
  uint32 res, q_pair;
  uint32 internal_vid_ndx; /* VLAN index (0-6) representing the VLAN ID */
  uint32 entry[3]; /* will hold an EPNI_MIRROR_PROFILE_TABLEm or EGQ_PCTm entry */
  uint32 context, channel; /* reassembly context and recycle interface channel */
  uint32 base_q_pair = 0; /* ID of first queue pair of out_port_ndx */
  uint32 end_q_pair  = 0; /* ID of last queue pair of out_port_ndx + 1*/
  uint8 first_appear;
  int mirror_profile_table_offset, clean_stage = 0;
  uint32 enable_mirror32 = enable_mirror;
  int core = 0;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, out_port_ndx, 0, &end_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if (!end_q_pair) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

  /* Search and if needed add vid in/to db */
  res = arad_sw_db_multiset_add(unit, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, &vid, &internal_vid_ndx, &first_appear, success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (*success != SOC_SAND_SUCCESS) { /* no room for new VID */
    goto exit;
  }
  clean_stage = 1;

  if (first_appear) { /* New VLAN, a VID was just created for it, add it to hardware */
    soc_reg_above_64_val_t fld_value = {0}, reg_above_64_val;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
      READ_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val)); /* read current hardware mapping */
    fld_value[0] = vid; /* set hardware mapping from new index internal_vid_ndx to vid */
    soc_reg_above_64_field_set(unit, EPNI_MIRROR_VID_REGr, reg_above_64_val, arad_pp_eg_mirror_vlan_id_flds[internal_vid_ndx], fld_value);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 40, exit, ARAD_REG_ACCESS_ERR,
      WRITE_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val)); /* write the modified hardware mapping */
  }


  /* Set reassembly context (mirror channel) for port in EGQ_PCTm if not set already */
  res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit,out_port_ndx, 0, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR, READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, base_q_pair, entry));

  if (!soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf)) { /* if disabled, need to allocate and set a context */
    ARAD_PORT2IF_MAPPING_INFO port_mapping_info;
    res = alloc_reassembly_context_and_recycle_channel_unsafe(unit, core, out_port_ndx, &context, &channel); /* allocate reassembly context */
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit); /* error if no port / reassembly context is available */
    clean_stage |= 2;
    end_q_pair += base_q_pair;
    for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
      if (q_pair > base_q_pair) { /* entry for base_q_pair already read */
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 70, exit, ARAD_REG_ACCESS_ERR,
          READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry));
      }
      soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_CHANNELf, channel);
      soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 1);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 80, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry));
    }
    /* map recycle interface channel to reassembly context (port) */
    port_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
    port_mapping_info.channel_id = channel;
    res = arad_port_ingr_map_write_val_unsafe(unit, context, TRUE, &port_mapping_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit); /* error if no port / reassembly context is available */
  }

  /* write the profile mapping for the port+vlan index */
  mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(out_port_ndx, internal_vid_ndx);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130, exit, ARAD_REG_ACCESS_ERR,
    READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, mirror_profile_table_offset, entry));
  /* prev_profile = ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(entry[0], out_port_ndx, internal_vid_ndx); */
  /* ARAD_PP_EG_MIRROR_PROFILE_TABLE_SET_PROFILE(entry[0], out_port_ndx, internal_vid_ndx, enable_mirror); */
  epni_mirror_profile_table_entry_set(unit, out_port_ndx, internal_vid_ndx,entry, &enable_mirror32, NULL, NULL, NULL, NULL);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 140, exit, ARAD_REG_ACCESS_ERR,
    WRITE_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ALL, mirror_profile_table_offset, entry));
  /* Add to SW */
  res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(unit, out_port_ndx, internal_vid_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res,  150, exit);

exit:

  if (ex != no_err) { /* cleanup on error */
    if (clean_stage & 2) { /* deallocate reassembly context */
      for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
        if (READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry) == SOC_E_NONE) {                                    \
          soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 0);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry));
        }
      }
      release_reassembly_context_and_mirror_channel_unsafe(unit, context);
    }
    if (clean_stage & 1) { /* decrease count of and possibly deallocate vlan index */
      uint8 last_appear;
      arad_sw_db_multiset_remove_by_index(unit, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, internal_vid_ndx, &last_appear);
      if (first_appear) { /* Remove from HW */
        soc_reg_above_64_val_t fld_value = {0}, reg_above_64_val;
        if (READ_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val) == SOC_E_NONE) { /* read current hardware mapping */
          fld_value[0] = ARAD_PP_EG_MIRROR_VID_NOT_IN_USE; /* set hardware mapping from the removed index to not mapped */
          soc_reg_above_64_field_set(unit, EPNI_MIRROR_VID_REGr, reg_above_64_val, arad_pp_eg_mirror_vlan_id_flds[internal_vid_ndx], fld_value);
          res = WRITE_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val); /* write the modified hardware mapping */
        }
      }
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_add_unsafe()", out_port_ndx, vid);
}

uint32
  arad_pp_eg_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_PORT         out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID  vid,
    SOC_SAND_IN  uint8                enable_mirror
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(enable_mirror, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR, 30, exit);

  /* IMPLEMENTED */
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_add_verify()", out_port_ndx, vid);
}

/*********************************************************************
 *     Remove a mirroring for port and VLAN, upon this packet
 *     transmitted out this out_port_ndx and vid will be
 *     mirrored or not according to default configuration for
 *     out_port_ndx. see soc_ppd_eg_mirror_port_dflt_set()
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  arad_pp_eg_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_PORT         out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID  vid
  )
{
  uint32 res, internal_vid_ndx, ref_count, dflt_profile[1];
  uint32 entry[3]; /* will hold an EPNI_MIRROR_PROFILE_TABLEm or EGQ_PCTm entry */
  uint32 q_pair, nof_priorities, end_q_pair; /* IDs of the current and last queue pairs of out_port_ndx + 1*/
  uint8 last_appear;
  int mirror_profile_table_offset;
  uint32 context, channel;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE);

  /* Search vid in db */
  res = arad_sw_db_multiset_lookup(unit, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, &vid, &internal_vid_ndx, &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count) { /* If the vid was found */
    /* Decrease count for this vid */
    res = arad_sw_db_multiset_remove_by_index(unit, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, internal_vid_ndx, &last_appear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (last_appear) { /* Remove from HW */
      soc_reg_above_64_val_t fld_value = {0}, reg_above_64_val;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
        READ_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val)); /* read current hardware mapping */
      fld_value[0] = ARAD_PP_EG_MIRROR_VID_NOT_IN_USE; /* set hardware mapping from the removed index to not mapped */
      soc_reg_above_64_field_set(unit, EPNI_MIRROR_VID_REGr, reg_above_64_val, arad_pp_eg_mirror_vlan_id_flds[internal_vid_ndx], fld_value);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 40, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val)); /* write the modified hardware mapping */
    }

    /* Remove from SW */
    res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(unit, out_port_ndx, internal_vid_ndx, FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* Remove specific mirroring for the removed port x vid index by mapping it to the default port mirror profile. */
    mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(out_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR,
      READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, mirror_profile_table_offset, entry));
    epni_mirror_profile_table_entry_get(unit, out_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX,entry, dflt_profile, NULL, NULL, NULL, NULL);

/*    ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(dflt_profile,entry, out_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX);  */

    mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(out_port_ndx, internal_vid_ndx);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 70, exit, ARAD_REG_ACCESS_ERR,
      READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, mirror_profile_table_offset, entry));
/*    ARAD_PP_EG_MIRROR_PROFILE_TABLE_SET_PROFILE(entry[0], out_port_ndx, internal_vid_ndx, *dflt_profile); */
    epni_mirror_profile_table_entry_set(unit, out_port_ndx, internal_vid_ndx,entry, dflt_profile, NULL, NULL, NULL, NULL);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 80, exit, ARAD_REG_ACCESS_ERR,
      WRITE_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ALL, mirror_profile_table_offset, entry));

    /* if the port's reassembly context (mirror channel) is not used any more (outbound port not mirrored), release it */
    for (internal_vid_ndx = 0; internal_vid_ndx < ARAD_PP_EG_MIRROR_VID_DFLT_NDX; ++internal_vid_ndx) {
      res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, out_port_ndx, internal_vid_ndx, &last_appear);
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      if (last_appear) { /* this vlan id index is used */
        break;
      }
    }
    if (!last_appear && !(*dflt_profile)) { /* no vlan id nor the default are mirrored for the port */
      ARAD_PORT2IF_MAPPING_INFO port_mapping_info;
      int enabled = 1;
      /* Remove reassembly context from hardware and deallocate it. */

      res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit,out_port_ndx,0, &q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 195, exit);
      res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, out_port_ndx, 0, &nof_priorities);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 197, exit);

      end_q_pair = q_pair + nof_priorities;

      for (; q_pair < end_q_pair; ++q_pair) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR,
          READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry));
        enabled &=  soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf);
        soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
          WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry));
      }
      /* release reassembly context */
      channel = soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_CHANNELf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 220, exit, ARAD_REG_ACCESS_ERR,
        READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &context));
      context = soc_mem_field32_get(unit, IRE_CTXT_MAPm, &context, REASSEMBLY_CONTEXTf);
      res = release_reassembly_context_and_mirror_channel_unsafe(unit, context);
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit); /* error if no port / reassembly context is available */
      /* remove recycle interface channel to reassembly context (port) mapping */
      port_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
      port_mapping_info.channel_id = channel;
      res = arad_port_ingr_map_write_val_unsafe(unit, context, FALSE, &port_mapping_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 250, exit, ARAD_REG_ACCESS_ERR,
        enabled ? SOC_E_NONE : SOC_E_INTERNAL); /* if disabled for any queue pair, it is an internal error */
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_remove_unsafe()", out_port_ndx, vid);
}

uint32
  arad_pp_eg_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_PP_PORT        out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID vid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_remove_verify()", out_port_ndx, vid);
}

/*********************************************************************
*     Get the assigned mirroring profile for port and VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_PP_PORT        out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID vid,
    SOC_SAND_OUT uint8               *enable_mirror
  )
{
  uint32 res, internal_vid_ndx, ref_count;
  uint32 entry[3]; /* will hold an EPNI_MIRROR_PROFILE_TABLEm entry */
  uint8  port_vlan_exists;
  uint32 enable_mirror32[1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(enable_mirror);

  *enable_mirror = 0;

  /* Search vid in db */
  res = arad_sw_db_multiset_lookup(unit, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, &vid, &internal_vid_ndx, &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count) { /* If the vid was found, and we have an index for it */
    res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, out_port_ndx, internal_vid_ndx, &port_vlan_exists);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (port_vlan_exists) { /* If port-vlan exists in the SWDB */
      /* Get the profile of the port x vid index */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
        READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, 
          ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(out_port_ndx, internal_vid_ndx), entry));
/*      ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(enable_mirror, entry[0], out_port_ndx, internal_vid_ndx); */
      epni_mirror_profile_table_entry_get(unit, out_port_ndx, internal_vid_ndx,entry, enable_mirror32, NULL, NULL, NULL, NULL);
      *enable_mirror = *enable_mirror32 & 0xff;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_get_unsafe()", out_port_ndx, vid);
}

uint32
  arad_pp_eg_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_get_verify()", out_port_ndx, vid);
}

/*********************************************************************
*     Set default mirroring profiles for port
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO            *dflt_mirroring_info
  )
{
  uint32 res, prev_profile[1];
  uint32 entry[3]; /* will hold an EPNI_MIRROR_PROFILE_TABLEm  entry */
  uint32 entry_context[3]; /* will hold an EGQ_PCTm entry */
  uint32 q_pair, nof_pairs, end_q_pair; /* IDs of the current and last queue pairs of out_port_ndx + 1*/
  unsigned internal_vid_ndx;
  int is_disable, has_mirrored_vlans = 0;
  int mirror_profile_table_offset;
  uint8 port_vlan_exist;
  uint32 context, channel;
  ARAD_PORT2IF_MAPPING_INFO port_mapping_info;
  uint32 default_profile[1];
   int core = 0;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);

  *default_profile = dflt_mirroring_info->dflt_profile;
  res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, local_port_ndx, 0, &end_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if (!end_q_pair) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

  /* read the profile mapping for the port, and get current default (untagged, or unmatched VLAN) profile */
  mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
    READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, mirror_profile_table_offset, entry));
/*  ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(prev_profile, entry[0], local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX); */
  epni_mirror_profile_table_entry_get(unit, local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX,entry, prev_profile, NULL, NULL, NULL, NULL);
  is_disable = !dflt_mirroring_info->dflt_profile && *prev_profile;

  if (dflt_mirroring_info->dflt_profile && !(*prev_profile)) { /* port default will start being mirrored */
    /* Set reassembly context (mirror channel) for port in EGQ_PCTm if not set already */
    uint32 base_q_pair;
    res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, local_port_ndx, 0, &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
      READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, base_q_pair, entry_context));

    if (!soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) { /* if disabled, need to allocate and set a context */
      res = alloc_reassembly_context_and_recycle_channel_unsafe(unit, core, local_port_ndx, &context, &channel); /* allocate reassembly context */
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); /* error if no port / reassembly context is available */
      end_q_pair += base_q_pair;
      for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
        if (q_pair > base_q_pair) { /* entry for base_q_pair already read */
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR,
            READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry_context));
        }
        soc_mem_field32_set(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf, channel);
        soc_mem_field32_set(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR,
          WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry_context));
      }
      /* map recycle interface channel to reassembly context (port) */
      port_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
      port_mapping_info.channel_id = channel;
      res = arad_port_ingr_map_write_val_unsafe(unit, context, TRUE, &port_mapping_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit); /* error if no port / reassembly context is available */
    }
  }

  /* set the profile mapping for default (untagged, or unmatched VLAN) */
/*  ARAD_PP_EG_MIRROR_PROFILE_TABLE_SET_PROFILE(entry[0], local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX, dflt_mirroring_info->dflt_profile); */
  epni_mirror_profile_table_entry_set(unit, local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX,entry, default_profile, NULL, NULL, NULL, NULL);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 100, exit, ARAD_REG_ACCESS_ERR,
    WRITE_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ALL, mirror_profile_table_offset, entry));

  /* set the profile mapping for VLAN IDs who are not mapped for this port */
  for (internal_vid_ndx = 0; internal_vid_ndx < ARAD_PP_EG_MIRROR_VID_DFLT_NDX; ++internal_vid_ndx) {
    if (!SOC_DPP_PP_ENABLE(unit)) {
      port_vlan_exist = FALSE;
    } else {
      res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, local_port_ndx, internal_vid_ndx, &port_vlan_exist);
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    }
    if (port_vlan_exist) {
      has_mirrored_vlans = 1;
    } else {
      mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(local_port_ndx, internal_vid_ndx);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 120, exit, ARAD_REG_ACCESS_ERR,
        READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, mirror_profile_table_offset, entry));
/*      ARAD_PP_EG_MIRROR_PROFILE_TABLE_SET_PROFILE(entry[0], local_port_ndx, internal_vid_ndx, dflt_mirroring_info->dflt_profile); */  /* set profile */
      epni_mirror_profile_table_entry_set(unit, local_port_ndx, internal_vid_ndx,entry, default_profile, NULL, NULL, NULL, NULL); /* set profile */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 130, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ALL, mirror_profile_table_offset, entry));
    }
  }

  /* If mirroring of the port is disabled (setting a profile of 0 and the previous profile was not 0;
     if the port's reassembly context (mirror channel) is not used any more (outbound port not mirrored), release it */
  if (is_disable && !has_mirrored_vlans) {
    /* Remove reassembly context from hardware and deallocate it. */
    int enabled = 1;

    res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, local_port_ndx, 0, &q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

    res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, local_port_ndx, 0, &nof_pairs);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 180, exit);

    end_q_pair = q_pair + nof_pairs;

    for (; q_pair < end_q_pair; ++q_pair) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR,
        READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry_context));
      enabled &=  soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf);
      soc_mem_field32_set(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf, 0);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry_context));
    }
    /* release reassembly context */
    channel = soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 220, exit, ARAD_REG_ACCESS_ERR,
      READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &context));
    context = soc_mem_field32_get(unit, IRE_CTXT_MAPm, &context, REASSEMBLY_CONTEXTf);
    res = release_reassembly_context_and_mirror_channel_unsafe(unit, context);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit); /* error if no port / reassembly context is available */
    /* remove recycle interface channel to reassembly context (port) mapping */
    port_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
    port_mapping_info.channel_id = channel;
    res = arad_port_ingr_map_write_val_unsafe(unit, context, FALSE, &port_mapping_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 250, exit, ARAD_REG_ACCESS_ERR,
      enabled ? SOC_E_NONE : SOC_E_INTERNAL); /* if disabled for any queue pair, it is an internal error */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_eg_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO       *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_EG_MIRROR_PORT_DFLT_INFO, dflt_mirroring_info, 20, exit);

   /* IMPLEMENTED */
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_eg_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

   /* IMPLEMENTED */
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Get default mirroring profiles for port
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PP_PORT                      local_port_ndx,
    SOC_SAND_OUT ARAD_PP_EG_MIRROR_PORT_DFLT_INFO  *dflt_mirroring_info
  )
{
  int res;
  uint32 entry[3];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);

  /* read the profile mapping for default (untagged, or unmatched VLAN) */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,
    READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, 
      ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX), entry));
/*  ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(&(dflt_mirroring_info->dflt_profile), entry[0], local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX); */
  epni_mirror_profile_table_entry_get(unit, local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX,entry, &(dflt_mirroring_info->dflt_profile), NULL, NULL, NULL, NULL);
  dflt_mirroring_info->enable_mirror = SOC_SAND_NUM2BOOL(dflt_mirroring_info->dflt_profile);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
* Enable or disable mirroring for a port by other (than mirroring) applications.
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_appl_set_unsafe(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  ARAD_PP_PORT  local_port_ndx, /* Local port ID */
    SOC_SAND_IN  uint8         enable          /* 0 will disable, other values will enable */
  )
{
  uint32 res;
  uint32 entry[3]; /* will hold an EGQ_PCTm or EPNI_MIRROR_PROFILE_TABLEm entry */
  uint32 q_pair, end_q_pair; /* IDs of the current and last queue pairs of out_port_ndx + 1*/
  uint8 is_reserved;
  uint32 default_mirroring_enable[1];
  uint32 context, channel;
  ARAD_PORT2IF_MAPPING_INFO port_mapping_info;
  int core = 0;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_APPL_SET_UNSAFE);

  res = arad_sw_db_is_port_reserved_for_reassembly_context(unit, local_port_ndx, &is_reserved);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); /* error if no port / reassembly context is available */
  if (is_reserved != (enable ? 1 : 0)) { /* need to do work only if the wanted state is different from the current one */

    res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, local_port_ndx, 0, &end_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

    if (!end_q_pair) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR, 20, exit);
    }

    if (enable) { /* reserve the mapping, if a reassembly context + channel are not allocated, allocate and set them */

      /* Set reassembly context (mirror channel) for port in EGQ_PCTm if not set already */
      uint32 base_q_pair;
      res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, local_port_ndx, 0, &base_q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
        READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, base_q_pair, entry));

      if (!soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf)) { /* if disabled, need to allocate and set a context */
        res = alloc_reassembly_context_and_recycle_channel_unsafe(unit, core, local_port_ndx, &context, &channel); /* allocate reassembly context */
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); /* error if no port / reassembly context is available */
        end_q_pair += base_q_pair;
        for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
          if (q_pair > base_q_pair) { /* entry for base_q_pair already read */
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR,
              READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry));
          }
          soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_CHANNELf, channel);
          soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 1);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR,
            WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry));
        }
        /* map recycle interface channel to reassembly context (port) */
        port_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
        port_mapping_info.channel_id = channel;
        res = arad_port_ingr_map_write_val_unsafe(unit, context, TRUE, &port_mapping_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit); /* error if no port / reassembly context is available */
      }

    } else { /* unreserve the mapping, if the reassembly context + channel are not used by mirroring, release them and update hardware */

      /* read the profile mapping for the port, and get current default (untagged, or unmatched VLAN) profile */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 100, exit, ARAD_REG_ACCESS_ERR,
        READ_EPNI_MIRROR_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, 
          ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX),
          entry));
      /* check if the deafult mirroring of the port is disabled (profile 0) */
 /*     ARAD_PP_EG_MIRROR_PROFILE_TABLE_GET_PROFILE(default_mirroring_enable, entry, local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX) */
      epni_mirror_profile_table_entry_get(unit, local_port_ndx, ARAD_PP_EG_MIRROR_VID_DFLT_NDX,entry, default_mirroring_enable, NULL, NULL, NULL, NULL);

      if (!(*default_mirroring_enable)) {

        /* check if any VLAN is mirrored for this port */
        unsigned internal_vid_ndx;
        uint8 port_vlan_exist;
        for (internal_vid_ndx = 0; internal_vid_ndx < ARAD_PP_EG_MIRROR_VID_DFLT_NDX; ++internal_vid_ndx) {
          if (!SOC_DPP_PP_ENABLE(unit)) {
              port_vlan_exist = FALSE;
          } else {
              res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, local_port_ndx, internal_vid_ndx, &port_vlan_exist);
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          }
          if (port_vlan_exist) {
            break;
          }
        }
        if (internal_vid_ndx >=ARAD_PP_EG_MIRROR_VID_DFLT_NDX) { /* if no VLAN is mirrored for this port */
          /* Remove reassembly context from hardware and deallocate it. */
          int was_recycle_enabled = 1;

          res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, local_port_ndx, 0, &q_pair);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

          end_q_pair += q_pair;
          for (; q_pair < end_q_pair; ++q_pair) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR,
              READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, q_pair, entry));
            was_recycle_enabled &=  soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf);
            soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
              WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, q_pair, entry));
          }
          /* release reassembly context */
          channel = soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_CHANNELf);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 220, exit, ARAD_REG_ACCESS_ERR,
            READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &context));
          context = soc_mem_field32_get(unit, IRE_CTXT_MAPm, &context, REASSEMBLY_CONTEXTf);
          res = release_reassembly_context_and_mirror_channel_unsafe(unit, context);
          SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit); /* error if no port / reassembly context is available */
          /* remove recycle interface channel to reassembly context (port) mapping */
          port_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
          port_mapping_info.channel_id = channel;
          res = arad_port_ingr_map_write_val_unsafe(unit, context, FALSE, &port_mapping_info);
          SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 250, exit, ARAD_REG_ACCESS_ERR,
            was_recycle_enabled ? SOC_E_NONE : SOC_E_INTERNAL); /* if disabled for any queue pair, it is an internal error */
        }
      }
    } /* finished unreserving */

    res = arad_sw_db_set_port_reserved_for_reassembly_context(unit, local_port_ndx, enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit); /* error if no port / reassembly context is available */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_appl_set_unsafe()", local_port_ndx, enable);
}

/*********************************************************************
*     Check if mirroring for a port by other (than mirroring) applications is enabled
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_appl_get_unsafe(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  ARAD_PP_PORT  local_port_ndx, /* Local port ID */
    SOC_SAND_OUT uint8         *is_enabled     /* 0 will disable, other values will enable */
  )
{
  int res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_APPL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(is_enabled);

  res = arad_sw_db_is_port_reserved_for_reassembly_context(unit, local_port_ndx, is_enabled);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); /* error if no port / reassembly context is available */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_appl_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Set RECYCLE_COMMAND table with trap code
*********************************************************************/
uint32
  arad_pp_eg_mirror_recycle_command_trap_set_unsafe(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command, /* Equal to mirror profile */
    SOC_SAND_IN  uint32        trap_code, /* PPD - not HW code */
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  )
{
  int res;
  uint32 entry, reg;
  uint32 internal_trap_code; 
  ARAD_SOC_REG_FIELD strength_fld_fwd, strength_fld_snp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_UNSAFE);

  res = arad_pp_trap_mgmt_trap_code_to_internal(trap_code, &internal_trap_code, &strength_fld_fwd, &strength_fld_snp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, CPU_TRAP_CODEf, &internal_trap_code);
  reg = forward_strengh;
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, FORWARD_STRENGTHf, &reg);
  reg = snoop_strength;
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, SNOOP_STRENGTHf, &reg);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_set_unsafe()", recycle_command, 0);
}

uint32
  arad_pp_eg_mirror_recycle_command_trap_set_verify(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command, /* Equal to mirror profile */
    SOC_SAND_IN  uint32        trap_code, /* PPD - not HW code */
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(recycle_command, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code, SOC_PPD_NOF_TRAP_CODES, ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_set_verify()", recycle_command, 0);
}

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_verify(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command  /* Equal to mirror profile */
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(recycle_command, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_get_verify()", recycle_command, 0);
}

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_unsafe(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command, /* Equal to mirror profile */
    SOC_SAND_OUT  uint32       *trap_code, /* PPD - not HW code */
    SOC_SAND_OUT  uint32       *snoop_strength
  )
{
  int res;
  uint32 entry;
  uint32 internal_trap_code;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(trap_code);
  SOC_SAND_CHECK_NULL_INPUT(snoop_strength);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));
  soc_IHP_RECYCLE_COMMANDm_field_get(unit, &entry, CPU_TRAP_CODEf, &internal_trap_code);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));
  *snoop_strength = soc_IHP_RECYCLE_COMMANDm_field32_get(unit, &entry, SNOOP_STRENGTHf);

  res = arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, internal_trap_code, trap_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_get_unsafe()", recycle_command, 0);
}

/*********************************************************************
*     Get default mirroring profiles for port
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PP_PORT                      local_port_ndx,
    SOC_SAND_OUT ARAD_PP_EG_MIRROR_PORT_INFO       *info
  )
{
  uint32 base_q_pair;
  uint32 res;
  uint32 entry_context[3]; /* will hold an EGQ_PCTm entry */
  uint32 context, channel; /* reassembly context and recycle interface channel */

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, local_port_ndx, 0, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR, 
      READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, base_q_pair, entry_context));

  if (!soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) 
  {
    info->outbound_mirror_enable = 0; 
  }
  else
  {
    info->outbound_mirror_enable = 1;
    channel = soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 220, exit, ARAD_REG_ACCESS_ERR,
        READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &context));

    info->outbound_port_ndx = soc_mem_field32_get(unit, IRE_CTXT_MAPm, &context, PORT_TERMINATION_CONTEXTf);    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_info_get_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_eg_mirror_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

   /* IMPLEMENTED */
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_eg_mirror module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_mirror_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_mirror;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_eg_mirror module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_mirror_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_mirror;
}

uint32
  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dflt_profile, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_EG_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}

uint32
  ARAD_PP_EG_MIRROR_PORT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->outbound_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_EG_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

