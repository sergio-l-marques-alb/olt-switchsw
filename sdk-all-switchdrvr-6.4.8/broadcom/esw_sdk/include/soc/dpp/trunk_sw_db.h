/*
 * $Id: trunk_sw_db.h,v 1.0 Broadcom SDK $
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
 */



#ifndef _BCM_INT_DPP_TRUNK_SW_DB_H_
#define _BCM_INT_DPP_TRUNK_SW_DB_H_

#include <soc/dpp/PPD/ppd_api_lag.h>

int soc_dpp_trunk_sw_db_get_nof_replications(int unit, int tid, const SOC_PPD_LAG_MEMBER *lag_member, int* nof_replications, int* last_replicated_member_index);
int soc_dpp_trunk_sw_db_get_nof_enabled_members(int unit, int tid, int* nof_enabled_members);
int soc_dpp_trunk_sw_db_get_first_replication_index(int unit, int tid, const SOC_PPD_LAG_MEMBER *lag_member, int* first_replication_index);
int soc_dpp_trunk_sw_db_add(int unit, int tid, const SOC_PPD_LAG_MEMBER *lag_member);
int soc_dpp_trunk_sw_db_remove_all(int unit, int tid);
int soc_dpp_trunk_sw_db_set(int unit, int tid, const SOC_PPD_LAG_INFO *lag_info);
int soc_dpp_trunk_sw_db_remove(int unit, int tid, SOC_PPD_LAG_MEMBER *lag_member);
int soc_dpp_trunk_sw_db_set_trunk_attributes(int unit, int tid, int psc, int is_stateful);
int soc_dpp_trunk_sw_db_get(int unit, int tid, SOC_PPD_LAG_INFO *lag_info);

#endif /* _BCM_INT_DPP_TRUNK_SW_DB_H_ */
