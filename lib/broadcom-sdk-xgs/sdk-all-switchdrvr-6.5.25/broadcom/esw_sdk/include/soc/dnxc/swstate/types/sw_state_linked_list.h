/** \file sw_state_linked_list.h
 *
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: linked list header file
 *
 */
#ifndef _DNX_SW_STATE_LINKED_LIST_H
/* { */
#define _DNX_SW_STATE_LINKED_LIST_H

/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/error.h>
#include <shared/bsl.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>

#define SW_STATE_LL_INVALID               (0)

#define DNX_SW_STATE_LL_IS_NODE_VALID(node)\
    (node != SW_STATE_LL_INVALID)

#define SW_STATE_LL_NO_FLAGS  0x0
#define SW_STATE_LL_SORTED    0x1
#define SW_STATE_LL_MULTIHEAD 0x2

#define SW_STATE_LL_NO_FLAGS  0x0
#define SW_STATE_LL_SORTED    0x1
#define SW_STATE_LL_MULTIHEAD 0x2

/* key type for regular (not sorted) list */
typedef int non_existing_dummy_type_t;

/**
 * \brief
 *   Typedef of procedure used to compare two keys (of elements on sorted list).
 * \par DIRECT INPUT
 *   \param [in]  buffer1 -
 *     Pointer to buffer of uint8s containing first sorted list key (of two)
 *     calculations
 *   \param [in]  buffer2 -
 *     Pointer to buffer of uint8s containing second sorted list key (of two)
 *     calculations
 *   \param [in]  size -
 *     Number of bytes on each key.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if the two keys are equal
 *   \retval Negative if the key on 'buffer1' is smaller than the key on 'buffer2'
 *   \retval Positive if the key on 'buffer1' is larger than the key on 'buffer2'
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   Built in algorithm, within this procedure, decides on the metric (for comparison)
 */
typedef int32 (
    *sw_state_sorted_ll_key_cmp_cb) (
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

typedef struct sw_state_ll_init_info_s
{
    uint32 max_nof_elements;    /* in case of multihead ll the max is for the sum of all lls */
    uint32 expected_nof_elements;
    char *key_cmp_cb_name;
    uint32 nof_heads;           /* no need to set it if not a multihead ll */
} sw_state_ll_init_info_t;

/* Type representing a node in the list.
   Note: It is invalidated by insertions and removals!  */
typedef uint16 sw_state_ll_node_t;

typedef struct sw_state_ll_s
{
    sw_state_ll_node_t *head;   /* array of heads */
    sw_state_ll_node_t *tail;   /* array of tails */
    uint8 is_sorted;
    uint32 max_level;           /* not used if linked list is not sorted */
    uint32 curr_level;          /* not used if linked list is not sorted */
    uint32 nof_heads;
    uint32 value_size;
    uint32 key_size;            /* not used if linked list is not sorted */
    sw_state_cb_t key_cmp_cb;
    uint8 *keys;                /* array of keys */
    uint8 *values;              /* array of values */
    sw_state_ll_node_t *nexts;
    sw_state_ll_node_t *prevs;
    sw_state_ll_node_t *service_downs;  /* not used if linked list is not sorted */
    sw_state_ll_node_t *service_nexts;
    sw_state_ll_node_t *service_prevs;
    sw_state_ll_node_t *service_pointers_keys;
    sw_state_ll_node_t *service_head;
    uint32 max_elements;
}  *sw_state_ll_t;

typedef sw_state_ll_t sw_state_sorted_ll_t;
typedef sw_state_ll_t sw_state_multihead_ll_t;

/**
* \brief
*  The function will create a new linked list Data Structure
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] init_info    - info about the Linked list (size of
*        data, nof elements etc.)
* \param [in] flags - 0 - none
*                     1 - sorted
*                     2 - multihead
* \param [in] key_size - key size
* \param [in] value_size - value size
* \param [in] ll_ptr - linked list to operate on
* \param [in] max_nof_elements - Max numbers of elements. Specified in the relevant sw state xml file.
* \param [in] alloc_flags - SW state allocation flags.
*        This flags value is determined by the variable type (dnx data for example) of the additional variables specified in the xml
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_create_empty(
    int unit,
    uint32 module_id,
    sw_state_ll_init_info_t * init_info,
    uint32 flags,
    uint32 key_size,
    uint32 value_size,
    sw_state_ll_t * ll_ptr,
    uint32 max_nof_elements,
    uint32 alloc_flags);

/**
* \brief
*  The function will add a new node to the linked list structure.
*  If the linked list is sorted then it will be added by the key to the appropriate place.
*  Else (not sorted), it will be added first.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] key    - the content of the node's key
* \param [in] value  - the content of the node's value
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_add(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    const void *key,
    const void *value);

/**
* \brief
*  The function will add a new node to the linked list Data
*  Structure as the first node in the list
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] key    - the content of the node's key
* \param [in] value  - the content of the node's value
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_add_first(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    const void *key,
    const void *value);

/**
* \brief
*  The function will add a new node to the linked list Data
*  Structure as the last node in the list - valid only for not sorted linked list.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] key    - the content of the node's key
* \param [in] value  - the content of the node's value
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_add_last(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    const void *key,
    const void *value);

/**
* \brief
*  The function will add a new node to the linked list Data
*  Structure as the first node in the list - valid only for not sorted linked list.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] node  - new node will be added before
*        the supplied node
* \param [in] key    - the content of the node's key
* \param [in] value  - the content of the node's value
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_add_before(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t node,
    const void *key,
    const void *value);

/**
* \brief
*  The function will add a new node to the linked list Data
*  Structure as the first node in the list - valid only for not sorted linked list.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] node  - new node will be added after
*        the supplied node
* \param [in] key    - the content of the node's key
* \param [in] value  - the content of the node's value
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_add_after(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t node,
    const void *key,
    const void *value);

/**
* \brief
*  The function will remove a specified node from the linked
*  list Data Structure
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] node  - node to remove
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_remove_node(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t node);

/**
* \brief
*  The function will replace data of a specified node in the
*  linked list Data Structure
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] node  - node to operate on
* \param [in] value  - new content of the node's value
* \param [in] ll  - linked list to operate on
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_node_update(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t node,
    const void *value);

/**
* \brief
*  The function will fetch the next node of a given node in the
*  linked list Data Structure.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] input_node  - node to operate on
* \param [in] output_node    - the next node
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * if the given node is the last node, it will fetch NULL
* \see
*   * None
*/
int sw_state_ll_next_node(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t input_node,
    sw_state_ll_node_t * output_node);

/**
* \brief
*  The function will fetch the prev node of a given node in the
*  linked list Data Structure.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] input_node  - node to operate on
* \param [in] output_node    - the prev node
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * if the given node is the first node, it will fetch NULL
* \see
*   * None
*/
int sw_state_ll_previous_node(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t input_node,
    sw_state_ll_node_t * output_node);

/**
* \brief
*  The function will fetch the first node i.e. the head node of
*  the linked list Data Structure.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [out] output_node    - the prev node
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_get_first(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t * output_node);

/**
* \brief
*  The function will fetch the first node i.e. the tail node of
*  the linked list Data Structure.
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [out] output_node    - the prev node
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_get_last(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t * output_node);

/**
* \brief
*  The function will destroy the linked list Data Structure
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] ll_ptr  - Pointer to linked list to operate on
* \return
*   int - Error Type
* \remark
*   * it will free all nodes and their data
* \see
*   * None
*/
int sw_state_ll_destroy(
    int unit,
    uint32 module_id,
    sw_state_ll_t * ll_ptr);

/**
* \brief
*  The function will extract the data kept in a linked list node's key
*  Structure node
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] ll  - linked list to operate on
* \param [in] node  - node to operate on
* \param [out] key  - the data will be copied to here
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_ll_node_key(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t node,
    void *key);

/**
* \brief
*  The function will extract the data kept in a linked list Data
*  Structure node
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] ll  - linked list to operate on
* \param [in] node  - node to operate on
* \param [out] value  - the data will be copied to here
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_ll_node_value(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t node,
    void *value);

/**
* \brief
*  The function will compute the number of elements kept in the
*  linked list
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \param [out] nof_elements  - the nof_elements will be copied to here
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_ll_nof_elements(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    uint32 *nof_elements);

/**
* \brief
*  The function will print to the screen a representation of the
*  linked list
* \param [in] unit    - Device Id
* \param [in] ll  - linked list to operate on
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_ll_print(
    int unit,
    sw_state_ll_t ll);

/**
* \brief
*  The function finds the node by the key if exists
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] ll  - linked list to operate on
* \param [in] ll_index  - linked list id to operate on in case
*        of a multihead ll.* \param [out] node    - the found node or null in case of not found
* \param [in] key    - the node's key to searched by
* \param [out] found    - 0 if not found. 1 if found
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_ll_find(
    int unit,
    uint32 module_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t * node,
    const void *key,
    uint8 *found);

/**********************************************/
/********** LINKED LIST MACROS ****************/
/**********************************************/

#define SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(module_id, ll, init_info, flags, key_type, value_type, max_nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_ll_create_empty(unit, module_id, init_info, flags, sizeof(key_type), sizeof(value_type), &ll, max_nof_elements, alloc_flags))

#define SW_STATE_MULTIHEAD_LL_DESTROY(module_id, ll) \
    SHR_IF_ERR_EXIT(sw_state_ll_destroy(unit, module_id, &ll))

#define SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(module_id, ll, ll_index,nof_elements) \
    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, module_id, ll, ll_index, nof_elements))

#define SW_STATE_MULTIHEAD_LL_NODE_KEY(module_id, ll, node, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_node_key(unit, module_id, ll, node, key))

#define SW_STATE_MULTIHEAD_LL_NODE_VALUE(module_id, ll, node, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_node_value(unit, module_id, ll, node, value))

#define SW_STATE_MULTIHEAD_LL_NODE_UPDATE(module_id, ll, node, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_node_update(unit, module_id, ll, node, value))

#define SW_STATE_MULTIHEAD_LL_NEXT_NODE(module_id, ll, ll_index,node, next_node) \
    SHR_IF_ERR_EXIT(sw_state_ll_next_node(unit, module_id, ll, ll_index, node, next_node))

#define SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(module_id, ll, ll_index,node, prev_node) \
    SHR_IF_ERR_EXIT(sw_state_ll_previous_node(unit, module_id, ll, ll_index, node, prev_node))

#define SW_STATE_MULTIHEAD_LL_ADD(module_id, ll, ll_index,key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add(unit, module_id, ll, ll_index, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_FIRST(module_id, ll, ll_index,key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_first(unit, module_id, ll, ll_index, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_LAST(module_id, ll, ll_index,key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_last(unit, module_id, ll, ll_index, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_BEFORE(module_id, ll, ll_index,node, key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_before(unit, module_id, ll, ll_index, node, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_AFTER(module_id, ll, ll_index, node, key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_after(unit, module_id, ll, ll_index, node, key, value))

#define SW_STATE_MULTIHEAD_LL_REMOVE_NODE(module_id, ll, ll_index, node) \
    SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, module_id, ll, ll_index, node))

#define SW_STATE_MULTIHEAD_LL_GET_LAST(module_id, ll, ll_index, node) \
    SHR_IF_ERR_EXIT(sw_state_ll_get_last(unit, module_id, ll, ll_index, node))

#define SW_STATE_MULTIHEAD_LL_GET_FIRST(module_id, ll, ll_index, node) \
    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, module_id, ll, ll_index, node))

#define SW_STATE_MULTIHEAD_LL_FIND(module_id, ll, ll_index, node, key, found) \
    SHR_IF_ERR_EXIT(sw_state_ll_find(unit, module_id, ll, ll_index, node, key, found))

#define SW_STATE_MULTIHEAD_LL_PRINT(module_id, ll)\
    SHR_IF_ERR_EXIT(sw_state_ll_print(unit, ll))

#define SW_STATE_MULTIHEAD_LL_DEFAULT_PRINT(unit, ll)\
    sw_state_ll_print(unit, *(ll))

/*
 * Regular LL uses multihead ll macros with index 0
 */
#define SW_STATE_LL_CREATE_EMPTY(module_id, ll, init_info, flags, key_type, value_type, max_nof_elements, alloc_flags) \
    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(module_id, ll, init_info, flags, key_type, value_type, max_nof_elements, alloc_flags)

#define SW_STATE_LL_DESTROY(module_id, ll) \
    SW_STATE_MULTIHEAD_LL_DESTROY(module_id, ll)

#define SW_STATE_LL_NOF_ELEMENTS(module_id, ll, nof_elements) \
    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(module_id, ll, 0, nof_elements)

#define SW_STATE_LL_NODE_KEY(module_id, ll, node, value) \
    SW_STATE_MULTIHEAD_LL_NODE_KEY(module_id, ll, node, value)

#define SW_STATE_LL_NODE_VALUE(module_id, ll, node, value) \
    SW_STATE_MULTIHEAD_LL_NODE_VALUE(module_id, ll, node, value)

#define SW_STATE_LL_NODE_UPDATE(module_id, ll, node, value) \
    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(module_id, ll, node, value)

#define SW_STATE_LL_NEXT_NODE(module_id, ll, node, next_node) \
    SW_STATE_MULTIHEAD_LL_NEXT_NODE(module_id, ll, 0, node, next_node)

#define SW_STATE_LL_PREVIOUS_NODE(module_id, ll, node, prev_node) \
    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(module_id, ll, 0, node, prev_node)

#define SW_STATE_LL_ADD(module_id, ll, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD(module_id, ll, 0, key, value)

#define SW_STATE_LL_ADD_FIRST(module_id, ll, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_FIRST(module_id, ll, 0, key, value)

#define SW_STATE_LL_ADD_LAST(module_id, ll, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_LAST(module_id, ll, 0, key, value)

#define SW_STATE_LL_ADD_BEFORE(module_id, ll, node, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_BEFORE(module_id, ll, 0, node, key, value)

#define SW_STATE_LL_ADD_AFTER(module_id, ll, node, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_AFTER(module_id, ll, 0, node, key, value)

#define SW_STATE_LL_REMOVE_NODE(module_id, ll, node) \
    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(module_id, ll, 0, node)

#define SW_STATE_LL_GET_LAST(module_id, ll, node) \
    SW_STATE_MULTIHEAD_LL_GET_LAST(module_id, ll, 0, node)

#define SW_STATE_LL_GET_FIRST(module_id, ll, node) \
    SW_STATE_MULTIHEAD_LL_GET_FIRST(module_id, ll, 0, node)

#define SW_STATE_LL_FIND(module_id, ll, node, key, found) \
    SW_STATE_MULTIHEAD_LL_FIND(module_id, ll, 0, node, key, found)

#define SW_STATE_LL_PRINT(module_id, ll)\
    SW_STATE_MULTIHEAD_LL_PRINT(module_id, ll)

#define SW_STATE_LL_DEFAULT_PRINT(unit, ll)\
    SW_STATE_MULTIHEAD_LL_DEFAULT_PRINT(unit, ll)

#define SW_STATE_LL_SIZE_GET(init_info, flags, key_type, value_type) \
        SW_STATE_MULTIHEAD_LL_CREATE_EMPTY_SIZE_GET(init_info, flags, key_type, value_type)

#endif /* _DNX_SW_STATE_LINKED_LIST_H */
