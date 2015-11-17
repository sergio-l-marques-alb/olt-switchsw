/*
 * $Id: vswitch.h,v 1.5 Broadcom SDK $
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
 * File:        vswitch.h
 * Purpose:     Vswitch internal definitions
 *              specific to FE2000 BCM API implementation
 */

#ifndef _BCM_INT_SBX_CALADAN3_VSWITCH_H_
#define _BCM_INT_SBX_CALADAN3_VSWITCH_H_

#include <bcm/port.h>
#include <bcm/vlan.h>

/*
 *  This structure describes a single gport that is a member of a VSI.
 */
typedef struct _bcm_caladan3_vswitch_port_s {
    bcm_gport_t gport;                       /* GPort ID */
    struct _bcm_caladan3_vswitch_port_s *next; /* next entry this list */
    struct _bcm_caladan3_vswitch_port_s *prev; /* prev entry this list */
} _bcm_caladan3_vswitch_port_t;

/*
 *  This structure contains information about a given unit's vswitch state.
 *  Since we must have a list of ports that are included in a VSI (due to the
 *  requirement for delete_all), this also includes pointers to such lists.
 *  We also need to include lists for ALL VSIs since we can add GPORTs to
 *  various other special cases, such as traditional bridging.
 */
typedef struct _bcm_caladan3_vswitch_state_s {
    sal_mutex_t lock;
    _bcm_caladan3_vswitch_port_t **portList;
} _bcm_caladan3_vswitch_state_t;



int _bcm_caladan3_vswitch_port_info_get(int unit, bcm_port_t port, 
                                      bcm_vlan_t vsi, int *keepUntagged);

int _bcm_caladan3_vswitch_port_gport_get(int unit, bcm_port_t port,
                                       bcm_vlan_t vsi, bcm_gport_t *gport);
#endif
