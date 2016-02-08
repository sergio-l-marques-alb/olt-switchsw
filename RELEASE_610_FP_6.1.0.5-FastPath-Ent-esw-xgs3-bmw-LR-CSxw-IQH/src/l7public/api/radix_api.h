/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    radix_api.h
*
* @purpose     File contains all the APIs, declared as external, 
*              needed for a radix tree implimentation.
*
* @component
*
* @comments
* 
* The radix tree is a variant of a PATRICIA tree and is used for 
* IP address lookup in many unix variants. It is best described as a 
* binary radix tree with one-way branching removed. The tree is 
* composed of internal nodes and leaves. The leaves represent address 
* classes, and contain information common to all possible destinations 
* in each class. Each leaf has at least a mask and prototype address.
* 
* A sample PATRICIA tree with keys "ababa" and "ababb" is shown below:
* 
* ----> [5]      -- i.e. test position #5
*      .   .
*    a.     . b
*    .       .
*   ababa     ababb
* 
* now insert "ba"
* 
* --------> [1]
*          .   .
*         .     .
*        .       .
*     [5]         ba
*    .   .
*   .     .
*  .       .
* ababa     ababb
* 
* 
* Each internal node represents a bit position to test. The bit position 
* is described by an offset (in bytes) and a mask for the bit test. 
* The tree branches to the left * if the bit is not set and to the right if 
* it is set. Nodes and leafs are implemented using the radix_node structure. 
* Nodes have a +ve value for rn_bit, leaves have a -ve * value (complemented). 
* 
* A tree entry is specified by a structure containing two radix_nodes followed 
* by space for the key and the mask. The last element of this structure is 
* a next ptr for tree heap management. 
* 
* For e.g.  
* struct sampleEntry_s
* {
*   struct radix_node nodes[2];
*   unsigned char key [MAX_KEY_LENGTH];
*   unsigned char mask[MAX_KEY_LENGTH];
*   struct my_node_data_s myData;
*   void * next;
* };
* 
* Here myData can be any user-specific structure/array/variables that hold user 
* data for that entry. All entries in a tree must be of the same type and must have a 
* specified total length and key length. Mask and key structures must also have the 
* same maximum length.
* 
* Why 2 nodes?
* Every time a leaf is added to the tree, two nodes are needed. 
* One for the leaf and one for the node to specify which bit to test.
*
* It is possible to have the same prototype address with differing masks; this is 
* handled by a linked list of leaf nodes. This requirement arises due to boundary 
* conditions for the smallest representation of the default route. 
* 
* E.g. 192.168.0.0/16 and 192.168.0.0/24. The implementation manages this linked list 
* using a linked list of "duplicate" nodes using the rn_Dupedkey member of rn_leaf.
* 
* A match consists of walking down nodes until a leaf is hit. Once a leaf node is hit, 
* we need to check whether we have selected the appropriate class by doing the masking 
* operation on the search key and the leaf key (and the dupedkey list as well)
* If the leaf node isn't correct, then we backtrack up the tree looking for 
* indications that a more general mask may apply (i.e. one having fewer one bits). 
* This may happen if we are asked to look up an address 
* other than the prototype addresses used to construct the tree.
* 
* NOTE: In addition to the leaf+node and linked list specified above, the implementation 
* adds each unique mask to the tree as an annotation. These annotations (leaf+node) 
* are never deleted, even if all keys for that mask are deleted from the tree. 
* As masks are usually contiguous (e.g. ff.00, ff.f0 etc), the potential number of 
* these annotation nodes should be small, i.e. 32 unique for IPv4 and 128 for IPv6. 
* The current implementation limits the number of annotations that can be added 
* to the bit length of the key. Annotaions are used to speed up searches.
*
* Internal nodes keep a netmask to facilitate backtracking (using the radix_mask structure). 
* Take the case of two routes 127.0.0.0/8 and 127.0.0.1/32 while 
* looking up address 127.0.0.3. 
*
*           A
*   (node, test bit=32)
*    (mask ff.0.0.0)
*           |
*    +--------------+
*    |              |  		
*    B              C
* (leaf, off)	 (leaf, on)
* 127.0.0.0	    127.0.0.1
* ff.0.0.0	   ff.ff.ff.ff
*
* Here the tree will contain a test for the 32nd bit (.1) and branch off to the 
* host address if its on. Now our lookup address does have the 32nd bit set 
* (.3 = 101) but .3 is not equal to .1! So we backtrack up from the .1 (C) leaf 
* to the node that was used to test the 32ndbit (A). This internal node has 
* a mask (ff.0.0.0), use this to mask the address and then try again! 
*
* This time the masked value takes us down to leaf (B) which is a match!
*
*
* @create      04/26/04
*
* @author      rkelkar
* @end
*
**********************************************************************/

#ifndef _RADIX_API_H_
#define _RADIX_API_H_

#ifdef _L7_OS_VXWORKS_
#include "vxWorks.h"
#endif
#ifdef _L7_OS_LINUX_
#include <sys/types.h>
#endif

/*********************************************************************
*
* @structures l7_radix_node
*
* @purpose    Radix search tree node layout.
*
* @notes      Nodes are used in pairs: one leaf, one node. Leafs have rn_bit -ve
*
*********************************************************************/
struct l7_radix_node {
	struct	l7_radix_mask *rn_mklist;	/* list of masks contained in subtree */
	struct	l7_radix_node *rn_parent;	/* parent */
	short	rn_bit;			/* bit offset; -1-index(netmask) */
	char	rn_bmask;		/* node: mask for bit test*/
	u_char	rn_flags;		/* enumerated next */
#define RNF_NORMAL	1		/* leaf contains normal route */
#define RNF_ROOT	2		/* leaf is root leaf for tree */
#define RNF_ACTIVE	4		/* This node is alive (for rtfree) */
	union {
		struct {			/* leaf only data: (rn_bit is -ve) */
			caddr_t	rn_Key;		/* object of search */
			caddr_t	rn_Mask;	/* netmask, if present */
			struct	radix_node *rn_Dupedkey;  /* nodes with the same key, different mask 
                                         * (most specific/longest first) 
                                         */
		} rn_leaf;
		struct {			/* node only data: (rn_bit is +ve) */
			int	rn_Off;		/* where to start compare */
			struct	radix_node *rn_L;/* progeny */
			struct	radix_node *rn_R;/* progeny */
		} rn_node;
	}		rn_u;
#ifdef RN_DEBUG
	int rn_info;
	struct l7_radix_node *rn_twin;
	struct l7_radix_node *rn_ybro;
#endif
};

/*********************************************************************
*
* @structures l7_radix_mask
*
* @purpose    Annotations to tree concerning potential routes applying to subtrees.
*
* @notes      Manages node pairs added for netmasks (refcounts etc)
*
*********************************************************************/
struct l7_radix_mask {
	short	rm_bit;			/* bit offset; -1-index(netmask) */
	char	rm_unused;		/* cf. rn_bmask */
	u_char	rm_flags;		/* cf. rn_flags */
	struct	l7_radix_mask *rm_mklist;	/* more masks to try */
	union	{
		caddr_t	rmu_mask;			/* the mask */
		struct	l7_radix_node *rmu_leaf;	/* for normal routes */
	}	rm_rmu;
	int	rm_refs;		/* # of references to this struct */
};

/*********************************************************************
*
* @structures l7_radix_node_head
*
* @purpose    Tree node head, contains root nodes
*
* @notes      Contains instance specific data used by the algorithm.
*
*********************************************************************/
struct l7_radix_node_head {

  int max_keylen;
  int mask_last_zeroed;
  struct l7_radix_mask *rn_mkfreelist;
  void * rn_masknodefreelist;
  char *rn_zeros;
  char *rn_ones;
  char *addmask_key;

	struct	l7_radix_node *rnh_treetop;
	int	rnh_addrsize;		/* permit, but not require fixed keys */
	int	rnh_pktsize;		/* permit, but not require fixed keys */
	struct	l7_radix_node rnh_nodes[3];	/* empty tree for common case */
};


/*********************************************************************
*
* @structures radixTree_s
*
* @purpose    this is the generic "per-instance" radix tree structure
*
* @notes      Contains information needed for the working of the radix tree.
*             However information originally in radix_node_head has not been
*             moved here. This structure is accessed by api and util fns.
*
*********************************************************************/
typedef struct radixTree_s
{
  L7_uint32 max_entries;              /* maximum entries possible in this tree */
  L7_uint32 count;                    /* number of entries currently used */
  L7_uint32 dataLength;               /* length in bytes of a data node */
  L7_uint32 offset_next;              /* offset in bytes to the next ptr of a data node */
  struct l7_radix_node_head mask_rnhead; /* radix tree head */
  void * dataHeap;                    /* ptr to the user provided heap for data nodes */
  void * maskNodeHeap;                /* ptr to the user provided heap for mask nodes (tree heap) */
  struct l7_radix_mask * maskListHeap;   /* ptr to the user provided heap for mask list nodes (tree heap) */
  void * rn_datanodefreelist;         /* ptr to the head of the free data nodes list */
  void * semId;                       /* tree semaphore */
} radixTree_t;

/* space required for a maskNode (node pair + key) */
#define RADIX_MASK_NODE_ITEM_SIZE_UNALIGNED(max_keylen) ( \
  ((sizeof(struct l7_radix_node)*2 + (max_keylen)) + sizeof(L7_uint32)) \
  )

#define RADIX_MASK_NODE_ITEM_SIZE(max_keylen) ( \
  (RADIX_MASK_NODE_ITEM_SIZE_UNALIGNED(max_keylen) + (4 - (RADIX_MASK_NODE_ITEM_SIZE_UNALIGNED(max_keylen) % 4))) \
  )


/* offset to next ptr for a maskNode */
#define RADIX_MASK_NODE_ITEM_OFFSET_NEXT(max_keylen) ( \
  RADIX_MASK_NODE_ITEM_SIZE((max_keylen)) - sizeof(L7_uint32))

/* space required for mask nodes (annotations), 
 * for key length (bits) nodes */
#define RADIX_TREE_HEAP_MASK_NODE_SIZE(max_keylen) ( \
  ((max_keylen*8) * RADIX_MASK_NODE_ITEM_SIZE((max_keylen))) \
  )

/* space required for mask lists */
#define RADIX_TREE_HEAP_MASK_LIST_SIZE(max_entries) ( \
  (max_entries) * sizeof(struct l7_radix_mask) \
  )

/********************************************************************
* @purpose  Used to calcuate the size of the tree heap.
*
* @param    max_entries   Maximum entries in the tree
* @param    max_keylen    Max key length in bytes
*
* @returns  Size of the tree heap in bytes 
*
* @notes    The tree heap includes space to store radix
*           internal data structures such as mask nodes
*           and mask lists.
*
* @end
********************************************************************/
#define RADIX_TREE_HEAP_SIZE(max_entries, max_keylen) ( \
  RADIX_TREE_HEAP_MASK_NODE_SIZE((max_keylen)) + \
  RADIX_TREE_HEAP_MASK_LIST_SIZE((max_entries)) + \
  (3 * (max_keylen)) \
  )


/********************************************************************
* @purpose  Used to calcuate the size of the data heap.
*
* @param    max_entries   Maximum entries in the tree
* @param    max_keylen    Size of the data item
*
* @returns  Size of the data heap in bytes 
*
* @notes    The data heap includes space to store a data item.
*           This includes a radix node+leaf pair to attach to this 
*           item to the tree, the tree key (addr, mask) and additional 
*           private data members. The last (must be aligned) 4 bytes
*           are interpreted as a next pointer. This pointer is used to
*           manage the heap.
*
* @end
********************************************************************/
#define RADIX_DATA_HEAP_SIZE(max_entries, dataLength) ( \
  (max_entries) * (dataLength) \
  )

/********************************************************************
 * lookup types
 *
 *  L7_RN_EXACT   forces a radix to return an entry that has a  
 *                matching key and mask.
 *
 *  L7_RN_GETNEXT 
 *               philosophy: best match/ longest mask first
 *               use in conjunction with radixGetNextEntry for 
 *               efficient iteration.
 *
 *                - next item with a matching key and shorter mask
 *                  (less specific match), or if there isnt one,
 *                - the best match (longest mask) for next larger item.
 *                  
 *  L7_RN_GETNEXT_ASCEND 
 *                philosophy: ascending key, mask values
 *                use for when you need ascending keys e.g for SNMP
 *
 *                - next item with a matching key and larger mask,
 *                  or if there isnt one,
 *                - the smallest mask for the next larger item.
 *
 ********************************************************************/
#define L7_RN_EXACT             1
#define L7_RN_GETNEXT           2 /* longest prefix first */
#define L7_RN_GETNEXT_ASCEND    3 /* shortest prefix first */

/*********************************************************************
* @purpose  Resets all entries
*
* @param    tree        @b{(input)} pointer to the radix tree structure
*
* @returns  void
*
* @notes   This function effectively resets the tree by clearing the
*          heaps and recreating the free node linked lists.
*
* @end
*********************************************************************/
void radixPurgeTree(radixTree_t * tree);

/*********************************************************************
* @purpose  Create the generic radix tree structure and initialize
*
* @param    tree       @b{(input)} pointer to the tree structure
* @param    dataHeap   @b{(input)} pointer to the data heap
* @param    treeHeap   @b{(input)} pointer to the tree heap
* @param    max_entries @b{(input)} maximum number of entries in the tree
* @param    dataLength @b{(input)} length of a data entry
* @param    max_keylen @b{(input)} maximum length of the search key
*
* @returns  radixTree_t   pointer to the generic radix tree structure if successful
* @returns  L7_NULLPTR  if not successful
*
* @notes    RESTRICTIONS & RULES
*           1. For the data structure of data heap:
*             a. First field must be a node pair i.e. struct l7_radix_node[2]
*             b. This must be followed by key[max_keylen] and mask[max_keylen]
*             c. Last field must be a void pointer type
*
*           2. First byte of the key and mask should be set to its length
*              e.g. sockaddr_in_new
*
*           3. Size of tree heap must be RADIX_TREE_HEAP_SIZE(max_entries, max_keylen)
*
* @end
*********************************************************************/
radixTree_t * radixCreateTree(radixTree_t * tree, void * dataHeap, void * treeHeap,  
  L7_uint32 max_entries, L7_uint32 dataLength, L7_uint32 max_keylen);

/*********************************************************************
* @purpose  Inserts ITEM into TREE.  Returns NULL if the item was inserted,
*           otherwise a pointer to the duplicate item
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    item   @b{(input)} pointer to item to be inserted
*
* @returns  NULL if item was inserted
* @returns  void  pointer to duplicate item if duplicate exists
* @returns  void  pointer to item, if error
*
* @notes    Every item must follow the rules specified for radixCreateTree
*
* @end
*********************************************************************/
void * radixInsertEntry(radixTree_t * tree, void * item);

/*********************************************************************
* @purpose  Lookup the radix tree for a specific node based on
*           key and mask values.
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    key    @b{(input)} pointer to the key
* @param    mask   @b{(input)} pointer to the mask
* @param    type   @b{(input)} lookup type can be L7_RN_EXACT or
*                              L7_RN_GETNEXT
*
* @returns  void *   pointer to the item if found
* @returns  NULL     if item does not exist in the tree
*
* @notes    The key and mask must have length in the first byte.
*           Entries are returned in increasing order of ipaddr,
*           for two entries with the same address, the entry with
*           the longer mask is returned first.
*
* @end
*********************************************************************/
void * radixLookupNode(radixTree_t * tree, void * key, void * mask, int type);

/*********************************************************************
* @purpose  Find the longest match for a key
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    key    @b{(input)} pointer to the key
* @param    skipentries @b{(input)} number of matches to skip
*
* @returns  void *   pointer to the longest match item if found
* @returns  NULL     if no item matches the key
*
* @notes    The key must have length in the first byte,
*           longest match refers to the matching item with the longest
*           mask.
*
*           Setting skipentries to 0 returns the best match.
*           To back track up the tree set skipentries to 1,2..n.
*           e.g. for a tree with 1.1.1.0/24 and 1.1.0.0/16, the address
*           1.1.1.10 will match A for skipentries=0, B for skipentries=1.
*           Setting skipentries to 3..n will return null as there are no
*           more matching entries.
*
* @end
*********************************************************************/
void * radixMatchNode(radixTree_t * tree, void * key, int skipentries);



/*********************************************************************
* @purpose  Find the longest match for a key, ignoring mask
*
* @param    tree      @b{(input)} pointer to the radix tree structure
* @param    key       @b{(input)} pointer to the key
* @param    key_bits  @b{(input)} len of key required to match.
*
* @returns  void *   pointer to the match item if found
* @returns  NULL     if no item matches the key
*
* @notes    The key must have length in the first byte,
*           longest match referts to the matching item. key_bits must
*           include 8 bits for the key len byte.
*
*           the mask len of the item is ignored, so item may not
*           be a route that resolves key. in fact, this function
*           is intended to be find one of a set of routes (eg item)
*           that the key resolves. if key is a prefix route with trailing
*           0s, then item will be the lowest valued route (or host) that
*           key resolves.
*
*       
* @end
*********************************************************************/
void * radixFirstResolved(radixTree_t * tree, void * key, int key_bits);

/*****************************************************************
* @purpose  Returns a count of entries in the tree
*
* @param    tree   @b{(input)} pointer to the radix tree structure
*
* @returns  count of entries in the tree
*
* @notes
*
* @end
*****************************************************************/
L7_uint32 radixTreeCount (radixTree_t * tree);

/*********************************************************************
* @purpose  Searches Radix tree for an item matching ITEM.  If found, the
*           item is removed from the tree and the actual item found is returned
*           to the caller.  If no item matching ITEM exists in the tree,
*           returns NULL.
*
* @param    tree @b{(input)} pointer to the radix tree structure
* @param    item @b{(input)} pointer to item to be deleted
*
* @returns  void *   pointer to the item if deleted
* @returns  NULL     if item does not exist in the tree
*
* @notes
*
* @end
*********************************************************************/
void * radixDeleteEntry(radixTree_t * tree, void * item);

/*********************************************************************
* @purpose  Deletes an radix tree structure
*
* @param    tree   @b{(input)} pointer to an redix tree structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t radixDeleteTree(radixTree_t *tree);

/*****************************************************************
* @purpose  Get the next entry in a radix tree
*
* @param    tree    pointer to the radix tree structure
* @param    item    pointer to a tree entry (radix_node)
*
* @returns  pointer to the next tree entry
* @returns  null pointer, if no more entries
*
* @notes    - The input entry must point to an existing entry in the
*           tree not just an entry with the key and mask set.
*           radixLookupNode can be used to get an entry based
*           on a key and mask.
*
*           - Pass a null ptr to get the first entry in the tree.
*
*           - Entries are returned in sorted order
*
*           !IMPORTANT!
*           do NOT use in conjunction with L7_RN_GETNEXT_ASCEND as
*           this function returns the best match (largest mask), 
*           first. Can be used with L7_RN_GETNEXT.
*
* @end
*****************************************************************/
void * radixGetNextEntry(radixTree_t * tree, void * item);

/*****************************************************************
* @purpose  Dump radix tree leafs to the console
*
* @param    tree    pointer to the radix tree structure
*
* @end
*
*****************************************************************/
void radixDumpTree(radixTree_t * tree);


#endif
