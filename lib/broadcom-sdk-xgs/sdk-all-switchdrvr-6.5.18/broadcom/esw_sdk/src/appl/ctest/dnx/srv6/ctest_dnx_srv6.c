/** \file ctest_dnx_srv6.c
 *
 * Test of Next_Protocol APIs (add, get, delete)
 */

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/bcm_dlist.h>
#include <include/bcm/qos.h>
#include <include/bcm/srv6.h>
#include "ctest_dnx_srv6.h"
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

/** SW state access of the SRv6 */
#include <soc/dnx/swstate/auto_generated/access/srv6_access.h>

/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/** defines that represent next_protocols above SRv6 per IANA RFC*/
#define IANA_IPV4_NEXT_PROTOCOL        (4)
#define IANA_IPV6_NEXT_PROTOCOL        (41)

/*************
 *  MACROS  *
 *************/

/*************
 * TYPEDEFS  *
 *************/

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - print sid initiator struct
 *
 *   \param [in] unit
 *   \param [in] info - next_protocol structure
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
static shr_error_e
ctest_dnx_srv6_print_sid_initiator_struct(
    int unit,
    bcm_srv6_sid_initiator_info_t * info,
    prt_control_t * prt_ctr)
{

    SHR_FUNC_INIT_VARS(unit);

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "tunnel_id [gport]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "flags");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "sid");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "encap_access");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "next_encap_id");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", info->tunnel_id);
    PRT_CELL_SET("%d", info->flags);
    PRT_CELL_SET("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x"
                 "0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                 info->sid[0], info->sid[1], info->sid[2], info->sid[3],
                 info->sid[4], info->sid[5], info->sid[6], info->sid[7],
                 info->sid[8], info->sid[9], info->sid[10], info->sid[11],
                 info->sid[12], info->sid[13], info->sid[14], info->sid[15]);
    PRT_CELL_SET("%d", info->encap_access);
    PRT_CELL_SET("%d", info->next_encap_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print sid initiator struct
 *
 *   \param [in] unit
 *   \param [in] info - next_protocol structure
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
static shr_error_e
ctest_dnx_srv6_print_srh_base_initiator_struct(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info,
    prt_control_t * prt_ctr)
{

    SHR_FUNC_INIT_VARS(unit);

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "tunnel_id [gport]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "flags");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "nof_sids");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos_map_id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ttl");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "dscp");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "egress_qos");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "egress_ttl");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ecn_eligible");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "next_encap_id [l3 int]");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", info->tunnel_id);
    PRT_CELL_SET("%d", info->flags);
    PRT_CELL_SET("%d", info->nof_sids);
    PRT_CELL_SET("%d", info->qos_map_id);
    PRT_CELL_SET("%d", info->ttl);
    PRT_CELL_SET("%d", info->dscp);
    PRT_CELL_SET("%d", info->egress_qos_model.egress_qos);
    PRT_CELL_SET("%d", info->egress_qos_model.egress_ttl);
    PRT_CELL_SET("%d", info->egress_qos_model.egress_ecn);
    PRT_CELL_SET("%d", info->next_encap_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print 2 bcm_srv6_terminator_next_protocol_mapping_t structures
 *
 *   \param [in] unit
 *   \param [in] info - next_protocol structure
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
static shr_error_e
ctest_dnx_srv6_print_next_protocol_struct(
    int unit,
    bcm_srv6_terminator_next_protocol_mapping_t * info,
    prt_control_t * prt_ctr)
{

    SHR_FUNC_INIT_VARS(unit);

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "next_protocol");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "flags");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "port [gport]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "encap_id [l3 int]");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", info->next_protocol);
    PRT_CELL_SET("%d", info->flags);
    PRT_CELL_SET("%d", info->port);
    PRT_CELL_SET("%d", info->encap_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - compare 2 bcm_srv6_terminator_next_protocol_mapping_t structures
 *
 *   \param [in] unit
 *   \param [in] expected_struct
 *   \param [in] test_struct
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
static shr_error_e
ctest_dnx_srv6_compare_next_protocol_struct(
    int unit,
    bcm_srv6_terminator_next_protocol_mapping_t * expected_struct,
    bcm_srv6_terminator_next_protocol_mapping_t * test_struct)
{
    SHR_FUNC_INIT_VARS(unit);

    if (expected_struct->next_protocol != test_struct->next_protocol)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match in next_protocol: expected.next_protocol=%d, test.next_protocol=%d",
                     expected_struct->next_protocol, test_struct->next_protocol);
    }
    if (expected_struct->port != test_struct->port)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in port: expected.port=%d, test.port=%d",
                     expected_struct->port, test_struct->port);
    }
    if (expected_struct->encap_id != test_struct->encap_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in encap_id: expected.encap_id=%d, test.encap_id=%d",
                     expected_struct->encap_id, test_struct->encap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - compare 2 bcm_srv6_sid_initiator_info_t structures
 *
 *   \param [in] unit
 *   \param [in] expected_struct
 *   \param [in] test_struct
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
static shr_error_e
ctest_dnx_srv6_compare_sid_initiator_struct(
    int unit,
    bcm_srv6_sid_initiator_info_t * expected_struct,
    bcm_srv6_sid_initiator_info_t * test_struct)
{
    SHR_FUNC_INIT_VARS(unit);

    if (expected_struct->tunnel_id != test_struct->tunnel_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in tunnel_id: expected.tunnel_id=%d, test.tunnel_id=%d",
                     expected_struct->tunnel_id, test_struct->tunnel_id);
    }

    if (expected_struct->encap_access != test_struct->encap_access)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match encap_access next_protocol: expected.encap_access=%d, test.encap_access=%d",
                     expected_struct->encap_access, test_struct->encap_access);
    }

    if (sal_memcmp(expected_struct->sid, test_struct->sid, sizeof(expected_struct->sid)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in sid: expected.sid=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,"
                     "0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x test.sid=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,"
                     "0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                     expected_struct->sid[0], expected_struct->sid[1], expected_struct->sid[2], expected_struct->sid[3],
                     expected_struct->sid[4], expected_struct->sid[5], expected_struct->sid[6], expected_struct->sid[7],
                     expected_struct->sid[8], expected_struct->sid[9], expected_struct->sid[10],
                     expected_struct->sid[11], expected_struct->sid[12], expected_struct->sid[13],
                     expected_struct->sid[14], expected_struct->sid[15], test_struct->sid[0], test_struct->sid[1],
                     test_struct->sid[2], test_struct->sid[3], test_struct->sid[4], test_struct->sid[5],
                     test_struct->sid[6], test_struct->sid[7], test_struct->sid[8], test_struct->sid[9],
                     test_struct->sid[10], test_struct->sid[11], test_struct->sid[12], test_struct->sid[13],
                     test_struct->sid[14], test_struct->sid[15]);
    }
    if (expected_struct->next_encap_id != test_struct->next_encap_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match in next_encap_id: expected.next_encap_id=%d, test.next_encap_id=%d",
                     expected_struct->next_encap_id, test_struct->next_encap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - compare 2 bcm_srv6_srh_base_initiator_info_t structures
 *
 *   \param [in] unit
 *   \param [in] expected_struct
 *   \param [in] test_struct
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
static shr_error_e
ctest_dnx_srv6_compare_srh_base_initiator_struct(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * expected_struct,
    bcm_srv6_srh_base_initiator_info_t * test_struct)
{
    SHR_FUNC_INIT_VARS(unit);

    if (expected_struct->tunnel_id != test_struct->tunnel_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in tunnel_id: expected.tunnel_id=%d, test.tunnel_id=%d",
                     expected_struct->tunnel_id, test_struct->tunnel_id);
    }

    if (expected_struct->nof_sids != test_struct->nof_sids)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in nof_sids: expected.nof_sids=%d, test.nof_sids=%d",
                     expected_struct->nof_sids, test_struct->nof_sids);
    }

    if (expected_struct->qos_map_id != test_struct->qos_map_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in qos_map_id: expected.qos_map_id=%d, test.qos_map_id=%d",
                     expected_struct->qos_map_id, test_struct->qos_map_id);
    }

    if (expected_struct->ttl != test_struct->ttl)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in ttl: expected.ttl=%d, test.ttl=%d",
                     expected_struct->ttl, test_struct->ttl);
    }

    if (expected_struct->dscp != test_struct->dscp)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in dscp: expected.dscp=%d, test.dscp=%d",
                     expected_struct->dscp, test_struct->dscp);
    }

    if (expected_struct->egress_qos_model.egress_ecn != test_struct->egress_qos_model.egress_ecn)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match in ecn_eligible: expected.ecn_eligible=%d, test.ecn_eligible=%d",
                     expected_struct->egress_qos_model.egress_ecn, test_struct->egress_qos_model.egress_ecn);
    }

    if (expected_struct->egress_qos_model.egress_qos != test_struct->egress_qos_model.egress_qos)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in egress_qos: expected.egress_qos=%d, test.egress_qos=%d",
                     expected_struct->egress_qos_model.egress_qos, test_struct->egress_qos_model.egress_qos);
    }

    if (expected_struct->egress_qos_model.egress_ttl != test_struct->egress_qos_model.egress_ttl)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in egress_ttl: expected.egress_ttl=%d, test.egress_ttl=%d",
                     expected_struct->egress_qos_model.egress_ttl, test_struct->egress_qos_model.egress_ttl);
    }

    if (expected_struct->next_encap_id != test_struct->next_encap_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match in next_encap_id: expected.next_encap_id=%d, test.next_encap_id=%d",
                     expected_struct->next_encap_id, test_struct->next_encap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - srv6_next_protocol mapping traverse callbacl function example.
 *          used to count entries
 */
int
next_protocol_counter_cb(
    int unit,
    bcm_srv6_terminator_next_protocol_mapping_t * info,
    void *counter)
{
    int count = *(int *) counter;
    count++;
    *(int *) counter = count;
    return 0;
}

/**
 * \brief - srv6_srh_base_initiator traverse callbacl function example.
 *          used to count entries
 */
int
srh_base_initiator_counter_cb(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info,
    void *counter)
{
    int count = *(int *) counter;
    count++;
    *(int *) counter = count;
    return 0;
}

/**
 * \brief - srv6_sid_initiator traverse callbacl function example.
 *          used to count entries
 */
int
sid_initiator_counter_cb(
    int unit,
    bcm_srv6_sid_initiator_info_t * info,
    void *counter)
{
    int count = *(int *) counter;
    count++;
    *(int *) counter = count;
    return 0;
}

/**
 * \brief - Semantic test of basic SRv6 Next_Protocol to RCH port and encapsulation mapping
 *          Sequence:
 *          -Create x2 next_protocol mappings (IPv4, IPv6) to different RCH ports and encapsulations
 *          -Read back the x2 next_protocols mappings and compare to above
 *          -Modify x2 next_protocol mappings (IPv4, IPv6)
 *          -Read back the x2 next_protocols mappings and compare to above
 *          -Delete the x2 next_protocols mappings
 *          -Read again the x2 next_protocols and expect _SHR_E_NOT_FOUND
 *
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_test_basic_next_protocol_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** valid ports in the system are 200-203 */
    int port_ipv4 = 200;
    int encap_id_ipv4 = 0x100002;
    int port_ipv4_2 = 202;
    int encap_id_ipv4_2 = 0x100004;
    int port_ipv6 = 201;
    int encap_id_ipv6 = 0x100003;
    int port_ipv6_2 = 203;
    int encap_id_ipv6_2 = 0x100005;

    bcm_srv6_terminator_next_protocol_mapping_t next_protocol_info_ipv4, next_protocol_info_ipv6;
    bcm_srv6_terminator_next_protocol_mapping_t replace_next_protocol_info_ipv4, replace_next_protocol_info_ipv6;
    bcm_srv6_terminator_next_protocol_mapping_t get_next_protocol_info_ipv4, get_next_protocol_info_ipv6;

    int rv;
    uint8 is_psp, is_reduced;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_srv6_terminator_next_protocol_mapping_t_init(&next_protocol_info_ipv4);
    bcm_srv6_terminator_next_protocol_mapping_t_init(&next_protocol_info_ipv6);
    bcm_srv6_terminator_next_protocol_mapping_t_init(&replace_next_protocol_info_ipv4);
    bcm_srv6_terminator_next_protocol_mapping_t_init(&replace_next_protocol_info_ipv6);
    bcm_srv6_terminator_next_protocol_mapping_t_init(&get_next_protocol_info_ipv4);
    bcm_srv6_terminator_next_protocol_mapping_t_init(&get_next_protocol_info_ipv6);

    for (is_psp = 0; is_psp < 2; is_psp++)
    {
        /** Configure the SW state to PSP/USP mode */
        SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.set(unit, is_psp));

        for (is_reduced = 0; is_reduced < 2; is_reduced++)
        {
            uint8 is_psp_read = -1;
            uint8 is_reduced_read = -1;

            /** Configure the SW state to Reduced/Normal mode */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.set(unit, is_reduced));

            /** Read back the SW states for validation */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.get(unit, &is_reduced_read));
            SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.get(unit, &is_psp_read));

            cli_out("******************************************************\n");
            cli_out("** Next Protocol CTest is_psp=(%d), is_reduced=(%d) **\n", is_psp_read, is_reduced_read);
            cli_out("******************************************************\n");

            /*
             *  create structure for IPv4 mapping
             */
            next_protocol_info_ipv4.flags = 0;
            next_protocol_info_ipv4.next_protocol = IANA_IPV4_NEXT_PROTOCOL;
            /** convert Port to GPORT */
            BCM_GPORT_SYSTEM_PORT_ID_SET(next_protocol_info_ipv4.port, port_ipv4);
            /** convert from global-LIF to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(next_protocol_info_ipv4.encap_id, encap_id_ipv4);

            /*
             *  create structure for IPv4 replace mapping
             */
            replace_next_protocol_info_ipv4.flags = BCM_SRV6_TERMINATOR_NEXT_PROTOCOL_REPLACE;
            replace_next_protocol_info_ipv4.next_protocol = IANA_IPV4_NEXT_PROTOCOL;
            /** convert Port to GPORT */
            BCM_GPORT_SYSTEM_PORT_ID_SET(replace_next_protocol_info_ipv4.port, port_ipv4_2);
            /** convert from global-LIF to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(replace_next_protocol_info_ipv4.encap_id, encap_id_ipv4_2);

            /*
             *  create structure for IPv6 mapping
             */
            next_protocol_info_ipv6.flags = 0;
            next_protocol_info_ipv6.next_protocol = IANA_IPV6_NEXT_PROTOCOL;
            /** convert Port to GPORT */
            BCM_GPORT_SYSTEM_PORT_ID_SET(next_protocol_info_ipv6.port, port_ipv6);
            /** convert from global-LIF to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(next_protocol_info_ipv6.encap_id, encap_id_ipv6);

            /*
             *  create structure for IPv6 replace mapping
             */
            replace_next_protocol_info_ipv6.flags = BCM_SRV6_TERMINATOR_NEXT_PROTOCOL_REPLACE;
            replace_next_protocol_info_ipv6.next_protocol = IANA_IPV6_NEXT_PROTOCOL;
            /** convert Port to GPORT */
            BCM_GPORT_SYSTEM_PORT_ID_SET(replace_next_protocol_info_ipv6.port, port_ipv6_2);
            /** convert from global-LIF to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(replace_next_protocol_info_ipv6.encap_id, encap_id_ipv6_2);

            /** call the next_protocol for IPv4 API to map to RCH port and encapsulation */
            cli_out("**** adding IPv4 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_add(unit, &next_protocol_info_ipv4));
            /** call the next_protocol for IPv4 API to map to RCH port and encapsulation */
            cli_out("**** replacing IPv4 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_add(unit, &replace_next_protocol_info_ipv4));
            /** call the next_protocol for IPv4 API to map to RCH port and encapsulation */
            cli_out("**** adding IPv6 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_add(unit, &next_protocol_info_ipv6));
            /** call the next_protocol for IPv4 API to map to RCH port and encapsulation */
            cli_out("**** replacing IPv6 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_add(unit, &replace_next_protocol_info_ipv6));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 2 entries per core */
            {
                bcm_srv6_terminator_next_protocol_traverse_info_t additional_info;
                int counter = 0, expected_counter = 2;
                SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_traverse
                                (unit, additional_info, next_protocol_counter_cb, &counter));
                if (counter != expected_counter)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "traverse found %d entries, but only %d is expected\n.",
                                 counter, expected_counter);
                }
            }
            cli_out("**** traverse completed ********\n");

            /** read back the next_protocol mapping and compare the structures for IPv4 */
            get_next_protocol_info_ipv4.flags = 0;
            get_next_protocol_info_ipv4.next_protocol = IANA_IPV4_NEXT_PROTOCOL;
            cli_out("**** getting IPv4 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_get(unit, &get_next_protocol_info_ipv4));

            /** read back the next_protocol mapping and compare the structures for IPv6 */
            get_next_protocol_info_ipv6.flags = 0;
            get_next_protocol_info_ipv6.next_protocol = IANA_IPV6_NEXT_PROTOCOL;
            cli_out("**** getting IPv6 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_get(unit, &get_next_protocol_info_ipv6));

            /*
             * test add/get/delete with IPv4 next_protocol structure
             */
            PRT_TITLE_SET("expected IPv4 next_protocol structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_next_protocol_struct(unit, &replace_next_protocol_info_ipv4, prt_ctr));
            PRT_COMMITX;

            /** print received IPv4 structure*/
            PRT_TITLE_SET("received IPv4 next_protocol structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_next_protocol_struct(unit, &get_next_protocol_info_ipv4, prt_ctr));
            PRT_COMMITX;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_next_protocol_struct
                            (unit, &replace_next_protocol_info_ipv4, &get_next_protocol_info_ipv4));

            /*
             * test add/get/delete with IPv6 next_protocol structure
             */
            PRT_TITLE_SET("expected IPv6 next_protocol structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_next_protocol_struct(unit, &replace_next_protocol_info_ipv6, prt_ctr));
            PRT_COMMITX;

            /** print received IPv6 structure*/
            PRT_TITLE_SET("received IPv6 next_protocol structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_next_protocol_struct(unit, &get_next_protocol_info_ipv6, prt_ctr));
            PRT_COMMITX;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_next_protocol_struct
                            (unit, &replace_next_protocol_info_ipv6, &get_next_protocol_info_ipv6));

            /*
             * check deletion case for IPv4 mapping
             */
            get_next_protocol_info_ipv4.encap_id = 0;
            get_next_protocol_info_ipv4.port = 0;
            get_next_protocol_info_ipv4.flags = 0;
            get_next_protocol_info_ipv4.next_protocol = IANA_IPV4_NEXT_PROTOCOL;
            cli_out("**** deleting IPv4 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_delete(unit, &get_next_protocol_info_ipv4));

            get_next_protocol_info_ipv4.flags = 0;
            get_next_protocol_info_ipv4.next_protocol = IANA_IPV4_NEXT_PROTOCOL;
            cli_out("**** getting IPv4 next_protocol mapping ********\n");
            rv = bcm_srv6_terminator_next_protocol_get(unit, &get_next_protocol_info_ipv4);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv,
                             "API bcm_srv6_terminator_next_protocol_delete() didn't delete well the IPv4 next_protocol mapping !");
            }

            /*
             * check deletion case for IPv6 mapping
             */
            get_next_protocol_info_ipv6.encap_id = 0;
            get_next_protocol_info_ipv6.port = 0;
            get_next_protocol_info_ipv6.flags = 0;
            get_next_protocol_info_ipv6.next_protocol = IANA_IPV6_NEXT_PROTOCOL;
            cli_out("**** deleting IPv6 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_terminator_next_protocol_delete(unit, &get_next_protocol_info_ipv6));

            get_next_protocol_info_ipv6.flags = 0;
            get_next_protocol_info_ipv6.next_protocol = IANA_IPV6_NEXT_PROTOCOL;
            cli_out("**** getting IPv6 next_protocol mapping ********\n");
            rv = bcm_srv6_terminator_next_protocol_get(unit, &get_next_protocol_info_ipv6);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv,
                             "API bcm_srv6_terminator_next_protocol_delete() didn't delete well the IPv6 next_protocol mapping !");
            }

        } /** of is_reduced loop */
    } /** of is_psp loop */

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Semantic test of basic SRv6 SRH Base initiator APIs
 *          Sequence:
 *              -Create a SRH Base entry
 *              -Read back the SRH Base structure and compare to above
 *              -Modify a SRH Base entry
 *              -Read back the SRH Base structure and compare to above
 *              -Delete the SRH Base entry
 *              -Read again the SRH Base entry and expect _SHR_E_NOT_FOUND
 *
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_test_basic_srh_base_initiator_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t replace_srh_base_info;
    bcm_srv6_srh_base_initiator_info_t received_srh_base_info;

    int rv;
    uint8 is_psp, is_reduced;

    int srh_base_global_lif = 0x1230;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    bcm_srv6_srh_base_initiator_info_t_init(&replace_srh_base_info);
    bcm_srv6_srh_base_initiator_info_t_init(&received_srh_base_info);

    for (is_psp = 0; is_psp < 2; is_psp++)
    {
        /** Configure the SW state to PSP/USP mode */
        SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.set(unit, is_psp));

        for (is_reduced = 0; is_reduced < 2; is_reduced++)
        {
            uint8 is_psp_read = -1;
            uint8 is_reduced_read = -1;

            /** Configure the SW state to Reduced/Normal mode */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.set(unit, is_reduced));

            /** Read back the SW states for validation */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.get(unit, &is_reduced_read));
            SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.get(unit, &is_psp_read));

            cli_out("***********************************************************\n");
            cli_out("** SRH Base Initiator CTest is_psp=(%d), is_reduced=(%d) **\n", is_psp_read, is_reduced_read);
            cli_out("***********************************************************\n");

            /*
             * define the SRH Base EEDB entry
             */
            /** we will pass the global-out-LIF id*/
            srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;
            /** convert SRH global-LIF id to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, srh_base_global_lif);
            /** we have 3 SIDs*/
            srh_base_info.nof_sids = 3;
            /** set TTL and QOS modes */
            srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
            srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
            srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
            /** set next encap_id to 0 */
            srh_base_info.next_encap_id = 0;
            srh_base_info.qos_map_id = 0;
            srh_base_info.dscp = 0;

            /*
             * define the SRH Base EEDB entry
             */
            /** we will pass the global-out-LIF id*/
            replace_srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID | BCM_SRV6_SRH_BASE_INITIATOR_REPLACE;
            /** convert SRH global-LIF id to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(replace_srh_base_info.tunnel_id, srh_base_global_lif);
            /** we have 3 SIDs*/
            replace_srh_base_info.nof_sids = 3;
            /** set TTL and QOS modes */
            replace_srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
            replace_srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            replace_srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
            replace_srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
            /** set next encap_id to 0 */
            replace_srh_base_info.next_encap_id = 0;
            replace_srh_base_info.qos_map_id = 0;
            replace_srh_base_info.dscp = 0;
            /** call the sid_initiator API to create the SRH Base EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** creating SRH Base entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info));

            /** call the sid_initiator API to create the SRH Base EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** replacing SRH Base entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_create(unit, &replace_srh_base_info));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 1 entry */
            {
                bcm_srv6_srh_base_initiator_traverse_info_t additional_info;
                int counter = 0, expected_counter = 1;
                SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_traverse
                                (unit, additional_info, srh_base_initiator_counter_cb, &counter));
                if (counter != expected_counter)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "traverse found %d entries, but only %d is expected\n.",
                                 counter, expected_counter);
                }
            }
            cli_out("**** traverse completed ********\n");

            /** print the SRH Base */
            PRT_TITLE_SET("expected SRH Base initiator structure after create");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_srh_base_initiator_struct(unit, &replace_srh_base_info, prt_ctr));
            PRT_COMMITX;

            /*
             * Read back the SRH Base entry and compare the original structures
             */

            /** initialize structure */
            received_srh_base_info.qos_map_id = 0;
            received_srh_base_info.flags = 0;
            received_srh_base_info.ttl = 0;
            received_srh_base_info.dscp = 0;
            received_srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            received_srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
            received_srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
            received_srh_base_info.next_encap_id = 0;
            received_srh_base_info.nof_sids = 0;

            /** use the tunnel_id that was assigned at SID create */
            received_srh_base_info.tunnel_id = srh_base_info.tunnel_id;

            cli_out("**** getting SRH Base entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_get(unit, &received_srh_base_info));

            /** print received SID structure*/
            PRT_TITLE_SET("received SRH Base structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_srh_base_initiator_struct(unit, &received_srh_base_info, prt_ctr));
            PRT_COMMITX;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_srh_base_initiator_struct
                            (unit, &replace_srh_base_info, &received_srh_base_info));

            /*
             * check deletion case for SRH Base initiator
             */
            cli_out("**** deleting SRH Base entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_delete(unit, &replace_srh_base_info));

            cli_out("**** getting SRH Base entry ********\n");
            rv = bcm_srv6_srh_base_initiator_get(unit, &received_srh_base_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv,
                             "API bcm_srv6_srh_base_initiator_delete() didn't delete well the SID initiator mapping !");
            }

        } /** of is_reduced loop */
    } /** of is_psp loop */

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Semantic test of basic SRv6 SID initiator APIs
 *          Sequence:
 *              -Create a SID entry
 *              -Read back the SID structure and compare to above
 *              -Modify a SID entry
 *              -Read back the SID structure and compare to above
 *              -Delete the SID entry
 *              -Read again the SID entry and expect _SHR_E_NOT_FOUND
 *
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_test_basic_sid_initiator_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    bcm_srv6_sid_initiator_info_t sid_info;
    bcm_srv6_sid_initiator_info_t replace_sid_info;
    bcm_srv6_sid_initiator_info_t received_sid_info;

    bcm_ip6_t sid = { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 };
    bcm_ip6_t sid_2 =
        { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };

    int rv;
    uint8 is_psp, is_reduced;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    bcm_srv6_sid_initiator_info_t_init(&replace_sid_info);
    bcm_srv6_sid_initiator_info_t_init(&received_sid_info);

    for (is_psp = 0; is_psp < 2; is_psp++)
    {
        /** Configure the SW state to PSP/USP mode */
        SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.set(unit, is_psp));

        for (is_reduced = 0; is_reduced < 2; is_reduced++)
        {
            uint8 is_psp_read = -1;
            uint8 is_reduced_read = -1;

            /** Configure the SW state to Reduced/Normal mode */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.set(unit, is_reduced));

            /** Read back the SW states for validation */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.get(unit, &is_reduced_read));
            SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.get(unit, &is_psp_read));

            cli_out("******************************************************\n");
            cli_out("** SID Initiator CTest is_psp=(%d), is_reduced=(%d) **\n", is_psp_read, is_reduced_read);
            cli_out("******************************************************\n");

            /*
             *  create structure for SID EEDB entry
             */

            /** don't use any special flags */
            sid_info.flags = 0;

            /** must set the tunnel id to 0 if not using WITH_ID flag */
            sid_info.tunnel_id = 0;

            /** set SID address to be last SID0 */
            sal_memcpy(sid_info.sid, sid, 16);

            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4;

            /** set to next_encap */
            sid_info.next_encap_id = 0;
            /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** creating SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_create(unit, &sid_info));

            /*
             *  create structure for SID EEDB replace entry
             */

            /** don't use any special flags */
            replace_sid_info.flags = BCM_SRV6_SID_INITIATOR_WITH_ID | BCM_SRV6_SID_INITIATOR_REPLACE;

            /** must set the tunnel id to 0 if not using WITH_ID flag */
            replace_sid_info.tunnel_id = sid_info.tunnel_id;

            /** set SID address to be last SID0 */
            sal_memcpy(replace_sid_info.sid, sid_2, 16);

            /** set to EEDB entry of SID0 */
            replace_sid_info.encap_access = bcmEncapAccessTunnel4;

            /** set to next_encap */
            replace_sid_info.next_encap_id = 0;

            /** call the sid_initiator API to replace the SID EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** creating SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_create(unit, &replace_sid_info));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 1 entry */
            {
                bcm_srv6_sid_initiator_traverse_info_t additional_info;
                int counter = 0, expected_counter = 1;
                SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_traverse
                                (unit, additional_info, sid_initiator_counter_cb, &counter));
                if (counter != expected_counter)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "traverse found %d entries, but only %d is expected\n.",
                                 counter, expected_counter);
                }
            }
            cli_out("**** traverse completed ********\n");

            /*
             * test add/get/delete with SID structure
             */
            PRT_TITLE_SET("expected SID initiator structure before create");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_sid_initiator_struct(unit, &replace_sid_info, prt_ctr));
            PRT_COMMITX;

            /** read back the SID entry and compare the original structures */
            received_sid_info.encap_access = 0;
            received_sid_info.flags = 0;
            received_sid_info.next_encap_id = 0;
            sal_memset(received_sid_info.sid, 0, sizeof(received_sid_info.sid));
            /** use the tunnel_id that was assigned at SID create */
            received_sid_info.tunnel_id = sid_info.tunnel_id;

            cli_out("**** getting SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_get(unit, &received_sid_info));

            /** print received SID structure*/
            PRT_TITLE_SET("received SID structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_sid_initiator_struct(unit, &received_sid_info, prt_ctr));
            PRT_COMMITX;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_sid_initiator_struct(unit, &replace_sid_info, &received_sid_info));

            /*
             * check deletion case for SID initiator
             */
            cli_out("**** deleting IPv6 next_protocol mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_delete(unit, &replace_sid_info));

            cli_out("**** getting IPv6 next_protocol mapping ********\n");
            rv = bcm_srv6_sid_initiator_get(unit, &received_sid_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "API bcm_srv6_sid_initiator_delete() didn't delete well the SID initiator mapping !");
            }

        } /** of is_reduced loop */
    } /** of is_psp loop */

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}


static sh_sand_man_t dnx_srv6_test_basic_next_protocol_man = {
    /** Brief */
    .brief = "Semantic test of basic SRv6 Next_Protocol to RCH port and encapsulation mapping",
    /** Full */
    .full =
        "Create x2 next_protocol mappings (IPv4, IPv6) to different RCH ports and encapsulations."
        "Read back the x2 next_protocols mappings and compare to above."
        "Modify x2 next_protocol mappings (IPv4, IPv6)"
        "Read back the x2 next_protocols mappings and compare to above."
        "Delete the x2 next_protocols mappings." "Read again the x2 next_protocols and expect _SHR_E_NOT_FOUND"
};


static sh_sand_man_t dnx_srv6_test_basic_srh_base_initiator_man = {
    /** Brief */
    .brief = "Semantic test of basic SRv6 SRH Base initiator APIs",
    /** Full */
    .full =
        "Create a SRH Base entry"
        "Read back the SRH Base structure and compare to above"
        "Modify a SRH Base entry"
        "Read back the SRH Base structure and compare to above"
        "Delete the SRH Base entry." "Read again the SRH Base entry and expect _SHR_E_NOT_FOUND"
};


static sh_sand_man_t dnx_srv6_test_basic_sid_initiator_man = {
    /** Brief */
    .brief = "Semantic test of basic SRv6 SID initiator APIs",
    /** Full */
    .full =
        "Create a SID entry"
        "Read back the SID structure and compare to above"
        "Modify a SID entry"
        "Read back the SID structure and compare to above"
        "Delete the SID entry." "Read again the SID entry and expect _SHR_E_NOT_FOUND"
};

static sh_sand_man_t dnx_srv6_test_basic_sub_man = {
    /** Brief */
    "Semantic test of basic SRv6 APIs",
    /** Full */
    "",
    /** Synopsis   */
    "",
    /** Example   */
    "",
};

static sh_sand_cmd_t dnx_srv6_test_basic_sub_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
    ********************************************************************************************************************************/
    {"Next_PRotoCoL", dnx_srv6_test_basic_next_protocol_cmds, NULL, NULL, &dnx_srv6_test_basic_next_protocol_man},
    {"SID", dnx_srv6_test_basic_sid_initiator_cmds, NULL, NULL, &dnx_srv6_test_basic_sid_initiator_man},
    {"SRH", dnx_srv6_test_basic_srh_base_initiator_cmds, NULL, NULL, &dnx_srv6_test_basic_srh_base_initiator_man},
    {NULL}
};

/********************************************************************************************************************************
 * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
 *          *                           * Level                   *                                *                            *
 *          *                           * CMD                     *                                *                            *
 ********************************************************************************************************************************/

/** List of SRv6 tests   */
sh_sand_cmd_t dnx_srv6_test_cmds[] = {
    {"BaSiC", NULL, dnx_srv6_test_basic_sub_cmds, NULL, &dnx_srv6_test_basic_sub_man},
    {NULL}
};
