/*
 * $Id: nlmcmmt.c,v 1.2.8.1 Broadcom SDK $
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


 
#if defined NLM_MT_OLD || defined NLM_MT

#include "nlmcmmt.h"

#if defined  NLM_POSIX || defined NLMPLATFORM_BCM
__thread nlm_u32 threadID;
#endif

#if defined NLM_NETOS || defined NLMPLATFORM_BCM
NlmBool isNlmCmMtInitDone = NlmFalse;
nlm_u32 threadIdArray[NLMNS_MT_MAXNUM_THREADS ] = {-1, };
#endif

NlmErrNum_t
NlmCmMt__RegisterThread(
        nlm_u32 thNum,
        nlm_u32 cpuId,
        NlmReasonCode* o_reason)
{
    NlmReasonCode dummy;

    if(o_reason == NULL)
        o_reason = &dummy;

    if(thNum > NLMNS_MT_MAXNUM_THREADS)
    {
        *o_reason = NLMRSC_MT_INVALID_THREAD_ID;
        return NLMERR_FAIL;
    }

#if defined NLM_NETOS || defined NLMPLATFORM_BCM

    /* Initialize the thread id array if not done */
    if(isNlmCmMtInitDone == NlmFalse)
    {
        nlm_u32 i = 0;
        for(i = 0; i < NLMNS_MT_MAXNUM_THREADS ; i++)
            threadIdArray[i] = -1;
        isNlmCmMtInitDone = NlmTrue;
    }
    
    if(threadIdArray[thNum] != -1)
    {
        *o_reason = NLMRSC_MT_THREAD_ALREADY_REGISTERED;
        return NLMERR_FAIL;
    }

    threadIdArray[thNum] = cpuId;
#else
    threadID = thNum;
#endif

    *o_reason = NLMRSC_REASON_OK;
    return NLMERR_OK;
    
}


#endif /* defined NLM_MT_OLD || defined NLM_MT */


