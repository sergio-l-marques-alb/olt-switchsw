/* $Id: petra_interrupt_service.h,v 1.8 Broadcom SDK $
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


#ifndef __SOC_PETRA_INTERRUPT_SERVICE_INCLUDED__
/* { */
#define __SOC_PETRA_INTERRUPT_SERVICE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_api_interrupt_service.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

typedef uint32 (*SOC_PETRA_INTERRUPT_SINGLE_FUNC) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE  cause, SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT*) ;

typedef struct
{
  /*
   *
   */
  uint8        valid;
  /*
   * The interrupt registers in the device.
   */
  SOC_PETRA_REG_FIELD  *interrupt_fld;
  /*
   * The mask registers in the device.
   */
  SOC_PETRA_REG_FIELD  *interrupt_mask_fld;
  /*
   *
   */
  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause[SOC_SAND_MAX_DEVICE];
  /*
   *
   */
  SOC_PETRA_INTERRUPT_SINGLE_FUNC clear_func;

} SOC_PETRA_INTERRUPT_FIELD;

typedef struct
{
  /*
   *
   */
  uint8                         valid;
  /*
   * The interrupt registers in the device.
   */
  SOC_PETRA_REG_ADDR                   *interrupt_reg;
  /*
   * The interrupt default clear value;
   */
  uint32                         interrupt_reg_def_clear_val;
  /*
   * The mask registers in the device.
   */
  SOC_PETRA_REG_ADDR                   *interrupt_mask_reg;
  /*
   *
   */
  uint32                        nof_fields;
  /*
   *
   */
  SOC_PETRA_INTERRUPT_FIELD            int_fields[SOC_PETRA_INTERRUPT_MAX_NOF_FIELD_IN_REG];

} SOC_PETRA_INTERRUPT_REG;

typedef struct
{
  /*
   *
   */
  uint8                         valid;
  /*
   *
   */
  uint32                        nof_instances;
  /*
   *
   */
  uint32                        nof_regs;
  /*
   *
   */
  SOC_PETRA_INTERRUPT_REG              int_regs[SOC_PETRA_INTERRUPT_MAX_NOF_REG_IN_MOD];
  /*
   *
   */
  const char                       *module_name;

} SOC_PETRA_INTERRUPT_MODULE_REG;

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
  soc_petra_interrupt_module_initialize(void);

uint32
  soc_petra_interrupt_init(
    SOC_SAND_IN  int                 unit
  );

uint32
  soc_petra_interrupt_info_get(
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_MODULE_REG       **interrupt_regs
  );

uint32
  soc_petra_interrupt_all_interrupts_and_indications_clear_unsafe(
    SOC_SAND_IN  int                        unit
  );

SOC_PETRA_INTERRUPT_MONITORED_CAUSE
  soc_petra_interrupt_cause2monitored_cause(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause
  );

uint32
  soc_petra_interrupt_unmask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_tcm
  );

uint32
  soc_petra_interrupt_mask_all_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_isr
  );

uint32
  soc_petra_interrupt_specific_cause_mask_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause
  );

uint32
  soc_petra_interrupt_specific_cause_unmask_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause
  );

uint32
  soc_petra_interrupt_mask_clear_unsafe(
    SOC_SAND_IN  int                        unit
  );

uint32
  soc_petra_interrupt_mask_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint32                         *bit_stream
  );

uint32
  soc_petra_interrupt_level_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint8                        *soc_petra_int
  );

uint32
  soc_petra_interrupt_specific_cause_monitor_start_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE            cause,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_MONITORED_CAUSE  *handle
  );

uint32
  soc_petra_interrupt_specific_cause_monitor_stop_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  handle
  );

uint32
  soc_petra_interrupt_all_specific_cause_monitor_stop_unsafe(
    SOC_SAND_IN  int                        unit
  );

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  cause,
    SOC_SAND_IN  uint8                        indicator
  );

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  cause,
    SOC_SAND_OUT uint8                        *indicator
  );

uint32
  soc_petra_interrupt_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT uint32                         *bit_stream
  );

uint32
  soc_petra_interrupt_single_clear_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  cause_ptr_i,
    SOC_SAND_OUT SOC_PETRA_INTERRUPT_SINGLE_STRUCT    *value
  );

uint32
  soc_petra_interrupt_single_clear_with_instance_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  monitored_cause,
    SOC_SAND_IN  uint8                          instance,
    SOC_SAND_OUT uint32                         *val_before_clear
  );

uint32
  soc_petra_interrupt_clear_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         *bit_stream
  );

uint32
  soc_petra_interrupt_initial_mask_lift(
    SOC_SAND_IN  int                        unit
  );

uint8
  soc_petra_interrupt_is_bit_autoclear(
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  cause
  );

uint8
  soc_petra_interrupt_is_masked_get(
    SOC_SAND_IN  int                        unit
  );

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  cause,
    SOC_SAND_IN  uint8                        indicator
  );

uint32
  soc_petra_interrupt_monitored_cause_mask_bit_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_MONITORED_CAUSE  cause,
    SOC_SAND_OUT uint8                        *indicator
  );

uint32
  soc_petra_interrupt_monitored_cause_nof_instances_get(
    SOC_SAND_IN   int                         unit,
    SOC_SAND_IN   SOC_PETRA_INTERRUPT_MONITORED_CAUSE      cause,
    SOC_SAND_OUT  uint8                           *nof_instances
  );

#if SOC_PETRA_DEBUG

uint32
  soc_petra_interrupt_all_interrupts_print_unsafe(
    SOC_SAND_IN  int                        unit
  );

#endif
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_INTERRUPT_SERVICE_INCLUDED__*/
#endif
