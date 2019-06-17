/* $Id: jer2_tmc_api_diagnostics.c,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_diagnostics.c
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


#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>
#include <soc/dnx/legacy/SAND/Management/sand_low_level.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_params.h>
#include <soc/dnx/legacy/SAND/Management/sand_callback_handles.h>

#include <soc/dnx/legacy/SAND/Management/sand_device_management.h>

#include <soc/dnx/legacy/SAND/SAND_FM/sand_trigger.h>
#include <soc/dnx/legacy/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Utils/sand_bitstream.h>
#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>

#include <soc/dnx/legacy/TMC/tmc_api_diagnostics.h>
#include <soc/dnx/legacy/TMC/tmc_api_statistics.h>
#include <soc/dnx/legacy/TMC/tmc_api_general.h>

#include <soc/dnx/legacy/TMC/tmc_api_ingress_packet_queuing.h>

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
  SOC_DNX_DIAG_SOFT_ERR_RESULT_clear(
    DNX_SAND_OUT SOC_DNX_DIAG_SOFT_ERR_RESULT *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_DIAG_SOFT_ERR_RESULT));
  info->err_sp = 0;
  info->err_dp = 0;
  info->err_rf = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_SOFT_SMS_RESULT_clear(
    DNX_SAND_OUT SOC_DNX_DIAG_SOFT_SMS_RESULT *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_DIAG_SOFT_SMS_RESULT));
  for (ind = 0; ind < SOC_DNX_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S; ++ind)
  {
    info->diag_chain[ind] = 0;
  }
  SOC_DNX_DIAG_SOFT_ERR_RESULT_clear(&(info->nof_errs));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_LBG_PACKET_PATTERN_print(
    DNX_SAND_IN SOC_DNX_DIAG_LBG_PACKET_PATTERN *info
  )
{
  uint32 ind=0;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (ind=0; ind<info->data_byte_size; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Data[%u]:       %02x\n\r"), ind,info->data[ind]));
  }
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Data_byte_size: %u\n\r"),info->data_byte_size));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_LBG_TRAFFIC_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_LBG_TRAFFIC_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Nof_packets: %u[Packets]\n\r"),info->nof_packets));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Packet_size: %u[Bytes]\n\r"),info->packet_size));
  SOC_DNX_DIAG_LBG_PACKET_PATTERN_print(&info->pattern);
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_LBG_PATH_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_LBG_PATH_INFO *info
  )
{
  uint32
    indx;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  for (indx = 0; indx < info->nof_ports; ++indx)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "%u \n\r"), info->ports[indx] ));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_LBG_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_LBG_INFO *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_DIAG_LBG_PATH_INFO_print(&info->path);

exit:
  SHR_VOID_FUNC_EXIT;
}

const char*
  SOC_DNX_DIAG_QDR_BIST_ADDR_MODE_to_string(
    DNX_SAND_IN SOC_DNX_DIAG_QDR_BIST_ADDR_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DIAG_QDR_BIST_ADDR_MODE_NORMAL:
    str = "QDR_BIST_ADDR_MODE_NORMAL";
  break;

  case SOC_DNX_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_SHIFT:
    str = "QDR_BIST_ADDR_MODE_ADDRESS_SHIFT";
  break;

  case SOC_DNX_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST:
    str = "QDR_BIST_ADDR_MODE_ADDRESS_TEST";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_DIAG_QDR_BIST_DATA_MODE_to_string(
    DNX_SAND_IN SOC_DNX_DIAG_QDR_BIST_DATA_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DIAG_QDR_BIST_DATA_MODE_NORMAL:
    str = "QDR_BIST_DATA_MODE_NORMAL";
  break;

  case SOC_DNX_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT:
    str = "QDR_BIST_DATA_MODE_PATTERN_BIT";
  break;

  case SOC_DNX_DIAG_QDR_BIST_DATA_MODE_RANDOM:
    str = "QDR_BIST_DATA_MODE_RANDOM";
  break;

  case SOC_DNX_DIAG_QDR_BIST_DATA_MODE_DATA_SHIFT:
    str = "QDR_BIST_DATA_MODE_DATA_SHIFT";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_DIAG_BIST_DATA_PATTERN_to_string(
    DNX_SAND_IN SOC_DNX_DIAG_BIST_DATA_PATTERN enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DIAG_BIST_DATA_PATTERN_DIFF:
    str = "BIST_DATA_PATTERN_DIFF";
  break;

  case SOC_DNX_DIAG_BIST_DATA_PATTERN_ONE:
    str = "BIST_DATA_PATTERN_ONE";
    break;

  case SOC_DNX_DIAG_BIST_DATA_PATTERN_ZERO:
    str = "BIST_DATA_PATTERN_ZERO";
    break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_DIAG_DRAM_BIST_DATA_MODE_to_string(
    DNX_SAND_IN SOC_DNX_DIAG_DRAM_BIST_DATA_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DIAG_DRAM_BIST_DATA_MODE_NORMAL:
    str = "DRAM_BIST_DATA_MODE_NORMAL";
  break;

  case SOC_DNX_DIAG_DRAM_BIST_DATA_MODE_PATTERN_BIT:
    str = "DRAM_BIST_DATA_MODE_PATTERN_BIT";
  break;

  case SOC_DNX_DIAG_DRAM_DATA_MODE_RANDOM:
    str = "DRAM_DATA_MODE_RANDOM";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_DIAG_SOFT_ERROR_PATTERN_to_string(
    DNX_SAND_IN  SOC_DNX_DIAG_SOFT_ERROR_PATTERN enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DIAG_SOFT_ERROR_PATTERN_ONE:
    str = "one";
  break;
  case SOC_DNX_DIAG_SOFT_ERROR_PATTERN_ZERO:
    str = "zero";
  break;
  case SOC_DNX_DIAG_SOFT_ERROR_PATTERN_DIFF1:
    str = "diff1";
  break;
  case SOC_DNX_DIAG_SOFT_ERROR_PATTERN_DIFF2:
    str = "diff2";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_DIAG_SOFT_COUNT_TYPE_to_string(
    DNX_SAND_IN  SOC_DNX_DIAG_SOFT_COUNT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DIAG_SOFT_COUNT_TYPE_FAST:
    str = "fast";
  break;
  case SOC_DNX_DIAG_SOFT_COUNT_TYPE_COMPLETE:
    str = "complete";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_DIAG_QDR_BIST_TEST_RUN_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_QDR_BIST_TEST_RUN_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Nof Write Commands: %u[Commands]\n\r"),info->nof_cmnds_write));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Nof Write Commands: %u[Commands]\n\r"),info->nof_cmnds_read));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Start_addr: %u\n\r"),info->start_addr));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "End_addr: %u\n\r"),info->end_addr));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Read Offset: %u[Commands]\n\r"),info->read_offset));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Data_mode %s \n\r"),
           SOC_DNX_DIAG_QDR_BIST_DATA_MODE_to_string(info->data_mode)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Address_mode %s \n\r"),
           SOC_DNX_DIAG_QDR_BIST_ADDR_MODE_to_string(info->address_mode)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Data_pattern %s \n\r"),
           SOC_DNX_DIAG_BIST_DATA_PATTERN_to_string(info->data_pattern)
           ));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_QDR_BIST_TEST_RES_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_QDR_BIST_TEST_RES_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  if (info->is_test_finished == FALSE)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Test not finished...\n\r")));
  }
  else
  {
    if (info->is_qdr_up == FALSE)
    {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "QDR interface is down!\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "QDR interface is up: \n\r")));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Bit_err_counter: %u[Bits]\n\r"),info->bit_err_counter));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Reply_err_counter: %u[Errors]\n\r"),info->reply_err_counter));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Bits_error_bitmap: %u\n\r"),info->bits_error_bitmap));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Last_addr_err: %u\n\r"),info->last_addr_err));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Last_data_err: %u\n\r"),info->last_data_err));
    }
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DRAM_BIST_TEST_RUN_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Finite_nof_cmnds: %u[Commands]\n\r"),info->is_finite_nof_cmnds));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Writes_per_cycle: %u[Write commands]\n\r"),info->writes_per_cycle));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Reads_per_cycle: %u[Read commands]\n\r"),info->reads_per_cycle));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Start_addr: %u\n\r"),info->start_addr));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "End_addr: %u\n\r"),info->end_addr));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "data_mode %s \n\r"),
           SOC_DNX_DIAG_DRAM_BIST_DATA_MODE_to_string(info->data_mode)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "data_pattern %s \n\r"),
           SOC_DNX_DIAG_BIST_DATA_PATTERN_to_string(info->data_pattern)
           ));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DRAM_BIST_TEST_RES_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_DRAM_BIST_TEST_RES_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  if (info->is_test_finished == FALSE)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Test not finished...\n\r")));
  }
  else
  {
    if (info->is_dram_up == FALSE)
    {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "DRAM interface is down!\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "DRAM interface is up. ")));
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Error counter: %u[Errors]\n\r"),info->reply_err_counter));
    }
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DRAM_ACCESS_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_DRAM_ACCESS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "address: %u\n\r"),info->address));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_data_size_bits_256_not_32: %u\n\r"),info->is_data_size_bits_256_not_32));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_infinite_nof_actions: %u\n\r"),info->is_infinite_nof_actions));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DRAM_READ_COMPARE_STATUS_print(
    DNX_SAND_IN SOC_DNX_DIAG_DRAM_READ_COMPARE_STATUS *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "success: %u\n\r"),info->success));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "error_bits_global: %u\n\r"),info->error_bits_global));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "nof_addr_with_errors: %u\n\r"),info->nof_addr_with_errors));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DLL_STATUS_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_DLL_STATUS_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL control 0                         : 0x%08X\n\r"),info->ddl_control_0));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL control 1                         : 0x%08X\n\r"),info->ddl_control_1));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL control 2                         : 0x%08X\n\r"),info->ddl_control_2));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DLL finished initialization           : %u\n\r"),info->dll_init_done));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Round trip delay value                : 0x%04X\n\r"), info->rnd_trp));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Round trip diff delay value           : 0x%04X\n\r"), info->rnd_trp_diff));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selection down indication       : 0x%04X\n\r"), info->dll_ph_dn));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selection up indication         : 0x%04X\n\r"), info->dll_ph_up));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Output selected phase                 : 0x%04X\n\r"), info->main_ph_sel));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selected for sync clock 2       : 0x%04X\n\r"), info->ph2sel));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Half cycle count vector               : 0x%04X\n\r"), info->hc_sel_vec));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Quarter cycle count vector            : 0x%04X\n\r"), info->qc_sel_vec));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Count vector                          : 0x%04X\n\r"), info->sel_vec));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Half granularity indication           : %u\n\r"),info->sel_hg));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selected for half cycle up      : %u\n\r"),info->ph_sel_hc_up));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Insertion delay compensation vector   : 0x%04X\n\r"), info->ins_dly_min_vec));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selection offset                : %u\n\r"),info->ddl_init_main_ph_sel_ofst));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL phase selected for half cycle up  : %u\n\r"),info->ddl_ph_sel_hc_up));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL train trigger up limit            : %u\n\r"),info->ddl_train_trig_up_limit));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL train trigger down limit          : %u\n\r"),info->ddl_train_trig_dn_limit));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selection error                 : %u\n\r"),info->ph_sel_err));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Delay max min mode                    : %u\n\r"),info->dly_max_min_mode));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Phase selected                        : 0x%04X\n\r"), info->ph_sel));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DRAM_STATUS_INFO_print(
    DNX_SAND_IN SOC_DNX_DIAG_DRAM_STATUS_INFO *info
  )
{
  uint32
    ind,
    ind2;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "\n\r")));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DRAM training sequence control        : 0x%08X\n\r"),info->training_seq));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Io calibration status                 : 0x%08X\n\r"),info->calibration_st));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DDL periodic training                 : 0x%08X\n\r"),info->ddl_periodic_training));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "DLL master control vector             : 0x%04X\n\r"), info->dll_mstr_s));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "\n\r")));

  for (ind=0; ind<SOC_DNX_DIAG_NOF_DDR_TRAIN_SEQS; ++ind)
  {
    ind2 = SOC_DNX_DIAG_NOF_DDR_TRAIN_SEQS - ind - 1;
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "DDR training sequence[%u]              : 0x%08X\n\r"),ind2,info->ddr_training_sequence[ind2]));
  }

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "\n\r")));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "--PER-DLL STATUS--\n\r")));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "\n\r")));

  for (ind=0; ind<SOC_DNX_DIAG_NOF_DLLS_PER_DRAM; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "--DLL %u--\n\r"),ind));
    SOC_DNX_DIAG_DLL_STATUS_INFO_print(&(info->dll_status[ind]));
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "\n\r")));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_DRAM_ERR_INFO_print(
    DNX_SAND_IN  SOC_DNX_DIAG_DRAM_ERR_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "bit_err_bitmap: %u\n\r"),info->bit_err_bitmap));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_clocking_err: %u\n\r"),info->is_clocking_err));
  for (ind = 0; ind < DIAG_DRAM_NOF_DQSS; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "is_dqs_con_err[%u]: %u\n\r"),ind,info->is_dqs_con_err[ind]));
  }
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_phy_ready_err: %u\n\r"),info->is_phy_ready_err));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_rtt_avg_min_err: %u\n\r"),info->is_rtt_avg_min_err));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_rtt_avg_max_err: %u\n\r"),info->is_rtt_avg_max_err));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_SOFT_ERR_INFO_print(
    DNX_SAND_IN  SOC_DNX_DIAG_SOFT_ERR_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "pattern %s "), SOC_DNX_DIAG_SOFT_ERROR_PATTERN_to_string(info->pattern)));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "sms: %u\n\r"),info->sms));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_SOFT_ERR_RESULT_print(
    DNX_SAND_IN  SOC_DNX_DIAG_SOFT_ERR_RESULT *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "err_sp: %u, "),info->err_sp));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "err_dp: %u, "),info->err_dp));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "err_rf: %u\n\r"),info->err_rf));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_DIAG_SOFT_SMS_RESULT_print(
    DNX_SAND_IN  uint32                  sms_ndx,
    DNX_SAND_IN  uint32                  sone_ndx,
    DNX_SAND_IN  SOC_DNX_DIAG_SOFT_SMS_RESULT *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");


exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */

void
  SOC_DNX_DIAG_LAST_PACKET_INFO_clear(
    DNX_SAND_OUT SOC_DNX_DIAG_LAST_PACKET_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_DIAG_LAST_PACKET_INFO));
  info->tm_port = 0;
  info->pp_port = 0;
  info->is_valid = FALSE;
  for (ind = 0; ind < SOC_DNX_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++ind)
  {
    info->buffer[ind] = 0;
  }
  info->packet_size = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}


void
  SOC_DNX_DIAG_LAST_PACKET_INFO_print(
    DNX_SAND_IN  SOC_DNX_DIAG_LAST_PACKET_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "tm_port: %u\n\r"),info->tm_port));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "pp_port: %u\n\r"),info->pp_port));
  for (ind = 0; ind < SOC_DNX_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "buffer[%u]: %08x\n\r"), ind,info->buffer[ind]));
  }
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "packet_size: %d\n\r"), info->packet_size));
exit:
  SHR_VOID_FUNC_EXIT;
}



