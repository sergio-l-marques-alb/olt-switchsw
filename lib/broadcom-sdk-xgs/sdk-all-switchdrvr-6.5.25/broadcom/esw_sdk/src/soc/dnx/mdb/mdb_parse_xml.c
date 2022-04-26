
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */



#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/mdb_internal_shared.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
#include "../dbal/dbal_internal.h"
#include <bcm_int/dnx/l3/l3_fec.h>
#include "mdb_internal.h"
#include "src/soc/dnx/mdb/auto_generated/dbx_pre_compiled_vmv_mdb_auto_generated_xml_parser.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX





#define MDB_NOF_PHY_DBS              38
#define MDB_XML_MAX_STRING_LENGTH    10
#define MDB_XML_INVALID_BUCKET_DEPTH -1
#define MDB_XML_MAX_DH_LEN           5



#define MDB_HW_ALLOCATION_DIR_PATH "mdb"

#define MDB_FILE_PATH                   "jericho_2"







#define MDB_DH_IS_FULLY_SUPPORTS_FLEX(unit,mdb_table) (dnx_data_mdb.em.flex_fully_supported_get(unit) && MDB_DH_IS_TABLE_SUPPORT_FLEX(unit,mdb_table))
#define MDB_IS_ODD(number) (number & 0x1)



#define MDB_GET_NEXT_BANK_POINTER(last_alloc_bank_plus_one,skip_odd) (last_alloc_bank_plus_one + ( skip_odd*MDB_IS_ODD(last_alloc_bank_plus_one)))


#define MDB_ALIGNMENT_DONT_FIT_POSITION(position, alignment) \
        ((MDB_IS_ODD(position) && (alignment == MDB_FEC_BANK_ALIGNMENT_EVEN)) || \
         (!MDB_IS_ODD(position) && (alignment == MDB_FEC_BANK_ALIGNMENT_ODD)))







typedef enum
{
    MDB_DH_INTERFACE_FSM_NO_CLUSTER_WAS_FOUND = 0,
    MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_FIRST_CLUSTER_FOUND = 1,
    MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_LAST_CLUSTER_FOUND = 2,
    MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND = 3,
    MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_LAST_CLUSTER_FOUND = 4
} mdb_parse_xml_second_interface_fsm_e;



typedef enum
{
    MDB_FEC_BANK_ALIGNMENT_EVEN = 0,
    MDB_FEC_BANK_ALIGNMENT_ODD = 1,
    MDB_FEC_BANK_ALIGNMENT_BOTH = 2,
} mdb_parse_xml_fec_banks_alignment_types_e;



typedef enum
{
    MDB_FEC_BANK_GRANULARITY_NA = 0,
    MDB_FEC_BANK_GRANULARITY_LOW = 0,
    MDB_FEC_BANK_GRANULARITY_HIGH = 1,
    MDB_FEC_BANK_GRANULARITY_NOF_TYPES = 2
} mdb_parse_xml_fec_banks_granularity_types_e;







typedef struct mdb_parse_xml_fec_macro_info_s
{
    uint32 count;
    uint32 sw_state_matching_cluster_id[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS];
    int position[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS];
} mdb_parse_xml_fec_macro_info_t;



typedef struct mdb_parse_xml_fec_hier_alloc_info_s
{
    uint32 first_bank;
    uint32 nof_banks;
    uint32 low_granularity_suppport;
    mdb_parse_xml_fec_banks_alignment_types_e alignments[MDB_FEC_BANK_GRANULARITY_NOF_TYPES];
    mdb_parse_xml_fec_macro_info_t macro_info[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES];
} mdb_parse_xml_fec_hier_alloc_info_t;


typedef struct mdb_parse_xml_fec_db_ranges_alloc_info_s
{
    uint8 skip_odd_bank_ids;
    uint8 high_granularity_indication;
    uint8 low_granularity_indication;
    uint32 max_bank_limitation;
    uint32 fec_db_alloc_first;
    uint32 rest_fec_dbs_to_alloc[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_DBS - 1];
} mdb_parse_xml_fec_db_ranges_alloc_info_t;







const int mdb_parse_xml_intf_1_em_120_clusters_division[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS] =
    { 1, 1, 2, 2, 4, 4, 0, 4 };




static shr_error_e
mdb_set_clusters_in_swstate(
    int unit,
    int allocation_size,
    int cluster_count,
    mdb_cluster_info_t * clusters,
    mdb_physical_table_e current_mdb_table)
{
    int cluster_iter;
    SHR_FUNC_INIT_VARS(unit);
    if (allocation_size > 0 && !SOC_WARM_BOOT(unit))
    {
        allocation_size = UTILEX_MAX(allocation_size, cluster_count);
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.alloc(unit, current_mdb_table, allocation_size));

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_alloc_clusters.set(unit, current_mdb_table, allocation_size));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, current_mdb_table, cluster_count));

        for (cluster_iter = 0; cluster_iter < cluster_count; cluster_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            clusters_info.set(unit, current_mdb_table, cluster_iter, &clusters[cluster_iter]));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
mdb_parse_xml_fec_db_ranges_allocate(
    int unit,
    const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t ** fec_physical_dbs_alloc_info,
    mdb_parse_xml_fec_hier_alloc_info_t * fec_db_info,
    mdb_parse_xml_fec_db_ranges_alloc_info_t * fec_db_ranges_alloc_info)
{
    uint32 fec_db_iter;
    uint32 max_nof_banks;
    uint32 last_fec_bank_allocated;
    uint8 found = FALSE;
    uint8 fec_db_range_alloc_allowed = FALSE;
    int bank_bottom_pointer;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_banks = dnx_data_l3.fec.max_nof_banks_get(unit);

    
    bank_bottom_pointer = dnx_data_l3.fec.first_bank_without_id_alloc_get(unit);

    
    while (bank_bottom_pointer >= 0)
    {
        uint32 bank_bottom_pointer_tmp = bank_bottom_pointer;
        uint32 fec_dbs_left_to_alloc;
        uint32 start_offset;
        uint8 increment_was_done = FALSE;
        uint8 tmp_valid[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_DBS];
        mdb_parse_xml_fec_banks_granularity_types_e granularity;
        found = FALSE;

        sal_memset(tmp_valid, 0, dnx_data_mdb.direct.nof_fec_dbs_get(unit));

        
        for (start_offset = 0; start_offset < 2; start_offset++)
        {
            
            if (bank_bottom_pointer_tmp + fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].nof_banks - 1 >
                fec_db_ranges_alloc_info->max_bank_limitation)
            {
                if (!fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].low_granularity_suppport)
                {
                    break;
                }
                granularity = fec_db_ranges_alloc_info->low_granularity_indication;
            }
            else
            {
                granularity = fec_db_ranges_alloc_info->high_granularity_indication;
            }

            
            if (MDB_ALIGNMENT_DONT_FIT_POSITION
                (bank_bottom_pointer_tmp,
                 fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].alignments[granularity]))
            {
                bank_bottom_pointer_tmp++;
            }
            else
            {
                found = TRUE;
                break;
            }
        }

        
        if (!found)
        {
            bank_bottom_pointer--;
            continue;
        }

        fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].first_bank = bank_bottom_pointer_tmp;

        bank_bottom_pointer_tmp =
            MDB_GET_NEXT_BANK_POINTER((fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].first_bank +
                                       fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].nof_banks),
                                      fec_db_ranges_alloc_info->skip_odd_bank_ids);

        last_fec_bank_allocated =
            (fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].first_bank +
             fec_db_info[fec_db_ranges_alloc_info->fec_db_alloc_first].nof_banks - 1);

        fec_dbs_left_to_alloc = dnx_data_mdb.direct.nof_fec_dbs_get(unit) - 1;

        
        while (fec_dbs_left_to_alloc > 0)
        {
            found = FALSE;
            
            for (fec_db_iter = 0; fec_db_iter < dnx_data_mdb.direct.nof_fec_dbs_get(unit) - 1; fec_db_iter++)
            {
                granularity =
                    (bank_bottom_pointer_tmp +
                     fec_db_info[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]].nof_banks - 1) <=
                    fec_db_ranges_alloc_info->
                    max_bank_limitation ? fec_db_ranges_alloc_info->high_granularity_indication :
                    fec_db_ranges_alloc_info->low_granularity_indication;

                fec_db_range_alloc_allowed =
                    (fec_db_info
                     [fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]].low_granularity_suppport
                     || (bank_bottom_pointer_tmp +
                         fec_db_info[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]].nof_banks - 1 <=
                         fec_db_ranges_alloc_info->max_bank_limitation));

                
                if (MDB_IS_ODD(bank_bottom_pointer_tmp)
                    &&
                    !MDB_IS_ODD(fec_physical_dbs_alloc_info
                                [fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]]->bank_alloc_resolution))
                {
                    bank_bottom_pointer_tmp++;
                }

                
                if (!tmp_valid[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]]
                    &&
                    !MDB_ALIGNMENT_DONT_FIT_POSITION((bank_bottom_pointer_tmp /
                                                      fec_physical_dbs_alloc_info
                                                      [fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc
                                                       [fec_db_iter]]->bank_alloc_resolution),
                                                     fec_db_info[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc
                                                                 [fec_db_iter]].alignments[granularity])
                    && fec_db_range_alloc_allowed)
                {
                    tmp_valid[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]] = TRUE;
                    fec_dbs_left_to_alloc--;

                    fec_db_info[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]].first_bank =
                        bank_bottom_pointer_tmp;

                    bank_bottom_pointer_tmp =
                        MDB_GET_NEXT_BANK_POINTER((fec_db_info
                                                   [fec_db_ranges_alloc_info->
                                                    rest_fec_dbs_to_alloc[fec_db_iter]].first_bank +
                                                   fec_db_info[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc
                                                               [fec_db_iter]].nof_banks),
                                                  fec_db_ranges_alloc_info->skip_odd_bank_ids);

                    last_fec_bank_allocated =
                        UTILEX_MAX(last_fec_bank_allocated,
                                   (fec_db_info[fec_db_ranges_alloc_info->rest_fec_dbs_to_alloc[fec_db_iter]].first_bank
                                    +
                                    fec_db_info[fec_db_ranges_alloc_info->
                                                rest_fec_dbs_to_alloc[fec_db_iter]].nof_banks - 1));

                    increment_was_done = FALSE;
                    found = TRUE;
                }
            }

            
            if (!found)
            {
                if (increment_was_done)
                {
                    break;
                }
                increment_was_done = TRUE;
                bank_bottom_pointer_tmp++;
            }
        }

        
        if ((bank_bottom_pointer_tmp > max_nof_banks) || (fec_dbs_left_to_alloc > 0))
        {
            bank_bottom_pointer--;
        }
        else
        {
            break;
        }
    }

    if (bank_bottom_pointer < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't find a valid FEC allocation.\n");
    }

    {
#if defined(BCM_DNX2_SUPPORT)
        if (last_fec_bank_allocated <= fec_db_ranges_alloc_info->max_bank_limitation)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.set(unit,
                                                              dnx_data_l3.fec.
                                                              max_fec_id_for_single_dhb_cluster_pair_granularity_get
                                                              (unit)));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.set(unit,
                                                              dnx_data_l3.fec.
                                                              max_fec_id_for_double_dhb_cluster_pair_granularity_get
                                                              (unit)));
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
mdb_parse_xml_fec_range_address_set(
    int unit,
    mdb_physical_table_e mdb_fec_physical_table_id,
    mdb_parse_xml_fec_hier_alloc_info_t * fec_db_info,
    uint32 offset)
{
    uint32 macro_global_index;
    uint32 cluster_iter, fec_cluster_iter;
    uint32 fec_cluster_alloc_granularity;
    uint32 bank_pointer;
    uint32 step_count;
    uint32 base_macro_nof_rows;
    uint32 range_start, range_size;
    int start_address, end_address;
    SHR_FUNC_INIT_VARS(unit);

    fec_cluster_alloc_granularity = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_fec_physical_table_id)->row_width /
        dnx_data_mdb.dh.cluster_row_width_bits_get(unit);

    base_macro_nof_rows =
        dnx_data_mdb.dh.macro_type_info_get(unit,
                                            dnx_data_mdb.direct.fec_allocation_base_macro_type_get(unit))->nof_rows;

    
    for (macro_global_index = 0; macro_global_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_global_index++)
    {
        step_count =
            MDB_MACRO_NOF_FEC_BANKS_GET(dnx_data_mdb.dh.logical_macro_info_get(unit, macro_global_index)->macro_type);

        for (cluster_iter = 0; cluster_iter < fec_db_info->macro_info[macro_global_index].count; cluster_iter++)
        {
            bank_pointer =
                fec_db_info->macro_info[macro_global_index].position[cluster_iter] + offset + fec_db_info->first_bank;
            start_address = bank_pointer * base_macro_nof_rows;
            end_address = (bank_pointer + step_count) * base_macro_nof_rows;

            for (fec_cluster_iter = 0; fec_cluster_iter < fec_cluster_alloc_granularity; fec_cluster_iter++)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.start_address.set(unit, mdb_fec_physical_table_id,
                                                                                fec_db_info->macro_info
                                                                                [macro_global_index].sw_state_matching_cluster_id
                                                                                [cluster_iter] + fec_cluster_iter,
                                                                                start_address));

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.end_address.set(unit, mdb_fec_physical_table_id,
                                                                              fec_db_info->macro_info
                                                                              [macro_global_index].sw_state_matching_cluster_id
                                                                              [cluster_iter] + fec_cluster_iter,
                                                                              end_address));
            }

        }
    }

    
    range_start = MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, fec_db_info->first_bank);

    range_size =
        MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit, (fec_db_info->first_bank + fec_db_info->nof_banks)) - range_start;

    SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_set(unit, mdb_fec_physical_table_id, range_start, range_size));

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT)

shr_error_e
mdb_parse_xml_update_fec_hierarchy_address(
    int unit)
{
    mdb_parse_xml_fec_hier_alloc_info_t *hier_info = NULL;
    mdb_parse_xml_fec_db_ranges_alloc_info_t fec_db_ranges_alloc_info;
    mdb_cluster_info_t clusters_info;
    uint32 cluster_iter, fec_hier_iter;
    uint32 macro_global_index;
    uint32 fec_db_alloc_index = 0;
    const dnx_data_l3_fec_fec_physical_db_t *fec_physical_tables_info;
    const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t
        * fec_physical_dbs_alloc_info[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_DBS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(hier_info,
                                sizeof(mdb_parse_xml_fec_hier_alloc_info_t) *
                                dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit),
                                "FEC allocation information per hierarchy array was allocated.", "%s%s%s\r\n", EMPTY,
                                EMPTY, EMPTY);

    fec_physical_tables_info = dnx_data_l3.fec.fec_physical_db_get(unit);

    sal_memset(&fec_db_ranges_alloc_info, 0, sizeof(mdb_parse_xml_fec_db_ranges_alloc_info_t));
    fec_db_ranges_alloc_info.skip_odd_bank_ids =
        dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fer_fec_granularity_double_size);
    fec_db_ranges_alloc_info.high_granularity_indication = MDB_FEC_BANK_GRANULARITY_HIGH;
    fec_db_ranges_alloc_info.low_granularity_indication = MDB_FEC_BANK_GRANULARITY_LOW;
    
    fec_db_ranges_alloc_info.max_bank_limitation =
        (dnx_data_l3.fec.max_fec_id_for_single_dhb_cluster_pair_granularity_get(unit) /
         dnx_data_l3.fec.bank_size_get(unit)) - 1;
    fec_db_ranges_alloc_info.fec_db_alloc_first =
        dnx_data_l3.fec.fec_tables_info_get(unit, MDB_PHYSICAL_TABLE_FEC_1)->index;

    for (fec_hier_iter = fec_db_ranges_alloc_info.fec_db_alloc_first;
         fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        int dh_iter, allocated_clusters;
        int odd_count = 0;
        int base_macro_count = 0;
        uint32 fec_cluster_alloc_granularity = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                  fec_physical_tables_info->physical_table
                                                                                  [fec_hier_iter])->row_width /
            dnx_data_mdb.dh.cluster_row_width_bits_get(unit);

        fec_physical_dbs_alloc_info[fec_hier_iter] =
            dnx_data_mdb.pdbs.fec_physical_dbs_alloc_info_get(unit,
                                                              fec_physical_tables_info->physical_table[fec_hier_iter]);

        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                        number_of_clusters.get(unit, fec_physical_tables_info->physical_table[fec_hier_iter],
                                               &allocated_clusters));

        for (cluster_iter = 0; cluster_iter < (allocated_clusters / fec_cluster_alloc_granularity); cluster_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            clusters_info.get(unit, fec_physical_tables_info->physical_table[fec_hier_iter],
                                              cluster_iter * fec_cluster_alloc_granularity, &clusters_info));

            macro_global_index =
                MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, clusters_info.macro_type, clusters_info.macro_index);
            hier_info[fec_hier_iter].
                macro_info[macro_global_index].sw_state_matching_cluster_id[hier_info[fec_hier_iter].
                                                                            macro_info[macro_global_index].count++] =
                cluster_iter * fec_cluster_alloc_granularity;
            hier_info[fec_hier_iter].nof_banks += MDB_MACRO_NOF_FEC_BANKS_GET(clusters_info.macro_type);
        }

        for (dh_iter =
             dnx_data_mdb.dh.macro_type_info_get(unit,
                                                 fec_physical_dbs_alloc_info[fec_hier_iter]->
                                                 base_macro_type)->global_start_index;
             dh_iter <
             (dnx_data_mdb.dh.
              macro_type_info_get(unit,
                                  fec_physical_dbs_alloc_info[fec_hier_iter]->base_macro_type)->global_start_index +
              dnx_data_mdb.dh.macro_type_info_get(unit,
                                                  fec_physical_dbs_alloc_info[fec_hier_iter]->base_macro_type)->
              nof_macros); dh_iter++)
        {
            odd_count += MDB_IS_ODD(hier_info[fec_hier_iter].macro_info[dh_iter].count);
            base_macro_count += hier_info[fec_hier_iter].macro_info[dh_iter].count;
        }

        

        if (odd_count > 2)
        {
            hier_info[fec_hier_iter].low_granularity_suppport = FALSE;
            hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_HIGH] = MDB_FEC_BANK_ALIGNMENT_BOTH;
        }
        else
        {

            hier_info[fec_hier_iter].low_granularity_suppport = TRUE;

            if (odd_count == 2)
            {
                hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_LOW] = MDB_FEC_BANK_ALIGNMENT_ODD;
                hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_HIGH] = MDB_FEC_BANK_ALIGNMENT_BOTH;
            }
            else if (odd_count == 1)
            {
                hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_LOW] = MDB_FEC_BANK_ALIGNMENT_BOTH;
                hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_HIGH] = MDB_FEC_BANK_ALIGNMENT_BOTH;
            }
            else
            {
                if (base_macro_count != 0)
                {
                    hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_LOW] = MDB_FEC_BANK_ALIGNMENT_BOTH;
                    hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_HIGH] = MDB_FEC_BANK_ALIGNMENT_BOTH;
                }
                else
                {
                    hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_LOW] = MDB_FEC_BANK_ALIGNMENT_EVEN;
                    hier_info[fec_hier_iter].alignments[MDB_FEC_BANK_GRANULARITY_HIGH] = MDB_FEC_BANK_ALIGNMENT_EVEN;
                }
            }
        }

        if (fec_hier_iter != fec_db_ranges_alloc_info.fec_db_alloc_first)
        {
            fec_db_ranges_alloc_info.rest_fec_dbs_to_alloc[fec_db_alloc_index++] = fec_hier_iter;
        }
    }

    
    SHR_IF_ERR_EXIT(mdb_parse_xml_fec_db_ranges_allocate
                    (unit, fec_physical_dbs_alloc_info, hier_info, &fec_db_ranges_alloc_info));

    
    for (fec_hier_iter = fec_db_ranges_alloc_info.fec_db_alloc_first;
         fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        uint32 odd_even_iter, last_macro_b = 0;
        uint32 offset = 0;
        int position = 0;

        
        for (macro_global_index = 0;
             macro_global_index < dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_A)->nof_macros;
             macro_global_index++)
        {
            for (cluster_iter = 0; cluster_iter < hier_info[fec_hier_iter].macro_info[macro_global_index].count;
                 cluster_iter++)
            {
                hier_info[fec_hier_iter].macro_info[macro_global_index].position[cluster_iter] = position;
                position += 2;
            }
        }

        
        for (odd_even_iter = 0; odd_even_iter < 2; odd_even_iter++)
        {
            for (macro_global_index = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_B)->global_start_index;
                 macro_global_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_global_index++)
            {
                if (MDB_IS_ODD(hier_info[fec_hier_iter].macro_info[macro_global_index].count) == odd_even_iter)
                {
                    for (cluster_iter = 0; cluster_iter < hier_info[fec_hier_iter].macro_info[macro_global_index].count;
                         cluster_iter++)
                    {
                        hier_info[fec_hier_iter].macro_info[macro_global_index].position[cluster_iter] = position++;
                        last_macro_b = macro_global_index;
                    }
                }
            }
        }

        
        if (MDB_IS_ODD(hier_info[fec_hier_iter].first_bank))
        {
            if (hier_info[fec_hier_iter].first_bank + hier_info[fec_hier_iter].nof_banks >
                fec_db_ranges_alloc_info.max_bank_limitation)
            {
                offset = hier_info[fec_hier_iter].macro_info[last_macro_b].count;
                for (cluster_iter = 0; cluster_iter < offset; cluster_iter++)
                {
                    hier_info[fec_hier_iter].macro_info[last_macro_b].position[cluster_iter] = cluster_iter - offset;
                }
            }
            else
            {
                offset = 1;
                hier_info[fec_hier_iter].macro_info[last_macro_b].
                    position[hier_info[fec_hier_iter].macro_info[last_macro_b].count - 1] = -1;
            }
        }

        
        SHR_IF_ERR_EXIT(mdb_parse_xml_fec_range_address_set
                        (unit, fec_physical_tables_info->physical_table[fec_hier_iter], &hier_info[fec_hier_iter],
                         offset));
    }

exit:
    if (hier_info != NULL)
    {
        SHR_FREE(hier_info);
    }

    SHR_FUNC_EXIT;
}
#endif 




static shr_error_e
mdb_parse_xml_update_address_map(
    int unit)
{
    mdb_physical_table_e mdb_physical_table_id_iter;

    SHR_FUNC_INIT_VARS(unit);

    

    for (mdb_physical_table_id_iter = 0; mdb_physical_table_id_iter < MDB_NOF_PHYSICAL_TABLES;
         mdb_physical_table_id_iter++)
    {
        if ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id_iter)->db_type ==
             DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
            || (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id_iter)->db_type ==
                DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
        {
            int nof_clusters;
            int table_cluster_idx;
            uint32 row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                   dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                      mdb_physical_table_id_iter)->physical_to_logical)->
                row_width;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id_iter, &nof_clusters));

            

            for (table_cluster_idx = 0; table_cluster_idx < nof_clusters;
                 table_cluster_idx += row_width / MDB_NOF_CLUSTER_ROW_BITS)
            {
                mdb_cluster_info_t cluster_info;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                clusters_info.get(unit, mdb_physical_table_id_iter, table_cluster_idx, &cluster_info));

                if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id_iter)->db_type ==
                    DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
                {
                    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set
                                    (unit, mdb_physical_table_id_iter, cluster_info.start_address,
                                     cluster_info.end_address, table_cluster_idx));
                }
                else if (cluster_info.start_address != MDB_DIRECT_INVALID_START_ADDRESS)
                {

                    

                    dbal_enum_value_field_mdb_eedb_phase_e eedb_phase;
                    dbal_enum_value_field_direct_payload_sizes_e payload_size_type;
                    int logical_addr_factor;

                    eedb_phase = mdb_eedb_table_to_phase(unit, mdb_physical_table_id_iter);
                    SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, eedb_phase, &payload_size_type));
                    logical_addr_factor =
                        MDB_NOF_CLUSTER_ROW_BITS / MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_size_type);

                    SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_set
                                    (unit, mdb_physical_table_id_iter, logical_addr_factor * cluster_info.start_address,
                                     logical_addr_factor * cluster_info.end_address, table_cluster_idx));

                    if (mdb_eedb_table_to_mem_type(unit, mdb_physical_table_id_iter) == MDB_EEDB_MEM_TYPE_DATA_CLUSTER)
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set
                                        (unit, mdb_physical_table_id_iter, cluster_info.start_address,
                                         cluster_info.end_address, table_cluster_idx));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_parse_xml_get_dh_info(
    int unit,
    int bucket_width,
    int cluster_index,
    char *dh_name,
    mdb_physical_table_e current_mdb_table,
    mdb_cluster_info_t * clusters,
    int *cluster_count)
{
    int macro_idx;
    int nof_clusters = bucket_width / MDB_NOF_CLUSTER_ROW_BITS;
    int cluster_pair_idx;
    uint32 dh_name_len;
    mdb_macro_types_e macro_type;

    SHR_FUNC_INIT_VARS(unit);

    
    for (macro_type = 0; macro_type < MDB_NOF_MACRO_TYPES; macro_type++)
    {
        char *macro_xml_str = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->xml_str;

        if (sal_strncmp(dh_name, macro_xml_str, sal_strnlen(macro_xml_str, MDB_XML_MAX_DH_LEN)) == 0)
        {
            break;
        }
    }
    if (macro_type == MDB_NOF_MACRO_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized dh: %s. Unable to match macro.\n", dh_name);
    }

    
    dh_name_len = sal_strnlen(dh_name, MDB_XML_MAX_DH_LEN);
    if ((sal_isdigit(dh_name[dh_name_len - 1])) && (sal_isdigit(dh_name[dh_name_len - 2])))
    {
        
        macro_idx = ((dh_name[dh_name_len - 2] - '0') * 10) + (dh_name[dh_name_len - 1] - '0');
    }
    else if (sal_isdigit(dh_name[dh_name_len - 1]))
    {
        
        macro_idx = (dh_name[dh_name_len - 1] - '0');
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized dh: %s. Unable to parse macro_idx\n", dh_name);
    }

    if (macro_idx >= dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_macros)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Device does not support MDB %s with macro index %d.",
                     dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->name, macro_idx);
    }


    
    for (cluster_pair_idx = 0; cluster_pair_idx < nof_clusters; cluster_pair_idx++)
    {
        uint8 cache_enabled;

        mdb_cluster_info_t *clusters_p = &clusters[*cluster_count];

        sal_memset(clusters_p, 0, sizeof(mdb_cluster_info_t));

        

        (*cluster_count)++;

        

        clusters_p->macro_type = macro_type;
        clusters_p->macro_index = macro_idx;
        clusters_p->cluster_index = cluster_index + cluster_pair_idx;
        clusters_p->cluster_position_in_entry = cluster_pair_idx;

        

        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_half_nof_clusters_enable))
        {
            if ((clusters_p->macro_type == MDB_MACRO_A)
                && (clusters_p->cluster_index >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Device does not support macro A %d utilization of cluster id %d (max %d).",
                             clusters_p->macro_index, clusters_p->cluster_index,
                             dnx_data_mdb.dh.nof_bucket_clusters_get(unit) - 1);
            }
        }

        

        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_75_nof_clusters_enable))
        {
            if ((clusters_p->macro_type == MDB_MACRO_A)
                && ((dnx_data_mdb.dh.mdb_75_macro_halved_get(unit, clusters_p->macro_index))->macro_halved == TRUE)
                && (clusters_p->cluster_index >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Device does not support macro A %d utilization of cluster id %d (max %d).",
                             clusters_p->macro_index, clusters_p->cluster_index,
                             dnx_data_mdb.dh.nof_bucket_clusters_get(unit) - 1);
            }
        }

        

        SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                        (unit, clusters_p->macro_type, clusters_p->macro_index,
                         clusters_p->cluster_index, current_mdb_table));

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));
        if (cache_enabled == TRUE)
        {

            

            if ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, current_mdb_table)->db_type ==
                 DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
                || (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, current_mdb_table)->db_type ==
                    DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
            {
                SHR_IF_ERR_EXIT(mdb_direct_cache_alloc
                                (unit, clusters_p->macro_type, macro_idx, cluster_index + cluster_pair_idx));
            }
        }

        

        if (mdb_eedb_table_to_mem_type(unit, current_mdb_table) == MDB_EEDB_MEM_TYPE_DATA_CLUSTER)
        {

            

            clusters_p->start_address = MDB_DIRECT_INVALID_START_ADDRESS;
            clusters_p->end_address = MDB_DIRECT_INVALID_START_ADDRESS;
        }
    }

    

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_parse_xml_em_banks_map(
    int unit,
    void *curTop)
{
    void *cur_phy_map, *cur_phydb;
    mdb_physical_table_e mdb_table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (mdb_table_id = 0; mdb_table_id < MDB_NOF_PHYSICAL_TABLES; mdb_table_id++)
    {
        if ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
            && (dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_get(unit, mdb_table_id)->valid == TRUE))
        {            

            mdb_cluster_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS *
                                        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];
            int cluster_count = 0;
            int nof_cluster_per_entry = 0;
            int bucket_width;

            cur_phydb =
                dbx_xml_child_get_first(curTop,
                                        dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_get(unit,
                                                                                            mdb_table_id)->xml_name);

            if (cur_phydb == NULL)
            {
                continue;
            }

            RHDATA_GET_INT_STOP(cur_phydb, "bucket_width", bucket_width);

            sal_memset(clusters, 0,
                       DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS * DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS *
                       sizeof(mdb_cluster_info_t));

            RHDATA_ITERATOR(cur_phy_map, cur_phydb, "physical_map")
            {
                char dh_name[MDB_XML_MAX_STRING_LENGTH];
                int cluster_idx;

                RHDATA_GET_INT_STOP(cur_phy_map, "cluster0_idx", cluster_idx);
                RHDATA_GET_STR_STOP(cur_phy_map, "dh", dh_name);

                SHR_IF_ERR_EXIT(mdb_parse_xml_get_dh_info
                                (unit, bucket_width, cluster_idx, dh_name, mdb_table_id, clusters, &cluster_count));

                

                nof_cluster_per_entry = bucket_width / MDB_NOF_CLUSTER_ROW_BITS;
            }

            SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate(unit, cluster_count, cluster_count, clusters, mdb_table_id));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.set(unit, mdb_table_id, nof_cluster_per_entry));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_mdb_clusters_map(
    int unit,
    void *curTop)
{
    int res = 0;
    void *cur_phydb, *cur_phy_map;
    uint32 max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);
    mdb_physical_table_e mdb_table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (mdb_table_id = 0; mdb_table_id < MDB_NOF_PHYSICAL_TABLES; mdb_table_id++)
    {
        dbal_enum_value_field_mdb_db_type_e db_type = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_table_id)->db_type;

        if (((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
             || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS))
            && (dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_get(unit, mdb_table_id)->valid == TRUE))
        {
            int bucket_width, nof_cluster_to_allocate, cluster_count = 0;
            mdb_cluster_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS *
                                        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];
            cur_phydb =
                dbx_xml_child_get_first(curTop,
                                        dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_get(unit,
                                                                                            mdb_table_id)->xml_name);

            if (cur_phydb == NULL)
            {
                continue;
            }

            

            if ((dnx_data_mdb.kaps.nof_dbs_get(unit) == MDB_LPM_DBS_IN_PAIR)
                && (mdb_lpm_xpt_physical_table_is_bb(unit, mdb_table_id)))
            {
                int requested_kaps_cfg, requested_xml_kaps_cfg;
                int mdb_profile_kaps_cfg;

                requested_kaps_cfg = dnx_data_mdb.pdbs.mdb_profile_kaps_cfg_get(unit)->val;

                

                if (requested_kaps_cfg >= MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE)
                {
                    uint8 db_idx = (requested_kaps_cfg / MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE) - 1;
                    mdb_physical_table_e small_kaps_mdb_table_id;
                    void *small_kaps_phydb;

                    if ((db_idx >= MDB_LPM_DBS_IN_PAIR)
                        || ((requested_kaps_cfg % MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE) != 0))
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "%s (%d) exceeds the small+big KAPS configuration range, must be under or equal %d, whole multiple of %d.\n",
                                     spn_MDB_PROFILE_KAPS_CFG, requested_kaps_cfg,
                                     MDB_LPM_DBS_IN_PAIR * MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE,
                                     MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE);
                    }

                    small_kaps_mdb_table_id = MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 + db_idx;

                    small_kaps_phydb =
                        dbx_xml_child_get_first(curTop,
                                                dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_get(unit,
                                                                                                    small_kaps_mdb_table_id)->xml_name);
                    while (small_kaps_phydb != NULL)
                    {
                        int num_big_bbs = 0;

                        RHDATA_ITERATOR(cur_phy_map, small_kaps_phydb, "physical_map")
                        {
                            num_big_bbs++;
                        }
                        if (num_big_bbs == 0)
                        {
                            break;
                        }
                        small_kaps_phydb = dbx_xml_child_get_next(small_kaps_phydb);
                    }

                    if (small_kaps_phydb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "Unable to find a KAPS cfg that corresponds with %s (%d), small KAPS cfg not found\n",
                                     spn_MDB_PROFILE_KAPS_CFG, requested_kaps_cfg);
                    }

                    RHDATA_GET_INT_STOP(small_kaps_phydb, "cfg", requested_xml_kaps_cfg);
                }
                else
                {
                    requested_xml_kaps_cfg = requested_kaps_cfg;
                }

                RHDATA_GET_INT_STOP(cur_phydb, "cfg", mdb_profile_kaps_cfg);
                while ((requested_xml_kaps_cfg != mdb_profile_kaps_cfg) && (mdb_profile_kaps_cfg != 0))
                {
                    cur_phydb = dbx_xml_child_get_next(cur_phydb);
                    if (cur_phydb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "Unsupported MDB profile and cfg (%d) combination. Max kaps cfg for selected profile is %d.",
                                     requested_kaps_cfg, mdb_profile_kaps_cfg);
                    }
                    RHDATA_GET_INT_STOP(cur_phydb, "cfg", mdb_profile_kaps_cfg);
                }
            }

            sal_memset(clusters, 0,
                       DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS * DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS *
                       sizeof(mdb_cluster_info_t));

            RHDATA_GET_INT_STOP(cur_phydb, "bucket_width", bucket_width);
            SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.set(unit, mdb_table_id,
                                                                      bucket_width / MDB_NOF_CLUSTER_ROW_BITS));

            RHDATA_ITERATOR(cur_phy_map, cur_phydb, "physical_map")
            {
                char dh_name[MDB_XML_MAX_STRING_LENGTH];
                int cluster_idx;

                RHDATA_GET_INT_STOP(cur_phy_map, "cluster0_idx", cluster_idx);
                RHDATA_GET_STR_STOP(cur_phy_map, "dh", dh_name);

                SHR_IF_ERR_EXIT(mdb_parse_xml_get_dh_info
                                (unit, bucket_width, cluster_idx, dh_name, mdb_table_id, clusters, &cluster_count));
            }

            nof_cluster_to_allocate = cluster_count;

#if defined(BCM_DNX2_SUPPORT)
            
            if (mdb_table_id == MDB_PHYSICAL_TABLE_EEDB_5_6_DATA || mdb_table_id == MDB_PHYSICAL_TABLE_EEDB_7_8_DATA)
            {
                nof_cluster_to_allocate += max_nof_macro_clusters;
            }
            else if (mdb_table_id == MDB_PHYSICAL_TABLE_FEC_1
                     || mdb_table_id == MDB_PHYSICAL_TABLE_FEC_2 || mdb_table_id == MDB_PHYSICAL_TABLE_FEC_3)
            {
                nof_cluster_to_allocate += 2 * max_nof_macro_clusters;
            }
#endif


            mdb_set_clusters_in_swstate(unit, nof_cluster_to_allocate, cluster_count, clusters, mdb_table_id);
        }
    }

exit:
    if (res)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("XML PARSING ERROR\n")));
        SHR_SET_CURRENT_ERR(res);
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
mdb_parse_xml_eedb_banks_map(
    int unit)
{
    int address_granularity[DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES];
    dbal_enum_value_field_mdb_eedb_phase_e phase_iter;
    uint8 cache_enabled;

    SHR_FUNC_INIT_VARS(unit);

    

    for (phase_iter = 0; phase_iter < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES; phase_iter++)
    {
        address_granularity[phase_iter] =
            dnx_data_mdb.eedb.outlif_physical_phase_granularity_get(unit, phase_iter)->data_granularity;
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));

    if (cache_enabled == TRUE)
    {
        int bank_iter;
        int nof_eedb_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);

        

        for (bank_iter = 0; bank_iter < nof_eedb_banks; bank_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_direct_cache_alloc(unit, MDB_EEDB_BANK, 0, bank_iter));
        }
    }

    for (phase_iter = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1; phase_iter < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
         phase_iter++)
    {
        mdb_physical_table_e mdb_ll_table_id;
        mdb_physical_table_e mdb_data_bank_table_id;

        mdb_ll_table_id =
            dnx_data_mdb.eedb.phase_to_table_get(unit, MDB_EEDB_MEM_TYPE_LL, phase_iter)->mdb_physical_table_id;
        mdb_data_bank_table_id =
            dnx_data_mdb.eedb.phase_to_table_get(unit, MDB_EEDB_MEM_TYPE_DATA_BANK, phase_iter)->mdb_physical_table_id;

        SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.set(unit, phase_iter,
                                                           MDB_PAYLOAD_SIZE_TO_PAYLOAD_SIZE_TYPE(address_granularity
                                                                                                 [phase_iter])));

        SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate
                        (unit, dnx_data_mdb.eedb.phase_info_get(unit, phase_iter)->nof_available_banks,
                         0, NULL, mdb_ll_table_id));

            

        SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate
                        (unit, dnx_data_mdb.eedb.phase_info_get(unit, phase_iter)->nof_available_banks,
                         0, NULL, mdb_data_bank_table_id));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_data_bank_table_id, 0));

    }

exit:
    SHR_FUNC_EXIT;
}



int
mdb_parse_xml_em_estimate_calc_way(
    int max_way_basic_capacity,
    int encoding_type_util_percentage_arr[MDB_EM_NOF_ENCODING_TYPES],
    int encoding_type,
    int row_width,
    int flex_enabled)
{
    int encoding_type_temp = encoding_type;
    int capacity_estimate;

    

    if ((flex_enabled == TRUE) && (encoding_type_temp < MDB_EM_ENCODING_TYPE_QUARTER))
    {
        encoding_type_temp++;
    }

    

    capacity_estimate =
        max_way_basic_capacity * encoding_type_util_percentage_arr[encoding_type_temp] / 100 * (1 << encoding_type) *
        MDB_DIRECT_BASIC_ENTRY_SIZE / row_width;

    return capacity_estimate;
}



static shr_error_e
mdb_parse_xml_em_estimate_set(
    int unit,
    int nof_ways,
    mdb_physical_table_e mdb_physical_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    

    if (nof_ways > 0)
    {
        int encoding_type_iter;


        int encoding_type_util_percentage_arr[MDB_EM_NOF_ENCODING_TYPES] = { 0 };
        int encoding_type_util_percentage_avg_arr[MDB_EM_NOF_ENCODING_TYPES] = { 0 };
        uint32 row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->row_width;

        

        if (nof_ways >= 4)
        {
            if (dnx_data_mdb.em.feature_get(unit, dnx_data_mdb_em_step_table_any_cmds_support))
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 92;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 95;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 97;
            }
            else if (!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 86;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 95;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 97;
            }
            else
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 90;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 94;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 97;
            }
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_ONE] = 95;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_HALF] = 97;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 97;
        }
        else if (nof_ways == 3)
        {
            if (dnx_data_mdb.em.feature_get(unit, dnx_data_mdb_em_step_table_any_cmds_support))
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 85;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 94;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 95;
            }
            else if (!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 75;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 92;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 95;
            }
            else
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 75;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 88;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 94;
            }
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_ONE] = 90;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_HALF] = 95;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 95;
        }
        else if (nof_ways == 2)
        {
            if (dnx_data_mdb.em.feature_get(unit, dnx_data_mdb_em_step_table_any_cmds_support))
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 49;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 65;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 85;
            }
            else if (!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 32;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 65;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 84;
            }
            else
            {
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 32;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 60;
                encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 75;
            }
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_ONE] = 49;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_HALF] = 85;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 92;
        }
        else
        {
            encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_ONE] = 5;
            encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_HALF] = 5;
            encoding_type_util_percentage_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 5;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_ONE] = 5;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_HALF] = 5;
            encoding_type_util_percentage_avg_arr[MDB_EM_ENCODING_TYPE_QUARTER] = 5;
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "MDB EM table %s, only has a single hash.\n"),
                       dbal_physical_table_to_string(unit,
                                                     dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                        mdb_physical_table_id)->physical_to_logical)));
        }

        

        for (encoding_type_iter = 0; encoding_type_iter < DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES; encoding_type_iter++)
        {
            int capacity_estimate = 0;
            int capacity_estimate_avg = 0;
            int cluster_idx;
            int temp_capacity = 0;
            int nof_clusters;
            int flex_enabled = FALSE;
            uint8 cluster_flags;

            

            int cluster_glob_index;
            mdb_macro_types_e prev_macro_type = MDB_NOF_MACRO_TYPES;
            uint8 prev_macro_index = 0xff;
            uint8 prev_cluster_flags = 0x0;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
            {
                mdb_macro_types_e macro_type;
                uint8 macro_index;
                int cluster_rows;

                

                uint8 cluster_local_index;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                macro_type.get(unit, mdb_physical_table_id, cluster_idx, &macro_type));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                macro_index.get(unit, mdb_physical_table_id, cluster_idx, &macro_index));

                

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                cluster_index.get(unit, mdb_physical_table_id, cluster_idx, &cluster_local_index));

                cluster_glob_index =
                    MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macro_type, macro_index, cluster_local_index);

                SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                                flags.get(unit, macro_type, cluster_glob_index, &cluster_flags));

                

                if ((macro_type != prev_macro_type) || (macro_index != prev_macro_index) ||
                    ((cluster_flags & MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES) !=
                     (prev_cluster_flags & MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES)))
                {
                    capacity_estimate +=
                        mdb_parse_xml_em_estimate_calc_way(temp_capacity, encoding_type_util_percentage_arr,
                                                           encoding_type_iter, row_width, flex_enabled);
                    capacity_estimate_avg +=
                        mdb_parse_xml_em_estimate_calc_way(temp_capacity, encoding_type_util_percentage_avg_arr,
                                                           encoding_type_iter, row_width, flex_enabled);

                    prev_macro_type = macro_type;
                    prev_macro_index = macro_index;
                    prev_cluster_flags = cluster_flags;

                    flex_enabled = FALSE;
                    temp_capacity = 0;
                }

                

                if (cluster_flags & MDB_INIT_CLUSTER_FLAG_FLEX)
                {
                    flex_enabled = TRUE;
                }

                SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, macro_type, &cluster_rows));

                

                temp_capacity += (cluster_rows * MDB_NOF_CLUSTER_ROW_BITS / (MDB_DIRECT_BASIC_ENTRY_SIZE));
            }

            

            capacity_estimate += mdb_parse_xml_em_estimate_calc_way(temp_capacity, encoding_type_util_percentage_arr,
                                                                    encoding_type_iter, row_width, flex_enabled);
            capacity_estimate_avg +=
                mdb_parse_xml_em_estimate_calc_way(temp_capacity, encoding_type_util_percentage_avg_arr,
                                                   encoding_type_iter, row_width, flex_enabled);

            

            if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
            {
                capacity_estimate = capacity_estimate * 75 / 100;
                capacity_estimate_avg = capacity_estimate_avg * 75 / 100;
            }

#ifdef ADAPTER_SERVER_MODE

            

            capacity_estimate = MDB_EM_LPM_ADAPTER_SIZE;
            capacity_estimate_avg = MDB_EM_LPM_ADAPTER_SIZE;
#endif
            SHR_IF_ERR_EXIT(mdb_db_infos.em_entry_capacity_estimate.set(unit,
                                                                        dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                           mdb_physical_table_id)->
                                                                        physical_to_logical, encoding_type_iter,
                                                                        capacity_estimate));
            SHR_IF_ERR_EXIT(mdb_db_infos.
                            em_entry_capacity_estimate_average.set(unit,
                                                                   dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                      mdb_physical_table_id)->
                                                                   physical_to_logical, encoding_type_iter,
                                                                   capacity_estimate_avg));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

    

static shr_error_e
mdb_parse_xml_address_update_second_interface_use(
    int unit,
    int nof_cluster_per_entry,
    mdb_dh_macro_info_t * macro)
{
    int cluster_idx;
    uint32 max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);
    int second_intf_first_cluster = max_nof_macro_clusters;
    mdb_parse_xml_second_interface_fsm_e state = MDB_DH_INTERFACE_FSM_NO_CLUSTER_WAS_FOUND;
    mdb_dh_init_interface_instance_in_macro_e counter_type = MDB_DH_INIT_FIRST_IF_USED;
    SHR_FUNC_INIT_VARS(unit);
    macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated = 0;

    

    for (cluster_idx = 0; cluster_idx < max_nof_macro_clusters; cluster_idx++)
    {
        int bit = (macro->intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map >> cluster_idx) & 0x1;

        if ((state == MDB_DH_INTERFACE_FSM_NO_CLUSTER_WAS_FOUND) && (bit == 1))
        {
            macro->intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster = cluster_idx;
            state = MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_FIRST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_FIRST_CLUSTER_FOUND) && (bit == 0))
        {
            state = MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_LAST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_LAST_CLUSTER_FOUND) && (bit == 1))
        {
            counter_type = MDB_DH_INIT_SECOND_IF_USED;
            macro->intf[MDB_DH_INIT_SECOND_IF_USED].first_alloc_cluster = cluster_idx;
            state = MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND) && (bit == 0))
        {
            state = MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_LAST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_LAST_CLUSTER_FOUND) && (bit == 1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "MACRO %d of type %s has an invalid clusters allocation (more than two separate clusters groups).",
                         macro->macro_index, dnx_data_mdb.dh.macro_type_info_get(unit, macro->macro_type)->name);
        }

        macro->intf[counter_type].nof_cluster_allocated += bit;
    }

    if (state >= MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND)
    {
        second_intf_first_cluster = macro->intf[MDB_DH_INIT_SECOND_IF_USED].first_alloc_cluster;
    }
    else if (macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated > nof_cluster_per_entry)
    {

        

        int first_interface_count;

        if (nof_cluster_per_entry == 1)
        {
            first_interface_count =
                mdb_parse_xml_intf_1_em_120_clusters_division[macro->
                                                              intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated -
                                                              1];
        }
        else
        {
            
            first_interface_count =
                UTILEX_ALIGN_UP((macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated / 2),
                                nof_cluster_per_entry);
        }

        if (first_interface_count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "MACRO %d of type %s has an invalid clusters allocation (can't split the clusters between the two interfaces).",
                         macro->macro_index, dnx_data_mdb.dh.macro_type_info_get(unit, macro->macro_type)->name);
        }

        macro->intf[MDB_DH_INIT_SECOND_IF_USED].nof_cluster_allocated =
            macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated - first_interface_count;
        macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated = first_interface_count;
        second_intf_first_cluster = first_interface_count + macro->intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster;
        macro->intf[MDB_DH_INIT_SECOND_IF_USED].first_alloc_cluster = second_intf_first_cluster;
    }

    macro->intf[MDB_DH_INIT_SECOND_IF_USED].alloc_bit_map =
        macro->intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map & UTILEX_ZERO_BITS_MASK(second_intf_first_cluster - 1, 0);

    macro->intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map -= macro->intf[MDB_DH_INIT_SECOND_IF_USED].alloc_bit_map;
exit:
    SHR_FUNC_EXIT;
}

    

static void
mdb_parse_xml_address_update_flex_add(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    mdb_dh_macro_info_t * macro)
{
    int interface_iter;

    if (MDB_DH_IS_FULLY_SUPPORTS_FLEX(unit, mdb_physical_table_id))
    {
        
        for (interface_iter = 0; interface_iter < MDB_DH_INIT_MAX_AVAILABLE_INTERFACES; interface_iter++)
        {
            
            if ((macro->intf[interface_iter].nof_cluster_allocated > 0)
                && ((macro->intf[interface_iter].nof_cluster_allocated & 0x1) == 0))
            {
                macro->intf[interface_iter].flags |= MDB_INIT_CLUSTER_FLAG_FLEX;
                macro->intf[interface_iter].nof_cluster_per_entry = 2;
            }
        }
    }
}




static shr_error_e
mdb_parse_xml_single_pdb_cluster_address_update(
    int unit,
    uint32 skip_address_update,
    mdb_physical_table_e mdb_physical_table_id)
{
    int cluster_idx;
    int macro_global_index;
    int nof_clusters;
    int ways_count = 0;
    int flex_ways_count = 0;
    int nof_cluster_per_entry;
    int nof_rows_accumulation = 0;
    int nof_rows;
    int ads_nof_rows = 0;
    mdb_cluster_info_t cluster_info;
    uint8 is_kaps_bb = mdb_lpm_xpt_physical_table_is_bb(unit, mdb_physical_table_id);
    mdb_dh_macro_info_t macros[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES];

    

    uint32 min_way_capacity =
        dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_A)->nof_rows * dnx_data_mdb.dh.macro_type_info_get(unit,
                                                                                                               MDB_MACRO_A)->nof_clusters;
    uint32 max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);


    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.get(unit, mdb_physical_table_id, &nof_cluster_per_entry));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

    sal_memset(macros, 0x0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES * sizeof(mdb_dh_macro_info_t));
    for (macro_global_index = 0; macro_global_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_global_index++)
    {
        macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster = max_nof_macro_clusters;
    }

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, cluster_idx, &cluster_info));
        macro_global_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info.macro_type, cluster_info.macro_index);
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                        (&(macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map),
                         cluster_info.cluster_index));
        macros[macro_global_index].macro_index = cluster_info.macro_index;
        macros[macro_global_index].macro_type = cluster_info.macro_type;
        macros[macro_global_index].cluster_index[cluster_info.cluster_index] = cluster_idx;
        macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated++;
        macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster =
            UTILEX_MIN(macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster,
                       cluster_info.cluster_index);
    }

    

    if (is_kaps_bb)
    {
        mdb_macro_types_e ads_macro_type;
        mdb_physical_table_e ads_db = MDB_PHYSICAL_TABLE_ADS_1 +
            (mdb_physical_table_id - MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1);
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, ads_db, &nof_clusters));
        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.macro_type.get(unit, ads_db, cluster_idx, &ads_macro_type));
            ads_nof_rows += dnx_data_mdb.dh.macro_type_info_get(unit, ads_macro_type)->nof_rows;
        }
    }

    for (macro_global_index = 0; macro_global_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_global_index++)
    {
        if (macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map > 0)
        {
            mdb_dh_init_interface_instance_in_macro_e interface_iter;

            

            ways_count++;

            
            if (UTILEX_GET_BIT
                (dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->two_ways_connectivity_bm,
                 dnx_data_mdb.dh.dh_info_get(unit,
                                             mdb_physical_table_id)->table_macro_interface_mapping[macro_global_index]))
            {
                mdb_parse_xml_address_update_second_interface_use(unit, nof_cluster_per_entry,
                                                                  &macros[macro_global_index]);

                if (macros[macro_global_index].intf[MDB_DH_INIT_SECOND_IF_USED].nof_cluster_allocated > 0)
                {
                    macros[macro_global_index].intf[MDB_DH_INIT_SECOND_IF_USED].nof_cluster_per_entry =
                        nof_cluster_per_entry;

                    

                    ways_count++;
                }
            }

            macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_per_entry = nof_cluster_per_entry;

            mdb_parse_xml_address_update_flex_add(unit, mdb_physical_table_id, &macros[macro_global_index]);

            nof_rows = dnx_data_mdb.dh.macro_type_info_get(unit, macros[macro_global_index].macro_type)->nof_rows;

            if (is_kaps_bb)
            {

                
                nof_rows_accumulation = 0;
            }


            

            for (interface_iter = 0; interface_iter < MDB_DH_INIT_MAX_AVAILABLE_INTERFACES; interface_iter++)
            {
                if (macros[macro_global_index].intf[interface_iter].nof_cluster_allocated > 0)
                {
                    uint8 extra_intf_flag = 0;
                    int end_address = 0;
                    int clusters_bundle_iter;
                    int nof_bundles =
                        macros[macro_global_index].intf[interface_iter].nof_cluster_allocated /
                        macros[macro_global_index].intf[interface_iter].nof_cluster_per_entry;

                    if (interface_iter == MDB_DH_INIT_SECOND_IF_USED)
                    {
                        extra_intf_flag |= MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES;
                    }

                    if (is_kaps_bb
                        && (macros[macro_global_index].intf[interface_iter].nof_cluster_allocated ==
                            max_nof_macro_clusters))
                    {

                        

                        extra_intf_flag |=
                            (nof_rows <
                             (ads_nof_rows / 2)) ? MDB_INIT_CLUSTER_FLAG_480_BPU : MDB_INIT_CLUSTER_FLAG_960_BPU;
                    }

                    for (clusters_bundle_iter = 0; clusters_bundle_iter < nof_bundles; clusters_bundle_iter++)
                    {
                        int start_pos =
                            macros[macro_global_index].intf[interface_iter].first_alloc_cluster +
                            (clusters_bundle_iter *
                             macros[macro_global_index].intf[interface_iter].nof_cluster_per_entry);
                        for (cluster_idx = 0;
                             cluster_idx < macros[macro_global_index].intf[interface_iter].nof_cluster_per_entry;
                             cluster_idx++)
                        {

                            

                            uint8 first_cluster_flag = (((start_pos + cluster_idx) ==
                                                         macros[macro_global_index].
                                                         intf[interface_iter].first_alloc_cluster) ?
                                                        MDB_INIT_CLUSTER_FLAG_FIRST_INTF_CLUSTER : 0);

                            uint8 cluster_flags = first_cluster_flag
                                | macros[macro_global_index].intf[interface_iter].flags | extra_intf_flag;

                            int cluster_glob_index = MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macros
                                                                                                 [macro_global_index].
                                                                                                 macro_type,
                                                                                                 macros
                                                                                                 [macro_global_index].
                                                                                                 macro_index,
                                                                                                 start_pos +
                                                                                                 cluster_idx);
                            int cluster_sw_state_idx =
                                macros[macro_global_index].cluster_index[start_pos + cluster_idx];

                            if (!skip_address_update)
                            {
                                int start_address = nof_rows_accumulation;
                                end_address = nof_rows_accumulation + nof_rows;
                                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                                start_address.set(unit, mdb_physical_table_id, cluster_sw_state_idx,
                                                                  start_address));
                                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                                end_address.set(unit, mdb_physical_table_id, cluster_sw_state_idx,
                                                                end_address));
                            }

                            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                                            flags.set(unit, macros[macro_global_index].macro_type, cluster_glob_index,
                                                      cluster_flags));
                        }

                        nof_rows_accumulation += nof_rows;
                        if (is_kaps_bb && (nof_rows_accumulation >= (ads_nof_rows / 2)))
                        {

                            

                            nof_rows_accumulation = 0;
                        }

                    }

                    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->db_type ==
                        DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
                    {

                        

                        nof_rows_accumulation = 0;

                        

                        if (utilex_nof_on_bits_in_long(end_address) != 1)
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG,
                                         "%s DB in MDB profile XML macro %d of type %s has an EM way with capacity %d which is not a power of 2.",
                                         dbal_physical_table_to_string(unit,
                                                                       dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                          mdb_physical_table_id)->physical_to_logical),
                                         macros[macro_global_index].macro_index,
                                         dnx_data_mdb.dh.macro_type_info_get(unit,
                                                                             macros[macro_global_index].
                                                                             macro_type)->name, end_address);
                        }

                        min_way_capacity = UTILEX_MIN(min_way_capacity, end_address);

                        if (macros[macro_global_index].intf[interface_iter].flags & MDB_INIT_CLUSTER_FLAG_FLEX)
                        {
                            flex_ways_count += 1;
                        }
                    }
                }

            }
        }

    }

    if ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        && (ways_count > 0))
    {
        SHR_IF_ERR_EXIT(mdb_parse_xml_em_estimate_set(unit, ways_count, mdb_physical_table_id));

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.nof_address_bits.set(unit,
                                                                        dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                           mdb_physical_table_id)->
                                                                        physical_to_logical,
                                                                        utilex_log2_round_down(min_way_capacity)));

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.nof_ways.set(unit,
                                                                dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                   mdb_physical_table_id)->physical_to_logical,
                                                                ways_count));

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.nof_flex_ways.set(unit,
                                                                     dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                        mdb_physical_table_id)->physical_to_logical,
                                                                     flex_ways_count));
    }

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_parse_xml_clusters_address_update(
    int unit)
{
    uint32 skip_address_update;
    mdb_physical_table_e mdb_physical_table_id;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_1)
    {
#if defined(BCM_DNX2_SUPPORT)
        SHR_IF_ERR_EXIT(mdb_parse_xml_update_fec_hierarchy_address(unit));
#endif
    }
    else
    {
    }

    for (mdb_physical_table_id = 0; mdb_physical_table_id < MDB_NOF_PHYSICAL_TABLES; mdb_physical_table_id++)
    {
        {
            mdb_eedb_mem_type_e eedb_mem_type;

            eedb_mem_type = mdb_eedb_table_to_mem_type(unit, mdb_physical_table_id);

            
            skip_address_update =
                ((MDB_INIT_IS_FEC_TABLE(mdb_physical_table_id)) || (eedb_mem_type != MDB_NOF_EEDB_MEM_TYPES));
            SHR_IF_ERR_EXIT(mdb_parse_xml_single_pdb_cluster_address_update
                            (unit, skip_address_update, mdb_physical_table_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_parse_xml_open_xml(
    int unit,
    void **curTop)
{
    void *curTop_temp = NULL;
    char file_path[RHFILE_MAX_SIZE];
    char *mdb_profile_str = NULL;
    char mdb_profile_str_lower[SOC_PROPERTY_VALUE_MAX];
    uint8 mdb_profile_u8;
    int char_iter;

    SHR_FUNC_INIT_VARS(unit);

    mdb_profile_str = dnx_data_mdb.pdbs.mdb_profile_get(unit)->profile;

    

    sal_memset(mdb_profile_str_lower, 0x0, sizeof(mdb_profile_str_lower));
    sal_strncpy(mdb_profile_str_lower, mdb_profile_str, SOC_PROPERTY_VALUE_MAX - 1);
    utilex_str_to_lower(mdb_profile_str_lower);

    

    for (mdb_profile_u8 = 0; mdb_profile_u8 < DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES; mdb_profile_u8++)
    {
        char *mdb_profile_name;

        

        if (dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->supported == FALSE)
        {
            continue;
        }

        mdb_profile_name = dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->name;

        if (sal_strncmp(mdb_profile_str_lower, mdb_profile_name, sal_strlen(mdb_profile_name)) == 0)
        {

            

            if ((sal_strlen(mdb_profile_name) == sal_strlen(mdb_profile_str_lower))
                || (sal_strncmp("custom", mdb_profile_name, sal_strlen("custom")) == 0))
            {
                break;
            }
        }
    }

    if (mdb_profile_u8 >= DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES)
    {
        uint8 mdb_profile_iter_u8;
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Available values for mdb_profile soc property:\n")));
        for (mdb_profile_iter_u8 = 0; mdb_profile_iter_u8 < DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES;
             mdb_profile_iter_u8++)
        {

            

            if (dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_iter_u8)->supported == FALSE)
            {
                continue;
            }
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s\n"),
                       dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_iter_u8)->name));
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown MDB profile: %s\n", mdb_profile_str);
    }

    

    for (char_iter = 0; char_iter < sal_strlen(mdb_profile_str_lower); char_iter++)
    {
        if (mdb_profile_str_lower[char_iter] == '-')
        {
            mdb_profile_str_lower[char_iter] = '_';
        }
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_profile.set(unit, mdb_profile_u8));

    sal_memset(file_path, 0x0, sizeof(file_path));
    sal_strncpy(file_path, MDB_HW_ALLOCATION_DIR_PATH, sizeof(file_path));
    sal_strncat_s(file_path, "/", sizeof(file_path));

    

    sal_strncat_s(file_path, "mdb_profile_", sizeof(file_path));
    sal_strncat_s(file_path, mdb_profile_str_lower, sizeof(file_path));

    

    if (sal_strncmp("custom", dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->name, sal_strlen("custom"))
        != 0)
    {
        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_half_nof_clusters_enable))
        {
            sal_strncat_s(file_path, "_half_a", sizeof(file_path));
        }
        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_75_nof_clusters_enable))
        {
            sal_strncat_s(file_path, "_75_a", sizeof(file_path));
        }
    }
    sal_strncat_s(file_path, ".xml", sizeof(file_path));

    curTop_temp = dbx_pre_compiled_devices_top_get(unit, file_path, "opt", CONF_OPEN_PER_DEVICE);
    if (curTop_temp == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find MDB HW allocation xml file: %s\n", file_path);
    }

    *curTop = curTop_temp;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_parse_xml_direct_physical_map(
    int unit)
{
    void *curTop = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_parse_xml_open_xml(unit, &curTop));

    

    SHR_IF_ERR_EXIT(mdb_parse_xml_eedb_banks_map(unit));

    SHR_IF_ERR_EXIT(mdb_parse_xml_mdb_clusters_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_em_banks_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_clusters_address_update(unit));

    SHR_IF_ERR_EXIT(mdb_parse_xml_update_address_map(unit));

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_parse_xml_em_vmv(
    int unit)
{
    char db_name[MDB_XML_MAX_STRING_LENGTH];
    void *cur_top = NULL;
    void *cur_phy_db = NULL;
    char vmv_file_path[RHFILE_MAX_SIZE];
    char *image_name = NULL;
    uint8 is_standard_image = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&db_name, 0, sizeof(db_name));
    sal_memset(&vmv_file_path, 0, sizeof(vmv_file_path));

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    if (is_standard_image)
    {
        sal_strncpy(vmv_file_path, "mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    }
    else
    {
        sal_strncpy(vmv_file_path, "bare_metal/mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    }
    sal_strncat_s(vmv_file_path, image_name, sizeof(vmv_file_path));
    sal_strncat_s(vmv_file_path, "/", sizeof(vmv_file_path));
    sal_strncat_s(vmv_file_path, image_name, sizeof(vmv_file_path));
    sal_strncat_s(vmv_file_path, "_vmv_values.xml", sizeof(vmv_file_path));

    if (is_standard_image)
    {
        cur_top = dbx_pre_compiled_mdb_vmv_top_get(unit, vmv_file_path, "VmvCatalog", CONF_OPEN_PER_DEVICE);
    }
    else
    {
        cur_top = dbx_pre_compiled_mdb_vmv_top_get(unit, vmv_file_path, "VmvCatalog", 0);
    }
    if (cur_top == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find MDB HW allocation xml file: %s\n", vmv_file_path);
    }

    RHDATA_ITERATOR(cur_phy_db, cur_top, "PhyDB")
    {
        dbal_physical_tables_e dbal_table_id_iter;
        uint8 found = FALSE;

        RHDATA_GET_STR_STOP(cur_phy_db, "Name", db_name);

        for (dbal_table_id_iter = 0; dbal_table_id_iter < DBAL_NOF_PHYSICAL_TABLES; dbal_table_id_iter++)
        {
            if (dnx_data_mdb.pdbs.dbal_vmv_str_mapping_get(unit, dbal_table_id_iter)->valid == TRUE)
            {
                found =
                    sal_strncmp(db_name, dnx_data_mdb.pdbs.dbal_vmv_str_mapping_get(unit, dbal_table_id_iter)->xml_name,
                                MDB_XML_MAX_STRING_LENGTH) == 0 ? TRUE : FALSE;

                if (found)
                {
                    
                    dbal_physical_tables_e dbal_table_id_iter_inner;

                    void *cur_container;
                    RHDATA_ITERATOR(cur_container, cur_phy_db, "Container")
                    {
                        void *cur_vmv;
                        int size;
                        int vmv_size = 0, vmv_value = SAL_INT32_MAX;
                        mdb_em_entry_encoding_e encoding = MDB_EM_ENTRY_ENCODING_EMPTY;
                        RHDATA_GET_INT_STOP(cur_container, "Size", size);
                        if (size)
                        {
                            encoding =
                                utilex_log2_round_up(dnx_data_mdb.pdbs.
                                                     dbal_pdb_info_get(unit, dbal_table_id_iter)->row_width / size);
                        }

                        

                        if (encoding == MDB_EM_ENTRY_ENCODING_EIGHTH)
                        {
                            continue;
                        }

                        RHDATA_ITERATOR(cur_vmv, cur_container, "Vmv")
                        {
                            uint32 vmv_suffix;
                            int tmp_value, tmp_size;
                            RHDATA_GET_INT_STOP(cur_vmv, "Value", tmp_value);
                            RHDATA_GET_INT_STOP(cur_vmv, "Size", tmp_size);

                            for (vmv_suffix = 0;
                                 vmv_suffix < (1 << (dnx_data_mdb.em.max_nof_vmv_size_get(unit) - tmp_size));
                                 vmv_suffix++)
                            {
                                
                                dbal_table_id_iter_inner = dbal_table_id_iter;
                                do
                                {
                                    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                                    encoding.set(unit, dbal_table_id_iter_inner, tmp_value | vmv_suffix,
                                                                 encoding));
                                    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                                    size.set(unit, dbal_table_id_iter_inner, tmp_value | vmv_suffix,
                                                             tmp_size));
                                }
                                while (dnx_data_mdb.pdbs.dbal_vmv_str_mapping_get(unit,
                                                                                  dbal_table_id_iter_inner++)->vmv_xml_chain
                                       == TRUE);
                            }

                            if (tmp_value < vmv_value)
                            {
                                vmv_value = tmp_value;
                                vmv_size = tmp_size;
                            }
                        }

                        

                        if (vmv_value < SAL_INT32_MAX)
                        {
                            int vmv_bit_size = dnx_data_mdb.em.max_nof_vmv_size_get(unit);

                            if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_table_id_iter)->db_subtype ==
                                MDB_EM_TYPE_GLEM)
                            {
                                vmv_bit_size = dnx_data_mdb.em.glem_nof_vmv_size_get(unit);
                            }
                            else if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_table_id_iter)->db_subtype ==
                                     MDB_EM_TYPE_ESEM)
                            {
                                vmv_bit_size = dnx_data_mdb.em.esem_nof_vmv_size_get(unit);
                            }
                            else if ((dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_table_id_iter)->db_subtype ==
                                      MDB_EM_TYPE_LEM) && encoding == MDB_EM_ENTRY_ENCODING_HALF)
                            {
                                vmv_bit_size = dnx_data_mdb.em.mact_nof_vmv_size_get(unit);
                            }

                            if (vmv_bit_size < vmv_size)
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VMV size was set for table %s\n", db_name);
                            }

                            
                            dbal_table_id_iter_inner = dbal_table_id_iter;
                            do
                            {
                                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                                value.set(unit, dbal_table_id_iter_inner, encoding, vmv_value));
                                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                                size.set(unit, dbal_table_id_iter_inner, encoding, vmv_size));
                            }
                            while (dnx_data_mdb.pdbs.
                                   dbal_vmv_str_mapping_get(unit, dbal_table_id_iter_inner++)->vmv_xml_chain == TRUE);
                        }
                    }

                    break;
                }
            }
        }

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find matching DB to %s\n", db_name);
        }
    }

exit:
    dbx_xml_top_close(cur_top);
    SHR_FUNC_EXIT;
}
