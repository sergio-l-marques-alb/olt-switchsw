/* $Id: petra_api_multicast_egress.h,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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


#ifndef __SOC_PETRA_API_MULTICAST_EGRESS_INCLUDED__
/* { */
#define __SOC_PETRA_API_MULTICAST_EGRESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     In order to configure the egress replication bitmap we
 *     need a bitmap of at least 80 bits, therefore- 3 longs   */
#define  SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP (SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_PETRA)


/* $Id: petra_api_multicast_egress.h,v 1.7 Broadcom SDK $
 *  Maximal number of MC Id-s used for VLAN membership MC
 */
#define SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX (SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX)

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

#define SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP                       SOC_TMC_MULT_EG_ENTRY_TYPE_OFP
#define SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR                  SOC_TMC_MULT_EG_ENTRY_TYPE_VLAN_PTR
typedef SOC_TMC_MULT_EG_ENTRY_TYPE                             SOC_PETRA_MULT_EG_ENTRY_TYPE;

typedef SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE            SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE;
typedef SOC_TMC_MULT_EG_ENTRY                                  SOC_PETRA_MULT_EG_ENTRY;
typedef SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP                  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP;

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
*     soc_petra_mult_eg_vlan_membership_group_range_set
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
  soc_petra_mult_eg_vlan_membership_group_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_range_get
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
  soc_petra_mult_eg_vlan_membership_group_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_open
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
*     Egress-Multicast ID to set. Range: 0 - (16k-1).
*  SOC_SAND_IN  SOC_PETRA_MULT_EG_GROUP       *mc_group -
*     Point to structure that include the multicast group members.
*  SOC_SAND_IN  uint32                  mc_group_size -
*     The groups size.
*  SOC_SAND_OUT uint8                 *insufficient_memory -
*     Indicates if the action succeeded or failed.
      TRUE - the group was opened as requested.
*     FALSE - the group was not opened as requested due to
*     insufficient space.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   It is very important to check the 'insufficient_memory' variable
*   when returning from procedure in, if the value is TRUE then the
*   procedure was not done.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_update
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
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*     Point to structure that include the multicast group members.
*  SOC_SAND_IN  uint32                  mc_group_size -
*     The size of the group.
*  SOC_SAND_OUT uint8                 *insufficient_memory -
*     Indicates if the action succeeded or failed.
*     TRUE - the group was opened as requested.
*     FALSE - the group was not opened as requested due to
*     insufficient space.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   It is very important to check the 'insufficient_memory' variable
*   when returning from procedure in, if the value is TRUE then the
*   procedure was not done.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_update(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY      *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_close
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
  soc_petra_mult_eg_group_close(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_add
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
*     Data-port and number of copies per port.
*  SOC_SAND_OUT SOC_TMC_ERROR                 *out_err -
*     Indicates if the action succeeded or failed.
*     _SHR_E_NONE - the group was updated as requested.
*     _SHR_E_FULL - the group was not updated as requested due to insufficient space.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   It is very important to check the 'out_err' variable
*   when returning from procedure in, if the value is TRUE then the
*   procedure was not done.
*********************************************************************/
uint32
  soc_petra_mult_eg_port_add(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID       multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY *entry,
    SOC_SAND_OUT SOC_TMC_ERROR           *out_err /* return possible errors that the caller may want to ignore : insufficient memory or replication exists */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_remove
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
  soc_petra_mult_eg_port_remove(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID       multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY *entry,
    SOC_SAND_OUT SOC_TMC_ERROR           *out_err /* return possible errors that the caller may want to ignore : replication does not exist or insufficient memory */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_size_get
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
  soc_petra_mult_eg_group_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT uint32                  *mc_group_size
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_group_get
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
*     (16k-1).
*  SOC_SAND_IN uint32                  mc_group_size -
*     The size of the array to be filled in by this function, this
*     parameter is in order to limit the size of the array.
*  SOC_SAND_OUT SOC_PETRA_MULT_EG_GROUP    *group -
*     The multicast group, the multicast_id being the entry to this
*     multicast group.
*  SOC_SAND_IN uint32               *exact_mc_group_size -
*     The actual size of the array after retrieving the
*     multicast group.
*  SOC_SAND_OUT uint8              *is_open -
*     This parameter indicates whether the group is open and can be
*     retrieved or not.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT  SOC_PETRA_MULT_EG_ENTRY      *mc_group,
    SOC_SAND_OUT uint32                  *exact_mc_group_size,
    SOC_SAND_OUT uint8                 *is_open
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_all_groups_close
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
  soc_petra_mult_eg_all_groups_close(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_open
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
*  SOC_SAND_OUT SOC_TMC_ERROR *out_err -
*     Indicates if the action succeeded or failed.
*     _SHR_E_NONE - the group was updated as requested.
*     _SHR_E_FULL - the group was not updated as requested due to insufficient space.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_update
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
*     Point to structure that include the multicast group members.
*  SOC_SAND_OUT SOC_TMC_ERROR *out_err -
*     Indicates if the action succeeded or failed.
*     _SHR_E_NONE - the group was updated as requested.
*     _SHR_E_FULL - the group was not updated as requested due to insufficient space.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_update(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_close
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
  soc_petra_mult_eg_vlan_membership_group_close(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_add
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
  soc_petra_mult_eg_vlan_membership_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID      multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port,
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err /* return possible errors that the caller may want to ignore */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_port_remove
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
  soc_petra_mult_eg_vlan_membership_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID      multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port,
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err /* return possible errors that the caller may want to ignore : port is not replicated to */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_group_get
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
  soc_petra_mult_eg_vlan_membership_group_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_eg_vlan_membership_all_groups_close
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
  soc_petra_mult_eg_vlan_membership_all_groups_close(
    SOC_SAND_IN  int                 unit
  );

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

void
  soc_petra_PETRA_MULT_EG_ENTRY_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY *info
  );

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY_TYPE enum_val
  );

void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(
    SOC_SAND_IN SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );



void
  soc_petra_PETRA_MULT_EG_ENTRY_print(
    SOC_SAND_IN SOC_PETRA_MULT_EG_ENTRY *info
  );



void
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(
    SOC_SAND_IN SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_MULTICAST_EGRESS_INCLUDED__*/
#endif
