/*
 * $Id: mim.h,v 1.14 Broadcom SDK $
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
 * Soc_petra-B Layer 2 Management
 */

#ifndef   _BCM_INT_DPP_MIM_H_
#define   _BCM_INT_DPP_MIM_H_

#include <bcm_int/dpp/error.h>

#include <bcm/mim.h>
#include <bcm/types.h>

#include <soc/dpp/drv.h>


/* 
 * Defines
 */

#define BCM_PETRA_MIM_VPN_INVALID   (0xffff)

#define BCM_PETRA_MIM_BTAG_TPID    (0x81a8)
#define BCM_PETRA_MIM_ITAG_TPID    (0x88e7)

#define BCM_PETRA_MIM_BVID_MC_GROUP_BASE    (12*1024)

#define MIM_ACCESS sw_state_access[unit].dpp.bcm.mim


/*
 * MiM Module Helper functions
 */

uint8
  __dpp_mim_initialized_get(int unit);

SOC_PPD_LIF_ID
  __dpp_mim_lif_ndx_get(int unit);

SOC_PPD_AC_ID
  __dpp_mim_global_out_ac_get(int unit);


int dpp_mim_set_global_mim_tpid(int unit);

int
_bcm_dpp_in_lif_mim_match_get(int unit, bcm_mim_port_t *mim_port, int lif);

/* 
 * Macros
 */

#define MIM_INIT(unit)                                    \
    do {                                                  \
        if (!__dpp_mim_initialized_get(unit)) {           \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

#define MIM_IS_INIT(unit)   (__dpp_mim_initialized_get(unit))


typedef struct bcm_dpp_mim_info_s {
    SOC_PPD_LIF_ID    mim_local_lif_ndx; /* default local lif, used only for MiM */
    SOC_PPD_AC_ID     mim_local_out_ac; /* default out-ac, used only for MiM */
} bcm_dpp_mim_info_t;


#endif /* _BCM_INT_DPP_MIM_H_ */
     
