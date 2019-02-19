/*
 * $Id: port_sw_db.c,v 1.43 Broadcom SDK $
 *
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
 * SOC PORT SW DB
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>

#define SW_DB_PRINT_COLUMN_LENGTH (24)
#define SW_DB_PRINT_MAX_COLUMNS_IN_LINE (6)


#include <soc/dcmn/error.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/cm.h>
#include <soc/portmode.h>
#include <soc/drv.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h> /* Doron TBD remove*/

/*****************************/
/****    Containers       ****/
/*****************************/
soc_port_unit_info_t        ports_unit_info[SOC_MAX_NUM_DEVICES];
soc_phy_port_sw_db_t        phy_ports_info[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
soc_logical_port_sw_db_t    logical_ports_info[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
soc_port_core_info_t        core_info[SOC_MAX_NUM_DEVICES][SOC_DPP_DEFS_MAX(NOF_CORES)];

/*****************************/
/****       Macros        ****/
/*****************************/
#define PORT_INFO       (logical_ports_info[unit][port])
#define PORT_PHY        (logical_ports_info[unit][port].first_phy_port)
#define PORT_PHY_INFO   (phy_ports_info[unit][PORT_PHY])
#define PORT_UNIT_INFO  (ports_unit_info[unit])
#define PORT_CORE       (PORT_PHY_INFO.core)
#define PORT_CORE_INFO  (core_info[unit][PORT_CORE])

#define PORT_INFO_INDEX(interface, phy) ((interface == SOC_PORT_IF_QSGMII) && ((phy-1)%4 == 0) ? 1 : 0)

/*****************************/
/**** Static  functions   ****/
/*****************************/
STATIC soc_error_t soc_port_sw_db_master_channel_set(int unit, soc_port_t port, soc_port_t master_port);
STATIC soc_error_t soc_port_sw_db_phy_ports_set(int unit, soc_port_t port, soc_pbmp_t phy_ports);
STATIC soc_error_t soc_port_sw_db_first_phy_port_set(int unit, soc_port_t port, uint32 phy_port /*one based*/);
STATIC soc_error_t soc_port_sw_db_flags_set(int unit, soc_port_t port, uint32 flags);
STATIC soc_error_t soc_port_sw_db_num_of_channels_set(int unit, soc_port_t port, uint32 num_of_channels);
STATIC soc_error_t soc_port_sw_db_channel_set(int unit, soc_port_t port, uint32 channel);
STATIC soc_error_t port_sw_db_packet_tdm_check(int unit, SOC_TMC_PORT_HEADER_TYPE header_type);

/*****************************/
/****  SW DB  functions   ****/
/*****************************/

soc_error_t
soc_port_sw_db_wb_init(int unit) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_init(int unit) 
{
    soc_port_t port;
    int blk, bindex, i;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(phy_ports_info[unit], 0, sizeof(soc_phy_port_sw_db_t) * SOC_MAX_NUM_PORTS);
    sal_memset(logical_ports_info[unit], 0, sizeof(soc_logical_port_sw_db_t) * SOC_MAX_NUM_PORTS);

    SOC_PBMP_CLEAR(PORT_UNIT_INFO.quads_out_of_reset);
    SOC_PBMP_CLEAR(PORT_UNIT_INFO.all_phy_pbmp);

    for (port = 0 ;; port++) {
        blk = SOC_DRIVER(unit)->port_info[port].blk;
        bindex = SOC_DRIVER(unit)->port_info[port].bindex;
        if (blk < 0 && bindex < 0) {            /* end of list */
            break;
        }
        if (blk < 0) {                          /* empty slot */
            continue;
        }
        SOC_PBMP_PORT_ADD(PORT_UNIT_INFO.all_phy_pbmp, port);
    }

    /* Init all ports */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        SOC_INFO(unit).port_p2l_mapping[port] = -1;
    }
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        SOC_INFO(unit).port_l2p_mapping[port] = -1;
        SOC_INFO(unit).port_speed_max[port] = -1;
        SOC_INFO(unit).port_num_lanes[port] = 1;
        SOC_INFO(unit).port_type[port] = SOC_BLK_NONE;
    }

    SOC_INFO(unit).rcy_port_count = 0;
    SOC_INFO(unit).rcy_port_start = -1;

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        PORT_INFO.base_q_pair = ARAD_EGR_INVALID_BASE_Q_PAIR;
        PORT_INFO.tm_port = -1;
        PORT_INFO.pp_port = -1;
        PORT_INFO.egr_interface = INVALID_EGR_INTERFACE;
        PORT_INFO.protocol_offset = 0;
        PORT_PHY_INFO.high_pirority_cal = INVALID_CALENDAR;
        PORT_PHY_INFO.low_pirority_cal = INVALID_CALENDAR;
        PORT_PHY_INFO.is_single_cal_mode = FALSE;
    }

    for(i=0 ; i<SOC_DPP_DEFS_MAX(NOF_CORES) ; i++) {
        for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
            core_info[unit][i].tm_to_local_port_map[port] = SOC_MAX_NUM_PORTS;
            core_info[unit][i].pp_to_local_port_map[port] = SOC_MAX_NUM_PORTS;
        }
    }

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_destroy(int unit) 
{
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(phy_ports_info[unit], 0, sizeof(soc_phy_port_sw_db_t) * SOC_MAX_NUM_PORTS);
    sal_memset(logical_ports_info[unit], 0, sizeof(soc_logical_port_sw_db_t) * SOC_MAX_NUM_PORTS);

    SOC_PBMP_CLEAR(PORT_UNIT_INFO.quads_out_of_reset);
    SOC_PBMP_CLEAR(PORT_UNIT_INFO.all_phy_pbmp);

    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_pm_interface_properties_set
 * Purpose:
 *      Per interface properties add
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port                - (IN) Port Number;
 *      all_phy_pbmp        - (IN) ALL phy ports of the device (include such that aren't in use)
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t
soc_port_sw_db_interface_properties_set(int unit, soc_port_t port) 
{
    char *bname;
    SOCDNX_INIT_FUNC_DEFS;

    switch (PORT_PHY_INFO.interface_type) {
        case SOC_PORT_IF_SGMII:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).ge.num;
            PORT_SW_DB_PORT_ADD(ge, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "ge";
            break;
        case SOC_PORT_IF_DNX_XAUI:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).xe.num;
            PORT_SW_DB_PORT_ADD(xe, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 10000;
            bname = "xe";
            break;
        case SOC_PORT_IF_XFI:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).xe.num;
            PORT_SW_DB_PORT_ADD(xe, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 10000;
            bname = "xe";
            break;
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLAUI2:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).xl.num;
            PORT_SW_DB_PORT_ADD(xl, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = (IS_HG_PORT(unit, port)) ? 42000 : 40000;
            bname = "xl";
            break;
        case SOC_PORT_IF_RXAUI:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).xe.num;
            PORT_SW_DB_PORT_ADD(xe, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 10000;
            bname = "xe";
            break;
        case SOC_PORT_IF_ILKN:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).il.num;
            PORT_SW_DB_PORT_ADD(il, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 12500;
            bname = "il";
            break;
        case SOC_PORT_IF_CAUI:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).ce.num;
            PORT_SW_DB_PORT_ADD(ce, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            if (SOC_INFO(unit).port_num_lanes[port] == 12) {
                SOC_INFO(unit).port_speed_max[port] = (IS_HG_PORT(unit, port)) ? 127000 : 120000;
            } else {
                SOC_INFO(unit).port_speed_max[port] = (IS_HG_PORT(unit, port)) ? 106000 : 100000;
            }
            bname = "ce";
            break;
        case SOC_PORT_IF_QSGMII:
            SOC_INFO(unit).port_offset[port] = SOC_INFO(unit).qsgmii.num;
            PORT_SW_DB_PORT_ADD(qsgmii, port);
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "qsgmii";
            break;
        case SOC_PORT_IF_CPU:
            SOC_PBMP_PORT_ADD((SOC_INFO(unit).cmic_bitmap), port);
            PORT_SW_DB_PORT_ADD(all, port);
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "cpu";
            break;
        case SOC_PORT_IF_TM_INTERNAL_PKT:
            PORT_SW_DB_PORT_ADD(port, port);
            PORT_SW_DB_PORT_ADD(all, port);
            /*Un-mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_DISABLED));
            SOC_INFO(unit).port_speed_max[port] = 1200000;
            bname = "tm_internal_pkt";
            break;
        case SOC_PORT_IF_RCY:
            PORT_SW_DB_PORT_ADD(rcy, port);
            PORT_SW_DB_PORT_ADD(all, port);
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "rcy";
            SOC_INFO(unit).rcy_port_count++;
            SOC_INFO(unit).rcy_port_start = (SOC_INFO(unit).rcy_port_start < port && SOC_INFO(unit).rcy_port_start >= 0 ? SOC_INFO(unit).rcy_port_start : port);
            break;
        case SOC_PORT_IF_OLP:
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "olp";
            break;
        case SOC_PORT_IF_OAMP:
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "oamp";
            break;
        case SOC_PORT_IF_ERP:
            SOC_INFO(unit).port_speed_max[port] = 1000;
            bname = "erp";
            break;
        case SOC_PORT_IF_NOCXN:
            SOC_INFO(unit).port_speed_max[port] = 0;
            bname = "nocxn";
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface %d isn't supported"), PORT_PHY_INFO.interface_type));
            break;
    }

    sal_snprintf(SOC_INFO(unit).port_name[port], sizeof(SOC_INFO(unit).port_name[port]), "%s%d", bname, port);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_port_add(int unit,  int core, soc_port_t port,uint32 channel, uint32 flags, soc_port_if_t interface, soc_pbmp_t phy_ports) 
{
    soc_port_t first_phy_port, phy_port, port_i;
    uint32 num_of_lanes, found;
    uint32 is_virtual; /*virtual port - isn't attached with real interface*/
    int blk, is_master_channel = 0;
    soc_block_type_t blktype;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_PBMP_NOT_NULL(phy_ports)) {
        is_virtual = 0;
        SOC_PBMP_ITER(phy_ports, first_phy_port) {break;}
    } else {
        /*virtual port*/
        is_virtual = 1;
        found = 0;
        /*1. attach to port with same interface if avaliable*/
        for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; port_i++) {
            if (logical_ports_info[unit][port_i].valid) {
                first_phy_port = logical_ports_info[unit][port_i].first_phy_port;
                if (interface == phy_ports_info[unit][first_phy_port].interface_type 
                    && core == phy_ports_info[unit][first_phy_port].core ) {
                    /*found port with same interface*/
                    found = 1;
                    break;
                }
            }
        }
        
        /*2. alllocate virtual phy port*/
        if (!found) {
            for (first_phy_port = 0; first_phy_port < SOC_MAX_NUM_PORTS; first_phy_port++) {
                /*make sure that: 1. the port not previously allocated 2. not phy port (can be virtual)*/
                if ((-1 == SOC_INFO(unit).port_p2l_mapping[first_phy_port])
                    && (!SOC_PBMP_MEMBER(PORT_UNIT_INFO.all_phy_pbmp, first_phy_port))) {
                    SOC_PBMP_PORT_ADD(PORT_UNIT_INFO.all_phy_pbmp, first_phy_port);
                    found = 1;
                    break;
                }
            }
        }
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("All virtual ports already allocated")));
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_set(unit, port, first_phy_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_set(unit, port, channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_set(unit, port, flags));

    SOCDNX_IF_ERR_EXIT(SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_CORE, &core, PORT_PHY));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_set(unit, port, PORT_PHY_INFO.channels_count + 1));
    if((PORT_PHY_INFO.channels_count > 1) || (interface == SOC_PORT_IF_ILKN)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_set(unit, port, 1)); 
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_set(unit, port, 0));
    }

    /*if first channel added to interface*/
    if (1 == PORT_PHY_INFO.channels_count) {
        /*init interface properties*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_set(unit, port, phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_set(unit, port, interface));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, 0));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_set(unit, port, port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port, 0));
        is_master_channel = 1;
    }

    /*num of lanes*/
    SOC_PBMP_COUNT(phy_ports, num_of_lanes);
    SOC_INFO(unit).port_num_lanes[port] = num_of_lanes;

    /*add to block bitmap*/
    if (SOC_PORT_IF_TM_INTERNAL_PKT == interface) {
        SOC_INFO(unit).port_type[port] = SOC_BLK_ECI;
    } else if (!is_virtual) {

        blk = SOC_PORT_IDX_INFO(unit, first_phy_port, PORT_INFO_INDEX(interface, first_phy_port)).blk;
        SOC_PBMP_PORT_ADD(SOC_INFO(unit).block_bitmap[blk], port);


        /*Set port block type*/
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        SOC_INFO(unit).port_type[port] = blktype;
        if (is_master_channel) {
            SOC_INFO(unit).block_port[blk] = port;
        }

        /*set CPU info*/
        if (SOC_PORT_IF_CPU == interface && is_master_channel) {
            SOC_INFO(unit).cmic_port = port;
            SOC_INFO(unit).cmic_block = blk;
        }
    } else {
        if (SOC_PORT_IF_NOCXN == interface) {
            SOC_INFO(unit).port_type[port] = SOC_BLK_NONE;
        } else {
            SOC_INFO(unit).port_type[port] = SOC_BLK_ECI;
        }
    }

    /*set logical-to-physical*/
    SOC_INFO(unit).port_l2p_mapping[port] = first_phy_port;

    /*set physical-to-logical*/
    if (is_master_channel) {
        SOC_PBMP_ITER(phy_ports, phy_port) {
            SOC_INFO(unit).port_p2l_mapping[phy_port] = port;

            /*for caui always set all 12 lanes*/
            if ((SOC_PORT_IF_CAUI == interface) && (10 == num_of_lanes)) {
                if (phy_port + 2 >= SOC_MAX_NUM_PORTS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Trying to acsess to port mapping with invalid index %d"), phy_port + 2));
                }
                SOC_INFO(unit).port_p2l_mapping[phy_port + 1] = port;
                SOC_INFO(unit).port_p2l_mapping[phy_port + 2] = port;
            }
        }
    }

    if (flags != SOC_PORT_FLAGS_VIRTUAL_RCY_INTERFACE) {
        /*specific interface properties*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_properties_set(unit, port));
    }

    /*increase number of ports*/
    SOC_INFO(unit).port_num++; 

    /* mark port as valid */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_set(unit, port, 1));

    /* set low and high calendars as invalid */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, port, INVALID_CALENDAR));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, port, INVALID_CALENDAR));
        
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_pm_port_add_validate
 * Purpose:
 *      Validate port before adding
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port                - (IN) Port Number.
 *      channel             - (IN) Channel number.
 *      interface           - (IN) Interface Type.
 *      phy_ports           - (IN) Phy ports in use by this port.
 * Returns:
 *      SOC_E_xxx
 */

STATIC soc_error_t
soc_port_sw_db_port_validate(int unit, soc_port_t port, uint32 channel, soc_port_if_t interface, soc_pbmp_t phy_ports) 
{
    uint32 port_i, is_valid, num_of_lanes, channel_o;
    soc_pbmp_t ports_bm, phy_ports_o, and_bm;
    soc_port_if_t interface_o;
    SOCDNX_INIT_FUNC_DEFS;

    /*check port not already defined*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Port %d already exists"),port));
    }

    /*1. check interface type is supported 
      2. check num_of_lanes according to interface type*/
    SOC_PBMP_COUNT(phy_ports, num_of_lanes);
    switch (interface) {
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_QSGMII:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_CPU:
            if (1 != num_of_lanes) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d number of lanes must be 1"), port));
            }
            break;
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_XLAUI2:
            if (2 != num_of_lanes) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d number of lanes must be 2"), port));
            }
            break;
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XLAUI:
            if (4 != num_of_lanes) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d number of lanes must be 4"), port));
            }
            break;
        case SOC_PORT_IF_CAUI:
            if (SOC_IS_JERICHO(unit)) {
                if(4 != num_of_lanes) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d number of lanes must be 4"), port));
                }
            } else {
                if (10 != num_of_lanes && 12 != num_of_lanes) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d number of lanes must be 10 or 12"), port));
                }
            }
            break;
        case SOC_PORT_IF_ILKN:
            if (1 > num_of_lanes) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d number of lanes must be at least 1"), port));
            }
            break;
        case SOC_PORT_IF_RCY:
        case SOC_PORT_IF_ERP:
        case SOC_PORT_IF_OLP:
        case SOC_PORT_IF_OAMP:
        case SOC_PORT_IF_TM_INTERNAL_PKT:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface %d isn't supported"), interface));
            break;
    }

    /*validate other interface on the same phy lanes*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));
    SOC_PBMP_ITER(ports_bm, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface_o));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port_i, &phy_ports_o));
        SOC_PBMP_ASSIGN(and_bm, phy_ports_o);
        SOC_PBMP_AND(and_bm, phy_ports);

        /*if two ports has overlap phy ports*/
        if (SOC_PBMP_NOT_NULL(and_bm)) {
            if (interface == interface_o) {
                /*if same interface physical lanes must have full agreement or no overlap at all*/
                if (SOC_PBMP_NEQ(phy_ports, phy_ports_o)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d and port %d has overlap phy ports"),port,port_i));
                } else {
                    /*same interface over same lanes - make sure channel is OK*/
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port_i, &channel_o));
                    if (channel == channel_o) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d and port %d has the same channel"),port,port_i));
                    }
                }

            } else {
                /*if not same interface overlap isn't allowed*/
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d and port %d has same interface overlap"),port,port_i));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_port_validate_and_add(int unit, int core, soc_port_t port,  uint32 channel, uint32 flags, soc_port_if_t interface, soc_pbmp_t phy_ports) 
{
    SOCDNX_INIT_FUNC_DEFS;

    /*validate port info*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate(unit, port, channel, interface, phy_ports));

    /*add port to DB*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_add(unit,  core, port,channel, flags, interface, phy_ports));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
soc_port_sw_db_bmp_remove(int unit, soc_ptype_t *bmp, soc_port_t port) 
{
    soc_pbmp_t temp;
    soc_port_t port_i;
    SOCDNX_INIT_FUNC_DEFS;

    /*save temp bitmap*/
    SOC_PBMP_ASSIGN(temp, bmp->bitmap);
    SOC_PBMP_PORT_REMOVE(temp, port);

    /*clear structure*/
    bmp->max = -1;
    bmp->min = -1;
    for (port_i = 0; port_i < bmp->num; port_i++) {
        bmp->port[port_i] = 0;
    }
    bmp->num = 0;
    SOC_PBMP_CLEAR(bmp->bitmap);

    /*rebuild structure*/
    SOC_PBMP_ITER(temp, port_i) {
        bmp->port[bmp->num] = port_i;
        bmp->num++;
        if ((bmp->min < 0) || (bmp->min > port_i)) {
            bmp->min = port_i;
        }
        if (port_i > bmp->max) {
            bmp->max = port_i;
        }
        SOC_PBMP_PORT_ADD(bmp->bitmap, port_i);
    }

    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_pm_interface_properties_remove
 * Purpose:
 *      Per interface properties remove
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port                - (IN) Port Number;
 *      all_phy_pbmp        - (IN) ALL phy ports of the device (include such that aren't in use)
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t
soc_port_sw_db_interface_properties_remove(int unit, soc_port_t port) 
{
    char *bname;
    SOCDNX_INIT_FUNC_DEFS;

    switch(PORT_PHY_INFO.interface_type) {   
        case SOC_PORT_IF_SGMII:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).ge), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
            break;
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_RXAUI:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).xe), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
            break;   
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLAUI2:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).xl), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
        break;
        case SOC_PORT_IF_ILKN:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).il), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
            break;
        case SOC_PORT_IF_CAUI:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).ce), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
            break;
        case SOC_PORT_IF_QSGMII:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).qsgmii), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
            break;
        case SOC_PORT_IF_CPU:
            SOC_PBMP_PORT_REMOVE((SOC_INFO(unit).cmic_bitmap), port);  
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port)); 
            break;
        case SOC_PORT_IF_TM_INTERNAL_PKT:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).port), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            /*Mark the port as disabled port - dynamic ports feature*/
            SOC_PBMP_PORT_ADD(SOC_INFO(unit).all.disabled_bitmap, port);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_DISABLED));
            break;
        case SOC_PORT_IF_RCY:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).rcy), port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).all), port));
            break;
        case SOC_PORT_IF_OLP:
        case SOC_PORT_IF_OAMP:
        case SOC_PORT_IF_ERP:
        case SOC_PORT_IF_NOCXN:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface %d isn't supported"), PORT_PHY_INFO.interface_type));
            break;
    }

    SOC_INFO(unit).port_speed_max[port] = -1;

    bname = "?";
    sal_snprintf(SOC_INFO(unit).port_name[port], sizeof(SOC_INFO(unit).port_name[port]), "%s%d", bname, port);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_next_master_get(int unit, soc_port_t port, soc_port_t *next_master) 
{
    soc_port_t port_i;
    SOCDNX_INIT_FUNC_DEFS;

    (*next_master) = -1;

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; port_i++) {
        /*search for port mapped to same interface*/
        if ((port_i != port) &&
            (logical_ports_info[unit][port_i].valid) &&
            (logical_ports_info[unit][port_i].first_phy_port == PORT_INFO.first_phy_port)) {
            (*next_master) = port_i;
            break;
        }
    }

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_port_remove(int unit, soc_port_t port) 
{
    soc_port_t new_master, port_i;
    uint32 is_master_channel, is_virtual, is_valid;
    int blk;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    is_virtual = SOC_PBMP_IS_NULL(PORT_PHY_INFO.phy_ports);
    is_master_channel = (PORT_PHY_INFO.master_port == port ? 1 : 0);

    PORT_CORE_INFO.tm_to_local_port_map[PORT_INFO.tm_port] = SOC_MAX_NUM_PORTS;
    PORT_CORE_INFO.pp_to_local_port_map[PORT_INFO.pp_port] = SOC_MAX_NUM_PORTS;

    PORT_INFO.base_q_pair = ARAD_EGR_INVALID_BASE_Q_PAIR;
    PORT_INFO.tm_port = -1;
    PORT_INFO.pp_port = -1;
    PORT_INFO.egr_interface = INVALID_EGR_INTERFACE;
    PORT_INFO.protocol_offset = 0;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_set(unit, port, 0));

    /*decrease number of ports*/
    SOC_INFO(unit).port_num--;

    /*specific interface properties*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_properties_remove(unit, port));

    /*need new master*/
    if (is_master_channel) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_next_master_get(unit, port, &new_master)); 

        for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; port_i++) {
            if (SOC_INFO(unit).port_p2l_mapping[port_i] == port) {
                SOC_INFO(unit).port_p2l_mapping[port_i] = new_master;
            }
        }
    } else {
        new_master = PORT_PHY_INFO.master_port;
    }

    /*clear logical-to-physical*/
    SOC_INFO(unit).port_l2p_mapping[port] = -1;

    /*clear to block bitmap*/
    if (!is_virtual) {
        blk = SOC_PORT_IDX_INFO(unit, PORT_INFO.first_phy_port, PORT_INFO_INDEX(PORT_PHY_INFO.interface_type, PORT_INFO.first_phy_port)).blk;
        SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).block_bitmap[blk], port);

        /*remove port block type*/
        SOC_INFO(unit).port_type[port] = SOC_BLK_NONE;
        if (is_master_channel) {
            SOC_INFO(unit).block_port[blk] = new_master;
        }

        /*remove CPU info*/
        if (SOC_PORT_IF_CPU == PORT_PHY_INFO.interface_type && is_master_channel) {
            SOC_INFO(unit).cmic_port = (new_master >= 0 ? new_master : 0);
            SOC_INFO(unit).cmic_block = (new_master >= 0 ? SOC_INFO(unit).cmic_block : -1);
        }
    } else {
        /*if virtual the phy_port isn't real and should be removed from all_phy bitmap*/
        SOC_PBMP_PORT_REMOVE(PORT_UNIT_INFO.all_phy_pbmp, PORT_INFO.first_phy_port);
    }

    /*num of lanes*/
    SOC_INFO(unit).port_num_lanes[port] = 1;

    /*if only channel added to interface*/
    if (1 == PORT_PHY_INFO.channels_count) {
        soc_pbmp_t pbmp_clear;
        /*init interface properties*/
        SOC_PBMP_CLEAR(pbmp_clear);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_set(unit, port, pbmp_clear));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_set(unit, port, SOC_PORT_IF_NOCXN));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, 0));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port, 0));
    }

    if (is_master_channel) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_set(unit, port, new_master));
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_set(unit, port, PORT_PHY_INFO.channels_count - 1));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_set(unit, port, -1));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_runt_pad_set(int unit, soc_port_t port, uint32 runt_pad) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_RUNT_PAD, &runt_pad, PORT_PHY)
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_runt_pad_get(int unit, soc_port_t port, uint32 *runt_pad) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_RUNT_PAD, runt_pad, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_first_phy_port_get(int unit, soc_port_t port, uint32 *phy_port /*one based*/) 
{
    uint32 found = 0, is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOC_PBMP_ITER(PORT_PHY_INFO.phy_ports, *phy_port) {
        found = 1;
        break;
    }

    if (!found) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d has no physical lanes"),port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_port_sw_db_first_phy_port_set(int unit, soc_port_t port, uint32 phy_port /*one based*/) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FIRST_PHY_PORT, &phy_port, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_in_block_index_get(int unit, soc_port_t port, uint32 *in_block_index /*zero based*/) 
{
    uint32 phy_port, is_valid;
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    *in_block_index = SOC_PORT_IDX_INFO(unit, phy_port, PORT_INFO_INDEX(interface, phy_port)).bindex;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_num_lanes_get(int unit, soc_port_t port, uint32 *count) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOC_PBMP_COUNT(PORT_PHY_INFO.phy_ports, (*count));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_interface_type_get(int unit, soc_port_t port, soc_port_if_t *interface_type) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *interface_type = PORT_PHY_INFO.interface_type;

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Check if the given port is a "virtual recycle port" which is really a dynamically allocated reassembly context.
 * returns TRUE or FALSE. On any error, just returns FALSE.
 */
int
soc_port_sw_db_interface_is_virt_rcy_port(int unit, soc_port_t port) 
{
    uint32 is_valid;
    return ((soc_port_sw_db_is_valid_port_get(unit, port, &is_valid) == SOC_E_NONE) &&
            (is_valid && PORT_PHY_INFO.interface_type == _SHR_PORT_IF_RCY) &&
            (PORT_INFO.flags & SOC_PORT_FLAGS_VIRTUAL_RCY_INTERFACE)) ?
           TRUE : FALSE;
}


soc_error_t
soc_port_sw_db_interface_type_set(int unit, soc_port_t port, soc_port_if_t interface_type) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INTERFACE_TYPE, &interface_type, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_speed_set(int unit, soc_port_t port, int speed) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_SPEED, &speed, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_speed_get(int unit, soc_port_t port, int *speed) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *speed = PORT_PHY_INFO.speed;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_flag_add(int unit, soc_port_t port, uint32 flag) 
{
    uint32 is_valid;
    uint32 flags;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (flag != SOC_PORT_FLAGS_DISABLED) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
        if (!is_valid) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
        }
    }

    flags = PORT_INFO.flags | flag;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FLAGS, &flags, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_flag_remove(int unit, soc_port_t port, uint32 flag) 
{
    uint32 is_valid;
    uint32 flags;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (flag != SOC_PORT_FLAGS_DISABLED) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
        if (!is_valid) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
        }
    }

    flags = PORT_INFO.flags & (~flag);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FLAGS, &flags, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_port_sw_db_flags_set(int unit, soc_port_t port, uint32 flags) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FLAGS, &flags, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_flags_get(int unit, soc_port_t port, uint32 *flags) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *flags = PORT_INFO.flags;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_encap_mode_set(int unit, soc_port_t port, soc_encap_mode_t encap_mode) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_ENCAP_MODE, &encap_mode, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

    /*we save this data in the hg in soc contol also - legacy*/
    if (encap_mode == SOC_ENCAP_HIGIG2) {
        PORT_SW_DB_PORT_ADD(hg, port);
        SOC_PBMP_PORT_ADD(SOC_HG2_PBM(unit), port);
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_bmp_remove(unit, &(SOC_INFO(unit).hg), port));
        SOC_PBMP_PORT_REMOVE(SOC_HG2_PBM(unit), port);
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_encap_mode_get(int unit, soc_port_t port, soc_encap_mode_t *encap_mode) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_ENCAP_MODE, encap_mode, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_is_hg_set(int unit, soc_port_t port, uint32 is_hg) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_set(unit, port, (is_hg) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_is_hg_get(int unit, soc_port_t port, uint32 *is_hg) 
{
    uint32 is_valid;
    soc_encap_mode_t encap_mode;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_get(unit, port, &encap_mode));
    *is_hg = (encap_mode == SOC_ENCAP_HIGIG2);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_initialized_set(int unit, soc_port_t port, uint32 is_initialized) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid && is_initialized) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INITIALIZED, &is_initialized, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_initialized_get(int unit, soc_port_t port, uint32 *is_initialized) 
{
    soc_error_t rv;
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INITIALIZED, is_initialized, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_single_cal_mode_set(int unit, soc_port_t port, int is_single_mode) 
{
    uint32      is_valid;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_SINGLE_CAL_MODE, &is_single_mode, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_single_cal_mode_get(int unit, soc_port_t port, int *is_single_mode) 
{
    soc_error_t rv;
    uint32 is_valid;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_SINGLE_CAL_MODE, is_single_mode, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_high_priority_cal_set(int unit, soc_port_t port, uint32 cal_id) 
{
    uint32 is_valid;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_HIGH_PRIORITY_CAL, &cal_id, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_high_priority_cal_get(int unit, soc_port_t port, uint32 *cal_id) 
{
    soc_error_t rv;
    uint32 is_valid;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_HIGH_PRIORITY_CAL, cal_id, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_low_priority_cal_set(int unit, soc_port_t port, uint32 cal_id) 
{
    uint32 is_valid;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LOW_PRIORITY_CAL, &cal_id, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_low_priority_cal_get(int unit, soc_port_t port, uint32 *cal_id) 
{
    soc_error_t rv;
    uint32 is_valid;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LOW_PRIORITY_CAL, cal_id, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_serdes_quads_out_of_reset_set(int unit, soc_pbmp_t quads) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(PORT_UNIT_INFO.quads_out_of_reset, quads);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_serdes_quads_out_of_reset_get(int unit, soc_pbmp_t *quads) 
{

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(*quads, PORT_UNIT_INFO.quads_out_of_reset);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_master_channel_get(int unit, soc_port_t port, soc_port_t *master_port) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }
    *master_port = PORT_PHY_INFO.master_port;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_is_master_get(int unit, soc_port_t port, uint32* is_master)
{
    soc_port_t master_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
    *is_master = (port == master_port);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_interface_rate_get(int unit, soc_port_t port, uint32* rate)
{
    uint32 num_of_lanes;
    int speed;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    
    if(SOC_PORT_IF_ILKN == interface_type) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));
        *rate = num_of_lanes*speed;
    } else {
        *rate = speed;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
port_sw_db_packet_tdm_check(int unit, SOC_TMC_PORT_HEADER_TYPE header_type)
{

    soc_error_t rv;
    soc_dpp_config_tdm_t *tdm = &(SOC_DPP_CONFIG(unit)->tdm);

    SOCDNX_INIT_FUNC_DEFS;

    rv = (!tdm->is_packet && header_type == SOC_TMC_PORT_HEADER_TYPE_TDM_RAW) ? SOC_E_CONFIG : SOC_E_NONE ;
    if (rv==SOC_E_CONFIG) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG,(_BSL_SOCDNX_MSG("Trying to configure port for packet tdm while packet tdm disabled for all device")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_port_sw_db_master_channel_set(int unit, soc_port_t port, soc_port_t master_port) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_MASTER_PORT, &master_port, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_is_valid_port_get(int unit, soc_port_t port, uint32 *is_valid) 
{
    SOCDNX_INIT_FUNC_DEFS;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        *is_valid = 0;
    } else {
        *is_valid = PORT_INFO.valid;
    }

    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_is_valid_port_set(int unit, soc_port_t port, uint32 valid) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    } else {
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_VALID, &valid, port);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_phy_ports_get(int unit, soc_port_t port, soc_pbmp_t *phy_ports) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOC_PBMP_ASSIGN(*phy_ports, PORT_PHY_INFO.phy_ports);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_port_sw_db_phy_ports_set(int unit, soc_port_t port, soc_pbmp_t phy_ports) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_PHY_PORTS, &phy_ports, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_valid_ports_get(int unit, uint32 required_flag, soc_pbmp_t *ports_bm) 
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*ports_bm);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (PORT_INFO.valid) {
            /*if no required flags - pass all*/
            if (!required_flag || (PORT_INFO.flags & required_flag)) {
                SOC_PBMP_PORT_ADD(*ports_bm, port);
            }
        }
    }

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_valid_ports_core_get(int unit, int core, uint32 required_flag, soc_pbmp_t *ports_bm) 
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*ports_bm);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (PORT_INFO.valid && PORT_PHY_INFO.core == core) {
            /*if no required flags - pass all*/
            if (!required_flag || (PORT_INFO.flags & required_flag)) {
                SOC_PBMP_PORT_ADD(*ports_bm, port);
            }
        }
    }

    SOCDNX_FUNC_RETURN;
}

/*ports bitmap get - include invalid ports*/
soc_error_t
soc_port_sw_db_ports_get(int unit, uint32 required_flag, soc_pbmp_t *ports_bm) 
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*ports_bm);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        /*if no required flags - pass all*/
        if (!required_flag || (PORT_INFO.flags & required_flag)) {
            SOC_PBMP_PORT_ADD(*ports_bm, port);
        }
    }

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_protocol_offset_set(int unit, soc_port_t port, uint32 offset) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PROTOCOL_OFFSET, &offset, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_protocol_offset_get(int unit, soc_port_t port, uint32 *offset) 
{
    soc_port_if_t interface_type;
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (SOC_PORT_IF_ILKN != interface_type && SOC_PORT_IF_CAUI != interface_type) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d isn't ILKN or CAUI port"),port));
    }

    *offset = PORT_INFO.protocol_offset;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_num_of_channels_get(int unit, soc_port_t port, uint32 *num_of_channels) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *num_of_channels = PORT_PHY_INFO.channels_count;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_port_sw_db_num_of_channels_set(int unit, soc_port_t port, uint32 num_of_channels) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_CHANNELS_COUNT, &num_of_channels, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_max_channel_num_get(int unit, soc_port_t port, uint32 *max_channel) 
{
    soc_port_t port_i;
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *max_channel = PORT_INFO.channel;

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; port_i++) {
        if (logical_ports_info[unit][port_i].valid) {
            if (PORT_INFO.first_phy_port == logical_ports_info[unit][port_i].first_phy_port) {
                if (*max_channel < logical_ports_info[unit][port_i].channel) {
                    *max_channel = logical_ports_info[unit][port_i].channel;
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_ports_to_same_interface_get(int unit, soc_port_t port, soc_pbmp_t *ports) 
{
    soc_port_t port_i;
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOC_PBMP_CLEAR(*ports);

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; port_i++) {
        if (logical_ports_info[unit][port_i].valid) {
            if (PORT_INFO.first_phy_port == logical_ports_info[unit][port_i].first_phy_port) {
                SOC_PBMP_PORT_ADD(*ports, port_i);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_port_from_interface_type_get(int unit, soc_port_if_t interface_type, int first_phy_port, soc_port_t *port_to_return) 
{
    soc_port_t port;

    SOCDNX_INIT_FUNC_DEFS;

    *port_to_return = SOC_MAX_NUM_PORTS;

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (logical_ports_info[unit][port].valid) {
            if (first_phy_port == logical_ports_info[unit][port].first_phy_port &&
                interface_type == PORT_PHY_INFO.interface_type) {
                *port_to_return = port;
                break;
            }
        }
    }

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_port_with_channel_get(int unit, soc_port_t port, uint32 channel, soc_port_t *port_match_channel) 
{
    soc_pbmp_t same_interface_ports;
    soc_port_t port_i;
    uint32 is_valid, channel_i = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    is_valid = 0;
    *port_match_channel = SOC_MAX_NUM_PORTS;
    SOC_PBMP_CLEAR(same_interface_ports);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &same_interface_ports));

    SOC_PBMP_ITER(same_interface_ports, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port_i, &channel_i));
        if (channel_i == channel) {
            *port_match_channel = port_i;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_channel_get(int unit, soc_port_t port, uint32 *channel) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *channel = PORT_INFO.channel;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_port_sw_db_channel_set(int unit, soc_port_t port, uint32 channel) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_CHANNEL, &channel, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_latch_down_get(int unit, soc_port_t port, int *latch_down) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *latch_down = PORT_PHY_INFO.latch_down;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_latch_down_set(int unit, soc_port_t port, int latch_down) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LATCH_DOWN, &latch_down, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_core_get(int unit, soc_port_t port, int* core)
{
     uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    *core = PORT_PHY_INFO.core;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_local_to_tm_port_set(int unit, soc_port_t port, uint32 tm_port) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    if ((tm_port != _SOC_DPP_PORT_INVALID) && (PORT_CORE_INFO.tm_to_local_port_map[tm_port] != SOC_MAX_NUM_PORTS)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Port %d: tm_port(%d) already in use"),port, tm_port));
    }

    if((tm_port != _SOC_DPP_PORT_INVALID)) {
        PORT_CORE_INFO.tm_to_local_port_map[tm_port] = port;
    } else {
        PORT_CORE_INFO.tm_to_local_port_map[PORT_INFO.tm_port] = SOC_MAX_NUM_PORTS;
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_TM_PORT, &tm_port, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_local_to_pp_port_set(int unit, soc_port_t port, uint32 pp_port) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    if ((pp_port != _SOC_DPP_PORT_INVALID) && (PORT_CORE_INFO.pp_to_local_port_map[pp_port] != SOC_MAX_NUM_PORTS)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("pp_port(%d) already in use"),pp_port));
    }

    if (pp_port != _SOC_DPP_PORT_INVALID) {
        PORT_CORE_INFO.pp_to_local_port_map[pp_port] = port;
    } else {
        PORT_CORE_INFO.pp_to_local_port_map[PORT_INFO.pp_port] = SOC_MAX_NUM_PORTS;
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_PORT, &pp_port, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_local_to_tm_port_get(int unit, soc_port_t port, uint32* tm_port, int* core) 
{

    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    *tm_port = PORT_INFO.tm_port;
    *core = PORT_PHY_INFO.core;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_local_to_pp_port_get(int unit, soc_port_t port, uint32* pp_port, int* core) {
    
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    if (PORT_INFO.pp_port == _SOC_DPP_PORT_INVALID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("PP Port for port %d is invalid"),port));
    }

    *pp_port = PORT_INFO.pp_port;
    *core = PORT_PHY_INFO.core;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_channelized_port_set(int unit, soc_port_t port, int is_channelized) 
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port %d"), port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_CHANNELIZED, &is_channelized, PORT_PHY);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_channelized_port_get(int unit, soc_port_t port, int *is_channelized) 
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    *is_channelized = PORT_PHY_INFO.is_channelized; 

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_tm_to_local_port_get(int unit, int core, uint32 tm_port, soc_port_t *port) {

    SOCDNX_INIT_FUNC_DEFS;

    if(core == MEM_BLOCK_ALL) {
        core = 0;
    }
    if(core < 0 || core >= SOC_DPP_DEFS_GET(unit, nof_cores)){
        return SOC_E_PARAM;
    }

    if (tm_port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tm_port %d"), tm_port));
    }

    *port = core_info[unit][core].tm_to_local_port_map[tm_port];

    if(*port == SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("tm_port %d invalid mapping"), tm_port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_pp_to_local_port_get(int unit, int core, uint32 pp_port, soc_port_t *port) 
{
    SOCDNX_INIT_FUNC_DEFS;

    if(core == MEM_BLOCK_ALL) {
        core = 0;
    }
    if(core < 0 || core >= SOC_DPP_DEFS_GET(unit, nof_cores)){
        return SOC_E_PARAM;
    }

    if (pp_port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid pp_port %d"), pp_port));
    }

    *port = core_info[unit][core].pp_to_local_port_map[pp_port];

    if(*port == SOC_MAX_NUM_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("pp_port %d invalid mapping"), pp_port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_pp_is_valid_get(int unit, int core, uint32 pp_port,  uint32 *is_valid)
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    if(core == MEM_BLOCK_ALL) {
        core = 0;
    }
    if(core < 0 || core >= SOC_DPP_DEFS_GET(unit, nof_cores)){
        return SOC_E_PARAM;
    }

    if (pp_port >= SOC_MAX_NUM_PORTS) {
        (*is_valid) = 0;
        SOC_EXIT;
    }

    port = core_info[unit][core].pp_to_local_port_map[pp_port];

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, is_valid));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_local_to_out_port_priority_set(int unit, soc_port_t port, uint32 nof_priorities) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PRIORITY_MODE, &nof_priorities, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_port_sw_db_local_to_out_port_priority_get(int unit, soc_port_t port, uint32* nof_priorities) 
{
    uint32 is_valid;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d invalid"),port));
    }

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PRIORITY_MODE, nof_priorities, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_port_sw_db_tm_port_to_out_port_priority_get(int unit, int core, uint32 tm_port, uint32* nof_priorities)
{
    soc_port_t port;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PRIORITY_MODE, nof_priorities, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_port_sw_db_pp_port_to_out_port_priority_get(int unit, int core, uint32 pp_port, uint32* nof_priorities)
{
    soc_port_t port;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &port));

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PRIORITY_MODE, nof_priorities, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_pp_port_to_base_q_pair_get(int unit,int core, uint32 pp_port,  uint32* base_q_pair)
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &port));

    *base_q_pair = PORT_INFO.base_q_pair;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_tm_port_to_base_q_pair_get(int unit, int core, uint32 tm_port, uint32* base_q_pair)
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));

    *base_q_pair = PORT_INFO.base_q_pair;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_mc_offset_set(int unit, soc_port_t port, uint32 mc_offset)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_MC_OFFSET, &mc_offset, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_egr_if_get(int unit, soc_port_t port, uint32* egr_if)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*egr_if) = PORT_INFO.egr_interface;

    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_egr_if_set(int unit, soc_port_t port, uint32 egr_if)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_EGR_INTERFACE, &egr_if, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_mc_offset_get(int unit, soc_port_t port, uint32* mc_offset)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*mc_offset) = PORT_INFO.multicast_offset;

    SOCDNX_FUNC_RETURN;
}

/*OTMH info*/
soc_error_t 
soc_port_sw_db_olif_ext_en_get(int unit, soc_port_t port, SOC_TMC_PORTS_FTMH_EXT_OUTLIF* outlif_ext_en)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*outlif_ext_en) = PORT_INFO.outlif_ext_en;

    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_olif_ext_en_set(int unit, soc_port_t port, SOC_TMC_PORTS_FTMH_EXT_OUTLIF outlif_ext_en)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_OLIF_EXT_EN, &outlif_ext_en, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_src_ext_en_get(int unit, soc_port_t port, int* src_ext_en)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*src_ext_en) = PORT_INFO.src_ext_en;

    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_src_ext_en_set(int unit, soc_port_t port, int src_ext_en)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_SRC_EXT_EN, &src_ext_en, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t
soc_port_sw_db_dst_ext_en_get(int unit, soc_port_t port, int* dst_ext_en)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*dst_ext_en) = PORT_INFO.dst_ext_en;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_dst_ext_en_set(int unit, soc_port_t port, int dst_ext_en)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_DST_EXT_EN, &dst_ext_en, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*Header info*/
soc_error_t
soc_port_sw_db_frst_hdr_sz_set(int unit, soc_port_t port, uint32 first_hdr_sz)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FRST_HDR_SZ, &first_hdr_sz, port);;
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_frst_hdr_sz_get(int unit, soc_port_t port, uint32* first_hdr_sz)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*first_hdr_sz) = PORT_INFO.first_header_size;

    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_hdr_type_out_set(int unit, soc_port_t port, SOC_TMC_PORT_HEADER_TYPE header_type_out)
{
    soc_error_t rv;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(port_sw_db_packet_tdm_check(unit, header_type_out));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    /* update port bitmaps */
    if(SOC_PORT_IF_OLP != interface && SOC_PORT_IF_OAMP != interface) {
        /* TDM port*/
        if ((header_type_out == SOC_TMC_PORT_HEADER_TYPE_TDM)
            || (header_type_out == SOC_TMC_PORT_HEADER_TYPE_TDM_RAW)) {
          PORT_SW_DB_PORT_ADD(tdm, port);
        }
    }
    /* update db */
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_HDR_T_OUT,
                                   &header_type_out, port);
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_hdr_type_out_get(int unit, soc_port_t port, SOC_TMC_PORT_HEADER_TYPE* header_type_out)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*header_type_out) = PORT_INFO.header_type_out;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_hdr_type_in_set(int unit, soc_port_t port, SOC_TMC_PORT_HEADER_TYPE header_type_in)
{
    soc_error_t rv;
    soc_port_if_t interface;

 
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(port_sw_db_packet_tdm_check(unit, header_type_in));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    /* update port bitmaps */
    if(SOC_PORT_IF_OLP != interface && SOC_PORT_IF_OAMP != interface) {
        if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_ETH) 
            || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW)
            || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP)
            || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP)
            || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)){
          PORT_SW_DB_PORT_ADD(ether, port);
        }  
        if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_STACKING) {
          PORT_SW_DB_PORT_ADD(st, port);
        }
    }
   
    /* update db */   
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_HDR_T_IN,
                                   &header_type_in, port);
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_port_sw_db_hdr_type_in_get(int unit, soc_port_t port, SOC_TMC_PORT_HEADER_TYPE* header_type_in)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*header_type_in) = PORT_INFO.header_type_in;

    SOCDNX_FUNC_RETURN;
}

/*PP port info*/

soc_error_t
soc_port_sw_db_pp_port_flags_set(int unit, soc_port_t port, uint32 pp_flags)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_FLAGS, &pp_flags, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_pp_port_flags_add(int unit, soc_port_t port, uint32 flag)
{
    int pp_flags;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    pp_flags = PORT_INFO.pp_flags | flag;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_FLAGS, &pp_flags, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_pp_port_flags_rmv(int unit, soc_port_t port, uint32 flag)
{
    int pp_flags;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    pp_flags = PORT_INFO.pp_flags & (~flag);

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_FLAGS, &pp_flags, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_port_sw_db_pp_port_flags_get(int unit, soc_port_t port, uint32* flag)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*flag) = PORT_INFO.pp_flags;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_fc_type_set(int unit, soc_port_t port, SOC_TMC_PORTS_FC_TYPE fc_type)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FC_TYPE, &fc_type, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_fc_type_get(int unit, soc_port_t port, SOC_TMC_PORTS_FC_TYPE* fc_type)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*fc_type) = PORT_INFO.fc_type;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_mirror_profile_set(int unit, soc_port_t port, uint32 mirror_profile)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_MIRROR_PROFILE, &mirror_profile, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_mirror_profile_get(int unit, soc_port_t port, uint32* mirror_profile)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*mirror_profile) = PORT_INFO.mirror_profile;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_tm_src_syst_port_ext_present_set(int unit, soc_port_t port, int is_tm_src_syst_port_ext_present)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_SRC_SYST_PORT_EXT_PRESENT, &is_tm_src_syst_port_ext_present, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_tm_src_syst_port_ext_present_get(int unit, soc_port_t port, int* is_tm_src_syst_port_ext_present)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*is_tm_src_syst_port_ext_present) = PORT_INFO.is_tm_src_syst_port_ext_present;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_tm_pph_present_en_set(int unit, soc_port_t port, int is_tm_pph_present_enabled)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_PPH_PRESENT_ENABLED, &is_tm_pph_present_enabled, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_tm_pph_present_en_get(int unit, soc_port_t port, int* is_tm_pph_present_enabled)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*is_tm_pph_present_enabled) = PORT_INFO.is_tm_pph_present_enabled;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_stag_enabled_set(int unit, soc_port_t port, int is_stag_enabled)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_STAG_ENABLED, &is_stag_enabled, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_stag_enabled_get(int unit, soc_port_t port, int* is_stag_enabled)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*is_stag_enabled) = PORT_INFO.is_stag_enabled;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_snoop_enabled_set(int unit, soc_port_t port, int is_snoop_enabled)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_SNOOP_ENABLED, &is_snoop_enabled, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_snoop_en_get(int unit, soc_port_t port, int* is_snoop_enabled)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*is_snoop_enabled) = PORT_INFO.is_snoop_enabled;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_tm_ing_shaping_enabled_set(int unit, soc_port_t port, int is_tm_ing_shaping_enabled)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv= SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_ING_SHAPING_ENABLED, &is_tm_ing_shaping_enabled, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_is_tm_ing_shaping_en_get(int unit, soc_port_t port, int* is_tm_ing_shaping_enabled)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*is_tm_ing_shaping_enabled) = PORT_INFO.is_tm_ing_shaping_enabled;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_shaper_mode_set(int unit, soc_port_t port, SOC_TMC_EGR_PORT_SHAPER_MODE shaper_mode)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_SHAPER_MODE, &shaper_mode, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_shaper_mode_get(int unit, soc_port_t port, SOC_TMC_EGR_PORT_SHAPER_MODE* shaper_mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*shaper_mode) = PORT_INFO.shaper_mode;

    SOCDNX_FUNC_RETURN;
}

/*Peer tm domain*/
soc_error_t
soc_port_sw_db_peer_tm_domain_set(int unit, soc_port_t port, uint32 peer_tm_domain)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PEER_TM_DOMAIN, &peer_tm_domain, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_peer_tm_domain_get(int unit, soc_port_t port, uint32* peer_tm_domain)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*peer_tm_domain) = PORT_INFO.peer_tm_domain;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_base_q_pair_set(int unit, soc_port_t port, uint32 base_q_pair)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_BASE_Q_PAIR, &base_q_pair, port);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_port_sw_db_base_q_pair_get(int unit, soc_port_t port, uint32* base_q_pair)
{
    SOCDNX_INIT_FUNC_DEFS;

    (*base_q_pair) = PORT_INFO.base_q_pair;

    SOCDNX_FUNC_RETURN;
}


/*****************************/
/****    SW DB Print      ****/
/*****************************/
typedef struct port_sw_db_print_line_s {
    int unit;
    int columns_in_line;
    int port_columns[SW_DB_PRINT_MAX_COLUMNS_IN_LINE];
    int ports[SW_DB_PRINT_MAX_COLUMNS_IN_LINE];
    int num_of_ports;
}port_sw_db_print_line_t;

typedef struct soc_phy_to_logical_ports_str {
    char ports[SOC_MAX_NUM_PORTS][SW_DB_PRINT_COLUMN_LENGTH];
} soc_phy_to_logical_ports_str_t;

/* print port_sw_db funcs*/

STATIC soc_error_t
soc_port_sw_db_create_centralized_string(int unit, char* destination, char* source, int window_size)
{
    int len, i;
    SOCDNX_INIT_FUNC_DEFS;
    
    sal_strncat(destination, "|", 1);
    len = sal_strlen(source);
    len = window_size - len;
    for (i = 0; i < (len/2); i++) {
        sal_strncat(destination, " ", 1);
    }
    sal_strncat(destination, source, len);
    for (i = 0; i < (len/2); i++) {
        sal_strncat(destination, " ", 1);
    }
    if ((len%2)) {
        sal_strncat(destination, " ", 1);
    }

    SOCDNX_FUNC_RETURN    
}




/**
 * get str of the ranges of physical ports whos first physical 
 * port is first_phy_port 
 * @param unit 
 * @param first_phy_port
 * @param ports_str - the output e.g: "1-12 24-28" (1 base port numbers)
 * 
 */
STATIC soc_error_t
_soc_port_sw_db_ports_str_get(int unit, int first_phy_port, char *ports_str)
{
    int port_i;
    int range_begin = -1;
    int last_added = -1;
    char str[SW_DB_PRINT_COLUMN_LENGTH];

    PBMP_ITER(phy_ports_info[unit][first_phy_port].phy_ports, port_i){
        /*first time handle*/
        if(last_added == -1){
            last_added = port_i;
            range_begin = port_i;
            sal_sprintf(ports_str, "%d", last_added);
            continue;
        }
        /*continue of range*/
        if((last_added == port_i - 1)){
            last_added = port_i;
            continue;
        }
        /*avoid overflow*/
        if(sal_strlen(ports_str) > 100){
            return SOC_E_FAIL;
        }
        /*the range that ended was not 1 item range*/
        if(last_added != range_begin){
            sal_sprintf(str, " - %d", last_added);
            sal_strncat(ports_str, str, sal_strlen(str));
        }
        range_begin = port_i;
        last_added = port_i;
        sal_sprintf(str, ", %d", last_added);
        sal_strncat(ports_str, str, sal_strlen(str));
    }
    /*close the last range*/
    if(last_added != range_begin){
        sal_sprintf(str, " - %d", last_added);
        sal_strncat(ports_str, str, sal_strlen(str));
    }
    /*handle no ports in list e.g: OLP ERP*/
    if(last_added == -1){
        sal_sprintf(str, "%d", first_phy_port);
        sal_sprintf(ports_str, str);
    }
    return SOC_E_NONE;
}

/**
 * get the flags of the logical port of the master channel that 
 * its first phyical port is  first_phy_port and 
 * 
 * @param unit 
 * @param first_phy_port 
 * @param str 
 * 
 * @return STATIC soc_error_t 
 */
STATIC soc_error_t
_soc_port_sw_db_flags_str_get(int unit, int first_phy_port, char *str)
{
    int port_flags;

    str[0] = 0;
    port_flags = logical_ports_info[unit][phy_ports_info[unit][first_phy_port].master_port].flags;
    if (port_flags & SOC_PORT_FLAGS_STAT_INTERFACE) {
        sal_strncat(str, "STAT", 149);
    }
    if (port_flags & SOC_PORT_FLAGS_NETWORK_INTERFACE) {
        sal_strncat(str, " NIF", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_PON_INTERFACE) {
        sal_strncat(str, " PON", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_VIRTUAL_RCY_INTERFACE) {
        sal_strncat(str, " VIRT RCY", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_DISABLED) {
        sal_strncat(str, " DIS", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_ELK) {
        sal_strncat(str, " ELK", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_XGS_MAC_EXT) {
        sal_strncat(str, " XGS MAC EXT", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_FIBER) {
        sal_strncat(str, " FIBER", 149 - sal_strlen(str));
    }
    if (port_flags & SOC_PORT_FLAGS_SCRAMBLER) {
        sal_strncat(str, " SCRAMBLER", 149 - sal_strlen(str));
    }
    return SOC_E_NONE;
}

/**
 * get the first col of the port_db table
 * 
 * @param line_number - the line number in the table
 * @param str - the output
 * 
 * @return STATIC soc_error_t 
 */
STATIC soc_error_t
_soc_port_sw_db_first_col_str_get(int line_number, char *str)
{
    switch(line_number){
    case 0:
    case 2:  /*fall through*/
    case 5:  /*fall through*/
    case 11: /*fall through*/
        sal_strncpy(str, "                      ", 150);
        break;
    case 1:
        sal_strncpy(str, "       Physical Port: ", 150);
        break;
    case 3:
        sal_strncpy(str, "           Interface: ", 150);
        break;
    case 4:
        sal_strncpy(str, "               Speed: ", 150);
        break;
    case 6:
        sal_strncpy(str, "LogicalPort, Channel: ", 150);
        break;
    case 7:
        sal_strncpy(str, "         Initialized: ", 150);
        break;
    case 8:
        sal_strncpy(str, "               Flags: ", 150);
        break;
    case 9:
        sal_strncpy(str, "          Latch Down: ", 150);
        break;
    case 10:
        sal_strncpy(str, "                Core: ", 150);
        break;
    }
    return SOC_E_NONE;
}


STATIC soc_error_t
_soc_port_sw_db_logical_ports_str_get(
   int unit,
   int port,
   int entry_id,
   soc_phy_to_logical_ports_str_t *logical_ports,
   int logical_port_count[SOC_MAX_NUM_PORTS],
   char *str)
{
    if (entry_id >= logical_port_count[port]){
       sal_strncpy(str, " ", 150);
    }
    else{
        sal_strncpy(str, logical_ports[port].ports[entry_id], 150);
    }
    return SOC_E_NONE;
}

/**
 * print a line of the port_db table
 * 
 * @param unit 
 * @param table_line - struct with information about the ports 
 *                     to print in the current line of the table
 * @param logical_ports - physical to logical ports 
 *                      mapping(array that in the
 *                      logical_ports[phy_port] -  is array of
 *                      strings: "logical_port , channel"
 * @param logical_port_count  - logical_port_count[phy_port] = 
 *                            the number of strings of
 *                            logical_ports for phy_port.
 * 
 * @return STATIC soc_error_t 
 */
STATIC soc_error_t
_soc_port_sw_db_line_print(
   int unit,
   port_sw_db_print_line_t *table_line,
   soc_phy_to_logical_ports_str_t *logical_ports,
    int logical_port_count[SOC_MAX_NUM_PORTS])
{
    char* interface_mode[] = SOC_PORT_IF_NAMES_INITIALIZER;
    int max_logical_ports = 0;
    int is_init;
    char line[200];
    char str[200];
    int i,j,k, logical_ports_count ;

    SOCDNX_INIT_FUNC_DEFS;
    for(i = 0 ; i < table_line->num_of_ports; i ++){
        if(logical_port_count[table_line->ports[i]] > max_logical_ports){
            max_logical_ports = logical_port_count[table_line->ports[i]];
        }
    }
    logical_ports_count = 0;
    for(j = 0; j < 12; ){
        _soc_port_sw_db_first_col_str_get(j, line);
        if(logical_ports_count != 0){
            sal_strncpy(line, "                      ", sizeof(line));
        }
        for(i = 0 ; i < table_line->num_of_ports; i ++){
            switch(j){
            case 0:
            case 11: /* fall through*/
                break;
            case 1:
                SOCDNX_IF_ERR_EXIT(_soc_port_sw_db_ports_str_get(unit, table_line->ports[i], str));
                break;
            case 2:
            case 5: /*fall through*/
                sal_strncpy(str, " ", sizeof(str));
                break;
            case 3:
                sal_strncpy(str, interface_mode[phy_ports_info[unit][table_line->ports[i]].interface_type], sizeof(str));
                break;
            case 4:
                sal_itoa(str, phy_ports_info[unit][table_line->ports[i]].speed, 10, 0, 0);
                break;
            case 6:
                _soc_port_sw_db_logical_ports_str_get(unit, table_line->ports[i], logical_ports_count, logical_ports, logical_port_count, str);
                break;
            case 7:
                is_init = phy_ports_info[unit][table_line->ports[i]].initialized;
                if (!is_init){
                    sal_strncpy(str, "Uninitialized", sizeof(str));
                } 
                break;
            case 8:
                _soc_port_sw_db_flags_str_get(unit, table_line->ports[i], str);
                break;
            case 9:
                if (phy_ports_info[unit][table_line->ports[i]].latch_down) {
                    sal_strncpy(str, "LatchWasDown", sizeof(str));
                }
                break;
            case 10:
                sal_itoa(str, phy_ports_info[unit][table_line->ports[i]].core, 10, 0, 0);
                break;
            }
            if(j == 0 || (j == 11) ){
                for(k = 0 ; k < table_line->port_columns[i]; k++){
                    sal_strncat(line, "-------------------------", sizeof(line) - sal_strlen(line) - 1);
                }
            }
            else{
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_create_centralized_string(unit, line, str, ((SW_DB_PRINT_COLUMN_LENGTH+2)*table_line->port_columns[i]-(table_line->port_columns[i]+1))));
            }
            str[0] = 0;            
        }
        if(j == 0 || (j == 11) ){
            LOG_CLI((BSL_META_U(unit,
                                "%s-\n"), line));
        }
        else{
            LOG_CLI((BSL_META_U(unit,
                                "%s|\n"), line));
        }
        if (j == 6){
            logical_ports_count++;
            if(logical_ports_count >= max_logical_ports){
                j++;
                logical_ports_count = 0;
            }
        }
        else{
            j++;
        }
        line[0] = 0;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n\n")));
    exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * add port to the table line structure 
 *  
 * @param table_line 
 * @param port 
 * @param nof_columns 
 * 
 * @return STATIC soc_error_t 
 */
STATIC soc_error_t
_soc_port_sw_db_line_port_add(port_sw_db_print_line_t *table_line, int port, int nof_columns){
    table_line->ports[table_line->num_of_ports] = port;
    table_line->port_columns[table_line->num_of_ports] = nof_columns;
    table_line->columns_in_line += nof_columns;
    table_line->num_of_ports++;
    return SOC_E_NONE;
}


/**
 * add port to the table. if the the add of the port cause 
 * overflow (exceeds from  SW_DB_PRINT_MAX_COLUMNS_IN_LINE) we 
 * print the current line and add port to a new line 
 * 
 * @param unit 
 * @param port - first physical port
 * @param table_line - struct with information about the ports 
 *                     to print in the current line of the table
 * @param logical_ports - physical to logical ports 
 *                      mapping(array that in the
 *                      logical_ports[phy_port] -  is array of
 *                      strings: "logical_port , channel"
 * @param logical_port_count  - logical_port_count[phy_port] = 
 *                            the number of strings of
 *                            logical_ports for phy_port.
 * 
 * @return STATIC soc_error_t 
 */
STATIC soc_error_t
_soc_port_sw_db_table_entry_add(int unit, int port, port_sw_db_print_line_t *table_line,
    soc_phy_to_logical_ports_str_t *logical_ports,
    int logical_port_count[SOC_MAX_NUM_PORTS])
{
    int nof_columns;
    SOCDNX_INIT_FUNC_DEFS;

     nof_columns = 1;

    if(table_line->columns_in_line + nof_columns > SW_DB_PRINT_MAX_COLUMNS_IN_LINE){
        SOCDNX_IF_ERR_EXIT(_soc_port_sw_db_line_print(unit, table_line, logical_ports, logical_port_count ));
        sal_memset(table_line , 0 , sizeof(table_line[0]));
    }
    _soc_port_sw_db_line_port_add(table_line, port, nof_columns);
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_port_sw_db_print(int unit, uint32 flags)
{
    int port;
    int  i, j; 
    char port_str[SW_DB_PRINT_COLUMN_LENGTH*2];
    char *logical_ports_str[SOC_MAX_NUM_PORTS];
    soc_phy_to_logical_ports_str_t *logical_ports = NULL;
    int logical_port_count[SOC_MAX_NUM_PORTS];
    port_sw_db_print_line_t line_struct;
    uint32 is_init;
    SOCDNX_INIT_FUNC_DEFS;
    
       
    sal_memset(&line_struct, 0 , sizeof(line_struct));
    /* prepare logical ports and channels*/
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        logical_port_count[i] = 0;
        logical_ports_str[i] = sal_alloc((SW_DB_PRINT_COLUMN_LENGTH+2)*SOC_MAX_NUM_PORTS, "soc_port_sw_db_print.logical_ports_str");
        if(!logical_ports_str[i]){
            LOG_CLI((BSL_META_U(unit,
                                "Memory allocation failure\n")));
            for(j = 0; j < i ; ++j) {
                sal_free(logical_ports_str[j]);
            }
            return SOC_E_MEMORY;
        }
        logical_ports_str[i][0] = 0;
        if (i > 0) {
            sal_strncat(logical_ports_str[i],"                      ", (SW_DB_PRINT_COLUMN_LENGTH+2)*SOC_MAX_NUM_PORTS - sal_strlen(logical_ports_str[i]) - 1);
        }
    }

    logical_ports = sal_alloc(sizeof(soc_phy_to_logical_ports_str_t) * SOC_MAX_NUM_PORTS, "soc_port_sw_db_print.logical_ports");
    if(!logical_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Memory allocation failure")));
    }
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (PORT_INFO.valid) {
            sal_snprintf(port_str,SW_DB_PRINT_COLUMN_LENGTH, "%d , %d", port, PORT_INFO.channel);
            sal_strncpy(logical_ports[PORT_INFO.first_phy_port].ports[logical_port_count[PORT_INFO.first_phy_port]], port_str, SW_DB_PRINT_COLUMN_LENGTH);
            logical_port_count[PORT_INFO.first_phy_port]++;
            port_str[0] = 0;
        } 
    }
    
    
    for (port = 0; port < SOC_MAX_NUM_PORTS; port ++) {
        is_init = phy_ports_info[unit][port].initialized;
        if (!is_init) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(_soc_port_sw_db_table_entry_add(unit, port, &line_struct, logical_ports, logical_port_count));
    }
    /*print last line*/
    SOCDNX_IF_ERR_EXIT(_soc_port_sw_db_line_print(unit, &line_struct, logical_ports, logical_port_count ));


exit:
    for(j = 0; j < SOC_MAX_NUM_PORTS ; ++j) {
        sal_free(logical_ports_str[j]);
    }
    
    if(logical_ports) {
        sal_free(logical_ports);
    }

    SOCDNX_FUNC_RETURN;
}

/*****************************/
/****  Snapshot Logic     ****/
/*****************************/

/*port sw data base snapshot support*/
/*Required for warmboot support - for any new information which added to the data base - support should be added*/
int soc_port_sw_db_snapshot_valid[SOC_MAX_NUM_DEVICES];
soc_phy_port_sw_db_t phy_ports_info_snapshot[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
soc_logical_port_sw_db_t logical_ports_info_snapshot[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];

#define PORT_PHY_INFO_SS (phy_ports_info_snapshot[unit][logical_ports_info_snapshot[unit][port].first_phy_port])
#define PORT_INFO_SS (logical_ports_info_snapshot[unit][port])

/*
 * Function:
 *      soc_port_sw_db_snapshot_take
 * Purpose:
 *      Take a snapshot of port sw data base.
 *      Restore according to this snap shot can be done by using soc_port_sw_db_snapshot_restore.
 *      Every new information that added to port sw data base should be added to the snapshot support
 * Parameters:
 *      unit                - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t
soc_port_sw_db_snapshot_take(int unit) {
    SOCDNX_INIT_FUNC_DEFS;

    soc_port_sw_db_snapshot_valid[unit] = 1;
    sal_memcpy(phy_ports_info_snapshot[unit], phy_ports_info[unit], sizeof(soc_phy_port_sw_db_t) * SOC_MAX_NUM_PORTS);
    sal_memcpy(logical_ports_info_snapshot[unit], logical_ports_info[unit], sizeof(soc_logical_port_sw_db_t) * SOC_MAX_NUM_PORTS);
    
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_port_sw_db_snapshot_restore
 * Purpose:
 *      Restore port sw data base.
 *      Restore according to this snap shot that was taken soc_port_sw_db_snapshot_take.
 *      Assumes soc_port_sw_db_snapshot_take already called
 *      Every new information that added to port sw data base should be added to the snapshot support
 * Parameters:
 *      unit                - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t
soc_port_sw_db_snapshot_restore(int unit) {
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    if (!soc_port_sw_db_snapshot_valid[unit]) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Port sw db snapshot was not taken")));
    }

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {

        if (PORT_INFO.valid) {

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_remove(unit, port));

        }
    }

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (PORT_INFO_SS.valid) {

            /*Add valid ports info*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_add(unit, 
                                                        PORT_PHY_INFO_SS.core,
                                                        port,
                                                        PORT_INFO_SS.channel,
                                                        PORT_INFO_SS.flags,
                                                        PORT_PHY_INFO_SS.interface_type,
                                                        PORT_PHY_INFO_SS.phy_ports));

            /*mark port as initialized*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 1));

            /*speed set*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, PORT_PHY_INFO_SS.speed));

            /*latch down support*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, PORT_PHY_INFO_SS.latch_down));

            /*runt pad*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port, PORT_PHY_INFO_SS.runt_pad));

            /*encap mode*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_set(unit, port, PORT_PHY_INFO_SS.encap_mode));

            /*is chnnelized*/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_set(unit, port, PORT_PHY_INFO_SS.is_channelized));

            /* cal mode */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_set(unit, port, PORT_PHY_INFO_SS.is_single_cal_mode));

            /* high priority cal */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, port, PORT_PHY_INFO_SS.high_pirority_cal));

            /* low priority cal */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, port, PORT_PHY_INFO_SS.low_pirority_cal));

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, port, PORT_INFO_SS.tm_port));                  
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, port, PORT_INFO_SS.pp_port));                  
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_set(unit, port, PORT_INFO_SS.priority_mode));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_mc_offset_set(unit, port, PORT_INFO_SS.multicast_offset));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_port_flags_set(unit,port, PORT_INFO_SS.pp_flags));                                         
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_fc_type_set(unit, port, PORT_INFO_SS.fc_type));                              
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_mirror_profile_set(unit, port, PORT_INFO_SS.mirror_profile));                                  
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_tm_src_syst_port_ext_present_set(unit, port, PORT_INFO_SS.is_tm_src_syst_port_ext_present));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_tm_pph_present_en_set(unit, port, PORT_INFO_SS.is_tm_pph_present_enabled));                    
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_stag_enabled_set(unit, port, PORT_INFO_SS.is_stag_enabled));                                
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_snoop_enabled_set(unit, port, PORT_INFO_SS.is_snoop_enabled));                              
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_tm_ing_shaping_enabled_set(unit, port, PORT_INFO_SS.is_tm_ing_shaping_enabled)); 
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_olif_ext_en_set(unit, port, PORT_INFO_SS.outlif_ext_en));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_src_ext_en_set(unit, port, PORT_INFO_SS.src_ext_en));                              
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_dst_ext_en_set(unit, port, PORT_INFO_SS.dst_ext_en)); 
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit, port, PORT_INFO_SS.header_type_out));                                                                      
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_set(unit, port, PORT_INFO_SS.header_type_in));                                           
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_frst_hdr_sz_set(unit, port, PORT_INFO_SS.first_header_size));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_peer_tm_domain_set(unit, port, PORT_INFO_SS.peer_tm_domain));                                                  
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_shaper_mode_set(unit, port, PORT_INFO_SS.shaper_mode));                                                                                    
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_set(unit, port, PORT_INFO_SS.base_q_pair));        
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_set(unit, port, PORT_INFO_SS.egr_interface));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port, PORT_INFO_SS.protocol_offset));                                                 
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_port_sw_db_snapshot_restore
 * Purpose:
 *      Init snapshot tool
 *      Every new information that added to port sw data base should be added to the snapshot support
 * Parameters:
 *      unit                - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_port_sw_db_snapshot_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    soc_port_sw_db_snapshot_valid[unit] = 0;


    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME


