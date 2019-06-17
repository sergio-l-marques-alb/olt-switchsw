#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
/* $Id: jer2_jer_ingress_scheduler.c,v 1.96 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INGRESS

/*************
 * INCLUDES  *
 *************/
#include <soc/mem.h>
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>
/* { */
#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_scheduler.h>
#include <soc/mcm/memregs.h>

#include <soc/dnx/legacy/drv.h>

/*************
 * DEFINES   *
 *************/
/* { */

#define JER2_JER_IPT_MC_SLOW_START_TIMER_MAX    (17)
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

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

static uint32
  jer2_jer_ingress_scheduler_regs_init(
    DNX_SAND_IN  int                 unit
  )
{   
  SHR_FUNC_INIT_VARS(unit);

  /* Slow start mechanism for multicast queues */

  /* Disable mechanism by default - core 0 */
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_9_SLOW_START_ENABLE_Nf,  0 ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_10_SLOW_START_ENABLE_Nf,  0 ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_ENABLE_Nf,  0 ));
  /* Disable mechanism by default - core 1 */
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_9_SLOW_START_ENABLE_Nf,  0 ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_10_SLOW_START_ENABLE_Nf,  0 ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_4_SLOW_START_ENABLE_Nf,  0 ));

  /*Configure mask time of slow start phases to max - core 0*/
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  /*Configure mask time of slow start phases to max - core 1*/
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER2_JER_IPT_MC_SLOW_START_TIMER_MAX ));


exit:
    SHR_FUNC_EXIT;
}

/*********************************************************************
* NAME:
*     jer2_jer_ingress_scheduler_init
* FUNCTION:
*     Initialization of the Jericho blocks configured in this module.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
shr_error_e
  jer2_jer_ingress_scheduler_init(
    DNX_SAND_IN  int                 unit
  )
{
  shr_error_e
    res = _SHR_E_NONE;

  SHR_FUNC_INIT_VARS(unit);

  res = jer2_jer_ingress_scheduler_regs_init(
          unit
        );
  SHR_IF_ERR_EXIT(res);

exit:    
    SHR_FUNC_EXIT;
}




#undef BSL_LOG_MODULE

