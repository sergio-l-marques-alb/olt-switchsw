/*
 * $Id: dfe_fabric_cell_snake_test.h,v 1.8 Broadcom SDK $
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
 * DFE FABRIC CELL SNAKE TEST H
 */
 
#ifndef _SOC_DFE_FABRIC_CELL_SNAKE_TEST_H_
#define _SOC_DFE_FABRIC_CELL_SNAKE_TEST_H_

#include <bcm/types.h>

/* using this leads to a recursive include #include <soc/dfe/cmn/dfe_drv.h> */


#define SOC_DFE_ENABLE_MAC_LOOPBACK         (0x1)
#define SOC_DFE_ENABLE_PHY_LOOPBACK         (0x0)
/* NOTE: all loops should be set at the same level. */ 
#define SOC_DFE_ENABLE_EXTERNAL_LOOPBACK    (0x2)
#define SOC_DFE_DONT_TOUCH_MAC_INTERRUPTS   (0x4)
#define SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK  (0x8)


#define SOC_DFE_SNAKE_INFINITE_RUN 0x1
#define SOC_DFE_SNAKE_STOP_TEST 0x2 

/* Failure stage flags */
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG 0x1
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET 0x2
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION 0x4
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION 0x8
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND 0x10
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND 0x20
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND 0x40
#define SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND 0x80

/*************
 * TYPE DEFS *
 *************/

typedef struct soc_fabric_cell_snake_test_results_s
{
    int                    test_failed;
    uint32                 interrupts_status[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32                 failure_stage_flags;
    int                    tdm_lfsr_value;                         /*Relevant for FE1600 only*/
    int                    non_tdm_lfsr_value;                     /*Relevant for FE1600 only*/
    int                    lfsr_per_pipe[SOC_DFE_MAX_NOF_PIPES];   /*Relevant from FE3200 and above*/
} soc_fabric_cell_snake_test_results_t;


/*************
 * FUNCTIONS *
 *************/

int
  soc_dfe_cell_snake_test_prepare(
    int unit, 
    uint32 flags);
    
int
  soc_dfe_cell_snake_test_run(
    int unit, 
    uint32 flags,
    soc_fabric_cell_snake_test_results_t* results);




#endif /*_SOC_DFE_FABRIC_CELL_SNAKE_TEST_H_*/
