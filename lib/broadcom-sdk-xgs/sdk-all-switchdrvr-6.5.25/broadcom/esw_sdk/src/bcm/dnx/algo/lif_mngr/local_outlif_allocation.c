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
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*************
 * INCLUDES  *
 *************/
/*
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include "lif_mngr_internal.h"
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <sal/appl/sal.h>
#include "src/bcm/dnx/algo/lif_mngr/auto_generated/dbx_pre_compiled_lif_mdb_auto_generated_xml_parser.h"
#include <appl/diag/bslenable.h>

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
 * \brief Maximum possible size for data on the link list field.
 */
#define EEDB_LL_DATA_MAX_SIZE           10

/*
 * \brief Internal, to initialize the local outlif before allocation.
 */
#define INVALID_LOCAL_OUTLIF           -1

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
 * \brief This struct processes
 */
typedef struct
{
    uint32 nof_outlifs_per_entry;
    uint32 nof_outlifs_per_alternative_entry;
    uint32 use_tag_and_align;
} dnx_algo_lif_local_outlif_resource_info_t;

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

extern shr_error_e dbal_tables_mdb_hw_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size);
extern shr_error_e dbal_tables_eedb_link_list_indication_get(
    int unit,
    uint32 local_outlif,
    uint32 *has_ll);
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

/*
 * Given a dbal table + result type and address granularity, returns the number of outlif IDs this entry
 * takes on this granularity.
 */
static shr_error_e
dnx_algo_local_outlif_result_type_and_granularity_to_nof_outlifs(
    int unit,
    int address_granularity,
    dbal_tables_e dbal_table_id,
    int dbal_result_type,
    uint32 *nof_outlifs_per_entry)
{
    int entry_size_in_bits;
    int extra_data_exists_rv = 0;
    uint8 rt_has_ll;
    uint32 actual_extradata_bits = 0;
    dbal_table_field_info_t extra_data_field_info, ll_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the required size of the entry (in bits).
     */
    SHR_IF_ERR_EXIT(dbal_tables_mdb_hw_payload_size_get(unit, dbal_table_id, dbal_result_type, &entry_size_in_bits));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit, dbal_table_id, dbal_result_type, &rt_has_ll));

    /**In case ll is required*/
    if (rt_has_ll)
    {
        /*
         * Get ll field size 
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_no_err(unit, dbal_table_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, 0,
                                                          dbal_result_type, 0, &ll_field_info));

        /*
         * Entry size includes the size of 20b of link list - it can be ignored.
         */
        entry_size_in_bits -= ll_field_info.field_nof_bits;

        /** Check if extra data field exists on result type */
        extra_data_exists_rv = dbal_tables_field_info_get_no_err(unit, dbal_table_id, DBAL_FIELD_EEDB_LL_EXTRA_DATA, 0,
                                                                 dbal_result_type, 0, &extra_data_field_info);

        /*
         * If extra data field exists - it can also be ignored 
         */
        if (extra_data_exists_rv == _SHR_E_NONE)
        {
            entry_size_in_bits -= extra_data_field_info.field_nof_bits;
        }
    }

    /*
     * Set the number of outlifs per entry.
     */
    *nof_outlifs_per_entry = UTILEX_DIV_ROUND_UP(entry_size_in_bits, address_granularity);

    if (address_granularity == 30 && *nof_outlifs_per_entry == 3)
    {
        /*
         * Special case: a 90b entry in granularity of 30 would actually take up 120b.
         */
        *nof_outlifs_per_entry = 4;
    }

    if (rt_has_ll)
    {
        /*
         * In case the result type has ll - the extra bits in the eedb bank can be used for optimization - to store
         * data 
         */
        actual_extradata_bits = MDB_DIRECT_BASIC_ENTRY_SIZE - ll_field_info.field_nof_bits;
        if ((entry_size_in_bits > address_granularity) && (entry_size_in_bits % address_granularity))
        {
            if ((entry_size_in_bits % address_granularity) <= actual_extradata_bits)
                /** If extra data field exists, and we can utilize it to store data instead of allocating another entry - reduce number of lifs per entry */
                *nof_outlifs_per_entry = *nof_outlifs_per_entry - 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given lif_mngr outlif info, translates it to the resource manager allocation info.
 *
 *  \param [in] unit - Unit-ID
 *  \param [in] outlif_info - Local outlif info.
 *  \param [out] resource_info - resource info.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_algo_local_outlif_info_to_resource_info(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_lif_local_outlif_resource_info_t * resource_info)
{
    uint8 address_granularity;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the logical phase, then get its granularity.
     * If the phase is invalid, then get the phase by the outlif, which should be set.
     * Since this is set by physical phase, we need to get it first.
     */
    logical_phase = outlif_info->logical_phase;
    if (outlif_info->logical_phase == DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID)
    {
        int outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(outlif_info->local_outlif);
        uint8 tmp;
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, outlif_bank, &tmp));
        logical_phase = tmp;
    }

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, logical_phase, &address_granularity));

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
         * If the granularity is 120, then every size of entry takes just 1 index and fragmentation is impossible.
         * Also, every table/result type combination takes exactly 1 entry, so no need for further calculations.
         */
        resource_info->nof_outlifs_per_entry = 1;
        resource_info->nof_outlifs_per_alternative_entry = 1;
        resource_info->use_tag_and_align = FALSE;
        SHR_EXIT();
    }

    /*
     * Get the nof outlifs per entry.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_result_type_and_granularity_to_nof_outlifs(unit,
                                                                                     address_granularity,
                                                                                     outlif_info->dbal_table_id,
                                                                                     outlif_info->dbal_result_type,
                                                                                     &resource_info->nof_outlifs_per_entry));

    if (_SHR_IS_FLAG_SET
        (outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE))
    {
        /*
         * If we have an alternative result type, get its size as well.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_result_type_and_granularity_to_nof_outlifs(unit,
                                                                                         address_granularity,
                                                                                         outlif_info->dbal_table_id,
                                                                                         outlif_info->alternative_result_type,
                                                                                         &resource_info->nof_outlifs_per_alternative_entry));
    }

exit:
    SHR_FUNC_EXIT;
}

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
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    int nof_eedb_banks_per_outlif_bank, nof_outlifs_per_entry;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    dnx_algo_lif_local_outlif_resource_extra_info_t extra_info = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Prepare create struct.
     */
    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    sal_strncpy(res_tag_bitmap_create_info.name, create_data->name, sizeof(res_tag_bitmap_create_info.name));
    sal_strncpy(res_tag_bitmap_create_info.advanced_algo_name, create_data->advanced_algo_name,
                sizeof(res_tag_bitmap_create_info.advanced_algo_name));
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements;
    res_tag_bitmap_create_info.first_element = create_data->first_element;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;
    res_tag_bitmap_create_info.advanced_algorithm = create_data->advanced_algorithm;

    if (create_data->first_element == 1)
    {
        /*
         * In case the rif is calling the advanced algorithm create, then it will specify 1 as its first element and
         * nof_rifs - 1 as the number of elements. In this case, we need to round up the bitmap's count,
         * otherwise we'll get an error when creating it.
         */
        res_tag_bitmap_create_info.nof_elements++;
        res_tag_bitmap_create_info.first_element = 0;
    }

    extra_info = *((dnx_algo_lif_local_outlif_resource_extra_info_t *) extra_arguments);
    nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / extra_info.granularity;

    /*
     * For eedb banks outlif banks - the grain size represents the number of eedb banks needed to fill the outlif bank 
     */
    if (extra_info.assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB)
    {
        nof_eedb_banks_per_outlif_bank =
            DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) /
            (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows * nof_outlifs_per_entry);

        extra_create_info.grain_size = nof_eedb_banks_per_outlif_bank;
        extra_create_info.max_tag_value = utilex_msb_bit_on(extra_create_info.grain_size);
    }
    else        /* assigned = DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB */
    {
        /*
         * For cluster outlif banks - the grain size represents the number of outlifs that can be fitted to each entry 
         */
        extra_create_info.grain_size = nof_outlifs_per_entry;
        extra_create_info.max_tag_value = utilex_msb_bit_on(extra_create_info.grain_size);
        if (extra_create_info.grain_size == 1)
        {
            /*
             * A grain of 1 means we get no optimization by using grains, so we may as well have only
             * one grain for the entire resource range.
             */
            extra_create_info.grain_size = res_tag_bitmap_create_info.nof_elements;
            extra_create_info.max_tag_value = 0;
        }
    }

    if (_SHR_IS_FLAG_SET(create_data->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
    {
        /*
         * If we got the allow forcing tag flag, it means that we need to support the illegal tag as the
         * max tag value.
         */
        res_tag_bitmap_create_info.flags |= RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;
        extra_create_info.max_tag_value = OUTLIF_RESOURCE_ILLEGAL_TAG;
    }

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, module_id, res_tag_bitmap, &res_tag_bitmap_create_info,
                                               &extra_create_info, nof_elements, alloc_flags));

    if (create_data->first_element == 1)
    {
        /*
         * In case the rif is calling the advanced algorithm create, then it will specify 1 as its first element and
         * nof_rifs - 1 as the number of elements. We need to allocate 1 so it will be unusable.
         * It will not show up when iterating over rif, because the resource manager will start the iteration from 1.
         */
        resource_tag_bitmap_alloc_info_t alloc_info;
        int element;

        sal_memset(&alloc_info, 0, sizeof(alloc_info));

        alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
        alloc_info.tag = 0;
        alloc_info.count = 1;
        element = 0;
        SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, *res_tag_bitmap, &alloc_info, &element));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The advanced fucntionality in this function compared to the basic algorithm is that it only returns TRUE
 * for the first index that an outlif takes, instead of all the indexes like a bitmap would return.
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
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    uint8 allocation_size;
    resource_tag_bitmap_tag_info_t tag_info;
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
    sal_memset(&tag_info, 0, sizeof(tag_info));
    tag_info.element = element;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

    allocation_size = OUTLIF_RESOURCE_TAG_TO_ALLOCATION_SIZE(tag_info.tag);

    if (element % allocation_size == 0)
    {
        /*
         * The element is a multiple of the entry size. In that case, the bitmap
         * can handle it.
         */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_is_allocated(unit, module_id, res_tag_bitmap, element, is_allocated));
    }
    else
    {
        *is_allocated = FALSE;
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
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    lif_mngr_local_outlif_info_t *outlif_info;
    dnx_algo_lif_local_outlif_resource_info_t resource_info;
    uint32 allocation_size, tag = 0;
    resource_tag_bitmap_alloc_info_t alloc_info;
    resource_tag_bitmap_tag_info_t tag_info;
    uint8 alloc_simulation, with_id, with_tag;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&alloc_info, 0, sizeof(alloc_info));
    sal_memset(&resource_info, 0, sizeof(resource_info));
    sal_memset(&tag_info, 0, sizeof(tag_info));

    alloc_simulation = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_SIMULATION);

    if (alloc_simulation)
    {
        alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }

    with_id =
        _SHR_IS_FLAG_SET(flags,
                         DNX_ALGO_RES_ALLOCATE_WITH_ID | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE |
                         DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID);

    if (with_id)
    {
        alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
    }

    with_tag = (_SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG));
    if (with_tag)
    {
        tag_info.element = *element;
        SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

        tag = tag_info.tag;
        alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG;
    }

    outlif_info = (lif_mngr_local_outlif_info_t *) extra_arguments;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, outlif_info, &resource_info));

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE))
    {
        /*
         * If replace operation, then entry is already allocated, and we need to free it before allocating.
         * First verify that entry exists and its size match the old entry.
         */
        uint8 is_allocated;
        resource_tag_bitmap_alloc_info_t free_info;
        uint32 old_size;

        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_resource_is_allocated(unit, module_id, res_tag_bitmap,
                                                                    *element, &is_allocated));

        if (!is_allocated)
        {
            /*
             * Entry wasn't allocated, return error.
             */
            if (alloc_simulation)
            {
                /*
                 * Return silent error.
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trying to replace an outlif that doesn't exist, or is too small.");
            }
        }

        /*
         * Entry is allocated, but verify that the entry size is big enough to hold the new entry.
         */

        sal_memset(&tag_info, 0, sizeof(tag_info));
        tag_info.element = *element;
        SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

        old_size = OUTLIF_RESOURCE_TAG_TO_ALLOCATION_SIZE(tag_info.tag);

        if (old_size == resource_info.nof_outlifs_per_entry)
        {
            /*
             * If the old entry is the same size as the new one, then nothing to do here.
             * The old and new entries are the same as far as resource manager is concerned.
             * Just return here.
             */
            SHR_EXIT();
        }
        if (old_size < resource_info.nof_outlifs_per_entry)
        {
            /*
             * If the old size is smaller than the new size, and the entry wasn't already reserved, then we're not
             * guaranteed success, so the allocation fails.
             */
            if (alloc_simulation)
            {
                /*
                 * Return silent error.
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trying to replace an existing outlif with a larger outlif size.");
            }
        }

        if (alloc_simulation)
        {
            /*
             * Free doesn't have a simulation so we can't run it, but if we got here then it would succeed.
             */
            SHR_EXIT();
        }

        /*
         * Free the entry and then reallocate it.
         */
        sal_memset(&free_info, 0, sizeof(free_info));

        free_info.count = resource_info.nof_outlifs_per_alternative_entry;
        SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free(unit, module_id, res_tag_bitmap, &free_info, *element));
    }

    allocation_size = resource_info.nof_outlifs_per_entry;

    if (with_tag)
    {
        tag_info.tag = tag;
    }
    if (resource_info.use_tag_and_align)
    {
        /*
         * Align the entry if it's required.
         * The tag represents the size of the entry. Since the size of the entry is always a power of 2,
         * we can optimize and use MSB_BIT_ON to represent it.
         *
         */
        alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
        alloc_info.align = allocation_size;
        alloc_info.count = allocation_size;
        if (!with_tag)
        {
            alloc_info.tag = OUTLIF_RESOURCE_ALLOCATION_SIZE_TO_TAG(allocation_size);
        }
    }
    else
    {
        /*
         * Don't align the entry if it's not required.
         * So just set the count.
         */
        alloc_info.count = allocation_size;
    }
    rv = resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, &alloc_info, element);

    if (!alloc_simulation)
        SHR_IF_ERR_EXIT(rv);
    else
        SHR_SET_CURRENT_ERR(rv);

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
dnx_lif_lib_encap_access_to_logical_phase_convert(
    int unit,
    bcm_encap_access_t encap_access,
    dnx_algo_local_outlif_logical_phase_e * logical_phase)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(logical_phase, _SHR_E_PARAM, "logical_phase");

    switch (encap_access)
    {
        case bcmEncapAccessTunnel1:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3;
            break;
        case bcmEncapAccessTunnel2:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4;
            break;
        case bcmEncapAccessTunnel3:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5;
            break;
        case bcmEncapAccessTunnel4:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6;
            break;
        case bcmEncapAccessNativeArp:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2;
            break;
        case bcmEncapAccessArp:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7;
            break;
        case bcmEncapAccessRif:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;
            break;
        case bcmEncapAccessAc:
            *logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8;
            break;
        case bcmEncapAccessInvalid:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unhandled bcmEncapAccessInvalid value, shouldn't be used internally");
        case bcmEncapAccessNativeAc:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcmEncapAccessNativeAc is not supported");
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "encap_access value (%d) is not supported", encap_access);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_logical_phase_to_encap_access_convert(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    bcm_encap_access_t * encap_access)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_access, _SHR_E_PARAM, "encap_access");

    switch (logical_phase)
    {
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1:
            *encap_access = bcmEncapAccessRif;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2:
            *encap_access = bcmEncapAccessNativeArp;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3:
            *encap_access = bcmEncapAccessTunnel1;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4:
            *encap_access = bcmEncapAccessTunnel2;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5:
            *encap_access = bcmEncapAccessTunnel3;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6:
            *encap_access = bcmEncapAccessTunnel4;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7:
            *encap_access = bcmEncapAccessArp;
            break;
        case DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8:
            *encap_access = bcmEncapAccessAc;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "logical_phase value (%d) is not supported by bcm_encap_access_t",
                         logical_phase);
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

            outlif_hw_info->data_bank_info.mdb_eedb_type = MDB_EEDB_MEM_TYPE_DATA_BANK;

            outlif_hw_info->data_bank_info.dbal_physical_table_id =
                EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(physical_phase);

            /*
             * Calculate the number of eedb banks per outlif bank for this phase.
             * We'll use this value below.
             */
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            address_granularity.get(unit, logical_phase, &address_granularity));

            /** Fix COVERITY */
            if (address_granularity == 0)
            {
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
            nof_outlifs_per_eedb_bank =
                EEDB_MAX_DATA_ENTRY_SIZE / address_granularity * dnx_data_mdb.dh.macro_type_info_get(unit,
                                                                                                     MDB_EEDB_BANK)->nof_rows;

            eedb_bank_array_index = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_OFFSET(local_outlif) / nof_outlifs_per_eedb_bank;

            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.get(unit,
                                                                                   outlif_bank,
                                                                                   eedb_bank_array_index, &tmp));
            outlif_hw_info->data_bank_info.mdb_eedb_type = MDB_EEDB_MEM_TYPE_DATA_BANK;

            outlif_hw_info->data_bank_info.macro_type = MDB_EEDB_BANK;

            outlif_hw_info->data_bank_info.cluster_idx = tmp;

            outlif_hw_info->data_bank_info.macro_idx = 0;

            outlif_hw_info->data_bank_info.logical_start_address =
                outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) + nof_outlifs_per_eedb_bank * eedb_bank_array_index;
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

    for (outlif_prefix_index = 0; outlif_prefix_index < MAX_EEDB_BANKS_PER_OUTLIF_BANK; outlif_prefix_index++)
    {
        /*
         * Reset the outlif bank eedb banks to 0.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.eedb_data_banks.set(unit,
                                                                               outlif_bank, outlif_prefix_index, 0));
    }

    /*
     * Remove the bank from the phase's linked list.
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
    resource_tag_bitmap_tag_info_t tag_set_info;

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
    sal_memset(&tag_set_info, 0, sizeof(resource_tag_bitmap_tag_info_t));
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
    resource_tag_bitmap_tag_info_t tag_set_info;

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
    sal_memset(&tag_set_info, 0, sizeof(resource_tag_bitmap_tag_info_t));
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
dnx_algo_local_outlif_alloc_in_used_bank(
    int unit,
    int bank,
    dnx_algo_local_outlif_logical_phase_e required_logical_phase,
    int ll_required,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *bank_match,
    int *bank_can_be_used)
{
    uint8 bank_logical_phase;
    uint8 ll_in_use;
    int element;
    shr_error_e rv;
    int nof_rifs;
    uint8 is_rif;
    int nof_free_elements;
    int flags = 0;
    
    bsl_severity_t orig_severity_res_mngr, orig_severity_lif;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We'll set it to true if it's a match.
     */
    *bank_can_be_used = FALSE;
    *bank_match = FALSE;

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_free_elements_get(unit, bank, &nof_free_elements));

    if (!nof_free_elements)
    {
        /*
         * No entries in bank, no need to run allocation simulation in it 
         */
        SHR_EXIT();
    }

    /*
     * If bank is already in use, check that it's used by the correct logical phase.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, bank, &bank_logical_phase));

    if (bank_logical_phase == required_logical_phase)
    {
        /*
         * Logical phase fits. If it's for phase 1 - check that it matches rif or non-rif
         */
        if (bank_logical_phase == DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1)
        {
            nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);
            is_rif = ((outlif_info->logical_phase == DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1)
                      && (outlif_info->dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)) ? TRUE : FALSE;

            /**In case of rif - check that the bank is within the rif range*/
            if (is_rif)
            {
                if ((bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit)) > nof_rifs)
                {
                    SHR_EXIT();
                }
            }
            else
            {
                /**Not a rif - check that the bank is not in the rif range*/
                if ((bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit)) < nof_rifs)
                {
                    SHR_EXIT();
                }
            }
        }
        /*
         * Now check that the LL fits the requirement.
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

    if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG))
    {
        flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG;
    }
    orig_severity_res_mngr = bslenable_get(bslLayerBcmdnx, bslSourceResmngr);
    orig_severity_lif = bslenable_get(bslLayerBcmdnx, bslSourceLif);

    bslenable_set(bslLayerBcmdnx, bslSourceResmngr, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourceLif, bslSeverityOff);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityOff);

    rv = lif_mngr_db.egress_local_lif.allocate_single(unit, bank, flags, (void *) outlif_info, &element);

    bslenable_set(bslLayerBcmdnx, bslSourceResmngr, orig_severity_res_mngr);
    bslenable_set(bslLayerBcmdnx, bslSourceLif, orig_severity_lif);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    if (rv == _SHR_E_NONE)
    {
        /*
         * Allocation successful.
         * Return TRUE, keep the allocation.
         */
        *bank_can_be_used = TRUE;
        outlif_info->local_outlif = element;
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
 * Given an outlif logical phase and linked list requirement, attempts to find
 * an available eedb bank to use to store data.
 * This function will first try to find an existing bank, and failing that, will attempt
 * to allocate a new one.
 */
static shr_error_e
dnx_algo_local_outlif_bank_in_eedb_bank_find(
    int unit,
    uint32 local_lif_flags,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int ll_required,
    lif_mngr_local_outlif_info_t * outlif_info,
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

    /** Fix COVERITY */
    if (address_granularity == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
    nof_eedb_banks_per_outlif_bank =
        DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows *
                                                 nof_outlifs_per_entry);

    /*
     * First, check this phase's list of used eedb banks to see if a bank has already been allocated.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.get_first(unit, logical_phase, &current_node));
    extendable_outlif_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;
    new_eedb_bank_array_index = 0;
    while (DNX_SW_STATE_LL_IS_NODE_VALID(current_node)
           && !_SHR_IS_FLAG_SET(local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION))
    {
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.node_value(unit, current_node, &current_outlif_bank));

        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_alloc_in_used_bank
                        (unit, current_outlif_bank, logical_phase, ll_required, outlif_info, &outlif_bank_match,
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

    if (_SHR_IS_FLAG_SET(local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_ALLOCATE_NEW_BANK))
    {
        /*
         * If a useable bank was found, it would've ended the function.
         * Since don't allocate new bank indication was given, don't try to allocate a new eedb bank.
         */
        SHR_EXIT();
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
        /*
         * It's impossible to allocate in an eedb bank. Return illegal bank.
         */
        *outlif_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;
        SHR_EXIT();
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

    /** Fix COVERITY */
    if (address_granularity == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
    nof_eedb_banks_per_outlif_bank =
        DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) / (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows *
                                                 nof_outlifs_per_entry);

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

        /*
         * Set the eedb bank as not in use.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_unassign_eedb_bank_to_outlif_bank
                        (unit, outlif_bank, eedb_bank_array_index, nof_outlifs_per_eedb_bank));

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

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Iterate over the outlif banks.
 * Try to find a bank that's in use, and allocate in it.
 * While passing over the banks, we also note if there's an unused bank we can use if we didn't find a used bank.
 */
static shr_error_e
dnx_algo_local_outlif_allocate_in_bank(
    int unit,
    uint32 local_lif_flags,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint8 ll_required,
    lif_mngr_local_outlif_info_t * outlif_info,
    int first_bank_for_phase,
    int last_bank_for_phase,
    uint32 *outlif_bank,
    uint32 *first_unused_bank_in_mdb_cluster)
{
    int current_bank, bank_can_be_used, bank_match;
    uint8 in_use;

    SHR_FUNC_INIT_VARS(unit);

    for (current_bank = first_bank_for_phase; current_bank <= last_bank_for_phase; current_bank++)
    {
        /*
         * Check if the bank is in use.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, current_bank, &in_use));

        if (in_use && !_SHR_IS_FLAG_SET(local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION))
        {
            /*
             * If bank is in use, make sure it's matching the phase and ll requirement, and
             * run an allocation simulation.
             */
            bank_can_be_used = FALSE;
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_alloc_in_used_bank
                            (unit, current_bank, logical_phase, ll_required, outlif_info, &bank_match,
                             &bank_can_be_used));

            if (bank_can_be_used)
            {
                /*
                 * Bank matches the criteria and the allocation simulation passed. Use it.
                 */
                *outlif_bank = current_bank;
                break;
            }
        }
        else if (*first_unused_bank_in_mdb_cluster == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
        {
            /*
             * Mark first unused bank in mdb cluster
             */
            *first_unused_bank_in_mdb_cluster = current_bank;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Try to find a used outlif bank for the given combination of phase, ll_required.
 * First - if exists and there's no ll demand - try to allocate in non-ll outlif banks (above 512k).
 * Second - try to allocate in the outlif banks below 512k
 * Third - try to allocate in an outlif bank mapped to an eedb bank for data
 */
static shr_error_e
dnx_algo_local_outlif_allocation_find_existing_bank_to_allocate(
    int unit,
    uint32 local_lif_flags,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint8 ll_required,
    lif_mngr_local_outlif_info_t * outlif_info,
    uint32 *outlif_bank,
    uint32 *first_unused_bank_in_mdb_cluster_p)
{
    uint8 disable_mdb_clusters, disable_eedb_data_banks;
    uint32 current_bank, first_bank_for_phase = 0, last_bank_for_phase = 0, first_bank_for_phase_no_ll =
        0, last_bank_for_phase_no_ll = 0;
    uint32 first_unused_bank_in_mdb_cluster;
    int nof_rifs, nof_rif_banks;

    SHR_FUNC_INIT_VARS(unit);

    *outlif_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;

    /*
     * Check whether to skip mdb cluster allocation and go straight to eedb bank allocation.
     */
    if (_SHR_IS_FLAG_SET(local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_DATA_IN_EEDB_BANKS))
    {
        disable_mdb_clusters = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(local_outlif_info.disable_mdb_clusters.get(unit, &disable_mdb_clusters));
    }

    SHR_IF_ERR_EXIT(local_outlif_info.disable_eedb_data_banks.get(unit, &disable_eedb_data_banks));

    /*
     * Get first and last banks mapped to this phase
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, logical_phase, &first_bank_for_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, logical_phase, &last_bank_for_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    first_no_ll_bank.get(unit, logical_phase, &first_bank_for_phase_no_ll));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    last_no_ll_bank.get(unit, logical_phase, &last_bank_for_phase_no_ll));

    first_unused_bank_in_mdb_cluster = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;

    /**Additional checks for logical phase 1, which is shared between rif and others*/
    if ((logical_phase == DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1)
        && ((outlif_info->logical_phase != DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1)
            || (outlif_info->dbal_table_id != DBAL_TABLE_EEDB_RIF_BASIC)))
    {
        /**In case of lif mngr phase which is not rif - don't allocate from the rif banks*/
        nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);
        nof_rif_banks = UTILEX_DIV_ROUND_UP(nof_rifs, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));
        first_bank_for_phase += nof_rif_banks;
    }

    if (!disable_mdb_clusters)
    {
        if ((!ll_required) && (first_bank_for_phase_no_ll))
        {
            /*
             * If ll is not required, and there're reserved outlif banks above DNX_ALGO_LOCAL_OUTLIF_LL_CAPABLE_NOF_BANKS(unit) (512k) - try to use them
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocate_in_bank
                            (unit, local_lif_flags, logical_phase, ll_required, outlif_info, first_bank_for_phase_no_ll,
                             last_bank_for_phase_no_ll, outlif_bank, &first_unused_bank_in_mdb_cluster));
        }
        if ((*outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
            && (first_unused_bank_in_mdb_cluster == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK))
        {
            /*
             * If no outlif bank was found - try to allocate in the regular range
             */
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocate_in_bank
                            (unit, local_lif_flags, logical_phase, ll_required, outlif_info, first_bank_for_phase,
                             last_bank_for_phase, outlif_bank, &first_unused_bank_in_mdb_cluster));
        }
    }

    if ((!disable_eedb_data_banks) && (*outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK))
    {
        /*
         * If the loop reached the last bank and didn't break, it means we failed to allocate in a used bank.
         * Check if an outlif bank already exists in an eedb bank, and if not, try to allocate a new unused bank.
         */
        current_bank = DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK;
        /*
         * Try using an existing EEDB bank. If none is found, try to allocate a new bank.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_eedb_bank_find
                        (unit, local_lif_flags | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_ALLOCATE_NEW_BANK,
                         logical_phase, ll_required, outlif_info, NULL, &current_bank));

        *outlif_bank = current_bank;
    }

    /*
     * If given - update the first unused bank
     */
    if (first_unused_bank_in_mdb_cluster_p)
    {
        *first_unused_bank_in_mdb_cluster_p = first_unused_bank_in_mdb_cluster;
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
    dnx_algo_local_outlif_logical_phase_e logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID;
    uint8 ll_required = FALSE;
    uint8 rt_has_ll = FALSE;
    uint32 outlif_bank, first_unused_bank_in_mdb_cluster, flags;
    int element, active_result_type, rv;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(outlif_hw_info, 0, sizeof(dnx_algo_local_outlif_hw_info_t));

    if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE))
    {
        if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE))
        {
            /*
             * If reserve and replace flags are set, it means that the old entry type was small
             * and a larger entry type is already allocated. Therefore, nothing to do here.
             */
            SHR_EXIT();
        }

        /*
         * If replace operation, then we don't need to calculate the bank.
         * Just get it from the outlif itself.
         */
        element = outlif_info->local_outlif;
        outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(element);
        flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE;
    }
    else
    {
        if (!(_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID)))
        {
            outlif_info->local_outlif = INVALID_LOCAL_OUTLIF;
        }

        flags = 0;
        /*
         * Get the linked list requirement through flag indication or dbal API.
         * For stress test, don't use linked list.
         * If the bank traffic lock is in effect, always use the banks with linked list because some phases will have a problem to
         *
         */

        active_result_type =
            (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE)) ?
            outlif_info->alternative_result_type : outlif_info->dbal_result_type;

        SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit,
                                                                   outlif_info->dbal_table_id,
                                                                   active_result_type, &rt_has_ll));
        if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST))
        {
            ll_required = TRUE;
        }
        else if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST))
        {
            ll_required = FALSE;
        }
        else
        {
            /**If we got here - no link list flag was set. Link list indication is taken from RT - if link list exists on it, then ll_required == True*/
            ll_required = rt_has_ll;
        }

        /*
         * Get the entry's logical phase. 
         */
        logical_phase = outlif_info->logical_phase;

        if ((ll_required == FALSE) && (rt_has_ll))
        {
            dbal_table_field_info_t field_info;
            rv = dbal_tables_field_info_get_no_err(unit, outlif_info->dbal_table_id, DBAL_FIELD_EEDB_LL_EXTRA_DATA, 0,
                                                   outlif_info->dbal_result_type, 0, &field_info);
            /**If the extra data bits can be placed on the ll data field - use it, no need to use another lif*/
            if (((rv == _SHR_E_NONE) && (field_info.field_nof_bits < EEDB_LL_DATA_MAX_SIZE))
                || (rv == _SHR_E_NOT_FOUND))
            {
                outlif_info->local_lif_flags &= (~DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST);
                outlif_info->local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST;
                ll_required = TRUE;
            }
        }

        if ((ll_required == FALSE) && (!rt_has_ll))
        {
            uint8 found;
            dbal_to_phase_info_t dbal_to_phase_info = { 0 };
            dnx_local_outlif_ll_indication_e ll_indication;
            dbal_to_phase_info.dbal_result_type = outlif_info->dbal_result_type;
            dbal_to_phase_info.dbal_table = outlif_info->dbal_table_id;
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            dbal_valid_combinations.find(unit, outlif_info->logical_phase, &dbal_to_phase_info,
                                                         &ll_indication, &found));
            if (found && ll_indication == DNX_LOCAL_OUTLIF_LL_ALWAYS)
            {
                ll_required = TRUE;
            }
        }

        /*
         * Try to find an existing used outlif bank. If we won't succeed - we'll later allocate in a free outlif bank
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_find_existing_bank_to_allocate(unit,
                                                                                        outlif_info->local_lif_flags,
                                                                                        logical_phase, ll_required,
                                                                                        outlif_info, &outlif_bank,
                                                                                        &first_unused_bank_in_mdb_cluster));

        /*
         * If we have the eedb traffic lock, and there's no available outlif bank in the mdb cluster, then it's likely
         * that there won't be an available eedb bank either. Try to allocate in an outlif bank that has linked list.
         */
        if (outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK
            && first_unused_bank_in_mdb_cluster == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK && !ll_required
            && dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_eedb_bank_traffic_lock))
        {
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_find_existing_bank_to_allocate(unit,
                                                                                            outlif_info->local_lif_flags,
                                                                                            logical_phase, TRUE,
                                                                                            outlif_info, &outlif_bank,
                                                                                            NULL));
        }

        /*
         * If the loop reached the last bank and didn't break, it means we failed to allocate in a used bank.
         * Check if we can allocate in an unused mdb cluster or eedb data bank.
         */
        if (outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
        {
            if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_ALLOCATE_NEW_BANK))
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "No room to allocate in existing outlif banks, and DONT_ALLOCATE_NEW_BANK flag set.");
            }

            if (first_unused_bank_in_mdb_cluster != DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
            {
                /*
                 * Before trying to allocate a bank, if ll is required then verify that we have an available ll bank.
                 */
                if (ll_required)
                {
                    int is_available = FALSE;
                    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_verify_ll_availability
                                    (unit, logical_phase, 1, &is_available));

                    if (!is_available)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE, "All banks using linked list on logical phase %d are full.",
                                     logical_phase + 1);
                    }
                }

                /*
                 * Assign the bank.
                 */
                if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID))
                {
                    first_unused_bank_in_mdb_cluster =
                        UTILEX_DIV_ROUND_DOWN(outlif_info->local_outlif, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));
                }
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_use_set
                                (unit, first_unused_bank_in_mdb_cluster, logical_phase, ll_required));
                if (ll_required)
                {
                    outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK;
                }
                outlif_hw_info->flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS;
                outlif_bank = first_unused_bank_in_mdb_cluster;
            }
            else
            {
                uint8 disable_eedb_data_banks;
                SHR_IF_ERR_EXIT(local_outlif_info.disable_eedb_data_banks.get(unit, &disable_eedb_data_banks));
                if (!disable_eedb_data_banks)
                {
                    /*
                     * Try using an EEDB bank. If none is found, and none can be allocated, then an error will be returned.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_in_eedb_bank_find
                                    (unit, outlif_info->local_lif_flags, logical_phase, ll_required, outlif_info,
                                     &outlif_hw_info->flags, &outlif_bank));
                }
            }

            /*
             * If linked list is not required, and no bank is found, then try to allocate in an existing bank that uses linked list.
             */
            if (outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK && !ll_required)
            {
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_find_existing_bank_to_allocate(unit,
                                                                                                outlif_info->local_lif_flags,
                                                                                                logical_phase, TRUE,
                                                                                                outlif_info,
                                                                                                &outlif_bank, NULL));
            }

            /*
             * Finally, if no outlif bank was found, return error.
             */
            if (outlif_bank == DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "No available outlif bank with%s linked list for logical phase %d.",
                             (ll_required) ? "" : "out", logical_phase + 1);
            }
        }
    }

    /*
     * If we reached this point, it means that we can allocate in current_bank.
     */
    if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID))
    {
        flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID;
        element = outlif_info->local_outlif;
    }
    /**Internal flag, enables to set local lif id in its corresponding tag*/
    if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG))
    {
        flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG;
    }

    /*
     * If there was not already a good allocation before - allocate on the free bank
     */
    if (outlif_info->local_outlif == INVALID_LOCAL_OUTLIF)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.allocate_single
                        (unit, outlif_bank, flags, (void *) outlif_info, &element));
    }
    else
    {
        /*
         * Prior allocation succeded, only assign to element 
         */
        element = outlif_info->local_outlif;
    }

    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
    {
        /*
         * If a new outlif bank was allocated, get the HW info required by the provided flags
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_hw_info_get(unit, element, outlif_hw_info));
    }

    outlif_info->local_outlif = element;

    /**If the outlif pointer size is 20 - in order to avoid aliasing, allow local outlif allocation with msb set to 0*/
    if ((DNX_ALGO_OUTLIF_IS_POINTER_BIGGER_THAN_EEDB_RESOURCE) && (ll_required))
    {
        if (element >= (1 << dnx_data_lif.out_lif.outlif_eedb_banks_pointer_size_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "Can't allocate lif with ll for logical phase %d", logical_phase);
        }
    }

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
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, outlif_info, &deallocation_size));

    SHR_IF_ERR_EXIT(lif_mngr_db.
                    egress_local_lif.free_several(unit, outlif_bank, deallocation_size, outlif_info->local_outlif,
                                                  NULL));

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

shr_error_e
dnx_algo_local_outlif_ll_match_check(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *ll_match)
{
    uint8 ll_required, alternative_ll_required;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if old entry had ll 
     */
    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, outlif_info->local_outlif, NULL, NULL, NULL, NULL, &alternative_ll_required, NULL));

    /*
     * Check if new entry requires ll 
     */
    if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST))
    {
        ll_required = TRUE;
    }
    else if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST))
    {
        ll_required = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit,
                                                                   outlif_info->dbal_table_id,
                                                                   outlif_info->alternative_result_type, &ll_required));
    }

    if (alternative_ll_required || !ll_required)
    {
        /*
         * If the old result type required LL, then it can be changed to entry without linked list without a
         * problem.
         * If new type has linked list - fine.
         * It it doesn't - it's possible not to have linked list in a linked list bank.
         *
         * If the new type doesn't require linked list, then we don't care.
         */
        *ll_match = TRUE;
    }
    else
    {
        /*
         * Only possible case is new type requires linked list and old type didn't.
         * Can't put entry without linked list in a bank with linked list.
         */
        *ll_match = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_can_be_reused(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *can_be_reused)
{
    dnx_algo_lif_local_outlif_resource_info_t resource_info;
    int can_be_reused_tmp, ll_match;
    uint32 original_flags;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    uint8 address_granularity;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get address granularity.
     */
    logical_phase = outlif_info->logical_phase;
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, logical_phase, &address_granularity));

    /*
     * Get entry sizes.
     */
    original_flags = outlif_info->local_lif_flags;
    outlif_info->local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, outlif_info, &resource_info));
    outlif_info->local_lif_flags = original_flags;

    if ((resource_info.nof_outlifs_per_alternative_entry < resource_info.nof_outlifs_per_entry)
        && _SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE)
        && _SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE))
    {
        /*
         * If the reserve and replace flags are given, then we assume the entry was already reserved with
         * the larger size, and now it can be replaced.
         */
        can_be_reused_tmp = TRUE;
    }
    else if (address_granularity == 30 && resource_info.nof_outlifs_per_alternative_entry == 2
             && resource_info.nof_outlifs_per_entry == 1)
    {
        /*
         * Special edge case - if the address granularity is 30 and we replace a 60b entry to 30b, we will
         * have a conflict of tags on the same 120b line. Don't allow that.
         */
        can_be_reused_tmp = FALSE;
    }
    else if (resource_info.nof_outlifs_per_alternative_entry >= resource_info.nof_outlifs_per_entry)
    {
        /*
         * Entry is smaller than previous entry, no problem.
         */
        can_be_reused_tmp = TRUE;
    }
    else
    {
        /*
         * Entry is larger than previous entry, can't reuse.
         */
        can_be_reused_tmp = FALSE;
    }

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_ll_match_check(unit, outlif_info, &ll_match));

    *can_be_reused = can_be_reused_tmp && ll_match;

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
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS], *cluster;
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
    int last_bank,
    int first_no_ll_bank,
    int last_no_ll_bank)
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
     * Set the first bank for the current phase above 512k.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    first_no_ll_bank.set(unit, first_logical_phase, first_no_ll_bank));

    /*
     * Set the pair phase with the same value.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.set(unit, second_logical_phase, first_bank));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    first_no_ll_bank.set(unit, second_logical_phase, first_no_ll_bank));

    /*
     * Set the current phase pair last bank.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.set(unit, first_logical_phase, last_bank));

    /*
     * Set the current phase pair last bank above 512k.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    last_no_ll_bank.set(unit, first_logical_phase, last_no_ll_bank));

    /*
     * Set the pair phase with the same value.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.set(unit, second_logical_phase, last_bank));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    last_no_ll_bank.set(unit, second_logical_phase, last_no_ll_bank));

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
    dnx_algo_res_create_data_t res_data;
    resource_tag_bitmap_tag_info_t tag_set_info;
    int *bank_address_granularities, current_granularity, minimum_granularity;
    uint32 first_bank, last_bank, first_bank_no_ll, last_bank_no_ll, current_bank;
    uint8 phase_address_granularity;
    uint32 current_outlif_bank;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    dnx_algo_lif_local_outlif_resource_extra_info_t extra_info = { 0 };

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
    minimum_granularity = EEDB_MAX_GRANULARITY;
    SHR_ALLOC_SET_ZERO(bank_address_granularities, sizeof(int) * (DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit)),
                       "bank_address_granularities", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; logical_phase++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, logical_phase, &first_bank));
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, logical_phase, &last_bank));
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        first_no_ll_bank.get(unit, logical_phase, &first_bank_no_ll));
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        last_no_ll_bank.get(unit, logical_phase, &last_bank_no_ll));

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        address_granularity.get(unit, logical_phase, &phase_address_granularity));

        for (current_bank = first_bank; current_bank <= last_bank; current_bank++)
        {
            bank_address_granularities[current_bank] = phase_address_granularity;
        }

        for (current_bank = first_bank_no_ll; current_bank <= last_bank_no_ll; current_bank++)
        {
            bank_address_granularities[current_bank] = phase_address_granularity;
        }

        if (phase_address_granularity < minimum_granularity)
        {
            minimum_granularity = phase_address_granularity;
        }
    }

    /*
     * Create the resources for the out lif table.
     * The resource pool indexes indicate the outlif banks.
     */
    sal_memset(&res_data, 0, sizeof(res_data));

    res_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    res_data.nof_elements = DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_OUTLIF;
    sal_strncpy(res_data.name, "lif_local_outlif 2", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * Prepare the illegal tag info for the unassigned banks.
     */
    sal_memset(&tag_set_info, 0, sizeof(resource_tag_bitmap_tag_info_t));
    tag_set_info.force_tag = TRUE;
    tag_set_info.tag = OUTLIF_RESOURCE_ILLEGAL_TAG;
    tag_set_info.nof_elements = res_data.nof_elements;

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.alloc(unit, DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit)));
    for (current_outlif_bank = 0; current_outlif_bank < DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit); current_outlif_bank++)
    {
        if (bank_address_granularities[current_outlif_bank] == 0)
        {
            /*
             * If the outlif bank wasn't assigned a physical MDB cluster,
             * then it means that it's reserved for an eedb bank.
             * We'll use the minimum granularity for it. This allow us to fit every existing granularity.
             * The maximum tag value will be updated to allow us to use an illegal tag that can't be
             * used by any legal entry (0b11).
             */
            current_granularity = minimum_granularity;
            res_data.flags |= RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;
        }
        else
        {
            current_granularity = bank_address_granularities[current_outlif_bank];
        }
        res_data.first_element = OUTLIF_BANK_FIRST_INDEX(unit, current_outlif_bank);
        extra_info.granularity = current_granularity;
        extra_info.assigned =
            (bank_address_granularities[current_outlif_bank] ==
             0) ? DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB : DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB;
        SHR_IF_ERR_EXIT(lif_mngr_db.
                        egress_local_lif.create(unit, current_outlif_bank, &res_data, (void *) &extra_info));

        if (bank_address_granularities[current_outlif_bank] == 0)
        {
            res_data.flags &= ~RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;
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

/*Calc the required outlif banks which are cluster-related for each phase pair*/
static shr_error_e
dnx_algo_local_outlif_calc_required_cluster_outlif_banks(
    int unit,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int nof_required_outlif_banks[NOF_EEDB_PHASE_PAIRS])
{
    int current_cluster, current_bank_in_cluster, nof_banks_per_cluster, current_bank;
    uint32 nof_entries_in_cluster;
    mdb_cluster_alloc_info_t *current_cluster_info;
    mdb_macro_types_e current_macro_type, first_macro_type, last_macro_type;
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS];
    int nof_valid_clusters;
    int current_physical_phase_pair;
    dbal_physical_tables_e physical_table;
    int physical_phase;
    int total_outlif_banks = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (current_physical_phase_pair = 0; current_physical_phase_pair < NOF_EEDB_PHASE_PAIRS;
         current_physical_phase_pair++)
    {
        physical_phase = EEDB_PHYSICAL_TABLE_PHASE_PAIR_TO_FIRST_PHYSICAL_PHASE(current_physical_phase_pair);
        physical_table = EEDB_PHYSICAL_PHASE_TO_PHYSICAL_TABLE(physical_phase);

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
        current_bank = 0;

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
             * If alignment is needed and an additional outlif bank is required - update count
             */
            if (UTILEX_ALIGN_UP(total_outlif_banks, nof_banks_per_cluster) > total_outlif_banks)
            {
                total_outlif_banks++;
                nof_required_outlif_banks[current_physical_phase_pair]++;
            }
            nof_required_outlif_banks[current_physical_phase_pair] =
                UTILEX_ALIGN_UP(current_bank, nof_banks_per_cluster);

            for (current_cluster = 0; current_cluster < nof_valid_clusters; current_cluster++)
            {
                current_cluster_info = &clusters[current_cluster];
                if (current_cluster_info->dbal_physical_table_id == physical_table
                    && current_cluster_info->macro_type == current_macro_type)
                {
                    if (current_bank == DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit))
                    {
                        SHR_ERR_EXIT(_SHR_E_FULL,
                                     "The input to the outlif bank allocation algorithm resulted in assigning more banks than the system supports."
                                     "See comment in code for possible solutions.");
                    }

                    for (current_bank_in_cluster = 0; current_bank_in_cluster < nof_banks_per_cluster;
                         current_bank_in_cluster++, current_bank++)
                    {
                        nof_required_outlif_banks[current_physical_phase_pair]++;
                        total_outlif_banks++;
                    }
                }
            }
        }
    }

exit:
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
    int starting_no_ll_outlif_bank,
    dbal_physical_tables_e physical_table,
    int *first_outlif_bank,
    int *last_outlif_bank,
    int *first_no_ll_outlif_bank,
    int *last_no_ll_outlif_bank,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters,
    uint8 optimize_cluster_outlif_banks,
    int nof_allowed_ll_outlif_banks)
{
    int current_cluster, current_bank_in_cluster, nof_banks_per_cluster, current_bank, current_no_ll_bank;
    uint32 nof_entries_in_cluster;
    mdb_cluster_alloc_info_t *current_cluster_info, *current_init_cluster;
    mdb_macro_types_e current_macro_type, first_macro_type, last_macro_type;
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS];
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
    current_no_ll_bank = starting_no_ll_outlif_bank;
    *first_outlif_bank = DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit);
    *first_no_ll_outlif_bank = DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit);

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
        current_no_ll_bank = UTILEX_ALIGN_UP(current_no_ll_bank, nof_banks_per_cluster);

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

                if (DNX_ALGO_OUTLIF_IS_POINTER_BIGGER_THAN_EEDB_RESOURCE)
                {
                    if (optimize_cluster_outlif_banks)
                    {
                        if ((current_bank - starting_outlif_bank + nof_banks_per_cluster) > nof_allowed_ll_outlif_banks)
                        {
                            if (current_no_ll_bank < *first_no_ll_outlif_bank)
                            {
                                /*
                                 * Update the first outlif bank if it comes before the current one.
                                 */
                                *first_no_ll_outlif_bank = current_no_ll_bank;
                            }

                            current_init_cluster->logical_start_address =
                                current_no_ll_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);

                            for (current_bank_in_cluster = 0; current_bank_in_cluster < nof_banks_per_cluster;
                                 current_bank_in_cluster++, current_no_ll_bank++)
                            {
                                /*
                                 * Set each bank as assigned, and that it's using MDB clusters.
                                 */
                                SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                                                assigned.set(unit, current_no_ll_bank,
                                                             DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB));
                            }
                            (*nof_init_clusters)++;
                            continue;
                        }
                    }
                }

                current_init_cluster->logical_start_address = current_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit);

                for (current_bank_in_cluster = 0; current_bank_in_cluster < nof_banks_per_cluster;
                     current_bank_in_cluster++, current_bank++)
                {
                    /*
                     * Set each bank as assigned, and that it's using MDB clusters.
                     */
                    SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.set(unit, current_bank,
                                                                                    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB));
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

    if (*first_no_ll_outlif_bank == DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit))
    {
        /*
         * This phase doesn't have any MDB clusters assigned to it. Set its first bank to
         * be greater than its last bank, so in practice it will have no banks.
         */
        *first_no_ll_outlif_bank = 1;
        *last_no_ll_outlif_bank = 0;
    }
    else
    {
        *last_no_ll_outlif_bank = current_no_ll_bank - 1;
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
    int current_no_ll_outlif_bank,
    int *next_outlif_bank,
    int *next_no_ll_outlif_bank,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters,
    uint8 optimize_cluster_outlif_banks,
    int nof_allowed_ll_outlif_banks)
{
    int first_physical_phase;
    dbal_physical_tables_e first_physical_table_in_pair;
    int first_outlif_bank = 0, last_outlif_bank = 0, first_no_ll_outlif_bank = 0, last_no_ll_outlif_bank = 0;
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
                    (unit, current_outlif_bank, current_no_ll_outlif_bank, first_physical_table_in_pair,
                     &first_outlif_bank, &last_outlif_bank, &first_no_ll_outlif_bank, &last_no_ll_outlif_bank,
                     init_clusters_info, nof_init_clusters, optimize_cluster_outlif_banks,
                     nof_allowed_ll_outlif_banks));

    /*
     * Now map the reverse, logical phase to banks.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_set_first_and_last_bank
                    (unit, first_physical_phase, first_outlif_bank, last_outlif_bank, first_no_ll_outlif_bank,
                     last_no_ll_outlif_bank));

    /**Update the next outlif bank only if eedb banks are actually used per this phase. Otherwise - last_outlif_bank is set to 0 to indicate that none are needed*/
    if (last_outlif_bank >= first_outlif_bank)
    {
        *next_outlif_bank = last_outlif_bank + 1;
    }

    if (last_no_ll_outlif_bank >= first_no_ll_outlif_bank)
    {
        *next_no_ll_outlif_bank = last_no_ll_outlif_bank + 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/* Sort the required outlif banks for cluster:
 * In case the demand for outlif banks exceeds 512k lifs, we want to split the outlif banks below 512k (which can support ll) in a fair manner.
 * - Each phase pair can receive 25% of those outlif banks
 * - In case one of the phase-pairs requires less than 25% of those banks - the following phase pair can use them
 * In this function, we calculate the above:
 * We first sort the required outlif banks from low to high.
 * Then - for each phase-pair that requires less outlif banks - we compensate the next phase pair and so on
 * nof_required_outlif_banks[] - is the input, required outlif banks per phase pair. It is also changed inside the function and is not valid afterwards
 * nof_allowed_ll_outlif_banks[] - is the output, number of allowed outlif banks per phase pair below 512k
 */
void
dnx_algo_local_outlif_sort_required_ll_outlif_banks(
    int unit,
    int nof_required_outlif_banks[NOF_EEDB_PHASE_PAIRS],
    int nof_allowed_ll_outlif_banks[NOF_EEDB_PHASE_PAIRS])
{
    int tmp_count =
        0, idx, idx2, pos, min, nof_phase_pair_required_outlif_banks, nof_phase_pair_required_outlif_banks_idx,
        current_physical_phase_pair;
    int fair_nof_outlif_banks_per_phase_pair = DNX_ALGO_LOCAL_OUTLIF_LL_CAPABLE_NOF_BANKS(unit) / NOF_EEDB_PHASE_PAIRS;
    int nof_required_outlif_banks_sorted[NOF_EEDB_PHASE_PAIRS] = { 0 };
    int nof_required_outlif_banks_idx_sorted[NOF_EEDB_PHASE_PAIRS] = { 0 };

    /*
     * Sort the required outlif banks array
     */
    for (idx = 0; idx < NOF_EEDB_PHASE_PAIRS; idx++)
    {
        pos = 0;
        min = nof_required_outlif_banks[pos];

        for (idx2 = 0; idx2 < NOF_EEDB_PHASE_PAIRS; idx2++)
        {
            if (((nof_required_outlif_banks[idx2] > 0) && (nof_required_outlif_banks[idx2] < min)) || (min == 0))
            {
                min = nof_required_outlif_banks[idx2];
                pos = idx2;
            }
        }
        nof_required_outlif_banks[pos] = 0;
        nof_required_outlif_banks_sorted[idx] = min;
        nof_required_outlif_banks_idx_sorted[idx] = pos;
    }

    /*
     * Calc the number of allowed outlif banks per phase below bank DNX_ALGO_LOCAL_OUTLIF_LL_CAPABLE_NOF_BANKS(unit) (below 512k)
     */
    for (current_physical_phase_pair = 0; current_physical_phase_pair < NOF_EEDB_PHASE_PAIRS;
         current_physical_phase_pair++)
    {
        nof_phase_pair_required_outlif_banks = nof_required_outlif_banks_sorted[current_physical_phase_pair];
        nof_phase_pair_required_outlif_banks_idx = nof_required_outlif_banks_idx_sorted[current_physical_phase_pair];
        if (nof_phase_pair_required_outlif_banks <= fair_nof_outlif_banks_per_phase_pair)
        {
            nof_allowed_ll_outlif_banks[nof_phase_pair_required_outlif_banks_idx] =
                nof_phase_pair_required_outlif_banks;
        }
        else if ((tmp_count + nof_phase_pair_required_outlif_banks) <=
                 ((current_physical_phase_pair + 1) * fair_nof_outlif_banks_per_phase_pair))
        {
            nof_allowed_ll_outlif_banks[nof_phase_pair_required_outlif_banks_idx] =
                nof_phase_pair_required_outlif_banks;
        }
        else
        {
            nof_allowed_ll_outlif_banks[nof_phase_pair_required_outlif_banks_idx] =
                ((current_physical_phase_pair + 1) * fair_nof_outlif_banks_per_phase_pair) - tmp_count;
        }
        tmp_count += nof_allowed_ll_outlif_banks[nof_phase_pair_required_outlif_banks_idx];
    }
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
    int current_eedb_data_bank, current_eedb_no_ll_data_bank, current_granularity;
    dnx_algo_local_outlif_logical_phase_e rif_logical_phase;
    uint8 rif_physical_phase;
    int current_physical_phase_pair, rif_phase_pair;
    int nof_rifs, nof_rif_banks;
    int nof_required_outlif_banks[NOF_EEDB_PHASE_PAIRS] = { 0 };
    int nof_allowed_ll_outlif_banks[NOF_EEDB_PHASE_PAIRS] = { 0 };
    int total_required_outlif_banks = 0;
    uint8 optimize_cluster_outlif_banks = FALSE;

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
     * In case we're working with 20 bits outlif pointer - we want to optimize the outlif banks position, in order not
     * to starve certain phases from using link list (above 512k lifs)
     */
    if (DNX_ALGO_OUTLIF_IS_POINTER_BIGGER_THAN_EEDB_RESOURCE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_calc_required_cluster_outlif_banks
                        (unit, init_clusters_info, nof_required_outlif_banks));
        for (current_physical_phase_pair = 0; current_physical_phase_pair < NOF_EEDB_PHASE_PAIRS;
             current_physical_phase_pair++)
        {
            total_required_outlif_banks += nof_required_outlif_banks[current_physical_phase_pair];
        }

        if (total_required_outlif_banks > DNX_ALGO_LOCAL_OUTLIF_LL_CAPABLE_NOF_BANKS(unit))
        {
            optimize_cluster_outlif_banks = TRUE;

            dnx_algo_local_outlif_sort_required_ll_outlif_banks(unit, nof_required_outlif_banks,
                                                                nof_allowed_ll_outlif_banks);
        }
    }

    /*
     * Start by assigning the physical phase containing the rifs to the first IDs.
     * because the rifs always hold the outlifs 0-nof_rifs.
     */
    rif_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    physical_phase.get(unit, rif_logical_phase, &rif_physical_phase));
    rif_phase_pair = EEDB_PHYSICAL_TABLE_PHYSICAL_PHASE_TO_PHASE_PAIR(rif_physical_phase);
    current_eedb_data_bank = 0;
    current_eedb_no_ll_data_bank = DNX_ALGO_LOCAL_OUTLIF_LL_CAPABLE_NOF_BANKS(unit);
    *nof_init_clusters = 0;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_bank_assign_range_to_eedb_phy_table
                    (unit, rif_phase_pair, current_eedb_data_bank, current_eedb_no_ll_data_bank,
                     &current_eedb_data_bank, &current_eedb_no_ll_data_bank, init_clusters_info, nof_init_clusters,
                     optimize_cluster_outlif_banks, nof_allowed_ll_outlif_banks[rif_phase_pair]));

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
                                (unit, current_physical_phase_pair, current_eedb_data_bank,
                                 current_eedb_no_ll_data_bank, &current_eedb_data_bank, &current_eedb_no_ll_data_bank,
                                 init_clusters_info, nof_init_clusters, optimize_cluster_outlif_banks,
                                 nof_allowed_ll_outlif_banks[current_physical_phase_pair]));
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
 * Get ll_indication enum id based on xml token string
 */
static shr_error_e
dnx_algo_local_outlif_ll_indication_get_by_name(
    int unit,
    char *ll_indicaiton_name_str,
    dnx_local_outlif_ll_indication_e * ll_indication)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (sal_strncasecmp(ll_indicaiton_name_str, "Mandatory", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        *ll_indication = DNX_LOCAL_OUTLIF_LL_ALWAYS;
    }
    else if (sal_strncasecmp(ll_indicaiton_name_str, "Optional", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        *ll_indication = DNX_LOCAL_OUTLIF_LL_OPTIONAL;
    }
    else if (sal_strncasecmp(ll_indicaiton_name_str, "Never", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        *ll_indication = DNX_LOCAL_OUTLIF_LL_NEVER;
    }
    else if (sal_strncasecmp(ll_indicaiton_name_str, "NA", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        *ll_indication = DNX_LOCAL_OUTLIF_LL_OPTIONAL;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized LL_Allocation token: %s", ll_indicaiton_name_str);
    }

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
dnx_algo_local_outlif_phases_info_init(
    int unit)
{
    char etps_to_phases_xml_path[RHFILE_MAX_SIZE];
    void *curTop = NULL, *curPhase;
    char *image_name = NULL;
    uint8 success;
    uint8 is_standard_image = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    if (is_standard_image)
    {
        sal_strncpy(etps_to_phases_xml_path, "mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    }
    else
    {
        sal_strncpy(etps_to_phases_xml_path, "bare_metal/mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    }
    sal_strncat_s(etps_to_phases_xml_path, image_name, sizeof(etps_to_phases_xml_path));
    sal_strncat_s(etps_to_phases_xml_path, "/", sizeof(etps_to_phases_xml_path));
    sal_strncat_s(etps_to_phases_xml_path, image_name, sizeof(etps_to_phases_xml_path));
    sal_strncat_s(etps_to_phases_xml_path, "_etps_in_eedb_phases.xml", sizeof(etps_to_phases_xml_path));

    /*
     * Read XMl
     */
    if (is_standard_image)
    {
        curTop =
            dbx_pre_compiled_lif_phases_top_get(unit, etps_to_phases_xml_path, "PhasesEedbCatalog",
                                                CONF_OPEN_PER_DEVICE);
    }
    else
    {
        curTop = dbx_pre_compiled_lif_phases_top_get(unit, etps_to_phases_xml_path, "PhasesEedbCatalog", 0);
    }
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
        int nof_mappings = 0;
        char phase_name[10];
        void *curElement;
        sw_state_htbl_init_info_t hash_tbl_init_info;

        RHDATA_GET_XSTR_STOP(curPhase, "Name", phase_name, 10);
        RHDATA_GET_INT_STOP(curPhase, "NumOfMappings", nof_mappings);

        /*
         * Assuming phase names are EEDB_A -> EEDB_H,
         * the mapping to logical phase is done by taking the 6th char and reduce 'A'
         */
        logical_phase = phase_name[5] - 'A';

        sal_memset(&hash_tbl_init_info, 0, sizeof(sw_state_htbl_init_info_t));

        hash_tbl_init_info.max_nof_elements = nof_mappings;
        hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;
        hash_tbl_init_info.print_cb_name = "dnx_algo_lif_local_outlif_valid_combinations_print_cb";

        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        dbal_valid_combinations.create(unit, logical_phase, &hash_tbl_init_info));

        RHDATA_ITERATOR(curElement, curPhase, "Element")
        {
            /*
             * Add all elements in XML to SW state.
             * Translate strings to DBAL IDs
             */
            char dbal_table_name[DBAL_MAX_STRING_LENGTH], dbal_result_type_name[DBAL_MAX_STRING_LENGTH],
                ll_indication_name[DBAL_MAX_STRING_LENGTH] = { 0 };
            dbal_tables_e dbal_table_id;
            int is_table_active;
            int result_type;
            dbal_to_phase_info_t dbal_to_phase_info;
            dnx_local_outlif_ll_indication_e ll_indication = DNX_LOCAL_OUTLIF_LL_OPTIONAL;

            RHDATA_GET_XSTR_STOP(curElement, "AppDb", dbal_table_name, DBAL_MAX_STRING_LENGTH);
            RHDATA_GET_XSTR_STOP(curElement, "ResultType", dbal_result_type_name, DBAL_MAX_STRING_LENGTH);
            RHDATA_GET_XSTR_STOP(curElement, "LL_Allocation", ll_indication_name, DBAL_MAX_STRING_LENGTH);

            SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, dbal_table_name, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, dbal_table_id, &is_table_active));
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_ll_indication_get_by_name(unit, ll_indication_name, &ll_indication));
            if (!is_table_active)
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
            sal_memset(&dbal_to_phase_info, 0, sizeof(dbal_to_phase_info));
            dbal_to_phase_info.dbal_result_type = result_type;
            dbal_to_phase_info.dbal_table = dbal_table_id;
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            dbal_valid_combinations.insert(unit, logical_phase, &dbal_to_phase_info, &ll_indication,
                                                           &success));
            if (success == 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Internal error - failed to add phase combinations entry to hash table: logical phase %d, dbal table %s, result type %s",
                             logical_phase, dbal_table_name, dbal_result_type_name);
            }
        }
    }

exit:
    dbx_xml_top_close(curTop);
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

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phases_info_init(unit));

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
        rif_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;

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

            /** Fix COVERITY */
            if (address_granularity == 0)
            {
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
            nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
            nof_eedb_banks_per_outlif_bank =
                DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) /
                (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows * nof_outlifs_per_entry);

            /*
             * Verify that we have enough EEDB banks available to store all the rifs.
             */
            nof_required_eedb_banks = (nof_rif_banks - nof_rif_banks_in_mdb_cluster) * nof_eedb_banks_per_outlif_bank;

            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_verify_ll_availability
                            (unit, rif_logical_phase, nof_required_eedb_banks, &is_available));

            if (!is_available)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
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
            /*
             * Assign the outlif banks to use the rif logical phase and LL.
             */
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
    logical_phase = outlif_info->logical_phase;
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

    /** Fix COVERITY */
    if (nof_outlifs_per_entry == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
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
    int nof_glem_lines;
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
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_glem_lines_get(unit, &nof_glem_lines));
    outlif_stats->nof_glem_entries = nof_glem_lines;

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

            /** Fix COVERITY */
            if (address_granularity == 0)
            {
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
            nof_outlifs_per_eedb_bank =
                EEDB_MAX_DATA_ENTRY_SIZE / address_granularity * dnx_data_mdb.dh.macro_type_info_get(unit,
                                                                                                     MDB_EEDB_BANK)->nof_rows;

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
dnx_algo_local_outlif_eedb_data_banks_disable_set(
    int unit,
    uint8 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(local_outlif_info.disable_eedb_data_banks.set(unit, value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_outlif_rif_init_info_get(
    int unit,
    dnx_algo_lif_local_outlif_resource_extra_info_t * init_info)
{
    dnx_algo_local_outlif_logical_phase_e rif_logical_phase;
    uint8 phase_address_granularity;
    SHR_FUNC_INIT_VARS(unit);

    rif_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    address_granularity.get(unit, rif_logical_phase, &phase_address_granularity));

    init_info->granularity = phase_address_granularity;

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
