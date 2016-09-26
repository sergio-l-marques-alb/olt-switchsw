/*
 * $Id: jer_defs.h,v 1.0 Broadcom SDK $
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
 * JERICHO DEFS H
 */
 
#ifndef _SOC_QAX_DEFS_H_
#define _SOC_QAX_DEFS_H_

#define SOC_QAX_NOF_BLK 128

#define SOC_QAX_NOF_INSTANCES_CFC           1
#define SOC_QAX_NOF_INSTANCES_CGM           1
#define SOC_QAX_NOF_INSTANCES_CRPS          1
#define SOC_QAX_NOF_INSTANCES_DDP           1
#define SOC_QAX_NOF_INSTANCES_DRCA          1
#define SOC_QAX_NOF_INSTANCES_DRCB          1
#define SOC_QAX_NOF_INSTANCES_DRCC          1
#define SOC_QAX_NOF_INSTANCES_DQM           1
#define SOC_QAX_NOF_INSTANCES_ECGM          1
#define SOC_QAX_NOF_INSTANCES_ECI           1
#define SOC_QAX_NOF_INSTANCES_EDB           1
#define SOC_QAX_NOF_INSTANCES_EGQ           1
#define SOC_QAX_NOF_INSTANCES_EPNI          1
#define SOC_QAX_NOF_INSTANCES_FCR           1
#define SOC_QAX_NOF_INSTANCES_FCT           1
#define SOC_QAX_NOF_INSTANCES_FDA           1
#define SOC_QAX_NOF_INSTANCES_FDR           1
#define SOC_QAX_NOF_INSTANCES_FDT           1
#define SOC_QAX_NOF_INSTANCES_FMAC          4
#define SOC_QAX_NOF_INSTANCES_MESH_TOPOLOGY 1 
#define SOC_QAX_NOF_INSTANCES_IDB           1
#define SOC_QAX_NOF_INSTANCES_IEP           1
#define SOC_QAX_NOF_INSTANCES_IHB           1
#define SOC_QAX_NOF_INSTANCES_IHP           1
#define SOC_QAX_NOF_INSTANCES_ILKN_PMH      1
#define SOC_QAX_NOF_INSTANCES_ILKN_PML      1
#define SOC_QAX_NOF_INSTANCES_ILB           1
#define SOC_QAX_NOF_INSTANCES_IMP           1
#define SOC_QAX_NOF_INSTANCES_IPS           1
#define SOC_QAX_NOF_INSTANCES_IPSEC         1
#define SOC_QAX_NOF_INSTANCES_IPSEC_SPU_WRAPPER_TOP          2
#define SOC_QAX_NOF_INSTANCES_IRE           1
#define SOC_QAX_NOF_INSTANCES_ITE           1
#define SOC_QAX_NOF_INSTANCES_KAPS          1
#define SOC_QAX_NOF_INSTANCES_MMU           1
#define SOC_QAX_NOF_INSTANCES_NBIH          1
#define SOC_QAX_NOF_INSTANCES_NBIL          2
#define SOC_QAX_NOF_INSTANCES_OAMP          1
#define SOC_QAX_NOF_INSTANCES_OLP           1
#define SOC_QAX_NOF_INSTANCES_PEM           1
#define SOC_QAX_NOF_INSTANCES_PPDB_A        1
#define SOC_QAX_NOF_INSTANCES_PPDB_B        1
#define SOC_QAX_NOF_INSTANCES_PTS           1
#define SOC_QAX_NOF_INSTANCES_RTP           1
#define SOC_QAX_NOF_INSTANCES_SCH           1
#define SOC_QAX_NOF_INSTANCES_SPB           1
#define SOC_QAX_NOF_INSTANCES_SQM           1
#define SOC_QAX_NOF_INSTANCES_TAR           1
#define SOC_QAX_NOF_INSTANCES_TXQ           1
#define SOC_QAX_NOF_INSTANCES_CLP           4
#define SOC_QAX_NOF_INSTANCES_XLP           8

#define SOC_QAX_NOF_INSTANCES_IDR           1
#define SOC_QAX_NOF_INSTANCES_IPT           1
#define SOC_QAX_NOF_INSTANCES_IQM           2
#define SOC_QAX_NOF_INSTANCES_IRR           1
#define SOC_QAX_NOF_INSTANCES_OCB           1
#define SOC_QAX_NOF_INSTANCES_OTPC          1
#define SOC_QAX_NOF_INSTANCES_MRPS          2
#define SOC_QAX_NOF_INSTANCES_MTRPS_EM       2
#define SOC_QAX_NOF_INSTANCES_IPST          1
#define SOC_QAX_NOF_INSTANCES_IQMT          1
#define SOC_QAX_NOF_INSTANCES_FSRD          3
#define SOC_QAX_NOF_INSTANCES_DRCBROADCAST  1
#define SOC_QAX_NOF_INSTANCES_GCK          1

#endif /*_SOC_QAX_DEFS_H_*/
