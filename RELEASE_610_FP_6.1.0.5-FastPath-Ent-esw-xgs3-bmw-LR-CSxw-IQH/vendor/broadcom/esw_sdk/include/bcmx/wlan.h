/* 
 * $Id: wlan.h,v 1.1 2011/04/18 17:10:56 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        bcmx/wlan.h
 * Purpose:     BCMX Wireless LAN APIs
 */

#ifndef   _BCMX_WLAN_H_
#define   _BCMX_WLAN_H_

#ifdef INCLUDE_L3

#include <bcm/types.h>
#include <bcm/wlan.h>
#include <bcmx/bcmx.h>
#include <bcmx/multicast.h>
#include <bcmx/l3.h>
#include <bcmx/tunnel.h>

typedef bcm_wlan_port_t      bcmx_wlan_port_t;
typedef bcm_wlan_client_t    bcmx_wlan_client_t;


extern void bcmx_wlan_port_t_init(bcmx_wlan_port_t *wlan_port);
extern void bcmx_wlan_client_t_init(bcmx_wlan_client_t *wlan_client);

extern int bcmx_wlan_init(void);
extern int bcmx_wlan_detach(void);

extern int bcmx_wlan_port_add(bcmx_wlan_port_t *info);
extern int bcmx_wlan_port_delete(bcm_gport_t wlan_port_id);
extern int bcmx_wlan_port_delete_all(void);
extern int bcmx_wlan_port_get(bcm_gport_t wlan_port_id,
                              bcmx_wlan_port_t *info);

extern int bcmx_wlan_client_add(bcmx_wlan_client_t *info);
extern int bcmx_wlan_client_delete(bcm_mac_t mac);
extern int bcmx_wlan_client_delete_all(void);
extern int bcmx_wlan_client_get(bcm_mac_t mac, bcmx_wlan_client_t *info);

extern int bcmx_wlan_tunnel_initiator_create(bcmx_tunnel_initiator_t *info);
extern int bcmx_wlan_tunnel_initiator_destroy(bcm_gport_t wlan_tunnel_id);
extern int bcmx_wlan_tunnel_initiator_get(bcmx_tunnel_initiator_t *info);

#endif /* INCLUDE_L3 */

#endif /* _BCMX_WLAN_H_ */
