#include <soc/mcm/memregs.h>
/* $Id: jer2_arad_general.c,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_COMMON

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/legacy/error.h>

#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/ARAD/arad_chip_defines.h>
#include <soc/dnx/legacy/ARAD/arad_chip_regs.h>
#include <soc/dnx/legacy/ARAD/arad_chip_tbls.h>
#include <soc/dnx/legacy/ARAD/arad_reg_access.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>

#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>
#include <soc/dnx/legacy/SAND/Management/sand_device_management.h>
#include <soc/dnx/legacy/SAND/Management/sand_low_level.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Utils/sand_bitstream.h>
#include <soc/dnx/legacy/SAND/Utils/sand_conv.h>
#include <soc/dnx/legacy/SAND/Management/sand_low_level.h>
#include <soc/dnx/legacy/mbcm.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Numeric correction used in Fabric Multicast rate calculation
 */
#define JER2_FMC_RATE_DELTA_CONST  (7)

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

typedef shr_error_e (*JER2_ARAD_SEND_MESSAGE_TO_QUEUE_FUNC)(DNX_SAND_IN  uint32 msg);

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
#if (0)
/* { */
int
  jer2_arad_fap_port_id_verify(
    DNX_SAND_IN  int          unit,
    DNX_SAND_IN  JER2_ARAD_FAP_PORT_ID  port_id
  )
{

  SHR_FUNC_INIT_VARS(unit);

  DNX_SAND_ERR_IF_ABOVE_MAX(
    port_id, JER2_ARAD_MAX_FAP_PORT_ID,
    JER2_ARAD_FAP_PORT_ID_INVALID_ERR,10,exit
  );

exit:
  SHR_FUNC_EXIT;
}
/* } */
#endif

const char*
  jer2_arad_JER2_ARAD_INTERFACE_TYPE_to_string(
    DNX_SAND_IN  JER2_ARAD_INTERFACE_TYPE enum_val
  )
{
  return SOC_DNX_INTERFACE_TYPE_to_string(enum_val);
}

const char*
  jer2_arad_JER2_ARAD_INTERFACE_ID_to_string(
    DNX_SAND_IN  JER2_ARAD_INTERFACE_ID enum_val
  )
{
  return SOC_DNX_INTERFACE_ID_to_string(enum_val);
}

const char*
  jer2_arad_JER2_ARAD_FC_DIRECTION_to_string(
    DNX_SAND_IN  JER2_ARAD_FC_DIRECTION enum_val
  )
{
  return SOC_DNX_FC_DIRECTION_to_string(enum_val);
}

const char*
  jer2_arad_JER2_ARAD_COMBO_QRTT_to_string(
    DNX_SAND_IN  JER2_ARAD_COMBO_QRTT enum_val
  )
{
  return SOC_DNX_COMBO_QRTT_to_string(enum_val);
}

const char*
  jer2_arad_JER2_ARAD_DEST_TYPE_to_string(
    DNX_SAND_IN JER2_ARAD_DEST_TYPE enum_val,
    DNX_SAND_IN uint8       short_name
  )
{
  return SOC_DNX_DEST_TYPE_to_string(enum_val, short_name);
}

const char*
  jer2_arad_JER2_ARAD_DEST_SYS_PORT_TYPE_to_string(
    DNX_SAND_IN  JER2_ARAD_DEST_SYS_PORT_TYPE enum_val
  )
{
  return SOC_DNX_DEST_SYS_PORT_TYPE_to_string(enum_val);
}

const char*
  jer2_arad_JER2_ARAD_CONNECTION_DIRECTION_to_string(
    DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION enum_val
  )
{
  return SOC_DNX_CONNECTION_DIRECTION_to_string(enum_val);
}

void
  jer2_arad_JER2_ARAD_INTERFACE_ID_print(
    DNX_SAND_IN JER2_ARAD_INTERFACE_ID if_ndx
  )
{
  LOG_CLI((BSL_META("Interface index: %s\n\r"),jer2_arad_JER2_ARAD_INTERFACE_ID_to_string(if_ndx)));
}


const char*
  jer2_arad_JER2_ARAD_SWAP_MODE_to_string(
    DNX_SAND_IN  JER2_ARAD_SWAP_MODE enum_val
  )
{
  return SOC_DNX_SWAP_MODE_to_string(enum_val);
}

/* } */


