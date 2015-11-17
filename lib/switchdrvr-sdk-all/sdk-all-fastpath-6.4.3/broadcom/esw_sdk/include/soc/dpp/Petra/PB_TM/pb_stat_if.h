/* $Id: pb_stat_if.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_stat_if.h
*
* MODULE PREFIX:  soc_pb_stat
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

#ifndef __SOC_PB_STAT_IF_INCLUDED__
/* { */
#define __SOC_PB_STAT_IF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_stat_if.h>

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
 *   soc_pb_stat_if_report_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures the format of the report sent
 *   through the statistics interface
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info -
 *     Statistics report format info
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_stat_if_report_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info
  );

uint32
  soc_pb_stat_if_report_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_stat_if_report_set_unsafe" API.
 *     Refer to "soc_pb_stat_if_report_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_stat_if_report_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_INFO      *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_stat_if_bist_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Statistic Interface under a BIST mode. Discard
 *   the statistic reports and allow the user to present test
 *   data on the statistic interface as a debug feature.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info -
 *     Pointer to configuration structure.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_stat_if_bist_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info
  );

uint32
  soc_pb_stat_if_bist_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_stat_if_bist_set_unsafe" API.
 *     Refer to "soc_pb_stat_if_bist_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_stat_if_bist_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_INFO        *info
  );


uint32
  SOC_PB_STAT_IF_BILLING_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_BILLING_INFO *info
  );

uint32
  SOC_PB_STAT_IF_FAP20V_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_FAP20V_INFO *info
  );

uint32
  SOC_PB_STAT_IF_REPORT_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_MODE    report_mode,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_MODE_INFO *info
  );

uint32
  SOC_PB_STAT_IF_REPORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO *info
  );

uint32
  SOC_PB_STAT_IF_BIST_PATTERN_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_PATTERN *info
  );

uint32
  SOC_PB_STAT_IF_BIST_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO *info
  );
#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_STAT_IF_INCLUDED__*/
#endif
