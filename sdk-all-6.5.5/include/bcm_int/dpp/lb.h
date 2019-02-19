/*
 * $Id: lb.h,v 1.55 Broadcom SDK $
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
 * File:        lb.h
 * Purpose:     LB internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_LB_H_
#define   _BCM_INT_DPP_LB_H_

#include <soc/dpp/TMC/tmc_api_link_bonding.h>

#define BCM_LB_PORT_INVALID          -1
#define BCM_LB_LBG_INVALID           0xFFFFFFFF

#define BCM_LB_VALUE_MAX_CHECK(val, max, para_name) {\
    if ((val) > (max)) { \
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, \
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) should be less than %d\n"), \
                             soc_errmsg(BCM_E_PARAM), para_name, val, (max+1)));\
    } \
}

#define BCM_LB_VALUE_CHECK(val, min, max, para_name) \
    if ((val) < (min) || (val) > (max)) { \
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, \
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) should be in range of [%d, %d]\n"), \
                             soc_errmsg(BCM_E_PARAM), para_name, val, min, max));\
    }

#define LB_INFO_ACCESS               sw_state_access[unit].dpp.bcm.linkbonding

typedef struct bcm_dpp_lb_info_s {
    SHR_BITDCL   *lbg_valid_bmp;
    int           modem_to_ports[SOC_TMC_LB_NOF_MODEM];
    uint32        port_to_lbg[SOC_MAX_NUM_PORTS];
    uint32        modem_to_ing_lbg[SOC_TMC_LB_NOF_MODEM];
    uint32        modem_to_egr_lbg[SOC_TMC_LB_NOF_MODEM];
} bcm_dpp_lb_info_t;

/* sw_state_access[unit].dpp.bcm.lb.cosq_res_info.queue_unicast_total_dynamic_res.bit_set */

extern int bcm_petra_lb_init(int unit);
extern int bcm_petra_lb_detach(int unit);

#endif /* _BCM_INT_DPP_LB_H_ */

