/* $Id: petra_multicast_egress.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_multicast_egress.h
*
* MODULE PREFIX:  soc_petra_mult_ing
*
* FILE DESCRIPTION: Egress Multicast refers to the act of replication at
*                   the egress.
*                   This file contains standard unsafe get/set and
*                   verify functions for configuration purposes.
*                   Also, this file contain dynamic configuration of ingress
*                   multicast groups (open, close, update and more).
*                   The file also contains an interface to the 'mult_link_list'
*                   service functions for implementing file together
*                   with additional service functions.
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PETRA_MULTICAST_EGRESS_INCLUDED__
/* { */
#define __SOC_PETRA_MULTICAST_EGRESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/Petra/petra_api_multicast_egress.h>
#include <soc/dpp/Petra/petra_multicast_linked_list.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY (3)
#define SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY (3)
#define SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY (2)
#define SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY (2)

/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   *  Format A table entry.
   */
  SOC_PETRA_MULT_EG_FORMAT_A = 0,
  /*
   *  Format B table entry.
   *  Note: Not valid for Soc_petra-A.
   */
  SOC_PETRA_MULT_EG_FORMAT_B = 1,
  /*
   *  Format C table entry.
   *  Note: Not valid for Soc_petra-A.
   */
  SOC_PETRA_MULT_EG_FORMAT_C = 2,
  /*
   *  Number of types in SOC_PETRA_MULT_EG_FORMAT_TYPE
   */
  SOC_PETRA_MULT_EG_NOF_FORMATS = 3
}SOC_PETRA_MULT_EG_FORMAT_TYPE;

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
* NAME:
*     soc_petra_multicast_egress_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_multicast_egress_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_range_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info -
*     info - The information to be configured.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_range_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info -
*     info - The information to be configured.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_range_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info -
*     info - The information to be configured.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );


/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_open_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API enables the egress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list/bitmap. The user only specifies the
*     multicast-id and copies. The user may open a multicast
*     group with no members, In this case the Multicast Id is
*     in use. All inner link-list nodes and bitmap are
*     allocated and handled by the driver.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Egress-Multicast ID to set. Range: 0 - (16k-1). SOC_SAND_IN
*     SOC_PETRA_MULT_EG_GROUP *group - Point to structure that
*     include the multicast group members. SOC_SAND_OUT uint8
*     *insufficient_memory - Indicates if the action succeeded
*     or failed. TRUE - the group was opened as requested.
*     FALSE - the group was not opened as requested due to
*     insufficient space.
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*  SOC_SAND_IN  uint32                  mc_group_size -
*  SOC_SAND_OUT uint8                 *insufficient_memory -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_update_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Egress-Multicast ID to set. Range: 0 - (16k-1). SOC_SAND_IN
*     SOC_PETRA_MULT_EG_GROUP *group - Point to structure that
*     include the multicast group members. SOC_SAND_OUT uint8*
*     insufficient_memory - Indicates if the action succeeded
*     or failed. TRUE - the group was opened as requested.
*     FALSE - the group was not opened as requested due to
*     insufficient space.
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*  SOC_SAND_IN  uint32                  mc_group_size -
*  SOC_SAND_OUT uint8                 *insufficient_memory -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_update_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY      *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_close_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Egress-Multicast ID to set. Range: 0 - (16k-1).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_close_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_add_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1).
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry -
*     Data-port and number of copies per port. SOC_SAND_OUT
*  SOC_SAND_OUT uint8                 *insufficient_memory -
*     Indicates if the action succeeded or failed. TRUE - the
*     group was opens as requested. FALSE - the group was
*     opens as requested due to insufficient space.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_port_add_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_remove_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Removes a port member of the egress multicast.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry -
*     Data-port to remove from multicast group. Range: 0 - 63
*     (0 - 30 while working with 4 queues per egress port
*     mode).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_size_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Returns the size of the multicast group with the
*     specified multicast id.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1). SOC_SAND_OUT
*  SOC_SAND_OUT uint32                 *mc_group_size -
*     mc_group_size - The size of the multicast group.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_size_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT uint32                  *mc_group_size
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Gets the multicast group with the specified multicast
*     id.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1). SOC_SAND_IN uint32 mc_group_size - The size of
*     the array to be filled in by this function, this
*     parameter is in order to limit the size of the array.
*     SOC_SAND_OUT SOC_PETRA_MULT_EG_GROUP *group - The multicast
*     group, the multicast_id being the entry to this
*     multicast group. SOC_SAND_IN uint32 *exact_mc_group_size
*     - The actual size of the array after retrieving the
*     multicast group. SOC_SAND_OUT uint8 *is_open - This
*     parameter indicates whether the group is open and can be
*     retrieved or not.
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*  SOC_SAND_IN  uint32                  mc_group_size -
*  SOC_SAND_OUT uint32                  *exact_mc_group_size -
*  SOC_SAND_OUT uint8                 *is_open -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_OUT uint32                  *exact_mc_group_size,
    SOC_SAND_OUT uint8                 *is_open
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_all_groups_close_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Closes all opened egress multicast groups.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_all_groups_close_unsafe(
    SOC_SAND_IN  int                 unit
  );

uint32
  soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );
uint32
 soc_petra_mult_eg_group_open_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_open_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API enables the egress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list/bitmap. The user only specifies the
*     multicast-id and copies. All inner link-list nodes and
*     bitmap are allocated and handled by the driver.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Egress-Multicast ID to set. Range: 0 - (16k-1). SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group -
*     Point to structure that include the multicast group
*     members.
*  SOC_SAND_OUT uint8 *insufficient_memory -
*     Indicates if the action succeeded or failed. TRUE - the
*     group was opened as requested. FALSE - the group was not
*     opened as requested due to insufficient space.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_update_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group -
*     Point to structure that include the multicast group
*     members.
*  SOC_SAND_OUT uint8* insufficient_memory -
*     Indicates if the action succeeded or failed. TRUE - the
*     group was opened as requested. FALSE - the group was not
*     opened as requested due to insufficient space.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_update_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_close_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_close_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_add_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1). SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID        port -
*     Data-port to include in the group.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_port_add_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID        port
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_remove_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Removes a port member of the egress multicast.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID        port -
*     Data-port to exclude from the group.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID        port
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Gets the multicast group with the specified multicast
*     id.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1). SOC_SAND_OUT
*  SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group -
*     The multicast group, the multicast_id being the entry to
*     this multicast group.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_all_groups_close_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Closes all opened egress multicast groups in range of
*     vlan membership.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_all_groups_close_unsafe(
    SOC_SAND_IN  int                 unit
  );



/*********************************************************************
* NAME:
*     soc_petra_mult_ing_multicast_group_entry_to_tbl
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Enters the multicast group part into the specified entry.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID           in_group_index -
*       this is the index within the mc_group from which
*       to start inputting information
*     SOC_SAND_IN  uint32                offset -
*       the table entry to receive the information.
*     SOC_SAND_IN SOC_PETRA_MULT_ENTRY         *mc_group -
*       a pointer to the multicast group to be inputed.
*     SOC_SAND_IN uint32                 *mc_group_size -
*       the mc-group size (0-3)
*     SOC_SAND_IN uint16                 *next_entry -
*       the next entry in the table from the multicast linked-list.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 in_group_index,
    SOC_SAND_IN  uint32                  offset,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint16                  next_entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_entry_content_get
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx -
*       the entry in the table of which we wish to get the content of.
*     SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group -
*       returns a pointer to the multicast group constructed
*       from what was in the table entry.
*     SOC_SAND_OUT uint32                *mc_group_size -
*       returns a mc-group size (0-3)
*     SOC_SAND_OUT uint16                 *next_entry -
*       returns a pointer to the next entry in the table from the
*       multicast linked-list.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_entry_content_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_fill_in_last_entry
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function adds the content of the entry into the last entry of
*     mc-group link list, if the entry has an unoccupied space.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish erase.
*     SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry -
*       The entry's details.
*     SOC_SAND_OUT uint8              *success -
*       TRUE - if there was space for the entry and it was added.
*       FALSE - otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_OUT uint8              *success
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_nof_occupied_elements_in_tbl_entry
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function returns the number of elements that are used per entry.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish to investigate.
*     SOC_SAND_OUT uint32               *size -
*       The return value - the number of elements that are occupied
*       per entry.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_nof_occupied_elements_in_tbl_entry(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint16                tbl_entry_id,
    SOC_SAND_OUT uint32               *size
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_MULTICAST_EGRESS_INCLUDED__*/
#endif
