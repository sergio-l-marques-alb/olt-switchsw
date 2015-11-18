/* $Id: petra_interrupt_service.c,v 1.14 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_api_flow_control.h>
#include <soc/dpp/Petra/petra_interrupt_service.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_defines.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_INTERRUPT_INVALID_CAUSE            SOC_PETRA_INTERRUPT_NOF_CAUSES
#define SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE  SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(field, mask_field, clean_read_func)                   \
          do                                                                                        \
          {                                                                                         \
            module_reg->int_fields[module_reg->nof_fields].valid = TRUE;                            \
            module_reg->int_fields[module_reg->nof_fields].interrupt_fld = field;                   \
            module_reg->int_fields[module_reg->nof_fields].interrupt_mask_fld = mask_field;         \
            module_reg->int_fields[module_reg->nof_fields].clear_func = clean_read_func;            \
            for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)                              \
            {                                                                                       \
              module_reg->int_fields[module_reg->nof_fields].monitored_cause[device_i] = SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES; \
            }                                                                                       \
            module_reg->nof_fields += 1;                                                            \
          } while(0)

#define SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(field, mask_field)                                    \
          do                                                                                        \
          {                                                                                         \
            module_reg->int_fields[module_reg->nof_fields].valid = TRUE;                            \
            module_reg->int_fields[module_reg->nof_fields].interrupt_fld = field;                   \
            module_reg->int_fields[module_reg->nof_fields].interrupt_mask_fld = mask_field;         \
            module_reg->int_fields[module_reg->nof_fields].clear_func = soc_petra_interrupt_gen_clear_write; \
            for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)                              \
            {                                                                                       \
              module_reg->int_fields[module_reg->nof_fields].monitored_cause[device_i] = SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES; \
            }                                                                                       \
            module_reg->nof_fields += 1;                                                            \
          } while(0)

#define SOC_PETRA_INTERRUPT_MODULE_REG_INIT(reg_i, intrpt, intrpt_mask, def_val)                        \
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

#define SOC_PETRA_INTERRUPT_MODULE_INIT(MODULE)                                                         \
          do                                                                                        \
          {                                                                                         \
            module = &(Soc_petra_interrupt_info[SOC_PETRA_INTERRUPT_##MODULE##_ID]);                        \
            module_reg = module->int_regs;                                                          \
            module->valid = TRUE;                                                                   \
            module->nof_instances = SOC_PETRA_BLK_NOF_INSTANCES_##MODULE;                               \
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
  SOC_PETRA_INTERRUPT_MODULE_REG
    Soc_petra_interrupt_info[SOC_PETRA_INTERRUPT_NOF_MODULES];

static
  SOC_PETRA_INTERRUPT_CAUSE
    Soc_petra_interrupt_data[SOC_SAND_MAX_DEVICE][SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES];

static
  uint8
    Soc_petra_interrupt_info_init = FALSE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC void
  soc_petra_PETRA_INTERRUPT_REG_clear(
    SOC_SAND_OUT  SOC_PETRA_INTERRUPT_REG    *info
  )
{
  uint32
    ind;

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INTERRUPT_REG));
  for (ind = 0; ind < SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG; ++ind)
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
  soc_petra_PETRA_INTERRUPT_MODULE_REG_clear(
    SOC_SAND_OUT  SOC_PETRA_INTERRUPT_MODULE_REG    *info
  )
{
  uint32
    ind;

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INTERRUPT_MODULE_REG));
  for (ind = 0; ind < SOC_PETRA_INTERRUPT_MAX_NOF_REG_IN_MOD; ++ind)
  {
    soc_petra_PETRA_INTERRUPT_REG_clear(&info->int_regs[ind]);
  }
  info->module_name = NULL;
  info->nof_instances = 0;
  info->valid = FALSE;
  info->nof_regs = 0;
}

STATIC uint32
  soc_petra_interrupt_gen_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR                   *int_clear_reg,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res;
  uint32
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_gen_clear_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_gen_clear_read_fld(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD                  *int_clear_fld,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res;
  uint32
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_read_fld_unsafe(
          unit,
          int_clear_fld,
          SOC_PETRA_DEFAULT_INSTANCE,
          &value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  data->interrupt_data[0].data = value;
  data->interrupt_data[0].addr = int_clear_fld->addr.base;
  data->nof_interrupts = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_gen_clear_read_fld()",0,0);
}

STATIC uint32
  soc_petra_interrupt_updt_fifo_ovf_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read_fld(unit, SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.updt_fifo_ovf), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_updt_fifo_ovf_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_txdscr_fifo_ovf_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read_fld(unit, SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.txdscr_fifo_ovf), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_txdscr_fifo_ovf_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_pd_fifo_ovf_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read_fld(unit, SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pd_fifo_ovf), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_pd_fifo_ovf_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_dqcq_depth_ovf_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_dqcq_depth_ovf_clear_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_empty_dqcq_write_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read_fld(unit, SOC_PETRA_REG_DB_ACC_REF(regs->ips.empty_dqcq_id_reg.empty_dqcq_id), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_empty_dqcq_write_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_system_port_queue_size_expired_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
 )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read_fld(unit, SOC_PA_REG_DB_ACC_REF(regs->ips.expired_system_port_id_reg.expired_system_port_id), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_system_port_queue_size_expired_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_act_flow_bad_params_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read(unit, SOC_PETRA_REG_DB_ACC_REF(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_act_flow_bad_params_clear_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_shp_flow_bad_params_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read(unit, SOC_PETRA_REG_DB_ACC_REF(regs->sch.last_flow_restart_event_reg.addr), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_shp_flow_bad_params_clear_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_restart_flow_event_clear_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read(unit, SOC_PETRA_REG_DB_ACC_REF(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_restart_flow_event_clear_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_smpthrow_scl_msg_read(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_interrupt_gen_clear_read(unit, SOC_PETRA_REG_DB_ACC_REF(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_smpthrow_scl_msg_read()",0,0);
}

STATIC uint32
  soc_petra_interrupt_gen_clear_write(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *data
  )
{
  uint32
    res;
  uint32
    msb,
    lsb;
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;
  uint32
    ins_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (ins_i = 0; ins_i < Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    lsb = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->lsb;
    msb = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->msb;

    if (Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_write_fld_unsafe(
            unit,
            Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
            ins_i,
            SOC_SAND_BITS_MASK(msb - lsb, 0)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_gen_clear_write()",0,0);
}

uint32
  soc_petra_interrupt_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_INIT);

  regs = soc_petra_regs();

  /* Initialize module to support unmonitored modules */
  SOC_PA_REG_ISET(regs->nif.interrupt_mask_reg, 0xf, 0, 10, exit);
  SOC_PA_REG_ISET(regs->nif.interrupt_mask_reg, 0xf, 1, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_init()",0,0);
}

uint32
  soc_petra_interrupt_module_initialize(void)
{
  uint32
    device_i;
  uint32
    ind;
  SOC_PETRA_INTERRUPT_MODULE_REG
    *module = NULL;
  SOC_PETRA_INTERRUPT_REG
    *module_reg = NULL;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_MODULE_INITIALIZE);

  if (Soc_petra_interrupt_info_init)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /* Module interrupts { */
  for (ind = 0; ind < SOC_PETRA_INTERRUPT_NOF_MODULES; ++ind)
  {
    soc_petra_PETRA_INTERRUPT_MODULE_REG_clear(&(Soc_petra_interrupt_info[ind]));
  }

  SOC_PETRA_INTERRUPT_MODULE_INIT(OLP);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_read_err), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_read_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_write_err), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_write_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.start_err), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.start_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_read_exp_err), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_read_exp_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.end_write_exp_err), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.end_write_exp_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_reg.start_exp_err), SOC_PA_REG_DB_ACC_REF(regs->olp.interrupt_mask_reg.start_exp_err_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(NIF);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_reg.mal_interrupt[0]), SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.mal_interrupt_mask[0]));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_reg.mal_interrupt[1]), SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.mal_interrupt_mask[1]));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_reg.mal_interrupt[2]), SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.mal_interrupt_mask[2]));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_reg.mal_interrupt[3]), SOC_PA_REG_DB_ACC_REF(regs->nif.interrupt_mask_reg.mal_interrupt_mask[3]));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.srd_lane_interrupt), SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.srd_lane_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.srd_ipu_interrupt), SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.srd_ipu_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_reg.srd_epb_interrupt), SOC_PA_REG_DB_ACC_REF(regs->nif.srd_interrupt_mask_reg.srd_epb_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(2, SOC_PA_REG_DB_ACC_REF(regs->nif.rx_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->nif.rx_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif.rx_interrupt_reg.rx_paeb_interrupt), SOC_PA_REG_DB_ACC_REF(regs->nif.rx_interrupt_mask_reg.rx_paeb_interrupt_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(MAL);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.stat_rx_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.stat_rx_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.link_status_change_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.link_status_change_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_ln_sync_stat_change_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_ln_sync_stat_change_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_sync_stat_change_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_sync_stat_change_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_local_fault_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_local_fault_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.rx_pcs_remote_fault_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.rx_pcs_remote_fault_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.sgmii_got_remote_fault_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.sgmii_got_remote_fault_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_reg.stat_tx_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.stat_tx_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_mlf_overrun_error_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_mlf_overrun_error_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_mlf_overrun_error_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_mlf_overrun_error_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.mac_tx_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.mac_tx_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.mac_rx_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.mac_rx_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_taf_overflow_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_taf_overflow_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_raf_overflow_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_raf_overflow_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_mlf_pkt_too_short_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_mlf_pkt_too_short_int_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.tx_a48_overflow_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.tx_a48_overflow_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_pcs_deskew_err_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_pcs_deskew_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_reg.rx_pcs_deskew_err_fatal_int), SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.err_interrupt_mask_reg.rx_pcs_deskew_err_fatal_int_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(IRE);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.rcy_packet_size_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.rcy_packet_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_err_data_arrived), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_err_data_arrived_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_32bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_32bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_packet_size_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_packet_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.cpu_if_64bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.cpu_if_64bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_err_data_arrived), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_err_data_arrived_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_32bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_32bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_packet_size_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_packet_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.olp_if_64bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.olp_if_64bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.nifa_err_data_arrived), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nifa_err_data_arrived_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.nifa_packet_size_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nifa_packet_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.nifa_64bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nifa_64bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.nifb_err_data_arrived), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nifb_err_data_arrived_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.nifb_packet_size_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nifb_packet_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.nifb_64bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.nifb_64bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.rcy_err_data_arrived), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.rcy_err_data_arrived_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.rcy_64bytes_pack_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.rcy_64bytes_pack_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_not_valid), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_not_valid_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_dif_context), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_dif_context_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_is_sop), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_is_sop_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.burst_err_follow_sop_is_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.burst_err_follow_sop_is_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_reg.fap_port_err), SOC_PA_REG_DB_ACC_REF(regs->ire.interrupt_mask_reg.fap_port_err_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(IDR);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_reassembly), SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_reassembly_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_mini_multicast_recycle), SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_mini_multicast_recycle_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_full_multicast_recycle), SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_full_multicast_recycle_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_timeout), SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_timeout_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_reg.error_unicast_recycle), SOC_PA_REG_DB_ACC_REF(regs->idr.interrupt_register_mask_reg.error_unicast_recycle_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_chunk_status_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_chunk_status_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_chunk_fifo_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_chunk_fifo_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_unicast_dp_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_unicast_dp_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_mini_multicast_dp_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_mini_multicast_dp_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_full_multicast_dp_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_full_multicast_dp_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_free_pcbs_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_free_pcbs_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_pcb_link_table_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_pcb_link_table_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_prm_dp_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_prm_dp_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_prm_pd_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_prm_pd_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_prm_cd_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_prm_cd_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_reassembly_status_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_reassembly_status_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_word_index_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_word_index_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_buffer_type_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_buffer_type_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_reg.error_complete_pc_ecc), SOC_PA_REG_DB_ACC_REF(regs->idr.ecc_interrupt_mask_reg.error_complete_pc_ecc_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(IRR);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.err_max_replication), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.err_max_replication_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_max_replication), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_max_replication_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.out_of_sequence), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.out_of_sequence_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.out_of_synchronization), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.out_of_synchronization_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_free_pcp_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_free_pcp_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_is_free_pcp_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_is_free_pcp_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_pcp_link_table_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_pcp_link_table_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_is_pcp_link_table_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_is_pcp_link_table_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_pcm_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_pcm_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_is_pcm_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_is_pcm_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_uc_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_uc_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_mc_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_mc_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_nif_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_nif_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_glag_mapping_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_glag_mapping_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_reg.error_isf_ecc), SOC_PA_REG_DB_ACC_REF(regs->irr.interrupt_register_mask_reg.error_isf_ecc_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(IQM);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.free_bdb_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.free_bdb_ovf_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.free_bdb_unf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.free_bdb_unf_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.full_uscnt_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.full_uscnt_ovf_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.mini_uscnt_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.mini_uscnt_ovf_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.updt_fifo_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.updt_fifo_ovf_mask), soc_petra_interrupt_updt_fifo_ovf_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.txdscr_fifo_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.txdscr_fifo_ovf_mask), soc_petra_interrupt_txdscr_fifo_ovf_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pd_fifo_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pd_fifo_ovf_mask), soc_petra_interrupt_pd_fifo_ovf_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.qdr_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.qdr_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pd_cd_type_bit_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pd_cd_type_bit_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_rsrc_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_rsrc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_sn_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_sn_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_mc_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_mc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.pkt_enq_qnvalid_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.pkt_enq_qnvalid_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.st_rpt_ovf), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.st_rpt_ovf_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.qroll_over), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.qroll_over_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.vsq_roll_over), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.vsq_roll_over_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_reg.ecc_intr_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.interrupt_mask_reg.ecc_intr_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.taildscr_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.taildscr_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.bdbll_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.bdbll_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.fluscnt_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.fluscnt_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.mnuscnt_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.mnuscnt_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.txpdm_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.txpdm_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.txdscrm_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.txdscrm_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.dbffm_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.dbffm_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.delffm_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.delffm_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqb_qsz_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqb_qsz_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqc_qsz_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqc_qsz_sft_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_reg.vsqd_qsz_sft_err), SOC_PA_REG_DB_ACC_REF(regs->iqm.ecc_interrupt_register_mask_reg.vsqd_qsz_sft_err_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(IPS);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.dqcq_depth_ovf), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.dqcq_depth_ovf_mask), soc_petra_interrupt_dqcq_depth_ovf_clear_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.empty_dqcq_write), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.empty_dqcq_write_mask), soc_petra_interrupt_empty_dqcq_write_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.system_port_queue_size_expired), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.system_port_queue_size_expired_mask), soc_petra_interrupt_system_port_queue_size_expired_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.qdesc_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.qdesc_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.dqcqmem_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.dqcqmem_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_reg.fsmrqmem_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->ips.interrupt_mask_reg.fsmrqmem_ecc_err_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(IPT);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.crc_err_pkt), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.crc_err_pkt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.sop_ecc_error), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.sop_ecc_error_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.mop_ecc_error), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.mop_ecc_error_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.fdtc_ecc_error), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.fdtc_ecc_error_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.egqc_ecc_error), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.egqc_ecc_error_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.bdq_ecc_error), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.bdq_ecc_error_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_reg.pdq_ecc_error), SOC_PA_REG_DB_ACC_REF(regs->ipt.interrupt_mask_reg.pdq_ecc_error_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(MMU);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->mmu.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->mmu.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.interrupt_reg.error_ecc), SOC_PA_REG_DB_ACC_REF(regs->mmu.interrupt_mask_reg.error_ecc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafaa_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafaa_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafab_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafab_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafac_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafac_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafad_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafad_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafae_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafae_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafaf_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafaf_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafba_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafba_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbb_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbb_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbc_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbc_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbd_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbd_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbe_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbe_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wafbf_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wafbf_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafa_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafa_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafb_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafb_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafc_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafc_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafd_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafd_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rafe_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rafe_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.raff_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.raff_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafa_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafa_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafb_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafb_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafc_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafc_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafd_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafd_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfafe_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfafe_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.wfaff_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.wfaff_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafa_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafa_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafb_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafb_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafc_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafc_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafd_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafd_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfafe_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfafe_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.rfaff_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.rfaff_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.idf_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.idf_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_reg.fdf_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->mmu.ecc_interrupt_register_mask_reg.fdf_ecc_err_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(DPI);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->dpi.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->dpi.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->dpi.interrupt_reg.dpi_error_int), SOC_PA_REG_DB_ACC_REF(regs->dpi.interrupt_mask_reg.dpi_error_int_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(FDR);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.addr), 0xfff3fff3);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdradesccnto_a), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdradesccnto_aint_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdraun_exp_cell), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdraun_exp_cell_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmafo_a), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmafo_aint_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmbfo_a), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmbfo_aint_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_a0), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_a0_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_a1), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_a1_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.alto_a), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.alto_intmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_out_of_sync), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_out_of_sync_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_tag_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_tag_ecc_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_filrter_drop_inta), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_filrter_drop_intamask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdra_filrter_drop_intb), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdra_filrter_drop_intbmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdradesccnto_b), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrbdesccnto_bint_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrbun_exp_cell), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrbun_exp_cell_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmafo_b), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmafo_bint_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.ifmbfo_b), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.ifmbfo_bintmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_b0), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_b0_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.cpudatacellfne_b1), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.cpudatacellfne_b1_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.alto_b_), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.alto_bint_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_out_of_sync_), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_out_of_sync_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_tag_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_tag_ecc_err_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_filrter_drop_inta), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_filrter_drop_intamask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdrb_filrter_drop_intb), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdrb_filrter_drop_intbmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdr_outpu_out_of_sync), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdr_outpu_out_of_sync_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_reg.fdr_out_tag_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->fdr.interrupt_mask_reg.fdr_out_tag_ecc_err_int_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(FDT);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->fdt.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fdt.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fdt.interrupt_reg.unrch_dest_evt), SOC_PA_REG_DB_ACC_REF(regs->fdt.interrupt_mask_reg.unrch_dest_evt_int_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(FCR);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.src_dv_cng_link_ev), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.src_dv_cng_link_evint_msk));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.cpucnt_cell_fne), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.cpucnt_cell_fneint_msk));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.local_rout_fs_ovf), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.local_routfs_ovfint_msk));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.local_routrc_ovf), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.local_routrc_ovfint_msk));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.reach_fifo_ovf), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.reach_fifoovfint_msk));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.flow_fifo_ovf), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.flow_fifoovfint_msk));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_reg.credit_fifo_ovf), SOC_PA_REG_DB_ACC_REF(regs->fcr.interrupt_mask_reg.credit_fifoovfint_msk));

  SOC_PETRA_INTERRUPT_MODULE_INIT(FCT);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->fct.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fct.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fct.interrupt_reg.unrch_dest_evt), SOC_PA_REG_DB_ACC_REF(regs->fct.interrupt_mask_reg.unrch_dest_evt_int_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(RTP);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->rtp.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->rtp.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->rtp.interrupt_reg.link_mask_change), SOC_PA_REG_DB_ACC_REF(regs->rtp.interrupt_mask_reg.link_mask_change));

  SOC_PETRA_INTERRUPT_MODULE_INIT(FABRIC_MAC);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_1_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_1_reg.rx_crcerr_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.rx_crcerr_n_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_1_reg.rx_cgerr_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_1_reg.rx_cgerr_n_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_2_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_2_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_2_reg.los_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_2_reg.los_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_2_reg.rx_mis_aerr_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_2_reg.rx_mis_aerr_n_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(2, SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_3_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_3_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_3_reg.rx_disp_err_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_3_reg.rx_disp_err_n_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(3, SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_4_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_4_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_4_reg.lnklvl_age_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_4_reg.lnklvl_age_n_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_4_reg.lnklvl_halt_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_4_reg.lnklvl_halt_n_int_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(4, SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_5_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_5_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_5_reg.wrong_size_n_int), SOC_PA_REG_DB_ACC_REF(regs->fabric_mac.interrupt_mask_5_reg.wrong_size_n_int_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(MSW);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd0_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd0_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd1_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd1_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd2_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd2_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd3_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd3_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_ipu_interrupt_group_a), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_ipu_interrupt_mask_group_a));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_epb_interrupt_group_a), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_epb_interrupt_group_amask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd4_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd4_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd5_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd5_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd6_macro_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd6_macro_interrupt_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_ipu_interrupt_group_b), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_ipu_interrupt_mask_group_b));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_reg.srd_epb_interrupt_group_b), SOC_PA_REG_DB_ACC_REF(regs->msw.interrupt_mask_reg.srd_epb_interrupt_group_bmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_reg.srd_lane_interrupt), SOC_PA_REG_DB_ACC_REF(regs->msw.srd_lane_interrupt_mask_reg.srd_lane_interrupt_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(EGQ);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.cpu_pack32_bytes_err), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cpu_pack32_bytes_err));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.cpu_data_arrived_err), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cpu_data_arrived_err));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.pdm_par_err), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_pdm_par_err));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.plm_par_err), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_plm_par_err));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.packet_aged), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_packet_aged));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.int_vlantable_oor), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.masknt_vlantable_oor));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.int_dif_af), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_int_dif_af));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.cfc_fc_int), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cfc_fc_int));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.nifa_fc_int), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_nifa_fc_int));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.nifb_fc_int), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_nifb_fc_int));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.pkt_reas_int_vec), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_pkt_reas_int_vec));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_reg.cnt_ovf_int_vec), SOC_PA_REG_DB_ACC_REF(regs->egq.interrupt_mask_reg.mask_cnt_ovf_int_vec));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_pkt_size_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_pkt_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_missing_sop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_missing_sop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_frag_num_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_frag_num_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_pkt_crc_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_pkt_crc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_sop_intr_mop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_sop_intr_mop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_fix129_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_fix129_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.vsc_eop_size_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.vsc_eop_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_sonts_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_sonts_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_pkt_size_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_pkt_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_missing_sop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_missing_sop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_frag_num_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_frag_num_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_sop_intr_mop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_sop_intr_mop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_eop_size_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_eop_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.fsc_sequence_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.fsc_sequence_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.csr_pkt_size_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.csr_pkt_size_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.csr_unexpected_eop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.csr_unexpected_eop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.csr_missing_eop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.csr_missing_eop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.csr_sop_and_eop_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.csr_sop_and_eop_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.csr_size_parity_err), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.csr_size_parity_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.rej_buff_sch), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.rej_buff_sch_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.rej_buff_usc), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.rej_buff_usc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.rej_desc_sch), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.rej_desc_sch_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg.rej_desc_usc), SOC_PA_REG_DB_ACC_REF(regs->egq.pkt_reas_int_reg_mask_reg.rej_desc_usc_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(2, SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.buflink_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.buflink_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.buflink_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.buflink_ecc_fix_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.rcnt_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.rcnt_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.rcnt_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.rcnt_ecc_fix_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.rrdm_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.rrdm_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.rrdm_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.rrdm_ecc_fix_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.rpdm_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.rpdm_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg.rpdm_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->egq.ecc_int_reg_mask_reg.rpdm_ecc_fix_mask));


  SOC_PETRA_INTERRUPT_MODULE_INIT(EPN);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_reg.tdm_fabric_stop), SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_tdm_fabric_stop));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_reg.mirr_ovf_int), SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_mirr_ovf));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_reg.int_otm_out_lif_mode), SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_int_otm_out_lif_mode));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_reg.tdm_int_vec), SOC_PA_REG_DB_ACC_REF(regs->epni.interrupt_mask_reg.mask_tdm_int_vec));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(1, SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.prdm_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.prdm_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.prdm_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.prdm_ecc_fixr_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.mdm_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.mdm_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.mdm_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.mdm_ecc_fix_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.nadm_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.nadm_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.nadm_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.nadm_ecc_fix_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.nbdm_ecc_err), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.nbdm_ecc_err_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg.nbdm_ecc_fix), SOC_PA_REG_DB_ACC_REF(regs->epni.ecc_int_reg_mask_reg.nbdm_ecc_fix_mask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(CFC);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.addr), 0xffffffff);
  for (ind = 0; ind < SOC_PETRA_FC_NOF_OOB_IFS; ind++)
  {
    SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_lock_err[ind]), SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrx_lock_err_mask[ind]));
    SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_out_of_frm[ind]), SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrx_out_of_frm_mask[ind]));
    SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_dip2_alarm[ind]), SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrx_dip2_alarm_mask[ind]));
    SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_frm_err[ind]), SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrx_frm_err_mask[ind]));
    SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_reg.obrx_dip2_err[ind]), SOC_PA_REG_DB_ACC_REF(regs->cfc.interrupt_mask_reg.obrx_dip2_err_mask[ind]));
  }

  SOC_PETRA_INTERRUPT_MODULE_INIT(SCH);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.act_flow_bad_params), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.act_flow_bad_params_mask), soc_petra_interrupt_act_flow_bad_params_clear_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.shp_flow_bad_params), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.shp_flow_bad_params_mask), soc_petra_interrupt_shp_flow_bad_params_clear_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.restart_flow_event), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.restart_flow_event_mask), soc_petra_interrupt_restart_flow_event_clear_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_R_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.smpthrow_scl_msg), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.smpthrow_scl_msg_mask), soc_petra_interrupt_smpthrow_scl_msg_read);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.smpfull_level1), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.smpfull_level1_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.smpfull_level2), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.smpfull_level2_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fctfifoovf), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fctfifoovf_mask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.dhdecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.dhdeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.dcdecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.dcdeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.flhhrecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.flhhreccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.flthrecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.flthreccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.flhclecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.flhcleccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fltclecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fltcleccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.flhfqecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.flhfqeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fltfqecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fltfqeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fdmsecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fdmseccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fdmdecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fdmdeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.shdsecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.shdseccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.shddecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.shddeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fqmecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fqmeccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.sflhecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.sflheccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.sfltecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.sflteccmask));
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_reg.fsmecc), SOC_PA_REG_DB_ACC_REF(regs->sch.interrupt_mask_data_reg.fsmeccmask));

  SOC_PETRA_INTERRUPT_MODULE_INIT(QDR);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT(0, SOC_PA_REG_DB_ACC_REF(regs->qdr.interrupt_reg.addr), SOC_PA_REG_DB_ACC_REF(regs->qdr.interrupt_mask_reg.addr), 0xffffffff);
  SOC_PETRA_INTERRUPT_MODULE_REG_INIT_W_FLD(SOC_PA_REG_DB_ACC_REF(regs->qdr.interrupt_reg.qdr_ecc_int), SOC_PA_REG_DB_ACC_REF(regs->qdr.interrupt_mask_reg.qdr_ecc_int_mask));

  /* } Module interrupts */

  /* Interrupt causes { */
  for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)
  {
    for (ind = 0; ind < SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES; ++ind)
    {
      Soc_petra_interrupt_data[device_i][ind] = SOC_PETRA_INTERRUPT_INVALID_CAUSE;
    }
  }

  /* } Interrupt causes */

  Soc_petra_interrupt_info_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_module_initialize()",0,0);
}

uint32
  soc_pa_interrupt_all_interrupts_and_indications_clear_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERRUPT_ALL_INTERRUPTS_AND_INDICATIONS_CLEAR_UNSAFE);

  regs = soc_petra_regs();

  for (idx = 0; idx < SOC_PA_TOTAL_SIZE_OF_REGS; idx += sizeof(uint32))
  {
    if ((SOC_PA_REG_DB_ACC(regs->fdt.cpu_data_cell_reg[0].addr.base) <= idx) &&
        (idx <= SOC_PA_REG_DB_ACC(regs->fdt.cpu_data_cell_reg[SOC_PETRA_NOF_TRANSMIT_DATA_CELLS_REGISTERS - 1].addr.base)))
    {
      continue;
    }
    if ((SOC_PA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.addr.base) <= idx) &&
        (idx <= SOC_PA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.addr.base)))
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

  for (mod_i = 0; mod_i < SOC_PETRA_INTERRUPT_NOF_MODULES; ++mod_i)
  {
    if (Soc_petra_interrupt_info[mod_i].valid)
    {
      for (ins_i = 0; ins_i < Soc_petra_interrupt_info[mod_i].nof_instances; ++ins_i)
      {
        if (Soc_petra_interrupt_info[mod_i].nof_instances == 1)
        {
          ins_i = SOC_PETRA_DEFAULT_INSTANCE;
        }

        for (reg_i = 0; reg_i < Soc_petra_interrupt_info[mod_i].nof_regs; ++reg_i)
        {
          res = soc_petra_write_reg_unsafe(
                  unit,
                  Soc_petra_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg,
                  ins_i,
                  Soc_petra_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg_def_clear_val
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
  SOC_PETRA_FLD_SET(regs->eci.cpu_streaming_if_interrupts_reg.cpuif_read_reg_timeout, 0x1, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_all_interrupts_and_indications_clear_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_all_interrupts_and_indications_clear_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_ALL_INTERRUPTS_AND_INDICATIONS_CLEAR_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(interrupt_all_interrupts_and_indications_clear_unsafe,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_all_interrupts_and_indications_clear_unsafe()",0,0);
}

SOC_PETRA_INTERRUPT_MONITORED_CAUSE
  soc_petra_interrupt_cause2monitored_cause(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause
  )
{
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  if (cause >= SOC_PETRA_INTERRUPT_NOF_CAUSES)
  {
    monitored_cause = SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE;
  }
  else
  {
    monitored_cause = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].monitored_cause[unit];;
  }

  return monitored_cause;
}

uint8
  soc_petra_interrupt_is_bit_autoclear(
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE monitored_cause
  )
{
  return FALSE;
}

uint8
  soc_petra_interrupt_is_masked_get(
    SOC_SAND_IN  int                        unit
  )
{
  return soc_petra_sw_db_interrupt_mask_on_get(unit);
}

uint32
  soc_petra_interrupt_info_get(
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_MODULE_REG **interrupt_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_INFO_GET);

  SOC_SAND_CHECK_NULL_INPUT(interrupt_regs);

  *interrupt_regs = Soc_petra_interrupt_info;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_info_get()",0,0);
}

uint32
  soc_pa_interrupt_mask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_isr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERRUPT_MASK_ALL_UNSAFE);

  /*
   * Handle the level/counter
   */
  if(soc_sand_device_is_between_isr_to_tcm(unit))
  {
    if (is_isr)
    {
      /* interrupt happens twice - before we were able to unmask it */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_DEVICE_BETWEEN_ISR_TO_TCM_ERR, 10, exit);
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
  SOC_PETRA_REG_SET(regs->eci.mask_all_interrupts_reg, 0x0, 10, exit);

  soc_petra_sw_db_interrupt_mask_on_set(unit, TRUE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_interrupt_mask_all_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_mask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_isr
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_MASK_ALL_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(interrupt_mask_all_unsafe,(unit,is_isr));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_mask_all_unsafe()",0,0);
}

uint32
  soc_pa_interrupt_unmask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_tcm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERRUPT_UNMASK_ALL_UNSAFE);

  if (is_tcm)
  {
    if( !soc_sand_device_is_between_isr_to_tcm(unit) )
    {
      /* tcm called unmask without the level being set before (by the int handler) */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_DEVICE_BETWEEN_ISR_TO_TCM_ERR, 10, exit);
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
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 30, exit);
    }
  }

  /* Unmasking the interrupts */
  SOC_PETRA_REG_SET(regs->eci.mask_all_interrupts_reg, 0x1, 40, exit);

  /*
   * Marking that the copy is not valid.
   */
  soc_petra_sw_db_interrupt_mask_on_set(unit, FALSE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_interrupt_unmask_all_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_unmask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_tcm
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_UNMASK_ALL_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(interrupt_unmask_all_unsafe,(unit, is_tcm));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_unmask_all_unsafe()",0,0);
}

uint32
  soc_pa_interrupt_mask_clear_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERRUPT_MASK_CLEAR_UNSAFE);

  soc_petra_sw_db_interrupt_mask_on_set(unit, FALSE);

  for(mod_i = 0; mod_i < SOC_PETRA_INTERRUPT_NOF_MODULES ; ++mod_i)
  {
    if (Soc_petra_interrupt_info[mod_i].valid)
    {
      for (ins_i = 0; ins_i < Soc_petra_interrupt_info[mod_i].nof_instances; ++ins_i)
      {
        if (Soc_petra_interrupt_info[mod_i].nof_instances == 1)
        {
          ins_i = SOC_PETRA_DEFAULT_INSTANCE;
        }

        for (reg_i = 0; reg_i < Soc_petra_interrupt_info[mod_i].nof_regs; ++reg_i)
        {
          res = soc_petra_write_reg_unsafe(
                  unit,
                  Soc_petra_interrupt_info[mod_i].int_regs[reg_i].interrupt_mask_reg,
                  ins_i,
                  0x0
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
      }
    }
  }

  SOC_PETRA_REG_SET(regs->eci.interrupt_block_mask_reg, 0x0, 20, exit);
  SOC_PETRA_REG_SET(regs->eci.interrupt_block_mask_register_cont_reg, 0x0, 21, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_interrupt_mask_clear_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_mask_clear_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_MASK_CLEAR_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(interrupt_mask_clear_unsafe,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_mask_clear_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_is_all_masked_get(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_IS_ALL_MASKED_GET);

  SOC_SAND_CHECK_NULL_INPUT(is_all_masked);

  SOC_PETRA_REG_GET(regs->eci.interrupt_block_mask_reg, *(buffer + 0), 10, exit);

  SOC_PETRA_REG_GET(regs->eci.interrupt_block_mask_register_cont_reg, *(buffer + 1), 11, exit);

  *is_all_masked = SOC_SAND_NUM2BOOL(soc_sand_bitstream_have_one_in_range(buffer, 0, 2 * SOC_SAND_NOF_BITS_IN_UINT32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_is_all_masked_get()",0,0);
}

uint32
  soc_petra_interrupt_specific_cause_monitor_start_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_MONITORED_CAUSE  *monitored_cause
  )
{
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE
    monitored_cause_i;
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_START_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(monitored_cause);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cause, 0, (SOC_PETRA_INTERRUPT_NOF_CAUSES-1), SOC_PETRA_INTERRUPT_ILLEGAL_CAUSE_NUMBER_ERR, 5, exit);
  
  if(!Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid)/*i.e. the interrupt is not exist*/
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_ILLEGAL_CAUSE_NUMBER_ERR, 10, exit);
  }

  for (monitored_cause_i = 0; monitored_cause_i < SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES; ++monitored_cause_i)
  {
    if (Soc_petra_interrupt_data[unit][monitored_cause_i] == SOC_PETRA_INTERRUPT_INVALID_CAUSE)
    {
      Soc_petra_interrupt_data[unit][monitored_cause_i] = cause;
      Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].monitored_cause[unit] = monitored_cause_i;
      *monitored_cause = monitored_cause_i;

      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 10, exit);
    }
  }

  SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_INSUFFICIENT_MEMORY_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_specific_cause_monitor_start_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_specific_cause_monitor_stop_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause
  )
{
  SOC_PETRA_INTERRUPT_CAUSE
    cause = Soc_petra_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP_UNSAFE);

  Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].monitored_cause[unit] = SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE;
  Soc_petra_interrupt_data[unit][monitored_cause] = SOC_PETRA_INTERRUPT_INVALID_CAUSE;
  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_specific_cause_monitor_stop_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_all_specific_cause_monitor_stop_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE
    monitored_cause_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP_UNSAFE);

  for (monitored_cause_i = 0; monitored_cause_i < SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES; ++monitored_cause_i)
  {
    Soc_petra_interrupt_data[unit][monitored_cause_i] = SOC_PETRA_INTERRUPT_INVALID_CAUSE;
  }

  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_specific_cause_monitor_stop_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_IN  uint8                        indicator
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ins_i;
  SOC_PETRA_INTERRUPT_CAUSE
    cause = Soc_petra_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;
  uint32
    msb = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld->msb,
    lsb = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld->lsb,
    value = indicator ? 0x0 : SOC_SAND_BITS_MASK(msb - lsb, 0);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET_UNSAFE);

  for (ins_i = 0; ins_i < Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    if (Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_write_fld_unsafe(
            unit,
            Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld,
            ins_i,
            value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_monitored_cause_mask_bit_set_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT uint8                        *indicator
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    interrupt_register;
  uint32
    ins_i;
  SOC_PETRA_INTERRUPT_CAUSE
    cause = Soc_petra_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(monitored_cause, SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES - 1, SOC_SAND_GEN_ERR, 1, exit);

  *indicator = FALSE;

  if (!Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid)
  {
    goto exit;
  }

  for (ins_i = 0; ins_i < Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    if (Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_read_fld_unsafe(
            unit,
            Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_mask_fld,
            ins_i,
            &interrupt_register
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    *indicator |= (interrupt_register ? TRUE : FALSE);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_monitored_cause_mask_bit_get_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_mask_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint32                         *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE
    monitored_cause_i;
  uint8
    mask_indicator;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_MASK_GET);

  for(monitored_cause_i = 0 ; monitored_cause_i < SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES ; ++monitored_cause_i)
  {
    if (Soc_petra_interrupt_data[unit][monitored_cause_i] == SOC_PETRA_INTERRUPT_INVALID_CAUSE)
    {
      mask_indicator = FALSE;
    }
    else
    {
      /* Call the safe version to disable interrupts */
      res = soc_petra_interrupt_monitored_cause_mask_bit_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_mask_get()",monitored_cause_i,0);
}

uint32
  soc_petra_interrupt_level_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint8                        *soc_petra_int
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    mask[SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32];
  uint32
    interrupt[SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_LEVEL_GET);

  /*
   * 1. read the mirror of the interrupts.
   */
  res = soc_petra_interrupt_mask_get(
          unit,
          mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  /*
   * 2. read the masks of the interrupts.
   */
  res = soc_petra_interrupt_get_unsafe(
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
          SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 33, exit);

  *soc_petra_int = (soc_sand_bitstream_have_one(interrupt, SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32) ? TRUE : FALSE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_level_get()",0,0);
}

uint32
  soc_petra_interrupt_initial_mask_lift(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_INITIAL_MASK_LIFT);

  SOC_SAND_INTERRUPTS_STOP;

  /*
   * If user has  requested real interrupts on this device
   * (See 'soc_tcmmockup_interrupts' parameter in soc_sand_module_open())
   * then enable interrupts on this device by lifting masks.
   */
  if (!soc_sand_general_get_tcm_mockup_interrupts())
  {
    SOC_PETRA_REG_SET(regs->eci.interrupt_block_mask_reg, 0xffffffff, 10, exit);

    SOC_PETRA_REG_SET(regs->eci.interrupt_block_mask_register_cont_reg, 0xffffffff, 11, exit);

    SOC_PETRA_REG_SET(regs->eci.mask_all_interrupts_reg, 0x1, 12, exit);

    soc_petra_sw_db_interrupt_mask_on_set(unit, FALSE);
  }

exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_initial_mask_lift_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_single_clear_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *value
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_INTERRUPT_CAUSE
    cause;
  uint32
    cause_ptr_i_mod,
    cause_ptr_i_reg,
    cause_ptr_i_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SINGLE_INFO_READ);

  if(monitored_cause == SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE_ERR,10,exit);
  }

  cause = Soc_petra_interrupt_data[unit][monitored_cause];
  cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
  cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
  cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  if (Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid)
  {
    res = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].clear_func(unit, cause, value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_petra_interrupt_all_interrupts_and_indications_clear_unsafe(
            unit
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_single_clear_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_single_clear_with_instance_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE   monitored_cause,
    SOC_SAND_IN  uint8                          instance,
    SOC_SAND_OUT uint32                         *val_before_clear
  )
{
  uint32
    res = SOC_SAND_OK,
    val;
  SOC_PETRA_INTERRUPT_CAUSE
    cause = Soc_petra_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;
  uint8 
    msb,
    lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SINGLE_CLEAR_WITH_INSTANCE_UNSAFE);

  lsb = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->lsb;
  msb = Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld->msb;


  if (
        (!Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].valid) ||
        (Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
      )
  {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INT_INSTANCE_CLEAR_NOT_APPLICABLE_ERR, 20, exit);
  }

  res = soc_petra_read_fld_unsafe(
          unit,
          Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
          instance,
          &val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  
  if (val)
  {
    res = soc_petra_write_fld_unsafe(
            unit,
            Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
            instance,
            SOC_SAND_BITS_MASK(msb - lsb, 0)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  *val_before_clear = val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_single_clear_with_instance_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_clear_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  SOC_PETRA_INTERRUPT_SINGLE_STRUCT
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_INFO_READ);

  for (monitored_cause = 0; monitored_cause < SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES; ++monitored_cause)
  {
    if(soc_sand_bitstream_test_bit(bit_stream, monitored_cause))
    {
      res = soc_petra_interrupt_single_clear_unsafe(
              unit,
              monitored_cause,
              &value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_clear_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_single_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT uint8                        *indicator
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    interrupt_register;
  uint32
    ins_i;
  SOC_PETRA_INTERRUPT_CAUSE
    cause = Soc_petra_interrupt_data[unit][monitored_cause];
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD),
    cause_ptr_i_reg = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG,
    cause_ptr_i_fld = (cause % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD) % SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SINGLE_GET);

  SOC_SAND_ERR_IF_ABOVE_MAX(monitored_cause, SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES - 1, SOC_SAND_GEN_ERR, 1, exit);

  *indicator = FALSE;

  if (Soc_petra_interrupt_data[unit][monitored_cause] == SOC_PETRA_INTERRUPT_INVALID_CAUSE)
  {
    goto exit;
  }

  for (ins_i = 0; ins_i < Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances; ++ins_i)
  {
    if (Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances == 1)
    {
      ins_i = SOC_PETRA_DEFAULT_INSTANCE;
    }

    res = soc_petra_read_fld_unsafe(
            unit,
            Soc_petra_interrupt_info[cause_ptr_i_mod].int_regs[cause_ptr_i_reg].int_fields[cause_ptr_i_fld].interrupt_fld,
            ins_i,
            &interrupt_register
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    *indicator |= (interrupt_register ? TRUE : FALSE);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_single_get_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint32                         *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  uint8
    is_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_GET);

  /*
   * First of all lets clear the bit stream
   */
  ret = soc_sand_bitstream_clear(
          bit_stream,
          SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES_IN_UINT32
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 5, exit);

  for (monitored_cause = 0; monitored_cause < SOC_PETRA_INTERRUPT_MAX_MONITORED_CAUSES ; ++monitored_cause)
  {
    res = soc_petra_interrupt_single_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_initial_mask_lift()",0,0);
}

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_IN  uint8                        indicator
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_interrupt_monitored_cause_mask_bit_set_unsafe(
          unit,
          monitored_cause,
          indicator
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_monitored_cause_mask_bit_set()",0,0);
}

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_OUT uint8                       *indicator
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_interrupt_monitored_cause_mask_bit_get_unsafe(
          unit,
          monitored_cause,
          indicator
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_monitored_cause_mask_bit_get()",0,0);
}

uint32
  soc_petra_interrupt_monitored_cause_nof_instances_get(
    SOC_SAND_IN   int                         unit,
    SOC_SAND_IN   SOC_PETRA_INTERRUPT_MONITORED_CAUSE      cause,
    SOC_SAND_OUT  uint8                           *nof_instances
  )
{
  uint32
    cause_ptr_i_mod = (cause / SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_MOD);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_NOF_INSTANCES_GET);

  if(cause > SOC_PETRA_INTERRUPT_NOF_CAUSES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
  }
  *nof_instances = (uint8)Soc_petra_interrupt_info[cause_ptr_i_mod].nof_instances;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_monitored_cause_nof_instances_get()",0,0);
}

#if SOC_PETRA_DEBUG

uint32
  soc_pa_interrupt_all_interrupts_print_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERRUPT_ALL_INTERRUPTS_PRINT_UNSAFE);

  for (mod_i = 0; mod_i < SOC_PETRA_INTERRUPT_NOF_MODULES; ++mod_i)
  {
    if (Soc_petra_interrupt_info[mod_i].valid)
    {
      for (ins_i = 0; ins_i < Soc_petra_interrupt_info[mod_i].nof_instances; ++ins_i)
      {
        for (reg_i = 0; reg_i < Soc_petra_interrupt_info[mod_i].nof_regs; ++reg_i)
        {
          res = soc_petra_read_reg_unsafe(
                  unit,
                  Soc_petra_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg,
                  Soc_petra_interrupt_info[mod_i].nof_instances == 1 ? SOC_PETRA_DEFAULT_INSTANCE : ins_i,
                  &value
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          soc_sand_os_printf(
            "%-10s address 0x%04x: 0x%08x\n\r",
            (Soc_petra_interrupt_info[mod_i].module_name),
            (Soc_petra_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg->base + ins_i * (Soc_petra_interrupt_info[mod_i].int_regs[reg_i].interrupt_reg->step)) / (int)sizeof(uint32),
            (value)
          );
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_interrupt_all_interrupts_get_unsafe()",0,0);
}

uint32
  soc_petra_interrupt_all_interrupts_print_unsafe(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_ALL_INTERRUPTS_PRINT_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(interrupt_all_interrupts_print_unsafe,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_all_interrupts_print_unsafe()",0,0);
}

#endif
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
