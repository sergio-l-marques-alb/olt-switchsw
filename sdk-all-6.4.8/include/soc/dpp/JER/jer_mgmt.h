/*
 * $Id: $
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
 *
 * File: jer_mgmt.h
 */

#ifndef __JER_MGMT_INCLUDED__

#define __JER_MGMT_INCLUDED__

/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth
  ) ;
/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*********************************************************************/
int
   jer_mgmt_credit_worth_get(
           SOC_SAND_IN  int                 unit,
           SOC_SAND_OUT uint32              *credit_worth
          ) ;



uint32
  jer_mgmt_credit_worth_remote_set(
    SOC_SAND_IN  int    unit,
	SOC_SAND_IN  uint32    credit_worth_remote
  ) ;


uint32
  jer_mgmt_credit_worth_remote_get(
    SOC_SAND_IN  int    unit,
	SOC_SAND_OUT uint32    *credit_worth_remote
  ) ;


uint32
  jer_mgmt_module_to_credit_worth_map_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type /* should be one of JERICHO_FAP_CREDIT_VALUE_* */
  ) ;


uint32
  jer_mgmt_module_to_credit_worth_map_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type /* will be one of JERICHO_FAP_CREDIT_VALUE_* */
  ) ;



uint32
  jer_mgmt_change_all_faps_credit_worth_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8     credit_value_to_use
  ) ;




/*********************************************************************
* Set the fabric system ID of the device. Must be unique in the system.
*********************************************************************/
uint32 jer_mgmt_system_fap_id_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  sys_fap_id
  );

/* return the FAP ID of (core 0 of) the device */
uint32
  jer_mgmt_system_fap_id_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32              *sys_fap_id
  );

int jer_mgmt_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances); 


/* 
 * PVT
 */
int jer_mgmt_temp_pvt_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
int jer_mgmt_drv_pvt_monitor_enable(int unit);

/*
 * Function:
 *      jer_mgmt_revision_fixes
 * Purpose:
 *      set all the bits controlling the revision fixes (chicken bits) in the device.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int jer_mgmt_revision_fixes (int unit);

/*********************************************************************
* NAME:
*     jer_mgmt_enable_traffic_set
* TYPE:
*   PROC
* DATE:
*   Nov 16 2014
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     SOC_SAND_IN uint8 enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  jer_mgmt_enable_traffic_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

uint32
  jer_mgmt_enable_traffic_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *enable
  );

/*********************************************************************
* NAME:
*     jer_mgmt_enable_traffic_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Nov 16 2014
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     SOC_SAND_IN uint32 enable_indication
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  jer_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

uint32
  jer_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  uint8 *enable
  );


/*********************************************************************
 * Set the MTU (maximal allowed packet size) for any packet,
 * according to the buffer size.
 *********************************************************************/
uint32 jer_mgmt_set_mru_by_dbuff_size(
    SOC_SAND_IN  int     unit
  );


#endif /*__JER_MGMT_INCLUDED__*/

