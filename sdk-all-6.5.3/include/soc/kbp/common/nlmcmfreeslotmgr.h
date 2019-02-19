/*
 * $Id: nlmcmfreeslotmgr.h,v 1.2.8.2 Broadcom SDK $
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


 
#ifndef INCLUDED_NLMFREESLOTMGR_H
#define INCLUDED_NLMFREESLOTMGR_H

#ifndef NLMPLATFORM_BCM
#include "nlmcmbasic.h"
#include "nlmcmallocator.h"
#include "nlmcmdbllinklist.h"
#include "nlmerrorcodes.h"
#else
#include <soc/kbp/common/nlmcmbasic.h>
#include <soc/kbp/common/nlmcmallocator.h>
#include <soc/kbp/common/nlmcmdbllinklist.h>
#include <soc/kbp/common/nlmerrorcodes.h>
#endif


typedef struct NlmFreeSlotMgr_t NlmFreeSlotMgr_t;

typedef struct NlmFreeSlotInfo_t NlmFreeSlotInfo_t;




struct NlmFreeSlotInfo_t
{
#ifndef NLMPLATFORM_BCM    
	#include "nlmcmdbllinklistdata.h"
#else
	#include <soc/kbp/common/nlmcmdbllinklistdata.h>
#endif
	NlmRecordIndex m_startIndex;
    NlmRecordIndex m_endIndex;
};


struct NlmFreeSlotMgr_t
{
    NlmFreeSlotInfo_t   *m_head_p;
    NlmCmAllocator  *m_alloc_p;
    NlmRecordIndex  m_indexIncr;
};


#ifndef NLMPLATFORM_BCM
#include "nlmcmexterncstart.h"
#else
#include <soc/kbp/common/nlmcmexterncstart.h>
#endif

/*******************************************************************************
 * Function : NlmFreeSlotInfo__destroyNode 
 *
 * Parameters:
 * void *alloc_p      = Pointer to general purpose memory allocator
 * NlmCmDblLinkList *node_p = Pointer to node in the linked list 
 *
 * Summary:
 * NlmFreeSlotInfo__destroyNode deletes a node from the linked list 
 ******************************************************************************/
void NlmFreeSlotInfo__destroyNode(
                NlmCmDblLinkList* node_p,  
                void* alloc_p);



/*******************************************************************************
 * Function : NlmFreeSlotMgr__Init 
 *
 * Parameters:
 * NlmCmAllocator *alloc_p   = Pointer to general purpose memory allocator
 * NlmRecordIndex startIndex = Start index for the free slot manager
 * NlmRecordIndex endIndex   = End index for the free slot manager
 * NlmRecordIndex indexIncr  = This actually indicates how many free slot
 *                             a record may occupy
 * NlmReasonCode *o_reason_p = Pointer to save the detailed reason code from
 *                             this function
 * Summary:
 * NlmFreeSlotMgr__Init creates and initializes a free slot manager object and
 * returns pointer to the newly created free slot manager object. 
 ******************************************************************************/

NlmFreeSlotMgr_t* NlmFreeSlotMgr__Init(
                NlmCmAllocator* alloc_p,
                NlmRecordIndex startIndex,
                NlmRecordIndex endIndex,
                NlmRecordIndex indexIncr,
                NlmReasonCode *o_reason_p);


/*******************************************************************************
 * Function : NlmFreeSlotMgr__FindNearestFreeSlot 
 *
 * Parameters:
 *
 * NlmFreeSlotMgr_t * self_p  = Pointer to free slot manager object 
 * NlmRecordIndex markerIndex = index from where the nearest slot to be seached
 * NlmBool isForwardDirection = nearest slot to be searched in which direction
 * NlmRecordIndex *o_bestFreeSlotIndex_p = this is to store the best free slot
 *                              index
 *
 * Summary:
 * NlmFreeSlotMgr__FindNearestFreeSlot looks for a free slot in the list of free
 * slots (maintained by the freeslotmgr)  which is nearest to the markerIndex 
 * passed. It returns NlmTrue if a free slot is found in the list, else
 * returns NlmFalse.
 ******************************************************************************/
NlmBool NlmFreeSlotMgr__FindNearestFreeSlot(
                                NlmFreeSlotMgr_t * self_p,
                                NlmRecordIndex markerIndex,
                                NlmBool isForwardDirection,
                                NlmRecordIndex *o_bestFreeSlotIndex_p);


/*******************************************************************************
 * Function : NlmFreeSlotMgr__RemoveFreeSlot 
 *
 * Parameters:
 *
 * NlmFreeSlotMgr_t * self_p  = Pointer to free slot manager object 
 * NlmRecordIndex freeSlotIndex = index of the free slot to be removed
 * NlmReasonCode *o_reason_p = Pointer to save the detailed reason code from
 *                             this function
 *
 * Summary:
 * NlmFreeSlotMgr__RemoveFreeSlot removes a free slot from the list of free
 * slots (maintained by the freeslotmgr)
 ******************************************************************************/

NlmErrNum_t NlmFreeSlotMgr__RemoveFreeSlot(NlmFreeSlotMgr_t * self_p,
                                        NlmRecordIndex markerIndex,
                                        NlmReasonCode *o_reason_p);



/*******************************************************************************
 * Function : NlmFreeSlotMgr__AddFreeSlot 
 *
 * Parameters:
 *
 * NlmFreeSlotMgr_t * self_p  = Pointer to free slot manager object 
 * NlmRecordIndex freeSlotIndex = index of the free slot to be added
 * NlmReasonCode *o_reason_p = Pointer to save the detailed reason code from
 *                             this function
 *
 * Summary:
 * NlmFreeSlotMgr__AddFreeSlot adds a free slot to the list of free
 * slots (maintained by the freeslotmgr)
 ******************************************************************************/

NlmErrNum_t  NlmFreeSlotMgr__AddFreeSlot(
                                NlmFreeSlotMgr_t * self_p,
                                NlmRecordIndex freeSlotIndex,
                                NlmReasonCode *o_reason_p);



/*******************************************************************************
 * Function : NlmFreeSlotMgr__Destroy 
 *
 * Parameters:
 *
 * NlmFreeSlotMgr_t * self_p  = Pointer to free slot manager object 
 *
 * Summary:
 * NlmFreeSlotMgr__Destroy frees the memory associated with the free slot
 * manager object and also freeu up the linked list maintained by free slot 
 * manager.
 ******************************************************************************/

void NlmFreeSlotMgr__Destroy(
                    NlmFreeSlotMgr_t * self_p);


NlmBool NlmFreeSlotMgr__isFreeSlot(NlmFreeSlotMgr_t * self_p,
                                NlmRecordIndex markerIndex);

#ifndef NLMPLATFORM_BCM
#include "nlmcmexterncend.h"
#else
#include <soc/kbp/common/nlmcmexterncend.h>
#endif

#endif




