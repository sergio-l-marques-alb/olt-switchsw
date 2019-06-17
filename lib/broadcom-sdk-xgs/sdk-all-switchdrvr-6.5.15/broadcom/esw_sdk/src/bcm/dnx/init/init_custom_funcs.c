
/** \file init_custom_funcs.c
 * Custom functions to be used to initialize DNX memories. 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MEM

#include <soc/mem.h> 
#include <shared/shrextend/shrextend_error.h>
#include <soc/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <bcm_int/dnx/field/field_actions.h>

/*
 * The following functions are used to initialize memories of 
 * Jericho2
 *
 * For other devices,
 * a different version of the following functions might be required
 */

/** See .h file */
shr_error_e
dnx_init_custom_sch_flow_to_fip_mapping_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, entry_data, DEVICE_NUMBERf,
                        dnx_data_device.general.invalid_fap_id_get(unit));

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_sch_port_schduler_weights_psw_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    /*
     * set all weight to default init value 
     */
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_0_WEIGHTf, 0x1);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_1_WEIGHTf, 0x2);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_2_WEIGHTf, 0x4);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_3_WEIGHTf, 0x8);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_4_WEIGHTf, 0x10);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_5_WEIGHTf, 0x20);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_6_WEIGHTf, 0x40);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_7_WEIGHTf, 0x80);

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_sch_shaper_descr_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int i, nof_fields = 8;

    soc_field_t shaper_mantissa_field[] = {
        PEAK_RATE_MAN_0f,
        PEAK_RATE_MAN_1f,
        PEAK_RATE_MAN_2f,
        PEAK_RATE_MAN_3f,
        PEAK_RATE_MAN_4f,
        PEAK_RATE_MAN_5f,
        PEAK_RATE_MAN_6f,
        PEAK_RATE_MAN_7f
    };

    soc_field_t shaper_max_burst_field[] = {
        MAX_BURST_0f,
        MAX_BURST_1f,
        MAX_BURST_2f,
        MAX_BURST_3f,
        MAX_BURST_4f,
        MAX_BURST_5f,
        MAX_BURST_6f,
        MAX_BURST_7f
    };

    uint32 shaper_mantissa_max_value = 0x3f; /** 6 bits */
    uint32 shaper_max_burst_value = 0x1ff; /** 9 bits */

    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    /*
     * set all shapers to maximal possible rate and maximal max_burst 
     * the shaper is unlimited by default 
     */
    for (i = 0; i < nof_fields; i++)
    {
        /** set the shaper rate */
        soc_mem_field32_set(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, entry_data, shaper_mantissa_field[i],
                            shaper_mantissa_max_value);
        /** set the shaper max burst */
        soc_mem_field32_set(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, entry_data, shaper_max_burst_field[i],
                            shaper_max_burst_value);
    }

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_sch_flow_descr_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    /** set all flows to be attached to reserved SE */
    soc_mem_field32_set(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, entry_data, SCH_NUMBERf,
                        dnx_data_sch.flow.default_credit_source_se_id_get(unit));

    /*
     * set cos value matching to dpp code 
     * could be set to 0, when all ported code is re-implemented
     */
    soc_mem_field32_set(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, entry_data, COSf,
                        dnx_data_sch.se.default_cos_get(unit));
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_fabric_pcp_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
    {
        uint32 default_packing_mode = dnx_data_fabric.cell.default_packing_mode_get(unit);
        uint32 packing_config = 0;
        int field_length = soc_mem_field_length(unit, IPT_PCP_CFGm, PACKING_CONFf);
        int offset = 0;

        /*
         * Packing config per destination FAP.
         * Each line in the table consists of 16 FAPS.
         * Set continuous packing by default to all the FAPs.
         * (2 bits for a fap: 0x0 - disabled; 0x1 - simple packing; 0x2 - continuous packing)
         */
        for (offset = 0; offset < field_length; offset += 2)
        {
            SHR_BITCOPY_RANGE(&packing_config, offset, &default_packing_mode, 0, 2);
        }
        soc_mem_field_set(unit, IPT_PCP_CFGm, entry_data, PACKING_CONFf, &packing_config);
    }
    else
    {
        /** Do nothing. Assuming entry_data is 0 */
    }

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_erpp_tm_pp_dsp_ptr_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ERPP_TM_PP_DSP_PTR_TABLEm, entry_data, OUT_TM_PORTf,
                        dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
    soc_mem_field32_set(unit, ERPP_TM_PP_DSP_PTR_TABLEm, entry_data, DST_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_erpp_prp_pp_dsp_ptr_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ERPP_PRP_PP_DSP_PTR_TABLEm, entry_data, OUT_TM_PORTf,
                        dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
    soc_mem_field32_set(unit, ERPP_PRP_PP_DSP_PTR_TABLEm, entry_data, DST_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_etppa_dsp_data_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ETPPA_DSP_DATA_TABLEm, entry_data, OUT_TM_PORTf,
                        dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
    soc_mem_field32_set(unit, ETPPA_DSP_DATA_TABLEm, entry_data, DST_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_ippc_fes_2nd_instruction_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, IPPC_FES_2ND_INSTRUCTIONm, entry_data, ACTION_TYPEf, DNX_FIELD_ACTION_ID_INVALID);

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_ippd_fes_2nd_instruction_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, IPPD_FES_2ND_INSTRUCTIONm, entry_data, ACTION_TYPEf, DNX_FIELD_ACTION_ID_INVALID);

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_erpp_fes_2nd_instruction_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ERPP_FES_2ND_INSTRUCTIONm, entry_data, ACTION_TYPEf, DNX_FIELD_ACTION_ID_INVALID);

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_etppa_prp_fes_program_table_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int action_type_field_index;
    int nof_action_type_fields;
    soc_field_t action_type_fields[] = {
        ACTION_TYPE_0f,
        ACTION_TYPE_1f,
        ACTION_TYPE_2f,
        ACTION_TYPE_3f,
        ACTION_TYPE_4f,
        ACTION_TYPE_5f,
        ACTION_TYPE_6f,
        ACTION_TYPE_7f
    };

    SHR_FUNC_INIT_VARS(unit);

    nof_action_type_fields = 8;

    for (action_type_field_index = 0; action_type_field_index < nof_action_type_fields; action_type_field_index++)
    {
        soc_mem_field32_set(unit, ETPPA_PRP_FES_PROGRAM_TABLEm, entry_data, action_type_fields[action_type_field_index],
                            34);
    }

    SHR_FUNC_EXIT;
}
