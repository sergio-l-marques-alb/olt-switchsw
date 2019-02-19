/* $Id$
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
/******************************************************************
*
* FILENAME:       DuneDriver/tmc/include/soc_tmcapi_cell.h
*
* MODULE PREFIX:  soc_tmccell
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

#ifndef __SOC_TMC_API_CELL_INCLUDED__
/* { */
#define __SOC_TMC_API_CELL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_TMC_CELL_NOF_LINKS_IN_PATH_LINKS            (4)

#define SOC_TMC_CELL_MC_NOF_LINKS                       (2)
#define SOC_TMC_CELL_MC_DATA_IN_UINT32S                   (3)
#define SOC_TMC_CELL_MC_NOF_CHANGES                     (7)

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
   *  The fabric destination of the inband cell is located at
   *  the first or third stage in a multi-stage system: FE1 or
   *  FE3.
   */
  SOC_TMC_CELL_FE_LOCATION_FE1 = 0,
  /*
   *  The fabric destination of the inband cell is located in
   *  a single-stage system or at the second stage in a
   *  multi-stage system: FE2.
   */
  SOC_TMC_CELL_FE_LOCATION_FE2 = 1,
  /*
   *  Total number of fabric locations.
   */
  SOC_TMC_CELL_NOF_FE_LOCATIONS = 2
}SOC_TMC_CELL_FE_LOCATION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The kind of destination entity type: FE1, FE2, FE3, or
   *  FOP in case of a FAP device.
   */
  SOC_SAND_DEVICE_ENTITY  dest_entity_type;
  /*
   *  The list of the links composing the path used by the
   *  source-routed data cell.
   */
  uint8            path_links[SOC_TMC_CELL_NOF_LINKS_IN_PATH_LINKS];

} SOC_TMC_SR_CELL_LINK_LIST;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Links to get to the fabric. For an FE1 fabric (in
   *  multi-stage) or FE2 fabric (in single stage), only the
   *  first value must be configured. For an FE2 fabric in
   *  multi-stage system, both values must be set. For each
   *  value, Range: 0 - 63.
   */
  uint32 data[SOC_TMC_CELL_MC_DATA_IN_UINT32S];

} SOC_TMC_CELL_MC_TBL_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The fabric stage location: FE1 or FE2.
   */
  SOC_TMC_CELL_FE_LOCATION fe_location;
  /*
   *  Links to get to the fabric. For an FE1 fabric (in
   *  multi-stage) or FE2 fabric (in single stage), only the
   *  first value must be configured. For an FE2 fabric in
   *  multi-stage system, both values must be set. For each
   *  value, Range: 0 - 63.
   */
  uint32 path_links[SOC_TMC_CELL_MC_NOF_LINKS];
  /*
   *  Filter indicating the field changes to perform on the
   *  inband cell (in comparison with the previous inband cell
   *  sent via tmd_cell_mc_tbl_write). If True, the field is
   *  updated, otherwise the previous value is set. The filter
   *  mapping is: 0 - 'mc_id_ndx', 1 - 'fe_location', 2 -
   *  'path_links[0]', 3 - 'path_links[1]', 4 - 'data[0]', 5 -
   *  'data[1]', 6 - 'data[2]'. For a read operation, only the
   *  indexes 0 to 4 are relevant.
   */
  uint8 filter[SOC_TMC_CELL_MC_NOF_CHANGES];

} SOC_TMC_CELL_MC_TBL_INFO;

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

void
  SOC_TMC_SR_CELL_LINK_LIST_clear(
    SOC_SAND_OUT SOC_TMC_SR_CELL_LINK_LIST *info
  );

void
  SOC_TMC_CELL_MC_TBL_DATA_clear(
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_DATA *info
  );

void
  SOC_TMC_CELL_MC_TBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CELL_FE_LOCATION_to_string(
    SOC_SAND_IN  SOC_TMC_CELL_FE_LOCATION enum_val
  );

void
  SOC_TMC_SR_CELL_LINK_LIST_print(
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST *info
  );

void
  SOC_TMC_CELL_MC_TBL_DATA_print(
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_DATA *info
  );

void
  SOC_TMC_CELL_MC_TBL_INFO_print(
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_INFO *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_CELL_INCLUDED__*/
#endif
