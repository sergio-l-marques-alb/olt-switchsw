/** \file ctest_dnx_stif_kbp_voq_db.c
 *
 * ctests for stif with kbp for voq database counting
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_kbp_stif.h"
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>

/** bcm */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <bcm/stack.h>
#include <bcm/stat.h>
#include <soc/dnx/kbp/kbp_common.h>

/** sal */
#include <sal/appl/sal.h>

#if defined (INCLUDE_KBP)
/** kbp */
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/kbp_pcie.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/tap.h>

typedef struct
{
    uint32 object_id;
    uint32 counter_set_offset;
    uint64 packets_value;
    uint64 expected_packets;
    uint64 bytes_value;
    uint64 expected_bytes;
} dnx_kbp_stif_element_ts;

shr_error_e
dnx_kbp_stif_counters_compare(
    int unit,
    int nof_elements,
    dnx_kbp_stif_element_ts * element_arr)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);
    for (i = 0; i < nof_elements; i++)
    {
        if ((COMPILER_64_HI(element_arr[i].packets_value) != COMPILER_64_HI(element_arr[i].expected_packets)) ||
            (COMPILER_64_LO(element_arr[i].packets_value) != COMPILER_64_LO(element_arr[i].expected_packets)))
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Counter Verification Failed: expected {0x%X,0x%X} packets, read from kbp {0x%X,0x%X} packets. (object_id=%d, counter_set_offset=%d) \n",
                         COMPILER_64_HI(element_arr[i].expected_packets),
                         COMPILER_64_LO(element_arr[i].expected_packets), COMPILER_64_HI(element_arr[i].packets_value),
                         COMPILER_64_LO(element_arr[i].packets_value), element_arr[i].object_id,
                         element_arr[i].counter_set_offset);
        }
        if ((COMPILER_64_HI(element_arr[i].bytes_value) != COMPILER_64_HI(element_arr[i].expected_bytes)) ||
            (COMPILER_64_LO(element_arr[i].bytes_value) != COMPILER_64_LO(element_arr[i].expected_bytes)))
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Counter Verification Failed: expected {0x%X,0x%X} bytes, read from kbp {0x%X,0x%X} bytes. (object_id=%d, counter_set_offset=%d) \n",
                         COMPILER_64_HI(element_arr[i].expected_bytes), COMPILER_64_LO(element_arr[i].expected_bytes),
                         COMPILER_64_HI(element_arr[i].bytes_value), COMPILER_64_LO(element_arr[i].bytes_value),
                         element_arr[i].object_id, element_arr[i].counter_set_offset);
        }
        LOG_CLI((BSL_META
                 ("counter verification PASS: object_id=%d, offset=%d, nof_packets={0x%X, 0x%X} nof_bytes={0x%X, 0x%X} \n"),
                 element_arr[i].object_id, element_arr[i].counter_set_offset,
                 COMPILER_64_HI(element_arr[i].packets_value), COMPILER_64_LO(element_arr[i].packets_value),
                 COMPILER_64_HI(element_arr[i].bytes_value), COMPILER_64_LO(element_arr[i].bytes_value)));
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_kbp_stif_read(
    int unit,
    struct kbp_device *device,
    struct kbp_tap_db *db,
    int nof_elements,
    dnx_kbp_stif_element_ts * element_arr)
{
    kbp_status status;
    int element_idx, is_complete;
    SHR_FUNC_INIT_VARS(unit);

    status = kbp_tap_db_read_initiate(db);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_read_initiate failed: %s\n", kbp_get_status_string(status));
    }
    is_complete = 0;
    do
    {
        status = kbp_device_scrub_tap_dma_buffer(device);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_is_read_complete(db, &is_complete);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_is_read_complete failed: %s\n", kbp_get_status_string(status));
        }
    }
    while (is_complete == 0);

    /*
     * Iterate through queue id, each counter within queue id, get
     * the count value.
     */
    for (element_idx = 0; element_idx < nof_elements; element_idx++)
    {
        status = kbp_tap_db_entry_pair_get_value(db, element_arr[element_idx].object_id,
                                                 element_arr[element_idx].counter_set_offset,
                                                 &element_arr[element_idx].packets_value,
                                                 &element_arr[element_idx].bytes_value);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_entry_pair_get_value failed: %s\n", kbp_get_status_string(status));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - set specifc IN-DP for a cetain in port, using ACL rule
* \param [in] unit             - Device ID
* \param [in] port             - in port
* \param [in] dp               - mapped DP
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_kbp_stif_acl_run(
    int unit,
    int port,
    int dp)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_t fe_id;
    void *dest_char;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    int presel_id = 10;
    bcm_field_context_t context_id = 10;
    int action_info_pri;
    SHR_FUNC_INIT_VARS(unit);

    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    dest_char = &(fg_info.name[0]);
    switch(dp)
    {
        case BCM_FIELD_COLOR_GREEN:
            sal_strncpy(dest_char, "DpG", sizeof(fg_info.name));            
        break;
        case BCM_FIELD_COLOR_YELLOW:
            sal_strncpy(dest_char, "DpY", sizeof(fg_info.name));            
        break;
        case BCM_FIELD_COLOR_RED:
            sal_strncpy(dest_char, "DpR", sizeof(fg_info.name));            
        break;
        case BCM_FIELD_COLOR_BLACK:
            sal_strncpy(dest_char, "DpB", sizeof(fg_info.name));            
        break;        
    }        

    fg_info.name[sizeof(fg_info.name) - 1] = 0;

    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    /** Create context */
    stage = bcmFieldStageIngressPMF1;
    bcm_field_context_info_t_init(&context_info);
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, stage, &context_info, &context_id));

    /** Create presel_entry to map relevant traffic to the context */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = stage;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    presel_entry_data.qual_data[0].qual_mask = 0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    action_info_pri = BCM_FIELD_ACTION_PRIORITY(0, fg_id % 16);
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].priority = action_info_pri;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /** Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    BCM_GPORT_LOCAL_SET(ent_info.entry_qual[0].value[0], port);
    ent_info.entry_qual[0].mask[0] = 0x1FF;
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = dp;
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &ent_info, &fe_id));
    LOG_CLI((BSL_META("Ingress Acl Entry add: fg_id=(0x%x), fe_id=(0x%x), InPort=(0x%x), dp=(0x%x)\n"),
             fg_id, fe_id, port, dp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configuration of dnx and kbp stif test. gather statistics for voq.
 *  configuration:
 *  One JR2, two cores configuration, 2 ports of 200G per core, billing mode.
 *  Count Voqs per core.
 *  2 databases. one for each core.
 *  Each database holds counter_set=8, as follow: 
 *  Offset-0: Forward, DP=0
 *  Offset-1: Forward, DP=1
 *  Offset-2: Forward, DP=2
 *  Offset-3: Forward, DP=3
 *  Offset-4: Drop, DP=0
 *  Offset-5: Drop, DP=1
 *  Offset-6: Drop, DP=2
 *  Offset-7: Drop, DP=3
 *  Ingress record format:
 *  Bits 0..15: queue_id.
 *  Bits 16: disposition (forward=0, drop=1)
 *  Bits 17..18: DP
 *  Bits 19..63: hole
 *  Bits 64..77: packet size
 *  Bits 78..79: record type. 
 * \param [in] unit         - Number of hardware unit used
 * \param [in] args         - args
 * \param [in] sand_control - sand_control
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
static shr_error_e
dnx_kbp_stif_voq_db_test(
    int unit)
{
    struct kbp_device *device = NULL;
    rhhandle_t ctest_soc_set_h = NULL;    
    struct kbp_device *device_th[2];
    struct kbp_tap_instruction *inst_th[2];
    struct kbp_tap_record *rec_th[2];
    struct kbp_tap_db *db[2];
    struct kbp_tap_db_pointer *ptr[2];
    struct kbp_tap_db_attribute *attr[2];
    int ing_rec_size, null_rec_size, packet_size;
    int attr_value, counter_offset;
    int db_id, nof_db;
    int num_queues = dnx_data_ipq.queues.nof_queues_get(unit);
    int hole_len, ptr_len, attr_len, set_size = 8;
    kbp_status status;
    bcm_port_t from_port[2] = { 201, 200 };
    bcm_port_t to_port[2] = { 203, 202 };
    bcm_color_t dp[2] = { BCM_FIELD_COLOR_YELLOW, BCM_FIELD_COLOR_BLACK };
    rhhandle_t packet_h = NULL;
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);
    bcm_stat_stif_source_t source;
    bcm_stat_stif_record_format_element_t ingress_record_format_elements[4];
    int nof_elements_ingress = 4;
    dnx_kbp_stif_element_ts kbp_stif_elements[64];
    bcm_switch_kbp_info_t kbp_info;

    ctest_soc_property_t ctest_soc_property_1[] = {
        {"ucode_port_33", "CCGE4:core_0:stat"},
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "96"},
        {"stat_if_idle_reports_present", "1"},
        {"port_init_speed_ext_stat0", "200000.4lanes"},        
        {"port_init_speed_ext_stat2", "200000.4lanes"},        
        {"port_init_cl72_ext_stat0", "1"},        
        {"port_init_cl72_ext_stat2", "1"},        
        {"appl_enable_l2", "0"},
        {NULL}
    };
    ctest_soc_property_t ctest_soc_property_2[] = {
        {"ucode_port_33", "CCGE4:core_0:stat"},
        {"ucode_port_34", "CCGE21:core_1:stat"},
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "80"},
        {"stat_if_idle_reports_present", "1"},
        {"port_init_speed_ext_stat0", "200000.4lanes"},        
        {"port_init_speed_ext_stat2", "200000.4lanes"},        
        {"port_init_cl72_ext_stat0", "1"},        
        {"port_init_cl72_ext_stat2", "1"},   
        {"appl_enable_l2", "0"},
        {NULL}
    };
    ctest_soc_property_t *ctest_soc_property;
    SHR_FUNC_INIT_VARS(unit);
    /** init soc properties */
    ctest_soc_property = (nof_cores == 1) ? ctest_soc_property_1 : ctest_soc_property_2;

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    

    /*
     * dnx configuration 
     */
    /** disable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 0));
    /** connect IngressEnqueue source to the logical port */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));
    if (nof_cores > 1)
    {
        source.core = 1;
        source.src_type = bcmStatStifSourceIngressEnqueue;
        SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 34));
    }

    /** define the record format */
    /**
     *  
     *  *  INGRESS
     *  record size set to 80b
     *  filled are 36b
     *  ----------------------------------------------------------------------------------------
     * |RecordType (2b)| PacketSize (14b)| Hole (45b)|DP(2b)|Disposition(1b)|queue_id(16b)
     *  ----------------------------------------------------------------------------------------
     */
    ingress_record_format_elements[0].element_type = bcmStatStifRecordElementIngressQueueNumber;
    ingress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[1].element_type = bcmStatStifRecordElementIngressDispositionIsDrop;
    ingress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[2].element_type = bcmStatStifRecordElementIngressIncomingDropPrecedence;
    ingress_record_format_elements[2].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[3].element_type = bcmStatStifRecordElementPacketSize;
    ingress_record_format_elements[3].mask = BCM_STAT_FULL_MASK;
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_record_format_elements));
    /** enable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 1));

    nof_db = nof_cores < 2 ? nof_cores : 2;
    /*
     * kbp configuration 
     */
    SHR_IF_ERR_EXIT(bcm_switch_kbp_info_get(unit, 0, &kbp_info));
    device = (struct kbp_device *)kbp_info.device_p;


    ing_rec_size = 80;
    null_rec_size = 96;

    /** Set the ingress and null record lengths */
    status = kbp_device_set_property(device, KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE, ing_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(device, KBP_DEVICE_PROP_TAP_NULL_REC_SIZE, null_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    /** Enable counter compression since packet and byte count mode */
    status = kbp_device_set_property(device, KBP_DEVICE_PROP_COUNTER_COMPRESSION, 1);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    /** Create tap databases */
    for (db_id = 0; db_id < nof_db; db_id++)
    {
        /** Port-0 is mapped to Thread-0 and Port-2 is mapped to Thread-1 */
        status = kbp_device_thread_init(device, db_id, &device_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_thread_init failed: %s\n", kbp_get_status_string(status));
        }
        /** Entry type is PAIR since packet and byte count mode */
        status = kbp_tap_db_init(device, db_id, num_queues, KBP_TAP_ENTRY_TYPE_PAIR, set_size, &db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_init failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize pointer */
        ptr_len = 16;
        status = kbp_tap_db_pointer_init(db[db_id], &ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_pointer_add_field(ptr[db_id], "QUEUE_ID", ptr_len);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_pointer(db[db_id], ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_set_pointer failed: %s\n", kbp_get_status_string(status));
        }

        /** Initialize attribute, add fields. attr_len = 1 + 2 */
        attr_len = 3;
        status = kbp_tap_db_attribute_init(db[db_id], &attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_attribute_add_field(attr[db_id], "DISPOSITION", 1);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_attribute_add_field(attr[db_id], "DROP_PRECEDENCE", 2);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_attribute(db[db_id], attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * Attribute value to counter offset mapping:
         *
         * Offset-0: Forward, DP = 0, Offset-1: Forward, DP = 1
         * Offset-2: Forward, DP = 2, Offset-3: Forward, DP = 3
         * Offset-4: Drop,    DP = 0, Offset-5: Drop, DP = 1
         * Offset-6: Drop,    DP = 2, Offset-7: Drop, DP = 3
         */
        for (attr_value = 0, counter_offset = 0; attr_value < set_size; attr_value++, counter_offset++)
        {
            status = kbp_tap_db_map_attr_value_to_entry(db[db_id], attr_value, counter_offset);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_map_attr_value_to_entry failed: %s\n",
                             kbp_get_status_string(status));
            }
        }
        /*
         * instructions, on Th-i
         * The ingress record looks as follows:
         *
         * Bits 78..79: record type
         * Bits 64..77: packet size
         * Bits 19..63: hole
         * Bits 17..18: DP
         * Bits 16: disposition (forward = 0, drop = 1)
         * Bits 0..15: queue_id.
         */
        status = kbp_tap_instruction_init(device_th[db_id], db_id, KBP_TAP_INSTRUCTION_TYPE_INGRESS, &inst_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(device_th[db_id], KBP_TAP_RECORD_TYPE_INGRESS, &rec_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
        packet_size = 14;
        hole_len = ing_rec_size - (2 + packet_size + attr_len + ptr_len);

        status = kbp_tap_record_add_field(rec_th[db_id], "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "PACKET_SIZE", packet_size, KBP_TAP_RECORD_FIELD_VALUE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "HOLE", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "DROP_PRECEDENCE", 2, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "QUEUE_ID", ptr_len, KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * Associate record with the instruction. Add database and finally install the instruction.
         */
        status = kbp_tap_instruction_set_record(inst_th[db_id], rec_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_add_db(inst_th[db_id], db[db_id], 0, 0x7FFF);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_install(inst_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
        }
    }

   /** Lock device configuration */
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    /*
     * Start pumping the Stats traffic. Periodically call 
     * DMA scrub API to clear dynamically evicted counters.
     */
    status = kbp_device_scrub_tap_dma_buffer(device);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
    }

    /** handle traffic +  read counterd and verify for each database*/
    for (db_id = 0; db_id < nof_db; db_id++)
    {
        /** map in port to TC and DP */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_run(unit, from_port[db_id], dp[db_id]));
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port[db_id], to_port[db_id], 1));
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h, "PTCH_2.PP_SSP", (uint32 *) (&from_port[db_id]), 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port[db_id], packet_h, FALSE));

        /** read counters */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get
                        (unit, 0, to_port[db_id], (int *) &kbp_stif_elements[0].object_id));

        for (int i = 0; i < set_size; i++)
        {
            kbp_stif_elements[i].counter_set_offset = i;
            kbp_stif_elements[i].object_id = kbp_stif_elements[0].object_id;
            /** offset= {disposition(1), dp(2)} */
            /** for first db: expected offset=1, disposition=forward, dp=yellow */
            /** for second db: expected offset=7, disposition=drop, dp=black */
            kbp_stif_elements[i].expected_packets = ((i == 1 && db_id == 0) || (i == 7 && db_id == 1)) ? 1 : 0;
            kbp_stif_elements[i].expected_bytes = ((i == 1 && db_id == 0) || (i == 7 && db_id == 1)) ? 64 : 0;
        }
        SHR_IF_ERR_EXIT(dnx_kbp_stif_read(unit, device, db[db_id], set_size, kbp_stif_elements));

        /*
         * verify counters 
         */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size, kbp_stif_elements));
    }

    /** Database destroy */
    for (db_id = 0; db_id < nof_db; db_id++)
    {
        status = kbp_tap_db_destroy(db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_destroy failed: %s\n", kbp_get_status_string(status));
        }
        /** instruction destroy */
        status = kbp_tap_instruction_destroy(inst_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_destroy failed: %s\n", kbp_get_status_string(status));
        }
    }

exit:

    diag_sand_packet_free(unit, packet_h);
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_stif_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_id;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("id", test_id);
    switch (test_id)
    {
        case 0:
            SHR_IF_ERR_EXIT(dnx_kbp_stif_voq_db_test(unit));
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "test ID=%d is not supported: \n", test_id);
    }

exit:
    SHR_FUNC_EXIT;
}

#else

shr_error_e
dnx_kbp_stif_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT(_SHR_E_UNAVAIL, "This test is not supported on compilations without KBP: \n");

exit:
    SHR_FUNC_EXIT;
}

#endif /** defined (INCLUDE_KBP) */

static sh_sand_option_t dnx_kbp_stif_test_list_options[] = {
    {"id", SAL_FIELD_TYPE_UINT32, "test id", "0"},
    {NULL}
};

static sh_sand_man_t dnx_kbp_stif_test_man = {
    .brief = "test kbp and dnx devices for statistics gathering using statistic interface",
    .full =
        "Test-0 configuration: ingress voq database per core. record_size=80. counter_set_size=8 (dp*disposition). packets and bytes \n"
        "Test-1 configuration: NONE \n",
    .synopsis = NULL,
    .examples = "ctest kbp stif test id=0"
};

/** 
 * \brief DNX STIF-KBP ctests * List of the supported ctests, pointer to command function and 
 * command usage function. This is the entry point for STIF-KBP  
 * ctest commands  */
sh_sand_cmd_t sh_dnx_kbp_stif_test_cmds[] = {
    {"test", dnx_kbp_stif_test, NULL, dnx_kbp_stif_test_list_options, &dnx_kbp_stif_test_man, NULL, NULL, 
        SH_CMD_SKIP_EXEC},
    {NULL}
};
