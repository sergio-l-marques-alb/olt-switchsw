/*
 * $Id: bfd_uc.h,v 1.15 Broadcom SDK $ 
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


/*
 * Function:
 *      bcm_petra_bfd_uc_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
extern int
bcm_petra_bfd_uc_event_register(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data);

/*
 * Function:
 *      bcm_petra_bfd_uc_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
extern int
bcm_petra_bfd_uc_event_unregister(int unit,
                               bcm_bfd_event_types_t event_types,
                               bcm_bfd_event_cb cb);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to counter information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_endpoint_stat_get(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint8 clear);
/*
 * Function:
 *      bcm_petra_bfd_uc_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_sha1_set(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1);

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_sha1_get(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1);

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_simple_password_set(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp);

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_simple_password_get(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp);


#endif /*_BCM_INT_DPP_BFD_UC_H_*/

