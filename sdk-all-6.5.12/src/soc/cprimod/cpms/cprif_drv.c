/*
 *
 * $Id:$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *
 */
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/esw/port.h>
#include <soc/cprimod/cprimod.h>
#include <soc/cprimod/cprimod_internal.h>
#include <soc/cprimod/cprimod_dispatch.h>
#include <soc/cprimod/cprif_drv.h>



#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef CPRIMOD_CPRI_FALCON_SUPPORT

#define CPRIF_DRV_NUMBER_OF_MSG 1920
#define CPRIF_DRV_N_MG_IN_MSTR_FRM 21
#define CPRIF_DRV_RSVD4_MSG_SIZE 19

#define CPRIF_DRV_MEM_PREP phy_port = SOC_INFO(unit).port_l2p_mapping[port]; \
        port_core_index = ((phy_port-1)%4); \
        local_mem = mem_select[port_core_index]; \
        num_words = soc_mem_entry_words(unit, local_mem); \
        mem_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, ""); \
        SOC_NULL_CHECK(mem_data_ptr); \
        sal_memset(mem_data_ptr, 0, sizeof(uint32) * num_words);



int cprif_bfa_bfp_table_entry_t_init( cprif_bfa_bfp_table_entry_t* cprimod_bfa_bfp_table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprimod_bfa_bfp_table_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprimod_bfa_bfp_table_entry NULL parameter"));
    }
    sal_memset(cprimod_bfa_bfp_table_entry, 0, sizeof(cprif_bfa_bfp_table_entry_t));
    cprimod_bfa_bfp_table_entry->axc_id = 0;
    cprimod_bfa_bfp_table_entry->rsrv_bit_cnt = 0;
    cprimod_bfa_bfp_table_entry->data_bit_cnt = 0;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_container_map_entry_t_init( cprif_cpri_container_map_entry_t* cprif_cpri_container_map_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_cpri_container_map_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_cpri_container_map_entry NULL parameter"));
    }
    sal_memset(cprif_cpri_container_map_entry, 0, sizeof(cprif_cpri_container_map_entry_t));
    cprif_cpri_container_map_entry->map_method = 0;
    cprif_cpri_container_map_entry->axc_id = 0;
    cprif_cpri_container_map_entry->stuffing_at_end = 0;
    cprif_cpri_container_map_entry->stuff_cnt = 0;
    cprif_cpri_container_map_entry->cblk_cnt = 0;
    cprif_cpri_container_map_entry->naxc_cnt = 0;
    cprif_cpri_container_map_entry->Nv = 0;
    cprif_cpri_container_map_entry->Na = 0;
    cprif_cpri_container_map_entry->rfrm_sync = 0;
    cprif_cpri_container_map_entry->hfrm_sync = 0;
    cprif_cpri_container_map_entry->bfn_offset = 0;
    cprif_cpri_container_map_entry->hfn_offset = 0;
    cprif_cpri_container_map_entry->bfrm_offset = 0;


exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_container_map_entry_t_init( cprif_rsvd4_container_map_entry_t* cprif_rsvd4_container_map_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_container_map_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_container_map_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_container_map_entry, 0, sizeof(cprif_rsvd4_container_map_entry_t));
    cprif_rsvd4_container_map_entry->map_method = 0;
    cprif_rsvd4_container_map_entry->axc_id = 0;
    cprif_rsvd4_container_map_entry->stuffing_at_end = 0;
    cprif_rsvd4_container_map_entry->stuff_cnt = 0;
    cprif_rsvd4_container_map_entry->cblk_cnt = 0;
    cprif_rsvd4_container_map_entry->rfrm_offset = 0;
    cprif_rsvd4_container_map_entry->bfn_offset = 0;
    cprif_rsvd4_container_map_entry->msg_ts_mode = 0;
    cprif_rsvd4_container_map_entry->rfrm_sync = 0;
    cprif_rsvd4_container_map_entry->hfrm_sync = 0;
    cprif_rsvd4_container_map_entry->msg_ts_cnt = 0;
    cprif_rsvd4_container_map_entry->msg_ts_sync = 0;
    cprif_rsvd4_container_map_entry->use_ts_dbm = 0;
    cprif_rsvd4_container_map_entry->ts_dbm_prof_num = 0;
    cprif_rsvd4_container_map_entry->num_active_slots = 0;
    cprif_rsvd4_container_map_entry->msg_addr = 0;
    cprif_rsvd4_container_map_entry->msg_type = 0;
    cprif_rsvd4_container_map_entry->msg_ts_offset = 0;


exit:
    SOC_FUNC_RETURN;

}

int cprif_iq_buffer_config_t_init( cprif_iq_buffer_config_t* cprif_iq_buffer_config_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_iq_buffer_config_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_iq_buffer_config_entry NULL parameter"));
    }
    sal_memset(cprif_iq_buffer_config_entry, 0, sizeof(cprif_iq_buffer_config_t));
    cprif_iq_buffer_config_entry->payload_size      = 0;
    cprif_iq_buffer_config_entry->last_packet_num  = 0;
    cprif_iq_buffer_config_entry->last_payload_size = 0;

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_decap_data_entry_t_init( cprif_encap_decap_data_entry_t* cprif_encap_decap_data_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_encap_decap_data_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_encap_decap_data_entry NULL parameter"));
    }
    sal_memset(cprif_encap_decap_data_entry, 0, sizeof(cprif_encap_decap_data_entry_t));
    cprif_encap_decap_data_entry->sample_size         = 0;
    cprif_encap_decap_data_entry->out_sample_size     = 0;
    cprif_encap_decap_data_entry->valid               = 0;
    cprif_encap_decap_data_entry->mux_enable          = 0;
    cprif_encap_decap_data_entry->queue_size          = 0;
    cprif_encap_decap_data_entry->queue_offset        = 0;
    cprif_encap_decap_data_entry->sign_ext_enable     = 0;
    cprif_encap_decap_data_entry->sign_ext_type       = CPRIF_DATA_SIGN_EXT_15_TO_16;
    cprif_encap_decap_data_entry->bit_reversal        = 0;
    cprif_encap_decap_data_entry->tx_cycle_size       = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_encap_decap_data_entry_t_init( cprif_rsvd4_encap_decap_data_entry_t* cprif_rsvd4_encap_decap_data_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_encap_decap_data_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_encap_decap_data_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_encap_decap_data_entry, 0, sizeof(cprif_rsvd4_encap_decap_data_entry_t));
    cprif_rsvd4_encap_decap_data_entry->gsm_pad_size            = 0;
    cprif_rsvd4_encap_decap_data_entry->gsm_extra_pad_size      = 0;
    cprif_rsvd4_encap_decap_data_entry->gsm_pad_enable          = 0;
    cprif_rsvd4_encap_decap_data_entry->gsm_control_location    = 0;
    cprif_rsvd4_encap_decap_data_entry->valid                   = 0;
    cprif_rsvd4_encap_decap_data_entry->queue_size              = 0;
    cprif_rsvd4_encap_decap_data_entry->queue_offset            = 0;
    cprif_rsvd4_encap_decap_data_entry->tx_cycle_size           = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_header_entry_t_init( cprif_encap_header_entry_t* cprif_encap_header_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_encap_header_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_encap_header_entry NULL parameter"));
    }
    sal_memset(cprif_encap_header_entry, 0, sizeof(cprif_encap_header_entry_t));
    cprif_encap_header_entry->header_type           = 0;
    cprif_encap_header_entry->roe_flow_id           = 0;
    cprif_encap_header_entry->roe_subtype           = 0;
    cprif_encap_header_entry->ordering_info_index   = 0;
    cprif_encap_header_entry->mac_da_index          = 0;
    cprif_encap_header_entry->mac_sa_index          = 0;
    cprif_encap_header_entry->vlan_type             = 0;
    cprif_encap_header_entry->vlan_id_0_index       = 0;
    cprif_encap_header_entry->vlan_id_1_index       = 0;
    cprif_encap_header_entry->vlan_0_priority       = 0;
    cprif_encap_header_entry->vlan_1_priority       = 0;
    cprif_encap_header_entry->ether_type_index      = 0;
    cprif_encap_header_entry->use_tagid_for_vlan    = 0;
    cprif_encap_header_entry->use_tagid_for_flowid  = 0;
    cprif_encap_header_entry->use_opcode            = 0;
    cprif_encap_header_entry->roe_opcode            = 0;
    cprif_encap_header_entry->tsn_bitmap            = 0;
    cprif_encap_header_entry->gsm_q_cnt_disable     = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_ordering_info_entry_t_init( cprif_encap_ordering_info_entry_t* cprif_encap_ordering_info_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_encap_ordering_info_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_encap_ordering_info_entry NULL parameter"));
    }
    sal_memset(cprif_encap_ordering_info_entry, 0, sizeof(cprif_encap_ordering_info_entry_t));
    cprif_encap_ordering_info_entry->type       = 0;
    cprif_encap_ordering_info_entry->p_size     = 0;
    cprif_encap_ordering_info_entry->q_size     = 0;
    cprif_encap_ordering_info_entry->max        = 0;
    cprif_encap_ordering_info_entry->increment  = 0;
    cprif_encap_ordering_info_entry->pcnt_prop  = 0;
    cprif_encap_ordering_info_entry->qcnt_prop  = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_decap_ordering_info_entry_t_init( cprif_decap_ordering_info_entry_t* cprif_decap_ordering_info_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_decap_ordering_info_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_decap_ordering_info_entry NULL parameter"));
    }
    sal_memset(cprif_decap_ordering_info_entry, 0, sizeof(cprif_decap_ordering_info_entry_t));

    cprif_decap_ordering_info_entry->type           = 0;
    cprif_decap_ordering_info_entry->p_size         = 0x20;
    cprif_decap_ordering_info_entry->q_size         = 0;
    cprif_decap_ordering_info_entry->max            = 0xFFFFFFFF;
    cprif_decap_ordering_info_entry->increment      = 1;
    cprif_decap_ordering_info_entry->pcnt_inc_p2    = 1;
    cprif_decap_ordering_info_entry->qcnt_inc_p2    = 0;
    cprif_decap_ordering_info_entry->pcnt_extended  = 0;
    cprif_decap_ordering_info_entry->pcnt_pkt_count = 0;
    cprif_decap_ordering_info_entry->modulo_2       = 1;
    cprif_decap_ordering_info_entry->bias           = 1;
    cprif_decap_ordering_info_entry->gsm_tsn_bitmap = 0x55;

exit:
    SOC_FUNC_RETURN;
}


int cprif_header_compare_entry_t_init( cprif_header_compare_entry_t* cprif_header_compare_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_header_compare_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_header_compare_entry NULL parameter"));
    }
    sal_memset(cprif_header_compare_entry, 0, sizeof(cprif_header_compare_entry_t));

    cprif_header_compare_entry->valid           = 0;
    cprif_header_compare_entry->match_data      = 0;
    cprif_header_compare_entry->mask            = 0;
    cprif_header_compare_entry->flow_id         = 0xFF;
    cprif_header_compare_entry->ctrl_flow       = 0;

exit:
    SOC_FUNC_RETURN;
}


int cprif_modulo_rule_entry_t_init( cprif_modulo_rule_entry_t* cprif_modulo_rule_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_modulo_rule_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_modulo_rule_entry NULL parameter"));
    }
    sal_memset(cprif_modulo_rule_entry, 0, sizeof(cprif_modulo_rule_entry_t));

    cprif_modulo_rule_entry->active = 0;
    cprif_modulo_rule_entry->modulo = 0;
    cprif_modulo_rule_entry->modulo_index = 0;
    cprif_modulo_rule_entry->dbm_enable = 0xFF;
    cprif_modulo_rule_entry->flow_id = 0xFF;
    cprif_modulo_rule_entry->ctrl_flow = 0;


exit:
    SOC_FUNC_RETURN;
}


int cprif_rsvd4_dbm_rule_entry_t_init( cprif_rsvd4_dbm_rule_entry_t* cprif_rsvd4_dbm_rule_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_dbm_rule_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_dbm_rule_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_dbm_rule_entry, 0, sizeof(cprif_rsvd4_dbm_rule_entry_t));

    cprif_rsvd4_dbm_rule_entry->bm1_mult = 0;
    cprif_rsvd4_dbm_rule_entry->bm1[0]= 0;
    cprif_rsvd4_dbm_rule_entry->bm1[1]= 0;
    cprif_rsvd4_dbm_rule_entry->bm1[2]= 0;
    cprif_rsvd4_dbm_rule_entry->bm1_size = 0;
    cprif_rsvd4_dbm_rule_entry->bm2[0]= 0;
    cprif_rsvd4_dbm_rule_entry->bm2[1]= 0;
    cprif_rsvd4_dbm_rule_entry->bm2_size = 0;
    cprif_rsvd4_dbm_rule_entry->num_slots = 0;
    cprif_rsvd4_dbm_rule_entry->pos_index = 0;
    cprif_rsvd4_dbm_rule_entry->pos_entries = 0;

exit:
    SOC_FUNC_RETURN;
}


int cprif_rsvd4_secondary_dbm_rule_entry_t_init( cprif_rsvd4_secondary_dbm_rule_entry_t* cprif_rsvd4_secondary_dbm_rule_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_secondary_dbm_rule_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_secondary_dbm_rule_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_secondary_dbm_rule_entry, 0, sizeof(cprif_rsvd4_secondary_dbm_rule_entry_t));

    cprif_rsvd4_secondary_dbm_rule_entry->bm1_mult = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm1 = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm1_size = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm2= 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm2_size = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->num_slots = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_dbm_pos_table_entry_t_init( cprif_rsvd4_dbm_pos_table_entry_t* cprif_rsvd4_dbm_pos_table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_dbm_pos_table_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_dbm_pos_table_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_dbm_pos_table_entry, 0, sizeof(cprif_rsvd4_dbm_pos_table_entry_t));

    cprif_rsvd4_dbm_pos_table_entry->valid      = 0;
    cprif_rsvd4_dbm_pos_table_entry->flow_id    = 0;
    cprif_rsvd4_dbm_pos_table_entry->ctrl_flow  = 0;
    cprif_rsvd4_dbm_pos_table_entry->flow_index = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_active_table_set(int unit, int port, uint8 table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));
    field_buf = table;
    soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                      BFRM_MAP_TAB_IDf, field_buf);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_parser_active_table_get(int unit, int port, uint8* table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));
    field_buf =  soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val, BFRM_MAP_TAB_IDf);
    *table = field_buf;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_table_num_entries_set(int unit, int port, uint8 table, uint32 num_entries)
{
    uint32 reg_val;
    uint32 active_table;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));

    active_table =  soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val, BFRM_MAP_TAB_IDf);

    /*
     * Check to see if it is trying to change number of entries to active table.
     */
    if (active_table == table) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,(_SOC_MSG("cannot change number of entries of active table.")));
    }

    field_buf = num_entries;
    if (table == 0) {
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          NUM_VLD_ENTRIES_TAB0f, field_buf);
    } else {
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          NUM_VLD_ENTRIES_TAB1f, field_buf);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_table_num_entries_get(int unit, int port, uint8 table, uint32* num_entries)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (num_entries == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("num_entries NULL parameter"));
    }

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));
    if (table == 0) {
        field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB0f);
    } else {
        field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB1f);
    }

    *num_entries = field_buf;

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_assembly_active_table_set(int unit, int port, uint8 table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));
    field_buf = table;
    soc_reg_field_set(unit, CPRI_TXFRM_BFA_CTRLr, &reg_val, BFA_MAP_TAB_IDf, field_buf);

    _SOC_IF_ERR_EXIT(WRITE_CPRI_TXFRM_BFA_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_basic_frame_assembly_active_table_get(int unit, int port, uint8* table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));
    field_buf = soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val, BFA_MAP_TAB_IDf);

    *table = field_buf;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_assembly_table_num_entries_set(int unit, int port, uint8 table, uint32 num_entries)
{
    uint32 reg_val;
    uint32 field_buf;
    uint32 active_table;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));

    active_table =  soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val, BFA_MAP_TAB_IDf);
    /*
     * Check to see if it is trying to change number of entries to active table.
     */

    if (active_table == table) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,(_SOC_MSG("cannot change number of entries of active table.")));
    }

    field_buf = num_entries;
    if (table == 0) {
        soc_reg_field_set(unit, CPRI_TXFRM_BFA_CTRLr, &reg_val,
                          BFA_NUM_VLD_ENTRIES_TAB0f, field_buf);
    } else {
        soc_reg_field_set(unit, CPRI_TXFRM_BFA_CTRLr, &reg_val,
                          BFA_NUM_VLD_ENTRIES_TAB1f, field_buf);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_TXFRM_BFA_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_assembly_table_num_entries_get(int unit, int port, uint8 table, uint32* num_entries)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));

    if (table == 0) {
        field_buf = soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB0f);
    } else {
        field_buf = soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB1f);
    }
    *num_entries = field_buf;
exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_entry_set(int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     uint8 index,
                                     const cprif_bfa_bfp_table_entry_t *entry)
{
    uint32 *wr_data_ptr;
    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 field_buf;
    uint32 num_words;


    soc_mem_t tx_mem_select[] = { CPRI_BFA_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFA_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFA_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFA_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFA_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFA_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFA_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFA_TAB1_CFG_3m }; /* port_3, table 1 */
    soc_mem_t rx_mem_select[] = { CPRI_BFP_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFP_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFP_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFP_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFP_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFP_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFP_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFP_TAB1_CFG_3m }; /* port_3, table 1 */

    SOC_INIT_FUNC_DEFS;
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[4*table + port_core_index];
    } else {
        local_mem = rx_mem_select[4*table + port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "cprimod");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->axc_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, AXCC_IDf, &field_buf);

    /*
     * Convert the bits into count as basic frame tables store the count.
     */
    field_buf = entry->rsrv_bit_cnt/2;    /* NB: this is 'cnt', not 'bits' */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RSRV_CNTf, &field_buf);

    /*
     * Convert the bits into count.
     */
    field_buf = entry->data_bit_cnt/2;    /* NB: this is 'cnt', not 'bits' */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, DATA_CNTf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}



int cprif_drv_basic_frame_entry_get(int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     uint8 index,
                                     cprif_bfa_bfp_table_entry_t *entry)
{

    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;

    soc_mem_t tx_mem_select[] = { CPRI_BFA_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFA_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFA_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFA_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFA_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFA_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFA_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFA_TAB1_CFG_3m }; /* port_3, table 1 */

    soc_mem_t rx_mem_select[] = { CPRI_BFP_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFP_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFP_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFP_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFP_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFP_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFP_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFP_TAB1_CFG_3m }; /* port_3, table 1 */
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[4*table + port_core_index];
    } else {
        local_mem = rx_mem_select[4*table + port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));
    soc_mem_field_get(unit, local_mem, rd_data_ptr, AXCC_IDf, &field_buf);
    entry->axc_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, RSRV_CNTf, &field_buf);

    /*
     * Convert the count into bits as basic frame table store the count.
     */
    entry->rsrv_bit_cnt = field_buf*2;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, DATA_CNTf, &field_buf);

    /*
     * Convert the count into bits.
     */
    entry->data_bit_cnt = field_buf*2;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_basic_frame_usage_entry_add(int unit, int port,
                                           uint8 table,
                                           cprimod_direction_t dir,
                                           cprimod_basic_frame_usage_entry_t new_entry,
                                           cprimod_basic_frame_usage_entry_t *user_table,
                                           int num_entries_in_table,
                                           int *num_entries )
{
    int table_index;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint16 current_bit_location;
    int table_end;
    int entry_added=0;
    int user_table_index;
    int last_axc_id=100;  /* valix id 0-63 and 255(0xff) */
    current_bit_location = 0;
    user_table_index = 0;
    table_end = FALSE;
    entry_added = FALSE;

    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                           table, dir,
                                           table_index,
                                           &table_entry));

        /*
         * reserve bits need to be count first.
         */

        if (table_entry.rsrv_bit_cnt != 0) {
            current_bit_location += table_entry.rsrv_bit_cnt;
        }

        /* if data count is non-zero, it is an active entry.*/
        if (table_entry.data_bit_cnt != 0) {

            if ((new_entry.start_bit <  current_bit_location) &&
                (!entry_added)) {
                if (user_table_index < num_entries_in_table) {
                    user_table[user_table_index].start_bit = new_entry.start_bit;
                    user_table[user_table_index].num_bits  = new_entry.num_bits;
                    user_table[user_table_index].axc_id    = new_entry.axc_id;
                    user_table_index++;
                }
                entry_added = TRUE;
            }

            /*
             * Add existing entry.
             */

            if (user_table_index < num_entries_in_table) {
                user_table[user_table_index].start_bit = current_bit_location;
                user_table[user_table_index].num_bits  = table_entry.data_bit_cnt;
                user_table[user_table_index].axc_id    = table_entry.axc_id;
                current_bit_location += table_entry.data_bit_cnt;
                user_table_index++;
            }
        }

        /*
         * if both count are 0, then it is end of the table.
         */
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.axc_id == last_axc_id)) {
            table_end = TRUE;
        }
        last_axc_id = table_entry.axc_id;
    } /* for loop table_index */

    if (!entry_added) {
        user_table[user_table_index].start_bit = new_entry.start_bit;
        user_table[user_table_index].num_bits = new_entry.num_bits;
        user_table[user_table_index].axc_id    = new_entry.axc_id;
        user_table_index++;
        entry_added = TRUE;
    }
    *num_entries = user_table_index;

    return SOC_E_NONE;
}

int cprif_drv_basic_frame_usage_entry_delete(int unit, int port,
                                              uint8 table,
                                              cprimod_direction_t dir,
                                              uint8 axc_id,
                                              cprimod_basic_frame_usage_entry_t *user_table,
                                              int num_entries_in_table,
                                              int *num_entries)
{
    int table_index;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint16 current_bit_location;
    int table_end;
    int user_table_index;

    current_bit_location = 0;
    user_table_index = 0;
    table_end = FALSE;

    for (table_index=0; (table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                           table, dir,
                                           table_index,
                                           &table_entry));

        if (table_entry.rsrv_bit_cnt != 0) {
            current_bit_location += table_entry.rsrv_bit_cnt;
        }

        /* if data count is non-zero, it is an active entry.*/
        if (table_entry.data_bit_cnt != 0) {
            if ((table_entry.axc_id != axc_id)&&
               (user_table_index < num_entries_in_table)) {
                /*
                 * If the AxC id is not matched, then added to the user table.
                 */
                user_table[user_table_index].start_bit = current_bit_location;
                user_table[user_table_index].num_bits  = table_entry.data_bit_cnt;
                user_table[user_table_index].axc_id    = table_entry.axc_id;
                user_table_index++;
            }
            /*
             *  Need to count bit position regardless of deleting entry or not.
             */
            current_bit_location += table_entry.data_bit_cnt;
        }

        /* if both count are 0, then it is end of the table. */
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    } /* for loop table_index */

    *num_entries = user_table_index;

    return SOC_E_NONE;
}

int cprif_drv_basic_frame_usage_entries_get(int unit, int port,
                                             uint8 table,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_basic_frame_usage_entry_t *user_table,
                                             int num_entries_in_table,
                                             int *num_entries )
{
    int table_index;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint16 current_bit_location;
    int table_end;
    int user_table_index;

    current_bit_location = 0;
    user_table_index = 0;
    table_end = FALSE;

    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                           table, dir,
                                           table_index,
                                           &table_entry));

        if (table_entry.rsrv_bit_cnt != 0) {
            current_bit_location += table_entry.rsrv_bit_cnt;
        }

        /* if data count is non-zero, it is an active entry.*/
        if (table_entry.data_bit_cnt != 0) {
            /* If the AxC is match or looking for all entries  and still have table space. */
            if (((table_entry.axc_id == axc_id)||
                (CPRIMOD_AXC_ID_ALL == axc_id)) &&
                (user_table_index < num_entries_in_table)) {
                /*
                 * If the AxC id is matching, then added to the user table.
                 */
                user_table[user_table_index].start_bit = current_bit_location;
                user_table[user_table_index].num_bits  = table_entry.data_bit_cnt;
                user_table[user_table_index].axc_id    = table_entry.axc_id;
                user_table_index++;
            }
            current_bit_location += table_entry.data_bit_cnt;
        }


        /* if both count are 0, then it is end of the table. */
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    } /* for loop table_index */

    *num_entries = user_table_index;

    return SOC_E_NONE;
}

int cprif_drv_basic_frame_usage_table_to_hw_table_set(int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     int basic_frm_len,
                                     cprimod_basic_frame_usage_entry_t *user_table,
                                     int num_entries )
{

    uint16 current_bit_location = 0;
    uint16 reserve_bit_cnt;
    uint16 data_bit_cnt;
    uint16 current_index = 0;
    cprif_bfa_bfp_table_entry_t table_entry;
    int table_index;
    SOC_INIT_FUNC_DEFS;

    cprif_bfa_bfp_table_entry_t_init(&table_entry);
    /*
     * Check for Overlap first.
     */

    if (num_entries > 1) {
        for (table_index=0; (table_index < (num_entries-1)) ; table_index++) {
            if ((user_table[table_index].start_bit+user_table[table_index].num_bits) >
                 user_table[table_index+1].start_bit) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Overlapping AxC Entries"));
            }
        }
    }

    current_index = 0;
    current_bit_location = 0;

    for (table_index=0; table_index < num_entries; table_index++) {

        if (current_bit_location < user_table[table_index].start_bit) {
            /*
             * if current bit position is less than start bit, need to add rsrv bits.
             */
            reserve_bit_cnt = user_table[table_index].start_bit - current_bit_location ;
            current_bit_location += reserve_bit_cnt;
            table_entry.axc_id = user_table[table_index].axc_id;
            table_entry.data_bit_cnt = 0;

            /* fill reserve bits first. */
            while (reserve_bit_cnt) {
                if (reserve_bit_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                    table_entry.rsrv_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY ;
                } else {
                    table_entry.rsrv_bit_cnt = reserve_bit_cnt;
                }

                reserve_bit_cnt -= table_entry.rsrv_bit_cnt;
                /* Do not write the last entry, will combine with data below. */
                if (reserve_bit_cnt != 0) {
                    _SOC_IF_ERR_EXIT
                        (cprif_drv_basic_frame_entry_set(unit, port,
                                                         table, dir,
                                                         current_index,
                                                         &table_entry));
                    current_index++;
                }
            }
        }
        /*
         * Fill data bits.
         */
        data_bit_cnt = user_table[table_index].num_bits ;
        current_bit_location += data_bit_cnt;
        table_entry.axc_id = user_table[table_index].axc_id;
        if (data_bit_cnt == 0) {
            _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     table, dir,
                                                     current_index,
                                                     &table_entry));
            current_index++;
        } else {
            while (data_bit_cnt) {
                if (data_bit_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                    table_entry.data_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY;
                } else {
                    table_entry.data_bit_cnt = data_bit_cnt;
                }

                data_bit_cnt -= table_entry.data_bit_cnt;
                _SOC_IF_ERR_EXIT
                        (cprif_drv_basic_frame_entry_set(unit, port,
                                                         table, dir,
                                                         current_index,
                                                         &table_entry));
                table_entry.rsrv_bit_cnt = 0;
                current_index++;
            }
        }
    }
    /* Fill the remaning bits with reserved */
    if(current_bit_location < basic_frm_len) {
        reserve_bit_cnt = basic_frm_len-current_bit_location;
        table_entry.data_bit_cnt = 0;
        while (reserve_bit_cnt) {
            if (reserve_bit_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                table_entry.rsrv_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY ;
            } else {
                table_entry.rsrv_bit_cnt = reserve_bit_cnt;
            }

            reserve_bit_cnt -= table_entry.rsrv_bit_cnt;
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_entry_set(unit, port,
                                                 table, dir,
                                                 current_index,
                                                 &table_entry));
            current_index++;

        }
    }

exit:
    SOC_FUNC_RETURN;
}


#define CPRIF_DIAG_OUT(str)  LOG_CLI(str);

int _cprif_print_usage_table (cprimod_basic_frame_usage_entry_t* usage_table,
                                int num_entries )
{
    int index;
    cprimod_basic_frame_usage_entry_t* entry;

    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | AxC ID  |  START_BIT  |  NUM BITS         |\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));

    for (index = 0; index < num_entries ; index++) {
        entry = &usage_table[index];
        CPRIF_DIAG_OUT(("    |   %3d   |     %3d     |      %3d          |\n", entry->axc_id,entry->start_bit, entry->num_bits));
        CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    }
    return SOC_E_NONE;
}

int cprif_drv_basic_frame_debug_usage(int unit, int port, uint32 axc_id, uint8 rx_table, uint8 tx_table)
{
    cprimod_basic_frame_usage_entry_t* tx_usage_table = NULL;
    cprimod_basic_frame_usage_entry_t* rx_usage_table = NULL;
    int tx_num_entries;
    int rx_num_entries;

    SOC_INIT_FUNC_DEFS;


    tx_usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    if (tx_usage_table == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }

    rx_usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    if (rx_usage_table == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }


    tx_num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;
    rx_num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;


    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 rx_table,
                                                 cprimod_dir_rx,
                                                 axc_id,
                                                 rx_usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &rx_num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 tx_table,
                                                 cprimod_dir_tx,
                                                 axc_id,
                                                 tx_usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &tx_num_entries));

    CPRIF_DIAG_OUT(("Rx Basic Frame Table...\n"));
    _cprif_print_usage_table(rx_usage_table, rx_num_entries);
    CPRIF_DIAG_OUT(("Tx Basic Frame Table...\n"));
    _cprif_print_usage_table(tx_usage_table, tx_num_entries);

exit:
    if (tx_usage_table) {
        sal_free(tx_usage_table);
    }
    if (rx_usage_table) {
        sal_free(rx_usage_table);
    }
    SOC_FUNC_RETURN;
}


int cprif_drv_basic_frame_debug_raw(int unit, int port, uint32 axc_id, uint8 rx_table, uint8 tx_table)
{
    cprif_bfa_bfp_table_entry_t entry;
    int table_index;
    uint8 table_end = 0;

    SOC_INIT_FUNC_DEFS;

    CPRIF_DIAG_OUT(("Rx Basic Frame HW Table...\n"));

    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | AxC ID  |   RSRV_BIT  |    DATA BITS      |\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));

    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {

        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                             rx_table, cprimod_dir_rx,
                                           table_index,
                                           &entry));
        if ((entry.axc_id == axc_id) ||
             (CPRIMOD_AXC_ID_ALL == axc_id)) {
            CPRIF_DIAG_OUT(("    |   %3d   |     %3d     |      %3d          |\n",
                             entry.axc_id, entry.rsrv_bit_cnt, entry.data_bit_cnt));
            CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
        }
        /* if both count are 0, then it is end of the table. */
        if ((entry.data_bit_cnt == 0) &&
            (entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    }

    CPRIF_DIAG_OUT(("Tx Basic Frame HW Table...\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | AxC ID  |   RSRV_BIT  |    DATA BITS      |\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));

    table_end = 0;
    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {

        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                             tx_table, cprimod_dir_tx,
                                             table_index,
                                             &entry));
        if ((entry.axc_id == axc_id) ||
             (CPRIMOD_AXC_ID_ALL == axc_id)) {
            CPRIF_DIAG_OUT(("    |   %3d   |     %3d     |      %3d          |\n",
                             entry.axc_id, entry.rsrv_bit_cnt, entry.data_bit_cnt));
            CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
        }
        /* if both count are 0, then it is end of the table. */
        if ((entry.data_bit_cnt == 0) &&
            (entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_container_entry_set(int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      const cprif_cpri_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->map_method;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MAP_METHODf, &field_buf);

    field_buf = entry->axc_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, AXC_IDf, &field_buf);

    field_buf = entry->stuffing_at_end;
    if (local_mem  == tx_mem_select[port_core_index]) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFFING_AT_ENDf, &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
    } 

    if (entry->map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_1) {
        field_buf = entry->stuff_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFF_CNTf, &field_buf);
    } else if (entry->map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3) {
        /* stuffing conut for method #3 has quotient and remainder. */
        field_buf = entry->stuff_cnt;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFF_CNTf, &field_buf);
    }


    field_buf = entry->cblk_cnt/2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, CBLK_CNTf, &field_buf);

    field_buf = entry->naxc_cnt/2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NAXC_CNTf, &field_buf);

    field_buf = entry->Nv;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NVf, &field_buf);

    field_buf = entry->Na;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NAf, &field_buf);

    field_buf = entry->rfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_SYNCf, &field_buf);

    field_buf = entry->hfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFRM_SYNCf, &field_buf);

    field_buf = entry->bfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BFN_OFFSETf, &field_buf);

    field_buf = entry->hfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_OFFSETf, &field_buf);

    field_buf = entry->bfrm_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BFRM_OFFSETf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_cpri_container_entry_get(int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      cprif_cpri_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MAP_METHODf, &field_buf);
    entry->map_method = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, AXCC_IDf, &field_buf);
    entry->axc_id = field_buf;

    if (local_mem  == tx_mem_select[port_core_index]) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, STUFFING_AT_ENDf, &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
    } 
    entry->stuffing_at_end = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, STUFF_CNTf, &field_buf);
    if (entry->map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3) {
        entry->stuff_cnt = field_buf;
    } else {
        entry->stuff_cnt = field_buf * 2;
    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr,CBLK_CNTf , &field_buf);
    entry->cblk_cnt = field_buf * 2;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NAXC_CNTf , &field_buf);
    entry->naxc_cnt = field_buf * 2;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NVf, &field_buf);
    entry->Nv = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NAf, &field_buf);
    entry->Na= field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, RFRM_SYNCf, &field_buf);
    entry->rfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFRM_SYNCf, &field_buf);
    entry->hfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BFN_OFFSETf, &field_buf);
    entry->bfn_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_OFFSETf, &field_buf);
    entry->hfn_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BFRM_OFFSETf, &field_buf);
    entry->bfrm_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_container_entry_set(int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      const cprif_rsvd4_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);


    field_buf = entry->map_method;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MAP_METHODf, &field_buf);

    field_buf = entry->axc_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, AXC_IDf, &field_buf);

    field_buf = entry->stuffing_at_end;
    if (local_mem  == tx_mem_select[port_core_index]) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFFING_AT_ENDf, &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
    } 

    field_buf = entry->stuff_cnt/2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_0f, &field_buf);

    if (dir == cprimod_dir_tx) {
        field_buf = entry->cblk_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_1f, &field_buf);

        field_buf = entry->rfrm_offset & 0xFFFFF; /* lower 20 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_OFFSET_LOWf, &field_buf);

        field_buf = (entry->rfrm_offset >> 20) & 0xFF; /* middle 8 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_8f, &field_buf);

        field_buf = (entry->rfrm_offset >> 28) & 0xF; /* upper 4 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_11f, &field_buf);

        field_buf = entry->use_ts_dbm;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_2f, &field_buf);

        field_buf = entry->ts_dbm_prof_num;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_3f, &field_buf);

        field_buf = entry->num_active_slots;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_4f, &field_buf);

        field_buf = entry->msg_addr;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_5f, &field_buf);

        field_buf = entry->msg_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_7f, &field_buf);

        field_buf = entry->msg_ts_offset;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_10f, &field_buf);

    } else {
        field_buf = entry->cblk_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, CBLK_CNTf, &field_buf);

        field_buf = entry->rfrm_offset & 0xFFFFF; /* lower 20 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_2f, &field_buf);

        field_buf = (entry->rfrm_offset >> 20) & 0xFFF; /* upper 12 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_3f, &field_buf);

        field_buf = entry->msg_ts_sync;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_5f, &field_buf);

    }

    field_buf = entry->msg_ts_cnt;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_9f, &field_buf);

    field_buf = entry->msg_ts_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_6f, &field_buf);


    field_buf = entry->hfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFRM_SYNCf, &field_buf);

    field_buf = entry->rfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_SYNCf, &field_buf);


    field_buf = entry->bfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BFN_OFFSETf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_container_entry_get(int unit, int port,
                                         cprimod_direction_t dir,
                                         uint8 index,
                                         cprif_rsvd4_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MAP_METHODf, &field_buf);
    entry->map_method = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, AXC_IDf, &field_buf);
    entry->axc_id = field_buf;


    if (local_mem  == tx_mem_select[port_core_index]) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, STUFFING_AT_ENDf, &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
    } 
    entry->stuffing_at_end  = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_0f, &field_buf);
    entry->stuff_cnt = field_buf * 2;

    if (dir == cprimod_dir_tx) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_1f, &field_buf);
        entry->cblk_cnt = field_buf * 2;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, RFRM_OFFSET_LOWf, &field_buf);
        entry->rfrm_offset = field_buf & 0xFFFFF;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_8f, &field_buf);
        entry->rfrm_offset |= ((field_buf & 0xFF) << 20);

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_11f, &field_buf);
        entry->rfrm_offset |= ((field_buf & 0xF) << 28);



        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_2f, &field_buf);
        entry->use_ts_dbm = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_3f, &field_buf);
        entry->ts_dbm_prof_num = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_4f, &field_buf);
        entry->num_active_slots = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_5f, &field_buf);
        entry->msg_addr = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_7f, &field_buf);
        entry->msg_type = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_10f, &field_buf);
        entry->msg_ts_offset = field_buf;

    } else {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, CBLK_CNTf, &field_buf);
        entry->cblk_cnt = field_buf * 2;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_2f, &field_buf);
        entry->rfrm_offset = field_buf & 0xFFFFF;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_3f, &field_buf);
        entry->rfrm_offset |= ((field_buf & 0xFFF) << 20);


        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_5f, &field_buf);
        entry->msg_ts_sync = field_buf;

    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_9f, &field_buf);
    entry->msg_ts_cnt = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_6f, &field_buf);
    entry->msg_ts_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFRM_SYNCf, &field_buf);
    entry->hfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, RFRM_SYNCf, &field_buf);
    entry->rfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BFN_OFFSETf, &field_buf);
    entry->bfn_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_container_map_print_cpri_entry (cprif_cpri_container_map_entry_t* entry)
{

    CPRIF_DIAG_OUT(("  0x%03x  0x%03x 0x%03x    0x%03x  0x%03x 0x%03x     0x%03x  0x%03x 0x%03x    0x%03x  0x%03x 0x%03x  0x%03x \n",
                            entry->map_method,
                            entry->axc_id,
                            entry->stuffing_at_end,
                            entry->stuff_cnt,
                            entry->cblk_cnt,
                            entry->naxc_cnt,
                            entry->Nv,
                            entry->Na,
                            entry->rfrm_sync,
                            entry->hfrm_sync,
                            entry->bfn_offset,
                            entry->hfn_offset,
                            entry->bfrm_offset
                            ));
    return SOC_E_NONE;
}

int cprif_container_map_print_rsvd4_entry (cprif_rsvd4_container_map_entry_t* entry)
{

    CPRIF_DIAG_OUT(("  0x%03x  0x%03x 0x%03x    0x%03x  0x%03x  0x%01x x0x%01x 0x%01x  0x%03x   0x%03x  0x%03x 0x%03x   0x%03x  0x%03x  0x%03x   0x%03x  0x%03x 0x%03x  0x%03x \n",
                            entry->map_method,
                            entry->axc_id,
                            entry->stuffing_at_end,
                            entry->stuff_cnt,
                            entry->cblk_cnt,
                            entry->rfrm_sync,
                            entry->hfrm_sync,
                            entry->msg_ts_sync,
                            entry->bfn_offset,
                            entry->rfrm_offset,
                            entry->bfn_offset,
                            entry->msg_ts_mode,
                            entry->msg_ts_cnt,
                            entry->use_ts_dbm,
                            entry->ts_dbm_prof_num,
                            entry->num_active_slots,
                            entry->msg_addr,
                            entry->msg_type,
                            entry->msg_ts_offset
                            ));
    return SOC_E_NONE;
}

int cprif_container_map_debug(int unit, int port, uint8 axc_id)
{
    cprif_cpri_container_map_entry_t cpri_entry;
    cprif_rsvd4_container_map_entry_t rsvd4_entry;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_container_entry_get(unit, port,
                                                      cprimod_dir_rx,
                                                      axc_id,
                                                      &cpri_entry));

    if (cpri_entry.map_method == CPRIF_CONTAINER_MAP_RSVD4) {

    CPRIF_DIAG_OUT(("    +------------------------------------------------------------------------------------------------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | Map M | AxC | S END | S CNT | CBLK CNT | SYNC rhm | BFN | RFRM | TS_MODE | TS CNT | DBM EN | Prof | ASlot | Addr | Type | TS Offset|\n"));
    CPRIF_DIAG_OUT(("    +------------------------------------------------------------------------------------------------------------------------------------+\n"));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_container_entry_get(unit, port,
                                              cprimod_dir_rx,
                                              axc_id,
                                              &rsvd4_entry));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_container_entry_get(unit, port,
                                              cprimod_dir_tx,
                                              axc_id,
                                              &rsvd4_entry));

    } else {

    CPRIF_DIAG_OUT(("    +----------------------------------------------------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | Map M | AxC | S END | S CNT | CBLK CNT | Naxc | Nv | Na | SYNC rh | BFN | HFN  |  BFRM |\n"));
    CPRIF_DIAG_OUT(("    +---------------------------------------------------------------------------------------+\n"));


        _SOC_IF_ERR_EXIT
            (cprif_drv_cpri_container_entry_get(unit, port,
                                              cprimod_dir_tx,
                                              axc_id,
                                              &cpri_entry));


    }

exit:
    SOC_FUNC_RETURN;
}


/*
 *
 *  IQ Packing/Unpacking
 *
 *
 */
int cprif_drv_iq_buffer_config_set(int unit, int port,
                                    cprimod_direction_t dir,
                                    uint8 index,
                                    cprif_iq_buffer_config_t* entry)
{
    uint32 *wr_data_ptr;
    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 field_buf;
    uint32 num_words;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_IQ_PK_BUFF_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_3m }; /* slice#3 */
    soc_mem_t tx_mem_select[] = { CPRI_CA_PAY_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CA_PAY_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CA_PAY_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CA_PAY_TAB_CFG_3m }; /* slice#3 */
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Convert the bits into number of sample pair.
     */
    field_buf = entry->payload_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, PLD_SIZEf, &field_buf);

    field_buf = entry->last_packet_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NTH_PLD_IDf, &field_buf);

    /*
     * Convert the bits into number of sample pair.
     */
    field_buf = entry->last_payload_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NTH_PLD_SIZEf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_buffer_config_get(int unit, int port,
                                    cprimod_direction_t dir,
                                    uint8 index,
                                    cprif_iq_buffer_config_t* entry)
{
    uint32 *rd_data_ptr;
    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 field_buf;
    uint32 num_words;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_IQ_PK_BUFF_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_3m }; /* slice#3 */
    soc_mem_t tx_mem_select[] = { CPRI_CA_PAY_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CA_PAY_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CA_PAY_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CA_PAY_TAB_CFG_3m }; /* slice#3 */
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, PLD_SIZEf, &field_buf);
    entry->payload_size  = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NTH_PLD_IDf, &field_buf);
    entry->last_packet_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NTH_PLD_SIZEf, &field_buf);
    entry->last_payload_size = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_iq_buffer_debug(int unit, int port, uint8 axc_id)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    cprif_iq_buffer_config_t_init(&entry);

    CPRIF_DIAG_OUT(("    +---------------------------------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    |  AxC      | Payload Size  | Last_Pkt_Num | Last Payload Size        |\n"));
    CPRIF_DIAG_OUT(("    +---------------------------------------------------------------------+\n"));

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                cprimod_dir_rx,
                                                axc_id,
                                                &entry));

    CPRIF_DIAG_OUT(("  Rx 0x%03x 0x%03x  0x%03x 0x%03x\n", axc_id, entry.payload_size,entry.last_packet_num, entry.last_payload_size));

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                cprimod_dir_tx,
                                                axc_id,
                                                &entry));

    CPRIF_DIAG_OUT(("  Tx 0x%03x 0x%03x  0x%03x 0x%03x\n", axc_id, entry.payload_size,entry.last_packet_num, entry.last_payload_size));
exit:
    SOC_FUNC_RETURN;
}


/* this is for cprif_drv_decap_data_entry_set as well */
int cprif_drv_encap_data_entry_set(int unit, int port,
                                    cprimod_direction_t dir,
                                    uint32 queue_num,
                                    const cprif_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, wr_data_ptr));

    field_buf = entry->sample_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SAMPLE_SIZEf, &field_buf);

    if (dir == cprimod_dir_rx) {

        field_buf = entry->valid;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_VALIDf, &field_buf);

        field_buf = entry->mux_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_DEMUX_ENf, &field_buf);

        field_buf = entry->queue_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIZEf, &field_buf);

        field_buf = entry->work_queue_select;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_WQ_SELf, &field_buf);

    } else {
        field_buf = entry->mux_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_MUX_ENf, &field_buf);

        field_buf = entry->queue_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BUF_SIZEf, &field_buf);

        field_buf = entry->tx_cycle_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_CYC_SIZEf, &field_buf);


    }

    field_buf = entry->out_sample_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);


    field_buf = entry->queue_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OFFSETf, &field_buf);


    field_buf = entry->sign_ext_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXDf, &field_buf);

    field_buf = entry->sign_ext_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXD_TYPEf, &field_buf);

    field_buf = entry->bit_reversal;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BIT_REVERSALf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_encap_data_entry_get(int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 cprif_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SAMPLE_SIZEf, &field_buf);
    entry->sample_size = field_buf;

    if (dir == cprimod_dir_rx) {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_VALIDf, &field_buf);
        entry->valid = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_DEMUX_ENf, &field_buf);
        entry->mux_enable = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIZEf, &field_buf);
        entry->queue_size = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_WQ_SELf, &field_buf);
        entry->work_queue_select = field_buf;
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_MUX_ENf, &field_buf);
        entry->mux_enable = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_BUF_SIZEf, &field_buf);
        entry->queue_size = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_CYC_SIZEf, &field_buf);
        entry->tx_cycle_size = field_buf;

    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);
    entry->out_sample_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_OFFSETf, &field_buf);
    entry->queue_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIGN_EXDf, &field_buf);
    entry->sign_ext_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIGN_EXD_TYPEf, &field_buf);
    entry->sign_ext_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_BIT_REVERSALf, &field_buf);
    entry->bit_reversal= field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}

/* for both encap and decap allocation can use this routine */
int cprif_drv_encap_data_allocate_buffer(int unit, int port,
                                         cprimod_direction_t dir,
                                         uint16 size_in_bytes,
                                         uint16* offset)
{

    cprif_encap_decap_data_entry_t entry;
    uint32 *usage_map = NULL;
    uint8 queue;
    uint16 total_buf;
    uint16 num_words;
    uint16 num_entries;
    uint16 index;
    uint16 first_location;
    uint16 queue_entry_end_loc;
    uint8  location_found;
    int rv = SOC_E_NONE;


    if (dir == cprimod_dir_rx) {
        total_buf = CPRIF_ENCAP_BUFFER_NUM_ENTRIES;
    } else {
        total_buf = CPRIF_DECAP_BUFFER_NUM_ENTRIES;
    }
    /*
     * Allocate the memory to keep track of buffer usage.
     * Fill every location with 0 to mark as available.
     */
    num_words = CPRIF_NUM_PBMP_WORDS(total_buf);
    usage_map = (uint32*)sal_alloc((sizeof(uint32) * num_words), "cprif_drv");
    CPRIMOD_NULL_CHECK(usage_map);
    sal_memset(usage_map, 0, sizeof(uint32) * num_words);
    /*
     * Marked all the buffer entries that are allocated to the queue.
     */
    for (queue = 0; queue < CPRIF_MAX_NUM_OF_QUEUES; queue++) {
        cprif_drv_encap_data_entry_get(unit, port, dir, queue, &entry);
        if (entry.queue_size != 0) {
            if(dir == cprimod_dir_tx) {
                queue_entry_end_loc = entry.queue_offset+(entry.queue_size
                                      * ((uint16)entry.tx_cycle_size));
            } else {
                queue_entry_end_loc = entry.queue_offset+entry.queue_size;
            }
            for (index = entry.queue_offset; index < queue_entry_end_loc; index++) {
                CPRIF_PBMP_IDX_MARK(usage_map,index);
            }
        }
    }
    /*
     * Find the buffer location that can accomodate num_entries.
     */
    index = 0;
    first_location = 0;
    location_found = FALSE;
    num_entries =  CPRIF_NUM_OF_BUFFER_ENTRIES(size_in_bytes);

    while (num_entries && (index < total_buf)) {
        num_entries =  CPRIF_NUM_OF_BUFFER_ENTRIES(size_in_bytes);
        /*
         * Find first available spot.
         */
        for ( ;index < total_buf; index++) {
            if (CPRIF_PBMP_AVAILABLE(usage_map, index)) {
                first_location = index;
                location_found  = TRUE;
                break;
            }
        } /* for */
        /*
         * Check that spot has enough available space.
         */
        if (location_found) {
            for ( ;(index < total_buf) && (num_entries > 0) ; index++) {
                if (CPRIF_PBMP_AVAILABLE(usage_map, index)){
                    num_entries--;
                } else {
                    location_found = FALSE;
                    break;
                }
            } /* for */
        }
    } /* while */

    if (num_entries == 0) {
        *offset = first_location;
    } else {
        rv = SOC_E_UNAVAIL;
    }
    if (usage_map) {
        sal_free(usage_map);
    }
    return rv;
}

int cprif_drv_rsvd4_encap_data_entry_set(int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 const cprif_rsvd4_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, wr_data_ptr));

    if (dir == cprimod_dir_rx) {

        field_buf = entry->valid;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_VALIDf, &field_buf);

        field_buf = 0;  /* mux/demux is always off. */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_DEMUX_ENf, &field_buf);

        field_buf = (entry->queue_size+CPRIF_DATA_BUFFER_BLOCK_SIZE-1)/CPRIF_DATA_BUFFER_BLOCK_SIZE;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIZEf, &field_buf);

        field_buf = entry->work_queue_select;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_WQ_SELf, &field_buf);

        field_buf = entry->gsm_control_location;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_3f, &field_buf);

        field_buf = 0;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXDf, &field_buf);
    } else {
        field_buf = 0; /* Mux Demux is off. */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_MUX_ENf, &field_buf);

        field_buf = ((entry->queue_size+CPRIF_DATA_BUFFER_BLOCK_SIZE-1)/CPRIF_DATA_BUFFER_BLOCK_SIZE)+1;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BUF_SIZEf, &field_buf);

        field_buf = entry->tx_cycle_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_CYC_SIZEf, &field_buf);

        if(entry->gsm_pad_enable != 0){
          field_buf = entry->gsm_control_location;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_3f, &field_buf);
        }else {
          field_buf = 0;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXDf, &field_buf);
        }
    }

    field_buf = entry->gsm_pad_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_2f, &field_buf);

    field_buf = entry->queue_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OFFSETf, &field_buf);


    field_buf = 0;  /* Sign extension is off */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXD_TYPEf, &field_buf);

    field_buf = 1; /* bit reversal is off */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BIT_REVERSALf, &field_buf);

    if(queue_num >= CPRIF_MAX_NUM_OF_DATA_QUEUES) {
        field_buf = 16;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SAMPLE_SIZEf, &field_buf);

        field_buf = 16;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);
    } else {
        if(entry->gsm_pad_enable != 0){
          field_buf = entry->gsm_pad_size;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_0f, &field_buf);
        } else {
          field_buf = 8;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SAMPLE_SIZEf, &field_buf);
        }

        if(entry->gsm_pad_enable != 0){
          field_buf = entry->gsm_extra_pad_size;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_1f, &field_buf);
        } else {
          field_buf = 8;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);
        }  
    }

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_encap_data_entry_get(int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 cprif_rsvd4_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == cprimod_dir_tx) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_0f, &field_buf);
    entry->gsm_pad_size = field_buf;

    if (dir == cprimod_dir_rx) {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_VALIDf, &field_buf);
        entry->valid = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIZEf, &field_buf);
        entry->queue_size = field_buf * CPRIF_DATA_BUFFER_BLOCK_SIZE;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_WQ_SELf, &field_buf);
        entry->work_queue_select = field_buf;
    } else {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_BUF_SIZEf, &field_buf);
        entry->queue_size = field_buf * CPRIF_DATA_BUFFER_BLOCK_SIZE;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_CYC_SIZEf, &field_buf);
        entry->tx_cycle_size = field_buf;

    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_1f, &field_buf);
    entry->gsm_extra_pad_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_OFFSETf, &field_buf);
    entry->queue_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_2f, &field_buf);
    entry->gsm_pad_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_3f, &field_buf);
    entry->gsm_control_location = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_encap_header_entry_set(int unit, int port,
                                      uint32 queue_num,
                                      uint32 flags,
                                      const cprif_encap_header_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_ETH_HDR_0m,
                               CPRI_ENCAP_QUE_ETH_HDR_1m,
                               CPRI_ENCAP_QUE_ETH_HDR_2m,
                               CPRI_ENCAP_QUE_ETH_HDR_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, wr_data_ptr));

    if (flags & CPRIF_ENCAP_HEADER_FLAGS_HDR_CONFIG_SET) {
        field_buf = entry->header_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_TYPEf, &field_buf);

        field_buf = entry->roe_flow_id;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_FLOWIDf , &field_buf);

        field_buf = entry->roe_subtype & 0x00000003; /* 2 lsb bits are ver */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VERf , &field_buf);

        field_buf = (entry->roe_subtype & 0x000000fc) >> 2; /* top 6 bits are pkttype */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_PKTTYPEf, &field_buf);

        field_buf = entry->ordering_info_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_ORDERINGINFO_INDEXf, &field_buf);

        field_buf = entry->mac_da_index;;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_DA_IDf, &field_buf);

        field_buf = entry->mac_sa_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_SA_IDf, &field_buf);

        field_buf = entry->vlan_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_TYPEf, &field_buf);

        field_buf = entry->vlan_id_0_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_0f, &field_buf);

        field_buf = entry->vlan_id_1_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_1f, &field_buf);

        field_buf = entry->vlan_0_priority;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_0f, &field_buf);

        field_buf = entry->vlan_1_priority;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_1f, &field_buf);

        field_buf = entry->ether_type_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_ETHER_TYPE_INDXf, &field_buf);

        field_buf = entry->use_tagid_for_vlan;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_VLANf, &field_buf);

        field_buf = entry->use_tagid_for_flowid;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_FLOWIDf, &field_buf);

        field_buf = entry->use_opcode;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_USE_SUBTYPEf, &field_buf);

        if (entry->use_opcode) {
            field_buf = entry->roe_opcode;
            soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_SUBTYPEf, &field_buf);
        }
    }

    if (flags & CPRIF_ENCAP_HEADER_FLAGS_GSM_CONFIG_SET) {
        if(entry->tsn_bitmap != 0){
          field_buf = entry->tsn_bitmap;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_334_1f, &field_buf);
        }
        /* ignore this for now.
        field_buf = entry->gsm_q_cnt_disable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_334_2f, &field_buf);
        */
    }
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_header_entry_get(int unit, int port,
                                      uint32 queue_num,
                                      cprif_encap_header_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_ETH_HDR_0m,
                               CPRI_ENCAP_QUE_ETH_HDR_1m,
                               CPRI_ENCAP_QUE_ETH_HDR_2m,
                               CPRI_ENCAP_QUE_ETH_HDR_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, rd_data_ptr));


    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_TYPEf, &field_buf);
    entry->header_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_FLOWIDf , &field_buf);
    entry->roe_flow_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VERf , &field_buf);
    entry->roe_subtype = (field_buf & 0x3) ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_PKTTYPEf, &field_buf);
    entry->roe_subtype |= (field_buf & 0x3F) << 2 ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_ORDERINGINFO_INDEXf, &field_buf);
    entry->ordering_info_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_DA_IDf, &field_buf);
    entry->mac_da_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_SA_IDf, &field_buf);
    entry->mac_sa_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_TYPEf, &field_buf);
    entry->vlan_type = field_buf;

    field_buf = entry->vlan_id_0_index;
    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_0f, &field_buf);
    entry->vlan_id_0_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_1f, &field_buf);
    entry->vlan_id_1_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_0f, &field_buf);
    entry->vlan_0_priority = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_1f, &field_buf);
    entry->vlan_1_priority = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_ETHER_TYPE_INDXf, &field_buf);
    entry->ether_type_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_VLANf, &field_buf);
    entry->use_tagid_for_vlan = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_FLOWIDf, &field_buf);
    entry->use_tagid_for_flowid = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_USE_SUBTYPEf, &field_buf);
    entry->use_opcode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_SUBTYPEf, &field_buf);
    entry->roe_opcode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_334_1f, &field_buf);
    entry->tsn_bitmap = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_334_2f, &field_buf);
    entry->gsm_q_cnt_disable = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}



int cprif_drv_cprimod_to_cprif_ordering_info_type (cprimod_ordering_info_type_t cprimod_type, uint8* cprif_type)
{
    switch (cprimod_type) {
        case cprimod_ordering_info_type_roe_sequence:
            *cprif_type = CPRIF_ORDERING_INFO_TYPE_ROE;
            break;

        case cprimod_ordering_info_type_bfn_for_qcnt:
            *cprif_type = CPRIF_ORDERING_INFO_TYPE_BFN_FOR_QCNT;
            break;

        case cprimod_ordering_info_type_use_pinfo:
            *cprif_type = CPRIF_ORDERING_INFO_TYPE_USE_PINFO;
            break;
        default:
            break;
    }
    return SOC_E_NONE;

}

int cprif_drv_cprif_to_cprimod_ordering_info_type (uint8 cprif_type , cprimod_ordering_info_type_t *cprimod_type )
{
    switch (cprif_type) {
        case CPRIF_ORDERING_INFO_TYPE_ROE :
            *cprimod_type = cprimod_ordering_info_type_roe_sequence;
            break;

        case CPRIF_ORDERING_INFO_TYPE_BFN_FOR_QCNT :
            *cprimod_type = cprimod_ordering_info_type_bfn_for_qcnt;
            break;

        case CPRIF_ORDERING_INFO_TYPE_USE_PINFO :
            *cprimod_type = cprimod_ordering_info_type_use_pinfo;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

int cprif_drv_cprimod_to_cprif_ordering_info_inc_prop (cprimod_ordering_info_prop_t cprimod_prop,  uint8 *cprif_prop)
{
    switch (cprimod_prop) {
        case cprimod_ordering_info_prop_no_increment:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_NO_INC;
            break;

        case cprimod_ordering_info_prop_increment_by_constant:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_INC_CONST;
            break;

        case cprimod_ordering_info_prop_increment_by_payload:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_INC_BY_PAYLOAD;
            break;

        default:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_INC_CONST;
            break;
    }
    return SOC_E_NONE;
}


int cprif_drv_cprif_to_cprimod_ordering_info_inc_prop (uint8 cprif_prop, cprimod_ordering_info_prop_t  *cprimod_prop)
{
    switch (cprif_prop) {
        case CPRIF_ORDERING_INFO_PROP_NO_INC:
            *cprimod_prop = cprimod_ordering_info_prop_no_increment;
            break;

        case CPRIF_ORDERING_INFO_PROP_INC_CONST:
            *cprimod_prop = cprimod_ordering_info_prop_increment_by_constant;
            break;

        case  CPRIF_ORDERING_INFO_PROP_INC_BY_PAYLOAD:
            *cprimod_prop = cprimod_ordering_info_prop_increment_by_payload;
            break;

        default:
            *cprimod_prop = CPRIF_ORDERING_INFO_PROP_INC_CONST;
            break;
    }
    return SOC_E_NONE;
}


int cprif_drv_encap_vlan_ethtype_set(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16 ethtype)

{
    uint64 reg_val;
    uint64 write_val;
    uint32 local_ethtype;


    switch (ethtype_id) {
        case CPRIF_ETHTYPE_ROE_ETHTYPE_0:
            COMPILER_64_SET(write_val, 0, (uint32) ethtype);
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_CTRLr, &reg_val,
                                ENCAP_ROE_ETHERTYPE_0f, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                            reg_val));
            break;

        case CPRIF_ETHTYPE_ROE_ETHTYPE_1:
            COMPILER_64_SET(write_val, 0, (uint32) ethtype);
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_QINQ_CTRLr,
                           &reg_val, ENCAP_ROE_ETHERTYPE_1f, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, reg_val));

            break;

        case CPRIF_ETHTYPE_VLAN_TAGGED:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            local_ethtype = ethtype;
            local_ethtype = local_ethtype << 16;
            COMPILER_64_SET(write_val, 0, local_ethtype);
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_CTRLr, &reg_val,
                                ENCAP_VLAN_TAGGEDf, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                            reg_val));

            break;

        case CPRIF_ETHTYPE_VLAN_QINQ:
            local_ethtype = ethtype;
            local_ethtype = local_ethtype << 16;
            COMPILER_64_SET(write_val, 0, local_ethtype);
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_QINQ_CTRLr,
                                &reg_val, ENCAP_VLAN_QINQf, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, reg_val));

            break;

        default:
            break;
    }

    return SOC_E_NONE;

}

int cprif_drv_encap_vlan_ethtype_get(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16* ethtype)

{
    uint64 reg_val;
    uint64 read_val;

    switch (ethtype_id) {
        case CPRIF_ETHTYPE_ROE_ETHTYPE_0:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            read_val = soc_reg64_field_get(unit, CPRI_ENCAP_VLAN_CTRLr,
                                        reg_val, ENCAP_ROE_ETHERTYPE_0f);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_ETHTYPE_ROE_ETHTYPE_1:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            read_val = soc_reg64_field_get(unit,
                                    CPRI_ENCAP_VLAN_QINQ_CTRLr, reg_val,
                                    ENCAP_ROE_ETHERTYPE_1f);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_ETHTYPE_VLAN_TAGGED:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            read_val = soc_reg64_field_get(unit, CPRI_ENCAP_VLAN_CTRLr,
                                           reg_val, ENCAP_VLAN_TAGGEDf);
            COMPILER_64_SHR(read_val, 16);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_ETHTYPE_VLAN_QINQ:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            read_val = soc_reg64_field_get(unit,
                                     CPRI_ENCAP_VLAN_QINQ_CTRLr, reg_val,
                                     ENCAP_VLAN_QINQf);
            COMPILER_64_SHR(read_val, 16);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        default:
            break;
    }

    return SOC_E_NONE;

}

int cprif_drv_encap_ordering_info_entry_set(int unit, int port,
                                             uint8 index ,
                                             const cprif_encap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_TAB_0m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_1m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_2m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_3m };

    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Encap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, wr_data_ptr));

    field_buf = entry->type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_TYPEf, &field_buf);

    field_buf = entry->p_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);

    field_buf = entry->q_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);

    field_buf = entry->max;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);

    field_buf = entry->increment;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);

    field_buf = entry->pcnt_prop;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_PCNT_PROPf, &field_buf);

    field_buf = entry->qcnt_prop;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_QCNT_PROPf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_ordering_info_entry_get(int unit, int port,
                                      uint8 index,
                                      cprif_encap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_TAB_0m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_1m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_2m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Encap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }


    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_TYPEf, &field_buf);
    entry->type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);
    entry->p_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);
    entry->q_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);
    entry->max = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);
    entry->increment = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_PCNT_PROPf, &field_buf);
    entry->pcnt_prop = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_QCNT_PROPf, &field_buf);
    entry->qcnt_prop = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_ordering_info_entry_set(int unit, int port,
                                             uint8 index ,
                                            const cprif_decap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_INFO_TAB_0m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_1m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_2m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Decap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, wr_data_ptr));

    field_buf = entry->type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_TYPEf, &field_buf);

    field_buf = entry->p_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);

    field_buf = entry->q_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);

    field_buf = entry->max;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);

    field_buf = entry->increment;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);


    field_buf = entry->pcnt_inc_p2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_P_INC_P2f, &field_buf);

    field_buf = entry->qcnt_inc_p2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_Q_INC_P2f, &field_buf);

    field_buf = entry->pcnt_extended;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_P_EXTENDf, &field_buf);

    field_buf = entry->pcnt_pkt_count;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_P_PKTCNTf, &field_buf);

    field_buf = entry->modulo_2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_MODULO_P2f, &field_buf);

    field_buf = entry->bias;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQINC_BIASf,  &field_buf);

    field_buf = entry->gsm_tsn_bitmap;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_43_0f,  &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_ordering_info_entry_get(int unit, int port,
                                      uint8 index,
                                      cprif_decap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_INFO_TAB_0m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_1m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_2m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Encap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }


    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_TYPEf, &field_buf);
    entry->type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);
    entry->p_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);
    entry->q_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);
    entry->max = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);
    entry->increment = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_P_INC_P2f, &field_buf);
    entry->pcnt_inc_p2 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_Q_INC_P2f, &field_buf);
    entry->qcnt_inc_p2 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_P_EXTENDf, &field_buf);
    entry->pcnt_extended = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_P_PKTCNTf, &field_buf);
    entry->pcnt_pkt_count = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_MODULO_P2f, &field_buf);
    entry->modulo_2 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQINC_BIASf,  &field_buf);
    entry->bias = field_buf;

    soc_mem_field_set(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_43_0f,  &field_buf);
    entry->gsm_tsn_bitmap = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_queue_to_ordering_info_index_set(int unit, int port,
                                                      uint32 queue,
                                                      uint32 index)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_0m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_1m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_2m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_3m };

    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_ORDERING_INFO_INDEXf,  &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_queue_to_ordering_info_index_get(int unit, int port,
                                                     uint32 queue,
                                                     uint32* index)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_0m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_1m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_2m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_ORDERING_INFO_INDEXf, &field_buf);
    *index = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_queue_ordering_info_sequence_offset_set(int unit, int port,
                                                            uint32 queue,
                                                            uint8 control,
                                                            uint32 pq_offset)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_STS_0m,
                               CPRI_ENCAP_ORDERING_INFO_STS_1m,
                               CPRI_ENCAP_ORDERING_INFO_STS_2m,
                               CPRI_ENCAP_ORDERING_INFO_STS_3m };


    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, wr_data_ptr));

    field_buf = pq_offset;
    if (control) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, CUR_SEQ_NUM_CTLf,  &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, CUR_SEQ_NUM_DATAf,  &field_buf);
    }
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_queue_ordering_info_sequence_offset_get(int unit, int port,
                                                            uint32 queue,
                                                            uint8 control,
                                                            uint32* pq_offset)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_STS_0m,
                               CPRI_ENCAP_ORDERING_INFO_STS_1m,
                               CPRI_ENCAP_ORDERING_INFO_STS_2m,
                               CPRI_ENCAP_ORDERING_INFO_STS_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, rd_data_ptr));

    if (control) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, CUR_SEQ_NUM_CTLf,  &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, CUR_SEQ_NUM_DATAf,  &field_buf);
    }

    *pq_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_ordering_info_sequence_offset_set(int unit, int port,
                                                                uint32 queue,
                                                                uint32 pq_offset)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_SEQOFF_TAB_0m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_1m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_2m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, wr_data_ptr));

    field_buf = pq_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQOFFf,  &field_buf);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_queue_ordering_info_sequence_offset_get(int unit, int port,
                                                               uint32 queue,
                                                               uint32* pq_offset)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_SEQOFF_TAB_0m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_1m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_2m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQOFFf,  &field_buf);

    *pq_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_mac_da_set(int unit, int port,
                                 uint32 index,
                                 uint64 mac_addr)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_DA_TAB_0m,
                               CPRI_ENCAP_QUE_DA_TAB_1m,
                               CPRI_ENCAP_QUE_DA_TAB_2m,
                               CPRI_ENCAP_QUE_DA_TAB_3m };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    COMPILER_64_TO_32_LO(local_mac_addr[0],mac_addr);
    COMPILER_64_TO_32_HI(local_mac_addr[1],mac_addr);
    local_mac_addr[1] &= 0xffff;

    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_DAf, local_mac_addr);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_mac_da_get(int unit, int port,
                                uint32 index,
                                uint64 *mac_addr)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_DA_TAB_0m,
                               CPRI_ENCAP_QUE_DA_TAB_1m,
                               CPRI_ENCAP_QUE_DA_TAB_2m,
                               CPRI_ENCAP_QUE_DA_TAB_3m };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));
    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_DAf, local_mac_addr);
    COMPILER_64_SET(*mac_addr,local_mac_addr[1], local_mac_addr[0]);

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;


}
int cprif_drv_encap_mac_sa_set(int unit, int port,
                                 uint32 index,
                                 uint64 mac_addr)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_SA_TAB_0m,
                               CPRI_ENCAP_QUE_SA_TAB_1m,
                               CPRI_ENCAP_QUE_SA_TAB_2m,
                               CPRI_ENCAP_QUE_SA_TAB_3m };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    COMPILER_64_TO_32_LO(local_mac_addr[0],mac_addr);
    COMPILER_64_TO_32_HI(local_mac_addr[1],mac_addr);
    local_mac_addr[1] &= 0xffff;

    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_SAf, local_mac_addr);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_mac_sa_get(int unit, int port,
                                uint32 index,
                                uint64* mac_addr)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_SA_TAB_0m,
                               CPRI_ENCAP_QUE_SA_TAB_1m,
                               CPRI_ENCAP_QUE_SA_TAB_2m,
                               CPRI_ENCAP_QUE_SA_TAB_3m };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_SAf, local_mac_addr);
    COMPILER_64_SET(*mac_addr,local_mac_addr[1], local_mac_addr[0]);
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_vlan_entry_set(int unit, int port,
                                    uint32 vlan_table_num,
                                    uint32 index,
                                    uint32 vlan_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t mem_select_vlan0[] = { CPRI_ENCAP_QUE_VLAN0_TAB_0m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_1m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_2m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_3m };


    soc_mem_t mem_select_vlan1[] = { CPRI_ENCAP_QUE_VLAN0_TAB_0m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_1m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_2m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_3m };
    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (vlan_table_num  == 0) {
        local_mem = mem_select_vlan0[port_core_index];
    } else {
        local_mem = mem_select_vlan1[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = vlan_id;
    if (vlan_table_num == 0) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_VLAN0f, &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_VLAN1f, &field_buf);
    }
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_vlan_entry_get(int unit, int port,
                                    uint32 vlan_table_num,
                                    uint32 index,
                                    uint32* vlan_id)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select_vlan0[] = { CPRI_ENCAP_QUE_VLAN0_TAB_0m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_1m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_2m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_3m };


    soc_mem_t mem_select_vlan1[] = { CPRI_ENCAP_QUE_VLAN0_TAB_0m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_1m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_2m,
                                     CPRI_ENCAP_QUE_VLAN0_TAB_3m };


    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (vlan_table_num == 0) {
        local_mem = mem_select_vlan0[port_core_index];
    } else {
        local_mem = mem_select_vlan1[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    if (vlan_table_num == 0) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_VLAN0f, &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_VLAN1f, &field_buf);
    }

    *vlan_id = field_buf;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_vlan_ethtype_set(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16 ethtype)

{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) ethtype);
    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));

    switch (ethtype_id) {
        case CPRIF_DECAP_ROE_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_ROE_ETHERTYPEf, write_val);
            break;

        case CPRIF_DECAP_FAST_ETH_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_FAST_ETH_ETHERTYPEf, write_val);
            break;

        case CPRIF_DECAP_VLAN_TAGGED_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_VLAN_ETYPEf, write_val);

            break;

        case CPRIF_DECAP_VLAN_QINQ_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_QINQ_ETYPEf, write_val);
            break;

        default:
            break;
    }

    SOC_IF_ERROR_RETURN(WRITE_CPRI_DECAP_CTRLr(unit, port, reg_val));
    return SOC_E_NONE;

}

int cprif_drv_decap_vlan_ethtype_get(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16* ethtype)

{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));
    switch (ethtype_id) {
        case CPRIF_DECAP_ROE_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                           reg_val, DECAP_ROE_ETHERTYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_DECAP_FAST_ETH_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                     reg_val, DECAP_FAST_ETH_ETHERTYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_DECAP_VLAN_TAGGED_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                           reg_val, DECAP_VLAN_ETYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_DECAP_VLAN_QINQ_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                           reg_val, DECAP_QINQ_ETYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        default:
            break;
    }
    return SOC_E_NONE;
}

int cprif_drv_decap_pkt_type_config_set(int unit, int port,
                                         uint16 roe_ethtype,
                                         uint16 fast_eth_ethtype,
                                         uint16 vlan_ethtype,
                                         uint16 qinq_ethtype)
{

    uint64 reg_val;
    uint64 write_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));

    COMPILER_64_SET(write_val, 0, (uint32) roe_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_ROE_ETHERTYPEf, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) fast_eth_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_FAST_ETH_ETHERTYPEf, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) vlan_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_VLAN_ETYPEf, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) qinq_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_QINQ_ETYPEf, write_val);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_DECAP_CTRLr(unit, port, write_val));

    return SOC_E_NONE;

}


int cprif_drv_decap_pkt_type_config_get(int unit, int port,
                                         uint16* roe_ethtype,
                                         uint16* fast_eth_ethtype,
                                         uint16* vlan_ethtype,
                                         uint16* qinq_ethtype)

{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_ROE_ETHERTYPEf);
    *roe_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_FAST_ETH_ETHERTYPEf);
    *fast_eth_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_VLAN_ETYPEf );
    *vlan_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_QINQ_ETYPEf);
    *qinq_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
    return SOC_E_NONE;
}

int cprif_drv_decap_flow_classification_entry_set(int unit, int port,
                                                   uint8 subtype,
                                                   uint32 queue_num,
                                                   uint32 option,
                                                   uint32 flow_type )
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_0m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_1m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_2m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_3m};

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_CFS_FLOW_TYPEf, &field_buf);

    field_buf = option;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_CFS_USE_FLOWIDf, &field_buf);

    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_CFS_QUEUE_NUMf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, subtype, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_flow_classification_entry_get(int unit, int port,
                                                   uint8 subtype,
                                                   uint32* queue_num,
                                                   uint32* option,
                                                   uint32* flow_type )

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_0m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_1m ,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_2m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_3m};
    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, subtype, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_CFS_QUEUE_NUMf, &field_buf);
    *queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_CFS_USE_FLOWIDf, &field_buf);
    *option = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_CFS_FLOW_TYPEf, &field_buf);
    *flow_type = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_flow_to_queue_map_set(int unit, int port,
                                           uint32 flow_id,
                                           uint32 queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_FLOWID_QUE_LOOKUP_0m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_1m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_2m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_3m };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_NUMf, &field_buf);
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, flow_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_flow_to_queue_map_get(int unit, int port,
                                           uint32 flow_id,
                                           uint32* queue_num)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_FLOWID_QUE_LOOKUP_0m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_1m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_2m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_3m };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_NUMf, &field_buf);
    *queue_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_framer_tgen_tick_set(int unit, int port,
                                       uint32 divider,
                                       uint32 bfrm_ticks,
                                       uint32 bfrm_ticks_bitmap,
                                       uint32 bitmap_size)
{


    uint64 write_val;
    uint64 field_buf;
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &write_val));

    COMPILER_64_SET(field_buf,0,divider);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_DIVIDERf, field_buf);
    COMPILER_64_SET(field_buf,0,bfrm_ticks_bitmap);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_BFRM_TICKS_BITMAPf, field_buf);
    COMPILER_64_SET(field_buf,0,bitmap_size);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_BFRM_TICKS_BITMAP_SIZEf, field_buf);
    COMPILER_64_SET(field_buf,0,bfrm_ticks);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_BFRM_TICKSf, field_buf);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG1r(unit, port, write_val));

    return SOC_E_NONE;

}

int cprif_drv_tx_framer_tgen_tick_get(int unit, int port,
                                       uint32* divider,
                                       uint32* bfrm_ticks,
                                       uint32* bfrm_ticks_bitmap,
                                       uint32* bitmap_size)

{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &reg_val));

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_DIVIDERf);
    *divider =  COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFRM_TICKS_BITMAPf);
    *bfrm_ticks =  COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFRM_TICKS_BITMAP_SIZEf);
    *bitmap_size = COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFRM_TICKSf);
    *bfrm_ticks_bitmap = COMPILER_64_LO(read_val);

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_enable(int unit, int port,
                                     uint8 enable)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) enable);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG2r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG2r, &reg_val,
                        TX_TGEN_ENABLEf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG2r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_offset_set(int unit, int port,
                                         uint64  offset)
{
    uint64 write_val;
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG2r(unit, port, &write_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG2r, &write_val, TX_TGEN_TS_OFFSETf, offset);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG2r(unit, port, write_val));
    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_offset_get(int unit, int port,
                                         uint64*  offset)
{
    uint64 reg_val;
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG2r(unit, port, &reg_val));
    *offset = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG2r, reg_val, TX_TGEN_TS_OFFSETf);
    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_bfn_set(int unit, int port,
                                           uint16  bfn)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) bfn);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG4r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG4r, &reg_val,
                        TX_TGEN_BFNf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG4r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_bfn_get(int unit, int port,
                                           uint16* bfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG4r(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG4r,
                                   reg_val, TX_TGEN_BFNf);
    *bfn = COMPILER_64_LO(read_val) & 0xFFFF;

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_hfn_set(int unit, int port,
                                           uint8 hfn)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) hfn);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG3r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG3r, &reg_val,
                        TX_TGEN_HFNf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG3r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_hfn_get(int unit, int port,
                                           uint8* hfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG3r(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG3r, reg_val,
                                   TX_TGEN_BFNf);
    *hfn = COMPILER_64_LO(read_val) & 0xFF;

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_select_counter_set(int unit, int port,
                                                 uint8  hfn_bfn)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) hfn_bfn);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &reg_val,
                        TX_TGEN_BFN_SELf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG1r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_select_counter_get(int unit, int port,
                                                 uint8*  hfn_bfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFN_SELf);
    *hfn_bfn = COMPILER_64_LO(read_val) & 0xFF;

    return SOC_E_NONE;
}

/*
 *
 *   RSVD4 Related Drivers.
 *
 */

int cprif_drv_rsvd4_header_compare_entry_set(int unit, int port,
                                              int index,
                                              cprimod_header_compare_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_HDR_CMP_TAB_CFG_0m,
                               CPRI_HDR_CMP_TAB_CFG_1m,
                               CPRI_HDR_CMP_TAB_CFG_2m,
                               CPRI_HDR_CMP_TAB_CFG_3m};

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->valid;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VALIDf, &field_buf);

    field_buf = entry->match_data;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MATCH_DATAf, &field_buf);

    field_buf = entry->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MASKf, &field_buf);

    field_buf = entry->flow_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, FLOW_IDf, &field_buf);

    field_buf = entry->flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, CTRL_FLOWf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_header_compare_entry_get(int unit, int port,
                                              int index,
                                              cprimod_header_compare_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_HDR_CMP_TAB_CFG_0m,
                               CPRI_HDR_CMP_TAB_CFG_1m,
                               CPRI_HDR_CMP_TAB_CFG_2m,
                               CPRI_HDR_CMP_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VALIDf, &field_buf);
    entry->valid =  field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MATCH_DATAf, &field_buf);
    entry->match_data = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MASKf, &field_buf);
    entry->mask = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, FLOW_IDf, &field_buf);
    entry->flow_id = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, CTRL_FLOWf, &field_buf);
    entry->flow_type = field_buf;;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_rsvd4_modulo_rule_entry_set(int unit, int port,
                                           uint8  index,
                                           uint8  control_rule,
                                           cprimod_modulo_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t data_mem_select[] = { BRCM_RESERVED_CPM_23m,
                                    BRCM_RESERVED_CPM_24m,
                                    BRCM_RESERVED_CPM_25m,
                                    BRCM_RESERVED_CPM_26m
                                  };
    soc_mem_t ctrl_mem_select[] = { BRCM_RESERVED_CPM_27m,
                                    BRCM_RESERVED_CPM_28m,
                                    BRCM_RESERVED_CPM_29m,
                                    BRCM_RESERVED_CPM_30m
                                  };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (control_rule) {
        local_mem = ctrl_mem_select[port_core_index];
    } else {
        local_mem = data_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->active;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_5f, &field_buf);

    field_buf = entry->modulo_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_4f, &field_buf);

    field_buf = entry->modulo_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_3f, &field_buf);

    field_buf = entry->dbm_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_2f, &field_buf);

    field_buf = entry->flow_dbm_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_1f, &field_buf);

    field_buf = entry->flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_0f, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_modulo_rule_entry_get(int unit, int port,
                                           uint8  index,
                                           uint8  control_rule,
                                           cprimod_modulo_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t data_mem_select[] = { BRCM_RESERVED_CPM_23m,
                                    BRCM_RESERVED_CPM_24m,
                                    BRCM_RESERVED_CPM_25m,
                                    BRCM_RESERVED_CPM_26m
                                  };
    soc_mem_t ctrl_mem_select[] = { BRCM_RESERVED_CPM_27m,
                                    BRCM_RESERVED_CPM_28m,
                                    BRCM_RESERVED_CPM_29m,
                                    BRCM_RESERVED_CPM_30m
                                  };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (control_rule) {
        local_mem = ctrl_mem_select[port_core_index];
    } else {
        local_mem = data_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_set(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_5f, &field_buf);
    entry->active = field_buf;

    soc_mem_field_set(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_4f, &field_buf);
    entry->modulo_value = field_buf;;

    soc_mem_field_set(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_3f, &field_buf);
    entry->modulo_index = field_buf;;

    soc_mem_field_set(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_2f, &field_buf);
    entry->dbm_enable = field_buf;;

    soc_mem_field_set(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_1f, &field_buf);
    entry->flow_dbm_id = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_0f, &field_buf);
    entry->flow_type = field_buf;;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_dual_bitmap_rule_entry_set(int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem0;
    soc_mem_t local_mem1;
    soc_mem_t dbm_rule0_mem_select[] = { BRCM_RESERVED_CPM_31m,
                                         BRCM_RESERVED_CPM_32m,
                                         BRCM_RESERVED_CPM_33m,
                                         BRCM_RESERVED_CPM_34m
                                       };
    soc_mem_t dbm_rule1_mem_select[] = { BRCM_RESERVED_CPM_35m,
                                         BRCM_RESERVED_CPM_36m,
                                         BRCM_RESERVED_CPM_37m,
                                         BRCM_RESERVED_CPM_38m
                                       };

    uint32 *wr_data_ptr0 = NULL;
    uint32 *wr_data_ptr1 = NULL;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem0 = dbm_rule0_mem_select[port_core_index];
    local_mem1 = dbm_rule1_mem_select[port_core_index];

    /*
     * Preparing DBM Rule 0.
     */

    num_words = soc_mem_entry_words(unit, local_mem0);
    wr_data_ptr0 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    if (wr_data_ptr0 == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }
    sal_memset(wr_data_ptr0, 0, sizeof(uint32) * num_words);

    field_buf = entry->bm1_mult;
    soc_mem_field_set(unit, local_mem0, wr_data_ptr0, BRCM_RESERVED_CPM_31_2f, &field_buf);

    soc_mem_field_set(unit, local_mem0, wr_data_ptr0, BRCM_RESERVED_CPM_31_1f, entry->bm1);

    field_buf = entry->bm1_size;
    soc_mem_field_set(unit, local_mem0, wr_data_ptr0, BRCM_RESERVED_CPM_31_0f, &field_buf);


    /*
     * Preparing DBM Rule 1.
     */

    num_words = soc_mem_entry_words(unit, local_mem1);
    wr_data_ptr1 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    if (wr_data_ptr1 == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }
    sal_memset(wr_data_ptr1, 0, sizeof(uint32) * num_words);

    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_4f, entry->bm2);

    field_buf = entry->bm2_size;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_3f, &field_buf);

    field_buf = entry->num_slots;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_2f, &field_buf);

    field_buf = entry->pos_grp_index;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_1f, &field_buf);

    field_buf = entry->pos_grp_size;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_0f, &field_buf);


    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem0, blk, index, wr_data_ptr0));
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem1, blk, index, wr_data_ptr1));

exit:
    if (wr_data_ptr0) {
        sal_free(wr_data_ptr0);
    }
    if (wr_data_ptr1) {
        sal_free(wr_data_ptr1);
    }

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_dual_bitmap_rule_entry_get(int unit, int port,
                                               uint8  index,
                                               cprimod_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem0;
    soc_mem_t local_mem1;
    soc_mem_t dbm_rule0_mem_select[] = { BRCM_RESERVED_CPM_31m,
                                         BRCM_RESERVED_CPM_32m,
                                         BRCM_RESERVED_CPM_33m,
                                         BRCM_RESERVED_CPM_34m
                                       };
    soc_mem_t dbm_rule1_mem_select[] = { BRCM_RESERVED_CPM_35m,
                                         BRCM_RESERVED_CPM_36m,
                                         BRCM_RESERVED_CPM_37m,
                                         BRCM_RESERVED_CPM_38m
                                       };

    uint32 *rd_data_ptr0 = 0;
    uint32 *rd_data_ptr1 = 0;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem0 = dbm_rule0_mem_select[port_core_index];
    local_mem1 = dbm_rule1_mem_select[port_core_index];

    /*
     * Preparing DBM Rule 0.
     */

    num_words = soc_mem_entry_words(unit, local_mem0);
    rd_data_ptr0 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr0);
    sal_memset(rd_data_ptr0, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem0, blk, index, rd_data_ptr0));

    soc_mem_field_get(unit, local_mem0, rd_data_ptr0, BRCM_RESERVED_CPM_31_2f, &field_buf);
    entry->bm1_mult = field_buf;

    soc_mem_field_get(unit, local_mem0, rd_data_ptr0, BRCM_RESERVED_CPM_31_1f, entry->bm1);

    soc_mem_field_get(unit, local_mem0, rd_data_ptr0, BRCM_RESERVED_CPM_31_0f, &field_buf);
    entry->bm1_size  = field_buf;;


    /*
     * Preparing DBM Rule 1.
     */

    num_words = soc_mem_entry_words(unit, local_mem1);
    rd_data_ptr1 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr1);
    sal_memset(rd_data_ptr1, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem1, blk, index, rd_data_ptr1));

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_4f, entry->bm2);

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_3f, &field_buf);
    entry->bm2_size  = field_buf;;

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_2f, &field_buf);
    entry->num_slots = field_buf;;

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_1f, &field_buf);
    entry->pos_grp_index = field_buf;;

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_0f, &field_buf);
    entry->pos_grp_size = field_buf;;



exit:
    sal_free(rd_data_ptr0);
    sal_free(rd_data_ptr1);

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_set(int unit, int port,
                                           uint8  index,
                                           cprimod_secondary_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_18m,
                               BRCM_RESERVED_CPM_19m,
                               BRCM_RESERVED_CPM_20m,
                               BRCM_RESERVED_CPM_21m
                             };


    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];


    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->bm1_mult;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_5f, &field_buf);

    field_buf = entry->bm1;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_4f, &field_buf);

    field_buf = entry->bm1_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_3f, &field_buf);

    field_buf = entry->bm2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_2f, &field_buf);

    field_buf = entry->bm2_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_1f, &field_buf);

    field_buf = entry->num_slots;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_0f, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_get(int unit, int port,
                                           uint8  index,
                                           cprimod_secondary_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_18m,
                               BRCM_RESERVED_CPM_19m,
                               BRCM_RESERVED_CPM_20m,
                               BRCM_RESERVED_CPM_21m
                             };


    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (index >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_5f, &field_buf);
    entry->bm1_mult = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_4f, &field_buf);
    entry->bm1  = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_3f, &field_buf);
    entry->bm1_size = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_2f, &field_buf);
    entry->bm2 = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_1f, &field_buf);
    entry->bm2_size = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_0f, &field_buf);
    entry->num_slots = field_buf ;

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, rd_data_ptr));

exit:
    sal_free(rd_data_ptr);

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_dbm_position_entry_set(int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_pos_table_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_39m,
                               BRCM_RESERVED_CPM_40m,
                               BRCM_RESERVED_CPM_41m,
                               BRCM_RESERVED_CPM_42m
                             };


    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];


    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->valid;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_3f, &field_buf);

    field_buf = entry->flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_2f, &field_buf);

    field_buf = entry->flow_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_1f, &field_buf);

    field_buf = entry->index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_0f, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_dbm_position_entry_get(int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_pos_table_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_39m,
                               BRCM_RESERVED_CPM_40m,
                               BRCM_RESERVED_CPM_41m,
                               BRCM_RESERVED_CPM_42m
                             };


    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_3f, &field_buf);
    entry->valid = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_2f, &field_buf);
    entry->flow_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_1f, &field_buf);
    entry->flow_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_0f, &field_buf);
    entry->index = field_buf;


exit:
    sal_free(rd_data_ptr);

    SOC_FUNC_RETURN;
}

/*
 * Below are the tasks useful to clear status during Dynamic Add/Del axcs
 */

int cprif_drv_encap_queue_sts_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUEUE_STS_0m,
                               CPRI_ENCAP_QUEUE_STS_1m,
                               CPRI_ENCAP_QUEUE_STS_2m,
                               CPRI_ENCAP_QUEUE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_ordering_info_sts_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_STS_0m,
                               CPRI_ENCAP_ORDERING_INFO_STS_1m,
                               CPRI_ENCAP_ORDERING_INFO_STS_2m,
                               CPRI_ENCAP_ORDERING_INFO_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_eth_hdr_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_ETH_HDR_0m,
                               CPRI_ENCAP_QUE_ETH_HDR_1m,
                               CPRI_ENCAP_QUE_ETH_HDR_2m,
                               CPRI_ENCAP_QUE_ETH_HDR_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_encap_data_tab_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_DATA_0m,
                               CPRI_ENCAP_DATA_1m,
                               CPRI_ENCAP_DATA_2m,
                               CPRI_ENCAP_DATA_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_container_parser_mapp_state_tab_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CPRSR_MAP_STATE_TAB_STS_0m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_1m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_2m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_pak_buff_state_tab_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_IQ_PK_BUFF_AXC_STATE_STS_0m,
                               CPRI_IQ_PK_BUFF_AXC_STATE_STS_1m,
                               CPRI_IQ_PK_BUFF_AXC_STATE_STS_2m,
                               CPRI_IQ_PK_BUFF_AXC_STATE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_pak_buff_payld_tab_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_IQ_PK_BUFF_AXC_PLD_STS_0m,
                               CPRI_IQ_PK_BUFF_AXC_PLD_STS_1m,
                               CPRI_IQ_PK_BUFF_AXC_PLD_STS_2m,
                               CPRI_IQ_PK_BUFF_AXC_PLD_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_unpsk_buff_state_tbl_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_IQU_STATE_STS_0m,
                               CPRI_IQU_STATE_STS_1m,
                               CPRI_IQU_STATE_STS_2m,
                               CPRI_IQU_STATE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_cont_assembly_map_state_tbl_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CA_MAP_STATE_TAB_STS_0m,
                               CPRI_CA_MAP_STATE_TAB_STS_1m,
                               CPRI_CA_MAP_STATE_TAB_STS_2m,
                               CPRI_CA_MAP_STATE_TAB_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_state_tbl_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_QUEUE_STS_0m,
                               CPRI_DECAP_QUEUE_STS_1m,
                               CPRI_DECAP_QUEUE_STS_2m,
                               CPRI_DECAP_QUEUE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_data_tbl_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_DATA_0m,
                               CPRI_DECAP_DATA_1m,
                               CPRI_DECAP_DATA_2m,
                               CPRI_DECAP_DATA_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

/*
 * Program rxframer ctrl register with length of ctrl-word (based on speed)
 * NB: this information is also in basic_frame_cfg.ctrl_w_length,
 *  but we duplicate it here so that users don't need to deal
 *  with that structure.
 */
int cprif_drv_set_rxframer_cw_num_bytes(int unit, int port, cprimod_port_speed_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 cw_len;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &data));

    cw_len = 0;
    switch (speed) {
        case cprimodSpd614p4:
            cw_len = 1;    /*  fl=16 */
            break;
        case cprimodSpd1228p8:
            cw_len = 2;    /*  fl=32 */
            break;
        case cprimodSpd2457p6:
            cw_len = 4;    /*  fl=64 */
            break;
        case cprimodSpd3072p0:
            cw_len = 5;    /*  fl=80 */
            break;
        case cprimodSpd4915p2:
            cw_len = 8;    /* fl=128 */
            break;
        case cprimodSpd6144p0:
            cw_len = 10;   /* fl=160 */
            break;
        case cprimodSpd8110p08:
            cw_len = 16;   /* fl=256 */
            break;
        case cprimodSpd9830p4:
            cw_len = 16;   /* fl=256 */
            break;
        case cprimodSpd10137p6:
            cw_len = 16;   /* fl=320 */
            break;
        case cprimodSpd12165p12:
            cw_len = 16;   /* fl=768 */
            break;
        case cprimodSpd24330p24:
            cw_len = 16;   /* fl=768 */
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }

    COMPILER_64_SET(write_val, 0, cw_len);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &data, RXFRM_CW_NUM_BYTESf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_cpri_rxpcs_speed_set(int unit, int port,
                                   cprimod_port_speed_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 is10b;
    uint32 pcs_wd_len;
    cprimod_cpri_pcs_mode_t pcs_mode;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    pcs_wd_len = 0;
    switch (speed) {
        case cprimodSpd614p4:
            pcs_wd_len = 8;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            break;
        case cprimodSpd1228p8:
            pcs_wd_len =  16;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            break;
        case cprimodSpd2457p6:
            pcs_wd_len =  32;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            break;
        case cprimodSpd3072p0:
            pcs_wd_len =  40;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            break;
        case cprimodSpd4915p2:
            pcs_wd_len =  64;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 0;
            break;
        case cprimodSpd6144p0:
            pcs_wd_len =  80;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 0;
            break;
        case cprimodSpd8110p08:
            pcs_wd_len =  80;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            break;
        case cprimodSpd9830p4:
            pcs_wd_len = 128;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 0;
            break;
        case cprimodSpd10137p6:
            pcs_wd_len = 160;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            break;
        case cprimodSpd12165p12:
            pcs_wd_len = 192;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            break;
        case cprimodSpd24330p24:
            pcs_wd_len = 384;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }

    COMPILER_64_SET(write_val, 0, (uint32) pcs_mode);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_MODEf,
                        write_val);

    COMPILER_64_SET(write_val, 0, pcs_wd_len);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_WORD_LENf,
                        write_val);

    COMPILER_64_SET(write_val, 0, is10b);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_10B_MODEf,
                        write_val);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(cprif_drv_set_rxframer_cw_num_bytes(unit, port, speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_rxpcs_speed_get(int unit, int port,
                                   cprimod_port_speed_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 pcs_wd_len;
    cprimod_cpri_pcs_mode_t pcs_mode;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));

    read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                   RXPCS_WORD_LENf);
    pcs_wd_len = COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                   RXPCS_MODEf);
    pcs_mode = COMPILER_64_LO(read_val);

    if ((pcs_wd_len == 8) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd614p4;
    }
    if ((pcs_wd_len == 16) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd1228p8;
    }
    if ((pcs_wd_len == 32) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd2457p6;
    }
    if ((pcs_wd_len == 40) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd3072p0;
    }
    if ((pcs_wd_len == 64) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd4915p2;
    }
    if ((pcs_wd_len == 80) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd6144p0;
    }
    if ((pcs_wd_len == 80) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd8110p08;
    }
    if ((pcs_wd_len == 128) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd9830p4;
    }
    if ((pcs_wd_len == 160) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd10137p6;
    }
    if ((pcs_wd_len == 192) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd12165p12;
    }
    if ((pcs_wd_len == 384) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd24330p24;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_txpcs_speed_set(int unit, int port,
                                   cprimod_port_speed_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 data32;
    cprimod_cpri_pcs_mode_t pcs_mode;
    int bfrm_ticks, ticks_bitmap_size, ticks_bitmap;
    int pcs_word_len, ctrl_byte_len;
    SOC_INIT_FUNC_DEFS;

    pcs_word_len =   8;
    pcs_mode = cprimodCpriPcsMode8b10b;
    ctrl_byte_len=1;
    ticks_bitmap=0;
    ticks_bitmap_size=0;
    bfrm_ticks=64;

    switch (speed) {
        case cprimodSpd614p4:
            pcs_word_len =   8;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=1;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=64;
            break;
        case cprimodSpd1228p8:
            pcs_word_len =  16;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=2;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=64;
            break;
        case cprimodSpd2457p6:
            pcs_word_len =  32;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=4;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=64;
            break;
        case cprimodSpd3072p0:
            pcs_word_len =  40;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=5;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=80;
            break;
        case cprimodSpd4915p2:
            pcs_word_len =  64;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=8;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=32;
            break;
        case cprimodSpd6144p0:
            pcs_word_len =  80;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=10;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=40;
            break;
        case cprimodSpd8110p08:
            pcs_word_len =  80;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=64;
            break;
        case cprimodSpd9830p4:
            pcs_word_len = 128;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=16;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=64;
            break;
        case cprimodSpd10137p6:
            pcs_word_len = 160;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            ticks_bitmap=0;
            ticks_bitmap_size=0;
            bfrm_ticks=66;
            break;
        case cprimodSpd12165p12:
            pcs_word_len = 192;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            ticks_bitmap=0xfff0;
            ticks_bitmap_size=4;
            bfrm_ticks=79;
            break;
        case cprimodSpd24330p24:
            pcs_word_len = 384;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            ticks_bitmap=0xfff8;
            ticks_bitmap_size=4;
            bfrm_ticks=158;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

    COMPILER_64_SET(write_val, 0, (uint32) pcs_mode);
    soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_MODEf,
                        write_val);

    COMPILER_64_SET(write_val, 0, (uint32) pcs_word_len);
    soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                        TX_PCS_WORD_LEN_8B10Bf, write_val);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                     &data32));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_GENERALr, &data32, TX_CWA_TCWf,
                      ctrl_byte_len);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                      data32));
    /* setting up tgen */
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &data));
    COMPILER_64_SET(write_val, 0, (uint32) ticks_bitmap);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &data,
                        TX_TGEN_BFRM_TICKS_BITMAPf, write_val);
    COMPILER_64_SET(write_val, 0, (uint32) ticks_bitmap_size);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &data,
                        TX_TGEN_BFRM_TICKS_BITMAP_SIZEf, write_val);
    COMPILER_64_SET(write_val, 0, (uint32) bfrm_ticks);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &data,
                        TX_TGEN_BFRM_TICKSf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG1r(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_txpcs_speed_get(int unit, int port,
                                   cprimod_port_speed_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 pcs_mode;
    uint32 pcs_word_len;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

    read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                   TX_PCS_WORD_LEN_8B10Bf);
    pcs_word_len = COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                   TX_PCS_MODEf);
    pcs_mode = COMPILER_64_LO(read_val);

    *speed = cprimodSpdCount;
    if ((pcs_word_len == 8) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd614p4;
    }
    if ((pcs_word_len == 16) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd1228p8;
    }
    if ((pcs_word_len == 32) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd2457p6;
    }
    if ((pcs_word_len == 40) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd3072p0;
    }
    if ((pcs_word_len == 64) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd4915p2;
    }
    if ((pcs_word_len == 80) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd6144p0;
    }
    if ((pcs_word_len == 80) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd8110p08;
    }
    if ((pcs_word_len == 128) && (pcs_mode == cprimodCpriPcsMode8b10b)) {
        *speed = cprimodSpd9830p4;
    }
    if ((pcs_word_len == 160) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd10137p6;
    }
    if ((pcs_word_len == 192) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd12165p12;
    }
    if ((pcs_word_len == 384) && (pcs_mode == cprimodCpriPcsMode64b66b)) {
        *speed = cprimodSpd24330p24;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rxpcs_speed_set(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 data32;
    uint32 speedi, speedv;
    uint32 speedi_h, speedi_l;
    uint64 speedi_64;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    if (speed == cprimodRsvd4SpdMult4X) {
        speedi = 4;
        speedv = 2;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        speedi = 8;
        speedv = 3;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for rsvd4"));
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, (uint32) cprimodCpriPcsMode8b10b);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                        RXPCS_MODEf, write_val);

    COMPILER_64_SET(write_val, (uint32) 0, (uint32) 0);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                        RXPCS_WORD_LENf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port,
                                                           &data));
    COMPILER_64_SET(write_val, (uint32) 0, (uint32) 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        RXPCS_BIT_ENDIAN_FLIPf, write_val);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_6f, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) speedv);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_2f, write_val);

    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port,
                                                            data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port,
                                                           &data));
    COMPILER_64_SET(write_val, (uint32) 0, (uint32) CPRIF_DRV_NUMBER_OF_MSG);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                        BRCM_RESERVED_CPM_308_3f, write_val);

    COMPILER_64_SET(write_val, (uint32) 0, (uint32) CPRIF_DRV_N_MG_IN_MSTR_FRM);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                        BRCM_RESERVED_CPM_308_2f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_308r(unit, port,
                                                            data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_309r(unit, port,
                                                           &data));
    speedi_h = 0;
    speedi_l = (speedi * CPRIF_DRV_NUMBER_OF_MSG *
               (CPRIF_DRV_N_MG_IN_MSTR_FRM*CPRIF_DRV_RSVD4_MSG_SIZE+1)) & 0xffffffff;
    COMPILER_64_SET(speedi_64, speedi_h, speedi_l);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_309r, &data,
                        BRCM_RESERVED_CPM_309_0f, speedi_64);

    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_309r(unit, port,
                                                            data));


    SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_TDM_CTRLr(unit, port, &data32));
    soc_reg_field_set(unit, CPRI_ENCAP_TDM_CTRLr, &data32,
                        TDM_DISABLEf, 1);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_TDM_CTRLr(unit, port, data32));

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &data32));
    soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &data32,
                        BRCM_RESERVED_CPM_306_0f, 1);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, data32));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rxpcs_speed_get(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 speedi;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port,
                                                           &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r,
                                  data, BRCM_RESERVED_CPM_307_2f);
    speedi = COMPILER_64_LO(read_val);

    if (speedi ==  2) {
        *speed = cprimodRsvd4SpdMult4X;
    } else if (speedi ==  3) {
        *speed = cprimodRsvd4SpdMult8X;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_txpcs_speed_set(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 speedi, speedv;
    uint32 speedi_h, speedi_l;
    uint64 speedi_64;
    SOC_INIT_FUNC_DEFS;

    if (speed == cprimodRsvd4SpdMult4X) {
        speedi = 4;
        speedv = 2;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        speedi = 8;
        speedv = 3;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for rsvd4"));
    }
    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port,
                                                         &data));

    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                        BRCM_RESERVED_CPM_12_3f, write_val);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                        BRCM_RESERVED_CPM_12_7f, write_val);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                        BRCM_RESERVED_CPM_12_8f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_12r(unit, port,
                                                          data));
    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_3r(unit, port,
                                                         &data));

    COMPILER_64_SET(write_val, 0, speedv);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_2f, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) CPRIF_DRV_NUMBER_OF_MSG);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_3f, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) CPRIF_DRV_N_MG_IN_MSTR_FRM);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_4f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_3r(unit, port,
                                                          data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_4r(unit, port,
                                                         &data));
    speedi_h = 0;
    speedi_l = ((speedi*CPRIF_DRV_NUMBER_OF_MSG)-1) & 0xffffffff;
    COMPILER_64_SET(speedi_64, speedi_h, speedi_l);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_4r, &data,
                        BRCM_RESERVED_CPM_4_0f, speedi_64);
    speedi_l = ((speedi*(CPRIF_DRV_N_MG_IN_MSTR_FRM-1) *
               CPRIF_DRV_NUMBER_OF_MSG)-1) & 0xffffffff;
    COMPILER_64_SET(speedi_64, speedi_h, speedi_l);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_4r, &data,
                        BRCM_RESERVED_CPM_4_1f, speedi_64);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_4r(unit, port, data));
    COMPILER_64_ZERO(data);
    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_2r, &data,
                        BRCM_RESERVED_CPM_2_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_2r(unit, port,
                                                          data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_txpcs_speed_get(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 speedv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_3r(unit, port,&data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_3r,
                                   data, BRCM_RESERVED_CPM_3_2f);

    speedv = COMPILER_64_LO(read_val);
    *speed = cprimodSpdCount;
    if (speedv ==  2) {
        *speed = cprimodRsvd4SpdMult4X;
    } else if (speedv ==  3) {
        *speed = cprimodRsvd4SpdMult8X;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_txcpri_port_type_get(int unit, int port,
                                   cprimod_port_interface_type_t *interface)
{
    uint64 data;
    uint64 read_val;
    uint32 iftype;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_2r(unit, port,
                                                         &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_2r,
                                   data, BRCM_RESERVED_CPM_2_0f);
    iftype = COMPILER_64_LO(read_val);

    if (iftype == 1) {
      *interface = cprimodRsvd4;
    } else {
      *interface = cprimodCpri;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_rxcpri_port_type_get(int unit, int port,
                                   cprimod_port_interface_type_t *interface)
{
    uint64 data;
    uint64 read_val;
    uint32 iftype;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port,
                                                           &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r,
                                   data, BRCM_RESERVED_CPM_307_0f);
    iftype = COMPILER_64_LO(read_val);

    if (iftype == 1) {
      *interface = cprimodRsvd4;
    } else {
      *interface = cprimodCpri;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_frame_optional_config_set(int unit, int port,
                                        cprimod_rx_config_field_t field,
                                        int value)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, (uint32) value);
    switch (field) {
    case cprimodRxConfig8b10bAllowSeedChange:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_ALLOW_SEED_CHANGEf, write_val);
        break;
    case cprimodRxConfig8b10bDescrHw:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_DESCR_HWf, write_val);
        break;
    case cprimodRxConfig8b10bDescrEn:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_DESCR_ENf, write_val);
        break;
    case cprimodRxConfiglinkAcqSwMode:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_LINK_ACQ_SW_MODEf, write_val);
        break;
    case cprimodRxConfig64b66bLosOption:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_LOS_OPTIONf, write_val);
        break;
    case cprimodRxConfig8b10bForceCommaAlignEn:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_FORCE_COMMA_ALIGN_ENf, write_val);
        break;
    case cprimodRxConfig64b66bBerWindowLimit:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_BER_WINDOW_LIMITf, write_val);
        break;
    case cprimodRxConfig64b66bBerLimit:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_BER_LIMITf, write_val);
        break;
    case cprimodRxConfigtestMode:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_TEST_MODEf, write_val);
        break;
    case cprimodRxConfig64b66bInvalidShCnt:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_INVALID_SH_CNTf, write_val);
        break;
    case cprimodRxConfig64b66bValidShCnt:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_VALID_SH_CNTf, write_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx cpri"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_frame_optional_config_get(int unit, int port,
                                           cprimod_rx_config_field_t field,
                                           int* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    switch (field) {
    case cprimodRxConfig8b10bAllowSeedChange:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_ALLOW_SEED_CHANGEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig8b10bDescrHw:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_DESCR_HWf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig8b10bDescrEn:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_DESCR_ENf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfiglinkAcqSwMode:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_LINK_ACQ_SW_MODEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bLosOption:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_LOS_OPTIONf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig8b10bForceCommaAlignEn:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_FORCE_COMMA_ALIGN_ENf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bBerWindowLimit:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_BER_WINDOW_LIMITf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bBerLimit:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_BER_LIMITf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfigtestMode:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_TEST_MODEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bInvalidShCnt:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_INVALID_SH_CNTf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bValidShCnt:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_VALID_SH_CNTf);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx cpri"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_tx_frame_optional_config_set(int unit, int port,
                                           cprimod_tx_config_field_t field,
                                           uint32 value)
{
    uint64 data;
    uint64 write_val;
    uint32 data32;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(write_val, 0, value);
    switch (field) {
    case cprimodTxConfigagnosticMode:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            BRCM_RESERVED_CPM_11_8f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigtxpmdDisableOverrideVal:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_TXPMD_DISABLE_OVERRIDE_VALf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigtxpmdDisableOverrideEn:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_TXPMD_DISABLE_OVERRIDE_ENf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigseed8B10B:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_SEED_8B10Bf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigscrambleBypass:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_SCRAMBLE_BYPASSf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigseed8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_SEED_8B10Bf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigcwaScrEn8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port, &data32));
        soc_reg_field_set(unit, CPRI_TXFRM_CWA_GENERALr, &data32,
                          TX_CWA_SCR_EN_8B10Bf, value);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GENERALr(unit, port, data32));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx cpri"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_tx_frame_optional_config_get(int unit, int port,
                                           cprimod_tx_config_field_t field,
                                           uint32* value)
{
    uint64 data;
    uint64 read_val;
    uint32 data32;
    SOC_INIT_FUNC_DEFS;

    switch (field) {
    case cprimodTxConfigagnosticMode:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     BRCM_RESERVED_CPM_11_8f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigtxpmdDisableOverrideVal:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_TXPMD_DISABLE_OVERRIDE_VALf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigtxpmdDisableOverrideEn:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_TXPMD_DISABLE_OVERRIDE_ENf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigseed8B10B:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_SEED_8B10Bf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigscrambleBypass:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_SCRAMBLE_BYPASSf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigseed8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_SEED_8B10Bf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigcwaScrEn8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port, &data32));
        *value = soc_reg_field_get(unit, CPRI_TXFRM_CWA_GENERALr, data32,
                                   TX_CWA_SCR_EN_8B10Bf);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx cpri"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_rx_frame_optional_config_set(int unit, int port,
                                                 cprimod_rsvd4_rx_config_field_t field,
                                                 int value)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(write_val, 0, (uint32) value);
    switch (field) {
    case cprimodRsvd4RxConfigDescrambleEnable:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_1f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    case cprimodRsvd4RxConfigSyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_3f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    case cprimodRsvd4RxConfigUnsyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_4f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    case cprimodRsvd4RxConfigFrameSyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                            BRCM_RESERVED_CPM_308_0f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_308r(unit, port, data));
        break;
    case cprimodRsvd4RxConfigFrameUnsyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                            BRCM_RESERVED_CPM_308_1f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_308r(unit, port, data));
        break;
    case cprimodRsvd4RxConfigBlockSize:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_5f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx rsvd4"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_rx_frame_optional_config_get(int unit, int port,
                                                 cprimod_rsvd4_rx_config_field_t field,
                                                 int* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    switch (field) {
    case cprimodRsvd4RxConfigDescrambleEnable:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_1f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4RxConfigSyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_3f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4RxConfigUnsyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_4f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4RxConfigFrameSyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_308r, data,
                                     BRCM_RESERVED_CPM_308_0f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4RxConfigFrameUnsyncThreshold:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_308r, data,
                                     BRCM_RESERVED_CPM_308_1f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4RxConfigBlockSize:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_5f);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx rsvd4"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_rx_fsm_state_set(int unit, int port,
                                     cprimod_rsvd4_rx_fsm_state_t state)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(data);
    COMPILER_64_SET(write_val, 0, state);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_310r, &data,
                        BRCM_RESERVED_CPM_310_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_310r(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_fsm_state_get(int unit, int port,
                                     cprimod_rsvd4_rx_fsm_state_t* state)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_310r(unit, port, &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_310r, data,
                                 BRCM_RESERVED_CPM_310_0f);
    *state = COMPILER_64_LO(read_val);

    SOC_FUNC_RETURN;
}


int cprif_drv_rsvd4_rx_overide_set(int unit, int port,
                                   cprimod_rsvd4_rx_overide_t parameter,
                                   int enable, int value)
{
    uint64 data;
    uint64 write_val, write_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_311r(unit, port, &data));

    COMPILER_64_SET(write_val, 0, (uint32) value);
    COMPILER_64_SET(write_val1, 0, (uint32) enable);
    switch (parameter) {
    case cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_0f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_1f, write_val1);
        break;
    case cprimodRsvd4RxOverideFrameSyncTVldMgRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_2f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_3f, write_val1);
        break;
    case cprimodRsvd4RxOverideKMgIdlesRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_4f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_5f, write_val1);
        break;
    case cprimodRsvd4RxOverideIdleReqRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_6f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_7f, write_val1);
        break;
    case cprimodRsvd4RxOverideIdleAckRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_8f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_9f, write_val1);
        break;
    case cprimodRsvd4RxOverideSeedCapAndVerifyDone:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_10f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_11f, write_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for rsvd4 Rx overide"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_311r(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_overide_get(int unit, int port,
                                   cprimod_rsvd4_rx_overide_t parameter,
                                   int* enable, int* value)
{
    uint64 data;
    uint64 read_val, read_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_311r(unit, port, &data));
    switch (parameter) {
    case cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_0f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_1f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideFrameSyncTVldMgRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_2f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_3f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideKMgIdlesRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_4f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_5f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideIdleReqRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_6f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_7f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideIdleAckRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_8f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_9f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideSeedCapAndVerifyDone:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_10f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_11f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for rsvd4 Rx overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_tx_frame_optional_config_set(int unit, int port,
                                                 cprimod_rsvd4_tx_config_field_t field,
                                                 int value)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port, &data));
    COMPILER_64_SET(write_val, 0, (uint32) value);
    switch (field) {
    case cprimodRsvd4TxConfigForceOffState:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_0f, write_val);
        break;
    case cprimodRsvd4TxConfigAckT:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_1f, write_val);
        break;
    case cprimodRsvd4TxConfigForceIdleAck:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_4f, write_val);
        break;
    case cprimodRsvd4TxConfigLosEnable:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_6f, write_val);
        break;
    case cprimodRsvd4TxConfigScrambleEnable:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_5f, write_val);
        break;
    case cprimodRsvd4TxConfigScramblerSeed:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_2f, write_val);
        break;
    case cprimodRsvd4TxConfigTransmitterEnable:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_7f, write_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_12r(unit, port, data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_tx_frame_optional_config_get(int unit, int port,
                                                 cprimod_rsvd4_tx_config_field_t field,
                                                 int* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port, &data));
    switch (field) {
    case cprimodRsvd4TxConfigForceOffState:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_0f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4TxConfigAckT:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_1f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4TxConfigForceIdleAck:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_4f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4TxConfigLosEnable:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_6f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4TxConfigScrambleEnable:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_5f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4TxConfigScramblerSeed:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_2f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRsvd4TxConfigTransmitterEnable:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_7f);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_tx_fsm_state_set(int unit, int port,
                                     cprimod_rsvd4_tx_fsm_state_t state)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(data);
    COMPILER_64_SET(write_val, 0, state);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_13r, &data,
                        BRCM_RESERVED_CPM_13_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_13r(unit, port, data));

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_tx_fsm_state_get(int unit, int port,
                                     cprimod_rsvd4_tx_fsm_state_t* state)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_13r(unit, port, &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_13r, data,
                                 BRCM_RESERVED_CPM_13_0f);
    *state = COMPILER_64_LO(read_val);

    SOC_FUNC_RETURN;
}


int cprif_drv_rsvd4_tx_overide_set(int unit, int port,
                                   cprimod_rsvd4_tx_overide_t parameter,
                                   int enable, int value)
{
    uint64 data;
    uint64 write_val, write_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_14r(unit, port, &data));
    COMPILER_64_SET(write_val, 0, value);
    COMPILER_64_SET(write_val1, 0, enable);
    switch (parameter) {
    case cprimodRsvd4TxOverideStartTx:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_0f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_1f, write_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsAckCap:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_2f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_3f, write_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsIdleReq:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_4f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_5f, write_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsScrLock:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_6f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_7f, write_val1);
        break;
    case cprimodRsvd4TxOverideLosStauts:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_8f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_9f, write_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_14r(unit, port, data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_tx_overide_get(int unit, int port,
                                   cprimod_rsvd4_tx_overide_t parameter,
                                   int* enable, int* value)
{
    uint64 data;
    uint64 read_val, read_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_14r(unit, port, &data));
    switch (parameter) {
    case cprimodRsvd4TxOverideStartTx:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_0f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_1f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsAckCap:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_2f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_3f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsIdleReq:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_4f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_5f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsScrLock:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_6f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_7f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideLosStauts:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_8f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_9f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_pcs_status_get(int unit, int port, cprimod_rx_pcs_status_t status,
                                uint32* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_STATUSr(unit, port, &data));
    switch (status) {
    case cprimodRxPcsStatusLinkStatusLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_LINK_STATUS_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatusSeedLocked:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_SEED_LOCKEDf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatusSeedVector:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_SEED_VECTORf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatusLosLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_LOS_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatus64b66bHiBerLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_64B66B_HI_BER_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatus64b66bBlockLockLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_64B66B_BLOCK_LOCK_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for rsvd4 Rx overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*
* Functions related to init
*/

int cprif_drv_datapath_credit_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, DATAPATH_CREDIT_RSTf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_credit_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             DATAPATH_CREDIT_RSTf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_rx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_rx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_RX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_tx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_tx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_TX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_tx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_tx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_TX_Hf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_rx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_rx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_RX_Hf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_rx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_DPf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_rx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_RX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_tx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_DPf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_tx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_TX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_rx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_Hf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_rx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_RX_Hf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_tx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_Hf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_tx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_TX_Hf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_rx_dp to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_rx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_RX_DPf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_rx_h to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_rx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_RX_Hf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_tx_dp to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_tx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_TX_DPf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_tx_h to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_tx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_TX_Hf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.glas_rst to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_glas_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, GLAS_RSTf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_glas_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, GLAS_RSTf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_cipclk_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_cip_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_CIPCLK_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_cip_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_CIPCLK_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_glas_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_glas_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_GLAS_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_glas_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_GLAS_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_rx_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_rx_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_RX_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_rx_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_RX_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_tx_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_tx_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_TX_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_tx_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_TX_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * Programming the cip_clk selection for a port. You could choose cip
 * generated from PLL0 or PLL1
 */
int cprif_drv_cip_clk_pll_select_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CIP_CLK_CTRLr, &read_val,
                      USE_PMD_CIP_CLK_PLL1f,val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CIP_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_clk_pll_select_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CIP_CLK_CTRLr, read_val,
                             USE_PMD_CIP_CLK_PLL1f);

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_clk_div_ctrl_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CIP_CLK_CTRLr, &read_val,
                      CIP_CLK_DIV_CTRLf,val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CIP_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_clk_div_ctrl_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CIP_CLK_CTRLr, read_val,
                             CIP_CLK_DIV_CTRLf);

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_flush_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TX_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_TX_CTRLr, &read_val, TX_FLUSHf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TX_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_flush_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TX_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_TX_CTRLr, read_val, TX_FLUSHf);

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_disable_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RX_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RX_CTRLr, &read_val, RX_DISABLEf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RX_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_disable_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RX_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RX_CTRLr, read_val, RX_DISABLEf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cpri_or_enet_port_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    read_val = 0;
    soc_reg_field_set(unit, CPMPORT_TOP_LANE_CTRLr, &read_val,
                      LOGICAL_LANE_OWNERf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_LANE_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_or_enet_port_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_LANE_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPMPORT_TOP_LANE_CTRLr, read_val,
                             LOGICAL_LANE_OWNERf);

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_top_ctrl_set(int unit, int port, int val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_TOP_CTRLf, val);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_top_ctrl_get(int unit, int port, int *val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    *val = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, CIP_TOP_CTRLf);

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_iddq_set(int unit, int port, int val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PMD_IDDQf, val);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_iddq_get(int unit, int port, int *val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    *val = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PMD_IDDQf);

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_pll_ctrl_set(int unit, int port, int pll_num, int cip_pll_ctrl)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL0_CTRLf, cip_pll_ctrl);
    }
    if (pll_num == 1) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL1_CTRLf, cip_pll_ctrl);
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_pll_ctrl_get(int unit, int port, int pll_num, int *cip_pll_ctrl)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        *cip_pll_ctrl = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, CIP_PLL0_CTRLf);
    }
    if (pll_num == 1) {
        *cip_pll_ctrl = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, CIP_PLL1_CTRLf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_ref_clk_ctrl_set(int unit, int port, int pll_num, int refin, int refout)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL0_REFIN_ENf, refin);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL0_REFOUT_ENf, refout);
    }
    if (pll_num == 1) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL1_REFIN_ENf, refin);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL1_REFOUT_ENf, refout);
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_ref_clk_ctrl_get(int unit, int port, int pll_num, int *refin, int *refout)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        *refin  = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL0_REFIN_ENf);
        *refout = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL0_REFOUT_ENf);
    }
    if (pll_num == 1) {
        *refin  = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL1_REFIN_ENf);
        *refout = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL1_REFOUT_ENf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_ctrl_por_h_rstb_set(int unit, int port, int por_h_rstb)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &data, PMD_POR_H_RSTBf, por_h_rstb);
    SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PMD_CTRLr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_ctrl_por_h_rstb_get(int unit, int port, int *por_h_rstb)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    *por_h_rstb = soc_reg_field_get(unit, CPMPORT_PMD_CTRLr, data, PMD_POR_H_RSTBf);

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_core_datapath_hard_reset_set(int unit, int port, int pll_num, int rst)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    if (pll_num==0) {
        soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &data, PMD_CORE_PLL0_DP_H_RSTBf, rst);
    }
    if (pll_num==1) {
        soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &data, PMD_CORE_PLL1_DP_H_RSTBf, rst);
    }
    SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PMD_CTRLr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_core_datapath_hard_reset_get(int unit, int port, int pll_num, int *rst)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    if (pll_num==0) {
        *rst = soc_reg_field_get(unit, CPMPORT_PMD_CTRLr, data, PMD_CORE_PLL0_DP_H_RSTBf);
    }
    if (pll_num==1) {
        *rst = soc_reg_field_get(unit, CPMPORT_PMD_CTRLr, data, PMD_CORE_PLL1_DP_H_RSTBf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpmport_pll_ctrl_config_set(int unit, int port, int pll_num, int lcref_sel)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    if (pll_num == 0) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL0_CTRL_CONFIGr(unit, port, &data));
        soc_reg_field_set(unit, CPMPORT_PLL0_CTRL_CONFIGr, &data, LCREF_SELf, lcref_sel);
        SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PLL0_CTRL_CONFIGr(unit, port, data));
    }
    if (pll_num == 1) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL1_CTRL_CONFIGr(unit, port, &data));
        soc_reg_field_set(unit, CPMPORT_PLL1_CTRL_CONFIGr, &data, LCREF_SELf, lcref_sel);
        SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PLL1_CTRL_CONFIGr(unit, port, data));
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpmport_pll_ctrl_config_get(int unit, int port, int pll_num, int *lcref_sel)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    if (pll_num == 0) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL0_CTRL_CONFIGr(unit, port, &data));
        *lcref_sel = soc_reg_field_get(unit, CPMPORT_PLL0_CTRL_CONFIGr, data, LCREF_SELf);
    }
    if (pll_num == 1) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL1_CTRL_CONFIGr(unit, port, &data));
        *lcref_sel = soc_reg_field_get(unit, CPMPORT_PLL1_CTRL_CONFIGr, data, LCREF_SELf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_lane_swap_set(int unit, int port, int lane, cprimod_direction_t dir,
                            int phyport)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    data = 0;
    if (dir == cprimod_dir_rx) {
        switch (lane) {
        case 0:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_0r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_0r(unit, port, data));
            break;
        case 1:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_1r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_1r(unit, port, data));
            break;
        case 2:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_2r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_2r(unit, port, data));
            break;
        case 3:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_3r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_3r(unit, port, data));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    } else {
        switch (lane) {
        case 0:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_0r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_0r(unit, port, data));
            break;
        case 1:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_1r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_1r(unit, port, data));
            break;
        case 2:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_2r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_2r(unit, port, data));
            break;
        case 3:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_3r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_3r(unit, port, data));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_lane_swap_get(int unit, int port, int lane, cprimod_direction_t dir,
                            int *phyport)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    if (dir == cprimod_dir_rx) {
        switch (lane) {
        case 0:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_0r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_0r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        case 1:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_1r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_1r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        case 2:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_2r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_2r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        case 3:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_3r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_3r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    } else {
        switch (lane) {
        case 0:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_0r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_0r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        case 1:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_1r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_1r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        case 2:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_2r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_2r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        case 3:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_3r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_3r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_master_frame_sync_config_set(int unit, int port,
                                                    uint32  master_frame_number,
                                                    uint64  master_frame_start_time)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(field_buf,0,master_frame_number);
    _SOC_IF_ERR_EXIT
        (READ_BRCM_RESERVED_CPM_317r(unit, port, &reg_val));
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_317r, &reg_val, BRCM_RESERVED_CPM_317_0f, field_buf);

    COMPILER_64_COPY(field_buf,master_frame_start_time);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_317r, &reg_val, BRCM_RESERVED_CPM_317_1f, field_buf);

    _SOC_IF_ERR_EXIT
        (WRITE_BRCM_RESERVED_CPM_317r(unit, port, reg_val));

exit:

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_master_frame_sync_config_get(int unit, int port,
                                                    uint32*  master_frame_number,
                                                    uint64*  master_frame_start_time)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (READ_BRCM_RESERVED_CPM_317r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_317r, reg_val, BRCM_RESERVED_CPM_317_1f);
    COMPILER_64_TO_32_LO(*master_frame_number,field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_317r, reg_val, BRCM_RESERVED_CPM_317_0f);
    COMPILER_64_COPY(*master_frame_start_time,field_buf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_control_word_status_get(int unit, int port, cprif_drv_rx_control_word_status_t* info)
{
    uint64 reg_val;
    uint64 field_buf;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_STATUSr(unit, port, &reg_val));

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_HFNf);
    COMPILER_64_TO_32_LO(info->hyper_frame,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_BFNf);
    COMPILER_64_TO_32_LO(info->radio_frame,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_FNf);
    COMPILER_64_TO_32_LO(info->basic_frame,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_HDLC_RATEf);
    COMPILER_64_TO_32_LO(info->hdlc_rate,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_PROT_VERf);
    COMPILER_64_TO_32_LO(info->protocol_ver,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_ETH_PTRf);
    COMPILER_64_TO_32_LO(info->eth_pointer,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_RESETf);
    COMPILER_64_TO_32_LO(info->reset,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_RAIf);
    COMPILER_64_TO_32_LO(info->l1_rai,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_SDIf);
    COMPILER_64_TO_32_LO(info->l1_sdi,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_LOSf);
    COMPILER_64_TO_32_LO(info->l1_los,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_LOFf);
    COMPILER_64_TO_32_LO(info->l1_lof,field_buf);

    return SOC_E_NONE;
}

int cprif_drv_cpri_rx_l1_signal_signal_protection_set(int unit, int port, uint32 signal_map, uint32 enable)
{
    uint64 reg_val, tmp_en;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));

    if (enable != 0) {
        COMPILER_64_SET(tmp_en, 0, 1);
    } else {
        COMPILER_64_SET(tmp_en, 0, 0);
    }

    if (CPRIMOD_L1_SIGNAL_LOF_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_LOFf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_LOS_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_LOSf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_SDI_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_SDIf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_RAI_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_RAIf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_RESET_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_RESETf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_ETH_PTR_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_ETH_PTRf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_HDLC_RATE_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_HDLC_RATEf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_VER_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_PROT_VERf, tmp_en );
    }

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_cpri_rx_l1_signal_signal_protection_get(int unit, int port, uint32 signal_map, uint32* enable)
{
    uint64 reg_val, ret_val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));

    if (CPRIMOD_L1_SIGNAL_LOF_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_LOFf);
        if (!COMPILER_64_IS_ZERO(ret_val64) ) {
            CPRIMOD_L1_SIGNAL_LOF_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_LOS_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_LOSf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_LOS_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_SDI_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_SDIf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_SDI_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_RAI_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_RAIf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_RAI_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_RESET_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_RESETf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_RESET_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_ETH_PTR_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_ETH_PTRf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_ETH_PTR_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_HDLC_RATE_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_HDLC_RATEf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_HDLC_RATE_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_VER_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_PROT_VERf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_VER_SET(*enable);
        }
    }

    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_slow_hdlc_config_set(int unit, int port, cprif_cpri_rx_hdlc_config_t* config_info)
{
    uint64 reg_val, val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));
    /*
     *Need to pass uint64 to soc_reg64_field_set; 
     *Use COMPILER_64_SET to create one
     */
    COMPILER_64_SET(val64, 0, config_info->cw_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HDLC_CW_SIZEf, val64);

    COMPILER_64_SET(val64, 0, config_info->cw_sel);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HDLC_CW_SELf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RX_HDLC_CTRLr(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, config_info->ignore_fcs_err);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_IGNORE_FCS_ERRf, val64);
    COMPILER_64_SET(val64, 0, config_info->fcs_size);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_FCS_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->runt_frame_drop);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_RUNT_FRAME_DROPf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_frame_size);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_MIN_FRAME_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->long_frame_drop);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_LONG_FRAME_DROPf, val64);
    COMPILER_64_SET(val64, 0, config_info->max_frame_size);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_MAX_FRAME_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->use_fe_mac);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_USE_FE_MACf, val64);
    COMPILER_64_SET(val64, 0, config_info->queue_num);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_QUEUE_NUMf, val64);
    COMPILER_64_SET(val64, 0, config_info->crc_byte_swap);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_CRC_BYTE_ORDERf, val64);
    COMPILER_64_SET(val64, 0, config_info->crc_init_val);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_CRC_INIT_VALf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RX_HDLC_CTRLr(unit, port, reg_val));
    return SOC_E_NONE;
}

int cprif_drv_cpri_port_tx_cw_slow_hdlc_config_set(int unit, int port, cprif_cpri_tx_hdlc_config_t* config_info)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TX_HDLC_CTRLr(unit, port, &reg_val));

    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CRC_MODEf, config_info->crc_mode);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CW_SELf,   config_info->cw_sel);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CW_SIZEf,  config_info->cw_size);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_FCS_SIZEf, config_info->fcs_size);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_MIN_FLAGf, config_info->flag_size);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_USE_FE_MACf, config_info->use_fe_mac);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_QUEUE_NUMf, config_info->queue_num);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CRC_BYTE_ORDERf, config_info->crc_byte_swap);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CRC_INIT_VALf, config_info->crc_init_val);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_FILLING_FLAG_SELf, config_info->filling_flag_pattern);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TX_HDLC_CTRLr(unit, port, reg_val));
    return SOC_E_NONE;

}

int cprif_drv_cpri_port_rx_cw_fast_eth_word_config_set(int unit, int port, cprif_cpri_rx_fast_eth_word_config_t* config_info)
{
    uint64 rxfrm_cprsr, val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &rxfrm_cprsr));

    COMPILER_64_SET(val64, 0,  config_info->sub_channel_start);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_SCHAN_STARTf, val64);
    COMPILER_64_SET(val64, 0, config_info->sub_channel_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_SCHAN_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_sel);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_CW_SELf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_CW_SIZEf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, rxfrm_cprsr));
    return SOC_E_NONE;
}


int cprif_drv_cpri_port_rx_cw_fast_eth_config_set(int unit, int port, cprif_cpri_rx_fast_eth_config_t* config_info)
{
    uint64 rxfrm_fast_eth_ctrl, val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RX_FAST_ETH_CTRLr(unit, port, &rxfrm_fast_eth_ctrl));
    COMPILER_64_SET(val64, 0, config_info->queue_num);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_QUEUE_NUMf, val64);
    COMPILER_64_SET(val64, 0, config_info->ignore_fcs_err ? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_IGNORE_FCS_ERRf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_packet_size);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_MIN_PKT_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_packet_drop ? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_DROP_MIN_PKTf, val64);
    COMPILER_64_SET(val64, 0, config_info->max_packet_size);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_MAX_PKT_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->max_packet_drop ? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_DROP_MAX_PKTf, val64);
    COMPILER_64_SET(val64, 0, config_info->strip_crc? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_STRIP_CRCf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RX_FAST_ETH_CTRLr(unit, port, rxfrm_fast_eth_ctrl));
    return SOC_E_NONE;
}

int cprif_drv_cpri_port_tx_cw_fast_eth_config_set(int unit, int port, cprif_cpri_tx_fast_eth_config_t* config_info)
{
  uint64 txfrm_cwa, val64;

   SOC_IF_ERROR_RETURN
        (READ_CPRI_TX_FAST_ETH_CTRLr(unit, port, &txfrm_cwa));

    COMPILER_64_SET(val64, 0, config_info->queue_num);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_QUEUE_NUMf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_ipg);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_MIN_IPGf, val64);
    COMPILER_64_SET(val64, 0, config_info->crc_mode);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_CRC_MODEf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_sel);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_CW_SELf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_size);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_CW_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->sub_channel_size);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_SCHAN_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->sub_channel_start);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_SCHAN_STARTf, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TX_FAST_ETH_CTRLr(unit, port, txfrm_cwa));

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_copy_id_get(int unit, int port, uint32* copy_id)
{
  uint32 reg_val=0;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));
    *copy_id =  soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_SEC_NUM_GEN_COPY_IDf);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_copy_id_set(int unit, int port, uint32 copy_id)
{
  uint32 reg_val=0;

    if (copy_id > 1) {
        LOG_CLI(("copy_id %d is out of range, either 0 or 1  only", copy_id));
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SEC_NUM_GEN_COPY_IDf, copy_id);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_CTRLr(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_rsvd_mask_set(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    COMPILER_64_SET(reg_val,rsvd_mask[1],rsvd_mask[0]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port, reg_val));

    COMPILER_64_SET(reg_val,rsvd_mask[3],rsvd_mask[2]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port, reg_val));

    return SOC_E_NONE;

}

int cprif_drv_rx_vsd_ctrl_rsvd_mask_get(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    /* get current rsvd_mask. */
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port, &reg_val));
    rsvd_mask[0] = COMPILER_64_LO(reg_val);
    rsvd_mask[1] = COMPILER_64_HI(reg_val);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port, &reg_val));
    rsvd_mask[2] = COMPILER_64_LO(reg_val);
    rsvd_mask[3] = COMPILER_64_HI(reg_val);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_ctrl_mask_set(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;

    COMPILER_64_SET(reg_val0,ctrl_mask[1],ctrl_mask[0]);
    COMPILER_64_SET(reg_val1,ctrl_mask[3],ctrl_mask[2]);

    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK0_COPY0_CTRLr(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK1_COPY0_CTRLr(unit, port,reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK0_COPY1_CTRLr(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK1_COPY1_CTRLr(unit, port,reg_val1));
    }
    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_ctrl_mask_get(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;
    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK0_COPY0_CTRLr(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK1_COPY0_CTRLr(unit, port,&reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK0_COPY1_CTRLr(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK1_COPY1_CTRLr(unit, port,&reg_val1));
    }
    ctrl_mask[0] = COMPILER_64_LO(reg_val0);
    ctrl_mask[1] = COMPILER_64_HI(reg_val0);

    ctrl_mask[2] = COMPILER_64_LO(reg_val1);
    ctrl_mask[3] = COMPILER_64_HI(reg_val1);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_config_set(int unit, int port, const cprif_cpri_vsd_config_t* config_info)
{
    uint32 copy_id=0xff;
    uint64 ctrl_mask;
    uint32 reg_val=0;

    /*
     * Clear the control mask before updating the config to stop the processing.
     */

    SOC_IF_ERROR_RETURN
        (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    COMPILER_64_ZERO(ctrl_mask);
    if (copy_id == 0){
        WRITE_CPRI_RXFRM_VSD_MASK0_COPY1_CTRLr(unit, port,ctrl_mask);
        WRITE_CPRI_RXFRM_VSD_MASK1_COPY1_CTRLr(unit, port,ctrl_mask);
        copy_id = 1; /* new copy id */
    } else { /* copy_id is 1 */
        WRITE_CPRI_RXFRM_VSD_MASK0_COPY0_CTRLr(unit, port,ctrl_mask);
        WRITE_CPRI_RXFRM_VSD_MASK1_COPY0_CTRLr(unit, port,ctrl_mask);
        copy_id = 0; /* new copy id */
    }

    SOC_IF_ERROR_RETURN
        (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, copy_id));

    /*
     * Update the vsd ctrl configuration.
     */

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));

    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_BYTE_ORDERf, config_info->byte_order);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_QUEUE_NUMf, config_info->queue_num);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_FLOW_BYTES_COPY0f, config_info->flow_bytes);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_STEP_COPY0f, config_info->sub_channel_steps);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_BYTES_COPY0f, config_info->sub_channel_num_bytes);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_SIZE_COPY0f, config_info->sub_channel_size);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_START_COPY0f, config_info->sub_channel_start);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_CTRLr(unit, port, reg_val));

    WRITE_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port,config_info->rsvd_mask[0]);
    WRITE_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port,config_info->rsvd_mask[1]);

    return SOC_E_NONE;
}



int cprif_drv_rx_vsd_ctrl_flow_config_set(int unit, int port,
                                       uint32 sector_num,
                                       const cprif_cpri_rx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_0m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_1m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_2m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->group_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GROUP_IDf, &field_buf);

    field_buf = entry->hyper_frame_number;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_INDEXf, &field_buf);

    field_buf = entry->hyper_frame_modulo;;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_MODULOf, &field_buf);

    field_buf = entry->filter_zero_data;;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, FILTER_ZERO_DATAf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, sector_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_flow_config_get(int unit, int port,
                                         uint32 sector_num,
                                          cprif_cpri_rx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_0m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_1m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_2m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , sector_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GROUP_IDf, &field_buf);
    entry->group_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_INDEXf, &field_buf);
    entry->hyper_frame_number = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_MODULOf, &field_buf);
    entry->hyper_frame_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, FILTER_ZERO_DATAf, &field_buf);
    entry->filter_zero_data = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_num_sector_set(int unit, int port,
                                               uint32 group_id,
                                               uint32 num_sector)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_NUM_SEC_TAB_CFG_0m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_1m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_2m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = num_sector;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NUM_SECf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, group_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_num_sector_get(int unit, int port,
                                               uint32 group_id,
                                               uint32* num_sector)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_NUM_SEC_TAB_CFG_0m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_1m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_2m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NUM_SECf, &field_buf);
    *num_sector = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_config_set(int unit, int port,
                                        uint32 group_id,
                                        uint32 tag_id,
                                        uint32 grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = tag_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, TAG_IDf, &field_buf);

    field_buf = grp_ptr_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GRP_PTRf, &field_buf);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, group_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_config_get(int unit, int port,
                                           uint32 group_id,
                                           uint32* tag_id,
                                           uint32* grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_NUM_SEC_TAB_CFG_0m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_1m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_2m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, TAG_IDf, &field_buf);
    *tag_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GRP_PTRf, &field_buf);
    *grp_ptr_index = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_rx_vsd_ctrl_group_assign_ptr_set(int unit, int port,
                                               uint32 ptr_index,
                                               uint32 sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = sec_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, SEC_NUMf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, ptr_index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_assign_ptr_get(int unit, int port,
                                                uint32 ptr_index,
                                                uint32* sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , ptr_index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, SEC_NUMf, &field_buf);
    *sec_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_allocate_grp_ptr (uint32* grp_ptr_mask, uint32 num_sec, uint32* ptr_index)
{
    uint16 num_entries;
    uint16 index;
    uint16 first_location;
    uint8  location_found;

    /*
     * Find the grp ptr location that can accomodate num_sec.
     */
    index = 0;
    first_location = 0;
    location_found = FALSE;
    num_entries = num_sec;

    while (num_entries && (index < CPRIF_VSD_CTRL_NUM_GROUP_PTR)) {
        num_entries = num_sec;
        /*
         * Find first available spot.
         */
        for ( ; index < CPRIF_VSD_CTRL_NUM_GROUP_PTR; index++) {
            if (CPRIF_PBMP_AVAILABLE(grp_ptr_mask, index)) {
                first_location = index;
                location_found  = TRUE;
                break;
            }
        } /* for */
        /*
         * Check that spot has enough available space.
         */
        if (location_found) {
            for ( ;(index < CPRIF_VSD_CTRL_NUM_GROUP_PTR) && (num_entries > 0) ; index++) {
                if (CPRIF_PBMP_AVAILABLE(grp_ptr_mask, index)){
                    num_entries--;
                } else {
                    location_found = FALSE;
                    break;
                }
            } /* for */
        }
    } /* while */

    if (num_entries == 0) {
        *ptr_index = first_location;
    } else {
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}


int cprif_drv_tx_vsd_ctrl_copy_id_get(int unit, int port, uint32* copy_id)
{
  uint32 reg_val=0;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    *copy_id =  soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, reg_val,
                                  TX_CWA_VSD_CTRL_COPY_IDf);

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_copy_id_set(int unit, int port, uint32 copy_id)
{
  uint32 reg_val=0;

    if (copy_id > 1) {
        LOG_CLI(("copy_id %d is out of range, either 0 or 1  only", copy_id));
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, &reg_val, TX_CWA_VSD_CTRL_COPY_IDf, copy_id);
    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_rsvd_mask_set(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    COMPILER_64_SET(reg_val,rsvd_mask[1],rsvd_mask[0]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port, reg_val));

    COMPILER_64_SET(reg_val,rsvd_mask[3],rsvd_mask[2]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK1_CTRL2r(unit, port, reg_val));

    return SOC_E_NONE;

}

int cprif_drv_tx_vsd_ctrl_rsvd_mask_get(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    /* get current rsvd_mask. */
    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port, &reg_val));
    rsvd_mask[0] = COMPILER_64_LO(reg_val);
    rsvd_mask[1] = COMPILER_64_HI(reg_val);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port, &reg_val));
    rsvd_mask[2] = COMPILER_64_LO(reg_val);
    rsvd_mask[3] = COMPILER_64_HI(reg_val);

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_ctrl_mask_set(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;

    COMPILER_64_SET(reg_val0,ctrl_mask[1],ctrl_mask[0]);
    COMPILER_64_SET(reg_val1,ctrl_mask[3],ctrl_mask[2]);

    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL0r(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL0r(unit, port,reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL1r(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL1r(unit, port,reg_val1));
    }
    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_ctrl_mask_get(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;
    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL0r(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL0r(unit, port,&reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL1r(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL1r(unit, port,&reg_val1));
    }
    ctrl_mask[0] = COMPILER_64_LO(reg_val0);
    ctrl_mask[1] = COMPILER_64_HI(reg_val0);

    ctrl_mask[2] = COMPILER_64_LO(reg_val1);
    ctrl_mask[3] = COMPILER_64_HI(reg_val1);

    return SOC_E_NONE;
}


int cprif_drv_tx_vsd_ctrl_flow_config_set(int unit, int port,
                                       uint32 sector_num,
                                       const cprif_cpri_tx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_CONFIG_CFG_0m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_1m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_2m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->hyper_frame_number;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_INDEXf, &field_buf);

    field_buf = entry->hyper_frame_modulo;;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_MODULOf, &field_buf);

    field_buf = entry->repeat_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, REPEAT_MODEf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, sector_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_flow_config_get(int unit, int port,
                                         uint32 sector_num,
                                          cprif_cpri_tx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_CONFIG_CFG_0m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_1m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_2m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , sector_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_INDEXf, &field_buf);
    entry->hyper_frame_number = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_MODULOf, &field_buf);
    entry->hyper_frame_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, REPEAT_MODEf, &field_buf);
    entry->repeat_mode = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_ctrl_section_group_num_set(int unit, int port,
                                                uint32 section,
                                                uint32 group_id)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = group_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_GROUP_IDf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, section, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_section_group_num_get(int unit, int port,
                                                uint32 section,
                                                uint32* group_id)

{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , section, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_GROUP_IDf, &field_buf);
    *group_id = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_ctrl_group_config_set(int unit, int port,
                                           uint32 group_id,
                                           uint32 num_sector,
                                           uint32 grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = num_sector;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_NUM_SECf, &field_buf);

    field_buf = grp_ptr_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_GROUP_PTRf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, group_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_group_config_get(int unit, int port,
                                           uint32 group_id,
                                           uint32* num_sector,
                                           uint32* grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_NUM_SECf, &field_buf);
    *num_sector = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_GROUP_PTRf, &field_buf);
    *grp_ptr_index = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_group_assign_ptr_set(int unit, int port,
                                               uint32 ptr_index,
                                               uint32 sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = sec_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_SECTORf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, ptr_index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_group_assign_ptr_get(int unit, int port,
                                                uint32 ptr_index,
                                                uint32* sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , ptr_index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_SECTORf, &field_buf);
    *sec_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_ctrl_config_set(int unit, int port, const cprif_cpri_vsd_config_t* config_info)
{
    uint32 copy_id=0xff;
    uint64 ctrl_mask;
    uint32 reg_val=0;

    /*
     * Clear the control mask before updating the config to stop the processing.
     */

    SOC_IF_ERROR_RETURN
        (cprif_drv_tx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    COMPILER_64_ZERO(ctrl_mask);
    if (copy_id == 0){
        WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL1r(unit, port,ctrl_mask);
        WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL1r(unit, port,ctrl_mask);
        copy_id = 1; /* new copy id */
    } else { /* copy_id is 1 */
        WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL0r(unit, port,ctrl_mask);
        WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL0r(unit, port,ctrl_mask);
        copy_id = 0; /* new copy id */
    }

    SOC_IF_ERROR_RETURN
        (cprif_drv_tx_vsd_ctrl_copy_id_set(unit, port, copy_id));

    /*
     * Update the vsd ctrl configuration.
     */

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_BYTE_ORDERf,  config_info->byte_order);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_FLOW_BYTESf,  config_info->flow_bytes);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_STEPf,  config_info->sub_channel_steps);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_BYTESf, config_info->sub_channel_num_bytes);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_SIZEf,  config_info->sub_channel_size);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_STARTf, config_info->sub_channel_start);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_CTRLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, &reg_val, TX_CWA_VSD_CTRL_QUEUE_NUMf, config_info->queue_num);
    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));

    WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port,config_info->rsvd_mask[0]);
    WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK1_CTRL2r(unit, port,config_info->rsvd_mask[0]);

    return SOC_E_NONE;
}

int cprif_drv_encap_tag_to_flow_id_set(int unit, int port,
                                       uint32 tag_id,
                                       uint32 flow_id)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = flow_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_CTL_FLOWIDf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, tag_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_tag_to_flow_id_get(int unit, int port,
                                       uint32 tag_id,
                                       uint32* flow_id)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , tag_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_CTL_FLOWIDf, &field_buf);
    *flow_id = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_raw_config_set(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_LOC_TAB_CFG_0m,
                              CPRI_VSDRAW_LOC_TAB_CFG_1m,
                              CPRI_VSDRAW_LOC_TAB_CFG_2m,
                              CPRI_VSDRAW_LOC_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->schan_start;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);

    field_buf = config->schan_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);

    field_buf = config->cw_select;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SELf, &field_buf);

    field_buf = config->cw_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_raw_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_LOC_TAB_CFG_0m,
                              CPRI_VSDRAW_LOC_TAB_CFG_1m,
                              CPRI_VSDRAW_LOC_TAB_CFG_2m,
                              CPRI_VSDRAW_LOC_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);
    config->schan_start = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);
    config->schan_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SELf, &field_buf);
    config->cw_select = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);
    config->cw_size = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_rx_vsd_raw_filter_config_set(int unit, int port,
                                        uint32 index,
                                        cprif_cpri_rx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_FLT_TAB_CFG_0m,
                              CPRI_VSDRAW_FLT_TAB_CFG_1m,
                              CPRI_VSDRAW_FLT_TAB_CFG_2m,
                              CPRI_VSDRAW_FLT_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->filter_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_FILTER_MODEf, &field_buf);

    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);

    field_buf = config->match_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MATCH_OFFSETf, &field_buf);

    field_buf = config->match_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MATCH_VALUEf, &field_buf);

    field_buf = config->match_mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MATCH_MASKf, &field_buf);

    field_buf = config->queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_raw_filter_config_get(int unit, int port,
                                           uint32 index,
                                           cprif_cpri_rx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_FLT_TAB_CFG_0m,
                              CPRI_VSDRAW_FLT_TAB_CFG_1m,
                              CPRI_VSDRAW_FLT_TAB_CFG_2m,
                              CPRI_VSDRAW_FLT_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_FILTER_MODEf, &field_buf);
    config->filter_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MATCH_OFFSETf, &field_buf);
    config->match_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MATCH_VALUEf, &field_buf);
    config->match_value = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MATCH_MASKf, &field_buf);
    config->match_mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);
    config->queue_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_raw_config_set(int unit, int port,
                                    uint32 index,
                                    cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_LOC_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->schan_start;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);

    field_buf = config->schan_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);

    field_buf = config->cw_select;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SELf, &field_buf);

    field_buf = config->cw_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_raw_config_get(int unit, int port,
                                    uint32 index,
                                    cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_LOC_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);
    config->schan_start = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);
    config->schan_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SELf, &field_buf);
    config->cw_select = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);
    config->cw_size = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_raw_filter_config_set(int unit, int port,
                                        uint32 index,
                                        cprif_cpri_tx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_MAP_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);

    field_buf = config->map_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MAP_MODEf, &field_buf);

    field_buf = config->repeat_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_REPEAT_MODEf, &field_buf);

    field_buf = config->bfn0_filter_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_BFN0_FILTER_MODEf, &field_buf);

    field_buf = config->bfn1_filter_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_BFN1_FILTER_MODEf, &field_buf);

    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);

    field_buf = config->idle_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_IDLE_VALUEf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_raw_filter_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_tx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_MAP_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);
    config->queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MAP_MODEf, &field_buf);
    config->map_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_REPEAT_MODEf, &field_buf);
    config->repeat_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_BFN0_FILTER_MODEf, &field_buf);
    config->bfn0_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_BFN1_FILTER_MODEf, &field_buf);
    config->bfn1_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_IDLE_VALUEf, &field_buf);
    config->idle_value = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_raw_radio_frame_filter_set(int unit, int port_num,
                                                uint32 bfn0_value,
                                                uint32 bfn0_mask,
                                                uint32 bfn1_value,
                                                uint32 bfn1_mask)
{
     uint64 vsd_raw_ctrl, val64;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_VSD_RAW_CTRLr(unit, port_num, &vsd_raw_ctrl));
    COMPILER_64_SET(val64, 0, bfn0_value); 
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN0_VALUEf, val64);
    COMPILER_64_SET(val64, 0, bfn0_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN0_MASKf, val64);
    COMPILER_64_SET(val64, 0, bfn1_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN1_VALUEf, val64);
    COMPILER_64_SET(val64, 0, bfn1_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN1_MASKf, val64);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_VSD_RAW_CTRLr(unit, port_num, vsd_raw_ctrl));
    return SOC_E_NONE;
}

int cprif_drv_rx_brcm_rsvd5_config_set(int unit, int port,
                                       uint32 schan_start, uint32 schan_size,
                                       uint32 queue_num, int parity_disable)
{
    uint32 reg_val=0;
    SOC_INIT_FUNC_DEFS;


    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_0f, schan_start);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_1f, schan_size);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_2f, parity_disable?1:0);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_315r(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_300r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_300r, &reg_val,
                      BRCM_RESERVED_CPM_300_0f, queue_num);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_300r(unit, port, reg_val));


exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_brcm_rsvd5_config_get(int unit, int port,
                                       uint32* schan_start, uint32* schan_size,
                                       uint32* queue_num, int* parity_disable)
{
    uint32 reg_val=0;
    uint32 field_buf=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_315r(unit, port, &reg_val));
    *schan_start = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_0f);

    *schan_size = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_1f);

    field_buf = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_2f);
    if (field_buf) {
        *parity_disable = 1;
    } else {
        *parity_disable = 0;
    }
    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_300r(unit, port, &reg_val));

    *queue_num = soc_reg_field_get(unit, BRCM_RESERVED_CPM_300r, reg_val,
                      BRCM_RESERVED_CPM_300_0f);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_brcm_rsvd5_config_set(int unit, int port,
                                       uint32 schan_start,
                                       uint32 schan_size,
                                       uint32 queue_num,
                                       int crc_enable)
{
    uint64 reg_val, val64;
    SOC_INIT_FUNC_DEFS;


    COMPILER_64_SET(val64, 0, schan_start);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_3f, val64);

    COMPILER_64_SET(val64, 0, schan_size);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_2f, val64);

    COMPILER_64_SET(val64, 0, queue_num);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_0f, val64);

    COMPILER_64_SET(val64, 0, crc_enable?1:0);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_1f, val64);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_0r(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_brcm_rsvd5_config_get(int unit, int port,
                                       uint32* schan_start,
                                       uint32* schan_size,
                                       uint32* queue_num,
                                       int* crc_enable)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_0r(unit, port, &reg_val));
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_3f);

    *schan_start = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_2f);
    *schan_size =  COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_0f);
    *queue_num =  COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_1f);
    if (!COMPILER_64_IS_ZERO(field_buf)) {
        *crc_enable = 1;
    } else {
        *crc_enable = 0;
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_gcw_config_set(int unit, int port,
                                uint32 index,
                                cprif_cpri_rx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_GCW_CFG_TAB_CFG_0m,
                              CPRI_GCW_CFG_TAB_CFG_1m,
                              CPRI_GCW_CFG_TAB_CFG_2m,
                              CPRI_GCW_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->Ns;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_NSf, &field_buf);

    field_buf = config->Xs;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_XSf, &field_buf);

    field_buf = config->Y;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_Yf, &field_buf);


    field_buf = config->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MASKf, &field_buf);

    field_buf = config->filter_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_FILTER_MODEf, &field_buf);


    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_MODULOf, &field_buf);

    field_buf = config->match_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MATCH_VALUEf, &field_buf);

    field_buf = config->match_mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MATCH_MASKf, &field_buf);



    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_gcw_config_get(int unit, int port,
                                uint32 index,
                                cprif_cpri_rx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_GCW_CFG_TAB_CFG_0m,
                              CPRI_GCW_CFG_TAB_CFG_1m,
                              CPRI_GCW_CFG_TAB_CFG_2m,
                              CPRI_GCW_CFG_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_NSf, &field_buf);
    config->Ns = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_XSf, &field_buf);
    config->Xs = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_Yf, &field_buf);
    config->Y = field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MASKf, &field_buf);
    config->mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_FILTER_MODEf, &field_buf);
    config->filter_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MATCH_VALUEf, &field_buf);
    config->match_value  = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MATCH_MASKf, &field_buf);
    config->match_mask = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_config_set(int unit, int port,
                                uint32 index,
                                cprif_cpri_tx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_GCW_CFG_TAB_CFG_0m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_1m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_2m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->Ns;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_NSf, &field_buf);

    field_buf = config->Xs;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_XSf, &field_buf);

    field_buf = config->Y;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_Yf, &field_buf);


    field_buf = config->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MASKf, &field_buf);

    field_buf = config->repeat_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_REPEAT_MODEf, &field_buf);

    field_buf = config->bfn0_filter_enable;
    soc_mem_field_get(unit, local_mem, wr_data_ptr, GCW_BFN0_FILTER_MODEf, &field_buf);

    field_buf = config->bfn1_filter_enable;
    soc_mem_field_get(unit, local_mem, wr_data_ptr, GCW_BFN1_FILTER_MODEf, &field_buf);


    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_MODULOf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_config_get(int unit, int port,
                                uint32 index,
                                cprif_cpri_tx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_GCW_CFG_TAB_CFG_0m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_1m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_2m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_NSf, &field_buf);
    config->Ns = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_XSf, &field_buf);
    config->Xs = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_Yf, &field_buf);
    config->Y = field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MASKf, &field_buf);
    config->mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_REPEAT_MODEf, &field_buf);
    config->repeat_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_BFN0_FILTER_MODEf, &field_buf);
    config->bfn0_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_BFN1_FILTER_MODEf, &field_buf);
    config->bfn1_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_filter_set(int unit, int port,
                                   uint32 bfn0_value, uint32 bfn0_mask,
                                   uint32 bfn1_value, uint32 bfn1_mask)
{
    uint64 gcw_ctrl, val64;

    COMPILER_64_ZERO(gcw_ctrl);
    COMPILER_64_SET(val64, 0, bfn0_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN0_VALUEf, val64);

    COMPILER_64_SET(val64, 0, bfn0_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN0_MASKf, val64);

    COMPILER_64_SET(val64, 0, bfn1_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN1_VALUEf, val64);

    COMPILER_64_SET(val64, 0, bfn1_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN1_MASKf, val64);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GCWr(unit, port, gcw_ctrl));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_word_set(int unit, int port,
                                 cprif_drv_tx_control_word_t* control)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_L1r(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_POINTER_Pf, control->enet_ptr );
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_L1_FUNCf, control->l1_fun);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_HDLC_BITRATEf, control->hdlc_rate);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_PROTOCOLf,  control->protocol_ver);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_L1r(unit, port, reg_val));
    return SOC_E_NONE;
}
/* RSVD4 */
int cprif_drv_rsvd4_rx_control_message_config_set( int unit, int port,
                                                   uint32 sm_queue_num,
                                                   uint32 default_tag,
                                                   uint32 no_match_tag)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_300r(unit, port, &reg_val));
    
    soc_reg_field_set(unit, BRCM_RESERVED_CPM_300r, &reg_val,
                      BRCM_RESERVED_CPM_300_0f, sm_queue_num);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_300r(unit, port, reg_val));


    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_318r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_1f,  default_tag);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_2f, no_match_tag);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_318r(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_control_flow_config_set ( int unit, int port,
                                             uint32 control_flow_id,
                                             cprif_rsvd4_rx_ctrl_flow_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_339m,
                              BRCM_RESERVED_CPM_340m,
                              BRCM_RESERVED_CPM_341m,
                              BRCM_RESERVED_CPM_342m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->proc_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_0f, &field_buf);

    field_buf = config->queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_1f, &field_buf);

    field_buf = config->sync_profile;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_2f, &field_buf);


    field_buf = config->sync_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_3f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, control_flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_control_flow_config_get( int unit, int port,
                                                uint32 control_flow_id,
                                                cprif_rsvd4_rx_ctrl_flow_config_t* config)

{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_339m,
                              BRCM_RESERVED_CPM_340m,
                              BRCM_RESERVED_CPM_341m,
                              BRCM_RESERVED_CPM_342m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, control_flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_0f, &field_buf);
    config->proc_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_1f, &field_buf);
    config->queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_2f, &field_buf);
    config->sync_profile= field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_3f, &field_buf);
    config->sync_enable= field_buf;


exit:
    sal_free(rd_data_ptr);
   SOC_FUNC_RETURN;
}

int cprif_drv_rx_control_flow_tag_option_set(int unit, int port,
                                             uint32 control_flow_id,
                                             uint32 tag_option)
{
    uint32 reg_val;
    uint32 tag_opt;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_318r(unit, port, &reg_val));

    tag_opt = soc_reg_field_get(unit, BRCM_RESERVED_CPM_318r, reg_val,
                      BRCM_RESERVED_CPM_318_0f);

    if (tag_option) {
        tag_opt |=  (0x1 << control_flow_id);
    } else {
        tag_opt &= ~(0x1 << control_flow_id);
    }

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_0f, tag_opt);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_318r(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_sync_profile_entry_set (int unit, int port,
                                          uint32 profile_id,
                                          cprif_rsvd4_rx_sync_profile_entry_t* profile)
{
    uint64 reg_val, val64;

    COMPILER_64_SET(val64, 0, profile->bfn_offset);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_319r, &reg_val,
                        BRCM_RESERVED_CPM_319_0f, val64);

    COMPILER_64_SET(val64, 0, profile->rfrm_offset);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_319r, &reg_val,
                        BRCM_RESERVED_CPM_319_0f, val64);

    COMPILER_64_SET(val64, 0, profile->count_cycle);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_319r, &reg_val,
                        BRCM_RESERVED_CPM_319_0f, val64);
    switch (profile_id) {

        case 0 :
            WRITE_BRCM_RESERVED_CPM_319r(unit, port, reg_val);
            break;
        case 1 :
            WRITE_BRCM_RESERVED_CPM_320r(unit, port, reg_val);
            break;
        case 2 :
            WRITE_BRCM_RESERVED_CPM_321r(unit, port, reg_val);
            break;
        case 3 :
            WRITE_BRCM_RESERVED_CPM_322r(unit, port, reg_val);
            break;
        default:
            LOG_CLI(("profile_id %d is out of range, between 0-3 ", profile_id));
            return SOC_E_PARAM;
            break;
    }
    return SOC_E_NONE;

}

int cprif_drv_rx_sync_profile_entry_get (int unit, int port,
                                          uint32 profile_id,
                                          cprif_rsvd4_rx_sync_profile_entry_t* profile)
{
    uint64 reg_val;
    uint64 field_buf;

    COMPILER_64_SET(reg_val, 0, 0);
    COMPILER_64_SET(field_buf, 0, 0);

    switch (profile_id) {

        case 0 :
            WRITE_BRCM_RESERVED_CPM_319r(unit, port, reg_val);
            break;
        case 1 :
            WRITE_BRCM_RESERVED_CPM_320r(unit, port, reg_val);
            break;
        case 2 :
            WRITE_BRCM_RESERVED_CPM_321r(unit, port, reg_val);
            break;
        case 3 :
            WRITE_BRCM_RESERVED_CPM_322r(unit, port, reg_val);
            break;
        default:
            LOG_CLI(("profile_id %d is out of range, between 0-3 ", profile_id));
            return SOC_E_PARAM;
            break;
    }
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_319r, reg_val,
                        BRCM_RESERVED_CPM_319_0f);

    profile->bfn_offset = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_319r, reg_val,
                        BRCM_RESERVED_CPM_319_0f);
    profile->rfrm_offset = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_319r, reg_val,
                        BRCM_RESERVED_CPM_319_0f);
    profile->count_cycle = COMPILER_64_LO(field_buf);

    return SOC_E_NONE;

}

int cprif_drv_rx_tag_config_set ( int unit, int port,
                                  uint32 default_tag,
                                  uint32 no_match_tag)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_318r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_1f, default_tag);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_2f, no_match_tag);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_318r(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_rx_tag_gen_entry_set( int unit, int port,
                                    uint32 index,
                                    cprif_rx_tag_gen_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_335m,
                              BRCM_RESERVED_CPM_336m,
                              BRCM_RESERVED_CPM_337m,
                              BRCM_RESERVED_CPM_338m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->valid;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_0f, &field_buf);

    field_buf = entry->header;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_1f, &field_buf);

    field_buf = entry->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_2f, &field_buf);


    field_buf = entry->tag_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_3f, &field_buf);

    field_buf = entry->rtwp_word_count;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_4f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_tag_gen_entry_get( int unit, int port,
                                    uint32 index,
                                    cprif_rx_tag_gen_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_335m,
                              BRCM_RESERVED_CPM_336m,
                              BRCM_RESERVED_CPM_337m,
                              BRCM_RESERVED_CPM_338m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_0f, &field_buf);
    entry->valid = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_1f, &field_buf);
    entry->header = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_2f, &field_buf);
    entry->mask = field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_3f, &field_buf);
    entry->tag_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_4f, &field_buf);
    entry->rtwp_word_count = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_tx_control_group_entry_set( int unit, int port,
                                                  uint32 index,
                                                  cprif_control_group_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_56m,
                              BRCM_RESERVED_CPM_57m,
                              BRCM_RESERVED_CPM_58m,
                              BRCM_RESERVED_CPM_59m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->valid_mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_8f , &field_buf);

    field_buf = entry->queue_num[0];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_7f, &field_buf);

    field_buf = entry->queue_num[1];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_6f, &field_buf);

    field_buf = entry->queue_num[2];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_5f, &field_buf);

    field_buf = entry->queue_num[3];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_4f, &field_buf);

    field_buf = entry->proc_type[0];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_3f, &field_buf);

    field_buf = entry->proc_type[1];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_2f, &field_buf);

    field_buf = entry->proc_type[2];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_1f, &field_buf);

    field_buf = entry->proc_type[3];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_0f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_tx_control_group_entry_get( int unit, int port,
                                                  uint32 index,
                                                  cprif_control_group_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_56m,
                              BRCM_RESERVED_CPM_57m,
                              BRCM_RESERVED_CPM_58m,
                              BRCM_RESERVED_CPM_59m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_8f, &field_buf);
    entry->valid_mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_7f, &field_buf);
    entry->queue_num[0] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_6f, &field_buf);
    entry->queue_num[1] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_5f, &field_buf);
    entry->queue_num[2] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_4f, &field_buf);
    entry->queue_num[3] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_3f, &field_buf);
    entry->proc_type[0] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_2f, &field_buf);
    entry->proc_type[1] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_1f, &field_buf);
    entry->proc_type[2] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_0f, &field_buf);
    entry->proc_type[3] = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_control_eth_msg_config_set(int unit, int port,
                                            uint32 msg_node,
                                            uint32 msg_subnode,
                                            uint32 msg_type,
                                            uint32 msg_padding)
{
  uint64 reg_val, val64;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));
    
    COMPILER_64_SET(val64, 0, msg_node);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_3f, val64);
    COMPILER_64_SET(val64, 0,msg_subnode);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_2f, val64);
    COMPILER_64_SET(val64, 0, msg_type);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_1f, val64);
    COMPILER_64_SET(val64, 0, msg_padding);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_0f, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_5r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_eth_msg_config_get(int unit, int port,
                                            uint32* msg_node,
                                            uint32* msg_subnode,
                                            uint32* msg_type,
                                            uint32* msg_padding)
{
  uint64 reg_val;
  uint64 field_buf;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_3f);
    *msg_node = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_2f);
    *msg_subnode = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_1f);
    *msg_type=COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_0f);
    *msg_padding =COMPILER_64_LO(field_buf);

    return SOC_E_NONE;
}

int cprif_drv_tx_control_single_msg_config_set(int unit, int port,
                                            uint32 msg_id,
                                            uint32 msg_type)
{
  uint64 reg_val, val64;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, msg_id);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_4f, val64);
    COMPILER_64_SET(val64, 0, msg_type);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_5f, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_5r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_single_tunnel_msg_config_set(int unit, int port,
                                            uint32 crc_option)
{
  uint64 reg_val, val64;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, crc_option);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_6f, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_5r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_single_msg_config_get(int unit, int port,
                                            uint32* msg_id,
                                            uint32* msg_type)
{
  uint64 reg_val;
  uint64 field_buf;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_4f);
    *msg_id = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_5f);
    *msg_type = COMPILER_64_LO(field_buf);

    return SOC_E_NONE;
}


int cprif_drv_brcm_rsvd4_tx_control_flow_config_set( int unit, int port,
                                                 uint32 flow_id,
                                                 uint32 queue_num,
                                                 uint32 proc_type)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_52m,
                              BRCM_RESERVED_CPM_53m,
                              BRCM_RESERVED_CPM_54m,
                              BRCM_RESERVED_CPM_55m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);


    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_52_0f, &field_buf);


    field_buf = proc_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_52_1f,&field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_tx_control_flow_config_get( int unit, int port,
                                                 uint32 flow_id,
                                                 uint32* queue_num,
                                                 uint32* proc_type)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_52m,
                              BRCM_RESERVED_CPM_53m,
                              BRCM_RESERVED_CPM_54m,
                              BRCM_RESERVED_CPM_55m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_52_0f, &field_buf);
    *queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_52_1f, &field_buf);
    *proc_type = field_buf;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_flow_header_index_set( int unit, int port,
                                                        uint32 roe_flow_id,
                                                        uint32 header_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_44m,
                              BRCM_RESERVED_CPM_45m,
                              BRCM_RESERVED_CPM_46m,
                              BRCM_RESERVED_CPM_47m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);


    field_buf = header_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_44_0f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, roe_flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_flow_header_index_get( int unit, int port,
                                                        uint32 roe_flow_id,
                                                        uint32* header_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_44m,
                              BRCM_RESERVED_CPM_45m,
                              BRCM_RESERVED_CPM_46m,
                              BRCM_RESERVED_CPM_47m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, roe_flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_44_0f, &field_buf);
    *header_index= field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_header_entry_set( int unit, int port,
                                                   uint32 index,
                                                   uint32 header_node,
                                                   uint32 header_subnode,
                                                   uint32 node)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_48m,
                              BRCM_RESERVED_CPM_49m,
                              BRCM_RESERVED_CPM_50m,
                              BRCM_RESERVED_CPM_51m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = (header_node << 5)|header_subnode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_48_1f, &field_buf);

    field_buf = node;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_48_0f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_header_entry_get( int unit, int port,
                                                   uint32 index,
                                                   uint32* header_node,
                                                   uint32* header_subnode,
                                                   uint32* node)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_48m,
                              BRCM_RESERVED_CPM_49m,
                              BRCM_RESERVED_CPM_50m,
                              BRCM_RESERVED_CPM_51m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_48_1f, &field_buf);
    *header_node = field_buf >> 5;
    *header_subnode = field_buf & 0x1F;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_48_1f, &field_buf);
    *node = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_stuff_rsrv_bits_set( int unit, int port,
                                   int roe_stuffing_bit,
                                   int roe_reserved_bit)
{
    uint32 data32;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_GENERAL_CTRLr(unit, port,
                                                      &data32));
    soc_reg_field_set(unit, CPRI_TXFRM_GENERAL_CTRLr, &data32, CA_STUFF_BITf,
                      roe_stuffing_bit);
    soc_reg_field_set(unit, CPRI_TXFRM_GENERAL_CTRLr, &data32, BFA_RSRV_BITf,
                      roe_reserved_bit);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_GENERAL_CTRLr(unit, port,
                                                       data32));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_reset_seq(int unit, int port, int pmd_touched)
{
    int val;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    if(pmd_touched == 0) {
        SOC_IF_ERROR_RETURN(cprif_drv_cip_top_ctrl_get(unit, port, &val));
        if(val == 0) {
            SOC_IF_ERROR_RETURN(READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 0);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, reg_val));
        } else {
            /* removing iddq from cpri side */
            SOC_IF_ERROR_RETURN(cprif_drv_pmd_iddq_set(unit, port, 0));
        }
    }
    SOC_FUNC_RETURN;
}

#endif /* CPRIMOD_CPRI_FALCON_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
