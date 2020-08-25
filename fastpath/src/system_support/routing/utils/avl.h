/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename       avl.h
 *
 * @purpose        Provides external API for the AVL tree object
 *
 * @component      Routing Util Component
 *
 * @comments
 *
 * @create         12/03/2000
 *
 * @author         Igor Dvoiris
 *
 * @end
 *
 * ********************************************************************/
#ifndef avl_h
#define avl_h

/* NOTE: The following 2 changes have been done to AVL Hashing:
 *
 *       1) Hashing function GET_AVL_TREE_INDEX has been changed to 
 *          pick up the 5 MSB bits of key.
 *
 *       2) AVLH_SIZE has been changed to 32.
 *       
 *       This will generate indexes in an order ( and not in random )
 *       So that the ordered traversal becomes easy with AVL Hash trees.
 *
 *       In future, we can define a hashing function GET_AVL_TREE_INDEX()
 *       to take an extra option for kind of hashing to be done.
 */

/* Side Direction values */
#define DIR_LEFT  0
#define DIR_RIGHT 1

/* Balance Factor values */
#define BF_LEFT   -1
#define BF_RIGHT  +1
#define BF_EVEN    0

/* Link Ptr Types */
#define THREAD   0
#define LINK     1

#define LLINK(x)        ((x)->Lbit)
#define RLINK(x)        ((x)->Rbit)
#define LTHREAD(x)      (!LLINK(x))
#define RTHREAD(x)      (!RLINK(x))
#define LEFTCHILD(x)    (LLINK(x) ? (x)->Lptr : NULL)
#define RIGHTCHILD(x)   (RLINK(x) ? (x)->Rptr : NULL)

#define PUSH_PATH(x, y, nxt) ((nxt)->p_node = (x),  ((nxt)++)->bf_side = (y))
#define POP_PATH(x, nxt)     (x = (--(nxt))->bf_side, ((nxt)->p_node))

/* Always true for head node of initialized avlTree, and false for all 
 * other nodes 
 */
#define IS_HEAD(x)      ((x)->Rptr == (x))

#define RECUR_STACK_SIZE    32     /* fits trees with up to 2**32 - 1 nodes */
#define AVLH_SIZE           32     /* number of hash buckets (AVL trees) */
#define AVL_STAT_Valid      31901

#define NUM_MSB_BITS        5

/* Used for getting and comparing keys */
#define GET_KEY_PTR(tree, node, offset) \
        (((byte *)(node)->dataptr) + (tree)->keyOffset + (offset))
#define GET_KEY(tree, node)      GET_KEY_PTR(tree, node, 0)
#define GET_KEY2(tree, node)     GET_KEY_PTR(tree, node, 4)
#define GET_KEY_VALUE(tree, key)   \
        ((tree)->FlgKeyIsUlng ? *(ulng *)(key) : A_GET_4B(key))

#define GET_MSB_KEY_VALUE(tree, key, msb_bits)   \
        ((GET_KEY_VALUE(tree, key)) >> (32 - msb_bits))

#define GET_AVL_TREE_INDEX(avlt, key)   \
        (GET_MSB_KEY_VALUE(avlt, key, NUM_MSB_BITS) % AVLH_SIZE)

/* Compare primary key or only key */
#define KEY_COMPARE(tree, k1, k2)                                            \
    ((tree)->FlgKeyIsUlng ?                                                  \
        (((*(ulng*)(k1)) == (*(ulng*)(k2))) ? 0 :                            \
            ((*(ulng*)(k1)) > (*(ulng*)(k2))) ? 1 : -1) :                    \
     (((tree)->KeyCmp)(k1, k2, (tree)->FlgKeyDouble ? ((tree)->keyLen / 2) : \
                                                      (tree)->keyLen)))
/* Compare secondary keys as a number (byte) or as a mask */
#define KEY_COMPARE2(tree, k1, k2)                              \
    ((tree)->FlgKey2IsNum ?                                     \
        ((*(byte *)(k1)) - (*(byte *)(k2))) :                   \
        KEY_COMPARE(tree, k1, k2))

/* Debug print if AVLDebugSet() level is non-zero */
#define AVL_DB_PRINT(format, args...) if (avl_debug) printf(format, ## args)

/* **********************************************************************
 * Data structures.
 * **********************************************************************/

typedef struct AvlNode
{
    struct AvlNode   *Lptr;     /* Left pointer   */
    struct AvlNode   *Rptr;     /* Right pointer  */
    void             *dataptr;  /* ptr to user's data structure (value) */
    struct AvlNode   *sll;      /* Next in ordered list for browse operations */
    signed char       balFactor;/* AVL balance factor   */
    unsigned char     Lbit;     /* Left link use */
    unsigned char     Rbit;     /* Right link use */
} t_AvlNode;

typedef struct AvlTree
{
    t_AvlNode  *head;           /* pointer to root of tree node */
    ulng       count;           /* number of nodes on tree */
    t_AvlNode  *sllhead;        /* single linked-list of nodes in key order */
    f_AvlCmp   KeyCmp;          /* Function to compare keys */
    t_AvlNode  *current;        /* for get first and get next operations */
    t_AvlNode  *next;           /* for get next when current is NULL */
    word       status;          /* valid or invalid status marker */
    word       keyOffset;       /* key offset in the value */
    word       keyLen;          /* key length in bytes */

    /* Boolean Flags */
    Bool       FlgDelData;      /* TRUE - free memory data flag */
    Bool       FlgKeyDouble;    /* TRUE - two keys else only 1 key */
    Bool       FlgKeyIsUlng;    /* TRUE - key is ulng else network order */
    Bool       FlgKey2IsNum;    /* TRUE - key2 is mask len else a 32-bit mask */
} t_AvlTree;

typedef struct AVLHTree
{
    t_AvlTree  *avlh_handle_tbl[AVLH_SIZE]; /* hash table of AVL trees */

    /* fields */
    e_Err      prev_result;     /* The result of previous get next */
    f_AvlCmp   KeyCmp;          /* Function to compare keys */
    word       status;          /* valid or invalid status marker */
    word       keyOffset;       /* key offset in the value */
    byte       keyLen;          /* key length in bytes */
    byte       cur_idx;         /* Index for get first/next operations */

    /* Boolean Flags */
    Bool       FlgDelData;      /* TRUE - free memory data flag */
    Bool       FlgKeyIsUlng;    /* TRUE - key is ulng else network order */

} t_AvlHTree;

typedef struct itemStack 
{
    int       bf_side;          /* link (LEFT or RIGHT side) traversed */
    t_AvlNode *p_node;          /* ptr to node traversed */
} t_itemStack;

#endif /* avl_h */
