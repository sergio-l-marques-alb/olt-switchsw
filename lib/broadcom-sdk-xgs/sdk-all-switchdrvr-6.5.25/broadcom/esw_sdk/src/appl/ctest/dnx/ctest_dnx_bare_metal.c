#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_string_apis.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

#include <shared/bsl.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <sal/core/thread.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_COMMON

/** Structure to describe a single DBAL table */
typedef struct dnx_bridge_router_table_def_t
{
    char *table_name;
    char **key_fields;
    int nof_key_fields;
    uint8 key_is_default[5];
    char **result_fields;
    int nof_result_fields;
    uint8 result_is_default[5];
    char *result_type;
    char *table_type;
} dnx_bridge_router_table_def_t;

static uint32
bare_metal_add_fec_entry(
    int unit,
    int with_id,
    uint32 *fec_id,
    uint32 destination,
    uint32 outlif0,
    uint32 outlif1)
{
    uint32 entry_handle_id = 0;
    uint32 super_fec_val = 0;
    uint32 fec_arr[3] = { 0 };
    uint32 fec_instance = 0;
    int fec_id_int = *fec_id;

    dnx_cint_algo_l3_fec_allocate(unit, &fec_id_int, (with_id ? 0x1 : 0), "HIERARCHY_LEVEL_1", "NO_PROT_NO_STAT");

    *fec_id = fec_id_int;
    super_fec_val = fec_id_int / 2;
    fec_instance = fec_id_int & 0x1;

    dnx_dbal_entry_handle_take(unit, "SUPER_FEC_1ST_HIERARCHY", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "SUPER_FEC_ID", &super_fec_val);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", -1, "SUPER_FEC_NO_PROTECTION");
    dnx_dbal_fields_struct_field_encode(unit, "FEC_DESTINATION_LIF0_17BIT_LIF1", "DESTINATION", &destination, fec_arr);
    dnx_dbal_fields_struct_field_encode(unit, "FEC_DESTINATION_LIF0_17BIT_LIF1", "GLOB_OUT_LIF", &outlif0, fec_arr);
    dnx_dbal_fields_struct_field_encode(unit, "FEC_DESTINATION_LIF0_17BIT_LIF1", "GLOB_OUT_LIF_2ND", &outlif1, fec_arr);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "FEC_DESTINATION_LIF0_17BIT_LIF1", fec_instance,
                                         fec_arr);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    return 0;
}

static uint32
bare_metal_add_eedb_entry(
    int unit,
    int with_id,
    uint32 *global_out_lif,
    char *table_name,
    char *format_name)
{
    int rv = 0;
    uint32 entry_handle_id = 0;
    uint32 core_id = -17;       /* CORE_ALL */
    uint32 local_lif = 0;
    lif_mngr_local_outlif_str_info_t outlif_info;
    lif_mngr_global_lif_info_t global_lif_info;
    sal_memset(&global_lif_info, 0x0, sizeof(lif_mngr_global_lif_info_t));

    /** Allocate Local lif*/
    if (global_out_lif)
    {
        global_lif_info.global_lif = *global_out_lif;
    }
    outlif_info.outlif_phase_name = "LOGICAL_2";
    outlif_info.table_name = table_name;
    outlif_info.result_type_name = format_name;
    rv = dnx_cint_lif_lib_allocate(unit, with_id, &global_lif_info, NULL, &outlif_info);
    if (rv)
    {
        sal_printf("Error! In \"dnx_cint_lif_lib_allocate\". with_id=%d, global_out_lif=%d, table=%s, format=%s\n",
                   with_id, global_lif_info.global_lif, table_name, format_name);
        return 0xFFFFFFFF;
    }
    *global_out_lif = global_lif_info.global_lif;
    local_lif = outlif_info.local_outlif;

    /** Add glem entry */
    dnx_dbal_entry_handle_take(unit, "GLOBAL_LIF_EM", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "CORE_ID", &core_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "GLOB_OUT_LIF", global_out_lif);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "OUT_LIF", 0, &local_lif);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** IPv4 host table*/
    dnx_dbal_entry_handle_take(unit, table_name, &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "OUT_LIF", &local_lif);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0, format_name);

    return entry_handle_id;
}

static int
npl_Bridge_Router_Common_App_main(
    int unit)
{
    uint32 rv;
    uint32 entry_handle_id;
    uint32 core_id = -17;       /* CORE_ALL */

    uint32 in_port = 201;
    uint32 destination_port = 0xc00ca;
    uint32 ethertype_vlan = 0x8100;
    bcm_mac_t mac_da = { 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
    uint32 ip_address_1 = 0x12123434;
    uint32 ip_address_2 = 0x34341212;
    uint32 vid_route = 1;
    uint32 vsi_bridge = 10;
    uint32 vsi_route = 20;
    uint32 vrf = 1;
    uint32 outlif_eth_encap_1 = 0x112233;
    uint32 outlif_eth_encap_2 = 0x112244;
    uint32 outlif_vlan_editing_1 = 0x112255;
    uint32 new_ethertype = 0x0800;
    bcm_mac_t new_dmac_1 = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };
    bcm_mac_t new_smac_1 = { 0x9a, 0x78, 0x56, 0x34, 0x12, 0x00 };
    bcm_mac_t new_dmac_2 = { 0x12, 0x34, 0x56, 0x12, 0x34, 0x56 };
    bcm_mac_t new_smac_2 = { 0x9a, 0x78, 0x56, 0x9a, 0x78, 0x55 };
    uint32 new_pcp = 5;
    uint32 new_cfi = 1;
    uint32 new_vlan_id_1 = 100;
    uint32 new_tpid_1 = 0x9100;
    uint32 fec;

    /** Port classification table*/
    dnx_dbal_entry_handle_take(unit, "NPL_BRIDGE_ROUTER_BASIC_PORT_CLASSIFICATION_TABLE", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_IN_PORT", &in_port);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0, "NPL_BRIDGE_ROUTER_BASIC_VSI_TYPE");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VSI", 0, &vsi_bridge);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** Port+Vlan_ID classification table*/
    dnx_dbal_entry_handle_take(unit, "NPL_BRIDGE_ROUTER_BASIC_PORT_VLAN_CLASSIFICATION_TABLE", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_IN_PORT", &in_port);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_L2_ETHERTYPE", &ethertype_vlan);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VLAN_TAG_VID", &vid_route);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0, "NPL_BRIDGE_ROUTER_BASIC_VSI_TYPE");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VSI", 0, &vsi_route);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** MyMac per VSI table*/
    dnx_dbal_entry_handle_take(unit, "NPL_BRIDGE_ROUTER_BASIC_MY_MAC_PER_VSI_TABLE", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VSI", &vsi_route);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "CORE_ID", &core_id);
    dnx_dbal_entry_key_field_arr8_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_L2_MACDA", mac_da);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0, "NPL_BRIDGE_ROUTER_BASIC_VRF_TYPE");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VRF", 0, &vrf);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** IPv4 host table*/
    dnx_dbal_entry_handle_take(unit, "NPL_BRIDGE_ROUTER_BASIC_IPV4_UNICAST_HOST_TABLE", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VRF", &vrf);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_IPV4_DIP", &ip_address_1);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0,
                                          "NPL_BRIDGE_ROUTER_BASIC_DESTINATION_OUTLIF_TYPE");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_DESTINATION", 0,
                                         &destination_port);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_OUTLIF", 0,
                                         &outlif_eth_encap_1);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** Allocate FEC and add it the destination port and vlan_editing_2 outlif */
    bare_metal_add_fec_entry(unit, 0, &fec, destination_port, 0, outlif_eth_encap_2);

    /** IPv4 route table*/
    dnx_dbal_entry_handle_take(unit, "NPL_BRIDGE_ROUTER_BASIC_IPV4_UNICAST_ROUTE_TABLE", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VRF", &vrf);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_IPV4_DIP", &ip_address_2);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0, "NPL_BRIDGE_ROUTER_BASIC_FEC_TYPE");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_FEC", 0, &fec);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** Bridge FWD table*/
    dnx_dbal_entry_handle_take(unit, "NPL_BRIDGE_ROUTER_BASIC_FWD_MACT_TABLE", &entry_handle_id);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_VSI", &vsi_bridge);
    dnx_dbal_entry_key_field_arr8_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_L2_MACDA", mac_da);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0,
                                          "NPL_BRIDGE_ROUTER_BASIC_DESTINATION_OUTLIF_TYPE");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_DESTINATION", 0,
                                         &destination_port);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_OUTLIF", 0,
                                         &outlif_vlan_editing_1);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
    dnx_dbal_entry_handle_free(unit, entry_handle_id);

    /** EEDB ETH Encapsulation - for Host*/
    {
        rv = bare_metal_add_eedb_entry(unit, 1, &outlif_eth_encap_1,
                                       "EEDB_NPL_BRIDGE_ROUTER_BASIC_ETH_ENCAPSULATION_TABLE",
                                       "NPL_BRIDGE_ROUTER_BASIC_ETH_ENCAPSULATION_TABLE_TYPE");
        if (rv == 0xFFFFFFFF)
        {
            sal_printf("Error! In \"bare_metal_add_eedb_entry\"\n");
            return rv;
        }
        else
        {
            entry_handle_id = rv;
        }

        dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_ETHERTYPE", 0,
                                             &new_ethertype);
        dnx_dbal_entry_value_field_arr8_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_DMAC", 0, new_dmac_1);
        dnx_dbal_entry_value_field_arr8_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_SMAC", 0, new_smac_1);
        dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
        dnx_dbal_entry_handle_free(unit, entry_handle_id);
    }

    /** EEDB ETH Encapsulation - For route*/
    {
        rv = bare_metal_add_eedb_entry(unit, 1, &outlif_eth_encap_2,
                                       "EEDB_NPL_BRIDGE_ROUTER_BASIC_ETH_ENCAPSULATION_TABLE",
                                       "NPL_BRIDGE_ROUTER_BASIC_ETH_ENCAPSULATION_TABLE_TYPE");
        if (rv == 0xFFFFFFFF)
        {
            sal_printf("Error! In \"bare_metal_add_eedb_entry\"\n");
            return rv;
        }
        else
        {
            entry_handle_id = rv;
        }

        dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_ETHERTYPE", 0,
                                             &new_ethertype);
        dnx_dbal_entry_value_field_arr8_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_DMAC", 0, new_dmac_2);
        dnx_dbal_entry_value_field_arr8_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_SMAC", 0, new_smac_2);
        dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
        dnx_dbal_entry_handle_free(unit, entry_handle_id);
    }
    /** EEDB Vlan Editing 1*/
    {
        rv = bare_metal_add_eedb_entry(unit, 1, &outlif_vlan_editing_1,
                                       "EEDB_NPL_BRIDGE_ROUTER_BASIC_VLAN_EDITING_TABLE",
                                       "NPL_BRIDGE_ROUTER_BASIC_VLAN_EDITING_TABLE_TYPE");
        if (rv == 0xFFFFFFFF)
        {
            sal_printf("Error! In \"bare_metal_add_eedb_entry\"\n");
            return rv;
        }
        else
        {
            entry_handle_id = rv;
        }

        /** EEDB ETH Encapsulation*/
        dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_PCP", 0, &new_pcp);
        dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_CFI", 0, &new_cfi);
        dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_VLAN_ID", 0,
                                             &new_vlan_id_1);
        dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "NPL_BRIDGE_ROUTER_BASIC_NEW_VLAN_ETHERTYPE", 0,
                                             &new_tpid_1);
        dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");
        dnx_dbal_entry_handle_free(unit, entry_handle_id);
    }

    return 0;
}

static shr_error_e
dnx_bare_metal_prepare_and_send_packet(
    int unit,
    bcm_port_t inP,
    char *dmac,
    char *smac,
    int in_tpid,
    int in_vid,
    int ip_ethertype,
    char *sip,
    char *dip_1,
    int in_ttl,
    int checksum)
{

    rhhandle_t packet_h = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate and init packet_h.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));

    /*
     * Add Ethernet header.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.Type", (uint32 *) &ip_ethertype, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.TPID", (uint32 *) &in_tpid, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", (uint32 *) &in_vid, 12));

    /*
     * Add IPv4 header.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.SIP", sip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.DIP", dip_1));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", (uint32 *) &in_ttl, 8));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.Checksum", (uint32 *) &checksum, 8));

    /*
     * Send packer.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, inP, packet_h, SAND_PACKET_RX));
    /*
     * Allow to possible other threads: learning/rx to kick in
     */
    sal_usleep(10000);

exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_bare_metal_npl_bridge_router_common_app_man = {
    "Basic route test, based on the bridge_router image",
};

static sh_sand_man_t dnx_bare_metal_bridge_router_performance_man = {
    "Performance test for adding and getting entries from DBAL tables.",
};

static sh_sand_option_t dnx_dbal_bm_performance_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", NULL},
    {"EntryNum", SAL_FIELD_TYPE_UINT32, "Number of entries", "4096"},
    {NULL}
};

static shr_error_e
dnx_bare_metal_npl_bridge_router_common_app_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    bcm_port_t inP;
    int ip_ethertype;
    char smac[RHNAME_MAX_SIZE] = "00:00:00:00:00:01";
    char dmac[RHNAME_MAX_SIZE] = "12:34:56:78:9a:bc";
    char new_smac_1[RHNAME_MAX_SIZE] = "00:12:34:56:78:9a";
    char new_dmac_1[RHNAME_MAX_SIZE] = "bc:9a:78:56:34:12";
    char new_smac_2[RHNAME_MAX_SIZE] = "55:78:9a:56:78:9a";
    char new_dmac_2[RHNAME_MAX_SIZE] = "56:34:12:56:34:12";

    char dip_1[RHNAME_MAX_SIZE] = "18.18.52.52";
    char dip_2[RHNAME_MAX_SIZE] = "52.52.18.18";
    char sip[RHNAME_MAX_SIZE] = "192.128.1.1";
    int in_tpid, in_vid1, in_vid2, in_checksum, in_ttl;
    int out_tpid, out_vid, out_pcp, out_dei, out_ttl, out_checksum;

    SHR_FUNC_INIT_VARS(unit);

    inP = 201;
    ip_ethertype = 0x800;
    in_tpid = 0x8100;
    in_vid1 = 1;
    in_vid2 = 2;
    in_ttl = 0x80;
    out_tpid = 0x9100;
    out_vid = 100;
    out_pcp = 5;
    out_dei = 1;
    out_ttl = 0x7f;
    in_checksum = 0x3302;
    out_checksum = 0x3303;

    /*
     * Configuration.
     */
    SHR_IF_ERR_EXIT(sh_process_command
                    (unit,
                     "mod IPPD_FWD_ACT_PROFILE 0 512 EGRESS_PARSING_INDEX_VALUE=0 EGRESS_PARSING_INDEX_OVERWRITE=1"));
    SHR_IF_ERR_EXIT(sh_process_command
                    (unit, "m ETPPC_CFG_TERMINATION_FWD_CODE_EN CFG_TERMINATION_FWD_CODE_EN=0xffffffffffffffff"));

    SHR_IF_ERR_EXIT(npl_Bridge_Router_Common_App_main(unit));

    /*
     * Host packet
     */
    SHR_IF_ERR_EXIT(dnx_bare_metal_prepare_and_send_packet
                    (unit, inP, dmac, smac, in_tpid, in_vid1, ip_ethertype, sip, dip_1, in_ttl, in_checksum));

    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));
    /*
     * Check the received packet.
     */
    {
        rhhandle_t packet_h = NULL;
        int match_count;

        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH0"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH0.DA", new_dmac_1));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH0.SA", new_smac_1));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH0.Type", (uint32 *) &ip_ethertype, 16));

        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.SIP", sip));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.DIP", dip_1));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", (uint32 *) &out_ttl, 8));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h, "IPv4.Checksum", (uint32 *) &out_checksum, 16));

        SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_h, &match_count));
        if (match_count == 0)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
        }
    }

    /*
     * Clean the rx buffer.
     */
    diag_sand_rx_clean(unit);

    /*
     * Bridge packet.
     */
    SHR_IF_ERR_EXIT(dnx_bare_metal_prepare_and_send_packet
                    (unit, inP, dmac, smac, in_tpid, in_vid1, ip_ethertype, sip, dip_2, in_ttl, in_checksum));

    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));
    /*
     * Check the received packet.
     */
    {
        rhhandle_t packet_h = NULL;
        int match_count;

        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH0"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH0.DA", new_dmac_2));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH0.SA", new_smac_2));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH0.Type", (uint32 *) &ip_ethertype, 16));

        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.SIP", sip));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.DIP", dip_2));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", (uint32 *) &out_ttl, 8));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h, "IPv4.Checksum", (uint32 *) &out_checksum, 16));

        SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_h, &match_count));
        if (match_count == 0)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
        }
    }

    SHR_IF_ERR_EXIT(dnx_bare_metal_prepare_and_send_packet
                    (unit, inP, dmac, smac, in_tpid, in_vid2, ip_ethertype, sip, dip_1, in_ttl, in_checksum));

    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));
    /*
     * Check the received packet.
     */
    {
        rhhandle_t packet_h = NULL;
        int match_count;

        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.TPID", (uint32 *) &out_tpid, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", (uint32 *) &out_vid, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", (uint32 *) &out_pcp, 3));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.DEI", (uint32 *) &out_dei, 1));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.Type", (uint32 *) &ip_ethertype, 16));

        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.SIP", sip));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.DIP", dip_1));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", (uint32 *) &in_ttl, 8));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h, "IPv4.Checksum", (uint32 *) &out_checksum, 16));

        SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_h, &match_count));
        if (match_count == 0)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
        }
    }

exit:
    /*
     * Clean the rx buffer.
     */
    diag_sand_rx_clean(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Retrieve the relevant fields describing a DBAL table.
 * \param [in] unit - The relevant unit ID
 * \param [in] table_name - The name of the DBAL table
 * \param [out] table_def - A structure which describes the provided DBAL table.
 * \return
 *   \retval Void
 * \see
 *  * dnx_bare_metal_bridge_router_performance_cmd
 */
shr_error_e
dnx_bare_metal_bridge_router_table_get(
    int unit,
    char *table_name,
    dnx_bridge_router_table_def_t * table_def)
{
    SHR_FUNC_INIT_VARS(unit);
    if (sal_strncmp("BRIDGE_ROUTER_IPV4_UNICAST_ROUTE_TABLE", table_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
        == 0)
    {
        table_def->table_name = "BRIDGE_ROUTER_IPV4_UNICAST_ROUTE_TABLE";
        table_def->nof_key_fields = 2;
        table_def->key_fields = sal_alloc(sizeof(char) * 23 * table_def->nof_key_fields, "Unicast route table");
        table_def->key_fields[0] = "VRF";
        table_def->key_fields[1] = "IPV4_DIP";
        table_def->key_is_default[0] = 1;
        table_def->nof_result_fields = 1;
        table_def->result_fields = sal_alloc(sizeof(char) * 18, "Unicast route table");
        table_def->result_fields[0] = "FEC";
        table_def->result_type = "BRIDGE_ROUTER_FEC_FORMAT";
        table_def->table_type = "LPM";
    }
    else if (sal_strncmp
             ("BRIDGE_ROUTER_IPV4_UNICAST_HOST_TABLE", table_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        table_def->table_name = "BRIDGE_ROUTER_IPV4_UNICAST_HOST_TABLE";
        table_def->nof_key_fields = 2;
        table_def->key_fields = sal_alloc(sizeof(char) * 23 * table_def->nof_key_fields, "Unicast host table keys");
        table_def->key_fields[0] = "VRF";
        table_def->key_fields[1] = "IPV4_DIP";
        table_def->key_is_default[0] = 1;
        table_def->nof_result_fields = 1;
        table_def->result_fields = sal_alloc(sizeof(char) * 18, "Unicast host table result");
        table_def->result_fields[0] = "FEC";
        table_def->result_type = "BRIDGE_ROUTER_FEC_FORMAT";
        table_def->table_type = "LEM";
    }
    else if (sal_strncmp("BRIDGE_ROUTER_ETH_BRIDGE_TABLE", table_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) ==
             0)
    {
        table_def->table_name = "BRIDGE_ROUTER_ETH_BRIDGE_TABLE";
        table_def->nof_key_fields = 2;
        table_def->key_fields = sal_alloc(sizeof(char) * 23 * table_def->nof_key_fields, "ETH bridge table keys");
        table_def->key_fields[0] = "VSI";
        table_def->key_fields[1] = "L2_MACDA";
        table_def->key_is_default[0] = 1;
        table_def->nof_result_fields = 2;
        table_def->result_fields =
            sal_alloc(sizeof(char) * 26 * table_def->nof_result_fields, "ETH bridge table result");
        table_def->result_fields[0] = "DESTINATION";
        table_def->result_fields[1] = "OUTLIF";
        table_def->result_type = "BRIDGE_ROUTER_DESTINATION_OUTLIF_FORMAT";
        table_def->table_type = "LEM";
    }
    else if (sal_strncmp
             ("BRIDGE_ROUTER_PORT_VLAN_CLASSIFICATION_TABLE", table_name,
              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        table_def->table_name = "BRIDGE_ROUTER_PORT_VLAN_CLASSIFICATION_TABLE";
        table_def->nof_key_fields = 2;
        table_def->key_fields = sal_alloc(sizeof(char) * 22 * table_def->nof_key_fields, "Port vlan table keys");
        table_def->key_fields[0] = "IN_PORT";
        table_def->key_fields[1] = "VID";
        table_def->nof_result_fields = 1;
        table_def->result_fields = sal_alloc(sizeof(char) * 18, "Port vlan table result");
        table_def->result_fields[0] = "VSI";
        table_def->result_type = "BRIDGE_ROUTER_VSI_FORMAT";
        table_def->table_type = "ISEM";
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Table %s is not defined in ctest \n", table_name);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   A DBAL performance test for Bare-Metal environment.
 */
static shr_error_e
dnx_bare_metal_bridge_router_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint32 nof_entries;
    uint32 default_val = 1;
    uint32 field_value = 1;
    uint32 timer_idx[2] = { 1, 2 };
    uint32 timers_group = UTILEX_LL_TIMER_NO_GROUP;
    uint8 is_image = 0;
    int idx;
    int field_idx;
    int rv;
    dnx_bridge_router_table_def_t table_def;
    dbal_tables_e table_id;
    char *table_name = NULL;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_current_image_check(unit, "Bridge_Router", &is_image));
    if (is_image == 0)
    {
        sal_printf("Expected image for performance test is Bridge_Router\n");
        SHR_EXIT();
    }
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_UINT32("EntryNum", nof_entries);
    /** Make sure a table name was provided to the input parameters. */
    if (!ISEMPTY(table_name))
    {
        /** Make sure the table is defined for the current image. */
        if (dbal_logical_table_string_to_id_no_error(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table must exists for this test\n");
    }

    sal_memset(&table_def, 0, sizeof(dnx_bridge_router_table_def_t));
    SHR_IF_ERR_EXIT(dnx_bare_metal_bridge_router_table_get(unit, table_name, &table_def));

    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    utilex_ll_timer_clear_all(timers_group);

    /**
     * Iterate over the given number of entries.
     * Create the entry and then find it.
     */
    for (idx = 0; idx < nof_entries; idx++)
    {
        /** ENTRY commit */
        dnx_dbal_entry_handle_take(unit, table_name, &entry_handle_id);
        /** Fill in table keys. */
        if (sal_strncmp(table_def.table_type, "ISEM", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
        {
            for (field_idx = 0; field_idx < table_def.nof_key_fields; field_idx++)
            {
                dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, table_def.key_fields[field_idx],
                                                   table_def.key_is_default[field_idx] ? &default_val : &field_value);
            }
        }
        else
        {
            dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, table_def.key_fields[0], &field_value);
            dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, table_def.key_fields[1], &default_val);
        }

        /** Fill in table result fields. */
        dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", 0, table_def.result_type);
        for (field_idx = 0; field_idx < table_def.nof_result_fields; field_idx++)
        {
            dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, table_def.result_fields[field_idx],
                                                 0, &default_val);
        }

        utilex_ll_timer_set("entry commit", timers_group, timer_idx[0]);
        dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT_FORCE");
        utilex_ll_timer_stop(timers_group, timer_idx[0]);
        dnx_dbal_entry_handle_free(unit, entry_handle_id);

        /** ENTRY get*/
        dnx_dbal_entry_handle_take(unit, table_name, &entry_handle_id);
        /** Fill in table key fields. */
        if (sal_strncmp(table_def.table_type, "ISEM", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
        {
            for (field_idx = 0; field_idx < table_def.nof_key_fields; field_idx++)
            {
                dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, table_def.key_fields[field_idx],
                                                   table_def.key_is_default[field_idx] ? &default_val : &field_value);
            }
        }
        else
        {
            dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, table_def.key_fields[0], &field_value);
            dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, table_def.key_fields[1], &default_val);
        }
        utilex_ll_timer_set("entry get", timers_group, timer_idx[1]);
        dnx_dbal_entry_get(unit, entry_handle_id);
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
        dnx_dbal_entry_handle_free(unit, entry_handle_id);
        field_value++;
        if (sal_strncmp(table_def.table_type, "ISEM", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if (field_value == 256)
            {
                field_value = 1;
                default_val++;
            }
        }
    }
    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);
    dnx_dbal_table_clear(unit, table_name);
exit:
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_bare_metal_cmds[] = {
    {"bridge_router", dnx_bare_metal_npl_bridge_router_common_app_cmd, NULL, NULL, &dnx_bare_metal_npl_bridge_router_common_app_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"performance", dnx_bare_metal_bridge_router_performance_cmd, NULL, dnx_dbal_bm_performance_options, &dnx_bare_metal_bridge_router_performance_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {NULL}
};
/* *INDENT-ON* */
