/* -*- Mode:c++; c-style:k&r; c-basic-offset:2; indent-tabs-mode: nil; -*- */
/* vi:set expandtab cindent shiftwidth=2 cinoptions=\:0l1(0t0g0: */
/*
 * $Id: sbFe2000CommonDriver.h,v 1.8 Broadcom SDK $
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
 * sbFe2000CommonDriver.h : FE2000 Common defines
 *
 *-----------------------------------------------------------------------------*/
#ifndef _SB_FE_2000_COMMON_DRIVER_H_
#define _SB_FE_2000_COMMON_DRIVER_H_

#include <soc/sbx/sbTypes.h>

#include "sbZfFe2000PmGroupConfig.hx"
uint32 
sbFe2000PmGroupConfigSet(sbhandle userDeviceHandle,
                         uint32 uGroupId,
                         sbZfFe2000PmGroupConfig_t *pConfig);

uint32 
sbFe2000PmGroupConfigGet(sbhandle userDeviceHandle,
                         uint32 uGroupId, 
                         sbZfFe2000PmGroupConfig_t *pConfig);

uint32 
sbFe2000PmProfileMemoryRead(sbhandle userDeviceHandle, uint32 uProfileId, uint32 *puData);

uint32 
sbFe2000BatchGroupRecordSizeWrite(sbhandle userDeviceHandle, uint32 uBatchGroup, uint32 uRecordSize);

uint32 
sbFe2000ByteHashConfigWrite(sbhandle userDeviceHandle, uint32 uTemplateId, uint32 *puData);

uint32 
sbFe2000ByteHashConfigRead(sbhandle userDeviceHandle, uint32 uTemplateId, uint32 *puData);

uint32 
sbFe2000BitHashConfigWrite(sbhandle userDeviceHandle, uint32 uTemplateId, uint32 *puData);

uint32 
sbFe2000BitHashConfigRead(sbhandle userDeviceHandle, uint32 uTemplateId, uint32 *puData);

#endif
