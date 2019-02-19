/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     l7_hl_api.h
 *
 * @purpose      Hash list data structure
 *
 * @component    
 *
 * @comments      This implementation is based upon the hash list in the
 *                routing utils (src/system_support/routing/utils), but
 *                is available in non-routing builds and does not 
 *                depend on the rest of the XX_ infrastructure. This
 *                version uses L7 data types. I have removed some 
 *                unused options from the original implementation 
 *                such as the ability for the caller to specify his 
 *                own hash function or get key function. There is no support
 *                here for inserting entries in sorted order. 
 *
 *                The hash list in the routing utils uses a single global
 *                lock (using OS_XX_Freeze()). Here, each hash list has
 *                its own binary semaphore to provide synchronization.
 *
 *                Objects that hash to the same hash bucket are linked in
 *                a doubly linked list. The prev and next pointers can be 
 *                part of the object inserted into the list or can be 
 *                provided by the hash list itself. In the former case, the first
 *                two fields of the item structure must be named "next" and 
 *                "prev." In the latter case,
 *                the entry is encapsulated in a container structure that 
 *                provides the next and prev pointers. The hash list allocates
 *                a pool of container structures when containers are used. The 
 *                user must specify the maximum number of entries in the hash 
 *                list in order to size this pool.
 *
 *                The key can be any member or contiguous set of members in
 *                the structure inserted at any location with the structure. 
 *                The hash list will accept multiple entries with the same key, 
 *                but it is an error to add the same object to the hash list multiple 
 *                times. Because the hash list accepts multiple entries with the 
 *                same key, the key is not a unique identifier for an entry. The
 *                only unique identifier is the pointer to the object. Thus, the
 *                hash list is not able to detect the arcane case where an 
 *                object is removed from the list, the memory is reused for a 
 *                new entry (maybe with a different key), and reinserted into
 *                the hash list while an iteration is underway and has cached
 *                a pointer to the object as the current location in the 
 *                iteration. 
 *
 *                The default hash is a checksum of the key, modulo hash table size.
 *
 *                The API provides functions to iterate through all entries in the
 *                hash list (hlGetFirst() and hlGetNext()) or to iterate through those
 *                entries that match a given key (hlFindFirst() and hlFindNext()). 
 *                Both store the current entry in a hash list specific variable. 
 *                If you have two iterations active at the same time, both should 
 *                return all elements in order, but you lose some efficiency (because
 *                we'll have to walk the entire list to find the current object in
 *                some cases). If the hash list changes during iteration, hlGetNext()
 *                or hlFindNext() may return L7_ERROR, for example if the current
 *                entry is deleted. 
 *
 *                Typical usage is as follows:
 *
 *                typedef struct val
 *                {
 *                   struct val  *next;
 *                   struct val *prev;
 *                   etc.
 *                } val;
 *                hlInit(compId, L7_HL_FREE_ON_DEL, keyLen, keyOffset, tableSize, maxEntries, &hl);
 *                valInstance = osapiMalloc(compId, val);
 *                hlInsert(hl, key, valInstance);
 *                hlDelete(hl, key, valInstance);    !! caller doesn't free valInstance; HL frees it.
 *                hlDestroy(hl);
 *       
 *                 - or - 
 *
 *                typedef struct val
 *                {
 *                   etc.     !! no next, prev pointers
 *                } val;
 *                !! ask HL to provide next, prev pointers
 *                hlInit(compId, L7_HL_VAL_CONTAINER, keyLen, keyOffset, tableSize, maxEntries, &hl);
 *                valInstance = osapiMalloc(compId, val);
 *                hlInsert(hl, key, valInstance);
 *                hlDelete(hl, key, valInstance);   
 *                osapiFree(compId, valInstance);   !! caller frees own memory
 *                hlDestroy(hl);
 *
 *                Both L7_HL_FREE_ON_DEL and L7_HL_VAL_CONTAINER can be set.
 *
 * @create       3/10/08
 *
 * @author       Alex Ossinski (original code)
 *               Rob Rice (modified for general L7 use)
 *
 * @end
 *
 * ********************************************************************/



#ifndef _L7_HL_API_H_
#define _L7_HL_API_H_

#include "datatypes.h"

#define L7_HL_SMALL_TABLE_SIZE 17  /* default small table size */
#define L7_HL_LARGE_TABLE_SIZE 139 /* default lagre table size */

typedef enum e_HL_flags
{
  /* Use to indicate that no flags are set */
  L7_HL_FLAGS_NONE = 0x0,

  /* When the hash list is created, set this flag to indicate that each entry 
   * must be encapsulated in a container that provides next and previous pointers
   * to link the value in the linked list off the hash bucket. Don't set 
   * this flag if the structure inserted using hlInsert() has its own forward 
   * and backward links with the names "next" and "prev" and the hash list 
   * can use these pointers to link the object into its own doubly linked list. */
  L7_HL_VAL_CONTAINER = 0x1,          

  /* When removing an entry from the hash list, deallocate memory for that
   * entry. Assumes data was allocated using osapiMalloc(), and therefore frees
   * with osapiFree(). */
  L7_HL_FREE_ON_DEL = 0x2  

} e_HL_flags_t;

/*********************************************************************
 * @purpose          Initialize HL object.
 *
 * @param  compId     @b{(input)}  component ID of component creating HL
 * @param  flags      @b{(input)}  control flags 
 * @param  keyLen     @b{(input)}  key length in bytes
 * @param  keyOffset  @b{(input)}  For entries in the hash list, the number of bytes from
 *                                 the beginning of the item to the beginning of the key.
 * @param  tableSize  @b{(input)}  number of hash buckets. Should be a prime
 *                                 number. Suggested values are L7_HL_SMALL_TABLE_SIZE
 *                                 and L7_HL_LARGE_TABLE_SIZE, but there is no
 *                                 restriction against using other values.
 * @param  maxEntries @b{(input)}  maximum number of entries. Only enforced if 
 *                                 L7_HL_VAL_CONTAINER flag set.
 * @param  p_hl    @b{(output)}    pointer to newly created hash list
 *
 * @returns           L7_SUCCESS      success
 * @returns           L7_NO_MEMORY    can't allocate memory
 * @returns           L7_ERROR        wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_RC_t hlInit(L7_COMPONENT_IDS_t compId, e_HL_flags_t flags, L7_uchar8 keyLen, 
               L7_ushort16 keyOffset, L7_ushort16 tableSize, L7_uint32 maxEntries,
               void **p_hl);

/*********************************************************************
* @purpose            destroy HL object.
*
* @param  p_objId     @b{(input)}  hash list pointer returned from hlInit
*
* @returns            L7_SUCCESS        success
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlDestroy(void *hl);

/*********************************************************************
* @purpose          Insert new entry to the hash list.
*
*
* @param  hl        @b{(input)}  hash list pointer returned from hlInit.
* @param  key       @b{(input)}  key pointer
* @param  value     @b{(input)}  value pointer
*
* @returns          L7_SUCCESS     success
* @returns          L7_ERROR       wrong parameters
* @returns          L7_NO_MEMORY   failed to allocate memory
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlInsert(void *hl, L7_uchar8 *key, void *value);

/*********************************************************************
* @purpose          Delete entry from the hash list.
*
* @param  hl        @b{(input)}  hash list pointer returned from hlInit.
* @param  key       @b{(input)}  key pointer
* @param  value     @b{(output)}  pointer to put value or L7_NULLPTR. If 
*                                 free on delete option is used, this
*                                 is always set to NULL on return.
*
* @returns   L7_SUCCESS     success
*            L7_FAILURE    
*            L7_ERROR       bad parameters 
*            L7_NOT_EXIST   entry wasn't found
*
* @notes     Removes the first entry that matches the key. 
*            Use this form of delete if you need a pointer to the 
*            item removed from the hash list. This delete form is 
*            useful for hash lists created without the
*            L7_HL_FREE_ON_DEL option, where the caller may need to get a 
*            pointer to the object so its memory can be freed.
*
* @end
* ********************************************************************/
L7_RC_t hlDeleteFirst( void *hl, L7_uchar8 *key, void **value);

/*********************************************************************
* @purpose        Remove an entry from the hash list.
*
* @param  hl      @b{(input)}  hash list pointer returned from hlInit.
* @param key      @b{(input)}  key pointer
* @param value    @b{(input)}  if NULL, first entry matching the key is 
*                              deleted. If non-NULL, will delete this
*                              specific item.
*
* @returns    L7_SUCCESS     success
*             L7_ERROR       bad parameters
*             L7_FAILURE    
*             L7_NOT_EXIST   entry wasn't found
*
* @notes      Use this form of delete if you want to identify the specify
*             object to be removed from the hash list. 
*
* @end
* ********************************************************************/
L7_RC_t hlDelete(void *hl, L7_uchar8 *key, void *value);

/*********************************************************************
* @purpose    Returns the number of entries in a hash list
*
* @param  hl   @b{(input)}  hash list pointer returned from hlInit.
*
* @returns     L7_SUCCESS        success
*
* @notes
*
* @end
* ********************************************************************/
L7_ushort16 hlNumEntriesGet(void *hl);

/*********************************************************************
* @purpose     Get first entry in a hash list
*
* @param  hl        @b{(input)}  hash list pointer returned from hlInit.
* @param  p_value   @b{(output)} pointer to put the value
*
* @returns     L7_SUCCESS    success
*              L7_ERROR      bad parameters
*              L7_FAILURE    hash list is empty
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlGetFirst(void *hl, void **p_value);

/*********************************************************************
* @purpose          Get next entry in hash list.
*
*
* @param  hl        @b{(input)}  hash list pointer returned from hlInit.
* @param  curValue  @b{(input)}  give next value after this one
* @param  p_value   @b{(output)} pointer to put the value
*
* @returns          L7_SUCCESS    success
*                   L7_ERROR      invalid params
*                   L7_FAILURE    
*                   L7_NOT_EXIST  end of the hash list
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlGetNext(void *hl, void *curValue, void **p_value);

/*********************************************************************
* @purpose          Find first entry with the given key
*
*
* @param  hl        @b{(input)}  hash list pointer returned from hlInit.
* @param  key       @b{(input)}  key pointer
* @param  p_value   @b{(output)}  pointer to put value or L7_NULLPTR
*
* @returns   L7_SUCCESS    success
*            L7_ERROR      wrong parameters
*            L7_FAILURE    
*            L7_NOT_EXIST  no matching entries
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlFindFirst(void *hl, L7_uchar8 *key, void **p_value);

/*********************************************************************
* @purpose          Find next entry with a given key.
*
*
* @param  hl        @b{(input)}  hash list pointer returned from hlInit.
* @param  key       @b{(input)}  key pointer
* @param  curValue  @b{(input)}  start from this entry
* @param  p_value   @b{(output)}  pointer to put the value or L7_NULLPTR
*
* @returns   L7_SUCCESS  success
*            L7_ERROR    wrong parameters
*            L7_FAILURE  
*            L7_NOT_EXIST   no more matching entries
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlFindNext(void *hl, L7_uchar8 *key, void *curValue, void **p_value);


#endif /* _L7_HL_API_H_ */
