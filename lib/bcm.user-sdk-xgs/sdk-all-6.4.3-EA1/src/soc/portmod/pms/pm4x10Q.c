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
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4x10Q_SUPPORT

#define PHY_REG_ADDR_DEVAD(_phy_reg)    (((_phy_reg) >> 27) & 0x1f)
#define QSGMII_REG_ADDR_LANE(_phy_reg)  (((_phy_reg) >> 16) & 0x7ff)
#define QSGMII_REG_ADDR_LANE_SET(_phy_reg_new, _phy_reg)    \
                            ((_phy_reg_new) |= ((_phy_reg) & 0x7ff0000))
#define QSGMII_REG_ADDR_REG(_phy_reg)   ((((_phy_reg) & 0x8000) >> 11) | \
                                        ((_phy_reg) & 0xf))
#define IS_QSGMII_REGISTER(_phy_reg) (((_phy_reg) & 0xf800f000) == 0x8000)

typedef struct pmx10q_user_data_s{
    phymod_access_t *pm4x10_access;
    portmod_default_user_access_t qsgmiie_user_data;
}pmx10q_user_data_t;


STATIC int
_pm4x10q_indacc_wait(int unit, int blk_id,  soc_field_t fwait)
{
    uint32 ctlsts;
    int poll = 1000;

    do {
        SOC_IF_ERROR_RETURN(
            READ_CHIP_INDACC_CTLSTSr(unit, blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, &ctlsts));
        if (soc_reg_field_get(unit, CHIP_INDACC_CTLSTSr, ctlsts, fwait)) {
            break;
        }
    } while (--poll > 0);

    if (poll <= 0) {
        return SOC_E_TIMEOUT;
    }
    return SOC_E_NONE;
}


STATIC int
_pm4x10q_indacc_gport_get(uint32 phy_reg, int *target_select)
{
    if (QSGMII_REG_ADDR_LANE(phy_reg) <= 15) {
        *target_select = (QSGMII_REG_ADDR_LANE(phy_reg) < 8) ? 0 : 1;
        return SOC_E_NONE;
    }
    
    return SOC_E_INTERNAL;
}


STATIC
int
_pm4x10q_indacc_write(portmod_default_user_access_t *user_data, uint32_t reg_addr, uint32_t val)
{
    int unit  = user_data->unit;
    int gport = 0;
    uint32 ctlsts;

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_gport_get(reg_addr, &gport));
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_WDATAr(unit, user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, val));
    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, gport);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, WR_REQf, 1);
    /*CL22 style*/
    reg_addr = reg_addr & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit, user_data->blk_id  | SOC_REG_ADDR_BLOCK_ID_MASK, ctlsts));

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_wait(unit, user_data->blk_id , WR_RDYf));
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit,user_data->blk_id  | SOC_REG_ADDR_BLOCK_ID_MASK,  0));

    return SOC_E_NONE;
}


STATIC
int
_pm4x10q_indacc_read(portmod_default_user_access_t *user_data, uint32_t reg_addr, uint32_t *val)
{
    int unit  = user_data->unit;
    int gport = 0;
    uint32 ctlsts;

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_gport_get(reg_addr, &gport));
    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, gport);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, RD_REQf, 1);
    /*CL22 style*/
    reg_addr = reg_addr & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit, user_data->blk_id  | SOC_REG_ADDR_BLOCK_ID_MASK, ctlsts));

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_wait(unit, user_data->blk_id , RD_RDYf));
    SOC_IF_ERROR_RETURN(READ_CHIP_INDACC_RDATAr(unit, user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, val));
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit,user_data->blk_id  | SOC_REG_ADDR_BLOCK_ID_MASK,  0));

    return SOC_E_NONE;
}


STATIC int
_pm4x10q_sbus_qsgmii_write(portmod_default_user_access_t *user_data, uint32_t phy_reg, uint32_t val)
{
    uint32  reg_addr, reg_data;

    /* The "phy_reg" in Sbus MDIO access is expected in 32 bits PHY address 
     *  format with AER information included. Since this interface allows  
     *  MDIO access in Claue22 only, the AER process must be applied.
     */

    /* AER process : AER block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = 0xffd0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* read data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, val));
    return SOC_E_NONE;
}


STATIC int
_pm4x10q_sbus_qsgmii_read(portmod_default_user_access_t *user_data, uint32_t phy_reg, uint32_t *val)
{
    uint32  reg_addr, reg_data;

    /* The "phy_reg" in Sbus MDIO access is expected in 32 bits PHY address 
     *  format with AER information included. Since this interface allows  
     *  MDIO access in Claue22 only, the AER process must be applied.
     */

    /* AER process : AER block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = 0xffd0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* read data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_read(user_data, reg_addr, val));
    return SOC_E_NONE;
}

STATIC int
_pm4x10q_is_pcs_reg(uint32_t reg_addr, int *is_pcs)
{
    *is_pcs = PHY_REG_ADDR_DEVAD(reg_addr == 0) && ((IS_QSGMII_REGISTER(reg_addr)) || (QSGMII_REG_ADDR_REG(reg_addr) < 0x10));
    return SOC_E_NONE;
}


STATIC int
_pm4x10q_sbus_reg_write(void *user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    pmx10q_user_data_t *user_data = user_acc;
    int is_pcs = 0;

    SOC_IF_ERROR_RETURN(_pm4x10q_is_pcs_reg(reg_addr, &is_pcs));

    if(is_pcs){
        return _pm4x10q_sbus_qsgmii_write(&user_data->qsgmiie_user_data, reg_addr, val);
    } 
    return user_data->pm4x10_access->bus->write(user_data->pm4x10_access->user_acc, core_addr, reg_addr, val);
}


STATIC int
_pm4x10q_sbus_reg_read(void *user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    pmx10q_user_data_t *user_data = user_acc;
    int is_pcs = 0;

    SOC_IF_ERROR_RETURN(_pm4x10q_is_pcs_reg(reg_addr, &is_pcs));
    if(is_pcs){
        return _pm4x10q_sbus_qsgmii_read(&user_data->qsgmiie_user_data, reg_addr, val);
    } 
    return user_data->pm4x10_access->bus->read(user_data->pm4x10_access->user_acc, core_addr, reg_addr, val);
}


phymod_bus_t pm4x10_tsc4e_0_bus = {
    "PM4X10Q PCS Bus",
    _pm4x10q_sbus_reg_read,
    _pm4x10q_sbus_reg_write,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY| PHYMOD_BUS_CAP_LANE_CTRL
};



int pm4x10Q_pm_interface_type_is_supported(int unit, int pm_id, soc_port_if_t interface, int* is_supported)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_pm_init(int unit, const portmod_pm_create_info_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;

    pm_info->type = portmodDispatchTypePm4x10Q;
    pm_info->wb_buffer_id = wb_buffer_index;

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_pm_destroy(int unit, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_detach(int unit, int port, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


int pm4x10Q_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


#endif /* PORTMOD_PM4x10Q_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
