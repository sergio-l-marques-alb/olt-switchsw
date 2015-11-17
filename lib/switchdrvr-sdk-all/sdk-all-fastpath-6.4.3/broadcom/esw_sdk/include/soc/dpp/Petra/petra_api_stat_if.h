/* $Id: petra_api_stat_if.h,v 1.7 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_STAT_IF_INCLUDED__
/* { */
#define __SOC_PETRA_API_STAT_IF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_stat_if.h>


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

typedef SOC_TMC_STAT_IF_PHASE                                  SOC_PETRA_STAT_IF_PHASE;

#define SOC_PETRA_STAT_IF_REPORT_MODE_BILLING                  SOC_TMC_STAT_IF_REPORT_MODE_BILLING
#define SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V                   SOC_TMC_STAT_IF_REPORT_MODE_FAP20V
#define SOC_PETRA_STAT_NOF_IF_REPORT_MODES                     SOC_TMC_STAT_NOF_IF_REPORT_MODES_PETRA
typedef SOC_TMC_STAT_IF_REPORT_MODE                            SOC_PETRA_STAT_IF_REPORT_MODE;

#define SOC_PETRA_STAT_IF_BILLING_MODE_EGR_Q_NUM               SOC_TMC_STAT_IF_BILLING_MODE_EGR_Q_NUM
#define SOC_PETRA_STAT_IF_BILLING_MODE_CUD                     SOC_TMC_STAT_IF_BILLING_MODE_CUD
typedef SOC_TMC_STAT_IF_BILLING_MODE                           SOC_PETRA_STAT_IF_BILLING_MODE;

#define SOC_PETRA_STAT_IF_FAP20V_MODE_Q_SIZE                   SOC_TMC_STAT_IF_FAP20V_MODE_Q_SIZE
#define SOC_PETRA_STAT_IF_FAP20V_MODE_PKT_SIZE                 SOC_TMC_STAT_IF_FAP20V_MODE_PKT_SIZE
typedef SOC_TMC_STAT_IF_FAP20V_MODE                            SOC_PETRA_STAT_IF_FAP20V_MODE;

#define SOC_PETRA_STAT_IF_MC_MODE_Q_NUM                        SOC_TMC_STAT_IF_MC_MODE_Q_NUM
#define SOC_PETRA_STAT_IF_MC_MODE_MC_ID                        SOC_TMC_STAT_IF_MC_MODE_MC_ID
typedef SOC_TMC_STAT_IF_MC_MODE                                SOC_PETRA_STAT_IF_MC_MODE;

typedef SOC_TMC_STAT_IF_CNM_MODE                               SOC_PETRA_STAT_IF_CNM_MODE;

typedef SOC_TMC_STAT_IF_PARITY_MODE                            SOC_PETRA_STAT_IF_PARITY_MODE;

typedef SOC_TMC_STAT_IF_BIST_EN_MODE                           SOC_PETRA_STAT_IF_BIST_EN_MODE;

typedef SOC_TMC_STAT_IF_INFO                                   SOC_PETRA_STAT_IF_INFO;
typedef SOC_TMC_STAT_IF_BILLING_INFO                           SOC_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO;
typedef SOC_TMC_STAT_IF_FAP20V_INFO                            SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO;
typedef SOC_TMC_STAT_IF_REPORT_MODE_INFO                       SOC_PETRA_STAT_IF_REPORT_MODE_INFO;
typedef SOC_TMC_STAT_IF_REPORT_INFO                            SOC_PETRA_STAT_IF_REPORT_INFO;
typedef SOC_TMC_STAT_IF_BIST_PATTERN                           SOC_PETRA_STAT_IF_BIST_PATTERN;
typedef SOC_TMC_STAT_IF_BIST_INFO                              SOC_PETRA_STAT_IF_BIST_INFO;


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
*     soc_petra_stat_if_info_set
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
  soc_petra_stat_if_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_info_get
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
  soc_petra_stat_if_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_report_info_set
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
  soc_petra_stat_if_report_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_stat_if_report_info_get
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
  soc_petra_stat_if_report_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_MODE_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_STAT_IF_PHASE_to_string(
    SOC_SAND_IN SOC_PETRA_STAT_IF_PHASE enum_val
  );

const char*
  soc_petra_PETRA_STAT_IF_REPORT_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_MODE enum_val
  );

void
  soc_petra_PETRA_STAT_IF_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_MODE_INFO *info
  );

void
  soc_petra_PETRA_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_INFO *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_STAT_IF_INCLUDED__*/
#endif
