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
   

#ifndef _SOC_ROBO_ARL_H
#define _SOC_ROBO_ARL_H

#include <shared/bsl.h>

#include <soc/arl.h>

#define DRV_ARL_WARN(stuff)         LOG_WARN(BSL_LS_SOC_ARL, stuff)
#define DRV_ARL_ERR(stuff)          LOG_ERROR(BSL_LS_SOC_ARL, stuff)
#define DRV_ARL_VERB(stuff)         LOG_VERBOSE(BSL_LS_SOC_ARL, stuff)
#define DRV_ARL_VVERB(stuff)        LOG_DEBUG(BSL_LS_SOC_ARL, stuff)


#define _ROBO_SEARCH_LOCK (1 << 0)
#define _ROBO_SCAN_LOCK (1 << 1)


#define TBX_ARL_SCAN_LOCK(unit, soc)\
    do{\
        MEM_RWCTRL_REG_LOCK(soc);\
        soc->arl_exit &= ~(_ROBO_SCAN_LOCK);\
        DRV_ARL_VVERB(("%s %d MEM_RWCTRL_REG_LOCK\n",FUNCTION_NAME(),__LINE__));\
    }while(0)


#define TBX_ARL_SCAN_UNLOCK(unit, soc)\
    do{\
        DRV_ARL_VVERB(("%s %d MEM_RWCTRL_REG_UNLOCK\n",FUNCTION_NAME(),__LINE__));\
        soc->arl_exit |= _ROBO_SCAN_LOCK;\
        MEM_RWCTRL_REG_UNLOCK(soc);\
    }while(0)


#define VO_ARL_SEARCH_LOCK(unit, soc) \
    do{\
        if(SOC_IS_VO(unit)) {\
            ARL_MEM_SEARCH_LOCK(soc);\
            soc->arl_exit &= ~(_ROBO_SEARCH_LOCK);\
            DRV_ARL_VVERB(("%s %d ARL_MEM_SEARCH_LOCK--\n",FUNCTION_NAME(),__LINE__));\
        }\
    }while(0)


#define VO_ARL_SEARCH_UNLOCK(unit, soc) \
do{\
    if(SOC_IS_VO(unit)) {\
        DRV_ARL_VVERB(("%s %d ARL_MEM_SEARCH_UNLOCK\n",FUNCTION_NAME(),__LINE__));\
        soc->arl_exit |= _ROBO_SEARCH_LOCK;\
        ARL_MEM_SEARCH_UNLOCK(soc);\
    }\
}while(0)

#endif /* _SOC_ROBO_ARL_H */
