/** \file kaps_verifier_fast_test_trie.c
 *
 * Ctests for KAPS
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <sal/core/boot.h>

#include <shared/utilex/utilex_framework.h>

#include "kaps_verifier_fast_test_trie.h"
#include "kaps_bitmap.h"

#define MIN_ENTRIES_PER_NODE_TO_SPLIT (100)

#define MAX_NUM_TOLERABLE_EXTRA_DC_ENTRIES (100)

#define ENTRYLIST_TO_TEST_RECORD(ptr) CONTAINER_OF((ptr), struct fast_test_trie_record, e_node)

#define NODELIST_TO_TEST_NODE(ptr) CONTAINER_OF((ptr), struct fast_test_trie_node, m_node)

static void
ftt_get_bit_count(
    const struct kaps_parse_record *entry,
    uint32 nbytes_valid,
    struct bit_count *count,
    uint8 *g_mask)
{
    int32 i;
    for (i = 0; i < nbytes_valid; i++)
    {
        int32 j, index;
        uint8 mask = entry->mask[i];
        uint8 data = entry->data[i];
        uint8 gmask = g_mask ? g_mask[i] : 0;
        index = i * KAPS_BITS_IN_BYTE;
        for (j = index + (KAPS_BITS_IN_BYTE - 1); j >= index; j--)
        {
            if (gmask && (gmask & 1))
                count[j].dc += 1;
            else if (mask & 1)
                count[j].dc += 1;
            else if (data & 1)
                count[j].ones += 1;
            else
                count[j].zeros += 1;
            if (gmask)
                gmask >>= 1;
            mask >>= 1;
            data >>= 1;
        }
    }
}

static struct kaps_list_node *
ftt_merge_lists(
    struct kaps_list_node *ptr1,
    struct kaps_list_node *ptr2)
{
    struct kaps_list_node *head, *tmp;
    struct fast_test_trie_record *e1, *e2;

    if (!ptr1)
        return ptr2;
    else if (!ptr2)
        return ptr1;

    e1 = ENTRYLIST_TO_TEST_RECORD(ptr1);
    e2 = ENTRYLIST_TO_TEST_RECORD(ptr2);
    if (e1->entry->priority > e2->entry->priority)
    {
        tmp = ptr1;
        ptr1 = ptr2;
        ptr2 = tmp;
    }
    head = ptr1;

    while (ptr1 && ptr2)
    {
        if (ptr1->next)
        {
            e1 = ENTRYLIST_TO_TEST_RECORD(ptr1->next);
            e2 = ENTRYLIST_TO_TEST_RECORD(ptr2);
            if (e1->entry->priority > e2->entry->priority)
            {
                tmp = ptr2->next;
                ptr2->next = ptr1->next;
                ptr1->next = ptr2;
                ptr2 = tmp;
                ptr1 = ptr1->next;
            }
            else
            {
                ptr1 = ptr1->next;
            }
        }
        else
        {
            ptr1->next = ptr2;
            break;
        }
    }

    return head;
}

static struct kaps_list_node *
ftt_divide_and_sort(
    struct kaps_list_node *ptr,
    uint32 num_elements)
{
    struct kaps_list_node *first_list_head;
    struct kaps_list_node *second_list_head;
    uint32 first_list_count, second_list_count;
    uint32 i;

    if (num_elements <= 1)
    {
        if (ptr)
        {
            ptr->next = NULL;
        }
        return ptr;
    }

    first_list_head = ptr;
    first_list_count = num_elements / 2;

    for (i = 0; i < first_list_count; ++i)
    {
        ptr = ptr->next;
    }

    second_list_head = ptr;
    second_list_count = num_elements - first_list_count;

    first_list_head = ftt_divide_and_sort(first_list_head, first_list_count);
    second_list_head = ftt_divide_and_sort(second_list_head, second_list_count);

    return ftt_merge_lists(first_list_head, second_list_head);

}

static kaps_status
ftt_merge_sort(
    struct kaps_c_list *list)
{
    struct kaps_list_node *cur_node;
    uint32 count;

    if (!list || list->count <= 1)
        return KAPS_OK;

    /*
     * Convert the circular list to a linear list
     * by making the last element point to NULL
     */

    list->head->prev->next = NULL;

    /*
     * Sort the list 
     */
    list->head = ftt_divide_and_sort(list->head, list->count);

    /*
     * Connect back the list to make it a doubly linked circular list 
     */
    cur_node = list->head;
    count = 1;
    while (cur_node->next)
    {
        cur_node->next->prev = cur_node;
        cur_node = cur_node->next;
        ++count;
    }
    list->head->prev = cur_node;
    cur_node->next = list->head;

    kaps_sassert(count == list->count);

    return KAPS_OK;
}

static void
ftt_sort_entries_in_nodes(
    struct fast_test_trie_node *cur_node)
{
    if (!cur_node)
        return;

    /*
     * Use merge sort to sort all the entries present in the node - highest
     * priority (lowest numeric value) first
     */

    ftt_merge_sort(&cur_node->entry_list);

    if (cur_node->zero_child)
    {
        ftt_sort_entries_in_nodes(cur_node->zero_child);
    }

    if (cur_node->one_child)
    {
        ftt_sort_entries_in_nodes(cur_node->one_child);
    }
}

static uint32
ftt_split_node(
    struct fast_test_trie_node *node,
    uint8 *visited,
    uint32 nbytes_valid,
    uint8 *gmask,
    uint32 *o_chosen_bit_pos)
{
    int32 chosen_bit_pos = -1;
    uint32 max_unvisited_value = 0;
    uint32 cur_unvisited_value = 0;
    double cur_ratio, highest_ratio = -1.0;
    struct kaps_c_list_iter entry_it;
    struct kaps_list_node *entry_el;
    struct bit_count count[KAPS_HW_MAX_DBA_WIDTH_1];
    uint32 i;
    static uint32 id = 1;

    if (node->entry_list.count <= MIN_ENTRIES_PER_NODE_TO_SPLIT)
    {
        *o_chosen_bit_pos = -1;
        return KAPS_OK;
    }

    kaps_memset(&count, 0, sizeof(count));

    if (node->trie->db_type == KAPS_DB_ACL)
    {
        kaps_c_list_iter_init(&node->entry_list, &entry_it);
        while ((entry_el = kaps_c_list_iter_next(&entry_it)) != NULL)
        {
            struct fast_test_trie_record *t_rec = ENTRYLIST_TO_TEST_RECORD(entry_el);
            ftt_get_bit_count(t_rec->entry, nbytes_valid, count, gmask);
        }

        for (i = 0; i < nbytes_valid * KAPS_BITS_IN_BYTE; ++i)
        {
            if (visited[i] == 0)
            {
                cur_unvisited_value = count[i].ones + count[i].zeros;
                if (cur_unvisited_value > max_unvisited_value)
                {
                    max_unvisited_value = cur_unvisited_value;
                }
            }
        }

        if (max_unvisited_value > 0)
        {
            for (i = 0; i < nbytes_valid * KAPS_BITS_IN_BYTE; ++i)
            {
                if (visited[i] == 0)
                {
                    cur_unvisited_value = count[i].ones + count[i].zeros;
                    if (cur_unvisited_value + MAX_NUM_TOLERABLE_EXTRA_DC_ENTRIES >= max_unvisited_value)
                    {
                        if (count[i].zeros == 0 || count[i].ones == 0)
                        {
                            cur_ratio = 0;
                        }
                        else if (count[i].zeros < count[i].ones)
                        {
                            cur_ratio = count[i].zeros / count[i].ones;
                        }
                        else
                        {
                            cur_ratio = count[i].ones / count[i].zeros;
                        }
                        if (highest_ratio < cur_ratio)
                        {
                            highest_ratio = cur_ratio;
                            chosen_bit_pos = i;
                        }
                    }
                }
            }
        }

    }
    else
    {
        /*
         * For LPM and EM tables, pick the bit corresponding to the node depth. So we will pick bit 0 at the first
         * level, bit 1 at the second level and so on. Also assume that the number of zeroes and number ones are equal
         * at the chosen bit position. This helps in avoiding counting the number of zeros and ones at each level of
         * the trie and speeds up the trie. Note that count array is only used to decide to create nodes or not. So if
         * we guess incorrectly the only side effect is that we create utmost an extra node for each split operation 
         */
        chosen_bit_pos = node->depth;
        count[chosen_bit_pos].zeros = node->entry_list.count / 2;
        count[chosen_bit_pos].ones = node->entry_list.count / 2;
    }

    if (chosen_bit_pos == -1)
    {
        *o_chosen_bit_pos = chosen_bit_pos;
        return KAPS_OK;
    }

    if (count[chosen_bit_pos].zeros)
    {
        node->zero_child = kaps_syscalloc(1, sizeof(struct fast_test_trie_node));
        if (node->zero_child == NULL)
            return KAPS_OUT_OF_MEMORY;
        node->zero_child->parent = node;
        node->zero_child->depth = node->depth + 1;
        node->zero_child->id = id++;
        node->zero_child->trie = node->trie;
        kaps_c_list_reset(&node->zero_child->entry_list);

        kaps_memcpy(node->zero_child->data, node->data, nbytes_valid);
        kaps_memcpy(node->zero_child->mask, node->mask, nbytes_valid);
        kaps_array_set_bit(node->zero_child->data, chosen_bit_pos, 0);
        kaps_array_set_bit(node->zero_child->mask, chosen_bit_pos, 0);
    }

    if (count[chosen_bit_pos].ones)
    {
        node->one_child = kaps_syscalloc(1, sizeof(struct fast_test_trie_node));
        if (node->one_child == NULL)
            return KAPS_OUT_OF_MEMORY;
        node->one_child->parent = node;
        node->one_child->depth = node->depth + 1;
        node->one_child->id = id++;
        node->one_child->trie = node->trie;
        kaps_c_list_reset(&node->one_child->entry_list);

        kaps_memcpy(node->one_child->data, node->data, nbytes_valid);
        kaps_memcpy(node->one_child->mask, node->mask, nbytes_valid);
        kaps_array_set_bit(node->one_child->data, chosen_bit_pos, 1);
        kaps_array_set_bit(node->one_child->mask, chosen_bit_pos, 0);
    }

    kaps_c_list_iter_init(&node->entry_list, &entry_it);

    while ((entry_el = kaps_c_list_iter_next(&entry_it)) != NULL)
    {
        struct fast_test_trie_record *t_rec = ENTRYLIST_TO_TEST_RECORD(entry_el);
        enum kaps_bit_type bit_val;
        uint32 index = chosen_bit_pos / KAPS_BITS_IN_BYTE;
        uint32 bit = 7 - (chosen_bit_pos - (index * KAPS_BITS_IN_BYTE));
        if (gmask && (gmask[index] & (1 << bit)))
        {
            bit_val = KAPS_DONT_CARE;
        }
        else
        {
            bit_val = kaps_array_get_bit(t_rec->entry->data, t_rec->entry->mask, chosen_bit_pos);
        }
        if (bit_val == KAPS_ZERO)
        {
            kaps_c_list_remove_node(&node->entry_list, entry_el, &entry_it);
            kaps_c_list_add_tail(&node->zero_child->entry_list, entry_el);
        }
        else if (bit_val == KAPS_ONE)
        {
            kaps_c_list_remove_node(&node->entry_list, entry_el, &entry_it);
            kaps_c_list_add_tail(&node->one_child->entry_list, entry_el);
        }
    }

    *o_chosen_bit_pos = chosen_bit_pos;
    return KAPS_OK;
}

static uint32
ftt_build_trie_internal(
    struct fast_test_trie_node *cur_node,
    uint8 *visited,
    uint32 nbytes_valid,
    uint8 *gmask)
{
    uint32 chosen_bit_pos = 0;

    KAPS_STRY(ftt_split_node(cur_node, visited, nbytes_valid, gmask, &chosen_bit_pos));

    if (chosen_bit_pos == -1)
    {
        return KAPS_OK;
    }

    visited[chosen_bit_pos] = 1;

    if (cur_node->zero_child)
    {
        ftt_build_trie_internal(cur_node->zero_child, visited, nbytes_valid, gmask);
    }

    if (cur_node->one_child)
    {
        ftt_build_trie_internal(cur_node->one_child, visited, nbytes_valid, gmask);
    }

    visited[chosen_bit_pos] = 0;

    return KAPS_OK;
}

static uint32
ftt_does_key_match(
    const uint8 *data,
    const uint8 *mask,
    const uint8 *key,
    uint32 nbytes_valid,
    uint8 *gmask)
{
    uint32 is_match = 1;
    int32 i;
    if (gmask)
    {
        for (i = (nbytes_valid - 1); i >= 0; --i)
        {
            if ((data[i] & ~mask[i]) != (key[i] & ~mask[i]) && (data[i] & ~gmask[i]) != (key[i] & ~gmask[i]))
            {
                is_match = 0;
                break;
            }
        }
    }
    else
    {
        for (i = (nbytes_valid - 1); i >= 0; --i)
        {
            if ((data[i] & ~mask[i]) != (key[i] & ~mask[i]))
            {
                is_match = 0;
                break;
            }
        }
    }
    return is_match;
}

static void
ftt_find_max_priority_in_node(
    struct fast_test_trie_node *cur_node,
    const uint8 *key,
    uint32 nbytes_valid,
    uint8 *gmask,
    uint32 all_matches,
    uint32 *count,
    uint32 *max_priority,
    struct kaps_parse_record **result)
{
    struct kaps_c_list_iter entry_it;
    struct kaps_list_node *entry_el;
    uint32 is_match;

    kaps_c_list_iter_init(&cur_node->entry_list, &entry_it);
    while ((entry_el = kaps_c_list_iter_next(&entry_it)) != NULL)
    {
        struct fast_test_trie_record *t_rec = ENTRYLIST_TO_TEST_RECORD(entry_el);

        /*
         * If the entry has not been added to the database OR has been added to db
         * and then deleted, then visited will be 0
         */

        if (t_rec->entry->status != FAST_TRIE_ACTIVE && t_rec->entry->status != FAST_TRIE_MAY_BE_ACTIVE)
        {
            continue;
        }

        /*
         * Since the entries are sorted based on priority, we can break
         * out as soon as we get a priority with higher
         * numeric value if we want first priority match
         */
        if (t_rec->entry->priority > *max_priority)
        {
            if (all_matches == 0)
                break;
        }

        is_match = ftt_does_key_match(t_rec->entry->data, t_rec->entry->mask, key, nbytes_valid, gmask);
        if (is_match)
        {
#if 0
            int32 i;

            for (i = 0; i < t_rec->entry->num_ranges; i++)
            {
                uint16 rvalue = acl_utils_get_range_from_key(key, t_rec->entry->ranges[i].offset_8);
                if (rvalue < t_rec->entry->ranges[i].lo || rvalue > t_rec->entry->ranges[i].hi)
                {
                    is_match = 0;
                }
            }
#endif
        }

        if (is_match)
        {
            if (t_rec->entry->priority < *max_priority)
            {
                *max_priority = t_rec->entry->priority;
                if (all_matches == 0)
                {
                    *result = t_rec->entry;
                    /*
                     * Since the entries are sorted based on priority, we can break
                     * out as soon as we get a priority with higher
                     * numeric value
                     */

                    break;
                }
            }

            if (all_matches)
            {
                if ((*count) >= FTT_MAX_POSSIBLE_MATCHES)
                    break;
                result[(*count)] = t_rec->entry;
                (*count)++;
            }

        }
    }
}

static void
ftt_find_max_matching_priority(
    struct fast_test_trie *test_trie_p,
    const uint8 *key,
    uint8 *gmask,
    uint32 nbytes_valid,
    uint32 all_matches,
    struct kaps_parse_record **result)
{
    struct kaps_c_list matching_node_list;
    struct kaps_list_node *cur_node_el, *head_node_el;
    uint32 max_priority = -1;
    uint32 is_head_processed;
    uint32 count = 0;

    kaps_c_list_reset(&matching_node_list);
    kaps_c_list_add_head(&matching_node_list, &test_trie_p->root->m_node);

    cur_node_el = &test_trie_p->root->m_node;
    head_node_el = &test_trie_p->root->m_node;
    is_head_processed = 0;
    while (cur_node_el != head_node_el || !is_head_processed)
    {
        struct fast_test_trie_node *cur_node = NODELIST_TO_TEST_NODE(cur_node_el);
        uint32 is_match = ftt_does_key_match(cur_node->data, cur_node->mask, key,
                                             nbytes_valid, gmask);

        if (is_match)
        {
            ftt_find_max_priority_in_node(cur_node, key, nbytes_valid,
                                          gmask, all_matches, &count, &max_priority, result);

            if (cur_node->zero_child)
            {
                kaps_c_list_add_tail(&matching_node_list, &cur_node->zero_child->m_node);
            }
            if (cur_node->one_child)
            {
                kaps_c_list_add_tail(&matching_node_list, &cur_node->one_child->m_node);
            }
        }

        if (cur_node_el == head_node_el)
        {
            is_head_processed = 1;
        }

        cur_node_el = cur_node_el->next;
    }

}

static void
ftt_destroy_trie_internal(
    struct fast_test_trie_node *cur_node)
{
    struct kaps_c_list_iter entry_it;
    struct kaps_list_node *entry_el;
    struct fast_test_trie_record *t_rec;

    if (cur_node)
    {
        if (cur_node->zero_child)
        {
            ftt_destroy_trie_internal(cur_node->zero_child);
        }

        if (cur_node->one_child)
        {
            ftt_destroy_trie_internal(cur_node->one_child);
        }

        kaps_c_list_iter_init(&cur_node->entry_list, &entry_it);
        while ((entry_el = kaps_c_list_iter_next(&entry_it)) != NULL)
        {
            t_rec = ENTRYLIST_TO_TEST_RECORD(entry_el);
            kaps_c_list_remove_node(&cur_node->entry_list, entry_el, &entry_it);
            kaps_sysfree(t_rec);
        }

        kaps_sysfree(cur_node);
    }
}

kaps_status
fast_test_trie_build(
    struct kaps_parse_record *e,
    uint32 num_entries,
    enum kaps_db_type db_type,
    uint32 nbytes_valid,
    uint8 *gmask,
    struct fast_test_trie **test_trie_pp)
{
    struct fast_test_trie *test_trie_p = kaps_syscalloc(1, sizeof(struct fast_test_trie));
    uint32 i = 0;
    uint8 visited[KAPS_HW_MAX_DBA_WIDTH_1];

    *test_trie_pp = NULL;
    kaps_memset(visited, 0, sizeof(visited));

    test_trie_p->db_type = db_type;

    test_trie_p->root = kaps_syscalloc(1, sizeof(struct fast_test_trie_node));
    test_trie_p->root->trie = test_trie_p;
    kaps_c_list_reset(&test_trie_p->root->entry_list);

    kaps_memset(test_trie_p->root->data, 0, KAPS_HW_MAX_DBA_WIDTH_8);
    kaps_memset(test_trie_p->root->mask, 0xFF, KAPS_HW_MAX_DBA_WIDTH_8);

    for (i = 0; i < num_entries; ++i)
    {
        struct fast_test_trie_record *new_rec;
        kaps_sassert(e->status != FAST_TRIE_INVALID);
        new_rec = kaps_syscalloc(1, sizeof(struct fast_test_trie_record));
        if (!new_rec)
        {
            kaps_sysfree(test_trie_p);
            return KAPS_OUT_OF_MEMORY;
        }
        new_rec->entry = e;
        kaps_c_list_add_tail(&test_trie_p->root->entry_list, &new_rec->e_node);
        ++e;
    }

    KAPS_STRY(ftt_build_trie_internal(test_trie_p->root, visited, nbytes_valid, gmask));

    ftt_sort_entries_in_nodes(test_trie_p->root);

    *test_trie_pp = test_trie_p;

    return KAPS_OK;
}

kaps_status
fast_test_trie_search(
    struct fast_test_trie * test_trie_p,
    const uint8 *keys,
    uint32 nbytes_valid,
    uint8 *g_mask,
    uint32 all_matches,
    struct kaps_parse_record ** match_records)
{

    ftt_find_max_matching_priority(test_trie_p, keys, g_mask, nbytes_valid, all_matches, match_records);
    return KAPS_OK;
}

kaps_status
fast_test_trie_destroy(
    struct fast_test_trie * test_trie_p)
{
    ftt_destroy_trie_internal(test_trie_p->root);
    kaps_sysfree(test_trie_p);
    return KAPS_OK;
}
