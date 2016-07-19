/*
 * $Id: jer_cnt.h Exp $
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
 */

#ifndef _JER_CNT_H
#define _JER_CNT_H

#define EIGHT_BITS (8)
#define MAX_ING_COMP_LIF_NUMBER (32)
#define MAX_ING_COMP_DELTA_VALUE (127)
#define MIN_ING_COMP_DELTA_VALUE (-128)

/*
 * Functions
 */
#include <soc/dpp/TMC/tmc_api_cnt.h>

int soc_jer_stat_counter_filter_set(SOC_SAND_IN int                             unit, 
                                    SOC_SAND_IN SOC_TMC_CNT_SOURCE*             source, 
                                    SOC_SAND_IN SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
                                    SOC_SAND_IN int                             filter_count,
                                    SOC_SAND_IN int                             is_active );

int soc_jer_stat_counter_filter_get(SOC_SAND_IN  int                             unit, 
                                    SOC_SAND_IN  SOC_TMC_CNT_SOURCE*             source, 
                                    SOC_SAND_IN  int                             filter_max_count,
                                    SOC_SAND_OUT SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
                                    SOC_SAND_OUT int*                            filter_count );

int soc_jer_cnt_ingress_compensation_profile_delta_set(SOC_SAND_IN int unit, SOC_SAND_IN int core, SOC_SAND_IN int src_type, SOC_SAND_IN int profileIndex, SOC_SAND_IN int delta);
int soc_jer_cnt_ingress_compensation_port_profile_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_IN  int profileIndex);
int soc_jer_cnt_ingress_compensation_outLif_delta_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_IN  int delta);
int soc_jer_cnt_ingress_compensation_outLif_delta_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_OUT  int * delta);
int soc_jer_cnt_ingress_compensation_port_delta_and_profile_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_OUT  int * profileIndex,  SOC_SAND_OUT  int * delta);

#endif /* _JER_CNT_H */
