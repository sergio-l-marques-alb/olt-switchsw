/** \file ingress_congestion_counters_dbal.c
 * $Id$
 *
 * Low level functions accessing DBAL counters tables
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>

#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <soc/dnx/dbal/dbal.h>

/**
 * \brief - Function to configure the VOQ programmable counter 
 *        to track certain VOQ or VOQ bundle
 * 
 */
shr_error_e
dnx_ingress_congestion_voq_programmable_counter_configuration_set(
    int unit,
    int core,
    uint32 base_voq_id,
    uint32 nof_voqs)
{
    uint32 entry_handle_id;
    uint32 voq_mask, mask_max_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get current queue id and its occupancy level   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_ID, INST_SINGLE, base_voq_id);

    /** Get maximum value for the mask field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                                                          DBAL_FIELD_VOQ_MASK,
                                                          FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &mask_max_value));

    /** Choose how many VOQs from the base VOQ will be tracked
     *  when collecting counters info */
    voq_mask = (nof_voqs - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_MASK, INST_SINGLE, voq_mask);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get all VOQ programmable counter 
 *        statistics
 * 
 */
shr_error_e
dnx_ingress_congestion_voq_programmable_counter_info_get(
    int unit,
    int core_id,
    dnx_ingress_congestion_counter_info_t * counter_info)
{
    int counter_size;
    int is_gtimer_enabled = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get SRAM Enqueued packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_ENQ_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_enq_pkt_cnt);

    /** Get SRAM Discarded packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_ENQ_RJCT_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_disc_pkt_cnt);

    /** Get SRAM to DRAM Dequeued packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_TO_DRAM_DEQ_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_to_dram_pkt_cnt);

    /** Get SRAM to Fabric Dequeued packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_TO_FABRIC_DEQ_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_to_fabric_pkt_cnt);

    /** Get SRAM Deleted packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_DEL_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_del_pkt_cnt);

    /** Get DRAM Dequeued bundle count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_DEQ_BUNDLE_CTR, INST_SINGLE,
                               &counter_info->dram_deq_bundle_cnt);

    /** Get DRAM Deletted bundle count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_DEL_BUNDLE_CTR, INST_SINGLE,
                               &counter_info->dram_del_bundle_cnt);
    /** Get SRAM Enqueued byte count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_SRAM_ENQ_BYTE_CTR, INST_SINGLE,
                               &counter_info->sram_enq_byte_cnt);

    /** Get SRAM Discarded byte count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_SRAM_ENQ_RJCT_BYTE_CTR, INST_SINGLE,
                               &counter_info->sram_disc_byte_cnt);

    /** Get SRAM Deleted byte count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_SRAM_DEL_BYTE_CTR, INST_SINGLE,
                               &counter_info->sram_del_byte_cnt);

    /** Get SRAM to DRAM Dequeued byte count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_SRAM_TO_DRAM_DEQ_BYTE_CTR, INST_SINGLE,
                               &counter_info->sram_to_dram_byte_cnt);

    /** Get SRAM to fabric Dequeued byte count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_SRAM_TO_FABRIC_DEQ_BYTE_CTR, INST_SINGLE,
                               &counter_info->sram_to_fabric_byte_cnt);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the gtimer status of block CGM */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_CGM, core_id, &is_gtimer_enabled));

    /** Get the overflow status and real counter value for SRAM Enqueued packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_ENQ_PKT_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_enq_pkt_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_enq_pkt_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_enq_pkt_cnt = counter_info->sram_enq_pkt_cnt & ((1 << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM Discarded packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_ENQ_RJCT_PKT_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_disc_pkt_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_disc_pkt_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_disc_pkt_cnt = counter_info->sram_disc_pkt_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM to DRAM Dequeued packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_TO_DRAM_DEQ_PKT_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_to_dram_pkt_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_to_dram_pkt_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_to_dram_pkt_cnt = counter_info->sram_to_dram_pkt_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM to Fabric Dequeued packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_TO_FABRIC_DEQ_PKT_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_to_fabric_pkt_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_to_fabric_pkt_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_to_fabric_pkt_cnt = counter_info->sram_to_fabric_pkt_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM Deleted packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_DEL_PKT_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_del_pkt_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_del_pkt_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_del_pkt_cnt = counter_info->sram_del_pkt_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for DRAM Dequeued bundle count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_DRAM_DEL_BUNDLE_CTR, 0, 0, 0, &counter_size);
    counter_info->dram_deq_bundle_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->dram_deq_bundle_cnt >> (counter_size - 1)) && 0x1);
    counter_info->dram_deq_bundle_cnt = counter_info->dram_deq_bundle_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for DRAM Deletted bundle count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_DRAM_DEL_BUNDLE_CTR, 0, 0, 0, &counter_size);
    counter_info->dram_del_bundle_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->dram_del_bundle_cnt >> (counter_size - 1)) && 0x1);
    counter_info->dram_del_bundle_cnt = counter_info->dram_del_bundle_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM Enqueued byte count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_ENQ_BYTE_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_enq_byte_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_enq_byte_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_enq_byte_cnt = counter_info->sram_enq_byte_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM Discarded byte count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_ENQ_RJCT_BYTE_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_disc_byte_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_disc_byte_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_disc_byte_cnt = counter_info->sram_disc_byte_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM Deleted byte count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_DEL_BYTE_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_del_byte_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_del_byte_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_del_byte_cnt = counter_info->sram_del_byte_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM to DRAM Dequeued byte count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_TO_DRAM_DEQ_BYTE_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_to_dram_byte_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_to_dram_byte_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_to_dram_byte_cnt = counter_info->sram_to_dram_byte_cnt & ((1UL << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for SRAM to Fabric Dequeued byte count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                               DBAL_FIELD_SRAM_TO_FABRIC_DEQ_BYTE_CTR, 0, 0, 0, &counter_size);
    counter_info->sram_to_fabric_byte_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->sram_to_fabric_byte_cnt >> (counter_size - 1)) && 0x1);
    counter_info->sram_to_fabric_byte_cnt = counter_info->sram_to_fabric_byte_cnt & ((1UL << (counter_size - 1)) - 1);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to configure the VSQ programmable counter 
 *        to track certain VSQ id
 */
shr_error_e
dnx_ingress_congestion_vsq_programmable_counter_configuration_set(
    int unit,
    int core,
    int vsq_group,
    int queue_id,
    int queue_id_provided)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set VSQ group to track its occupancy level   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_GROUP, INST_SINGLE, vsq_group);

    /** ONLY If a specific queue id from the VSQ is specified */
    if (queue_id_provided)
    {
        /** Set mask to 0 in order to track only specific queue*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_ID_MASK, INST_SINGLE, 0);

        /** Choose which queue from the VSQ will be tracked */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_ID, INST_SINGLE, queue_id);
    }
    else
    {
        /** Setting the mask to its max value so all queues will be tracked*/
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_VSQ_ID_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \ see .h file
 */
shr_error_e
dnx_ingress_congestion_vsq_programmable_counter_configuration_clear(
    int unit,
    int core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Reset table entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get all VSQ programmable counter 
 *        statistics
 * 
 */
shr_error_e
dnx_ingress_congestion_vsq_programmable_counter_info_get(
    int unit,
    int core_id,
    dnx_ingress_congestion_counter_info_t * counter_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get SRAM Enqueued packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_ENQ_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_enq_pkt_cnt);

    /** Get SRAM Discarded packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_ENQ_RJCT_PKT_CTR, INST_SINGLE,
                               &counter_info->sram_disc_pkt_cnt);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get all the occupancy statistics for the 
 *        current VOQ defined by the dbal iterator.
 * 
 */
shr_error_e
dnx_ingress_congestion_current_voq_occupancy_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_ingress_congestion_voq_occupancy_info_t * voq_occupancy_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_VOQ, &voq_occupancy_info->id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIZE_IN_WORDS, INST_SINGLE, &voq_occupancy_info->size));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIZE_IN_WORDS_SRAM, INST_SINGLE,
                     &voq_occupancy_info->size_sram));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BUFFERS_SIZE_SRAM, INST_SINGLE,
                     &voq_occupancy_info->buffer_size));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_PDS_SRAM, INST_SINGLE,
                     &voq_occupancy_info->nof_pds_in_sram));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIZE_AVRG, INST_SINGLE, &voq_occupancy_info->size_avrg));

    /** Translate Words to Bytes */
    voq_occupancy_info->size *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
    voq_occupancy_info->size_sram *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
    voq_occupancy_info->size_avrg *= dnx_data_ingr_congestion.info.words_resolution_get(unit);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get all the occupancy statistics for
 *        a specific VOQ.
 *
 */
shr_error_e
dnx_ingress_congestion_voq_occupancy_info_get(
    int unit,
    int core,
    int queue_id,
    dnx_ingress_congestion_voq_occupancy_info_t * voq_occupancy_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_OCCUPANCY, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Select the queue */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, queue_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SIZE_IN_WORDS, INST_SINGLE, &voq_occupancy_info->size);
    dbal_value_field32_request
        (unit, entry_handle_id, DBAL_FIELD_SIZE_IN_WORDS_SRAM, INST_SINGLE, &voq_occupancy_info->size_sram);
    dbal_value_field32_request
        (unit, entry_handle_id, DBAL_FIELD_BUFFERS_SIZE_SRAM, INST_SINGLE, &voq_occupancy_info->buffer_size);
    dbal_value_field32_request
        (unit, entry_handle_id, DBAL_FIELD_NOF_PDS_SRAM, INST_SINGLE, &voq_occupancy_info->nof_pds_in_sram);
    dbal_value_field32_request
        (unit, entry_handle_id, DBAL_FIELD_SIZE_AVRG, INST_SINGLE, &voq_occupancy_info->size_avrg);

     /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

     /** Translate Words to Bytes */
    voq_occupancy_info->size *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
    voq_occupancy_info->size_sram *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
    voq_occupancy_info->size_avrg *= dnx_data_ingr_congestion.info.words_resolution_get(unit);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * 
 */
shr_error_e
dnx_ingress_congestion_current_vsq_occupancy_info_get(
    int unit,
    uint32 entry_handle_id,
    int vsq_group,
    dnx_ingress_congestion_vsq_occupancy_info_t * vsq_non_empty_info_array)
{
    int resource = 0;
    int vsq_a_to_d_statistic_dbal_fields[] =
        { DBAL_FIELD_SIZE_IN_WORDS, DBAL_FIELD_NOF_BUFFERS_SRAM, DBAL_FIELD_NOF_PDS_SRAM };

    int vsq_e_and_f_guaranteed_dbal_fields[] =
        { DBAL_FIELD_SIZE_IN_WORDS_GRNTD, DBAL_FIELD_NOF_BUFFERS_SRAM_GRNTD, DBAL_FIELD_NOF_PDS_SRAM_GRNTD };
    int vsq_e_and_f_shared_dbal_fields[] =
        { DBAL_FIELD_SIZE_IN_WORDS_SHRD, DBAL_FIELD_NOF_BUFFERS_SRAM_SHRD, DBAL_FIELD_NOF_PDS_SRAM_SHRD };
    int vsq_e_and_f_headroom_dbal_fields[] =
        { DBAL_FIELD_SIZE_IN_WORDS_HDRM, DBAL_FIELD_NOF_BUFFERS_SRAM_HDRM, DBAL_FIELD_NOF_PDS_SRAM_HDRM };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_VSQ_ID, &vsq_non_empty_info_array->vsq_id));

    /** Get Average size */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIZE_IN_WORDS_AVRG, INST_SINGLE,
                     &vsq_non_empty_info_array->size_avrg));

    for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
    {
        /** For VSQ Groups A to D have only info for WORDS and PDS is
         *  collected */
        if (vsq_group <= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, vsq_a_to_d_statistic_dbal_fields[resource], INST_SINGLE,
                             &vsq_non_empty_info_array->resources[resource].total));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, vsq_e_and_f_guaranteed_dbal_fields[resource], INST_SINGLE,
                             &vsq_non_empty_info_array->resources[resource].guaranteed));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, vsq_e_and_f_shared_dbal_fields[resource], INST_SINGLE,
                             &vsq_non_empty_info_array->resources[resource].shared));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, vsq_e_and_f_headroom_dbal_fields[resource], INST_SINGLE,
                             &vsq_non_empty_info_array->resources[resource].headroom));

        }
    }

    /** For Group F also Headroom extension info is collected */
    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_BUFFERS_SRAM_HDRME, INST_SINGLE,
                         &vsq_non_empty_info_array->
                         resources[DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS].headroom_ex));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_PDS_SRAM_HDRME, INST_SINGLE,
                         &vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS].headroom_ex));
    }

    /** If VSQ is group E or F the total queue size is spread
     *  into three separate buffers - guaranteed, shared and
     *  headroom. For control purposes it is required to have also
     *  the total queue size. Thats why we sum up the three
     *  buffers */
    if (vsq_group >= DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {
        vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total =
            vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].guaranteed +
            vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].shared +
            vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].headroom;
    }

    /** Translate words to Bytes */
    vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total *=
        dnx_data_ingr_congestion.info.words_resolution_get(unit);
    vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].guaranteed *=
        dnx_data_ingr_congestion.info.words_resolution_get(unit);
    vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].shared *=
        dnx_data_ingr_congestion.info.words_resolution_get(unit);
    vsq_non_empty_info_array->resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].headroom *=
        dnx_data_ingr_congestion.info.words_resolution_get(unit);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - init configuration for VOQ programmable counters
 */
static shr_error_e
dnx_ingress_congestion_programmable_counter_voq_init_config_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS, &entry_handle_id));

    /** set core to ALL */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Get maximum value for the VOQ field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                                                          DBAL_FIELD_VOQ_ID,
                                                          FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    /** set counters to all queues */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_ID, INST_SINGLE, max_value);

    /** Get maximum value for the mask field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS,
                                                          DBAL_FIELD_VOQ_MASK,
                                                          FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    /** set mask to all ones */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_MASK, INST_SINGLE, max_value);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - init configuration for VSQ programmable counters
 */
static shr_error_e
dnx_ingress_congestion_programmable_counter_vsq_init_config_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS, &entry_handle_id));

    /** set core to ALL */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Get maximum value for the VSQ field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS,
                                                          DBAL_FIELD_VSQ_ID,
                                                          FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    /** set counters to all queues */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_ID, INST_SINGLE, max_value);

    /** Get maximum value for the mask field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS,
                                                          DBAL_FIELD_VSQ_ID_MASK,
                                                          FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    /** set mask to all ones */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_ID_MASK, INST_SINGLE, max_value);

    /** Get maximum value for the VSQ group field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_CGM_VSQ_PRG_CTR_STATS,
                                                          DBAL_FIELD_VSQ_GROUP,
                                                          FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    /** set VSQ group */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_GROUP, INST_SINGLE, max_value);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_programmable_counter_init_all_ones_config_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * VOQs
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_programmable_counter_voq_init_config_set(unit));

    /*
     * VSQs
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_programmable_counter_vsq_init_config_set(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_free_resource_counter_get(
    int unit,
    int core,
    dnx_ingress_congestion_statistics_info_t * stats)
{
    uint32 entry_handle_id, resource;
    dbal_fields_e free_count_fields[] =
        { DBAL_FIELD_FREE_DRAM_BDBS_ST, DBAL_FIELD_FREE_SRAM_BUFFERS_ST, DBAL_FIELD_FREE_SRAM_PDBS_ST };
    dbal_fields_e min_free_count_fields[] =
        { DBAL_FIELD_MIN_FREE_DRAM_BDBS_ST, DBAL_FIELD_MIN_FREE_SRAM_BUFFERS_ST, DBAL_FIELD_MIN_FREE_SRAM_PDBS_ST };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_FREE_RESOURCE_COUNTERS, &entry_handle_id));

    /** Set core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    for (resource = 0; resource < DNX_INGRESS_CONGESTION_STATISTICS_NOF; ++resource)
    {
        dbal_value_field32_request(unit, entry_handle_id, free_count_fields[resource], INST_SINGLE,
                                   &stats->free_count[resource]);
        dbal_value_field32_request(unit, entry_handle_id, min_free_count_fields[resource], INST_SINGLE,
                                   &stats->min_free_count[resource]);
    }

    /** Get values to HW */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
