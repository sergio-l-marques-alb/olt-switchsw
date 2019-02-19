/* $Id: pb_sim_main.c,v 1.11 Broadcom SDK $
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

/* we need this junk function only to avoid building error of pedantic compilation */
void ___junk_function_pb_sim_main(void){
}
#ifdef SAND_LOW_LEVEL_SIMULATION

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <sim/dpp/ChipSim/chip_sim_task.h>
#include <sim/dpp/ChipSim/chip_sim_command.h>

#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/petra_chip_defines.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_error_defs.h>
#include <appl/diag/dpp/utils_host_board.h>
#include <appl/diag/dpp/utils_dffs_cpu_mez.h>
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/tasks_info.h>

#include  <appl/dpp/UserInterface/ui_pure_defi.h>

#if !DUNE_BCM

#include <stdlib.h>

#include "SAND/Management/include/sand_module_management.h"
#include "SAND/Management/include/sand_error_code.h"
#include "SAND/Management/include/sand_low_level.h"
#include "SAND/Management/include/sand_chip_descriptors.h"

#include "SAND/SAND_FM/include/sand_mem_access.h"

#include "SAND/Utils/include/sand_os_interface.h"
#include "SAND/Utils/include/sand_64cnt.h"
#include "SAND/Utils/include/sand_bitstream.h"

#include "DuneDriver/Petra/PB_TM/include/pb_chip_regs.h"
#include "DuneDriver/Petra/include/petra_chip_defines.h"
#include "DuneDriver/Petra/include/petra_api_reg_access.h"
#include "DuneDriver/Petra/include/petra_reg_access.h"
#include "DuneDriver/Petra/include/petra_serdes_regs.h"

#include "DuneDriver/Petra/PB_TM/include/pb_api_mgmt.h"
#include "DuneDriver/Petra/include/petra_api_mgmt.h"

#include "DuneDriver/Petra/PB_TM/include/pb_reg_access.h"
#include "DuneDriver/Petra/PB_TM/include/pb_tbl_access.h"
#include "DuneDriver/Petra/PB_TM/include/pb_pp_tbl_access.h"
#include "DuneDriver/Petra/PB_PP/include/pb_pp_mgmt.h"
#include "DuneDriver/Petra/include/petra_tbl_access.h"

#include "DuneDriver/Petra/PB_PP/include/pb_pp_sw_db.h"

#include "DuneDriver/SAND/Management/include/sand_device_management.h"
#include "DuneDriver/SAND/SAND_FM/include/sand_indirect_access.h"

#include "fe200_chip_defines.h"
#include "Petra/PB_TM/include/pb_pp_chip_regs.h"

#include "ChipSim/include/chip_sim_task.h"
#include "ChipSim/include/chip_sim_command.h"

#include "Utilities/include/utils_board_general.h"
#include "Utilities/include/utils_aux_input.h"
#include "dune_driver_pc_test.h"
#include "pb_sim_main.h"
/* $Id: pb_sim_main.c,v 1.11 Broadcom SDK $
#include "CSR/include/csr_client_sand.h"
#include "ChipSim/include/chip_sim_hw_simulator.h"

#include "Utilities/include/utils_board_general.h"
#include "Utilities/include/utils_line_TGS.h"
#include "Utilities/include/utils_ip_mgmt.h"

#include "UserInterface/include/ui_consts.h"

#include "ATM/include/atm_eng.h"
#include "ATM/PB_PP/include/pb_pp_atm_cfg_api.h"
*/


#include "Pub/include/ui_defx.h"

#else


uint32
  get_time_in_microseconds(
    void
    )
{
  uint32
    seconds,
    nano_seconds;

  soc_sand_os_get_time(&seconds, &nano_seconds);
  return seconds*1000 + nano_seconds/1000000;
}
#endif /* !DUNE_BCM */

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#if !DUNE_BCM
#else
/*
 * Definitions related to 'version' register.
 */
#define FE200_CHIP_TYPE_MS_BIT     23
#define FE200_CHIP_TYPE_NUM_BITS   20
#define FE200_CHIP_TYPE_LS_BIT     (FE200_CHIP_TYPE_MS_BIT + 1 - FE200_CHIP_TYPE_NUM_BITS)
#define FE200_CHIP_TYPE_MASK   \
    (((unsigned long)SOC_SAND_BIT(FE200_CHIP_TYPE_MS_BIT) - SOC_SAND_BIT(FE200_CHIP_TYPE_LS_BIT)) + SOC_SAND_BIT(FE200_CHIP_TYPE_MS_BIT))
#define FE200_CHIP_TYPE_SHIFT      FE200_CHIP_TYPE_LS_BIT

#endif

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

uint32
  Soc_pb_reg_buffer[CHIP_SIM_NOF_CHIPS][SOC_PB_TOTAL_SIZE_OF_REGS] ;


const uint32 Soc_pb_sim_regs_srd_offset[SOC_PETRA_REGS_SRD_NOF_INSTANCES] =
{
  0x5100 * sizeof(uint32), /* NIF A */
  0x5500 * sizeof(uint32), /* NIF B */
  0x4900 * sizeof(uint32), /* MSW A */
  0x4980 * sizeof(uint32)/* MSW B */
};

#define SOC_PB_1_BASE_ADDR (*(Soc_pb_reg_buffer + 0))
#define SOC_PB_2_BASE_ADDR (*(Soc_pb_reg_buffer + 1))

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

extern uint32
  soc_pb_init_constant_values(
    int unit,
    uint32 load_drc_conf
  );

extern void
  init_ui_module(
    void
  );

const
  DESC_FE_REG
    Desc_pb_reg[]
   =
{
  {
    FE_CHIP_TYPE_FIELD,
    FE_VERSION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
#if !DUNE_BCM
    SOC_SAND_OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.version),
#else
    0,
#endif
    FE200_CHIP_TYPE_MASK,
    FE200_CHIP_TYPE_SHIFT,
    "FE_CHIP_TYPE_FIELD",
    "Type of chip (FE)",
    "FE_VERSION_REG",
    "Version related information for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },

/*
 * Last element. Do not remove.
 */
  {
    FE_LAST_FIELD
  }
} ;

#define SOC_PB_SIM_GET_ADDR(_base_addr_, _step_, _idx_)  \
  ((_base_addr_) + (_step_)*(_idx_) + \
  ((SOC_SAND_IS_VAL_IN_RANGE(_base_addr_, SOC_PETRA_NIF_MAL_GRP_B_ADDR_MIN, SOC_PETRA_NIF_MAL_GRP_B_ADDR_MAX) \
  && (_idx_ >= 8))?   \
  0x200 : 0))

void
  soc_pb_sim_reg_init_val(
    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field,
    SOC_SAND_IN  uint32          instance_idx,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    offset = 0;

  offset = SOC_PB_SIM_GET_ADDR(field->addr.base, field->addr.step, instance_idx);

  if (SOC_SAND_IS_VAL_IN_RANGE(instance_idx, SOC_PETRA_REGS_SRD_INST_BASE+1, SOC_PETRA_REGS_SRD_INST_LAST-1))
  {
    offset += Soc_pb_sim_regs_srd_offset[instance_idx - SOC_PETRA_REGS_SRD_INST_BASE - 1];
  }

  soc_sand_read_modify_write(
    SOC_PB_1_BASE_ADDR,
    offset,
    field->lsb,
    SOC_SAND_BITS_MASK(field->msb, field->lsb),
    val
  );

  soc_sand_read_modify_write(
    SOC_PB_2_BASE_ADDR,
    offset,
    field->lsb,
    SOC_SAND_BITS_MASK(field->msb, field->lsb),
    val
  );
}


STATIC void
  soc_pb_sim_init_consts()
{
  uint32
    inst,
    idx;
  uint32
    instance_idx;
  SOC_PB_REGS
    *regs = NULL;
  int unit = 0;

  regs = soc_pb_regs();

  for (inst = SOC_PETRA_REGS_SRD_INST_NIFA; inst < SOC_PETRA_REGS_SRD_INST_LAST; inst++)
  {

   soc_pb_sim_reg_init_val(
      &(regs->serdes.srd_epb_rd_reg.valid),
      inst,
      0x1
    );
  }


  soc_pb_sim_reg_init_val(
    &(regs->eci.test_mux_config_reg.core_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->eci.pll_status_reg.ddr_ab_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->eci.pll_status_reg.ddr_ef_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->eci.pll_status_reg.ddr_hs_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->eci.pll_status_reg.ddr_ab_hs_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->eci.pll_status_reg.ddr_ef_hs_lock),
    0,
    0x1
  ); 

  soc_pb_sim_reg_init_val(
    &(regs->eci.test_mux_config_reg.ddr_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->eci.test_mux_config_reg.qdr_lock),
    0,
    0x1
  );

  soc_pb_sim_reg_init_val(
    &(regs->qdr.qdr_init_status_reg.ready),
    0,
    0x1
  );

  for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx)
  {
    soc_pb_sim_reg_init_val(
      &(regs->dpi.dpi_init_status_reg.ready),
      instance_idx,
      0x1
    );
  }

  for (instance_idx = SOC_PETRA_REGS_SRD_INST_NIFA; instance_idx < SOC_PETRA_REGS_SRD_INST_LAST; ++instance_idx)
  {
    for (idx = 0; idx < SOC_PB_SRD_NOF_PER_QRTT_REGS; idx++)
    {
      soc_pb_sim_reg_init_val(
        &(regs->serdes.srd_cmu_stat_reg[idx].cmu_tx_trimming),
        instance_idx,
        0x0
      );
    }
  }
}

void
  init_simulate_pb_device(
    uint32 chip_ver
  )
{
  uint32
    identification,
    version,
    fld_shift,
    fld_mask,
    dbg_version,
    uj,
    *chip_ptr;
  SOC_PB_REGS
    *regs = NULL;

  soc_petra_regs_init();
  soc_pb_pp_regs_init();

#if !DUNE_BCM
  soc_petra_srd_regs_init();
#endif

  regs = soc_pb_regs();

  for (uj = 0 ; uj < CHIP_SIM_NOF_CHIPS ; uj++)
  {
    chip_ptr = (uint32*)(&Soc_pb_reg_buffer[uj]);
    version = 0xF ;

    fld_shift = SOC_PETRA_FLD_SHIFT(regs->eci.version_reg.chip_type);
    fld_mask  = SOC_PETRA_FLD_MASK(regs->eci.version_reg.chip_type);
    version |= SOC_SAND_SET_FLD_IN_PLACE((SOC_PB_EXPECTED_CHIP_TYPE), fld_shift, fld_mask) ;

    fld_shift = SOC_PETRA_FLD_SHIFT(regs->eci.version_reg.dbg_ver);
    fld_mask  = SOC_PETRA_FLD_MASK(regs->eci.version_reg.dbg_ver);
    version |= SOC_SAND_SET_FLD_IN_PLACE(0  ,fld_shift,  fld_mask) ;

    fld_shift = SOC_PETRA_FLD_SHIFT(regs->eci.version_reg.chip_ver);
    fld_mask  = SOC_PETRA_FLD_MASK(regs->eci.version_reg.chip_ver);
    version |= SOC_SAND_SET_FLD_IN_PLACE(SOC_PETRA_EXPECTED_CHIP_VER, fld_shift, fld_mask) ;

    chip_ptr[0] = SOC_SAND_BYTE_SWAP(version);

    identification = 0 ;
    fld_shift = SOC_PETRA_FLD_SHIFT(regs->eci.identification_reg.pipe_id);
    fld_mask  = SOC_PETRA_FLD_MASK(regs->eci.identification_reg.pipe_id);
    identification |= SOC_SAND_SET_FLD_IN_PLACE(254, fld_shift,fld_mask) ;

    chip_ptr[1] = CHIP_SIM_BYTE_SWAP(identification);

    if(SOC_PB_EXPECTED_CHIP_TYPE == chip_ver)
    {
      dbg_version = 0;
      fld_shift = SOC_PETRA_FLD_SHIFT(regs->eci.version_reg.dbg_ver);
      fld_mask  = SOC_PETRA_FLD_MASK(regs->eci.version_reg.dbg_ver);
      dbg_version |= SOC_SAND_SET_FLD_IN_PLACE(SOC_PETRA_EXPECTED_CHIP_VER_01, fld_shift, fld_mask) ;
      chip_ptr[2] = CHIP_SIM_BYTE_SWAP(dbg_version);
    }
  }
}

int
  print_to_screen(
    const char *out_string,
    int  add_cr_lf
  )
{
  cli_out("%s", out_string);
  if (add_cr_lf) {
    cli_out("\n\r");
  }

  return 0;
}

int
  soc_pb_sim_main(
    uint32 chip_ver,
    uint32 run_ui,
    uint32 run_test,
    uint32 csr,
    uint32 hw,
    uint32 run_device_embedded_app
  )
{

  /*
   * Register the device through CHIP_SIM
   */
  uint32
    start_address[CHIP_SIM_NOF_CHIPS],
   res = SOC_SAND_OK;
  uint32
    device_handle[CHIP_SIM_NOF_CHIPS],
#if !DUNE_BCM
    is_already_opened,
#endif
    chip_sim_result;
  char
    chip_sim_msg[80] = "";
  uint32
    err = 0;
/* The stucture SOC_PB_HW_ADJUSTMENTS is very big */
  SOC_PB_HW_ADJUSTMENTS
    *hw_adjustments = NULL;
  SOC_PB_INIT_BASIC_CONF
    basic_conf;
  SOC_PB_MGMT_OPERATION_MODE
    op_mode;
  SOC_PB_PP_MGMT_OPERATION_MODE
    pp_op_mode;
/* The stucture SOC_PB_INIT_PORTS is very big */
  SOC_PB_INIT_PORTS
    *fap_ports = NULL;
  SOC_PB_INIT_OOR
    oor;
  uint32 idx;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_ALLOC(hw_adjustments, SOC_PB_HW_ADJUSTMENTS, 1);
  SOC_PETRA_ALLOC(fap_ports, SOC_PB_INIT_PORTS, 1);
    
  SOC_PB_HW_ADJUSTMENTS_clear(hw_adjustments);
  SOC_PB_INIT_BASIC_CONF_clear(&basic_conf);
  SOC_PB_INIT_PORTS_clear(fap_ports);
  SOC_PB_INIT_OOR_clear(&oor);
  SOC_PB_MGMT_OPERATION_MODE_clear(&op_mode);
  SOC_PB_PP_MGMT_OPERATION_MODE_clear(&pp_op_mode);
  device_handle[0]=0; /*in case of error in chip_sim_start*/
  printf("Enter soc_pb_sim_main()\n\r");
  op_mode.pp_enable = TRUE;

  start_address[0] = (uint32)SOC_PB_1_BASE_ADDR;
  start_address[1] = (uint32)SOC_PB_2_BASE_ADDR;

  /************************************************************************/
  /* START CHIP-SIM                                                       */
  /************************************************************************/
  chip_sim_result = chip_sim_start(
                       CHIP_SIMULATOR_TASK_NAME,
                       CHIP_SIMULATOR_TASK_PRIO,
                       STACK_SIZE,
                       CHIP_SIMULATOR_TASK_ID,
                       SOC_SAND_DEV_PB,
                       chip_ver,
                       start_address,
                       Desc_pb_reg,
                       print_to_screen,
                       get_time_in_microseconds,
                       FALSE
                     ) ;
  cli_out("----> chip sim start returned with 0x%X\n\r", chip_sim_result) ;

  if (chip_sim_result != OK)
  {
    cli_out("chip_sim_start() returned with error\n\r");
    goto exit;
  }
  else
  {
    cli_out("chip_sim_start(): Success\n\r");
  }

  /*
   * Let the simulator start.
   */
  soc_sand_os_task_delay_milisec(200);
  if(chip_sim_task_is_alive() == FALSE)
  {
    cli_out("chip_sim_task_is_alive is FALSE\n\r");
    goto exit;
  }
  else
  {
    cli_out("chip_sim_task_is_alive is TRUE\n\r");
  }

  /*
   * start chip_sim interrupt handling.
   * start chip_sim counter   handling.
   */
  chip_sim_command_en_int(TRUE, chip_sim_msg);
  chip_sim_command_en_counter(TRUE,chip_sim_msg);

  soc_sand_mnt_binary_fraction_test(FALSE);

  soc_pb_regs_init();
  soc_pb_pp_regs_init();

#if !DUNE_BCM
  soc_petra_srd_regs_init();
#endif

  soc_pb_sim_init_consts();
 
  /************************************************************************/
  /* APPLICATION                                                          */
  /************************************************************************/
  if (run_device_embedded_app)
  {
    /*soc_sand_set_print_when_writing(TRUE, TRUE, TRUE);*/

    /*
     *  Sweep
     */
    for(idx = 0; idx < CHIP_SIM_NOF_CHIPS; idx++) {
        device_handle[idx] = 0xffffff;
    }
  }
  else
  {
#if !DUNE_BCM
    /*
     * Minimal application to start the driver
     */
    res = soc_sand_module_open(
            SOC_SAND_MAX_DEVICE,                            /* max devices */
            16,                                         /* system_tick_in_ms */
            70,                                         /* tcm task priority */
            1,                                          /* min_time_between_tcm_activation */
            1,                                          /* TCM mock up interrupts */
            pc_agent_error_handler,
            Pc_agent_error_buffer,
            &is_already_opened
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    cli_out("soc_sand_module_open(): Success\n\r");

    soc_sand_tcm_set_enable_flag(FALSE);

    /*
     * register devices
     */

    cli_out("----> PB registering 2 PB devices:\n\r", TRUE) ;

    SOC_PB_HW_ADJUSTMENTS_clear(hw_adjustments);
    res = soc_pb_register_device(
            (uint32*)start_address[0],
            NULL,
            &device_handle[0]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    cli_out("soc_pb_register_device(): Success\n\r");

    op_mode.ref_clocks_conf.combo_ref_clk =
      op_mode.ref_clocks_conf.fabric_ref_clk =
      op_mode.ref_clocks_conf.nif_ref_clk = SOC_PETRA_MGMT_SRD_REF_CLK_125_00;
    op_mode.tdm_mode = SOC_PETRA_MGMT_TDM_MODE_PACKET;

    res = soc_pb_mgmt_operation_mode_set(
            device_handle[0],
            &op_mode
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      if (op_mode.pp_enable)
      {
        res = soc_pb_pp_mgmt_operation_mode_set(
                device_handle[0],
                &pp_op_mode
              );
         SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }

      res = soc_pb_mgmt_init_sequence_phase1(
            device_handle[0],
            hw_adjustments,
            &basic_conf,
            fap_ports,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    soc_petra_mgmt_system_fap_id_set(
      device_handle[0],
      0
    );
    cli_out("---->\r\n"
            "  1. soc_pb_register_device with handle 0x%X\n\r",device_handle[0]) ;

    res = soc_pb_mgmt_init_sequence_phase2(
            device_handle[0],
            &oor,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    res = soc_petra_mgmt_enable_traffic_set(
            device_handle[0],
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
#else
    cli_out("FIX: no application\n\r");
#endif
  }

  
  soc_sand_tcm_set_enable_flag(FALSE);

exit:
  if(
      (err) ||
      (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
    )
  {
    cli_out("\r\n"
            "****> soc_pb_sim_main FAILED\n\r");
  }
  else
  {
    cli_out("\r\n"
            "\r\n"
            "\r\n"
            "----> soc_pb_sim_main Success\n\r"
            "\r\n"
            "\r\n"
            "\r\n"
            ) ;
  }

  /************************************************************************/
  /* RUN UI                                                               */
  /************************************************************************/
  goto exit_ui;
exit_ui:
  if(run_ui)
  {
#if !DUNE_BCM

    set_aux_port(11100);
    soc_petra_set_default_unit(device_handle[0]);
    init_ui_module();
#endif
    while(1)
    {
      soc_sand_os_task_delay_milisec(200);
      err = 0;
    }
  }

/*  exit(EXIT_SUCCESS);*/
#if !DUNE_BCM
#else
#if defined(INCLUDE_DUNE_UI) && defined (BCM_PETRAB_SUPPORT)
  soc_petra_set_default_unit(device_handle[0]);
#endif /* INCLUDE_DUNE_UI && BCM_PETRAB_SUPPORT */
#endif
  SOC_PETRA_FREE(hw_adjustments);
  SOC_PETRA_FREE(fap_ports);
  return 0;
}

#endif  /* SAND_LOW_LEVEL_SIMULATION */

