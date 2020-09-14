/** \file sw_state_binary_tree.h
 *  
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: binary tree header file
 *
 */
#ifndef _DNX_SW_STATE_BINARY_TREE_H
/* { */
#define _DNX_SW_STATE_BINARY_TREE_H

/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/error.h>
#include <shared/bsl.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>

#define SW_STATE_BT_INVALID               (0)

#define DNX_SW_STATE_BT_IS_NODE_VALID(node)\
    (node != SW_STATE_BT_INVALID)

typedef struct sw_state_bt_init_info_s
{
    uint32 max_nof_elements;
    uint32 expected_nof_elements;
} sw_state_bt_init_info_t;

/* Type representing a node in the tree.
   Note: It is invalidated by insertions and removals!  */
typedef uint32 sw_state_bt_node_t;

typedef struct sw_state_bt_s
{
    sw_state_bt_node_t free_list_head;  /* free node list pool head, it is chained using left and right child pointers */
    sw_state_bt_node_t root;    /* root node of the tree */
    uint32 value_size;
    uint8 *values;              /* array of values */
    sw_state_bt_node_t *parents;        /* Indexes of all parent nodes */
    sw_state_bt_node_t *lchilds;        /* Indexes of all left child nodes */
    sw_state_bt_node_t *rchilds;        /* Indexes of all right child nodes */
}  *sw_state_bt_t;

/**
* \brief
*  The function will create a new binary tree Data Structure
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] init_info    - info about the Binary tree (size of 
*        data, nof elements etc.)
* \param [in] value_size - value size
* \param [in] bt_ptr  - binary tree to operate on
* \param [in] max_nof_elements - Max numbers of elements. Specified in the relevant sw state xml file.
* \param [in] alloc_flags - SW state allocation flags.
*        This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_create_empty(
    int unit,
    uint32 module_id,
    sw_state_bt_init_info_t * init_info,
    uint32 value_size,
    sw_state_bt_t * bt_ptr,
    uint32 max_nof_elements,
    uint32 alloc_flags);

/**
* \brief
*  The function will add a new node to the binary tree Data
*  Structure as the root of the tree
* \param [in] unit    - Device Id
* \param [in] module_id  - Module Id
* \param [in] value  - the content of the node's value
* \param [in] bt  - binary tree to operate on 
* \return
*   int - Error Type
* \remark
*   * If the root is already present in the tree, it will return error
* \see
*   * None
*/
int sw_state_bt_add_root(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    const uint8 *value);

/**
* \brief
*  The function will add a new node to the binary tree Data
*  Structure as the left child of the given node
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] node_parent  - new node will be added as a left child
*        of the supplied parent node
* \param [in] value  - the content of the node's value
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_add_left_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node_parent,
    const uint8 *value);

/**
* \brief
*  The function will add a new node to the binary tree Data
*  Structure as the right child of the given node
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] node_parent  - new node will be added as a right child
*        of the supplied parent node
* \param [in] value  - the content of the node's value
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_add_right_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node_parent,
    const uint8 *value);

/**
* \brief
*  The function will replace the data of a specified node in the
*  binary tree Data Structure
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] node  - node to operate on
* \param [in] value  - new content of the node's value
* \param [in] bt  - binary tree to operate on
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_node_update(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    const uint8 *value);

/**
* \brief
*  The function frees a node
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] node  - the node to be freed
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_bt_node_free(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node);

/**
* \brief
*  The function will set the parent node of a given node in the
*  binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] node  - node to operate on
* \param [in] node_parent    - the parent node
* \return
*   int - Error Type
* \see
*   * None
*/
int sw_state_bt_set_parent(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    sw_state_bt_node_t node_parent);

/**
* \brief
*  The function will set the left child of a given node in the
*  binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] bt  - binary tree to operate on 
* \param [in] module_id  - Module Id
* \param [in] node  - node to operate on
* \param [in] node_left    - the left child node
* \remark
*   * None
* \return
*   int - Error Type
* \see
*   * None
*/
int sw_state_bt_set_left_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    sw_state_bt_node_t node_left);

/**
* \brief
*  The function will set the right child of a given node in the
*  binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] bt  - binary tree to operate on 
* \param [in] module_id  - Module Id
* \param [in] node  - node to operate on
* \param [in] node_right    - the right child node
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_set_right_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    sw_state_bt_node_t node_right);

/**
* \brief
*  The function will fetch the root node of
*  the binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [out] output_node    - the root node will be put here
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_get_root(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t * output_node);

/**
* \brief
*  The function will fetch the parent node of a given node in the
*  binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] input_node  - node to operate on
* \param [out] output_node    - the parent node will be put here
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_get_parent(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t input_node,
    sw_state_bt_node_t * output_node);

/**
* \brief
*  The function will fetch the left child node of a given node in the
*  binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] input_node  - node to operate on
* \param [out] output_node    - the left child node will be put here
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_get_left_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t input_node,
    sw_state_bt_node_t * output_node);

/**
* \brief
*  The function will fetch the right child node of a given node in the
*  binary tree Data Structure.
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] input_node  - node to operate on
* \param [out] output_node    - the right child node will be put here
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int sw_state_bt_get_right_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t input_node,
    sw_state_bt_node_t * output_node);

/**
* \brief
*  The function will destroy the binary tree Data Structure
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt_ptr  - Pointer to binary tree to operate on
* \return
*   int - Error Type
* \remark
*   * it will free all nodes and their data
* \see
*   * None
*/
int sw_state_bt_destroy(
    int unit,
    uint32 module_id,
    sw_state_bt_t * bt_ptr);

/**
* \brief
*  The function will extract the data kept in a binary tree Data
*  Structure node
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [in] node  - node to operate on 
* \param [out] value  - the data will be copied to here 
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_bt_node_value(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    uint8 *value);

/**
* \brief
*  The function will compute the number of elements kept in the
*  binary tree
* \param [in] unit    - Device Id 
* \param [in] module_id  - Module Id
* \param [in] bt  - binary tree to operate on 
* \param [out] nof_elements  - the nof_elements will be copied to here 
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_bt_nof_elements(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    uint32 *nof_elements);

/**
* \brief
*  The function will print to the screen a representation of the
*  binary tree
* \param [in] unit    - Device Id 
* \param [in] bt  - binary tree to operate on
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_bt_print(
    int unit,
    sw_state_bt_t bt);

/**
 * \brief - Returned a bt structure size.
 *
 * \param [in] init_info    - info about the Binary tree (size of
 *        data, nof elements etc.)
 * \param [in] value_size - value size
 *
 * \return
 *   int
 *
 * \remark
 *   None
 * \see
 *   None
 */
int sw_state_bt_create_empty_size_get(
    sw_state_bt_init_info_t * init_info,
    uint32 value_size);

/**********************************************/
/**********     BINARY TREE MACROS ****************/
/**********************************************/

#define SW_STATE_BT_CREATE_EMPTY(module_id, bt, init_info, value_type, max_nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_bt_create_empty(unit, module_id, init_info, sizeof(value_type), &bt, max_nof_elements, alloc_flags))

#define SW_STATE_BT_DESTROY(module_id, bt) \
    SHR_IF_ERR_EXIT(sw_state_bt_destroy(unit, module_id, &bt))

#define SW_STATE_BT_NOF_ELEMENTS(module_id, bt, nof_elements) \
    SHR_IF_ERR_EXIT(sw_state_bt_nof_elements(unit, module_id, bt, nof_elements))

#define SW_STATE_BT_NODE_VALUE(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_node_value(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_NODE_UPDATE(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_node_update(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_NODE_FREE(module_id, bt, node) \
    SHR_IF_ERR_EXIT(sw_state_bt_node_free(unit, module_id, bt, node))

#define SW_STATE_BT_GET_ROOT(module_id, bt, root) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_root(unit, module_id, bt, root))

#define SW_STATE_BT_GET_PARENT(module_id, bt, node, node_parent) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_parent(unit, module_id, bt, node, node_parent))

#define SW_STATE_BT_GET_LEFT_CHILD(module_id, bt, node, node_left) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_left_child(unit, module_id, bt, node, node_left))

#define SW_STATE_BT_GET_RIGHT_CHILD(module_id, bt, node, node_right) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_right_child(unit, module_id, bt, node, node_right))

#define SW_STATE_BT_ADD_ROOT(module_id, bt, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_add_root(unit, module_id, bt, (uint8 *)value))

#define SW_STATE_BT_ADD_LEFT_CHILD(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_add_left_child(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_ADD_RIGHT_CHILD(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_add_right_child(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_SET_PARENT(module_id, bt, node, node_parent) \
    SHR_IF_ERR_EXIT(sw_state_bt_set_parent(unit, module_id, bt, node, node_parent))

#define SW_STATE_BT_SET_LEFT_CHILD(module_id, bt, node, node_left) \
    SHR_IF_ERR_EXIT(sw_state_bt_set_left_child(unit, module_id, bt, node, node_left))

#define SW_STATE_BT_SET_RIGHT_CHILD(module_id, bt, node, node_right) \
    SHR_IF_ERR_EXIT(sw_state_bt_set_right_child(unit, module_id, bt, node, node_right))

#define SW_STATE_BT_PRINT(module_id, bt)\
    SHR_IF_ERR_EXIT(sw_state_bt_print(unit, bt))

#define SW_STATE_BT_DEFAULT_PRINT(unit, bt)\
    SHR_IF_ERR_EXIT(sw_state_bt_print(unit, *(bt)))

#define SW_STATE_BT_CREATE_EMPTY_SIZE_GET(init_info, value_type) \
    sw_state_bt_create_empty_size_get(init_info, sizeof(value_type))

#define SW_STATE_BT_SIZE_GET(init_info, value_type) \
        SW_STATE_BT_CREATE_EMPTY_SIZE_GET(init_info, value_type)

#endif /* _DNX_SW_STATE_BINARY_TREE_H */
