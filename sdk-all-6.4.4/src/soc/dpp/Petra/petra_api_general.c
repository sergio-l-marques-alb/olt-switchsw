/* $Id: petra_api_general.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#endif

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

/*********************************************************************
*     This procedure is called from the user system ISR when
*     interrupt handling for SOC_PETRA is required.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_interrupt_handler(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_HANDLER);

  SOC_SAND_INTERRUPTS_STOP;

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_interrupt_handler_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_handler()",0,0);
}

SOC_PETRA_FAR_DEVICE_TYPE
  soc_petra_device_type_get(
    SOC_SAND_IN  uint32 internal_device_type
  )
{

  switch(internal_device_type)
  {
  case 0:
  case 1:
  case 4:
  case 5:
    return SOC_PETRA_FAR_DEVICE_TYPE_FAP;
  case 2:
    return SOC_PETRA_FAR_DEVICE_TYPE_FE3;
  case 6:
    return SOC_PETRA_FAR_DEVICE_TYPE_FE1;
  case 3:
  case 7:
    return SOC_PETRA_FAR_DEVICE_TYPE_FE2;
  default:
    return SOC_PETRA_FAR_NOF_DEVICE_TYPES;
  }
}
/*********************************************************************
*     Verifies validity of port id
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fap_port_id_verify(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port_id,
    SOC_SAND_IN  uint8          is_per_port_if_fatp
  )
{
  uint32
    fatp_nof_ports;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FAP_PORT_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_id, SOC_PETRA_MAX_FAP_PORT_ID,
    SOC_PETRA_FAP_PORT_ID_INVALID_ERR,10,exit
  );

  /*
   *  If FAT-PIPE is enabled, Fat pipe configuration is typically performed
   *  on port index 1 only. The exception is API-s that configure blocks close to Soc_petra
   *  interfaces (e.g. port to interface mapping, port type etc.) - for those API-s,
   *  is_per_port_if_fatp is TRUE.
   *  If FAT-PIPE is enabled, ports 2-4 can not be used (see the exception above).
   */
  if (!is_per_port_if_fatp)
  {
    fatp_nof_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

    if ( (fatp_nof_ports != 0) &&
         (SOC_PETRA_IS_FAT_PIPE_PORT_ID(port_id) &&
         !SOC_PETRA_IS_FAT_PIPE_FIRST(port_id))
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAP_PORT_ID_INVALID_WHEN_FAT_PIPE_ENABLED_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fap_port_id_verify()",port_id,0);
}

uint32
  soc_petra_drop_precedence_verify(
    SOC_SAND_IN  uint32      dp_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DROP_PRECEDENCE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    dp_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DROP_PRECEDENCE_OUT_OF_RANGE_ERR,10,exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fap_port_id_verify()",dp_ndx,0);
}

uint32
  soc_petra_traffic_class_verify(
    SOC_SAND_IN  uint32      tc_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TRAFFIC_CLASS_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tc_ndx, SOC_PETRA_TR_CLS_MAX,
    SOC_PETRA_TRAFFIC_CLASS_OUT_OF_RANGE_ERR,10,exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_traffic_class_verify()",tc_ndx,0);
}


SOC_PETRA_MAL_EQUIVALENT_TYPE
  soc_petra_mal_type_from_id(
    SOC_SAND_IN uint32 mal_id
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type = SOC_PETRA_MAL_TYPE_NONE;

  if (mal_id == SOC_PETRA_MAL_ID_CPU)
  {
    mal_type = SOC_PETRA_MAL_TYPE_CPU;
  }
  else if (mal_id == SOC_PETRA_MAL_ID_OLP)
  {
    mal_type = SOC_PETRA_MAL_TYPE_OLP;
  }
  else if (mal_id == SOC_PETRA_MAL_ID_RCY)
  {
    mal_type = SOC_PETRA_MAL_TYPE_RCY;
  }
  else if (mal_id == SOC_PETRA_MAL_ID_ERP)
  {
    mal_type = SOC_PETRA_MAL_TYPE_ERP;
  }
  else if (mal_id < SOC_PETRA_NOF_MAC_LANES)
  {
    mal_type = SOC_PETRA_MAL_TYPE_NIF;
  }

  return mal_type;

}

uint32
  soc_pa_if_type_from_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     if_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_TYPE   *interface_type
  )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IF_TYPE_FROM_ID);
  
  SOC_SAND_CHECK_NULL_INPUT(interface_type);

  *interface_type = SOC_PETRA_IF_TYPE_NONE;

  if (SOC_PETRA_IS_CPU_IF_ID(if_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_CPU;
  }
  else if (SOC_PETRA_IS_OLP_IF_ID(if_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_OLP;
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(if_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_RCY;
  }
  else if (SOC_PETRA_IS_ERP_IF_ID(if_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_ERP;
  }
  else if (SOC_PETRA_IS_NIF_ID(if_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_NIF;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_if_type_from_id()",if_id,0);
}

uint32
  soc_petra_if_type_from_id(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     if_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_TYPE   *interface_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IF_TYPE_FROM_ID);

  SOC_PETRA_DIFF_DEVICE_CALL(if_type_from_id,(if_id, interface_type));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_if_type_from_id()",if_id,0);
}
 

/*****************************************************
* NAME
*    soc_petra_interface_id_verify
* TYPE:
*   PROC
* DATE:
*   03/12/2007
* FUNCTION:
*   Input Verification for interface index
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_pa_interface_id_verify(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx
  )
{
  uint8
    not_found = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERFACE_ID_VERIFY);

  if (SOC_PETRA_IS_NIF_ID(if_ndx))
  {
    not_found = FALSE;
  }
  else if (SOC_PETRA_IS_CPU_IF_ID(if_ndx))
  {
    not_found = FALSE;
  }
  else if (SOC_PETRA_IS_OLP_IF_ID(if_ndx))
  {
    not_found = FALSE;
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(if_ndx))
  {
    not_found = FALSE;
  }
  else if (SOC_PETRA_IS_ERP_IF_ID(if_ndx))
  {
    not_found = FALSE;
  }
  else if (SOC_PETRA_IS_NONE_IF_ID(if_ndx))
  {
    not_found = FALSE;
  }
  else
  {
    not_found = TRUE;
  }

  if(not_found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_interface_id_verify()",if_ndx,0);
}

uint32
  soc_petra_interface_id_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     if_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERFACE_ID_VERIFY);

  SOC_PETRA_DIFF_DEVICE_CALL(interface_id_verify,(if_ndx));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interface_id_verify(unit, )",if_ndx,0);
}

uint32
  soc_pa_mal_equivalent_id_verify(
    SOC_SAND_IN  uint32 mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_MAL_EQUIVALENT_ID_VERIFY);

  if (
      (mal_ndx != SOC_PETRA_MAL_ID_CPU) &&
      (mal_ndx != SOC_PETRA_MAL_ID_OLP) &&
      (mal_ndx != SOC_PETRA_MAL_ID_RCY) &&
      (mal_ndx != SOC_PETRA_MAL_ID_ERP)
     )
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
      SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_mal_equivalent_id_verify()",mal_ndx,0);
}

uint32
  soc_petra_mal_equivalent_id_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mal_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MAL_EQUIVALENT_ID_VERIFY);

  SOC_PETRA_DIFF_DEVICE_CALL(mal_equivalent_id_verify,(mal_ndx));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mal_equivalent_id_verify(unit, )",0,0);
}

uint8
  soc_petra_is_channelized_nif_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx
  )
{

  uint8
    is_channelized_nif = FALSE;

  is_channelized_nif = SOC_PETRA_IS_MAL_NIF_ID(if_ndx);

  return is_channelized_nif;
}

/* $Id: petra_api_general.c,v 1.10 Broadcom SDK $
 *  TRUE if the interface index can belong to a channelized interface
 */
uint8
  soc_petra_is_channelized_interface_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx
  )
{
  uint8
    res = FALSE;

  if (SOC_PETRA_IS_CPU_IF_ID(if_ndx) ||
      SOC_PETRA_IS_RCY_IF_ID(if_ndx))
  {
    res = TRUE;
  }
  else if (soc_petra_is_channelized_nif_id(if_ndx))
  {
    res = TRUE;
  }
  else
  {
    res = FALSE;
  }

  return res;
}

uint32
  soc_petra_dest_info_verify(
    SOC_SAND_IN  SOC_PETRA_DEST_INFO      *destination
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DEST_INFO_VERIFY);

  switch(destination->type) {
  case SOC_PETRA_DEST_TYPE_QUEUE:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      destination->type, SOC_PETRA_MAX_QUEUE_ID,
      SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit
    );
    break;
  case SOC_PETRA_DEST_TYPE_MULTICAST:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      destination->type, SOC_PETRA_MULT_ID_MAX,
      SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR, 20, exit
    );
    break;
  case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      destination->id, SOC_PETRA_NOF_SYS_PHYS_PORTS,
      SOC_PETRA_SYS_PHYSICAL_PORT_NDX_OUT_OF_RANGE_ERR, 30, exit
    );
    break;
  case SOC_PETRA_DEST_TYPE_LAG:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      destination->type, SOC_PETRA_MAX_LAG_GROUP_ID,
      SOC_PETRA_LAG_GROUP_ID_OUT_OF_RANGE_ERR,40,exit
    );
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DEST_TYPE_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dest_info_verify()",0,0);
}

uint32
  soc_petra_dest_sys_port_info_verify(
    SOC_SAND_IN  SOC_PETRA_DEST_SYS_PORT_INFO      *dest_sys_port
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DEST_SYS_PORT_INFO_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    dest_sys_port->type, (SOC_PETRA_DEST_SYS_PORT_NOF_TYPES-1),
      SOC_PETRA_DEST_SYS_PORT_TYPE_OUT_OF_RANGE_ERR, 10, exit
    );

  if (dest_sys_port->type == SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      dest_sys_port->id, SOC_PETRA_NOF_SYS_PHYS_PORTS,
      SOC_PETRA_SYS_PHYSICAL_PORT_NDX_OUT_OF_RANGE_ERR, 20, exit
      );
    SOC_SAND_ERR_IF_ABOVE_MAX(
      dest_sys_port->member_id, 0,
      SOC_PETRA_LAG_ENTRY_ID_OUT_OF_RANGE_ERR, 30, exit
      );
  }

  if (dest_sys_port->type == SOC_PETRA_DEST_SYS_PORT_TYPE_LAG)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      dest_sys_port->id, SOC_PETRA_MAX_LAG_GROUP_ID,
      SOC_PETRA_LAG_GROUP_ID_OUT_OF_RANGE_ERR,40,exit
      );
    SOC_SAND_ERR_IF_ABOVE_MAX(
      dest_sys_port->member_id, SOC_PETRA_MAX_LAG_ENTRY_ID,
      SOC_PETRA_LAG_ENTRY_ID_OUT_OF_RANGE_ERR, 50, exit
      );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dest_sys_port_info_verify()",0,0);
}

void
  soc_petra_PETRA_DEST_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_SYS_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DEST_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_THRESH_WITH_HYST_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_THRESH_WITH_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_THRESH_WITH_HYST_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_FAR_DEVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_FAR_DEVICE_TYPE enum_val
  )
{
  return SOC_TMC_FAR_DEVICE_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_INTERFACE_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_TYPE enum_val
  )
{
  return SOC_TMC_INTERFACE_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_INTERFACE_ID_to_string(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID enum_val
  )
{
  return SOC_TMC_INTERFACE_ID_to_string(enum_val);
}

const char*
  soc_petra_PETRA_FC_DIRECTION_to_string(
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION enum_val
  )
{
  return SOC_TMC_FC_DIRECTION_to_string(enum_val);
}

const char*
  soc_petra_PETRA_COMBO_QRTT_to_string(
    SOC_SAND_IN  SOC_PETRA_COMBO_QRTT enum_val
  )
{
  return SOC_TMC_COMBO_QRTT_to_string(enum_val);
}

const char*
  soc_petra_PETRA_DEST_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_DEST_TYPE enum_val,
    SOC_SAND_IN uint8       short_name
  )
{
  return SOC_TMC_DEST_TYPE_to_string(enum_val, short_name);
}

const char*
  soc_petra_PETRA_DEST_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_DEST_SYS_PORT_TYPE enum_val
  )
{
  return SOC_TMC_DEST_SYS_PORT_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_CONNECTION_DIRECTION_to_string(
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION enum_val
  )
{
  return SOC_TMC_CONNECTION_DIRECTION_to_string(enum_val);
}

void
  soc_petra_PETRA_INTERFACE_ID_print(
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID if_ndx
  )
{
  soc_sand_os_printf("Interface index: %s\n\r",soc_petra_PETRA_INTERFACE_ID_to_string(if_ndx));
}

void
  soc_petra_PETRA_DEST_SYS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_SYS_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DEST_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_THRESH_WITH_HYST_INFO_print(
    SOC_SAND_IN SOC_PETRA_THRESH_WITH_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_THRESH_WITH_HYST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DEST_SYS_PORT_INFO_table_format_print(
    SOC_SAND_IN SOC_PETRA_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_SYS_PORT_INFO_table_format_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /*SOC_PETRA_DEBUG_IS_LVL1*/
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
