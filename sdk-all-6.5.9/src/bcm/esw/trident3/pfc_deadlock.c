/*
 * $Id: pfc_deadlock.c $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * PFC Deadlock Detection & Recovery
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/trident3.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident3.h>

#if defined(BCM_TRIDENT3_SUPPORT)

#define BCM_TD3_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS 4
#define BCM_TD3_PFC_DEADLOCK_SPLIT0_MAX_PORT 63

int
_bcm_td3_pfc_deadlock_hw_cos_index_get(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control->hw_cos_idx_inuse[priority] == TRUE) {
        *hw_cos_index = priority;
        return BCM_E_NONE;
    }
    return BCM_E_NOT_FOUND;
}

int
_bcm_td3_pfc_deadlock_hw_cos_index_set(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    int rv = BCM_E_NONE;
    int temp_hw_index = -1;
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_config = NULL;

    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);

    rv = _bcm_td3_pfc_deadlock_hw_cos_index_get(unit, priority, &temp_hw_index);
    if (rv != BCM_E_NONE) {
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    if (temp_hw_index != -1) {
        /* Config for this priority exists already. Donot reprogram */
        *hw_cos_index = priority;
        return BCM_E_NONE;
    }

    /* New priority */
    pfc_deadlock_control->hw_cos_idx_inuse[priority] = TRUE;
    pfc_deadlock_config = _BCM_PFC_DEADLOCK_CONFIG(unit, priority);
    pfc_deadlock_config->priority = priority;
    pfc_deadlock_config->flags |= _BCM_PFC_DEADLOCK_F_ENABLE;
    pfc_deadlock_control->pfc_deadlock_cos_used++;
    *hw_cos_index = priority;
    return BCM_E_NONE;
}

int
 _bcm_td3_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port)
{
    
    return BCM_E_UNAVAIL;
}

int
_bcm_td3_pfc_deadlock_recovery_begin(int unit, int cos, int pipe, int pipe_mmu_port)
{
    
    return BCM_E_UNAVAIL;
}

int
_bcm_td3_pfc_deadlock_recovery_reset(int unit)
{
    
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_td3_pfc_deadlock_monitor
 * Purpose:
 *     1) Monitor the hardware for Deadlock status
 *     2) Start Recovery process for the Deadlocked port/queue
 *     3) Reset Port back to original state on expiry of recovery phase
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Step 1 - Monitor (Deadlock Detection phase):
 *          Sw polls for Deadlock detection intr set for port
 *          Q_SCHED_DD_TIMER_STATUS_SPLIT0|1 (pbmp)
 *
 *     Step 2: Recovery Begin phase
 *          2.a: Mask the Intr Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT0|1 by setting 1 (pbmp)
 *          2.b: Turn timer off Q_SCHED_DD_TIMER_ENABLE_SPLITE0|1 by setting 0 (pbmp)
 *          2.c: For that port, set ignore_pfc_xoff = 1 (per port reg)
 *          2.4: Start recovery timer
 *      Step 3: Recovery End phase (On expiry of recovery timer)
 *          3.a: Reset ignore_xoff; set ignore_pfc_xoff = 0
 *          3.b: UnMask the Intr Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT0|1 by setting 0 (pbmp)
 *          3.c: Turn timer off Q_SCHED_DD_TIMER_ENABLE_SPLITE0|1 by setting 1 (pbmp)
 */
int
_bcm_td3_pfc_deadlock_monitor(int unit)
{
    int cos, reg_index, mmu_port = 0, priority = 0;
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_pfc_hw_resorces_t *hw_res = NULL;
    uint64 status64, mask64;

    COMPILER_64_ZERO(status64);
    COMPILER_64_ZERO(mask64);
    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    for (cos = 0; cos < pfc_deadlock_control->pfc_deadlock_cos_max; cos++) {
        /* Check if index is in Use */
        if (pfc_deadlock_control->hw_cos_idx_inuse[cos] == TRUE) {
            /* Check Hardware if New ports have been declared to be in
             * Deadlock condition
             */
            for (reg_index = 0; reg_index < COUNTOF( hw_res->timer_status); reg_index++) {

                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, hw_res->timer_status[reg_index], 0, cos, &status64)); 
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, hw_res->timer_mask[reg_index], 0, cos, &status64)); 

                /* Mask - bit 0 indicates enabled for reporting */
                COMPILER_64_NOT(mask64);
                COMPILER_64_AND(status64, mask64);
                if (COMPILER_64_IS_ZERO(status64) == 0 && reg_index == 0) {
                    /* New ports declared to be in Deadlock status */
                    for (mmu_port = 0; mmu_port <= BCM_TD3_PFC_DEADLOCK_SPLIT0_MAX_PORT; mmu_port++) {
                        if (mmu_port < 32) {
                            if ((COMPILER_64_LO(status64) &
                                (1 << mmu_port)) == FALSE) {
                                continue;
                            }
                        } else {
                            if ((COMPILER_64_HI(status64) &
                                (1 << (mmu_port - 32))) == FALSE) {
                                continue;
                            }
                        }
                        BCM_IF_ERROR_RETURN(
                            _bcm_td3_pfc_deadlock_recovery_begin(unit, cos,
                                                                 0, mmu_port));
                    }
                }
                else if (COMPILER_64_IS_ZERO(status64) == 0 && reg_index == 1) {
                    /* Q_SCHED_DD_TIMER_STATUS_(MASK)_SPLIT1 has only one bit valid  */
#if 0
                    /* TD3TBD Need to update correct enable bitmap */
                    if ((COMPILER_64_LO(status64) &
                            (1 << mmu_port)) == FALSE) {
                            continue;
                    }
#endif
                    BCM_IF_ERROR_RETURN(
                        _bcm_td3_pfc_deadlock_recovery_begin(unit, cos,
                                                             0, mmu_port));
                }

            }

            /* Update the count for ports already in Deadlock state and Reset
             * those ports where recovery timer has expired.
             */
            priority = pfc_deadlock_control->pfc_cos2pri[cos];
            pfc_deadlock_pri_config = _BCM_PFC_DEADLOCK_CONFIG(unit, priority);
            /* Updates required for Enabled COS */
            if (SOC_PBMP_NOT_NULL(pfc_deadlock_pri_config->deadlock_ports)) {
                BCM_IF_ERROR_RETURN(_bcm_pfc_deadlock_recovery_update(unit, cos));
            }
        }
    }
    return BCM_E_NONE;
}

/* TD3 Q_SCHED_DD_CHIP_CONFIG CONTENTS
 * Q_SCHED_DD_CHIP_CONFIG0:DD_TIMER_INIT_VALUE_COS_0~DD_TIMER_INIT_VALUE_COS_4
 * Q_SCHED_DD_CHIP_CONFIG1:DD_TIMER_INIT_VALUE_COS_5~DD_TIMER_INIT_VALUE_COS_9
 * Q_SCHED_DD_CHIP_CONFIG2: ignore_pfc_off_pkt_discard
 */
int
_bcm_td3_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config)
{
    if (priority <= BCM_TD3_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG0r;
    } else {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG1r;
    }
    return BCM_E_NONE;
}

/* Routine to perform the Set/Get operation for the Priority */
int
_bcm_td3_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config)
{
    int hw_cos_index = -1;
    uint32 rval = 0;
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    uint32 detection_granularity = 0;
    soc_reg_t   chip_config;
    soc_field_t cos_init_timer_field;
    _bcm_pfc_hw_resorces_t *hw_res = NULL;

    hw_res = &pfc_deadlock_control->hw_regs_fields;
    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);
    detection_granularity = (_BCM_PFC_DEADLOCK_TIME_UNIT(unit) ==
        bcmSwitchPFCDeadlockDetectionInterval10MiliSecond) ? 10 : 100;

    BCM_IF_ERROR_RETURN(
        _bcm_td3_pfc_deadlock_hw_cos_index_set(unit, priority, &hw_cos_index));

    if (hw_cos_index == -1) {
        /* No matching or free hw_index available for use */
        return BCM_E_RESOURCE;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td3_pfc_deadlock_chip_config_get(unit, priority, &chip_config));

    cos_init_timer_field = hw_res->time_init_val[hw_cos_index];

    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                      REG_PORT_ANY, 0, &rval));
    if (operation == _bcmPfcDeadlockOperGet) {

        config->detection_timer = soc_reg_field_get(unit,
                                        chip_config, rval,
                                        cos_init_timer_field);
        config->detection_timer *= detection_granularity;
    } else { /* _bcmPfcDeadlockOperSet */
        soc_reg_field_set(unit, chip_config, &rval,
                          cos_init_timer_field,
                          (config->detection_timer / detection_granularity));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, chip_config,
                                          REG_PORT_ANY, 0, rval));
        config->priority = priority;
        pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = priority;
        pfc_deadlock_control->pfc_pri2cos[priority] = hw_cos_index;
    }

    return BCM_E_NONE;
}

int
_bcm_td3_pfc_deadlock_ignore_pfc_xoff_clear(int unit, int cos, bcm_port_t port)
{
    int priority = 0;
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval;

    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }

    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    /* For that port, set ignore_pfc_xoff = 0 (per port reg) */
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config, port, 0, &rval));
    rval &= ~(1 << priority);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config, port, 0, rval));

    return BCM_E_NONE;
}

int
_bcm_td3_pfc_deadlock_q_config_helper(int unit,
                                     _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status)
{
    int rv = BCM_E_NONE;
    uint32 temp_val_lo = 0, temp_val_hi = 0;
    uint64 rval64;
    bcm_port_t local_port;
    _bcm_pfc_hw_resorces_t *hw_res = NULL;
    soc_reg_t reg = INVALIDr;
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port, mmu_port, priority, hw_cos_index = -1;

    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;

    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* Invalid Gport */
        return BCM_E_PARAM;
    }
    hw_res = &pfc_deadlock_control->hw_regs_fields;
    if (pfc_deadlock_control->cosq_inv_mapping_get) {
        rv = pfc_deadlock_control->cosq_inv_mapping_get(unit, gport, -1,
                                          BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                          &local_port, &priority);
    } else {
        /* Mapping function not defined */
        return BCM_E_INIT;
    }
    if (rv != BCM_E_NONE) {
        if (rv == BCM_E_NOT_FOUND) {
            /* Given GPort/Queue has No Input priority mapping */
            return BCM_E_RESOURCE;
        }
        return rv;
    }
    pfc_deadlock_pri_config = _BCM_PFC_DEADLOCK_CONFIG(unit, priority);


    BCM_IF_ERROR_RETURN(
        _bcm_td3_pfc_deadlock_hw_cos_index_get(unit, priority, &hw_cos_index));
    if (hw_cos_index == -1) {
        /* No matching or free hw_index available for use */
        return BCM_E_RESOURCE;
    }
    if (local_port > BCM_TD3_PFC_DEADLOCK_SPLIT0_MAX_PORT) {
        reg = hw_res->timer_en[1];
    } else {
        reg = hw_res->timer_en[0];
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    mmu_port %= SOC_TD3_MMU_PORT_STRIDE;

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, hw_cos_index, &rval64));
    if (operation == _bcmPfcDeadlockOperGet) { /* GET */
        if (enable_status) {
            *enable_status =
                BCM_PBMP_MEMBER(pfc_deadlock_pri_config->deadlock_ports,
                            local_port);
        }
#if 0
        /* TD3TBD Need to update correct enable bitmap */
        if (config) {
            if (mmu_port < 32) {
                config->enable =
                    (COMPILER_64_LO(rval64) & (1 << mmu_port)) ? TRUE : FALSE;
            } else {
                config->enable =
                    (COMPILER_64_HI(rval64) & (1 << (mmu_port - 32))) ?
                                 TRUE : FALSE;
            }
        }
#endif
        return BCM_E_NONE;
    } else { /* _bcmPfcDeadlockOperSet */
        temp_val_lo = COMPILER_64_LO(rval64);
        temp_val_hi = COMPILER_64_HI(rval64);

#if 0
        /* TD3TBD Need to update correct enable bitmap */
        if (config->enable) {
            if (mmu_port < 32) {
                temp_val_lo |= (1 << mmu_port);
            } else {
                temp_val_hi |= (1 << (mmu_port - 32));
            }
            BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports,
                              local_port);
        } else {
            if (mmu_port < 32) {
                temp_val_lo &= ~(1 << mmu_port);
            } else {
                temp_val_hi &= ~(1 << (mmu_port - 32));
            }

            SOC_PBMP_PORT_REMOVE(pfc_deadlock_pri_config->enabled_ports,
                                 local_port);
        }
#endif

        COMPILER_64_SET(rval64, temp_val_hi, temp_val_lo);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, hw_cos_index, rval64));
        if (SOC_PBMP_IS_NULL(pfc_deadlock_pri_config->enabled_ports)) {
            /*First for that port, set ignore_pfc_xoff = 0 (per port reg) */
            BCM_IF_ERROR_RETURN(
                _bcm_td3_pfc_deadlock_ignore_pfc_xoff_clear(unit, hw_cos_index, local_port));
            pfc_deadlock_control->hw_cos_idx_inuse[hw_cos_index] = FALSE;
            pfc_deadlock_pri_config->flags &= ~_BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = -1;
            pfc_deadlock_control->pfc_pri2cos[priority] = -1;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_pfc_deadlock_update_cos_used(unit));
    return BCM_E_NONE;
}

/* Chip specific init and assign default values */
int
_bcm_td3_pfc_deadlock_init(int unit)
{
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_hw_resorces_t *hw_res = NULL;
    int i;
    static soc_reg_t timer_count[1] = {
        Q_SCHED_DD_TIMERr
    };
    static soc_reg_t timer_en_regs[2] = {
        Q_SCHED_DD_TIMER_ENABLE_SPLIT0r, Q_SCHED_DD_TIMER_ENABLE_SPLIT1r
    };
    static soc_reg_t timer_status_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_SPLIT0r, Q_SCHED_DD_TIMER_STATUS_SPLIT1r
    };
    static soc_reg_t timer_mask_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT0r, Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT1r
    };
    static soc_reg_t config_regs[3] = {
        Q_SCHED_DD_CHIP_CONFIG0r, Q_SCHED_DD_CHIP_CONFIG1r, Q_SCHED_DD_CHIP_CONFIG2r
    };

    static soc_field_t deadlock_time_field[10] = {
        DD_TIMER_INIT_VALUE_COS_0f, DD_TIMER_INIT_VALUE_COS_1f,
        DD_TIMER_INIT_VALUE_COS_2f, DD_TIMER_INIT_VALUE_COS_3f,
        DD_TIMER_INIT_VALUE_COS_4f, DD_TIMER_INIT_VALUE_COS_5f,
        DD_TIMER_INIT_VALUE_COS_6f, DD_TIMER_INIT_VALUE_COS_7f,
        DD_TIMER_INIT_VALUE_COS_8f, DD_TIMER_INIT_VALUE_COS_9f
    };

    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    pfc_deadlock_control->pfc_deadlock_cos_max = PFC_DEADLOCK_MAX_COS;
    pfc_deadlock_control->cosq_inv_mapping_get = _bcm_td3_cosq_inv_mapping_get;

    hw_res->timer_count[0]  = timer_count[0];
    hw_res->timer_status[0] = timer_status_regs[0];
    hw_res->timer_status[1] = timer_status_regs[1];
    hw_res->timer_mask[0]   = timer_mask_regs[0];
    hw_res->timer_mask[1]   = timer_mask_regs[1];
    hw_res->timer_en[0]     = timer_en_regs[0];
    hw_res->timer_en[1]    = timer_en_regs[1];

    for (i = 0; i < PFC_DEADLOCK_MAX_COS; i++) {
        hw_res->time_init_val[i] = deadlock_time_field[i];
    }
    hw_res->port_config = Q_SCHED_DD_PORT_CONFIGr;
    hw_res->chip_config[0] = config_regs[0];
    hw_res->chip_config[1] = config_regs[1];
    /* Check the TD3 chip config registers
     * hw_res->chip_config[2] = config_regs[2];
     */
    /* Q_SCHED_DD_CHIP_CONFIG1 */
    hw_res->time_unit_field = DD_TIMER_TICK_UNITf;
    /* Q_SCHED_DD_CHIP_CONFIG2 */
    hw_res->recovery_action_field = IGNORE_PFC_OFF_PKT_DISCARDf;
    /* Q_SCHED_DD_TIMER_STATUS_SPLIT0/1 */
    hw_res->deadlock_status_field = DEADLOCK_DETECTEDf;
    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT3_SUPPORT */
