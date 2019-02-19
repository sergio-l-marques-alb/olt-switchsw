/* $Id: petra_api_ports.c,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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

#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/petra_ports.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#endif

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_general.h>

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
*     Map System Physical FAP Port to a <mapped_fap_id, mapped_fap_port_id>
*     pair. The mapping is unique - single System Physical
*     Port is mapped to a single local port per specified
*     device. This configuration effects: 1. Resolving
*     destination FAP Id and OFP Id 2. Per-port pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sys_phys_to_local_port_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_sys_phys_to_local_port_map_verify(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_sys_phys_to_local_port_map_set_unsafe(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sys_phys_to_local_port_map_set()",0,0);
}

/*********************************************************************
*     Map System Physical FAP Port to a <mapped_fap_id, mapped_fap_port_id>
*     pair. The mapping is unique - single System Physical
*     Port is mapped to a single local port per specified
*     device. This configuration effects: 1. Resolving
*     destination FAP Id and OFP Id 2. Per-port pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sys_phys_to_local_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_id);
  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_port_id);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sys_phys_to_local_port_map_get()",0,0);
}

/*********************************************************************
*     Get a System Physical FAP Port mapped to a FAP port in
*     the local device. The mapping is unique - single System
*     Physical Port is mapped to a single local port per
*     specified device. This configuration effects: 1.
*     Resolving destination FAP Id and OFP Id 2. Per-port
*     pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_local_to_sys_phys_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LOCAL_TO_SYS_PHYS_PORT_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_local_to_sys_phys_port_map_get_unsafe(
    unit,
    fap_ndx,
    fap_local_port_ndx,
    sys_phys_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_local_to_sys_phys_port_map_get()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int 
soc_petra_port_to_interface_map_set_dispatch(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  int                 unmap
    )
{
    int rv, res;
    SOC_PETRA_PORT2IF_MAPPING_INFO  info;

    info.channel_id = 0;
    info.if_id = 0; 

    res = soc_petra_port_to_interface_map_set(unit, 0, 0, SOC_PETRA_PORT_DIRECTION_BOTH, &info);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}
uint32
  soc_petra_port_to_interface_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_TO_INTERFACE_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_port_to_interface_map_verify(
    unit,
    port_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_to_interface_map_set_unsafe(
    unit,
    port_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_to_interface_map_set()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
  soc_petra_port_to_interface_map_get_dispatch(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_ID *if_id,
    SOC_SAND_OUT uint32                 *channel_id
  )
{
    SOC_PETRA_PORT2IF_MAPPING_INFO
        info_incoming,
        info_outgoing;
    int
        rv,
        res;

     if (if_id == NULL) {
         return SOC_E_PARAM;
     }

    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&info_incoming);
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&info_outgoing);

    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            port_ndx,
            &info_incoming,
            &info_outgoing
          );
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    *if_id = info_outgoing.if_id;

    if (channel_id) {
        *channel_id = info_outgoing.channel_id;
    }

    return SOC_E_NONE;
}

uint32
  soc_petra_port_to_interface_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_TO_INTERFACE_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_incoming);
  SOC_SAND_CHECK_NULL_INPUT(info_outgoing);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_to_interface_map_get_unsafe(
          unit,
          port_ndx,
          info_incoming,
          info_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_to_interface_map_get()",0,0);
}

/********************************************************************* 
*  Get a local port and answer wether this port is a lag member. 
*  if is_in_lag = TRUE  get the lag ID to lag_id  
*********************************************************************/ 
uint32  
  soc_petra_ports_is_port_lag_member( 
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  int                core_id, 
    SOC_SAND_IN  uint32                port_id, 
    SOC_SAND_OUT uint8                 *is_in_lag, 
    SOC_SAND_OUT uint32                *lag_id) 
{
  uint32 
    res = SOC_SAND_OK; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
   
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
  
  SOC_SAND_CHECK_NULL_INPUT(is_in_lag); 
  SOC_SAND_CHECK_NULL_INPUT(lag_id); 
  
   SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
  
  res = soc_petra_ports_is_port_lag_member_unsafe( 
          unit, 
          port_id, 
          is_in_lag, 
          lag_id 
        ); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore); 
  
exit_semaphore: 
  SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_is_port_lag_member()",0,0); 
}


/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ports_lag_verify(
          unit,
          direction_ndx,
          lag_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_set_unsafe(
          unit,
          direction_ndx,
          lag_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_set()",0,0);
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_incoming);
  SOC_SAND_CHECK_NULL_INPUT(info_outgoing);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          info_incoming,
          info_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_get()",0,0);
}


uint32
  soc_petra_ports_lag_sys_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  uint32                 sys_port
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_ADD);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_ports_lag_add_verify(
          unit,
          direction_ndx,
          lag_ndx,
          sys_port
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_sys_port_add_unsafe(
          unit,
          direction_ndx,
          lag_ndx,
          sys_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_add()",0,0);
}

/*********************************************************************
*     Add a system port as a member in LAG.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_member_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_MEMBER_ADD);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_member);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_member_add_unsafe(
          unit,
          direction_ndx,
          lag_ndx,
          lag_member,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_member_add()",0,0);
}

/*********************************************************************
*     remove a system port from a LAG.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_sys_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER        *lag_member
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_REMOVE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_member);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_PORT_NOF_DIRECTIONS-1,
    SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, SOC_PETRA_MAX_LAG_GROUP_ID,
    SOC_PETRA_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_sys_port_remove_unsafe(
          unit,
          direction_ndx,
          lag_ndx,
          lag_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_remove()",0,0);
}


uint32
  soc_petra_ports_lag_sys_port_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_port,
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *port_lag_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_lag_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_sys_port_info_get_unsafe(
          unit,
          sys_port,
          port_lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_info_get()",0,0);
}
/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_order_preserve_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_ports_lag_order_preserve_verify(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_order_preserve_set_unsafe(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_order_preserve_set()",0,0);
}

/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_order_preserve_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_order_preserving);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_lag_order_preserve_get_unsafe(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_order_preserve_get()",0,0);
}


/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_port_header_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_HEADER_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_header_type_verify(
    unit,
    port_ndx,
    direction_ndx,
    header_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_port_header_type_set_unsafe(
    unit,
    port_ndx,
    direction_ndx,
    header_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_header_type_set()",0,0);
}

/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_port_header_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_HEADER_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(header_type_incoming);
  SOC_SAND_CHECK_NULL_INPUT(header_type_outgoing);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_ndx,
          header_type_incoming,
          header_type_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_header_type_get()",0,0);
}

/*********************************************************************
*     Configure inbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_INBOUND_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_mirror_inbound_verify(
    unit,
    ifp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_mirror_inbound_set_unsafe(
    unit,
    ifp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_inbound_set()",0,0);
}

/*********************************************************************
*     Configure inbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_INBOUND_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_mirror_inbound_get_unsafe(
    unit,
    ifp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_inbound_get()",0,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_OUTBOUND_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_mirror_outbound_verify(
    unit,
    ofp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_mirror_outbound_set_unsafe(
    unit,
    ofp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_outbound_set()",0,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_OUTBOUND_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_mirror_outbound_get_unsafe(
    unit,
    ofp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_outbound_get()",0,0);
}

/*********************************************************************
*     Configure the snooping function.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_snoop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_SNOOP_INFO     *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SNOOP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ports_snoop_verify(
    unit,
    snoop_cmd_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_snoop_set_unsafe(
    unit,
    snoop_cmd_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_snoop_set()",0,0);
}

/*********************************************************************
*     Configure the snooping function.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_snoop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_SNOOP_INFO     *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SNOOP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_snoop_get_unsafe(
    unit,
    snoop_cmd_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_snoop_get()",0,0);
}

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_itmh_extension_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  uint8                 ext_en
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_ITMH_EXTENSION_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_itmh_extension_verify(
          unit,
          port_ndx,
          ext_en
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_itmh_extension_set_unsafe(
          unit,
          port_ndx,
          ext_en
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_itmh_extension_set()",0,0);
}

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_itmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT uint8                 *ext_en
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_ITMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ext_en);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_itmh_extension_get_unsafe(
    unit,
    port_ndx,
    ext_en
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_itmh_extension_get()",0,0);
}

/*********************************************************************
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ISP_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SHAPING_HEADER_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(1,exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_shaping_header_verify(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_shaping_header_set_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_shaping_header_set()",0,0);
}

/*********************************************************************
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_ISP_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SHAPING_HEADER_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(1,exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_shaping_header_get_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_shaping_header_get()",0,0);
}

/*********************************************************************
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH          *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FORWARDING_HEADER_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_forwarding_header_verify(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_forwarding_header_set_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_forwarding_header_set()",0,0);
}

/*********************************************************************
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FORWARDING_HEADER_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_forwarding_header_get_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_forwarding_header_get()",0,0);
}

/*********************************************************************
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_stag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_STAG_FIELDS   *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_STAG_FIELDS_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(1,exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_stag_verify(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_stag_set_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_stag_set()",0,0);
}

/*********************************************************************
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_stag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_STAG_FIELDS   *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_STAG_FIELDS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(1,exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_stag_get_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_stag_get()",0,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_ftmh_extension_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FTMH_EXTENSION_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_ftmh_extension_set_unsafe(
    unit,
    ext_option
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_ftmh_extension_set()",0,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_ftmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FTMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ext_option);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_ftmh_extension_get_unsafe(
    unit,
    ext_option
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_ftmh_extension_get()",0,0);
}

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_otmh_extension_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_OTMH_EXTENSION_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_otmh_extension_verify(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ports_otmh_extension_set_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_otmh_extension_set()",0,0);
}

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_otmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_OTMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ports_otmh_extension_get_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_otmh_extension_get()",0,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_port_egr_hdr_credit_discount_type_verify(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_egr_hdr_credit_discount_type_set_unsafe(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_type_set()",0,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_egr_hdr_credit_discount_type_get_unsafe(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_type_get()",0,0);
}


/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_egr_hdr_credit_discount_select_verify(
    unit,
    port_ndx,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_port_egr_hdr_credit_discount_select_set_unsafe(
    unit,
    port_ndx,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_select_set()",0,0);
}

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_select_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_egr_hdr_credit_discount_select_get_unsafe(
    unit,
    port_ndx,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_select_get()",0,0);
}

/* $Id: petra_api_ports.c,v 1.12 Broadcom SDK $
 *  Get the MAL index or equivalent of the MAL
 *  to which the given port is mapped at the egress
 */
uint32
  soc_petra_port_ofp_mal_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID ofp_ndx,
    SOC_SAND_OUT uint32         *mal_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_OFP_MAL_GET);
  SOC_SAND_CHECK_NULL_INPUT(mal_ndx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_port_ofp_mal_get_unsafe(
          unit,
          ofp_ndx,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_ofp_mal_get()",0,0);
}

/*
 *  Clear functions
 */

void
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT2IF_MAPPING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_LAG_MEMBER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

 SOC_TMC_PORTS_LAG_INFO_PETRAB_clear(info); 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_OVERRIDE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_OVERRIDE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_OVERRIDE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_INBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_INBOUND_MIRROR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_OUTBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_SNOOP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ITMH_BASE_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_BASE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ITMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ITMH_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ISP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ISP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ISP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
soc_petra_PETRA_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_LAG_SYS_PORT_INFO_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_PORTS_STAG_FIELDS_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_STAG_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_STAG_FIELDS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_PORT_DIRECTION_to_string(
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION enum_val
  )
{
  return SOC_TMC_PORT_DIRECTION_to_string(enum_val);
}

const char*
  soc_petra_PETRA_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE enum_val
  )
{
  return SOC_TMC_PORT_HEADER_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_PORTS_SNOOP_SIZE_to_string(
    SOC_SAND_IN  SOC_PETRA_PORTS_SNOOP_SIZE enum_val
  )
{
  return SOC_TMC_PORTS_SNOOP_SIZE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF enum_val
  )
{
  return SOC_TMC_PORTS_FTMH_EXT_OUTLIF_to_string(enum_val);
}

const char*
  soc_petra_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE enum_val
  )
{
  return SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_to_string(enum_val);
}

void
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT2IF_MAPPING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_LAG_MEMBER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_LAG_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_LAG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_OVERRIDE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_OVERRIDE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_OVERRIDE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_INBOUND_MIRROR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_INBOUND_MIRROR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_OUTBOUND_MIRROR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORT_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_SNOOP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ITMH_BASE_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_BASE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_PORT_LAG_SYS_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT_LAG_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_LAG_SYS_PORT_INFO_print(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ITMH_EXT_SRC_PORT_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ITMH_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_ISP_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_ISP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ISP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_STAG_FIELDS_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_STAG_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_STAG_FIELDS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

