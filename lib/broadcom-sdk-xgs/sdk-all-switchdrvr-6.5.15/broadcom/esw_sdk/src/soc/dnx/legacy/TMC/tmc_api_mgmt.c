/* $Id: jer2_tmc_api_mgmt.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_mgmt.c
*
* MODULE PREFIX:  soc_jer2_tmcmgmt
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


#include <soc/dnx/legacy/TMC/tmc_api_mgmt.h>

#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>

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
  SOC_DNX_MGMT_PCKT_SIZE_clear(
    DNX_SAND_OUT SOC_DNX_MGMT_PCKT_SIZE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MGMT_PCKT_SIZE));
  info->min = 0;
  info->max = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MGMT_OCB_VOQ_INFO_clear(
    DNX_SAND_OUT SOC_DNX_MGMT_OCB_VOQ_INFO *info
  )
{
  int32
    index;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MGMT_OCB_VOQ_INFO));
  info->voq_eligible = TRUE;
  for(index = 0; index < SOC_DNX_MGMT_OCB_VOQ_NOF_THRESHOLDS; ++index)
  {
      info->th_words[index] = SOC_DNX_MGMT_OCB_PRM_EN_TH_DEFAULT;
      info->th_buffers[index] = SOC_DNX_MGMT_OCB_PRM_EN_TH_DEFAULT;
  }


exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_MGMT_FABRIC_HDR_TYPE_to_string(
    DNX_SAND_IN  SOC_DNX_MGMT_FABRIC_HDR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_MGMT_FABRIC_HDR_TYPE_PETRA:
    str = "petra";
  break;
  case SOC_DNX_MGMT_FABRIC_HDR_TYPE_FAP20:
    str = "fap20";
  break;
  case SOC_DNX_MGMT_FABRIC_HDR_TYPE_FAP10M:
    str = "fap10m";
  break;
  case SOC_DNX_MGMT_NOF_FABRIC_HDR_TYPES:
    str = " Not initialized";
  }
  return str;
}


const char*
  SOC_DNX_MGMT_TDM_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_MGMT_TDM_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_MGMT_TDM_MODE_PACKET:
    str = "packet";
  break;
  case SOC_DNX_MGMT_TDM_MODE_TDM_OPT:
    str = "tdm_opt";
  break;
  case SOC_DNX_MGMT_TDM_MODE_TDM_STA:
    str = "tdm_sta";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    str = "extern";
  break;
  case SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    str = "intern";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_MGMT_PCKT_SIZE_print(
    DNX_SAND_IN SOC_DNX_MGMT_PCKT_SIZE *info
  )
{
  char
    min_str[30],
    max_str[30];

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  if (info->min == SOC_DNX_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
  {
    sal_sprintf(min_str, "Not Limited by Original Size");
  }
  else
  {
    sal_sprintf(min_str, "%u[Bytes]", info->min);
  }

  if (info->max == SOC_DNX_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
  {
    sal_sprintf(max_str, "Not Limited by Original Size");
  }
  else
  {
    sal_sprintf(max_str, "%u[Bytes]", info->max);
  }

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Min: %s, Max: %s\n\r"),min_str, max_str));
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


