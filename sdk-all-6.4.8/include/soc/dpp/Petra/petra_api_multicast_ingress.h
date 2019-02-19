/* $Id: petra_api_multicast_ingress.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_api_multicast_ingress.h
*
* MODULE PREFIX:  soc_petra_mult_ing
*
* FILE DESCRIPTION: Ingress Multicast refers to the act of replication at
*                   the ingress.
*                   This file holds the API functions and Structures
*                   which implement the Soc_petra egress multicast.
*                   The file contains the standard get/set, clear and print
*                   for configuration. This file also contains dynamic
*                   configuration of multicast groups (open. close, update
*                   and more).
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PETRA_API_MULTICAST_INGRESS_INCLUDED__
/* { */
#define __SOC_PETRA_API_MULTICAST_INGRESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_multicast_ingress.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

typedef SOC_TMC_MULT_ING_TR_CLS_MAP                            SOC_PETRA_MULT_ING_TR_CLS_MAP;
typedef SOC_TMC_MULT_ING_ENTRY                                 SOC_PETRA_MULT_ING_ENTRY;

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
*     soc_petra_mult_ing_traffic_class_map_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_TR_CLS_MAP *map -
*     This array holds the mapping of one traffic class to
*     another. The index of the array is the class, which is
*     mapped to the class in the value of the array indicated
*     by the certain index.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_ing_traffic_class_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_TR_CLS_MAP *map
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_traffic_class_map_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_MULT_ING_TR_CLS_MAP *map -
*     This array holds the mapping of one traffic class to
*     another. The index of the array is the class, which is
*     mapped to the class in the value of the array indicated
*     by the certain index.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_ing_traffic_class_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_TR_CLS_MAP *map
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_group_open
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API enables the ingress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list. The user specifies the multicast-id,
*     which is as the entry point to the link-list. All inner
*     link-list nodes are allocated and handled by the driver.
*     This function also configures the table which indicates
*     per Multicast ID whether to perform ingress replication
*     or not, if ingress replication is not chosen, fabric or
*     egress multicast will be performed on the packet.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Ingress-Multicast ID to set. Range: 0 - (16k-1). SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*     mc_group_size - The number of members in the array (in
*     the multicast group).
*  SOC_SAND_IN  uint32                  mc_group_size -
*     The size of the group.
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
  soc_petra_mult_ing_group_open(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID        multicast_id_ndx,    /* group mcid */
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY *mc_group,           /* group replications to set */
    SOC_SAND_IN  uint32                   mc_group_size,       /* number of group replications (size of mc_group) */
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err             /* return possible errors that the caller may want to ignore: insufficient memory or duplicate replications */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_group_update
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API updates ingress-multicast-replication for the
*     specific multicast-id, and re-allocate if needed the
*     devices' link-list. The user only specifies the
*     multicast-id, which is as the entry point to the
*     link-list. The driver handles all inner link-list nodes
*     for allocation/free/moves. This function also configures
*     the table which indicates per Multicast ID whether to
*     perform ingress replication or not, if ingress
*     replication is not chosen, fabric or egress multicast
*     will be performed on the packet.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Ingress-Multicast ID to set. Range: 0 - (16k-1). SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*     mc_group_size - The number of members in the array (in
*     the multicast group).
*  SOC_SAND_IN  uint32                  mc_group_size -
*     The size of the group.
*  SOC_SAND_OUT SOC_TMC_ERROR           *out_err -
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
  soc_petra_mult_ing_group_update(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID        multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY *mc_group,        /* group replications to set */
    SOC_SAND_IN  uint32                   mc_group_size,    /* number of group replications (size of mc_group) */
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err          /* return possible errors that the caller may want to ignore: insufficient memory or duplicate replications */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_group_close
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This API closes ingress-multicast-replication for the
*     specific multicast-id, and frees the device's link-list.
*     The user only specifies the multicast-id, which is as
*     the entry point to the link-list. All inner link-list
*     nodes are freed and handled by the driver. This function
*     also configures the table which indicates per Multicast
*     ID whether to perform ingress replication or not, if
*     ingress replication is not chosen, fabric or egress
*     multicast will be performed on the packet.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     Egress-Multicast ID to set. Range: 0 - (16k-1).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_close(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_destination_add
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Add an entry, including a destination and an cud, to
*     a multicast group indicated by a multicast-id. An error
*     is carried out in case of an invalid entry.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1).
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *entry -
*     Destination queue/system-port-id and cud to be added
*     to the group.
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
  soc_petra_mult_ing_destination_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID        multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY *entry,
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err /* return possible errors that the caller may want to ignore: insufficient memory or replication exists */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_destination_remove
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Removes an entry, including a destination and an cud,
*     from a multicast group indicated by a multicast-id. An
*     error is carried out in case of an invalid entry.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx -
*     14 bit multicast-ID (as given from the NP). Range: 0 -
*     (16k-1). SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *entry -
*     Destination queue/system-port-id and cud to be
*     removed from the group.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_ing_destination_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID        multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY *entry,           /* replication to remove */
    SOC_SAND_OUT SOC_TMC_ERROR            *out_err /* return possible errors that the caller may want to ignore: replication does not exist */
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_group_size_get
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
  soc_petra_mult_ing_group_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT uint32                 *mc_group_size
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_group_get
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
*  SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY      *mc_group -
*     mc_group_size - The size of the array to be filled in by
*     this function, this parameter is in order to limit the
*     size of the array. SOC_SAND_OUT uint32
*     *exact_mc_group_size - The actual size of the array
*     after retrieving the multicast group. SOC_SAND_OUT uint8
*     *is_open - This parameter indicates whether the group is
*     open and can be retrieved or not.
*  SOC_SAND_IN  uint32                 mc_group_size -
*  SOC_SAND_OUT uint32                 *exact_mc_group_size -
*  SOC_SAND_OUT uint8                 *is_open -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint32                 mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY      *mc_group,
    SOC_SAND_OUT uint32                  *exact_mc_group_size,
    SOC_SAND_OUT uint8                 *is_open
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_ing_all_groups_close
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Closes all opened ingress multicast groups.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_ing_all_groups_close(
    SOC_SAND_IN  int                 unit
  );

void
  soc_petra_PETRA_MULT_ING_TR_CLS_MAP_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_ING_TR_CLS_MAP *info
  );

void
  soc_petra_PETRA_MULT_ING_ENTRY_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1


void
  soc_petra_PETRA_MULT_ING_TR_CLS_MAP_print(
    SOC_SAND_IN SOC_PETRA_MULT_ING_TR_CLS_MAP *info
  );



void
  soc_petra_PETRA_MULT_ING_ENTRY_print(
    SOC_SAND_IN SOC_PETRA_MULT_ING_ENTRY *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_MULTICAST_INGRESS_INCLUDED__*/
#endif
