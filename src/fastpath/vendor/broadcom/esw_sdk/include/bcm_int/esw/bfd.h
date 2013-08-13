/*
 * $Id: bfd.h 1.17 Broadcom SDK $
 * 
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
 * File:        bfd.h
 * Purpose:     Definitions for BFD.
 */

#ifndef _BCM_INT_BFD_H_
#define _BCM_INT_BFD_H_

#if defined(INCLUDE_BFD)

#include <soc/mcm/allenum.h>
#include <soc/tnl_term.h>
#include <bcm/bfd.h>
#include <bcm/tunnel.h>

/*
 * BFD Function Vector Driver
 */
typedef struct _esw_bfd_drv_s {
    /*
     * BFD APIs
     */
    /* Initalize BFD module */
    int (*init)(int unit);
    /* Detach BFD module */
    int (*detach)(int unit);
    /* Create BFD endpoint */
    int (*endpoint_create)(int unit, bcm_bfd_endpoint_info_t *endpoint_info);
    /* Get BFD endpoint */
    int (*endpoint_get)(int unit, bcm_bfd_endpoint_t endpoint, 
                        bcm_bfd_endpoint_info_t *endpoint_info);
    /* Destroy BFD endpoint */
    int (*endpoint_destroy)(int unit, bcm_bfd_endpoint_t endpoint);
    /* Destroy all BFD endpoints */
    int (*endpoint_destroy_all)(int unit);
    /* Generate BFD poll sequence */
    int (*endpoint_poll)(int unit, bcm_bfd_endpoint_t endpoint);
    /* Register BFD callback for event notification */
    int (*event_register)(int unit, bcm_bfd_event_types_t event_types, 
                          bcm_bfd_event_cb cb, void *user_data);
    /* Unregister BFD callback */
    int (*event_unregister)(int unit, bcm_bfd_event_types_t event_types, 
                            bcm_bfd_event_cb cb);
    /* Get BFD endpoint statistics */
    int (*endpoint_stat_get)(int unit, bcm_bfd_endpoint_t endpoint, 
                             bcm_bfd_endpoint_stat_t *ctr_info, uint8 clear);
    /* Set BFD SHA1 authentication */
    int (*auth_sha1_set)(int unit, int index, bcm_bfd_auth_sha1_t *sha1);
    /* Get BFD SHA1 authentication */
    int (*auth_sha1_get)(int unit, int index, bcm_bfd_auth_sha1_t *sha1);
    /* Set BFD Simple Password authentication */
    int (*auth_simple_password_set)(int unit, int index, 
                                    bcm_bfd_auth_simple_password_t *sp);
    /* Get BFD Simple Password authentication */
    int (*auth_simple_password_get)(int unit, int index, 
                                    bcm_bfd_auth_simple_password_t *sp);
    /*
     * Internal
     */
    int (*l3_tnl_term_entry_init)(int unit, 
                                  bcm_tunnel_terminator_t *tnl_info,
                                  soc_tunnel_term_t *entry);
    int (*mpls_lock)(int unit);
    void (*mpls_unlock)(int unit);
} _esw_bfd_drv_t;

extern _esw_bfd_drv_t    *_esw_bfd_drv[BCM_MAX_NUM_UNITS];

#define _ESW_BFD_DRV(_u)   (_esw_bfd_drv[(_u)])


/*
 * Default BFD RX Event Notification thread priority
 */
#define _ESW_BFD_THREAD_PRI_DFLT     200

#endif /* INCLUDE_BFD */

#endif /* _BCM_INT_BFD_H_ */
