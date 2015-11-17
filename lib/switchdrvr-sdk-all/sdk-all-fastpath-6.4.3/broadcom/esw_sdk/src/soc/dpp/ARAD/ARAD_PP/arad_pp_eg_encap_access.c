#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)


/* $Id: arad_pp_eg_encap_access.c,v 1.43 Broadcom SDK $
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
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/dpp_config_defs.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_LSB(start_index)       (35 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_MSB(start_index)       (35 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_NOF_BITS(start_index)  SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_MSB(start_index))

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_LSB(start_index)       (34 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_MSB(start_index)       (34 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_NOF_BITS(start_index)  SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_MSB(start_index))

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_LSB(start_index)       (33 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_MSB(start_index)       (33 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_NOF_BITS(start_index)  SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_MSB(start_index))

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_LSB(start_index)       (17 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_MSB(start_index)       (32 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_NOF_BITS(start_index)  SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_MSB(start_index))

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_LSB(start_index)       (16 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_MSB(start_index)       (16 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_NOF_BITS(start_index)  SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_MSB(start_index))

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_LSB(start_index)       (0 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_MSB(start_index)       (15 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_NOF_BITS(start_index)  SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_MSB(start_index))

/* Mutual Prefix 2 ENT - other fields might overlab key prefix */
#define ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_LSB(start_index)           (34 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_MSB(start_index)           (36 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_NOF_BITS(start_index)      SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_MSB(start_index))


/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* Get location of entry from outlif - in case one entry indicates two outlifs */
#define ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif) \
  ((outlif & 0x1) == 0x0)

#ifdef BCM_88660_A0
/* BIG AC always located at the start of the 1st entry */
#define ARAD_PP_EG_ENCAP_ACCESS_IS_BIG_AC_LOCATION(outlif) \
  ((outlif & 0x1) == 0x0)
#endif

#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_RIF_FORMAT 0x01
#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_RIF(half_entry_format)    \
      (half_entry_format == ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_RIF_FORMAT)

#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_AC_FORMAT 0x00
#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_AC(half_entry_format)    \
      (half_entry_format == ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_AC_FORMAT)



/* In Jericho, the next outlif field is only 17 bits, instead of 18. The 0 bit is 
 * is always assumed to be 0.
 */
#define ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(_unit, _next_outlif)\
      ((_next_outlif) = (SOC_IS_JERICHO(_unit)) ? ((_next_outlif) << 1) : (_next_outlif))

#define ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(_unit, _next_outlif) \
      if (SOC_IS_JERICHO(_unit) && ((_next_outlif) & 1)) { SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_LSB_MUST_BE_0, 999, exit);}

#define ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(_unit, _next_outlif) \
      ((SOC_IS_JERICHO(_unit)) ? (_next_outlif >> 1) : (_next_outlif))

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */



CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_encap_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_encap_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR,
    "ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR",
    "Entry type mismatch the requested entry hadle. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR",
    "mpls1_command is out of range. \n\r ",
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
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  arad_pp_eg_encap_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
#ifdef BCM_88660_A0
  uint32 tmp;
#endif
  uint32
    table_entry[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE] = {0x0,0x6,0x4C0}; /* Initialize EEDB table with None format and actions DROP.
                                                                                    This configuration for arad is fixed and will not be changed. */

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

/* In ARADPLUS and above, the "empty" lif is a lif with the trill format MSBs (since trill lifs are not used) with the AC "use as data"
   bits enabled for both half entries (In case half the lif becomes an AC lif, the other lif shouldn't be used yet). */
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit))  {
    ARAD_CLEAR(table_entry, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

    /* Key prefix set */
    tmp = 1;
    res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), table_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, table_entry, AC_0_ACTION_USE_AS_DATA_BITf, 1);
    soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, table_entry, AC_1_ACTION_USE_AS_DATA_BITf, 1);
  }
#endif

  
  if (SOC_IS_JERICHO(unit)) {
    uint32 reg_val;
    res = soc_reg_field32_modify(unit, EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 999, exit);

    /* EPNI Access to OutLIF 0 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 295, exit, READ_EPNI_PP_CONFIGr(unit, REG_PORT_ANY, &reg_val));
    soc_reg_field_set(unit, EPNI_PP_CONFIGr, &reg_val, INVALID_OUTLIF_FIX_ENf, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, WRITE_EPNI_PP_CONFIGr(unit, REG_PORT_ANY, reg_val));
  }
    
  res = arad_fill_table_with_entry(unit, EDB_EEDB_BANKm, MEM_BLOCK_ANY, &table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Inits devision of the Egress Encapsulation Table entry
 *     This configuration only take effect the entry type is not ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER and 
 *     ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_encap_access_entry_init_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif
  )
{  
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,  
    data2[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE],
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE]  = {0x0, 0x6, 0x4C0}; /* Initialize EEDB table with NONE format and actions DROP. */ 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  /* This function resets an EEDB entry for arad only. On ARADPLUS and above, it shouldn't be called. */
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_UNSUPPORTED_FOR_DEVICE_ERR, 5, exit);
  }

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }
  /* check that the outlif ID value in input is even */
  if ((outlif & 0x1) == 0x1)
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 900, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data2));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data2, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Check entry type matches except ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER and ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE
        No confiugaration need for them*/
  if (((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER)) &&
    ((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE)))
  {

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));
  }  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_entry_init_unsafe()", outlif, 0);
}



uint32
  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE *key_prefix
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    is_location_at_start,
    internal_offset,
    access_prefix = 0,
    access_suffix = 0,
    access_prefix_2_ent_ac = 0,
    access_prefix_2_ent_rif = 0,
    mpls1_command = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  
  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));
  
  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &access_prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_MPLS_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS){
    *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS;
  }
  else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA){
      /* The two LSBs of the data represent its type. Get the data. */
      uint64 data_entry_data;
      uint32 data_lsbs;
      soc_mem_field64_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_DATAf, &data_entry_data);
      COMPILER_64_TO_32_LO(data_lsbs, data_entry_data);
      res = soc_sand_bitstream_get_any_field(&data_lsbs, ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_LSB, ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_NOF_BITS, &access_suffix);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE) 
      {
        if(access_suffix == ARAD_PRGE_DATA_ENTRY_LSBS_IPV6_TUNNEL && SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_enable) {
            *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6;
        } 
        else {
          *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA;
        }
      }
      else
      {
        /* Device is TM */
        *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA;
      }
  }
  else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER){
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER;
      } 
      /* from jericho: LL format is shared by LL eedb entry and ROO LL eedb entry. 
         To differentiate between them, check additional bit: ROO-Link-Layer-Format-Identifier */
      else {
          uint32 roo_link_format_identifier; 

          /* get ROO-Link-Layer-Format-Identifier */
          roo_link_format_identifier = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_IDENTIFIERf); 
          
          /* ROO Link Layer format */
          if (roo_link_format_identifier) {
               *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER;
          } 
          else {
               *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER;
          }
      }
  }
  else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_IP_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP){
    *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP;
  }
  else if (access_prefix  ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_ISID){
    *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ISID;
  }
  else if (access_prefix  ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE){
    *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  }
  else if (access_prefix  ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER){

    is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);
  
    /* In ARAD, the ac format is 3 bits and the rif format is 2 bits. Therefore, they should be read and checked seperatly. */
    access_prefix_2_ent_ac = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf);
    access_prefix_2_ent_rif = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ENTRY_FORMATf : OUT_RIF_1_ENTRY_FORMATf);

    if (ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_RIF(access_prefix_2_ent_rif)){
      *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF;
    }
    else if (ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_AC(access_prefix_2_ent_ac)){
        *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC;
    }
    else {
      *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
    }
  }

  if(*key_prefix == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS) {
    /* mpls command get*/
    mpls1_command = soc_mem_field32_get(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_ACTION_MPLS_1_CMDf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (mpls1_command==ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val) {
        *key_prefix=ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP;
    } 
    else if (mpls1_command==ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val) {
        *key_prefix=ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP;
    }
    else /*command is push*/
    {
        *key_prefix=ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL;
    }
  }

  if (SOC_IS_ARAD_A0(unit)) {
    
    if ((outlif == 0) && ((*key_prefix) != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA)) {
        LOG_INFO(BSL_LS_SOC_EGRESS,
                 (BSL_META_U(unit,
                             "Notice: OutLIF 0 is being set to non DATA entry, this might affect Bridge traffic \n ")));
    }    
    
  }

  /* outlif 0 is not used */
  if (outlif == 0) {
      *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_key_prefix_type_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));
  
  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches MPLS */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_MPLS_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  /* get MPLS generic table data */

  tbl_data->drop = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_DROPf);
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_OAM_LIF_SETf);

  tbl_data->mpls1_command = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_MPLS_1_CMDf);

  tbl_data->next_vsi_lsb = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_VSI_LSBf);

  tbl_data->next_outlif = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_NEXT_OUTLIFf);
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_OUTLIF_PROFILEf);
  }

  if (tbl_data->mpls1_command <= ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val)
  {
    /* Only in case of MPLS Push */
    /* MPLS-Push-0-7 */  
    tbl_data->mpls1_label = soc_mem_field32_get(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_1_LABELf);

    tbl_data->mpls2_command = soc_mem_field32_get(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_ACTION_MPLS_2_CMD_LSBf);

    tbl_data->mpls2_label = soc_mem_field32_get(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_ACTION_MPLS_2_LABELf);


  } else if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val) {
    /* Only in case of MPLS pop */  
    tbl_data->tpid_profile = soc_mem_field32_get(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_TPID_PROFILEf);

    tbl_data->model_is_pipe = soc_mem_field32_get(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_MODEL_IS_PIPEf);

    tbl_data->has_cw = soc_mem_field32_get(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_CWf);

    tbl_data->upper_layer_protocol = soc_mem_field32_get(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_UPPER_LAYER_PROTOCOLf);


  } else if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val) {
    /* Only in case of MPLS swap */
    tbl_data->mpls1_label = soc_mem_field32_get(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_MPLS_1_LABELf);    

  } else {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR, 150, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
 
  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
   
  /* Set mpls generic table data. */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_DROPf, tbl_data->drop);


  soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);


  soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_MPLS_1_CMDf, tbl_data->mpls1_command);  


  soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_VSI_LSBf, tbl_data->next_vsi_lsb);    

  ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);
  soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }


  if (tbl_data->mpls1_command <= ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val)
  {
    /* Only in case of MPLS Push */
    /* MPLS-Push-0-7 */
    soc_mem_field32_set(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_1_LABELf, tbl_data->mpls1_label);


    soc_mem_field32_set(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_ACTION_MPLS_2_CMD_LSBf, tbl_data->mpls2_command);


    soc_mem_field32_set(unit, EPNI_MPLS_PUSH_FORMATm, data, MPLS_ACTION_MPLS_2_LABELf, tbl_data->mpls2_label);


  } else if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val) {
    /* Only in case of MPLS pop */
    soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_TPID_PROFILEf, tbl_data->tpid_profile);       


    soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_MODEL_IS_PIPEf, tbl_data->model_is_pipe);


    soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_CWf, tbl_data->has_cw);  


    soc_mem_field32_set(unit, EPNI_MPLS_POP_FORMATm, data, MPLS_ACTION_UPPER_LAYER_PROTOCOLf, tbl_data->upper_layer_protocol);    


  } else if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val) {
    /* Only in case of MPLS swap */
    soc_mem_field32_set(unit, EPNI_MPLS_SWAP_FORMATm, data, MPLS_ACTION_MPLS_1_LABELf, tbl_data->mpls1_label);    

  } else {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR, 150, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe()", outlif, 0);
}




uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1200, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches IP */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_IP_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  /* get IP table data */
  tbl_data->drop = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_DROPf);


  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_OAM_LIF_SETf);


  tbl_data->next_vsi_lsb = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_VSI_LSBf);


  tbl_data->next_outlif = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_NEXT_OUTLIFf);
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);


  tbl_data->ipv4_dst = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_DSTf);


  tbl_data->ipv4_src_index = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_SRC_INDEXf);

  
  tbl_data->ipv4_tos_index = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_TTL_INDEXf);

  
  tbl_data->ipv4_ttl_index = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_TOS_INDEXf);

  
  tbl_data->encapsulation_mode = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_ENC_MODEf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_OUTLIF_PROFILEf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
 
  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
   
  /* Key prefix set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* set IP table data */
  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_DROPf, tbl_data->drop);       
  

  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_VSI_LSBf, tbl_data->next_vsi_lsb);  
  
  ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);
  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));    
  

  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_DSTf, tbl_data->ipv4_dst);
  

  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_SRC_INDEXf, tbl_data->ipv4_src_index);
  
  
  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_TTL_INDEXf, tbl_data->ipv4_ttl_index);
  
  
  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_IPV4_TOS_INDEXf, tbl_data->ipv4_tos_index);
  
  
  soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_ENC_MODEf, tbl_data->encapsulation_mode);

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_IPV4_TUNNEL_FORMATm, data, IP_TUNNEL_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1300, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 mac64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1400, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches LL */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  /* get LL table data */
  tbl_data->drop = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_DROPf);
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_OAM_LIF_SETf);
  
  tbl_data->remark_profile = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_REMARK_PROFILEf);
  
  soc_mem_field64_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_MACf, &mac64);
  
  COMPILER_64_TO_32_HI(tbl_data->dest_mac[1], mac64);
  COMPILER_64_TO_32_LO(tbl_data->dest_mac[0], mac64);
  
  tbl_data->vid_valid = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_VSI_VALIDf);
  
  tbl_data->vid = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_VSIf);
  
  tbl_data->next_outlif_valid = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_NEXT_OUT_LIF_VALIDf);
  
  tbl_data->next_outlif_lsb = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_NEXT_OUT_LIF_LSBf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_OUTLIF_PROFILEf);
  }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 mac64;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  /* Key prefix set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* set LL table data */
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_DROPf, tbl_data->drop);       
  

  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_REMARK_PROFILEf, tbl_data->remark_profile);  
  
  COMPILER_64_SET(mac64, tbl_data->dest_mac[1], tbl_data->dest_mac[0]);

  soc_mem_field64_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_MACf, mac64);

  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_VSI_VALIDf, tbl_data->vid_valid);
  

  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_VSIf, tbl_data->vid);
  
  
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_NEXT_OUT_LIF_VALIDf, tbl_data->next_outlif_valid);
  
  
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_NEXT_OUT_LIF_LSBf, tbl_data->next_outlif_lsb);

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_LINK_LAYER_OR_ARP_FORMATm, data, ARP_LL_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1920, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 data64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches Data */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  /* get Data table data */
  tbl_data->drop = soc_mem_field32_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_DROPf);       
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_OAM_LIF_SETf);
  
  soc_mem_field64_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_DATAf, &data64);
  
  COMPILER_64_TO_32_HI(tbl_data->data[1], data64);
  COMPILER_64_TO_32_LO(tbl_data->data[0], data64);
  
  tbl_data->next_outlif_valid = soc_mem_field32_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_NEXT_OUTLIF_VALIDf);
  
  
  tbl_data->next_outlif = soc_mem_field32_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_NEXT_OUTLIFf);
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_OUTLIF_PROFILEf);
  }

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_error_t
    rv = SOC_E_NONE;
  uint64 data64;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_SET_UNSAFE);

  

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  /* Key prefix set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* set Data table data */
    soc_mem_field32_set(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_DROPf, tbl_data->drop);

  soc_mem_field32_set(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);

  COMPILER_64_SET(data64, tbl_data->data[1], tbl_data->data[0]);
  soc_mem_field64_set(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_DATAf, data64);

  
  soc_mem_field32_set(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_NEXT_OUTLIF_VALIDf, tbl_data->next_outlif_valid);
  
  ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);
  soc_mem_field32_set(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_DATA_FORMATm, data, DATA_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }


  SOC_DPP_ALLOW_WARMBOOT_WRITE(WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data), rv);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 110, exit);

exit:
  SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(rv);
  if(rv != SOC_E_NONE) {
    LOG_ERROR(BSL_LS_SOC_EGRESS,
              (BSL_META_U(unit,
                          " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    start_index,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1600, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches other */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  start_index = (is_location_at_start) ? ARAD_PP_EG_ENCAP_ACCESS_1ST_ENTRY_START_LOCATION:ARAD_PP_EG_ENCAP_ACCESS_2ND_ENTRY_START_LOCATION;
  
  /* Key 2_ENT prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_NOF_BITS(start_index), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check 2_ENT type matches trill */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_TRILL_2_ENT_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_TRILL))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 30, exit);
  }

  /* get Trill table data */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_NOF_BITS(start_index), &tbl_data->drop);       
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_NOF_BITS(start_index), &tbl_data->oam_lif_set);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_NOF_BITS(start_index), &tbl_data->m);  
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_NOF_BITS(start_index), &tbl_data->nick);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_NOF_BITS(start_index), &tbl_data->next_outlif_valid);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_NOF_BITS(start_index), &tbl_data->next_outlif);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    start_index,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1700, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  start_index = (is_location_at_start) ? ARAD_PP_EG_ENCAP_ACCESS_1ST_ENTRY_START_LOCATION:ARAD_PP_EG_ENCAP_ACCESS_2ND_ENTRY_START_LOCATION;
  
  /* Key prefix set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Key prefix 2_ENT set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_TRILL;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_NOF_BITS(start_index), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Set Trill data */
  res = soc_sand_bitstream_set_any_field(&tbl_data->drop, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_DROP_NOF_BITS(start_index), data);       
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_sand_bitstream_set_any_field(&tbl_data->oam_lif_set, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_OAM_LIF_SET_NOF_BITS(start_index), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_sand_bitstream_set_any_field(&tbl_data->m, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_M_NOF_BITS(start_index), data);  
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
  res = soc_sand_bitstream_set_any_field(&tbl_data->nick, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NICK_NOF_BITS(start_index), data);  
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_sand_bitstream_set_any_field(&tbl_data->next_outlif_valid, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_VALID_NOF_BITS(start_index), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
  res = soc_sand_bitstream_set_any_field(&tbl_data->next_outlif, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_NEXT_OUTLIF_NOF_BITS(start_index), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1710, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1810, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches other */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  /* Key 2_ENT prefix get */
  tmp = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ENTRY_FORMATf : OUT_RIF_1_ENTRY_FORMATf);
  

  /* Check 2_ENT type matches out rif */
  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_RIF(tmp))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 30, exit)
  }

  /* get outrif table data */
  tbl_data->drop = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_DROPf : OUT_RIF_1_ACTION_DROPf);
  

  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_OAM_LIF_SETf : OUT_RIF_1_ACTION_OAM_LIF_SETf);
  

  tbl_data->remark_profile = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_REMARK_PROFILEf : OUT_RIF_1_ACTION_REMARK_PROFILEf);
  

  tbl_data->next_vsi_lsb = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_VSI_LSBSf : OUT_RIF_1_ACTION_VSI_LSBSf);
  
  
  tbl_data->next_outlif_valid = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTR_VALIDf : OUT_RIF_1_ACTION_ARP_PTR_VALIDf);
  
  tbl_data->next_outlif = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTRf : OUT_RIF_1_ACTION_ARP_PTRf);
  
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_OUTLIF_PROFILEf : OUT_RIF_1_ACTION_OUTLIF_PROFILEf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1910, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_RIF_FORMAT;

  /* Key prefix 2_ENT set */
  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ENTRY_FORMATf : OUT_RIF_1_ENTRY_FORMATf, tmp);
  

  /* Set OUT-RIF data */
  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_DROPf : OUT_RIF_1_ACTION_DROPf, tbl_data->drop);       
  

  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_OAM_LIF_SETf : OUT_RIF_1_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_REMARK_PROFILEf : OUT_RIF_1_ACTION_REMARK_PROFILEf, tbl_data->remark_profile);  
  
  
  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_VSI_LSBSf : OUT_RIF_1_ACTION_VSI_LSBSf, tbl_data->next_vsi_lsb);  
  

  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTR_VALIDf : OUT_RIF_1_ACTION_ARP_PTR_VALIDf, tbl_data->next_outlif_valid);
  
  soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTRf : OUT_RIF_1_ACTION_ARP_PTRf, tbl_data->next_outlif);

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_OUTLIF_PROFILEf : OUT_RIF_1_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1920, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  )
{
    uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0, ent_access_prefix = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  uint64 data64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT, 1);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2020, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

  /* Key prefix get */
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Check entry type matches other */
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }


  /* Key 2_ENT prefix get */
  ent_access_prefix = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf);

  /* Check 2_ENT type matches AC */
  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_AC(ent_access_prefix))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 30, exit);
  }

  /* get AC table data */
  tbl_data->drop = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_DROPf : AC_1_ACTION_DROPf);
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_OAM_LIF_SETf : AC_1_ACTION_OAM_LIF_SETf);
  
  tbl_data->vlan_edit_profile = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_VLAN_EDIT_PROFILEf : AC_1_ACTION_VLAN_EDIT_PROFILEf);
  
  tbl_data->vid[0] = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_VID_1f : AC_1_ACTION_VID_1f);
    
  tbl_data->vid[1] = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_VID_2f : AC_1_ACTION_VID_2f);
  
  tbl_data->pcp_dei_profile = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_PCP_DEI_PROFILEf : AC_1_ACTION_PCP_DEI_PROFILEf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_OUTLIF_PROFILEf : AC_1_ACTION_OUTLIF_PROFILEf);
  }

      

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit) && is_location_at_start) {

          tbl_data->use_as_data_entry = soc_mem_field32_get(unit, EPNI_AC_ENTRY_WITH_DATA_FORMATm, data, AC_ACTION_USE_AS_DATA_BITf);

          if (tbl_data->use_as_data_entry) {
              soc_mem_field64_get(unit, EPNI_AC_ENTRY_WITH_DATA_FORMATm, data, AC_ACTION_DATAf, &data64);
              COMPILER_64_TO_32_HI(tbl_data->data[1], data64);
              COMPILER_64_TO_32_LO(tbl_data->data[0], data64);
          }
      }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  )
{
    uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    internal_offset,
    tmp = 0,
    bank_id,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE],
    previous_prefix = 0;
  uint8
    is_location_at_start;
#ifdef BCM_88660_A0
  uint32
    is_big_ac_entry=0;
  uint64 data64;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif);
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATILN_START(outlif);

  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset))
  { 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

#ifdef BCM_88660_A0
  if ((SOC_IS_ARADPLUS(unit)) && (tbl_data->use_as_data_entry) && (is_location_at_start == 0)) {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 12, exit);
  }
#endif

  bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
  internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2120, exit, READ_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

#ifdef BCM_88660_A0
    /* The Big AC identification should be done before other settings as it relies on the retrieved data as well */
    if (SOC_IS_ARADPLUS(unit)) {

        /* It's a Big AC either if the user refered to it as a data entry or if it was previously a Big AC
           entry (Prefix is other, outlif is in the start entry and the entry prefix is Out AC). */
        if (tbl_data->use_as_data_entry)
        {
            is_big_ac_entry = 1;
        } else if (ARAD_PP_EG_ENCAP_ACCESS_IS_BIG_AC_LOCATION(outlif)) {
            previous_prefix = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, AC_ENTRY_FORMATf);
            if (previous_prefix == ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER) {
                is_big_ac_entry = soc_mem_field32_get(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, AC_0_ACTION_USE_AS_DATA_BITf);
            }
        } else { /* In case of odd entry, it's not a big AC entry. use_as_data_entry should be cleared. */
            soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, AC_0_ACTION_USE_AS_DATA_BITf, is_big_ac_entry);
        }
    }
#endif


  /* Key prefix set */
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER;
  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, AC_ENTRY_FORMATf, tmp);
  

  /* Key prefix 2_ENT set */

  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf, ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_OUT_AC);

  /* Set AC data */
  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_DROPf : AC_1_ACTION_DROPf, tbl_data->drop);
  

  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_OAM_LIF_SETf : AC_1_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_VLAN_EDIT_PROFILEf : AC_1_ACTION_VLAN_EDIT_PROFILEf, tbl_data->vlan_edit_profile);
  
  
  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_VID_1f : AC_1_ACTION_VID_1f, (tbl_data->vid[0]));
  

  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_VID_2f : AC_1_ACTION_VID_2f, (tbl_data->vid[1]));
  
  
  soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_PCP_DEI_PROFILEf : AC_1_ACTION_PCP_DEI_PROFILEf, tbl_data->pcp_dei_profile);

  if (SOC_IS_ARADPLUS(unit)) {
      soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_USE_AS_DATA_BITf : AC_1_ACTION_USE_AS_DATA_BITf, tbl_data->use_as_data_entry);
  }

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_OUTLIF_PROFILEf : AC_1_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }
 

#ifdef BCM_88660_A0
    /* In case of a Big AC entry set the data and the data entry usage bit in the start entry.
       This also clears the entry if it was previously a Big AC entry */
    if (is_big_ac_entry == 1)
    {
        COMPILER_64_SET(data64, tbl_data->data[1], tbl_data->data[0]);

        /* This Memory format has a different name in Jericho. */
        soc_mem_field32_set(unit, EPNI_AC_ENTRY_WITH_DATA_FORMATm, data, AC_ACTION_USE_AS_DATA_BITf, tbl_data->use_as_data_entry);

        soc_mem_field64_set(unit, EPNI_AC_ENTRY_WITH_DATA_FORMATm, data, AC_ACTION_DATAf, data64);  
    }
#endif


  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2130, exit, WRITE_EDB_EEDB_BANKm(unit, bank_id, MEM_BLOCK_ANY, internal_offset, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe()", outlif, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_esem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_encap_access;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_esem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_encap_access;
}
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */


