/*
 * $Id: KaminoDriver.h,v 1.5 Broadcom SDK $
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
 */

#ifndef AB_KaminoDriver_H
#define AB_KaminoDriver_H


#include <sal/types.h>

#define SAND_DRV_KA_STATUS_OK     0 
#define SAND_DRV_KA_STATUS_ERROR -1


int sandDrvKaSetDefaultProfile(uint32 unit);
int sandDrvKaVerifyProfile(uint32 unit);

int sandDrvKaQsRankWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaQsRankRead(uint32 unit,  uint32 nAddress, uint32 *pData);

int sandDrvKaQsRandWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaQsRandRead(uint32 unit,  uint32 nAddress, uint32 *pData);

int sandDrvKaQsLnaRankWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaQsLnaRankRead(uint32 unit,  uint32 nAddress, uint32 *pData);

int sandDrvKaQsLnaRandWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaQsLnaRandRead(uint32 unit,  uint32 nAddress, uint32 *pData);

int sandDrvKaQsMemWrite(uint32 unit, uint32 nTableId, uint32 nAddress, uint32 *pData);
int sandDrvKaQsMemRead(uint32 unit,  uint32 nTableId, uint32 nAddress, uint32 *pData);

int sandDrvKaQsMemLnaWrite(uint32 unit, uint32 nTableId, uint32 nAddress, uint32 *pData);
int sandDrvKaQsMemLnaRead(uint32 unit,  uint32 nTableId, uint32 nAddress, uint32 *pData);

int sandDrvKaQmMemWrite(uint32 unit, uint32 nTableId, uint32 nAddress,  uint32 *pData);
int sandDrvKaQmMemRead(uint32 unit,  uint32 nTableId, uint32 nAddress,  uint32 *pData);

int sandDrvKaQmFbCacheFifoWrite(uint32 unit, uint32 *pData);
int sandDrvKaQmFbCacheFifoRead(uint32 unit,  uint32 *pData);

int sandDrvKaPmMemWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaPmMemRead(uint32 unit,  uint32 nAddress, uint32 *pData);

int sandDrvKaSfMemWrite(uint32 unit, uint32 nInstance, uint32 nAddress, uint32 *pData);
int sandDrvKaSfMemRead(uint32 unit,  uint32 nInstance, uint32 nAddress, uint32 *pData);

int sandDrvKaEgMemWrite(uint32 unit, uint32 nTableId, uint32 nAddress, uint32 *pData);
int sandDrvKaEgMemRead(uint32 unit,  uint32 nTableId, uint32 nAddress, uint32 *pData);

int sandDrvKaEiMemWrite(uint32 unit, uint32 nTableId, uint32 nAddress, uint32 *pData0);
int sandDrvKaEiMemRead(uint32 unit,  uint32 nTableId, uint32 nAddress, uint32 *pData0);

int sandDrvKaEbMemWrite(uint32 unit, uint32 nAddress,  uint32 pData[8]);
int sandDrvKaEbMemRead(uint32 unit, uint32 nAddress,  uint32 pData[8]);

int sandDrvKaEpAmClMemWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaEpAmClMemRead(uint32 unit, uint32 nAddress,  uint32 *pData);

int sandDrvKaEpBmBfMemWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaEpBmBfMemRead(uint32 unit,  uint32 nAddress, uint32 *pData);

int sandDrvKaEpMmIpMemWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaEpMmIpMemRead(uint32 unit, uint32 nAddress,  uint32 *pData);

int sandDrvKaEpMmBfMemWrite(uint32 unit, uint32 nAddress, uint32 *pData);
int sandDrvKaEpMmBfMemRead(uint32 unit, uint32 nAddress,  uint32 *pData);

int sandDrvKaRbPolMemWrite(uint32 unit, uint32 nTableId, uint32 nAddress, uint32 *pData);
int sandDrvKaRbPolMemRead(uint32 unit,  uint32 nTableId, uint32 nAddress, uint32 *pData);

int sandDrvKaRbClassMemWrite(uint32 unit, uint32 nSpi, uint32 nTableId, uint32 nAddress, uint32 *pData);
int sandDrvKaRbClassMemRead(uint32 unit,  uint32 nSpi, uint32 nTableId, uint32 nAddress, uint32 *pData);

int sandDrvKaPmDllLutWrite(uint32 unit, uint32 nTableId, uint32 nAddress, uint32 *pData);
int sandDrvKaPmDllLutRead(uint32 unit,  uint32 nTableId, uint32 nAddress, uint32 *pData);

#ifdef __cplusplus
}
#endif



#endif
