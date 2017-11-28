/** \file sw_state_linked_list.c
 *
 * A linked list implementation that is used through sw_state
 * interface
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
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
#include <soc/dnxc/swstate/sw_state_linked_list.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.h>
#include <soc/dnxc/swstate/sw_state_cb_db.h>
#include <soc/error.h>
#include <shared/bsl.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#define SW_STATE_LL_CHECK_NULL_PARAMETER(param) \
    do { \
        if ((param) == NULL) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: NULL argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)

/**
* \brief
*  The function is an internal function that will create a new
*  node
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id.
* \param [in] prev  - the previous node, can be NULL.
* \param [in] next  - the next node, can be NULL. 
* \param [out] location  - the created node will be but here.
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
STATIC int sw_state_ll_create_new_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *location, 
                                const void *key, const void *value, sw_state_ll_node_t prev, sw_state_ll_node_t next) {

    uint32 fll;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_cb_t cb_db;
    sw_state_ll_node_t prev_location = *location;
    
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted && ll->key_cmp_cb_name != NULL) {
        sal_strncpy(cb_db.function_name, ll->key_cmp_cb_name, SW_STATE_CB_DB_NAME_STR_SIZE-1);
        dnx_sw_state_sorted_list_cb_get_cb(&cb_db, 0, &sorted_list_cmp_cb);        
    }

    assert(ll != NULL);
    assert(location != NULL);

    assert(DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION == (1 << 4));

    fll = DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION;

    (*location) = NULL;

    DNX_SW_STATE_ALLOC(unit, module_id, (*location), sw_state_ll_node_impl, 1, fll, "sw_state ll node");

    if ((*location) == NULL) {
        *location = prev_location;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "sw state ll ERROR: failed to alloc node.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if (key != NULL) {

        DNX_SW_STATE_ALLOC(unit, module_id, (*location)->key, uint8, ll->key_size, DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION, "sw_state ll node's key");

        if ((*location)->key == NULL) {

            DNX_SW_STATE_FREE(unit, module_id, (*location), "sw sate ll free");

            *location = prev_location;
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "sw state ll ERROR: failed to alloc node's key.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, (*location)->key, key, ll->key_size, 0x0, "sw state ll node's key");
    } else {
        (*location)->key = NULL;
    }

    DNX_SW_STATE_ALLOC(unit, module_id, (*location)->value, uint8, ll->value_size, DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION, "sw_state ll node's value");

    if ((*location)->value == NULL) {
        if ((*location)->key == NULL) {
            DNX_SW_STATE_FREE(unit, module_id, (*location)->key, "sw state ll key free");
        }

        DNX_SW_STATE_FREE(unit, module_id, (*location), "sw state ll location free");

        *location = prev_location;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "sw state ll ERROR: failed to alloc node's value.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, (*location)->value, value, ll->value_size, 0x0, "sw state ll node's value");

    (*location)->next=next;
    (*location)->prev=prev;
    
    if (ll->is_sorted) {
        
        /* in case of sorted list, the key must be (strictly) bigger than prev->key and (strictly) smaller than next->key */
        if (((*location)->next != NULL && sorted_list_cmp_cb((*location)->key, (*location)->next->key, ll->key_size) >= 0)
            || ((*location)->prev != NULL && sorted_list_cmp_cb((*location)->key, (*location)->prev->key, ll->key_size) <= 0)) {

                DNX_SW_STATE_FREE(unit, module_id, (*location)->key, "sw state ll key free");
                DNX_SW_STATE_FREE(unit, module_id, (*location)->value, "sw state ll value free");
                DNX_SW_STATE_FREE(unit, module_id, (*location), "sw state ll entry free");

                *location = prev_location;
                SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }   
    }

    if ((*location)->next != NULL) {
        (*location)->next->prev = (*location);
    } else {
        ll->tail[ll_index] = (*location);
    }
    
    if ((*location)->prev != NULL) {
        (*location)->prev->next = (*location);
    } else {
        ll->head[ll_index] = (*location);
    }
    DNX_SW_STATE_FUNC_RETURN;

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
    sw_state_cb_t cb_db;
    
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        sal_strncpy(cb_db.function_name, ll->key_cmp_cb_name, SW_STATE_CB_DB_NAME_STR_SIZE-1);
        dnx_sw_state_sorted_list_cb_get_cb(&cb_db, 0, &sorted_list_cmp_cb);        
    }

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    assert(ll != NULL);
    if (ll->is_sorted) {
        assert(key != NULL);
    }
    assert(value != NULL);

    /* add to an empty list */
    /* same for sorted and not sorted */
    if (ll->tail[ll_index] == NULL) {
        assert(ll->head[ll_index]==NULL);
        assert(node_to_add_after==NULL);

        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &(ll->head[ll_index]), key, value, NULL, NULL));

        ll->tail[ll_index] = ll->head[ll_index];
        SOC_EXIT;
    }

    /* if got node to add after */
    if (node_to_add_after != NULL) {
        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &(node_to_add_after->next), key, value, node_to_add_after, node_to_add_after->next));
        SOC_EXIT;
    }
    
    if (ll->is_sorted && !is_first) {
        sw_state_ll_node_t node = ll->head[ll_index];
        
        /* if only one element is the list */
        if (node->next == NULL) {
            if (sorted_list_cmp_cb((uint8*)node->key, (uint8*)key, ll->key_size) < 0) {
                SHR_IF_ERR_EXIT(sw_state_ll_add_last(unit, module_id, ll, ll_index, key, value));
            } else {
                SHR_IF_ERR_EXIT(sw_state_ll_add_first(unit, module_id, ll, ll_index, key, value));
            }
            SOC_EXIT;
        }
        while (node->next != NULL && sorted_list_cmp_cb((uint8*)node->next->key, (uint8*)key, ll->key_size) < 0) {
            node = node->next;
        }
        if (node == ll->head[ll_index]) {
            SHR_IF_ERR_EXIT(sw_state_ll_add_first(unit, module_id, ll, ll_index, key, value));
            SOC_EXIT;
        } else if (node == ll->tail[ll_index]) {
            SHR_IF_ERR_EXIT(sw_state_ll_add_last(unit, module_id, ll, ll_index, key, value));
            SOC_EXIT;
        } else {
            SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &(node->next), key, value, node, node->next));
            SOC_EXIT;
        }
    } else {
        /* if didn't get a node to add after, add at the beginnig of list */
        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &(ll->head[ll_index]), key, value, NULL, ll->head[ll_index]));
        SOC_EXIT;
    }


    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_create_empty(int unit, uint32 module_id, sw_state_ll_init_info_t *init_info, uint32 flags, uint32 key_size, uint32 value_size, sw_state_ll_t * ll) {

    int ll_index;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(init_info);
    
    DNX_SW_STATE_ALLOC(unit, module_id, *ll, **ll, /*number of elements*/ 1, DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION, "sw_state linked list");

    /* if single head linked list - overide nof_heads with 1 */
    if (!sw_state_is_flag_on(flags, SW_STATE_LL_MULTIHEAD)) {
        init_info->nof_heads = 1;
    }

    /* allocate head and tail arrays */
    DNX_SW_STATE_ALLOC(unit, module_id, (*ll)->head, sw_state_ll_node_t, init_info->nof_heads, DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION, "ll heads");
    DNX_SW_STATE_ALLOC(unit, module_id, (*ll)->tail, sw_state_ll_node_t, init_info->nof_heads, DNX_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION, "ll tails");

    (*ll)->nof_heads = init_info->nof_heads;

    for (ll_index=0; ll_index < (*ll)->nof_heads; ll_index++) {
        (*ll)->head[ll_index] = NULL;
        (*ll)->tail[ll_index] = NULL;
    }

    (*ll)->is_sorted = sw_state_is_flag_on(flags, SW_STATE_LL_SORTED);
    
    (*ll)->key_size = key_size;
    (*ll)->value_size = value_size;
    
    if (init_info->key_cmp_cb_name != NULL) {
        sal_strncpy((*ll)->key_cmp_cb_name, init_info->key_cmp_cb_name, SW_STATE_CB_DB_NAME_STR_SIZE-1);
    }
    
    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_add(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, ll_index, NULL, 0, key, value);
    
    DNX_SW_STATE_FUNC_RETURN;
}


/*
 * see .h file for description
 */
int sw_state_ll_add_first(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, ll_index, NULL, 1, key, value);

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_add_last(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, ll_index, ll->tail[ll_index], 0, key, value);
    
    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_add_before(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value) {

    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);

    /* if no prev add as first */
    if (node->prev == NULL) {
        return sw_state_ll_add_internal(unit, module_id, ll, ll_index, NULL, 0, key, value);
    }

    /* prev next should be node */
    assert (node->prev->next != NULL);

    /* add as next node of prev node */
    return sw_state_ll_add_internal(unit, module_id, ll, ll_index, node->prev, 0, key, value);

    DNX_SW_STATE_FUNC_RETURN;    
}

/*
 * see .h file for description
 */
int sw_state_ll_add_after(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value) {

    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    
    return sw_state_ll_add_internal(unit, module_id, ll, ll_index, node, 0, key, value);
    
    DNX_SW_STATE_FUNC_RETURN; 
}

/*
 * see .h file for description
 */
int sw_state_ll_remove_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node) {


    /*
     * used for freeing sw state memory
     */
    sw_state_ll_node_t prev = NULL;
    sw_state_ll_node_t next = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    
    /* free the data */
    if (ll->is_sorted)
    {
        DNX_SW_STATE_FREE(unit, module_id, node->key, "sw state ll key free");
    }

    DNX_SW_STATE_FREE(unit, module_id, node->value, "sw state ll value free");

    /* regular case */
    if ((node->next != NULL) && (node->prev != NULL)) {
        prev = node->prev;
        next = node->next;

        /* free the node itself, use real sw state location */
        DNX_SW_STATE_FREE_BASIC(unit, module_id, (uint8 **)(&node->prev->next), DNX_SW_STATE_LINKED_LIST_FREE, "sw state ll node free");
        node = NULL;

        prev->next = next;
        next->prev = prev;

        SOC_EXIT;
    }

    /* removing the only node */
    if ((node->next == NULL) && (node->prev == NULL)) {
        /* free the node itself */
        DNX_SW_STATE_FREE_BASIC(unit, module_id, (uint8 **)(&ll->head[ll_index]), DNX_SW_STATE_LINKED_LIST_FREE, "sw state ll node free");
        node = NULL;

        ll->head[ll_index] = NULL;
        ll->tail[ll_index] = NULL;

        SOC_EXIT;
    }

    /* removing the tail node */
    if ((node->next == NULL)) {
        prev = node->prev;

        /* free the node itself */
        DNX_SW_STATE_FREE_BASIC(unit, module_id, (uint8 **)(&node->prev->next), DNX_SW_STATE_LINKED_LIST_FREE, "sw state ll remove tail node");
        node = NULL;

        prev->next = NULL;
        ll->tail[ll_index] = prev;

        SOC_EXIT;
    }

    /* removing the head node */
    if ((node->prev == NULL)) {
        next = node->next;

        /* free the node itself */
        DNX_SW_STATE_FREE_BASIC(unit, module_id, (uint8 **)(&ll->head[ll_index]), DNX_SW_STATE_LINKED_LIST_FREE, "sw state ll remove tail node");
        node = NULL;

        next->prev = NULL;
        ll->head[ll_index] = next;

        SOC_EXIT;
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_node_update(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, const void *value) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    
    /* replace the data */
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, node->value, value, ll->value_size, 0x0, "sw state ll node's value");

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_next_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(input_node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = input_node->next;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_previous_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(input_node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = input_node->prev;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_get_first(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);
    
    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = ll->head[ll_index];

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_get_last(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node) {

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    /* verify ll was created */
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = ll->tail[ll_index];

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_nof_elements(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, uint32 *nof_elements) {

    sw_state_ll_node_t node;
    int count = 0;
    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(nof_elements);
    
    node = ll->head[ll_index];

    while (node != NULL) {
        count++;
        node = node->next;
    }

    *nof_elements = count;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_node_key(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *key) {
    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    
    sal_memcpy(key,node->key,ll->key_size);

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_node_value(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *value) {
    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    
    sal_memcpy(value,node->value,ll->value_size);

    DNX_SW_STATE_FUNC_RETURN;

}

/*
 * see .h file for description
 */
int sw_state_ll_destroy(int unit, uint32 module_id, sw_state_ll_t *ll_ptr) {

    uint32 ll_index;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert((*ll_ptr) != NULL);

    for (ll_index = 0; ll_index < (*ll_ptr)->nof_heads ; ll_index++) {
        if ((*ll_ptr) != NULL) {
            while ((*ll_ptr)->tail[ll_index] != NULL) {
                SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, module_id, (*ll_ptr), ll_index, (*ll_ptr)->tail[ll_index]));
            }
        }
    }

    DNX_SW_STATE_FREE(unit, module_id, (*ll_ptr)->head, "ll heads");
    DNX_SW_STATE_FREE(unit, module_id, (*ll_ptr)->tail, "ll tails");
    DNX_SW_STATE_FREE(unit, module_id, (*ll_ptr), "swstate linked list");

    DNX_SW_STATE_FUNC_RETURN;
}

STATIC DNX_SW_STATE_INLINE void
sw_state_ll_data_hexdump(int unit, uint8 *data, uint32 size) {
    int i;
    assert(data != NULL);
    
    for (i = 0; i < size; i++)
    {
        _DNX_SW_STATE_PRINT(unit, "%02X", data[i]);
    }
}

/*
 * see .h file for description
 */
int sw_state_ll_print(int unit, sw_state_ll_t ll) {

    sw_state_ll_node_t node;
    int count = 0;
    uint32 ll_index;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);


    for (ll_index = 0; ll_index < ll->nof_heads; ll_index++) {
        node = ll->head[ll_index];

        if (ll->nof_heads == 1) {
            _DNX_SW_STATE_PRINT(unit, "Printing Linked List:\n");
        }
        else {
            _DNX_SW_STATE_PRINT(unit, "Printing Linked List #%d:\n", ll_index);
        }


        /* regular print - without stride improvements */
        while (node != NULL) {
            count++;
            _DNX_SW_STATE_PRINT(unit, " %d -> ", count);
            if (ll->is_sorted) {
                _DNX_SW_STATE_PRINT(unit, "key: ");
                sw_state_ll_data_hexdump(unit, (uint8 *) node->key, ll->key_size);
                _DNX_SW_STATE_PRINT(unit, ", value: ");
            }
            sw_state_ll_data_hexdump(unit, (uint8 *) node->value, ll->value_size);
            _DNX_SW_STATE_PRINT(unit, "\n");
            node = node->next;
        }
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int sw_state_ll_find(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t * node, const void *key, uint8* found) {

    sw_state_ll_node_t curr_node;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_cb_t cb_db;
    int32 cmp_res;
    
    DNX_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    SW_STATE_LL_CHECK_NULL_PARAMETER(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    SW_STATE_LL_CHECK_NULL_PARAMETER(found);
    
    sal_strncpy(cb_db.function_name, ll->key_cmp_cb_name, SW_STATE_CB_DB_NAME_STR_SIZE-1);
    dnx_sw_state_sorted_list_cb_get_cb(&cb_db, 0, &sorted_list_cmp_cb);

    *found = 0;
    for(curr_node = ll->head[ll_index]; curr_node != NULL; curr_node = curr_node->next) {
        cmp_res = sorted_list_cmp_cb((uint8*)curr_node->key, (uint8*)key, ll->key_size);
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

    DNX_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


