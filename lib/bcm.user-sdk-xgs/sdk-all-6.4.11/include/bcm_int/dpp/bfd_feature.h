/*
 * $Id$
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
 * File:    bfd_feature.h
 */

#ifndef BFD_FEATURE_H_
#define BFD_FEATURE_H_


/*Bit 1 : BFD 64bit packet statistics*/
#define BFD_SDK_BASE_VERSION    0x01000001
#define BFD_SDK_VERSION         BFD_SDK_BASE_VERSION |              \
                                    (1 << BFD_FEATURE_POLL_BIT)

#define BFD_UC_MIN_VERSION 0x01000200

/* Indicates the feature supported : Same bit has to be used in SDK and UKERNEL
 * 0 , 9 and 24th bit cannot be used as they are set in base version string
 * BFD_SDK_VERSION : 0x01000001 BFD_APPL_VERSION : 0x01000200*/

#define BFD_FEATURE_RESERVED1  0
#define BFD_FEATURE_RESERVED2  9
#define BFD_FEATURE_RESERVED3  24
#define BFD_FEATURE_64BIT_STAT 1
#define BFD_FEATURE_POLL_BIT   2

/*Macro for BFD feature check*/
#define BFD_SDK_FEATURE_CHECK(sdk_version, feature)     ((sdk_version) & (1 << (feature))) 
#define BFD_UC_FEATURE_CHECK(uc_version, feature)       ((uc_version) & (1 << (feature)))
#define BFD_FEATURE_CHECK(sdk_version, uc_version, feature)  BFD_SDK_FEATURE_CHECK(sdk_version, feature) && BFD_UC_FEATURE_CHECK(uc_version, feature)

#endif /* BFD_FEAT_H_ */

