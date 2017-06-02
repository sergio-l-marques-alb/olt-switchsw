/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     hl.ext
 *
 * @purpose      Provides external API for the hash list object
 *
 * @component    Routing
 *
 * @comments
 *
 * @create       12/01/1997
 *
 * @author       Alex Ossinski
 *
 * @end
 *
 * ********************************************************************/



#ifndef _L7_HL_H_
#define _L7_HL_H_

#include "datatypes.h"
#include "commdefs.h"


/* Value structure definition */
typedef struct hl_value_s
{
  struct hl_value_s *next;  /* pointer to the previous entry */
  struct hl_value_s *prev;  /* pointer to next entry */
  void              *value; /* data itself */
} hl_value_t;

/* hash list instance structure */
typedef struct l7_hl_s
{
  L7_ushort16  numBuckets;         /* number of hash buckets */
  hl_value_t **entry;             /* pointer to the entries list   */
  L7_BOOL      delData;             /* free memory data flag */
  L7_BOOL      valContainer;       /* L7_TRUE if entry encapsulated in next/prev container */
  L7_ushort16  maxEntries;          /* maximum number of entries */
  L7_ushort16  numEntries;          /* current number of entries in the hash table */
  L7_uchar8    keyLen;              /* key length in bytes */
  L7_ushort16  keyOffset;           /* key offset in the value */

  /* When iterating through all entries in hash list, getIndex is the index of 
   * the current hash bucket and getValue is the last entry returned. If the 
   * hash list uses value containers, getValue points to the container, 
   * while the user passes its own object in the GetNext(). We need a pointer 
   * to the container to avoid searching for the current value. */
  L7_ushort16  getIndex;            
  hl_value_t  *getValue; 

  /* When iterating through entries that match a given key (hlFindFirst(), hlFindNext()),
   * curValue points to the previous entry returned. */
  hl_value_t  *curValue;           

  L7_COMPONENT_IDS_t compId;       /* component ID of owner of this hash list */
  void        *semId;              /* binary semaphore for data synchronization */
  L7_uint32   bufferPoolId;        /* buffer pool for DLL containers */
} l7_hl_t;


#define GET_VALUE(h,v)((void *)((h)->valContainer ? ((hl_value_t *)(v))->value : v))

#define GET_KEYPOINTER(h,v) ((L7_uchar8 *)(((L7_uchar8 *)(GET_VALUE(h,v))) + (h)->keyOffset))

#define GET_KEY(h,v) GET_KEYPOINTER(h,v)


/* Internal functions, intentionally not declared static so that 
 * they show up properly in backtraces. */
void hlAddToDLList(hl_value_t **head, hl_value_t *object);
void hlDelFromDLList(hl_value_t **head, hl_value_t *object);
L7_ushort16 hlGetIndex(l7_hl_t *p_H, L7_uchar8 *key);
L7_RC_t hlBufferPoolCreate(l7_hl_t *p_H, L7_uint32 num_buffers);
L7_RC_t hlCleanup(l7_hl_t *p_H);


#endif /* _L7_HL_H_ */
