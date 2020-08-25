/** \file diag_dnxc_pbmp.c
 *
 * Diagnostic pack for PBMP
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
/*soc*/
#ifdef BCM_DNXF_SUPPORT
#include <appl/diag/dnxf/diag_dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif

#define PRINT_PORT_PBMP_CELLS(name, pbmp) \
    do { \
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE); \
        PRT_CELL_SET(name); \
        sal_memset(buf, 0, (sizeof(char) * FORMAT_PBMP_MAX)); \
        PRT_CELL_SET("%s", shr_pbmp_range_format(pbmp, buf, sizeof(buf))); \
    } while (0)

/* *INDENT-OFF* */
sh_sand_option_t dnxc_pbmp_options[] = {
    {"port", SAL_FIELD_TYPE_STR, "A port/pbmp string.", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/* *INDENT-ON* */
static shr_error_e
diag_dnxc_pbmp_print_all(
    int unit,
    sh_sand_control_t * sand_control)
{
    char buf[FORMAT_PBMP_MAX];
    bcm_port_config_t port_config;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    PRT_TITLE_SET("Current Bitmaps");
    PRT_COLUMN_ADD("Ports");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bit(s)");

    if (SOC_PBMP_NOT_NULL(port_config.fe))
    {
        PRINT_PORT_PBMP_CELLS("FE", port_config.fe);
    }
    if (SOC_PBMP_NOT_NULL(port_config.ge))
    {
        PRINT_PORT_PBMP_CELLS("GE", port_config.ge);
    }
    if (SOC_PBMP_NOT_NULL(port_config.xe))
    {
        PRINT_PORT_PBMP_CELLS("XE", port_config.xe);
    }
    if (SOC_PBMP_NOT_NULL(port_config.ce))
    {
        PRINT_PORT_PBMP_CELLS("CE", port_config.ce);
    }
    if (SOC_PBMP_NOT_NULL(port_config.cd))
    {
        PRINT_PORT_PBMP_CELLS("CD", port_config.cd);
    }
    if (SOC_PBMP_NOT_NULL(port_config.cc))
    {
        PRINT_PORT_PBMP_CELLS("CC", port_config.cc);
    }
    if (SOC_PBMP_NOT_NULL(port_config.le))
    {
        PRINT_PORT_PBMP_CELLS("LE", port_config.le);
    }
    if (SOC_PBMP_NOT_NULL(port_config.e))
    {
        PRINT_PORT_PBMP_CELLS("E", port_config.e);
    }
    if (SOC_PBMP_NOT_NULL(port_config.xl))
    {
        PRINT_PORT_PBMP_CELLS("XL", port_config.xl);
    }
    if (SOC_PBMP_NOT_NULL(port_config.il))
    {
        PRINT_PORT_PBMP_CELLS("IL", port_config.il);
    }
    if (SOC_PBMP_NOT_NULL(port_config.rcy))
    {
        PRINT_PORT_PBMP_CELLS("RCY", port_config.rcy);
    }
    if (SOC_PBMP_NOT_NULL(port_config.port))
    {
        PRINT_PORT_PBMP_CELLS("PORT", port_config.port);
    }
    if (SOC_PBMP_NOT_NULL(port_config.cpu))
    {
        PRINT_PORT_PBMP_CELLS("CMIC", port_config.cpu);
    }
    if (SOC_PBMP_NOT_NULL(port_config.sci))
    {
        PRINT_PORT_PBMP_CELLS("SCI", port_config.sci);
    }
    if (SOC_PBMP_NOT_NULL(port_config.sfi))
    {
        PRINT_PORT_PBMP_CELLS("SFI", port_config.sfi);
    }
    if (SOC_PBMP_NOT_NULL(port_config.spi))
    {
        PRINT_PORT_PBMP_CELLS("SPI", port_config.spi);
    }
    if (SOC_PBMP_NOT_NULL(port_config.spi_subport))
    {
        PRINT_PORT_PBMP_CELLS("SPI_SUB", port_config.spi_subport);
    }
    if (SOC_PBMP_NOT_NULL(port_config.stack_ext))
    {
        PRINT_PORT_PBMP_CELLS("STK", port_config.stack_ext);
    }
    if (SOC_PBMP_NOT_NULL(port_config.hg))
    {
        PRINT_PORT_PBMP_CELLS("HiG", port_config.hg);
    }
    if (SOC_PBMP_NOT_NULL(port_config.tdm))
    {
        PRINT_PORT_PBMP_CELLS("TDM", port_config.tdm);
    }
    if (SOC_PBMP_NOT_NULL(port_config.pon))
    {
        PRINT_PORT_PBMP_CELLS("PON", port_config.pon);
    }
    if (SOC_PBMP_NOT_NULL(port_config.llid))
    {
        PRINT_PORT_PBMP_CELLS("LLID", port_config.llid);
    }
    if (SOC_PBMP_NOT_NULL(port_config.sat))
    {
        PRINT_PORT_PBMP_CELLS("SAT", port_config.sat);
    }
    if (SOC_PBMP_NOT_NULL(port_config.ipsec))
    {
        PRINT_PORT_PBMP_CELLS("IPSEC", port_config.ipsec);
    }
    if (SOC_PBMP_NOT_NULL(port_config.control))
    {
        PRINT_PORT_PBMP_CELLS("CTRL", port_config.control);
    }
    if (SOC_PBMP_NOT_NULL(port_config.eventor))
    {
        PRINT_PORT_PBMP_CELLS("EVENTOR", port_config.eventor);
    }
    if (SOC_PBMP_NOT_NULL(port_config.olp))
    {
        PRINT_PORT_PBMP_CELLS("OLP", port_config.olp);
    }
    if (SOC_PBMP_NOT_NULL(port_config.oamp))
    {
        PRINT_PORT_PBMP_CELLS("OAMP", port_config.oamp);
    }
    if (SOC_PBMP_NOT_NULL(port_config.erp))
    {
        PRINT_PORT_PBMP_CELLS("SFI", port_config.erp);
    }
    if (SOC_PBMP_NOT_NULL(port_config.nif))
    {
        PRINT_PORT_PBMP_CELLS("NIF", port_config.nif);
    }
    if (SOC_PBMP_NOT_NULL(port_config.all))
    {
        PRINT_PORT_PBMP_CELLS("ALL", port_config.all);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_pbmp_print_pbmp(
    int unit,
    char *bitmap_str,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    char pbmp_str[FORMAT_PBMP_MAX];
    char buf[FORMAT_PBMP_MAX];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(pbmp);
    sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));

    if (parse_pbmp(unit, bitmap_str, &pbmp) < 0)
    {
        cli_out("PBMP: Invalid pbmp string (%s); use 'pbmp ?' for more info.\n", bitmap_str);
        SHR_EXIT();
    }

    format_pbmp(unit, pbmp_str, sizeof(pbmp_str), pbmp);

    PRT_TITLE_SET("Ports to Bits");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Port(s)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bit(s)");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", pbmp_str);
    sal_memset(buf, 0, (sizeof(char) * FORMAT_PBMP_MAX));
    PRT_CELL_SET("%s", shr_pbmp_range_format(pbmp, buf, sizeof(buf)));

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * { PBMP Printout functions
 */
shr_error_e
cmd_dnxc_pbmp(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *ports = NULL;
    int is_port_present = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("port", is_port_present);
    if (TRUE == is_port_present)
    {
        SH_SAND_GET_STR("port", ports);
        diag_dnxc_pbmp_print_pbmp(unit, ports, sand_control);
    }
    else
    {
        diag_dnxc_pbmp_print_all(unit, sand_control);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * } PBMP Printout functions
 */

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_pbmp_man = {
    .brief =    "Convert port bitmap string to hex.",
    .full =     "Parameters: <pbmp>\n"
                "        Converts a pbmp string into a hardware port bitmap.  A pbmp string\n"
                "        is a single port, or a group of ports specified in a list using ','\n"
                "        to separate them and '-' for ranges, e.g. 1-8,25,cpu.  Ports may be\n"
                "        specified using the one-based port number (1-29) or port type and\n"
                "        zero-based number (fe0-fe23,ge0-ge7).  'cpu' is the CPU port,\n"
                "        'fe' is all FE ports, 'ge' is all GE ports, 'e' is all ethernet\n"
                "        ports, 'all' is all ports, and 'none' is no ports (0x0).\n"
                "        A '~' may be used to exclude port previously given (e.g. e,~fe19)\n"
                "        Acceptable strings and values also depend on the chip being used.\n"
                "        A pbmp may also be given as a raw hex (0x) number, e.g. 0xbffffff.\n",
    .synopsis = "Parameters: <pbmp>",
    .examples = "\n",
};


/* *INDENT-ON* */
