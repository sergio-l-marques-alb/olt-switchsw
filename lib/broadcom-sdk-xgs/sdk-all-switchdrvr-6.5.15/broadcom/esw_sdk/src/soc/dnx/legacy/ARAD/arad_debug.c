#include <soc/mcm/memregs.h>
/* $Id: jer2_arad_debug.c,v 1.26 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/legacy/error.h>


#include <soc/dnx/legacy/drv.h>

#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/Utils/sand_conv.h>

#include <soc/dnx/legacy/SAND/Management/sand_low_level.h>

#include <soc/dnx/legacy/ARAD/arad_debug.h>
#include <soc/dnx/legacy/ARAD/arad_reg_access.h>
#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_chip_defines.h>
#include <soc/dnx/legacy/JER2/jer2_fabric.h>
#include <soc/dnx/legacy/mbcm.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define JER2_ARAD_DEBUG_RST_DOMAIN_MAX                (JER2_ARAD_DBG_NOF_RST_DOMAINS-1)
/*
 *  Polling values for the queue flush
 */

/************************************************************************/
/* Flush Queue register: must be aligned with register database,        */
/* but defined here to minimize access time                             */
/************************************************************************/

/*
 *   Manual Queue Operation : 0x0380 :
 *   ips.manual_queue_operation_reg
 */


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
/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in the H file. (search for prototype)
*********************************************************************/

int
jer2_arad_dbg_egress_shaping_enable_set(
   DNX_SAND_IN  int                 unit,
   DNX_SAND_IN  uint8                 enable
   )
{
    uint32 fld_val;
    soc_reg_above_64_val_t reg_val;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    fld_val = DNX_SAND_BOOL2NUM(enable);

    SOC_DNX_CORES_ITER(SOC_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, core, 0, reg_val));

        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf, fld_val);
        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, QPAIR_SPR_ENAf, fld_val);
        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, TCG_SPR_ENAf, fld_val);

        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, core, 0, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

int
jer2_arad_dbg_egress_shaping_enable_get(
   DNX_SAND_IN  int                 unit,
   DNX_SAND_OUT uint8                 *enable
   )
{
    uint32 fld_val;
    soc_reg_above_64_val_t  reg_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, 0, 0, reg_val));
    fld_val = soc_reg_above_64_field32_get(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf);

    *enable = DNX_SAND_NUM2BOOL(fld_val);

exit:
    SHR_FUNC_EXIT;
}

