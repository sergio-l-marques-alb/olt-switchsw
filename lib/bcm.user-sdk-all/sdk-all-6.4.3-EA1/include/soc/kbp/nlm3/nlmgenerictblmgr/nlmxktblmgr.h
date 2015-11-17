/*
 * $Id: nlmxktblmgr.h,v 1.1.6.5 Broadcom SDK $
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
 */
 
#ifndef INCLUDED_NLMXKTBLMGR_H
#define INCLUDED_NLMXKTBLMGR_H

#ifndef NLMPLATFORM_BCM
#include "nlmcmbasic.h"
#include "nlmcmallocator.h"
#include "nlmerrorcodes.h"
#include "nlmcmexterncstart.h"
#else
#include <soc/kbp/common/nlmcmbasic.h>
#include <soc/kbp/common/nlmcmallocator.h>
#include <soc/kbp/common/nlmerrorcodes.h>
#include <soc/kbp/common/nlmcmexterncstart.h>
#endif

/* function pointers for virtual table */
typedef  NlmErrNum_t (*NlmXkTblMgr__CreateTable_t)(
                NlmGenericTblMgr  *genericTblMgr_p,
                NlmPortNum          portNum,
                NlmGenericTbl* table_p,
                NlmReasonCode    *o_reason
                );



    

typedef  NlmErrNum_t (*NlmXkTblMgr__ConfigSearch_t)(
        NlmGenericTblMgr  *genericTblMgr,
        NlmPortNum          portNum,
        nlm_u8             ltrNum, 
        NlmGenericTblSearchAttributes  *searchAttrs,
        NlmReasonCode    *o_reason
        );


typedef  NlmErrNum_t (*NlmXkTblMgr__LockConfiguration)(
            NlmGenericTblMgr  *genericTblMgr,
            NlmPortNum          portNum,
            NlmReasonCode     *o_reason
        );

typedef  NlmErrNum_t (*NlmXkTblMgr__AddRecord_t)(
    NlmGenericTbl       *genericTbl,
    NlmPortNum          portNum,
    NlmGenericTblRecord *tblRecord,
    nlm_u8          *assocData,
    nlm_u16              recordPriority, 
    NlmRecordIndex      *o_recordIndex,
    nlm_u16             GroupId,
    NlmReasonCode       *o_reason
    );

typedef  NlmErrNum_t (*NlmXkTblMgr__FindRecord_t)(
    NlmGenericTbl       *genericTbl,
    NlmGenericTblRecord *tblRecord,
    NlmRecordIndex      *o_recordIndex,
    NlmReasonCode       *o_reason
    );

typedef  NlmErrNum_t (*NlmXkTblMgr__DeleteRecord_t)(
    NlmGenericTbl       *genericTbl,
    NlmPortNum          portNum,
    NlmRecordIndex      recordIndex,
    NlmReasonCode       *o_reason
    );

typedef  NlmErrNum_t (*NlmXkTblMgr__UpdateRecord_t)(
    NlmGenericTbl            *genericTbl,
    NlmPortNum          portNum,
    NlmGenericTblRecord     *tblRecord,
    nlm_u8          *assocData,
    NlmRecordIndex           recordIndex,
    NlmReasonCode       *o_reason
    );


typedef  NlmErrNum_t  (*NlmXkTblMgr__Destroy_t)(
            NlmGenericTblMgr *genericTblMgr,    
            NlmPortNum          portNum,
            NlmReasonCode    *o_reason
        );


typedef NlmErrNum_t (*NlmXkTblMgr__DestroyTable_t) (
            NlmGenericTblMgr  *genericTblMgr_p,
            NlmPortNum          portNum,
            NlmGenericTbl     *genericTbl,
            NlmReasonCode     *o_reason);

typedef NlmErrNum_t (*NlmXkTblMgr__Iter_next_t)(
            NlmGenericTblIterHandle *iterHandle,
            NlmGenericTblIterData  *iterInfo);

typedef NlmErrNum_t (*NlmXkTblMgr__GetAdInfo_t)(
            NlmGenericTblMgr  *genericTblMgr_p,
			NlmTblAssoDataWidth	adWidth,
            NlmRecordIndex	  recordIndex,
			NlmGenericTblAdInfoReadType RdType,
			NlmGenericTblAdInfo *adInfo,
			NlmReasonCode *o_reason);

/* VTable for Table manager */
typedef struct NlmTblMgr__pvt_vtbl
{
    NlmXkTblMgr__CreateTable_t          CreateTable;
    NlmXkTblMgr__ConfigSearch_t         ConfigSearch;
    NlmXkTblMgr__LockConfiguration      LockConfiguration;
    NlmXkTblMgr__AddRecord_t            AddRecord;
	NlmXkTblMgr__FindRecord_t            FindRecord;
    NlmXkTblMgr__DeleteRecord_t         DeleteRecord;
    NlmXkTblMgr__UpdateRecord_t         UpdateRecord;
    NlmXkTblMgr__Destroy_t              Destroy;
    NlmXkTblMgr__DestroyTable_t         DestroyTable;
    NlmXkTblMgr__Iter_next_t            iter_next;
	NlmXkTblMgr__GetAdInfo_t			GetAdInfo;

} NlmTblMgr__pvt_vtbl;

#ifndef NLMPLATFORM_BCM
#include "nlmcmexterncend.h"
#else
#include <soc/kbp/common/nlmcmexterncend.h>
#endif

#endif /* INCLUDED_NLMXKTBLMGR_H */
