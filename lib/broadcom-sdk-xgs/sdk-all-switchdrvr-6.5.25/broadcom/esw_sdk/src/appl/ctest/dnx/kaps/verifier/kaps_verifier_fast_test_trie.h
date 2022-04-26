/*
 * ! \file kaps_verifier_fast_test_trie.h
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include "kaps_verifier_parse.h"
#include "kaps_list.h"

#ifndef __KAPS_VERIFIER_FAST_TRIE_H_INCLUDED
#define __KAPS_VERIFIER_FAST_TRIE_H_INCLUDED

/** Max number possible hits */
#define FTT_MAX_POSSIBLE_MATCHES 1024

/**
 * @cond INTERNAL
 *
 * @file kaps_verifierfast_test_trie.h
 *
 * Trie of ACL entries maintained in the test application in order to do fast searches of the keys and find the highest
 * matching ACL entry
 *
 */

/**
 * For ::kaps_parse_entries added to the fast trie, the
 * status field may be set to active or inactive in order
 * to enable a search hit
 */
enum fast_test_trie_status
{
    FAST_TRIE_INVALID = 0,         /**< Invalid status of the entry, should not be used */
    FAST_TRIE_ACTIVE = 1,          /**< The entry is available for search */
    FAST_TRIE_INACTIVE = 2,        /**< The entry is not available for search */
    FAST_TRIE_PENDING = 3,         /**< Awaiting activation */
    FAST_TRIE_MAY_BE_ACTIVE = 4,    /**< May be active */
    FAST_TRIE_MAY_BE_INACTIVE = 5    /**< May be inactive */
};

struct fast_test_trie;

/**
 * @brief Record having the ACL entry that is part of a
 * linked list associated with the trie node
 */
struct fast_test_trie_record
{
    struct kaps_list_node e_node;        /**< kaps_list_node for pointing to previous and next entries in the trie node */
    struct kaps_parse_record *entry; /**< pointer to the ACL entry */
};

/**
 * @brief Node in the fast test trie
 */
struct fast_test_trie_node
{
    uint8 data[KAPS_HW_MAX_DBA_WIDTH_8];   /**< Data corresponding to the trie node */
    uint8 mask[KAPS_HW_MAX_DBA_WIDTH_8];   /**< Mask corresponding to the trie node */
    struct fast_test_trie_node *zero_child; /**< All nodes with zero at bit location split into this node */
    struct fast_test_trie_node *one_child;  /**< All nodes with one at bit location split into this node */
    struct fast_test_trie_node *parent;     /**< link to walk back to the root */
    struct kaps_c_list entry_list;               /**< list of ACL entries of type fast_test_trie_record associated with the node*/
    uint32 depth;                         /**< Depth of the node in the fast test trie*/
    uint32 id;                            /**< Unique identifier for the node in the fast test trie*/
    struct kaps_list_node m_node;                /**< List_node for list formed during a search that correspond to Breadth First
                                               traversal of the trie*/
    struct fast_test_trie *trie;           /**< Pointer to the trie that has this node */
};

/**
 * @brief Stats for the fast test trie
 */
struct fast_test_trie_stats
{
    uint32 max_depth;                     /**< Maximum depth of the trie */
    uint32 num_nodes;                     /**< Number of nodes in the trie */
    uint32 total_num_entries;             /**< Total number of ACL entries stored in the trie */
    uint32 max_entries_per_node;          /**< Maximum number of ACL entries in a trie node */
    uint32 node_with_max_entries;         /**< Node-ID of the node with the maximum number of ACL entries */
};

/**
 * @brief trie used in the test application
 */
struct fast_test_trie
{
    struct fast_test_trie_node *root;              /**< Root of the decision tree */
    enum kaps_db_type db_type;                      /**< Type of the database */

};

struct bit_count
{
    uint32 ones;  /**< Number of ones at this bit location */
    uint32 zeros; /**< Number of zeros at this bit location */
    uint32 dc;    /**< Number of dont cares at this bit location */
};

/**
 * Construct the fast test trie
 *
 * @param e array of ACL entries to be inserted into the fast test trie
 * @param num_entries number of ACL entries to be inserted into the fast test trie
 * @param nbytes_valid number of valid bytes in each ACL entry
 * @param gmask Global Mask of the Database
 * @param test_trie_pp the pointer to the initialized fast test trie is returned here
 *
 * @return KAPS_OK on success or an error code
 */
kaps_status fast_test_trie_build(
    struct kaps_parse_record *e,
    uint32 num_entries,
    enum kaps_db_type db_type,
    uint32 nbytes_valid,
    uint8 *gmask,
    struct fast_test_trie **test_trie_pp);

/**
 * Search the keys in the fast test trie to determine the highest matching ACL entry for each key
 *
 * @param test_trie_p pointer to the initialized fast test trie
 * @param keys pointer to the array containing the keys
 * @param gmask Global Mask of the Database
 * @param all_matches Get all the possible matches
 * @param entries array of kaps_parse record pointers populated by search algorithm. The highest matching ACL entries for the keys will be returned
 *
 * @return KAPS_OK on success or an error code
 */
kaps_status fast_test_trie_search(
    struct fast_test_trie *test_trie_p,
    const uint8 *keys,
    uint32 nbytes_valid,
    uint8 *gmask,
    uint32 all_matches,
    struct kaps_parse_record **entries);

/**
 * Print the Stats of the fast test trie
 *
 * @param test_trie_p pointer to the initialized fast test trie
 *
 * @return KAPS_OK on success or an error code
 */
kaps_status fast_test_trie_print_stats(
    struct fast_test_trie *test_trie_p);

/**
 * Destroy the fast test trie
 *
 * @param test_trie_p pointer to the initialized fast test trie
 *
 * @return KAPS_OK on success or an error code
 */
kaps_status fast_test_trie_destroy(
    struct fast_test_trie *test_trie_p);

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif
