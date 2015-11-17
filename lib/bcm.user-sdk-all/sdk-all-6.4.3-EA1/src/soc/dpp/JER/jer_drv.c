/*
 * $Id: jer_drv.c Exp $
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
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*
 * Includes
 */
#include <shared/bsl.h>
/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/ipoll.h>

/*SOC DCMN includes*/
#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dcmn/dcmn_iproc.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dcmn/dcmn_mem.h>

/* SOC DPP JER includes */
#include <soc/dpp/JER/jer_drv.h>
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_regs.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/JER/jer_intr.h>
/* SOC DPP Arad includes */
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>

/*
 * Internal functions declaration
 */
int soc_jer_init_reset(int unit, int reset_action);

/******************************************/
/******       port information       ******/
/******************************************/

typedef struct soc_jer_ucode_port_config_s {
    soc_port_if_t   interface;
    soc_pbmp_t      phy_pbmp;
    uint32          channel;
    int             core;
    uint32          tm_port;
    uint32          is_hg;
    uint32          is_nif;
    uint32          protocol_offset;
} soc_jer_ucode_port_config_t;

#define SUB_PHYS_IN_QSGMII 4

STATIC int
soc_jer_index_update(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp) 
{
    uint32 phy;
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*new_pbmp);

    SOC_PBMP_ITER(*pbmp, phy) 
    {
        if(phy < 37) {
            skip = 0;
        } else {
            qsgmii_count = phy - 37;
            if (qsgmii_count > 12) {
                qsgmii_count = 12;
            }
            if (phy > 15*4) {
               qsgmii_count += phy - 61;
            }

            skip = qsgmii_count*(SUB_PHYS_IN_QSGMII - 1);
        }

        SOC_PBMP_PORT_ADD(*new_pbmp, phy + skip);
    }

    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_general_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int first_phy, i;
    soc_pbmp_t pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp);
    first_phy = (id * default_nof_lanes) + 1;

    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy+i);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_index_update(unit, &pbmp, &config->phy_pbmp));
   
exit:  
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_xaui_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int i, first_phy;
    soc_pbmp_t pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp);
    first_phy = (id * default_nof_lanes) + 25;

    for(i=0 ; i<default_nof_lanes ; i++) {
        SOC_PBMP_PORT_ADD(pbmp, first_phy + i);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_index_update(unit, &pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_qsgmii_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int first_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(config->phy_pbmp);
    if(id<32) {
        first_phy = id  + 37;
    } else {
        first_phy = id - 32 + 113;
    }

    SOC_PBMP_PORT_ADD(config->phy_pbmp, first_phy);

    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_cpu_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_PORT_ADD(config->phy_pbmp, 0);

    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_ilkn_phy_pbmp_get(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config)
{
    int i;
    char* propval;
    char* propkey;
    int first_phy, lanes;
    soc_pbmp_t bm, phy_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_pbmp);
    first_phy = id*12 + 1;

    propkey = spn_ILKN_LANES;
    propval = soc_property_port_get_str(unit, id, propkey);   

    if(propval != NULL) {
        if (_shr_pbmp_decode(propval, &bm) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("failed to decode (\"%s\") for %s"), propval, propkey)); 
        }

        SOC_PBMP_ITER(bm, i) {
            SOC_PBMP_PORT_ADD(phy_pbmp, first_phy+i);
        }

    } else {
        lanes = soc_property_port_get(unit, id, spn_ILKN_NUM_LANES, default_nof_lanes);

        for(i=0 ; i<lanes ; i++) {
            SOC_PBMP_PORT_ADD(phy_pbmp, first_phy+i);
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_index_update(unit, &phy_pbmp, &config->phy_pbmp));

exit:
    SOCDNX_FUNC_RETURN;
}


typedef int (*soc_jer_phy_pbmp_get_f)(int unit, soc_port_t port, uint32 id, uint32 default_nof_lanes, soc_jer_ucode_port_config_t* config);

typedef struct soc_jer_ucode_port_s {
    char* prefix;
    soc_port_if_t interface;
    uint32 default_nof_lanes;
    soc_jer_phy_pbmp_get_f phy_pbmp_get;
    uint32 flags;
} soc_jer_ucode_port_t;

#define SOC_JER_UCODE_FLAG_REQUIRED_INDEX   0x1
#define SOC_JER_UCODE_FLAG_NIF              0x2

static soc_jer_ucode_port_t ucode_ports[] = {
    {"XE",          SOC_PORT_IF_XFI,        1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"10GBase-R",   SOC_PORT_IF_XFI,        1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF}, /*same as XE, legacy name*/
    {"XLGE",        SOC_PORT_IF_XLAUI,      4,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"XLGE2_",      SOC_PORT_IF_XLAUI2,     2,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"CGE",         SOC_PORT_IF_CAUI,       4,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"ILKN",        SOC_PORT_IF_ILKN,       12, &soc_jer_ilkn_phy_pbmp_get,     SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"GE",          SOC_PORT_IF_SGMII,      1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"SGMII",       SOC_PORT_IF_SGMII,      1,  &soc_jer_general_phy_pbmp_get,  SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF}, /*same as GE, legacy name*/
    {"XAUI",        SOC_PORT_IF_DNX_XAUI,   4,  &soc_jer_xaui_phy_pbmp_get,     SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"RXAUI",       SOC_PORT_IF_RXAUI,      2,  &soc_jer_xaui_phy_pbmp_get,     SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"QSGMII",      SOC_PORT_IF_QSGMII,     1,  &soc_jer_qsgmii_phy_pbmp_get,   SOC_JER_UCODE_FLAG_REQUIRED_INDEX | SOC_JER_UCODE_FLAG_NIF},
    {"CPU",         SOC_PORT_IF_CPU,        1,  &soc_jer_cpu_phy_pbmp_get,      0},
    {"RCY",         SOC_PORT_IF_RCY,        0,  &soc_jer_general_phy_pbmp_get,  0},
    {NULL,          SOC_PORT_IF_NULL,       0,  NULL,                           0}, /*last*/

};


STATIC int
soc_jer_str_prop_parse_ucode_port(int unit, soc_port_t port, soc_jer_ucode_port_config_t* port_config)
{
    uint32 local_chan;
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len, id = 0, idx;

    SOCDNX_INIT_FUNC_DEFS;

    port_config->is_hg = 0;
    port_config->is_nif = 0;
    port_config->channel = 0;
    port_config->interface = SOC_PORT_IF_NULL;
    port_config->core = -1;
    port_config->tm_port = 0xFFFFFFFF;
    port_config->protocol_offset = 0;
    SOC_PBMP_CLEAR(port_config->phy_pbmp);

    propkey = spn_UCODE_PORT;
    propval = soc_property_port_get_str(unit, port, propkey);    
    s = propval;

    /* Parse interfaces */
    if (propval) {

        idx = 0;

        while(ucode_ports[idx].prefix) {
            prefix = ucode_ports[idx].prefix;
            prefix_len = sal_strlen(prefix);

            /*check prefix*/
            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += prefix_len;
                port_config->interface = ucode_ports[idx].interface;
                
                if(ucode_ports[idx].flags & SOC_JER_UCODE_FLAG_REQUIRED_INDEX) {
                    id = sal_ctoi(s, &ss);
                    if (s == ss) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                    }  
                    s = ss;
                }

                break;
            }

            idx++;
        }

        if (!ucode_ports[idx].prefix /*not found*/) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey)); 
        }

        if (s && (*s == '.')) {
            /* Parse channel number */
            ++s;
            local_chan = sal_ctoi(s, &ss);
            if (s != ss) {
                port_config->channel = local_chan;
            }
            s = ss;
        }

        /* search for additional properties */
        while (s && (*s == ':')) {
            ++s;

            /* Check if higig port */
            prefix = "hg";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += prefix_len;
                port_config->is_hg = 1;
                continue;
            } 

            prefix = "core_"; /*TBD doron*/
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {

                s += prefix_len;
                port_config->core = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Core not specify for (\"%s\") for %s"), propval, propkey));
                }
                s = ss;

                if (s && (*s == '.')) {
                    /* Parse tm_port number */
                    ++s;
                    port_config->tm_port = sal_ctoi(s, &ss);
                    if (s == ss) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("TM port not specify for (\"%s\") for %s"), propval, propkey));
                    }
                    s = ss; 
                } 

                continue;
            } 
        }

        if(port_config->core == -1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Core not specify for (\"%s\") for %s"), propval, propkey));
        }

        if(port_config->tm_port == 0xFFFFFFFF) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("TM port not specify for (\"%s\") for %s"), propval, propkey));
        }

        if(ucode_ports[idx].flags & SOC_JER_UCODE_FLAG_NIF) {
            port_config->is_nif = 1;
        }

        if(ucode_ports[idx].phy_pbmp_get) {
            SOCDNX_IF_ERR_EXIT(ucode_ports[idx].phy_pbmp_get(unit, port, id, ucode_ports[idx].default_nof_lanes, port_config));
        }

        port_config->protocol_offset = id;
    } 


exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_ports_config(int unit) 
{
    uint32 flags = 0, is_pon, init_vid_enabled;
    int port_i, stat_if_en;
    soc_jer_ucode_port_config_t port_config;
    int rv, core, val;
    soc_info_t          *si;
    uint32 erp_tm_port_id, erp_base_q_pair;
    soc_pbmp_t phy_ports, ports_bm;
    SOCDNX_INIT_FUNC_DEFS;
    
    si  = &SOC_INFO(unit);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_init(unit));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_wb_init(unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_interface_init(unit));

    for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++port_i) 
    {
        /* parse ucode_port */
        rv = soc_jer_str_prop_parse_ucode_port(unit, port_i, &port_config);
        SOCDNX_IF_ERR_EXIT(rv);

        if(port_config.interface == SOC_PORT_IF_NULL) {
            continue;
        }

        flags = 0;

        /* Is NIF ? */
        if(port_config.is_nif) {
            flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
        }

        /* Is STIF ? */
        stat_if_en = soc_property_port_get(unit, port_i, spn_STAT_IF_ENABLE, 0);
        if(stat_if_en) {
            flags |= SOC_PORT_FLAGS_STAT_INTERFACE;
        }

        /* Is PON ? */
        rv = soc_arad_str_prop_parse_pon_port(unit, port_i, &is_pon);
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_pon) {
            SOC_DPP_CONFIG(unit)->pp.pon_application_enable = 1;
            flags |= SOC_PORT_FLAGS_PON_INTERFACE;
        }

        /* Is init VID enabled ? */
        rv = soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port_i, &init_vid_enabled);
        SOCDNX_IF_ERR_EXIT(rv);

        /* In this port, support use Initial-VID with no differencebetween 
         * untagged packets and tagged packets. and need install initial-vid
         * program when init isem DB.
         * The decision to install programs on init isem DB is decided by 
         * port_use_initial_vlan_only_enabled parameter.
         */
        if (!init_vid_enabled) {
            SOC_DPP_CONFIG(unit)->pp.port_use_initial_vlan_only_enabled = 1;
            flags |= SOC_PORT_FLAGS_INIT_VID_ONLY;
        } else {
            /* indicates if we really need to allocate some default programs */
            /* In case Initial-VID is enabled for all ports then we can skip some programs */
            SOC_DPP_CONFIG(unit)->pp.port_use_outer_vlan_and_initial_vlan_enabled = 1;
        }

        /* Add to DB */
        rv = soc_port_sw_db_port_validate_and_add(unit, port_i, port_config.core, port_config.channel, flags, port_config.interface, port_config.phy_pbmp);
        SOCDNX_IF_ERR_EXIT(rv);

        if ( port_config.interface == SOC_PORT_IF_ILKN || port_config.interface == SOC_PORT_IF_CAUI) {
            rv = soc_port_sw_db_protocol_offset_set(unit, port_i, port_config.protocol_offset);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        rv = soc_port_sw_db_is_hg_set(unit, port_i, port_config.is_hg);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_port_sw_db_local_to_tm_port_set(unit, port_i, port_config.tm_port);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_port_sw_db_local_to_pp_port_set(unit, port_i, port_config.tm_port); /* tm_port == pp_port*/
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Add OLP/OAMP/ERP ports */
    SOC_PBMP_CLEAR(phy_ports);

    erp_base_q_pair = (ARAD_EGR_NOF_PS-1)*ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    for(core=0 ; core<SOC_DPP_DEFS_GET(unit, nof_cores) ; core++) 
    {
        si->olp_port[core] = -1;
        val = soc_property_suffix_num_get(unit, core, spn_NUM_OLP_TM_PORTS, "core", 0);
        if (val > 0) {
            si->olp_port[core] = ARAD_OLP_PORT_ID + core;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, si->olp_port[core], core, 0, 0, SOC_PORT_IF_OLP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->olp_port[core], ARAD_OLP_PORT_ID));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->olp_port[core], ARAD_OLP_PORT_ID));  
        }

        si->oamp_port[core] = -1;
        val = soc_property_suffix_num_get(unit, core, spn_NUM_OAMP_PORTS, "core", 0);
        if (val > 0) {
            si->oamp_port[core] = ARAD_OAMP_PORT_ID + core;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, si->oamp_port[core], core, 0, 0, SOC_PORT_IF_OAMP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->oamp_port[core], ARAD_OAMP_PORT_ID));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->oamp_port[core], ARAD_OAMP_PORT_ID));
        }

        si->erp_port[core] = -1;
        val = soc_property_suffix_num_get(unit, core, spn_NUM_ERP_TM_PORTS, "core", 0);
        if (val > 0) {
            si->erp_port[core] = SOC_DPP_PORT_INTERNAL_ERP(core);
            erp_tm_port_id = soc_property_suffix_num_get(unit, si->erp_port[core], spn_LOCAL_TO_TM_PORT, "erp", ARAD_ERP_PORT_ID);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, si->erp_port[core], core, 0, 0, SOC_PORT_IF_ERP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->erp_port[core], erp_tm_port_id)); 
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->erp_port[core], erp_tm_port_id)); /*meaningless, added for compliance*/  
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_set(unit, si->erp_port[core], erp_base_q_pair));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_set(unit, si->erp_port[core], 1));
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

    /* Get Speeds for all ports */
    SOC_PBMP_ITER(ports_bm, port_i) {
        val = soc_property_port_get(unit, port_i, spn_PORT_INIT_SPEED, -1);
        if(-1 == val) {
            SOCDNX_IF_ERR_EXIT(soc_pm_default_speed_get(unit, port_i, &val));
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port_i, val));
    }

    /* allocate egress interface per core */
    SOC_PBMP_ITER(ports_bm, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_interface_alloc(unit, port_i));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

/*
 *  Reset Functions
 */

int soc_jer_device_blocks_reset(
    int unit,
    int reset_action)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));

        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PDM_RSTNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above_64_val));

        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));

        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PDM_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above_64_val));

        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

        /* Verify blocks are OOR */
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr,                   REG_PORT_ANY, 0, IQC_INITf,         0x0));
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, IPS_INIT_TRIGGERf, 0x0));
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr,      REG_PORT_ANY, 0, EGQ_BLOCK_INITf,   0x0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Configures soc data structures specific to Jericho
 */
int soc_dpp_get_default_config_jer(
    int unit)
{

    soc_dpp_config_jer_t *jer;

    SOCDNX_INIT_FUNC_DEFS;

    jer = SOC_DPP_JER_CONFIG(unit);

    /* fix soc_dpp_defines_t in case of QMX */ 
    if (SOC_IS_QMX(unit)) {
        SOC_DPP_DEFS_SET(unit, nof_fabric_links, 16); 
        SOC_DPP_DEFS_SET(unit, nof_fabric_macs, 4);
        SOC_DPP_DEFS_SET(unit, nof_instances_fmac, 4);
    }

    /* Already reset in SOC attach */
    sal_memset(jer, 0, sizeof(soc_dpp_config_jer_t));

/* exit: */
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_pp_config_protection_get
 * Purpose:
 *      Retrieve Jericho specific configuration for the PP Protection module.
 *      Retrieve SOC Property values.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      void
 */
void soc_jer_pp_config_protection_get(
    int unit)
{
    soc_dpp_config_jer_pp_t *jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);
    char *prop_key;
    uint32 prop_value;

    /* Get the SOC Property value for the Ingress protection Coupled mode
       Default: Coupled mode (1)
       The mode is valid only for Jericho and above */
    prop_key = spn_BCM886XX_INGRESS_PROTECTION_COUPLED_MODE;
    prop_value = soc_property_get(unit, prop_key, 1);
    jer_pp_config->protection_ingress_coupled_mode = (prop_value) ? 1 : 0;

    /* Get the SOC Property value for the Egress protection Coupled mode
       Default: Coupled mode (1)
       The mode is valid only for Jericho and above */
    prop_key = spn_BCM886XX_EGRESS_PROTECTION_COUPLED_MODE;
    prop_value = soc_property_get(unit, prop_key, 1);
    jer_pp_config->protection_egress_coupled_mode = (prop_value) ? 1 : 0;

    /* Get the SOC Property value for the FEC protection accelerated reroute mode
       Default: Not accelerated (0)
       The mode is valid only for Jericho and above */
    prop_key = spn_BCM886XX_FEC_ACCELERATED_REROUTE_MODE;
    prop_value = soc_property_get(unit, prop_key, 0);
    jer_pp_config->protection_fec_accelerated_reroute_mode = (prop_value) ? 1 : 0;
}

/*
 * Function:
 *      soc_jer_specific_info_config_direct
 * Purpose:
 *      Configures soc properties specific for Jericho which are not dependent on
 *      other common soc properties to both Jericho and Arad.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_specific_info_config_direct(int unit)
{
    int rv, fmc_dbuff_mode;
    uint32 dram_total_size;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_INFO(unit).fabric_logical_port_base = soc_property_get(unit, spn_FABRIC_LOGICAL_PORT_BASE, SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT);
	SOCDNX_IF_ERR_EXIT(soc_jer_ports_config(unit));

    rv = soc_jer_str_prop_mc_nbr_full_dbuff_get(unit, &fmc_dbuff_mode);
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode = fmc_dbuff_mode;

    SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_init_pdm_nof_entries_calc(unit, SOC_DPP_CONFIG(unit)->arad->init.dram.pdm_mode, &dram_total_size)));
    SOC_DPP_CONFIG(unit)->jer->dbuffs.max_nof_dram_buffers = SOC_SAND_MIN(dram_total_size, (SOC_DPP_CONFIG(unit)->arad->init.dram.dram_size_total_mbyte * 1024 * 1024) / SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size);

    SOC_INFO(unit).fabric_logical_port_base = soc_property_get(unit, spn_FABRIC_LOGICAL_PORT_BASE, SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT);

exit: 
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_specific_info_config_derived
 * Purpose:
 *      Configures soc properties specific for Jericho which are dependent on soc properties 
 *      which are either common to both Jericho and Arad or uniqe for Jericho.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_specific_info_config_derived(int unit) 
{
    ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);
    SOCDNX_INIT_FUNC_DEFS;

    if (init->pp_enable == TRUE) {
        /* Get Jericho specific Protection configuration */
        soc_jer_pp_config_protection_get(unit);
    }

    /* arp entry msbs for roo host format. 
     * This soc property is used to configure MactFormat3EeiBits[2:1]. 
     * in arad+ eei format: 1111 HI(4) Native-ARP(15) 0.
     *  jericho eei format: 11 MactFormat3EeiBits[2:1] HI(4) Native-ARP(15) MactFormat3EeiBits[0]
     */
    SOC_DPP_CONFIG(unit)->jer->pp.roo_host_arp_msbs = (soc_property_get(unit, spn_BCM886XX_ROO_HOST_ARP_MSBS, 0x11)); 

    /* configure LAG to use the LSB/MSB of the lb-key */
    init->ports.smooth_division_resolve_using_msb = (soc_property_get(unit, spn_TRUNK_RESOLVE_USE_LB_KEY_MSB_SMOOTH_DIVISION, 0x0));
    init->ports.stack_resolve_using_msb = (soc_property_get(unit, spn_TRUNK_RESOLVE_USE_LB_KEY_MSB_STACK, 0x0));

    SOCDNX_FUNC_RETURN;
}

/*
 * Init functions
 */



int soc_jer_device_hard_reset(
    int unit,
    int reset_action)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_device_hard_reset(unit, reset_action));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_init_reg_access(
    int unit,
    int reset_action)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*
     * Reset device.
     * Also enable device access, set default Iproc/CmicD configuration
     * No access allowed before this stage.
     */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_reset(unit, reset_action));

    /* Enable Access to device blocks */
    SOCDNX_IF_ERR_EXIT(soc_jer_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET));

    /* Init blocks' broadcast IDs */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_brdc_blk_id_set(unit));

    /* Init WB engine for ports reg access if not already initiated */
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        SOCDNX_IF_ERR_RETURN(soc_dpp_wb_engine_init(unit)); 
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#endif

    /* Enable ports reg access */
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        soc_dpp_implementation_defines_init(unit);
    }
    SOCDNX_IF_ERR_EXIT(soc_jer_ports_config(unit));

    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_device_reset(
    int unit,
    int mode,
    int action)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Jericho function. invalid Device")));
    }

    switch (mode) {
    case SOC_DPP_RESET_MODE_HARD_RESET:
        rv = soc_jer_device_hard_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_RESET:
        rv = soc_jer_device_blocks_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET:
        /*rv = soc_arad_device_blocks_soft_reset(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET:
        /*rv = soc_arad_device_blocks_soft_ingress_reset(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET:
        /*rv = soc_arad_device_blocks_soft_egress_reset(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        /*rv = soc_arad_device_blocks_and_fabric_soft_reset(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET:
        /*rv = soc_arad_device_blocks_and_fabric_soft_ingress_reset(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET:
        /*rv = soc_arad_device_blocks_and_fabric_soft_egress_reset(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_INIT_RESET:
        /*rv = soc_arad_init(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;
    case SOC_DPP_RESET_MODE_REG_ACCESS:
        rv = soc_jer_init_reg_access(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_ENABLE_TRAFFIC:
        /*rv = soc_arad_init_enable_traffic(unit, action);
           SOCDNX_IF_ERR_RETURN(rv); */
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("not implemented on Jericho yet")));
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown/Unsupported Reset Mode")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_init_dma_init
 * Purpose:
 *      init cmic DMA mechanizems: SBUSDMA, Packet DMA
 * Parameters:
 *      unit -  unit number
 * Returns:
 *      SOC_E_XXX
 *
 */
int soc_jer_init_dma_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Packet DMA descriptors Initilazation */
    soc_dcb_unit_init(unit);

    /* Setup Packet DMA structures when a device is attached */
    SOCDNX_IF_ERR_EXIT(soc_dma_attach(unit, 1 /* Reset */));

    /* Init cmic_pcie_userif_purge_ctrl */
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_pcie_userif_purge_ctrl_init(unit));

    /* Init mutexes for Table/Slam DMA */
    SOCDNX_IF_ERR_EXIT(soc_arad_dma_mutex_init(unit));

#ifdef BCM_SBUSDMA_SUPPORT
    /* Init SBUSDMA descriptors */
    SOCDNX_IF_ERR_EXIT(soc_sbusdma_desc_init(unit, 0, 0));
#endif

    SOCDNX_IF_ERR_EXIT(dcmn_init_fill_table(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Purpose:
 *      de-init cmic DMA mechanizems: SBUSDMA, Packet DMA
 * Parameters:
 *      unit -  unit number
 * Returns:
 *      SOC_E_XXX
 *
 */
int soc_jer_deinit_dma(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_SBUSDMA_SUPPORT
    /* De-init SBUSDMA descriptors */
    SOCDNX_IF_ERR_EXIT(soc_sbusdma_desc_detach(unit));
#endif
    SOCDNX_IF_ERR_EXIT(dcmn_deinit_fill_table(unit));

    /* Detach DMA */
    soc_arad_dma_mutex_destroy(unit);

    SOCDNX_IF_ERR_EXIT(soc_dma_detach(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Configure CMIC. */
int soc_jer_init_reset_cmic_regs(
    int unit)
{
    uint32 core_freq = 0x0;
    int schan_timeout = 0x0;

    SOCDNX_INIT_FUNC_DEFS;

    /*
     * Map the blocks to their Sbus rings.
     * SBUS ring map:
     * Ring 2:
     * Ring 3:
     * Ring 5:
     * Ring 7:
     */
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x04444447));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x22222334));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x33433222));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x33333333));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x22222233));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x55222222));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x46655555));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x42444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x66666664));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x66666666));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x66666666));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_88_95r(unit, 0x66666666));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_96_103r(unit, 0x66666666));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_104_111r(unit, 0x55220666));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_112_119r(unit, 0x63633445));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x20000000));

    /* Set SBUS timeout */
    SOCDNX_IF_ERR_EXIT(soc_arad_core_frequency_config_get(unit, SOC_JER_CORE_FREQ_KHZ_DEFAULT, &core_freq));
    SOCDNX_IF_ERR_EXIT(soc_arad_schan_timeout_config_get(unit, &schan_timeout));
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_sbus_timeout_set(unit, core_freq, schan_timeout));

    /* Clear SCHAN_ERR */
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC0_SCHAN_ERRr(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC1_SCHAN_ERRr(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC2_SCHAN_ERRr(unit, 0));

    /* ECI check */
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_eci_access_check(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_reset(
    int unit,
    int reset_action)
{
    int disable_hard_reset = 0x0;

    SOCDNX_INIT_FUNC_DEFS;

    /* Configure PAXB, enabling the access of CMIC */
    SOCDNX_IF_ERR_EXIT(soc_dcmn_iproc_config_paxb(unit));

    /* Arad CPS Reset */
    disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0);
    if (disable_hard_reset == 0) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_device_hard_reset(unit, reset_action));
    }

    SOCDNX_IF_ERR_EXIT(soc_dcmn_iproc_config_paxb(unit));

    /* Config Endianess */
    soc_endian_config(unit);
    soc_pci_ep_config(unit, 0);

    /* Config Default/Basic cmic registers */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_reset_cmic_regs(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_rcpu_base_q_pair_init(int unit, int port_i)
{
    uint32 base_q_pair = 0, rval = 0;
    
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port_i, &base_q_pair));

    if (base_q_pair < 32) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_0r(unit, &rval));
        rval |= 0x1 << base_q_pair;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_0r(unit, rval));
    } else if (base_q_pair < 64) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_1r(unit, &rval));
        rval |= 0x1 << (base_q_pair - 32);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_1r(unit, rval));
    } else if (base_q_pair < 96) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_2r(unit, &rval));
        rval |= 0x1 << (base_q_pair - 64);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_2r(unit, rval));
    } else if (base_q_pair == 96) 
    {
        rval = 0x1;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_3r(unit, rval));
    } else 
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: RCPU base_q_pair range is 0 - 96\n")) );
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }              
    exit:
         SOCDNX_FUNC_RETURN;
}

int soc_jer_rcpu_init(int unit, soc_dpp_config_t *dpp)
{
    int port_i = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(dpp->arad->init.rcpu.slave_port_pbmp, port_i) 
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_rcpu_base_q_pair_init(unit, port_i));
    }

    exit:
         SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_dpp_jericho_init
 * Purpose:
 *      Optionally reset, and initialize the Device.
 * Parameters:
 *      unit -  unit number
 *      reset_action - Integer, indicates the reset action.
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 *
 */
int soc_dpp_jericho_init(
    int unit,
    int reset_action)
{

    soc_dpp_config_t *dpp = NULL;

#ifdef JERICHO_HW_IMPLEMENTATION
    int silent = 0;
    int port_i, tm_port;
    int header_type_in;
    uint32 base_q_pair, rval;
#endif                          /* JERICHO_HW_IMPLEMENTATION */

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_CONTROL(unit) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("SOC_CONTROL() is not allocated.")));
    }

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Jericho function. Invalid Device")));
    }

    dpp = SOC_DPP_CONFIG(unit);
    if ((dpp == NULL) || (dpp->arad == NULL) || (SOC_IS_ARADPLUS(unit) && (dpp->arad_plus == NULL)) || (dpp->jer == NULL)) {         
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("Soc control stractures are not allocated.")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#endif


    /* Set default configuration */
    SOCDNX_IF_ERR_EXIT(soc_dpp_get_default_config_jer(unit));

    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_direct(unit));

    SOCDNX_IF_ERR_EXIT(soc_arad_info_config(unit));

    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_derived(unit));

    /*
     * Reset device.
     * Also enable device access, set default Iproc/CmicD configuration
     * No access allowed before this stage.
     */
    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_reset(unit, reset_action));
    }

    /* Init interrupts */
    SOCDNX_IF_ERR_EXIT(soc_jer_interrupts_init(unit));

    /* Keep device blocks in reset */
    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET));
    }

    /* Disable interrupts */
    SOCDNX_IF_ERR_EXIT(soc_jer_interrupts_disable(unit));

    /* Attach/Init DMA */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_dma_init(unit));

    /* Common device init */
    SOCDNX_IF_ERR_EXIT(soc_dpp_common_init(unit));

#ifdef JERICHO_HW_IMPLEMENTATION
    SOCDNX_IF_ERR_EXIT(soc_arad_init_interrupts_db(unit));

    /*
     * Initialize SOC link control module
     */
    SOCDNX_IF_ERR_RETURN(soc_linkctrl_init(unit, &soc_linkctrl_driver_arad));
#endif                          /* JERICHO_HW_IMPLEMENTATION */

    if (SOC_WARM_BOOT(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Init phase 1 */
        SOCDNX_SAND_IF_ERR_EXIT(soc_jer_init_sequence_phase1(unit));

        /* Common TM init */
        SOCDNX_IF_ERR_EXIT(soc_dpp_common_tm_init(unit, &(dpp->tm.multicast_egress_bitmap_group_range)));
    } else {
        /* only reinit operation mode */
#ifdef JERICHO_HW_IMPLEMENTATION
        arad_sw_db_tdm_mode_set(unit, dpp->arad->init.tdm_mode);
        arad_sw_db_ilkn_tdm_dedicated_queuing_set(unit, dpp->arad->init.ilkn_tdm_dedicated_queuing);
        arad_sw_db_is_petrab_in_system_set(unit, dpp->arad->init.is_petrab_in_system);
#endif                          /* JERICHO_HW_IMPLEMENTATION */
    }

    /*
     * pp init
     */
    if (dpp->arad->init.pp_enable) {
        SOCDNX_IF_ERR_EXIT(soc_arad_pp_init(unit));
    }

    /* Arad-only SOC properties */

    /*
     * Init TDM
     */

    /* no need to reinit TDM during warm reboot. */
#ifdef JERICHO_HW_IMPLEMENTATION
    if (!SOC_WARM_BOOT(unit)) 
    {
        int core;
        /*
         * Initialize TDM applications.
         */
        if (dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT || dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA) {
            /* Set default ftmh for each relevant tm port */
            SOC_TMC_TDM_FTMH_INFO ftmh_info;

            SOC_TMC_TDM_FTMH_INFO_clear(&ftmh_info);
            ftmh_info.action_ing = ARAD_TDM_ING_ACTION_ADD;
            ftmh_info.action_eg = ARAD_TDM_EG_ACTION_REMOVE;

            ARAD_PORT_SW_DB_VALID_PORTS_ITER(unit,port_i) {/*!!! Iteration !!!*/
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
                if (port_i != SOC_DPP_PORT_INTERNAL_ERP(core)) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_get(unit, port_i, &header_type_in));
                    if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                        SOCDNX_SAND_IF_ERR_EXIT(arad_tdm_ftmh_set(unit, tm_port, &ftmh_info));
                    }
                }
            }
        }

        /* TDM Optimize init */
        if (dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT) {
            /* cell size in Optimize mode. */
            SOCDNX_SAND_IF_ERR_EXIT(arad_tdm_opt_size_set(unit, SOC_DPP_DRV_TDM_OPT_SIZE));
        }

        /* TDM Always High Priority Scheduling { */
        {
            uint32 is_valid;
            SOC_TMC_EGR_OFP_SCH_INFO ofp_sch_info;
            SOC_TMC_EGR_OFP_SCH_INFO_clear(&ofp_sch_info);

            for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
                /* Invalid port */
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
                if (!is_valid) {
                    continue;
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));

                if (IS_TDM_PORT(unit, port_i)) {
                    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_ofp_scheduling_get(unit, tm_port, &ofp_sch_info));
                    ofp_sch_info.nif_priority = SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH;
                    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_ofp_scheduling_set(unit, tm_port, &ofp_sch_info));
                }
            }
        }
        /* TDM High Low Priority ILKN } */

        /* Init RCPU */
        SOCDNX_IF_ERR_EXIT(soc_jer_rcpu_init(unit, dpp));

        rv = soc_jer_ser_init(unit);
        SOCDNX_SAND_IF_ERR_RETURN(rv);
    }
#endif                          /* JERICHO_HW_IMPLEMENTATION */
    arad_fast_regs_and_fields_access_init(unit);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_dpp_jericho_deinit
 * Purpose:
 *      De-initialize the Device.
 * Parameters:
 *      unit -  unit number
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a soc_dpp_jericho_init()
 *
 */
int soc_dpp_jericho_deinit(
    int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Detach/De-init interrupts */
    SOCDNX_IF_ERR_EXIT(soc_jer_interrupts_deinit(unit));

    /* Detach/De-init tables */
    SOCDNX_IF_ERR_EXIT(soc_jer_tbls_deinit(unit));

    /* Detach/De-init DMA */
    SOCDNX_IF_ERR_EXIT(soc_jer_deinit_dma(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

