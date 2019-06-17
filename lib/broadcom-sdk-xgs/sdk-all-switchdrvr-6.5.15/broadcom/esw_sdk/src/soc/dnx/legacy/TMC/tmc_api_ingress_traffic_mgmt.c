/* $Id: jer2_tmc_api_ingress_traffic_mgmt.c,v 1.12 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_ingress_traffic_mgmt.c
*
* MODULE PREFIX:  soc_jer2_tmcitm
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

#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dnx/legacy/TMC/tmc_api_general.h>

#include <soc/dnxc/legacy/error.h>

/* } */

void
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(
    DNX_SAND_OUT SOC_DNX_ITM_GLOB_RCS_FC_TYPE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ITM_GLOB_RCS_FC_TYPE));
  SOC_DNX_THRESH_WITH_HYST_INFO_clear(&(info->hp));
  SOC_DNX_THRESH_WITH_HYST_INFO_clear(&(info->lp));
exit:
  SHR_VOID_FUNC_EXIT;
}


void
  SOC_DNX_ITM_GLOB_RCS_FC_TH_clear(
    DNX_SAND_OUT SOC_DNX_ITM_GLOB_RCS_FC_TH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_ITM_GLOB_RCS_FC_TH));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->bdbs));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->unicast));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->full_mc));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->mini_mc));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->ocb));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->ocb_p0));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->ocb_p1));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->mix_p0));
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(&(info->mix_p1));
  
exit:
  SHR_VOID_FUNC_EXIT;
}

