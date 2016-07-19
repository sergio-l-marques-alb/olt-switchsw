/*
 *         
 * $Id: portmod_chain.c,v 1.2.2.13 Broadcom SDK $
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
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/portmod/portmod_chain.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define PM_EXTPHY_ITER(rv, phy_idx)    while((PHYMOD_E_UNAVAIL== rv) && (phy_idx >= 0))
#define PM_ALLPHY_ITER(rv, phy_idx)    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_idx >= 0))

STATIC
portmod_ext_phy_core_info_t *_ext_phy_info[SOC_MAX_NUM_DEVICES][MAX_PHYN][SOC_MAX_NUM_PORTS] = {{{NULL}}};

STATIC
portmod_xphy_core_info_t* _xphy_info[SOC_MAX_NUM_DEVICES][PORTMOD_MAX_NUM_XPHY_SUPPORTED];

STATIC
int _xphy_addr[SOC_MAX_NUM_DEVICES][PORTMOD_MAX_NUM_XPHY_SUPPORTED];

int portmod_xphy_addr_set(int unit, int idx, int xphy_addr)
{
    _xphy_addr[unit][idx] = xphy_addr;
    return SOC_E_NONE;
}

int portmod_phychain_ext_phy_info_set(int unit, int phyn, int core_index, const portmod_ext_phy_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
        _ext_phy_info[unit][phyn - 1][core_index] = sal_alloc(sizeof(portmod_ext_phy_core_info_t), "core_info");
        if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
            _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOC_MSG("phyn core info mem allocation failed")));
        }
    }
    sal_memcpy(_ext_phy_info[unit][phyn - 1][core_index], core_info, sizeof(portmod_ext_phy_core_info_t));

exit:
    SOC_FUNC_RETURN; 
}

int portmod_phychain_ext_phy_info_get(int unit, int phyn, int core_index, portmod_ext_phy_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("for phyn core,the information is not available")));
    }
    sal_memcpy(core_info, _ext_phy_info[unit][phyn - 1][core_index], sizeof(portmod_ext_phy_core_info_t));

exit:
    SOC_FUNC_RETURN; 
}

int portmod_xphy_core_info_t_validate(int unit, const portmod_xphy_core_info_t* portmod_xphy_core_info)
{
    SOC_INIT_FUNC_DEFS;
    if(portmod_xphy_core_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("NULL parameter"));
    }
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_info_t_init(int unit, portmod_xphy_core_info_t* portmod_xphy_core_info)
{
    SOC_INIT_FUNC_DEFS;

    if(portmod_xphy_core_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("portmod_xphy_core_info NULL parameter"));
    }
    sal_memset(portmod_xphy_core_info, 0, sizeof(portmod_xphy_core_info_t));
    phymod_core_access_t_init(&portmod_xphy_core_info->core_access);
    portmod_xphy_core_info->core_initialized = 0;
    portmod_xphy_core_info->ref_clk = phymodRefClkCount;
    portmod_xphy_core_info->fw_load_method = phymodFirmwareLoadMethodCount;
    portmod_xphy_core_info->is_initialized = 0;
exit:
    SOC_FUNC_RETURN;
}

int portmod_ext_phy_core_info_t_init(int unit, portmod_ext_phy_core_info_t* portmod_ext_phy_core_info)
{
    SOC_INIT_FUNC_DEFS;

    if(portmod_ext_phy_core_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("portmod_ext_phy_core_info NULL parameter"));
    }
    sal_memset(portmod_ext_phy_core_info, 0, sizeof(portmod_ext_phy_core_info_t));
    phymod_core_access_t_init(&portmod_ext_phy_core_info->core_access);
    portmod_ext_phy_core_info->is_initialized = 0;

exit:
    SOC_FUNC_RETURN;
}

typedef enum xphy_wb_vars {
    xphy_is_initialized,
    xphy_phy_type,
    xphy_num_var
}xphy_wb_vars_t;

int portmod_xphy_wb_buffer_init(int unit, uint32 wb_buffer_index, portmod_xphy_core_info_t* xphy_core_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv = 0;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "xphy", NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_is_initialized ]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_phy_type]= wb_var_id;


    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _xphy_find_index(int unit, int xphy_addr, int* xphy_id)
{
    int idx;

    *xphy_id = PORTMOD_MAX_NUM_XPHY_SUPPORTED;
    for (idx=0; idx<PORTMOD_MAX_NUM_XPHY_SUPPORTED; idx++) {
        if (_xphy_info[unit][idx] != NULL) {
            if (_xphy_info[unit][idx]->core_access.access.addr == xphy_addr) {
                *xphy_id = idx;
                return SOC_E_NONE;
            }
        } else {
            if (*xphy_id == PORTMOD_MAX_NUM_XPHY_SUPPORTED)
                *xphy_id = idx;
        }
    }

    return SOC_E_PARAM;
}

int portmod_xphy_add(int unit, int xphy_addr, const portmod_xphy_core_info_t* xphy_core_info)
{
    int rv;
    
    int max_pms=0;
    int xphy_is_initialized = 0, xphy_id, idx;
    uint32 wb_buffer_index;
    SOC_INIT_FUNC_DEFS;

    xphy_id = PORTMOD_MAX_NUM_XPHY_SUPPORTED;
    if (_xphy_find_index(unit,xphy_addr,&xphy_id) == SOC_E_NONE)
        SOC_EXIT;

    if ( PORTMOD_MAX_NUM_XPHY_SUPPORTED <= xphy_id){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("xphy_id is out of the range of allowed external phys - 0- %d"), (PORTMOD_MAX_NUM_XPHY_SUPPORTED-1)));
    }
    if( NULL == _xphy_info[unit][xphy_id]){
         _xphy_info[unit][xphy_id] = sal_alloc(sizeof(portmod_xphy_core_info_t), "xphy core_info");
        if(NULL == _xphy_info[unit][xphy_id]){
            _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOC_MSG("xphy core info mem allocation failed")));
        }
    } else {
        /* This is not err,  reminder that use added again,,
            shoud err it?? . */
        /*_SOC_EXIT_WITH_ERR(SOC_E_NONE, (_SOC_MSG("xphy core info added previously.")));*/
        SOC_EXIT;
    }

    sal_memcpy(_xphy_info[unit][xphy_id], xphy_core_info, sizeof(portmod_xphy_core_info_t));
    portmod_max_pms_get(unit, &max_pms);

    /* if warmboot, restore the probed phy type into the phymod core_access. */
    if(SOC_WARM_BOOT(unit)){
        for (idx=0; idx<PORTMOD_MAX_NUM_XPHY_SUPPORTED; idx++) {
            if (_xphy_addr[unit][idx] == xphy_addr)
                break;
        }

        wb_buffer_index = idx + max_pms + 1;
        portmod_xphy_wb_buffer_init(unit, wb_buffer_index, _xphy_info[unit][xphy_id]);

        rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                    _xphy_info[unit][xphy_id]->wb_var_ids[xphy_phy_type], 
                    &_xphy_info[unit][xphy_id]->core_access.type);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                    _xphy_info[unit][xphy_id]->wb_var_ids[xphy_is_initialized],
                                    &_xphy_info[unit][xphy_id]->is_initialized);
        _SOC_IF_ERR_EXIT(rv);

    }

    if(!SOC_WARM_BOOT(unit)){
        portmod_xphy_db_addr_set(unit, xphy_id, xphy_addr);

        wb_buffer_index = xphy_id + max_pms + 1;
        portmod_xphy_wb_buffer_init(unit, wb_buffer_index, _xphy_info[unit][xphy_id]);

        _xphy_info[unit][xphy_id]->is_initialized = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                    _xphy_info[unit][xphy_id]->wb_var_ids[xphy_is_initialized],
                                    &_xphy_info[unit][xphy_id]->is_initialized);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}


int portmod_xphy_delete(int unit, int xphy_addr)
{
    int xphy_id;
    SOC_INIT_FUNC_DEFS;
    
    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_id));

    if ( PORTMOD_MAX_NUM_XPHY_SUPPORTED <= xphy_id){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("xphy_id is out of the range of allowed external phys - 0- %d"), (PORTMOD_MAX_NUM_XPHY_SUPPORTED-1)));
    }
    if( NULL ==  _xphy_info[unit][xphy_id]){
        /* This is not err, just reminder that user may have overwritten previous xphy. */
        _SOC_EXIT_WITH_ERR(SOC_E_NONE, (_SOC_MSG("portmod_xphy_delete xphy core info does not exist.")));
    } else {
        sal_free(_xphy_info[unit][xphy_id]);
        _xphy_info[unit][xphy_id] = NULL ;
    }
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_delete_all(int unit)
{
    int xphy_id;
    SOC_INIT_FUNC_DEFS;

    for(xphy_id = 0; xphy_id< PORTMOD_MAX_NUM_XPHY_SUPPORTED; xphy_id++){
        if(_xphy_info[unit][xphy_id] != NULL){
            sal_free(_xphy_info[unit][xphy_id]);
            _xphy_info[unit][xphy_id] = NULL ;
        }
    }

    SOC_FUNC_RETURN;
}

int portmod_xphy_core_info_set(int unit, uint32 xphy_addr, const portmod_xphy_core_info_t* xphy_core_info)
{
    int xphy_id, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_id));

    if ( PORTMOD_MAX_NUM_XPHY_SUPPORTED <= xphy_id){
        /*_SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("xphy_id is out of the range of allowed external phys - 0- %d"), (PORTMOD_MAX_NUM_XPHY_SUPPORTED-1)));*/
    }
    if( NULL ==  _xphy_info[unit][xphy_id]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("portmod_xphy_core_info_set reqd xphy_core_info  does not exist for xphy_id %d."),xphy_id));
    } else {
        sal_memcpy( _xphy_info[unit][xphy_id], xphy_core_info,  sizeof(portmod_xphy_core_info_t));

        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                    _xphy_info[unit][xphy_id]->wb_var_ids[xphy_phy_type], 
                                    &_xphy_info[unit][xphy_id]->core_access.type);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                    _xphy_info[unit][xphy_id]->wb_var_ids[xphy_is_initialized],
                                    &_xphy_info[unit][xphy_id]->is_initialized);
        _SOC_IF_ERR_EXIT(rv);

    }
exit:
    SOC_FUNC_RETURN;
}


int portmod_xphy_core_info_get(int unit, uint32 xphy_addr, portmod_xphy_core_info_t* xphy_core_info)
{
    int xphy_id;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_id));

    if ( PORTMOD_MAX_NUM_XPHY_SUPPORTED <= xphy_id){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("xphy_id is out of the range of allowed external phys - 0- %d"), (PORTMOD_MAX_NUM_XPHY_SUPPORTED-1)));
    }
    if( NULL ==  _xphy_info[unit][xphy_id]){
         return (SOC_E_PARAM);
    } else { 
        sal_memcpy( xphy_core_info, _xphy_info[unit][xphy_id],  sizeof(portmod_xphy_core_info_t));
    }
exit:
    SOC_FUNC_RETURN;
}


int portmod_xphy_core_info_get_by_idx(int unit, uint32 idx, portmod_xphy_core_info_t* xphy_core_info)
{
    if(NULL ==  _xphy_info[unit][idx])
        return SOC_E_PARAM;

    sal_memcpy( xphy_core_info, _xphy_info[unit][idx], sizeof(portmod_xphy_core_info_t));
    return SOC_E_NONE;
}


int portmod_port_phychain_core_access_set(int unit, int phyn, int core_index, const phymod_core_access_t* core_access)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL != _ext_phy_info[unit][phyn - 1][core_index]){
        sal_memcpy(&(_ext_phy_info[unit][phyn - 1][core_index]->core_access), core_access, sizeof(phymod_core_access_t));
    }
    
exit:
    SOC_FUNC_RETURN; 

}

int portmod_port_phychain_core_access_get(int unit, int port, const int chain_length, int max_cores, phymod_core_access_t* core_access)
{
    int phyn = 0;
    int nof_cores = 0;
    int is_most_ext = 0;
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= chain_length) || (0 >= chain_length)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("chain_length is out of the range of allowed external phys")));
    }
    for (phyn = 0; phyn < chain_length; phyn++) {
        _SOC_IF_ERR_EXIT(portmod_port_core_access_get(unit, port, phyn, max_cores, &core_access[phyn], &nof_cores, &is_most_ext));
    }
    
exit:
    SOC_FUNC_RETURN; 
}

int portmod_port_phychain_core_probe(phymod_core_access_t* core,
                                      const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int phy_addr = 0x0;
    int unit, port = 0;
    int is_phymod_probed=0;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        /* currently portmod supports only one external phy (1(int) + 1(ext))
         * external phy is the outermost one. check for legacy ext phy if present */
        if (phy_index) {
            rv = phymod_core_probe(&(core[phy_index].access), &(core[phy_index].type), &is_phymod_probed);

            if ( (rv == PHYMOD_E_NONE) && (!is_phymod_probed) )
            {
                unit = PORTMOD_USER_ACC_UNIT_GET(&(core[phy_index].access));
                port = PORTMOD_USER_ACC_LPORT_GET(&(core[phy_index].access));

                if(portmod_port_legacy_phy_probe(unit, port))
                {
                    PORTMOD_IS_LEGACY_PHY_SET(&(core[phy_index].access));
                    /* Update the phy address in core access struct -- used by phy info */
                    phy_addr = portmod_port_legacy_phy_addr_get(unit, port);
                    core[phy_index].access.addr = phy_addr;
                    rv = PHYMOD_E_NONE;
                }
            }
        } else {
            rv = phymod_core_probe( &(core[phy_index].access), &(core[phy_index].type), &is_phymod_probed);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_core_identify ( phymod_core_access_t* core,
                                          const int chain_length,
                                          uint32_t core_id,
                                          uint32_t* identified) 
{
int rv = PHYMOD_E_UNAVAIL;
    return(rv);
}

int portmod_port_phychain_core_lane_map_set(const phymod_core_access_t* core,
                                            const int chain_length,
                                            const phymod_lane_map_t* lane_map) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while( (PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else { 
            rv = phymod_core_lane_map_set(&core[phy_index], lane_map);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_core_lane_map_get(const phymod_core_access_t* core,
                                            const int chain_length,
                                            phymod_lane_map_t* lane_map) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while( (PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_lane_map_get(&core[phy_index], lane_map);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_reset_set(const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t direction) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_reset_set(&core[phy_index], reset_mode, direction);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_reset_get(const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t* direction ) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((rv == PHYMOD_E_UNAVAIL) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_reset_get(&core[phy_index], reset_mode, direction);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_core_firmware_info_get(const phymod_core_access_t* core,
                                                 const int chain_length,
                                                 phymod_core_firmware_info_t* fw_info) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_firmware_info_get(&core[phy_index], fw_info);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_loopback_get(const phymod_phy_access_t* phy_access,
                                       const int chain_length,  
                                       phymod_loopback_mode_t loopback, 
                                       uint32_t *enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_loopback_get(unit, port, loopback, enable);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_loopback_get(&phy_access[phy_index], loopback, enable);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_loopback_set(const phymod_phy_access_t* phy_access,
                                       const int chain_length,  
                                       phymod_loopback_mode_t loopback, 
                                       uint32_t enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    
    /* Loopback should be enabled always on the outermost phy */
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));             
            rv = portmod_port_legacy_loopback_set(unit, port, loopback, enable);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_loopback_set(&phy_access[phy_index], loopback, enable);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        if (rv == PHYMOD_E_UNAVAIL) {
            LOG_CLI((BSL_META_U(unit,
                                 "phy_index=%d Loopback=%d Unavail - try next in chain\n"),
                                  phy_index, loopback));
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_firmware_core_config_set(const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_core_config_t fw_core_config) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(phy_access);

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_firmware_core_config_set(&phy_access[phy_index], fw_core_config);
        }
        phy_index--;
    }
    return(rv);
}


int phymod_chain_phy_firmware_core_config_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_firmware_core_config_t *fw_core_config) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_firmware_core_config_get(&phy_access[phy_index], fw_core_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_firmware_lane_config_set(const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t fw_lane_config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_firmware_lane_config_set(&phy_access[phy_index], fw_lane_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_firmware_lane_config_get(const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t *fw_lane_config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                   
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_firmware_lane_config_get(&phy_access[phy_index], fw_lane_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_pll_sequencer_restart(const phymod_core_access_t* core, 
                                                     const int chain_length,
                                                     uint32_t flags, 
                                                     phymod_sequencer_operation_t operation)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_pll_sequencer_restart(&core[phy_index], flags, operation);
        }
        phy_index--;
    }
    return(rv);
}

int phymod_chain_core_wait_event(const phymod_core_access_t* core, 
                                 const int chain_length,
                                 phymod_core_event_t event, 
                                 uint32_t timeout) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_wait_event(&core[phy_index], event, timeout);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_restart(const phymod_phy_access_t* phy_access,
                                     const int chain_length) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_rx_restart(&phy_access[phy_index]);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_polarity_set(const phymod_phy_access_t* phy_access,
                                       const int chain_length, 
                                       const phymod_polarity_t* polarity) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_polarity_set(&phy_access[phy_index], polarity);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_polarity_get(const phymod_phy_access_t* phy_access,
                                       const int chain_length, 
                                       phymod_polarity_t* polarity) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_polarity_get(&phy_access[phy_index], polarity);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_set(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_tx_t* tx) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            rv = portmod_port_legacy_tx_set(phy_access, chain_length, tx); 
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_tx_set(&phy_access[phy_index], tx);
            } else {
                rv = PHYMOD_E_NONE ;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_get(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_tx_t* tx) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_tx_get(&phy_access[phy_index], tx);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_media_type_tx_get(const phymod_phy_access_t* phy_access,
                                           const int chain_length,
                                           phymod_media_typed_t media, 
                                           phymod_tx_t* tx) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_media_type_tx_get(&phy_access[phy_index], media, tx);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_override_set(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          const phymod_tx_override_t* tx_override) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_tx_override_set(&phy_access[phy_index], tx_override);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_override_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_tx_override_t* tx_override) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_tx_override_get(&phy_access[phy_index], tx_override);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_set(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_rx_t* rx) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_rx_set(&phy_access[phy_index], rx);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_get(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_rx_t* rx) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            if (phy_index && (phy_access[phy_index].access.lane_mask == 0)) {
                rv = PHYMOD_E_NONE; 
            } else { 
                rv = phymod_phy_rx_get(&phy_access[phy_index], rx);
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reset_set(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    const phymod_phy_reset_t* reset) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;


    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_phy_reset_set(unit, port);
        } else {
            if((phy_access[phy_index].access.lane_mask == 0) && phy_index) {
                rv = PHYMOD_E_NONE;
            } else { 
                rv = phymod_phy_reset_set(&phy_access[phy_index], reset);
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reset_get(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    phymod_phy_reset_t* reset) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_reset_get(&phy_access[phy_index], reset);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_power_set(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    const phymod_phy_power_t* power) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return  phymod_phy_power_set(&phy_access[0], power);
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_power_set(unit, port, power); 
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_power_set(&phy_access[phy_index], power);
            } else {
                rv = PHYMOD_E_NONE ;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_power_get(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    phymod_phy_power_t* power) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return  phymod_phy_power_get(&phy_access[0], power);
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_power_get(unit, port, power); 
            /* Get the value for outer most phy */
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_power_get(&phy_access[phy_index], power);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_lane_control_set(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_tx_lane_control_t tx_control) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return  phymod_phy_tx_lane_control_set(&phy_access[0], tx_control);
    }
    /* Most EXT */
    /* Program only the outer most phy */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_tx_ln_ctrl_set(unit, port, &tx_control);           
        } else {
            if(phy_access[phy_index].access.lane_mask){
            rv = phymod_phy_tx_lane_control_set(&phy_access[phy_index], tx_control);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_lane_control_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_tx_lane_control_t *tx_control) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return  phymod_phy_tx_lane_control_get(&phy_access[0], tx_control);
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_tx_ln_ctrl_get(unit, port, tx_control);                 
            return rv;
        } else {
        
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_tx_lane_control_get(&phy_access[phy_index], tx_control);
            } else {
                rv = PHYMOD_E_NONE;
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_lane_control_set(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_rx_lane_control_t rx_control)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_rx_lane_control_set(&phy_access[0], rx_control);
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
           unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_rx_ln_ctrl_set(unit, port, &rx_control);               
        } else {
        if(phy_access[phy_index].access.lane_mask){ 
            rv = phymod_phy_rx_lane_control_set(&phy_access[phy_index], rx_control);
        } else {
            rv = PHYMOD_E_NONE;
        }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_lane_control_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_rx_lane_control_t *rx_control)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_rx_lane_control_get(&phy_access[0], rx_control);
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_rx_ln_ctrl_get(unit, port, rx_control);                 
            return rv;
            
        } else {
        
            if((phy_access[phy_index].access.lane_mask == 0) && phy_index) {
                rv = PHYMOD_E_NONE;
            } else { 
                rv = phymod_phy_rx_lane_control_get(&phy_access[phy_index], rx_control);
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_fec_enable_set(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t enable) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_fec_set(unit, port, enable);
            return rv;
        } else {
        
        rv = phymod_phy_fec_enable_set(&phy_access[phy_index], enable);
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_fec_enable_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *enable) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_fec_get(unit, port, enable);
            return rv;
        } else {
        
        rv = phymod_phy_fec_enable_get(&phy_access[phy_index], enable);
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_eee_set(const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t enable) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_eee_set(unit, port, enable); 
            return rv; 
        } else {
        
        rv = phymod_phy_eee_set(&phy_access[phy_index], enable);
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_eee_get(const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t *enable)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_eee_get(unit, port, enable);
            return rv;
        } else {
        
        rv = phymod_phy_eee_get(&phy_access[phy_index], enable);
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_interface_config_set(const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t flags,
                                               const phymod_phy_inf_config_t* config,
                                               phymod_interface_t serdes_interface,
                                               int ref_clk, 
                                               const uint32 phy_init_flags)
{
    int rv = PHYMOD_E_NONE;
    int phy_index;
    phymod_phy_inf_config_t int_config = *(phymod_phy_inf_config_t*)config;
    int unit = 0, port = 0;
    phymod_autoneg_control_t an;
    uint32_t an_done;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        rv = portmod_port_line_to_sys_intf_map(&phy_access[1], &phy_access[0], port,
                       config->data_rate, ref_clk, int_config.interface_type, &int_config.interface_type);
        return phymod_phy_interface_config_set(&phy_access[0], flags, &int_config);
    }

    /* in case of external phy's default interface set may not be supported.
     * fix the interface to appropriate type */
    rv = portmod_port_line_intf_map_fix(&phy_access[phy_index], port, config->data_rate,
                   int_config.interface_type, &int_config.interface_type);
    if (rv) return (rv);

    if(phy_init_flags == PORTMOD_INIT_F_ALL_PHYS) { 
        /* ALL PHYS */
        while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
            if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
                unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
                port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
                if (flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP) {
                    rv = portmod_port_legacy_an_get(unit, port, &an, &an_done);
                    if (rv != PHYMOD_E_NONE) {
                        return rv;
                    }
                    if (an.enable) {
                        phy_index--;
                        continue;
                    }
                }
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                if(phy_access[phy_index].access.lane_mask){
                    if (flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP){
                        rv = phymod_phy_autoneg_get(&phy_access[phy_index], &an, &an_done);
                        if (rv != PHYMOD_E_NONE) {
                            return rv;
                        }
                        if (an.enable) {
                            phy_index--;
                            continue;
                        }
                    }
                    if (phy_index < (chain_length-1)) {
                        if ((serdes_interface != phymodInterfaceCount) && (phy_index == 0)) {
                            /* if user specify the serdes_interface, it will get priority. */ 
                            int_config.interface_type = serdes_interface; 
                        } else {
                            rv = portmod_port_line_to_sys_intf_map(&phy_access[phy_index+1], &phy_access[phy_index], port,
                                           config->data_rate, ref_clk, int_config.interface_type, &int_config.interface_type);
                        }
                        if (rv) return (rv);
                    }
                    rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags, &int_config);
                }
            }
            phy_index--;
        }
    } else if (phy_init_flags == PORTMOD_INIT_F_EXTERNAL_MOST_ONLY) {
        /* Most EXT, reprogram the internal and intermediate phy with existing setting. */
        while((PHYMOD_E_NONE == rv) && (phy_index >= 0)) {
            if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
                unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
                port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                if((phy_index == (chain_length - 1)) || (!(flags & PHYMOD_INTF_F_SET_SPD_NO_TRIGGER))) {
                    if (phy_index < (chain_length-1)) {
                        if ((serdes_interface != phymodInterfaceCount) && (phy_index == 0)) {
                            /* if user specify the serdes_interface, it will get priority. */ 
                            int_config.interface_type = serdes_interface; 
                        } else {
                            rv = portmod_port_line_to_sys_intf_map(&phy_access[phy_index+1], &phy_access[phy_index], port,
                                           config->data_rate, ref_clk, int_config.interface_type, &int_config.interface_type);
                        }
                        if (rv) return (rv);
                    }
                    if(phy_access[phy_index].access.lane_mask){
                        rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags, &int_config);
                    }
                }
            }
            phy_index--;
        }
        /* ALL PHYS - rest of the phys except most ext */
        while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
            if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
                unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
                port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                if(phy_access[phy_index].access.lane_mask){
                    rv = phymod_phy_interface_config_get(&phy_access[phy_index], flags,ref_clk, &int_config);
                    rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags,&int_config);
                }
            }
            phy_index--;
        }        
    } else if (phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        while(phy_index >= 0) {
            if (phy_index < (chain_length-1)) {
                if ((serdes_interface != phymodInterfaceCount) && (phy_index == 0)) {
                    /* if user specify the serdes_interface, it will get priority. */ 
                    int_config.interface_type = serdes_interface; 
                } else {
                    rv = portmod_port_line_to_sys_intf_map(&phy_access[phy_index+1], &phy_access[phy_index], port,
                                   config->data_rate, ref_clk, int_config.interface_type, &int_config.interface_type);
                }
                if (rv) return (rv);
            }
            phy_index--;
        }
        rv = phymod_phy_interface_config_set(&phy_access[0], flags, &int_config);
    }
    return(rv);
}

int portmod_port_phychain_interface_config_get(const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t flags,
                                  phymod_ref_clk_t ref_clock,
                                  phymod_phy_inf_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_interface_config_get(&phy_access[0], flags, ref_clock, config); 
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_interface_config_get(unit, port, config);
            return rv;
        } else {
        
            if ((phy_access[phy_index].access.lane_mask == 0) & phy_index) {
                rv = PHYMOD_E_NONE;
            } else {
                rv = phymod_phy_interface_config_get(&phy_access[phy_index], flags, ref_clock, config);
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_cl72_set(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t cl72_en)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_cl72_set(unit, port, cl72_en);
            return rv;              
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_cl72_set(&phy_access[phy_index], cl72_en);
            } else {
                rv = PHYMOD_E_NONE;
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_cl72_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *cl72_en)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
        if(phy_access[phy_index].access.lane_mask){
            rv = phymod_phy_cl72_get(&phy_access[phy_index], cl72_en);
        } else {
            rv = PHYMOD_E_NONE;
        }
        return rv;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_cl72_status_get(const phymod_phy_access_t* phy_access, 
                                          const int chain_length,
                                          phymod_cl72_status_t *status)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
        
        rv = phymod_phy_cl72_status_get(&phy_access[phy_index], status);
        return rv;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_ability_set(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              const phymod_autoneg_ability_t* an_ability)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_autoneg_ability_set(&phy_access[0], an_ability); 
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_advert_set(unit, port, an_ability);
            rv = PHYMOD_E_NONE;
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_autoneg_ability_set(&phy_access[phy_index], an_ability);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_ability_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_autoneg_ability_t* an_ability)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_advert_get(unit, port, an_ability);
            rv = PHYMOD_E_NONE;
        } else {
            rv = phymod_phy_autoneg_ability_get(&phy_access[phy_index], an_ability);
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_local_ability_get(const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            portmod_port_ability_t* port_ability)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_ability_local_get(unit, port, port_ability);
        } else {
        
            return rv;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_remote_ability_get(const phymod_phy_access_t* phy_access,
                                                     const int chain_length,
                                                     phymod_autoneg_ability_t* an_ability)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_autoneg_remote_ability_get(&phy_access[phy_index], an_ability);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_set(const phymod_phy_access_t* phy_access,
                                      const int chain_length,
                                      const phymod_autoneg_control_t *an_config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_autoneg_set(&phy_access[0], an_config); 
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_an_set(unit, port, an_config);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_autoneg_set(&phy_access[phy_index], an_config);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_get(const phymod_phy_access_t* phy_access,
                                      const int chain_length,
                                      phymod_autoneg_control_t *an_config,
                                      uint32_t * an_done)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_autoneg_get(&phy_access[0], an_config, an_done); 
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_an_get(unit, port, an_config, an_done);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_autoneg_get(&phy_access[phy_index], an_config, an_done);
            } else {
                rv = PHYMOD_E_NONE;
                *an_done = 1;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_status_get(const phymod_phy_access_t* phy_access, 
                                          const int chain_length,
                                          phymod_autoneg_status_t *status)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
phymod_autoneg_control_t an;
uint32 an_done;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_an_get(unit, port, &an, &an_done);
        } else {
/*          rv = phymod_phy_autoneg_status_get(phy_access, status); */
            rv = phymod_phy_autoneg_get(&phy_access[phy_index], &an, &an_done);
        }
        phy_index--;
        if (!rv) {
            status->enabled   = an.enable; 
            status->locked    = an_done; 
        }
        return rv;
    }
    return(rv);
}

int portmod_port_phychain_core_init(const phymod_core_access_t* core,
                                    const int chain_length,
                                    const phymod_core_init_config_t* init_config, 
                                    const phymod_core_status_t* core_status)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
phymod_core_init_config_t core_config;
int lane, unit;
portmod_xphy_core_info_t xphy_core_info;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
            /* core init for the legacy phys are done as part of phy init */
        } else {

        sal_memcpy(&core_config, init_config,sizeof(phymod_core_init_config_t));
        if(phy_index > 0) { /* external phy */
            sal_memset(&xphy_core_info, 0, sizeof(xphy_core_info));
            unit = PORTMOD_USER_ACC_UNIT_GET(&(core[phy_index].access));
                rv = portmod_xphy_core_info_get(unit, core[phy_index].access.addr, &xphy_core_info);
            if (rv == PHYMOD_E_NONE) {
            /* for external phys, FW load method is default to Internal
               unless it is force to NONE. */
            core_config.firmware_load_method = xphy_core_info.fw_load_method;
            core_config.firmware_loader = NULL;
            if (xphy_core_info.force_fw_load == phymodFirmwareLoadForce) {
                PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&core_config);
            }
            else if (xphy_core_info.force_fw_load == phymodFirmwareLoadAuto) {
                PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&core_config);
            }
        } else {
            return (rv);
        }
            /* config "no swap" for external phys.set later using APIs*/
            for(lane=0 ; lane < xphy_core_info.lane_map.num_of_lanes; lane++) {
                core_config.lane_map.lane_map_rx[lane] = xphy_core_info.lane_map.lane_map_rx[lane];
                core_config.lane_map.lane_map_tx[lane] = xphy_core_info.lane_map.lane_map_tx[lane];
            }
        }
        rv = phymod_core_init(&core[phy_index], &core_config, core_status);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pll_multiplier_get(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *core_vco_pll_multiplier)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pll_multiplier_get(&phy_access[phy_index], core_vco_pll_multiplier);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_init(const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   const phymod_phy_init_config_t* init_config)
{
int rv = PHYMOD_E_NONE;
int tx_rv = PHYMOD_E_NONE;
int phy_index;
phymod_phy_init_config_t local_init_config;
phymod_tx_t phymod_tx;
int i, unit = 0;
portmod_xphy_core_info_t xphy_core_info;

    sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {

#if 0 /* It is being doing part of broadcast in portmod_legacy_ext_phy_init */
            rv = portmod_port_legacy_phy_init(
                        PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access)),
                        PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)));
#endif
            phy_index--;
            sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));
            /* get default tx paramters for internal and intermediary phys. */ 
            tx_rv = phymod_phy_media_type_tx_get(&phy_access[phy_index], phymodMediaTypeChipToChip, &phymod_tx);
            /* if we can get the custom default tx paramter, use it.  Otherwise, use whatever it has in there. */
            if( tx_rv == PHYMOD_E_NONE )
            {
                for(i=0 ; i<PHYMOD_MAX_LANES_PER_CORE; i++) {
                    local_init_config.tx[i].pre  = phymod_tx.pre  ;
                    local_init_config.tx[i].main = phymod_tx.main ;
                    local_init_config.tx[i].post = phymod_tx.post ;
                    local_init_config.tx[i].post2= phymod_tx.post2;
                    local_init_config.tx[i].post3= phymod_tx.post3;
                    local_init_config.tx[i].amp  = phymod_tx.amp  ;
                }
            }
        } else {
            sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));

            if(phy_index != 0 ){
                phymod_polarity_t_init(&local_init_config.polarity);
                unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
                rv = portmod_xphy_core_info_get(unit, phy_access[phy_index].access.addr, &xphy_core_info);
                if ( rv == PHYMOD_E_NONE) {
                sal_memcpy(&(local_init_config.polarity), &(xphy_core_info.polarity), sizeof(phymod_polarity_t));

                /* overwrite tx params if set by config. phymod_phy_init only use tx, not ext_phy_tx */
                for(i=0 ; i<PHYMOD_MAX_LANES_PER_CORE; i++) {
                    local_init_config.tx[i].pre  = init_config->ext_phy_tx[i].pre  ;
                    local_init_config.tx[i].main = init_config->ext_phy_tx[i].main ;
                    local_init_config.tx[i].post = init_config->ext_phy_tx[i].post ;
                    local_init_config.tx[i].post2 = init_config->ext_phy_tx[i].post2 ;
                    local_init_config.tx[i].post3 = init_config->ext_phy_tx[i].post3 ;
                    local_init_config.tx[i].amp  = init_config->ext_phy_tx[i].amp  ;
                } 
                 } else { 
                    return (rv);}
            }

            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_init(&phy_access[phy_index], &local_init_config);
            }

            phy_index--;
        }
    } /* while */
    return(rv);
}
int portmod_port_phychain_rx_pmd_locked_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *rx_pmd_locked)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_rx_pmd_locked_get(&phy_access[phy_index], rx_pmd_locked);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_link_status_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *link_status)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Check if the phy access is targetted for internal phy only */
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return  phymod_phy_link_status_get(&phy_access[0], link_status);
    }
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_phy_link_get(unit, port, link_status);
        } else {
            if (phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_link_status_get(&phy_access[phy_index], link_status);
            } else {
                rv = PHYMOD_E_NONE;
                *link_status = 1;
            } 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pcs_userspeed_set(const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            const phymod_pcs_userspeed_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pcs_userspeed_set(&phy_access[phy_index], config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pcs_userspeed_get(const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            phymod_pcs_userspeed_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pcs_userspeed_get(&phy_access[phy_index], config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reg_write(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    uint32_t reg_addr,
                                    uint32_t reg_val) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_reg_write(&phy_access[phy_index], reg_addr, reg_val);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reg_read(const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   uint32_t reg_addr,
                                   uint32_t *reg_val)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_reg_read(&phy_access[phy_index], reg_addr, reg_val);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_slicer_position_set(const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 const phymod_slicer_position_t* position)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_rx_slicer_position_set(&phy_access[phy_index], flags, position);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_slicer_position_get(const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 phymod_slicer_position_t* position)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;           
                                            
    /* Most EXT */                          
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_rx_slicer_position_get(&phy_access[phy_index], flags, position);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_slicer_position_max_get(const phymod_phy_access_t* phy_access,
                                                     const int chain_length,
                                                     uint32_t flags,
                                                     phymod_slicer_position_t* position_min,
                                                     phymod_slicer_position_t* position_max)
{ 
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_rx_slicer_position_max_get(&phy_access[phy_index], flags, position_min, position_max);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_config_set(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          const phymod_prbs_t* prbs)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_prbs_config_set(&phy_access[phy_index], flags, prbs);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_config_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_t* prbs)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_prbs_config_get(&phy_access[phy_index], flags, prbs);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_enable_set(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t enable)                                                   
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_prbs_enable_set(&phy_access[phy_index], flags, enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_enable_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t *enable)                                                   
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_prbs_enable_get(&phy_access[phy_index], flags, enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_status_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_status_t *prbs_status)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_prbs_status_get(&phy_access[phy_index], flags, prbs_status);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_pattern_config_set(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             const phymod_pattern_t* pattern)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pattern_config_set(&phy_access[phy_index], pattern);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pattern_config_get(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_pattern_t* pattern)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pattern_config_get(&phy_access[phy_index], pattern);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pattern_enable_set(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t enable,
                                             phymod_pattern_t* pattern)                                                   
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pattern_enable_set(&phy_access[phy_index], enable, pattern);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pattern_enable_get(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *enable)                                                   
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pattern_enable_get(&phy_access[phy_index], enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_diagnostics_get(const phymod_core_access_t* core,
                                               const int chain_length,
                                               phymod_core_diagnostics_t* diag) 
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while( (PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
        } else {
            rv = phymod_core_diagnostics_get(&core[phy_index], diag);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_phy_diagnostics_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_diagnostics_t *diag)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_diagnostics_get(&phy_access[phy_index], diag);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pmd_info_dump(const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_pmd_info_dump(&phy_access[phy_index], type);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pcs_info_dump(const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            if(phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_pcs_info_dump(&phy_access[phy_index], type);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

#ifdef _OLD_EYE_
int portmod_port_phychain_meas_lowber_eye(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_phy_eyescan_options_t eyescan_options,
                                          uint32_t *buffer)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_meas_lowber_eye(&phy_access[phy_index], eyescan_options, buffer);
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_display_lowber_eye(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_phy_eyescan_options_t eyescan_options,
                                             uint32_t *buffer)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_display_lowber_eye(&phy_access[phy_index], eyescan_options, buffer);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_pmd_ber_end_cmd(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint8_t supp_info,
                                          uint32_t timeout_ms)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_pmd_ber_end_cmd(&phy_access[phy_index], supp_info, timeout_ms); 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_meas_eye_scan_start(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint8_t direction)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_meas_eye_scan_start(&phy_access[phy_index], direction); 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_meas_eye_scan_done(const phymod_phy_access_t* phy_access,
                                             const int chain_length)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_meas_eye_scan_done(&phy_access[phy_index]);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_read_eye_scan_stripe(const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t *buffer,
                                               uint16_t *status)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_read_eye_scan_stripe(&phy_access[phy_index], buffer, status);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_display_eye_scan_header(const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_display_eye_scan_header(&phy_access[phy_index], index);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_display_eye_scan_footer(const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_display_eye_scan_footer(&phy_access[phy_index], index);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_display_eye_scan_stripe(const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index,
                                                  uint32_t *buffer)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_display_eye_scan_stripe(&phy_access[phy_index], index,buffer);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_eye_scan_debug_info_dump(const phymod_phy_access_t* phy_access,
                                                   const int chain_length)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_eye_scan_debug_info_dump(&phy_access[phy_index]);
        }
        phy_index--;
    }
    return(rv);
}
#else
/* add phy_eyescan_run here */

#endif /* _OLD_EYE_ */

int portmod_port_phychain_timesync_config_set(const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              const portmod_phy_timesync_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_timesync_config_set(unit, port, config);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = portmod_common_timesync_config_set(&phy_access[phy_index], config);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_config_get(const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              portmod_phy_timesync_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        if(phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_timesync_config_get(unit, port, config);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = portmod_common_timesync_config_get(&phy_access[phy_index], config);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    
    return(rv);
}


int portmod_port_phychain_timesync_enable_set(const phymod_phy_access_t* phy_access, 
                                              const int chain_length, uint32 enable)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_enable_set(&phy_access[phy_index], enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_enable_get(const phymod_phy_access_t* phy_access, 
                                              const int chain_length, uint32* enable)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_enable_get(&phy_access[phy_index], enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_nco_addend_set(const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, uint32 freq_step)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_nco_addend_set(&phy_access[phy_index], freq_step);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_nco_addend_get(const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  uint32* freq_step)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;                   

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                      
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_timesync_nco_addend_get(&phy_access[phy_index], freq_step);
        }
        phy_index--;
    }                                             
    return(rv);
}

int portmod_port_phychain_timesync_framesync_mode_set(const phymod_phy_access_t* phy_access, 
                                                      const int chain_length, 
                                                      const portmod_timesync_framesync_t* framesync)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;                   
phymod_timesync_framesync_t phymod_framesync;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    sal_memcpy(&phymod_framesync, framesync, sizeof(phymod_timesync_framesync_t));
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_timesync_framesync_mode_set(&phy_access[phy_index], &phymod_framesync);
        }
        phy_index--;
    }                                             
    return(rv);
}

int portmod_port_phychain_timesync_framesync_mode_get(const phymod_phy_access_t* phy_access, 
                                                      const int chain_length, 
                                                      phymod_timesync_framesync_t* framesync)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
phymod_timesync_framesync_t phymod_framesync;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                      
    /* Most EXT */                                
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_framesync_mode_get(&phy_access[phy_index], &phymod_framesync);
        }
        phy_index--;
    }
    sal_memcpy(framesync,&phymod_framesync,  sizeof(phymod_timesync_framesync_t));
                                                      
    return(rv);
}

int portmod_port_phychain_timesync_local_time_set(const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  const uint64 local_time)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                      
    /* Most EXT */                                
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_local_time_set(&phy_access[phy_index], local_time);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_local_time_get(const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  uint64* local_time)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                       
    /* Most EXT */                                
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_local_time_get(&phy_access[phy_index], local_time);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_load_ctrl_set(const phymod_phy_access_t* phy_access, 
                                                 const int chain_length, 
                                                 uint32 load_once, 
                                                 uint32 load_always)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                       
    /* Most EXT */                                
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_load_ctrl_set(&phy_access[phy_index], load_once,
                                                         load_always);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_load_ctrl_get(const phymod_phy_access_t* phy_access, 
                                                 const int chain_length, 
                                                 uint32* load_once, 
                                                 uint32* load_always)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                       
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_load_ctrl_get(&phy_access[phy_index], load_once,
                                                         load_always);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_tx_timestamp_offset_set(const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32 ts_offset)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_tx_timestamp_offset_set(&phy_access[phy_index],ts_offset);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_tx_timestamp_offset_get(const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32* ts_offset)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */                                         
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_timesync_tx_timestamp_offset_get(&phy_access[phy_index],ts_offset);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_timesync_rx_timestamp_offset_set(const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32 ts_offset)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */                                         
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_timesync_rx_timestamp_offset_set(&phy_access[phy_index],ts_offset);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_timesync_rx_timestamp_offset_get(const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32* ts_offset)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */                                         
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_timesync_rx_timestamp_offset_get(&phy_access[phy_index],ts_offset);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_enable_set(const phymod_phy_access_t* phy_access, 
                                       const int chain_length, 
                                       uint32_t enable)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */                                         
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_phy_intr_enable_set(&phy_access[phy_index], enable);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_enable_get(const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              uint32_t* enable)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_phy_intr_enable_get(&phy_access[phy_index],enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_status_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint32_t* enable)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_intr_status_get(&phy_access[phy_index],enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_status_clear(const phymod_phy_access_t* phy_access,
                                              const int chain_length)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
uint32_t enable=0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_phy_intr_status_get(&phy_access[phy_index], &enable);
            if (rv) return (rv);

            rv = phymod_phy_intr_status_clear(&phy_access[phy_index], enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_timesync_do_sync(const phymod_phy_access_t* phy_access,
                                               const int chain_length )
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
             rv = phymod_timesync_do_sync(&phy_access[phy_index]); 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_capture_timestamp_get(const phymod_phy_access_t* phy_access, 
                                                         const int chain_length, uint64* cap_ts)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {    
            rv = phymod_timesync_capture_timestamp_get(&phy_access[phy_index],cap_ts);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_heartbeat_timestamp_get(const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, uint64* hb_ts)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_timesync_heartbeat_timestamp_get(&phy_access[phy_index],hb_ts);
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_edc_config_set(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         const phymod_edc_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
phymod_edc_config_t phymod_config;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    sal_memcpy(&phymod_config, config,  sizeof(phymod_edc_config_t));
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_edc_config_set(&phy_access[phy_index],&phymod_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_edc_config_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         phymod_edc_config_t* config)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
phymod_edc_config_t phymod_config;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
            rv = phymod_edc_config_get(&phy_access[phy_index],&phymod_config);
        }
        phy_index--;
    }
    sal_memcpy(config,&phymod_config,  sizeof(phymod_edc_config_t));
    return(rv);
}

int portmod_port_phychain_failover_mode_get(const phymod_phy_access_t *phy_access, 
                                  int chain_length, phymod_failover_mode_t *failover)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (!(phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access)))) {
            rv = phymod_failover_mode_get(&phy_access[phy_index],failover);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_failover_mode_set(const phymod_phy_access_t *phy_access,
                                  int chain_length, phymod_failover_mode_t failover)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (!(phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access)))) {
            rv = phymod_failover_mode_set(&phy_access[phy_index],failover);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_phy_link_up_event(const phymod_phy_access_t* phy_access,
                                            const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;


    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_phy_link_up_event(unit, port);
        } else {
            rv = PHYMOD_E_NONE;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_link_down_event(const phymod_phy_access_t* phy_access,
                                            const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;


    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_phy_link_down_event(unit, port);
        } else {
            rv = PHYMOD_E_NONE;
        }
        phy_index--;
    }
    return(rv);
}

/*
 * common routines between pm4x10 and pm4x25 
 */

int portmod_phy_port_diag_ctrl(int unit, soc_port_t port, phymod_phy_access_t *phy_access, 
                      int nof_phys, uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_tx_t  ln_txparam[PHYMOD_MAX_LANES_PER_CORE];
    uint32 lane_map = phy_access[0].access.lane_mask; 

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_DSC));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pmd_info_dump(phy_access, nof_phys,
                                                 (void*)arg));

            break;

        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_PCS));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pcs_info_dump(phy_access, nof_phys,
                                                    (void*)arg));
            break;

        case PHY_DIAG_CTRL_LINKMON_MODE:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_MODE  0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_LINKMON_MODE));

            if(phy_access[0].access.lane_mask){
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_link_mon_enable_set(phy_access, nof_phys, PTR_TO_INT(arg)));
            }
            break;

        case PHY_DIAG_CTRL_LINKMON_STATUS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_STATUS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_LINKMON_STATUS));

            if(phy_access[0].access.lane_mask){
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_link_mon_status_get(phy_access, nof_phys));
            }
            break;

       default:
            if(op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "portmod_phy_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_SET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !(phy_access->access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys, 
                                        op_cmd, ln_txparam, lane_map, PTR_TO_INT(arg)));
                    }
                }
            } else if(op_type == PHY_DIAG_CTRL_GET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "portmod_phy_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_GET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !(phy_access->access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,
                                         op_cmd, ln_txparam, lane_map, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                return (SOC_E_UNAVAIL);
            }

            break;
    }

    return (SOC_E_NONE);
}

int portmod_phy_port_reset_get (int unit, int port, pm_info_t pm_info,
                           int reset_mode, int opcode, int* direction)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;
    phymod_reset_direction_t reset_direction;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_core_access_get(unit, port, pm_info,
                                           core_access, (1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_get(core_access, nof_phys,
                                              reset_mode,
                                              &reset_direction));
    *direction = reset_direction;

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_reset_set (int unit, int port, pm_info_t pm_info,
                           int reset_mode, int opcode, int direction)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_core_access_get(unit, port, pm_info,
                                           core_access, (1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_set(core_access, nof_phys,
                                              reset_mode,
                                              direction));
exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_prbs_config_set (int unit, int port, pm_info_t pm_info, 
                                      portmod_prbs_mode_t mode, int flags, 
                                      const phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
   
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_set(phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_prbs_config_get (int unit, int port, pm_info_t pm_info, 
                                      portmod_prbs_mode_t mode, int flags, 
                                      phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_get(phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}


int portmod_phy_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (
              _SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_set(phy_access, nof_phys, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, int* enable)
{ 
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 is_enabled;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_get(phy_access, nof_phys, flags, &is_enabled));

    (*enable) = (is_enabled ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}


int portmod_phy_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, 
                                     phymod_prbs_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_status_get(phy_access, nof_phys, flags, status));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_link_status_get(phy_access, nof_phys, (uint32_t*) link));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_status_get(phy_access, nof_phys, an_status));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_autoneg_get(int unit, int port, pm_info_t pm_info, phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int         nof_phys;
    uint32      an_done;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_get(phy_access, nof_phys, an, &an_done));

exit:
    SOC_FUNC_RETURN;
}


int portmod_port_phychain_control_phy_timesync_set(const phymod_phy_access_t* phy_access,
                                                    const int chain_length,
                                                    const portmod_port_control_phy_timesync_t type,
                                                    uint64_t value)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_control_phy_timesync_set(unit, port, type, value);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = portmod_common_control_phy_timesync_set(&phy_access[phy_index], type, value);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_control_phy_timesync_get(const phymod_phy_access_t* phy_access,
                                                    const int chain_length,
                                                    const portmod_port_control_phy_timesync_t type,
                                                    uint64_t* value)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int unit = 0, port = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_control_phy_timesync_get(unit, port, type, value);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = portmod_common_control_phy_timesync_get(&phy_access[phy_index], type, value);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_medium_config_set(int unit, int port, soc_port_medium_t medium,
                                             soc_phy_config_t* config)
{
    return portmod_port_legacy_medium_config_set(unit, port, medium, config);
}

int portmod_port_phychain_medium_config_get(int unit, int port, soc_port_medium_t medium,
                                             soc_phy_config_t* config)
{
    return portmod_port_legacy_medium_config_get(unit, port, medium, config);
}

int portmod_port_phychain_medium_get(int unit, int port, soc_port_medium_t* medium)
{
    return portmod_port_legacy_medium_get(unit, port, medium);
}

