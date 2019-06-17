/* $Id: jer2_tmc_api_ofp_rates.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_ofp_rates.c
*
* MODULE PREFIX:  soc_jer2_tmcofp_rates
*
* FILE DESCRIPTION:
*   Rates and burst configuration of the Outgoing FAP Ports.
*   The configuration envolves End-to-end scheduler and Egress processor.
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

#include <soc/dnx/legacy/TMC/tmc_api_ofp_rates.h>
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
  SOC_DNX_OFP_RATES_MAL_SHPR_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_MAL_SHPR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_MAL_SHPR));
  info->rate_update_mode = SOC_DNX_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
  info->rate = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_MAL_SHPR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_MAL_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_MAL_SHPR_INFO));
  SOC_DNX_OFP_RATES_MAL_SHPR_clear(&(info->sch_shaper));
  SOC_DNX_OFP_RATES_MAL_SHPR_clear(&(info->egq_shaper));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_INTERFACE_SHPR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_INTERFACE_SHPR));
  info->rate_update_mode = SOC_DNX_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
  info->rate = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_INTERFACE_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_INTERFACE_SHPR_INFO));
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_clear(&(info->sch_shaper));
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_clear(&(info->egq_shaper));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATE_INFO));
  info->port_id = SOC_DNX_OFP_RATES_ILLEGAL_PORT_ID;
  info->sch_rate = 0;
  info->egq_rate = 0;
  info->max_burst = SOC_DNX_OFP_RATES_BURST_LIMIT_MAX;
  info->port_priority = 0;
  info->tcg_ndx = 0;
  info->sch_max_burst = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_TBL_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_TBL_INFO *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_TBL_INFO));
  info->nof_valid_entries = 0;
  for (ind=0; ind<SOC_DNX_NOF_FAP_PORTS; ++ind)
  {
    SOC_DNX_OFP_RATE_INFO_clear(&(info->rates[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_FAT_PIPE_RATE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_FAT_PIPE_RATE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_FAT_PIPE_RATE_INFO));
  info->sch_rate = 0;
  info->egq_rate = 0;
  info->max_burst = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_PORT_PRIORITY_SHPR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_PORT_PRIORITY_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_PORT_PRIORITY_SHPR_INFO));
  SOC_DNX_OFP_RATE_SHPR_INFO_clear(&info->sch_shaper);
  info->sch_shaper.max_burst = SOC_DNX_OFP_RATES_SCH_BURST_LIMIT_MAX;
  SOC_DNX_OFP_RATE_SHPR_INFO_clear(&info->egq_shaper);
  info->egq_shaper.max_burst = SOC_DNX_OFP_RATES_BURST_LIMIT_MAX;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATE_SHPR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATE_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATE_SHPR_INFO));
  info->rate = 0;  
  info->max_burst = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_TCG_SHPR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_TCG_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_OFP_RATES_TCG_SHPR_INFO));
  SOC_DNX_OFP_RATE_SHPR_INFO_clear(&info->sch_shaper);
  info->sch_shaper.max_burst = SOC_DNX_OFP_RATES_SCH_BURST_LIMIT_MAX;
  SOC_DNX_OFP_RATE_SHPR_INFO_clear(&info->egq_shaper); 
  info->egq_shaper.max_burst = SOC_DNX_OFP_RATES_BURST_LIMIT_MAX;
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_OFP_RATES_CAL_SET_to_string(
    DNX_SAND_IN SOC_DNX_OFP_RATES_CAL_SET enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_OFP_RATES_CAL_SET_A:
    str = "a";
  break;
  case SOC_DNX_OFP_RATES_CAL_SET_B:
    str = "b";
  break;
  case SOC_DNX_OFP_NOF_RATES_CAL_SETS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_OFP_SHPR_UPDATE_MODE_to_string(
    DNX_SAND_IN SOC_DNX_OFP_SHPR_UPDATE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS:
    str = "sum_of_ports";
  break;
  case SOC_DNX_OFP_SHPR_UPDATE_MODE_OVERRIDE:
    str = "override";
  break;
  case SOC_DNX_OFP_SHPR_UPDATE_MODE_DONT_TUCH:
    str = "dont_tuch";
  break;
  case SOC_DNX_OFP_NOF_SHPR_UPDATE_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_OFP_RATES_MAL_SHPR_print(
    DNX_SAND_IN SOC_DNX_OFP_RATES_MAL_SHPR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Rate_update_mode: %s \n\r"),
           SOC_DNX_OFP_SHPR_UPDATE_MODE_to_string(info->rate_update_mode)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Rate: %u[Kbps]\n\r"),info->rate));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_MAL_SHPR_INFO_print(
    DNX_SAND_IN SOC_DNX_OFP_RATES_MAL_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Sch_shaper:\n\r")));
  SOC_DNX_OFP_RATES_MAL_SHPR_print(&(info->sch_shaper));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Egq_shaper:\n\r")));
  SOC_DNX_OFP_RATES_MAL_SHPR_print(&(info->egq_shaper));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_print(
    DNX_SAND_IN SOC_DNX_OFP_RATES_INTERFACE_SHPR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Rate_update_mode: %s \n\r"),
           SOC_DNX_OFP_SHPR_UPDATE_MODE_to_string(info->rate_update_mode)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Rate: %u[Kbps]\n\r"),info->rate));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_INFO_print(
    DNX_SAND_IN SOC_DNX_OFP_RATES_INTERFACE_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Sch_shaper:\n\r")));
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_print(&(info->sch_shaper));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Egq_shaper:\n\r")));
  SOC_DNX_OFP_RATES_INTERFACE_SHPR_print(&(info->egq_shaper));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATE_INFO_print(
    DNX_SAND_IN SOC_DNX_OFP_RATE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Port[%-2u] rate: "),info->port_id));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "SCH: %-8u[Kbps], "),info->sch_rate));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "EGQ: %-8u[Kbps], "),info->egq_rate));

  if (info->max_burst == SOC_DNX_OFP_RATES_BURST_LIMIT_MAX)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Max Burst: No Limit.\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Max Burst: %-6u[Byte].\n\r"),info->max_burst));
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_TBL_INFO_print(
    DNX_SAND_IN SOC_DNX_OFP_RATES_TBL_INFO *info
  )
{
  uint32
    ind=0,
    nof_zero_rate=0;
  uint8
    zero_rate;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Nof_valid_entries: %u[Entries]\n\r"),info->nof_valid_entries));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Rates:\n\r")));
  for (ind=0; ind<info->nof_valid_entries; ++ind)
  {
    zero_rate = DNX_SAND_NUM2BOOL((info->rates[ind].egq_rate == 0) && (info->rates[ind].sch_rate == 0));
    if(!zero_rate)
    {
      SOC_DNX_OFP_RATE_INFO_print(&(info->rates[ind]));
    }
    else
    {
      nof_zero_rate++;
    }
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_FAT_PIPE_RATE_INFO_print(
    DNX_SAND_IN SOC_DNX_OFP_FAT_PIPE_RATE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Sch_rate:  %u[Kbps]\n\r"),info->sch_rate));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Egq_rate:  %u[Kbps]\n\r"),info->egq_rate));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Max_burst: %u[Bytes]\n\r"),info->max_burst));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_PORT_PRIORITY_SHPR_INFO_print(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_PORT_PRIORITY_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "SCH: ")));
  SOC_DNX_OFP_RATE_SHPR_INFO_print(&info->sch_shaper);
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "EGQ: ")));
  SOC_DNX_OFP_RATE_SHPR_INFO_print(&info->egq_shaper);  
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATE_SHPR_INFO_print(
    DNX_SAND_OUT SOC_DNX_OFP_RATE_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "rate:  %u[Kbps]\n\r"),info->rate));  
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Max_burst: %u[Bytes]\n\r"),info->max_burst));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_OFP_RATES_TCG_SHPR_INFO_print(
    DNX_SAND_OUT SOC_DNX_OFP_RATES_TCG_SHPR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "SCH: ")));
  SOC_DNX_OFP_RATE_SHPR_INFO_print(&info->sch_shaper);
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "EGQ: ")));
  SOC_DNX_OFP_RATE_SHPR_INFO_print(&info->egq_shaper);  
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


