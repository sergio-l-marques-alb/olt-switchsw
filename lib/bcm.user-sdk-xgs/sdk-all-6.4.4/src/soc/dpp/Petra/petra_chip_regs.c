/* $Id: petra_chip_regs.c,v 1.9 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#endif
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
#ifdef LINK_PA_LIBRARIES
static SOC_PA_REGS  Soc_petra_a_regs;
#else
static SOC_PETRA_INVALID_DEVICE_REG  Soc_petra_a_regs;
#endif
static uint8    Soc_petra_a_regs_initialized = FALSE;

#ifdef LINK_PB_LIBRARIES
#else
static SOC_PETRA_INVALID_DEVICE_REG  Soc_pb_regs;
static uint8    Soc_pb_regs_initialized = FALSE;
#endif

static SOC_PETRA_REGS  Soc_petra_regs;
static uint8   Soc_petra_regs_initialized = FALSE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* soc_petra_reg_fld_set
*TYPE:
*  PROC
*DATE:
*  27/07/2007
*FUNCTION:
*  Sets a soc_petra register field
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_PETRA_REG_FIELD *field - pointer to soc_petra register field structure
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
  soc_petra_reg_fld_set(
    SOC_PETRA_REG_FIELD *field,
    uint32         base,
    uint16         step,
    uint8       msb,
    uint8       lsb
  )
{
   field->addr.base = base;
   field->addr.step = step;
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}

#ifdef LINK_PA_LIBRARIES
uint8
  soc_pa_regs_is_dprc_reg(
    SOC_SAND_IN SOC_PETRA_REG_ADDR* addr
  )
{
  uint8
    is_drc = FALSE,
    is_dpi = FALSE,
    is_dprc = FALSE;

  if (addr != NULL)
  {
    if (
        (addr->base >= Soc_petra_a_regs.drc.addr.base) &&
        (addr->base <= Soc_petra_a_regs.drc.training_sequence_configuration_reg.addr.base)
      )
    {
      is_drc = TRUE;
    }

    if (
        (addr->base >= Soc_petra_a_regs.dpi.addr.base) &&
        (addr->base <= Soc_petra_a_regs.dpi.dpi_retrain_reg.addr.base)
      )
    {
      is_dpi = TRUE;
    }
  }

  is_dprc = (is_dpi || is_drc);

  return is_dprc;
}

STATIC void
  soc_petra_regs_block_pcmi_registers_init(
    SOC_SAND_IN  uint32                           addr,
    SOC_SAND_IN  uint16                           step,
    SOC_SAND_OUT SOC_PETRA_REGS_PCMI_CONFIG_REG_FORMAT  *config,
    SOC_SAND_OUT SOC_PETRA_REGS_PCMI_RESULTS_REG_FORMAT *results
  )
{
  /* Pcmi Config */
  config->addr.base = sizeof(uint32) * addr;
  config->addr.step = sizeof(uint32) * step;
  soc_petra_reg_fld_set( &(config->cntr_data), sizeof(uint32) * addr, sizeof(uint32) * step, 30, 0);

  /* Pcmi Config */
  results->addr.base = sizeof(uint32) * (addr + 0x0001);
  results->addr.step = sizeof(uint32) * step;
  soc_petra_reg_fld_set( &(results->pcm_ivrgoc_cntr), sizeof(uint32) * (addr + 0x0001), sizeof(uint32) * step, 29, 0);
  soc_petra_reg_fld_set( &(results->pcm_iv_clk_exp), sizeof(uint32) * (addr + 0x0001), sizeof(uint32) * step, 30, 30);
  soc_petra_reg_fld_set( &(results->pcm_ivrgoc_overflw), sizeof(uint32) * (addr + 0x0001), sizeof(uint32) * step, 31, 31);
}

/* Block registers initialization: ECI */
STATIC void
  soc_pa_regs_init_ECI(void)
{
  uint32
     fld_idx;
  Soc_petra_a_regs.eci.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_ECI;
  Soc_petra_a_regs.eci.addr.base = sizeof(uint32) * 0x0000;
  Soc_petra_a_regs.eci.addr.step = sizeof(uint32) * 0x0000;

  /* Version Register */
  Soc_petra_a_regs.eci.version_reg.addr.base = sizeof(uint32) * 0x0000;
  Soc_petra_a_regs.eci.version_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.version_reg.chip_type), sizeof(uint32) * 0x0000, sizeof(uint32) * 0x0000, 23, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.version_reg.dbg_ver), sizeof(uint32) * 0x0000, sizeof(uint32) * 0x0000, 27, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.version_reg.chip_ver), sizeof(uint32) * 0x0000, sizeof(uint32) * 0x0000, 31, 28);

  /* Identification Register */
  Soc_petra_a_regs.eci.identification_reg.addr.base = sizeof(uint32) * 0x0001;
  Soc_petra_a_regs.eci.identification_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.identification_reg.pipe_id), sizeof(uint32) * 0x0001, sizeof(uint32) * 0x0000, 10, 0);

  /* Oc768cMode Register */
  Soc_petra_a_regs.eci.oc768c_and_misc_reg.addr.base = sizeof(uint32) * 0x0002;
  Soc_petra_a_regs.eci.oc768c_and_misc_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.oc768c_and_misc_reg.oc768c_mode), sizeof(uint32) * 0x0002, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.oc768c_and_misc_reg.dcf_a), sizeof(uint32) * 0x0002, sizeof(uint32) * 0x0000, 3, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.oc768c_and_misc_reg.dcf_p), sizeof(uint32) * 0x0002, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.oc768c_and_misc_reg.dcf_disable), sizeof(uint32) * 0x0002, sizeof(uint32) * 0x0000, 5, 5);

  /* Spare register. Used by SW for managment flags                 */
  Soc_petra_a_regs.eci.spare_flags_reg.addr.base = sizeof(uint32) * 0x0003;
  Soc_petra_a_regs.eci.spare_flags_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.spare_flags_reg.hot_init_done), sizeof(uint32) * 0x0003, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.spare_flags_reg.sch_flow_ipf_is_prop_not_inverse), sizeof(uint32) * 0x0003, sizeof(uint32) * 0x0000, 31, 31);

  /* Device revision register */
  Soc_petra_a_regs.eci.revision_reg.addr.base = sizeof(uint32) * 0x0004;
  Soc_petra_a_regs.eci.revision_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.revision_reg.revision), sizeof(uint32) * 0x0004, sizeof(uint32) * 0x0000, 11, 0);

  /* Cpu Asynchronous Packet Data */
  Soc_petra_a_regs.eci.cpu_asynchronous_packet_data_reg.addr.base = sizeof(uint32) * 0x0005;
  Soc_petra_a_regs.eci.cpu_asynchronous_packet_data_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_asynchronous_packet_data_reg.tdata), sizeof(uint32) * 0x0005, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_asynchronous_packet_data_reg.tcmd), sizeof(uint32) * 0x0005, sizeof(uint32) * 0x0000, 18, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_asynchronous_packet_data_reg.tdv), sizeof(uint32) * 0x0005, sizeof(uint32) * 0x0000, 20, 20);

  /* Unicast Dbuff Pointers Start */
  Soc_petra_a_regs.eci.unicast_dbuff_pointers_start_reg.addr.base = sizeof(uint32) * 0x0007;
  Soc_petra_a_regs.eci.unicast_dbuff_pointers_start_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.unicast_dbuff_pointers_start_reg.uc_db_ptr_start), sizeof(uint32) * 0x0007, sizeof(uint32) * 0x0000, 20, 0);

  /* Unicast Dbuff Pointers End */
  Soc_petra_a_regs.eci.unicast_dbuff_pointers_end_reg.addr.base = sizeof(uint32) * 0x0008;
  Soc_petra_a_regs.eci.unicast_dbuff_pointers_end_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.unicast_dbuff_pointers_end_reg.uc_db_ptr_end), sizeof(uint32) * 0x0008, sizeof(uint32) * 0x0000, 20, 0);

  /* Mini Multicast Dbuff Pointers Start */
  Soc_petra_a_regs.eci.mini_multicast_dbuff_pointers_start_reg.addr.base = sizeof(uint32) * 0x0009;
  Soc_petra_a_regs.eci.mini_multicast_dbuff_pointers_start_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mini_multicast_dbuff_pointers_start_reg.mn_mul_db_ptr_start), sizeof(uint32) * 0x0009, sizeof(uint32) * 0x0000, 20, 0);

  /* Mini Multicast Dbuff Pointers End */
  Soc_petra_a_regs.eci.mini_multicast_dbuff_pointers_end_reg.addr.base = sizeof(uint32) * 0x000a;
  Soc_petra_a_regs.eci.mini_multicast_dbuff_pointers_end_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mini_multicast_dbuff_pointers_end_reg.mn_mul_db_ptr_end), sizeof(uint32) * 0x000a, sizeof(uint32) * 0x0000, 20, 0);

  /* Full Multicast Dbuff Pointers Start */
  Soc_petra_a_regs.eci.full_multicast_dbuff_pointers_start_reg.addr.base = sizeof(uint32) * 0x000b;
  Soc_petra_a_regs.eci.full_multicast_dbuff_pointers_start_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.full_multicast_dbuff_pointers_start_reg.fl_mul_db_ptr_start), sizeof(uint32) * 0x000b, sizeof(uint32) * 0x0000, 20, 0);

  /* Full Multicast Dbuff Pointers End */
  Soc_petra_a_regs.eci.full_multicast_dbuff_pointers_end_reg.addr.base = sizeof(uint32) * 0x000c;
  Soc_petra_a_regs.eci.full_multicast_dbuff_pointers_end_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.full_multicast_dbuff_pointers_end_reg.fl_mul_db_ptr_end), sizeof(uint32) * 0x000c, sizeof(uint32) * 0x0000, 20, 0);

  /* Soc_petra Soft Reset */
  Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.addr.base = sizeof(uint32) * 0x000d;
  Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.mas_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.nifa_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.nifb_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.sch_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.ihp_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 4, 4);
  for (fld_idx = 0; fld_idx < SOC_PETRA_BLK_NOF_INSTANCES_DRC; fld_idx++)
  {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.dprc_reset[fld_idx]),
        sizeof(uint32) * 0x000d,
        sizeof(uint32) * 0x0000,
        (uint8)(5 + fld_idx),
        (uint8)(5 + fld_idx)
      );

  }
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.irdp_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.mmu_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.egq_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.epni_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.fdrc_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.pts_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.iqm_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.olp_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.mdio_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.cfc_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.mcc_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_reset_reg.qdio_reset), sizeof(uint32) * 0x000d, sizeof(uint32) * 0x0000, 22, 22);

  /* Soc_petra Soft Init */
  Soc_petra_a_regs.eci.soc_petra_soft_init_reg.addr.base = sizeof(uint32) * 0x000e;
  Soc_petra_a_regs.eci.soc_petra_soft_init_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.ips_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.iqm_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.qdr_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.ipt_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.mmu_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 4, 4);

  for (fld_idx = 0; fld_idx < SOC_PETRA_BLK_NOF_INSTANCES_DRC; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.dprc_init[fld_idx]),
      sizeof(uint32) * 0x000e,
      sizeof(uint32) * 0x0000,
     (uint8)(5 + fld_idx),
     (uint8)(5 + fld_idx)
    );
  }

  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.ire_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.ihp_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.idr_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.irr_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.fdt_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.fdr_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.fct_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.fcr_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.rtp_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.egq_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.epni_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.maca_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.macb_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.macc_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.sch_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.olp_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.msw_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.mcc_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.nifa_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.nifb_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.soc_petra_soft_init_reg.mdio_init), sizeof(uint32) * 0x000e, sizeof(uint32) * 0x0000, 31, 31);

  /* Ingress Shaping Queue Boundaries */
  Soc_petra_a_regs.eci.ingress_shaping_queue_boundaries_reg.addr.base = sizeof(uint32) * 0x000f;
  Soc_petra_a_regs.eci.ingress_shaping_queue_boundaries_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ingress_shaping_queue_boundaries_reg.isp_qnum_low), sizeof(uint32) * 0x000f, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ingress_shaping_queue_boundaries_reg.isp_qnum_high), sizeof(uint32) * 0x000f, sizeof(uint32) * 0x0000, 30, 16);

  /* Fabric Multicast Queue Boundaries */
  Soc_petra_a_regs.eci.fabric_multicast_queue_boundaries_reg.addr.base = sizeof(uint32) * 0x0010;
  Soc_petra_a_regs.eci.fabric_multicast_queue_boundaries_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.fabric_multicast_queue_boundaries_reg.fmc_qnum_low), sizeof(uint32) * 0x0010, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.fabric_multicast_queue_boundaries_reg.fmc_qnum_high), sizeof(uint32) * 0x0010, sizeof(uint32) * 0x0000, 30, 16);

  /* Drc Bist Enables */
  Soc_petra_a_regs.eci.drc_bist_enables_reg.addr.base = sizeof(uint32) * 0x0011;
  Soc_petra_a_regs.eci.drc_bist_enables_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drca_bist_en), sizeof(uint32) * 0x0011, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drcb_bist_en), sizeof(uint32) * 0x0011, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drcc_bist_en), sizeof(uint32) * 0x0011, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drcd_bist_en), sizeof(uint32) * 0x0011, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drce_bist_en), sizeof(uint32) * 0x0011, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drcf_bist_en), sizeof(uint32) * 0x0011, sizeof(uint32) * 0x0000, 5, 5);
  for (fld_idx = 0; fld_idx < SOC_PETRA_DRC_NOF_BIST_ENABLES; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.drc_bist_enables_reg.drcs_bist_en[fld_idx]), sizeof(uint32) * 0x0011 , sizeof(uint32) * 0x0000, (uint8)fld_idx, (uint8)fld_idx);
  }


  /* General Controls */
  Soc_petra_a_regs.eci.general_controls_reg.addr.base = sizeof(uint32) * 0x0012;
  Soc_petra_a_regs.eci.general_controls_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.fsc_en), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.mesh_mode), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.sel_stat_data_out), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.stat_ddr_mode), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.stat_tag_en), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.qdr_par_sel), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.add_dram_crc), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.no_fab_crc), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.en_16k_mul), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.tdm_mode), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.stat_oe), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.dbuff_size), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.ftmh_ext_mode), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 21, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.hdr_type), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 25, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.general_controls_reg.stat_out_phase), sizeof(uint32) * 0x0012, sizeof(uint32) * 0x0000, 29, 28);

  /* Svlan Tpid Configuration Register */
  Soc_petra_a_regs.eci.svlan_tpid_configuration_reg.addr.base = sizeof(uint32) * 0x0013;
  Soc_petra_a_regs.eci.svlan_tpid_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.svlan_tpid_configuration_reg.svlan_tpid_val), sizeof(uint32) * 0x0013, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.svlan_tpid_configuration_reg.svlan_tpid), sizeof(uint32) * 0x0013, sizeof(uint32) * 0x0000, 31, 16);

  /* Cvlan Tpid Configuration Register */
  Soc_petra_a_regs.eci.cvlan_tpid_configuration_reg.addr.base = sizeof(uint32) * 0x0014;
  Soc_petra_a_regs.eci.cvlan_tpid_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cvlan_tpid_configuration_reg.cvlan_tpid_val), sizeof(uint32) * 0x0014, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cvlan_tpid_configuration_reg.cvlan_tpid), sizeof(uint32) * 0x0014, sizeof(uint32) * 0x0000, 31, 16);

  /* RLAG2 Configuration Register */
  Soc_petra_a_regs.eci.rlag2_configuration_reg.addr.base = sizeof(uint32) * 0x0015;
  Soc_petra_a_regs.eci.rlag2_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.rlag2_configuration_reg.rlag2_low), sizeof(uint32) * 0x0015, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.rlag2_configuration_reg.rlag2_high), sizeof(uint32) * 0x0015, sizeof(uint32) * 0x0000, 27, 16);

  /* RLAG4 Configuration Register */
  Soc_petra_a_regs.eci.rlag4_configuration_reg.addr.base = sizeof(uint32) * 0x0016;
  Soc_petra_a_regs.eci.rlag4_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.rlag4_configuration_reg.rlag4_low), sizeof(uint32) * 0x0016, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.rlag4_configuration_reg.rlag4_high), sizeof(uint32) * 0x0016, sizeof(uint32) * 0x0000, 27, 16);

  /* Endian Mode */
  Soc_petra_a_regs.eci.endian_mode_reg.addr.base = sizeof(uint32) * 0x0017;
  Soc_petra_a_regs.eci.endian_mode_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.endian_mode_reg.little_endian), sizeof(uint32) * 0x0017, sizeof(uint32) * 0x0000, 31, 0);

  /* Byte Access Order */
  Soc_petra_a_regs.eci.byte_access_order_reg.addr.base = sizeof(uint32) * 0x0018;
  Soc_petra_a_regs.eci.byte_access_order_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.byte_access_order_reg.fourth_byte_access), sizeof(uint32) * 0x0018, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.byte_access_order_reg.third_byte_access), sizeof(uint32) * 0x0018, sizeof(uint32) * 0x0000, 3, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.byte_access_order_reg.second_byte_access), sizeof(uint32) * 0x0018, sizeof(uint32) * 0x0000, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.byte_access_order_reg.first_byte_access), sizeof(uint32) * 0x0018, sizeof(uint32) * 0x0000, 7, 6);

  Soc_petra_a_regs.eci.mesh_ctxt_reg.addr.base = sizeof(uint32) * 0x0019;
  Soc_petra_a_regs.eci.mesh_ctxt_reg.addr.step = sizeof(uint32) * 0x0000;

  /* Test Register */
  Soc_petra_a_regs.eci.test_reg.addr.base = sizeof(uint32) * 0x001a;
  Soc_petra_a_regs.eci.test_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.test_reg.test_register), sizeof(uint32) * 0x001a, sizeof(uint32) * 0x0000, 31, 0);

  /* Power Down And Cpu Streaming If Statuses */
  Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.addr.base = sizeof(uint32) * 0x001b;
  Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.maca_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.macb_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.macc_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.msw_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.nifa_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.dprca_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.dprcb_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.dprcc_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.dprcd_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.dprce_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.dprcf_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.pdmca_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.pdmcb_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.qdio_pd), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.cpuif_mode), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.cpuif_speed), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 21, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.cpuif_out_phase), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 25, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.power_down_and_cpu_streaming_if_statuses_reg.cpuif_in_phase), sizeof(uint32) * 0x001b, sizeof(uint32) * 0x0000, 26, 26);

  /* Cpu Streaming If Configurations */
  Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.addr.base = sizeof(uint32) * 0x001c;
  Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.cpuif_multi_port_mode), sizeof(uint32) * 0x001c, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.cpuif_enable_timeoutcnt), sizeof(uint32) * 0x001c, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.cpuif_timeout_prd), sizeof(uint32) * 0x001c, sizeof(uint32) * 0x0000, 22, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.cpuif_quiet_mode), sizeof(uint32) * 0x001c, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_configurations_reg.cpuif_no_dis_bad_parity), sizeof(uint32) * 0x001c, sizeof(uint32) * 0x0000, 25, 25);

  /* Olp Streaming If Configurations */
  Soc_petra_a_regs.eci.olp_streaming_if_configurations_reg.addr.base = sizeof(uint32) * 0x001d;
  Soc_petra_a_regs.eci.olp_streaming_if_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_configurations_reg.olpif_enable_timeoutcnt), sizeof(uint32) * 0x001d, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_configurations_reg.olpif_timeout_prd), sizeof(uint32) * 0x001d, sizeof(uint32) * 0x0000, 22, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_configurations_reg.olpif_quiet_mode), sizeof(uint32) * 0x001d, sizeof(uint32) * 0x0000, 24, 24);

  /* Cpu Streaming If Interrupts */
  Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.addr.base = sizeof(uint32) * 0x001e;
  Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_rx_pkt_prty_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_rx_cmd_prty_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_rx_pkt_seq_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_rx_cmd_type_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_rx_cmd_seq_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_read_reg_timeout), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_tx_sop_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_tx_eop_err), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_reg.cpuif_bad_parity_data), sizeof(uint32) * 0x001e, sizeof(uint32) * 0x0000, 31, 16);

  /* Cpu Streaming If Interrupts Mask Register */
  Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.addr.base = sizeof(uint32) * 0x001f;
  Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_rx_pkt_prty_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_rx_cmd_prty_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_rx_pkt_seq_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_rx_cmd_type_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_rx_cmd_seq_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_read_reg_timeout_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_tx_sop_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_streaming_if_interrupts_mask_reg.cpuif_tx_eop_err_mask), sizeof(uint32) * 0x001f, sizeof(uint32) * 0x0000, 9, 9);

  /* Olp Streaming If Interrupts */
  Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.addr.base = sizeof(uint32) * 0x0020;
  Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.olpif_rx_pkt_seq_err), sizeof(uint32) * 0x0020, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.olpif_rx_cmd_type_err), sizeof(uint32) * 0x0020, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.olpif_rx_cmd_seq_err), sizeof(uint32) * 0x0020, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.olpif_read_reg_timeout), sizeof(uint32) * 0x0020, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.olpif_tx_sop_err), sizeof(uint32) * 0x0020, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_reg.olpif_tx_eop_err), sizeof(uint32) * 0x0020, sizeof(uint32) * 0x0000, 9, 9);

  /* Olp Streaming If Interrupts Mask Register */
  Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.addr.base = sizeof(uint32) * 0x0021;
  Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.olpif_rx_pkt_seq_err_mask), sizeof(uint32) * 0x0021, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.olpif_rx_cmd_type_err_mask), sizeof(uint32) * 0x0021, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.olpif_rx_cmd_seq_err_mask), sizeof(uint32) * 0x0021, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.olpif_read_reg_timeout_mask), sizeof(uint32) * 0x0021, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.olpif_tx_sop_err_mask), sizeof(uint32) * 0x0021, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.olp_streaming_if_interrupts_mask_reg.olpif_tx_eop_err_mask), sizeof(uint32) * 0x0021, sizeof(uint32) * 0x0000, 9, 9);

  /* Asynchronous Mode Interrupts */
  Soc_petra_a_regs.eci.asynchronous_mode_interrupts_reg.addr.base = sizeof(uint32) * 0x0022;
  Soc_petra_a_regs.eci.asynchronous_mode_interrupts_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.asynchronous_mode_interrupts_reg.asynch_byte_order_err), sizeof(uint32) * 0x0022, sizeof(uint32) * 0x0000, 0, 0);

  /* Asynchronous Mode Interrupts Mask Register */
  Soc_petra_a_regs.eci.asynchronous_mode_interrupts_mask_reg.addr.base = sizeof(uint32) * 0x0023;
  Soc_petra_a_regs.eci.asynchronous_mode_interrupts_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.asynchronous_mode_interrupts_mask_reg.asynch_byte_order_err_mask), sizeof(uint32) * 0x0023, sizeof(uint32) * 0x0000, 0, 0);

  /* Cpu Pads Configurations */
  Soc_petra_a_regs.eci.cpu_pads_configurations_reg.addr.base = sizeof(uint32) * 0x0024;
  Soc_petra_a_regs.eci.cpu_pads_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_pads_configurations_reg.cpu_cr_val), sizeof(uint32) * 0x0024, sizeof(uint32) * 0x0000, 4, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_pads_configurations_reg.cpu_use_cr_cfg), sizeof(uint32) * 0x0024, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_pads_configurations_reg.cpu_en_odt), sizeof(uint32) * 0x0024, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_pads_configurations_reg.cpu_tx_imp), sizeof(uint32) * 0x0024, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.cpu_pads_configurations_reg.cpu_cr), sizeof(uint32) * 0x0024, sizeof(uint32) * 0x0000, 20, 16);

  /* Misc Pads Configurations */
  Soc_petra_a_regs.eci.misc_pads_configurations_reg.addr.base = sizeof(uint32) * 0x0025;
  Soc_petra_a_regs.eci.misc_pads_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.misc_pads_configurations_reg.misc_cr_val), sizeof(uint32) * 0x0025, sizeof(uint32) * 0x0000, 4, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.misc_pads_configurations_reg.misc_use_cr_cfg), sizeof(uint32) * 0x0025, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.misc_pads_configurations_reg.misc_en_odt), sizeof(uint32) * 0x0025, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.misc_pads_configurations_reg.misc_tx_imp), sizeof(uint32) * 0x0025, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.misc_pads_configurations_reg.misc_cr), sizeof(uint32) * 0x0025, sizeof(uint32) * 0x0000, 20, 16);

  /* Tdm Configuration */
  Soc_petra_a_regs.eci.tdm_configuration_reg.addr.base = sizeof(uint32) * 0x0026;
  Soc_petra_a_regs.eci.tdm_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.tdm_configuration_reg.tdm_frg_num), sizeof(uint32) * 0x0026, sizeof(uint32) * 0x0000, 8, 0);

  /* Ddr Pll Config */
  Soc_petra_a_regs.eci.ddr_pll_config_reg.addr.base = sizeof(uint32) * 0x0027;
  Soc_petra_a_regs.eci.ddr_pll_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_config_reg.ddr_pll_m), sizeof(uint32) * 0x0027, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_config_reg.ddr_pll_n), sizeof(uint32) * 0x0027, sizeof(uint32) * 0x0000, 12, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_config_reg.ddr_pll_k), sizeof(uint32) * 0x0027, sizeof(uint32) * 0x0000, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_config_reg.ddr_pll_p), sizeof(uint32) * 0x0027, sizeof(uint32) * 0x0000, 26, 24);

  /* Qdr Pll Config */
  Soc_petra_a_regs.eci.qdr_pll_config_reg.addr.base = sizeof(uint32) * 0x0028;
  Soc_petra_a_regs.eci.qdr_pll_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_config_reg.qdr_pll_m), sizeof(uint32) * 0x0028, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_config_reg.qdr_pll_n), sizeof(uint32) * 0x0028, sizeof(uint32) * 0x0000, 12, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_config_reg.qdr_pll_k), sizeof(uint32) * 0x0028, sizeof(uint32) * 0x0000, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_config_reg.qdr_pll_p), sizeof(uint32) * 0x0028, sizeof(uint32) * 0x0000, 26, 24);

  /* Ddr Pll Ext Prog */
  Soc_petra_a_regs.eci.ddr_pll_ext_prog_reg.addr.base = sizeof(uint32) * 0x0029;
  Soc_petra_a_regs.eci.ddr_pll_ext_prog_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_ext_prog_reg.ddr_pll_vprog), sizeof(uint32) * 0x0029, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_ext_prog_reg.ddr_pll_iprog), sizeof(uint32) * 0x0029, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.ddr_pll_ext_prog_reg.ddr_pll_rprog), sizeof(uint32) * 0x0029, sizeof(uint32) * 0x0000, 11, 8);

  /* Qdr Pll Ext Prog */
  Soc_petra_a_regs.eci.qdr_pll_ext_prog_reg.addr.base = sizeof(uint32) * 0x002a;
  Soc_petra_a_regs.eci.qdr_pll_ext_prog_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_ext_prog_reg.qdr_pll_vprog), sizeof(uint32) * 0x002a, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_ext_prog_reg.qdr_pll_iprog), sizeof(uint32) * 0x002a, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.qdr_pll_ext_prog_reg.qdr_pll_rprog), sizeof(uint32) * 0x002a, sizeof(uint32) * 0x0000, 11, 8);

  /* Pll Resets */
  Soc_petra_a_regs.eci.pll_resets_reg.addr.base = sizeof(uint32) * 0x002b;
  Soc_petra_a_regs.eci.pll_resets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.pll_resets_reg.ddr_pll_rst_n), sizeof(uint32) * 0x002b, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.pll_resets_reg.qdr_pll_rst_n), sizeof(uint32) * 0x002b, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.pll_resets_reg.qdr_pll_sel), sizeof(uint32) * 0x002b, sizeof(uint32) * 0x0000, 5, 5);

  /* Test Mux Config */
  Soc_petra_a_regs.eci.test_mux_config_reg.addr.base = sizeof(uint32) * 0x002c;
  Soc_petra_a_regs.eci.test_mux_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.test_mux_config_reg.test_mux_sel), sizeof(uint32) * 0x002c, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.test_mux_config_reg.test_mux_div), sizeof(uint32) * 0x002c, sizeof(uint32) * 0x0000, 6, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.test_mux_config_reg.ddr_lock), sizeof(uint32) * 0x002c, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.test_mux_config_reg.core_lock), sizeof(uint32) * 0x002c, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.test_mux_config_reg.qdr_lock), sizeof(uint32) * 0x002c, sizeof(uint32) * 0x0000, 16, 16);

  /* Msb Mirror Data Bus */
  Soc_petra_a_regs.eci.msb_mirror_data_bus_reg.addr.base = sizeof(uint32) * 0x002d;
  Soc_petra_a_regs.eci.msb_mirror_data_bus_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.msb_mirror_data_bus_reg.msb_mdata_bus), sizeof(uint32) * 0x002d, sizeof(uint32) * 0x0000, 17, 0);

  /* Lsb Mirror Data Bus */
  Soc_petra_a_regs.eci.lsb_mirror_data_bus_reg.addr.base = sizeof(uint32) * 0x002e;
  Soc_petra_a_regs.eci.lsb_mirror_data_bus_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.lsb_mirror_data_bus_reg.lsb_mdata_bus), sizeof(uint32) * 0x002e, sizeof(uint32) * 0x0000, 17, 0);

  /* Udr Reg0 */
  Soc_petra_a_regs.eci.udr_reg0_reg.addr.base = sizeof(uint32) * 0x002f;
  Soc_petra_a_regs.eci.udr_reg0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.udr_reg0_reg.udr_reg0), sizeof(uint32) * 0x002f, sizeof(uint32) * 0x0000, 31, 0);

  /* Udr Reg1 */
  Soc_petra_a_regs.eci.udr_reg1_reg.addr.base = sizeof(uint32) * 0x0030;
  Soc_petra_a_regs.eci.udr_reg1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.udr_reg1_reg.udr_reg1), sizeof(uint32) * 0x0030, sizeof(uint32) * 0x0000, 31, 0);

  /* Mdio Cfg */
  Soc_petra_a_regs.eci.mdio_cfg_reg.addr.base = sizeof(uint32) * 0x0080;
  Soc_petra_a_regs.eci.mdio_cfg_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_cfg_reg.cfg_tick_cnt), sizeof(uint32) * 0x0080, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_cfg_reg.cfg_ta_length), sizeof(uint32) * 0x0080, sizeof(uint32) * 0x0000, 14, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_cfg_reg.cfg_init), sizeof(uint32) * 0x0080, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_cfg_reg.cfg_pd), sizeof(uint32) * 0x0080, sizeof(uint32) * 0x0000, 17, 17);

  /* Mdio Op */
  Soc_petra_a_regs.eci.mdio_op_reg.addr.base = sizeof(uint32) * 0x0081;
  Soc_petra_a_regs.eci.mdio_op_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_op_reg.op_data), sizeof(uint32) * 0x0081, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_op_reg.op_ta), sizeof(uint32) * 0x0081, sizeof(uint32) * 0x0000, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_op_reg.op_regad), sizeof(uint32) * 0x0081, sizeof(uint32) * 0x0000, 22, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_op_reg.op_phyad), sizeof(uint32) * 0x0081, sizeof(uint32) * 0x0000, 27, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_op_reg.op_op), sizeof(uint32) * 0x0081, sizeof(uint32) * 0x0000, 29, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_op_reg.op_st), sizeof(uint32) * 0x0081, sizeof(uint32) * 0x0000, 31, 30);

  /* Mdio Stat */
  Soc_petra_a_regs.eci.mdio_stat_reg.addr.base = sizeof(uint32) * 0x0082;
  Soc_petra_a_regs.eci.mdio_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_data), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_ta), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_regad), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 22, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_phyad), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 27, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_rd_en), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_type), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mdio_stat_reg.stat_valid), sizeof(uint32) * 0x0082, sizeof(uint32) * 0x0000, 31, 31);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x0090,
    0x0000,
    &(Soc_petra_a_regs.eci.pcmi_config_reg),
    &(Soc_petra_a_regs.eci.pcmi_results_reg)
  );

  /* Interrupt Block Mask Register */
  Soc_petra_a_regs.eci.interrupt_block_mask_reg.addr.base = sizeof(uint32) * 0x0100;
  Soc_petra_a_regs.eci.interrupt_block_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.ips_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.iqm_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.qdr_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.ipt_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.mmu_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.dprca_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.dprcb_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.dprcc_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.dprcd_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.dprce_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.dprcf_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.ire_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.ihp_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.idr_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.irr_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.fdt_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.fdr_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.fct_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.fcr_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.rtp_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.egq_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.epni_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.maca_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.macb_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.macc_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.sch_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.olp_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.cfc_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.msw_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.pdmca_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_reg.pdmcb_int_mask), sizeof(uint32) * 0x0100, sizeof(uint32) * 0x0000, 30, 30);

  /* Interrupt Block Mask Register Cont */
  Soc_petra_a_regs.eci.interrupt_block_mask_register_cont_reg.addr.base = sizeof(uint32) * 0x0101;
  Soc_petra_a_regs.eci.interrupt_block_mask_register_cont_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_register_cont_reg.nifa_int_mask), sizeof(uint32) * 0x0101, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_register_cont_reg.nifb_int_mask), sizeof(uint32) * 0x0101, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_register_cont_reg.mdio_int_mask), sizeof(uint32) * 0x0101, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_mask_register_cont_reg.eci_int_mask), sizeof(uint32) * 0x0101, sizeof(uint32) * 0x0000, 3, 3);

  /* Interrupt Block Source Register */
  Soc_petra_a_regs.eci.interrupt_block_source_reg.addr.base = sizeof(uint32) * 0x0104;
  Soc_petra_a_regs.eci.interrupt_block_source_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.ips_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.iqm_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.qdr_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.ipt_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.mmu_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.dprca_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.dprcb_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.dprcc_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.dprcd_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.dprce_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.dprcf_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.ire_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.ihp_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.idr_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.irr_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.fdt_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.fdr_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.fct_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.fcr_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.rtp_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.egq_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.epni_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.maca_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.macb_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.macc_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.sch_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.olp_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.cfc_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.msw_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.pdmca_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_reg.pdmcb_int_reg), sizeof(uint32) * 0x0104, sizeof(uint32) * 0x0000, 30, 30);

  /* Interrupt Block Source Register Cont */
  Soc_petra_a_regs.eci.interrupt_block_source_register_cont_reg.addr.base = sizeof(uint32) * 0x0105;
  Soc_petra_a_regs.eci.interrupt_block_source_register_cont_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_register_cont_reg.nifa_int_reg), sizeof(uint32) * 0x0105, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_register_cont_reg.nifb_int_reg), sizeof(uint32) * 0x0105, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_register_cont_reg.mdio_int_reg), sizeof(uint32) * 0x0105, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.interrupt_block_source_register_cont_reg.eci_int_reg), sizeof(uint32) * 0x0105, sizeof(uint32) * 0x0000, 3, 3);

  /* Mask All Interrupts */
  Soc_petra_a_regs.eci.mask_all_interrupts_reg.addr.base = sizeof(uint32) * 0x0108;
  Soc_petra_a_regs.eci.mask_all_interrupts_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.mask_all_interrupts_reg.all_int_mask), sizeof(uint32) * 0x0108, sizeof(uint32) * 0x0000, 0, 0);

  /* Override Interrupt */
  Soc_petra_a_regs.eci.override_interrupt_reg.addr.base = sizeof(uint32) * 0x0109;
  Soc_petra_a_regs.eci.override_interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.override_interrupt_reg.override_interrupt), sizeof(uint32) * 0x0109, sizeof(uint32) * 0x0000, 0, 0);

  /* Fat Pipe Conf     */
  Soc_petra_a_regs.eci.fatp_conf_reg.addr.base = sizeof(uint32) * 0x0198;
  Soc_petra_a_regs.eci.fatp_conf_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.fatp_conf_reg.fatp_conf), sizeof(uint32) * 0x0198, sizeof(uint32) * 0x0000, 31, 0);

  /* Fat Pipe Enable   */
  Soc_petra_a_regs.eci.fatp_enable_reg.addr.base = sizeof(uint32) * 0x01ab;
  Soc_petra_a_regs.eci.fatp_enable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.eci.fatp_enable_reg.fatp_enable), sizeof(uint32) * 0x01ab, sizeof(uint32) * 0x0000, 4, 4);
}

/* Block registers initialization: OLP */
STATIC void
  soc_pa_regs_init_OLP(void)
{
  Soc_petra_a_regs.olp.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_OLP;
  Soc_petra_a_regs.olp.addr.base = sizeof(uint32) * 0x4400;
  Soc_petra_a_regs.olp.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.olp.interrupt_reg.addr.base = sizeof(uint32) * 0x4400;
  Soc_petra_a_regs.olp.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_reg.end_read_err), sizeof(uint32) * 0x4400, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_reg.end_write_err), sizeof(uint32) * 0x4400, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_reg.start_err), sizeof(uint32) * 0x4400, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_reg.end_read_exp_err), sizeof(uint32) * 0x4400, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_reg.end_write_exp_err), sizeof(uint32) * 0x4400, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_reg.start_exp_err), sizeof(uint32) * 0x4400, sizeof(uint32) * 0x0000, 5, 5);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.olp.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x4410;
  Soc_petra_a_regs.olp.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_mask_reg.end_read_err_mask), sizeof(uint32) * 0x4410, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_mask_reg.end_write_err_mask), sizeof(uint32) * 0x4410, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_mask_reg.start_err_mask), sizeof(uint32) * 0x4410, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_mask_reg.end_read_exp_err_mask), sizeof(uint32) * 0x4410, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_mask_reg.end_write_exp_err_mask), sizeof(uint32) * 0x4410, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.interrupt_mask_reg.start_exp_err_mask), sizeof(uint32) * 0x4410, sizeof(uint32) * 0x0000, 5, 5);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.olp.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x4420;
  Soc_petra_a_regs.olp.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x4420, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.olp.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x4430;
  Soc_petra_a_regs.olp.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x4430, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.olp.indirect_command_reg.addr.base = sizeof(uint32) * 0x4440;
  Soc_petra_a_regs.olp.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x4440, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x4440, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x4440, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x4440, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x4440, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.olp.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x4441;
  Soc_petra_a_regs.olp.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x4441, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x4441, sizeof(uint32) * 0x0000, 31, 31);

  /* Scratch Pad */
  Soc_petra_a_regs.olp.scratch_pad_reg.addr.base = sizeof(uint32) * 0x4452;
  Soc_petra_a_regs.olp.scratch_pad_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.scratch_pad_reg.scratch_pad), sizeof(uint32) * 0x4452, sizeof(uint32) * 0x0000, 31, 16);

  /* Incremental Mode Configurations */
  Soc_petra_a_regs.olp.incremental_mode_configurations_reg.addr.base = sizeof(uint32) * 0x4461;
  Soc_petra_a_regs.olp.incremental_mode_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.incremental_mode_configurations_reg.inc_val), sizeof(uint32) * 0x4461, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.incremental_mode_configurations_reg.inc_offset), sizeof(uint32) * 0x4461, sizeof(uint32) * 0x0000, 28, 16);

  /* Packet Configuration Register */
  Soc_petra_a_regs.olp.packet_configuration_reg.addr.base = sizeof(uint32) * 0x4462;
  Soc_petra_a_regs.olp.packet_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.packet_configuration_reg.pckt_length), sizeof(uint32) * 0x4462, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.packet_configuration_reg.pckt_length_mode), sizeof(uint32) * 0x4462, sizeof(uint32) * 0x0000, 14, 14);

  /* Packet Counter Configuration */
  Soc_petra_a_regs.olp.packet_counter_configuration_reg.addr.base = sizeof(uint32) * 0x4463;
  Soc_petra_a_regs.olp.packet_counter_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.packet_counter_configuration_reg.pckt_cnt), sizeof(uint32) * 0x4463, sizeof(uint32) * 0x0000, 31, 0);

  /* Packet Delay Configuration */
  Soc_petra_a_regs.olp.packet_delay_configuration_reg.addr.base = sizeof(uint32) * 0x4464;
  Soc_petra_a_regs.olp.packet_delay_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.packet_delay_configuration_reg.pckt_delay), sizeof(uint32) * 0x4464, sizeof(uint32) * 0x0000, 31, 0);

  /* Packet Length Incremental Value */
  Soc_petra_a_regs.olp.packet_length_incremental_value_reg.addr.base = sizeof(uint32) * 0x4465;
  Soc_petra_a_regs.olp.packet_length_incremental_value_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.olp.packet_length_incremental_value_reg.pckt_length_inc_val), sizeof(uint32) * 0x4465, sizeof(uint32) * 0x0000, 31, 0);
}

/* Block registers initialization: NIF 0X14000,0X15000 */
STATIC void
  soc_pa_regs_init_NIF(void)
{
  Soc_petra_a_regs.nif.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_NIF;
  Soc_petra_a_regs.nif.addr.base = sizeof(uint32) * 0x5000;
  Soc_petra_a_regs.nif.addr.step = sizeof(uint32) * 0x0400;

  /* Interrupt Register */
  Soc_petra_a_regs.nif.interrupt_reg.addr.base = sizeof(uint32) * 0x5000;
  Soc_petra_a_regs.nif.interrupt_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_reg.mal_interrupt[0]), sizeof(uint32) * 0x5000, sizeof(uint32) * 0x0400, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_reg.mal_interrupt[1]), sizeof(uint32) * 0x5000, sizeof(uint32) * 0x0400, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_reg.mal_interrupt[2]), sizeof(uint32) * 0x5000, sizeof(uint32) * 0x0400, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_reg.mal_interrupt[3]), sizeof(uint32) * 0x5000, sizeof(uint32) * 0x0400, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_reg.srd_interrupt), sizeof(uint32) * 0x5000, sizeof(uint32) * 0x0400, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_reg.rx_interrupt), sizeof(uint32) * 0x5000, sizeof(uint32) * 0x0400, 5, 5);

  /* Srd Interrupt Register */
  Soc_petra_a_regs.nif.srd_interrupt_reg.addr.base = sizeof(uint32) * 0x5001;
  Soc_petra_a_regs.nif.srd_interrupt_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_interrupt_reg.srd_lane_interrupt), sizeof(uint32) * 0x5001, sizeof(uint32) * 0x0400, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_interrupt_reg.srd_ipu_interrupt), sizeof(uint32) * 0x5001, sizeof(uint32) * 0x0400, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_interrupt_reg.srd_epb_interrupt), sizeof(uint32) * 0x5001, sizeof(uint32) * 0x0400, 17, 17);

  /* Rx Interrupt Register */
  Soc_petra_a_regs.nif.rx_interrupt_reg.addr.base = sizeof(uint32) * 0x5002;
  Soc_petra_a_regs.nif.rx_interrupt_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.rx_interrupt_reg.rx_paeb_interrupt), sizeof(uint32) * 0x5002, sizeof(uint32) * 0x0400, 21, 0);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.nif.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x5010;
  Soc_petra_a_regs.nif.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_mask_reg.mal_interrupt_mask[0]), sizeof(uint32) * 0x5010, sizeof(uint32) * 0x0400, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_mask_reg.mal_interrupt_mask[1]), sizeof(uint32) * 0x5010, sizeof(uint32) * 0x0400, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_mask_reg.mal_interrupt_mask[2]), sizeof(uint32) * 0x5010, sizeof(uint32) * 0x0400, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_mask_reg.mal_interrupt_mask[3]), sizeof(uint32) * 0x5010, sizeof(uint32) * 0x0400, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.interrupt_mask_reg.srd_interrupt_mask), sizeof(uint32) * 0x5010, sizeof(uint32) * 0x0400, 4, 4);

  /* Srd Interrupt Mask Register */
  Soc_petra_a_regs.nif.srd_interrupt_mask_reg.addr.base = sizeof(uint32) * 0x5011;
  Soc_petra_a_regs.nif.srd_interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_interrupt_mask_reg.srd_lane_interrupt_mask), sizeof(uint32) * 0x5011, sizeof(uint32) * 0x0400, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_interrupt_mask_reg.srd_ipu_interrupt_mask), sizeof(uint32) * 0x5011, sizeof(uint32) * 0x0400, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_interrupt_mask_reg.srd_epb_interrupt_mask), sizeof(uint32) * 0x5011, sizeof(uint32) * 0x0400, 17, 17);

  /* Rx Interrupt Mask Register */
  Soc_petra_a_regs.nif.rx_interrupt_mask_reg.addr.base = sizeof(uint32) * 0x5012;
  Soc_petra_a_regs.nif.rx_interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.rx_interrupt_mask_reg.rx_paeb_interrupt_mask), sizeof(uint32) * 0x5012, sizeof(uint32) * 0x0400, 21, 0);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x5040,
    0x0400,
    &(Soc_petra_a_regs.nif.pcmi_config_reg),
    &(Soc_petra_a_regs.nif.pcmi_results_reg)
  );
  soc_petra_regs_block_pcmi_registers_init(
    0x5042,
    0x0400,
    &(Soc_petra_a_regs.nif.pcmi_config_reg1),
    &(Soc_petra_a_regs.nif.pcmi_results_reg1)
  );

  /* NIF_Status */
  Soc_petra_a_regs.nif.nif_status_reg.addr.base = sizeof(uint32) * 0x5060;
  Soc_petra_a_regs.nif.nif_status_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_status_reg.lane_status), sizeof(uint32) * 0x5060, sizeof(uint32) * 0x0400, 15, 0);

  /* NIF_Config */
  Soc_petra_a_regs.nif.nif_config_reg.addr.base = sizeof(uint32) * 0x5061;
  Soc_petra_a_regs.nif.nif_config_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_config_reg.serdes_g4_en), sizeof(uint32) * 0x5061, sizeof(uint32) * 0x0400, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_config_reg.config_err), sizeof(uint32) * 0x5061, sizeof(uint32) * 0x0400, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_config_reg.pdmc_en), sizeof(uint32) * 0x5061, sizeof(uint32) * 0x0400, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_config_reg.pdmc_pd), sizeof(uint32) * 0x5061, sizeof(uint32) * 0x0400, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_config_reg.pdmc_loopback_en), sizeof(uint32) * 0x5061, sizeof(uint32) * 0x0400, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_config_reg.pdmc_ext_loopback_en), sizeof(uint32) * 0x5061, sizeof(uint32) * 0x0400, 11, 11);

  /* NIF_DFT */
  Soc_petra_a_regs.nif.nif_dft_reg.addr.base = sizeof(uint32) * 0x5062;
  Soc_petra_a_regs.nif.nif_dft_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_dft_reg.init_cfc_calendar), sizeof(uint32) * 0x5062, sizeof(uint32) * 0x0400, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_dft_reg.init_tx_splitter), sizeof(uint32) * 0x5062, sizeof(uint32) * 0x0400, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_dft_reg.init_rx_interleaver), sizeof(uint32) * 0x5062, sizeof(uint32) * 0x0400, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_dft_reg.init_mal_hard_reset), sizeof(uint32) * 0x5062, sizeof(uint32) * 0x0400, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_dft_reg.init_mal_soft_reset), sizeof(uint32) * 0x5062, sizeof(uint32) * 0x0400, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_dft_reg.init_pdmc_hard_reset), sizeof(uint32) * 0x5062, sizeof(uint32) * 0x0400, 16, 16);

  /* NIF_BOM_DFT */
  Soc_petra_a_regs.nif.nif_bom_dft_reg.addr.base = sizeof(uint32) * 0x5063;
  Soc_petra_a_regs.nif.nif_bom_dft_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_paeb_rd_ptr_sel), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_a_paeb_rd_ptr_valid), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_b_paeb_rd_ptr_valid), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_c_paeb_rd_ptr_valid), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_d_paeb_rd_ptr_valid), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_a_paeb_rd_ptr_ind), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_b_paeb_rd_ptr_ind), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_c_paeb_rd_ptr_ind), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 27, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.nif_bom_dft_reg.rx_mal_d_paeb_rd_ptr_ind), sizeof(uint32) * 0x5063, sizeof(uint32) * 0x0400, 31, 28);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x5040,
    0x0400,
    &(Soc_petra_a_regs.nif.pcmi_config_reg),
    &(Soc_petra_a_regs.nif.pcmi_results_reg)
  );
  soc_petra_regs_block_pcmi_registers_init(
    0x5042,
    0x0400,
    &(Soc_petra_a_regs.nif.pcmi_config_reg1),
    &(Soc_petra_a_regs.nif.pcmi_results_reg1)
  );

  /* BIST_Tx CFG */
  Soc_petra_a_regs.nif.bist_tx_cfg_reg.addr.base = sizeof(uint32) * 0x5080;
  Soc_petra_a_regs.nif.bist_tx_cfg_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_cfg_reg.bist_tx_en), sizeof(uint32) * 0x5080, sizeof(uint32) * 0x0400, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_cfg_reg.bist_tx_flow_continuity), sizeof(uint32) * 0x5080, sizeof(uint32) * 0x0400, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_cfg_reg.bist_tx_port), sizeof(uint32) * 0x5080, sizeof(uint32) * 0x0400, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_cfg_reg.bist_tx_number_of_flows), sizeof(uint32) * 0x5080, sizeof(uint32) * 0x0400, 31, 12);

  /* BIST_Tx Pkt Length Seed */
  Soc_petra_a_regs.nif.bist_tx_pkt_length_seed_reg.addr.base = sizeof(uint32) * 0x5081;
  Soc_petra_a_regs.nif.bist_tx_pkt_length_seed_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_pkt_length_seed_reg.bist_tx_pkt_length_seed), sizeof(uint32) * 0x5081, sizeof(uint32) * 0x0400, 13, 0);

  /* BIST_Tx Breakpoint */
  Soc_petra_a_regs.nif.bist_tx_breakpoint_reg.addr.base = sizeof(uint32) * 0x5082;
  Soc_petra_a_regs.nif.bist_tx_breakpoint_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_breakpoint_reg.bist_tx_breakpoint_on_pkt), sizeof(uint32) * 0x5082, sizeof(uint32) * 0x0400, 29, 0);

  /* BIST_Tx Status1 */
  Soc_petra_a_regs.nif.bist_tx_status1_reg.addr.base = sizeof(uint32) * 0x5083;
  Soc_petra_a_regs.nif.bist_tx_status1_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_status1_reg.bist_tx_cnt_pkts), sizeof(uint32) * 0x5083, sizeof(uint32) * 0x0400, 29, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_status1_reg.bist_tx_done), sizeof(uint32) * 0x5083, sizeof(uint32) * 0x0400, 31, 31);

  /* BIST_Tx Status2 */
  Soc_petra_a_regs.nif.bist_tx_status2_reg.addr.base = sizeof(uint32) * 0x5084;
  Soc_petra_a_regs.nif.bist_tx_status2_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_status2_reg.bist_tx_cnt_flows), sizeof(uint32) * 0x5084, sizeof(uint32) * 0x0400, 19, 0);

  /* BIST_Tx Status3 */
  Soc_petra_a_regs.nif.bist_tx_status3_reg.addr.base = sizeof(uint32) * 0x5085;
  Soc_petra_a_regs.nif.bist_tx_status3_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_status3_reg.bist_tx_cnt_err_pkts), sizeof(uint32) * 0x5085, sizeof(uint32) * 0x0400, 15, 0);

  /* BIST_Tx Flow Desc1 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_0.addr.base = sizeof(uint32) * 0x5090;
  Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_0.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_0.flow_desc_data_seed), sizeof(uint32) * 0x5090, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc1 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_1.addr.base = sizeof(uint32) * 0x5091;
  Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_1.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_1.flow_desc_ipg), sizeof(uint32) * 0x5091, sizeof(uint32) * 0x0400, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_1.flow_desc_pkts_in_flow), sizeof(uint32) * 0x5091, sizeof(uint32) * 0x0400, 17, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_1.flow_desc_min_pkt_length), sizeof(uint32) * 0x5091, sizeof(uint32) * 0x0400, 31, 18);

  /* BIST_Tx Flow Desc1 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.addr.base = sizeof(uint32) * 0x5092;
  Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_pkt_length_mask), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_pt), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 16, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_llfc), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_insert_error_inc), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 22, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_insert_error), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_data_type), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 25, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_port), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 27, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc1_reg_2.flow_desc_valid), sizeof(uint32) * 0x5092, sizeof(uint32) * 0x0400, 28, 28);

  /* BIST_Tx Flow Desc2 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_0.addr.base = sizeof(uint32) * 0x5094;
  Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_0.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_0.bist_tx_flow_desc2), sizeof(uint32) * 0x5094, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc2 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_1.addr.base = sizeof(uint32) * 0x5095;
  Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_1.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_1.bist_tx_flow_desc2), sizeof(uint32) * 0x5095, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc2 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_2.addr.base = sizeof(uint32) * 0x5096;
  Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_2.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc2_reg_2.bist_tx_flow_desc2), sizeof(uint32) * 0x5096, sizeof(uint32) * 0x0400, 28, 0);

  /* BIST_Tx Flow Desc3 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_0.addr.base = sizeof(uint32) * 0x5098;
  Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_0.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_0.bist_tx_flow_desc3), sizeof(uint32) * 0x5098, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc3 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_1.addr.base = sizeof(uint32) * 0x5099;
  Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_1.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_1.bist_tx_flow_desc3), sizeof(uint32) * 0x5099, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc3 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_2.addr.base = sizeof(uint32) * 0x509a;
  Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_2.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc3_reg_2.bist_tx_flow_desc3), sizeof(uint32) * 0x509a, sizeof(uint32) * 0x0400, 28, 0);

  /* BIST_Tx Flow Desc4 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_0.addr.base = sizeof(uint32) * 0x509c;
  Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_0.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_0.bist_tx_flow_desc4), sizeof(uint32) * 0x509c, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc4 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_1.addr.base = sizeof(uint32) * 0x509d;
  Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_1.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_1.bist_tx_flow_desc4), sizeof(uint32) * 0x509d, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Tx Flow Desc4 */
  Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_2.addr.base = sizeof(uint32) * 0x509e;
  Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_2.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_tx_flow_desc4_reg_2.bist_tx_flow_desc4), sizeof(uint32) * 0x509e, sizeof(uint32) * 0x0400, 28, 0);

  /* BIST_Rx CFG */
  Soc_petra_a_regs.nif.bist_rx_cfg_reg.addr.base = sizeof(uint32) * 0x50a0;
  Soc_petra_a_regs.nif.bist_rx_cfg_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cfg_reg.bist_rx_en), sizeof(uint32) * 0x50a0, sizeof(uint32) * 0x0400, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cfg_reg.bist_rx_flow_continuity), sizeof(uint32) * 0x50a0, sizeof(uint32) * 0x0400, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cfg_reg.bist_rx_port), sizeof(uint32) * 0x50a0, sizeof(uint32) * 0x0400, 7, 4);

  /* BIST_Rx Status1 */
  Soc_petra_a_regs.nif.bist_rx_status1_reg.addr.base = sizeof(uint32) * 0x50a1;
  Soc_petra_a_regs.nif.bist_rx_status1_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_status1_reg.bist_rx_cnt_pkts), sizeof(uint32) * 0x50a1, sizeof(uint32) * 0x0400, 29, 0);

  /* BIST_Rx Status2 */
  Soc_petra_a_regs.nif.bist_rx_status2_reg.addr.base = sizeof(uint32) * 0x50a2;
  Soc_petra_a_regs.nif.bist_rx_status2_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_status2_reg.bist_rx_cnt_flows), sizeof(uint32) * 0x50a2, sizeof(uint32) * 0x0400, 19, 0);

  /* BIST_Rx Err Cnt1 */
  Soc_petra_a_regs.nif.bist_rx_err_cnt1_reg.addr.base = sizeof(uint32) * 0x50a3;
  Soc_petra_a_regs.nif.bist_rx_err_cnt1_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt1_reg.bist_rx_cnt_err_data), sizeof(uint32) * 0x50a3, sizeof(uint32) * 0x0400, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt1_reg.bist_rx_cnt_err_length), sizeof(uint32) * 0x50a3, sizeof(uint32) * 0x0400, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt1_reg.bist_rx_cnt_err_index), sizeof(uint32) * 0x50a3, sizeof(uint32) * 0x0400, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt1_reg.bist_rx_err_status), sizeof(uint32) * 0x50a3, sizeof(uint32) * 0x0400, 28, 24);

  /* BIST_Rx Err Cnt2 */
  Soc_petra_a_regs.nif.bist_rx_err_cnt2_reg.addr.base = sizeof(uint32) * 0x50a4;
  Soc_petra_a_regs.nif.bist_rx_err_cnt2_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt2_reg.bist_rx_cnt_err_crc_miss), sizeof(uint32) * 0x50a4, sizeof(uint32) * 0x0400, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt2_reg.bist_rx_cnt_err_mac), sizeof(uint32) * 0x50a4, sizeof(uint32) * 0x0400, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_err_cnt2_reg.bist_rx_cnt_crc_ok), sizeof(uint32) * 0x50a4, sizeof(uint32) * 0x0400, 31, 16);

  /* BIST_Rx Cnt Event Sel */
  Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.addr.base = sizeof(uint32) * 0x50a5;
  Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_mac_err), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_crc_miss_err), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_pkt_length_err), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_pkt_data_err), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_pkt_index_err), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_pkt_eop), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_good_pkts), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_sel_reg.bist_rx_cnt_event_pkt_chk_ok), sizeof(uint32) * 0x50a5, sizeof(uint32) * 0x0400, 7, 7);

  /* BIST_Rx Cnt Event */
  Soc_petra_a_regs.nif.bist_rx_cnt_event_reg.addr.base = sizeof(uint32) * 0x50a6;
  Soc_petra_a_regs.nif.bist_rx_cnt_event_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_cnt_event_reg.bist_rx_cnt_event), sizeof(uint32) * 0x50a6, sizeof(uint32) * 0x0400, 29, 0);

  /* BIST_Rx Snapshot1 */
  Soc_petra_a_regs.nif.bist_rx_snapshot1_reg.addr.base = sizeof(uint32) * 0x50a7;
  Soc_petra_a_regs.nif.bist_rx_snapshot1_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_reg.bist_rx_snapshot1_limit), sizeof(uint32) * 0x50a7, sizeof(uint32) * 0x0400, 29, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_reg.bist_rx_snapshot1_type), sizeof(uint32) * 0x50a7, sizeof(uint32) * 0x0400, 31, 30);

  /* BIST_Rx Snapshot1 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_0.addr.base = sizeof(uint32) * 0x50a8;
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_0.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_0.bist_rx_snapshot_pkt_desc), sizeof(uint32) * 0x50a8, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Rx Snapshot1 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_1.addr.base = sizeof(uint32) * 0x50a9;
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_1.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_1.bist_rx_snapshot_pkt_desc), sizeof(uint32) * 0x50a9, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Rx Snapshot1 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_2.addr.base = sizeof(uint32) * 0x50aa;
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_2.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_2.bist_rx_snapshot_pkt_index), sizeof(uint32) * 0x50aa, sizeof(uint32) * 0x0400, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_2.bist_rx_snapshot_flow_index), sizeof(uint32) * 0x50aa, sizeof(uint32) * 0x0400, 31, 24);

  /* BIST_Rx Snapshot1 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_3.addr.base = sizeof(uint32) * 0x50ab;
  Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_3.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_3.bist_rx_snapshot_flow_index), sizeof(uint32) * 0x50ab, sizeof(uint32) * 0x0400, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_3.bist_rx_snapshot_valid), sizeof(uint32) * 0x50ab, sizeof(uint32) * 0x0400, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_3.bist_rx_snapshot_pkt_err), sizeof(uint32) * 0x50ab, sizeof(uint32) * 0x0400, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot1_data_reg_3.bist_rx_snapshot_err_sticky), sizeof(uint32) * 0x50ab, sizeof(uint32) * 0x0400, 28, 24);

  /* BIST_Rx Snapshot2 */
  Soc_petra_a_regs.nif.bist_rx_snapshot2_reg.addr.base = sizeof(uint32) * 0x50ac;
  Soc_petra_a_regs.nif.bist_rx_snapshot2_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot2_reg.bist_rx_snapshot2_limit), sizeof(uint32) * 0x50ac, sizeof(uint32) * 0x0400, 29, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot2_reg.bist_rx_snapshot2_type), sizeof(uint32) * 0x50ac, sizeof(uint32) * 0x0400, 31, 30);

  /* BIST_Rx Snapshot2 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_0.addr.base = sizeof(uint32) * 0x50ad;
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_0.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_0.bist_rx_snapshot2_data), sizeof(uint32) * 0x50ad, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Rx Snapshot2 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_1.addr.base = sizeof(uint32) * 0x50ae;
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_1.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_1.bist_rx_snapshot2_data), sizeof(uint32) * 0x50ae, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Rx Snapshot2 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_2.addr.base = sizeof(uint32) * 0x50af;
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_2.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_2.bist_rx_snapshot2_data), sizeof(uint32) * 0x50af, sizeof(uint32) * 0x0400, 31, 0);

  /* BIST_Rx Snapshot2 Data */
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_3.addr.base = sizeof(uint32) * 0x50b0;
  Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_3.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.bist_rx_snapshot2_data_reg_3.bist_rx_snapshot2_data), sizeof(uint32) * 0x50b0, sizeof(uint32) * 0x0400, 28, 0);

  /* srd0_ln0_cfga */
  Soc_petra_a_regs.nif.srd0_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x5100;
  Soc_petra_a_regs.nif.srd0_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln0_cfga_reg.srd0_ln0_cfga), sizeof(uint32) * 0x5100, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_ln0_stat */
  Soc_petra_a_regs.nif.srd0_ln0_stat_reg.addr.base = sizeof(uint32) * 0x5101;
  Soc_petra_a_regs.nif.srd0_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln0_stat_reg.srd0_ln0_stat), sizeof(uint32) * 0x5101, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_ln0_EBIST */
  Soc_petra_a_regs.nif.srd0_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x5102;
  Soc_petra_a_regs.nif.srd0_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln0_ebist_reg.srd0_ln0_ebist), sizeof(uint32) * 0x5102, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN1_cfga */
  Soc_petra_a_regs.nif.srd0_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x5104;
  Soc_petra_a_regs.nif.srd0_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln1_cfga_reg.srd0_ln1_cfga), sizeof(uint32) * 0x5104, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN1_stat */
  Soc_petra_a_regs.nif.srd0_ln1_stat_reg.addr.base = sizeof(uint32) * 0x5105;
  Soc_petra_a_regs.nif.srd0_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln1_stat_reg.srd0_ln1_stat), sizeof(uint32) * 0x5105, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN1_EBIST */
  Soc_petra_a_regs.nif.srd0_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x5106;
  Soc_petra_a_regs.nif.srd0_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln1_ebist_reg.srd0_ln1_ebist), sizeof(uint32) * 0x5106, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN2_cfga */
  Soc_petra_a_regs.nif.srd0_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x5108;
  Soc_petra_a_regs.nif.srd0_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln2_cfga_reg.srd0_ln2_cfga), sizeof(uint32) * 0x5108, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN2_stat */
  Soc_petra_a_regs.nif.srd0_ln2_stat_reg.addr.base = sizeof(uint32) * 0x5109;
  Soc_petra_a_regs.nif.srd0_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln2_stat_reg.srd0_ln2_stat), sizeof(uint32) * 0x5109, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN2_EBIST */
  Soc_petra_a_regs.nif.srd0_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x510a;
  Soc_petra_a_regs.nif.srd0_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln2_ebist_reg.srd0_ln2_ebist), sizeof(uint32) * 0x510a, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN3_cfga */
  Soc_petra_a_regs.nif.srd0_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x510c;
  Soc_petra_a_regs.nif.srd0_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln3_cfga_reg.srd0_ln3_cfga), sizeof(uint32) * 0x510c, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN3_stat */
  Soc_petra_a_regs.nif.srd0_ln3_stat_reg.addr.base = sizeof(uint32) * 0x510d;
  Soc_petra_a_regs.nif.srd0_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln3_stat_reg.srd0_ln3_stat), sizeof(uint32) * 0x510d, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_LN3_EBIST */
  Soc_petra_a_regs.nif.srd0_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x510e;
  Soc_petra_a_regs.nif.srd0_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_ln3_ebist_reg.srd0_ln3_ebist), sizeof(uint32) * 0x510e, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_cmu_cfga */
  Soc_petra_a_regs.nif.srd0_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x5110;
  Soc_petra_a_regs.nif.srd0_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_cmu_cfga_reg.srd0_cmu_cfga), sizeof(uint32) * 0x5110, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_cmu_cfgb */
  Soc_petra_a_regs.nif.srd0_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x5111;
  Soc_petra_a_regs.nif.srd0_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_cmu_cfgb_reg.srd0_cmu_cfgb), sizeof(uint32) * 0x5111, sizeof(uint32) * 0x0400, 31, 0);

  /* srd0_cmu_stat */
  Soc_petra_a_regs.nif.srd0_cmu_stat_reg.addr.base = sizeof(uint32) * 0x5112;
  Soc_petra_a_regs.nif.srd0_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd0_cmu_stat_reg.srd0_cmu_stat), sizeof(uint32) * 0x5112, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN0_cfga */
  Soc_petra_a_regs.nif.srd1_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x5120;
  Soc_petra_a_regs.nif.srd1_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln0_cfga_reg.srd1_ln0_cfga), sizeof(uint32) * 0x5120, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN0_stat */
  Soc_petra_a_regs.nif.srd1_ln0_stat_reg.addr.base = sizeof(uint32) * 0x5121;
  Soc_petra_a_regs.nif.srd1_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln0_stat_reg.srd1_ln0_stat), sizeof(uint32) * 0x5121, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN0_EBIST */
  Soc_petra_a_regs.nif.srd1_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x5122;
  Soc_petra_a_regs.nif.srd1_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln0_ebist_reg.srd1_ln0_ebist), sizeof(uint32) * 0x5122, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN1_cfga */
  Soc_petra_a_regs.nif.srd1_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x5124;
  Soc_petra_a_regs.nif.srd1_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln1_cfga_reg.srd1_ln1_cfga), sizeof(uint32) * 0x5124, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN1_stat */
  Soc_petra_a_regs.nif.srd1_ln1_stat_reg.addr.base = sizeof(uint32) * 0x5125;
  Soc_petra_a_regs.nif.srd1_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln1_stat_reg.srd1_ln1_stat), sizeof(uint32) * 0x5125, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN1_EBIST */
  Soc_petra_a_regs.nif.srd1_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x5126;
  Soc_petra_a_regs.nif.srd1_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln1_ebist_reg.srd1_ln1_ebist), sizeof(uint32) * 0x5126, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN2_cfga */
  Soc_petra_a_regs.nif.srd1_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x5128;
  Soc_petra_a_regs.nif.srd1_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln2_cfga_reg.srd1_ln2_cfga), sizeof(uint32) * 0x5128, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN2_stat */
  Soc_petra_a_regs.nif.srd1_ln2_stat_reg.addr.base = sizeof(uint32) * 0x5129;
  Soc_petra_a_regs.nif.srd1_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln2_stat_reg.srd1_ln2_stat), sizeof(uint32) * 0x5129, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN2_EBIST */
  Soc_petra_a_regs.nif.srd1_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x512a;
  Soc_petra_a_regs.nif.srd1_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln2_ebist_reg.srd1_ln2_ebist), sizeof(uint32) * 0x512a, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN3_cfga */
  Soc_petra_a_regs.nif.srd1_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x512c;
  Soc_petra_a_regs.nif.srd1_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln3_cfga_reg.srd1_ln3_cfga), sizeof(uint32) * 0x512c, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN3_stat */
  Soc_petra_a_regs.nif.srd1_ln3_stat_reg.addr.base = sizeof(uint32) * 0x512d;
  Soc_petra_a_regs.nif.srd1_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln3_stat_reg.srd1_ln3_stat), sizeof(uint32) * 0x512d, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_LN3_EBIST */
  Soc_petra_a_regs.nif.srd1_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x512e;
  Soc_petra_a_regs.nif.srd1_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_ln3_ebist_reg.srd1_ln3_ebist), sizeof(uint32) * 0x512e, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_cmu_cfga */
  Soc_petra_a_regs.nif.srd1_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x5130;
  Soc_petra_a_regs.nif.srd1_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_cmu_cfga_reg.srd1_cmu_cfga), sizeof(uint32) * 0x5130, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_cmu_cfgb */
  Soc_petra_a_regs.nif.srd1_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x5131;
  Soc_petra_a_regs.nif.srd1_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_cmu_cfgb_reg.srd1_cmu_cfgb), sizeof(uint32) * 0x5131, sizeof(uint32) * 0x0400, 31, 0);

  /* srd1_cmu_stat */
  Soc_petra_a_regs.nif.srd1_cmu_stat_reg.addr.base = sizeof(uint32) * 0x5132;
  Soc_petra_a_regs.nif.srd1_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd1_cmu_stat_reg.srd1_cmu_stat), sizeof(uint32) * 0x5132, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN0_cfga */
  Soc_petra_a_regs.nif.srd2_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x5140;
  Soc_petra_a_regs.nif.srd2_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln0_cfga_reg.srd2_ln0_cfga), sizeof(uint32) * 0x5140, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN0_stat */
  Soc_petra_a_regs.nif.srd2_ln0_stat_reg.addr.base = sizeof(uint32) * 0x5141;
  Soc_petra_a_regs.nif.srd2_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln0_stat_reg.srd2_ln0_stat), sizeof(uint32) * 0x5141, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN0_EBIST */
  Soc_petra_a_regs.nif.srd2_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x5142;
  Soc_petra_a_regs.nif.srd2_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln0_ebist_reg.srd2_ln0_ebist), sizeof(uint32) * 0x5142, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN1_cfga */
  Soc_petra_a_regs.nif.srd2_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x5144;
  Soc_petra_a_regs.nif.srd2_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln1_cfga_reg.srd2_ln1_cfga), sizeof(uint32) * 0x5144, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN1_stat */
  Soc_petra_a_regs.nif.srd2_ln1_stat_reg.addr.base = sizeof(uint32) * 0x5145;
  Soc_petra_a_regs.nif.srd2_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln1_stat_reg.srd2_ln1_stat), sizeof(uint32) * 0x5145, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN1_EBIST */
  Soc_petra_a_regs.nif.srd2_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x5146;
  Soc_petra_a_regs.nif.srd2_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln1_ebist_reg.srd2_ln1_ebist), sizeof(uint32) * 0x5146, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN2_cfga */
  Soc_petra_a_regs.nif.srd2_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x5148;
  Soc_petra_a_regs.nif.srd2_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln2_cfga_reg.srd2_ln2_cfga), sizeof(uint32) * 0x5148, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN2_stat */
  Soc_petra_a_regs.nif.srd2_ln2_stat_reg.addr.base = sizeof(uint32) * 0x5149;
  Soc_petra_a_regs.nif.srd2_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln2_stat_reg.srd2_ln2_stat), sizeof(uint32) * 0x5149, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN2_EBIST */
  Soc_petra_a_regs.nif.srd2_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x514a;
  Soc_petra_a_regs.nif.srd2_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln2_ebist_reg.srd2_ln2_ebist), sizeof(uint32) * 0x514a, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN3_cfga */
  Soc_petra_a_regs.nif.srd2_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x514c;
  Soc_petra_a_regs.nif.srd2_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln3_cfga_reg.srd2_ln3_cfga), sizeof(uint32) * 0x514c, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN3_stat */
  Soc_petra_a_regs.nif.srd2_ln3_stat_reg.addr.base = sizeof(uint32) * 0x514d;
  Soc_petra_a_regs.nif.srd2_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln3_stat_reg.srd2_ln3_stat), sizeof(uint32) * 0x514d, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_LN3_EBIST */
  Soc_petra_a_regs.nif.srd2_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x514e;
  Soc_petra_a_regs.nif.srd2_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_ln3_ebist_reg.srd2_ln3_ebist), sizeof(uint32) * 0x514e, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_cmu_cfga */
  Soc_petra_a_regs.nif.srd2_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x5150;
  Soc_petra_a_regs.nif.srd2_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_cmu_cfga_reg.srd2_cmu_cfga), sizeof(uint32) * 0x5150, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_cmu_cfgb */
  Soc_petra_a_regs.nif.srd2_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x5151;
  Soc_petra_a_regs.nif.srd2_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_cmu_cfgb_reg.srd2_cmu_cfgb), sizeof(uint32) * 0x5151, sizeof(uint32) * 0x0400, 31, 0);

  /* srd2_cmu_stat */
  Soc_petra_a_regs.nif.srd2_cmu_stat_reg.addr.base = sizeof(uint32) * 0x5152;
  Soc_petra_a_regs.nif.srd2_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd2_cmu_stat_reg.srd2_cmu_stat), sizeof(uint32) * 0x5152, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN0_cfga */
  Soc_petra_a_regs.nif.srd3_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x5160;
  Soc_petra_a_regs.nif.srd3_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln0_cfga_reg.srd3_ln0_cfga), sizeof(uint32) * 0x5160, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN0_stat */
  Soc_petra_a_regs.nif.srd3_ln0_stat_reg.addr.base = sizeof(uint32) * 0x5161;
  Soc_petra_a_regs.nif.srd3_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln0_stat_reg.srd3_ln0_stat), sizeof(uint32) * 0x5161, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN0_EBIST */
  Soc_petra_a_regs.nif.srd3_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x5162;
  Soc_petra_a_regs.nif.srd3_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln0_ebist_reg.srd3_ln0_ebist), sizeof(uint32) * 0x5162, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN1_cfga */
  Soc_petra_a_regs.nif.srd3_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x5164;
  Soc_petra_a_regs.nif.srd3_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln1_cfga_reg.srd3_ln1_cfga), sizeof(uint32) * 0x5164, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN1_stat */
  Soc_petra_a_regs.nif.srd3_ln1_stat_reg.addr.base = sizeof(uint32) * 0x5165;
  Soc_petra_a_regs.nif.srd3_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln1_stat_reg.srd3_ln1_stat), sizeof(uint32) * 0x5165, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN1_EBIST */
  Soc_petra_a_regs.nif.srd3_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x5166;
  Soc_petra_a_regs.nif.srd3_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln1_ebist_reg.srd3_ln1_ebist), sizeof(uint32) * 0x5166, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN2_cfga */
  Soc_petra_a_regs.nif.srd3_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x5168;
  Soc_petra_a_regs.nif.srd3_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln2_cfga_reg.srd3_ln2_cfga), sizeof(uint32) * 0x5168, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN2_stat */
  Soc_petra_a_regs.nif.srd3_ln2_stat_reg.addr.base = sizeof(uint32) * 0x5169;
  Soc_petra_a_regs.nif.srd3_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln2_stat_reg.srd3_ln2_stat), sizeof(uint32) * 0x5169, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN2_EBIST */
  Soc_petra_a_regs.nif.srd3_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x516a;
  Soc_petra_a_regs.nif.srd3_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln2_ebist_reg.srd3_ln2_ebist), sizeof(uint32) * 0x516a, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN3_cfga */
  Soc_petra_a_regs.nif.srd3_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x516c;
  Soc_petra_a_regs.nif.srd3_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln3_cfga_reg.srd3_ln3_cfga), sizeof(uint32) * 0x516c, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN3_stat */
  Soc_petra_a_regs.nif.srd3_ln3_stat_reg.addr.base = sizeof(uint32) * 0x516d;
  Soc_petra_a_regs.nif.srd3_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln3_stat_reg.srd3_ln3_stat), sizeof(uint32) * 0x516d, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_LN3_EBIST */
  Soc_petra_a_regs.nif.srd3_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x516e;
  Soc_petra_a_regs.nif.srd3_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_ln3_ebist_reg.srd3_ln3_ebist), sizeof(uint32) * 0x516e, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_cmu_cfga */
  Soc_petra_a_regs.nif.srd3_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x5170;
  Soc_petra_a_regs.nif.srd3_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_cmu_cfga_reg.srd3_cmu_cfga), sizeof(uint32) * 0x5170, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_cmu_cfgb */
  Soc_petra_a_regs.nif.srd3_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x5171;
  Soc_petra_a_regs.nif.srd3_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_cmu_cfgb_reg.srd3_cmu_cfgb), sizeof(uint32) * 0x5171, sizeof(uint32) * 0x0400, 31, 0);

  /* srd3_cmu_stat */
  Soc_petra_a_regs.nif.srd3_cmu_stat_reg.addr.base = sizeof(uint32) * 0x5172;
  Soc_petra_a_regs.nif.srd3_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd3_cmu_stat_reg.srd3_cmu_stat), sizeof(uint32) * 0x5172, sizeof(uint32) * 0x0400, 31, 0);

  /* Srd Ipu Cfg */
  Soc_petra_a_regs.nif.srd_ipu_cfg_reg.addr.base = sizeof(uint32) * 0x5178;
  Soc_petra_a_regs.nif.srd_ipu_cfg_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_ipu_cfg_reg.srd_ipu_cfg), sizeof(uint32) * 0x5178, sizeof(uint32) * 0x0400, 31, 0);

  /* Srd Epb Op */
  Soc_petra_a_regs.nif.srd_epb_op_reg.addr.base = sizeof(uint32) * 0x5179;
  Soc_petra_a_regs.nif.srd_epb_op_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_epb_op_reg.srd_epb_op), sizeof(uint32) * 0x5179, sizeof(uint32) * 0x0400, 31, 0);

  /* Srd Epb Rd */
  Soc_petra_a_regs.nif.srd_epb_rd_reg.addr.base = sizeof(uint32) * 0x517a;
  Soc_petra_a_regs.nif.srd_epb_rd_reg.addr.step = sizeof(uint32) * 0x0400;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif.srd_epb_rd_reg.srd_epb_rd), sizeof(uint32) * 0x517a, sizeof(uint32) * 0x0400, 31, 0);
}

/* Block registers initialization: NIF MAC LANE */
STATIC void
  soc_pa_regs_init_NIF_MAC_LANE(void)
{
  uint32
    fld_idx = 0;

  Soc_petra_a_regs.nif_mac_lane.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_MAL;
  Soc_petra_a_regs.nif_mac_lane.addr.base = sizeof(uint32) * 0x5200;
  Soc_petra_a_regs.nif_mac_lane.addr.step = sizeof(uint32) * 0x0040;

  /* Interrupt Register */
  Soc_petra_a_regs.nif_mac_lane.interrupt_reg.addr.base = sizeof(uint32) * 0x5200;
  Soc_petra_a_regs.nif_mac_lane.interrupt_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.link_status_change_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.rx_pcs_ln_sync_stat_change_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 9, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.rx_pcs_sync_stat_change_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.rx_pcs_local_fault_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.rx_pcs_remote_fault_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.sgmii_got_remote_fault_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 16, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.stat_rx_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.stat_tx_int), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_reg.error_interrupt), sizeof(uint32) * 0x5200, sizeof(uint32) * 0x0040, 19, 19);

  /* Err Interrupt Register */
  Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.addr.base = sizeof(uint32) * 0x5201;
  Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.tx_mlf_overrun_error_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.rx_mlf_overrun_error_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.mac_tx_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.mac_rx_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.tx_taf_overflow_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.rx_raf_overflow_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.tx_mlf_pkt_too_short_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.tx_a48_overflow_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 24, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.rx_pcs_deskew_err_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_reg.rx_pcs_deskew_err_fatal_int), sizeof(uint32) * 0x5201, sizeof(uint32) * 0x0040, 26, 26);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x5210;
  Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.link_status_change_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.rx_pcs_ln_sync_stat_change_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 9, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.rx_pcs_sync_stat_change_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.rx_pcs_local_fault_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.rx_pcs_remote_fault_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.sgmii_got_remote_fault_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 16, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.stat_rx_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.stat_tx_int_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.interrupt_mask_reg.error_interrupt_mask), sizeof(uint32) * 0x5210, sizeof(uint32) * 0x0040, 19, 19);

  /* Err Interrupt Mask Register */
  Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.addr.base = sizeof(uint32) * 0x5211;
  Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.tx_mlf_overrun_error_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.rx_mlf_overrun_error_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.mac_tx_err_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.mac_rx_err_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.tx_taf_overflow_err_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.rx_raf_overflow_err_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.tx_mlf_pkt_too_short_int_err_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.tx_a48_overflow_err_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 24, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.rx_pcs_deskew_err_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.err_interrupt_mask_reg.rx_pcs_deskew_err_fatal_int_mask), sizeof(uint32) * 0x5211, sizeof(uint32) * 0x0040, 26, 26);

  /* MAL Configuration register 1 */
  Soc_petra_a_regs.nif_mac_lane.mal_config1_reg.addr.base = sizeof(uint32) * 0x5212;
  Soc_petra_a_regs.nif_mac_lane.mal_config1_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.mal_config1_reg.cbfc_edge_detector), sizeof(uint32) * 0x5212, sizeof(uint32) * 0x0040, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.mal_config1_reg.llfc_edge_detector), sizeof(uint32) * 0x5212, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.mal_config1_reg.refresh_timer_events), sizeof(uint32) * 0x5212, sizeof(uint32) * 0x0040, 18, 18);

  /* Link Status Register */
  Soc_petra_a_regs.nif_mac_lane.link_status_reg.addr.base = sizeof(uint32) * 0x5213;
  Soc_petra_a_regs.nif_mac_lane.link_status_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.link_status_reg.link_status), sizeof(uint32) * 0x5213, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.link_status_reg.link_status_sticky), sizeof(uint32) * 0x5213, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.link_status_reg.rx_pcs_sync_stat), sizeof(uint32) * 0x5213, sizeof(uint32) * 0x0040, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.link_status_reg.rx_pcs_local_fault), sizeof(uint32) * 0x5213, sizeof(uint32) * 0x0040, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.link_status_reg.rx_pcs_remote_fault), sizeof(uint32) * 0x5213, sizeof(uint32) * 0x0040, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.link_status_reg.rx_pcs_ln_sync_stat), sizeof(uint32) * 0x5213, sizeof(uint32) * 0x0040, 21, 16);

  /* Enable Register */
  Soc_petra_a_regs.nif_mac_lane.enable_reg.addr.base = sizeof(uint32) * 0x5214;
  Soc_petra_a_regs.nif_mac_lane.enable_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.mal_en), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.xaui_rx_lane_en), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.xaui_tx_lane_en), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.sgmii_rx_lane_en), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.sgmii_tx_lane_en), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.tx_flush_egress), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.rx_xaui_pcs_reset_deskew), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.enable_reg.rx_xaui_mac_reset), sizeof(uint32) * 0x5214, sizeof(uint32) * 0x0040, 21, 21);

  /* Tx MLFConfig Status */
  Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.addr.base = sizeof(uint32) * 0x5215;
  Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_almost_full_threshold), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.start_tx_threshold), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_discard_too_short_pkts), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_repeater), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_repeater_port), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 19, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_reset_mlf), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_mlf_almost_full_status), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 27, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_mlfconfig_status_reg.tx_mlf_overrun_error), sizeof(uint32) * 0x5215, sizeof(uint32) * 0x0040, 31, 28);

  /* Rx MLFConfig Status */
  Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.addr.base = sizeof(uint32) * 0x5216;
  Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_almost_full_threshold), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_discard_too_short_pkts), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_ensure_discard_ind), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_ensure_overflow_ind), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_repeater), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_reset_mlf), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_mlf_almost_full_status), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 27, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_mlfconfig_status_reg.rx_mlf_overrun_error), sizeof(uint32) * 0x5216, sizeof(uint32) * 0x0040, 31, 28);

  /* Async Fifo Config */
  Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.addr.base = sizeof(uint32) * 0x5217;
  Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.tx_extra_delay_rd), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.tx_extra_delay_wr), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.rx_extra_delay_rd), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 9, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.rx_extra_delay_wr), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 13, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.taf_af_wm), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.raf_ae_wm), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.async_fifo_config_reg.raf_ensure_overflow_ind), sizeof(uint32) * 0x5217, sizeof(uint32) * 0x0040, 28, 28);

  /* XAUI_Config */
  Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.addr.base = sizeof(uint32) * 0x5218;
  Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.rx_min_pkt_length), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.tx_min_pkt_length), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.ipg_dic_threshold), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.ipg_dic_enable), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.mask_check_end), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.idle_rand_polynom), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.tx_term_pkt_on_err), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.tx_insert_e_on_err), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.lfs_ovrd_send_idle), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.lfs_ovrd_send_q), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_config_reg.lfs_ovrd_en), sizeof(uint32) * 0x5218, sizeof(uint32) * 0x0040, 31, 31);

  /* SPAUI_Extensions */
  Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.addr.base = sizeof(uint32) * 0x5219;
  Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.mal_48b_mode), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_crc_check_en), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_crc_terminate_en), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_crc24_mode), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bct_size), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_preamble_size), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 7, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bct_ch_position), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_preamble_sop), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bct_en_llfc), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bct_en_ch), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bct_en_bt), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bct_en_crc_check), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_bom_deskew_err_en), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_deskew_a_on_even_lanes), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_ensure_fce_ind), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.rx_alt_crc_mode), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_reg.tx_alt_crc_mode), sizeof(uint32) * 0x5219, sizeof(uint32) * 0x0040, 24, 24);

  /* SPAUI_Extensions_Tx */
  Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.addr.base = sizeof(uint32) * 0x521a;
  Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_sop_on_even_only), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_a_on_even_only), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_crc_insert_en), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_crc24_mode), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_size), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_preamble_size), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 7, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ch_position), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_preamble_sop), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_llfc), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_ch), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_bt), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_crc), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_ch_value), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_bt_value), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 26, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_llfc_value), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ovrd_crc_value), sizeof(uint32) * 0x521a, sizeof(uint32) * 0x0040, 31, 28);

  /* SPAUI_Insert Idle Config */
  Soc_petra_a_regs.nif_mac_lane.spaui_insert_idle_config_reg.addr.base = sizeof(uint32) * 0x521b;
  Soc_petra_a_regs.nif_mac_lane.spaui_insert_idle_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_insert_idle_config_reg.insert_idles_count), sizeof(uint32) * 0x521b, sizeof(uint32) * 0x0040, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.spaui_insert_idle_config_reg.insert_idles_mode), sizeof(uint32) * 0x521b, sizeof(uint32) * 0x0040, 29, 28);

  /* BOM */
  Soc_petra_a_regs.nif_mac_lane.bom_reg.addr.base = sizeof(uint32) * 0x521c;
  Soc_petra_a_regs.nif_mac_lane.bom_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.rx_bom_en), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.rx_bom_a_on_lsb), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.rx_bom_a_on_msb), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.srd_dr_mode_ind), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.rx_swap_ll), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.rx_swap_ll_pair), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.tx_swap_ll), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.tx_swap_ll_pair), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.tx_k_mapping_en), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 13, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.tx_a_mapping_en), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 19, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.bom_reg.tx_mapping_value), sizeof(uint32) * 0x521c, sizeof(uint32) * 0x0040, 28, 20);

  /* XAUI_Rx Pcs Deskew Ind */
  Soc_petra_a_regs.nif_mac_lane.xaui_rx_pcs_deskew_ind_reg.addr.base = sizeof(uint32) * 0x521d;
  Soc_petra_a_regs.nif_mac_lane.xaui_rx_pcs_deskew_ind_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_rx_pcs_deskew_ind_reg.rx_pcs_deskew_mux_ind), sizeof(uint32) * 0x521d, sizeof(uint32) * 0x0040, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.xaui_rx_pcs_deskew_ind_reg.rx_bom_align_status), sizeof(uint32) * 0x521d, sizeof(uint32) * 0x0040, 30, 28);

  /* Tx Flow Control Config */
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.addr.base = sizeof(uint32) * 0x5220;
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.tx_llfc_on_mlf_req), sizeof(uint32) * 0x5220, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.tx_llfc_on_cfc_req), sizeof(uint32) * 0x5220, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.xon_on_edge), sizeof(uint32) * 0x5220, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.tx_pause_frame), sizeof(uint32) * 0x5220, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config_reg.tx_bct_llfc), sizeof(uint32) * 0x5220, sizeof(uint32) * 0x0040, 16, 16);

  /* Tx Flow Control Config2 */
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config2_reg.addr.base = sizeof(uint32) * 0x5221;
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config2_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config2_reg.tx_cbfc), sizeof(uint32) * 0x5221, sizeof(uint32) * 0x0040, 3, 0);
  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CLASS_TABLE_L_FLDS; ++fld_idx)
  {
     soc_petra_reg_fld_set(
       &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_config2_reg.tx_class_table_l[fld_idx]),
       sizeof(uint32) * 0x5221,
       sizeof(uint32) * 0x0040,
       (uint8)(6 + ((6-4+1)*fld_idx)),
       (uint8)(4 + ((6-4+1)*fld_idx))
     );
  }

  /* Tx Flow Control Timer Config */
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_timer_config_reg.addr.base = sizeof(uint32) * 0x5222;
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_timer_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_timer_config_reg.fct_timer), sizeof(uint32) * 0x5222, sizeof(uint32) * 0x0040, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_timer_config_reg.fct_timer_en), sizeof(uint32) * 0x5222, sizeof(uint32) * 0x0040, 16, 16);

  /* Tx Pause Quanta */
  Soc_petra_a_regs.nif_mac_lane.tx_pause_quanta_reg.addr.base = sizeof(uint32) * 0x5223;
  Soc_petra_a_regs.nif_mac_lane.tx_pause_quanta_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_pause_quanta_reg.tx_pause_quanta), sizeof(uint32) * 0x5223, sizeof(uint32) * 0x0040, 15, 0);

  /* Tx Pause Frame Source Addr */
  Soc_petra_a_regs.nif_mac_lane.tx_pause_frame_source_addr_reg_0.addr.base = sizeof(uint32) * 0x5224;
  Soc_petra_a_regs.nif_mac_lane.tx_pause_frame_source_addr_reg_0.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_pause_frame_source_addr_reg_0.tx_pause_frame_source_addr), sizeof(uint32) * 0x5224, sizeof(uint32) * 0x0040, 31, 0);

  /* Tx Pause Frame Source Addr */
  Soc_petra_a_regs.nif_mac_lane.tx_pause_frame_source_addr_reg_1.addr.base = sizeof(uint32) * 0x5225;
  Soc_petra_a_regs.nif_mac_lane.tx_pause_frame_source_addr_reg_1.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_pause_frame_source_addr_reg_1.tx_pause_frame_source_addr), sizeof(uint32) * 0x5225, sizeof(uint32) * 0x0040, 15, 0);

  /* Rx Flow Control Config */
  Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.addr.base = sizeof(uint32) * 0x5226;
  Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_cbfc), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 3, 0);
  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CLASS_TABLE_L_FLDS; ++fld_idx)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_class_table_l[fld_idx]),
      sizeof(uint32) * 0x5226,
      sizeof(uint32) * 0x0040,
      (uint8)(6 + ((2-0+1)*fld_idx)),
      (uint8)(4 + ((2-0+1)*fld_idx))
    );
  }

  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.flat_mode), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_terminate_pause), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_terminate_mac_ctrl), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_terminate_fc_burst), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_mask_mac_ctrl), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_mask_fc_burst), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_config_reg.rx_bct_llfc), sizeof(uint32) * 0x5226, sizeof(uint32) * 0x0040, 29, 29);

  /* LLFC_Pause Quanta */
  Soc_petra_a_regs.nif_mac_lane.llfc_pause_quanta_reg.addr.base = sizeof(uint32) * 0x5227;
  Soc_petra_a_regs.nif_mac_lane.llfc_pause_quanta_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.llfc_pause_quanta_reg.llfc_pause_quanta), sizeof(uint32) * 0x5227, sizeof(uint32) * 0x0040, 15, 0);

  /* Tx Flow Control Overrides */
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.addr.base = sizeof(uint32) * 0x5228;
  Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.tx_cfc_fast_llfc_en), sizeof(uint32) * 0x5228, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.tx_cfc_fast_llfc_force), sizeof(uint32) * 0x5228, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.tx_cfc_gen_llfc_en), sizeof(uint32) * 0x5228, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.tx_cfc_gen_llfc_force), sizeof(uint32) * 0x5228, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.tx_cfc_gen_cbfc_en), sizeof(uint32) * 0x5228, sizeof(uint32) * 0x0040, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.tx_flow_control_overrides_reg.tx_cfc_gen_cbfc_force), sizeof(uint32) * 0x5228, sizeof(uint32) * 0x0040, 31, 24);

  /* Rx Flow Control Overrides1 */
  Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.addr.base = sizeof(uint32) * 0x5229;
  Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.rx_start_en), sizeof(uint32) * 0x5229, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.rx_start_force), sizeof(uint32) * 0x5229, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.rx_stop_en), sizeof(uint32) * 0x5229, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.rx_stop_force), sizeof(uint32) * 0x5229, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.rx_pause_force), sizeof(uint32) * 0x5229, sizeof(uint32) * 0x0040, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides1_reg.rx_fc_link_speed_force), sizeof(uint32) * 0x5229, sizeof(uint32) * 0x0040, 20, 20);

  /* Rx Flow Control Overrides2 */
  Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides2_reg.addr.base = sizeof(uint32) * 0x522a;
  Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides2_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides2_reg.cfc_paused_indic_en), sizeof(uint32) * 0x522a, sizeof(uint32) * 0x0040, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.rx_flow_control_overrides2_reg.cfc_paused_indic_force), sizeof(uint32) * 0x522a, sizeof(uint32) * 0x0040, 15, 8);

  /* Statistics */
  Soc_petra_a_regs.nif_mac_lane.statistics_reg.addr.base = sizeof(uint32) * 0x5230;
  Soc_petra_a_regs.nif_mac_lane.statistics_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_rx_init), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_tx_init), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_rx_init_err), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_tx_init_err), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_rx_include_bct_crc_err), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_rx_allow1522), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_reg.stat_rx_max_pkt_length), sizeof(uint32) * 0x5230, sizeof(uint32) * 0x0040, 31, 16);

  /* Statistics Rd Select */
  Soc_petra_a_regs.nif_mac_lane.statistics_rd_select_reg.addr.base = sizeof(uint32) * 0x5231;
  Soc_petra_a_regs.nif_mac_lane.statistics_rd_select_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_rd_select_reg.stat_rd_cnt_sel), sizeof(uint32) * 0x5231, sizeof(uint32) * 0x0040, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_rd_select_reg.stat_rd_port_sel), sizeof(uint32) * 0x5231, sizeof(uint32) * 0x0040, 9, 8);

  /* Statistics Rd LSB */
  Soc_petra_a_regs.nif_mac_lane.statistics_rd_lsb_reg.addr.base = sizeof(uint32) * 0x5232;
  Soc_petra_a_regs.nif_mac_lane.statistics_rd_lsb_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_rd_lsb_reg.stat_rd_cnt_lsb), sizeof(uint32) * 0x5232, sizeof(uint32) * 0x0040, 31, 0);

  /* Statistics Rd MSB */
  Soc_petra_a_regs.nif_mac_lane.statistics_rd_msb_reg.addr.base = sizeof(uint32) * 0x5233;
  Soc_petra_a_regs.nif_mac_lane.statistics_rd_msb_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.statistics_rd_msb_reg.stat_rd_cnt_msb), sizeof(uint32) * 0x5233, sizeof(uint32) * 0x0040, 7, 0);

  /* Stat Rx Discards Ln0 */
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln0_reg.addr.base = sizeof(uint32) * 0x5234;
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln0_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln0_reg.stat_rx_discards_ln0), sizeof(uint32) * 0x5234, sizeof(uint32) * 0x0040, 31, 0);

  /* Stat Rx Discards Ln1 */
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln1_reg.addr.base = sizeof(uint32) * 0x5235;
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln1_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln1_reg.stat_rx_discards_ln1), sizeof(uint32) * 0x5235, sizeof(uint32) * 0x0040, 31, 0);

  /* Stat Rx Discards Ln2 */
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln2_reg.addr.base = sizeof(uint32) * 0x5236;
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln2_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln2_reg.stat_rx_discards_ln2), sizeof(uint32) * 0x5236, sizeof(uint32) * 0x0040, 31, 0);

  /* Stat Rx Discards Ln3 */
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln3_reg.addr.base = sizeof(uint32) * 0x5237;
  Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln3_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.stat_rx_discards_ln3_reg.stat_rx_discards_ln3), sizeof(uint32) * 0x5237, sizeof(uint32) * 0x0040, 31, 0);

  /* SGMII_Config */
  Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.addr.base = sizeof(uint32) * 0x5238;
  Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.sgmii_mode), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.fast_sgmii_mode), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.sgmii_rx_crc_check_en), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.sgmii_c_mode), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.sgmii_rx_min_pkt_length), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.sgmii_tx_min_pkt_length), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config_reg.sgmii_tx_ipg_length), sizeof(uint32) * 0x5238, sizeof(uint32) * 0x0040, 31, 24);

  /* SGMII_Config2 */
  Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.addr.base = sizeof(uint32) * 0x5239;
  Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.prefetch_const), sizeof(uint32) * 0x5239, sizeof(uint32) * 0x0040, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.early_sop_prefetch_const), sizeof(uint32) * 0x5239, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.tx_max_credits), sizeof(uint32) * 0x5239, sizeof(uint32) * 0x0040, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.tx_early_fc_release), sizeof(uint32) * 0x5239, sizeof(uint32) * 0x0040, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.fct_large_delay), sizeof(uint32) * 0x5239, sizeof(uint32) * 0x0040, 10, 10);

  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_SGMII_NOF_LINKS; fld_idx++ )
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.nif_mac_lane.sgmii_config2_reg.an_xmit_ovrd_l[fld_idx]),
      sizeof(uint32) * 0x5239,
      sizeof(uint32) * 0x0040,
      (uint8)(18 + 4*fld_idx),
      (uint8)(16 + 4*fld_idx)
    );
  }

  /* Auto Neg Config */
  Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.addr.base = sizeof(uint32) * 0x523a;
  Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.enable_auto_neg), sizeof(uint32) * 0x523a, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.restart_auto_neg), sizeof(uint32) * 0x523a, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.send_offline), sizeof(uint32) * 0x523a, sizeof(uint32) * 0x0040, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.send_remote_fault), sizeof(uint32) * 0x523a, sizeof(uint32) * 0x0040, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.link_speed_ovrd_en), sizeof(uint32) * 0x523a, sizeof(uint32) * 0x0040, 23, 20);

  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_SGMII_NOF_LINKS; fld_idx++ )
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config_reg.link_speed_l[fld_idx]),
      sizeof(uint32) * 0x523a,
      sizeof(uint32) * 0x0040,
      (uint8)(25 + 2*fld_idx),
      (uint8)(24 + 2*fld_idx)
    );
  }

  /* Auto Neg Config2 */
  Soc_petra_a_regs.nif_mac_lane.auto_neg_config2_reg.addr.base = sizeof(uint32) * 0x523b;
  Soc_petra_a_regs.nif_mac_lane.auto_neg_config2_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config2_reg.num_clocks_in_hundred_us), sizeof(uint32) * 0x523b, sizeof(uint32) * 0x0040, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config2_reg.partner_pause_ovrd_en), sizeof(uint32) * 0x523b, sizeof(uint32) * 0x0040, 27, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_config2_reg.partner_pause_ovrd_value), sizeof(uint32) * 0x523b, sizeof(uint32) * 0x0040, 28, 28);

  /* Auto Neg Status */
  Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.addr.base = sizeof(uint32) * 0x523c;
  Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.addr.step = sizeof(uint32) * 0x0040;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.auto_neg_complete), sizeof(uint32) * 0x523c, sizeof(uint32) * 0x0040, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.auto_neg_error), sizeof(uint32) * 0x523c, sizeof(uint32) * 0x0040, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.partner_pause_rx), sizeof(uint32) * 0x523c, sizeof(uint32) * 0x0040, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.got_remote_fault), sizeof(uint32) * 0x523c, sizeof(uint32) * 0x0040, 23, 20);

  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_SGMII_NOF_LINKS; fld_idx++ )
  {
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.nif_mac_lane.auto_neg_status_reg.remote_fault_type_l[fld_idx]),
    sizeof(uint32) * 0x523c,
    sizeof(uint32) * 0x0040,
    (uint8)(25 + 2*fld_idx),
    (uint8)(24 + 2*fld_idx)
  );
  }
}

/* Block registers initialization: IRE */
STATIC void
  soc_pa_regs_init_IRE(void)
{
  uint32
    reg_idx;

  Soc_petra_a_regs.ire.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IRE;
  Soc_petra_a_regs.ire.addr.base = sizeof(uint32) * 0x2400;
  Soc_petra_a_regs.ire.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.ire.interrupt_reg.addr.base = sizeof(uint32) * 0x2400;
  Soc_petra_a_regs.ire.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.cpu_if_err_data_arrived), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.cpu_if_32bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.cpu_if_packet_size_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.cpu_if_64bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.olp_if_err_data_arrived), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.olp_if_32bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.olp_if_packet_size_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.olp_if_64bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.nifa_err_data_arrived), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.nifa_packet_size_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.nifa_64bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.nifb_err_data_arrived), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.nifb_packet_size_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.nifb_64bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.rcy_err_data_arrived), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.rcy_packet_size_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.rcy_64bytes_pack_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.burst_err_follow_sop_not_valid), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.burst_err_follow_sop_dif_context), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.burst_err_follow_sop_is_sop), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.burst_err_follow_sop_is_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_reg.fap_port_err), sizeof(uint32) * 0x2400, sizeof(uint32) * 0x0000, 21, 21);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.ire.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x2410;
  Soc_petra_a_regs.ire.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.cpu_if_err_data_arrived_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.cpu_if_32bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.cpu_if_packet_size_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.cpu_if_64bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.olp_if_err_data_arrived_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.olp_if_32bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.olp_if_packet_size_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.olp_if_64bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.nifa_err_data_arrived_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.nifa_packet_size_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.nifa_64bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.nifb_err_data_arrived_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.nifb_packet_size_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.nifb_64bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.rcy_err_data_arrived_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.rcy_packet_size_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.rcy_64bytes_pack_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.burst_err_follow_sop_not_valid_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.burst_err_follow_sop_dif_context_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.burst_err_follow_sop_is_sop_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.burst_err_follow_sop_is_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.interrupt_mask_reg.fap_port_err_mask), sizeof(uint32) * 0x2410, sizeof(uint32) * 0x0000, 21, 21);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x2420;
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x2420, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x2421;
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x2421, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x2422;
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x2422, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x2423;
  Soc_petra_a_regs.ire.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x2423, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x2430;
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x2430, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x2431;
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x2431, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x2432;
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x2432, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x2433;
  Soc_petra_a_regs.ire.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x2433, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.ire.indirect_command_reg.addr.base = sizeof(uint32) * 0x2440;
  Soc_petra_a_regs.ire.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x2440, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x2440, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x2440, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x2440, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x2440, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.ire.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x2441;
  Soc_petra_a_regs.ire.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x2441, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x2441, sizeof(uint32) * 0x0000, 31, 31);

  /* Dynamic Configuration */
  Soc_petra_a_regs.ire.dynamic_configuration_reg.addr.base = sizeof(uint32) * 0x2454;
  Soc_petra_a_regs.ire.dynamic_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.dynamic_configuration_reg.enable_data_path), sizeof(uint32) * 0x2454, sizeof(uint32) * 0x0000, 0, 0);

  /* Cpu Fap Port Configuration Register */
  Soc_petra_a_regs.ire.cpu_fap_port_configuration_reg.addr.base = sizeof(uint32) * 0x2455;
  Soc_petra_a_regs.ire.cpu_fap_port_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_fap_port_configuration_reg.cpu_fap_port), sizeof(uint32) * 0x2455, sizeof(uint32) * 0x0000, 6, 0);

  /* Olp Fap Port Configuration Register */
  Soc_petra_a_regs.ire.olp_fap_port_configuration_reg.addr.base = sizeof(uint32) * 0x2456;
  Soc_petra_a_regs.ire.olp_fap_port_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.olp_fap_port_configuration_reg.olp_fap_port), sizeof(uint32) * 0x2456, sizeof(uint32) * 0x0000, 6, 0);

  /* Nif Groups Scheduling Weights */
  Soc_petra_a_regs.ire.nif_groups_scheduling_weights_reg.addr.base = sizeof(uint32) * 0x2457;
  Soc_petra_a_regs.ire.nif_groups_scheduling_weights_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.nif_groups_scheduling_weights_reg.nifa_weight), sizeof(uint32) * 0x2457, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.nif_groups_scheduling_weights_reg.nifb_weight), sizeof(uint32) * 0x2457, sizeof(uint32) * 0x0000, 7, 4);

  /* Recycling Shaper */
  Soc_petra_a_regs.ire.recycling_shaper_reg.addr.base = sizeof(uint32) * 0x2458;
  Soc_petra_a_regs.ire.recycling_shaper_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.recycling_shaper_reg.rcy_shaper_timer_cycles), sizeof(uint32) * 0x2458, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.recycling_shaper_reg.rcy_shaper_grant_size), sizeof(uint32) * 0x2458, sizeof(uint32) * 0x0000, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.recycling_shaper_reg.rcy_shaper_max_burst), sizeof(uint32) * 0x2458, sizeof(uint32) * 0x0000, 29, 16);

  /* Ingress Mac Type Register */
  Soc_petra_a_regs.ire.ingress_mac_type_reg.addr.base = sizeof(uint32) * 0x2459;
  Soc_petra_a_regs.ire.ingress_mac_type_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.ingress_mac_type_reg.ing_mac_type_reg), sizeof(uint32) * 0x2459, sizeof(uint32) * 0x0000, 7, 0);

  /* Word Fifo Thresholds */
  Soc_petra_a_regs.ire.word_fifo_thresholds_reg.addr.base = sizeof(uint32) * 0x245a;
  Soc_petra_a_regs.ire.word_fifo_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.word_fifo_thresholds_reg.threshold_wdf_low), sizeof(uint32) * 0x245a, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.word_fifo_thresholds_reg.threshold_wdf_high), sizeof(uint32) * 0x245a, sizeof(uint32) * 0x0000, 24, 16);

  /* Word Fifo Status */
  Soc_petra_a_regs.ire.word_fifo_status_reg.addr.base = sizeof(uint32) * 0x245b;
  Soc_petra_a_regs.ire.word_fifo_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.word_fifo_status_reg.wdf_empty), sizeof(uint32) * 0x245b, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.word_fifo_status_reg.wdf_full), sizeof(uint32) * 0x245b, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.word_fifo_status_reg.wdf_fifo_count), sizeof(uint32) * 0x245b, sizeof(uint32) * 0x0000, 16, 8);

  /* Ihp Fifo Thresholds */
  Soc_petra_a_regs.ire.ihp_fifo_thresholds_reg.addr.base = sizeof(uint32) * 0x245c;
  Soc_petra_a_regs.ire.ihp_fifo_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.ihp_fifo_thresholds_reg.threshold_hpf_low), sizeof(uint32) * 0x245c, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.ihp_fifo_thresholds_reg.threshold_hpf_high), sizeof(uint32) * 0x245c, sizeof(uint32) * 0x0000, 14, 8);

  /* Ihp Fifo Status */
  Soc_petra_a_regs.ire.ihp_fifo_status_reg.addr.base = sizeof(uint32) * 0x245d;
  Soc_petra_a_regs.ire.ihp_fifo_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.ihp_fifo_status_reg.hpf_empty), sizeof(uint32) * 0x245d, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.ihp_fifo_status_reg.hpf_full), sizeof(uint32) * 0x245d, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.ihp_fifo_status_reg.hpf_fifo_count), sizeof(uint32) * 0x245d, sizeof(uint32) * 0x0000, 14, 8);

  /* Packet Edit Fifo Thresholds */
  Soc_petra_a_regs.ire.packet_edit_fifo_thresholds_reg.addr.base = sizeof(uint32) * 0x245e;
  Soc_petra_a_regs.ire.packet_edit_fifo_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.packet_edit_fifo_thresholds_reg.threshold_pef_low), sizeof(uint32) * 0x245e, sizeof(uint32) * 0x0000, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.packet_edit_fifo_thresholds_reg.threshold_pef_high), sizeof(uint32) * 0x245e, sizeof(uint32) * 0x0000, 13, 8);

  /* Packet Edit Fifo Status */
  Soc_petra_a_regs.ire.packet_edit_fifo_status_reg.addr.base = sizeof(uint32) * 0x245f;
  Soc_petra_a_regs.ire.packet_edit_fifo_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.packet_edit_fifo_status_reg.pef_empty), sizeof(uint32) * 0x245f, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.packet_edit_fifo_status_reg.pef_full), sizeof(uint32) * 0x245f, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.packet_edit_fifo_status_reg.pef_fifo_count), sizeof(uint32) * 0x245f, sizeof(uint32) * 0x0000, 13, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.packet_edit_fifo_status_reg.pef_count), sizeof(uint32) * 0x245f, sizeof(uint32) * 0x0000, 21, 16);

  /* Cpu Interface Select Non Streaming Packet Path */
  Soc_petra_a_regs.ire.cpu_interface_select_non_streaming_packet_path_reg.addr.base = sizeof(uint32) * 0x2460;
  Soc_petra_a_regs.ire.cpu_interface_select_non_streaming_packet_path_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_select_non_streaming_packet_path_reg.cpu_if_sel_non_stream_pkt), sizeof(uint32) * 0x2460, sizeof(uint32) * 0x0000, 0, 0);

  /* Cpu Interface Packet Control */
  Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.addr.base = sizeof(uint32) * 0x2461;
  Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.cpu_if_pkt_send_data), sizeof(uint32) * 0x2461, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.cpu_if_pkt_start), sizeof(uint32) * 0x2461, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.cpu_if_pkt_end), sizeof(uint32) * 0x2461, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.cpu_if_pkt_err), sizeof(uint32) * 0x2461, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_control_reg.cpu_if_pkt_be), sizeof(uint32) * 0x2461, sizeof(uint32) * 0x0000, 8, 4);

  /* Cpu Interface Packet Data */
  for (reg_idx = 0; reg_idx <SOC_PETRA_REGS_IRE_NOF_CPU_PKT_DATA_REGS; ++reg_idx )
  {
    Soc_petra_a_regs.ire.cpu_interface_packet_data_regs[reg_idx].addr.base = sizeof(uint32) * (0x2462+reg_idx);
    Soc_petra_a_regs.ire.cpu_interface_packet_data_regs[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_regs[reg_idx].cpu_if_pkt_data), sizeof(uint32) * (0x2462+reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_0.addr.base = sizeof(uint32) * 0x2462;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_0.cpu_if_pkt_data), sizeof(uint32) * 0x2462, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_1.addr.base = sizeof(uint32) * 0x2463;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_1.cpu_if_pkt_data), sizeof(uint32) * 0x2463, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_2.addr.base = sizeof(uint32) * 0x2464;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_2.cpu_if_pkt_data), sizeof(uint32) * 0x2464, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_3.addr.base = sizeof(uint32) * 0x2465;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_3.cpu_if_pkt_data), sizeof(uint32) * 0x2465, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_4.addr.base = sizeof(uint32) * 0x2466;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_4.cpu_if_pkt_data), sizeof(uint32) * 0x2466, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_5.addr.base = sizeof(uint32) * 0x2467;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_5.cpu_if_pkt_data), sizeof(uint32) * 0x2467, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_6.addr.base = sizeof(uint32) * 0x2468;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_6.cpu_if_pkt_data), sizeof(uint32) * 0x2468, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Interface Packet Data */
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_7.addr.base = sizeof(uint32) * 0x2469;
  Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.cpu_interface_packet_data_reg_7.cpu_if_pkt_data), sizeof(uint32) * 0x2469, sizeof(uint32) * 0x0000, 31, 0);

  /* Invalid Port */
  Soc_petra_a_regs.ire.invalid_port_reg.addr.base = sizeof(uint32) * 0x246a;
  Soc_petra_a_regs.ire.invalid_port_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.invalid_port_reg.port), sizeof(uint32) * 0x246a, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.invalid_port_reg.interfac), sizeof(uint32) * 0x246a, sizeof(uint32) * 0x0000, 13, 12);

  /* Reset Rxi Interfaces */
  Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.addr.base = sizeof(uint32) * 0x246d;
  Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.reset_cpu_rxi), sizeof(uint32) * 0x246d, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.reset_olp_rxi), sizeof(uint32) * 0x246d, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.reset_nifa_rxi), sizeof(uint32) * 0x246d, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.reset_nifb_rxi), sizeof(uint32) * 0x246d, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ire.reset_rxi_interfaces_reg.reset_rcy_rxi), sizeof(uint32) * 0x246d, sizeof(uint32) * 0x0000, 4, 4);

}

/* Block registers initialization: IDR */
STATIC void
  soc_pa_regs_init_IDR(void)
{
  uint32
    fld_idx;

  Soc_petra_a_regs.idr.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IDR;
  Soc_petra_a_regs.idr.addr.base = sizeof(uint32) * 0x2800;
  Soc_petra_a_regs.idr.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.idr.interrupt_reg.addr.base = sizeof(uint32) * 0x2800;
  Soc_petra_a_regs.idr.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_reg.error_unicast_recycle), sizeof(uint32) * 0x2800, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_reg.error_mini_multicast_recycle), sizeof(uint32) * 0x2800, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_reg.error_full_multicast_recycle), sizeof(uint32) * 0x2800, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_reg.error_reassembly), sizeof(uint32) * 0x2800, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_reg.error_timeout), sizeof(uint32) * 0x2800, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_reg.error_ecc), sizeof(uint32) * 0x2800, sizeof(uint32) * 0x0000, 5, 5);

  /* Ecc Interrupt Register */
  Soc_petra_a_regs.idr.ecc_interrupt_reg.addr.base = sizeof(uint32) * 0x2801;
  Soc_petra_a_regs.idr.ecc_interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_chunk_status_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_chunk_fifo_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_unicast_dp_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_mini_multicast_dp_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_full_multicast_dp_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_free_pcbs_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_pcb_link_table_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_prm_dp_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_prm_pd_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_prm_cd_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_reassembly_status_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_word_index_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_buffer_type_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_reg.error_complete_pc_ecc), sizeof(uint32) * 0x2801, sizeof(uint32) * 0x0000, 13, 13);

  /* Interrupt Register Mask */
  Soc_petra_a_regs.idr.interrupt_register_mask_reg.addr.base = sizeof(uint32) * 0x2810;
  Soc_petra_a_regs.idr.interrupt_register_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_register_mask_reg.error_unicast_recycle_mask), sizeof(uint32) * 0x2810, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_register_mask_reg.error_mini_multicast_recycle_mask), sizeof(uint32) * 0x2810, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_register_mask_reg.error_full_multicast_recycle_mask), sizeof(uint32) * 0x2810, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_register_mask_reg.error_reassembly_mask), sizeof(uint32) * 0x2810, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_register_mask_reg.error_timeout_mask), sizeof(uint32) * 0x2810, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.interrupt_register_mask_reg.error_ecc_mask), sizeof(uint32) * 0x2810, sizeof(uint32) * 0x0000, 5, 5);

  /* Ecc Interrupt Mask Register */
  Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.addr.base = sizeof(uint32) * 0x2811;
  Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_chunk_status_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_chunk_fifo_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_unicast_dp_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_mini_multicast_dp_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_full_multicast_dp_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_free_pcbs_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_pcb_link_table_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_prm_dp_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_prm_pd_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_prm_cd_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_reassembly_status_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_word_index_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_buffer_type_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.ecc_interrupt_mask_reg.error_complete_pc_ecc_mask), sizeof(uint32) * 0x2811, sizeof(uint32) * 0x0000, 13, 13);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x2820;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x2820, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x2821;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x2821, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x2822;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x2822, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x2823;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x2823, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x2824;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x2824, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x2825;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x2825, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_6.addr.base = sizeof(uint32) * 0x2826;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_6.indirect_command_wr_data), sizeof(uint32) * 0x2826, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_7.addr.base = sizeof(uint32) * 0x2827;
  Soc_petra_a_regs.idr.indirect_command_wr_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_wr_data_reg_7.indirect_command_wr_data), sizeof(uint32) * 0x2827, sizeof(uint32) * 0x0000, 15, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x2830;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x2830, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x2831;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x2831, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x2832;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x2832, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x2833;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x2833, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x2834;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x2834, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x2835;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x2835, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_6.addr.base = sizeof(uint32) * 0x2836;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_6.indirect_command_rd_data), sizeof(uint32) * 0x2836, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_7.addr.base = sizeof(uint32) * 0x2837;
  Soc_petra_a_regs.idr.indirect_command_rd_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_rd_data_reg_7.indirect_command_rd_data), sizeof(uint32) * 0x2837, sizeof(uint32) * 0x0000, 15, 0);

  /* Indirect Command */
  Soc_petra_a_regs.idr.indirect_command_reg.addr.base = sizeof(uint32) * 0x2840;
  Soc_petra_a_regs.idr.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x2840, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x2840, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x2840, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x2840, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x2840, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.idr.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x2841;
  Soc_petra_a_regs.idr.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x2841, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x2841, sizeof(uint32) * 0x0000, 31, 31);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x2890,
    0x0000,
    &(Soc_petra_a_regs.idr.pcmi_config_reg),
    &(Soc_petra_a_regs.idr.pcmi_results_reg)
  );

  /* Reset Status Register */
  Soc_petra_a_regs.idr.reset_status_reg.addr.base = sizeof(uint32) * 0x2900;
  Soc_petra_a_regs.idr.reset_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reset_status_reg.context_status_init_done), sizeof(uint32) * 0x2900, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reset_status_reg.chunk_status_init_done), sizeof(uint32) * 0x2900, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reset_status_reg.word_index_fifo_init_done), sizeof(uint32) * 0x2900, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reset_status_reg.free_pcb_fifo_init_done), sizeof(uint32) * 0x2900, sizeof(uint32) * 0x0000, 3, 3);

  /* Static Configuration */
  Soc_petra_a_regs.idr.static_configuration_reg.addr.base = sizeof(uint32) * 0x2901;
  Soc_petra_a_regs.idr.static_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.dis_ecc), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.enable_buffer_change), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.reassembly_no_mmc_mirror_only), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.fbc_internal_reuse), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.fbc_unicast_autogen_enable), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.fbc_mini_multicast_autogen_enable), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.static_configuration_reg.fbc_full_multicast_autogen_enable), sizeof(uint32) * 0x2901, sizeof(uint32) * 0x0000, 8, 8);

  /*
   *	CAUTION: the following register has in practice a different appearance (Alignment with Soc_petra-B).
   */
  /* Dynamic Configuration */
  Soc_petra_a_regs.idr.dynamic_configuration_reg.addr.base = sizeof(uint32) * 0x2902;
  Soc_petra_a_regs.idr.dynamic_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg.check_min_size), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg.check_max_size), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg.check_min_org_size), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg.check_max_org_size), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 5, 5);
  Soc_petra_a_regs.idr.dynamic_configuration_reg_cmn.addr.base = sizeof(uint32) * 0x2902;
  Soc_petra_a_regs.idr.dynamic_configuration_reg_cmn.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg_cmn.enable_data_path), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg_cmn.fbc_halt), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.dynamic_configuration_reg_cmn.max_dp_threshold), sizeof(uint32) * 0x2902, sizeof(uint32) * 0x0000, 8, 6);

  /* Bcn Register */
  Soc_petra_a_regs.idr.bcn_reg.addr.base = sizeof(uint32) * 0x2903;
  Soc_petra_a_regs.idr.bcn_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.bcn_reg.bcn_threshold), sizeof(uint32) * 0x2903, sizeof(uint32) * 0x0000, 19, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.bcn_reg.bcn_mode), sizeof(uint32) * 0x2903, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.bcn_reg.bcn_snoop_cmd), sizeof(uint32) * 0x2903, sizeof(uint32) * 0x0000, 27, 24);

  /* Resequencing Timeout Reset */
  Soc_petra_a_regs.idr.resequencing_timeout_reset_reg.addr.base = sizeof(uint32) * 0x2904;
  Soc_petra_a_regs.idr.resequencing_timeout_reset_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.resequencing_timeout_reset_reg.resequencing_skew_mask), sizeof(uint32) * 0x2904, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.resequencing_timeout_reset_reg.resequencing_transfer_mask), sizeof(uint32) * 0x2904, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.resequencing_timeout_reset_reg.resequencing_mmu_fc_mask), sizeof(uint32) * 0x2904, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.resequencing_timeout_reset_reg.resequencing_irr_fc_mask), sizeof(uint32) * 0x2904, sizeof(uint32) * 0x0000, 9, 9);

  /* Packet Sizes */
  Soc_petra_a_regs.idr.packet_sizes_reg.addr.base = sizeof(uint32) * 0x2905;
  Soc_petra_a_regs.idr.packet_sizes_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.packet_sizes_reg.min_packet_size), sizeof(uint32) * 0x2905, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.packet_sizes_reg.max_packet_size), sizeof(uint32) * 0x2905, sizeof(uint32) * 0x0000, 27, 14);

  /* Original Packet Sizes */
  Soc_petra_a_regs.idr.original_packet_sizes_reg.addr.base = sizeof(uint32) * 0x2906;
  Soc_petra_a_regs.idr.original_packet_sizes_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.original_packet_sizes_reg.min_org_packet_size), sizeof(uint32) * 0x2906, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.original_packet_sizes_reg.max_org_packet_size), sizeof(uint32) * 0x2906, sizeof(uint32) * 0x0000, 27, 14);

  /* Rct Cpu Request Register */
  Soc_petra_a_regs.idr.rct_cpu_request_reg.addr.base = sizeof(uint32) * 0x2907;
  Soc_petra_a_regs.idr.rct_cpu_request_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.rct_cpu_request_reg.rct_cpu_request), sizeof(uint32) * 0x2907, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.rct_cpu_request_reg.rct_cpu_request_fap_port), sizeof(uint32) * 0x2907, sizeof(uint32) * 0x0000, 7, 1);

  /* Fbc Full Multicast Autogen Start */
  Soc_petra_a_regs.idr.fbc_full_multicast_autogen_start_reg.addr.base = sizeof(uint32) * 0x2908;
  Soc_petra_a_regs.idr.fbc_full_multicast_autogen_start_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_full_multicast_autogen_start_reg.fbc_full_multicast_autogen_start), sizeof(uint32) * 0x2908, sizeof(uint32) * 0x0000, 20, 0);

  /* Fbc Full Multicast Autogen End */
  Soc_petra_a_regs.idr.fbc_full_multicast_autogen_end_reg.addr.base = sizeof(uint32) * 0x2909;
  Soc_petra_a_regs.idr.fbc_full_multicast_autogen_end_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_full_multicast_autogen_end_reg.fbc_full_multicast_autogen_end), sizeof(uint32) * 0x2909, sizeof(uint32) * 0x0000, 20, 0);

  /* Fbc Mini Multicast Autogen Start */
  Soc_petra_a_regs.idr.fbc_mini_multicast_autogen_start_reg.addr.base = sizeof(uint32) * 0x290a;
  Soc_petra_a_regs.idr.fbc_mini_multicast_autogen_start_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_mini_multicast_autogen_start_reg.fbc_mini_multicast_autogen_start), sizeof(uint32) * 0x290a, sizeof(uint32) * 0x0000, 20, 0);

  /* Fbc Mini Multicast Autogen End */
  Soc_petra_a_regs.idr.fbc_mini_multicast_autogen_end_reg.addr.base = sizeof(uint32) * 0x290b;
  Soc_petra_a_regs.idr.fbc_mini_multicast_autogen_end_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_mini_multicast_autogen_end_reg.fbc_mini_multicast_autogen_end), sizeof(uint32) * 0x290b, sizeof(uint32) * 0x0000, 20, 0);

  /* Fbc Unicast Autogen Start */
  Soc_petra_a_regs.idr.fbc_unicast_autogen_start_reg.addr.base = sizeof(uint32) * 0x290c;
  Soc_petra_a_regs.idr.fbc_unicast_autogen_start_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_unicast_autogen_start_reg.fbc_unicast_autogen_start), sizeof(uint32) * 0x290c, sizeof(uint32) * 0x0000, 20, 0);

  /* Fbc Unicast Autogen End */
  Soc_petra_a_regs.idr.fbc_unicast_autogen_end_reg.addr.base = sizeof(uint32) * 0x290d;
  Soc_petra_a_regs.idr.fbc_unicast_autogen_end_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_unicast_autogen_end_reg.fbc_unicast_autogen_end), sizeof(uint32) * 0x290d, sizeof(uint32) * 0x0000, 20, 0);

  /* Fbc Cache Configuration */
  Soc_petra_a_regs.idr.fbc_cache_configuration_reg.addr.base = sizeof(uint32) * 0x290e;
  Soc_petra_a_regs.idr.fbc_cache_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_cache_configuration_reg.fbc_unicast_cache_divider), sizeof(uint32) * 0x290e, sizeof(uint32) * 0x0000, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_cache_configuration_reg.fbc_mini_multicast_cache_divider), sizeof(uint32) * 0x290e, sizeof(uint32) * 0x0000, 11, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_cache_configuration_reg.fbc_full_multicast_cache_divider), sizeof(uint32) * 0x290e, sizeof(uint32) * 0x0000, 17, 12);

  /* Fbc Unicast Thresholds */
  Soc_petra_a_regs.idr.fbc_unicast_thresholds_reg.addr.base = sizeof(uint32) * 0x290f;
  Soc_petra_a_regs.idr.fbc_unicast_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_unicast_thresholds_reg.fbc_internal_unicast_new_write_th), sizeof(uint32) * 0x290f, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_unicast_thresholds_reg.fbc_internal_unicast_new_read_th), sizeof(uint32) * 0x290f, sizeof(uint32) * 0x0000, 14, 8);

  /* Fbc Mini Multicast Thresholds */
  Soc_petra_a_regs.idr.fbc_mini_multicast_thresholds_reg.addr.base = sizeof(uint32) * 0x2910;
  Soc_petra_a_regs.idr.fbc_mini_multicast_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_mini_multicast_thresholds_reg.fbc_internal_mini_multicast_new_write_th), sizeof(uint32) * 0x2910, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_mini_multicast_thresholds_reg.fbc_internal_mini_multicast_new_read_th), sizeof(uint32) * 0x2910, sizeof(uint32) * 0x0000, 14, 8);

  /* Fbc Full Multicast Thresholds */
  Soc_petra_a_regs.idr.fbc_full_multicast_thresholds_reg.addr.base = sizeof(uint32) * 0x2911;
  Soc_petra_a_regs.idr.fbc_full_multicast_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_full_multicast_thresholds_reg.fbc_internal_full_multicast_new_write_th), sizeof(uint32) * 0x2911, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_full_multicast_thresholds_reg.fbc_internal_full_multicast_new_read_th), sizeof(uint32) * 0x2911, sizeof(uint32) * 0x0000, 14, 8);

  /* Fbc External Full Multicast Limit Low */
  Soc_petra_a_regs.idr.fbc_external_full_multicast_limit_low_reg.addr.base = sizeof(uint32) * 0x2912;
  Soc_petra_a_regs.idr.fbc_external_full_multicast_limit_low_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_external_full_multicast_limit_low_reg.fbc_external_full_multicast_limit_low), sizeof(uint32) * 0x2912, sizeof(uint32) * 0x0000, 21, 0);

  /* Fbc External Full Multicast Limit High */
  Soc_petra_a_regs.idr.fbc_external_full_multicast_limit_high_reg.addr.base = sizeof(uint32) * 0x2913;
  Soc_petra_a_regs.idr.fbc_external_full_multicast_limit_high_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_external_full_multicast_limit_high_reg.fbc_external_full_multicast_limit_high), sizeof(uint32) * 0x2913, sizeof(uint32) * 0x0000, 21, 0);

  /* Fbc External Mini Multicast Limit Low */
  Soc_petra_a_regs.idr.fbc_external_mini_multicast_limit_low_reg.addr.base = sizeof(uint32) * 0x2914;
  Soc_petra_a_regs.idr.fbc_external_mini_multicast_limit_low_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_external_mini_multicast_limit_low_reg.fbc_external_mini_multicast_limit_low), sizeof(uint32) * 0x2914, sizeof(uint32) * 0x0000, 21, 0);

  /* Fbc External Mini Multicast Limit High */
  Soc_petra_a_regs.idr.fbc_external_mini_multicast_limit_high_reg.addr.base = sizeof(uint32) * 0x2915;
  Soc_petra_a_regs.idr.fbc_external_mini_multicast_limit_high_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_external_mini_multicast_limit_high_reg.fbc_external_mini_multicast_limit_high), sizeof(uint32) * 0x2915, sizeof(uint32) * 0x0000, 21, 0);

  /* Fbc External Unicast Limit Low */
  Soc_petra_a_regs.idr.fbc_external_unicast_limit_low_reg.addr.base = sizeof(uint32) * 0x2916;
  Soc_petra_a_regs.idr.fbc_external_unicast_limit_low_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_external_unicast_limit_low_reg.fbc_external_unicast_limit_low), sizeof(uint32) * 0x2916, sizeof(uint32) * 0x0000, 21, 0);

  /* Fbc External Unicast Limit High */
  Soc_petra_a_regs.idr.fbc_external_unicast_limit_high_reg.addr.base = sizeof(uint32) * 0x2917;
  Soc_petra_a_regs.idr.fbc_external_unicast_limit_high_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.fbc_external_unicast_limit_high_reg.fbc_external_unicast_limit_high), sizeof(uint32) * 0x2917, sizeof(uint32) * 0x0000, 21, 0);

  /* Reassembly Errors */
  Soc_petra_a_regs.idr.reassembly_errors_reg.addr.base = sizeof(uint32) * 0x2977;
  Soc_petra_a_regs.idr.reassembly_errors_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reassembly_errors_reg.reassembly_error_mask), sizeof(uint32) * 0x2977, sizeof(uint32) * 0x0000, 12, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reassembly_errors_reg.reassembly_errors), sizeof(uint32) * 0x2977, sizeof(uint32) * 0x0000, 31, 16);

  /* Reassembly Timeout */
  Soc_petra_a_regs.idr.reassembly_timeout_reg.addr.base = sizeof(uint32) * 0x2978;
  Soc_petra_a_regs.idr.reassembly_timeout_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.reassembly_timeout_reg.reassembly_timeout), sizeof(uint32) * 0x2978, sizeof(uint32) * 0x0000, 23, 0);

  /* Timeout Reg */
  Soc_petra_a_regs.idr.timeout_reg_0.addr.base = sizeof(uint32) * 0x297c;
  Soc_petra_a_regs.idr.timeout_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.timeout_reg_0.timeout_reg), sizeof(uint32) * 0x297c, sizeof(uint32) * 0x0000, 31, 0);

  /* Timeout Reg */
  Soc_petra_a_regs.idr.timeout_reg_1.addr.base = sizeof(uint32) * 0x297d;
  Soc_petra_a_regs.idr.timeout_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.timeout_reg_1.timeout_reg), sizeof(uint32) * 0x297d, sizeof(uint32) * 0x0000, 31, 0);

  /* Timeout Reg */
  Soc_petra_a_regs.idr.timeout_reg_2.addr.base = sizeof(uint32) * 0x297e;
  Soc_petra_a_regs.idr.timeout_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.timeout_reg_2.timeout_reg), sizeof(uint32) * 0x297e, sizeof(uint32) * 0x0000, 15, 0);

  /* Counters Fap Ports */
  Soc_petra_a_regs.idr.counters_fap_ports_reg.addr.base = sizeof(uint32) * 0x2999;
  Soc_petra_a_regs.idr.counters_fap_ports_reg.addr.step = sizeof(uint32) * 0x0000;

  for (fld_idx = 0; fld_idx < SOC_PETRA_COUNTERS_FAP_PORTS_NOF_FLDS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.idr.counters_fap_ports_reg.fap_port[fld_idx]),
      sizeof(uint32) * 0x2999,
      sizeof(uint32) * 0x0000,
      (uint8)(6 + fld_idx*8),
      (uint8)(0 + fld_idx*8)
    );
  }

  /* Received Packets0 */
  Soc_petra_a_regs.idr.received_packets0_reg_0.addr.base = sizeof(uint32) * 0x299a;
  Soc_petra_a_regs.idr.received_packets0_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets0_reg_0.received_packets0), sizeof(uint32) * 0x299a, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets0 */
  Soc_petra_a_regs.idr.received_packets0_reg_1.addr.base = sizeof(uint32) * 0x299b;
  Soc_petra_a_regs.idr.received_packets0_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets0_reg_1.received_packets0), sizeof(uint32) * 0x299b, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets1 */
  Soc_petra_a_regs.idr.received_packets1_reg_0.addr.base = sizeof(uint32) * 0x299c;
  Soc_petra_a_regs.idr.received_packets1_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets1_reg_0.received_packets1), sizeof(uint32) * 0x299c, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets1 */
  Soc_petra_a_regs.idr.received_packets1_reg_1.addr.base = sizeof(uint32) * 0x299d;
  Soc_petra_a_regs.idr.received_packets1_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets1_reg_1.received_packets1), sizeof(uint32) * 0x299d, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets2 */
  Soc_petra_a_regs.idr.received_packets2_reg_0.addr.base = sizeof(uint32) * 0x299e;
  Soc_petra_a_regs.idr.received_packets2_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets2_reg_0.received_packets2), sizeof(uint32) * 0x299e, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets2 */
  Soc_petra_a_regs.idr.received_packets2_reg_1.addr.base = sizeof(uint32) * 0x299f;
  Soc_petra_a_regs.idr.received_packets2_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets2_reg_1.received_packets2), sizeof(uint32) * 0x299f, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets3 */
  Soc_petra_a_regs.idr.received_packets3_reg_0.addr.base = sizeof(uint32) * 0x29a0;
  Soc_petra_a_regs.idr.received_packets3_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets3_reg_0.received_packets3), sizeof(uint32) * 0x29a0, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Packets3 */
  Soc_petra_a_regs.idr.received_packets3_reg_1.addr.base = sizeof(uint32) * 0x29a1;
  Soc_petra_a_regs.idr.received_packets3_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_packets3_reg_1.received_packets3), sizeof(uint32) * 0x29a1, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets0 */
  Soc_petra_a_regs.idr.received_octets0_reg_0.addr.base = sizeof(uint32) * 0x29a2;
  Soc_petra_a_regs.idr.received_octets0_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets0_reg_0.received_octets0), sizeof(uint32) * 0x29a2, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets0 */
  Soc_petra_a_regs.idr.received_octets0_reg_1.addr.base = sizeof(uint32) * 0x29a3;
  Soc_petra_a_regs.idr.received_octets0_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets0_reg_1.received_octets0), sizeof(uint32) * 0x29a3, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets1 */
  Soc_petra_a_regs.idr.received_octets1_reg_0.addr.base = sizeof(uint32) * 0x29a4;
  Soc_petra_a_regs.idr.received_octets1_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets1_reg_0.received_octets1), sizeof(uint32) * 0x29a4, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets1 */
  Soc_petra_a_regs.idr.received_octets1_reg_1.addr.base = sizeof(uint32) * 0x29a5;
  Soc_petra_a_regs.idr.received_octets1_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets1_reg_1.received_octets1), sizeof(uint32) * 0x29a5, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets2 */
  Soc_petra_a_regs.idr.received_octets2_reg_0.addr.base = sizeof(uint32) * 0x29a6;
  Soc_petra_a_regs.idr.received_octets2_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets2_reg_0.received_octets2), sizeof(uint32) * 0x29a6, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets2 */
  Soc_petra_a_regs.idr.received_octets2_reg_1.addr.base = sizeof(uint32) * 0x29a7;
  Soc_petra_a_regs.idr.received_octets2_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets2_reg_1.received_octets2), sizeof(uint32) * 0x29a7, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets3 */
  Soc_petra_a_regs.idr.received_octets3_reg_0.addr.base = sizeof(uint32) * 0x29a8;
  Soc_petra_a_regs.idr.received_octets3_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets3_reg_0.received_octets3), sizeof(uint32) * 0x29a8, sizeof(uint32) * 0x0000, 31, 0);

  /* Received Octets3 */
  Soc_petra_a_regs.idr.received_octets3_reg_1.addr.base = sizeof(uint32) * 0x29a9;
  Soc_petra_a_regs.idr.received_octets3_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.received_octets3_reg_1.received_octets3), sizeof(uint32) * 0x29a9, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets0 */
  Soc_petra_a_regs.idr.discarded_packets0_reg_0.addr.base = sizeof(uint32) * 0x29aa;
  Soc_petra_a_regs.idr.discarded_packets0_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets0_reg_0.discarded_packets0), sizeof(uint32) * 0x29aa, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets0 */
  Soc_petra_a_regs.idr.discarded_packets0_reg_1.addr.base = sizeof(uint32) * 0x29ab;
  Soc_petra_a_regs.idr.discarded_packets0_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets0_reg_1.discarded_packets0), sizeof(uint32) * 0x29ab, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets1 */
  Soc_petra_a_regs.idr.discarded_packets1_reg_0.addr.base = sizeof(uint32) * 0x29ac;
  Soc_petra_a_regs.idr.discarded_packets1_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets1_reg_0.discarded_packets1), sizeof(uint32) * 0x29ac, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets1 */
  Soc_petra_a_regs.idr.discarded_packets1_reg_1.addr.base = sizeof(uint32) * 0x29ad;
  Soc_petra_a_regs.idr.discarded_packets1_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets1_reg_1.discarded_packets1), sizeof(uint32) * 0x29ad, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets2 */
  Soc_petra_a_regs.idr.discarded_packets2_reg_0.addr.base = sizeof(uint32) * 0x29ae;
  Soc_petra_a_regs.idr.discarded_packets2_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets2_reg_0.discarded_packets2), sizeof(uint32) * 0x29ae, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets2 */
  Soc_petra_a_regs.idr.discarded_packets2_reg_1.addr.base = sizeof(uint32) * 0x29af;
  Soc_petra_a_regs.idr.discarded_packets2_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets2_reg_1.discarded_packets2), sizeof(uint32) * 0x29af, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets3 */
  Soc_petra_a_regs.idr.discarded_packets3_reg_0.addr.base = sizeof(uint32) * 0x29b0;
  Soc_petra_a_regs.idr.discarded_packets3_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets3_reg_0.discarded_packets3), sizeof(uint32) * 0x29b0, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Packets3 */
  Soc_petra_a_regs.idr.discarded_packets3_reg_1.addr.base = sizeof(uint32) * 0x29b1;
  Soc_petra_a_regs.idr.discarded_packets3_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_packets3_reg_1.discarded_packets3), sizeof(uint32) * 0x29b1, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets0 */
  Soc_petra_a_regs.idr.discarded_octets0_reg_0.addr.base = sizeof(uint32) * 0x29b2;
  Soc_petra_a_regs.idr.discarded_octets0_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets0_reg_0.discarded_octets0), sizeof(uint32) * 0x29b2, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets0 */
  Soc_petra_a_regs.idr.discarded_octets0_reg_1.addr.base = sizeof(uint32) * 0x29b3;
  Soc_petra_a_regs.idr.discarded_octets0_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets0_reg_1.discarded_octets0), sizeof(uint32) * 0x29b3, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets1 */
  Soc_petra_a_regs.idr.discarded_octets1_reg_0.addr.base = sizeof(uint32) * 0x29b4;
  Soc_petra_a_regs.idr.discarded_octets1_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets1_reg_0.discarded_octets1), sizeof(uint32) * 0x29b4, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets1 */
  Soc_petra_a_regs.idr.discarded_octets1_reg_1.addr.base = sizeof(uint32) * 0x29b5;
  Soc_petra_a_regs.idr.discarded_octets1_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets1_reg_1.discarded_octets1), sizeof(uint32) * 0x29b5, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets2 */
  Soc_petra_a_regs.idr.discarded_octets2_reg_0.addr.base = sizeof(uint32) * 0x29b6;
  Soc_petra_a_regs.idr.discarded_octets2_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets2_reg_0.discarded_octets2), sizeof(uint32) * 0x29b6, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets2 */
  Soc_petra_a_regs.idr.discarded_octets2_reg_1.addr.base = sizeof(uint32) * 0x29b7;
  Soc_petra_a_regs.idr.discarded_octets2_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets2_reg_1.discarded_octets2), sizeof(uint32) * 0x29b7, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets3 */
  Soc_petra_a_regs.idr.discarded_octets3_reg_0.addr.base = sizeof(uint32) * 0x29b8;
  Soc_petra_a_regs.idr.discarded_octets3_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets3_reg_0.discarded_octets3), sizeof(uint32) * 0x29b8, sizeof(uint32) * 0x0000, 31, 0);

  /* Discarded Octets3 */
  Soc_petra_a_regs.idr.discarded_octets3_reg_1.addr.base = sizeof(uint32) * 0x29b9;
  Soc_petra_a_regs.idr.discarded_octets3_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.idr.discarded_octets3_reg_1.discarded_octets3), sizeof(uint32) * 0x29b9, sizeof(uint32) * 0x0000, 31, 0);
}

/* Block registers initialization: IRR */
STATIC void
  soc_pa_regs_init_IRR(void)
{
  uint32
    fld_idx;

  Soc_petra_a_regs.irr.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IRR;
  Soc_petra_a_regs.irr.addr.base = sizeof(uint32) * 0x2a00;
  Soc_petra_a_regs.irr.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.irr.interrupt_reg.addr.base = sizeof(uint32) * 0x2a00;
  Soc_petra_a_regs.irr.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.err_max_replication), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_max_replication), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.out_of_sequence), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.out_of_synchronization), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_free_pcp_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_is_free_pcp_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_pcp_link_table_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_is_pcp_link_table_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_pcm_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_is_pcm_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_uc_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_mc_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_nif_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_glag_mapping_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_reg.error_isf_ecc), sizeof(uint32) * 0x2a00, sizeof(uint32) * 0x0000, 14, 14);

  /* Interrupt Register Mask */
  Soc_petra_a_regs.irr.interrupt_register_mask_reg.addr.base = sizeof(uint32) * 0x2a10;
  Soc_petra_a_regs.irr.interrupt_register_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.err_max_replication_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_max_replication_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.out_of_sequence_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.out_of_synchronization_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_free_pcp_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_is_free_pcp_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_pcp_link_table_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_is_pcp_link_table_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_pcm_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_is_pcm_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_uc_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_mc_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_nif_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_glag_mapping_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.interrupt_register_mask_reg.error_isf_ecc_mask), sizeof(uint32) * 0x2a10, sizeof(uint32) * 0x0000, 14, 14);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x2a20;
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x2a20, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x2a21;
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x2a21, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x2a22;
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x2a22, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x2a23;
  Soc_petra_a_regs.irr.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x2a23, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x2a30;
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x2a30, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x2a31;
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x2a31, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x2a32;
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x2a32, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x2a33;
  Soc_petra_a_regs.irr.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x2a33, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.irr.indirect_command_reg.addr.base = sizeof(uint32) * 0x2a40;
  Soc_petra_a_regs.irr.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x2a40, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x2a40, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x2a40, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x2a40, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x2a40, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.irr.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x2a41;
  Soc_petra_a_regs.irr.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x2a41, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x2a41, sizeof(uint32) * 0x0000, 31, 31);

  /* Static Configuration */
  Soc_petra_a_regs.irr.static_configuration_reg.addr.base = sizeof(uint32) * 0x2a54;
  Soc_petra_a_regs.irr.static_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.dis_ecc), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.disable_sn_error), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.drop_precedence_high_priority), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 5, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.idr_mc_desc_weight), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.iqm_is_desc_weight), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 13, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.deq_fc_threshold_l), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 20, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.deq_fc_threshold_h), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 27, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.add_is_traffic_class), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.static_configuration_reg.dis_glag_mapping_ecc), sizeof(uint32) * 0x2a54, sizeof(uint32) * 0x0000, 29, 29);

  /* Dynamic Configuration */
  Soc_petra_a_regs.irr.dynamic_configuration_reg.addr.base = sizeof(uint32) * 0x2a55;
  Soc_petra_a_regs.irr.dynamic_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.dynamic_configuration_reg.enable_data_path_idr), sizeof(uint32) * 0x2a55, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.dynamic_configuration_reg.enable_data_path_iqm), sizeof(uint32) * 0x2a55, sizeof(uint32) * 0x0000, 1, 1);

  /* Snoop Num Of Bytes Configuration Register */
  Soc_petra_a_regs.irr.snoop_num_of_bytes_configuration_reg.addr.base = sizeof(uint32) * 0x2a56;
  Soc_petra_a_regs.irr.snoop_num_of_bytes_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_PER_SNOOP_CMD_NOF_FLDS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.irr.snoop_num_of_bytes_configuration_reg.snoop_num_of_bytes[fld_idx]),
      sizeof(uint32) * 0x2a56,
      sizeof(uint32) * 0x0000,
      (uint8)(2*fld_idx + 1),
      (uint8)(2*fld_idx)
    );
  }

  /* Resequencer Timeout Register */
  Soc_petra_a_regs.irr.resequencer_timeout_reg.addr.base = sizeof(uint32) * 0x2a57;
  Soc_petra_a_regs.irr.resequencer_timeout_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.resequencer_timeout_reg.resequencer_skew_timeout), sizeof(uint32) * 0x2a57, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.resequencer_timeout_reg.resequencer_transfer_timeout), sizeof(uint32) * 0x2a57, sizeof(uint32) * 0x0000, 31, 16);

  /* Resequencing Port Enable */
  Soc_petra_a_regs.irr.resequencing_port_enable_reg.addr.base = sizeof(uint32) * 0x2a58;
  Soc_petra_a_regs.irr.resequencing_port_enable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.resequencing_port_enable_reg.resequencing_port_enable), sizeof(uint32) * 0x2a58, sizeof(uint32) * 0x0000, 3, 0);

  /* Error Counters */
  Soc_petra_a_regs.irr.error_counters_reg.addr.base = sizeof(uint32) * 0x2a59;
  Soc_petra_a_regs.irr.error_counters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.error_counters_reg.sn_out_of_synchronization_cnt), sizeof(uint32) * 0x2a59, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.error_counters_reg.sn_out_of_sequence_cnt), sizeof(uint32) * 0x2a59, sizeof(uint32) * 0x0000, 31, 16);

  /* Err Max Replication Mc Id Value */
  Soc_petra_a_regs.irr.err_max_replication_mc_id_value_reg.addr.base = sizeof(uint32) * 0x2a5a;
  Soc_petra_a_regs.irr.err_max_replication_mc_id_value_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.err_max_replication_mc_id_value_reg.err_max_replication_mc_id_value), sizeof(uint32) * 0x2a5a, sizeof(uint32) * 0x0000, 13, 0);

  /* Error Max Replication Mc Id Value */
  Soc_petra_a_regs.irr.error_max_replication_mc_id_value_reg.addr.base = sizeof(uint32) * 0x2a5b;
  Soc_petra_a_regs.irr.error_max_replication_mc_id_value_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.error_max_replication_mc_id_value_reg.error_max_replication_mc_id_value), sizeof(uint32) * 0x2a5b, sizeof(uint32) * 0x0000, 13, 0);

  /* Max Replication Register */
  Soc_petra_a_regs.irr.max_replication_reg.addr.base = sizeof(uint32) * 0x2a5c;
  Soc_petra_a_regs.irr.max_replication_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.max_replication_reg.max_replication_register), sizeof(uint32) * 0x2a5c, sizeof(uint32) * 0x0000, 12, 0);

  /* Nif Port Configuration Register */
  Soc_petra_a_regs.irr.nif_port_configuration_reg.addr.base = sizeof(uint32) * 0x2a5d;
  Soc_petra_a_regs.irr.nif_port_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.nif_port_configuration_reg.nif_port), sizeof(uint32) * 0x2a5d, sizeof(uint32) * 0x0000, 6, 0);

  /* Resequencing Configuration */
  Soc_petra_a_regs.irr.resequencing_configuration_reg.addr.base = sizeof(uint32) * 0x2a5e;
  Soc_petra_a_regs.irr.resequencing_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.resequencing_configuration_reg.bypass_reseq), sizeof(uint32) * 0x2a5e, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.resequencing_configuration_reg.enable_fast_resequencing), sizeof(uint32) * 0x2a5e, sizeof(uint32) * 0x0000, 1, 1);

  /* Resequence Reset Configuration Register */
  Soc_petra_a_regs.irr.resequence_reset_configuration_reg.addr.base = sizeof(uint32) * 0x2a5f;
  Soc_petra_a_regs.irr.resequence_reset_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.resequence_reset_configuration_reg.resequence_reset), sizeof(uint32) * 0x2a5f, sizeof(uint32) * 0x0000, 0, 0);

  /* Unicast Traffic Class Mapping Register */
  Soc_petra_a_regs.irr.unicast_traffic_class_mapping_reg.addr.base = sizeof(uint32) * 0x2a60;
  Soc_petra_a_regs.irr.unicast_traffic_class_mapping_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_PER_TC_NOF_FLDS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.irr.unicast_traffic_class_mapping_reg.unicast_traffic_class_mapping_register[fld_idx]),
      sizeof(uint32) * 0x2a60,
      sizeof(uint32) * 0x0000,
      (uint8)(fld_idx*4 + 2),
      (uint8)(fld_idx*4)
    );
  }

  /* Ingress Multicast Traffic Class Mapping Register */
  Soc_petra_a_regs.irr.ingress_mc_tc_mapping_reg.addr.base = sizeof(uint32) * 0x2a61;
  Soc_petra_a_regs.irr.ingress_mc_tc_mapping_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_PER_TC_NOF_FLDS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.irr.ingress_mc_tc_mapping_reg.ingress_mc_tc_mapping[fld_idx]),
      sizeof(uint32) * 0x2a61,
      sizeof(uint32) * 0x0000,
      (uint8)(fld_idx*4 + 2),
      (uint8)(fld_idx*4)
    );
  }

  /* Egress Or Fabric Multicast Traffic Class Mapping Register */
  for (fld_idx = 0; fld_idx < SOC_PETRA_REGS_PER_TC_NOF_FLDS; fld_idx++)
  {
    Soc_petra_a_regs.irr.egress_or_fabric_mc_tc_class_mapping_reg.addr.base = sizeof(uint32) * 0x2a62;
    Soc_petra_a_regs.irr.egress_or_fabric_mc_tc_class_mapping_reg.addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.irr.egress_or_fabric_mc_tc_class_mapping_reg.egr_or_fabric_mc_tc_mapping[fld_idx]),
      sizeof(uint32) * 0x2a62,
      sizeof(uint32) * 0x0000,
      (uint8)(fld_idx*2 + 1),
      (uint8)(fld_idx*2)
    );
  }

  /* Egress Or Fabric Multicast Base Queue Register */
  Soc_petra_a_regs.irr.egress_or_fabric_multicast_base_queue_reg.addr.base = sizeof(uint32) * 0x2a63;
  Soc_petra_a_regs.irr.egress_or_fabric_multicast_base_queue_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.irr.egress_or_fabric_multicast_base_queue_reg.egress_or_fabric_multicast_base_queue_register), sizeof(uint32) * 0x2a63, sizeof(uint32) * 0x0000, 14, 0);
}

/* Block registers initialization: IHP */
STATIC void
  soc_pa_regs_init_IHP(void)
{
  uint32
    reg_idx = 0,
    fld_idx = 0,
    fld_idx2 = 0;

  Soc_petra_a_regs.ihp.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IHP;
  Soc_petra_a_regs.ihp.addr.base = sizeof(uint32) * 0x2600;
  Soc_petra_a_regs.ihp.addr.step = sizeof(uint32) * 0x0000;

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x2620;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x2620, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x2621;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x2621, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x2622;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x2622, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x2623;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x2623, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x2624;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x2624, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x2625;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x2625, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_6.addr.base = sizeof(uint32) * 0x2626;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_6.indirect_command_wr_data), sizeof(uint32) * 0x2626, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_7.addr.base = sizeof(uint32) * 0x2627;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_7.indirect_command_wr_data), sizeof(uint32) * 0x2627, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_8.addr.base = sizeof(uint32) * 0x2628;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_8.indirect_command_wr_data), sizeof(uint32) * 0x2628, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_9.addr.base = sizeof(uint32) * 0x2629;
  Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_wr_data_reg_9.indirect_command_wr_data), sizeof(uint32) * 0x2629, sizeof(uint32) * 0x0000, 21, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x2630;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x2630, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x2631;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x2631, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x2632;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x2632, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x2633;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x2633, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x2634;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x2634, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x2635;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x2635, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_6.addr.base = sizeof(uint32) * 0x2636;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_6.indirect_command_rd_data), sizeof(uint32) * 0x2636, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_7.addr.base = sizeof(uint32) * 0x2637;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_7.indirect_command_rd_data), sizeof(uint32) * 0x2637, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_8.addr.base = sizeof(uint32) * 0x2638;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_8.indirect_command_rd_data), sizeof(uint32) * 0x2638, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_9.addr.base = sizeof(uint32) * 0x2639;
  Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_rd_data_reg_9.indirect_command_rd_data), sizeof(uint32) * 0x2639, sizeof(uint32) * 0x0000, 21, 0);

  /* Indirect Command */
  Soc_petra_a_regs.ihp.indirect_command_reg.addr.base = sizeof(uint32) * 0x2640;
  Soc_petra_a_regs.ihp.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x2640, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x2640, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x2640, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x2640, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x2640, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.ihp.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x2641;
  Soc_petra_a_regs.ihp.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x2641, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x2641, sizeof(uint32) * 0x0000, 31, 31);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x2690,
    0x0000,
    &(Soc_petra_a_regs.ihp.pcmi_config_reg),
    &(Soc_petra_a_regs.ihp.pcmi_results_reg)
  );

  /* General Configuration Register */
  Soc_petra_a_regs.ihp.general_configuration_reg.addr.base = sizeof(uint32) * 0x2702;
  Soc_petra_a_regs.ihp.general_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.general_configuration_reg.sample_dropped_only), sizeof(uint32) * 0x2702, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.general_configuration_reg.lag_hash_crc_select), sizeof(uint32) * 0x2702, sizeof(uint32) * 0x0000, 11, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.general_configuration_reg.uc_flow_base_qnum_add), sizeof(uint32) * 0x2702, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.general_configuration_reg.uc_flow_base_qnum), sizeof(uint32) * 0x2702, sizeof(uint32) * 0x0000, 27, 13);

  /* Tcam Configuration Register */
  Soc_petra_a_regs.ihp.tcam_configuration_reg.addr.base = sizeof(uint32) * 0x2703;
  Soc_petra_a_regs.ihp.tcam_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.tcam_configuration_reg.tcam_disable), sizeof(uint32) * 0x2703, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.tcam_configuration_reg.tcam_power_down), sizeof(uint32) * 0x2703, sizeof(uint32) * 0x0000, 4, 4);

  /* Sync Configuration */
  Soc_petra_a_regs.ihp.sync_configuration_reg.addr.base = sizeof(uint32) * 0x2726;
  Soc_petra_a_regs.ihp.sync_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.sync_configuration_reg.sync_counter), sizeof(uint32) * 0x2726, sizeof(uint32) * 0x0000, 10, 1);

  /* Lag Hash Offset Config */
  Soc_petra_a_regs.ihp.lag_hash_offset_config_reg.addr.base = sizeof(uint32) * 0x2727;
  Soc_petra_a_regs.ihp.lag_hash_offset_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.lag_hash_offset_config_reg.lag_hash_offset_00), sizeof(uint32) * 0x2727, sizeof(uint32) * 0x0000, 4, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.lag_hash_offset_config_reg.lag_hash_offset_01), sizeof(uint32) * 0x2727, sizeof(uint32) * 0x0000, 9, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.lag_hash_offset_config_reg.lag_hash_offset_10), sizeof(uint32) * 0x2727, sizeof(uint32) * 0x0000, 14, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.lag_hash_offset_config_reg.lag_hash_offset_11), sizeof(uint32) * 0x2727, sizeof(uint32) * 0x0000, 19, 15);

  /* Port Map Register */
  Soc_petra_a_regs.ihp.port_map_reg.addr.base = sizeof(uint32) * 0x2728;
  Soc_petra_a_regs.ihp.port_map_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.port_map_reg.port_map_enable), sizeof(uint32) * 0x2728, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.port_map_reg.port_map_index), sizeof(uint32) * 0x2728, sizeof(uint32) * 0x0000, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.port_map_reg.port_mapped0), sizeof(uint32) * 0x2728, sizeof(uint32) * 0x0000, 22, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.port_map_reg.port_mapped1), sizeof(uint32) * 0x2728, sizeof(uint32) * 0x0000, 30, 24);

  /* Ptc Custom Macro Cmd0-7 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_PTC_CUSTOM_MACRO_CMD_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].addr.base = sizeof(uint32) * (0x2729 + reg_idx);
    Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;

    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].load_offset), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 0, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].shift_b), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 5, 1);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].shift_a), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 10, 6);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].cond_sel), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 14, 11);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].mask_right), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 19, 15);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].mask_left), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 24, 20);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_macro_cmd_reg[reg_idx].dw_sel), sizeof(uint32) * (0x2729 + reg_idx), sizeof(uint32) * 0x0000, 28, 25);
  }

  /* Ptc Custom Protocols 0-6 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_PTC_CUSTOM_PROTOCOLS_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ihp.ptc_custom_protocols_reg[reg_idx].addr.base = sizeof(uint32) * (0x2731 + reg_idx);
    Soc_petra_a_regs.ihp.ptc_custom_protocols_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_custom_protocols_reg[reg_idx].ptc_custom_protocols), sizeof(uint32) * (0x2731 + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Ecn Config */
  Soc_petra_a_regs.ihp.ecn_config_reg.addr.base = sizeof(uint32) * 0x276c;
  Soc_petra_a_regs.ihp.ecn_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ecn_config_reg.ecn_enable), sizeof(uint32) * 0x276c, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ecn_config_reg.ecn_mapping), sizeof(uint32) * 0x276c, sizeof(uint32) * 0x0000, 15, 8);


  /* Ptc Ip Protocols */
  for (reg_idx = 0; reg_idx < SOC_PETRA_PTC_IP_PROTOCOLS_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ihp.ptc_ip_protocols_reg[reg_idx].addr.base = sizeof(uint32) * (0x2738 + reg_idx);
    Soc_petra_a_regs.ihp.ptc_ip_protocols_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_PTC_IP_PROTOCOLS_NOF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.ihp.ptc_ip_protocols_reg[reg_idx].ptc_ip_protocols[fld_idx]),
        sizeof(uint32) * (0x2738 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(7 + (8*fld_idx)),
        (uint8)(0 + (8*fld_idx))
      );
    }
  }

  /* Ptc Ethernet Types */
  for (reg_idx = 0; reg_idx < SOC_PETRA_PTC_ETHERNET_TYPES_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ihp.ptc_ethernet_types_reg[reg_idx].addr.base = sizeof(uint32) * (0x273a + reg_idx);
    Soc_petra_a_regs.ihp.ptc_ethernet_types_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_PTC_ETHERNET_TYPES_NOF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.ihp.ptc_ethernet_types_reg[reg_idx].ptc_ethernet_types[fld_idx]),
        sizeof(uint32) * (0x273a + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(15 + (16*fld_idx)),
        (uint8)(0 + (16*fld_idx))
      );
    }
  }

  /* Ptc Is Ethernet Type */
  Soc_petra_a_regs.ihp.ptc_is_ethernet_type_reg.addr.base = sizeof(uint32) * 0x273e;
  Soc_petra_a_regs.ihp.ptc_is_ethernet_type_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.ptc_is_ethernet_type_reg.ptc_is_ethernet_type), sizeof(uint32) * 0x273e, sizeof(uint32) * 0x0000, 6, 0);

  /* Debug Header */
  for (reg_idx = 0; reg_idx < SOC_PA_NOF_DEBUG_HEADER_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ihp.debug_header_reg[reg_idx].addr.base = sizeof(uint32) * (0x276d + reg_idx);
    Soc_petra_a_regs.ihp.debug_header_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.debug_header_reg[reg_idx].debug_header), sizeof(uint32) * (0x276d + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Debug Port Type */
  Soc_petra_a_regs.ihp.debug_port_type_reg.addr.base = sizeof(uint32) * 0x2770;
  Soc_petra_a_regs.ihp.debug_port_type_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.debug_port_type_reg.debug_port_type), sizeof(uint32) * 0x2770, sizeof(uint32) * 0x0000, 2, 0);

  /* Snoop Config Reg */
  Soc_petra_a_regs.ihp.snoop_config_reg.addr.base = sizeof(uint32) * 0x2771;
  Soc_petra_a_regs.ihp.snoop_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.snoop_config_reg.snoop_is_mc), sizeof(uint32) * 0x2771, sizeof(uint32) * 0x0000, 15, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.snoop_config_reg.snoop_disable), sizeof(uint32) * 0x2771, sizeof(uint32) * 0x0000, 31, 17);

  /* Acl Bank Select */
  Soc_petra_a_regs.ihp.acl_bank_select_reg.addr.base = sizeof(uint32) * 0x27c8;
  Soc_petra_a_regs.ihp.acl_bank_select_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle0_bank_a), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle0_bank_b), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 5, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle0_bank_c), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 8, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle0_bank_d), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 11, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle1_bank_a), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 14, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle1_bank_b), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 17, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle1_bank_c), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 20, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycle1_bank_d), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, 23, 21);

  for (fld_idx = 0,reg_idx=0; fld_idx < SOC_PETRA_REGS_TCAM_NOF_CYCLES ; ++fld_idx )
  {
    for (fld_idx2 = 0; fld_idx2 < SOC_PETRA_REGS_TCAM_NOF_BANKS ; ++fld_idx2 )
    {
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.acl_bank_select_reg.tcam_source_cycles_banks[fld_idx][fld_idx2]), sizeof(uint32) * 0x27c8, sizeof(uint32) * 0x0000, (uint8)((reg_idx * 3) + 2), (uint8)(reg_idx * 3));
      ++reg_idx;
    }
  }

  /* Exact Match Mask0 */
  Soc_petra_a_regs.ihp.exact_match_mask0_reg_0.addr.base = sizeof(uint32) * 0x27f2;
  Soc_petra_a_regs.ihp.exact_match_mask0_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask0_reg_0.exact_match_mask0), sizeof(uint32) * 0x27f2, sizeof(uint32) * 0x0000, 31, 0);

  /* Exact Match Mask0 */
  Soc_petra_a_regs.ihp.exact_match_mask0_reg_1.addr.base = sizeof(uint32) * 0x27f3;
  Soc_petra_a_regs.ihp.exact_match_mask0_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask0_reg_1.exact_match_mask0), sizeof(uint32) * 0x27f3, sizeof(uint32) * 0x0000, 27, 0);

  /* Exact Match Mask1 */
  Soc_petra_a_regs.ihp.exact_match_mask1_reg_0.addr.base = sizeof(uint32) * 0x27f4;
  Soc_petra_a_regs.ihp.exact_match_mask1_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask1_reg_0.exact_match_mask1), sizeof(uint32) * 0x27f4, sizeof(uint32) * 0x0000, 31, 0);

  /* Exact Match Mask1 */
  Soc_petra_a_regs.ihp.exact_match_mask1_reg_1.addr.base = sizeof(uint32) * 0x27f5;
  Soc_petra_a_regs.ihp.exact_match_mask1_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask1_reg_1.exact_match_mask1), sizeof(uint32) * 0x27f5, sizeof(uint32) * 0x0000, 27, 0);

  /* Exact Match Mask2 */
  Soc_petra_a_regs.ihp.exact_match_mask2_reg_0.addr.base = sizeof(uint32) * 0x27f6;
  Soc_petra_a_regs.ihp.exact_match_mask2_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask2_reg_0.exact_match_mask2), sizeof(uint32) * 0x27f6, sizeof(uint32) * 0x0000, 31, 0);

  /* Exact Match Mask2 */
  Soc_petra_a_regs.ihp.exact_match_mask2_reg_1.addr.base = sizeof(uint32) * 0x27f7;
  Soc_petra_a_regs.ihp.exact_match_mask2_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask2_reg_1.exact_match_mask2), sizeof(uint32) * 0x27f7, sizeof(uint32) * 0x0000, 27, 0);

  /* Exact Match Mask3 */
  Soc_petra_a_regs.ihp.exact_match_mask3_reg_0.addr.base = sizeof(uint32) * 0x27f8;
  Soc_petra_a_regs.ihp.exact_match_mask3_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask3_reg_0.exact_match_mask3), sizeof(uint32) * 0x27f8, sizeof(uint32) * 0x0000, 31, 0);

  /* Exact Match Mask3 */
  Soc_petra_a_regs.ihp.exact_match_mask3_reg_1.addr.base = sizeof(uint32) * 0x27f9;
  Soc_petra_a_regs.ihp.exact_match_mask3_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_match_mask3_reg_1.exact_match_mask3), sizeof(uint32) * 0x27f9, sizeof(uint32) * 0x0000, 27, 0);

  /* Key Generation Co SHdr Var Masks */
  Soc_petra_a_regs.ihp.key_generation_co_shdr_var_masks_reg.addr.base = sizeof(uint32) * 0x27fa;
  Soc_petra_a_regs.ihp.key_generation_co_shdr_var_masks_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.key_generation_co_shdr_var_masks_reg.kgn_cos_hdr_var_mask0), sizeof(uint32) * 0x27fa, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.key_generation_co_shdr_var_masks_reg.kgn_cos_hdr_var_mask1), sizeof(uint32) * 0x27fa, sizeof(uint32) * 0x0000, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.key_generation_co_shdr_var_masks_reg.kgn_cos_hdr_var_mask2), sizeof(uint32) * 0x27fa, sizeof(uint32) * 0x0000, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.key_generation_co_shdr_var_masks_reg.kgn_cos_hdr_var_mask3), sizeof(uint32) * 0x27fa, sizeof(uint32) * 0x0000, 31, 24);

  /* Exact Lookup For Stat */
  Soc_petra_a_regs.ihp.exact_lookup_for_stat_reg_0.addr.base = sizeof(uint32) * 0x27fb;
  Soc_petra_a_regs.ihp.exact_lookup_for_stat_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_lookup_for_stat_reg_0.exact_lookup_for_stat), sizeof(uint32) * 0x27fb, sizeof(uint32) * 0x0000, 31, 0);

  /* Exact Lookup For Stat */
  Soc_petra_a_regs.ihp.exact_lookup_for_stat_reg_1.addr.base = sizeof(uint32) * 0x27fc;
  Soc_petra_a_regs.ihp.exact_lookup_for_stat_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.exact_lookup_for_stat_reg_1.exact_lookup_for_stat), sizeof(uint32) * 0x27fc, sizeof(uint32) * 0x0000, 31, 0);

  /* Tcam Lookup For Stat */
  Soc_petra_a_regs.ihp.tcam_lookup_for_stat_reg_0.addr.base = sizeof(uint32) * 0x27fd;
  Soc_petra_a_regs.ihp.tcam_lookup_for_stat_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.tcam_lookup_for_stat_reg_0.tcam_lookup_for_stat), sizeof(uint32) * 0x27fd, sizeof(uint32) * 0x0000, 31, 0);

  /* Tcam Lookup For Stat */
  Soc_petra_a_regs.ihp.tcam_lookup_for_stat_reg_1.addr.base = sizeof(uint32) * 0x27fe;
  Soc_petra_a_regs.ihp.tcam_lookup_for_stat_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.tcam_lookup_for_stat_reg_1.tcam_lookup_for_stat), sizeof(uint32) * 0x27fe, sizeof(uint32) * 0x0000, 31, 0);

  /* Default Resolved Stat Data */
  Soc_petra_a_regs.ihp.default_resolved_stat_data_reg.addr.base = sizeof(uint32) * 0x27ff;
  Soc_petra_a_regs.ihp.default_resolved_stat_data_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ihp.default_resolved_stat_data_reg.default_resolved_stat_data), sizeof(uint32) * 0x27ff, sizeof(uint32) * 0x0000, 11, 0);
}

/* Block registers initialization: IQM */
STATIC void
  soc_pa_regs_init_IQM(void)
{
  uint32
    reg_idx = 0,
    fld_idx = 0;

  Soc_petra_a_regs.iqm.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IQM;
  Soc_petra_a_regs.iqm.addr.base = sizeof(uint32) * 0x0400;
  Soc_petra_a_regs.iqm.addr.step = sizeof(uint32) * 0x0000;

  /* InterruptRegister */
  Soc_petra_a_regs.iqm.interrupt_reg.addr.base = sizeof(uint32) * 0x0400;
  Soc_petra_a_regs.iqm.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.free_bdb_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.free_bdb_unf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.full_uscnt_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.mini_uscnt_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.updt_fifo_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.txdscr_fifo_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.pd_fifo_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.qdr_sft_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.pd_cd_type_bit_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.pkt_enq_rsrc_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.pkt_enq_sn_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.pkt_enq_mc_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.pkt_enq_qnvalid_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.st_rpt_ovf), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.qroll_over), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.vsq_roll_over), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_reg.ecc_intr_err), sizeof(uint32) * 0x0400, sizeof(uint32) * 0x0000, 19, 19);

  /* EccInterruptRegister */
  Soc_petra_a_regs.iqm.ecc_interrupt_reg.addr.base = sizeof(uint32) * 0x0401;
  Soc_petra_a_regs.iqm.ecc_interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.pqdmd_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.taildscr_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.bdbll_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.fluscnt_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.mnuscnt_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.txpdm_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.txdscrm_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.dbffm_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.delffm_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.vsqb_qsz_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.vsqc_qsz_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_reg.vsqd_qsz_sft_err), sizeof(uint32) * 0x0401, sizeof(uint32) * 0x0000, 11, 11);

  /* InterruptMaskRegister */
  Soc_petra_a_regs.iqm.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x0410;
  Soc_petra_a_regs.iqm.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.free_bdb_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.free_bdb_unf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.full_uscnt_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.mini_uscnt_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.updt_fifo_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.txdscr_fifo_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.pd_fifo_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.qdr_sft_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.pd_cd_type_bit_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.pkt_enq_rsrc_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.pkt_enq_sn_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.pkt_enq_mc_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.pkt_enq_qnvalid_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.free_bdb_prot_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.st_rpt_ovf_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.qroll_over_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.vsq_roll_over_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.interrupt_mask_reg.ecc_intr_err_mask), sizeof(uint32) * 0x0410, sizeof(uint32) * 0x0000, 19, 19);

  /* EccInterruptRegisterMask */
  Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.addr.base = sizeof(uint32) * 0x0411;
  Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.pqdmd_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.taildscr_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.bdbll_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.fluscnt_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.mnuscnt_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.txpdm_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.txdscrm_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.dbffm_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.delffm_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.vsqb_qsz_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.vsqc_qsz_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_interrupt_register_mask_reg.vsqd_qsz_sft_err_mask), sizeof(uint32) * 0x0411, sizeof(uint32) * 0x0000, 11, 11);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x0420;
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x0420, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x0421;
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x0421, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x0422;
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x0422, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x0423;
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x0423, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x0424;
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x0424, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x0425;
  Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x0425, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x0430;
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x0430, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x0431;
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x0431, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x0432;
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x0432, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x0433;
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x0433, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x0434;
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x0434, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x0435;
  Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x0435, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommand */
  Soc_petra_a_regs.iqm.indirect_command_reg.addr.base = sizeof(uint32) * 0x0440;
  Soc_petra_a_regs.iqm.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x0440, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x0440, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x0440, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x0440, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x0440, sizeof(uint32) * 0x0000, 31, 31);

  /* IndirectCommandAddress */
  Soc_petra_a_regs.iqm.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x0441;
  Soc_petra_a_regs.iqm.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x0441, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x0441, sizeof(uint32) * 0x0000, 31, 31);

  /* ECC Configuration register */
  Soc_petra_a_regs.iqm.ecc_configuration_reg.addr.base = sizeof(uint32) *0x0460;
  Soc_petra_a_regs.iqm.ecc_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.ecc_configuration_reg.dis_ecc), sizeof(uint32) *0x0460, sizeof(uint32) * 0x0000, 0, 0);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x0470,
    0x0000,
    &(Soc_petra_a_regs.iqm.pcmi_config_reg),
    &(Soc_petra_a_regs.iqm.pcmi_results_reg)
  );

  /* Iqm Init */
  Soc_petra_a_regs.iqm.iqm_init_reg.addr.base = sizeof(uint32) *0x0480;
  Soc_petra_a_regs.iqm.iqm_init_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.iqm_init_reg.iqc_init), sizeof(uint32) *0x0480, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.iqm_init_reg.ste_init), sizeof(uint32) *0x0480, sizeof(uint32) * 0x0000, 1, 1);

  /* Iqm Enablers */
  Soc_petra_a_regs.iqm.iqm_enablers_reg.addr.base = sizeof(uint32) *0x0481;
  Soc_petra_a_regs.iqm.iqm_enablers_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.iqm_enablers_reg.en_ipt_cd), sizeof(uint32) *0x0481, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.iqm_enablers_reg.fwd_act_sel), sizeof(uint32) *0x0481, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.iqm_enablers_reg.dscrd_all_pkt), sizeof(uint32) *0x0481, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.iqm_enablers_reg.ecn_enable), sizeof(uint32) *0x0481, sizeof(uint32) * 0x0000, 3, 3);

  /* Packet Queues Categories1 */
  Soc_petra_a_regs.iqm.packet_queues_categories1_reg.addr.base = sizeof(uint32) *0x0482;
  Soc_petra_a_regs.iqm.packet_queues_categories1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.packet_queues_categories1_reg.top_pkt_qcat0), sizeof(uint32) *0x0482, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.packet_queues_categories1_reg.top_pkt_qcat1), sizeof(uint32) *0x0482, sizeof(uint32) * 0x0000, 30, 16);

  /* Packet Queues Categories2 */
  Soc_petra_a_regs.iqm.packet_queues_categories2_reg.addr.base = sizeof(uint32) *0x0483;
  Soc_petra_a_regs.iqm.packet_queues_categories2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.packet_queues_categories2_reg.top_pkt_qcat2), sizeof(uint32) *0x0483, sizeof(uint32) * 0x0000, 14, 0);

  /* General Flow Control Configuration Bdb High Priority */
  Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_high_priority_reg.addr.base = sizeof(uint32) *0x0484;
  Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_high_priority_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_mnt), sizeof(uint32) *0x0484, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_exp), sizeof(uint32) *0x0484, sizeof(uint32) * 0x0000, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_clr_fr_bdb_th_hp_mnt), sizeof(uint32) *0x0484, sizeof(uint32) * 0x0000, 19, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_clr_fr_bdb_th_hp_exp), sizeof(uint32) *0x0484, sizeof(uint32) * 0x0000, 23, 20);

  /* General Flow Control Configuration Bdb Low Priority */
  Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_low_priority_reg.addr.base = sizeof(uint32) *0x0485;
  Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_low_priority_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_set_fr_bdb_th_lp_mnt), sizeof(uint32) * 0x0485, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_set_fr_bdb_th_lp_exp), sizeof(uint32) * 0x0485, sizeof(uint32) * 0x0000, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_clr_fr_bdb_th_lp_mnt), sizeof(uint32) * 0x0485, sizeof(uint32) * 0x0000, 19, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_clr_fr_bdb_th_lp_exp), sizeof(uint32) * 0x0485, sizeof(uint32) * 0x0000, 23, 20);

  /* General Reject Configuration BDBs  */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
  {
    Soc_petra_a_regs.iqm.general_reject_configuration_bdbs_reg[reg_idx].addr.base = sizeof(uint32) * (0x0486 + reg_idx);
    Soc_petra_a_regs.iqm.general_reject_configuration_bdbs_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bdbs_reg[reg_idx].rjct_set_fr_bdb_th_mnt), sizeof(uint32) * (0x0486 + reg_idx), sizeof(uint32) * 0x0000, 7, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bdbs_reg[reg_idx].rjct_set_fr_bdb_th_exp), sizeof(uint32) * (0x0486 + reg_idx), sizeof(uint32) * 0x0000, 11, 8);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bdbs_reg[reg_idx].rjct_clr_fr_bdb_th_mnt), sizeof(uint32) * (0x0486 + reg_idx), sizeof(uint32) * 0x0000, 19, 12);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bdbs_reg[reg_idx].rjct_clr_fr_bdb_th_exp), sizeof(uint32) * (0x0486 + reg_idx), sizeof(uint32) * 0x0000, 23, 20);
  }

  /* General Reject Configuration BDs  */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
  {
    Soc_petra_a_regs.iqm.general_reject_configuration_bds_reg[reg_idx].addr.base = sizeof(uint32) * (0x048a + reg_idx);
    Soc_petra_a_regs.iqm.general_reject_configuration_bds_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bds_reg[reg_idx].rjct_set_oc_bd_th_mnt), sizeof(uint32) * (0x048a + reg_idx), sizeof(uint32) * 0x0000, 11, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bds_reg[reg_idx].rjct_set_oc_bd_th_exp), sizeof(uint32) * (0x048a + reg_idx), sizeof(uint32) * 0x0000, 15, 12);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bds_reg[reg_idx].rjct_clr_oc_bd_th_mnt), sizeof(uint32) * (0x048a + reg_idx), sizeof(uint32) * 0x0000, 27, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_configuration_bds_reg[reg_idx].rjct_clr_oc_bd_th_exp), sizeof(uint32) * (0x048a + reg_idx), sizeof(uint32) * 0x0000, 31, 28);
  }
  /* General Set Flow Control Configuration BDs High Priority */
  Soc_petra_a_regs.iqm.general_set_flow_control_configuration_bds_high_priority_reg.addr.base = sizeof(uint32) *0x048e;
  Soc_petra_a_regs.iqm.general_set_flow_control_configuration_bds_high_priority_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_uc_th_hp_mnt), sizeof(uint32) *0x048e, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_uc_th_hp_exp), sizeof(uint32) *0x048e, sizeof(uint32) * 0x0000, 10, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_flmc_th_hp_mnt), sizeof(uint32) *0x048e, sizeof(uint32) * 0x0000, 18, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_flmc_th_hp_exp), sizeof(uint32) *0x048e, sizeof(uint32) * 0x0000, 21, 19);

  /* General Clear Flow Control Configuration DBs High Priority */
  Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.addr.base = sizeof(uint32) *0x048f;
  Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_mnt), sizeof(uint32) *0x048f, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_exp), sizeof(uint32) *0x048f, sizeof(uint32) * 0x0000, 10, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_mnt), sizeof(uint32) *0x048f, sizeof(uint32) * 0x0000, 18, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_exp), sizeof(uint32) *0x048f, sizeof(uint32) * 0x0000, 21, 19);

  /* General Set Flow Control Configuration DBs Low Priority */
  Soc_petra_a_regs.iqm.general_set_flow_control_configuration_dbs_low_priority_reg.addr.base = sizeof(uint32) *0x0490;
  Soc_petra_a_regs.iqm.general_set_flow_control_configuration_dbs_low_priority_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_uc_th_lp_mnt), sizeof(uint32) *0x0490, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_uc_th_lp_exp), sizeof(uint32) *0x0490, sizeof(uint32) * 0x0000, 10, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_flmc_th_lp_mnt), sizeof(uint32) *0x0490, sizeof(uint32) * 0x0000, 18, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_flmc_th_lp_exp), sizeof(uint32) *0x0490, sizeof(uint32) * 0x0000, 21, 19);

  /* General Clear Flow Control Configuration DBs Low Priority */
  Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.addr.base = sizeof(uint32) *0x0491;
  Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_uc_th_lp_mnt), sizeof(uint32) *0x0491, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_uc_th_lp_exp), sizeof(uint32) *0x0491, sizeof(uint32) * 0x0000, 10, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_flmc_th_lp_mnt), sizeof(uint32) *0x0491, sizeof(uint32) * 0x0000, 18, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_flmc_th_lp_exp), sizeof(uint32) *0x0491, sizeof(uint32) * 0x0000, 21, 19);

  /* General Reject Set Configuration DBuffs   */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
  {
    Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].addr.base = sizeof(uint32) * (0x0492 + reg_idx);
    Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].rjct_set_fr_db_uc_th_mnt), sizeof(uint32) * (0x0492 + reg_idx), sizeof(uint32) * 0x0000, 6, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].rjct_set_fr_db_uc_th_exp), sizeof(uint32) * (0x0492 + reg_idx), sizeof(uint32) * 0x0000, 10, 7);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].rjct_set_fr_db_mnmc_th_mnt), sizeof(uint32) * (0x0492 + reg_idx), sizeof(uint32) * 0x0000, 16, 11);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].rjct_set_fr_db_mnmc_th_exp), sizeof(uint32) * (0x0492 + reg_idx), sizeof(uint32) * 0x0000, 20, 17);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].rjct_set_fr_db_flmc_th_mnt), sizeof(uint32) * (0x0492 + reg_idx), sizeof(uint32) * 0x0000, 28, 21);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_set_configuration_dbuffs_reg[reg_idx].rjct_set_fr_db_flmc_th_exp), sizeof(uint32) * (0x0492 + reg_idx), sizeof(uint32) * 0x0000, 31, 29);
  }

  /* General Reject Clear Configuration DBuffs  */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
  {
    Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].addr.base = sizeof(uint32) * (0x0496 + reg_idx);
    Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].rjct_clr_fr_db_uc_th_mnt), sizeof(uint32) * (0x0496 + reg_idx), sizeof(uint32) * 0x0000, 6, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].rjct_clr_fr_db_uc_th_exp), sizeof(uint32) * (0x0496 + reg_idx), sizeof(uint32) * 0x0000, 10, 7);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].rjct_clr_fr_db_mnmc_th_mnt), sizeof(uint32) * (0x0496 + reg_idx), sizeof(uint32) * 0x0000, 16, 11);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].rjct_clr_fr_db_mnmc_th_exp), sizeof(uint32) * (0x0496 + reg_idx), sizeof(uint32) * 0x0000, 20, 17);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].rjct_clr_fr_db_flmc_th_mnt), sizeof(uint32) * (0x0496 + reg_idx), sizeof(uint32) * 0x0000, 28, 21);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.general_reject_clear_configuration_dbuffs_reg[reg_idx].rjct_clr_fr_db_flmc_th_exp), sizeof(uint32) * (0x0496 + reg_idx), sizeof(uint32) * 0x0000, 31, 29);
  }

  /* Statistics Tag Configuration */
  Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.addr.base = sizeof(uint32) *0x049a;
  Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.stat_tag_vsq_msb), sizeof(uint32) *0x049a, sizeof(uint32) * 0x0000, 4, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.stat_tag_vsq_lsb), sizeof(uint32) *0x049a, sizeof(uint32) * 0x0000, 12, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.stat_tag_vsq_en), sizeof(uint32) *0x049a, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.stat_tag_deq_en), sizeof(uint32) *0x049a, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.stat_tag_dropp_en), sizeof(uint32) *0x049a, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_tag_configuration_reg.stat_tag_dropp_lsb), sizeof(uint32) *0x049a, sizeof(uint32) * 0x0000, 24, 20);

  /* Statistics Report Configurations */
  Soc_petra_a_regs.iqm.statistics_report_configurations_reg.addr.base = sizeof(uint32) *0x049b;
  Soc_petra_a_regs.iqm.statistics_report_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_bill), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_pckt_size), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_mcid_spt_mc), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_mcid_ing_rep), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_cnt_snp), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_egr_olif), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.statistics_report_configurations_reg.st_rpt_sync_prd), sizeof(uint32) *0x049b, sizeof(uint32) * 0x0000, 12, 8);

  /* Bdb Configuration */
  Soc_petra_a_regs.iqm.bdb_configuration_reg.addr.base = sizeof(uint32) *0x049c;
  Soc_petra_a_regs.iqm.bdb_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.bdb_configuration_reg.bdb_size), sizeof(uint32) *0x049c, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.bdb_configuration_reg.recycle_fr_bdb), sizeof(uint32) *0x049c, sizeof(uint32) * 0x0000, 3, 3);

  /* Internal Thesholds */
  Soc_petra_a_regs.iqm.intern_thresh_reg.addr.base = sizeof(uint32) *0x049d;
  Soc_petra_a_regs.iqm.intern_thresh_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.intern_thresh_reg.thr_0), sizeof(uint32) *0x049d, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.intern_thresh_reg.thr_1), sizeof(uint32) *0x049d, sizeof(uint32) * 0x0000, 12, 8);

  /* Reject Admission */
  Soc_petra_a_regs.iqm.reject_admission_reg.addr.base = sizeof(uint32) *0x049e;
  Soc_petra_a_regs.iqm.reject_admission_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx<SOC_PETRA_NOF_RJCT_ADMIT_TMPLTS; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.reject_admission_reg.rjct_tmplt_set[fld_idx]), sizeof(uint32) *0x049e, sizeof(uint32) * 0x0000, (uint8)((fld_idx*4)+3), (uint8)(fld_idx*4));
  }



  /* Free Unicast Dbuff Threshold0-2 */

  for (reg_idx = 0 ; reg_idx < SOC_PETRA_NOF_DBUFFS_THS ; ++reg_idx)
  {
    Soc_petra_a_regs.iqm.free_unicast_dbuff_threshold_reg[reg_idx].addr.base = sizeof(uint32) * (0x049f + reg_idx);
    Soc_petra_a_regs.iqm.free_unicast_dbuff_threshold_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_unicast_dbuff_threshold_reg[reg_idx].fr_un_db_th), sizeof(uint32) * (0x049f + reg_idx), sizeof(uint32) * 0x0000, 20, 0);
  }
  /* Free Unicast Dbuff Range Values */
  Soc_petra_a_regs.iqm.free_unicast_dbuff_range_values_reg.addr.base = sizeof(uint32) *0x04a2;
  Soc_petra_a_regs.iqm.free_unicast_dbuff_range_values_reg.addr.step = sizeof(uint32) * 0x0000;

  for (fld_idx = 0 ; fld_idx < SOC_PETRA_NOF_DBUFFS_VALS ; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_unicast_dbuff_range_values_reg.fr_un_db_rng_val[fld_idx]), sizeof(uint32) *0x04a2, sizeof(uint32) * 0x0000, (uint8)(4 * (fld_idx + 1) - 1), (uint8)(4 * fld_idx));
  }


  /* Free Full Multicast Dbuff Threshold0-2 */

  for (reg_idx = 0 ; reg_idx < SOC_PETRA_NOF_DBUFFS_THS ; ++reg_idx)
  {
    Soc_petra_a_regs.iqm.free_full_multicast_dbuff_threshold_reg[reg_idx].addr.base = sizeof(uint32) * (0x04a3 + reg_idx);
    Soc_petra_a_regs.iqm.free_full_multicast_dbuff_threshold_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_full_multicast_dbuff_threshold_reg[reg_idx].fr_fl_mul_db_th), sizeof(uint32) * (0x04a3 + reg_idx), sizeof(uint32) * 0x0000, 12, 0);
  }
  /* Free Full Multicast Dbuff Range Values */
  Soc_petra_a_regs.iqm.free_full_multicast_dbuff_range_values_reg.addr.base = sizeof(uint32) *0x04a6;
  Soc_petra_a_regs.iqm.free_full_multicast_dbuff_range_values_reg.addr.step = sizeof(uint32) * 0x0000;

  for (fld_idx = 0 ; fld_idx < SOC_PETRA_NOF_DBUFFS_VALS ; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_full_multicast_dbuff_range_values_reg.fr_fl_ml_db_rng_val[fld_idx]), sizeof(uint32) *0x04a6, sizeof(uint32) * 0x0000, (uint8)(4 * (fld_idx + 1) - 1),(uint8)(4 * fld_idx));
  }



  /* Free Bdb Threshold0-2 */
  for (reg_idx = 0 ; reg_idx < SOC_PETRA_NOF_DBUFFS_THS ; ++reg_idx)
  {
    Soc_petra_a_regs.iqm.free_bdb_threshold_reg[reg_idx].addr.base = sizeof(uint32) * (0x04a7 + reg_idx);
    Soc_petra_a_regs.iqm.free_bdb_threshold_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_bdb_threshold_reg[reg_idx].fr_bdb_th), sizeof(uint32) * (0x04a7 + reg_idx), sizeof(uint32) * 0x0000, 15, 0);
  }
  /* Free Bdb Range Values */
  Soc_petra_a_regs.iqm.free_bdb_range_values_reg.addr.base = sizeof(uint32) *0x04aa;
  Soc_petra_a_regs.iqm.free_bdb_range_values_reg.addr.step = sizeof(uint32) * 0x0000;

  for (fld_idx = 0 ; fld_idx < SOC_PETRA_NOF_DBUFFS_VALS ; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_bdb_range_values_reg.fr_bdb_rng_val[fld_idx]), sizeof(uint32) *0x04aa, sizeof(uint32) * 0x0000, (uint8)(4 * (fld_idx + 1) - 1), (uint8)(4 * fld_idx));
  }

  /* Programmable Counter Queue Select */
  Soc_petra_a_regs.iqm.programmable_counter_queue_select_reg.addr.base = sizeof(uint32) * 0x0500;
  Soc_petra_a_regs.iqm.programmable_counter_queue_select_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.programmable_counter_queue_select_reg.prg_cnt_q), sizeof(uint32) * 0x0500, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.programmable_counter_queue_select_reg.prg_cnt_msk), sizeof(uint32) * 0x0500, sizeof(uint32) * 0x0000, 31, 17);

  /* Vsq Programmable Counter Select */
  Soc_petra_a_regs.iqm.vsq_programmable_counter_select_reg.addr.base = sizeof(uint32) * 0x0501;
  Soc_petra_a_regs.iqm.vsq_programmable_counter_select_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_q), sizeof(uint32) * 0x0501, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_msk), sizeof(uint32) * 0x0501, sizeof(uint32) * 0x0000, 19, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_programmable_counter_select_reg.vsq_prg_grp_sel), sizeof(uint32) * 0x0501, sizeof(uint32) * 0x0000, 21, 20);

  /* Global Time Counter */
  Soc_petra_a_regs.iqm.global_time_counter_reg.addr.base = sizeof(uint32) * 0x0502;
  Soc_petra_a_regs.iqm.global_time_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_time_counter_reg.gt_prd), sizeof(uint32) * 0x0502, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_time_counter_reg.gt_rst_cntrs), sizeof(uint32) * 0x0502, sizeof(uint32) * 0x0000, 31, 31);

  /* Global Time Counter Trigger */
  Soc_petra_a_regs.iqm.global_time_counter_trigger_reg.addr.base = sizeof(uint32) * 0x0503;
  Soc_petra_a_regs.iqm.global_time_counter_trigger_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_time_counter_trigger_reg.glb_cnt_trg), sizeof(uint32) * 0x0503, sizeof(uint32) * 0x0000, 0, 0);

  /* Global Time Counter Configuration */
  Soc_petra_a_regs.iqm.global_time_counter_configuration_reg.addr.base = sizeof(uint32) * 0x0504;
  Soc_petra_a_regs.iqm.global_time_counter_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_time_counter_configuration_reg.iqc_cnt_by_gt), sizeof(uint32) * 0x0504, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_time_counter_configuration_reg.iqc_prg_cnt_by_gt), sizeof(uint32) * 0x0504, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_time_counter_configuration_reg.ste_cnt_by_gt), sizeof(uint32) * 0x0504, sizeof(uint32) * 0x0000, 3, 3);

  /* Global Reject State */
  Soc_petra_a_regs.iqm.global_reject_state_reg.addr.base = sizeof(uint32) * 0x0550;
  Soc_petra_a_regs.iqm.global_reject_state_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_reject_state_reg.glb_rjct_bdb), sizeof(uint32) * 0x0550, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_reject_state_reg.glb_rjct_oc_bd), sizeof(uint32) * 0x0550, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_reject_state_reg.glb_rjct_frdb_flmc), sizeof(uint32) * 0x0550, sizeof(uint32) * 0x0000, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_reject_state_reg.glb_rjct_frdb_mnmc), sizeof(uint32) * 0x0550, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_reject_state_reg.glb_rjct_frdb_uc), sizeof(uint32) * 0x0550, sizeof(uint32) * 0x0000, 19, 16);

  /* Global Flow Control State */
  Soc_petra_a_regs.iqm.global_flow_control_state_reg.addr.base = sizeof(uint32) * 0x0551;
  Soc_petra_a_regs.iqm.global_flow_control_state_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_flow_control_state_reg.glb_fc_bdb), sizeof(uint32) * 0x0551, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_flow_control_state_reg.glb_fc_frdb_uc), sizeof(uint32) * 0x0551, sizeof(uint32) * 0x0000, 3, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_flow_control_state_reg.glb_fc_frdb_flmc), sizeof(uint32) * 0x0551, sizeof(uint32) * 0x0000, 5, 4);

  /* Global Resource Counters */
  Soc_petra_a_regs.iqm.global_resource_counters_reg.addr.base = sizeof(uint32) * 0x0552;
  Soc_petra_a_regs.iqm.global_resource_counters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_resource_counters_reg.free_bdb_count), sizeof(uint32) * 0x0552, sizeof(uint32) * 0x0000, 16, 0);

  /* Global Resource Counters BD */
  Soc_petra_a_regs.iqm.global_resource_counters_bd_reg.addr.base = sizeof(uint32) * 0x0553;
  Soc_petra_a_regs.iqm.global_resource_counters_bd_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_resource_counters_bd_reg.oc_bd_count), sizeof(uint32) * 0x0553, sizeof(uint32) * 0x0000, 22, 0);

  /* Free Unicast Dbuffs Counter */
  Soc_petra_a_regs.iqm.free_unicast_dbuffs_counter_reg.addr.base = sizeof(uint32) * 0x0554;
  Soc_petra_a_regs.iqm.free_unicast_dbuffs_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_unicast_dbuffs_counter_reg.fr_db_count_uc), sizeof(uint32) * 0x0554, sizeof(uint32) * 0x0000, 21, 0);

  /* Free Full Multicast Dbuffs Counter */
  Soc_petra_a_regs.iqm.free_full_multicast_dbuffs_counter_reg.addr.base = sizeof(uint32) * 0x0555;
  Soc_petra_a_regs.iqm.free_full_multicast_dbuffs_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_full_multicast_dbuffs_counter_reg.fr_db_count_flmc), sizeof(uint32) * 0x0555, sizeof(uint32) * 0x0000, 13, 0);

  /* Free Mini Multicast Dbuffs Counter */
  Soc_petra_a_regs.iqm.free_mini_multicast_dbuffs_counter_reg.addr.base = sizeof(uint32) * 0x0556;
  Soc_petra_a_regs.iqm.free_mini_multicast_dbuffs_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_mini_multicast_dbuffs_counter_reg.fr_db_count_mnmc), sizeof(uint32) * 0x0556, sizeof(uint32) * 0x0000, 16, 0);

  /* Global Resource Minimum Occupancy */
  Soc_petra_a_regs.iqm.global_resource_minimum_occupancy_reg.addr.base = sizeof(uint32) * 0x0557;
  Soc_petra_a_regs.iqm.global_resource_minimum_occupancy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.global_resource_minimum_occupancy_reg.free_bdb_min_oc), sizeof(uint32) * 0x0557, sizeof(uint32) * 0x0000, 16, 0);

  /* Free Unicast Dbuffs Minimum Occupancy */
  Soc_petra_a_regs.iqm.free_unicast_dbuffs_minimum_occupancy_reg.addr.base = sizeof(uint32) * 0x0558;
  Soc_petra_a_regs.iqm.free_unicast_dbuffs_minimum_occupancy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_unicast_dbuffs_minimum_occupancy_reg.fr_db_min_oc), sizeof(uint32) * 0x0558, sizeof(uint32) * 0x0000, 21, 0);

  /* Free Full Multicast Dbuffs Minimum Occupancy */
  Soc_petra_a_regs.iqm.free_full_multicast_dbuffs_minimum_occupancy_reg.addr.base = sizeof(uint32) * 0x0559;
  Soc_petra_a_regs.iqm.free_full_multicast_dbuffs_minimum_occupancy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_full_multicast_dbuffs_minimum_occupancy_reg.fr_db_flmc_min_oc), sizeof(uint32) * 0x0559, sizeof(uint32) * 0x0000, 13, 0);

  /* Free Mini Multicast Dbuffs Minimum Occupancy */
  Soc_petra_a_regs.iqm.free_mini_multicast_dbuffs_minimum_occupancy_reg.addr.base = sizeof(uint32) * 0x055a;
  Soc_petra_a_regs.iqm.free_mini_multicast_dbuffs_minimum_occupancy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_mini_multicast_dbuffs_minimum_occupancy_reg.fr_db_mnmc_min_oc), sizeof(uint32) * 0x055a, sizeof(uint32) * 0x0000, 16, 0);

  /* Enqueue Packet Counter */
  Soc_petra_a_regs.iqm.enqueue_packet_counter_reg.addr.base = sizeof(uint32) * 0x055b;
  Soc_petra_a_regs.iqm.enqueue_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.enqueue_packet_counter_reg.enq_pkt_cnt), sizeof(uint32) * 0x055b, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.enqueue_packet_counter_reg.enq_pkt_cnt_ovf), sizeof(uint32) * 0x055b, sizeof(uint32) * 0x0000, 31, 31);

  /* Dequeue Packet Counter */
  Soc_petra_a_regs.iqm.dequeue_packet_counter_reg.addr.base = sizeof(uint32) * 0x055c;
  Soc_petra_a_regs.iqm.dequeue_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.dequeue_packet_counter_reg.deq_pkt_cnt), sizeof(uint32) * 0x055c, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.dequeue_packet_counter_reg.deq_pkt_cnt_ovf), sizeof(uint32) * 0x055c, sizeof(uint32) * 0x0000, 31, 31);

  /* Total Discarded Packet Counter */
  Soc_petra_a_regs.iqm.total_discarded_packet_counter_reg.addr.base = sizeof(uint32) * 0x055d;
  Soc_petra_a_regs.iqm.total_discarded_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.total_discarded_packet_counter_reg.tot_dscrd_pkt_cnt), sizeof(uint32) * 0x055d, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.total_discarded_packet_counter_reg.tot_dscrd_pkt_cnt_ovf), sizeof(uint32) * 0x055d, sizeof(uint32) * 0x0000, 31, 31);

  /* Deleted Packet Counter */
  Soc_petra_a_regs.iqm.deleted_packet_counter_reg.addr.base = sizeof(uint32) * 0x055e;
  Soc_petra_a_regs.iqm.deleted_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.deleted_packet_counter_reg.deq_delete_pkt_cnt), sizeof(uint32) * 0x055e, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.deleted_packet_counter_reg.deq_delete_pkt_cnt_ovf), sizeof(uint32) * 0x055e, sizeof(uint32) * 0x0000, 31, 31);

  /* Isp Packet Counter */
  Soc_petra_a_regs.iqm.isp_packet_counter_reg.addr.base = sizeof(uint32) * 0x055f;
  Soc_petra_a_regs.iqm.isp_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.isp_packet_counter_reg.isp_pkt_cnt), sizeof(uint32) * 0x055f, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.isp_packet_counter_reg.isp_pkt_cnt_ovf), sizeof(uint32) * 0x055f, sizeof(uint32) * 0x0000, 31, 31);

  /* Queue Enqueue Packet Counter */
  Soc_petra_a_regs.iqm.queue_enqueue_packet_counter_reg.addr.base = sizeof(uint32) * 0x0560;
  Soc_petra_a_regs.iqm.queue_enqueue_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_enqueue_packet_counter_reg.qenq_pkt_cnt), sizeof(uint32) * 0x0560, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_enqueue_packet_counter_reg.qenq_pkt_cnt_ovf), sizeof(uint32) * 0x0560, sizeof(uint32) * 0x0000, 31, 31);

  /* Queue Dequeue Packet Counter */
  Soc_petra_a_regs.iqm.queue_dequeue_packet_counter_reg.addr.base = sizeof(uint32) * 0x0561;
  Soc_petra_a_regs.iqm.queue_dequeue_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_dequeue_packet_counter_reg.qdeq_pkt_cnt), sizeof(uint32) * 0x0561, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_dequeue_packet_counter_reg.qdeq_pkt_cnt_ovf), sizeof(uint32) * 0x0561, sizeof(uint32) * 0x0000, 31, 31);

  /* Queue Total Discarded Packet Counter */
  Soc_petra_a_regs.iqm.queue_total_discarded_packet_counter_reg.addr.base = sizeof(uint32) * 0x0562;
  Soc_petra_a_regs.iqm.queue_total_discarded_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_total_discarded_packet_counter_reg.qtot_dscrd_pkt_cnt), sizeof(uint32) * 0x0562, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_total_discarded_packet_counter_reg.qtot_dscrd_pkt_cnt_ovf), sizeof(uint32) * 0x0562, sizeof(uint32) * 0x0000, 31, 31);

  /* Queue Deleted Packet Counter */
  Soc_petra_a_regs.iqm.queue_deleted_packet_counter_reg.addr.base = sizeof(uint32) * 0x0563;
  Soc_petra_a_regs.iqm.queue_deleted_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_deleted_packet_counter_reg.qdeq_delete_pkt_cnt), sizeof(uint32) * 0x0563, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_deleted_packet_counter_reg.qdeq_delete_pkt_cnt_ovf), sizeof(uint32) * 0x0563, sizeof(uint32) * 0x0000, 31, 31);

  /* Free Resource Rejected Packet Counter1 */
  Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter1_reg.addr.base = sizeof(uint32) * 0x0564;
  Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter1_reg.rjct_db_pkt_cnt), sizeof(uint32) * 0x0564, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter1_reg.rjct_db_pkt_cnt_ovf), sizeof(uint32) * 0x0564, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter1_reg.rjct_bdb_pkt_cnt), sizeof(uint32) * 0x0564, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter1_reg.rjct_bdb_pkt_cnt_ovf), sizeof(uint32) * 0x0564, sizeof(uint32) * 0x0000, 31, 31);

  /* Free Resource Rejected Packet Counter2 */
  Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter2_reg.addr.base = sizeof(uint32) * 0x0565;
  Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter2_reg.rjct_bdb_protct_pkt_cnt), sizeof(uint32) * 0x0565, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter2_reg.rjct_bdb_protct_pkt_cnt_ovf), sizeof(uint32) * 0x0565, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter2_reg.rjct_oc_bd_pkt_cnt), sizeof(uint32) * 0x0565, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.free_resource_rejected_packet_counter2_reg.rjct_oc_bd_pkt_cnt_ovf), sizeof(uint32) * 0x0565, sizeof(uint32) * 0x0000, 31, 31);

  /* Irr Error Rejected Packet Counters  */
  Soc_petra_a_regs.iqm.irr_error_rejected_packet_counters_reg.addr.base = sizeof(uint32) * 0x0566;
  Soc_petra_a_regs.iqm.irr_error_rejected_packet_counters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.irr_error_rejected_packet_counters_reg.rjct_sn_err_pkt_cnt), sizeof(uint32) * 0x0566, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.irr_error_rejected_packet_counters_reg.rjct_sn_err_pkt_cnt_ovf), sizeof(uint32) * 0x0566, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.irr_error_rejected_packet_counters_reg.rjct_mc_err_pkt_cnt), sizeof(uint32) * 0x0566, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.irr_error_rejected_packet_counters_reg.rjct_mc_err_pkt_cnt_ovf), sizeof(uint32) * 0x0566, sizeof(uint32) * 0x0000, 31, 31);

  /* Idr Error Reject Packet Counters  */
  Soc_petra_a_regs.iqm.idr_error_reject_packet_counters_reg.addr.base = sizeof(uint32) * 0x0567;
  Soc_petra_a_regs.iqm.idr_error_reject_packet_counters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.idr_error_reject_packet_counters_reg.rjct_rsrc_err_pkt_cnt), sizeof(uint32) * 0x0567, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.idr_error_reject_packet_counters_reg.rjct_rsrc_err_pkt_cnt_ovf), sizeof(uint32) * 0x0567, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.idr_error_reject_packet_counters_reg.rjct_qnvalid_err_pkt_cnt), sizeof(uint32) * 0x0567, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.idr_error_reject_packet_counters_reg.rjct_qnvalid_err_pkt_cnt_ovf), sizeof(uint32) * 0x0567, sizeof(uint32) * 0x0000, 31, 31);

  /* Maximum Occupancy Queue Size0 */
  Soc_petra_a_regs.iqm.maximum_occupancy_queue_size0_reg.addr.base = sizeof(uint32) * 0x0568;
  Soc_petra_a_regs.iqm.maximum_occupancy_queue_size0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.maximum_occupancy_queue_size0_reg.mx_oc_qsz), sizeof(uint32) * 0x0568, sizeof(uint32) * 0x0000, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.maximum_occupancy_queue_size0_reg.mx_oc_rfrsh), sizeof(uint32) * 0x0568, sizeof(uint32) * 0x0000, 31, 31);

  /* Maximum Occupancy Queue Size1 */
  Soc_petra_a_regs.iqm.maximum_occupancy_queue_size1_reg.addr.base = sizeof(uint32) * 0x0569;
  Soc_petra_a_regs.iqm.maximum_occupancy_queue_size1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.maximum_occupancy_queue_size1_reg.mx_oc_qnum), sizeof(uint32) * 0x0569, sizeof(uint32) * 0x0000, 14, 0);

  /* Queue Maximum Occupancy Queue Size0 */
  Soc_petra_a_regs.iqm.queue_maximum_occupancy_queue_size0_reg.addr.base = sizeof(uint32) * 0x056a;
  Soc_petra_a_regs.iqm.queue_maximum_occupancy_queue_size0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_maximum_occupancy_queue_size0_reg.qmx_oc_qsz), sizeof(uint32) * 0x056a, sizeof(uint32) * 0x0000, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.queue_maximum_occupancy_queue_size0_reg.qmx_oc_rfrsh), sizeof(uint32) * 0x056a, sizeof(uint32) * 0x0000, 31, 31);

  /* Vsq Maximum Occupancy0 */
  Soc_petra_a_regs.iqm.vsq_maximum_occupancy0_reg.addr.base = sizeof(uint32) * 0x056b;
  Soc_petra_a_regs.iqm.vsq_maximum_occupancy0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_maximum_occupancy0_reg.vsq_mx_oc_qsz), sizeof(uint32) * 0x056b, sizeof(uint32) * 0x0000, 23, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_maximum_occupancy0_reg.vsq_mx_oc_rfrsh), sizeof(uint32) * 0x056b, sizeof(uint32) * 0x0000, 31, 31);

  /* Vsq Maximum Occupancy1 */
  Soc_petra_a_regs.iqm.vsq_maximum_occupancy1_reg.addr.base = sizeof(uint32) * 0x056c;
  Soc_petra_a_regs.iqm.vsq_maximum_occupancy1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_maximum_occupancy1_reg.vsq_mx_oc_qnum), sizeof(uint32) * 0x056c, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_maximum_occupancy1_reg.vsq_mx_oc_bdsz), sizeof(uint32) * 0x056c, sizeof(uint32) * 0x0000, 30, 9);

  /* Vsq Enqueue Packet Counter */
  Soc_petra_a_regs.iqm.vsq_enqueue_packet_counter_reg.addr.base = sizeof(uint32) * 0x056d;
  Soc_petra_a_regs.iqm.vsq_enqueue_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_enqueue_packet_counter_reg.vsq_enq_pkt_cnt), sizeof(uint32) * 0x056d, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_enqueue_packet_counter_reg.vsq_enq_pkt_cnt_ovf), sizeof(uint32) * 0x056d, sizeof(uint32) * 0x0000, 31, 31);

  /* Vsq Dequeue Packet Counter */
  Soc_petra_a_regs.iqm.vsq_dequeue_packet_counter_reg.addr.base = sizeof(uint32) * 0x056e;
  Soc_petra_a_regs.iqm.vsq_dequeue_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_dequeue_packet_counter_reg.vsq_deq_pkt_cnt), sizeof(uint32) * 0x056e, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.vsq_dequeue_packet_counter_reg.vsq_deq_pkt_cnt_ovf), sizeof(uint32) * 0x056e, sizeof(uint32) * 0x0000, 31, 31);

  /* Qdc Tag Ctrl Dbg Sel */
  Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_sel_reg.addr.base = sizeof(uint32) * 0x056f;
  Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_sel_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_sel_reg.tag_ctl_dbg_sel), sizeof(uint32) * 0x056f, sizeof(uint32) * 0x0000, 4, 0);

  /* Qdc Tag Ctrl Dbg */
  Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.addr.base = sizeof(uint32) * 0x0570;
  Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.tag_dbg_free), sizeof(uint32) * 0x0570, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.tag_dbg_line_v), sizeof(uint32) * 0x0570, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.tag_dbg_dirty), sizeof(uint32) * 0x0570, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.tag_dbg_user_cnt), sizeof(uint32) * 0x0570, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.tag_dbg_rd_pnd), sizeof(uint32) * 0x0570, sizeof(uint32) * 0x0000, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.iqm.qdc_tag_ctrl_dbg_reg.tag_dbg_qnum), sizeof(uint32) * 0x0570, sizeof(uint32) * 0x0000, 26, 12);

}
/* Block registers initialization: QDR */
STATIC void
  soc_pa_regs_init_QDR(void)
{
  uint32
    reg_idx;

  Soc_petra_a_regs.qdr.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_QDR;
  Soc_petra_a_regs.qdr.addr.base = sizeof(uint32) * 0x0600;
  Soc_petra_a_regs.qdr.addr.step = sizeof(uint32) * 0x0000;

  /* InterruptRegister */
  Soc_petra_a_regs.qdr.interrupt_reg.addr.base = sizeof(uint32) * 0x0600;
  Soc_petra_a_regs.qdr.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.interrupt_reg.qdr_ecc_int), sizeof(uint32) * 0x0600, sizeof(uint32) * 0x0000, 0, 0);

  /* InterruptMaskRegister */
  Soc_petra_a_regs.qdr.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x0610;
  Soc_petra_a_regs.qdr.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.interrupt_mask_reg.qdr_ecc_int_mask), sizeof(uint32) * 0x0610, sizeof(uint32) * 0x0000, 0, 0);

  /* IndirectCommandWrData */
  Soc_petra_a_regs.qdr.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x0620;
  Soc_petra_a_regs.qdr.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x0620, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommandRdData */
  Soc_petra_a_regs.qdr.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x0630;
  Soc_petra_a_regs.qdr.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x0630, sizeof(uint32) * 0x0000, 31, 0);

  /* IndirectCommand */
  Soc_petra_a_regs.qdr.indirect_command_reg.addr.base = sizeof(uint32) * 0x0640;
  Soc_petra_a_regs.qdr.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x0640, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x0640, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x0640, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x0640, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x0640, sizeof(uint32) * 0x0000, 31, 31);

  /* IndirectCommandAddress */
  Soc_petra_a_regs.qdr.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x0641;
  Soc_petra_a_regs.qdr.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x0641, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x0641, sizeof(uint32) * 0x0000, 31, 31);

  /* QdrEccErrorCounter */
  Soc_petra_a_regs.qdr.qdr_ecc_error_counter_reg.addr.base = sizeof(uint32) * 0x0660;
  Soc_petra_a_regs.qdr.qdr_ecc_error_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_ecc_error_counter_reg.qdr_ecc_err_cnt), sizeof(uint32) * 0x0660, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_ecc_error_counter_reg.qdr_ecc_err_cnt_ovf), sizeof(uint32) * 0x0660, sizeof(uint32) * 0x0000, 31, 31);

  /* ReplyThreshold */
  Soc_petra_a_regs.qdr.reply_threshold_reg.addr.base = sizeof(uint32) * 0x0661;
  Soc_petra_a_regs.qdr.reply_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.reply_threshold_reg.reply_th), sizeof(uint32) * 0x0661, sizeof(uint32) * 0x0000, 5, 0);

  /* DllUpdatePeriod */
  Soc_petra_a_regs.qdr.dll_update_period_reg.addr.base = sizeof(uint32) * 0x0662;
  Soc_petra_a_regs.qdr.dll_update_period_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.dll_update_period_reg.dll_update_prd), sizeof(uint32) * 0x0662, sizeof(uint32) * 0x0000, 7, 0);

  /* QdrProtectDisable */
  Soc_petra_a_regs.qdr.qdr_protect_disable_reg.addr.base = sizeof(uint32) * 0x0663;
  Soc_petra_a_regs.qdr.qdr_protect_disable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_protect_disable_reg.qdr_protect_dis), sizeof(uint32) * 0x0663, sizeof(uint32) * 0x0000, 0, 0);

  /* QdrEccBitCorrectionCounter */
  Soc_petra_a_regs.qdr.qdr_ecc_bit_correction_counter_reg.addr.base = sizeof(uint32) * 0x0664;
  Soc_petra_a_regs.qdr.qdr_ecc_bit_correction_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_ecc_bit_correction_counter_reg.qdr_ecc_bit_corr_cnt), sizeof(uint32) * 0x0664, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_ecc_bit_correction_counter_reg.qdr_ecc_bit_corr_cnt_ovf), sizeof(uint32) * 0x0664, sizeof(uint32) * 0x0000, 31, 31);

  /* QdrBistEnable */
  Soc_petra_a_regs.qdr.qdr_bist_enable_reg.addr.base = sizeof(uint32) * 0x0667;
  Soc_petra_a_regs.qdr.qdr_bist_enable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_bist_enable_reg.qdr_bist_en), sizeof(uint32) * 0x0667, sizeof(uint32) * 0x0000, 0, 0);

  /* BistWriteNumberConfigurationRegister */
  Soc_petra_a_regs.qdr.bist_write_number_configuration_reg.addr.base = sizeof(uint32) * 0x0668;
  Soc_petra_a_regs.qdr.bist_write_number_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_write_number_configuration_reg.bist_write_num), sizeof(uint32) * 0x0668, sizeof(uint32) * 0x0000, 31, 0);

  /* BistReadNumberConfigurationRegister */
  Soc_petra_a_regs.qdr.bist_read_number_configuration_reg.addr.base = sizeof(uint32) * 0x0669;
  Soc_petra_a_regs.qdr.bist_read_number_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_read_number_configuration_reg.bist_read_num), sizeof(uint32) * 0x0669, sizeof(uint32) * 0x0000, 31, 0);

  /* BistInfiniteTest */
  Soc_petra_a_regs.qdr.bist_infinite_test_reg.addr.base = sizeof(uint32) * 0x066a;
  Soc_petra_a_regs.qdr.bist_infinite_test_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_infinite_test_reg.bist_write_infinite), sizeof(uint32) * 0x066a, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_infinite_test_reg.bist_read_infinite), sizeof(uint32) * 0x066a, sizeof(uint32) * 0x0000, 4, 4);

  /* BistReadDelay */
  Soc_petra_a_regs.qdr.bist_read_delay_reg.addr.base = sizeof(uint32) * 0x066b;
  Soc_petra_a_regs.qdr.bist_read_delay_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_read_delay_reg.bist_read_delay), sizeof(uint32) * 0x066b, sizeof(uint32) * 0x0000, 31, 0);

  /* BistThresholds */
  Soc_petra_a_regs.qdr.bist_thresholds_reg.addr.base = sizeof(uint32) * 0x066c;
  Soc_petra_a_regs.qdr.bist_thresholds_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_thresholds_reg.bist_wr_th), sizeof(uint32) * 0x066c, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_thresholds_reg.bist_rd_th), sizeof(uint32) * 0x066c, sizeof(uint32) * 0x0000, 11, 8);

  /* BistGap */
  Soc_petra_a_regs.qdr.bist_gap_reg.addr.base = sizeof(uint32) * 0x066e;
  Soc_petra_a_regs.qdr.bist_gap_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_gap_reg.bist_gap), sizeof(uint32) * 0x066e, sizeof(uint32) * 0x0000, 7, 0);

  /* BistPatterns */
  for (reg_idx = 0; reg_idx < SOC_PETRA_QDR_NOF_PATTERN_REGS; ++reg_idx)
  {
    Soc_petra_a_regs.qdr.bist_pattern_regs[reg_idx].addr.base = sizeof(uint32) * (0x066f + reg_idx);
    Soc_petra_a_regs.qdr.bist_pattern_regs[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern_regs[reg_idx].bist_pattern), sizeof(uint32) * (0x066f + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* BistPattern0 */
  Soc_petra_a_regs.qdr.bist_pattern0_reg.addr.base = sizeof(uint32) * 0x066f;
  Soc_petra_a_regs.qdr.bist_pattern0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern0_reg.bist_pattern0), sizeof(uint32) * 0x066f, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern1 */
  Soc_petra_a_regs.qdr.bist_pattern1_reg.addr.base = sizeof(uint32) * 0x0670;
  Soc_petra_a_regs.qdr.bist_pattern1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern1_reg.bist_pattern1), sizeof(uint32) * 0x0670, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern2 */
  Soc_petra_a_regs.qdr.bist_pattern2_reg.addr.base = sizeof(uint32) * 0x0671;
  Soc_petra_a_regs.qdr.bist_pattern2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern2_reg.bist_pattern2), sizeof(uint32) * 0x0671, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern3 */
  Soc_petra_a_regs.qdr.bist_pattern3_reg.addr.base = sizeof(uint32) * 0x0672;
  Soc_petra_a_regs.qdr.bist_pattern3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern3_reg.bist_pattern3), sizeof(uint32) * 0x0672, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern4 */
  Soc_petra_a_regs.qdr.bist_pattern4_reg.addr.base = sizeof(uint32) * 0x0673;
  Soc_petra_a_regs.qdr.bist_pattern4_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern4_reg.bist_pattern4), sizeof(uint32) * 0x0673, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern5 */
  Soc_petra_a_regs.qdr.bist_pattern5_reg.addr.base = sizeof(uint32) * 0x0674;
  Soc_petra_a_regs.qdr.bist_pattern5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern5_reg.bist_pattern5), sizeof(uint32) * 0x0674, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern6 */
  Soc_petra_a_regs.qdr.bist_pattern6_reg.addr.base = sizeof(uint32) * 0x0675;
  Soc_petra_a_regs.qdr.bist_pattern6_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern6_reg.bist_pattern6), sizeof(uint32) * 0x0675, sizeof(uint32) * 0x0000, 31, 0);

  /* BistPattern7 */
  Soc_petra_a_regs.qdr.bist_pattern7_reg.addr.base = sizeof(uint32) * 0x0676;
  Soc_petra_a_regs.qdr.bist_pattern7_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_pattern7_reg.bist_pattern7), sizeof(uint32) * 0x0676, sizeof(uint32) * 0x0000, 31, 0);

  /* BistStartAddress */
  Soc_petra_a_regs.qdr.bist_start_address_reg.addr.base = sizeof(uint32) * 0x0677;
  Soc_petra_a_regs.qdr.bist_start_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_start_address_reg.bist_start_address), sizeof(uint32) * 0x0677, sizeof(uint32) * 0x0000, 21, 0);

  /* BistEndAddress */
  Soc_petra_a_regs.qdr.bist_end_address_reg.addr.base = sizeof(uint32) * 0x0678;
  Soc_petra_a_regs.qdr.bist_end_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_end_address_reg.bist_end_address), sizeof(uint32) * 0x0678, sizeof(uint32) * 0x0000, 21, 0);

  /* BistOffsetAddress */
  Soc_petra_a_regs.qdr.bist_offset_address_reg.addr.base = sizeof(uint32) * 0x0679;
  Soc_petra_a_regs.qdr.bist_offset_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_offset_address_reg.bist_offset_address), sizeof(uint32) * 0x0679, sizeof(uint32) * 0x0000, 21, 0);

  /* BistTestMode */
  Soc_petra_a_regs.qdr.bist_test_mode_reg.addr.base = sizeof(uint32) * 0x067a;
  Soc_petra_a_regs.qdr.bist_test_mode_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_test_mode_reg.bist_pattern_bit_mode), sizeof(uint32) * 0x067a, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_test_mode_reg.bist_data_random_mode), sizeof(uint32) * 0x067a, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_test_mode_reg.bist_data_shift_mode), sizeof(uint32) * 0x067a, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_test_mode_reg.bist_address_shift_mode), sizeof(uint32) * 0x067a, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_test_mode_reg.bist_address_test), sizeof(uint32) * 0x067a, sizeof(uint32) * 0x0000, 16, 16);

  /* BistAddressBitsNumber */
  Soc_petra_a_regs.qdr.bist_address_bits_number_reg.addr.base = sizeof(uint32) * 0x067b;
  Soc_petra_a_regs.qdr.bist_address_bits_number_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_address_bits_number_reg.bist_addr_bit_num), sizeof(uint32) * 0x067b, sizeof(uint32) * 0x0000, 4, 0);

  /* BistDataShiftModOffset */
  Soc_petra_a_regs.qdr.bist_data_shift_mod_offset_reg.addr.base = sizeof(uint32) * 0x067c;
  Soc_petra_a_regs.qdr.bist_data_shift_mod_offset_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_data_shift_mod_offset_reg.bist_data_shift_mod_offset), sizeof(uint32) * 0x067c, sizeof(uint32) * 0x0000, 4, 0);

  /* BistBurstMask0 */
  Soc_petra_a_regs.qdr.bist_burst_mask0_reg.addr.base = sizeof(uint32) * 0x067d;
  Soc_petra_a_regs.qdr.bist_burst_mask0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_burst_mask0_reg.bist_burst_mask0), sizeof(uint32) * 0x067d, sizeof(uint32) * 0x0000, 17, 0);

  /* BistBurstMask1 */
  Soc_petra_a_regs.qdr.bist_burst_mask1_reg.addr.base = sizeof(uint32) * 0x067e;
  Soc_petra_a_regs.qdr.bist_burst_mask1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_burst_mask1_reg.bist_burst_mask1), sizeof(uint32) * 0x067e, sizeof(uint32) * 0x0000, 17, 0);

  /* BistBitMask */
  Soc_petra_a_regs.qdr.bist_bit_mask_reg.addr.base = sizeof(uint32) * 0x067f;
  Soc_petra_a_regs.qdr.bist_bit_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_bit_mask_reg.bist_bit_mask), sizeof(uint32) * 0x067f, sizeof(uint32) * 0x0000, 17, 0);

  /* BistErrorBurstCounter */
  Soc_petra_a_regs.qdr.bist_error_burst_counter_reg.addr.base = sizeof(uint32) * 0x0680;
  Soc_petra_a_regs.qdr.bist_error_burst_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_error_burst_counter_reg.bist_err_burst_counter), sizeof(uint32) * 0x0680, sizeof(uint32) * 0x0000, 31, 0);

  /* BistErrorBitCounter */
  Soc_petra_a_regs.qdr.bist_error_bit_counter_reg.addr.base = sizeof(uint32) * 0x0681;
  Soc_petra_a_regs.qdr.bist_error_bit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_error_bit_counter_reg.bist_err_bit_counter), sizeof(uint32) * 0x0681, sizeof(uint32) * 0x0000, 31, 0);

  /* BistErrorOccurred */
  Soc_petra_a_regs.qdr.bist_error_occurred_reg.addr.base = sizeof(uint32) * 0x0682;
  Soc_petra_a_regs.qdr.bist_error_occurred_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_error_occurred_reg.bist_err_occurred), sizeof(uint32) * 0x0682, sizeof(uint32) * 0x0000, 17, 0);

  /* BistGlobalErrorCounter */
  Soc_petra_a_regs.qdr.bist_global_error_counter_reg.addr.base = sizeof(uint32) * 0x0683;
  Soc_petra_a_regs.qdr.bist_global_error_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_global_error_counter_reg.bist_global_err_counter), sizeof(uint32) * 0x0683, sizeof(uint32) * 0x0000, 31, 0);

  /* BistErrorAddress */
  Soc_petra_a_regs.qdr.bist_error_address_reg.addr.base = sizeof(uint32) * 0x0684;
  Soc_petra_a_regs.qdr.bist_error_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_error_address_reg.bist_err_address), sizeof(uint32) * 0x0684, sizeof(uint32) * 0x0000, 21, 0);

  /* BistErrorData1 */
  Soc_petra_a_regs.qdr.bist_error_data1_reg.addr.base = sizeof(uint32) * 0x0685;
  Soc_petra_a_regs.qdr.bist_error_data1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_error_data1_reg.bist_err_data1), sizeof(uint32) * 0x0685, sizeof(uint32) * 0x0000, 17, 0);

  /* BistErrorData2 */
  Soc_petra_a_regs.qdr.bist_error_data2_reg.addr.base = sizeof(uint32) * 0x0686;
  Soc_petra_a_regs.qdr.bist_error_data2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_error_data2_reg.bist_err_data2), sizeof(uint32) * 0x0686, sizeof(uint32) * 0x0000, 17, 0);

  /* BistFinished */
  Soc_petra_a_regs.qdr.bist_finished_reg.addr.base = sizeof(uint32) * 0x0687;
  Soc_petra_a_regs.qdr.bist_finished_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.bist_finished_reg.bist_finished), sizeof(uint32) * 0x0687, sizeof(uint32) * 0x0000, 0, 0);

  /* QdrDllControl */
  Soc_petra_a_regs.qdr.qdr_dll_control_reg.addr.base = sizeof(uint32) * 0x0688;
  Soc_petra_a_regs.qdr.qdr_dll_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dll_control_reg.dll_offn), sizeof(uint32) * 0x0688, sizeof(uint32) * 0x0000, 0, 0);

  /* QdrControllerResetRegister */
  Soc_petra_a_regs.qdr.qdr_controller_reset_reg.addr.base = sizeof(uint32) * 0x0689;
  Soc_petra_a_regs.qdr.qdr_controller_reset_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_controller_reset_reg.qdrc_rst_n), sizeof(uint32) * 0x0689, sizeof(uint32) * 0x0000, 0, 0);

  /* QdrDummyData0 */
  Soc_petra_a_regs.qdr.qdr_dummy_data0_reg.addr.base = sizeof(uint32) * 0x068a;
  Soc_petra_a_regs.qdr.qdr_dummy_data0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data0_reg.dummy_data0), sizeof(uint32) * 0x068a, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData1 */
  Soc_petra_a_regs.qdr.qdr_dummy_data1_reg.addr.base = sizeof(uint32) * 0x068b;
  Soc_petra_a_regs.qdr.qdr_dummy_data1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data1_reg.dummy_data1), sizeof(uint32) * 0x068b, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData2 */
  Soc_petra_a_regs.qdr.qdr_dummy_data2_reg.addr.base = sizeof(uint32) * 0x068c;
  Soc_petra_a_regs.qdr.qdr_dummy_data2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data2_reg.dummy_data2), sizeof(uint32) * 0x068c, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData3 */
  Soc_petra_a_regs.qdr.qdr_dummy_data3_reg.addr.base = sizeof(uint32) * 0x068d;
  Soc_petra_a_regs.qdr.qdr_dummy_data3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data3_reg.dummy_data3), sizeof(uint32) * 0x068d, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData4 */
  Soc_petra_a_regs.qdr.qdr_dummy_data4_reg.addr.base = sizeof(uint32) * 0x068e;
  Soc_petra_a_regs.qdr.qdr_dummy_data4_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data4_reg.dummy_data4), sizeof(uint32) * 0x068e, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData5 */
  Soc_petra_a_regs.qdr.qdr_dummy_data5_reg.addr.base = sizeof(uint32) * 0x068f;
  Soc_petra_a_regs.qdr.qdr_dummy_data5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data5_reg.dummy_data5), sizeof(uint32) * 0x068f, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData6 */
  Soc_petra_a_regs.qdr.qdr_dummy_data6_reg.addr.base = sizeof(uint32) * 0x0690;
  Soc_petra_a_regs.qdr.qdr_dummy_data6_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data6_reg.dummy_data6), sizeof(uint32) * 0x0690, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrDummyData7 */
  Soc_petra_a_regs.qdr.qdr_dummy_data7_reg.addr.base = sizeof(uint32) * 0x0691;
  Soc_petra_a_regs.qdr.qdr_dummy_data7_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_dummy_data7_reg.dummy_data7), sizeof(uint32) * 0x0691, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrSeekData1 */
  Soc_petra_a_regs.qdr.qdr_seek_data1_reg.addr.base = sizeof(uint32) * 0x0692;
  Soc_petra_a_regs.qdr.qdr_seek_data1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_seek_data1_reg.seek_data1), sizeof(uint32) * 0x0692, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrSeekData2 */
  Soc_petra_a_regs.qdr.qdr_seek_data2_reg.addr.base = sizeof(uint32) * 0x0693;
  Soc_petra_a_regs.qdr.qdr_seek_data2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_seek_data2_reg.seek_data2), sizeof(uint32) * 0x0693, sizeof(uint32) * 0x0000, 17, 0);

  /* QdrCalibrationConfigurationRegister */
  Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.addr.base = sizeof(uint32) * 0x0694;
  Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.pre_read), sizeof(uint32) * 0x0694, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.post_read), sizeof(uint32) * 0x0694, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.use_dummy_data), sizeof(uint32) * 0x0694, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.max_seek_cnt), sizeof(uint32) * 0x0694, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_configuration_reg.max_iteration), sizeof(uint32) * 0x0694, sizeof(uint32) * 0x0000, 19, 16);

  /* QdrCalibrationTrigger */
  Soc_petra_a_regs.qdr.qdr_calibration_trigger_reg.addr.base = sizeof(uint32) * 0x0695;
  Soc_petra_a_regs.qdr.qdr_calibration_trigger_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_trigger_reg.seek_trigger), sizeof(uint32) * 0x0695, sizeof(uint32) * 0x0000, 0, 0);

  /* QdrCalibrationBypass */
  Soc_petra_a_regs.qdr.qdr_calibration_bypass_reg.addr.base = sizeof(uint32) * 0x0696;
  Soc_petra_a_regs.qdr.qdr_calibration_bypass_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_bypass_reg.set_delay), sizeof(uint32) * 0x0696, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_calibration_bypass_reg.bypass_calib), sizeof(uint32) * 0x0696, sizeof(uint32) * 0x0000, 4, 4);

  /* QdrReplyLock */
  Soc_petra_a_regs.qdr.qdr_reply_lock_reg.addr.base = sizeof(uint32) * 0x0697;
  Soc_petra_a_regs.qdr.qdr_reply_lock_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_reply_lock_reg.reply_lock), sizeof(uint32) * 0x0697, sizeof(uint32) * 0x0000, 0, 0);

  /* QdrReplyDelay */
  Soc_petra_a_regs.qdr.qdr_reply_delay_reg.addr.base = sizeof(uint32) * 0x0698;
  Soc_petra_a_regs.qdr.qdr_reply_delay_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_reply_delay_reg.reply_delay), sizeof(uint32) * 0x0698, sizeof(uint32) * 0x0000, 3, 0);

  /* QdrPhyReset */
  Soc_petra_a_regs.qdr.qdr_phy_reset_reg.addr.base = sizeof(uint32) * 0x069a;
  Soc_petra_a_regs.qdr.qdr_phy_reset_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_phy_reset_reg.qdio_reset), sizeof(uint32) * 0x069a, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_phy_reset_reg.ddl_init_reset), sizeof(uint32) * 0x069a, sizeof(uint32) * 0x0000, 8, 8);

  /* QdrInitStatus */
  Soc_petra_a_regs.qdr.qdr_init_status_reg.addr.base = sizeof(uint32) * 0x069b;
  Soc_petra_a_regs.qdr.qdr_init_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_init_status_reg.ready), sizeof(uint32) * 0x069b, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_init_status_reg.ddl_init_done), sizeof(uint32) * 0x069b, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_init_status_reg.dll_mstr_s), sizeof(uint32) * 0x069b, sizeof(uint32) * 0x0000, 8, 2);

  /* QdrQcSelVecLsb */
  Soc_petra_a_regs.qdr.qdr_qc_sel_vec_lsb_reg.addr.base = sizeof(uint32) * 0x069c;
  Soc_petra_a_regs.qdr.qdr_qc_sel_vec_lsb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_qc_sel_vec_lsb_reg.qc_sel_vec_lsb), sizeof(uint32) * 0x069c, sizeof(uint32) * 0x0000, 31, 25);

  /* QdrTrnFail */
  Soc_petra_a_regs.qdr.qdr_training_fail_reg.addr.base = sizeof(uint32) * 0x069d;
  Soc_petra_a_regs.qdr.qdr_training_fail_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_training_fail_reg.qc_sel_vec_msb), sizeof(uint32) * 0x069d, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_training_fail_reg.fail), sizeof(uint32) * 0x069d, sizeof(uint32) * 0x0000, 11, 11);

  /* OutputClockDelaySelect */
  Soc_petra_a_regs.qdr.output_clock_control_reg.addr.base = sizeof(uint32) * 0x06a0;
  Soc_petra_a_regs.qdr.output_clock_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.output_clock_control_reg.ccn_del_sel), sizeof(uint32) * 0x06a0, sizeof(uint32) * 0x0000, 2, 0);

  /* DllUpdtateCounter */
  Soc_petra_a_regs.qdr.dll_updtate_counter_reg.addr.base = sizeof(uint32) * 0x06a1;
  Soc_petra_a_regs.qdr.dll_updtate_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.dll_updtate_counter_reg.dll_updt_cnt), sizeof(uint32) * 0x06a1, sizeof(uint32) * 0x0000, 31, 0);

  /* PeriodicTrainingCounter */
  Soc_petra_a_regs.qdr.periodic_training_counter_reg.addr.base = sizeof(uint32) * 0x06a2;
  Soc_petra_a_regs.qdr.periodic_training_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.periodic_training_counter_reg.prd_trn_cnt), sizeof(uint32) * 0x06a2, sizeof(uint32) * 0x0000, 31, 0);

  /* PeriodicEnable */
  Soc_petra_a_regs.qdr.periodic_enable_reg.addr.base = sizeof(uint32) * 0x06a3;
  Soc_petra_a_regs.qdr.periodic_enable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.periodic_enable_reg.dll_updt_en), sizeof(uint32) * 0x06a3, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.periodic_enable_reg.prd_trn_en), sizeof(uint32) * 0x06a3, sizeof(uint32) * 0x0000, 4, 4);

  /* QdrCqEdgeSelect */
  Soc_petra_a_regs.qdr.qdr_cq_edge_select_reg.addr.base = sizeof(uint32) * 0x06a4;
  Soc_petra_a_regs.qdr.qdr_cq_edge_select_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.qdr_cq_edge_select_reg.cq_edge_sel), sizeof(uint32) * 0x06a4, sizeof(uint32) * 0x0000, 0, 0);

  /* DllControl */
  Soc_petra_a_regs.qdr.dll_control_reg.addr.base = sizeof(uint32) * 0x06a5;
  Soc_petra_a_regs.qdr.dll_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.dll_control_reg.dll_div2_en), sizeof(uint32) * 0x06a5, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.dll_control_reg.dll_update_single), sizeof(uint32) * 0x06a5, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.dll_control_reg.dll_slave_override_en), sizeof(uint32) * 0x06a5, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.qdr.dll_control_reg.dll_ready), sizeof(uint32) * 0x06a5, sizeof(uint32) * 0x0000, 16, 16);
}

/* Block registers initialization: IPS */
STATIC void
  soc_pa_regs_init_IPS(void)
{
  Soc_petra_a_regs.ips.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IPS;
  Soc_petra_a_regs.ips.addr.base = sizeof(uint32) * 0x0200;
  Soc_petra_a_regs.ips.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.ips.interrupt_reg.addr.base = sizeof(uint32) * 0x0200;
  Soc_petra_a_regs.ips.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.dqcq_depth_ovf), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.empty_dqcq_write), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.queue_entered_del), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.credit_lost), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.credit_overflow), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.system_port_queue_size_expired), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.flwid_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.qtype_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.qdesc_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.qsz_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.fsmrqctrl_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.dqcqmem_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_reg.fsmrqmem_ecc_err), sizeof(uint32) * 0x0200, sizeof(uint32) * 0x0000, 12, 12);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.ips.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x0210;
  Soc_petra_a_regs.ips.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.dqcq_depth_ovf_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.empty_dqcq_write_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.queue_entered_del_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.credit_lost_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.credit_overflow_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.system_port_queue_size_expired_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.flwid_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.qtype_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.qdesc_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.qsz_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.fsmrqctrl_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.dqcqmem_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interrupt_mask_reg.fsmrqmem_ecc_err_mask), sizeof(uint32) * 0x0210, sizeof(uint32) * 0x0000, 12, 12);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ips.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x0220;
  Soc_petra_a_regs.ips.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x0220, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ips.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x0221;
  Soc_petra_a_regs.ips.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x0221, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ips.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x0230;
  Soc_petra_a_regs.ips.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x0230, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ips.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x0231;
  Soc_petra_a_regs.ips.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x0231, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.ips.indirect_command_reg.addr.base = sizeof(uint32) * 0x0240;
  Soc_petra_a_regs.ips.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x0240, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x0240, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x0240, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x0240, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x0240, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.ips.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x0241;
  Soc_petra_a_regs.ips.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x0241, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x0241, sizeof(uint32) * 0x0000, 31, 31);

  /* ECC Configuration register */
  Soc_petra_a_regs.ips.ecc_configuration_reg.addr.base = sizeof(uint32) * 0x0260;
  Soc_petra_a_regs.ips.ecc_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ecc_configuration_reg.dis_ecc), sizeof(uint32) * 0x0260, sizeof(uint32) * 0x0000, 0, 0);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x0290,
    0x0000,
    &(Soc_petra_a_regs.ips.pcmi_config_reg),
    &(Soc_petra_a_regs.ips.pcmi_results_reg)
  );

  /* Ips General Configurations */
  Soc_petra_a_regs.ips.ips_general_configurations_reg.addr.base = sizeof(uint32) * 0x0300;
  Soc_petra_a_regs.ips.ips_general_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.discard_all_iqmmsg), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.discard_all_crdt), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.disable_status_msg_gen), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.disable_credit_surplus), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.dis_single_del_deq_cmd), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.fmc_credits_from_sch), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.dis_off_send_after_deq), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.send_fsm_on_credit), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.send_no_chg_on_credit), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.send_fsm_for_bigger_max_queue_size), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.send_fsm_for_mul_cross_down), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.update_max_qsz_from_local), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.dis_auto_credit_send_msg), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.en_int_lp_dqcq_fc), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.dis_deq_cmds), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.ips_count_by_timer), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_general_configurations_reg.ips_init_trigger), sizeof(uint32) * 0x0300, sizeof(uint32) * 0x0000, 31, 31);

  /* Ips Credit Config */
  Soc_petra_a_regs.ips.ips_credit_config_reg.addr.base = sizeof(uint32) * 0x0301;
  Soc_petra_a_regs.ips.ips_credit_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_credit_config_reg.credit_value), sizeof(uint32) * 0x0301, sizeof(uint32) * 0x0000, 12, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ips_credit_config_reg.resereved), sizeof(uint32) * 0x0301, sizeof(uint32) * 0x0000, 13, 13);

  /* Auto Credit Mechanism Queue Boundaries */
  Soc_petra_a_regs.ips.auto_credit_mechanism_queue_boundaries_reg.addr.base = sizeof(uint32) * 0x0304;
  Soc_petra_a_regs.ips.auto_credit_mechanism_queue_boundaries_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_frst_que), sizeof(uint32) * 0x0304, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_last_que), sizeof(uint32) * 0x0304, sizeof(uint32) * 0x0000, 30, 16);

  /* Auto Credit Mechanism Rate Configuration */
  Soc_petra_a_regs.ips.auto_credit_mechanism_rate_configuration_reg.addr.base = sizeof(uint32) * 0x0305;
  Soc_petra_a_regs.ips.auto_credit_mechanism_rate_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.auto_credit_mechanism_rate_configuration_reg.auto_cr_rate), sizeof(uint32) * 0x0305, sizeof(uint32) * 0x0000, 3, 0);

  /* High Priority Dqcq Depth Config1 */
  Soc_petra_a_regs.ips.high_priority_dqcq_depth_config1_reg.addr.base = sizeof(uint32) * 0x0306;
  Soc_petra_a_regs.ips.high_priority_dqcq_depth_config1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.high_priority_dqcq_depth_config1_reg.del_depth), sizeof(uint32) * 0x0306, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.high_priority_dqcq_depth_config1_reg.is_depth), sizeof(uint32) * 0x0306, sizeof(uint32) * 0x0000, 27, 16);

  /* High Priority Dqcq Depth Config2 */
  Soc_petra_a_regs.ips.high_priority_dqcq_depth_config2_reg.addr.base = sizeof(uint32) * 0x0307;
  Soc_petra_a_regs.ips.high_priority_dqcq_depth_config2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.high_priority_dqcq_depth_config2_reg.low_delay_depth), sizeof(uint32) * 0x0307, sizeof(uint32) * 0x0000, 11, 0);

  /* Low Priority Dqcq Depth Config1 */
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config1_reg.addr.base = sizeof(uint32) * 0x0308;
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config1_reg.dest0_depth), sizeof(uint32) * 0x0308, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config1_reg.dest1_depth), sizeof(uint32) * 0x0308, sizeof(uint32) * 0x0000, 27, 16);

  /* Low Priority Dqcq Depth Config2 */
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config2_reg.addr.base = sizeof(uint32) * 0x0309;
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config2_reg.dest2_depth), sizeof(uint32) * 0x0309, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config2_reg.dest3_depth), sizeof(uint32) * 0x0309, sizeof(uint32) * 0x0000, 27, 16);

  /* Low Priority Dqcq Depth Config3 */
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config3_reg.addr.base = sizeof(uint32) * 0x030a;
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config3_reg.dest4_depth), sizeof(uint32) * 0x030a, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config3_reg.dest5_depth), sizeof(uint32) * 0x030a, sizeof(uint32) * 0x0000, 27, 16);

  /* Low Priority Dqcq Depth Config4 */
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config4_reg.addr.base = sizeof(uint32) * 0x030b;
  Soc_petra_a_regs.ips.low_priority_dqcq_depth_config4_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config4_reg.dest6_depth), sizeof(uint32) * 0x030b, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.low_priority_dqcq_depth_config4_reg.dest7_depth), sizeof(uint32) * 0x030b, sizeof(uint32) * 0x0000, 27, 16);

  /* Credit Flow Control Threshold */
  Soc_petra_a_regs.ips.credit_flow_control_threshold_reg.addr.base = sizeof(uint32) * 0x030c;
  Soc_petra_a_regs.ips.credit_flow_control_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_flow_control_threshold_reg.crdt_fc_set), sizeof(uint32) * 0x030c, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_flow_control_threshold_reg.crdt_fc_reset), sizeof(uint32) * 0x030c, sizeof(uint32) * 0x0000, 27, 16);

  /* Ipt High-Priority Flow-Control Mask */
  Soc_petra_a_regs.ips.ipt_high_priority_flow_control_mask_reg.addr.base = sizeof(uint32) * 0x030d;
  Soc_petra_a_regs.ips.ipt_high_priority_flow_control_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ipt_high_priority_flow_control_mask_reg.ipt_fc_mask), sizeof(uint32) * 0x030d, sizeof(uint32) * 0x0000, 7, 0);

  /* Fmc Scheduler Configs */
  Soc_petra_a_regs.ips.fmc_scheduler_configs_reg.addr.base = sizeof(uint32) * 0x030e;
  Soc_petra_a_regs.ips.fmc_scheduler_configs_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fmc_scheduler_configs_reg.fmc_max_cr_rate), sizeof(uint32) * 0x030e, sizeof(uint32) * 0x0000, 25, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fmc_scheduler_configs_reg.fmc_max_burst), sizeof(uint32) * 0x030e, sizeof(uint32) * 0x0000, 31, 26);

  /* Gfmc Shaper Configs */
  Soc_petra_a_regs.ips.gfmc_shaper_configs_reg.addr.base = sizeof(uint32) * 0x030f;
  Soc_petra_a_regs.ips.gfmc_shaper_configs_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.gfmc_shaper_configs_reg.gfmc_max_cr_rate), sizeof(uint32) * 0x030f, sizeof(uint32) * 0x0000, 25, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.gfmc_shaper_configs_reg.gfmc_max_burst), sizeof(uint32) * 0x030f, sizeof(uint32) * 0x0000, 31, 26);

  /* Bfmc Shaper Configs */
  Soc_petra_a_regs.ips.bfmc_shaper_configs_reg.addr.base = sizeof(uint32) * 0x0310;
  Soc_petra_a_regs.ips.bfmc_shaper_configs_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc_shaper_configs_reg.bfmc_max_cr_rate), sizeof(uint32) * 0x0310, sizeof(uint32) * 0x0000, 25, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc_shaper_configs_reg.bfmc_max_burst), sizeof(uint32) * 0x0310, sizeof(uint32) * 0x0000, 31, 26);

  /* Bfmc Class Configs */
  Soc_petra_a_regs.ips.bfmc_class_configs_reg.addr.base = sizeof(uint32) * 0x0311;
  Soc_petra_a_regs.ips.bfmc_class_configs_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set(&(Soc_petra_a_regs.ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_0]), sizeof(uint32) * 0x0311, sizeof(uint32) * 0x0000, 4, 0);
  soc_petra_reg_fld_set(&(Soc_petra_a_regs.ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_1]), sizeof(uint32) * 0x0311, sizeof(uint32) * 0x0000, 12, 8);
  soc_petra_reg_fld_set(&(Soc_petra_a_regs.ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_2]), sizeof(uint32) * 0x0311, sizeof(uint32) * 0x0000, 20, 16);
  soc_petra_reg_fld_set(&(Soc_petra_a_regs.ips.bfmc_class_configs_reg.bfmc_wfq_en), sizeof(uint32) * 0x0311, sizeof(uint32) * 0x0000, 31, 31);

  /* Ingress Shape Scheduler Config */
  Soc_petra_a_regs.ips.ingress_shape_scheduler_config_reg.addr.base = sizeof(uint32) * 0x0312;
  Soc_petra_a_regs.ips.ingress_shape_scheduler_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.ingress_shape_scheduler_config_reg.iss_max_cr_rate), sizeof(uint32) * 0x0312, sizeof(uint32) * 0x0000, 25, 0);

  /* Credit Watchdog Queue Boundaries */
  Soc_petra_a_regs.ips.credit_watchdog_queue_boundaries_reg.addr.base = sizeof(uint32) * 0x0313;
  Soc_petra_a_regs.ips.credit_watchdog_queue_boundaries_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_watchdog_queue_boundaries_reg.cr_wd_bottom_q), sizeof(uint32) * 0x0313, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_watchdog_queue_boundaries_reg.cr_wd_top_q), sizeof(uint32) * 0x0313, sizeof(uint32) * 0x0000, 30, 16);

  /* Credit Watchdog Configuration */
  Soc_petra_a_regs.ips.credit_watchdog_configuration_reg.addr.base = sizeof(uint32) * 0x0314;
  Soc_petra_a_regs.ips.credit_watchdog_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_watchdog_configuration_reg.cr_wd_max_flow_msg_gen_rate), sizeof(uint32) * 0x0314, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_watchdog_configuration_reg.cr_wd_max_flow_msg_gen_burst_size), sizeof(uint32) * 0x0314, sizeof(uint32) * 0x0000, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_watchdog_configuration_reg.cr_wd_min_scan_cycle_period), sizeof(uint32) * 0x0314, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_watchdog_configuration_reg.cr_wd_gen_off_msgs), sizeof(uint32) * 0x0314, sizeof(uint32) * 0x0000, 31, 31);

  /* Fsm On Message Shaper */
  Soc_petra_a_regs.ips.fsm_on_message_shaper_reg.addr.base = sizeof(uint32) * 0x0315;
  Soc_petra_a_regs.ips.fsm_on_message_shaper_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsm_on_message_shaper_reg.fsm_max_rate), sizeof(uint32) * 0x0315, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsm_on_message_shaper_reg.fsm_max_burst), sizeof(uint32) * 0x0315, sizeof(uint32) * 0x0000, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsm_on_message_shaper_reg.shaper_low_pri), sizeof(uint32) * 0x0315, sizeof(uint32) * 0x0000, 31, 31);

  /* Fms Parameters */
  Soc_petra_a_regs.ips.fms_parameters_reg.addr.base = sizeof(uint32) * 0x0316;
  Soc_petra_a_regs.ips.fms_parameters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_parameters_reg.bypass_shaper), sizeof(uint32) * 0x0316, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_parameters_reg.disable_slow_delay), sizeof(uint32) * 0x0316, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_parameters_reg.local_route_bypass), sizeof(uint32) * 0x0316, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_parameters_reg.min_dly), sizeof(uint32) * 0x0316, sizeof(uint32) * 0x0000, 8, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_parameters_reg.time_scale), sizeof(uint32) * 0x0316, sizeof(uint32) * 0x0000, 13, 12);

  /* System Red Aging Configuration */
  Soc_petra_a_regs.ips.system_red_aging_configuration_reg.addr.base = sizeof(uint32) * 0x0317;
  Soc_petra_a_regs.ips.system_red_aging_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.system_red_aging_configuration_reg.system_red_age_period), sizeof(uint32) * 0x0317, sizeof(uint32) * 0x0000, 13, 0);

  /* Interdigitated Mode */
  Soc_petra_a_regs.ips.interdigitated_mode_reg.addr.base = sizeof(uint32) * 0x0318;
  Soc_petra_a_regs.ips.interdigitated_mode_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.interdigitated_mode_reg.interdigitated_mode), sizeof(uint32) * 0x0318, sizeof(uint32) * 0x0000, 31, 0);

  /* Int Lp Dqcq Fc Config */
  Soc_petra_a_regs.ips.int_lp_dqcq_fc_config_reg.addr.base = sizeof(uint32) * 0x0319;
  Soc_petra_a_regs.ips.int_lp_dqcq_fc_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.int_lp_dqcq_fc_config_reg.int_lp_dqcq_fc_set_th), sizeof(uint32) * 0x0319, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.int_lp_dqcq_fc_config_reg.int_lp_dqcq_fc_reset_th), sizeof(uint32) * 0x0319, sizeof(uint32) * 0x0000, 29, 16);

  /* Stored Credits Usage Configuration */
  Soc_petra_a_regs.ips.stored_credits_usage_configuration_reg.addr.base = sizeof(uint32) * 0x031a;
  Soc_petra_a_regs.ips.stored_credits_usage_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.stored_credits_usage_configuration_reg.mul_pkt_deq), sizeof(uint32) * 0x031a, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.stored_credits_usage_configuration_reg.mul_pkt_deq_bytes), sizeof(uint32) * 0x031a, sizeof(uint32) * 0x0000, 23, 16);

  /* Del Queue Number */
  Soc_petra_a_regs.ips.del_queue_number_reg.addr.base = sizeof(uint32) * 0x0340;
  Soc_petra_a_regs.ips.del_queue_number_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.del_queue_number_reg.del_queue_num), sizeof(uint32) * 0x0340, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.del_queue_number_reg.queue_last_cr_time), sizeof(uint32) * 0x0340, sizeof(uint32) * 0x0000, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.del_queue_number_reg.current_wd_time), sizeof(uint32) * 0x0340, sizeof(uint32) * 0x0000, 28, 24);

  /* Lost Credit Queue Number */
  Soc_petra_a_regs.ips.lost_credit_queue_number_reg.addr.base = sizeof(uint32) * 0x0341;
  Soc_petra_a_regs.ips.lost_credit_queue_number_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.lost_credit_queue_number_reg.lost_cr_queue_num), sizeof(uint32) * 0x0341, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.lost_credit_queue_number_reg.dqcq_id), sizeof(uint32) * 0x0341, sizeof(uint32) * 0x0000, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.lost_credit_queue_number_reg.dqcq_nrdy), sizeof(uint32) * 0x0341, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.lost_credit_queue_number_reg.fms_nrdy), sizeof(uint32) * 0x0341, sizeof(uint32) * 0x0000, 24, 24);

  /* Credit Overflow Queue Num */
  Soc_petra_a_regs.ips.credit_overflow_queue_num_reg.addr.base = sizeof(uint32) * 0x0342;
  Soc_petra_a_regs.ips.credit_overflow_queue_num_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_overflow_queue_num_reg.credit_overflow_queue_num), sizeof(uint32) * 0x0342, sizeof(uint32) * 0x0000, 14, 0);

  /* Expired System Port Id */
  Soc_petra_a_regs.ips.expired_system_port_id_reg.addr.base = sizeof(uint32) * 0x0343;
  Soc_petra_a_regs.ips.expired_system_port_id_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.expired_system_port_id_reg.expired_system_port_id), sizeof(uint32) * 0x0343, sizeof(uint32) * 0x0000, 11, 0);

  /* Empty Dqcq Id */
  Soc_petra_a_regs.ips.empty_dqcq_id_reg.addr.base = sizeof(uint32) * 0x0344;
  Soc_petra_a_regs.ips.empty_dqcq_id_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.empty_dqcq_id_reg.empty_dqcq_id), sizeof(uint32) * 0x0344, sizeof(uint32) * 0x0000, 3, 0);

  /* Manual Queue Operation */
  Soc_petra_a_regs.ips.manual_queue_operation_reg.addr.base = sizeof(uint32) * 0x0380;
  Soc_petra_a_regs.ips.manual_queue_operation_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.man_queue_id), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.deq_cmd_cr2send), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.fs_value), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 25, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.fs_override), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.del_dqcq), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.fs_trigger), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.grant_credit), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.flush_trigger), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.manual_queue_operation_reg.send_pkt_trigger), sizeof(uint32) * 0x0380, sizeof(uint32) * 0x0000, 31, 31);

  /* Timer Configuration */
  Soc_petra_a_regs.ips.timer_configuration_reg.addr.base = sizeof(uint32) * 0x0381;
  Soc_petra_a_regs.ips.timer_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.timer_configuration_reg.time_count_config), sizeof(uint32) * 0x0381, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.timer_configuration_reg.counter_reset), sizeof(uint32) * 0x0381, sizeof(uint32) * 0x0000, 31, 31);

  /* Activate Timer */
  Soc_petra_a_regs.ips.activate_timer_reg.addr.base = sizeof(uint32) * 0x0382;
  Soc_petra_a_regs.ips.activate_timer_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.activate_timer_reg.activate_timer), sizeof(uint32) * 0x0382, sizeof(uint32) * 0x0000, 0, 0);

  /* Queue Num Filter */
  Soc_petra_a_regs.ips.queue_num_filter_reg.addr.base = sizeof(uint32) * 0x0383;
  Soc_petra_a_regs.ips.queue_num_filter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.queue_num_filter_reg.queue_num_filter), sizeof(uint32) * 0x0383, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.queue_num_filter_reg.queue_num_filter_mask), sizeof(uint32) * 0x0383, sizeof(uint32) * 0x0000, 30, 16);

  /* Flow Status Filter */
  Soc_petra_a_regs.ips.flow_status_filter_reg.addr.base = sizeof(uint32) * 0x0384;
  Soc_petra_a_regs.ips.flow_status_filter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_status_filter_reg.fs_dest_port_id), sizeof(uint32) * 0x0384, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_status_filter_reg.fs_dest_dev_id), sizeof(uint32) * 0x0384, sizeof(uint32) * 0x0000, 18, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_status_filter_reg.flow_status_filter), sizeof(uint32) * 0x0384, sizeof(uint32) * 0x0000, 22, 20);

  /* Flow Status Filter Mask */
  Soc_petra_a_regs.ips.flow_status_filter_mask_reg.addr.base = sizeof(uint32) * 0x0385;
  Soc_petra_a_regs.ips.flow_status_filter_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_status_filter_mask_reg.fs_dest_port_id_mask), sizeof(uint32) * 0x0385, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_status_filter_mask_reg.fs_dest_dev_id_mask), sizeof(uint32) * 0x0385, sizeof(uint32) * 0x0000, 18, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_status_filter_mask_reg.flow_status_filter_mask), sizeof(uint32) * 0x0385, sizeof(uint32) * 0x0000, 22, 20);

  /* Iqm Report Filter */
  Soc_petra_a_regs.ips.iqm_report_filter_reg.addr.base = sizeof(uint32) * 0x0386;
  Soc_petra_a_regs.ips.iqm_report_filter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.iqm_report_filter_reg.deq_not_enq_fltr), sizeof(uint32) * 0x0386, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.iqm_report_filter_reg.discard_fltr), sizeof(uint32) * 0x0386, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.iqm_report_filter_reg.wqup_fltr), sizeof(uint32) * 0x0386, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.iqm_report_filter_reg.deq_not_enq_fltr_mask), sizeof(uint32) * 0x0386, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.iqm_report_filter_reg.discard_fltr_mask), sizeof(uint32) * 0x0386, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.iqm_report_filter_reg.wqup_fltr_mask), sizeof(uint32) * 0x0386, sizeof(uint32) * 0x0000, 6, 6);

  /* Dqcq Id Filter */
  Soc_petra_a_regs.ips.dqcq_id_filter_reg.addr.base = sizeof(uint32) * 0x0387;
  Soc_petra_a_regs.ips.dqcq_id_filter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_id_filter_reg.dqcq_id_filter), sizeof(uint32) * 0x0387, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_id_filter_reg.dqcq_dest_port_filter), sizeof(uint32) * 0x0387, sizeof(uint32) * 0x0000, 11, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_id_filter_reg.dqcq_dest_dev_filter), sizeof(uint32) * 0x0387, sizeof(uint32) * 0x0000, 22, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_id_filter_reg.dqcq_id_filter_en), sizeof(uint32) * 0x0387, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_id_filter_reg.dqcq_dest_port_filter_en), sizeof(uint32) * 0x0387, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_id_filter_reg.dqcq_dest_dev_filter_en), sizeof(uint32) * 0x0387, sizeof(uint32) * 0x0000, 30, 30);

  /* Fsmrq Max Occupancy */
  Soc_petra_a_regs.ips.fsmrq_max_occupancy_reg.addr.base = sizeof(uint32) * 0x03a0;
  Soc_petra_a_regs.ips.fsmrq_max_occupancy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_max_occupancy_reg.lp_fsmrq_max_occ), sizeof(uint32) * 0x03a0, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_max_occupancy_reg.hp_fsmrq_max_occ), sizeof(uint32) * 0x03a0, sizeof(uint32) * 0x0000, 23, 16);

  /* Dqcq Max Occupancy Hp */
  Soc_petra_a_regs.ips.dqcq_max_occupancy_hp_reg.addr.base = sizeof(uint32) * 0x03a1;
  Soc_petra_a_regs.ips.dqcq_max_occupancy_hp_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_max_occupancy_hp_reg.hp_dqcq_max_occ), sizeof(uint32) * 0x03a1, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_max_occupancy_hp_reg.hp_dqcq_max_occ_id), sizeof(uint32) * 0x03a1, sizeof(uint32) * 0x0000, 19, 16);

  /* Dqcq Max Occupancy Lp */
  Soc_petra_a_regs.ips.dqcq_max_occupancy_lp_reg.addr.base = sizeof(uint32) * 0x03a2;
  Soc_petra_a_regs.ips.dqcq_max_occupancy_lp_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_max_occupancy_lp_reg.lp_dqcq_max_occ), sizeof(uint32) * 0x03a2, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.dqcq_max_occupancy_lp_reg.lp_dqcq_max_occ_id), sizeof(uint32) * 0x03a2, sizeof(uint32) * 0x0000, 19, 16);

  /* Active Queue Count */
  Soc_petra_a_regs.ips.active_queue_count_reg.addr.base = sizeof(uint32) * 0x03a3;
  Soc_petra_a_regs.ips.active_queue_count_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.active_queue_count_reg.max_active_queue_count), sizeof(uint32) * 0x03a3, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.active_queue_count_reg.active_queue_count), sizeof(uint32) * 0x03a3, sizeof(uint32) * 0x0000, 31, 16);

  /* Fms Max Occupancy */
  Soc_petra_a_regs.ips.fms_max_occupancy_reg.addr.base = sizeof(uint32) * 0x03a4;
  Soc_petra_a_regs.ips.fms_max_occupancy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_max_occupancy_reg.fms_max_occupancy), sizeof(uint32) * 0x03a4, sizeof(uint32) * 0x0000, 7, 0);

  /* Int Lp Dqcq Fc Status */
  Soc_petra_a_regs.ips.int_lp_dqcq_fc_status_reg.addr.base = sizeof(uint32) * 0x03a5;
  Soc_petra_a_regs.ips.int_lp_dqcq_fc_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.int_lp_dqcq_fc_status_reg.lp_dqcq_queue_id), sizeof(uint32) * 0x03a5, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.int_lp_dqcq_fc_status_reg.lp_dqcq_queue_size), sizeof(uint32) * 0x03a5, sizeof(uint32) * 0x0000, 29, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.int_lp_dqcq_fc_status_reg.lp_dqcq_flow_control), sizeof(uint32) * 0x03a5, sizeof(uint32) * 0x0000, 31, 31);

  /* Fsmrq Delay Counter */
  Soc_petra_a_regs.ips.fsmrq_delay_counter_reg.addr.base = sizeof(uint32) * 0x03a6;
  Soc_petra_a_regs.ips.fsmrq_delay_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_delay_counter_reg.fsmrq_delay_counter), sizeof(uint32) * 0x03a6, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_delay_counter_reg.fsmrq_delay_queue), sizeof(uint32) * 0x03a6, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_delay_counter_reg.fsmrq_delay_by_queue), sizeof(uint32) * 0x03a6, sizeof(uint32) * 0x0000, 31, 31);

  /* Fms Delay Counter */
  Soc_petra_a_regs.ips.fms_delay_counter_reg.addr.base = sizeof(uint32) * 0x03a7;
  Soc_petra_a_regs.ips.fms_delay_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_delay_counter_reg.fms_delay_counter), sizeof(uint32) * 0x03a7, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_delay_counter_reg.fms_delay_queue), sizeof(uint32) * 0x03a7, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_delay_counter_reg.fms_delay_by_queue), sizeof(uint32) * 0x03a7, sizeof(uint32) * 0x0000, 31, 31);

  /* Credit Fabric Latency Counter */
  Soc_petra_a_regs.ips.credit_fabric_latency_counter_reg.addr.base = sizeof(uint32) * 0x03a8;
  Soc_petra_a_regs.ips.credit_fabric_latency_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_fabric_latency_counter_reg.cr_ltncy_counter), sizeof(uint32) * 0x03a8, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.credit_fabric_latency_counter_reg.cr_ltncy_que), sizeof(uint32) * 0x03a8, sizeof(uint32) * 0x0000, 30, 16);

  /* Flow Control Count Select */
  Soc_petra_a_regs.ips.flow_control_count_select_reg.addr.base = sizeof(uint32) * 0x03a9;
  Soc_petra_a_regs.ips.flow_control_count_select_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_control_count_select_reg.fc_count_sel), sizeof(uint32) * 0x03a9, sizeof(uint32) * 0x0000, 4, 0);

  /* Capture Queue Descriptor Config */
  Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.addr.base = sizeof(uint32) * 0x03aa;
  Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_crs), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_crs_en), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_fsmrq_ctrl), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_fsmrq_ctrl_en), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_in_dqcq), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_in_dqcq_en), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_wd_del), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_wd_del_en), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_one_pkt_deq), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_config_reg.capture_one_pkt_deq_en), sizeof(uint32) * 0x03aa, sizeof(uint32) * 0x0000, 17, 17);

  /* Capture Queue Descriptor */
  Soc_petra_a_regs.ips.capture_queue_descriptor_reg.addr.base = sizeof(uint32) * 0x03b0;
  Soc_petra_a_regs.ips.capture_queue_descriptor_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_cr_bal), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_qsz), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 22, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_crs), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 25, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_fsmrq_ctrl), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 27, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_in_dqcq), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_one_pkt_deq), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_wd_del), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.capture_queue_descriptor_reg.cptrd_valid), sizeof(uint32) * 0x03b0, sizeof(uint32) * 0x0000, 31, 31);

  /* Max Cr Bal */
  Soc_petra_a_regs.ips.max_cr_bal_reg.addr.base = sizeof(uint32) * 0x03b1;
  Soc_petra_a_regs.ips.max_cr_bal_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.max_cr_bal_reg.max_cr_bal), sizeof(uint32) * 0x03b1, sizeof(uint32) * 0x0000, 15, 0);

  /* Max Fsmrq Req Queues */
  Soc_petra_a_regs.ips.max_fsmrq_req_queues_reg.addr.base = sizeof(uint32) * 0x03b2;
  Soc_petra_a_regs.ips.max_fsmrq_req_queues_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.max_fsmrq_req_queues_reg.max_fsmrq_req_queues), sizeof(uint32) * 0x03b2, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.max_fsmrq_req_queues_reg.fsmrq_req_queues), sizeof(uint32) * 0x03b2, sizeof(uint32) * 0x0000, 31, 16);

  /* Max Port Queue Size */
  Soc_petra_a_regs.ips.max_port_queue_size_reg.addr.base = sizeof(uint32) * 0x03b3;
  Soc_petra_a_regs.ips.max_port_queue_size_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.max_port_queue_size_reg.max_port_queue_size), sizeof(uint32) * 0x03b3, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.max_port_queue_size_reg.ins_port_queue_size), sizeof(uint32) * 0x03b3, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.max_port_queue_size_reg.max_port_queue_size_port_id), sizeof(uint32) * 0x03b3, sizeof(uint32) * 0x0000, 27, 16);

  /* Time In Slow */
  Soc_petra_a_regs.ips.time_in_slow_reg.addr.base = sizeof(uint32) * 0x03b4;
  Soc_petra_a_regs.ips.time_in_slow_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.time_in_slow_reg.time_in_slow), sizeof(uint32) * 0x03b4, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.time_in_slow_reg.time_in_slow_valid), sizeof(uint32) * 0x03b4, sizeof(uint32) * 0x0000, 31, 31);

  /* Time In Norm */
  Soc_petra_a_regs.ips.time_in_norm_reg.addr.base = sizeof(uint32) * 0x03b5;
  Soc_petra_a_regs.ips.time_in_norm_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.time_in_norm_reg.time_in_norm), sizeof(uint32) * 0x03b5, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.time_in_norm_reg.time_in_norm_valid), sizeof(uint32) * 0x03b5, sizeof(uint32) * 0x0000, 31, 31);

  /* Fcr Credit Counter */
  Soc_petra_a_regs.ips.fcr_credit_counter_reg.addr.base = sizeof(uint32) * 0x03d0;
  Soc_petra_a_regs.ips.fcr_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fcr_credit_counter_reg.fcr_credit_counter), sizeof(uint32) * 0x03d0, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fcr_credit_counter_reg.fcr_credit_counter_overflow), sizeof(uint32) * 0x03d0, sizeof(uint32) * 0x0000, 31, 31);

  /* Global Credit Counter */
  Soc_petra_a_regs.ips.global_credit_counter_reg.addr.base = sizeof(uint32) * 0x03d1;
  Soc_petra_a_regs.ips.global_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.global_credit_counter_reg.global_credit_counter), sizeof(uint32) * 0x03d1, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.global_credit_counter_reg.global_credit_counter_overflow), sizeof(uint32) * 0x03d1, sizeof(uint32) * 0x0000, 31, 31);

  /* Gfmc Credit Counter */
  Soc_petra_a_regs.ips.gfmc_credit_counter_reg.addr.base = sizeof(uint32) * 0x03d2;
  Soc_petra_a_regs.ips.gfmc_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.gfmc_credit_counter_reg.gfmc_credit_counter), sizeof(uint32) * 0x03d2, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.gfmc_credit_counter_reg.gfmc_credit_counter_overflow), sizeof(uint32) * 0x03d2, sizeof(uint32) * 0x0000, 31, 31);

  /* Bfmc1 Credit Counter */
  Soc_petra_a_regs.ips.bfmc1_credit_counter_reg.addr.base = sizeof(uint32) * 0x03d3;
  Soc_petra_a_regs.ips.bfmc1_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc1_credit_counter_reg.bfmc1_credit_counter), sizeof(uint32) * 0x03d3, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc1_credit_counter_reg.bfmc1_credit_counter_overflow), sizeof(uint32) * 0x03d3, sizeof(uint32) * 0x0000, 31, 31);

  /* Bfmc2 Credit Counter */
  Soc_petra_a_regs.ips.bfmc2_credit_counter_reg.addr.base = sizeof(uint32) * 0x03d4;
  Soc_petra_a_regs.ips.bfmc2_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc2_credit_counter_reg.bfmc2_credit_counter), sizeof(uint32) * 0x03d4, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc2_credit_counter_reg.bfmc2_credit_counter_overflow), sizeof(uint32) * 0x03d4, sizeof(uint32) * 0x0000, 31, 31);

  /* Bfmc3 Credit Counter */
  Soc_petra_a_regs.ips.bfmc3_credit_counter_reg.addr.base = sizeof(uint32) * 0x03d5;
  Soc_petra_a_regs.ips.bfmc3_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc3_credit_counter_reg.bfmc3_credit_counter), sizeof(uint32) * 0x03d5, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.bfmc3_credit_counter_reg.bfmc3_credit_counter_overflow), sizeof(uint32) * 0x03d5, sizeof(uint32) * 0x0000, 31, 31);

  /* Global Flow Status Counter */
  Soc_petra_a_regs.ips.global_flow_status_counter_reg.addr.base = sizeof(uint32) * 0x03d6;
  Soc_petra_a_regs.ips.global_flow_status_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.global_flow_status_counter_reg.global_flow_status_counter), sizeof(uint32) * 0x03d6, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.global_flow_status_counter_reg.global_flow_status_counter_overflow), sizeof(uint32) * 0x03d6, sizeof(uint32) * 0x0000, 31, 31);

  /* Fsmrq Flow Status Counter */
  Soc_petra_a_regs.ips.fsmrq_flow_status_counter_reg.addr.base = sizeof(uint32) * 0x03d7;
  Soc_petra_a_regs.ips.fsmrq_flow_status_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_flow_status_counter_reg.fsmrq_flow_status_counter), sizeof(uint32) * 0x03d7, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fsmrq_flow_status_counter_reg.fsmrq_flow_status_counter_overflow), sizeof(uint32) * 0x03d7, sizeof(uint32) * 0x0000, 31, 31);

  /* Fms Flow Status Counter */
  Soc_petra_a_regs.ips.fms_flow_status_counter_reg.addr.base = sizeof(uint32) * 0x03d8;
  Soc_petra_a_regs.ips.fms_flow_status_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_flow_status_counter_reg.fms_flow_status_counter), sizeof(uint32) * 0x03d8, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.fms_flow_status_counter_reg.fms_flow_status_counter_overflow), sizeof(uint32) * 0x03d8, sizeof(uint32) * 0x0000, 31, 31);

  /* Global Iqm Report Counter */
  Soc_petra_a_regs.ips.global_iqm_report_counter_reg.addr.base = sizeof(uint32) * 0x03d9;
  Soc_petra_a_regs.ips.global_iqm_report_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.global_iqm_report_counter_reg.glbl_iqm_rprt_counter), sizeof(uint32) * 0x03d9, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.global_iqm_report_counter_reg.glbl_iqm_rprt_counter_overflow), sizeof(uint32) * 0x03d9, sizeof(uint32) * 0x0000, 31, 31);

  /* Programmable Iqm Report Counter */
  Soc_petra_a_regs.ips.programmable_iqm_report_counter_reg.addr.base = sizeof(uint32) * 0x03da;
  Soc_petra_a_regs.ips.programmable_iqm_report_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.programmable_iqm_report_counter_reg.prg_iqm_rprt_counter), sizeof(uint32) * 0x03da, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.programmable_iqm_report_counter_reg.prg_iqm_rprt_counter_overflow), sizeof(uint32) * 0x03da, sizeof(uint32) * 0x0000, 31, 31);

  /* Crwd Event Counter */
  Soc_petra_a_regs.ips.crwd_event_counter_reg.addr.base = sizeof(uint32) * 0x03db;
  Soc_petra_a_regs.ips.crwd_event_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.crwd_event_counter_reg.crwd_counter), sizeof(uint32) * 0x03db, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.crwd_event_counter_reg.crwd_counter_overflow), sizeof(uint32) * 0x03db, sizeof(uint32) * 0x0000, 31, 31);

  /* Deq Cmd Byte Counter */
  Soc_petra_a_regs.ips.deq_cmd_byte_counter_reg.addr.base = sizeof(uint32) * 0x03dc;
  Soc_petra_a_regs.ips.deq_cmd_byte_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.deq_cmd_byte_counter_reg.deq_cmd_byte_counter), sizeof(uint32) * 0x03dc, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.deq_cmd_byte_counter_reg.deq_cmd_byte_counter_overflow), sizeof(uint32) * 0x03dc, sizeof(uint32) * 0x0000, 31, 31);

  /* Deq Cmd Counter */
  Soc_petra_a_regs.ips.deq_cmd_counter_reg.addr.base = sizeof(uint32) * 0x03dd;
  Soc_petra_a_regs.ips.deq_cmd_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.deq_cmd_counter_reg.deq_cmd_counter), sizeof(uint32) * 0x03dd, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.deq_cmd_counter_reg.deq_cmd_counter_overflow), sizeof(uint32) * 0x03dd, sizeof(uint32) * 0x0000, 31, 31);

  /* Masked Iqm Event Counter */
  Soc_petra_a_regs.ips.masked_iqm_event_counter_reg.addr.base = sizeof(uint32) * 0x03de;
  Soc_petra_a_regs.ips.masked_iqm_event_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.masked_iqm_event_counter_reg.mskd_iqm_evnts_counter), sizeof(uint32) * 0x03de, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.masked_iqm_event_counter_reg.mskd_iqm_evnts_counter_overflow), sizeof(uint32) * 0x03de, sizeof(uint32) * 0x0000, 31, 31);

  /* Flow Control Counter */
  Soc_petra_a_regs.ips.flow_control_counter_reg.addr.base = sizeof(uint32) * 0x03df;
  Soc_petra_a_regs.ips.flow_control_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_control_counter_reg.fc_count), sizeof(uint32) * 0x03df, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.flow_control_counter_reg.fc_count_overflow), sizeof(uint32) * 0x03df, sizeof(uint32) * 0x0000, 31, 31);

  /* Returned Credit Counter */
  Soc_petra_a_regs.ips.returned_credit_counter_reg.addr.base = sizeof(uint32) * 0x03e0;
  Soc_petra_a_regs.ips.returned_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.returned_credit_counter_reg.returned_credit_count), sizeof(uint32) * 0x03e0, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ips.returned_credit_counter_reg.returned_credit_count_overflow), sizeof(uint32) * 0x03e0, sizeof(uint32) * 0x0000, 31, 31);
}

/* Block registers initialization: IPT */
STATIC void
  soc_pa_regs_init_IPT(void)
{
  uint32
    reg_idx,
    fld_idx;

  Soc_petra_a_regs.ipt.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_IPT;
  Soc_petra_a_regs.ipt.addr.base = sizeof(uint32) * 0x0800;
  Soc_petra_a_regs.ipt.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.ipt.interrupt_reg.addr.base = sizeof(uint32) * 0x0800;
  Soc_petra_a_regs.ipt.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.sop_ecc_error), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.mop_ecc_error), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.fdtc_ecc_error), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.egqc_ecc_error), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.bdq_ecc_error), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.pdq_ecc_error), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_reg.crc_err_pkt), sizeof(uint32) * 0x0800, sizeof(uint32) * 0x0000, 6, 6);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.ipt.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x0810;
  Soc_petra_a_regs.ipt.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.sop_ecc_error_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.mop_ecc_error_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.fdtc_ecc_error_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.egqc_ecc_error_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.bdq_ecc_error_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.pdq_ecc_error_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.interrupt_mask_reg.crc_err_pkt_mask), sizeof(uint32) * 0x0810, sizeof(uint32) * 0x0000, 6, 6);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x0820;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x0820, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x0821;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x0821, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x0822;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x0822, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x0823;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x0823, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x0824;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x0824, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x0825;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x0825, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_6.addr.base = sizeof(uint32) * 0x0826;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_6.indirect_command_wr_data), sizeof(uint32) * 0x0826, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_7.addr.base = sizeof(uint32) * 0x0827;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_7.indirect_command_wr_data), sizeof(uint32) * 0x0827, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_8.addr.base = sizeof(uint32) * 0x0828;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_8.indirect_command_wr_data), sizeof(uint32) * 0x0828, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_9.addr.base = sizeof(uint32) * 0x0829;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_9.indirect_command_wr_data), sizeof(uint32) * 0x0829, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_10.addr.base = sizeof(uint32) * 0x082a;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_10.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_10.indirect_command_wr_data), sizeof(uint32) * 0x082a, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_11.addr.base = sizeof(uint32) * 0x082b;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_11.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_11.indirect_command_wr_data), sizeof(uint32) * 0x082b, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_12.addr.base = sizeof(uint32) * 0x082c;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_12.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_12.indirect_command_wr_data), sizeof(uint32) * 0x082c, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_13.addr.base = sizeof(uint32) * 0x082d;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_13.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_13.indirect_command_wr_data), sizeof(uint32) * 0x082d, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_14.addr.base = sizeof(uint32) * 0x082e;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_14.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_14.indirect_command_wr_data), sizeof(uint32) * 0x082e, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_15.addr.base = sizeof(uint32) * 0x082f;
  Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_15.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_wr_data_reg_15.indirect_command_wr_data), sizeof(uint32) * 0x082f, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x0830;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x0830, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x0831;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x0831, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x0832;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x0832, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x0833;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x0833, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x0834;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x0834, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x0835;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x0835, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_6.addr.base = sizeof(uint32) * 0x0836;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_6.indirect_command_rd_data), sizeof(uint32) * 0x0836, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_7.addr.base = sizeof(uint32) * 0x0837;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_7.indirect_command_rd_data), sizeof(uint32) * 0x0837, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_8.addr.base = sizeof(uint32) * 0x0838;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_8.indirect_command_rd_data), sizeof(uint32) * 0x0838, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_9.addr.base = sizeof(uint32) * 0x0839;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_9.indirect_command_rd_data), sizeof(uint32) * 0x0839, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_10.addr.base = sizeof(uint32) * 0x083a;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_10.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_10.indirect_command_rd_data), sizeof(uint32) * 0x083a, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_11.addr.base = sizeof(uint32) * 0x083b;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_11.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_11.indirect_command_rd_data), sizeof(uint32) * 0x083b, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_12.addr.base = sizeof(uint32) * 0x083c;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_12.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_12.indirect_command_rd_data), sizeof(uint32) * 0x083c, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_13.addr.base = sizeof(uint32) * 0x083d;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_13.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_13.indirect_command_rd_data), sizeof(uint32) * 0x083d, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_14.addr.base = sizeof(uint32) * 0x083e;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_14.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_14.indirect_command_rd_data), sizeof(uint32) * 0x083e, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_15.addr.base = sizeof(uint32) * 0x083f;
  Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_15.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_rd_data_reg_15.indirect_command_rd_data), sizeof(uint32) * 0x083f, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.ipt.indirect_command_reg.addr.base = sizeof(uint32) * 0x0840;
  Soc_petra_a_regs.ipt.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x0840, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x0840, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x0840, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x0840, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x0840, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.ipt.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x0841;
  Soc_petra_a_regs.ipt.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x0841, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x0841, sizeof(uint32) * 0x0000, 31, 31);

  /* Disable Ecc */
  Soc_petra_a_regs.ipt.disable_ecc_reg.addr.base = sizeof(uint32) * 0x0842;
  Soc_petra_a_regs.ipt.disable_ecc_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.disable_ecc_reg.disable_ecc), sizeof(uint32) * 0x0842, sizeof(uint32) * 0x0000, 0, 0);

  /* Data Queue EGQThreshold */
  Soc_petra_a_regs.ipt.data_queue_egqthreshold_reg.addr.base = sizeof(uint32) * 0x0860;
  Soc_petra_a_regs.ipt.data_queue_egqthreshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.data_queue_egqthreshold_reg.dtq_egq_th), sizeof(uint32) * 0x0860, sizeof(uint32) * 0x0000, 4, 0);

  /* Gci Flag Enable Disable */
  Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.addr.base = sizeof(uint32) * 0x0861;
  Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.bfmc0_ips_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.bfmc01_ips_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.bfmc2_ips_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.gfmc_ips_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.lkyb1_ipt_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.lkyb2_ipt_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.lkyb3_ipt_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.lkyb4_ipt_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gci_flag_enable_disable_reg.mci1_ipt_enable), sizeof(uint32) * 0x0861, sizeof(uint32) * 0x0000, 8, 8);

  /* GCIMCILeaky Bucket Configuration Register0 */
  Soc_petra_a_regs.ipt.gcimcileaky_bucket_configuration_0_reg.addr.base = sizeof(uint32) * 0x0862;
  Soc_petra_a_regs.ipt.gcimcileaky_bucket_configuration_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcimcileaky_bucket_configuration_0_reg.lky_bkt_max_cnt), sizeof(uint32) * 0x0862, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcimcileaky_bucket_configuration_0_reg.lky_bkt_cng_th), sizeof(uint32) * 0x0862, sizeof(uint32) * 0x0000, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcimcileaky_bucket_configuration_0_reg.hold_mciprd), sizeof(uint32) * 0x0862, sizeof(uint32) * 0x0000, 29, 24);

  /* GCILeaky Bucket Configuration Register1 */
  Soc_petra_a_regs.ipt.gcileaky_bucket_configuration_1_reg.addr.base = sizeof(uint32) * 0x0863;
  Soc_petra_a_regs.ipt.gcileaky_bucket_configuration_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcileaky_bucket_configuration_1_reg.lky_bkt_dcr_rate1), sizeof(uint32) * 0x0863, sizeof(uint32) * 0x0000, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcileaky_bucket_configuration_1_reg.lky_bkt_dcr_rate2), sizeof(uint32) * 0x0863, sizeof(uint32) * 0x0000, 13, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcileaky_bucket_configuration_1_reg.lky_bkt_dcr_rate3), sizeof(uint32) * 0x0863, sizeof(uint32) * 0x0000, 21, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gcileaky_bucket_configuration_1_reg.lky_bkt_dcr_rate4), sizeof(uint32) * 0x0863, sizeof(uint32) * 0x0000, 29, 24);

  /* Mapping Traffic Class */
  Soc_petra_a_regs.ipt.mapping_traffic_class_reg.addr.base = sizeof(uint32) * 0x0864;
  Soc_petra_a_regs.ipt.mapping_traffic_class_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.mapping_traffic_class_reg.class_table), sizeof(uint32) * 0x0864, sizeof(uint32) * 0x0000, 15, 0);

  /* Fabric Header Extended Mode */
  Soc_petra_a_regs.ipt.fabric_header_extended_mode_reg.addr.base = sizeof(uint32) * 0x0865;
  Soc_petra_a_regs.ipt.fabric_header_extended_mode_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.fabric_header_extended_mode_reg.extmode), sizeof(uint32) * 0x0865, sizeof(uint32) * 0x0000, 0, 0);

  /* CRCErr Rate Configuration Register0 */
  Soc_petra_a_regs.ipt.crcerr_rate_configuration_0_reg.addr.base = sizeof(uint32) * 0x0866;
  Soc_petra_a_regs.ipt.crcerr_rate_configuration_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_rate_configuration_0_reg.crcerr_rate_dcr), sizeof(uint32) * 0x0866, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_rate_configuration_0_reg.crcerr_rate_max_cnt), sizeof(uint32) * 0x0866, sizeof(uint32) * 0x0000, 31, 16);

  /* CRCErr Rate Configuration Register1 */
  Soc_petra_a_regs.ipt.crcerr_rate_configuration_1_reg.addr.base = sizeof(uint32) * 0x0867;
  Soc_petra_a_regs.ipt.crcerr_rate_configuration_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_rate_configuration_1_reg.crcerr_rate_cng_th), sizeof(uint32) * 0x0867, sizeof(uint32) * 0x0000, 15, 0);

  /* CRCErr Weight Configuration Register0 */
  Soc_petra_a_regs.ipt.crcerr_weight_configuration_0_reg.addr.base = sizeof(uint32) * 0x0868;
  Soc_petra_a_regs.ipt.crcerr_weight_configuration_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_weight_configuration_0_reg.crcerr_weight), sizeof(uint32) * 0x0868, sizeof(uint32) * 0x0000, 4, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_weight_configuration_0_reg.crcnoerr_weight), sizeof(uint32) * 0x0868, sizeof(uint32) * 0x0000, 20, 16);

  /* CRCErr Weight Configuration Register1 */
  Soc_petra_a_regs.ipt.crcerr_weight_configuration_1_reg.addr.base = sizeof(uint32) * 0x0869;
  Soc_petra_a_regs.ipt.crcerr_weight_configuration_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_weight_configuration_1_reg.crcerr_weight_max_cnt), sizeof(uint32) * 0x0869, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crcerr_weight_configuration_1_reg.crcerr_weight_cng_th), sizeof(uint32) * 0x0869, sizeof(uint32) * 0x0000, 31, 16);

  /* Force Local Or Fabric */
  Soc_petra_a_regs.ipt.force_local_or_fabric_reg.addr.base = sizeof(uint32) * 0x086a;
  Soc_petra_a_regs.ipt.force_local_or_fabric_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.force_local_or_fabric_reg.force_local), sizeof(uint32) * 0x086a, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.force_local_or_fabric_reg.force_fabric), sizeof(uint32) * 0x086a, sizeof(uint32) * 0x0000, 1, 1);

  /* DRAMBuffer Pointer Queue Threshold0-1 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].addr.base = sizeof(uint32) * (0x0870+reg_idx);
    Soc_petra_a_regs.ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].dpq_th_hp),
      sizeof(uint32) * (0x0870+reg_idx),
      sizeof(uint32) * 0x0000,
      11,
      0
    );

    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].dpq_th_lp),
      sizeof(uint32) * (0x0870+reg_idx),
      sizeof(uint32) * 0x0000,
      27,
      16
    );
  
  }

  /* DRAMBuffer Pointer Queue Size0-1 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size0_1_reg.addr.base = sizeof(uint32) * 0x0880;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size0_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size0_1_reg.dpq_size0), sizeof(uint32) * 0x0880, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size0_1_reg.dpq_size1), sizeof(uint32) * 0x0880, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size2-3 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size2_3_reg.addr.base = sizeof(uint32) * 0x0881;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size2_3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size2_3_reg.dpq_size2), sizeof(uint32) * 0x0881, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size2_3_reg.dpq_size3), sizeof(uint32) * 0x0881, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size4-5 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size4_5_reg.addr.base = sizeof(uint32) * 0x0882;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size4_5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size4_5_reg.dpq_size4), sizeof(uint32) * 0x0882, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size4_5_reg.dpq_size5), sizeof(uint32) * 0x0882, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size6-7 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size6_7_reg.addr.base = sizeof(uint32) * 0x0883;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size6_7_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size6_7_reg.dpq_size6), sizeof(uint32) * 0x0883, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size6_7_reg.dpq_size7), sizeof(uint32) * 0x0883, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size8-9 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size8_9_reg.addr.base = sizeof(uint32) * 0x0884;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size8_9_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size8_9_reg.dpq_size8), sizeof(uint32) * 0x0884, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size8_9_reg.dpq_size9), sizeof(uint32) * 0x0884, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size10-11 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size10_11_reg.addr.base = sizeof(uint32) * 0x0885;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size10_11_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size10_11_reg.dpq_size10), sizeof(uint32) * 0x0885, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size10_11_reg.dpq_size11), sizeof(uint32) * 0x0885, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size12-13 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size12_13_reg.addr.base = sizeof(uint32) * 0x0886;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size12_13_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size12_13_reg.dpq_size12), sizeof(uint32) * 0x0886, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size12_13_reg.dpq_size13), sizeof(uint32) * 0x0886, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Size14-15 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size14_15_reg.addr.base = sizeof(uint32) * 0x0887;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size14_15_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size14_15_reg.dpq_size14), sizeof(uint32) * 0x0887, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_size14_15_reg.dpq_size15), sizeof(uint32) * 0x0887, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address0-1 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address0_1_reg.addr.base = sizeof(uint32) * 0x0890;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address0_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address0_1_reg.dpq_start0), sizeof(uint32) * 0x0890, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address0_1_reg.dpq_start1), sizeof(uint32) * 0x0890, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address2-3 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address2_3_reg.addr.base = sizeof(uint32) * 0x0891;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address2_3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address2_3_reg.dpq_start2), sizeof(uint32) * 0x0891, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address2_3_reg.dpq_start3), sizeof(uint32) * 0x0891, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address4-5 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address4_5_reg.addr.base = sizeof(uint32) * 0x0892;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address4_5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address4_5_reg.dpq_start4), sizeof(uint32) * 0x0892, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address4_5_reg.dpq_start5), sizeof(uint32) * 0x0892, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address6-7 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address6_7_reg.addr.base = sizeof(uint32) * 0x0893;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address6_7_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address6_7_reg.dpq_start6), sizeof(uint32) * 0x0893, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address6_7_reg.dpq_start7), sizeof(uint32) * 0x0893, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address8-9 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address8_9_reg.addr.base = sizeof(uint32) * 0x0894;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address8_9_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address8_9_reg.dpq_start8), sizeof(uint32) * 0x0894, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address8_9_reg.dpq_start9), sizeof(uint32) * 0x0894, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address10-11 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address10_11_reg.addr.base = sizeof(uint32) * 0x0895;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address10_11_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address10_11_reg.dpq_start10), sizeof(uint32) * 0x0895, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address10_11_reg.dpq_start11), sizeof(uint32) * 0x0895, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address12-13 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address12_13_reg.addr.base = sizeof(uint32) * 0x0896;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address12_13_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address12_13_reg.dpq_start12), sizeof(uint32) * 0x0896, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address12_13_reg.dpq_start13), sizeof(uint32) * 0x0896, sizeof(uint32) * 0x0000, 27, 16);

  /* DRAMBuffer Pointer Queue Start Address14-15 */
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address14_15_reg.addr.base = sizeof(uint32) * 0x0897;
  Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address14_15_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address14_15_reg.dpq_start14), sizeof(uint32) * 0x0897, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.drambuffer_pointer_queue_start_address14_15_reg.dpq_start15), sizeof(uint32) * 0x0897, sizeof(uint32) * 0x0000, 27, 16);

  /* Transmit Data Queue Size0-7 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_TRANSMIT_DATA_QUEUE_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ipt.transmit_data_queue_size_reg[reg_idx].addr.base = sizeof(uint32) * (0x0900 + reg_idx);
    Soc_petra_a_regs.ipt.transmit_data_queue_size_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;

    for (fld_idx = 0; fld_idx < SOC_PETRA_TRANSMIT_DATA_QUEUE_NOF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.ipt.transmit_data_queue_size_reg[reg_idx].dtq_size[fld_idx]),
        sizeof(uint32) * (0x0900 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(8 + (16*fld_idx)),
        (uint8)(0 + (16*fld_idx))
      );
    }
  }

  /* Transmit Data Queue Start Adress0-7 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_TRANSMIT_DATA_QUEUE_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.ipt.transmit_data_queue_start_adress_reg[reg_idx].addr.base = sizeof(uint32) * (0x0910 + reg_idx);
    Soc_petra_a_regs.ipt.transmit_data_queue_start_adress_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;

    for (fld_idx = 0; fld_idx < SOC_PETRA_TRANSMIT_DATA_QUEUE_NOF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.ipt.transmit_data_queue_start_adress_reg[reg_idx].dtq_start[fld_idx]),
        sizeof(uint32) * (0x0910 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(8 + (16*fld_idx)),
        (uint8)(0 + (16*fld_idx))
      );
    }
  }

  /* Transmit Data Queue Threshold0-1 */
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold0_1_reg.addr.base = sizeof(uint32) * 0x0920;
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold0_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold0_1_reg.dtq_th0), sizeof(uint32) * 0x0920, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold0_1_reg.dtq_th1), sizeof(uint32) * 0x0920, sizeof(uint32) * 0x0000, 24, 16);

  /* Transmit Data Queue Threshold2-3 */
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold2_3_reg.addr.base = sizeof(uint32) * 0x0921;
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold2_3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold2_3_reg.dtq_th2), sizeof(uint32) * 0x0921, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold2_3_reg.dtq_th3), sizeof(uint32) * 0x0921, sizeof(uint32) * 0x0000, 24, 16);

  /* Transmit Data Queue Threshold4-5 */
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold4_5_reg.addr.base = sizeof(uint32) * 0x0922;
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold4_5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold4_5_reg.dtq_th4), sizeof(uint32) * 0x0922, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold4_5_reg.dtq_th5), sizeof(uint32) * 0x0922, sizeof(uint32) * 0x0000, 24, 16);

  /* Transmit Data Queue Threshold6 */
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold6_reg.addr.base = sizeof(uint32) * 0x0923;
  Soc_petra_a_regs.ipt.transmit_data_queue_threshold6_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.transmit_data_queue_threshold6_reg.dtq_th6), sizeof(uint32) * 0x0923, sizeof(uint32) * 0x0000, 8, 0);

  /* Shaper0-7 Max Credit */
  for (reg_idx = 0; reg_idx< SOC_PETRA_NOF_SHAPER_MAX_CREDIT_REGS; ++reg_idx)
  {
    Soc_petra_a_regs.ipt.shaper_max_credit_reg[reg_idx].addr.base = sizeof(uint32) * (0x0930 + reg_idx);
    Soc_petra_a_regs.ipt.shaper_max_credit_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper_max_credit_reg[reg_idx].shaper0_max_credit), sizeof(uint32) * (0x0930 + reg_idx), sizeof(uint32) * 0x0000, 15, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper_max_credit_reg[reg_idx].shaper1_max_credit), sizeof(uint32) * (0x0930 + reg_idx), sizeof(uint32) * 0x0000, 31, 16);
  }

  /* Shaper8 Max Credit */
  Soc_petra_a_regs.ipt.shaper8_max_credit_reg.addr.base = sizeof(uint32) * 0x0934;
  Soc_petra_a_regs.ipt.shaper8_max_credit_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper8_max_credit_reg.shaper8_max_credit), sizeof(uint32) * 0x0934, sizeof(uint32) * 0x0000, 15, 0);

  /* Shaper0-7 Delay */
  for (reg_idx = 0; reg_idx< SOC_PETRA_NOF_SHAPER_DELAY_REGS; ++reg_idx)
  {
    Soc_petra_a_regs.ipt.shaper_delay_reg[reg_idx].addr.base = sizeof(uint32) * (0x0940 + reg_idx);
    Soc_petra_a_regs.ipt.shaper_delay_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper_delay_reg[reg_idx].shaper0_delay), sizeof(uint32) * (0x0940 + reg_idx), sizeof(uint32) * 0x0000, 15, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper_delay_reg[reg_idx].shaper1_delay), sizeof(uint32) * (0x0940 + reg_idx), sizeof(uint32) * 0x0000, 31, 16);
  }

  /* Shaper8 Delay */
  Soc_petra_a_regs.ipt.shaper8_delay_reg.addr.base = sizeof(uint32) * 0x0944;
  Soc_petra_a_regs.ipt.shaper8_delay_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper8_delay_reg.shaper8_delay), sizeof(uint32) * 0x0944, sizeof(uint32) * 0x0000, 15, 0);

  /* Shaper0-7 Cal */
  for (reg_idx = 0; reg_idx< SOC_PETRA_NOF_SHAPER_CAL_REGS; ++reg_idx)
  {
    Soc_petra_a_regs.ipt.shaper_cal_reg[reg_idx].addr.base = sizeof(uint32) * (0x0950 + reg_idx);
    Soc_petra_a_regs.ipt.shaper_cal_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper_cal_reg[reg_idx].shaper0_cal), sizeof(uint32) * (0x0950 + reg_idx), sizeof(uint32) * 0x0000, 15, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper_cal_reg[reg_idx].shaper1_cal), sizeof(uint32) * (0x0950 + reg_idx), sizeof(uint32) * 0x0000, 31, 16);
  }

  /* Shaper8 Cal */
  Soc_petra_a_regs.ipt.shaper8_cal_reg.addr.base = sizeof(uint32) * 0x0954;
  Soc_petra_a_regs.ipt.shaper8_cal_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.shaper8_cal_reg.shaper8_cal), sizeof(uint32) * 0x0954, sizeof(uint32) * 0x0000, 15, 0);

  /* WfqWeight0-1 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_WFQ_REGS; ++reg_idx)
  {
    Soc_petra_a_regs.ipt.wfq_weight_reg[reg_idx].addr.base = sizeof(uint32) * (0x0960 + reg_idx);
    Soc_petra_a_regs.ipt.wfq_weight_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.wfq_weight_reg[reg_idx].wfq0_weight), sizeof(uint32) * (0x0960 + reg_idx), sizeof(uint32) * 0x0000, 5, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.wfq_weight_reg[reg_idx].wfq1_weight), sizeof(uint32) * (0x0960 + reg_idx), sizeof(uint32) * 0x0000, 13, 8);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.wfq_weight_reg[reg_idx].wfq2_weight), sizeof(uint32) * (0x0960 + reg_idx), sizeof(uint32) * 0x0000, 21, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.wfq_weight_reg[reg_idx].wfq3_weight), sizeof(uint32) * (0x0960 + reg_idx), sizeof(uint32) * 0x0000, 29, 24);
  }
  /* Stamping Fabric Header Enable */
  Soc_petra_a_regs.ipt.stamping_fabric_header_enable_reg.addr.base = sizeof(uint32) * 0x0970;
  Soc_petra_a_regs.ipt.stamping_fabric_header_enable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.stamping_fabric_header_enable_reg.stamp_fap_port), sizeof(uint32) * 0x0970, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.stamping_fabric_header_enable_reg.stamp_dp), sizeof(uint32) * 0x0970, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.stamping_fabric_header_enable_reg.stamp_fwdaction), sizeof(uint32) * 0x0970, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.stamping_fabric_header_enable_reg.stamp_outlif), sizeof(uint32) * 0x0970, sizeof(uint32) * 0x0000, 3, 3);

  /* Mapping Queue Type To Snoop Packet */
  Soc_petra_a_regs.ipt.mapping_queue_type_to_snoop_packet_reg.addr.base = sizeof(uint32) * 0x0971;
  Soc_petra_a_regs.ipt.mapping_queue_type_to_snoop_packet_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.mapping_queue_type_to_snoop_packet_reg.snoop_table), sizeof(uint32) * 0x0971, sizeof(uint32) * 0x0000, 3, 0);

  /* Crc Error Counter */
  Soc_petra_a_regs.ipt.crc_error_counter_reg.addr.base = sizeof(uint32) * 0x0972;
  Soc_petra_a_regs.ipt.crc_error_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crc_error_counter_reg.crc_err_cnt), sizeof(uint32) * 0x0972, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.crc_error_counter_reg.crc_err_cnt_ovf), sizeof(uint32) * 0x0972, sizeof(uint32) * 0x0000, 31, 31);

  /* Msb Mirror Data Bus */
  Soc_petra_a_regs.ipt.msb_mirror_data_bus_reg.addr.base = sizeof(uint32) * 0x0973;
  Soc_petra_a_regs.ipt.msb_mirror_data_bus_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.msb_mirror_data_bus_reg.msb_mdata_bus), sizeof(uint32) * 0x0973, sizeof(uint32) * 0x0000, 17, 0);

  /* Lsb Mirror Data Bus */
  Soc_petra_a_regs.ipt.lsb_mirror_data_bus_reg.addr.base = sizeof(uint32) * 0x0974;
  Soc_petra_a_regs.ipt.lsb_mirror_data_bus_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.lsb_mirror_data_bus_reg.lsb_mdata_bus), sizeof(uint32) * 0x0974, sizeof(uint32) * 0x0000, 17, 0);

  /* Check Bw To Qnum */
  Soc_petra_a_regs.ipt.check_bw_to_qnum_reg.addr.base = sizeof(uint32) * 0x0975;
  Soc_petra_a_regs.ipt.check_bw_to_qnum_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.check_bw_to_qnum_reg.qnum_to_check_bw), sizeof(uint32) * 0x0975, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.check_bw_to_qnum_reg.check_bw_to_qnum), sizeof(uint32) * 0x0975, sizeof(uint32) * 0x0000, 4, 4);

  /* Check Bw To Fdt Qnum */
  Soc_petra_a_regs.ipt.check_bw_to_fdt_qnum_reg.addr.base = sizeof(uint32) * 0x0976;
  Soc_petra_a_regs.ipt.check_bw_to_fdt_qnum_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.check_bw_to_fdt_qnum_reg.fdt_qnum_to_check_bw), sizeof(uint32) * 0x0976, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.check_bw_to_fdt_qnum_reg.check_bw_to_fdt_qnum), sizeof(uint32) * 0x0976, sizeof(uint32) * 0x0000, 3, 3);

  /* Gtimer Config */
  Soc_petra_a_regs.ipt.gtimer_config_reg.addr.base = sizeof(uint32) * 0x0977;
  Soc_petra_a_regs.ipt.gtimer_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gtimer_config_reg.timer_config), sizeof(uint32) * 0x0977, sizeof(uint32) * 0x0000, 30, 0);

  /* Gtimer Config Cont */
  Soc_petra_a_regs.ipt.gtimer_config_cont_reg.addr.base = sizeof(uint32) * 0x0978;
  Soc_petra_a_regs.ipt.gtimer_config_cont_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gtimer_config_cont_reg.clear_gtimer), sizeof(uint32) * 0x0978, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gtimer_config_cont_reg.activate_gtimer), sizeof(uint32) * 0x0978, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.gtimer_config_cont_reg.gtimer_active), sizeof(uint32) * 0x0978, sizeof(uint32) * 0x0000, 2, 2);

  /* Select Source To Count */
  Soc_petra_a_regs.ipt.select_source_to_count_reg.addr.base = sizeof(uint32) * 0x0979;
  Soc_petra_a_regs.ipt.select_source_to_count_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.select_source_to_count_reg.select_source_to_count), sizeof(uint32) * 0x0979, sizeof(uint32) * 0x0000, 2, 0);

  /* Select Source To Sum */
  Soc_petra_a_regs.ipt.select_source_to_sum_reg.addr.base = sizeof(uint32) * 0x0980;
  Soc_petra_a_regs.ipt.select_source_to_sum_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.ipt.select_source_to_sum_reg.select_source_to_sum), sizeof(uint32) * 0x0980, sizeof(uint32) * 0x0000, 0, 0);
}

/* Block registers initialization: MMU */
STATIC void
  soc_pa_regs_init_MMU(void)
{
  Soc_petra_a_regs.mmu.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_MMU;
  Soc_petra_a_regs.mmu.addr.base = sizeof(uint32) * 0x0a00;
  Soc_petra_a_regs.mmu.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.mmu.interrupt_reg.addr.base = sizeof(uint32) * 0x0a00;
  Soc_petra_a_regs.mmu.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.interrupt_reg.error_ecc), sizeof(uint32) * 0x0a00, sizeof(uint32) * 0x0000, 0, 0);

  /* Ecc Interrupt Register */
  Soc_petra_a_regs.mmu.ecc_interrupt_reg.addr.base = sizeof(uint32) * 0x0a01;
  Soc_petra_a_regs.mmu.ecc_interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafaa_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafab_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafac_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafad_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafae_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafaf_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafba_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafbb_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafbc_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafbd_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafbe_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wafbf_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rafa_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rafb_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rafc_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rafd_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rafe_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.raff_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wfafa_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wfafb_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wfafc_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wfafd_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wfafe_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.wfaff_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rfafa_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rfafb_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rfafc_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rfafd_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rfafe_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.rfaff_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.idf_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_reg.fdf_ecc_err), sizeof(uint32) * 0x0a01, sizeof(uint32) * 0x0000, 31, 31);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.mmu.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x0a10;
  Soc_petra_a_regs.mmu.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.interrupt_mask_reg.error_ecc_mask), sizeof(uint32) * 0x0a10, sizeof(uint32) * 0x0000, 0, 0);

  /* Ecc Interrupt Register Mask */
  Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.addr.base = sizeof(uint32) * 0x0a11;
  Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafaa_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafab_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafac_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafad_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafae_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafaf_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafba_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafbb_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafbc_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafbd_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafbe_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wafbf_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rafa_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rafb_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rafc_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rafd_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rafe_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.raff_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wfafa_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wfafb_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wfafc_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wfafd_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wfafe_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.wfaff_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rfafa_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rfafb_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rfafc_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rfafd_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rfafe_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.rfaff_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.idf_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_interrupt_register_mask_reg.fdf_ecc_err_mask), sizeof(uint32) * 0x0a11, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x0a20;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x0a20, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x0a21;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x0a21, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x0a22;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x0a22, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x0a23;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x0a23, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x0a24;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x0a24, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x0a25;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x0a25, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_6.addr.base = sizeof(uint32) * 0x0a26;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_6.indirect_command_wr_data), sizeof(uint32) * 0x0a26, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_7.addr.base = sizeof(uint32) * 0x0a27;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_7.indirect_command_wr_data), sizeof(uint32) * 0x0a27, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_8.addr.base = sizeof(uint32) * 0x0a28;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_8.indirect_command_wr_data), sizeof(uint32) * 0x0a28, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_9.addr.base = sizeof(uint32) * 0x0a29;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_9.indirect_command_wr_data), sizeof(uint32) * 0x0a29, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_10.addr.base = sizeof(uint32) * 0x0a2a;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_10.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_10.indirect_command_wr_data), sizeof(uint32) * 0x0a2a, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_11.addr.base = sizeof(uint32) * 0x0a2b;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_11.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_11.indirect_command_wr_data), sizeof(uint32) * 0x0a2b, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_12.addr.base = sizeof(uint32) * 0x0a2c;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_12.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_12.indirect_command_wr_data), sizeof(uint32) * 0x0a2c, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_13.addr.base = sizeof(uint32) * 0x0a2d;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_13.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_13.indirect_command_wr_data), sizeof(uint32) * 0x0a2d, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_14.addr.base = sizeof(uint32) * 0x0a2e;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_14.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_14.indirect_command_wr_data), sizeof(uint32) * 0x0a2e, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_15.addr.base = sizeof(uint32) * 0x0a2f;
  Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_15.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_wr_data_reg_15.indirect_command_wr_data), sizeof(uint32) * 0x0a2f, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x0a30;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x0a30, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x0a31;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x0a31, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x0a32;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x0a32, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x0a33;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x0a33, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x0a34;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x0a34, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x0a35;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x0a35, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_6.addr.base = sizeof(uint32) * 0x0a36;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_6.indirect_command_rd_data), sizeof(uint32) * 0x0a36, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_7.addr.base = sizeof(uint32) * 0x0a37;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_7.indirect_command_rd_data), sizeof(uint32) * 0x0a37, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_8.addr.base = sizeof(uint32) * 0x0a38;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_8.indirect_command_rd_data), sizeof(uint32) * 0x0a38, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_9.addr.base = sizeof(uint32) * 0x0a39;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_9.indirect_command_rd_data), sizeof(uint32) * 0x0a39, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_10.addr.base = sizeof(uint32) * 0x0a3a;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_10.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_10.indirect_command_rd_data), sizeof(uint32) * 0x0a3a, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_11.addr.base = sizeof(uint32) * 0x0a3b;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_11.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_11.indirect_command_rd_data), sizeof(uint32) * 0x0a3b, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_12.addr.base = sizeof(uint32) * 0x0a3c;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_12.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_12.indirect_command_rd_data), sizeof(uint32) * 0x0a3c, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_13.addr.base = sizeof(uint32) * 0x0a3d;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_13.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_13.indirect_command_rd_data), sizeof(uint32) * 0x0a3d, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_14.addr.base = sizeof(uint32) * 0x0a3e;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_14.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_14.indirect_command_rd_data), sizeof(uint32) * 0x0a3e, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_15.addr.base = sizeof(uint32) * 0x0a3f;
  Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_15.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_rd_data_reg_15.indirect_command_rd_data), sizeof(uint32) * 0x0a3f, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.mmu.indirect_command_reg.addr.base = sizeof(uint32) * 0x0a40;
  Soc_petra_a_regs.mmu.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x0a40, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x0a40, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x0a40, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x0a40, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x0a40, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.mmu.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x0a41;
  Soc_petra_a_regs.mmu.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x0a41, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x0a41, sizeof(uint32) * 0x0000, 31, 31);

  /* General Configuration Register */
  Soc_petra_a_regs.mmu.general_configuration_reg.addr.base = sizeof(uint32) * 0x0a60;
  Soc_petra_a_regs.mmu.general_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.dram_bank_num), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.dram_num), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.scrambling_bit_position), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 7, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.dram_col_num), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.bank_interleaving_mode), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.interleaving_rvrse_mode), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.cons_banks_addr_map_mode), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.layer_periodicity), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 19, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.read_fifos_allowed), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.general_configuration_reg.min_read_commands), sizeof(uint32) * 0x0a60, sizeof(uint32) * 0x0000, 31, 24);

  /* Bank Access Controller Configurations */
  Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.addr.base = sizeof(uint32) * 0x0a61;
  Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.write_inhibit_cnt_sat), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.address_map_config_gen), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 14, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.address_map_config_rd), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 18, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.wr_cmd_distance), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.allow_consecutive16_byte), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.per_bank_half_mode), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.bank_access_controller_configurations_reg.wr_priority_mode), sizeof(uint32) * 0x0a61, sizeof(uint32) * 0x0000, 29, 28);

  /* Ipt Data Fifo Configuration Register */
  Soc_petra_a_regs.mmu.ipt_data_fifo_configuration_reg.addr.base = sizeof(uint32) * 0x0a62;
  Soc_petra_a_regs.mmu.ipt_data_fifo_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ipt_data_fifo_configuration_reg.idf_pkt_th), sizeof(uint32) * 0x0a62, sizeof(uint32) * 0x0000, 9, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ipt_data_fifo_configuration_reg.idf_size_th), sizeof(uint32) * 0x0a62, sizeof(uint32) * 0x0000, 30, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ipt_data_fifo_configuration_reg.idf_use_size_th), sizeof(uint32) * 0x0a62, sizeof(uint32) * 0x0000, 31, 31);

  /* Global Time Counter */
  Soc_petra_a_regs.mmu.global_time_counter_reg.addr.base = sizeof(uint32) * 0x0a66;
  Soc_petra_a_regs.mmu.global_time_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.global_time_counter_reg.gt_prd), sizeof(uint32) * 0x0a66, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.global_time_counter_reg.gt_rst_cntrs), sizeof(uint32) * 0x0a66, sizeof(uint32) * 0x0000, 31, 31);

  /* Global Time Counter Trigger */
  Soc_petra_a_regs.mmu.global_time_counter_trigger_reg.addr.base = sizeof(uint32) * 0x0a67;
  Soc_petra_a_regs.mmu.global_time_counter_trigger_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.global_time_counter_trigger_reg.glb_cnt_trg), sizeof(uint32) * 0x0a67, sizeof(uint32) * 0x0000, 0, 0);

  /* Global Time Counter Configuration */
  Soc_petra_a_regs.mmu.global_time_counter_configuration_reg.addr.base = sizeof(uint32) * 0x0a68;
  Soc_petra_a_regs.mmu.global_time_counter_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.global_time_counter_configuration_reg.mmu_cnt_by_gt), sizeof(uint32) * 0x0a68, sizeof(uint32) * 0x0000, 0, 0);

  /* ECC Configuration register */
  Soc_petra_a_regs.mmu.ecc_configuration_reg.addr.base = sizeof(uint32) * 0x0a70;
  Soc_petra_a_regs.mmu.ecc_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mmu.ecc_configuration_reg.dis_ecc), sizeof(uint32) * 0x0a70, sizeof(uint32) * 0x0000, 0, 0);
}

/* Block registers initialization: DRC */
STATIC void
  soc_pa_regs_init_DRC(void)
{
  uint32
    reg_idx;
  Soc_petra_a_regs.drc.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_DRC;
  Soc_petra_a_regs.drc.addr.base = sizeof(uint32) * 0x1800;
  Soc_petra_a_regs.drc.addr.step = sizeof(uint32) * 0x0200;

  /* DDR Controller Triggers */
  Soc_petra_a_regs.drc.ddr_controller_triggers_reg.addr.base = sizeof(uint32) * 0x1800;
  Soc_petra_a_regs.drc.ddr_controller_triggers_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_controller_triggers_reg.ddrrstn), sizeof(uint32) * 0x1800, sizeof(uint32) * 0x0200, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_controller_triggers_reg.ddrdef_valn), sizeof(uint32) * 0x1800, sizeof(uint32) * 0x0200, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_controller_triggers_reg.ddrinit_dis), sizeof(uint32) * 0x1800, sizeof(uint32) * 0x0200, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_controller_triggers_reg.ddrcke_dis), sizeof(uint32) * 0x1800, sizeof(uint32) * 0x0200, 3, 3);

  /* DDR Mode Register1 */
  Soc_petra_a_regs.drc.ddr_mode_1_reg.addr.base = sizeof(uint32) * 0x1801;
  Soc_petra_a_regs.drc.ddr_mode_1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_mode_1_reg.mode_reg_wr1), sizeof(uint32) * 0x1801, sizeof(uint32) * 0x0200, 31, 0);

  /* DDR Mode Register2 */
  Soc_petra_a_regs.drc.ddr_mode_2_reg.addr.base = sizeof(uint32) * 0x1802;
  Soc_petra_a_regs.drc.ddr_mode_2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_mode_2_reg.mode_reg_wr2), sizeof(uint32) * 0x1802, sizeof(uint32) * 0x0200, 31, 0);

  /* DDR Extended Mode Register1 */
  Soc_petra_a_regs.drc.ddr_extended_mode_1_reg.addr.base = sizeof(uint32) * 0x1803;
  Soc_petra_a_regs.drc.ddr_extended_mode_1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_extended_mode_1_reg.ext_mode_wr1), sizeof(uint32) * 0x1803, sizeof(uint32) * 0x0200, 31, 0);

  /* AC Operating Conditions1 */
  Soc_petra_a_regs.drc.ac_operating_conditions1_reg.addr.base = sizeof(uint32) * 0x1804;
  Soc_petra_a_regs.drc.ac_operating_conditions1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions1_reg.ddrt_rst), sizeof(uint32) * 0x1804, sizeof(uint32) * 0x0200, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions1_reg.ddrt_dll), sizeof(uint32) * 0x1804, sizeof(uint32) * 0x0200, 21, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions1_reg.ddrt_rc), sizeof(uint32) * 0x1804, sizeof(uint32) * 0x0200, 28, 22);

  /* AC Operating Conditions2 */
  Soc_petra_a_regs.drc.ac_operating_conditions2_reg.addr.base = sizeof(uint32) * 0x1805;
  Soc_petra_a_regs.drc.ac_operating_conditions2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions2_reg.ddrt_rrd), sizeof(uint32) * 0x1805, sizeof(uint32) * 0x0200, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions2_reg.ddrt_rfc), sizeof(uint32) * 0x1805, sizeof(uint32) * 0x0200, 11, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions2_reg.ddrt_rcdrd), sizeof(uint32) * 0x1805, sizeof(uint32) * 0x0200, 16, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions2_reg.ddrt_rcdwr), sizeof(uint32) * 0x1805, sizeof(uint32) * 0x0200, 24, 20);

  /* Init Sequence Register */
  Soc_petra_a_regs.drc.init_sequence_reg.addr.base = sizeof(uint32) * 0x1806;
  Soc_petra_a_regs.drc.init_sequence_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.init_sequence_reg.init_wait_prd), sizeof(uint32) * 0x1806, sizeof(uint32) * 0x0200, 6, 0);

  /* AC Operating Conditions3 */
  Soc_petra_a_regs.drc.ac_operating_conditions3_reg.addr.base = sizeof(uint32) * 0x1807;
  Soc_petra_a_regs.drc.ac_operating_conditions3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions3_reg.cnt_rasrdprd), sizeof(uint32) * 0x1807, sizeof(uint32) * 0x0200, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions3_reg.cnt_raswrprd), sizeof(uint32) * 0x1807, sizeof(uint32) * 0x0200, 13, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions3_reg.cnt_rdapprd), sizeof(uint32) * 0x1807, sizeof(uint32) * 0x0200, 19, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions3_reg.cnt_wrapprd), sizeof(uint32) * 0x1807, sizeof(uint32) * 0x0200, 25, 20);

  /* CPU Commands */
  Soc_petra_a_regs.drc.cpu_commands_reg.addr.base = sizeof(uint32) * 0x1808;
  Soc_petra_a_regs.drc.cpu_commands_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.bank_address), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.address), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 17, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.we_n), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.cas_n), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.ras_n_or_ref_n), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.cs_n), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.cke), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.res), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.odt), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.const_val), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.stop_baccmds), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.cpu_commands_reg.cpu_cmdvld), sizeof(uint32) * 0x1808, sizeof(uint32) * 0x0200, 31, 31);

  /* Training Sequence */
  Soc_petra_a_regs.drc.training_sequence_reg.addr.base = sizeof(uint32) * 0x1809;
  Soc_petra_a_regs.drc.training_sequence_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_reg.ddrtrn_seq_gen_prd), sizeof(uint32) * 0x1809, sizeof(uint32) * 0x0200, 12, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_reg.ddrtrn_seq_gen_num), sizeof(uint32) * 0x1809, sizeof(uint32) * 0x0200, 21, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_reg.start_train_seq), sizeof(uint32) * 0x1809, sizeof(uint32) * 0x0200, 31, 31);

  for (reg_idx = 0; reg_idx < SOC_PETRA_DRAM_NOF_TRAIN_SEQ_WORDS_REGS ; ++reg_idx)
  {
    /* Training Sequence Word */
    Soc_petra_a_regs.drc.training_sequence_word_reg[reg_idx].addr.base = sizeof(uint32) * (0x180a + reg_idx);
    Soc_petra_a_regs.drc.training_sequence_word_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_word_reg[reg_idx].ddrtrn_seq), sizeof(uint32) * (0x180a + reg_idx), sizeof(uint32) * 0x0200, 31, 0);
  }

  /* Training Sequence Address */
  Soc_petra_a_regs.drc.training_sequence_address_reg.addr.base = sizeof(uint32) * 0x1812;
  Soc_petra_a_regs.drc.training_sequence_address_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_address_reg.trn_seq_add), sizeof(uint32) * 0x1812, sizeof(uint32) * 0x0200, 25, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_address_reg.trn_seq_add_val), sizeof(uint32) * 0x1812, sizeof(uint32) * 0x0200, 28, 28);

  /* DRC General Configurations */
  Soc_petra_a_regs.drc.drc_general_configurations_reg.addr.base = sizeof(uint32) * 0x1813;
  Soc_petra_a_regs.drc.drc_general_configurations_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.drc_general_configurations_reg.num_cols), sizeof(uint32) * 0x1813, sizeof(uint32) * 0x0200, 6, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.drc_general_configurations_reg.apbit_pos), sizeof(uint32) * 0x1813, sizeof(uint32) * 0x0200, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.drc_general_configurations_reg.refresh_burst_size), sizeof(uint32) * 0x1813, sizeof(uint32) * 0x0200, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.drc_general_configurations_reg.refresh_delay_prd), sizeof(uint32) * 0x1813, sizeof(uint32) * 0x0200, 29, 24);

  /* Write Read Rates */
  Soc_petra_a_regs.drc.write_read_rates_reg.addr.base = sizeof(uint32) * 0x1814;
  Soc_petra_a_regs.drc.write_read_rates_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.write_read_rates_reg.cnt_wr_prd), sizeof(uint32) * 0x1814, sizeof(uint32) * 0x0200, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.write_read_rates_reg.cnt_rd_prd), sizeof(uint32) * 0x1814, sizeof(uint32) * 0x0200, 13, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.write_read_rates_reg.ddrt_faw), sizeof(uint32) * 0x1814, sizeof(uint32) * 0x0200, 22, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.write_read_rates_reg.ddrt_zqcs), sizeof(uint32) * 0x1814, sizeof(uint32) * 0x0200, 31, 24);

  /* DRAM Compliance Configuration Register */
  Soc_petra_a_regs.drc.dram_compliance_configuration_reg.addr.base = sizeof(uint32) * 0x1815;
  Soc_petra_a_regs.drc.dram_compliance_configuration_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.wr_latency), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.burst_size_mode), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.dramtype), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.addr_term_half), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.gddr3_dummy_write), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.static_odt_en), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.ddrreset_polarity), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.enable_wr_lvl), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dram_compliance_configuration_reg.enable8_banks), sizeof(uint32) * 0x1815, sizeof(uint32) * 0x0200, 20, 20);

  /* Extended Mode WR2 Register */
  Soc_petra_a_regs.drc.extended_mode_wr2_reg.addr.base = sizeof(uint32) * 0x1816;
  Soc_petra_a_regs.drc.extended_mode_wr2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.extended_mode_wr2_reg.ext_mode_wr2), sizeof(uint32) * 0x1816, sizeof(uint32) * 0x0200, 31, 0);

  /* DDR2 Extended Mode WR3 Register */
  Soc_petra_a_regs.drc.ddr2_extended_mode_wr3_reg.addr.base = sizeof(uint32) * 0x1817;
  Soc_petra_a_regs.drc.ddr2_extended_mode_wr3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr2_extended_mode_wr3_reg.ext_mode_wr3), sizeof(uint32) * 0x1817, sizeof(uint32) * 0x0200, 31, 0);

  /* DDR Extended Mode Register2 */
  Soc_petra_a_regs.drc.ddr_extended_mode_2_reg.addr.base = sizeof(uint32) * 0x1818;
  Soc_petra_a_regs.drc.ddr_extended_mode_2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_extended_mode_2_reg.emr2), sizeof(uint32) * 0x1818, sizeof(uint32) * 0x0200, 13, 0);

  /* DDR Extended Mode Register3 */
  Soc_petra_a_regs.drc.ddr_extended_mode_3_reg.addr.base = sizeof(uint32) * 0x1819;
  Soc_petra_a_regs.drc.ddr_extended_mode_3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ddr_extended_mode_3_reg.emr3), sizeof(uint32) * 0x1819, sizeof(uint32) * 0x0200, 13, 0);

  /* AC Operating Conditions4 */
  Soc_petra_a_regs.drc.ac_operating_conditions4_reg.addr.base = sizeof(uint32) * 0x181a;
  Soc_petra_a_regs.drc.ac_operating_conditions4_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions4_reg.ddrt_refi), sizeof(uint32) * 0x181a, sizeof(uint32) * 0x0200, 12, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions4_reg.cnt_wrrdprd), sizeof(uint32) * 0x181a, sizeof(uint32) * 0x0200, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.ac_operating_conditions4_reg.cnt_rdwrprd), sizeof(uint32) * 0x181a, sizeof(uint32) * 0x0200, 28, 24);

  /* Odt Configuration Register */
  Soc_petra_a_regs.drc.odt_configuration_reg.addr.base = sizeof(uint32) * 0x181b;
  Soc_petra_a_regs.drc.odt_configuration_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.odt_configuration_reg.ddr3_zqcalib_gen_prd), sizeof(uint32) * 0x181b, sizeof(uint32) * 0x0200, 12, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.odt_configuration_reg.dyn_odt_start_delay), sizeof(uint32) * 0x181b, sizeof(uint32) * 0x0200, 20, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.odt_configuration_reg.dyn_odt_length), sizeof(uint32) * 0x181b, sizeof(uint32) * 0x0200, 28, 24);

  /* Odt Configuration Register */
  Soc_petra_a_regs.drc.glue_logic_reg.addr.base = sizeof(uint32) * 0x181f;
  Soc_petra_a_regs.drc.glue_logic_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.glue_logic_reg.glue_logic_reg), sizeof(uint32) * 0x181f, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Configurations */
  Soc_petra_a_regs.drc.bist_configurations_reg.addr.base = sizeof(uint32) * 0x1820;
  Soc_petra_a_regs.drc.bist_configurations_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.write_weight), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.read_weight), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.pattern_bit_mode), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.two_addr_mode), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.prbsmode), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.ind_wr_rd_addr_mode), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_configurations_reg.bist_en), sizeof(uint32) * 0x1820, sizeof(uint32) * 0x0200, 24, 24);

  /* BIST Number Of Actions */
  Soc_petra_a_regs.drc.bist_number_of_actions_reg.addr.base = sizeof(uint32) * 0x1821;
  Soc_petra_a_regs.drc.bist_number_of_actions_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_number_of_actions_reg.bist_num_actions), sizeof(uint32) * 0x1821, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Start Address */
  Soc_petra_a_regs.drc.bist_start_address_reg.addr.base = sizeof(uint32) * 0x1822;
  Soc_petra_a_regs.drc.bist_start_address_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_start_address_reg.bist_start_address), sizeof(uint32) * 0x1822, sizeof(uint32) * 0x0200, 25, 0);

  /* BIST End Address */
  Soc_petra_a_regs.drc.bist_end_address_reg.addr.base = sizeof(uint32) * 0x1823;
  Soc_petra_a_regs.drc.bist_end_address_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_end_address_reg.bist_end_address), sizeof(uint32) * 0x1823, sizeof(uint32) * 0x0200, 25, 0);

  /* BIST Single Bit Mask */
  Soc_petra_a_regs.drc.bist_single_bit_mask_reg.addr.base = sizeof(uint32) * 0x1824;
  Soc_petra_a_regs.drc.bist_single_bit_mask_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_single_bit_mask_reg.bist_single_mask), sizeof(uint32) * 0x1824, sizeof(uint32) * 0x0200, 31, 0);

  for (reg_idx = 0; reg_idx < SOC_PETRA_DRAM_NOF_PATTERN_REGS ; ++reg_idx)
  {
    /* BIST Pattern Word */
    Soc_petra_a_regs.drc.bist_pattern_words_regs[reg_idx].addr.base = sizeof(uint32) * (0x1830 + reg_idx);
    Soc_petra_a_regs.drc.bist_pattern_words_regs[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_words_regs[reg_idx].bist_pattern), sizeof(uint32) * (0x1830 + reg_idx), sizeof(uint32) * 0x0200, 31, 0);
  }

  /* BIST Pattern Word7 */
  Soc_petra_a_regs.drc.bist_pattern_word7_reg.addr.base = sizeof(uint32) * 0x1830;
  Soc_petra_a_regs.drc.bist_pattern_word7_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word7_reg.bist_pattern7), sizeof(uint32) * 0x1830, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word6 */
  Soc_petra_a_regs.drc.bist_pattern_word6_reg.addr.base = sizeof(uint32) * 0x1831;
  Soc_petra_a_regs.drc.bist_pattern_word6_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word6_reg.bist_pattern6), sizeof(uint32) * 0x1831, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word5 */
  Soc_petra_a_regs.drc.bist_pattern_word5_reg.addr.base = sizeof(uint32) * 0x1832;
  Soc_petra_a_regs.drc.bist_pattern_word5_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word5_reg.bist_pattern5), sizeof(uint32) * 0x1832, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word4 */
  Soc_petra_a_regs.drc.bist_pattern_word4_reg.addr.base = sizeof(uint32) * 0x1833;
  Soc_petra_a_regs.drc.bist_pattern_word4_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word4_reg.bist_pattern4), sizeof(uint32) * 0x1833, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word3 */
  Soc_petra_a_regs.drc.bist_pattern_word3_reg.addr.base = sizeof(uint32) * 0x1834;
  Soc_petra_a_regs.drc.bist_pattern_word3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word3_reg.bist_pattern3), sizeof(uint32) * 0x1834, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word2 */
  Soc_petra_a_regs.drc.bist_pattern_word2_reg.addr.base = sizeof(uint32) * 0x1835;
  Soc_petra_a_regs.drc.bist_pattern_word2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word2_reg.bist_pattern2), sizeof(uint32) * 0x1835, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word1 */
  Soc_petra_a_regs.drc.bist_pattern_word1_reg.addr.base = sizeof(uint32) * 0x1836;
  Soc_petra_a_regs.drc.bist_pattern_word1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word1_reg.bist_pattern1), sizeof(uint32) * 0x1836, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Pattern Word0 */
  Soc_petra_a_regs.drc.bist_pattern_word0_reg.addr.base = sizeof(uint32) * 0x1837;
  Soc_petra_a_regs.drc.bist_pattern_word0_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_pattern_word0_reg.bist_pattern0), sizeof(uint32) * 0x1837, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask */
  for (reg_idx = 0; reg_idx < SOC_PETRA_DRAM_NOF_PATTERN_REGS ; ++reg_idx)
  {
    Soc_petra_a_regs.drc.bist_full_mask_word_reg[reg_idx].addr.base = sizeof(uint32) * (0x1838 + reg_idx);
    Soc_petra_a_regs.drc.bist_full_mask_word_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word_reg[reg_idx].bist_full_mask), sizeof(uint32) * (0x1838 + reg_idx), sizeof(uint32) * 0x0200, 31, 0);
  }

  /* BIST Full Mask Word7 */
  Soc_petra_a_regs.drc.bist_full_mask_word7_reg.addr.base = sizeof(uint32) * 0x1838;
  Soc_petra_a_regs.drc.bist_full_mask_word7_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word7_reg.bist_full_mask7), sizeof(uint32) * 0x1838, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word6 */
  Soc_petra_a_regs.drc.bist_full_mask_word6_reg.addr.base = sizeof(uint32) * 0x1839;
  Soc_petra_a_regs.drc.bist_full_mask_word6_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word6_reg.bist_full_mask6), sizeof(uint32) * 0x1839, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word5 */
  Soc_petra_a_regs.drc.bist_full_mask_word5_reg.addr.base = sizeof(uint32) * 0x183a;
  Soc_petra_a_regs.drc.bist_full_mask_word5_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word5_reg.bist_full_mask5), sizeof(uint32) * 0x183a, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word4 */
  Soc_petra_a_regs.drc.bist_full_mask_word4_reg.addr.base = sizeof(uint32) * 0x183b;
  Soc_petra_a_regs.drc.bist_full_mask_word4_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word4_reg.bist_full_mask4), sizeof(uint32) * 0x183b, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word3 */
  Soc_petra_a_regs.drc.bist_full_mask_word3_reg.addr.base = sizeof(uint32) * 0x183c;
  Soc_petra_a_regs.drc.bist_full_mask_word3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word3_reg.bist_full_mask3), sizeof(uint32) * 0x183c, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word2 */
  Soc_petra_a_regs.drc.bist_full_mask_word2_reg.addr.base = sizeof(uint32) * 0x183d;
  Soc_petra_a_regs.drc.bist_full_mask_word2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word2_reg.bist_full_mask2), sizeof(uint32) * 0x183d, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word1 */
  Soc_petra_a_regs.drc.bist_full_mask_word1_reg.addr.base = sizeof(uint32) * 0x183e;
  Soc_petra_a_regs.drc.bist_full_mask_word1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word1_reg.bist_full_mask1), sizeof(uint32) * 0x183e, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Full Mask Word0 */
  Soc_petra_a_regs.drc.bist_full_mask_word0_reg.addr.base = sizeof(uint32) * 0x183f;
  Soc_petra_a_regs.drc.bist_full_mask_word0_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_word0_reg.bist_full_mask0), sizeof(uint32) * 0x183f, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Statuses */
  Soc_petra_a_regs.drc.bist_statuses_reg.addr.base = sizeof(uint32) * 0x1840;
  Soc_petra_a_regs.drc.bist_statuses_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_statuses_reg.bist_finished), sizeof(uint32) * 0x1840, sizeof(uint32) * 0x0200, 0, 0);

  /* BIST Full Mask Error Counter */
  Soc_petra_a_regs.drc.bist_full_mask_error_counter_reg.addr.base = sizeof(uint32) * 0x1841;
  Soc_petra_a_regs.drc.bist_full_mask_error_counter_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_full_mask_error_counter_reg.full_err_cnt), sizeof(uint32) * 0x1841, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Single Bit Mask Error Counter */
  Soc_petra_a_regs.drc.bist_single_bit_mask_error_counter_reg.addr.base = sizeof(uint32) * 0x1842;
  Soc_petra_a_regs.drc.bist_single_bit_mask_error_counter_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_single_bit_mask_error_counter_reg.single_err_cnt), sizeof(uint32) * 0x1842, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Error Occurred */
  Soc_petra_a_regs.drc.bist_error_occurred_reg.addr.base = sizeof(uint32) * 0x1843;
  Soc_petra_a_regs.drc.bist_error_occurred_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_error_occurred_reg.err_occurred), sizeof(uint32) * 0x1843, sizeof(uint32) * 0x0200, 31, 0);

  /* BIST Global Error Counter */
  Soc_petra_a_regs.drc.bist_global_error_counter_reg.addr.base = sizeof(uint32) * 0x1844;
  Soc_petra_a_regs.drc.bist_global_error_counter_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.bist_global_error_counter_reg.global_err_cnt), sizeof(uint32) * 0x1844, sizeof(uint32) * 0x0200, 31, 0);

  /* Training Sequence Configuration Register */
  Soc_petra_a_regs.drc.training_sequence_configuration_reg.addr.base = sizeof(uint32) * 0x1858;
  Soc_petra_a_regs.drc.training_sequence_configuration_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_configuration_reg.train_seq_addr_num), sizeof(uint32) * 0x1858, sizeof(uint32) * 0x0200, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.training_sequence_configuration_reg.train_seq_use_prbs), sizeof(uint32) * 0x1858, sizeof(uint32) * 0x0200, 8, 8);

  /* Duty Cycle */
  Soc_petra_a_regs.drc.dcf.addr.base = sizeof(uint32) * 0x18b0;
  Soc_petra_a_regs.drc.dcf.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dcf.dcf_a), sizeof(uint32) * 0x18b0, sizeof(uint32) * 0x0200, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dcf.dcf_p), sizeof(uint32) * 0x18b0, sizeof(uint32) * 0x0200, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.drc.dcf.dcf_disable), sizeof(uint32) * 0x18b0, sizeof(uint32) * 0x0200, 4, 4);

}

/* Block registers initialization: DPI */
STATIC void
  soc_pa_regs_init_DPI(void)
{
  uint32
    reg_idx;
  uint8
    fld_idx;

  Soc_petra_a_regs.dpi.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_DPI;
  Soc_petra_a_regs.dpi.addr.base = sizeof(uint32) * 0x0c00;
  Soc_petra_a_regs.dpi.addr.step = sizeof(uint32) * 0x0200;

  /* Interrupt Register */
  Soc_petra_a_regs.dpi.interrupt_reg.addr.base = sizeof(uint32) * 0x0c00;
  Soc_petra_a_regs.dpi.interrupt_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.interrupt_reg.dpi_error_int), sizeof(uint32) * 0x0c00, sizeof(uint32) * 0x0200, 0, 0);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.dpi.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x0c10;
  Soc_petra_a_regs.dpi.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.interrupt_mask_reg.dpi_error_int_mask), sizeof(uint32) * 0x0c10, sizeof(uint32) * 0x0200, 0, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.dpi.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x0c20;
  Soc_petra_a_regs.dpi.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x0c20, sizeof(uint32) * 0x0200, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.dpi.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x0c21;
  Soc_petra_a_regs.dpi.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x0c21, sizeof(uint32) * 0x0200, 23, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.dpi.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x0c30;
  Soc_petra_a_regs.dpi.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x0c30, sizeof(uint32) * 0x0200, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.dpi.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x0c31;
  Soc_petra_a_regs.dpi.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x0c31, sizeof(uint32) * 0x0200, 23, 0);

  /* Indirect Command */
  Soc_petra_a_regs.dpi.indirect_command_reg.addr.base = sizeof(uint32) * 0x0c40;
  Soc_petra_a_regs.dpi.indirect_command_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x0c40, sizeof(uint32) * 0x0200, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x0c40, sizeof(uint32) * 0x0200, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x0c40, sizeof(uint32) * 0x0200, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x0c40, sizeof(uint32) * 0x0200, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x0c40, sizeof(uint32) * 0x0200, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.dpi.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x0c41;
  Soc_petra_a_regs.dpi.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x0c41, sizeof(uint32) * 0x0200, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x0c41, sizeof(uint32) * 0x0200, 31, 31);

  /* DPRC enable (A2 and above) */
  Soc_petra_a_regs.dpi.dprc_enable_reg.addr.base = sizeof(uint32) * 0x0c52;
  Soc_petra_a_regs.dpi.dprc_enable_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dprc_enable_reg.dprc_en), sizeof(uint32) * 0x0c52, sizeof(uint32) * 0x0200, 0, 0);

  /* Dpi Init Start */
  Soc_petra_a_regs.dpi.dpi_init_start_reg.addr.base = sizeof(uint32) * 0x0c60;
  Soc_petra_a_regs.dpi.dpi_init_start_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_init_start_reg.dpi_init_start), sizeof(uint32) * 0x0c60, sizeof(uint32) * 0x0200, 31, 0);

  /* dpi_init_reset */
  Soc_petra_a_regs.dpi.dpi_init_reset_reg.addr.base = sizeof(uint32) * 0x0c61;
  Soc_petra_a_regs.dpi.dpi_init_reset_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_init_reset_reg.dpi_init_reset), sizeof(uint32) * 0x0c61, sizeof(uint32) * 0x0200, 31, 0);

  /* dpi_init_status */
  Soc_petra_a_regs.dpi.dpi_init_status_reg.addr.base = sizeof(uint32) * 0x0c62;
  Soc_petra_a_regs.dpi.dpi_init_status_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_init_status_reg.ready), sizeof(uint32) * 0x0c62, sizeof(uint32) * 0x0200, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_init_status_reg.init_dn), sizeof(uint32) * 0x0c62, sizeof(uint32) * 0x0200, 4, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_init_status_reg.dll_mstr_s), sizeof(uint32) * 0x0c62, sizeof(uint32) * 0x0200, 11, 5);

  /* dll_control */
  Soc_petra_a_regs.dpi.dll_control_reg.addr.base = sizeof(uint32) * 0x0c63;
  Soc_petra_a_regs.dpi.dll_control_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dll_control_reg.dll_control), sizeof(uint32) * 0x0c63, sizeof(uint32) * 0x0200, 31, 0);

  /* ddl Periodic Training Register */
  Soc_petra_a_regs.dpi.ddl_periodic_training_reg.addr.base = sizeof(uint32) * 0x0c64;
  Soc_petra_a_regs.dpi.ddl_periodic_training_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_periodic_training_reg.ddl_auto_trn_en), sizeof(uint32) * 0x0c64, sizeof(uint32) * 0x0200, 4, 4);

  for (reg_idx = 0; reg_idx < SOC_PETRA_DRAM_NOF_DLL_REGS ; ++reg_idx)
  {
    /* ddl_ctrl */
    Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].addr.base = sizeof(uint32) * (0x0c65 + (0x3 * reg_idx));
    Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_en), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 0, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_main_ph_sel), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 2, 1);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_ph2_sel), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 4, 3);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].unused4), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 10, 5);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_dl_sel_hg), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 11, 11);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_dl_sel_vec), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 19, 12);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_dl_hc_sel_vec), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 27, 20);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_0[reg_idx].predef_dl_qc_sel_vec), sizeof(uint32) * (0x0c65 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 31, 28);

    /* ddl_ctrl */
    Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].addr.base = sizeof(uint32) * (0x0c66 + (0x3 * reg_idx));
    Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].predef_dl_qc_sel_vec), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 3, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].stat_predef_mode), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 4, 4);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].predef_ph_up), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 8, 5);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].predef_ph_dn), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 12, 9);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].tqc_x8_step_en), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 13, 13);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].fast_dlwalk_inc), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 15, 14);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].phsel_margin), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 17, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].sel_margin), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 19, 18);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].train_trig_threshold), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 21, 20);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].train_trig_en), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 22, 22);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].unused7), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 24, 23);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].unused5), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 27, 25);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].phsel_hc_up), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 28, 28);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].unused6), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 29, 29);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].unused1), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 30, 30);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_1[reg_idx].dly_maxmin_mode), sizeof(uint32) * (0x0c66 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 31, 31);

    /* ddl_ctrl */
    Soc_petra_a_regs.dpi.ddl_ctrl_reg_2[reg_idx].addr.base = sizeof(uint32) * (0x0c67 + (0x3 * reg_idx));
    Soc_petra_a_regs.dpi.ddl_ctrl_reg_2[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_2[reg_idx].ins_dly_comp_threshold), sizeof(uint32) * (0x0c67 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 5, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_2[reg_idx].unused2), sizeof(uint32) * (0x0c67 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 9, 6);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl_ctrl_reg_2[reg_idx].predef_ins_dly_min_vec), sizeof(uint32) * (0x0c67 + (0x3 * reg_idx)), sizeof(uint32) * 0x0200, 15, 10);

    /* ddl1_stat_lsb_reg */
    Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].addr.base = sizeof(uint32) * (0x0c71 + (0x2 * reg_idx));
    Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].ph_sel), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 1, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].main_ph_sel), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 3, 2);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].ph2_sel), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 5, 4);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].dly_maxmin_mode), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 6, 6);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].phsel_error), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 7, 7);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].hc_sel_vec), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 15, 8);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].sel_vec), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 23, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].sel_hg), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 24, 24);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_lsb_reg[reg_idx].qc_sel_vec_0), sizeof(uint32) * (0x0c71 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 31, 25);

    /* ddl1_stat_msb_reg */
    Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].addr.base = sizeof(uint32) * (0x0c72 + (0x2 * reg_idx));
    Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].qc_sel_vec_1), sizeof(uint32) * (0x0c72 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 0, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].train_trigger), sizeof(uint32) * (0x0c72 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 1, 1);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].ph_up), sizeof(uint32) * (0x0c72 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 5, 2);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].phsel_hc_up), sizeof(uint32) * (0x0c72 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 6, 6);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].unused1), sizeof(uint32) * (0x0c72 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 11, 7);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddl1_stat_msb_reg[reg_idx].ins_dly_min_vec), sizeof(uint32) * (0x0c72 + (0x2 * reg_idx)), sizeof(uint32) * 0x0200, 17, 12);
  }

  /* read_gap_mask_reg */
  Soc_petra_a_regs.dpi.read_gap_mask_reg.addr.base = sizeof(uint32) * 0x0c79;
  Soc_petra_a_regs.dpi.read_gap_mask_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.read_gap_mask_reg.read_gap_mask), sizeof(uint32) * 0x0c79, sizeof(uint32) * 0x0200, 31, 0);

  /* IoConfig */
  Soc_petra_a_regs.dpi.io_config_reg.addr.base = sizeof(uint32) * 0x0c7f;
  Soc_petra_a_regs.dpi.io_config_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.tximp45_oct0), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pd50_oct0), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pu50_oct0), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.tximp45_oct1), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pd50_oct1), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pu50_oct1), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.tximp45_oct2), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pd50_oct2), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pu50_oct2), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.tximp45_oct3), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pd50_oct3), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pu50_oct3), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.tximp45_ctrl), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pd50_ctrl), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pu50_ctrl), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.tximp45_ck), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pd50_ck), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.pu50_ck), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.enodtvref), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_config_reg.enable_outputs), sizeof(uint32) * 0x0c7f, sizeof(uint32) * 0x0200, 31, 31);

  /* io_calibration_status_reg */
  Soc_petra_a_regs.dpi.io_calibration_status_reg.addr.base = sizeof(uint32) * 0x0c80;
  Soc_petra_a_regs.dpi.io_calibration_status_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_calibration_status_reg.cr), sizeof(uint32) * 0x0c80, sizeof(uint32) * 0x0200, 4, 0);

  /* Io Odt Read Config */
  Soc_petra_a_regs.dpi.io_odt_read_config_reg.addr.base = sizeof(uint32) * 0x0c81;
  Soc_petra_a_regs.dpi.io_odt_read_config_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_odt_read_config_reg.io_odt_read_rise_aloc), sizeof(uint32) * 0x0c81, sizeof(uint32) * 0x0200, 4, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_odt_read_config_reg.io_odt_read_fall_aloc), sizeof(uint32) * 0x0c81, sizeof(uint32) * 0x0200, 9, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_odt_read_config_reg.io_odt_read_rise_ofset), sizeof(uint32) * 0x0c81, sizeof(uint32) * 0x0200, 14, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_odt_read_config_reg.io_odt_read_fall_ofset), sizeof(uint32) * 0x0c81, sizeof(uint32) * 0x0200, 19, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_odt_read_config_reg.io_odt_read_ofset_en), sizeof(uint32) * 0x0c81, sizeof(uint32) * 0x0200, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.io_odt_read_config_reg.io_odt_disable), sizeof(uint32) * 0x0c81, sizeof(uint32) * 0x0200, 24, 24);

  /* DPI General Config */
  Soc_petra_a_regs.dpi.dpi_general_config_reg.addr.base = sizeof(uint32) * 0x0c82;
  Soc_petra_a_regs.dpi.dpi_general_config_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_general_config_reg.dpi_general_config), sizeof(uint32) * 0x0c82, sizeof(uint32) * 0x0200, 31, 0);

  /* DdrPhyMode */
  Soc_petra_a_regs.dpi.ddr_phy_mode_reg.addr.base = sizeof(uint32) * 0x0c83;
  Soc_petra_a_regs.dpi.ddr_phy_mode_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddr_phy_mode_reg.gddr3_mode), sizeof(uint32) * 0x0c83, sizeof(uint32) * 0x0200, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddr_phy_mode_reg.ddr3_mode), sizeof(uint32) * 0x0c83, sizeof(uint32) * 0x0200, 2, 2);

  /* dpi_rnd_trp_status_reg */
  Soc_petra_a_regs.dpi.dpi_rnd_trp_status_reg.addr.base = sizeof(uint32) * 0x0c84;
  Soc_petra_a_regs.dpi.dpi_rnd_trp_status_reg.addr.step = sizeof(uint32) * 0x0200;
  for (fld_idx = 0; fld_idx < SOC_PETRA_DRAM_NOF_RND_TRP_FLDS ; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_rnd_trp_status_reg.rnd_trp[fld_idx]), sizeof(uint32) * 0x0c84, sizeof(uint32) * 0x0200, 4 + (5 * fld_idx), 0 + (5 * fld_idx));
  }

  /* dpi_rnd_trp_diff_status_reg */
  Soc_petra_a_regs.dpi.dpi_rnd_trp_diff_status_reg.addr.base = sizeof(uint32) * 0x0c85;
  Soc_petra_a_regs.dpi.dpi_rnd_trp_diff_status_reg.addr.step = sizeof(uint32) * 0x0200;
  for (fld_idx = 0; fld_idx < SOC_PETRA_DRAM_NOF_RND_TRP_DIFF_FLDS ; ++fld_idx)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_rnd_trp_diff_status_reg.rnd_trp_diff[fld_idx]), sizeof(uint32) * 0x0c85, sizeof(uint32) * 0x0200, 4 + (5 * fld_idx), 0 + (5 * fld_idx));
  }

  /* ddio_bit_align_control_reg */
  for (reg_idx = 0; reg_idx < SOC_PETRA_DRAM_NOF_SHIFT_REGS ; ++reg_idx)
  {
    Soc_petra_a_regs.dpi.ddio_bit_align_control_reg[reg_idx].addr.base = sizeof(uint32) * (0x0c87 + reg_idx);
    Soc_petra_a_regs.dpi.ddio_bit_align_control_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.ddio_bit_align_control_reg[reg_idx].shift_val), sizeof(uint32) * (0x0c87 + reg_idx), sizeof(uint32) * 0x0200, 31, 0);
  }

  /* Dpi Retrain */
  Soc_petra_a_regs.dpi.dpi_retrain_reg.addr.base = sizeof(uint32) * 0x0c8b;
  Soc_petra_a_regs.dpi.dpi_retrain_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.dpi.dpi_retrain_reg.dpi_retrain), sizeof(uint32) * 0x0c8b, sizeof(uint32) * 0x0200, 4, 0);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x0c90,
    0x0200,
    &(Soc_petra_a_regs.dpi.pcmi_config_reg),
    &(Soc_petra_a_regs.dpi.pcmi_results_reg)
  );
}

/* Block registers initialization: FDR */
STATIC void
  soc_pa_regs_init_FDR(void)
{
  uint32
    reg_idx;

  Soc_petra_a_regs.fdr.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_FDR;
  Soc_petra_a_regs.fdr.addr.base = sizeof(uint32) * 0x2e00;
  Soc_petra_a_regs.fdr.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.fdr.interrupt_reg.addr.base = sizeof(uint32) * 0x2e00;
  Soc_petra_a_regs.fdr.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdradesccnto_a), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdraun_exp_cell), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.ifmafo_a), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.ifmbfo_a), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.cpudatacellfne_a0), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.cpudatacellfne_a1), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.alto_a), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdra_out_of_sync), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdra_tag_ecc_err), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdra_filrter_drop_inta), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdra_filrter_drop_intb), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdradesccnto_b), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdrbun_exp_cell), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.ifmafo_b), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.ifmbfo_b), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.cpudatacellfne_b0), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.cpudatacellfne_b1), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.alto_b_), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdrb_out_of_sync_), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdrb_tag_ecc_err), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdrb_filrter_drop_inta), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdrb_filrter_drop_intb), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdr_outpu_out_of_sync), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_reg.fdr_out_tag_ecc_err), sizeof(uint32) * 0x2e00, sizeof(uint32) * 0x0000, 31, 31);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.fdr.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x2e10;
  Soc_petra_a_regs.fdr.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdradesccnto_aint_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdraun_exp_cell_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.ifmafo_aint_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.ifmbfo_aint_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.cpudatacellfne_a0_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.cpudatacellfne_a1_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.alto_intmask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdra_out_of_sync_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdra_tag_ecc_err_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdra_filrter_drop_intamask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdra_filrter_drop_intbmask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdrbdesccnto_bint_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdrbun_exp_cell_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.ifmafo_bint_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.ifmbfo_bintmask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.cpudatacellfne_b0_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.cpudatacellfne_b1_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.alto_bint_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdrb_out_of_sync_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdrb_tag_ecc_err_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdrb_filrter_drop_intamask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdrb_filrter_drop_intbmask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdr_outpu_out_of_sync_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.interrupt_mask_reg.fdr_out_tag_ecc_err_int_mask), sizeof(uint32) * 0x2e10, sizeof(uint32) * 0x0000, 31, 31);

  /* Fdr Enablers Register */
  Soc_petra_a_regs.fdr.fdr_enablers_reg.addr.base = sizeof(uint32) * 0x2e60;
  Soc_petra_a_regs.fdr.fdr_enablers_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdr_enablers_reg.bits_1_0), sizeof(uint32) * 0x2e60, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdr_enablers_reg.fdrmtch_act), sizeof(uint32) * 0x2e60, sizeof(uint32) * 0x0000, 29, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdr_enablers_reg.fdrmtch_logicnot), sizeof(uint32) * 0x2e60, sizeof(uint32) * 0x0000, 30, 30);

  /* Programmable Data Cell Counter0 */
  Soc_petra_a_regs.fdr.programmable_data_cell_counter0_reg.addr.base = sizeof(uint32) * 0x2e61;
  Soc_petra_a_regs.fdr.programmable_data_cell_counter0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.programmable_data_cell_counter0_reg.pcmtch_lb0), sizeof(uint32) * 0x2e61, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Data Cell Counter1 */
  Soc_petra_a_regs.fdr.programmable_data_cell_counter1_reg.addr.base = sizeof(uint32) * 0x2e62;
  Soc_petra_a_regs.fdr.programmable_data_cell_counter1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.programmable_data_cell_counter1_reg.pcmtch_lb1), sizeof(uint32) * 0x2e62, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Data Cell Counter Mask0 */
  Soc_petra_a_regs.fdr.programmable_data_cell_counter_mask0_reg.addr.base = sizeof(uint32) * 0x2e63;
  Soc_petra_a_regs.fdr.programmable_data_cell_counter_mask0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.programmable_data_cell_counter_mask0_reg.pcmtch_msk0), sizeof(uint32) * 0x2e63, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Data Cell Counter Mask1 */
  Soc_petra_a_regs.fdr.programmable_data_cell_counter_mask1_reg.addr.base = sizeof(uint32) * 0x2e64;
  Soc_petra_a_regs.fdr.programmable_data_cell_counter_mask1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.programmable_data_cell_counter_mask1_reg.pcmtch_msk1), sizeof(uint32) * 0x2e64, sizeof(uint32) * 0x0000, 31, 0);

  /* Link Level Flow Control */
  Soc_petra_a_regs.fdr.link_level_flow_control_reg.addr.base = sizeof(uint32) * 0x2e65;
  Soc_petra_a_regs.fdr.link_level_flow_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.link_level_flow_control_reg.lnk_lvl_fc_th), sizeof(uint32) * 0x2e65, sizeof(uint32) * 0x0000, 7, 0);

  /* Filter Match Input Link */
  Soc_petra_a_regs.fdr.filter_match_input_link_reg.addr.base = sizeof(uint32) * 0x2e66;
  Soc_petra_a_regs.fdr.filter_match_input_link_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.filter_match_input_link_reg.pc_mtch_link), sizeof(uint32) * 0x2e66, sizeof(uint32) * 0x0000, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.filter_match_input_link_reg.pc_mtch_link_on), sizeof(uint32) * 0x2e66, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.filter_match_input_link_reg.crc_mtch_on), sizeof(uint32) * 0x2e66, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.filter_match_input_link_reg.data_mtch_on), sizeof(uint32) * 0x2e66, sizeof(uint32) * 0x0000, 8, 8);

  /* Programmable Cells Counter */
  Soc_petra_a_regs.fdr.programmable_cells_counter_reg.addr.base = sizeof(uint32) * 0x2e68;
  Soc_petra_a_regs.fdr.programmable_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.programmable_cells_counter_reg.prg_cell_cnt), sizeof(uint32) * 0x2e68, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.programmable_cells_counter_reg.prg_cell_cnt_o), sizeof(uint32) * 0x2e68, sizeof(uint32) * 0x0000, 31, 31);

  /* Link Up Status From Fdra */
  Soc_petra_a_regs.fdr.link_up_status_from_fdra_reg.addr.base = sizeof(uint32) * 0x2e69;
  Soc_petra_a_regs.fdr.link_up_status_from_fdra_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.link_up_status_from_fdra_reg.fdrlink_up_a), sizeof(uint32) * 0x2e69, sizeof(uint32) * 0x0000, 17, 0);

  /* Link Up Status From Fdrb */
  Soc_petra_a_regs.fdr.link_up_status_from_fdrb_reg.addr.base = sizeof(uint32) * 0x2e6a;
  Soc_petra_a_regs.fdr.link_up_status_from_fdrb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.link_up_status_from_fdrb_reg.fdrlink_up_b), sizeof(uint32) * 0x2e6a, sizeof(uint32) * 0x0000, 17, 0);

  /* FDROverflows And Fifos Statuses FDRA */
  Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.addr.base = sizeof(uint32) * 0x2e6b;
  Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.cpudata_cell_foa0), sizeof(uint32) * 0x2e6b, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.cpudata_cell_foa1), sizeof(uint32) * 0x2e6b, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.max_ifmaf), sizeof(uint32) * 0x2e6b, sizeof(uint32) * 0x0000, 9, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.max_ifmbf), sizeof(uint32) * 0x2e6b, sizeof(uint32) * 0x0000, 17, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.max_ifmfanum), sizeof(uint32) * 0x2e6b, sizeof(uint32) * 0x0000, 21, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdra_reg.max_ifmfbnum), sizeof(uint32) * 0x2e6b, sizeof(uint32) * 0x0000, 25, 22);

  /* FDROverflows And Fifos Statuses Fdr B */
  Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.addr.base = sizeof(uint32) * 0x2e6c;
  Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.cpudata_cell_fob0), sizeof(uint32) * 0x2e6c, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.cpudata_cell_fob1), sizeof(uint32) * 0x2e6c, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.max_ifmaf), sizeof(uint32) * 0x2e6c, sizeof(uint32) * 0x0000, 9, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.max_ifmbf), sizeof(uint32) * 0x2e6c, sizeof(uint32) * 0x0000, 17, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.max_ifmfanum), sizeof(uint32) * 0x2e6c, sizeof(uint32) * 0x0000, 21, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdroverflows_and_fifos_statuses_fdr_b_reg.max_ifmfbnum), sizeof(uint32) * 0x2e6c, sizeof(uint32) * 0x0000, 25, 22);

  /* Local Fifo Rci Level */
  Soc_petra_a_regs.fdr.local_fifo_rci_level_reg.addr.base = sizeof(uint32) * 0x2e6f;
  Soc_petra_a_regs.fdr.local_fifo_rci_level_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.local_fifo_rci_level_reg.rci_low_level), sizeof(uint32) * 0x2e6f, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.local_fifo_rci_level_reg.rci_high_level), sizeof(uint32) * 0x2e6f, sizeof(uint32) * 0x0000, 22, 16);

  /* Cpu Data Cell A */
  for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS; reg_idx++)
  {
    Soc_petra_a_regs.fdr.cpu_data_cell_a_reg[reg_idx].addr.base = sizeof(uint32) * ( 0x2e70 + reg_idx ) ;
    Soc_petra_a_regs.fdr.cpu_data_cell_a_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.cpu_data_cell_a_reg[reg_idx].cpudata_cell_a), sizeof(uint32) * ( 0x2e70 + reg_idx ), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x2e7a,
    0x0000,
    &(Soc_petra_a_regs.fdr.pcmi_config_reg),
    &(Soc_petra_a_regs.fdr.pcmi_results_reg)
  );

  /* Cpu Data Cell B */
  for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS; reg_idx++)
  {
    Soc_petra_a_regs.fdr.cpu_data_cell_b_reg[reg_idx].addr.base = sizeof(uint32) * ( 0x2e80 + reg_idx ) ;
    Soc_petra_a_regs.fdr.cpu_data_cell_b_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.cpu_data_cell_b_reg[reg_idx].cpudata_cell_b), sizeof(uint32) * ( 0x2e80 + reg_idx ), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Cpu Data Cell C */
  for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS; reg_idx++)
  {
    Soc_petra_a_regs.fdr.cpu_data_cell_c_reg[reg_idx].addr.base = sizeof(uint32) * ( 0x2e90 + reg_idx ) ;
    Soc_petra_a_regs.fdr.cpu_data_cell_c_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.cpu_data_cell_c_reg[reg_idx].cpudata_cell_c), sizeof(uint32) * ( 0x2e90 + reg_idx ), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Cpu Data Cell D */
  for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS; reg_idx++)
  {
    Soc_petra_a_regs.fdr.cpu_data_cell_d_reg[reg_idx].addr.base = sizeof(uint32) * ( 0x2ea0 + reg_idx ) ;
    Soc_petra_a_regs.fdr.cpu_data_cell_d_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.cpu_data_cell_d_reg[reg_idx].cpudata_cell_d), sizeof(uint32) * ( 0x2ea0 + reg_idx ), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* FDRFifo Discard Counter B */
  Soc_petra_a_regs.fdr.fdrfifo_discard_counter_b_reg.addr.base = sizeof(uint32) * 0x2eb1;
  Soc_petra_a_regs.fdr.fdrfifo_discard_counter_b_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdrfifo_discard_counter_b_reg.fdrdesc_cnt_b), sizeof(uint32) * 0x2eb1, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.fdrfifo_discard_counter_b_reg.fdrdesc_cnt_ob), sizeof(uint32) * 0x2eb1, sizeof(uint32) * 0x0000, 31, 31);

  /* Error Filter */
  Soc_petra_a_regs.fdr.error_filter_reg.addr.base = sizeof(uint32) * 0x2eb2;
  Soc_petra_a_regs.fdr.error_filter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.error_filter_reg.error_filter), sizeof(uint32) * 0x2eb2, sizeof(uint32) * 0x0000, 127, 0);

  /* Error Filter Mask */
  Soc_petra_a_regs.fdr.error_filter_mask_reg.addr.base = sizeof(uint32) * 0x2eb6;
  Soc_petra_a_regs.fdr.error_filter_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.error_filter_mask_reg.error_filter_mask), sizeof(uint32) * 0x2eb6, sizeof(uint32) * 0x0000, 127, 0);

  /* Error Filter Mask En */
  Soc_petra_a_regs.fdr.error_filter_mask_en_reg.addr.base = sizeof(uint32) * 0x2eba;
  Soc_petra_a_regs.fdr.error_filter_mask_en_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.error_filter_mask_en_reg.error_filter_mask_en), sizeof(uint32) * 0x2eba, sizeof(uint32) * 0x0000, 0, 0);

  /* Error Filter Cnt */
  Soc_petra_a_regs.fdr.error_filter_cnt_reg.addr.base = sizeof(uint32) * 0x2ebb;
  Soc_petra_a_regs.fdr.error_filter_cnt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.error_filter_cnt_reg.error_filter_cnt), sizeof(uint32) * 0x2ebb, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdr.error_filter_cnt_reg.error_filter_cnt_o), sizeof(uint32) * 0x2ebb, sizeof(uint32) * 0x0000, 31, 31);
}

/* Block registers initialization: FDT */
STATIC void
  soc_pa_regs_init_FDT(void)
{
  uint32
    reg_idx = 0;

  Soc_petra_a_regs.fdt.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_FDT;
  Soc_petra_a_regs.fdt.addr.base = sizeof(uint32) * 0x2c00;
  Soc_petra_a_regs.fdt.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.fdt.interrupt_reg.addr.base = sizeof(uint32) * 0x2c00;
  Soc_petra_a_regs.fdt.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.interrupt_reg.unrch_dest_evt), sizeof(uint32) * 0x2c00, sizeof(uint32) * 0x0000, 0, 0);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.fdt.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x2c10;
  Soc_petra_a_regs.fdt.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.interrupt_mask_reg.unrch_dest_evt_int_mask), sizeof(uint32) * 0x2c10, sizeof(uint32) * 0x0000, 0, 0);

  /* Indirect Command Rd Data0 */
  Soc_petra_a_regs.fdt.indirect_command_rd_data0_reg.addr.base = sizeof(uint32) * 0x2c30;
  Soc_petra_a_regs.fdt.indirect_command_rd_data0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.indirect_command_rd_data0_reg.indirect_command_rd_data0), sizeof(uint32) * 0x2c30, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data1 */
  Soc_petra_a_regs.fdt.indirect_command_rd_data1_reg.addr.base = sizeof(uint32) * 0x2c31;
  Soc_petra_a_regs.fdt.indirect_command_rd_data1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.indirect_command_rd_data1_reg.indirect_command_rd_data1), sizeof(uint32) * 0x2c31, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.fdt.indirect_command_reg.addr.base = sizeof(uint32) * 0x2c40;
  Soc_petra_a_regs.fdt.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x2c40, sizeof(uint32) * 0x0000, 0, 0);

  /* Indirect Command Address */
  Soc_petra_a_regs.fdt.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x2c41;
  Soc_petra_a_regs.fdt.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x2c41, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x2c41, sizeof(uint32) * 0x0000, 31, 31);

  /* Fdt Enabler Register */
  /* Solve the Fdt Enabler Register Errata causing bit-shift        */
  /* on read. When reading, bits 4 - 31 appear as 3 - 30.           */
  /* Bits 0 - 2 are read OK                                         */
  /* Note: it is wrong to access this register using                */
  /* soc_petra_write_fld, since it performs Read-Modify-Write.          */
  /* Instead, read the whole register register, use                 */
  /* SOC_PETRA_REGS_FDT_ENABLER_READ_ADJUST macro to adjust the value,  */
  /* modify and write                                               */
  Soc_petra_a_regs.fdt.fdt_enabler_reg.addr.base = sizeof(uint32) * 0x2c60;
  Soc_petra_a_regs.fdt.fdt_enabler_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.discard_dll_pkts), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.segment_pkt), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.del_crc_pkt), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.context_stat), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 6, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.use_coex_in_multicast), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.mask_tdm_bmp), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.dis_lclrt), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.rand_limk_en), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.fdt_enabler_reg.force_all_local), sizeof(uint32) * 0x2c60, sizeof(uint32) * 0x0000, 11, 11);

  /* Transmitted Data Cells Counter */
  Soc_petra_a_regs.fdt.transmitted_data_cells_counter_reg.addr.base = sizeof(uint32) * 0x2c61;
  Soc_petra_a_regs.fdt.transmitted_data_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.transmitted_data_cells_counter_reg.data_cell_cnt), sizeof(uint32) * 0x2c61, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.transmitted_data_cells_counter_reg.data_cell_cnt0), sizeof(uint32) * 0x2c61, sizeof(uint32) * 0x0000, 31, 31);

  /* Unreachable Destination Discarded Cells Counter */
  Soc_petra_a_regs.fdt.unreachable_destination_discarded_cells_counter_reg.addr.base = sizeof(uint32) * 0x2c62;
  Soc_petra_a_regs.fdt.unreachable_destination_discarded_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest), sizeof(uint32) * 0x2c62, sizeof(uint32) * 0x0000, 12, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest_cnt), sizeof(uint32) * 0x2c62, sizeof(uint32) * 0x0000, 28, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest_cnto), sizeof(uint32) * 0x2c62, sizeof(uint32) * 0x0000, 29, 29);

  /* Context Cell Counter */
  Soc_petra_a_regs.fdt.context_cell_counter_reg.addr.base = sizeof(uint32) * 0x2c63;
  Soc_petra_a_regs.fdt.context_cell_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.context_cell_counter_reg.context_cell_cnt), sizeof(uint32) * 0x2c63, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.context_cell_counter_reg.context_cell_cnto), sizeof(uint32) * 0x2c63, sizeof(uint32) * 0x0000, 31, 31);

  /* Transmit_Data_Cells_Registers */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS; ++reg_idx)
  {
    Soc_petra_a_regs.fdt.cpu_data_cell_reg[reg_idx].addr.base = sizeof(uint32) * (0x2c81 + reg_idx) ;
    Soc_petra_a_regs.fdt.cpu_data_cell_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    if(reg_idx == SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS - 1)
    {
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.cpu_data_cell_reg[reg_idx].cpudata_cell), sizeof(uint32) * (0x2c81 + reg_idx), sizeof(uint32) * 0x0000, 23, 0);
    }
    else
    {
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.cpu_data_cell_reg[reg_idx].cpudata_cell), sizeof(uint32) * (0x2c81 + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
    }
  }

  Soc_petra_a_regs.fdt.transmit_cell_output_link_number_reg.addr.base = sizeof(uint32) * 0x2c8b;
  Soc_petra_a_regs.fdt.transmit_cell_output_link_number_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.transmit_cell_output_link_number_reg.cpu_link_num), sizeof(uint32) * 0x2c8b, sizeof(uint32) * 0x0000, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.transmit_cell_output_link_number_reg.cpu_cell_size), sizeof(uint32) * 0x2c8b, sizeof(uint32) * 0x0000, 14, 8);

  /* Transmit Data Cell Trigger */
  Soc_petra_a_regs.fdt.transmit_data_cell_trigger_reg.addr.base = sizeof(uint32) * 0x2c8c;
  Soc_petra_a_regs.fdt.transmit_data_cell_trigger_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.transmit_data_cell_trigger_reg.cpu_trg), sizeof(uint32) * 0x2c8c, sizeof(uint32) * 0x0000, 0, 0);

  /* Coexist Dupx2 Unicast Bitmap */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_COEXIST_MAP_REGS; reg_idx++)
  {
    Soc_petra_a_regs.fdt.coexist_dupx2_unicast_bitmap_reg[reg_idx].addr.base = sizeof(uint32) * (0x2c90 + reg_idx);
    Soc_petra_a_regs.fdt.coexist_dupx2_unicast_bitmap_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.coexist_dupx2_unicast_bitmap_reg[reg_idx].coexist_dupx2_unicast_bitmap), sizeof(uint32) * (0x2c90 + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Tdm Link Mask */
  Soc_petra_a_regs.fdt.tdm_link_mask_reg[0].addr.base = sizeof(uint32) * 0x2ca9;
  Soc_petra_a_regs.fdt.tdm_link_mask_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.tdm_link_mask_reg[0].tdm_link_mask), sizeof(uint32) * 0x2ca9, sizeof(uint32) * 0x0000, 31, 0);

  /* Tdm Link Mask */
  Soc_petra_a_regs.fdt.tdm_link_mask_reg[1].addr.base = sizeof(uint32) * 0x2caa;
  Soc_petra_a_regs.fdt.tdm_link_mask_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.tdm_link_mask_reg[1].tdm_link_mask), sizeof(uint32) * 0x2caa, sizeof(uint32) * 0x0000, 3, 0);

 /* Crc Droped Pck Cnt */
  Soc_petra_a_regs.fdt.crc_droped_pck_cnt_reg.addr.base = sizeof(uint32) * 0x2cab;
  Soc_petra_a_regs.fdt.crc_droped_pck_cnt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.crc_droped_pck_cnt_reg.crc_droped_pck_cnt), sizeof(uint32) * 0x2cab, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.crc_droped_pck_cnt_reg.crc_droped_pck_cnt0), sizeof(uint32) * 0x2cab, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdm Cells Counter */
  Soc_petra_a_regs.fdt.tdm_cells_counter_reg.addr.base = sizeof(uint32) * 0x2cac;
  Soc_petra_a_regs.fdt.tdm_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.tdm_cells_counter_reg.tdm_data_cell_cnt), sizeof(uint32) * 0x2cac, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.tdm_cells_counter_reg.tdm_data_cell_cnt0), sizeof(uint32) * 0x2cac, sizeof(uint32) * 0x0000, 31, 31);

  /* Local Tdm Cells Counter */
  Soc_petra_a_regs.fdt.local_tdm_cells_counter_reg.addr.base = sizeof(uint32) * 0x2cad;
  Soc_petra_a_regs.fdt.local_tdm_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.local_tdm_cells_counter_reg.local_tdm_data_cell_cnt), sizeof(uint32) * 0x2cad, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fdt.local_tdm_cells_counter_reg.local_tdm_data_cell_cnt0), sizeof(uint32) * 0x2cad, sizeof(uint32) * 0x0000, 31, 31);
}

/* Block registers initialization: FCR */
STATIC void
  soc_pa_regs_init_FCR(void)
{
  uint32
    reg_idx = 0;

  Soc_petra_a_regs.fcr.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_FCR;
  Soc_petra_a_regs.fcr.addr.base = sizeof(uint32) * 0x3200;
  Soc_petra_a_regs.fcr.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.fcr.interrupt_reg.addr.base = sizeof(uint32) * 0x3200;
  Soc_petra_a_regs.fcr.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.src_dv_cng_link_ev), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.cpucnt_cell_fne), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.local_rout_fs_ovf), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.local_routrc_ovf), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.reach_fifo_ovf), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.flow_fifo_ovf), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_reg.credit_fifo_ovf), sizeof(uint32) * 0x3200, sizeof(uint32) * 0x0000, 6, 6);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.fcr.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x3210;
  Soc_petra_a_regs.fcr.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.src_dv_cng_link_evint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.cpucnt_cell_fneint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.local_routfs_ovfint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.local_routrc_ovfint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.reach_fifoovfint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.flow_fifoovfint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.interrupt_mask_reg.credit_fifoovfint_msk), sizeof(uint32) * 0x3210, sizeof(uint32) * 0x0000, 6, 6);

  /* Gtimer Configuration register  */
  Soc_petra_a_regs.fcr.gtimer_configuration_reg.addr.base = sizeof(uint32) * 0x3254;
  Soc_petra_a_regs.fcr.gtimer_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.gtimer_configuration_reg.time_count_config), sizeof(uint32) * 0x3254, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.gtimer_configuration_reg.counter_reset), sizeof(uint32) * 0x3254, sizeof(uint32) * 0x0000, 31, 31);

  /* Activate Gtimer */
  Soc_petra_a_regs.fcr.activate_gtimer_reg.addr.base = sizeof(uint32) * 0x3255;
  Soc_petra_a_regs.fcr.activate_gtimer_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.activate_gtimer_reg.activate_gtimer), sizeof(uint32) * 0x3255, sizeof(uint32) * 0x0000, 0, 0);

  /* FCREnablers And Filter Match Input Link */
  Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.addr.base = sizeof(uint32) * 0x3260;
  Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.dis_fl_sts), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.dis_crd_fcr), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.dis_sr), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.trap_all_cnt), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.mtch_act), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.fcrmtch_logic_not), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.pc_mtch_link), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 12, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.pc_mtch_link_on), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.fcrenablers_and_filter_match_input_link_reg.fcr_count_by_gtimer), sizeof(uint32) * 0x3260, sizeof(uint32) * 0x0000, 14, 14);

  /* Connectivity Map Change Event Registers0 */
  Soc_petra_a_regs.fcr.connectivity_map_change_event_registers0_reg.addr.base = sizeof(uint32) * 0x3261;
  Soc_petra_a_regs.fcr.connectivity_map_change_event_registers0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.connectivity_map_change_event_registers0_reg.src_dv_cng_link_l), sizeof(uint32) * 0x3261, sizeof(uint32) * 0x0000, 31, 0);

  /* Connectivity Map Change Event Registers1 */
  Soc_petra_a_regs.fcr.connectivity_map_change_event_registers1_reg.addr.base = sizeof(uint32) * 0x3262;
  Soc_petra_a_regs.fcr.connectivity_map_change_event_registers1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.connectivity_map_change_event_registers1_reg.src_dv_cng_link_h), sizeof(uint32) * 0x3262, sizeof(uint32) * 0x0000, 3, 0);

  /* Fap20b Destination Id Bitmap */
  for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_FAP20B_MAP_REGS; reg_idx++)
  {
    Soc_petra_a_regs.fcr.fap20b_destination_id_bitmap_reg[reg_idx].addr.base = sizeof(uint32) * (0x3263 + reg_idx);
    Soc_petra_a_regs.fcr.fap20b_destination_id_bitmap_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(&(Soc_petra_a_regs.fcr.fap20b_destination_id_bitmap_reg[reg_idx].fap20b_destination_id_bitmap),
      sizeof(uint32) * (0x3263 + reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* Connectivity Map Registers */
  for(reg_idx = 0; reg_idx < SOC_PETRA_FBR_NOF_LINKS; reg_idx++)
  {
    Soc_petra_a_regs.fcr.connectivity_map_registers_reg[reg_idx].addr.base = sizeof(uint32) * (0x3270 + reg_idx);
    Soc_petra_a_regs.fcr.connectivity_map_registers_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.fcr.connectivity_map_registers_reg[reg_idx].source_unit),
      sizeof(uint32) * (0x3270 + reg_idx),
      sizeof(uint32) * 0x0000,
      10,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.fcr.connectivity_map_registers_reg[reg_idx].source_device_level),
      sizeof(uint32) * (0x3270 + reg_idx),
      sizeof(uint32) * 0x0000,
      13,
      11
    );

    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.fcr.connectivity_map_registers_reg[reg_idx].source_device_link),
      sizeof(uint32) * (0x3270 + reg_idx),
      sizeof(uint32) * 0x0000,
      19,
      14
    );
  }

  /* Credit Cells Counter */
  Soc_petra_a_regs.fcr.credit_cells_counter_reg.addr.base = sizeof(uint32) * 0x3294;
  Soc_petra_a_regs.fcr.credit_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.credit_cells_counter_reg.credit_cells_counter), sizeof(uint32) * 0x3294, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.credit_cells_counter_reg.credit_cells_counter_ovf), sizeof(uint32) * 0x3294, sizeof(uint32) * 0x0000, 31, 31);

  /* Flow Status Cells Counter */
  Soc_petra_a_regs.fcr.flow_status_cells_counter_reg.addr.base = sizeof(uint32) * 0x3295;
  Soc_petra_a_regs.fcr.flow_status_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.flow_status_cells_counter_reg.fs_cells_counter), sizeof(uint32) * 0x3295, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.flow_status_cells_counter_reg.fs_cells_counter_ovf), sizeof(uint32) * 0x3295, sizeof(uint32) * 0x0000, 31, 31);

  /* Reachability Cells Counter */
  Soc_petra_a_regs.fcr.reachability_cells_counter_reg.addr.base = sizeof(uint32) * 0x3296;
  Soc_petra_a_regs.fcr.reachability_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.reachability_cells_counter_reg.reach_cells_counter), sizeof(uint32) * 0x3296, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.reachability_cells_counter_reg.reach_cells_counter_ovf), sizeof(uint32) * 0x3296, sizeof(uint32) * 0x0000, 31, 31);

  /* Total Cells Counter */
  Soc_petra_a_regs.fcr.total_cells_counter_reg.addr.base = sizeof(uint32) * 0x3298;
  Soc_petra_a_regs.fcr.total_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.total_cells_counter_reg.total_cells_counter), sizeof(uint32) * 0x3298, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.total_cells_counter_reg.total_cells_counter_ovf), sizeof(uint32) * 0x3298, sizeof(uint32) * 0x0000, 31, 31);

  /* Cell Drop Counter0 */
  Soc_petra_a_regs.fcr.cell_drop_counter0_reg.addr.base = sizeof(uint32) * 0x3299;
  Soc_petra_a_regs.fcr.cell_drop_counter0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.cell_drop_counter0_reg.credit_drop_count), sizeof(uint32) * 0x3299, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.cell_drop_counter0_reg.credit_drop_count_ovf), sizeof(uint32) * 0x3299, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.cell_drop_counter0_reg.fs_drop_count), sizeof(uint32) * 0x3299, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.cell_drop_counter0_reg.fs_drop_count_ovf), sizeof(uint32) * 0x3299, sizeof(uint32) * 0x0000, 31, 31);

  /* Cell Drop Counter1 */
  Soc_petra_a_regs.fcr.cell_drop_counter1_reg.addr.base = sizeof(uint32) * 0x329a;
  Soc_petra_a_regs.fcr.cell_drop_counter1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.cell_drop_counter1_reg.reach_drop_count), sizeof(uint32) * 0x329a, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.cell_drop_counter1_reg.reach_drop_count_ovf), sizeof(uint32) * 0x329a, sizeof(uint32) * 0x0000, 15, 15);

  /* Programmable Control Cell Counter0 */
  Soc_petra_a_regs.fcr.programmable_control_cell_counter0_reg.addr.base = sizeof(uint32) * 0x32a1;
  Soc_petra_a_regs.fcr.programmable_control_cell_counter0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_control_cell_counter0_reg.pcmtch_ctrl_lb0), sizeof(uint32) * 0x32a1, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Control Cell Counter1 */
  Soc_petra_a_regs.fcr.programmable_control_cell_counter1_reg.addr.base = sizeof(uint32) * 0x32a2;
  Soc_petra_a_regs.fcr.programmable_control_cell_counter1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_control_cell_counter1_reg.pcmtch_ctrl_lb1), sizeof(uint32) * 0x32a2, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Control Cell Counter2 */
  Soc_petra_a_regs.fcr.programmable_control_cell_counter2_reg.addr.base = sizeof(uint32) * 0x32a3;
  Soc_petra_a_regs.fcr.programmable_control_cell_counter2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_control_cell_counter2_reg.pcmtch_ctrl_lb2), sizeof(uint32) * 0x32a3, sizeof(uint32) * 0x0000, 5, 0);

  /* Programmable Control Cell Counter Mask0 */
  Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask0_reg.addr.base = sizeof(uint32) * 0x32a4;
  Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask0_reg.pcmtch_ctrl_msk0), sizeof(uint32) * 0x32a4, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Control Cell Counter Mask1 */
  Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask1_reg.addr.base = sizeof(uint32) * 0x32a5;
  Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask1_reg.pcmtch_ctrl_msk1), sizeof(uint32) * 0x32a5, sizeof(uint32) * 0x0000, 31, 0);

  /* Programmable Control Cell Counter Mask2 */
  Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask2_reg.addr.base = sizeof(uint32) * 0x32a6;
  Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_control_cell_counter_mask2_reg.pcmtch_ctrl_msk2), sizeof(uint32) * 0x32a6, sizeof(uint32) * 0x0000, 5, 0);

  /* Programmable Cells Counter */
  Soc_petra_a_regs.fcr.programmable_cells_counter_reg.addr.base = sizeof(uint32) * 0x32a7;
  Soc_petra_a_regs.fcr.programmable_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_cells_counter_reg.prg_ctrl_cell_cnt), sizeof(uint32) * 0x32a7, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.programmable_cells_counter_reg.prg_ctrl_cell_cnto), sizeof(uint32) * 0x32a7, sizeof(uint32) * 0x0000, 31, 31);

  /* Control Cell FIFOBuffer */
  Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_0.addr.base = sizeof(uint32) * 0x32a9;
  Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_0.control_cell_fifobuffer), sizeof(uint32) * 0x32a9, sizeof(uint32) * 0x0000, 31, 8);

  /* Control Cell FIFOBuffer */
  Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_1.addr.base = sizeof(uint32) * 0x32aa;
  Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_1.control_cell_fifobuffer), sizeof(uint32) * 0x32aa, sizeof(uint32) * 0x0000, 31, 0);

  /* Control Cell FIFOBuffer */
  Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_2.addr.base = sizeof(uint32) * 0x32ab;
  Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fcr.control_cell_fifobuffer_reg_2.control_cell_fifobuffer), sizeof(uint32) * 0x32ab, sizeof(uint32) * 0x0000, 13, 0);
}

/* Block registers initialization: FCT */
STATIC void
  soc_pa_regs_init_FCT(void)
{
  Soc_petra_a_regs.fct.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_FCT;
  Soc_petra_a_regs.fct.addr.base = sizeof(uint32) * 0x3000;
  Soc_petra_a_regs.fct.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.fct.interrupt_reg.addr.base = sizeof(uint32) * 0x3000;
  Soc_petra_a_regs.fct.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.interrupt_reg.unrch_dest_evt), sizeof(uint32) * 0x3000, sizeof(uint32) * 0x0000, 0, 0);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.fct.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x3010;
  Soc_petra_a_regs.fct.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.interrupt_mask_reg.unrch_dest_evt_int_mask), sizeof(uint32) * 0x3010, sizeof(uint32) * 0x0000, 0, 0);

  /* Fct Enabler Register  */
  Soc_petra_a_regs.fct.fct_enabler_reg.addr.base = sizeof(uint32) * 0x3060;
  Soc_petra_a_regs.fct.fct_enabler_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.fct_enabler_reg.dis_sts), sizeof(uint32) * 0x3060, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.fct_enabler_reg.dis_crd), sizeof(uint32) * 0x3060, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.fct_enabler_reg.dis_rch), sizeof(uint32) * 0x3060, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.fct_enabler_reg.dis_lclrt), sizeof(uint32) * 0x3060, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.fct_enabler_reg.force_all_local), sizeof(uint32) * 0x3060, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.fct_enabler_reg.stat_credit_and_rch_ctrl), sizeof(uint32) * 0x3060, sizeof(uint32) * 0x0000, 2, 0);

  /* Cpu Transmit Cell1 */
  Soc_petra_a_regs.fct.cpu_transmit_cell1_reg.addr.base = sizeof(uint32) * 0x3061;
  Soc_petra_a_regs.fct.cpu_transmit_cell1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.cpu_transmit_cell1_reg.cpu_cell_buff0), sizeof(uint32) * 0x3061, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Transmit Cell2 */
  Soc_petra_a_regs.fct.cpu_transmit_cell2_reg.addr.base = sizeof(uint32) * 0x3062;
  Soc_petra_a_regs.fct.cpu_transmit_cell2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.cpu_transmit_cell2_reg.cpu_cell_buff1), sizeof(uint32) * 0x3062, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Transmit Cell3 */
  Soc_petra_a_regs.fct.cpu_transmit_cell3_reg.addr.base = sizeof(uint32) * 0x3063;
  Soc_petra_a_regs.fct.cpu_transmit_cell3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.cpu_transmit_cell3_reg.cpu_cell_buff2), sizeof(uint32) * 0x3063, sizeof(uint32) * 0x0000, 31, 0);

  /* Cpu Transmit Cell Link Number */
  Soc_petra_a_regs.fct.cpu_transmit_cell_link_number_reg.addr.base = sizeof(uint32) * 0x3064;
  Soc_petra_a_regs.fct.cpu_transmit_cell_link_number_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.cpu_transmit_cell_link_number_reg.cpu_link_num), sizeof(uint32) * 0x3064, sizeof(uint32) * 0x0000, 4, 0);

  /* Cpu Transmit Cells Trigger  */
  Soc_petra_a_regs.fct.cpu_transmit_cells_trigger_reg.addr.base = sizeof(uint32) * 0x3066;
  Soc_petra_a_regs.fct.cpu_transmit_cells_trigger_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.cpu_transmit_cells_trigger_reg.cpu_trg), sizeof(uint32) * 0x3066, sizeof(uint32) * 0x0000, 0, 0);

  /* Transmitted Control Cells Counter */
  Soc_petra_a_regs.fct.transmitted_control_cells_counter_reg.addr.base = sizeof(uint32) * 0x3070;
  Soc_petra_a_regs.fct.transmitted_control_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.transmitted_control_cells_counter_reg.ctrl_cell_cnt), sizeof(uint32) * 0x3070, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.transmitted_control_cells_counter_reg.ctrl_cell_cnto), sizeof(uint32) * 0x3070, sizeof(uint32) * 0x0000, 31, 31);

  /* Unreachable Destination  */
  Soc_petra_a_regs.fct.unreachable_destination_reg.addr.base = sizeof(uint32) * 0x3071;
  Soc_petra_a_regs.fct.unreachable_destination_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.unreachable_destination_reg.unrch_dest), sizeof(uint32) * 0x3071, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.unreachable_destination_reg.unrch_dest_evt), sizeof(uint32) * 0x3071, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.unreachable_destination_reg.unrch_crdt_cnt), sizeof(uint32) * 0x3071, sizeof(uint32) * 0x0000, 30, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.unreachable_destination_reg.unrch_crdt_cnto), sizeof(uint32) * 0x3071, sizeof(uint32) * 0x0000, 31, 31);

  /* Local Route Cells Counter  */
  Soc_petra_a_regs.fct.local_route_cells_counter_reg.addr.base = sizeof(uint32) * 0x3072;
  Soc_petra_a_regs.fct.local_route_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.local_route_cells_counter_reg.local_rt_cellcnt), sizeof(uint32) * 0x3072, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fct.local_route_cells_counter_reg.local_rt_cellcnto), sizeof(uint32) * 0x3072, sizeof(uint32) * 0x0000, 31, 31);
}

/* Block registers initialization: MESH TOPOLOGY */
STATIC void
  soc_pa_regs_init_MESH_TOPOLOGY(void)
{
  Soc_petra_a_regs.mesh_topology.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_MESH_TOPOLOGY;
  Soc_petra_a_regs.mesh_topology.addr.base = sizeof(uint32) * 0x3400;
  Soc_petra_a_regs.mesh_topology.addr.step = sizeof(uint32) * 0x0000;

  /* Mesh Topology */
  Soc_petra_a_regs.mesh_topology.mesh_topology_reg.addr.base = sizeof(uint32) * 0x3400;
  Soc_petra_a_regs.mesh_topology.mesh_topology_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.stan_aln), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.in_system), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 8, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.fld0), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 16, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.fld1), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 20, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.reserved), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 23, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.multi_fap), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.stan_aln2), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_topology_reg.trig), sizeof(uint32) * 0x3400, sizeof(uint32) * 0x0000, 31, 31);

  /* mesh_config_0_reg */
  Soc_petra_a_regs.mesh_topology.mesh_config_0_reg.addr.base = sizeof(uint32) * 0x3402;
  Soc_petra_a_regs.mesh_topology.mesh_config_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_config_0_reg.conf1), sizeof(uint32) * 0x3402, sizeof(uint32) * 0x0000, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_config_0_reg.conf2), sizeof(uint32) * 0x3402, sizeof(uint32) * 0x0000, 31, 24);

  /* Init */
  Soc_petra_a_regs.mesh_topology.init_reg.addr.base = sizeof(uint32) * 0x3403;
  Soc_petra_a_regs.mesh_topology.init_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.init_reg.config1), sizeof(uint32) * 0x3403, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.init_reg.config2), sizeof(uint32) * 0x3403, sizeof(uint32) * 0x0000, 7, 4);

  /* mesh_config_1_reg */
  Soc_petra_a_regs.mesh_topology.mesh_config_1_reg.addr.base = sizeof(uint32) * 0x3404;
  Soc_petra_a_regs.mesh_topology.mesh_config_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_config_1_reg.mesh_config_1_reg), sizeof(uint32) * 0x3404, sizeof(uint32) * 0x0000, 31, 0);

  /* mesh_config_2_reg */
  Soc_petra_a_regs.mesh_topology.mesh_config_2_reg.addr.base = sizeof(uint32) * 0x3405;
  Soc_petra_a_regs.mesh_topology.mesh_config_2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_config_2_reg.mesh_config_2_reg), sizeof(uint32) * 0x3405, sizeof(uint32) * 0x0000, 31, 0);


  /* Fap Detect Ctrl Cells Cnt */
  Soc_petra_a_regs.mesh_topology.fap_detect_ctrl_cells_cnt_reg.addr.base = sizeof(uint32) * 0x3440;
  Soc_petra_a_regs.mesh_topology.fap_detect_ctrl_cells_cnt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.fap_detect_ctrl_cells_cnt_reg.rcv_ctl1), sizeof(uint32) * 0x3440, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.fap_detect_ctrl_cells_cnt_reg.rcv_ctl2), sizeof(uint32) * 0x3440, sizeof(uint32) * 0x0000, 31, 16);

  /* mesh_status_reg */
  Soc_petra_a_regs.mesh_topology.mesh_status_reg.addr.base = sizeof(uint32) * 0x3441;
  Soc_petra_a_regs.mesh_topology.mesh_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_status_reg.status), sizeof(uint32) * 0x3441, sizeof(uint32) * 0x0000, 5, 5);

  /* mesh_status_cnt_reg */
  Soc_petra_a_regs.mesh_topology.mesh_status_cnt_reg.addr.base = sizeof(uint32) * 0x3445;
  Soc_petra_a_regs.mesh_topology.mesh_status_cnt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mesh_topology.mesh_status_cnt_reg.status), sizeof(uint32) * 0x3445, sizeof(uint32) * 0x0000, 14, 0);
}

/* Block registers initialization: RTP */
STATIC void
  soc_pa_regs_init_RTP(void)
{
  Soc_petra_a_regs.rtp.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_RTP;
  Soc_petra_a_regs.rtp.addr.base = sizeof(uint32) * 0x3600;
  Soc_petra_a_regs.rtp.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.rtp.interrupt_reg.addr.base = sizeof(uint32) * 0x3600;
  Soc_petra_a_regs.rtp.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.interrupt_reg.link_mask_change), sizeof(uint32) * 0x3600, sizeof(uint32) * 0x0000, 0, 0);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.rtp.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x3610;
  Soc_petra_a_regs.rtp.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.interrupt_mask_reg.link_mask_change), sizeof(uint32) * 0x3610, sizeof(uint32) * 0x0000, 1, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.rtp.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x3620;
  Soc_petra_a_regs.rtp.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x3620, sizeof(uint32) * 0x0000, 71, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.rtp.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x3630;
  Soc_petra_a_regs.rtp.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x3630, sizeof(uint32) * 0x0000, 71, 0);

  /* Indirect Command */
  Soc_petra_a_regs.rtp.indirect_command_reg.addr.base = sizeof(uint32) * 0x3640;
  Soc_petra_a_regs.rtp.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x3640, sizeof(uint32) * 0x0000, 0, 0);

  /* Indirect Command Address */
  Soc_petra_a_regs.rtp.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x3641;
  Soc_petra_a_regs.rtp.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x3641, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x3641, sizeof(uint32) * 0x0000, 31, 31);

  /* RTP Enable */
  Soc_petra_a_regs.rtp.rtp_enable_reg.addr.base = sizeof(uint32) * 0x3660;
  Soc_petra_a_regs.rtp.rtp_enable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_enable_reg.rtpwp), sizeof(uint32) * 0x3660, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_enable_reg.aclm), sizeof(uint32) * 0x3660, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_enable_reg.rtp_up_en), sizeof(uint32) * 0x3660, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_enable_reg.rtp_en_msk), sizeof(uint32) * 0x3660, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_enable_reg.rmgr), sizeof(uint32) * 0x3660, sizeof(uint32) * 0x0000, 12, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_enable_reg.en_local_link_reduction), sizeof(uint32) * 0x3660, sizeof(uint32) * 0x0000, 13, 13);

  /* RTP By Pass */
  Soc_petra_a_regs.rtp.rtp_by_pass_reg.addr.base = sizeof(uint32) * 0x3661;
  Soc_petra_a_regs.rtp.rtp_by_pass_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_by_pass_reg.frc_lnks_high), sizeof(uint32) * 0x3661, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_by_pass_reg.frc_lnk_num_high), sizeof(uint32) * 0x3661, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_by_pass_reg.frc_lnk_num), sizeof(uint32) * 0x3661, sizeof(uint32) * 0x0000, 7, 2);

  /* RTP Coexist configuration register */
  Soc_petra_a_regs.rtp.rtp_coexist_configuration_reg[0].addr.base = sizeof(uint32) * 0x3662;
  Soc_petra_a_regs.rtp.rtp_coexist_configuration_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_coexist_configuration_reg[0].rtpcoexist), sizeof(uint32) * 0x3662, sizeof(uint32) * 0x0000, 31, 0);

  Soc_petra_a_regs.rtp.rtp_coexist_configuration_reg[1].addr.base = sizeof(uint32) * 0x3663;
  Soc_petra_a_regs.rtp.rtp_coexist_configuration_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.rtp_coexist_configuration_reg[1].rtpcoexist), sizeof(uint32) * 0x3663, sizeof(uint32) * 0x0000, 3, 0);


  /* Multicast Link Up */
  Soc_petra_a_regs.rtp.multicast_link_up_reg[0].addr.base = sizeof(uint32) * 0x3664;
  Soc_petra_a_regs.rtp.multicast_link_up_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.multicast_link_up_reg[0].mclink_up), sizeof(uint32) * 0x3664, sizeof(uint32) * 0x0000, 31, 0);
  Soc_petra_a_regs.rtp.multicast_link_up_reg[1].addr.base = sizeof(uint32) * 0x3665;
  Soc_petra_a_regs.rtp.multicast_link_up_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.multicast_link_up_reg[1].mclink_up), sizeof(uint32) * 0x3665, sizeof(uint32) * 0x0000, 3, 0);

  /* Multicast Distribution Configuration register */
  Soc_petra_a_regs.rtp.multicast_distribution_configuration_reg.addr.base = sizeof(uint32) * 0x3666;
  Soc_petra_a_regs.rtp.multicast_distribution_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.multicast_distribution_configuration_reg.mul_num_trav), sizeof(uint32) * 0x3666, sizeof(uint32) * 0x0000, 1, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.multicast_distribution_configuration_reg.bypass_update), sizeof(uint32) * 0x3666, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.multicast_distribution_configuration_reg.enable_mclupdates), sizeof(uint32) * 0x3666, sizeof(uint32) * 0x0000, 3, 3);

  /* Maximum Base Index */
  Soc_petra_a_regs.rtp.maximum_base_index_reg.addr.base = sizeof(uint32) * 0x3667;
  Soc_petra_a_regs.rtp.maximum_base_index_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.maximum_base_index_reg.max_bi), sizeof(uint32) * 0x3667, sizeof(uint32) * 0x0000, 5, 0);

  /* Link Active Mask */
  Soc_petra_a_regs.rtp.link_active_mask_reg[0].addr.base = sizeof(uint32) * 0x3668;
  Soc_petra_a_regs.rtp.link_active_mask_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.link_active_mask_reg[0].lnk_actv_msk), sizeof(uint32) * 0x3668, sizeof(uint32) * 0x0000, 31, 0);

  Soc_petra_a_regs.rtp.link_active_mask_reg[1].addr.base = sizeof(uint32) * 0x3669;
  Soc_petra_a_regs.rtp.link_active_mask_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.link_active_mask_reg[1].lnk_actv_msk), sizeof(uint32) * 0x3669, sizeof(uint32) * 0x0000, 3, 0);


  /* ACL Received */
  Soc_petra_a_regs.rtp.acl_received_reg[0].addr.base = sizeof(uint32) * 0x3670;
  Soc_petra_a_regs.rtp.acl_received_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.acl_received_reg[0].aclrecieve), sizeof(uint32) * 0x3670, sizeof(uint32) * 0x0000, 31, 0);

  /* ACL Received */
  Soc_petra_a_regs.rtp.acl_received_reg[1].addr.base = sizeof(uint32) * 0x3671;
  Soc_petra_a_regs.rtp.acl_received_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.acl_received_reg[1].aclrecieve), sizeof(uint32) * 0x3671, sizeof(uint32) * 0x0000, 3, 0);

  /* Locally generated ACL */
  Soc_petra_a_regs.rtp.locally_generated_acl_reg[0].addr.base = sizeof(uint32) * 0x3672;
  Soc_petra_a_regs.rtp.locally_generated_acl_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.locally_generated_acl_reg[0].local_acl), sizeof(uint32) * 0x3672, sizeof(uint32) * 0x0000, 31, 0);

  /* Locally generated ACL */
  Soc_petra_a_regs.rtp.locally_generated_acl_reg[1].addr.base = sizeof(uint32) * 0x3673;
  Soc_petra_a_regs.rtp.locally_generated_acl_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.locally_generated_acl_reg[1].local_acl), sizeof(uint32) * 0x3673, sizeof(uint32) * 0x0000, 3, 0);

  /* MC Distribution map */
  Soc_petra_a_regs.rtp.mc_distribution_map_reg[0].addr.base = sizeof(uint32) * 0x3674;
  Soc_petra_a_regs.rtp.mc_distribution_map_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.mc_distribution_map_reg[0].mcdistribution), sizeof(uint32) * 0x3674, sizeof(uint32) * 0x0000, 31, 0);

  /* MC Distribution map */
  Soc_petra_a_regs.rtp.mc_distribution_map_reg[1].addr.base = sizeof(uint32) * 0x3674;
  Soc_petra_a_regs.rtp.mc_distribution_map_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.mc_distribution_map_reg[1].mcdistribution), sizeof(uint32) * 0x3675, sizeof(uint32) * 0x0000, 3, 0);

  /* Exclude Dest ID for MC Links 0 */
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_0_reg.addr.base = sizeof(uint32) * 0x3676;
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_0_reg.dest_id), sizeof(uint32) * 0x3676, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_0_reg.valid), sizeof(uint32) * 0x3676, sizeof(uint32) * 0x0000, 11, 11);

  /* Exclude Dest ID for MC Links 1 */
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_1_reg.addr.base = sizeof(uint32) * 0x3677;
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_1_reg.dest_id), sizeof(uint32) * 0x3677, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_1_reg.valid), sizeof(uint32) * 0x3677, sizeof(uint32) * 0x0000, 11, 11);

  /* Exclude Dest ID for MC Links 2 */
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_2_reg.addr.base = sizeof(uint32) * 0x3678;
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_2_reg.dest_id), sizeof(uint32) * 0x3678, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_2_reg.valid), sizeof(uint32) * 0x3678, sizeof(uint32) * 0x0000, 11, 11);

  /* Exclude Dest ID for MC Links 3 */
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_3_reg.addr.base = sizeof(uint32) * 0x3679;
  Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_3_reg.dest_id), sizeof(uint32) * 0x3679, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.exclude_dest_id_for_mc_links_3_reg.valid), sizeof(uint32) * 0x3679, sizeof(uint32) * 0x0000, 11, 11);

  /* Allowed Links */
  Soc_petra_a_regs.rtp.allowed_links_reg[0].addr.base = sizeof(uint32) * 0x3680;
  Soc_petra_a_regs.rtp.allowed_links_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.allowed_links_reg[0].allowed_links), sizeof(uint32) * 0x3680, sizeof(uint32) * 0x0000, 31, 0);

  Soc_petra_a_regs.rtp.allowed_links_reg[1].addr.base = sizeof(uint32) * 0x3681;
  Soc_petra_a_regs.rtp.allowed_links_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.rtp.allowed_links_reg[1].allowed_links), sizeof(uint32) * 0x3681, sizeof(uint32) * 0x0000, 3, 0);
}

/* Block registers initialization: FABRIC MAC */
STATIC void
  soc_pa_regs_init_FABRIC_MAC(void)
{

  uint32
    fld_idx = 0,
    reg_idx = 0;

  Soc_petra_a_regs.fabric_mac.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_FABRIC_MAC;
  Soc_petra_a_regs.fabric_mac.addr.base = sizeof(uint32) * 0x3c00;
  Soc_petra_a_regs.fabric_mac.addr.step = sizeof(uint32) * 0x0200;

  /* Per-SerDes link registers */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_PER_LINK_REGS; reg_idx ++)
  {
     /*Leaky Bucket */
    Soc_petra_a_regs.fabric_mac.leaky_bucket_reg[reg_idx].addr.base = sizeof(uint32) * (0x3c01 + (reg_idx*0x10));
    Soc_petra_a_regs.fabric_mac.leaky_bucket_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_reg[reg_idx].lky_bkt_value), sizeof(uint32) * (0x3c01 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 5, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_reg[reg_idx].rx_good_cell_cnt), sizeof(uint32) * (0x3c01 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 17, 6);

     /*Transmit Cell Counters */
    Soc_petra_a_regs.fabric_mac.transmit_cell_counters_reg[reg_idx].addr.base = sizeof(uint32) * (0x3c02 + (reg_idx*0x10));
    Soc_petra_a_regs.fabric_mac.transmit_cell_counters_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.transmit_cell_counters_reg[reg_idx].tx_cell_cnt_n), sizeof(uint32) * (0x3c02 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 11, 0);

     /*Loopback And Link Level Flow Control Enable Register */
    Soc_petra_a_regs.fabric_mac.loopback_and_link_level_flow_control_enable_reg[reg_idx].addr.base = sizeof(uint32) * (0x3c03 + (reg_idx*0x10));
    Soc_petra_a_regs.fabric_mac.loopback_and_link_level_flow_control_enable_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.loopback_and_link_level_flow_control_enable_reg[reg_idx].lcl_lpbk_on), sizeof(uint32) * (0x3c03 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 2, 2);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.loopback_and_link_level_flow_control_enable_reg[reg_idx].double_rate_en), sizeof(uint32) * (0x3c03 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 3, 3);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.loopback_and_link_level_flow_control_enable_reg[reg_idx].lnk_lvl_fc_tx_en), sizeof(uint32) * (0x3c03 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 4, 4);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.loopback_and_link_level_flow_control_enable_reg[reg_idx].lnk_lvl_fc_rx_en), sizeof(uint32) * (0x3c03 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 5, 5);

     /*CRCError Counter */
    Soc_petra_a_regs.fabric_mac.crcerror_counter_reg[reg_idx].addr.base = sizeof(uint32) * (0x3c05 + (reg_idx*0x10));
    Soc_petra_a_regs.fabric_mac.crcerror_counter_reg[reg_idx].addr.step = sizeof(uint32) * 0x0200;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.crcerror_counter_reg[reg_idx].crcerr_cnt), sizeof(uint32) * (0x3c05 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 30, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.crcerror_counter_reg[reg_idx].crcerr_cnt_ovf), sizeof(uint32) * (0x3c05 + (reg_idx*0x10)), sizeof(uint32) * 0x0200, 31, 31);
  }

  /* Interrupt Register5 */
  Soc_petra_a_regs.fabric_mac.interrupt_5_reg.addr.base = sizeof(uint32) * 0x3cea;
  Soc_petra_a_regs.fabric_mac.interrupt_5_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_5_reg.wrong_size_n_int), sizeof(uint32) * 0x3cea, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Mask Register5 */
  Soc_petra_a_regs.fabric_mac.interrupt_mask_5_reg.addr.base = sizeof(uint32) * 0x3ceb;
  Soc_petra_a_regs.fabric_mac.interrupt_mask_5_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_5_reg.wrong_size_n_int_mask), sizeof(uint32) * 0x3ceb, sizeof(uint32) * 0x0200, 23, 12);

  /* Enablers Register */
  Soc_petra_a_regs.fabric_mac.enablers_reg.addr.base = sizeof(uint32) * 0x3cf0;
  Soc_petra_a_regs.fabric_mac.enablers_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.enable_serial_link), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.del_crcerr_cell), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.del_tdmcrcerr_cel), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.del_data_cell_lb), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.del_fscr_cell_lb), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.del_allcells_lb), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.del_wrong_size_cell), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.tx_cnt_cfg), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.enablers_reg.rx_cnt_cfg), sizeof(uint32) * 0x3cf0, sizeof(uint32) * 0x0200, 19, 18);

  /* Leaky Bucket Control Register */
  Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.addr.base = sizeof(uint32) * 0x3cf1;
  Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.bkt_fill_rate), sizeof(uint32) * 0x3cf1, sizeof(uint32) * 0x0200, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.bkt_link_up_th), sizeof(uint32) * 0x3cf1, sizeof(uint32) * 0x0200, 9, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.bkt_link_dn_th), sizeof(uint32) * 0x3cf1, sizeof(uint32) * 0x0200, 15, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.sig_det_bkt_rst_ena), sizeof(uint32) * 0x3cf1, sizeof(uint32) * 0x0200, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.ser_lck_bkt_rst_ena), sizeof(uint32) * 0x3cf1, sizeof(uint32) * 0x0200, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.leaky_bucket_control_reg.align_lck_bkt_rst_ena), sizeof(uint32) * 0x3cf1, sizeof(uint32) * 0x0200, 18, 18);

  /* Control Cell Burst And TDMRegister */
  Soc_petra_a_regs.fabric_mac.control_cell_burst_and_tdmreg_reg.addr.base = sizeof(uint32) * 0x3cf2;
  Soc_petra_a_regs.fabric_mac.control_cell_burst_and_tdmreg_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.control_cell_burst_and_tdmreg_reg.cntrl_burst_period), sizeof(uint32) * 0x3cf2, sizeof(uint32) * 0x0200, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.control_cell_burst_and_tdmreg_reg.max_cntrl_cell_burst), sizeof(uint32) * 0x3cf2, sizeof(uint32) * 0x0200, 9, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.control_cell_burst_and_tdmreg_reg.tdmfragment_number), sizeof(uint32) * 0x3cf2, sizeof(uint32) * 0x0200, 24, 16);

  /* Receive Reset Register */
  Soc_petra_a_regs.fabric_mac.receive_reset_reg.addr.base = sizeof(uint32) * 0x3cf4;
  Soc_petra_a_regs.fabric_mac.receive_reset_reg.addr.step = sizeof(uint32) * 0x0200;

  for(fld_idx = 0; fld_idx < SOC_PETRA_NOF_PER_LINK_REGS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.fabric_mac.receive_reset_reg.fmacrx_rst_n[fld_idx]),
      sizeof(uint32) * 0x3cf4,
      sizeof(uint32) * 0x0200,
      (uint8)(0 + fld_idx),
      (uint8)(0 + fld_idx)
    );

    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.fabric_mac.receive_reset_reg.fmactx_rst_n[fld_idx]),
      sizeof(uint32) * 0x3cf4,
      sizeof(uint32) * 0x0200,
      (uint8)(16 + fld_idx),
      (uint8)(16 + fld_idx)
    );
  }

  /* Link Level Flow Control And Comma Configuration Register */
  Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.addr.base = sizeof(uint32) * 0x3cf6;
  Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.lnk_lvl_age_prd), sizeof(uint32) * 0x3cf6, sizeof(uint32) * 0x0200, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.lnk_lvl_frag_num), sizeof(uint32) * 0x3cf6, sizeof(uint32) * 0x0200, 12, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.cm_brst_size), sizeof(uint32) * 0x3cf6, sizeof(uint32) * 0x0200, 21, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.cm_tx_period), sizeof(uint32) * 0x3cf6, sizeof(uint32) * 0x0200, 28, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.link_level_flow_control_and_comma_configuration_reg.cm_tx_byte_mode), sizeof(uint32) * 0x3cf6, sizeof(uint32) * 0x0200, 31, 31);

  /* Interrupt Register1 */
  Soc_petra_a_regs.fabric_mac.interrupt_1_reg.addr.base = sizeof(uint32) * 0x3cf8;
  Soc_petra_a_regs.fabric_mac.interrupt_1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_1_reg.rx_crcerr_n_int), sizeof(uint32) * 0x3cf8, sizeof(uint32) * 0x0200, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_1_reg.rx_cgerr_n_int), sizeof(uint32) * 0x3cf8, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Mask Register1 */
  Soc_petra_a_regs.fabric_mac.interrupt_mask_1_reg.addr.base = sizeof(uint32) * 0x3cf9;
  Soc_petra_a_regs.fabric_mac.interrupt_mask_1_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_1_reg.rx_crcerr_n_int_mask), sizeof(uint32) * 0x3cf9, sizeof(uint32) * 0x0200, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_1_reg.rx_cgerr_n_int_mask), sizeof(uint32) * 0x3cf9, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Register2 */
  Soc_petra_a_regs.fabric_mac.interrupt_2_reg.addr.base = sizeof(uint32) * 0x3cfa;
  Soc_petra_a_regs.fabric_mac.interrupt_2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_2_reg.los_int), sizeof(uint32) * 0x3cfa, sizeof(uint32) * 0x0200, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_2_reg.rx_mis_aerr_n_int), sizeof(uint32) * 0x3cfa, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Mask Register2 */
  Soc_petra_a_regs.fabric_mac.interrupt_mask_2_reg.addr.base = sizeof(uint32) * 0x3cfb;
  Soc_petra_a_regs.fabric_mac.interrupt_mask_2_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_2_reg.los_int_mask), sizeof(uint32) * 0x3cfb, sizeof(uint32) * 0x0200, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_2_reg.rx_mis_aerr_n_int_mask), sizeof(uint32) * 0x3cfb, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Register3 */
  Soc_petra_a_regs.fabric_mac.interrupt_3_reg.addr.base = sizeof(uint32) * 0x3cfc;
  Soc_petra_a_regs.fabric_mac.interrupt_3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_3_reg.rx_disp_err_n_int), sizeof(uint32) * 0x3cfc, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Mask Register3 */
  Soc_petra_a_regs.fabric_mac.interrupt_mask_3_reg.addr.base = sizeof(uint32) * 0x3cfd;
  Soc_petra_a_regs.fabric_mac.interrupt_mask_3_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_3_reg.rx_disp_err_n_int_mask), sizeof(uint32) * 0x3cfd, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Register4 */
  Soc_petra_a_regs.fabric_mac.interrupt_4_reg.addr.base = sizeof(uint32) * 0x3cfe;
  Soc_petra_a_regs.fabric_mac.interrupt_4_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_4_reg.lnklvl_age_n_int), sizeof(uint32) * 0x3cfe, sizeof(uint32) * 0x0200, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_4_reg.lnklvl_halt_n_int), sizeof(uint32) * 0x3cfe, sizeof(uint32) * 0x0200, 23, 12);

  /* Interrupt Mask Register4 */
  Soc_petra_a_regs.fabric_mac.interrupt_mask_4_reg.addr.base = sizeof(uint32) * 0x3cff;
  Soc_petra_a_regs.fabric_mac.interrupt_mask_4_reg.addr.step = sizeof(uint32) * 0x0200;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_4_reg.lnklvl_age_n_int_mask), sizeof(uint32) * 0x3cff, sizeof(uint32) * 0x0200, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.fabric_mac.interrupt_mask_4_reg.lnklvl_halt_n_int_mask), sizeof(uint32) * 0x3cff, sizeof(uint32) * 0x0200, 23, 12);
}

/* Block registers initialization: MSW */
STATIC void
  soc_pa_regs_init_MSW(void)
{
  Soc_petra_a_regs.msw.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_MSW;
  Soc_petra_a_regs.msw.addr.base = sizeof(uint32) * 0x4800;
  Soc_petra_a_regs.msw.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.msw.interrupt_reg.addr.base = sizeof(uint32) * 0x4800;
  Soc_petra_a_regs.msw.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd0_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd1_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd2_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd3_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd_ipu_interrupt_group_a), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd_epb_interrupt_group_a), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd4_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd5_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd6_macro_interrupt), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd_ipu_interrupt_group_b), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_reg.srd_epb_interrupt_group_b), sizeof(uint32) * 0x4800, sizeof(uint32) * 0x0000, 28, 28);

  /* Srd Lane Interrupt Register */
  Soc_petra_a_regs.msw.srd_lane_interrupt_reg.addr.base = sizeof(uint32) * 0x4801;
  Soc_petra_a_regs.msw.srd_lane_interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd_lane_interrupt_reg.srd_lane_interrupt), sizeof(uint32) * 0x4801, sizeof(uint32) * 0x0000, 27, 0);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.msw.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x4810;
  Soc_petra_a_regs.msw.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd0_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd1_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd2_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd3_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd_ipu_interrupt_mask_group_a), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd_epb_interrupt_group_amask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd4_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd5_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd6_macro_interrupt_mask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd_ipu_interrupt_mask_group_b), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.interrupt_mask_reg.srd_epb_interrupt_group_bmask), sizeof(uint32) * 0x4810, sizeof(uint32) * 0x0000, 28, 28);

  /* Srd Lane Interrupt Mask Register */
  Soc_petra_a_regs.msw.srd_lane_interrupt_mask_reg.addr.base = sizeof(uint32) * 0x4811;
  Soc_petra_a_regs.msw.srd_lane_interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd_lane_interrupt_mask_reg.srd_lane_interrupt_mask), sizeof(uint32) * 0x4811, sizeof(uint32) * 0x0000, 27, 0);

  /* Scif Control */
  Soc_petra_a_regs.msw.scif_control_reg.addr.base = sizeof(uint32) * 0x4860;
  Soc_petra_a_regs.msw.scif_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_control_reg.scif_en), sizeof(uint32) * 0x4860, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_control_reg.scif_asel_en), sizeof(uint32) * 0x4860, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_control_reg.scif_asel_reset), sizeof(uint32) * 0x4860, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_control_reg.scif_star_sel), sizeof(uint32) * 0x4860, sizeof(uint32) * 0x0000, 6, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_control_reg.scif_macro_sel), sizeof(uint32) * 0x4860, sizeof(uint32) * 0x0000, 11, 8);

  /* Scif Status */
  Soc_petra_a_regs.msw.scif_status_reg.addr.base = sizeof(uint32) * 0x4861;
  Soc_petra_a_regs.msw.scif_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_status_reg.scif_macro_sel_status), sizeof(uint32) * 0x4861, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_status_reg.scif_star_sel_status), sizeof(uint32) * 0x4861, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_status_reg.scif_asel_status), sizeof(uint32) * 0x4861, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.scif_status_reg.scif_ext_en_status), sizeof(uint32) * 0x4861, sizeof(uint32) * 0x0000, 12, 12);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x4870,
    0x0000,
    &(Soc_petra_a_regs.msw.pcmi_config_reg),
    &(Soc_petra_a_regs.msw.pcmi_results_reg)
  );

  /* Srd0 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd0_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x4900;
  Soc_petra_a_regs.msw.srd0_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln0_cfga_reg.srd0_ln0_cfga), sizeof(uint32) * 0x4900, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln0 Stat */
  Soc_petra_a_regs.msw.srd0_ln0_stat_reg.addr.base = sizeof(uint32) * 0x4901;
  Soc_petra_a_regs.msw.srd0_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln0_stat_reg.srd0_ln0_stat), sizeof(uint32) * 0x4901, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd0_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x4902;
  Soc_petra_a_regs.msw.srd0_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln0_ebist_reg.srd0_ln0_ebist), sizeof(uint32) * 0x4902, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd0_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x4904;
  Soc_petra_a_regs.msw.srd0_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln1_cfga_reg.srd0_ln1_cfga), sizeof(uint32) * 0x4904, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln1 Stat */
  Soc_petra_a_regs.msw.srd0_ln1_stat_reg.addr.base = sizeof(uint32) * 0x4905;
  Soc_petra_a_regs.msw.srd0_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln1_stat_reg.srd0_ln1_stat), sizeof(uint32) * 0x4905, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd0_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x4906;
  Soc_petra_a_regs.msw.srd0_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln1_ebist_reg.srd0_ln1_ebist), sizeof(uint32) * 0x4906, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd0_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x4908;
  Soc_petra_a_regs.msw.srd0_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln2_cfga_reg.srd0_ln2_cfga), sizeof(uint32) * 0x4908, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln2 Stat */
  Soc_petra_a_regs.msw.srd0_ln2_stat_reg.addr.base = sizeof(uint32) * 0x4909;
  Soc_petra_a_regs.msw.srd0_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln2_stat_reg.srd0_ln2_stat), sizeof(uint32) * 0x4909, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd0_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x490a;
  Soc_petra_a_regs.msw.srd0_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln2_ebist_reg.srd0_ln2_ebist), sizeof(uint32) * 0x490a, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd0_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x490c;
  Soc_petra_a_regs.msw.srd0_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln3_cfga_reg.srd0_ln3_cfga), sizeof(uint32) * 0x490c, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln3 Stat */
  Soc_petra_a_regs.msw.srd0_ln3_stat_reg.addr.base = sizeof(uint32) * 0x490d;
  Soc_petra_a_regs.msw.srd0_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln3_stat_reg.srd0_ln3_stat), sizeof(uint32) * 0x490d, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd0_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x490e;
  Soc_petra_a_regs.msw.srd0_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_ln3_ebist_reg.srd0_ln3_ebist), sizeof(uint32) * 0x490e, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Cmu Cfga */
  Soc_petra_a_regs.msw.srd0_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x4910;
  Soc_petra_a_regs.msw.srd0_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_cmu_cfga_reg.srd0_cmu_cfga), sizeof(uint32) * 0x4910, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd0_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x4911;
  Soc_petra_a_regs.msw.srd0_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_cmu_cfgb_reg.srd0_cmu_cfgb), sizeof(uint32) * 0x4911, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd0 Cmu Stat */
  Soc_petra_a_regs.msw.srd0_cmu_stat_reg.addr.base = sizeof(uint32) * 0x4912;
  Soc_petra_a_regs.msw.srd0_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd0_cmu_stat_reg.srd0_cmu_stat), sizeof(uint32) * 0x4912, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd1_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x4920;
  Soc_petra_a_regs.msw.srd1_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln0_cfga_reg.srd1_ln0_cfga), sizeof(uint32) * 0x4920, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln0 Stat */
  Soc_petra_a_regs.msw.srd1_ln0_stat_reg.addr.base = sizeof(uint32) * 0x4921;
  Soc_petra_a_regs.msw.srd1_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln0_stat_reg.srd1_ln0_stat), sizeof(uint32) * 0x4921, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd1_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x4922;
  Soc_petra_a_regs.msw.srd1_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln0_ebist_reg.srd1_ln0_ebist), sizeof(uint32) * 0x4922, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd1_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x4924;
  Soc_petra_a_regs.msw.srd1_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln1_cfga_reg.srd1_ln1_cfga), sizeof(uint32) * 0x4924, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln1 Stat */
  Soc_petra_a_regs.msw.srd1_ln1_stat_reg.addr.base = sizeof(uint32) * 0x4925;
  Soc_petra_a_regs.msw.srd1_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln1_stat_reg.srd1_ln1_stat), sizeof(uint32) * 0x4925, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd1_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x4926;
  Soc_petra_a_regs.msw.srd1_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln1_ebist_reg.srd1_ln1_ebist), sizeof(uint32) * 0x4926, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd1_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x4928;
  Soc_petra_a_regs.msw.srd1_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln2_cfga_reg.srd1_ln2_cfga), sizeof(uint32) * 0x4928, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln2 Stat */
  Soc_petra_a_regs.msw.srd1_ln2_stat_reg.addr.base = sizeof(uint32) * 0x4929;
  Soc_petra_a_regs.msw.srd1_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln2_stat_reg.srd1_ln2_stat), sizeof(uint32) * 0x4929, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd1_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x492a;
  Soc_petra_a_regs.msw.srd1_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln2_ebist_reg.srd1_ln2_ebist), sizeof(uint32) * 0x492a, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd1_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x492c;
  Soc_petra_a_regs.msw.srd1_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln3_cfga_reg.srd1_ln3_cfga), sizeof(uint32) * 0x492c, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln3 Stat */
  Soc_petra_a_regs.msw.srd1_ln3_stat_reg.addr.base = sizeof(uint32) * 0x492d;
  Soc_petra_a_regs.msw.srd1_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln3_stat_reg.srd1_ln3_stat), sizeof(uint32) * 0x492d, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd1_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x492e;
  Soc_petra_a_regs.msw.srd1_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_ln3_ebist_reg.srd1_ln3_ebist), sizeof(uint32) * 0x492e, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Cmu Cfga */
  Soc_petra_a_regs.msw.srd1_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x4930;
  Soc_petra_a_regs.msw.srd1_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_cmu_cfga_reg.srd1_cmu_cfga), sizeof(uint32) * 0x4930, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd1_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x4931;
  Soc_petra_a_regs.msw.srd1_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_cmu_cfgb_reg.srd1_cmu_cfgb), sizeof(uint32) * 0x4931, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd1 Cmu Stat */
  Soc_petra_a_regs.msw.srd1_cmu_stat_reg.addr.base = sizeof(uint32) * 0x4932;
  Soc_petra_a_regs.msw.srd1_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd1_cmu_stat_reg.srd1_cmu_stat), sizeof(uint32) * 0x4932, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd2_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x4940;
  Soc_petra_a_regs.msw.srd2_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln0_cfga_reg.srd2_ln0_cfga), sizeof(uint32) * 0x4940, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln0 Stat */
  Soc_petra_a_regs.msw.srd2_ln0_stat_reg.addr.base = sizeof(uint32) * 0x4941;
  Soc_petra_a_regs.msw.srd2_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln0_stat_reg.srd2_ln0_stat), sizeof(uint32) * 0x4941, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd2_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x4942;
  Soc_petra_a_regs.msw.srd2_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln0_ebist_reg.srd2_ln0_ebist), sizeof(uint32) * 0x4942, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd2_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x4944;
  Soc_petra_a_regs.msw.srd2_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln1_cfga_reg.srd2_ln1_cfga), sizeof(uint32) * 0x4944, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln1 Stat */
  Soc_petra_a_regs.msw.srd2_ln1_stat_reg.addr.base = sizeof(uint32) * 0x4945;
  Soc_petra_a_regs.msw.srd2_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln1_stat_reg.srd2_ln1_stat), sizeof(uint32) * 0x4945, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd2_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x4946;
  Soc_petra_a_regs.msw.srd2_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln1_ebist_reg.srd2_ln1_ebist), sizeof(uint32) * 0x4946, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd2_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x4948;
  Soc_petra_a_regs.msw.srd2_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln2_cfga_reg.srd2_ln2_cfga), sizeof(uint32) * 0x4948, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln2 Stat */
  Soc_petra_a_regs.msw.srd2_ln2_stat_reg.addr.base = sizeof(uint32) * 0x4949;
  Soc_petra_a_regs.msw.srd2_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln2_stat_reg.srd2_ln2_stat), sizeof(uint32) * 0x4949, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd2_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x494a;
  Soc_petra_a_regs.msw.srd2_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln2_ebist_reg.srd2_ln2_ebist), sizeof(uint32) * 0x494a, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd2_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x494c;
  Soc_petra_a_regs.msw.srd2_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln3_cfga_reg.srd2_ln3_cfga), sizeof(uint32) * 0x494c, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln3 Stat */
  Soc_petra_a_regs.msw.srd2_ln3_stat_reg.addr.base = sizeof(uint32) * 0x494d;
  Soc_petra_a_regs.msw.srd2_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln3_stat_reg.srd2_ln3_stat), sizeof(uint32) * 0x494d, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd2_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x494e;
  Soc_petra_a_regs.msw.srd2_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_ln3_ebist_reg.srd2_ln3_ebist), sizeof(uint32) * 0x494e, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Cmu Cfga */
  Soc_petra_a_regs.msw.srd2_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x4950;
  Soc_petra_a_regs.msw.srd2_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_cmu_cfga_reg.srd2_cmu_cfga), sizeof(uint32) * 0x4950, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd2_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x4951;
  Soc_petra_a_regs.msw.srd2_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_cmu_cfgb_reg.srd2_cmu_cfgb), sizeof(uint32) * 0x4951, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd2 Cmu Stat */
  Soc_petra_a_regs.msw.srd2_cmu_stat_reg.addr.base = sizeof(uint32) * 0x4952;
  Soc_petra_a_regs.msw.srd2_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd2_cmu_stat_reg.srd2_cmu_stat), sizeof(uint32) * 0x4952, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd3_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x4960;
  Soc_petra_a_regs.msw.srd3_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln0_cfga_reg.srd3_ln0_cfga), sizeof(uint32) * 0x4960, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln0 Stat */
  Soc_petra_a_regs.msw.srd3_ln0_stat_reg.addr.base = sizeof(uint32) * 0x4961;
  Soc_petra_a_regs.msw.srd3_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln0_stat_reg.srd3_ln0_stat), sizeof(uint32) * 0x4961, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd3_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x4962;
  Soc_petra_a_regs.msw.srd3_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln0_ebist_reg.srd3_ln0_ebist), sizeof(uint32) * 0x4962, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd3_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x4964;
  Soc_petra_a_regs.msw.srd3_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln1_cfga_reg.srd3_ln1_cfga), sizeof(uint32) * 0x4964, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln1 Stat */
  Soc_petra_a_regs.msw.srd3_ln1_stat_reg.addr.base = sizeof(uint32) * 0x4965;
  Soc_petra_a_regs.msw.srd3_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln1_stat_reg.srd3_ln1_stat), sizeof(uint32) * 0x4965, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd3_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x4966;
  Soc_petra_a_regs.msw.srd3_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln1_ebist_reg.srd3_ln1_ebist), sizeof(uint32) * 0x4966, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd3_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x4968;
  Soc_petra_a_regs.msw.srd3_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln2_cfga_reg.srd3_ln2_cfga), sizeof(uint32) * 0x4968, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln2 Stat */
  Soc_petra_a_regs.msw.srd3_ln2_stat_reg.addr.base = sizeof(uint32) * 0x4969;
  Soc_petra_a_regs.msw.srd3_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln2_stat_reg.srd3_ln2_stat), sizeof(uint32) * 0x4969, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd3_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x496a;
  Soc_petra_a_regs.msw.srd3_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln2_ebist_reg.srd3_ln2_ebist), sizeof(uint32) * 0x496a, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd3_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x496c;
  Soc_petra_a_regs.msw.srd3_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln3_cfga_reg.srd3_ln3_cfga), sizeof(uint32) * 0x496c, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln3 Stat */
  Soc_petra_a_regs.msw.srd3_ln3_stat_reg.addr.base = sizeof(uint32) * 0x496d;
  Soc_petra_a_regs.msw.srd3_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln3_stat_reg.srd3_ln3_stat), sizeof(uint32) * 0x496d, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd3_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x496e;
  Soc_petra_a_regs.msw.srd3_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_ln3_ebist_reg.srd3_ln3_ebist), sizeof(uint32) * 0x496e, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Cmu Cfga */
  Soc_petra_a_regs.msw.srd3_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x4970;
  Soc_petra_a_regs.msw.srd3_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_cmu_cfga_reg.srd3_cmu_cfga), sizeof(uint32) * 0x4970, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd3_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x4971;
  Soc_petra_a_regs.msw.srd3_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_cmu_cfgb_reg.srd3_cmu_cfgb), sizeof(uint32) * 0x4971, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd3 Cmu Stat */
  Soc_petra_a_regs.msw.srd3_cmu_stat_reg.addr.base = sizeof(uint32) * 0x4972;
  Soc_petra_a_regs.msw.srd3_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd3_cmu_stat_reg.srd3_cmu_stat), sizeof(uint32) * 0x4972, sizeof(uint32) * 0x0000, 31, 0);

  /* Srda Ipu Cfg */
  Soc_petra_a_regs.msw.srda_ipu_cfg_reg.addr.base = sizeof(uint32) * 0x4978;
  Soc_petra_a_regs.msw.srda_ipu_cfg_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srda_ipu_cfg_reg.srda_ipu_cfg), sizeof(uint32) * 0x4978, sizeof(uint32) * 0x0000, 31, 0);

  /* Srda Epb Op */
  Soc_petra_a_regs.msw.srda_epb_op_reg.addr.base = sizeof(uint32) * 0x4979;
  Soc_petra_a_regs.msw.srda_epb_op_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srda_epb_op_reg.srda_epb_op), sizeof(uint32) * 0x4979, sizeof(uint32) * 0x0000, 31, 0);

  /* Srda Epb Rd */
  Soc_petra_a_regs.msw.srda_epb_rd_reg.addr.base = sizeof(uint32) * 0x497a;
  Soc_petra_a_regs.msw.srda_epb_rd_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srda_epb_rd_reg.srda_epb_rd), sizeof(uint32) * 0x497a, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd4_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x4980;
  Soc_petra_a_regs.msw.srd4_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln0_cfga_reg.srd4_ln0_cfga), sizeof(uint32) * 0x4980, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln0 Stat */
  Soc_petra_a_regs.msw.srd4_ln0_stat_reg.addr.base = sizeof(uint32) * 0x4981;
  Soc_petra_a_regs.msw.srd4_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln0_stat_reg.srd4_ln0_stat), sizeof(uint32) * 0x4981, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd4_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x4982;
  Soc_petra_a_regs.msw.srd4_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln0_ebist_reg.srd4_ln0_ebist), sizeof(uint32) * 0x4982, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd4_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x4984;
  Soc_petra_a_regs.msw.srd4_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln1_cfga_reg.srd4_ln1_cfga), sizeof(uint32) * 0x4984, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln1 Stat */
  Soc_petra_a_regs.msw.srd4_ln1_stat_reg.addr.base = sizeof(uint32) * 0x4985;
  Soc_petra_a_regs.msw.srd4_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln1_stat_reg.srd4_ln1_stat), sizeof(uint32) * 0x4985, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd4_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x4986;
  Soc_petra_a_regs.msw.srd4_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln1_ebist_reg.srd4_ln1_ebist), sizeof(uint32) * 0x4986, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd4_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x4988;
  Soc_petra_a_regs.msw.srd4_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln2_cfga_reg.srd4_ln2_cfga), sizeof(uint32) * 0x4988, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln2 Stat */
  Soc_petra_a_regs.msw.srd4_ln2_stat_reg.addr.base = sizeof(uint32) * 0x4989;
  Soc_petra_a_regs.msw.srd4_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln2_stat_reg.srd4_ln2_stat), sizeof(uint32) * 0x4989, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd4_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x498a;
  Soc_petra_a_regs.msw.srd4_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln2_ebist_reg.srd4_ln2_ebist), sizeof(uint32) * 0x498a, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd4_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x498c;
  Soc_petra_a_regs.msw.srd4_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln3_cfga_reg.srd4_ln3_cfga), sizeof(uint32) * 0x498c, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln3 Stat */
  Soc_petra_a_regs.msw.srd4_ln3_stat_reg.addr.base = sizeof(uint32) * 0x498d;
  Soc_petra_a_regs.msw.srd4_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln3_stat_reg.srd4_ln3_stat), sizeof(uint32) * 0x498d, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd4_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x498e;
  Soc_petra_a_regs.msw.srd4_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_ln3_ebist_reg.srd4_ln3_ebist), sizeof(uint32) * 0x498e, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Cmu Cfga */
  Soc_petra_a_regs.msw.srd4_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x4990;
  Soc_petra_a_regs.msw.srd4_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_cmu_cfga_reg.srd4_cmu_cfga), sizeof(uint32) * 0x4990, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd4_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x4991;
  Soc_petra_a_regs.msw.srd4_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_cmu_cfgb_reg.srd4_cmu_cfgb), sizeof(uint32) * 0x4991, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd4 Cmu Stat */
  Soc_petra_a_regs.msw.srd4_cmu_stat_reg.addr.base = sizeof(uint32) * 0x4992;
  Soc_petra_a_regs.msw.srd4_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd4_cmu_stat_reg.srd4_cmu_stat), sizeof(uint32) * 0x4992, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd5_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x49a0;
  Soc_petra_a_regs.msw.srd5_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln0_cfga_reg.srd5_ln0_cfga), sizeof(uint32) * 0x49a0, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln0 Stat */
  Soc_petra_a_regs.msw.srd5_ln0_stat_reg.addr.base = sizeof(uint32) * 0x49a1;
  Soc_petra_a_regs.msw.srd5_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln0_stat_reg.srd5_ln0_stat), sizeof(uint32) * 0x49a1, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd5_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x49a2;
  Soc_petra_a_regs.msw.srd5_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln0_ebist_reg.srd5_ln0_ebist), sizeof(uint32) * 0x49a2, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd5_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x49a4;
  Soc_petra_a_regs.msw.srd5_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln1_cfga_reg.srd5_ln1_cfga), sizeof(uint32) * 0x49a4, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln1 Stat */
  Soc_petra_a_regs.msw.srd5_ln1_stat_reg.addr.base = sizeof(uint32) * 0x49a5;
  Soc_petra_a_regs.msw.srd5_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln1_stat_reg.srd5_ln1_stat), sizeof(uint32) * 0x49a5, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd5_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x49a6;
  Soc_petra_a_regs.msw.srd5_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln1_ebist_reg.srd5_ln1_ebist), sizeof(uint32) * 0x49a6, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd5_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x49a8;
  Soc_petra_a_regs.msw.srd5_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln2_cfga_reg.srd5_ln2_cfga), sizeof(uint32) * 0x49a8, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln2 Stat */
  Soc_petra_a_regs.msw.srd5_ln2_stat_reg.addr.base = sizeof(uint32) * 0x49a9;
  Soc_petra_a_regs.msw.srd5_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln2_stat_reg.srd5_ln2_stat), sizeof(uint32) * 0x49a9, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd5_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x49aa;
  Soc_petra_a_regs.msw.srd5_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln2_ebist_reg.srd5_ln2_ebist), sizeof(uint32) * 0x49aa, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd5_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x49ac;
  Soc_petra_a_regs.msw.srd5_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln3_cfga_reg.srd5_ln3_cfga), sizeof(uint32) * 0x49ac, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln3 Stat */
  Soc_petra_a_regs.msw.srd5_ln3_stat_reg.addr.base = sizeof(uint32) * 0x49ad;
  Soc_petra_a_regs.msw.srd5_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln3_stat_reg.srd5_ln3_stat), sizeof(uint32) * 0x49ad, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd5_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x49ae;
  Soc_petra_a_regs.msw.srd5_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_ln3_ebist_reg.srd5_ln3_ebist), sizeof(uint32) * 0x49ae, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Cmu Cfga */
  Soc_petra_a_regs.msw.srd5_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x49b0;
  Soc_petra_a_regs.msw.srd5_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_cmu_cfga_reg.srd5_cmu_cfga), sizeof(uint32) * 0x49b0, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd5_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x49b1;
  Soc_petra_a_regs.msw.srd5_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_cmu_cfgb_reg.srd5_cmu_cfgb), sizeof(uint32) * 0x49b1, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd5 Cmu Stat */
  Soc_petra_a_regs.msw.srd5_cmu_stat_reg.addr.base = sizeof(uint32) * 0x49b2;
  Soc_petra_a_regs.msw.srd5_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd5_cmu_stat_reg.srd5_cmu_stat), sizeof(uint32) * 0x49b2, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln0 Cfga */
  Soc_petra_a_regs.msw.srd6_ln0_cfga_reg.addr.base = sizeof(uint32) * 0x49c0;
  Soc_petra_a_regs.msw.srd6_ln0_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln0_cfga_reg.srd6_ln0_cfga), sizeof(uint32) * 0x49c0, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln0 Stat */
  Soc_petra_a_regs.msw.srd6_ln0_stat_reg.addr.base = sizeof(uint32) * 0x49c1;
  Soc_petra_a_regs.msw.srd6_ln0_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln0_stat_reg.srd6_ln0_stat), sizeof(uint32) * 0x49c1, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln0 Ebist */
  Soc_petra_a_regs.msw.srd6_ln0_ebist_reg.addr.base = sizeof(uint32) * 0x49c2;
  Soc_petra_a_regs.msw.srd6_ln0_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln0_ebist_reg.srd6_ln0_ebist), sizeof(uint32) * 0x49c2, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln1 Cfga */
  Soc_petra_a_regs.msw.srd6_ln1_cfga_reg.addr.base = sizeof(uint32) * 0x49c4;
  Soc_petra_a_regs.msw.srd6_ln1_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln1_cfga_reg.srd6_ln1_cfga), sizeof(uint32) * 0x49c4, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln1 Stat */
  Soc_petra_a_regs.msw.srd6_ln1_stat_reg.addr.base = sizeof(uint32) * 0x49c5;
  Soc_petra_a_regs.msw.srd6_ln1_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln1_stat_reg.srd6_ln1_stat), sizeof(uint32) * 0x49c5, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln1 Ebist */
  Soc_petra_a_regs.msw.srd6_ln1_ebist_reg.addr.base = sizeof(uint32) * 0x49c6;
  Soc_petra_a_regs.msw.srd6_ln1_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln1_ebist_reg.srd6_ln1_ebist), sizeof(uint32) * 0x49c6, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln2 Cfga */
  Soc_petra_a_regs.msw.srd6_ln2_cfga_reg.addr.base = sizeof(uint32) * 0x49c8;
  Soc_petra_a_regs.msw.srd6_ln2_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln2_cfga_reg.srd6_ln2_cfga), sizeof(uint32) * 0x49c8, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln2 Stat */
  Soc_petra_a_regs.msw.srd6_ln2_stat_reg.addr.base = sizeof(uint32) * 0x49c9;
  Soc_petra_a_regs.msw.srd6_ln2_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln2_stat_reg.srd6_ln2_stat), sizeof(uint32) * 0x49c9, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln2 Ebist */
  Soc_petra_a_regs.msw.srd6_ln2_ebist_reg.addr.base = sizeof(uint32) * 0x49ca;
  Soc_petra_a_regs.msw.srd6_ln2_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln2_ebist_reg.srd6_ln2_ebist), sizeof(uint32) * 0x49ca, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln3 Cfga */
  Soc_petra_a_regs.msw.srd6_ln3_cfga_reg.addr.base = sizeof(uint32) * 0x49cc;
  Soc_petra_a_regs.msw.srd6_ln3_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln3_cfga_reg.srd6_ln3_cfga), sizeof(uint32) * 0x49cc, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln3 Stat */
  Soc_petra_a_regs.msw.srd6_ln3_stat_reg.addr.base = sizeof(uint32) * 0x49cd;
  Soc_petra_a_regs.msw.srd6_ln3_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln3_stat_reg.srd6_ln3_stat), sizeof(uint32) * 0x49cd, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Ln3 Ebist */
  Soc_petra_a_regs.msw.srd6_ln3_ebist_reg.addr.base = sizeof(uint32) * 0x49ce;
  Soc_petra_a_regs.msw.srd6_ln3_ebist_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_ln3_ebist_reg.srd6_ln3_ebist), sizeof(uint32) * 0x49ce, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Cmu Cfga */
  Soc_petra_a_regs.msw.srd6_cmu_cfga_reg.addr.base = sizeof(uint32) * 0x49d0;
  Soc_petra_a_regs.msw.srd6_cmu_cfga_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_cmu_cfga_reg.srd6_cmu_cfga), sizeof(uint32) * 0x49d0, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Cmu Cfgb */
  Soc_petra_a_regs.msw.srd6_cmu_cfgb_reg.addr.base = sizeof(uint32) * 0x49d1;
  Soc_petra_a_regs.msw.srd6_cmu_cfgb_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_cmu_cfgb_reg.srd6_cmu_cfgb), sizeof(uint32) * 0x49d1, sizeof(uint32) * 0x0000, 31, 0);

  /* Srd6 Cmu Stat */
  Soc_petra_a_regs.msw.srd6_cmu_stat_reg.addr.base = sizeof(uint32) * 0x49d2;
  Soc_petra_a_regs.msw.srd6_cmu_stat_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srd6_cmu_stat_reg.srd6_cmu_stat), sizeof(uint32) * 0x49d2, sizeof(uint32) * 0x0000, 31, 0);

  /* Srdb Ipu Cfg */
  Soc_petra_a_regs.msw.srdb_ipu_cfg_reg.addr.base = sizeof(uint32) * 0x49f8;
  Soc_petra_a_regs.msw.srdb_ipu_cfg_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srdb_ipu_cfg_reg.srdb_ipu_cfg), sizeof(uint32) * 0x49f8, sizeof(uint32) * 0x0000, 31, 0);

  /* Srdb Epb Op */
  Soc_petra_a_regs.msw.srdb_epb_op_reg.addr.base = sizeof(uint32) * 0x49f9;
  Soc_petra_a_regs.msw.srdb_epb_op_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srdb_epb_op_reg.srdb_epb_op), sizeof(uint32) * 0x49f9, sizeof(uint32) * 0x0000, 31, 0);

  /* Srdb Epb Rd */
  Soc_petra_a_regs.msw.srdb_epb_rd_reg.addr.base = sizeof(uint32) * 0x49fa;
  Soc_petra_a_regs.msw.srdb_epb_rd_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.msw.srdb_epb_rd_reg.srdb_epb_rd), sizeof(uint32) * 0x49fa, sizeof(uint32) * 0x0000, 31, 0);
}

/* Block registers initialization: EGQ */
STATIC void
  soc_pa_regs_init_EGQ(void)
{
  uint32
    reg_idx,
    fld_idx,
    q_prio_idx,
    inner_reg_idx;

  Soc_petra_a_regs.egq.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_EGQ;
  Soc_petra_a_regs.egq.addr.base = sizeof(uint32) * 0x3800;
  Soc_petra_a_regs.egq.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.egq.interrupt_reg.addr.base = sizeof(uint32) * 0x3800;
  Soc_petra_a_regs.egq.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.cpu_pack32_bytes_err), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.cpu_data_arrived_err), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.pdm_par_err), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.plm_par_err), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.packet_aged), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.int_vlantable_oor), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.int_dif_af), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.cfc_fc_int), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.nifa_fc_int), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.nifb_fc_int), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.pkt_reas_int_vec), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.cnt_ovf_int_vec), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_reg.ecc_err_vec), sizeof(uint32) * 0x3800, sizeof(uint32) * 0x0000, 31, 31);

  /* Pkt Reas Int Reg */
  Soc_petra_a_regs.egq.pkt_reas_int_reg.addr.base = sizeof(uint32) * 0x3801;
  Soc_petra_a_regs.egq.pkt_reas_int_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_pkt_size_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_missing_sop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_frag_num_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_pkt_crc_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_sop_intr_mop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_fix129_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.vsc_eop_size_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_sonts_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_pkt_size_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_missing_sop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_frag_num_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_sop_intr_mop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_eop_size_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.fsc_sequence_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.csr_pkt_size_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.csr_unexpected_eop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.csr_missing_eop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.csr_sop_and_eop_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.csr_size_parity_err), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.rej_buff_sch), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.rej_buff_usc), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.rej_desc_sch), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg.rej_desc_usc), sizeof(uint32) * 0x3801, sizeof(uint32) * 0x0000, 22, 22);

  /* Cnt Int Reg */
  Soc_petra_a_regs.egq.cnt_int_reg.addr.base = sizeof(uint32) * 0x3802;
  Soc_petra_a_regs.egq.cnt_int_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cnt_int_reg.prp_sop_cnt_ovf_int), sizeof(uint32) * 0x3802, sizeof(uint32) * 0x0000, 0, 0);

  /* Ecc Int Reg */
  Soc_petra_a_regs.egq.ecc_int_reg.addr.base = sizeof(uint32) * 0x3803;
  Soc_petra_a_regs.egq.ecc_int_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.buflink_ecc_err), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.buflink_ecc_fix), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.rcnt_ecc_err), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.rcnt_ecc_fix), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.rrdm_ecc_err), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.rrdm_ecc_fix), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.rpdm_ecc_err), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg.rpdm_ecc_fix), sizeof(uint32) * 0x3803, sizeof(uint32) * 0x0000, 7, 7);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.egq.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x3810;
  Soc_petra_a_regs.egq.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_cpu_pack32_bytes_err), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_cpu_data_arrived_err), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_pdm_par_err), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_plm_par_err), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_packet_aged), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.masknt_vlantable_oor), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_int_dif_af), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_int_ucfifo_af), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_pqp_lbp_th), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_cfc_fc_int), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_nifa_fc_int), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_nifb_fc_int), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_pkt_reas_int_vec), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_cnt_ovf_int_vec), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.interrupt_mask_reg.mask_ecc_err_vec), sizeof(uint32) * 0x3810, sizeof(uint32) * 0x0000, 31, 31);

  /* Pkt Reas Int Reg Mask */
  Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.addr.base = sizeof(uint32) * 0x3811;
  Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_pkt_size_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_missing_sop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_frag_num_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_pkt_crc_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_sop_intr_mop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_fix129_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.vsc_eop_size_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_sonts_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_pkt_size_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_missing_sop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_frag_num_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_sop_intr_mop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_eop_size_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.fsc_sequence_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.csr_pkt_size_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.csr_unexpected_eop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.csr_missing_eop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.csr_sop_and_eop_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.csr_size_parity_err_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.rej_buff_sch_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.rej_buff_usc_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.rej_desc_sch_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.pkt_reas_int_reg_mask_reg.rej_desc_usc_mask), sizeof(uint32) * 0x3811, sizeof(uint32) * 0x0000, 22, 22);

  /* Cnt Int Reg Mask */
  Soc_petra_a_regs.egq.cnt_int_reg_mask_reg.addr.base = sizeof(uint32) * 0x3812;
  Soc_petra_a_regs.egq.cnt_int_reg_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cnt_int_reg_mask_reg.cnt_int_reg_mask), sizeof(uint32) * 0x3812, sizeof(uint32) * 0x0000, 0, 0);

  /* Ecc Int Reg Mask */
  Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.addr.base = sizeof(uint32) * 0x3813;
  Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.buflink_ecc_err_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.buflink_ecc_fix_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.rcnt_ecc_err_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.rcnt_ecc_fix_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.rrdm_ecc_err_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.rrdm_ecc_fix_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.rpdm_ecc_err_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_int_reg_mask_reg.rpdm_ecc_fix_mask), sizeof(uint32) * 0x3813, sizeof(uint32) * 0x0000, 7, 7);
  

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x3820;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x3820, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x3821;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x3821, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x3822;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x3822, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x3823;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x3823, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x3824;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x3824, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x3825;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x3825, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_6.addr.base = sizeof(uint32) * 0x3826;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_6.indirect_command_wr_data), sizeof(uint32) * 0x3826, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_7.addr.base = sizeof(uint32) * 0x3827;
  Soc_petra_a_regs.egq.indirect_command_wr_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_wr_data_reg_7.indirect_command_wr_data), sizeof(uint32) * 0x3827, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x3830;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x3830, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x3831;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x3831, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x3832;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x3832, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x3833;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x3833, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x3834;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x3834, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x3835;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x3835, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_6.addr.base = sizeof(uint32) * 0x3836;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_6.indirect_command_rd_data), sizeof(uint32) * 0x3836, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_7.addr.base = sizeof(uint32) * 0x3837;
  Soc_petra_a_regs.egq.indirect_command_rd_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_rd_data_reg_7.indirect_command_rd_data), sizeof(uint32) * 0x3837, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.egq.indirect_command_reg.addr.base = sizeof(uint32) * 0x3840;
  Soc_petra_a_regs.egq.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x3840, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x3840, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x3840, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x3840, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x3840, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.egq.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x3841;
  Soc_petra_a_regs.egq.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x3841, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x3841, sizeof(uint32) * 0x0000, 31, 31);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x3860,
    0x0000,
    &(Soc_petra_a_regs.egq.pcmi_config_reg),
    &(Soc_petra_a_regs.egq.pcmi_results_reg)
  );
  soc_petra_regs_block_pcmi_registers_init(
    0x3862,
    0x0000,
    &(Soc_petra_a_regs.egq.pcmi_config_reg1),
    &(Soc_petra_a_regs.egq.pcmi_results_reg1)
  );

  /* Cpu Packet Control */
  Soc_petra_a_regs.egq.cpu_packet_control_reg.addr.base = sizeof(uint32) * 0x3865;
  Soc_petra_a_regs.egq.cpu_packet_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_packet_control_reg.send_pkt_frag), sizeof(uint32) * 0x3865, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_packet_control_reg.start_of_packet), sizeof(uint32) * 0x3865, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_packet_control_reg.end_of_packet), sizeof(uint32) * 0x3865, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_packet_control_reg.pkt_frag_err), sizeof(uint32) * 0x3865, sizeof(uint32) * 0x0000, 3, 3);

  /* Cpu Packet Fragmentation Words */
  for (reg_idx = 0; reg_idx < SOC_PETRA_CPU_PACKET_FRAGMENTATION_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.cpu_packet_fragmentation_words_reg[reg_idx].addr.base = sizeof(uint32) * (0x3866 + reg_idx);
    Soc_petra_a_regs.egq.cpu_packet_fragmentation_words_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_packet_fragmentation_words_reg[reg_idx].packet_word), sizeof(uint32) * (0x3866 + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* EGQBlock Init Status */
  Soc_petra_a_regs.egq.egqblock_init_status_reg.addr.base = sizeof(uint32) * 0x3870;
  Soc_petra_a_regs.egq.egqblock_init_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egqblock_init_status_reg.egqblock_init), sizeof(uint32) * 0x3870, sizeof(uint32) * 0x0000, 0, 0);

  /* Ecc Corecction Disable */
  Soc_petra_a_regs.egq.ecc_corecction_disable_reg.addr.base = sizeof(uint32) * 0x3871;
  Soc_petra_a_regs.egq.ecc_corecction_disable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_corecction_disable_reg.buflink_dis_ecc), sizeof(uint32) * 0x3871, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_corecction_disable_reg.rcnt_dis_ecc), sizeof(uint32) * 0x3871, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_corecction_disable_reg.rrdm_dis_ecc), sizeof(uint32) * 0x3871, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.ecc_corecction_disable_reg.rpdm_dis_ecc), sizeof(uint32) * 0x3871, sizeof(uint32) * 0x0000, 3, 3);

  /* Tdm Configuration */
  Soc_petra_a_regs.egq.tdm_configuration_reg.addr.base = sizeof(uint32) * 0x3873;
  Soc_petra_a_regs.egq.tdm_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.tdm_configuration_reg.tdm_nifa_en), sizeof(uint32) * 0x3873, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.tdm_configuration_reg.tdm_nifb_en), sizeof(uint32) * 0x3873, sizeof(uint32) * 0x0000, 1, 1);

  /* Maximum Fragment Number */
  Soc_petra_a_regs.egq.maximum_fragment_number_reg.addr.base = sizeof(uint32) * 0x3874;
  Soc_petra_a_regs.egq.maximum_fragment_number_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.maximum_fragment_number_reg.max_frg_num), sizeof(uint32) * 0x3874, sizeof(uint32) * 0x0000, 8, 0);

  /* Fabric Minimum Packet Size */
  Soc_petra_a_regs.egq.fabric_minimum_packet_size_reg.addr.base = sizeof(uint32) * 0x3875;
  Soc_petra_a_regs.egq.fabric_minimum_packet_size_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.fabric_minimum_packet_size_reg.fabric_min_pkt_size), sizeof(uint32) * 0x3875, sizeof(uint32) * 0x0000, 9, 0);

  /* Egq Local And Fabric Arbiter */
  Soc_petra_a_regs.egq.egq_local_and_fabric_arbiter_reg.addr.base = sizeof(uint32) * 0x3876;
  Soc_petra_a_regs.egq.egq_local_and_fabric_arbiter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egq_local_and_fabric_arbiter_reg.local_vs_fabric_arb), sizeof(uint32) * 0x3876, sizeof(uint32) * 0x0000, 2, 0);

  /* Reassembly Reject Threshold Enable Configuration */
  Soc_petra_a_regs.egq.reassembly_reject_threshold_enable_configuration_reg.addr.base = sizeof(uint32) * 0x3877;
  Soc_petra_a_regs.egq.reassembly_reject_threshold_enable_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.reassembly_reject_threshold_enable_configuration_reg.rej_schd_desc_th_en), sizeof(uint32) * 0x3877, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.reassembly_reject_threshold_enable_configuration_reg.rej_un_schd_desc_th_en), sizeof(uint32) * 0x3877, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.reassembly_reject_threshold_enable_configuration_reg.rej_schd_buff_th_en), sizeof(uint32) * 0x3877, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.reassembly_reject_threshold_enable_configuration_reg.rej_un_schd_buff_th_en), sizeof(uint32) * 0x3877, sizeof(uint32) * 0x0000, 3, 3);

  /* Schededuled Reject Buffer Threshold */
  Soc_petra_a_regs.egq.schededuled_reject_buffer_threshold_reg.addr.base = sizeof(uint32) * 0x3878;
  Soc_petra_a_regs.egq.schededuled_reject_buffer_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.schededuled_reject_buffer_threshold_reg.clear_reas_rej_schd_buff_th), sizeof(uint32) * 0x3878, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.schededuled_reject_buffer_threshold_reg.set_reas_rej_schd_buff_th), sizeof(uint32) * 0x3878, sizeof(uint32) * 0x0000, 24, 16);

  /* Scheduled Reject Descriptor Threshold */
  Soc_petra_a_regs.egq.scheduled_reject_descriptor_threshold_reg.addr.base = sizeof(uint32) * 0x3879;
  Soc_petra_a_regs.egq.scheduled_reject_descriptor_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_reject_descriptor_threshold_reg.clear_reas_rej_schd_desc_th), sizeof(uint32) * 0x3879, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_reject_descriptor_threshold_reg.set_reas_rej_schd_desc_th), sizeof(uint32) * 0x3879, sizeof(uint32) * 0x0000, 24, 16);

  /* Unscheduled Reject Buffer Threshold */
  Soc_petra_a_regs.egq.unscheduled_reject_buffer_threshold_reg.addr.base = sizeof(uint32) * 0x387a;
  Soc_petra_a_regs.egq.unscheduled_reject_buffer_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unscheduled_reject_buffer_threshold_reg.clear_reas_rej_un_schd_buff_th), sizeof(uint32) * 0x387a, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unscheduled_reject_buffer_threshold_reg.set_reas_rej_un_schd_buff_th), sizeof(uint32) * 0x387a, sizeof(uint32) * 0x0000, 24, 16);

  /* Unscheduled Reject Descriptor Threshold */
  Soc_petra_a_regs.egq.unscheduled_reject_descriptor_threshold_reg.addr.base = sizeof(uint32) * 0x387b;
  Soc_petra_a_regs.egq.unscheduled_reject_descriptor_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unscheduled_reject_descriptor_threshold_reg.clear_reas_rej_un_schd_desc_th), sizeof(uint32) * 0x387b, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unscheduled_reject_descriptor_threshold_reg.set_reas_rej_un_schd_desc_th), sizeof(uint32) * 0x387b, sizeof(uint32) * 0x0000, 24, 16);

  /* Multicast Packet Header Shim Layer Translation */
  Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.addr.base = sizeof(uint32) * 0x387d;
  Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.mc_out_lif_msb), sizeof(uint32) * 0x387d, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.mc_epph_present), sizeof(uint32) * 0x387d, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.mc_ing_qsig), sizeof(uint32) * 0x387d, sizeof(uint32) * 0x0000, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.mc_dp), sizeof(uint32) * 0x387d, sizeof(uint32) * 0x0000, 7, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_packet_header_shim_layer_translation_reg.mc_out_mirror_disable), sizeof(uint32) * 0x387d, sizeof(uint32) * 0x0000, 8, 8);

  /* Shim Layer Translation Multicast Class Mapping Table */
  Soc_petra_a_regs.egq.shim_layer_translation_multicast_class_mapping_table_reg.addr.base = sizeof(uint32) * 0x387e;
  Soc_petra_a_regs.egq.shim_layer_translation_multicast_class_mapping_table_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.shim_layer_translation_multicast_class_mapping_table_reg.mc_class_map4to8), sizeof(uint32) * 0x387e, sizeof(uint32) * 0x0000, 11, 0);

  /* Unicast Packet Header Shim Layer Translation */
  Soc_petra_a_regs.egq.unicast_packet_header_shim_layer_translation_reg.addr.base = sizeof(uint32) * 0x387f;
  Soc_petra_a_regs.egq.unicast_packet_header_shim_layer_translation_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_packet_header_shim_layer_translation_reg.uc_epph_present), sizeof(uint32) * 0x387f, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_packet_header_shim_layer_translation_reg.uc_ing_qsig), sizeof(uint32) * 0x387f, sizeof(uint32) * 0x0000, 2, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_packet_header_shim_layer_translation_reg.uc_dp), sizeof(uint32) * 0x387f, sizeof(uint32) * 0x0000, 4, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_packet_header_shim_layer_translation_reg.uc_out_mirror_disable), sizeof(uint32) * 0x387f, sizeof(uint32) * 0x0000, 5, 5);

  /* Available Free Resources */
  Soc_petra_a_regs.egq.available_free_resources_reg.addr.base = sizeof(uint32) * 0x3881;
  Soc_petra_a_regs.egq.available_free_resources_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.available_free_resources_reg.available_free_descriptors), sizeof(uint32) * 0x3881, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.available_free_resources_reg.available_free_buffers), sizeof(uint32) * 0x3881, sizeof(uint32) * 0x0000, 28, 16);

  /* Fabric Route Sop Counter */
  Soc_petra_a_regs.egq.fabric_route_sop_counter.addr.base = sizeof(uint32) * 0x3882;
  Soc_petra_a_regs.egq.fabric_route_sop_counter.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.fabric_route_sop_counter.fsr_sop_cnt), sizeof(uint32) * 0x3882, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.fabric_route_sop_counter.fsr_sop_cnt_ovf), sizeof(uint32) * 0x3882, sizeof(uint32) * 0x0000, 31, 31);

  /* Local Route Sop Counter */
  Soc_petra_a_regs.egq.local_route_sop_counter.addr.base = sizeof(uint32) * 0x3883;
  Soc_petra_a_regs.egq.local_route_sop_counter.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.local_route_sop_counter.lsr_sop_cnt), sizeof(uint32) * 0x3883, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.local_route_sop_counter.lsr_sop_cnt_ovf), sizeof(uint32) * 0x3883, sizeof(uint32) * 0x0000, 31, 31);

  /* CPU interface Sop Counter */
  Soc_petra_a_regs.egq.cpu_interface_sop_counter.addr.base = sizeof(uint32) * 0x3884;
  Soc_petra_a_regs.egq.cpu_interface_sop_counter.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_interface_sop_counter.csr_sop_cnt), sizeof(uint32) * 0x3884, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.cpu_interface_sop_counter.csr_sop_cnt_ovf), sizeof(uint32) * 0x3884, sizeof(uint32) * 0x0000, 31, 31);

  /* Packet reassembly output Sop Counter */
  Soc_petra_a_regs.egq.packet_reassembly_sop_counter_0.addr.base = sizeof(uint32) * 0x3885;
  Soc_petra_a_regs.egq.packet_reassembly_sop_counter_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.packet_reassembly_sop_counter_0.prp_sop_cnt), sizeof(uint32) * 0x3885, sizeof(uint32) * 0x0000, 31, 0);

  /* Packet reassembly output Sop Counter */
  Soc_petra_a_regs.egq.packet_reassembly_sop_counter_1.addr.base = sizeof(uint32) * 0x3886;
  Soc_petra_a_regs.egq.packet_reassembly_sop_counter_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.packet_reassembly_sop_counter_1.prp_sop_cnt), sizeof(uint32) * 0x3886, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.packet_reassembly_sop_counter_1.prp_sop_cnt_ovf), sizeof(uint32) * 0x3886, sizeof(uint32) * 0x0000, 31, 31);

  /* Egress De Queue Scheme */
  Soc_petra_a_regs.egq.egress_de_queue_scheme_reg.addr.base = sizeof(uint32) * 0x3890;
  Soc_petra_a_regs.egq.egress_de_queue_scheme_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_de_queue_scheme_reg.schd_queue_scheme), sizeof(uint32) * 0x3890, sizeof(uint32) * 0x0000, 0, 0);

  /* Oc768 Unicast Multicast Weight */
  Soc_petra_a_regs.egq.oc768_unicast_multicast_weight_reg.addr.base = sizeof(uint32) * 0x3892;
  Soc_petra_a_regs.egq.oc768_unicast_multicast_weight_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.oc768_unicast_multicast_weight_reg.oc768_unicast_weight), sizeof(uint32) * 0x3892, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.oc768_unicast_multicast_weight_reg.oc768_multicast_weight), sizeof(uint32) * 0x3892, sizeof(uint32) * 0x0000, 15, 8);

  /* Unscheduled Resources Per-Queue-Priority Thresholds For Drop Precedence [0 - 3] */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
    {
      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_0[q_prio_idx][reg_idx].addr.base =
        sizeof(uint32) * (0x3893 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS));

      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_0[q_prio_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_0[q_prio_idx][reg_idx].total_usc_wrds),
        sizeof(uint32) * (0x3893 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        8,
        0
      );
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_0[q_prio_idx][reg_idx].total_usc_pkts),
        sizeof(uint32) * (0x3893 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        24,
        16
      );
    }
  }
  /* Unscheduled Resources Per-Queue-Priority Thresholds For Drop Precedence [0 - 3] */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
    {
      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_1[q_prio_idx][reg_idx].addr.base =
        sizeof(uint32) * (0x3894 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS));
      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_1[q_prio_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_1[q_prio_idx][reg_idx].avail_usc_desc),
        sizeof(uint32) * (0x3894 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        8,
        0
      );
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_1[q_prio_idx][reg_idx].avail_usc_bufs),
        sizeof(uint32) * (0x3894 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        24,
        16
      );
    }
  }

  /* Unscheduled Resources Per-Queue-Priority Thresholds For Drop Precedence [0 - 3] */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
    {
      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_2[q_prio_idx][reg_idx].addr.base =
        sizeof(uint32) * (0x3895  + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS));
      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_2[q_prio_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_2[q_prio_idx][reg_idx].uch_queue_wrds_type0),
        sizeof(uint32) * (0x3895  + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        8,
        0
      );
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_2[q_prio_idx][reg_idx].uch_queue_wrds_type1),
        sizeof(uint32) * (0x3895  + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        24,
        16
      );
    }
  }

  /* Unscheduled Resources Per-Queue-Priority Thresholds For Drop Precedence [0 - 3] */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; reg_idx++)
    {
      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_3[q_prio_idx][reg_idx].addr.base =
        sizeof(uint32) * (0x3896 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS));

      Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_3[q_prio_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_3[q_prio_idx][reg_idx].uch_queue_wrds_type2),
        sizeof(uint32) * (0x3896 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        8,
        0
      );
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unsched_qs_threshs_for_dp_reg_3[q_prio_idx][reg_idx].uch_queue_wrds_type3),
        sizeof(uint32) * (0x3896 + \
        (q_prio_idx * SOC_PETRA_NOF_DROP_PRECEDENCE * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS) + \
        (reg_idx * SOC_PETRA_NOF_UNSCHED_QS_THRESH_FOR_DP_REGS)),
        sizeof(uint32) * 0x0000,
        24,
        16
      );
    }
  }

  /* Scheduled Per-Queue-Priority Queues Resource Consumption Thresholds */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_0[q_prio_idx].addr.base = sizeof(uint32) * (0x38b3 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS));
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_0[q_prio_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_0[q_prio_idx].total_sch_wrds),
      sizeof(uint32) * (0x38b3 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_0[q_prio_idx].total_sch_pkts),
      sizeof(uint32) * (0x38b3 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Scheduled Per-Queue-Priority Queues Resource Consumption Thresholds */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_1[q_prio_idx].addr.base = sizeof(uint32) * (0x38b4 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS));
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_1[q_prio_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_1[q_prio_idx].avail_sch_dscs),
      sizeof(uint32) * (0x38b4 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_1[q_prio_idx].avail_sch_bufs),
      sizeof(uint32) * (0x38b4 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Scheduled Per-Queue-Priority Queues Resource Consumption Thresholds */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_2[q_prio_idx].addr.base = sizeof(uint32) * (0x38b5 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS));
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_2[q_prio_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_2[q_prio_idx].sch_queue_wrds_type0),
      sizeof(uint32) * (0x38b5 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_2[q_prio_idx].sch_queue_wrds_type1),
      sizeof(uint32) * (0x38b5 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Scheduled Per-Queue-Priority Queues Resource Consumption Thresholds */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_3[q_prio_idx].addr.base = sizeof(uint32) * (0x38b6 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS));
    Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_3[q_prio_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_3[q_prio_idx].sch_queue_wrds_type2),
      sizeof(uint32) * (0x38b6 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_rsc_consumption_threshs_reg_3[q_prio_idx].sch_queue_wrds_type3),
      sizeof(uint32) * (0x38b6 + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_RSC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Scheduled High Queues Resource Consumption Thresholds */
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_0.addr.base = sizeof(uint32) * 0x38b7;
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_0.total_sch_wrds), sizeof(uint32) * 0x38b7, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_0.total_sch_pkts), sizeof(uint32) * 0x38b7, sizeof(uint32) * 0x0000, 24, 16);

  /* Scheduled High Queues Resource Consumption Thresholds */
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_1.addr.base = sizeof(uint32) * 0x38b8;
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_1.avail_sch_dscs), sizeof(uint32) * 0x38b8, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_1.avail_sch_bufs), sizeof(uint32) * 0x38b8, sizeof(uint32) * 0x0000, 24, 16);

  /* Scheduled High Queues Resource Consumption Thresholds */
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_2.addr.base = sizeof(uint32) * 0x38b9;
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_2.sch_queue_wrds_type0), sizeof(uint32) * 0x38b9, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_2.sch_queue_wrds_type1), sizeof(uint32) * 0x38b9, sizeof(uint32) * 0x0000, 24, 16);

  /* Scheduled High Queues Resource Consumption Thresholds */
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_3.addr.base = sizeof(uint32) * 0x38ba;
  Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_3.sch_queue_wrds_type2), sizeof(uint32) * 0x38ba, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.scheduled_high_queues_resource_consumption_thresholds_reg_3.sch_queue_wrds_type3), sizeof(uint32) * 0x38ba, sizeof(uint32) * 0x0000, 24, 16);

  /* Total Resources Consumption Thresholds Configuration For Device Flow Control */
  Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.addr.base = sizeof(uint32) * 0x38bb;
  Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.total_dvc_fc_sch_wrds), sizeof(uint32) * 0x38bb, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.total_dvc_fc_sch_bufs), sizeof(uint32) * 0x38bb, sizeof(uint32) * 0x0000, 24, 16);

  /* Total Resources Consumption Thresholds Configuration For Device Flow Control */
  Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.addr.base = sizeof(uint32) * 0x38bc;
  Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_sch_dscs), sizeof(uint32) * 0x38bc, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_wrds), sizeof(uint32) * 0x38bc, sizeof(uint32) * 0x0000, 24, 16);

  /* Total Resources Consumption Thresholds Configuration For Device Flow Control */
  Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.addr.base = sizeof(uint32) * 0x38bd;
  Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.total_dvc_fc_bufs), sizeof(uint32) * 0x38bd, sizeof(uint32) * 0x0000, 8, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.total_dvc_fc_dscs), sizeof(uint32) * 0x38bd, sizeof(uint32) * 0x0000, 24, 16);

  /* Mci0 Thresolds Configuration */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_0[reg_idx].addr.base =
      sizeof(uint32) * (0x38be + SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_SKIP*reg_idx);
    Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_0[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_0[reg_idx].total_usc_wrds),
      sizeof(uint32) * (0x38be + SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_SKIP*reg_idx),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_0[reg_idx].total_usc_pkts),
      sizeof(uint32) * (0x38be + SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_SKIP*reg_idx),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Mci0 Thresolds Configuration */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_1[reg_idx].addr.base =
      sizeof(uint32) * (0x38bf + SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_SKIP*reg_idx);
    Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_1[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_1[reg_idx].total_usc_bufs),
      sizeof(uint32) * (0x38bf + SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_SKIP*reg_idx),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.mci_thresolds_configuration_reg_1[reg_idx].total_dscs),
      sizeof(uint32) * (0x38bf + SOC_PETRA_NOF_MCI_LVLS_FOR_THRESH_CONF_SKIP*reg_idx),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Channelized Ports Total Packets Flow Control Threshold */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.channelized_ports_total_packets_fc_thresh_reg[reg_idx].addr.base =
      sizeof(uint32) * (0x38c2 + reg_idx);
    Soc_petra_a_regs.egq.channelized_ports_total_packets_fc_thresh_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.channelized_ports_total_packets_fc_thresh_reg[reg_idx].total_cp_npkts_th[fld_idx]),
        sizeof(uint32) * (0x38c2 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)((16 * fld_idx) + 8),
        (uint8)(16 * fld_idx)
      );
    }
  }

  /* Channelized Ports Total Words Flow Control Threshold */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.channelized_ports_total_words_fc_thresh_reg[reg_idx].addr.base =
      sizeof(uint32) * (0x38c7 + reg_idx);
    Soc_petra_a_regs.egq.channelized_ports_total_words_fc_thresh_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.channelized_ports_total_words_fc_thresh_reg[reg_idx].total_cp_nwrds_th[fld_idx]),
        sizeof(uint32) * (0x38c7 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)((16 * fld_idx) + 8),
        (uint8)(16 * fld_idx)
      );
    }
  }

  /* Scheduled Queues Per-Queue-Priority Ofp Flow Control Threshold Types */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[q_prio_idx].addr.base =
      sizeof(uint32) * (0x38cc + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_FC_CONS_THR_REGS));
    Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[q_prio_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[q_prio_idx].fcl_wrds_type0),
      sizeof(uint32) * (0x38cc + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_FC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[q_prio_idx].fcl_wrds_type1),
      sizeof(uint32) * (0x38cc + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_FC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Scheduled Per-Queue-Priority Queues Ofp Flow Control Threshold Types */
  for (q_prio_idx = 0; q_prio_idx < SOC_PETRA_REGS_NOF_EGR_Q_PRIORITIES; q_prio_idx++)
  {
    Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[q_prio_idx].addr.base =
      sizeof(uint32) * (0x38cd + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_FC_CONS_THR_REGS));
    Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[q_prio_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[q_prio_idx].fcl_wrds_type2),
      sizeof(uint32) * (0x38cd + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_FC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      8,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[q_prio_idx].fcl_wrds_type3),
      sizeof(uint32) * (0x38cd + (q_prio_idx * SOC_PETRA_NOF_SCHED_QS_FC_CONS_THR_REGS)),
      sizeof(uint32) * 0x0000,
      24,
      16
    );
  }

  /* Disable Mci */
  Soc_petra_a_regs.egq.disable_mci_reg.addr.base = sizeof(uint32) * 0x38d0;
  Soc_petra_a_regs.egq.disable_mci_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.disable_mci_reg.mci0_dis), sizeof(uint32) * 0x38d0, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.disable_mci_reg.mci1_dis), sizeof(uint32) * 0x38d0, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.disable_mci_reg.erp_fc_en), sizeof(uint32) * 0x38d0, sizeof(uint32) * 0x0000, 2, 2);

  /* Ofp Threshold Type Select */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_OFP_TH_TYPE_SEL_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.ofp_threshold_type_select_reg[reg_idx].addr.base = sizeof(uint32) * (0x38d2 + reg_idx);
    Soc_petra_a_regs.egq.ofp_threshold_type_select_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.ofp_threshold_type_select_reg[reg_idx].qth_sel[fld_idx]),
        sizeof(uint32) * (0x38d2 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(1 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS)*fld_idx)),
        (uint8)(0 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS)*fld_idx))
      );
    }
  }

  /* Total Used Descriptor Counter */
  Soc_petra_a_regs.egq.total_used_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38d7;
  Soc_petra_a_regs.egq.total_used_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_used_descriptor_counter_reg.total_desc), sizeof(uint32) * 0x38d7, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Available Descriptor Counter */
  Soc_petra_a_regs.egq.total_available_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38d8;
  Soc_petra_a_regs.egq.total_available_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_available_descriptor_counter_reg.total_avail_desc), sizeof(uint32) * 0x38d8, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Scheduled Used Descriptor Counter */
  Soc_petra_a_regs.egq.total_scheduled_used_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38d9;
  Soc_petra_a_regs.egq.total_scheduled_used_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_scheduled_used_descriptor_counter_reg.total_sch_desc), sizeof(uint32) * 0x38d9, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Unscheduled Used Descriptor Counter */
  Soc_petra_a_regs.egq.total_unscheduled_used_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38da;
  Soc_petra_a_regs.egq.total_unscheduled_used_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_unscheduled_used_descriptor_counter_reg.total_usc_desc), sizeof(uint32) * 0x38da, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Used Unicast High Descriptor Counter */
  Soc_petra_a_regs.egq.total_used_unicast_high_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38db;
  Soc_petra_a_regs.egq.total_used_unicast_high_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_used_unicast_high_descriptor_counter_reg.total_desc_uch), sizeof(uint32) * 0x38db, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Used Unicast Low Descriptor Counter */
  Soc_petra_a_regs.egq.total_used_unicast_low_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38dc;
  Soc_petra_a_regs.egq.total_used_unicast_low_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_used_unicast_low_descriptor_counter_reg.total_desc_ucl), sizeof(uint32) * 0x38dc, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Used Multicast High Descriptor Counter */
  Soc_petra_a_regs.egq.total_used_multicast_high_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38dd;
  Soc_petra_a_regs.egq.total_used_multicast_high_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_used_multicast_high_descriptor_counter_reg.total_desc_mch), sizeof(uint32) * 0x38dd, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Used Multicast Low Descriptor Counter */
  Soc_petra_a_regs.egq.total_used_multicast_low_descriptor_counter_reg.addr.base = sizeof(uint32) * 0x38de;
  Soc_petra_a_regs.egq.total_used_multicast_low_descriptor_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_used_multicast_low_descriptor_counter_reg.total_desc_mcl), sizeof(uint32) * 0x38de, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Scheduled Used Buffers */
  Soc_petra_a_regs.egq.total_scheduled_used_buffers_reg.addr.base = sizeof(uint32) * 0x38df;
  Soc_petra_a_regs.egq.total_scheduled_used_buffers_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_scheduled_used_buffers_reg.total_sch_bufs), sizeof(uint32) * 0x38df, sizeof(uint32) * 0x0000, 12, 0);

  /* Total Unscheduled Used Buffers */
  Soc_petra_a_regs.egq.total_unscheduled_used_buffers_reg.addr.base = sizeof(uint32) * 0x38e0;
  Soc_petra_a_regs.egq.total_unscheduled_used_buffers_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_unscheduled_used_buffers_reg.total_usc_bufs), sizeof(uint32) * 0x38e0, sizeof(uint32) * 0x0000, 12, 0);

  /* Total Scheduled Used Words */
  Soc_petra_a_regs.egq.total_scheduled_used_words_reg.addr.base = sizeof(uint32) * 0x38e1;
  Soc_petra_a_regs.egq.total_scheduled_used_words_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_scheduled_used_words_reg.total_sch_wrds), sizeof(uint32) * 0x38e1, sizeof(uint32) * 0x0000, 14, 0);

  /* Total Unscheduled Used Words */
  Soc_petra_a_regs.egq.total_unscheduled_used_words_reg.addr.base = sizeof(uint32) * 0x38e2;
  Soc_petra_a_regs.egq.total_unscheduled_used_words_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_unscheduled_used_words_reg.total_usc_wrds), sizeof(uint32) * 0x38e2, sizeof(uint32) * 0x0000, 14, 0);

  /* Total Nifa Channel0 Used Packets */
  Soc_petra_a_regs.egq.total_nifa_channel0_used_packets_reg.addr.base = sizeof(uint32) * 0x38e3;
  Soc_petra_a_regs.egq.total_nifa_channel0_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifa_channel0_used_packets_reg.total_nifa_ch0_cp_pkts), sizeof(uint32) * 0x38e3, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifa Channel1 Used Packets */
  Soc_petra_a_regs.egq.total_nifa_channel1_used_packets_reg.addr.base = sizeof(uint32) * 0x38e4;
  Soc_petra_a_regs.egq.total_nifa_channel1_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifa_channel1_used_packets_reg.total_nifa_ch1_cp_pkts), sizeof(uint32) * 0x38e4, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifa Channel2 Used Packets */
  Soc_petra_a_regs.egq.total_nifa_channel2_used_packets_reg.addr.base = sizeof(uint32) * 0x38e5;
  Soc_petra_a_regs.egq.total_nifa_channel2_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifa_channel2_used_packets_reg.total_nifa_ch2_cp_pkts), sizeof(uint32) * 0x38e5, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifa Channel3 Used Packets */
  Soc_petra_a_regs.egq.total_nifa_channel3_used_packets_reg.addr.base = sizeof(uint32) * 0x38e6;
  Soc_petra_a_regs.egq.total_nifa_channel3_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifa_channel3_used_packets_reg.total_nifa_ch3_cp_pkts), sizeof(uint32) * 0x38e6, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifb Channel0 Used Packets */
  Soc_petra_a_regs.egq.total_nifb_channel0_used_packets_reg.addr.base = sizeof(uint32) * 0x38e7;
  Soc_petra_a_regs.egq.total_nifb_channel0_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifb_channel0_used_packets_reg.total_nifb_ch0_cp_pkts), sizeof(uint32) * 0x38e7, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifb Channel1 Used Packets */
  Soc_petra_a_regs.egq.total_nifb_channel1_used_packets_reg.addr.base = sizeof(uint32) * 0x38e8;
  Soc_petra_a_regs.egq.total_nifb_channel1_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifb_channel1_used_packets_reg.total_nifb_ch1_cp_pkts), sizeof(uint32) * 0x38e8, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifb Channel2 Used Packets */
  Soc_petra_a_regs.egq.total_nifb_channel2_used_packets_reg.addr.base = sizeof(uint32) * 0x38e9;
  Soc_petra_a_regs.egq.total_nifb_channel2_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifb_channel2_used_packets_reg.total_nifb_ch2_cp_pkts), sizeof(uint32) * 0x38e9, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Nifb Channel3 Used Packets */
  Soc_petra_a_regs.egq.total_nifb_channel3_used_packets_reg.addr.base = sizeof(uint32) * 0x38ea;
  Soc_petra_a_regs.egq.total_nifb_channel3_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_nifb_channel3_used_packets_reg.total_nifb_ch3_cp_pkts), sizeof(uint32) * 0x38ea, sizeof(uint32) * 0x0000, 13, 0);

  /* Total CPUUsed Packets */
  Soc_petra_a_regs.egq.total_cpuused_packets_reg.addr.base = sizeof(uint32) * 0x38eb;
  Soc_petra_a_regs.egq.total_cpuused_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_cpuused_packets_reg.total_cpu_cp_pkts), sizeof(uint32) * 0x38eb, sizeof(uint32) * 0x0000, 13, 0);

  /* Total Recycling Used Packets */
  Soc_petra_a_regs.egq.total_recycling_used_packets_reg.addr.base = sizeof(uint32) * 0x38ec;
  Soc_petra_a_regs.egq.total_recycling_used_packets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.total_recycling_used_packets_reg.total_rcy_cp_pkts), sizeof(uint32) * 0x38ec, sizeof(uint32) * 0x0000, 13, 0);

  /* Low Bandwidth Ofp Bitmap */
  Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_0.addr.base = sizeof(uint32) * 0x38f0;
  Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_0.low_bw_ofp), sizeof(uint32) * 0x38f0, sizeof(uint32) * 0x0000, 31, 0);

  /* Low Bandwidth Ofp Bitmap */
  Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_1.addr.base = sizeof(uint32) * 0x38f1;
  Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_1.low_bw_ofp), sizeof(uint32) * 0x38f1, sizeof(uint32) * 0x0000, 31, 0);

  /* Low Bandwidth Ofp Bitmap */
  Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_2.addr.base = sizeof(uint32) * 0x38f2;
  Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.low_bandwidth_ofp_bitmap_reg_2.low_bw_ofp), sizeof(uint32) * 0x38f2, sizeof(uint32) * 0x0000, 15, 0);

  /* Unicast Egress Packet Header Compensation */
  for (reg_idx = 0; reg_idx < SOC_PETRA_REGS_PER_HDR_CR_DSCNT_TYPE_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.unicast_egress_packet_header_compensation_reg[reg_idx].addr.base = sizeof(uint32) * (0x38f3 + reg_idx);
    Soc_petra_a_regs.egq.unicast_egress_packet_header_compensation_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_uc_raw), sizeof(uint32) * (0x38f3 + reg_idx), sizeof(uint32) * 0x0000, 7, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_uc_cpu), sizeof(uint32) * (0x38f3 + reg_idx), sizeof(uint32) * 0x0000, 15, 8);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_uc_eth), sizeof(uint32) * (0x38f3 + reg_idx), sizeof(uint32) * 0x0000, 23, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_uc_tm), sizeof(uint32) * (0x38f3 + reg_idx), sizeof(uint32) * 0x0000, 31, 24);
  }

  /* Multicast Egress Packet Header Compensation */
  for (reg_idx = 0; reg_idx < SOC_PETRA_REGS_PER_HDR_CR_DSCNT_TYPE_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.multicast_egress_packet_header_compensation_reg[reg_idx].addr.base = sizeof(uint32) * (0x38f5 + reg_idx);
    Soc_petra_a_regs.egq.multicast_egress_packet_header_compensation_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_mc_raw), sizeof(uint32) * (0x38f5 + reg_idx), sizeof(uint32) * 0x0000, 7, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_mc_cpu), sizeof(uint32) * (0x38f5 + reg_idx), sizeof(uint32) * 0x0000, 15, 8);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_mc_eth), sizeof(uint32) * (0x38f5 + reg_idx), sizeof(uint32) * 0x0000, 23, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_egress_packet_header_compensation_reg[reg_idx].hdr_adjust_mc_tm), sizeof(uint32) * (0x38f5 + reg_idx), sizeof(uint32) * 0x0000, 31, 24);
  }

  /* Egress Interface No Fragmentation Mode Configuration */
  Soc_petra_a_regs.egq.egress_interface_no_fragmentation_mode_configuration_reg.addr.base = sizeof(uint32) * 0x38f8;
  Soc_petra_a_regs.egq.egress_interface_no_fragmentation_mode_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_interface_no_fragmentation_mode_configuration_reg.nifa_no_frag), sizeof(uint32) * 0x38f8, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_interface_no_fragmentation_mode_configuration_reg.nifb_no_frag), sizeof(uint32) * 0x38f8, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_interface_no_fragmentation_mode_configuration_reg.rcy_no_frag), sizeof(uint32) * 0x38f8, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_interface_no_fragmentation_mode_configuration_reg.cpu_no_frag), sizeof(uint32) * 0x38f8, sizeof(uint32) * 0x0000, 9, 9);

  /* Egress Aging Settings */
  Soc_petra_a_regs.egq.egress_aging_settings_reg.addr.base = sizeof(uint32) * 0x38f9;
  Soc_petra_a_regs.egq.egress_aging_settings_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_aging_settings_reg.aging_ena), sizeof(uint32) * 0x38f9, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_aging_settings_reg.aging_fc), sizeof(uint32) * 0x38f9, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_aging_settings_reg.aging_rd_q), sizeof(uint32) * 0x38f9, sizeof(uint32) * 0x0000, 2, 2);

  /* Egress Aging Time Settings */
  Soc_petra_a_regs.egq.egress_aging_time_settings_reg.addr.base = sizeof(uint32) * 0x38fa;
  Soc_petra_a_regs.egq.egress_aging_time_settings_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_aging_time_settings_reg.aging_time), sizeof(uint32) * 0x38fa, sizeof(uint32) * 0x0000, 2, 0);

  /* Force Not Ready On All Egress OFP */
  Soc_petra_a_regs.egq.force_not_ready_on_all_egress_ofp_reg.addr.base = sizeof(uint32) * 0x3901;
  Soc_petra_a_regs.egq.force_not_ready_on_all_egress_ofp_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.force_not_ready_on_all_egress_ofp_reg.force_not_rdy), sizeof(uint32) * 0x3901, sizeof(uint32) * 0x0000, 0, 0);

  /* Egress Port Priority Configuration */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.egress_port_prio_conf_reg[reg_idx].addr.base =
      sizeof(uint32) * (0x3902 + reg_idx);
    Soc_petra_a_regs.egq.egress_port_prio_conf_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.egress_port_prio_conf_reg[reg_idx].port_priority[fld_idx]),
        sizeof(uint32) * (0x3902 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(1 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS)*fld_idx)),
        (uint8)(0 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS)*fld_idx))
      );
    }
  }

  /* Mapping Configuration For Nif Channelized Ports */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_MAP_CONF_CHNIF_REGS; reg_idx++)
  {
    for (inner_reg_idx = 0; inner_reg_idx < SOC_PETRA_NOF_MAP_CONF_PER_CHNIF_REGS; inner_reg_idx++)
    {
      Soc_petra_a_regs.egq.map_conf_chnif_port_reg[reg_idx][inner_reg_idx].addr.base =
        sizeof(uint32) * (0x3907 + (reg_idx * SOC_PETRA_NOF_MAP_CONF_PER_CHNIF_REGS) + inner_reg_idx);
      Soc_petra_a_regs.egq.map_conf_chnif_port_reg[reg_idx][inner_reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.map_conf_chnif_port_reg[reg_idx][inner_reg_idx].map_nif_port_ch),
        sizeof(uint32) * (0x3907 + (reg_idx * SOC_PETRA_NOF_MAP_CONF_PER_CHNIF_REGS) + inner_reg_idx),
        sizeof(uint32) * 0x0000,
        31,
        0
      );
    }
  }

  /* Mapping Configuration For Nifa Non Channelized Ports */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_MAP_CONF_NONCH_NIF_REGS; reg_idx++)
  {
    for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_MAP_CONF_NONCH_NIF_FLDS; fld_idx++)
    {
      Soc_petra_a_regs.egq.map_conf_nif_non_ch_ports_reg[reg_idx].addr.base = sizeof(uint32) * (0x391f + reg_idx);
      Soc_petra_a_regs.egq.map_conf_nif_non_ch_ports_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.map_conf_nif_non_ch_ports_reg[reg_idx].ofp_index[fld_idx]),
        sizeof(uint32) * (0x391f + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(6 + (8 * fld_idx)),
        (uint8)(0 + (8 * fld_idx))
      );
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.map_conf_nif_non_ch_ports_reg[reg_idx].valid[fld_idx]),
        sizeof(uint32) * (0x391f + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(7 + (8 * fld_idx)),
        (uint8)(7 + (8 * fld_idx))
      );
    }
  }

  /* Mapping Configuration For Recycling Ports */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_MAP_CONF_PER_CHNIF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.map_conf_rcy_ports_reg[reg_idx].addr.base = sizeof(uint32) * (0x3925 + reg_idx);
    Soc_petra_a_regs.egq.map_conf_rcy_ports_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.map_conf_rcy_ports_reg[reg_idx].map_rcy_port),
      sizeof(uint32) * (0x3925 + reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* Mapping Configuration For Cpu Ports */
  Soc_petra_a_regs.egq.map_conf_cpu_ports_reg.addr.base = sizeof(uint32) * 0x3928;
  Soc_petra_a_regs.egq.map_conf_cpu_ports_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.map_conf_cpu_ports_reg.map_cpu_port), sizeof(uint32) * 0x3928, sizeof(uint32) * 0x0000, 7, 0);

  /* Disable Egress OFP */
  Soc_petra_a_regs.egq.disable_egress_ofp_reg[0].addr.base = sizeof(uint32) * 0x3929;
  Soc_petra_a_regs.egq.disable_egress_ofp_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.disable_egress_ofp_reg[0].disable_egress_ofp), sizeof(uint32) * 0x3929, sizeof(uint32) * 0x0000, 31, 0);

  Soc_petra_a_regs.egq.disable_egress_ofp_reg[1].addr.base = sizeof(uint32) * 0x392a;
  Soc_petra_a_regs.egq.disable_egress_ofp_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.disable_egress_ofp_reg[1].disable_egress_ofp), sizeof(uint32) * 0x392a, sizeof(uint32) * 0x0000, 31, 0);

  Soc_petra_a_regs.egq.disable_egress_ofp_reg[2].addr.base = sizeof(uint32) * 0x392b;
  Soc_petra_a_regs.egq.disable_egress_ofp_reg[2].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.disable_egress_ofp_reg[2].disable_egress_ofp), sizeof(uint32) * 0x392b, sizeof(uint32) * 0x0000, 15, 0);

  /* Fragmentation Queues Ready Words Threshold */
  Soc_petra_a_regs.egq.fragmentation_queues_ready_words_threshold_reg.addr.base = sizeof(uint32) * 0x392c;
  Soc_petra_a_regs.egq.fragmentation_queues_ready_words_threshold_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.fragmentation_queues_ready_words_threshold_reg.fq_rdy_th), sizeof(uint32) * 0x392c, sizeof(uint32) * 0x0000, 9, 0);


  for (reg_idx = 0; reg_idx < 3; reg_idx++)
  {
    /* Oc768 Mask Reg */
    Soc_petra_a_regs.egq.fragmentation_queues_empty_indication[reg_idx].addr.base = sizeof(uint32) * (0x392f + reg_idx );
    Soc_petra_a_regs.egq.fragmentation_queues_empty_indication[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.fragmentation_queues_empty_indication[reg_idx].fqempty), sizeof(uint32) * (0x392f + reg_idx ), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* Oc768 Mask Reg */
  Soc_petra_a_regs.egq.oc768_mask_reg.addr.base = sizeof(uint32) * 0x3932;
  Soc_petra_a_regs.egq.oc768_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.oc768_mask_reg.oc768_mask), sizeof(uint32) * 0x3932, sizeof(uint32) * 0x0000, 3, 0);

  /* Nifa Sgmii */
  Soc_petra_a_regs.egq.nifa_sgmii_reg.addr.base = sizeof(uint32) * 0x3933;
  Soc_petra_a_regs.egq.nifa_sgmii_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifa_sgmii_reg.nifa_sgmii), sizeof(uint32) * 0x3933, sizeof(uint32) * 0x0000, 3, 0);

  /* Nifb Sgmii */
  Soc_petra_a_regs.egq.nifb_sgmii_reg.addr.base = sizeof(uint32) * 0x3934;
  Soc_petra_a_regs.egq.nifb_sgmii_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifb_sgmii_reg.nifb_sgmii), sizeof(uint32) * 0x3934, sizeof(uint32) * 0x0000, 3, 0);

  /* Nifa Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_0.addr.base = sizeof(uint32) * 0x3935;
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_0.nifa_sgmii_ports_delay), sizeof(uint32) * 0x3935, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifa Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_1.addr.base = sizeof(uint32) * 0x3936;
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_1.nifa_sgmii_ports_delay), sizeof(uint32) * 0x3936, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifa Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_2.addr.base = sizeof(uint32) * 0x3937;
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_2.nifa_sgmii_ports_delay), sizeof(uint32) * 0x3937, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifa Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_3.addr.base = sizeof(uint32) * 0x3938;
  Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifa_sgmii_ports_delay_configuration_reg_3.nifa_sgmii_ports_delay), sizeof(uint32) * 0x3938, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifb Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_0.addr.base = sizeof(uint32) * 0x3939;
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_0.nifb_sgmii_ports_delay), sizeof(uint32) * 0x3939, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifb Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_1.addr.base = sizeof(uint32) * 0x393a;
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_1.nifb_sgmii_ports_delay), sizeof(uint32) * 0x393a, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifb Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_2.addr.base = sizeof(uint32) * 0x393b;
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_2.nifb_sgmii_ports_delay), sizeof(uint32) * 0x393b, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifb Sgmii Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_3.addr.base = sizeof(uint32) * 0x393c;
  Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifb_sgmii_ports_delay_configuration_reg_3.nifb_sgmii_ports_delay), sizeof(uint32) * 0x393c, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifa Spaui Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifa_spaui_ports_delay_configuration_reg.addr.base = sizeof(uint32) * 0x393d;
  Soc_petra_a_regs.egq.nifa_spaui_ports_delay_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifa_spaui_ports_delay_configuration_reg.nifa_spaui_ports_delay), sizeof(uint32) * 0x393d, sizeof(uint32) * 0x0000, 31, 0);

  /* Nifb Spaui Ports Delay Configuration */
  Soc_petra_a_regs.egq.nifb_spaui_ports_delay_configuration_reg.addr.base = sizeof(uint32) * 0x393e;
  Soc_petra_a_regs.egq.nifb_spaui_ports_delay_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nifb_spaui_ports_delay_configuration_reg.nifb_spaui_ports_delay), sizeof(uint32) * 0x393e, sizeof(uint32) * 0x0000, 31, 0);

  /* Nif Sp Over Cpu Rcy */
  Soc_petra_a_regs.egq.nif_sp_over_cpu_rcy_reg.addr.base = sizeof(uint32) * 0x393f;
  Soc_petra_a_regs.egq.nif_sp_over_cpu_rcy_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nif_sp_over_cpu_rcy_reg.nif_sp_ovr_cpu), sizeof(uint32) * 0x393f, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.nif_sp_over_cpu_rcy_reg.nif_sp_ovr_rcy), sizeof(uint32) * 0x393f, sizeof(uint32) * 0x0000, 1, 1);

  /* Packet Aged Counter */
  Soc_petra_a_regs.egq.packet_aged_counter_reg.addr.base = sizeof(uint32) * 0x3940;
  Soc_petra_a_regs.egq.packet_aged_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.packet_aged_counter_reg.pkt_aged_cnt), sizeof(uint32) * 0x3940, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.packet_aged_counter_reg.pkt_aged_cnt_ovf), sizeof(uint32) * 0x3940, sizeof(uint32) * 0x0000, 31, 31);

  /* Egress Replication Bitmap Group Value Configuration */
  Soc_petra_a_regs.egq.egress_replication_bitmap_group_value_configuration_reg.addr.base = sizeof(uint32) * 0x3945;
  Soc_petra_a_regs.egq.egress_replication_bitmap_group_value_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_replication_bitmap_group_value_configuration_reg.egress_rep_bitmap_group_value_bot), sizeof(uint32) * 0x3945, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_replication_bitmap_group_value_configuration_reg.egress_rep_bitmap_group_value_top), sizeof(uint32) * 0x3945, sizeof(uint32) * 0x0000, 29, 16);

  /* Rlag2 System Port Configuration */
  Soc_petra_a_regs.egq.rlag2_system_port_configuration_reg.addr.base = sizeof(uint32) * 0x3946;
  Soc_petra_a_regs.egq.rlag2_system_port_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.rlag2_system_port_configuration_reg.rlag2_sys_port_id_bot), sizeof(uint32) * 0x3946, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.rlag2_system_port_configuration_reg.rlag2_sys_port_id_top), sizeof(uint32) * 0x3946, sizeof(uint32) * 0x0000, 27, 16);

  /* Rlag4 Sys Port Id */
  Soc_petra_a_regs.egq.rlag4_sys_port_id_reg.addr.base = sizeof(uint32) * 0x3947;
  Soc_petra_a_regs.egq.rlag4_sys_port_id_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.rlag4_sys_port_id_reg.rlag4_sys_port_id_bot), sizeof(uint32) * 0x3947, sizeof(uint32) * 0x0000, 11, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.rlag4_sys_port_id_reg.rlag4_sys_port_id_top), sizeof(uint32) * 0x3947, sizeof(uint32) * 0x0000, 27, 16);

  /* Unicast To Unicast Class Lookup Table */
  Soc_petra_a_regs.egq.unicast_to_unicast_class_lookup_table_reg.addr.base = sizeof(uint32) * 0x3948;
  Soc_petra_a_regs.egq.unicast_to_unicast_class_lookup_table_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.unicast_to_unicast_class_lookup_table_reg.uc2uc_class_table), sizeof(uint32) * 0x3948, sizeof(uint32) * 0x0000, 31, 0);

  /* Multicast To Unicast Class Lookup Table */
  Soc_petra_a_regs.egq.multicast_to_unicast_class_lookup_table_reg.addr.base = sizeof(uint32) * 0x3949;
  Soc_petra_a_regs.egq.multicast_to_unicast_class_lookup_table_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_to_unicast_class_lookup_table_reg.mc2uc_class_table), sizeof(uint32) * 0x3949, sizeof(uint32) * 0x0000, 31, 0);

  /* Multicast To Multicast Class Lookup Table */
  Soc_petra_a_regs.egq.multicast_to_multicast_class_lookup_table_reg.addr.base = sizeof(uint32) * 0x394a;
  Soc_petra_a_regs.egq.multicast_to_multicast_class_lookup_table_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_to_multicast_class_lookup_table_reg.mc2mc_class_table), sizeof(uint32) * 0x394a, sizeof(uint32) * 0x0000, 31, 0);

  /* Multicast To Multicast Drop Precedence Lookup Table */
  Soc_petra_a_regs.egq.multicast_to_multicast_drop_precedence_lookup_table_reg[0].addr.base = sizeof(uint32) * 0x394b;
  Soc_petra_a_regs.egq.multicast_to_multicast_drop_precedence_lookup_table_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_to_multicast_drop_precedence_lookup_table_reg[0].mc2mc_dp_table), sizeof(uint32) * 0x394b, sizeof(uint32) * 0x0000, 31, 0);

  /* Multicast To Multicast Drop Precedence Lookup Table */
  Soc_petra_a_regs.egq.multicast_to_multicast_drop_precedence_lookup_table_reg[1].addr.base = sizeof(uint32) * 0x394c;
  Soc_petra_a_regs.egq.multicast_to_multicast_drop_precedence_lookup_table_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_to_multicast_drop_precedence_lookup_table_reg[1].mc2mc_dp_table), sizeof(uint32) * 0x394c, sizeof(uint32) * 0x0000, 31, 0);

  /* Mc Priority Lookup Table */
  Soc_petra_a_regs.egq.mc_priority_lookup_table_reg.addr.base = sizeof(uint32) * 0x394d;
  Soc_petra_a_regs.egq.mc_priority_lookup_table_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.mc_priority_lookup_table_reg.mc_priority), sizeof(uint32) * 0x394d, sizeof(uint32) * 0x0000, 31, 0);

  /* Multicast Link List Max Length */
  Soc_petra_a_regs.egq.multicast_link_list_max_length_reg.addr.base = sizeof(uint32) * 0x394e;
  Soc_petra_a_regs.egq.multicast_link_list_max_length_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_link_list_max_length_reg.link_list_max_length), sizeof(uint32) * 0x394e, sizeof(uint32) * 0x0000, 14, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.multicast_link_list_max_length_reg.link_list_max_length_vld), sizeof(uint32) * 0x394e, sizeof(uint32) * 0x0000, 16, 16);

  /* Marvel Configuration Register */
  Soc_petra_a_regs.egq.marvel_configuration_reg.addr.base = sizeof(uint32) * 0x394f;
  Soc_petra_a_regs.egq.marvel_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_mh_mode), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_mid_vidx), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_dsa_to_analyzer_ingress_ofp), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 9, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_dsa_to_analyzer_egress_ofp), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 13, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_other_low_pr_table), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_to_cpu_low_pr), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_configuration_reg.marvel_to_analyzer_low_pr), sizeof(uint32) * 0x394f, sizeof(uint32) * 0x0000, 28, 28);

  /* Marvel Dx Forward Low Pr Table */
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_0.addr.base = sizeof(uint32) * 0x3950;
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_0.marvel_dx_forward_low_pr_table), sizeof(uint32) * 0x3950, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Dx Forward Low Pr Table */
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_1.addr.base = sizeof(uint32) * 0x3951;
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_1.marvel_dx_forward_low_pr_table), sizeof(uint32) * 0x3951, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Dx Forward Low Pr Table */
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_2.addr.base = sizeof(uint32) * 0x3952;
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_2.marvel_dx_forward_low_pr_table), sizeof(uint32) * 0x3952, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Dx Forward Low Pr Table */
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_3.addr.base = sizeof(uint32) * 0x3953;
  Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_dx_forward_low_pr_table_reg_3.marvel_dx_forward_low_pr_table), sizeof(uint32) * 0x3953, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_0.addr.base = sizeof(uint32) * 0x3954;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_0.marvel_port_map_table_other), sizeof(uint32) * 0x3954, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_1.addr.base = sizeof(uint32) * 0x3955;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_1.marvel_port_map_table_other), sizeof(uint32) * 0x3955, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_2.addr.base = sizeof(uint32) * 0x3956;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_2.marvel_port_map_table_other), sizeof(uint32) * 0x3956, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_3.addr.base = sizeof(uint32) * 0x3957;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_3.marvel_port_map_table_other), sizeof(uint32) * 0x3957, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_4.addr.base = sizeof(uint32) * 0x3958;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_4.marvel_port_map_table_other), sizeof(uint32) * 0x3958, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_5.addr.base = sizeof(uint32) * 0x3959;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_5.marvel_port_map_table_other), sizeof(uint32) * 0x3959, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_6.addr.base = sizeof(uint32) * 0x395a;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_6.marvel_port_map_table_other), sizeof(uint32) * 0x395a, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Other */
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_7.addr.base = sizeof(uint32) * 0x395b;
  Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_other_reg_7.marvel_port_map_table_other), sizeof(uint32) * 0x395b, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_0.addr.base = sizeof(uint32) * 0x395c;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_0.marvel_port_map_table_cpu), sizeof(uint32) * 0x395c, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_1.addr.base = sizeof(uint32) * 0x395d;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_1.marvel_port_map_table_cpu), sizeof(uint32) * 0x395d, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_2.addr.base = sizeof(uint32) * 0x395e;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_2.marvel_port_map_table_cpu), sizeof(uint32) * 0x395e, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_3.addr.base = sizeof(uint32) * 0x395f;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_3.marvel_port_map_table_cpu), sizeof(uint32) * 0x395f, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_4.addr.base = sizeof(uint32) * 0x3960;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_4.marvel_port_map_table_cpu), sizeof(uint32) * 0x3960, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_5.addr.base = sizeof(uint32) * 0x3961;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_5.marvel_port_map_table_cpu), sizeof(uint32) * 0x3961, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_6.addr.base = sizeof(uint32) * 0x3962;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_6.marvel_port_map_table_cpu), sizeof(uint32) * 0x3962, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_7.addr.base = sizeof(uint32) * 0x3963;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_7.marvel_port_map_table_cpu), sizeof(uint32) * 0x3963, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_8.addr.base = sizeof(uint32) * 0x3964;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_8.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_8.marvel_port_map_table_cpu), sizeof(uint32) * 0x3964, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_9.addr.base = sizeof(uint32) * 0x3965;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_9.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_9.marvel_port_map_table_cpu), sizeof(uint32) * 0x3965, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_10.addr.base = sizeof(uint32) * 0x3966;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_10.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_10.marvel_port_map_table_cpu), sizeof(uint32) * 0x3966, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_11.addr.base = sizeof(uint32) * 0x3967;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_11.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_11.marvel_port_map_table_cpu), sizeof(uint32) * 0x3967, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_12.addr.base = sizeof(uint32) * 0x3968;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_12.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_12.marvel_port_map_table_cpu), sizeof(uint32) * 0x3968, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_13.addr.base = sizeof(uint32) * 0x3969;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_13.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_13.marvel_port_map_table_cpu), sizeof(uint32) * 0x3969, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_14.addr.base = sizeof(uint32) * 0x396a;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_14.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_14.marvel_port_map_table_cpu), sizeof(uint32) * 0x396a, sizeof(uint32) * 0x0000, 31, 0);

  /* Marvel Port Map Table Cpu */
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_15.addr.base = sizeof(uint32) * 0x396b;
  Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_15.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.marvel_port_map_table_cpu_reg_15.marvel_port_map_table_cpu), sizeof(uint32) * 0x396b, sizeof(uint32) * 0x0000, 31, 0);

  /* Egress Shaper Enable Settings */
  Soc_petra_a_regs.egq.egress_shaper_enable_settings_reg.addr.base = sizeof(uint32) * 0x396c;
  Soc_petra_a_regs.egq.egress_shaper_enable_settings_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_shaper_enable_settings_reg.sch_enable), sizeof(uint32) * 0x396c, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_shaper_enable_settings_reg.spr_set_sel), sizeof(uint32) * 0x396c, sizeof(uint32) * 0x0000, 1, 1);

  /* Egress Shapers Credit Memory Init Status */
  Soc_petra_a_regs.egq.egress_shapers_credit_memory_init_status_reg.addr.base = sizeof(uint32) * 0x396d;
  Soc_petra_a_regs.egq.egress_shapers_credit_memory_init_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.egress_shapers_credit_memory_init_status_reg.sch_init), sizeof(uint32) * 0x396d, sizeof(uint32) * 0x0000, 0, 0);

  /* Egress Shaper Calendars Arbitration Cycle Length */
  Soc_petra_a_regs.egq.egress_shaper_calendars_arbitration_cycle_length_reg.addr.base = sizeof(uint32) * 0x396e;
  Soc_petra_a_regs.egq.egress_shaper_calendars_arbitration_cycle_length_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_INSTANCES_EGQ_CAL_SETS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.egq.egress_shaper_calendars_arbitration_cycle_length_reg.cal_cal_len[fld_idx]),
      sizeof(uint32) * 0x396e,
      sizeof(uint32) * 0x0000,
      (uint8)(fld_idx * 8 + 7),
      (uint8)(fld_idx * 8 + 0)
    );
  }

  /*
   * Note! For per-calendar-group registers, instance differentiates
   * between Group A and Group B registers.
   */

  /* Egress Shaper Configuration For Nif Channelized Port */
  for (reg_idx = 0; reg_idx < SOC_PETRA_EGR_SHPR_CONF_NIF_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.egress_shpr_conf_nif_reg[reg_idx].addr.base = sizeof(uint32) * (0x396f + reg_idx);
    Soc_petra_a_regs.egq.egress_shpr_conf_nif_reg[reg_idx].addr.step = sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET;
    soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_nif_reg[reg_idx].spr_rate),
    sizeof(uint32) * (0x396f + reg_idx),
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    15,
    0
  );
    soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_nif_reg[reg_idx].spr_max_burst),
    sizeof(uint32) * (0x396f + reg_idx),
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    28,
    16
  );
  }

  /* Egress Shaper Configuration For Nifa And Nifb Non Channelized Ports */
  Soc_petra_a_regs.egq.egress_shpr_conf_for_nifa_and_nifb_non_channelized_ports_reg.addr.base = sizeof(uint32) * 0x3977;
  Soc_petra_a_regs.egq.egress_shpr_conf_for_nifa_and_nifb_non_channelized_ports_reg.addr.step = sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET;
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_nifa_and_nifb_non_channelized_ports_reg.nifab_nch_spr_rate),
    sizeof(uint32) * 0x3977,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    15,
    0
  );
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_nifa_and_nifb_non_channelized_ports_reg.nifab_nch_spr_max_burst),
    sizeof(uint32) * 0x3977,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    28,
    16
  );

  /* Egress Shaper Configuration For Recycle Interface Ports */
  Soc_petra_a_regs.egq.egress_shpr_conf_for_recycle_interface_ports_reg.addr.base = sizeof(uint32) *  0x3978;
  Soc_petra_a_regs.egq.egress_shpr_conf_for_recycle_interface_ports_reg.addr.step = sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET;
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_recycle_interface_ports_reg.rcy_spr_rate),
    sizeof(uint32) *  0x3978,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    15,
    0
  );
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_recycle_interface_ports_reg.rcy_spr_max_burst),
    sizeof(uint32) *  0x3978,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    28,
    16
  );

  /* Egress Shaper Configuration For Eci Ports */
  Soc_petra_a_regs.egq.egress_shpr_conf_for_eci_ports_reg.addr.base = sizeof(uint32) * 0x3979;
  Soc_petra_a_regs.egq.egress_shpr_conf_for_eci_ports_reg.addr.step = sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET;
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_eci_ports_reg.cpu_spr_rate),
    sizeof(uint32) * 0x3979,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    15,
    0
  );
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_eci_ports_reg.cpu_spr_max_burst),
    sizeof(uint32) * 0x3979,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    28,
    16
  );

  /* Egress Shaper Configuration For Nif Calendars Length */
  for (reg_idx = 0; reg_idx < SOC_PETRA_EGR_SHPR_PER_NIF_GRP_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.egq.egress_shpr_conf_for_nif_cal_len_reg[reg_idx].addr.base = sizeof(uint32) * (0x397a + reg_idx);
    Soc_petra_a_regs.egq.egress_shpr_conf_for_nif_cal_len_reg[reg_idx].addr.step = sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET;
    for (fld_idx = 0; fld_idx < SOC_PETRA_EGR_SHPR_PER_NIF_GRP_NOF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.egq.egress_shpr_conf_for_nif_cal_len_reg[reg_idx].nif_ch_spr_cal_len[fld_idx]),
        sizeof(uint32) * (0x397a + reg_idx),
        sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
        (uint8)(6 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_EGR_SHPR_PER_NIF_GRP_NOF_FLDS)*fld_idx)),
        (uint8)(0 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_EGR_SHPR_PER_NIF_GRP_NOF_FLDS)*fld_idx))
      );
    }
  }

  /* Egress Shaper Configuration For Other Calendars Length */
  Soc_petra_a_regs.egq.egress_shpr_conf_for_other_calendars_length_reg.addr.base = sizeof(uint32) * 0x397c;
  Soc_petra_a_regs.egq.egress_shpr_conf_for_other_calendars_length_reg.addr.step = sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET;
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_other_calendars_length_reg.nifab_nch_spr_cal_len),
    sizeof(uint32) * 0xe1ee,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    6,
    0
  );
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_other_calendars_length_reg.rcy_spr_cal_len),
    sizeof(uint32) * 0x397c,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    14,
    8
  );
  soc_petra_reg_fld_set(
    &(Soc_petra_a_regs.egq.egress_shpr_conf_for_other_calendars_length_reg.cpu_spr_cal_len),
    sizeof(uint32) * 0x397c,
    sizeof(uint32) * SOC_PETRA_INSTANCES_EGQ_CAL_SETS_OFFSET,
    22,
    16
  );

  /* Check Bw To Ofp */
  Soc_petra_a_regs.egq.check_bw_to_ofp_reg.addr.base = sizeof(uint32) * 0x398b;
  Soc_petra_a_regs.egq.check_bw_to_ofp_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.check_bw_to_ofp_reg.ofp_to_check_bw), sizeof(uint32) * 0x398b, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.check_bw_to_ofp_reg.check_bw_to_ofp), sizeof(uint32) * 0x398b, sizeof(uint32) * 0x0000, 7, 7);

  /* Gtimer Config */
  Soc_petra_a_regs.egq.gtimer_config_reg.addr.base = sizeof(uint32) * 0x398c;
  Soc_petra_a_regs.egq.gtimer_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.gtimer_config_reg.timer_config), sizeof(uint32) * 0x398c, sizeof(uint32) * 0x0000, 30, 0);

  /* Gtimer Config Cont */
  Soc_petra_a_regs.egq.gtimer_config_cont_reg.addr.base = sizeof(uint32) * 0x398d;
  Soc_petra_a_regs.egq.gtimer_config_cont_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.gtimer_config_cont_reg.clear_gtimer), sizeof(uint32) * 0x398d, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.gtimer_config_cont_reg.activate_gtimer), sizeof(uint32) * 0x398d, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.gtimer_config_cont_reg.gtimer_active), sizeof(uint32) * 0x398d, sizeof(uint32) * 0x0000, 2, 2);

  /* Chicken Bit Register */
  Soc_petra_a_regs.egq.chicken_bit_reg.addr.base = sizeof(uint32) * 0x398e;
  Soc_petra_a_regs.egq.chicken_bit_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.chicken_bit_reg.seg_cnt_chicken_bit), sizeof(uint32) * 0x398e, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.chicken_bit_reg.oc768_chicken_bit), sizeof(uint32) * 0x398e, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.egq.chicken_bit_reg.oc768_chicken_bit1), sizeof(uint32) * 0x398e, sizeof(uint32) * 0x0000, 2, 2);

}

/* Block registers initialization: EPNI */
STATIC void
  soc_pa_regs_init_EPNI(void)
{
  uint32
    reg_idx = 0,
    fld_idx = 0;

  Soc_petra_a_regs.epni.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_EPN;
  Soc_petra_a_regs.epni.addr.base = sizeof(uint32) * 0x3a00;
  Soc_petra_a_regs.epni.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.epni.interrupt_reg.addr.base = sizeof(uint32) * 0x3a00;
  Soc_petra_a_regs.epni.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_reg.tdm_fabric_stop), sizeof(uint32) * 0x3a00, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_reg.mirr_ovf_int), sizeof(uint32) * 0x3a00, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_reg.int_otm_out_lif_mode), sizeof(uint32) * 0x3a00, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_reg.tdm_int_vec), sizeof(uint32) * 0x3a00, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_reg.ecc_err_vec), sizeof(uint32) * 0x3a00, sizeof(uint32) * 0x0000, 28, 28);

  /* Tdm Int Reg */
  Soc_petra_a_regs.epni.tdm_int_reg.addr.base = sizeof(uint32) * 0x3a01;
  Soc_petra_a_regs.epni.tdm_int_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdm_nifa_cnt_ovf_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdm_nifb_cnt_ovf_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdm_frag_err_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdm_one_fifo_mc_loss_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdma_loss_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdmb_loss_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdmc_loss_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg.tdmd_loss_int), sizeof(uint32) * 0x3a01, sizeof(uint32) * 0x0000, 7, 7);

  /* Ecc Int Reg */
  Soc_petra_a_regs.epni.ecc_int_reg.addr.base = sizeof(uint32) * 0x3a02;
  Soc_petra_a_regs.epni.ecc_int_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.prdm_ecc_err), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.prdm_ecc_fix), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.mdm_ecc_err), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.mdm_ecc_fix), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.nadm_ecc_err), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.nadm_ecc_fix), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.nbdm_ecc_err), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg.nbdm_ecc_fix), sizeof(uint32) * 0x3a02, sizeof(uint32) * 0x0000, 7, 7);

  /* Interrupt Mask Register */
  Soc_petra_a_regs.epni.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x3a10;
  Soc_petra_a_regs.epni.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_mask_reg.mask_tdm_fabric_stop), sizeof(uint32) * 0x3a10, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_mask_reg.mask_mirr_ovf), sizeof(uint32) * 0x3a10, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_mask_reg.mask_int_otm_out_lif_mode), sizeof(uint32) * 0x3a10, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_mask_reg.mask_tdm_int_vec), sizeof(uint32) * 0x3a10, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.interrupt_mask_reg.mask_ecc_err_vec), sizeof(uint32) * 0x3a10, sizeof(uint32) * 0x0000, 28, 28);

  /* Tdm Int Reg Mask */
  Soc_petra_a_regs.epni.tdm_int_reg_mask_reg.addr.base = sizeof(uint32) * 0x3a11;
  Soc_petra_a_regs.epni.tdm_int_reg_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_int_reg_mask_reg.tdm_int_reg_mask), sizeof(uint32) * 0x3a11, sizeof(uint32) * 0x0000, 7, 0);

  /* Ecc Int Reg Mask */
  Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.addr.base = sizeof(uint32) * 0x3a12;
  Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.prdm_ecc_err_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.prdm_ecc_fixr_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.mdm_ecc_err_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.mdm_ecc_fix_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.nadm_ecc_err_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.nadm_ecc_fix_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.nbdm_ecc_err_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_int_reg_mask_reg.nbdm_ecc_fix_mask), sizeof(uint32) * 0x3a12, sizeof(uint32) * 0x0000, 7, 7);
  
  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x3a20;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x3a20, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_1.addr.base = sizeof(uint32) * 0x3a21;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_1.indirect_command_wr_data), sizeof(uint32) * 0x3a21, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_2.addr.base = sizeof(uint32) * 0x3a22;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_2.indirect_command_wr_data), sizeof(uint32) * 0x3a22, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_3.addr.base = sizeof(uint32) * 0x3a23;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_3.indirect_command_wr_data), sizeof(uint32) * 0x3a23, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_4.addr.base = sizeof(uint32) * 0x3a24;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_4.indirect_command_wr_data), sizeof(uint32) * 0x3a24, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_5.addr.base = sizeof(uint32) * 0x3a25;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_5.indirect_command_wr_data), sizeof(uint32) * 0x3a25, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_6.addr.base = sizeof(uint32) * 0x3a26;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_6.indirect_command_wr_data), sizeof(uint32) * 0x3a26, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_7.addr.base = sizeof(uint32) * 0x3a27;
  Soc_petra_a_regs.epni.indirect_command_wr_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_wr_data_reg_7.indirect_command_wr_data), sizeof(uint32) * 0x3a27, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x3a30;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x3a30, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_1.addr.base = sizeof(uint32) * 0x3a31;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_1.indirect_command_rd_data), sizeof(uint32) * 0x3a31, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_2.addr.base = sizeof(uint32) * 0x3a32;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_2.indirect_command_rd_data), sizeof(uint32) * 0x3a32, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_3.addr.base = sizeof(uint32) * 0x3a33;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_3.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_3.indirect_command_rd_data), sizeof(uint32) * 0x3a33, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_4.addr.base = sizeof(uint32) * 0x3a34;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_4.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_4.indirect_command_rd_data), sizeof(uint32) * 0x3a34, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_5.addr.base = sizeof(uint32) * 0x3a35;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_5.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_5.indirect_command_rd_data), sizeof(uint32) * 0x3a35, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_6.addr.base = sizeof(uint32) * 0x3a36;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_6.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_6.indirect_command_rd_data), sizeof(uint32) * 0x3a36, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_7.addr.base = sizeof(uint32) * 0x3a37;
  Soc_petra_a_regs.epni.indirect_command_rd_data_reg_7.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_rd_data_reg_7.indirect_command_rd_data), sizeof(uint32) * 0x3a37, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.epni.indirect_command_reg.addr.base = sizeof(uint32) * 0x3a40;
  Soc_petra_a_regs.epni.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x3a40, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x3a40, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x3a40, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x3a40, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x3a40, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.epni.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x3a41;
  Soc_petra_a_regs.epni.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x3a41, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x3a41, sizeof(uint32) * 0x0000, 31, 31);

  /* PCMI registers */
  soc_petra_regs_block_pcmi_registers_init(
    0x3a60,
    0x0000,
    &(Soc_petra_a_regs.epni.pcmi_config_reg),
    &(Soc_petra_a_regs.epni.pcmi_results_reg)
  );

  /* Sequence Number Masking Disable */
  Soc_petra_a_regs.epni.sequence_number_masking_disable_reg.addr.base = sizeof(uint32) * 0x3b05;
  Soc_petra_a_regs.epni.sequence_number_masking_disable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.sequence_number_masking_disable_reg.seqnum_msk_dis), sizeof(uint32) * 0x3b05, sizeof(uint32) * 0x0000, 0, 0);

  /* Sequence Number Masking Msb Configuration */
  Soc_petra_a_regs.epni.sequence_number_masking_msb_configuration_reg.addr.base = sizeof(uint32) * 0x3b06;
  Soc_petra_a_regs.epni.sequence_number_masking_msb_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.sequence_number_masking_msb_configuration_reg.seqnum_msk_msb), sizeof(uint32) * 0x3b06, sizeof(uint32) * 0x0000, 1, 0);

  /* ECN */
  Soc_petra_a_regs.epni.ecn_reg.addr.base = sizeof(uint32) * 0x3b08;
  Soc_petra_a_regs.epni.ecn_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecn_reg.ecn_en), sizeof(uint32) * 0x3b08, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecn_reg.ecn), sizeof(uint32) * 0x3b08, sizeof(uint32) * 0x0000, 2, 1);

  /* Recycling Interface Priority Settings */
  Soc_petra_a_regs.epni.recycling_interface_priority_settings_reg.addr.base = sizeof(uint32) * 0x3b09;
  Soc_petra_a_regs.epni.recycling_interface_priority_settings_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.recycling_interface_priority_settings_reg.rcy_sp_or_rr), sizeof(uint32) * 0x3b09, sizeof(uint32) * 0x0000, 0, 0);

  /* Ecc Corecction Disable */
  Soc_petra_a_regs.epni.ecc_corecction_disable_reg.addr.base = sizeof(uint32) * 0x3b0a;
  Soc_petra_a_regs.epni.ecc_corecction_disable_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_corecction_disable_reg.prdm_dis_ecc), sizeof(uint32) * 0x3b0a, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_corecction_disable_reg.mdm_dis_ecc), sizeof(uint32) * 0x3b0a, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_corecction_disable_reg.nadm_dis_ecc), sizeof(uint32) * 0x3b0a, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.ecc_corecction_disable_reg.nbdm_dis_ecc), sizeof(uint32) * 0x3b0a, sizeof(uint32) * 0x0000, 3, 3);

  /* Nifa Ports Flow Control Threshold Configuration */
  for (fld_idx = 0; fld_idx < SOC_PETRA_NIF_PORTS_FC_THRESH_NOF_FLDS; fld_idx++)
  {
    Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_reg[0].addr.base = sizeof(uint32) * 0x3b0c;
    Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_reg[0].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_reg[0].nif_per_port_th[fld_idx]),
      sizeof(uint32) * 0x3b0c,
      sizeof(uint32) * 0x0000,
      (uint8)(3 + 4*fld_idx),
      (uint8)(0 + 4*fld_idx)
    );
  }
  /* Nifb Ports Flow Control Threshold Configuration */
  for (fld_idx = 0; fld_idx < SOC_PETRA_NIF_PORTS_FC_THRESH_NOF_FLDS; fld_idx++)
  {
    Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_reg[1].addr.base = sizeof(uint32) * 0x3b0e;
    Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_reg[1].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_reg[1].nif_per_port_th[fld_idx]),
      sizeof(uint32) * 0x3b0e,
      sizeof(uint32) * 0x0000,
      (uint8)(3 + 4*fld_idx),
      (uint8)(0 + 4*fld_idx)
    );
  }

  Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_const_reg[0].addr.base = sizeof(uint32) * 0x3b0d;
  Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_const_reg[0].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set(&(Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_const_reg[0].nif_per_port_const), sizeof(uint32) * 0x3b0d, sizeof(uint32) * 0x0000, 31, 0);

  Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_const_reg[1].addr.base = sizeof(uint32) * 0x3b0f;
  Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_const_reg[1].addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set(&(Soc_petra_a_regs.epni.nif_ports_fc_thresh_config_const_reg[1].nif_per_port_const), sizeof(uint32) * 0x3b0f, sizeof(uint32) * 0x0000, 31, 0);

  /* Egress Interface No Fragmentation Mode Configuration */
  Soc_petra_a_regs.epni.egress_interface_no_fragmentation_mode_configuration_reg.addr.base = sizeof(uint32) * 0x3b10;
  Soc_petra_a_regs.epni.egress_interface_no_fragmentation_mode_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.egress_interface_no_fragmentation_mode_configuration_reg.nifa_no_frag), sizeof(uint32) * 0x3b10, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.egress_interface_no_fragmentation_mode_configuration_reg.nifb_no_frag), sizeof(uint32) * 0x3b10, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.egress_interface_no_fragmentation_mode_configuration_reg.rcy_no_frag), sizeof(uint32) * 0x3b10, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.egress_interface_no_fragmentation_mode_configuration_reg.cpu_no_frag), sizeof(uint32) * 0x3b10, sizeof(uint32) * 0x0000, 9, 9);

  /* Map Ofp To Mirr Channel */
  for (reg_idx = 0; reg_idx < SOC_PETRA_MAP_OFP_TO_MIRR_CHANNEL_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.epni.map_ofp_to_mirr_channel_reg[reg_idx].addr.base = sizeof(uint32) * (0x3b11 + reg_idx);
    Soc_petra_a_regs.epni.map_ofp_to_mirr_channel_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.epni.map_ofp_to_mirr_channel_reg[reg_idx].map_ofp_to_mirr_cha),
      sizeof(uint32) * (0x3b11 + reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* Tdm Configurations */
  Soc_petra_a_regs.epni.tdm_configurations_reg.addr.base = sizeof(uint32) * 0x3b20;
  Soc_petra_a_regs.epni.tdm_configurations_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_configurations_reg.one_fifo_opr_mode), sizeof(uint32) * 0x3b20, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_configurations_reg.dis_frag_err_cells), sizeof(uint32) * 0x3b20, sizeof(uint32) * 0x0000, 1, 1);

  /* Tdm Destination Output Port Disable Settings */
  Soc_petra_a_regs.epni.tdm_destination_output_port_disable_settings_reg.addr.base = sizeof(uint32) * 0x3b21;
  Soc_petra_a_regs.epni.tdm_destination_output_port_disable_settings_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_destination_output_port_disable_settings_reg.tdma_out_dis), sizeof(uint32) * 0x3b21, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_destination_output_port_disable_settings_reg.tdmb_out_dis), sizeof(uint32) * 0x3b21, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_destination_output_port_disable_settings_reg.tdmc_out_dis), sizeof(uint32) * 0x3b21, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_destination_output_port_disable_settings_reg.tdmd_out_dis), sizeof(uint32) * 0x3b21, sizeof(uint32) * 0x0000, 3, 3);

  /* Tdm Nifa Cell Counter */
  Soc_petra_a_regs.epni.tdm_nifa_cell_counter_reg.addr.base = sizeof(uint32) * 0x3b22;
  Soc_petra_a_regs.epni.tdm_nifa_cell_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_nifa_cell_counter_reg.tdm_nifa_cnt), sizeof(uint32) * 0x3b22, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_nifa_cell_counter_reg.tdm_nifa_cnt_ovf), sizeof(uint32) * 0x3b22, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdm Nifb Cell Counter */
  Soc_petra_a_regs.epni.tdm_nifb_cell_counter_reg.addr.base = sizeof(uint32) * 0x3b23;
  Soc_petra_a_regs.epni.tdm_nifb_cell_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_nifb_cell_counter_reg.tdm_nifb_cnt), sizeof(uint32) * 0x3b23, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_nifb_cell_counter_reg.tdm_nifb_cnt_ovf), sizeof(uint32) * 0x3b23, sizeof(uint32) * 0x0000, 31, 31);

  /* Last Local Tdm Cell Header */
  Soc_petra_a_regs.epni.last_local_tdm_cell_header_reg.addr.base = sizeof(uint32) * 0x3b24;
  Soc_petra_a_regs.epni.last_local_tdm_cell_header_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.last_local_tdm_cell_header_reg.last_local_tdm_hdr), sizeof(uint32) * 0x3b24, sizeof(uint32) * 0x0000, 31, 0);

  /* Last Fabric Tdm Cell Header */
  Soc_petra_a_regs.epni.last_fabric_tdm_cell_header_reg.addr.base = sizeof(uint32) * 0x3b25;
  Soc_petra_a_regs.epni.last_fabric_tdm_cell_header_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.last_fabric_tdm_cell_header_reg.last_fabric_tdm_hdr), sizeof(uint32) * 0x3b25, sizeof(uint32) * 0x0000, 31, 0);

  /* Tdm One Fifo Mc Loss Cells Counter */
  Soc_petra_a_regs.epni.tdm_one_fifo_mc_loss_cells_counter_reg.addr.base = sizeof(uint32) * 0x3b26;
  Soc_petra_a_regs.epni.tdm_one_fifo_mc_loss_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_one_fifo_mc_loss_cells_counter_reg.tdm_one_fifo_mc_loss_cnt), sizeof(uint32) * 0x3b26, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_one_fifo_mc_loss_cells_counter_reg.tdm_one_fifo_mc_loss_cnt_ovf), sizeof(uint32) * 0x3b26, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdm Fragment Error Cells Counter */
  Soc_petra_a_regs.epni.tdm_fragment_error_cells_counter_reg.addr.base = sizeof(uint32) * 0x3b27;
  Soc_petra_a_regs.epni.tdm_fragment_error_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_fragment_error_cells_counter_reg.tdm_frag_err_cnt), sizeof(uint32) * 0x3b27, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdm_fragment_error_cells_counter_reg.tdm_frag_err_cnt_ovf), sizeof(uint32) * 0x3b27, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdma Loss Cells Counter */
  Soc_petra_a_regs.epni.tdma_loss_cells_counter_reg.addr.base = sizeof(uint32) * 0x3b28;
  Soc_petra_a_regs.epni.tdma_loss_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdma_loss_cells_counter_reg.tdma_loss_cnt), sizeof(uint32) * 0x3b28, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdma_loss_cells_counter_reg.tdma_loss_cnt_ovf), sizeof(uint32) * 0x3b28, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdmb Loss Cells Counter */
  Soc_petra_a_regs.epni.tdmb_loss_cells_counter_reg.addr.base = sizeof(uint32) * 0x3b29;
  Soc_petra_a_regs.epni.tdmb_loss_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdmb_loss_cells_counter_reg.tdmb_loss_cnt), sizeof(uint32) * 0x3b29, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdmb_loss_cells_counter_reg.tdmb_loss_cnt_ovf), sizeof(uint32) * 0x3b29, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdmc Loss Cells Counter */
  Soc_petra_a_regs.epni.tdmc_loss_cells_counter_reg.addr.base = sizeof(uint32) * 0x3b2a;
  Soc_petra_a_regs.epni.tdmc_loss_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdmc_loss_cells_counter_reg.tdmc_loss_cnt), sizeof(uint32) * 0x3b2a, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdmc_loss_cells_counter_reg.tdmc_loss_cnt_ovf), sizeof(uint32) * 0x3b2a, sizeof(uint32) * 0x0000, 31, 31);

  /* Tdmd Loss Cells Counter */
  Soc_petra_a_regs.epni.tdmd_loss_cells_counter_reg.addr.base = sizeof(uint32) * 0x3b2b;
  Soc_petra_a_regs.epni.tdmd_loss_cells_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdmd_loss_cells_counter_reg.tdmd_loss_cnt), sizeof(uint32) * 0x3b2b, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.tdmd_loss_cells_counter_reg.tdmd_loss_cnt_ovf), sizeof(uint32) * 0x3b2b, sizeof(uint32) * 0x0000, 31, 31);

  /* Check Bw To Packet Descriptor */
  Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.addr.base = sizeof(uint32) * 0x3b2c;
  Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.ofp_to_check_bw), sizeof(uint32) * 0x3b2c, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.fifo_to_check_bw), sizeof(uint32) * 0x3b2c, sizeof(uint32) * 0x0000, 9, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.nif_port_to_check_bw), sizeof(uint32) * 0x3b2c, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.nif_channel_to_check_bw), sizeof(uint32) * 0x3b2c, sizeof(uint32) * 0x0000, 21, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.check_bw_to_packet_descriptor_reg.outbound_mirror_to_check_bw), sizeof(uint32) * 0x3b2c, sizeof(uint32) * 0x0000, 24, 24);

  /* Mask Check Bw To Packet Descriptor */
  Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.addr.base = sizeof(uint32) * 0x3b2d;
  Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.mask_ofp_to_check_bw), sizeof(uint32) * 0x3b2d, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.mask_fifo_to_check_bw), sizeof(uint32) * 0x3b2d, sizeof(uint32) * 0x0000, 9, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.mask_nif_port_to_check_bw), sizeof(uint32) * 0x3b2d, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.mask_nif_channel_to_check_bw), sizeof(uint32) * 0x3b2d, sizeof(uint32) * 0x0000, 21, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.mask_check_bw_to_packet_descriptor_reg.mask_outbound_mirror_to_check_bw), sizeof(uint32) * 0x3b2d, sizeof(uint32) * 0x0000, 24, 24);

  /* Gtimer Config */
  Soc_petra_a_regs.epni.gtimer_config_reg.addr.base = sizeof(uint32) * 0x3b2e;
  Soc_petra_a_regs.epni.gtimer_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.gtimer_config_reg.timer_config), sizeof(uint32) * 0x3b2e, sizeof(uint32) * 0x0000, 30, 0);

  /* Gtimer Config Cont */
  Soc_petra_a_regs.epni.gtimer_config_cont_reg.addr.base = sizeof(uint32) * 0x3b2f;
  Soc_petra_a_regs.epni.gtimer_config_cont_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.gtimer_config_cont_reg.clear_gtimer), sizeof(uint32) * 0x3b2f, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.gtimer_config_cont_reg.activate_gtimer), sizeof(uint32) * 0x3b2f, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.gtimer_config_cont_reg.gtimer_active), sizeof(uint32) * 0x3b2f, sizeof(uint32) * 0x0000, 2, 2);

  /* Epe Packet Counter */
  Soc_petra_a_regs.epni.epe_packet_counter_reg.addr.base = sizeof(uint32) * 0x43a00;
  Soc_petra_a_regs.epni.epe_packet_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.epe_packet_counter_reg.epe_packet_counter), sizeof(uint32) * 0x43a00, sizeof(uint32) * 0x0000, 32, 0);

  /* Epe Bytes Counter */
  Soc_petra_a_regs.epni.epe_bytes_counter_reg_0.addr.base = sizeof(uint32) * 0x53a00;
  Soc_petra_a_regs.epni.epe_bytes_counter_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.epe_bytes_counter_reg_0.epe_bytes_counter), sizeof(uint32) * 0x53a00, sizeof(uint32) * 0x0000, 31, 0);

  /* Epe Bytes Counter */
  Soc_petra_a_regs.epni.epe_bytes_counter_reg_1.addr.base = sizeof(uint32) * 0x53a01;
  Soc_petra_a_regs.epni.epe_bytes_counter_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.epni.epe_bytes_counter_reg_1.epe_bytes_counter), sizeof(uint32) * 0x53a01, sizeof(uint32) * 0x0000, 14, 0);
}

/* Block registers initialization: CFC */
STATIC void
  soc_pa_regs_init_CFC(void)
{
  uint32
    fld_idx = 0,
    reg_idx = 0;

  Soc_petra_a_regs.cfc.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_CFC;
  Soc_petra_a_regs.cfc.addr.base = sizeof(uint32) * 0x4600;
  Soc_petra_a_regs.cfc.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.cfc.interrupt_reg.addr.base = sizeof(uint32) * 0x4600;
  Soc_petra_a_regs.cfc.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_BLK_NOF_INSTANCES_OOB; fld_idx++)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_reg.obrx_lock_err[fld_idx]),
                       sizeof(uint32) * 0x4600,
                       sizeof(uint32) * 0x0000,
                       (uint8)(0 + fld_idx),
                       (uint8)(0 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_reg.obrx_out_of_frm[fld_idx]),
                       sizeof(uint32) * 0x4600,
                       sizeof(uint32) * 0x0000,
                       (uint8)(2 + fld_idx),
                       (uint8)(2 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_reg.obrx_dip2_alarm[fld_idx]),
                       sizeof(uint32) * 0x4600,
                       sizeof(uint32) * 0x0000,
                       (uint8)(4 + fld_idx),
                       (uint8)(4 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_reg.obrx_frm_err[fld_idx]),
                       sizeof(uint32) * 0x4600,
                       sizeof(uint32) * 0x0000,
                       (uint8)(6 + fld_idx),
                       (uint8)(6 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_reg.obrx_dip2_err[fld_idx]),
                       sizeof(uint32) * 0x4600,
                       sizeof(uint32) * 0x0000,
                       (uint8)(8 + fld_idx),
                       (uint8)(8 + fld_idx));
  }


  /* Interrupt Mask Register */
  Soc_petra_a_regs.cfc.interrupt_mask_reg.addr.base = sizeof(uint32) * 0x4610;
  Soc_petra_a_regs.cfc.interrupt_mask_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_BLK_NOF_INSTANCES_OOB; fld_idx++)
  {
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_mask_reg.obrx_lock_err_mask[fld_idx]),
                       sizeof(uint32) * 0x4610,
                       sizeof(uint32) * 0x0000,
                       (uint8)(0 + fld_idx),
                       (uint8)(0 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_mask_reg.obrx_out_of_frm_mask[fld_idx]),
                       sizeof(uint32) * 0x4610,
                       sizeof(uint32) * 0x0000,
                       (uint8)(2 + fld_idx),
                       (uint8)(2 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_mask_reg.obrx_dip2_alarm_mask[fld_idx]),
                       sizeof(uint32) * 0x4610,
                       sizeof(uint32) * 0x0000,
                       (uint8)(4 + fld_idx),
                       (uint8)(4 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_mask_reg.obrx_frm_err_mask[fld_idx]),
                       sizeof(uint32) * 0x4610,
                       sizeof(uint32) * 0x0000,
                       (uint8)(6 + fld_idx),
                       (uint8)(6 + fld_idx));
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.interrupt_mask_reg.obrx_dip2_err_mask[fld_idx]),
                       sizeof(uint32) * 0x4610,
                       sizeof(uint32) * 0x0000,
                       (uint8)(8 + fld_idx),
                       (uint8)(8 + fld_idx));
  }

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.cfc.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x4620;
  Soc_petra_a_regs.cfc.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x4620, sizeof(uint32) * 0x0000, 13, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.cfc.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x4630;
  Soc_petra_a_regs.cfc.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x4630, sizeof(uint32) * 0x0000, 13, 0);

  /* Indirect Command */
  Soc_petra_a_regs.cfc.indirect_command_reg.addr.base = sizeof(uint32) * 0x4640;
  Soc_petra_a_regs.cfc.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x4640, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_reg.indirect_command_trigger_on_data), sizeof(uint32) * 0x4640, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_reg.indirect_command_count), sizeof(uint32) * 0x4640, sizeof(uint32) * 0x0000, 15, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_reg.indirect_command_timeout), sizeof(uint32) * 0x4640, sizeof(uint32) * 0x0000, 30, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_reg.indirect_command_status), sizeof(uint32) * 0x4640, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Address */
  Soc_petra_a_regs.cfc.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x4641;
  Soc_petra_a_regs.cfc.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x4641, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x4641, sizeof(uint32) * 0x0000, 31, 31);

  /* Cfc Enablers */
  Soc_petra_a_regs.cfc.cfc_enablers_reg.addr.base = sizeof(uint32) * 0x4700;
  Soc_petra_a_regs.cfc.cfc_enablers_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.cfc_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.nif_cls_hr_sch_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.rcl_ofp_hp_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.rcl_ofp_lp_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.rcl_vsq_hr_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.oob_sch_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.oob_egq_ofp_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.frc_llvl_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.egq_device_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.egq_erp_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 9, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.egq_ch_port_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.cpu_port_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.rcl_port_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.egq_ofp_lp_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.egq_ofp_hp_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.nif_clb_sel), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.oob_intrfb_sel), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.oob_nifa_fast_ll_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.oob_nifb_fast_ll_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.hgl_mode), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.cfc_enablers_reg.hp_glb_cls_ovrd_en), sizeof(uint32) * 0x4700, sizeof(uint32) * 0x0000, 20, 20);

  /* Low Priority Control */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_LOW_PRIORITY_CONTROL_REGS; reg_idx++)
  {
    Soc_petra_a_regs.cfc.low_priority_control_reg[reg_idx].addr.base = sizeof(uint32) * (0x4701 + reg_idx);
    Soc_petra_a_regs.cfc.low_priority_control_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.cfc.low_priority_control_reg[reg_idx].lp_glb_cls_map),
      sizeof(uint32) * (0x4701 + reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* Recycle Out Going Fap Port Hr Map */
  for (reg_idx = 0; reg_idx < SOC_PETRA_RCY2FAP_PORT_HR_MAP_REGS; reg_idx++)
  {
    Soc_petra_a_regs.cfc.recycle_out_going_fap_port_hr_map_reg[reg_idx].addr.base = sizeof(uint32) * (0x4705 - reg_idx);
    Soc_petra_a_regs.cfc.recycle_out_going_fap_port_hr_map_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.cfc.recycle_out_going_fap_port_hr_map_reg[reg_idx].rcl_ofp_hr_map),
      sizeof(uint32) * (0x4705 - reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* High Priority Hr Map */
  Soc_petra_a_regs.cfc.high_priority_hr_map_reg.addr.base = sizeof(uint32) * 0x4706;
  Soc_petra_a_regs.cfc.high_priority_hr_map_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.high_priority_hr_map_reg.rcl_hp_hr_map), sizeof(uint32) * 0x4706, sizeof(uint32) * 0x0000, 31, 0);

  /* Low Priority Hr Map */
  Soc_petra_a_regs.cfc.low_priority_hr_map_reg.addr.base = sizeof(uint32) * 0x4707;
  Soc_petra_a_regs.cfc.low_priority_hr_map_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.low_priority_hr_map_reg.rcl_lp_hr_map), sizeof(uint32) * 0x4707, sizeof(uint32) * 0x0000, 31, 0);

  /* Rcycle Out Going Fap Port Map */
  Soc_petra_a_regs.cfc.rcycle_out_going_fap_port_map_reg.addr.base = sizeof(uint32) * 0x4708;
  Soc_petra_a_regs.cfc.rcycle_out_going_fap_port_map_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.rcycle_out_going_fap_port_map_reg.rcl_egq_ofp_map), sizeof(uint32) * 0x4708, sizeof(uint32) * 0x0000, 31, 0);

  /* Class Based Low Priority Hr Map0 */
  Soc_petra_a_regs.cfc.class_based_low_priority_hr_map0_reg.addr.base = sizeof(uint32) * 0x4709;
  Soc_petra_a_regs.cfc.class_based_low_priority_hr_map0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.class_based_low_priority_hr_map0_reg.clb_lp_ofp_map0), sizeof(uint32) * 0x4709, sizeof(uint32) * 0x0000, 31, 0);

  /* Class Based Low Priority Hr Map1 */
  Soc_petra_a_regs.cfc.class_based_low_priority_hr_map1_reg.addr.base = sizeof(uint32) * 0x470a;
  Soc_petra_a_regs.cfc.class_based_low_priority_hr_map1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.class_based_low_priority_hr_map1_reg.clb_lp_ofp_map1), sizeof(uint32) * 0x470a, sizeof(uint32) * 0x0000, 31, 0);

  /* Class Based HIgh Priority Hr Map0 */
  Soc_petra_a_regs.cfc.class_based_high_priority_hr_map0_reg.addr.base = sizeof(uint32) * 0x470b;
  Soc_petra_a_regs.cfc.class_based_high_priority_hr_map0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.class_based_high_priority_hr_map0_reg.clb_hp_ofp_map0), sizeof(uint32) * 0x470b, sizeof(uint32) * 0x0000, 31, 0);

  /* Class Based HIgh Priority Hr Map1 */
  Soc_petra_a_regs.cfc.class_based_high_priority_hr_map1_reg.addr.base = sizeof(uint32) * 0x470c;
  Soc_petra_a_regs.cfc.class_based_high_priority_hr_map1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.class_based_high_priority_hr_map1_reg.clb_hp_ofp_map1), sizeof(uint32) * 0x470c, sizeof(uint32) * 0x0000, 31, 0);

  /* Class Based Out Going Fap Port Map0 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_LOW_PRIORITY_CONTROL_REGS; reg_idx++)
  {
    Soc_petra_a_regs.cfc.class_based_out_going_fap_port_map_reg[reg_idx].addr.base = sizeof(uint32) * (0x470d + reg_idx);
    Soc_petra_a_regs.cfc.class_based_out_going_fap_port_map_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.cfc.class_based_out_going_fap_port_map_reg[reg_idx].clb_egq_ofp_map),
      sizeof(uint32) * (0x470d + reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* Sch Hr Mask0-3 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_SCH_HR_MASK_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.cfc.sch_hr_mask_reg[reg_idx].addr.base = sizeof(uint32) * (0x470f + reg_idx);
    Soc_petra_a_regs.cfc.sch_hr_mask_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.cfc.sch_hr_mask_reg[reg_idx].sch_hr_msk),
      sizeof(uint32) * (0x470f + reg_idx),
      sizeof(uint32) * 0x0000,
      31,
      0
    );
  }

  /* Out Of Band Tx Configuration0 */
  Soc_petra_a_regs.cfc.out_of_band_tx_configuration0_reg.addr.base = sizeof(uint32) * 0x4713;
  Soc_petra_a_regs.cfc.out_of_band_tx_configuration0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_tx_configuration0_reg.tx_cal_len), sizeof(uint32) * 0x4713, sizeof(uint32) * 0x0000, 9, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_tx_configuration0_reg.tx_cal_m), sizeof(uint32) * 0x4713, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_tx_configuration0_reg.oob_tx_en), sizeof(uint32) * 0x4713, sizeof(uint32) * 0x0000, 16, 16);

  /* Out Of Band Tx Configuration1 */
  Soc_petra_a_regs.cfc.out_of_band_tx_configuration1_reg.addr.base = sizeof(uint32) * 0x4714;
  Soc_petra_a_regs.cfc.out_of_band_tx_configuration1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_tx_configuration1_reg.out_stat_phase_sel), sizeof(uint32) * 0x4714, sizeof(uint32) * 0x0000, 1, 0);

  /* Out Of Band Rx0 Configuration0 */
  Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.addr.base = sizeof(uint32) * 0x4715;
  Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.addr.step = sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.rx_cal_len), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 9, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.rx_cal_m), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.oob_rx_en), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.oob_rx_lb_en), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.oofrm_sts_sel), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.in_stat_phase_sel), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_configuration0_reg.rx_async_rstn), sizeof(uint32) * 0x4715, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 24, 24);

  /* Out Of Band Rx0 Threshold Configuration0 */
  Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration0_reg.addr.base = sizeof(uint32) * 0x4716;
  Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration0_reg.addr.step = sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration0_reg.dip2_good2_bad_ratio), sizeof(uint32) * 0x4716, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration0_reg.max_frm_pattrn), sizeof(uint32) * 0x4716, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 15, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration0_reg.in_frm_th), sizeof(uint32) * 0x4716, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 23, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration0_reg.out_of_frm_th), sizeof(uint32) * 0x4716, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 31, 24);

  /* Out Of Band Rx0 Threshold Configuration1 */
  Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration1_reg.addr.base = sizeof(uint32) * 0x4717;
  Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration1_reg.addr.step = sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_threshold_configuration1_reg.dip2_alrm_th), sizeof(uint32) * 0x4717, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_CONF_REGS, 7, 0);

  /* Out Of Band Rx0 Error Counter */
  Soc_petra_a_regs.cfc.out_of_band_rx_error_counter_reg.addr.base = sizeof(uint32) * 0x471b;
  Soc_petra_a_regs.cfc.out_of_band_rx_error_counter_reg.addr.step = sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_ERROR_REGS;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_error_counter_reg.frm_err_cnt), sizeof(uint32) * 0x471b, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_ERROR_REGS, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_error_counter_reg.frm_err_cnt_ovf), sizeof(uint32) * 0x471b, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_ERROR_REGS, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_error_counter_reg.dip2_err_cnt), sizeof(uint32) * 0x471b, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_ERROR_REGS, 14, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.out_of_band_rx_error_counter_reg.dip2_err_cnt_ovf), sizeof(uint32) * 0x471b, sizeof(uint32) * SOC_PETRA_BLK_NOF_OOB_ERROR_REGS, 15, 15);

  /* Force Scheduler Flow Control0 */
  Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.addr.base = sizeof(uint32) * 0x4750;
  Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.frc_sch_dev_fc), sizeof(uint32) * 0x4750, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.frc_sch_erp_fc), sizeof(uint32) * 0x4750, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.frc_sch_chn_port_fc), sizeof(uint32) * 0x4750, sizeof(uint32) * 0x0000, 9, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.frc_sch_cpu_port_fc), sizeof(uint32) * 0x4750, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control0_reg.frc_sch_rcl_port_fc), sizeof(uint32) * 0x4750, sizeof(uint32) * 0x0000, 11, 11);

  /* Force Scheduler Flow Control1 */
  Soc_petra_a_regs.cfc.force_scheduler_flow_control1_reg.addr.base = sizeof(uint32) * 0x4751;
  Soc_petra_a_regs.cfc.force_scheduler_flow_control1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control1_reg.frc_sch_hr_fc159_128), sizeof(uint32) * 0x4751, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Scheduler Flow Control2 */
  Soc_petra_a_regs.cfc.force_scheduler_flow_control2_reg.addr.base = sizeof(uint32) * 0x4752;
  Soc_petra_a_regs.cfc.force_scheduler_flow_control2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control2_reg.frc_sch_hr_fc191_160), sizeof(uint32) * 0x4752, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Scheduler Flow Control3 */
  Soc_petra_a_regs.cfc.force_scheduler_flow_control3_reg.addr.base = sizeof(uint32) * 0x4753;
  Soc_petra_a_regs.cfc.force_scheduler_flow_control3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control3_reg.frc_sch_hr_fc223_192), sizeof(uint32) * 0x4753, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Scheduler Flow Control4 */
  Soc_petra_a_regs.cfc.force_scheduler_flow_control4_reg.addr.base = sizeof(uint32) * 0x4754;
  Soc_petra_a_regs.cfc.force_scheduler_flow_control4_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_scheduler_flow_control4_reg.frc_sch_hr_fc255_224), sizeof(uint32) * 0x4754, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Egq Flow Control0 */
  Soc_petra_a_regs.cfc.force_egq_flow_control0_reg.addr.base = sizeof(uint32) * 0x4755;
  Soc_petra_a_regs.cfc.force_egq_flow_control0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_egq_flow_control0_reg.frc_egq_ofp_fc31_0), sizeof(uint32) * 0x4755, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Egq Flow Control1 */
  Soc_petra_a_regs.cfc.force_egq_flow_control1_reg.addr.base = sizeof(uint32) * 0x4756;
  Soc_petra_a_regs.cfc.force_egq_flow_control1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_egq_flow_control1_reg.frc_egq_ofp_fc63_32), sizeof(uint32) * 0x4756, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Egq Flow Control2 */
  Soc_petra_a_regs.cfc.force_egq_flow_control2_reg.addr.base = sizeof(uint32) * 0x4757;
  Soc_petra_a_regs.cfc.force_egq_flow_control2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_egq_flow_control2_reg.frc_egq_ofp_fc79_64), sizeof(uint32) * 0x4757, sizeof(uint32) * 0x0000, 15, 0);

  /* Force Nif Link Level Flow Control */
  Soc_petra_a_regs.cfc.force_nif_link_level_flow_control_reg.addr.base = sizeof(uint32) * 0x4758;
  Soc_petra_a_regs.cfc.force_nif_link_level_flow_control_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_nif_link_level_flow_control_reg.frc_nif_lnk_fc0), sizeof(uint32) * 0x4758, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_nif_link_level_flow_control_reg.frc_nif_lnk_fc1), sizeof(uint32) * 0x4758, sizeof(uint32) * 0x0000, 31, 16);

  /* Force Nif Class Based Flow Control0 */
  Soc_petra_a_regs.cfc.force_nif_class_based_flow_control0_reg.addr.base = sizeof(uint32) * 0x4759;
  Soc_petra_a_regs.cfc.force_nif_class_based_flow_control0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_nif_class_based_flow_control0_reg.frc_nif_clb0), sizeof(uint32) * 0x4759, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Nif Class Based Flow Control1 */
  Soc_petra_a_regs.cfc.force_nif_class_based_flow_control1_reg.addr.base = sizeof(uint32) * 0x475a;
  Soc_petra_a_regs.cfc.force_nif_class_based_flow_control1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_nif_class_based_flow_control1_reg.frc_nif_clb1), sizeof(uint32) * 0x475a, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Nif Link Level Flow Control1 */
  Soc_petra_a_regs.cfc.force_nif_link_level_flow_control1_reg.addr.base = sizeof(uint32) * 0x475b;
  Soc_petra_a_regs.cfc.force_nif_link_level_flow_control1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_nif_link_level_flow_control1_reg.frc_nifa_fast_ll), sizeof(uint32) * 0x475b, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.force_nif_link_level_flow_control1_reg.frc_nifb_fast_ll), sizeof(uint32) * 0x475b, sizeof(uint32) * 0x0000, 31, 16);

  /* Egq Flow Control Status0 */
  Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.addr.base = sizeof(uint32) * 0x475c;
  Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.egq_dev_fc), sizeof(uint32) * 0x475c, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.egq_erp_fc), sizeof(uint32) * 0x475c, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.egq_chn_port_fc), sizeof(uint32) * 0x475c, sizeof(uint32) * 0x0000, 9, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.egq_cpu_port_fc), sizeof(uint32) * 0x475c, sizeof(uint32) * 0x0000, 10, 10);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status0_reg.egq_rcl_port_fc), sizeof(uint32) * 0x475c, sizeof(uint32) * 0x0000, 11, 11);

  /* Egq Flow Control Status1 */
  Soc_petra_a_regs.cfc.egq_flow_control_status1_reg.addr.base = sizeof(uint32) * 0x475d;
  Soc_petra_a_regs.cfc.egq_flow_control_status1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status1_reg.egq_ofp_hp_fc31_0), sizeof(uint32) * 0x475d, sizeof(uint32) * 0x0000, 31, 0);

  /* Egq Flow Control Status2 */
  Soc_petra_a_regs.cfc.egq_flow_control_status2_reg.addr.base = sizeof(uint32) * 0x475e;
  Soc_petra_a_regs.cfc.egq_flow_control_status2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status2_reg.egq_ofp_hp_fc63_32), sizeof(uint32) * 0x475e, sizeof(uint32) * 0x0000, 31, 0);

  /* Egq Flow Control Status3 */
  Soc_petra_a_regs.cfc.egq_flow_control_status3_reg.addr.base = sizeof(uint32) * 0x475f;
  Soc_petra_a_regs.cfc.egq_flow_control_status3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.egq_flow_control_status3_reg.egq_ofp_hp_fc79_64), sizeof(uint32) * 0x475f, sizeof(uint32) * 0x0000, 15, 0);

  /* Nif Flow Control Status0 */
  Soc_petra_a_regs.cfc.nif_flow_control_status0_reg.addr.base = sizeof(uint32) * 0x4760;
  Soc_petra_a_regs.cfc.nif_flow_control_status0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.nif_flow_control_status0_reg.nif_ing_fifo_fc0), sizeof(uint32) * 0x4760, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.nif_flow_control_status0_reg.nif_ing_fifo_fc1), sizeof(uint32) * 0x4760, sizeof(uint32) * 0x0000, 31, 16);

  /* Nif Flow Control Status1 */
  Soc_petra_a_regs.cfc.nif_flow_control_status1_reg.addr.base = sizeof(uint32) * 0x4761;
  Soc_petra_a_regs.cfc.nif_flow_control_status1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.nif_flow_control_status1_reg.nif_clb_fc0), sizeof(uint32) * 0x4761, sizeof(uint32) * 0x0000, 31, 0);

  /* Nif Flow Control Status2 */
  Soc_petra_a_regs.cfc.nif_flow_control_status2_reg.addr.base = sizeof(uint32) * 0x4762;
  Soc_petra_a_regs.cfc.nif_flow_control_status2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.nif_flow_control_status2_reg.nif_clb_fc1), sizeof(uint32) * 0x4762, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status0 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status0_reg.addr.base = sizeof(uint32) * 0x4763;
  Soc_petra_a_regs.cfc.obrx_flow_control_status0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status0_reg.obrx_schb_fc31_0), sizeof(uint32) * 0x4763, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status1 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status1_reg.addr.base = sizeof(uint32) * 0x4764;
  Soc_petra_a_regs.cfc.obrx_flow_control_status1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status1_reg.obrx_schb_fc63_32), sizeof(uint32) * 0x4764, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status2 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status2_reg.addr.base = sizeof(uint32) * 0x4765;
  Soc_petra_a_regs.cfc.obrx_flow_control_status2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status2_reg.obrx_schb_fc95_64), sizeof(uint32) * 0x4765, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status3 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status3_reg.addr.base = sizeof(uint32) * 0x4766;
  Soc_petra_a_regs.cfc.obrx_flow_control_status3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status3_reg.obrx_schb_fc127_96), sizeof(uint32) * 0x4766, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status4 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status4_reg.addr.base = sizeof(uint32) * 0x4767;
  Soc_petra_a_regs.cfc.obrx_flow_control_status4_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status4_reg.obrx_ofp_fc31_0), sizeof(uint32) * 0x4767, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status5 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status5_reg.addr.base = sizeof(uint32) * 0x4768;
  Soc_petra_a_regs.cfc.obrx_flow_control_status5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status5_reg.obrx_ofp_fc63_32), sizeof(uint32) * 0x4768, sizeof(uint32) * 0x0000, 31, 0);

  /* Obrx Flow Control Status6 */
  Soc_petra_a_regs.cfc.obrx_flow_control_status6_reg.addr.base = sizeof(uint32) * 0x4769;
  Soc_petra_a_regs.cfc.obrx_flow_control_status6_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.obrx_flow_control_status6_reg.obrx_ofp_fc79_64), sizeof(uint32) * 0x4769, sizeof(uint32) * 0x0000, 15, 0);

  /* Vsqa Glb Flow Control Status */
  Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.addr.base = sizeof(uint32) * 0x476a;
  Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.vsqa_fc), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.fr_bdb_fc_lp), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.fr_bdb_fc_hp), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.fr_uni_db_fc_lp), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.fr_uni_db_fc_hp), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.fr_fl_mc_db_fc_lp), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqa_glb_flow_control_status_reg.fr_fl_mc_db_fc_hp), sizeof(uint32) * 0x476a, sizeof(uint32) * 0x0000, 9, 9);

  /* Vsqb Flow Control Status */
  Soc_petra_a_regs.cfc.vsqb_flow_control_status_reg.addr.base = sizeof(uint32) * 0x476b;
  Soc_petra_a_regs.cfc.vsqb_flow_control_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqb_flow_control_status_reg.vsqb_fc), sizeof(uint32) * 0x476b, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqc Flow Control Status0 */
  Soc_petra_a_regs.cfc.vsqc_flow_control_status0_reg.addr.base = sizeof(uint32) * 0x476c;
  Soc_petra_a_regs.cfc.vsqc_flow_control_status0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqc_flow_control_status0_reg.vsqc_fc0), sizeof(uint32) * 0x476c, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqc Flow Control Status1 */
  Soc_petra_a_regs.cfc.vsqc_flow_control_status1_reg.addr.base = sizeof(uint32) * 0x476d;
  Soc_petra_a_regs.cfc.vsqc_flow_control_status1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqc_flow_control_status1_reg.vsqc_fc1), sizeof(uint32) * 0x476d, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control0 */
  Soc_petra_a_regs.cfc.vsqd_flow_control0_reg.addr.base = sizeof(uint32) * 0x476e;
  Soc_petra_a_regs.cfc.vsqd_flow_control0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control0_reg.vsqd_fc0), sizeof(uint32) * 0x476e, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control1 */
  Soc_petra_a_regs.cfc.vsqd_flow_control1_reg.addr.base = sizeof(uint32) * 0x476f;
  Soc_petra_a_regs.cfc.vsqd_flow_control1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control1_reg.vsqd_fc1), sizeof(uint32) * 0x476f, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control2 */
  Soc_petra_a_regs.cfc.vsqd_flow_control2_reg.addr.base = sizeof(uint32) * 0x4770;
  Soc_petra_a_regs.cfc.vsqd_flow_control2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control2_reg.vsqd_fc2), sizeof(uint32) * 0x4770, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control3 */
  Soc_petra_a_regs.cfc.vsqd_flow_control3_reg.addr.base = sizeof(uint32) * 0x4771;
  Soc_petra_a_regs.cfc.vsqd_flow_control3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control3_reg.vsqd_fc3), sizeof(uint32) * 0x4771, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control4 */
  Soc_petra_a_regs.cfc.vsqd_flow_control4_reg.addr.base = sizeof(uint32) * 0x4772;
  Soc_petra_a_regs.cfc.vsqd_flow_control4_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control4_reg.vsqd_fc4), sizeof(uint32) * 0x4772, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control5 */
  Soc_petra_a_regs.cfc.vsqd_flow_control5_reg.addr.base = sizeof(uint32) * 0x4773;
  Soc_petra_a_regs.cfc.vsqd_flow_control5_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control5_reg.vsqd_fc5), sizeof(uint32) * 0x4773, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control6 */
  Soc_petra_a_regs.cfc.vsqd_flow_control6_reg.addr.base = sizeof(uint32) * 0x4774;
  Soc_petra_a_regs.cfc.vsqd_flow_control6_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control6_reg.vsqd_fc6), sizeof(uint32) * 0x4774, sizeof(uint32) * 0x0000, 31, 0);

  /* Vsqd Flow Control7 */
  Soc_petra_a_regs.cfc.vsqd_flow_control7_reg.addr.base = sizeof(uint32) * 0x4775;
  Soc_petra_a_regs.cfc.vsqd_flow_control7_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.cfc.vsqd_flow_control7_reg.vsqd_fc7), sizeof(uint32) * 0x4775, sizeof(uint32) * 0x0000, 31, 0);
}

/* Block registers initialization: SCH */
STATIC void
  soc_pa_regs_init_SCH(void)
{
  uint32
    reg_idx = 0,
    fld_idx = 0,
    gap = 0;
  Soc_petra_a_regs.sch.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_SCH;
  Soc_petra_a_regs.sch.addr.base = sizeof(uint32) * 0x4200;
  Soc_petra_a_regs.sch.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  Soc_petra_a_regs.sch.interrupt_reg.addr.base = sizeof(uint32) * 0x4200;
  Soc_petra_a_regs.sch.interrupt_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.smp_bad_msg), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.act_flow_bad_params), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.shp_flow_bad_params), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.restart_flow_event), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.smpthrow_scl_msg), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.smpfull_level1), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.smpfull_level2), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fctfifoovf), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.dhdecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.dcdecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.flhhrecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.flthrecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.flhclecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fltclecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.flhfqecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fltfqecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fdmsecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fdmdecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.shdsecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.shddecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fqmecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.sflhecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.sfltecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_reg.fsmecc), sizeof(uint32) * 0x4200, sizeof(uint32) * 0x0000, 31, 31);

  /* Interrupt Mask Data */
  Soc_petra_a_regs.sch.interrupt_mask_data_reg.addr.base = sizeof(uint32) * 0x4210;
  Soc_petra_a_regs.sch.interrupt_mask_data_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.smp_bad_msg_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.act_flow_bad_params_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.shp_flow_bad_params_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.restart_flow_event_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.smpthrow_scl_msg_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 5, 5);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.smpfull_level1_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.smpfull_level2_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fctfifoovf_mask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 14, 14);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.dhdeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.dcdeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.flhhreccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.flthreccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.flhcleccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fltcleccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.flhfqeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fltfqeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fdmseccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fdmdeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.shdseccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 26, 26);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.shddeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 27, 27);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fqmeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 28, 28);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.sflheccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 29, 29);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.sflteccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 30, 30);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.interrupt_mask_data_reg.fsmeccmask), sizeof(uint32) * 0x4210, sizeof(uint32) * 0x0000, 31, 31);

  /* Indirect Command Wr Data */
  Soc_petra_a_regs.sch.indirect_command_wr_data_reg_0.addr.base = sizeof(uint32) * 0x4220;
  Soc_petra_a_regs.sch.indirect_command_wr_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.indirect_command_wr_data_reg_0.indirect_command_wr_data), sizeof(uint32) * 0x4220, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command Rd Data */
  Soc_petra_a_regs.sch.indirect_command_rd_data_reg_0.addr.base = sizeof(uint32) * 0x4230;
  Soc_petra_a_regs.sch.indirect_command_rd_data_reg_0.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.indirect_command_rd_data_reg_0.indirect_command_rd_data), sizeof(uint32) * 0x4230, sizeof(uint32) * 0x0000, 31, 0);

  /* Indirect Command */
  Soc_petra_a_regs.sch.indirect_command_reg.addr.base = sizeof(uint32) * 0x4240;
  Soc_petra_a_regs.sch.indirect_command_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.indirect_command_reg.indirect_command_trigger), sizeof(uint32) * 0x4240, sizeof(uint32) * 0x0000, 0, 0);

  /* Indirect Command Address */
  Soc_petra_a_regs.sch.indirect_command_address_reg.addr.base = sizeof(uint32) * 0x4241;
  Soc_petra_a_regs.sch.indirect_command_address_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.indirect_command_address_reg.indirect_command_addr), sizeof(uint32) * 0x4241, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.indirect_command_address_reg.indirect_command_type), sizeof(uint32) * 0x4241, sizeof(uint32) * 0x0000, 31, 31);

  /* Credit Counter */
  Soc_petra_a_regs.sch.credit_counter_reg.addr.base = sizeof(uint32) * 0x4260;
  Soc_petra_a_regs.sch.credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_reg.credit_cnt), sizeof(uint32) * 0x4260, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_reg.credit_ovf), sizeof(uint32) * 0x4260, sizeof(uint32) * 0x0000, 31, 31);

  /* Scheduler Counter */
  Soc_petra_a_regs.sch.scheduler_counter_reg.addr.base = sizeof(uint32) * 0x4261;
  Soc_petra_a_regs.sch.scheduler_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scheduler_counter_reg.cmlschcredit_cnt), sizeof(uint32) * 0x4261, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scheduler_counter_reg.cmlschcredit_ovf), sizeof(uint32) * 0x4261, sizeof(uint32) * 0x0000, 31, 31);

  /* SMP Message Counter */
  Soc_petra_a_regs.sch.smp_message_counter_reg.addr.base = sizeof(uint32) * 0x4266;
  Soc_petra_a_regs.sch.smp_message_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_message_counter_reg.smpmsg_cnt), sizeof(uint32) * 0x4266, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_message_counter_reg.smpmsg_cnt_ovf), sizeof(uint32) * 0x4266, sizeof(uint32) * 0x0000, 31, 31);

  /* Attempt to Activate Flow / Scheduler with Bad Parameters */
  Soc_petra_a_regs.sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr.base = sizeof(uint32) * 0x4268;
  Soc_petra_a_regs.sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_id), sizeof(uint32) * 0x4268, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_cosn_valid), sizeof(uint32) * 0x4268, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_bad_sch), sizeof(uint32) * 0x4268, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_bad_params), sizeof(uint32) * 0x4268, sizeof(uint32) * 0x0000, 31, 31);

  /* Attempt to Activate A shaper with Bad Parameters */
  Soc_petra_a_regs.sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.addr.base = sizeof(uint32) * 0x4269;
  Soc_petra_a_regs.sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.shp_flow_id), sizeof(uint32) * 0x4269, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.shp_flow_bad_params), sizeof(uint32) * 0x4269, sizeof(uint32) * 0x0000, 31, 31);

  /* Last Flow Restart Event */
  Soc_petra_a_regs.sch.last_flow_restart_event_reg.addr.base = sizeof(uint32) * 0x426a;
  Soc_petra_a_regs.sch.last_flow_restart_event_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.last_flow_restart_event_reg.restart_flow_id), sizeof(uint32) * 0x426a, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.last_flow_restart_event_reg.restart_flow_event), sizeof(uint32) * 0x426a, sizeof(uint32) * 0x0000, 31, 31);

  /* SCL - SMP Messages */
  Soc_petra_a_regs.sch.scl_smp_messages_reg.addr.base = sizeof(uint32) * 0x426c;
  Soc_petra_a_regs.sch.scl_smp_messages_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scl_smp_messages_reg.smpsclmsg_id), sizeof(uint32) * 0x426c, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scl_smp_messages_reg.smpsclmsg_status), sizeof(uint32) * 0x426c, sizeof(uint32) * 0x0000, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scl_smp_messages_reg.smpsclmsg_type), sizeof(uint32) * 0x426c, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scl_smp_messages_reg.smpsclmsg_throw), sizeof(uint32) * 0x426c, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scl_smp_messages_reg.smpsclmsg), sizeof(uint32) * 0x426c, sizeof(uint32) * 0x0000, 31, 31);

  /* DVS Credit Counter */
  Soc_petra_a_regs.sch.dvs_credit_counter_reg.addr.base = sizeof(uint32) * 0x4270;
  Soc_petra_a_regs.sch.dvs_credit_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_credit_counter_reg.dvscreditcnt), sizeof(uint32) * 0x4270, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_credit_counter_reg.dvscreditcntovf), sizeof(uint32) * 0x4270, sizeof(uint32) * 0x0000, 31, 31);

  /* DVS Flow Control Counter */
  Soc_petra_a_regs.sch.dvs_flow_control_counter_reg.addr.base = sizeof(uint32) * 0x4272;
  Soc_petra_a_regs.sch.dvs_flow_control_counter_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_flow_control_counter_reg.dvsfccnt), sizeof(uint32) * 0x4272, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_flow_control_counter_reg.dvsfccnt_fin), sizeof(uint32) * 0x4272, sizeof(uint32) * 0x0000, 31, 31);

  /* DVS Link Status */
  Soc_petra_a_regs.sch.dvs_link_status_reg.addr.base = sizeof(uint32) * 0x4273;
  Soc_petra_a_regs.sch.dvs_link_status_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_link_status_reg.dvslink_cnt), sizeof(uint32) * 0x4273, sizeof(uint32) * 0x0000, 5, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_link_status_reg.dvsrci_level), sizeof(uint32) * 0x4273, sizeof(uint32) * 0x0000, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_link_status_reg.dvsmax_rci_level), sizeof(uint32) * 0x4273, sizeof(uint32) * 0x0000, 14, 12);

  /* Scheduler Configuration Register  */
  Soc_petra_a_regs.sch.scheduler_configuration_reg.addr.base = sizeof(uint32) * 0x4301;
  Soc_petra_a_regs.sch.scheduler_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scheduler_configuration_reg.sub_flow_enable), sizeof(uint32) * 0x4301, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scheduler_configuration_reg.smpdisable_fabric), sizeof(uint32) * 0x4301, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scheduler_configuration_reg.discard_credits), sizeof(uint32) * 0x4301, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.scheduler_configuration_reg.virtual_flow_enable), sizeof(uint32) * 0x4301, sizeof(uint32) * 0x0000, 31, 31);

  /* Shaper Configuration Register 1. */
  Soc_petra_a_regs.sch.shaper_configuration_1_reg.addr.base = sizeof(uint32) * 0x4302;
  Soc_petra_a_regs.sch.shaper_configuration_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.shaper_configuration_1_reg.shaper_slow_rate1), sizeof(uint32) * 0x4302, sizeof(uint32) * 0x0000, 17, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.shaper_configuration_1_reg.shaper_slow_rate2), sizeof(uint32) * 0x4302, sizeof(uint32) * 0x0000, 29, 20);

  /* Credit Counter Configuration Register 1 */
  Soc_petra_a_regs.sch.credit_counter_configuration_1_reg.addr.base = sizeof(uint32) * 0x4304;
  Soc_petra_a_regs.sch.credit_counter_configuration_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_1_reg.filter_flow), sizeof(uint32) * 0x4304, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_1_reg.filter_flow_mask), sizeof(uint32) * 0x4304, sizeof(uint32) * 0x0000, 31, 16);

  /* Credit Counter Configuration Register 2 */
  Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.addr.base = sizeof(uint32) * 0x4305;
  Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.filter_dest_fap), sizeof(uint32) * 0x4305, sizeof(uint32) * 0x0000, 10, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.filter_by_dest_fap), sizeof(uint32) * 0x4305, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.filter_by_flow), sizeof(uint32) * 0x4305, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.filter_by_sub_flow), sizeof(uint32) * 0x4305, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_counter_configuration_2_reg.cnt_by_gtimer), sizeof(uint32) * 0x4305, sizeof(uint32) * 0x0000, 31, 31);

  /* Credit Scheduler Counter in the CML Configuration */
  Soc_petra_a_regs.sch.credit_scheduler_counter_in_the_cml_configuration_reg.addr.base = sizeof(uint32) * 0x4306;
  Soc_petra_a_regs.sch.credit_scheduler_counter_in_the_cml_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_scheduler_counter_in_the_cml_configuration_reg.filter_sch), sizeof(uint32) * 0x4306, sizeof(uint32) * 0x0000, 13, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.credit_scheduler_counter_in_the_cml_configuration_reg.filter_sch_mask), sizeof(uint32) * 0x4306, sizeof(uint32) * 0x0000, 29, 16);

  /* SMP Message Counter and Status Configuration */
  Soc_petra_a_regs.sch.smp_message_counter_and_status_configuration_reg.addr.base = sizeof(uint32) * 0x4307;
  Soc_petra_a_regs.sch.smp_message_counter_and_status_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_message_counter_and_status_configuration_reg.smpcnt_type), sizeof(uint32) * 0x4307, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_message_counter_and_status_configuration_reg.smpfilter_by_type), sizeof(uint32) * 0x4307, sizeof(uint32) * 0x0000, 4, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_message_counter_and_status_configuration_reg.smpcnt_flow_port), sizeof(uint32) * 0x4307, sizeof(uint32) * 0x0000, 23, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_message_counter_and_status_configuration_reg.smpfilter_by_flow_port), sizeof(uint32) * 0x4307, sizeof(uint32) * 0x0000, 25, 24);

  /* Global Timer Configuration Register */
  Soc_petra_a_regs.sch.global_timer_configuration_reg.addr.base = sizeof(uint32) * 0x4308;
  Soc_petra_a_regs.sch.global_timer_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.global_timer_configuration_reg.gtimer_cnt), sizeof(uint32) * 0x4308, sizeof(uint32) * 0x0000, 30, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.global_timer_configuration_reg.gtimer_clr_cnt), sizeof(uint32) * 0x4308, sizeof(uint32) * 0x0000, 31, 31);


  /* Global Timer Activation Register */
  Soc_petra_a_regs.sch.global_timer_activation_reg.addr.base = sizeof(uint32) * 0x4309;
  Soc_petra_a_regs.sch.global_timer_activation_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.global_timer_activation_reg.gtimer_act), sizeof(uint32) * 0x4309, sizeof(uint32) * 0x0000, 0, 0);

  /* Counters-config */
  Soc_petra_a_regs.sch.credit_cnt_cfg_agg_reg.addr.base = sizeof(uint32) * 0x430a;
  Soc_petra_a_regs.sch.credit_cnt_cfg_agg_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( & Soc_petra_a_regs.sch.credit_cnt_cfg_agg_reg.scl_cr_cnt_by_flow, sizeof(uint32) * 0x430a, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( & Soc_petra_a_regs.sch.credit_cnt_cfg_agg_reg.scl_cr_cnt_flow, sizeof(uint32) * 0x430a, sizeof(uint32) * 0x0000, 31, 16);

  /* DLM */
  Soc_petra_a_regs.sch.dlm_reg.addr.base = sizeof(uint32) * 0x430b;
  Soc_petra_a_regs.sch.dlm_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( & Soc_petra_a_regs.sch.dlm_reg.dlm_ena, sizeof(uint32) * 0x430b, sizeof(uint32) * 0x0000, 2, 0);
  soc_petra_reg_fld_set( & Soc_petra_a_regs.sch.dlm_reg.dlm_bw, sizeof(uint32) * 0x430b, sizeof(uint32) * 0x0000, 13, 4);


  /* Switch CIR EIR in dual shapers */
  Soc_petra_a_regs.sch.switch_cir_eir_in_dual_shapers_reg.addr.base = sizeof(uint32) * 0x430c;
  Soc_petra_a_regs.sch.switch_cir_eir_in_dual_shapers_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.switch_cir_eir_in_dual_shapers_reg.switch_cireir), sizeof(uint32) * 0x430c, sizeof(uint32) * 0x0000, 31, 0);

  /* FSF Composite Configuration */
  Soc_petra_a_regs.sch.fsf_composite_configuration_reg.addr.base = sizeof(uint32) * 0x430d;
  Soc_petra_a_regs.sch.fsf_composite_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.fsf_composite_configuration_reg.fsfcomp_odd_even), sizeof(uint32) * 0x430d, sizeof(uint32) * 0x0000, 31, 0);

  /* Select flow to queue mapping */
  Soc_petra_a_regs.sch.select_flow_to_queue_mapping_reg.addr.base = sizeof(uint32) * 0x430e;
  Soc_petra_a_regs.sch.select_flow_to_queue_mapping_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.select_flow_to_queue_mapping_reg.inter_dig), sizeof(uint32) * 0x430e, sizeof(uint32) * 0x0000, 31, 0);

  /* System Red Configuration */
  Soc_petra_a_regs.sch.system_red_configuration_reg.addr.base = sizeof(uint32) * 0x430f;
  Soc_petra_a_regs.sch.system_red_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.system_red_configuration_reg.aging_timer_cfg), sizeof(uint32) * 0x430f, sizeof(uint32) * 0x0000, 20, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.system_red_configuration_reg.reset_xpired_qsz), sizeof(uint32) * 0x430f, sizeof(uint32) * 0x0000, 24, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.system_red_configuration_reg.aging_only_dec_pqs), sizeof(uint32) * 0x430f, sizeof(uint32) * 0x0000, 25, 25);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.system_red_configuration_reg.enable_sys_red), sizeof(uint32) * 0x430f, sizeof(uint32) * 0x0000, 28, 28);

  /* HRPort En Register 1 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_HRPORT_EN_NOF_REGS; reg_idx++)
  {
    Soc_petra_a_regs.sch.hrport_en_reg[reg_idx].addr.base = sizeof(uint32) * (0x4310 + reg_idx);
    Soc_petra_a_regs.sch.hrport_en_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.hrport_en_reg[reg_idx].hrport_en), sizeof(uint32) * (0x4310 + reg_idx), sizeof(uint32) * 0x0000, 31, 0);
  }

  /* HR Flow Control Mask 0 1  */
  Soc_petra_a_regs.sch.hr_flow_control_mask_0_1_reg.addr.base = sizeof(uint32) * 0x4318;
  Soc_petra_a_regs.sch.hr_flow_control_mask_0_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.hr_flow_control_mask_0_1_reg.hrfcmask0), sizeof(uint32) * 0x4318, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.hr_flow_control_mask_0_1_reg.hrfcmask1), sizeof(uint32) * 0x4318, sizeof(uint32) * 0x0000, 31, 16);

  /* HR Flow Control Mask 2 3  */
  Soc_petra_a_regs.sch.hr_flow_control_mask_2_3_reg.addr.base = sizeof(uint32) * 0x4319;
  Soc_petra_a_regs.sch.hr_flow_control_mask_2_3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.hr_flow_control_mask_2_3_reg.hrfcmask2), sizeof(uint32) * 0x4319, sizeof(uint32) * 0x0000, 15, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.hr_flow_control_mask_2_3_reg.hrfcmask3), sizeof(uint32) * 0x4319, sizeof(uint32) * 0x0000, 31, 16);

  /* Device Scheduler (DVS) Config0 */
  Soc_petra_a_regs.sch.dvs_config0_reg.addr.base = sizeof(uint32) * 0x4320;
  Soc_petra_a_regs.sch.dvs_config0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_config0_reg.nifforce_pause), sizeof(uint32) * 0x4320, sizeof(uint32) * 0x0000, 31, 0);

  /* Device Scheduler (DVS) Config1 */
  Soc_petra_a_regs.sch.dvs_config1_reg.addr.base = sizeof(uint32) * 0x4321;
  Soc_petra_a_regs.sch.dvs_config1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_config1_reg.cpuforce_pause), sizeof(uint32) * 0x4321, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_config1_reg.rcyforce_pause), sizeof(uint32) * 0x4321, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_config1_reg.erpforce_pause), sizeof(uint32) * 0x4321, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_config1_reg.olpforce_pause), sizeof(uint32) * 0x4321, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_config1_reg.force_pause), sizeof(uint32) * 0x4321, sizeof(uint32) * 0x0000, 4, 4);

  /* Device Scheduler (DVS) Weight Config0-3 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_DVS_WEIGHT_CONFIG_REGS; reg_idx++)
  {
    Soc_petra_a_regs.sch.dvs_weight_config_reg[reg_idx].addr.base = sizeof(uint32) * (0x4322 + reg_idx);
    Soc_petra_a_regs.sch.dvs_weight_config_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_weight_config_reg[reg_idx].wfqweight_xx), sizeof(uint32) * (0x4322 + reg_idx), sizeof(uint32) * 0x0000, 9, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_weight_config_reg[reg_idx].wfqweight_xx_plus_1), sizeof(uint32) * (0x4322 + reg_idx), sizeof(uint32) * 0x0000, 25, 16);
  }
  /* RCI Params */
  Soc_petra_a_regs.sch.rci_params_reg.addr.base = sizeof(uint32) * 0x4326;
  Soc_petra_a_regs.sch.rci_params_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_params_reg.rciena), sizeof(uint32) * 0x4326, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_params_reg.fap_rciena), sizeof(uint32) * 0x4326, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_params_reg.rciinc_val), sizeof(uint32) * 0x4326, sizeof(uint32) * 0x0000, 10, 4);

  /* RCI Decrement Values */
  Soc_petra_a_regs.sch.rci_decrement_values_reg.addr.base = sizeof(uint32) * 0x4327;
  Soc_petra_a_regs.sch.rci_decrement_values_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement0), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement1), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 7, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement2), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 11, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement3), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 15, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement4), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 19, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement5), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 23, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement6), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 27, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rci_decrement_values_reg.rcidecrement7), sizeof(uint32) * 0x4327, sizeof(uint32) * 0x0000, 31, 28);
  /* NIF[0-3],[16-19] CAL Config (Ch NIF) */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_CH_NIF_CAL_REGS; reg_idx++)
  {
    Soc_petra_a_regs.sch.ch_nif_cal_config_reg[reg_idx].addr.base = sizeof(uint32) * (0x4330 + 2*reg_idx);
    Soc_petra_a_regs.sch.ch_nif_cal_config_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_CH_NIF_CAL_LEN_NOF_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.sch.ch_nif_cal_config_reg[reg_idx].cal_len[fld_idx]),
        sizeof(uint32) * (0x4330 + 2*reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(9 + (12*fld_idx)),
        (uint8)(0 + (12*fld_idx))
      );
    }
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.ch_nif_cal_config_reg[reg_idx].ch_nifxx_weight), sizeof(uint32) * (0x4330 + 2*reg_idx), sizeof(uint32) * 0x0000, 26, 24);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.ch_nif_cal_config_reg[reg_idx].dvscalendar_sel_ch_nifxx), sizeof(uint32) * (0x4330 + 2*reg_idx), sizeof(uint32) * 0x0000, 28, 28);
  }

  /* NIF[0-3],[16-19] Rates (Ch NIF) */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_CH_NIF_RATE_REGS; reg_idx++)
  {
    Soc_petra_a_regs.sch.ch_nif_reg[reg_idx].addr.base = sizeof(uint32) * (0x4331 + 2*reg_idx);
    Soc_petra_a_regs.sch.ch_nif_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.ch_nif_reg[reg_idx].ch_nifxx_sum_of_ports), sizeof(uint32) * (0x4331 + 2*reg_idx), sizeof(uint32) * 0x0000, 17, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.ch_nif_reg[reg_idx].ch_nifxx_max_cr_rate), sizeof(uint32) * (0x4331 + 2*reg_idx), sizeof(uint32) * 0x0000, 31, 18);
  }

  /* NIF[4-15], [20-31] Config (Port NIF) */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_1PORT_NIF_RATE_REGS; reg_idx++)
  {
    /* There is a gap in the addresses between NIF groups A/B */
    gap = (reg_idx >= SOC_PETRA_NOF_1PORT_NIF_RATE_REGS / 2)?0x4:0x0;

    Soc_petra_a_regs.sch.port_nif_reg[reg_idx].addr.base = sizeof(uint32) * (0x4344+reg_idx+gap);
    Soc_petra_a_regs.sch.port_nif_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.sch.port_nif_reg[reg_idx].port_nifxx_max_cr_rate),
      sizeof(uint32) * (0x4344+reg_idx+gap),
      sizeof(uint32) * 0x0000,
      17,
      0
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.sch.port_nif_reg[reg_idx].nifxx_port_id),
      sizeof(uint32) * (0x4344+reg_idx+gap),
      sizeof(uint32) * 0x0000,
      26,
      20
    );
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.sch.port_nif_reg[reg_idx].port_nifxx_weight),
      sizeof(uint32) * (0x4344+reg_idx+gap),
      sizeof(uint32) * 0x0000,
      30,
      28
    );
  }

  /* CPU CAL Configuration Register */
  Soc_petra_a_regs.sch.cpu_cal_configuration_reg.addr.base = sizeof(uint32) * 0x4360;
  Soc_petra_a_regs.sch.cpu_cal_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_CH_NIF_CAL_LEN_NOF_FLDS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.sch.cpu_cal_configuration_reg.cal_len[fld_idx]),
      sizeof(uint32) * 0x4360,
      sizeof(uint32) * 0x0000,
      (uint8)(9 + (12*fld_idx)),
      (uint8)(0 + (12*fld_idx))
    );
  }
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.cpu_cal_configuration_reg.cpuweight), sizeof(uint32) * 0x4360, sizeof(uint32) * 0x0000, 26, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.cpu_cal_configuration_reg.dvscalendar_sel_cpu), sizeof(uint32) * 0x4360, sizeof(uint32) * 0x0000, 28, 28);

  /* CPU Rates0 */
  Soc_petra_a_regs.sch.cpu_rates0_reg.addr.base = sizeof(uint32) * 0x4361;
  Soc_petra_a_regs.sch.cpu_rates0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.cpu_rates0_reg.cpusum_of_ports), sizeof(uint32) * 0x4361, sizeof(uint32) * 0x0000, 17, 0);

  /* CPU Rate1 */
  Soc_petra_a_regs.sch.cpu_rate1_reg.addr.base = sizeof(uint32) * 0x4362;
  Soc_petra_a_regs.sch.cpu_rate1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.cpu_rate1_reg.cpumax_cr_rate), sizeof(uint32) * 0x4362, sizeof(uint32) * 0x0000, 21, 0);

  /* RCY CAL Configuration */
  Soc_petra_a_regs.sch.rcy_cal_configuration_reg.addr.base = sizeof(uint32) * 0x4364;
  Soc_petra_a_regs.sch.rcy_cal_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  for (fld_idx = 0; fld_idx < SOC_PETRA_CH_NIF_CAL_LEN_NOF_FLDS; fld_idx++)
  {
    soc_petra_reg_fld_set(
      &(Soc_petra_a_regs.sch.rcy_cal_configuration_reg.cal_len[fld_idx]),
      sizeof(uint32) * 0x4364,
      sizeof(uint32) * 0x0000,
      (uint8)(9 + (12*fld_idx)),
      (uint8)(0 + (12*fld_idx))
    );
  }
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rcy_cal_configuration_reg.rcyweight), sizeof(uint32) * 0x4364, sizeof(uint32) * 0x0000, 26, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rcy_cal_configuration_reg.dvscalendar_sel_rcy), sizeof(uint32) * 0x4364, sizeof(uint32) * 0x0000, 28, 28);

  /* RCY Rate0 */
  Soc_petra_a_regs.sch.rcy_rate0_reg.addr.base = sizeof(uint32) * 0x4365;
  Soc_petra_a_regs.sch.rcy_rate0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rcy_rate0_reg.rcysum_of_ports), sizeof(uint32) * 0x4365, sizeof(uint32) * 0x0000, 17, 0);

  /* RCY Rate1 */
  Soc_petra_a_regs.sch.rcy_rate1_reg.addr.base = sizeof(uint32) * 0x4366;
  Soc_petra_a_regs.sch.rcy_rate1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.rcy_rate1_reg.rcymax_cr_rate), sizeof(uint32) * 0x4366, sizeof(uint32) * 0x0000, 21, 0);

  /* ERP Config */
  Soc_petra_a_regs.sch.erp_config_reg.addr.base = sizeof(uint32) * 0x4368;
  Soc_petra_a_regs.sch.erp_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.erp_config_reg.erpmax_cr_rate), sizeof(uint32) * 0x4368, sizeof(uint32) * 0x0000, 17, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.erp_config_reg.erpport_id), sizeof(uint32) * 0x4368, sizeof(uint32) * 0x0000, 26, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.erp_config_reg.erpweight), sizeof(uint32) * 0x4368, sizeof(uint32) * 0x0000, 30, 28);

  /* OLP Config */
  Soc_petra_a_regs.sch.olp_config_reg.addr.base = sizeof(uint32) * 0x4369;
  Soc_petra_a_regs.sch.olp_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.olp_config_reg.olpmax_cr_rate), sizeof(uint32) * 0x4369, sizeof(uint32) * 0x0000, 17, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.olp_config_reg.olpport_id), sizeof(uint32) * 0x4369, sizeof(uint32) * 0x0000, 26, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.olp_config_reg.olpweight), sizeof(uint32) * 0x4369, sizeof(uint32) * 0x0000, 30, 28);

  /* DVS Credit Counter Configuration register */
  Soc_petra_a_regs.sch.dvs_credit_counter_configuration_reg.addr.base = sizeof(uint32) * 0x436a;
  Soc_petra_a_regs.sch.dvs_credit_counter_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_credit_counter_configuration_reg.dvsfilter_port), sizeof(uint32) * 0x436a, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_credit_counter_configuration_reg.dvsfilter_nif), sizeof(uint32) * 0x436a, sizeof(uint32) * 0x0000, 17, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_credit_counter_configuration_reg.cnt_by_port), sizeof(uint32) * 0x436a, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_credit_counter_configuration_reg.cnt_by_nif), sizeof(uint32) * 0x436a, sizeof(uint32) * 0x0000, 21, 21);

  /* DVS FC and RCI Counters Configuration register */
  Soc_petra_a_regs.sch.dvs_fc_and_rci_counters_configuration_reg.addr.base = sizeof(uint32) * 0x436b;
  Soc_petra_a_regs.sch.dvs_fc_and_rci_counters_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_fc_and_rci_counters_configuration_reg.fccnt_port), sizeof(uint32) * 0x436b, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_fc_and_rci_counters_configuration_reg.cnt_high_fc), sizeof(uint32) * 0x436b, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.dvs_fc_and_rci_counters_configuration_reg.cnt_low_fc), sizeof(uint32) * 0x436b, sizeof(uint32) * 0x0000, 21, 21);

  /* STL Group Config0 */
  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_STL_GROUP_CONFIG_REGS; reg_idx++)
  {
    Soc_petra_a_regs.sch.stl_group_config_reg[reg_idx].addr.base = sizeof(uint32) * (0x4370 + reg_idx);
    Soc_petra_a_regs.sch.stl_group_config_reg[reg_idx].addr.step = sizeof(uint32) * 0x0000;
    for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_STL_GROUP_CONFIG_FLDS; fld_idx++)
    {
      soc_petra_reg_fld_set(
        &(Soc_petra_a_regs.sch.stl_group_config_reg[reg_idx].port_group[fld_idx]),
        sizeof(uint32) * (0x4370 + reg_idx),
        sizeof(uint32) * 0x0000,
        (uint8)(1 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_NOF_STL_GROUP_CONFIG_FLDS)*fld_idx)),
        (uint8)(0 + ((SOC_SAND_REG_SIZE_BITS/SOC_PETRA_NOF_STL_GROUP_CONFIG_FLDS)*fld_idx))
      );
    }
  }

  /* SCH_Fabric_Multicast_Port_Configuration_register */
  Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.addr.base = sizeof(uint32) * 0x4376;
  Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_0].port_id), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_0].enable), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_1].port_id), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 14, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_1].enable), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_2].port_id), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 22, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_2].enable), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_3].port_id), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 30, 24);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.sch_fabric_multicast_port_configuration_reg.multicast_class[SOC_PETRA_MULTICAST_CLASS_3].enable), sizeof(uint32) * 0x4376, sizeof(uint32) * 0x0000, 31, 31);

  /* Ingress Shaping Port Configuration */
  Soc_petra_a_regs.sch.ingress_shaping_port_configuration_reg.addr.base = sizeof(uint32) * 0x4377;
  Soc_petra_a_regs.sch.ingress_shaping_port_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.ingress_shaping_port_configuration_reg.ingress_shaping_port_id), sizeof(uint32) * 0x4377, sizeof(uint32) * 0x0000, 6, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.ingress_shaping_port_configuration_reg.ingress_shaping_enable), sizeof(uint32) * 0x4377, sizeof(uint32) * 0x0000, 8, 8);

  /* SMP Internal messages */
  Soc_petra_a_regs.sch.smp_internal_messages.addr.base = sizeof(uint32) * 0x437a;
  Soc_petra_a_regs.sch.smp_internal_messages.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_internal_messages.delay), sizeof(uint32) * 0x437a, sizeof(uint32) * 0x0000, 7, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.smp_internal_messages.enable), sizeof(uint32) * 0x437a, sizeof(uint32) * 0x0000, 12, 12);

  /* Force High FC Register 0 */
  Soc_petra_a_regs.sch.force_high_fc_0_reg.addr.base = sizeof(uint32) * 0x4380;
  Soc_petra_a_regs.sch.force_high_fc_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_high_fc_0_reg.force_high_fc31_0), sizeof(uint32) * 0x4380, sizeof(uint32) * 0x0000, 31, 0);

  /* Force High FC Register 1 */
  Soc_petra_a_regs.sch.force_high_fc_1_reg.addr.base = sizeof(uint32) * 0x4381;
  Soc_petra_a_regs.sch.force_high_fc_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_high_fc_1_reg.force_high_fc63_32), sizeof(uint32) * 0x4381, sizeof(uint32) * 0x0000, 31, 0);

  /* Force High FC Register 2 */
  Soc_petra_a_regs.sch.force_high_fc_2_reg.addr.base = sizeof(uint32) * 0x4382;
  Soc_petra_a_regs.sch.force_high_fc_2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_high_fc_2_reg.force_high_fc81_64), sizeof(uint32) * 0x4382, sizeof(uint32) * 0x0000, 16, 0);

  /* Force Low FC Register 0 */
  Soc_petra_a_regs.sch.force_low_fc_0_reg.addr.base = sizeof(uint32) * 0x4384;
  Soc_petra_a_regs.sch.force_low_fc_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_low_fc_0_reg.force_low_fc31_0), sizeof(uint32) * 0x4384, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Low FC Register 1 */
  Soc_petra_a_regs.sch.force_low_fc_1_reg.addr.base = sizeof(uint32) * 0x4385;
  Soc_petra_a_regs.sch.force_low_fc_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_low_fc_1_reg.force_low_fc63_32), sizeof(uint32) * 0x4385, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Low FC Register 2 */
  Soc_petra_a_regs.sch.force_low_fc_2_reg.addr.base = sizeof(uint32) * 0x4386;
  Soc_petra_a_regs.sch.force_low_fc_2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_low_fc_2_reg.force_low_fc81_64), sizeof(uint32) * 0x4386, sizeof(uint32) * 0x0000, 16, 0);

  /* Force Aggr FC Register 0 */
  Soc_petra_a_regs.sch.force_aggr_fc_0_reg.addr.base = sizeof(uint32) * 0x4388;
  Soc_petra_a_regs.sch.force_aggr_fc_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_aggr_fc_0_reg.force_aggr_fc159_128), sizeof(uint32) * 0x4388, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Aggr FC Register 1 */
  Soc_petra_a_regs.sch.force_aggr_fc_1_reg.addr.base = sizeof(uint32) * 0x4389;
  Soc_petra_a_regs.sch.force_aggr_fc_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_aggr_fc_1_reg.force_aggr_fc191_160), sizeof(uint32) * 0x4389, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Aggr FC Register 2 */
  Soc_petra_a_regs.sch.force_aggr_fc_2_reg.addr.base = sizeof(uint32) * 0x438a;
  Soc_petra_a_regs.sch.force_aggr_fc_2_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_aggr_fc_2_reg.force_aggr_fc223_192), sizeof(uint32) * 0x438a, sizeof(uint32) * 0x0000, 31, 0);

  /* Force Aggr FC Register 3 */
  Soc_petra_a_regs.sch.force_aggr_fc_3_reg.addr.base = sizeof(uint32) * 0x438b;
  Soc_petra_a_regs.sch.force_aggr_fc_3_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_aggr_fc_3_reg.force_aggr_fc255_224), sizeof(uint32) * 0x438b, sizeof(uint32) * 0x0000, 31, 0);

  /* Force FC Configuration Register */
  Soc_petra_a_regs.sch.force_fc_configuration_reg.addr.base = sizeof(uint32) * 0x438c;
  Soc_petra_a_regs.sch.force_fc_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_fc_configuration_reg.force_high_fcoverride), sizeof(uint32) * 0x438c, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_fc_configuration_reg.force_low_fcoverride), sizeof(uint32) * 0x438c, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_fc_configuration_reg.force_aggr_fcen), sizeof(uint32) * 0x438c, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.sch.force_fc_configuration_reg.force_aggr_fcoverride), sizeof(uint32) * 0x438c, sizeof(uint32) * 0x0000, 3, 3);
}

/* Block registers initialization: SERDES */
STATIC void
  soc_pa_regs_init_SERDES(void)
{
  uint32
    qrtt_idx,
    reg_idx;

  Soc_petra_a_regs.serdes.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_SERDES;
  Soc_petra_a_regs.serdes.addr.base = sizeof(uint32) * 0x0000;
  Soc_petra_a_regs.serdes.addr.step = sizeof(uint32) * 0x0000;

  for (qrtt_idx = 0; qrtt_idx < SOC_PETRA_SRD_NOF_PER_QRTT_REGS; qrtt_idx++)
  {
    /* srd_LN_cfga */
    for (reg_idx = 0; reg_idx < SOC_PETRA_SRD_NOF_PER_LANE_REGS; reg_idx++)
    {

      Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].addr.base = sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET);
      Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_tx_en), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 0, 0);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_tx_idle), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 1, 1);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_tx_inv), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 2, 2);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].rx_nearest_comma_align), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 3, 3);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_tx_rate), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 5, 4);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_rate), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 7, 6);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_en), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 8, 8);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_idle), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 9, 9);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_inv), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 10, 10);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_align_en_req), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 11, 11);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_comma_det_mask_lsb), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 12, 12);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_comma_det_mask_msb), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 13, 13);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_rx_pcs_sync_fsm_init), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 14, 14);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_share_cfg), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 15, 15);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_int_en_rx_decode_err), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 16, 16);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_int_en_rx_disp_err), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 17, 17);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_int_en_rx_comma_realign), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 18, 18);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_int_en_rx_sig_det_change), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 19, 19);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ln_int_sel_rx_pcs_sync_fsm), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 23, 20);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ebist_rx_en), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 24, 24);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ebist_tx_en), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 25, 25);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ebist_prbs_pol_sel), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 26, 26);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ebist_tx_const_data_en), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 27, 27);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ebist_cnt_mode), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 29, 28);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].ext_tx_data_ovrd_en), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 30, 30);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_cfga_reg[qrtt_idx][reg_idx].tx_use_ext_byte_toggle), sizeof(uint32) * (0x0000 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 31, 31);

      /* srd_ln_stat */
      Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].addr.base = sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET);
      Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_sig_det), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 0, 0);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_sig_det_change), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 1, 1);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].cmu_tx_trimming), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 2, 2);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].cmu_pll_rst), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 3, 3);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_rst), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 4, 4);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_sig_lost), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 5, 5);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_freq_invld), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 6, 6);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_sync_stat), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 8, 8);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_pcs_sync_fsm_ind), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 9, 9);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_code_err), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 13, 12);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_disp_err), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 15, 14);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_comma_align_en), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 16, 16);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_comma_realign), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 17, 17);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_stat_reg[qrtt_idx][reg_idx].ln_rx_comma_alignment), sizeof(uint32) * (0x0001 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 24, 20);

      /* srd_ln_EBIST */
      Soc_petra_a_regs.serdes.srd_ln_ebist_reg[qrtt_idx][reg_idx].addr.base = sizeof(uint32) * (0x0002 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET);
      Soc_petra_a_regs.serdes.srd_ln_ebist_reg[qrtt_idx][reg_idx].addr.step = sizeof(uint32) * 0x0000;
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_ebist_reg[qrtt_idx][reg_idx].ebist_err_cnt), sizeof(uint32) * (0x0002 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 30, 0);
      soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ln_ebist_reg[qrtt_idx][reg_idx].ebist_sync), sizeof(uint32) * (0x0002 + 4*reg_idx + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 31, 31);
    }
    /* srd_cmu_cfga */
    Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].addr.base = sizeof(uint32) * (0x0010+ qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET);
    Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].cmu_macro_pd), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 0, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].cmu_macro_rst), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 1, 1);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].cmu_start_trim), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 2, 2);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].cmu_trim_self), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 3, 3);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].cmu_plln), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 5, 4);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].cmu_pllm), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 8, 6);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].ln_20b_mode), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 9, 9);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].ln_tx_sync_fifo_en), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 10, 10);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].ln_tx_sync_fifo_rstn), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 11, 11);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].ln_txob_pd), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 15, 12);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].ln_rx_sig_det_mode), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 21, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfga_reg[qrtt_idx].ln_rx_sig_det_cnt), sizeof(uint32) * (0x0010 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 31, 24);

    /* srd_cmu_cfgb */
    Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].addr.base = sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET);
    Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].ln_tx_const_data), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 19, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].cmu_msel_in), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 23, 23);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].ln_pcomp_en), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 24, 24);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].ln_set_ctl), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 27, 25);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].ln_tx_rdet_en), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 28, 28);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].ln_tx_beacon_en), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 29, 29);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_cfgb_reg[qrtt_idx].ln_mac_phy_pd), sizeof(uint32) * (0x0011 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 31, 30);

    /* srd_cmu_stat */
    Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].addr.base = sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET);
    Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].addr.step = sizeof(uint32) * 0x0000;
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].cmu_tx_trimming), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 0, 0);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].cmu_pll_rst), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 1, 1);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].ln_rx_rst), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 7, 4);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].ln_rx_sig_det), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 11, 8);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].ln_rx_sync_stat), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 15, 12);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].ln_int_ind), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 19, 16);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].cmu_scif_data_out), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 20, 20);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].cmu_scif_int_req), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 21, 21);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].ln_pcg2_p), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 27, 24);
    soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_cmu_stat_reg[qrtt_idx].ln_pcg2_n), sizeof(uint32) * (0x0012 + qrtt_idx*SOC_PETRA_SRD_QRTT_DELTA_OFFSET), sizeof(uint32) * 0x0000, 31, 28);
  }

  /* Srd Ipu Cfg */
  Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.addr.base = sizeof(uint32) * 0x0078;
  Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.iram_rst), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.rst8051), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.epb_rst), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 2, 2);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ext_scif_rst), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 3, 3);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.clk_sel8051), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 5, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.pd8051), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 6, 6);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.clk_pd8051), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 7, 7);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.chan_id), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 10, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.int8051), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 11, 11);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.epb_int_timeout_en), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 12, 12);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.scif_ipu_en_on_cmu_sel), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 13, 13);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ipu_parity_en), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 16, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ipu_parity_force_err), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 17, 17);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ipu_parity_err_int_en), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ipu_intreq_int_en), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ipu_intack_int_en), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_ipu_cfg_reg.ipu_8051_prom_loaded), sizeof(uint32) * 0x0078, sizeof(uint32) * 0x0000, 21, 21);

  /* Srd Epb Op */
  Soc_petra_a_regs.serdes.srd_epb_op_reg.addr.base = sizeof(uint32) * 0x0079;
  Soc_petra_a_regs.serdes.srd_epb_op_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.addr_element), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.addr_channel), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 6, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.addr_global), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.addr_reg), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 14, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.addr_ipu_cs), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.macro_sel), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.macro_sel_all), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.wr_en), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.int_req), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.int_hold), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_op_reg.data), sizeof(uint32) * 0x0079, sizeof(uint32) * 0x0000, 31, 24);

  /* Srd Epb Rd */
  Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr.base = sizeof(uint32) * 0x007a;
  Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr_element), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr_channel), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 6, 4);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr_global), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 8, 8);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr_reg), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 14, 9);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.addr_ipu_cs), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 15, 15);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.macro_sel), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 17, 16);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.macro_sel_all), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 18, 18);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.wr_op), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 19, 19);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.error), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 20, 20);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.valid), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 21, 21);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.int_req), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 22, 22);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.int_ack), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 23, 23);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.serdes.srd_epb_rd_reg.data), sizeof(uint32) * 0x007a, sizeof(uint32) * 0x0000, 31, 24);
}

  /* Block registers initialization: MCC */
STATIC void
  soc_pa_regs_init_MCC(void)
{
  Soc_petra_a_regs.mcc.nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_MCC;
  Soc_petra_a_regs.mcc.addr.base = sizeof(uint32) * 0x4e00;
  Soc_petra_a_regs.mcc.addr.step = sizeof(uint32) * 0x0000;

  /* SMS Register */
  Soc_petra_a_regs.mcc.sms_reg.addr.base = sizeof(uint32) * 0x4e00;
  Soc_petra_a_regs.mcc.sms_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.sms_reg.sms), sizeof(uint32) * 0x4e00, sizeof(uint32) * 0x0000, 31, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.sms_reg.cmd_active), sizeof(uint32) * 0x4e00, sizeof(uint32) * 0x0000, 31, 31);

  /* TAP Configuration Register */
  Soc_petra_a_regs.mcc.tap_config_reg.addr.base = sizeof(uint32) * 0x4e01;
  Soc_petra_a_regs.mcc.tap_config_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_config_reg.tap), sizeof(uint32) * 0x4e01, sizeof(uint32) * 0x0000, 31, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_config_reg.tap_cmd), sizeof(uint32) * 0x4e01, sizeof(uint32) * 0x0000, 4, 4);

  /* TAP Result-0 Register */
  Soc_petra_a_regs.mcc.tap_res_0_reg.addr.base = sizeof(uint32) * 0x4e02;
  Soc_petra_a_regs.mcc.tap_res_0_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_res_0_reg.res), sizeof(uint32) * 0x4e02, sizeof(uint32) * 0x0000, 31, 0);

  /* TAP Result-1 Register */
  Soc_petra_a_regs.mcc.tap_res_1_reg.addr.base = sizeof(uint32) * 0x4e03;
  Soc_petra_a_regs.mcc.tap_res_1_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_res_1_reg.res), sizeof(uint32) * 0x4e03, sizeof(uint32) * 0x0000, 31, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_res_1_reg.shift), sizeof(uint32) * 0x4e03, sizeof(uint32) * 0x0000, 3, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_res_1_reg.mask), sizeof(uint32) * 0x4e03, sizeof(uint32) * 0x0000, 9, 4);

  /* Memory Configuration Register */
  Soc_petra_a_regs.mcc.memory_configuration_reg.addr.base = sizeof(uint32) * 0x4e04;
  Soc_petra_a_regs.mcc.memory_configuration_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.memory_configuration_reg.mbist_mode), sizeof(uint32) * 0x4e04, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.memory_configuration_reg.repair_memories), sizeof(uint32) * 0x4e04, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.memory_configuration_reg.dm), sizeof(uint32) * 0x4e04, sizeof(uint32) * 0x0000, 18, 16);

  /* Ready Sms Statuses1 */
  Soc_petra_a_regs.mcc.ready_sms_statuses1.addr.base = sizeof(uint32) * 0x4e05;
  Soc_petra_a_regs.mcc.ready_sms_statuses1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.ready_sms_statuses1.ready_sms1), sizeof(uint32) * 0x4e05, sizeof(uint32) * 0x0000, 31, 0);

  /* Ready Sms Statuses2 */
  Soc_petra_a_regs.mcc.ready_sms_statuses2.addr.base = sizeof(uint32) * 0x4e06;
  Soc_petra_a_regs.mcc.ready_sms_statuses2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.ready_sms_statuses2.ready_sms2), sizeof(uint32) * 0x4e06, sizeof(uint32) * 0x0000, 31, 0);

  /* Fail Sms Statuses1 */
  Soc_petra_a_regs.mcc.fail_sms_statuses1.addr.base = sizeof(uint32) * 0x4e07;
  Soc_petra_a_regs.mcc.fail_sms_statuses1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.fail_sms_statuses1.fail_sms1), sizeof(uint32) * 0x4e07, sizeof(uint32) * 0x0000, 31, 0);

  /* Fail Sms Statuses2 */
  Soc_petra_a_regs.mcc.fail_sms_statuses2.addr.base = sizeof(uint32) * 0x4e08;
  Soc_petra_a_regs.mcc.fail_sms_statuses2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.fail_sms_statuses2.fail_sms2), sizeof(uint32) * 0x4e08, sizeof(uint32) * 0x0000, 31, 0);

  /* SFP Ready Register */
  Soc_petra_a_regs.mcc.sfp_ready_reg.addr.base = sizeof(uint32) * 0x4e09;
  Soc_petra_a_regs.mcc.sfp_ready_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.sfp_ready_reg.sfp_ready), sizeof(uint32) * 0x4e09, sizeof(uint32) * 0x0000, 0, 0);

  /* TAP General Register1 */
  Soc_petra_a_regs.mcc.tap_gen_reg_1.addr.base = sizeof(uint32) * 0x4e10;
  Soc_petra_a_regs.mcc.tap_gen_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_gen_reg_1.gen1), sizeof(uint32) * 0x4e10, sizeof(uint32) * 0x0000, 31, 0);

  /* TAP General Register2 */
  Soc_petra_a_regs.mcc.tap_gen_reg_2.addr.base = sizeof(uint32) * 0x4e11;
  Soc_petra_a_regs.mcc.tap_gen_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_gen_reg_2.gen2), sizeof(uint32) * 0x4e11, sizeof(uint32) * 0x0000, 31, 0);

  /* Mbist Resets */
  Soc_petra_a_regs.mcc.mbist_resets_reg.addr.base = sizeof(uint32) * 0x4e0a;
  Soc_petra_a_regs.mcc.mbist_resets_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.mbist_resets_reg.sms_rst), sizeof(uint32) * 0x4e0a, sizeof(uint32) * 0x0000, 0, 0);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.mbist_resets_reg.sfp_rst), sizeof(uint32) * 0x4e0a, sizeof(uint32) * 0x0000, 1, 1);
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.mbist_resets_reg.mbist_reset), sizeof(uint32) * 0x4e0a, sizeof(uint32) * 0x0000, 2, 2);

  /* TAP TMS value Register1 */
  Soc_petra_a_regs.mcc.tap_tms_value_reg_1.addr.base = sizeof(uint32) * 0x4e20;
  Soc_petra_a_regs.mcc.tap_tms_value_reg_1.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_tms_value_reg_1.gen), sizeof(uint32) * 0x4e20, sizeof(uint32) * 0x0000, 31, 0);

  /* TAP TMS value Register2 */
  Soc_petra_a_regs.mcc.tap_tms_value_reg_2.addr.base = sizeof(uint32) * 0x4e21;
  Soc_petra_a_regs.mcc.tap_tms_value_reg_2.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.tap_tms_value_reg_2.gen), sizeof(uint32) * 0x4e21, sizeof(uint32) * 0x0000, 31, 0);

  /* TAP TMS value Register2 */
  Soc_petra_a_regs.mcc.udr_mode_reg.addr.base = sizeof(uint32) * 0x4e32;
  Soc_petra_a_regs.mcc.udr_mode_reg.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.mcc.udr_mode_reg.gen), sizeof(uint32) * 0x4e32, sizeof(uint32) * 0x0000, 31, 0);
}

/*****************************************************
*NAME
* soc_pa_regs_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to registers database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN   int   unit - device identifier
*    SOC_SAND_OUT  SOC_PA_REGS  *soc_petra_regs - pointer to soc_petra
*                           registers database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_regs.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*SEE ALSO:
*****************************************************/
uint32
  soc_pa_regs_get(
    SOC_SAND_OUT  SOC_PA_REGS  **soc_petra_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_GET);

  if (Soc_petra_a_regs_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NOT_INITIALIZED, 20, exit);
  }

  *soc_petra_regs = &Soc_petra_a_regs;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_regs_get()", 0, 0);
}

/*****************************************************
*NAME
*  soc_pa_regs_init
*TYPE:
*  PROC
*FUNCTION:
*  Dynamically allocates and initializes Soc_petra registers database.
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
  soc_pa_regs_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_INIT);

  if (Soc_petra_a_regs_initialized == TRUE)
  {
    goto exit;
  }

  soc_pa_regs_init_ECI();
  soc_pa_regs_init_OLP();
  soc_pa_regs_init_NIF();
  soc_pa_regs_init_NIF_MAC_LANE();
  soc_pa_regs_init_IRE();
  soc_pa_regs_init_IDR();
  soc_pa_regs_init_IRR();
  soc_pa_regs_init_IHP();
  soc_pa_regs_init_IQM();
  soc_pa_regs_init_MMU();
  soc_pa_regs_init_QDR();
  soc_pa_regs_init_IPS();
  soc_pa_regs_init_IPT();
  soc_pa_regs_init_DRC();
  soc_pa_regs_init_DPI();
  soc_pa_regs_init_FDR();
  soc_pa_regs_init_FDT();
  soc_pa_regs_init_FCR();
  soc_pa_regs_init_FCT();
  soc_pa_regs_init_MESH_TOPOLOGY();
  soc_pa_regs_init_RTP();
  soc_pa_regs_init_FABRIC_MAC();
  soc_pa_regs_init_MSW();
  soc_pa_regs_init_EGQ();
  soc_pa_regs_init_EPNI();
  soc_pa_regs_init_CFC();
  soc_pa_regs_init_SCH();
  soc_pa_regs_init_SERDES();
  soc_pa_regs_init_MCC();

  Soc_petra_a_regs_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_regs_init",0,0);
}
#else
/* $Id: petra_chip_regs.c,v 1.9 Broadcom SDK $
 *	Initialization when the compilation flag of Soc_petra-A is off
 */
uint32
  soc_pa_regs_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_INIT);

  if (Soc_petra_a_regs_initialized == TRUE)
  {
    goto exit;
  }

  /* Invalid device */
  Soc_petra_a_regs.addr.base = 0xffffffff;
  Soc_petra_a_regs.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_petra_a_regs.invalid_device), 0xffffffff, sizeof(uint32) * 0x0000, 32, 32);

  Soc_petra_a_regs_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_regs_init",0,0);
}
#endif

#ifdef LINK_PB_LIBRARIES
#else
/*
 *	Initialization when the compilation flag of Soc_petra-B is off
 */
uint32
  soc_pb_regs_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_INIT);

  if (Soc_pb_regs_initialized == TRUE)
  {
    goto exit;
  }

  /* Invalid device */
  Soc_pb_regs.addr.base = 0xffffffff;
  Soc_pb_regs.addr.step = sizeof(uint32) * 0x0000;
  soc_petra_reg_fld_set( &(Soc_pb_regs.invalid_device), 0xffffffff, sizeof(uint32) * 0x0000, 32, 32);

  Soc_pb_regs_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_regs_init",0,0);
}
#endif

uint32
  soc_petra_regs_get(
    SOC_SAND_OUT  SOC_PETRA_REGS  **soc_petra_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_GET);

  if (Soc_petra_regs_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NOT_INITIALIZED, 20, exit);
  }

  *soc_petra_regs = &Soc_petra_regs;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_regs_get()", 0, 0);
}


uint32
  soc_petra_regs_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_INIT);

  if (Soc_petra_regs_initialized == TRUE)
  {
    goto exit;
  }

  soc_pa_regs_init();
#ifdef LINK_PA_LIBRARIES
  Soc_petra_regs.a_regs = &Soc_petra_a_regs;
#else
  Soc_petra_regs.invalid_device_reg = &Soc_petra_a_regs;
#endif

  soc_pb_regs_init();
#ifdef LINK_PB_LIBRARIES
  Soc_petra_regs.b_regs = soc_pb_regs();
#else
  Soc_petra_regs.invalid_device_reg = &Soc_pb_regs;
#endif

  Soc_petra_regs_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_regs_init",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

