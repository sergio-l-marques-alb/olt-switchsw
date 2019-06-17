/* $Id: jer2_tmc_api_fabric.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_fabric.c
*
* MODULE PREFIX:  soc_jer2_tmcfabric
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

#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>

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
  SOC_DNX_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_LINKS_CONNECT_MAP_STAT_INFO));
  info->far_unit = SOC_DNX_DEVICE_ID_IRRELEVANT;
  info->far_dev_type = SOC_DNX_FAR_NOF_DEVICE_TYPES;
  info->far_link_id = 0;
  info->is_connected_expected = 0;
  info->is_phy_connect = 0;
  info->is_logically_connected = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    DNX_SAND_OUT SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR));
  for (ind=0; ind<DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS; ++ind)
  {
    SOC_DNX_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(&(info->link_info[ind]));
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_CELL_FORMAT_clear(
    DNX_SAND_OUT SOC_DNX_FABRIC_CELL_FORMAT *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_CELL_FORMAT));
  info->variable_cell_size_enable = TRUE;
  info->segmentation_enable = TRUE;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_FC_clear(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_OUT SOC_DNX_FABRIC_FC *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_FC));
  for (ind=0; ind<dnx_data_fabric.links.nof_links_get(unit); ++ind)
  {
    info->enable[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_COEXIST_INFO_clear(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_OUT SOC_DNX_FABRIC_COEXIST_INFO *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_COEXIST_INFO));
  info->enable = 0;
  for (ind=0; ind<SOC_DNX_FABRIC_NOF_COEXIST_DEV_ENTRIES; ++ind)
  {
    info->coexist_dest_map[ind] = 0;
  }
  for (ind=0; ind<dnx_data_fabric.links.nof_links_get(unit); ++ind)
  {
    info->fap_id_odd_link_map[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_LINKS_STATUS_SINGLE_clear(
    DNX_SAND_OUT SOC_DNX_FABRIC_LINKS_STATUS_SINGLE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_LINKS_STATUS_SINGLE));
  info->valid = 0;
  info->up_tx = 0;
  info->up_rx = 0;
  info->errors_bm = 0;
  info->indications_bm = 0;
  info->crc_err_counter = 0;
  info->leaky_bucket_counter = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_LINKS_STATUS_ALL_clear(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_OUT SOC_DNX_FABRIC_LINKS_STATUS_ALL *info
  )
{
  uint32 ind;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_LINKS_STATUS_ALL));
  info->error_in_some = 0;
  for (ind=0; ind<dnx_data_fabric.links.nof_links_get(unit); ++ind)
  {
    SOC_DNX_FABRIC_LINKS_STATUS_SINGLE_clear(&(info->single_link_status[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_LINK_STATE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_LINK_STATE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_LINK_STATE_INFO));
  info->on_off = SOC_DNX_LINK_NOF_STATES;
  info->serdes_also = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void 
  SOC_DNX_FABRIC_FC_SHAPER_clear(
      DNX_SAND_OUT SOC_DNX_FABRIC_FC_SHAPER *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_FC_SHAPER));
  info->data_shaper.bytes = 0;
  info->data_shaper.cells = 0;
  info->fc_shaper.bytes = 0;
  info->fc_shaper.cells = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void 
  SOC_DNX_FABRIC_FC_SHAPER_MODE_INFO_clear(
      DNX_SAND_OUT SOC_DNX_FABRIC_FC_SHAPER_MODE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
  sal_memset(info, 0x0, sizeof(SOC_DNX_FABRIC_FC_SHAPER_MODE_INFO));
  info->shaper_mode = SOC_DNX_FABRIC_SHAPER_NOF_MODES;
exit:
  SHR_VOID_FUNC_EXIT;
}

const char*
  SOC_DNX_LINK_ERR_TYPE_to_string(
    DNX_SAND_IN SOC_DNX_LINK_ERR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_LINK_ERR_TYPE_CRC:
    str = "crc";
  break;
  case SOC_DNX_LINK_ERR_TYPE_SIZE:
    str = "size";
  break;
  case SOC_DNX_LINK_ERR_TYPE_MISALIGN:
    str = "misalign";
  break;
  case SOC_DNX_LINK_ERR_TYPE_CODE_GROUP:
    str = "code_group";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_LINK_INDICATE_TYPE_to_string(
    DNX_SAND_IN SOC_DNX_LINK_INDICATE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_LINK_INDICATE_TYPE_SIG_LOCK:
    str = "sig_lock";
  break;
  case SOC_DNX_LINK_INDICATE_TYPE_ACCEPT_CELL:
    str = "accept_cell";
  break;
  case SOC_DNX_LINK_INDICATE_INTRNL_FIXED:
    str = "intrnl_fixed";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_FABRIC_CONNECT_MODE_to_string(
    DNX_SAND_IN SOC_DNX_FABRIC_CONNECT_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_FABRIC_CONNECT_MODE_FE:
    str = "fe";
  break;
  case SOC_DNX_FABRIC_CONNECT_MODE_BACK2BACK:
    str = "back2back";
  break;
  case SOC_DNX_FABRIC_CONNECT_MODE_MESH:
    str = "mesh";
  break;
  case SOC_DNX_FABRIC_CONNECT_MODE_MULT_STAGE_FE:
    str = "mult_stage_fe";
  break;
  case SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP:
    str = "single_fap";
  break;
  case SOC_DNX_FABRIC_NOF_CONNECT_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_LINK_STATE_to_string(
    DNX_SAND_IN  SOC_DNX_LINK_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_LINK_STATE_ON:
    str = "on";
  break;
  case SOC_DNX_LINK_STATE_OFF:
    str = "off";
  break;
  case SOC_DNX_LINK_NOF_STATES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_FABRIC_LINE_CODING_to_string(
    DNX_SAND_IN  SOC_DNX_FABRIC_LINE_CODING enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_FABRIC_LINE_CODING_8_10:
    str = "coding_8_10";
  break;
  case SOC_DNX_FABRIC_LINE_CODING_8_9_FEC:
    str = "coding_8_9_fec";
  break;
  case SOC_DNX_FABRIC_NOF_LINE_CODINGS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_FABRIC_CELL_FORMAT_print(
    DNX_SAND_IN SOC_DNX_FABRIC_CELL_FORMAT *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Variable_cell_size_enable: %u, "),info->variable_cell_size_enable));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Segmentation_enable: %u\n\r"),info->segmentation_enable));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_FC_print(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_IN SOC_DNX_FABRIC_FC *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind=0; ind<dnx_data_fabric.links.nof_links_get(unit); ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Enable[%2u]: %d\n\r"),ind,info->enable[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FABRIC_COEXIST_INFO_print(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_IN SOC_DNX_FABRIC_COEXIST_INFO *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Enable: %d\n\r"),info->enable));
  for (ind=0; ind<SOC_DNX_FABRIC_NOF_COEXIST_DEV_ENTRIES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Coexist_dest_map[%3u]: %d\n\r"),ind,info->coexist_dest_map[ind]));
  }
  for (ind=0; ind<dnx_data_fabric.links.nof_links_get(unit); ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Fap_id_odd_link_map[%2u]: %d\n\r"),ind,info->fap_id_odd_link_map[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

#define SOC_DNX_SRD_LANE_INVALID 60
uint32
  soc_jer2_tmcfbr2srd_lane_id(
    DNX_SAND_IN  uint32 fabric_link_id
  )
{
  uint32
    serdes_lane_id;

  if (fabric_link_id <= 27)
  {
    /* Fabric 0 - 27, SerDes 32-59 */
    serdes_lane_id = fabric_link_id + 32;
  }
  else if (fabric_link_id <= 31)
  {
    /* Fabric 28 - 31, SerDes 12-15*/
    serdes_lane_id = fabric_link_id - 16;
  }
  else if (fabric_link_id <= 35)
  {
    /* Fabric 32 - 35, SerDes 28-31 */
    serdes_lane_id = fabric_link_id - 4;
  }
  else
  {
    serdes_lane_id = SOC_DNX_SRD_LANE_INVALID;
  }

  return serdes_lane_id;
}

void
  SOC_DNX_LINK_STATE_INFO_print(
    DNX_SAND_IN SOC_DNX_LINK_STATE_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "On_off %s \n\r"),
           SOC_DNX_LINK_STATE_to_string(info->on_off)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Serdes_also: %u\n\r"),info->serdes_also));
exit:
  SHR_VOID_FUNC_EXIT;
}
void
  SOC_DNX_FABRIC_LINKS_STATUS_ALL_print(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_IN SOC_DNX_FABRIC_LINKS_STATUS_ALL *info,
    DNX_SAND_IN DNX_SAND_PRINT_FLAVORS     print_flavour
  )
{
  uint32
    ind=0,
    nof_valid_links = 0;
  char
    crc_err_cnt[11];
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "error_in_some: %u\n\r"),info->error_in_some));

    LOG_CLI((BSL_META_U(NO_UNIT,
                        "    |Srds|       Indications      |             Errors         |Up |Up |Leaky  \n\r"
                        "Link|Lane| Signal |Accept|Internal|          |Cell| mis  |Code |TX |RX |Bucket \n\r"
                        "ID  |ID  |  Lock  |Cells |  Fix   |   CRC    |Size|align |Group|   |   | Th    \n\r"
                        "-------------------------------------------------------------------------------\n\r"
                 )));

  for (ind=0; ind<dnx_data_fabric.links.nof_links_get(unit); ++ind)
  {
    if(info->single_link_status[ind].indications_bm & SOC_DNX_LINK_INDICATE_TYPE_ACCEPT_CELL)
    {
      nof_valid_links++;
    }
    if(info->single_link_status[ind].valid == FALSE)
    {
      continue;
    }
    if(DNX_SAND_PRINT_FLAVORS_ERRS_ONLY == print_flavour && !info->single_link_status[ind].errors_bm)
    {
      continue;
    }
    if (!(info->single_link_status[ind].crc_err_counter))
    {
      sal_sprintf(crc_err_cnt, "%s",  (info->single_link_status[ind].errors_bm & SOC_DNX_LINK_ERR_TYPE_CRC)?"   ***    ":"     -    ");
    }
    else
    {
      sal_sprintf(crc_err_cnt, "%10u", info->single_link_status[ind].crc_err_counter);
    }
    LOG_CLI((BSL_META_U(NO_UNIT,
                        " %2d | %2d |  %s   | %s  |  %s   |%s|%s | %s  | %s |%s|%s|   %2u    \n\r"),
             ind,
             soc_jer2_tmcfbr2srd_lane_id(ind),
             (info->single_link_status[ind].indications_bm & SOC_DNX_LINK_INDICATE_TYPE_SIG_LOCK)?" + ":"***",
             (info->single_link_status[ind].indications_bm & SOC_DNX_LINK_INDICATE_TYPE_ACCEPT_CELL)?" + ":"***",
             (info->single_link_status[ind].indications_bm & SOC_DNX_LINK_INDICATE_INTRNL_FIXED)?"***":" - ",
             crc_err_cnt,
             (info->single_link_status[ind].errors_bm & SOC_DNX_LINK_ERR_TYPE_SIZE)?"***":" - ",
             (info->single_link_status[ind].errors_bm & SOC_DNX_LINK_ERR_TYPE_MISALIGN)?"***":" - ",
             (info->single_link_status[ind].errors_bm & SOC_DNX_LINK_ERR_TYPE_CODE_GROUP)?"***":" - ",
             (info->single_link_status[ind].up_tx)?" + ":"***",
             (info->single_link_status[ind].up_rx)?" + ":"***",
             info->single_link_status[ind].leaky_bucket_counter
             ));
  }

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Number of valid links: %d\n\r"),nof_valid_links));
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */

