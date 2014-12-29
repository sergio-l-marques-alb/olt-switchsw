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
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_system.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT


#define PORT_MAX_PHY_ACCESS_STRUCTURES (6)

STATIC 
int
portmod_common_port_phy_access_get(int unit, int port, int max_phys, phymod_phy_access_t* access, int* nof_phys){
    portmod_access_get_params_t params;
    SOC_INIT_FUNC_DEFS

    portmod_access_get_params_t_init(unit, &params);
    _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, max_phys, access, nof_phys));
exit:
    SOC_FUNC_RETURN; 
}

int
portmod_common_phy_prbs_config_set(int unit, int port, int flags, const phymod_prbs_t* config)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    for( i = 0 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_set(&phys_access[i], flags, config));
    }
exit:
    SOC_FUNC_RETURN; 
}

int
portmod_common_phy_prbs_config_get(int unit, int port, int flags, phymod_prbs_t* config)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    phymod_prbs_t tmp_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_get(&phys_access[0], flags, config));
    for( i = 1 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_get(&phys_access[i], flags, &tmp_config));
        if (sal_memcmp(config, &tmp_config, sizeof(tmp_config)) != 0){
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("phys are not configured the same"));
        }
    }
exit:
    SOC_FUNC_RETURN; 
}


int portmod_commmon_portmod_to_phymod_loopback_type(int unit, portmod_loopback_mode_t loopback_type, phymod_loopback_mode_t *phymod_lb_type)
{
        
    SOC_INIT_FUNC_DEFS;
    switch(loopback_type){
    case portmodLoopbackPhyGloopPCS:
        *phymod_lb_type = phymodLoopbackGlobal;
        break;
    case portmodLoopbackPhyGloopPMD:
        *phymod_lb_type = phymodLoopbackGlobalPMD;
        break;
    case portmodLoopbackPhyRloopPCS:
        *phymod_lb_type = phymodLoopbackRemotePCS;
        break;
    case portmodLoopbackPhyRloopPMD:
        *phymod_lb_type = phymodLoopbackRemotePMD;
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("unsupported loopback type %d", loopback_type));
    }
exit:
    SOC_FUNC_RETURN; 
}

int
portmod_common_phy_loopback_set(int unit, int port, portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    phymod_loopback_mode_t phymod_lb_type;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit, loopback_type, &phymod_lb_type));
    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    for( i = 0 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_loopback_set(&phys_access[i], phymod_lb_type, enable));
    }
exit:
    SOC_FUNC_RETURN; 
}

int
portmod_common_phy_loopback_get(int unit, int port, portmod_loopback_mode_t loopback_type, int *enable)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    uint32 tmp_enable = 0;
    int tmp_enable_signed;
    phymod_loopback_mode_t phymod_lb_type;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit, loopback_type, &phymod_lb_type));

    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));

    _SOC_IF_ERR_EXIT(phymod_phy_loopback_get(&phys_access[0], phymod_lb_type, &tmp_enable));
    *enable = (tmp_enable != 0);
    for( i = 1 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_loopback_get(&phys_access[i], phymod_lb_type, &tmp_enable));
        tmp_enable_signed = (tmp_enable != 0);
        if (*enable != tmp_enable_signed){
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("phys are not configured the same"));
        }
    }
exit:
    SOC_FUNC_RETURN; 
}


int portmod_common_phy_prbs_enable_set(int unit, int port, int flags, int enable)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    for( i = 0 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_set(&phys_access[i], flags, enable));
    }
exit:
    SOC_FUNC_RETURN; 
}


int portmod_common_phy_prbs_enable_get(int unit, int port, int flags, int* enable)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    uint32 tmp_enable = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_get(&phys_access[0], flags, &tmp_enable));
    *enable = (tmp_enable != 0);
    for( i = 1 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_get(&phys_access[i], flags, &tmp_enable));
        if (*enable != tmp_enable){
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("phys are not configured the same"));
        }
    }
exit:
    SOC_FUNC_RETURN; 
}


int portmod_common_phy_prbs_status_get(int unit, int port, int flags, phymod_prbs_status_t* status)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    phymod_prbs_status_t tmp_status;
    SOC_INIT_FUNC_DEFS;

    phymod_prbs_status_t_init(status);
    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    for( i = 0 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_prbs_status_get(&phys_access[i], flags, &tmp_status));
        status->error_count += tmp_status.error_count;
        status->prbs_lock |= tmp_status.prbs_lock;
        status->prbs_lock_loss |= tmp_status.prbs_lock_loss;
    }
exit:
    SOC_FUNC_RETURN; 
}


int portmod_port_cl72_set(int unit, int port, int enable)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    uint32 tmp_enable = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    tmp_enable = (enable != 0);
    for( i = 0 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_cl72_set(&phys_access[i], tmp_enable));
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_port_cl72_get(int unit, int port, int* enable)
{
    phymod_phy_access_t phys_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
    int phys_num = 0;
    int i;
    uint32 tmp_enable = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_port_phy_access_get(unit, port, PORT_MAX_PHY_ACCESS_STRUCTURES, phys_access, &phys_num));
    _SOC_IF_ERR_EXIT(phymod_phy_cl72_get(&phys_access[0], &tmp_enable));
    *enable = (tmp_enable != 0);
    for( i = 1 ; i < phys_num; i++){
        _SOC_IF_ERR_EXIT(phymod_phy_cl72_get(&phys_access[i], &tmp_enable));
        if (*enable != tmp_enable){
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("phys are not configured the same"));
        }
    }
exit:
    SOC_FUNC_RETURN; 
}


int portmod_common_phy_firmware_mode_set(int unit, int port, phymod_firmware_mode_t fw_mode)
{
    SOC_INIT_FUNC_DEFS;
    SOC_FUNC_RETURN; 
}


int portmod_common_phy_firmware_mode_get(int unit, int port, phymod_firmware_mode_t *fw_mode)
{
    SOC_INIT_FUNC_DEFS;
    SOC_FUNC_RETURN; 
}




/** 
 *  @brief generic function for register write for PM4X25,
 *         PM4X10, PM 12X10
 *  @param reg_access_mem - the memory that used for access PHY
 *                        registers. e.g: CLPORT_UCMEM_DATAm,
 *                        XLPORT_UCMEM_DATA
 *  @param user_acc - user data. this function expect to get
 *                  portmod_phy_op_data_t structure
 *  @param core_address -  only 5 bits of PHY id used in case
 *                       of sbus
 *  @param reg_addr - the reigister addrress
 *  @param val - the value to write to register. 16 bits value +
 *             16 bits mask

 */
int
portmod_common_phy_sbus_reg_write(soc_mem_t reg_access_mem, void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    int rv= SOC_E_NONE;
    soc_reg_above_64_val_t mem_data;
    portmod_default_user_access_t *user_data = user_acc;
    uint32 data, data_mask;
    SOC_REG_ABOVE_64_CLEAR(mem_data);

    if(user_data == NULL){
        return SOC_E_PARAM;
    }

    /* If write mask (upper 16 bits) is empty, add full mask */
    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }
    
    soc_mem_lock(user_data->unit, reg_access_mem);


    /* assigning TSC register address to ucmem_data[31:0]  and write the 
     * data/datamask to to ucmem_data[63:32] */
    mem_data[0] = (reg_addr & 0xffffffff) | ((core_addr & 0x1f) << 19);
    /* data: ucmem_data[48:63]
       datamask: ucmem_data[32:47]
    */
    data = (val & 0xffff) << 16;
    data_mask = (~val & 0xffff0000) >> 16;
    mem_data[1] = data | data_mask;
    mem_data[2] = 1; /* for TSC register write */

    rv = soc_mem_write(user_data->unit, reg_access_mem, user_data->blk_id, 0, mem_data); 
    
    soc_mem_unlock(user_data->unit, reg_access_mem);

    LOG_BSL_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(user_data->unit,
                          "_portmod_utils_sbus_reg_write[%d]: "
                          "addr=0x%x reg=0x%08x data=0x%08x mask=0x%08x(%d/%d)\n"),
               user_data->unit, core_addr, reg_addr, val , data_mask, user_data->blk_id, rv));

    return rv;
}


/** 
 *  @brief generic function for register read for PM4X25,
 *         PM4X10, PM 12X10
 *  @param reg_access_mem - the memory that used for access PHY
 *                        registers. e.g: CLPORT_UCMEM_DATAm,
 *                        XLPORT_UCMEM_DATA
 *  @param user_acc - user data. this function expect to get
 *                  portmod_phy_op_data_t structure
 *  @param core_address -  only 5 bits of PHY id used in case
 *                       of sbus
 *  @param reg_addr - the reigister addrress
 *  @param val - the value to write to register. no mask for
 *             read
 */
int
portmod_common_phy_sbus_reg_read(soc_mem_t reg_access_mem, void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t mem_data;
    portmod_default_user_access_t *user_data = user_acc;
    SOC_REG_ABOVE_64_CLEAR(mem_data);

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    
    /* assigning TSC register address to ucmem_data[31:0] */
    mem_data[0] = (reg_addr & 0xffffffff) | ((core_addr & 0x1f) << 19);
    mem_data[2] = 0; /* for TSC register READ */

    rv = soc_mem_write(user_data->unit, reg_access_mem, user_data->blk_id, 0, mem_data); 
    
    /* read data back from ucmem_data[47:32] */
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_read(user_data->unit, reg_access_mem, user_data->blk_id, 0, mem_data);
    }
    *val = mem_data[1];

    LOG_BSL_DEBUG(BSL_LS_SOC_MII, (BSL_META_U(user_data->unit,
                 "_portmod_utils_sbus_reg_read[%d]: "
                 "addr=0x%x reg=0x%08x data=0x%08x (%d/%d)\n"),
                 user_data->unit, core_addr, reg_addr, *val, user_data->blk_id, rv));

    return rv;
}

int
portmod_common_mutex_take(void* user_acc)
{
    portmod_default_user_access_t* user_data;
    int unit = ((portmod_default_user_access_t*)user_acc)->unit;
    SOC_INIT_FUNC_DEFS;
    (void)unit;

    user_data = (portmod_default_user_access_t*)user_acc;

    if(user_data->mutex != NULL) {
        _SOC_IF_ERR_EXIT(sal_mutex_take(user_data->mutex, sal_mutex_FOREVER));
    }

exit:
    SOC_FUNC_RETURN;
}

int 
portmod_common_mutex_give(void* user_acc)
{
    portmod_default_user_access_t* user_data;
    int unit = ((portmod_default_user_access_t*)user_acc)->unit;
    SOC_INIT_FUNC_DEFS;
    (void)unit;

    user_data = (portmod_default_user_access_t*)user_acc;

    if(user_data->mutex != NULL) {
        _SOC_IF_ERR_EXIT(sal_mutex_give(user_data->mutex));
    }

exit:
    SOC_FUNC_RETURN;
}

int
portmod_firmware_set(int unit, 
                     int blk_id,
                     const uint8 *array, 
                     uint32 datalen, 
                     portmod_ucode_buf_order_t data_swap, 
                     portmod_ucode_buf_t* buf,
                     soc_mem_t ucmem_data, 
                     soc_reg_t ucmem_ctrl)
{
    int entry_bytes, entry_num;
    int count, extra_bytes, i, j;
    uint8 *dma_buf_ptr;
    const uint8 *array_ptr;
    int arr_pos_le[3] [16]
        = {{15, 14, 13, 12, 11, 10,  9,  8, 7, 6, 5, 4, 3, 2, 1, 0},
           { 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7},
           { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    int arr_pos_be[3] [16]
        = {{12, 13, 14, 15,  8,  9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3},
            {11, 10,  9,  8, 15, 14, 13, 12, 3, 2, 1, 0, 7, 6, 5, 4},
            {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12}};
    int * arr_pos;
    int endian;
    int reg_access_idx = (blk_id | SOC_REG_ADDR_BLOCK_ID_MASK);
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    entry_bytes = soc_mem_entry_bytes(unit, ucmem_data);
    entry_num = soc_mem_index_count(unit, ucmem_data);
    if (datalen > (entry_bytes * entry_num)) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Can't fit all of the firmware into the device load table."));
    }

    portmod_sys_get_endian(unit, &endian);
    if(endian) {
        arr_pos = arr_pos_be[data_swap];
    } else {
        arr_pos = arr_pos_le[data_swap];
    }

    if (buf->ucode_dma_buf == NULL) {
        count = datalen / entry_bytes;
        extra_bytes = datalen % entry_bytes;
        buf->ucode_alloc_size = datalen;
        if (extra_bytes != 0) {
            buf->ucode_alloc_size += entry_bytes - extra_bytes;
        }
        buf->ucode_dma_buf =
            portmod_sys_dma_alloc(unit, buf->ucode_alloc_size,
                          "WC ucode DMA buffer");
        if (buf->ucode_dma_buf == NULL) {
            _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, ("Failed to alloc WC ucode DMA buffer"));
        }

        array_ptr = array;
        dma_buf_ptr = buf->ucode_dma_buf;
        for (i = 0; i < count; i++) {
            for (j = 0; j < 16; j++) {
                dma_buf_ptr[arr_pos[j]] = array_ptr[j];
            }
            array_ptr += entry_bytes;
            dma_buf_ptr += entry_bytes;
        }
        if (extra_bytes != 0) {
            sal_memset(dma_buf_ptr, 0, entry_bytes);
            for (j = 0; j < extra_bytes; j++) {
                dma_buf_ptr[arr_pos[j]] = array_ptr[j];
            }
        }
    }

    /* enable parallel bus access */
    _SOC_IF_ERR_EXIT(soc_reg_get(unit,ucmem_ctrl, reg_access_idx, 0, &reg_val));
    soc_reg64_field32_set(unit, ucmem_ctrl, &reg_val, ACCESS_MODEf, 1);
    _SOC_IF_ERR_EXIT(soc_reg_set(unit,ucmem_ctrl, reg_access_idx, 0, reg_val));

    _SOC_IF_ERR_EXIT
        (soc_mem_write_range(unit, ucmem_data, blk_id, 0,
                             buf->ucode_alloc_size / entry_bytes - 1,
                             buf->ucode_dma_buf));

    /* disable parallel bus access, and enable MDIO access */
    _SOC_IF_ERR_EXIT(soc_reg_get(unit,ucmem_ctrl, reg_access_idx, 0, &reg_val));
    soc_reg64_field32_set(unit, ucmem_ctrl, &reg_val, ACCESS_MODEf, 0);
    _SOC_IF_ERR_EXIT(soc_reg_set(unit,ucmem_ctrl, reg_access_idx, 0, reg_val));

exit:
    SOC_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
