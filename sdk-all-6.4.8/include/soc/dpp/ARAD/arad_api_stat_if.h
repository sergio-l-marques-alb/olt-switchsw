/* $Id: arad_api_stat_if.h,v 1.5 Broadcom SDK $
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

#ifndef __ARAD_API_STAT_IF_INCLUDED__
/* { */
#define __ARAD_API_STAT_IF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_general.h>
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

#define ARAD_STAT_IF_PHASE_000                            SOC_TMC_STAT_IF_PHASE_000
#define ARAD_STAT_IF_PHASE_090                            SOC_TMC_STAT_IF_PHASE_090
#define ARAD_STAT_IF_PHASE_180                            SOC_TMC_STAT_IF_PHASE_180
#define ARAD_STAT_IF_PHASE_270                            SOC_TMC_STAT_IF_PHASE_270
#define ARAD_STAT_NOF_IF_PHASES                           SOC_TMC_STAT_NOF_IF_PHASES
typedef SOC_TMC_STAT_IF_PHASE                                  ARAD_STAT_IF_PHASE;

#define ARAD_STAT_IF_REPORT_MODE_BILLING                  SOC_TMC_STAT_IF_REPORT_MODE_BILLING
#define ARAD_STAT_IF_REPORT_MODE_FAP20V                   SOC_TMC_STAT_IF_REPORT_MODE_FAP20V
#define ARAD_STAT_NOF_IF_REPORT_MODES                     SOC_TMC_STAT_NOF_IF_REPORT_MODES
typedef SOC_TMC_STAT_IF_REPORT_MODE                            ARAD_STAT_IF_REPORT_MODE;

#define ARAD_STAT_IF_BILLING_MODE_EGR_Q_NUM               SOC_TMC_STAT_IF_BILLING_MODE_EGR_Q_NUM
#define ARAD_STAT_IF_BILLING_MODE_CUD                     SOC_TMC_STAT_IF_BILLING_MODE_CUD
#define ARAD_STAT_IF_BILLING_MODE_VSI_VLAN                SOC_TMC_STAT_IF_BILLING_MODE_VSI_VLAN
#define ARAD_STAT_IF_BILLING_MODE_BOTH_LIFS               SOC_TMC_STAT_IF_BILLING_MODE_BOTH_LIFS
typedef SOC_TMC_STAT_IF_BILLING_MODE                           ARAD_STAT_IF_BILLING_MODE;

#define ARAD_STAT_IF_FAP20V_MODE_Q_SIZE                   SOC_TMC_STAT_IF_FAP20V_MODE_Q_SIZE
#define ARAD_STAT_IF_FAP20V_MODE_PKT_SIZE                 SOC_TMC_STAT_IF_FAP20V_MODE_PKT_SIZE
typedef SOC_TMC_STAT_IF_FAP20V_MODE                            ARAD_STAT_IF_FAP20V_MODE;

#define ARAD_STAT_IF_MC_MODE_Q_NUM                        SOC_TMC_STAT_IF_MC_MODE_Q_NUM
#define ARAD_STAT_IF_MC_MODE_MC_ID                        SOC_TMC_STAT_IF_MC_MODE_MC_ID
typedef SOC_TMC_STAT_IF_MC_MODE                                ARAD_STAT_IF_MC_MODE;

#define ARAD_STAT_IF_CNM_MODE_DIS                         SOC_TMC_STAT_IF_CNM_MODE_DIS
#define ARAD_STAT_IF_CNM_MODE_EN                          SOC_TMC_STAT_IF_CNM_MODE_EN
typedef SOC_TMC_STAT_IF_CNM_MODE                               ARAD_STAT_IF_CNM_MODE;

#define ARAD_STAT_IF_PARITY_MODE_DIS                      SOC_TMC_STAT_IF_PARITY_MODE_DIS
#define ARAD_STAT_IF_PARITY_MODE_EN                       SOC_TMC_STAT_IF_PARITY_MODE_EN
typedef SOC_TMC_STAT_IF_PARITY_MODE                            ARAD_STAT_IF_PARITY_MODE;

#define ARAD_STAT_IF_BIST_EN_MODE_DIS                     SOC_TMC_STAT_IF_BIST_EN_MODE_DIS
#define ARAD_STAT_IF_BIST_EN_MODE_PATTERN                 SOC_TMC_STAT_IF_BIST_EN_MODE_PATTERN
#define ARAD_STAT_IF_BIST_EN_MODE_WALKING_ONE             SOC_TMC_STAT_IF_BIST_EN_MODE_WALKING_ONE
#define ARAD_STAT_IF_BIST_EN_MODE_PRBS                    SOC_TMC_STAT_IF_BIST_EN_MODE_PRBS
typedef SOC_TMC_STAT_IF_BIST_EN_MODE                           ARAD_STAT_IF_BIST_EN_MODE;

typedef SOC_TMC_STAT_IF_INFO                                   ARAD_STAT_IF_INFO;
typedef SOC_TMC_STAT_IF_BILLING_INFO                           ARAD_STAT_IF_REPORT_MODE_BILLING_INFO;
typedef SOC_TMC_STAT_IF_FAP20V_INFO                            ARAD_STAT_IF_REPORT_MODE_FAP20V_INFO;
typedef SOC_TMC_STAT_IF_REPORT_MODE_INFO                       ARAD_STAT_IF_REPORT_MODE_INFO;
typedef SOC_TMC_STAT_IF_REPORT_INFO                            ARAD_STAT_IF_REPORT_INFO;
typedef SOC_TMC_STAT_IF_BIST_PATTERN                           ARAD_STAT_IF_BIST_PATTERN;
typedef SOC_TMC_STAT_IF_BIST_INFO                              ARAD_STAT_IF_BIST_INFO;

#define ARAD_STAT_NOF_IF_BILLING_MODES                   SOC_TMC_STAT_NOF_IF_BILLING_MODES;
#define ARAD_STAT_NOF_IF_FAP20V_MODES                    SOC_TMC_STAT_NOF_IF_FAP20V_MODES;

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
*     arad_stat_if_info_set
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
  arad_stat_if_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_info_get
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
  arad_stat_if_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_INFO        *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_report_info_set
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
  arad_stat_if_report_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  );

/*********************************************************************
* NAME:
*     arad_stat_if_report_info_get
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
  arad_stat_if_report_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  );

void
  arad_ARAD_STAT_IF_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_INFO *info
  );


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_API_STAT_IF_INCLUDED__*/
#endif

