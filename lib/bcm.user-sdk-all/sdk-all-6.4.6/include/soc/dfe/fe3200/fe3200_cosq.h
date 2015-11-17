/*
 * $Id: fe3200_fabric_cell.h,v 1.6 Broadcom SDK $
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
 * FE3200 COSQ H
 */


#ifndef _SOC_FE3200_COSQ_H_
#define _SOC_FE3200_COSQ_H_

/**********************************************************/
/*                  Includes                              */
/**********************************************************/

#include <soc/error.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/types.h>

/**********************************************************/
/*                  Defines                               */
/**********************************************************/
#define SOC_FE3200_COSQ_WFQ_WEIGHT_MAX                127
#define SOC_FE3200_COSQ_WFQ_WEIGHT_IGNORE             1
#define SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS           7

#define SOC_FE3200_COSQ_WFQ_DCH_THRESH_MAX            511
#define SOC_FE3200_COSQ_WFQ_DCM_THRESH_MAX            2047
#define SOC_FE3200_COSQ_WFQ_DCL_THRESH_MAX            1023

#define SOC_FE3200_COSQ_RATE_DCH_PERIOD_MAX           0xf
#define SOC_FE3200_COSQ_RATE_DCL_DEC_INC_VAL_MAX      0xff
#define SOC_FE3200_COSQ_RATE_DCL_DEC_INC_NOF_BITS     8
/**********************************************************/
/*                  Functions                             */
/**********************************************************/

/* pipe wfq weight functions */
soc_error_t  soc_fe3200_cosq_pipe_rx_weight_set(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight);
soc_error_t  soc_fe3200_cosq_pipe_rx_weight_get(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight);
soc_error_t  soc_fe3200_cosq_pipe_mid_weight_set(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight);
soc_error_t  soc_fe3200_cosq_pipe_mid_weight_get(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight);
soc_error_t  soc_fe3200_cosq_pipe_tx_weight_set(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight);
soc_error_t  soc_fe3200_cosq_pipe_tx_weight_get(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight);

/* pipe wfq threshold functions*/
soc_error_t  soc_fe3200_cosq_pipe_rx_threshold_set(int unit, int pipe, int port, int threshold);
soc_error_t  soc_fe3200_cosq_pipe_rx_threshold_get(int unit, int pipe, int port, int *threshold);
soc_error_t  soc_fe3200_cosq_pipe_mid_threshold_set(int unit, int pipe, int port, int threshold);
soc_error_t  soc_fe3200_cosq_pipe_mid_threshold_get(int unit, int pipe, int port, int *threshold);
soc_error_t  soc_fe3200_cosq_pipe_tx_threshold_set(int unit, int pipe, int port, int threshold);
soc_error_t  soc_fe3200_cosq_pipe_tx_threshold_get(int unit, int pipe, int port, int *threshold);

/* pipe rate fucntions */
soc_error_t  soc_fe3200_cosq_pipe_rx_rate_set(int unit, int pipe, int port, soc_dfe_cosq_shaper_mode_t shaper_mode, uint32 rate);
soc_error_t  soc_fe3200_cosq_pipe_rx_rate_get(int unit, int pipe, int port, soc_dfe_cosq_shaper_mode_t *shaper_mode, uint32 *rate);
soc_error_t  soc_fe3200_cosq_pipe_tx_rate_set(int unit, int port, soc_dfe_cosq_shaper_mode_t shaper_mode, uint32 rate);
soc_error_t  soc_fe3200_cosq_pipe_tx_rate_get(int unit, int port, soc_dfe_cosq_shaper_mode_t *shaper_mode, uint32 *rate);
soc_error_t  soc_fe3200_cosq_pipe_tx_rate_enable_set(int unit, int pipe, int port, int enable);
soc_error_t  soc_fe3200_cosq_pipe_tx_rate_enable_get(int unit, int pipe, int port, int *enable);

#endif /*!_SOC_FE3200_COSQ_H_*/

