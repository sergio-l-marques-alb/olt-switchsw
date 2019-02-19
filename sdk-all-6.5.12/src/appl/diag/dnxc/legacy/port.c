/* 
 * $Id: port.c,v 1.55 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        port.c
 * Purpose:     PORT CLI commands
 *
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/util.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>

#if defined PORTMOD_SUPPORT
#include <appl/diag/phymod/phymod_sdk_diag.h>
#include <appl/diag/phymod/phymod_symop.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#endif

#include <appl/diag/dnxc/legacy/port.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/port.h>

#include <soc/phy.h>
#include <soc/eyescan.h>
#include <soc/phyreg.h>
#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnxc/legacy/dnxc_defs.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <bcm_int/dnxf/port.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/legacy/dnx_config_defs.h>
#include <soc/dnx/legacy/dnx_config_imp_defs.h>
#include <soc/dnx/legacy/ARAD/arad_defs.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/mbcm.h>
#endif
#ifdef BCM_JER2_JERICHO_SUPPORT
#include <soc/dnx/legacy/JER/jer_nif.h>
#endif

extern cmd_result_t port_phy_control_update(
    int u,
    bcm_port_t p,
    bcm_port_phy_control_t type,
    uint32 val,
    uint32 flags,
    int *print_header);
#if defined PHYMOD_SUPPORT
extern int phymod_sym_access(
    int u,
    args_t * a,
    int intermediate,
    soc_pbmp_t * pbm);
#endif

#if defined PORTMOD_SUPPORT
#define MAX_CORE_ACCESS_STRUCTURES_PER_PORT (6)
#endif

#define _call_bpi(pbm, pbm_mask) \
    tmp_pbm = pbm_mask; \
    BCM_PBMP_AND(tmp_pbm, pbm); \
    PBMP_ITER(tmp_pbm, p) { \
        brief_port_info(unit, p, &info_all[p], mask); \
    }

#define _call_pdi(unit, p, mp, s) \
        if (_dnx_port_disp_iter(unit, p, mp, s) != CMD_OK) { \
             sal_free(info_all); \
             sal_free(ability_all); \
             return CMD_FAIL; \
        }
#define DIAG_DNXC_PRBS_INJECTION_PHY 0
#define DIAG_DNXC_PRBS_INJECTION_MAC 1
#define DIAG_DNXC_PRBS_POLY_DEFAULT -1
/*local functions*/
STATIC cmd_result_t dnxc_phy_prbs(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_power(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_diag_phy_unit_get(
    int unit_value,
    int *phy_dev);

STATIC cmd_result_t dnxc_phy_diag_phy_if_get(
    char *if_str,
    int *dev_if);

STATIC cmd_result_t dnxc_phy_diag(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_diag_prbs(
    int unit,
    bcm_pbmp_t pbmp,
    args_t * args);

STATIC cmd_result_t dnxc_phy_diag_loopback(
    int unit,
    bcm_pbmp_t pbmp,
    args_t * args);

STATIC cmd_result_t dnxc_phy_control(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_measure(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_cl72(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_info(
    int unit,
    args_t * args);

STATIC cmd_result_t dnxc_phy_raw(
    int unit,
    args_t * args);

char if_dnx_port_stat_usage[] =
    "Display info about port status in table format.\n"
    "    Link scan modes:\n"
    "        SW = software\n"
    "        HW = hardware\n"
    "    Learn operations (source lookup failure control):\n"
    "        F = SLF packets are forwarded\n"
    "        C = SLF packets are sent to the CPU\n"
    "        A = SLF packets are learned in L2 table\n"
    "        D = SLF packets are discarded.\n"
    "    Pause:\n" "        TX = Switch will transmit pause packets\n" "        RX = Switch will obey pause packets\n";

/*
 * Function:
 *     if_dnx_port_stat
 * Purpose:
 *     Table display of port status information.
 * Parameters:
 *     unit - Device number
 *     args - Pointer to args
 * Returns:
 *     CMD_OK   - Success
 *     CMD_FAIL - Failure
 */
cmd_result_t
if_dnx_port_stat(
    int unit,
    args_t * a)
{
    pbmp_t pbm, tmp_pbm;
    bcm_port_info_t *info_all;
    bcm_port_config_t pcfg;
    soc_port_t p;
    int r;
    char *c;
    uint32 mask;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE)
    {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL)
    {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    }
    else if (parse_bcm_pbmp(unit, c, &pbm) < 0)
    {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm))
    {
        cli_out("No ports specified.\n");
        return CMD_OK;
    }

    mask = BCM_PORT_ATTR_ALL_MASK;

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t), "if_port_stat");
    if (info_all == NULL)
    {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    sal_memset(info_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t));

    
    PBMP_ITER(pbm, p)
    {
        port_info_init(unit, p, &info_all[p], mask);
        if ((r = bcm_port_selective_get(unit, p, &info_all[p])) < 0)
        {
            cli_out("%s: Error: Could not get port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            sal_free(info_all);
            return (CMD_FAIL);
        }
    }

    brief_port_info_header(unit);

    if (soc_property_get(unit, spn_DPORT_MAP_ENABLE, TRUE))
    {
        /*
         * If port mapping is enabled, then use port order 
         */
        _call_bpi(pbm, pcfg.port);
    }
    else
    {
        /*
         * If no port mapping, ensure that ports are grouped by type 
         */
        _call_bpi(pbm, pcfg.fe);
        _call_bpi(pbm, pcfg.ge);
        _call_bpi(pbm, pcfg.xe);
        _call_bpi(pbm, pcfg.hg);
    }

    sal_free(info_all);

    return CMD_OK;
}

char if_dnx_phy_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "  phy <option> [args...]\n"
    "Default: [int] <ports> [regnum [value/devad] [value]]\n"
    "Subcommands: raw, info, diag, list, nz, reset\n"
    "             power, prbs , dumpall, control, int and <blank> for read and write\n"
#else
    "Subcommands:\n"
    "               int <ports>\n"
    "               info\n"
    "               list [pattern] print register matching the specified pattern/address\n"
    "               nz [pattern] print registers matching the pattern that the value is not zero\n"
    "               verbose Port=<ports> Verbose=<value>\n"
    "               power <ports> mode=<on|off>\n"
    "               prbs <ports> <set|get|clear> Mode=<phy|mac> [Polynomial=<value>]\n"
    "                       [Loopback=<true|false>] [Invert=<value>] [interval=<value>]\n"
    "               diag <ports> <eyescan|dsc|loopback|prbs> <parameters>\n"
    "               measure <ports> [<tx|rx>]\n"
    "               cl72 <ports> <on|off>\n"
    "               dumpall <c22|c45>\n"
    "               control <ports> [phy_control_type=value]\n"
    "               Direct Access (without symbol table):\n"
    "                      phy <port> direct [reg address] [value]\n"
    "NOTES: <ports> is a standard port bitmap,\n"
    "       e.g. fe for all 10/100 ports, fe5-fe7 for three FE's, etc. (see \"help pbmp\").\n"
    "       Add 'external' to the end of the cmd for external phy diagnostic (relevant for 'dsc').\n"
    "       Add 'state' to the end of the cmd for dumpping the detailed DSC info (relevant for 'dsc').\n"
#endif
    ;

#ifdef PORTMOD_SUPPORT
STATIC int
dnxc_phy_direct_access(
    int u,
    args_t * a,
    int intermediate,
    soc_pbmp_t * pbm)
{
    int rv, p, dport;
    int nof_phys_structs;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[MAX_CORE_ACCESS_STRUCTURES_PER_PORT];
    uint8 is_write_op = ARG_CNT(a) > 1;
    uint32 reg_addr;
    uint32 reg_val;
    int i;
    char *c;

    if (ARG_CNT(a) == 0)
    {
        cli_out("please supply address\n");
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL)
    {
        return CMD_USAGE;
    }
    reg_addr = soc_parse_integer(c);
    if (is_write_op)
    {
        c = ARG_GET(a);
        reg_val = soc_parse_integer(c);
    }

    if (portmod_access_get_params_t_init(u, &params))
    {
        cli_out("portmod_access_get_params_t_init failed\n");
        return CMD_FAIL;
    }
    if (intermediate)
    {
        params.phyn = 0;
    }
    DPORT_SOC_PBMP_ITER(u, *pbm, dport, p)
    {
        rv = portmod_port_phy_lane_access_get(u, p, &params, MAX_CORE_ACCESS_STRUCTURES_PER_PORT, phy_access,
                                              &nof_phys_structs, NULL);
        if (rv != CMD_OK)
        {
            return rv;
        }
        for (i = 0; i < nof_phys_structs; i++)
        {
            if (is_write_op)
            {
                rv = phymod_phy_reg_write(&(phy_access[i]), reg_addr, reg_val);
            }
            else
            {
                rv = phymod_phy_reg_read(&(phy_access[i]), reg_addr, &reg_val);
                if (rv == CMD_OK)
                {
                    cli_out("Port %s%s(0x%02x):\n",
                            SOC_PORT_NAME(u, p), intermediate ? " (int)" : "", phy_access[i].access.addr);
                    cli_out("DIRECT [0x%x] = 0x%x\n", reg_addr, reg_val);

                }
            }
            if (rv != CMD_OK)
            {
                cli_out("Failure access DIRECT [0x%x]\n", reg_addr);
                return rv;
            }
        }
    }

    return CMD_OK;

}

STATIC int
dnxc_phy_portmod_sym_access(
    int u,
    args_t * a,
    int intermediate,
    soc_pbmp_t * pbm)
{
    phymod_symbols_iter_t iter;
    phymod_symbols_t *symbols_table = NULL;
    int rv, p, dport;
    int i = 0;
    char hdr[32];
    phymod_phy_access_t phy_access[MAX_CORE_ACCESS_STRUCTURES_PER_PORT];
    int nof_phys_structs;
    portmod_access_get_params_t params;

    portmod_access_get_params_t_init(u, &params);
    if (intermediate)
    {
        params.phyn = 0;
    }
    rv = phymod_symop_init(&iter, a);
    if (rv != CMD_OK)
    {
        return rv;
    }

    DPORT_SOC_PBMP_ITER(u, *pbm, dport, p)
    {
        rv = portmod_port_phy_lane_access_get(u, p, &params, MAX_CORE_ACCESS_STRUCTURES_PER_PORT, phy_access,
                                              &nof_phys_structs, NULL);
        if (rv != CMD_OK)
        {
            phymod_symop_cleanup(&iter);
            return rv;
        }
        for (i = 0; i < nof_phys_structs; i++)
        {
            rv = sal_snprintf(hdr, sizeof(hdr), "Port %s%s(0x%02x):\n",
                              SOC_PORT_NAME(u, p), intermediate ? " (int)" : "", phy_access[i].access.addr);
            if (rv >= sizeof(hdr))
            {
                continue;
            }
            rv = phymod_diag_symbols_table_get(&phy_access[i], &symbols_table);
            if (rv != CMD_OK)
            {
                phymod_symop_cleanup(&iter);
                return rv;
            }
            rv = phymod_symop_exec(&iter, symbols_table, &phy_access[i], hdr);
            if (rv != CMD_OK)
            {
                phymod_symop_cleanup(&iter);
                return rv;
            }
        }
    }

    return phymod_symop_cleanup(&iter);
}
#endif /* PORTMOD_SUPPORT */

#define MAX_PORT_CORES 7

/*
 * Function:     if_phy
 * Purpose:    Show/configure phy registers.
 * Parameters:    u - SOC unit #
 *        a - pointer to args
 * Returns:    CMD_OK/CMD_FAIL/
 */
cmd_result_t
if_dnx_phy(
    int unit,
    args_t * a)
{

    char *c;
    soc_pbmp_t pbm, pbm_temp, pbm_phys;
    int intermediate = 0;
    char pfmt[SOC_PBMP_FMT_LEN];

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }
#ifdef PORTMOD_SUPPORT
    /*
     * register print function
     */
    phymod_diag_print_func = cli_out;
#endif

    if ((c = ARG_GET(a)) == NULL)
    {
        return CMD_USAGE;
    }

    /*******************************/
    /******    Info support    ********/
    /*******************************/

    if (c != NULL && sal_strcasecmp(c, "info") == 0)
    {
        return dnxc_phy_info(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "power") == 0)
    {
        return dnxc_phy_power(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "prbs") == 0)
    {
        return dnxc_phy_prbs(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "control") == 0)
    {
        return dnxc_phy_control(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "measure") == 0)
    {
        return dnxc_phy_measure(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "cl72") == 0)
    {
        return dnxc_phy_cl72(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "diag") == 0)
    {
        return dnxc_phy_diag(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "raw") == 0)
    {
        return dnxc_phy_raw(unit, a);
    }

    /*******************************/
    /******    int support        ********/
    /*******************************/

    if (sal_strcasecmp(c, "int") == 0)
    {
        intermediate = 1;
        c = ARG_GET(a);
        if (c == NULL)
        {
            cli_out("        If the int option is given, the intermediate PHY for the port is\n"
                    "        used instead of the outer PHY.\n" "        Example: phy int 6\n");
            return CMD_FAIL;
        }
    }

    /*******************************/
    /******read/write support********/
    /*******************************/

    /*
     * Parse the bitmap. 
     */
    if (parse_pbmp(unit, c, &pbm) < 0)
    {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    SOC_PBMP_ASSIGN(pbm_phys, pbm);
    SOC_PBMP_AND(pbm_phys, PBMP_PORT_ALL(unit));
    if (SOC_PBMP_IS_NULL(pbm_phys))
    {
        cli_out("Ports specified do not have PHY drivers.\n");
    }
    else
    {
        SOC_PBMP_ASSIGN(pbm_temp, pbm);
        SOC_PBMP_REMOVE(pbm_temp, PBMP_PORT_ALL(unit));
        if (SOC_PBMP_NOT_NULL(pbm_temp))
        {
            cli_out("Not all ports given have PHY drivers.  Using %s\n", SOC_PBMP_FMT(pbm_phys, pfmt));
        }
    }
    SOC_PBMP_ASSIGN(pbm, pbm_phys);

#if defined(PORTMOD_SUPPORT)
    if (soc_feature(unit, soc_feature_portmod))
    {
        c = ARG_GET(a);
        if (sal_strcasecmp(c, "direct") == 0)
        {
            return dnxc_phy_direct_access(unit, a, intermediate, &pbm);
        }
        ARG_PREV(a);
        return dnxc_phy_portmod_sym_access(unit, a, intermediate, &pbm);
    }
#endif /* defined(PORTMOD_SUPPORT) */

    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_power(
    int unit,
    args_t * args)
{

    parse_table_t pt;
    char *c = NULL;
    char *modes[] = { "off", "on", NULL };
    int mode_value = -1;
    bcm_port_t dport, p;
    bcm_pbmp_t pbm;

    if ((c = ARG_GET(args)) == NULL)
    {
        cli_out("        power: set power mode for the PHY devices implemented power control.\n"
                "        Example: phy power 1 mode=on\n");
        return CMD_FAIL;
    }

    if ((parse_bcm_pbmp(unit, c, &pbm) < 0))
    {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(args), c);
        return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "mode", PQ_MULTI | PQ_DFL, (void *) (0), &mode_value, &modes);

    if (parse_arg_eq(args, &pt) < 0)
    {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (ARG_CNT(args) > 0)
    {
        cli_out("%s: ERROR: Unknown argument %s\n", ARG_CMD(args), ARG_CUR(args));
        return CMD_FAIL;
    }

    if (mode_value == -1)
    {
        cli_out("Missing mode parameter \n");
        return CMD_FAIL;
    }

    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p)
    {
        BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, p, mode_value));
    }
    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_prbs(
    int unit,
    args_t * args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    bcm_pbmp_t pbmp;
    int rv, cmd, enable, mode = 0, lb_mode = BCM_PORT_LOOPBACK_PHY, interval = 1;
    char *cmd_str, *port_str, *mode_str;
    char *prbs_polys[] =
        { "X7_X6_1", "X15_X14_1", "X23_X18_1", "X31_X28_1", "X9_X5_1", "X11_X9_1", "X58_X31_1", "X49_X40_1", "X20_X3_1",
        "X13_X12_X2_1", "X10_X7_1", "PAM4_13Q",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"
    };

    int poly = DIAG_DNXC_PRBS_POLY_DEFAULT, lb = 0, invert = 0;
    int nof_prbs_polys;

    enum
    { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };
    enum
    { _PHY_PRBS_SI_MODE, _PHY_PRBS_HC_MODE };

    nof_prbs_polys = (sizeof(prbs_polys) / sizeof(prbs_polys[0])) / 2;

    if ((port_str = ARG_GET(args)) == NULL)
    {
        cli_out("        prbs: set, get, or clear internal PRBS generator/checker.\n"
                "        Example: phy prbs 6 set mode=phy loopback=true\n"
                "                 phy prbs 6 get interval=5\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0)
    {
        cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL)
    {
        cli_out("ERROR: please insert a valid command: set|get|clear\n");
        return CMD_FAIL;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0)
    {
        cmd = _PHY_PRBS_SET_CMD;
        enable = 1;
    }
    else if (sal_strcasecmp(cmd_str, "get") == 0)
    {
        cmd = _PHY_PRBS_GET_CMD;
        enable = 0;
    }
    else if (sal_strcasecmp(cmd_str, "clear") == 0)
    {
        cmd = _PHY_PRBS_CLEAR_CMD;
        enable = 0;
    }
    else
    {
        cli_out("ERROR: please insert a valid command: set|get|clear\n");
        return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mode", PQ_STRING, 0, &mode_str, NULL);
    if (cmd == _PHY_PRBS_SET_CMD)
    {
        parse_table_add(&pt, "Polynomial", PQ_DFL | PQ_MULTI, (void *) DIAG_DNXC_PRBS_POLY_DEFAULT, &poly, &prbs_polys);
        parse_table_add(&pt, "LoopBack", PQ_DFL | PQ_BOOL, (void *) (0), &lb, NULL);
        parse_table_add(&pt, "Invert", PQ_DFL | PQ_BOOL, (void *) (0), &invert, NULL);
    }
    else if (cmd == _PHY_PRBS_GET_CMD)
    {
        parse_table_add(&pt, "interval", PQ_DFL | PQ_INT, (void *) (0), &interval, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0)
    {
        cli_out("ERROR: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (mode_str)
    {
        if ((sal_strcasecmp(mode_str, "si") == 0) || (sal_strcasecmp(mode_str, "mac") == 0))
        {
            mode = DIAG_DNXC_PRBS_INJECTION_MAC;
            lb_mode = BCM_PORT_LOOPBACK_MAC;
        }
        else if ((sal_strcasecmp(mode_str, "hc") == 0) || (sal_strcasecmp(mode_str, "phy") == 0))
        {
            mode = DIAG_DNXC_PRBS_INJECTION_PHY;
            lb_mode = BCM_PORT_LOOPBACK_PHY;
        }
    }

    if (poly == DIAG_DNXC_PRBS_POLY_DEFAULT)
    {
        poly = BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1;
    }
    /*
     * Now free allocated strings 
     */
    parse_arg_eq_done(&pt);

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
    {

        int status;
        /*
         * First set prbs mode 
         */
        if (IS_SFI_PORT(unit, port) && (enable == 1))
        {
            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsMode, mode);
            if (rv != BCM_E_NONE)
            {
                cli_out("ERROR: Setting prbs mode failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        if (cmd == _PHY_PRBS_SET_CMD || cmd == _PHY_PRBS_CLEAR_CMD)
        {
            poly = poly % nof_prbs_polys;

            /*
             * Set polynomial 
             */
            if (mode == DIAG_DNXC_PRBS_INJECTION_MAC)
            {
                /*
                 * MAC PRBS polynomial set in jer2_arad and DNXF is not supported
                 */
                if (poly != BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1)
                {
                    cli_out
                        ("ERROR: Setting prbs polynomial %d is not allowed when prbs injection mode is MAC. Only supported value for MAC PRBS is %d \n",
                         poly, BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1);
                    return CMD_FAIL;
                }
            }
            else
            {
                rv = bcm_port_control_set(unit, port, bcmPortControlPrbsPolynomial, poly);
                if (rv != BCM_E_NONE)
                {
                    cli_out("ERROR: Setting prbs polynomial failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }

            /*
             * lb == 1 ==>internal prbs
             */
            /*
             * lb == 0 ==>external prbs
             */
            if (lb)
            {
                rv = bcm_port_loopback_set(unit, port, lb_mode);

                if (rv != BCM_E_NONE)
                {
                    cli_out("ERROR: loopback set failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }

            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsRxEnable, enable);
            if (rv != BCM_E_NONE)
            {
                cli_out("ERROR: Setting prbs rx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            /*
             * invert
             */
            if (!mode /* not mac prbs */ )
            {
                rv = bcm_port_control_set(unit, port, bcmPortControlPrbsTxInvertData, invert);
                if (rv != BCM_E_NONE)
                {
                    cli_out("ERROR: Invert prbs tx failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsTxEnable, enable);
            if (rv != BCM_E_NONE)
            {
                cli_out("ERROR: Setting prbs tx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            if (cmd == _PHY_PRBS_SET_CMD)
            {
                /*
                 * clear errors 
                 */
                rv = bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status);
                if (rv != BCM_E_NONE)
                {
                    cli_out("ERROR: Getting prbs rx status failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }

    }
    if (cmd == _PHY_PRBS_GET_CMD)
    {
        sal_sleep(interval);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
        {

            int status;
            /*
             * check prbs results
             */
            rv = bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status);
            if (rv != BCM_E_NONE)
            {
                cli_out("ERROR: Getting prbs rx status failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            switch (status)
            {
                case 0:
                    cli_out("%s (%2d):  PRBS PASSED(LOCKED and returns no error)!\n", BCM_PORT_NAME(unit, port), port);
                    break;
                case -1:
                    cli_out("%s (%2d):  PRBS FAILED (not LOCKED)!\n", BCM_PORT_NAME(unit, port), port);
                    break;
                case -2:
                    cli_out("%s (%2d):  PRBS FAILED(currently LOCKED but was not LOCKED since last read)!\n",
                            BCM_PORT_NAME(unit, port), port);
                    break;
                default:
                    cli_out("%s (%2d):  PRBS FAILED with %d errors!\n", BCM_PORT_NAME(unit, port), port, status);
                    break;
            }

        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_diag_phy_unit_get(
    int unit_value,
    int *phy_dev)
{

    *phy_dev = PHY_DIAG_DEV_DFLT;
    if (unit_value == 0)
    {   /* internal PHY */
        *phy_dev = PHY_DIAG_DEV_INT;
    }
    else if ((unit_value > 0) && (unit_value < 4))
    {
        *phy_dev = PHY_DIAG_DEV_EXT;
    }
    else if (unit_value != -1)
    {
        cli_out("unit is numeric value: 0,1,2, ...\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_diag_phy_if_get(
    char *if_str,
    int *dev_if)
{

    *dev_if = PHY_DIAG_INTF_DFLT;
    if (if_str)
    {
        if (sal_strcasecmp(if_str, "sys") == 0)
        {
            *dev_if = PHY_DIAG_INTF_SYS;
        }
        else if (sal_strcasecmp(if_str, "line") == 0)
        {
            *dev_if = PHY_DIAG_INTF_LINE;
        }
        else if (if_str[0] != 0)
        {
            cli_out("InterFace must be sys or line.\n");
            return CMD_FAIL;
        }
    }
    else
    {
        cli_out("Invalid Interface string\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_diag_portmod_dsc(
    int unit,
    bcm_pbmp_t pbmp,
    args_t * args)
{
#ifdef PORTMOD_SUPPORT
    bcm_port_t port, dport;
    int rv = CMD_OK;
    char *cmd_str = NULL;
    parse_table_t pt;
    char *if_str;
    int phy_unit, phy_unit_value = 0, phy_unit_if;
    uint32 inst;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *) (0), &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (parse_arg_eq(args, &pt) < 0)
    {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    rv = dnxc_phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == CMD_OK)
    {
        rv = dnxc_phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /*
     * Now free allocated strings 
     */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK)
    {
        return rv;
    }
    /*
     * construct inst 
     */
    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    if (((cmd_str = ARG_GET(args)) == NULL) ||
        (sal_strcasecmp(cmd_str, "config") == 0) || (sal_strcasecmp(cmd_str, "state") == 0))
    {
        /*
         * nothing need to do, phymod will handle this string
         */
    }
    else if (sal_strcasecmp(cmd_str, "external") == 0)
    {
        /*
         * default phyn param is external- do nothing
         */
        cmd_str = NULL;
    }
    else
    {
        cli_out("ERROR: Invalid format, %s", cmd_str);
        return CMD_FAIL;
    }

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
    {
        rv = portmod_port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_DSC, cmd_str);

        if (rv < 0)
        {
            return rv;
        }
    }
    return rv;
#else
    return CMD_FAIL;
#endif
}

STATIC cmd_result_t
dnxc_phy_diag_prbs(
    int unit,
    bcm_pbmp_t pbmp,
    args_t * args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv, cmd, enable;
    char *cmd_str, *if_str /* , *poly_str=NULL */ ;
    int poly = 0, invert = 0;
    int phy_unit, phy_unit_value = -1, phy_unit_if;
    uint32 inst;
    char *prbs_polys[] =
        { "X7_X6_1", "X15_X14_1", "X23_X18_1", "X31_X28_1", "X9_X5_1", "X11_X9_1", "X58_X31_1", "X49_X40_1", "X20_X3_1",
        "X13_X12_X2_1", "X10_X7_1", "PAM4_13Q",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"
    };
    int nof_prbs_polys;

    enum
    { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };
    enum
    { _PHY_PRBS_SI_MODE, _PHY_PRBS_HC_MODE };

    nof_prbs_polys = (sizeof(prbs_polys) / sizeof(prbs_polys[0])) / 2;

    if ((cmd_str = ARG_GET(args)) == NULL)
    {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0)
    {
        cmd = _PHY_PRBS_SET_CMD;
        enable = 1;
    }
    else if (sal_strcasecmp(cmd_str, "get") == 0)
    {
        cmd = _PHY_PRBS_GET_CMD;
        enable = 0;
    }
    else if (sal_strcasecmp(cmd_str, "clear") == 0)
    {
        cmd = _PHY_PRBS_CLEAR_CMD;
        enable = 0;
    }
    else
        return CMD_USAGE;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *) (0), &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (cmd == _PHY_PRBS_SET_CMD)
    {
        parse_table_add(&pt, "Polynomial", PQ_DFL | PQ_MULTI, (void *) (0), &poly, &prbs_polys);
        parse_table_add(&pt, "Invert", PQ_DFL | PQ_INT, (void *) (0), &invert, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0)
    {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    rv = (int) dnxc_phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == ((int) CMD_OK))
    {
        rv = (int) dnxc_phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }
    /*
     * Now free allocated strings 
     */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK)
    {
        return rv;
    }
    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
    {
        if (cmd == _PHY_PRBS_SET_CMD || cmd == _PHY_PRBS_CLEAR_CMD)
        {
            poly = poly % nof_prbs_polys;
            /*
             * Set polynomial 
             */
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET, SOC_PHY_CONTROL_PRBS_POLYNOMIAL, INT_TO_PTR(poly));
            if (rv != ((int) BCM_E_NONE))
            {
                cli_out("Setting prbs polynomial failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            /*
             * Set invert 
             */
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA, INT_TO_PTR(invert));
            if (rv != ((int) BCM_E_NONE))
            {
                cli_out("Setting prbs invertion failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                SOC_PHY_CONTROL_PRBS_TX_ENABLE, INT_TO_PTR(enable));
            if (rv != ((int) BCM_E_NONE))
            {
                cli_out("Setting prbs tx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                SOC_PHY_CONTROL_PRBS_RX_ENABLE, INT_TO_PTR(enable));
            if (rv != ((int) BCM_E_NONE))
            {
                cli_out("Setting prbs rx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        else
        {       /* _PHY_PRBS_GET_CMD */
            int status;

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_GET, SOC_PHY_CONTROL_PRBS_RX_STATUS, (void *) &status);
            if (rv != ((int) BCM_E_NONE))
            {
                cli_out("Getting prbs rx status failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            switch (status)
            {
                case 0:
                    cli_out("%s (%2d):  PRBS OK!\n", BCM_PORT_NAME(unit, port), port);
                    break;
                case -1:
                    cli_out("%s (%2d):  PRBS Failed!\n", BCM_PORT_NAME(unit, port), port);
                    break;
                default:
                    cli_out("%s (%2d):  PRBS has %d errors!\n", BCM_PORT_NAME(unit, port), port, status);
                    break;
            }
        }
    }
    return CMD_OK;
}

/* 
 * Diagnostic utilities for serdes and PHY devices.
 *
 * Command format used in BCM diag shell:
 * phy diag <pbm> <sub_cmd> [sub cmd parameters]
 * All sub commands take two general parameters: unit and if. This identifies
 * the instance the command targets to.
 * unit = 0,1, ....  
 *   unit takes numeric values identifying the instance of the PHY devices 
 *   associated with the given port. A value 0 indicates the internal 
 *   PHY(serdes) the one directly connected to the MAC. A value 1 indicates
 *   the first external PHY.
 * if(interface) = [sys | line] 
 *   interface identifies the system side interface or line side interface of
 *   PHY device.
 * The list of sub commands:
 *   dsc - display tx/rx equalization information. Warpcore(WC) only.
 *   loopback - put the device in the given loopback mode 
 *              parameter: mode=[local | none]
 *      
 */
STATIC cmd_result_t
dnxc_phy_diag(
    int unit,
    args_t * args)
{
    bcm_pbmp_t pbmp;
    int rv;
    int lscan_time;
    char *cmd_str, *port_str;

    rv = CMD_OK;
    if ((port_str = ARG_GET(args)) == NULL)
    {
        cli_out("        diag: Subcommands:\n"
                "              eyescan [vertical_max=<value>] [vertical_min=<value>]\n"
                "                 [horizontal_max=<value>] [horizontal_min=<value>]\n"
                "                 [sample_resolution=<value>] [sample_time=<value>]\n"
                "                 [counter=<value>] [flags=<value>] [error_threshold=<value>]\n"
                "                 [time_upper_bound=<value>] [nof_threshold_links=<value>] [lane=<value>]\n"
                "              loopback mode=<none|mac|phy>\n"
                "              dsc\n" "        Example: phy diag 6 eyescan sample_time=15\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0)
    {
        cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL)
    {
        cli_out("ERROR: Invalid format. Please include sub option: eyescan|LoopBack\n");
        return CMD_FAIL;
    }

    /*
     * linkscan should be disabled. soc_phyctrl_diag_ctrl() doesn't assume exclusive access to the device. 
     */

    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit, &lscan_time));
    if (lscan_time != 0)
    {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
        /*
         * Give enough time for linkscan task to exit. 
         */
        sal_usleep(lscan_time * 2);
    }

    if (sal_strcasecmp(cmd_str, "dsc") == 0)
    {
        rv = dnxc_phy_diag_portmod_dsc(unit, pbmp, args);
    }
    else if (sal_strcasecmp(cmd_str, "pcs") == 0)
    {
        rv = _phy_diag_pcs(unit, pbmp, args);
    }
    else if (sal_strcasecmp(cmd_str, "eyescan") == 0)
    {
        rv = _phy_diag_eyescan(unit, pbmp, args);
    }
    else if (sal_strcasecmp(cmd_str, "LoopBack") == 0)
    {
        rv = dnxc_phy_diag_loopback(unit, pbmp, args);
    }
    else if (sal_strcasecmp(cmd_str, "prbs") == 0)
    {
        rv = dnxc_phy_diag_prbs(unit, pbmp, args);
    }
    else
    {
        cli_out("valid sub option: eyescan, LoopBack, dsc, prbs\n");
        rv = CMD_FAIL;
    }

    if (lscan_time != 0)
    {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, lscan_time));
    }

    return rv;
}

STATIC cmd_result_t
dnxc_phy_diag_loopback(
    int unit,
    bcm_pbmp_t pbmp,
    args_t * args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    bcm_error_t rv = BCM_E_NONE;
    char *modes[] = { "none", "mac", "phy", "rmt", NULL };
    int mode = -1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "mode", PQ_MULTI | PQ_DFL, 0, &mode, &modes);

    if (parse_arg_eq(args, &pt) < 0)
    {
        cli_out("ERROR: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    if (mode == -1)
    {
        cli_out("Missing mode parameter \n");
        return CMD_FAIL;
    }

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
    {
        rv = bcm_port_loopback_set(unit, port, mode);
        if (rv != BCM_E_NONE)
        {
            cli_out("Setting loopback failed: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

#ifdef BCM_JER2_JERICHO_SUPPORT
STATIC cmd_result_t
jer2_jer_phy_measure_pll_calc(
    soc_dnxc_init_serdes_ref_clock_t ref_clk,
    int n_div,
    int p_div,
    int m0_div,
    int *ch0_out_int,
    int *ch0_out_remainder)
{
    int ref_clk_speed, ref_clk_speed_remainder;

    DNXC_LEGACY_FIXME_ASSERT;

    if (p_div == 0 || m0_div == 0)
    {
        return CMD_FAIL;
    }

    switch (ref_clk)
    {
        case soc_dnxc_init_serdes_ref_clock_125:
            ref_clk_speed = 125;
            ref_clk_speed_remainder = 0;
            break;
        case soc_dnxc_init_serdes_ref_clock_156_25:
            ref_clk_speed = 156;
            ref_clk_speed_remainder = 25;
            break;
        default:
            return CMD_FAIL;
    }

    *ch0_out_int = (ref_clk_speed / p_div * n_div) / m0_div;
    *ch0_out_remainder = (ref_clk_speed_remainder / p_div * n_div) / m0_div;

    return CMD_OK;
}

cmd_result_t
jer2_jer_phy_measure_nif_pll(
    int unit,
    bcm_port_t port,
    int *ch0_out_int,
    int *ch0_out_remainder)
{
    int rv;
    soc_dnxc_init_serdes_ref_clock_t ref_clk;
    int p_div, n_div, m0_div;

    DNXC_LEGACY_FIXME_ASSERT;

    rv = soc_jer2_jer_phy_nif_pll_div_get(unit, port, &ref_clk, &p_div, &n_div, &m0_div);
    if (rv != SOC_E_NONE)
    {
        return CMD_FAIL;
    }

    return jer2_jer_phy_measure_pll_calc(ref_clk, n_div, p_div, m0_div, ch0_out_int, ch0_out_remainder);
}

cmd_result_t
jer2_jer_phy_measure_fabric_pll(
    int unit,
    bcm_port_t port,
    int *ch0_out_int,
    int *ch0_out_remainder)
{
    int rv;
    bcm_port_t lane;
    soc_reg_above_64_val_t reg_val;
    soc_dnxc_init_serdes_ref_clock_t ref_clk;
    int p_div, n_div, m0_div;

    DNXC_LEGACY_FIXME_ASSERT;

    lane = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);

    if (lane / SOC_DNXC_NOF_LINKS_IN_MAC < 6)
    {
        rv = READ_ECI_FAB_0_PLL_CONFIGr(unit, reg_val);
        if (rv != BCM_E_NONE)
        {
            return CMD_FAIL;
        }
        n_div = soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val, FAB_0_PLL_CFG_NDIVf);
        p_div = soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val, FAB_0_PLL_CFG_PDIVf);
        m0_div = soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val, FAB_0_PLL_CFG_CH_0_MDIVf);
        ref_clk = SOC_DNX_CONFIG(unit)->jer2_jer->pll.ref_clk_fabric_in[0];
    }
    else
    {
        rv = READ_ECI_FAB_1_PLL_CONFIGr(unit, reg_val);
        if (rv != BCM_E_NONE)
        {
            return CMD_FAIL;
        }
        n_div = soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val, FAB_1_PLL_CFG_NDIVf);
        p_div = soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val, FAB_1_PLL_CFG_PDIVf);
        m0_div = soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val, FAB_1_PLL_CFG_CH_0_MDIVf);
        ref_clk = SOC_DNX_CONFIG(unit)->jer2_jer->pll.ref_clk_fabric_in[1];
    }

    return jer2_jer_phy_measure_pll_calc(ref_clk, n_div, p_div, m0_div, ch0_out_int, ch0_out_remainder);
}

#endif /* BCM_JER2_JERICHO_SUPPORT */

int
diag_dnxc_phy_measure_port(
    int unit,
    bcm_port_t port,
    int is_rx,
    uint32 *rate_int,
    uint32 *rate_remainder)
{
    int fabric_mac_bus_size = 0, counter_disabled = 0, clock_speed_int = 0, clock_speed_remainder = 0, rv = BCM_E_NONE;
    uint64 period, val, reg_val64, rate_start, rate_end;
    uint32 rate;
    bcm_port_t lane;
    bcm_stat_val_t type;
    int ilkn_o_fabric_port = 0;
#ifdef BCM_DNX_SUPPORT
    int offset;
    int first_phy_port;
#endif

    type = is_rx ? snmpBcmRxAsynFifoRate : snmpBcmTxAsynFifoRate;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_ARAD(unit))
    {
        
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {

        clock_speed_int = ((1000000 / dnxf_data_port.general.fmac_clock_khz_get(unit)));
        clock_speed_remainder = ((1000000 / (dnxf_data_port.general.fmac_clock_khz_get(unit) / 1000)) % 1000);
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (!IS_SFI_PORT(unit, port))
    {
        rv = dnx_algo_port_interface_offset_get(unit, port, &offset);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }
        
    }
    else
#endif
    {
        ilkn_o_fabric_port = 0;
    }

    if (SOC_IS_DNXF(unit) || (SOC_IS_ARAD(unit) && (IS_SFI_PORT(unit, port) || ilkn_o_fabric_port)))
    {
        lane = port;

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {

            
            /*
             * coverity[dead_error_line::FALSE] 
             */
            if (ilkn_o_fabric_port)
            {
                rv = dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy_port);
                if (rv != BCM_E_NONE)
                {
                    return CMD_FAIL;
                }
                lane = first_phy_port - SOC_DNX_FIRST_FABRIC_PHY_PORT(unit);
            }
            else
            {
                lane = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);
            }
        }
#endif
        COMPILER_64_SET(period, 0, 10000);
        COMPILER_64_SET(val, 0, 0);
        COMPILER_64_SET(reg_val64, 0, 0);
        rv = READ_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane / SOC_DNXC_NOF_LINKS_IN_MAC, &reg_val64);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }
        soc_reg64_field_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_PERIODf, period);
        soc_reg64_field_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_STARTf, val);
        rv = WRITE_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane / SOC_DNXC_NOF_LINKS_IN_MAC, reg_val64);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }

        /*
         * clear stats
         */
        rv = bcm_stat_sync(unit);
        /*
         * if counter thread is disabled, no need to sync
         */
        if (rv == BCM_E_DISABLED)
        {
            counter_disabled = 1;
        }
        else if (rv != BCM_E_NONE)
        {
            return CMD_FAIL;
        }

        rv = bcm_stat_get(unit, port, type, &rate_start);
        if (rv != BCM_E_NONE)
        {
            return CMD_FAIL;
        }

        /*
         * start gtimer stats
         */
        COMPILER_64_SET(val, 0, 1);
        rv = READ_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane / SOC_DNXC_NOF_LINKS_IN_MAC, &reg_val64);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }
        soc_reg64_field_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_STARTf, val);
        rv = WRITE_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane / SOC_DNXC_NOF_LINKS_IN_MAC, reg_val64);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }

        /*
         * getting rate
         */
        rv = bcm_stat_sync(unit);
        /*
         * if counter thread is disabled, no need to sync
         */
        if (rv == BCM_E_DISABLED)
        {
            counter_disabled = 1;
        }
        else if (rv != BCM_E_NONE)
        {
            return CMD_FAIL;
        }
        rv = bcm_stat_get(unit, port, type, &rate_end);
        if (rv != BCM_E_NONE)
        {
            return CMD_FAIL;
        }

        /*
         * FifoRate counter collected in DNXF, not collected in JER2_ARAD if DNXF, subtract collected rate_end from
         * collected rate_start if JER2_ARAD, rate_start clears counter, and therefore rate_end is correct rate 
         */
        if ((SOC_IS_DNXF(unit) || SOC_IS_JERICHO(unit)) && !counter_disabled)
        {
            COMPILER_64_SUB_64(rate_end, rate_start);
        }
        rate = COMPILER_64_LO(rate_end);

#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            bcm_port_resource_t resource;
            rv = bcm_port_resource_get(unit, port, &resource);
            if (rv != SOC_E_NONE)
            {
                return CMD_FAIL;
            }

            fabric_mac_bus_size = dnxf_data_port.general.fmac_bus_size_get(unit, resource.fec_type)->size;
        }
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            fabric_mac_bus_size = SOC_DNX_DEFS_GET(unit, fabric_mac_bus_size);
            if (ilkn_o_fabric_port)
            {
                /*
                 * asyn fifo is in units of 40 (instead of 60) for ilkn over fabric
                 */
                fabric_mac_bus_size = 40;
            }
        }
#endif

        /*
         * coverity - check division by zero 
         */
        if (((COMPILER_64_LO(period) / 1000) * (clock_speed_int * 1000 + (clock_speed_remainder % 1000))) == 0)
        {
            return CMD_FAIL;
        }

        *rate_int =
            (rate * fabric_mac_bus_size * 1000) / (COMPILER_64_LO(period) *
                                                   (clock_speed_int * 1000 + (clock_speed_remainder % 1000)));
        *rate_remainder =
            ((rate * fabric_mac_bus_size * 1000) /
             ((COMPILER_64_LO(period) / 1000) * (clock_speed_int * 1000 + (clock_speed_remainder % 1000)))) % 1000;

        /*
         * clear fmac_fmal_statistics_gtimer
         */

        rv = READ_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane / SOC_DNXC_NOF_LINKS_IN_MAC, &reg_val64);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }
        soc_reg64_field32_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_STARTf, 0);
        rv = WRITE_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane / SOC_DNXC_NOF_LINKS_IN_MAC, reg_val64);
        if (rv != SOC_E_NONE)
        {
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_control(
    int unit,
    args_t * args)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    uint32 wan_mode, preemphasis, predriver_current, driver_current, dfe, lp_dfe, br_dfe, linkTraining;
    uint32 sw_rx_los_nval = 0;
    uint32 eq_boost;
    uint32 interface;
    uint32 interfacemax;
    uint32 flags;
    int print_header;
    cmd_result_t cmd_rv;
    bcm_port_config_t pcfg;
    int eq_tune = FALSE;
    int eq_status = FALSE;
    int dump = FALSE;
    int farEndEqValue = 0;
#ifdef INCLUDE_MACSEC
    uint32 macsec_switch_fixed, macsec_switch_fixed_speed;
    uint32 macsec_switch_fixed_duplex, macsec_switch_fixed_pause;
    uint32 macsec_pause_rx_fwd, macsec_pause_tx_fwd;
    uint32 macsec_line_ipg, macsec_switch_ipg;

    macsec_switch_fixed = 0;
    macsec_switch_fixed_speed = 0;
    macsec_switch_fixed_duplex = 0;
    macsec_switch_fixed_pause = 0;
    macsec_pause_rx_fwd = 0;
    macsec_pause_tx_fwd = 0;
    macsec_line_ipg = 0;
    macsec_switch_ipg = 0;
#endif

    DNXC_LEGACY_FIXME_ASSERT;

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE)
    {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(args));
        return CMD_FAIL;
    }

    wan_mode = 0;
    preemphasis = 0;
    predriver_current = 0;
    driver_current = 0;
    interface = 0;
    interfacemax = 0;
    eq_boost = 0;

    dfe = 0;
    lp_dfe = 0;
    br_dfe = 0;
    linkTraining = 0;

    if ((c = ARG_GET(args)) == NULL)
    {
        SOC_PBMP_ASSIGN(pbm, pcfg.port);
    }
    else if (parse_bcm_pbmp(unit, c, &pbm) < 0)
    {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(args), c);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    flags = 0;
    if ((c = ARG_CUR(args)) != NULL)
    {
        parse_table_t pt;
        int i;

        if (c[0] == '=')
        {
            return CMD_USAGE;   /* '=' unsupported */
        }
        if (sal_strcasecmp(c, "RxTune") == 0)
        {
            if (ARG_CNT(args) <= 1)
            {   /* Current counter is at RxTune */
                ARG_NEXT(args);
                farEndEqValue = 0;
                cli_out("far end equalization value not input, using 0\n");
            }
            else
            {
                /*
                 * go to next argument 
                 */
                ARG_NEXT(args);
                /*
                 * Get far end equalization value 
                 */
                if ((c = ARG_GET(args)) != NULL)
                {
                    farEndEqValue = sal_ctoi(c, 0);
                    cli_out("far end equalization value input (%d)\n", farEndEqValue);
                }
            }
            eq_tune = TRUE;
        }
        if (sal_strcasecmp(c, "Dump") == 0)
        {
            c = ARG_GET(args);
            dump = TRUE;
        }

        if ((eq_tune == FALSE) && (dump == FALSE))
        {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "WanMode", PQ_DFL | PQ_BOOL, 0, &wan_mode, 0);
            parse_table_add(&pt, "Preemphasis", PQ_DFL | PQ_INT, 0, &preemphasis, 0);
            parse_table_add(&pt, "DriverCurrent", PQ_DFL | PQ_INT, 0, &driver_current, 0);
            parse_table_add(&pt, "PreDriverCurrent", PQ_DFL | PQ_INT, 0, &predriver_current, 0);
            parse_table_add(&pt, "EqualizerBoost", PQ_DFL | PQ_INT, 0, &eq_boost, 0);
            parse_table_add(&pt, "Interface", PQ_DFL | PQ_INT, 0, &interface, 0);
            parse_table_add(&pt, "InterfaceMax", PQ_DFL | PQ_INT, 0, &interfacemax, 0);
            parse_table_add(&pt, "SwRxLos", PQ_DFL | PQ_INT, 0, &sw_rx_los_nval, 0);
            parse_table_add(&pt, "Dfe", PQ_DFL | PQ_INT, 0, &dfe, 0);
            parse_table_add(&pt, "LpDfe", PQ_DFL | PQ_INT, 0, &lp_dfe, 0);
            parse_table_add(&pt, "BrDfe", PQ_DFL | PQ_INT, 0, &br_dfe, 0);
            parse_table_add(&pt, "LT", PQ_DFL | PQ_INT, 0, &linkTraining, 0);

#ifdef INCLUDE_MACSEC
            parse_table_add(&pt, "MacsecSwitchFixed", PQ_DFL | PQ_BOOL, 0, &macsec_switch_fixed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedSpeed", PQ_DFL | PQ_INT, 0, &macsec_switch_fixed_speed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedDuplex", PQ_DFL | PQ_BOOL, 0, &macsec_switch_fixed_duplex, 0);
            parse_table_add(&pt, "MacsecSwitchFixedPause", PQ_DFL | PQ_BOOL, 0, &macsec_switch_fixed_pause, 0);
            parse_table_add(&pt, "MacsecPauseRXForward", PQ_DFL | PQ_BOOL, 0, &macsec_pause_rx_fwd, 0);
            parse_table_add(&pt, "MacsecPauseTXForward", PQ_DFL | PQ_BOOL, 0, &macsec_pause_tx_fwd, 0);
            parse_table_add(&pt, "MacsecLineIPG", PQ_DFL | PQ_INT, 0, &macsec_line_ipg, 0);
            parse_table_add(&pt, "MacsecSwitchIPG", PQ_DFL | PQ_INT, 0, &macsec_switch_ipg, 0);
#endif

            if (parse_arg_eq(args, &pt) < 0)
            {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (ARG_CNT(args) > 0)
            {
                cli_out("%s: Unknown argument %s\n", ARG_CMD(args), ARG_CUR(args));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            for (i = 0; i < pt.pt_cnt; i++)
            {
                if (pt.pt_entries[i].pq_type & PQ_PARSED)
                {
                    flags |= (1 << i);
                }
            }
            parse_arg_eq_done(&pt);
        }
    }
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p)
    {
        print_header = TRUE;

        if (eq_tune == TRUE)
        {

            cmd_rv = bcm_port_control_set(unit, p, bcmPortControlSerdesDriverEqualizationFarEnd, farEndEqValue);

            cmd_rv = bcm_port_control_set(unit, p, bcmPortControlSerdesDriverTune, 1);
            if (cmd_rv != CMD_OK)
            {
                cli_out("unit %d port %d Tuning function not available\n", unit, p);
                continue;
            }
            cli_out("Rx Equalization Tuning start\n");
            sal_usleep(1000000);
            cmd_rv = bcm_port_control_get(unit, p, bcmPortControlSerdesDriverEqualizationTuneStatusFarEnd, &eq_status);

            cli_out("unit %d port %d Tuning done, Status: %s\n",
                    unit, p, ((cmd_rv == CMD_OK) && eq_status) ? "OK" : "FAIL");
            continue;
        }
        if (dump == TRUE)
        {
            /*
             * coverity[returned_value] 
             */
            cmd_rv = bcm_port_phy_control_set(unit, p, BCM_PORT_PHY_CONTROL_DUMP, 1);
            continue;
        }

        /*
         * Read and set Driver Current 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_DRIVER_CURRENT, driver_current, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set DFE 
         */
        cmd_rv = port_phy_control_update(unit, p, BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE, dfe, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set LP_DFE 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE, lp_dfe, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set BR_DFE 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE, br_dfe, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set forced cl72/93 
         */
        cmd_rv = port_phy_control_update(unit, p, BCM_PORT_PHY_CONTROL_CL72, linkTraining, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set the interface 
         */
        cmd_rv = port_phy_control_update(unit, p, BCM_PORT_PHY_CONTROL_INTERFACE, interface, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }
#ifdef INCLUDE_MACSEC

        /*
         * Read and set the Switch fixed 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED,
                                         macsec_switch_fixed, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set the Switch fixed Speed 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED,
                                         macsec_switch_fixed_speed, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set the Switch fixed Duplex 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX,
                                         macsec_switch_fixed_duplex, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set the Switch fixed Pause 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE,
                                         macsec_switch_fixed_pause, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set Pause Receive Forward 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD,
                                         macsec_pause_rx_fwd, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set Pause transmit Forward 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD,
                                         macsec_pause_tx_fwd, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set Line Side IPG 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_LINE_IPG, macsec_line_ipg, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }

        /*
         * Read and set Switch Side IPG 
         */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_IPG,
                                         macsec_switch_ipg, flags, &print_header);
        if (cmd_rv != CMD_OK)
        {
            return cmd_rv;
        }
#endif
    }
    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_measure(
    int unit,
    args_t * args)
{

    char *port_str, *cmd_str;
    bcm_pbmp_t pbmp;
    int is_rx = 1;
    bcm_port_t port, dport, lane;
    uint32 rate_int, rate_remainder;
    bcm_error_t rv = BCM_E_NONE;
    cmd_result_t result;
    int ilkn_o_fabric_port = 0;
#ifdef BCM_DNX_SUPPORT
#ifdef BCM_JER2_JERICHO_SUPPORT
    int ch0_out_int = 0, ch0_out_remainder = 0;
#endif /* BCM_JER2_JERICHO_SUPPORT */
    int offset;
#endif /* BCM_DNX_SUPPORT */

    if ((port_str = ARG_GET(args)) == NULL)
    {
        cli_out("        Measured rate of chosen port\n");
        cli_out("        measure <ports> [<tx|rx>]\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0)
    {
        cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) != NULL)
    {
        if (sal_strcasecmp(cmd_str, "tx") == 0)
        {
            is_rx = 0;
        }
        else if (sal_strcasecmp(cmd_str, "rx") == 0)
        {
            is_rx = 1;
        }
        else
        {
            cli_out("ERROR: valid values are tx or rx\n");
            return CMD_FAIL;
        }
    }

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
    {

#ifdef BCM_DNX_SUPPORT
        if (!IS_SFI_PORT(unit, port))
        {
            result = dnx_algo_port_interface_offset_get(unit, port, &offset);
            if (result != CMD_OK)
            {
                return CMD_FAIL;
            }
            
        }
        else
#endif
        {
            ilkn_o_fabric_port = 0;
        }

        if (SOC_IS_DNXF(unit) || (SOC_IS_ARAD(unit) && (IS_SFI_PORT(unit, port) || ilkn_o_fabric_port)))
        {
            lane = port;

#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit))
            {
                lane = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);
            }
#endif
            result = diag_dnxc_phy_measure_port(unit, port, is_rx, &rate_int, &rate_remainder);
            if (result != CMD_OK)
            {
                cli_out("diag_dnxc_phy_measure_port failed %d\n", rv);
                return result;
            }
            cli_out("Measured rate for port %1d (QUAD%1d,L-%1d) is %d.%d\n", port, lane / SOC_DNXC_NOF_LINKS_IN_MAC,
                    lane % SOC_DNXC_NOF_LINKS_IN_MAC, rate_int, rate_remainder);

#ifdef BCM_DNX_SUPPORT
#ifdef BCM_JER2_JERICHO_SUPPORT
            if (SOC_IS_JERICHO(unit))
            {
                result = jer2_jer_phy_measure_fabric_pll(unit, port, &ch0_out_int, &ch0_out_remainder);
                if (CMD_OK != result)
                    return result;

                cli_out("PLL is %d.%d\n", ch0_out_int, ch0_out_remainder);
            }
#endif /* BCM_JER2_JERICHO_SUPPORT */
#endif /* BCM_DNX_SUPPORT */
        }
#ifdef BCM_DNX_SUPPORT
        else if (SOC_IS_ARAD(unit))
        {
            

        }

#endif /* BCM_DNX_SUPPORT */
    }

    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_cl72(
    int unit,
    args_t * args)
{

    char *port_str, *cmd_str;
    bcm_pbmp_t pbmp;
    soc_port_t port, dport;
    int rv;
    uint32 cl72_configured, cl72_status;

    if ((port_str = ARG_GET(args)) == NULL)
    {
        cli_out("        Enable|Disable link training (either cl72 or cl93)\n");
        cli_out("        cl72 <ports> <on|off>\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0)
    {
        cli_out("Error: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    cmd_str = ARG_GET(args);

    if (NULL == cmd_str)
    {
        /*
         * print cl72 status
         */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
        {
            rv = bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_CL72, &cl72_configured);
            if (BCM_FAILURE(rv))
            {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_CL72_STATUS, &cl72_status);
            if (BCM_FAILURE(rv))
            {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            cli_out("CL72 configured %d\n", cl72_configured);
            cli_out("CL72 status %d\n", cl72_status);
        }
    }
    else if (sal_strcasecmp(cmd_str, "off") == 0)
    {
        /*
         * turn off cl72
         */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
        {
            rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_CL72, 0);
            if (BCM_FAILURE(rv))
            {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
    }
    else if (sal_strcasecmp(cmd_str, "on") == 0)
    {
        /*
         * turn on cl72
         */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port)
        {
            rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_CL72, 1);
            if (BCM_FAILURE(rv))
            {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
    }
    else
    {
        cli_out("ERROR: valid values are on or off\n");
        return CMD_FAIL;
    }

    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_info(
    int unit,
    args_t * args)
{
#ifdef PORTMOD_SUPPORT

    soc_port_t p, dport;
    soc_pbmp_t pbm;
    phymod_core_access_t internal_core[MAX_PORT_CORES];
    phymod_core_access_t core_acc[MAX_PORT_CORES];
    phymod_core_info_t core_info;
    int nof_cores = 0, an_timeout = -1;
    int phy = 0;
    int range_start = 0;
    int is_first_range;
    int core_num = 0;
#ifdef BCM_JER2_JERICHO_SUPPORT
    uint32 first_phy_port = 0, phy_lane = 0;
#endif
    portmod_port_diag_info_t diag_info;
    int rv, i;
    uint8 pcount = 0;
    char lnstr[32], *pname, namelen;

    SOC_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
    SOC_PBMP_REMOVE(pbm, PBMP_HG_SUBPORT_ALL(unit));
    SOC_PBMP_REMOVE(pbm, PBMP_REQ_ALL(unit));

    cli_out("Phy mapping dump:\n");
    cli_out("%11s %5s %5s %5s %5s %23s %10s\n", "port", "id0", "id1", "addr", "iaddr", "name", "timeout");

    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p)
    {

        for (i = 0; i < MAX_PORT_CORES; i++)
        {
            rv = phymod_core_access_t_init(&core_acc[i]);
            if (BCM_FAILURE(rv))
            {
                cli_out("Failed to init core access \n");
                return CMD_FAIL;
            }
            rv = phymod_core_access_t_init(&internal_core[i]);
            if (BCM_FAILURE(rv))
            {
                cli_out("Failed to init internal core access \n");
                return CMD_FAIL;
            }
        }

        rv = phymod_core_info_t_init(&core_info);
        if (BCM_FAILURE(rv))
        {
            cli_out("Failed to init core info\n");
            return CMD_FAIL;
        }

        sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));

        rv = portmod_port_core_access_get(unit, p, -1 /* most external phy */ , MAX_PORT_CORES, core_acc, &nof_cores,
                                          NULL);
        if (BCM_FAILURE(rv))
        {
            cli_out("Failed to get core access\n");
            return CMD_FAIL;
        }
        if (nof_cores == 0)
        {
            continue;
        }

        rv = portmod_port_core_access_get(unit, p, 0, MAX_PORT_CORES, internal_core, &nof_cores, NULL);
        if (BCM_FAILURE(rv))
        {
            cli_out("Failed to get internal core access\n");
            return CMD_FAIL;
        }
        if (nof_cores == 0)
        {
            continue;
        }

        rv = portmod_port_diag_info_get(unit, p, &diag_info);
        if (BCM_FAILURE(rv))
        {
            cli_out("Failed to get port info\n");
            return CMD_FAIL;
        }

#ifdef BCM_JER2_JERICHO_SUPPORT
        if (!(IS_SFI_PORT(unit, p) || IS_IL_PORT(unit, p)))
        {
            rv = dnx_algo_port_nif_first_phy_get(unit, p, 0, &first_phy_port);
            if (BCM_FAILURE(rv))
            {
                cli_out("Failed to get first phy port info\n");
            }
            rv = MBCM_DNX_SOC_DRIVER_CALL(unit, mbcm_dnx_qsgmii_offsets_remove, (unit, first_phy_port, &phy_lane));
            if (BCM_FAILURE(rv))
            {
                cli_out("Failed to get  phy lane info\n");
            }
            core_num = (phy_lane - 1) / NUM_OF_LANES_IN_PM;
        }
#endif

        is_first_range = TRUE;
        PORTMOD_PBMP_ITER(diag_info.phys, phy)
        {
            if (is_first_range)
            {
                /*
                 * For SFI ports, the phy id is zero-based
                 */
                range_start = IS_SFI_PORT(unit, p) ? phy : phy - 1;
                is_first_range = FALSE;
            }
        }
        an_timeout = soc_property_port_get(unit, p, spn_PHY_AUTONEG_TIMEOUT, 250000);

        rv = phymod_core_info_get(core_acc, &core_info);
        if (BCM_FAILURE(rv))
        {
            cli_out("Failed to get core info\n");
            return CMD_FAIL;
        }

        PORTMOD_PBMP_COUNT(diag_info.phys, pcount);

        pname = phymod_core_version_t_mapping[core_info.core_version].key;
        namelen = strlen(pname);

        sal_snprintf(lnstr, sizeof(lnstr), "%s", pname);
        sal_snprintf(lnstr + namelen - 2, sizeof(lnstr) - (namelen - 2), "-%s/%02d/", pname + namelen - 2, core_num);

        pname = lnstr;
        while (*pname != '-')
        {
            *pname = sal_toupper(*pname);
            pname++;
        }

        pname = lnstr + strlen(lnstr);
        if (pcount == 4)
            sal_snprintf(pname, sizeof(lnstr), "%d", pcount);
        else if (pcount == 2)
            sal_snprintf(pname, sizeof(lnstr), "%d-%d", range_start % 4, (range_start % 4) + 1);
        else
            sal_snprintf(pname, sizeof(lnstr), "%d", range_start % 4);

        cli_out("%6s(%3d) %5x %5x %5x %5x %23s %10d \n",
                SOC_PORT_NAME(unit, p), p,
                core_info.phy_id0,
                core_info.phy_id1, core_acc[0].access.addr, internal_core[0].access.addr, lnstr, an_timeout);

    }
#endif

    return CMD_OK;
}

STATIC cmd_result_t
dnxc_phy_raw(
    int unit,
    args_t * args)
{

    uint16 phy_data;
    uint32 phy_reg;
    uint16 phy_devad = 0;
    uint16 phy_addr;
    int caluse = 22;
    char *cmd_str;
    int ret = CMD_OK;

    if ((cmd_str = ARG_GET(args)) == NULL)
    {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "c45") == 0)
    {
        caluse = 45;
        if ((cmd_str = ARG_GET(args)) == NULL)
        {
            return CMD_USAGE;
        }
    }
    phy_addr = strtoul(cmd_str, NULL, 0);
    if ((cmd_str = ARG_GET(args)) == NULL)
    {   /* Get register number */
        cli_out("%s: ERROR: Invalid register number\n", ARG_CMD(args));
        return CMD_FAIL;
    }

    /*
     * when caluse is 45, get phy_devad
     */
    if (45 == caluse)
    {
        phy_devad = strtoul(cmd_str, NULL, 0);
        if ((cmd_str = ARG_GET(args)) == NULL)
        {
            return CMD_USAGE;
        }
    }
    else
    {
        cli_out("ERROR: Clause 22 not supported\n");
        return CMD_FAIL;
    }

    phy_reg = strtoul(cmd_str, NULL, 0);

    if ((cmd_str = ARG_GET(args)) == NULL)
    {   /* Read register */
        ret = soc_dcmn_miim_read(unit, caluse, phy_addr, SOC_PHY_CLAUSE45_ADDR(phy_devad, phy_reg), &phy_data);

        if (ret < 0)
        {
            cli_out("ERROR: MII Addr %d: soc_miim_read failed: %s\n", phy_addr, soc_errmsg(ret));
            return CMD_FAIL;
        }
        cli_out("%s\t0x%02x: 0x%04x\n", "", phy_reg, phy_data);
    }
    else
    {   /* write */
        phy_data = strtoul(cmd_str, NULL, 0);
        ret = soc_dcmn_miim_write(unit, caluse, phy_addr, SOC_PHY_CLAUSE45_ADDR(phy_devad, phy_reg), phy_data);

        if (ret < 0)
        {
            cli_out("ERROR: MII Addr %d: soc_miim_write failed: %s\n", phy_addr, soc_errmsg(ret));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

char if_dnx_port_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: port <option> [args...]\n"
#else
    "Parameters: <pbmp>\n\t"
    "[LinkScan=on|off|hw|sw] [SPeed=10|100|1000] [FullDuplex=true|false]\n\t"
    "[SpanningTreeProtocol=Disable|Block|LIsten|LEarn|Forward]\n\t"
    "[VlanFilter=<value>] [Enable=true|false] [InterFace=<interface>]\n\t"
    "[LoopBack=NONE|MAC|PHY]] [AutoNeg=on|off]\n\t"
    "[TxPAUse=on|off] [RxPAUse=on|off]\n\t"
    "If only <ports> is specified, characteristics for that port are\n\t"
    "displayed. <ports> is a standard port bitmap (see \"help pbmp\").\n\t"
    "If AutoNeg is on, SPeed and DUPlex are inoperative.\n\t"
    "If AutoNeg is off, SPeed and DUPlex are the FORCED values.\n\t"
    "SPeed of zero indicates maximum speed.\n\t"
    "LinkScan enables automatic scanning for link changes with updating\n\t"
    "of MAC registers, and EPC_LINK (or equivalent)\n\t"
    "PAUse enables send/receive of pause frames in full duplex mode.\n\t"
    "VlanFilter drops input packets that not tagged with a valid VLAN\n\t" "that contains the port.\n"
#endif
    ;

/* Iterate thru a port bitmap with the given mask; display info */
STATIC int
_dnx_port_disp_iter(
    int unit,
    pbmp_t pbm,
    pbmp_t pbm_mask,
    uint32 seen)
{
    bcm_port_info_t info;
    soc_port_t port, dport;
    int r;

    BCM_PBMP_AND(pbm, pbm_mask);
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port)
    {

        sal_memset(&info, 0, sizeof(bcm_port_info_t));
        port_info_init(unit, port, &info, seen);

        if ((r = bcm_port_selective_get(unit, port, &info)) < 0)
        {
            cli_out("Error: Could not get port %s information: %s\n", BCM_PORT_NAME(unit, port), bcm_errmsg(r));
            return (CMD_FAIL);
        }

        disp_port_info(unit, BCM_PORT_NAME(unit, port), port, &info, IS_ST_PORT(unit, port), seen);
    }

    return CMD_OK;
}

/*
 * Function:
 *  if_port
 * Purpose:
 *  Configure port specific parameters.
 * Parameters:
 *  u   - SOC unit #
 *  a   - pointer to args
 * Returns:
 *  CMD_OK/CMD_FAIL
 */

cmd_result_t
if_dnx_port(
    int unit,
    args_t * a)
{
    pbmp_t pbm;
    bcm_port_config_t pcfg;
    bcm_port_info_t *info_all;
    bcm_port_info_t info_given;
    bcm_port_ability_t *ability_all;    /* Abilities for all ports */
    bcm_port_ability_t ability_port;    /* Ability for current port */
    bcm_port_ability_t ability_given;
    char *c;
    int r, rv = 0, cmd_rv = CMD_OK;
    soc_port_t p, dport;
    parse_table_t pt;
    uint32 seen = 0;
    uint32 parsed = 0, parsed_adj;
    char pfmt[SOC_PBMP_FMT_LEN];
#ifdef BCM_DNX_SUPPORT
    dnx_algo_port_type_e port_type;
#endif /* BCM_DNX_SUPPORT */

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE)
    {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL)
    {
        return (CMD_USAGE);
    }

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t), "if_port");

    if (info_all == NULL)
    {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    ability_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t), "if_port");

    if (ability_all == NULL)
    {
        cli_out("Insufficient memory.\n");
        sal_free(info_all);
        return CMD_FAIL;
    }

    sal_memset(&info_given, 0, sizeof(bcm_port_info_t));
    sal_memset(info_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t));
    sal_memset(&ability_given, 0, sizeof(bcm_port_ability_t));
    sal_memset(&ability_port, 0, sizeof(bcm_port_ability_t));
    sal_memset(ability_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t));

    if (parse_bcm_pbmp(unit, c, &pbm) < 0)
    {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm))
    {
        ARG_DISCARD(a);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_OK;
    }

    if (ARG_CNT(a) == 0)
    {

        seen = BCM_PORT_ATTR_ALL_MASK;

        if (SOC_IS_ARAD(unit))
        {
#ifdef BCM_DNX_SUPPORT
            /*
             * SDK-118677
             * seen = _BCM_DNX_PORT_ATTRS;
             */
#endif
        }
        else if (SOC_IS_DNXF(unit))
        {
#ifdef BCM_DNXF_SUPPORT
            seen = _BCM_DNXF_PORT_ATTRS;
#endif
        }

    }
    else
    {
        /*
         * Otherwise, arguments are given.  Use them to determine which
         * properties need to be gotten/set.
         *
         * Probe and detach, hidden commands.
         */
        if (!sal_strcasecmp(_ARG_CUR(a), "detach"))
        {
            pbmp_t detached;
            bcm_port_detach(unit, pbm, &detached);
            cli_out("Detached port bitmap %s\n", SOC_PBMP_FMT(detached, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        }
        else if ((!sal_strcasecmp(_ARG_CUR(a), "probe")) || (!sal_strcasecmp(_ARG_CUR(a), "attach")))
        {
            pbmp_t probed;
            bcm_port_probe(unit, pbm, &probed);
            cli_out("Probed port bitmap %s\n", SOC_PBMP_FMT(probed, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        }

        if (!sal_strcmp(_ARG_CUR(a), "="))
        {
            /*
             * For "=" where the user is prompted to enter all the parameters,
             * use the parameters from the first selected port as the defaults.
             */
            if (ARG_CNT(a) != 1)
            {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, p)
            {
                break;  /* Find first port in bitmap */
            }

            port_info_init(unit, p, &info_given, 0);
            if ((rv = bcm_port_info_get(unit, p, &info_given)) < 0)
            {
                cli_out("%s: Error: Failed to get port info\n", ARG_CMD(a));
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_FAIL;
            }
        }
        else
        {
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, p)
            {
                break;  /* Find first port in bitmap */
            }
            port_info_init(unit, p, &info_given, 0);
        }

        /*
         * Parse the arguments.  Determine which ones were actually given.
         */
        port_parse_setup(unit, &pt, &info_given);

        if (parse_arg_eq(a, &pt) < 0)
        {
            parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }

        /*
         * Translate port_info into port abilities. 
         */

        if (ARG_CNT(a) > 0)
        {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }

        /*
         * Find out what parameters specified.  Record values specified.
         */
        port_parse_mask_get(&pt, &seen, &parsed);

        parse_arg_eq_done(&pt);
    }

    if (seen && parsed)
    {
        cli_out("%s: Cannot get and set " "port properties in one command\n", ARG_CMD(a));
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    }
    else if (seen)
    {   /* Show selected information */
        cli_out("%s: Status (* indicates PHY link up)\n", ARG_CMD(a));
        /*
         * Display the information by port type 
         */

        _call_pdi(unit, pbm, pcfg.fe, seen);
        _call_pdi(unit, pbm, pcfg.ge, seen);
        _call_pdi(unit, pbm, pcfg.xe, seen);
        _call_pdi(unit, pbm, pcfg.ce, seen);
        _call_pdi(unit, pbm, pcfg.hg, seen);
        _call_pdi(unit, pbm, pcfg.sfi, seen);
        sal_free(info_all);
        sal_free(ability_all);
        return (CMD_OK);
    }

    /*
     * Some set information was given 
     */

    if (parsed & BCM_PORT_ATTR_LINKSCAN_MASK)
    {
        /*
         * Map ON --> S/W, OFF--> None 
         */
        if (info_given.linkscan > 2)
        {
            info_given.linkscan -= 3;
        }
    }

    parsed_adj = parsed;
    if (parsed & (BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK))
    {
        parsed_adj |= BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK;
    }

    /*
     * Retrieve all requested port information first, then later write
     * back all port information.  That prevents a problem with loopback
     * cables where setting one port's info throws another into autoneg
     * causing it to return info in flux (e.g. suddenly go half duplex).
     */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p)
    {
        port_info_init(unit, p, &info_all[p], parsed_adj);
        if ((r = bcm_port_selective_get(unit, p, &info_all[p])) < 0)
        {
            cli_out("%s: Error: Could not get port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }
    }

    /*
     * Loop through all the specified ports, changing whatever field
     * values were actually given.  This avoids copying unaffected
     * information from one port to another and prevents having to
     * re-parse the arguments once per port.
     */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p)
    {
        if ((rv = bcm_port_speed_max(unit, p, &info_given.speed_max)) < 0)
        {
            cli_out("port parse: Error: Could not get port %s max speed: %s\n", BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
            continue;
        }
        /*
         * bcm_port_ability_local_get() is not implemented for QSGMII ports. 
         */
        if (!SOC_IS_DNXF(unit) && !IS_SFI_PORT(unit, p) && !IS_QSGMII_PORT(unit, p))
        {
            if ((rv = bcm_port_ability_local_get(unit, p, &info_given.port_ability)) < 0)
            {
                cli_out("port parse: Error: Could not get port %s ability: %s\n",
                        BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                continue;
            }
            if ((rv = soc_port_ability_to_mode(&info_given.port_ability, &info_given.ability)) < 0)
            {
                cli_out("port parse: Error: Could not transform port %s ability to mode: %s\n",
                        BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                continue;
            }
        }
        if ((r = port_parse_port_info_set(parsed, &info_given, &info_all[p])) < 0)
        {
            cli_out("%s: Error: Could not parse port %s info: %s\n", ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }

        /*
         * If AN is on, do not set speed, duplex, pause 
         */
        if (info_all[p].autoneg)
        {
            info_all[p].action_mask &= ~BCM_PORT_AN_ATTRS;
        }

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_ARAD(unit) && !BCM_PBMP_MEMBER(pcfg.sfi, p))
        {

            rv = dnx_algo_port_type_get(unit, p, &port_type);
            if (BCM_FAILURE(rv))
            {
                cli_out("Error: Could not get port type\n");
                cmd_rv = CMD_FAIL;
                continue;
            }

            if (port_type == DNX_ALGO_PORT_TYPE_STIF)
            {
                info_all[p].action_mask &= ~(BCM_PORT_ATTR_LINKSCAN_MASK | BCM_PORT_ATTR_LINKSTAT_MASK);
            }
        }
#endif

        if ((r = bcm_port_selective_set(unit, p, &info_all[p])) < 0)
        {
            cli_out("%s: Error: Could not set port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }
    }

    sal_free(info_all);
    sal_free(ability_all);

    return (cmd_rv);
}
