/* $Id: petra_stat_if.h,v 1.7 Broadcom SDK $
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


#ifndef __SOC_PETRA_STAT_IF_INCLUDED__
/* { */
#define __SOC_PETRA_STAT_IF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/petra_api_stat_if.h>

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
*     soc_petra_stat_if_info_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the working mode of the
*     statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info -
*     Statistics interface info
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_stat_if_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_info_verify
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the working mode of the
*     statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info -
*     Statistics interface info
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_stat_if_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_info_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the working mode of the
*     statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO        *info -
*     Statistics interface info
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_stat_if_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_report_info_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the format of the report sent
*     through the statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info -
*     Statistics report format info
* REMARKS:
*     For Soc_petra-B, the API soc_pb_stat_if_report_set must be used instead.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_stat_if_report_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_report_info_verify
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the format of the report sent
*     through the statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info -
*     Statistics report format info
* REMARKS:
*     For Soc_petra-B, the API soc_pb_stat_if_report_set must be used instead.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_stat_if_report_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_report_info_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function configures the format of the report sent
*     through the statistics interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info -
*     Statistics report format info
* REMARKS:
*     For Soc_petra-B, the API soc_pb_stat_if_report_set must be used instead.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_stat_if_report_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_STAT_IF_INCLUDED__*/
#endif
