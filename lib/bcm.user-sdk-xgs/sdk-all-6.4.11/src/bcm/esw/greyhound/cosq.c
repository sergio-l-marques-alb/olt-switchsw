/*
 * $Id: $
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
 */

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_GREYHOUND_SUPPORT)
#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/greyhound.h>
#include <bcm_int/esw_dispatch.h>

int
bcm_gh_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t port;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_gh_cosq_control_set: unit=%d gport=0x%x cosq=%d \
                         type=%d arg=%d\n"),
              unit, gport, cosq, type, arg));
    switch (type) {
    case bcmCosqControlPFCBackpressureEnable:
        if (soc_feature(unit, soc_feature_priority_flow_control) &&
            soc_feature(unit, soc_feature_gh_style_pfc_config)) {
            uint32 rval;
            uint32 fval;
            if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(READ_MMU_FC_RX_ENr(unit, port, &rval));
            fval = soc_reg_field_get(unit, MMU_FC_RX_ENr, 
                                     rval, MMU_FC_RX_ENABLEf);
            if (arg) {
                fval |= (1 << cosq);
            } else {
                fval &= ~(1 << cosq);
            }
            soc_reg_field_set(unit, MMU_FC_RX_ENr, 
                              &rval, MMU_FC_RX_ENABLEf, fval);
            BCM_IF_ERROR_RETURN(WRITE_MMU_FC_RX_ENr(unit, port, rval));
            return BCM_E_NONE;
        }
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_gh_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t port;
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_gh_cosq_control_get: unit=%d gport=0x%x cosq=%d type=%d\n"),
              unit, gport, cosq, type));
    switch (type) {
    case bcmCosqControlPFCBackpressureEnable:
        if (soc_feature(unit, soc_feature_priority_flow_control) &&
            soc_feature(unit, soc_feature_gh_style_pfc_config)) {
            uint32 value;
            if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(READ_MMU_FC_RX_ENr(unit, port, &value));
            if (value & (1 << cosq)) {
                *arg = 1;
            } else {
                *arg = 0;
            }
            return BCM_E_NONE;
        }
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

#endif /* BCM_GREYHOUND_SUPPORT */

