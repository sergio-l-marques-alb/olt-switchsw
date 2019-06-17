/** \file diag_dnx_stif.c
 *
 * Main diagnostics for STIF applications
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
/** soc  */
#include <soc/dnx/swstate/auto_generated/access/stif_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
/** sal */
#include <sal/appl/sal.h>
#include "diag_dnx_stif.h"

/*************
 * TYPEDEFS  *
 *************/
typedef struct diag_dnx_stif_source_s
{
    bcm_stat_stif_source_type_t src_type;
    char *source_str;
} diag_dnx_stif_source_t;

static const diag_dnx_stif_source_t source_enum_to_char_binding_table[] = {
    {bcmStatStifSourceIngressEnqueue, "Ingress Enqueue"},
    {bcmStatStifSourceIngressDequeue, "Ingress Dequeue"},
    {bcmStatStifSourceIngressScrubber, "Ingress Scrubber"},
    {bcmStatStifSourceEgressDequeue, "Egress Dequeue"}
};

typedef struct diag_dnx_stif_report_elements_s
{
    bcm_stat_stif_record_element_type_t element;
    char *element_str;
} diag_dnx_stif_ingress_report_elements_t;

static const diag_dnx_stif_ingress_report_elements_t report_element_enum_to_string_binding_table[] = {
    {bcmStatStifRecordElementObj0, "Stat-Obj-0"},
    {bcmStatStifRecordElementObj1, "Stat-Obj-1"},
    {bcmStatStifRecordElementObj2, "Stat-Obj-2"},
    {bcmStatStifRecordElementObj3, "Stat-Obj-3"},
    {bcmStatStifRecordElementPacketSize, "PacketSize"},
    {bcmStatStifRecordElementOpCode, "Opcode"},
    {bcmStatStifRecordElementIngressDispositionIsDrop, "DispositionIsDrop"},
    {bcmStatStifRecordElementIngressTmDropReason, "TmDropReason"},
    {bcmStatStifRecordElementIngressTrafficClass, "TrafficClass"},
    {bcmStatStifRecordElementIngressIncomingDropPrecedence, "IncomingDP"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeterResolved, "DP-MeterResolved"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter0Valid, "DP-Meter0-Valid"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter0Value, "DP-Meter0-Value"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter1Valid, "DP-Meter1-Valid"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter1Value, "DP-Meter1-Value"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter2Valid, "DP-Meter2-Valid"},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter2Value, "DP-Meter2-Value"},
    {bcmStatStifRecordElementIngressCore, "IngressCore"},
    {bcmStatStifRecordElementIngressPpMetaData, "PP-MetaData"},
    {bcmStatStifRecordElementIngressQueueNumber, "QueueNumber"},
    {bcmStatStifRecordElementEgressMetaDataMultiCast, "MetaData-Multicast"},
    {bcmStatStifRecordElementEgressMetaDataPpDropReason, "MetaData-PP-Drop-Reason"},
    {bcmStatStifRecordElementEgressMetaDataPort, "MetaData-Port"},
    {bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni, "MetaData-EcnEligibleAndCni"},
    {bcmStatStifRecordElementEgressMetaDataTrafficClass, "MetaData-TrafficClass"},
    {bcmStatStifRecordElementEgressMetaDataDropPrecedence, "MetaData-DP"},
    {bcmStatStifRecordElementEgressMetaDataObj0, "MetaData-Stat-Obj-0"},
    {bcmStatStifRecordElementEgressMetaDataObj1, "MetaData-Stat-Obj-1"},
    {bcmStatStifRecordElementEgressMetaDataObj2, "MetaData-Stat-Obj-2"},
    {bcmStatStifRecordElementEgressMetaDataObj3, "MetaData-Stat-Obj-3"},
    {bcmStatStifRecordElementEgressMetaDataCore, "Core-ID"}
};

/**
 \brief - Print error if STIF is not enabled
*Also return the report mode that STIF is working in
* \param [in] unit - unit id 
* \param [out] report_mode - return the report mode
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None 
 */
static shr_error_e
sh_dnx_stif_validation(
    int unit,
    int *report_mode)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        SHR_CLI_EXIT(_SHR_E_DISABLED, "unit %d Statistic Interface is not enabled! \n", unit);
    }

    *report_mode = dnx_data_stif.config.stif_report_mode_get(unit);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_mapping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_instance_present, core_id, source_type, port_core, report_mode, flags = 0, instance_id;
    bcm_port_t port;
    bcm_stat_stif_source_t source;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("instance", is_instance_present);

    /** check STIF */
    SHR_IF_ERR_EXIT(sh_dnx_stif_validation(unit, &report_mode));

    /** print table */
    PRT_TITLE_SET("Statistic Interface (source <-> port) mapping for unit=%d ", unit);
    PRT_COLUMN_ADD("Source Type");
    PRT_COLUMN_ADD("Source Core");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Port Core");
    if (is_instance_present)
    {
        PRT_COLUMN_ADD("Instance ID");
    }
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        source.core = core_id;

        for (source_type = 0; source_type < dnx_data_stif.report.nof_source_types_get(unit); source_type++)
        {
            switch (source_type)
            {
                case bcmStatStifSourceIngressEnqueue:
                case bcmStatStifSourceIngressDequeue:
                case bcmStatStifSourceIngressScrubber:
                case bcmStatStifSourceEgressDequeue:
                {
                    /** skip unsupported source types for the selected report
                     *  mode */
                    if ((report_mode == dnx_stat_stif_mgmt_report_mode_qsize
                         && source_type == bcmStatStifSourceEgressDequeue)
                        || (report_mode == dnx_stat_stif_mgmt_report_mode_billing
                            && (source_type == bcmStatStifSourceIngressScrubber
                                || source_type == bcmStatStifSourceIngressDequeue))
                        || (report_mode == dnx_stat_stif_mgmt_report_mode_billing_ingress
                            && (source_type == bcmStatStifSourceEgressDequeue
                                || source_type == bcmStatStifSourceIngressScrubber)))
                    {
                        continue;
                    }
                    source.src_type = source_type;
                    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_get(unit, flags, source, &port));
                    /** if get port is valid means that the source is connected
                     *  to this port */
                    if (port != BCM_PORT_INVALID)
                    {
                        /** get the internal core from the given logical port */
                        SHR_IF_ERR_EXIT(dnx_stif_mgmt_logical_port_to_instance_id_get
                                        (unit, port, source, &port_core, &instance_id));
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%s", source_enum_to_char_binding_table[source_type].source_str);
                        PRT_CELL_SET("%d", core_id);
                        PRT_CELL_SET("%d", port);
                        PRT_CELL_SET("%d", port_core);
                        if (is_instance_present)
                        {
                            PRT_CELL_SET("%d", instance_id);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
*\brief -  Find if the record has a hole and calculate the 
*size of the hole; get the elements array and their sizes
*error 
* \param [in] unit - unit id
* \param [in] flags - flags - ing/egr
* \param [in] record_size - the size of the record
* \param [out] nof_elements - return the number of elements in
*        the record
* \param [out] record_format_elements - elements in the record
* \param [out] hole_found - is there a hole or not
* \param [out] hole_size - return the size of the hole in bits
* \param [out] element_sizes - array with the corresponding
*        element sizes
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None 
 */
static shr_error_e
sh_dnx_stif_record_find_hole(
    int unit,
    int flags,
    int record_size,
    int *nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements,
    int *hole_found,
    int *hole_size,
    int *element_sizes)
{
    int element_id, calculate_size = 0, max_nof_elements;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_elements =
        DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS + DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS +
        DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1;
    /** each record contains msb opcode record type and
     *  packetsize is part of the elements and is
     *  handled with them count the opcode record type here */
    calculate_size = dnx_data_stif.report.billing_format_msb_opcode_size_get(unit);

    if (flags & BCM_STAT_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_ingress_record_format_get
                        (unit, max_nof_elements, record_format_elements, nof_elements, element_sizes));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_egress_record_format_get
                        (unit, max_nof_elements, record_format_elements, nof_elements, element_sizes));
    }
    for (element_id = 0; element_id < *nof_elements; element_id++)
    {
        calculate_size += element_sizes[element_id];
    }

    /** Check if there is a hole in the record and calculate the
     *  size */
    if (calculate_size < record_size)
    {
        *hole_found = TRUE;
        *hole_size = record_size - calculate_size;
    }
    else
    {
        *hole_found = FALSE;
        *hole_size = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_format_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int report_mode, report_size;
    const dnx_data_stif_report_report_size_t *ingress_report_size, *egress_report_size;
    bcm_stat_stif_record_format_element_t elements_array[DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS +
                                                         DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS +
                                                         DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1];
    int nof_elements, element_id, table_count, table_id;
    int flags;
    int element_array_size[DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS + DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1] =
        { 0 }, hole_found = 0, hole_size = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** check STIF */
    SHR_IF_ERR_EXIT(sh_dnx_stif_validation(unit, &report_mode));
    /** Diagnostic is supported for billing modes only */
    if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_CLI_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not configured in mode BILLING/INGRESS_BILLING where the format is configurable. For QSIZE the format is default. \n");
    }
    ingress_report_size =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_ingress_get(unit));
    egress_report_size =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_egress_get(unit));
    /** for BILLING two tables will be printed with information
     *    for INGRESS report and EGRESS report for BILLING
     *    INGRESS - there will be one tabke orinted for INGRESS
     *    record */
    table_count = (report_mode == dnx_stat_stif_mgmt_report_mode_billing) ? 2 : 1;

    for (table_id = 0; table_id < table_count; table_id++)
    {
        flags = (table_id == 0) ? BCM_STAT_INGRESS : BCM_STAT_EGRESS;
        report_size = (table_id == 0) ? ingress_report_size->size : egress_report_size->size;

        /** print table for ingress record format   */
        PRT_TITLE_SET("Statistic Interface unit=%d %s Record Format", unit, (table_id == 0) ? "Ingress" : "Egress");
        PRT_COLUMN_ADD(" ");
        PRT_COLUMN_ADD("Opcode Type %s", (table_id == 0) ? "Ingress" : "Egress");
        PRT_COLUMN_ADD("PacketSize");
       /** Get the elements sizes and check for special case if the
        *  report has a hole in it - the hole is placed before
        *  PacketSize */
        SHR_IF_ERR_EXIT(sh_dnx_stif_record_find_hole
                        (unit, flags, report_size, &nof_elements, elements_array, &hole_found, &hole_size,
                         element_array_size));

        /** If there is a hole in the record - print a column for it */
        if (hole_found)
        {
            PRT_COLUMN_ADD("Hole");
        }
        /**  Print the element names, exclude packet size as the last
         *   element - already printed */
        for (element_id = nof_elements - 2; element_id >= 0; element_id--)
        {
            if (elements_array[element_id].element_type >= 0
                && elements_array[element_id].element_type <= bcmStatStifRecordElementEgressMetaDataObj3)
            {
                PRT_COLUMN_ADD("%s",
                               report_element_enum_to_string_binding_table[elements_array[element_id].
                                                                           element_type].element_str);
            }
            else
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "unit %d accessing wrong element_type for bcm_stat_stif_record_element_type_t, please check, %d \n",
                             unit, element_id);
            }
        }

        /**  First Row is the SIZE */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Size: %d", report_size);
        /** first is the opcode */
        PRT_CELL_SET("%d", dnx_data_stif.report.billing_format_msb_opcode_size_get(unit));
        /** then is the Packet Size */
        PRT_CELL_SET("%d", dnx_data_stif.report.billing_format_element_packet_size_get(unit));
        /** after the PacketSize will be placed a hole if exists */
        if (hole_found)
        {
            PRT_CELL_SET("%d", hole_size);
        }
        for (element_id = nof_elements - 2; element_id >= 0; element_id--)
        {
            /** first row indicates the size of the field */
            PRT_CELL_SET("%d", element_array_size[element_id]);
        }

        /** Second Row is the MASK the user selected */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        /** third row represents the mask value the user set */
        PRT_CELL_SET("Mask:");
        PRT_CELL_SET("FULL"); /** for opcode type*/
        PRT_CELL_SET("FULL"); /** for packet size*/
        if (hole_found)
        {
            PRT_CELL_SET(" -- ");
        }
        for (element_id = nof_elements - 2; element_id >= 0; element_id--)
        {
            if (elements_array[element_id].mask == BCM_STAT_FULL_MASK)
            {
                PRT_CELL_SET("FULL");
            }
            else
            {
                PRT_CELL_SET("%04x", elements_array[element_id].mask);
            }
        }
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_stif_mapping_list_options[] = {
    {"INSTance", SAL_FIELD_TYPE_BOOL, "instance id will be printed - used for debug only", "false"},
    {NULL}
};

static sh_sand_man_t sh_dnx_stif_mapping_man = {
    .brief = "Print the source to logical port mapping for STIF",
    .full = "Print the source to logical port mapping for STIF - source type, source core, port id, port core",
    .synopsis = NULL,
    .examples = "instance=1"
};

static sh_sand_man_t sh_dnx_stif_format_man = {
    .brief = "Print the format of the record - ingress (and egress) relevant for billing",
    .full = "Print the format of the record in table with the names, sizes and masks \n "
        "For mode INGRESS BILLING - print one table with the ingress record format \n"
        "For mode BILLING - print one table for the ingress record fomat and one for the egress",
    .synopsis = NULL,
    .examples = NULL
};

/**
 * \brief DNX STIF diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for STIF diagnostic commands 
 */
sh_sand_cmd_t sh_dnx_stif_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"MAPping", sh_dnx_mapping_cmd, NULL, dnx_stif_mapping_list_options, &sh_dnx_stif_mapping_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {"FoRMat", sh_dnx_format_cmd, NULL, NULL, &sh_dnx_stif_format_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_man_t sh_dnx_stif_man = {
    .brief = "STIF diagnostics \n"
};
