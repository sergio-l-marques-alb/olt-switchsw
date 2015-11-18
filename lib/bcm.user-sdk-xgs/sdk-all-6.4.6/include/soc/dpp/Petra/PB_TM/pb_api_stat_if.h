/* $Id: pb_api_stat_if.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_stat_if.h
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

#ifndef __SOC_PB_API_STAT_IF_INCLUDED__
/* { */
#define __SOC_PB_API_STAT_IF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

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

typedef enum
{
  /*
   *  Egress queue number presentation in the egress report.
   */
  SOC_PB_STAT_IF_BILLING_MODE_EGR_Q_NB = 0,
  /*
   *  Copy-Unique-Data (Out-LIF) presentation in the egress
   *  report.
   */
  SOC_PB_STAT_IF_BILLING_MODE_CUD = 1,
  /*
   *  Egress statistics according to the VSI (VLAN).
   */
  SOC_PB_STAT_IF_BILLING_MODE_VSI_VLAN = 2,
  /*
   *  Both In-LIF and Out-LIF presentations in the egress
   *  report.
   */
  SOC_PB_STAT_IF_BILLING_MODE_BOTH_LIFS = 3,
  /*
   *  Number of types in SOC_PB_STAT_IF_BILLING_MODE
   */
  SOC_PB_STAT_NOF_IF_BILLING_MODES = 4
}SOC_PB_STAT_IF_BILLING_MODE;

typedef enum
{
  /*
   *  Queue size report format.
   */
  SOC_PB_STAT_IF_FAP20V_MODE_Q_SIZE = 0,
  /*
   *  Packet size report format.
   */
  SOC_PB_STAT_IF_FAP20V_MODE_PKT_SIZE = 1,
  /*
   *  Number of types in SOC_PB_STAT_IF_FAP20V_MODE
   */
  SOC_PB_STAT_NOF_IF_FAP20V_MODES = 2
}SOC_PB_STAT_IF_FAP20V_MODE;

typedef enum
{
  /*
   *  The multicast packets are reported with the Queue
   *  number.
   */
  SOC_PB_STAT_IF_MC_MODE_Q_NUM = 0,
  /*
   *  The multicast packets are reported with the Multicast
   *  ID.
   */
  SOC_PB_STAT_IF_MC_MODE_MC_ID = 1,
  /*
   *  Number of types in SOC_PB_STAT_IF_MC_MODE
   */
  SOC_PB_STAT_NOF_IF_MC_MODES = 2
}SOC_PB_STAT_IF_MC_MODE;

typedef enum
{
  /*
   *  The CNM packet statistics are not reported.
   */
  SOC_PB_STAT_IF_CNM_MODE_DIS = 0,
  /*
   *  The CNM packet statistics are reported.
   */
  SOC_PB_STAT_IF_CNM_MODE_EN = 1,
  /*
   *  Number of types in SOC_PB_STAT_IF_CNM_MODE
   */
  SOC_PB_STAT_NOF_IF_CNM_MODES = 2
}SOC_PB_STAT_IF_CNM_MODE;

typedef enum
{
  /*
   *  The parity bit/bits errors identification is not set.
   */
  SOC_PB_STAT_IF_PARITY_MODE_DIS = 0,
  /*
   *  The parity bit/bits errors identification is set.
   */
  SOC_PB_STAT_IF_PARITY_MODE_EN = 1,
  /*
   *  Number of types in SOC_PB_STAT_IF_PARITY_MODE
   */
  SOC_PB_STAT_NOF_IF_PARITY_MODES = 2
}SOC_PB_STAT_IF_PARITY_MODE;

typedef enum
{
  /*
   *  BIST commands on Statistic interface are disabled.
   *  Single mode which enables the transmission of statistic
   *  reports.
   */
  SOC_PB_STAT_IF_BIST_EN_MODE_DIS = 0,
  /*
   *  The BIST commands write alternatively two pattern words
   *  in continuous.
   */
  SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN = 1,
  /*
   *  The BIST commands perform a walking one writing.
   */
  SOC_PB_STAT_IF_BIST_EN_MODE_WALKING_ONE = 2,
  /*
   *  The BIST patterns are generated under a PRBS mode.
   */
  SOC_PB_STAT_IF_BIST_EN_MODE_PRBS = 3,
  /*
   *  Number of types in SOC_PB_STAT_IF_BIST_EN_MODE
   */
  SOC_PB_STAT_NOF_IF_BIST_EN_MODES = 4
}SOC_PB_STAT_IF_BIST_EN_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Billing mode for the egress report: with Egress Queue
   *  number / CUD / VSI-VLAN / In-LIF and Out-LIF
   */
  SOC_PB_STAT_IF_BILLING_MODE mode;

} SOC_PB_STAT_IF_BILLING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Select between Packet-size and Queue-size formats
   */
  SOC_PB_STAT_IF_FAP20V_MODE mode;
  /*
   *  Multicast report format for the Fabric Multicast: report
   *  of the copies with their Queue number or with their
   *  Multicast-Ids. Valid only if the mode is 'PKT_SIZE'.
   */
  SOC_PB_STAT_IF_MC_MODE fabric_mc;
  /*
   *  Multicast report format for the Ingress Replication
   *  Multicast: report of the copies with their Queue number
   *  or with their Multicast-Ids. Valid only if the mode is
   *  'PKT_SIZE'.
   */
  SOC_PB_STAT_IF_MC_MODE ing_rep_mc;
  /*
   *  TRUE - snoop/mirror packets are also counted in the
   *  Copy-Count
   */
  uint8 count_snoop;
  /*
   *  If True, then the reported packet size is the one at the
   *  packet reception. Otherwise, the reported packet size is
   *  the one after the header editing.
   */
  uint8 is_original_pkt_size;
  /*
   *  If True, then only a single copy per multicast packet
   *  (for ingress replication multicast packets) is reported.
   *  Otherwise, all the packets are reported.
   */
  uint8 single_copy_reported;
  /*
   *  CNM (Congestion Notification Message) report mode. Valid
   *  only if the mode is 'PKT_SIZE'.
   */
  SOC_PB_STAT_IF_CNM_MODE cnm_report;

} SOC_PB_STAT_IF_FAP20V_INFO;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Billing mode configuration
   */
  SOC_PB_STAT_IF_BILLING_INFO billing;
  /*
   *  Fap20v mode configuration
   */
  SOC_PB_STAT_IF_FAP20V_INFO fap20v;

} SOC_PB_STAT_IF_REPORT_MODE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Statistics report mode
   */
  SOC_PETRA_STAT_IF_REPORT_MODE mode;
  /*
   *  Format mode configuration
   */
  SOC_PB_STAT_IF_REPORT_MODE_INFO format;
  /*
   *  Statistics' interface sync period. Defines the maximal
   *  period between consecutive sync patterns transmitted on
   *  the statistics interface. Zero disables sync patterns
   *  transmission. Units: Nanoseconds.
   */
  uint32 sync_rate;
  /*
   *  Parity mode. If enabled, an identification of errors in
   *  the report discards it. According to its value, the
   *  fields are different in the statistic report.
   */
  SOC_PB_STAT_IF_PARITY_MODE parity_mode;

} SOC_PB_STAT_IF_REPORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  First word of the pattern. Range: 0 - 0xFFFFF.
   */
  uint32 word1;
  /*
   *  Second word of the pattern. Range: 0 - 0xFFFFF.
   */
  uint32 word2;

} SOC_PB_STAT_IF_BIST_PATTERN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable (in the specified mode)/Disable BIST commands on
   *  the Statistic Interface.
   */
  SOC_PB_STAT_IF_BIST_EN_MODE en_mode;
  /*
   *  Pattern parameters. Relevant only if 'en_mode' is set to
   *  'PATTERN'.
   */
  SOC_PB_STAT_IF_BIST_PATTERN pattern;
  /*
   *  Number of successive duplications of the BIST commands
   *  in a single burst. Range: 0 - 63.
   */
  uint32 nof_duplications;
  /*
   *  Number of clock cycles representing the time interval
   *  between two BIST command bursts. Must be a multiple of
   *  2. Range: 0 - 126.
   */
  uint32 nof_idle_clocks;

} SOC_PB_STAT_IF_BIST_INFO;


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
 *   soc_pb_stat_if_report_set
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
  soc_pb_stat_if_report_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_stat_if_report_set" API.
 *     Refer to "soc_pb_stat_if_report_set" API for details.
*********************************************************************/
uint32
  soc_pb_stat_if_report_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_INFO      *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_stat_if_bist_set
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
  soc_pb_stat_if_bist_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_stat_if_bist_set"
 *     API.
 *     Refer to "soc_pb_stat_if_bist_set" API for details.
*********************************************************************/
uint32
  soc_pb_stat_if_bist_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_INFO        *info
  );

void
  SOC_PB_STAT_IF_BILLING_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_BILLING_INFO *info
  );

void
  SOC_PB_STAT_IF_FAP20V_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_FAP20V_INFO *info
  );

void
  SOC_PB_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_MODE    report_mode,
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_MODE_INFO *info
  );

void
  SOC_PB_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_INFO *info
  );

void
  SOC_PB_STAT_IF_BIST_PATTERN_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_PATTERN *info
  );

void
  SOC_PB_STAT_IF_BIST_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_STAT_IF_BILLING_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_BILLING_MODE enum_val
  );

const char*
  SOC_PB_STAT_IF_FAP20V_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_FAP20V_MODE enum_val
  );

const char*
  SOC_PB_STAT_IF_MC_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_MC_MODE enum_val
  );

const char*
  SOC_PB_STAT_IF_CNM_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_CNM_MODE enum_val
  );

const char*
  SOC_PB_STAT_IF_PARITY_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_PARITY_MODE enum_val
  );

const char*
  SOC_PB_STAT_IF_BIST_EN_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_EN_MODE enum_val
  );

void
  SOC_PB_STAT_IF_BILLING_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_BILLING_INFO *info
  );

void
  SOC_PB_STAT_IF_FAP20V_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_FAP20V_INFO *info
  );

void
  SOC_PB_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_MODE_INFO *info
  );

void
  SOC_PB_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO *info
  );

void
  SOC_PB_STAT_IF_BIST_PATTERN_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_PATTERN *info
  );

void
  SOC_PB_STAT_IF_BIST_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_STAT_IF_INCLUDED__*/
#endif
