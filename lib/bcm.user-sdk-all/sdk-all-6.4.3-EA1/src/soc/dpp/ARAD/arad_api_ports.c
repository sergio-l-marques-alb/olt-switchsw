#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_api_ports.c,v 1.23 Broadcom SDK $
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


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_ports.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_general.h>

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
  arad_sys_phys_to_local_port_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_sys_phys_to_local_port_map_verify(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_sys_phys_to_local_port_map_set_unsafe(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_phys_to_local_port_map_set()",0,0);
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
  arad_sys_phys_to_local_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_id);
  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_port_id);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_sys_phys_to_local_port_map_get_unsafe(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_phys_to_local_port_map_get()",0,0);
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
  arad_local_to_sys_phys_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_local_to_sys_phys_port_map_get_unsafe(
    unit,
    fap_ndx,
    fap_local_port_ndx,
    sys_phys_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_local_to_sys_phys_port_map_get()",0,0);
}

/**********************************************************************
 * Function:
 *      arad_port_control_pcs_set
 * Purpose:
 *      Set link PCS
 * Parameters:
 *      unit    -    (IN)    unit number.
 *      link_ndx    -     (IN)    Number of link from the ARAD device toward the fabric element.
 *      pcs           -    (IN)    PCS for link
 * Returns:
 *      BCM_E_NONE     No Error  
 *      BCM_E_UNAVAIL  Feature unavailable  
 *      BCM_E_PORT     Invalid Port number specified  
 *      BCM_E_XXX      Error occurred  
 **********************************************************************/
uint32
  arad_port_control_pcs_set(
    SOC_SAND_IN    int      unit,
    SOC_SAND_IN    uint32      link_ndx,
    SOC_SAND_IN    ARAD_PORT_PCS pcs
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_CONTROL_PCS_SET);
  /* SOC_SAND_CHECK_DRIVER_AND_DEVICE; */


  res = arad_port_control_pcs_set_verify(
    unit,
    link_ndx,
    pcs
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* SOC_SAND_TAKE_DEVICE_SEMAPHORE; */

  res = arad_port_control_pcs_set_unsafe(
    unit,
    link_ndx,
    pcs
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  /* SOC_SAND_GIVE_DEVICE_SEMAPHORE; */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_control_pcs_set()",0,0);

}

/**********************************************************************
 * Function:
 *      arad_port_control_pcs_get_unsafe
 * Purpose:
 *      Get link PCS
 * Parameters:
 *      unit    -    (IN)    unit number.
 *      link_ndx    -     (IN)    Number of link from the ARAD device toward the fabric element.
 *      pcs           -    (OUT)    PCS of link
 * Returns:
 *      BCM_E_NONE     No Error  
 *      BCM_E_UNAVAIL  Feature unavailable  
 *      BCM_E_PORT     Invalid Port number specified  
 *      BCM_E_XXX      Error occurred  
 **********************************************************************/

uint32
  arad_port_control_pcs_get(
    SOC_SAND_IN     int       unit,
    SOC_SAND_IN     uint32       link_ndx,
    SOC_SAND_OUT ARAD_PORT_PCS *pcs
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_CONTROL_PCS_GET);
  /* SOC_SAND_CHECK_DRIVER_AND_DEVICE; */


  res = arad_port_control_pcs_get_verify(
    unit,
    link_ndx,
    pcs
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* SOC_SAND_TAKE_DEVICE_SEMAPHORE; */

  res = arad_port_control_pcs_get_unsafe(
    unit,
    link_ndx,
    pcs
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  /* SOC_SAND_GIVE_DEVICE_SEMAPHORE; */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_control_pcs_get()",0,0);

}
/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_to_dynamic_interface_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8                    is_init
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_DYNAMIC_INTERFACE_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_to_interface_map_verify(
    unit,
    port_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_to_dynamic_interface_map_set_unsafe(
    unit,
    port_ndx,
    direction_ndx,
    info,
    is_init
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_dynamic_interface_map_set()",0,0);
}

uint32
  arad_port_to_interface_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_INTERFACE_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_to_interface_map_verify(
    unit,
    port_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_to_interface_map_set_unsafe(
    unit,
    port_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_interface_map_set()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_rx_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  int32                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_RX_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;



   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_rx_enable_set_unsafe(
          unit,
          port_ndx,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_rx_enable_set()",0,0);
}
  
/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_rx_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT int32                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_RX_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(enable);


   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_rx_enable_get_unsafe(
          unit,
          port_ndx,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_rx_enable_get()",0,0);
}


/********************************************************************* 
*  Get a local port and answer wether this port is a lag member. 
*  if is_in_lag = TRUE  get the lag ID to lag_id  
*********************************************************************/ 
uint32  
     arad_ports_is_port_lag_member( 
         SOC_SAND_IN  int                                 unit, 
         SOC_SAND_IN  uint32                                 port_id, 
        SOC_SAND_OUT uint8                                 *is_in_lag, 
        SOC_SAND_OUT uint32                                 *lag_id) 
{   
  uint32 
    res = SOC_SAND_OK; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
   
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
  
  SOC_SAND_CHECK_NULL_INPUT(is_in_lag); 
  SOC_SAND_CHECK_NULL_INPUT(lag_id); 
  
   SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
  
  res = arad_ports_is_port_lag_member_unsafe( 
          unit, 
          port_id, 
          is_in_lag, 
          lag_id 
        ); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore); 
  
exit_semaphore: 
  SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_is_port_lag_member()",0,0); 
} 
  


/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_ports_lag_verify(
          unit,
          direction_ndx,
          lag_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_set_unsafe(
          unit,
          direction_ndx,
          lag_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_set()",0,0);
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT ARAD_PORTS_LAG_INFO      *info_incoming,
    SOC_SAND_OUT ARAD_PORTS_LAG_INFO      *info_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_incoming);
  SOC_SAND_CHECK_NULL_INPUT(info_outgoing);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          info_incoming,
          info_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_get()",0,0);
}

/*********************************************************************
*     Add a system port as a member in LAG.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_lag_member_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_MEMBER_ADD);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_member);
  SOC_SAND_CHECK_NULL_INPUT(success);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_member_add_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_member_add()",0,0);
}

/*********************************************************************
*     remove a system port from a LAG.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_lag_sys_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER  *lag_member
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_SYS_PORT_REMOVE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_member);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, ARAD_PORT_NOF_DIRECTIONS-1,
    ARAD_PORT_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, ARAD_MAX_LAG_GROUP_ID,
    ARAD_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_sys_port_remove_unsafe(
          unit,
          direction_ndx,
          lag_ndx,
          lag_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_sys_port_remove()",0,0);
}


uint32
  arad_ports_lag_sys_port_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_port,
    SOC_SAND_OUT ARAD_PORT_LAG_SYS_PORT_INFO *port_lag_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_SYS_PORT_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_lag_info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_sys_port_info_get_unsafe(
          unit,
          sys_port,
          port_lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_sys_port_info_get()",0,0);
}
/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_lag_order_preserve_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_ports_lag_order_preserve_verify(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_order_preserve_set_unsafe(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_set()",0,0);
}

/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_lag_order_preserve_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_order_preserving);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_order_preserve_get_unsafe(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_get()",0,0);
}

uint32 arad_ports_lag_lb_key_range_set(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  ARAD_PORTS_LAG_INFO      *info)
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_lb_key_range_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_lb_key_range_set()",0,0);
}

/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_header_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_HEADER_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_header_type_verify(
    unit,
    port_ndx,
    direction_ndx,
    header_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_header_type_set_unsafe(
    unit,
    port_ndx,
    direction_ndx,
    header_type
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_header_type_set()",0,0);
}

/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_header_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE    *header_type_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_HEADER_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(header_type_incoming);
  SOC_SAND_CHECK_NULL_INPUT(header_type_outgoing);
  SOC_SAND_ERR_IF_ABOVE_NOF(port_ndx, ARAD_NOF_LOCAL_PORTS, ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_header_type_get_unsafe(
          unit,
          port_ndx,
          header_type_incoming,
          header_type_outgoing
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_header_type_get()",0,0);
}
/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_ftmh_extension_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_ftmh_extension_set_unsafe(
    unit,
    ext_option
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
  
  goto exit;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_set()",0,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_ftmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ext_option);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_ftmh_extension_get_unsafe(
    unit,
    ext_option
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_get()",0,0);
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
  arad_ports_otmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_OTMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_otmh_extension_get_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_otmh_extension_get()",0,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_port_egr_hdr_credit_discount_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_egr_hdr_credit_discount_type_verify(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_type_set_unsafe(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_set()",0,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_port_egr_hdr_credit_discount_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_type_get_unsafe(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_get()",0,0);
}

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using arad_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_port_egr_hdr_credit_discount_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_egr_hdr_credit_discount_select_verify(
    unit,
    port_ndx,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_select_set_unsafe(
    unit,
    port_ndx,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_set()",0,0);
}

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using arad_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_port_egr_hdr_credit_discount_select_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_select_get_unsafe(
    unit,
    port_ndx,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_get()",0,0);
}

/*********************************************************************
*     Set Stacking information on the relevant port
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32 arad_port_stacking_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_port_stacking_info_verify(unit, local_port_ndx, is_stacking, peer_tmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_port_stacking_info_set_unsafe(unit, local_port_ndx, is_stacking, peer_tmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_set()",0,0);
}

uint32 arad_port_stacking_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_OUT  uint32                              *is_stacking,
    SOC_SAND_OUT  uint32                              *peer_tmd)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(is_stacking);
    SOC_SAND_CHECK_NULL_INPUT(peer_tmd);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_port_stacking_info_get_unsafe(unit, local_port_ndx, is_stacking, peer_tmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_get()",0,0);
}

/*********************************************************************
*     Set Stacking route history bitmap on the relevant port
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 arad_port_stacking_route_history_bitmap_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN  uint32                              bitmap)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

     SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_port_stacking_route_history_bitmap_set_unsafe(unit, local_port_ndx, tm_port_profile_stack, bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_get()",0,0);
}


/*********************************************************************
*     Configure the Port profile for ports of type TM and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_pp_port_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_port_pp_port_set_verify(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_pp_port_set_unsafe(
          unit,
          pp_port_ndx,
          0,
          info,
          success
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_pp_port_set()", pp_port_ndx, 0);
}

/*********************************************************************
*     Get the Port profile settings.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_pp_port_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_pp_port_get_verify(
          unit,
          pp_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_pp_port_get_unsafe(
          unit,
          pp_port_ndx,
          0,
          info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_pp_port_get()", pp_port_ndx, 0);
}

/*********************************************************************
*     Set port to pp port map
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_port_to_pp_port_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,  exit, arad_port_to_pp_port_map_set_verify(unit, port, ARAD_PORT_DIRECTION_BOTH));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20,  exit, arad_port_to_pp_port_map_set_unsafe(unit,port,ARAD_PORT_DIRECTION_BOTH));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_to_pp_port_map_set()", port, 0);
}

/*********************************************************************
*     Set per port swap configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_swap_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           port_ndx,
    SOC_SAND_IN  ARAD_PORTS_SWAP_INFO       *info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_SET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_swap_set_verify(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_swap_set_unsafe(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_swap_set()", port_ndx, info->enable);
}

/*********************************************************************
*     Get per port swap configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_swap_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           port_ndx,
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO       *info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_GET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_swap_get_verify(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_swap_get_unsafe(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_swap_get()", port_ndx, info->enable);
}


/*********************************************************************
*     Map PON port and its channel_ID to ports of type IN_PP
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_pon_tunnel_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_pon_tunnel_info_set_verify(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_pon_tunnel_info_set_unsafe(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_pon_tunnel_info_set()", port_ndx, tunnel);
}

/*********************************************************************
*     Map PON port and its channel_ID to ports of type IN_PP
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_pon_tunnel_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_pon_tunnel_info_get_verify(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_pon_tunnel_info_get_unsafe(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_pon_tunnel_info_get()", port_ndx, tunnel);
}

/*
 *  Set lb-key min value
 */
uint32
  arad_port_direct_lb_key_min_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 min_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_set_verify(
          unit, 
          local_port,
          min_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_min_set_unsafe(
          unit, 
          local_port,
          min_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_min_set()",0,0);
}

uint32
  arad_port_direct_lb_key_max_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 max_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_set_verify(
          unit, 
          local_port,
          max_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_max_set_unsafe(
          unit, 
          local_port,
          max_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_max_set()",0,0);
}

uint32
  arad_port_direct_lb_key_min_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint32* min_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(min_lb_key);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_get_verify(
          unit, 
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_min_get_unsafe(
          unit, 
          local_port,
          min_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_min_get()",0,0);
}

uint32
  arad_port_direct_lb_key_max_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint32* max_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(max_lb_key);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_get_verify(
          unit, 
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_max_get_unsafe(
          unit, 
          local_port,
          max_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_max_get()",0,0);
}

#ifdef BCM_88660_A0

uint32
   arad_port_synchronize_lb_key_tables_at_egress(
   SOC_SAND_IN int unit
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_synchronize_lb_key_tables_at_egress_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_synchronize_lb_key_tables_at_egress()",0,0);
}


uint32
  arad_port_switch_lb_key_tables(
     SOC_SAND_IN int unit
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res =  arad_port_switch_lb_key_tables_unsafe(
          unit 
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_synchronize_lb_key_tables_at_egress()",0,0);
}
#endif /* BCM_88660_A0 */

/*********************************************************************
*     Port configration for L2 header get
*     
*********************************************************************/
uint32
  arad_port_encap_config_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_L2_ENCAP_INFO       *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_port_encap_config_verify(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_port_encap_config_get_unsafe(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_encap_config_get()", port_ndx, 0);
}

/*********************************************************************
*     Port configration for L2 header set
*     
*********************************************************************/

uint32
  arad_port_encap_config_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_L2_ENCAP_INFO       *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_port_encap_config_verify(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_port_encap_config_set_unsafe(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_encap_config_get()", port_ndx, 0);
}

/*********************************************************************
*      Set TM port properties for programmable blocks (VT, TT, FLP).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_ports_programs_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_programs_info_set_verify(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_programs_info_set_unsafe(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_set()", port_ndx,0);
}

uint32
  arad_ports_programs_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  )
{
   uint32
      res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
   SOC_SAND_CHECK_NULL_INPUT(info);

   res = arad_ports_programs_info_get_verify(
           unit,
           port_ndx,
            info
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
   res = arad_ports_programs_info_get_unsafe(
          unit,
          port_ndx,
          info
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_get()", port_ndx, 0);
}

/*********************************************************************
* NAME:
*     arad_ports_application_mapping_info_set
* TYPE:
*   PROC
* FUNCTION:
*     Allows different mappings for (packet, TM-PTC-port)
*     to PP port and might also apply opposite.
*     For XGS MAC extender, it allows packet mapping
*     (HG.Port,HG.Modid) to PP port.
*     Might be used in the future for other applications that have
*     not typical Port mappings.
* INPUT:
*  SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  port -
*       Port #
*  SOC_SAND_IN  uint32                  info -
*       Application mapping information.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32 
  arad_ports_application_mapping_info_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    )
{
  uint32
      res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
   SOC_SAND_CHECK_NULL_INPUT(info);

   res = arad_ports_application_mapping_info_set_verify(
           unit,
           port_ndx,
            info
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
   res = arad_ports_application_mapping_info_set_unsafe(
          unit,
          port_ndx,
          info
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_application_mapping_info_set()", port_ndx, 0);
}

/*********************************************************************
* NAME:
*     arad_ports_application_mapping_info_get
* TYPE:
*   PROC
* FUNCTION:
*     Reterive Configuration of the ports mappings according
*     to application.
* INPUT:
*  SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  port -
*       Port #
*  SOC_SAND_IN  uint32                  info -
*       Application mapping information.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32 
  arad_ports_application_mapping_info_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_INOUT ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    )
{
   uint32
      res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
   SOC_SAND_CHECK_NULL_INPUT(info);

   res = arad_ports_application_mapping_info_get_verify(
           unit,
           port_ndx,
            info
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
   res = arad_ports_application_mapping_info_get_unsafe(
          unit,
          port_ndx,
          info
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_get()", port_ndx, 0);
}


/*
 *  Clear functions
 */

void
  ARAD_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_COUNTER_INFO_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT ARAD_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT2IF_MAPPING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  arad_ARAD_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT ARAD_PORTS_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_LAG_MEMBER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_LAG_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_LAG_INFO_ARAD_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_ITMH_clear(
    SOC_SAND_OUT ARAD_PORTS_ITMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
arad_ARAD_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_LAG_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_LAG_SYS_PORT_INFO_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_SWAP_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_SWAP_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  arad_ARAD_PORTS_PON_TUNNEL_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_PON_TUNNEL_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_PROGRAMS_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_PROGRAMS_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1
void
  ARAD_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  arad_ARAD_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE enum_val
  )
{
  return SOC_TMC_PORT_HEADER_TYPE_to_string(enum_val);
}
const char*
  arad_ARAD_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF enum_val
  )
{
  return SOC_TMC_PORTS_FTMH_EXT_OUTLIF_to_string(enum_val);
}
void
  arad_ARAD_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT2IF_MAPPING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_SWAP_INFO_print(
    SOC_SAND_IN  ARAD_PORTS_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_SWAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#endif /* ARAD_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */
