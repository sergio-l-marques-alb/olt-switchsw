/* $Id:
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: cint_rx_trap_svtag.c
 * Purpose: Shows an example for configuration of SV Tag traps - svtag
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_svtag.c
 *
 * Main Function:
 *      cint_rx_trap_svtag_main(unit,sci,sci_profile,err_code,err_code_profile,action_gport);
 * Destroy Function:
 *      cint_rx_trap_svtag_destroy(unit);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_svtag.c
 * cint
 * cint_rx_trap_svtag_main(0,0xC,2,0x1B,2,0x1600f019);
 */

bcm_rx_trap_svtag_key_t  cint_rx_trap_svtag_svtag_trap_key = {0};

int dnx_svtag_swap(
    int unit,
    int in_core,
    int in_port)
{
    int rv;
    int inst_id = -1;
    uint32 entry_handle_id;
    uint32 port = in_port;
    uint32 core = in_core;
   /*
    * Enable global swap
    */
   dnx_dbal_entry_handle_take(unit, "INGRESS_PTC_PORT", &entry_handle_id);
   dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "PTC", &port);
   dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "CORE_ID", &core);

   uint32 tag_swap_enable = 1; /* enable */
   dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "TAG_SWAP_ENABLE", inst_id, &tag_swap_enable);
   uint32 tag_swap_mode = 0; /* DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER */
   dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "TAG_SWAP_OP_MODE", inst_id, &tag_swap_mode);
   dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");


   /*
    * Set the global swap
    */

   dnx_dbal_entry_handle_take(unit, "SIT_INGRESS_GENERAL_CONFIGURATION", &entry_handle_id);
   uint32 swap_offset = 14; /* 12 bytes of the SA DA and another 2 for the PTCH */
   dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "GLOBAL_SWAP_TPID_OFFSET", inst_id, &swap_offset);
   uint32 swap_size = 0; /* DBAL_ENUM_FVAL_SIT_TAG_SWAP_SIZE_4_BYTES which is the size of the SVTAG */
   dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "GLOBAL_SWAP_TPID_SIZE", inst_id, &swap_size);
   dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");


    return rv;
}

int cint_rx_trap_svtag_main(
    int unit,
    int in_port,
    int in_core,
    int sci,
    int sci_profile,
    int err_code,
    int err_code_profile,
    bcm_gport_t action_gport)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;

    rv = dnx_svtag_swap(unit, in_core, in_port);

    key.type = bcmSwitchSvtagSciProfile;
    key.index = sci;

    info.value = sci_profile;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_set SCI \n");
        return rv;
    }

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = err_code;

    info.value = err_code_profile;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_set Error Code \n");
        return rv;
    }

    cint_rx_trap_svtag_svtag_trap_key.sci_profile = sci_profile;
    cint_rx_trap_svtag_svtag_trap_key.error_code_profile = err_code_profile;
    rv = bcm_rx_trap_svtag_set(unit, &cint_rx_trap_svtag_svtag_trap_key, action_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_svtag_set \n");
        return rv;
    }

    return rv;
}


int cint_rx_trap_svtag_destroy(
     int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_rx_trap_svtag_set(unit, &cint_rx_trap_svtag_svtag_trap_key, BCM_GPORT_INVALID);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_svtag_set \n");
        return rv;
    }

    return rv;
}
