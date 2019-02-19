/*
 * $Id: sbFe2000UcodeLoad.h,v 1.7 Broadcom SDK $
 *
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
 *
 * sbFe2000UcodeLoad.h : FE2000 Ucode Functions
 *
 *-----------------------------------------------------------------------------*/

#ifndef _SB_FE2000_UCODE_LOAD_H_
#define _SB_FE2000_UCODE_LOAD_H_

#include "soc/sbx/sbTypes.h"
#include <soc/sbx/fe2k_common/sbFe2000Common.h>
#include "soc/sbx/glue.h"
#include "soc/sbx/fe2k/sbFe2000Asm.h"

uint32 sbFe2000LrpInstructionMemoryRead(sbhandle userDeviceHandle,
					  UINT uAddress,
					  UINT *pData0,
					  UINT *pData1,
					  UINT *pData2);

uint32 sbFe2000LrpGetInstruction(sbhandle userDeviceHandle,
                                   int stream, int pc,
                                   UINT *pData0, UINT *pData1, UINT *pData2,
                                   int *task);

uint32 sbFe2000LrpInstructionMemoryWrite(sbhandle userDeviceHandle,
					   uint32 uAddress,
					   uint32 uData0,
					   uint32 uData1,
					   uint32 uData2);

uint32 sbFe2000UcodeLoad(sbhandle userDeviceHandle, uint8 *ucode);

uint32 sbFe2000BringUpLr(sbhandle userDeviceHandle, uint8 *ucode);

uint32 sbFe2000EnableLr(sbhandle userDeviceHandle, uint8 *ucode);

uint32 sbFe2000DisableLr(sbhandle userDeviceHandle);

uint32 sbFe2000ReloadUcode(sbhandle userDeviceHandle, uint8 *ucode);
uint32 sbFe2000GetNamedConstant(sbhandle userDeviceHandle,
                                  fe2kAsm2IntD *ucode, char * name, uint32 *val);
uint32 sbFe2000SetNamedConstant(sbhandle userDeviceHandle, fe2kAsm2IntD *ucode, char * name, uint32 val);

uint32 sbFe2000UcodeLoadFromBuffer(sbhandle userDeviceHandle,
				     fe2kAsm2IntD *a_p,
                                     unsigned char *a_b,
                                     unsigned int a_l);

int32 sbFe2000LrpInstructionMemoryWriteCallback(void *a_pv,
						  unsigned int a_sn, 
						  unsigned int a_in, 
						  unsigned char *a_n);

int32 sbFe2000LrpInstructionMemoryReadCallback(void *a_pv,
						 unsigned int a_sn, 
						 unsigned int a_in, 
						 unsigned char *a_n);

uint32 sbFe2000SwapInstructionMemoryBank(sbhandle userDeviceHandle);
#endif
