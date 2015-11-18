/*
 * $Id: nlmcmdevice.h,v 1.2.8.1 Broadcom SDK $
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


 #ifndef INCLUDED_NLMCMDEVICE_H
#define INCLUDED_NLMCMDEVICE_H


/* This file contains common device attributes among all devices supported by Cynapse S/W */

/* Enum for device type. One specific usage of device type is that it is passed as
 * one of the input parameters during Table Manager Init(s).
 */
typedef enum NlmDevType
{
    NLM_DEVTYPE_9K,     /* non-sahasra NL9K device       */
    NLM_DEVTYPE_9K_S,   /* Sahasra NL9K device           */

    NLM_DEVTYPE_10K,        /* non-sahasra NL10K device       */
    NLM_DEVTYPE_10K_S,      /* Sahasra NL10K device           */
    NLM_DEVTYPE_10K_80M,    /* non-sahasra NL10K 80M device   */
    NLM_DEVTYPE_10K_80M_S,  /* Sahasra NL10K 80M device       */

    NLM_DEVTYPE_11K,    /* non-sahasra NL11K device       */
    NLM_DEVTYPE_11K_S,  /* Sahasra NL11K device           */

    NLM_DEVTYPE_12K,    /*non sahasra NL12K device */ 

    NLM_DEVTYPE_0,          
    NLM_DEVTYPE_0_S,        

    NLM_DEVTYPE_1,          
    NLM_DEVTYPE_1_S,        

    NLM_DEVTYPE_1_80M,      
    NLM_DEVTYPE_1_80M_S,    

    NLM_DEVTYPE_2,          
    NLM_DEVTYPE_2_S,        

    NLM_DEVTYPE_3,          
    NLM_DEVTYPE_3_N,
    NLM_DEVTYPE_3_40M,
    NLM_DEVTYPE_3_N_40M,

    NLM_DEVTYPE_END  /* must be the last element */

} NlmDevType;

#define NLM_DEV_NUM_KEYS    4

#endif
/* */
