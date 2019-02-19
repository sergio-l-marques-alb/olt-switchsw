/* $Id: pb_fabric.c,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_fabric.c
*
* MODULE PREFIX:  pb
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

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_fabric.h>

#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_serdes.h>
#include <soc/dpp/Petra/petra_fabric.h>

#include <soc/dpp/dpp_config_defs.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_CODING_MAX                    (SOC_PB_FABRIC_NOF_LINE_CODINGS-1)
#define SOC_PB_FABRIC_NOF_LINKS_PER_MAC      (SOC_DPP_DEFS_GET(unit, nof_fabric_links)/SOC_PB_BLK_NOF_INSTANCES_FABRIC_MAC)

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Sets fabric Line Coding and Error Correction mode
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fabric_line_coding_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              link_ndx,
    SOC_SAND_IN  SOC_PB_FABRIC_LINE_CODING                  coding
  )
{
  uint32
    res = SOC_SAND_OK,
    group_id,
    bit_id,
    bit_val,
    val;
  uint32
    srd_lane_glbl_id,
    lane_inner_id,
    srd_qrtt_glbl_id,
    srd_qrtt_inner_id,
    srd_instance_id,
    star_id,
    nif_grp_id;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_combo = FALSE,
    is_enabled;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FABRIC_LINE_CODING_SET_UNSAFE);

  regs = soc_petra_regs();

  switch(coding)
  {
  case SOC_PB_FABRIC_LINE_CODING_8_10:
    is_enabled = FALSE;
    break;
  
  case SOC_PB_FABRIC_LINE_CODING_8_9_FEC:
    is_enabled = TRUE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CODING_OUT_OF_RANGE_ERR, 10, exit);
  }

  group_id = link_ndx / SOC_PB_FABRIC_NOF_LINKS_PER_MAC;
  bit_id   = link_ndx % SOC_PB_FABRIC_NOF_LINKS_PER_MAC;

  SOC_PB_FLD_IGET(regs->fabric_mac.forward_error_correction_enabler_reg.use_fec, val, group_id, 15, exit);

  if(is_enabled)
  {
    val |= SOC_SAND_BIT(bit_id);
  }
  else
  {
    val &= (SOC_SAND_RBIT(bit_id));
  }
  SOC_PB_FLD_ISET(regs->fabric_mac.forward_error_correction_enabler_reg.use_fec, val, group_id, 20, exit);
  
  /*  In FEC mode, the SerDes must be configured for raw data mode. */
  srd_lane_glbl_id = soc_petra_fbr2srd_lane_id(link_ndx);
  srd_qrtt_glbl_id = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_lane_glbl_id);
  srd_qrtt_inner_id = SOC_PETRA_SRD_QRTT2INNER_ID(srd_qrtt_glbl_id);
  lane_inner_id = SOC_PETRA_SRD_LANE2INNER_ID(srd_lane_glbl_id);
  star_id = SOC_PETRA_SRD_LANE2STAR(srd_lane_glbl_id);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);


  SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_inner_id][lane_inner_id].ext_tx_data_ovrd_en, is_enabled, srd_instance_id, 30, exit);
  
  if (SOC_PETRA_SRD_IS_COMBO_LANE(srd_lane_glbl_id))
  {
    is_combo = soc_petra_is_fabric_quartet(
                 unit,
                 srd_qrtt_glbl_id
               );
    bit_val = SOC_SAND_BOOL2NUM(is_combo && (coding == SOC_PB_FABRIC_LINE_CODING_8_9_FEC));

    nif_grp_id = soc_petra_srd_qrtt2combo_id(srd_qrtt_glbl_id);
    SOC_PB_FLD_IGET(regs->nif.nif_config_reg.fabric_fec_mode, val, nif_grp_id, 12, exit);
    SOC_SAND_SET_BIT(val, bit_val, lane_inner_id);
    SOC_PB_FLD_ISET(regs->nif.nif_config_reg.fabric_fec_mode, val, nif_grp_id, 14, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fabric_line_coding_set_unsafe()", link_ndx, 0);
}

uint32
  soc_pb_fabric_line_coding_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              link_ndx,
    SOC_SAND_IN  SOC_PB_FABRIC_LINE_CODING                  coding
  )
{
  uint8
    is_fbr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FABRIC_LINE_CODING_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links)-1, SOC_PB_FBR_NOF_LINKS_OUT_OF_RANGE_ERR, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(coding, SOC_PB_CODING_MAX, SOC_PB_CODING_OUT_OF_RANGE_ERR, 10, exit);
  is_fbr = soc_petra_fabric_is_fabric_link(unit, link_ndx);
  SOC_SAND_ERR_IF_EQUALS_VALUE(is_fbr, FALSE, SOC_SAND_ERR, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fabric_line_coding_set_verify()", link_ndx, 0);
}

uint32
  soc_pb_fabric_line_coding_get_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              link_ndx
  )
{
  uint8
    is_fbr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FABRIC_LINE_CODING_GET_VERIFY);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links)-1, SOC_PB_FBR_NOF_LINKS_OUT_OF_RANGE_ERR, 5, exit);
  
  is_fbr = soc_petra_fabric_is_fabric_link(unit, link_ndx);
  SOC_SAND_ERR_IF_EQUALS_VALUE(is_fbr, FALSE, SOC_SAND_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fabric_line_coding_get_verify()", link_ndx, 0);
}

/*********************************************************************
*     Sets fabric Line Coding and Error Correction mode
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fabric_line_coding_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              link_ndx,
    SOC_SAND_OUT SOC_PB_FABRIC_LINE_CODING                  *coding
  )
{
  uint32
    res = SOC_SAND_OK,
    group_id,
    bit_id,
    val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FABRIC_LINE_CODING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(coding);

  regs = soc_petra_regs();

  group_id = link_ndx / SOC_PB_FABRIC_NOF_LINKS_PER_MAC;
  bit_id   = link_ndx % SOC_PB_FABRIC_NOF_LINKS_PER_MAC;

  SOC_PB_FLD_IGET(regs->fabric_mac.forward_error_correction_enabler_reg.use_fec, val, group_id, 10, exit);

  *coding = SOC_PB_FABRIC_LINE_CODING_8_10;
  if(val & SOC_SAND_BIT(bit_id))
  {
    *coding = SOC_PB_FABRIC_LINE_CODING_8_9_FEC;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fabric_line_coding_get_unsafe()", link_ndx, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

