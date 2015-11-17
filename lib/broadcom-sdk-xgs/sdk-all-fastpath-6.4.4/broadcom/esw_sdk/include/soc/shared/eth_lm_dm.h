/*
 * $Id: eth_lm_dm.h,v 1.0 Broadcom SDK $
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
 * File:    eth_lm_dm.h
 * Purpose: ETH_LM_DM definitions common to SDK and uKernel
 *
 */

#ifndef _SOC_SHARED_ETH_LM_DM_H
#define _SOC_SHARED_ETH_LM_DM_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif



/*****************************************
 * ETH_LM_DM uController Error codes
 */
typedef enum shr_eth_lm_dm_uc_error_e {
    SHR_ETH_LM_DM_UC_E_NONE = 0,
    SHR_ETH_LM_DM_UC_E_INTERNAL,
    SHR_ETH_LM_DM_UC_E_MEMORY,
    SHR_ETH_LM_DM_UC_E_PARAM,
    SHR_ETH_LM_DM_UC_E_RESOURCE,
    SHR_ETH_LM_DM_UC_E_EXISTS,
    SHR_ETH_LM_DM_UC_E_NOT_FOUND,
    SHR_ETH_LM_DM_UC_E_UNAVAIL,
    SHR_ETH_LM_DM_UC_E_VERSION,
    SHR_ETH_LM_DM_UC_E_INIT
} shr_eth_lm_dm_uc_error_t;

#define SHR_ETH_LM_DM_UC_SUCCESS(rv)              ((rv) == SHR_ETH_LM_DM_UC_E_NONE)
#define SHR_ETH_LM_DM_UC_FAILURE(rv)              ((rv) != SHR_ETH_LM_DM_UC_E_NONE)

/*
 * Macro:
 *      SHR_ETH_LM_DM_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_ETH_LM_DM_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_ETH_LM_DM_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

/*
 * ETH_LM_DM Session Set flags
 */
#define SHR_ETH_LM_DM_SESS_SET_F_CREATE               0x00000001
#define SHR_ETH_LM_DM_SESS_SET_F_PERIOD               0x00000002
#define SHR_ETH_LM_DM_SESS_SET_F_LM                   0x00000004
#define SHR_ETH_LM_DM_SESS_SET_F_DM                   0x00000008
#define SHR_ETH_LM_DM_SESS_SET_F_PASSIVE              0x00000010
#define SHR_ETH_LM_DM_SESS_SET_F_SLM                  0x00000020


/*
 * ETH_LM_DM Encapsulation types
 */
#define SHR_ETH_LM_DM_ENCAP_TYPE_RAW      0

/*
 * Op Codes
 */

#define SHR_ETH_LM_DM_OPCODE_LMM 43
#define SHR_ETH_LM_DM_OPCODE_LMR 42
#define SHR_ETH_LM_DM_OPCODE_1DM 45
#define SHR_ETH_LM_DM_OPCODE_DMM 47
#define SHR_ETH_LM_DM_OPCODE_DMR 46
#define SHR_ETH_LM_DM_OPCODE_SLM 55
#define SHR_ETH_LM_DM_OPCODE_SLR 54
#define SHR_ETH_LM_DM_OPCODE_CFS XX

/*
 * Flags
 */
#define BCM_ETH_INC_REQUESTING_MEP_TLV         0x00000001
#define BCM_ETH_LM_SLM			               0x00000002
#define BCM_ETH_LM_SINGLE_ENDED                0x00000004
#define BCM_ETH_DM_ONE_WAY                     0x00000008
#define BCM_ETH_LM_TX_ENABLE                   0x00000010
#define BCM_ETH_DM_TX_ENABLE                   0x00000020
#define BCM_ETH_LM_UPDATE					   0x00000040
#define BCM_ETH_DM_UPDATE					   0x00000080



 /*
  * Replacement type in OLP Tx header
  */
/* OAM Replacement types */
#define BCM_ETH_LM_DM_RPL_TYPE_NONE        0 
#define BCM_ETH_LM_DM_RPL_TYPE_PTP_DM      1 
#define BCM_ETH_LM_DM_RPL_TYPE_NTP_DM      2 
#define BCM_ETH_LM_DM_RPL_TYPE_LM          3 


/*
 * Macro:
 *      SHR_ETH_LM_DM_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_ETH_LM_DM_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_ETH_LM_DM_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

#endif /* _SOC_SHARED_ETH_LM_DM_H */
