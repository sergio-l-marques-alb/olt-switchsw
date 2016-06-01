/*
 * $Id: portmod_phy_intf_map.c $
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
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif 
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

typedef void (*portmod_line_to_sys_intf_map_f)(const phymod_phy_access_t*, soc_port_t port, int data_rate,
                                            phymod_interface_t line_intf, phymod_interface_t *sys_intf); 

typedef struct _portmod_intf_map_s_
{
    phymod_dispatch_type_t            dispatch_type;
    portmod_line_to_sys_intf_map_f    portmod_intf_map; 
} portmod_intf_map_t;

#ifdef PHYMOD_SESTO_SUPPORT
STATIC
void sesto_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf) 
{
    if (data_rate >= 100000) {
         *sys_intf = phymodInterfaceCAUI;
    } else if (data_rate >= 40000) {
        if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy_acc)) {
            *sys_intf = phymodInterfaceXLAUI2;
        } else {
            *sys_intf = phymodInterfaceXLAUI;
        }
    } else if (data_rate >= 10000) {
         *sys_intf = phymodInterfaceXFI;
    }
}

STATIC
void sesto_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *new_line_intf)
{
    switch(line_intf) {
        case phymodInterfaceXLAUI2:
             *new_line_intf = phymodInterfaceSR4;
             break;
        default: /* by default set system side same as line side */
             *new_line_intf = line_intf;
             break;
    }
}
#endif

#ifdef PHYMOD_DINO_SUPPORT
STATIC
void dino_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf) 
{
    if (data_rate >= 100000) {
         *sys_intf = phymodInterfaceCAUI;
    } else if (data_rate >= 40000) {
        if (phy_acc->access.lane_mask == 0xF) {
            *sys_intf = phymodInterfaceXLAUI;
        } else {
            *sys_intf = phymodInterfaceXLAUI2;
        }
    } else if (data_rate >= 10000) {
         *sys_intf = phymodInterfaceXFI;
    }
}

STATIC
void dino_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *new_line_intf)
{
    switch(line_intf) {
        case phymodInterfaceXLAUI2:
             *new_line_intf = phymodInterfaceSR4;
             break;
        default: /* by default set system side same as line side */
             *new_line_intf = line_intf;
             break;
    }
}
#endif

#ifdef PHYMOD_QUADRA28_SUPPORT
STATIC
void quadra28_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf) 
{
    switch(line_intf) {
        case phymodInterfaceLR:
        case phymodInterfaceSR:
        case phymodInterfaceCR:
        case phymodInterfaceKR:
             *sys_intf = phymodInterfaceXFI;
             break;

        case phymodInterfaceLR4:
        case phymodInterfaceSR4:
        case phymodInterfaceCR4:
        case phymodInterfaceKR4:
             *sys_intf = phymodInterfaceXLAUI;
             break;

        case phymodInterfaceXFI:
             *sys_intf = phymodInterfaceKR;
             break;
  
        default: /* by default set system side same as line side */
             *sys_intf = line_intf;
             break;
    }
}
STATIC
void quadra28_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *new_line_intf) 
{
    switch(line_intf) {
        case phymodInterfaceSFI:
             *new_line_intf = phymodInterfaceSR;
             break;
        default: /* by default set system side same as line side */
             *new_line_intf = line_intf;
             break;
    }
}
#endif

#ifdef PHYMOD_FURIA_SUPPORT
STATIC
void furia_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf)
{
    if (data_rate >= 100000) {
         *sys_intf = phymodInterfaceCAUI4;
    } else if (data_rate >= 40000) {
         *sys_intf = phymodInterfaceXLAUI;
    } else if (data_rate >= 10000) {
         *sys_intf = phymodInterfaceXFI;
    }

}

STATIC
void furia_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *new_line_intf)
{
    switch(line_intf) {
        case phymodInterfaceCAUI:
             *new_line_intf = phymodInterfaceSR4;
             break;
        case phymodInterfaceXFI:
             *new_line_intf = phymodInterfaceSR;
             break;
        default: /* by default set system side same as line side */
             *new_line_intf = line_intf;
             break;
    }
}
#endif


portmod_intf_map_t  portmod_intf_map_array[] =
{
#ifdef PHYMOD_SESTO_SUPPORT
    {phymodDispatchTypeSesto, sesto_intf_map},
#endif
#ifdef PHYMOD_DINO_SUPPORT
    {phymodDispatchTypeDino, dino_intf_map},
#endif
#ifdef PHYMOD_QUADRA28_SUPPORT
    {phymodDispatchTypeQuadra28, quadra28_intf_map},
#endif
#ifdef PHYMOD_FURIA_SUPPORT
        {phymodDispatchTypeFuria, furia_intf_map},
#endif
    {phymodDispatchTypeCount, NULL}
};

int
portmod_port_line_to_sys_intf_map(const phymod_phy_access_t* line_phy_acc, const phymod_phy_access_t* sys_phy_acc,
                                  soc_port_t port, int data_rate, int ref_clk,
                                  phymod_interface_t line_intf, phymod_interface_t *sys_intf)
{
    int count = 0;
    int rv = PHYMOD_E_NONE;
    phymod_phy_inf_config_t tmp_config;
    int sys_if_valid = 1;

    /* play safe - always make sys_intf same as line intf */
    *sys_intf = line_intf;

    while(portmod_intf_map_array[count].dispatch_type != phymodDispatchTypeCount) {
        if (portmod_intf_map_array[count].dispatch_type == line_phy_acc->type) {
            portmod_intf_map_array[count].portmod_intf_map(line_phy_acc, port, data_rate, line_intf, sys_intf);
            break;
        } 
        count++;
    } 

    /* If the line-side PHY is legacy, the sys-side interface is yet to be determined at this point.
     * The stragegy below is:
     * 1. if there is already a meaningful interface type in the system side, keep it.
     * 2. otherwise sys-side interface = line-side interface
     */
    if (portmod_intf_map_array[count].dispatch_type == phymodDispatchTypeCount &&
            PORTMOD_IS_LEGACY_PHY_GET(&(line_phy_acc->access))){
        rv = phymod_phy_interface_config_get(sys_phy_acc, 0, ref_clk, &tmp_config);
        if (rv) return (rv);

        /* It is possible that the interface type of the internal PHY does not make sense according
         * to the given speed. This happens during the boot-up process. For example,
         * _tsce_speed_id_interface_config_get() returns SGMII if the speed ID does not
         * exist. At the same time the speed given by the external PHY callback function
         * may be 40G. In this case, we honor whatever interface type provided from the line side.
         */
        if (data_rate > 1100 && tmp_config.interface_type == phymodInterfaceSGMII){
            sys_if_valid = 0;
        }

        if (tmp_config.interface_type != phymodInterfaceBypass && tmp_config.interface_type != phymodInterfaceCount && sys_if_valid){
            *sys_intf = tmp_config.interface_type;
        } else{
            *sys_intf = line_intf;
        }
    }

    return (SOC_E_NONE);
}

portmod_intf_map_t  portmod_line_intf_map_array[] =
{
#ifdef PHYMOD_SESTO_SUPPORT
    {phymodDispatchTypeSesto, sesto_line_intf_map},
#endif
#ifdef PHYMOD_DINO_SUPPORT
    {phymodDispatchTypeDino, dino_line_intf_map},
#endif
#ifdef PHYMOD_QUADRA28_SUPPORT
    {phymodDispatchTypeQuadra28, quadra28_line_intf_map},
#endif
#ifdef PHYMOD_FURIA_SUPPORT
    {phymodDispatchTypeFuria, furia_line_intf_map},
#endif
    {phymodDispatchTypeCount, NULL}
};

int
portmod_port_line_intf_map_fix(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                                  phymod_interface_t line_intf, phymod_interface_t *new_line_intf)
{
    int count = 0;

    /* play safe - always make new_line_intf same as line intf */
    *new_line_intf = line_intf;

    while(portmod_line_intf_map_array[count].dispatch_type != phymodDispatchTypeCount) {
        if (portmod_line_intf_map_array[count].dispatch_type == phy_acc->type) {
            portmod_line_intf_map_array[count].portmod_intf_map(phy_acc, port, data_rate, line_intf, new_line_intf);
            break;
        } 
        count++;
    } 
    return (SOC_E_NONE);
}

#undef _ERR_MSG_MODULE_NAME
