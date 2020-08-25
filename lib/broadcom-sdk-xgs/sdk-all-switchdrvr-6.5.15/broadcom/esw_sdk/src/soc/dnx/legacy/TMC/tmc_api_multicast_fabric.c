/* $Id: jer2_tmc_api_multicast_fabric.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_multicast_fabric.c
*
* MODULE PREFIX:  soc_jer2_tmcmult_fabric
*
* FILE DESCRIPTION: refer to H file
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

#include <soc/dnx/legacy/TMC/tmc_api_multicast_fabric.h>
#include <soc/dnx/legacy/TMC/tmc_api_general.h>

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
  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO));
  info->multicast_class_valid = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_SHAPER_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_SHAPER_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_SHAPER_INFO));
  info->rate = 0;
  info->max_burst = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_BE_CLASS_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_BE_CLASS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_BE_CLASS_INFO));
  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO_clear(&(info->be_sch));
  info->weight = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_BE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_BE_INFO *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_BE_INFO));
  SOC_DNX_MULT_FABRIC_SHAPER_INFO_clear(&(info->be_shaper));
  info->wfq_enable = 0;
  for (ind=0; ind<SOC_DNX_MULT_FABRIC_NOF_BE_CLASSES; ++ind)
  {
    SOC_DNX_MULT_FABRIC_BE_CLASS_INFO_clear(&(info->be_sch[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_GR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_GR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_GR_INFO));
  SOC_DNX_MULT_FABRIC_SHAPER_INFO_clear(&(info->gr_shaper));
  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO_clear(&(info->gr_sch));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_INFO));
  SOC_DNX_MULT_FABRIC_GR_INFO_clear(&(info->guaranteed));
  SOC_DNX_MULT_FABRIC_BE_INFO_clear(&(info->best_effort));
  info->max_rate = 0;
  info->max_burst = 1;
  info->credits_via_sch = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_ACTIVE_LINKS_clear(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_ACTIVE_LINKS *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_ACTIVE_LINKS));
  for (ind=0; ind<SOC_DNX_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS(unit); ++ind)
  {
    info->bitmap[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_FLOW_CONTROL_MAP_clear(
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_FLOW_CONTROL_MAP *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_FABRIC_FLOW_CONTROL_MAP));
  
  info->bfmc0_lb_fc_map = SOC_DNX_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;
  info->bfmc1_lb_fc_map = SOC_DNX_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;
  info->bfmc2_lb_fc_map = SOC_DNX_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;
  info->gfmc_lb_fc_map = SOC_DNX_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;


exit:
  SHR_VOID_FUNC_EXIT;
}



const char*
  SOC_DNX_MULT_FABRIC_CLS_RNG_to_string(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_CLS_RNG enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_MULT_FABRIC_CLS_MIN:
    str = "MIN";
  break;

  case SOC_DNX_MULT_FABRIC_CLS_MAX:
    str = "MAX";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO_print(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "   Class_valid: %d, "),info->multicast_class_valid));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Port_id: %u\n\r"),info->mcast_class_hr_id));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_SHAPER_INFO_print(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_SHAPER_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Rate: %u, "),info->rate));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Max_burst: %u.\n\r"),info->max_burst));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_BE_CLASS_INFO_print(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_BE_CLASS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Be_sch_port:\n\r")));
  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO_print(&(info->be_sch));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Weight: %u\n\r"),info->weight));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_BE_INFO_print(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_BE_INFO *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Be_shaper: ")));
  SOC_DNX_MULT_FABRIC_SHAPER_INFO_print(&(info->be_shaper));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Wfq_enable: %d\n\r"),info->wfq_enable));
  for (ind=0; ind<SOC_DNX_MULT_FABRIC_NOF_BE_CLASSES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        " Be_sch_port[%u]:\n\r"),ind));
    SOC_DNX_MULT_FABRIC_BE_CLASS_INFO_print(&(info->be_sch[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_GR_INFO_print(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_GR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Gr_shaper: ")));
  SOC_DNX_MULT_FABRIC_SHAPER_INFO_print(&(info->gr_shaper));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      " Gr_sch_port:\n\r")));
  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO_print(&(info->gr_sch));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_INFO_print(
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Guaranteed:\n\r")));
  SOC_DNX_MULT_FABRIC_GR_INFO_print(&(info->guaranteed));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Best_effort:\n\r")));
  SOC_DNX_MULT_FABRIC_BE_INFO_print(&(info->best_effort));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Max_rate:        %u[Kbps]\n\r"),info->max_rate));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Credits_via_sch: %d\n\r"),info->credits_via_sch));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_FABRIC_ACTIVE_LINKS_print(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_IN SOC_DNX_MULT_FABRIC_ACTIVE_LINKS *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind=0; ind<SOC_DNX_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS(unit); ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Bitmap[%u]: %u\n\r"),ind,info->bitmap[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


