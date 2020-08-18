/* $Id: jer2_tmc_api_ingress_scheduler.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_ingress_scheduler.c
*
* MODULE PREFIX:  soc_jer2_tmcingress_scheduler
*
* FILE DESCRIPTION:  in the H file.
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

#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/TMC/tmc_api_ingress_scheduler.h>

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
  SOC_DNX_ING_SCH_SHAPER_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_SHAPER *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_SHAPER));
  info->max_rate = 0;
  info->max_burst = 0;

  info->slow_start_enable = 0;
  info->slow_start_rate_phase_0 = 0;
  info->slow_start_rate_phase_1 = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_MESH_CONTEXT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_MESH_CONTEXT_INFO));
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->shaper));
  info->weight = 0;
  info->id = SOC_DNX_ING_NOF_SCH_MESH_CONTEXTSS;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_MESH_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_MESH_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_MESH_INFO));
  for (ind = 0; ind < SOC_DNX_ING_SCH_MESH_LAST; ++ind)
  {
    SOC_DNX_ING_SCH_MESH_CONTEXT_INFO_clear(&(info->contexts[ind]));
  }
  info->nof_entries = 0;
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->total_rate_shaper));
exit:
  SHR_VOID_FUNC_EXIT;
}
  
void
  SOC_DNX_ING_SCH_MESH_INFO_SHAPERS_dont_touch(
    DNX_SAND_OUT SOC_DNX_ING_SCH_MESH_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
  info->contexts[SOC_DNX_ING_SCH_MESH_CON1].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_DNX_ING_SCH_MESH_CON2].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_DNX_ING_SCH_MESH_CON3].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_DNX_ING_SCH_MESH_CON4].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_DNX_ING_SCH_MESH_CON5].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_DNX_ING_SCH_MESH_CON6].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_DNX_ING_SCH_MESH_CON7].shaper.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->total_rate_shaper.max_rate=SOC_DNX_ING_SCH_DONT_TOUCH;

exit:
  SHR_VOID_FUNC_EXIT;
}  
  

void
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT));
  info->weight1 = 0;
  info->weight2 = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_WFQS_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_CLOS_WFQS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_CLOS_WFQS));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->fabric_hp));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->fabric_lp));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->global_hp));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->global_lp));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_HP_SHAPERS_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_CLOS_HP_SHAPERS));
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->local));
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->fabric_unicast));
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->fabric_multicast));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_SHAPERS_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_CLOS_SHAPERS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_CLOS_SHAPERS));
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->local));
  SOC_DNX_ING_SCH_SHAPER_clear(&(info->fabric));
  SOC_DNX_ING_SCH_CLOS_HP_SHAPERS_clear(&(info->hp));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ING_SCH_CLOS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ING_SCH_CLOS_INFO));
  SOC_DNX_ING_SCH_CLOS_SHAPERS_clear(&(info->shapers));
  SOC_DNX_ING_SCH_CLOS_WFQS_clear(&(info->weights));
exit:
  SHR_VOID_FUNC_EXIT;
}
void
  SOC_DNX_ING_SCH_CLOS_INFO_SHAPERS_dont_touch(
    DNX_SAND_OUT SOC_DNX_ING_SCH_CLOS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  info->shapers.local.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.hp.local.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.fabric.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_unicast.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.slow_start_enable = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_unicast.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.max_rate = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.slow_start_enable = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 = SOC_DNX_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = SOC_DNX_ING_SCH_DONT_TOUCH;

exit:
  SHR_VOID_FUNC_EXIT;
}
	


const char*
  SOC_DNX_ING_SCH_MESH_CONTEXTS_to_string(
    DNX_SAND_IN  SOC_DNX_ING_SCH_MESH_CONTEXTS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_ING_SCH_MESH_LOCAL:
    str = "LOCAL";
  break;

  case SOC_DNX_ING_SCH_MESH_CON1:
    str = "CONTEXT1";
  break;

  case SOC_DNX_ING_SCH_MESH_CON2:
    str = "CONTEXT2";
  break;

  case SOC_DNX_ING_SCH_MESH_CON3:
    str = "CONTEXT3";
  break;

  case SOC_DNX_ING_SCH_MESH_CON4:
    str = "CONTEXT4";
  break;

  case SOC_DNX_ING_SCH_MESH_CON5:
    str = "CONTEXT5";
  break;

  case SOC_DNX_ING_SCH_MESH_CON6:
    str = "CONTEXT6";
  break;

  case SOC_DNX_ING_SCH_MESH_CON7:
    str = "CONTEXT7";
  break;

  case SOC_DNX_ING_SCH_MESH_LAST:
    str = " Not initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_ING_SCH_SHAPER_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_SHAPER *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Max_rate: %u[Kbps], "),info->max_rate));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Max_burst: %u[Bytes].\n\r"),info->max_burst));

  if (info->slow_start_enable)
  {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Slow rate first phase precent: %u%% "),info->slow_start_rate_phase_0));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Slow rate second phase precent: %u%% "),info->slow_start_rate_phase_1));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_MESH_CONTEXT_INFO_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Shaper:")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->shaper));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Weight: %u\n\r"),info->weight));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Id %s \n\r"),
           SOC_DNX_ING_SCH_MESH_CONTEXTS_to_string(info->id)
           ));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_MESH_INFO_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_MESH_INFO *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind=0; ind<SOC_DNX_ING_SCH_MESH_LAST; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Contexts[%u]:\n\r"),ind));
    SOC_DNX_ING_SCH_MESH_CONTEXT_INFO_print(&(info->contexts[ind]));
  }
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Nof_entries: %u[Entries]\n\r"),info->nof_entries));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Total_rate_shaper: ")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->total_rate_shaper));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "    Weight1: %u, "),info->weight1));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Weight2: %u\n\r"),info->weight2));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_WFQS_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_CLOS_WFQS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Fabric_hp:\n\r")));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->fabric_hp));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Fabric_lp:\n\r")));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->fabric_lp));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Global_hp:\n\r")));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->global_hp));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Global_lp:\n\r")));
  SOC_DNX_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->global_lp));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_HP_SHAPERS_print(
    DNX_SAND_IN  SOC_DNX_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Local:")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->local));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Fabric_unicast:")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->fabric_unicast));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Fabric_multicast:")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->fabric_multicast));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_SHAPERS_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_CLOS_SHAPERS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Local: ")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->local));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Fabric: ")));
  SOC_DNX_ING_SCH_SHAPER_print(&(info->fabric));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "hp:")));
  SOC_DNX_ING_SCH_CLOS_HP_SHAPERS_print(&(info->hp));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_ING_SCH_CLOS_INFO_print(
    DNX_SAND_IN SOC_DNX_ING_SCH_CLOS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Shapers:\n\r")));
  SOC_DNX_ING_SCH_CLOS_SHAPERS_print(&(info->shapers));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Weights:\n\r")));
  SOC_DNX_ING_SCH_CLOS_WFQS_print(&(info->weights));
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


