/* $Id: jer2_tmc_api_multicast_ingress.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_multicast_ingress.c
*
* MODULE PREFIX:  soc_jer2_tmcmult_ing
*
* FILE DESCRIPTION: refer to H file.
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

#include <soc/dnx/legacy/TMC/tmc_api_multicast_ingress.h>
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
  SOC_DNX_MULT_ING_TR_CLS_MAP_clear(
    DNX_SAND_OUT SOC_DNX_MULT_ING_TR_CLS_MAP *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_ING_TR_CLS_MAP));
  for (ind=0; ind<SOC_DNX_NOF_TRAFFIC_CLASSES; ++ind)
  {
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_ING_ENTRY_clear(
    DNX_SAND_OUT SOC_DNX_MULT_ING_ENTRY *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_ING_ENTRY));
  SOC_DNX_DEST_INFO_clear(&(info->destination));
  info->cud = 1;
exit:
  SHR_VOID_FUNC_EXIT;
}


void
  SOC_DNX_MULT_ING_TR_CLS_MAP_print(
    DNX_SAND_IN SOC_DNX_MULT_ING_TR_CLS_MAP *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind=0; ind<SOC_DNX_NOF_TRAFFIC_CLASSES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Map[%u]: %d\n\r"),ind,info->map[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_ING_ENTRY_print(
    DNX_SAND_IN SOC_DNX_MULT_ING_ENTRY *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Destination: ")));
  SOC_DNX_DEST_INFO_print(&(info->destination));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Copy-unique-data: %u\n\r"),info->cud));
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


