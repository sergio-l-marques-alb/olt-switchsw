/* $Id: petra_api_egr_queuing.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_egr_queuing.c
*
* MODULE PREFIX:  soc_petra_egr
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_egr_queuing.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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

/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_THRESH_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_thresh_type_verify(
    unit,
    ofp_ndx,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_egr_ofp_thresh_type_set_unsafe(
    unit,
    ofp_ndx,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_thresh_type_set()",0,0);
}

uint32
  soc_petra_egr_ofp_thresh_type_set_dispatch(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID   ofp_ndx,
    SOC_SAND_IN  int                     core,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_THRESH_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_egr_ofp_thresh_type_set(
    unit,
    ofp_ndx,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_thresh_type_set()",0,0);
}

/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_THRESH_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ofp_thresh_type);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_thresh_type_get_unsafe(
    unit,
    ofp_ndx,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_thresh_type_get()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_SCHED_DROP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = soc_petra_egr_sched_drop_verify(
    unit,
    prio_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_sched_drop_set_unsafe(
    unit,
    prio_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_sched_drop_set()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_SCHED_DROP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_sched_drop_get_unsafe(
    unit,
    prio_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_sched_drop_get()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = soc_petra_egr_unsched_drop_verify(
    unit,
    prio_ndx,
    dp_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_unsched_drop_set_unsafe(
    unit,
    prio_ndx,
    dp_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_set()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_unsched_drop_get_unsafe(
    unit,
    prio_ndx,
    dp_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_get()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_DEV_FC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = soc_petra_egr_dev_fc_verify(
    unit,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_dev_fc_set_unsafe(
    unit,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_dev_fc_set()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_DEV_FC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_dev_fc_get_unsafe(
    unit,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_dev_fc_get()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_xaui_spaui_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_CHNIF_FC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = soc_petra_egr_xaui_spaui_fc_verify(
    unit,
    if_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_xaui_spaui_fc_set_unsafe(
    unit,
    if_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_xaui_spaui_fc_set()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_xaui_spaui_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_CHNIF_FC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_xaui_spaui_fc_get_unsafe(
    unit,
    if_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_xaui_spaui_fc_get()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_FC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = soc_petra_egr_ofp_fc_verify(
    unit,
    prio_ndx,
    ofp_type_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_fc_set_unsafe(
    unit,
    prio_ndx,
    ofp_type_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_fc_set()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_FC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_fc_get_unsafe(
    unit,
    prio_ndx,
    ofp_type_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_fc_get()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);


  res = soc_petra_egr_mci_fc_verify(
    unit,
    mci_prio_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_mci_fc_set_unsafe(
    unit,
    mci_prio_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_set()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_mci_fc_get_unsafe(
    unit,
    mci_prio_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_get()",0,0);
}

/*********************************************************************
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  uint8                 mci_enable,
    SOC_SAND_IN  uint8                 erp_enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_egr_mci_fc_enable_verify(
    unit,
    mci_prio_ndx,
    mci_enable,
    erp_enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_mci_fc_enable_set_unsafe(
    unit,
    mci_prio_ndx,
    mci_enable,
    erp_enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_enable_set()",0,0);
}

/*********************************************************************
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT uint8                 *mci_enable,
    SOC_SAND_OUT uint8                 *erp_enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mci_enable);
  SOC_SAND_CHECK_NULL_INPUT(erp_enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_mci_fc_enable_get_unsafe(
    unit,
    mci_prio_ndx,
    mci_enable,
    erp_enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_enable_get()",0,0);
}

/*********************************************************************
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCH_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sch_mode);

  res = soc_petra_egr_ofp_sch_mode_verify(
    unit,
    sch_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_sch_mode_set_unsafe(
    unit,
    sch_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_sch_mode_set()",0,0);
}

/*********************************************************************
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCH_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sch_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_sch_mode_get_unsafe(
    unit,
    sch_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_sch_mode_get()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCHEDULING_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_scheduling_verify(
    unit,
    ofp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_egr_ofp_scheduling_set_unsafe(
    unit,
    ofp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_scheduling_set()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCHEDULING_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_ofp_scheduling_get_unsafe(
    unit,
    ofp_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_scheduling_get()",0,0);
}

/*********************************************************************
*     Sets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_set(
   SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_IN  uint32   drop_prio
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_PRIO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_egr_unsched_drop_prio_verify(
          unit,
          tc_ndx,
          dp_ndx,
          drop_prio
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_unsched_drop_prio_set_unsafe(
          unit,
          tc_ndx,
          dp_ndx,
          drop_prio
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_prio_set()",0,0);
}

/*********************************************************************
*     Gets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_OUT uint32   *drop_prio
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_PRIO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(drop_prio);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_egr_unsched_drop_prio_get_unsafe(
          unit,
          tc_ndx,
          dp_ndx,
          drop_prio
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_prio_get()",0,0);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
*     precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_q_prio_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_Q_PRIO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_egr_q_prio_verify(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          prio
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egr_q_prio_set_unsafe(
          unit,
          map_type_ndx,
          is_sched_mc,
          tc_ndx,
          dp_ndx,
          prio
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_q_prio_set()",0,0);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
*     precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_q_prio_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_Q_PRIO          *prio
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_Q_PRIO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prio);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_egr_q_prio_get_unsafe(
    unit,
    map_type_ndx,
    is_sched_mc,
    tc_ndx,
    dp_ndx,
    prio
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_q_prio_get()",0,0);
}

void
  soc_petra_PETRA_EGR_DROP_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_DROP_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_DEV_THRESH_INNER_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEV_THRESH_INNER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEV_THRESH_INNER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_DEVICE_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEVICE_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_MCI_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_MCI_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_CHNIF_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_CHNIF_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_OFP_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_OFP_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_OFP_SCH_WFQ_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_WFQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_WFQ_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_OFP_SCH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_EGR_Q_PRIO_to_string(
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO enum_val
  )
{
  return SOC_TMC_EGR_Q_PRIO_to_string(enum_val);
}

const char*
  soc_petra_PETRA_EGR_PORT_THRESH_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE enum_val
  )
{
  return SOC_TMC_EGR_PORT_THRESH_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_EGR_MCI_PRIO_to_string(
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO enum_val
  )
{
  return SOC_TMC_EGR_MCI_PRIO_to_string(enum_val);
}

const char*
  soc_petra_PETRA_EGR_OFP_INTERFACE_PRIO_to_string(
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_INTERFACE_PRIO enum_val
  )
{
  return SOC_TMC_EGR_OFP_INTERFACE_PRIO_to_string(enum_val);
}

const char*
  soc_petra_PETRA_EGR_OFP_SCH_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE enum_val
  )
{
  return SOC_TMC_EGR_OFP_SCH_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_EGR_Q_PRIO_MAPPING_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE enum_val
  )
{
  return SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE_to_string(enum_val);
}

void
  soc_petra_PETRA_EGR_DROP_THRESH_print(
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_DROP_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_DEV_THRESH_INNER_print(
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEV_THRESH_INNER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEV_THRESH_INNER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_DEVICE_THRESH_print(
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEVICE_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_MCI_THRESH_print(
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_MCI_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_CHNIF_THRESH_print(
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_CHNIF_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_FC_OFP_THRESH_print(
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_OFP_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_OFP_SCH_WFQ_print(
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_WFQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_WFQ_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_EGR_OFP_SCH_INFO_print(
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

