/*
 * $Id: appl_dcmn_intr.h, v1 Broadcom SDK $
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

#ifndef _DCMN_INTR_H_
#define _DCMN_INTR_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/defs.h>
#include <soc/types.h>
#include <bcm/error.h>

/*************
 * TYPE DEFS *
 *************/

/*************
 * STRUCTURES *
 *************/

/*
 * static data strucure which holds the info about: 
 *             the function suppose to run for each interrupt
 *              info about storm threshold
 */
typedef struct interrupt_common_params_s {
    uint32* int_disable_on_init;
    uint32* int_active_on_init;
    uint32* int_disable_print_on_init;
    soc_mem_t* cached_mem;
} intr_common_params_t;

/*************
 * FUNCTIONS *
 *************/
int interrupt_appl_init(int unit);
int interrupt_appl_deinit(int unit);

#ifdef BCM_QAX_SUPPORT
int qax_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

#ifdef BCM_JERICHO_SUPPORT
int jer_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif
#ifdef BCM_DFE_SUPPORT
int fe3200_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

uint32 *interrupt_active_on_intr_get(int unit);

#endif /*_DCMN_INTR_H_ */
