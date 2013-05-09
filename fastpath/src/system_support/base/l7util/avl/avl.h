/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    avl.h
*
* @purpose     File contains defines needed for a avl tree implimentation
*
* @component 
*
* @comments 
*
* @create      8/10/2000
*
* @author      skalyan
* @end
*
**********************************************************************/
/*************************************************************
                    
*************************************************************/




 
#ifndef _AVL_H_
#define _AVL_H_

 
#define AVL_LESS_THAN       -1
#define AVL_EQUAL            0
#define AVL_GREATER_THAN     1


#if defined(FUTURE_RELEASE) && FUTURE_RELEASE
/* defines for type of avl tree*/ 
 #define    MAC_ADDR            1
 #define    QVLAN_ADDR          2
 #define    MAC_FILTER          3
/* -----------------------------*/
#endif

#ifndef L7_AVL_MAX_DEPTH
#define L7_AVL_MAX_DEPTH	32          
#endif

#define L7_LEFT 0
#define L7_RIGHT 1
#define L7_LEFT_WEIGHT -1
#define L7_RIGHT_WEIGHT 1

void* avlNewNewDataNode(avlTree_t *avl_tree );
void avlNewFreeDataNode(avlTree_t * avl_tree, void *ptr);
void * avlRemoveEntry( avlTree_t *avl_tree, void *item);
void ** avlFindEntry(avlTree_t *avl_tree, void *item);
void * avlAddEntry( avlTree_t *avl_tree, void *item);
int avlCompareKey(void *item,
                  void *DataItem,
                  L7_uint32 lengthSearchKey,
                  avlComparator_t func);
void swapNodes (int direction, avlTreeTables_t *oldUpper, 
                avlTreeTables_t *oldLower);
avlTreeTables_t *rotateNodes (int otherDirection, avlTreeTables_t *node1, 
                  avlTreeTables_t *node2);
void balance (avlTreeTables_t *q, avlTreeTables_t *s, 
              avlTreeTables_t *t, avlTree_t *avl_tree, int direction);
avlTreeTables_t *removeAndBalance (avlTreeTables_t *s, int direction, 
                                   L7_BOOL *done);

/*****************************************************************
* @purpose  Get next AVL tree element according to the given direction
*
* @param    avl_tree    pointer to the avl tree structure
* @param    ptr         pointer to current item -> returns the following element
* @param    direction   L7_LEFT or L7_RIGHT
*  
* @returns  void *      pointer to the item
* @returns  L7_NULL     if item does not exist in the tree
*
* @notes   
*
* @end
*****************************************************************/
extern void * avlGetNextLVL7 (avlTree_t *avl_tree, avlTreeTables_t **ptr, L7_uint direction);

#endif
