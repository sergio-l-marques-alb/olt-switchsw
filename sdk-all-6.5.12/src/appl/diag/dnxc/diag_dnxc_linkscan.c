/** \file diag_dnxc_linkscan.c
 *
 * linkscan diagnostic pack for fabric
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/* shared */
#include <shared/bsl.h>
/* appl */
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
/*bcm*/
#include <bcm/fabric.h>
#include <bcm/port.h>
#include <bcm/link.h>

static int linkscan_interval_saved[BCM_MAX_NUM_UNITS];


/* *INDENT-OFF* */
static sh_sand_man_t dnxc_linkscan_on_man = {
    "Enable Linkscan",
    "Enable linkscan.",
    "linkscan on",
    "linkscan on",
};
/* *INDENT-ON* */


/* *INDENT-OFF* */
static sh_sand_man_t dnxc_linkscan_off_man = {
    "Disable Linkscan",
    "Disable linkscan.",
    "linkscan off",
    "linkscan off",
};
/* *INDENT-ON* */


/* *INDENT-OFF* */
static sh_sand_option_t dnxc_linkscan_interval_options[] = {
    {"variable", SAL_FIELD_TYPE_INT32, "linkscan interval value", NULL, NULL},
    {NULL}
};

static sh_sand_man_t dnxc_linkscan_interval_man = {
    "Configure Linkscan Interval",
    "Configure linkscan polling interval, which is used in the linkscan thread.",
    "linkscan interval [value]",
    "linkscan interval 250000",
};
/* *INDENT-ON* */

typedef enum
{
    DIAG_LINKSCAN_MODE_NONE = BCM_LINKSCAN_MODE_NONE,
    DIAG_LINKSCAN_MODE_SW = BCM_LINKSCAN_MODE_SW,
    DIAG_LINKSCAN_MODE_HW = BCM_LINKSCAN_MODE_HW
} diag_linkscan_mode_t;

/* Link the values in the two enums*/
static sh_sand_enum_t dnxc_linkscan_mode_enum_table[] = {
    {"NONE", DIAG_LINKSCAN_MODE_NONE},
    {"SW", DIAG_LINKSCAN_MODE_SW},
    {"HW", DIAG_LINKSCAN_MODE_HW},
    {NULL}
};

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_linkscan_port_options[] = {
    {"variable",    SAL_FIELD_TYPE_PORT,    "filter on Port ID or port name",   NULL,   NULL                                 },
    {"mode",        SAL_FIELD_TYPE_ENUM,    "NONE/SW/HW",                       NULL,   (void *)dnxc_linkscan_mode_enum_table},
    {NULL}
};

static sh_sand_man_t dnxc_linkscan_port_man = {
    "Configure Linkscan Mode",
    "Configure linkscan mode for given ports.",
    "linkscan port [integer|port_name|'tm'|'nif'|'fabric'|'all'] [mode=<none|sw|hw>]",
    "linkscan port xe13 mode=hw - configure hardware linkscan mode for port xe13\n"
    "linkscan port sfi100-sfi106 mode=sw - configure software linkscan mode for fabric ports from 100 to 106\n"
    "linkscan port nif mode=sw - configure software linkscan mode for all fap ports\n",
};
/* *INDENT-ON* */


/* *INDENT-OFF* */
static sh_sand_man_t dnxc_linkscan_info_man = {
    "Show Linkscan Infomation",
    "Show linkscan infomation, including enabling status, mode and interval.",
    "linkscan info",
    "linkscan info",
};
/* *INDENT-ON* */

/**
 * \brief - enable linkscan globally
 */
static shr_error_e
cmd_dnxc_linkscan_on(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int us, linkscan_interval_default;
    SHR_FUNC_INIT_VARS(unit);

    linkscan_interval_default = 250000  ;

    us = linkscan_interval_saved[unit] ? linkscan_interval_saved[unit] : linkscan_interval_default;

    if (us == 0)
    {
        cli_out("WARNING: Cannot enable Linkscan if the polling interval is zero\n");
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, us));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - disable linkscan globally
 */
static shr_error_e
cmd_dnxc_linkscan_off(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int us, rv, linkscan_interval_default;
    SHR_FUNC_INIT_VARS(unit);

    linkscan_interval_default = 250000  ;

    rv = bcm_linkscan_enable_get(unit, &us);
    if (SHR_FAILURE(rv) || us <= 0)
    {
        us = linkscan_interval_default;
    }

    linkscan_interval_saved[unit] = us;

    SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure linkscan interval
 */
static shr_error_e
cmd_dnxc_linkscan_interval(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int us;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("variable", us);

    SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, us));

    linkscan_interval_saved[unit] = us;

exit:SHR_FUNC_EXIT;
}

/**
 * \brief - configure linkscan mode
 */
static shr_error_e
cmd_dnxc_linkscan_port(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode;
    bcm_pbmp_t pbm;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("variable", pbm);
    SH_SAND_GET_ENUM("mode", mode);

    SHR_IF_ERR_EXIT(bcm_linkscan_mode_set_pbm(unit, pbm, mode));

exit:SHR_FUNC_EXIT;
}

/**
 * \brief - dump linkscan info
 */
static shr_error_e
cmd_dnxc_linkscan_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int us;
    bcm_pbmp_t pbm_sw, pbm_hw, pbm_temp, pbm;
    bcm_port_config_t pcfg;
    soc_port_t port, dport;
    char buf[FORMAT_PBMP_MAX];
    char pfmt[SOC_PBMP_FMT_LEN];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &us));

    if (us)
    {
        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &pcfg));

        BCM_PBMP_CLEAR(pbm_sw);
        BCM_PBMP_CLEAR(pbm_hw);

        DPORT_BCM_PBMP_ITER(unit, pcfg.port, dport, port)
        {
            int mode;

            SHR_IF_ERR_EXIT(bcm_linkscan_mode_get(unit, port, &mode));

            switch (mode)
            {
                case BCM_LINKSCAN_MODE_SW:
                    BCM_PBMP_PORT_ADD(pbm_sw, port);
                    break;
                case BCM_LINKSCAN_MODE_HW:
                    BCM_PBMP_PORT_ADD(pbm_hw, port);
                    break;
                default:
                    break;
            }
        }

        cli_out("Linkscan enabled\n");
        cli_out("  Software polling interval: %d usec\n", us);
        format_bcm_pbmp(unit, buf, sizeof(buf), pbm_sw);
        cli_out("  Software Port BitMap %s: %s\n", SOC_PBMP_FMT(pbm_sw, pfmt), buf);
        format_bcm_pbmp(unit, buf, sizeof(buf), pbm_hw);
        cli_out("  Hardware Port BitMap %s: %s\n", SOC_PBMP_FMT(pbm_hw, pfmt), buf);
        BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
        BCM_PBMP_OR(pbm_temp, pbm_hw);
        BCM_PBMP_REMOVE(pbm_temp, PBMP_HG_SUBPORT_ALL(unit));
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
        BCM_PBMP_XOR(pbm, pbm_temp);
        format_bcm_pbmp(unit, buf, sizeof(buf), pbm);
        cli_out("  Disabled Port BitMap %s: %s\n", SOC_PBMP_FMT(pbm, pfmt), buf);
    }
    else
    {
        cli_out("Linkscan disabled\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * } LOCAL DIAG PACK
 */

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_linkscan_cmds[] = {
    /*keyword,          action,                            command,                              options,                                  man                  */
    {"on",              cmd_dnxc_linkscan_on,              NULL,                                 NULL,                                     &dnxc_linkscan_on_man},
    {"off",             cmd_dnxc_linkscan_off,             NULL,                                 NULL,	                                   &dnxc_linkscan_off_man},
    {"interval",        cmd_dnxc_linkscan_interval,        NULL,                                 dnxc_linkscan_interval_options,	       &dnxc_linkscan_interval_man},
    {"port",            cmd_dnxc_linkscan_port,            NULL,                                 dnxc_linkscan_port_options,	           &dnxc_linkscan_port_man},
    {"info",            cmd_dnxc_linkscan_info,            NULL,                                 NULL,	                                   &dnxc_linkscan_info_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_linkscan_man = {
    "Misc facilities for displaying dnxc linkscan information",
    NULL,
    NULL,
    NULL,
};

const char cmd_dnxc_linkscan_usage[] = "Display dnxc linkscan information";
/* *INDENT-ON* */
