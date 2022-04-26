/*
 * ctest_dnx_rx_trap_prog.c
 *
 *  Created on: Dec 6, 2018
 *      Author: sm888907
 */
/*
  * Include files.
  * {
  */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include "ctest_dnx_rx_trap_prog.h"
#include "ctest_dnx_rx_trap.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX
/*
 * }
 */

/** Ingress trap test details */
sh_sand_man_t sh_dnx_rx_trap_prog_man = {
    "Programmable traps configuration testing",
    "Test all API and fields of programmable trap configuration."
};

/**
 * \brief
 *   Options list for 'prog' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_prog_options[] = {
    /**         Name                               Type                 Description                 Default */
    {CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test (pos, neg)", "pos"}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for Programmable traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_prog_tests[] = {
    {"pos", "type=pos", CTEST_POSTCOMMIT}
    ,
    {"neg", "type=neg", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

void
ctest_dnx_rx_trap_prog_fill(
    bcm_rx_trap_prog_config_t * prog_config_p)
{
    uint8 byte_num;

    prog_config_p->src_mac_enable = (sal_rand() % 3);
    if (prog_config_p->src_mac_enable != bcmRxTrapProgDisable)
    {
        for (byte_num = 0; byte_num < 6; byte_num++)
        {
            prog_config_p->src_mac[byte_num] = (sal_rand() % 0x100);
        }

        prog_config_p->src_mac_nof_bits = (sal_rand() % 49);
    }

    prog_config_p->dest_mac_enable = (sal_rand() % 3);
    if (prog_config_p->dest_mac_enable != bcmRxTrapProgDisable)
    {
        for (byte_num = 0; byte_num < 6; byte_num++)
        {
            prog_config_p->dest_mac[byte_num] = (sal_rand() % 0x100);
        }

        prog_config_p->dest_mac_nof_bits = (sal_rand() % 49);
    }

    prog_config_p->ether_type_enable = (sal_rand() % 3);
    if (prog_config_p->ether_type_enable != bcmRxTrapProgDisable)
    {
        prog_config_p->ether_type = (sal_rand() % 0x10000);
    }

    prog_config_p->sub_type_enable = (sal_rand() % 3);
    if (prog_config_p->sub_type_enable != bcmRxTrapProgDisable)
    {
        prog_config_p->sub_type = (sal_rand() % 0x100);
        prog_config_p->sub_type_mask = (sal_rand() % 0x100);
    }

    prog_config_p->ip_protocol_enable = (sal_rand() % 3);
    if (prog_config_p->ip_protocol_enable != bcmRxTrapProgDisable)
    {
        prog_config_p->ip_protocol = (sal_rand() % 0x100);
    }

    prog_config_p->l4_ports_enable = (sal_rand() % 3);
    if (prog_config_p->l4_ports_enable != bcmRxTrapProgDisable)
    {
        prog_config_p->src_port = (sal_rand() % 0x10000);
        prog_config_p->src_port_mask = (sal_rand() % 0x10000);
        prog_config_p->dest_port = (sal_rand() % 0x10000);
        prog_config_p->dest_port_mask = (sal_rand() % 0x10000);
    }

    prog_config_p->tcp_flags_enable = (sal_rand() % 3);
    if (prog_config_p->tcp_flags_enable != bcmRxTrapProgDisable)
    {
        prog_config_p->tcp_flags = (sal_rand() % 0x200);
        prog_config_p->tcp_flags_mask = (sal_rand() % 0x200);
    }

    prog_config_p->tcp_seq_is_zero_enable = (sal_rand() % 3);

}

/**
* \brief
*   Compares two configuration of programmable trap configuration.
* \param [in] unit - unit ID
* \param [in] prog_index - prog profile index
* \param [in] prog_config_set_p - set prog trap configuration
* \param [in] prog_config_get_p - get prog trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e
ctest_dnx_rx_trap_prog_compare(
    int unit,
    int prog_index,
    bcm_rx_trap_prog_config_t * prog_config_set_p,
    bcm_rx_trap_prog_config_t * prog_config_get_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Source MAC */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->src_mac_enable, &prog_config_get_p->src_mac_enable,
                     sizeof(prog_config_set_p->src_mac_enable), "src_mac_enable"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->src_mac, &prog_config_get_p->src_mac,
                     sizeof(prog_config_set_p->src_mac), "src_mac"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->src_mac_nof_bits, &prog_config_get_p->src_mac_nof_bits,
                     sizeof(prog_config_set_p->src_mac_nof_bits), "src_mac_nof_bits"));

    /** Dest MAC */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->dest_mac_enable, &prog_config_get_p->dest_mac_enable,
                     sizeof(prog_config_set_p->dest_mac_enable), "dest_mac_enable"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->dest_mac, &prog_config_get_p->dest_mac,
                     sizeof(prog_config_set_p->dest_mac), "dest_mac"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->dest_mac_nof_bits, &prog_config_get_p->dest_mac_nof_bits,
                     sizeof(prog_config_set_p->dest_mac_nof_bits), "dest_mac_nof_bits"));

    /** Ether Type */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->ether_type_enable, &prog_config_get_p->ether_type_enable,
                     sizeof(prog_config_set_p->ether_type_enable), "ether_type_enable"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->ether_type, &prog_config_get_p->ether_type,
                     sizeof(prog_config_set_p->ether_type), "ether_type"));

    /** Sub type - First byte after link layer header */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->sub_type_enable, &prog_config_get_p->sub_type_enable,
                     sizeof(prog_config_set_p->sub_type_enable), "sub_type_enable"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->sub_type, &prog_config_get_p->sub_type,
                     sizeof(prog_config_set_p->sub_type), "sub_type"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->sub_type_mask, &prog_config_get_p->sub_type_mask,
                     sizeof(prog_config_set_p->sub_type_mask), "sub_type_mask"));

    /** IP Protocol */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->ip_protocol_enable, &prog_config_get_p->ip_protocol_enable,
                     sizeof(prog_config_set_p->ip_protocol_enable), "ip_protocol_enable"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->ip_protocol, &prog_config_get_p->ip_protocol,
                     sizeof(prog_config_set_p->ip_protocol), "ip_protocol"));

    /** L4 Ports enable */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->src_port, &prog_config_get_p->src_port,
                     sizeof(prog_config_set_p->src_port), "src_port"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->src_port_mask, &prog_config_get_p->src_port_mask,
                     sizeof(prog_config_set_p->src_port_mask), "src_port_mask"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->dest_port, &prog_config_get_p->dest_port,
                     sizeof(prog_config_set_p->dest_port), "dest_port"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->dest_port_mask, &prog_config_get_p->dest_port_mask,
                     sizeof(prog_config_set_p->dest_port_mask), "dest_port_mask"));

    /** TCP Flags */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->tcp_flags_enable, &prog_config_get_p->tcp_flags_enable,
                     sizeof(prog_config_set_p->tcp_flags_enable), "tcp_flags_enable"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->tcp_flags, &prog_config_get_p->tcp_flags,
                     sizeof(prog_config_set_p->tcp_flags), "tcp_flags"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->tcp_flags_mask, &prog_config_get_p->tcp_flags_mask,
                     sizeof(prog_config_set_p->tcp_flags_mask), "tcp_flags_mask"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->tcp_seq_is_zero_enable, &prog_config_get_p->tcp_seq_is_zero_enable,
                     sizeof(prog_config_set_p->tcp_seq_is_zero_enable), "tcp_seq_is_zero_enable"));

    /** Trap GPORT */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &prog_config_set_p->trap_gport, &prog_config_get_p->trap_gport,
                     sizeof(prog_config_set_p->trap_gport), "trap_gport"));

    LOG_INFO_EX(BSL_LOG_MODULE, "Programmable trap compare was done successfuly \n %s%s%s%s",
                EMPTY, EMPTY, EMPTY, EMPTY);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of Programmable traps.
 *   Sets a Programmable trap
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_prog_pos(
    int unit)
{
    bcm_rx_trap_prog_config_t prog_config_set, prog_config_get;
    uint8 prog_index, nof_prog_traps = dnx_data_trap.ingress.nof_programmable_traps_get(unit);
    uint8 repeat_index;
    SHR_FUNC_INIT_VARS(unit);

    for (repeat_index = 0; repeat_index < 10; repeat_index++)
    {
        for (prog_index = 0; prog_index < nof_prog_traps; prog_index++)
        {
            /** Before set */
            bcm_rx_trap_prog_config_t_init(&prog_config_set);

            SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, prog_index, &prog_config_get));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_prog_compare(unit, 0, &prog_config_set, &prog_config_get));

            /** Set and compare */
            bcm_rx_trap_prog_config_t_init(&prog_config_set);
            ctest_dnx_rx_trap_prog_fill(&prog_config_set);
            SHR_IF_ERR_EXIT(bcm_rx_trap_prog_set(unit, 0, prog_index, &prog_config_set));

            SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, prog_index, &prog_config_get));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_prog_compare(unit, 0, &prog_config_set, &prog_config_get));

            /** Do get with an unclear struct */
            ctest_dnx_rx_trap_prog_fill(&prog_config_get);
            SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, prog_index, &prog_config_get));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_prog_compare(unit, 0, &prog_config_set, &prog_config_get));
        }

        for (prog_index = 0; prog_index < nof_prog_traps; prog_index++)
        {
            /** Clear and compare */
            bcm_rx_trap_prog_config_t_init(&prog_config_set);
            SHR_IF_ERR_EXIT(bcm_rx_trap_prog_set(unit, 0, prog_index, &prog_config_set));

            SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, prog_index, &prog_config_get));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_prog_compare(unit, 0, &prog_config_set, &prog_config_get));
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "PROGRAMMABLE TRAP CONFIGURATION TEST END.\n")));
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Negative testing for programmable traps APIs
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_prog_neg(
    int unit)
{
    int trap_id;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_prog_config_t prog_config;
    bcm_error_t rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------- bcm_rx_trap_set NEGATIVE TEST START ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "       Supplying Invalid Prog Index to set and get APIs     \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    rv = bcm_rx_trap_prog_set(unit, 0, 8, &prog_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    rv = bcm_rx_trap_prog_get(unit, 8, &prog_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           Supplying Invalid Enabler to Source MAC          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.src_mac_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "         Supplying Invalid Num of Bits to Source MAC        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.src_mac_nof_bits = 49;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "            Supplying Invalid Enabler to Dest MAC           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.dest_mac_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Supplying Invalid Num of Bits to Dest MAC         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.dest_mac_nof_bits = 49;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "            Supplying Invalid Enabler to Ether type         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.ether_type_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "            Supplying Invalid Enabler to Sub type           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.sub_type_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           Supplying Invalid Enabler to IP Protocol         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.ip_protocol_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying Invalid Enabler to L4 Ports          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.l4_ports_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           Supplying Invalid Enabler to TCP Flags           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.tcp_flags_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "            Supplying Invalid Value to TCP Flags            \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.tcp_flags = 0x200;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying Invalid Mask to TCP Flags            \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.tcp_flags_mask = 0x200;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying Invalid Enabler to TCP Sequence number is Zero  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    prog_config.tcp_seq_is_zero_enable = 3;

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           Supplying Trap id which is not allocated         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    BCM_GPORT_TRAP_SET(prog_config.trap_gport, 0, 15, 0);

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "        Supplying Snoop profile which is not allocated      \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_prog_config_t_init(&prog_config);
    BCM_GPORT_TRAP_SET(prog_config.trap_gport, 4, 0, 7);

    rv = bcm_rx_trap_prog_set(unit, 0, 0, &prog_config);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------- bcm_rx_trap_set NEGATIVE TEST END -----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Semantic testing of Programmable traps.
 *
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_prog(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_TYPE, test_type);

    if (sal_strncasecmp
        (test_type, CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_POSITIVE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_prog_pos(unit));
    }
    else if (sal_strncasecmp
             (test_type, CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_NEGATIVE,
              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_prog_neg(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test type indicated is unsupported\n Supported types (type=): %s, %s\n ",
                     CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_POSITIVE, CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_NEGATIVE);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_prog_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_prog(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
