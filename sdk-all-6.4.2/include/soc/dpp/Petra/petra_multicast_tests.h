/* $Id: petra_multicast_tests.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_multicast_tests.h
*
* MODULE PREFIX:  soc_petra_mult_tests
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/



#ifndef __SOC_PETRA_MULTICAST_TESTS_INCLUDED__
/* { */
#define __SOC_PETRA_MULTICAST_TESTS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>

#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_multicast_egress.h>
#include <soc/dpp/Petra/petra_api_multicast_ingress.h>
#include <soc/dpp/Petra/petra_multicast_egress.h>
#include <soc/dpp/Petra/petra_multicast_ingress.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_sw_db.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

  typedef enum
  {
    /*
    *  multicast groups compare result is equal: Value 0
    */
    SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL=0,
    /*
    *  multicast groups compare result is different sizes: Value 1
    */
    SOC_PETRA_MULT_TESTS_GROUP_DIFF_SIZES=1,
    /*
    *  multicast groups compare result is different in port ids: Value 2
    */
    SOC_PETRA_MULT_TESTS_GROUP_DIFF_PORT_IDS=2,
    /*
    *  multicast groups compare result is different in copy-unique-data: Value 3
    */
    SOC_PETRA_MULT_TESTS_GROUP_DIFF_CUD=3,
    /*
    * multicast groups compare result is different in type: Value 4
    */
    SOC_PETRA_MULT_TESTS_GROUP_DIFF_TYPE = 4,
    /*
    * multicast groups compare result is different in vlan mc ids: Value 5
    */
    SOC_PETRA_MULT_TESTS_GROUP_DIFF_VLAN_MC_IDS = 5,
    /*
    *  Must be the last value
    */
    SOC_PETRA_MULT_TESTS_NOF_GROUP_DIFFS
  }SOC_PETRA_MULT_TESTS_GROUP_DIFF;


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
#if SOC_PETRA_DEBUG_IS_LVL1

/***********************************************************************
* Compare two groups,
* and return enum describing if groups are
* equal/ different sizes/ different in port ids/ different in copy-unique-data.
***********************************************************************/
SOC_SAND_RET
  soc_petra_mult_ing_groups_compare(
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY         *group_1,
    SOC_SAND_IN  uint32                     size_group_1,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY         *group_2,
    SOC_SAND_IN  uint32                     size_group_2,
    SOC_SAND_OUT SOC_PETRA_MULT_TESTS_GROUP_DIFF  *diff
  );

/***********************************************************************
* Compare two groups,
* and return enum describing if groups are
* equal/ different sizes/ different in port ids/ different in copy-unique-data.
***********************************************************************/
SOC_SAND_RET
  soc_petra_mult_eg_groups_compare(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY          *group_1,
    SOC_SAND_IN  uint32                     size_group_1,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY          *group_2,
    SOC_SAND_IN  uint32                     size_group_2,
    SOC_SAND_OUT SOC_PETRA_MULT_TESTS_GROUP_DIFF  *diff
  );

void
  soc_petra_multicast_ing_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_ING_ENTRY            *group,
    SOC_SAND_IN uint32                    group_size
  );

void
  soc_petra_multicast_eg_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_EG_ENTRY            *group,
    SOC_SAND_IN uint32                    group_size
  );

void
  soc_petra_multicast_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY            *group,
    SOC_SAND_IN uint32                    group_size,
    SOC_SAND_IN uint8                   is_ingress
  );


void
  soc_petra_mult_tests_create_ing_multicast_group(
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY      *mc_group
  );


void
  soc_petra_mult_tests_create_eg_multicast_group(
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group
  );

void
  soc_petra_mult_tests_create_eg_vlan_membership_multicast_group(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP   *bitmap_mc_group
  );

uint32
  soc_petra_mult_tests_get_ing_mc_group_from_hw(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  mc_group_id,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY      *mc_group,
    SOC_SAND_OUT uint32                 *mc_group_size_exact,
    SOC_SAND_OUT uint16                  *list_of_next_entries
  );

uint32
  soc_petra_mult_tests_get_eg_mc_group_from_hw(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  mc_group_id,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_OUT uint32                 *mc_group_size_exact,
    SOC_SAND_OUT uint16                  *list_of_next_entries
  );



/*****************************************************
*NAME
*  soc_petra_mult_open_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Test data base consistency for the Open mechanism.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_open_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_open_2_intersecting_groups_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Open a MC-GROUP with 2 entries (MC-ID = 0).
*  Then, write another MC-GROUP with 2 entries (MC-ID = 1).
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_open_2_intersecting_groups_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_update_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Open a MC-GROUP with 2 entries (MC-ID = 0).
*  Update (MC-ID = 0) with MC-GROUP with 1 entry.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_update_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_open_add_to_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Open a MC-GROUP with X entries (MC-ID = 0).
*  Test the ADD and REMOVE functions..
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_open_add_to_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_open_remove_from_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Open a MC-GROUP with X entries (MC-ID = 0).
*  Test the ADD and REMOVE functions..
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_open_remove_from_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_open_close_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Open 2 MC-GROUP with X entries (MC-ID = 0).
*  Close MC-GROUP (MC-ID = 0) and MC-GROUP (MC-ID = 1).
*  Check that DB is empty.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_open_close_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_open_relocation_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
*  Check the reloaction Operation is performed correctly.
*  Relocation is required when a MC-Group is to be opened with
*  a MC-ID that is occupied in the multicast table by another MC-Group
*  linked-list.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_open_relocation_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_occupied_but_empty_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Fab-2008
*FUNCTION:
* Verification that Data Base skips over an occupied but empty entry for allocation
* 1.  Allocate a MC-Group with no entries.
* 2.  Try and Allocate in the same space (with a new MC-Group).
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_occupied_but_empty_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_bitmap_allocation_TEST
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification of Bitmap Allocation - tests that after
* a configuration of a bitmap range the program react correctly,
* meaning that a linked-list mc-group cannot be opened in that range
* and that a bitmap mc-group group cannot be opened out of this range.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_bitmap_allocation_TEST(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_full_table_TEST_1
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that opening new MC-Groups will fail when table is full.
* Small amount of LARGE groups:
* 1.  Open a small amount of LARGE groups (4*4K Ingress) and (4*4K Egress) MC-Groups.
* 2.  Then try and open an Ingress MC-Group with one entry (MC-ID = Don't Care).
*     Expect INSUFFICIENT MEMORY value to be TRUE.
* 3.  Close Egress Group (MC-ID = 0).
* 4.  Open an Ingress MC-Group with one entry (MC-ID = 1). Expect Success.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_full_table_TEST_1(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_full_set_and_get
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that opening new MC-Groups will fail when table is full.
* Large amount of SMALL groups:
* 1.  Open a large amount of SMALL groups (4K*4 Ingress) and (4K*4 Egress) MC-Groups.
* 2.  Then try and open an Ingress MC-Group with one entry (MC-ID = Don't Care). Expect INSUFFICIENT MEMORY value to be TRUE.
* 3.  Close Egress Group (MC-ID = 0). Close Ingress Group (MC-ID = 16K-4).
* 4.  Open an Ingress MC-Group with 8 entries (MC-ID = 1). Expect Success.

*
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_full_set_and_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_full_table_TEST_3
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that a MC-Group that requires more than available will fail to open
* 1.  Open many large groups (8*2K Ingress) and (7*2K Egress) MC-Groups.
*     And open one more Egress MC-Group with 1 entry (MC-ID = 16K-2K = 14K).
* 2.  Then try and open an Ingress MC-Group with 2K entries (MC-ID = 1).
*     Expect INSUFFICIENT MEMORY value to be TRUE.
* 3.  Delete the Egress group with 1 entry (MC-ID = 14K).
* 4.  Open an Ingress MC-Group with 2K entries (MC-ID = 1). Expect success.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_full_table_TEST_3(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/*****************************************************
*NAME
*  soc_petra_mult_logical_test_all
*TYPE:
*  PROC
*DATE:
*  03-Apr-2008
*FUNCTION:
*
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_logical_test_all(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

/* } */
#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_MULTICAST_INGRESS_INCLUDED__*/
#endif
