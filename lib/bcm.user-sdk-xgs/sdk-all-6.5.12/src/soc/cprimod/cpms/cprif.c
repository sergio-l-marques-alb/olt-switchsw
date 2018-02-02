/*
 *
 * $Id:$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *
 */

#include <phymod/phymod.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/esw/port.h>
#include <soc/cprimod/cprimod.h>
#include <soc/cprimod/cprimod_internal.h>
#include <soc/cprimod/cprimod_dispatch.h>
#include <soc/cprimod/cprif_drv.h>
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon2_monterey_interface.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon_api_uc_common.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon_dpll_cfg_seq.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon2_monterey_enum.h"


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef CPRIMOD_CPRI_FALCON_SUPPORT
#define CPRI_NOF_LANES_IN_CORE     4

#define CPRI_PMD_CRC_UCODE_VERIFY 1

extern unsigned char  falcon_dpll_ucode[];
extern unsigned short falcon_dpll_ucode_ver;
extern unsigned short falcon_dpll_ucode_crc;
extern unsigned short falcon_dpll_ucode_len;


int cprif_test_api_dispatch(int unit, int port, int* value)
{
    SOC_INIT_FUNC_DEFS;
    LOG_CLI((" cprif_test_api_dispatch call u=%d port=%d .\n",unit,port));
    SOC_FUNC_RETURN;

}

static
int _cprif_from_speed_toframe_len (cprimod_port_speed_t speed)
{
    int basic_frm_len = 0;

    switch (speed) {
    case cprimodSpd614p4:
        basic_frm_len = 128;        /* 16*8   */
        break;
    case cprimodSpd1228p8:
        basic_frm_len = 256;        /* 16*16  */
        break;
    case cprimodSpd2457p6:
        basic_frm_len = 512;        /* 16*32  */
        break;
    case cprimodSpd3072p0:
        basic_frm_len = 640;        /* 16*40  */
        break;
    case cprimodSpd4915p2:
        basic_frm_len = 1024;       /* 16*64  */
        break;
    case cprimodSpd6144p0:
        basic_frm_len = 1280;       /* 16*80  */
        break;
    case cprimodSpd8110p08:
        basic_frm_len = 2048;       /* 16*128 */
        break;
    case cprimodSpd9830p4:
        basic_frm_len = 2048;       /* 16*128 */
        break;
    case cprimodSpd10137p6:
        basic_frm_len = 2560;       /* 16*160 */
        break;
    case cprimodSpd12165p12:
        basic_frm_len = 3072;       /* 16*192 */
        break;
    case cprimodSpd24330p24:
        basic_frm_len = 6144;       /* 16*384 */
        break;
    default:
        basic_frm_len = 256;        /* 16*16  */
        break;
    }

    return(basic_frm_len);
}

static
int _cprif_from_speed_towd_len (cprimod_port_speed_t speed)
{

    int wd_len = 0;

    switch (speed) {
    case cprimodSpd614p4:
        wd_len = 8;
        break;
    case cprimodSpd1228p8:
        wd_len =  16;
        break;
    case cprimodSpd2457p6:
        wd_len =  32;
        break;
    case cprimodSpd3072p0:
        wd_len =  40;
        break;
    case cprimodSpd4915p2:
        wd_len =  64;
        break;
    case cprimodSpd6144p0:
        wd_len =  80;
        break;
    case cprimodSpd8110p08:
        wd_len =  80;
        break;
    case cprimodSpd9830p4:
        wd_len = 128;
        break;
    case cprimodSpd10137p6:
        wd_len = 128;
        break;
    case cprimodSpd12165p12:
        wd_len = 128;
        break;
    case cprimodSpd24330p24:
        wd_len = 128;
        break;
    default:
        wd_len =  16;
        break;
    }
    return(wd_len);
}

static
int _cprif_get_basic_frm_len(int unit, int port, int *basic_frm_len)
{
    cprimod_port_speed_t speed;
    cprimod_port_interface_type_t interface;

    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_txcpri_port_type_get(unit, port,
                                                       &interface));
    if (interface != cprimodRsvd4) {
        _SOC_IF_ERR_EXIT(cprif_drv_cpri_rxpcs_speed_get(unit, port, &speed));
        *basic_frm_len = _cprif_from_speed_toframe_len(speed);
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_basic_frame_parser_active_table_set(int unit, int port, cprimod_basic_frame_table_id_t table)
{
    cprimod_basic_frame_usage_entry_t* usage_table;
    uint8 new_active_table;
    uint8 current_active_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    SOC_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len));

    if (table == cprimod_basic_frame_table_0) {
        new_active_table = 0;
        current_active_table = 1;
    } else {
        new_active_table = 1;
        current_active_table = 0;
    }
    /*
     * Get Entries from the new_active_table/current standby table to be reprogram to
     * current_active_table/new standby table.
     */
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 new_active_table,
                                                 cprimod_dir_rx,
                                                 CPRIMOD_AXC_ID_ALL,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_active_table_set(unit, port, new_active_table));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                           current_active_table,
                                                           cprimod_dir_rx,
                                                           basic_frm_len,
                                                           usage_table,
                                                           num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, current_active_table, num_entries));
exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_parser_active_table_get(int unit, int port, cprimod_basic_frame_table_id_t* table)
{
    uint8 tmp_table;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_active_table_get(unit, port, &tmp_table));

    if (tmp_table == CPRIF_BASIC_FRAME_TABLE_0) {
        *table = cprimod_basic_frame_table_0;
    } else {
        *table = cprimod_basic_frame_table_1;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_basic_frame_parser_table_num_entries_set(int unit, int port, cprimod_basic_frame_table_id_t table, uint32 num_entries)
{
    uint8 tmp_table;

    SOC_INIT_FUNC_DEFS;

    if (table == cprimod_basic_frame_table_0) {
        tmp_table = 0;
    } else {
        tmp_table = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, tmp_table, num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_parser_table_num_entries_get(int unit, int port, cprimod_basic_frame_table_id_t table, uint32* num_entries)
{
    uint8 tmp_table;

    SOC_INIT_FUNC_DEFS;

    if (num_entries == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("num_entries NULL parameter"));
    }

    if (table == cprimod_basic_frame_table_0) {
        tmp_table = 0;
    } else {
        tmp_table = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_get(unit, port, tmp_table, num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_assembly_active_table_set(int unit, int port, cprimod_basic_frame_table_id_t table)
{
    cprimod_basic_frame_usage_entry_t* usage_table;
    uint8 new_active_table;
    uint8 current_active_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    SOC_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len));

    if (table == cprimod_basic_frame_table_0) {
        new_active_table = 0;
        current_active_table = 1;
    } else {
        new_active_table = 1;
        current_active_table = 0;
    }
    /*
     * Get Entries from the new_active_table/current standby table before activation to be reprogram to
     * current_active_table/new standby table.
     */

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 new_active_table,
                                                 cprimod_dir_tx,
                                                 CPRIMOD_AXC_ID_ALL,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_active_table_set(unit, port, new_active_table));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                           current_active_table,
                                                           cprimod_dir_tx,
                                                           basic_frm_len,
                                                           usage_table,
                                                           num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, current_active_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}


int cprif_basic_frame_assembly_active_table_get(int unit, int port, cprimod_basic_frame_table_id_t* table)
{
    uint8 tmp_table;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_active_table_get(unit, port, &tmp_table));

    if (tmp_table == 0) {
        *table = cprimod_basic_frame_table_0;
    } else {
        *table = cprimod_basic_frame_table_1;
    }

exit:
    SOC_FUNC_RETURN;
}
int cprif_basic_frame_assembly_table_num_entries_set(int unit, int port, cprimod_basic_frame_table_id_t table, uint32 num_entries)
{
    uint8 table_num;
    SOC_INIT_FUNC_DEFS;

    if (table == cprimod_basic_frame_table_0) {
        table_num = 0;
    } else {
        table_num = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, table_num, num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_assembly_table_num_entries_get(int unit, int port, cprimod_basic_frame_table_id_t table, uint32* num_entries)
{
    uint8 table_num;
    SOC_INIT_FUNC_DEFS;

    if (table == cprimod_basic_frame_table_0) {
        table_num = 0;
    } else {
        table_num = 1;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_get(unit, port, table_num, num_entries));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_add(int unit, int port,
                                  uint32 axc_id,
                                  uint32 start_bit,
                                  uint32 num_bits)
{
    cprimod_basic_frame_usage_entry_t new_entry;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0 ;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len));
    if ((axc_id >= CPRIF_MAX_NUM_OF_AXC)&&(axc_id != 0xff)) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_parser_active_table_get(unit, port,
                                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);

    new_entry.axc_id = axc_id;
    new_entry.start_bit = start_bit;
    new_entry.num_bits = num_bits;

    /*
     * Transfer HW table to usage table and also add the new
     * entry to the appropriate location.
     */

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_add(unit, port,
                                              standby_table,
                                              cprimod_dir_rx,
                                              new_entry,
                                              usage_table,
                                              CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                              &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          cprimod_dir_rx,
                                                          basic_frm_len,
                                                          usage_table,
                                                          num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, standby_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_delete(int unit, int port,
                                     uint32 axc_id)
{
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len));
    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_parser_active_table_get(unit, port,
                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_delete(unit, port,
                                                 standby_table,
                                                 cprimod_dir_rx,
                                                 axc_id,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                                 &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          cprimod_dir_rx,
                                                          basic_frm_len,
                                                          usage_table,
                                                          num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, standby_table, num_entries));


exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_clear(int unit, int port)
{
    cprif_bfa_bfp_table_entry_t table_entry;
    uint8 active_table;
    uint8 standby_table;
    int table_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_parser_active_table_get(unit, port,
                                                                    &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    /* clear the entry. */
    cprif_bfa_bfp_table_entry_t_init( &table_entry);

    for (table_index=0; table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY ; table_index++) {

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_entry_set(unit, port,
                                           standby_table,
                                           cprimod_dir_rx,
                                           table_index,
                                           &table_entry));
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_get(int unit, int port,
                                  uint32 axc_id,
                                  cprimod_basic_frame_table_id_t table,
                                  cprimod_basic_frame_usage_entry_t* usage_table,
                                  int* num_entries)
{
    uint8 tmp_table;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    if (table == cprimod_basic_frame_table_0) {
        tmp_table = CPRIF_BASIC_FRAME_TABLE_0;
    } else {
        tmp_table = CPRIF_BASIC_FRAME_TABLE_1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 tmp_table,
                                                 cprimod_dir_rx,
                                                 axc_id,
                                                 usage_table,
                                                 *num_entries,  /* table size */
                                                 num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_axc_basic_frame_add(int unit, int port, uint32 axc_id, uint32 start_bit, uint32 num_bits)
{
    cprimod_basic_frame_usage_entry_t new_entry;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0 ;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len));
    if ((axc_id >= CPRIF_MAX_NUM_OF_AXC)&&(axc_id != 0xff)) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_assembly_active_table_get(unit, port,
                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);

    new_entry.axc_id = axc_id;
    new_entry.start_bit = start_bit;
    new_entry.num_bits = num_bits;

    /*
     * Transfer HW table to usage table and also add the new
     * entry to the appropriate location.
     */

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_add(unit, port,
                                              standby_table,
                                              cprimod_dir_tx,
                                              new_entry,
                                              usage_table,
                                              CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                              &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          cprimod_dir_tx,
                                                          basic_frm_len,
                                                          usage_table,
                                                          num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, standby_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}


int cprif_tx_axc_basic_frame_delete(int unit, int port, uint32 axc_id)
{
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len));
    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_assembly_active_table_get(unit, port,
                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_delete(unit, port,
                                                 standby_table,
                                                 cprimod_dir_tx,
                                                 axc_id,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                                &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          cprimod_dir_tx,
                                                          basic_frm_len,
                                                          usage_table,
                                                          num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, standby_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_tx_axc_basic_frame_clear(int unit, int port)
{
    uint16 table_index = 0;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint8 active_table;
    uint8 standby_table;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_assembly_active_table_get(unit, port,
                                                                    &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    /* clear the entry. */
    cprif_bfa_bfp_table_entry_t_init( &table_entry);

    for (table_index=0; table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY ; table_index++) {

        _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_entry_set( unit, port, standby_table,
                                       cprimod_dir_tx, table_index, &table_entry));
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_axc_basic_frame_get(int unit, int port,
                                  uint32 axc_id,
                                  cprimod_basic_frame_table_id_t table,
                                  cprimod_basic_frame_usage_entry_t* usage_table,
                                  int* num_entries)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                table,
                                                cprimod_dir_tx,
                                                axc_id,
                                                usage_table,
                                                *num_entries,  /* table size */
                                                num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_debug(int unit, int port, uint32 axc_id,
                              cprimod_basic_frame_table_id_t rx_table,
                              cprimod_basic_frame_table_id_t tx_table,
                              uint32 flags)
{

    SOC_INIT_FUNC_DEFS;

    /*
     * If axc_id is not between 0-63, and not control AxC, it want to list all AxCs.
     */
    /* coverity[unsigned_compare:FALSE] */
    if (!((axc_id >= 0) && (axc_id <=63)) &&
       (axc_id != CPRIMOD_AXC_ID_CONTROL)) {
        axc_id = CPRIMOD_AXC_ID_ALL ;
    }


    switch (flags) {

        case CPRIMOD_DEBUG_BASIC_FRAME_RAW :
            cprif_drv_basic_frame_debug_raw (unit, port, axc_id, rx_table, tx_table);
            break;
        case CPRIMOD_DEBUG_BASIC_FRAME_USAGE :
        default:
            cprif_drv_basic_frame_debug_usage (unit, port, axc_id, rx_table, tx_table);
            break;
    }

    SOC_FUNC_RETURN;

}


/*
 *
 *    CONTAINER Mapping
 *
 */
static
int _cprif_cpri_container_map_config_set(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_cpri_container_config_t* config_info)
{
    cprif_cpri_container_map_entry_t entry;
    uint32 quotient;
    uint32 remainder;
    uint32 Na,Nc,Naxc,K,Nst,Nv; /* CPRI spec. */
    SOC_INIT_FUNC_DEFS;

    cprif_cpri_container_map_entry_t_init( &entry);

    if (config_info->map_method == cprimod_cpri_frame_map_method_1) {
        entry.map_method = CPRIF_CONTAINER_MAP_CPRI_METHOD_1;
    } else if (config_info->map_method == cprimod_cpri_frame_map_method_3) {
        entry.map_method = CPRIF_CONTAINER_MAP_CPRI_METHOD_3;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unsupported Map Method Only Method 1 and 3 are supported"));
    }

    Naxc    = config_info->Naxc;
    Nst     = config_info->Nst;
    K       = config_info->K;

    Na      = config_info->Na;
    Nc      = config_info->Nc;
    Nv      = config_info->Nv;

    entry.axc_id = axc_id;

    /* For CPRI, stuffing is always at the beginning. */
    entry.stuffing_at_end = CPRIF_CONTAINER_MAP_STUFFING_AT_BEGINNING;


    if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_1) {
        /* MAP METHOD #1 */
        entry.cblk_cnt  = K*Naxc;
        entry.naxc_cnt  = 2;
        entry.stuff_cnt = Nst;
        entry.Na        = 0;
        entry.Nv        = 0;
    } else {
        /* MAP METHOD #3 */
        entry.naxc_cnt  = Naxc; /* has to be 2M */
        entry.cblk_cnt  = K*Nc*Naxc;

        if (entry.Nv) {
            quotient  = ((K * Nc) / Nv);
            remainder = ((K * Nc) % Nv);
            entry.stuff_cnt = ((quotient & 0x00ff) << 13) | (remainder & 0x1fff);
        } else {
            entry.stuff_cnt = 0;
        }
        entry.Na        = Na;
        entry.Nv        = Nv;
    }

    entry.bfrm_offset = config_info->basic_frame_offset;
    entry.hfn_offset = config_info->hyper_frame_offset;
    entry.bfn_offset = config_info->radio_frame_offset;

    if (config_info->frame_sync_mode == cprimod_cpri_frame_sync_mode_hyper) {
        entry.hfrm_sync = 1;
        entry.rfrm_sync = 0; /* don't care */

    } else if (config_info->frame_sync_mode == cprimod_cpri_frame_sync_mode_radio) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 1;

    } else if (config_info->frame_sync_mode == cprimod_cpri_frame_sync_mode_basic) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 0;

    } else {
        /* ERROR Condition */
    }

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_container_entry_set(unit, port,
                                      dir,
                                      axc_id,
                                      &entry));
exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_cpri_container_map_config_get(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (config_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info NULL parameter"));
    }
    if (SOC_E_NONE != cprimod_cpri_container_config_t_init( config_info)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
    }

exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_rsvd4_container_map_config_set(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 flow_id,
                                             cprimod_rsvd4_container_config_t* config_info)
{
    cprif_rsvd4_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    cprif_rsvd4_container_map_entry_t_init( &entry);

    /* always this. */
    entry.map_method = CPRIF_CONTAINER_MAP_RSVD4;

    entry.axc_id = config_info->axc_id;

    /* For RSVD4, stuffing is always at the end. */
    entry.stuffing_at_end = CPRIF_CONTAINER_MAP_STUFFING_AT_END;
    entry.stuff_cnt = config_info->stuffing_cnt;

    entry.cblk_cnt = config_info->container_block_cnt;

    entry.rfrm_offset = config_info->message_number_offset;
    entry.bfn_offset = config_info->master_frame_offset;

    if ((config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeNoSync)||
        (config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgTsNoSync)) {
        entry.hfrm_sync = 1;
        entry.rfrm_sync = 0; /* don't care */
    } else if ((config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgOffset)||
               (config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgTsMsgOffset)) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 1;
    } else if ((config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgAndMasterOffset)||
               (config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgTsMsgAndMasterOffset)) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 0;
    }

    /*
     * if GSM mode and rx dirction and any one of msg_ts mode.
     */

    if (
        ((config_info->msg_ts_mode==cprimod_rsvd4_msg_ts_mode_GSM_DL)||
         (config_info->msg_ts_mode==cprimod_rsvd4_msg_ts_mode_GSM_UL)) &&
        (dir == cprimod_dir_rx) &&
        ((config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgTsNoSync)||
         (config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgTsMsgOffset)||
         (config_info->frame_sync_mode == cprimodRsvd4FrameSyncModeMsgTsMsgAndMasterOffset))
      ) {
        entry.msg_ts_sync = 1;
    }

    if (config_info->msg_ts_mode == cprimod_rsvd4_msg_ts_mode_WCDMA) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_WCDMA;
    } else if (config_info->msg_ts_mode == cprimod_rsvd4_msg_ts_mode_LTE) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_LTE;
    } else if (config_info->msg_ts_mode == cprimod_rsvd4_msg_ts_mode_GSM_DL) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_DL;
    }else if (config_info->msg_ts_mode == cprimod_rsvd4_msg_ts_mode_GSM_UL) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_UL;
    }

    entry.msg_ts_cnt = config_info->msg_ts_cnt;

    if (dir == cprimod_dir_tx) {
        entry.use_ts_dbm = config_info->use_ts_dbm;

        if (entry.use_ts_dbm) {
            entry.ts_dbm_prof_num = config_info->ts_dbm_prof_num;
            entry.num_active_slots = config_info->num_active_slots;
        }
        entry.msg_addr = config_info->msg_addr;
        entry.msg_type = config_info->msg_type;
        entry.msg_ts_offset = config_info->msg_ts_offset;
    }
    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_container_entry_set(unit, port,
                                                       dir,
                                                       flow_id,
                                                       &entry));
exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_rsvd4_container_map_config_get(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (config_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info NULL parameter"));
    }
    if (SOC_E_NONE != cprimod_rsvd4_container_config_t_init( config_info)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
    }

    /* Place your code here */


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_axc_container_config_set(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_set(unit, port,
                                               cprimod_dir_rx,
                                               axc_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_rx_axc_container_config_get(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    /* NOT IMPLEMENTED */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_get(unit, port,
                                               cprimod_dir_rx,
                                               axc_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_axc_container_config_set(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_set(unit, port,
                                               cprimod_dir_tx,
                                               axc_id,
                                               config_info));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_axc_container_config_get(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

   /* NOT IMPLEMENTED */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_get(unit, port,
                                               cprimod_dir_tx,
                                               axc_id,
                                               config_info));

exit:
    SOC_FUNC_RETURN;

}


int cprif_rsvd4_rx_axc_container_config_set(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_set(unit, port,
                                               cprimod_dir_rx,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_axc_container_config_get(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_get(unit, port,
                                               cprimod_dir_rx,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_axc_container_config_set(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_set(unit, port,
                                               cprimod_dir_tx,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_axc_container_config_get(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_get(unit, port,
                                               cprimod_dir_tx,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

/*
 *
 *    IQ Packing/Unpacking
 *
 */

int cprif_rx_roe_payload_size_set(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    entry.payload_size      = config_info->packet_size;
    entry.last_packet_num   = config_info->last_packet_num;
    entry.last_payload_size = config_info->last_packet_size;

    _SOC_IF_ERR_EXIT (cprif_drv_iq_buffer_config_set(unit, port,
                                                 cprimod_dir_rx,
                                                 axc_id,
                                                 &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_roe_payload_size_get(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                cprimod_dir_rx,
                                                axc_id,
                                                &entry));

    config_info->packet_size        = entry.payload_size;
    config_info->last_packet_num    = entry.last_packet_num;
    config_info->last_packet_size   = entry.last_payload_size;

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_roe_payload_size_set(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    entry.payload_size      = config_info->packet_size;
    entry.last_packet_num   = config_info->last_packet_num;
    entry.last_payload_size = config_info->last_packet_size;

    _SOC_IF_ERR_EXIT (cprif_drv_iq_buffer_config_set(unit, port,
                                                 cprimod_dir_tx,
                                                 axc_id,
                                                 &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_roe_payload_size_get(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                cprimod_dir_tx,
                                                axc_id,
                                                &entry));

    config_info->packet_size        = entry.payload_size;
    config_info->last_packet_num    = entry.last_packet_num;
    config_info->last_packet_size   = entry.last_payload_size;

exit:
    SOC_FUNC_RETURN;

}

/*
 *
 * ENCAP/DECAP  DATA Configuration.
 *
 */
int cprif_cpri_encap_data_config_set(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    uint16 queue_offset=0;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_decap_data_entry_t_init( &entry);

    entry.valid = 1;
    entry.sample_size = config_info->sample_size;
    entry.out_sample_size = entry.sample_size;

    /* for 16 bits  to 15 bits, process before compression. */

    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_16_to_15)||
            (config_info->truncation_type == cprimod_truncation_type_add_1)) {
            if (entry.out_sample_size == 16) {
                entry.out_sample_size = 15;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_15_to_16) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_16_TO_15;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_1;
                }

            } else {
                LOG_CLI(("truncation can only apply to 16 bit sample.\n"));
            }
        }
    }

    if (config_info->compression_type == cprimod_decompress) {
        if (entry.out_sample_size == 9) {
            entry.out_sample_size = 15;
        } else {
            LOG_CLI(("decompression can only apply to 9 bits sample.\n"));
        }
    } else if (config_info->compression_type == cprimod_compress) {

        if (entry.out_sample_size == 15) {
                entry.out_sample_size = 9;
        } else {
                LOG_CLI(("Compression can only be done on 15 bit samples."));
        }
    }

    /* for 15 bits  to 16 bits, process after compression. */
    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_15_to_16)||
            (config_info->truncation_type == cprimod_truncation_type_add_0)) {
            if (entry.out_sample_size == 15) {
                entry.out_sample_size = 16;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_15_to_16) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_15_TO_16;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_0;
                }
            } else {
                LOG_CLI(("15 to 16 or add 0  truncation can only apply to 15 bit sample.\n"));
            }
        }
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_data_allocate_buffer(unit, port,
                                              cprimod_dir_rx,
                                              config_info->buffer_size,
                                              &queue_offset));

    entry.mux_enable        = config_info->mux_enable;
    entry.queue_offset      = queue_offset;
    /*
     * Convert the number of bytes into number of 16 bytes entries.
     */
    entry.queue_size        = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size);
    entry.work_queue_select = config_info->priority;
    entry.bit_reversal      = config_info->bit_reversal;

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_set(unit, port,
                                                 cprimod_dir_rx,
                                                 queue_num,
                                                 &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_encap_data_config_get(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_get(unit, port,
                                                 cprimod_dir_rx,
                                                 queue_num,
                                                 &entry));
    config_info->sample_size = entry.sample_size;
    config_info->truncation_enable = entry.sign_ext_enable;
    if (config_info->truncation_enable) {
        if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_15_TO_16) {
            config_info->truncation_type = cprimod_truncation_type_15_to_16;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_0) {
            config_info->truncation_type = cprimod_truncation_type_add_0;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_16_TO_15) {
            config_info->truncation_type = cprimod_truncation_type_16_to_15;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_1) {
            config_info->truncation_type = cprimod_truncation_type_add_1;
        }
    }

    if ((entry.out_sample_size == 9) && (entry.sample_size > 9)) {
        config_info->compression_type = cprimod_decompress;
    } else if ((entry.out_sample_size >= 15) &&
       (entry.sample_size < 15)) {
        config_info->compression_type = cprimod_compress;
    }

    config_info->mux_enable = entry.mux_enable;
    config_info->bit_reversal = entry.bit_reversal;
    /*
     * Convert number of 16 bytes entries into bytes.
     */
    config_info->buffer_size  = entry.queue_size * CPRIF_DATA_BUFFER_BLOCK_SIZE;
    config_info->priority     = entry.work_queue_select;

exit:
    SOC_FUNC_RETURN;

}


int cprif_cpri_decap_data_config_set(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    uint16 queue_offset =0;
    uint32 buffer_size_in_bytes;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_decap_data_entry_t_init( &entry);

    entry.valid             = 1;
    entry.sample_size       = config_info->sample_size;
    entry.out_sample_size   = entry.sample_size;

    /* for 16 bits  to 15 bits, process before compression. */

    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_16_to_15)||
            (config_info->truncation_type == cprimod_truncation_type_add_1)) {
            if (entry.out_sample_size == 16) {
                entry.out_sample_size = 15;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_15_to_16) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_16_TO_15;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_1;
                }

            } else {
                LOG_CLI(("truncation can only apply to 16 bit sample.\n"));
            }
        }
    }

    if (config_info->compression_type == cprimod_decompress) {
        if (entry.out_sample_size == 9) {
            entry.out_sample_size = 15;
        } else {
            LOG_CLI(("decompression can only apply to 9 bits sample.\n"));
        }
    } else if (config_info->compression_type == cprimod_compress) {

        if (entry.out_sample_size == 15) {
                entry.out_sample_size = 9;
        } else {
                LOG_CLI(("Compression can only be done on 15 bit samples."));
        }
    }

    /* for 15 bits  to 16 bits, process after compression. */
    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_15_to_16)||
            (config_info->truncation_type == cprimod_truncation_type_add_0)) {
            if (entry.out_sample_size == 15) {
                entry.out_sample_size = 16;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_15_to_16) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_15_TO_16;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_0;
                }
            } else {
                LOG_CLI(("15 to 16 or add 0  truncation can only apply to 15 bit sample.\n"));
            }
        }
    }

    /*
     * Calculate space for the buffer.
     */

    buffer_size_in_bytes = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size) * CPRIF_DATA_BUFFER_BLOCK_SIZE * config_info->cycle_size;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_data_allocate_buffer(unit, port,
                                              cprimod_dir_tx,
                                              buffer_size_in_bytes,
                                              &queue_offset));

    entry.mux_enable    = config_info->mux_enable;
    entry.queue_offset  = queue_offset; /* Memory management Here. */
    /*
     * Conver Buffer Size into number of 16 bits entries.
     */
    entry.queue_size    = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size);
    entry.tx_cycle_size = config_info->cycle_size;
    entry.bit_reversal  = config_info->bit_reversal;

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_set(unit, port,
                                                 cprimod_dir_tx,
                                                 queue_num,
                                                 &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_decap_data_config_get(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_get(unit, port,
                                                 cprimod_dir_tx,
                                                 queue_num,
                                                 &entry));
    config_info->sample_size = entry.sample_size;
    config_info->truncation_enable = entry.sign_ext_enable;
    if (config_info->truncation_enable) {
        if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_15_TO_16) {
            config_info->truncation_type = cprimod_truncation_type_15_to_16;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_0) {
            config_info->truncation_type = cprimod_truncation_type_add_0;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_16_TO_15) {
            config_info->truncation_type = cprimod_truncation_type_16_to_15;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_1) {
            config_info->truncation_type = cprimod_truncation_type_add_1;
        }
    }

    if ((entry.out_sample_size == 9) && (entry.sample_size > 9)) {
        config_info->compression_type = cprimod_decompress;
    } else if ((entry.out_sample_size >= 15) &&
       (entry.sample_size < 15)) {
        config_info->compression_type = cprimod_compress;
    }

    config_info->mux_enable     = entry.mux_enable;
    config_info->bit_reversal   = entry.bit_reversal;
    /*
     * Convert number of 16 bytes entries into bytes.
     */
    config_info->buffer_size    = entry.queue_size * CPRIF_DATA_BUFFER_BLOCK_SIZE ;
    config_info->cycle_size     = entry.tx_cycle_size;

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_encap_data_config_set(int unit, int port, uint32 queue_num, cprimod_rsvd4_encap_data_config_t* config_info)
{
    uint16 queue_offset=0;
    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    entry.valid                 = 1;
    entry.gsm_pad_size          = config_info->gsm_pad_size;
    entry.gsm_extra_pad_size    = config_info->gsm_extra_pad_size;
    entry.gsm_pad_enable        = config_info->gsm_pad_enable;
    entry.gsm_control_location  = config_info->gsm_control_location;
    entry.queue_size            = config_info->buffer_size;
    entry.work_queue_select     = config_info->priority;

    _SOC_IF_ERR_EXIT (cprif_drv_encap_data_allocate_buffer(unit, port,
                                                           cprimod_dir_rx,
                                                           config_info->buffer_size,
                                                           &queue_offset));
    entry.queue_offset          = queue_offset;

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_set(unit, port,
                                                       cprimod_dir_rx,
                                                       queue_num,
                                                       &entry));

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_encap_data_config_get(int unit, int port, uint32 queue_num, cprimod_rsvd4_encap_data_config_t* config_info)
{

    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_get(unit, port,
                                                       cprimod_dir_rx,
                                                       queue_num,
                                                       &entry));

    config_info->gsm_pad_size          = entry.gsm_pad_size;
    config_info->gsm_extra_pad_size    = entry.gsm_extra_pad_size;
    config_info->gsm_pad_enable        = entry.gsm_pad_enable;
    config_info->gsm_control_location  = entry.gsm_control_location;
    config_info->buffer_size           = entry.queue_size;
    config_info->priority              = entry.work_queue_select;


exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_decap_data_config_set(int unit, int port, uint32 queue_num, cprimod_rsvd4_decap_data_config_t* config_info)
{
    uint16 queue_offset =0;
    uint32 buffer_size_in_bytes;
    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    entry.valid                 = 1;
    entry.gsm_pad_size          = config_info->gsm_pad_size;
    entry.gsm_extra_pad_size    = config_info->gsm_extra_pad_size;
    entry.gsm_pad_enable        = config_info->gsm_pad_enable;
    entry.gsm_control_location  = config_info->gsm_control_location;
    entry.queue_size            = config_info->buffer_size;
    entry.tx_cycle_size         = config_info->cycle_size;

    buffer_size_in_bytes = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size)
                         * CPRIF_DATA_BUFFER_BLOCK_SIZE
                         * config_info->cycle_size;
    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_allocate_buffer(unit, port,
                                                          cprimod_dir_tx,
                                                          buffer_size_in_bytes,
                                                          &queue_offset));
    entry.queue_offset          = queue_offset;

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_set(unit, port,
                                                       cprimod_dir_tx,
                                                       queue_num,
                                                       &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_decap_data_config_get(int unit, int port, uint32 queue_num, cprimod_rsvd4_decap_data_config_t* config_info)
{
    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_get(unit, port,
                                                       cprimod_dir_tx,
                                                       queue_num,
                                                       &entry));

    config_info->gsm_pad_size          = entry.gsm_pad_size;
    config_info->gsm_extra_pad_size    = entry.gsm_extra_pad_size;
    config_info->gsm_pad_enable        = entry.gsm_pad_enable;
    config_info->gsm_control_location  = entry.gsm_control_location;
    config_info->buffer_size           = entry.queue_size;
    config_info->cycle_size            = entry.tx_cycle_size;


exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_header_config_set(int unit, int port, uint32 queue_num, cprimod_encap_header_config_t* config_info)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    if (config_info->header_type == cprimod_hdr_encap_type_roe) {
        entry.header_type = CPRIF_ENCAP_HEADER_HDR_TYPE_ROE;
    } else if (config_info->header_type == cprimod_hdr_encap_type_encap_none) {
        entry.header_type = CPRIF_ENCAP_HEADER_HDR_TYPE_ENCAP_NONE;
    } else {
        /* not suppose to be here. */
    }

    switch (config_info->vlan_type) {
        case cprimodHdrVlanTypeUntagged :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED;
            break;

        case cprimodHdrVlanTypeTaggedVlan0 :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_0;
            break;

        case cprimodHdrVlanTypeQinQ :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_Q_IN_Q;
            break;

        case cprimodHdrVlanTypeTaggedVlan1 :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_1;
            break;
        default:
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED;
            break;
    }


    entry.roe_flow_id           = config_info->roe_flow_id;
    entry.roe_subtype           = config_info->roe_subtype;
    entry.ordering_info_index   = config_info->ordering_info_index;
    entry.mac_da_index          = config_info->mac_da_index;
    entry.mac_sa_index          = config_info->mac_sa_index;
    entry.vlan_id_0_index       = config_info->vlan_id_0_index;
    entry.vlan_id_1_index       = config_info->vlan_id_1_index;
    entry.vlan_0_priority       = config_info->vlan_0_priority;
    entry.vlan_1_priority       = config_info->vlan_1_priority;
    entry.ether_type_index      = config_info->vlan_eth_type_index;
    entry.use_tagid_for_flowid  = config_info->use_tagid_for_flowid;
    entry.use_tagid_for_vlan    = config_info->use_tagid_for_vlan;
    entry.use_opcode            = config_info->use_opcode;
    entry.roe_opcode            = config_info->roe_opcode;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_set(unit, port,
                                          queue_num,
                                          CPRIF_ENCAP_HEADER_FLAGS_HDR_CONFIG_SET,
                                          &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_header_config_get(int unit, int port, uint32 queue_num, cprimod_encap_header_config_t* config_info)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_header_entry_t_init(&entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_get(unit, port,
                                          queue_num,
                                          &entry));

    if (entry.header_type == CPRIF_ENCAP_HEADER_HDR_TYPE_ROE) {
        config_info->header_type = cprimod_hdr_encap_type_roe;
    } else if (entry.header_type == CPRIF_ENCAP_HEADER_HDR_TYPE_ENCAP_NONE) {
        config_info->header_type = cprimod_hdr_encap_type_encap_none;
    }
    switch (entry.vlan_type) {
        case CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED:
            config_info->vlan_type = cprimodHdrVlanTypeUntagged;
            break;

        case CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_0:
            config_info->vlan_type = cprimodHdrVlanTypeTaggedVlan0;
            break;

        case CPRIF_ENCAP_HEADER_VLAN_TYPE_Q_IN_Q:
            config_info->vlan_type = cprimodHdrVlanTypeQinQ;
            break;

        case CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_1:
            config_info->vlan_type = cprimodHdrVlanTypeTaggedVlan1;
            break;
        default:
            config_info->vlan_type = cprimodHdrVlanTypeUntagged; /* ???? */
            break;
    }

    config_info->roe_flow_id            = entry.roe_flow_id         ;
    config_info->roe_subtype            = entry.roe_subtype         ;
    config_info->ordering_info_index    = entry.ordering_info_index ;
    config_info->mac_da_index           = entry.mac_da_index        ;
    config_info->mac_sa_index           = entry.mac_sa_index        ;
    config_info->vlan_id_0_index        = entry.vlan_id_0_index     ;
    config_info->vlan_id_1_index        = entry.vlan_id_1_index     ;
    config_info->vlan_0_priority        = entry.vlan_0_priority  ;
    config_info->vlan_1_priority        = entry.vlan_1_priority  ;
    config_info->vlan_eth_type_index    = entry.ether_type_index    ;
    config_info->use_tagid_for_flowid   = entry.use_tagid_for_flowid;
    config_info->use_tagid_for_vlan     = entry.use_tagid_for_vlan  ;
    config_info->use_opcode             = entry.use_opcode;
    config_info->roe_opcode             = entry.roe_opcode;

exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_rsvd4_gsm_tsn_bitmap_set(int unit, int port, uint32 queue_num, uint32 tsn_bitmap)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_header_entry_t_init( &entry);

    entry.tsn_bitmap = tsn_bitmap;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_set(unit, port,
                                          queue_num,
                                          CPRIF_ENCAP_HEADER_FLAGS_GSM_CONFIG_SET,
                                          &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_rsvd4_gsm_tsn_bitmap_get(int unit, int port, uint8 queue_num, uint8* tsn_bitmap)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;


    cprif_encap_header_entry_t_init(&entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_get(unit, port,
                                          queue_num,
                                          &entry));
    *tsn_bitmap = entry.tsn_bitmap;
exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_sequence_number_size_validate(uint8 p_size, uint8 q_size)
{

    if ((p_size < 1) || (p_size > 32)) {
        LOG_CLI(("P size need to be between 1 and 32.\n"));
        return SOC_E_PARAM;
    }

    if (q_size > p_size) {
        LOG_CLI(("Q size cannot exceed P size.\n"));
        return SOC_E_PARAM;
    }

    if ((q_size+p_size)>32) {
        LOG_CLI(("P and Q cannot exceed 32.\n"));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}


int cprif_encap_ordering_info_entry_set(int unit, int port,
                                        uint32 index,
                                        const cprimod_encap_ordering_info_entry_t* user_entry)
{
    cprif_encap_ordering_info_entry_t entry;
    uint32  p_mask;
    uint32  q_mask;
    uint32  r_mask;
    uint8     r_bits;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_ordering_info_entry_t_init( &entry);

    cprif_drv_cprimod_to_cprif_ordering_info_type(user_entry->type, &entry.type);
    cprif_drv_cprimod_to_cprif_ordering_info_inc_prop(user_entry->pcnt_prop, &entry.pcnt_prop);
    cprif_drv_cprimod_to_cprif_ordering_info_inc_prop(user_entry->qcnt_prop, &entry.qcnt_prop);

    entry.p_size = user_entry->pcnt_size;
    entry.q_size = user_entry->qcnt_size;

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

        p_mask = ((0x00000001<<entry.p_size)-1);
    q_mask = ((0x00000001 << entry.q_size)-1);
    r_bits =  32 - entry.p_size - entry.q_size;
    r_mask = ((0x00000001 << r_bits)-1);

    entry.max = 0x00;
    entry.max |= (user_entry->pcnt_max & p_mask);
    entry.max |= ((user_entry->qcnt_max & q_mask) << entry.p_size);

    entry.increment = 0x00;
    entry.increment |= (user_entry->pcnt_increment & p_mask);
    entry.increment |= ((user_entry->qcnt_increment & q_mask) << entry.p_size);
    entry.increment |= ((user_entry->seq_reserve_value & r_mask) << (entry.p_size+entry.q_size));

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_ordering_info_entry_set(unit, port,
                                                 index,
                                                 &entry));


exit:
    SOC_FUNC_RETURN;

}


int cprif_encap_ordering_info_entry_get(int unit, int port, uint32 index,
                                        cprimod_encap_ordering_info_entry_t* user_entry)
{
    cprif_encap_ordering_info_entry_t entry;
    uint32 p_mask;
    uint32 q_mask;
    uint32 r_mask;
    uint8  r_bits;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_ordering_info_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_ordering_info_entry_get(unit, port,
                                                 index,
                                                 &entry));

    cprif_drv_cprif_to_cprimod_ordering_info_type(entry.type, &user_entry->type);
    cprif_drv_cprif_to_cprimod_ordering_info_inc_prop(entry.pcnt_prop,&user_entry->pcnt_prop);
    cprif_drv_cprif_to_cprimod_ordering_info_inc_prop(entry.qcnt_prop,&user_entry->qcnt_prop);

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

    user_entry->pcnt_size = entry.p_size;
    user_entry->qcnt_size = entry.q_size;

        p_mask = (0x00000001 << entry.p_size);
    q_mask = (0x00000001 << entry.q_size)-1;
    r_bits =  32 - entry.p_size - entry.q_size;
    r_mask = ((0x00000001 << r_bits)-1);


    user_entry->pcnt_max = entry.max & p_mask;
    user_entry->qcnt_max = ((entry.max >> entry.p_size) & q_mask);

    user_entry->pcnt_increment      = entry.increment & p_mask;
    user_entry->qcnt_increment      = ((entry.increment >> entry.p_size) & q_mask);
    user_entry->seq_reserve_value   = ((entry.increment >> (entry.p_size+entry.q_size)) & r_mask);

exit:
    SOC_FUNC_RETURN;

}



int cprif_decap_ordering_info_entry_set(int unit, int port, uint32 index,
                                        const cprimod_decap_ordering_info_entry_t* user_entry)
{
    cprif_decap_ordering_info_entry_t entry;
    uint32  p_mask;
    uint32  q_mask;
    uint32  r_mask;
    uint8     r_bits;

    SOC_INIT_FUNC_DEFS;
    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_decap_ordering_info_entry_t_init( &entry);

    cprif_drv_cprimod_to_cprif_ordering_info_type(user_entry->type, &entry.type);

    entry.p_size = user_entry->pcnt_size;
    entry.q_size = user_entry->qcnt_size;

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

        p_mask = ((0x00000001 << entry.p_size)-1);
    q_mask = ((0x00000001 << entry.q_size)-1);
    r_bits =  32 - entry.p_size - entry.q_size;
    r_mask = ((0x00000001 << r_bits)-1);

    entry.max           = 0x00;
    entry.max          |= (user_entry->pcnt_max & p_mask);
    entry.max          |= ((user_entry->qcnt_max & q_mask) << entry.p_size);

    entry.bias          = 0x00;
    entry.bias         |= (user_entry->pcnt_bias & p_mask);
    entry.bias         |= ((user_entry->qcnt_bias & q_mask) << entry.p_size);

    entry.increment     = 0x00;
    entry.increment    |= (user_entry->pcnt_increment & p_mask);
    entry.increment    |= ((user_entry->qcnt_increment & q_mask) << entry.p_size);
    entry.increment    |= ((user_entry->seq_reserve_value & r_mask) << (entry.p_size+entry.q_size));

    entry.pcnt_inc_p2       = user_entry->pcnt_increment_p2;
    entry.qcnt_inc_p2       = user_entry->qcnt_increment_p2;
    entry.pcnt_extended     = user_entry->pcnt_extended;
    entry.pcnt_pkt_count    = user_entry->pcnt_pkt_count;
    entry.modulo_2          = user_entry->modulo_2;

    entry.gsm_tsn_bitmap    = user_entry->gsm_tsn_bitmap;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_ordering_info_entry_set(unit, port,
                                                 index,
                                                 &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_ordering_info_entry_get(int unit, int port, uint32 index,
                                        cprimod_decap_ordering_info_entry_t* user_entry)
{
    cprif_decap_ordering_info_entry_t entry;
    uint32  p_mask;
    uint32  q_mask;
    uint32  r_mask;
    uint8     r_bits;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_decap_ordering_info_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_ordering_info_entry_get(unit, port,
                                                 index,
                                                 &entry));
    cprif_drv_cprif_to_cprimod_ordering_info_type(entry.type, &user_entry->type);

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

    user_entry->pcnt_increment_p2   = entry.pcnt_inc_p2;
    user_entry->qcnt_increment_p2   = entry.qcnt_inc_p2;
    user_entry->pcnt_extended       = entry.pcnt_extended;
    user_entry->pcnt_pkt_count      = entry.pcnt_pkt_count;
    user_entry->modulo_2            = entry.modulo_2;
    user_entry->gsm_tsn_bitmap       = entry.gsm_tsn_bitmap;

    user_entry->pcnt_size           = entry.p_size;
    user_entry->qcnt_size           = entry.q_size;

        p_mask = ((0x00000001 << entry.p_size)-1);
    q_mask = ((0x00000001 << entry.q_size)-1);
    r_bits =  32 - entry.p_size - entry.q_size;
    r_mask = ((0x00000001 << r_bits)-1);


    user_entry->pcnt_max = (entry.max & p_mask);
    user_entry->qcnt_max = ((entry.max >> entry.p_size) & q_mask);

    user_entry->pcnt_bias = (entry.bias & p_mask);
    user_entry->qcnt_bias = ((entry.bias >> entry.p_size) & q_mask);

    user_entry->pcnt_increment = (entry.increment & p_mask);
    user_entry->qcnt_increment = ((entry.increment>>entry.p_size) & q_mask);
    user_entry->seq_reserve_value = ((entry.increment >> (entry.p_size+entry.q_size)) & r_mask);

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_ordering_info_sequence_offset_set(int unit, int port, uint32 queue,
                                                  uint32 control,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;

    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }


    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(offset_entry->p_size, offset_entry->q_size));

    p_mask = 0xFFFFFFFF;
    if (offset_entry->p_size < 32) {
        p_mask = ((0x00000001 << offset_entry->p_size)-1);
    }

    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    offset  = 0;
    offset |= (offset_entry->p_offset & p_mask);
    offset |= ((offset_entry->q_offset & q_mask) << offset_entry->q_size);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_queue_ordering_info_sequence_offset_set (unit, port,
                                                                  queue, control,
                                                                  offset));


exit:
    SOC_FUNC_RETURN;

}

/* caller need to provide the p_size and q_size. */
int cprif_encap_ordering_info_sequence_offset_get(int unit, int port, uint32 queue,
                                                  uint8 control,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;

    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(_cprif_sequence_number_size_validate(offset_entry->p_size,
                                                     offset_entry->q_size));

    p_mask = ((0x00000001 << offset_entry->p_size)-1);
    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_queue_ordering_info_sequence_offset_get (unit, port,
                                                                  queue, control,
                                                                  &offset));
    offset_entry->p_offset = offset & p_mask;
    offset_entry->q_offset = ((offset >> offset_entry->p_size) & q_mask);
exit:
    SOC_FUNC_RETURN;

}


int cprif_decap_ordering_info_sequence_offset_set(int unit, int port, uint32 queue,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;
    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(offset_entry->p_size, offset_entry->q_size));

    p_mask = 0xFFFFFFFF;
    if (offset_entry->p_size < 32) {
        p_mask = ((0x00000001 << offset_entry->p_size)-1);
    }

    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    offset  = 0;
    offset |= (offset_entry->p_offset & p_mask);
    offset |= ((offset_entry->q_offset & q_mask) << offset_entry->q_size);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_ordering_info_sequence_offset_set (unit, port,
                                                                  queue,
                                                                  offset));


exit:
    SOC_FUNC_RETURN;

}

/* caller need to provide the p_size and q_size. */
int cprif_decap_ordering_info_sequence_offset_get(int unit, int port, uint32 queue,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;

    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(_cprif_sequence_number_size_validate(
                      offset_entry->p_size, offset_entry->q_size));

    p_mask = ((0x00000001 << offset_entry->p_size)-1);
    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_ordering_info_sequence_offset_get (unit, port,
                                                                  queue,
                                                                  &offset));
    offset_entry->p_offset = offset & p_mask;
    offset_entry->q_offset = ((offset >> offset_entry->p_size) & q_mask);
exit:
    SOC_FUNC_RETURN;

}


static
int _cprif_pack_mac_addr(cprimod_mac_addr_t addr, uint64 *mac_addr)
{
    int loop;
    uint64 tmp_addr;
    uint8 shift_bits;

    /* packing addr bytes */
    COMPILER_64_ZERO(*mac_addr);
    shift_bits = 0;
    for(loop = 0; loop < CPRIF_NUM_BYTES_IN_MAC_ADDR; loop++) {

        /*
         * Change shift bit formula when the packing byte order
         * need to be reverse. (loop * 8). Change both pack and unpack.
         */
        shift_bits = (CPRIF_NUM_BYTES_IN_MAC_ADDR - 1 - loop)*8;
        COMPILER_64_SET(tmp_addr, 0, (uint32) addr[loop]);
        COMPILER_64_SHL(tmp_addr, shift_bits);
        COMPILER_64_OR(*mac_addr, tmp_addr);
    }

    return SOC_E_NONE;
}

int _cprif_unpack_mac_addr(uint64 mac_addr, cprimod_mac_addr_t addr)
{
    int loop;
    uint64 tmp_addr;
    uint8 shift_bits;

    /* unpacking addr bytes */
    COMPILER_64_ZERO(mac_addr);
    for(loop = 0; loop < CPRIF_NUM_BYTES_IN_MAC_ADDR; loop++) {

        shift_bits = (CPRIF_NUM_BYTES_IN_MAC_ADDR - 1 - loop)*8;
        COMPILER_64_COPY(tmp_addr, mac_addr);
        COMPILER_64_SHR(tmp_addr, shift_bits);
        addr[loop] = COMPILER_64_LO(tmp_addr) & 0xff;
    }
    return SOC_E_NONE;
}

int cprif_encap_mac_da_entry_set(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);

    /* packing addr bytes */
    _cprif_pack_mac_addr(addr,&mac_addr);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_da_set(unit, port,
                                    index,
                                    mac_addr));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_mac_da_entry_get(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);
    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_da_get(unit, port,
                                    index,
                                    &mac_addr));

    _cprif_unpack_mac_addr(mac_addr, addr);

exit:
    SOC_FUNC_RETURN;

}


int cprif_encap_mac_sa_entry_set(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);

    /* packing addr bytes */
    _cprif_pack_mac_addr(addr,&mac_addr);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_sa_set(unit, port,
                                    index,
                                    mac_addr));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_mac_sa_entry_get(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);
    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_sa_get(unit, port,
                                    index,
                                    &mac_addr));

    _cprif_unpack_mac_addr(mac_addr, addr);

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_vlan_id_entry_set(int unit, int port, cprimod_vlan_table_id_t table_id, uint32 index, uint32 vlan_id)
{
    uint8   table_num;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_VLAN_ID_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                          index,
                          CPRIF_VLAN_ID_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    if (table_id == cprimodVlanTable0) {
        table_num = 0;
    } else {
        table_num = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_entry_set(unit, port,
                                        table_num,
                                        index,
                                        vlan_id));



exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_vlan_id_entry_get(int unit, int port, cprimod_vlan_table_id_t table_id, uint32 index, uint32* vlan_id)
{
    uint8   table_num;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_VLAN_ID_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                          index,
                          CPRIF_VLAN_ID_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    if (table_id == cprimodVlanTable0) {
        table_num = 0;
    } else {
        table_num = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_entry_get(unit, port,
                                        table_num,
                                        index,
                                        vlan_id));
exit:
    SOC_FUNC_RETURN;

}


static
void _cprif_cprimod_to_cprif_decap_ethtype_id(cprimod_ethertype_t type_id, uint8 *int_type)
{
    switch (type_id) {
        case cprimodEthertypeRoe:
            *int_type = CPRIF_DECAP_ROE_ETHTYPE;
            break;

        case cprimodEthertypeFast:
            *int_type = CPRIF_DECAP_FAST_ETH_ETHTYPE;
            break;

        case cprimodEthertypeVlan:
            *int_type = CPRIF_DECAP_VLAN_TAGGED_ETHTYPE;
            break;

        case cprimodEthertypeQinQ:
            *int_type = CPRIF_DECAP_VLAN_QINQ_ETHTYPE;
            break;

        default:
            *int_type = CPRIF_DECAP_ROE_ETHTYPE;
            break;

    }
}

int cprif_decap_ethertype_config_set(int unit, int port, cprimod_ethertype_t type_id, uint16 ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_decap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_vlan_ethtype_set(unit, port,
                                          int_type,
                                          ethertype));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_ethertype_config_get(int unit, int port, cprimod_ethertype_t type_id, uint16* ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_decap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_vlan_ethtype_get(unit, port,
                                          int_type,
                                          ethertype));

exit:
    SOC_FUNC_RETURN;

}

static
void _cprif_cprimod_to_cprif_encap_ethtype_id(cprimod_ethertype_t type_id, uint8 *int_type)
{
    switch (type_id) {
        case cprimodEthertypeRoe:
            *int_type = CPRIF_ETHTYPE_ROE_ETHTYPE_0;
            break;

        case cprimodEthertypeRoe1:
            *int_type = CPRIF_ETHTYPE_ROE_ETHTYPE_1;
            break;

        case cprimodEthertypeVlan:
            *int_type = CPRIF_ETHTYPE_VLAN_TAGGED;
            break;

        case cprimodEthertypeQinQ:
            *int_type = CPRIF_ETHTYPE_VLAN_QINQ;
            break;

        default:
            *int_type = CPRIF_ETHTYPE_ROE_ETHTYPE_0;
            break;

    }
}


int cprif_encap_ethertype_config_set(int unit, int port, cprimod_ethertype_t type_id, uint16 ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_encap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_ethtype_set(unit, port,
                                          int_type,
                                          ethertype));

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_ethertype_config_get(int unit, int port, cprimod_ethertype_t type_id, uint16* ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_encap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_ethtype_get(unit, port,
                                          int_type,
                                          ethertype));
exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_memroy_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_encap_queue_sts_clear(unit, port,
                     queue_num));
    _SOC_IF_ERR_EXIT(cprif_drv_encap_ordering_info_sts_clear(unit, port,
                     queue_num));
    _SOC_IF_ERR_EXIT(cprif_drv_encap_eth_hdr_clear(unit, port,
                     queue_num));
    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_tab_clear(unit, port,
                     queue_num));
exit:
    SOC_FUNC_RETURN;
}


int cprif_cpri_rx_framer_state_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
    /* only for data framer state queue_num <64 for data axc*/
    if (queue_num < 64) {
        _SOC_IF_ERR_EXIT(cprif_drv_container_parser_mapp_state_tab_clear(unit,
                                                               port, queue_num));
        _SOC_IF_ERR_EXIT(cprif_drv_iq_pak_buff_state_tab_clear(unit, port,
                                                               queue_num));
        _SOC_IF_ERR_EXIT(cprif_drv_iq_pak_buff_payld_tab_clear(unit, port,
                                                               queue_num));
    }
exit:
    SOC_FUNC_RETURN;

}

/*
 * Before clearing queue from decap the flows should be cleared. This will
 * be done at higher level. Keeping the flow and removing the queue may result
 * in unpredictable behavior
 */

int cprif_decap_memroy_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
     _SOC_IF_ERR_EXIT(cprif_drv_decap_queue_data_tbl_clear(unit, port,
                                                           queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_framer_state_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
    /* only for data framer state queue_num <64 for data axc*/
    if (queue_num < 64) {
        _SOC_IF_ERR_EXIT(cprif_drv_iq_unpsk_buff_state_tbl_clear(unit, port,
                         queue_num));
        _SOC_IF_ERR_EXIT(cprif_drv_cont_assembly_map_state_tbl_clear(unit, port,
                         queue_num));
    }
    _SOC_IF_ERR_EXIT(cprif_drv_decap_queue_state_tbl_clear(unit, port,
                     queue_num));
exit:
    SOC_FUNC_RETURN;

}

#define CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM            0
#define CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE     1
#define CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE     2

int cprif_decap_flow_classification_config_set(int unit, int port,
                                               uint8 subtype,
                                               uint32 queue_num,
                                               cprimod_cls_option_t cls_option,
                                               cprimod_cls_flow_type_t flow_type)
{
    uint32 option=0;
    uint32 type=0;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    switch (cls_option) {
        case cprimodClsOptionUseQueueNum:
            option = CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM;
            break;

        case cprimodClsOptionUseFlowIdToQueue:
            option = CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE;
            break;

        case cprimodClsOptionUseOpcodeToQueue:
            option = CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE;
            break;

        default:
            break;
    }

    switch (flow_type) {
        case cprimodClsFlowTypeData:
            type = CPRIF_DECAP_CLS_FLOW_TYPE_DATA;
            break;

        case cprimodClsFlowTypeDataWithExt: /* Not Suppoprted */
            type = CPRIF_DECAP_CLS_FLOW_TYPE_DATA_WITH_EXT;
            break;

        case cprimodClsFlowTypeCtrlWithOpcode:
            type = CPRIF_DECAP_CLS_FLOW_TYPE_CTRL_WITH_OPCODE;
            break;

        case cprimodClsFlowTypeCtrl:
            type = CPRIF_DECAP_CLS_FLOW_TYPE_CTRL;
            break;
        default:
            break;
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_classification_entry_set (unit, port,
                                                        subtype,
                                                        queue_num,
                                                        option,
                                                        type));


exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_classification_config_get(int unit, int port,
                                               uint8 subtype,
                                               uint32* queue_num,
                                               cprimod_cls_option_t* cls_option,
                                               cprimod_cls_flow_type_t* flow_type)
{
    uint32 option;
    uint32 type;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_classification_entry_get(unit, port,
                                                       subtype,
                                                       queue_num,
                                                       &option,
                                                       &type));
    switch (option) {
        case CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM:
            *cls_option = cprimodClsOptionUseQueueNum;
            break;

        case CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE:
            *cls_option = cprimodClsOptionUseFlowIdToQueue;
            break;

        case CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE:
            *cls_option = cprimodClsOptionUseOpcodeToQueue;
            break;

        default:
            break;
    }

    switch (type) {
        case CPRIF_DECAP_CLS_FLOW_TYPE_DATA:
            *flow_type = cprimodClsFlowTypeData;
            break;

        case CPRIF_DECAP_CLS_FLOW_TYPE_DATA_WITH_EXT: /* Not Suppoprted */
            *flow_type = cprimodClsFlowTypeDataWithExt;
            break;

        case CPRIF_DECAP_CLS_FLOW_TYPE_CTRL_WITH_OPCODE:
            *flow_type = cprimodClsFlowTypeCtrlWithOpcode;
            break;

        case CPRIF_DECAP_CLS_FLOW_TYPE_CTRL:
            *flow_type = cprimodClsFlowTypeCtrl;
            break;
        default:
            break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_to_queue_mapping_set(int unit, int port, uint32 flow_id, uint32 queue_num)
{
    uint8 queue;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    queue = queue_num;
    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_to_queue_map_set(unit, port,
                                               flow_id,
                                               queue));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_to_queue_mapping_get(int unit, int port, uint32 flow_id, uint32* queue_num)
{

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_to_queue_map_get(unit, port,
                                               flow_id,
                                               queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_queue_to_ordering_info_index_mapping_set(int unit, int port, uint32 queue_num, uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    if (ordering_info_index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Decap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           ordering_info_index));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_to_ordering_info_index_set(unit, port,
                                                          queue_num,
                                                          ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_queue_to_ordering_info_index_mapping_get(int unit, int port, uint32 queue_num, uint32* ordering_info_index)
{

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_to_ordering_info_index_get(unit, port,
                                                          queue_num,
                                                          ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_tick_set(int unit, int port, uint32 divider, uint32 basic_frame_tick, uint32 tick_bitmap, uint32 tick_bitmap_size)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_tick_set(unit, port,
                                           divider,
                                           basic_frame_tick,
                                           tick_bitmap,
                                           tick_bitmap_size));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_tick_get(int unit, int port, uint32* divider, uint32* basic_frame_tick, uint32* tick_bitmap, uint32* tick_bitmap_size)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_tick_get(unit, port,
                                           divider,
                                           basic_frame_tick,
                                           tick_bitmap,
                                           tick_bitmap_size));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_enable(int unit, int port, uint8 enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_enable(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}


int cprif_tx_framer_tgen_offset_set(int unit, int port, uint64 timestamp_offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_offset_set(unit, port,timestamp_offset));



exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_offset_get(int unit, int port, uint64* timestamp_offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_offset_get(unit, port,timestamp_offset));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_bfn_set(int unit, int port, uint16 tgen_bfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_bfn_set(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_bfn_get(int unit, int port, uint16* tgen_bfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_bfn_get(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_hfn_set(int unit, int port, uint8 tgen_hfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_hfn_set(unit, port, tgen_hfn));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_hfn_get(int unit, int port, uint8* tgen_hfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_hfn_get(unit, port, tgen_hfn));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_select_counter_set(int unit, int port, cprimod_tgen_counter_select_t tgen_counter_select)
{
    uint8 bfn_hfn;
    SOC_INIT_FUNC_DEFS;

    if (tgen_counter_select == cprimodTgenCounterSelectBfn) {
        bfn_hfn = CPRIF_TX_TGEN_USE_BFN;
    } else {
        bfn_hfn = CPRIF_TX_TGEN_USE_HFN;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_select_counter_set(unit,port,bfn_hfn));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_select_counter_get(int unit, int port, cprimod_tgen_counter_select_t* tgen_counter_select)
{
    uint8 bfn_hfn;
    SOC_INIT_FUNC_DEFS;

    bfn_hfn = 0;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_select_counter_get(unit,port,&bfn_hfn));

    if (bfn_hfn == CPRIF_TX_TGEN_USE_BFN) {
        *tgen_counter_select = cprimodTgenCounterSelectBfn;
    } else {
        *tgen_counter_select = cprimodTgenCounterSelectHfn;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_header_compare_entry_add(int unit, int port,
                                    cprimod_header_compare_entry_t* entry)
{
    cprimod_header_compare_entry_t local_entry;
    int index;

    cprimod_header_compare_entry_t_init( &local_entry);
    for(index=0; index < CPRIF_HEADER_COMPARE_TABLE_SIZE;index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_rsvd4_header_compare_entry_get(unit, port,
                                                      index,
                                                      &local_entry));
        /*
         * if empty spot is found, add the entry.
         */
        if (local_entry.valid == 0) {
            return(cprif_drv_rsvd4_header_compare_entry_set(unit, port,
                                                            index,
                                                            entry));
        }
    }
    LOG_CLI(("Header Compare Table No Empty Space Found.\n"));
    return SOC_E_PARAM;
}


int cprif_header_compare_entry_delete(int unit, int port,
                                      cprimod_header_compare_entry_t* entry)
{
    cprimod_header_compare_entry_t local_entry;
    int index;

    cprimod_header_compare_entry_t_init( &local_entry);

    for(index=0; index < CPRIF_HEADER_COMPARE_TABLE_SIZE;index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_rsvd4_header_compare_entry_get(unit, port,
                                                      index,
                                                      &local_entry));
        /*
         * if valid entry, compare to see if match.
         */
        if (local_entry.valid == 1) {

            /*
             * If matched entry found, invalidate and get out.
             */

            if ((local_entry.match_data == entry->match_data) &&
               (local_entry.mask == entry->mask) &&
               (local_entry.flow_id == entry->flow_id) &&
               (local_entry.flow_type == entry->flow_type)) {

                    cprimod_header_compare_entry_t_init( &local_entry);
                    local_entry.valid = 0;
                    return(cprif_drv_rsvd4_header_compare_entry_set(unit, port,
                                                            index,
                                                            &local_entry));
            }
        }
    }
    LOG_CLI(("No Matching Header Compare Entry Found to Delete .\n"));
    return SOC_E_PARAM;
}

int cprif_modulo_rule_entry_set(int unit, int port,
                                uint8 modulo_rule_num,
                                cprimod_transmission_rule_type_t modulo_rule_type,
                                cprimod_modulo_rule_entry_t* modulo_rule)
{
    uint8 ctrl_rule;
    SOC_INIT_FUNC_DEFS;

    if (modulo_rule_type == cprimodTxRuleTypeCtrl) {
        ctrl_rule = CPRIF_MODULO_RULE_USE_CTRL_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_CTRL_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_CTRL_RULE_SIZE));
            return SOC_E_PARAM;
        }

    } else {
        ctrl_rule = CPRIF_MODULO_RULE_USE_DATA_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_DATA_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_DATA_RULE_SIZE));
            return SOC_E_PARAM;
        }
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_modulo_rule_entry_set(unit, port,
                                               modulo_rule_num,
                                               ctrl_rule,
                                               modulo_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_modulo_rule_entry_get(int unit, int port,
                                uint8 modulo_rule_num,
                                cprimod_transmission_rule_type_t modulo_rule_type,
                                cprimod_modulo_rule_entry_t* modulo_rule)
{
    uint8 ctrl_rule;
    SOC_INIT_FUNC_DEFS;

    if (modulo_rule_type == cprimodTxRuleTypeCtrl) {
        ctrl_rule = CPRIF_MODULO_RULE_USE_CTRL_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_CTRL_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_CTRL_RULE_SIZE));
            return SOC_E_PARAM;
        }

    } else {
        ctrl_rule = CPRIF_MODULO_RULE_USE_DATA_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_DATA_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_DATA_RULE_SIZE));
            return SOC_E_PARAM;
        }
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_modulo_rule_entry_get(unit, port,
                                               modulo_rule_num,
                                               ctrl_rule,
                                               modulo_rule));
exit:
    SOC_FUNC_RETURN;
}

int cprif_dual_bitmap_rule_entry_set(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_dbm_rule_entry_t* dbm_rule)
{
    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dual_bitmap_rule_entry_set(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dual_bitmap_rule_entry_get(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_dbm_rule_entry_t* dbm_rule)
{

    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dual_bitmap_rule_entry_get(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_secondary_dual_bitmap_rule_entry_set(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_secondary_dbm_rule_entry_t* dbm_rule)
{
    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Secondary DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_set(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_secondary_dual_bitmap_rule_entry_get(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_secondary_dbm_rule_entry_t* dbm_rule)
{

    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Secondary DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_get(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dbm_position_entry_set(int unit, int port, uint8 index,
                                 cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Position Table Index  %d is out of range > than %d",
                              index,
                              CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dbm_position_entry_set(unit, port, index, pos_entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dbm_position_entry_get(int unit, int port, uint8 index,
                                 cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;
    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Position Table Index  %d is out of range > than %d",
                              index,
                              CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dbm_position_entry_get(unit, port, index, pos_entry));
exit:
    SOC_FUNC_RETURN;

}


static
int _cprif_port_rx_speed_set(int unit, int port,
                               cprimod_port_speed_t speed)
{
    uint32 wd_len; 
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_set(unit, port,
                                                       speed));
    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
    wd_len = _cprif_from_speed_towd_len(speed);
    /* Adding AXC for control word */
    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 255,
                                                  0, wd_len));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rx_interface_config_set(int unit, int port,
                               const cprimod_port_interface_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodSpdCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    _SOC_IF_ERR_EXIT(_cprif_port_rx_speed_set(unit, port, config->speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rx_interface_config_get(int unit, int port,
                                       cprimod_port_interface_config_t* config)
{
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rxcpri_port_type_get(unit, port,
                                                   &(config->interface)));
    if (config->interface == cprimodRsvd4) {
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_get(unit, port,
                                                       &rsvd4_speed));
        switch(rsvd4_speed) {
        case cprimodRsvd4SpdMult4X:
            config->speed = cprimodSpd3072p0;
            break;
        case cprimodRsvd4SpdMult8X:
            config->speed = cprimodSpd6144p0;
            break;
        default:
            config->speed = cprimodSpd3072p0;
            break;
        }
    } else {
        SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_get(unit, port,
                                                    &(config->speed)));
    }

    SOC_FUNC_RETURN;
}

static
int _cprif_port_tx_speed_set(int unit, int port,
                               cprimod_port_speed_t speed)
{
    uint32 wd_len; 
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_cpri_txpcs_speed_set(unit, port,
                                                       speed));

    _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
    wd_len = _cprif_from_speed_towd_len(speed);
    /* Adding AXC for control word */
    _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 255,
                                                  0, wd_len));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_tx_interface_config_set(int unit, int port,
                                       const cprimod_port_interface_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodSpdCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    SOC_IF_ERROR_RETURN(_cprif_port_tx_speed_set(unit, port,
                                                        config->speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_tx_interface_config_get(int unit, int port,
                                       cprimod_port_interface_config_t* config)
{
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_txcpri_port_type_get(unit, port,
                                                   &(config->interface)));
    if (config->interface == cprimodRsvd4) {
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port,
                                                       &rsvd4_speed));
        switch(rsvd4_speed) {
        case cprimodRsvd4SpdMult4X:
            config->speed = cprimodSpd3072p0;
            break;
        case cprimodRsvd4SpdMult8X:
            config->speed = cprimodSpd6144p0;
            break;
        default:
            config->speed = cprimodSpd3072p0;
            break;
        }
    } else {
        SOC_IF_ERROR_RETURN(cprif_drv_cpri_txpcs_speed_get(unit, port,
                                                       &(config->speed)));
    }

    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_interface_config_set(int unit, int port,
                          const cprimod_port_rsvd4_config_t* config) {
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodRsvd4SpdMultCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    /* coverity[mixed_enums:FALSE] */
    SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_set(unit, port,
                                                        config->speed));
exit:
    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_interface_config_set(int unit, int port,
                          const cprimod_port_rsvd4_config_t* config) {
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodRsvd4SpdMultCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    /* coverity[mixed_enums:FALSE] */
    SOC_IF_ERROR_RETURN(cprif_drv_cpri_txpcs_speed_set(unit, port,
                                                        config->speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_tx_interface_config_get(int unit, int port,
                          cprimod_port_rsvd4_config_t* config) {
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port,
                                                   &(config->speed)));

    SOC_FUNC_RETURN;
}

int cprif_port_rx_frame_optional_config_set(int unit, int port,
                     cprimod_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rx_frame_optional_config_set(
                        unit, port, field, value));
    SOC_FUNC_RETURN;
}

int cprif_port_rx_frame_optional_config_get(int unit, int port,
                     cprimod_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}


int cprif_port_tx_frame_optional_config_set(int unit, int port,
                     cprimod_tx_config_field_t field, uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_tx_frame_optional_config_set(
                        unit, port, field, value));
    SOC_FUNC_RETURN;
}

int cprif_port_tx_frame_optional_config_get(int unit, int port,
                     cprimod_tx_config_field_t field, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_tx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}


int cprif_port_rsvd4_rx_frame_optional_config_set(int unit, int port,
                     cprimod_rsvd4_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_frame_optional_config_set(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_frame_optional_config_get(int unit, int port,
                     cprimod_rsvd4_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_speed_set(int unit, int port, cprimod_port_rsvd4_speed_mult_t speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_set(unit, port, speed));
    /* add following
     *entry num   axcc_id     rsrv_cnt    data_cnt    Description
     *0            0           0           24         extract 24 bits of hdr
     *1            1           0           128        extract 128 bits of pld
     */
    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 0, 0,
                                                  CPRIMOD_RSVD4_HEADER_SIZE));
    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 1,
                                                  CPRIMOD_RSVD4_HEADER_SIZE,
                                                  CPRIMOD_RSVD4_PAYLOAD_SIZE));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_speed_get(int unit, int port, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_get(unit, port, speed));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_speed_set(int unit, int port, cprimod_port_rsvd4_speed_mult_t speed)
{
    int i;
    uint32 divider, basic_frame_tick;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_set(unit, port, speed));
    /* for both speed 4x and 8x programming same value */
    divider = 1;
    basic_frame_tick = 100;
    SOC_IF_ERROR_RETURN(cprif_tx_framer_tgen_tick_set(unit, port, divider, basic_frame_tick, 0, 0));

    /*entry num   axcc_id     rsrv_cnt    data_cnt    Description
     *0           0           0           24          extract 24 bits of hdr
     *1           1           0           128         extract 128 bits of pld
     *2           0           0           24          extract 24 bits of hdr
     *3           1           0           128         extract 128 bits of pld
     *  ...............................
     *40          0           0           24          extract 24 bits of hdr
     *41          1           0           128         extract 128 bits of pld
     *
     * i  axcc_id   start bit             datasize
     *
     * 0  0         0*(24+128)    = 0      24
     * 0  1         0*(24+128)+24 = 24     128
     * 1  0         1*(24+128)    = 152    24
     * 1  1         1*(24+128)+24 = 176    128
     * 2  0         2*(24+128)    = 304    24
     * 2  1         2*(24+128)+24 = 328    128
     */
    for(i=0; i<21; i++) {
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 0,
                         i*(CPRIMOD_RSVD4_HEADER_SIZE+CPRIMOD_RSVD4_PAYLOAD_SIZE),
                         CPRIMOD_RSVD4_HEADER_SIZE));
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 1,
                         i*(CPRIMOD_RSVD4_HEADER_SIZE+CPRIMOD_RSVD4_PAYLOAD_SIZE)+CPRIMOD_RSVD4_HEADER_SIZE,
                         CPRIMOD_RSVD4_PAYLOAD_SIZE));
    }
    /* Special case, last entry of 8 bits */
    _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 1,
                     21*(CPRIMOD_RSVD4_HEADER_SIZE+CPRIMOD_RSVD4_PAYLOAD_SIZE)+8, 0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_speed_get(int unit, int port, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port, speed));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_rx_fsm_state_set(int unit, int port,
                     cprimod_rsvd4_rx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_fsm_state_set(
                        unit, port, state));

    SOC_FUNC_RETURN;
}


int cprif_port_rsvd4_rx_fsm_state_get(int unit, int port,
                     cprimod_rsvd4_rx_fsm_state_t* state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_fsm_state_get(
                        unit, port, state));

    SOC_FUNC_RETURN;
}


int cprif_port_rsvd4_rx_overide_set(int unit, int port,
          cprimod_rsvd4_rx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_overide_set(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_rx_overide_get(int unit, int port,
          cprimod_rsvd4_rx_overide_t parameter, int* enable, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_overide_get(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_frame_optional_config_set(int unit, int port,
                     cprimod_rsvd4_tx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_frame_optional_config_set(
                        unit, port, field, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_frame_optional_config_get(int unit, int port,
                     cprimod_rsvd4_tx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_fsm_state_set(int unit, int port,
                     cprimod_rsvd4_tx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_fsm_state_set(
                        unit, port, state));
    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_fsm_state_get(int unit, int port,
                     cprimod_rsvd4_tx_fsm_state_t *state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_fsm_state_get(
                        unit, port, state));
    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_overide_set(int unit, int port,
          cprimod_rsvd4_tx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_overide_set(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_overide_get(int unit, int port,
          cprimod_rsvd4_tx_overide_t parameter, int* enable, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_overide_get(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rx_pcs_status_get(int unit, int port,
                     cprimod_rx_pcs_status_t status, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rx_pcs_status_get(
                        unit, port, status, value));
    SOC_FUNC_RETURN;

}

STATIC
int _cpri_core_firmware_load(const phymod_core_access_t* core, cprimod_firmware_load_method_t load_method, cprimod_firmware_loader_f fw_loader)
{
    SOC_INIT_FUNC_DEFS;
    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        SOC_IF_ERROR_RETURN(falcon2_monterey_ucode_mdio_load(&core->access, falcon_dpll_ucode, falcon_dpll_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader);
        SOC_IF_ERROR_RETURN(falcon2_monterey_ucode_init(&core->access));
        SOC_IF_ERROR_RETURN
            (falcon2_monterey_pram_firmware_enable(&core->access, 1, 0));
        SOC_IF_ERROR_RETURN(fw_loader(core, falcon_dpll_ucode_len, falcon_dpll_ucode));
        SOC_IF_ERROR_RETURN
            (falcon2_monterey_pram_firmware_enable(&core->access, 0, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _cpri_core_lane_map_set(int unit, int port,
                            const phymod_core_access_t* core,
                            const phymod_lane_map_t* lane_map)
{
    uint32_t lane;
    uint8_t pmd_tx_addr[4], pmd_rx_addr[4];
    SOC_INIT_FUNC_DEFS;

    if (lane_map->num_of_lanes != CPRI_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for (lane = 0; lane < CPRI_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= CPRI_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= CPRI_NOF_LANES_IN_CORE)){
            return SOC_E_PARAM;
        }
    }
    /* PMD lane addr is based on PCS logical to physical mapping */
    for (lane = 0; lane < CPRI_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[(lane_map->lane_map_tx[lane] & 0xf)] = lane;
        pmd_rx_addr[(lane_map->lane_map_rx[lane] & 0xf)] = lane;
    }

    for (lane = 0; lane < CPRI_NOF_LANES_IN_CORE; lane++){
        SOC_IF_ERROR_RETURN
        (cprif_drv_lane_swap_set(unit, port, lane, cprimod_dir_tx,
                                 lane_map->lane_map_tx[lane]));
        SOC_IF_ERROR_RETURN
        (cprif_drv_lane_swap_set(unit, port, lane, cprimod_dir_rx,
                                 lane_map->lane_map_rx[lane]));
    }

    SOC_IF_ERROR_RETURN
        (falcon2_monterey_map_lanes(&core->access, CPRI_NOF_LANES_IN_CORE, pmd_tx_addr, pmd_rx_addr));
    SOC_FUNC_RETURN;
}

STATIC
int _cpri_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{
    struct falcon2_monterey_uc_core_config_st serdes_firmware_core_config;

    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_core_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_core_config.VcoRate;
    serdes_firmware_core_config.field.disable_write_pll_iqp = fw_core_config.disable_write_pll_iqp;
    SOC_IF_ERROR_RETURN(falcon2_monterey_set_uc_core_config(&phy->access, serdes_firmware_core_config));

    return PHYMOD_E_NONE;
}

STATIC
int _cpri_pmd_pll_configure (phymod_access_t* pc, int pll_div,  phymod_ref_clk_t ref_clock)
{
    enum falcon2_monterey_pll_enum pll_cfg;

    switch (pll_div) {
        case phymod_TSCF_PLL_DIV160:
            pll_cfg = FALCON2_MONTEREY_pll_div_160x;
            break;
        case phymod_TSCF_PLL_DIV165:
            pll_cfg = FALCON2_MONTEREY_pll_div_165x_refc122;
            break;
        case phymod_TSCF_PLL_DIV198:
            pll_cfg = FALCON2_MONTEREY_pll_div_198x;
            break;
        case phymod_TSCF_PLL_DIV200:
            pll_cfg = FALCON2_MONTEREY_pll_div_200x;
            break;
        default:
            pll_cfg = FALCON2_MONTEREY_pll_div_160x;
            break;
    }

     SOC_IF_ERROR_RETURN
        (falcon2_monterey_configure_pll(pc, pll_cfg));

    return PHYMOD_E_NONE;
}

/*
 * Caculate vco rate in MHz.
 */
STATIC
int _cpri_pmd_pll_to_vco_rate(phymod_access_t* pc, int pll_div, phymod_ref_clk_t ref_clk, uint32_t* vco_rate)
{
    uint32_t pll_multiplier = 0;

    switch (pll_div){
        case phymod_TSCF_PLL_DIV160:
            pll_multiplier = 160;
            break;
        case phymod_TSCF_PLL_DIV165:
            pll_multiplier = 165;
            break;
        case phymod_TSCF_PLL_DIV198:
            pll_multiplier = 198;
            break;
        case phymod_TSCF_PLL_DIV200:
            pll_multiplier = 200;
            break;
        default:
            pll_multiplier = 160;
            break;
    }
   *vco_rate = pll_multiplier * 122 + pll_multiplier *88/100;

    return PHYMOD_E_NONE;
}

STATIC
int _cpri_core_init_pass2(int unit, int port, const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t  phy_access;
    phymod_phy_access_t  phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;
    uint32_t vco_rate;

    SOC_INIT_FUNC_DEFS;
    sal_memcpy(&phy_access.access, &core->access, sizeof(phy_access.access));
    sal_memcpy(&core_copy, core, sizeof(core_copy));

    phy_access.type           = core->type; \
    phy_access.port_loc       = core->port_loc; \
    phy_access.device_op_mode = core->device_op_mode; \
    phy_access.access.lane_mask = 0x1;

    core_copy.access.lane_mask = 0x1;
    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

#ifndef CPRI_PMD_CRC_UCODE_VERIFY
    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        int rv;
        rv = falcon2_monterey_ucode_load_verify(&core_copy.access, (uint8_t *) &falcon_dpll_ucode, falcon_dpll_ucode_len);
        if (rv != SOC_E_NONE) {
            /*_SOC_EXIT_WITH_ERR(SOC_E_FAIL, ("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));*/
            SOC_IF_ERROR_RETURN(rv);
        }
    }
#endif

    /* release pmd lane hard reset */
    SOC_IF_ERROR_RETURN
        (falcon2_monterey_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x1));

    /* Set uc_active = 1 */
    SOC_IF_ERROR_RETURN
        (falcon2_monterey_uc_active_set(&core_copy.access, 1));

    /* De-assert micro reset */
    SOC_IF_ERROR_RETURN
        (falcon2_monterey_uc_reset(&core_copy.access, 0));

    if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef CPRI_PMD_CRC_UCODE_VERIFY
        /* poll the ready bit in 10 ms */
        SOC_IF_ERROR_RETURN(
            falcon2_monterey_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 100));

#else
        SOC_IF_ERROR_RETURN(
            falcon2_monterey_ucode_crc_verify(&core_copy.access, falcon_dpll_ucode_len, falcon_dpll_ucode_crc));
#endif
    }

    SOC_IF_ERROR_RETURN
        (falcon2_monterey_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));

    /* set charge pump current */
    if (init_config->afe_pll.afe_pll_change_default){
        SOC_IF_ERROR_RETURN
            (falcon2_monterey_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
        firmware_core_config_tmp.disable_write_pll_iqp = 1;
    } else {
        SOC_IF_ERROR_RETURN
            (falcon2_monterey_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
        firmware_core_config_tmp.disable_write_pll_iqp = 0;
    }

    /* PLL configuration */
    phy_access_copy.access.lane_mask = 0x1;
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;


    /* reset core DP */
    core_copy.access.pll_idx = 0; 
    SOC_IF_ERROR_RETURN
        (falcon2_monterey_core_soft_reset_release(&core_copy.access, 0));

    if (init_config->pll1_div_init_value) {
        core_copy.access.pll_idx = 1; 
        SOC_IF_ERROR_RETURN
            (falcon2_monterey_core_soft_reset_release(&core_copy.access, 0));
    }
    /* PLL0 config */
    phy_access_copy.access.pll_idx = 0;

    SOC_IF_ERROR_RETURN
        (_cpri_pmd_pll_configure(&phy_access_copy.access, init_config->pll1_div_init_value, init_config->interface.ref_clock));
    SOC_IF_ERROR_RETURN
        (_cpri_pmd_pll_to_vco_rate(&phy_access_copy.access, init_config->pll0_div_init_value, init_config->interface.ref_clock, &vco_rate));
    firmware_core_config_tmp.VcoRate = MHZ_TO_VCO_RATE(vco_rate);
    SOC_IF_ERROR_RETURN
        (_cpri_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));

    /* PLL1 config */
    if (init_config->pll1_div_init_value) {
        phy_access_copy.access.pll_idx = 1;
        SOC_IF_ERROR_RETURN
            (_cpri_pmd_pll_configure(&phy_access_copy.access, init_config->pll1_div_init_value, init_config->interface.ref_clock));
        SOC_IF_ERROR_RETURN
            (_cpri_pmd_pll_to_vco_rate(&phy_access_copy.access, init_config->pll0_div_init_value, init_config->interface.ref_clock, &vco_rate));

        SOC_IF_ERROR_RETURN
            (_cpri_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));
    }
    /* Release core DP soft reset */
    core_copy.access.pll_idx = 0;
    SOC_IF_ERROR_RETURN
        (falcon2_monterey_core_soft_reset_release(&core_copy.access, 1));

    if (init_config->pll1_div_init_value) {
        core_copy.access.pll_idx = 1;
        SOC_IF_ERROR_RETURN
            (falcon2_monterey_core_soft_reset_release(&core_copy.access, 1));
    }

    SOC_FUNC_RETURN;
}

STATIC
int _cpri_core_init_pass1(int unit, int port, const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    uint32_t uc_enable = 0;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    SOC_INIT_FUNC_DEFS;

    sal_memcpy(&phy_access.access, &core->access, sizeof(phy_access.access));
    sal_memcpy(&core_copy, core, sizeof(core_copy));

    phy_access.type           = core->type; \
    phy_access.port_loc       = core->port_loc; \
    phy_access.device_op_mode = core->device_op_mode; \
    phy_access.access.lane_mask = 0xf;

    core_copy.access.lane_mask = 0x1;

    /* De-assert PMD core power and core data path reset */
    SOC_IF_ERROR_RETURN
        (cprif_drv_pmd_reset_seq(unit, port, core_status->pmd_active));


    SOC_IF_ERROR_RETURN
        (falcon2_monterey_uc_active_get(&phy_access.access, &uc_enable));
    if (uc_enable) return PHYMOD_E_NONE;

    SOC_IF_ERROR_RETURN
        (_cpri_core_lane_map_set(unit, port, &core_copy, &init_config->lane_map));

    /*  Micro code load */
    _rv = _cpri_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader);
    if (_rv != SOC_E_NONE) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, ("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
    }
exit:
    SOC_FUNC_RETURN;

} 

int cprif_core_init(int unit, int port, const phymod_core_access_t* core,
                    const phymod_core_init_config_t* init_config,
                    phymod_core_status_t* core_status)
{
    int lane;
    SOC_INIT_FUNC_DEFS;

    /* Control using CLPORT */
    SOC_IF_ERROR_RETURN(cprif_drv_cip_top_ctrl_set(unit, port, 0));

    if ((!CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
         !CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
          CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        SOC_IF_ERROR_RETURN
            (_cpri_core_init_pass1(unit, port, core, init_config, core_status));

        if (CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
          CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        SOC_IF_ERROR_RETURN
            (_cpri_core_init_pass2(unit, port, core, init_config, core_status));
    }


    /* remove POR H reset */
    SOC_IF_ERROR_RETURN(cprif_drv_pmd_ctrl_por_h_rstb_set(unit, port, 0));
    /* PLL programming */
    if (init_config->pll0_div_init_value) {
        /* PLL is CPRI */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_pll_ctrl_set(unit, port, 0, 1));
        /* enable reg clk in and ref clk out */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_ref_clk_ctrl_set(unit, port, 0, 1, 1));
        /* Set lcref_sel */
        SOC_IF_ERROR_RETURN(cprif_drv_cpmport_pll_ctrl_config_set(unit, port, 0, 0));
        /*
         * init pll0
         */
        SOC_IF_ERROR_RETURN(cprif_drv_pmd_core_datapath_hard_reset_set(unit, port, 0, 1));
        /* wait for pll to lock */
    }
    if (init_config->pll1_div_init_value) {
        /* PLL is CPRI */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_pll_ctrl_set(unit, port, 1, 1));
        /* enable reg clk in and ref clk out */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_ref_clk_ctrl_set(unit, port, 1, 1, 1));
        /* Set lcref_sel */
        SOC_IF_ERROR_RETURN(cprif_drv_cpmport_pll_ctrl_config_set(unit, port, 1, 0));
        /*
         * init pll1
         */
        SOC_IF_ERROR_RETURN(cprif_drv_pmd_core_datapath_hard_reset_set(unit, port, 1, 1));
        /* wait for pll to lock */
    }
    /* Load firmware */
    /* enable UC */
    for(lane = 0; lane < CPRIMOD_MAX_LANES_PER_CORE; lane++) {
        SOC_IF_ERROR_RETURN(cprif_drv_lane_swap_set(unit, port, lane, cprimod_dir_rx,
                            init_config->lane_map.lane_map_rx[lane]));
        SOC_IF_ERROR_RETURN(cprif_drv_lane_swap_set(unit, port, lane, cprimod_dir_tx,
                            init_config->lane_map.lane_map_tx[lane]));
    }

    SOC_FUNC_RETURN;
}

int cprif_port_init(int unit, int port, const phymod_phy_access_t* phy,
                    const cprimod_port_init_config_t* port_config)
{
    int i;
    uint32 wd_len;
    cprimod_basic_frame_table_id_t table;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_tx_h_reset_set(unit, port, 0));
    SOC_IF_ERROR_RETURN(cprif_drv_rx_h_reset_set(unit, port, 0));

    SOC_IF_ERROR_RETURN(cprif_drv_cip_tx_h_reset_set(unit, port, 0));
    SOC_IF_ERROR_RETURN(cprif_drv_cip_rx_h_reset_set(unit, port, 0));

    SOC_IF_ERROR_RETURN(cprif_drv_datapath_tx_h_reset_set(unit, port, 0));
    SOC_IF_ERROR_RETURN(cprif_drv_datapath_rx_h_reset_set(unit, port, 0));
    /* remove POR H reset */
    SOC_IF_ERROR_RETURN(cprif_drv_pmd_ctrl_por_h_rstb_set(unit, port, 0));

    /*
    * set polarity port_config->polarity
    * set other per lane PMD setting
    */
    /*
     * Check speed if current VCO will support it or through error
     */
    if (port_config->interface == cprimodCpri) {
        SOC_IF_ERROR_RETURN(_cprif_port_tx_speed_set(unit, port,
                                                           port_config->tx_cpri_speed));
        SOC_IF_ERROR_RETURN(_cprif_port_rx_speed_set(unit, port,
                                                           port_config->rx_cpri_speed));
    } else {
        SOC_IF_ERROR_RETURN(cprif_port_rsvd4_tx_speed_set(unit, port,
                                                            port_config->tx_rsvd4_speed));
        SOC_IF_ERROR_RETURN(cprif_port_rsvd4_rx_speed_set(unit, port,
                                                            port_config->rx_rsvd4_speed));
    }
    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_parser_active_table_get(unit, port,&table));
    if (table == cprimod_basic_frame_table_0) {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_1));
    } else {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_0));
    }
    _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_assembly_active_table_get(unit, port,&table));
    if (table == cprimod_basic_frame_table_0) {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_1));
    } else {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_0));
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_stuff_rsrv_bits_set(unit, port, port_config->roe_stuffing_bit, port_config->roe_reserved_bit));
    /* remove soft resets Rx path*/
    _SOC_IF_ERR_EXIT(cprif_drv_glas_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_credit_reset_set(unit, port, 0));
    /* remove soft resets Tx path*/
    _SOC_IF_ERR_EXIT(cprif_drv_cip_tx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_tx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_dp_reset_set(unit, port, 0));
    if (port_config->interface == cprimodCpri) {
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
        wd_len = _cprif_from_speed_towd_len(port_config->rx_cpri_speed);
        /* Adding AXC for control word */
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 255,
                                                      0, wd_len));
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
        wd_len = _cprif_from_speed_towd_len(port_config->tx_cpri_speed);
        /* Adding AXC for control word */
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 255,
                                                      0, wd_len));
    } else {
        /* Set up the rsvd4 Tx stand by table */
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
        for(i=0; i<21; i++) {
            _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 0,
                             i*(CPRIMOD_RSVD4_HEADER_SIZE+CPRIMOD_RSVD4_PAYLOAD_SIZE),
                             CPRIMOD_RSVD4_HEADER_SIZE));
            _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 1,
                             i*(CPRIMOD_RSVD4_HEADER_SIZE+CPRIMOD_RSVD4_PAYLOAD_SIZE)+CPRIMOD_RSVD4_HEADER_SIZE,
                             CPRIMOD_RSVD4_PAYLOAD_SIZE));
        }
        /* Special case, last entry of 8 bits */
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 1,
                         21*(CPRIMOD_RSVD4_HEADER_SIZE+CPRIMOD_RSVD4_PAYLOAD_SIZE)+8, 0));
        /* Set up the rsvd4 Rx stand by table */
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 0, 0,
                                                      CPRIMOD_RSVD4_HEADER_SIZE));
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 1,
                                                      CPRIMOD_RSVD4_HEADER_SIZE,
                                                      CPRIMOD_RSVD4_PAYLOAD_SIZE));
    }
exit:
    SOC_FUNC_RETURN;
}


int cprif_rsvd4_rx_master_frame_sync_config_set(int unit, int port, uint32 master_frame_number, uint64 master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_master_frame_sync_config_set(unit, port,
                                                    master_frame_number,
                                                    master_frame_start_time));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_master_frame_sync_config_get(int unit, int port, uint32* master_frame_number, uint64* master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_master_frame_sync_config_get(unit, port,
                                                    master_frame_number,
                                                    master_frame_start_time));


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_sync_info_get(int unit, int port, uint32* hyper_frame_num, uint32* radio_frame_num)
{
    cprif_drv_rx_control_word_status_t cw_info;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&cw_info, 0, sizeof(cprif_drv_rx_control_word_status_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_control_word_status_get(unit, port, &cw_info));

    *hyper_frame_num    =  cw_info.hyper_frame;
    *radio_frame_num    =  cw_info.radio_frame;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_l1_signal_info_get(int unit, int port, cprimod_cpri_cw_l1_protocol_info_t* l1_info)
{
    cprif_drv_rx_control_word_status_t cw_info;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&cw_info, 0, sizeof(cprif_drv_rx_control_word_status_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_control_word_status_get(unit, port, &cw_info));

    l1_info->hdlc_rate      = cw_info.hdlc_rate;
    l1_info->protocol_ver   = cw_info.protocol_ver;
    l1_info->eth_pointer    = cw_info.eth_pointer;
    l1_info->reset          = cw_info.reset  ? 1:0;
    l1_info->l1_rai         = cw_info.l1_rai ? 1:0;
    l1_info->l1_sdi         = cw_info.l1_sdi ? 1:0;
    l1_info->l1_los         = cw_info.l1_los ? 1:0;
    l1_info->l1_lof         = cw_info.l1_lof ? 1:0;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_l1_signal_signal_protection_set(int unit, int port, uint32 signal_map, uint32 enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_rx_l1_signal_signal_protection_set(unit, port, signal_map, enable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_l1_signal_signal_protection_get(int unit, int port, uint32 signal_map, uint32* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_rx_l1_signal_signal_protection_get(unit, port, signal_map, enable));

exit:
    SOC_FUNC_RETURN;

}

static
void _cprimod_to_cprif_fcs_size ( cprimod_cpri_hdlc_fcs_size_t cprimod_fcs_size, uint32* cprif_fcs_size)
{
    switch (cprimod_fcs_size) {
        case CPRIMOD_CPRI_HDLC_FCS_SIZE_8_BITS:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_8_BITS;
            break;
        case CPRIMOD_CPRI_HDLC_FCS_SIZE_16_BITS:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_16_BITS;
            break;
        case CPRIMOD_CPRI_HDLC_FCS_SIZE_32_BITS:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_32_BITS;
            break;
        default:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_8_BITS;
            break;
    }
}

static
void _cprimod_to_cprif_filling_flag_pattern(cprimod_cpri_hdlc_filling_flag_pattern_t cprimod_pattern, uint32* cprif_pattern)
{
    switch (cprimod_pattern) {

        case CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7E:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_7E;
            break;
        case CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7F:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_7F;
            break;
        case CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_FF:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_FF;
            break;
        default:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_7E;
            break;
    }
}

static
void _cprimod_to_cprif_hdlc_crc_mode( cprimod_cpri_crc_mode_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CRC_MODE_APPEND:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_APPEND;
            break;
        case CPRIMOD_CPRI_CRC_MODE_REPLACE:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_REPLACE;
            break;
        case CPRIMOD_CPRI_CRC_MODE_NO_UPDATE:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_NO_UPDATE;
            break;
        default:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_APPEND;
    }
}

int cprif_cpri_port_cw_slow_hdlc_config_set(int unit, int port, const cprimod_slow_hdlc_config_info_t* config_info)
{
    cprif_cpri_rx_hdlc_config_t rx_config;
    cprif_cpri_tx_hdlc_config_t tx_config;
    SOC_INIT_FUNC_DEFS;


    /****  RX Setting   ****/
    sal_memset(&rx_config, 0, sizeof(cprif_cpri_rx_hdlc_config_t));

    rx_config.cw_sel = config_info->cw_sel;
    rx_config.cw_size= config_info->cw_size;

    if (config_info->crc_byte_swap) {
        rx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_SWAP;
    } else {
        rx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_NO_SWAP;
    }

    if (config_info->crc_init_val == CPRIMOD_CPRI_HDLC_CRC_INIT_VAL_ALL_1) {
        rx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_1S;
    } else {
        rx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_0S;
    }

    rx_config.queue_num= config_info->queue_num;

    if (config_info->use_fe_mac) {
        rx_config.use_fe_mac= CPRIF_HDLC_USE_FE_MAC;

    } else {
        rx_config.use_fe_mac= CPRIF_HDLC_USE_HDLC;
    }

    if (config_info->no_fcs_err_check) {
        rx_config.ignore_fcs_err= CPRIF_HDLC_FCS_NO_CHECK;
    } else {
        rx_config.ignore_fcs_err= CPRIF_HDLC_FCS_CHECK;
    }
    _cprimod_to_cprif_fcs_size(config_info->fcs_size, &rx_config.fcs_size);

    rx_config.runt_frame_drop= config_info->runt_frame_drop ? 1 : 0;
    rx_config.long_frame_drop= config_info->long_frame_drop ? 1 : 0;
    rx_config.min_frame_size = config_info->min_frame_size;
    rx_config.max_frame_size = config_info->max_frame_size;;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_slow_hdlc_config_set(unit, port, &rx_config));

    /****  TX Setting   ****/

    sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_hdlc_config_t));

    tx_config.cw_sel = config_info->cw_sel;
    tx_config.cw_size= config_info->cw_size;
    _cprimod_to_cprif_filling_flag_pattern(config_info->tx_filling_flag_pattern, &tx_config.filling_flag_pattern);

    if (config_info->tx_flag_size == CPRIMOD_HDLC_FLAG_SIZE_1_BYTE) {
        tx_config.flag_size = CPRIF_HDLC_FLAG_SIZE_1_BYTE;
    } else if (config_info->tx_flag_size == CPRIMOD_HDLC_FLAG_SIZE_2_BYTE) {
        tx_config.flag_size = CPRIF_HDLC_FLAG_SIZE_2_BYTE;
    } else {

    }
    if (config_info->crc_byte_swap) {
        tx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_SWAP;
    } else {
        tx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_NO_SWAP;
    }

    if (config_info->crc_init_val == CPRIMOD_CPRI_HDLC_CRC_INIT_VAL_ALL_1) {
        tx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_1S;
    } else {
        tx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_0S;
    }

    tx_config.queue_num = config_info->queue_num;

    if (config_info->use_fe_mac) {
        tx_config.use_fe_mac= CPRIF_HDLC_USE_FE_MAC;

    } else {
        tx_config.use_fe_mac= CPRIF_HDLC_USE_HDLC;
    }

    _cprimod_to_cprif_fcs_size(config_info->fcs_size, &tx_config.fcs_size);
    _cprimod_to_cprif_hdlc_crc_mode(config_info->tx_crc_mode, &tx_config.crc_mode);

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_tx_cw_slow_hdlc_config_set(unit, port, &tx_config));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_cw_slow_hdlc_config_get(int unit, int port, cprimod_slow_hdlc_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    LOG_CLI((" Not implement .\n"));


    SOC_FUNC_RETURN;

}

static
void _cprimod_to_cprif_fast_eth_crc_mode( cprimod_cpri_crc_mode_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CRC_MODE_APPEND:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_APPEND;
            break;
        case CPRIMOD_CPRI_CRC_MODE_REPLACE:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_REPLACE;
            break;
        case CPRIMOD_CPRI_CRC_MODE_NO_UPDATE:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_NO_UPDATE;
            break;
        default:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_APPEND;
            break;
    }
}

int cprif_cpri_port_cw_fast_eth_config_set(int unit, int port, const cprimod_fast_eth_config_info_t* config_info)
{
    cprif_cpri_rx_fast_eth_config_t rx_config;
    cprif_cpri_rx_fast_eth_word_config_t rx_word_config;
    cprif_cpri_tx_fast_eth_config_t tx_config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&rx_config, 0, sizeof(cprif_cpri_rx_fast_eth_config_t));
    sal_memset(&rx_word_config, 0, sizeof(cprif_cpri_rx_fast_eth_word_config_t));

    rx_word_config.sub_channel_start     = config_info->schan_start ;
    rx_word_config.sub_channel_size      = config_info->schan_size ;
    rx_word_config.cw_sel                = config_info->cw_sel ;
    rx_word_config.cw_size               = config_info->cw_size ;
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_word_config_set(unit, port, &rx_word_config));

    rx_config.queue_num             = config_info->queue_num ;
    rx_config.ignore_fcs_err        = config_info->no_fcs_err_check ;
    rx_config.min_packet_drop       = config_info->min_packet_drop ;
    rx_config.max_packet_drop       = config_info->max_packet_drop ;
    rx_config.min_packet_size       = config_info->min_packet_size ;
    rx_config.max_packet_size       = config_info->max_packet_size ;
    rx_config.strip_crc             = config_info->strip_crc;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_config_set(unit, port, &rx_config));

    sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_fast_eth_config_t));
    tx_config.sub_channel_start     = config_info->schan_start ;
    tx_config.sub_channel_size      = config_info->schan_size ;
    tx_config.cw_sel                = config_info->cw_sel ;
    tx_config.cw_size               = config_info->cw_size ;
    tx_config.queue_num             = config_info->queue_num ;
    tx_config.min_ipg               = config_info->min_ipg;
    _cprimod_to_cprif_fast_eth_crc_mode(config_info->tx_crc_mode, &tx_config.crc_mode);
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_tx_cw_fast_eth_config_set(unit, port, &tx_config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_cw_fast_eth_config_get(int unit, int port, cprimod_fast_eth_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */

    LOG_CLI((" Not implement .\n"));

    SOC_FUNC_RETURN;

}


static
void _cprimod_to_cprif_vsd_control_schan_num_bytes( cprimod_cpri_control_schan_num_bytes_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CONTROL_SCHAN_NUM_BYTES_1:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_1;
            break;
        case CPRIMOD_CPRI_CONTROL_SCHAN_NUM_BYTES_2:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_2;
            break;
        case CPRIMOD_CPRI_CONTROL_SCHAN_NUM_BYTES_4:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_4;
            break;
        default:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_1;
            break;
    }
}

static
void _cprimod_to_cprif_vsd_control_flow_num_bytes( cprimod_cpri_control_flow_num_bytes_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CONTROL_FLOW_NUM_BYTES_2:
            *cprif_mode = CPRIF_VSD_FLOW_NUM_OF_BYTES_2;
            break;
        case CPRIMOD_CPRI_CONTROL_FLOW_NUM_BYTES_4:
            *cprif_mode = CPRIF_VSD_FLOW_NUM_OF_BYTES_4;
            break;
        default:
            *cprif_mode = CPRIF_VSD_FLOW_NUM_OF_BYTES_2;
            break;
    }
}

int cprif_cpri_port_rx_vsd_cw_config_set(int unit, int port, const cprimod_cpri_rx_vsd_config_info_t* config_info)
{
    cprif_cpri_vsd_config_t config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_config_t));

    config.sub_channel_start        = config_info->schan_start ;
    config.sub_channel_size         = config_info->schan_size ;
    config.sub_channel_steps        = config_info->schan_steps ;
    config.queue_num                = config_info->queue_num ;

    _cprimod_to_cprif_vsd_control_schan_num_bytes(config_info->schan_bytes,
                                                  &config.sub_channel_num_bytes);

    _cprimod_to_cprif_vsd_control_flow_num_bytes(config_info->flow_bytes,
                                                 &config.flow_bytes);
    config.byte_order = CPRIF_VSD_CTRL_BYTE_NO_SWAP ;

    COMPILER_64_SET(config.rsvd_mask[0],config_info->rsvd_sector_mask[1],config_info->rsvd_sector_mask[0]);
    COMPILER_64_SET(config.rsvd_mask[1],config_info->rsvd_sector_mask[3],config_info->rsvd_sector_mask[2]);

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_config_set(unit, port, &config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_cw_config_get(int unit, int port, cprimod_cpri_rx_vsd_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */

    LOG_CLI((" Not implement .\n"));

    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_ctrl_flow_add(int unit, int port, uint32 group_id, const cprimod_cpri_rx_vsd_flow_info_t* config_info)
{
    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 grp_ptr_mask[2]={0};
    uint32 valid_group_mask[2]={0};
    uint32 grp_ptr_index=0;
    uint32 tag_id;
    int index=0;
    uint32 sector=0;
    uint32 section=0;
    uint32 sector_num[CPRIF_VSD_CTRL_MAX_GROUP_SIZE];
    uint32 copy_id;
    cprif_cpri_rx_vsd_ctrl_flow_config_t flow_config;
    uint32 sector_index=0;
    uint32 group=0;
    uint32 num_sector=0;

    SOC_INIT_FUNC_DEFS;

    if (group_id > CPRIF_VSD_CTRL_NUM_GROUPS) {
        LOG_CLI(("VSD CTRL group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    if (config_info->num_sector > CPRIF_VSD_CTRL_MAX_GROUP_SIZE) {
        return SOC_E_PARAM;
    }

    for ( index=0 ; index < config_info->num_sector ; index++) {

        if (config_info->section_num[index] >= CPRIF_VSD_CTRL_MAX_NUM_SECTION){
            /* if sector has illegal number, error and get out */
            return SOC_E_PARAM;
        }
        if (!CPRIF_PBMP_IS_SET(rsvd_mask, config_info->section_num[index])) {
            /* if sector is not reserved, error and get out */
            return SOC_E_PARAM;
        }

        /* find sector number */
        sector = 0;
        for (section = 0; section <= config_info->section_num[index]; section++)
        {
            if (CPRIF_PBMP_IS_SET(rsvd_mask, sector)) {
                sector++;
            }
        }
        sector_num[index] = sector -1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    /* find valid  group list. */
    sector = 0;
    for ( section = 0 ; section < CPRIF_VSD_CTRL_MAX_NUM_SECTION; section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            sector++;
            if (CPRIF_PBMP_IS_SET(ctrl_mask, section)) {
                /* This is valid sector. */
                sector_index = sector - 1;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rx_vsd_ctrl_flow_config_get(unit, port, sector_index, &flow_config));
                CPRIF_PBMP_IDX_MARK(valid_group_mask, flow_config.group_id);
            }
        }
    }

    /* marked all used grp ptr. */
    for ( group = 0 ; group < CPRIF_VSD_CTRL_NUM_GROUPS; group++) {
        if (CPRIF_PBMP_IS_SET(valid_group_mask, group)){
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_group_config_get(unit, port, group,  &tag_id, &grp_ptr_index));

            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_group_num_sector_get(unit, port, group,  &num_sector));
            for(index = 0; index < (num_sector + 1); index++){
                CPRIF_PBMP_IDX_MARK(grp_ptr_mask, grp_ptr_index + index);
            }
        }
    }

    /* get the grp ptr space. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_allocate_grp_ptr(grp_ptr_mask, config_info->num_sector, &grp_ptr_index));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_group_num_sector_set(unit, port, group_id, config_info->num_sector-1));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_group_config_set(unit, port, group_id, config_info->tag_id, grp_ptr_index));
    sal_memset(&flow_config, 0, sizeof(cprif_cpri_rx_vsd_ctrl_flow_config_t));

    flow_config.group_id            = group_id;
    flow_config.hyper_frame_number  = config_info->hyper_frame_number;
    flow_config.hyper_frame_modulo  = config_info->hyper_frame_modulo;
    flow_config.filter_zero_data    = config_info->filter_zero_data? 1:0;


    for(index = 0; index < config_info->num_sector; index++){

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_vsd_ctrl_flow_config_set(unit,port,
                                                   sector_num[index],
                                                   &flow_config));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_vsd_ctrl_group_assign_ptr_set(unit, port,
                                                        grp_ptr_index + index,
                                                        sector_num[index]));
    }

    /* add ctrl_mask with new bits. */
    for ( index=0 ; index < config_info->num_sector ; index++) {
        CPRIF_PBMP_IDX_MARK(ctrl_mask, config_info->section_num[index]);
    }

    /* update the ctrl mask to alternate location and set the active copy. */

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_control_queue_tag_to_flow_id_map_set(int unit, int port, uint32 tag_id, uint32 flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_tag_to_flow_id_set(unit, port, tag_id, flow_id));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_control_queue_tag_to_flow_id_map_get(int unit, int port, uint32 tag_id, uint32* flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_tag_to_flow_id_get(unit, port, tag_id, flow_id));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_cw_config_set(int unit, int port, const cprimod_cpri_tx_vsd_config_info_t* config_info)
{
    cprif_cpri_vsd_config_t config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_config_t));

    config.sub_channel_start        = config_info->schan_start ;
    config.sub_channel_size         = config_info->schan_size ;
    config.sub_channel_steps        = config_info->schan_steps ;
    config.queue_num                = config_info->queue_num ;

    _cprimod_to_cprif_vsd_control_schan_num_bytes(config_info->schan_bytes,
                                                  &config.sub_channel_num_bytes);

    _cprimod_to_cprif_vsd_control_flow_num_bytes(config_info->flow_bytes,
                                                 &config.flow_bytes);
    config.byte_order = CPRIF_VSD_CTRL_BYTE_NO_SWAP ;

    COMPILER_64_SET(config.rsvd_mask[0],config_info->rsvd_sector_mask[1],config_info->rsvd_sector_mask[0]);
    COMPILER_64_SET(config.rsvd_mask[1],config_info->rsvd_sector_mask[3],config_info->rsvd_sector_mask[2]);

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_config_set(unit, port, &config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_vsd_cw_config_get(int unit, int port, cprimod_cpri_tx_vsd_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */


    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_ctrl_flow_add(int unit, int port, uint32 group_id, const cprimod_cpri_tx_vsd_flow_info_t* config_info)
{

    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 grp_ptr_mask[2]={0};
    uint32 valid_group_mask[2]={0};
    uint32 grp_ptr_index=0;
    int index=0;
    uint32 sector=0;
    uint32 section=0;
    uint32 sector_num[CPRIF_VSD_CTRL_MAX_GROUP_SIZE];
    uint32 copy_id;
    cprif_cpri_tx_vsd_ctrl_flow_config_t flow_config;
    uint32 group=0;
    uint32 num_sector=0;
    SOC_INIT_FUNC_DEFS;

    if (group_id > CPRIF_VSD_CTRL_NUM_GROUPS) {
        LOG_CLI(("VSD CTRL group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    if (config_info->num_sector > CPRIF_VSD_CTRL_MAX_GROUP_SIZE) {
        LOG_CLI(("Num Of Sector  =%d is out of range.\n",config_info->num_sector));
        return SOC_E_PARAM;
    }

    for ( index=0 ; index < config_info->num_sector ; index++) {

        if (config_info->section_num[index] >= CPRIF_VSD_CTRL_MAX_NUM_SECTION){
            /* if sector has illegal number, error and get out */
            return SOC_E_PARAM;
        }
        if (!CPRIF_PBMP_IS_SET(rsvd_mask, config_info->section_num[index])) {
            /* if sector is not reserved, error and get out */
            return SOC_E_PARAM;
        }

        /* find sector number */
        sector = 0;
        for (section = 0; section <= config_info->section_num[index]; section++)
        {
            if (CPRIF_PBMP_IS_SET(rsvd_mask, sector)) {
                sector++;
            }
        }
        sector_num[index] = sector -1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    /* find valid  group list. */
    sector = 0;
    for ( section = 0 ; section < CPRIF_VSD_CTRL_MAX_NUM_SECTION; section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            sector++;
            if (CPRIF_PBMP_IS_SET(ctrl_mask, section)) {
                /* This is valid sector. */
                _SOC_IF_ERR_EXIT
                    (cprif_drv_tx_vsd_ctrl_section_group_num_get(unit, port,
                                                                 section,
                                                                 &group));
                CPRIF_PBMP_IDX_MARK(valid_group_mask, group);
            }
        }
    }

    /* marked all used grp ptr. */
    for ( group = 0 ; group < CPRIF_VSD_CTRL_NUM_GROUPS; group++) {
        if (CPRIF_PBMP_IS_SET(valid_group_mask, group)){

            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_vsd_ctrl_group_config_get(unit, port,
                                           group,
                                           &num_sector,
                                           &grp_ptr_index));
            for(index = 0; index < (num_sector + 1); index++){
                CPRIF_PBMP_IDX_MARK(grp_ptr_mask, grp_ptr_index + index);
            }
        }
    }

    /* get the grp ptr space. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_allocate_grp_ptr(grp_ptr_mask, config_info->num_sector, &grp_ptr_index));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_group_config_set(unit, port, group_id, config_info->num_sector-1, grp_ptr_index));

    sal_memset(&flow_config, 0, sizeof(cprif_cpri_tx_vsd_ctrl_flow_config_t));

    flow_config.hyper_frame_number  = config_info->hyper_frame_number;
    flow_config.hyper_frame_modulo  = config_info->hyper_frame_modulo;
    flow_config.repeat_mode         = config_info->repeat_mode ? 1:0;


    for(index = 0; index < config_info->num_sector; index++){

        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_flow_config_set(unit,port,
                                                   sector_num[index],
                                                   &flow_config));

        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_group_assign_ptr_set(unit, port,
                                                        grp_ptr_index + index,
                                                        sector_num[index]));
    }

    /* add ctrl_mask with new bits. */
    for ( index=0 ; index < config_info->num_sector ; index++) {
        CPRIF_PBMP_IDX_MARK(ctrl_mask, config_info->section_num[index]);
    }

    /* update the ctrl mask to alternate location and set the active copy. */

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

exit:
    SOC_FUNC_RETURN;

}

void _cprimod_to_cprif_cw_filter_mode( cprimod_cpri_cw_filter_mode_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CW_FILTER_DISABLE:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_DISABLE;
            break;
        case CPRIMOD_CPRI_CW_FILTER_NON_ZERO:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_NON_ZERO;
            break;
        case CPRIMOD_CPRI_CW_FILTER_PERIODIC:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_PERIODIC;
            break;
        case CPRIMOD_CPRI_CW_FILTER_CHANGE:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_CHANGE;
            break;
        case CPRIMOD_CPRI_CW_FILTER_PATTERN_MATCH:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_PATTERN_MATCH;
            break;
        default:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_DISABLE;
            break;
    }
}

void _cprif_to_cprimod_cw_filter_mode( uint32 cprif_mode , cprimod_cpri_cw_filter_mode_t *cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_VSD_RAW_FILTER_MODE_DISABLE:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_DISABLE;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_NON_ZERO:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_NON_ZERO;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_PERIODIC:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_PERIODIC;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_CHANGE:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_CHANGE;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_PATTERN_MATCH:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_PATTERN_MATCH;
            break;
        default:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_DISABLE;
            break;
    }
}

int cprif_cpri_port_rx_vsd_raw_config_set(int unit, int port, uint32 index, const cprimod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_rx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));
    config.schan_start  = config_info->schan_start;
    config.schan_size   = config_info->schan_size;
    config.cw_select    = config_info->cw_sel;
    config.cw_size      = config_info->cw_size;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_config_set(unit, port, index, &config));

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_rx_vsd_raw_filter_config_t));

    _cprimod_to_cprif_cw_filter_mode(config_info->filter_mode, &filter_config.filter_mode);

    filter_config.hfn_index     = config_info->hyper_frame_index;
    filter_config.hfn_modulo    = config_info->hyper_frame_modulo;
    filter_config.match_value   = config_info->match_value;
    filter_config.match_offset  = config_info->match_offset;
    filter_config.match_mask    = config_info->match_mask;
    filter_config.queue_num     = config_info->queue_num;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_filter_config_set(unit, port, index, &filter_config));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_raw_config_get(int unit, int port, uint32 index, cprimod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_rx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

   if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }
 sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_config_get(unit, port, index, &config));
    config_info->schan_start    = config.schan_start;
    config_info->schan_size     = config.schan_size;
    config_info->cw_sel         = config.cw_select;
    config_info->cw_size        = config.cw_size;

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_rx_vsd_raw_filter_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_filter_config_get(unit, port, index, &filter_config));
    config_info->filter_mode = filter_config.filter_mode;

    _cprif_to_cprimod_cw_filter_mode(filter_config.filter_mode, &config_info->filter_mode);
    config_info->hyper_frame_index  = filter_config.hfn_index;
    config_info->hyper_frame_modulo = filter_config.hfn_modulo;
    config_info->match_value        = filter_config.match_value;
    config_info->match_offset       = filter_config.match_offset;
    config_info->match_mask         = filter_config.match_mask;
    config_info->queue_num          = filter_config.queue_num;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_config_set(int unit, int port, uint32 index, const cprimod_cpri_tx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_tx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));
    config.schan_start  = config_info->schan_start;
    config.schan_size   = config_info->schan_size;
    config.cw_select    = config_info->cw_sel;
    config.cw_size      = config_info->cw_size;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_config_set(unit, port, index, &config));

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_tx_vsd_raw_filter_config_t));

    filter_config.queue_num     = config_info->queue_num;

    if (config_info->map_mode == CPRIMOD_CPRI_CW_MAP_MODE_ROE_FRAME){
        filter_config.map_mode = CPRIF_VSD_RAW_MAP_MODE_USE_ROE_HDR;
    } else {
        filter_config.map_mode = CPRIF_VSD_RAW_MAP_MODE_USE_MODULO;
    }

    if (config_info->repeat_enable) {
        filter_config.repeat_mode = CPRIF_CW_REPEAT_MODE_CONTINUOUS;
    } else {
        filter_config.repeat_mode = CPRIF_CW_REPEAT_MODE_ONCE;
    }

    if (config_info->bfn0_filter_enable) {
        filter_config.bfn0_filter_enable = 1;
    } else {
        filter_config.bfn0_filter_enable = 0;
    }

    if (config_info->bfn1_filter_enable) {
        filter_config.bfn1_filter_enable = 1;
    } else {
        filter_config.bfn1_filter_enable = 0;
    }

    filter_config.hfn_index     = config_info->hyper_frame_index;
    filter_config.hfn_modulo    = config_info->hyper_frame_modulo;
    filter_config.idle_value    = config_info->idle_value;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_filter_config_set(unit, port, index, &filter_config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_config_get(int unit, int port, uint32 index, cprimod_cpri_tx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_tx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_config_get(unit, port, index, &config));
    config_info->schan_start    = config.schan_start;
    config_info->schan_size     = config.schan_size;
    config_info->cw_sel         = config.cw_select;
    config_info->cw_size        = config.cw_size;

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_tx_vsd_raw_filter_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_filter_config_get(unit, port, index, &filter_config));

    config_info->hyper_frame_index  = filter_config.hfn_index;
    config_info->hyper_frame_modulo = filter_config.hfn_modulo;
    config_info->queue_num          = filter_config.queue_num;
    config_info->idle_value         = filter_config.idle_value;

    if (filter_config.bfn0_filter_enable) {
        config_info->bfn0_filter_enable = 1;
    } else {
        config_info->bfn0_filter_enable = 0;
    }

    if (filter_config.bfn1_filter_enable) {
        config_info->bfn1_filter_enable = 1;
    } else {
        config_info->bfn1_filter_enable = 0;
    }
    if (filter_config.repeat_mode == CPRIF_CW_REPEAT_MODE_CONTINUOUS){
        config_info->repeat_enable = 1;
    } else {
        config_info->repeat_enable = 0;
    }
    if (filter_config.map_mode == CPRIF_VSD_RAW_MAP_MODE_USE_ROE_HDR) {
        config_info->map_mode = CPRIMOD_CPRI_CW_MAP_MODE_ROE_FRAME;
    } else {
        config_info->map_mode = CPRIMOD_CPRI_CW_MAP_MODE_PERIODIC;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_filter_set(int unit, int port, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_radio_frame_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_brcm_rsvd5_control_config_set(int unit, int port, uint32 schan_start, uint32 schan_size, uint32 queue_num, int parity_disable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_brcm_rsvd5_config_set(unit, port,schan_start, schan_size, queue_num, parity_disable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_brcm_rsvd5_control_config_get(int unit, int port, uint32* schan_start, uint32* schan_size, uint32* queue_num, int* parity_disable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_brcm_rsvd5_config_get(unit, port,schan_start, schan_size, queue_num, parity_disable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_brcm_rsvd5_control_config_set(int unit, int port, uint32 schan_start, uint32 schan_size, uint32 queue_num, int crc_enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_brcm_rsvd5_config_set(unit, port,schan_start, schan_size, queue_num, crc_enable));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_brcm_rsvd5_control_config_get(int unit, int port, uint32* schan_start, uint32* schan_size, uint32* queue_num, int* crc_enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_brcm_rsvd5_config_get(unit, port,schan_start, schan_size, queue_num, crc_enable));
exit:
    SOC_FUNC_RETURN;

}

void _cprimod_to_cprif_gcw_mask( cprimod_cpri_gcw_mask_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_GCW_MASK_NONE:
            *cprif_mode = CPRIF_GCW_MASK_NONE;
            break;
        case CPRIMOD_CPRI_GCW_MASK_LSB:
            *cprif_mode = CPRIF_GCW_MASK_LSB;
            break;
        case CPRIMOD_CPRI_GCW_MASK_MSB:
            *cprif_mode = CPRIF_GCW_MASK_MSB;
            break;
        case CPRIMOD_CPRI_GCW_MASK_BOTH:
            *cprif_mode = CPRIF_GCW_MASK_BOTH;
            break;
        default:
            *cprif_mode = CPRIF_GCW_MASK_NONE;
            break;
    }
}

void _cprif_to_cprimod_gcw_mask( uint32 cprif_mode, cprimod_cpri_gcw_mask_t* cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_GCW_MASK_NONE:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_NONE;
            break;
        case CPRIF_GCW_MASK_LSB:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_LSB;
            break;
        case CPRIF_GCW_MASK_MSB:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_MSB;
            break;
        case CPRIF_GCW_MASK_BOTH:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_BOTH;
            break;
        default:
            *cprimod_mode = cprimodGcwMaskNone;
            break;
    }
}


int cprif_cpri_port_rx_gcw_config_set(int unit, int port, uint32 index, const cprimod_cpri_rx_gcw_config_t* config_info)
{
    cprif_cpri_rx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_rx_gcw_config_t));

    config.Ns           = config_info->Ns;
    config.Xs           = config_info->Xs;
    config.Y            = config_info->Y;
    config.hfn_index    = config_info->hyper_frame_index;
    config.hfn_modulo   = config_info->hyper_frame_modulo;
    config.match_mask   = config_info->match_mask;
    config.match_value  = config_info->match_value;

    _cprimod_to_cprif_gcw_mask(config_info->mask, &config.mask);
    _cprimod_to_cprif_cw_filter_mode(config_info->filter_mode, &config.filter_mode);

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_gcw_config_set(unit, port,
                                     index, &config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_gcw_config_get(int unit, int port, uint32 index, cprimod_cpri_rx_gcw_config_t* config_info)
{
    cprif_cpri_rx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_rx_gcw_config_t));
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_gcw_config_get(unit, port,
                                     index, &config));
    config_info->Ns                 = config.Ns;
    config_info->Xs                 = config.Xs;
    config_info->Y                  = config.Y;
    config_info->hyper_frame_index  = config.hfn_index;
    config_info->hyper_frame_modulo = config.hfn_modulo;
    config_info->match_mask         = config.match_mask;
    config_info->match_value        = config.match_value;

    _cprif_to_cprimod_gcw_mask(config.mask, &config_info->mask);
    _cprif_to_cprimod_cw_filter_mode(config.filter_mode, &config_info->filter_mode);

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_gcw_config_set(int unit, int port, uint32 index, const cprimod_cpri_tx_gcw_config_t* config_info)
{
    cprif_cpri_tx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_tx_gcw_config_t));
    config.Ns = config_info->Ns;
    config.Xs = config_info->Xs;
    config.Y = config_info->Y;
    _cprimod_to_cprif_gcw_mask(config_info->mask, &config.mask);

    if (config_info->bfn0_filter_enable) {
        config.bfn0_filter_enable = 1;
    } else {
        config.bfn0_filter_enable = 0;
    }

    if (config_info->bfn1_filter_enable) {
        config.bfn1_filter_enable = 1;
    } else {
        config.bfn1_filter_enable = 0;
    }

    if (config_info->repeat_enable){
        config.repeat_mode = CPRIF_CW_REPEAT_MODE_CONTINUOUS;
    } else {
        config.repeat_mode = CPRIF_CW_REPEAT_MODE_ONCE;
    }
    config.hfn_index = config_info->hyper_frame_index;
    config.hfn_modulo = config_info->hyper_frame_modulo;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_config_set(unit, port,
                                     index, &config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_gcw_config_get(int unit, int port, uint32 index, cprimod_cpri_tx_gcw_config_t* config_info)
{
    cprif_cpri_tx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_tx_gcw_config_t));
    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_config_get(unit, port,
                                     index, &config));
    config_info->Ns = config.Ns;
    config_info->Xs = config.Xs;
    config_info->Y = config.Y;
    _cprif_to_cprimod_gcw_mask(config.mask, &config_info->mask);

    if (config.repeat_mode == CPRIF_CW_REPEAT_MODE_CONTINUOUS){
        config_info->repeat_enable = 1;
    } else {
        config_info->repeat_enable = 0;
    }

    if (config.bfn0_filter_enable) {
        config_info->bfn0_filter_enable = 1;
    } else {
        config_info->bfn0_filter_enable = 0;
    }

    if (config.bfn1_filter_enable) {
        config_info->bfn1_filter_enable = 1;
    } else {
        config_info->bfn1_filter_enable = 0;
    }

    config_info->hyper_frame_index = config.hfn_index;
    config_info->hyper_frame_modulo = config.hfn_modulo;

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_gcw_filter_set(int unit, int port, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_control_word_set(int unit, int port, const cprimod_cpri_cw_l1_config_info_t* control_info)
{
    cprif_drv_tx_control_word_t control;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&control, 0, sizeof(cprif_drv_tx_control_word_t));
    control.enet_ptr    = control_info->enet_ptr;
    control.l1_fun     = control_info->l1_fun;
    control.hdlc_rate   = control_info->hdlc_rate;
    control.protocol_ver= control_info->protocol_ver;
    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_word_set(unit, port, &control));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_rx_control_message_config_set(int unit, int port, uint32 queue_num, uint32 default_tag, uint32 no_match_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_control_message_config_set( unit, port,
                                                        queue_num,
                                                        default_tag,
                                                        no_match_tag));
exit:
    SOC_FUNC_RETURN;
}

void _cprimod_to_cprif_proc_type( cprimod_control_msg_proc_type_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FE:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_ETH;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FCB:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_FCB;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_PAYLOAD:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_PAYLOAD;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE_W_TAG:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG_WITH_TAG;
            break;
        default:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_ETH;
            break;
    }
}

void _cprif_to_cprimod_proc_type( uint32 cprif_mode, cprimod_control_msg_proc_type_t* cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_CTRL_FLOW_PROC_TYPE_ETH:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FE;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_FCB:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FCB;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_PAYLOAD:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_PAYLOAD;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG_WITH_TAG:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE_W_TAG;
            break;
        default:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FE;
            break;
    }
}

int cprif_rsvd4_rx_control_flow_config_set(int unit, int port, uint32 flow_id, cprimod_control_flow_config_t* config)
{
    cprif_rsvd4_rx_ctrl_flow_config_t config_info;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }

    sal_memset(&config_info, 0, sizeof(cprif_rsvd4_rx_ctrl_flow_config_t));

    _cprimod_to_cprif_proc_type(config->proc_type, &config_info.proc_type);
    config_info.queue_num       = config->queue_num;
    config_info.sync_profile    = config->sync_profile;
    config_info.sync_enable     = config->sync_enable;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_control_flow_config_set ( unit, port,
                                                     flow_id,
                                                     &config_info));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_control_flow_config_get(int unit, int port, uint32 flow_id, cprimod_control_flow_config_t* config)
{
    cprif_rsvd4_rx_ctrl_flow_config_t config_info;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }

    sal_memset(&config_info, 0, sizeof(cprif_rsvd4_rx_ctrl_flow_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_control_flow_config_get(unit, port,
                                                    flow_id,
                                                    &config_info));
    _cprif_to_cprimod_proc_type(config_info.proc_type, &config->proc_type);
    config->queue_num       = config_info.queue_num;
    config->sync_profile    = config_info.sync_profile;
    config->sync_enable     = config_info.sync_enable;
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_rx_sync_profile_entry_set(int unit, int port, uint32 profile_id, const cprimod_sync_profile_entry_t* entry)
{
    cprif_rsvd4_rx_sync_profile_entry_t int_entry;
    SOC_INIT_FUNC_DEFS;

    if (profile_id >= CPRIF_SYNC_PROFILE_MAX) {
        LOG_CLI(("sync profile_id  =%d is out of range.\n",profile_id));
        return SOC_E_PARAM;
    }
    sal_memset(&int_entry, 0, sizeof(cprif_rsvd4_rx_sync_profile_entry_t));

    int_entry.bfn_offset  = entry->master_frame_offset;
    int_entry.rfrm_offset = entry->message_offset;

    if (entry->count_cycle == CPRIMOD_SYNC_COUNT_CYCLE_UP_TO_6MF){
        int_entry.count_cycle = CPRIF_SYNC_PROFILE_UP_TO_6MF;
    } else {
        int_entry.count_cycle = CPRIF_SYNC_PROFILE_EVERY_MF;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_sync_profile_entry_set(unit, port, profile_id, &int_entry));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_rx_sync_profile_entry_get(int unit, int port, uint32 profile_id, cprimod_sync_profile_entry_t* entry)
{
    cprif_rsvd4_rx_sync_profile_entry_t int_entry;
    SOC_INIT_FUNC_DEFS;

    if (profile_id >= CPRIF_SYNC_PROFILE_MAX) {
        LOG_CLI(("sync profile_id  =%d is out of range.\n",profile_id));
        return SOC_E_PARAM;
    }

    sal_memset(&int_entry, 0, sizeof(cprif_rsvd4_rx_sync_profile_entry_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_sync_profile_entry_get(unit, port, profile_id, &int_entry));
    entry->master_frame_offset  = int_entry.bfn_offset;
    entry->message_offset       = int_entry.rfrm_offset;

    if ( int_entry.count_cycle == CPRIF_SYNC_PROFILE_UP_TO_6MF){
        entry->count_cycle = CPRIMOD_SYNC_COUNT_CYCLE_UP_TO_6MF;
    } else {
        entry->count_cycle = CPRIMOD_SYNC_COUNT_CYCLE_EVERY_MF;
    }

exit:
    SOC_FUNC_RETURN;

}
int cprif_rx_tag_config_set(int unit, int port, uint32 default_tag, uint32 no_match_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_tag_config_set( unit, port, default_tag, no_match_tag));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_tag_gen_entry_add(int unit, int port, cprimod_tag_gen_entry_t* entry)
{
    cprif_rx_tag_gen_entry_t table_entry;
    int index;

    SOC_INIT_FUNC_DEFS;

    for(index=0; index < CPRIF_TAG_GEN_MAX_ENTRY; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_tag_gen_entry_get(unit, port, index, &table_entry));

        if (table_entry.valid == 0) {
            /* found an avaialble location */
            table_entry.valid  = 1;
            table_entry.mask   = entry->mask;
            table_entry.header = entry->header;
            table_entry.tag_id = entry->tag_id;
                return (cprif_drv_rx_tag_gen_entry_set(unit, port, index, &table_entry));
        }
    }
    LOG_CLI(("No Space to add Tag Gen Entry  .\n"));
    return SOC_E_PARAM;
exit:
    SOC_FUNC_RETURN;
}

int cprif_rx_tag_gen_entry_delete(int unit, int port, cprimod_tag_gen_entry_t* entry)
{
    cprif_rx_tag_gen_entry_t table_entry;
    int index;

    SOC_INIT_FUNC_DEFS;

    for(index=0; index < CPRIF_TAG_GEN_MAX_ENTRY; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_tag_gen_entry_get(unit, port, index, &table_entry));

        if (table_entry.valid == 1) {
            /* found a valid entry */
            if ((table_entry.mask   == entry->mask)  &&
                (table_entry.header == entry->header) &&
                (table_entry.tag_id == entry->tag_id)){
                    /* Matching valid entry found, delete the entry by invalidating the entry.*/
                    table_entry.valid = 0;
                    return (cprif_drv_rx_tag_gen_entry_set(unit, port, index, &table_entry));
                }
        }
    }
    LOG_CLI(("No Matching Entry Found  .\n"));
    return SOC_E_PARAM;
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_control_flow_tag_option_set(int unit, int port, uint32 flow_id, cprimod_cpri_tag_option_t tag_option)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_control_flow_tag_option_set(unit, port,
                                                  flow_id,
                                                  (tag_option == CPRIMOD_CPRI_TAG_LOOKUP)?1:0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_group_member_add(int unit, int port, uint32 group_id,
                                                uint32 priority, uint32 queue_num,
                                                cprimod_control_msg_proc_type_t proc_type)
{
    cprif_control_group_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (group_id >= CPRIF_TX_CONTROL_MAX_GROUP) {
        LOG_CLI(("RSVD4 Tx Control Group group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    if (priority >= CPRIF_TX_CONTROL_MAX_MEMBER) {
        LOG_CLI(("RSVD4 Tx Control Priority   =%d is out of range.\n",priority));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_get(unit, port,group_id, &entry));

    entry.queue_num[priority] = queue_num;
    _cprimod_to_cprif_proc_type(proc_type, &entry.proc_type[priority]);
    entry.valid_mask |=  (0x1 << priority);

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_set(unit, port,group_id, &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_group_member_delete(int unit, int port, uint32 group_id, uint32 priority)
{
    cprif_control_group_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (group_id >= CPRIF_TX_CONTROL_MAX_GROUP) {
        LOG_CLI(("RSVD4 Tx Control Group group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    if (priority >= CPRIF_TX_CONTROL_MAX_MEMBER) {
        LOG_CLI(("RSVD4 Tx Control Priority   =%d is out of range.\n",priority));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_get(unit, port,group_id, &entry));

    /* masked off the valid bit. */
    entry.valid_mask &=  ~(0x1 << priority);

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_set(unit, port,group_id, &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_eth_message_config_set(int unit, int port, uint32 msg_node, uint32 msg_subnode, uint32 msg_type, uint32 msg_padding)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_eth_msg_config_set( unit, port,
                                                  msg_node, msg_subnode,
                                                  msg_type, msg_padding));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_single_raw_message_config_set(int unit, int port, uint32 msg_id, uint32 msg_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_single_msg_config_set(unit, port, msg_id, msg_type));

exit:
    SOC_FUNC_RETURN;

}


int cprif_rsvd4_tx_single_tunnel_message_config_set(int unit, int port, cprimod_cpri_crc_option_t crc_option)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_single_tunnel_msg_config_set(unit, port, (crc_option == CPRIMOD_CPRI_CRC_REGENERATE)?1:0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_config_set(int unit, int port, uint32 flow_id, uint32 queue_num, cprimod_control_msg_proc_type_t proc_type)
{
    uint32 int_proc_type=0;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }

    _cprimod_to_cprif_proc_type(proc_type, &int_proc_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_flow_config_set( unit, port, flow_id, queue_num, int_proc_type));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_config_get(int unit, int port, uint32 flow_id, uint32* queue_num, cprimod_control_msg_proc_type_t* proc_type)
{
    uint32 int_proc_type;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_flow_config_get( unit, port, flow_id, queue_num, &int_proc_type));

    _cprif_to_cprimod_proc_type(int_proc_type, proc_type);

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_flow_header_index_set(int unit, int port, uint32 roe_flow_id, uint32 index)
{
    SOC_INIT_FUNC_DEFS;

    if (roe_flow_id >= CPRIF_CTRL_MAX_ROE_FLOW_ID) {
        LOG_CLI(("control flow roe_flow_id  =%d is out of range.\n",roe_flow_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_flow_header_index_set( unit, port,
                                                             roe_flow_id,
                                                             index));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_flow_header_index_get(int unit, int port, uint32 roe_flow_id, uint32* index)
{
    SOC_INIT_FUNC_DEFS;

    if (roe_flow_id >= CPRIF_CTRL_MAX_ROE_FLOW_ID) {
        LOG_CLI(("control flow roe_flow_id  =%d is out of range.\n",roe_flow_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_flow_header_index_get( unit, port,
                                                             roe_flow_id,
                                                             index));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_header_entry_set(int unit, int port, uint32 index, uint32 header_node, uint32 header_subnode, uint32 payload_node)
{
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_TX_HEADER_CONFIG_TABLE_SIZE) {
        LOG_CLI(("header look up index =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_header_entry_set( unit, port,
                                                        index,
                                                        header_node,
                                                        header_subnode,
                                                        payload_node));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_header_entry_get(int unit, int port, uint32 index, uint32* header_node, uint32* header_subnode, uint32* payload_node)
{
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_TX_HEADER_CONFIG_TABLE_SIZE) {
        LOG_CLI(("header look up index =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_header_entry_get( unit, port,
                                                        index,
                                                        header_node,
                                                        header_subnode,
                                                        payload_node));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_fast_eth_config_set(int unit, int port, const cprimod_cpri_fast_eth_config_t* config_info)
{
    cprif_cpri_rx_fast_eth_config_t rx_config;
    SOC_INIT_FUNC_DEFS;

    rx_config.queue_num             = config_info->queue_num ; /* Not used for for RSVD4 */
    rx_config.ignore_fcs_err        = config_info->no_fcs_err_check ;
    rx_config.min_packet_drop       = config_info->min_packet_drop ;
    rx_config.max_packet_drop       = config_info->max_packet_drop ;
    rx_config.min_packet_size       = config_info->min_packet_size ;
    rx_config.max_packet_size       = config_info->max_packet_size ;
    rx_config.strip_crc             = config_info->strip_crc;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_config_set(unit, port, &rx_config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_or_enet_port_set(int unit, int port, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_cpri_or_enet_port_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_or_enet_port_get(int unit, int port, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_cpri_or_enet_port_get(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_enable_set(int unit, int port, int val)
{
    SOC_INIT_FUNC_DEFS;

    /* make val ^1 to convert enable to disable */
    _SOC_IF_ERR_EXIT (cprif_drv_rx_disable_set(unit, port, (val&1)^1));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_enable_get(int unit, int port, int* val)
{
    int tmpval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_rx_disable_get(unit, port, &tmpval));
    /* make val ^1 to convert disable to enable */
    *val = (tmpval&1)^1;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_enable_set(int unit, int port, int val)
{
    SOC_INIT_FUNC_DEFS;

    /* make val ^1 to convert enable to disable */
    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_set(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideVal, (val&1)^1));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_set(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideEn, (val&1)^1));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_enable_get(int unit, int port, int* val)
{
    uint32 tmpval0, tmpval1;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_get(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideVal, &tmpval0));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_get(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideEn, &tmpval1));
    /* make val ^1 to convert disable to enable */
    *val = (tmpval0 & tmpval1 & 1) ^ 1;

exit:
    SOC_FUNC_RETURN;

}



#endif /* CPRIMOD_CPRI_FALCON_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
