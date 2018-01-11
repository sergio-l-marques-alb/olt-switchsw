/*
 * $Id: //depot/tomahawk2/dv/tdm/chip/include/tdm_th2_soc.h#4 $
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
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56970
 */

#ifndef TDM_BCM56970_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56970_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th2_defines.h>
#else
	#include <soc/tdm/tomahawk2/tdm_th2_defines.h>
#endif

#define TDM_CHIP_EXEC__ACCESSORIZE 13


enum port_speed_indx_e { INDX_10G=0, INDX_20G=1, INDX_25G=2, INDX_40G=3, INDX_50G=4, INDX_100G=5};
enum flexport_pm_state_e { FLEXPORT_PM_DOWN_DOWN=0, FLEXPORT_PM_SAME=1, FLEXPORT_PM_SOME_SAME=2, FLEXPORT_PM_UP_DOWN=3, FLEXPORT_PM_DOWN_UP=4, FLEXPORT_PM_UP_UP=5};

typedef struct {
	int  ovs_tables[2][TH2_OS_VBS_GRP_NUM/2][TH2_OS_VBS_GRP_LEN]; /* indexed by HP, ovs group num; ovs group index */
	enum port_speed_e ovs_grp_speed[2][TH2_OS_VBS_GRP_NUM/2];
	int               ovs_grp_weight[2][TH2_OS_VBS_GRP_NUM/2];
	int  prev_num_grps[2][6];  /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  new_num_grps[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  ports_to_be_removed[TH2_NUM_EXT_PORTS];  /* indexed by phy_port; set to 1 if needs to be removed */
	int  ports_to_be_added[TH2_NUM_EXT_PORTS];    /* indexed by phy_port; set to 1 if needs to be added   */
	int  ovs_grp_pms[2][TH2_OS_VBS_GRP_NUM/2][TH2_OS_VBS_GRP_LEN]; /* indicates which PMs contains each group */

	int  skip_ovs_for_speed[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
} th2_flxport_t;

#endif
