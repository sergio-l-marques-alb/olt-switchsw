/** \file diag_dnx_egr_queuing.c
 * 
 * diagnostics for egress queuing
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_PORT

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <bcm_int/dnx/cosq/cosq.h>

#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx_dispatch.h>

/** sal */
#include <sal/appl/sal.h>
#include "diag_dnx_egr_queuing.h"

/*
 * data structure for egq coutner rate information
 */
typedef struct dnx_egq_counter_rate_info_e
{
    char name[DNX_EGQ_COUNTER_NAME_LEN];
    dnx_egq_counter_type_t type;
    int count_ovf;
    uint32 rate;
} dnx_egq_counter_rate_info_t;

/*
 * specifies the egress epni rate scheme
 */
typedef enum dnx_egr_epni_rate_scheme_e
{
    DNX_EGR_EPNI_RATE_SCHEME_TOTAL,
    DNX_EGR_EPNI_RATE_SCHEME_IF,
    DNX_EGR_EPNI_RATE_SCHEME_PORT,
    DNX_EGR_EPNI_RATE_SCHEME_QP,
    DNX_EGR_EPNI_RATE_SCHEME_MIRROR
} dnx_egr_epni_rate_scheme_t;

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egq_compensation_options[] = {
    /*name        type                  desc                                        default     ext*/
    {"port",      SAL_FIELD_TYPE_PORT,  "port # / logical port type / port name",    "all",      NULL},
    {"value",     SAL_FIELD_TYPE_INT32, "compensation value to set",                 "0",        NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egq_compensation_man = {
    .brief = "Set/Print egress compensation information per port",
    .full = NULL,
    .synopsis = NULL,
    .examples = "port=13\n" "port=13 value=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egq_shaping_options[] = {
    /*name        type                  desc                                          default     ext*/
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",      "all",      NULL},
    {"GRaphical", SAL_FIELD_TYPE_BOOL, "print graphical illustration of the PS",      "FALSE",    NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egq_shaping_man = {
    .brief = "Print egress shaping information per port",
    .full = "If no port is provided then all ports shaping info is printed",
    .synopsis = NULL,
    .examples = "\n" "port=13\n" "port=13 graphical"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_rqp_rate_options[] = {
    /*name              type                     desc                                 default       ext*/
    {"core",            SAL_FIELD_TYPE_INT32,    "core id",                           "-17",        NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_rqp_rate_man = {
    .brief = "calculate and display egress rqp rate",
    .full = NULL,
    .synopsis = NULL,
    .examples = "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_pqp_rate_options[] = {
    /*name              type                     desc                                         default    ext*/
    {"core",            SAL_FIELD_TYPE_INT32,    "core id",                                   "0",       NULL},
    {"port",            SAL_FIELD_TYPE_INT32,    "port # / logical port type / port name",    "-1",      NULL},
    {"TrafficClasS",    SAL_FIELD_TYPE_INT32,    "parameter: traffic class index",            "-1",      NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_pqp_rate_man = {
    .brief = "calculate and display egress pqp rate",
    .full = NULL,
    .synopsis = NULL,
    .examples = "port=13 tc=0 \n" "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_epep_rate_options[] = {
    /*name              type                     desc                                         default    ext*/
    {"core",            SAL_FIELD_TYPE_INT32,    "core id",                                   "0",       NULL},
    {"port",            SAL_FIELD_TYPE_INT32,    "port # / logical port type / port name",    "-1",       NULL},
    {"TrafficClasS",    SAL_FIELD_TYPE_INT32,    "parameter: traffic class index",            "-1",       NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_epep_rate_man = {
    .brief = "calculate and display egress epep rate",
    .full = NULL,
    .synopsis = NULL,
    .examples = "port=13 tc=0 \n" "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_epni_rate_options[] = {
    /*name              type                     desc                                         default    ext*/
    {"SCheMe",          SAL_FIELD_TYPE_INT32,    "Indicate scheme",                          "0",       NULL},
    {"BandWidth",       SAL_FIELD_TYPE_INT32,    "Indicate the objective to be measured",    "0",       NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_epni_rate_man = {
    .brief = "calculate and display egress epni rate according to the scheme.\n",
    .full = "when scheme = 0(measure total device), bw can be ignored,\n"
        "when scheme = 1(measure EGQ IF), bw indicates EGQ IF to be measured,\n"
        "when scheme = 2(measure port), bw indicates port to be measured,\n"
        "when scheme = 3(measure QPair), bw indicates QPair to be measured,\n"
        "when scheme = 4(measure Mirror Priority), bw indicates Mirror Priority to be measured.",
    .synopsis = "[scheme=<integer>] [bw=<integer>]",
    .examples = "scheme=2 bw=1"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_queuing_rate_options[] = {
    /*name              type                     desc                                         default    ext*/
    {"core",            SAL_FIELD_TYPE_INT32,    "core id",                                   "0",       NULL},
    {"port",            SAL_FIELD_TYPE_INT32,    "port # / logical port type / port name",    "-1",       NULL},
    {"TrafficClasS",    SAL_FIELD_TYPE_INT32,    "parameter: traffic class index",            "-1",       NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_queuing_rate_man = {
    .brief = "calculate and display egress queuing rate",
    .full = NULL,
    .synopsis = NULL,
    .examples = "port=13 tc=0 \n" "core=0"
};

static void
sh_dnx_print_string_in_loop(
    int nof_iters,
    const char *str,
    uint8 new_line)
{
    int i = 0;
    for (i = 0; i < nof_iters; i++)
    {
        cli_out("%s", str);
    }
    if (new_line == 1)
    {
        cli_out("|\n| ");
    }
    else if (new_line == 2)
    {
        cli_out(" \n| ");
    }
    else if (new_line == 3)
    {
        cli_out("\n  ");
    }
    else if (new_line == 4)
    {
        cli_out("\n _");
    }
}

/**
 * \brief - Return the actual counter rate of rqp
 */
static shr_error_e
sh_dnx_egq_rqp_counter_rate_get(
    int unit,
    int core,
    dnx_egq_counter_rate_info_t * rate_info,
    int *nof_counter)
{
    int counter_idx;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[DNX_EGQ_RQP_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, DNX_EGQ_RQP_COUNTER_NUM * sizeof(dnx_egq_counter_info_t));

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    gtimer_time.time = 1;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_RQP, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_RQP, core));

    /** Get actual Credit count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_rqp_counter_info_get(unit, core, counter_info, nof_counter));

    /** Convert byte counter to Kbps */
    /** Convert packet counter to Kpps */
    /** In order to avoid overflow the order of the calculation must be kept*/
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        sal_snprintf(rate_info[counter_idx].name, DNX_EGQ_COUNTER_NAME_LEN, "%s", counter_info[counter_idx].name);
        if (!counter_info[counter_idx].count_ovf)
        {
            if (DNX_EGQ_COUTNER_TYPE_BYTE == counter_info[counter_idx].type)
            {
                rate_info[counter_idx].rate = (counter_info[counter_idx].count_val / 1000) * UTILEX_NOF_BITS_IN_BYTE;
            }
            else
            {
                rate_info[counter_idx].rate = counter_info[counter_idx].count_val / 1000;
            }
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_RQP, core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of pqp
 */
static shr_error_e
sh_dnx_egq_pqp_counter_rate_get(
    int unit,
    int core,
    int port,
    int tc,
    dnx_egq_counter_rate_info_t * rate_info)
{
    bcm_core_t core_used;
    int counter_idx;
    int qpair, base_qp;
    int num_priorities;
    uint32 otm;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[DNX_EGQ_PQP_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, DNX_EGQ_PQP_COUNTER_NUM * sizeof(dnx_egq_counter_info_t));

    if ((-1 != port) && (-1 != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_used));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qp));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

        SHR_RANGE_VERIFY(tc, 0, num_priorities - 1, _SHR_E_PARAM,
                         "tc (%d) is out of range. (Number of priorities is %d)\n", tc, num_priorities);
        qpair = base_qp + tc;

        /** Select port HR to be tracked */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_set
                        (unit, core_used, DNX_EGQ_COUTNER_FILTER_BY_QP, qpair));
    }
    else if (-1 != port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core_used, &otm));

        /** Select OTM port to be tracked */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_set
                        (unit, core_used, DNX_EGQ_COUTNER_FILTER_BY_OTM, otm));
    }
    else
    {
        core_used = core;
        /** Select the device to be tracked */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_set
                        (unit, core_used, DNX_EGQ_COUTNER_FILTER_BY_NONE, 0));
    }

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    gtimer_time.time = 1;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_PQP, core_used));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_PQP, core_used));

    /** Get actual Credit count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_info_get(unit, core_used, counter_info));

    /** Convert byte counter to Kbps */
    /** Convert packet counter to Kpps */
    /** In order to avoid overflow the order of the calculation must be kept*/
    for (counter_idx = 0; counter_idx < DNX_EGQ_PQP_COUNTER_NUM; counter_idx++)
    {
        sal_snprintf(rate_info[counter_idx].name, DNX_EGQ_COUNTER_NAME_LEN, "%s", counter_info[counter_idx].name);
        if (!counter_info[counter_idx].count_ovf)
        {
            if (DNX_EGQ_COUTNER_TYPE_BYTE == counter_info[counter_idx].type)
            {
                rate_info[counter_idx].rate = (counter_info[counter_idx].count_val / 1000) * UTILEX_NOF_BITS_IN_BYTE;
            }
            else
            {
                rate_info[counter_idx].rate = counter_info[counter_idx].count_val / 1000;
            }
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_PQP, core_used));

    /** Disable the credit counter filters */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_reset(unit, core_used));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of rqp
 */
static shr_error_e
sh_dnx_egq_epep_counter_rate_get(
    int unit,
    int core,
    int port,
    int tc,
    dnx_egq_counter_rate_info_t * rate_info,
    int *nof_counter)
{
    bcm_core_t core_used;
    int flag = 0;
    int counter_idx;
    int qpair, base_qp;
    int num_priorities;
    int nof_counter_tmp;
    int count_by, count_place;
    uint32 otm;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, 2 * DNX_EGQ_EPNI_COUNTER_NUM * sizeof(dnx_egq_counter_info_t));

    if ((-1 != port) && (-1 != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_used));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qp));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

        SHR_RANGE_VERIFY(tc, 0, num_priorities - 1, _SHR_E_PARAM,
                         "tc (%d) is out of range. (Number of priorities is %d)\n", tc, num_priorities);
        qpair = base_qp + tc;

        /** Select port HR to be tracked */
        count_by = DNX_EGQ_COUTNER_FILTER_BY_QP;
        count_place = qpair;
    }
    else if (-1 != port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core_used, &otm));

        /** Select OTM port to be tracked */
        count_by = DNX_EGQ_COUTNER_FILTER_BY_OTM;
        count_place = otm;
    }
    else
    {
        core_used = core;

        /** Select the device to be tracked */
        count_by = DNX_EGQ_COUTNER_FILTER_BY_NONE;
        count_place = 0;
    }

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    gtimer_time.time = 1;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    /** Count packet.*/
    /** Set EPNI counter to trace the selected objective.*/
    flag = DNX_EGQ_CONFIGURATION_COUNT_PACKETS;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core_used, flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core_used));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core_used));

    /** Get actual packet count for 1 sec */
    flag = DNX_EGQ_EPNI_QUEUE_COUNTER | DNX_EGQ_NON_PROGRAMMABLE_COUNTER;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_info_get(unit, core_used, flag, counter_info, &nof_counter_tmp));
    *nof_counter = nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core_used));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_reset(unit, core_used));

    /** Count byte.*/
    /** Set EPNI counter to trace the selected objective.*/
    flag = DNX_EGQ_CONFIGURATION_COUNT_BYTES;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core_used, flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core_used));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core_used));

    /** Get actual packet count for 1 sec */
    flag = DNX_EGQ_EPNI_QUEUE_COUNTER | DNX_EGQ_NON_PROGRAMMABLE_COUNTER;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_info_get
                    (unit, core_used, flag, &counter_info[*nof_counter], &nof_counter_tmp));
    *nof_counter += nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core_used));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_reset(unit, core_used));

    /** Convert byte counter to Kbps */
    /** Convert packet counter to Kpps */
    /** In order to avoid overflow the order of the calculation must be kept*/
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        sal_snprintf(rate_info[counter_idx].name, DNX_EGQ_COUNTER_NAME_LEN, "%s", counter_info[counter_idx].name);
        if (!counter_info[counter_idx].count_ovf)
        {
            if (DNX_EGQ_COUTNER_TYPE_BYTE == counter_info[counter_idx].type)
            {
                rate_info[counter_idx].rate = (counter_info[counter_idx].count_val / 1000) * UTILEX_NOF_BITS_IN_BYTE;
            }
            else
            {
                rate_info[counter_idx].rate = counter_info[counter_idx].count_val / 1000;
            }
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of rqp
 */
static shr_error_e
sh_dnx_egq_epni_counter_rate_get(
    int unit,
    int core,
    int scheme,
    int bw,
    dnx_egq_counter_rate_info_t * rate_info,
    int *nof_counter)
{
    int set_flag, get_flag;
    int counter_idx;
    int nof_counter_tmp;
    int count_by, count_place;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, 2 * DNX_EGQ_EPNI_COUNTER_NUM * sizeof(dnx_egq_counter_info_t));

    switch (scheme)
    {
        case DNX_EGR_EPNI_RATE_SCHEME_TOTAL:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_NONE;
            get_flag =
                DNX_EGQ_EPNI_QUEUE_COUNTER | DNX_EGQ_EPNI_IF_COUNTER | DNX_EGQ_EPNI_MIRROR_COUNTER |
                DNX_EGQ_NON_PROGRAMMABLE_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_QP:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_QP;
            get_flag = DNX_EGQ_EPNI_QUEUE_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_PORT:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_OTM;
            get_flag = DNX_EGQ_EPNI_QUEUE_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_IF:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_IF;
            get_flag = DNX_EGQ_EPNI_IF_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_MIRROR:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_MIRROR;
            get_flag = DNX_EGQ_EPNI_MIRROR_COUNTER;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected EPNI rate scheme %d\n", scheme);
            break;
    }

    count_place = bw;

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    gtimer_time.time = 1;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    /** Count packet.*/
    /** Set EPNI counter to trace the selected objective.*/
    set_flag = DNX_EGQ_CONFIGURATION_COUNT_PACKETS;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core, set_flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core));

    /** Get actual packet count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_info_get(unit, core, get_flag, counter_info, &nof_counter_tmp));
    *nof_counter = nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_reset(unit, core));

    /** Count byte.*/
    /** Set EPNI counter to trace the selected objective.*/
    set_flag = DNX_EGQ_CONFIGURATION_COUNT_BYTES;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core, set_flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core));

    /** Get actual packet count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_info_get
                    (unit, core, get_flag, &counter_info[*nof_counter], &nof_counter_tmp));
    *nof_counter += nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_reset(unit, core));

    /** Convert byte counter to Kbps */
    /** Convert packet counter to Kpps */
    /** In order to avoid overflow the order of the calculation must be kept*/
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        sal_snprintf(rate_info[counter_idx].name, DNX_EGQ_COUNTER_NAME_LEN, "%s", counter_info[counter_idx].name);
        if (!counter_info[counter_idx].count_ovf)
        {
            if (DNX_EGQ_COUTNER_TYPE_BYTE == counter_info[counter_idx].type)
            {
                rate_info[counter_idx].rate = (counter_info[counter_idx].count_val / 1000) * UTILEX_NOF_BITS_IN_BYTE;
            }
            else
            {
                rate_info[counter_idx].rate = counter_info[counter_idx].count_val / 1000;
            }
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_egq_shaping_graphic(
    int unit,
    int port_num)
{
    int i = 0, j = 0;
    uint8 is_sp_bmp = 0;
    uint32 TCG_to_TCs[8];       /* TCG_to_TCs_table[i]=if the x-th bit is on, port priority-x is attached to TCG i */
    uint32 tm_port;
    int port_nof_q_pairs, pps_base_q_pair;
    int ps_num;
    soc_port_t local_port_gport;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(TCG_to_TCs, 0, 8 * sizeof(uint32));

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_num, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_num, &port_nof_q_pairs));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_num, &pps_base_q_pair));

    ps_num = pps_base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;

    BCM_GPORT_LOCAL_SET(local_port_gport, port_num);

    cli_out("\n Graphic representation of port %d through port scheduler number %d:\n"
            "**********************************************************************\n  ", port_num, ps_num);
    /*
     * iterates over q_pairs, and when nof_q_pairs is reached finds next port index 
     */
    for (i = 0; i < port_nof_q_pairs; i++)
    {
        cli_out("  Port Index %03d   ", port_num);
    }
    cli_out(" \n  ");

    for (i = 0; i < port_nof_q_pairs; i++)
    {
        cli_out("  Q-Pair %03d(P%d)   ", pps_base_q_pair + i, i);
    }
    cli_out("\n  ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "     | U | M |     ", 3);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "     |___|___|     ", 3);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "     |___|___|     ", 4);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "_______|___|_______", 2);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "       |   |       ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  ---------------  ", 1);

    /*
     * iterates over q_pairs and prints the weights or the strict priority
     */
    for (i = 0; i < port_nof_q_pairs; i++)
    {
        int mode_uc, mode_mc, weight_uc, weight_mc;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = port_nof_q_pairs;

        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeUnicastEgress, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode_uc, &weight_uc));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeMulticastEgress, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode_mc, &weight_mc));

        if (mode_uc != -1 || mode_mc != -1)
        {       /* strict priority queue (and not weighted queue) */
            if (mode_uc > mode_mc)
            {
                cli_out("  \\    L   H    /  ");
            }
            else
            {
                cli_out("  \\    H   L    /  ");
            }
            is_sp_bmp = is_sp_bmp | (1 << i);
        }
        else
        {
            cli_out("  \\ %03d     %03d /  ", weight_uc, weight_mc);
        }
    }
    cli_out("|\n| ");

    for (i = 0; i < port_nof_q_pairs; i++)
    {
        if (is_sp_bmp % 2 == 1)
        {
            cli_out("   \\   -SP-    /   ");
        }
        else
        {
            cli_out("   \\   -WFQ-   /   ");
        }
        is_sp_bmp = is_sp_bmp >> 1;
    }
    cli_out("|\n| ");

    sh_dnx_print_string_in_loop(port_nof_q_pairs, "    -----------    ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "       |\\|/|       ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "       |/|\\|       ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

    /*
     * iterates over q_pairs and prints the shapers bandwidth
     */
    for (i = 0; i < port_nof_q_pairs; i++)
    {
        uint32 kbits_sec_max, flags, kbits_sec_min;
        int TCG, temp;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = i;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTC, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, info.out_gport, i, &kbits_sec_min, &kbits_sec_max, &flags));
        if (kbits_sec_max == 0)
        {
            cli_out("    (unlimited)    ");
        }
        else
        {
            cli_out("  (%010uKbs)  ", kbits_sec_max);
        }

        /*
         * fill the TCG_to_TCs with values
         */
        if (port_nof_q_pairs == 1)
        {
            cli_out("|\n|__________|_________|\n");
            cli_out("           |          \n");
            return CMD_OK;
        }
        else if (port_nof_q_pairs == 2)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, i, &TCG, &temp));    /* get multicast queue
                                                                                                 * number i */
            TCG -= BCM_COSQ_SP0;
            TCG_to_TCs[TCG] = TCG_to_TCs[TCG] | (1 << i);
        }
    }
    cli_out("|\n| ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

    if (port_nof_q_pairs == 2)
    {
        cli_out("     ----------------------------     |\n| ");
        cli_out("     \\   H                  L   /     |\n| ");
        cli_out("      \\           SP           /      |\n| ");
        cli_out("       ------------------------       |\n|_");
        cli_out("__________________|___________________|\n  ");
        cli_out("                  |                    \n");
        return CMD_OK;
    }
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "                   ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, "   |   |  ...  |   ", 0);
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, "         |         ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, " ----------------- ", 1);

    for (i = 0; i < port_nof_q_pairs; i++)
    {
        uint8 space_left = 15;
        uint32 temp_TFG_to_TCs_i = TCG_to_TCs[i];
        if (i < 4)
        {
            cli_out(" \\");
        }
        else
        {
            cli_out(" |");
        }
        for (j = 0; j < 8 && space_left > 0; j++)
        {
            if (TCG_to_TCs[i] % 2 == 1)
            {
                cli_out(" P%d", j);
                space_left -= 3;
            }
            TCG_to_TCs[i] = TCG_to_TCs[i] >> 1;
        }
        TCG_to_TCs[i] = temp_TFG_to_TCs_i;
        if (space_left == 15)
        {
            cli_out("(none attached)");
            space_left = 0;
        }
        sh_dnx_print_string_in_loop(space_left, " ", 0);
        if (i < 4)
        {
            cli_out("/ ");
        }
        else
        {
            cli_out("| ");
        }
    }
    cli_out("|\n| ");

    for (i = 0; i < port_nof_q_pairs; i++)
    {
        uint8 space_left = 10;
        int8 skip_first = 5;
        if (i < 4)
        {
            cli_out("  \\   ");
        }
        else
        {
            cli_out(" |    ");
        }
        for (j = 0; j < 8 && space_left > 0; j++)
        {
            if (TCG_to_TCs[i] % 2 == 1 && (skip_first--) <= 0)
            {
                cli_out("P%d ", j);
                space_left -= 3;
            }
            TCG_to_TCs[i] = TCG_to_TCs[i] >> 1;
        }
        sh_dnx_print_string_in_loop(space_left, " ", 0);
        if (i < 4)
        {
            cli_out("/  ");
        }
        else
        {
            cli_out(" | ");
        }

    }
    cli_out("|\n| ");
    for (i = 0; i < 8; i++)
    {
        if (i < 4)
        {
            cli_out("   \\-SP%d Group-/   ", i);
        }
        else
        {
            cli_out(" |     -SP%d-     | ", i);
        }
    }
    cli_out("|\n| ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, "    -----------    ", 0);
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, " ----------------- ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

    /*
     * iterates over q_pairs and prints the TCG's CIR rates
     */
    for (i = 0; i < port_nof_q_pairs; i++)
    {
        uint32 flags, kbits_sec_max, kbits_sec_min;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = i;  /* TCG number */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTCG, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, info.out_gport, i, &kbits_sec_min, &kbits_sec_max, &flags));
        cli_out(" CIR:%010uKbs ", kbits_sec_max);
    }
    cli_out("|\n| ");

    /*
     * iterates over q_pairs and prints the TCG's EIR rates
     */
    for (i = 0; i < port_nof_q_pairs; i++)
    {
        int weight, mode;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = i;  /* TCG number */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTCG, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode, &weight));
        if (mode == -1)
        {
            cli_out(" EIR:%010u    ", weight);
        }
        else
        {
            cli_out(" EIR:   none       ");
        }
        pps_base_q_pair++;
    }
    cli_out("|\n| ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
    cli_out
        ("         --------------------------------------------------------------------------------------------------------------------------------------         |\n| ");
    cli_out
        ("                                                      |                                          |                                                      |\n| ");
    cli_out
        ("                                                   All EIRs                                   All CIRs                                                  |\n| ");
    cli_out
        ("                                                      |                                          |                                                      |\n| ");
    cli_out
        ("                                               ---------------                            ---------------                                               |\n| ");
    cli_out
        ("                                               \\    -WFQ-    /                            |     -FQ-    |                                               |\n| ");
    cli_out
        ("                                                -------------                             ---------------                                               |\n| ");
    cli_out
        ("                                                      |                                          |                                                      |\n| ");
    cli_out
        ("                                                  ----------------------------------------------------                                                  |\n| ");
    cli_out
        ("                                                  \\   L                                          H   /                                                  |\n| ");
    cli_out
        ("                                                   \\                      -SP-                      /                                                   |\n|");
    cli_out
        ("                                                     ------------------------------------------------                                                    |\n|");
    cli_out
        ("_____________________________________________________________________________|___________________________________________________________________________|\n ");
    cli_out("                                                                             |\n\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - dump egq port shaping information
 */
shr_error_e
sh_dnx_egq_shaping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t egq_ports;
    bcm_port_t port;
    bcm_gport_t gport;
    uint32 if_speed, port_speed, min_speed, flags;
    bcm_core_t core;
    int channelized, egr_if;
    bcm_pbmp_t logical_ports;
    int port_count;
    int is_graphical, is_port_present;
    dnx_algo_port_type_e port_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_IS_PRESENT("port", is_port_present);
    SH_SAND_GET_BOOL("GRaphical", is_graphical);
    _SHR_PBMP_FIRST(logical_ports, port);
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    BCM_PBMP_COUNT(logical_ports, port_count);

    if (is_graphical)
    {
        if ((port_count == 1) && (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type)))
        {
            SHR_IF_ERR_EXIT(sh_dnx_egq_shaping_graphic(unit, port));
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Graphical option requires a single valid egress TM port %s%s%s",
                              EMPTY, EMPTY, EMPTY);
        }
    }
    else
    {
        /**
         * Print table header
         */
        PRT_TITLE_SET("EGQ shaping");
        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD("EGQ Port rate [Kbps]");
        PRT_COLUMN_ADD("Core");
        PRT_COLUMN_ADD("EGQ IF");
        PRT_COLUMN_ADD("EGQ IF rate [Kbps]");

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &egq_ports));

        /** if ports were provided then make sure to only print the egress TM ports, otherwise print all */
        if (is_port_present)
        {
            BCM_PBMP_AND(egq_ports, logical_ports);
        }

        /** iterate all logical egq_ports */
        BCM_PBMP_ITER(egq_ports, port)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            BCM_GPORT_LOCAL_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &flags));
            SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
            PRT_CELL_SET("%d", port);
            PRT_CELL_SET("%d", port_speed);
            PRT_CELL_SET("%d", core);
            PRT_CELL_SET("%d", egr_if);
            /**
             * interface speed get for non-channelized interface is not allowed. it's configured to the maximum allowed at HW
             */
            if (channelized)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_fabric_port_get(unit, gport, 0, &gport));
                SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &if_speed, &flags));
                PRT_CELL_SET("%d", if_speed);
            }
            else
            {
                PRT_CELL_SET("unlimited");
            }
        }
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - dump egq port compensation information
 */
shr_error_e
sh_dnx_egq_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t egq_ports;
    bcm_port_t port;
    bcm_gport_t gport;
    int comp;
    uint32 is_set;
    bcm_pbmp_t logical_ports;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_IS_PRESENT("value", is_set);

    /*
     * AND ports with egq ports. if port is not an egq port it will not be displayed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &egq_ports));
    BCM_PBMP_AND(egq_ports, logical_ports);

    if (is_set)
    {
        SH_SAND_GET_INT32("value", comp);
    }
    else
    {
        /**
         * Print table header
         */
        PRT_TITLE_SET("EGQ Compensation");
        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD("Compensation [Bytes]");
    }

    /** iterate all logical egq_ports */
    BCM_PBMP_ITER(egq_ports, port)
    {
        if (is_set)
        {
            BCM_GPORT_LOCAL_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, comp));
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            BCM_GPORT_LOCAL_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_get(unit, gport, 0, bcmCosqControlPacketLengthAdjust, &comp));
            PRT_CELL_SET("%d", port);
            PRT_CELL_SET("%d", comp);
        }
    }
    if (!is_set)
    {
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress rqp couter rate
 */
shr_error_e
sh_dnx_egr_rqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_id;
    int counter_idx, nof_counter;
    dnx_egq_counter_rate_info_t rate_info[DNX_EGQ_RQP_COUNTER_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("Egress RQP Rate on core (%d)", core);
        PRT_COLUMN_ADD("No.");
        PRT_COLUMN_ADD("Coutner");
        PRT_COLUMN_ADD("Rate");
        PRT_COLUMN_ADD("Unit");

        SHR_IF_ERR_EXIT(sh_dnx_egq_rqp_counter_rate_get(unit, core, rate_info, &nof_counter));

        for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", counter_idx);
            PRT_CELL_SET("%s", rate_info[counter_idx].name);
            if (!rate_info[counter_idx].count_ovf)
            {
                PRT_CELL_SET("%u", rate_info[counter_idx].rate);
            }
            else
            {
                PRT_CELL_SET("%s", "overflow!!!");
            }
            if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
            {
            PRT_CELL_SET("%s", "kpps")}
            else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
            {
                PRT_CELL_SET("%s", "kbps");
            }
            else
            {
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type,
                                  EMPTY, EMPTY);
            }
        }

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress pqp couter rate
 */
shr_error_e
sh_dnx_egr_pqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    uint32 otm;
    int logical_port, tc;
    int qpair, base_qp;
    int counter_idx;
    dnx_egq_counter_rate_info_t rate_info[DNX_EGQ_PQP_COUNTER_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("port", logical_port);
    SH_SAND_GET_INT32("TrafficClasS", tc);

    SHR_IF_ERR_EXIT(sh_dnx_egq_pqp_counter_rate_get(unit, core, logical_port, tc, rate_info));

    if ((-1 != logical_port) && (-1 != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qp));
        qpair = base_qp + tc;
        PRT_TITLE_SET("Egress PQP Rate on QPair: port=%d, tc=%d, qpair=%d", logical_port, tc, qpair);
    }
    else if (-1 != logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &otm));
        PRT_TITLE_SET("Egress PQP Rate on OTM: port=%d, otm=%d", logical_port, otm);
    }
    else
    {
        PRT_TITLE_SET("Egress PQP Rate on core (%d)", core);
    }

    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    for (counter_idx = 0; counter_idx < DNX_EGQ_PQP_COUNTER_NUM; counter_idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", counter_idx);
        PRT_CELL_SET("%s", rate_info[counter_idx].name);
        if (!rate_info[counter_idx].count_ovf)
        {
            PRT_CELL_SET("%u", rate_info[counter_idx].rate);
        }
        else
        {
            PRT_CELL_SET("%s", "overflow!!!");
        }
        if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
        {
        PRT_CELL_SET("%s", "kpps")}
        else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
        {
            PRT_CELL_SET("%s", "kbps");
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type, EMPTY,
                              EMPTY);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress epep couter rate
 */
shr_error_e
sh_dnx_egr_epep_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    uint32 otm;
    int logical_port, tc;
    int qpair, base_qp;
    int counter_idx, nof_counter;
    dnx_egq_counter_rate_info_t rate_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("port", logical_port);
    SH_SAND_GET_INT32("TrafficClasS", tc);

    SHR_IF_ERR_EXIT(sh_dnx_egq_epep_counter_rate_get(unit, core, logical_port, tc, rate_info, &nof_counter));

    if ((-1 != logical_port) && (-1 != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qp));
        qpair = base_qp + tc;
        PRT_TITLE_SET("Egress EPEP Rate on QPair: port=%d, tc=%d, hr_id=%d", logical_port, tc, qpair);
    }
    else if (-1 != logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &otm));
        PRT_TITLE_SET("Egress EPEP Rate on OTM: port=%d, otm=%d", logical_port, otm);
    }
    else
    {
        PRT_TITLE_SET("Egress EPEP Rate on core(%d)", core);
    }

    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", counter_idx);
        PRT_CELL_SET("%s", rate_info[counter_idx].name);
        if (!rate_info[counter_idx].count_ovf)
        {
            PRT_CELL_SET("%u", rate_info[counter_idx].rate);
        }
        else
        {
            PRT_CELL_SET("%s", "overflow!!!");
        }
        if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
        {
        PRT_CELL_SET("%s", "kpps")}
        else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
        {
            PRT_CELL_SET("%s", "kbps");
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type, EMPTY,
                              EMPTY);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress epni according to the scheme
 */
shr_error_e
sh_dnx_egr_epni_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    int scheme, bw;
    int counter_idx, nof_counter;
    dnx_egq_counter_rate_info_t rate_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("SCheMe", scheme);
    SH_SAND_GET_INT32("BandWidth", bw);

    switch (scheme)
    {
        case DNX_EGR_EPNI_RATE_SCHEME_TOTAL:
            PRT_TITLE_SET("Egress EPNI Rate on the device");
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_QP:
            PRT_TITLE_SET("Egress EPNI Rate on QPair (%d)", bw);
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_PORT:
            PRT_TITLE_SET("Egress EPNI Rate on Port (%d)", bw);
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_IF:
            PRT_TITLE_SET("Egress EPNI Rate on EGQ Interface (%d)", bw);
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_MIRROR:
            PRT_TITLE_SET("Egress EPNI Rate on Mirror (%d)", bw);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected EPNI rate scheme %d\n", scheme);
            break;
    }

    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(sh_dnx_egq_epni_counter_rate_get(unit, core, scheme, bw, rate_info, &nof_counter));
        if (0 != core)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        }
        PRT_CELL_SET("%d", core);
        for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
        {
            if (0 != counter_idx)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%d", counter_idx);
            PRT_CELL_SET("%s", rate_info[counter_idx].name);
            if (!rate_info[counter_idx].count_ovf)
            {
                PRT_CELL_SET("%u", rate_info[counter_idx].rate);
            }
            else
            {
                PRT_CELL_SET("%s", "overflow!!!");
            }
            if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
            {
            PRT_CELL_SET("%s", "kpps")}
            else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
            {
                PRT_CELL_SET("%s", "kbps");
            }
            else
            {
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type,
                                  EMPTY, EMPTY);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress queuing couter rate
 */
shr_error_e
sh_dnx_egr_queuing_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    uint32 otm;
    int logical_port, tc;
    int qpair, base_qp;
    int counter_idx, nof_counter, nof_counter_total = 0;
    dnx_egq_counter_rate_info_t rate_info[DNX_EGQ_COUNTER_NUM_SUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("port", logical_port);
    SH_SAND_GET_INT32("TrafficClasS", tc);

    if ((-1 != logical_port) && (-1 != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qp));
        qpair = base_qp + tc;
        PRT_TITLE_SET("Egress Queuing Rate on HR: port=%d, tc=%d, qpair=%d", logical_port, tc, qpair);
    }
    else if (-1 != logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &otm));
        PRT_TITLE_SET("Egress Queuing Rate on OTM: port=%d, otm=%d", logical_port, otm);
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_egq_rqp_counter_rate_get(unit, core, rate_info, &nof_counter));
        nof_counter_total += nof_counter;

        PRT_TITLE_SET("Egress Queuing Rate on core (%d)", core);
    }

    SHR_IF_ERR_EXIT(sh_dnx_egq_pqp_counter_rate_get(unit, core, logical_port, tc, &rate_info[nof_counter_total]));
    nof_counter_total += DNX_EGQ_PQP_COUNTER_NUM;
    SHR_IF_ERR_EXIT(sh_dnx_egq_epep_counter_rate_get
                    (unit, core, logical_port, tc, &rate_info[nof_counter_total], &nof_counter));
    nof_counter_total += nof_counter;

    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    for (counter_idx = 0; counter_idx < nof_counter_total; counter_idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", counter_idx);
        PRT_CELL_SET("%s", rate_info[counter_idx].name);
        if (!rate_info[counter_idx].count_ovf)
        {
            PRT_CELL_SET("%u", rate_info[counter_idx].rate);
        }
        else
        {
            PRT_CELL_SET("%s", "overflow!!!");
        }
        if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
        {
        PRT_CELL_SET("%s", "kpps")}
        else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
        {
            PRT_CELL_SET("%s", "kbps");
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type, EMPTY,
                              EMPTY);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
