/*
 * $Id: tdm_td3_soc.h$
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56870
 */

#ifndef TDM_BCM56870_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56870_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_td3_defines.h>
#else
	#include <soc/tdm/trident3/tdm_td3_defines.h>
#endif

/* Speed index used in oversub half pipes */
enum td3_port_speed_indx_e {
    TD3_SPEED_IDX_10G=0,
    TD3_SPEED_IDX_20G=1,
    TD3_SPEED_IDX_25G=2,
    TD3_SPEED_IDX_40G=3,
    TD3_SPEED_IDX_50G=4, 
    TD3_SPEED_IDX_100G=5
};
/* flexport status */
enum td3_flex_pm_state_e {
    TD3_FLEXPORT_PM_DOWN_DOWN=0,
    TD3_FLEXPORT_PM_SAME=1,
    TD3_FLEXPORT_PM_SOME_SAME=2,
    TD3_FLEXPORT_PM_UP_DOWN=3,
    TD3_FLEXPORT_PM_DOWN_UP=4,
    TD3_FLEXPORT_PM_UP_UP=5
};


typedef struct {
	int  ovs_tables[2][TD3_OS_VBS_GRP_NUM/2][TD3_OS_VBS_GRP_LEN]; /* indexed by HP, ovs group num; ovs group index */
	enum port_speed_e ovs_grp_speed[2][TD3_OS_VBS_GRP_NUM/2];
	int  ovs_grp_weight[2][TD3_OS_VBS_GRP_NUM/2];
	int  prev_num_grps[2][6];  /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  new_num_grps[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  ports_to_be_removed[TD3_NUM_EXT_PORTS];  /* indexed by phy_port; set to 1 if needs to be removed */
	int  ports_to_be_added[TD3_NUM_EXT_PORTS];    /* indexed by phy_port; set to 1 if needs to be added   */
	int  ovs_grp_pms[2][TD3_OS_VBS_GRP_NUM/2][TD3_OS_VBS_GRP_LEN]; /* indicates which PMs contains each group */

	int  skip_ovs_for_speed[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
} td3_flxport_t;

#endif
