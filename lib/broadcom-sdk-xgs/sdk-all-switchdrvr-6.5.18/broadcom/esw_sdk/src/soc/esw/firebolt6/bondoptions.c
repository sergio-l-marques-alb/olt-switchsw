/*
 * $Id: bondoptions.c $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        bondoptions.c
 * Purpose:     Populates Firebolt6 Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_FIREBOLT6_SUPPORT

#include <shared/bitop.h>
#include <soc/iproc.h>
#include <soc/firebolt6.h>
#include <soc/bondoptions.h>


/*
 * Function to conver to Core Clock frequency encodings to
 * real core clock frequencies.
 */
STATIC int
_soc_firebolt6_bond_option_to_core_clk_freq(int unit, int val)
{
    int freq;

    switch (val) {
        case 0x0:
            freq = 893;
            break;
        case 0x1:
            freq = 762;
            break;
        case 0x2:
            freq = 668;
            break;
        case 0x3:
            freq = 587;
            break;
        case 0x4:
            freq = 543;
            break;
        case 0x5:
            freq = 450;
            break;
        case 0x6:
            freq = 400;
            break;

        default:
            freq = 893;
            /* unknown max frequency */
            break;
    }

    return freq;
}

/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_firebolt6_bond_info_init(int unit)
{
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    uint32 addr, val;
    int i, bit_val;

    soc_reg_t soc_fb6_bond_info_regs[] = {
        DMU_PCU_OTP_CONFIG_0r,
        DMU_PCU_OTP_CONFIG_1r,
        DMU_PCU_OTP_CONFIG_2r,
        DMU_PCU_OTP_CONFIG_3r,
        DMU_PCU_OTP_CONFIG_4r,
        DMU_PCU_OTP_CONFIG_5r,
        DMU_PCU_OTP_CONFIG_6r,
        DMU_PCU_OTP_CONFIG_7r,
        DMU_PCU_OTP_CONFIG_8r,
        DMU_PCU_OTP_CONFIG_9r,
        DMU_PCU_OTP_CONFIG_10r,
        DMU_PCU_OTP_CONFIG_11r,
        DMU_PCU_OTP_CONFIG_12r,
        DMU_PCU_OTP_CONFIG_13r,
        DMU_PCU_OTP_CONFIG_14r,
        DMU_PCU_OTP_CONFIG_15r,
        DMU_PCU_OTP_CONFIG_16r,
        DMU_PCU_OTP_CONFIG_17r,
        DMU_PCU_OTP_CONFIG_18r,
        DMU_PCU_OTP_CONFIG_19r,
        DMU_PCU_OTP_CONFIG_20r,
        DMU_PCU_OTP_CONFIG_21r,
        DMU_PCU_OTP_CONFIG_22r,
        DMU_PCU_OTP_CONFIG_23r,
        DMU_PCU_OTP_CONFIG_24r,
        DMU_PCU_OTP_CONFIG_25r,
        DMU_PCU_OTP_CONFIG_26r,
        DMU_PCU_OTP_CONFIG_27r,
        DMU_PCU_OTP_CONFIG_28r,
        DMU_PCU_OTP_CONFIG_29r,
        DMU_PCU_OTP_CONFIG_30r,
        DMU_PCU_OTP_CONFIG_31r
    };
    int reg_num = COUNTOF(soc_fb6_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_fb6_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit | Bool Invert? | Feature Enum | Description */
        {436, 1, socBondInfoBskGroupDepthHalfEntries}, /* BROADSCAN2.0 Group Depth */
        {437, 0, socBondInfoBskDisable}, /* BROADSCAN Disable */
        {438, 0, socBondInfoBskNoPktSampling}, /* BROADSCAN packet sampling disable */
        {439, 0, socBondInfoBskNoHWExportPktBuild},  /* BROADSCAN HW export packet build disable */
        {440, 0, socBondInfoBskNoTCPBidir},  /* BROADSCAN TCP bidir processing disable */
        {441, 0, socBondInfoBskNoDoSAttackVector},  /* BROADSCAN DOS attack vector check disable */
        {442, 0, socBondInfoBskNoMicroFlowPolicer}, /* BROADSCAN micro flow policer disable */
        {443, 0, socBondInfoBskNoALU32Inst2to11}, /* BROADSCAN ALU32 inst 2_to_11 disable */
        {444, 0, socBondInfoBskNoDropAnalysis},    /* BROADSCAN2.0 Drop reason tracking disable */
        {445, 0, socBondInfoBskNoLatencyTracking}, /* BROADSCAN2.0 E2E and Chip Delay tracking disable */
        {446, 0, socBondInfoBskNoAluLatencySupport}, /* BROADSCAN2.0 ALU Delay calculation disable */
        {447, 0, socBondInfoBskNoIATIDTTracking}, /* BROADSCAN2.0 IAT & IDT Delay tracking disable */
        {448, 0, socBondInfoBskNoCongestionTracking}, /* BROADSCAN2.0 MMU congestion level delay trakcing disabled */
        {449, 0, socBondInfoBskNoLoadBalancingTracking}, /* BROADSCAN2.0 Load Balancing tracking disable */
        {450, 0, socBondInfoBskAggregateDisable}, /* BROADSCAN Aggregate tracking disable */
        {-1,  0, -1 }                              /* Always last */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_fb6_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, i * 32, &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);
    SOC_BOND_OPTIONS_COPY(bond_info->mb_bank_num, uint32, bond_info_regval, 28, 33);

    /* Max Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 28, 31);
    bond_info->max_core_freq = _soc_firebolt6_bond_option_to_core_clk_freq(unit, val);

    /* Default Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 32, 35);
    bond_info->default_core_freq = _soc_firebolt6_bond_option_to_core_clk_freq(unit, val);

    /* BROADSCAN Session table depth */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 434, 435);
    if (val == 1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth3Banks);
    } else if (val == 2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth5Banks);
    } else if (val == 3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth9Banks);
    }

    /* Set the feature enable status */
    for(i = 0; i < COUNTOF(feature_enables); i++) {
        bit_val = SHR_BITGET(bond_info_regval, feature_enables[i][0]);
        if (feature_enables[i][1]) {
            /* Boolean invert  */
            bit_val = !bit_val;
        }

        if (bit_val) {
            SOC_BOND_INFO_FEATURE_SET(unit, feature_enables[i][2]);
        }
    }

    return SOC_E_NONE;
}

#endif /* BCM_FIREBOLT6_SUPPORT */

