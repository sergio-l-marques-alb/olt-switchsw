/*
 * $Id: $
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
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/Petra/petra_drv.h>
#include <soc/dpp/port_map.h>
#include <soc/dpp/dpp_wb_engine.h>

#define _SOC_DPP_PORT_DEF_PRIORITY (2)

/* Track which pp ports and tm ports are free or in use */
int _dflt_tm_pp_port_map[SOC_MAX_NUM_DEVICES];
soc_dpp_port_map_t _port_map[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];

/* Internal port map ERP/OLP not visible in pbmp bcm_port_config_t */
static soc_dpp_port_map_t _internal_port_map[SOC_MAX_NUM_DEVICES][SOC_DPP_PORT_RANGE_NUM_ENTRIES];

/* Track which pp ports and tm ports are free or in use */
soc_dpp_port_t *soc_dpp_pp_ports[SOC_MAX_NUM_DEVICES] = {NULL};
soc_dpp_port_t *soc_dpp_tm_ports[SOC_MAX_NUM_DEVICES] = {NULL};



#ifdef BCM_ARAD_SUPPORT
int
_soc_dpp_wb_pp_port_use_set(int unit, soc_port_t port, uint8 in_use)
{
    int rv;
    soc_dpp_port_t pp_port_use;
    pp_port_use.in_use = in_use;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_MAP_PP_PORT_USE,
                                   &pp_port_use,
                                   port);
    return rv;
}

int
_soc_dpp_wb_pp_port_restore(int unit)
{
    int rv;
    int port_i;
    int nof_pp_ports;
    int pp_port;
    soc_dpp_port_t pp_port_use;    
    soc_dpp_port_map_t *pmap;
    
    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
        pmap = &_port_map[unit][port_i];

        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_MAP_PP_PORT,
                                  &pp_port, port_i);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Error restore SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_MAP_PP_PORT for port(%d) in _soc_dpp_wb_pp_port_restore\n"), port_i));
        }
        SOCDNX_IF_ERR_RETURN(rv);
        pmap->pp_port = pp_port;
    }

    nof_pp_ports = SOC_DPP_DEFS_GET(unit, nof_pp_ports);
    for (port_i=0; port_i< nof_pp_ports; port_i++) {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,
            SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_MAP_PP_PORT_USE,
            &pp_port_use,
            port_i);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Error restore SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_MAP_PP_PORT_USE for port(%d) in _soc_dpp_wb_pp_port_restore\n"), port_i));
        }
        SOCDNX_IF_ERR_RETURN(rv);            
        soc_dpp_pp_ports[unit][port_i].in_use = pp_port_use.in_use;
    }
    
    return rv;
}
#endif

STATIC int
_soc_dpp_is_this_port_internal(soc_port_t port, int *port_adjust)
{
    int port_is_internal;

    if ((port >= SOC_DPP_PORT_INTERNAL_START) && (port <= SOC_DPP_PORT_INTERNAL_END)) {
         *port_adjust = port - SOC_DPP_PORT_INTERNAL_START;
         port_is_internal = TRUE;
    } else {
         *port_adjust = port;
         port_is_internal = FALSE;
    }
    return port_is_internal;
}

int
_soc_dpp_port_map_init(int unit) {
    int port_i;
    int nof_pp_ports;
    int nof_tm_ports;
    soc_dpp_port_map_t *pmap, *ipmap;

    nof_pp_ports = SOC_DPP_DEFS_GET(unit, nof_pp_ports);
    nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports);

/* ERP is a virtual TM port on Petra-B and real on ARAD   
       * allocate additional virtual port for Petra-B.   
       */   
      if(SOC_IS_ARAD(unit)) {   
          nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports);   
      } else {   
          nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports) + 1;   
      } 

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
        pmap = &_port_map[unit][port_i];
        pmap->nif_id = SOC_TMC_NIF_ID_NONE;
        pmap->tm_port = _SOC_DPP_PORT_INVALID;
        pmap->pp_port = _SOC_DPP_PORT_INVALID;
        pmap->channel = 0;
        pmap->pp_port = _SOC_DPP_PORT_INVALID;
        pmap->if_rate_mbps = 0;
    }

    for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES; ++port_i) {
        ipmap = &_internal_port_map[unit][port_i];
        ipmap->nif_id = SOC_TMC_NIF_ID_NONE;
        ipmap->tm_port = _SOC_DPP_PORT_INVALID;
        ipmap->pp_port = _SOC_DPP_PORT_INVALID;
        ipmap->channel = 0;
    }

    /* In use tm and pp ports, initially none */
    if (soc_dpp_pp_ports[unit] == NULL) {
        soc_dpp_pp_ports[unit] = sal_alloc(sizeof(soc_dpp_port_t) * nof_pp_ports, "pp_ports");
        if (soc_dpp_pp_ports[unit] == NULL) {
             return SOC_E_MEMORY;
        }
    }

    if (soc_dpp_tm_ports[unit] == NULL) {
        soc_dpp_tm_ports[unit] = sal_alloc(sizeof(soc_dpp_port_t) * nof_tm_ports, "tm_ports");
        if (soc_dpp_tm_ports[unit] == NULL) {
             return SOC_E_MEMORY;
        }
    }
    for (port_i=0; port_i< nof_pp_ports; port_i++) {
        SOC_DPP_PP_PORT_FREE(unit, port_i);  
    }

    for (port_i=0; port_i< nof_tm_ports; port_i++) {
        SOC_DPP_TM_PORT_FREE(unit, port_i);  
    }

    _dflt_tm_pp_port_map[unit] = TRUE;

    return SOC_E_NONE;
}

int
_soc_dpp_port_map_deinit(int unit) 
{
    int port_i; 
    int nof_pp_ports;
    int nof_tm_ports;

    nof_pp_ports = SOC_DPP_DEFS_GET(unit, nof_pp_ports);
    nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports);
    /* ERP is a virtual TM port on Petra-B and real on ARAD   
       * allocate additional virtual port for Petra-B.   
       */   
      if(SOC_IS_ARAD(unit)) {   
          nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports);   
      } else {   
          nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports) + 1;   
      } 

   /* soc_dpp_port_map_t *pmap, *ipmap; */
    for (port_i=0; port_i< nof_pp_ports; port_i++) {
        SOC_DPP_PP_PORT_FREE(unit, port_i);  
    }

    for (port_i=0; port_i< nof_tm_ports; port_i++) {
        SOC_DPP_TM_PORT_FREE(unit, port_i);  
    }

    _dflt_tm_pp_port_map[unit] = FALSE;


    /* In use tm and pp ports, initially none */
    if (soc_dpp_pp_ports[unit] != NULL) {
        sal_free(soc_dpp_pp_ports[unit]);

    }

    if (soc_dpp_tm_ports[unit] != NULL) {
        sal_free(soc_dpp_tm_ports[unit]); 
    }
    soc_dpp_pp_ports[unit]  = NULL;
    soc_dpp_tm_ports[unit] = NULL;
    return SOC_E_NONE;
}


int
petra_soc_dpp_local_port_valid(int unit, soc_port_t port) {
    int valid_port = FALSE, valid_internal_port = FALSE;
    int port_adjust;
    soc_dpp_port_map_t *ipmap;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {

        ipmap = &_internal_port_map[unit][port_adjust];
        valid_internal_port = ((ipmap->tm_port != _SOC_DPP_PORT_INVALID) && 
                               (ipmap->nif_id != SOC_TMC_NIF_ID_NONE));
    } else {
        valid_port = (port >= 0 &&  port <SOC_MAX_NUM_PORTS &&
                     (_port_map[unit][port].tm_port != _SOC_DPP_PORT_INVALID) && 
                     (_port_map[unit][port].pp_port != _SOC_DPP_PORT_INVALID) &&
                     (_port_map[unit][port].nif_id != SOC_TMC_NIF_ID_NONE));
    }
    return (valid_port || valid_internal_port);
}

int
petra_soc_dpp_local_port_partial(int unit, soc_port_t port) {
    int port_adjust;
    soc_dpp_port_map_t *pmap;

     
    if ((50 <= port) && (port <= 53)) return FALSE;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
        /* these ports can be partial (no pp_port required for erp */
        return FALSE;
    }
    pmap = &_port_map[unit][port];

    return (!petra_soc_dpp_local_port_valid(unit, port) &&
        ((pmap->tm_port != _SOC_DPP_PORT_INVALID) ||
         (pmap->pp_port != _SOC_DPP_PORT_INVALID) ||
         (pmap->nif_id != SOC_TMC_NIF_ID_NONE)));
}

int
petra_soc_dpp_local_to_nif_id_get(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID *nif_id, uint32 *channel) {

    int port_adjust;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
        ipmap = &_internal_port_map[unit][port_adjust];
        *nif_id = ipmap->nif_id;
        *channel = ipmap->channel;
        return SOC_E_NONE;

    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }
    pmap = &_port_map[unit][port];
    *nif_id = pmap->nif_id;
    *channel = pmap->channel;

    return SOC_E_NONE;
}

int
petra_soc_dpp_local_to_nif_id_set(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID nif_id, uint32 channel) {

    int port_adjust;
    soc_dpp_port_map_t *pmap, *ipmap;

   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
       ipmap = &_internal_port_map[unit][port_adjust];

       ipmap->nif_id = nif_id;
       ipmap->channel = channel;

       /* internal ports do not have a default mapping */
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "petra_soc_dpp_local_to_nif_id_set() internal port(%d) mapped to nif_id(%d) channel(%d)\n"), 
                             port, nif_id, channel));
#endif
        return SOC_E_NONE;

    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }
    pmap = &_port_map[unit][port];

    pmap->nif_id = nif_id;
    pmap->channel = channel;
    
    /* Default mapping:
     * All CPU ports are mapped to pp port 0 in Petra-B
     * Local port 0 is mapped to tm/pp ports 62 
     * Olp port  
     */
    if (_dflt_tm_pp_port_map[unit] == TRUE) {
        if (SOC_IS_PETRAB(unit)) {
          if (nif_id == SOC_TMC_IF_ID_CPU) {
              pmap->tm_port = port;
              pmap->pp_port = 0;
              SOC_DPP_TM_PORT_RESERVE(unit, port);
              SOC_DPP_PP_PORT_RESERVE(unit, 0);
          }
          else if (port == 0) {
              pmap->tm_port = 62;
              pmap->pp_port = 62;
              SOC_DPP_TM_PORT_RESERVE(unit, 62);
              SOC_DPP_PP_PORT_RESERVE(unit, 62);
          } else {
              pmap->tm_port = port;
              pmap->pp_port = port;
              SOC_DPP_TM_PORT_RESERVE(unit, port);
              SOC_DPP_PP_PORT_RESERVE(unit, port);
          }    
        } else {
            /* 1x1 mapping without any restriction (i.e., even CPU) */
              pmap->tm_port = port;
              pmap->pp_port = port;
              SOC_DPP_TM_PORT_RESERVE(unit, port);
              SOC_DPP_PP_PORT_RESERVE(unit, port);
        }
    }
#ifdef DBG_LOCAL_PORT
    LOG_INFO(BSL_LS_SOC_PORT,
             (BSL_META_U(unit,
                         "petra_soc_dpp_local_to_nif_id_set() pbmp port(%d) mapped to nif_id(%d) channel(%d)\n"), 
                         port, nif_id, channel));
#endif
    return SOC_E_NONE;
}

int
petra_soc_dpp_local_to_tm_port_get(int unit, soc_port_t port, uint32* tm_port, int* core) {
    int port_adjust;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
       ipmap = &_internal_port_map[unit][port_adjust];    
        *tm_port = ipmap->tm_port;

    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }

    pmap = &_port_map[unit][port];
    *tm_port = pmap->tm_port; 
    *core = 0;

    return SOC_E_NONE;
}

int
petra_soc_dpp_tm_to_local_port_get(int unit, int core, uint32 tm_port, soc_port_t *port) {
    int port_i;
    soc_dpp_port_map_t *pmap, *ipmap;

    *port = _SOC_DPP_PORT_INVALID;

    for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
        pmap = &_port_map[unit][port_i];
        if (pmap->tm_port == tm_port) {
            *port = port_i;
            return SOC_E_NONE;
        }
    }

    for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES ; ++port_i) {
        ipmap = &_internal_port_map[unit][port_i];    
        if (ipmap->tm_port == tm_port) {
            *port = port_i + SOC_DPP_PORT_INTERNAL_START;
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

int
petra_soc_dpp_pp_to_local_port_get(int unit, int core, uint32 pp_port, soc_port_t *port) {
    int port_i;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (!SOC_DPP_PP_PORT_IN_USE(unit, pp_port)) {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "pp_port(%d) not in use\n"), pp_port));
        *port = _SOC_DPP_PORT_INVALID;
    } else {

        for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
            pmap = &_port_map[unit][port_i];
            if (pmap->pp_port == pp_port) {
                *port = port_i;
                return SOC_E_NONE;
            }
        }
        for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES ; ++port_i) {
           ipmap = &_internal_port_map[unit][port_i];
            if (ipmap->pp_port == pp_port) {
                *port = port_i + SOC_DPP_PORT_INTERNAL_START;
                return SOC_E_NONE;
            }
        }    
    }
    return SOC_E_NOT_FOUND;
}
/* set an internal port up without affecting the default mapping */
int
petra_soc_dpp_local_to_tm_port_set_internal(int unit, soc_port_t port, int tm_port) {

    int port_adjust;
    soc_dpp_port_map_t *ipmap;

    if (!SOC_DPP_TM_PORT_IN_USE(unit, tm_port)) {

        if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "soc_dpp_local_to_tm_port_set_internal() internal port(%d) map to tm_port(%d)\n"), 
                                 port, tm_port));
#endif
            ipmap = &_internal_port_map[unit][port_adjust];
            ipmap->tm_port = tm_port;
            SOC_DPP_TM_PORT_RESERVE(unit, tm_port);

        } else {
           LOG_ERROR(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "only internal ports valid for this function\n"))); 
            return SOC_E_PARAM;
        }
    } else {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "tm_port(%d) already in use\n"), tm_port));
        return SOC_E_RESOURCE;
    }
    
    return SOC_E_NONE;
}
/* Note: first call to this function invalidates default mapping */
int
petra_soc_dpp_local_to_tm_port_set(int unit, soc_port_t port, int core, uint32 tm_port) {
    int port_adjust=0;
    int port_i;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {

    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }
    
    if (_dflt_tm_pp_port_map[unit] == TRUE) {
        /* This is the first mapping which overrides the default mapping.
           Let's invalidate all entries */
        for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
            pmap = &_port_map[unit][port_i];
            pmap->tm_port = _SOC_DPP_PORT_INVALID;
            pmap->pp_port = _SOC_DPP_PORT_INVALID;
        }      
        for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES; ++port_i) {
            ipmap = &_internal_port_map[unit][port_i];
            ipmap->tm_port = _SOC_DPP_PORT_INVALID;
            ipmap->pp_port = _SOC_DPP_PORT_INVALID;
        }    
    for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++port_i) {
            SOC_DPP_TM_PORT_FREE(unit, port_i);
        }
    for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_pp_ports); ++port_i) {
            SOC_DPP_PP_PORT_FREE(unit, port_i);
        }
        _dflt_tm_pp_port_map[unit] = FALSE;

    }

    if (tm_port != _SOC_DPP_PORT_INVALID && SOC_DPP_TM_PORT_IN_USE(unit, tm_port)) {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "tm_port(%d) already in use\n"), tm_port));
        return SOC_E_RESOURCE;
    }

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "soc_dpp_local_to_tm_port_set() internal port(%d) map to tm_port(%d)\n"), 
                             port, tm_port));
#endif
        ipmap = &_internal_port_map[unit][port_adjust];
        _internal_port_map[unit][port_adjust].tm_port = tm_port;
        if (tm_port != _SOC_DPP_PORT_INVALID) {
            SOC_DPP_TM_PORT_RESERVE(unit, tm_port);
        }
    } else {
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "soc_dpp_local_to_tm_port_set() pbmp port(%d) map to tm_port(%d)\n"), 
                             port, tm_port));
#endif
        pmap = &_port_map[unit][port];
        pmap->tm_port = tm_port;
        if (tm_port != _SOC_DPP_PORT_INVALID) {
            SOC_DPP_TM_PORT_RESERVE(unit, tm_port);
        }
    }
    return SOC_E_NONE;
}

int
petra_soc_dpp_local_to_pp_port_get(int unit, soc_port_t port, uint32* pp_port, int* core) {
    int port_adjust;
    soc_dpp_port_map_t *pmap, *ipmap;

    *core = 0;

   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
        ipmap = &_internal_port_map[unit][port_adjust];
        *pp_port = ipmap->pp_port;
    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit) || port < 0) {
        return SOC_E_PARAM;
    }

    pmap = &_port_map[unit][port];
    *pp_port = pmap->pp_port; 

    return SOC_E_NONE;
}

/* set an internal port up without affecting the default mapping */
int
petra_soc_dpp_local_to_pp_port_set_internal(int unit, soc_port_t port, int pp_port) {
    int port_adjust;
    soc_dpp_port_map_t *ipmap;

    if (SOC_DPP_PP_PORT_IN_USE(unit, pp_port)) {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "pp_port(%d) already in use\n"), pp_port));
        return SOC_E_RESOURCE;
    }

   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "soc_dpp_local_to_tm_port_set_internal() internal port(%d) map to pp_port(%d)\n"), 
                             port, pp_port));
#endif
        ipmap = &_internal_port_map[unit][port_adjust];
        ipmap->pp_port = pp_port;
        SOC_DPP_PP_PORT_RESERVE(unit, pp_port);
    } else {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "only internal ports valid for this function\n"))); 
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

int
petra_soc_dpp_local_to_pp_port_set(int unit, soc_port_t port, uint32 pp_port) {
    soc_error_t rv;
    int port_i;
    int port_adjust=0;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }    
    if (_dflt_tm_pp_port_map[unit]) {
        /* This is the first mapping which overrides the default mapping.
           Let's invalidate all entries */
        for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
            pmap = &_port_map[unit][port_i];
            pmap->tm_port = _SOC_DPP_PORT_INVALID;
            pmap->pp_port = _SOC_DPP_PORT_INVALID;
        }      
        for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES; ++port_i) {
            ipmap = &_internal_port_map[unit][port_i];
            ipmap->tm_port = _SOC_DPP_PORT_INVALID;
            ipmap->pp_port = _SOC_DPP_PORT_INVALID;
        }        
        for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++port_i) {
            SOC_DPP_TM_PORT_FREE(unit, port_i);
        }
        for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_pp_ports); ++port_i) {
            SOC_DPP_PP_PORT_FREE(unit, port_i);
        }
        _dflt_tm_pp_port_map[unit] = FALSE;
   }
   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT    
       LOG_INFO(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "soc_dpp_local_to_pp_port_set() internal port(%d) map to pp_port(%d)\n"), 
                            port, pp_port));
#endif   
       ipmap = &_internal_port_map[unit][port_adjust];
       ipmap->pp_port = pp_port;
       if(pp_port != _SOC_DPP_PORT_INVALID) {
           SOC_DPP_PP_PORT_RESERVE(unit, pp_port);
       }

   } else {

#ifdef DBG_LOCAL_PORT    
       LOG_INFO(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "soc_dpp_local_to_pp_port_set() pbmp port(%d) map to pp_port(%d)\n"), 
                            port, pp_port));
#endif
        pmap = &_port_map[unit][port];
        pmap->pp_port = pp_port;
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_MAP_PP_PORT,
                                       &pp_port, port);
        if(SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "SOC_DPP_WB_ENGINE_SET_ARR failed\n")));
            return rv;
        }

        if(pp_port != _SOC_DPP_PORT_INVALID) {
            SOC_DPP_PP_PORT_RESERVE(unit, pp_port);
        }

   }
  return SOC_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME



