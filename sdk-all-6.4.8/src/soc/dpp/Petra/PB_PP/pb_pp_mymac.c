/* $Id: pb_pp_mymac.c,v 1.12 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_mymac.c
*
* MODULE PREFIX:  soc_pb_pp_pp
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
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mymac.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>

#include <soc/dpp/Petra/petra_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_MYMAC_MODE_MAX                                   (SOC_PB_PP_NOF_MYMAC_VRRP_MODES-1)
#define SOC_PB_PP_MYMAC_MY_NICK_NAME_MAX                           ((1<<16)-1)
#define SOC_PB_PP_MYMAC_VSI_LSB_MAX ((1<<6)-1)

#define SOC_PB_PP_MY_BMAC_MSB_LSB (8)
#define SOC_PB_PP_MY_BMAC_MSB_MSB (47)

#define SOC_PB_PP_MYMAC_VRRP_BITMAP_KEY_ALL_VSI_BASED(vsi, mac_lsb) \
  ((vsi<<1) + SOC_SAND_GET_BIT(mac_lsb,0))
#define SOC_PB_PP_MYMAC_VRRP_BITMAP_KEY_256_VSI_BASED(vsi, mac_lsb) \
  ((SOC_SAND_GET_BITS_RANGE(vsi,7,0)<<5) + SOC_SAND_GET_BITS_RANGE(mac_lsb,4,0))

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
  SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_DISABLED = 0,
  SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_ALL_VSI_BASED = 2,
  SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_256_VSI_BASED = 3
} SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_mymac[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_MSB_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VSI_LSB_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_VRRP_MAC_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_TRILL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_MSB_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MYMAC_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_mymac[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_MYMAC_VRRP_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MYMAC_VRRP_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vrrp_id_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MYMAC_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MYMAC_VRRP_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_MY_NICK_NAME_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MYMAC_MY_NICK_NAME_OUT_OF_RANGE_ERR",
    "The parameter 'my_nick_name' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_MYMAC_VSI_LSB_OUT_OF_RANGE,
    "SOC_PB_PP_MYMAC_VSI_LSB_OUT_OF_RANGE",
    "Vsi lsb is out of range. \n\r "
    "Maximal value is 00:00:00:00:00:3f.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_VRRP_IS_DISABLED_ERR,
    "SOC_PB_PP_MYMAC_VRRP_IS_DISABLED_ERR",
    "Mymac should be enabled and configured prior to performing\n\r "
    "this operation.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_LOCAL_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MYMAC_LOCAL_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'local_port' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_VSI_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MYMAC_VSI_OUT_OF_RANGE_ERR",
    "Vsi index is out of range. \n\r "
    "The range is: 1-4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MY_BMAC_MSB_LOW_BITS_NOT_ZERO_ERR,
    "SOC_PB_PP_MY_BMAC_MSB_LOW_BITS_NOT_ZERO_ERR",
    "'my_bmac_msb' defines bits 47:8. Bits 7:0 are ignored in this \n\r "
    "API, and therefor are validated to be zero.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_SYS_PORT_NON_SINGLE_PORT_NOT_SUPPORTED_ERR,
    "SOC_PB_PP_MYMAC_SYS_PORT_NON_SINGLE_PORT_NOT_SUPPORTED_ERR",
    "Non single port is not supported for this operation. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MYMAC_SYS_PORT_OUT_OF_RANGE,
    "SOC_PB_PP_MYMAC_SYS_PORT_OUT_OF_RANGE",
    "System port is out of range. \n\r "
    "The range is: 1 to 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_AUX_TABLE_MODE_MISMATCH_ERR,
    "SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_AUX_TABLE_MODE_MISMATCH_ERR",
    "MAC-IN-MAC not supported according to auxiliary table mode. \n\r ",
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
  soc_pb_pp_mymac_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the MAC address of the device. Used for
 *     ingress termination and egress encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_msb_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_mac_msb
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    reg_vals[SOC_PB_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS];
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_MSB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(my_mac_msb);

  regs = soc_pb_pp_regs();

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    my_mac_msb,
    mac_in_longs
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  SOC_PB_PP_FLD_TO_REG(regs->ihp.my_mac_msb_config_reg[0].my_mac_msb, mac_in_longs[0], reg_vals[0], 20, exit);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.my_mac_msb_config_reg[1].my_mac_msb, mac_in_longs[1], reg_vals[1], 30, exit);

  SOC_PB_PP_REG_BUFFER_SET(
    regs->ihp.my_mac_msb_config_reg,
    SOC_PB_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS,
    reg_vals,
    40,
    exit);

  reg_vals[0] = 0;
  reg_vals[1] = 0;

  res = soc_sand_bitstream_get_any_field(
    mac_in_longs,
    6,
    42,
    reg_vals
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);


  SOC_PB_PP_REG_SET(regs->epni.sa_msb_reg_0,reg_vals[0],50,exit);
  SOC_PB_PP_REG_SET(regs->epni.sa_msb_reg_1,reg_vals[1],60,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_msb_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mymac_msb_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_mac_msb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_MSB_SET_VERIFY);

  /* Nothing to verify */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_msb_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mymac_msb_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_MSB_GET_VERIFY);

  /* Nothing to do */

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_msb_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the MAC address of the device. Used for
 *     ingress termination and egress encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_msb_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                           *my_mac_msb
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    reg_vals[SOC_PB_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS];
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_MSB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(my_mac_msb);

  soc_sand_SAND_PP_MAC_ADDRESS_clear(my_mac_msb);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_BUFFER_GET(
    regs->ihp.my_mac_msb_config_reg,
    SOC_PB_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS,
    reg_vals,
    10,
    exit);

  SOC_PB_PP_FLD_FROM_REG(regs->ihp.my_mac_msb_config_reg[0].my_mac_msb, mac_in_longs[0], reg_vals[0], 20, exit);
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.my_mac_msb_config_reg[1].my_mac_msb, mac_in_longs[1], reg_vals[1], 30, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
    mac_in_longs,
    my_mac_msb
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_msb_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set MAC address LSB according to VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_vsi_lsb_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_mac_lsb
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA
    vsi_general_cfg_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VSI_LSB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  res = soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe(
          unit,
          vsi_ndx,
          &vsi_general_cfg_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /* Field size is 6b, so only 1st char is relevant */
  vsi_general_cfg_tbl_data.my_mac_lsb = my_mac_lsb->address[0];

  res = soc_pb_pp_ihp_vsi_my_mac_tbl_set_unsafe(
          unit,
          vsi_ndx,
          &vsi_general_cfg_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vsi_lsb_set_unsafe()", vsi_ndx, 0);
}

uint32
  soc_pb_pp_mymac_vsi_lsb_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_mac_lsb
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    my_mac_lsb_max_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] =
      {SOC_PB_PP_MYMAC_VSI_LSB_MAX+1, 0};
  SOC_SAND_PP_MAC_ADDRESS
    my_mac_lsb_max;
  uint8
    is_smaller;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VSI_LSB_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          my_mac_lsb_max_longs,
          &my_mac_lsb_max
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_sand_pp_mac_address_is_smaller(
          my_mac_lsb,
          &my_mac_lsb_max,
          &is_smaller
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  if (!is_smaller)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_VSI_LSB_OUT_OF_RANGE, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vsi_lsb_set_verify()", vsi_ndx, 0);
}

uint32
  soc_pb_pp_mymac_vsi_lsb_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VSI_LSB_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vsi_lsb_get_verify()", vsi_ndx, 0);
}

/*********************************************************************
*     Set MAC address LSB according to VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_vsi_lsb_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                           *my_mac_lsb
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA
    vsi_general_cfg_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VSI_LSB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  /* Field size is 6b, so only 1st char is relevant */
  res = soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe(
          unit,
          vsi_ndx,
          &vsi_general_cfg_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  my_mac_lsb->address[0] = (uint8)vsi_general_cfg_tbl_data.my_mac_lsb;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vsi_lsb_get_unsafe()", vsi_ndx, 0);
}

/*********************************************************************
*     Set My-MAC according to Virtual Router Redundancy
 *     Protocol.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_vrrp_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_MYMAC_VRRP_INFO                         *vrrp_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val = 0,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    reg_vals[SOC_PB_PP_IHP_VRID_MY_MAC_CONFIG_REG_MULT_NOF_REGS];
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrrp_info);

  regs = soc_pb_pp_regs();

  if (vrrp_info->enable)
  {
    switch (vrrp_info->mode)
    {
    case SOC_PB_PP_MYMAC_VRRP_MODE_ALL_VSI_BASED:
      fld_val = SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_ALL_VSI_BASED;
      break;
    case SOC_PB_PP_MYMAC_VRRP_MODE_256_VSI_BASED:
      fld_val = SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_256_VSI_BASED;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_MODE_OUT_OF_RANGE_ERR, 10, exit);
      break;
    }
  }

  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.vrid_my_mac_mode, fld_val, 10, exit);

  /* VRID_MY_MAC */
  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    &vrrp_info->vrid_my_mac_msb,
    mac_in_longs
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  SOC_PB_PP_FLD_TO_REG(regs->ihp.vrid_my_mac_config_reg_0.vrid_my_mac, mac_in_longs[0], reg_vals[0], 20, exit);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.vrid_my_mac_config_reg_1.vrid_my_mac, mac_in_longs[1], reg_vals[1], 30, exit);

  SOC_PB_PP_REG_SET(regs->ihp.vrid_my_mac_config_reg_0,reg_vals[0],40,exit);
  SOC_PB_PP_REG_SET(regs->ihp.vrid_my_mac_config_reg_1,reg_vals[1],50,exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mymac_vrrp_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_MYMAC_VRRP_INFO                         *vrrp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MYMAC_VRRP_INFO, vrrp_info, 10, exit);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mymac_vrrp_info_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_INFO_GET_VERIFY);

  /* Nothing to verify */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set My-MAC according to Virtual Router Redundancy
 *     Protocol.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_vrrp_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_MYMAC_VRRP_INFO                         *vrrp_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    reg_vals[SOC_PB_PP_IHP_VRID_MY_MAC_CONFIG_REG_MULT_NOF_REGS];
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrrp_info);

  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(vrrp_info->vrid_my_mac_msb));

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->ihp.vtt_general_configs_reg.vrid_my_mac_mode, fld_val, 10, exit);

  if (fld_val == SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_DISABLED)
  {
    vrrp_info->enable = FALSE;
    vrrp_info->mode = SOC_PB_PP_NOF_MYMAC_VRRP_MODES;
  }
  else
  {
    vrrp_info->enable = TRUE;

    switch (fld_val)
    {
    case SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_ALL_VSI_BASED:
      vrrp_info->mode = SOC_PB_PP_MYMAC_VRRP_MODE_ALL_VSI_BASED;
      break;
    case SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_256_VSI_BASED:
      vrrp_info->mode = SOC_PB_PP_MYMAC_VRRP_MODE_256_VSI_BASED;
      break;
    }
  }

  /* VRID MY MAC */
  SOC_PB_PP_REG_GET(regs->ihp.vrid_my_mac_config_reg_0.vrid_my_mac,reg_vals[0],20,exit);
  SOC_PB_PP_REG_GET(regs->ihp.vrid_my_mac_config_reg_1.vrid_my_mac,reg_vals[1],30,exit);

  SOC_PB_PP_FLD_FROM_REG(regs->ihp.vrid_my_mac_config_reg_0.vrid_my_mac, mac_in_longs[0], reg_vals[0], 40, exit);
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.vrid_my_mac_config_reg_0.vrid_my_mac, mac_in_longs[1], reg_vals[1], 50, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
    mac_in_longs,
    &(vrrp_info->vrid_my_mac_msb)
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable / Disable MyMac/MyVRID according to VRRP-ID and
 *     Mac Address LSB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_vrrp_mac_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK,
    global_bitmap_key,
    entry_offset,
    bitmap_key;
  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA
    vrid_my_mac_map_tbl_data;
  SOC_PB_PP_MYMAC_VRRP_INFO
    vrrp_info;
  SOC_PB_PP_TBLS
    *tbls = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_MAC_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(vrrp_mac_lsb_key);

  tbls = soc_pb_pp_tbls();

  res = soc_pb_pp_mymac_vrrp_info_get_unsafe(
    unit,
    &vrrp_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (!vrrp_info.enable)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_VRRP_IS_DISABLED_ERR, 20, exit);
  }
  else
  {
    switch(vrrp_info.mode)
    {
    
    case SOC_PB_PP_MYMAC_VRRP_MODE_ALL_VSI_BASED:
      global_bitmap_key =
        SOC_PB_PP_MYMAC_VRRP_BITMAP_KEY_ALL_VSI_BASED(vrrp_id_ndx, vrrp_mac_lsb_key->address[0]);
    	break;

    case SOC_PB_PP_MYMAC_VRRP_MODE_256_VSI_BASED:
      global_bitmap_key =
        SOC_PB_PP_MYMAC_VRRP_BITMAP_KEY_256_VSI_BASED(vrrp_id_ndx, vrrp_mac_lsb_key->address[0]);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_MODE_OUT_OF_RANGE_ERR, 30, exit);
      break;
    }
  }

  entry_offset = global_bitmap_key / tbls->ihp.vrid_my_mac_map_tbl.addr.width_bits;

  res = soc_pb_pp_ihp_vrid_my_mac_map_tbl_get_unsafe(
    unit,
    entry_offset,
    &vrid_my_mac_map_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  bitmap_key = global_bitmap_key % tbls->ihp.vrid_my_mac_map_tbl.addr.width_bits;

  SOC_SAND_SET_BIT(vrid_my_mac_map_tbl_data.vrid_my_mac_map, enable, bitmap_key);

  res = soc_pb_pp_ihp_vrid_my_mac_map_tbl_set_unsafe(
    unit,
    entry_offset,
    &vrid_my_mac_map_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_mac_set_unsafe()", vrrp_id_ndx, 0);
}

uint32
  soc_pb_pp_mymac_vrrp_mac_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_MAC_SET_VERIFY);

  res = soc_pb_pp_mymac_vrrp_mac_get_verify(
    unit,
    vrrp_id_ndx,
    vrrp_mac_lsb_key
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_mac_set_verify()", vrrp_id_ndx, 0);
}

uint32
  soc_pb_pp_mymac_vrrp_mac_get_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32               vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *vrrp_mac_lsb_key
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_MYMAC_VRRP_INFO
    vrrp_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_MAC_GET_VERIFY);

  res = soc_pb_pp_mymac_vrrp_info_get(
    unit,
    &vrrp_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (!vrrp_info.enable)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_VRRP_IS_DISABLED_ERR, 20, exit);
  }
  else
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(vrrp_id_ndx, SOC_PB_PP_VSI_ID_MIN, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_MYMAC_VSI_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_mac_get_verify()", vrrp_id_ndx, 0);
}

/*********************************************************************
*     Enable / Disable MyMac/MyVRID according to VRRP-ID and
 *     Mac Address LSB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_vrrp_mac_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *vrrp_mac_lsb_key,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK,
    global_bitmap_key,
    entry_offset,
    bitmap_key,
    vrpp_mode_fld_val;
  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA
    vrid_my_mac_map_tbl_data;
  SOC_PB_PP_TBLS
    *tbls = NULL;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_VRRP_MAC_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(vrrp_mac_lsb_key);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  tbls = soc_pb_pp_tbls();
  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->ihp.vtt_general_configs_reg.vrid_my_mac_mode, vrpp_mode_fld_val, 10, exit);

  switch(vrpp_mode_fld_val)
  {
  
  case SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_ALL_VSI_BASED:
    global_bitmap_key =
      SOC_PB_PP_MYMAC_VRRP_BITMAP_KEY_ALL_VSI_BASED(vrrp_id_ndx, vrrp_mac_lsb_key->address[0]);
    break;

  case SOC_PB_PP_MYMAC_VRRP_MODE_FLD_VAL_256_VSI_BASED:
    global_bitmap_key =
      SOC_PB_PP_MYMAC_VRRP_BITMAP_KEY_256_VSI_BASED(vrrp_id_ndx, vrrp_mac_lsb_key->address[0]);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_MODE_OUT_OF_RANGE_ERR, 30, exit);
    break;
  }

  entry_offset = global_bitmap_key / tbls->ihp.vrid_my_mac_map_tbl.addr.width_bits;
  bitmap_key = global_bitmap_key % tbls->ihp.vrid_my_mac_map_tbl.addr.width_bits;

  res = soc_pb_pp_ihp_vrid_my_mac_map_tbl_get_unsafe(
    unit,
    entry_offset,
    &vrid_my_mac_map_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  *enable = SOC_SAND_GET_BIT(vrid_my_mac_map_tbl_data.vrid_my_mac_map, bitmap_key);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_vrrp_mac_get_unsafe()", vrrp_id_ndx, 0);
}

/*********************************************************************
*     Set TRILL My-Nick-Name and reserved Nick-Name.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_trill_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_MYMAC_TRILL_INFO                        *trill_info
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    reg_vals[SOC_PB_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS];
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_TRILL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_info);

  regs = soc_pb_pp_regs();

  /* Nickname */
  SOC_PB_PP_FLD_SET(regs->eci.my_trill_nickname_reg.my_trill_nickname, trill_info->my_nick_name, 10, exit);
  SOC_PB_PP_FLD_SET(regs->epni.trill_config_reg.trill_nickname, trill_info->my_nick_name, 10, exit);
  
  /* Rbridges mac */
  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    &trill_info->all_rbridges_mac,
    mac_in_longs
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  SOC_PB_PP_FLD_TO_REG(regs->ihp.all_rbridges_mac_config_reg[0].all_rbridges_mac, mac_in_longs[0], reg_vals[0], 20, exit);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.all_rbridges_mac_config_reg[1].all_rbridges_mac, mac_in_longs[1], reg_vals[1], 30, exit);

  SOC_PB_PP_REG_BUFFER_SET(
    regs->ihp.all_rbridges_mac_config_reg,
    SOC_PB_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS,
    reg_vals,
    40,
    exit);

  /* Esadi mac */
  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    &trill_info->all_esadi_rbridges,
    mac_in_longs
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  SOC_PB_PP_FLD_TO_REG(regs->ihb.all_esadi_rbridges_reg[0].all_esadi_rbridges, mac_in_longs[0], reg_vals[0], 60, exit);
  SOC_PB_PP_FLD_TO_REG(regs->ihb.all_esadi_rbridges_reg[1].all_esadi_rbridges, mac_in_longs[1], reg_vals[1], 70, exit);

  SOC_PB_PP_REG_BUFFER_SET(
    regs->ihb.all_esadi_rbridges_reg,
    SOC_PB_PP_IHB_ALL_ESADI_RBRIDGES_REG_MULT_NOF_REGS,
    reg_vals,
    80,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_trill_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mymac_trill_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_MYMAC_TRILL_INFO                        *trill_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_TRILL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MYMAC_TRILL_INFO, trill_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_trill_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mymac_trill_info_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_TRILL_INFO_GET_VERIFY);

  /* Nothing to verify */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_trill_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set TRILL My-Nick-Name and reserved Nick-Name.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mymac_trill_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_MYMAC_TRILL_INFO                        *trill_info
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    reg_vals[SOC_PB_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS],
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MYMAC_TRILL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_info);

  SOC_PB_PP_MYMAC_TRILL_INFO_clear(trill_info);

  regs = soc_pb_pp_regs();

  /* Nickname */
  SOC_PB_PP_FLD_GET(regs->eci.my_trill_nickname_reg.my_trill_nickname, fld_val, 10, exit);

  trill_info->my_nick_name = fld_val;

  /* Rbridges mac */
  SOC_PB_PP_REG_BUFFER_GET(
    regs->ihp.all_rbridges_mac_config_reg,
    SOC_PB_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS,
    reg_vals,
    20,
    exit);

  SOC_PB_PP_FLD_FROM_REG(regs->ihp.all_rbridges_mac_config_reg[0].all_rbridges_mac, mac_in_longs[0], reg_vals[0], 30, exit);
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.all_rbridges_mac_config_reg[1].all_rbridges_mac, mac_in_longs[1], reg_vals[1], 40, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
    mac_in_longs,
    &trill_info->all_rbridges_mac
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /* Esadi mac */
  SOC_PB_PP_REG_BUFFER_GET(
    regs->ihb.all_esadi_rbridges_reg,
    SOC_PB_PP_IHB_ALL_ESADI_RBRIDGES_REG_MULT_NOF_REGS,
    reg_vals,
    50,
    exit);

  SOC_PB_PP_FLD_FROM_REG(regs->ihb.all_esadi_rbridges_reg[0].all_esadi_rbridges, mac_in_longs[0], reg_vals[0], 60, exit);
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.all_esadi_rbridges_reg[1].all_esadi_rbridges, mac_in_longs[1], reg_vals[1], 70, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
    mac_in_longs,
    &trill_info->all_esadi_rbridges
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mymac_trill_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the My-B-MAC. My-B-MAC forwards the
 *     packets to the I Component, and is added as the SA when
 *     sending toward the backbone network.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_my_bmac_msb_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_bmac_msb
  )
{
  uint32
	  mac_in_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
	  reg_val[SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS],
	  res = SOC_SAND_OK;
  SOC_PB_PP_REGS
	  *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_MSB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(my_bmac_msb);

  regs = soc_pb_pp_regs();

  SOC_PETRA_CLEAR(mac_in_long, uint32, SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S);
  SOC_PETRA_CLEAR(reg_val, uint32, SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    my_bmac_msb,
    mac_in_long
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_sand_bitstream_get_any_field(
	  mac_in_long,
	  0,
	  SOC_PB_PP_MY_BMAC_MSB_MSB + 1,
	  reg_val
	  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  /*
   * set mymac msb in egress
   */
  SOC_PB_PP_REG_SET(
	  regs->epni.bsa_reg_0,
	  reg_val[0],
	  10,
	  exit);
  SOC_PB_PP_REG_SET(
	  regs->epni.bsa_reg_1,
	  reg_val[1],
	  10,
	  exit);

  SOC_PETRA_CLEAR(reg_val, uint32, SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS);

    res = soc_sand_bitstream_get_any_field(
    mac_in_long,
	SOC_PB_PP_MY_BMAC_MSB_LSB,
    SOC_PB_PP_MY_BMAC_MSB_MSB + 1 - SOC_PB_PP_MY_BMAC_MSB_LSB,
    reg_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  /*
   * set mymac msb in ingress
   */
  SOC_PB_PP_REG_BUFFER_SET(
    regs->ihp.my_bmac_uc_msb_config_reg,
    SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS,
    reg_val,
    10,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_lif_my_bmac_msb_set_unsafe()",0,0);
}

uint32
  soc_pb_pp_lif_my_bmac_msb_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_bmac_msb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_MSB_SET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_my_bmac_msb_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_lif_my_bmac_msb_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_MSB_GET_VERIFY);

  /* Do nothing */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_my_bmac_msb_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the My-B-MAC. My-B-MAC forwards the
 *     packets to the I Component, and is added as the SA when
 *     sending toward the backbone network.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_my_bmac_msb_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                           *my_bmac_msb
  )
{
  uint32
	  mac_in_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
	  reg_val[SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS],
      res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_MSB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(my_bmac_msb);

  soc_sand_SAND_PP_MAC_ADDRESS_clear(my_bmac_msb);

  regs = soc_pb_pp_regs();

  SOC_PETRA_CLEAR(mac_in_long, uint32, SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S);
  SOC_PETRA_CLEAR(reg_val, uint32, SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS);

  SOC_PB_PP_REG_GET(
	  regs->epni.bsa_reg_0,
	  reg_val[0],
	  10,
	  exit);
  SOC_PB_PP_REG_GET(
	  regs->epni.bsa_reg_1,
	  reg_val[1],
	  10,
	  exit);

  res = soc_sand_bitstream_get_any_field(
	  reg_val,
	  0,
	  SOC_PB_PP_MY_BMAC_MSB_MSB + 1,
	  mac_in_long
	  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
	mac_in_long,
	my_bmac_msb
	);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_lif_my_bmac_msb_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the My-B-MAC LSB according to the source system
 *     port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_my_bmac_port_lsb_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                           *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_mac_lsb,
    SOC_SAND_IN  uint8                                     enable
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    bit_offset,
    field_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode, field_val, 5, exit);
  if (field_val != 0x2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_AUX_TABLE_MODE_MISMATCH_ERR, 7, exit);
  }


  if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_NOF_SYS_PORT_TYPES) { /* port is invalid - set global LSB */

	  	/* Set enable bit for this lsb */
  		reg_offset = my_mac_lsb->address[0] / SOC_SAND_NOF_BITS_IN_UINT32;
  		bit_offset = my_mac_lsb->address[0] % SOC_SAND_NOF_BITS_IN_UINT32;

  		SOC_PB_PP_FLD_GET(regs->ihp.my_bmac_uc_lsb_bitmap_reg[reg_offset].my_bmac_uc_lsb_bitmap, field_val, 10, exit);
  		SOC_SAND_SET_BIT(field_val, enable, bit_offset);
  		SOC_PB_PP_FLD_SET(regs->ihp.my_bmac_uc_lsb_bitmap_reg[reg_offset].my_bmac_uc_lsb_bitmap, field_val, 20, exit);
  }
  else { /* port is valid: set per-port LSB in egress (for encapsulation) */

      /* Configure entry in aux table */
      aux_table_tbl_data.auxtable = my_mac_lsb->address[0];

		  res = soc_pb_pp_egq_aux_table_tbl_set_unsafe(
    		unit,
    		src_sys_port_ndx->sys_id,
    		&aux_table_tbl_data
    	);
  		SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_my_bmac_port_lsb_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_lif_my_bmac_port_lsb_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                           *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                           *my_mac_lsb,
    SOC_SAND_IN  uint8                                     enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_SET_VERIFY);

  res = soc_pb_pp_lif_my_bmac_port_lsb_get_verify(
    unit,
    src_sys_port_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_my_bmac_port_lsb_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_lif_my_bmac_port_lsb_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                           *src_sys_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_GET_VERIFY);

  if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MYMAC_SYS_PORT_NON_SINGLE_PORT_NOT_SUPPORTED_ERR, 10, exit);
  }
  else if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT)
  {
      SOC_SAND_ERR_IF_ABOVE_MAX(src_sys_port_ndx->sys_id, SOC_PB_PP_SYS_PORT_MAX, SOC_PB_PP_MYMAC_SYS_PORT_OUT_OF_RANGE, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_my_bmac_port_lsb_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the My-B-MAC LSB according to the source system
 *     port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_my_bmac_port_lsb_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                           *src_sys_port_ndx,
    SOC_SAND_INOUT SOC_SAND_PP_MAC_ADDRESS                           *my_mac_lsb,
    SOC_SAND_OUT uint8                                     *enable
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    bit_offset,
    field_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode, field_val, 5, exit);
  if (field_val != 0x2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_MY_BMAC_PORT_LSB_AUX_TABLE_MODE_MISMATCH_ERR, 7, exit);
  }

  if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_NOF_SYS_PORT_TYPES) { /* port is invalid - get global LSB */

      /* Get enable bit for this lsb */
      reg_offset = my_mac_lsb->address[0] / SOC_SAND_NOF_BITS_IN_UINT32;

      SOC_PB_PP_FLD_GET(regs->ihp.my_bmac_uc_lsb_bitmap_reg[reg_offset].my_bmac_uc_lsb_bitmap, field_val, 10, exit);

      bit_offset = my_mac_lsb->address[0] % SOC_SAND_NOF_BITS_IN_UINT32;
      *enable = SOC_SAND_GET_BIT(field_val, bit_offset);
  }
  else { /* port is valid - get per-port LSB */

      /* Get entry from aux table */
      res = soc_pb_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          src_sys_port_ndx->sys_id,
          &aux_table_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      my_mac_lsb->address[0] = (uint8)aux_table_tbl_data.auxtable;
      *enable = TRUE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_my_bmac_port_lsb_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_mymac module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_mymac_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_mymac;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_mymac module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_mymac_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_mymac;
}

uint32
  SOC_PB_PP_MYMAC_VRRP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MYMAC_VRRP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_PP_MYMAC_MODE_MAX, SOC_PB_PP_MYMAC_MODE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MYMAC_VRRP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MYMAC_TRILL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MYMAC_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
 
  SOC_SAND_ERR_IF_ABOVE_MAX(info->my_nick_name, SOC_PB_PP_MYMAC_MY_NICK_NAME_MAX, SOC_PB_PP_MYMAC_MY_NICK_NAME_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MYMAC_TRILL_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

