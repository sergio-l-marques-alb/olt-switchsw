/** \file local_outlif_allocation.c
 *
 * Local outlif allocation file.
 *
 * The local outlif logical table has the following properties:
 * 1. Each outlifs can be accessed by one of eight logical phases. These determine the order that the outlif is accessed.
 * 2. The logical phase accessing the outlif is determined by its data bank. A data bank is several thousand consecutive outlifs.
 * 3. Each outlif can have a linked list pointer. This lif it's pointing to will be accessed next.
 * 4. The linked list pointers are also divided into banks, and each linked list bank is assigned to an outlif data bank.
 *
 *
 * The local outlif physical table has the following properties:
 *  0. Each outlif bank is mapped to a specific physical cluster. This is done by assigning a first outlif to the cluster.
 *  1. There are two different kinds of physical clusters: MDB clusters, which are spread across the MDB,
 *       and linked list (LL) clusters, which are smaller, and can either hold a linked list bank, or hold
 *       a data bank.
 *  2. The local outlif table can be accessed from 5 MAGs: 4 data MAGs access the MDB clusters, and return the data of
 *       the entries, and one linked list MAG accesses the EEDB banks and returns the next outlif pointer.
 *  3. Each data MAG can be accessed by two physical phases. These two phases are called "phase pair".
 *  4. Each logical phase is mapped to one physical phase.
 *  5. Each MAG has a granularity, which determines how many bits each outlif id points to. For example, an outlif entry of 80 bits would take
 *        three outlifs on 30b granularity, 2 outlifs on 60b granularity, and 1 entry on 120b granularity.
 *  6. The outlif banks don't have to be consecutive, but can have gaps between them. The first outlif of each cluster is independant of the
 *        start address of other clusters (except they can't be the same).
 *  7. A physical cluster may hold several logical banks, depending on the cluster size and phase granularity.
 *
 *  Allocation algorithm:
 *  Init:
 *  ----
 *
 *  Input:
 *  - Number and types of clusters available to each logical phase.
 *  - Granularity of each logical phase.
 *  - Logical to physical phase mapping.
 *
 *  Init sequence:
 *  1. Allocate all databases.
 *  2. For each MAG, for each cluster assigned to this MAG, Assign the required number of outlif banks for this cluster.
 *  3. For each logical phase, save the available linked list banks for this phase.
 *  4. Assign the first local outlif banks for rif.
 *
 *  Device run:
 *  ----------
 *
 *  Input:
 *   - Required logical phase.
 *   - Entry size.
 *   - Linked list required indication.
 *
 *  Allocation:
 *   1. Check if there's an outlif bank already assigned to this logical phase, with/without linked list.
 *   1.1 If there is, try to allocate in it.
 *   2. If there are no allocated banks available, translate the logical phase to MAG, and search for an unassigned bank on this MAG.
 *   2.1. If linked list is required, verify that there are unassigned linked lists banks for this logical phase.
 *   2.2. If an outlif bank (and linked list bank) were found, then use them to allocate an entry.
 *
 *  Entry allocation in bank:
 *   - The number of outlifs taken by an entry is determined by the MAG's (and therefore, the bank's) granularity, and the entry size.
 *   - The entry must be aligned to the start of an line in the outlif bank. E.g., in 30b granularity, each line hold 4 outlifs,
 *       so a 120b entry must be aligned to 4.
 *   - The res_tag_bitmap does this for us.
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*************
 * INCLUDES  *
 *************/
/*
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/swstate/auto_generated/access/lif_mngr_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include "lif_mngr_internal.h"
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/**
 * \brief Resource name for the resource manager.
 */
#define DNX_ALGO_LOCAL_OUTLIF_RESOURCE          "Local outlif."

/*
 * \brief Nof eedb phase pairs that exist in the device.
 */
#define NOF_EEDB_PHASE_PAIRS        (DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES / 2)

/*
 * \brief Maximum EEDB table granularity in the device.
 */
#define EEDB_MAX_GRANULARITY        MDB_NOF_CLUSTER_ROW_BITS

/*
 * \brief Mimimum EEDB table granularity in the device.
 */
#define EEDB_MIN_GRANLARITY         MDB_DIRECT_BASIC_ENTRY_SIZE
/*
 * \brief Maximum size of entry in the data banks.
 */
#define EEDB_MAX_DATA_ENTRY_SIZE    MDB_NOF_CLUSTER_ROW_BITS

/**
 * \brief The EEDB granularity is a multiple of this number.
 */
#define EEDB_GRANULARITY_UNIT       EEDB_MIN_GRANLARITY

/**
 * \brief This tag value can be used as an illegal tag because it's impossible
 * to get it from the macro \ref OUTLIF_RESOURCE_ALLOCATION_SIZE_TO_TAG
 */
#define OUTLIF_RESOURCE_ILLEGAL_TAG     (0x3)

/*
 * }
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/**
 * \brief Map between dbal physical table to eedb physical phase and vice versa.
 */
#define EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(_physical_phase) (_physical_phase + DBAL_PHYSICAL_TABLE_EEDB_1)

#define EEDB_PHYSICAL_TABLE_TO_PHYSICAL_PHASE(_physical_table) (_physical_table - DBAL_PHYSICAL_TABLE_EEDB_1)

/**
 * \brief Given an EEDB physical phase returns the index of the pair that this phase belongs to.
 */
#define EEDB_PHYSICAL_TABLE_PHYSICAL_PHASE_TO_PHASE_PAIR(_array_index) \
        (_array_index / 2)

/**
 * \brief Given an EEDB phase pair, returns the first phase of this pair.
 */
#define EEDB_PHYSICAL_TABLE_PHASE_PAIR_TO_FIRST_PHYSICAL_PHASE(_phase_pair) \
        (_phase_pair * 2)

/**
 * \brief Given an EEDB physical phase (as array index) returns the other phase in the pair. The partners are differentiated by their lsb.
 */
#define EEDB_PHYSICAL_PHASE_TO_PARTNER_PHASE(_eedb_physical_phase) \
        (_eedb_physical_phase ^ 1)

/**
 * Macros for the advanced algorithm:
 */
#define OUTLIF_RESOURCE_ALLOCATION_SIZE_TO_TAG(_allocation_size) (utilex_msb_bit_on(_allocation_size))

#define OUTLIF_RESOURCE_TAG_TO_ALLOCATION_SIZE(_tag)    (1 << (_tag))
/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/**
 * \brief Criteria to use when assigning an outlif bank.
 *
 * Bank assignment signifies whether it's mapped to some physical DB.
 *
 * A bank can be assigned to either an MDB cluster, or available/assigned to an EEDB bank.
 */
typedef enum
{
    DNX_ALGO_OUTLIF_BANK_UNASSIGNED,
    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB,
    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB
} dnx_algo_outlif_bank_assignment_mode_e;
/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * RESOURCE MANAGER ADVANCED ALGORTIHM - START
 *
 * The advanced algorithm is responsible for managing the lines of the tables and preventing fragmentation.
 *
 * The fragmentation is prevented by assigning each line in the outlif table (120b) a tag, and allocating
 * entries according to this size. E.g., if the granularity is 30b, then the tag would encompass 4 outlifs,
 * and these 30b, 60b and 120b entries would have their own unique tag to guarantee that each 4 entries would hold
 * either 4 30b entries, or 2 60b entries.
 *
 * This is guaranteed by the res_tag_bitmap.
 *
 * {
 */

/**
 * \brief
 *
 * Create the advanced algorithm instance. See the algorithm's description above.
 *
 * Full header description in \ref dnx_algo_res_create_cb
 *
 * \see
 *  dnx_algo_res_create_cb
 */
shr_error_e
dnx_algo_local_outlif_resource_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int outlif_bank,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    int granularity;
    sw_state_resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Prepare create struct.
     */
    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(sw_state_resource_tag_bitmap_create_info_t));
    res_tag_bitmap_create_info.count = create_data->nof_elements;
    res_tag_bitmap_create_info.low_id = create_data->first_element;
    res_tag_bitmap_create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;

    if (create_data->first_element == 1)
    {
        /*
         * In case the rif is calling the advanced algorithm create, then it will specify 1 as its first element and
         * nof_rifs - 1 as the number of elements. In this case, we need to round up the bitmap's count,
         * otherwise we'll get an error when creating it.
         */
        res_tag_bitmap_create_info.count++;
        res_tag_bitmap_create_info.low_id = 0;
    }

    granularity = *((int *) extra_arguments);

    res_tag_bitmap_create_info.grain_size = EEDB_MAX_DATA_ENTRY_SIZE / granularity;
    res_tag_bitmap_create_info.max_tag_value = utilex_msb_bit_on(res_tag_bitmap_create_info.grain_size);

    if (res_tag_bitmap_create_info.grain_size == 1)
    {
        /*
         * A grain of 1 means we get no optimization by using grains, so we may as well have only
         * one grain for the entire resource range.
         */
        res_tag_bitmap_create_info.grain_size = res_tag_bitmap_create_info.count;
        res_tag_bitmap_create_info.max_tag_value = 0;
    }

    if (_SHR_IS_FLAG_SET(create_data->flags, DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_FORCING_TAG))
    {
        /*
         * If we got the allow forcing tag flag, it means that we need to support the illegal tag as the
         * max tag value.
         */
        res_tag_bitmap_create_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;
        res_tag_bitmap_create_info.max_tag_value = OUTLIF_RESOURCE_ILLEGAL_TAG;
    }

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_create_info, nof_elements, alloc_flags));

    if (create_data->first_element == 1)
    {
        /*
         * In case the rif is calling the advanced algorithm create, then it will specify 1 as its first element and
         * nof_rifs - 1 as the number of elements. We need to allocate 1 so it will be unusable.
         * It will not show up when iterating over rif, because the resource manager will start the iteration from 1.
         */
        sw_state_resource_tag_bitmap_alloc_info_t alloc_info;
        int element;

        sal_memset(&alloc_info, 0, sizeof(alloc_info));

        alloc_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
        alloc_info.tag = 0;
        alloc_info.count = 1;
        element = 0;
        SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc(unit, module_id, *res_tag_bitmap, alloc_info, &element));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * See above a description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_allocate_cb
 *
 * \see
 * dnx_algo_res_allocate_cb
 */
shr_error_e
dnx_algo_local_outlif_resource_allocate(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    dnx_algo_lif_local_outlif_resource_info_t *outlif_info;
    uint32 allocation_size;
    sw_state_resource_tag_bitmap_alloc_info_t alloc_info;
    uint8 alloc_simulation, with_id;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&alloc_info, 0, sizeof(alloc_info));

    alloc_simulation = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION);

    if (alloc_simulation)
    {
        alloc_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }

    with_id = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID);

    if (with_id)
    {
        alloc_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
    }

    outlif_info = (dnx_algo_lif_local_outlif_resource_info_t *) extra_arguments;

    allocation_size = outlif_info->nof_outlifs_per_entry;

    if (outlif_info->use_tag_and_align)
    {
        /*
         * Align the entry if it's required.
         * The tag represents the size of the entry. Since the size of the entry is always a power of 2,
         * we can optimize and use MSB_BIT_ON to represent it.
         *
         */
        alloc_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
        alloc_info.align = allocation_size;
        alloc_info.count = allocation_size;
        alloc_info.tag = OUTLIF_RESOURCE_ALLOCATION_SIZE_TO_TAG(allocation_size);
    }
    else
    {
        /*
         * Don't align the entry if it's not required.
         * So just set the count.
         */
        alloc_info.count = allocation_size;
    }
    rv = sw_state_resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, alloc_info, element);

    if (!alloc_simulation)
        SHR_IF_ERR_EXIT(rv);
    else
        SHR_SET_CURRENT_ERR(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The advanced fucntionality in this function compared to the basic algorithm is that it only returns TRUE
 * for the first index that an outlif takes, instead of all the indexes like a simple bitmap would return.
 *
 * We do this because we assume that because outlif allocation is internal to the module and not exposed
 * to the rest of the SDK, it will not be necessary to ever check if a local outlif is allocated for anything other
 * than iterations. If that changes, then this needs to be reviewed.
 *
 * \see
 * dnx_algo_res_is_allocated_cb
 */
shr_error_e
dnx_algo_local_outlif_resource_is_allocated(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    uint32 tag;
    uint8 allocation_size;
    SHR_FUNC_INIT_VARS(unit);

    if (res_tag_bitmap == NULL)
    {
        *is_allocated = FALSE;
        SHR_EXIT();
    }

    /*
     * Get the tag. Only if the index is a multiple of the entry size then it's considered allocated.
     * Note that we can get the tag on banks that are allocated with
     */
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, element, &tag));

    allocation_size = OUTLIF_RESOURCE_TAG_TO_ALLOCATION_SIZE(tag);

    if (element % allocation_size == 0)
    {
        /*
         * The element is a multiple of the entry size. In that case, the simple bitmap
         * can handle it.
         */
        SHR_IF_ERR_EXIT(dnx_algo_res_simple_bitmap_is_allocated
                        (unit, module_id, res_tag_bitmap, element, is_allocated));
    }
    else
    {
        *is_allocated = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 * RESOURCE MANAGER ADVANCED ALGORTIHM - END
 */

/*
 * OUTLIF ALLOCATION FUNCTIONS - START
 * {
 */
shr_error_e
dnx_algo_local_outlif_phase_enum_to_logical_phase_num(
    int unit,
    lif_mngr_outlif_phase_e outlif_phase,
    dnx_algo_local_outlif_logical_phase_e * logical_phase)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(logical_phase, _SHR_E_PARAM, "phy_table");

    switch (outlif_phase)
    {
        case LIF_MNGR_OUTLIF_PHASE_AC:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8;
            break;
        case LIF_MNGR_OUTLIF_PHASE_ARP:
        case LIF_MNGR_OUTLIF_PHASE_IPV6_RAW_SRV6_TUNNEL:
        case LIF_MNGR_OUTLIF_PHASE_RCH:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7;
            break;
        case LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP:
        case LIF_MNGR_OUTLIF_PHASE_SRV6_BASE:
        case LIF_MNGR_OUTLIF_PHASE_REFLECTOR:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2;
            break;
        case LIF_MNGR_OUTLIF_PHASE_NATIVE_AC:
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1:
        case LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1:
        case LIF_MNGR_OUTLIF_PHASE_VPLS_1:
        case LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1:
        case LIF_MNGR_OUTLIF_PHASE_SFLOW:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3;
            break;
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2:
        case LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_2:
        case LIF_MNGR_OUTLIF_PHASE_VPLS_2:
        case LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_2:
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4;
            break;
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3:
        case LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3:
        case LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_3:
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5;
            break;
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_4:
        case LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4:
        case LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_4:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6;
            break;
        case LIF_MNGR_OUTLIF_PHASE_RIF:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Something went wrong - no eedb phase mapping for eedb phase enum %d.", outlif_phase);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Return all the information required to configure the HW of this outlif's bank.
 * Use outlif_hw_info->flags to specify the required information.
 */
shr_error_e
dnx_algo_local_outlif_bank_hw_info_get(
    int unit,
    int local_outlif,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int outlif_bank;
    uint8 ll_in_use;
    uint8 logical_phase, physical_phase, tmp;
    uint8 current_bank_in_cluster;
    SHR_FUNC_INIT_VARS(unit);

    outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(local_outlif);

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, outlif_bank, &logical_phase));

    outlif_hw_info->logical_phase = logical_phase;

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.physical_phase.get(unit, logical_phase, &physical_phase));

    /*
     * Read the data cluster info.
     */
    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK))
    {
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.get(unit, outlif_bank, &tmp));

        if (tmp == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB)
        {
            int nof_outlifs_per_eedb_bank, eedb_bank_array_index;
            uint8 address_granularity;

            /*
             * If this bank is stored in an EEDB bank, then read the EEDB bank data.
             */

            outlif_hw_info->data_bank_info.macro_type = MDB_EEDB_BANK;

            outlif_hw_info->data_bank_info.mdb_eedb_type = MDB_EEDB_MEM_TYPE_PHY;

            outlif_hw_info->data_bank_info.dbal_physical_table_id =
                EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(physical_phase);

            /*
             * Calculate the number of eedb banks per outlif bank for this phase.
             * We'll use this value below.
             */
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            address_granularity.get(unit, logical_phase, &address_granularity));

            nof_outlifs_per_eedb_bank = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity * MDB_NOF_ROWS_IN_EEDB_BANK;

            eedb_bank_array_index = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_OFFSET(local_outlif) / nof_outlifs_per_eedb_bank;

            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.get(unit,
                                                                                   outlif_bank,
                                                                                   eedb_bank_array_index, &tmp));
            outlif_hw_info->data_bank_info.mdb_eedb_type = MDB_EEDB_MEM_TYPE_PHY;

            outlif_hw_info->data_bank_info.macro_type = MDB_EEDB_BANK;

            outlif_hw_info->data_bank_info.cluster_idx = tmp;

            outlif_hw_info->data_bank_info.macro_idx = 0;

            outlif_hw_info->data_bank_info.logical_start_address =
                outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) + nof_outlifs_per_eedb_bank * eedb_bank_array_index;

            outlif_hw_info->data_bank_info.phase_alloc_logical_entries[0] = physical_phase;

            for (current_bank_in_cluster = 1;
                 current_bank_in_cluster < (MDB_NOF_ROWS_IN_MACRO_A_CLUSTER / MDB_NOF_ROWS_IN_EEDB_BANK);
                 current_bank_in_cluster++)
            {
                outlif_hw_info->data_bank_info.phase_alloc_logical_entries[current_bank_in_cluster] =
                    DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
            }
        }
        else
        {
            /*
             * If this bank is stored in an MDB macro, then it was already configured on init.
             * Nothing to do here.
             */
        }
    }

    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK))
    {
        /*
         * Check if the outlif bank has ll. If so, return the ll physical bank.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, outlif_bank, &ll_in_use));
        if (ll_in_use)
        {
            uint8 ll_index;

            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.get(unit, outlif_bank, &ll_index));

            outlif_hw_info->ll_bank_info.macro_type = MDB_EEDB_BANK;
            outlif_hw_info->ll_bank_info.macro_idx = 0;
            outlif_hw_info->ll_bank_info.dbal_physical_table_id = EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(physical_phase);
            outlif_hw_info->ll_bank_info.mdb_eedb_type = MDB_EEDB_MEM_TYPE_LL;

            outlif_hw_info->ll_bank_info.cluster_idx = ll_index;

            outlif_hw_info->ll_bank_info.logical_start_address = outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);

            /*
             * Set all the outlif banks in the cluster to don't care, except for the first one.
             * Linked list holds just one bank.
             */
            outlif_hw_info->ll_bank_info.phase_alloc_logical_entries[0] = physical_phase;
            for (current_bank_in_cluster = 1;
                 current_bank_in_cluster < (MDB_NOF_ROWS_IN_MACRO_A_CLUSTER / MDB_NOF_ROWS_IN_EEDB_BANK);
                 current_bank_in_cluster++)
            {
                outlif_hw_info->ll_bank_info.phase_alloc_logical_entries[current_bank_in_cluster] =
                    DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong - outlif %d doesn't have a linked list assigned.",
                         local_outlif);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_local_outlif_get_nof_available_ll_banks(
    int unit,
    int logical_phase,
    int *nof_available_banks)
{
    int count;
    uint8 first_ll_bank, last_ll_bank, nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_ll_bank.get(unit, logical_phase, &first_ll_bank));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_ll_bank.get(unit, logical_phase, &last_ll_bank));

    nof_banks = last_ll_bank - first_ll_bank + 1;

    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    bank_occupation_bitmap.bit_range_count(unit, first_ll_bank, nof_banks, &count));

    *nof_available_banks = (nof_banks - count);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Given a logical phase, verify if there are linked list banks assigned to this
 * logical phase that weren't assigned to a specific outlif range yet.
 */
static shr_error_e
dnx_algo_local_outlif_verify_ll_availability(
    int unit,
    int logical_phase,
    int nof_required_banks,
    int *is_available)
{
    int nof_available_banks = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_get_nof_available_ll_banks(unit, logical_phase, &nof_available_banks));

    *is_available = nof_required_banks <= nof_available_banks;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given a logical phase, find an eedb banks assigned to this
 * logical phase that wasn't assigned already, and mark it as assigned.
 */
static shr_error_e
dnx_algo_local_outlif_eedb_bank_allocate(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int *bank)
{
    uint8 first_ll_bank, last_ll_bank, ll_bank_used, current_ll_bank;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the used ll banks bitmap, and search for an available bank.
     */

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_ll_bank.get(unit, logical_phase, &first_ll_bank));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_ll_bank.get(unit, logical_phase, &last_ll_bank));

    for (current_ll_bank = first_ll_bank; current_ll_bank <= last_ll_bank; current_ll_bank++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        bank_occupation_bitmap.bit_get(unit, current_ll_bank, &ll_bank_used));
        if (!ll_bank_used)
        {
            break;
        }
    }

    if (current_ll_bank == last_ll_bank + 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Sanity failed: an ll banks is supposed to be available for phase %d, but none was found.",
                     logical_phase);
    }

    /*
     * Set the ll bank as in use.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.bank_occupation_bitmap.bit_set(unit, current_ll_bank));

    *bank = current_ll_bank;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an outlif bank, logical phase and linked list requirement, maps the outlif bank to the
 * specified logical phase, allocates a linked list bank and maps it to this outlif bank.
 */
static shr_error_e
dnx_algo_local_outlif_bank_in_use_set(
    int unit,
    int bank,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int ll_required)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First make sure that ll allocation works properly.
     */
    if (ll_required)
    {
        int eedb_bank;
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_eedb_bank_allocate(unit, logical_phase, &eedb_bank));

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.set(unit, bank, eedb_bank));
    }

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.set(unit, bank, TRUE));

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.set(unit, bank, logical_phase));

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.set(unit, bank, ll_required));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an outlif bank, unassigns it, and unassigns its attached linked list if exists.
 */
static shr_error_e
dnx_algo_local_outlif_bank_in_use_unset(
    int unit,
    int bank)
{
    uint8 ll_in_use;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if we need to free a linked list as well.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, bank, &ll_in_use));

    if (ll_in_use)
    {
        uint8 used_ll_bank;

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.get(unit, bank, &used_ll_bank));

        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.bank_occupation_bitmap.bit_clear(unit, used_ll_bank));

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.set(unit, bank, 0));

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.set(unit, bank, FALSE));
    }

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.set(unit, bank, 0));

    /*
     * Set bank as not in use. The rest of the fields will be invalid so no need to reset them.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.set(unit, bank, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function updates an outlif bank's sw state to point to eedb banks,
 * as well as a logical phase's sw state to point to this outlif bank as an eedb bank.
 */
static shr_error_e
dnx_algo_local_outlif_bank_prefix_for_eedb_bank_assign(
    int unit,
    int granularity,
    int nof_eedb_banks_per_outlif_bank,
    int logical_phase,
    uint32 outlif_bank)
{
    int current_outlif_prefix, outlif_prefix_index;
    uint8 tmp;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the prefix as in use, the bank as assigned and the outlif bank sw state to point to an eedb bank.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                    assigned.set(unit, outlif_bank, DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB));

    current_outlif_prefix =
        MDB_OUTLIF_AND_GRANULARITY_TO_PREFIX(unit, outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit), granularity);
    for (outlif_prefix_index = 0; outlif_prefix_index < nof_eedb_banks_per_outlif_bank; outlif_prefix_index++)
    {
        /*
         * Set the prefixes as in use.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.outlif_prefix_occupation_bitmap.bit_set(unit,
                                                                                                  current_outlif_prefix
                                                                                                  +
                                                                                                  outlif_prefix_index));
    }

    for (outlif_prefix_index = 0; outlif_prefix_index < MAX_EEDB_BANKS_PER_OUTLIF_BANK; outlif_prefix_index++)
    {
        /*
         * Set the outlif bank eedb banks as invalid, until they'll be allocated.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.set(unit,
                                                                               outlif_bank,
                                                                               outlif_prefix_index,
                                                                               DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK));
    }

    /*
     * Add the new bank to the phase's linked list.
     */
    tmp = outlif_bank;
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.add_first(unit, logical_phase, &tmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function removes the outlif bank's sw state pointer to eedb banks,
 * as well as the logical phase's sw state pointer to this outlif bank as an eedb bank.
 */
static shr_error_e
dnx_algo_local_outlif_bank_prefix_for_eedb_bank_unassign(
    int unit,
    int granularity,
    int nof_eedb_banks_per_outlif_bank,
    int logical_phase,
    uint32 outlif_bank)
{
    int current_outlif_prefix, outlif_prefix_index;
    uint8 tmp;
    sw_state_ll_node_t current_node;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the prefix as not in use, the bank as unassigned and remove the outlif bank sw state pointer.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                    assigned.set(unit, outlif_bank, DNX_ALGO_OUTLIF_BANK_UNASSIGNED));

    current_outlif_prefix =
        MDB_OUTLIF_AND_GRANULARITY_TO_PREFIX(unit, outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit), granularity);
    for (outlif_prefix_index = 0; outlif_prefix_index < nof_eedb_banks_per_outlif_bank; outlif_prefix_index++)
    {
        /*
         * Unset the prefixes as in use.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.outlif_prefix_occupation_bitmap.bit_clear(unit,
                                                                                                    current_outlif_prefix
                                                                                                    +
                                                                                                    outlif_prefix_index));
    }

    /*
     * Add the new bank to the phase's linked list.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.get_first(unit, logical_phase, &current_node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(current_node))
    {
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.node_value(unit, current_node, &tmp));

        if (tmp == outlif_bank)
        {
            break;
        }

        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.next_node(unit, logical_phase, current_node, &current_node));
    }

    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.remove_node(unit, logical_phase, current_node));

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function looks for an outlif bank that can be mapped to an eedb bank to be used as data. The outlif bank needs to not be used by
 * any MDB cluster, and not be in conflict with any existing outlif bank.
 */
static shr_error_e
dnx_algo_local_outlif_bank_prefix_for_eedb_bank_allocate(
    int unit,
    int granularity,
    int nof_eedb_banks_for_granularity,
    uint32 *outlif_bank)
{
    int current_outlif_bank, current_outlif_prefix, outlif_prefix_index;
    uint8 bank_assigned, prefix_in_use;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Run on all outlif banks. When a free one is found, check if the prefixes for all its eedb banks don't conflict with existing
     * eedb banks' prefixes.
     */
    for (current_outlif_bank = 0; current_outlif_bank < DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit); current_outlif_bank++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.get(unit, current_outlif_bank, &bank_assigned));

        if (bank_assigned == DNX_ALGO_OUTLIF_BANK_UNASSIGNED)
        {
            current_outlif_prefix =
                MDB_OUTLIF_AND_GRANULARITY_TO_PREFIX(unit, current_outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit),
                                                     granularity);
            for (outlif_prefix_index = 0; outlif_prefix_index < nof_eedb_banks_for_granularity; outlif_prefix_index++)
            {
                SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.outlif_prefix_occupation_bitmap.bit_get(unit,
                                                                                                          current_outlif_prefix
                                                                                                          +
                                                                                                          outlif_prefix_index,
                                                                                                          &prefix_in_use));

                if (prefix_in_use)
                {
                    /*
                     * We can't use this outlif bank because its prefix is already in use.
                     * Break and continue checking outlif banks.
                     */
                    break;
                }
            }

            if (outlif_prefix_index == nof_eedb_banks_for_granularity)
            {
                /*
                 * If the loop finished, it means we can use this outlif bank.
                 * Break the loop.
                 */
                break;
            }
        }
    }

    /*
     * If we finished the loop and it didn't break, it means that either there are no
     * available outlif banks, or they all conflict with existing eedb banks.
     * Return E_RESOURCE.
     */
    if (current_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "There are no more outlif banks available for use.");
    }

    /*
     * Return the outlif bank.
     */
    *outlif_bank = current_outlif_bank;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Given an outlif bank that's mapped to eedb banks, allocates an eedb bank, sets it in the outlif bank's sw state, and
 * marks its elements available in the resource manager.
 */
static shr_error_e
dnx_algo_local_outlif_assign_eedb_bank_to_outlif_bank(
    int unit,
    int outlif_bank,
    int logical_phase,
    int eedb_bank_offset_in_outlif_bank,
    int nof_outlifs_per_eedb_bank)
{
    int eedb_bank;
    simple_bitmap_wrap_tag_info_t tag_set_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_eedb_bank_allocate(unit, logical_phase, &eedb_bank));

    /*
     * Set the eedb bank in the outlif bank's eedb bank list.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.set(unit,
                                                                           outlif_bank,
                                                                           eedb_bank_offset_in_outlif_bank, eedb_bank));

    /*
     * Unset the illegal tag from these entries so they can be allocated.
     */
    sal_memset(&tag_set_info, 0, sizeof(simple_bitmap_wrap_tag_info_t));
    tag_set_info.force_tag = FALSE;
    tag_set_info.tag = 0;
    tag_set_info.nof_elements = nof_outlifs_per_eedb_bank;
    tag_set_info.element = OUTLIF_BANK_FIRST_INDEX(unit, outlif_bank) +
        nof_outlifs_per_eedb_bank * eedb_bank_offset_in_outlif_bank;

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.advanced_algorithm_info_set(unit, outlif_bank, &tag_set_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Given an outlif bank that's mapped to eedb banks, unsets the eedb bank from the outlif bank's sw state, and
 * marks its elements unavailable in the resource manager, and marks the bank as clear.
 */
static shr_error_e
dnx_algo_local_outlif_unassign_eedb_bank_to_outlif_bank(
    int unit,
    int outlif_bank,
    int eedb_bank_offset_in_outlif_bank,
    int nof_outlifs_per_eedb_bank)
{
    uint8 eedb_bank;
    simple_bitmap_wrap_tag_info_t tag_set_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.get(unit,
                                                                           outlif_bank,
                                                                           eedb_bank_offset_in_outlif_bank,
                                                                           &eedb_bank));

    /*
     * Set the eedb bank in the outlif bank's eedb bank list.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.set(unit,
                                                                           outlif_bank,
                                                                           eedb_bank_offset_in_outlif_bank,
                                                                           DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK));

    /*
     * Reset the illegal tag for these entries so they won't be used anymore.
     */
    sal_memset(&tag_set_info, 0, sizeof(simple_bitmap_wrap_tag_info_t));
    tag_set_info.force_tag = TRUE;
    tag_set_info.tag = OUTLIF_RESOURCE_ILLEGAL_TAG;
    tag_set_info.nof_elements = nof_outlifs_per_eedb_bank;
    tag_set_info.element = OUTLIF_BANK_FIRST_INDEX(unit, outlif_bank) +
        nof_outlifs_per_eedb_bank * eedb_bank_offset_in_outlif_bank;

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.advanced_algorithm_info_set(unit, outlif_bank, &tag_set_info));

    /*
     * Clear this eedb bank's in use indication.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.bank_occupation_bitmap.bit_clear(unit, eedb_bank));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an outlif bank, required logical phase and linked list requirement
 * returns whether the bank matches the required criteria.
 */
static shr_error_e
dnx_algo_local_outlif_check_bank(
    int unit,
    int bank,
    dnx_algo_local_outlif_logical_phase_e required_logical_phase,
    int ll_required,
    dnx_algo_lif_local_outlif_resource_info_t * resource_info,
    int *bank_match,
    int *bank_can_be_used)
{
    uint8 bank_logical_phase;
    uint8 ll_in_use;
    int element;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * We'll set it to true if it's a match.
     */
    *bank_can_be_used = FALSE;
    *bank_match = FALSE;

    /*
     * If bank is already in use, check that it's used by the correct logical phase.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, bank, &bank_logical_phase));

    if (bank_logical_phase == required_logical_phase)
    {
        /*
         * Logical phase fits. Now check that the LL fits the requirement.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, bank, &ll_in_use));

        if ((ll_in_use && ll_required) || (!ll_in_use && !ll_required))
        {
            *bank_match = TRUE;
        }
        else
        {
            /*
             * Bank doesn't match and can't be used. Return false on both (already set above).
             */
            SHR_EXIT();
        }
    }
    else
    {
        /*
         * Bank doesn't match and can't be used. Return false on both (already set above).
         */
        SHR_EXIT();
    }

    /*
     * If the bank fits the criteria, run an allocation simulation.
     */
    rv = lif_mngr_db.egress_local_lif.allocate_single(unit, bank,
                                                      SW_STATE_ALGO_RES_ALLOCATE_SIMULATION,
                                                      (void *) resource_info, &element);

    if (rv == _SHR_E_NONE)
    {
        /*
         * Allocation successful.
         * Return TRUE, and the entry will be allocated.
         */
        *bank_can_be_used = TRUE;
    }
    else if (rv != _SHR_E_RESOURCE)
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given lif_mngr outlif info, translates it to the resource manager allocation info.
 * The logical phase can be passed if it's known (for example, in free operation).
 *  Otherwise, it should be DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT.
 */
shr_error_e
dnx_algo_local_outlif_info_to_resource_info(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_lif_local_outlif_resource_info_t * resource_info)
{
    int entry_size_in_bits;
    uint8 address_granularity;
    uint8 ll_required;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the required size of the entry (in bits).
     */
    SHR_IF_ERR_EXIT(dbal_tables_mdb_hw_payload_size_get
                    (unit, outlif_info->dbal_table_id, outlif_info->dbal_result_type, &entry_size_in_bits));

    SHR_IF_ERR_EXIT(dbal_tables_eedb_link_list_indication_get(unit,
                                                              outlif_info->dbal_table_id, outlif_info->dbal_result_type,
                                                              &ll_required));

    if (ll_required)
    {
        entry_size_in_bits -= MDB_DIRECT_BASIC_ENTRY_SIZE;
    }

    /*
     * Get the logical phase, then get its granularity.
     * If the phase is invalid, then get the phase by the outlif, which should be set.
     * Since this is set by physical phase, we need to get it first.
     */
    if (outlif_info->outlif_phase != LIF_MNGR_OUTLIF_PHASE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                        (unit, outlif_info->outlif_phase, &logical_phase));
    }
    else
    {
        int outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(outlif_info->local_outlif);
        uint8 tmp;
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, outlif_bank, &tmp));
        logical_phase = tmp;
    }

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, logical_phase, &address_granularity));

    /*
     * Set the number of outlifs per entry.
     */
    resource_info->nof_outlifs_per_entry = UTILEX_DIV_ROUND_UP(entry_size_in_bits, address_granularity);

    if (address_granularity == 30 && resource_info->nof_outlifs_per_entry == 3)
    {
        /*
         * Special case: a 90b entry in granularity of 30 would actually take up 120b.
         */
        resource_info->nof_outlifs_per_entry = 4;
    }

    /*
     * Set the use_tag_and_align.
     */
    if (address_granularity == 30 || address_granularity == 60)
    {
        /*
         * If the granularity is 30 or 60, then entries must be aligned to the number of indexes they occupy.
         * We want to use the tag to mark a line to serve either 30, 60 or 120 bit entries, so we don't get
         * fragmentation by allocating 30 and then 120, for example.
         */
        resource_info->use_tag_and_align = TRUE;
    }
    else
    {
        /*
         * If the granularity is 90 then the entry can start at any index.
         * If the granularity is 120, then every size of entry takes just 1 index and fragmentation is impossible.
         */
        resource_info->use_tag_and_align = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an outlif logical phase and linked list requirement, attempts to find
 * an available eedb bank to use to store data.
 * This function will first try to find an existing bank, and failing that, will attempt
 * to allocate a new one.
 */
static shr_error_e
dnx_algo_local_outlif_bank_in_eedb_bank_find(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int ll_required,
    dnx_algo_lif_local_outlif_resource_info_t * resource_info,
    uint32 *output_flags,
    uint32 *outlif_bank)
{
    sw_state_ll_node_t current_node;
    uint8 address_granularity, current_outlif_bank, new_eedb_bank_array_index, current_eedb_bank;
    int outlif_bank_match, outlif_bank_can_be_used, nof_eedb_banks_to_allocate, eedb_bank_is_available;
    int nof_outlifs_per_entry, nof_eedb_banks_per_outlif_bank;
    int current_eedb_bank_array_index, extendable_outlif_bank;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Calculate the number of eedb banks per outlif bank for this phase.
     * We'll use this value below.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, logical_phase, &address_granularity));

    nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
    nof_eedb_banks_per_outlif_bank =
        DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / (MDB_NOF_ROWS_IN_EEDB_BANK * nof_outlifs_per_entry);

    /*
     * First, check this phase's list of used eedb banks to see if a bank has already been allocated.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.get_first(unit, logical_phase, &current_node));
    extendable_outlif_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;
    new_eedb_bank_array_index = 0;
    while (DNX_SW_STATE_LL_IS_NODE_VALID(current_node))
    {
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.node_value(unit, current_node, &current_outlif_bank));

        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_check_bank
                        (unit, current_outlif_bank, logical_phase, ll_required, resource_info, &outlif_bank_match,
                         &outlif_bank_can_be_used));

        if (outlif_bank_can_be_used)
        {
            /*
             * A matching bank using EEDB bank can be used. Nothing else needs to be done but return it.
             */
            *outlif_bank = current_outlif_bank;
            SHR_EXIT();
        }

        /*
         * If the bank can't be used for allocation (probably because it's full) but it matches the criteria,
         * check if the outlif bank is using the maximum number of eedb banks it can use.
         * If not, then we'll allocate another eedb bank for it.
         * Stop checking after such a bank is found.
         */
        if (outlif_bank_match && extendable_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
        {
            for (current_eedb_bank_array_index = 0; current_eedb_bank_array_index < nof_eedb_banks_per_outlif_bank;
                 current_eedb_bank_array_index++)
            {
                SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                                eedb_data_banks.get(unit, current_outlif_bank, current_eedb_bank_array_index,
                                                    &current_eedb_bank));

                if (current_eedb_bank == (uint8) DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
                {
                    extendable_outlif_bank = current_outlif_bank;
                    new_eedb_bank_array_index = current_eedb_bank_array_index;
                    break;
                }
            }
        }

        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.next_node(unit, logical_phase, current_node, &current_node));
    }

    /*
     * If we got here, it means that we didn't find a bank that's ready to allocate.
     * In that case, we need to allocate a new eedb bank.
     * If ll is required and we don't extend an existing bank (which should already have an ll bank) then we need to allocate an
     * ll bank as well.
     */
    if (extendable_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK && ll_required)
    {
        nof_eedb_banks_to_allocate = 2;
    }
    else
    {
        nof_eedb_banks_to_allocate = 1;
    }

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_verify_ll_availability
                    (unit, logical_phase, nof_eedb_banks_to_allocate, &eedb_bank_is_available));

    if (!eedb_bank_is_available)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "All eedb banks available for outlif phase %d are in use.", logical_phase + 1);
    }

    /*
     * If we don't extend an existing outlif bank, then allocate one.
     * First allocate an available outlif bank, then assign it.
     */
    if (extendable_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
    {
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_prefix_for_eedb_bank_allocate
                        (unit, address_granularity, nof_eedb_banks_per_outlif_bank, outlif_bank));

        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_prefix_for_eedb_bank_assign
                        (unit, address_granularity, nof_eedb_banks_per_outlif_bank, logical_phase, *outlif_bank));

        /*
         * After the bank was allocated, assign it.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_use_set(unit, *outlif_bank, logical_phase, ll_required));
    }
    else
    {
        /*
         * If there's an outlif bank that can hold more outlif entries than it has,
         * then we extend it.
         */
        *outlif_bank = extendable_outlif_bank;
    }

    /*
     * Now allocate an eedb bank for this outlif bank and write it to the outlif's sw state.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_assign_eedb_bank_to_outlif_bank
                    (unit, *outlif_bank, logical_phase, new_eedb_bank_array_index,
                     DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / nof_eedb_banks_per_outlif_bank));

    /*
     * Mark that HW needs to be updated with the newly assigned eedb bank.
     * If the outlif bank has already been in use, then no need to update the pointers
     * or linked list bank as well.
     */
    *output_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK;

    if (extendable_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
    {
        *output_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS;
        if (ll_required)
        {
            *output_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given a local outlif allocated on an eedb bank, check if its eedb bank is empty and can be cleared.
 * If the entire outlif bank can be cleared, then clear it.
 */
static shr_error_e
dnx_algo_local_outlif_bank_in_eedb_bank_check_and_unassign(
    int unit,
    int local_outlif,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    uint8 address_granularity, logical_phase;
    int outlif_bank, bank_offset, eedb_bank_array_index;
    int nof_outlifs_per_entry, nof_eedb_banks_per_outlif_bank, nof_outlifs_per_eedb_bank;
    int nof_allocated_elements, nof_free_elements;

    SHR_FUNC_INIT_VARS(unit);

    outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(local_outlif);
    bank_offset = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_OFFSET(local_outlif);

    /*
     * Calculate the number of eedb banks per outlif bank for this phase.
     * We'll use this value below.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, outlif_bank, &logical_phase));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, logical_phase, &address_granularity));

    nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
    nof_eedb_banks_per_outlif_bank =
        DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / (MDB_NOF_ROWS_IN_EEDB_BANK * nof_outlifs_per_entry);

    nof_outlifs_per_eedb_bank = DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / nof_eedb_banks_per_outlif_bank;

    eedb_bank_array_index = bank_offset / nof_outlifs_per_eedb_bank;

    /*
     * Check the resource to see if the EEDB bank is empty and could be freed.
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_allocated_elements_in_range_get(unit, outlif_bank,
                                                                                     OUTLIF_BANK_FIRST_INDEX(unit,
                                                                                                             outlif_bank)
                                                                                     +
                                                                                     nof_outlifs_per_eedb_bank *
                                                                                     eedb_bank_array_index,
                                                                                     nof_outlifs_per_eedb_bank,
                                                                                     &nof_allocated_elements));

    if (nof_allocated_elements == 0)
    {
        outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK;
        /*
         * Now check if the entire bank can be freed.
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_free_elements_get(unit, outlif_bank, &nof_free_elements));

        if (nof_free_elements == DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))
        {
            uint8 ll_in_use;

            outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS;

            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, outlif_bank, &ll_in_use));

            if (ll_in_use)
            {
                outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK;
            }

        }

        /*
         * Get the hw info before clearing the bank.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_hw_info_get(unit, local_outlif, outlif_hw_info));

        if (nof_free_elements == DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))
        {
            /*
             * Remove the prefix.
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_prefix_for_eedb_bank_unassign
                            (unit, address_granularity, nof_eedb_banks_per_outlif_bank, logical_phase, outlif_bank));

            /*
             * Clear in use indication, and some more stuff.
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_use_unset(unit, outlif_bank));
        }

        /*
         * Set the eedb bank as not in use.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_unassign_eedb_bank_to_outlif_bank
                        (unit, outlif_bank, eedb_bank_array_index, nof_outlifs_per_eedb_bank));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_allocate(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    dnx_algo_lif_local_outlif_resource_info_t resource_info;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    uint8 ll_required = TRUE, in_use, disable_mdb_clusters;
    uint32 current_bank, first_unused_bank, first_bank_for_phase = 0, last_bank_for_phase = 0;
    int bank_match, bank_can_be_used;
    int element;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(outlif_hw_info, 0, sizeof(dnx_algo_local_outlif_hw_info_t));

    /*
     * Get the linked list requirement through dbal API.
     * For stress test, don't use linked list.
     */
    SHR_IF_ERR_EXIT(dbal_tables_eedb_link_list_indication_get(unit,
                                                              outlif_info->dbal_table_id, outlif_info->dbal_result_type,
                                                              &ll_required));
    /*
     * Get the data to be used by the resource manager.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, outlif_info, &resource_info));

    /*
     * Check whether to skip mdb cluster allocation and go straight to eedb bank allocation.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.disable_mdb_clusters.get(unit, &disable_mdb_clusters));

    /*
     * Get the entry's logical phase.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, outlif_info->outlif_phase, &logical_phase));

    /*
     * Iterate over the banks.
     * Try to find a bank that's in use, and allocate in it.
     * While passing over the banks, we also note if there's an unused bank we can use if we didn't find a used bank.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, logical_phase, &first_bank_for_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, logical_phase, &last_bank_for_phase));
    first_unused_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;

    if (!disable_mdb_clusters)
    {
        for (current_bank = first_bank_for_phase; current_bank <= last_bank_for_phase; current_bank++)
        {
            /*
             * Check if the bank is in use.
             */
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, current_bank, &in_use));

            if (in_use)
            {
                /*
                 * If bank is in use, make sure it's matching the phase and ll requirement, and
                 * run an allocation simulation.
                 */
                bank_can_be_used = FALSE;
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_check_bank
                                (unit, current_bank, logical_phase, ll_required, &resource_info, &bank_match,
                                 &bank_can_be_used));

                if (bank_can_be_used)
                {
                    /*
                     * Bank matches the criteria and the allocation simulation passed. Use it.
                     */
                    break;
                }
            }
            else if (first_unused_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
            {
                first_unused_bank = current_bank;
            }
        }
    }
    else
    {
        current_bank = last_bank_for_phase + 1;
    }

    /*
     * If the loop reached the last bank and didn't break, it means we failed to allocate in a used bank.
     * Try to allocate in an unused bank.
     */
    if (current_bank > last_bank_for_phase)
    {
        if (first_unused_bank != DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
        {
            /*
             * Before trying to allocate a bank, if ll is required then verify that we have an available ll bank.
             */
            if (ll_required)
            {
                int is_available = FALSE;
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_verify_ll_availability(unit, logical_phase, 1, &is_available));

                if (!is_available)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "All banks using linked list on logical phase %d are full.",
                                 logical_phase + 1);
                }
            }

            /*
             * Otherwise, assign the bank.
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_use_set(unit, first_unused_bank, logical_phase, ll_required));
            if (ll_required)
            {
                outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK;
            }
            outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS;
            current_bank = first_unused_bank;
        }
        else
        {
            /*
             * Try using an EEDB bank. If none is found, and none can be allocated, then an error will be returned.
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_eedb_bank_find
                            (unit, logical_phase, ll_required, &resource_info, &outlif_hw_info->flags, &current_bank));
        }
    }

    /*
     * If we reached this point, it means that we can allocate in current_bank.
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.allocate_single
                    (unit, current_bank, 0, (void *) &resource_info, &element));

    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
    {
        /*
         * If a new outlif bank was allocated, get the HW info required by the provided flags
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_hw_info_get(unit, element, outlif_hw_info));
    }

    outlif_info->local_outlif = element;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_free(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int outlif_bank, nof_free_elements;
    int deallocation_size;
    dnx_algo_lif_local_outlif_resource_info_t resource_info;
    uint8 assigned;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update the outlif info with its phase. This can be deduced from the outlif bank, but only in this function
     * and not in any of the calling functions.
     */
    outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(outlif_info->local_outlif);

    /*
     * Get the number of entries to free.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, outlif_info, &resource_info));

    deallocation_size = resource_info.nof_outlifs_per_entry;

    SHR_IF_ERR_EXIT(lif_mngr_db.
                    egress_local_lif.free_several(unit, outlif_bank, deallocation_size, outlif_info->local_outlif));

    /*
     * Check if the bank needs to be unassigned, according to its physical type - MDB macro, or EEDB bank.
     */
    sal_memset(outlif_hw_info, 0, sizeof(dnx_algo_local_outlif_hw_info_t));
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.get(unit, outlif_bank, &assigned));

    if (assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB)
    {
        /*
         * EEDB banks update hw if an eedb bank was cleared. It's usually smaller than an outlif bank, then it needs to be checked.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_eedb_bank_check_and_unassign
                        (unit, outlif_info->local_outlif, outlif_hw_info));
    }
    else
    {
        /*
         * MDB macros are cleared when the outilf bank is cleared.
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_free_elements_get(unit, outlif_bank, &nof_free_elements));

        if (nof_free_elements == DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))
        {
            uint8 ll_in_use;
            /*
             * Update the pointers to this bank, because it's compeltely removed.
             */
            outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS;

            /*
             * Check if this bank also had a linked list which should be removed.
             */
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, outlif_bank, &ll_in_use));
            if (ll_in_use)
            {
                outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK;
            }

            /*
             * Get the hw info before clearing the bank.
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_hw_info_get(unit, outlif_info->local_outlif, outlif_hw_info));

            /*
             * Clear the bank.
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_use_unset(unit, outlif_bank));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * OUTLIF ALLOCATION FUNCTIONS - END
 */

/*
 *
 * MODULE INIT DEINIT - START.
 * {
 */

/**
 * \brief
 * For each logical phase, get the first and last available linked list bank for this
 * phase, and save them to sw state.
 *
 * To be called once during device init.
 */
static shr_error_e
dnx_algo_local_outlif_ll_banks_init(
    int unit)
{
    int current_logical_phase;
    int nof_valid_clusters, current_cluster;
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS], *cluster;
    uint8 physical_phase;
    uint8 first_ll_bank, last_ll_bank;
    SHR_FUNC_INIT_VARS(unit);

    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {

        /*
         * Get the mdb clusters belonging to this phase.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        physical_phase.get(unit, current_logical_phase, &physical_phase));

        SHR_IF_ERR_EXIT(mdb_init_get_table_resources
                        (unit, EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(physical_phase), clusters, &nof_valid_clusters));

        /*
         * Initialize the first and last banks to the extreme values.
         */
        first_ll_bank = 0xff;
        last_ll_bank = 0;

        for (current_cluster = 0; current_cluster < nof_valid_clusters; current_cluster++)
        {
            cluster = &clusters[current_cluster];
            if (cluster->macro_type == MDB_EEDB_BANK)
            {
                /*
                 * If the cluster index (ll bank index) is greater than the current last, or smaller than
                 * the current first, update them.
                 */
                if (cluster->cluster_idx < first_ll_bank)
                {
                    first_ll_bank = cluster->cluster_idx;
                }

                if (cluster->cluster_idx > last_ll_bank)
                {
                    last_ll_bank = cluster->cluster_idx;
                }
            }
        }

        /*
         * Now, write the first and last banks to the sw state.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        first_ll_bank.set(unit, current_logical_phase, first_ll_bank));

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        last_ll_bank.set(unit, current_logical_phase, last_ll_bank));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given a the first physical phase in a phase pair, and a range of outlif banks represented by first and last,
 * saves the outlif range to the sw state of the logical phases mapped to this physical phase pair.
 *
 * To be called once per physical phase pair during device init.
 */
static shr_error_e
dnx_algo_local_outlif_phase_set_first_and_last_bank(
    int unit,
    int first_physical_phase,
    int first_bank,
    int last_bank)
{
    uint8 first_logical_phase, second_logical_phase;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(local_outlif_info.
                    physical_phase_to_logical_phase_map.get(unit, first_physical_phase, &first_logical_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.
                    physical_phase_to_logical_phase_map.get(unit,
                                                            EEDB_PHYSICAL_PHASE_TO_PARTNER_PHASE(first_physical_phase),
                                                            &second_logical_phase));

    /*
     * Set the first bank for the current phase.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.set(unit, first_logical_phase, first_bank));

    /*
     * Set the pair phase with the same value.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.set(unit, second_logical_phase, first_bank));

    /*
     * Set the current phase pair last bank.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.set(unit, first_logical_phase, last_bank));

    /*
     * Set the pair phase with the same value.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.set(unit, second_logical_phase, last_bank));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Creates the local outlif resource.
 * This function must come after initializing the outlif banks for MDB clusters.
 */
static shr_error_e
dnx_algo_local_outlif_resource_init(
    int unit)
{
    sw_state_algo_res_create_data_t res_data;
    simple_bitmap_wrap_tag_info_t tag_set_info;
    int *bank_address_granularities, current_granularity;
    uint32 first_bank, last_bank, current_bank;
    uint8 phase_address_granularity;
    uint32 current_outlif_bank;
    dnx_algo_local_outlif_logical_phase_e logical_phase;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Prepare an array of address granularity per bank to initialize the sub resource for this bank.
     *
     * Also calculate default granularity - the lowest granularity currently configured in the system.
     * The rest of the array will hold the granularity of the phase pair the bank is assigned to.
     *
     * The default granularity is used for EEDB data banks, that can be assigned to any phase, so we can't know
     * their phase in advance.
     */
    bank_address_granularities = NULL;
    SHR_ALLOC_SET_ZERO(bank_address_granularities, sizeof(int) * (DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit)),
                       "bank_address_granularities", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; logical_phase++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, logical_phase, &first_bank));
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, logical_phase, &last_bank));

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        address_granularity.get(unit, logical_phase, &phase_address_granularity));

        for (current_bank = first_bank; current_bank <= last_bank; current_bank++)
        {
            bank_address_granularities[current_bank] = phase_address_granularity;
        }
    }

    /*
     * Create the resources for the out lif table.
     * The resource pool indexes indicate the outlif banks.
     */
    sal_memset(&res_data, 0, sizeof(res_data));

    res_data.flags = SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM | SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    res_data.nof_elements = DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);
    res_data.nof_resource_pool_indexes = DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit);
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_OUTLIF;
    sal_strncpy(res_data.name, "lif_local_outlif 2", SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * Prepare the illegal tag info for the unassigned banks.
     */
    sal_memset(&tag_set_info, 0, sizeof(simple_bitmap_wrap_tag_info_t));
    tag_set_info.force_tag = TRUE;
    tag_set_info.tag = OUTLIF_RESOURCE_ILLEGAL_TAG;
    tag_set_info.nof_elements = res_data.nof_elements;

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.alloc(unit, res_data.nof_resource_pool_indexes));
    for (current_outlif_bank = 0; current_outlif_bank < res_data.nof_resource_pool_indexes; current_outlif_bank++)
    {
        if (bank_address_granularities[current_outlif_bank] == 0)
        {
            /*
             * If the outlif bank wasn't assigned a physical MDB cluster,
             * then it means that it's reserved for an eedb bank.
             * We'll use the minimum granularity for it. This allow us to fit every existing granularity,
             * and use an illegal tag that can't be used by any legal entry (0b11).
             */
            current_granularity = EEDB_MIN_GRANLARITY;
            res_data.flags |= DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_FORCING_TAG;
        }
        else
        {
            current_granularity = bank_address_granularities[current_outlif_bank];
        }
        res_data.first_element = OUTLIF_BANK_FIRST_INDEX(unit, current_outlif_bank);
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.create
                        (unit, current_outlif_bank, &res_data, (void *) &current_granularity));

        if (bank_address_granularities[current_outlif_bank] == 0)
        {
            res_data.flags &= ~DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_FORCING_TAG;
            /*
             * If the outlif bank wasn't assigned a physical MDB cluster, then we'll set all of it with an
             * illegal tag to make sure it's not allocated until an eedb bank is assigned to it.
             */
            tag_set_info.element = res_data.first_element;
            SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.advanced_algorithm_info_set(unit,
                                                                                     current_outlif_bank,
                                                                                     &tag_set_info));
        }
    }

exit:
    SHR_FREE(bank_address_granularities);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * Given an outlif bank to start from, and the first physical phase of the phase pair we want to work with,
 * counts the number of physical clusters mapped to this phase pair, and assigns each of them outlif banks
 * according to the phase's granularity.
 *
 * Returns the next outlif bank that can be assigned, and the cluster info to be written back to MDB.
 */
static shr_error_e
dnx_algo_local_outlif_perform_bank_assignment(
    int unit,
    int starting_outlif_bank,
    dbal_physical_tables_e physical_table,
    int *first_outlif_bank,
    int *last_outlif_bank,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters)
{
    int current_cluster, current_bank_in_cluster, nof_banks_per_cluster, current_bank;
    uint32 nof_entries_in_cluster;
    mdb_cluster_alloc_info_t *current_cluster_info, *current_init_cluster;
    mdb_macro_types_e current_macro_type, first_macro_type, last_macro_type;
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS];
    int nof_valid_clusters;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the mdb clusters belonging to this phase.
     */
    SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, physical_table, clusters, &nof_valid_clusters));

    /*
     * We want to traverse the clusters from largest to smallest macro type. We assume that the macro types are
     * consecutive, with the largest first.
     */
    first_macro_type = MDB_MACRO_A;
    last_macro_type = MDB_MACRO_B;
    current_bank = starting_outlif_bank;
    *first_outlif_bank = DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit);

    for (current_macro_type = first_macro_type; current_macro_type <= last_macro_type; current_macro_type++)
    {
        /*
         * Calculate the number of outlif banks each cluster holds.
         * We asssume that each cluster holds at least one bank.
         */
        SHR_IF_ERR_EXIT(mdb_eedb_table_nof_entries_per_cluster_type_get
                        (unit, physical_table, current_macro_type, &nof_entries_in_cluster));

        nof_banks_per_cluster = UTILEX_DIV_ROUND_UP(nof_entries_in_cluster, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));

        /*
         * The index of the first (current) bank must be aligned up to the number of outlif banks in the cluster,
         * because the cluster start must be aligned to the lif id it holds.
         */
        current_bank = UTILEX_ALIGN_UP(current_bank, nof_banks_per_cluster);

        for (current_cluster = 0; current_cluster < nof_valid_clusters; current_cluster++)
        {
            current_cluster_info = &clusters[current_cluster];
            if (current_cluster_info->dbal_physical_table_id == physical_table
                && current_cluster_info->macro_type == current_macro_type)
            {
                if (current_bank == DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit))
                {
                    /*
                     * It's possible that there are not enough outlif banks in the system.
                     * Possible solutions:
                     * 1. In this function, aligning the current bank to the start of the cluster may burn up to 7
                     *      banks. This can be improved by improving this function to use these banks.
                     * 2. If all outlif banks are in use, then the granualirty of one of the phases can be increased
                     *      to reduce the number of required banks.
                     * 3. Finally, if all else fails, then the outlif pointer size can be increased from 19 to 20 bits.
                     *      Note that if that happens, then a whole rethinking of the ll use must be made, because
                     *      the pointer for the ll is always 19 bits.
                     */
                    SHR_ERR_EXIT(_SHR_E_FULL,
                                 "The input to the outlif bank allocation algorithm resulted in assigning more banks than the system supports."
                                 "See comment in code for possible solutions.");
                }

                if (current_bank < *first_outlif_bank)
                {
                    /*
                     * Update the first outlif bank if it comes before the current one.
                     */
                    *first_outlif_bank = current_bank;
                }

                /*
                 * Copy the cluster information to the init clusters. It will later be written back to
                 * the MDB.
                 */
                current_init_cluster = &init_clusters_info[*nof_init_clusters];

                sal_memcpy(current_init_cluster, current_cluster_info, sizeof(mdb_cluster_alloc_info_t));

                current_init_cluster->logical_start_address = current_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);

                for (current_bank_in_cluster = 0; current_bank_in_cluster < nof_banks_per_cluster;
                     current_bank_in_cluster++, current_bank++)
                {
                    /*
                     * Set each bank as assigned, and that it's using MDB clusters.
                     */
                    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.set(unit, current_bank,
                                                                                    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB));

                    current_init_cluster->phase_alloc_logical_entries[current_bank_in_cluster] =
                        EEDB_PHYSICAL_TABLE_TO_PHYSICAL_PHASE(physical_table);
                }

                (*nof_init_clusters)++;
            }
        }
    }

    if (*first_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit))
    {
        /*
         * This phase doesn't have any MDB clusters assigned to it. Set its first bank to
         * be greater than its last bank, so in practice it will have no banks.
         */
        *first_outlif_bank = 1;
        *last_outlif_bank = 0;
    }
    else
    {
        *last_outlif_bank = current_bank - 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * Given an outlif bank to start from, and the phase pair we want to work with, assigns outlif banks to the
 * clusters mapped to this phase pair, and maps the reverse (phase to banks) in the sw state.
 *
 * Returns the next outlif bank that can be assigned, and the cluster info to be written back to MDB.
 */
static shr_error_e
dnx_algo_local_outlif_bank_assign_range_to_eedb_phy_table(
    int unit,
    int phase_pair,
    int current_outlif_bank,
    int *next_outlif_bank,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters)
{
    int first_physical_phase;
    dbal_physical_tables_e first_physical_table_in_pair;
    int first_outlif_bank = 0, last_outlif_bank = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Setup parameters to be saved for the banks.
     */
    first_physical_phase = EEDB_PHYSICAL_TABLE_PHASE_PAIR_TO_FIRST_PHYSICAL_PHASE(phase_pair);
    first_physical_table_in_pair = EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(first_physical_phase);

    /*
     * First, get the clusters on that are unique to the current logical phase (indicated by
     * logical table being identical to the one we provided).
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_perform_bank_assignment
                    (unit, current_outlif_bank, first_physical_table_in_pair, &first_outlif_bank, &last_outlif_bank,
                     init_clusters_info, nof_init_clusters));

    /*
     * Now map the reverse, logical phase to banks.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_set_first_and_last_bank
                    (unit, first_physical_phase, first_outlif_bank, last_outlif_bank));

    *next_outlif_bank = last_outlif_bank + 1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Maps all physical clusters to outlif banks.
 *
 * Returns the next outlif bank that can be assigned, and the cluster info to be written back to MDB.
 *
 */
shr_error_e
dnx_algo_local_outlif_mdb_bank_database_init(
    int unit,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters)
{
    uint32 eedb_pair_granularities[NOF_EEDB_PHASE_PAIRS];
    int current_eedb_data_bank, current_granularity;
    dnx_algo_local_outlif_logical_phase_e rif_logical_phase;
    uint8 rif_physical_phase;
    int current_physical_phase_pair, rif_phase_pair;
    int nof_rifs, nof_rif_banks;

    SHR_FUNC_INIT_VARS(unit);

    for (current_physical_phase_pair = 0;
         current_physical_phase_pair < NOF_EEDB_PHASE_PAIRS; current_physical_phase_pair++)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_data_granularity_get
                        (unit,
                         EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(EEDB_PHYSICAL_TABLE_PHASE_PAIR_TO_FIRST_PHYSICAL_PHASE
                                                               (current_physical_phase_pair)),
                         &eedb_pair_granularities[current_physical_phase_pair]));
    }

    /*
     * Start by assigning the physical phase containing the rifs to the first IDs.
     * because the rifs always hold the outlifs 0-nof_rifs.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, LIF_MNGR_OUTLIF_PHASE_RIF, &rif_logical_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    physical_phase.get(unit, rif_logical_phase, &rif_physical_phase));
    rif_phase_pair = EEDB_PHYSICAL_TABLE_PHYSICAL_PHASE_TO_PHASE_PAIR(rif_physical_phase);
    current_eedb_data_bank = 0;
    *nof_init_clusters = 0;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_assign_range_to_eedb_phy_table
                    (unit, rif_phase_pair, current_eedb_data_bank, &current_eedb_data_bank,
                     init_clusters_info, nof_init_clusters));

    /*
     * If not enough outlif banks were assigned to the rif phase pair, it means that it didn't get any MDB clusters.
     * We still need to reserve the low outlif banks for the rifs, which will be using eedb banks later.
     */

    nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);
    nof_rif_banks = UTILEX_DIV_ROUND_UP(nof_rifs, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));

    if (current_eedb_data_bank < nof_rif_banks)
    {
        current_eedb_data_bank = nof_rif_banks;
    }

    /*
     * Now go from small granularity to large granularity, and assign the rest of the stages.
     * Skip the rif pair obviously.
     */
    for (current_granularity = EEDB_GRANULARITY_UNIT; current_granularity <= EEDB_MAX_GRANULARITY;
         current_granularity += EEDB_GRANULARITY_UNIT)
    {
        for (current_physical_phase_pair = 0;
             current_physical_phase_pair < NOF_EEDB_PHASE_PAIRS; current_physical_phase_pair++)
        {
            if ((current_physical_phase_pair != rif_phase_pair) &&
                (eedb_pair_granularities[current_physical_phase_pair] == current_granularity))

            {
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_assign_range_to_eedb_phy_table
                                (unit, current_physical_phase_pair, current_eedb_data_bank, &current_eedb_data_bank,
                                 init_clusters_info, nof_init_clusters));
            }
        }
    }

    /*
     * Now that the outlif banks in MDB clusters are allocated, we can initialize the outlif resource.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_resource_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This procedure read the XML which holds the definition of which result type is valid in each logical EEDB phase.
 * The information is saved in SW state.
 * Note: if result type in SW state is (-1), it means that result type should not be considered in mapping
 */
static shr_error_e
dnx_algo_local_outlif_phases_dbal_info_init(
    int unit)
{
    char etps_to_phases_xml_path[RHFILE_MAX_SIZE];
    void *curTop, *curPhase;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(etps_to_phases_xml_path, "mdb/auto_generated/etps_in_eedb_phases.xml", RHFILE_MAX_SIZE);

    /*
     * Read XMl
     */
    curTop = dbx_file_get_xml_top(unit, etps_to_phases_xml_path, "PhasesEedbCatalog", CONF_OPEN_PER_DEVICE);
    if (curTop == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find etps to phase XML file: %s\n", etps_to_phases_xml_path);
    }

    /*
     * Run over all phases
     */
    RHDATA_ITERATOR(curPhase, curTop, "EedbPhase")
    {
        int logical_phase = 0;
        int nof_mappings = 0, mapping_counter = 0;
        char phase_name[10];
        void *curElement;

        RHDATA_GET_XSTR_STOP(curPhase, "Name", phase_name, 10);
        RHDATA_GET_INT_STOP(curPhase, "NumOfMappings", nof_mappings);

        /*
         * Assuming phase names are EEDB_A -> EEDB_H,
         * the mapping to logical phase is done by taking the 6th char and reduce 'A'
         */
        logical_phase = phase_name[5] - 'A';

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        dbal_valid_combinations.alloc(unit, logical_phase, nof_mappings));
        RHDATA_ITERATOR(curElement, curPhase, "Element")
        {
            /*
             * Add all elements in XML to SW state.
             * Translate strings to DBAL IDs
             */
            char dbal_table_name[DBAL_MAX_STRING_LENGTH], dbal_result_type_name[DBAL_MAX_STRING_LENGTH];
            dbal_tables_e dbal_table_id;
            CONST dbal_logical_table_t *dbal_table;
            int result_type;

            RHDATA_GET_XSTR_STOP(curElement, "AppDb", dbal_table_name, DBAL_MAX_STRING_LENGTH);
            RHDATA_GET_XSTR_STOP(curElement, "ResultType", dbal_result_type_name, DBAL_MAX_STRING_LENGTH);

            SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, dbal_table_name, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &dbal_table));
            if (dbal_table->maturity_level == DBAL_MATURITY_LOW)
            {
                continue;
            }

            if (sal_strncmp(dbal_result_type_name, "", DBAL_MAX_STRING_LENGTH) == 0)
            {
                result_type = -1;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_tables_result_type_by_name_get
                                (unit, dbal_table_id, dbal_result_type_name, &result_type));
            }
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.dbal_valid_combinations.
                            dbal_table.set(unit, logical_phase, mapping_counter, dbal_table_id));
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.dbal_valid_combinations.
                            dbal_result_type.set(unit, logical_phase, mapping_counter, result_type));
            mapping_counter++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initializes the sw state of physical to logical phase mapping, as well as extra per stage data.
 */
static shr_error_e
dnx_algo_local_outlif_phase_data_init(
    int unit)
{
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    uint32 address_granularity;
    int physical_phase;
    int mapped_physical_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Mapping is taken from soc property.
     */
    for (logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; logical_phase++)
    {
        physical_phase =
            (dnx_data_lif.out_lif.logical_to_physical_phase_map_get(unit, logical_phase + 1))->physical_phase;

        if (physical_phase == -1)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Soc property outlif_logical_to_physical_phase_map must have a valid value "
                         "for phases 1-%d. Phase %d didn't have any.", DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT,
                         logical_phase + 1);
        }

        if (mapped_physical_phase[physical_phase])
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Two logical phases were set with the same physical phase %d.", physical_phase);
        }

        mapped_physical_phase[physical_phase] = TRUE;

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.physical_phase.set(unit, logical_phase, physical_phase));

        SHR_IF_ERR_EXIT(local_outlif_info.physical_phase_to_logical_phase_map.set(unit, physical_phase, logical_phase));

        /*
         * Get the phase address granularity for this logical phase, and save it to sw state.
         */
        SHR_IF_ERR_EXIT(mdb_eedb_table_data_granularity_get
                        (unit, EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(physical_phase), &address_granularity));

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        address_granularity.set(unit, logical_phase, address_granularity));
    }

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phases_dbal_info_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * Setup the out lif sw database:
 * - Logical to physical phase mapping.
 * - Initialize the linked list bank assignment.
 */
static shr_error_e
dnx_algo_local_outlif_databases_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Fill the outlif phase and ll banks databases.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_data_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_ll_banks_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * Allocate all databases in the outlif sw state.
 */
static shr_error_e
dnx_algo_local_outlif_databases_allocate(
    int unit)
{
    sw_state_ll_init_info_t used_data_banks_list_init_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First, initialize the sw state.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.init(unit));

    /*
     * Allocate the maximum number of outlif banks, since we don't know how many we'll need.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.alloc(unit, DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit)));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.alloc(unit, DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT));

    SHR_IF_ERR_EXIT(local_outlif_info.
                    physical_phase_to_logical_phase_map.alloc(unit, DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES));

    /*
     * Allocate the eedb banks databases.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    bank_occupation_bitmap.alloc_bitmap(unit, dnx_data_mdb.eedb.nof_eedb_banks_get(unit)));

    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    outlif_prefix_occupation_bitmap.alloc_bitmap(unit, 1 << dnx_data_mdb.eedb.bank_id_bits_get(unit)));

    sal_memset(&used_data_banks_list_init_info, 0, sizeof(sw_state_ll_init_info_t));
    used_data_banks_list_init_info.max_nof_elements = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
    used_data_banks_list_init_info.expected_nof_elements = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
    used_data_banks_list_init_info.nof_heads = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT;

    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.create_empty(unit, &used_data_banks_list_init_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * To be called during device init.
 * Alocate all the rifs in the resource manager, mark their banks as in use and remove them from their partner
 * phase's availble banks.
 */
shr_error_e
dnx_algo_local_outlif_rif_init(
    int unit)
{
    int current_rif_bank, nof_rif_banks;
    dnx_algo_local_outlif_logical_phase_e rif_logical_phase;
    int nof_rifs;
    uint8 rif_physical_phase, partner_logical_phase;
    uint32 first_rif_bank, last_rif_bank, nof_rif_banks_in_mdb_cluster;

    uint8 address_granularity = 0;
    int nof_outlifs_per_entry = 0, nof_eedb_banks_per_outlif_bank = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Assign the rif banks to rif.
     * In addition, since we don't use rif allocation to allocate rifs, we can fill these res_indexes
     * completely, to avoid getting them allocated for another feature.
     */
    nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);
    nof_rif_banks = UTILEX_DIV_ROUND_UP(nof_rifs, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));

    if (nof_rif_banks > 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                        (unit, LIF_MNGR_OUTLIF_PHASE_RIF, &rif_logical_phase));

        /*
         * First, check if the rif phase has enough MDB clusters for all its rifs.
         * If not, then assign it eedb banks.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, rif_logical_phase, &last_rif_bank));
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, rif_logical_phase, &first_rif_bank));

        nof_rif_banks_in_mdb_cluster = (last_rif_bank >= first_rif_bank) ? last_rif_bank - first_rif_bank + 1 : 0;

        if (nof_rif_banks_in_mdb_cluster < nof_rif_banks)
        {
            int nof_required_eedb_banks, is_available;
            /*
             * If not MDB cluster assigned, and we need to use eedb banks, then assign the outlif bank to use eedb banks.
             */
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            address_granularity.get(unit, rif_logical_phase, &address_granularity));

            nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
            nof_eedb_banks_per_outlif_bank =
                DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / (MDB_NOF_ROWS_IN_EEDB_BANK * nof_outlifs_per_entry);

            /*
             * Verify that we have enough EEDB banks available to store all the rifs.
             */
            nof_required_eedb_banks = (nof_rif_banks - nof_rif_banks_in_mdb_cluster) * nof_eedb_banks_per_outlif_bank;

            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_verify_ll_availability
                            (unit, rif_logical_phase, nof_required_eedb_banks, &is_available));

            if (!is_available)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "There are not enough MDB clusters or eedb banks available for the number of "
                             "rifs declared in soc property rif_id_max.\n" "rif_id_max=0x%08x rifs.", nof_rifs);
            }

            for (current_rif_bank = nof_rif_banks_in_mdb_cluster; current_rif_bank < nof_rif_banks; current_rif_bank++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_prefix_for_eedb_bank_assign
                                (unit, address_granularity, nof_eedb_banks_per_outlif_bank, rif_logical_phase,
                                 current_rif_bank));
            }
        }

        for (current_rif_bank = 0; current_rif_bank < nof_rif_banks; current_rif_bank++)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_use_set(unit, current_rif_bank, rif_logical_phase, FALSE));

            if (current_rif_bank >= nof_rif_banks_in_mdb_cluster)
            {
                /*
                 * If we need to use eedb banks, then allocate them and assign them to this outlif bank.
                 */
                int current_eedb_bank_index;

                for (current_eedb_bank_index = 0; current_eedb_bank_index < nof_eedb_banks_per_outlif_bank;
                     current_eedb_bank_index++)
                {
                    /*
                     * Now allocate an eedb bank for this outlif bank and write it to the outlif's sw state.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_assign_eedb_bank_to_outlif_bank
                                    (unit, current_rif_bank, rif_logical_phase, current_eedb_bank_index,
                                     DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / nof_eedb_banks_per_outlif_bank));
                }
            }
        }

        /*
         * If the rifs are using mdb clusters, get the rif's partner phase, and update its first bank to not include the rif banks.
         */
        if (nof_rif_banks_in_mdb_cluster > 0)
        {
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            physical_phase.get(unit, rif_logical_phase, &rif_physical_phase));

            SHR_IF_ERR_EXIT(local_outlif_info.physical_phase_to_logical_phase_map.get(unit,
                                                                                      EEDB_PHYSICAL_PHASE_TO_PARTNER_PHASE
                                                                                      (rif_physical_phase),
                                                                                      &partner_logical_phase));

            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            first_bank.set(unit, partner_logical_phase, nof_rif_banks));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_allocation_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * - Allocate sw state databases.
     * - Initialize them.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_databases_allocate(unit));

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_databases_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_allocation_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * MODULE INIT DEINIT - END.
 *
 */

/*
 * EXTERNAL APIS INTERFACE - START
 * {
 *
 */

shr_error_e
dnx_algo_local_outlif_logical_phase_to_physical_phase(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint8 *physical_phase)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.physical_phase.get(unit, logical_phase, physical_phase));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_to_eedb_logical_phase(
    int unit,
    int local_outlif,
    dbal_tables_e dbal_table_id,
    lif_mngr_outlif_phase_e * outlif_phase)
{
    int outlif_bank, phase_offset, max_phase_offset;
    uint8 logical_phase;
    lif_mngr_outlif_phase_e basic_outlif_phase;

    SHR_FUNC_INIT_VARS(unit);

    outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(local_outlif);

    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, outlif_bank, &logical_phase));

    switch (logical_phase)
    {
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1:
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2:
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3:
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4:
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5:
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6:
            switch (dbal_table_id)
            {
                    /*
                     * Several eedb phases use these logical phases. Use the dbal table to figure out which one.
                     */
                case DBAL_TABLE_EEDB_DATA_ENTRY:
                case DBAL_TABLE_EEDB_IPFIX_PSAMP:
                case DBAL_TABLE_EEDB_IPV4_TUNNEL:
                case DBAL_TABLE_EEDB_IPV6_TUNNEL:
                case DBAL_TABLE_EEDB_ERSPAN:
                case DBAL_TABLE_EEDB_RSPAN:
                    basic_outlif_phase = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1;
                    max_phase_offset = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4 - LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1;
                    break;
                case DBAL_TABLE_EEDB_PWE:
                case DBAL_TABLE_EEDB_EVPN:
                    basic_outlif_phase = LIF_MNGR_OUTLIF_PHASE_VPLS_1;
                    max_phase_offset = LIF_MNGR_OUTLIF_PHASE_VPLS_2 - LIF_MNGR_OUTLIF_PHASE_VPLS_1;
                    break;
                case DBAL_TABLE_EEDB_MPLS_TUNNEL:
                    basic_outlif_phase = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1;
                    max_phase_offset = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_4 - LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1;
                    break;
                case DBAL_TABLE_EEDB_OUT_AC:
                    basic_outlif_phase = LIF_MNGR_OUTLIF_PHASE_NATIVE_AC;
                    max_phase_offset = LIF_MNGR_OUTLIF_PHASE_NATIVE_AC - LIF_MNGR_OUTLIF_PHASE_NATIVE_AC;
                    break;
                case DBAL_TABLE_EEDB_SFLOW:
                    basic_outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW;
                    max_phase_offset = LIF_MNGR_OUTLIF_PHASE_SFLOW - LIF_MNGR_OUTLIF_PHASE_SFLOW;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong - unkown dbal table id %d.", dbal_table_id);
            }
            phase_offset = logical_phase - DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3;
            if (phase_offset > max_phase_offset)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Something went wrong - no eedb phase enum for logical_phase %d and dbal table id %d.",
                             logical_phase + 1, dbal_table_id);
            }
            *outlif_phase = basic_outlif_phase + phase_offset;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7:
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_ARP;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8:
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_AC;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong - illegal logical phase for outlif %d.", local_outlif);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *nof_outlifs_per_entry)
{
    dnx_algo_lif_local_outlif_resource_info_t resource_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, outlif_info, &resource_info));

    *nof_outlifs_per_entry = resource_info.nof_outlifs_per_entry;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_get_potential_nof_mdb_outlifs_for_entry(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *nof_potential_outlifs)
{
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    uint32 first_bank_for_phase, last_bank_for_phase, current_bank;
    uint8 in_use, used_logical_phase;
    int nof_free_elements;
    int nof_outlifs_per_entry;
    int nof_free_outlifs;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the entry's logical phase and first and last bank.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, outlif_info->outlif_phase, &logical_phase));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, logical_phase, &first_bank_for_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, logical_phase, &last_bank_for_phase));

    nof_free_outlifs = 0;
    for (current_bank = first_bank_for_phase; current_bank < last_bank_for_phase; current_bank++)
    {
        /*
         * If the bank is in use by the logical phase, then add its free elements to the potential outlifs.
         * If the bank is not in use, then add all of it to the potential.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, current_bank, &in_use));

        if (in_use)
        {
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                            used_logical_phase.get(unit, current_bank, &used_logical_phase));

            if (used_logical_phase == logical_phase)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.
                                egress_local_lif.nof_free_elements_get(unit, current_bank, &nof_free_elements));

                nof_free_outlifs += nof_free_elements;
            }
        }
        else
        {
            nof_free_outlifs += DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);
        }
    }

    /*
     * Now return the number of free outlifs, divided by the outlif entry size.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, outlif_info, &nof_outlifs_per_entry));

    *nof_potential_outlifs = nof_free_outlifs / nof_outlifs_per_entry;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_stats_get(
    int unit,
    dnx_algo_local_outlif_stats_t * outlif_stats)
{
    int current_bank, count;
    uint8 assigned, in_use;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(outlif_stats, 0, sizeof(dnx_algo_local_outlif_stats_t));

    /*
     * First, iterate over all banks, check which ones are assigned and which are in use, and it it
     * to the stats struct.
     */
    for (current_bank = 0; current_bank < DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit); current_bank++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.get(unit, current_bank, &assigned));

        if (assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB)
        {
            outlif_stats->nof_outlif_banks_in_external_memory++;
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, current_bank, &in_use));

            if (in_use)
            {
                outlif_stats->nof_used_outlif_banks_in_external_memory++;
            }
        }
    }

    /*
     * Now, get the number of used eedb banks.
     */
    outlif_stats->nof_eedb_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    bank_occupation_bitmap.bit_range_count(unit, 0, dnx_data_mdb.eedb.nof_eedb_banks_get(unit),
                                                           &count));
    outlif_stats->nof_used_eedb_banks = count;

    /*
     * Now, get the GLEM stats.
     */
    outlif_stats->nof_glem_entries = dnx_data_lif.global_lif.nof_glem_lines_get(unit);

    SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.egress_global_lif_counter.get(unit, &count));

    outlif_stats->nof_used_glem_entries = count;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_next_bank_get(
    int unit,
    uint32 *next_bank)
{
    uint8 in_use;
    uint32 current_bank;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over the outlif banks until an in use bank was found.
     */
    current_bank = *next_bank;
    while (current_bank < DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit))
    {
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, current_bank, &in_use));

        if (in_use)
        {
            *next_bank = current_bank;
            SHR_EXIT();
        }
        current_bank++;
    };

    /*
     * If we got here, it means no bank was found. Return DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK.
     */
    *next_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_outrif_physical_bank_hw_info_get(
    int unit,
    int *outrif_bank_starts,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_infos,
    int *nof_outrifs_info)
{
    int current_outrif_physical_bank_start, current_outrif_info, current_outlif_bank;
    uint8 assigned, is_eedb_bank;
    uint8 logical_phase;
    SHR_FUNC_INIT_VARS(unit);

    current_outrif_physical_bank_start = 0;
    current_outrif_info = 0;

    while (current_outrif_physical_bank_start < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        sal_memset(&outlif_hw_infos[current_outrif_info], 0, sizeof(dnx_algo_local_outlif_hw_info_t));

        outrif_bank_starts[current_outrif_info] = current_outrif_physical_bank_start;

        /*
         * If this is the first physical database to use this outlif bank, then update pointers.
         */
        if (current_outrif_physical_bank_start % DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) == 0)
        {
            outlif_hw_infos[current_outrif_info].flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS;

        }

        current_outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(current_outrif_physical_bank_start);

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                        used_logical_phase.get(unit, current_outlif_bank, &logical_phase));

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.get(unit, current_outlif_bank, &assigned));

        is_eedb_bank = assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB;

        /*
         * If this outlif bank is using eedb banks, then update data bank in HW as well.
         */
        if (is_eedb_bank)
        {
            outlif_hw_infos[current_outrif_info].flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK;
        }

        /*
         * Get the hw info.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_hw_info_get(unit,
                                                               current_outrif_physical_bank_start,
                                                               &outlif_hw_infos[current_outrif_info]));

        if (is_eedb_bank)
        {
            /*
             * If this bank is stored in an EEDB bank, then read the EEDB bank data.
             */
            int nof_outlifs_per_eedb_bank;
            uint8 address_granularity;

            /*
             * Calculate the number of eedb banks per outlif bank for this phase.
             * We'll use this value below.
             */
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            address_granularity.get(unit, logical_phase, &address_granularity));

            nof_outlifs_per_eedb_bank = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity * MDB_NOF_ROWS_IN_EEDB_BANK;

            current_outrif_physical_bank_start += nof_outlifs_per_eedb_bank;
        }
        else
        {
            current_outrif_physical_bank_start += DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);
        }

        current_outrif_info++;
    }

    *nof_outrifs_info = current_outrif_info;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_mdb_clusters_disable_set(
    int unit,
    uint8 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(local_outlif_info.disable_mdb_clusters.set(unit, value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_rif_init_info_get(
    int unit,
    int *init_info)
{
    dnx_algo_local_outlif_logical_phase_e rif_logical_phase;
    uint8 phase_address_granularity;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, LIF_MNGR_OUTLIF_PHASE_RIF, &rif_logical_phase));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, rif_logical_phase, &phase_address_granularity));

    *init_info = phase_address_granularity;

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
