/* $Id: qax_fabric.h,v 1.30 Broadcom SDK $
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


#ifndef __QAX_FABRIC_INCLUDED__
/* { */
#define __QAX_FABRIC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


/*Index to TXQ_PRIORITY_BITS_MAPPING_2_FDT is built: hp:1, mc:1, dp:2, trfcls:3, i.e. bhmddttt*/
#define SOC_QAX_FABRIC_PRIORITY_NDX_NOF             (128)
#define SOC_QAX_FABRIC_PRIORITY_NDX_TC_MASK         (0x7)
#define SOC_QAX_FABRIC_PRIORITY_NDX_TC_OFFSET       (0)
#define SOC_QAX_FABRIC_PRIORITY_NDX_DP_MASK         (0x18)
#define SOC_QAX_FABRIC_PRIORITY_NDX_DP_OFFSET       (3)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_MASK      (0x20)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_OFFSET    (5)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_MASK      (0x40)
#define SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_OFFSET    (6)


/* } */

/*************
 * MACROS    *
 *************/
/* { */

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
*     soc_qax_fabric_multicast_set
* FUNCTION:
*     Setting destination for a specific multicast id in kalia
* INPUT:
*       int             unit            - Identifier of the device to access.
*       soc_multicast_t mc_id           - multicast id
*       uint32          destid_count    - number of destination for this mc_id
*       soc_module_t    *destid_array   - specific destination for replication for this specific mc_id
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
soc_qax_fabric_multicast_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count,
    SOC_SAND_IN  soc_module_t                   *destid_array
  );

/*********************************************************************
* NAME:
*     soc_qax_fabric_force_set
* FUNCTION:
*     DIAG function. Force fabric interface for local / fabric or restore back to operational mode
* INPUT:
*       int   unit - Identifier of the device to access.
*       soc_dpp_fabric_force_t force - enum for requested force mode (local/fabric/restore)
* RETURNS:
*       OK or ERROR indication.
* REMARKS:
*       Relevant for Kalia only. Not supported in qax.
*       Used in mbcm dispatcher.
*********************************************************************/
soc_error_t
  soc_qax_fabric_force_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN soc_dpp_fabric_force_t        force
  );

/*********************************************************************
* NAME:
*     soc_qax_fabric_link_config_ovrd
* FUNCTION:
*     Overwriting qax default fabric configuration in case of kalia
* INPUT:
*       int   unit - Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
soc_qax_fabric_link_config_ovrd(
  int                unit
);




/*********************************************************************
* NAME:
*     soc_qax_fabric_cosq_control_backward_flow_control_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 03 2015
* FUNCTION:
*     Enable / disable backwards flow control on supported fifos
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          port -
*     gport number.
*  SOC_SAND_IN int                                   enable -
*     Whether to enable / disable the feature.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_set(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_IN int                                   enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );

soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_get(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_OUT int                                  *enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );


/*********************************************************************
* NAME:
*     soc_qax_fabric_egress_core_cosq_gport_sched_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 03 2015
* FUNCTION:
*     Set WFQ weight on supported fifos.
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          pipe -
*     Which pipe to access.
*  SOC_SAND_IN int                                  weight -
*     Weight value to configure.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type);

soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type
  );

/*********************************************************************
* NAME:
*     soc_qax_cosq_gport_sched_set/get
* FUNCTION:
*     Configuration of weight for WFQs in fabric pipes:
*     all, ingress, egress.
* INPUT:
*  SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  int                                pipe -
*     Which pipe's weight to configure (0,1,2)
*  SOC_SAND_IN/SOC_SAND_OUT  int/int*              weight -
*     value to configure/retrieve pipe's weight
*  SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type -
*     type of fabric pipe to configure (all, ingress, egress)
*     Note: egress is not legal argument for QAX. "All" argument is actually identical to "ingress" argument
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  );

soc_error_t
  soc_qax_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT  int*                               weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  );

/*********************************************************************
* NAME:
*     soc_qax_fabric_cosq_gport_priority_drop_threshold_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 10 2015
* FUNCTION:
*     Set priority drop threshold on supported fifos.
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          gport -
*     gport number.
*  SOC_SAND_IN  soc_cosq_threshold_t                *threshold_val -
*     sturuct which contains the threshold value
*     to configure / retreive.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*     NOTE: Only soc_dpp_cosq_gport_egress_core_fifo_local_mcast fifo_type is supported in QAX!
*
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );


/*********************************************************************
* NAME:
*     soc_qax_fabric_cosq_gport_rci_threshold_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 10 2015
* FUNCTION:
*     Set rci threshold on supported fifos.
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          gport -
*     gport number.
*  SOC_SAND_IN  int                                 threshold_val -
*     Threshold value to configure.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*     Note: For QAX, only soc_dpp_cosq_gport_egress_core_fifo_local_ucast is supported.
*
* REMARKS:
*     Used in mbcm dispatcher.
*     For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT  int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );


/*********************************************************************
* NAME:
*     soc_qax_fabric_priority_set / set
* TYPE:
*   PROC
* DATE:
*   Dec 13 2015
* FUNCTION:
*     Set / Get fabric priority according to:
*     traffic_class, queue_type: hc/lc (flags), dp(color).
* INPUT:
*  SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                             tc -
*     traffic class
*  SOC_SAND_IN  uint32                             dp -
*     drop precedence
*  SOC_SAND_IN  uint32                             flags -
*     relevant flags for cell (is_mc, is_hp)
*  SOC_SAND_IN/OUT   int/int*                      fabric_priority -
*     fabric priority to set/ get in TXQ_PRIORITY_BITS_MAPPING_2_FDT
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
soc_qax_fabric_priority_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_IN  int                fabric_priority
  );

soc_error_t
soc_qax_fabric_priority_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_OUT  int                *fabric_priority
  );

/*********************************************************************
* NAME:
*     soc_qax_fabric_queues_info_get
* TYPE:
*   PROC
* DATE:
*   Jun 8 2016
* FUNCTION:
*     Get DTQ and PDQ (DQCQ/DBLF) queues status
* INPUT:
*  SOC_SAND_IN  int                         unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT soc_dpp_fabric_queues_info_t*
*                                           queues_info-
*     struct to hold the status.
* REMARKS:
*   Used in mbcm dispatcher.
*********************************************************************/
uint32
  soc_qax_fabric_queues_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_OUT soc_dpp_fabric_queues_info_t    *queues_info
  );


/*********************************************************************
* NAME:
*     qax_fabric_pcp_dest_mode_config_set
* TYPE:
*   PROC
* DATE:
*   Dec 16 2015
* FUNCTION:
*     Enables set / get operations on fabric-pcp (packet cell packing)
*     per destination device.
*     there are three supported pcp modes:
*       - 0- No Packing
*       - 1- Simple Packing
*       - 2- Continuous Packing
* INPUT:
*  SOC_SAND_IN  int                                     unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                                  flags-
*  SOC_SAND_IN  uint32                                  modid-
*     Id of destination device
*  SOC_SAND_IN/OUT uint32*     							pcp_mode-
*     mode of pcp to set/get.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  qax_fabric_pcp_dest_mode_config_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_IN uint32           pcp_mode
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __QAX_FABRIC_INCLUDED__*/
#endif
