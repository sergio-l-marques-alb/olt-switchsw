/* $Id: jer2_tmc_api_cell.c,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_cell.c
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
#include <soc/dnx/legacy/SAND/SAND_FM/sand_cell.h>
#include <soc/dnx/legacy/SAND/Management/sand_low_level.h>

#include <soc/dnx/legacy/TMC/tmc_api_cell.h>

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
  SOC_DNX_SR_CELL_LINK_LIST_clear(
    DNX_SAND_OUT SOC_DNX_SR_CELL_LINK_LIST *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_SR_CELL_LINK_LIST));
  for (ind = 0; ind < SOC_DNX_CELL_NOF_LINKS_IN_PATH_LINKS; ++ind)
  {
    info->path_links[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CELL_MC_TBL_DATA_clear(
    DNX_SAND_OUT SOC_DNX_CELL_MC_TBL_DATA *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CELL_MC_TBL_DATA));
  for (ind = 0; ind < SOC_DNX_CELL_MC_DATA_IN_UINT32S; ++ind)
  {
    info->data[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CELL_MC_TBL_INFO_clear(
    DNX_SAND_OUT SOC_DNX_CELL_MC_TBL_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CELL_MC_TBL_INFO));
  info->fe_location = SOC_DNX_CELL_NOF_FE_LOCATIONS;
  for (ind = 0; ind < SOC_DNX_CELL_MC_NOF_LINKS; ++ind)
  {
    info->path_links[ind] = 0;
  }
  for (ind = 0; ind < SOC_DNX_CELL_MC_NOF_CHANGES; ++ind)
  {
    info->filter[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_CELL_FE_LOCATION_to_string(
    DNX_SAND_IN  SOC_DNX_CELL_FE_LOCATION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_CELL_FE_LOCATION_FE1:
    str = "fe1";
  break;
  case SOC_DNX_CELL_FE_LOCATION_FE2:
    str = "fe2";
  break;
  case SOC_DNX_CELL_NOF_FE_LOCATIONS:
    str = "nof_fe_locations";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_SR_CELL_LINK_LIST_print(
    DNX_SAND_IN  SOC_DNX_SR_CELL_LINK_LIST *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind = 0; ind < SOC_DNX_CELL_NOF_LINKS_IN_PATH_LINKS; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "path_links[%u]: %u\n\r"), ind,info->path_links[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CELL_MC_TBL_DATA_print(
    DNX_SAND_IN  SOC_DNX_CELL_MC_TBL_DATA *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind = 0; ind < SOC_DNX_CELL_MC_DATA_IN_UINT32S; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "data[%u]: %u\n\r"),ind,info->data[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CELL_MC_TBL_INFO_print(
    DNX_SAND_IN  SOC_DNX_CELL_MC_TBL_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "fe_location %s "), SOC_DNX_CELL_FE_LOCATION_to_string(info->fe_location)));
  for (ind = 0; ind < SOC_DNX_CELL_MC_NOF_LINKS; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "path_links[%u]: %u\n\r"),ind,info->path_links[ind]));
  }
  for (ind = 0; ind < SOC_DNX_CELL_MC_NOF_CHANGES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "filter[%u]: %u\n\r"),ind,info->filter[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */

