/*
 * $Id: nlmdevmgrmapping.c,v 1.1.6.2 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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




#include "nlmarch.h"
#include "nlmdevmgr.h"

#ifdef NLM_12K_11K

#include<nlmdevmgr/nlmdevmgr_shadow.h>
#include<nlmdevmgr/nlmdevmgr.h>

 Nlm11kDevMgr* kbp_dm_pvt_11k_init(
    NlmCmAllocator      *alloc_p,
    void                    *xpt_p,
    NlmDevType          devType,
    NlmDevOperationMode  operMode,
    NlmReasonCode       *o_reason
    )
{
    Nlm11kDevMgr* devMgr_p = NULL;

    (void)devType;

    devMgr_p = Nlm11kDevMgr__create(alloc_p, xpt_p, operMode, o_reason);

    return devMgr_p;
}


NlmErrNum_t
kbp_dm_pvt_11k_map_ltrs(
    NlmDevLtrRegType regType,
    Nlm11kDevLtrRegType *o_11kRegType,
    NlmReasonCode       *o_reason
    )
{
    NlmErrNum_t errNum = NLMERR_OK;

    switch(regType)
    {
        case NLMDEV_BLOCK_SELECT_0_LTR:
        case NLMDEV_BLOCK_SELECT_1_LTR:
            *o_11kRegType = regType;
            break;

        case NLMDEV_BLOCK_SELECT_2_LTR:
        case NLMDEV_BLOCK_SELECT_3_LTR:
            errNum = NLMERR_FAIL;
            break;

        case NLMDEV_PARALLEL_SEARCH_0_LTR:
        case NLMDEV_PARALLEL_SEARCH_1_LTR:
        case NLMDEV_PARALLEL_SEARCH_2_LTR:
        case NLMDEV_PARALLEL_SEARCH_3_LTR:
            *o_11kRegType =  NLM11KDEV_PARALLEL_SEARCH_0_LTR +
                                    (regType -NLMDEV_PARALLEL_SEARCH_0_LTR);
            break;

        case NLMDEV_PARALLEL_SEARCH_4_LTR:
        case NLMDEV_PARALLEL_SEARCH_5_LTR:
        case NLMDEV_PARALLEL_SEARCH_6_LTR:
        case NLMDEV_PARALLEL_SEARCH_7_LTR:
            errNum = NLMERR_FAIL;
            break;

        case NLMDEV_SUPER_BLK_KEY_MAP_LTR:
            *o_11kRegType = NLM11KDEV_SUPER_BLK_KEY_MAP_LTR;
            break;

        case NLMDEV_EXT_CAPABILITY_REG_0_LTR:
            *o_11kRegType = NLM11KDEV_MISCELLENEOUS_LTR;
            break;
        case NLMDEV_EXT_CAPABILITY_REG_1_LTR:
            errNum = NLMERR_FAIL;
            break;

        case NLMDEV_KEY_0_KCR_0_LTR:
        case NLMDEV_KEY_0_KCR_1_LTR:
        case NLMDEV_KEY_1_KCR_0_LTR:
        case NLMDEV_KEY_1_KCR_1_LTR:
        case NLMDEV_KEY_2_KCR_0_LTR:
        case NLMDEV_KEY_2_KCR_1_LTR:
        case NLMDEV_KEY_3_KCR_0_LTR:
        case NLMDEV_KEY_3_KCR_1_LTR:
            *o_11kRegType =  NLM11KDEV_KEY_0_KCR_0_LTR +
                                    (regType -NLMDEV_KEY_0_KCR_0_LTR);
            break;

        case NLMDEV_OPCODE_EXT_LTR:
            errNum = NLMERR_FAIL;
            break;

        case NLMDEV_RANGE_INSERTION_0_LTR:
        case NLMDEV_RANGE_INSERTION_1_LTR:
            *o_11kRegType =  NLM11KDEV_RANGE_INSERTION_0_LTR +
                                    (regType -NLMDEV_RANGE_INSERTION_0_LTR);
            break;

        case NLMDEV_SS_LTR:
            *o_11kRegType =  NLM11KDEV_SS_LTR;
            break;

        case NLMDEV_LTR_REG_END:
        default:
            errNum = NLMERR_FAIL;
            break;
    }

    if(errNum == NLMERR_FAIL)
        *o_reason = NLMRSC_INVALID_REG_ADDRESS;

    return errNum;
}


NlmErrNum_t
kbp_dm_pvt_11k_map_global_reg(
    NlmDevGlobalRegType regType,
    Nlm11kDevGlobalRegType *o_11kRegType,
    NlmReasonCode       *o_reason
    )
{
    NlmErrNum_t errNum = NLMERR_OK;

    switch(regType)
    {
        case NLMDEV_DEVICE_ID_REG:  /* Read only Reg */
        case NLMDEV_DEVICE_CONFIG_REG:
        case NLMDEV_ERROR_STATUS_REG: /* Read only Reg; Reading of this register clears the set bits */
        case NLMDEV_ERROR_STATUS_MASK_REG:
            *o_11kRegType = regType;
            break;

        case NLMDEV_SOFT_SCAN_WRITE_REG:
            errNum = NLMERR_FAIL;
            break;

        case NLMDEV_SOFT_ERROR_FIFO_REG:
        case NLMDEV_ADV_FEATURE_SOFT_ERROR_REG:
            *o_11kRegType = NLM11KDEV_DATABASE_SOFT_ERROR_FIFO_REG +
                        (regType - NLMDEV_SOFT_ERROR_FIFO_REG) ;
            break;

        case NLMDEV_LPT_ENABLE_REG:
            errNum = NLMERR_FAIL;
            break;

        case NLMDEV_SCRATCH_PAD0_REG:
        case NLMDEV_SCRATCH_PAD1_REG:
        case NLMDEV_RESULT0_REG:
        case NLMDEV_RESULT1_REG:
            *o_11kRegType = NLM11KDEV_SCRATCH_PAD0_REG +
                        (regType - NLMDEV_SCRATCH_PAD0_REG) ;
            break;

        case NLMDEV_RESULT2_REG:
        case NLMDEV_UDA_SOFT_ERROR_COUNT_REG:
        case NLMDEV_UDA_SOFT_ERROR_FIFO_REG:
        case NLMDEV_UDA_CONFIG_REG:
        case NLMDEV_GLOBALREG_END:
        default:
            errNum = NLMERR_FAIL;
            break;
    }

    if(errNum == NLMERR_FAIL)
        *o_reason = NLMRSC_INVALID_REG_ADDRESS;

    return errNum;

}


NlmErrNum_t
kbp_dm_pvt_11k_map_cmp_result(
    Nlm11kDevCmpRslt *cmpResult11k,
    NlmDevCmpResult  *o_cmpResult12k,
    NlmReasonCode       *o_reason
    )
{
    nlm_u32 i = 0;
    *o_reason = NLMRSC_REASON_OK;

    for(i = 0; i < NLM_MAX_NUM_RESULTS; i++)
    {
        o_cmpResult12k->m_resultValid[i] = NLMDEV_RESULT_VALID;
        o_cmpResult12k->m_respType[i] = NLMDEV_INDEX_AND_NO_AD;

        o_cmpResult12k->m_hitOrMiss[i] = cmpResult11k->m_hitOrMiss[i];
        o_cmpResult12k->m_hitIndex[i] = cmpResult11k->m_hitIndex[i];
        o_cmpResult12k->m_hitDevId[i] = cmpResult11k->m_hitDevId[i];
    }

    return NLMERR_OK;

}

void*
kbp_dm_pvt_11k_map_create_shadow_st(
    NlmCmAllocator *alloc_p)
{
    /* Create memory for ST. */

    Nlm11kDevShadowST *shadowSt_p = NULL;

    shadowSt_p = (Nlm11kDevShadowST*) NlmCmAllocator__calloc(alloc_p,
                                                    NLM11KDEV_SS_NUM,
                                                    sizeof(Nlm11kDevShadowST) );

    return (void*)shadowSt_p;
}

#endif
