/*
 *         
 * $Id: portmod_chain.c,v 1.2.2.13 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT


#define PM_EXTPHY_ITER(rv, phy_idx)    while((PHYMOD_E_UNAVAIL== rv) && (phy_idx >= 0))
#define PM_ALLPHY_ITER(rv, phy_idx)    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_idx >= 0))

STATIC
portmod_phy_core_info_t *_ext_phy_info[SOC_MAX_NUM_DEVICES][MAX_PHYN][SOC_MAX_NUM_PORTS] = {{{NULL}}};


int portmod_phychain_ext_phy_info_set(int unit, int phyn, int core_index, const portmod_phy_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
        _ext_phy_info[unit][phyn - 1][core_index] = sal_alloc(sizeof(portmod_phy_core_info_t), "core_info");   
        if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
            _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOC_MSG("phyn core info mem allocation failed")));
        }     
    }
    sal_memcpy(_ext_phy_info[unit][phyn - 1][core_index], core_info, sizeof(portmod_phy_core_info_t));

exit:
    SOC_FUNC_RETURN; 
}

int portmod_phychain_ext_phy_info_get(int unit, int phyn, int core_index, portmod_phy_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("for phyn core,the information is not available")));
    }
    sal_memcpy(core_info, _ext_phy_info[unit][phyn - 1][core_index], sizeof(portmod_phy_core_info_t));

exit:
    SOC_FUNC_RETURN; 
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

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        /* currently portmod supports only one external phy (1(int) + 1(ext))
         * external phy is the outermost one. check for legacy ext phy if present */
        if (phy_index) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(core[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(core[phy_index].access)); 
            if(portmod_port_legacy_phy_probe(unit, port)) {
                PORTMOD_IS_LEGACY_PHY_SET(&(core[phy_index].access));
                /* Update the phy address in core access struct -- used by phy info */
                phy_addr = portmod_port_legacy_phy_addr_get(unit, port);
                core[phy_index].access.addr = phy_addr; 
                rv = PHYMOD_E_NONE;        
            } else {
                rv = phymod_core_probe(&(core[phy_index].access), &(core[phy_index].type));
            }
        } else {
        rv = phymod_core_probe( &(core[phy_index].access), &(core[phy_index].type));
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
            return rv;
        } else {
        
        rv = phymod_phy_loopback_get(&phy_access[phy_index], loopback, enable);
            return rv;
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
            return rv;
        } else {

        rv = phymod_phy_loopback_set(&phy_access[phy_index], loopback, enable);
            return rv;
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
    /* int unit, port */

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            /* unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
               port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));  */
            rv = portmod_port_legacy_tx_set(phy_access, chain_length, tx); 
            return rv;
        } else {
        
        rv = phymod_phy_tx_set(&phy_access[phy_index], tx);
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
        
        rv = phymod_phy_rx_get(&phy_access[phy_index], rx);
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

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        } else {
        
        rv = phymod_phy_reset_set(&phy_access[phy_index], reset);
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
        
        rv = phymod_phy_power_set(&phy_access[phy_index], power);
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
            return rv; 
        } else {
         
        rv = phymod_phy_power_get(&phy_access[phy_index], power);
             return rv;
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
        
        rv = phymod_phy_tx_lane_control_set(&phy_access[phy_index], tx_control);
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
        
        rv = phymod_phy_tx_lane_control_get(&phy_access[phy_index], tx_control);
            return rv;
        }
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
        
        rv = phymod_phy_rx_lane_control_set(&phy_access[phy_index], rx_control);
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
        
        rv = phymod_phy_rx_lane_control_get(&phy_access[phy_index], rx_control);
            return rv;
        }
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
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_interface_config_set(const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t flags,
                                               const phymod_phy_inf_config_t* config,
                                               int ref_clk, 
                                               const uint32 all_phy)
{
int rv = PHYMOD_E_UNAVAIL;
int phy_index;
phymod_phy_inf_config_t int_config;
int unit = 0, port = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    if (PORTMOD_USER_ACC_CMD_FOR_PHY_GET(phy_access, 0)) {
        return phymod_phy_interface_config_set(&phy_access[0], flags,config); 
    }

    if(all_phy) { 
        /* ALL PHYS */
        while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
            if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
                unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
                port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags,config);
            }
            phy_index--;
        }
    } else {
        /* Most EXT, reprogram the internal and intermediate phy with existing setting. */
        while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
            if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
                unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
                port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags,config);
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
                rv = phymod_phy_interface_config_get(&phy_access[phy_index], flags,ref_clk, &int_config);
                rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags,&int_config);
            }
            phy_index--;
        }        
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
        
        rv = phymod_phy_interface_config_get(&phy_access[phy_index], flags, ref_clock, config);
            return rv;
        }
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
        
        rv = phymod_phy_cl72_set(&phy_access[phy_index], cl72_en);
            return rv;
        }
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
        
        rv = phymod_phy_cl72_get(&phy_access[phy_index], cl72_en);
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

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_advert_set(unit, port, an_ability);
            rv = PHYMOD_E_NONE;
            return rv;        
        } else {
        
        rv = phymod_phy_autoneg_ability_set(&phy_access[phy_index], an_ability);
            return rv;
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
            return rv; 
        } else {
        
        rv = phymod_phy_autoneg_ability_get(&phy_access[phy_index], an_ability);
            return rv;
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
            return rv;        
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

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access));
            rv = portmod_port_legacy_an_set(unit, port, an_config);
            return rv;           
        } else {
        
        rv = phymod_phy_autoneg_set(&phy_access[phy_index], an_config);
            return rv;
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

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            unit = PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access));
            port = PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)); 
            rv = portmod_port_legacy_an_get(unit, port, an_config, an_done);
            return rv;            
        } else {
        
        rv = phymod_phy_autoneg_get(&phy_access[phy_index], an_config, an_done);
            return rv;
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
        
/*        rv = phymod_phy_autoneg_status_get(phy_access, status); */

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
int lane;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(core[phy_index].access))) {
            /* core init for the legacy phys are done as part of phy init */
        } else {

        sal_memcpy(&core_config, init_config,sizeof(phymod_core_init_config_t));
        if(phy_index > 0) { /* external phy */

            /* for external phys, FW load method is default to Internal
               unless it is force to NONE. */
            core_config.firmware_load_method = (init_config->firmware_load_method ==
                                                phymodFirmwareLoadMethodNone) ?
                                                    phymodFirmwareLoadMethodNone :
                                                    phymodFirmwareLoadMethodInternal;
            core_config.firmware_loader = NULL;

            /* config "no swap" for external phys.set later using APIs*/
            for(lane=0 ; lane<init_config->lane_map.num_of_lanes; lane++) {
                core_config.lane_map.lane_map_rx[lane] = lane;
                core_config.lane_map.lane_map_tx[lane] = lane;
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
int rv = PHYMOD_E_UNAVAIL;
int tx_rv = PHYMOD_E_NONE;
int phy_index;
phymod_phy_init_config_t local_init_config;
phymod_tx_t phymod_tx;
int i;

    sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
            rv = portmod_port_legacy_phy_init(
                        PORTMOD_USER_ACC_UNIT_GET(&(phy_access[phy_index].access)),
                        PORTMOD_USER_ACC_LPORT_GET(&(phy_access[phy_index].access)));
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
            if(phy_index != 0 ){
                /* During phy init, polarity information is only for internal phy. */
                phymod_polarity_t_init(&local_init_config.polarity);
            }
            rv = phymod_phy_init(&phy_access[phy_index], &local_init_config);
            phy_index--;

            if((phy_index >= 0)&& (rv == PHYMOD_E_NONE))
            {
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
            }
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
            return rv;
        } else {
        
        rv = phymod_phy_link_status_get(&phy_access[phy_index], link_status);
            return rv;
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
        
        rv = phymod_phy_pmd_info_dump(&phy_access[phy_index], type);
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
        
        rv = phymod_phy_pcs_info_dump(&phy_access[phy_index], type);
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

