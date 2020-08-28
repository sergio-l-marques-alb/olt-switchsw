/** \file sw_state_linked_list.c
 *
 * A linked list implementation that is used through sw_state
 * interface
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <assert.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/drv.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#define SW_STATE_LL_CHECK_VALID_NODE(param) \
    do { \
        if ((param) == SW_STATE_LL_INVALID) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: SW_STATE_LL_INVALID argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)

#define SW_STATE_LL_CHECK_NULL_PARAMETER(param) \
    do { \
        if ((param) == NULL) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: NULL argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)

/* verify that the ll_index is not out of bounds */
/* allow it to go +/-1 out of bound due to internal index translation */
#define SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index) \
    do { \
        if (ll_index > ll->nof_heads && ll_index != -1) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: accessed multihead linked list out of bound. index=%d nof_heads=%d\n%s", ll_index, ll->nof_heads, EMPTY); \
        } \
    } while(0)

#define TRANSLATE_LL_IDX(index)           (index+1) /* used to translate user index to internal index */
#define REVERSE_TRANSLATE_LL_IDX(index)   (index-1) /* reverse translation to be used when internal implementation calls
                                                       sw_state_ll APIs that expect user index */
#define KEY_AT_IDX(idx)                   ((uint8 *) ll->keys + (ll->key_size * idx))
#define VALUE_AT_IDX(idx)                 ((uint8 *) ll->values + (ll->value_size * idx))

#define SW_STATE_LL_SET_HEAD(ll_index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->head[ll_index]), &value, sizeof(sw_state_ll_node_t), 0x0, "head")

#define SW_STATE_LL_SET_TAIL(ll_index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->tail[ll_index]), &value, sizeof(sw_state_ll_node_t), 0x0, "tail")

#define SW_STATE_LL_SET_NEXT(index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->nexts[index]), &value, sizeof(sw_state_ll_node_t), 0x0, "next")

#define SW_STATE_LL_SET_PREV(index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->prevs[index]), &value, sizeof(sw_state_ll_node_t), 0x0, "prev")

const uint32 sw_state_ll_invalid = SW_STATE_LL_INVALID;

/**
* \brief
*  The function is an internal function that will create a new
*  node
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id.
* \param [in] prev  - the previous node, can be NULL.
* \param [in] next  - the next node, can be NULL. 
* \param [out] created_node  - the created node will be put 
*        here.
* \param [in] key    - the content of the new node's key
* \param [in] value    - the content of the new node's value
* \param [in] ll  - linked list to operate on 
* \param [in] ll_index  - linked list id to operate on in case 
*        of a multihead ll.* \return
*   location - the new node's address or NULL if error
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
STATIC int sw_state_ll_create_new_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *created_node, 
                                const void *key, const void *value, sw_state_ll_node_t prev, sw_state_ll_node_t next) {

    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    uint8 is_suppressed = FALSE;
    
    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    if (ll->is_sorted) {
        SW_STATE_CB_DB_GET_CB(
            module_id,
            ll->key_cmp_cb,
            &sorted_list_cmp_cb,
            dnx_sw_state_sorted_list_cb_get_cb
        );
    }

    assert(ll != NULL);
    assert(created_node != NULL);

    /* check if there is unused node available */
    if (ll->head[0] == SW_STATE_LL_INVALID) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state ll ERROR: ll is full.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /* take the first free node */
    (*created_node) = ll->head[0];
    SW_STATE_LL_SET_HEAD(0,ll->nexts[ll->head[0]]);

    if (ll->head[0] != SW_STATE_LL_INVALID) {
        SW_STATE_LL_SET_PREV(ll->head[0], sw_state_ll_invalid);
    }
    SW_STATE_LL_SET_PREV((*created_node), sw_state_ll_invalid);
    SW_STATE_LL_SET_NEXT((*created_node), sw_state_ll_invalid);


    /* set the key if sorted list */
    if (key != NULL) {
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, KEY_AT_IDX(*created_node), key, ll->key_size, 0x0, "sw state ll node's key");
    }

    /* set the value */
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, VALUE_AT_IDX(*created_node), value, ll->value_size, 0x0, "sw state ll node's value");

    SW_STATE_LL_SET_PREV((*created_node), prev);
    SW_STATE_LL_SET_NEXT((*created_node), next);

    if (ll->nexts[(*created_node)] != SW_STATE_LL_INVALID) {
        SW_STATE_LL_SET_PREV(ll->nexts[(*created_node)], *created_node);
    } else {
        SW_STATE_LL_SET_TAIL(ll_index, *created_node);
    }
    
    if (ll->prevs[(*created_node)] != SW_STATE_LL_INVALID) {
        SW_STATE_LL_SET_NEXT(ll->prevs[(*created_node)], *created_node);
    } else {
        SW_STATE_LL_SET_HEAD(ll_index, *created_node);
    }

    if (ll->is_sorted) {
        /* in case of sorted list, the key must be (strictly) bigger than prev->key and (strictly) smaller than next->key */
        if ((ll->nexts[(*created_node)] != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(*created_node), KEY_AT_IDX(ll->nexts[(*created_node)]), ll->key_size) >= 0)
            || (ll->prevs[(*created_node)] != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(*created_node), KEY_AT_IDX(ll->prevs[(*created_node)]), ll->key_size) <= 0)) {
                SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), *created_node));
                SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/**
* \brief
*  The function is an internal function that will handle
*  additions of a new node to the linked list Data Structure
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] node_to_add_after  - new node will be added after 
*        the supplied node, if NULL, it will be added at the
*        beginning of the list
* \param [in] key    - the content of the new node's key
* \param [in] value    - the content of the new node's value
* \param [in] is_first - 0 if not first, 1 if first
* \param [in] ll  - linked list to operate on 
* \param [in] ll_index  - linked list id to operate on in case 
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
STATIC int sw_state_ll_add_internal(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node_to_add_after, uint8 is_first, const void* key, const void* value) {

    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_ll_node_t created_node;
    uint8 is_suppressed = FALSE;
    
    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    /* get the comparison cb if sorted ll */
    if (ll->is_sorted) {
        SW_STATE_CB_DB_GET_CB(
            module_id,
            ll->key_cmp_cb,
            &sorted_list_cmp_cb,
            dnx_sw_state_sorted_list_cb_get_cb
        );
    }

    /* sanity checks */
    assert(ll != NULL);
    if (ll->is_sorted) {
        assert(key != NULL);
    }
    assert(value != NULL);

    /* add to an empty list */
    /* same for sorted and not sorted */
    if (ll->tail[ll_index] == SW_STATE_LL_INVALID) {

        /* sanity checks */
        assert(ll->head[ll_index] == SW_STATE_LL_INVALID);
        assert(node_to_add_after == SW_STATE_LL_INVALID);

        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID));

        SW_STATE_LL_SET_HEAD(ll_index, created_node);
        SW_STATE_LL_SET_TAIL(ll_index, created_node);

        SOC_EXIT;
    }

    /* if got node to add after */
    if (node_to_add_after != SW_STATE_LL_INVALID) {
        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, node_to_add_after, ll->nexts[node_to_add_after]));
        SOC_EXIT;
    }
    
    if (ll->is_sorted && !is_first) {
        sw_state_ll_node_t node = ll->head[ll_index];
        
        /* if only one element is the list */
        if (ll->nexts[node] == SW_STATE_LL_INVALID) {
            if (sorted_list_cmp_cb((KEY_AT_IDX(node)), (uint8*) key, ll->key_size) < 0) {
                SHR_IF_ERR_EXIT(sw_state_ll_add_last(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), key, value));
            } else {
                SHR_IF_ERR_EXIT(sw_state_ll_add_first(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), key, value));
            }
            SOC_EXIT;
        }
        while (ll->nexts[node] != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(ll->nexts[node]), (uint8*)key, ll->key_size) < 0) {
            node = ll->nexts[node];
        }
        if (node == ll->head[ll_index]) {
            SHR_IF_ERR_EXIT(sw_state_ll_add_first(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), key, value));
            SOC_EXIT;
        } else if (node == ll->tail[ll_index]) {
            SHR_IF_ERR_EXIT(sw_state_ll_add_last(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), key, value));
            SOC_EXIT;
        } else {
            SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, node, ll->nexts[node]));
            SOC_EXIT;
        }
    } else {
        /* if didn't get a node to add after, add at the beginnig of list */
        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, SW_STATE_LL_INVALID, ll->head[ll_index]));
        SW_STATE_LL_SET_HEAD(ll_index, created_node);
        SOC_EXIT;
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

int
sw_state_ll_create_empty_size_get(
    sw_state_ll_init_info_t *init_info,
    uint32 flags,
    uint32 key_size,
    uint32 value_size)
{
    int size = 0;
    uint32 elements_to_alloc;
    uint32 heads_to_alloc;

    elements_to_alloc = init_info->max_nof_elements + 1;
    heads_to_alloc = init_info->nof_heads + 1;

    /*
     * Add ll structure size.
     */
    size += (sizeof(sw_state_ll_t));

    /*
     * Add ll head size.
     */
    size += (sizeof(sw_state_ll_node_t)*heads_to_alloc);

    /*
     * Add ll tail size.
     */
    size += (sizeof(sw_state_ll_node_t)*heads_to_alloc);

    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED))
    {
        /*
         * Add ll keys size.
         */
        size += (sizeof(uint8)*(key_size * elements_to_alloc));
    }
    /*
     * Add ll values size.
     */
    size += (sizeof(uint8)*(value_size * elements_to_alloc));

    /*
     * Add ll prevs size.
     */
    size += (sizeof(sw_state_ll_node_t)*(elements_to_alloc));

    /*
     * Add ll nexts size.
     */
    size += (sizeof(sw_state_ll_node_t)*(elements_to_alloc));

    return size;
}

/*
 * see .h file for description
 */
int sw_state_ll_create_empty(int unit, uint32 module_id, sw_state_ll_init_info_t *init_info, uint32 flags, uint32 key_size, uint32 value_size, sw_state_ll_t * ll_ptr, uint32 max_nof_elements, uint32 alloc_flags) {

    uint32 elements_to_alloc;
    uint32 heads_to_alloc;
    uint32 index;
    sw_state_ll_t ll;
    uint32 node;
    uint8 is_sorted_ll;
    uint8 is_suppressed = FALSE;
    uint32 el_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(ll_ptr != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(init_info);

    if (*ll_ptr != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state ll create ERROR: ll already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /*
     * Replace the max_nof_elements and the sw state allocation flags,
     * if the nof_elements are specified in the relevant xml file
     */
    if (max_nof_elements != 0)
    {
        /* allocate extra element so we can use element 0 as invalid */
        elements_to_alloc = max_nof_elements + 1;
        el_alloc_flags = alloc_flags;
    }
    else
    {
        /* allocate extra element so we can use element 0 as invalid */
        elements_to_alloc = init_info->max_nof_elements + 1;
        el_alloc_flags = DNXC_SW_STATE_NO_FLAGS;
    }

    DNX_SW_STATE_ALLOC(unit, module_id, *ll_ptr, **ll_ptr, /*number of elements*/ 1, DNXC_SW_STATE_NO_FLAGS, "sw_state linked list");
    ll = *ll_ptr;

    /* if single head linked list - overide nof_heads with 1 */
    if (!sw_state_is_flag_on(flags, SW_STATE_LL_MULTIHEAD)) {
        init_info->nof_heads = 1;
    }

    /* allocate extra list so we can use list #0 to store free cells */
    heads_to_alloc = init_info->nof_heads + 1;

    /* allocate values, keys, prevs, nexts, head and tail arrays */
    DNX_SW_STATE_ALLOC(unit, module_id, ll->head, sw_state_ll_node_t, heads_to_alloc, DNXC_SW_STATE_NO_FLAGS, "ll heads");
    DNX_SW_STATE_ALLOC(unit, module_id, ll->tail, sw_state_ll_node_t, heads_to_alloc, DNXC_SW_STATE_NO_FLAGS, "ll tails");
    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED))
    {
        DNX_SW_STATE_ALLOC(unit, module_id, ll->keys, uint8, key_size * elements_to_alloc, el_alloc_flags, "ll keys");
    }
    DNX_SW_STATE_ALLOC(unit, module_id, ll->values, uint8, value_size * elements_to_alloc, el_alloc_flags, "ll values");
    DNX_SW_STATE_ALLOC(unit, module_id, ll->prevs, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll prevs");
    DNX_SW_STATE_ALLOC(unit, module_id, ll->nexts, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll nexts");

    /* initialize the list of free cells */
    node = 1;
    SW_STATE_LL_SET_HEAD(0, node);
    SW_STATE_LL_SET_TAIL(0, init_info->max_nof_elements);

    /*
     * Due to large number of iterations during init, we optimize and bypass the swstate memcpy. 
     */
    if(SOC_IS_DONE_INIT(unit)) {
        for (index = 2; index < (init_info->max_nof_elements - 1); index++) {
            node = index - 1;
            SW_STATE_LL_SET_PREV(index, node);

            node = index + 1;
            SW_STATE_LL_SET_NEXT(index, node);
        }
    } else {
        for (index = 2; index < (init_info->max_nof_elements - 1); index++) {
            node = index - 1;
            ll->prevs[index] = node;

            node = index + 1;
            ll->nexts[index] = node;
        }
    }

    SW_STATE_LL_SET_PREV(1, sw_state_ll_invalid);

    node = 2;
    SW_STATE_LL_SET_NEXT(1, node);

    node = init_info->max_nof_elements - 1;
    SW_STATE_LL_SET_PREV(init_info->max_nof_elements, node);

    SW_STATE_LL_SET_NEXT(init_info->max_nof_elements, sw_state_ll_invalid);


    /* save some of the ll properties */
    is_sorted_ll = sw_state_is_flag_on(flags, SW_STATE_LL_SORTED);
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->is_sorted, &is_sorted_ll, sizeof(is_sorted_ll), 0x0, "is_sorted_ll");
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->nof_heads, &init_info->nof_heads, sizeof(init_info->nof_heads), 0x0, "nof_heads");
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->key_size, &key_size, sizeof(key_size), 0x0, "key_size");
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->value_size, &value_size, sizeof(value_size), 0x0, "value_size");

    if (ll->is_sorted) {
        if (init_info->key_cmp_cb_name != NULL) {
            SW_STATE_CB_DB_REGISTER_CB(module_id, ll->key_cmp_cb, init_info->key_cmp_cb_name, dnx_sw_state_sorted_list_cb_get_cb);
        }
        else{
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state sorted ll ERROR: ll must be given a compare cb.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int sw_state_ll_add(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    /* sanity validations */
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);

    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), SW_STATE_LL_INVALID, 0, key, value);
    
    DNXC_SW_STATE_FUNC_RETURN;
}


/*
 * see .h file for description
 */
int sw_state_ll_add_first(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), SW_STATE_LL_INVALID, 1, key, value);

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_add_last(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), ll->tail[TRANSLATE_LL_IDX(ll_index)], 0, key, value);
    
    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_add_before(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    SW_STATE_LL_CHECK_VALID_NODE(node);

    /* if no prev add as first */
    if (ll->prevs[node] == SW_STATE_LL_INVALID) {
        return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), SW_STATE_LL_INVALID, 0, key, value);
    }

    /* prev next should be node */
    assert (ll->nexts[ll->prevs[node]] != SW_STATE_LL_INVALID);

    /* add as next node of prev node */
    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), ll->prevs[node], 0, key, value);

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_add_after(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    SW_STATE_LL_CHECK_VALID_NODE(node);

    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), node, 0, key, value);
    
    DNXC_SW_STATE_FUNC_RETURN; 
}

STATIC int sw_state_ll_return_node_to_pool(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node) {

    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    if (ll->head[0] == SW_STATE_LL_INVALID) {
        /* if free list is empty */
        SW_STATE_LL_SET_HEAD(0, node);
        SW_STATE_LL_SET_TAIL(0, node);

        SW_STATE_LL_SET_NEXT(node, sw_state_ll_invalid);
        SW_STATE_LL_SET_PREV(node, sw_state_ll_invalid);

    }
    else {
        SW_STATE_LL_SET_NEXT(node, ll->head[0]);
        SW_STATE_LL_SET_PREV(ll->head[0], node);
        SW_STATE_LL_SET_HEAD(0, node);

    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int sw_state_ll_remove_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node) {

    /*
     * used for freeing sw state memory
     */
    sw_state_ll_node_t prev = SW_STATE_LL_INVALID;
    sw_state_ll_node_t next = SW_STATE_LL_INVALID;
    uint32 translated_ll_index = TRANSLATE_LL_IDX(ll_index);
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_VALID_NODE(node);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);
    
    /* set key to -1 */
    if (ll->is_sorted) {
        DNX_SW_STATE_MEMSET(unit, module_id, KEY_AT_IDX(node), 0, -1, ll->key_size, 0x0, "sw state ll node's key");
    }

    /* set value to -1 */
    DNX_SW_STATE_MEMSET(unit, module_id, VALUE_AT_IDX(node), 0, -1, ll->value_size, 0x0, "sw state ll node's value");

    /* regular case */
    if ((ll->nexts[node] != SW_STATE_LL_INVALID) && (ll->prevs[node] != SW_STATE_LL_INVALID)) {

        prev = ll->prevs[node];
        next = ll->nexts[node];

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_NEXT(prev, next);
        SW_STATE_LL_SET_PREV(next, prev);

        SOC_EXIT;
    }

    /* removing the only node */
    if ((ll->nexts[node] == SW_STATE_LL_INVALID) && (ll->prevs[node] == SW_STATE_LL_INVALID)) {

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_HEAD(translated_ll_index, sw_state_ll_invalid);
        SW_STATE_LL_SET_TAIL(translated_ll_index, sw_state_ll_invalid);

        SOC_EXIT;
    }

    /* removing the tail node */
    if (ll->nexts[node] == SW_STATE_LL_INVALID) {

        prev = ll->prevs[node];

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));


        SW_STATE_LL_SET_NEXT(prev, sw_state_ll_invalid);
        SW_STATE_LL_SET_TAIL(translated_ll_index, prev);

        SOC_EXIT;
    }

    /* removing the head node */
    if (ll->prevs[node] == SW_STATE_LL_INVALID) {

        next = ll->nexts[node];

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_PREV(next, sw_state_ll_invalid);
        SW_STATE_LL_SET_HEAD(translated_ll_index, next);

        SOC_EXIT;
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int sw_state_ll_node_update(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, const void *value) {

    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /* replace the data */
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, VALUE_AT_IDX(node), value, ll->value_size, 0x0, "sw state ll node's value");

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int sw_state_ll_next_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_VALID_NODE(input_node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->nexts[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_previous_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_VALID_NODE(input_node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->prevs[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_get_first(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->head[TRANSLATE_LL_IDX(ll_index)];

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_get_last(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->tail[TRANSLATE_LL_IDX(ll_index)];

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_nof_elements(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, uint32 *nof_elements) {

    sw_state_ll_node_t node;
    int count = 0;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to access inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);
  
    SW_STATE_LL_CHECK_NULL_PARAMETER(nof_elements);
    
    node = ll->head[TRANSLATE_LL_IDX(ll_index)];

    while (node != SW_STATE_LL_INVALID) {
        count++;
        node = ll->nexts[node];
    }

    *nof_elements = count;

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_node_key(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *key) {
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to access inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(key);

    sal_memcpy(key, KEY_AT_IDX(node), ll->key_size);

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int sw_state_ll_node_value(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *value) {

    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(ll != NULL);

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);

    sal_memcpy(value, VALUE_AT_IDX(node), ll->value_size);

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int sw_state_ll_destroy(int unit, uint32 module_id, sw_state_ll_t *ll_ptr) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert((*ll_ptr) != NULL);

    /* this function is generally not allowed */
    /* all memory will get freed automaticaly at detach */

    DNXC_SW_STATE_FUNC_RETURN;
}

STATIC DNXC_SW_STATE_INLINE void
sw_state_ll_data_hexdump(int unit, uint8 *data, uint32 size) {
    int i;
    assert(data != NULL);
    
    for (i = 0; i < size; i++)
    {
        DNX_SW_STATE_PRINT(unit, "%02X", data[i]);
    }
}

/*
 * see .h file for description
 */
int sw_state_ll_print(int unit, sw_state_ll_t ll) {

    sw_state_ll_node_t node;
    int count = 0;
    uint32 ll_index;
    uint32 nof_elements;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);

    /* check how many free nodes are there */
    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, -1, ll, REVERSE_TRANSLATE_LL_IDX(0), &nof_elements));
    DNX_SW_STATE_PRINT(unit, "Number of unused nodes: %d\n", nof_elements);

    /* loop on index 0..nof_heads+1 as index 0 is reserved for free nodes pool */
    for (ll_index = 1; ll_index < ll->nof_heads+1; ll_index++) {
        node = ll->head[ll_index];

        if (ll->nof_heads == 1) {
            DNX_SW_STATE_PRINT(unit, "Printing Linked List:\n");
        }
        else {
            DNX_SW_STATE_PRINT(unit, "Printing Linked List #%d:\n", ll_index);
        }


        /* regular print - without stride improvements */
        while (node != SW_STATE_LL_INVALID) {
            count++;
            DNX_SW_STATE_PRINT(unit, " %d -> ", count);
            if (ll->is_sorted) {
                DNX_SW_STATE_PRINT(unit, "key: ");
                sw_state_ll_data_hexdump(unit, KEY_AT_IDX(node), ll->key_size);
                DNX_SW_STATE_PRINT(unit, ", value: ");
            }
            sw_state_ll_data_hexdump(unit, VALUE_AT_IDX(node), ll->value_size);
            DNX_SW_STATE_PRINT(unit, "\n");
            node = ll->nexts[node];
        }
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_find(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t * node, const void *key, uint8* found) {

    sw_state_ll_node_t curr_node;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    int32 cmp_res;
    
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    SW_STATE_LL_CHECK_NULL_PARAMETER(found);

    SW_STATE_CB_DB_GET_CB(
        module_id,
        ll->key_cmp_cb,
        &sorted_list_cmp_cb,
        dnx_sw_state_sorted_list_cb_get_cb
    );

    *found = 0;
    for(curr_node = ll->head[TRANSLATE_LL_IDX(ll_index)]; curr_node != SW_STATE_LL_INVALID; curr_node = ll->nexts[curr_node]) {
        cmp_res = sorted_list_cmp_cb(KEY_AT_IDX(curr_node), (uint8*)key, ll->key_size);
        if (cmp_res == 0) {
            *node = curr_node;
            *found = 1;
            break;
        } else if (cmp_res < 0) {
            *node = curr_node;
        } else {
            break;
        }
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


