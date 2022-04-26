/** \file ctest_dnx_srv6.c
 *
 * Test of nof_sids APIs (add, get, delete)
 */

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/bcm_dlist.h>
#include <include/bcm/qos.h>
#include <include/bcm/vlan.h>
#include <include/bcm/tunnel.h>
#include <include/bcm/srv6.h>
#include <bcm/l3.h>
#include <bcm/switch.h>
#include "ctest_dnx_srv6.h"
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <appl/diag/sand/diag_sand_packet.h>
/** SW state access of the SRv6 */
#include <soc/dnx/swstate/auto_generated/access/srv6_access.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/** DNX data */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/** represents the test case of nof_sids in the SRv6 Terminator mapping */
#define SRV6_TERMINATOR_NOF_SIDS       (5)

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
 *   \param [in] info - nof_sids structure
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
 *   \param [in] info - nof_sids structure
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
 * \brief - print 2 bcm_srv6_extension_terminator_mapping_t structures
 *
 *   \param [in] unit
 *   \param [in] info - nof_sids structure
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
shr_error_e
ctest_dnx_srv6_print_nof_sids_struct(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info,
    prt_control_t * prt_ctr)
{

    SHR_FUNC_INIT_VARS(unit);

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "nof_sids");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "flags");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "port [gport]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "encap_id [l3 int]");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", info->nof_sids);
    PRT_CELL_SET("%d", info->flags);
    PRT_CELL_SET("%d", info->port);
    PRT_CELL_SET("%d", info->encap_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - compare 2 bcm_srv6_extension_terminator_mapping_t structures
 *
 *   \param [in] unit
 *   \param [in] expected_struct
 *   \param [in] test_struct
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok
 */
shr_error_e
ctest_dnx_srv6_compare_nof_sids_struct(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * expected_struct,
    bcm_srv6_extension_terminator_mapping_t * test_struct)
{
    SHR_FUNC_INIT_VARS(unit);

    if (expected_struct->nof_sids != test_struct->nof_sids)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match in nof_sids: expected.nof_sids=%d, test.nof_sids=%d",
                     expected_struct->nof_sids, test_struct->nof_sids);
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
                     "structures don't match encap_access nof_sids: expected.encap_access=%d, test.encap_access=%d",
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

    if (expected_struct->flags != test_struct->flags)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "structures don't match in flags: expected.flags=%d, test.flags=%d",
                     expected_struct->flags, test_struct->flags);
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

    /** if create API was called with estimated_encap_size=-1, sdk set default value and return it in the "get" API */
    if ((expected_struct->estimated_encap_size != test_struct->estimated_encap_size) &&
        (expected_struct->estimated_encap_size != -1))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "structures don't match in estimated_encap_size: expected=%d, test=%d",
                     expected_struct->estimated_encap_size, test_struct->estimated_encap_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - srv6_nof_sids mapping traverse callbacl function example.
 *          used to count entries
 */
int
nof_sids_counter_cb(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info,
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
 * \brief - Semantic test of basic SRv6 nof_sids to RCH port and encapsulation mapping
 *          Sequence:
 *           Create a nof_sids mapping to same RCH ports and encapsulations.
 *           Read back the nof_sids mapping and compare to above.
 *           Modify the nof_sids mapping.
 *           Read back the nof_sids mapping and compare to above.
 *           Delete the nof_sids mapping." "Read again the nof_sidss and expect _SHR_E_NOT_FOUND
 *
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_test_basic_nof_sids_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** valid ports in the system are 200-203 */
    int port = 200;
    int encap_id = 0x100002;
    int port_2 = 202;
    int encap_id_2 = 0x100004;

    bcm_srv6_extension_terminator_mapping_t nof_sids_info;
    bcm_srv6_extension_terminator_mapping_t replace_nof_sids_info;
    bcm_srv6_extension_terminator_mapping_t get_nof_sids_info;

    int rv;
    uint8 is_psp, is_reduced;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_srv6_extension_terminator_mapping_t_init(&nof_sids_info);
    bcm_srv6_extension_terminator_mapping_t_init(&replace_nof_sids_info);
    bcm_srv6_extension_terminator_mapping_t_init(&get_nof_sids_info);

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

            cli_out("********************************************************\n");
            cli_out("** SRv6 Terminator CTest is_psp=(%d), is_reduced=(%d) **\n", is_psp_read, is_reduced_read);
            cli_out("********************************************************\n");

            /*
             *  create structure for nof_sids mapping
             */
            nof_sids_info.flags = 0;
            nof_sids_info.nof_sids = SRV6_TERMINATOR_NOF_SIDS;
            /** convert Port to GPORT */
            BCM_GPORT_SYSTEM_PORT_ID_SET(nof_sids_info.port, port);
            /** convert from global-LIF to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(nof_sids_info.encap_id, encap_id);

            /*
             *  create structure for nof_sids replace mapping
             */
            replace_nof_sids_info.flags = BCM_SRV6_EXTENSION_TERMINATOR_REPLACE;
            replace_nof_sids_info.nof_sids = SRV6_TERMINATOR_NOF_SIDS;
            /** convert Port to GPORT */
            BCM_GPORT_SYSTEM_PORT_ID_SET(replace_nof_sids_info.port, port_2);
            /** convert from global-LIF to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(replace_nof_sids_info.encap_id, encap_id_2);

            /** call the nof_sids for  API to map to RCH port and encapsulation */
            cli_out("**** adding nof_sids mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_extension_terminator_add(unit, &nof_sids_info));
            /** call the nof_sids API to map to RCH port and encapsulation */
            cli_out("**** replacing nof_sids mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_extension_terminator_add(unit, &replace_nof_sids_info));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 2 entries per core */
            {
                bcm_srv6_extension_terminator_traverse_info_t additional_info = { 0 };
                int counter = 0, expected_counter = 1;
                SHR_IF_ERR_EXIT(bcm_srv6_extension_terminator_traverse
                                (unit, additional_info, nof_sids_counter_cb, &counter));
                if (counter != expected_counter)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "traverse found %d entries, but only %d is expected\n.",
                                 counter, expected_counter);
                }
            }
            cli_out("**** traverse completed ********\n");

            /** read back the nof_sids mapping and compare the structure */
            get_nof_sids_info.flags = 0;
            get_nof_sids_info.nof_sids = SRV6_TERMINATOR_NOF_SIDS;
            cli_out("**** getting nof_sids mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_extension_terminator_get(unit, &get_nof_sids_info));

            /*
             * test add/get/delete with  nof_sids structure
             */
            PRT_TITLE_SET("expected nof_sids structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_nof_sids_struct(unit, &replace_nof_sids_info, prt_ctr));
            PRT_COMMITX;

            /** print received structure*/
            PRT_TITLE_SET("received  nof_sids structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_nof_sids_struct(unit, &get_nof_sids_info, prt_ctr));
            PRT_COMMITX;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_nof_sids_struct(unit, &replace_nof_sids_info, &get_nof_sids_info));

            /*
             * check deletion case for nof_sids=SRV6_TERMINATOR_NOF_SIDS mapping
             */
            get_nof_sids_info.encap_id = 0;
            get_nof_sids_info.port = 0;
            get_nof_sids_info.flags = 0;
            get_nof_sids_info.nof_sids = SRV6_TERMINATOR_NOF_SIDS;
            cli_out("**** deleting nof_sids mapping ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_extension_terminator_delete(unit, &get_nof_sids_info));

            get_nof_sids_info.flags = 0;
            get_nof_sids_info.nof_sids = SRV6_TERMINATOR_NOF_SIDS;
            cli_out("**** getting nof_sids mapping ********\n");
            rv = bcm_srv6_extension_terminator_get(unit, &get_nof_sids_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv,
                             "API bcm_srv6_extension_terminator_delete() didn't delete well the nof_sids mapping !");
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
    int srh_base_global_lif = 0x8230;

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

            bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
            bcm_srv6_srh_base_initiator_info_t_init(&replace_srh_base_info);
            bcm_srv6_srh_base_initiator_info_t_init(&received_srh_base_info);

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
             * define the replace SRH Base EEDB entry
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
            /** call the srh_initiator API to create the SRH Base EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** creating SRH Base entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info));

            /** call the srh_initiator API to do the replace SRH Base EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** replacing SRH Base entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_create(unit, &replace_srh_base_info));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 1 entry */
            {
                bcm_srv6_srh_base_initiator_traverse_info_t additional_info = { 0 };
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

            /** before comparison in expected structure expecting following flags */
            replace_srh_base_info.flags = 0;
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

            /*
             * Check ext encap cases
             */
            cli_out("****Starting Ext encap cases ********\n");
            bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
            bcm_srv6_srh_base_initiator_info_t_init(&received_srh_base_info);

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
            if (dnx_data_lif.out_lif.global_lif_allocation_estimated_encap_size_enable_get(unit))
            {
                srh_base_info.estimated_encap_size =
                    dnx_data_lif.out_lif.global_srv6_outlif_bta_sop_get(unit) * srh_base_info.nof_sids + 8;
            }
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info));

            /** use the tunnel_id that was assigned at SID create */
            received_srh_base_info.tunnel_id = srh_base_info.tunnel_id;

            cli_out("**** getting SRH Base with additional sids entry ********\n");
            /** in order to validate estimated_encap_size, we will set it to zero - sdk will return its real value */
            if (dnx_data_lif.out_lif.global_lif_allocation_estimated_encap_size_enable_get(unit))
            {
                received_srh_base_info.estimated_encap_size = 0;
            }
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_get(unit, &received_srh_base_info));

            /** before comparison in expected structure expecting following flags */
            srh_base_info.flags = 0;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_srh_base_initiator_struct
                            (unit, &srh_base_info, &received_srh_base_info));
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_delete(unit, &received_srh_base_info));

            bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
            bcm_srv6_srh_base_initiator_info_t_init(&received_srh_base_info);

            /** we will pass the global-out-LIF id*/
            srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;
            /** convert SRH global-LIF id to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, srh_base_global_lif);
            srh_base_info.nof_sids = 5;
            /** set TTL and QOS modes */
            srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
            srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
            srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
            /** set next encap_id to 0 */
            srh_base_info.next_encap_id = 0;
            srh_base_info.qos_map_id = 0;
            srh_base_info.dscp = 0;
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info));

            /** use the tunnel_id that was assigned at SID create */
            received_srh_base_info.tunnel_id = srh_base_info.tunnel_id;

            cli_out("**** getting SRH Base ext encap entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_get(unit, &received_srh_base_info));

            /** before comparison in expected structure expecting following flags */
            srh_base_info.flags = 0;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_srh_base_initiator_struct
                            (unit, &srh_base_info, &received_srh_base_info));
            SHR_IF_ERR_EXIT(bcm_srv6_srh_base_initiator_delete(unit, &received_srh_base_info));
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
    bcm_srv6_sid_initiator_info_t eedb_pointed_sid_info;

    bcm_ip6_t sid = { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 };
    bcm_ip6_t sid_2 =
        { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };

    int rv;
    uint8 is_psp, is_reduced;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    bcm_srv6_sid_initiator_info_t_init(&replace_sid_info);
    bcm_srv6_sid_initiator_info_t_init(&eedb_pointed_sid_info);
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
            cli_out("**** creating SID Replace entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_create(unit, &replace_sid_info));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 1 entry */
            {
                bcm_srv6_sid_initiator_traverse_info_t additional_info = { 0 };
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
            cli_out("**** deleting SID initiator entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_delete(unit, &replace_sid_info));

            cli_out("**** trying to get the deleted SID entry ********\n");
            rv = bcm_srv6_sid_initiator_get(unit, &received_sid_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "API bcm_srv6_sid_initiator_delete() didn't delete well the SID initiator mapping !");
            }

            cli_out("**** Passed Regular Pointed Outlif Checks ********\n");

            /*
             * creating EEDB Virtual Pointed only SID entry - No ID
             */


            eedb_pointed_sid_info.flags = BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED;

            /** must set the tunnel id to 0 if not using WITH_ID flag */
            eedb_pointed_sid_info.tunnel_id = 0;

            /** set SID address to be last SID0 */
            sal_memcpy(eedb_pointed_sid_info.sid, sid, 16);

            /** set to EEDB entry of SID0 */
            eedb_pointed_sid_info.encap_access = bcmEncapAccessTunnel3;

            /** set to next_encap */
            eedb_pointed_sid_info.next_encap_id = 0;

            /** call the sid_initiator API to create EEDB only pointed SID entry */
            cli_out("**** creating EEDB Pointed Only SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_create(unit, &eedb_pointed_sid_info));

            PRT_TITLE_SET("expected EEDB Pointed Only SID initiator structure before create");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_sid_initiator_struct(unit, &eedb_pointed_sid_info, prt_ctr));
            PRT_COMMITX;

            /*
             * Create structure for EEDB Virtual Pointed SID EEDB replace entry
             */
            replace_sid_info.flags =
                BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED | BCM_SRV6_SID_INITIATOR_WITH_ID |
                BCM_SRV6_SID_INITIATOR_REPLACE;

             /** must set the tunnel id to 0 if not using WITH_ID flag */
            replace_sid_info.tunnel_id = eedb_pointed_sid_info.tunnel_id;

             /** changing SID address  */
            sal_memcpy(replace_sid_info.sid, sid_2, 16);

             /** changing EEDB entry phase as well */
            replace_sid_info.encap_access = bcmEncapAccessTunnel3;

             /** set to next_encap */
            replace_sid_info.next_encap_id = 0;

             /** call the sid_initiator API to replace the SID EEDB Entry and return its GPORT Tunnel-id */
            cli_out("**** creating replace EEDB Pointed Only  SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_create(unit, &replace_sid_info));

            cli_out("**** Validating traverse ********\n");
            /** Validate traverse, count entries, need to find 1 entry */
            {
                bcm_srv6_sid_initiator_traverse_info_t additional_info = { 0 };
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
             *  Read back the SID entry and compare to the replaced structure
             */
            received_sid_info.encap_access = 0;
            received_sid_info.flags = 0;
            received_sid_info.next_encap_id = 0;
            sal_memset(received_sid_info.sid, 0, sizeof(received_sid_info.sid));
            /** use the tunnel_id that was assigned at SID create */
            received_sid_info.tunnel_id = eedb_pointed_sid_info.tunnel_id;

            cli_out("**** getting EEDB Pointed Only SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_get(unit, &received_sid_info));

            PRT_TITLE_SET("received EEDB Pointed Only SID structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_sid_initiator_struct(unit, &received_sid_info, prt_ctr));
            PRT_COMMITX;

            /** compare the received and expected structures */
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_sid_initiator_struct(unit, &replace_sid_info, &received_sid_info));

            cli_out("**** deleting EEDB Pointed Only SID entry ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_delete(unit, &eedb_pointed_sid_info));

            cli_out("**** trying to get the deleted EEDB Pointed Only SID entry ********\n");
            rv = bcm_srv6_sid_initiator_get(unit, &received_sid_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "API bcm_srv6_sid_initiator_delete() didn't delete well the SID initiator mapping !");
            }

            /*
             * creating EEDB Virtual Pointed only SID entry With-ID
             */

            eedb_pointed_sid_info.flags =
                BCM_SRV6_SID_INITIATOR_WITH_ID | BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED;

             /** must set the tunnel id to 0 if not using WITH_ID flag */
            eedb_pointed_sid_info.tunnel_id = 0x4E000001;  /** to use a new WITH_ID tunnel_id */

             /** set SID address to be last SID0 */
            sal_memcpy(eedb_pointed_sid_info.sid, sid, 16);

             /** set to EEDB entry of SID0 */
            eedb_pointed_sid_info.encap_access = bcmEncapAccessTunnel2;

             /** set to next_encap */
            eedb_pointed_sid_info.next_encap_id = 0;

             /** call the sid_initiator API to create EEDB only pointed SID entry */
            cli_out("**** creating EEDB Pointed Only SID entry WITH-ID ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_create(unit, &eedb_pointed_sid_info));

            PRT_TITLE_SET("expected EEDB Pointed Only SID WITH-ID initiator structure before create");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_sid_initiator_struct(unit, &eedb_pointed_sid_info, prt_ctr));
            PRT_COMMITX;

            received_sid_info.encap_access = 0;
            received_sid_info.flags = 0;
            received_sid_info.next_encap_id = 0;
            sal_memset(received_sid_info.sid, 0, sizeof(received_sid_info.sid));
             /** use the tunnel_id that was assigned at SID create */
            received_sid_info.tunnel_id = eedb_pointed_sid_info.tunnel_id;

            cli_out("**** get the EEDB Pointed Only SID entry WITH-ID ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_get(unit, &received_sid_info));

            PRT_TITLE_SET("received EEDB Pointed Only SID WITH-ID structure");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_print_sid_initiator_struct(unit, &received_sid_info, prt_ctr));
            PRT_COMMITX;

            cli_out("**** comparing EEDB Pointed Only SID entry WITH-ID ********\n");
            SHR_IF_ERR_EXIT(ctest_dnx_srv6_compare_sid_initiator_struct
                            (unit, &eedb_pointed_sid_info, &received_sid_info));

            cli_out("**** deleting EEDB Pointed Only SID entry WITH-ID ********\n");
            SHR_IF_ERR_EXIT(bcm_srv6_sid_initiator_delete(unit, &received_sid_info));

            cli_out("**** trying to get the deleted EEDB Pointed Only SID entry WITH-ID ********\n");
            rv = bcm_srv6_sid_initiator_get(unit, &received_sid_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv,
                             "API bcm_srv6_sid_initiator_delete() didn't delete well the EEDB Only SID initiator mapping WITH-ID !");
            }

            cli_out("**** Passed Virtual Non-Pointed Outlif Checks ********\n");

        } /** of is_reduced loop */
    } /** of is_psp loop */

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 *   Retrieve the first valid FEC for the given hierarchy
 *   which is not in the ECMP range.
 * \param [in] unit - unit id
 * \param [in] hierarchy - hirarchy ID
 * \param [out] first_fec - First valid FEC in the allocated range for the hierarchy out of the ECMP range.
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_l3_egress_first_fec_get(
    int unit,
    uint32 hierarchy,
    int *first_fec)
{
    bcm_switch_fec_property_config_t fec_config;
    uint32 ecmp_range = dnx_data_l3.ecmp.nof_ecmp_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    fec_config.flags = hierarchy;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    if (ecmp_range > fec_config.start && ecmp_range < fec_config.end)
    {
        *first_fec = ecmp_range;
    }
    else
    {
        *first_fec = fec_config.start;;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_srv6_interfaces_config(
    int unit)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t ingress_rif;
    bcm_port_t in_port = 200;
    bcm_port_t out_port = 202;
    bcm_l3_egress_t l3eg, l3eg_ingress_tunnel_arp_to_ac, l3eg_ingress_tunnel, l3eg_ingress_tunnel_arp,
        l3eg_ingress_tunnel_sid0;
    bcm_l3_route_t l3rt;
    int kaps_result, kaps_result_2nd;
    int intf_in = 15;
    int intf_out = 100;
    int vrf = 1;
    int encap_id = 0x6384;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
    bcm_mac_t arp_next_hop_mac_ingress_tunnel = { 0x00, 0x00, 0xcd, 0x00, 0xcd, 0x1d };
    bcm_mac_t arp_to_ac_next_hop_mac = { 0x00, 0xaa, 0x00, 0x00, 0xcd, 0x1d };
    bcm_ip6_t ip6_mask =
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    bcm_ip6_t ip6_dip_route = { 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0, 0, 0x00, 0x00, 0xFF, 0x13 };
    bcm_gport_t gport;
    int srv6_basic_lif_encode;
    int encoded_fec1;
    int srv6_first_lif_encode;
    int encoded_fec2;
    bcm_l3_host_t host;
    uint32 addr = 0x7fffff02;
    uint32 addr_arp_to_ac = 0x7fffffff;
    int sid_lif_id = 0x4231, srh_lif_id = 0x2230;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the initial FEC ID values for 1st and 2nd hierarchy*/
    SHR_IF_ERR_EXIT(dnx_l3_egress_first_fec_get(unit, BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY, &kaps_result));
    SHR_IF_ERR_EXIT(dnx_l3_egress_first_fec_get(unit, BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY, &kaps_result_2nd));

    /** Create ingress AC LIF*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = intf_in;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_add(unit, intf_in, in_port, 0), "Test failed, bcm_vlan_gport_add failed.\n");

    /** Set Out-Port default properties*/
    bcm_vlan_port_t_init(&vlan_port);
    bcm_port_match_info_t_init(&match_info);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = out_port;
    SHR_CLI_EXIT_IF_ERR(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info),
                        "Test failed, bcm_port_match_add failed.\n");
    /** Create outport for ARP->AC*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");

    /**Create ARP pointing to AC*/
    bcm_l3_egress_t_init(&l3eg_ingress_tunnel_arp_to_ac);
    sal_memcpy(l3eg_ingress_tunnel_arp_to_ac.mac_addr, arp_to_ac_next_hop_mac, sizeof(bcm_mac_t));
    l3eg_ingress_tunnel_arp_to_ac.encap_id = encap_id + 0x384;
    l3eg_ingress_tunnel_arp_to_ac.vlan = intf_out;
    l3eg_ingress_tunnel_arp_to_ac.flags = 0;
    l3eg_ingress_tunnel_arp_to_ac.vlan_port_id = vlan_port.vlan_port_id;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg_ingress_tunnel_arp_to_ac, NULL),
                        "Test failed, bcm_l3_egress_create failed.\n");

    /** Create ingress and egress ETH-RIFs and set its properties */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_in_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_in;
    l3if.l3a_vid = intf_in;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed, bcm_l3_intf_create failed.\n");

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_out_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_out;
    l3if.l3a_vid = intf_out;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed, bcm_l3_intf_create failed.\n");

    /** Set Incoming ETH-RIF properties, such as VRF*/
    bcm_l3_ingress_t_init(&ingress_rif);
    ingress_rif.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_rif.vrf = vrf;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_ingress_create(unit, &ingress_rif, &intf_in),
                        "Test failed, bcm_l3_ingress_create failed.\n");

    /** Create ARP for the egress tunnel and the endpoint cases and set its properties*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, arp_next_hop_mac, sizeof(bcm_mac_t));
    l3eg.encap_id = encap_id;
    l3eg.vlan = intf_out;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, NULL),
                        "Test failed, bcm_l3_egress_create failed.\n");
    encap_id = l3eg.encap_id;

    /** Create ARP for the ingress tunnel case and set its properties*/
    bcm_l3_egress_t_init(&l3eg_ingress_tunnel_arp);
    sal_memcpy(l3eg_ingress_tunnel_arp.mac_addr, arp_next_hop_mac_ingress_tunnel, sizeof(bcm_mac_t));
    l3eg_ingress_tunnel_arp.encap_id = 0x6385;
    l3eg_ingress_tunnel_arp.vlan = intf_out;
    l3eg_ingress_tunnel_arp.flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg_ingress_tunnel_arp, NULL),
                        "Test failed, bcm_l3_egress_create failed.\n");

    /** Create FEC entry used as destination for the endpoint and the egress node cases */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = intf_out;
    l3eg.encap_id = encap_id;
    l3eg.port = out_port;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &kaps_result),
                        "Test failed, bcm_l3_egress_create failed.\n");

    /** Create 2 Hierarchical FEC entry used as destination for the ingress node case */
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, srh_lif_id);
    BCM_GPORT_FORWARD_PORT_SET(gport, kaps_result_2nd + 0x2500);
    encoded_fec1 = kaps_result + 0x2500;
    encoded_fec2 = kaps_result_2nd + 0x2500;
    bcm_l3_egress_t_init(&l3eg_ingress_tunnel);
    l3eg_ingress_tunnel.intf = srv6_basic_lif_encode;
    l3eg_ingress_tunnel.port = gport;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg_ingress_tunnel, &(encoded_fec1)),
                        "Test failed, bcm_l3_egress_create failed.\n");

    BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, sid_lif_id);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    bcm_l3_egress_t_init(&l3eg_ingress_tunnel_sid0);
    l3eg_ingress_tunnel_sid0.intf = srv6_first_lif_encode;
    l3eg_ingress_tunnel_sid0.port = gport;
    l3eg_ingress_tunnel_sid0.flags = BCM_L3_2ND_HIERARCHY;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg_ingress_tunnel_sid0, &(encoded_fec2)),
                        "Test failed, bcm_l3_egress_create failed.\n");

    /** Add Route and Host entries */
    bcm_l3_route_t_init(&l3rt);
    sal_memcpy(l3rt.l3a_ip6_net, ip6_dip_route, 16);
    sal_memcpy(l3rt.l3a_ip6_mask, ip6_mask, 16);
    l3rt.l3a_vrf = 1;
    l3rt.l3a_intf = kaps_result;
    l3rt.l3a_flags = BCM_L3_IP6;
    l3rt.l3a_flags2 =
        (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? BCM_L3_FLAGS2_FWD_ONLY : 0);
    SHR_CLI_EXIT_IF_ERR(bcm_l3_route_add(unit, &l3rt), "Test failed, bcm_l3_route_add failed.\n");

    BCM_GPORT_FORWARD_PORT_SET(gport, kaps_result + 0x2500);
    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = l3eg_ingress_tunnel_arp.encap_id;
    host.encap_id = 0;
    host.l3a_port_tgid = gport;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_add(unit, &host), "Test failed, bcm_l3_host_add failed.\n");

    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = addr_arp_to_ac;
    host.l3a_vrf = vrf;
    host.l3a_intf = l3eg_ingress_tunnel_arp_to_ac.encap_id;
    host.encap_id = 0;
    host.l3a_port_tgid = gport;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_add(unit, &host), "Test failed, bcm_l3_host_add failed.\n");

    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, " dnx_srv6_interfaces_config .\nResult: PASS!\n")));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Srv6 basic endpoint traffic test
 */
static shr_error_e
dnx_srv6_test_endpoint_sub_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int intf_id;
    bcm_ip6_t ip6_mask =
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    bcm_ip6_t ip6_dip = { 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0, 0, 0xFF, 0xFF, 0xFF, 0x13 };
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_l3_ingress_t ingress_tunnel_lif;
    int vrf = 1;
    int in_port = 200;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    SHR_FUNC_INIT_VARS(unit);

    /** Configure the port header type of the in_port, in case of egress node and enpoint it should be ETH */
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    SHR_CLI_EXIT_IF_ERR(bcm_switch_control_indexed_port_set(unit, in_port, key, value),
                        "Test failed, bcm_switch_control_indexed_port_set failed.\n");

    SHR_CLI_EXIT_IF_ERR(dnx_srv6_interfaces_config(unit), "Test failed, dnx_srv6_interfaces_config failed.\n");
    /** Configure the keys for the resulting LIF on termination (tunnel type, DIP, VRF) */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = vrf;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    SHR_CLI_EXIT_IF_ERR(bcm_tunnel_terminator_create(unit, &tunnel_term_set),
                        "Test failed, bcm_tunnel_terminator_create failed.\n");

    /** Configure the termination resulting LIF's FODO to VRF */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term_set.tunnel_id);

    /** Set Incoming ETH-RIF properties*/
    bcm_l3_ingress_t_init(&ingress_tunnel_lif);
    ingress_tunnel_lif.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_tunnel_lif.vrf = 1;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_ingress_create(unit, &ingress_tunnel_lif, &intf_id),
                        "Test failed, bcm_l3_ingress_create failed.\n");
    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Srv6 endpoint configuration.\nResult: PASS!\n")));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Srv6 basic egress node traffic test
 */
static shr_error_e
dnx_srv6_test_egress_tunnel_sub_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int intf_id;
    bcm_ip6_t ip6_mask =
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    bcm_ip6_t ip6_dip = { 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0x56, 0x78, 0xFF, 0xFF, 0xFF, 0x13 };
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_l3_ingress_t ingress_tunnel_lif;
    int vrf = 1;
    int in_port = 200;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    SHR_FUNC_INIT_VARS(unit);

    /** Configure the port header type of the in_port, in case of egress node and enpoint it should be ETH */
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    SHR_CLI_EXIT_IF_ERR(bcm_switch_control_indexed_port_set(unit, in_port, key, value),
                        "Test failed, bcm_switch_control_indexed_port_set failed.\n");

    SHR_CLI_EXIT_IF_ERR(dnx_srv6_interfaces_config(unit), "Test failed, dnx_srv6_interfaces_config failed.\n");

    /** Configure the keys for the resulting LIF on termination (DIP, VRF, tunnel type) */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = vrf;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    SHR_CLI_EXIT_IF_ERR(bcm_tunnel_terminator_create(unit, &tunnel_term_set),
                        "Test failed, bcm_tunnel_terminator_create failed.\n");

    /** Configure the termination resulting LIF's FODO to VRF */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term_set.tunnel_id);
    bcm_l3_ingress_t_init(&ingress_tunnel_lif);
    ingress_tunnel_lif.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_tunnel_lif.vrf = 1;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_ingress_create(unit, &ingress_tunnel_lif, &intf_id),
                        "Test failed, bcm_l3_ingress_create failed.\n");

    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Srv6 egress node configuration.\nResult: PASS!\n")));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Srv6 basic ingress_tunnel traffic test
 */
static shr_error_e
dnx_srv6_test_ingress_tunnel_sub_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip6_t ip6_sid0 =
        { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc };
    bcm_ip6_t ip6_sip = { 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0, 0, 0xFF, 0xFF, 0x13, 0x13 };
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;
    bcm_srv6_sid_initiator_info_t sid_info;
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    int ip_tunnel_lif_id = 0x4231, srh_lif_id = 0x2230;

    SHR_FUNC_INIT_VARS(unit);
    /** Call the generic function for configuring ingress and egress interfaces*/
    SHR_CLI_EXIT_IF_ERR(dnx_srv6_interfaces_config(unit), "Test failed, dnx_srv6_interfaces_config failed.\n");

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** Configure the SID encapsulation entry, which will point to the Ipv6 tunnel one*/
    sid_info.flags = 0;
    sal_memcpy(sid_info.sid, ip6_sid0, 16);
    sid_info.encap_access = bcmEncapAccessTunnel4;
    SHR_CLI_EXIT_IF_ERR(bcm_srv6_sid_initiator_create(unit, &sid_info),
                        "Test failed, bcm_srv6_sid_initiator_create failed.\n");

    /** define the SRH Base EEDB entry */
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, srh_lif_id);
    srh_base_info.nof_sids = 1;
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    srh_base_info.nof_additional_sids_extended_encap = 0;
    srh_base_info.next_encap_id = 0;
    srh_base_info.qos_map_id = 0;
    srh_base_info.dscp = 0;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    SHR_CLI_EXIT_IF_ERR(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info),
                        "Test failed, bcm_srv6_srh_base_initiator_create failed.\n");

    /** Configure the encapsulation Ipv6 entry for the SIP*/
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_init_set.sip6, ip6_sip, 16);
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, ip_tunnel_lif_id);
    SHR_CLI_EXIT_IF_ERR(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set),
                        "Test failed, bcm_tunnel_initiator_create failed.\n");

    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Srv6 ingress tunnel configuration.\nResult: PASS!\n")));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Srv6 basic ingress_tunnel traffic test
 */
static shr_error_e
dnx_srv6_test_ingress_tunnel_arp_sub_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip6_t ip6_sid0 =
        { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc };
    bcm_ip6_t ip6_sip = { 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0, 0, 0xFF, 0xFF, 0x13, 0x13 };
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;
    bcm_srv6_sid_initiator_info_t sid_info;
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    int ip_tunnel_lif_id = 0x4231, srh_lif_id = 0x2230;

    SHR_FUNC_INIT_VARS(unit);
    /** Call the generic function for configuring ingress and egress interfaces*/
    SHR_CLI_EXIT_IF_ERR(dnx_srv6_interfaces_config(unit), "Test failed, dnx_srv6_interfaces_config failed.\n");

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** Configure the SID encapsulation entry, which will point to the Ipv6 tunnel one*/
    sid_info.flags = 0;
    sal_memcpy(sid_info.sid, ip6_sid0, 16);
    sid_info.encap_access = bcmEncapAccessTunnel3;
    SHR_CLI_EXIT_IF_ERR(bcm_srv6_sid_initiator_create(unit, &sid_info),
                        "Test failed, bcm_srv6_sid_initiator_create failed.\n");

    /** define the SRH Base EEDB entry */
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, srh_lif_id);
    srh_base_info.nof_sids = 1;
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    srh_base_info.nof_additional_sids_extended_encap = 0;
    srh_base_info.next_encap_id = 0;
    srh_base_info.qos_map_id = 0;
    srh_base_info.dscp = 0;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    SHR_CLI_EXIT_IF_ERR(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info),
                        "Test failed, bcm_srv6_srh_base_initiator_create failed.\n");

    /** Configure the encapsulation Ipv6 entry for the SIP*/
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_init_set.sip6, ip6_sip, 16);
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.encap_access = bcmEncapAccessRif;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, ip_tunnel_lif_id);
    SHR_CLI_EXIT_IF_ERR(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set),
                        "Test failed, bcm_tunnel_initiator_create failed.\n");

    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Srv6 ingress tunnel configuration.\nResult: PASS!\n")));
exit:
    SHR_FUNC_EXIT;
}
static sh_sand_man_t dnx_srv6_test_basic_nof_sids_man = {
    /** Brief */
    .brief = "Semantic test of basic SRv6 nof_sids to RCH port and encapsulation mapping",
    /** Full */
    .full =
        "Create a nof_sids mapping to same RCH ports and encapsulations."
        "Read back the nof_sids mapping and compare to above."
        "Modify the nof_sids mapping"
        "Read back the nof_sids mapping and compare to above."
        "Delete the nof_sids mapping." "Read again the nof_sidss and expect _SHR_E_NOT_FOUND"
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

static sh_sand_man_t dnx_srv6_test_endpoint_sub_man = {
    /** Brief */
    "Traffic tests of SRv6 endpoint/ingress and egress node flows",
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
    {"TERMINATOR", dnx_srv6_test_basic_nof_sids_cmds, NULL, NULL, &dnx_srv6_test_basic_nof_sids_man},
    {"SID", dnx_srv6_test_basic_sid_initiator_cmds, NULL, NULL, &dnx_srv6_test_basic_sid_initiator_man},
    {"SRH", dnx_srv6_test_basic_srh_base_initiator_cmds, NULL, NULL, &dnx_srv6_test_basic_srh_base_initiator_man},
    {NULL}
};

static sh_sand_cmd_t dnx_srv6_test_traffic_sub_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
    ********************************************************************************************************************************/
    {"ENDPOINT", dnx_srv6_test_endpoint_sub_cmds, NULL, NULL, NULL},
    {"IngressTunnel", dnx_srv6_test_ingress_tunnel_sub_cmds, NULL, NULL, NULL},
    {"IngressTunnelArp", dnx_srv6_test_ingress_tunnel_arp_sub_cmds, NULL, NULL, NULL},
    {"EgressTunnel", dnx_srv6_test_egress_tunnel_sub_cmds, NULL, NULL, NULL},
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
    {"Traffic", NULL, dnx_srv6_test_traffic_sub_cmds, NULL, &dnx_srv6_test_endpoint_sub_man},
    {NULL}
};
