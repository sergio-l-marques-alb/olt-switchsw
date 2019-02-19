/*
 * $Id: rx_los.h,v 1.9 Broadcom SDK $
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
 * RX LOS
 */


#ifndef _RX_LOS_H_INCLUDED_
#define _RX_LOS_H_INCLUDED_

#include <soc/types.h>
#include <bcm/port.h>

/* 
 *RX LOS application - default values
 */
#define RX_LOS_THREAD_PRIORITY_DEFAULT              (1)
#define RX_LOS_SHORT_SLEEP_DEFAULT                  (500000)
#define RX_LOS_LONG_SLEEP_DEFAULT                   (5000000)
#define RX_LOS_ALLOWED_RETRIES_DEFAULT              (5)
#define RX_LOS_LINK_DOWN_CYCLE_COUNT_DEFAULT        (1)
#define RX_LOS_LINK_DOWN_CYCLE_TIME_DEFAULT         (0)
#define RX_LOS_RX_SEQ_CHANGE_CYCLE_COUNT_DEFAULT    (5)
#define RX_LOS_RX_SEQ_CHANGE_CYCLE_TIME_DEFAULT     (5000000)
#define RX_LOS_PBMP_DEFAULT(unit, pbmp) {               \
        SOC_PBMP_CLEAR(pbmp);                            \
        if (SOC_IS_DFE(unit)) {                         \
            SOC_PBMP_OR(pbmp, PBMP_SFI_ALL(unit));      \
        } else if (SOC_IS_ARAD(unit)) {                 \
            SOC_PBMP_OR(pbmp, PBMP_SFI_ALL(unit));      \
            SOC_PBMP_OR(pbmp, PBMP_IL_ALL(unit));       \
            SOC_PBMP_OR(pbmp, PBMP_CE_ALL(unit));       \
            SOC_PBMP_OR(pbmp, PBMP_XL_ALL(unit));       \
            SOC_PBMP_OR(pbmp, PBMP_XE_ALL(unit));       \
            SOC_PBMP_OR(pbmp, PBMP_GE_ALL(unit));       \
        }                                               \
    }

/*
 * RX LOS application - defines
 */
/*rx_los_generic_unit_detach flags*/
#define RX_LOS_GENERIC_UNIT_DETACH_F_SW_ONLY        (0x1)

/*rx_los_generic_port_enable flags*/
#define RX_LOS_GENERIC_PORT_ENABLE_F_SW_ONLY        (0x1)

/*
 * RX LOS port states
 */
typedef enum rx_los_state_e {
    rx_los_state_ideal_state = 0,
    rx_los_state_no_signal,
    rx_los_state_no_signal_active, /*no signal state with polling over rx_seq_done*/
    rx_los_state_rx_seq_change,
    rx_los_state_sleep_one,
    rx_los_state_restart,
    rx_los_state_sleep_two,
    rx_los_state_link_up_check,
    rx_los_state_long_sleep,
    rx_los_states_count
} rx_los_state_t;

/*
 * Functions
 */
int rx_los_set_config(uint32 short_sleep_usec, uint32 long_sleep_usec, uint32 allowed_retries, uint32 priority, int link_down_count_max, int link_down_cycle_time);
int rx_los_mac_rxlostofsync_interrupt_set_config(int unit, int interrupt_occurences, int cycle_time);
int rx_los_port_enable(int unit, bcm_port_t port, int enable, int warm_boot);
int rx_los_generic_port_enable(int unit, bcm_port_t port, int enable, int warm_boot, uint32 flags);
int rx_los_unit_attach(int unit, const pbmp_t pbmp, int warm_boot);
int rx_los_enable_get(int unit, bcm_pbmp_t *pbmp, int *enable);
int rx_los_unit_detach(int unit);
int rx_los_sw_unit_detach(int unit);
int rx_los_generic_unit_detach(int unit, uint32 flags);
int rx_los_handle(int unit, int port);
int rx_los_exit_thread(void);
int rx_loss_appl_enable(int unit);
int rx_los_dump(int unit);
int rx_los_dump_port(int unit, bcm_port_t port);

/* 
 * RX LOS callback register 
 *  
 * RX LOS application will call the callback when a port move to a stable or active stable state: 
 *  * rx_los_state_ideal_state
 *  * rx_los_state_no_signal
 *  * rx_los_state_no_signal_active 
 */

typedef void (*rx_los_callback_t) (int unit, bcm_port_t port, rx_los_state_t state);
int rx_los_register(int unit, rx_los_callback_t callback);

/*
 * RX LOS - wait for stable port
 * RX LOS api will when a port is in a stable or active stable state: 
 *  * rx_los_state_ideal_state
 *  * rx_los_state_no_signal
 *  * rx_los_state_no_signal_active
 *  
 *  If port is not supported by RX LOS the API will return state=rx_los_states_count.
 *  max time to wait: timeout
 */
int rx_los_port_stable(int unit, bcm_port_t port, int timeout_usec, rx_los_state_t *state);

#endif /*_RX_LOS_H_INCLUDED_*/
