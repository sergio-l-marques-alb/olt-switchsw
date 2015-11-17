/* $Id: petra_api_multicast_egress.c,v 1.10 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_multicast_egress.c
*
* MODULE PREFIX:  soc_petra_mult_eg
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_multicast_egress.h>
#include <soc/dpp/Petra/petra_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
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
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
 *     are to be found according to a bitmap (as opposed to a
 *     Link List). This range MC-IDs is used for
 *     Vlan-Membership Multicast purposes, in this type of
 *     multicast a packet can be replicated once to each egress
 *     port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_mult_eg_vlan_membership_group_range_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_group_range_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_range_set()",0,0);
}

/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
 *     are to be found according to a bitmap (as opposed to a
 *     Link List). This range MC-IDs is used for
 *     Vlan-Membership Multicast purposes, in this type of
 *     multicast a packet can be replicated once to each egress
 *     port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_range_get()",0,0);
}


/*********************************************************************
*     This API enables the egress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list/bitmap. The user only specifies the
*     multicast-id and copies. The user may open a multicast
*     group with no members, In this case the Multicast Id is
*     in use. All inner link-list nodes and bitmap are
*     allocated and handled by the driver.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_group_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_OPEN);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  res = soc_petra_mult_eg_group_open_verify(
          unit,
          multicast_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_group_open_unsafe(
          unit,
          multicast_id_ndx,
          mc_group,
          mc_group_size,
          insufficient_memory
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_open()",0,0);
}

/*********************************************************************
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_group_update(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_UPDATE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx,
    SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
    10,
    exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_group_update_unsafe(
    unit,
    multicast_id_ndx,
    mc_group,
    mc_group_size,
    insufficient_memory
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_update()",0,0);
}

/*********************************************************************
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_group_close(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_CLOSE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx,
    SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
    10,
    exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_group_close_unsafe(
    unit,
    multicast_id_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_close()",0,0);
}

/*********************************************************************
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_port_add(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID       multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY *entry,
    SOC_SAND_OUT SOC_TMC_ERROR           *out_err /* return possible errors that the caller may want to ignore : insufficient memory or replication exists */
  )
{
  uint32  res;
  uint8 insufficient_memory;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_ADD);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(out_err);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx,
    SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
    10,
    exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_port_add_unsafe(
    unit,
    multicast_id_ndx,
    entry,
    &insufficient_memory
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
  *out_err = insufficient_memory ? _SHR_E_FULL : _SHR_E_NONE;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_add()",0,0);
}

/*********************************************************************
*     Removes a port member of the egress multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_port_remove(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID       multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY *entry,
    SOC_SAND_OUT SOC_TMC_ERROR           *out_err /* return possible errors that the caller may want to ignore : replication does not exist or insufficient memory */
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_REMOVE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(out_err);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx,
    SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
    10,
    exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_port_remove_unsafe(
    unit,
    multicast_id_ndx,
    entry
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
  *out_err = _SHR_E_NONE;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_remove()",0,0);
}

/*********************************************************************
*     Returns the size of the multicast group with the
*     specified multicast id.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_group_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT uint32                  *mc_group_size
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_SIZE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx,
    SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
    10,
    exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_group_size_get_unsafe(
    unit,
    multicast_id_ndx,
    mc_group_size
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_size_get()",0,0);
}

/*********************************************************************
*     Gets the multicast group with the specified multicast
*     id.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_group_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_OUT uint32                  *exact_mc_group_size,
    SOC_SAND_OUT uint8                 *is_open
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(exact_mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(is_open);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx,
    SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
    10,
    exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_group_get_unsafe(
    unit,
    multicast_id_ndx,
    mc_group_size,
    mc_group,
    exact_mc_group_size,
    is_open
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_get()",0,0);
}

/*********************************************************************
*     Closes all opened egress multicast groups.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_all_groups_close(
    SOC_SAND_IN  int                 unit
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_ALL_GROUPS_CLOSE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_all_groups_close_unsafe(
    unit
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_all_groups_close()",0,0);
}

/*********************************************************************
*     This API enables the egress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list/bitmap. The user only specifies the
*     multicast-id and copies. All inner link-list nodes and
*     bitmap are allocated and handled by the driver.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(group);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
          unit,
          multicast_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


  res = soc_petra_mult_eg_vlan_membership_group_open_unsafe(
    unit,
    multicast_id_ndx,
    group
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_open()",0,0);
}

/*********************************************************************
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_update(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(group);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_group_update_unsafe(
    unit,
    multicast_id_ndx,
    group
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_update()",0,0);
}

/*********************************************************************
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_close(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
          unit,
          multicast_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mult_eg_vlan_membership_group_close_unsafe(
    unit,
    multicast_id_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_close()",0,0);
}

/*********************************************************************
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID      multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port,
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err /* return possible errors that the caller may want to ignore */
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_ADD);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(out_err);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
          unit,
          multicast_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mult_eg_vlan_membership_port_add_unsafe(
    unit,
    multicast_id_ndx,
    port
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
  *out_err = _SHR_E_NONE;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_add()",0,0);
}

/*********************************************************************
*     Removes a port member of the egress multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID      multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port,
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err /* return possible errors that the caller may want to ignore : port is not replicated to */
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_REMOVE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(out_err);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
          unit,
          multicast_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mult_eg_vlan_membership_port_remove_unsafe(
    unit,
    multicast_id_ndx,
    port
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
  *out_err = _SHR_E_NONE;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_remove()",0,0);
}

/*********************************************************************
*     Gets the multicast group with the specified multicast
*     id.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(group);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
          unit,
          multicast_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mult_eg_vlan_membership_group_get_unsafe(
    unit,
    multicast_id_ndx,
    group
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_get()",0,0);
}

/*********************************************************************
*     Closes all opened egress multicast groups in range of
*     vlan membership.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_all_groups_close(
    SOC_SAND_IN  int                 unit
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_eg_vlan_membership_all_groups_close_unsafe(
    unit
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_all_groups_close()",0,0);
}

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_EG_ENTRY_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_EG_ENTRY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY_TYPE enum_val
  )
{
  return SOC_TMC_MULT_EG_ENTRY_TYPE_to_string(enum_val);
}

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_EG_ENTRY_print(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_EG_ENTRY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

