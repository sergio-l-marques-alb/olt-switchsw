/*
 * $Id: $
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
 */

 #ifdef SW_AUTONEG_SUPPORT 


 #ifndef __BCM_SW_AN_H__
 #define __BCM_SW_AN_H__

#include <phymod/phymod.h>
#include <bcm/types.h>

 typedef enum bcm_port_sw_an_event_e {
 	BCM_PORT_SW_AN_EVENT_NONE = 0,
 	BCM_PORT_SW_AN_EVENT_AN_START,
 	BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS,
 	BCM_PORT_SW_AN_EVENT_AN_RESTART,
 	BCM_PORT_SW_AN_EVENT_AN_DOWN,
 	BCM_PORT_SW_AN_EVENT_AN_STOP,
 	BCM_PORT_SW_AN_EVENT_AN_MAX
 } bcm_port_sw_an_event_t;

 typedef enum bcm_sw_an_fsm_event_e {
    BCM_FSM_SW_AN_EVT_NONE = 0,
    BCM_FSM_SW_AN_EVT_WAIT, /* wait for event */
    BCM_FSM_SW_AN_EVT_START, /* SW state to kick off the state machine */
    BCM_FSM_SW_AN_EVT_SEQ_START, /* HW event to restart AN from base page exchange */
    BCM_FSM_SW_AN_EVT_LD_PAGE,  /* HW req to load the new page */
    BCM_FSM_SW_AN_EVT_LP_PAGE_RDY, /* HW event indicating LP page rcvd */
    BCM_FSM_SW_AN_EVT_PAGE_EX_COMPLETE,
    BCM_FSM_SW_AN_EVT_SEND_ACK,
    BCM_FSM_SW_AN_EVT_MAX
} bcm_sw_an_fsm_event_t;

#define SW_AN_MODE_CL73_MSA     0x3
#define SW_AN_MODE_MSA_ONLY     0x4


/* initialize the SW AN module */
extern int bcm_sw_an_module_init(int unit);

/* deInit the SW AN module */
extern int bcm_sw_an_module_deinit(int unit);

/* Start/Stop the SW AN thread for the unit */
extern int bcm_sw_an_enable_set(int unit, int enable); 

extern int bcm_sw_an_enable_get(int unit, int *enable);

/* add the port to SW AN */
extern int bcm_sw_an_port_register(int unit, bcm_port_t port);

/* remove the port from SW AN */
extern int bcm_sw_an_port_unregister(int unit, bcm_port_t port);

/* notify the SW AN thread of any AN event */
extern int bcm_sw_an_post_event(int unit, bcm_port_t port, bcm_port_sw_an_event_t event);

/* sw an advert set */
extern int bcm_sw_an_advert_set(int unit, bcm_port_t port, phymod_autoneg_ability_t *ability);

/* debug function to dump an context for the port */
int bcm_sw_an_port_diag(int unit, int port);

#endif /* __BCM_SW_AN_H__ */

#endif /* SW_AUTONEG_SUPPORT */

