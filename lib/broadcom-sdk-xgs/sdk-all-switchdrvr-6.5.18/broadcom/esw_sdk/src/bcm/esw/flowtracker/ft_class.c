/*
 * $Id: trident2.c,v 1.298 2017/05/18 vashisht Exp $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        ft_class.c
 * Purpose:     The purpose of this file is to configure
 *              Flowtracker class related methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_class.h>
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)

typedef struct bcmi_ft_agg_class_source_type_map_s {
    uint32 amft_hw_val;

    uint32 aeft_hw_val;
} bcmi_ft_agg_class_source_type_map_t;

typedef struct bcmi_ft_aggregate_class_state_s {
    /* Bitmap for class ids */
    SHR_BITDCL *bitmap[BCMI_FT_GROUP_TYPE_COUNT];

    int *ref_count_array;

    /* Class Source type mapping */
    bcmi_ft_agg_class_source_type_map_t *map;

} bcmi_ft_aggregate_class_state_t;

bcmi_ft_aggregate_class_state_t
    bcmi_ft_aggregate_class_state[BCM_MAX_NUM_UNITS];

#define BCMI_FT_AGG_CLASS_BITMAP(_unit_, _group_type_)                          \
    bcmi_ft_aggregate_class_state[(_unit_)].bitmap[(_group_type_)]

#define BCMI_FT_AGG_CLASS_REF_COUNT(_unit_, _idx_)                              \
    bcmi_ft_aggregate_class_state[(_unit_)].ref_count_array[(_idx_)]

#define BCMI_FT_AGG_CLASS_ST_MAP(_unit_)                                        \
    bcmi_ft_aggregate_class_state[(_unit_)].map

#define BCMI_FT_AGG_CLASS_ST_MAP_HW_VAL(_unit_, _source_type_, aft)             \
    (bcmi_ft_aggregate_class_state[(_unit_)].map)[(_source_type_)].aft ##_hw_val

#define BCMI_FT_AGG_CLASS_ID_MIN 1
#define BCMI_FT_AGG_CLASS_ID_MAX 63
#define BCMI_FT_AGG_CLASS_ST_INVALID (-1)

bcmi_ft_agg_class_source_type_map_t bcmi_ftv3_agg_class_st_map[] =  {
    /* {AMFT, AEFT}                         Enum */
    {  0,  0},                           /* bcmFlowtrackerClassSourceTypeNone */
    {  1,  8},                           /* bcmFlowtrackerClassSourceTypePort */
    {  2, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeSvp */
    {  3, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeL3Interface */
    {  4, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeFieldSrc  */
    {  5, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeFieldDst */
    {  6, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeL2Src */
    {  7, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeL2Dst */
    {  8, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeL3Src */
    {  9, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeL3Dst */
    { 10, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeVlan */
    { 11, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeVrf */
    { 12, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeFieldExactMatchLookUp0 */
    { 13, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeFieldExactMatchLookUp1 */
    { 15, BCMI_FT_AGG_CLASS_ST_INVALID}, /* bcmFlowtrackerClassSourceTypeFieldIngress */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  1}, /* bcmFlowtrackerClassSourceTypeI2E */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  2}, /* bcmFlowtrackerClassSourceTypeNextHopA  */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  3}, /* bcmFlowtrackerClassSourceTypeNextHopB */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  4}, /* bcmFlowtrackerClassSourceTypeL3InterfaceA */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  5}, /* bcmFlowtrackerClassSourceTypeL3InterfaceB */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  6}, /* bcmFlowtrackerClassSourceTypeDstVp */
    { BCMI_FT_AGG_CLASS_ST_INVALID,  7}, /* bcmFlowtrackerClassSourceTypeIpTunnel */
    { BCMI_FT_AGG_CLASS_ST_INVALID, 10}, /* bcmFlowtrackerClassSourceTypeXlateNextHopA */
    { BCMI_FT_AGG_CLASS_ST_INVALID, 11}, /* bcmFlowtrackerClassSourceTypeXlateNextHopB */
    { BCMI_FT_AGG_CLASS_ST_INVALID, 12}, /* bcmFlowtrackerClassSourceTypeXlateL3InterfaceA */
    { BCMI_FT_AGG_CLASS_ST_INVALID, 13}, /* bcmFlowtrackerClassSourceTypeXlateL3InterfaceB */
    { BCMI_FT_AGG_CLASS_ST_INVALID, 14}, /* bcmFlowtrackerClassSourceTypeXlateDstVp */
    { BCMI_FT_AGG_CLASS_ST_INVALID, 15}, /* bcmFlowtrackerClassSourceTypeXlateIpTunnel */
    { BCMI_FT_AGG_CLASS_ST_INVALID, BCMI_FT_AGG_CLASS_ST_INVALID}  /* bcmFlowtrackerClassSourceTypeCount */
};

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_init
 * Purpose:
 *   Initialize Flowtracker aggregate class related data structs.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ftv3_aggregate_class_init(int unit)
{
    int rv = BCM_E_NONE;
    int num_entries;
    bcm_flowtracker_group_type_t group_type;

    sal_memset(&bcmi_ft_aggregate_class_state[unit],
        0, sizeof (bcmi_ft_aggregate_class_state_t));

    num_entries = soc_mem_index_count(unit, AMFT_CLASSID_PROFILEm);

    /* Bitmap for Aggregate Mmu Flowtracker */
    group_type = bcmFlowtrackerGroupTypeAggregateMmu;
    BCMI_FT_ALLOC_IF_ERR_CLEANUP(
        BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
        SHR_BITALLOCSIZE(num_entries), "amft agg class bitmap");

    /* Bitmap for Aggregate Egress Flowtracker */
    group_type = bcmFlowtrackerGroupTypeAggregateEgress;
    BCMI_FT_ALLOC_IF_ERR_CLEANUP(
        BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
        SHR_BITALLOCSIZE(num_entries), "aeft agg class bitmap");

    /* Allocate ref count array */
    BCMI_FT_ALLOC_IF_ERR_CLEANUP(
        bcmi_ft_aggregate_class_state[unit].ref_count_array,
        num_entries, " agg class ref count");

    /* Enum to hw values mapping */
    BCMI_FT_AGG_CLASS_ST_MAP(unit) = bcmi_ftv3_agg_class_st_map;

cleanup:

    if (BCM_FAILURE(rv)) {
        bcmi_ftv3_aggregate_class_cleanup(unit);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_cleanup
 * Purpose:
 *   Clear Flowtracker aggregate clas related data structs.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ftv3_aggregate_class_cleanup(int unit)
{
    bcm_flowtracker_group_type_t group_type;

    /* Cleanup bitmap for Aggregate Mmu Flowtracker */
    group_type = bcmFlowtrackerGroupTypeAggregateMmu;
    if (BCMI_FT_AGG_CLASS_BITMAP(unit, group_type) != NULL) {
        sal_free(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type));
        BCMI_FT_AGG_CLASS_BITMAP(unit, group_type) = NULL;
    }

    /* Cleanup bitmap for Aggregate Egress Flowtracker */
    group_type = bcmFlowtrackerGroupTypeAggregateEgress;
    if (BCMI_FT_AGG_CLASS_BITMAP(unit, group_type) != NULL) {
        sal_free(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type));
        BCMI_FT_AGG_CLASS_BITMAP(unit, group_type) = NULL;
    }

    if (bcmi_ft_aggregate_class_state[unit].ref_count_array != NULL) {
        sal_free(bcmi_ft_aggregate_class_state[unit].ref_count_array);
        bcmi_ft_aggregate_class_state[unit].ref_count_array = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_source_type_hw_val_get
 * Purpose:
 *   Get hardware value for given class source type
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   source_type    - (IN) Class Source Type
 *   hw_source_type - (OUT) Hw value for given source type
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_source_type_hw_val_get(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_class_source_type_t source_type,
    uint32 *hw_source_type)
{
    if (hw_source_type == NULL) {
        return BCM_E_PARAM;
    }

    if (group_type == bcmFlowtrackerGroupTypeAggregateMmu) {
        *hw_source_type =
            BCMI_FT_AGG_CLASS_ST_MAP_HW_VAL(unit, source_type, amft);
    } else {
        *hw_source_type =
            BCMI_FT_AGG_CLASS_ST_MAP_HW_VAL(unit, source_type, aeft);
    }

    if (*hw_source_type == BCMI_FT_AGG_CLASS_ST_INVALID) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_source_type_val_get
 * Purpose:
 *   Get enum value for given class source type
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   hw_source_type - (IN) Hw value for source type
 *   source_type    - (OUT) Corresponding Class Source Type
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_source_type_val_get(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    uint32 hw_source_type,
    bcm_flowtracker_class_source_type_t *source_type)
{
    uint32 tmp_hw_value;

    if (source_type == NULL) {
        return BCM_E_PARAM;
    }

    for (*source_type = bcmFlowtrackerClassSourceTypeNone;
            *source_type < bcmFlowtrackerClassSourceTypeCount;
            (*source_type)++) {

        if (group_type == bcmFlowtrackerGroupTypeAggregateMmu) {
            tmp_hw_value =
                BCMI_FT_AGG_CLASS_ST_MAP_HW_VAL(unit, *source_type, amft);
        } else {
            tmp_hw_value =
                BCMI_FT_AGG_CLASS_ST_MAP_HW_VAL(unit, *source_type, aeft);
        }

        if (hw_source_type == tmp_hw_value) {
            break;
        }
    }

    if (*source_type == bcmFlowtrackerClassSourceTypeCount) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_is_invalid
 * Purpose:
 *   Validate aggregate class
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   aggregate_class- (IN) Flowtracker Aggregate Class
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_is_invalid(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    uint32 aggregate_class)
{
    int in_use = FALSE;

    if ((aggregate_class < BCMI_FT_AGG_CLASS_ID_MIN) ||
        (aggregate_class > BCMI_FT_AGG_CLASS_ID_MAX)) {
        return BCM_E_PARAM;
    }

    in_use = SHR_BITGET
        (BCMI_FT_AGG_CLASS_BITMAP(unit, group_type), aggregate_class);

    if (in_use == FALSE) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_idx_reserve
 * Purpose:
 *   Reserve given index for aggregate class
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   aggregate_class- (IN) Flowtracker Aggregate Class
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_idx_reserve(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    uint32 aggregate_class)
{
    int in_use = FALSE;

    if ((aggregate_class < BCMI_FT_AGG_CLASS_ID_MIN) ||
            (aggregate_class > BCMI_FT_AGG_CLASS_ID_MAX)) {
        return BCM_E_PARAM;
    }

    in_use = SHR_BITGET(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
            aggregate_class);

    if (in_use != FALSE) {
        return BCM_E_EXISTS;
    }

    SHR_BITSET(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
            aggregate_class);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_free_idx_get
 * Purpose:
 *   Get Free index for aggregate class
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   aggregate_class- (OUT) Flowtracker Aggregate Class
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_free_idx_get(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    uint32 *aggregate_class)
{
    uint32 idx;
    int amft_in_use = FALSE, aeft_in_use = FALSE;
    bcm_flowtracker_group_type_t gt;

    for (idx = BCMI_FT_AGG_CLASS_ID_MIN;
            idx <= BCMI_FT_AGG_CLASS_ID_MAX; idx++) {

        gt = bcmFlowtrackerGroupTypeAggregateMmu;
        amft_in_use = SHR_BITGET(BCMI_FT_AGG_CLASS_BITMAP(unit, gt), idx);

        gt = bcmFlowtrackerGroupTypeAggregateEgress;
        aeft_in_use = SHR_BITGET(BCMI_FT_AGG_CLASS_BITMAP(unit, gt), idx);

        if ((amft_in_use == FALSE) && (aeft_in_use == FALSE)) {

            SHR_BITSET(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type), idx);
            *aggregate_class = idx;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_idx_free
 * Purpose:
 *   Free aggregate class index
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   aggregate_class- (IN) Flowtracker Aggregate Class
 * Returns:
 *   BCM_E_xxx
 */

int
bcmi_ftv3_aggregate_class_idx_free(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    uint32 aggregate_class)
{
    SHR_BITCLR(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
            aggregate_class);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_create
 * Purpose:
 *   Create flowtracker aggregate class.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   options        - (IN) Options
 *   group_type     - (IN) Flowtracker Group Type
 *   aggregate_class_info - (IN) Aggregate Class info Struct
 *   aggregate_class - (IN/OUT) Flowtracker Aggregate Class
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info,
    uint32 *aggregate_class)
{
    int rv = BCM_E_NONE;
    void *entry = NULL;
    soc_mem_t mem = INVALIDm;
    uint32 hw_source_type1 = 0, hw_source_type2 = 0;
    amft_classid_profile_entry_t amft_classid_profile_entry;
    aeft_classid_profile_entry_t aeft_classid_profile_entry;

    sal_memset(&amft_classid_profile_entry, 0,
            sizeof(amft_classid_profile_entry_t));
    sal_memset(&aeft_classid_profile_entry, 0,
            sizeof(aeft_classid_profile_entry_t));

    if (!ft_initialized[unit]) {
        return BCM_E_INIT;
    }

    if ((aggregate_class == NULL) || (aggregate_class_info == NULL)) {
        return BCM_E_PARAM;
    }

    /* Check if given Group Type are allowed */
    if ((group_type != bcmFlowtrackerGroupTypeAggregateMmu) &&
        (group_type != bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_PARAM;
    }

    /* Get hw value for given class source types */
    rv = bcmi_ftv3_aggregate_class_source_type_hw_val_get(unit, group_type,
            aggregate_class_info->source_type1, &hw_source_type1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ftv3_aggregate_class_source_type_hw_val_get(unit, group_type,
            aggregate_class_info->source_type2, &hw_source_type2);
    BCM_IF_ERROR_RETURN(rv);

    /* Get mem */
    switch(group_type) {
        case bcmFlowtrackerGroupTypeAggregateMmu:
            mem = AMFT_CLASSID_PROFILEm;
            entry = &amft_classid_profile_entry;
            break;
        case bcmFlowtrackerGroupTypeAggregateEgress:
        default:
            mem = AEFT_CLASSID_PROFILEm;
            entry = &aeft_classid_profile_entry;
            break;
    }

    if (options & BCM_FLOWTRACKER_AGGREGATE_CLASS_WITH_ID) {

        /* Reserve aggregate class */
        rv = bcmi_ftv3_aggregate_class_idx_reserve(unit,
                    group_type, *aggregate_class);
    } else {

        /* Get Free index for aggregate class */
        *aggregate_class = 0;
        rv = bcmi_ftv3_aggregate_class_free_idx_get(unit,
                group_type, aggregate_class);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Fill up entry */
    soc_mem_field32_set(unit, mem, entry, CLASS_ID1f, hw_source_type1);
    soc_mem_field32_set(unit, mem, entry, CLASS_ID2f, hw_source_type2);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, *aggregate_class, entry);
    if (BCM_FAILURE(rv)) {

        /* Free aggregate class index */
        (void) bcmi_ftv3_aggregate_class_idx_free(unit,
                    group_type, *aggregate_class);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_get
 * Purpose:
 *   Get flowtracker aggregate class info.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   aggregate_class- (IN) Flowtracker Aggregate Class
 *   group_type     - (IN) Flowtracker Group Type
 *   aggregate_class_info - (OUT) Aggregate Class info Struct
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_get(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info)
{
    int rv = BCM_E_NONE;
    void *entry = NULL;
    soc_mem_t mem = INVALIDm;
    uint32 hw_source_type1 = 0, hw_source_type2 = 0;
    amft_classid_profile_entry_t amft_classid_profile_entry;
    aeft_classid_profile_entry_t aeft_classid_profile_entry;

    sal_memset(&amft_classid_profile_entry, 0,
            sizeof(amft_classid_profile_entry_t));
    sal_memset(&aeft_classid_profile_entry, 0,
            sizeof(aeft_classid_profile_entry_t));

    if (aggregate_class_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Check if given Group Type are allowed */
    if ((group_type != bcmFlowtrackerGroupTypeAggregateMmu) &&
        (group_type != bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_PARAM;
    }

    rv = bcmi_ftv3_aggregate_class_is_invalid(unit,
            group_type, aggregate_class);
    BCM_IF_ERROR_RETURN(rv);

    /* Get mem */
    switch(group_type) {
        case bcmFlowtrackerGroupTypeAggregateMmu:
            mem = AMFT_CLASSID_PROFILEm;
            entry = &amft_classid_profile_entry;
            break;
        case bcmFlowtrackerGroupTypeAggregateEgress:
        default:
            mem = AEFT_CLASSID_PROFILEm;
            entry = &aeft_classid_profile_entry;
            break;
    }

    /* read entry */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, mem,
                MEM_BLOCK_ANY, aggregate_class, entry));

    hw_source_type1 = soc_mem_field32_get(unit, mem, entry, CLASS_ID1f);
    hw_source_type2 = soc_mem_field32_get(unit, mem, entry, CLASS_ID2f);

    rv = bcmi_ftv3_aggregate_class_source_type_val_get(unit, group_type,
            hw_source_type1, &aggregate_class_info->source_type1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ftv3_aggregate_class_source_type_val_get(unit, group_type,
            hw_source_type2, &aggregate_class_info->source_type2);

    return rv;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_get_all
 * Purpose:
 *   Get all flowtracker aggregate class configured for given
 *   flowtracker group type in the system.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   group_type     - (IN) Flowtracker Group Type
 *   size           - (IN) Size of aggregate_class_array
 *   aggregate_class_array- (OUT) Flowtracker Aggregate Class array.
 *   actual_size    - (OUT) Actual number in Aggregate Class array.
 * Returns:
 *   BCM_E_xxx
 */

int
bcmi_ftv3_aggregate_class_get_all(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    int size,
    uint32 *aggregate_class_array,
    int *actual_size)
{
    int idx, j, in_use;

    /* Check if given Group Type are allowed */
    if ((group_type != bcmFlowtrackerGroupTypeAggregateMmu) &&
        (group_type != bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_PARAM;
    }

    if ((size == 0) && (actual_size == NULL)) {
        return BCM_E_PARAM;
    }

    if ((size != 0) && (aggregate_class_array == NULL)) {
        return BCM_E_PARAM;
    }

    for (j = 0, idx = BCMI_FT_AGG_CLASS_ID_MIN;
            (idx <= BCMI_FT_AGG_CLASS_ID_MAX); idx++) {

        in_use = SHR_BITGET(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type), idx);
        if (in_use != FALSE) {
            if (aggregate_class_array != NULL) {
                aggregate_class_array[j] = idx;
            }
            j++;
            if ((size != 0) && (j >= size)) {
                break;
            }
        }
    }

    if (actual_size != NULL) {
        *actual_size = j;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *   bcmi_ftv3_aggregate_class_destroy
 * Purpose:
 *   Destroy flowtracker aggregate class.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   aggregate_class- (IN) Flowtracker Aggregate Class
 *   group_type     - (IN) Flowtracker Group Type
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_destroy(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type)
{
    int rv = BCM_E_NONE;
    void *entry = NULL;
    soc_mem_t mem = INVALIDm;
    amft_classid_profile_entry_t amft_classid_profile_entry;
    aeft_classid_profile_entry_t aeft_classid_profile_entry;

    /* Check if given Group Type are allowed */
    if ((group_type != bcmFlowtrackerGroupTypeAggregateMmu) &&
        (group_type != bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_PARAM;
    }

    rv = bcmi_ftv3_aggregate_class_is_invalid(unit,
            group_type, aggregate_class);
    BCM_IF_ERROR_RETURN(rv);

    if (BCMI_FT_AGG_CLASS_REF_COUNT(unit, aggregate_class)) {
        return BCM_E_BUSY;
    }

    sal_memset(&amft_classid_profile_entry, 0,
            sizeof(amft_classid_profile_entry_t));
    sal_memset(&aeft_classid_profile_entry, 0,
            sizeof(aeft_classid_profile_entry_t));
    /* Get mem */
    switch(group_type) {
        case bcmFlowtrackerGroupTypeAggregateMmu:
            mem = AMFT_CLASSID_PROFILEm;
            entry = &amft_classid_profile_entry;
            break;
        case bcmFlowtrackerGroupTypeAggregateEgress:
        default:
            mem = AEFT_CLASSID_PROFILEm;
            entry = &aeft_classid_profile_entry;
            break;
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
            aggregate_class, entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Reset bitmap */
    rv = bcmi_ftv3_aggregate_class_idx_free(unit,
            group_type, aggregate_class);

    return rv;
}

/*
 * Function:
 *   bcmi_ftv3_aggregate_class_entry_add_check
 * Purpose:
 *   Verify aggregate class before adding aggregate
 *   class to field entry.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   aggregate_class- (IN) Flowtracker Aggregate Class
 *   group_type     - (IN) Flowtracker Group Type
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ftv3_aggregate_class_entry_add_check(
    int unit,
    uint32 aggregate_class)
{
    int amft_in_use = FALSE;
    int aeft_in_use = FALSE;
    bcm_flowtracker_group_type_t gt;

    if ((aggregate_class < BCMI_FT_AGG_CLASS_ID_MIN) ||
            (aggregate_class > BCMI_FT_AGG_CLASS_ID_MAX)) {
        return BCM_E_NONE;
    }

    gt = bcmFlowtrackerGroupTypeAggregateMmu;
    amft_in_use = SHR_BITGET(BCMI_FT_AGG_CLASS_BITMAP(unit, gt),
            aggregate_class);

    gt = bcmFlowtrackerGroupTypeAggregateEgress;
    aeft_in_use = SHR_BITGET(BCMI_FT_AGG_CLASS_BITMAP(unit, gt),
            aggregate_class);

    if ((amft_in_use == FALSE) && (aeft_in_use == FALSE)) {
        /* Aggregate Class not configured */
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

int
bcmi_ftv3_aggregate_class_ref_update(int unit,
    uint32 aggregate_class,
    int count)
{
    int ref_count;

    ref_count = BCMI_FT_AGG_CLASS_REF_COUNT(unit, aggregate_class);
    ref_count += count;

    if (ref_count < 0) {
        BCMI_FT_AGG_CLASS_REF_COUNT(unit, aggregate_class) = 0;
    } else {
        BCMI_FT_AGG_CLASS_REF_COUNT(unit, aggregate_class) = ref_count;
    }

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRCAKER_V3_SUPPORT*/
int bcmi_ft_class_not_empty;
#endif /* BCM_FLOWTRCAKER_V3_SUPPORT */
