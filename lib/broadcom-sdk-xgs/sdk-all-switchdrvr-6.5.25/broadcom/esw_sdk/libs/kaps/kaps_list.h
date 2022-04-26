/*******************************************************************************
 *
 * Copyright 2011-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __KAPS_LIST_H
#define __KAPS_LIST_H

#include "kaps_portable.h"
#include "kaps_utility.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 *
 * @file list.h
 *
 * Linked list management
 *
 */

/**
 * Generate the parent member address from inner pointer
 */
#define CONTAINER_OF(ptr, type, member) (type *)((char *)ptr - offsetof(type, member))

/**
 * @brief A node in a doubly linked list
 */

    struct kaps_list_node
    {
        struct kaps_list_node *next;
                                  /**< Next element in the list */
        struct kaps_list_node *prev;
                                  /**< Previous element in the list */
        /*
         * data 
         */
    };

/**
 * @brief Circular linked list definition
 */

    struct kaps_c_list
    {
        struct kaps_list_node *head;
                                  /**< head of circular linked list */
        uint32_t count;          /**< count of elements in linked list */
    };

/**
 * @brief Circular linked list iterator
 */

    struct kaps_c_list_iter
    {
        struct kaps_list_node *head;
                                 /**< Head of the list iterating */
        struct kaps_list_node *cur;
                                 /**< current node in iterator */
        uint32_t active;        /**< iteration has started */
    };

/**
 * Initialize a circular linked list to reset state
 *
 * @param l pointer to valid circular list descriptor memory
 */

    static KAPS_INLINE void kaps_c_list_reset(
    struct kaps_c_list *l)
    {
        l->head = NULL;
        l->count = 0;
    }

/**
 * Number of elements in circular linked list
 */
#define kaps_c_list_count(n) ((n)->count)

/**
 * Return head of the list
 */

#define kaps_c_list_head(l) ((l)->head)

/**
 * Insert the linked list node before the specified node in the circular list
 *
 * @param l valid pointer to circular linked list descriptor
 * @param node the specified node is inserted before this node
 * @param newnode the node to insert into the list
 */

    static KAPS_INLINE void kaps_c_list_insert_before(
    struct kaps_c_list *l,
    struct kaps_list_node *node,
    struct kaps_list_node *newnode)
    {
        if (l->count > 0)
        {
            newnode->next = node;
            newnode->prev = node->prev;
            newnode->prev->next = newnode;
            node->prev = newnode;

            if (l->head == node)
            {
                l->head = newnode;
            }

        }
        else
        {
            l->head = newnode;
            newnode->next = newnode;
            newnode->prev = newnode;
        }

        l->count++;
    }

/**
 * Insert the specified node to head of the circular linked list
 *
 * @param l valid pointer to circular linked list descriptor
 * @param n the node to add to the head
 */

    static KAPS_INLINE void kaps_c_list_add_head(
    struct kaps_c_list *l,
    struct kaps_list_node *n)
    {
        kaps_c_list_insert_before(l, l->head, n);
    }

/**
 * Insert the specified node to tail of the circular linked list
 *
 * @param l valid pointer to circular linked list descriptor
 * @param n the node to add to the tail
 */

    static KAPS_INLINE void kaps_c_list_add_tail(
    struct kaps_c_list *l,
    struct kaps_list_node *n)
    {
        if (l->count > 0)
        {
            n->next = l->head;
            n->prev = l->head->prev;
            n->prev->next = n;
            l->head->prev = n;

        }
        else
        {
            l->head = n;
            n->next = n;
            n->prev = n;
        }

        l->count++;
    }

/**
 * Delete the specified node from the circular linked list
 * If an active iterator is being used, then the iterator
 * must be passed in so that remove works concurrently
 * with iteration
 *
 * @param l valid pointer to circular linked list descriptor
 * @param n the node to delete from list
 * @param it if non null the iterator is updated as the node is removed
 */

    static KAPS_INLINE void kaps_c_list_remove_node(
    struct kaps_c_list *l,
    struct kaps_list_node *n,
    struct kaps_c_list_iter *it)
    {
        kaps_sassert(l->count > 0);
        if (n != n->next)
        {
            n->next->prev = n->prev;
            n->prev->next = n->next;
            if (n == l->head)
                l->head = l->head->next;
            if (it)
            {
                if (it->cur == n)
                    it->cur = n->next;
                if (it->head == n)
                {
                    it->head = l->head;
                    /*
                     * Don't stop iteration if we just deleted head,
                     * and cur is pointing to next node
                     */
                    if (it->head == it->cur)
                        it->active = 0;
                }
            }
        }
        else
        {
            l->head = NULL;
            if (it)
            {
                it->head = it->cur = NULL;
            }
        }
        l->count--;
        n->next = n->prev = NULL;
    }

/**
 * Concatenate the second list into the first circular list
 *
 * @param first the first circular linked list into which the second is concatenated
 * @param second the circular linked list concatenated into first
 */

    static KAPS_INLINE void kaps_c_list_concat(
    struct kaps_c_list *first,
    struct kaps_c_list *second)
    {
        if (second->count == 0)
            return;

        if (first->count == 0)
        {
            first->count = second->count;
            first->head = second->head;
        }
        else
        {
            struct kaps_list_node *new_tail = second->head->prev;
            struct kaps_list_node *old_tail = first->head->prev;
            new_tail->next = first->head;
            old_tail->next = second->head;

            first->head->prev = new_tail;
            second->head->prev = old_tail;
            first->count += second->count;
        }
        kaps_c_list_reset(second);
    }

/**
 * Concatenate N elements of the second list into the first circular list
 *
 * @param first the first circular linked list into which the second is concatenated
 * @param second the circular linked list concatenated into first
 * @param nelements the number of elements from second list to add to first list
 *
 * The elements get deleted from the second list after they get added to the
 * first list
 */

    static KAPS_INLINE void kaps_c_list_concat_num(
    struct kaps_c_list *first,
    struct kaps_c_list *second,
    uint32_t nelements)
    {
        struct kaps_list_node *n, *sub_list_head, *sub_list_tail, *sec_tail;
        uint32_t count;

        if (nelements == 0)
            return;

        if (nelements == second->count)
        {
            kaps_c_list_concat(first, second);
            return;
        }

        kaps_sassert(second->count > nelements);

        count = 0;
        n = sub_list_head = second->head;
        while (count < nelements)
        {
            n = n->next;
            count++;
        }

        sec_tail = second->head->prev;
        sub_list_tail = n->prev;

        /*
         * The second list will contain the remaining nodes 
         */
        sec_tail->next = n;
        n->prev = sec_tail;
        second->head = n;
        second->count -= nelements;

        /*
         * Sub list is added to first 
         */
        if (first->count)
        {
            first->head->prev->next = sub_list_head;
            sub_list_head->prev = first->head->prev;
            sub_list_tail->next = first->head;
            first->head->prev = sub_list_tail;
        }
        else
        {
            first->head = sub_list_head;
            sub_list_head->prev = sub_list_tail;
            sub_list_tail->next = sub_list_head;
        }

        first->count += nelements;
    }

/**
 * Initialize the iterator to iterate through the circular
 * list elements
 *
 * @param l valid circular linked list
 * @param it valid pointer to iterator memory initialized by call
 */

    static KAPS_INLINE void kaps_c_list_iter_init(
    const struct kaps_c_list *l,
    struct kaps_c_list_iter *it)
    {
        it->head = l->head;
        it->cur = l->head;
        it->active = 0;
    }

/**
 * Initialize the iterator at a specific element in the list
 *
 * @param l valid circular linked list
 * @param el the element to start the iterator at
 * @param it valid pointer to iterator memory initialized by call
 */

    static KAPS_INLINE void kaps_c_list_iter_init_at_element(
    const struct kaps_c_list *l,
    struct kaps_list_node *el,
    struct kaps_c_list_iter *it)
    {
        it->head = l->head;
        it->cur = el;
        it->active = 0;
    }

/**
 * Obtain the next node in the list. Returns a valid node or a NULL
 * pointer at end of list
 *
 *  @param it valid iterator initialized using kaps_c_list_iter_init()
 */

    static KAPS_INLINE struct kaps_list_node *kaps_c_list_iter_next(
    struct kaps_c_list_iter *it)
    {
        if (it->cur)
        {
            if (it->cur != it->head || !it->active)
            {
                struct kaps_list_node *res = it->cur;
                it->cur = it->cur->next;
                it->active = 1;
                return res;
            }
        }

        return NULL;
    }

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif

#endif /* __LIST_H */
