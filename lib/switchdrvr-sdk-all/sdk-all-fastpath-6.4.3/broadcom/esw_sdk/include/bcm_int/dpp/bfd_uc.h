/*
 * $Id: bfd_uc.h,v 1.15 Broadcom SDK $ 
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
 *
 * BFD UC Internal header
 */
#ifndef _BCM_INT_DPP_BFD_UC_H_
#define _BCM_INT_DPP_BFD_UC_H_

#include <bcm/bfd.h>
#include <bcm_int/dpp/oam.h>

/*
 * Function:
 *      bcm_petra_bfd_uc_is_init
 * Purpose:
 *      Whether uKernel BFD has been inited.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_is_init(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_init
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_init(int unit);


/*
 * Function:
 *      bcm_petra_bfd_uc_deinit
 * Purpose:
 *      Send a BFD message to deinit the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_deinit(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_tx_start
 * Purpose:
 *      Start all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to start TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_tx_start(int unit);

/*
 * Function:
 *      _bcm_petra_bfd_uc_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to stop TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_tx_stop(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_create
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_endpoint_create(int unit,
                                        bcm_bfd_endpoint_info_t *endpoint_info);
/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_get
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_endpoint_get(int unit,
                                    bcm_bfd_endpoint_t endpoint,
                                    bcm_bfd_endpoint_info_t *endpoint_info);


/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_destroy
 * Purpose:
 *      Destroy an BFD endpoint object.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to destroy.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to destroy endpoint which does not exist
 *      BCM_E_INTERNAL  Unable to release resource /
 *                      Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_destroy_all
 * Purpose:
 *      Destroy all BFD endpoint objects.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_endpoint_destroy_all(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND BFD Session ID not in use
 *      BCM_E_INTERNAL  Unable to read or write resource
 *      BCM_E_PARAM     Session not in Demand Mode/
 *                      Session not in UP state
 * Notes: The poll operation will be initiated.  Poll failure will be
 * signalled via asynchronous callback (aka session failure)
 */
extern int
bcm_petra_bfd_uc_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint);


#endif /*_BCM_INT_DPP_BFD_UC_H_*/

