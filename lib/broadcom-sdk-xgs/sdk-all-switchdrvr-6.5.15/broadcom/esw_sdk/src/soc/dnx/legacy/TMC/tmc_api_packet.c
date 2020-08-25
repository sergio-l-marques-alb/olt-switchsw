/* $Id: jer2_tmc_api_packet.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_packet.c
*
* MODULE PREFIX:  soc_jer2_tmcpkt
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
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>

#include <soc/dnx/legacy/TMC/tmc_api_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_packet.h>

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
  SOC_DNX_PKT_PACKET_BUFFER_clear(
    DNX_SAND_OUT SOC_DNX_PKT_PACKET_BUFFER *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_PKT_PACKET_BUFFER));
  for (ind = 0; ind < SOC_DNX_PKT_MAX_CPU_PACKET_BYTE_SIZE; ++ind)
  {
    info->data[ind] = 0;
  }
  info->data_byte_size = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_PKT_TX_PACKET_INFO_clear(
    DNX_SAND_OUT SOC_DNX_PKT_TX_PACKET_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_PKT_TX_PACKET_INFO));

  SOC_DNX_PKT_PACKET_BUFFER_clear(&(info->packet));
  info->path_type = SOC_DNX_PACKET_SEND_NOF_PATH_TYPES;

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_PKT_RX_PACKET_INFO_clear(
    DNX_SAND_OUT SOC_DNX_PKT_RX_PACKET_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_PKT_RX_PACKET_INFO));

  SOC_DNX_PKT_PACKET_BUFFER_clear(&(info->packet));

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_PKT_PACKET_TRANSFER_clear(
    DNX_SAND_OUT SOC_DNX_PKT_PACKET_TRANSFER *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_PKT_PACKET_TRANSFER));
  info->packet_send = NULL;
  info->packet_recv = NULL;

exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_PACKET_SEND_PATH_TYPE_to_string(
    DNX_SAND_IN SOC_DNX_PACKET_SEND_PATH_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_PACKET_SEND_PATH_TYPE_INGRESS:
    str = "ingress";
  break;
  case SOC_DNX_PACKET_SEND_PATH_TYPE_EGRESS:
    str = "egress";
  break;
  default:
    str = "Unknown";
  }
  return str;
}

const char*
  SOC_DNX_PKT_PACKET_RECV_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_PKT_PACKET_RECV_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_PKT_PACKET_RECV_MODE_MSB_TO_LSB:
    str = "msb_to_lsb";
  break;
  case SOC_DNX_PKT_PACKET_RECV_MODE_LSB_TO_MSB:
    str = "lsb_to_msb";
  break;
  default:
    str = " Unknown";
  }
  return str;
}
void
  SOC_DNX_PKT_PACKET_BUFFER_print(
    DNX_SAND_IN SOC_DNX_PKT_PACKET_BUFFER *info,
    DNX_SAND_IN SOC_DNX_PKT_PACKET_RECV_MODE recv_to_msb
  )
{
  uint32 ind=0;
  char next_char;
  static int32 bytes_per_row = 10;


  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  if (info->data_byte_size > SOC_DNX_PKT_MAX_CPU_PACKET_BYTE_SIZE)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Invalid data size (%u)\n\r"
                        "Maximal allowed is %u"),
             info->data_byte_size,
             SOC_DNX_PKT_MAX_CPU_PACKET_BYTE_SIZE
             ));
    SOC_DNX_DO_NOTHING_AND_EXIT;
  }

  for (ind=0; ind < info->data_byte_size; ++ind)
  {
    if (recv_to_msb == SOC_DNX_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
    {
      next_char = info->data[SOC_DNX_PKT_MAX_CPU_PACKET_BYTE_SIZE - ind - 1];
    }
    else
    {
      next_char = info->data[ind];
    }

    if ((ind % bytes_per_row) == 0)
    {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "Data[%04u-%04u]:   "), ind, DNX_SAND_MIN((ind+bytes_per_row-1), (info->data_byte_size-1))));
    }

    LOG_CLI((BSL_META_U(NO_UNIT,
                        "%02x "), next_char));

    if ((((ind+1) % bytes_per_row) == 0) || ((ind+1) == info->data_byte_size))
    {
      LOG_CLI((BSL_META_U(NO_UNIT,
                          "\n\r")));
    }
  }

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Data_byte_size: %u\n\r"),info->data_byte_size));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_PKT_TX_PACKET_INFO_print(
    DNX_SAND_IN SOC_DNX_PKT_TX_PACKET_INFO *info,
    DNX_SAND_IN SOC_DNX_PKT_PACKET_RECV_MODE recv_to_msb
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_PKT_PACKET_BUFFER_print(&(info->packet), recv_to_msb);
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Path type:     %s \n\r"),
           SOC_DNX_PACKET_SEND_PATH_TYPE_to_string(info->path_type)
           ));

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_PKT_RX_PACKET_INFO_print(
    DNX_SAND_IN SOC_DNX_PKT_RX_PACKET_INFO *info,
    DNX_SAND_IN SOC_DNX_PKT_PACKET_RECV_MODE recv_to_msb
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_PKT_PACKET_BUFFER_print(&(info->packet), recv_to_msb);

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_PKT_PACKET_TRANSFER_print(
    DNX_SAND_IN SOC_DNX_PKT_PACKET_TRANSFER *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


