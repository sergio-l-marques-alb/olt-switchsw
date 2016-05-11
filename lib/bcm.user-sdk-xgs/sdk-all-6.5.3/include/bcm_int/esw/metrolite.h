/*
 * $Id: metrolite.h,v 1.34 Broadcom SDK $ 
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
 * File:        metrolite.h
 * Purpose:     Function declarations for metrolite bcm functions
 */

#ifndef _BCM_INT_METROLITE_H_
#define _BCM_INT_METROLITE_H_
#if defined(BCM_METROLITE_SUPPORT)
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm/qos.h>
#include <bcm/failover.h>
#include <bcm/sat.h>
#if defined(INCLUDE_L3)

#endif
extern int _bcm_ml_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_ml_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_ml_port_lanes_get(int unit, bcm_port_t port, int *value);

extern int _bcm_ml_port_sw_info_display(int unit, bcm_port_t port);
extern int _bcm_ml_port_hw_info_display(int unit, bcm_port_t port);

extern int bcm_ml_oam_opcodes_count_profile_create( int unit, uint8 *lm_count_profile);

extern int bcm_ml_oam_opcodes_count_profile_set(
        int unit, 
        uint8 lm_count_profile, 
        uint8 count_enable, 
        bcm_oam_opcodes_t *opcodes_bitmap); 

extern int bcm_ml_oam_opcodes_count_profile_get(
        int unit, 
        uint8 lm_count_profile, 
        uint8 count_enable, 
        bcm_oam_opcodes_t *opcodes_bitmap); 

extern int bcm_ml_oam_opcodes_count_profile_delete(
        int unit, 
        uint8 lm_count_profile);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP /* BCM_WARM_BOOT_SUPPORT_SW_DUMP*/

#endif /* !BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_WARM_BOOT_SUPPORT /* BCM_WARM_BOOT_SUPPORT */

#endif/* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_METROLITE_SUPPORT */
#endif  /* !_BCM_INT_METROLITE_H_ */

