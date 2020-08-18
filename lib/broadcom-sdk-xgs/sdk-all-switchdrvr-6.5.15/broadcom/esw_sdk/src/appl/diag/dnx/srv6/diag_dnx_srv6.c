/** \file diag_dnx_srv6.c
 *
 *
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SRV6

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/swstate/auto_generated/access/lif_mngr_access.h>
#include <bcm/tunnel.h>
#include <bcm/l2.h>


/*************
 * TYPEDEFS  *
 *************/

/*************
 *   MACROS  *
 *************/

#define __ADD_GLEM_ENTRY_IF_GLOBAL_NOT_NULL(__global_lif, __local_lif)                                    \
    if (__global_lif)                                                                                     \
    {                                                                                                     \
        /** Write global to local LIF mapping to GLEM. */                                                 \
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, *__global_lif, __local_lif));         \
    }                                                                                                     \

/*************
* FUNCTIONS *
*************/
extern int bshell(int unit, char* cmd);

static sh_sand_man_t dnx_srv6_lif_alloc_man = {
    "TBD"
};

static sh_sand_option_t dnx_srv6_lif_alloc_options[] = {
    {"NofSegments", SAL_FIELD_TYPE_UINT32, "Number of segments in Tunnel", "2", NULL, NULL, SH_SAND_ARG_QUIET},
    {"Test_MoDe", SAL_FIELD_TYPE_UINT32, "Indication if called from SRv6 test", "0", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_man_t dnx_srv6_egress_alloc_man = {
    "TBD"
};

static sh_sand_option_t dnx_srv6_egress_alloc_options[] = {
    {"Test_MoDe", SAL_FIELD_TYPE_UINT32, "Indication if called from SRv6 test", "0", NULL, NULL, SH_SAND_ARG_QUIET},
    {"PROTOcol", SAL_FIELD_TYPE_UINT32, "SRH Next Protocol value (default is 0xff)", "0xff", NULL, NULL,
     SH_SAND_ARG_QUIET},
    {"Port", SAL_FIELD_TYPE_UINT32, "Recycle Port to use", "0", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static shr_error_e
cmd_dnx_srv6_ingress_tunnel_config_legacy(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip6_t sip_ipv6 =
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 };
    bcm_ip6_t sid_3 =
        { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x5a, 0x5a };
    bcm_ip6_t sid_2 =
        { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 };
    bcm_ip6_t sid_1 =
        { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x22, 0x22 };
    bcm_ip6_t sid_0 =
        { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc };
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    int lif_base, lif_1, lif_2, lif_3, lif_4, first_segment_lif = 0;
    uint32 nof_segments = 0;
    uint32 test_mode = 0;
    uint32 entry_handle_id;
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;
    uint32 arp_global_lif = 4996;
    bcm_gport_t arp_gport;
    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("NofSegments", nof_segments);
    SH_SAND_GET_UINT32("Test_MoDe", test_mode);

    if (test_mode == 0)
    {
        LOG_CLI((BSL_META("Srv6 tunnelConfig diagnostic should run from srv6 test only\n")));
        SHR_EXIT();
    }
    /*
     * Allocate the srv6 tunnel outlifs
     */
    /** SRV6 Base entry - Always part of he tunnel, should be pointed from GLEM */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_BASE;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_SRV6;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info));
    lif_base = outlif_info.local_outlif;

    /** SRV6 phase1 - first SID in tunnel in case of 4 segments */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info));
    lif_1 = outlif_info.local_outlif;

    /** SRV6 phase2 - first SID in tunnel in case of 3 segments */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1 + 1;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info));
    lif_2 = outlif_info.local_outlif;

    /** SRV6 phase3 - first SID in tunnel in case of 2 segments */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1 + 2;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info));
    lif_3 = outlif_info.local_outlif;

    /** SRV6 phase4 - last SID in tunnel */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1 + 3;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info));
    lif_4 = outlif_info.local_outlif;

    if (nof_segments == 2)
    {
        first_segment_lif = lif_3;
    }
    else if (nof_segments == 3)
    {
        first_segment_lif = lif_2;
    }
    else if (nof_segments == 4)
    {
        first_segment_lif = lif_1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported nof_segments %d. can be 2-4 \n", nof_segments);
    }

    /** Add the SRv6 base entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, arp_global_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_RESULT_TYPE_GLOBAL_LIF_EM_LIF_TYPE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, lif_base);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

    /** Add the SRv6 base entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_SRV6, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_VAR, INST_SINGLE, nof_segments * 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, INST_SINGLE, nof_segments - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_SEGMENT_LEFT, INST_SINGLE, nof_segments - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, first_segment_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Create IPv6 Tunnel of type SRv6 */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_init_set.dip6, sip_ipv6, 16);
    sal_memcpy(tunnel_init_set.sip6, sip_ipv6, 16);
    BCM_L3_ITF_SET(tunnel_init_set.l3_intf_id, BCM_L3_ITF_TYPE_LIF, arp_global_lif);
    BCM_GPORT_TUNNEL_ID_SET(arp_gport, arp_global_lif);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, arp_gport);
    SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, tunnel_init_set.tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                     &tunnel_gport_hw_resources));

    /** Add the SRv6 phase1 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 3);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, lif_2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add the SRv6 phase2 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 3);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, lif_3);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add the SRv6 phase3 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 3);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, lif_4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add the SRv6 phase4 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 3);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                 tunnel_gport_hw_resources.local_out_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnx_srv6_ingress_tunnel_config(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip6_t sip_ipv6 =
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 };
    bcm_ip6_t sid_3 =
        { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x5a, 0x5a };
    bcm_ip6_t sid_2 =
        { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 };
    bcm_ip6_t sid_1 =
        { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x22, 0x22 };
    bcm_ip6_t sid_0 =
        { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc };
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    int lif_base, lif_1, lif_2, lif_3, lif_4;
    uint32 nof_segments = 0;
    uint32 test_mode = 0;
    uint32 entry_handle_id;
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;
    int global_lif_base = 0x1230;
    int global_lif_first = 0x1231;
    uint32 flag_lif_1 = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    uint32 flag_lif_2 = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    uint32 flag_lif_3 = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    uint32 flag_lif_4 = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    int *global_lif_1 = NULL;
    int *global_lif_2 = NULL;
    int *global_lif_3 = NULL;
    int *global_lif_4 = NULL;
    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("NofSegments", nof_segments);
    SH_SAND_GET_UINT32("Test_MoDe", test_mode);

    if (test_mode == 0)
    {
        LOG_CLI((BSL_META("Srv6 tunnelConfig diagnostic should run from srv6 test only\n")));
        SHR_EXIT();
    }
    /*
     * Allocate the srv6 tunnel outlifs
     */
    /** SRV6 Base entry - Always part of he tunnel, should be pointed from GLEM */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_BASE;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_SRV6;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_GLOBAL_LIF_WITH_ID, &global_lif_base, NULL, &outlif_info));
    lif_base = outlif_info.local_outlif;
    /** Write global to local LIF mapping to GLEM. */
    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif_base, lif_base));

    if (nof_segments == 2)
    {
        flag_lif_3 = LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif_3 = &global_lif_first;
    }
    else if (nof_segments == 3)
    {
        flag_lif_2 = LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif_2 = &global_lif_first;
    }
    else if (nof_segments == 4)
    {
        flag_lif_1 = LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif_1 = &global_lif_first;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported nof_segments %d. can be 2-4 \n", nof_segments);
    }

    /** SRV6 phase1 - first SID in tunnel in case of 4 segments */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flag_lif_1, global_lif_1, NULL, &outlif_info));
    lif_1 = outlif_info.local_outlif;
    __ADD_GLEM_ENTRY_IF_GLOBAL_NOT_NULL(global_lif_1, lif_1);

    /** SRV6 phase2 - first SID in tunnel in case of 3 segments */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1 + 1;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flag_lif_2, global_lif_2, NULL, &outlif_info));
    lif_2 = outlif_info.local_outlif;
    __ADD_GLEM_ENTRY_IF_GLOBAL_NOT_NULL(global_lif_2, lif_2);

    /** SRV6 phase3 - first SID in tunnel in case of 2 segments */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1 + 2;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flag_lif_3, global_lif_3, NULL, &outlif_info));
    lif_3 = outlif_info.local_outlif;
    __ADD_GLEM_ENTRY_IF_GLOBAL_NOT_NULL(global_lif_3, lif_3);

    /** SRV6 phase4 - last SID in tunnel */
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1 + 3;
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flag_lif_4, global_lif_4, NULL, &outlif_info));
    lif_4 = outlif_info.local_outlif;
    __ADD_GLEM_ENTRY_IF_GLOBAL_NOT_NULL(global_lif_4, lif_4);

    /** Add the SRv6 base entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SRV6, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_VAR, INST_SINGLE, nof_segments * 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, INST_SINGLE, nof_segments - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_SEGMENT_LEFT, INST_SINGLE, nof_segments - 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Create IPv6 Tunnel of type SRv6 */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_init_set.dip6, sip_ipv6, 16);
    sal_memcpy(tunnel_init_set.sip6, sip_ipv6, 16);

    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

    SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set));
    /** extract the ipv6 tunnel local lif in order to point to it from SID list local lif **/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, tunnel_init_set.tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                     &tunnel_gport_hw_resources));

    /** Add the SRv6 phase1 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, lif_2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add the SRv6 phase2 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, lif_3);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add the SRv6 phase3 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, lif_4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add the SRv6 phase4 entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, sid_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                 tunnel_gport_hw_resources.local_out_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Create a Recycle Entry.
 * unit - relevant unit
 * eth_type - ether type, per what's layer is passed for RCH ETH dummy packet encapsulation
 * recycle_entry_encap_id - returned encap id.
 */
int
create_recycle_entry(
    int unit,
    uint32 eth_type,
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;

    bcm_mac_t dest_my_mac = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 };     /* my-MAC per l3 basic example IPV4oETH */
    bcm_mac_t src_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 }; /* src MAC not really important to termination */

    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.ethertype = eth_type;
    recycle_entry.outer_tpid = 0x8100;
    recycle_entry.inner_tpid = 0x9100;
    sal_memcpy(recycle_entry.dest_mac, dest_my_mac, sizeof(dest_my_mac));
    sal_memcpy(recycle_entry.src_mac, src_mac, sizeof(src_mac));
    /** recycle_entry.dest_mac = cint_vxlan_tunnel_bud_node_info.recycle_mac_address; */
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;

    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    *recycle_entry_encap_id = recycle_entry.encap_id;

    return rv;
}

/*
 * Set Out-Port default properties:
 * - out_port: Outgoing port ID
 * function configures a default OUT_LIF in Egress which points to the out_port
 */
int
out_port_set(
    int unit,
    int out_port)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    int rc;
    char *proc_name;

    proc_name = "out_port_set";
    /*
     * printf("%s(): Enter. out_port %d\n",proc_name, out_port); 
     */
    bcm_vlan_port_t_init(&vlan_port);
    bcm_port_match_info_t_init(&match_info);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;

    vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
    vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;

    rc = bcm_vlan_port_create(unit, &vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create vlan_port\n", proc_name);
        return rc;
    }

    printf("%s(): bcm_vlan_port_create() result: port = %d, out_lif = 0x%08X\r\n", proc_name, out_port,
           vlan_port.vlan_port_id);

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = out_port;
    rc = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rc != 0)
    {
        printf("%s(): Error, in bcm_port_match_add \n", proc_name);
        return rc;
    }

    return rc;
}

static int
srh_nxt_hdr_to_eth_type(
    int srh_next_header_idx)
{
    int map_array[256] = { 0 };

    /** IPV4 */
    map_array[4] = 0x0800;

    /** IPV6 */
    map_array[41] = 0x86DD;

    return map_array[srh_next_header_idx];
}

static shr_error_e
cmd_dnx_srv6_egress_tunnel_config(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint32 test_mode = 0;
    uint32 srh_next_protocol = 0;
    int core;
    lif_mngr_local_inlif_info_t inlif_info;
    int local_inlif;
    int rv;

    int recycle_entry_encap_id;
    /** global lif to send to do the RCH header for 2nd pass */
    uint32 global_lif_rch_header;
    int recycle_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("Test_MoDe", test_mode);
    SH_SAND_GET_UINT32("PROTOcol", srh_next_protocol);
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_UINT32("Port", recycle_port);

    /** validate valid core input, else assume core=0 */
    if (!(core >= 0 && core <= dnx_data_device.general.nof_cores_get(unit) - 1))
    {
        core = 0;
    }

    /** check that we're called from TCL otherwise exit */
    if (test_mode == 0)
    {
        LOG_CLI((BSL_META("Srv6 tunnelConfigEgress diagnostic should run from srv6 test only\n")));
        SHR_EXIT();
    }

    /*
     * Allocate the Egress srv6 tunnel P2P IN_LIF
     */
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED;
    inlif_info.core_id = _SHR_CORE_ALL;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    local_inlif = inlif_info.local_inlif;

    /*
     * Received a local_inlif from allocation manager, we can now define add to VTT5 Lookup table SRV6_TERMINATED_LIF
     * do same as [bcm shell "dbal ENTry CoMmit TaBLe=SRV6_TERMINATED_LIF IP_PROTOCOL=0xFF P_FLAG=0 O_FLAG=0 A_FLAG=0 CORE_ID=0 IN_LIF=3"]
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_TERMINATED_LIF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, srh_next_protocol);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Add the P2P IN_LIF itself in the IN_LIF_FORMAT_SRV6_TERMINATED for Recycling first time
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** configure port as recycle port, Part of context selection at 2nd pass VTT1 */
    rv = bcm_port_control_set(unit, recycle_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_control_set \n");
        return rv;
    }

    /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
    rv = create_recycle_entry(unit, srh_nxt_hdr_to_eth_type(srh_next_protocol), &recycle_entry_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_recycle_entry \n");
        return rv;
    }

    
#ifdef ADAPTER_SERVER_MODE
    bshell(unit, "dbal entry coMmit table=TRAP_FWD_RCY_CMD_MAP RECYCLE_COMMAND_ID=15 CORE_ID=0 TRAP_FWD_STRENGTH=0");
    bshell(unit,
           "dbal entry commit table=EGRESS_ETPP_TRAP_CONTEXT_PROPERTIES trap_context_id=GENERAL___RCH_ENC RCY_ENABLE=1");
#endif

    /** Get from encap_id the global_lif_id to do the RCH header encapsulation */
    global_lif_rch_header = BCM_L3_ITF_VAL_GET(recycle_entry_encap_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, 0);
    /** in order to not use the P2P IN_LIF Destination and Global_OUT_LIF but go to FWD */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, global_lif_rch_header);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_ID, INST_SINGLE, recycle_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, 0X0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_srv6_man = {
    "TBD"
};

sh_sand_cmd_t sh_dnx_srv6_diag_cmds[] = {
    {"TUNnelConFiG", cmd_dnx_srv6_ingress_tunnel_config, NULL, dnx_srv6_lif_alloc_options, &dnx_srv6_lif_alloc_man}
    ,
    {"TUNnelConFiGLegacy", cmd_dnx_srv6_ingress_tunnel_config_legacy, NULL, dnx_srv6_lif_alloc_options,
     &dnx_srv6_lif_alloc_man}
    ,
    {"TUNnelConFiGEgress", cmd_dnx_srv6_egress_tunnel_config, NULL, dnx_srv6_egress_alloc_options,
     &dnx_srv6_egress_alloc_man}
    ,
    {NULL}
};
/* *INDENT-ON* */
