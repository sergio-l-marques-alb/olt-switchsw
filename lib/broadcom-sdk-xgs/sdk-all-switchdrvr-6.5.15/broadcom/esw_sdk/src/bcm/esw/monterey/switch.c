/*
 * $Id: switch.c,v 6.5.14 April 10 2018 Rajesh Thakur Exp $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Module : Switch
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/monterey.h>
#include <soc/soc_ser_log.h>

#include <bcm_int/esw/xgs4.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/monterey.h>
#include <bcm/switch.h>

int
bcm_mn_switch_control_set(int unit,
                          bcm_switch_control_t type,
                          int arg)
{
    soc_field_t  field;
    uint64       rval64;
    uint64       val64 = COMPILER_64_INIT(0, 0);
    uint32       val32 = 0, val32_valid = 1;

    switch (type) {
        case bcmSwitchIomStartTime:
            if (arg <= 0) {
                return BCM_E_PARAM;
            }
            COMPILER_64_ZERO(val64);
            COMPILER_64_SET(val64, 0, arg);
            COMPILER_64_UMUL_32(val64, 1000);
            field = SAMPLING_WND_START_TIMEf;
            val32_valid = 0;
            break;
        case bcmSwitchIomEpochTime:
            if (arg <=0) {
                return BCM_E_PARAM;
            }
            switch (arg) {
                case 100:
                    val32 = 0;
                    break;
                case 1000:
                    val32 = 1;
                    break;
                case 10000:
                    val32 = 2;
                    break;
                case 100000:
                    val32 = 3;
                    break;
                case 1000000:
                    val32 = 4;
                    break;
                case 66:
                    val32 = 5;
                    break;
                case 500:
                    val32 = 6;
                    break;
                case 133:
                    val32 = 7;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_COMMON,
                             (BSL_META_U(unit,
                             "ERROR: Invalid IomEpochTime %d. Allowed values are "
                             "66, 100, 133, 500, 1000, 10000, 100000 and 1000000\n"), arg));
                    return BCM_E_PARAM;
            }
            field = EPOCH_DURATIONf;
            break;
        case bcmSwitchIomEnable:
            val32 = (arg ? 1 : 0);
            field = ENABLEf;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    SOC_IF_ERROR_RETURN(READ_PGW_OBM_MONITOR_CONFIGr(unit, &rval64));
    if (val32_valid) {
        soc_reg64_field32_set(unit, PGW_OBM_MONITOR_CONFIGr, &rval64,
                              field, val32);
    } else {
        soc_reg64_field_set(unit, PGW_OBM_MONITOR_CONFIGr, &rval64,
                            field, val64);
    }
    SOC_IF_ERROR_RETURN(WRITE_PGW_OBM_MONITOR_CONFIGr(unit, rval64));
    return BCM_E_NONE;
}

int
bcm_mn_switch_control_get(int unit,
                          bcm_switch_control_t type,
                          int *arg)
{
    uint64       rval64;
    uint64       val64;
    uint32       val32;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(READ_PGW_OBM_MONITOR_CONFIGr(unit, &rval64));
    switch (type) {
        case bcmSwitchIomStartTime:
            val64 = soc_reg64_field_get(unit, PGW_OBM_MONITOR_CONFIGr, rval64,
                                        SAMPLING_WND_START_TIMEf);
            COMPILER_64_UDIV_32(val64, 1000);
            COMPILER_64_TO_32_LO(val32, val64);
            *arg = val32;
            break;
        case bcmSwitchIomEpochTime:
            val32 = soc_reg64_field32_get(unit, PGW_OBM_MONITOR_CONFIGr, rval64,
                                          EPOCH_DURATIONf);
            switch (val32) {
                case 0:
                    *arg = 100;
                    break;
                case 1:
                    *arg = 1000;
                    break;
                case 2:
                    *arg = 10000;
                    break;
                case 3:
                    *arg = 100000;
                    break;
                case 4:
                    *arg = 1000000;
                    break;
                case 5:
                    *arg = 66;
                    break;
                case 6:
                    *arg = 500;
                    break;
                case 7:
                    *arg = 133;
                    break;
                default:
                    return BCM_E_PARAM;
            }
            break;
        case bcmSwitchIomEnable:
            *arg = soc_reg64_field32_get(unit, PGW_OBM_MONITOR_CONFIGr, rval64,
                                         ENABLEf);
            break;
        case bcmSwitchIomStatus:
            *arg = soc_reg64_field32_get(unit, PGW_OBM_MONITOR_CONFIGr, rval64,
                                         CONFIG_ERRORf);
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

int
bcm_mn_switch_control_port_set(int unit,
                               bcm_port_t port,
                               bcm_switch_control_t type,
                               int arg)
{
    soc_field_t  field;
    soc_reg_t    reg;
    uint32       rval32, val, reg32 = 0;
    uint64       rval64;
    soc_info_t   *si;
    int          pgw, pgw_inst, subport, obm, phy_port, max_val;

    static const soc_reg_t obm_bucket_regs[] = {
        IDB_OBM0_BUCKET_COUNT_CONFIGr, IDB_OBM1_BUCKET_COUNT_CONFIGr,
        IDB_OBM2_BUCKET_COUNT_CONFIGr, IDB_OBM3_BUCKET_COUNT_CONFIGr,
        IDB_OBM4_BUCKET_COUNT_CONFIGr, IDB_OBM5_BUCKET_COUNT_CONFIGr,
        IDB_OBM6_BUCKET_COUNT_CONFIGr, IDB_OBM7_BUCKET_COUNT_CONFIGr
    };
    static const soc_reg_t obm_monitor_regs[] = {
        IDB_OBM0_MONITOR_STATS_CONFIGr, IDB_OBM1_MONITOR_STATS_CONFIGr,
        IDB_OBM2_MONITOR_STATS_CONFIGr, IDB_OBM3_MONITOR_STATS_CONFIGr,
        IDB_OBM4_MONITOR_STATS_CONFIGr, IDB_OBM5_MONITOR_STATS_CONFIGr,
        IDB_OBM6_MONITOR_STATS_CONFIGr, IDB_OBM7_MONITOR_STATS_CONFIGr
    };


    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MACSEC_PORT(unit, port)) {
        /*  Do not allow programming on non-idb ports */
        return BCM_E_PARAM;
    }
    si = &SOC_INFO(unit);
    obm = si->port_serdes[port];
    obm %= _MN_TSCS_PER_PGW;

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        /* Port is invalid */
        return BCM_E_PORT;
    }
    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % _MN_PORTS_PER_TSC;

    switch (type) {
        case bcmSwitchIomSamplingType:
            reg = obm_bucket_regs[obm];
            field = BUCKET_COUNT_SELECTf;
            val = (arg ? 1 : 0);
            break;
        case bcmSwitchIomSamplingMaxUsageType:
            reg = obm_monitor_regs[obm];
            field = MAX_USAGE_SELECTf;
            val = (arg ? 1 : 0);
            reg32 = 1;
            break;
        case bcmSwitchIomSamplingMaxUsageReset:
            reg = obm_monitor_regs[obm];
            field = RESET_MAX_USAGE_PER_EPOCHf;
            val = (arg ? 1 : 0);
            reg32 = 1;
            break;
        case bcmSwitchIomSamplingSelectDropType:
            reg = obm_monitor_regs[obm];
            field = DROP_COUNT_SELECTf;
            if (arg > 4) {
                return BCM_E_PARAM;
            }
            val = arg;
            reg32 = 1;
            break;
        case bcmSwitchIomMaxPacketSizeBucketM:
            reg = obm_bucket_regs[obm];
            field = BUCKET_M_RANGE_MAXf;
            val = arg;
            break;
        case bcmSwitchIomMinPacketSizeBucketB:
            reg = obm_bucket_regs[obm];
            field = BUCKET_B_RANGE_MINf;
            val = arg;
            break;
        case bcmSwitchIomMaxPacketSizeBucketB:
            reg = obm_bucket_regs[obm];
            field = BUCKET_B_RANGE_MAXf;
            val = arg;
            break;
        case bcmSwitchIomMinPacketSizeBucketC:
            reg = obm_bucket_regs[obm];
            field = BUCKET_C_RANGE_MINf;
            val = arg;
            break;
        case bcmSwitchIomMaxPacketSizeBucketC:
            reg = obm_bucket_regs[obm];
            field = BUCKET_C_RANGE_MAXf;
            val = arg;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    max_val = (1 << soc_reg_field_length(unit, reg, field)) - 1;
    if (val > max_val) {
        return BCM_E_PARAM;
    }
    if (reg32) {
        /* 32 bit reg */
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
        soc_reg_field_set(unit, reg, &rval32, field, val);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
    } else {
        /* 64 bit reg */
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, field, val);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, 0, rval64));
    }
    return BCM_E_NONE;
}

int
bcm_mn_switch_control_port_get(int unit,
                               bcm_port_t port,
                               bcm_switch_control_t type,
                               int *arg)
{
    soc_field_t  field;
    soc_reg_t    reg;
    uint32       rval32, val, reg32 = 0;
    uint64       rval64;
    soc_info_t   *si;
    int          pgw, pgw_inst, subport, obm, phy_port;

    static const soc_reg_t obm_bucket_regs[] = {
        IDB_OBM0_BUCKET_COUNT_CONFIGr, IDB_OBM1_BUCKET_COUNT_CONFIGr,
        IDB_OBM2_BUCKET_COUNT_CONFIGr, IDB_OBM3_BUCKET_COUNT_CONFIGr,
        IDB_OBM4_BUCKET_COUNT_CONFIGr, IDB_OBM5_BUCKET_COUNT_CONFIGr,
        IDB_OBM6_BUCKET_COUNT_CONFIGr, IDB_OBM7_BUCKET_COUNT_CONFIGr
    };
    static const soc_reg_t obm_monitor_regs[] = {
        IDB_OBM0_MONITOR_STATS_CONFIGr, IDB_OBM1_MONITOR_STATS_CONFIGr,
        IDB_OBM2_MONITOR_STATS_CONFIGr, IDB_OBM3_MONITOR_STATS_CONFIGr,
        IDB_OBM4_MONITOR_STATS_CONFIGr, IDB_OBM5_MONITOR_STATS_CONFIGr,
        IDB_OBM6_MONITOR_STATS_CONFIGr, IDB_OBM7_MONITOR_STATS_CONFIGr
    };


    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MACSEC_PORT(unit, port) || (arg == NULL)) {
        /*  Do not allow programming on non-idb ports */
        return BCM_E_PARAM;
    }
    si = &SOC_INFO(unit);
    obm = si->port_serdes[port];
    obm %= _MN_TSCS_PER_PGW;

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        /* Port is invalid */
        return BCM_E_PORT;
    }
    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % _MN_PORTS_PER_TSC;

    switch (type) {
        case bcmSwitchIomSamplingType:
            reg = obm_bucket_regs[obm];
            field = BUCKET_COUNT_SELECTf;
            break;
        case bcmSwitchIomSamplingMaxUsageType:
            reg = obm_monitor_regs[obm];
            field = MAX_USAGE_SELECTf;
            reg32 = 1;
            break;
        case bcmSwitchIomSamplingMaxUsageReset:
            reg = obm_monitor_regs[obm];
            field = RESET_MAX_USAGE_PER_EPOCHf;
            reg32 = 1;
            break;
        case bcmSwitchIomSamplingSelectDropType:
            reg = obm_monitor_regs[obm];
            field = DROP_COUNT_SELECTf;
            reg32 = 1;
            break;
        case bcmSwitchIomMaxPacketSizeBucketM:
            reg = obm_bucket_regs[obm];
            field = BUCKET_M_RANGE_MAXf;
            break;
        case bcmSwitchIomMinPacketSizeBucketB:
            reg = obm_bucket_regs[obm];
            field = BUCKET_B_RANGE_MINf;
            break;
        case bcmSwitchIomMaxPacketSizeBucketB:
            reg = obm_bucket_regs[obm];
            field = BUCKET_B_RANGE_MAXf;
            break;
        case bcmSwitchIomMinPacketSizeBucketC:
            reg = obm_bucket_regs[obm];
            field = BUCKET_C_RANGE_MINf;
            break;
        case bcmSwitchIomMaxPacketSizeBucketC:
            reg = obm_bucket_regs[obm];
            field = BUCKET_C_RANGE_MAXf;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    if (reg32) {
        /* 32 bit reg */
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
        val = soc_reg_field_get(unit, reg, rval32, field);
    } else {
        /* 64 bit reg */
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, 0, &rval64));
        val = soc_reg64_field32_get(unit, reg, rval64, field);
    }
    *arg = val;
    return BCM_E_NONE;
}

#endif              /* defined(BCM_MONTEREY_SUPPORT) */
