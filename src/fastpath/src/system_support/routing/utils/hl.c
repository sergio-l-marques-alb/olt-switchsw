/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      hl.c
 *
 * @purpose       Hash list object implementation
 *                The code in this file is used by all modes of
 *
 * @component     Routing Utils Component
 *
 * @comments 
 *            HL_Init init hash list object
 * HL_Destroy destroy hash list object
 * HL_Insert  insert a new entry
 * HL_SortedInsert  insert a new entry in the sorted order
 * HL_FindFirst  find first entry
 * HL_FindNextfind next entry
 * HL_DeleteFirstdelete the first entry
 * HL_Delete  delete entry that is equal to the ginev one
 * HL_Browse  browse hash list
 * HL_GetEntriesNmb get number of entries
 * HL_GetFirstget first entry
 * HL_GetNext get next entry
 * HL_Move move entry according to the new key
 * HL_Cleanup free all user/internal allocated entries
 * HL_SetKeyLen  change key length to the new value
 *
 * Internal static routines:
 *     HL_CleanupAllEntries   deallocate etries memory
 *
 * @create       09/01/1997
 *
 * @author       Alex Ossinski
 *
 * @end
 *
 *********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: HL.C 1.0 97/12/14 18:57:17 alexo";
#endif

/* --- standard include files --- */

#include <string.h>
#include "xx.ext"
#include "gservice.ext"
#include "hl.ext"
#include "common.h"

/* Extra debugging for defect 42015 */
#if defined(_L7_OS_VXWORKS_) || defined(_L7_OS_VXWORKS_PNE_)
extern char *sysMemTop (void );
static unsigned int debug_mem_top = 0;
#define LOG_ERROR(error_code)                                                   \
          log_error_code ((unsigned long) (error_code), __FILE__, __LINE__)
extern void log_error_code (unsigned long err_code,
                            unsigned char *file_name,
                            unsigned long line_num);
#define DEBUG_PTR_CHECK(p) \
        if (debug_mem_top == 0) debug_mem_top = (unsigned int)sysMemTop(); \
        if (((unsigned long)(p)) < 0x1000 || ((unsigned long)(p)) >= debug_mem_top) { \
            LOG_ERROR(p); \
        }
#else
#define DEBUG_PTR_CHECK(p)
#endif

/* Value structure definition */
typedef struct tagt_Value
{
   struct tagt_Value *next;  /* pointer to the previous entry */
   struct tagt_Value *prev;  /* pointer to next entry */
   void              *value; /* data itself */
} t_Value;

/* hash list instance structure */
typedef struct tagt_HL
{
#ifdef ERRCHK
   struct tagt_HL *next;
   struct tagt_HL *prev;
#endif
   word       status;              /* current status HL_STAT_Valid */
   word       entryNmb;            /* number of entries in hash list */
   t_Value    **entry;             /* pointer to the entries list   */
   Bool       insertInc;           /* flag that inserting has to be sorted by increment */
   Bool       delData;             /* free memory data flag */
   Bool       dataOffset;          /* handle pointer offset in the entry */
   Bool       keyIsValue;          /* key and value are equal */
   Bool       keyAtStart;          /* key place is at the start of key string flag */
   word       valueNmb;            /* current values number in the table */
   byte       keyLen;              /* key length in bytes */
   word       keyOffset;           /* key offset in the value */
   word       (*f_hash)(byte *);   /* user hash function */
   byte       *(*f_getKey)(void *);/* user get key function */
   word       getIndex;            /* current entry index for the get first/next requests */
   t_Value    *getValue;           /* current value for the get first/next requests */
   t_Value    *curValue;           /* current value pointer for the find request*/
} t_HL;

#ifdef ERRCHK
/* global list of objects for the debuging only */
t_HL *HLObjList;
#endif

#define DEF_HASH_FUN(h,k)\
    ( \
      IndexForMacro = 0,\
      memcpy(&IndexForMacro, k+((((h)->keyLen>4)&&((h)->keyAtStart==0))? (h)->keyLen - 4: 0),(h)->keyLen >=4? 4: (h)->keyLen), \
      ((word)(IndexForMacro % (h)->entryNmb))\
    )
#define GET_VALUE(h,v)((void *)((h)->dataOffset? ((t_Value *)(v))->value: v))

#define GET_KEYPOINTER(h,v) ((byte *)(((byte *)(GET_VALUE(h,v))) + (h)->keyOffset))

#define GET_KEY(h,v) (((h)->f_getKey)? (*(h)->f_getKey)(GET_VALUE(h,v)): (h)->keyIsValue? ((byte *)GET_VALUE(h,v)): GET_KEYPOINTER(h,v))

#define GET_INDEX(h,k) ((word)(((h)->f_hash)? (*(h)->f_hash)(k): DEF_HASH_FUN(h,k)))

#define OFFSET ((word)(sizeof(void *) * 2))

#define HL_STAT_Valid 31901

/* static routines prototype */
static void HL_CleanupAllEntries(t_Handle );


/*********************************************************************
 * @purpose          Initialize HL object.
 *
 *
 * @param  flags      @b{(input)}  control flags SORTED_INSERT,
 *                                SORT_DEC, OWN_LINK
 * @param  keyLen     @b{(input)}  key length in bytes
 * @param  keyOffset  @b{(input)}  key offset in the value structure
 * @param  tableSize  @b{(input)}  hash list table enties number
 * @param  f_hash     @b{(input)}  user hash function or NULLP
 * @param  f_getKey   @b{(input)}  user get key function or NULLP
 * @param  p_objId    @b{(output)}  pointer to put hash list objectId
 *
 * @returns           E_OK        success
 * @returns           E_NOMEMORY  can't allocate memory
 * @returns           E_BADPARAM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Init(byte flags, byte keyLen, word keyOffset, word tableSize,
              word (*f_hash)(byte *), byte *(*f_getKey)(void *),
              t_Handle *p_objId)
{
   t_HL *p_H;
   ASSERT(keyLen);
   ASSERT(tableSize);
   ASSERT(p_objId);

    /* allocate object header memory */
    if((p_H = (t_HL *)XX_Malloc(sizeof(t_HL))) == NULLP)
        return E_NOMEMORY;

    memset (p_H, 0, sizeof(t_HL));

    p_H->entryNmb = tableSize;
    p_H->insertInc = (Bool)((flags & SORTED_DEC)? FALSE : TRUE);
    p_H->delData   = (Bool)((flags & FREE_ON_DEL)? TRUE: FALSE);
    p_H->keyLen = keyLen;
    p_H->keyOffset = keyOffset;
    p_H->f_hash = f_hash;
    p_H->f_getKey = f_getKey;

    if(flags & OWN_LINK)
        p_H->dataOffset = OFFSET;
    if(flags & KEY_IS_VALUE)
        p_H->keyIsValue = TRUE;
    if(flags & KEY_AT_START)
        p_H->keyAtStart = TRUE;

    /* allocate memory for the entries table */
    if((p_H->entry = (t_Value **)XX_Malloc((word)
        (sizeof(t_Value **) *p_H->entryNmb))) == NULLP)
    {
        XX_Free(p_H);
        return E_NOMEMORY;
    }
    memset(p_H->entry, 0, sizeof(t_Value *) * p_H->entryNmb);
    p_H->status = HL_STAT_Valid;
    *p_objId = p_H;
#ifdef ERRCHK
    XX_AddToDLList(p_H, HLObjList);
#endif
    return E_OK;
}


 /*********************************************************************
 * @purpose            destroy HL object.
 *
 *
 * @param  p_objId     @b{(input)}  pointer to get and clear hash list object Id
 *
 * @returns            E_OK        success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Destroy ( t_Handle *p_objId)
{
    t_HL *p_H = (t_HL *)*p_objId;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    if(p_H->dataOffset || p_H->delData)
        HL_CleanupAllEntries(p_H);

    XX_Free(p_H->entry);
    p_H->status = 0;
#ifdef ERRCHK
    XX_DelFromDLList(p_H, HLObjList);
#endif
    XX_Free(p_H);
    *p_objId = NULLP;
    OS_XX_Thaw();
    return E_OK;
}


 /*********************************************************************
 * @purpose          Insert new entry to the hash list.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(input)}  value pointer
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Insert( t_Handle objId, byte *key, void *value)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V = (t_Value *)value;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

    index = GET_INDEX(p_H, key);

    /* allocate entry memory if necessary */
    if(p_H->dataOffset)
    {
        if((p_V = (t_Value *)XX_Malloc(sizeof(t_Value))) == NULLP)
		{
			OS_XX_Thaw();
            return E_NOMEMORY;
		}
        p_V->value = value;
    }

#ifdef ERRCHK
      /* check may be this same value is included already */
    else
    {
       t_Value *ctr = p_H->entry[index];
       for(;ctr != NULLP; ctr = ctr->next)
       {
          if(ctr == p_V)
             ASSERT(0); /* Yes !!! */
       }
    }
#endif
    /* add value to the entry */
    p_V->prev = p_V->next = NULLP;
    XX_AddToDLList(p_V, p_H->entry[index]);
    p_H->valueNmb++;
    OS_XX_Thaw();
    return E_OK;
}


 /*********************************************************************
 * @purpose          Insert new entry in sorted order to the hash list.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(input)}  value pointer
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_SortedInsert( t_Handle objId, byte *key, void *value)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V = (t_Value *)value;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    index = GET_INDEX(p_H, key);

    /* allocate entry memory if necessary */
    if(p_H->dataOffset)
    {
        if((p_V = (t_Value *)XX_Malloc(sizeof(t_Value))) == NULLP)
		{
			OS_XX_Thaw();
            return E_NOMEMORY;
		}
        p_V->value = value;
    }

    /* add value to the entry */
    if(p_H->entry[index])
    {
        /* entry has other values already */
        t_Value *p_CV = p_H->entry[index];
        t_Value *p_TmpCv = p_CV;
        if(p_H->insertInc)
        {
            /* find nearest entry for the inserting */
            while(p_CV &&
                memcmp(GET_KEY(p_H, p_CV),
                GET_KEY(p_H, p_V), p_H->keyLen) < 0)
            {
                p_TmpCv = p_CV;
                p_CV= p_CV->next;
            }
        }
        else
        {
            /* find nearest entry for the inserting */
            while(p_CV &&
                memcmp(GET_KEY(p_H, p_CV),
                GET_KEY(p_H, p_V), p_H->keyLen) > 0)
            {
                p_TmpCv = p_CV;
                p_CV= p_CV->next;
            }
        }

        if(p_CV)
        {
           /* insert before current */
           p_V->next = p_CV;
           p_V->prev = p_CV->prev;
           if(p_CV->prev)
               p_CV->prev->next = p_V;
           p_CV->prev = p_V;
           if(p_H->entry[index] == p_CV)
              p_H->entry[index] = p_V;
        }
        else
        {
           /* insert after last non-null */
           p_V->next = p_TmpCv->next;
           p_V->prev = p_TmpCv;
           p_TmpCv->next = p_V;
        }
    }
    else
    {
        /* entry is empty */
        p_V->next = p_V->prev = NULLP;
        p_H->entry[index] = p_V;
    }
    p_H->valueNmb++;
    OS_XX_Thaw();
    return E_OK;
}

/*********************************************************************
 * @purpose          Change the value in a hash list entry.
 *
 * @param  objId     @b{(input)}  hash list pointer
 * @param  key       @b{(input)}  key to find existing entry
 * @param  value     @b{(input)}  item replacing existing entry
 *
 * @returns          E_OK         success
 *                   E_NOT_FOUND  if no entry matches key
 * @returns          E_BADPARAM   wrong parameters
 *
 * @notes            Faster then deleting an entry and inserting a new
 *                   one if both entries have the same key.
 *
 * @end
 * ********************************************************************/
e_Err HL_Change(t_Handle objId, byte *key, void *value)
{
  t_HL    *p_H = (t_HL *)objId;
  t_Value *p_V;
  word index;
  ulng IndexForMacro;
  int cmpval;

  OS_XX_Freeze();

	if (!p_H || (p_H->status != HL_STAT_Valid))
	{
		OS_XX_Thaw();
		return E_FAILED;
	} 

  /* Find existing entry */
  index = GET_INDEX(p_H, key);
  p_V = p_H->entry[index];  /* head of hash bucket */

  while (p_V)
  {
    if (memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
      break;
    p_V = p_V->next;
  }
  if (!p_V)
  {
    /* No existing entry matches key */
    OS_XX_Thaw();
    return E_NOT_FOUND;
  }

  /* Replace value */
  if (p_H->dataOffset)
  {
    /* container is already linked into list. */
    p_V->value = value;
  }
  else
  {
    /* Need to set next/prev pointers on value and next and prev items */
    XX_DelFromDLList(p_V, p_H->entry[index]);
    if (p_H->delData)
    {
      XX_Free(p_V);
    }
    p_V = (t_Value*) value;
    p_V->prev = p_V->next = NULLP;
    XX_AddToDLList(p_V, p_H->entry[index]);
  }

  /* Make sure keys match */
  cmpval = memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen);
  ASSERT(cmpval == 0);
  
  OS_XX_Thaw();
  return E_OK;
}

 /*********************************************************************
 * @purpose          Find first entry with the given key
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  p_value   @b{(input)}  pointer to put value or NULLP
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters
 * @returns          E_FAILED    value wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_FindFirst( t_Handle objId, byte *key, void **p_value)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    index = GET_INDEX(p_H, key);
    p_V = p_H->entry[index];

    while(p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
            break;
        p_V = p_V->next;
    }
    if(!p_V)
	{
		OS_XX_Thaw();
        return E_FAILED;
	}
    if(p_value != NULLP)
        *p_value = GET_VALUE(p_H, p_V);
    p_H->curValue = p_V;
    OS_XX_Thaw();
    return E_OK;
}


 /*********************************************************************
 * @purpose          Find next entry from the given value.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  curValue  @b{(input)}  start from this entry
 * @param  p_value   @b{(input)}  pointer to put the value or NULLP
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters
 * @returns          E_FAILED    value wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_FindNext( t_Handle objId, byte *key, void *curValue, void **p_value)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    if(curValue != (void *)GET_VALUE(p_H, p_H->curValue))
    {
        /* find current value */
        index = GET_INDEX(p_H, key);
        p_V = p_H->entry[index];

        while(p_V)
        {
            if(memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0
                && GET_VALUE(p_H, p_V) == curValue)
            {
                p_H->curValue = p_V;
                break;
            }
            p_V = p_V->next;
        }
        if(!p_V)
		{
			OS_XX_Thaw();
            return E_BADPARM;
		}
    }
    else
        p_V = p_H->curValue;

    p_V = p_V->next;

    /* find next value */
    while(p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
        {
            p_H->curValue = p_V;
            break;
        }
        p_V = p_V->next;
    }
    if(!p_V)
	{
		OS_XX_Thaw();
        return E_FAILED; /* not found */
	}
    if(p_value != NULLP)
        *p_value = GET_VALUE(p_H, p_V);
    OS_XX_Thaw();
    return E_OK;
}


 /*********************************************************************
 * @purpose          Delete entry from the hash list.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(output)}  pointer to put value or NULLP
 *
 * @returns          E_OK        success
 * @returns          E_FAILED    entry wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_DeleteFirst( t_Handle objId, byte *key, void **value)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();


	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    index = GET_INDEX(p_H, key);

    /* find given entry */
    p_V = p_H->entry[index];
    while (p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
                break;
        p_V = p_V->next;
    }
    if(!p_V)
    {
        OS_XX_Thaw();
        return E_FAILED;
    }

    if(value )
        *value = GET_VALUE(p_H, p_V);

    /* delete value */
    XX_DelFromDLList(p_V, p_H->entry[index]);
    if(p_H->dataOffset ||(p_H->delData && value == NULLP))
        XX_Free(p_V);

    p_H->valueNmb--;

    OS_XX_Thaw();
    return E_OK;
}


 /*********************************************************************
 * @purpose        Delete next entry from the given one.
 *
 *
 * @param objId    @b{(input)}  object Id
 * @param key      @b{(input)}  key pointer
 * @param value    @b{(input)}  given entry pointer
 *
 * @returns        E_OK        success
 * @returns        E_FAILED    entry wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Delete( t_Handle objId, byte *key, void *value)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    index = GET_INDEX(p_H, key);

    /* find given entry */
    p_V = p_H->entry[index];
    while (p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
        {
           if(!value)
              break;
           if(value == GET_VALUE(p_H, p_V))
              break;
        }
        p_V = p_V->next;
    }
    if(!p_V)
    {

        OS_XX_Thaw();
        return E_FAILED;
    }

    /* delete value */
    XX_DelFromDLList(p_V, p_H->entry[index]);
    if(p_H->dataOffset || p_H->delData)
        XX_Free(p_V);
    p_H->valueNmb--;

    OS_XX_Thaw();
    return E_OK;
}

 /*********************************************************************
 * @purpose           Go through all the entry and call user browse
 *                    function; if user browse function returned FALSE
 *                    delete current entry.
 *
 *
 * @param  objId      @b{(input)}  hash list object Id
 * @param  f_browse   @b{(input)}  user browse function
 * @param  flag       @b{(input)}  flag to put as a parameter to the browse function
 *
 * @returns           E_OK        success
 * @returns           E_BADPARAM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Browse( t_Handle objId, Bool (*f_browse)(void *,ulng),
                ulng flag)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V, *p_tmpV;
    word index;

	if (!p_H)
	{
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		return E_FAILED;
	}

    ASSERT(f_browse);

    if(!p_H->valueNmb)
        return E_OK; /* hash list is empty */

    for(index = 0; index < p_H->entryNmb; index++)
    {
        p_V = p_H->entry[index];
        while(p_V)
        {
            t_Value *tmp_prev = p_V->prev, *tmp_next = p_V->next;
            p_tmpV = p_V->next;
            if(!f_browse(GET_VALUE(p_H, p_V), flag))
            {
                /* delete entry */
               OS_XX_Freeze();
               if(tmp_prev != NULLP)
                  tmp_prev->next = tmp_next;
               else
                  p_H->entry[index] = tmp_next;
               if(tmp_next != NULLP)
                  tmp_next->prev = tmp_prev;
               OS_XX_Thaw();

               if(p_H->dataOffset != NULLP || p_H->delData)
                  XX_Free(p_V);
               p_H->valueNmb--;
            }
            p_V = p_tmpV;
        }
    }
    return E_OK;
}


 /*********************************************************************
 * @purpose               returns number of active entries in a hash list
 *
 *
 * @param  objId          @b{(input)}  hash list object Id
 * @param  p_entriesNmb   @b{(output)}  pointer to put return value
 *
 * @returns               E_OK        success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_GetEntriesNmb(t_Handle objId, word *p_entriesNmb)
{
    t_HL    *p_H = (t_HL *)objId;

	if (!p_H)
	{
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		return E_FAILED;
	}

    *p_entriesNmb = p_H->valueNmb;
    return E_OK;
}


 /*********************************************************************
 * @purpose            Get first entry from the hash list
 *
 *
 * @param  objId       @b{(input)}  hash list object Id
 * @param  p_value     @b{(output)}  pointer to put the value
 *
 * @returns            E_OK        success
 * @returns            E_FAILED    hash list is empty
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_GetFirst(t_Handle objId, void **p_value)
{
    t_HL    *p_H = (t_HL *)objId;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}
    DEBUG_PTR_CHECK(p_H);

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

    p_H->getIndex = 0;
    if(!p_H->valueNmb)
    {
        /* hash list is empty */
        p_H->getValue = NULLP;

        OS_XX_Thaw();
        return E_FAILED;
    }
    for(p_H->getValue = p_H->entry[p_H->getIndex]; p_H->getIndex
        < p_H->entryNmb; p_H->getIndex++)
    {
        p_H->getValue = p_H->entry[p_H->getIndex];
        if(p_H->getValue)
        {
          *p_value = GET_VALUE(p_H, p_H->getValue);
          OS_XX_Thaw();
          return E_OK;
        }
    }
    
    OS_XX_Thaw();
    return E_FAILED;
}


 /*********************************************************************
 * @purpose          Initialize HL object.
 *
 *
 * @param  objId     @b{(input)}   hash list object Id
 * @param  p_value   @b{(output)}   pointer to put the value
 * @param  curValue  @b{(input)}   give next value after this one
 *
 * @returns          E_OK        success
 * @returns          E_FAILED    end of the hash list
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_GetNext(t_Handle objId, void **p_value, void *curValue)
{
    t_HL    *p_H = (t_HL *)objId;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}
    DEBUG_PTR_CHECK(p_H);

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}


    if((p_H->getValue == NULLP) || (GET_VALUE(p_H, p_H->getValue) != curValue))
    {
        /* find current value */
        for(p_H->getIndex = 0; p_H->getIndex < p_H->entryNmb; p_H->getIndex++)
        {
            p_H->getValue = p_H->entry[p_H->getIndex];
            while (p_H->getValue)
            {
                if(GET_VALUE(p_H, p_H->getValue) == (t_Value *)curValue )
                    break;
                p_H->getValue = p_H->getValue->next;
            }
            if(p_H->getValue)
                break;
        }
        if(!p_H->getValue)
        {

            OS_XX_Thaw();
            return E_FAILED;
        }
    }
    /* try to get next value in this same entry */
    if((p_H->getValue = p_H->getValue->next) != 0)
    {
        *p_value = GET_VALUE(p_H, p_H->getValue);
        ASSERT(*p_value != curValue);

        OS_XX_Thaw();
        return E_OK;
    }
    /* try to find next not empty entry */
    p_H->getIndex++;
    for( p_H->getValue = NULLP;
    p_H->getIndex < p_H->entryNmb; p_H->getIndex++)
    {
        if((p_H->getValue = p_H->entry[p_H->getIndex]) != NULLP)
           break;
    }
    /* do we get it ? */
    if(p_H->getValue)
    {
        *p_value = GET_VALUE(p_H, p_H->getValue);

         OS_XX_Thaw();
         return E_OK;
    }
    /* this is the end of the hash list */
    p_H->getIndex = 0;

    OS_XX_Thaw();
    return E_FAILED;
}


 /*********************************************************************
 * @purpose             Move the value to other entry according to the
 *                      new key
 *
 *
 * @param  objId        @b{(input)}  hash list object Id
 * @param  oldKey       @b{(input)}  key to find entry
 * @param  value        @b{(input)}  entry value
 * @param  newKey       @b{(input)}  new key to be changed and moved
 *
 * @returns             E_OK        success
 * @returns             E_FAILED    entry not found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Move(t_Handle objId, byte *oldKey, void *value, byte *newKey)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    OS_XX_Freeze();

	if (!p_H)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

	if (p_H->status != HL_STAT_Valid)
	{
		OS_XX_Thaw();
		return E_FAILED;
	}

    ASSERT(value);
    ASSERT(oldKey);
    ASSERT(newKey);

    index = GET_INDEX(p_H, oldKey);
    p_V = p_H->entry[index];

    /* try to find value in the hash list */
    while(p_V)
    {
        if(memcmp(GET_KEY(p_H, p_V), oldKey, p_H->keyLen) == 0 &&
            (t_Value *)value == p_V)

            break;
        p_V = p_V->next;
    }
    if(!p_V)
    {

        OS_XX_Thaw();
        return E_FAILED; /* old value wasn't fount */
    }

    /* delete the value from the old place */
    XX_DelFromDLList(p_V, p_H->entry[index])

    /* insert the value according to the new key */
    index = GET_INDEX(p_H, newKey);
    XX_AddToDLList(p_V, p_H->entry[index]);

    OS_XX_Thaw();
    return E_OK;

}


 /*********************************************************************
 * @purpose           free all user/internal allocated entries
 *
 *
 * @param  objId      @b{(input)}  hash list object Id
 * @param  delflag    @b{(input)}  entry's value delete flag
 *
 * @returns           n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void HL_Cleanup(t_Handle objId, Bool delflag)
{
    t_HL    *p_H = (t_HL *)objId;
    t_Value *value, *next_value;
    word    index;

    OS_XX_Freeze();

    for(index = 0; index < p_H->entryNmb; index ++)
    {
       value = p_H->entry[index];
       while(value)
       {
          next_value = value->next;

          if(delflag)
             XX_Free(GET_VALUE(p_H, value));

          if(p_H->dataOffset != NULLP)
             XX_Free(value);

          p_H->valueNmb -= 1;
          p_H->entry[index] = NULLP;
          value = next_value;
       }
    }

    OS_XX_Thaw();
}


 /*********************************************************************
 * @purpose          Change HL key length to the new value
 *
 *
 * @param  objId     @b{(input)}  hash list object Id
 * @param  keyLen    @b{(input)}  the new value of key length
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void HL_SetKeyLen(t_Handle objId, byte keyLen)
{
    t_HL    *p_H = (t_HL *)objId;

    p_H->keyLen = keyLen;
}

/* Glue code for _Find method */
e_Err  HL_FindGlue (IN  t_Handle  objId,
                    IN  Bool      flag,
                    IN  byte      *p_key,
                    OUT void      **pp_value)
{
   return HL_FindFirst(objId, p_key, pp_value);
}

/* Glue code for _FindBrowse method */
e_Err HL_FindBrowseGlue (IN  t_Handle      objId,
                         IN  byte          *key,
                         IN  GS_BROWSEFUNC f_browse,
                         IN  ulng          param)
{
   t_Handle value;
   e_Err    rc;

   for ( rc = HL_FindFirst(objId, key, (void**)&value);
         rc == E_OK;
         rc = HL_FindNext(objId, key, (void*)value, (void**)&value) )
   {
      if (!f_browse((byte*)value, param) )
      {
         /* delete entry */
         HL_Delete(objId, key, (void *)value);
      }
   }

   return E_OK;
}
 /*********************************************************************
 * @purpose     Generic constructor (see type GS_Constructor in gservice.ext)
 *
 *
 * @param dParam     @b{(input)} set of input parameters to construct object;
 * @param fTarg      @b{(input)} handle of the set of object methods that to
 *                               be filled by constructor;
 * @param pp_objId   @b{(output)} pointer to handle of the object created by
 *                               constructor.
 *
 * @returns     E_OK - success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HL_Constructor  (IN     t_Handle             dParam,
                       IN OUT t_Handle             fTarg,
                       OUT    t_Handle             *pp_objId)
{
   t_ConstrDataParams *params  = (t_ConstrDataParams *)dParam;
   t_GServiceAspect   *p_fTarg = (t_GServiceAspect *)fTarg;

   if ( !p_fTarg->f_Destruct )
      p_fTarg->f_Destruct      = (GS_Destruct)HL_Destroy;
   if ( !p_fTarg->f_Insert )
      p_fTarg->f_Insert        = (GS_Insert)HL_Insert;
   if ( !p_fTarg->f_Delete )
      p_fTarg->f_Delete        = (GS_Delete)HL_DeleteFirst;
   if ( !p_fTarg->f_Find )
      p_fTarg->f_Find          = (GS_Find)HL_FindGlue;
   if ( !p_fTarg->f_FindNext )
      p_fTarg->f_FindNext      = (GS_FindNext)HL_FindNext;
   if ( !p_fTarg->f_GetNext )
      p_fTarg->f_GetNext       = (GS_GetNext)HL_GetNext;
   if ( !p_fTarg->f_GetFirst )
      p_fTarg->f_GetFirst      = (GS_GetFirst)HL_GetFirst;
   if ( !p_fTarg->f_GetEntriesNmb )
      p_fTarg->f_GetEntriesNmb = (GS_GetEntriesNmb)HL_GetEntriesNmb;
   if ( !p_fTarg->f_Browse )
      p_fTarg->f_Browse        = (GS_Browse)HL_Browse;
   if ( !p_fTarg->f_FindBrowse )
      p_fTarg->f_FindBrowse    = (GS_FindBrowse)HL_FindBrowseGlue;
   if ( !p_fTarg->f_SortedInsert )
      p_fTarg->f_SortedInsert  = (GS_SortedInsert)HL_SortedInsert;
   if ( !p_fTarg->f_Delete )
      p_fTarg->f_Delete        = (GS_Delete)HL_DeleteFirst;
   if ( !p_fTarg->f_Delete )
      p_fTarg->f_DeleteFixed   = (GS_DeleteFixed)HL_Delete;
   if ( !p_fTarg->f_Move )
      p_fTarg->f_Move          = (GS_Move)HL_Move;
   if ( !p_fTarg->f_Cleanup )
      p_fTarg->f_Cleanup       = (GS_Cleanup)HL_Cleanup;

   return HL_Init ((byte)params->flags, (byte)params->KeyLen, params->KeyOffset,
                   (word)params->TableSize, params->f_Hash, params->f_GetKey, pp_objId);
}


 /*********************************************************************
 * @purpose        deallocate memory for all etries
 *
 *
 * @param  p_H     @b{(input)}  hash list object pointer
 *
 * @returns        n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void HL_CleanupAllEntries(t_Handle hnd)
{
    t_HL    *p_H = (t_HL *)hnd;
    t_Value *value, *next_value;
    word    index;

    for(index = 0; index < p_H->entryNmb; index ++)
    {
        value = p_H->entry[index];
        while(value)
        {
            next_value = value->next;
            XX_Free(value);
            value = next_value;
        }
    }
}
/******************end of file**************************************/
