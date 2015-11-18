/* $Id: soc_petra_multicast_linked_list.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_MULTICAST_LINKED_LIST_INCLUDED__
/* { */
#define __SOC_PETRA_MULTICAST_LINKED_LIST_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/Petra/petra_api_multicast_ingress.h>
#include <soc/dpp/Petra/petra_api_multicast_egress.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

  /* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION         0x7FFF
#define SOC_PETRA_MULT_LL_INGRESS_MC_ID_START_ENTRY                 0
#define SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY                  0x4000
#define SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION      0xFFFF
#define SOC_PETRA_MULT_UNOCCUPIED_CUD_INDICATION                    0xFF80
#define SOC_PETRA_MULT_EGR_DATA_DFLT                                0x1F
#define SOC_PETRA_MULT_ING_NOF_ENTRIES                              (16*1024)
#define SOC_PETRA_MULT_EGR_NOF_ENTRIES                              (16*1024)

/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PETRA_MULT_USE_SMART_ADD_ENTRY TRUE
/* $Id: soc_petra_multicast_linked_list.h,v 1.5 Broadcom SDK $
 * If TRUE, when the user add entry, the driver validate it ain't already exist.
 * Make the Add commands much slower when there are very big multicast groups.
 */
#define SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS TRUE

#define SOC_PETRA_MULT_ID_FROM_LL_TBL_ID(ll_tbl_id)           \
  (ll_tbl_id) &(~(SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY))
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */



typedef union
{
  SOC_PETRA_MULT_ING_ENTRY ing_entry;

  SOC_PETRA_MULT_EG_ENTRY eg_entry;

}SOC_PETRA_MULT_ENTRY;



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
*     soc_petra_mc_init
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
  soc_petra_mc_init(
    SOC_SAND_IN  int                 unit
  );

uint16
  soc_petra_mc_null_id(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_ingr_not_egr
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_nof_vlan_bitmaps_in_group_get
* TYPE:
*   PROC
* DATE:
*   July 04 2010
* FUNCTION:
*     This function returns nof vlan bitmap exists
*     in group given.
* INPUT:
*     SOC_SAND_IN  int                 unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group -
*       the mc group.
*     SOC_SAND_IN  uint16                  mc_group_size -
*       the size of the mc-group.
*     SOC_SAND_IN  uint8                 is_ingress -
*       indicates ingress or egress group.
* RETURNS:
*   nof vlan bitmaps in multicast group given.
*********************************************************************/
uint32
  soc_petra_mult_nof_vlan_bitmaps_in_group_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MULT_EG_ENTRY *mc_group,
    SOC_SAND_IN uint32 mc_group_size,
    SOC_SAND_IN uint8 is_ingress
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_is_multicast_id_group_exist
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function returns TRUE if the group size fits in the
*     available table entries and FALSE otherwise.
* INPUT:
*     SOC_SAND_IN  int                 unit -
*     SOC_SAND_IN  uint16                  mc_group_size -
*       the size of the mc-group.
* RETURNS:
*   TRUE if size is larger than remaining space, FALSE otherwise.
*********************************************************************/
uint8
  soc_petra_mult_is_insufficient_memory(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_IN  uint32  		           nof_vlan_bitmaps
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_is_multicast_id_group_exist
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function returns TRUE if the multicast-id
*     group already exists and FALSE otherwise.
* INPUT:
*     SOC_SAND_IN  int                 unit -
*     SOC_SAND_IN  uint16                  tbl_entry_id -
*       the table entry that is investigated.
* RETURNS:
*   TRUE if table-entry is a mc-id, FALSE otherwise.
*********************************************************************/
uint8
  soc_petra_mult_is_multicast_id_group_exist(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                   tbl_entry_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_is_multicast_id_occupied
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function returns TRUE if the multicast-id is occupied by
*     another entry and FALSE otherwise.
* INPUT:
*     SOC_SAND_IN  int                 unit -
*     SOC_SAND_IN  uint16                  tbl_entry_id -
*       the table entry that is investigated.
* RETURNS:
*   TRUE if table-entry occupied, FALSE otherwise.
*********************************************************************/
uint8
  soc_petra_mult_is_multicast_id_occupied(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                   tbl_entry_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_id_as_unoccupied_head_set
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function sets the head of the unoccupied list to be a new
*     unoccupied entry, as given.
* INPUT:
*     SOC_SAND_IN  int                 unit -
*     SOC_SAND_IN  uint16                  tbl_entry_id -
*       the table entry that should be the new empty list head.
* RETURNS:
*   void.
*********************************************************************/
void
  soc_petra_mult_multicast_id_as_unoccupied_head_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id
  );


/*********************************************************************
* NAME:
*     soc_petra_mult_group_open_unsafe_joint
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     A joint function, for ingress and egress open unsafe functions.
*     This method opens a multicast group.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID           multicast_id_ndx -
*       this is the multicast-id of the group to be opened.
*     SOC_SAND_IN  SOC_PETRA_MULT_ENTRY        *mc_group -
*       a pointer to the multicast group to be inputed (ing/eg).
*     SOC_SAND_IN uint32                 *mc_group_size -
*       the mc-group size.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_OUT uint8                 *insufficient_memory -
*       this is a parameter that returns of there is insufficient memory
*       and therefore the group was not opened.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_group_open_unsafe_joint(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_group_input_first_entry_db_set
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function sets the sw-db for for handling the input of a
*     new first entry of a multicast group.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  uint16                tbl_entry_id -
*       This is the mc-id of the group (functions as first table entry).
* RETURNS:
*   void.
*********************************************************************/
uint32
  soc_petra_mult_multicast_group_input_first_entry_db_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_group_input_middle_entry_db_set
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function sets the sw-db for for handling the input of any
*     entry of a multicast group, besides the first one.
* INPUT:
*    SOC_SAND_IN  int               unit -
*    SOC_SAND_IN  uint16                prev_entry -
*      gets the previous linked-list entry.
*    SOC_SAND_IN  uint8               is_ingress -
*      TRUE if this is an ingress mc-group, FALSE otherwise.
*    SOC_SAND_OUT uint16                *entry -
*      returns the pointer to the table entry id, in which to
*      input the entry.
* RETURNS:
*   void.
*********************************************************************/
uint32
  soc_petra_mult_multicast_group_input_middle_entry_db_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  prev_entry,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint16                  *entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_group_input
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Inputs the multicast group elements to the table
*     in order to open a group.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  uint16                  tbl_entry_id -
*       the multicast id of the group (functions as first table entry).
*     SOC_SAND_IN SOC_PETRA_MULT_ENTRY         *mc_group -
*       a pointer to the multicast group to be inputed.
*     SOC_SAND_IN uint32                 *mc_group_size -
*       the mc-group size.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_multicast_group_input(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_group_entry_to_tbl
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Enters the multicast group part into the specified table entry.
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
*       the mc-group size.
*     SOC_SAND_IN  uint8                 is_ingress,
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_IN uint16                 *next_entry -
*       the next entry in the table from the multicast linked-list.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 in_group_index,
    SOC_SAND_IN  uint32                  offset,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_IN  uint16                  next_entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_id_relocation
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Relocates an occupied entry in the multicast group linked list to a
*     different free entry, and connects the new occupied entry to the list.
* INPUT:
*     SOC_SAND_IN  int                 unit -
*     SOC_SAND_IN  uint16                  tbl_entry_id -
*       the entry id that should be relocated.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_multicast_id_relocation(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id
  );


/*********************************************************************
* NAME:
*     soc_petra_mult_entry_content_get
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
*     SOC_SAND_IN  uint8                is_ingress -
*       TRUE if this is an ingress table entry and FALSE otherwise.
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
  soc_petra_mult_entry_content_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_IN  uint8                is_ingress,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_group_update_unsafe_joint
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     A joint function, for ingress and egress update unsafe functions.
*     This method updates the elements of an existing multicast group.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID           multicast_id_ndx -
*       this is the multicast-id of the group to be opened.
*     SOC_SAND_IN  SOC_PETRA_MULT_ENTRY        *mc_group -
*       a pointer to the multicast group to be inputed (ing/eg).
*     SOC_SAND_IN uint32                 *mc_group_size -
*       the mc-group size.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_OUT uint8                 *insufficient_memory -
*       this is a parameter that returns of there is insufficient memory
*       and therefore the group was not opened.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_group_update_unsafe_joint(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint8                 *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_link_list_ptr_get
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Returns the id of the next entry in the table
*     (pointed at by the reference to the linked-list).
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 table_entry_id -
*       The entry in the table of which we wish to get the following entry of.
*     SOC_SAND_OUT uint16                 *link_list_ptr -
*       The id of the next entry in the table.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
 soc_petra_mult_link_list_ptr_get(
   SOC_SAND_IN  int           unit,
   SOC_SAND_IN  uint16            tbl_entry_id,
   SOC_SAND_OUT uint16            *link_list_ptr
 );

/*********************************************************************
* NAME:
*     soc_petra_mult_erase_multicast_group
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Erases the linked list of entries starting from the given
*     tbl_entry_id.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 table_entry_id -
*       The entry of the table from which to start erasing entries.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_erase_multicast_group(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint16            tbl_entry_id,
    SOC_SAND_IN uint8           is_ingress
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_erase_one_entry
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     Erases the an entry in the list specified by tbl_entry_id.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 table_entry_id -
*       The entry of the table from which to start erasing entries.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_erase_one_entry(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint16            tbl_entry_id,
    SOC_SAND_IN uint8           is_ingress
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_group_close_unsafe_joint
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     A joint function, for ingress and egress close unsafe functions.
*     This method closes a multicast group.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID           multicast_id_ndx -
*       this is the multicast-id of the group to be closed.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_group_close_unsafe_joint(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN SOC_PETRA_MULT_ID           multicast_id_ndx,
    SOC_SAND_IN uint8               is_ingress
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_destination_add_unsafe_joint
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     A joint function, for ingress and egress add unsafe functions.
*     This method adds an entry to a multicast group.
* INPUT:
*     SOC_SAND_IN  int               unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID           multicast_id_ndx -
*       this is the multicast-id of the group that the entry
*       should be added to.
*     SOC_SAND_IN  SOC_PETRA_MULT_ENTRY        *entry -
*       a pointer to the multicast group entry to be added to the group.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_OUT uint8                 *insufficient_memory -
*       this is a parameter that returns of there is insufficient memory
*       and therefore the element was not added.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_destination_add_unsafe_joint(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY         *entry,
    SOC_SAND_IN uint8                is_ingress,
    SOC_SAND_OUT uint8                *insufficient_memory
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fill_in_last_entry
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
*     SOC_SAND_IN uint8               is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_OUT uint8              *success -
*       TRUE - if there was space for the entry and it was added.
*       FALSE - otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_IN uint8               is_ingress,
    SOC_SAND_OUT uint8               *success
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_last_entry_in_list_get
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function finds the table entry id of the last entry
*     of the link-list multicast group.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish erase.
*     SOC_SAND_OUT uint16                 *ptr_to_last_entry -
*       the last mc-group link-list entry id.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_last_entry_in_list_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_OUT uint16                 *ptr_to_last_entry
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_add_entry_in_end_of_link_list
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function adds a new entry to the end of the link list in
*     order to add another element to the mc-group.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The last entry of the mc-group linked list.
*     SOC_SAND_IN  SOC_PETRA_MULT_ENTRY         *entry -
*       the last mc-group link-list entry id.
*     SOC_SAND_IN  uint8                is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_add_entry_in_end_of_link_list(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY         *entry,
    SOC_SAND_IN  uint8                is_ingress
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_group_size_get_unsafe_joint
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function returns the size of the mc-group .
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*       the mc-id of the group we want the size of.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_OUT uint32                  *mc_group_size -
*       the returned value - the size of the mc-group.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_group_size_get_unsafe_joint(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint32                  *mc_group_size
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_nof_occupied_elements_in_tbl_entry
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function returns the number of elements that are used per
*     given entry.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish to investigate.
*     SOC_SAND_IN  uint8               is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
*     SOC_SAND_OUT uint32               *size -
*       The return value - the number of elements that are occupied
*       per entry.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_nof_occupied_elements_in_tbl_entry(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint16                tbl_entry_id,
    SOC_SAND_IN  uint8               is_ingress,
    SOC_SAND_OUT uint32               *size
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_multicast_ingress_replication_hw_table_set
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function sets ingress replication hardware table,
*     indicating whether an entry is multicast-id or not
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx -
*       the entry in the table of which we wish to set.
*     SOC_SAND_IN  uint8                 is_assert -
*       If TRUE - assert entry (it is a multicast-id).
*       If FALSE - reset entry (delete multicast-id).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_multicast_ingress_replication_hw_table_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint8                 is_assert
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_rplct_tbl_entry_unoccupied_set
* TYPE:
*   PROC
* DATE:
*   Feb 10 2008
* FUNCTION:
*     This function sets the specified entry to be marked as unoccupied.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish erase.
*     SOC_SAND_IN  uint8                 is_ingress -
*       TRUE if this is an ingress mc-group, FALSE otherwise.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_unoccupied_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN uint8               is_ingress
  );

/*********************************************************************
* Initialize MC replication database
* The initialization accesses the replication table as if it was an
* Ingress replication, for all entries (including Egress MC)
  ********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int               unit
  );

/*********************************************************************
 *  Check if the replication DB table entry is marked as occupied.
 *  Note: same entry value marks unoccupied entry for Ingress/Egress MC
 *********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_is_occupied(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint16   tbl_entry_id,
    SOC_SAND_OUT uint8  *is_occupied
  );

/*********************************************************************
* This function sets the specified entry be Occupied but Empty.
*********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_occupied_but_empty_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN uint8               is_ingress
  );

/*********************************************************************
 *  Check if the replication DB table entry is marked as occupied.
 *  Note: same entry value marks occupied-but-empty entry for Ingress/Egress MC
 *********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_is_occupied_but_empty(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint16   tbl_entry_id,
    SOC_SAND_OUT uint8  *is_occupied_but_empty
  );

/*********************************************************************
 *  Check if the replication DB table entry is marked as empty.
 *  Note: same entry value marks unoccupied/occupied-but-empty entry
 *  for Ingress/Egress MC
 *********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_is_empty(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint16   tbl_entry_id,
    SOC_SAND_OUT uint8  *is_empty
  );

/*********************************************************************
*  Check if next pointer is null.
*********************************************************************/
uint8
  soc_petra_mc_is_next_null(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint16  next_ptr
  );

/* } */

uint32
  soc_petra_mult_progress_index_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID  multicast_id_ndx,
    SOC_SAND_IN  uint16       tbl_entry_id,
    SOC_SAND_IN  uint8      is_ingress,
    SOC_SAND_OUT uint32      *progress_index_by
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_MULTICAST_INGRESS_INCLUDED__*/
#endif
