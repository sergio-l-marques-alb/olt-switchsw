/* $Id: petra_fabric.c,v 1.14 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_api_fabric.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_serdes.h>
#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_api_fabric.h>
#include <soc/dpp/Petra/PB_TM/pb_fabric.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* Soc_petra connect mode definitions */
#define SOC_PETRA_FBC_CONNECT_FE_VAL_1        (0x0)
#define SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_1   (0x58)
#define SOC_PETRA_FBC_CONNECT_BACK2BACK_VAL_1 (0x22)
#define SOC_PETRA_FBC_CONNECT_MESH_VAL_1      (0x23)

#define SOC_PETRA_FBC_CONNECT_FE_VAL_2        (0x0)
#define SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_2   (0x0)
#define SOC_PETRA_FBC_CONNECT_BACK2BACK_VAL_2 (0x1)
#define SOC_PETRA_FBC_CONNECT_MESH_VAL_2      (0x1)
#define SOC_PETRA_FBC_CONNECT_MESH_VAL_3      (0x1)


/* } */

/*************
 *  MACROS   *
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
* NAME:
*     soc_petra_fabric_regs_init
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
  soc_petra_fabric_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  uint32
    mac_id;
  uint8
    is_var_size_cells;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_REGS_INIT);

  regs = soc_petra_regs();

  is_var_size_cells = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);

  if (is_var_size_cells)
  {
    SOC_PETRA_FLD_SET(regs->fdr.link_level_flow_control_reg.lnk_lvl_fc_th, 52, 10, exit);
  }

  for (mac_id = 0; mac_id < SOC_DPP_DEFS_GET(unit, nof_fabric_macs); mac_id++)
  {
    SOC_PETRA_FLD_ISET(regs->fabric_mac.control_cell_burst_and_tdmreg_reg.cntrl_burst_period, 0x10, mac_id, 19, exit);
    SOC_PETRA_FLD_ISET(regs->fabric_mac.control_cell_burst_and_tdmreg_reg.max_cntrl_cell_burst, 0x0, mac_id, 21, exit);
    SOC_PETRA_FLD_ISET(regs->fabric_mac.control_cell_burst_and_tdmreg_reg.tdmfragment_number, 0xc0, mac_id, 23, exit);
    SOC_PETRA_FLD_ISET(regs->fabric_mac.link_level_flow_control_and_comma_configuration_reg.cm_tx_period, 0x7, mac_id, 25, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_regs_init()",0,0);
}

uint8
  soc_petra_is_fabric_quartet(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 qrtt_ndx
  )
{
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint8
    is_combo_fbr,
    is_fbr = FALSE;
  SOC_PETRA_COMBO_QRTT
    combo_idx;

  star_id = SOC_PETRA_SRD_QRTT2STAR_ID(qrtt_ndx);

  if (SOC_SAND_IS_VAL_IN_RANGE(star_id, SOC_PETRA_SRD_STAR_ID_2, SOC_PETRA_SRD_STAR_ID_3))
  {
    is_fbr = TRUE;
  }
  else
  {
    combo_idx = soc_petra_srd_qrtt2combo_id(qrtt_ndx);

    if (combo_idx < SOC_PETRA_COMBO_NOF_QRTTS)
    {
      is_combo_fbr = !(soc_petra_sw_db_combo_nif_not_fabric_get(unit, combo_idx));
      if (is_combo_fbr)
      {
        if (SOC_PETRA_SRD_IS_COMBO_BY_IDX(SOC_PETRA_SRD_QRTT_FIRST_LANE(qrtt_ndx), combo_idx))
        {
          is_fbr = TRUE;
        }
      }
    }
  }

  return is_fbr;
}

/*********************************************************************
* NAME:
*     soc_petra_fabric_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*   Note! Must run after the SerDes initialization.
*********************************************************************/
uint32
  soc_petra_fabric_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_FABRIC_FC
    fc;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    link_i,
    srd_qrtt_id_global,
    srd_qrtt_id,
    srd_instance_id;
  uint8
    is_srd_qrtt_active;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_INIT);

  regs = soc_petra_regs();

  /*
   *  Set default - enabled.
   */
  soc_petra_PETRA_FABRIC_FC_clear(unit, &fc);

  res = soc_petra_fabric_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  SerDes configuration
   *  And Fabric Link Out-Of-Reset (for powered-up SerDes only)
   */
  for (srd_qrtt_id_global = 0; srd_qrtt_id_global < SOC_PETRA_SRD_NOF_QUARTETS; srd_qrtt_id_global++)
  {
    star_id = SOC_PETRA_SRD_QRTT2STAR_ID(srd_qrtt_id_global);
    srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
    srd_qrtt_id = SOC_PETRA_SRD_QRTT2INNER_ID(srd_qrtt_id_global);

    /*
     *  Check if the SerDes quartet is initialized.
     *  Skip SerDes configuration otherwise
     */
    SOC_PETRA_FLD_IGET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].cmu_macro_rst, fld_val, srd_instance_id, 10, exit);
    is_srd_qrtt_active = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

    if (is_srd_qrtt_active)
    {
      if (soc_petra_is_fabric_quartet(unit, srd_qrtt_id_global))
      {
        res = soc_petra_fabric_srd_qrtt_reset_unsafe(
                unit,
                srd_qrtt_id_global,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      } /* Fabric Quartet */
    }/* Active SerDes quartet */
  } /* Loop on SerDes quartets */

  for (link_i=0; link_i<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_i)
  {
    fc.enable[link_i] = TRUE;
  }

  res = soc_petra_fabric_fc_enable_set_unsafe(
          unit,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          &fc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_init()",0,0);
}

uint32
  soc_petra_fabric_srd_qrtt_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 srd_qrtt_ndx,
    SOC_SAND_IN  uint8                 is_oor
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    link_i,
    srd_qrtt_id,
    srd_lane_id,
    srd_lane_id_global,
    srd_instance_id,
    link_in_mac = 0,
    mac_of_link;
  uint8
    is_srd_up_rx,
    is_srd_up_tx;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_rx,
    srd_pwr_tx;
  SOC_PETRA_LINK_STATE_INFO
    link_state;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_SRD_QRTT_RESET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    srd_qrtt_ndx, SOC_PETRA_SRD_NOF_QUARTETS-1,
    SOC_PETRA_SRD_STAR_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  star_id = SOC_PETRA_SRD_QRTT2STAR_ID(srd_qrtt_ndx);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_QRTT2INNER_ID(srd_qrtt_ndx);

  soc_petra_PETRA_LINK_STATE_INFO_clear(&link_state);
  link_state.serdes_also = FALSE;

  /*
   *  Configure The Out-Of-Reset parameters
   */
  if (is_oor)
  {
    if (soc_petra_is_fabric_quartet(unit, srd_qrtt_ndx) == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SRD_NOT_FABRIC_QUARTET_ERR, 20, exit);
    }
    SOC_PETRA_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, 0x1, srd_instance_id, 90, exit);
    
    res = soc_petra_srd_qrtt_sync_fifo_en(
            unit,
            srd_qrtt_ndx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);

    for (srd_lane_id = 0; srd_lane_id < SOC_PETRA_SRD_NOF_LANES_PER_QRTT; srd_lane_id++)
    {
      srd_lane_id_global = srd_qrtt_ndx * SOC_PETRA_SRD_NOF_LANES_PER_QRTT + srd_lane_id;

      SOC_PETRA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_align_en_req, 0x1, srd_instance_id, 94, exit);
      SOC_PETRA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].rx_nearest_comma_align, 0x0, srd_instance_id, 96, exit);
      SOC_PETRA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_lsb, 0x1, srd_instance_id, 98, exit);
      SOC_PETRA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_msb, 0x1, srd_instance_id, 100, exit);
      SOC_PETRA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].tx_use_ext_byte_toggle, 0x0, srd_instance_id, 110, exit);

      res = soc_petra_srd_lane_power_state_get_unsafe(
              unit,
              srd_lane_id_global,
              &srd_pwr_rx,
              &srd_pwr_tx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

      is_srd_up_rx = SOC_SAND_NUM2BOOL(srd_pwr_rx == SOC_PETRA_SRD_POWER_STATE_UP);
      is_srd_up_tx = SOC_SAND_NUM2BOOL(srd_pwr_tx == SOC_PETRA_SRD_POWER_STATE_UP);
      link_i = soc_petra_srd2fbr_link_id(srd_lane_id_global);

      if (is_srd_up_rx && is_srd_up_tx)
      {
        link_state.on_off = SOC_PETRA_LINK_STATE_ON;
        res = soc_petra_link_on_off_set_unsafe(
                unit,
                link_i,
                &link_state
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
      }
      else
      {
        link_in_mac = SOC_PETRA_FBC_LINK_IN_MAC(link_i);
        mac_of_link = SOC_PETRA_FBC_MAC_OF_LINK(link_i);
        /*
         *  Take fabric link out-of-reset - the requested direction
         */
        if (is_srd_up_tx)
        {
          SOC_PETRA_FLD_ISET(regs->fabric_mac.receive_reset_reg.fmactx_rst_n[link_in_mac], 0x0, mac_of_link, 130, exit);
        }

        if (is_srd_up_rx)
        {
          SOC_PETRA_FLD_ISET(regs->fabric_mac.receive_reset_reg.fmacrx_rst_n[link_in_mac], 0x0, mac_of_link, 135, exit);
        }
      }
    } /* Loop on SerDes lanes */
  }/* OOR */
  else
  {
    for (srd_lane_id = 0; srd_lane_id < SOC_PETRA_SRD_NOF_LANES_PER_QRTT; srd_lane_id++)
    {
      srd_lane_id_global = srd_qrtt_ndx * SOC_PETRA_SRD_NOF_LANES_PER_QRTT + srd_lane_id;
      link_i = soc_petra_srd2fbr_link_id(srd_lane_id_global);
      link_state.on_off = SOC_PETRA_LINK_STATE_OFF;
      res = soc_petra_link_on_off_set_unsafe(
              unit,
              link_i,
              &link_state
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_srd_qrtt_reset_unsafe()",srd_qrtt_ndx,0);
}


/*********************************************************************
*     This procedure enables/disables flow-control on fabric
*     links.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC            *info
  )
{
  uint32
    fld_val = 0,
    res;
  uint32
    link_idx = 0,
    link_in_mac = 0,
    mac_of_link;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FC_ENABLE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  for (link_idx = 0; link_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_idx)
  {
    fld_val = SOC_SAND_BOOL2NUM(info->enable[link_idx]);
    link_in_mac = SOC_PETRA_FBC_LINK_IN_MAC(link_idx);
    mac_of_link = SOC_PETRA_FBC_MAC_OF_LINK(link_idx);
    /*
     *	CAUTION: the following register has in practice a different
     *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
     */

    if (tx_enable)
    {
      SOC_PETRA_FLD_ISET(regs->fabric_mac.loopback_and_link_level_flow_control_enable_reg[link_in_mac].lnk_lvl_fc_tx_en, fld_val, mac_of_link, 10, exit);
    }

    if (rx_enable)
    {
      SOC_PETRA_FLD_ISET(regs->fabric_mac.loopback_and_link_level_flow_control_enable_reg[link_in_mac].lnk_lvl_fc_rx_en, fld_val, mac_of_link, 20, exit);
    }

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fc_enable_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure enables/disables flow-control on fabric
*     links.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC            *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FC_ENABLE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_NOF_CONNECTION_DIRECTIONS,
    SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fc_enable_verify()",0,0);
}

/*********************************************************************
*     This procedure enables/disables flow-control on fabric
*     links.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC            *info_rx,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC            *info_tx
  )
{
  uint32
    tx_fld_val = 0,
    rx_fld_val = 0,
    res;
  uint32
    link_idx = 0,
    link_in_mac = 0,
    mac_of_link;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_FABRIC_FC
    info_local_rx,
    info_local_tx;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FC_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  regs = soc_petra_regs();

  soc_petra_PETRA_FABRIC_FC_clear(unit, &info_local_rx);
  soc_petra_PETRA_FABRIC_FC_clear(unit, &info_local_tx);

  for (link_idx = 0; link_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_idx)
  {
    link_in_mac = SOC_PETRA_FBC_LINK_IN_MAC(link_idx);
    mac_of_link = SOC_PETRA_FBC_MAC_OF_LINK(link_idx);

    SOC_PETRA_FLD_IGET(regs->fabric_mac.loopback_and_link_level_flow_control_enable_reg[link_in_mac].lnk_lvl_fc_tx_en, tx_fld_val, mac_of_link, 10, exit);
    info_local_tx.enable[link_idx] = SOC_SAND_NUM2BOOL(tx_fld_val);

    SOC_PETRA_FLD_IGET(regs->fabric_mac.loopback_and_link_level_flow_control_enable_reg[link_in_mac].lnk_lvl_fc_rx_en, rx_fld_val, mac_of_link, 20, exit);
    info_local_rx.enable[link_idx] = SOC_SAND_NUM2BOOL(rx_fld_val);

  }

  SOC_PETRA_COPY(info_rx, &info_local_rx, SOC_PETRA_FABRIC_FC, 1);
  SOC_PETRA_COPY(info_tx, &info_local_tx, SOC_PETRA_FABRIC_FC, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fc_enable_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets fabric links operation mode.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_cell_format_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CELL_FORMAT  *info
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CELL_FORMAT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->fdt.fdt_enabler_reg, reg_val, 10, exit);
  SOC_PETRA_REGS_FDT_ENABLER_READ_ADJUST(reg_val);

  fld_val = SOC_SAND_BOOL2NUM(info->segmentation_enable);
  SOC_PETRA_FLD_TO_REG(regs->fdt.fdt_enabler_reg.segment_pkt, fld_val, reg_val, 15, exit);

  SOC_PETRA_REG_SET(regs->fdt.fdt_enabler_reg, reg_val, 20, exit);

  fld_val = SOC_SAND_BOOL2NUM(!(info->variable_cell_size_enable));
  SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.fsc_en, fld_val, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_cell_format_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets fabric links operation mode.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_cell_format_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT  *info
  )
{
  uint32
    fld_val = 0,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CELL_FORMAT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->fdt.fdt_enabler_reg, reg_val, 10, exit);
  SOC_PETRA_REGS_FDT_ENABLER_READ_ADJUST(reg_val);

  SOC_PETRA_FLD_FROM_REG(regs->fdt.fdt_enabler_reg.segment_pkt, fld_val, reg_val, 15, exit);

  info->segmentation_enable = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.fsc_en, fld_val, 20, exit);
  info->variable_cell_size_enable = SOC_SAND_NUM2BOOL(!fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_cell_format_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_coexist_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  uint32
    fld_val = 0,
    reg_val,
    coex_reg_val[SOC_PETRA_NOF_COEXIST_MAP_REGS],
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    fld_idx = 0,
    regs_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_COEXIST_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /* Clear the registers buffer */
  sal_memset(
    coex_reg_val,
    0x0,
    sizeof(coex_reg_val)
  );

  SOC_PETRA_REG_GET(regs->fdt.fdt_enabler_reg, reg_val, 10, exit);
  SOC_PETRA_REGS_FDT_ENABLER_READ_ADJUST(reg_val);

  fld_val = SOC_SAND_BOOL2NUM(info->enable);

  SOC_PETRA_FLD_TO_REG(regs->fdt.fdt_enabler_reg.use_coex_in_multicast, fld_val, reg_val, 15, exit);

  SOC_PETRA_REG_SET(regs->fdt.fdt_enabler_reg, reg_val, 20, exit);

  /* Build the bitmap */
  for (fld_idx = 0; fld_idx < SOC_PETRA_FABRIC_NOF_COEXIST_DEV_ENTRIES; fld_idx ++)
  {
    regs_idx = SOC_PETRA_REG_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS);
    fld_val = SOC_SAND_BOOL2NUM(info->coexist_dest_map[fld_idx]);
    SOC_SAND_SET_BIT(
      coex_reg_val[regs_idx],
      fld_val,
      SOC_PETRA_FLD_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS)
    );
  }

  /* Write the bitmap to registers - coexist destinations */
  for (regs_idx = 0; regs_idx < SOC_PETRA_NOF_COEXIST_MAP_REGS; regs_idx++)
  {
    SOC_PETRA_REG_SET(regs->fdt.coexist_dupx2_unicast_bitmap_reg[regs_idx], coex_reg_val[regs_idx], 20, exit);
  }

  /* Clear the registers buffer */
  sal_memset(
    coex_reg_val,
    0x0,
    sizeof(coex_reg_val)
  );

  /* Write the bitmap to registers - coexist source id odd/even */
  for (fld_idx = 0; fld_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); fld_idx ++)
  {
    regs_idx = SOC_PETRA_REG_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS);
    fld_val = SOC_SAND_BOOL2NUM(info->fap_id_odd_link_map[fld_idx]);
    SOC_SAND_SET_BIT(
      coex_reg_val[regs_idx],
      fld_val,
      SOC_PETRA_FLD_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS)
    );
  }

  for (regs_idx = 0; regs_idx < SOC_PETRA_NOF_BIT_PER_LINK_REGS; regs_idx++)
  {
     SOC_PETRA_REG_SET(regs->rtp.rtp_coexist_configuration_reg[regs_idx], coex_reg_val[regs_idx], 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_coexist_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_coexist_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_COEXIST_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_coexist_verify()",0,0);
}

/*********************************************************************
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_coexist_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  uint32
    fld_val = 0,
    reg_val,
    coex_reg_val[SOC_PETRA_NOF_COEXIST_MAP_REGS],
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    fld_idx = 0,
    regs_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_COEXIST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /* Clear the registers buffer */
  sal_memset(
    coex_reg_val,
    0x0,
    sizeof(coex_reg_val)
  );

  SOC_PETRA_REG_GET(regs->fdt.fdt_enabler_reg, reg_val, 10, exit);
  SOC_PETRA_REGS_FDT_ENABLER_READ_ADJUST(reg_val);

  SOC_PETRA_FLD_FROM_REG(regs->fdt.fdt_enabler_reg.use_coex_in_multicast, fld_val, reg_val, 15, exit);

  info->enable = SOC_SAND_NUM2BOOL(fld_val);

  /* read the bitmap to registers - coexist destinations */
  for (regs_idx = 0; regs_idx < SOC_PETRA_NOF_COEXIST_MAP_REGS; regs_idx++)
  {
    SOC_PETRA_REG_GET(regs->fdt.coexist_dupx2_unicast_bitmap_reg[regs_idx], (coex_reg_val[regs_idx]), 20, exit);
  }

  /* Read the bitmap */
  for (fld_idx = 0; fld_idx < SOC_PETRA_FABRIC_NOF_COEXIST_DEV_ENTRIES; fld_idx ++)
  {
    regs_idx = SOC_PETRA_REG_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS);
    fld_val =
      SOC_SAND_GET_BIT(
        coex_reg_val[regs_idx],
        SOC_PETRA_FLD_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS)
      );
    info->coexist_dest_map[fld_idx] = SOC_SAND_NUM2BOOL(fld_val );
  }

  /* Clear the registers buffer */
  sal_memset(
    coex_reg_val,
    0x0,
    sizeof(coex_reg_val)
  );

  /* read the bitmap to registers - coexist source id odd/even */
  for (regs_idx = 0; regs_idx < SOC_PETRA_NOF_BIT_PER_LINK_REGS; regs_idx++)
  {
     SOC_PETRA_REG_GET(regs->rtp.rtp_coexist_configuration_reg[regs_idx], (coex_reg_val[regs_idx]), 30, exit);
  }

  for (fld_idx = 0; fld_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); fld_idx ++)
  {
    regs_idx = SOC_PETRA_REG_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS);
    fld_val =
      SOC_SAND_GET_BIT(
        coex_reg_val[regs_idx],
        SOC_PETRA_FLD_IDX_GET(fld_idx, SOC_SAND_REG_SIZE_BITS)
      );
    info->fap_id_odd_link_map[fld_idx] = SOC_SAND_NUM2BOOL(fld_val);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_coexist_get_unsafe()",0,0);
}


/*********************************************************************
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_SET_UNSAFE);

  regs = soc_petra_regs();

  fld_val = SOC_SAND_BOOL2NUM(is_single_fap_mode);

  SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_topology_reg.stan_aln, fld_val, 10, exit);
  SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_topology_reg.stan_aln2, fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_stand_alone_fap_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_stand_alone_fap_mode_verify()",0,0);
}

/*********************************************************************
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_single_fap_mode);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->mesh_topology.mesh_topology_reg.stan_aln, fld_val, 10, exit);
  *is_single_fap_mode = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_stand_alone_fap_mode_get_unsafe()",0,0);
}

/*********************************************************************
*     This function should be called before calling to to
*     soc_petra_set_single_fap_mode() The value in 'is_standalone'
*     indicates on-line if the device is in single FAP mode.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_standalone_fap_mode_detect_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_standalone
  )
{
  uint32
    res;
  uint32
    fld_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_STANDALONE_FAP_MODE_DETECT_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_standalone);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->mesh_topology.fap_detect_ctrl_cells_cnt_reg.rcv_ctl1, fld_val, 10, exit);

  sal_msleep(150);

  SOC_PETRA_FLD_GET(regs->mesh_topology.fap_detect_ctrl_cells_cnt_reg.rcv_ctl1, fld_val, 20, exit);

  *is_standalone = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_standalone_fap_mode_detect_unsafe()",0,0);
}

/*********************************************************************
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode
  )
{
  uint32
    clk_freq,
    fld_val,
    res,
    rate_kbps = 0,
    value_1,
    value_2,
    value_3;
  uint8
    is_fec,
    is_single_mesh_cntxt;
  SOC_PETRA_REGS
    *regs;
  uint32
    lane_ndx;
  SOC_PETRA_SRD_POWER_STATE
    state_rx,
    state_tx;
#ifdef LINK_PB_LIBRARIES
  SOC_PB_FABRIC_LINE_CODING
    coding;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_SET_UNSAFE);

  regs = soc_petra_regs();

  is_single_mesh_cntxt = soc_petra_sw_db_is_fap2x_in_system_get(unit);
  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.mesh_mode, fld_val, 2, exit);

  if ((fld_val == 0x1) && (fabric_mode != SOC_PETRA_FABRIC_CONNECT_MODE_MESH))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FABRIC_CONNECT_MESH_MODE_CHANGE_ERR, 5, exit);
  }

  switch(fabric_mode)
  {
  case SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP:
  case SOC_PETRA_FABRIC_CONNECT_MODE_FE:

    value_1 = SOC_PETRA_FBC_CONNECT_FE_VAL_1;
    value_2 = SOC_PETRA_FBC_CONNECT_FE_VAL_2;
    break;

  case SOC_PETRA_FABRIC_CONNECT_MODE_MULT_STAGE_FE:

    value_1 = SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_1;
    value_2 = SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_2;
    break;

  case SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK:

    value_1 = SOC_PETRA_FBC_CONNECT_BACK2BACK_VAL_1;
    value_2 = SOC_PETRA_FBC_CONNECT_BACK2BACK_VAL_2;
    break;

  case SOC_PETRA_FABRIC_CONNECT_MODE_MESH:

    value_1 = SOC_PETRA_FBC_CONNECT_MESH_VAL_1;
    value_2 = SOC_PETRA_FBC_CONNECT_MESH_VAL_2;
    value_3 = SOC_PETRA_FBC_CONNECT_MESH_VAL_3;
    if (!is_single_mesh_cntxt)
    {
      SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.mesh_mode, value_3, 7, exit);
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 10, exit);
  }

  SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_topology_reg.in_system, value_1, 20, exit);

  SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_topology_reg.multi_fap, value_2, 30, exit);

  if (
      (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_FE) ||
      (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP) ||
      (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MULT_STAGE_FE)
    )
  {
    SOC_PETRA_REG_SET(regs->mesh_topology.init_reg, 0x22d, 35, exit);
  }
  else
  {
    SOC_PETRA_REG_SET(regs->mesh_topology.init_reg, 0x200, 37, exit);
  }
  
  if(fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP)
  {
    /* Set Force Local and disable Force Fabric */
    SOC_PETRA_FLD_SET(regs->ipt.force_local_or_fabric_reg.force_local, 1, 40, exit);
    SOC_PETRA_FLD_SET(regs->ipt.force_local_or_fabric_reg.force_fabric, 0, 41, exit);
  }
  else
  {
    /* Disable Force Local */
    SOC_PETRA_FLD_SET(regs->ipt.force_local_or_fabric_reg.force_local, 0, 42, exit);
  }

  if (
      (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK) ||
      (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MESH)
      )
  {
    /*
     * Find the lane rate
     */
    for(lane_ndx = 0 ; lane_ndx < SOC_PETRA_SRD_NOF_LANES; lane_ndx++)
    {
      if(soc_petra_srd_is_fabric_lane(unit, lane_ndx))
      {
        res = soc_petra_srd_lane_power_state_get_unsafe(
                unit,
                lane_ndx,
                &state_rx,
                &state_tx
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if ((state_rx != SOC_PETRA_SRD_POWER_STATE_DOWN) || (state_tx != SOC_PETRA_SRD_POWER_STATE_DOWN))
        {
          rate_kbps = soc_petra_srd_rate_calc_kbps(unit, lane_ndx);
          break; /* The lane rate is found */
        }
      }
    }

    if(rate_kbps == 0)
    {
      /* Running in Back2back or Mesh with all SerDes powered down */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 15, exit);
    }
    
    /*
     * Get core clock and if FEC is enabled (Soc_petra-B)
     */
    is_fec = FALSE;
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      res = soc_pb_fabric_line_coding_get_unsafe(
              unit,
              soc_petra_srd2fbr_link_id(lane_ndx),
              &coding
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      if (coding == SOC_PB_FABRIC_LINE_CODING_8_9_FEC)
      {
        is_fec = TRUE;
      }
    }
#endif

    SOC_PETRA_DIFF_DEVICE_CALL(mgmt_core_frequency_get_unsafe,(unit, &clk_freq));

    fld_val = (621875 * clk_freq) / rate_kbps + (is_fec? (390625000 / rate_kbps): (72 * clk_freq / 1000));
    SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_topology_reg.in_system, fld_val / 2 + 1, 30, exit);
    SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_topology_reg.fld0, fld_val / 2, 30, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_connect_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
      fabric_mode,  SOC_PETRA_FABRIC_NOF_CONNECT_MODES - 1,
      SOC_PETRA_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_connect_mode_verify()",0,0);
}

/*********************************************************************
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CONNECT_MODE *fabric_mode
  )
{
  uint32
    res;
  uint32
    value_1,
    value_2;
  uint8
    is_a1_and_coexist;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fabric_mode);

  regs = soc_petra_regs();

  is_a1_and_coexist = soc_petra_sw_db_is_fap20_and_a1_in_system_get(unit);
  

  SOC_PETRA_FLD_GET(regs->mesh_topology.mesh_topology_reg.in_system, value_1, 10, exit);
  SOC_PETRA_FLD_GET(regs->mesh_topology.mesh_topology_reg.multi_fap, value_2, 20, exit);

  if (is_a1_and_coexist)
  {
    if (
        (value_2 == SOC_PETRA_FBC_CONNECT_FE_VAL_2) ||
        (value_2 == SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_2)
       )
    {
      *fabric_mode = SOC_PETRA_FABRIC_CONNECT_MODE_FE;
    }
    else if (
        (value_2 == SOC_PETRA_FBC_CONNECT_MESH_VAL_2) ||
        (value_2 == SOC_PETRA_FBC_CONNECT_BACK2BACK_VAL_2)
       )
    {
      *fabric_mode = SOC_PETRA_FABRIC_CONNECT_MODE_MESH;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 25, exit);
    }
  }
  else
  {
    if      ((value_1 == SOC_PETRA_FBC_CONNECT_FE_VAL_1) && (value_2 == SOC_PETRA_FBC_CONNECT_FE_VAL_2))
    {
      SOC_PETRA_FLD_GET(regs->ipt.force_local_or_fabric_reg.force_local, value_1, 23, exit);
      *fabric_mode = (value_1 == 0x1) ?SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP: SOC_PETRA_FABRIC_CONNECT_MODE_FE;
    }
    else if ((value_1 == SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_1) && (value_2 == SOC_PETRA_FBC_CONNECT_FE_MULT_VAL_2))
    {
      *fabric_mode = SOC_PETRA_FABRIC_CONNECT_MODE_MULT_STAGE_FE;
    }
    else if ((value_1 == SOC_PETRA_FBC_CONNECT_MESH_VAL_1) && (value_2 == SOC_PETRA_FBC_CONNECT_MESH_VAL_2))
    {
      *fabric_mode = SOC_PETRA_FABRIC_CONNECT_MODE_MESH;
    }
    else if (value_2 == SOC_PETRA_FBC_CONNECT_BACK2BACK_VAL_2)
    {
      *fabric_mode = SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 30, exit);
    }
  } /* Not A1 and Coexist */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_connect_mode_get_unsafe()",0,0);
}

/* $Id: petra_fabric.c,v 1.14 Broadcom SDK $
 *	'is_mesh' is TRUE if fabric connection mode
 *  is either MESH or BACK2BACK.
 *  Most configurations are identical for these modes,
 *  and both are often referred to as 'MESH'
 */
uint32
  soc_petra_fabric_is_mesh(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_mesh,
    SOC_SAND_OUT uint8 *is_single_context
  )
{
  uint32
    res;
  uint8
    mesh_not_clos;
  SOC_PETRA_FABRIC_CONNECT_MODE
    fabric_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_mesh);

  res = soc_petra_fabric_connect_mode_get_unsafe(
          unit,
          &fabric_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mesh_not_clos = SOC_SAND_NUM2BOOL(
                 (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK) ||
                 (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MESH)
                );
  *is_mesh = mesh_not_clos;

  *is_single_context = SOC_SAND_NUM2BOOL(!(mesh_not_clos) || soc_petra_sw_db_is_fap2x_in_system_get(unit));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_is_mesh()",0,0);
}

/*
 *	'is_fabric' is TRUE if fabric connection mode
 *  is either FE or MULT_STAGE_FE.
 *  Most configurations are identical for these modes,
 *  and both are often referred to as 'Fabric'
 */
uint32
  soc_petra_fabric_is_fabric(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_fabric
  )
{
  uint32
    res;
  SOC_PETRA_FABRIC_CONNECT_MODE
    fabric_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_fabric);

  res = soc_petra_fabric_connect_mode_get_unsafe(
          unit,
          &fabric_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *is_fabric = SOC_SAND_NUM2BOOL(
                 (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_FE) ||
                 (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MULT_STAGE_FE)
                );
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_is_fabric()",0,0);
}


/*********************************************************************
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_IN  uint8                 is_fap20_device
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    reg_idx = 0,
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FAP20_MAP_SET_UNSAFE);

  regs = soc_petra_regs();

  reg_idx = SOC_PETRA_REG_IDX_GET(other_device_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(other_device_ndx, SOC_SAND_REG_SIZE_BITS);

  SOC_PETRA_REG_GET(regs->fcr.fap20b_destination_id_bitmap_reg[reg_idx], fld_val, 10, exit);

  SOC_SAND_SET_BIT(fld_val, SOC_SAND_BOOL2NUM(is_fap20_device), fld_idx);

  SOC_PETRA_REG_SET(regs->fcr.fap20b_destination_id_bitmap_reg[reg_idx], fld_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fap20_map_set_unsafe()",0,0);
}

/*********************************************************************
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_IN  uint8                 is_fap20_device
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FAP20_MAP_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    other_device_ndx, SOC_PETRA_FABRIC_NOF_FAP20_DEVICES - 1,
    SOC_PETRA_FABRIC_FAP20_ID_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fap20_map_verify()",0,0);
}

/*********************************************************************
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_OUT uint8                 *is_fap20_device
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    reg_idx = 0,
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FAP20_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_fap20_device);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    other_device_ndx, SOC_PETRA_FABRIC_NOF_FAP20_DEVICES - 1,
    SOC_PETRA_FABRIC_FAP20_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  reg_idx = SOC_PETRA_REG_IDX_GET(other_device_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(other_device_ndx, SOC_SAND_REG_SIZE_BITS);

  SOC_PETRA_REG_GET(regs->fcr.fap20b_destination_id_bitmap_reg[reg_idx], fld_val, 10, exit);

  *is_fap20_device = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, fld_idx));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fap20_map_get_unsafe()",0,0);
}

uint32
  soc_petra_fabric_links_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 read_crc_counter,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL    *links_status
  )
{
  uint32
    is_reset = 0,
    fec_en = 0,
    crc_errs = 0,
    misalign_non_fec = 0,
    misalign_fec = 0,
    decode_fec = 0,
#ifdef LINK_PB_LIBRARIES
    internal_fix_fec,
#endif
    sig_not_lock,
    fdr_link_up,
    wrong_size_cell = 0,
    link_i,
    link_per_mac_i,
    mac_group_i,
    serdes_group_i,
    serdes_lane_id,
    qrtt_id,
    lane_in_qrtt,
    reg_val,
    val = 0,
    val2,
    crc_err_cnt = 0,
    crc_overflow,
    res;
  SOC_PETRA_SRD_LANE_STATUS_INFO
    srd_lane_stat;
  uint8
    is_link_fabric;
  SOC_PETRA_REGS*
    regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(links_status);
  soc_petra_PETRA_FABRIC_LINKS_STATUS_ALL_clear(unit, links_status);

  for(mac_group_i=0 ; mac_group_i < SOC_DPP_DEFS_GET(unit, nof_fabric_macs); mac_group_i++)
  {
    SOC_PETRA_FLD_IGET(regs->fabric_mac.interrupt_1_reg.rx_crcerr_n_int,crc_errs,mac_group_i,120+mac_group_i,exit);
    SOC_PETRA_FLD_IGET(regs->fabric_mac.interrupt_2_reg.los_int,sig_not_lock,mac_group_i,140+mac_group_i,exit);
    SOC_PETRA_FLD_IGET(regs->fabric_mac.interrupt_2_reg.rx_mis_aerr_n_int,misalign_non_fec,mac_group_i,150+mac_group_i,exit);
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      SOC_PB_FLD_IGET(regs->fabric_mac.forward_error_correction_enabler_reg.use_fec, fec_en,mac_group_i,110+mac_group_i,exit);
      SOC_PB_FLD_IGET(regs->fabric_mac.interrupt_4_reg.dec_err_int,decode_fec,mac_group_i,160+mac_group_i,exit);
      SOC_PB_FLD_IGET(regs->fabric_mac.interrupt_4_reg.oof_int,misalign_fec,mac_group_i,170+mac_group_i,exit);
      SOC_PB_FLD_IGET(regs->fabric_mac.interrupt_6_reg.transmit_err_int,internal_fix_fec,mac_group_i,180+mac_group_i,exit);
    }
#endif

    /*
     *	CAUTION: the following register has in practice a different
     *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
     */
    SOC_PETRA_FLD_IGET(regs->fabric_mac.interrupt_5_reg.wrong_size_n_int,wrong_size_cell,mac_group_i,130+mac_group_i,exit);

    for(link_per_mac_i=0 ; link_per_mac_i < SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac); link_per_mac_i++)
    {
      link_i = mac_group_i * SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac) + link_per_mac_i;
      serdes_lane_id = soc_petra_fbr2srd_lane_id(link_i);
      qrtt_id = SOC_PETRA_SRD_QRTT2INNER_ID(SOC_PETRA_SRD_LANE2QRTT_GLBL(serdes_lane_id));
      lane_in_qrtt = SOC_PETRA_SRD_LANE2INNER_ID(serdes_lane_id);
      serdes_group_i = SOC_PETRA_SRD_LANE2INSTANCE(serdes_lane_id);

      is_link_fabric = soc_petra_fabric_is_fabric_link(unit, link_i);
      if (is_link_fabric == FALSE)
      {
        soc_petra_PETRA_FABRIC_LINKS_STATUS_SINGLE_clear(&(links_status->single_link_status[link_i]));
        continue;
      }
      else
      {
        links_status->single_link_status[link_i].valid = TRUE;
      }

      SOC_PETRA_FLD_IGET(regs->fabric_mac.receive_reset_reg.fmacrx_rst_n[link_per_mac_i], is_reset,mac_group_i,100+mac_group_i,exit);
      links_status->single_link_status[link_i].up_rx = SOC_SAND_NUM2BOOL_INVERSE(is_reset);

      SOC_PETRA_FLD_IGET(regs->fabric_mac.receive_reset_reg.fmactx_rst_n[link_per_mac_i], is_reset,mac_group_i,100+mac_group_i,exit);
      links_status->single_link_status[link_i].up_tx = SOC_SAND_NUM2BOOL_INVERSE(is_reset);

      SOC_PETRA_FLD_IGET(regs->fabric_mac.leaky_bucket_reg[link_per_mac_i].lky_bkt_value, val , mac_group_i ,200+link_i,exit);
      links_status->single_link_status[link_i].leaky_bucket_counter = val;


      if(crc_errs & SOC_SAND_BIT(link_per_mac_i))
      {
        links_status->single_link_status[link_i].errors_bm |= SOC_PETRA_LINK_ERR_TYPE_CRC;
        links_status->error_in_some = TRUE;
      }
      if(wrong_size_cell & SOC_SAND_BIT(link_per_mac_i))
      {
        links_status->single_link_status[link_i].errors_bm |= SOC_PETRA_LINK_ERR_TYPE_SIZE;
        links_status->error_in_some = TRUE;
      }

      SOC_PETRA_REG_IGET(regs->serdes.srd_ln_stat_reg[qrtt_id][lane_in_qrtt], reg_val, serdes_group_i, 110, exit);

      res = soc_petra_srd_lane_status_get_unsafe(
              unit,
              serdes_lane_id,
              &srd_lane_stat
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

      if((srd_lane_stat.rx_signal_detected && (!(srd_lane_stat.rx_power_off))) || (!(srd_lane_stat.tx_power_off)))
      {
        links_status->single_link_status[link_i].indications_bm |= SOC_PETRA_LINK_INDICATE_TYPE_SIG_LOCK;
      }

      if(fec_en & SOC_SAND_BIT(link_per_mac_i))
      {
        if(misalign_fec & SOC_SAND_BIT(link_per_mac_i))
        {
          links_status->single_link_status[link_i].errors_bm |= SOC_PETRA_LINK_ERR_TYPE_MISALIGN;
          links_status->error_in_some = TRUE;
        }
        if(decode_fec & SOC_SAND_BIT(link_per_mac_i))
        {
          links_status->single_link_status[link_i].errors_bm |= SOC_PETRA_LINK_ERR_TYPE_CODE_GROUP;
          links_status->error_in_some = TRUE;
        }
      }
      else
      {
        if(misalign_non_fec & SOC_SAND_BIT(link_per_mac_i))
        {
          links_status->single_link_status[link_i].errors_bm |= SOC_PETRA_LINK_ERR_TYPE_MISALIGN;
          links_status->error_in_some = TRUE;
        }
        SOC_PETRA_FLD_FROM_REG(regs->serdes.srd_ln_stat_reg[qrtt_id][lane_in_qrtt].ln_rx_code_err, val, reg_val,120, exit);
        SOC_PETRA_FLD_FROM_REG(regs->serdes.srd_ln_stat_reg[qrtt_id][lane_in_qrtt].ln_rx_disp_err, val2, reg_val,121, exit);
        if(val || val2)
        {
          links_status->single_link_status[link_i].errors_bm |= SOC_PETRA_LINK_ERR_TYPE_CODE_GROUP;
          links_status->error_in_some = TRUE;
        }
      }

      if(link_i >= 18)
      {
        SOC_PETRA_FLD_GET(regs->fdr.link_up_status_from_fdrb_reg.fdrlink_up_b,fdr_link_up,190,exit);
        if((fdr_link_up) & SOC_SAND_BIT(link_i-18))
        {
          links_status->single_link_status[link_i].indications_bm |= SOC_PETRA_LINK_INDICATE_TYPE_ACCEPT_CELL;
        }
        if((internal_fix_fec) & SOC_SAND_BIT(link_i-18))
        {
          links_status->single_link_status[link_i].indications_bm |= SOC_PETRA_LINK_INDICATE_INTRNL_FIXED;
        }
      }
      else
      {
        SOC_PETRA_FLD_GET(regs->fdr.link_up_status_from_fdra_reg.fdrlink_up_a,fdr_link_up,191,exit);
        if((fdr_link_up) & SOC_SAND_BIT(link_i))
        {
          links_status->single_link_status[link_i].indications_bm |= SOC_PETRA_LINK_INDICATE_TYPE_ACCEPT_CELL;
        }
        if((internal_fix_fec) & SOC_SAND_BIT(link_i))
        {
          links_status->single_link_status[link_i].indications_bm |= SOC_PETRA_LINK_INDICATE_INTRNL_FIXED;
        }
      }

      if (read_crc_counter)
      {
        SOC_PETRA_FLD_IGET(regs->fabric_mac.crcerror_counter_reg[link_per_mac_i].crcerr_cnt, crc_err_cnt, mac_group_i, 82, exit);
        SOC_PETRA_FLD_IGET(regs->fabric_mac.crcerror_counter_reg[link_per_mac_i].crcerr_cnt_ovf, crc_overflow, mac_group_i, 84, exit);
        
        if (crc_overflow)
        {
          crc_err_cnt = SOC_SAND_U32_MAX;
        }
      }

      links_status->single_link_status[link_i].crc_err_counter = crc_err_cnt;
    } /* For all links */

    /*
     *  Clear the interrupt indications by setting them to 1
     */
    val = 0xFFFFFFFF;
    SOC_PETRA_REG_ISET(regs->fabric_mac.interrupt_2_reg,val,mac_group_i,350+mac_group_i,exit);
    /*
     *	CAUTION: the following register has in practice a different
     *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
     */
    val = (1<<SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac))-1;
    SOC_PETRA_FLD_ISET(regs->fabric_mac.interrupt_1_reg.rx_crcerr_n_int,val,mac_group_i,320+mac_group_i,exit);
    SOC_PETRA_FLD_ISET(regs->fabric_mac.interrupt_5_reg.wrong_size_n_int,val,mac_group_i,335+mac_group_i,exit);
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      val = 0xffffffff;
      SOC_PB_REG_ISET(regs->fabric_mac.interrupt_4_reg, val, mac_group_i, 360 + mac_group_i, exit);
      SOC_PB_REG_ISET(regs->fabric_mac.interrupt_6_reg, val, mac_group_i, 370 + mac_group_i, exit);
    }
#endif
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_links_status_get_unsafe()",0,0);
}


uint8
soc_petra_topology_status_connectivity_is_link_expected(
  SOC_SAND_IN SOC_PETRA_FAR_DEVICE_TYPE   far_dev_type
  )
{
  /*
   *	This field is meaningless for Soc_petra (always TRUE).
   *  The indication is kept only for compatibility with the FE connectivity_get API,
   *  to produce a similar output
   */
  if(
      ( far_dev_type == SOC_PETRA_FAR_DEVICE_TYPE_FE3 || far_dev_type == SOC_PETRA_FAR_DEVICE_TYPE_FE2 || \
        far_dev_type == SOC_PETRA_FAR_DEVICE_TYPE_FE1 || far_dev_type == SOC_PETRA_FAR_DEVICE_TYPE_FAP )
    )
  {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************************
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_link_on_off_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info
  )
{
  uint32
    reg_val,
    pwr_up_val,
    rst_val,
    res;
  uint32
    reg_idx,
    fld_idx,
    srd_lane_id = 0;
  uint32
    link_in_mac = 0,
    mac_of_link;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_state = SOC_PETRA_SRD_NOF_POWER_STATES;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LINK_ON_OFF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  link_in_mac = SOC_PETRA_FBC_LINK_IN_MAC(link_ndx);
  mac_of_link = SOC_PETRA_FBC_MAC_OF_LINK(link_ndx);

  reg_idx = SOC_PETRA_REG_IDX_GET(link_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(link_ndx, SOC_SAND_REG_SIZE_BITS);

  pwr_up_val = (info->on_off == SOC_PETRA_LINK_STATE_ON)?0x1:0x0;
  rst_val    = (info->on_off == SOC_PETRA_LINK_STATE_ON)?0x0:0x1;

  /* SerDes */
  if (info->serdes_also == TRUE)
  {
    srd_lane_id = soc_petra_fbr2srd_lane_id(link_ndx);

    srd_pwr_state = (info->on_off == SOC_PETRA_LINK_STATE_ON)?SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK:SOC_PETRA_SRD_POWER_STATE_DOWN;
  }

  if (info->on_off == SOC_PETRA_LINK_STATE_ON)
  {
    if (info->serdes_also == TRUE)
    {
      res = soc_petra_srd_lane_power_state_set_unsafe(
              unit,
              srd_lane_id,
              SOC_PETRA_CONNECTION_DIRECTION_BOTH,
              srd_pwr_state
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    /* MAC-TX */
    SOC_PETRA_FLD_ISET(regs->fabric_mac.receive_reset_reg.fmactx_rst_n[link_in_mac], rst_val, mac_of_link, 10, exit);

    /* RTP */
    SOC_PETRA_REG_GET(regs->rtp.allowed_links_reg[reg_idx], reg_val, 20, exit);
    SOC_SAND_SET_BIT(reg_val, pwr_up_val, fld_idx);
    SOC_PETRA_REG_SET(regs->rtp.allowed_links_reg[reg_idx], reg_val, 30, exit);

    /* MAC-RX */
    SOC_PETRA_FLD_ISET(regs->fabric_mac.receive_reset_reg.fmacrx_rst_n[link_in_mac], rst_val, mac_of_link, 40, exit);
  }
  else /* info->on_off == SOC_PETRA_LINK_STATE_OFF */
  {
    /*
     *  Same as above, in reverse order
     */

    /* MAC-RX */
    SOC_PETRA_FLD_ISET(regs->fabric_mac.receive_reset_reg.fmacrx_rst_n[link_in_mac], rst_val, mac_of_link, 50, exit);

    /* RTP */
    SOC_PETRA_REG_GET(regs->rtp.allowed_links_reg[reg_idx], reg_val, 60, exit);
    SOC_SAND_SET_BIT(reg_val, pwr_up_val, fld_idx);
    SOC_PETRA_REG_SET(regs->rtp.allowed_links_reg[reg_idx], reg_val, 70, exit);

    /* MAC-TX */
    SOC_PETRA_FLD_ISET(regs->fabric_mac.receive_reset_reg.fmactx_rst_n[link_in_mac], rst_val, mac_of_link, 80, exit);

    /* SerDes */
    if (info->serdes_also == TRUE)
    {
      res = soc_petra_srd_lane_power_state_set_unsafe(
              unit,
              srd_lane_id,
              SOC_PETRA_CONNECTION_DIRECTION_BOTH,
              srd_pwr_state
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
}

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_link_on_off_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_link_on_off_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LINK_ON_OFF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

   SOC_SAND_ERR_IF_ABOVE_MAX(
    link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links),
    SOC_PETRA_FBR_LINK_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->on_off, SOC_PETRA_LINK_NOF_STATES,
    SOC_PETRA_FBR_LINK_ON_OFF_STATE_OUT_OF_RANGE_ERR, 20, exit
  );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_link_on_off_verify()",0,0);
}

/*********************************************************************
*     Get nof fabric links
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_fabric_nof_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT int                    *nof_links
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(nof_links);

    *nof_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_nof_links_get()",0,0);
}


/*********************************************************************
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_link_on_off_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO     *info
  )
{
  uint32
    val,
    res;
  uint32
    link_in_mac,
    mac_of_link,
    srd_lane_id;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_state,
    tx_srd_pwr_state;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LINK_ON_OFF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  link_in_mac = SOC_PETRA_FBC_LINK_IN_MAC(link_ndx);
  mac_of_link = SOC_PETRA_FBC_MAC_OF_LINK(link_ndx);

  SOC_PETRA_FLD_IGET(regs->fabric_mac.receive_reset_reg.fmactx_rst_n[link_in_mac], val, mac_of_link, 80, exit);

  info->on_off = (val == 0x0)?SOC_PETRA_LINK_STATE_ON:SOC_PETRA_LINK_STATE_OFF;

  srd_lane_id = soc_petra_fbr2srd_lane_id(link_ndx);

  res = soc_petra_srd_lane_power_state_get_unsafe(
          unit,
          srd_lane_id,
          &srd_pwr_state,
          &tx_srd_pwr_state
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (
      ((info->on_off == SOC_PETRA_LINK_STATE_ON) && (srd_pwr_state == SOC_PETRA_SRD_POWER_STATE_UP)) ||
      ((info->on_off == SOC_PETRA_LINK_STATE_OFF) && (srd_pwr_state == SOC_PETRA_SRD_POWER_STATE_DOWN))
     )
  {
    info->serdes_also = TRUE;
  }
  else
  {
    info->serdes_also = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_link_on_off_get_unsafe()",0,0);
}

/*
 *  Returns TRUE if and only if the Fabric link
 *  with index 'link_ndx' is towards the fabric.
 *  This depends on the index, and the COMBO
 *  quartet configuration (whether dedicated to the Fabric or the NIF)
 */
uint8
  soc_petra_fabric_is_fabric_link(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 link_ndx
  )
{
  uint32
    srd_lane_id;
  uint8
    is_fbr;

  srd_lane_id = soc_petra_fbr2srd_lane_id(link_ndx);

  is_fbr = soc_petra_srd_is_fabric_lane(unit, srd_lane_id);

  return is_fbr;
}


/*********************************************************************
*     Retrieve the connectivity map from the device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int             link_index_min,
    SOC_SAND_IN  int             link_index_max,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  )
{
  uint32
    fld_val = 0,
    reg_val,
    fbr_link_id,
    srd_lane_id,
    rtp_links_state[SOC_PETRA_NOF_BIT_PER_LINK_REGS],
    res;
  SOC_PETRA_SRD_LANE_STATUS_INFO
    srd_lane_stat;
  uint8
    is_srd_lane_stat_ok,
    is_link_fabric;
  SOC_PETRA_REGS*
    regs = soc_petra_regs();
  SOC_PETRA_FAR_DEVICE_TYPE
    far_dev_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TOPOLOGY_STATUS_CONNECTIVITY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(connectivity_map);
  SOC_SAND_ERR_IF_ABOVE_MAX(
    link_index_max, SOC_DPP_DEFS_GET(unit, nof_fabric_links) - 1,
    SOC_PETRA_FBR_LINK_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_petra_PETRA_SRD_LANE_STATUS_INFO_clear(&srd_lane_stat);

  SOC_PETRA_REG_GET(regs->rtp.link_active_mask_reg[0], reg_val,20,exit);
  rtp_links_state[0] = reg_val;
  SOC_PETRA_REG_GET(regs->rtp.link_active_mask_reg[1], reg_val,21,exit);
  rtp_links_state[1] = reg_val;


  for(fbr_link_id=link_index_min;fbr_link_id<=link_index_max ;fbr_link_id++)
  {
    srd_lane_id = soc_petra_fbr2srd_lane_id(fbr_link_id);

    is_link_fabric = soc_petra_fabric_is_fabric_link(unit, fbr_link_id);

    if (is_link_fabric == FALSE)
    {
      soc_petra_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(&(connectivity_map->link_info[fbr_link_id]));
    }
    else
    {
      SOC_PETRA_FLD_GET(regs->fcr.connectivity_map_registers_reg[fbr_link_id].source_unit, fld_val,100+fbr_link_id,exit);
      connectivity_map->link_info[fbr_link_id].far_unit = fld_val;
      SOC_PETRA_FLD_GET(regs->fcr.connectivity_map_registers_reg[fbr_link_id].source_device_level, fld_val,200+fbr_link_id,exit);
      far_dev_type =  soc_petra_device_type_get(fld_val);
      connectivity_map->link_info[fbr_link_id].far_dev_type = far_dev_type;
      SOC_PETRA_FLD_GET(regs->fcr.connectivity_map_registers_reg[fbr_link_id].source_device_link, fld_val,300+fbr_link_id,exit);
      connectivity_map->link_info[fbr_link_id].far_link_id = fld_val;

      if(SOC_SAND_BIT(fbr_link_id%32) & rtp_links_state[fbr_link_id/32])
      {
        connectivity_map->link_info[fbr_link_id].is_logically_connected = FALSE;
      }
      else
      {
        connectivity_map->link_info[fbr_link_id].is_logically_connected = TRUE;
      }

      res = soc_petra_srd_lane_status_get_unsafe(
              unit,
              srd_lane_id,
              &srd_lane_stat
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      is_srd_lane_stat_ok = soc_petra_srd_lane_status_is_ok(&srd_lane_stat);

      connectivity_map->link_info[fbr_link_id].is_phy_connect = is_srd_lane_stat_ok;

      /*
       *  A link can not be logically connected without being physically connected
       */
      if (connectivity_map->link_info[fbr_link_id].is_phy_connect == FALSE)
      {
        connectivity_map->link_info[fbr_link_id].is_logically_connected = FALSE;
      }

      connectivity_map->link_info[fbr_link_id].is_connected_expected =
        soc_petra_topology_status_connectivity_is_link_expected(
          far_dev_type
        );
    } /* Fabric Lane */
  } /* Loop on all Fabric (or Combo) lanes */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_topology_status_connectivity_get_unsafe()",0,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
