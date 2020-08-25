/* $Id: jer2_tmc_api_tdm.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_tdm.c
*
* MODULE PREFIX:  jer2_tmc
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
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

/*************
 * INCLUDES  *
 *************/
/* { */



#include <shared/bsl.h>


#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/TMC/tmc_api_tdm.h>

#include <soc/dnxc/legacy/error.h>

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

void
  SOC_DNX_TDM_FTMH_OPT_UC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_OPT_UC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_FTMH_OPT_UC));
  info->dest_if = 0;
  info->dest_fap_id = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_OPT_MC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_OPT_MC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_FTMH_OPT_MC));
  info->mc_id = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_STANDARD_UC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_STANDARD_UC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_FTMH_STANDARD_UC));
  info->user_def = 0;
  info->sys_phy_port = 0;
  info->user_def_2 = 0;
  info->dest_fap_port = 0;
  info->dest_fap_id = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_STANDARD_MC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_STANDARD_MC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_FTMH_STANDARD_MC));
  info->user_def = 0;
  info->mc_id = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_FTMH));
  SOC_DNX_TDM_FTMH_OPT_UC_clear(&(info->opt_uc));
  SOC_DNX_TDM_FTMH_OPT_MC_clear(&(info->opt_mc));
  SOC_DNX_TDM_FTMH_STANDARD_UC_clear(&(info->standard_uc));
  SOC_DNX_TDM_FTMH_STANDARD_MC_clear(&(info->standard_mc));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_FTMH_INFO));
  info->action_ing = SOC_DNX_TDM_NOF_ING_ACTIONS;
  SOC_DNX_TDM_FTMH_clear(&(info->ftmh));
  info->action_eg = SOC_DNX_TDM_NOF_EG_ACTIONS;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_MC_STATIC_ROUTE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_MC_STATIC_ROUTE_INFO));
  dnx_sand_u64_clear(&(info->link_bitmap));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_DIRECT_ROUTING_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TDM_DIRECT_ROUTING_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_TDM_DIRECT_ROUTING_INFO));
  dnx_sand_u64_clear(&(info->link_bitmap));
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_TDM_ING_ACTION_to_string(
    DNX_SAND_IN  SOC_DNX_TDM_ING_ACTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_TDM_ING_ACTION_ADD:
    str = "add";
  break;
  case SOC_DNX_TDM_ING_ACTION_NO_CHANGE:
    str = "no_change";
  break;
  case SOC_DNX_TDM_ING_ACTION_CUSTOMER_EMBED:
    str = "customer_embed";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_TDM_EG_ACTION_to_string(
    DNX_SAND_IN  SOC_DNX_TDM_EG_ACTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_TDM_EG_ACTION_REMOVE:
    str = "remove";
  break;
  case SOC_DNX_TDM_EG_ACTION_NO_CHANGE:
    str = "no_change";
  break;
  case SOC_DNX_TDM_EG_ACTION_CUSTOMER_EXTRACT:
    str = "customer_extract";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_TDM_FTMH_OPT_UC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_OPT_UC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "dest_if: %u\n\r"),info->dest_if));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "dest_fap_id: %u\n\r"),info->dest_fap_id));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_OPT_MC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_OPT_MC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "mc_id: %u\n\r"),info->mc_id));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_STANDARD_UC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_STANDARD_UC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "user_def: %u\n\r"),info->user_def));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "sys_phy_port: %u\n\r"),info->sys_phy_port));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_STANDARD_MC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_STANDARD_MC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "user_def: %u\n\r"),info->user_def));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "mc_id: %u\n\r"),info->mc_id));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "\n\ropt_uc:\n\r")));
  SOC_DNX_TDM_FTMH_OPT_UC_print(&(info->opt_uc));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "opt_mc:\n\r")));
  SOC_DNX_TDM_FTMH_OPT_MC_print(&(info->opt_mc));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "standard_uc:\n\r")));
  SOC_DNX_TDM_FTMH_STANDARD_UC_print(&(info->standard_uc));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "standard_mc:\n\r")));
  SOC_DNX_TDM_FTMH_STANDARD_MC_print(&(info->standard_mc));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_TDM_FTMH_INFO_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "action_ing %s "), SOC_DNX_TDM_ING_ACTION_to_string(info->action_ing)));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "ftmh:")));
  SOC_DNX_TDM_FTMH_print(&(info->ftmh));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_mc: %u\n\r"),info->is_mc));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "action_eg %s "), SOC_DNX_TDM_EG_ACTION_to_string(info->action_eg)));
exit:
  SHR_VOID_FUNC_EXIT;
}
void
  SOC_DNX_TDM_MC_STATIC_ROUTE_INFO_print(
    DNX_SAND_IN  SOC_DNX_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "link_bitmap:")));
  dnx_sand_u64_print(&(info->link_bitmap), 1, 0);

exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


