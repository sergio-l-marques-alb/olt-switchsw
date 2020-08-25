/* $Id: jer2_tmc_api_ingress_packet_queuing.c,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_ingress_packet_queuing.c
*
* MODULE PREFIX:  soc_jer2_tmcipq
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

#include <soc/dnx/legacy/TMC/tmc_api_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_packet_queuing.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>

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
  SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO));
  info->base_queue_id = 0;
  /*
   *  Invalid configuration - the user must change it before setting
   */
  info->queue_id_add_not_decrement = FALSE;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_IPQ_BASEQ_MAP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_IPQ_BASEQ_MAP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_IPQ_BASEQ_MAP_INFO));
  info->valid = 0;
  info->base_queue = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_IPQ_QUARTET_MAP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_IPQ_QUARTET_MAP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_IPQ_QUARTET_MAP_INFO));
  info->flow_quartet_index = SOC_DNX_IPQ_INVALID_FLOW_QUARTET;
  info->is_composite = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_IPQ_TR_CLS_RNG_to_string(
    DNX_SAND_IN SOC_DNX_IPQ_TR_CLS_RNG enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_IPQ_TR_CLS_MIN:
    str = "TR_CLS_MIN";
  break;

  case SOC_DNX_IPQ_TR_CLS_MAX:
    str = "TR_CLS_MAX";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    DNX_SAND_IN SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Base_queue_id: %u\n\r"),info->base_queue_id));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Queue_id_add_not_decrement: %d\n\r"),info->queue_id_add_not_decrement));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_IPQ_BASEQ_MAP_INFO_print(
    DNX_SAND_IN  SOC_DNX_IPQ_BASEQ_MAP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "base_queue: %u\n\r"),info->base_queue));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_IPQ_QUARTET_MAP_INFO_print(
    DNX_SAND_IN SOC_DNX_IPQ_QUARTET_MAP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  if (
      (info->flow_quartet_index == SOC_DNX_IPQ_INVALID_FLOW_QUARTET) &&
      (info->is_composite == 0)
     )
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "The queue quartet is unmapped.\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Flow_quartet_index: %u\n\r"),info->flow_quartet_index));
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Is_composite: %d\n\r"),info->is_composite));
  }

exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


