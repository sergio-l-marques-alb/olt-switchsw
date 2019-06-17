/* $Id: jer2_tmc_api_multicast_egress.c,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_multicast_egress.c
*
* MODULE PREFIX:  soc_jer2_tmcmult_eg
*
* FILE DESCRIPTION:  refer to H file.
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

#include <soc/dnx/legacy/TMC/tmc_api_multicast_egress.h>
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
  SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(
    DNX_SAND_OUT SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE));
  info->mc_id_low = 0;
  info->mc_id_high = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_EG_ENTRY_clear(
    DNX_SAND_OUT SOC_DNX_MULT_EG_ENTRY *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_EG_ENTRY));
  info->type = SOC_DNX_MULT_EG_ENTRY_TYPE_OFP;
  info->vlan_mc_id = 0;
  info->cud = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(
    DNX_SAND_OUT SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP));
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_MULT_EG_ENTRY_TYPE_to_string(
    DNX_SAND_IN  SOC_DNX_MULT_EG_ENTRY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_MULT_EG_ENTRY_TYPE_OFP:
    str = "ofp";
  break;
  case SOC_DNX_MULT_EG_ENTRY_TYPE_VLAN_PTR:
    str = "vlan_ptr";
  break;
  case SOC_DNX_MULT_EG_ENTRY_NOF_TYPES:
    str = "nof_types";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(
    DNX_SAND_IN SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Mc_id_low:  %u, Mc_id_high: %u\n\r"),info->mc_id_low, info->mc_id_high));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Mc_id_high: %u\n\r"),info->mc_id_high));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_EG_ENTRY_print(
    DNX_SAND_IN SOC_DNX_MULT_EG_ENTRY *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Type %s "), SOC_DNX_MULT_EG_ENTRY_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Port-id: %u, Copy-unique-data: %u\n\r"),info->port, info->cud));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Vlan_mc_id: %u\n\r"),info->vlan_mc_id));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(
    DNX_SAND_IN SOC_DNX_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  )
{
  uint32
    ind=0,
    bit_idx,
    reg_idx,
    bit_val,
    cnt = 0;
  uint8
    found = FALSE;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Member FAP Ports:\n\r")));
  for (ind=0; ind<SOC_DNX_NOF_FAP_PORTS_MAX; ++ind) 
  {
    bit_idx = ind % DNX_SAND_REG_SIZE_BITS;
    reg_idx = ind / DNX_SAND_REG_SIZE_BITS;
    bit_val = DNX_SAND_GET_BIT(info->bitmap[reg_idx], bit_idx);
    if (bit_val)
    {
      found = TRUE;
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "%02u "), ind));

      if ((++cnt % 10) == 0)
      {
        LOG_CLI((BSL_META_U(NO_UNIT,
                            "\n\r")));
      }
    }
  }
  if (!found)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "None")));
  }
  else
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "\n\rTotal: %u"), cnt));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


