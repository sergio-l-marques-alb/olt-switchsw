/*
 * $Id: dcmn_cmic.h,v 1.0 Broadcom SDK $
 *
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
 *
 * SOC DCMN IPROC
 */

#ifndef _SOC_DCMN_CMIC_DRV_H_
#define _SOC_DCMN_CMIC_DRV_H_

#include <soc/defs.h>
#include <soc/dcmn/dcmn_defs.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#endif /* BCM_DPP_SUPPORT */


/*
 * Function:
 *      soc_dcmn_cmic_device_hard_reset
 * Purpose:
 *      if proper reset action is given, resets device and makes sure device is out of reset.
 *      reset flags are: SOC_DCMN_RESET_ACTION_IN_RESET, SOC_DCMN_RESET_ACTION_INOUT_RESET
 * Parameters:
 *      unit            - Device Number
 *      reset_action    - Action to perform
 * Returns:
 *      SOC_E_XXX
 */
int soc_dcmn_cmic_device_hard_reset(int unit, int reset_action);


/*
 * Function:
 *      soc_dcmn_cmic_sbus_timeout_set
 * Purpose:
 *      setting the timeout value of the sbus
 * Parameters:
 *      unit            - Device Number
 *      core_freq_khz   - the freq of the core in khz
 *      schan_timeout   - time in microseconds
 * Returns:
 *      SOC_E_XXX
 */
int soc_dcmn_cmic_sbus_timeout_set(int unit, uint32 core_freq_khz, int schan_timeout);
int soc_dcmn_cmic_pcie_userif_purge_ctrl_init(int unit);
int soc_dcmn_cmic_mdio_config(int unit, int dividend, int divisor, int delay);

/*
 * Function:
 *      soc_dcmn_cmic_mdio_set
 * Purpose:
 *      setting the CMIC MDIO parameters
 * Parameters:
 *      unit            - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_dcmn_cmic_mdio_set(int unit);

#endif /* _SOC_DCMN_CMIC_DRV_H_ */

