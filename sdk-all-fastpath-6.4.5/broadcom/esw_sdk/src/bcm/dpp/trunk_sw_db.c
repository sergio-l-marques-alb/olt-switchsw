/*
 * $Id: trunk_sw_db.c,v 1.0 Broadcom SDK $
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
 *
 * File:    trunk_sw_db.c
 * Purpose: software database action to manage Trunk
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRUNK

#include <shared/bsl.h>

#include <soc/dpp/PPD/ppd_api_lag.h>

#include <shared/swstate/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/trunk_sw_db.h>

#define TRUNK_ACCESS         sw_state_access[unit].dpp.bcm.trunk

/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_get_nof_replications
 * Purpose:
 *      gets the number of replication of input lag member in input trunk id and index of last replication.
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 *      lag_member                      - lag member to look for
 *      nof_replications                - return by ref number of replications
 *      last_replicated_member_index    - return by ref index of last replication found
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      if none are found nof_replications is 0 and last_replicated_member_index is -1
 */
int _bcm_dpp_trunk_sw_db_get_nof_replications(int unit, int tid, SOC_PPD_LAG_MEMBER *lag_member, int* nof_replications, int* last_replicated_member_index)
{
    int temp_last_replicated_member_index = -1;
    int nof_members = 0;
    int current_member_index = 0;
    int current_member_position = -1;
    int max_nof_port_in_lag = -1;
    int current_member_system_port = -1;
    int nof_replications_counter = 0;
    
    BCMDNX_INIT_FUNC_DEFS;

    /* sanity checks */
    BCMDNX_NULL_CHECK(nof_replications);
    BCMDNX_NULL_CHECK(last_replicated_member_index);

    /* loop on members and count replications */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = max_nof_port_in_lag * tid + current_member_index;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, current_member_position, &current_member_system_port));
        if(current_member_system_port != lag_member->sys_port)
        {
            continue;
        }

        ++nof_replications_counter;
        temp_last_replicated_member_index = current_member_index;
    }

    /* update outgoing veriables */
    *nof_replications = nof_replications_counter;
    *last_replicated_member_index = temp_last_replicated_member_index;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_add
 * Purpose:
 *      add member to trunk software data-base.
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 *      lag_member                      - lag member to add
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      valid flags are BCM_TRUNK_MEMBER_INGRESS_DISABLE. when adding member with this flag, it cannot have replications in the trunk.
 */
int _bcm_dpp_trunk_sw_db_add(int unit, int tid, SOC_PPD_LAG_MEMBER *lag_member)
{
    int         max_nof_tid = 0;
    int         nof_replications = 0;
    int         last_replicated_member_index = 0;
    int         last_replicated_member_position = 0;
    int         max_nof_port_in_lag = 0;
    int         nof_members;
    int         new_member_id;
    int         new_mamber_position;
    uint32      last_replicated_member_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* sanity checks */
    BCMDNX_NULL_CHECK(lag_member);
    if(lag_member->flags & SOC_PPC_LAG_MEMBER_EGRESS_DISABLE) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  INVALID flag for add\n"), FUNCTION_NAME(), tid));
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trunk_sw_db_get_nof_replications(unit, tid, lag_member, &nof_replications, &last_replicated_member_index));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));

    if(nof_replications > 0)
    {
        if(lag_member->flags & SOC_PPC_LAG_MEMBER_INGRESS_DISABLE)
        {
            /* Adding replications with flag INGRESS_DISABLE is not allowed */
            BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  INGRESS_DISABLEd members can have only one replication in lag\n"), FUNCTION_NAME(), tid));
        }
    
        last_replicated_member_position = max_nof_port_in_lag * tid + last_replicated_member_index;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, last_replicated_member_position, &last_replicated_member_flags));
        if(last_replicated_member_flags & (SOC_PPC_LAG_MEMBER_INGRESS_DISABLE | SOC_PPC_LAG_MEMBER_EGRESS_DISABLE))
        {
            /* Only one member with INGRESS_DISABLE or EGRESS_DISABLE and added member is W/O falgs --> remove flags from entry */
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_replicated_member_position, 0x0));
            BCM_EXIT;
        }
    }

    if(nof_members + 1 > max_nof_port_in_lag) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed. excided allowed number of port per lag (%d)\n"), FUNCTION_NAME(), max_nof_port_in_lag ));
    }

    /* add new member */
    if (nof_members == 0) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, TRUE)); 
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, nof_members + 1));
    new_member_id = nof_members;
    new_mamber_position = tid * max_nof_port_in_lag + new_member_id;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, new_mamber_position, lag_member->sys_port));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, new_mamber_position, lag_member->flags));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_remove_all
 * Purpose:
 *      remove all members out of trunk software data-base
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 * Returns:
 *      SOC_E_XXX
 */
int _bcm_dpp_trunk_sw_db_remove_all(int unit, int tid)
{
    int max_nof_tid = 0;
    int nof_members = 0;
    int max_nof_port_in_lag = 0;
    int current_member_index = 0;
    int current_member_position = 0;


    BCMDNX_INIT_FUNC_DEFS;

    /* sanity checks */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }

    /* clear sw db info for given tid */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = max_nof_port_in_lag * tid + current_member_index;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, current_member_position, BCM_GPORT_INVALID));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, current_member_position, 0));
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, 0));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, 0));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, 0));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.set(unit, tid, 0));

exit:
    BCMDNX_FUNC_RETURN;    
}


/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_set
 * Purpose:
 *      set trunk software data-base with lag info
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 *      lag_info                        - lag info to set
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      this function will over-run the data already found in data=base regarding input tid. 
 *      same rulls apply regarding flags as with _bcm_dpp_trunk_sw_db_add.
 */
int _bcm_dpp_trunk_sw_db_set(int unit, int tid, SOC_PPD_LAG_INFO *lag_info)
{
    int max_nof_tid = 0;
    int nof_members = 0;
    int current_member_index = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* sanity checks */
    BCMDNX_NULL_CHECK(lag_info);
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }

    /* clear sw db info for given tid */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trunk_sw_db_remove_all(unit, tid));
    
    /* loop over members and add them one by one */
    nof_members = lag_info->nof_entries;
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_trunk_sw_db_add(unit, tid, &lag_info->members[current_member_index]));            
    }

    /* set genreal configuration for this trunk */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, lag_info->lb_type));
    if (SOC_IS_ARADPLUS(unit)) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.set(unit, tid, lag_info->is_stateful)); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_remove
 * Purpose:
 *      remove member from trunk software data-base
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 *      lag_member                      - member to remove
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      allowed flags are: BCM_TRUNK_MEMBER_EGRESS_DISABLE. when removing with this flag only one replication of the removed member can be found in lag
 *      if member not found - does nothing.
 */
int _bcm_dpp_trunk_sw_db_remove(int unit, int tid, SOC_PPD_LAG_MEMBER *lag_member)
{

    int         last_replicated_member_index = -1;
    int         last_replicated_member_position = -1;
    int         max_nof_port_in_lag = 0;
    int         nof_replications = 0;
    int         max_nof_tid = 0;
    int         nof_members = 0;
    int         last_member_index = 0;
    int         last_member_position = 0;
    int         last_member_system_port = BCM_GPORT_INVALID;
    uint32      last_replicated_member_flags = 0;
    uint32      last_member_flags = 0x0;


    BCMDNX_INIT_FUNC_DEFS;
    
    /* sanity checks */
    BCMDNX_NULL_CHECK(lag_member);
    if(lag_member->flags & SOC_PPC_LAG_MEMBER_INGRESS_DISABLE) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  INVALID flag for remove\n"), FUNCTION_NAME(), tid));
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }
    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trunk_sw_db_get_nof_replications(unit, tid, lag_member, &nof_replications, &last_replicated_member_index));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));

    if(nof_replications > 0)
    {
        /* member is found in lag at least once */
        last_replicated_member_position = max_nof_port_in_lag * tid + last_replicated_member_index;
        if(lag_member->flags & SOC_PPC_LAG_MEMBER_EGRESS_DISABLE)
        {
            if(nof_replications > 1)
            {
                /* Removing when replications exists with flag EGRESS_DISABLE is not allowed */
                BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed.  EGRESS_DISABLEd remove of members is allowed only when one replication in lag\n"), FUNCTION_NAME(), tid));
            }

            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, last_replicated_member_position, &last_replicated_member_flags));
            
            /* Only one member found with INGRESS_DISABLE or EGRESS_DISABLE and removed member is With SOC_PPC_LAG_MEMBER_EGRESS_DISABLE --> update flags to entry */
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_replicated_member_position, SOC_PPC_LAG_MEMBER_EGRESS_DISABLE));
            BCM_EXIT;
        }

        last_member_index = nof_members - 1;
        last_member_position = max_nof_port_in_lag * tid + last_member_index;
        if(nof_members > 1)
        {
            /* at least one more member is found in lag --> copy last member to last replication index */
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, last_member_position, &last_member_system_port));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, last_member_position, &last_member_flags));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, last_replicated_member_position, last_member_system_port));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_replicated_member_position, last_member_flags));         
        } else 
        {
            /* member is the only member in lag */
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, FALSE));
        }

        /* delete last member in lag and update nof_members in lag */
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, last_member_position, BCM_GPORT_INVALID));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_member_position, 0x0));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, nof_members - 1));
    }

exit:
    BCMDNX_FUNC_RETURN;
}   


/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_set_trunk_attributes
 * Purpose:
 *      set trunk attributes - port selection creteria and is stateful.
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 *      psc                             - port selection criteria
 *      is_stateful                     - indication if in stateful mode
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      this function doesn't verify psc and is_stateful are valid inputs
 */
int _bcm_dpp_trunk_sw_db_set_trunk_attributes(int unit, int tid, int psc, int is_stateful)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, psc));
    
    if(SOC_IS_ARADPLUS(unit)){
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, psc));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_trunk_sw_db_get
 * Purpose:
 *      gets trunk member's info from software database.
 * Parameters:
 *      unit                            - Device Number
 *      tid                             - Trunk id
 *      lag_info                        - retrieved info
 * Returns:
 *      SOC_E_XXX
 */
int _bcm_dpp_trunk_sw_db_get(int unit, int tid, SOC_PPD_LAG_INFO *lag_info)
{
    int             nof_members = 0;
    int             max_nof_port_in_lag = 0;
    int             current_member_index = 0;
    int             current_member_position = 0;    
    int             is_stateful = 0;
    int             psc = 0;
    bcm_gport_t     system_port = BCM_GPORT_INVALID;

    BCMDNX_INIT_FUNC_DEFS;
    
    /* sanity checks */
    BCMDNX_NULL_CHECK(lag_info);

    /* get info */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    lag_info->nof_entries = nof_members;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.get(unit, tid, &psc));
    lag_info->lb_type = psc;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.get(unit, tid, &is_stateful));
    lag_info->is_stateful = is_stateful;

    /* get members */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = tid * max_nof_port_in_lag + current_member_index;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, current_member_position, &system_port));
        lag_info->members[current_member_index].sys_port = system_port;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, current_member_position, &lag_info->members[current_member_index].flags));
    }

exit:
    BCMDNX_FUNC_RETURN;    
}

