/** \file dnx_hw_overwrites.c
 *
 *   Wrapper functions for SOC HW configuration APIs.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  All Rights Reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_hw_overwrites.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/cmicx.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/dnxc_ha.h>
#include <bcm_int/dnx/rx/rx.h>


#define FIELD_0_5_DEFAULT 61
#define FIELD_6_6_DEFAULT 1
#define FIELD_7_11_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_MPLS
#define FIELD_12_16_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_IPV4
#define FIELD_7_16_DEFAULT (FIELD_7_11_DEFAULT) | (FIELD_12_16_DEFAULT << 5)
#define FIELD_17_24_DEFAULT 0xFF
#define FIELD_25_27_DEFAULT 6
#define FIELD_28_49_DEFAULT 0
#define FIELD_50_51_DEFAULT 2
#define FIELD_52_69_DEFAULT 0
#define FIELD_70_72_DEFAULT 4
#define FIELD_73_80_DEFAULT 1
#define FIELD_81_82_DEFAULT 3
#define FIELD_83_85_DEFAULT 0
#define FIELD_86_88_DEFAULT 4
#define FIELD_89_92_DEFAULT 7
#define FIELD_93_94_DEFAULT 2
#define FIELD_95_97_DEFAULT 0
#define FIELD_98_99_DEFAULT 3
#define FIELD_100_102_DEFAULT 0
#define IPPF_REG_03A0_FIELD_17_17_DEFAULT 0
#define VTT_ST_TWO_LE_SHARED_PDS_STAGE_SELECTORS_DEFAULT 2
#define ITPP_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe
#define ITPPD_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe

shr_error_e
dnx_hw_overwrite_init(
    int unit)
{
    uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 field_data[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32 reg_val;
    uint64 reg64;
    int system_headers_mode;
    uint32 udh_size = 0;
    int ii = 0, field_0 = 0, field_1 = 0;
    uint32 hw_context_id1, hw_context_id2, hw_context_id3;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * OAM overwrites
     */

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        reg_val = OAMP_REG_0127r;
        if (soc_is(unit, J2P_DEVICE))
        {
            reg_val = OAMP_REG_0128r;
        }
        SHR_IF_ERR_EXIT(soc_reg_get(unit, reg_val, _SHR_CORE_ALL, 0, &reg64));

        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_5_5f, 0);

        udh_size = BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_2_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_3_get(unit));
        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_49_55f, udh_size);

        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_8_8f, 1);
        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_47_48f, 3);

        SHR_IF_ERR_EXIT(soc_reg_set(unit, reg_val, _SHR_CORE_ALL, 0, reg64));

        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD_CONTEXT_ID,
                                      DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP, &hw_context_id1);
        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD_CONTEXT_ID,
                                      DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT, &hw_context_id2);
        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD_CONTEXT_ID,
                                      DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2, &hw_context_id3);
        for (ii = 0; ii < 64; ii++)
        {
            SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, -1, ii, data));
            field_0 = soc_mem_field32_get(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data, PROGRAMf);
            field_1 = soc_mem_field32_get(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data, PHP_PERFORMEDf);
            if ((((field_0 >> 1) == hw_context_id1) || ((field_0 >> 1) == hw_context_id2)
                 || ((field_0 >> 1) == hw_context_id3)) && (field_1 == 1))
            {
                soc_mem_field32_set(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data, PHP_PERFORMEDf, 0);
                soc_mem_field32_set(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data,
                                    soc_is(unit, J2P_DEVICE) ? TERMINATION_CONTEXT_PROFILEf : TERMINATION_CONTEXTf, 0);
                soc_mem_field32_set(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data,
                                    soc_is(unit,
                                           J2P_DEVICE) ? TERMINATION_CONTEXT_PROFILE_MASKf : TERMINATION_CONTEXT_MASKf,
                                    0x3f);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, -1, ii, data));
            }
        }
    }


    sal_memset(field_data, 0, sizeof(field_data));
    sal_memset(data, 0x0, sizeof(data));


    reg_val = OAMP_REG_0129r;
    if (soc_is(unit, J2P_DEVICE))
    {
        reg_val = OAMP_REG_012Ar;
    }
    field_data[0] = FIELD_0_5_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_0_5f, field_data);


    field_data[0] = FIELD_6_6_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_6_6f, field_data);


    field_data[0] = FIELD_7_16_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_7_16f, field_data);


    field_data[0] = FIELD_17_24_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_17_24f, field_data);


    field_data[0] = FIELD_25_27_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_25_27f, field_data);


    field_data[0] = FIELD_28_49_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_28_49f, field_data);


    field_data[0] = FIELD_50_51_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_50_51f, field_data);


    field_data[0] = FIELD_52_69_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_52_69f, field_data);


    field_data[0] = FIELD_70_72_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_70_72f, field_data);


    field_data[0] = FIELD_73_80_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_73_80f, field_data);


    field_data[0] = FIELD_81_82_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_81_82f, field_data);


    field_data[0] = FIELD_83_85_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_83_85f, field_data);


    field_data[0] = FIELD_86_88_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_86_88f, field_data);


    field_data[0] = FIELD_89_92_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_89_92f, field_data);


    field_data[0] = FIELD_93_94_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_93_94f, field_data);


    field_data[0] = FIELD_95_97_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_95_97f, field_data);


    field_data[0] = FIELD_98_99_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_98_99f, field_data);


    field_data[0] = FIELD_100_102_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_100_102f, field_data);

    if (soc_is(unit, J2P_DEVICE))
    {
        SHR_IF_ERR_EXIT(WRITE_OAMP_REG_012Ar(unit, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(WRITE_OAMP_REG_0129r(unit, data));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
