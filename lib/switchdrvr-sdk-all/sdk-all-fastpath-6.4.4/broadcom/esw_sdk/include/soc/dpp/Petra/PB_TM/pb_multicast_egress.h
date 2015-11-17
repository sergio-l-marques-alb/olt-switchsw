/* $Id: pb_multicast_egress.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/soc_pb_tm/include/soc_pb_multicast_egress.h
*
* MODULE PREFIX:  soc_petra_mult_eg
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


#ifndef __SOC_PB_MULTICAST_EGRESS_INCLUDED__
/* { */
#define __SOC_PB_MULTICAST_EGRESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/Petra/petra_multicast_linked_list.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_multicast_egress.h>
#include <soc/dpp/Petra/petra_multicast_egress.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_MULT_EG_FORMAT_SELECT_B                          0x7E
#define SOC_PB_MULT_EG_FORMAT_SELECT_C                          0x7D

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
*     soc_pb_mult_eg_multicast_group_entry_to_tbl
* TYPE:
*   PROC
* DATE:
*   July 04 2010
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
*     SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE  format_type -
*       the entry format.
*     SOC_SAND_IN uint16                 *next_entry -
*       the next entry in the table from the multicast linked-list.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mult_eg_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 in_group_index,
    SOC_SAND_IN  uint32                  offset,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE  format_type,
    SOC_SAND_IN  uint16                  next_entry
  );

/*********************************************************************
* NAME:
*     soc_pb_mult_eg_entry_content_get
* TYPE:
*   PROC
* DATE:
*   July 04 2010
* FUNCTION:
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx -
*       the entry in the table of which we wish to get the content of.
*     SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY_TYPE  format_type -
*       the entry format.
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
  soc_pb_mult_eg_entry_content_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE format_type,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  );

/*********************************************************************
* NAME:
*     soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry
* TYPE:
*   PROC
* DATE:
*   Jul 04 2010
* FUNCTION:
*     This function returns the number of elements that are used per entry.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish to investigate.
*     SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE format_type -
*       Entry format type.
*     SOC_SAND_OUT uint32               *size -
*       The return value - the number of elements that are occupied
*       per entry.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE format_type,
    SOC_SAND_OUT uint32                *size
  );

/*********************************************************************
* NAME:
*     soc_pb_mult_eg_fill_in_group
* TYPE:
*   PROC
* DATE:
*   Jul 04 2008
* FUNCTION:
*     This function fill the content of the mc group from table
*     into the mc group given, limited by mc group size.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish to get.
*     SOC_SAND_IN  uint32                  mc_group_size -
*       The size of the multicast group.
*     SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group -
*       The multicast group which we wish to fill.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mult_eg_fill_in_group(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group
  );

/*********************************************************************
* NAME:
*     soc_pb_mult_eg_fill_in_last_entry
* TYPE:
*   PROC
* DATE:
*   Jul 04 2008
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
  soc_pb_mult_eg_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_OUT uint8              *success
  );

/*********************************************************************
* NAME:
*     soc_pb_mult_eg_tbl_entry_format_get
* TYPE:
*   PROC
* DATE:
*   Jul 04 2010
* FUNCTION:
*     This function returns entry format of the entry given.
* INPUT:
*     SOC_SAND_IN  int                unit -
*     SOC_SAND_IN  uint16                 tbl_entry_id -
*       The entry in the table of which we wish to investigate.
*     SOC_SAND_OUT SOC_PETRA_MULT_EG_FORMAT_TYPE *entry_format -
*       The return value - the entry format.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mult_eg_tbl_entry_format_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_FORMAT_TYPE *entry_format
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PB_MULTICAST_EGRESS_INCLUDED__*/
#endif
