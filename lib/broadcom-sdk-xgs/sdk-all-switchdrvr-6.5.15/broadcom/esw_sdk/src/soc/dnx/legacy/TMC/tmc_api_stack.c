/* $Id: jer2_tmc_api_stack.c,v 1.5 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
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

#include <soc/dnx/legacy/TMC/tmc_api_stack.h>

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
  SOC_DNX_STACK_GLBL_INFO_clear(
    DNX_SAND_OUT SOC_DNX_STACK_GLBL_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_STACK_GLBL_INFO));
  info->max_nof_tm_domains = SOC_DNX_STACK_MAX_NOF_TM_DOMAINS_1;
  info->my_tm_domain = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_STACK_PORT_DISTR_INFO_clear(
    DNX_SAND_OUT SOC_DNX_STACK_PORT_DISTR_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_STACK_PORT_DISTR_INFO));
  info->peer_tm_domain = 0;
  for (ind = 0; ind < SOC_DNX_STACK_PRUN_BMP_LEN; ++ind)
  {
    info->prun_bmp[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_STACK_MAX_NOF_TM_DOMAINS_to_string(
    DNX_SAND_IN  SOC_DNX_STACK_MAX_NOF_TM_DOMAINS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_STACK_MAX_NOF_TM_DOMAINS_1:
    str = "1";
  break;
  case SOC_DNX_STACK_MAX_NOF_TM_DOMAINS_8:
    str = "8";
  break;
  case SOC_DNX_STACK_MAX_NOF_TM_DOMAINS_16:
    str = "16";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_STACK_GLBL_INFO_print(
    DNX_SAND_IN  SOC_DNX_STACK_GLBL_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "max_nof_tm_domains %s "), SOC_DNX_STACK_MAX_NOF_TM_DOMAINS_to_string(info->max_nof_tm_domains)));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "my_tm_domain: %u\n\r"),info->my_tm_domain));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_STACK_PORT_DISTR_INFO_print(
    DNX_SAND_IN  SOC_DNX_STACK_PORT_DISTR_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "peer_tm_domain: %u\n\r"),info->peer_tm_domain));
  for (ind = 0; ind < SOC_DNX_STACK_PRUN_BMP_LEN; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "prun_bmp[%u]: %u\n\r"),ind,info->prun_bmp[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


