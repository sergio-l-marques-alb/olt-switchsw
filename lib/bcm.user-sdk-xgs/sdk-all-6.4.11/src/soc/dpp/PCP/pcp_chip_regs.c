/* $Id: pcp_chip_regs.c,v 1.8 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_api_framework.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define PCP_DB_REG_SET(reg, base_value, step_value)     \
         reg.addr.base = base_value * sizeof(uint32); \
         reg.addr.step = step_value * sizeof(uint32)

#define PCP_DB_REG_FLD_SET(fld_value, base_value, step_value, msb_value, lsb_value)                                       \
          pcp_reg_fld_set(fld_value, base_value * sizeof(uint32), step_value * sizeof(uint32), msb_value, lsb_value)


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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_chip_regs[] =
{
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_REGS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_REGS_INIT),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_chip_regs[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_REGS_NOT_INITIALIZED_ERR,
    "PCP_REGS_NOT_INITIALIZED_ERR",
    "pcp regs haven't been initialized.\n\r "
    "Check Management init.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /* Added section */
  {
    PCP_LOW_LEVEL_ACCESS_ERR,
    "PCP_LOW_LEVEL_ACCESS_ERR",
    "the CPU can not perform read/write operations \n\r"
    "on the Pcp device.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static PCP_REGS  Pcp_regs;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* pcp_reg_fld_set
*TYPE:
*  PROC
*FUNCTION:
*  Sets a pcp register field
*INPUT:
*  SOC_SAND_DIRECT:
*    PCP_DB_REG_FIELD *field - pointer to pcp register field structure
*    uint32    base - base address of the register
*    uint16    step - if the block may have multiple instances
*                       (appear at multiple addresses),
*                       this is the offset between
*                       two such subsequent instances.
*    uint8  msb -  field most significant bit
*    uint8  lsb -  field least significant bit
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    field.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  pcp_reg_fld_set(
    PCP_REG_FIELD *field,
    uint32         base,
    uint16         step,
    uint8       msb,
    uint8       lsb
  )
{
   field->base = base;
   field->step = step;
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}

/* Block registers initialization: ECI 	 */
STATIC void
  pcp_regs_init_ECI(void)
{

  Pcp_regs.eci.nof_instances = PCP_BLK_NOF_INSTANCES_ECI;
  Pcp_regs.eci.addr.base = 0x20000;
  Pcp_regs.eci.addr.step = 0x0000;

  /* Interrupt Register */
  PCP_DB_REG_SET(Pcp_regs.eci.interrupt_reg, 0x20000, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_reg.csiinterrupt), 0x20000, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_reg.elkinterrupt), 0x20000, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_reg.oampinterrupt), 0x20000, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_reg.stsinterrupt), 0x20000, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_reg.eciinterrupt), 0x20000, 0x0000, 4, 4);

  /* Interrupt Mask Register */
  PCP_DB_REG_SET(Pcp_regs.eci.interrupt_mask_reg, 0x20010, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_mask_reg.csiinterrupt_mask), 0x20010, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_mask_reg.elkinterrupt_mask), 0x20010, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_mask_reg.oampinterrupt_mask), 0x20010, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_mask_reg.stsinterrupt_mask), 0x20010, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.interrupt_mask_reg.eciinterrupt_mask), 0x20010, 0x0000, 4, 4);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_wr_data_reg_0, 0x20020, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_wr_data_reg_0.indirect_command_wr_data), 0x20020, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_wr_data_reg_1, 0x20021, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_wr_data_reg_1.indirect_command_wr_data), 0x20021, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_wr_data_reg_2, 0x20022, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_wr_data_reg_2.indirect_command_wr_data), 0x20022, 0x0000, 7, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_rd_data_reg_0, 0x20030, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_rd_data_reg_0.indirect_command_rd_data), 0x20030, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_rd_data_reg_1, 0x20031, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_rd_data_reg_1.indirect_command_rd_data), 0x20031, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_rd_data_reg_2, 0x20032, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_rd_data_reg_2.indirect_command_rd_data), 0x20032, 0x0000, 7, 0);

  /* Indirect Command */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_reg, 0x20040, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_reg.indirect_command_trigger), 0x20040, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_reg.indirect_command_trigger_on_data), 0x20040, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_reg.indirect_command_count), 0x20040, 0x0000, 15, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_reg.indirect_command_timeout), 0x20040, 0x0000, 30, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_reg.indirect_command_status), 0x20040, 0x0000, 31, 31);

  /* Indirect Command Address */
  PCP_DB_REG_SET(Pcp_regs.eci.indirect_command_address_reg, 0x20041, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_address_reg.indirect_command_addr), 0x20041, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.indirect_command_address_reg.indirect_command_type), 0x20041, 0x0000, 31, 31);

  /* Debug subver */
  PCP_DB_REG_SET(Pcp_regs.eci.debug_subver_reg, 0x2004f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.debug_subver_reg.debug_subver), 0x2004f, 0x0000, 15, 0);

  /* Rev Reg */
  PCP_DB_REG_SET(Pcp_regs.eci.rev_reg, 0x20050, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.rev_reg.chip_type), 0x20050, 0x0000, 19, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.rev_reg.dbg_ver), 0x20050, 0x0000, 27, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.rev_reg.chip_ver), 0x20050, 0x0000, 31, 28);

  /* Scratch Register */
  PCP_DB_REG_SET(Pcp_regs.eci.scratch_reg, 0x20051, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.scratch_reg.scratch_register), 0x20051, 0x0000, 31, 0);

  /* Cpu Acc Per Clk Reg */
  PCP_DB_REG_SET(Pcp_regs.eci.cpu_acc_per_clk_reg, 0x20052, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.cpu_acc_per_clk_reg.cpu_acc_per_clk), 0x20052, 0x0000, 8, 0);

  /* eif phy cfg ctrl */
  PCP_DB_REG_SET(Pcp_regs.eci.eif_phy_cfg_ctrl_reg, 0x20053, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_phy_cfg_ctrl_reg.phy_write), 0x20053, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_phy_cfg_ctrl_reg.phy_busy), 0x20053, 0x0000, 1, 1);

  /* eif phy cfg val */
  PCP_DB_REG_SET(Pcp_regs.eci.eif_phy_cfg_val_reg, 0x20054, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_phy_cfg_val_reg.rx_eqctrl), 0x20054, 0x0000, 3, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_phy_cfg_val_reg.rx_eqdcgain), 0x20054, 0x0000, 5, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_phy_cfg_val_reg.tx_preemp), 0x20054, 0x0000, 10, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_phy_cfg_val_reg.tx_vodctrl), 0x20054, 0x0000, 13, 11);

  /* Eif Mac Ctrl */
  PCP_DB_REG_SET(Pcp_regs.eci.eif_mac_ctrl_reg, 0x20055, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_mac_ctrl_reg.eif_enable_rx), 0x20055, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_mac_ctrl_reg.eif_enable_tx), 0x20055, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_mac_ctrl_reg.eif_ing_fc), 0x20055, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_mac_ctrl_reg.eif_link_ok), 0x20055, 0x0000, 3, 3);

  /* Eif Debug Reg */
  PCP_DB_REG_SET(Pcp_regs.eci.eif_debug_reg, 0x20056, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.phy_rx_dig_reset), 0x20056, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.phy_tx_dig_reset), 0x20056, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.phy_rx_anal_reset), 0x20056, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.mac_tx_reset), 0x20056, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.mac_rx_reset), 0x20056, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.phy_freq_locked), 0x20056, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.phy_pll_locked), 0x20056, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.mac_fifo_underflow), 0x20056, 0x0000, 7, 7);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eif_debug_reg.mac_fifo_overflow), 0x20056, 0x0000, 8, 8);

  /* Gen Cfg Reg */
  PCP_DB_REG_SET(Pcp_regs.eci.gen_cfg_reg, 0x20057, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.gen_cfg_reg.rldram_576mb), 0x20057, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.gen_cfg_reg.qdr_36mb), 0x20057, 0x0000, 1, 1);

  /* Mem Init Reg */
  PCP_DB_REG_SET(Pcp_regs.eci.mem_init_reg, 0x20058, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.mem_init_reg.rld_init_done), 0x20058, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.mem_init_reg.qdc_init_done), 0x20058, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.mem_init_reg.mem_init_done), 0x20058, 0x0000, 2, 2);

  /* General Interrupt Mask Register */
  PCP_DB_REG_SET(Pcp_regs.eci.general_interrupt_mask_reg, 0x20059, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.general_interrupt_mask_reg.fpga_interrupt_mask), 0x20059, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.general_interrupt_mask_reg.soc_petra_interrupt_mask), 0x20059, 0x0000, 1, 1);

  /* ECI Error Reg */
  PCP_DB_REG_SET(Pcp_regs.eci.eci_error_reg, 0x2005a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.pcie_error), 0x2005a, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.qdr_underflow), 0x2005a, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.qdr_overflow), 0x2005a, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.rld1_underflow), 0x2005a, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.rld1_overflow), 0x2005a, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.rld2_underflow), 0x2005a, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.eci_error_reg.rld2_overflow), 0x2005a, 0x0000, 6, 6);

  /* debug fifo */
  PCP_DB_REG_SET(Pcp_regs.eci.debug_fifo_reg, 0x2005b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.eci.debug_fifo_reg.debug_fifo), 0x2005b, 0x0000, 31, 0);
}

/* Block registers initialization: CSI 	 */
STATIC void
  pcp_regs_init_CSI(void)
{

  Pcp_regs.csi.nof_instances = PCP_BLK_NOF_INSTANCES_CSI;
  Pcp_regs.csi.addr.base = 0x24000;
  Pcp_regs.csi.addr.step = 0x0000;

  /* Interrupt Register */
  PCP_DB_REG_SET(Pcp_regs.csi.interrupt_reg, 0x24000, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.cgl_fifo_full), 0x24000, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.cgl_cmd_err), 0x24000, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.smx_tpr_low_err), 0x24000, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.smx_rpr_thresh_err), 0x24000, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.tpm_fifo_ovf), 0x24000, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.rpm_fifo_ovf), 0x24000, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.rpm_pkt_err), 0x24000, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.ingress_eob), 0x24000, 0x0000, 12, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.ingress_eop), 0x24000, 0x0000, 13, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.dma_error), 0x24000, 0x0000, 14, 14);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class1_eob), 0x24000, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class2_eob), 0x24000, 0x0000, 17, 17);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class3_eob), 0x24000, 0x0000, 18, 18);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class4_eob), 0x24000, 0x0000, 19, 19);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class5_eob), 0x24000, 0x0000, 20, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class6_eob), 0x24000, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class7_eob), 0x24000, 0x0000, 22, 22);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class1_eop), 0x24000, 0x0000, 24, 24);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class2_eop), 0x24000, 0x0000, 25, 25);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class3_eop), 0x24000, 0x0000, 26, 26);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class4_eop), 0x24000, 0x0000, 27, 27);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class5_eop), 0x24000, 0x0000, 28, 28);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class6_eop), 0x24000, 0x0000, 29, 29);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_reg.class7_eop), 0x24000, 0x0000, 30, 30);

  /* Interrupt Mask Register */
  PCP_DB_REG_SET(Pcp_regs.csi.interrupt_mask_reg, 0x24010, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.cglfifo_full_mask), 0x24010, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.cgl_cmd_err_mask), 0x24010, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.smxtpr_low_err_mask), 0x24010, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.smxrpr_thresh_err_mask), 0x24010, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.tpm_fifo_ovf_mask), 0x24010, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.rpm_fifo_ovf_mask), 0x24010, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.rpm_pkt_err_mask), 0x24010, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.ingress_eobmask), 0x24010, 0x0000, 12, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.ingress_eopmask), 0x24010, 0x0000, 13, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.dma_error_mask), 0x24010, 0x0000, 14, 14);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class1_eobmask), 0x24010, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class2_eobmask), 0x24010, 0x0000, 17, 17);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class3_eobmask), 0x24010, 0x0000, 18, 18);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class4_eobmask), 0x24010, 0x0000, 19, 19);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class5_eobmask), 0x24010, 0x0000, 20, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class6_eobmask), 0x24010, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class7_eobmask), 0x24010, 0x0000, 22, 22);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class1_eopmask), 0x24010, 0x0000, 24, 24);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class2_eopmask), 0x24010, 0x0000, 25, 25);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class3_eopmask), 0x24010, 0x0000, 26, 26);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class4_eopmask), 0x24010, 0x0000, 27, 27);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class5_eopmask), 0x24010, 0x0000, 28, 28);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class6_eopmask), 0x24010, 0x0000, 29, 29);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.interrupt_mask_reg.class7_eopmask), 0x24010, 0x0000, 30, 30);

  /* Csi Cmd */
  PCP_DB_REG_SET(Pcp_regs.csi.csi_cmd_reg, 0x24080, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.csi_cmd_reg.csi_rst), 0x24080, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.csi_cmd_reg.csi_init), 0x24080, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.csi_cmd_reg.csi_oe), 0x24080, 0x0000, 2, 2);

  /* PUC */
  PCP_DB_REG_SET(Pcp_regs.csi.puc_reg, 0x24090, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.puc_reg.pucdata), 0x24090, 0x0000, 19, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.puc_reg.pucenable), 0x24090, 0x0000, 20, 20);

  /* Cgl Cmd Err */
  PCP_DB_REG_SET(Pcp_regs.csi.cgl_cmd_err_reg, 0x24095, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.invalidate_completion), 0x24095, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.read_signature_error), 0x24095, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.read_unsuccess_error), 0x24095, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.read_parity_error), 0x24095, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.write_signature_error), 0x24095, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.write_unsuccess_error), 0x24095, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.write_parity_error), 0x24095, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.read_reply_invalid), 0x24095, 0x0000, 7, 7);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.write_reply_invalid), 0x24095, 0x0000, 8, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.cmd_formation_error), 0x24095, 0x0000, 9, 9);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.missing_start_of_cmd), 0x24095, 0x0000, 10, 10);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.read_reply_timeout), 0x24095, 0x0000, 11, 11);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_cmd_err_reg.write_reply_timeout), 0x24095, 0x0000, 12, 12);

  /* CGL Status */
  PCP_DB_REG_SET(Pcp_regs.csi.cgl_status_reg, 0x24100, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_status_reg.cglegress_fifo_used_dw), 0x24100, 0x0000, 9, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_status_reg.cglegress_fifo_empty), 0x24100, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_status_reg.cglegress_fifo_full), 0x24100, 0x0000, 20, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.cgl_status_reg.cgl_af), 0x24100, 0x0000, 21, 21);

  /* Tpm Pkt CFG */
  PCP_DB_REG_SET(Pcp_regs.csi.tpm_pkt_cfg_reg, 0x24101, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.tpm_pkt_cfg_reg.tpm_pkt_format), 0x24101, 0x0000, 0, 0);

  /* Tpm Fifo Err */
  PCP_DB_REG_SET(Pcp_regs.csi.tpm_fifo_err_reg, 0x24102, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.tpm_fifo_err_reg.tpm_dma_fifo_full), 0x24102, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.tpm_fifo_err_reg.tpm_elk_fifo_full), 0x24102, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.tpm_fifo_err_reg.tpm_oam_fifo_full), 0x24102, 0x0000, 2, 2);

  /* Rpm Stat Cfg */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_stat_cfg_reg, 0x24103, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_stat_cfg_reg.rpm_stat_cal_len), 0x24103, 0x0000, 3, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_stat_cfg_reg.rpm_stat_cal_m), 0x24103, 0x0000, 7, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_stat_cfg_reg.rpm_stat_en), 0x24103, 0x0000, 8, 8);

  /* Rpm Pkt Pause Timeout */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_pkt_pause_timeout_reg, 0x24104, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_pause_timeout_reg.rpm_pkt_pause_timeout), 0x24104, 0x0000, 31, 0);

  /* Rpm Pkt Err */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_pkt_err_reg, 0x24105, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_err_reg.rpm_err_ignore_data), 0x24105, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_err_reg.rpm_err_force_eop), 0x24105, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_err_reg.rpm_err_missing_sop), 0x24105, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_err_reg.rpm_err_unexpected_sop), 0x24105, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_err_reg.rpm_err_parity_during_packet), 0x24105, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_pkt_err_reg.rpm_err_eop_timeout), 0x24105, 0x0000, 5, 5);

  /* Rpm Map Class1 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class1_reg, 0x24108, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class1_reg.rpm_map_class1), 0x24108, 0x0000, 5, 0);

  /* Rpm Map Class2 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class2_reg, 0x24109, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class2_reg.rpm_map_class2), 0x24109, 0x0000, 5, 0);

  /* Rpm Map Class3 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class3_reg, 0x2410a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class3_reg.rpm_map_class3), 0x2410a, 0x0000, 5, 0);

  /* Rpm Map Class4 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class4_reg, 0x2410b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class4_reg.rpm_map_class4), 0x2410b, 0x0000, 5, 0);

  /* Rpm Map Class5 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class5_reg, 0x2410c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class5_reg.rpm_map_class5), 0x2410c, 0x0000, 5, 0);

  /* Rpm Map Class6 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class6_reg, 0x2410d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class6_reg.rpm_map_class6), 0x2410d, 0x0000, 5, 0);

  /* Rpm Map Class7 */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_class7_reg, 0x2410e, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_class7_reg.rpm_map_class7), 0x2410e, 0x0000, 5, 0);

  /* Rpm Map Elk */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_elk_reg, 0x2410f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_elk_reg.rpm_map_elk), 0x2410f, 0x0000, 5, 0);

  /* Rpm Map Oam */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_map_oam_reg, 0x24110, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_map_oam_reg.rpm_map_oam), 0x24110, 0x0000, 5, 0);

  /* Rpm Class1 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class1_stat_slot_reg, 0x24111, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class1_stat_slot_reg.rpm_class1_stat_slot), 0x24111, 0x0000, 3, 0);

  /* Rpm Class2 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class2_stat_slot_reg, 0x24112, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class2_stat_slot_reg.rpm_class2_stat_slot), 0x24112, 0x0000, 3, 0);

  /* Rpm Class3 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class3_stat_slot_reg, 0x24113, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class3_stat_slot_reg.rpm_class3_stat_slot), 0x24113, 0x0000, 3, 0);

  /* Rpm Class4 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class4_stat_slot_reg, 0x24114, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class4_stat_slot_reg.rpm_class4_stat_slot), 0x24114, 0x0000, 3, 0);

  /* Rpm Class5 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class5_stat_slot_reg, 0x24115, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class5_stat_slot_reg.rpm_class5_stat_slot), 0x24115, 0x0000, 3, 0);

  /* Rpm Class6 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class6_stat_slot_reg, 0x24116, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class6_stat_slot_reg.rpm_class6_stat_slot), 0x24116, 0x0000, 3, 0);

  /* Rpm Class7 Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_class7_stat_slot_reg, 0x24117, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_class7_stat_slot_reg.rpm_class7_stat_slot), 0x24117, 0x0000, 3, 0);

  /* Rpm Elk Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_elk_stat_slot_reg, 0x24118, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_elk_stat_slot_reg.rpm_elk_stat_slot), 0x24118, 0x0000, 3, 0);

  /* Rpm Oam Stat Slot */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_oam_stat_slot_reg, 0x24119, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_oam_stat_slot_reg.rpm_oam_stat_slot), 0x24119, 0x0000, 3, 0);

  /* Rpm Fifo Err */
  PCP_DB_REG_SET(Pcp_regs.csi.rpm_fifo_err_reg, 0x2411a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full1), 0x2411a, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full2), 0x2411a, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full3), 0x2411a, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full4), 0x2411a, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full5), 0x2411a, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full6), 0x2411a, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_full7), 0x2411a, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full1), 0x2411a, 0x0000, 8, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full2), 0x2411a, 0x0000, 9, 9);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full3), 0x2411a, 0x0000, 10, 10);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full4), 0x2411a, 0x0000, 11, 11);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full5), 0x2411a, 0x0000, 12, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full6), 0x2411a, 0x0000, 13, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_size_fifo_full7), 0x2411a, 0x0000, 14, 14);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_elk_fifo_full), 0x2411a, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_elk_fifo_hazard), 0x2411a, 0x0000, 17, 17);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_oam_fifo_full), 0x2411a, 0x0000, 20, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_oam_fifo_hazard), 0x2411a, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard1), 0x2411a, 0x0000, 24, 24);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard2), 0x2411a, 0x0000, 25, 25);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard3), 0x2411a, 0x0000, 26, 26);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard4), 0x2411a, 0x0000, 27, 27);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard5), 0x2411a, 0x0000, 28, 28);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard6), 0x2411a, 0x0000, 29, 29);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.rpm_fifo_err_reg.rpm_dma_fifo_hazard7), 0x2411a, 0x0000, 30, 30);

  /* Dma Err */
  PCP_DB_REG_SET(Pcp_regs.csi.dma_err_reg, 0x2411b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.dma_err_reg.idm_desc_reply_err), 0x2411b, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.dma_err_reg.edm_desc_reply_err), 0x2411b, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.dma_err_reg.iss_abort_error), 0x2411b, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.dma_err_reg.ess_error), 0x2411b, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.dma_err_reg.multi_cmplt_byte_count_err), 0x2411b, 0x0000, 4, 4);

  /* Idm Rst */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_rst_reg, 0x2411c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_rst_reg.idm_rst), 0x2411c, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_rst_reg.idm_enable), 0x2411c, 0x0000, 1, 1);

  /* Idm Trig */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_trig_reg, 0x2411d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_trig_reg.idm_trig_petra), 0x2411d, 0x0000, 0, 0);

  /* Idm Poll Timeout */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_poll_timeout_reg, 0x24120, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_poll_timeout_reg.idm_poll_timeout), 0x24120, 0x0000, 31, 0);

  /* Idm Desc Head */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_desc_head_reg_0, 0x24121, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_desc_head_reg_0.idm_desc_head), 0x24121, 0x0000, 31, 0);

  /* Idm Desc Head */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_desc_head_reg_1, 0x24122, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_desc_head_reg_1.idm_desc_head), 0x24122, 0x0000, 31, 0);

  /* Idm Desc Curr */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_desc_curr_reg_0, 0x24123, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_desc_curr_reg_0.idm_desc_curr), 0x24123, 0x0000, 31, 0);

  /* Idm Desc Curr */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_desc_curr_reg_1, 0x24124, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_desc_curr_reg_1.idm_desc_curr), 0x24124, 0x0000, 31, 0);

  /* Idm Last Desc Header */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_last_desc_header_reg, 0x24125, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_last_desc_header_reg.idm_last_desc_header), 0x24125, 0x0000, 31, 0);

  /* Idm Desc Counter */
  PCP_DB_REG_SET(Pcp_regs.csi.idm_desc_counter_reg, 0x24126, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_desc_counter_reg.idm_desc_counter), 0x24126, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.idm_desc_counter_reg.idm_desc_counter_ovf), 0x24126, 0x0000, 31, 31);

  /* Iss Rst */
  PCP_DB_REG_SET(Pcp_regs.csi.iss_rst_reg, 0x24127, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.iss_rst_reg.iss_rst), 0x24127, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.iss_rst_reg.iss_enable), 0x24127, 0x0000, 1, 1);

  /* Iss Last Burst Head */
  PCP_DB_REG_SET(Pcp_regs.csi.iss_last_burst_head_reg, 0x2412a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.iss_last_burst_head_reg.iss_last_burst_head), 0x2412a, 0x0000, 31, 0);

  /* Iss Burst Counter */
  PCP_DB_REG_SET(Pcp_regs.csi.iss_burst_counter_reg, 0x2412b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.iss_burst_counter_reg.iss_burst_counter), 0x2412b, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.iss_burst_counter_reg.iss_burst_counter_ovf), 0x2412b, 0x0000, 31, 31);

  /* Edm Rst */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_rst_reg, 0x2412c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_rst_reg.edm_rst), 0x2412c, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_rst_reg.edm_enable), 0x2412c, 0x0000, 1, 1);

  /* Edm Desc Head Class1 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class1_msb_reg, 0x24130, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class1_msb_reg.edm_desc_head_class1_msb), 0x24130, 0x0000, 31, 0);

  /* Edm Desc Head Class1 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class1_lsb_reg, 0x24131, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class1_lsb_reg.edm_desc_head_class1_lsb), 0x24131, 0x0000, 31, 0);

  /* Edm Desc Head Class2 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class2_msb_reg, 0x24132, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class2_msb_reg.edm_desc_head_class2_msb), 0x24132, 0x0000, 31, 0);

  /* Edm Desc Head Class2 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class2_lsb_reg, 0x24133, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class2_lsb_reg.edm_desc_head_class2_lsb), 0x24133, 0x0000, 31, 0);

  /* Edm Desc Head Class3 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class3_msb_reg, 0x24134, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class3_msb_reg.edm_desc_head_class3_msb), 0x24134, 0x0000, 31, 0);

  /* Edm Desc Head Class3 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class3_lsb_reg, 0x24135, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class3_lsb_reg.edm_desc_head_class3_lsb), 0x24135, 0x0000, 31, 0);

  /* Edm Desc Head Class4 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class4_msb_reg, 0x24136, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class4_msb_reg.edm_desc_head_class4_msb), 0x24136, 0x0000, 31, 0);

  /* Edm Desc Head Class4 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class4_lsb_reg, 0x24137, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class4_lsb_reg.edm_desc_head_class4_lsb), 0x24137, 0x0000, 31, 0);

  /* Edm Desc Head Class5 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class5_msb_reg, 0x24138, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class5_msb_reg.edm_desc_head_class5_msb), 0x24138, 0x0000, 31, 0);

  /* Edm Desc Head Class5 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class5_lsb_reg, 0x24139, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class5_lsb_reg.edm_desc_head_class5_lsb), 0x24139, 0x0000, 31, 0);

  /* Edm Desc Head Class6 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class6_msb_reg, 0x2413a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class6_msb_reg.edm_desc_head_class6_msb), 0x2413a, 0x0000, 31, 0);

  /* Edm Desc Head Class6 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class6_lsb_reg, 0x2413b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class6_lsb_reg.edm_desc_head_class6_lsb), 0x2413b, 0x0000, 31, 0);

  /* Edm Desc Head Class7 Msb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class7_msb_reg, 0x2413c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class7_msb_reg.edm_desc_head_class7_msb), 0x2413c, 0x0000, 31, 0);

  /* Edm Desc Head Class7 Lsb */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_head_class7_lsb_reg, 0x2413d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_head_class7_lsb_reg.edm_desc_head_class7_lsb), 0x2413d, 0x0000, 31, 0);

  /* Edm Last Desc Header */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_last_desc_header_reg, 0x2414c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_last_desc_header_reg.edm_last_desc_header), 0x2414c, 0x0000, 31, 0);

  /* Edm Desc Counter */
  PCP_DB_REG_SET(Pcp_regs.csi.edm_desc_counter_reg, 0x2414d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_counter_reg.edm_desc_counter), 0x2414d, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.edm_desc_counter_reg.edm_desc_counter_ovf), 0x2414d, 0x0000, 31, 31);

  /* Ess Rst */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_rst_reg, 0x2414e, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_rst_reg.ess_rst), 0x2414e, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_rst_reg.ess_enable), 0x2414e, 0x0000, 1, 1);

  /* Ess Buff Size */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_buff_size_reg, 0x2414f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_buff_size_reg.ess_buffer_size), 0x2414f, 0x0000, 29, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_buff_size_reg.ess_use_buffer_size), 0x2414f, 0x0000, 31, 31);

  /* Ess Weight Class1 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class1_reg, 0x24150, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class1_reg.ess_weight_class1), 0x24150, 0x0000, 7, 0);

  /* Ess Weight Class2 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class2_reg, 0x24151, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class2_reg.ess_weight_class2), 0x24151, 0x0000, 7, 0);

  /* Ess Weight Class3 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class3_reg, 0x24152, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class3_reg.ess_weight_class3), 0x24152, 0x0000, 7, 0);

  /* Ess Weight Class4 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class4_reg, 0x24153, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class4_reg.ess_weight_class4), 0x24153, 0x0000, 7, 0);

  /* Ess Weight Class5 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class5_reg, 0x24154, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class5_reg.ess_weight_class5), 0x24154, 0x0000, 7, 0);

  /* Ess Weight Class6 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class6_reg, 0x24155, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class6_reg.ess_weight_class6), 0x24155, 0x0000, 7, 0);

  /* Ess Weight Class7 */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_weight_class7_reg, 0x24156, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_weight_class7_reg.ess_weight_class7), 0x24156, 0x0000, 7, 0);

  /* Ess Pkt Pending */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_pkt_pending_reg, 0x24157, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class1_pkt_pending), 0x24157, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class2_pkt_pending), 0x24157, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class3_pkt_pending), 0x24157, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class4_pkt_pending), 0x24157, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class5_pkt_pending), 0x24157, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class6_pkt_pending), 0x24157, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_class7_pkt_pending), 0x24157, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_pkt_pending_reg.ess_petra_oamppkt_pending), 0x24157, 0x0000, 7, 7);

  /* Ess Class Sleep Timer */
  PCP_DB_REG_SET(Pcp_regs.csi.ess_class_sleep_timer_reg, 0x2415a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.csi.ess_class_sleep_timer_reg.ess_class_sleep_period), 0x2415a, 0x0000, 31, 0);
}

/* Block registers initialization: ELK 	 */
STATIC void
  pcp_regs_init_ELK(void)
{
  uint8
    fld_ndx,
    reg_ndx;

  Pcp_regs.elk.nof_instances = PCP_BLK_NOF_INSTANCES_ELK;
  Pcp_regs.elk.addr.base = 0x28000;
  Pcp_regs.elk.addr.step = 0x0000;

  /* Interrupt Register */
  PCP_DB_REG_SET(Pcp_regs.elk.interrupt_reg, 0x28000, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.error_delete_unknown_key), 0x28000, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.error_reached_entry_limit), 0x28000, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.error_cam_table_full), 0x28000, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.error_inserted_existing), 0x28000, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.error_learn_over_static), 0x28000, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.error_refresh_over_static), 0x28000, 0x0000, 5, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.mact_reached_fid_limit), 0x28000, 0x0000, 6, 6);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.mact_reached_fid_limit_static_allowed), 0x28000, 0x0000, 7, 7);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.dropped_lrf_command), 0x28000, 0x0000, 8, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.event_ready), 0x28000, 0x0000, 9, 9);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.event_fifo_event_drop), 0x28000, 0x0000, 10, 10);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.event_fifo_high_threshold_reached), 0x28000, 0x0000, 11, 11);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.reply_fifo_ready), 0x28000, 0x0000, 12, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.reply_fifo_reply_drop), 0x28000, 0x0000, 13, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.diagnostic_command_completed), 0x28000, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.unexp_sop_err), 0x28000, 0x0000, 20, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.unexp_eop_err), 0x28000, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.unexp_valid_bytes_err), 0x28000, 0x0000, 22, 22);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_reg.dsp_eth_type_err), 0x28000, 0x0000, 23, 23);

  /* Interrupt Mask Register */
  PCP_DB_REG_SET(Pcp_regs.elk.interrupt_mask_reg, 0x28010, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.error_delete_unknown_key_mask), 0x28010, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.error_reached_entry_limit_mask), 0x28010, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.error_inserted_existing_mask), 0x28010, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.dropped_lrf_command_mask), 0x28010, 0x0000, 8, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.event_ready_mask), 0x28010, 0x0000, 9, 9);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.event_fifo_event_drop_mask), 0x28010, 0x0000, 10, 10);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.event_fifo_high_threshold_reached_mask), 0x28010, 0x0000, 11, 11);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.reply_fifo_reply_drop_mask), 0x28010, 0x0000, 12, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.diagnostic_command_completed_mask), 0x28010, 0x0000, 15, 15);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.unexp_sop_err_mask), 0x28010, 0x0000, 19, 19);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.unexp_eop_err_mask), 0x28010, 0x0000, 20, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.unexp_valid_bytes_err_mask), 0x28010, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.interrupt_mask_reg.dsp_eth_type_err_mask), 0x28010, 0x0000, 22, 22);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_0, 0x28020, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_0.indirect_command_wr_data), 0x28020, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_1, 0x28021, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_1.indirect_command_wr_data), 0x28021, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_2, 0x28022, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_2.indirect_command_wr_data), 0x28022, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_3, 0x28023, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_3.indirect_command_wr_data), 0x28023, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_4, 0x28024, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_4.indirect_command_wr_data), 0x28024, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_5, 0x28025, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_5.indirect_command_wr_data), 0x28025, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_wr_data_reg_6, 0x28026, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_wr_data_reg_6.indirect_command_wr_data), 0x28026, 0x0000, 3, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_0, 0x28030, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_0.indirect_command_rd_data), 0x28030, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_1, 0x28031, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_1.indirect_command_rd_data), 0x28031, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_2, 0x28032, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_2.indirect_command_rd_data), 0x28032, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_3, 0x28033, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_3.indirect_command_rd_data), 0x28033, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_4, 0x28034, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_4.indirect_command_rd_data), 0x28034, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_5, 0x28035, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_5.indirect_command_rd_data), 0x28035, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_rd_data_reg_6, 0x28036, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_rd_data_reg_6.indirect_command_rd_data), 0x28036, 0x0000, 3, 0);

  /* Indirect Command */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_reg, 0x28040, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_reg.indirect_command_trigger), 0x28040, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_reg.indirect_command_trigger_on_data), 0x28040, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_reg.indirect_command_count), 0x28040, 0x0000, 15, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_reg.indirect_command_timeout), 0x28040, 0x0000, 30, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_reg.indirect_command_status), 0x28040, 0x0000, 31, 31);

  /* Indirect Command Address */
  PCP_DB_REG_SET(Pcp_regs.elk.indirect_command_address_reg, 0x28041, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_address_reg.indirect_command_addr), 0x28041, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.indirect_command_address_reg.indirect_command_type), 0x28041, 0x0000, 31, 31);

  /* Gtimer Configuration */
  PCP_DB_REG_SET(Pcp_regs.elk.gtimer_configuration_reg, 0x28042, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.gtimer_configuration_reg.gtimer_cycle), 0x28042, 0x0000, 29, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.gtimer_configuration_reg.gtimer_enable), 0x28042, 0x0000, 30, 30);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.gtimer_configuration_reg.gtimer_reset_on_trigger), 0x28042, 0x0000, 31, 31);

  /* Gtimer Trigger */
  PCP_DB_REG_SET(Pcp_regs.elk.gtimer_trigger_reg, 0x28043, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.gtimer_trigger_reg.gtimer_trigger), 0x28043, 0x0000, 0, 0);

  /* Key Table Entry Limit */
  PCP_DB_REG_SET(Pcp_regs.elk.key_table_entry_limit_reg, 0x28045, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.key_table_entry_limit_reg.key_table_entry_limit), 0x28045, 0x0000, 21, 0);

  /* Age Aging Resolution */
  PCP_DB_REG_SET(Pcp_regs.elk.age_aging_resolution_reg, 0x28046, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_aging_resolution_reg.age_aging_resolution), 0x28046, 0x0000, 0, 0);

  /* Management Unit Failure */
  PCP_DB_REG_SET(Pcp_regs.elk.management_unit_failure_reg_0, 0x28047, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.management_unit_failure_reg_0.mngmnt_unit_failure), 0x28047, 0x0000, 31, 0);

  /* Management Unit Failure */
  PCP_DB_REG_SET(Pcp_regs.elk.management_unit_failure_reg_1, 0x28048, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.management_unit_failure_reg_1.mngmnt_unit_failure), 0x28048, 0x0000, 31, 0);

  /* Management Unit Failure */
  PCP_DB_REG_SET(Pcp_regs.elk.management_unit_failure_reg_2, 0x28049, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.management_unit_failure_reg_2.mngmnt_unit_failure), 0x28049, 0x0000, 10, 0);

  /* Diagnostics */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_reg, 0x2804a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_reg.diagnostics_lookup), 0x2804a, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_reg.diagnostics_read), 0x2804a, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_reg.diagnostics_read_age), 0x2804a, 0x0000, 2, 2);

  /* Diagnostics Index */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_index_reg, 0x2804b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_index_reg.diagnostics_index), 0x2804b, 0x0000, 21, 0);

  /* Diagnostics Key */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_key_reg[0], 0x2804c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_key_reg[0].diagnostics_key), 0x2804c, 0x0000, 31, 0);

  /* Diagnostics Key */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_key_reg[1], 0x2804d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_key_reg[1].diagnostics_key), 0x2804d, 0x0000, 30, 0);

  /* Diagnostics Lookup Result */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_lookup_result_reg_0, 0x2804e, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_lookup_result_reg_0.entry_found), 0x2804e, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_lookup_result_reg_0.entry_payload), 0x2804e, 0x0000, 31, 1);

  /* Diagnostics Lookup Result */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_lookup_result_reg_1, 0x2804f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_lookup_result_reg_1.entry_payload), 0x2804f, 0x0000, 9, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_lookup_result_reg_1.entry_age_stat), 0x2804f, 0x0000, 13, 10);

  /* Diagnostics Read Result */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_read_result_reg_0, 0x28050, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_read_result_reg_0.entry_valid), 0x28050, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_read_result_reg_0.entry_key), 0x28050, 0x0000, 31, 1);

  /* Diagnostics Read Result */
  PCP_DB_REG_SET(Pcp_regs.elk.diagnostics_read_result_reg_1, 0x28051, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.diagnostics_read_result_reg_1.entry_key), 0x28051, 0x0000, 31, 0);

  /* EMC Defrag Configuration Register */
  PCP_DB_REG_SET(Pcp_regs.elk.emc_defrag_configuration_register_reg, 0x28052, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.emc_defrag_configuration_register_reg.defrag_enable), 0x28051, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.emc_defrag_configuration_register_reg.defrag_mode), 0x28051, 0x0000, 2, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.emc_defrag_configuration_register_reg.defrag_cam_threshold), 0x28051, 0x0000, 7, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.emc_defrag_configuration_register_reg.defrag_period), 0x28051, 0x0000, 31, 8);

  /* Entry Counter */
  PCP_DB_REG_SET(Pcp_regs.elk.entry_counter_reg, 0x28053, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.entry_counter_reg.entry_counter), 0x28053, 0x0000, 21, 0);

  /* Flu Machine Hit Counter */
  PCP_DB_REG_SET(Pcp_regs.elk.flu_machine_hit_counter_reg, 0x28055, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_hit_counter_reg.flu_machine_hit_counter), 0x28055, 0x0000, 16, 0);

  /* Mact Managment Request Configuration */
  PCP_DB_REG_SET(Pcp_regs.elk.mact_managment_request_configuration_reg, 0x28056, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.mact_managment_request_configuration_reg.allow_static_2_exceed), 0x28056, 0x0000, 8, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.mact_managment_request_configuration_reg.acknowledge_only_failure), 0x28056, 0x0000, 12, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.mact_managment_request_configuration_reg.check_fid_limit), 0x28056, 0x0000, 13, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.mact_managment_request_configuration_reg.acknowledge_fid_check_failure_always), 0x28056, 0x0000, 14, 14);

  /* Learn Filter Properties */
  PCP_DB_REG_SET(Pcp_regs.elk.learn_filter_properties_reg, 0x28057, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.learn_filter_properties_reg.learn_filter_drop_duplicate), 0x28057, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.learn_filter_properties_reg.learn_filter_entry_ttl), 0x28057, 0x0000, 9, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.learn_filter_properties_reg.learn_filter_res), 0x28057, 0x0000, 13, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.learn_filter_properties_reg.learn_filter_watermark), 0x28057, 0x0000, 19, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.learn_filter_properties_reg.learn_filter_drop_duplicate_counter), 0x28057, 0x0000, 31, 22);

  /* Lrn Lkp Gen Conf */
  PCP_DB_REG_SET(Pcp_regs.elk.lrn_lkp_gen_conf_reg, 0x28058, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lrn_lkp_gen_conf_reg.learn_lookup_elk_bits_63to58), 0x28058, 0x0000, 7, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lrn_lkp_gen_conf_reg.learn_lookup_elk_bits_127to122), 0x28058, 0x0000, 13, 8);

  /* Lookup Lookup Mode */
  PCP_DB_REG_SET(Pcp_regs.elk.lookup_lookup_mode_reg, 0x28059,0x0000);
  for (fld_ndx = 0; fld_ndx < PCP_ELK_LOOKUP_NOF_MODES; ++fld_ndx)
  {
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lookup_lookup_mode_reg.lookup_allowed_events_dynamic[fld_ndx]), 0x28059, 0x0000, 1 + (PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx), 0 + (PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx));
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lookup_lookup_mode_reg.lookup_allowed_events_static[fld_ndx]),  0x28059, 0x0000, 3 + (PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx), 2 + (PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx));
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lookup_lookup_mode_reg.lookup_allowed_events_learn[fld_ndx]),   0x28059, 0x0000, 4 + (PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx), 4 + (PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx));
  }

  /* Cpu Request */
  PCP_DB_REG_SET(Pcp_regs.elk.cpu_request_reg_0, 0x2805a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_0.mact_req_key_mac), 0x2805a, 0x0000, 31, 2);

  /* Cpu Request */
  PCP_DB_REG_SET(Pcp_regs.elk.cpu_request_reg_1, 0x2805b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_1.mact_req_key_mac), 0x2805b, 0x0000, 17, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_1.mact_req_key_vsi), 0x2805b, 0x0000, 31, 18);

  /* Cpu Request */
  PCP_DB_REG_SET(Pcp_regs.elk.cpu_request_reg_2, 0x2805c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_2.mact_req_key_db), 0x2805c, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_2.mact_req_part_of_lag), 0x2805c, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_2.mact_req_command), 0x2805c, 0x0000, 4, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_2.mact_req_stamp), 0x2805c, 0x0000, 12, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_2.mact_req_payload_destination), 0x2805c, 0x0000, 28, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_2.mact_req_payload_eei), 0x2805c, 0x0000, 31, 29);

  /* Cpu Request */
  PCP_DB_REG_SET(Pcp_regs.elk.cpu_request_reg_3, 0x2805d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_3.mact_req_payload_eei), 0x2805d, 0x0000, 20, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_3.mact_req_payload_is_dynamic), 0x2805d, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_3.mact_req_age_payload), 0x2805d, 0x0000, 24, 22);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_3.mact_req_success), 0x2805d, 0x0000, 25, 25);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_3.mact_req_reason), 0x2805d, 0x0000, 27, 26);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_reg_3.mact_req_self), 0x2805d, 0x0000, 30, 30);

  /* Cpu Request Trigger */
  PCP_DB_REG_SET(Pcp_regs.elk.cpu_request_trigger_reg, 0x2805e, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.cpu_request_trigger_reg.cpu_request_trigger), 0x2805e, 0x0000, 0, 0);

  /* Age Machine Configuration */
  PCP_DB_REG_SET(Pcp_regs.elk.age_machine_configuration_reg, 0x2805f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_aging_enable), 0x2805f, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_machine_pause), 0x2805f, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_machine_access_shaper), 0x2805f, 0x0000, 15, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_stamp), 0x2805f, 0x0000, 27, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_machine_delete_aged_out), 0x2805f, 0x0000, 28, 28);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_machine_notify_aged_out), 0x2805f, 0x0000, 29, 29);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_machine_delete_aged_out_static), 0x2805f, 0x0000, 30, 30);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_configuration_reg.age_machine_notify_aged_out_static), 0x2805f, 0x0000, 31, 31);

  /* Age Aging Mode */
  PCP_DB_REG_SET(Pcp_regs.elk.age_aging_mode_reg, 0x28060,0x0000);
  for (fld_ndx = 0; fld_ndx < PCP_ELK_AGE_AGING_NOF_MODES; ++fld_ndx)
  {
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_aging_mode_reg.aging_cfg_ptr[fld_ndx]),        0x28060, 0x0000, 1 + (PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx), 0 + (PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx));
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_aging_mode_reg.own_system_lag_port[fld_ndx]),  0x28060, 0x0000, 2 + (PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx), 2 + (PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx));
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_aging_mode_reg.own_system_phys_port[fld_ndx]), 0x28060, 0x0000, 3 + (PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx), 3 + (PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx));
  }

  /* Age Machine Current Index */
  PCP_DB_REG_SET(Pcp_regs.elk.age_machine_current_index_reg, 0x28061, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_current_index_reg.age_machine_current_index), 0x28061, 0x0000, 21, 0);

  /* Age Machine Status */
  PCP_DB_REG_SET(Pcp_regs.elk.age_machine_status_reg, 0x28062, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_status_reg.age_machine_active), 0x28062, 0x0000, 0, 0);

  /* Age Machine Meta Cycle */
  PCP_DB_REG_SET(Pcp_regs.elk.age_machine_meta_cycle_reg, 0x28063, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_meta_cycle_reg.age_machine_meta_cycle), 0x28063, 0x0000, 31, 0);

  /* Age Machine Start Index */
  PCP_DB_REG_SET(Pcp_regs.elk.age_machine_start_index_reg, 0x28064, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_start_index_reg.age_machine_start_index), 0x28064, 0x0000, 21, 0);

  /* Age Machine End Index */
  PCP_DB_REG_SET(Pcp_regs.elk.age_machine_end_index_reg, 0x28065, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.age_machine_end_index_reg.age_machine_end_index), 0x28065, 0x0000, 21, 0);

  /* Flu Machine Configuration */
  PCP_DB_REG_SET(Pcp_regs.elk.flu_machine_configuration_reg, 0x28067, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_configuration_reg.flu_machine_pause), 0x28067, 0x0000, 4, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_configuration_reg.flu_machine_access_shaper), 0x28067, 0x0000, 15, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_configuration_reg.flu_stamp), 0x28067, 0x0000, 27, 20);

  /* Flu Machine Current Index */
  PCP_DB_REG_SET(Pcp_regs.elk.flu_machine_current_index_reg, 0x28068, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_current_index_reg.flu_machine_current_index), 0x28068, 0x0000, 21, 0);

  /* Flu Machine Status */
  PCP_DB_REG_SET(Pcp_regs.elk.flu_machine_status_reg, 0x28069, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_status_reg.flu_machine_active), 0x28069, 0x0000, 0, 0);

  /* Flu Machine End Index */
  PCP_DB_REG_SET(Pcp_regs.elk.flu_machine_end_index_reg, 0x2806a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.flu_machine_end_index_reg.flu_machine_end_index), 0x2806a, 0x0000, 21, 0);

  /* Evf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.evf_event_reg_0, 0x2806b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.evf_event_reg_0.evf_event), 0x2806b, 0x0000, 31, 0);

  /* Evf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.evf_event_reg_1, 0x2806c, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.evf_event_reg_1.evf_event), 0x2806c, 0x0000, 31, 0);

  /* Evf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.evf_event_reg_2, 0x2806d, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.evf_event_reg_2.evf_event), 0x2806d, 0x0000, 31, 0);

  /* Evf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.evf_event_reg_3, 0x2806e, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.evf_event_reg_3.evf_event), 0x2806e, 0x0000, 31, 0);

  /* Crf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.crf_event_reg_0, 0x2806f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_0.mact_req_key_mac), 0x2806f, 0x0000, 31, 2);

  /* Crf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.crf_event_reg_1, 0x28070, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_1.mact_req_key_mac), 0x28070, 0x0000, 17, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_1.mact_req_key_vsi), 0x28070, 0x0000, 31, 18);

  /* Crf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.crf_event_reg_2, 0x28071, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_2.mact_req_key_db), 0x28071, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_2.mact_req_part_of_lag), 0x28071, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_2.mact_req_command), 0x28071, 0x0000, 4, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_2.mact_req_stamp), 0x28071, 0x0000, 12, 5);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_2.mact_req_payload_destination), 0x28071, 0x0000, 28, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_2.mact_req_payload_eei), 0x28071, 0x0000, 31, 29);

  /* Crf Event */
  PCP_DB_REG_SET(Pcp_regs.elk.crf_event_reg_3, 0x28072, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_3.mact_req_payload_eei), 0x28072, 0x0000, 20, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_3.mact_req_payload_is_dynamic), 0x28072, 0x0000, 21, 21);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_3.mact_req_age_payload), 0x28072, 0x0000, 24, 22);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_3.mact_req_success), 0x28072, 0x0000, 25, 25);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_3.mact_req_reason), 0x28072, 0x0000, 27, 26);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.crf_event_reg_3.mact_req_self), 0x28072, 0x0000, 30, 30);

  /* Event Fifo High Threshold */
  PCP_DB_REG_SET(Pcp_regs.elk.event_fifo_high_threshold_reg, 0x28073, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_high_threshold_reg.event_fifo_high_threshold), 0x28073, 0x0000, 6, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_high_threshold_reg.event_read_by_cpu), 0x28073, 0x0000, 7, 7);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_high_threshold_reg.map_fid_to_fid_type), 0x28073, 0x0000, 8, 8);

  /* Event Fifo Event Drop Counters */
  PCP_DB_REG_SET(Pcp_regs.elk.event_fifo_event_drop_counters_reg, 0x28074, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_event_drop_counters_reg.event_fifo_rpf_event_drop_counter), 0x28074, 0x0000, 7, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_event_drop_counters_reg.event_fifo_age_event_drop_counter), 0x28074, 0x0000, 15, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_event_drop_counters_reg.event_fifo_lrf_event_drop_counter), 0x28074, 0x0000, 23, 16);

  /* Event Fifo Status */
  PCP_DB_REG_SET(Pcp_regs.elk.event_fifo_status_reg, 0x28075, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_status_reg.event_fifo_watermark), 0x28075, 0x0000, 7, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.event_fifo_status_reg.event_fifo_entry_count), 0x28075, 0x0000, 15, 8);

  /* Reply Fifo Entry Count */
  PCP_DB_REG_SET(Pcp_regs.elk.reply_fifo_entry_count_reg, 0x28076, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.reply_fifo_entry_count_reg.reply_fifo_entry_count), 0x28076, 0x0000, 7, 0);

  /* Reply Fifo Watermark */
  PCP_DB_REG_SET(Pcp_regs.elk.reply_fifo_watermark_reg, 0x28077, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.reply_fifo_watermark_reg.reply_fifo_watermark), 0x28077, 0x0000, 7, 0);

  /* Mrq Fifo Configuration */
  PCP_DB_REG_SET(Pcp_regs.elk.mrq_fifo_configuration_reg, 0x28078, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.mrq_fifo_configuration_reg.request_fifo_max_level), 0x28078, 0x0000, 6, 0);

  /* Managment In Counter */
  PCP_DB_REG_SET(Pcp_regs.elk.managment_in_counter_reg, 0x28079, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.managment_in_counter_reg.mrq_counter_in), 0x28079, 0x0000, 31, 0);

  /* Olp General Configuration0 */
  PCP_DB_REG_SET(Pcp_regs.elk.olp_general_configuration0_reg, 0x2807a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.olp_general_configuration0_reg.dsp_eth_type), 0x2807a, 0x0000, 15, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.olp_general_configuration0_reg.dis_dsp_eth_type), 0x2807a, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.olp_general_configuration0_reg.bytes_to_skip), 0x2807a, 0x0000, 25, 20);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.olp_general_configuration0_reg.da_fwd_type), 0x2807a, 0x0000, 30, 28);

  /* Olp General Configuration1 */
  PCP_DB_REG_SET(Pcp_regs.elk.olp_general_configuration1_reg, 0x2807b, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.olp_general_configuration1_reg.source_device), 0x2807b, 0x0000, 10, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.olp_general_configuration1_reg.dismem), 0x2807b, 0x0000, 11, 11);

  /* Dsp Event Table */
  for(reg_ndx = 0; reg_ndx < PCP_ELK_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    PCP_DB_REG_SET(Pcp_regs.elk.dsp_event_table_reg[reg_ndx], (0x2807e + reg_ndx), 0x0000);
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_event_table_reg[reg_ndx].dsp_event_table), (0x2807e + reg_ndx), 0x0000, 31, 0);
  }

  /* Dsp Engine Configuration[1..2] */
  PCP_DB_REG_SET(Pcp_regs.elk.dsp_engine_configuration_reg[0], 0x2808f, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[0].max_cmd_delay), 0x2808f, 0x0000, 10, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[0].dsp_generation_en), 0x2808f, 0x0000, 11, 11);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[0].max_dsp_cmd), 0x2808f, 0x0000, 14, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[0].min_dsp), 0x2808f, 0x0000, 22, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[0].dsp_header_size), 0x2808f, 0x0000, 29, 24);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[0].dsp_header_en), 0x2808f, 0x0000, 30, 30);

  PCP_DB_REG_SET(Pcp_regs.elk.dsp_engine_configuration_reg[1], 0x28098, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[1].max_cmd_delay), 0x28098, 0x0000, 10, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[1].dsp_generation_en), 0x28098, 0x0000, 11, 11);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[1].max_dsp_cmd), 0x28098, 0x0000, 14, 12);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[1].min_dsp), 0x28098, 0x0000, 22, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[1].dsp_header_size), 0x28098, 0x0000, 29, 24);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_engine_configuration_reg[1].dsp_header_en), 0x28098, 0x0000, 30, 30);

  /* Dsp Header[1..2] */
  
  for(reg_ndx = 0; reg_ndx < PCP_ELK_OLP_DSP_HEADER_REG_MULT_NOF_REGS; reg_ndx++)
  {
    PCP_DB_REG_SET(Pcp_regs.elk.dsp_header_reg[0][reg_ndx], (0x28090 + reg_ndx), 0x0000);
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_header_reg[0][reg_ndx].dsp_header), (0x28090 + reg_ndx), 0x0000, 31, 0);
    PCP_DB_REG_SET(Pcp_regs.elk.dsp_header_reg[1][reg_ndx], (0x28099 + reg_ndx), 0x0000);
    PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.dsp_header_reg[1][reg_ndx].dsp_header), (0x28099 + reg_ndx), 0x0000, 31, 0);
  }

  /* Lpm Lkp Conf */
  PCP_DB_REG_SET(Pcp_regs.elk.lpm_lkp_conf_reg, 0x28121, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.lpm_req_bits63to60), 0x28121, 0x0000, 3, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.lpm_key_vrf_size), 0x28121, 0x0000, 7, 4);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.rld_is_shared), 0x28121, 0x0000, 8, 8);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.use_large_rld), 0x28121, 0x0000, 9, 9);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.lpm_sec_lkp_size), 0x28121, 0x0000, 12, 10);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.lpm_thrd_lkp_size), 0x28121, 0x0000, 15, 13);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.lpm_debug_mode), 0x28121, 0x0000, 16, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_lkp_conf_reg.lpm_debug_engine), 0x28121, 0x0000, 18, 17);
  
  /* lpm debug key:*/
  PCP_DB_REG_SET(Pcp_regs.elk.lpm_debug_key_0, 0x28124, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_debug_key_0.debug_key_0), 0x28124, 0x0000, 31, 0);

  PCP_DB_REG_SET(Pcp_regs.elk.lpm_debug_key_1, 0x28125, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_debug_key_1.debug_key_1), 0x28125, 0x0000, 7, 0);

  PCP_DB_REG_SET(Pcp_regs.elk.lpm_debug_pld, 0x28126, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_debug_pld.debug_pld), 0x28126, 0x0000, 31, 0);

  PCP_DB_REG_SET(Pcp_regs.elk.lpm_debug_trigger, 0x28127, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.elk.lpm_debug_trigger.debug_trigger), 0x28127, 0x0000, 0, 0);

}

/* Block registers initialization: OAM 	 */
STATIC void
  pcp_regs_init_OAM(void)
{

  Pcp_regs.oam.nof_instances = PCP_BLK_NOF_INSTANCES_OAM;
  Pcp_regs.oam.addr.base = 0x2c000;
  Pcp_regs.oam.addr.step = 0x0000;

  /* Interrupt Register */
  PCP_DB_REG_SET(Pcp_regs.oam.interrupt_reg, 0x2c000, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.interrupt_reg.msg_fifo_not_empty), 0x2c000, 0x0000, 0, 0);

  /* Interrupt Mask Register */
  PCP_DB_REG_SET(Pcp_regs.oam.interrupt_mask_reg, 0x2c010, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.interrupt_mask_reg.msg_fifo_not_empty_mask), 0x2c010, 0x0000, 0, 0);

  /* Version */
  PCP_DB_REG_SET(Pcp_regs.oam.version_reg, 0x2c012, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.version_reg.version), 0x2c012, 0x0000, 31, 0);

  /* Scratchpad */
  PCP_DB_REG_SET(Pcp_regs.oam.scratchpad_reg, 0x2c013, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.scratchpad_reg.scratchpad), 0x2c013, 0x0000, 31, 0);

  /* Mode Register */
  PCP_DB_REG_SET(Pcp_regs.oam.mode_reg, 0x2c014, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.mode_reg.soft_reset), 0x2c014, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.mode_reg.ccm_tx_enable), 0x2c014, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.mode_reg.ccm_scan_enable), 0x2c014, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.mode_reg.rx2_cpu), 0x2c014, 0x0000, 3, 3);

  /* CPUPORT */
  PCP_DB_REG_SET(Pcp_regs.oam.cpuport_reg, 0x2c015, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.cpuport_reg.cpu_sys_port), 0x2c015, 0x0000, 12, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.cpuport_reg.cpu_port_tc), 0x2c015, 0x0000, 19, 17);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.cpuport_reg.cpu_port_dp), 0x2c015, 0x0000, 23, 22);

  /* status1reg */
  PCP_DB_REG_SET(Pcp_regs.oam.status1_reg, 0x2c016, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.status1_reg.txc_bp), 0x2c016, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.status1_reg.rxc_bp), 0x2c016, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.status1_reg.itr_drop), 0x2c016, 0x0000, 2, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.status1_reg.oversized_rx_packet), 0x2c016, 0x0000, 3, 3);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.status1_reg.itr_count), 0x2c016, 0x0000, 11, 4);

  /* TX TPID */
  PCP_DB_REG_SET(Pcp_regs.oam.tx_tpid_reg, 0x2c017, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.tx_tpid_reg.tx_tpid), 0x2c017, 0x0000, 15, 0);

  /* TIMERS */
  PCP_DB_REG_SET(Pcp_regs.oam.timers_reg, 0x2c018, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.timers_reg.count100), 0x2c018, 0x0000, 6, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.timers_reg.count_seconds), 0x2c018, 0x0000, 17, 8);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_0, 0x2c020, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_0.indirect_command_wr_data), 0x2c020, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_1, 0x2c021, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_1.indirect_command_wr_data), 0x2c021, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_2, 0x2c022, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_2.indirect_command_wr_data), 0x2c022, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_3, 0x2c023, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_3.indirect_command_wr_data), 0x2c023, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_4, 0x2c024, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_4.indirect_command_wr_data), 0x2c024, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_5, 0x2c025, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_5.indirect_command_wr_data), 0x2c025, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_6, 0x2c026, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_6.indirect_command_wr_data), 0x2c026, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_7, 0x2c027, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_7.indirect_command_wr_data), 0x2c027, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_8, 0x2c028, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_8.indirect_command_wr_data), 0x2c028, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_9, 0x2c029, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_9.indirect_command_wr_data), 0x2c029, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_wr_data_reg_10, 0x2c02a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_wr_data_reg_10.indirect_command_wr_data), 0x2c02a, 0x0000, 11, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_0, 0x2c030, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_0.indirect_command_rd_data), 0x2c030, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_1, 0x2c031, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_1.indirect_command_rd_data), 0x2c031, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_2, 0x2c032, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_2.indirect_command_rd_data), 0x2c032, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_3, 0x2c033, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_3.indirect_command_rd_data), 0x2c033, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_4, 0x2c034, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_4.indirect_command_rd_data), 0x2c034, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_5, 0x2c035, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_5.indirect_command_rd_data), 0x2c035, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_6, 0x2c036, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_6.indirect_command_rd_data), 0x2c036, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_7, 0x2c037, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_7.indirect_command_rd_data), 0x2c037, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_8, 0x2c038, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_8.indirect_command_rd_data), 0x2c038, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_9, 0x2c039, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_9.indirect_command_rd_data), 0x2c039, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_rd_data_reg_10, 0x2c03a, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_rd_data_reg_10.indirect_command_rd_data), 0x2c03a, 0x0000, 11, 0);

  /* Indirect Command */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_reg, 0x2c040, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_reg.indirect_command_trigger), 0x2c040, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_reg.indirect_command_trigger_on_data), 0x2c040, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_reg.indirect_command_count), 0x2c040, 0x0000, 15, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_reg.indirect_command_timeout), 0x2c040, 0x0000, 30, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_reg.indirect_command_status), 0x2c040, 0x0000, 31, 31);

  /* Indirect Command Address */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_address_reg, 0x2c041, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_address_reg.indirect_command_addr), 0x2c041, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_address_reg.indirect_command_type), 0x2c041, 0x0000, 31, 31);

  /* Indirect Command Data Increment */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_data_increment_reg_0, 0x2c042, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_data_increment_reg_0.indirect_command_data_increment), 0x2c042, 0x0000, 31, 0);

  /* Indirect Command Data Increment */
  PCP_DB_REG_SET(Pcp_regs.oam.indirect_command_data_increment_reg_1, 0x2c043, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.indirect_command_data_increment_reg_1.indirect_command_data_increment), 0x2c043, 0x0000, 31, 0);

  /* MAC SA BASE */
  PCP_DB_REG_SET(Pcp_regs.oam.mac_sa_base_reg_0, 0x2c100, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.mac_sa_base_reg_0.mac_sa_base), 0x2c100, 0x0000, 31, 0);

  /* MAC SA BASE */
  PCP_DB_REG_SET(Pcp_regs.oam.mac_sa_base_reg_1, 0x2c101, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.mac_sa_base_reg_1.mac_sa_base), 0x2c101, 0x0000, 8, 0);

  /* Interrupt Message */
  PCP_DB_REG_SET(Pcp_regs.oam.interrupt_message_reg, 0x2c102, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.interrupt_message_reg.interrupt_message), 0x2c102, 0x0000, 31, 0);

  /* Rxc Counters */
  PCP_DB_REG_SET(Pcp_regs.oam.rxc_counters_reg, 0x2c103, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.rxc_counters_reg.oam_sop_count), 0x2c103, 0x0000, 15, 0);

  /* Txc Counters */
  PCP_DB_REG_SET(Pcp_regs.oam.txc_counters_reg, 0x2c104, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.txc_counters_reg.down_sop_count), 0x2c104, 0x0000, 15, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.txc_counters_reg.up_sop_count), 0x2c104, 0x0000, 31, 16);

  /* Enable Interrupt Message Event */
  PCP_DB_REG_SET(Pcp_regs.oam.enable_interrupt_message_event_reg, 0x2c105, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.enable_interrupt_message_event_reg.enable_ccm_timeout_set_event), 0x2c105, 0x0000, 0, 0);

  /* Debug Reg0 */
  PCP_DB_REG_SET(Pcp_regs.oam.debug_reg0_reg, 0x2c106, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.debug_reg0_reg.debug_reg0), 0x2c106, 0x0000, 31, 0);

  /* PTCH size */
  PCP_DB_REG_SET(Pcp_regs.oam.ptch_size_reg, 0x2c107, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.ptch_size_reg.ptch_size), 0x2c107, 0x0000, 2, 0);

  /* CPU PTCH */
  PCP_DB_REG_SET(Pcp_regs.oam.cpu_ptch_reg, 0x2c108, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.oam.cpu_ptch_reg.cpu_ptch), 0x2c108, 0x0000, 31, 0);
}

/* Block registers initialization: STS 	 */
STATIC void
  pcp_regs_init_STS(void)
{

  Pcp_regs.sts.nof_instances = PCP_BLK_NOF_INSTANCES_STS;
  Pcp_regs.sts.addr.base = 0x30000;
  Pcp_regs.sts.addr.step = 0x0000;

  /* Interrupt Register */
  PCP_DB_REG_SET(Pcp_regs.sts.interrupt_reg, 0x30000, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.interrupt_reg.one_error_fixed), 0x30000, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.interrupt_reg.two_errors_detected), 0x30000, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.interrupt_reg.loss_sync), 0x30000, 0x0000, 2, 2);

  /* Interrupt Mask Register */
  PCP_DB_REG_SET(Pcp_regs.sts.interrupt_mask_reg, 0x30010, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.interrupt_mask_reg.one_error_fixed_mask), 0x30010, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.interrupt_mask_reg.two_errors_detected_mask), 0x30010, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.interrupt_mask_reg.loss_sync_mask), 0x30010, 0x0000, 2, 2);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_wr_data_reg_0, 0x30020, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_wr_data_reg_0.indirect_command_wr_data), 0x30020, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_wr_data_reg_1, 0x30021, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_wr_data_reg_1.indirect_command_wr_data), 0x30021, 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_wr_data_reg_2, 0x30022, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_wr_data_reg_2.indirect_command_wr_data), 0x30022, 0x0000, 15, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_rd_data_reg_0, 0x30030, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_rd_data_reg_0.indirect_command_rd_data), 0x30030, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_rd_data_reg_1, 0x30031, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_rd_data_reg_1.indirect_command_rd_data), 0x30031, 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_rd_data_reg_2, 0x30032, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_rd_data_reg_2.indirect_command_rd_data), 0x30032, 0x0000, 15, 0);

  /* Indirect Command */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_reg, 0x30040, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_reg.indirect_command_trigger), 0x30040, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_reg.indirect_command_trigger_on_data), 0x30040, 0x0000, 1, 1);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_reg.indirect_command_count), 0x30040, 0x0000, 15, 2);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_reg.indirect_command_timeout), 0x30040, 0x0000, 30, 16);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_reg.indirect_command_status), 0x30040, 0x0000, 31, 31);

  /* Indirect Command Address */
  PCP_DB_REG_SET(Pcp_regs.sts.indirect_command_address_reg, 0x30041, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_address_reg.indirect_command_addr), 0x30041, 0x0000, 30, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.indirect_command_address_reg.indirect_command_type), 0x30041, 0x0000, 31, 31);

  /* Mode */
  PCP_DB_REG_SET(Pcp_regs.sts.mode_reg, 0x30100, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.mode_reg.ecc_mode), 0x30100, 0x0000, 0, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.mode_reg.stat_mode), 0x30100, 0x0000, 2, 1);

    /* AC offset */
  PCP_DB_REG_SET(Pcp_regs.sts.ac_offset_reg, 0x30101, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.ac_offset_reg.in_ac_offset), 0x30101, 0x0000, 2, 0);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.ac_offset_reg.out_ac_offset), 0x30101, 0x0000, 5, 3);

  /* STS Status */
  PCP_DB_REG_SET(Pcp_regs.sts.sts_status_reg, 0x30102, 0x0000);
  PCP_DB_REG_FLD_SET( &(Pcp_regs.sts.sts_status_reg.report_sync), 0x30102, 0x0000, 0, 0);
}
/*****************************************************
*NAME
* pcp_regs_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to registers database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int   unit - device identifier
*    SOC_SAND_OUT PCP_REGS  *pcp_regs - pointer to pcp
*                           registers database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *pcp_regs.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*SEE ALSO:
*****************************************************/
uint32
  pcp_regs_get(
    SOC_SAND_OUT PCP_REGS  **pcp_regs
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_REGS_GET);

  res = pcp_regs_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *pcp_regs = &Pcp_regs;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_regs_get()", 0, 0);
}

/*****************************************************
*NAME
*  pcp_regs_init
*TYPE:
*  PROC
*FUNCTION:
*  Dynamically allocates and initializes Pcp registers database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized registers will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  pcp_regs_init(
  )
{
  static uint8    Pcp_regs_initialized = FALSE;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_REGS_INIT);

  if (Pcp_regs_initialized == TRUE)
  {
    goto exit;
  }

  pcp_regs_init_ECI();
  pcp_regs_init_CSI();
  pcp_regs_init_ELK();
  pcp_regs_init_OAM();
  pcp_regs_init_STS();

  Pcp_regs_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_regs_init",0,0);
}

PCP_REGS*
  pcp_regs()
{
  PCP_REGS*
    regs = NULL;

  pcp_regs_get(
    &(regs)
  );

  return regs;
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_chip_regs_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_chip_regs;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_chip_regs_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_chip_regs;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

