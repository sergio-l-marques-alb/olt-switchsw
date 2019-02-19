/*
 * $Id: fe3200_fabric_flow_control.h,v 1.4 Broadcom SDK $
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
 * FE3200 FABRIC FLOW CONTROL H
 */
 
#ifndef _SOC_FE3200_FABRIC_FLOW_CONTROL_H_
#define _SOC_FE3200_FABRIC_FLOW_CONTROL_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/error.h>
#include <soc/dcmn/fabric.h>
#include <soc/types.h>
#include <soc/dfe/fe3200/fe3200_drv.h>

/* DCH Default threshold values */

#define SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_SINGLE_PIPE                (384)
#define SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_DUAL_PIPE                  (192)
#define SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_TRIPLE_PIPE                (128)
#define SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_SINGLE_PIPE           (320)
#define SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE             (160)
#define SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE           (110)
#define SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE           (380)
#define SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_DUAL_PIPE             (188)
#define SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE           (190)
#define SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_SINGLE_PIPE    (192)
#define SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_DUAL_PIPE      (96)
#define SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_TRIPLE_PIPE    (64)

/* DCM Default threshold values */

#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_SINGLE_PIPE                (1152)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_DUAL_PIPE                  (576)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_TRIPLE_PIPE                (384)
#define SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_SINGLE_PIPE    (800)
#define SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_DUAL_PIPE      (400)
#define SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_TRIPLE_PIPE    (270)
#define SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_SINGLE_PIPE    (840)
#define SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_DUAL_PIPE      (440)
#define SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_TRIPLE_PIPE    (280)
#define SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_SINGLE_PIPE    (880)
#define SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_DUAL_PIPE      (480)
#define SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_TRIPLE_PIPE    (300)
#define SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_SINGLE_PIPE    (1146)
#define SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_DUAL_PIPE      (570)
#define SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_TRIPLE_PIPE    (378)
#define SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE           (2047)
#define SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_DUAL_PIPE             (2047)
#define SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE           (2047)
#define SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE    (2047)
#define SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE      (2047)
#define SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE    (2047)

#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_THRESHOLD_SINGLE_PIPE                   (100)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (360)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (100)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (360)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (360)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (300)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_THRESHOLD_SINGLE_PIPE                   (110)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (380)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (110)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (380)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (380)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (320)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_THRESHOLD_SINGLE_PIPE                  (120)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE       (400)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE      (120)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE      (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (400)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (400)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (340)

#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_THRESHOLD_SINGLE_PIPE                   (150)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (360)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (150)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (360)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (360)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_THRESHOLD_SINGLE_PIPE                   (160)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (380)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (160)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (380)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (380)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_THRESHOLD_SINGLE_PIPE                  (170)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE       (400)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE      (170)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE      (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (400)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (400)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)

/*DCM local switch values*/
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_SINGLE_PIPE           (1152)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_UC_MC_DUAL_PIPE_0     (1152 - 8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_UC_MC_DUAL_PIPE_1     (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_IP_TDM_DUAL_PIPE      (1152/2)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_0 (1136)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_1 (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_2 (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_0  (572)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_1  (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_2  (572)
#define SOC_FE3200_DCM_MAX_THRESHOLD                                        (2047)
#define SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET                      (19)

/* DCL Default threshold values */

#define SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_SINGLE_PIPE                (768)
#define SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_DUAL_PIPE                  (384)
#define SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_TRIPLE_PIPE                (256)
#define SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_SINGLE_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE          (1023)
#define SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE    (1023)
#define SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE      (1023)
#define SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE    (1023)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_THERSHOLD_SINGLE_PIPE (40) 
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_THRESHOLD_DUAL_PIPE  (160)    
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_THRESHOLD_TRIPLE_PIPE (40)  
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_THRESHOLD_SINGLE_PIPE (400)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_THRESHOLD_DUAL_PIPE   (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_THRESHOLD_TRIPLE_PIPE (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_THRESHOLD_SINGLE_PIPE (500)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_THRESHOLD_DUAL_PIPE   (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_THRESHOLD_TRIPLE_PIPE (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_THRESHOLD_SINGLE_PIPE (762)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_THRESHOLD_DUAL_PIPE   (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_THRESHOLD_TRIPLE_PIPE (378)

#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_THRESHOLD_SINGLE_PIPE                   (24)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (240)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (24)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (240)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (200)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (280)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_THRESHOLD_SINGLE_PIPE                   (32)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (260)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (32)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (260)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (220)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (300)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_THRESHOLD_SINGLE_PIPE                  (40)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE       (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE       (280)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE      (40)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE      (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (280)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (240)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (320)

#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_THRESHOLD_SINGLE_PIPE               (50)
#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE           (240)
#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE          (50)
#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_TRIPLE_PIPE                         (240)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_THRESHOLD_SINGLE_PIPE               (58)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE           (260)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE          (58)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_TRIPLE_PIPE                         (260)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_THRESHOLD_SINGLE_PIPE              (64)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE          (280)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE         (64)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_TRIPLE_PIPE                        (280)
#define SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1                               (1023)
#define SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_2                               (1023)

#define SOC_FE3200_DCM_FIFO_SIZE_BUFFER_SIZE (1152)

soc_error_t soc_fe3200_fabric_flow_control_rci_gci_control_source_set(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t val);
soc_error_t soc_fe3200_fabric_flow_control_rci_gci_control_source_get(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t* val);
soc_error_t soc_fe3200_fabric_flow_control_thresholds_flags_validate(int unit,uint32 flags);
soc_error_t soc_fe3200_fabric_links_link_type_set(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count, soc_port_t* links);
soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3);
soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_set(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_get(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_links_link_type_get(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count_max, soc_port_t* links, uint32* links_count);
soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3);
soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int *value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_type_set(int unit, int dcm_instance, int fifo_index, soc_dfe_fabric_link_fifo_type_index_t fifo_type);


int soc_fe3200_init_dch_thresholds_config(int unit, soc_dfe_drv_dch_default_thresholds_t* dch_thresholds_default_values);
int soc_fe3200_init_dcm_thresholds_config(int unit, soc_dfe_drv_dcm_default_thresholds_t* dcm_thresholds_default_values);
int soc_fe3200_init_dcl_thresholds_config(int unit, soc_dfe_drv_dcl_default_thresholds_t* dcl_thresholds_default_values);

#endif

