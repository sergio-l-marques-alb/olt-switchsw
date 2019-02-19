/*
 * $Id: phymod_sim.c,v 1.18 Broadcom SDK $
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

/*
 * PHY Simulator (requires PHYMOD library)
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/spl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/scache.h>

#ifndef PHY_NUM_SIMS
#define PHY_NUM_SIMS            36
#endif

#if defined(PHYMOD_SUPPORT) && (PHY_NUM_SIMS > 0)
#define PHY_SIM_SUPPORT
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_WB_DEFAULT_VERSION            SOC_WB_VERSION_1_0
#endif

#ifdef PHY_SIM_SUPPORT

#include <phymod/phymod.h>
#include <phymod/chip/eagle_sim.h>

#include <phymod/chip/falcon_sim.h>
#include <phymod/chip/tscf_sim.h>
#include <phymod/chip/tsce_sim.h>
#include <phymod/chip/qsgmiie_sim.h>
#include <phymod/chip/viper_sim.h>

#include <soc/phy/phymod_sim.h>

#ifndef PHY_NUM_SIM_ENTRIES
#define PHY_NUM_SIM_ENTRIES     200
#endif

typedef struct soc_phy_sim_s {
    phymod_sim_entry_t pms_entries[PHY_NUM_SIM_ENTRIES];
    phymod_sim_t pms;
    int unit;
    uint32 phy_id;
} soc_phy_sim_t;

static soc_phy_sim_t soc_phy_sim[PHY_NUM_SIMS];
static int soc_phy_sims_used;


STATIC soc_phy_sim_t *
soc_physim_find(int unit, uint32 phy_id)
{
    int idx;
    soc_phy_sim_t *psim;

    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        psim = &soc_phy_sim[idx];
        if (psim->unit == unit && psim->phy_id == phy_id) {
            return psim;
        }
    }
    return NULL;
}

int
soc_physim_add(int unit, uint32 phy_id, phymod_sim_drv_t *pms_drv)
{
    int idx;
    soc_phy_sim_t *psim;

    /* Check if this simulator exists already */
    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        psim = &soc_phy_sim[idx];
        if (psim->unit == unit && psim->phy_id == phy_id) {
            /* Reset simulator */
            SOC_IF_ERROR_RETURN(
                phymod_sim_reset(&psim->pms));
            return SOC_E_NONE;
        }
    }

    /* Any free simulators? */
    if (soc_phy_sims_used >= PHY_NUM_SIMS) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "soc_physim_add: Out of resources for"
                              " unit=%d phy_id=0x%x\n"), unit, phy_id));
        return SOC_E_RESOURCE;
    }

    /* Add new simulator entry */
    psim = &soc_phy_sim[soc_phy_sims_used++];
    psim->unit = unit;
    psim->phy_id = phy_id;
    psim->pms.drv = pms_drv;

    /* Initialize and reset simulator */
    SOC_IF_ERROR_RETURN(
        phymod_sim_init(&psim->pms,
                        COUNTOF(psim->pms_entries), psim->pms_entries));
    SOC_IF_ERROR_RETURN(
        phymod_sim_reset(&psim->pms));

    return SOC_E_NONE;
}

#endif /* PHY_SIM_SUPPORT */

int
soc_physim_wrmask(int unit, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 phy_wr_data, uint16 wr_mask)
{
#ifdef PHY_SIM_SUPPORT
    soc_phy_sim_t *psim = soc_physim_find(unit, phy_id);
    uint32 data32;

    if (psim == NULL) {
        return SOC_E_INIT;
    }

    data32 = wr_mask;
    data32 <<= 16;
    data32 |= phy_wr_data;

    SOC_IF_ERROR_RETURN(
        phymod_sim_write(&psim->pms, phy_reg_addr, data32));

    LOG_INFO(BSL_LS_SOC_PHYSIM,
             (BSL_META_U(unit,
                         "soc_physim_wrmask 0x%03x:0x%04x = 0x%04x/0x%04x\n"),
              phy_id, phy_reg_addr, phy_wr_data, wr_mask));
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_physim_read(int unit, uint32 phy_id,
                uint32 phy_reg_addr, uint16 *phy_rd_data)
{
#ifdef PHY_SIM_SUPPORT
    soc_phy_sim_t *psim = soc_physim_find(unit, phy_id);
    uint32 data32;

    if (psim == NULL) {
        return SOC_E_INIT;
    }

    SOC_IF_ERROR_RETURN(
        phymod_sim_read(&psim->pms, phy_reg_addr, &data32));

    *phy_rd_data = data32;

    LOG_INFO(BSL_LS_SOC_PHYSIM,
             (BSL_META_U(unit,
                         "soc_physim_read 0x%03x:0x%04x = 0x%04x\n"),
              phy_id, phy_reg_addr, *phy_rd_data));
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_physim_write(int unit, uint32 phy_id,
                 uint32 phy_reg_addr, uint16 phy_wr_data)
{
    return soc_physim_wrmask(unit, phy_id, phy_reg_addr, phy_wr_data, 0);
}

#ifdef PORTMOD_SUPPORT

#include <soc/portmod/portmod.h>

/* Default simulator bus */
STATIC
int sim_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    portmod_default_user_access_t *user_data;
    uint16_t tmp=0;
    int rv;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (portmod_default_user_access_t*)user_acc;

    (*val) = 0;
    rv = soc_physim_read(user_data->unit, core_addr, reg_addr, &tmp);
    (*val) = tmp;

    return rv;
}

STATIC
int sim_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    portmod_default_user_access_t *user_data;
    uint16_t tmp = 0;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (portmod_default_user_access_t*)user_acc;

    tmp = (val & 0xFFFF);
    return soc_physim_write(user_data->unit, core_addr, reg_addr, tmp);
}

phymod_bus_t sim_bus = {
    "portmod_phy_sim",
    sim_bus_read,
    sim_bus_write,
    NULL,
    NULL,
    0
};

void
soc_physim_enable_get(int unit, int* is_sim)
{
    int use_sim;

    use_sim = soc_property_get(unit, spn_PHY_SIMUL, 0);

    if (use_sim || SAL_BOOT_PLISIM) {
        (*is_sim) = 1;
    } else {
        (*is_sim) = 0;
    }

    return ;
}

/*
addr_ident - in some cases the access->phy_id is the same for several PMs (which are accesses from diffrent blocks). 
In this case addr_ident is ORed with the phy_id to distinguish between the cores.
*/
int
soc_physim_check_sim(int unit, phymod_dispatch_type_t type, phymod_access_t* access, uint32 addr_ident, int* is_sim)
{
    int use_sim;
    phymod_sim_drv_t *sim = NULL;

    use_sim = soc_property_get(unit, spn_PHY_SIMUL, 0);

    if(use_sim || SAL_BOOT_PLISIM) {
        switch(type) {
            case phymodDispatchTypeEagle:
                sim = &eagle_sim_drv;
                break;
            case phymodDispatchTypeFalcon:
                sim = &falcon_sim_drv;
                break;
            case phymodDispatchTypeQsgmiie:
                sim = &qsgmiie_sim_drv;
                break;
            case phymodDispatchTypeTsce:
                sim = &tsce_sim_drv;
                break;
            case phymodDispatchTypeTscf:
                sim = &tscf_sim_drv;
                break;
#if 0
            case phymodDispatchTypeViper:
                sim = &viper_sim_drv;
                break;
#endif
            default:
                LOG_ERROR(BSL_LS_SOC_PHYSIM,
                         (BSL_META_U(unit,
                                     "Invalid simulator %d\n"), type));
                return SOC_E_NOT_FOUND;
        }

        PHYMOD_ACC_ADDR(access) |= addr_ident;
        SOC_IF_ERROR_RETURN(soc_physim_add(unit, PHYMOD_ACC_ADDR(access), sim));

        PHYMOD_ACC_BUS(access) = &sim_bus;
        (*is_sim) = 1;

    } else {
        /* Not simulator - do nothing*/
        (*is_sim) = 0;
    }

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_physim_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *physim_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    int idx;
    int default_ver = SOC_WB_DEFAULT_VERSION;

    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        alloc_size +=   sizeof(int) +   /* unit*/
               sizeof(int) +            /* phy_id */
               sizeof(int) +            /* num_entries */
               sizeof(int) +            /* entries_used */
               (sizeof(phymod_sim_entry_t) * PHY_NUM_SIM_ENTRIES); /* pms_entries */
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_PHYSIM_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_size,
                                          &physim_scache_ptr,
                                          default_ver,
                                          NULL);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int
soc_physim_scache_sync(int unit)
{
    uint8 *physim_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    int rv = 0;
    int idx, idx_pms;
    soc_phy_sim_t *psim;
    phymod_sim_entry_t *physim_entry_scache_p;
    uint32 *u32_scache_p;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_PHYSIM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_size,
                        &physim_scache_ptr,
                        SOC_WB_DEFAULT_VERSION,
                        NULL);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    u32_scache_p = (uint32 *)physim_scache_ptr;
    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        psim = &soc_phy_sim[idx];

        *u32_scache_p = psim->unit;     /* unit*/
        u32_scache_p++;
        *u32_scache_p = psim->phy_id;   /* phy_id */
        u32_scache_p++;
        *u32_scache_p = psim->pms.data.num_entries; /* num_entries */
        u32_scache_p++;
        *u32_scache_p = psim->pms.data.entries_used;/* entries_used */
        u32_scache_p++;

        /* pms_entries */
        physim_entry_scache_p = (phymod_sim_entry_t *)u32_scache_p;
        for (idx_pms = 0; idx_pms < psim->pms.data.entries_used; idx_pms++) {
            physim_entry_scache_p->flags = psim->pms_entries[idx_pms].flags;
            physim_entry_scache_p->addr = psim->pms_entries[idx_pms].addr;
            physim_entry_scache_p->data = psim->pms_entries[idx_pms].data;
            physim_entry_scache_p++;
        }
        u32_scache_p = (uint32 *)physim_entry_scache_p;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int
soc_physim_scache_recovery(int unit)
{
    uint8 *physim_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    int rv = 0;
    int idx, idx_pms;
    soc_phy_sim_t *psim;
    phymod_sim_entry_t *physim_entry_scache_p;
    uint32 *u32_scache_p;
    uint16  recovered_ver = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_PHYSIM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_size,
                        &physim_scache_ptr,
                        SOC_WB_DEFAULT_VERSION,
                        &recovered_ver);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    if (recovered_ver >= SOC_WB_VERSION_1_0) {
        u32_scache_p = (uint32 *)physim_scache_ptr;
        for (idx = 0; idx < soc_phy_sims_used; idx++) {
            psim = &soc_phy_sim[idx];

            psim->unit = *u32_scache_p;     /* unit*/
            u32_scache_p++;
            psim->phy_id = *u32_scache_p;   /* phy_id */
            u32_scache_p++;
            psim->pms.data.num_entries = *u32_scache_p; /* num_entries */
            u32_scache_p++;
            psim->pms.data.entries_used = *u32_scache_p;/* entries_used */
            u32_scache_p++;

            /* pms_entries */
            physim_entry_scache_p = (phymod_sim_entry_t *)u32_scache_p;
            for (idx_pms = 0; idx_pms < psim->pms.data.entries_used; idx_pms++) {
                psim->pms_entries[idx_pms].flags = physim_entry_scache_p->flags;
                psim->pms_entries[idx_pms].addr = physim_entry_scache_p->addr;
                psim->pms_entries[idx_pms].data = physim_entry_scache_p->data;
                physim_entry_scache_p++;
            }
            u32_scache_p = (uint32 *)physim_entry_scache_p;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* PORTMOD_SUPPORT */

