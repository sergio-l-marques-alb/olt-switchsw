/* 
 * $Id: rx.c,v 1.131 Broadcom SDK $
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
 * File:    tx.c
 * Purpose: Implementation of bcm_rx* API for dune devices
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_RX

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm_int/dpp/pkt.h>

#include <bcm_int/control.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/common/tx.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/utils.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/mirror.h>

#include <soc/dcmn/dcmn_wb.h>

#include <soc/dpp/trunk_sw_db.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/headers.h>
#include <soc/dpp/TMC/tmc_api_stack.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lag.h>

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_action_cmd.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_ports.h>

#endif

#include <shared/shr_template.h>

/* #define ARAD_RX_COSQ_MODE_ENABLE */


#define DPP_RX_START_CHECK(unit) \
        BCM_DPP_UNIT_CHECK(unit);\
        if(!RX_UNIT_STARTED(unit)) { return BCM_E_INIT; }

/* DPP_RX_INIT_CHECK is different from RX_INIT_CHECK (which calls bcm_petra_rx_init 
 * when RX module is not initialized yet)
 */
#define DPP_RX_INIT_CHECK(unit) \
        BCM_DPP_UNIT_CHECK(unit);\
        if(!RX_INIT_DONE(unit)) { return BCM_E_INIT; }

/* Default data for configuring RX system */
#define DPP_RX_PKT_SIZE_DEFAULT     (16 * 1024) /* 16K pkts */
#define DPP_RX_PPC_DEFAULT          (1)         /* no chains */
#define DPP_RX_PPS_DEFAULT          (1000)      /* 1000 pkts/sec */
#define DPP_RX_FREE_LIST_NEXT(data) (((void **)data)[0])
#define DPP_RX_QUEUE(unit)          (&(rx_ctl[unit]->pkt_queue[0])) /* cos 0 */
#define DPP_RX_PKT(unit, idx)       (rx_ctl[unit]->all_pkts[idx])
#define DPP_RX_PKT_USED(unit, idx)  (*(rx_ctl[unit]->pkt_load_ids + idx))
#define DPP_RX_BUF_CHECK_RETRY_COUNT    (10)
#define DPP_RX_PKT_USED_SET_LOCK(unit, idx, val)\
        do {                             \
            RX_LOCK(unit);                      \
            DPP_RX_PKT_USED(unit, idx) = val;   \
            RX_UNLOCK(unit);                    \
        } while (0)

#define DPP_RX_CPU_ASYNC_TCMD_SOP       (0)
#define DPP_RX_CPU_ASYNC_TCMD_EOP_2BV   (3) /* EOP with 2Bytes valid */
#define DPP_RX_CPU_ASYNC_TCMD_EOP_1BV   (4) /* EOP with 1Byte valid */

#define DPP_RX_DEFAULT_ALLOC    bcm_rx_pool_alloc
#define DPP_RX_DEFAULT_FREE     bcm_rx_pool_free




#define _BCM_TRAP_CODE_VIRTUAL_BASE (_BCM_TRAP_VIRTUAL_TO_ID(0))

/* MACRO returns new trap id that was allocated for user defined and virtual traps*/

#define _BCM_TRAP_EG_MASK (0x7ff)

#define MAX_ITTR_COUNT (10)

/* Number of snoop and traps reserved for ITMH parsing in Aard at init */
#define _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT   (SOC_PPC_TRAP_CODE_NOF_USER_DEFINED_SNOOPS_FOR_TM)


#define _BCM_RX_SNOOP_CONFIG_PROB_TO_TMC_ACTION_CMD_SNOOP_INFO_PROB(snoop_config_prob, tmc_action_command_prob) \
do {\
    uint64 prob_res;\
    /* the math is  x ---> x* (2^16 -1)/ 100,000. Unfortunately for this we need a uint64*/\
        COMPILER_64_SET(prob_res, 0, snoop_config_prob);\
        COMPILER_64_UMUL_32(prob_res, 64*1024-1);\
        /* we would like now to divide by 100000.*/ \
        /*   in prob_res_tmp only bit 32 may be set bits 33-63 must be reset because config->probability */\
        /*doesn't exceed 100000.*/\
        /* we will shift right by 1 and then divide by 50000 */\
        COMPILER_64_SHR(prob_res, 1);\
        tmc_action_command_prob = COMPILER_64_LO(prob_res) / 50000;\
} while (0)


#define _BCM_RX_TMC_ACTION_CMD_SNOOP_INFO_PROB_TO_SNOOP_CONFIG_PROB(snoop_config_prob, tmc_action_command_prob) \
do {\
    uint64 prob_res;\
    /* the math is x ---> x* (2^100,000)/(2^16-1). Unfortunately for this we need a uint64, but there is no compiler64 division, so a compromise is needed.*/\
        COMPILER_64_SET(prob_res, 0, tmc_action_command_prob);\
        COMPILER_64_UMUL_32(prob_res, 50000);\
        snoop_config_prob = COMPILER_64_LO(prob_res) / ((SOC_IS_JERICHO(unit) ? 64 :1)*32*1024-1);\
            if (snoop_config_prob > 100000 -4) {\
            /* close enough... */\
                snoop_config_prob = 100000;\
            }\
} while (0)


#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)

/**
 * return one of three available snoop sizes: 64 192 and -1
 */
#define _BCM_RX_TMC_ACTION_SIZE_TO_SNOOP_SIZE(tmc_action_size)  (tmc_action_size==SOC_TMC_ACTION_CMD_SIZE_BYTES_64)? 64 : \
(tmc_action_size==SOC_TMC_ACTION_CMD_SIZE_BYTES_192)? 192 : -1 ;

#define RX_ACCESS   sw_state_access[unit].dpp.bcm.rx
                                   
/*
 * Function
 *      _bcm_dpp_rx_virtual_trap_get _rx_virtual_traps array.
 * Purpose
 *      find the specific struct in the 
 * Parameters
 *      (in)  unit                  = unit number
 *      (in)  prm_profile_info    = trap date
 *      (out) config              = config field
 * Returns
 *      rv = error code.
 *                    
 */
int _bcm_dpp_rx_virtual_trap_get(int unit,int virtual_trap_id,bcm_dpp_rx_virtual_traps_t *virtual_trap_str)
{
    BCMDNX_INIT_FUNC_DEFS;
        
    if ( (virtual_trap_id>=0) && (virtual_trap_id<_BCM_NUMBER_OF_VIRTUAL_TRAPS) ) {
        /*returns pointer access to _rx_virtual_traps array*/
        BCMDNX_IF_ERR_EXIT(
           RX_ACCESS.virtual_traps.get(unit, virtual_trap_id, virtual_trap_str));
    } else {
        /*Wrong input value*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong argument,function _bcm_dpp_rx_virtual_trap_get, on unit:%d \n"), unit));
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_rx_virtual_trap_set(int unit,int virtual_trap_id,bcm_dpp_rx_virtual_traps_t *virtual_trap_str)
{
    BCMDNX_INIT_FUNC_DEFS;

    if ( (virtual_trap_id>=0) && (virtual_trap_id<_BCM_NUMBER_OF_VIRTUAL_TRAPS) ) {
        /*returns pointer access to _rx_virtual_traps array*/
        BCMDNX_IF_ERR_EXIT(
           RX_ACCESS.virtual_traps.set(unit, virtual_trap_id, virtual_trap_str));
    } else {
        /*Wrong input value*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong argument,function _bcm_dpp_rx_virtual_trap_set, on unit:%d \n"), unit));
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_dpp_rx_fwd_action_to_config
 * Purpose
 *      Fill config field with trap data(prm_profile_info).
 * Parameters
 *      (in)  unit                  = unit number
 *      (in)  prm_profile_info    = trap date
 *      (out) config              = config field
 * Returns
 *      rv = error code.
 *                    
 */
STATIC int _bcm_dpp_rx_fwd_action_to_config(
    int unit,SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO* prm_profile_info,bcm_rx_trap_config_t *config)
{
    int rv= BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL")));         
    }

    if(prm_profile_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("prm_profile_info pointer is NULL")));         
    }

    config->trap_strength = prm_profile_info->strength;

    if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST) {        
        if (prm_profile_info->dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_MC) {
            config->dest_group = prm_profile_info->dest_info.frwrd_dest.dest_id;
            config->flags |= BCM_RX_TRAP_DEST_MULTICAST;
        }else{
            config->flags |= BCM_RX_TRAP_UPDATE_DEST;

            if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST_OUTLIF)
            {
                config->flags |= BCM_RX_TRAP_UPDATE_ENCAP_ID;

                rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit, 
                                                                &config->dest_port, 
                                                                &config->encap_id, 
                                                                &(prm_profile_info->dest_info.frwrd_dest), 
                                                                BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL/*encap_usage*/,
                                                                1/*force_destination*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit, 
                                                                &config->dest_port, 
                                                                NULL, 
                                                                &(prm_profile_info->dest_info.frwrd_dest), 
                                                                -1/*encap_usage*/,
                                                                0/*force_destination*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC) {
        config->prio = prm_profile_info->cos_info.tc;
        config->flags |= BCM_RX_TRAP_UPDATE_PRIO;
    }

    if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
        config->color = prm_profile_info->cos_info.dp;
        config->flags |= BCM_RX_TRAP_UPDATE_COLOR;
    }

    if (prm_profile_info->processing_info.is_control == TRUE) {
        config->flags |= BCM_RX_TRAP_BYPASS_FILTERS;
    }

    if (prm_profile_info->processing_info.enable_learning == FALSE) {
        config->flags |= BCM_RX_TRAP_LEARN_DISABLE;
    }

    if (prm_profile_info->dest_info.add_vsi == TRUE) {
        config->flags |= BCM_RX_TRAP_UPDATE_ADD_VLAN;
    }
    if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_TYPE) {
         config->flags          |= BCM_RX_TRAP_UPDATE_FORWARDING_TYPE;
         _bcm_dpp_rx_ppd_to_forwarding_type(prm_profile_info->processing_info.frwrd_type, &config->forwarding_type);
     }

    if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_METER_0) 
    {
            config->policer_id = prm_profile_info->meter_info.meter_id;
            config->flags |= BCM_RX_TRAP_UPDATE_POLICER;
    }

    if (prm_profile_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_POLICER) {
        /* Check if ethernet_police_id is correct*/
        config->ether_policer_id = prm_profile_info->policing_info.ethernet_police_id;
        config->flags |= BCM_RX_TRAP_UPDATE_ETHERNET_POLICER;
    }

    
    config->counter_num = prm_profile_info->count_info.counter_id;
    
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function
 *      _bcm_dpp_rx_egress_set
 * Purpose
 *       Maps egress trap type to a cofigured egress action profile
 * Parameters
 *      (in) trap_id                 = The ppd trap code
 *      (in) prm_egr_profile_info    = Egress action profile 
 * Returns
 *      rv = Error code
 *                    
 */

int _bcm_dpp_rx_egress_set(int unit,int trap_id, SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *prm_egr_profile_info)
{
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO               default_info;
    int                                           old_template,is_last,is_allocated,curr_template;
    int                                           flags=0;    

    int                                           rv= BCM_E_NONE;
    uint32                                       soc_sand_rv;
    uint32 trap_index; /* for the allocation manager only, not PPD calls */

    BCMDNX_INIT_FUNC_DEFS;
    
    if(prm_egr_profile_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("prm_egr_profile_info pointer is NULL")));         
    }

    unit = (unit); 

    /* Translate Trap id to trap index for the allocation manager for egress traps */
    rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                       trap_id, /* from the enum */
                                       &trap_index); /* 0...NOF_TRAPS */
    BCMDNX_IF_ERR_EXIT(rv);

    /*Egress handle*/        
    rv = _bcm_dpp_am_template_trap_egress_exchange(unit,flags, trap_id, trap_index,
        prm_egr_profile_info,&old_template,&is_last,&curr_template,&is_allocated);
    BCM_SAND_IF_ERR_EXIT(rv);


    if (old_template != curr_template) {
        if(curr_template == 0) {
            curr_template = SOC_PPC_TRAP_EG_NO_ACTION;
        }
        soc_sand_rv = soc_ppd_trap_to_eg_action_map_set(unit,trap_id,curr_template);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    if (is_last) {

        SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);

        /*Call to old template with default profile*/
        soc_sand_rv = soc_ppd_trap_eg_profile_info_set(unit,old_template,&default_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    if (is_allocated) {
        if(curr_template == SOC_PPC_TRAP_EG_NO_ACTION) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("cannot update default no-action.")));
        }
        soc_sand_rv = soc_ppd_trap_eg_profile_info_set(unit,curr_template,prm_egr_profile_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }



    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_rx_egress_set_to_drop_info(int unit, SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *eg_drop_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(eg_drop_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("eg_drop_info pointer is NULL")));
    }

    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(eg_drop_info);
    eg_drop_info->bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    eg_drop_info->out_tm_port = SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID;



    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_rx_egress_set_to_cpu_drop_info(int unit, SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *eg_cpu_drop_info)
{

    BCMDNX_INIT_FUNC_DEFS;

    if(eg_cpu_drop_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("eg_cpu_drop_info pointer is NULL")));
    }

    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(eg_cpu_drop_info);

    eg_cpu_drop_info->bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    eg_cpu_drop_info->out_tm_port = SOC_PPC_TRAP_ACTION_PKT_CPU_ID;

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function
 *      _bcm_dpp_trap_root_mate_get
 * Purpose
 *      It is supposed that each mate group of traps contains a root mate that has a BCM name.
 * This procedure returns the trap ID of a root mate for the given trap ID (input)
 * if a root mate exists.
 *      The root mate MUST have a trap type defined.
 * Parameters
 *      (in) trap_id    = the ppd trap code
 * Returns
 *      rv = ppd trap root mate code
 *  ATTENTION: The functions _bcm_dpp_trap_mate_get and
 *        _bcm_dpp_trap_root_mate_get contain conversions in opposite directions.
 *   When one of them is modified, the other MUST be modified accordingly.
*/                  
STATIC int
_bcm_dpp_trap_root_mate_get(int unit, int trap_id)
{
	int rv = -1;
	BCMDNX_INIT_FUNC_DEFS;

	switch (trap_id) {

	case SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS:
	case SOC_PPC_TRAP_CODE_MPLS_VRF_NO_BOS:
	case SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS:
		rv = SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14;
		break;
	case SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_0:
		rv = SOC_PPC_TRAP_CODE_MPLS_TTL0;
		break;
	case SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_1:
		rv = SOC_PPC_TRAP_CODE_MPLS_TTL1;
		break;
	case SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
	case SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
		rv = SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_TRAP;
		break;
	case SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP:
		break;

	case SOC_PPC_TRAP_CODE_IPV4_TERM_DIP_ZERO:
		break;
	case SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_IS_MC:
		break;
	case SOC_PPC_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
		break;
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_7:
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_6:
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_5:
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_4:
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_3:
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_2:
	case SOC_PPC_TRAP_CODE_UNKNOWN_DA_1:
		rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_0;
		break;

	case SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_BOS:
		rv = SOC_PPC_TRAP_CODE_MPLS_LSP_BOS;
		break;

		/*Trap mates that are supported only in Jericho*/
	case SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY:
		if (SOC_IS_JERICHO(unit)) {
			rv = SOC_PPC_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY;
		}
		break;
	case SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG:
		if (SOC_IS_JERICHO(unit)) {
			rv = SOC_PPC_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG;
		}
		break;
	case SOC_PPC_TRAP_CODE_IGMP_UNDEFINED:
		if (SOC_IS_JERICHO(unit)) {
			rv = SOC_PPC_TRAP_CODE_ETH_FL_IGMP_UNDEFINED;
		}
		break;
	default:
		rv = -1;
	}
	BCM_RETURN_VAL_EXIT(rv);
exit:
BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_dpp_trap_mate_get
 * Purpose
 *      Get the trap mates for each trap code if exist.
 * Parameters
 *      (in) trap_id    = the ppd trap code
 * Returns
 *      rv = ppd trap mate
 *                    
 */
STATIC int
_bcm_dpp_trap_mate_get(int unit, int trap_id)
{
    int rv = -1;
    
    BCMDNX_INIT_FUNC_DEFS;
    switch(trap_id) 
    {
    /* For example: SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14 work with a link list style that follows first 3 cases*/
    case SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14:
        rv = SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS:
        rv = SOC_PPC_TRAP_CODE_MPLS_VRF_NO_BOS;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_VRF_NO_BOS:
        rv = SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TTL0:
        rv = SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_0;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TTL1:
        rv = SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_1;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_1:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_TRAP:
        rv = SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
        rv = SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_IPV4_TERM_DIP_ZERO:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_IS_MC:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_0:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_1;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_1:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_2;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_2:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_3;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_3:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_4;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_4:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_5;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_5:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_6;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_6:
        rv = SOC_PPC_TRAP_CODE_UNKNOWN_DA_7;
        break;
    case SOC_PPC_TRAP_CODE_UNKNOWN_DA_7:
        rv = -1;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_LSP_BOS:
        rv = SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_BOS;
        break;
    case SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_BOS:
      
        break;
    /*Trap mates that are supported only in Jericho*/
    case SOC_PPC_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY:
        if (SOC_IS_JERICHO(unit))
        {
              rv = SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY;
              break;
        }
        else
        {
              rv = -1;
              break;
        }
    case SOC_PPC_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG:
        if (SOC_IS_JERICHO(unit))
        {
              rv = SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG;
              break;
        }
        else
        {
              rv = -1;
              break;
        }
    case SOC_PPC_TRAP_CODE_ETH_FL_IGMP_UNDEFINED:
        if (SOC_IS_JERICHO(unit))
        {
              rv = SOC_PPC_TRAP_CODE_IGMP_UNDEFINED;
              break;
        }
        else
        {
              rv = -1;
              break;
        }

    default:
        rv = -1;
    }
    BCM_RETURN_VAL_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



   
#ifdef BCM_ARAD_SUPPORT
/* Default data for configuring RX system */
CONST STATIC bcm_rx_cfg_t _rx_arad_dflt_cfg = {
    DPP_RX_PKT_SIZE_DEFAULT,       /* packet alloc size */
    RX_PPC_DFLT,            /* Packets per chain */
    RX_PPS_DFLT,            /* Default pkt rate, global (all COS, one unit) */
    0,                      /* Burst */
    {                       /* Just configure channel 1 */
        {0},                /* Channel 0 is usually TX */
        {                   /* Channel 1, default RX */
            RX_CHAINS_DFLT, /* DV count (number of chains) */
            1000,           /* Default pkt rate, DEPRECATED */
            0,              /* No flags */
            0xff            /* COS bitmap channel to receive */
        }
    },
    DPP_RX_DEFAULT_ALLOC,       /* alloc function */
    DPP_RX_DEFAULT_FREE,        /* free function */
    0                       /* flags */
};
#endif


STATIC SOC_PPC_TRAP_CODE _bcm_rx_trap_to_ppd[bcmRxTrapCount];

/*
 * Function
 *      _bcm_dpp_rx_trap_egrr_calc
 * Purpose
 *      convert bit map trap id to value trap id.
 * Parameters
 *      (in)  egrr_trap    			= bit map trap id
 * Returns
 *      rv = value trap id.
 *                    
 */
int
_bcm_dpp_rx_trap_egrr_calc(int egrr_trap)
{
    int counter=0;

    while (egrr_trap) {
        egrr_trap = egrr_trap>>1;
        counter++;
    }
    return (counter-1);
}

/*
 * Function
 *      _bcm_dpp_rx_trap_egrr_reverse_calc
 * Purpose
 *      convert value trap id to bit map trap id.
 * Parameters
 *      (in)  egrr_trap    			= value trap id
 * Returns
 *      rv = bit map trap id.
 *                    
 */
int
_bcm_dpp_rx_trap_egrr_reverse_calc(int egrr_trap)
{

    int counter=1;

    while (egrr_trap) {
        counter = counter<<1;
        egrr_trap--;
    }
    return counter;
}

/*
 * Function
 *      _bcm_dpp_rx_trap_id_to_egress
 * Purpose
 *      convert value trap id to bit map trap id,support 2 cases.
 * Parameters
 *      (in)  egrr_trap    			= value trap id
 * Returns
 *      rv = bit map trap id.
 *                    
 */
int _bcm_dpp_rx_trap_id_to_egress(int _eg_trap)
{

    return ( ( ((_eg_trap&(_BCM_TRAP_EG_MASK)) <= _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT)? (_bcm_dpp_rx_trap_egrr_reverse_calc(_eg_trap&(_BCM_TRAP_EG_MASK))) : (((_eg_trap&(_BCM_TRAP_EG_MASK))- _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT)<<_BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT) ) ); /*set bit 9 to low*/
}



/*
 * Function
 *      _bcm_dpp_rx_trap_id_to_etpp
 * Purpose
 *      removes etpp bit
 * Parameters
 *      (in)  etpp_trap = value trap id                 
 */ 
int _bcm_dpp_rx_trap_id_to_etpp(int _etpp_trap)
{
    int mask = ~(_BCM_DPP_RX_TRAP_ETPP_TYPE_MASK << _BCM_DPP_RX_TRAP_ETPP_TYPE_SHIFT);
    return (_etpp_trap & mask);
}


/*
 * Function
 *       _bcm_dpp_rx_trap_to_ppd_init
 * Purpose
 *      init _bcm_rx_trap_to_ppd array.
 * Parameters
 *      (in)  unit = unit number
 *
 */
void _bcm_dpp_rx_trap_to_ppd_init(int unit)
{
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaFwd]                 = SOC_PPC_TRAP_CODE_PBP_SA_DROP_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaDrop]                = SOC_PPC_TRAP_CODE_PBP_SA_DROP_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaTrap]                = SOC_PPC_TRAP_CODE_PBP_SA_DROP_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaSnoop]               = SOC_PPC_TRAP_CODE_PBP_SA_DROP_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimTeSaMove]                        = SOC_PPC_TRAP_CODE_PBP_TE_TRANSPLANT;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimTeSaUnknown]                     = SOC_PPC_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimSaMove]                          = SOC_PPC_TRAP_CODE_PBP_TRANSPLANT;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimSaUnknown]                       = SOC_PPC_TRAP_CODE_PBP_LEARN_SNOOP;
    _bcm_rx_trap_to_ppd[bcmRxTrapAuthSaLookupFail]                   = SOC_PPC_TRAP_CODE_SA_AUTHENTICATION_FAILED;
    _bcm_rx_trap_to_ppd[bcmRxTrapAuthSaPortFail]                     = SOC_PPC_TRAP_CODE_PORT_NOT_PERMITTED;
    _bcm_rx_trap_to_ppd[bcmRxTrapAuthSaVlanFail]                     = SOC_PPC_TRAP_CODE_UNEXPECTED_VID;
    _bcm_rx_trap_to_ppd[bcmRxTrapSaMulticast]                        = SOC_PPC_TRAP_CODE_SA_MULTICAST;
    _bcm_rx_trap_to_ppd[bcmRxTrapSaEqualsDa]                         = SOC_PPC_TRAP_CODE_SA_EQUALS_DA;
    _bcm_rx_trap_to_ppd[bcmRxTrap8021xFail]                          = SOC_PPC_TRAP_CODE_8021X;
    _bcm_rx_trap_to_ppd[bcmRxTrapArpMyIp]                            = SOC_PPC_TRAP_CODE_MY_ARP;
    _bcm_rx_trap_to_ppd[bcmRxTrapArp]                                = SOC_PPC_TRAP_CODE_ARP;
    _bcm_rx_trap_to_ppd[bcmRxTrapIgmpMembershipQuery]                = SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapIgmpReportLeaveMsg]                 = SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG;
    _bcm_rx_trap_to_ppd[bcmRxTrapIgmpUndefined]                      = SOC_PPC_TRAP_CODE_IGMP_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpv6MldMcListenerQuery]           = SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Cache]                            = _BCM_TRAP_CODE_VIRTUAL_BASE;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv6Server]                       = SOC_PPC_TRAP_CODE_DHCPV6_SERVER;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv6Client]                       = SOC_PPC_TRAP_CODE_DHCPV6_CLIENT;
    _bcm_rx_trap_to_ppd[bcmRxTrapPortNotVlanMember]                  = SOC_PPC_TRAP_CODE_PORT_NOT_VLAN_MEMBER;
    _bcm_rx_trap_to_ppd[bcmRxTrapHeaderSizeErr]                      = SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyBmacUknownTunnel]                 = SOC_PPC_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyBmacUnknownISid]                  = SOC_PPC_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID;
    _bcm_rx_trap_to_ppd[bcmRxTrapStpStateBlock]                      = SOC_PPC_TRAP_CODE_STP_STATE_BLOCK;
    _bcm_rx_trap_to_ppd[bcmRxTrapStpStateLearn]                      = SOC_PPC_TRAP_CODE_STP_STATE_LEARN;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpCompMcInvalidIp]                  = SOC_PPC_TRAP_CODE_IP_COMP_MC_INVALID_IP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyMacAndIpDisabled]                 = SOC_PPC_TRAP_CODE_MY_MAC_AND_IP_DISABLE;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillVersion]                       = SOC_PPC_TRAP_CODE_TRILL_VERSION;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillTtl0]                          = SOC_PPC_TRAP_CODE_TRILL_INVALID_TTL;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillChbh]                          = SOC_PPC_TRAP_CODE_TRILL_CHBH;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillUnknonwnIngressNickname]       = SOC_PPC_TRAP_CODE_TRILL_NO_REVERSE_FEC;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillCite]                          = SOC_PPC_TRAP_CODE_TRILL_CITE;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillIllegalInnerMc]                = SOC_PPC_TRAP_CODE_TRILL_ILLEGAL_INNER_MC;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyMacAndMplsDisable]                = SOC_PPC_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE;
    _bcm_rx_trap_to_ppd[bcmRxTrapArpReply]                           = SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyMacAndUnknownL3]                  = SOC_PPC_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0150]                      = SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0151]                      = SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0152]                      = SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0153]                      = SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsTerminationFail]                = SOC_PPC_TRAP_CODE_MPLS_TERM_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsUnexpectedBos]                  = SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_BOS;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsUnexpectedNoBos]                = SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS;
    _bcm_rx_trap_to_ppd[bcmRxTrapCfmAcceleratedIngress]              = SOC_PPC_TRAP_CODE_CFM_ACCELERATED_INGRESS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIllegelPacketFormat]                = SOC_PPC_TRAP_CODE_ILLEGEL_PFC;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaFwd]                  = SOC_PPC_TRAP_CODE_SA_DROP_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaDrop]                 = SOC_PPC_TRAP_CODE_SA_DROP_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaTrap]                 = SOC_PPC_TRAP_CODE_SA_DROP_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaSnoop]                = SOC_PPC_TRAP_CODE_SA_DROP_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn0]                           = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn1]                           = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn2]                           = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn3]                           = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapVlanUnknownDa]                      = SOC_PPC_TRAP_CODE_UNKNOWN_DA_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapExternalLookupError]                = SOC_PPC_TRAP_CODE_ELK_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfFwd]                           = SOC_PPC_TRAP_CODE_DA_NOT_FOUND_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfDrop]                          = SOC_PPC_TRAP_CODE_DA_NOT_FOUND_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfTrap]                          = SOC_PPC_TRAP_CODE_DA_NOT_FOUND_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfSnoop]                         = SOC_PPC_TRAP_CODE_DA_NOT_FOUND_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapExtendedP2pLookupFail]              = SOC_PPC_TRAP_CODE_P2P_MISCONFIGURATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapSameInterface]                      = SOC_PPC_TRAP_CODE_SAME_INTERFACE;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillUnknownUc]                     = SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_UC;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillUnknownMc]                     = SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_MC;
    _bcm_rx_trap_to_ppd[bcmRxTrapUcLooseRpfFail]                     = SOC_PPC_TRAP_CODE_UC_LOOSE_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapDefaultUcv6]                        = SOC_PPC_TRAP_CODE_DEFAULT_UCV6;
    _bcm_rx_trap_to_ppd[bcmRxTrapDefaultMcv6]                        = SOC_PPC_TRAP_CODE_DEFAULT_MCV6;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsP2pNoBos]                       = SOC_PPC_TRAP_CODE_MPLS_P2P_NO_BOS;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsControlWordTrap]                = SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_TRAP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsControlWordDrop]                = SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_DROP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsUnknownLabel]                   = SOC_PPC_TRAP_CODE_MPLS_UNKNOWN_LABEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsExtendP2pMplsx4]                = SOC_PPC_TRAP_CODE_MPLS_P2P_MPLSX4;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIgmpMembershipQuery]           = SOC_PPC_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIgmpReportLeaveMsg]            = SOC_PPC_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIgmpUndefined]                 = SOC_PPC_TRAP_CODE_ETH_FL_IGMP_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIcmpv6MldMcListenerQuery]      = SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIcmpv6MldReportDone]           = SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIcmpv6MldUndefined]            = SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4VersionError]                   = SOC_PPC_TRAP_CODE_IPV4_VERSION_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4ChecksumError]                  = SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4HeaderLengthError]              = SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4TotalLengthError]               = SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4Ttl0]                           = SOC_PPC_TRAP_CODE_IPV4_TTL0;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4HasOptions]                     = SOC_PPC_TRAP_CODE_IPV4_HAS_OPTIONS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4Ttl1]                           = SOC_PPC_TRAP_CODE_IPV4_TTL1;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4SipEqualDip]                    = SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4DipZero]                        = SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4SipIsMc]                        = SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4TunnelTerminationAndFragmented] = SOC_PPC_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6VersionError]                   = SOC_PPC_TRAP_CODE_IPV6_VERSION_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6HopCount0]                      = SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT0;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6HopCount1]                      = SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6UnspecifiedDestination]         = SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LoopbackAddress]                = SOC_PPC_TRAP_CODE_IPV6_LOOPBACK_ADDRESS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6MulticastSource]                = SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6NextHeaderNull]                 = SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6UnspecifiedSource]              = SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalLinkDestination]           = SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalSiteDestination]           = SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalLinkSource]                = SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalSiteSource]                = SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6Ipv4CompatibleDestination]      = SOC_PPC_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6Ipv4MappedDestination]          = SOC_PPC_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6MulticastDestination]           = SOC_PPC_TRAP_CODE_IPV6_MULTICAST_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsTtl0]                           = SOC_PPC_TRAP_CODE_MPLS_TTL0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsTtl1]                           = SOC_PPC_TRAP_CODE_MPLS_TTL1;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpSnFlagsZero]                     = SOC_PPC_TRAP_CODE_TCP_SN_FLAGS_ZERO;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpSnZeroFlagsSet]                  = SOC_PPC_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpSynFin]                          = SOC_PPC_TRAP_CODE_TCP_SYN_FIN;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpEqualPorts]                      = SOC_PPC_TRAP_CODE_TCP_EQUAL_PORTS;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpFragmentIncompleteHeader]        = SOC_PPC_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpFragmentOffsetLt8]               = SOC_PPC_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8;
    _bcm_rx_trap_to_ppd[bcmRxTrapUdpEqualPorts]                      = SOC_PPC_TRAP_CODE_UDP_EQUAL_PORTS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpDataGt576]                      = SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpFragmented]                     = SOC_PPC_TRAP_CODE_ICMP_FRAGMENTED;
    _bcm_rx_trap_to_ppd[bcmRxTrapFailoverFacilityInvalid]            = SOC_PPC_TRAP_CODE_FACILITY_INVALID;
    _bcm_rx_trap_to_ppd[bcmRxTrapUcStrictRpfFail]                    = SOC_PPC_TRAP_CODE_UC_STRICT_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMcExplicitRpfFail]                  = SOC_PPC_TRAP_CODE_MC_EXPLICIT_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMcUseSipRpfFail]                    = SOC_PPC_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMcUseSipMultipath]                  = SOC_PPC_TRAP_CODE_MC_USE_SIP_ECMP;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpRedirect]                       = SOC_PPC_TRAP_CODE_ICMP_REDIRECT;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamp]                               = SOC_PPC_TRAP_CODE_USER_OAMP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamEthAccelerated]                  = SOC_PPC_TRAP_CODE_TRAP_ETH_OAM;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamMplsAccelerated]                 = SOC_PPC_TRAP_CODE_USER_MPLS_OAM_ACCELERATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdIpTunnelAccelerated]          = SOC_PPC_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdPweAccelerated]               = SOC_PPC_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamEthUpAccelerated]                = SOC_PPC_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED;

    _bcm_rx_trap_to_ppd[bcmRxTrapDfltDroppedPacket]                  = SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP;
    _bcm_rx_trap_to_ppd[bcmRxTrapDfltRedirectToCpuPacket]            = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;
    _bcm_rx_trap_to_ppd[bcmRxTrapRedirectToCpuOamPacket]             = SOC_PPC_TRAP_CODE_OAM_CPU;
    _bcm_rx_trap_to_ppd[bcmRxTrapSnoopOamPacket]                     = SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP;
    _bcm_rx_trap_to_ppd[bcmRxTrapRecycleOamPacket]                   = SOC_PPC_TRAP_CODE_OAM_RECYCLE;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop0]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop1]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop2]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop3]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop4]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_4;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop5]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_5;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop6]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_6;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop7]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_7;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop8]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_8;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop9]                         = SOC_PPC_TRAP_CODE_ITMH_SNOOP_9;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop10]                        = SOC_PPC_TRAP_CODE_ITMH_SNOOP_10;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop11]                        = SOC_PPC_TRAP_CODE_ITMH_SNOOP_11;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop12]                        = SOC_PPC_TRAP_CODE_ITMH_SNOOP_12;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop13]                        = SOC_PPC_TRAP_CODE_ITMH_SNOOP_13;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop14]                        = SOC_PPC_TRAP_CODE_ITMH_SNOOP_14;
    _bcm_rx_trap_to_ppd[bcmRxTrapItmhSnoop15]                        = SOC_PPC_TRAP_CODE_ITMH_SNOOP_15;
    _bcm_rx_trap_to_ppd[bcmRxTrapFcoeFcfPacket]                      = SOC_PPC_TRAP_CODE_FCOE_WA_FIX_OFFSET;
    _bcm_rx_trap_to_ppd[bcmRxTrapVlanTagDiscard]                     = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
    _bcm_rx_trap_to_ppd[bcmRxTrapVlanTagAccept]                      = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    _bcm_rx_trap_to_ppd[bcmRxTrap1588Discard]                        = SOC_PPC_TRAP_CODE_1588_PACKET_1;
    _bcm_rx_trap_to_ppd[bcmRxTrap1588Accepted]                       = SOC_PPC_TRAP_CODE_1588_PACKET_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapBfdEchoOverIpv4]                    = SOC_PPC_TRAP_CODE_BFD_ECHO;
    _bcm_rx_trap_to_ppd[bcmRxTrapSat0]                               = SOC_PPC_TRAP_CODE_SAT_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapSat1]                               = SOC_PPC_TRAP_CODE_SAT_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapSat2]                               = SOC_PPC_TRAP_CODE_SAT_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamMipSnoop2ndPass]                 = SOC_PPC_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
    _bcm_rx_trap_to_ppd[bcmRxTrapBfdOamDownMEP]                      = SOC_PPC_TRAP_CODE_OAM_CPU;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamUpMEP]                           = SOC_PPC_TRAP_CODE_OAM_CPU_MIRROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamUpMEP2]                          = SOC_PPC_TRAP_CODE_OAM_OAMP_MIRROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamUpMEP3]                          = SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamUpMEP4]                          = SOC_PPC_TRAP_CODE_OAM_CPU_FREE_UP;

    /*Jericho Ingress Traps*/
    _bcm_rx_trap_to_ppd[bcmRxTrapInnerIpCompMcInvalidIp]             = SOC_PPC_TRAP_CODE_INNER_IP_COMP_MC_INVALID_IP_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapIllegalReservedLabel]               = SOC_PPC_TRAP_CODE_MPLS_ILLEGAL_LABEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapInnerMyMacAndIpDisabled]            = SOC_PPC_TRAP_CODE_INNER_ETHERNET_MY_MAC_IP_DISABLED;

    _bcm_rx_trap_to_ppd[bcmRxTrapEgPortNotVlanMember]                = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_VSI_MEMBERSHIP);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgHairPinFilter]                    = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_HAIR_PIN);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgSplitHorizonFilter]               = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_SPLIT_HORIZON);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgUnknownDa]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_UNKNOWN_DA);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgMtuFilter]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_MTU_VIOLATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgPvlanFilter]                      = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_PRIVATE_VLAN);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgDiscardFrameTypeFilter]           = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_ACC_FRM);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTrillHairPinFilter]               = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_TRILL_SAME_INTERFACE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTrillBounceBack]                  = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_TRILL_BOUNCE_BACK);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTrillTtl0]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_TRILL_TTL_0);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpmcTtlErr]                       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_TTL_SCOPE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgCfmAccelerated]                   = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_CFM_TRAP);
    _bcm_rx_trap_to_ppd[bcmRxTrapDssStacking]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_DSS_STACKING);
    _bcm_rx_trap_to_ppd[bcmRxTrapLagMulticast]                       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_LAG_MULTICAST);
    _bcm_rx_trap_to_ppd[bcmRxTrapExEgCnm]                            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_CNM_PACKET);

    /*arad only:*/
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillDesignatedVlanNoMymac]         = SOC_PPC_TRAP_CODE_TRILL_DISABLE_BRIDGE_IF_DESIGNATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapAdjacentCheckFail]                  = SOC_PPC_TRAP_CODE_TRILL_NO_ADJACENT;
    _bcm_rx_trap_to_ppd[bcmRxTrapFcoeSrcIdMismatchSa]                = SOC_PPC_TRAP_CODE_FCOE_SRC_SA_MISMATCH;
    _bcm_rx_trap_to_ppd[bcmRxTrapSipMove]                            = -1;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamY1731MplsTp]                     = SOC_PPC_TRAP_CODE_TRAP_Y1731_O_MPLS_TP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamY1731Pwe]                        = SOC_PPC_TRAP_CODE_TRAP_Y1731_O_PWE;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdIpv4]                         = SOC_PPC_TRAP_CODE_TRAP_BFD_O_IPV4;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdMpls]                         = SOC_PPC_TRAP_CODE_TRAP_BFD_O_MPLS;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdPwe]                          = SOC_PPC_TRAP_CODE_TRAP_BFD_O_PWE;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdCcMplsTp]                     = SOC_PPC_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdCvMplsTp]                     = SOC_PPC_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamLevel]                           = SOC_PPC_TRAP_CODE_OAM_LEVEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamPassive]                         = SOC_PPC_TRAP_CODE_OAM_PASSIVE;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdIpv6]                         = SOC_PPC_TRAP_CODE_TRAP_BFD_O_IPV6;
    _bcm_rx_trap_to_ppd[bcmRxTrap1588]                               = SOC_PPC_TRAP_CODE_1588_PACKET_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabelIndexNoSupport]            = SOC_PPC_TRAP_CODE_MPLS_P2P_MPLSX4;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv4Client]                       = SOC_PPC_TRAP_CODE_DHCP_CLIENT;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv4Server]                       = SOC_PPC_TRAP_CODE_DHCP_SERVER;
    _bcm_rx_trap_to_ppd[bcmRxTrapEtherIpVersion]                     = SOC_PPC_TRAP_CODE_ETHER_IP_VERSION_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapEgressObjectAccessed]               = SOC_PPC_TRAP_CODE_FEC_ENTRY_ACCESSED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpv6MldReportDone]                = SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpv6MldUndefined]                 = SOC_PPC_TRAP_CODE_ICMPV6_MLD_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapFailover1Plus1Fail]                 = SOC_PPC_TRAP_CODE_LIF_PROTECT_PATH_INVALID;

    _bcm_rx_trap_to_ppd[bcmRxTrapEgInvalidDestPort]                  = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4VersionError]                 = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_VERSION_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4ChecksumError]                = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_CHECKSUM_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4HeaderLengthError]            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_HEADER_LENGTH_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4TotalLengthError]             = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_TOTAL_LENGTH_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4Ttl0]                         = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IP_TTL0);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4HasOptions]                   = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IP_HAS_OPTIONS);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4Ttl1]                         = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IP_TTL1);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4SipEqualDip]                  = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_SIP_EQUAL_DIP);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4DipZero]                      = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_DIP_ZERO);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4SipIsMc]                      = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV4_SIP_IS_MC);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6UnspecifiedDestination]       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_UNSPECIFIED_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LoopbackAddress]              = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_LOOPBACK_ADDRESS);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6MulticastSource]              = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_MULTICAST_SOURCE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6UnspecifiedSource]            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_UNSPECIFIED_SOURCE);

    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalLinkDestination]         = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_LINK_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalSiteDestination]         = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_SITE_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalLinkSource]              = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_LINK_SOURCE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalSiteSource]              = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_SITE_SOURCE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6Ipv4CompatibleDestination]    = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_IPV4_COMPATIBLE_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6Ipv4MappedDestination]        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_IPV4_MAPPED_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6MulticastDestination]         = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_MULTICAST_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6NextHeaderNull]               = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPC_TRAP_EG_TYPE_IPV6_HOP_BY_HOP);

    /* ETPP traps */
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop0]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop1]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_1);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop2]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_2);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop3]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_3);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop4]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_4);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop5]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_5);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop6]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_6);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop7]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_7);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop8]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_8);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop9]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_9);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop10]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_10);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop11]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_11);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop12]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_12);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop13]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_13);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop14]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_14);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxFieldSnoop15]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_15);
    _bcm_rx_trap_to_ppd[bcmRxTrapOutVPortDiscard]                    = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_OUT_VPORT_DISCARD);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxStpStateFail]                   = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_STP_STATE_FAIL);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxProtectionPathUnexpected]       = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_PROTECTION_PATH_UNEXPECTED);
    _bcm_rx_trap_to_ppd[bcmRxTrapOutVPortLookupFail]                 = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_VPORT_LOOKUP_FAIL);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxMtuFilter]                      = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_MTU_FILTER);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxDiscardFrameTypeFilter]         = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_ACC_FRAME_TYPE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTxSplitHorizonFilter]             = _BCM_TRAP_ETPP_TO_ID(SOC_PPC_TRAP_CODE_ETPP_SPLIT_HORIZON);

    _bcm_rx_trap_to_ppd[bcmRxTrapUserDefine]                         = SOC_PPC_TRAP_CODE_USER_DEFINED_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapReserved]                           = -1; 
    _bcm_rx_trap_to_ppd[bcmRxTrapFcoeZoneCheckFail]                  = SOC_PPC_TRAP_CODE_FCOE_FCF_FLP_LOOKUP_FAIL; /* for all fcf lookup fail zone + forwarding*/
    _bcm_rx_trap_to_ppd[bcmRxTrapArplookupFail]                      = SOC_PPC_TRAP_CODE_ARP_FLP_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2cpPeer]                           = SOC_PPC_TRAP_CODE_ETH_L2CP_PEER;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2cpDrop]                           = SOC_PPC_TRAP_CODE_ETH_L2CP_DROP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimMyBmacMulticastContinue]         = SOC_PPC_TRAP_CODE_MY_B_MAC_MC_CONTINUE;
}


STATIC sal_thread_t rx_poll_tid;

#if (defined BCM_PETRA_SUPPORT) || (defined BCM_PETRA_SUPPORT)

/*
 * Function:
 *      bcm_petra_rx_sched_register
 * Purpose:
 *      Rx scheduler registration function. 
 * Parameters:
 *      unit       - (IN) Unused. 
 *      sched_cb   - (IN) Rx scheduler routine.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_rx_sched_register(int unit, bcm_rx_sched_cb sched_cb)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_sched_register(unit, sched_cb));
     } 
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_rx_sched_unregister
 * Purpose:
 *      Rx scheduler de-registration function. 
 * Parameters:
 *      unit  - (IN) Unused. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_rx_sched_unregister(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_sched_unregister(unit));
     } 
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_rx_unit_next_get
 * Purpose:
 *      Rx started units iteration routine.
 * Parameters:
 *      unit       - (IN)  BCM device number. 
 *      unit_next  - (OUT) Next attached unit with started rx.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_rx_unit_next_get(int unit, int *unit_next)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_unit_next_get(unit, unit_next));
     } 
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN; 
}

int 
bcm_petra_rx_channels_running(
    int unit, 
    uint32 *channels)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_channels_running(unit, channels));
     } 
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;        
}

/* Function:    bcm_petra_rx_active
 * Purpose:     Indicates if RX is active on this unit
 * Parameters:  unit (IN)   - Device Number
 * Returns:     1 if RX is running, 0 otherwise
 */
int
bcm_petra_rx_active(int unit)
{
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT

    if (SOC_IS_ARAD(unit)) {
         rv = _bcm_common_rx_active(unit);
         BCM_RETURN_VAL_EXIT(rv);
     } 
#endif     
exit:
    BCMDNX_FUNC_RETURN;        
}

/* Function:    bcm_petra_rx_alloc
 * Purpose:     Allocate an RX buffer using configured allocator
 * Parameters:  unit (IN)       - Device Number
 *              pkt_size (IN)   - size of packet to allocate
 *              flags (IN)      - flags passed to allocator
 *              buf (OUT)       - allocated buffer
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_alloc(int unit, int pkt_size, uint32 flags, void **buf)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         rv = _bcm_common_rx_alloc(unit,pkt_size,flags, buf);
         BCM_RETURN_VAL_EXIT(rv);
     } 
#endif
    
    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);

exit:
    BCMDNX_FUNC_RETURN;        
}

/* Function:    bcm_petra_rx_cfg_get
 * Purpose:     Returns the current RX configuration
 * Parameters:  unit (IN)       - Device Number
 *              cfg (OUT)       - Current RX configuration
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cfg_get(int unit, bcm_rx_cfg_t *cfg)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_cfg_get(unit,cfg));
     } 
#endif
   
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cfg_init
 * Purpose:     Initialize user level RX configuration to default values
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cfg_init(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        /*    BCMDNX_IF_ERR_EXIT(_bcm_common_rx_cfg_init(unit)); */
        RX_INIT_CHECK(unit);
        if (RX_UNIT_STARTED(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("RX already started on unit:%d \n"), unit));
        } 
        sal_memcpy(&rx_ctl[unit]->user_cfg, &_rx_arad_dflt_cfg, sizeof(bcm_rx_cfg_t));
    } 
#endif    
  
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_free
 * Description: 
 * Parameters:  unit (IN)       - Device number
 *              pkt_data (IN)   - pointer to packet data
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_free(int unit, void *pkt_data)
{ 
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_free(unit,pkt_data));
     } 
#endif

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_rx_free_queued
 * Purpose:     Free all buffers listed in pending free list 
 * Parameters:  unit (IN)       - Device Number
 */
void
_bcm_petra_rx_free_queued(int unit)
{
    void *free_list, *next_free;

    /* Steal list of pkts to be freed for unit */
    RX_INTR_LOCK;
    free_list = (bcm_pkt_t *)rx_ctl[unit]->free_list;
    rx_ctl[unit]->free_list = NULL;
    RX_INTR_UNLOCK;

    while (free_list) {
        next_free = DPP_RX_FREE_LIST_NEXT(free_list);
        bcm_petra_rx_free(unit, free_list);
        free_list = next_free;
    }
}

/* Function:    _bcm_petra_rx_queue_cleanup
 * Purpose:     Remove all packets in queue
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
void
_bcm_petra_rx_queue_cleanup(int unit)
{
    rx_queue_t *queue;

    /* empty the queue first */
    _bcm_petra_rx_free_queued(unit);

    /* mark the queue as empty */
    queue = DPP_RX_QUEUE(unit);
    if (!_Q_EMPTY(queue)) {
        queue->count = 0;
        queue->head = NULL;
        queue->tail = NULL;
    }
}
/* Function:    _bcm_petra_rx_cleanup
 * Purpose:     Remove all packets in queue and cleanup resources
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
void
_bcm_petra_rx_cleanup(int unit)
{
    int i;
    bcm_pkt_t *pkt_block = NULL;
    bcm_pkt_t *pkt = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    
    /* cleanup the queue */
    _bcm_petra_rx_queue_cleanup(unit);

    /* Packets are allocated in a single contiguous block, packet index 0
     * points to the single block, don't free each individual packet
     */
    if (rx_ctl[unit]->all_pkts && rx_ctl[unit]->all_pkts[0]) {
        sal_memset(rx_ctl[unit]->all_pkts[0], 0, RX_PPC(unit) * sizeof(bcm_pkt_t));
    }

    pkt_block = rx_ctl[unit]->all_pkts[0];

    for (i=0; i < RX_PPC(unit); i++) {
        pkt = DPP_RX_PKT(unit, i) = pkt_block++;            
        pkt->unit = unit;
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
        pkt->_idx = i;
    }

    if (rx_ctl[unit]->pkt_load_ids) {
        sal_memset(rx_ctl[unit]->pkt_load_ids, 0, RX_PPC(unit) * sizeof(int));
    }    

    BCMDNX_FUNC_RETURN_VOID;
}

/*
 * Function:
 *      _bcm_petra_rx_shutdown
 * Purpose:
 *      Shutdown threads, free up resources without touching
 *      hardware
 * Parameters:
 *      unit - Unit reference
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_petra_rx_shutdown(int unit)
{
    if (0 == RX_IS_SETUP(unit)) {
        return (BCM_E_NONE);
    }

    /* Packets are allocated in a single contiguous block, packet index 0
     * points to the single block, don't free each individual packet
     */
    if (rx_ctl[unit]->all_pkts) {
        BCM_FREE(DPP_RX_PKT(unit, 0));
        BCM_FREE(rx_ctl[unit]->all_pkts);
        rx_ctl[unit]->all_pkts = NULL;            
    }

    if (rx_ctl[unit]->pkt_load_ids) {
        BCM_FREE(rx_ctl[unit]->pkt_load_ids);
        rx_ctl[unit]->pkt_load_ids = NULL;
    }

    /* cleanup dynamically allocated resources */
    if (rx_ctl[unit]->pkt_queue) {
        BCM_FREE(rx_ctl[unit]->pkt_queue);
        rx_ctl[unit]->pkt_queue = NULL;
    }
    
    if (rx_ctl[unit]->rc_callout) {
        BCM_FREE(rx_ctl[unit]->rc_callout);
        rx_ctl[unit]->rc_callout = NULL;
    }

    if (rx_ctl[unit]->rx_mutex) {
        sal_mutex_destroy(rx_ctl[unit]->rx_mutex);
        rx_ctl[unit]->rx_mutex = NULL;
    }
    BCM_FREE(rx_ctl[unit]);
    rx_ctl[unit] = NULL;
    
    /* free up the rx pool */
    bcm_rx_pool_cleanup();
    
    return BCM_E_NONE;
}

/*
 * Function:    bcm_petra_rx_stop
 * Purpose:     Stop RX for the given unit; saves current configuration
 * Parameters:  unit (IN)   - device number top stop RX on
 *              cfg (IN)    - OUT Configuration copied to this parameter
 * Returns:     BCM_E_XXX
 * Notes:       This signals the thread to exit if this is the only unit 
 *              on which RX is started.
 */
int
bcm_petra_rx_stop(int unit, bcm_rx_cfg_t *cfg)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);   

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_stop(unit, cfg));
     }
#endif
   
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_clear
 * Purpose:     Stop RX on all modules and cleanup any resources
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_clear(unit));

         /* free up the rx pool */
         bcm_rx_pool_cleanup();
     } 
#endif

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_dpp_rx_tm_snoop_trap_reserve
 * Purpose
 *      Reserve the traps for ITMH snoop parsing in Arad
 * Parameters
 *      (in)  unit          = unit number
 *      (in)  num_snoops    = number of allocated snoop commands (and traps)
 * Returns
 *      rv = error code.
 *                    
 */
STATIC int _bcm_dpp_rx_tm_snoop_trap_reserve(
            int unit,
            uint8 is_set, /* 0: get function, 1: set function */
            int *num_snoops)
{
    int rv= BCM_E_NONE;
    int snoop_ndx;
    int trap_id;
    bcm_rx_trap_config_t config_get, config_set;
    int trap_id_sw;
    int trap_code_converted;
    BCMDNX_INIT_FUNC_DEFS;

    if (num_snoops == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL input pointer num_snoops")));
    }
    
    if(is_set && (*num_snoops > _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Too large required number (%d) of ITMH snoop commands to reserve."), *num_snoops));         
    }

    if (is_set == 0) {
        /* For get function, assume the maximum */
        *num_snoops = _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT;
    }

    /* Reserve / free the respective trap and snoop commands */
    for (snoop_ndx = 0; snoop_ndx < _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT; snoop_ndx++) {
        /* Reserve always the last traps */
        trap_id = SOC_PPC_TRAP_CODE_USER_DEFINED_LAST - _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT + 1 + snoop_ndx;

        /* Build the trap configuration */
        bcm_rx_trap_config_t_init(&config_set);
        config_set.snoop_cmnd = snoop_ndx;
        config_set.snoop_strength = PPC_TRAP_MGMT_SNP_STRENGTH_MAX;

        /*
         * See if the trap exists
         * If so, compare if same configuration, otherwise return error
         */
        if (_bcm_dpp_am_trap_is_alloced(unit, trap_id) == BCM_E_EXISTS) {
            bcm_rx_trap_config_t_init(&config_get);

            trap_id_sw = trap_id; 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
            rv = bcm_petra_rx_trap_get(unit,  trap_code_converted, &config_get);
            BCMDNX_IF_ERR_EXIT(rv);
            /* Assumption that the config-port or config-group will be changed if allocated */
            if ((config_get.dest_group != 0) || (config_get.dest_port != 0) || (config_get.trap_strength != 0)) {
                if (is_set == 0) {
                    /* For get function, 1st trap not TM */
                    *num_snoops = snoop_ndx;
                    break;
                }
                else {
                    /* Error in set only if under the required number of snoops */
                    if (snoop_ndx < (*num_snoops)) {
                        LOG_ERROR(BSL_LS_BCM_RX,
                                  (BSL_META_U(unit,
                                              "Unit %d: Error (%s) Try to reserve a TM snoop traps for snoop %d "
                                              "and trap-id %d when already allocated \n"), 
                                   unit, bcm_errmsg(rv), snoop_ndx, trap_code_converted));
                    }
                }
            }
            else { /* TM configuration on this snoop */
                if (is_set && (snoop_ndx >= (*num_snoops))) {
                    /* Free the TM configurations above the required number of snoops */
                    bcm_rx_trap_config_t_init(&config_set);
                    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
        else { /* trap not alloced */
            if (is_set == 0) {
                /* For get function, 1st trap not TM */
                *num_snoops = snoop_ndx;
                break;
            }
            trap_id_sw = trap_id; 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));

            /* Allocate the trap when not already reserved */
            rv = bcm_petra_rx_trap_type_create(
                    unit, 
                    BCM_RX_TRAP_WITH_ID, 
                    bcmRxTrapUserDefine,
                    &trap_code_converted);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &config_set);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }


    
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


/* Function:    bcm_petra_rx_control_get
 * Description: Get the status of specified RX feature.
 * Parameters:  unit (IN)   - Device number
 *              type (IN)   - RX control parameter
 *              value (OUT) - Current value of control parameter
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_control_get(int unit, bcm_rx_control_t type, int *value)
{
    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(type);
    COMPILER_REFERENCE(value);

    switch (type) {
    case bcmRxControlTmSnoopCount:
        if (SOC_IS_ARAD(unit)) {
            int rv;
            int num_snoops = 0;
            rv = _bcm_dpp_rx_tm_snoop_trap_reserve(unit, FALSE /*is_set*/,  &num_snoops);
            BCMDNX_IF_ERR_EXIT(rv);
            *value = num_snoops;
        } else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_control_set
 * Description: Set the status of specified RX feature.
 * Parameters:  unit (IN)   - Device number
 *              type (IN)   - RX control parameter
 *              value (IN)  - Value of control parameter to set to
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_control_set(int unit, bcm_rx_control_t type, int value)
{
    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(type);
    COMPILER_REFERENCE(value);

    switch (type) {
    case bcmRxControlTmSnoopCount:
        if (SOC_IS_ARAD(unit)) {
            int num_snoops = value;
            int rv;
            rv = _bcm_dpp_rx_tm_snoop_trap_reserve(unit, TRUE /*is_set*/,  &num_snoops);
            BCMDNX_IF_ERR_EXIT(rv);
        } else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cosq_mapping_get
 * Description: Get the COS Queue mapping details for the index specified
 * Parameters:  unit (IN)           - Device number
 *              index (IN)          - Index into COSQ mapping table 
 *                                      0 - lowest match priority
 *              reasons (OUT)       - packet "reasons" bitmap
 *              reasons_mask (OUT)  - mask for packet "reasons" bitmap
 *              int_prio (OUT)      - internal priority value
 *              int_prio_mask (OUT) - mask for internal priority value
 *              packet_type (OUT)   - packet type bitmap (BCM_RX_COSQ_PACKET_TYPE_*)
 *              packet_type_mask(OUT)- mask for packet type bitmap
 *              cosq (OUT)          - CPU cos queue
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cosq_mapping_get(int unit, int index, bcm_rx_reasons_t *reasons, 
                              bcm_rx_reasons_t *reasons_mask, 
                              uint8 *int_prio, uint8 *int_prio_mask,
                              uint32 *packet_type, uint32 *packet_type_mask,
                              bcm_cos_queue_t *cosq)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_cosq_mapping_get is not available yet")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cosq_mapping_set
 * Description: Set the COS Queue mapping details for the specified index
 * Parameters:  unit (IN)           - Device number
 *              index (IN)          - Index into COSQ mapping table
 *                                      0 - lowest match priority
 *              reasons (IN)        - packet "reasons" bitmap
 *              reasons_mask (IN)   - mask for packet "reasons" bitmap
 *              int_prio (IN)       - internal priority value
 *              int_prio_mask (IN)  - mask for internal priority value
 *              packet_type (IN)    - packet type bitmap (BCM_RX_COSQ_PACKET_TYPE_*)
 *              packet_type_mask(IN)- mask for packet type bitmap
 *              cosq (IN)           - CPU cos queue
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cosq_mapping_set(int unit, int index, bcm_rx_reasons_t reasons, 
                              bcm_rx_reasons_t reasons_mask, 
                              uint8 int_prio, uint8 int_prio_mask,
                              uint32 packet_type, uint32 packet_type_mask,
                              bcm_cos_queue_t cosq)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_cosq_mapping_set is not available yet")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cpu_rate_get
 * Description: Get the CPU RX rate in packets per second
 * Parameters:  unit (IN)   - Device number
 *              pps (OUT)   - Rate in packets per second
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cpu_rate_get(int unit, int *pps)
{
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
  
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_cpu_rate_get(unit, pps));
    } 
#endif
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_free_enqueue
 * Purpose:     Queue a packet to be freed by the RX thread.
 * Parameters:  unit (IN)   - Unit reference
 *              pkt (IN)    - pointer to packet data to be enqueued for free
 * Returns:     BCM_E_XXX
 * Notes:       This may be called in interrupt context to queue a packet to 
 *              be freed later.
 *              Assumes pkt_data is 32-bit aligned. Uses the first word of 
 *              the freed data as a "next" pointer for the free list.
 */
int
bcm_petra_rx_free_enqueue(int unit, void *pkt_data)
{
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_free_enqueue(unit,pkt_data));
     }  
#endif
 
exit:
    BCMDNX_FUNC_RETURN;
}


/* Function:    bcm_petra_rx_init
 * Purpose:     Initialize RX module on the specified unit
 * Parameters:  unit (IN)   - Unit reference
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_init(int unit)
{
    int rv;
    int mc_id;  
    int lag_mc_trap_id;
    int trap_id_converted;    
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    bcm_rx_trap_config_t lag_mc_trap;
    bcm_rx_trap_core_config_t dests_arr[MAX_NUM_OF_CORES_EGRESS_ACTION_PROFILE];
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO default_info;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO trap_info;
    uint8 is_allocated;
    int cores_num = SOC_DPP_DEFS_GET(unit, nof_cores);
    int core_lcl;
    int nof_pkts;


    BCMDNX_INIT_FUNC_DEFS;
    
    if (!SOC_WARM_BOOT(unit)) {
        BCMDNX_IF_ERR_EXIT(RX_ACCESS.is_allocated(unit, &is_allocated));
        if(!is_allocated) {
            BCMDNX_IF_ERR_EXIT(RX_ACCESS.alloc(unit));
        }
    }

    BCM_DPP_UNIT_CHECK(unit);
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        if (!bcm_rx_pool_setup_done()) {
            nof_pkts = soc_property_get(unit, spn_RX_POOL_NOF_PKTS, BCM_RX_POOL_COUNT_DEFAULT);
            BCMDNX_IF_ERR_EXIT(bcm_rx_pool_setup(nof_pkts, BCM_RX_POOL_BYTES_DEFAULT + sizeof(void *)));
        }
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_init(unit));
     } 
#endif    

    /* A.M initliaztion - Start*/
    _bcm_dpp_rx_trap_to_ppd_init(unit);
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);

    soc_sand_dev_id = (unit);
    if (!SOC_WARM_BOOT(unit)) {
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPC_TRAP_EG_TYPE_ALL,SOC_PPC_TRAP_EG_NO_ACTION);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /*set invalid out tm port to discard pkts*/
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT,SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /*points all ports to default profile= NO ACTION*/
    /*SW config*/

    rv = _bcm_dpp_am_template_trap_egress_init(unit,_BCM_DPP_TRAP_EG_NO_ACTION, &default_info);/*profile = 0, trap-info = default */   
    BCMDNX_IF_ERR_EXIT(rv);
     
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /*
         * Allocate the last 16 User-defined traps for TM Snoop field parsing:
         * - The Arad.ITMH has a snoop field that must be mapped through PMF
         * - the PMF maps this field to a snooop command (8b) that is mapped to 
         * a snoop code (4b) afterwards
         * - These snoop codes are in the same space than the traps, thus these
         * user-defined traps have an invalid snoop mapping
         * - Thus we allocate the 16 last User-defined traps for TM 
         * at init. The user can reduce the size with bcmRxControlTmSnoopCount
         */ 
        if (!SOC_WARM_BOOT(unit)) {
            int num_snoops = _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT;
            rv = _bcm_dpp_rx_tm_snoop_trap_reserve(unit, TRUE /*is_set*/,  &num_snoops);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

#endif /* BCM_ARAD_SUPPORT */


    /* Do not Change!
     * Capture a User Defined SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS trap
     * Trap destination is to the recycle and is used for IPv6-UC-with-RPF FLP Program
     * Since FLP is initialized first need to make sure to capture this trap code before the application 
     */ 
    if (SOC_DPP_PP_ENABLE(unit)) 
    {
        if (!SOC_WARM_BOOT(unit))
        {
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists) 
            {
                int trap_id_sw;
                int trap_code;

                trap_id_sw = SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code));
                /*
                 * Capture the SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS trap 
                 */

                rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code);
                if (rv != BCM_E_NONE) 
                {
                    /* Trap is already captured */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("IPv6 UC with RPF 2pass trap:0x% already captured, error %d unit %d"), trap_code,rv,unit));
                }
            }
        }
    }

   if (!SOC_WARM_BOOT(unit))
   {
        /* allocating a trap for outbound mirroring/recycle*/  
        if(_BCM_RX_EXPOSE_HW_ID(unit))   
        {  
         trap_id_converted = 205;  
        }  
       
        else  
        {  
            trap_id_converted = SOC_PPC_TRAP_CODE_USER_DEFINED_27;   
        }  
        /* allocate user define trap */  
         BCMDNX_IF_ERR_EXIT(bcm_petra_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_id_converted)); 
        SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);  

        if(SOC_IS_JERICHO(unit))  
        {  
            /* set the trap to Multicast */  
            mc_id = (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - 1);  
            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, &(trap_info.dest_info.frwrd_dest), mc_id, soc_sand_rv);   
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);  
        }  
        else  
        {  
            /* set the trap to DROP */  
            trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);  
            trap_info.dest_info.frwrd_dest.dest_id = 0;  
            SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);  
        }  

        {
            trap_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;  
            /* this function expects the SW trap id*/
            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_27 /* SW id */,&trap_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);  
        }

        /* Configure Trap for Lag Pruning */
        BCMDNX_IF_ERR_EXIT(bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapLagMulticast, &lag_mc_trap_id));
        bcm_rx_trap_config_t_init(&lag_mc_trap);
        lag_mc_trap.flags |= (BCM_RX_TRAP_UPDATE_DEST); 
        lag_mc_trap.trap_strength = 7;
        if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) 
        {
            for ( core_lcl = 0 ; core_lcl < cores_num; ++core_lcl)
            {
                dests_arr[core_lcl].dest_port =  BCM_GPORT_BLACK_HOLE;
            }
      
            lag_mc_trap.core_config_arr = dests_arr;
            lag_mc_trap.core_config_arr_len = cores_num;
        }
        else
        {
            lag_mc_trap.dest_port=BCM_GPORT_BLACK_HOLE;
        }
        BCMDNX_IF_ERR_EXIT(bcm_petra_rx_trap_set(unit, lag_mc_trap_id, &lag_mc_trap));
   }


exit:
   BCMDNX_FUNC_RETURN;
}
    
/* Function:    bcm_petra_rx_detach
 * Purpose:     Detach RX module on the specified unit
 * Parameters:  unit (IN)   - Unit reference
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_FUNC_RETURN;
}
    

/* Function:    bcm_petra_rx_partial_init
 * Purpose:     Initialize RX module on the specified unit without thread creating 
 * Parameters:  unit (IN)   - Unit reference
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_partial_init(int unit)
{
    int rv;
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO default_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(RX_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(RX_ACCESS.alloc(unit));
    }

    soc_sand_dev_id = (unit);

    /* A.M initliaztion - Start*/
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);

    soc_sand_dev_id = (unit);
    if (!SOC_WARM_BOOT(unit)) {
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPC_TRAP_EG_TYPE_ALL,SOC_PPC_TRAP_EG_NO_ACTION);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /*set invalid out tm port to discard pkts*/
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT,SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /*points all ports to default profile= NO ACTION*/
    /*SW config*/
    rv = _bcm_dpp_am_template_trap_egress_init(unit,_BCM_DPP_TRAP_EG_NO_ACTION, &default_info);/*profile = 0, trap-info = default */   
    BCMDNX_IF_ERR_EXIT(rv);

exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_queue_max_get
 * Purpose:     Get maximum cos queue number for the device.
 * Parameters:  unit (IN)   - device number. 
 *              cosq (OUT)  - Maximum queue priority.
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_queue_max_get(int unit, bcm_cos_queue_t *cosq)
{
    BCMDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_max_get(unit, cosq));
    } 
#endif
   
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_queue_packet_count_get
 * Purpose:     Get number of packets awaiting processing in the specific 
 *              device/queue.
 * Parameters:  unit (IN)           - device number. 
 *              cosq (IN)           - Queue priority
 *              packet_count (OUT)  - Number of packets awaiting processing
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_queue_packet_count_get(int unit, bcm_cos_queue_t cosq, 
                                    int *packet_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_packet_count_get(unit, cosq, packet_count));
    }  
#endif    
   

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_rate_get
 * Purpose:     Get the configured CPU RX rate in pps
 * Parameters:  unit (IN)   - device number. 
 *              pps (OUT)   - RX rate in packets per second rate
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_rate_get(int unit, int *pps)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_rate_get(unit, pps));
     } 
#endif
 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_reasons_policer_set
 * Purpose:     Set the policer for the specified rx reasons
 * Parameters:  unit (IN)       - device number. 
 *              rx_reasons(IN)  - RX reasons for which to set the policer
 *              pol_id (IN)     - Policer ID to set to
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_reasons_policer_set(int unit, bcm_rx_reasons_t reasons, 
                                 bcm_policer_t pol_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_reasons_policer_set is not available yet")));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:    bcm_petra_rx_register
 * Purpose:     Register an upper layer driver
 * Parameters:  unit (IN)   - device number.
 *              name (IN)   - constant character string for debug purposes.
 *              cb_f (IN)   - callback function when packet arrives
 *              pri (IN)    - priority of handler in list (0:lowest priority)
 *              cookie (IN) - cookie passed to driver when packet arrives.
 *              flags (IN)  - Register for interrupt or non-interrupt callback
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_register(int unit, const char *name, bcm_rx_cb_f cb_f, uint8 pri, 
                      void *cookie, uint32 flags)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT

    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_register(unit, name, cb_f, pri, cookie, flags));
     }  
#endif
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:    bcm_petra_rx_unregister
 * Purpose:     Un-register an upper layer driver
 * Parameters:  unit (IN)   - device number.
 *              cb_f (IN)   - callback function to unregister
 *              pri (IN)    - priority of handler in list (0:lowest priority)
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_unregister(int unit, bcm_rx_cb_f cb_f, uint8 pri)
{

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_unregister(unit, cb_f, pri));
     } 
#endif
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

#if defined(BROADCOM_DEBUG)
/*
 * Function:    bcm_petra_rx_show
 * Purpose:     Un-register an upper layer driver
 * Parameters:  unit (IN)   - device number.
 *              cb_f (IN)   - callback function to unregister
 *              pri (IN)    - priority of handler in list (0:lowest priority)
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_show(int unit)
{
    volatile rx_callout_t   *rco;
    sal_usecs_t             cur_time;

    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT(_bcm_common_rx_show(unit));
        BCM_EXIT;   
    } 

      
    BCM_DPP_UNIT_CHECK(unit);
    if (!RX_INIT_DONE(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "Unit:%d RX not initialized. \n"), unit));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("TODO err message")));
    }

    cur_time = sal_time_usecs();
    LOG_CLI((BSL_META_U(unit,
                        "RX Info @ time=%u: %sstarted. Thread is %srunning.\n"
             "    +verbose for more info\n"),cur_time, 
             RX_UNIT_STARTED(unit) ? "" : "not ",
             rx_control.thread_running ? "" : "not "));
    LOG_CLI((BSL_META_U(unit,
                        "    Pkt Size %d. All COS PPS %d \n"),
             RX_PKT_SIZE(unit), RX_PPS(unit)));
    LOG_CLI((BSL_META_U(unit,
                        "    Cntrs:  Pkts %d. Last start %d. Tunnel %d. Owned %d.\n"
             "        Bad Hndlr %d. No Hndlr %d. Not Running %d.\n"
                        "        Thrd Not Running %d.\n"),
             rx_ctl[unit]->tot_pkts, rx_ctl[unit]->pkts_since_start,
             rx_ctl[unit]->tunnelled, rx_ctl[unit]->pkts_owned,
             rx_ctl[unit]->bad_hndlr_rv, rx_ctl[unit]->no_hndlr,
             rx_ctl[unit]->not_running, rx_ctl[unit]->thrd_not_running));

    LOG_CLI((BSL_META_U(unit,
                        "  Registered callbacks:\n")));
    /* Display callouts and priority in order */
    for (rco = rx_ctl[unit]->rc_callout; rco; rco = rco->rco_next) {
        LOG_CLI((BSL_META_U(unit,
                            "        %-10s Priority=%3d%s. "
                 "Argument=0x%x. COS 0x%x%08x.\n"),
                 rco->rco_name, (uint32)rco->rco_priority,
                 (rco->rco_flags & BCM_RCO_F_INTR) ? " Interrupt" : "",
                 PTR_TO_INT(rco->rco_cookie),
                 rco->rco_cos[1], rco->rco_cos[0]));
        LOG_CLI((BSL_META_U(unit,
                            "        %10s Packets handled %u, owned %u.\n"), " ",
                 rco->rco_pkts_handled, rco->rco_pkts_owned));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BROADCOM_DEBUG */

/*
 * Function:    _bcm_petra_rx_thread_pkt_process
 * Purpose:     Process a single RX packet. Registered callbacks are called
 * Parameters:  param (IN)  - device number
 *              pkt (IN)    - packet structure to process
 */
void
_bcm_petra_rx_thread_pkt_process(int unit, bcm_pkt_t *pkt)
{
    volatile rx_callout_t   *rco;
    bcm_rx_t                handler_rc;
    int                     handled;

    if (pkt == NULL) {
        return;
    }
    if (rx_ctl[unit]->hndlr_cnt == 0) {
        /* No callbacks...free the packet and return */
        rx_ctl[unit]->no_hndlr++;
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX thread Error. No handlers, returning\n"), unit));
        bcm_petra_rx_free(unit, pkt->pkt_data->data);
        pkt->pkt_data->data = NULL;
        DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        return;
    }

    handled = FALSE;
    RX_LOCK(unit);  /* Can't modify handler list while doing callbacks */
    /* Loop through registered drivers until packet consumed */
    for (rco = rx_ctl[unit]->rc_callout; rco; rco = rco->rco_next) {
        if (rco->rco_flags & BCM_RCO_F_INTR) { /* Non interrupt context */
            continue;
        }
        handler_rc = rco->rco_function(unit, pkt, rco->rco_cookie);

        switch (handler_rc) {
        case BCM_RX_NOT_HANDLED:
            break;                      /* Next callout */
        case BCM_RX_HANDLED:
            handled = TRUE;
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d pkt handled "
                                    "by %s\n"), unit, rco->rco_name));
            rco->rco_pkts_handled++;
            break;
        case BCM_RX_HANDLED_OWNED:
            handled = TRUE;
            pkt->_pkt_data.data = NULL;
            pkt->alloc_ptr = NULL;
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d pkt handled "
                                    "and owned by %s\n"), unit, rco->rco_name));
            rx_ctl[unit]->pkts_owned++;
            rco->rco_pkts_owned++;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d RX packet invalid callback return value=%d\n"), 
                                  unit, handler_rc));
            break;
        }

        if (handled) {
            break;
        }
    }
    RX_UNLOCK(unit);

    if (pkt->pkt_data->data) {
        bcm_petra_rx_free(unit, pkt->pkt_data->data);
        pkt->pkt_data->data = NULL;
    }
    DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);

    if (!handled) {
        /* Internal error as discard should have handled packet */
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX Error, no handler processed packet. Port %d\n"),
                              unit, pkt->rx_port));
    }
}

/*
 * Function:    _bcm_petra_rx_thread_pkts_process
 * Purpose:     Handles the RX pkt queue on the unit
 * Parameters:  param (IN)  - device number
 * Returns:     BCM_E_XXX
 * Notes:       Processes all the packets in the queue.
 */
int
_bcm_petra_rx_thread_pkts_process(int unit)
{
    int         count;
    rx_queue_t  *queue;
    bcm_pkt_t   *pkt_list = NULL;
    bcm_pkt_t   *next_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    queue = DPP_RX_QUEUE(unit);
    if (queue == NULL) {
        BCM_EXIT; /* Should not hit this. Some internal error */
    }
    count = queue->count;
    if (count == 0) {
        BCM_EXIT;  /* queue empty */
    }
    _Q_STEAL(queue, pkt_list, count);
    if (pkt_list == NULL) {
        /* no packets. but queue count does not say so? */
        BCM_EXIT;   
    }
    /* Process all packets */
    while (pkt_list) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "Unit:%d RX dequeued "
                                "packet (%p)\n"), unit, pkt_list));
        next_pkt = pkt_list->_next;

        /* No rate limiting imposed */
        _bcm_petra_rx_thread_pkt_process(unit, pkt_list);
        pkt_list = next_pkt;

        _BCM_RX_CHECK_THREAD_DONE;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Sleep .01 seconds when running. */
#define RUNNING_SLEEP 10000

/* Sleep .5 seconds when not running. */

/* Check if given quantity is < cur sleep secs; set to that value if so */
#define BASE_SLEEP_VAL RUNNING_SLEEP

/* Set sleep to base value */
#define INIT_SLEEP    rx_control.sleep_cur = BASE_SLEEP_VAL

/* Sleep .1 seconds before polling again */
#define RX_POLL_SLEEP 100000

/* sleep .05 seconds before checking for another packet again */
#define RX_POLL_WAIT_SLEEP 50000

/* sleep for 0.01 seconds before retrying for buffer available */
#define RX_POLL_WAIT_BUFFER 10000

/* Lower sleep time if val is < current */
#define SLEEP_MIN_SET(val)                                           \
    (rx_control.sleep_cur = ((val) < rx_control.sleep_cur) ?         \
     (val) : rx_control.sleep_cur)

#define CHECK_RX_THREAD_DONE    \
    if (!rx_control.thread_running) goto rx_thread_done

/*
 * Function:    _bcm_petra_rx_pkt_thread
 * Purpose:     User level thread that handles packets
 * Parameters:  param (IN)  - unused
 * Notes:   
 *      This thread can get awoken for the following reasons:
 *      1.  Packets ready to process
 *      2.  RX stop is called ending processing
 *      3.  Interrupt processing has handled packets 
 *      4.  Periodic check in case memory has freed up
 */
STATIC void
_bcm_petra_rx_pkt_thread(void *param)
{
    int unit;
    COMPILER_REFERENCE(param);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX Packet thread starting\n")));

    INIT_SLEEP;

    /* Sleep on sem */
    while (rx_control.thread_running) {
        CHECK_RX_THREAD_DONE;
        for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
            if (!RX_UNIT_STARTED(unit)) {
                continue;
            }
            CHECK_RX_THREAD_DONE;

            while (_bcm_petra_rx_thread_pkts_process(unit)) {
                CHECK_RX_THREAD_DONE;

                /* free pkt data on the free list */
                _bcm_petra_rx_free_queued(unit);
            }

            /* Free queued packets */
            _bcm_petra_rx_free_queued(unit);
        }

        SLEEP_MIN_SET(BASE_SLEEP_VAL);
        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX thread Sleeping %d "
"at %u\n"), rx_control.sleep_cur, sal_time_usecs()));
    
        sal_sem_take(rx_control.pkt_notify, rx_control.sleep_cur);
        rx_control.pkt_notify_given = FALSE;

        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX thread Woke %u\n"),
sal_time_usecs()));
        INIT_SLEEP;
    }

rx_thread_done:
    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
        if (RX_IS_SETUP(unit)) {
            _bcm_petra_rx_cleanup(unit);
        }
    }
    rx_control.thread_exit_complete = TRUE;
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX: Packet thread exitting\n")));

    sal_thread_exit(0);
}


/* Function:    _bcm_petra_rx_init_buffer_mgt
 * Purpose:     Initialize the pkt buffer pool
 * Parameters:  unit (IN)   - device number
 * Returns:     BCM_E_XXX
 * Notes:       Should only be called once...If not, overrides current buffer
 */
int
_bcm_petra_rx_init_buffer_mgt(int unit)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_pkt_t *pkt_block = NULL;
    bcm_pkt_t *pkt = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    RX_LOCK(unit);
    if (BCM_SUCCESS(rv)) {
        BCMDNX_ALLOC(rx_ctl[unit]->all_pkts, RX_PPC(unit) * sizeof(bcm_pkt_t*), "Rx pkt array");
        if (rx_ctl[unit]->all_pkts == NULL) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Failed(%s) to alloc packet descriptors\n"), 
                                  unit, bcm_errmsg(rv)));
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        BCMDNX_ALLOC(rx_ctl[unit]->pkt_load_ids, RX_PPC(unit) * sizeof(int), "RxPktLoadIDs");
        if (rx_ctl[unit]->pkt_load_ids == NULL) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Failed(%s) to alloc RX load IDs\n"), 
                                  unit, bcm_errmsg(rv)));
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        BCMDNX_ALLOC(pkt_block, RX_PPC(unit) * sizeof(bcm_pkt_t), "Rx pkt block");
        if (pkt_block == NULL) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d failed to alloc packet descriptor block\n"), 
                                  unit));
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_FAILURE(rv)) {
        BCM_FREE(rx_ctl[unit]->all_pkts);
        BCM_FREE(rx_ctl[unit]->pkt_load_ids);

        RX_UNLOCK(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    sal_memset(pkt_block, 0, RX_PPC(unit) * sizeof(bcm_pkt_t));
    /* pkt_load_ids is being used to keep track of pkt_block usage */
    sal_memset(rx_ctl[unit]->pkt_load_ids, 0, RX_PPC(unit) * sizeof(int));
    rx_ctl[unit]->all_pkts[0] = pkt_block; /* to appease coverity */

    for (i=0; i < RX_PPC(unit); i++) {
        pkt = DPP_RX_PKT(unit, i) = pkt_block++;            
        pkt->unit = unit;
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
        pkt->_idx = i;
    }

    RX_UNLOCK(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_rx_check_buffer_level
 * Purpose:     Checks the buffer pool and allocates if needed.
 * Parameters:  unit (IN)   - device number
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_check_buffer_level(int unit)
{
    int rv = BCM_E_NONE;
    bcm_pkt_t *pkt = NULL;
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    /* allocate packet descriptors and packet buffers for RX */
    if (rx_ctl[unit]->all_pkts == NULL) {
        rv = _bcm_petra_rx_init_buffer_mgt(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    RX_LOCK(unit);
    /* fill in packet data buffers if needed */
    for (i=0; i < RX_PPC(unit); i++) {
        void *pkt_data = NULL;
        pkt = DPP_RX_PKT(unit, i);

        if ((pkt->pkt_data->data == NULL) && (DPP_RX_PKT_USED(unit, i) == 0)) {
            rv = bcm_petra_rx_alloc(unit, rx_ctl[unit]->user_cfg.pkt_size, 
                                    0, &pkt_data);

            if ((pkt_data == NULL) || (BCM_FAILURE(rv))) {
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "Unit:%d Failed (%s) to allocate pkt buffer\n"), 
                                      unit, bcm_errmsg(rv)));
                RX_UNLOCK(unit);
                BCM_RETURN_VAL_EXIT(BCM_E_MEMORY);
            }

            pkt->_pkt_data.data = pkt_data;
            pkt->_pkt_data.len  = rx_ctl[unit]->user_cfg.pkt_size;
            pkt->alloc_ptr = pkt_data;
        }
    }

    RX_UNLOCK(unit);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_rx_pkt_buffer_get
 * Purpose:     Returns a bcm_pkt_t buffer
 * Parameters:  unit (IN)   - device number
 *              pkt (out)   - ptr to ptr to pkt structure
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_pkt_buffer_get(int unit, bcm_pkt_t **p_pkt)
{
    int rv, idx;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(p_pkt);

    rv = _bcm_petra_rx_check_buffer_level(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d Error (%s) filling RX buffers\n"), 
                              unit, bcm_errmsg(rv)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = BCM_E_RESOURCE;
    RX_LOCK(unit);
    for (idx=0; ((idx < RX_PPC(unit)) && (BCM_FAILURE(rv))); idx++) {
        if (DPP_RX_PKT_USED(unit, idx) == 0) {
            /* bcm_pkt_t struct is free. return it */
            *p_pkt = DPP_RX_PKT(unit, idx);
            DPP_RX_PKT_USED(unit, idx) = 1;
            rv = BCM_E_NONE;
        }
    }
    RX_UNLOCK(unit);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    _bcm_petra_rx_pkt_available
 * Purpose:     Check if a packet is in CPU RX path for Async interface
 * Parameters:  param (IN)  - unused
 * Returns:     1 if pkt is to be RX'ed from HW, 0 otherwise
 */
int
_bcm_petra_rx_pkt_available(int unit, uint32 *val)
{
    int     avail = 0;

#if defined (BCM_88650_A0)
    /* assume not available */
    avail = 0;
    
    if (val == NULL) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d Internal error. Null input param \n"), unit));
        return avail;
    }
    
#endif /*defined(BCM_88640_A0)*/
    return avail;
}

/*
 * Function:    _bcm_petra_rx_pkt_enqueue
 * Purpose:     Grab a packet destined for CPU RX from chip's async interface
 * Parameters:  param (IN)  - unused
 */
void
_bcm_petra_rx_pkt_enqueue(int unit, uint32 regval)
{
#if defined(BCM_88640_A0)

    int                         rv=0, eop, tcmd, tdv, num_bytes, tdata;
    int                         soc_sand_rv, hdr_len, check_mc;
    int                         hdr_len_read, is_mc, retry_count;
    bcm_pkt_t                   *pkt = NULL;
    rx_queue_t                  *queue;
    SOC_TMC_STACK_GLBL_INFO          stk_info;
    SOC_TMC_PORTS_FTMH_EXT_OUTLIF   ext_info = SOC_TMC_PORTS_FTMH_NOF_EXT_OUTLIFS;
    uint8                       buff[2048]; 
    int                         buff_len = 0;
    SOC_PPC_TRAP_PACKET_INFO        packet_info;
    bcm_module_t                mod_id;
    bcm_port_t                  mod_port;
    bcm_pkt_t                   dnx_pkt;
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d RX enqueuing "
                            "a packet\n"), unit));

    retry_count = 0;
    while ((retry_count++ < DPP_RX_BUF_CHECK_RETRY_COUNT)) {
        rv = _bcm_petra_rx_pkt_buffer_get(unit, &pkt);
        if (BCM_SUCCESS(rv)) {
            break;
        }
        sal_usleep(RX_POLL_WAIT_BUFFER);
    }
    if (BCM_FAILURE(rv) || (pkt == NULL)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX buffer pool full. \n"), unit));
        return;
    }

    /* FTMH base header is always present */
    hdr_len = DPP_HDR_FTMH_BASE_LEN;

    /* check if OUTLIF extension header is present */    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_ftmh_extension_get,(unit, &ext_info)));
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d failed to get FTMH outlif extension info. Error:%d "
                              "(%s) \n"), unit, rv, bcm_errmsg(rv)));
        DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        return;
    }

    /* Not really needed. Just to avoid coverity defect. */
    if (unit >= BCM_LOCAL_UNITS_MAX) {
        return;
    }

    is_mc = 0;
    if (ext_info == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC) {
        check_mc = 1;
    } else {
        /* SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER & 
           SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS */
        check_mc = 0;
        if (ext_info == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS) {
            hdr_len += 2;
        }
    }
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "FTMH Outlif ext: %d \n"), 
                            ext_info));

    SOC_TMC_STACK_GLBL_INFO_clear(&stk_info);

    /* check if LB-Key extension header is present */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_stack_global_info_get,(unit, &stk_info)));
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d failed to get FTMH LB-key ext header info. Error:%d"
                              " (%s) \n"), unit, rv, bcm_errmsg(rv)));
        DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        return;
    }

    if ((stk_info.max_nof_tm_domains == SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_8) || 
        (stk_info.max_nof_tm_domains == SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16)) {
        /* LB extension is enabled when 8 or 16 TM domains are present */
        hdr_len += 2;
    }
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "FTMH hdr len:%d \n"), hdr_len));

    
    /* following code is for Asynchronous CPU RX interface */
    eop = 0;
    pkt->tot_len = 0;
    pkt->pkt_len = 0;
    hdr_len_read = 0;
    while ((!eop) && (BCM_SUCCESS(rv))) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "Regval: %08x \n"), regval));
        tcmd = soc_reg_field_get(unit, CPUASYNCHRONOUSPACKETDATAr, regval,
                                 TCMDf);
        tdv = soc_reg_field_get(unit, CPUASYNCHRONOUSPACKETDATAr, regval, 
                                TDVf);
        if (tdv) {
            num_bytes = 2;
            if (tcmd == DPP_RX_CPU_ASYNC_TCMD_EOP_1BV) {
                num_bytes = 1;
                eop = 1;
            } else if (tcmd == DPP_RX_CPU_ASYNC_TCMD_EOP_2BV) {
                eop = 1;
            }
        } else {
            /* some error ? */
            eop = 1;
            num_bytes = 0;
            rv = BCM_E_INTERNAL;
        }
        if (num_bytes) {
            tdata = soc_reg_field_get(unit, CPUASYNCHRONOUSPACKETDATAr, 
                                      regval, TDATAf);
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Packet content: "
                                    "%04x. num_bytes:%d \n"), tdata, num_bytes));
                                                 
            sal_memcpy((buff + buff_len), 
                           (((char *)(&tdata)) + 2), num_bytes);
            buff_len += num_bytes;

            if (hdr_len_read < hdr_len) {
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "Header:%04x "
                                        "hdr_len_read:%d \n"), tdata, hdr_len_read));
                if (num_bytes < 2) {
                    LOG_ERROR(BSL_LS_BCM_RX,
                              (BSL_META_U(unit,
                                          "Unit:%d Error rx'ing packet. Packet size"
                                          "< FTMH header length \n"), unit));
                    rv = BCM_E_INTERNAL;
                } else {
                    sal_memcpy((pkt->_dpp_hdr + hdr_len_read), 
                               (((char *)(&tdata)) + 2), num_bytes);
                    hdr_len_read += num_bytes;
                    if (check_mc &&
                        (hdr_len_read == DPP_HDR_FTMH_BASE_LEN)) {
                        /* check if packet is system multicast */
                        if (tdata & 0x1) {
                            /* SYS_MC, FTMH-OUTLIF(CUD) ext hdr present */
                            hdr_len += 2;
                            is_mc = 1;
                        }
                    }
                }
            } else {
                sal_memcpy((pkt->_pkt_data.data + pkt->pkt_len), 
                           (((char *)(&tdata)) + 2), num_bytes);
                pkt->pkt_len += num_bytes;
            }
        }
        if (!eop) {
            rv = READ_CPUASYNCHRONOUSPACKETDATAr(unit, &regval);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "Unit:%d RX poll thread failed (%s) to read "
                                      "CPUASYNCHRONOUSPACKETDATA reg\n"), unit, bcm_errmsg(rv)));
            }
        }
    }
    {
    int rc = soc_ppd_trap_packet_parse(unit, buff, buff_len, &packet_info, (soc_pkt_t *)&dnx_pkt);
    if (rc < 0) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "RX buffer translate err. \n")));
        rv = BCM_E_INTERNAL;
    }
    }
    if (BCM_SUCCESS(rv)) {
        switch (hdr_len) {
        case DPP_HDR_FTMH_BASE_LEN:
            pkt->_dpp_hdr_type = DPP_HDR_ftmh_base;
            break;
        case DPP_HDR_FTMH_OUTLIF_EXT_LEN: /* or DPP_HDR_FTMH_MC_LB_EXT_LEN */
            if (check_mc && is_mc) {
                pkt->_dpp_hdr_type = DPP_HDR_ftmh_mc_lb_ext;
            } else {
                pkt->_dpp_hdr_type = DPP_HDR_ftmh_outlif_ext;
            }
            break;
        case DPP_HDR_FTMH_OUTLIF_MC_LB_EXT_LEN:
            pkt->_dpp_hdr_type = DPP_HDR_ftmh_outlif_mc_lb_ext;
            break;
        default:
            /* should not hit this */
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Invalid header length (%d) on RX'ed packet. \n"), 
                                  unit, hdr_len));
            DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
            return;
        }

        /* Remove PPH from pkt_len */
        pkt->pkt_len -= (packet_info.ntwrk_header_ptr - hdr_len);
        pkt->tot_len = pkt->pkt_len + hdr_len;
        sal_memcpy((pkt->_pkt_data.data ), 
                   (((char *)(buff)) + packet_info.ntwrk_header_ptr), 
                   pkt->pkt_len);
        pkt->_pkt_data.len = pkt->pkt_len;
        rv = _bcm_dpp_port_mod_port_from_sys_port(unit, &mod_id, &mod_port, 
                                             packet_info.src_sys_port);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d _bcm_dpp_port_mod_port_from_sys_port failed"), unit));
            return;
        }
        pkt->src_mod = mod_id;
        pkt->src_port = mod_port;
        pkt->rx_unit = unit;
        pkt->rx_port = pkt->src_port;
        pkt->rx_reason = packet_info.cpu_trap_code;

        queue = DPP_RX_QUEUE(unit);
        if (queue == NULL) {
            /* Should not hit this. Some internal error */
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Invalid RX queue state. \n"), unit));
            DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        } else {
            rx_ctl[unit]->tot_pkts++;
            rx_ctl[unit]->pkts_since_start++;

            _Q_ENQUEUE_LOCK(queue, pkt);

            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d RX enqueued "
                                    "a packet (%p) successfully\n"), unit, pkt));
            RX_THREAD_NOTIFY(unit);
        }
    }
#endif /*defined(BCM_88640_A0)*/
}
/*
 * Function:    _bcm_petra_rx_poll_pkt_thread
 * Purpose:     User level thread that polls for packets
 * Parameters:  param (IN)  - unused
 * Notes:       This thread wakes up periodically and polls for any available 
 *              packets in the CPU RX path. Packets are pushed into queue to 
 *              be picked up by RX thread.
 *              This RX Poll thread exit is controlled by RX thread controls 
 *              (rx_control.thread_running)
 */
#define CHECK_RX_POLL_THREAD_DONE    \
    if (!rx_control.thread_running) goto rx_poll_thread_done

STATIC void
_bcm_petra_rx_poll_pkt_thread(void *param)
{
    int     unit;
    uint32  val;
    COMPILER_REFERENCE(param);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX packet poll thread "
"starting\n")));

    while (rx_control.thread_running) {
        CHECK_RX_POLL_THREAD_DONE;
        for (unit = 0; unit < BCM_LOCAL_UNITS_MAX; unit++) {
            if (!RX_UNIT_STARTED(unit)) {
                continue;
            }
            CHECK_RX_POLL_THREAD_DONE;

                while (_bcm_petra_rx_pkt_available(unit, &val)) {
                CHECK_RX_POLL_THREAD_DONE;
               
                sal_usleep(RX_POLL_WAIT_SLEEP);
            }
        }

        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX poll thread Sleeping"
" %d at %u\n"), RX_POLL_SLEEP, sal_time_usecs()));
        sal_usleep(RX_POLL_SLEEP);
        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX poll thread Woke up"
"at %u\n"), sal_time_usecs()));
        INIT_SLEEP;
    }

rx_poll_thread_done:
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX: Packet poll thread exitting\n")));
    sal_thread_exit(0);
}


/*
 * Function:    _bcm_petra_rx_thread_start
 * Purpose:     Start the RX thread and RX polling thread when using 
 *              Async CPU interface
 * Parameters:  unit (IN)   - device number
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_thread_start(int unit)
{
    int priority;

    BCMDNX_INIT_FUNC_DEFS;
    /* Timer/Event semaphore thread sleeping on. */
    if (rx_control.pkt_notify == NULL) {
        rx_control.pkt_notify = sal_sem_create("RX pkt ntfy", sal_sem_BINARY, 
                                               0);
        if (rx_control.pkt_notify == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
        rx_control.pkt_notify_given = FALSE;
    }

    /* RX start/stop on one of the units protection mutex. */
    if (rx_control.system_lock == NULL) {
        rx_control.system_lock = sal_mutex_create("RX system lock");
        if (rx_control.system_lock == NULL) {
            sal_sem_destroy(rx_control.pkt_notify);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
    }

    priority = rx_ctl[unit]->rx_thread_pri;

    /* Start rx thread. */
    rx_control.rx_tid = sal_thread_create("bcmRX", SAL_THREAD_STKSZ,
                                     priority, _bcm_petra_rx_pkt_thread, NULL);

    /* Start rx polling thread */
    rx_poll_tid = sal_thread_create("bcmRxPoll", SAL_THREAD_STKSZ,
                                priority, _bcm_petra_rx_poll_pkt_thread, NULL);

    /* Thread creation error handling. */
    if ((rx_control.rx_tid == NULL) || (rx_poll_tid == NULL)) {
        sal_sem_destroy(rx_control.pkt_notify);
        sal_mutex_destroy(rx_control.system_lock);
        rx_control.pkt_notify = NULL;
        rx_control.system_lock = NULL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    _bcm_petra_rx_user_cfg_check
 * Purpose:     Validate and adjust user specified cfg to reasonable values
 * Parameters:  unit (IN)   - device number
 */
void
_bcm_petra_rx_user_cfg_check(int unit)
{
    if (RX_PPS(unit) < 0) {
        RX_PPS(unit) = 0;
    }

    
}



/*
 * Function:    bcm_petra_rx_start
 * Purpose:     Starts bcm_rx thread for Soc_petra device. Allocates packet 
 *              descriptors, packet buffers, and initializes HW to recveive
 *              data into buffers.
 * Parameters:  unit (IN)   - device number
 *              cfg (IN)    - rx config parameters, if null, default is used
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT(_bcm_common_rx_start(unit, cfg));  
        BCMDNX_IF_ERR_EXIT(soc_dpp_arad_dma_init(unit));              
    } 
#endif
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get the PPD Trap code according to the BCM Trap code
 */
int _bcm_dpp_rx_ppd_trap_get(int unit,
                                 bcm_rx_trap_t trap_type,
                                 int *soc_ppd_trap_id){

     BCMDNX_INIT_FUNC_DEFS;

    *soc_ppd_trap_id = _bcm_rx_trap_to_ppd[trap_type];

    /* Jericho only ingress traps*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) 
    {
        switch (*soc_ppd_trap_id )
        {
            case SOC_PPC_TRAP_CODE_INNER_IP_COMP_MC_INVALID_IP_0:
                *soc_ppd_trap_id = -1;
                break;
            case SOC_PPC_TRAP_CODE_MPLS_ILLEGAL_LABEL:
                *soc_ppd_trap_id = -1;
                break;
            case SOC_PPC_TRAP_CODE_INNER_ETHERNET_MY_MAC_IP_DISABLED:
                *soc_ppd_trap_id = -1;
                break;
        }
    }

    /* Jericho and above only ETPPcint traps */
    if ( SOC_IS_ARADPLUS_AND_BELOW(unit) && _BCM_TRAP_ID_IS_ETPP(*soc_ppd_trap_id)) 
    {
                *soc_ppd_trap_id = -1;
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("ETPP traps are available for Jericho and above only")));
    }
exit:
    BCMDNX_FUNC_RETURN;

}

/*
 * Function:    _bcm_dpp_rx_trap_type_get_from_array
 * Purpose:     Gets the bcm trap type according to the ppd trap code, using the mapping array. 
 *  
 * Parameters: 
 *              soc_ppd_trap_id - soc ppd trap code to be tested.
 *              trap_type       - will be filled with the bcm trap code.
 *              found           - Indication if the trap code was found.
 * Returns:     BCM_E_XXX
 */
void
_bcm_dpp_rx_trap_type_get_from_array(int soc_ppd_trap_id, bcm_rx_trap_t *trap_type, uint8 *found){
    bcm_rx_trap_t trap_type_index;

    *found = FALSE;

    for (trap_type_index = 0; trap_type_index < sizeof(_bcm_rx_trap_to_ppd) / sizeof(SOC_PPC_TRAP_CODE); trap_type_index ++) {
        if (_bcm_rx_trap_to_ppd[trap_type_index] == soc_ppd_trap_id) {
            *trap_type = trap_type_index;
            *found = TRUE;
            break;
        }
     }
}

/*
 * Get the BCM Trap type according to the PPD Trap ID
 */
void _bcm_dpp_rx_trap_type_get(int unit, 
                               int soc_ppd_trap_id, 
                               uint8 *found,
                               bcm_rx_trap_t *trap_type)
{
    bcm_rx_trap_t trap_type_index = 0;
    int soc_ppd_trap_id_cmp = soc_ppd_trap_id;
    int root_trap_id;

    /* Map all user defined trap codes to bcmRxTrapUserDefine */
    if (SOC_IS_ARAD(unit)) {
        if ((SOC_PPC_TRAP_CODE_USER_DEFINED_0 <= soc_ppd_trap_id) && (soc_ppd_trap_id <= _BCM_LAST_USER_DEFINE)) {
            soc_ppd_trap_id_cmp = SOC_PPC_TRAP_CODE_USER_DEFINED_0;
        }
        else if (((SOC_PPC_TRAP_CODE_PROG_TRAP_0 <= soc_ppd_trap_id) && (soc_ppd_trap_id <= SOC_PPC_TRAP_CODE_PROG_TRAP_3))
                 || ((SOC_PPC_TRAP_CODE_RESERVED_MC_0 <= soc_ppd_trap_id) && (soc_ppd_trap_id <= SOC_PPC_TRAP_CODE_RESERVED_MC_7))) {
            soc_ppd_trap_id_cmp = _BCM_TRAP_CODE_VIRTUAL_BASE;
        }
    }

    _bcm_dpp_rx_trap_type_get_from_array(soc_ppd_trap_id_cmp, &trap_type_index, found);
    if (*found) {
        *trap_type = trap_type_index;
        return;
    }
    
	 /* There is no Trap type (BCM name) for this trap;
	    Looking for a root trap mate that has a trap type assigned 
	 */ 
	 if(!(*found)) {
        root_trap_id = _bcm_dpp_trap_root_mate_get(unit, soc_ppd_trap_id_cmp);
        if (root_trap_id >= 0) {
           for (trap_type_index = 0; trap_type_index < sizeof(_bcm_rx_trap_to_ppd) / sizeof(SOC_PPC_TRAP_CODE); trap_type_index ++) {
              if (_bcm_rx_trap_to_ppd[trap_type_index] == root_trap_id) {
                 *found = TRUE;
                 *trap_type = trap_type_index;
                 break;
              }
           }
		}
	 }	
}

int bcm_petra_rx_trap_type_from_id_get(
    int unit,
    int flags,
    int trap_id,
    bcm_rx_trap_t *trap_type)
{
/* parameter "flags" is reserved for future use */
	int sw_trap_id;
    uint8 found;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit, trap_id, &sw_trap_id));
    _bcm_dpp_rx_trap_type_get(unit, sw_trap_id, &found, trap_type);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("BCM trapType not found for the given trap_id %d\n"), sw_trap_id));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_rx_ppd_trap_code_from_trap_id(int unit, int trap_id, SOC_PPC_TRAP_CODE *ppd_trap_code) {
     bcm_dpp_rx_virtual_traps_t p_virtual_traps;
     int trap_id_sw; /*in case "use the hw id" soc is turned on - will be used to store the sw hw id */


     BCMDNX_INIT_FUNC_DEFS;

     BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit, trap_id, &trap_id_sw));
     if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw)) {
        if (_bcm_dpp_rx_virtual_trap_get(unit,_BCM_TRAP_ID_TO_VIRTUAL(trap_id_sw),&p_virtual_traps) != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed\n")));
        }

        if (p_virtual_traps.soc_ppc_trap_code != 0) {
            *ppd_trap_code = p_virtual_traps.soc_ppc_trap_code;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Expected trap_id %d to be mapped to a programable trap or reserved mc trap"), trap_id_sw));
        }
    } else {

        *ppd_trap_code = trap_id_sw;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_rx_trap_type_create(
    int unit, 
    int flags, 
    bcm_rx_trap_t trap_type,
    int *trap_id)
{
    int rv = BCM_E_NONE; 
    int epp =0;
    int alloc_flags=0;
    uint32 oamp_error_trap_id;
    int trap_id_sw; /*in case "use the hw id" soc is turned on - will be used to store the sw hw id */

    BCMDNX_INIT_FUNC_DEFS;

    if(trap_id == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trap_id  pointer is NULL"))); 
    }
    if (flags & BCM_RX_TRAP_WITH_ID)
    {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit, *trap_id, &trap_id_sw));
    }
    else
    {
            trap_id_sw = *trap_id;
    }


    /*Check if enum is valid and in range*/
    if(trap_type >= bcmRxTrapCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trap_type not valid"))); 
    }

    if ((trap_type >= bcmRxTrapOampTrapErr) && (trap_type <= bcmRxTrapOampProtection)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}

        /* OAMP trap - taken from a separate pool, must be created with_id */
        if (!(flags & BCM_RX_TRAP_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OAMP Error traps must be created with WITH_ID flag"))); 
        }
        if (!_BCM_TRAP_ID_IS_OAM(trap_id_sw)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OAMP Error traps must be in range 0x400-0x4FF"))); 
        }
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(trap_id_sw);
        rv = _bcm_dpp_oam_error_trap_allocate(unit, trap_type, oamp_error_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }


    if ((flags & BCM_RX_TRAP_WITH_ID) && ( (trap_type == bcmRxTrapUserDefine)||(trap_type == bcmRxTrapL2Cache) )) {
        alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        if (trap_type == bcmRxTrapL2Cache) {
            trap_id_sw = _BCM_TRAP_VIRTUAL_TO_ID(trap_id_sw);
        }
    }else {    
        alloc_flags = 0;
        rv = _bcm_dpp_rx_ppd_trap_get(unit, trap_type, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);

    }


    /*Check if trap type is valid*/
    if(trap_id_sw == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("trap_id value is -1"))); 
    }


    rv = _bcm_dpp_am_trap_alloc(unit,alloc_flags,trap_id_sw,&epp);
    /* In this case, if the trap was already defined the alloc manager returns BCM_E_RESOURCE */
    if (rv == BCM_E_RESOURCE && !(_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw))) 
    {
        if (!SOC_WARM_BOOT(unit))
        {
            /*
             * If this is a warm boot then allow usage of resources which have already been created.
             * Cancel the error flag.
             */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("The trap was already created. Please use bcm_rx_trap_type_destroy and then try again."))); 
        }
        else
        {
            LOG_INFO(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                              "Unit:%d, If this is a warm boot then allow usage of resources which have already been created\n"), 
                              unit));
            rv = BCM_E_NONE ;
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);

    if(trap_type == bcmRxTrapUserDefine)
    {
        /*User define allocation, without ID*/
         trap_id_sw = epp;           
    }else if (trap_type == bcmRxTrapL2Cache) {
        /*Set bcmRxTrapL2Cache trap id with bit 30 high*/
        trap_id_sw = _BCM_TRAP_VIRTUAL_TO_ID(epp);
    }  

    if ( (trap_type != bcmRxTrapL2Cache)) 
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, trap_id));
    }
    else
    {
        *trap_id  = trap_id_sw;
    }
                       
    BCM_EXIT; 
exit:

    BCMDNX_FUNC_RETURN;
}


/* Destroy user defined trap code. */
int bcm_petra_rx_trap_type_destroy(
    int unit,  
    int trap_id)
{
    int                                         elem=0,flags=0,ittr_counter,current_trap_id=0;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO      default_info;
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO         egrr_default_info;
    SOC_PPC_TRAP_ETPP_INFO                      default_etpp_trap_info;

    int                                         rv= BCM_E_NONE, virtual_trap_id;
    bcm_dpp_rx_virtual_traps_t                  virtual_traps_p;
    uint32                                      oamp_error_trap_id;
    int trap_id_sw; /*in case "use the hw id" soc is turned on - will be used to store the sw hw id */
    int trap_id_etpp;

    BCMDNX_INIT_FUNC_DEFS;

    if (_BCM_TRAP_ID_IS_OAM(trap_id)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(trap_id);
        rv = _bcm_dpp_oam_error_trap_destroy(unit, oamp_error_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit, trap_id, &trap_id_sw));

    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&default_info);
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&egrr_default_info);
    default_info.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    default_info.processing_info.frwrd_type = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
    unit = (unit);


    if (_BCM_TRAP_ID_IS_ETPP(trap_id_sw)) 
    {
        SOC_PPC_TRAP_ETPP_INFO_clear(&default_etpp_trap_info); 
        trap_id_etpp =  _bcm_dpp_rx_trap_id_to_etpp(trap_id_sw);
        BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_etpp_trap_set,(unit,trap_id_etpp,&default_etpp_trap_info)));

    }

    else if (!_BCM_TRAP_ID_IS_EGRESS(trap_id_sw)) {    
        if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw)) {
            virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id_sw);

            if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_get function returned error"))); 
            }
            if (virtual_traps_p.soc_ppc_trap_code != 0) {
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,virtual_traps_p.soc_ppc_trap_code,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                virtual_traps_p.soc_ppc_trap_code = 0;
                if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_set function returned error"))); 
                    }
                }
        }else
        {
            ittr_counter=0;
            current_trap_id = trap_id_sw;
            do { 
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,current_trap_id,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                /* Check if there are other actions*/
                rv = _bcm_dpp_trap_mate_get(unit, current_trap_id);
                if (rv != -1)
                {
                    /* Update trap id only if there is a mate.*/
                    current_trap_id = rv;
                }
                 ittr_counter++;

            } while ( (rv != -1) && (ittr_counter < MAX_ITTR_COUNT) );
            rv = BCM_E_NONE; /* clear rv */
            if(ittr_counter == MAX_ITTR_COUNT) {                
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Reached max iteration while destroying single instance trap : %d \n"), MAX_ITTR_COUNT));                                             
            }
        }
    } 
    else {
        /* Additional care for egress trap*/
        rv = _bcm_dpp_rx_egress_set(unit,_bcm_dpp_rx_trap_id_to_egress(trap_id_sw),&egrr_default_info);
        BCM_IF_ERROR_RETURN(rv);                
    }

    /* egress trap is traeted as single instance
       For ingress case:if there is a mate-> dealloc only the first trap_id */
    rv = _bcm_dpp_am_trap_dealloc(unit,flags,trap_id_sw,elem); 
    BCM_IF_ERROR_RETURN(rv);

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;

}

int bcm_petra_rx_trap_type_get(int unit, int flags, bcm_rx_trap_t type, int *trap_id)
{
    int trap_id_sw; /*in case "use the hw id" soc is turned on - will be used to store the sw hw id */
    BCMDNX_INIT_FUNC_DEFS;

    
    if(trap_id == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trap_id  pointer is NULL"))); 
    }

    /*Check if enum is valid and in range*/
    if(type >= bcmRxTrapCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type is not valid"))); 
    }


    if ((type >= bcmRxTrapOampTrapErr) && (type <= bcmRxTrapOampProtection))  /* When creating an OAM trap - the "BCM_RX_TRAP_WITH_ID" flag must be transfarred as a param.
              Since we don't support getting the trap type with the flag: "BCM_RX_TRAP_WITH_ID" - We don't support getting the type of an OAM trap.*/
    {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Getting the trap_id with the flag: BCM_RX_TRAP_WITH_ID is unsupported")));    
    }

    if (flags & BCM_RX_TRAP_WITH_ID)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Getting the trap_id with the flag: BCM_RX_TRAP_WITH_ID is unsupported")));    
    }
    else 
    {    
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_rx_ppd_trap_get(unit, type, &trap_id_sw));

        /*Check if trap type is valid*/
        if(trap_id_sw == -1) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("trap_id value is -1"))); 
        }

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw,trap_id));

    }

    /*Check if trap type is valid*/
    if(*trap_id == -1) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("trap_id value is -1"))); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* Set how to handle packets according to trap type. */
int bcm_petra_rx_trap_set(
    int unit, 
    int trap_id, 
    bcm_rx_trap_config_t *config)
{
    int                                        rv= BCM_E_NONE;
    int                                        virtual_trap_id;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO     prm_profile_info;
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO        prm_egr_profile_info,default_info;
    int                                        ittr_counter;

    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO     snoop_profile_info;
    SOC_PPC_TRAP_ETPP_INFO                     etpp_trap_info;
    bcm_dpp_rx_virtual_traps_t                 virtual_traps_p;
    uint32                                     oamp_error_trap_id;
    int                                        trap_id_sw; /*in case "use the hw id" soc is turned on - will be used to store the sw hw id */
    uint32                                     trap_fwd_offset_index; 
    uint32                                     out_lif_id;
    int                                        cores_num;
    int                                        core;
    uint32                                     pp_port;
    uint32                                     tm_port;
    int                                        dest_port;       
    int                                        encap_id;

    BCMDNX_INIT_FUNC_DEFS;

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL"))); 
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit, trap_id, &trap_id_sw));

    if (_BCM_TRAP_ID_IS_OAM(trap_id_sw)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}
		if (config->color || config->counter_num || config->dest_group || config->ether_policer_id || config->policer_id || config->prio || config->snoop_cmnd ||
			config->snoop_strength || config->trap_strength) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only dest_port field should be set for OAMP trap."))); 
		}
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(trap_id_sw);
        rv = _bcm_dpp_oam_error_trap_set(unit, oamp_error_trap_id, config->dest_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&prm_profile_info);
    prm_profile_info.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);   
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&prm_egr_profile_info); 
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_profile_info);
    SOC_PPC_TRAP_ETPP_INFO_clear(&etpp_trap_info); 

    /*If forwarding header and RX trap configured - its invalid config
     * since its not clear where the packet should be forwarded (to CPU or for example to IPv4 port) */
    if ( (config->flags & BCM_RX_TRAP_UPDATE_FORWARDING_TYPE)  &&
          (config->flags & BCM_RX_TRAP_TRAP)  )
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Invalid configuration trap forwarding")));
    }
    /* if BCM_RX_TRAP_UPDATE_FORWARDING_HEADER is set, the user specifies the forwarding offset */
    if ( (config->flags) & BCM_RX_TRAP_UPDATE_FORWARDING_HEADER )
    {
       BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_bcm_forwarding_header_to_ppd(config->forwarding_header, &trap_fwd_offset_index));
    }
    else /* default forwarding offset */
    {
        trap_fwd_offset_index = 0;
    }

    if (trap_fwd_offset_index != 0 || ((config->flags) & BCM_RX_TRAP_UPDATE_FORWARDING_HEADER)) {
        prm_profile_info.bitmap_mask  |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;
        prm_profile_info.processing_info.frwrd_offset_index = trap_fwd_offset_index;
    }
    
    if (BCM_E_EXISTS != _bcm_dpp_am_trap_is_alloced(unit,trap_id_sw)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trap id %d is in not allocated, on unit:%d \n"), trap_id_sw,unit));
    }

     if (( _BCM_TRAP_ID_IS_EGRESS(trap_id_sw) || _BCM_TRAP_ID_IS_ETPP(trap_id_sw))  && 
         ((config->flags & BCM_RX_TRAP_UPDATE_POLICER) ||
          (config->flags & BCM_RX_TRAP_UPDATE_ETHERNET_POLICER) ||
          (config->flags & BCM_RX_TRAP_UPDATE_COUNTER) ||
          (config->flags & BCM_RX_TRAP_DEST_MULTICAST) ||
          (config->flags & BCM_RX_TRAP_BYPASS_FILTERS) ||
          (config->flags & BCM_RX_TRAP_LEARN_DISABLE) ||
          (config->flags & BCM_RX_TRAP_WITH_ID) ||
          (config->flags & BCM_RX_TRAP_REPLACE) ||
          (config->flags & BCM_RX_TRAP_TRAP) ||
          (config->flags & BCM_RX_TRAP_UPDATE_ADD_VLAN)||
          (config->flags & BCM_RX_TRAP_UPDATE_FORWARDING_TYPE)||
          (config->flags & BCM_RX_TRAP_UPDATE_COUNTER_2))) {


         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Egress profile doesn't support those features")));
     }
    unit = (unit); 
    cores_num = SOC_DPP_DEFS_GET(unit, nof_cores);

    /*Insert snoop profile data*/
    snoop_profile_info.snoop_cmnd = config->snoop_cmnd;
    snoop_profile_info.strength = config->snoop_strength;  


    if (_BCM_TRAP_ID_IS_ETPP(trap_id_sw))
    {

        if(config->mirror_cmd == NULL)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL"))); 
        }

        etpp_trap_info.mirror_cmd = config->mirror_cmd->recycle_cmd;
        etpp_trap_info.mirror_strength = config->mirror_cmd->copy_strength;

        if (config->mirror_cmd->flags & BCM_MIRROR_OPTIONS_COPY_DROP ) 
        {
            etpp_trap_info.mirror_enable = 1;
        }
        else
        {
            etpp_trap_info.mirror_enable = 0;
        }

        etpp_trap_info.fwd_strength = config->trap_strength;

        if ((config->dest_port ==  BCM_GPORT_BLACK_HOLE) && (config->flags & BCM_RX_TRAP_UPDATE_DEST) )
        {
            etpp_trap_info.fwd_enable = 0;
        }
        else
        {
            etpp_trap_info.fwd_enable = 1;
        }
        trap_id_sw =  _bcm_dpp_rx_trap_id_to_etpp(trap_id_sw);
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_etpp_trap_set,(unit,trap_id_sw,&etpp_trap_info)));
        BCMDNX_IF_ERR_EXIT(rv);

    }

    else if (!_BCM_TRAP_ID_IS_EGRESS(trap_id_sw)) {

        /*User define traps must have trap_strength =0*/
        if ( (SOC_PPC_TRAP_CODE_USER_DEFINED_0 <= trap_id_sw) && (trap_id_sw <= _BCM_LAST_USER_DEFINE) ) { 
            if( config->trap_strength != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("User define traps must have trap_strength =0"))); 
            }

        }
        /*Ingress trap handle*/
        prm_profile_info.strength = config->trap_strength; 

        if (config->flags & (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_DEST_MULTICAST)) {
            prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            if (config->flags & BCM_RX_TRAP_DEST_MULTICAST) {
                prm_profile_info.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
                prm_profile_info.dest_info.frwrd_dest.dest_id = config->dest_group;
            }   
            else
            {
                rv = _bcm_dpp_gport_to_fwd_decision(unit, config->dest_port, &prm_profile_info.dest_info.frwrd_dest);
                BCMDNX_IF_ERR_EXIT(rv);
  
                /* if BCM_RX_TRAP_UPDATE_ENCAP_ID is set, outlif can be updated */
                if (config->flags & BCM_RX_TRAP_UPDATE_ENCAP_ID) 
                {
                    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST_OUTLIF;
  
                    if ( BCM_GPORT_IS_SET(config->encap_id)) 
                    {
                        _bcm_dpp_field_gport_to_global_lif_convert(unit, config->encap_id, 0 /*out lif */, &out_lif_id);
                    }
                    else
                    {
                        out_lif_id = config->encap_id;
                    }
  
                    prm_profile_info.dest_info.frwrd_dest.additional_info.outlif.val = out_lif_id;
                    prm_profile_info.dest_info.frwrd_dest.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;

                }

             }

        }

        if (config->flags & BCM_RX_TRAP_UPDATE_PRIO) {
             prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
            prm_profile_info.cos_info.tc = config->prio;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_COLOR) {
            prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP;
            prm_profile_info.cos_info.dp = config->color;
        }

        if (config->flags & BCM_RX_TRAP_BYPASS_FILTERS) {
            prm_profile_info.processing_info.is_control = TRUE;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_POLICER) {

            /* In Arad/Arad+ devices the meter-id cannot be updated due to an overlapping bit in IHB_FWD_ACT_PROFILE [0 - 255] FWD table. 
                        The overlap is between _ACT_METER_POINTER and FWD_ACT_METER_POINTER_UPDATE fields (bit 89).*/
            if (SOC_IS_ARADPLUS_AND_BELOW(unit))
            {
                uint32 maxMeterId = 0x4000;
                if(config->policer_id < maxMeterId)
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For A/A+ Meter id range is between 16384 - 32767 "))); 
                }
            }

            prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_METER_0;
            prm_profile_info.meter_info.meter_id = config->policer_id;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_ETHERNET_POLICER) {
            prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_POLICER;
            prm_profile_info.policing_info.ethernet_police_id = config->ether_policer_id;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_COUNTER) {
            prm_profile_info.bitmap_mask  |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_0;
            prm_profile_info.count_info.counter_id = config->counter_num;
        }
        if (config->flags & BCM_RX_TRAP_UPDATE_COUNTER_2) {
            prm_profile_info.bitmap_mask  |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_1;
            prm_profile_info.count_info.counter_id = config->counter_num;
        }    
        if (config->flags & BCM_RX_TRAP_LEARN_DISABLE) {
            /*There is no need for bitmap mask*/
            /*prm_profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;*/
            
            prm_profile_info.processing_info.enable_learning = FALSE;
        }    

        if (config->flags & BCM_RX_TRAP_WITH_ID) {
            /*!!! How should this be used ? */
        }    


        if (config->flags & BCM_RX_TRAP_TRAP) {
            /*!!! Should update a bitmap ?*/
            prm_profile_info.processing_info.is_trap = TRUE;
            /*config->dest_port = CPU*/
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_ADD_VLAN) {
            prm_profile_info.dest_info.add_vsi = TRUE;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_FORWARDING_TYPE) {
            prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_TYPE;
            _bcm_dpp_rx_forwarding_type_to_ppd(config->forwarding_type, &prm_profile_info.processing_info.frwrd_type);
        }


        if (config->flags & BCM_RX_TRAP_DROP_AND_LEARN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Ingress trap doesn't support BCM_RX_TRAP_DROP_AND_LEARN flag")));
        }

               

        /* Check if virtual bit is marked */
        if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw)) 
        {
        
            /*Clears virtual bit before allocation,range 0-11*/
            virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id_sw);
            /*Save config data in _rx_virtual_traps array*/

            /* get a local copy of rx virtual traps info */
            if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_get function failed"))); 
            }

            sal_memcpy(&(virtual_traps_p.trap_info), &prm_profile_info, sizeof(prm_profile_info));
            sal_memcpy(&(virtual_traps_p.snoop_info),&snoop_profile_info, sizeof(snoop_profile_info));

            /* update sw_db */
            if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_set function failed"))); 
            }

            if (virtual_traps_p.soc_ppc_trap_code != 0) {
                
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,virtual_traps_p.soc_ppc_trap_code,&prm_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                /*Connect snoop to trap*/
                rv =  soc_ppd_trap_snoop_profile_info_set(unit,virtual_traps_p.soc_ppc_trap_code,&snoop_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                
            }

        }
        else
        {
            /*Trap is not virtual*/
            ittr_counter = 0;
            do { 
                if (!SOC_WARM_BOOT(unit))
                {
                    rv =  soc_ppd_trap_frwrd_profile_info_set(unit,trap_id_sw,&prm_profile_info);
                    BCM_SAND_IF_ERR_EXIT(rv);

                    /*Connect snoop to trap*/
                    rv = soc_ppd_trap_snoop_profile_info_set(unit,trap_id_sw,&snoop_profile_info);
                    BCM_SAND_IF_ERR_EXIT(rv);
                }
                /*check if there are other actions*/
                rv = _bcm_dpp_trap_mate_get(unit, trap_id_sw);
                if (rv != -1)
                {
                    /*update trap id only if there is a mate.*/
                    trap_id_sw = rv;
                }


                ittr_counter++;

            } while ( (rv != -1) && (ittr_counter < MAX_ITTR_COUNT) );
            rv = BCM_E_NONE; /* clear rv */
            if(ittr_counter == MAX_ITTR_COUNT) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("ittr_counter reached its max value, on unit:%d \n"), unit));
            }
        }
    }
    else 
    { /* EGRESS */
        if (!SOC_IS_ARADPLUS_AND_BELOW(unit))
        {
            if (config->flags & (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID)) {
                /* Input validation: Multi core device must use config_arr*/
                if (cores_num > 1 && !(config->dest_port == BCM_GPORT_BLACK_HOLE /* Special case, see below */))
                {
                    if (config->core_config_arr_len != cores_num)
                    {
                          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Updating destination requires the use of core_config_arr per core")));
                    }
                    if(config->core_config_arr == NULL)
                    {
                          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("core_config_arr pointer is NULL")));         
                    }
                }
            }

            if (config->flags & BCM_RX_TRAP_UPDATE_DEST) 
            {
                prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
                prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_PP_DSP;
                prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_USE_ARR;

                /* Special case - drop situation - we allow the "old" use and set to drop on each core*/
                if (config->dest_port == BCM_GPORT_BLACK_HOLE)
                {
                    BCM_DPP_CORES_ITER(_SHR_CORE_ALL, core)
                    {
                        prm_egr_profile_info.out_tm_port_arr[core] =  SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID;
                    }
                }
                else
                {
                    BCM_DPP_CORES_ITER(_SHR_CORE_ALL, core)
                    {
                        /* 
                         *  In case of single core the destination port can come either from the core_config_arr index 0,
                         *    or from the config itself, which is the old method.
                         *  In case of multi core, we always take the destination from the array, and we above we validate
                         *    that it's defined.
                         */ 
                        dest_port = (config->core_config_arr_len > 0) ? config->core_config_arr[core].dest_port
                                      : config->dest_port;
                        _bcm_dpp_rx_fill_destination_info_egress_trap( unit,core, dest_port, &prm_egr_profile_info, &prm_profile_info);
                    }
                }
            }
             /* if BCM_RX_TRAP_UPDATE_ENCAP_ID is set, destination (other than DROP) and outlif can be updated */
            if (config->flags & BCM_RX_TRAP_UPDATE_ENCAP_ID) 
            {
                  prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD;
                  prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_USE_ARR;

                  BCM_DPP_CORES_ITER(_SHR_CORE_ALL, core)
                  {
                      /* 
                       *  In case of single core the encap id can come either from the core_config_arr index 0,
                       *    or from the config itself, which is the old method.
                       *  In case of multi core, we always take the destination from the array, and we above we validate
                       *    that it's defined.
                       */ 
                      encap_id = (config->core_config_arr_len > 0) ? config->core_config_arr[core].encap_id 
                          : config->encap_id;
                      if (BCM_GPORT_IS_SET(encap_id)) 
                      {
                          /* The encap id can be given either as a gport, or as an egress global lif. */
                          rv = _bcm_dpp_field_gport_to_global_lif_convert(unit, encap_id, 0 /*egress*/, &out_lif_id);
                          BCM_IF_ERROR_RETURN(rv);
                      }
                      else
                      {
                          out_lif_id = encap_id;
                      }
                      prm_egr_profile_info.header_data_arr[core].cud = out_lif_id;
                  }
              }
        }
         /* Arad plus and below*/
         else
         {
            if (config->flags & BCM_RX_TRAP_UPDATE_DEST) 
            {
                       prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
                       rv= _bcm_dpp_gport_to_fwd_decision(unit, config->dest_port, &prm_profile_info.dest_info.frwrd_dest);
                       BCM_IF_ERROR_RETURN(rv);

                       /* if BCM_RX_TRAP_UPDATE_ENCAP_ID is set, destination (other than DROP) and outlif can be updated */
                       if (config->flags & BCM_RX_TRAP_UPDATE_ENCAP_ID) 
                       {
                           prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD;

                           if (BCM_GPORT_IS_SET(config->encap_id)) 
                           {
                               _bcm_dpp_field_gport_to_global_lif_convert(unit, config->encap_id, 0 /*out lif */, &out_lif_id);
                           }
                           else
                           {
                               out_lif_id = config->encap_id;
                           }

                           /* getting the tm port from the gport */
                           rv = _bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit, config->dest_port, &pp_port, &tm_port,&core);
                           BCM_IF_ERROR_RETURN(rv);

                           prm_egr_profile_info.header_data.dsp_ptr = tm_port;
                           prm_egr_profile_info.out_tm_port = tm_port; 
                           prm_egr_profile_info.header_data.cud = out_lif_id ;                                               
                           /* updating the CUD value */
                           arad_pp_trap_eg_profile_info_hdr_data_to_cud(unit, prm_egr_profile_info.header_data, &prm_egr_profile_info.cud);

                       }
                       else
                       {  
                           if (prm_profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) {
                               BCMDNX_IF_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("overriding our port is not supported")));                
                           } else if (prm_profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
                               prm_egr_profile_info.out_tm_port = SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID;
                           } else 
                           {
                               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported. Has to be a Local port or Drop"))); 
                           }
                       }

               }
        }

        if (config->flags & BCM_RX_TRAP_DROP_AND_LEARN) {
             if (SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Egress trap doesn't support BCM_RX_TRAP_DROP_AND_LEARN flag, for DROP use BCM_RX_TRAP_UPDATE_DEST and dest_port BLACK_HOLE")));
             }
             prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
             prm_egr_profile_info.out_tm_port = SOC_PPC_TRAP_ACTION_PKT_DISCARD_AND_LEARN_ID;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_PRIO) {
             prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
             prm_egr_profile_info.cos_info.tc = config->prio;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_COLOR) {
            prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP;
            prm_egr_profile_info.cos_info.dp = config->color;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_QOS_MAP_ID) 
        {
            /* Updating the qos_map_id is unsupported in Arad and Arad +*/
            if (SOC_IS_ARADPLUS_AND_BELOW(unit))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_RX_TRAP_UPDATE_QOS_MAP_ID flag is unsupported for Arad Plus or Arad devices"))); 
            }
            prm_egr_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_COS_PROFILE;
            prm_egr_profile_info.header_data.cos_profile = config->qos_map_id;
        }

        /*Egress handle*/ 
        rv = _bcm_dpp_rx_egress_set(unit,_bcm_dpp_rx_trap_id_to_egress(trap_id_sw),&prm_egr_profile_info);               
        BCM_IF_ERROR_RETURN(rv); 
    }
      
    BCM_EXIT; 

exit:
    BCMDNX_FUNC_RETURN;

}

int bcm_petra_rx_trap_get(
    int unit, 
    int trap_id, 
    bcm_rx_trap_config_t *config)
{ 
    int                                         trap_id_sw; /*in case "use the hw id" soc is turned on - will be used to store the sw hw id */
    int                                         rv=BCM_E_NONE;
    int                                         virtual_trap_id=0;
    uint32                                      soc_sand_rv;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO      prm_profile_info;
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO         prm_egr_profile_info;
    SOC_PPC_TRAP_ETPP_INFO                      etpp_trap_info;

    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO      snoop_profile_info;
    bcm_dpp_rx_virtual_traps_t                  virtual_traps_p;
    uint32                                      trap_index; /* for the allocation manager only, not PPD calls */
    int                                         trap_id_egress;
    uint32                                      oamp_error_trap_id;
    int                                         core;
    int                                         port_lcl;
    int                                         cores_num;
    uint32                                      header_type;
    int                                         dest_port;
    int                                         is_discard;
    BCMDNX_INIT_FUNC_DEFS;

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pointer is NULL ")));         
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit, trap_id, &trap_id_sw));

    if (_BCM_TRAP_ID_IS_OAM(trap_id_sw)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(trap_id_sw);
        rv = _bcm_dpp_oam_error_trap_get(unit, oamp_error_trap_id, &config->dest_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    if (_bcm_dpp_am_trap_is_alloced(unit,trap_id_sw) != BCM_E_EXISTS ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trap id not alloced"))); 
    }

    unit = (unit);
    cores_num = SOC_DPP_DEFS_GET(unit, nof_cores);


    if (_BCM_TRAP_ID_IS_ETPP(trap_id_sw)) 
    {

        SOC_PPC_TRAP_ETPP_INFO_clear(&etpp_trap_info);
        if(config->mirror_cmd == NULL)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL"))); 
        }

        trap_id_sw =  _bcm_dpp_rx_trap_id_to_etpp(trap_id_sw);
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_etpp_trap_get,(unit,trap_id_sw,&etpp_trap_info)));
        BCMDNX_IF_ERR_EXIT(rv);

        config->mirror_cmd->recycle_cmd = etpp_trap_info.mirror_cmd;
        config->mirror_cmd->copy_strength = etpp_trap_info.mirror_strength;

        if (etpp_trap_info.mirror_enable == 1)
        {
            config->mirror_cmd->flags |= BCM_MIRROR_OPTIONS_COPY_DROP;
        }

        config->trap_strength = etpp_trap_info.fwd_strength;

        if (etpp_trap_info.fwd_enable == 0) 
        {
            config->dest_port =  BCM_GPORT_BLACK_HOLE;
            config->flags |= BCM_RX_TRAP_UPDATE_DEST;
        }


    }

    else if (!_BCM_TRAP_ID_IS_EGRESS(trap_id_sw)) 
    {    
        /* Check if virtual bit is marked */
        if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw)) 
        {

            /*Clears virtual bit before allocation,range 0-11*/
            virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id_sw);
            /*Save config data in _rx_virtual_traps array*/
             if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_get function failed"))); 
             }

             if (_bcm_dpp_rx_fwd_action_to_config(unit,&(virtual_traps_p.trap_info),config) != BCM_E_NONE)
             {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_fwd_action_to_config failed, on unit:%d \n"), unit));                              
             }
     
             config->snoop_cmnd =  virtual_traps_p.snoop_info.snoop_cmnd;
             config->snoop_strength = virtual_traps_p.snoop_info.strength;

        } 
        else 
        {

            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_id_sw,&prm_profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (_bcm_dpp_rx_fwd_action_to_config(unit,&prm_profile_info,config) != BCM_E_NONE)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_fwd_action_to_config failed, on unit:%d \n"), unit));                                                           
            }

            /*Connect snoop to trap*/
            soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(unit,trap_id_sw,&snoop_profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            config->snoop_cmnd = snoop_profile_info.snoop_cmnd;
            config->snoop_strength = snoop_profile_info.strength;

            if (  prm_profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET) {
                  config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                  BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_bcm_ppd_to_forwarding_header(prm_profile_info.processing_info.frwrd_offset_index, &header_type));
                  config->forwarding_header = header_type; 
            } 
        }

    }else {

        /* Translate the indexing between enum and int for egress traps */
        rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                           _bcm_dpp_rx_trap_id_to_egress(trap_id_sw), /* from the enum */
                                           &trap_index); /* 0...NOF_TRAPS */
        BCMDNX_IF_ERR_EXIT(rv);
        _bcm_dpp_rx_egress_trap_to_soc_ppc_trap(trap_id_sw, &trap_id_egress);
        rv =_bcm_dpp_am_template_trap_egress_data_get(unit, trap_id_egress, trap_index, &prm_egr_profile_info);
        BCM_IF_ERROR_RETURN(rv);

        if (prm_egr_profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC) {
            config->prio = prm_egr_profile_info.cos_info.tc;
            config->flags |= BCM_RX_TRAP_UPDATE_PRIO;
        }

        if (prm_egr_profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
            config->color = prm_egr_profile_info.cos_info.dp;
            config->flags |= BCM_RX_TRAP_UPDATE_COLOR;
        }

        if (prm_egr_profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST)
        {
            if (prm_egr_profile_info.out_tm_port == SOC_PPC_TRAP_ACTION_PKT_DISCARD_AND_LEARN_ID)
            {
                      config->flags |= BCM_RX_TRAP_DROP_AND_LEARN;
            }
            else
            {      
                config->flags |= BCM_RX_TRAP_UPDATE_DEST;

                if (!SOC_IS_ARADPLUS_AND_BELOW(unit))
                {
                    is_discard = (prm_egr_profile_info.out_tm_port_arr[0] == SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID);
                    if ( cores_num > 1)
                    {
                        /* In multi core devices, this has to be true. */
                        if ((config->core_config_arr_len != cores_num   || config->core_config_arr == NULL) && !is_discard)
                        {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Please provide a core_config_arr and a matching core_config_arr_len "))); 
                        }
                    }
                    BCM_DPP_CORES_ITER(_SHR_CORE_ALL, core)
                    {
                        if(is_discard)
                        {
                            dest_port = BCM_GPORT_BLACK_HOLE;
                        }
                        else
                        {
                            rv = soc_port_sw_db_tm_to_local_port_get(unit, core, prm_egr_profile_info.out_tm_port_arr[core] , &port_lcl);
                            BCMDNX_IF_ERR_EXIT(rv);
                            BCM_GPORT_LOCAL_SET(dest_port, port_lcl);
                        }

                       /* In multi core devices, this condition has to be true.
                          In single core devices, if the user wishes, we also support returning the dest port in the core_config_arr*/
                       if (config->core_config_arr_len == 1   && config->core_config_arr != NULL)
                       {
                           config->core_config_arr[0].dest_port = dest_port;
                       }

                       if (cores_num == 1 || is_discard) {
                           /* Support for the old calling method, not using an array. */
                           config->dest_port = dest_port;
                           break;
                       }

                    }
                }
                else 
                { 
                    BCM_GPORT_LOCAL_SET(config->dest_port, prm_egr_profile_info.out_tm_port);
                }

            }
        }

        if (prm_egr_profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD)
        {
            config->flags |= BCM_RX_TRAP_UPDATE_ENCAP_ID;
            if (!SOC_IS_ARADPLUS_AND_BELOW(unit))
            {
                if ( cores_num > 1)
                {
                    if (config->core_config_arr_len != cores_num   || config->core_config_arr == NULL) 
                    {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Please provide a core_config_arr and a matching core_config_arr_len "))); 
                    }
                }

                BCM_DPP_CORES_ITER(_SHR_CORE_ALL, core)
                {
                    if (config->core_config_arr_len == 1   && config->core_config_arr != NULL)
                    {
                        BCM_FORWARD_ENCAP_ID_VAL_SET(config->core_config_arr[core].encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF , BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL , prm_egr_profile_info.header_data_arr[core].cud);       
                    }

                    if (cores_num == 1) {
                        BCM_FORWARD_ENCAP_ID_VAL_SET(config->encap_id , BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF , BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL , prm_egr_profile_info.header_data_arr[0].cud);
                    }
                }
            }
            else
            {
                /* if the user wishes, we also support returning the encap_id in the core_config_arr*/
                if (config->core_config_arr_len == 1   && config->core_config_arr != NULL)
                {
                    BCM_FORWARD_ENCAP_ID_VAL_SET(config->core_config_arr[0].encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF , BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL , prm_egr_profile_info.header_data_arr[0].cud);       
                }  

                BCM_FORWARD_ENCAP_ID_VAL_SET(config->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF , BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL , prm_egr_profile_info.header_data.cud);       
            }
        }
        if (prm_egr_profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_COS_PROFILE)
        {
            config->qos_map_id = prm_egr_profile_info.header_data.cos_profile;
            config->flags |= BCM_RX_TRAP_UPDATE_QOS_MAP_ID;
        }
    }
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/* Create snoop command. */
int bcm_petra_rx_snoop_create(
    int unit, 
    int flags, 
    int *snoop_cmnd)
{
    int rv = BCM_E_NONE; 
    int epp =0;
    
    BCMDNX_INIT_FUNC_DEFS;

    if(snoop_cmnd == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("snoop_cmnd pointer is NULL"))); 
    }

    rv = _bcm_dpp_am_snoop_alloc(unit,flags,*snoop_cmnd,&epp);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if use creat without id ,update snoop_cmnd with alloction number. range: 2-15(0=no snoop,1=saved snoop)*/
    if(!(flags & BCM_RX_SNOOP_WITH_ID)) {
        *snoop_cmnd = epp; 
    }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;  
}

/* Destroy snoop command. */
int bcm_petra_rx_snoop_destroy(
    int unit, 
    int flags, 
    int snoop_cmnd)
{
    int elem=0;
    int rv= BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_snoop_dealloc(unit,flags,snoop_cmnd,elem); 
    BCM_IF_ERROR_RETURN(rv);

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;

}



int bcm_petra_rx_snoop_set(
    int unit, 
    int snoop_cmnd, 
    bcm_rx_snoop_config_t *config)
{

    uint32 soc_sand_rv;    
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO prm_profile_info;
    int rv;
    bcm_mirror_destination_t snoop_dest;
    uint32 out_lif_id;

    BCMDNX_INIT_FUNC_DEFS;

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL"))); 
    }

    if ((config->flags & BCM_RX_SNOOP_UPDATE_COUNTER) && ((config->flags & BCM_RX_SNOOP_UPDATE_COUNTER_1) || 
                                                           (config->flags & BCM_RX_SNOOP_UPDATE_COUNTER_2))) 
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
      (_BSL_BCM_MSG("BCM_RX_SNOOP_UPDATE_COUNTER can not be in use with BCM_RX_SNOOP_UPDATE_COUNTER_1 or BCM_RX_SNOOP_UPDATE_COUNTER_2")));
    }

    if (SOC_IS_JERICHO(unit)) {

        if ((config->flags & BCM_RX_SNOOP_UPDATE_DEST) != BCM_RX_SNOOP_UPDATE_DEST) {
            /*The destination field under IRR_SNOOP_MIRROR_DEST_TABLE doesn't have an enabler, so the user must specify the destination*/
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_RX_SNOOP_UPDATE_DEST flag must be raised")));
        }

        /*convert bcm_rx_snoop_config_t to bcm_mirror_destination_t*/
        bcm_mirror_destination_t_init(&snoop_dest); /*initialize snoop_dest*/
        bcm_mirror_pkt_header_updates_t_init(&snoop_dest.packet_control_updates); /*initialize snoop_dest.packet_control_updates*/

        if (config->flags & BCM_RX_SNOOP_DEST_MULTICAST) {
            BCM_GPORT_MCAST_SET(snoop_dest.gport, config->dest_group);
        } else {
            snoop_dest.gport = config->dest_port;
        }
        snoop_dest.policer_id = config->policer_id;
        snoop_dest.packet_control_updates.prio = config->prio;
        snoop_dest.packet_control_updates.color = config->color;
        snoop_dest.packet_copy_size = config->size == -1 ? 0 : config->size; /*bcm_mirror_destination_t uses 0 to indicate no cropping while bcm_rx_snoop_config_t uses -1*/
        snoop_dest.stat_id =  config->counter_num;
        BCM_GPORT_MIRROR_SNOOP_SET(snoop_dest.mirror_dest_id, snoop_cmnd); /*set snoop profile id*/
        snoop_dest.sample_rate_dividend = config->probability; /*snoop probability is measured between 0 and 100 (100000)*/
        snoop_dest.sample_rate_divisor = 100000;

        /*setting up flags*/
        snoop_dest.flags |= BCM_MIRROR_DEST_IS_SNOOP; /*is snoop command*/
        snoop_dest.flags |= BCM_MIRROR_DEST_WITH_ID; /*should always snoop with specified ID (snoop_cmnd)*/
        snoop_dest.flags |= BCM_MIRROR_DEST_REPLACE; /*will always replace destination*/
        snoop_dest.flags |= (config->flags & BCM_RX_SNOOP_DEST_MULTICAST) ? BCM_MIRROR_DEST_DEST_MULTICAST : 0; 
        snoop_dest.flags |= (config->flags & BCM_RX_SNOOP_UPDATE_COUNTER) ? BCM_MIRROR_DEST_UPDATE_COUNTER : 0; 
        snoop_dest.flags |= (config->flags & BCM_RX_SNOOP_UPDATE_COUNTER_1) ? BCM_MIRROR_DEST_UPDATE_COUNTER_1 : 0;
        snoop_dest.flags |= (config->flags & BCM_RX_SNOOP_UPDATE_COUNTER_2) ? BCM_MIRROR_DEST_UPDATE_COUNTER_2 : 0;
        snoop_dest.flags |= (config->flags & BCM_RX_SNOOP_UPDATE_POLICER) ? BCM_MIRROR_DEST_UPDATE_POLICER : 0; 
        snoop_dest.packet_control_updates.valid |= (config->flags & BCM_RX_SNOOP_UPDATE_COLOR) ? BCM_MIRROR_PKT_HEADER_UPDATE_COLOR : 0;
        snoop_dest.packet_control_updates.valid |= (config->flags & BCM_RX_SNOOP_UPDATE_PRIO) ? BCM_MIRROR_PKT_HEADER_UPDATE_PRIO : 0;

        /* if BCM_RX_SNOOP_UPDATE_DEST is set with BCM_RX_SNOOP_UPDATE_ENCAP_ID, outlif can be updated */
        if ((config->flags & (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_ENCAP_ID)) == (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_ENCAP_ID)) {
            snoop_dest.encap_id = config->encap_id;
            snoop_dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
        }

        /*set_dest = 1 only if BCM_RX_SNOOP_UPDATE_DEST flag is raised, to maintain backward compatability*/
        rv = _bcm_petra_mirror_or_snoop_destination_create(unit, &snoop_dest, (config->flags & BCM_RX_SNOOP_UPDATE_DEST) ? TRUE : FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {

        SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&prm_profile_info);

        /* check size one of valid options, -1: for all packet, 128 not supported HW bug */
        switch(config->size) {
        case 64:
            prm_profile_info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_64;
            break;
        case 192:
            prm_profile_info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_192;
            break;
        case -1:
            prm_profile_info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config->size not valid"))); 
        }

        _BCM_RX_SNOOP_CONFIG_PROB_TO_TMC_ACTION_CMD_SNOOP_INFO_PROB(config->probability,prm_profile_info.prob );  


		if (config->flags & BCM_RX_SNOOP_UPDATE_DEST) { 	   

			if (config->flags & BCM_RX_SNOOP_DEST_MULTICAST) {  	  
				prm_profile_info.cmd.dest_id.type = SOC_TMC_DEST_TYPE_MULTICAST;
				prm_profile_info.cmd.dest_id.id = config->dest_group;
			}else
			{
                /* if BCM_RX_SNOOP_UPDATE_ENCAP_ID is set, outlif can be updated */
                if (config->flags & BCM_RX_SNOOP_UPDATE_ENCAP_ID) 
                {
                    _bcm_dpp_field_gport_to_global_lif_convert(unit, config->encap_id, 0 /*out lif */, &out_lif_id);
                    prm_profile_info.cmd.outlif.enable = 1;
                    prm_profile_info.cmd.outlif.value = out_lif_id;
                }

				rv = _bcm_dpp_gport_to_tm_dest_info(unit,config->dest_port,&prm_profile_info.cmd.dest_id);
				BCM_SAND_IF_ERR_EXIT(rv);
			}
		}
        if (config->flags & BCM_RX_SNOOP_UPDATE_PRIO) {
           prm_profile_info.cmd.tc.value = config->prio;  

        }

        prm_profile_info.cmd.tc.enable = SOC_SAND_NUM2BOOL(config->flags & BCM_RX_SNOOP_UPDATE_PRIO);


        if (config->flags & BCM_RX_SNOOP_UPDATE_COLOR) {
            prm_profile_info.cmd.dp.value = config->color;
        }
        prm_profile_info.cmd.dp.enable = SOC_SAND_NUM2BOOL(config->flags & BCM_RX_SNOOP_UPDATE_COLOR);
    
        if (config->flags & BCM_RX_SNOOP_UPDATE_POLICER) {
            /*prm_profile_info.cmd.dp = config.policer_id;*/
        }
    
        if (config->flags & BCM_RX_SNOOP_UPDATE_ETHERNET_POLICER) {

        }
    
        if (config->flags & BCM_RX_SNOOP_UPDATE_COUNTER) {
        } 

    
        if (config->flags & BCM_RX_SNOOP_DEST_MULTICAST) {
        }   
        if (prm_profile_info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_MULTICAST) {
            /* mark prm_profile_info.cmd.is_ing_mc true if prm_profile_info.cmd.dest_id.id is an ingress MC group */ 
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_group_exists(unit, prm_profile_info.cmd.dest_id.id, &prm_profile_info.cmd.is_ing_mc));
        }
        /**prm_profile_info.cmd.outlif.value = snoop_cmnd;  consider inserting this in the future*/ 

    
        unit = (unit);
    #ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)) {
            if (!SOC_WARM_BOOT(unit)) {
                soc_sand_rv = arad_action_cmd_snoop_set_unsafe(unit,snoop_cmnd,&prm_profile_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    #endif


    }

    BCM_EXIT;    
    
exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_rx_snoop_get(
    int unit, 
    int snoop_cmnd, 
    bcm_rx_snoop_config_t *config)
{

    uint32 soc_sand_rv;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO prm_profile_info;
    bcm_mirror_destination_t snoop_dest;
    bcm_gport_t snoop_dest_id;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (SOC_IS_JERICHO(unit)) {
        BCM_GPORT_MIRROR_SNOOP_SET(snoop_dest_id, snoop_cmnd); /*convert snoop_cmnd to destination_id*/
        BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_destination_get(unit, snoop_dest_id, &snoop_dest)); /*use bcm_petra_mirror_destination_get to get snoop destination configuration*/
        
        config->dest_port = snoop_dest.gport;
        config->dest_group = BCM_GPORT_MCAST_GET(snoop_dest.gport);
        config->policer_id = snoop_dest.policer_id;
        config->prio = snoop_dest.packet_control_updates.prio;
        config->color = snoop_dest.packet_control_updates.color;
        config->size = snoop_dest.packet_copy_size == 0 ? -1 : snoop_dest.packet_copy_size; /*bcm_mirror_destination_t uses 0 to indicate no cropping while bcm_rx_snoop_config_t uses -1*/
        config->counter_num = snoop_dest.stat_id;
        config->probability = snoop_dest.sample_rate_dividend;

        /*setting up flags*/
        config->flags |= (snoop_dest.packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_COLOR) ? BCM_RX_SNOOP_UPDATE_COLOR : 0;
        config->flags |= (snoop_dest.packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_PRIO) ? BCM_RX_SNOOP_UPDATE_PRIO : 0;
        if (BCM_GPORT_IS_MCAST(snoop_dest.gport)) {
            config->flags |= BCM_RX_SNOOP_DEST_MULTICAST;
        }else{
            config->flags |= BCM_RX_SNOOP_UPDATE_DEST;
        }
    } else {
        if (config == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL")));
        }

        if (_bcm_dpp_am_snoop_is_alloced(unit,snoop_cmnd) != BCM_E_EXISTS ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("snoop_cmnd not alloced"))); 
        }
        unit = (unit);
    
    #ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)) {
            soc_sand_rv = arad_action_cmd_snoop_get(unit,snoop_cmnd,&prm_profile_info);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        }
    #endif
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("supporting only DNX devices"))); 
        }

        if(prm_profile_info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_MULTICAST) {
            config->dest_group = prm_profile_info.cmd.dest_id.id;
            config->flags |= BCM_RX_SNOOP_DEST_MULTICAST;
        }else
        {
            rv = _bcm_dpp_gport_from_tm_dest_info(unit,&config->dest_port,&prm_profile_info.cmd.dest_id);
            BCM_SAND_IF_ERR_EXIT(rv);
            config->flags |= BCM_RX_SNOOP_UPDATE_DEST;
            /* when we set the outlif, the value is not 0*/
            if (prm_profile_info.cmd.outlif.value != 0)
            {
                   config->flags |= BCM_RX_SNOOP_UPDATE_ENCAP_ID;
                   BCM_FORWARD_ENCAP_ID_VAL_SET(config->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF , BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL , prm_profile_info.cmd.outlif.value);
            }
        }

        if(prm_profile_info.cmd.tc.enable) {
            config->prio = prm_profile_info.cmd.tc.value;  
            config->flags |= BCM_RX_SNOOP_UPDATE_PRIO;
        }

        if(prm_profile_info.cmd.dp.enable) {
            config->color = prm_profile_info.cmd.dp.value;
            config->flags |= BCM_RX_SNOOP_UPDATE_COLOR;
        }
            
        config->size  = _BCM_RX_TMC_ACTION_SIZE_TO_SNOOP_SIZE(prm_profile_info.size);
        _BCM_RX_TMC_ACTION_CMD_SNOOP_INFO_PROB_TO_SNOOP_CONFIG_PROB(config->probability,prm_profile_info.prob );

    
        /*config.flags  = ??? */

        /*config->policer_id = ??? */
        /*config->dest_group  = ??? */
        /*config->dest_group  = ??? */
        /*config->counter_num  = ??? */
        
    }
    BCM_EXIT;     
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_rx_queue_channel_get(
    int unit, 
    bcm_cos_queue_t queue_id, 
    bcm_rx_chan_t *chan_id)
{
BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_channel_get(unit,queue_id, chan_id));
    } 
#endif
   BCM_EXIT;    
exit:
   BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_rx_queue_channel_set(
    int unit, 
    bcm_cos_queue_t queue_id, 
    bcm_rx_chan_t chan_id)
{
BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_channel_set(unit,queue_id, chan_id));
    } 
#endif
   BCM_EXIT;    
exit:
   BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_rx_queue_rate_limit_status_get(
    int unit, 
    bcm_cos_queue_t cosq, 
    int *packet_tokens)
{
BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_rate_limit_status_get(unit, cosq, packet_tokens));
    } 
#endif
   BCM_EXIT;    
exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_rx_queue_register(
    int unit, 
    const char *name, 
    bcm_cos_queue_t cosq, 
    bcm_rx_cb_f callback, 
    uint8 priority, 
    void *cookie, 
    uint32 flags)
{
 BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_register(unit,name,cosq,callback,priority, cookie,flags));
     } 
#endif
    BCM_EXIT;    
exit:
    BCMDNX_FUNC_RETURN;

}

int 
bcm_petra_rx_queue_unregister(
    int unit, 
    bcm_cos_queue_t cosq, 
    bcm_rx_cb_f callback, 
    uint8 priority)
{
    return BCM_E_UNAVAIL;
}

void
_bcm_dpp_rx_packet_cos_parse(
    int unit, 
    bcm_pkt_t *pkt)
{
#ifdef BCM_ARAD_SUPPORT
    bcm_pbmp_t  bmp;
    bcm_port_t  port;
    uint32 queue_base, queue_offset;
    bcm_error_t rv;
    int core=0; 
    
    if (SOC_IS_ARAD(unit)) {
        /* Extracting port and TC */ 
        unit = (unit); 
        bmp = PBMP_CMIC(unit);
        PBMP_ITER(bmp, port){
            if(port >= ARAD_NOF_FAP_PORTS) {
                break;
            }
            rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core,  port, &queue_base);
            if(BCM_FAILURE(rv)) {
                return;
            }
            rv = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core, port, &queue_offset);
            if(BCM_FAILURE(rv)) {
                return;
            }

            if ((pkt->rx_port >= queue_base) && (pkt->rx_port < (queue_base+queue_offset))) {
                pkt->cos = (pkt->rx_port - queue_base);
                break;
            }
        }
        return;
    }
#endif
}
    /*
     * This function dumps the packet contents, then returns that we have handled and 
     * we are done with the packet. 
     *
     * The bcm_pkt_t structure is defined in orion/include/bcm/pkt.h
     * The RX API supports only one packet buffer per packet. 
     * There is no scatter/gather RX support.
     * 
     * In order to fill pkt->src_port and pkt->src_mod fields we have to access the device,
     * which is not interrupt safe. The flag device_access_allowed should be set
     * when using this function ountside interrupt context in order to enable
     * calculation of these fields. Otherwise they are set to 0.
     */
int
_bcm_dpp_rx_packet_parse(
    int unit, 
    bcm_pkt_t *pkt,
    uint8 device_access_allowed)
{
#ifdef BROADCOM_DEBUG
    int i;
#endif /*BROADCOM_DEBUG*/
#ifdef BCM_ARAD_SUPPORT
    SOC_PPC_TRAP_PACKET_INFO packetInfo;
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t mod_port, gport;

    uint32 sand_rv;
    uint8 found, is_lag ;
    uint32 lag_id, lag_member_id, sys_port,
            port_i, tm_port = SOC_MAX_NUM_PORTS, tm_port_i; 
    ARAD_PORT_HEADER_TYPE header_type_out;
    bcm_pbmp_t  bmp;
    bcm_port_t  port;
    uint32 queue_base, queue_offset;
    SOC_PPC_LAG_INFO lag_info;
    int core = 0, core_i; 
    soc_pkt_t   soc_pkt;
    int         max_port;
    uint16 vlan_control_tag = 0;
    uint8 *eth_pkt_header = NULL;
#endif /*BCM_ARAD_SUPPORT*/
    BCMDNX_INIT_FUNC_DEFS;

#if defined(BROADCOM_DEBUG)
    if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "*************************************RX: (Egress) port %d, cos %d, tot len %d, reason %04x\n"), 
                                pkt->rx_port, pkt->cos, pkt->tot_len, pkt->rx_reason));
    
        {
            char *packet_ptr, *ch_ptr;
            packet_ptr = sal_alloc(3 * pkt->pkt_len, "Packet print");
            if (packet_ptr != NULL) {
                ch_ptr = packet_ptr;
                    /* Dump the packet data block */
                for(i = 0; i < pkt->pkt_len; i++) {
                        if ((i % 4) == 0) {
                            sal_sprintf(ch_ptr," ");
                            ch_ptr++;
                        }
                        if ((i % 32) == 0) {
                            sal_sprintf(ch_ptr,"\n");
                            ch_ptr++;
                        }
                        
                        sal_sprintf(ch_ptr,"%02x", (pkt->_pkt_data.data)[i]); 
                        ch_ptr++;
                        ch_ptr++;
                }
                sal_sprintf(ch_ptr,"\n");
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "%s"),packet_ptr));
                sal_free(packet_ptr);
            }
        }
    }
#endif/*BROADCOM_DEBUG*/
   
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /* Extracting port and TC */ 
        unit = (unit);
        found = 0;    
        bmp = PBMP_CMIC(unit);
        max_port = SOC_DPP_DEFS_GET(unit, nof_local_ports);

        PBMP_ITER(bmp, port){
            if(port >= max_port) {
                break;
            }
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));

            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &queue_base));
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core, tm_port, &queue_offset));

            if ((pkt->rx_port >= queue_base) && (pkt->rx_port < (queue_base+queue_offset))) {
                found = 1;
                pkt->cos = (pkt->rx_port - queue_base);
                BCM_GPORT_LOCAL_SET(pkt->dst_gport, port);
                pkt->src_gport = -1;
                break;
            }
        }
        if (!found) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Error in rx_port extraction! Unit=%d, queue_id=%d\n"), unit, pkt->rx_port));
        }

        found = 0; 
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &bmp));
        PBMP_ITER(bmp,port_i){ 
              if (port_i != SOC_DPP_PORT_INTERNAL_ERP(0)) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));
                if(tm_port_i == tm_port) {
                    found = 1;
                    break;
                }
            }
        }

        if (!found) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Internal Error in finding port header type! Unit=%d, port=%d\n"), unit, port));
        }
        
        /* Checking header type */
        if (found) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port_i, &header_type_out);
            BCMDNX_IF_ERR_EXIT(rv); 
            if ((header_type_out == SOC_TMC_PORT_HEADER_TYPE_CPU)
                || 
                (header_type_out == SOC_TMC_PORT_HEADER_TYPE_STACKING) )
            {
                sand_rv = soc_ppd_trap_packet_parse(unit, pkt->_pkt_data.data, pkt->tot_len, &packetInfo, &soc_pkt);
                BCM_SAND_IF_ERR_EXIT(sand_rv);

                /* Convert soc_pkt to bcm_pkt*/
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_soc_pkt_to_bcm_pkt(&soc_pkt, pkt));

                pkt->cos = packetInfo.tc;
                pkt->prio_int = packetInfo.dp;
                pkt->pkt_len = pkt->tot_len-packetInfo.ntwrk_header_ptr;
                /* update priority and vlan id based on (outer) vlan tag of pkt */
                eth_pkt_header = pkt->_pkt_data.data + packetInfo.ntwrk_header_ptr;
                vlan_control_tag = ((uint16)((eth_pkt_header[14] << 8) | eth_pkt_header[15]));
                pkt->prio_int = ((vlan_control_tag >> 13) & 0x007);
                pkt->vlan = ((vlan_control_tag >> 0) & 0xfff);
                if (packetInfo.is_trapped) {

                    _bcm_dpp_rx_trap_type_get(unit, packetInfo.cpu_trap_code, &found, &pkt->rx_reason);        
                    if (!found) {
                        LOG_VERBOSE(BSL_LS_BCM_RX,
                                    (BSL_META_U(unit,
                                                "No trap code found! Unit %d\n"), unit));
                    }
                    if (pkt->rx_reason == bcmRxTrapUserDefine) {
                        pkt->rx_trap_data = packetInfo.cpu_trap_code;
                    }
                    else {
                        pkt->rx_trap_data = packetInfo.cpu_trap_qualifier;
                    }
                } else {
                    pkt->rx_trap_data = 0;
                    pkt->rx_reason = 0;
                }

                if (device_access_allowed) {
                    sand_rv = arad_ports_logical_sys_id_parse(unit, packetInfo.src_sys_port, 
                                                              &is_lag, &lag_id, &lag_member_id, 
                                                              &sys_port);
                     BCM_SAND_IF_ERR_EXIT(sand_rv);

                     if (is_lag) {
                         pkt->src_trunk = lag_id;

                        SOC_PPC_LAG_INFO_clear(&lag_info);

                        /* Get LAG info */
                        rv = soc_dpp_trunk_sw_db_get(unit, lag_id, &lag_info);
                        BCM_SAND_IF_ERR_EXIT(rv);

                        /* Get the gport of index lag_member_ndx into gport */
                        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, lag_info.members[lag_member_id].sys_port);
                        rv = bcm_petra_stk_sysport_gport_get(unit, sys_port, &gport);
                        BCMDNX_IF_ERR_EXIT(rv);

                        
                        /*and set to src_gport after the problem with src_gport/dest_gport cunfusion is fixed. */
                        pkt->src_port = BCM_GPORT_MODPORT_PORT_GET(gport);
                        pkt->src_mod = BCM_GPORT_MODPORT_MODID_GET(gport);
                     }
                     else {
                         pkt->src_trunk = -1;
                         BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, packetInfo.src_sys_port);
                         rv = bcm_petra_stk_sysport_gport_get(unit, sys_port, &mod_port);
                         if (BCM_E_NONE != rv) {
                             LOG_VERBOSE(BSL_LS_BCM_RX,
                                         (BSL_META_U(unit,
                                                     "rx_intr_process_packet: system gport unvalid: "
                                                     "Unit %d Port %d\n"),
                                          unit, sys_port));
                         } else {
                             pkt->src_port = BCM_GPORT_MODPORT_PORT_GET(mod_port);
                             pkt->src_mod = BCM_GPORT_MODPORT_MODID_GET(mod_port);
                         }

                     }
    #ifdef BROADCOM_DEBUG
                     if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
                         SOC_PPC_TRAP_PACKET_INFO_print(&packetInfo);
                         LOG_VERBOSE(BSL_LS_BCM_RX,
                                     (BSL_META_U(unit,
                                                 "*************************************RX: is_lag %d, lag_id %d, src_port %d, src_mod %d\n"), 
                                                 is_lag,pkt->src_trunk, (uint8)pkt->src_port, pkt->src_mod));                         /* Dump the dnx headers.*/

                 }
                 if (bsl_check(bslLayerBcm, bslSourcePkt, bslSeverityVerbose, unit)) {

                     bcm_pkt_dnx_dump(pkt);
                 }
#endif/*BROADCOM_DEBUG*/
                 if ((int8)(packetInfo.otm_port) == -1) {
                     pkt->multicast_group =  packetInfo.outlif_mcid;
                 } 
                 pkt->dest_port =  packetInfo.otm_port;
                 pkt->stk_load_balancing_number =  packetInfo.lb_key;
                 pkt->stk_dst_gport = packetInfo.dsp_ext;
                 pkt->stk_route_tm_domains= packetInfo.stacking_ext;
                 pkt->dst_vport = packetInfo.outlif_mcid;
                 pkt->src_vport = packetInfo.internal_inlif_inrif;

                 if (packetInfo.internal_unknown_addr) {
                     pkt->flags2 |= BCM_PKT_F2_UNKNOWN_DEST;
                 }
                 if (packetInfo.frwrd_type == SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP) {
                     pkt->flags2 |= BCM_PKT_F2_SNOOPED;
                 }
                
                 switch (packetInfo.internal_fwd_code) {
                     case SOC_TMC_PKT_FRWRD_TYPE_MPLS:
                         pkt->stk_forward = BCM_PKT_STK_FORWARD_MPLS;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_TRILL:
                         pkt->stk_forward = BCM_PKT_STK_FORWARD_TRILL;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1:
                         pkt->stk_forward =  BCM_PKT_STK_FORWARD_FCOE;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_SNOOP:
                         pkt->stk_forward =  BCM_PKT_STK_FORWARD_SNOOP;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_TM:
                         pkt->stk_forward =  BCM_PKT_STK_FORWARD_TRAFFIC_MANAGEMENT;
                         break;
                     default:
                         break;
                 }

                }
                else {
                    pkt->src_port = 0;
                    pkt->src_mod = 0;
                }
#if defined(BROADCOM_DEBUG)
                if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
                    LOG_VERBOSE(BSL_LS_BCM_RX,
                                (BSL_META_U(unit,
                                            "*************************************RX: (Egress) packet len %d\n"), pkt->pkt_len));
                }
#endif/*BROADCOM_DEBUG*/
            }
        }
        else {
            LOG_WARN(BSL_LS_BCM_RX,
                     (BSL_META_U(unit,
                                 "RX: skipping packet parsing because of non-cpu port header type\n")));
        }
    }
#else
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available for this unit")));
#endif /*BCM_ARAD_SUPPORT*/
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_rx_forwarding_type_to_ppd(bcm_forwarding_type_t bcmForwardingType,
                                          SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType)
{
    switch(bcmForwardingType) {
    case bcmForwardingTypeL2:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
        break;
    case bcmForwardingTypeIp4Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
        break;
    case bcmForwardingTypeIp4Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC;
        break;
    case bcmForwardingTypeIp6Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC;
        break;
    case bcmForwardingTypeIp6Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC;
        break;
    case bcmForwardingTypeMpls:
         *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
        break;
    case bcmForwardingTypeTrill:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TRILL;
        break;
    case bcmForwardingTypeRxReason:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP;
        break;
    case bcmForwardingTypeFCoE:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1;
        break;
    case bcmForwardingTypeTrafficManagement:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TM;
        break;
    case bcmForwardingTypeSnoop:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_SNOOP;
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}

int
_bcm_dpp_rx_ppd_to_forwarding_type(SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType,
									bcm_forwarding_type_t *bcmForwardingType)
{
    switch(ppdForwardingType) {
    case SOC_TMC_PKT_FRWRD_TYPE_BRIDGE :
        *bcmForwardingType = bcmForwardingTypeL2;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC:
        *bcmForwardingType = bcmForwardingTypeIp4Ucast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC :
        *bcmForwardingType = bcmForwardingTypeIp4Mcast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC :
        *bcmForwardingType = bcmForwardingTypeIp6Ucast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC :
        *bcmForwardingType = bcmForwardingTypeIp6Mcast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_MPLS :
         *bcmForwardingType = bcmForwardingTypeMpls;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_TRILL :
        *bcmForwardingType = bcmForwardingTypeTrill;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP :
        *bcmForwardingType = bcmForwardingTypeRxReason;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1 :
        *bcmForwardingType = bcmForwardingTypeFCoE;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_TM :
        *bcmForwardingType = bcmForwardingTypeTrafficManagement;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_SNOOP :
        *bcmForwardingType = bcmForwardingTypeSnoop;
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}



/* if bcm886xx_rx_use_hw_trap_id SOC is turned on, trap_id_converted will store the HW trap id of trap_id_sw. if the SOC is turned off - will store the same value as the trap_id_sw*/
int
_bcm_dpp_rx_get_converted_trap_hw_id(int unit,
                                  int trap_id_sw, 
                                  int *trap_id_converted)
{
    uint32 trap_id_hw;

    BCMDNX_INIT_FUNC_DEFS;

    if (_BCM_RX_EXPOSE_HW_ID(unit) && _BCM_RX_TRAP_IS_REGULAR(trap_id_sw))   
    {
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_field_trap_ppd_to_hw(unit,trap_id_sw,&trap_id_hw));    /* exposing the hw id*/
        *trap_id_converted = trap_id_hw;
    }
    else
    {
        *trap_id_converted = trap_id_sw;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* is bcm886xx_rx_use_hw_trap_id SOC is turned on, trap_id_converted will store the SW trap id of trap_id. if the SOC is turned off - will store the same value as the trap_id*/
int
_bcm_dpp_rx_get_converted_trap_sw_id(int unit,
                                  int trap_id, 
                                  int *trap_id_converted)
{

    SOC_PPC_TRAP_CODE ppd_trap_code;

    BCMDNX_INIT_FUNC_DEFS;

    if (_BCM_RX_EXPOSE_HW_ID(unit) &&  _BCM_RX_TRAP_IS_REGULAR(trap_id)) 
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_trap_ppd_from_hw(unit,trap_id,&ppd_trap_code)); /* converting the HW trap id back to ppd trap code */
        *trap_id_converted = ppd_trap_code;
    }
    else
    {
       *trap_id_converted = trap_id;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* Translation of bcm ForwardingHeader Offset to it's ppd value */
int
_bcm_dpp_rx_bcm_forwarding_header_to_ppd(bcm_rx_trap_forwarding_header_t fwd_header,
                                      uint32 *ppd_fwd_header)
{
    switch (fwd_header) {
    case bcmRxTrapForwardingHeaderPacketStart:
        *ppd_fwd_header = 0;
        break;
    case bcmRxTrapForwardingHeaderL2Header:
        *ppd_fwd_header = 1;
        break;
    case bcmRxTrapForwardingHeaderFirstHeader:
        *ppd_fwd_header = 2;
        break;
    case bcmRxTrapForwardingHeaderSecondHeader:
        *ppd_fwd_header = 3;
        break;
    case bcmRxTrapForwardingHeaderThirdHeader:
        *ppd_fwd_header = 4;
        break;
    case bcmRxTrapForwardingHeaderFourthHeader:
        *ppd_fwd_header = 5;
        break;
    case bcmRxTrapForwardingHeaderOamBfdPdu:
        *ppd_fwd_header = 7; /* -1  - OAM offset*/
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/* Translation of ppd value to its bcm ForwardingHeader Offset */
int
_bcm_dpp_rx_bcm_ppd_to_forwarding_header(uint32 ppd_fwd_header,
                                         bcm_rx_trap_forwarding_header_t* fwd_header)
{
    switch (ppd_fwd_header) {
    case 0:
        *fwd_header = bcmRxTrapForwardingHeaderPacketStart;
        break;
    case 1:
        *fwd_header = bcmRxTrapForwardingHeaderL2Header;
        break;
    case 2:
        *fwd_header = bcmRxTrapForwardingHeaderFirstHeader;
        break;
    case 3:
        *fwd_header = bcmRxTrapForwardingHeaderSecondHeader;
        break;
    case 4:
        *fwd_header = bcmRxTrapForwardingHeaderThirdHeader;
        break;
    case 5:
        *fwd_header = bcmRxTrapForwardingHeaderFourthHeader;
        break;
    case 7:
        *fwd_header = bcmRxTrapForwardingHeaderOamBfdPdu;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/* returns the next eg trap as shown in SOC_PPC_TRAP_EG_TYPE enum   */
int
_bcm_dpp_rx_get_next_eg_soc_ppc_trap(/*in*/ SOC_PPC_TRAP_EG_TYPE current_eg_trap_id,
                                     /*out*/ SOC_PPC_TRAP_EG_TYPE *next_eg_trap_id)
{
    /* last eg trap */
    if (current_eg_trap_id == SOC_PPC_TRAP_EG_TYPE_HIGHEST_ARAD)
    {
        return BCM_E_PARAM;
    }
    /* First enum member */
    else if( current_eg_trap_id == SOC_PPC_TRAP_EG_TYPE_NONE) 
    {
        *next_eg_trap_id = SOC_PPC_TRAP_EG_TYPE_NO_VSI_TRANSLATION;
    }
    /* till SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT traps are incremented by a left shift. */
    else if ( current_eg_trap_id < SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT)
    {
        *next_eg_trap_id = current_eg_trap_id << 1;
    }
    else /* from here, traps are incremented by one and are shifted PPC_EG_TRAP_SHIFT_SIZE bits to the left*/
    {
        *next_eg_trap_id = ((current_eg_trap_id >> PPC_EG_TRAP_SHIFT_SIZE) + 1) << PPC_EG_TRAP_SHIFT_SIZE;
    }

    return BCM_E_NONE;
}


/* reverse function for _BCM_TRAP_EG_TYPE_TO_ID macro  */
void
_bcm_dpp_rx_egress_trap_to_soc_ppc_trap(/*in*/ int trap_id_eg,
                                     /*out*/ SOC_PPC_TRAP_EG_TYPE *trap_id_ppc)
{
    /* removing the egress bit */
    trap_id_eg = (~(_BCM_DPP_RX_TRAP_EGRESS << _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT)) & trap_id_eg;

    /* In this case the egress trap belongs to the section of SOC_PPC_TRAP_EG_TYPE_NONE - SOC_PPC_TRAP_EG_TYPE_CFM_TRAP */
    if (trap_id_eg <= _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT)
    {
       *trap_id_ppc = (SOC_PPC_TRAP_EG_TYPE)_bcm_dpp_rx_trap_egrr_reverse_calc(trap_id_eg);
    }
    else
    {
        trap_id_eg -= _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT;
        trap_id_eg <<= _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT;
        *trap_id_ppc = trap_id_eg;
    }

    return;
}

int
_bcm_dpp_rx_fill_destination_info_egress_trap(/*in*/ int unit, int core, int dest_port,
                                     /*out*/ SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *prm_egr_profile_info, SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO  *prm_profile_info)
{
    int rv = BCM_E_NONE;                             
    uint32   pp_port;
    uint32   tm_port;
    int      core_usr;

    BCMDNX_INIT_FUNC_DEFS;

    if (dest_port != BCM_GPORT_BLACK_HOLE) 
    {
        /* getting the tm port from the gport */
        rv = _bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit,dest_port, &pp_port, &tm_port,&core_usr);
        BCM_IF_ERROR_RETURN(rv);

        /* The user is transferring a local port. We check that the core of the local port is correct*/
        if( core_usr != core) 
        {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Local port - core mismatch")));         
        }

        rv= _bcm_dpp_gport_to_fwd_decision(unit, dest_port, &(prm_profile_info->dest_info.frwrd_dest));
        BCM_IF_ERROR_RETURN(rv);

        prm_egr_profile_info->header_data_arr[core].dsp_ptr = tm_port;
        prm_egr_profile_info->out_tm_port_arr[core] = tm_port; 

    }
    else
    {
        prm_egr_profile_info->out_tm_port_arr[core] = SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID;
    }

    return BCM_E_NONE;

exit:
    BCMDNX_FUNC_RETURN;
}


#endif /* BCM_PETRA_SUPPORT */





