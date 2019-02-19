/* $Id: arad_stat_if.h,v 1.8 Broadcom SDK $
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

#ifndef __ARAD_STAT_IF_INCLUDED__
/* { */
#define __ARAD_STAT_IF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_stat_if.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_STAT_IF_REPORT_THRESHOLD_LINES                      15
#define ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN                   ((96*1024)-1)
#define ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX                   ((96*1024)-1) 
#define ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED                    0xFFFFFFFF
#define ARAD_STAT_IF_REPORT_SCRUBBER_DISABLE                     0
#define ARAD_STAT_IF_REPORT_WC_ID                                7

#ifdef BCM_88650_B0
#define ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MIN                   (0)
#define ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MAX                   ((96*1024)-1) 
#endif

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
* arad_mgmt_functional_init     
* FUNCTION:
*   Initialization of the Arad blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  arad_stat_if_init(
    SOC_SAND_IN  int                 unit
  );



/*********************************************************************
* NAME:
*     arad_stat_if_info_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the working mode of the
*     statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  ARAD_STAT_IF_INFO        *info -
*     Statistics interface info
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  arad_stat_if_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_info_verify
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the working mode of the
*     statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  ARAD_STAT_IF_INFO        *info -
*     Statistics interface info
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  arad_stat_if_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_info_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the working mode of the
*     statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT ARAD_STAT_IF_INFO        *info -
*     Statistics interface info
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  arad_stat_if_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_report_info_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the format of the report sent
*     through the statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info -
*     Statistics report format info
* REMARKS:
*     For Arad-B, the API soc_pb_stat_if_report_set must be used instead.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  arad_stat_if_report_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_report_info_verify
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the format of the report sent
*     through the statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info -
*     Statistics report format info
* REMARKS:
*     For Arad-B, the API soc_pb_stat_if_report_set must be used instead.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  arad_stat_if_report_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_report_info_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the format of the report sent
*     through the statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info -
*     Statistics report format info
* REMARKS:
*     For Arad-B, the API soc_pb_stat_if_report_set must be used instead.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  arad_stat_if_report_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  );


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_STAT_IF_INCLUDED__*/
#endif

