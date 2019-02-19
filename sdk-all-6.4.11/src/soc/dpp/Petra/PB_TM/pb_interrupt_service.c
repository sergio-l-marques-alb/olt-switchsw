/* $Id: pb_interrupt_service.c,v 1.15 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_defines.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_INTERRUPT_INVALID_CAUSE            SOC_PB_INTERRUPT_NOF_CAUSES
#define SOC_PB_INTERRUPT_INVALID_MONITORED_CAUSE  SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(field, mask_field, clean_read_func)                   \
          do                                                                                        \
          {                                                                                         \
            module_reg->int_fields[module_reg->nof_fields].valid = TRUE;                            \
            module_reg->int_fields[module_reg->nof_fields].interrupt_fld = field;                   \
            module_reg->int_fields[module_reg->nof_fields].interrupt_mask_fld = mask_field;         \
            module_reg->int_fields[module_reg->nof_fields].clear_func = clean_read_func;            \
            for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)                              \
            {                                                                                       \
              module_reg->int_fields[module_reg->nof_fields].monitored_cause[device_i] = SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES; \
            }                                                                                       \
            module_reg->nof_fields += 1;                                                            \
          } while(0)

#define SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(field, mask_field)                                    \
          do                                                                                        \
          {                                                                                         \
            module_reg->int_fields[module_reg->nof_fields].valid = TRUE;                            \
            module_reg->int_fields[module_reg->nof_fields].interrupt_fld = field;                   \
            module_reg->int_fields[module_reg->nof_fields].interrupt_mask_fld = mask_field;         \
            module_reg->int_fields[module_reg->nof_fields].clear_func = soc_pb_interrupt_gen_clear_write; \
            for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)                              \
            {                                                                                       \
              module_reg->int_fields[module_reg->nof_fields].monitored_cause[device_i] = SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES; \
            }                                                                                       \
            module_reg->nof_fields += 1;                                                            \
          } while(0)

#define SOC_PB_INTERRUPT_MODULE_REG_INIT(reg_i, intrpt, intrpt_mask, def_val)                        \
          do                                                                                        \
          {                                                                                         \
            module_reg = &(module->int_regs[reg_i]);                                                \
            module_reg->valid = TRUE;                                                               \
            module_reg->nof_fields = 0;                                                             \
            module_reg->interrupt_reg = intrpt;                                                     \
            module_reg->interrupt_reg_def_clear_val = def_val;                                      \
            module_reg->interrupt_mask_reg = intrpt_mask;                                           \
            module->nof_regs += 1;\
          } while(0)

#define SOC_PB_INTERRUPT_MODULE_INIT(MODULE)                                                         \
          do                                                                                        \
          {                                                                                         \
            module = &(Soc_pb_interrupt_info[SOC_PB_INTERRUPT_##MODULE##_ID]);                        \
            module_reg = module->int_regs;                                                          \
            module->valid = TRUE;                                                                   \
            module->nof_instances = SOC_PB_BLK_NOF_INSTANCES_##MODULE;                               \
            module->nof_regs = 0;                                                                   \
            module->module_name = #MODULE;                                                          \
          } while(0)

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

static
  SOC_PB_INTERRUPT_MODULE_REG
    Soc_pb_interrupt_info[SOC_PB_INTERRUPT_NOF_MODULES];

static
  SOC_PB_INTERRUPT_CAUSE
    Soc_pb_interrupt_data[SOC_SAND_MAX_DEVICE][SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES];

static
  uint8
    Soc_pb_interrupt_info_init = FALSE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC void
  soc_pb_PETRA_INTERRUPT_REG_clear(
    SOC_SAND_OUT  SOC_PB_INTERRUPT_REG    *info
  )
{
  uint32
    ind;

  sal_memset(info, 0x0, sizeof(SOC_PB_INTERRUPT_REG));
  for (ind = 0; ind < SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG; ++ind)
  {
    info->int_fields[ind].clear_func = NULL;
    info->int_fields[ind].interrupt_fld = NULL;
    info->int_fields[ind].interrupt_mask_fld = NULL;
    info->int_fields[ind].valid = FALSE;
  }
  info->interrupt_mask_reg = NULL;
  info->interrupt_reg = NULL;
  info->nof_fields = 0;
  info->valid = FALSE;
}

STATIC void
  soc_pb_PETRA_INTERRUPT_MODULE_REG_clear(
    SOC_SAND_OUT  SOC_PB_INTERRUPT_MODULE_REG    *info
  )
{
  uint32
    ind;

  sal_memset(info, 0x0, sizeof(SOC_PB_INTERRUPT_MODULE_REG));
  for (ind = 0; ind < SOC_PB_INTERRUPT_MAX_NOF_REG_IN_MOD; ++ind)
  {
    soc_pb_PETRA_INTERRUPT_REG_clear(&info->int_regs[ind]);
  }
  info->module_name = NULL;
  info->nof_instances = 0;
  info->valid = FALSE;
  info->nof_regs = 0;
}

STATIC uint32
  soc_pb_interrupt_gen_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR                   *int_clear_reg,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res;
  uint32
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_GEN_CLEAR_READ);

  res = soc_petra_read_reg_unsafe(
          unit,
          int_clear_reg,
          SOC_PETRA_DEFAULT_INSTANCE,
          &value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  data->interrupt_data[0].data = value;
  data->interrupt_data[0].addr = int_clear_reg->base;
  data->nof_interrupts = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_gen_clear_read()",0,0);
}

STATIC uint32
  soc_pb_interrupt_dqcq_depth_ovf_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.addr), data);
}

STATIC uint32
soc_pb_interrupt_queue_entered_del_clear_read(
  SOC_SAND_IN  int                        unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.del_queue_number_reg.del_queue_num.addr), data);
}

STATIC uint32
soc_pb_interrupt_credit_lost_clear_read(
  SOC_SAND_IN  int                     unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.lost_credit_queue_number_reg.lost_cr_queue_num.addr), data);
}
STATIC uint32
soc_pb_interrupt_credit_overflow_clear_read(
  SOC_SAND_IN  int                     unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.del_queue_number_reg.del_queue_num.addr), data);
}
STATIC uint32
  soc_pb_interrupt_empty_dqcq_write_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.empty_dqcq_id_reg.addr), data);
}


STATIC uint32
  soc_pb_interrupt_dq_command_timeout_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.deq_cmd_timeout_queue_num_reg.addr), data);
}

STATIC uint32
soc_pb_interrupt_overflow_dqcq_id_clear_read(
  SOC_SAND_IN  int                        unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.overflow_dqcq_id_reg.overflow_dqcq_id.addr), data);
}
STATIC uint32
soc_pb_interrupt_active_push_queue_num_clear_read(
  SOC_SAND_IN  int                        unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->ips.active_push_queue_id_reg.active_push_queue_num.addr), data);
}

STATIC uint32
soc_pb_interrupt_aunrch_dest_evt_clear_read(
  SOC_SAND_IN  int                        unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest.addr), data);
}

STATIC uint32
soc_pb_interrupt_bunrch_dest_evt_clear_read(
  SOC_SAND_IN  int                        unit,
  SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
  SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->fct.unreachable_destination_reg.unrch_dest.addr), data);
}


STATIC uint32
  soc_pb_interrupt_act_flow_bad_params_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr), data);
}

STATIC uint32
  soc_pb_interrupt_shp_flow_bad_params_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->sch.last_flow_restart_event_reg.addr), data);
}

STATIC uint32
  soc_pb_interrupt_restart_flow_event_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr), data);
}


STATIC uint32
  soc_pb_interrupt_ecc_status_error_event_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  return soc_pb_interrupt_gen_clear_read(unit, SOC_PB_REG_DB_ACC_REF(regs->sch.ecc_status_reg.ecc_error_fields.addr), data);
}

STATIC uint32
  soc_pb_interrupt_gen_clear_write(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res;
  uint32
    msb,
    lsb;
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;
  uint32
    ins_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_GEN_CLEAR_WRITE);

  for (ins_i = 0; ins_i < Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    lsb = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->lsb;
    msb = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->msb;

    if (Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_write_fld_unsafe(
            unit,
            Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
            ins_i,
            SOC_SAND_BITS_MASK(msb - lsb, 0)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_gen_clear_write()",0,0);
}

uint32
  soc_pb_interrupt_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_INIT);

  regs = soc_petra_regs();

  /* Initialize module to support unmonitored modules */
  SOC_PB_REG_ISET(regs->nif.interrupt_reg, 0x3ff, 0, 10, exit);
  SOC_PB_REG_ISET(regs->nif.interrupt_reg, 0x3ff, 1, 20, exit);
  SOC_PB_REG_ISET(regs->nif.interrupt_mask_reg, 0x3ff, 0, 30, exit);
  SOC_PB_REG_ISET(regs->nif.interrupt_mask_reg, 0x3ff, 1, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_init()",0,0);
}

uint32
  soc_pb_interrupt_module_initialize(void)
{
  uint32
    device_i,
    ind,
    reg_ndx;
  SOC_PB_INTERRUPT_MODULE_REG
    *module = NULL;
  SOC_PB_INTERRUPT_REG
    *module_reg = NULL;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_MODULE_INITIALIZE);

  if (Soc_pb_interrupt_info_init)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /* Module interrupts { */
  for (ind = 0; ind < SOC_PB_INTERRUPT_NOF_MODULES; ++ind)
  {
    soc_pb_PETRA_INTERRUPT_MODULE_REG_clear(&(Soc_pb_interrupt_info[ind]));
  }

  SOC_PB_INTERRUPT_MODULE_INIT(OLP);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_read_err), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_read_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_write_err), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_write_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.start_err), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.start_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_read_exp_err), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_read_exp_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_write_exp_err), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_write_exp_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_reg.start_exp_err), SOC_PB_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.start_exp_err_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(NIF);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_reg.mal_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.mal_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_reg.srd_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.srd_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_reg.paeb_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.paeb_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_reg.sync_eth_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.sync_eth_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.srd_lane_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.srd_lane_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.srd_ipu_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.srd_ipu_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.srd_epb_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.srd_epb_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->nif.paeb_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nif.paeb_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif.paeb_interrupt_reg.rx_paeb_err_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif.paeb_interrupt_mask_reg.rx_paeb_err_interrupt_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(MAL);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.link_status_change_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.link_status_change_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_ln_sync_stat_change_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_ln_sync_stat_change_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_local_fault_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_local_fault_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_local_fault_phy_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_local_fault_phy_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_remote_fault_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_remote_fault_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_unknown_fault_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_unknown_fault_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.stat_counter_af_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.stat_counter_af_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.an_restart_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.an_restart_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.an_complete_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.an_complete_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.an_error_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.an_error_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.error_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.error_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_mac_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_mac_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_mac_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_mac_err_int_mask));  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_alnr_ovf_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_alnr_ovf_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_ptp_1588_stamping_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_ptp_1588_stamping_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_ptp_1588_stamping_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_ptp_1588_stamping_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_taf_overflow_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_taf_overflow_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_raf_overflow_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_raf_overflow_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_pcs_deskew_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_pcs_deskew_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_pcs_bom_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_pcs_bom_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_pcs_deskew_err_fatal_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_pcs_deskew_err_fatal_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_mac_bct_crc_err_int), SOC_PB_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_mac_bct_crc_err_int_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(NBI);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.tx_fifo_overflow_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.tx_fifo_overflow_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.rx_fifo_overflow_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.rx_fifo_overflow_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilegal_ilkn_input_from_rx0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilegal_ilkn_input_from_rx0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilegal_ilkn_input_from_rx1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilegal_ilkn_input_from_rx1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.err_bits_from_egq_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.err_bits_from_egq_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.wrong_malg_word_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.wrong_malg_word_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.wrong_egq_word_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.wrong_egq_word_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.wrong_port_from_egq_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.wrong_port_from_egq_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.rx_port_discarded_packet_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.rx_port_discarded_packet_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.rx_num_thrown_eops_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.rx_num_thrown_eops_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.rx_num_thrown_eops_75p_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.rx_num_thrown_eops_75p_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.stat_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.stat_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilkn_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilkn_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.nbi_thrown_bursts_counters0_75p_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.nbi_thrown_bursts_counters0_75p_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.nbi_thrown_bursts_counters1_75p_interrupt), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.nbi_thrown_bursts_counters1_75p_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilkn_tx0_ecc_err0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilkn_tx0_ecc_err0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilkn_tx0_ecc_err1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilkn_tx0_ecc_err1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilkn_tx1_ecc_err0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilkn_tx1_ecc_err0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_reg.ilkn_tx1_ecc_err1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.interrupt_mask_reg.ilkn_tx1_ecc_err1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_read_err_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_read_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_rx_frame_err_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_rx_frame_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_tx_frame_err_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_tx_frame_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_rx_burst_length_overflow_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_rx_burst_length_overflow_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_tx_burst_length_overflow_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_tx_burst_length_overflow_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_cnt75p_port0_31), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_cnt75p_port0_31_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_reg.stat_cnt75p_port32_63), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_interrupt_mask_reg.stat_cnt75p_port32_63_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port0_31_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port0_31_int_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port0_31_interrupt_reg.stat_cnt75p_port0_31_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port0_31_int_mask_reg.stat_cnt75p_port0_31_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(3, SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port32_63_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port32_63_int_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port32_63_interrupt_reg.stat_cnt75p_port32_63_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.stat_cnt75p_port32_63_int_mask_reg.stat_cnt75p_port32_63_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(4, SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.addr), 0xffffffff);
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_status_change_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_rx_port_status_change_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_link_partner_status_change_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_link_partner_status_change_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_align_err_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_rx_port_align_err_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_crc32_lane_err_indication_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_crc32_lane_err_indication_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_synch_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_synch_err_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_framing_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_framing_err_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_bad_framing_type_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_bad_framing_type_err_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_sync_word_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_meta_frame_sync_word_err_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_scrambler_state_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_scrambler_state_err_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_length_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_meta_frame_length_err_lanes_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_repeat_err_lanes_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_lane_meta_frame_repeat_err_lanes_int_mask[reg_ndx]));
  }
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_rx_parity_err_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_rx_parity_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_tx_parity_err_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_tx_parity_err_int_mask));
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_status_err_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_rx_port_status_err_int_mask[reg_ndx]));
  }
  for (reg_ndx = 0; reg_ndx < SOC_PB_REG_NOF_ILKNS; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_reg.ilkn_tx_port_status_err_int[reg_ndx]), SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn_interrupt_mask_reg.ilkn_tx_port_status_err_int_mask[reg_ndx]));
  }
    
  SOC_PB_INTERRUPT_MODULE_REG_INIT(5, SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx0_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx0_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx0_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx0_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx0_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx0_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx0_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx0_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx1_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx1_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx1_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx1_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx1_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx1_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx1_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx1_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx2_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx2_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx2_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx2_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx2_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx2_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx2_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx2_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx3_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx3_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx3_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx3_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx3_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx3_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx3_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx3_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx4_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx4_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx4_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx4_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx4_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx4_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx4_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx4_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx5_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx5_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx5_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx5_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx5_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx5_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx5_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx5_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx6_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx6_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx6_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx6_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx6_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx6_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx6_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx6_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx7_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx7_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx7_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx7_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx7_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx7_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx7_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters0_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx7_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(6, SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx8_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx8_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx8_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx8_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx8_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx8_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx8_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx8_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx9_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx9_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx9_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx9_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx9_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx9_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx9_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx9_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx10_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx10_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx10_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx10_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx10_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx10_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx10_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx10_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx11_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx11_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx11_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx11_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx11_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx11_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx11_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx11_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx12_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx12_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx12_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx12_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx12_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx12_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx12_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx12_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx13_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx13_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx13_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx13_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx13_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx13_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx13_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx13_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx14_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx14_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx14_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx14_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx14_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx14_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx14_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx14_port3_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx15_port0_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx15_port0_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx15_port1_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx15_port1_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx15_port2_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx15_port2_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_reg.num_thrown_bursts_counter_75p_rx15_port3_int), SOC_PB_REG_DB_ACC_REF(regs->nbi.nbi_thrown_bursts_counters1_75p_interrupt_mask_reg.num_thrown_bursts_counter75p_rx15_port3_int_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(IRE);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_err_data_arrived), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_err_data_arrived_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_32bytes_pack_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_32bytes_pack_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_packet_size_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_packet_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_64bytes_pack_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_64bytes_pack_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_err_data_arrived), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_err_data_arrived_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_32bytes_pack_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_32bytes_pack_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_packet_size_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_packet_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_64bytes_pack_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_64bytes_pack_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.nif_packet_size_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nif_packet_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.rcy_err_data_arrived), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.rcy_err_data_arrived_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.rcy_packet_size_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.rcy_packet_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.rcy_64bytes_pack_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.rcy_64bytes_pack_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_not_valid), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_not_valid_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_dif_context), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_dif_context_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_is_sop), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_is_sop_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_is_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_is_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.fap_port_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.fap_port_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.tdm_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.tdm_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_reg.tdm_size_err), SOC_PB_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.tdm_size_err_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(IDR);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_unicast_recycle), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_unicast_recycle_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_mini_multicast_recycle), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_mini_multicast_recycle_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_full_multicast_recycle), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_full_multicast_recycle_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_reassembly), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_reassembly_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_timeout), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_timeout_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_memory0), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_memory0_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_memory1), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_memory1_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_memory2), SOC_PB_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_memory2_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_minimum_original_size), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_minimum_original_size_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_minimum_size), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_minimum_size_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_maximum_original_size), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_maximum_original_size_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_maximum_size), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_maximum_size_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_maximum_dp), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_maximum_dpmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_no_pcb), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_no_pcbmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_no_sop_pcb), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_no_sop_pcbmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_no_dp), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_no_dpmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_unexpected_eop), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_unexpected_eopmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_unexpected_mop), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_unexpected_mopmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_unexpected_sop), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_unexpected_sopmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_general_mop), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_general_mopmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_reg.reassembly_error_general_idle), SOC_PB_REG_DB_ACC_REF(regs->idr.reassembly_interrupt_register_mask_reg.reassembly_error_general_idle_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_0_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register0_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_0_reg.reassembly4_bit_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register0_mask_reg.reassembly4_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_0_reg.ethernet_meter_profiles_bit_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register0_mask_reg.ethernet_meter_profiles_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_0_reg.ethernet_meter_status_bit_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register0_mask_reg.ethernet_meter_status_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_0_reg.context_status_bit_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register0_mask_reg.context_status_bit_error_mask));

  for(reg_ndx = 0; reg_ndx < SOC_PB_IDR_REGS_MEMORY_INTERRUPT_REGISTER_MASK_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT(3 + reg_ndx, SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].addr), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].addr), 0xffffffff);
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].chunk_status_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].chunk_status_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].chunk_fifo_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].chunk_fifo_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].dbuff_pointer_cache01_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].dbuff_pointer_cache01_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].dbuff_pointer_cache23_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].dbuff_pointer_cache23_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].dbuff_pointer_cache45_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].dbuff_pointer_cache45_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].free_pcbs_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].free_pcbs_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].pcb_link_table_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].pcb_link_table_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].dp_reassembly_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].dp_reassembly_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].cd_reassembly_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].cd_reassembly_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].context_status_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].context_status_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].word_index_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].word_index_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].complete_pc_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].complete_pc_ecc_error_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_reg[reg_ndx].mmu_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->idr.memory_interrupt_register_mask_reg[reg_ndx].mmu_ecc_error_mask));
  }

  SOC_PB_INTERRUPT_MODULE_INIT(IRR);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.err_max_replication), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.err_max_replication_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.err_is_max_replication), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.err_is_max_replication_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_resequencer0_out_of_seq), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_resequencer0_out_of_seq_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_resequencer1_out_of_seq), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_resequencer1_out_of_seq_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_resequencer2_out_of_seq), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_resequencer2_out_of_seq_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_resequencer0_out_of_sync), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_resequencer0_out_of_sync_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_resequencer1_out_of_sync), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_resequencer1_out_of_sync_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_resequencer2_out_of_sync), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_resequencer2_out_of_sync_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_memory0), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_memory0_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_memory1), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_memory1_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_memory2), SOC_PB_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_memory2_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.lag_mapping_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.lag_mapping_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.lag_next_member_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.lag_next_member_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.lag_to_lag_range_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.lag_to_lag_range_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.mcdb_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.mcdb_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.irdb_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.irdb_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.destination_table_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.destination_table_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.smooth_division_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.smooth_division_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.snoop_mirror_table0_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.snoop_mirror_table0_bit_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_0_reg.snoop_mirror_table1_bit_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register0_mask_reg.snoop_mirror_table1_bit_error_mask));

  for(reg_ndx = 0; reg_ndx < SOC_PB_IRR_REGS_MEMORY_INTERRUPT_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT(2 + reg_ndx, SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].addr), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].addr), 0xffffffff);
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].free_pcb_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].free_pcb_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].is_free_pcb_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].is_free_pcb_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].is_pc_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].is_pc_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].is_pcb_link_table_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].is_pcb_link_table_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].isf_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].isf_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].mcr_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].mcr_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].pc_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].pc_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].pcb_link_table_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].pcb_link_table_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].rpf_memory_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].rpf_memory_ecc_error_mask_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_reg[reg_ndx].rsq_fifo_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->irr.memory_interrupt_register_mask_reg[reg_ndx].rsq_fifo_ecc_error_mask_mask));
  }

  SOC_PB_INTERRUPT_MODULE_INIT(IHP);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->ihp.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ihp.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.interrupt_reg.isem_interrupt), SOC_PB_REG_DB_ACC_REF(regs->ihp.interrupt_mask_reg.isem_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.interrupt_reg.mact_interrupt), SOC_PB_REG_DB_ACC_REF(regs->ihp.interrupt_mask_reg.mact_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_error_cam_table_full), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_error_cam_table_full_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_error_table_coherency), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_error_table_coherency_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_error_delete_unknown_key), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_error_delete_unknown_key_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_error_reached_max_entry_limit), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_error_reached_max_entry_limit_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_warning_inserted_existing), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_warning_inserted_existing_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_management_unit_failure_valid), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_management_unit_failure_valid_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_reg.isem_management_completed), SOC_PB_REG_DB_ACC_REF(regs->ihp.isem_interrupt_mask_reg.isem_management_completed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_cam_table_full), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_cam_table_full_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_table_coherency), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_table_coherency_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_delete_unknown_key), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_delete_unknown_key_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_reached_max_entry_limit), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_reached_max_entry_limit_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_warning_inserted_existing), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_warning_inserted_existing_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_learn_request_over_static), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_learn_request_over_static_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_warning_learn_over_existing), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_warning_learn_over_existing_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_change_fail_non_exist), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_change_fail_non_exist_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_error_change_request_over_static), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_error_change_request_over_static_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_warning_change_non_exist_from_other), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_warning_change_non_exist_from_other_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_warning_change_non_exist_from_self), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_warning_change_non_exist_from_self_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_management_unit_failure_valid), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_management_unit_failure_valid_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_mngmnt_req_fid_exceed_limit), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_mngmnt_req_fid_exceed_limit_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_mngmnt_req_fid_exceed_limit_static_allowed), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_mngmnt_req_fid_exceed_limit_static_allowed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_mngmnt_req_system_vsi_not_found), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_mngmnt_req_system_vsi_not_found_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_event_ready), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_event_ready_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_event_fifo_event_drop), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_event_fifo_event_drop_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_event_fifo_high_threshold_reached), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_event_fifo_high_threshold_reached_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_reply_ready), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_reply_ready_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_reply_fifo_reply_drop), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_reply_fifo_reply_drop_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_lookup_burst_fifo_drop), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_lookup_burst_fifo_drop_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_age_reached_end_index), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_age_reached_end_index_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_flu_reached_end_index), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_flu_reached_end_index_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_amsg_drop), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_amsg_drop_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_fmsg_drop), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_fmsg_drop_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_lookup_request_contention), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_lookup_request_contention_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_reg.mact_src_or_ll_lookup_on_wrong_cycle), SOC_PB_REG_DB_ACC_REF(regs->ihp.mact_interrupt_mask_reg.mact_src_or_ll_lookup_on_wrong_cycle_mask));


  SOC_PB_INTERRUPT_MODULE_INIT(IHB);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->ihb.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ihb.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ihb.interrupt_reg.invalid_destination_valid), SOC_PB_REG_DB_ACC_REF(regs->ihb.interrupt_mask_reg.invalid_destination_valid_mask));\

  SOC_PB_INTERRUPT_MODULE_INIT(IQM);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.free_bdb_ovf), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.free_bdb_ovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.free_bdb_unf), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.free_bdb_unf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.full_uscnt_ovf), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.full_uscnt_ovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.mini_uscnt_ovf), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.mini_uscnt_ovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.qdr_sft_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.qdr_sft_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_rsrc_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_rsrc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_sn_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_sn_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_mc_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_mc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_qnvalid_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_qnvalid_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.st_rpt_ovf), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.st_rpt_ovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.qroll_over), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.qroll_over_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.vsq_roll_over), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.vsq_roll_over_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.dram_dyn_size_roll_over), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.dram_dyn_size_roll_over_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.cnm_pkt_rjct), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.cnm_pkt_rjct_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.double_cd_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.double_cd_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.mtrpa_packet_size_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.mtrpa_packet_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.mtrpb_packet_size_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.mtrpb_packet_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.crps_intr), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.crps_intr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_reg.ecc_intr_err), SOC_PB_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.ecc_intr_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.pqdmd_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.pqdmd_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.taildscr_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.taildscr_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.bdbll_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.bdbll_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.fluscnt_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.fluscnt_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.mnuscnt_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.mnuscnt_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.txpdm_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.txpdm_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.txdscrm_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.txdscrm_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.dbffm_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.dbffm_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.delffm_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.delffm_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqb_qsz_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqb_qsz_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqc_qsz_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqc_qsz_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqd_qsz_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqd_qsz_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.cpdmd_two_berr), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.cpdmd_two_berr_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.pqdmd_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.pqdmd_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.taildscr_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.taildscr_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.bdbll_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.bdbll_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.fluscnt_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.fluscnt_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.mnuscnt_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.mnuscnt_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.txpdm_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.txpdm_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.txdscrm_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.txdscrm_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.dbffm_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.dbffm_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.delffm_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.delffm_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqb_qsz_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqb_qsz_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqc_qsz_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqc_qsz_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqd_qsz_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqd_qsz_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.cpdmd_one_berr_fixed), SOC_PB_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.cpdmd_one_berr_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_cnt_ovf[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_cnt_ovf_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_invld_ad_acc[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ad_acc_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_invld_ptr_acc[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ptr_acc_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_full[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_full_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_not_empty[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_not_empty_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_direct_rd_when_waiting[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_direct_rd_when_waiting_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_static_rd_when_waiting[0]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_static_rd_when_waiting_mask[0]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_cnt_ovf[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_cnt_ovf_mask[1]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_invld_ad_acc[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ad_acc_mask[1]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_invld_ptr_acc[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ptr_acc_mask[1]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_full[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_full_mask[1]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_not_empty[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_not_empty_mask[1]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_direct_rd_when_waiting[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_direct_rd_when_waiting_mask[1]));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_reg.crps_static_rd_when_waiting[1]), SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_interrupt_register_mask_reg.crps_static_rd_when_waiting_mask[1]));

  SOC_PB_INTERRUPT_MODULE_INIT(QDR);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->qdr.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->qdr.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->qdr.interrupt_reg.qdr_ecc_int), SOC_PB_REG_DB_ACC_REF(regs->qdr.interrupt_mask_reg.qdr_ecc_int_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(IPS);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.dqcq_depth_ovf), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.dqcq_depth_ovf_mask), soc_pb_interrupt_dqcq_depth_ovf_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.empty_dqcq_write), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.empty_dqcq_write_mask), soc_pb_interrupt_empty_dqcq_write_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.queue_entered_del), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.queue_entered_del_mask),soc_pb_interrupt_queue_entered_del_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.credit_lost), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.credit_lost_mask), soc_pb_interrupt_credit_lost_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.credit_overflow), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.credit_overflow_mask),soc_pb_interrupt_credit_overflow_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.deq_command_timeout), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.deq_command_timeout_mask), soc_pb_interrupt_dq_command_timeout_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.dqcq_overflow), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.dqcq_overflow_mask),soc_pb_interrupt_overflow_dqcq_id_clear_read);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.push_queue_active), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.push_queue_active_mask),soc_pb_interrupt_active_push_queue_num_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.qdesc_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.qdesc_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.qdesc_ecc_one_err_fixed), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.qdesc_ecc_one_err_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.dqcqmem_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.dqcqmem_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.dqcqmem_ecc_one_err_fixed), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.dqcqmem_ecc_one_err_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.fsmrqmem_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.fsmrqmem_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_reg.fsmrqmem_ecc_one_err_fixed), SOC_PB_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.fsmrqmem_ecc_one_err_fixed_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(IPT);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.sop_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.sop_ecc_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.mop_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.mop_ecc_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.fdtc_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.fdtc_ecc_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.bdq_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.bdq_ecc_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.pdq_ecc_error), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.pdq_ecc_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.sop_ecc_fixed), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.sop_ecc_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.mop_ecc_fixed), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.mop_ecc_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.fdtc_ecc_fixed), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.fdtc_ecc_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.bdq_ecc_fixed), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.bdq_ecc_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.pdq_ecc_fixed), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.pdq_ecc_fixed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_reg.crc_err_pkt), SOC_PB_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.crc_err_pkt_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(MMU);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->mmu.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->mmu.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.interrupt_reg.error_ecc), SOC_PB_REG_DB_ACC_REF(regs->mmu.interrupt_mask_reg.error_ecc_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafaa_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafaa_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafab_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafab_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafac_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafac_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafad_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafad_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafae_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafae_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafaf_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafaf_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafba_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafba_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbb_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbb_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbc_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbc_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbd_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbd_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbe_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbe_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbf_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbf_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafa_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafa_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafb_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafb_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafc_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafc_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafd_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafd_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafe_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafe_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.raff_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.raff_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafa_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafa_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafb_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafb_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafc_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafc_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafd_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafd_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafe_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafe_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfaff_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfaff_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafa_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafa_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafb_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafb_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafc_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafc_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafd_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafd_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafe_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafe_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfaff_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfaff_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.idf_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.idf_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.fdf_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.fdf_ecc_err_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(DPI);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->dpi.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->dpi.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->dpi.interrupt_reg.dpi_error_int), SOC_PB_REG_DB_ACC_REF(regs->dpi.interrupt_mask_reg.dpi_error_int_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(FDR);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.srtdesccnto_a), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.srtdesccnto_amask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.orgtimeerr_a), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.orgtimeerr_amask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmafo_a), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmafo_amask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmbfo_a), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmbfo_amask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_a0), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_a0_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_a1), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_a1_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.alto_a), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.alto_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_out_of_sync), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_out_of_sync_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_tag_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_tag_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_filrter_dropp_inta), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_filrter_dropp_intamask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_filrter_dropp_intb), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_filrter_dropp_intbmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.srtdesccnto_b), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.srtdesccnto_bmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.orgtimeerr_b), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.orgtimeerr_bmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmafo_b), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmafo_bmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmbfo_b), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmbfo_bmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_b0), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_b0_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_b1), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_b1_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.alto_b_), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.alto_bmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_out_of_sync_), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_out_of_sync_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_tag_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_tag_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_filrter_dropp_inta), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_filrter_dropp_intamask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_filrter_dropp_intb), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_filrter_dropp_intbmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdr_outpu_out_of_sync), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdr_outpu_out_of_sync_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdr_out_tag_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdr_out_tag_ecc_err_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(FDT);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_reg.unrch_dest_evt), SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_mask_reg.unrch_dest_evt_mask),soc_pb_interrupt_aunrch_dest_evt_clear_read);  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_reg.ecc_error), SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_mask_reg.ecc_error_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_reg.ecc_error_fixed), SOC_PB_REG_DB_ACC_REF(regs->fdt.interrupt_mask_reg.ecc_error_fixed_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(FCR);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.src_dv_cng_link_ev), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.src_dv_cng_link_ev_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.cpucnt_cell_fne), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.cpucnt_cell_fnemask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.local_rout_fs_ovf), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.local_routfs_ovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.local_routrc_ovf), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.local_routrc_ovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.reach_fifo_ovf), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.reach_fifoovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.flow_fifo_ovf), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.flow_fifoovf_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_reg.credit_fifo_ovf), SOC_PB_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.credit_fifoovf_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(FCT);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->fct.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fct.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->fct.interrupt_reg.unrch_dest_evt), SOC_PB_REG_DB_ACC_REF(regs->fct.interrupt_mask_reg.unrch_dest_evt_mask),soc_pb_interrupt_bunrch_dest_evt_clear_read);

  SOC_PB_INTERRUPT_MODULE_INIT(RTP);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->rtp.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->rtp.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->rtp.interrupt_reg.link_mask_change), SOC_PB_REG_DB_ACC_REF(regs->rtp.interrupt_mask_reg.link_mask_change_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(FABRIC_MAC);    
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_1_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_1_reg.rx_crcerr_n_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.rx_crcerr_n_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_5_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_5_reg.wrong_size_n_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.wrong_size_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_2_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_2_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_2_reg.los_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_2_reg.los_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_2_reg.rx_mis_aerr_n_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_2_reg.rx_mis_aerr_n_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(3, SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_3_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_3_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_3_reg.lnklvl_age_n_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_3_reg.lnklvl_age_n_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_3_reg.lnklvl_halt_n_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_3_reg.lnklvl_halt_n_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(4, SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_4_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_4_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_4_reg.oof_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_4_reg.oof_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_4_reg.dec_err_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_4_reg.dec_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(5, SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_6_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_5_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_6_reg.transmit_err_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_5_reg.transmit_err_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_6_reg.rx_ctrl_overflow_int), SOC_PB_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_5_reg.rx_ctrl_overflow_int_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(MSW);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd0_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd0_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd1_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd1_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd2_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd2_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd3_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd3_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_ipu_interrupt_group_a), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_ipu_interrupt_mask_group_a));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_epb_interrupt_group_a), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_epb_interrupt_group_amask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd4_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd4_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd5_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd5_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd6_macro_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd6_macro_interrupt_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_ipu_interrupt_group_b), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_ipu_interrupt_mask_group_b));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_epb_interrupt_group_b), SOC_PB_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_epb_interrupt_group_bmask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_reg.srd_lane_interrupt), SOC_PB_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_mask_reg.srd_lane_interrupt_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(EGQ);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.cpu_pack32_bytes_err), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cpu_pack32_bytes_err));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.cpu_data_arrived_err), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cpu_data_arrived_err));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.pdm_par_err), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_pdm_par_err));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.plm_par_err), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_plm_par_err));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.packet_aged), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_packet_aged));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.int_vlantable_oor), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.masknt_vlantable_oor));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.vlan_empty_int), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_vlan_empty_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.int_dif_af), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_int_dif_af));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.cfc_fc_int), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cfc_fc_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.nifa_fc_int), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_nifa_fc_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.nifb_fc_int), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_nifb_fc_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_error_cam_table_full), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_error_cam_table_full_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_error_table_coherency), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_error_table_coherency_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_error_delete_unknown_key), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_error_delete_unknown_key_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_error_reached_max_entry_limit), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_error_reached_max_entry_limit_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_warning_inserted_existing), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_warning_inserted_existing_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_management_unit_failure_valid), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_management_unit_failure_valid_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.svem_management_completed), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.svem_management_completed_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.erpp_error_code_int), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.erpp_error_code_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.erpp_discard_int_vec), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.erpp_discard_int_vec_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.pkt_reas_int_vec), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_pkt_reas_int_vec));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.cnt_ovf_int_vec), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cnt_ovf_int_vec));  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_reg.ecc_err_vec), SOC_PB_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_ecc_err_vec));  
  
  SOC_PB_INTERRUPT_MODULE_REG_INIT(1, SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.cnm_intercept_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.cnm_intercept_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.cfm_trap_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.cfm_trap_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.no_vsi_translation_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.no_vsi_translation_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.dss_stacking_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.dss_stacking_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.lag_multicast_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.lag_multicast_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.exclude_src_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.exclude_src_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.vlan_membership_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.vlan_membership_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.unacceptable_frame_type_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.unacceptable_frame_type_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.src_equal_dest_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.src_equal_dest_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.unknown_da_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.unknown_da_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.split_horizon_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.split_horizon_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.private_vlan_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.private_vlan_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.ttl_scope_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.ttl_scope_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.mtu_violation_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.mtu_violation_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.trill_ttl_zero_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.trill_ttl_zero_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.trill_same_interface_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.trill_same_interface_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.bounce_back_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.bounce_back_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discard_interrupt_reg.illegal_eep_discard), SOC_PB_REG_DB_ACC_REF(regs->egq.erpp_discards_interrupt_register_mask_reg.illegal_eep_discard_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(2, SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_pkt_size_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_pkt_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_missing_sop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_missing_sop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_frag_num_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_frag_num_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_pkt_crc_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_pkt_crc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_sop_intr_mop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_sop_intr_mop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_fix129_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_fix129_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.vsc_eop_size_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.vsc_eop_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_sonts_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_sonts_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_pkt_size_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_pkt_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_missing_sop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_missing_sop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_frag_num_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_frag_num_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_sop_intr_mop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_sop_intr_mop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_eop_size_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_eop_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.fsc_sequence_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.fsc_sequence_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.csr_pkt_size_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.csr_pkt_size_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.csr_unexpected_eop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.csr_unexpected_eop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.csr_missing_eop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.csr_missing_eop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.csr_sop_and_eop_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.csr_sop_and_eop_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.csr_size_parity_err), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.csr_size_parity_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.rej_buff_sch), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.rej_buff_sch_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.rej_buff_usc), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.rej_buff_usc_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.rej_desc_sch), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.rej_desc_sch_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_reg.rej_desc_usc), SOC_PB_REG_DB_ACC_REF(regs->egq.packet_reassembly_interrupt_register_mask_reg.rej_desc_usc_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(3, SOC_PB_REG_DB_ACC_REF(regs->egq.counter_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->egq.counter_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.counter_interrupt_reg.prp_sop_cnt_ovf_int), SOC_PB_REG_DB_ACC_REF(regs->egq.counter_interrupt_register_mask_reg.prp_sop_cnt_ovf_int_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT(4, SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_reg.buflink_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_register_mask_reg.buflink_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_reg.buflink_ecc_fix), SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_register_mask_reg.buflink_ecc_fix_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_reg.rcnt_ecc_err), SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_register_mask_reg.rcnt_ecc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_reg.rcnt_ecc_fix), SOC_PB_REG_DB_ACC_REF(regs->egq.ecc_interrupt_register_mask_reg.rcnt_ecc_fix_mask));  

  SOC_PB_INTERRUPT_MODULE_INIT(EPNI);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.mirr_ovf_int), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_mirr_ovf));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.invalid_mpls_cmd_int), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_invalid_mpls_cmd_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.invalid_ip_tunnel_cmd_int), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_invalid_ip_tunnel_cmd_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.invalid_eth_code_int), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_invalid_eth_code_int));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.esem_int_vec), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_esem_int_vec));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_reg.ecc_err_vec), SOC_PB_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_ecc_err_vec));

  SOC_PB_INTERRUPT_MODULE_INIT(CFC);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.addr), 0xffffffff);
  for (ind = 0; ind < SOC_TMC_FC_NOF_OOB_IDS; ind++)
  {
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_lock_err[ind]), ind == SOC_TMC_FC_OOB_ID_A ? SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxa_lock_err_mask) : SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxb_lock_err_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_out_of_frm[ind]), ind == SOC_TMC_FC_OOB_ID_A ? SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxa_out_of_frm_mask) : SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxb_out_of_frm_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_dip2_alarm[ind]), ind == SOC_TMC_FC_OOB_ID_A ? SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxa_dip2_alarm_mask) : SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxb_dip2_alarm_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_frm_err[ind]), ind == SOC_TMC_FC_OOB_ID_A ? SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxa_frm_err_mask) : SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxb_frm_err_mask));
    SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_dip2_err[ind]), ind == SOC_TMC_FC_OOB_ID_A ? SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxa_dip2_err_mask) : SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrxb_dip2_err_mask));
  }

  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn0_oob_rx_crc_err), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn0_oob_rx_crc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn0_oob_rx_overflow), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn0_oob_rx_overflow_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn0_oob_rx_if_stat_err), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn0_oob_rx_crc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn0_oob_rx_lanes_stat_err), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn0_oob_rx_overflow_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn1_oob_rx_crc_err), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn1_oob_rx_crc_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn1_oob_rx_overflow), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn1_oob_rx_overflow_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn1_oob_rx_if_stat_err), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn1_oob_ox_if_stat_err_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_reg.ilkn1_oob_rx_lanes_stat_err), SOC_PB_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.ilkn1_oob_rx_lane_stat_err_mask));

  SOC_PB_INTERRUPT_MODULE_INIT(SCH);
  SOC_PB_INTERRUPT_MODULE_REG_INIT(0, SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.addr), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.addr), 0xffffffff);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.act_flow_bad_params), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.act_flow_bad_params_mask), soc_pb_interrupt_act_flow_bad_params_clear_read);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.shp_flow_bad_params), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.shp_flow_bad_params_mask), soc_pb_interrupt_shp_flow_bad_params_clear_read);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.restart_flow_event), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.restart_flow_event_mask), soc_pb_interrupt_restart_flow_event_clear_read);
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.smp_bad_msg), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.smp_bad_msg_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.smpfull_level1), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.smpfull_level1_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.smpfull_level2), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.smpfull_level2_mask));
  SOC_PB_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.fctfifoovf), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fctfifoovf_mask));  
  SOC_PB_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_reg.ecc_error), SOC_PB_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.ecc_error_mask), soc_pb_interrupt_ecc_status_error_event_clear_read);  

  /* } Module interrupts */

  /* } Module interrupts */

  /* Interrupt causes { */
  for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)
  {
    for (ind = 0; ind < SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES; ++ind)
    {
      Soc_pb_interrupt_data[device_i][ind] = SOC_PB_INTERRUPT_INVALID_CAUSE;
    }
  }

  /* } Interrupt causes */

  Soc_pb_interrupt_info_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_module_initialize()",0,0);
}

uint32
  soc_pb_interrupt_all_interrupts_and_indications_clear_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    buffer;
  uint32
    ins_i,
    reg_i,
    mod_i;
  SOC_PETRA_REGS
    *regs;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_ALL_INTERURUPTS_AND_INDICATIONS_CLEAR_UNSAFE);

  regs = soc_petra_regs();

  for (idx = 0; idx < SOC_PB_TOTAL_SIZE_OF_REGS; idx += sizeof(uint32))
  {
    if ((SOC_PB_REG_DB_ACC(regs->fdt.cpu_data_cell_reg[0].addr.base) <= idx) &&
        (idx <= SOC_PB_REG_DB_ACC(regs->fdt.cpu_data_cell_reg[SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS - 1].addr.base)))
    {
      continue;
    }
    if ((SOC_PB_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.addr.base) <= idx) &&
        (idx <= SOC_PB_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.addr.base)))
    {
      continue;
    }
    ret = soc_sand_mem_read_unsafe(
          unit,
          &buffer,
          idx,
          sizeof(uint32),
          FALSE
        );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);
  }

  for (mod_i = 0; mod_i < SOC_PB_INTERRUPT_NOF_MODULES; ++mod_i)
  {
    if (Soc_pb_interrupt_info[mod_i].valid)
    {
      for (ins_i = 0; ins_i < Soc_pb_interrupt_info[mod_i].nof_instances; ++ins_i)
      {
        if (Soc_pb_interrupt_info[mod_i].nof_instances == 1)
        {
          ins_i = SOC_PETRA_DEFAULT_INSTANCE;
        }

        for (reg_i = 0; reg_i < Soc_pb_interrupt_info[mod_i].nof_regs; ++reg_i)
        {
          res = soc_petra_write_reg_unsafe(
                  unit,
                  Soc_pb_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg,
                  ins_i,
                  Soc_pb_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg_def_clear_val
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
      }
    }
  }

  /*
   *  Note! we intentionally access all address space here, including undefined
   *  addresses.
   *  This action will rise the CpuifReadRegTimeout interrupt.
   *  The below clears the interrupt
   */
  SOC_PB_FLD_SET(regs->eci.cpu_streaming_if_interrupts_reg.cpuif_read_reg_timeout, 0x1, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_all_interrupts_and_indications_clear_unsafe()",0,0);
}

SOC_PB_INTERRUPT_MONITORED_CAUSE
  soc_pb_interrupt_cause2monitored_cause(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause
  )
{
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  if (cause >= SOC_PB_INTERRUPT_NOF_CAUSES)
  {
    monitored_cause = SOC_PB_INTERRUPT_INVALID_MONITORED_CAUSE;
  }
  else
  {
    monitored_cause = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].monitored_cause[unit];
  }

  return monitored_cause;
}

uint8
  soc_pb_interrupt_is_bit_autoclear(
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE monitored_cause
  )
{
  return FALSE;
}

uint8
  soc_pb_interrupt_is_masked_get(
    SOC_SAND_IN  int                        unit
  )
{
  return soc_petra_sw_db_interrupt_mask_on_get(unit);
}

uint32
  soc_pb_interrupt_info_get(
    SOC_SAND_OUT SOC_PB_INTERRUPT_MODULE_REG **interrupt_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_INFO_GET);

  SOC_SAND_CHECK_NULL_INPUT(interrupt_regs);

  *interrupt_regs = Soc_pb_interrupt_info;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_info_get()",0,0);
}

uint32
  soc_pb_interrupt_mask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_isr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_MASK_ALL_UNSAFE);

  /*
   * Handle the level/counter
   */
  if(soc_sand_device_is_between_isr_to_tcm(unit))
  {
    if (is_isr)
    {
      /* interrupt happens twice - before we were able to unmask it */
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    }
    else /* not ISR - nothing to do (interrupts are masked anyway) */
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 10, exit);
    }
  }
  else
  {
    /* not between ISR and TCM */
    if (is_isr)
    {
      soc_sand_device_set_between_isr_to_tcm(unit, TRUE);
    }
  }

  /*
  * (Write 0 in mask_all bit).
  */
  SOC_PB_REG_SET(regs->eci.mask_all_interrupts_reg, 0x0, 10, exit);

  soc_petra_sw_db_interrupt_mask_on_set(unit, TRUE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_mask_all_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_unmask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_tcm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_UNMASK_ALL_UNSAFE);

  if (is_tcm)
  {
    if( !soc_sand_device_is_between_isr_to_tcm(unit) )
    {
      /* tcm called unmask without the level being set before (by the int handler) */
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    }
    /* it is legal tcm call - so lets take the flag down */
    soc_sand_device_set_between_isr_to_tcm(unit, FALSE);
  }
  else
  {  /* not tcm - which means it's a registered service
      * 2 options - either we are between isr to tcm and then we can do nothing
      * at this point (tcm will unmask later on, when it finishes handling), or
      * we are out of isr-tcm and then we can unmask them back (even without checking,
      * if some of the causes are up - only tcm does that)
      */
    if(soc_sand_device_is_between_isr_to_tcm(unit))
    {
      /*
       * only tcm can unmask at this state, again this is
       * not an error, but we have nothing to do here, but to exit.
       */
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 10, exit);
    }
  }

  /* Unmasking the interrupts */
  SOC_PB_REG_SET(regs->eci.mask_all_interrupts_reg, 0x1, 10, exit);

  /*
   * Marking that the copy is not valid.
   */
  soc_petra_sw_db_interrupt_mask_on_set(unit, FALSE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_unmask_all_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_mask_clear_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ins_i,
    reg_i,
    mod_i;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_MASK_CLEAR_UNSAFE);

  soc_petra_sw_db_interrupt_mask_on_set(unit, FALSE);

  for(mod_i = 0; mod_i < SOC_PB_INTERRUPT_NOF_MODULES ; ++mod_i)
  {
    if (Soc_pb_interrupt_info[mod_i].valid)
    {
      for (ins_i = 0; ins_i < Soc_pb_interrupt_info[mod_i].nof_instances; ++ins_i)
      {
        if (Soc_pb_interrupt_info[mod_i].nof_instances == 1)
        {
          ins_i = SOC_PETRA_DEFAULT_INSTANCE;
        }

        for (reg_i = 0; reg_i < Soc_pb_interrupt_info[mod_i].nof_regs; ++reg_i)
        {
          res = soc_petra_write_reg_unsafe(
                  unit,
                  Soc_pb_interrupt_info[mod_i].int_regs[reg_i].interrupt_mask_reg,
                  ins_i,
                  0x0
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
      }
    }
  }

  SOC_PB_REG_SET(regs->eci.interrupt_block_mask_reg, 0x0, 20, exit);
  SOC_PB_REG_SET(regs->eci.interrupt_block_mask_register_cont_reg, 0x0, 21, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_mask_clear_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_is_all_masked_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint8                        *is_all_masked
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    buffer[2];

  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_IS_ALL_MARKED_GET);

  SOC_SAND_CHECK_NULL_INPUT(is_all_masked);

  SOC_PB_REG_GET(regs->eci.interrupt_block_mask_reg, *(buffer + 0), 10, exit);

  SOC_PB_REG_GET(regs->eci.interrupt_block_mask_register_cont_reg, *(buffer + 1), 11, exit);

  *is_all_masked = SOC_SAND_NUM2BOOL(soc_sand_bitstream_have_one_in_range(buffer, 0, 2 * SOC_SAND_NOF_BITS_IN_UINT32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_is_all_masked_get()",0,0);
}

uint32
  soc_pb_interrupt_specific_cause_monitor_start_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_MONITORED_CAUSE  *monitored_cause
  )
{
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause_i;
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_MONITOR_START_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(monitored_cause);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cause, 0, (SOC_PB_INTERRUPT_NOF_CAUSES-1), SOC_PETRA_INTERRUPT_ILLEGAL_CAUSE_NUMBER_ERR, 5, exit);
  
  if(!Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid) /*i.e. the interrupt does not exist*/
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_ILLEGAL_CAUSE_NUMBER_ERR, 10, exit);
  }

  for (monitored_cause_i = 0; monitored_cause_i < SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES; ++monitored_cause_i)
  {
    if (Soc_pb_interrupt_data[unit][monitored_cause_i] == SOC_PB_INTERRUPT_INVALID_CAUSE)
    {
      Soc_pb_interrupt_data[unit][monitored_cause_i] = cause;
      Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].monitored_cause[unit] = monitored_cause_i;
      *monitored_cause = monitored_cause_i;

      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 10, exit);
    }
  }

  SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_specific_cause_monitor_start_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_specific_cause_monitor_stop_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause
  )
{
  SOC_PB_INTERRUPT_CAUSE
    cause = Soc_pb_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP_UNSAFE);

  Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].monitored_cause[unit] = SOC_PB_INTERRUPT_INVALID_MONITORED_CAUSE;
  Soc_pb_interrupt_data[unit][monitored_cause] = SOC_PB_INTERRUPT_INVALID_CAUSE;
  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_specific_cause_monitor_stop_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_all_specific_cause_monitor_stop_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP_UNSAFE);

  for (monitored_cause_i = 0; monitored_cause_i < SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES; ++monitored_cause_i)
  {
    Soc_pb_interrupt_data[unit][monitored_cause_i] = SOC_PB_INTERRUPT_INVALID_CAUSE;
  }

  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_specific_cause_monitor_stop_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_monitored_cause_mask_bit_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_IN  uint8                        indicator
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ins_i;
  SOC_PB_INTERRUPT_CAUSE
    cause = Soc_pb_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;
  uint32
    msb = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld->msb,
    lsb = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld->lsb,
    value = indicator ? 0x0 : SOC_SAND_BITS_MASK(msb - lsb, 0);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET_UNSAFE);

  for (ins_i = 0; ins_i < Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    if (Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_write_fld_unsafe(
            unit,
            Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld,
            ins_i,
            value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_monitored_cause_mask_bit_set_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_monitored_cause_mask_bit_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT uint8                        *indicator
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    interrupt_register;
  uint32
    ins_i;
  SOC_PB_INTERRUPT_CAUSE
    cause = Soc_pb_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(monitored_cause, SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES - 1, SOC_SAND_GEN_ERR, 1, exit);

  *indicator = FALSE;

  if (!Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid)
  {
    goto exit;
  }

  for (ins_i = 0; ins_i < Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    if (Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_read_fld_unsafe(
            unit,
            Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld,
            ins_i,
            &interrupt_register
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    *indicator |= (interrupt_register ? TRUE : FALSE);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_monitored_cause_mask_bit_get_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_mask_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint32                         *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause_i;
  uint8
    mask_indicator;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_MASK_GET);

  for(monitored_cause_i = 0 ; monitored_cause_i < SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES ; ++monitored_cause_i)
  {
    if (Soc_pb_interrupt_data[unit][monitored_cause_i] == SOC_PB_INTERRUPT_INVALID_CAUSE)
    {
      mask_indicator = FALSE;
    }
    else
    {
      /* Call the safe version to disable interrupts */
      res = soc_pb_interrupt_monitored_cause_mask_bit_get_unsafe(
              unit,
              monitored_cause_i,
              &mask_indicator
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    }

    ret = soc_sand_bitstream_set(
            bit_stream,
            monitored_cause_i,
            (mask_indicator ? 0x1 : 0x0)
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_mask_get()",monitored_cause_i,0);
}

uint32
  soc_pb_interrupt_level_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint8                        *soc_petra_int
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    mask[SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32];
  uint32
    interrupt[SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_LEVEL_GET);

  /*
   * 1. read the mirror of the interrupts.
   */
  res = soc_pb_interrupt_mask_get(
          unit,
          mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  /*
   * 2. read the masks of the interrupts.
   */
  res = soc_pb_interrupt_get_unsafe(
          unit,
          interrupt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  /*
   * 3. AND both above. If any bit is on, the device is in interrupt level.
   */
  ret = soc_sand_bitstream_and(
          interrupt,
          mask,
          SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 33, exit);

  *soc_petra_int = (soc_sand_bitstream_have_one(interrupt, SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32) ? TRUE : FALSE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_level_get()",0,0);
}

uint32
  soc_pb_interrupt_initial_mask_lift(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_INITIAL_MASK_LIFT);

  SOC_SAND_INTERRUPTS_STOP;

  /*
   * If user has  requested real interrupts on this device
   * (See 'soc_tcmmockup_interrupts' parameter in soc_sand_module_open())
   * then enable interrupts on this device by lifting masks.
   */
  if (!soc_sand_general_get_tcm_mockup_interrupts())
  {
    SOC_PB_REG_SET(regs->eci.interrupt_block_mask_reg, 0xffffffff, 10, exit);

    SOC_PB_REG_SET(regs->eci.interrupt_block_mask_register_cont_reg, 0xffffffff, 11, exit);

    SOC_PB_REG_SET(regs->eci.mask_all_interrupts_reg, 0x1, 12, exit);

    soc_petra_sw_db_interrupt_mask_on_set(unit, FALSE);
  }

exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_initial_mask_lift_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_single_clear_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT SOC_PB_INTERRUPT_SINGLE_STRUCT    *value
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_INTERRUPT_CAUSE
    cause;
  uint32
    cause_ptr_i_mod,
    cause_ptr_i_reg,
    cause_ptr_i_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SINGLE_INFO_READ);

  if(monitored_cause == SOC_PB_INTERRUPT_INVALID_MONITORED_CAUSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE_ERR,10,exit);
  }

  cause = Soc_pb_interrupt_data[unit][monitored_cause];
  cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
  cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
  cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  if (Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid)
  {
    res = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].clear_func(unit, cause, value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_pb_interrupt_all_interrupts_and_indications_clear_unsafe(
            unit
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_single_clear_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_single_clear_with_instance_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE     monitored_cause,
    SOC_SAND_IN  uint8                          instance,
    SOC_SAND_OUT uint32                         *val_before_clear
  )
{
  uint32
    res = SOC_SAND_OK,
    val;
  SOC_PB_INTERRUPT_CAUSE
    cause = Soc_pb_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;
  uint8 
    msb,
    lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SINGLE_INFO_READ_WITH_INSTANCE);

  lsb = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->lsb;
  msb = Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->msb;

  if (
        (!Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid) ||
        (Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
      )
  {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INT_INSTANCE_CLEAR_NOT_APPLICABLE_ERR, 20, exit);
  }

  res = soc_petra_read_fld_unsafe(
          unit,
          Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
          instance,
          &val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  
  if (val)
  {
    res = soc_petra_write_fld_unsafe(
            unit,
            Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
            instance,
            SOC_SAND_BITS_MASK(msb - lsb, 0)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  *val_before_clear = val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_single_clear_with_instance_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_clear_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  SOC_PB_INTERRUPT_SINGLE_STRUCT
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_INFO_READ);

  for (monitored_cause = 0; monitored_cause < SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES; ++monitored_cause)
  {
    if(soc_sand_bitstream_test_bit(bit_stream, monitored_cause))
    {
      res = soc_pb_interrupt_single_clear_unsafe(
              unit,
              monitored_cause,
              &value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_clear_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_single_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT uint8                        *indicator
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    interrupt_register;
  uint32
    ins_i;
  SOC_PB_INTERRUPT_CAUSE
    cause = Soc_pb_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SINGLE_GET);

  SOC_SAND_ERR_IF_ABOVE_MAX(monitored_cause, SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES - 1, SOC_SAND_GEN_ERR, 1, exit);

  *indicator = FALSE;

  if (Soc_pb_interrupt_data[unit][monitored_cause] == SOC_PB_INTERRUPT_INVALID_CAUSE)
  {
    goto exit;
  }

  for (ins_i = 0; ins_i < Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    if (Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_read_fld_unsafe(
            unit,
            Soc_pb_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
            ins_i,
            &interrupt_register
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    *indicator |= (interrupt_register ? TRUE : FALSE);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_single_get_unsafe()",0,0);
}

uint32
  soc_pb_interrupt_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint32                         *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  uint8
    is_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_GET);

  /*
   * First of all lets clear the bit stream
   */
  ret = soc_sand_bitstream_clear(
          bit_stream,
          SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 5, exit);

  for (monitored_cause = 0; monitored_cause < SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES ; ++monitored_cause)
  {
    res = soc_pb_interrupt_single_get_unsafe(
            unit,
            monitored_cause,
            &is_set
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Set relevant bit */
    ret = soc_sand_bitstream_set(
            bit_stream,
            monitored_cause,
            (is_set == TRUE ? 0x1 : 0x0)
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_initial_mask_lift()",0,0);
}

uint32
  soc_pb_interrupt_monitored_cause_mask_bit_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_IN  uint8                        indicator
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_pb_interrupt_monitored_cause_mask_bit_set_unsafe(
          unit,
          monitored_cause,
          indicator
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_monitored_cause_mask_bit_set()",0,0);
}

uint32
  soc_pb_interrupt_monitored_cause_mask_bit_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT uint8                       *indicator
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_pb_interrupt_monitored_cause_mask_bit_get_unsafe(
          unit,
          monitored_cause,
          indicator
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_monitored_cause_mask_bit_get()",0,0);
}

uint32
  soc_pb_interrupt_monitored_cause_nof_instances_get(
    SOC_SAND_IN   int                         unit,
    SOC_SAND_IN   SOC_PB_INTERRUPT_MONITORED_CAUSE      cause,
    SOC_SAND_OUT  uint8                           *nof_instances
  )
{
  uint32
    cause_ptr_i_mod = (cause / SOC_PB_INTERRUPT_MAX_NOF_FIELD_IN_MOD);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_SPECIFIC_CAUSE_NOF_INSTANCES_GET);

  if(cause > SOC_PB_INTERRUPT_NOF_CAUSES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
  }
  *nof_instances = (uint8)Soc_pb_interrupt_info[cause_ptr_i_mod].nof_instances;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_monitored_cause_nof_instances_get()",0,0);
}
#if SOC_PB_DEBUG

uint32
  soc_pb_interrupt_all_interrupts_print_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ins_i,
    reg_i,
    mod_i;
  uint32
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_ALL_INTERRUPTS_PRINT_UNSAFE);

  for (mod_i = 0; mod_i < SOC_PB_INTERRUPT_NOF_MODULES; ++mod_i)
  {
    if (Soc_pb_interrupt_info[mod_i].valid)
    {
      for (ins_i = 0; ins_i < Soc_pb_interrupt_info[mod_i].nof_instances; ++ins_i)
      {
        for (reg_i = 0; reg_i < Soc_pb_interrupt_info[mod_i].nof_regs; ++reg_i)
        {
          res = soc_petra_read_reg_unsafe(
                  unit,
                  Soc_pb_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg,
                  Soc_pb_interrupt_info[mod_i].nof_instances == 1 ? SOC_PETRA_DEFAULT_INSTANCE : ins_i,
                  &value
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          soc_sand_os_printf(
            "%-10s address 0x%04x: 0x%08x\n\r",
            (Soc_pb_interrupt_info[mod_i].module_name),
            (Soc_pb_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg->base + ins_i * (Soc_pb_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg->step)) / (int)sizeof(uint32),
            (value)
          );
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interrupt_all_interrupts_get_unsafe()",0,0);
}

#endif
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
