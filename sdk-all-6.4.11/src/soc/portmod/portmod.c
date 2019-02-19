/*
 *         
 * $Id:$
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
 *     
 */

#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/group_member_list.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/wb_engine.h>

        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define PMM_WB_BUFFER_ID (0)
#define INVALID_PM_ID (-1)
#define INVALID_PORT (-1)
#define SUB_PHYS_NUM (4)
#define MAX_PHYS_PER_PORT (24)
#define MAX_ILKN_AGGREGATED_PMS (6)

typedef enum pmm_wb_var_ids_e{
    PMM_WB_PORT_PM_ID_MAP,
    PMM_WB_PORT_ALIAS_MAP,
    PMM_WB_PHY_PM_MAP,
    PMM_WB_PORT_INTERFACE_TYPE_MAP,
    PMM_WB_PORT_DB_PHYS,
    PMM_WB_PORT_DB_PORTS,
    PMM_WB_XPHY_DB_ADDR,
    PMM_WB_VARS_COUNT
}pmm_wb_var_ids;


typedef struct pmm_info_s{
    int unit;
    int pm_null_support;
    uint32 pms_in_use;
    pm_info_t pms;
    uint32 wb_vars_in_use;
    uint32 max_phys;
    uint32 max_ports;
    group_member_list_t ports_phys_mapping; 
    uint32 max_pms;
}pmm_info_t;
  

pmm_info_t *_pmm_info[SOC_MAX_NUM_DEVICES] = {NULL};

STATIC
int portmod_pmm_free(int unit, pmm_info_t *pmm){
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    SOC_NULL_CHECK(pmm);
    if(pmm->pms != NULL){
        for(i = 0; i< pmm->pms_in_use; i++){
            if(pmm->pms[i].pm_data.pm4x25_db != NULL){ /*all members of the union are pointers*/
                LOG_WARN(BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "potential memory leak: pm %d wasn't NULL at pmm free\n"),
                          i));
            }
        }
        sal_free(pmm->pms);
    }
    sal_free(pmm);
exit:
    SOC_FUNC_RETURN; 
}


STATIC
int port_db_port_set(void *user_data, group_entry_id_t group_id, group_entry_t* group){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_SET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, group, group_id);
}

STATIC
int port_db_port_get(void *user_data, group_entry_id_t group_id, group_entry_t* group){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_GET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, group, group_id);
}

STATIC
int port_db_phy_set(void *user_data, member_entry_id_t member_id, member_entry_t* member){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_SET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, member, member_id);
}

STATIC
int port_db_phy_get(void *user_data, member_entry_id_t member_id, member_entry_t* member){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_GET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, member, member_id);
}



int portmod_xphy_db_addr_set(int unit, int idx, int xphy_addr)
{
    int rv;

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, &xphy_addr, idx);
    return rv;
}

int portmod_pm_create_info_internal_t_init(int unit, portmod_pm_create_info_internal_t *create_info_internal)
{
    sal_memset(create_info_internal, 0 , sizeof(*create_info_internal));
    return SOC_E_NONE;
}

/*calculate how many PMs objects are required for each PM type*/
STATIC
int _portmod_pm_type_to_nof_pms(int unit, portmod_dispatch_type_t pm_type, int *nof_pms)
{
    SOC_INIT_FUNC_DEFS;

    switch(pm_type){
#ifdef PORTMOD_PM4X25_SUPPORT
    case portmodDispatchTypePm4x25:
#ifdef PORTMOD_PM4X25TD_SUPPORT
    case portmodDispatchTypePm4x25td:
#endif /* PORTMOD_PM4X25TD_SUPPORT */
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM4X10_SUPPORT
    case portmodDispatchTypePm4x10:
#ifdef PORTMOD_PM4X10TD_SUPPORT
    case portmodDispatchTypePm4x10td:
#endif /*PORTMOD_PM4X10TD_SUPPORT  */
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM12X10_SUPPORT
    case portmodDispatchTypePm12x10:
        *nof_pms = 5; /*Top, 3 times 4x10, 4X25 */
        break;
#endif /*PORTMOD_PM12X10_SUPPORT  */
#ifdef PORTMOD_PM4x10Q_SUPPORT
    case portmodDispatchTypePm4x10Q:
        *nof_pms = 2; /*Top, 4X10Q */
        break;
#endif /*PORTMOD_PM4x10Q_SUPPORT  */
#ifdef PORTMOD_PM_OS_ILKN_SUPPORT
    case portmodDispatchTypePmOsILKN:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM_OS_ILKN_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_SUPPORT
    case portmodDispatchTypeDnx_fabric:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_DNX_FABRIC_SUPPORT  */
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Can't retrieve number of warmboot buffers for the specified PM type %d"), pm_type));
    }
exit:
    SOC_FUNC_RETURN; 
}

STATIC
int _portmod_max_pms_get(int unit, int nof_pm_instances, const portmod_pm_instances_t* pm_instances, int *max_pms)
{
    int i = 0;
    int nof_pms = 0;
    SOC_INIT_FUNC_DEFS;

    *max_pms = 0;
   
    for( i = 0 ; i < nof_pm_instances; i++){
        _SOC_IF_ERR_EXIT(_portmod_pm_type_to_nof_pms(unit, pm_instances[i].type, &nof_pms));
        *max_pms  += nof_pms* pm_instances[i].instances;
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_wb_upgrade_func(int unit, int recovered_version, int new_version) 
{
    int port, phy, pm_id, rv, i;
    SOC_INIT_FUNC_DEFS;

    /* Change from version 1 to version 2:
        pmNull was added as pm_id 0 (optionaly) 
        once it's done it pushes all other pm ids +1 */
    if (recovered_version == 1 &&
        new_version >= 2 &&
        _pmm_info[unit]->pm_null_support) {

        for (port=0 ; port<_pmm_info[unit]->max_ports ; port++) {
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
            _SOC_IF_ERR_EXIT(rv);
            if(pm_id != INVALID_PM_ID){
                pm_id++;
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
        for(phy=0;phy<_pmm_info[unit]->max_phys;phy++){
            for(i=0;i<MAX_PMS_PER_PHY;i++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit,SOC_WB_ENGINE_PORTMOD,PMM_WB_PHY_PM_MAP,phy,i, (uint8*)&pm_id));
                if(pm_id!=INVALID_PM_ID){
                    pm_id++;
                    _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit,SOC_WB_ENGINE_PORTMOD,PMM_WB_PHY_PM_MAP,phy,i, (uint8*)&pm_id));
                }
            }
        }
    }

exit:
    SOC_FUNC_RETURN; 
}

int portmod_create(int unit, int flags, int max_ports, int max_phys, int nof_pm_instances, const portmod_pm_instances_t* pm_instances)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int buffer_id;
    int rv;
    int max_pms = 0;
    pmm_info_t *pmm = NULL;
    portmod_pm_create_info_t pm_null_create_info;
    int max_wb_buffer_ids = 0;
    int max_wb_var_ids = 0 ;

    SOC_INIT_FUNC_DEFS;
 
    COMPILER_REFERENCE(buffer_is_dynamic);

    _SOC_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_null_create_info));

    _SOC_IF_ERR_EXIT(portmod_pm_instances_t_validate(unit, pm_instances));
    
    if(nof_pm_instances <= 0){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("nof_pm_instances must be > 0")));
    }

    if(_pmm_info[unit] != NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod already created for the unit")));
    }

    _SOC_IF_ERR_EXIT(_portmod_max_pms_get(unit, nof_pm_instances, pm_instances, &max_pms));

    if (PORTMOD_CREATE_F_PM_NULL_GET(flags)) {
        max_pms++;
    }

    pmm = sal_alloc(sizeof(pmm_info_t), "unit pmm");
    SOC_NULL_CHECK(pmm);
    _pmm_info[unit] = pmm;
    pmm->pm_null_support = 0;
#ifdef PORTMOD_PMNULL_SUPPORT
    if (PORTMOD_CREATE_F_PM_NULL_GET(flags)) {
        pmm->pm_null_support = 1;
    }
#endif /* PORTMOD_PMNULL_SUPPORT */
    pmm->pms = NULL;
    /*the pms are not saved in WB*/
    pmm->pms = sal_alloc(sizeof(struct pm_info_s)*max_pms, "port_macros");
    SOC_NULL_CHECK(pmm->pms);
    sal_memset(pmm->pms, 0, sizeof(struct pm_info_s)*max_pms);
    pmm->pms_in_use = 0;
    pmm->ports_phys_mapping.groups_count = max_ports;
    pmm->ports_phys_mapping.members_count = max_phys * SUB_PHYS_NUM;
    pmm->ports_phys_mapping.user_data = pmm;
    pmm->ports_phys_mapping.group_set = port_db_port_set;
    pmm->ports_phys_mapping.group_get = port_db_port_get;
    pmm->ports_phys_mapping.member_set = port_db_phy_set;
    pmm->ports_phys_mapping.member_get = port_db_phy_get;
    pmm->max_phys = max_phys;
    pmm->max_ports = max_ports;
    pmm->max_pms = max_pms;
    pmm->unit = unit;
    pmm->wb_vars_in_use = PMM_WB_VARS_COUNT;

    buffer_id = PMM_WB_BUFFER_ID;

    max_wb_buffer_ids = max_pms + 1 + PORTMOD_MAX_NUM_XPHY_SUPPORTED;
    max_wb_var_ids = max_wb_buffer_ids * MAX_VARS_PER_BUFFER ;
 
    _SOC_IF_ERR_EXIT(soc_wb_engine_init_tables(unit, SOC_WB_ENGINE_PORTMOD, max_wb_buffer_ids, max_wb_var_ids )); 
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, PMM_WB_BUFFER_ID, "pmm_buffer", portmod_wb_upgrade_func, VERSION(3), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, "ports_to_pm_id_mapping", PMM_WB_BUFFER_ID, sizeof(int), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, "ports_interface_type", PMM_WB_BUFFER_ID, sizeof(soc_port_if_t), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, "ports_alias", PMM_WB_BUFFER_ID, sizeof(int), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, "phys_to_pm_ids", PMM_WB_BUFFER_ID, sizeof(int), NULL, max_phys, MAX_PMS_PER_PHY, VERSION(1));        
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, "port_db_phys", PMM_WB_BUFFER_ID, group_member_list_member_entry_size_get(), NULL, max_phys*SUB_PHYS_NUM, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, "port_db_ports", PMM_WB_BUFFER_ID, group_member_list_group_entry_size_get(), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, "xphy_db_addr", PMM_WB_BUFFER_ID, sizeof(int), NULL, PORTMOD_MAX_NUM_XPHY_SUPPORTED, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_BUFFER_ID, FALSE));

    if(!SOC_WARM_BOOT(unit)){ /*Cold boot*/
        int phy_id, pm;
        uint32 invalid_pm_id = INVALID_PM_ID;
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, INVALID_PM_ID);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, SOC_PORT_IF_NULL);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, INVALID_PORT);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, INVALID_PM_ID);
        _SOC_IF_ERR_EXIT(rv);
        
        for(phy_id = 0 ; phy_id < max_phys ; phy_id++){
            for(pm = 0 ; pm < MAX_PMS_PER_PHY ; pm++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy_id, pm, (uint8 *)&invalid_pm_id));
            }
        }
        _SOC_IF_ERR_EXIT(group_member_list_init(&pmm->ports_phys_mapping));
    }

    if (SOC_WARM_BOOT(unit)) {
        int xphy_id, xphy_addr;
        for (xphy_id=0; xphy_id<PORTMOD_MAX_NUM_XPHY_SUPPORTED; xphy_id++) {
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, &xphy_addr, xphy_id);
            _SOC_IF_ERR_EXIT(rv);
            portmod_xphy_addr_set(unit, xphy_id, xphy_addr);
        }
    }

    _pmm_info[unit] = pmm;

#ifdef PORTMOD_PMNULL_SUPPORT
    if (PORTMOD_CREATE_F_PM_NULL_GET(flags)) {
        pm_null_create_info.type = portmodDispatchTypePmNull;
        rv = portmod_port_macro_add(unit, &pm_null_create_info);
        _SOC_IF_ERR_EXIT(rv);
    }
#endif /* PORTMOD_PMNULL_SUPPORT */

exit:
    /*free memories in case of error*/
    if (SOC_FUNC_ERROR){
        if(pmm != NULL){
            portmod_pmm_free(unit, pmm);
        }
    }
    SOC_FUNC_RETURN; 
}



int portmod_destroy(int unit)
{
    int i;
    pm_info_t pm_info = NULL;
    SOC_INIT_FUNC_DEFS;
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    if(_pmm_info[unit]->pms != NULL){
        for(i = 0; i< _pmm_info[unit]->pms_in_use; i++){
            _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, i, &pm_info));
            _SOC_IF_ERR_EXIT(portmod_pm_destroy(unit, pm_info));
        }
    }

    _SOC_IF_ERR_EXIT(portmod_xphy_delete_all(unit));

    _SOC_IF_ERR_EXIT(soc_wb_engine_deinit_tables(unit, SOC_WB_ENGINE_PORTMOD));
    portmod_pmm_free(unit, _pmm_info[unit]);
    _pmm_info[unit] = NULL;
exit:
    SOC_FUNC_RETURN;   
}


STATIC
int _portmod_port_macro_internal_add(int unit , const portmod_pm_create_info_internal_t* pm_add_info, int *pm_created_id)
{
    int pm_id, free_slot_found, current_val, i, phy;
    uint32 invalid_pm_id = INVALID_PM_ID;
    int pm_initialized = FALSE; 
    int should_add_to_map = TRUE;
    int wb_buffer_id;
    SOC_INIT_FUNC_DEFS;
    
    pm_id = _pmm_info[unit]->pms_in_use;
    /* WB buffer ID is used to identify the specific PM database
    WB buffer order must be kept to insure WB and ISSU works.
    Buffers order:
        0 - Global Portmod DB
        1..n-1 - PMs buffers.
            when PmNull present it takes pm_id 0, so wb_buffer_id==pm_id
            when it's not present wb_buffer_id==pm_id+1
            The PmNull always tales last buffer
    */
    if (_pmm_info[unit]->pm_null_support) {
#ifdef PORTMOD_PMNULL_SUPPORT
        if (pm_add_info->type == portmodDispatchTypePmNull) {
            wb_buffer_id = _pmm_info[unit]->max_pms;
        } else 
#endif
        {
            wb_buffer_id = pm_id;
        }
    } else {
        wb_buffer_id = pm_id + 1;
    }

    _SOC_IF_ERR_EXIT(portmod_pm_init(unit, pm_add_info, wb_buffer_id, &_pmm_info[unit]->pms[pm_id]));
    pm_initialized = TRUE;
    if(!SOC_WARM_BOOT(unit)){
        /*add to map just in case of cold boot*/
        _SHR_PBMP_ITER(pm_add_info->phys, phy){
            free_slot_found = FALSE;
            for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&current_val));
                if(current_val == INVALID_PM_ID && pm_add_info->type == _pmm_info[unit]->pms[pm_id].type){
                    _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm_id));
                    free_slot_found = TRUE;
                    break;
                }
            }
            if(!free_slot_found){
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("phy (%d) already used by the maximum number of pms %d"), phy, MAX_PMS_PER_PHY));
            }
        }
    }
    _pmm_info[unit]->pms_in_use++;
    *pm_created_id = pm_id;
exit:
    if (SOC_FUNC_ERROR){
        if(pm_initialized){

            portmod_pm_destroy(unit, &_pmm_info[unit]->pms[pm_id]);
        }
        /*clean the map*/
        if(should_add_to_map){
            _SHR_PBMP_ITER(pm_add_info->phys, phy){
                for(i = 0 ; !_rv && (i < MAX_PMS_PER_PHY); i++){
                    _rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&current_val);
                    if(!_rv && (current_val == pm_id)){
                        _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&invalid_pm_id));
                        break;
                    }
                }
            }
        }
    }
    SOC_FUNC_RETURN;   
}


#if defined(PORTMOD_PM4X25_SUPPORT) || defined(PORTMOD_PM4X10_SUPPORT) || defined(PORTMOD_DNX_FABRIC_SUPPORT)
STATIC
int _portmod_simple_pm_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t create_info;
    int specific_info_size = 0;
    int pm_id = 0;
    SOC_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(create_info.phys, pm_add_info->phys);
    create_info.type = pm_add_info->type;

    switch(pm_add_info->type){
#ifdef PORTMOD_PM4X25_SUPPORT
    case portmodDispatchTypePm4x25:
#ifdef PORTMOD_PM4X25TD_SUPPORT
    case portmodDispatchTypePm4x25td:
#endif /* PORTMOD_PM4X25TD_SUPPORT */
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm4x25);
        break;
#endif /*PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM4X10_SUPPORT
    case portmodDispatchTypePm4x10:
#ifdef PORTMOD_PM4X10TD_SUPPORT
    case portmodDispatchTypePm4x10td:
#endif /*PORTMOD_PM4X10TD_SUPPORT  */
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm4x10);
        break;
#endif /*PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_SUPPORT
    case portmodDispatchTypeDnx_fabric:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.dnx_fabric);
        break;
#endif /*PORTMOD_DNX_FABRIC_SUPPORT  */
#ifdef PORTMOD_PMNULL_SUPPORT
    case portmodDispatchTypePmNull:
        specific_info_size = 0;
        break;
#endif /*PORTMOD_PMNULL_SUPPORT  */
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unknown PM type %d"), pm_add_info->type));
    }
    sal_memcpy(&create_info.pm_specific_info, &pm_add_info->pm_specific_info, specific_info_size);
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &create_info, &pm_id));
exit:
    SOC_FUNC_RETURN;  
}
#endif



#ifdef PORTMOD_PM12X10_SUPPORT
STATIC
int _portmod_pm12x10_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    int i = 0;
    int phy;
    int nof_phys = 0;
    int pm4x10_index = 0;
    portmod_pm_create_info_internal_t pm4x10_create_info;
    portmod_pm_create_info_internal_t pm4x25_create_info;
    portmod_pm_create_info_internal_t pm12x10_create_info;
    int pm_ids[] = {INVALID_PM_ID, INVALID_PM_ID, INVALID_PM_ID, INVALID_PM_ID, INVALID_PM_ID};
    SOC_INIT_FUNC_DEFS;

    SOC_PBMP_COUNT(pm_add_info->phys, nof_phys);
    if(nof_phys != 12){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("number of phys should be 12")));
    }

    /*PM4x10*/
    SOC_PBMP_ITER(pm_add_info->phys, phy){
        if( i % 4 == 0){
            if(i != 0){
                _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10_create_info, &pm_ids[pm4x10_index]));
                pm4x10_index ++;
            }
            portmod_pm_create_info_internal_t_init(unit, &pm4x10_create_info);
            sal_memcpy(&pm4x10_create_info.pm_specific_info.pm4x10,&pm_add_info->pm_specific_info.pm12x10.pm4x10_infos[pm4x10_index], sizeof(pm4x10_create_info.pm_specific_info.pm4x10));
#ifdef PORTMOD_PM4X10TD_SUPPORT
            if (PORTMOD_PM12x10_F_USE_PM_TD_GET(pm_add_info->pm_specific_info.pm12x10.flags)) { 
                pm4x10_create_info.type = portmodDispatchTypePm4x10td;
            } else
#endif /* PORTMOD_PM4X10TD_SUPPORT */
            {
                pm4x10_create_info.type = portmodDispatchTypePm4x10;
            }
            pm4x10_create_info.pm_specific_info.pm4x10.in_pm_12x10 = TRUE;
        }
        SOC_PBMP_PORT_ADD(pm4x10_create_info.phys, phy);
        i++;
    }
    /*add the last one*/
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10_create_info, &pm_ids[pm4x10_index]));

    /*PM4x25*/
    i = 0;       
    portmod_pm_create_info_internal_t_init(unit, &pm4x25_create_info);
#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (PORTMOD_PM12x10_F_USE_PM_TD_GET(pm_add_info->pm_specific_info.pm12x10.flags)) {
        pm4x25_create_info.type = portmodDispatchTypePm4x25td;
    } else 
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        pm4x25_create_info.type = portmodDispatchTypePm4x25;
    }
    pm4x25_create_info.pm_specific_info.pm4x25.in_pm_12x10 = TRUE;
    SOC_PBMP_ITER(pm_add_info->phys, phy){
        if( i == 4){
            break;
        }
        SOC_PBMP_PORT_ADD(pm4x25_create_info.phys, phy);
        i++;
    }
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x25_create_info, &pm_ids[3]));
    /*PM12x10*/
    portmod_pm_create_info_internal_t_init(unit, &pm12x10_create_info);
    SOC_PBMP_ASSIGN(pm12x10_create_info.phys, pm_add_info->phys);
    pm12x10_create_info.pm_specific_info.pm12x10.flags = pm_add_info->pm_specific_info.pm12x10.flags;
    pm12x10_create_info.type = portmodDispatchTypePm12x10;
    pm12x10_create_info.pm_specific_info.pm12x10.blk_id = pm_add_info->pm_specific_info.pm12x10.blk_id;
    pm12x10_create_info.pm_specific_info.pm12x10.pm4x25 = &_pmm_info[unit]->pms[pm_ids[3]];
    for(i = 0 ; i < 3 ; i++)
    {
        pm12x10_create_info.pm_specific_info.pm12x10.pm4x10[i] = &_pmm_info[unit]->pms[pm_ids[i]];
    }
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm12x10_create_info, &pm_ids[4]));

exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_PM12X10_SUPPORT*/




#ifdef PORTMOD_PM4x10Q_SUPPORT
STATIC
int _portmod_pm4x10q_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t pm4x10_create_info;
    portmod_pm_create_info_internal_t pm4x10q_create_info;
    int pm4x10_id = INVALID_PM_ID;
    int pm4x10q_id = INVALID_PM_ID;
    SOC_INIT_FUNC_DEFS;

    portmod_pm_create_info_internal_t_init(unit, &pm4x10_create_info);
    sal_memcpy(&pm4x10_create_info.pm_specific_info.pm4x10, &pm_add_info->pm_specific_info.pm4x10q.pm4x10_info, sizeof(pm4x10_create_info.pm_specific_info.pm4x10));
    pm4x10_create_info.type = portmodDispatchTypePm4x10;
    SOC_PBMP_ASSIGN(pm4x10_create_info.phys, pm_add_info->phys);
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10_create_info, &pm4x10_id));
  
    portmod_pm_create_info_internal_t_init(unit, &pm4x10q_create_info);
    pm4x10q_create_info.type = portmodDispatchTypePm4x10Q;
    SOC_PBMP_ASSIGN(pm4x10q_create_info.phys, pm_add_info->phys);
    pm4x10q_create_info.pm_specific_info.pm4x10q.pm4x10 = &_pmm_info[unit]->pms[pm4x10_id];
    pm4x10q_create_info.pm_specific_info.pm4x10q.blk_id = pm_add_info->pm_specific_info.pm4x10q.blk_id;
    pm4x10q_create_info.pm_specific_info.pm4x10q.qsgmii_user_acc = pm_add_info->pm_specific_info.pm4x10q.qsgmii_user_acc;
    pm4x10q_create_info.pm_specific_info.pm4x10q.pm4x10_user_acc = pm_add_info->pm_specific_info.pm4x10q.pm4x10_info.access.user_acc;
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10q_create_info, &pm4x10q_id));

exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_PM4x10Q_SUPPORT*/


#ifdef PORTMOD_PM_OS_ILKN_SUPPORT
STATIC
int _portmod_os_ilkn_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t os_ilkn_create_info;
    int i, j, phy, tmp_pm_id, rv = 0;
    pm_info_t tmp_pm_info, ilkn_pms[MAX_ILKN_AGGREGATED_PMS] = {NULL};
    portmod_dispatch_type_t pm_type;
    int pm_id= INVALID_PM_ID;
    SOC_INIT_FUNC_DEFS;

    if(pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms > MAX_ILKN_AGGREGATED_PMS)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Number of PMs(%d) should be less the %d"), pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms, MAX_ILKN_AGGREGATED_PMS));
    }
    portmod_pm_create_info_internal_t_init(unit, &os_ilkn_create_info);
    os_ilkn_create_info.type = portmodDispatchTypePmOsILKN;
    SOC_PBMP_ASSIGN(os_ilkn_create_info.phys, pm_add_info->phys);

    os_ilkn_create_info.pm_specific_info.os_ilkn.wm_high = pm_add_info->pm_specific_info.os_ilkn.wm_high;
    os_ilkn_create_info.pm_specific_info.os_ilkn.wm_low = pm_add_info->pm_specific_info.os_ilkn.wm_low;
    os_ilkn_create_info.pm_specific_info.os_ilkn.nof_aggregated_pms = pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms;
    os_ilkn_create_info.pm_specific_info.os_ilkn.pms = ilkn_pms;
    os_ilkn_create_info.pm_specific_info.os_ilkn.is_over_fabric = pm_add_info->pm_specific_info.os_ilkn.is_over_fabric;
    for(i = 0 ; i < pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms; i++)
    {
         for( j = 0 ; j < MAX_PMS_PER_PHY ; j++){
             phy = pm_add_info->pm_specific_info.os_ilkn.controlled_pms[i].phy;
             pm_type = pm_add_info->pm_specific_info.os_ilkn.controlled_pms[i].type;
             rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &tmp_pm_id, phy, j);
             _SOC_IF_ERR_EXIT(rv);
             if(tmp_pm_id == INVALID_PM_ID){
                 _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("PM of type %d not found in PHY %d"), pm_type, phy));
             }
             
             rv = portmod_pm_info_from_pm_id_get(unit, tmp_pm_id, &tmp_pm_info);
             _SOC_IF_ERR_EXIT(rv);
             if(tmp_pm_info->type == pm_type){
                 ilkn_pms[i] = tmp_pm_info;
                 break;
             }

         }
    }
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &os_ilkn_create_info, &pm_id));
exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_PM_OS_ILKN_SUPPORT*/


int portmod_port_macro_add(int unit, const portmod_pm_create_info_t* pm_add_info)
{
    SOC_INIT_FUNC_DEFS;
    
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    if(pm_add_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("pm_add_info NULL parameter")));
    }
    switch(pm_add_info->type){
#ifdef PORTMOD_PMNULL_SUPPORT
    case portmodDispatchTypePmNull:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PMNULL_SUPPORT  */
#ifdef PORTMOD_PM12X10_SUPPORT
    case portmodDispatchTypePm12x10:
        _SOC_IF_ERR_EXIT(_portmod_pm12x10_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM12X10_SUPPORT  */
#ifdef PORTMOD_PM4X25_SUPPORT
    case portmodDispatchTypePm4x25:
#ifdef PORTMOD_PM4X25TD_SUPPORT
    case portmodDispatchTypePm4x25td:
#endif /* PORTMOD_PM4X25TD_SUPPORT */
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM4X10_SUPPORT
    case portmodDispatchTypePm4x10:
#ifdef PORTMOD_PM4X10TD_SUPPORT
    case portmodDispatchTypePm4x10td:
#endif /*PORTMOD_PM4X10TD_SUPPORT  */
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_SUPPORT
    case portmodDispatchTypeDnx_fabric:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_DNX_FABRIC_SUPPORT  */
#ifdef PORTMOD_PM4x10Q_SUPPORT
    case portmodDispatchTypePm4x10Q:
        _SOC_IF_ERR_EXIT(_portmod_pm4x10q_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4x10Q_SUPPORT  */
#ifdef PORTMOD_PM_OS_ILKN_SUPPORT
    case portmodDispatchTypePmOsILKN:
        _SOC_IF_ERR_EXIT(_portmod_os_ilkn_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM_OS_ILKN_SUPPORT  */        
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid PM type %d"), pm_add_info->type));
    }
exit:
    SOC_FUNC_RETURN;   
}

STATIC
int portmod_validate_all_phys_in_pm(int unit, int pm_id, soc_pbmp_t phys, int *all_phys_in_pm){
    int i, phy, other_pm_id, belongs_to_pm, rv;
    SOC_INIT_FUNC_DEFS;
 
    *all_phys_in_pm = TRUE;
    SOC_PBMP_ITER(phys, phy){
        belongs_to_pm = FALSE;
        for( i = 0 ; i < MAX_PMS_PER_PHY ; i++){
            rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &other_pm_id, phy, i);
            _SOC_IF_ERR_EXIT(rv);
            if(pm_id == other_pm_id){
                belongs_to_pm = TRUE;
                break;
            }else if(other_pm_id == INVALID_PM_ID){
                /*end of list*/
                break;
            }
        }
        if(!belongs_to_pm){
              *all_phys_in_pm = FALSE;
              break;
        }
    }
exit:
    SOC_FUNC_RETURN;
}


int portmod_port_add(int unit, int port, const portmod_port_add_info_t *port_add_info)
{
    group_entry_id_t port_num;
    pm_info_t pm_info = NULL;
    int pm_id, rv, phy, phys_count = 0, sub_phy = 0, i, is_interface_supported, all_phys_in_pm = FALSE;
    int is_alias = 0, init_all = 0;
#ifdef PORTMOD_PM4X10_SUPPORT
    int is_qsgmii = 0;
#endif
    soc_port_if_t interface;
    int master_port;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    SOC_NULL_CHECK(port_add_info);

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(port_add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(port_add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(port_add_info)) ? 1 : 0;

    /* do this only once (first time) per port */
    if((PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(port_add_info)) || (init_all)) {
        /*check that port is not already in use*/
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
        _SOC_IF_ERR_EXIT(rv);
        if(pm_id != INVALID_PM_ID){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Port %d already in use"), port));
        }
        
        /*check for overlaps*/
        if (port_add_info->interface_config.interface != SOC_PORT_IF_NULL) {
            if(port_add_info->interface_config.interface == SOC_PORT_IF_QSGMII){
                SOC_PBMP_COUNT(port_add_info->phys, phys_count);
                if(phys_count != 1){
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("QSGMII must has one lane")));
                }
#ifdef PORTMOD_PM4X10_SUPPORT
                is_qsgmii = 1;
#endif
                sub_phy = port_add_info->sub_phy;
            } else {
                sub_phy = 0; /* sub phy is zero for non QSGMII */
                SOC_PBMP_ITER(port_add_info->phys, phy){
                    rv = group_member_list_group_get(&_pmm_info[unit]->ports_phys_mapping, phy*SUB_PHYS_NUM + sub_phy, &port_num);
                    _SOC_IF_ERR_EXIT(rv);
                    if(port_num != GROUP_MEM_LIST_END){
                        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, port_num);
                        _SOC_IF_ERR_EXIT(rv);
                        if(port_add_info->interface_config.interface == interface) {
                            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &port_num, port);
                            _SOC_IF_ERR_EXIT(rv);
                            is_alias = 1;
                            break;
                        } else {
                            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d overlap with port %d"), port, port_num));
                        }
                    }
                }
            }
        }

        if (!is_alias) {
            /*find the PM to add the port to*/
            SOC_PBMP_ITER(port_add_info->phys, phy){
                break;
            }

            if (port_add_info->interface_config.interface == SOC_PORT_IF_NULL) {
                pm_id = 0; /* pmNull is always added first, so it's pm_id will be always 0 */
            } else {
                is_interface_supported = FALSE;
                for( i = 0 ; i < MAX_PMS_PER_PHY ; i++){
                    rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &pm_id, phy, i);
                    _SOC_IF_ERR_EXIT(rv);
                    if(pm_id == INVALID_PM_ID){
                        break;
                    } else {
                        rv = portmod_pm_info_from_pm_id_get(unit, pm_id, &pm_info);
                        _SOC_IF_ERR_EXIT(rv);
#ifdef PORTMOD_PM4X10_SUPPORT
                        if (is_qsgmii) {
                            if (pm_info->type == portmodDispatchTypePm4x10) continue;
#ifdef PORTMOD_PM4X10TD_SUPPORT
                            if (pm_info->type == portmodDispatchTypePm4x10td) continue;
#endif
                        }
#endif
                        rv = portmod_pm_interface_type_is_supported(unit, pm_info, port_add_info->interface_config.interface, &is_interface_supported); 
                        _SOC_IF_ERR_EXIT(rv);
                        if(is_interface_supported){
                            rv = portmod_validate_all_phys_in_pm(unit, pm_id, port_add_info->phys, &all_phys_in_pm);
                            _SOC_IF_ERR_EXIT(rv);
                            if(all_phys_in_pm){
                                break;
                            }
                        }
                    }
                }
                if((!is_interface_supported) || (!all_phys_in_pm)){
                    _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("No PM found for the specified phy %d and interface_type %d"), phy, port_add_info->interface_config.interface));
                }
            
                /*adding the new port to the PMM*/
                SOC_PBMP_ITER(port_add_info->phys, phy){
                    rv = group_member_list_member_add(&_pmm_info[unit]->ports_phys_mapping, port, phy*SUB_PHYS_NUM + sub_phy);
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &(port_add_info->interface_config.interface), port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &port, port);
            _SOC_IF_ERR_EXIT(rv);
        } else {

            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port, port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, master_port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, master_port);
            _SOC_IF_ERR_EXIT(rv);


            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
            _SOC_IF_ERR_EXIT(rv);
        }
    } 

    if (!is_alias) {
        /*add port in PM level */
        rv = portmod_port_attach(unit, port, port_add_info);
        if(SOC_FAILURE(rv)){
            int invalid_pm = INVALID_PM_ID, rv_warn;
            soc_port_if_t invalid_interface = SOC_PORT_IF_NULL;
            soc_port_if_t invalid_port = -1;
            /*remove from PMM*/
            group_member_list_group_remove(&_pmm_info[unit]->ports_phys_mapping, port);
            rv_warn = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &invalid_pm, port);
            if(SOC_FAILURE(rv_warn)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail remove port %d from PMM_WB_PORT_PM_ID_MAP\n"), port));
            }
            rv_warn = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &invalid_interface, port);
            if(SOC_FAILURE(rv_warn)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail remove port %d from PMM_WB_PORT_INTERFACE_TYPE_MAP\n"), port));
            }
            rv_warn = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &invalid_port, port);
            if(SOC_FAILURE(rv_warn)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail remove port %d from PMM_WB_PORT_ALIAS_MAP\n"), port));
            }
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int portmod_port_remove(int unit, int port){
    int rv;
    int invalid_pm = INVALID_PM_ID;
    int is_master = 0, is_channelized = 0, master_port = 0, new_master_port = 0, port_i = 0, master_port_i = 0, i;
    soc_port_if_t invalid_interface = SOC_PORT_IF_NULL;
    soc_port_if_t invalid_port = -1, port_interface;
    uint32 phys[MAX_PHYS_PER_PORT];
    uint32 phys_count;

    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, 
                                &port_interface, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail port %d interface get"), port));
    }

    /* check if master port */
    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port, port));
    if (port == master_port) {
        is_master = 1;
    }

    if (is_master) {

        /* check if channelized (if other ports exist on the same interface) and find new master port*/
        for (port_i = 0; port_i < _pmm_info[unit]->max_ports; port_i++){
            if (port == port_i) {
                continue;
            }
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port_i, port_i));
            if (master_port_i == port) {
                is_channelized = 1;
                new_master_port = port_i;
                break;
            }
        }
        if (!is_channelized) {
            _SOC_IF_ERR_EXIT(portmod_port_detach(unit, port));

            if (port_interface != SOC_PORT_IF_NULL) {
                /*remove from PMM*/
                rv = group_member_list_group_remove(&_pmm_info[unit]->ports_phys_mapping, port);
                if(rv != SOC_E_NONE){
                    LOG_ERROR(BSL_LS_SOC_PORT,
                              (BSL_META_U(unit,
                                      "fail remove port %d from port to phys map\n"), port));
                }
            }
        } else { /* channelized interface */

            /* replace port mapping */
            _SOC_IF_ERR_EXIT(portmod_port_replace(unit, port, new_master_port));

            /* update ALIAS map*/
            for (port_i = 0; port_i < _pmm_info[unit]->max_ports; port_i++){
                _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port_i, port_i));
                if (master_port_i == port) {
                    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &new_master_port, port_i));
                }
            }


            /* start - update phys mapping */
            rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
            _SOC_IF_ERR_EXIT(rv);
            if(phys_count < 1) {
                _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("No Phys attached to port")));
            }

            /* remove old master from PMM */
            if (port_interface != SOC_PORT_IF_NULL) {
                rv = group_member_list_group_remove(&_pmm_info[unit]->ports_phys_mapping, port);
                if(rv != SOC_E_NONE){
                    LOG_ERROR(BSL_LS_SOC_PORT,
                              (BSL_META_U(unit,
                                          "fail remove port %d from port to phys map\n"), port));
                }
            }

            /*adding the new master port to the PMM*/
            for(i = 0; i < phys_count; i++){
                 rv = group_member_list_member_add(&_pmm_info[unit]->ports_phys_mapping, new_master_port, phys[i]);
                _SOC_IF_ERR_EXIT(rv);
            }

            /* end - update phys mapping */
        }        
    }

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &invalid_port, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail remove port %d from port alias map"), port));
    }

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &invalid_pm, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail remove port %d from port to pm map\n"), port));
    }
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &invalid_interface, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail remove port %d from port to interface type map"), port));
    }

    
exit:
    SOC_FUNC_RETURN;
}


int portmod_next_wb_var_id_get(int unit, int *var_id){
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    *var_id = _pmm_info[unit]->wb_vars_in_use;
    _pmm_info[unit]->wb_vars_in_use++;

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_port_is_valid
 *
 * @brief Get the specific port has valid pm.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out] valid           - TRUE if port has valid pm_id
 */
int portmod_port_is_valid(int unit, int port, int* valid) {
    int rv, pm_id;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
    _SOC_IF_ERR_EXIT(rv);

    *valid = ((pm_id >= _pmm_info[unit]->pms_in_use) || (pm_id == INVALID_PM_ID)) ?
                   FALSE : TRUE;
exit:
    SOC_FUNC_RETURN;
}

/*PM info retreive*/
int portmod_port_pm_id_get(int unit, int port, int *pm_id){
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, pm_id, port);
    _SOC_IF_ERR_EXIT(rv);
    if((*pm_id >= _pmm_info[unit]->pms_in_use) || (*pm_id == INVALID_PM_ID)){
        _SOC_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_SOC_MSG("Valid PM Not found.")));
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_pm_id_pm_type_get(int unit, int pm_id, portmod_dispatch_type_t *type){
    pm_info_t pm_info;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, pm_id, &pm_info));
    *type = pm_info->type;
exit:
    SOC_FUNC_RETURN;  
}


int portmod_pm_info_get(int unit, int port, pm_info_t* pm_info){
    int pm_id;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
    *pm_info = &_pmm_info[unit]->pms[pm_id];
    if(*pm_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("pm_info null not as expected")));
    }
exit:
    SOC_FUNC_RETURN;  
}

int portmod_pm_info_type_get(int unit, int port, portmod_dispatch_type_t type, pm_info_t* pm_info)
{
    int pm_id, i, rv, actual_phy;
    uint32 phys[MAX_PHYS_PER_PORT];
    uint32 phys_count;
    SOC_INIT_FUNC_DEFS;

    *pm_info = NULL;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
    _SOC_IF_ERR_EXIT(rv);

    if(phys_count < 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("No Phys attached to port")));
    }

    actual_phy = phys[0]/SUB_PHYS_NUM;

    for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
        _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, actual_phy, i, (uint8 *)&pm_id));
        if(pm_id == INVALID_PM_ID){
            break; 
        }

        if(type == _pmm_info[unit]->pms[pm_id].type) {
            *pm_info = &_pmm_info[unit]->pms[pm_id];
            break;
        }
    }

    if(*pm_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("pm_info null not found")));
    }

exit:
    SOC_FUNC_RETURN; 
}

int portmod_pm_info_from_pm_id_get(int unit, int pm_id, pm_info_t* pm_info){
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    if((pm_id >= _pmm_info[unit]->pms_in_use)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid pm")));
    }
    *pm_info = &_pmm_info[unit]->pms[pm_id];
    if(*pm_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("pm_info null not as expected")));
    }
exit:
    SOC_FUNC_RETURN;  
}




int portmod_port_pm_type_get(int unit, int port, int *real_port, portmod_dispatch_type_t* type){
    pm_info_t pm_info;
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, real_port, port);
    if (SOC_FAILURE(rv)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, ("port %d doesn't exist", port));
    }

    /* if real_port is invalid, it should not call portmod_pm_info_get with invalid port.*/
    if(*real_port == INVALID_PORT) {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, ("port %d is not valid.", port));        
    }

    _SOC_IF_ERR_EXIT(portmod_pm_info_get(unit, *real_port, &pm_info));
    *type = pm_info->type;
exit:
    SOC_FUNC_RETURN;     
}



int portmod_pms_num_get(int unit, int *pms_num){
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pms_num);
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    *pms_num = _pmm_info[unit]->pms_in_use;
exit:
    SOC_FUNC_RETURN;
}

int portmod_port_interface_type_get(int unit, int port, soc_port_if_t *interface){
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, interface, port);
    _SOC_IF_ERR_EXIT(rv);
   
exit:
    SOC_FUNC_RETURN;   
}

int portmod_pm_diag_info_get(int unit, int pm_id, portmod_pm_diag_info_t *diag_info){
    int i, phy, port, pm;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    SOC_NULL_CHECK(diag_info);

    if(pm_id >= _pmm_info[unit]->pms_in_use){
       diag_info->type = portmodDispatchTypeCount;
       SOC_EXIT;
    }
    SOC_PBMP_CLEAR(diag_info->phys);
    diag_info->type = _pmm_info[unit]->pms[pm_id].type;
    for( phy = 0 ; phy < _pmm_info[unit]->max_phys; phy++){
         for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
            _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm));
            if(pm == INVALID_PM_ID){
                break; 
            }else if (pm == pm_id){
                SOC_PBMP_PORT_ADD(diag_info->phys, phy);
                break;
            }
        }

    }
    SOC_PBMP_CLEAR(diag_info->ports);
    for( port = 1 ; port < _pmm_info[unit]->max_ports; port++){
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm, port));
        /*bsl_printf("portmod_pm_diag_info_get: unit=%d,port=%d,pm=%d,pmid=%d\r\n", unit, port, pm, pm_id);*/
        if(pm == INVALID_PM_ID){
            continue; 
        }else if (pm == pm_id){
            SOC_PBMP_PORT_ADD(diag_info->ports, port);
            continue;
        }
    }

    _SOC_IF_ERR_EXIT(portmod_pm_core_info_get(unit, &_pmm_info[unit]->pms[pm_id], -1, &diag_info->core_info));    
    
exit:
    SOC_FUNC_RETURN;
}

/* Combo port control/status values. */
typedef enum _portmod_port_medium_e {
    PORTMOD_PORT_MEDIUM_NONE,   /* _SHR_PORT_MEDIUM_NONE   */
    PORTMOD_PORT_MEDIUM_COPPER, /* _SHR_PORT_MEDIUM_COPPER */
    PORTMOD_PORT_MEDIUM_FIBER,  /* _SHR_PORT_MEDIUM_FIBER  */
    PORTMOD_PORT_MEDIUM_COUNT   /* _SHR_PORT_MEDIUM_COUNT  */
} _portmod_port_medium_t;

int portmod_port_diag_info_get(int unit, int port, portmod_port_diag_info_t *diag_info){
    member_entry_id_t phys[MAX_PHYS_PER_PORT];
    uint32 phys_count = 0;
    int rv;
    int i, nof_phys;
    portmod_access_get_params_t access_param;
    phymod_phy_access_t phy_access;
    phymod_phy_access_t phy_access12[3];
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    SOC_NULL_CHECK(diag_info);
    rv = portmod_port_diag_info_t_init(unit, diag_info);
    _SOC_IF_ERR_EXIT(rv);
    if(port >= _pmm_info[unit]->max_ports){
        diag_info->pm_id = INVALID_PM_ID;
        SOC_EXIT;
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &diag_info->original_port, port);
    _SOC_IF_ERR_EXIT(rv);
    if(diag_info->original_port == INVALID_PORT){
        diag_info->pm_id = INVALID_PM_ID;
        SOC_EXIT;
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &diag_info->interface, port);
    _SOC_IF_ERR_EXIT(rv);
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &diag_info->pm_id, port);
    _SOC_IF_ERR_EXIT(rv);
    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
    _SOC_IF_ERR_EXIT(rv);
    SOC_PBMP_CLEAR(diag_info->phys);
    if(phys_count != 0){
        diag_info->sub_phy = phys[phys_count-1] % SUB_PHYS_NUM;
        for(i = 0; i < phys_count ;i++){
            SOC_PBMP_PORT_ADD(diag_info->phys, phys[i] / SUB_PHYS_NUM);
        }
    }

    if (diag_info->interface == SOC_PORT_IF_NULL)
        return (SOC_E_NONE);

    /* get phy polority */
    rv = portmod_access_get_params_t_init(unit, &access_param);
    _SOC_IF_ERR_EXIT(rv);
    if (IS_C_PORT(unit, port)) {
        
        access_param.phyn = 0;
        rv = portmod_port_phy_lane_access_get(unit, port, &access_param,
                                              3, &phy_access12[0], &nof_phys, NULL);
    } else {
        
        access_param.phyn = 0;
        rv = portmod_port_phy_lane_access_get(unit, port, &access_param, 
                                              1, &phy_access, &nof_phys, NULL);
    }
    _SOC_IF_ERR_EXIT(rv);
    if (!SAL_BOOT_SIMULATION) {
        if (IS_C_PORT(unit, port)) {
            rv = phymod_phy_polarity_get(&phy_access12[0], &diag_info->polarity);
        } else {
            rv = phymod_phy_polarity_get(&phy_access, &diag_info->polarity);
        }
        _SOC_IF_ERR_EXIT(rv);
    }

    rv = portmod_port_mode_get(unit, port, &diag_info->core_mode);
    _SOC_IF_ERR_EXIT(rv);
    
    /* CHECK CHECK WITH DORON */
    diag_info->medium = PORTMOD_PORT_MEDIUM_NONE;
exit:
    SOC_FUNC_RETURN;   
}

int portmod_port_first_phy_get(int unit, int port, int *first_phy, int *sub_phy){
    member_entry_id_t phys[MAX_PHYS_PER_PORT];
    uint32 phys_count = 0;
    int i;
    int min_phy;
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    
    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
    _SOC_IF_ERR_EXIT(rv);

    min_phy = _pmm_info[unit]->max_phys * SUB_PHYS_NUM;
    /*git the minimal one*/
    for(i = 0; i < phys_count ;i++){
        if(phys[i] < min_phy){
            min_phy = phys[i];
        }
    }
    *first_phy = min_phy / SUB_PHYS_NUM;
    *sub_phy = min_phy % SUB_PHYS_NUM;
exit:
    SOC_FUNC_RETURN; 
}


/*get all the PMS of specific phy*/
int portmod_phy_pms_info_get(int unit, int phy, int max_pms, pm_info_t *pms_info, int *nof_pms){
    int pm_id = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    *nof_pms = 0;
    for( i = 0 ; i < MAX_PMS_PER_PHY ; i++){
        _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm_id));
        if(pm_id == INVALID_PM_ID){
            break;
        }
        if (max_pms == *nof_pms){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Array supplied has less entries than needed")));
        }
        _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, pm_id, &pms_info[*nof_pms]));
        *nof_pms += 1;
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_ext_phy_lane_attach(int unit, int iphy, int phyn, const portmod_lane_connection_t* lane_connection)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }

    /* Attach the external phy to the phy chain of each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_ext_phy_lane_attach_to_pm(unit, pms_info[i], iphy, phyn, lane_connection));
    }
    SOC_FUNC_RETURN;
exit:
    /* Rollingback the attach operations because of an error */
    --i;
    for( ; i >= 0 ; --i) {
        portmod_ext_phy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, NULL);
    }
    SOC_FUNC_RETURN;
}

int portmod_ext_phy_lane_detach(int unit, int iphy, int phyn)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    portmod_lane_connection_t lane_connection[MAX_PMS_PER_PHY];

    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }
    /* Detach the last external phy in the phy chain from each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_ext_phy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, &lane_connection[i]));
    }
    SOC_FUNC_RETURN;
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_lane_attach(int unit, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_conn)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }

    /* Attach the external phy to the phy chain of each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_xphy_lane_attach_to_pm(unit, pms_info[i], iphy, phyn, lane_conn));
    }
    SOC_FUNC_RETURN;
exit:
    /* Rollingback the attach operations because of an error */
    --i;
    for( ; i >= 0 ; --i) {
        portmod_xphy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, NULL);
    }
    SOC_FUNC_RETURN;
}

int portmod_xphy_lane_detach(int unit, int iphy, int phyn)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    portmod_xphy_lane_connection_t lane_conn[MAX_PMS_PER_PHY];

    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }
    /* Detach the last external phy in the phy chain from each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_xphy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, &lane_conn[i]));
    }
    SOC_FUNC_RETURN;
exit:
    SOC_FUNC_RETURN;
}

int portmod_max_pms_get(int unit, int* max_pms)
{
    SOC_INIT_FUNC_DEFS;
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    *max_pms = _pmm_info[unit]->max_pms;
exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_pm_type_get(int unit, int phy, portmod_dispatch_type_t *type) 
{ 
    int nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY];

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, phy, MAX_PMS_PER_PHY, pms_info, &nof_pms));

    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }

    *type = pms_info[nof_pms-1]->type;

exit:
    SOC_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME
