/*
 *         
 * $Id:$
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
 *     
 *
 */
#include <soc/portmod/portmod.h>
#include <appl/diag/bslmgmt.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/bsldnx.h>
#include <shared/bsl.h>
#include <appl/portmod/portmod_reg_access.h>

#define MAX_PHYS  (140)
#define MAX_PORTS (256)
#define MAX_PMS (10)



/*this struct passed to every read/write SerDes register and is used just by the user code(not used by portmod or phymod)*/
typedef struct phymod_user_data_example_s{
    int unit;
}phymod_user_data_example_t;


/*read SerDes register function*/
STATIC
int bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    phymod_user_data_example_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = user_acc;
    
    /*place the read register function here*/
    *val = 0 ;
    LOG_VERBOSE(BSL_LS_SOC_PHYMOD,
                (BSL_META_U(user_data->unit,
                            "phy reg read: core_addr=0x0%8x, reg 0x%08x = 0x%04x\n"),
                            core_addr, reg_addr, *val));
    return SOC_E_NONE;
}

/*write SerDes register function*/
STATIC
int bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    phymod_user_data_example_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = user_acc;

    LOG_VERBOSE(BSL_LS_SOC_PHYMOD,
                (BSL_META_U(user_data->unit,
                            "phy reg write: core_addr=0x0%8x, reg 0x%08x = 0x%04x\n"),
                            core_addr, reg_addr, val));
    return SOC_E_NONE;
}


phymod_bus_t phy_bus = {
    "phy_bus",
    bus_read,
    bus_write,
    NULL, /*mutex take*/
    NULL, /*mutex release*/
    0
};

int portmod_pm12x10_add(int unit, soc_pbmp_t phys, int core_addrs[3], int first_blk_id, void **user_data)
{
    int rv;
    int i;
    portmod_pm_create_info_t pm_info;
    phymod_user_data_example_t *pm4x10_user_data = NULL;
    portmod_pm4x10_create_info_t *pm4x10_info = NULL;
    phymod_access_t *access = NULL;


    portmod_pm_create_info_t_init(unit, &pm_info);
    /*PM add*/
    pm_info.type = portmodDispatchTypePm12x10;
    pm_info.first_blk_id = first_blk_id; 
    /*PM phys*/
    SOC_PBMP_ASSIGN(pm_info.phys, phys);
    /*PM phys polarity*/
    *user_data = NULL;
    pm4x10_user_data = sal_alloc(sizeof(phymod_user_data_example_t)*3, "phymod user data");
    *user_data = pm4x10_user_data;
    if(*user_data == NULL){
        return SOC_E_MEMORY;
    }

    for( i = 0 ; i < 3 ; i++)
    {
        pm4x10_info = &pm_info.pm_specific_info.pm12x10.pm4x10_infos[i];    
        /*PM phys polarity*/
        pm4x10_info->polarity[0].rx_polarity = 0;
        pm4x10_info->polarity[0].tx_polarity = 0;
        pm4x10_info->polarity[1].rx_polarity = 1;
        pm4x10_info->polarity[1].tx_polarity = 1;
        pm4x10_info->polarity[2].rx_polarity = 1;
        pm4x10_info->polarity[2].tx_polarity = 1;
        pm4x10_info->polarity[3].rx_polarity = 0;
        pm4x10_info->polarity[3].tx_polarity = 0;

        /*PM lane map*/
        pm4x10_info->lane_map.num_of_lanes = 4;
        pm4x10_info->lane_map.lane_map_rx[0] = 0;
        pm4x10_info->lane_map.lane_map_rx[1] = 1;
        pm4x10_info->lane_map.lane_map_rx[2] = 2;
        pm4x10_info->lane_map.lane_map_rx[3] = 3;
        pm4x10_info->lane_map.lane_map_tx[0] = 0;
        pm4x10_info->lane_map.lane_map_tx[1] = 1;
        pm4x10_info->lane_map.lane_map_tx[2] = 2;
        pm4x10_info->lane_map.lane_map_tx[3] = 3;

        pm4x10_info->ref_clk = phymodRefClk156Mhz;
        pm4x10_info->fw_load_method = phymodFirmwareLoadMethodExternal;

        /*phy access*/
        pm4x10_user_data[i].unit = unit;
        access  = &(pm4x10_info->access);
        PHYMOD_ACC_BUS(access) = &phy_bus;
        PHYMOD_ACC_ADDR(access) = core_addrs[i]; /*core address*/
        PHYMOD_ACC_USER_ACC(access) = &pm4x10_user_data[i]; 
    }

    rv = portmod_port_macro_add(unit, &pm_info);
    if((rv < 0) && (*user_data != NULL)){
        sal_free(*user_data);
        *user_data = NULL;
         return rv;
    }
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "pm 4x25 added!\n")));
    return rv;
}



int portmod_pm4x25_add(int unit, soc_pbmp_t phys, int core_addr, int first_blk_id, void **user_data)
{
    int rv;
    portmod_pm_create_info_t pm_info;
    portmod_default_user_access_t *pm4x25_user_data = NULL;
    portmod_pm4x25_create_info_t *pm4x25_info = NULL;
    phymod_access_t *access = NULL;


    portmod_pm_create_info_t_init(unit, &pm_info);
    /*PM add*/
    pm_info.type = portmodDispatchTypePm4x25;
    pm_info.first_blk_id = first_blk_id; 
    /*PM phys*/
    SOC_PBMP_ASSIGN(pm_info.phys, phys);
    /*PM phys polarity*/
    *user_data = NULL;
    pm4x25_user_data = sal_alloc(sizeof(portmod_default_user_access_t), "phymod user data");
    *user_data = pm4x25_user_data;
    if(*user_data == NULL){
        return SOC_E_MEMORY;
    }
    pm4x25_info = &pm_info.pm_specific_info.pm4x25;
    pm4x25_info->polarity[0].rx_polarity = 0;
    pm4x25_info->polarity[0].tx_polarity = 0;
    pm4x25_info->polarity[1].rx_polarity = 1;
    pm4x25_info->polarity[1].tx_polarity = 1;
    pm4x25_info->polarity[2].rx_polarity = 1;
    pm4x25_info->polarity[2].tx_polarity = 1;
    pm4x25_info->polarity[3].rx_polarity = 0;
    pm4x25_info->polarity[3].tx_polarity = 0;

    /*PM lane map*/
    pm4x25_info->lane_map.num_of_lanes = 4;
    pm4x25_info->lane_map.lane_map_rx[0] = 0;
    pm4x25_info->lane_map.lane_map_rx[1] = 1;
    pm4x25_info->lane_map.lane_map_rx[2] = 2;
    pm4x25_info->lane_map.lane_map_rx[3] = 3;
    pm4x25_info->lane_map.lane_map_tx[0] = 0;
    pm4x25_info->lane_map.lane_map_tx[1] = 1;
    pm4x25_info->lane_map.lane_map_tx[2] = 2;
    pm4x25_info->lane_map.lane_map_tx[3] = 3;

    pm4x25_info->ref_clk = phymodRefClk156Mhz;
    pm4x25_info->fw_load_method = phymodFirmwareLoadMethodExternal;
    pm4x25_info->in_pm_12x10 = FALSE;
    /*phy access*/
    pm4x25_user_data->blk_id = first_blk_id;
    pm4x25_user_data->mutex = NULL;
    pm4x25_user_data->unit = unit;

    access  = &(pm4x25_info->access);
    PHYMOD_ACC_BUS(access) = NULL;
    PHYMOD_ACC_ADDR(access) = core_addr; /*core address*/
    PHYMOD_ACC_USER_ACC(access) = pm4x25_user_data; 

    rv = portmod_port_macro_add(unit, &pm_info);
    if((rv < 0) && (*user_data != NULL)){
        sal_free(*user_data);
        *user_data = NULL;
        return rv;
    }
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "pm 12x10 added!\n")));
    return rv;
}


int portmod_appl_init(int unit, void **user_data){
    int rv = 0;
    int i = 0;
    int core_addrs[]= {0x80, 0x84, 0x88};
    portmod_pm_instances_t pm_types_and_instances[] = {{portmodDispatchTypePm12x10, 1},
    {portmodDispatchTypePm4x25, 1},};
    soc_pbmp_t phys;

    rv = portmod_create(unit, MAX_PORTS, MAX_PHYS, 2, pm_types_and_instances);
    if (rv < 0){
        return rv;
    }

    /*12x10 add*/
    SOC_PBMP_CLEAR(phys);
    for(i = 0 ; i < 12 ; i++ ){
        SOC_PBMP_PORT_ADD(phys, i);
    }
    rv = portmod_pm12x10_add(unit, phys, core_addrs, 0, &user_data[0]);
    if (rv < 0){
        return rv;
    }

    SOC_PBMP_CLEAR(phys);
    for(i = 12 ; i < 16 ; i++ ){
        SOC_PBMP_PORT_ADD(phys, i);
    }
    rv = portmod_pm4x25_add(unit, phys , 0x8e, 5, &user_data[1]);

    return rv;
}

int
portmod_appl_deinit(int unit, void **user_data)
{
    int i = 0;
    int rv;

    rv  = portmod_destroy(unit);
    for( i = 0 ; i < MAX_PMS; i++){
        if(user_data[i]){
            sal_free(user_data[i]);
        }
    }
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "portmod destroyed!\n")));
    return rv;
}


int
portmod_appl_add_ports(int unit){
    int port = 5;
    int port2 = 6;
    int rv = 0;
    portmod_port_add_info_t add_info;


    /*3 is the PM 4x25 block in the PM12X10*/
    rv = portmod_reg_access_port_bindex_set(unit, port, 3, 0);
     rv = portmod_reg_access_port_bindex_set(unit, port2, 5, 0);
    if(rv < 0){
        return rv;
    }

    portmod_port_add_info_t_init(unit, &add_info);
    add_info.interface_config.speed = 1000000;
    add_info.interface_config.interface = SOC_PORT_IF_CAUI;
    add_info.interface_config.interface_modes = 0;
    SOC_PBMP_PORT_ADD(add_info.phys, 1);
    SOC_PBMP_PORT_ADD(add_info.phys, 2);
    SOC_PBMP_PORT_ADD(add_info.phys, 3);
    SOC_PBMP_PORT_ADD(add_info.phys, 4);
    SOC_PBMP_PORT_ADD(add_info.phys, 5);
    SOC_PBMP_PORT_ADD(add_info.phys, 6);
    SOC_PBMP_PORT_ADD(add_info.phys, 7);
    SOC_PBMP_PORT_ADD(add_info.phys, 8);
    SOC_PBMP_PORT_ADD(add_info.phys, 9);
    SOC_PBMP_PORT_ADD(add_info.phys, 10);
    rv = portmod_port_add(unit, port, &add_info);
    if(rv < 0){
        return rv;
    }
    rv = portmod_port_enable_set(unit, port, 0 , 1);
    if(rv < 0){
        return rv;
    }

    portmod_port_add_info_t_init(unit, &add_info);
    add_info.interface_config.speed = 1000000;
    add_info.interface_config.interface = SOC_PORT_IF_CAUI;
    add_info.interface_config.interface_modes = 0;
    SOC_PBMP_PORT_ADD(add_info.phys, 12);
    SOC_PBMP_PORT_ADD(add_info.phys, 13);
    SOC_PBMP_PORT_ADD(add_info.phys, 14);
    SOC_PBMP_PORT_ADD(add_info.phys, 15);

    rv = portmod_port_add(unit, port2, &add_info);
    if(rv < 0){
        return rv;
    }
    rv = portmod_port_enable_set(unit, port2, 0 , 1);
    return rv;

}

int
portmod_appl_main(void)
{
    soc_control_t        *soc;
    void *user_data[SOC_MAX_NUM_DEVICES][MAX_PMS];
    int rv = 0;
    int unit = 0;

    /*logging mechanisem init*/
    rv = bslmgmt_init();
    if(rv < 0){
        return rv;
    }
    rv = bsldnx_mgmt_init(unit);
    if(rv < 0){
        return rv;
    }
    /*logging settings*/
    bslenable_set(bslLayerAppl, bslSourcePort, bslSeverityNormal);
    bslenable_set(bslLayerSoc, bslSourcePort, bslSeverityNormal);
    bslenable_set(bslLayerSoc, bslSourcePhymod, bslSeverityVerbose);

    sal_memset(user_data, 0 , sizeof(user_data));

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {  
        soc = sal_alloc(sizeof (soc_control_t), "soc_control");
        if (soc == NULL) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "failed to allocate soc_control\n")));
            return SOC_E_MEMORY;
        }
        sal_memset(soc, 0, sizeof (soc_control_t));
        SOC_CONTROL(unit) = soc;
    }

    /* Register access init */
    rv = portmod_reg_access_init(unit);
    if(rv < 0){
        return rv;
    }

    /* portmod init */
    rv = portmod_appl_init(unit, user_data[unit]);
    if(rv < 0){
        portmod_reg_access_deinit(unit);
        return rv;
    }

    rv = portmod_appl_add_ports(unit);
    if(rv < 0){
        portmod_appl_deinit(unit, user_data[unit]);
        portmod_reg_access_deinit(unit);
        return rv;
    }
    else{
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "ports added successfully!\n")));
    }

    /* portmod deinit */
    rv = portmod_appl_deinit(unit, user_data[unit]);
    if(rv < 0){
        return rv;
    }

    /* Register access deinit */
    rv = portmod_reg_access_deinit(unit);
    if(rv < 0){
        return rv;
    }

    return 0;
}

