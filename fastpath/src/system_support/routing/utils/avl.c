/******************************************************************** * <pre>
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      avl.c 
 *
 * @purpose       AVL and AVLH tree object implementation
 *
 * @component     Routing Utils Component
 *
 * @comments     
 * AVL(AVLH)_Init       init AVL tree object
 * AVL(AVLH)_Destroy    destroy AVL tree object
 * AVL(AVLH)_Insert     insert a new entry into AVL tree
 * AVL(AVLH)_Find       find entry 
 * AVL(AVLH)_Delete     delete entry 
 * AVL(AVLH)_GetEntriesNmb get number of entries in tree
 * AVL(AVLH)_Cleanup    deallocate AVL node entries
 * AVL(AVLH)_GetFirst   get first entry (lowest key) of AVL tree 
 * AVL(AVLH)_GetNext    get next entry 
 * AVL(AVLH)_Browse     Traverse entire AVL tree
 * AVL(AVLH)_Change     change AVL entry's value 
 * AVL_FindNext         get next entry based on a key.
 * AVL_InsertFind       Insert entry or return matching entry
 *
 * Internal static routines:
 * avlFind
 * avlFindNext
 * avlInsert
 * avlDelete
 * avlDeleteAll
 * avlRebalance
 * avlRemoveNode
 * avlRemoveMax
 * avlRemoveMin
 * avlPred
 * avlSucc
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] =
    "$Header: lvl7dev\src\system_support\routing\utils\avl.c 1.1.3.1 2002/02/13 23:05:16EST ssaraswatula Development  $";
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "std.h"
#include "xx.ext"
#include "common.h"
#include "iptypes.h"
#include "gservice.ext"
#include "datatypes.h"

#include "avl.ext"
#include "avl.h"

/* static functions */
static t_AvlNode *avlFind(t_AvlTree *p_tree, byte *key, Bool besMatch_flag);
static t_AvlNode *avlFindNext(t_AvlTree *p_tree, byte *key);
static e_Err      avlInsert(t_AvlTree *p_tree, t_AvlNode *p_newNode, 
                            t_AvlNode **pp_found_node);
static e_Err      avlDelete(t_AvlTree *p_tree, byte *key, void **dataptr);
static void       avlDeleteAll(t_AvlTree *p_tree, Bool freeEntry);
static t_AvlNode *avlRebalance(t_AvlNode *p_subTreeNode, Bool *htReduced);
static t_AvlNode *avlRemoveNode(t_AvlNode **pp_node, Bool *htReduced);
static t_AvlNode *avlRemoveMax(t_AvlNode *p_node, t_AvlNode **maxnode, 
                               Bool *htReduced);
static t_AvlNode *avlRemoveMin(t_AvlNode *p_node, t_AvlNode **minnode, 
                               Bool *htReduced);
static t_AvlNode *avlPred(t_AvlNode *p_node);
static t_AvlNode *avlSucc(t_AvlNode *p_node);

/* Convert mask length (0 - 32) to bit mask */
static ulng num_2_mask[] = 
{
    0x0,
    0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
    0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
    0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
    0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
    0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
    0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
    0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
    0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff
};

static ulng avl_debug = 0;

/* ************************************************************************
 * Enable this extra debug code to print at all the entries in a tree
 * before the AVL function is performed.  
 * Control printing by setting AVLDebugSet().
 * ************************************************************************/
#ifdef AVL_EXTRA_DEBUG
#define DEBUG_AVL_WALKTREE(tree) if (avl_debug) avlWalkTree(tree)

static
void print_avl_node(t_AvlNode *p_cur)
{
    printf("%p: bf %2d, Lptr: %p, Rptr %p (%c, %c) data %p, sll %p\n", 
        p_cur, p_cur->balFactor, p_cur->Lptr, p_cur->Rptr,
        LLINK(p_cur) ? 'L' : 'T',
        RLINK(p_cur) ? 'L' : 'T',
        p_cur->dataptr,
        p_cur->sll);
}
static
void avlWalkSubTree(t_AvlNode *p_cur)
{
    if (p_cur == NULL)
        return;

    if (LLINK(p_cur))
        avlWalkSubTree(p_cur->Lptr);
    print_avl_node(p_cur);
    if (RLINK(p_cur) && !IS_HEAD(p_cur))
        avlWalkSubTree(p_cur->Rptr);
}

static
void print_sll_list(t_AvlTree *p_tree)
{
    t_AvlNode *p_cur;
    int        cnt = 0;

    p_cur = p_tree->sllhead;
    while (p_cur && cnt++ < 32)
    {
        print_avl_node(p_cur);
        p_cur = p_cur->sll;
    }
}
static
void avlWalkTree(t_AvlTree *p_tree)
{
    t_AvlNode *p_cur;
    
    p_cur = p_tree->head;
    printf("Head node: %p cnt %lu\n", p_cur, p_tree->count);
    avlWalkSubTree(p_cur);
    if (avl_debug > 1)
    {
        printf("\n== In sll key order ===\n");
        print_sll_list(p_tree);
    }
}
#else
#define DEBUG_AVL_WALKTREE(tree)
#endif /* AVL_EXTRA_DEBUG */

/*********************************************************************
 * @purpose           Allocate an AVL node
 *
 * @param  p_avl      @b{(input)}  AVL tree
 * @param  bm_flag    @b{(input)}  using BM
 *
 * @returns           pointer allocated memory or NULL
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_AvlNode *
avlAllocNode(void *userdata)
{
    t_AvlNode *p_node;
    if ((p_node = (t_AvlNode *) XX_Malloc(sizeof(t_AvlNode))) != NULL)
    {
        memset(p_node, 0, sizeof(t_AvlNode));
        p_node->Lbit = THREAD;
        p_node->Lbit = THREAD;
        p_node->dataptr = userdata;
    }

    return p_node;
}

/*********************************************************************
 * @purpose           finds an entry in AVL using or not BM option.
 *
 *
 * @param  key        @b{(input)}  value to be found
 * @param  p_avl      @b{(input)}  AVL tree
 * @param  bm_flag    @b{(input)}  using BM
 *
 * @returns           pointer to found value or NULL
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_AvlNode *
avlFind(t_AvlTree *p_tree, byte *key, Bool bestMatch_flag)
{
    ulng      netmask = 0;
    ulng      curBestMatch_mask = 0;
    int       cmpval1;
    int       cmpval2 = 0;
    byte      *key1, *key1b;
    byte      *key2, *key2b;
    t_AvlNode *p_cur;
    t_AvlNode *p_curBestMatch = NULL;

    DEBUG_AVL_WALKTREE(p_tree);    /* DEBUG version only */

    key1 = key;
    if (p_tree->FlgKeyDouble)
    {
        key1b = key + p_tree->keyLen / 2;
    }
    else
    {
        key1b = NULL;
    }
    AVL_DB_PRINT("avlFind: key1 = 0x%08lx, key1b = 0x%08lx\n",
                 *((ulng *)key1), 
                 key1b ? *((ulng *)key1b) : 0);

    p_cur = LEFTCHILD(p_tree->head);

    while (p_cur) 
    {
        key2 = GET_KEY(p_tree, p_cur);
        cmpval1 = KEY_COMPARE(p_tree, key1, key2);
        AVL_DB_PRINT("avlFind: key2 = 0x%08lx, cmpval1 = %d\n", 
                     *((ulng *)key2), cmpval1);
        key2b = NULL;
        if (cmpval1 == 0 && key1b)
        {
            key2b = GET_KEY2(p_tree, p_cur);
            cmpval2 = KEY_COMPARE2(p_tree, key1b, key2b);
            AVL_DB_PRINT("avl_find: key2b = 0x%08lx, cmpval2 = %d\n", 
                         *((ulng *)key2b), cmpval2);
        }

        if (bestMatch_flag)
        {
            if (!key2b)
            {
                /* Get key2b if we didn't already get it above */
                key2b = GET_KEY2(p_tree, p_cur);
            }

            netmask = GET_KEY_VALUE(p_tree, key2b);
            if (p_tree->FlgKey2IsNum && (netmask >> 24) <= 32)
                netmask = num_2_mask[netmask >> 24];
            if (!netmask)
                netmask = 0xffffffff;

            /* Is the search key masked with mask of key2 the same as
             * key2?  That is, using the same mask, are they the same?
             * (We assume the mask has already been applied to key2.)
             */
            if ((GET_KEY_VALUE(p_tree, key1) & netmask) == 
                 GET_KEY_VALUE(p_tree, key2))
            {
                /* Check if the mask is more specific than previous mask
                 * which matched.
                 */
                if (!p_curBestMatch || (curBestMatch_mask < netmask))
                {
                    /* Found a more specific match */
                    curBestMatch_mask = netmask;
                    p_curBestMatch = p_cur;
                }
            }
        }

        if (cmpval1 < 0)
        {
            p_cur = LEFTCHILD(p_cur);
        }
        else if (cmpval1 > 0)
        {
            p_cur = RIGHTCHILD(p_cur);
        }
        else if (cmpval2 < 0)
        {
            p_cur = LEFTCHILD(p_cur);
        }
        else if (cmpval2 > 0)
        {
            p_cur = RIGHTCHILD(p_cur);
        }
        else
        {
            /* Exact match found */
            break;
        }
    }

    if (bestMatch_flag && p_curBestMatch)
        return p_curBestMatch;
    else
        return (p_cur);
}


/*********************************************************************
 * @purpose           finds the next entry in a tree                
 *
 *
 * @param  p_tree     @b{(input)}  AVL tree
 * @param  key        @b{(input)}  value from which to find next
 *
 * @returns           pointer to found next value or NULL
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_AvlNode *
avlFindNext(t_AvlTree *p_tree, byte *key)
{
    int       cmpval1;
    int       cmpval2 = 0;
    byte      *key1, *key1b;
    byte      *key2, *key2b;
    t_AvlNode *p_cur;
    t_AvlNode *p_next;

    key1 = key;
    if (p_tree->FlgKeyDouble)
    {
        key1b = key + p_tree->keyLen / 2;
    }
    else
    {
        key1b = NULL;
        cmpval2 = 0;
    }
    AVL_DB_PRINT("avlFindNext: key1 = 0x%08lx, key1b = 0x%08lx\n",
                 *((ulng *)key1), key1b ? *((ulng *)key1b) : 0);

    p_cur = LEFTCHILD(p_tree->head);

    while (p_cur) 
    {
        key2 = GET_KEY(p_tree, p_cur);
        cmpval1 = KEY_COMPARE(p_tree, key1, key2);
        AVL_DB_PRINT("avlFindNext: key2 = 0x%08lx, cmpval1 = %d\n", 
                     *((ulng *)key2), cmpval1);
        if (cmpval1 == 0 && key1b)
        {
            key2b = GET_KEY2(p_tree, p_cur);
            cmpval2 = KEY_COMPARE2(p_tree, key1b, key2b);
            AVL_DB_PRINT("avlFindNext: key2b = 0x%08lx, cmpval2 = %d\n", 
                         *((ulng *)key2b), cmpval2);
        }

        if (cmpval1 < 0)
        {
            /* key is less than current tree node */
            if ((p_next = LEFTCHILD(p_cur)) == NULL)
            {
                /* no left sub-tree.  This node is the next key */
                break;
            }
            p_cur = p_next;
        }
        else if (cmpval1 > 0)
        {
            /* key is greater than current tree node */
            if ((p_next = RIGHTCHILD(p_cur)) == NULL)
            {
                /* no right sub-tree, find next of this node */
                p_cur = avlSucc(p_cur);
                break;
            }
            p_cur = p_next;
        }
        else if (cmpval2 < 0)
        {
            /* key is less than current tree node */
            if ((p_next = LEFTCHILD(p_cur)) == NULL)
            {
                /* no left sub-tree.  This node is the next key */
                break;
            }
            p_cur = p_next;
        }
        else if (cmpval2 > 0)
        {
            /* key is greater than current tree node */
            if ((p_next = RIGHTCHILD(p_cur)) == NULL)
            {
                /* no right sub-tree, find next of this node */
                p_cur = avlSucc(p_cur);
                break;
            }
            p_cur = p_next;
        }
        else
        {
            /* Exact match found.  Return the next node */
            AVL_DB_PRINT("exact match found");
            p_cur = avlSucc(p_cur);
            break;
        }
    }
   
   return (p_cur);
}

/*********************************************************************
 * @purpose               Add item to balanced tree.
 *
 *
 * @param  p_tree        @b{(input)}  AVL tree
 * @param  p_newNode     @b{(input)}  element to be inserted
 * @param  pp_found_node @b{(output)} pointer to existing node (E_IN_MATCH)
 *
 * @returns              E_IN_MATCH - if key already exists in tree.
 * @returns              E_OK - otherwise
 *
 * @notes
 *     Add item to balanced tree.  Search for place in tree, remembering
 *     the balance point (the last place where the tree balance was
 *     non-zero).  Insert the item, and then read just the balance factors
 *     starting at the balance point.  If the balance at the balance point
 *     is now +2 or -2, must shift to get the tree back in balance.  There
 *     are four cases: simple left shift, double left shift, and the right
 *     shift counterparts.
 *
 *     Build single link list (SLL) as new node is being inserted into tree.
 *
 * @end
 * ********************************************************************/
e_Err
avlInsert(t_AvlTree *p_tree, t_AvlNode *p_newNode, 
          t_AvlNode **pp_found_node)
{
    t_AvlNode *p_topUnBalNode;
    t_AvlNode *p_UnBalParent;
    t_AvlNode *p_cur, *p_prev;
    t_AvlNode *newTop;
    t_AvlNode *pred, *next;
    int       cmpval1;
    int       cmpval2 = 0;
    int       bal_side;        
    char      junk;
    byte      *key1, *key1b;
    byte      *key2, *key2b;
    int       dirIdx;
    byte      dirArray[RECUR_STACK_SIZE];

    DEBUG_AVL_WALKTREE(p_tree);    /* DEBUG version only */

    /*  Locate insertion point for item.  "p_topUnBalNode" keeps track of most
     *  recently seen node with (balFactor != 0) - or it is the top of the
     *  tree, if no nodes with (p_cur->balFactor != 0) are encountered.  
     *  "p_UnBalParent" is parent of "p_topUnBalNode".  "p_prev" follows 
     *  "p_cur" through tree.
     */

    p_prev = p_tree->head;   
    p_topUnBalNode = p_prev;  
    p_UnBalParent = NULL;  
    p_cur = LEFTCHILD(p_prev);

    /* Set pointers to new entries key(s) */
    key1 = GET_KEY(p_tree, p_newNode);
    if (p_tree->FlgKeyDouble)
    {
        key1b = GET_KEY2(p_tree, p_newNode);
    }
    else
    {
        key1b = NULL;
    }

    dirIdx = 0;
    dirArray[dirIdx++] = DIR_LEFT;

    while (p_cur)
    {
        if (p_cur->balFactor != BF_EVEN) 
        {
            /* Set new top unbalanced Node poiner */
            p_topUnBalNode = p_cur; 
            p_UnBalParent = p_prev; 
            dirIdx = 0;
        }

        p_prev = p_cur;

        key2 = GET_KEY(p_tree, p_cur);
        cmpval1 = KEY_COMPARE(p_tree, key1, key2);
        if (cmpval1 == 0 && key1b)
        {
            key2b = GET_KEY2(p_tree, p_cur);
            cmpval2 = KEY_COMPARE2(p_tree, key1b, key2b);
        }

        if (cmpval1 < 0)
        {
            p_cur = LEFTCHILD(p_cur);
            dirArray[dirIdx++] = DIR_LEFT;
        }
        else if (cmpval1 > 0)
        {
            p_cur = RIGHTCHILD(p_cur);
            dirArray[dirIdx++] = DIR_RIGHT;
        }
        else if (cmpval2 < 0)
        {
            p_cur = LEFTCHILD(p_cur);
            dirArray[dirIdx++] = DIR_LEFT;
        }
        else if (cmpval2 > 0)
        {
            p_cur = RIGHTCHILD(p_cur);
            dirArray[dirIdx++] = DIR_RIGHT;
        }
        else
        {
            if (pp_found_node) 
            {
                *pp_found_node = p_cur;
            }
            return E_IN_MATCH;
        }
    }

    /* wasn't found - create new node as child of p_prev */

    dirIdx--;
    if (dirArray[dirIdx] == DIR_LEFT)
    {
        /* connect to tree and thread it */
        p_newNode->Lptr = p_prev->Lptr;
        p_newNode->Rptr = p_prev;
        p_prev->Lbit = LINK;
        p_prev->Lptr = p_newNode;
    }
    else 
    {
        p_newNode->Rptr = p_prev->Rptr;
        p_newNode->Lptr = p_prev;
        p_prev->Rbit = LINK;
        p_prev->Rptr = p_newNode;
    }
    p_tree->count++;

    /* Update ordered singly linked list */
    pred = avlPred(p_newNode);
    if (pred == NULL)
    {
        /* New first on list */
        next = p_tree->sllhead;
        p_tree->sllhead = p_newNode;
        p_newNode->sll = next;
    }
    else
    {
        /* Add to the middle of the chain */
        next = pred->sll;
        p_newNode->sll = next;
        pred->sll = p_newNode;
    }

    /* Adjust balance factors on path from p_topUnBalNode to p_prev.  By 
     * definition of "p_topUnBalNode", all nodes on this path have 
     * balFactor = BF_EVEN, and so will change to BF_LEFT or BF_RIGHT.
     */

    if ((p_topUnBalNode == p_tree->head) || dirArray[0] == DIR_LEFT)
    {
        p_cur = p_topUnBalNode->Lptr; 
        bal_side = BF_LEFT;
    }
    else 
    {
        p_cur = p_topUnBalNode->Rptr; 
        bal_side = BF_RIGHT;
    }

    /* adjust balance factors */
    dirIdx = 1;
    while (p_cur != p_newNode) 
    {
        if (dirArray[dirIdx++] == DIR_LEFT) 
        {
            p_cur->balFactor = BF_LEFT;
            p_cur = p_cur->Lptr;
        }
        else 
        {
            p_cur->balFactor = BF_RIGHT;  
            p_cur = p_cur->Rptr;
        }
    }

    /* if p_topUnBalNode==tree->head, tree is already balanced */
    p_tree->head->balFactor = 0;     

    /* Is tree balanced? */
    p_topUnBalNode->balFactor += bal_side;
    if (abs(p_topUnBalNode->balFactor) < 2) 
        return E_OK;

    /* Tree needs to be rebalanced */
    newTop = avlRebalance(p_topUnBalNode, &junk);

    ASSERT(junk);               /* rebalance always sets junk to 1 */
    ASSERT(p_UnBalParent);      /* p_UnBalParent was set non-NULL by the search loop */

    if (p_UnBalParent->Rptr != p_topUnBalNode)
        p_UnBalParent->Lptr = newTop;
    else
        p_UnBalParent->Rptr = newTop;

    return E_OK;
}


/*********************************************************************
 * @purpose          Remove item from balanced tree.
 *
 *
 * @param key        @b{(input)}  to be deleted
 * @param pp_node    @b{(output)}  output pointer to item to be deleted
 * @param p_avl      @b{(input)}  AVL tree
 *
 * @returns          E_NOT_FOUND - not found
 * @returns          E_OK - otherwise.
 *
 * @notes
 *     Remove item from balanced tree. First, reduce to the case where the
 *     item being removed has a NULL right or left pointer. If this is not 
 *     the case, simply find the item that is just previous (in key order); 
 *     this is guaranteed to have a NULL right pointer. Then swap the 
 *     entries before deleting. After deleting, go up the stack adjusting 
 *     the balance when necessary.
 *
 *     We can stop whenevr the balance factor ceases to change.
 *
 *     If we are deleting the node pointed to by current, then find the 
 *     next node to use.  It doesn't matter if new nodes are later added 
 *     to the tree which keys are greater than the deleted node 
 *     but less than the current next node (i.e. a new better next 
 *     node that isn't looked at.) If nodes are added and deleted, 
 *     you will have to walk the tree again to get them all.
 *
 * @end
 * ********************************************************************/
e_Err
avlDelete(t_AvlTree *p_tree, byte *key, void **pp_data)
{
    Bool        rebalanced;
    Bool        htReduced;
    int         bal_side;
    int         cmpval1;
    int         cmpval2 = 0;
    byte        *key1, *key1b;
    byte        *key2, *key2b;
    Bool        found;
    t_AvlNode   *p_cur;
    t_AvlNode   *pred;
    t_AvlNode   *p_origDelNode;
    t_AvlNode   *p_savNode = NULL;
    t_AvlNode   *p_delNode;
    t_itemStack block[RECUR_STACK_SIZE];
    t_itemStack *next = block;   /* initialize recursion stack */

    DEBUG_AVL_WALKTREE(p_tree);    /* DEBUG version only */

    htReduced = FALSE;
    found = FALSE;
    p_cur = LEFTCHILD(p_tree->head);

    key1 = key;
    if (p_tree->FlgKeyDouble)
    {
        key1b = key + p_tree->keyLen / 2;
    }
    else
    {
        key1b = NULL;
    }

    /* prevent p_tree->head from being rebalanced */
    p_tree->head->balFactor = 0;      

    PUSH_PATH(p_tree->head, BF_LEFT, next);

    while (p_cur) 
    {
        key2 = GET_KEY(p_tree, p_cur);
        cmpval1 = KEY_COMPARE(p_tree, key1, key2);
        if (cmpval1 == 0 && key1b)
        {
            key2b = GET_KEY2(p_tree, p_cur);
            cmpval2 = KEY_COMPARE2(p_tree, key1b, key2b);
        }

        if (cmpval1 < 0)
        {
            PUSH_PATH(p_cur, BF_LEFT, next);
            p_cur = LEFTCHILD(p_cur);
        }
        else if (cmpval1 > 0)
        {
            PUSH_PATH(p_cur, BF_RIGHT, next);
            p_cur = RIGHTCHILD(p_cur);
        }
        else if (cmpval2 < 0)
        {
            PUSH_PATH(p_cur, BF_LEFT, next);
            p_cur = LEFTCHILD(p_cur);
        }
        else if (cmpval2 > 0)
        {
            PUSH_PATH(p_cur, BF_RIGHT, next);
            p_cur = RIGHTCHILD(p_cur);
        }
        else
        {
            /* cmpval1 == cmpval2 == 0 */ 
            p_savNode = p_cur;
            p_cur = NULL;
            found = TRUE;
        }
    } /* end while(p_cur) */

    if (!found) 
    {
        return E_NOT_FOUND;
    }

    if (pp_data)
    {
       *pp_data = p_savNode->dataptr;  /* output to caller pointer to node */
    }
    
    p_delNode = p_savNode;
    p_origDelNode = p_savNode;
    pred = avlPred(p_savNode);
    p_savNode = avlRemoveNode(&p_delNode, &htReduced);
    p_tree->count--;

    /* Fix the sll list and the current/next pointers.
     */

    /* If next pointer node is being deleted or changed, adjust pointer */
    if (p_tree->next == p_origDelNode && pred == p_delNode)
    {
        /* p_savNode is the pred of the original node */
        p_tree->next = avlSucc(p_savNode);
    }
    else if (p_tree->next == p_delNode ||
             p_tree->next == p_origDelNode)
    {
        p_tree->next = p_savNode;
    }

    if (p_delNode == p_origDelNode)
    {
        if (pred == NULL)
        {
            /* Remove from the head of the list */
            ASSERT(p_tree->sllhead == p_delNode);
            p_tree->sllhead = p_delNode->sll;
        }
        else
        {
            /* Remove from the middle or end of the chain */
            ASSERT(pred->sll == p_delNode);
            pred->sll = p_delNode->sll;
        }
        if (p_tree->current == p_origDelNode)
        {
            p_tree->current = NULL;
            p_tree->next = p_delNode->sll;
        }
    }
    else if (p_delNode == pred)
    {
        /* The node before the original node to be delete is to be deleted */
        ASSERT(p_savNode == p_origDelNode && p_delNode->sll == p_savNode);
        pred = avlPred(p_savNode);
        if (pred == NULL)
        {
            p_tree->sllhead = p_savNode;
        }
        else
        {
            pred->sll = p_savNode;
        }
        if (p_tree->current == p_origDelNode ||
            p_tree->current == p_delNode)
        {
            p_tree->current = p_savNode;;
        } 
    }
    else if (p_savNode->sll == p_delNode)
    {
        /* The node after the original node to be delete is to be deleted */
        ASSERT(p_savNode == p_origDelNode && p_savNode->sll == p_delNode);
        ASSERT(pred == NULL || pred->sll == p_savNode);
        p_savNode->sll = p_delNode->sll;
        if (p_tree->current == p_origDelNode)
        {
            p_tree->current = NULL;
            p_tree->next = p_savNode; /* 2,2 */
        }
        else if (p_tree->current == p_delNode)
        {
            p_tree->current = NULL;
            p_tree->next = p_delNode->sll;
        }
    }
    else
        ASSERT(0);

    /* Update balance factor */
    do 
    {
        p_cur = POP_PATH(bal_side, next);

        if (bal_side != BF_RIGHT)
        {
            p_cur->Lptr = p_savNode;
        }
        else
        {
            p_cur->Rptr = p_savNode;
        }

        p_savNode = p_cur;  
        rebalanced = FALSE;

        if (htReduced) 
        {
            p_cur->balFactor -= bal_side;
            switch (p_cur->balFactor) 
            {
                case BF_EVEN:     
                    /* longest side shrank to equal shortest, therefore
                     * htReduced remains true */
                    break;  

                case BF_LEFT:
                case BF_RIGHT: 
                    htReduced = FALSE;/* other side is deeper */
                    break;

                default:    
                    /* BF_LEFT+BF_LEFT or BF_RIGHT+BF_RIGHT */
                    p_savNode = avlRebalance(p_cur, &htReduced);
                    rebalanced = TRUE;
                    break;
            }
        }
    } while ((p_cur != p_tree->head) && (rebalanced || htReduced));

    XX_Free(p_delNode);

    return E_OK;

} /* avlDelete */


/*********************************************************************
 * @purpose        
 *
 * @param  pp_node     @b{(input)}  address of ptr to node to deallocate
 * @param  htReduced   @b{(input)}  was the height reduced?
 *
 * @returns            ptr to replacement node
 *
 * @notes
 *     The original node to be removed may not be the one to be
 *     marked for deletion.  In pp_node, return to the caller the
 *     actual node to be deleted.
 *
 * @end
 * ********************************************************************/
static t_AvlNode *
avlRemoveNode(t_AvlNode **pp_node, Bool *htReduced)
{
    Bool      deltaHt;
    t_AvlNode *q;
    t_AvlNode *p_node;

    p_node = *pp_node;
    *htReduced = FALSE;

    if (p_node->balFactor != BF_LEFT) 
    {
        /* Balance Factor is BF_EVEN or BF_RIGHT */
        if (RLINK(p_node)) 
        {
            p_node->Rptr = avlRemoveMin(p_node->Rptr, &q, &deltaHt);
            if (deltaHt) 
            {
                p_node->balFactor += BF_LEFT;  /* becomes 0 or BF_LEFT */
                *htReduced = (p_node->balFactor) ? FALSE : TRUE;
            }
        }
        else 
        {
            /* leftchild(p_node), rightchild(p_node) == NULL */
            ASSERT(p_node->balFactor == BF_EVEN);
            ASSERT(LTHREAD(p_node));

            /* p_node will be removed, so height changes */
            *htReduced = TRUE;           

            if (p_node->Rptr->Lptr == p_node) 
            {
                /* p_node is leftchild of it's parent */
                p_node->Rptr->Lbit = THREAD;
                q = p_node->Lptr;
            }
            else 
            {
                /* p_node is rightchild of it's parent */
                ASSERT(p_node->Lptr->Rptr == p_node);
                p_node->Lptr->Rbit = THREAD;
                q = p_node->Rptr;
            }
            return q;
        }
    }
    else 
    {
        /* p_node->balFactor == BF_LEFT */
        p_node->Lptr = avlRemoveMax(p_node->Lptr, &q, &deltaHt);
        if (deltaHt) 
        {
            p_node->balFactor += BF_RIGHT;      /* becomes 0 or BF_RIGHT */
            *htReduced = FALSE;
            if (p_node->balFactor == BF_EVEN)
                *htReduced = TRUE;
        }
    }

    p_node->dataptr = q->dataptr;
    *pp_node = q;

    return p_node;
}

/*********************************************************************
 * @purpose        Find the minimum value node for this sub-tree
 *
 * @param  p_node      @b{(input)}  pointer to the top sub-tree node
 * @param  minnode     @b{(output)} address of the ptr to the found 
 *                                  minimum node or NULL
 * @param  htReduced   @b{(output)} address of the boolean whether this
 *                                  procedure changed the height of the
 *                                  sub-tree
 *
 * @returns            ptr to top of new sub-tree
 *
 * @notes
 *                  If the current p_node is a leaf:
 *                  return the left or right thread. Change the parent's
 *                  appropriate link to a thread (to match the leaf's.)
 *                  Otherwise, return p_node.
 *
 * @end
 * ********************************************************************/
static t_AvlNode *
avlRemoveMin(t_AvlNode *p_node, t_AvlNode **minnode, Bool *htReduced)
{
    char deltaHt;
    int  idx = 0;
    t_AvlNode *p_returnNode;
    t_AvlNode *nodeStack[RECUR_STACK_SIZE];

    *htReduced = L7_FALSE;
    while (LLINK(p_node)) 
    {
        nodeStack[idx++] = p_node;
        p_node = LEFTCHILD(p_node);
    }

    /* p_node is minimum */
    *minnode = p_node;
    deltaHt = TRUE;
    if (RLINK(p_node)) 
    {
        ASSERT(p_node->Rptr->Lptr == p_node);
        ASSERT(LTHREAD(p_node->Rptr) && RTHREAD(p_node->Rptr));

        p_node->Rptr->Lptr = p_node->Lptr;
        p_returnNode = p_node->Rptr;
    }
    else
    {
        if (p_node->Rptr->Lptr != p_node) 
        {
            *htReduced = TRUE;
            p_node->Lptr->Rbit = THREAD; 
            p_returnNode = p_node->Rptr;
        }
        else 
        {
            p_node->Rptr->Lbit = THREAD;
            p_returnNode = p_node->Lptr;
        }
    }

    while (idx > 0)
    {
        p_node = nodeStack[--idx];
        p_node->Lptr = p_returnNode;

        if (deltaHt) 
        {
            deltaHt = FALSE;
            p_node->balFactor += BF_RIGHT;
            switch (p_node->balFactor) 
            {
                case BF_EVEN: 
                    deltaHt = TRUE;
                    break;

                case BF_RIGHT+BF_RIGHT:
                    p_node = avlRebalance(p_node, &deltaHt);
                    break;

                default:
                    break;
            }
        }
        p_returnNode = p_node;
    }

    *htReduced = deltaHt;
    return p_returnNode;

} /* avlRemoveMin */

/*********************************************************************
 * @purpose        Find the maximum node of this sub-tree
 *
 * @param  p_node      @b{(input)}  pointer to the top sub-tree node
 * @param  maxnode     @b{(output)} address of the ptr to the found 
 *                                  maximum node or NULL
 * @param  htReduced   @b{(output)} address of the boolean whether this
 *                                  procedure changed the height of the
 *
 * @returns            ptr to top of new sub-tree
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static t_AvlNode *
avlRemoveMax(t_AvlNode *p_node, t_AvlNode **maxnode, Bool *htReduced)
{
    Bool       deltaHt;
    int       idx = 0;
    t_AvlNode *p_returnNode;
    t_AvlNode *nodeStack[RECUR_STACK_SIZE];

    *htReduced = L7_FALSE;

    while (RLINK(p_node)) 
    {
        /* p_node is not maximum node */
        nodeStack[idx++] = p_node;
        p_node = RIGHTCHILD(p_node);
    }
    /* p_node is maximum (max node in left sub-tree) */
    *maxnode = p_node;
    deltaHt = TRUE;
    if (LLINK(p_node)) 
    {
        ASSERT(LTHREAD(p_node->Lptr) && RTHREAD(p_node->Lptr));
        ASSERT(p_node->Lptr->Rptr == p_node);

        p_node->Lptr->Rptr = p_node->Rptr;
        p_returnNode = p_node->Lptr;
    }
    else
    {
        if (p_node->Rptr->Lptr == p_node) 
        {
            /* p_node is leftchild of its parent.
             * Must use p_node->Rptr->Lptr because p_node may be 
             * predecessor of head node */
            p_node->Rptr->Lbit = THREAD; 
            p_returnNode = p_node->Lptr;         
        }                           
        else 
        {
            /* p_node is rightchild of its parent */
            p_node->Lptr->Rbit = THREAD;  
            p_returnNode = p_node->Rptr;
        }
    }

    while (idx > 0)
    {
        p_node = nodeStack[--idx];
        p_node->Rptr = p_returnNode;

        if (deltaHt) 
        {
            deltaHt = FALSE;
            p_node->balFactor += BF_LEFT;
            switch (p_node->balFactor) 
            {
                case BF_EVEN: 
                    deltaHt = TRUE;
                    break;

                case BF_LEFT+BF_LEFT:
                    p_node = avlRebalance(p_node, &deltaHt);
                    break;

                default:
                    break;
            }
        }
        p_returnNode = p_node;
    }

    *htReduced = deltaHt;
    return p_returnNode;

} /* avlRemoveMax */


/*********************************************************************
 * @purpose        Remove all nodes from an AVL tree.
 *
 * @param  p_tree      @b{(input)}  AVL tree
 * @param  freeEntry   @b{(input)}  Boolean whether to free dataptr memory
 *
 * @returns            Nothing
 *
 * @notes
 *     Use the single link list (SLL) to walk the AVL tree.
 *     If freeEntry flag TRUE then also frees structure 
 *     pointed to by dataptr.
 *
 * @end
 * ********************************************************************/
void
avlDeleteAll(t_AvlTree *p_tree, Bool freeEntry)
{
    t_AvlNode *p_node, *p_next;
    t_AvlNode *p_head;

    for (p_node = p_tree->sllhead; p_node; p_node = p_next)
    {
        p_next = p_node->sll;

        if (freeEntry && p_node->dataptr)
            XX_Free(p_node->dataptr);

        XX_Free(p_node);
    }

    /* Reset head to initial value */
    p_head = p_tree->head;
    p_head->Rbit = LINK;
    p_head->Lbit = THREAD;
    p_head->Lptr = p_tree->head;
    p_head->Rptr = p_tree->head;

    p_tree->sllhead = NULL;
    p_tree->count = 0;
    p_tree->current = NULL;
    p_tree->next = NULL;
}

/*********************************************************************
 * @purpose     Rebalance the AVL sub-tree 
 *
 * @param  p_subTreeNode  @b{(input)}  pointer to sub-tree to be rebalanced
 * @param  htReduced      @b{(output)} if the sub-tree height decreased
 *
 * @returns               pointer to node on top of sub-tree.
 *
 * @notes
 *     The sub-tree pointer to by p_subTreeNode is unbalanced by at most
 *     one leaf.  This function may be called by either "avlInsert"
 *     or "avlDelete".  Assumes balance factors & threads are correct. 
 *     Returns pointer to root of balanced tree. Threads & balance 
 *     factors have been corrected if necessary.  If the height of the 
 *     p_subTreeNode subtree decreases by one, avlRebalance sets htReduced
 *     to 1, otherwise htReduced is set to 0.  If avlRebalance is 
 *     called by avlInsert, htReduced will always be set to 1 - 
 *     just by the nature of the algorithm.  So the function avlInsert 
 *     does not need the information provided by htReduced.  
 *     However, avlDelete does need this information.
 *
 *     definition(tree-height(X)) : the maximum path length from node X to 
 *                                  p_subTreeNode leaf node.
 *     htReduced is set to 1 if and only if 
 *            tree-height(rebalance()) < tree-height(p_subTreeNode)
 *
 * @end
 * ********************************************************************/

t_AvlNode *
avlRebalance(t_AvlNode *p_subTreeNode, Bool *htReduced)
{
    t_AvlNode *p_nodeB, *p_nodeC;
    t_AvlNode *sub_root = NULL;   /* sub_root will be the return value, */

    *htReduced = FALSE;   
    ASSERT(!IS_HEAD(p_subTreeNode));

    if (abs(p_subTreeNode->balFactor) <= 1)
    {
        /* Already balanced.  */
        return(p_subTreeNode);
    }

    if (p_subTreeNode->balFactor == BF_LEFT+BF_LEFT) 
    {
        /* rebalance after insertion into left sub-tree (-2) */
        p_nodeB = p_subTreeNode->Lptr;
        if (p_nodeB->balFactor != BF_RIGHT)
        {
            /* LL (left-left) rotation */
            if (RTHREAD(p_nodeB))
            {
                /* p_nodeB->Rptr is p_subTreeNode thread to "p_subTreeNode" */
                ASSERT(p_nodeB->Rptr == p_subTreeNode);
                p_subTreeNode->Lbit = THREAD;   /* change from link to thread */
                p_nodeB->Rbit = LINK;           /* change thread to link */
            }
            else
            {
                p_subTreeNode->Lptr = p_nodeB->Rptr;
                p_nodeB->Rptr = p_subTreeNode;
            }

            *htReduced = p_nodeB->balFactor ? TRUE : FALSE;
            p_nodeB->balFactor += BF_RIGHT;
            p_subTreeNode->balFactor = -p_nodeB->balFactor;

            sub_root = p_nodeB;
        }
        else 
        {
            /* nodeB's balFactor == BF_RIGHT. Needs LR (left-right) rotation 
             */
            *htReduced = TRUE;

            p_nodeC = p_nodeB->Rptr;
            if (LTHREAD(p_nodeC))
            {
                ASSERT(p_nodeC->Lptr == p_nodeB);
                p_nodeC->Lbit = LINK;
                p_nodeB->Rbit = THREAD;
            }
            else 
            {
                p_nodeB->Rptr = p_nodeC->Lptr;
                p_nodeC->Lptr = p_nodeB;
            }

            if (RTHREAD(p_nodeC))
            {
                ASSERT(p_nodeC->Rptr == p_subTreeNode);
                p_nodeC->Rbit = LINK;
                p_subTreeNode->Lptr = p_nodeC;
                p_subTreeNode->Lbit = THREAD;
            }
            else
            {
                p_subTreeNode->Lptr = p_nodeC->Rptr;
                p_nodeC->Rptr = p_subTreeNode;
            }

            switch (p_nodeC->balFactor)
            {
                case BF_LEFT:  
                    p_nodeB->balFactor = BF_EVEN;
                    p_subTreeNode->balFactor = BF_RIGHT;
                    break;

                case BF_RIGHT: 
                    p_nodeB->balFactor = BF_LEFT;
                    p_subTreeNode->balFactor = BF_EVEN;
                    break;

                case BF_EVEN:     
                    p_nodeB->balFactor = BF_EVEN;
                    p_subTreeNode->balFactor = BF_EVEN;
                    break;

                default:
                    ASSERT(abs(p_nodeC->balFactor) < 2);
                    break;
            }

            p_nodeC->balFactor = BF_EVEN;

            sub_root = p_nodeC;
        }
    }
    else if (p_subTreeNode->balFactor == BF_RIGHT+BF_RIGHT)
    {
        /* rebalance after insertion into right sub-tree (+2) */
        p_nodeB = p_subTreeNode->Rptr;
        if (p_nodeB->balFactor != BF_LEFT)
        {
            /* RR rotation */
            if (LTHREAD(p_nodeB))
            {
                /* p_nodeB->Lptr is p_subTreeNode thread to "p_subTreeNode" */
                ASSERT(p_nodeB->Lptr == p_subTreeNode);
                p_subTreeNode->Rbit = THREAD;   /* change from link to thread */
                p_nodeB->Lbit = LINK;     /* change thread to link */
            }
            else
            {
                p_subTreeNode->Rptr = p_nodeB->Lptr;
                p_nodeB->Lptr = p_subTreeNode;
            }
            *htReduced = p_nodeB->balFactor ? TRUE : FALSE;
            p_nodeB->balFactor += BF_LEFT;
            p_subTreeNode->balFactor = -p_nodeB->balFactor;

            sub_root = p_nodeB;
        }
        else 
        {
            /* RL rotation */
            *htReduced = TRUE;

            p_nodeC = p_nodeB->Lptr;
            if (RTHREAD(p_nodeC))
            {
                ASSERT(p_nodeC->Rptr == p_nodeB);
                p_nodeC->Rbit = LINK;
                p_nodeB->Lbit = THREAD;
            }
            else
            {
                p_nodeB->Lptr = p_nodeC->Rptr;
                p_nodeC->Rptr = p_nodeB;
            }

            if (LTHREAD(p_nodeC))
            {
                ASSERT(p_nodeC->Lptr == p_subTreeNode);
                p_nodeC->Lbit = LINK;
                p_subTreeNode->Rptr = p_nodeC;
                p_subTreeNode->Rbit = THREAD;
            }
            else
            {
                p_subTreeNode->Rptr = p_nodeC->Lptr;
                p_nodeC->Lptr = p_subTreeNode;
            }

            switch (p_nodeC->balFactor)
            {
                case BF_RIGHT: 
                    p_nodeB->balFactor = BF_EVEN;
                    p_subTreeNode->balFactor = BF_LEFT;
                    break;

                case BF_LEFT:  
                    p_nodeB->balFactor = BF_RIGHT;
                    p_subTreeNode->balFactor = BF_EVEN;
                    break;

                case BF_EVEN:     
                    p_nodeB->balFactor = BF_EVEN;
                    p_subTreeNode->balFactor = BF_EVEN;
                    break;

                default:
                    ASSERT(abs(p_nodeC->balFactor) < 2);
                    break;
            }

            p_nodeC->balFactor = BF_EVEN;

            sub_root = p_nodeC;
        }
    }

    return sub_root;

} /* end avlRebalance */

/*********************************************************************
 * @purpose          Find the predecessor of the specified node
 *                   (in key order.)
 *
 * @param p_node     @b{(input)}  pointer to current node
 *
 * @returns          Pointer to predecessor of current node or NULL
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_AvlNode *
avlPred(t_AvlNode *p_node)
{
    t_AvlNode *p_pred;

    ASSERT(p_node);
    p_pred = p_node->Lptr;

    if (LLINK(p_node))
    {
        while (RLINK(p_pred))
            p_pred = p_pred->Rptr;
    }

    return (IS_HEAD(p_pred) ? NULL : p_pred);
}

/*********************************************************************
 * @purpose          Find the successor of the specified node.             
 *                   (in key order.)
 *
 * @param p_node     @b{(input)}  pointer to current node
 *
 * @returns          Pointer to successor of current node or NULL
 *
 * @notes   
 *
 * @end
 * ********************************************************************/
t_AvlNode *
avlSucc(t_AvlNode *p_node)
{
    t_AvlNode *p_succ;

    ASSERT(p_node);
    p_succ = p_node->Rptr;

    if (RLINK(p_node))
    {
        while (LLINK(p_succ))
            p_succ = p_succ->Lptr;
    }
    {
        byte      *key2 = ((byte *)((t_AvlNode *)(p_succ))->dataptr) + 4;
        AVL_DB_PRINT("avlSucc: key2 = 0x%08lx\n", 
                     *((ulng *)key2));
    }

/*    return (IS_HEAD(p_succ) ? NULL : p_succ); */
    if(IS_HEAD(p_succ))
    {
	    AVL_DB_PRINT("is head\n");
	    return NULL;
    }
    else
    {
	    AVL_DB_PRINT("not head\n");
	    return p_succ;
    }
    return NULL;
}

/*********************************************************************
 * @purpose              Creates AVL tree according to user specific
 *                       parameters
 *
 *
 * @param  flags         @b{(input)}  control flags
 * @param  keyLen        @b{(input)}  key length in bytes
 * @param  keyOffset     @b{(input)}  key offset in the handle structure
 * @param  KeyCmp        @b{(input)}  user defined key compare function
 *                                    if NULLP then
 * @param  getKey        @b{(input)}  user defined function to get key via
 *                                    the handle
 * @param  p_objId       @b{(output)}  output pointer to AVL tree handle
 *
 * @returns              E_OK
 * @returns              E_NOMEMORY
 * @returns              E_PADPARM
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Init(byte flags,
         byte keyLen,
         word keyOffset,
         f_AvlCmp KeyCmp,
         void *getKey,
         t_Handle* p_objId)
{
    t_AvlTree *p_tree;
    t_AvlNode *p_node;

    if (getKey != NULL)
        return E_BADPARM;   /* not supported anymore */

    if ((p_tree = (t_AvlTree *) XX_Malloc(sizeof(t_AvlTree))) == NULL)
    {
        return E_NOMEMORY;
    }
    memset(p_tree, 0, sizeof(t_AvlTree));

    if ((p_node = avlAllocNode(NULL)) == NULL)
    {
        XX_Free(p_tree);
        return E_NOMEMORY;
    }
    p_tree->head = p_node;
    p_node->Rbit = LINK;
    p_node->Lptr = p_tree->head;
    p_node->Rptr = p_tree->head;

    p_tree->sllhead = NULL;
    p_tree->count = 0;
    p_tree->current = NULL;
    p_tree->next = NULL;

    p_tree->keyLen = keyLen;
    p_tree->keyOffset = keyOffset;

    if (KeyCmp)
    {
        p_tree->KeyCmp = KeyCmp;
    } 
    else 
    {
        p_tree->KeyCmp = (f_AvlCmp) memcmp;
    }

    if (flags & AVL_KEY_DOUBLE)
        p_tree->FlgKeyDouble = TRUE;
    if (flags & AVL_KEY_ULNG)
        p_tree->FlgKeyIsUlng = TRUE;
    if (flags & AVL_FREE_ON_DEL)
        p_tree->FlgDelData = TRUE;
    if (flags & AVL_KEY2_AS_NUM)
        p_tree->FlgKey2IsNum = TRUE;

    p_tree->status = AVL_STAT_Valid;

    *p_objId = p_tree;

    return E_OK;
}


/*********************************************************************
 * @purpose          Destroys AVL tree. Deallocates its entries and tree
 *                   itself. No internal deallocation performed.
 *
 *
 * @param  p_objId   @b{(input)}  pointer to the AVL tree Id
 *
 * @returns          E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Destroy(t_Handle *p_objId)
{
    t_AvlTree *p_tree = (t_AvlTree *) *p_objId;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    AVL_Cleanup(p_tree, p_tree->FlgDelData);

    p_tree->status = 0;
    XX_Free(p_tree->head);
    XX_Free(p_tree);
    *p_objId = NULLP;

    return E_OK;
}



/*********************************************************************
 * @purpose            Deallocates AVL entries leaving tree intact.
 *
 *
 * @param  objId       @b{(input)}  AVL  tree object Id
 * @param  freeEntry   @b{(input)}  free entry's memory flag
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void
AVL_Cleanup(t_Handle objId, Bool freeflag)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    avlDeleteAll(p_tree, freeflag);
}



/*********************************************************************
 * @purpose         Inserts an entry into AVL tree. Allocates memory if
 *                  specific option exists.
 *
 *
 * @param  objId    @b{(input)}  AVL  tree object Id
 * @param  key      @b{(input)}  key to be inserted
 * @param  value    @b{(input)}  to be inserted (key owner)
 *
 * @returns         E_OK
 * @returns         E_NOMEMORY
 * @returns         E_FAILED - duplicate entry
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Insert(t_Handle objId, byte *key, void *value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *p_node;
    e_Err      e;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    if ((p_node = avlAllocNode(value)) == NULL)
    {
        return E_NOMEMORY;
    }

    if ((e = avlInsert(p_tree, p_node, NULL)) != E_OK)
    {
        XX_Free(p_node);
        return E_FAILED;
    }

    return E_OK;
}


/*********************************************************************
 * @purpose         Deletes an entry from AVL tree. Deallocates memory
*                   if specific option exists.
 *
 *
 * @param  objId    @b{(input)}  AVL tree object Id
 * @param  key      @b{(input)}  to be deleted
 * @param  pp_value @b{(output)}  ouput pointer to deleted value
 *
 * @returns         E_NOT_FOUND
 * @returns         E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Delete(t_Handle objId, byte *key, void **pp_value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    void      *dataptr = NULL;
    e_Err     e;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    if ((e = avlDelete(p_tree, key, &dataptr)) == E_OK)
    {
        if (pp_value)
            *pp_value = dataptr;

        if (p_tree->FlgDelData && dataptr)
            XX_Free(dataptr);

        return E_OK;
    }

    return E_NOT_FOUND;
}


/*********************************************************************
 * @purpose          Finds an entry in AVL tree.
 *
 *
 * @param  objId     @b{(input)}  AVL tree object Id
 * @param  bm_flag   @b{(input)}  using BM
 * @param  key       @b{(input)}  to be found
 * @param  value     @b{(output)}  ouput pointer to found value
 *
 * @returns          E_NOT_FOUND
 * @returns          E_OK
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Find(t_Handle objId, Bool bm_flag, byte *key, void **pp_value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *p_node;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    if (!p_tree->count)
    {
        return E_NOT_FOUND;
    }

    p_node = (t_AvlNode *) avlFind(p_tree, key, bm_flag);
    if (p_node)
    {
        if (pp_value)
            *pp_value = p_node->dataptr;

        return E_OK;
    }

    if (pp_value)
        *pp_value = NULL;

    return E_NOT_FOUND;
}



/*********************************************************************
 * @purpose           Changes an entry from AVL tree.
 *
 *
 * @param  objId      @b{(input)}  AVL tree object Id
 * @param  key        @b{(input)}  to be changed
 * @param  value      @b{(input)}  new value
 *
 * @returns           E_NOT_FOUND
 * @returns           E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Change(t_Handle objId, byte *key, void *value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *p_node;
    byte      *key2;
    int       cmpval;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    p_node = (void *) avlFind(p_tree, key, FALSE);
    if (p_node)
    {
        p_node->dataptr = value;
        key2 = GET_KEY(p_tree, p_node);
        cmpval = KEY_COMPARE(p_tree, key, key2);
        ASSERT(cmpval == 0);

        return E_OK;
    }

    return E_NOT_FOUND;
}


/*********************************************************************
 * @purpose           AVL_GetFirst
 *
 *
 * @param  objId      @b{(input)} AVL tree
 * @param  pp_value   @b{(output)} output pointer to obtained element
 *
 * @returns           E_OK
 * @returns           E_NOT_FOUND
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_GetFirst(t_Handle objId, void **pp_value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *p_node;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);
    ASSERT(pp_value != NULL);

    /* find the node with the lowest key */
    p_node = p_tree->sllhead;
    if (p_node == NULL)
    {
        /* Empty list */
        return E_NOT_FOUND;
    }

    p_tree->current = p_node;
    p_tree->next = NULL;
    *pp_value = p_node->dataptr;

    return E_OK;
}


/*********************************************************************
 * @purpose           AVL_GetNext
 *
 *
 * @param  objId      @b{(input)}  AVL tree
 * @param  pp_value   @b{(output)}  output pointer to obtained element
 *
 * @returns           E_NOT_FOUND - end of list
 * @returns           E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_GetNext(t_Handle objId, void **pp_value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);
    ASSERT(pp_value != NULL);

    if (p_tree->current == NULL)
    {
        /*****************************************************************/
        /* If no current, then either didn't call GetFirst or current    */
        /* was deleted. If delete, then next hold the next node.         */
        /*****************************************************************/
        if (p_tree->next == NULL)
        {
            return E_NOT_FOUND;     /* at the end of the list */
        }
        else
        {
            p_tree->current = p_tree->next;
            p_tree->next = NULL;
        }
    }
    else
    {
        /* Advance current to the next node on the (key ordered) list */
        if ((p_tree->current = p_tree->current->sll) == NULL)
        {
            return E_NOT_FOUND;
        }
    }

    *pp_value = p_tree->current->dataptr;

    return E_OK;
}

/*********************************************************************
 * @purpose           AVLH_FindNext
 *
 *
 * @param  objId      @b{(input)}  AVLH tree
 * @param  key        @b{(output)} key of the element.
 * @param  pp_value   @b{(input)}  value of next element if found.
 *
 * @returns           E_OK
 * @returns           E_NOT_FOUND
 *
 * @notes             This returns the next found entry not in key order.
 *                    This routine will return all entries in all trees.
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_FindNext(t_Handle objId, byte *key, void **pp_value)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    t_AvlTree  *p_tree;
    e_Err      e;
    int        currIdx;
    int        i;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    currIdx = GET_AVL_TREE_INDEX(p_Htree, key);

    p_tree = p_Htree->avlh_handle_tbl[currIdx];
    ASSERT(p_tree->status == AVL_STAT_Valid);

    if(AVL_FindNext(p_tree, key, pp_value) == E_OK)
        return E_OK;

    /* Try to find next not empty list */
    for (i = currIdx + 1; i < AVLH_SIZE; i++)
    {
        if (p_Htree->avlh_handle_tbl[i]->count)
            break;
    }
    if (i == AVLH_SIZE)
    {
        return E_NOT_FOUND;
    }

    /* Get first entry */
    e = AVL_GetFirst(p_Htree->avlh_handle_tbl[i], pp_value);

    return e;
}

/*********************************************************************
 * @purpose           AVL_FindNext
 *
 *
 * @param  objId      @b{(input)}  AVL tree
 * @param  key        @b{(input)}  to be found
 * @param  pp_value   @b{(output)}  ouput pointer to found value
 *
 * @returns           E_NOT_FOUND
 * @returns           E_OK
 *
 * @notes 
 *      Find the next node in key sequence.         
 *
 * @end
 * ********************************************************************/
e_Err
AVL_FindNext(t_Handle objId, byte *key, void **pp_value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *p_next;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);
    ASSERT(key);

    if (!p_tree->count)
    {
        if (pp_value)
            *pp_value = NULL;

        return E_NOT_FOUND;
    }

    p_next = avlFindNext(p_tree, key);
    if (p_next != NULL)
    {
        if (pp_value)
            *pp_value = p_next->dataptr;

        return E_OK;
    }

    /* Next not found */
    if (pp_value)
        *pp_value = NULL;

    return E_NOT_FOUND;
}

/*********************************************************************
 * @purpose          Try to insert an Entry.  If it already exists, 
 *                   return the pointer to the existing entry.
 *
 *
 * @param  objId     @b{(input)}  AVL tree object Id
 * @param  key       @b{(input)}  key to be inserted
 * @param  value     @b{(input)}  to be inserted (key owner)
 * @param  pp_value  @b{(output)} ouput pointer to found value
 *
 * @returns          E_IN_MATCH  - existing entry
 * @returns          E_NO_MEMORY - cannot allocate entry
 * @returns          E_OK        - inserted entry
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_InsertFind(t_Handle objId, byte *key, void *value, void **pp_value)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *p_node;
    t_AvlNode *p_foundNode;
    e_Err e;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    if ((p_node = (t_AvlNode *) avlAllocNode(value)) == NULL)
    {
        if (pp_value)
            *pp_value = NULL;

        return E_NOMEMORY;
    }

    e = avlInsert(p_tree, p_node, &p_foundNode);
    if (e == E_IN_MATCH)
    {
        XX_Free(p_node);
        p_node = p_foundNode;
    }

    if (pp_value)
        *pp_value = p_node->dataptr;

    return e;
}


/*********************************************************************
 * @purpose               AVL_GetEntriesNmb
 *
 *
 * @param  objId          @b{(input)}  AVL tree
 * @param  entriesNmb     @b{(output)}  output pointer number of elements
 *
 * @returns               E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_GetEntriesNmb(t_Handle objId, ulng *entriesNmb)
{
    t_AvlTree *p_tree = (t_AvlTree *) objId;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);

    *entriesNmb = p_tree->count;

    return E_OK;
}



/*********************************************************************
 * @purpose           AVL_Browse  
 *
 *
 * @param  objId       @b{(input)}  binary tree object Id
 * @param  userBrowser @b{(input)}  user browse function
 * @param  flag        @b{(input)}  user flag
 *
 * @returns            E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVL_Browse(t_Handle objId, BROWSEFUNC userBrowser, ulng flag)
{
    e_Err     e, er;
    void      *value;
    byte      *key;
    t_AvlTree *p_tree = (t_AvlTree *) objId;
    t_AvlNode *current = NULL;

    ASSERT(p_tree);
    ASSERT(p_tree->status == AVL_STAT_Valid);
    ASSERT(userBrowser);

    e = AVL_GetFirst(p_tree, &value);
    while (e == E_OK)
    {
        current = p_tree->current;

        if (!userBrowser((byte *) value, flag))
        {
            /* If user's browser function returned FALSE, remove the item 
             * from the tree.
             */
            key = GET_KEY(p_tree, current);
            er = avlDelete(p_tree, key, &value);
            ASSERT(er == E_OK);

            if (p_tree->FlgDelData && value)
                XX_Free(value);
        }

        e = AVL_GetNext(p_tree, &value);
    }

    return E_OK;
}


/********************************************************************/
/*           Hashed AVL tree API implementation                     */
/********************************************************************/

/*********************************************************************
 * @purpose           Creates AVLH tree according to user specific
 *                    parameters Exact match.
 *
 *
 * @param  flags         @b{(input)}  control flags
 * @param  keyLen        @b{(input)}  key length in bytes
 * @param  keyOffset     @b{(input)}  key offset in the handle structure
 * @param  tableSize     @b{(input)}  UNUSED
 * @param  KeyCmp        @b{(input)}  user defined key compare function if
 *                                    NULLP then
 * @param  f_getKey      @b{(input)}  Must be NULL.
 * @param  p_objId       @b{(output)} output pointer to AVLH tree handle
 *
 * @returns              E_OK
 * @returns              E_NOMEMORY
 * @returns              E_BADPARM
 *
 * @notes
 *                    Assumption : this structure works with FlgKeyDouble
 *                                 = TRUE.
 *                                 key is supposed to be 2-components long.
 *                                 That is, if key > 4, then key is double -
 *                                 2-components long.
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Init(byte flags,
          byte keyLen,
          word keyOffset,
          f_AvlCmp KeyCmp,
          void *getKey,
          t_Handle *p_objId)
{
    t_AvlHTree *p_Htree;
    byte i;
    e_Err e = E_OK;

    if (getKey != NULL)
        return E_BADPARM;   /* not supported anymore */

    if ((p_Htree = (t_AvlHTree *) XX_Malloc(sizeof(t_AvlHTree))) == NULL)
    {
        return E_NOMEMORY;
    }
    memset(p_Htree, 0, sizeof(t_AvlHTree));


    p_Htree->keyLen    = keyLen;
    p_Htree->keyOffset = keyOffset;
    p_Htree->KeyCmp    = KeyCmp;

    if (flags & AVL_KEY_ULNG)
        p_Htree->FlgKeyIsUlng = TRUE;
    if (flags & AVL_FREE_ON_DEL)
        p_Htree->FlgDelData = TRUE;
    if (keyLen > 4)
        flags |= AVL_KEY_DOUBLE;

    p_Htree->status = AVL_STAT_Valid;

    for (i = 0; i < AVLH_SIZE; i++)
    {
        e |= AVL_Init(flags, p_Htree->keyLen, p_Htree->keyOffset,
                      KeyCmp, NULL,
                      (t_Handle *) &p_Htree->avlh_handle_tbl[i]);
    }

    *p_objId = p_Htree;

    return e;
}


/*********************************************************************
 * @purpose            Deallocates AVLH entries leaving tree intact
 *
 *
 * @param objId        @b{(input)}  AVLH  tree object Id
 * @param freeEntry    @b{(input)}  free entry's memory flag
 *
 * @returns            Nothing
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void
AVLH_Cleanup(t_Handle objId, Bool freeEntry)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    byte i;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    for (i = 0; i < AVLH_SIZE; i++)
    {
        AVL_Cleanup(p_Htree->avlh_handle_tbl[i], freeEntry);
    }
}


/*********************************************************************
 * @purpose              Destroys AVLH tree. Deallocates its entries and
 *                       tree itself.
 *                       No internal deallocation performed.
 *
 *
 * @param  p_objId       @b{(input)}  pointer to the AVLH tree Id
 *
 * @returns              E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Destroy(t_Handle *p_objId)        /* pointer to the binary tree Id */
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) *p_objId;
    word i;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    for (i = 0; i < AVLH_SIZE; i++)
    {
        if (&p_Htree->avlh_handle_tbl[i] != 0)
        {
            AVL_Destroy((t_Handle *) &p_Htree->avlh_handle_tbl[i]);
        }
    }

    p_Htree->status = 0;
    XX_Free(p_Htree);
    *p_objId = NULLP;

    return E_OK;
}


/*********************************************************************
 * @purpose           Inserts an entry into AVLH tree. Allocates memory
 *                    if specific option exists.
 *
 *
 * @param  objId      @b{(input)}  AVLH  tree object Id
 * @param  key        @b{(input)}  key to be inserted
 * @param  value      @b{(input)}  to be inserted (key owner)
 *
 * @returns           E_OK
 * @returns           E_NOMEMORY
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Insert(t_Handle objId, byte *key, void *value)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    int        idx;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    idx = GET_AVL_TREE_INDEX(p_Htree, key);
    return AVL_Insert(p_Htree->avlh_handle_tbl[idx], key, value);
}



/*********************************************************************
 * @purpose           Deletes an entry from AVLH tree. Deallocates memory
 *                    if specific option exists.
 *
 *
 * @param   objId     @b{(input)}  AVLH tree object Id
 * @param   key       @b{(input)}  to be deleted
 * @param   value     @b{(output)}  ouput pointer to deleted value
 *
 * @returns           E_NOT_FOUND
 * @returns           E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Delete(t_Handle objId, byte *key, void **value)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    int        idx;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    idx = GET_AVL_TREE_INDEX(p_Htree, key);
    return AVL_Delete(p_Htree->avlh_handle_tbl[idx], key, value);
}


/*********************************************************************
 * @purpose            Finds an entry from AVLH tree.  Exact match only.
 *
 *
 * @param  objId       @b{(input)}  AVLH tree object Id
 * @param  bm_flag     @b{(input)}  using BM
 * @param  value       @b{(output)}  ouput pointer to found value
 * @param  shortKey    @b{(input)}  use shortKey (first part)
 *
 * @returns            E_NOT_FOUND
 * @returns            E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Find(t_Handle objId, byte *key, void **value, Bool shortKey)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    t_AvlTree *p_tree;
    e_Err     e;
    Bool      FlgKeyDouble = FALSE;
    int       idx;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    idx = GET_AVL_TREE_INDEX(p_Htree, key);
    p_tree = p_Htree->avlh_handle_tbl[idx];
    if (!p_tree->count)
        return E_NOT_FOUND;

    if (shortKey && p_tree->FlgKeyDouble)
    {
        p_tree->keyLen >>= 1;
        FlgKeyDouble = p_tree->FlgKeyDouble;
        p_tree->FlgKeyDouble = FALSE;
    }

    e = AVL_Find(p_tree, FALSE, key, value);

    if (shortKey && FlgKeyDouble)
    {
        p_tree->keyLen <<= 1;
        p_tree->FlgKeyDouble = FlgKeyDouble;
    }

    return e;
}


/*********************************************************************
 * @purpose           Changes an entry from AVLH tree.
 *
 *
 * @param  objId      @b{(input)}  AVLH tree object Id
 * @param  key        @b{(input)}  to be changed
 * @param  value      @b{(input)}  new value
 *
 * @returns           E_NOT_FOUND
 * @returns           E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Change(t_Handle objId, byte *key, void *value)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    int        idx;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    idx = GET_AVL_TREE_INDEX(p_Htree, key);
    return AVL_Change(p_Htree->avlh_handle_tbl[idx], key, value);
}


/*********************************************************************
 * @purpose              AVLH_GetEntriesNmb
 *
 *
 * @param  objId         @b{(input)}  AVLH tree
 * @param  entriesNmb    @b{(output)}  output pointer number of elements
 *
 * @returns              E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_GetEntriesNmb(t_Handle objId, ulng *entriesNmb)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    ulng cnt = 0;
    int i;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    for (*entriesNmb = 0, i = 0; i < AVLH_SIZE; i++)
    {
        ASSERT(p_Htree->avlh_handle_tbl[i]->status == AVL_STAT_Valid);
        cnt += p_Htree->avlh_handle_tbl[i]->count;
    }
    *entriesNmb = cnt;

    return E_OK;
}




/*********************************************************************
 * @purpose            AVLH_Browse  
 *
 *
 * @param  objId       @b{(input)} AVLH tree object Id
 * @param  userBrowser @b{(input)} user browse function
 * @param  flag        @b{(input)} user flag
 *
 * @returns            E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_Browse(t_Handle objId, BROWSEFUNC userBrowser, ulng flag)
{
    e_Err      e, er;
    void       *value;
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    t_AvlTree  *p_tree;
    byte       i;
    t_AvlNode  *current = NULL;
    void       *dataptr;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);
    ASSERT(userBrowser);

    for (i = 0; i < AVLH_SIZE; i++)
    {
        p_tree = p_Htree->avlh_handle_tbl[i];
        if (!p_tree->count)
            continue;

        e = AVL_GetFirst(p_tree, &value);
        while (e == E_OK)
        {
            current = p_tree->current;
            if (!userBrowser((byte *) value, flag))
            {
                er = avlDelete(p_tree, GET_KEY(p_tree, current), &dataptr);
                ASSERT(er == E_OK);

                if (p_tree->FlgDelData && dataptr)
                    XX_Free(dataptr);
            }

            e = AVL_GetNext(p_tree, &value);
        }
    }

    return E_OK;
}



/*********************************************************************
 * @purpose           AVLH_GetFirst
 *
 *
 * @param objId       @b{(input)}  AVLH tree
 * @param pp_value    @b{(output)}  output pointer to obtained element
 *
 * @returns           E_OK
 * @returns           E_NOT_FOUND
 *
 * @notes    The value returned is not in key order since the keys are
 *           randomly distributed amount N AVL trees.
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_GetFirst(t_Handle objId, void **pp_value)
{
    byte i;
    t_AvlTree *p_tree;
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    for (i = 0; i < AVLH_SIZE; i++)
    {
        p_tree = p_Htree->avlh_handle_tbl[i];
        ASSERT(p_tree->status == AVL_STAT_Valid);
        if (p_tree->count)
            break;
    }

    if (i >= AVLH_SIZE)
        return E_NOT_FOUND;

    p_Htree->cur_idx = i;

    return AVL_GetFirst(p_tree, pp_value);
}

/*********************************************************************
 * @purpose           AVLH_GetNext
 *
 *
 * @param  objId      @b{(input)}  AVLH tree
 * @param  pp_value   @b{(output)}  output pointer to obtained element
 * @param  curValue   @b{(input)}  current value
 *
 * @returns           E_OK
 * @returns           E_NOT_FOUND
 * @returns           E_FAILED(if current value cannot be traced in
 *                             the AVLH tree)
 *
 * @notes             This returns the next found entry not in key order.
 *                    This routine will return all entries in all trees.
 *
 * @end
 * ********************************************************************/
e_Err
AVLH_GetNext(t_Handle objId, void **pp_value, void *currValue)
{
    t_AvlHTree *p_Htree = (t_AvlHTree *) objId;
    t_AvlTree  *p_tree;
    t_AvlNode  *p_node;
    byte       *key;
    e_Err      e;
    int        currIdx;
    int        i;

    ASSERT(p_Htree);
    ASSERT(p_Htree->status == AVL_STAT_Valid);

    p_tree = p_Htree->avlh_handle_tbl[p_Htree->cur_idx];
    ASSERT(p_tree->status == AVL_STAT_Valid);

    if (!p_tree->current || (p_tree->current->dataptr != currValue))
    {
        key = (byte *) currValue + p_Htree->keyOffset;
        currIdx = GET_AVL_TREE_INDEX(p_Htree, key);
        p_node = avlFind(p_Htree->avlh_handle_tbl[currIdx], key, FALSE);

        /* Although the current value may be deleted, there may be a next
         * entry in the tree.  But to maintain compatibility, return the 
         * error if current is not found.
         */
        if (p_node == NULL)
        {
            return E_FAILED;
        }

        p_Htree->cur_idx = currIdx;
        p_Htree->avlh_handle_tbl[currIdx]->current = p_node;
        p_tree = p_Htree->avlh_handle_tbl[currIdx];
    }

    /* Try to find next entry in current list */
    p_Htree->prev_result = AVL_GetNext(p_tree, pp_value);

    if (p_Htree->prev_result == E_OK)
        return E_OK;

    /* Try to find next not empty list */
    for (i = p_Htree->cur_idx + 1; i < AVLH_SIZE; i++)
    {
        if (p_Htree->avlh_handle_tbl[i]->count)
            break;
    }

    if (i >= AVLH_SIZE)
    {
        p_Htree->cur_idx = 0;
        return E_NOT_FOUND;
    }

    p_Htree->cur_idx = i;

    /* Get first entry */
    e = AVL_GetFirst(p_Htree->avlh_handle_tbl[p_Htree->cur_idx], pp_value);

    return e;
}

void
AVLDebugSet(ulng level)
{
    avl_debug = level;
}
