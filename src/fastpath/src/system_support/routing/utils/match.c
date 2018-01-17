/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename          match.c
 *
 * @purpose          Match Table implementation
 *
 * @component        Routing Utils Component
 *
 * @comments
 *    External Routines:
 *
 * MT_Construct
 * MT_Destruct
 * MT_Insert
 * MT_SortedInsert
 * MT_FindFirst
 * MT_FindNext
 * MT_DeleteFirst
 * MT_Delete
 * MT_Browse
 * MT_FindBrowse
 * MT_GetEntriesNmb
 * MT_GetFirst
 * MT_GetNext
 * MT_Move
 * MT_DisplayMTs
 *
 * @create          08/09/98
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\match.c 1.1.3.1 2002/02/13 23:06:26EST ssaraswatula Development  $";
#endif

#include <string.h>
#include "std.h"
#include "xx.ext"
#include "match.ext"

/* Value list definition */
typedef struct t_Value
{
   struct t_Value *prev;    /* pointer to next entry         */
   struct t_Value *next;    /* pointer to the previous entry */
   byte           data[1];  /* data itself                   */
}t_Value;

/* Match table header definition */
typedef struct t_MTH
{
#ifdef MATCHK
   struct t_MTH      *next;
   struct t_MTH      *prev;
   word              allocLine;
   word              freeLine;
   const char        *allocFile;
   const char        *freeFile;
#endif
#ifdef ERRCHK
   word       status;        /* current status XX_STAT_Valid                   */
#endif
   word       entryNmb;      /* number of entries in the table                 */
   t_Value    **entry;       /* pointer to the entries table                   */
   Bool       returnPointer; /* flag that return value should be a pointer     */
   Bool       keyIsPointer;  /* flag that key is a pointer else - a value      */
   Bool       insertInc;     /* flag that insert should be sorted by increment */
   word       valueNmb;      /* current values number in the table             */
   word       keyLen;        /* key length in bytes                            */
   word       valueLen;      /* value length in bytes                          */
   HASHFUNC   f_Hash;        /* pointer to user defined hash function          */
   MATCHFUNC  f_Match;       /* pointer to user defined Match function         */
   word       getIndex;      /* current entry index (MT_GetFirst, MT_GetNext)  */
   t_Value    *getValue;     /* current value for the get first/next requests  */
   t_Value    *curValue;     /* current value pointer for the find request     */
   t_Handle   mutex;         /* MT object mutex                                */
}t_MTH;

/* Get key MACRO */
#define GET_KEY(h,d) (h)->f_Match? (h)->f_Match(d):\
    (h)->keyIsPointer? (byte *)(*((t_Handle *)&d[(h)->valueLen])): \
    (&(d)[(h)->valueLen])

/* Default hash function MACRO */
#define DEF_HASH_FUN(h,k)\
    ( \
      IndexForMacro = 0,\
      memcpy(&IndexForMacro, k+((h)->keyLen>=4? (h)->keyLen - 4: 0),(h)->keyLen >=4? 4: (h)->keyLen), \
      ((word)(IndexForMacro % (h)->entryNmb))\
    )

/* Get entry index MACRO */
#define GET_INDEX(h,k) ((word)(((h)->f_Hash)? (*(h)->f_Hash)(k): DEF_HASH_FUN(h,k)))

/* Put value according to the configuration MACRO */
#define PUT_VALUE(h,v,p) \
    (h)->returnPointer? *(p) = (byte *)&((v)->data): \
    memcpy((p),(v)->data, (h)->valueLen)

#ifdef MATCHK
#include <stdio.h>
t_MTH *MTObjList = NULLP;
#endif

/* static routines prototype */
void MT_CleanupAllEntries(t_Handle );

/*  Match Table API Calls */


/*********************************************************************
 * @purpose            Initialize MT object.
 *
 *
 * @param  keyLen      @b{(input)}  key length in bytes
 * @param  valueLen    @b{(input)}  value length in bytes
 * @param  tableSize   @b{(input)}  number of linked lists
 * @param  f_hash      @b{(input)}  user hash function or NULLP
 * @param  f_match     @b{(input)}  user match function or NULLP
 * @param  p_objId     @b{(output)}  pointer to put successfully created
 *                                  MT object
 *
 * @returns            E_OK        success
 * @returns            E_NOMEMORY  can't allocate memory
 * @returns            E_BADPARM   invalid parameter(s)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#ifdef MATCHK
e_Err _MT_Construct(word keyLen, word valueLen,
                   word tableSize, HASHFUNC f_hash,
                   MATCHFUNC f_match, t_Handle *p_objId,
                   const char *file, word line)
#else
e_Err MT_Construct(word keyLen, word valueLen,
                   word tableSize, HASHFUNC f_hash,
                   MATCHFUNC f_match, t_Handle *p_objId)
#endif
{
   t_MTH *p_H;
   ASSERT(keyLen);
   ASSERT(tableSize);
   ASSERT(p_objId);
   ASSERT(valueLen);

   /* Allocate MT object header */
   if((p_H = (t_MTH *)XX_Malloc(sizeof(t_MTH))) == NULLP)
      return E_NOMEMORY;

   memset (p_H, 0, sizeof(t_MTH));

   p_H->entryNmb = tableSize;
   p_H->keyLen = keyLen & 0xff;
   p_H->valueLen = valueLen;
   p_H->f_Hash = f_hash;
   p_H->f_Match = f_match;
   p_H->insertInc = (keyLen & INSERT_INC) !=0? TRUE: FALSE;
   p_H->keyIsPointer = (keyLen & KEY_IS_POINTER) == 0? FALSE: TRUE;
   p_H->returnPointer =(keyLen & RETURN_POINTERS)== 0? FALSE: TRUE;


   /* Allocate MT object body */
   if((p_H->entry = (t_Value **)XX_Malloc((word)
      (sizeof(t_Value **) *p_H->entryNmb))) == NULLP)
   {
      XX_Free(p_H);
      return E_NOMEMORY;
   }
   XX_CreateMutex(&p_H->mutex);
   memset(p_H->entry, 0, sizeof(t_Value *) * p_H->entryNmb);
#ifdef ERRCHK
   p_H->status = XX_STAT_Valid;
#endif
   *p_objId = p_H;
#ifdef MATCHK
   p_H->allocFile = file;
   p_H->allocLine = line;
   XX_AddToDLList(p_H, MTObjList);
#endif
   return E_OK;
}


/*********************************************************************
 * @purpose     Destroy MT object.
 *
 *
 * @param       @b{(input)}  pointer to MT object handle to be
 *                           destroyd 
 *
 * @returns     E_OK        success
 * @returns     E_BADPARM   wrong handle
 *
 * @notes
 *
 * @end
 * ********************************************************************/      
#ifdef MATCHK
e_Err _MT_Destruct ( t_Handle *p_objId, const char *file, word line)
#else
e_Err MT_Destruct ( t_Handle *p_objId)
#endif
{
    t_MTH *p_H = (t_MTH *)*p_objId;
    
    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    MT_CleanupAllEntries(p_H);
    XX_DeleteMutex(&p_H->mutex);

    XX_Free(p_H->entry);
#ifdef ERRCHK
    p_H->status = 0;
#endif
#ifdef MATCHK
    p_H->freeFile = file;
    p_H->freeLine = line;
    XX_DelFromDLList(p_H, MTObjList);
#endif
    XX_Free(p_H);
    *p_objId = NULLP;
    return E_OK;
}


/*********************************************************************
 * @purpose          Initialize new entry in regular (non-sorted way)
 *
 *
 * @param  objId     @b{(input)} MT object handle
 * @param  key       @b{(input)} pointer to key
 * @param  value     @b{(input)} pointer to value
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_Insert( t_Handle objId, byte *key, byte *value)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index, memSize;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    index = GET_INDEX(p_H, key);

    /* Calculate memory size to store value */
    memSize = p_H->f_Match != NULLP? p_H->valueLen + sizeof(t_Value)- 1:
    p_H->keyIsPointer  ? p_H->valueLen + sizeof(void *) + sizeof(t_Value) - 1:
    p_H->valueLen + p_H->keyLen + sizeof(t_Value) - 1;

    /* Allocate memory for new entry */
    if((p_V = (t_Value *)XX_Malloc(memSize)) == NULLP)
        return E_NOMEMORY;

    /* Copy value */
    memcpy(p_V->data, value, p_H->valueLen);
    if(p_H->f_Match == NULLP)
    {
       /* Store key */
       if(p_H->keyIsPointer)
          memcpy(p_V->data + p_H->valueLen, &key, sizeof(void *));
       else
          memcpy(p_V->data + p_H->valueLen, key, p_H->keyLen);
    }

    /* Add entry to MT's linked list */
    p_V->prev = p_V->next = NULLP;
    XX_AddToDLList(p_V, p_H->entry[index]);
    p_H->valueNmb++;
    return E_OK;
}


/*********************************************************************
 * @purpose          Initialize new entry in sorted way
 *
 *
 * @param  objId     @b{(input)} MT object handle
 * @param  key       @b{(input)} pointer to key
 * @param  value     @b{(input)} pointer to value
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_SortedInsert( t_Handle objId, byte *key, byte *value)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index, memSize;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    index = GET_INDEX(p_H, key);

    /* Calculate memory size to store value */
    memSize = p_H->f_Match != NULLP? p_H->valueLen + sizeof(t_Value)- 1:
    p_H->keyIsPointer  ? p_H->valueLen + sizeof(void *) + sizeof(t_Value) - 1:
    p_H->valueLen + p_H->keyLen + sizeof(t_Value) - 1;

    /* Allocate memory for new entry */
    if((p_V = (t_Value *)XX_Malloc(memSize)) == NULLP)
        return E_NOMEMORY;

    /* Copy value */
    memcpy(p_V->data, value, p_H->valueLen);
    if(p_H->f_Match == NULLP)
    {
       /* Store key */
       if(p_H->keyIsPointer)
          memcpy(p_V->data + p_H->valueLen, &key, sizeof(void *));
       else
          memcpy(p_V->data + p_H->valueLen, key, p_H->keyLen);
    }

    /* Add entry to MT's linked list */
    XX_Lock(p_H->mutex);
    if(p_H->entry[index] != NULLP)
    {
        /* Non-empty linked list */
        t_Value *p_CV = p_H->entry[index];
        t_Value *p_TmpCv = p_CV;
        if(p_H->insertInc)
        {
            /* find nearest entry for the inserting */
            while(p_CV &&
                memcmp(GET_KEY(p_H, p_CV->data),
                GET_KEY(p_H, p_V->data), p_H->keyLen) < 0)
            {
                p_TmpCv = p_CV;
                p_CV= p_CV->next;
            }
        }
        else
        {
            /* find nearest entry for the inserting */
            while(p_CV &&
                memcmp(GET_KEY(p_H, p_CV->data),
                GET_KEY(p_H, p_V->data), p_H->keyLen) > 0)
            {
                p_TmpCv = p_CV;
                p_CV= p_CV->next;
            }
        }

        if(p_CV)
        {
           /* insert before current */
           XX_Freeze();
           p_V->next = p_CV;
           p_V->prev = p_CV->prev;
           if(p_CV->prev)
               p_CV->prev->next = p_V;
           p_CV->prev = p_V;
           if(p_H->entry[index] == p_CV)
              p_H->entry[index] = p_V;
           XX_Thaw();
        }
        else
        {
           /* insert after last non-null */
           XX_Freeze();
           p_V->next = p_TmpCv->next;
           p_V->prev = p_TmpCv;
           p_TmpCv->next = p_V;
           XX_Thaw();
        }
    }
    else
    {
        /* Empty linked list */
        XX_Freeze();
        p_V->next = p_V->prev = NULLP;
        p_H->entry[index] = p_V;
        XX_Thaw();
    }
    p_H->valueNmb++;
    XX_Unlock(p_H->mutex);
    return E_OK;
}


/*********************************************************************
 * @purpose          Find first entry which hits specified key
 *
 *
 * @param  objId     @b{(input)} MT object handle
 * @param  key       @b{(input)} pointer to key
 * @param  value     @b{(output)} pointer to return value or NULLP
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM   wrong parameters
 * @returns          E_FAILED    value wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_FindFirst( t_Handle objId, byte *key, byte **p_value)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    index = GET_INDEX(p_H, key);
    p_V = p_H->entry[index];

    XX_Lock(p_H->mutex);
    while(p_V != NULLP)
    {
        if(memcmp(key, GET_KEY(p_H, p_V->data), p_H->keyLen) == 0)
            break;
        p_V = p_V->next;
    }
    if(!p_V)
    {
        XX_Unlock(p_H->mutex);
        return E_FAILED;
    }
    if(p_value != NULLP)
        PUT_VALUE(p_H, p_V, p_value);
    p_H->curValue = p_V;
    XX_Unlock(p_H->mutex);
    return E_OK;
}
/*********************************************************************
 * @purpose          Find entry which hits specified key next after
 *                   previously found in MT_FindFirst / MT_FindNext loop
 *
 *
 * @param objId      @b{(input)}  MT object handle
 * @param key        @b{(input)}  pointer to key
 * @param p_value    @b{(output)}  pointer to return value or NULLP
 * @param curValue   @b{(input)}  start from entry with this value
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM   wrong parameters
 * @returns          E_FAILED    value wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_FindNext( t_Handle objId, byte *key, byte **p_value, byte **curValue)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    XX_Lock(p_H->mutex);
    if(memcmp(p_H->curValue->data,curValue, p_H->valueLen) !=0 )
    {
        /* Find entry with *curValue value */
        index = GET_INDEX(p_H, key);
        p_V = p_H->entry[index];

        while(p_V != NULLP)
        {
            if(memcmp(key, GET_KEY(p_H, p_V->data), p_H->keyLen) == 0
                && memcmp(p_V->data, curValue, p_H->valueLen)==0)
            {
                p_H->curValue = p_V;
                break;
            }
            p_V = p_V->next;
        }
        if(!p_V)
        {
            XX_Unlock(p_H->mutex);
            return E_BADPARM;
        }
        p_H->curValue = p_V;
    }
    p_V = p_H->curValue->next;

    /* Resume to find entry with specified key */
    while(p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V->data), p_H->keyLen) == 0)
        {
            p_H->curValue = p_V;
            break;
        }
        p_V = p_V->next;
    }
    if(!p_V)
    {
        XX_Unlock(p_H->mutex);
        return E_FAILED; /* not found */
    }
    if(p_value != NULLP)
        PUT_VALUE(p_H, p_V, p_value);
    XX_Unlock(p_H->mutex);
    return E_OK;
}
/*********************************************************************
 * @purpose        Delete entry from MT object
 *
 *
 * @param objId    @b{(input)} MT object handle
 * @param key      @b{(input)} pointer to key
 * @param value    @b{(output)} pointer to return value or NULLP
 *
 * @returns        E_OK        success
 * @returns        E_FAILED    entry wasn't found
 * @returns        E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_DeleteFirst( t_Handle objId, byte *key, byte **value)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    XX_Lock(p_H->mutex);
    index = GET_INDEX(p_H, key);

    /* Find first entry with specified key */
    p_V = p_H->entry[index];
    while (p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V->data), p_H->keyLen) == 0)
            break;
        p_V = p_V->next;
    }
    if(!p_V)
    {
        XX_Unlock(p_H->mutex);
        return E_FAILED;
    }

    if(value )
        PUT_VALUE(p_H, p_V, value); /* copy value of entry
                                       which is about to be
                                       deleted */

    /* Delete entry */
    XX_DelFromDLList(p_V, p_H->entry[index]);
    XX_Free(p_V);
    p_H->valueNmb--;

    XX_Unlock(p_H->mutex);
    return E_OK;
}


/*********************************************************************
 * @purpose       Delete data from match table which hits specified key
 *
 *
 * @param  objId   @b{(input)} MT object handle
 * @param  key     @b{(input)} pointer to key
 * @param  value   @b{(input)} pointer to value of entry to be deleted
 *
 * @returns        E_OK        success
 * @returns        E_FAILED    entry wasn't found
 * @returns        E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_Delete( t_Handle objId, byte *key, byte **value)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    XX_Lock(p_H->mutex);
    index = GET_INDEX(p_H, key);

    /* Find first entry with specified key */
    p_V = p_H->entry[index];
    while (p_V)
    {
        if(memcmp(key, GET_KEY(p_H, p_V->data), p_H->keyLen) == 0)
        {
           if(!value)
              break;         /* go to delete first found entry */
           if(memcmp(value, p_V->data, p_H->valueLen) == 0)
              break;         /* entry to be deleted is found */
        }
        p_V = p_V->next;
    }
    if(!p_V)
    {
        XX_Unlock(p_H->mutex);
        return E_FAILED;
    }

    /* Delete entry */
    XX_DelFromDLList(p_V, p_H->entry[index]);
    XX_Free(p_V);
    p_H->valueNmb--;

    XX_Unlock(p_H->mutex);
    return E_OK;
}
/*********************************************************************
 * @purpose          Browse entire MT, calling  browse funcion for every
 *                   entry.
 *
 *
 * @param  objId     @b{(input)}    MT object handle
 * @param  f_browse  @b{(input)}    user defined browse function
 * @param  parameter @b{(input)}    parameter to be passed to browse function
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM  wrong parameters
 *
 * @notes            If browse funcion for some entry returns FALSE,
 *                   delete the entry
 *
 * @end
 * ********************************************************************/
e_Err MT_Browse( t_Handle objId, BROWSEFUNC f_browse, t_HandleAsLong parameter)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V, *p_tmpV;
    word index;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);
    ASSERT(f_browse);

    XX_Lock(p_H->mutex);

    /* If MT is empty, just return */
    if(!p_H->valueNmb)
    {
        XX_Unlock(p_H->mutex);
        return E_OK;
    }

    for(index = 0; index < p_H->entryNmb &&
        p_H->valueNmb > 0; index++)
    {
        p_V = p_H->entry[index];
        while(p_V)
        {
            t_Value *tmp_prev = p_V->prev, *tmp_next = p_V->next;
            p_tmpV = p_V->next;
            if(f_browse(p_V->data, parameter) == FALSE)
            {
                /* MT was advised to delete entry */
               XX_Freeze();
               if(tmp_prev != NULLP)
                  tmp_prev->next = tmp_next;
               else
                  p_H->entry[index] = tmp_next;
               if(tmp_next != NULLP)
                  tmp_next->prev = tmp_prev;
               XX_Thaw();

               XX_Free(p_V);
               p_H->valueNmb--;
            }
            p_V = p_tmpV;
        }
    }
    XX_Unlock(p_H->mutex);
    return E_OK;
}


/*********************************************************************
 * @purpose     Browse entries of MT which hit specified key, calling
 *              browse funcion for every entry.
 *
 *
 * @param  objId     @b{(input)}    MT object handle
 * @param  key       @b{(input)}    pointer to key
 * @param  f_browse  @b{(input)}    user defined browse function
 * @param  parameter @b{(input)}    parameter to be passed to browse function
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM   wrong parameters
 *
 * @notes            If browse funcion for
 *                   some entry returns FALSE, delete the entry
 *
 * @end
 * ********************************************************************/
e_Err MT_FindBrowse( t_Handle objId, byte *key,
                    BROWSEFUNC f_browse, t_HandleAsLong parameter)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V, *p_tmpV;
    word index;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);
    ASSERT(f_browse);

    XX_Lock(p_H->mutex);

    /* If MT is empty, just return */
    if(!p_H->valueNmb)
    {
        XX_Unlock(p_H->mutex);
        return E_OK;
    }

    index = GET_INDEX(p_H, key);
    p_V = p_H->entry[index];
    while(p_V)
    {
        t_Value *tmp_prev = p_V->prev, *tmp_next = p_V->next;
        p_tmpV = p_V->next;

        if(memcmp(key, GET_KEY(p_H, p_V->data), p_H->keyLen) != 0)
        {
           p_V = p_tmpV;
           continue;
        }

        if(f_browse(p_V->data, parameter) == FALSE)
        {
           /* MT was advised to delete entry */
           XX_Freeze();
           if(tmp_prev != NULLP)
              tmp_prev->next = tmp_next;
           else
              p_H->entry[index] = tmp_next;
           if(tmp_next != NULLP)
              tmp_next->prev = tmp_prev;
           XX_Thaw();

           XX_Free(p_V);
           p_H->valueNmb--;
        }
        p_V = p_tmpV;
    }
    XX_Unlock(p_H->mutex);
    return E_OK;
}


/*********************************************************************
 * @purpose                Returns number of entries in  MT object
 *
 *
 * @param  objId           @b{(input)}  MT object handle
 * @param  p_entriesNmb    @b{(input)}  pointer to return MT entries number
 *
 * @returns                E_OK        success
 * @returns                E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_GetEntriesNmb(t_Handle objId, word *p_entriesNmb)
{
    t_MTH   *p_H = (t_MTH *)objId;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    *p_entriesNmb = p_H->valueNmb;
    return E_OK;
}


/*********************************************************************
 * @purpose          Returns first entry in MT object
 *
 *
 * @param objId      @b{(input)} MT object handle
 * @param p_value    @b{(output)} pointer to put value of first entry or
 *                               NULLP
 *
 * @returns          E_OK        success
 * @returns          E_FAILED    MT object is empty
 * @returns          E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_GetFirst(t_Handle objId, byte **p_value)
{
    t_MTH   *p_H = (t_MTH *)objId;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    XX_Lock(p_H->mutex);
    p_H->getIndex = 0;
    if(!p_H->valueNmb)
    {
       XX_Unlock(p_H->mutex);
        /* MT object is empty */
        p_H->getValue = NULLP;
        return E_FAILED;
    }
    for(p_H->getValue = p_H->entry[p_H->getIndex]; p_H->getIndex
        < p_H->entryNmb; p_H->getIndex++)
    {
        p_H->getValue = p_H->entry[p_H->getIndex];
        if(p_H->getValue)
        {
          if(p_value)
          {
            PUT_VALUE(p_H, p_H->getValue, p_value);
          }
          XX_Unlock(p_H->mutex);
          return E_OK;
        }
    }
    
    XX_Unlock(p_H->mutex);
    return E_FAILED;
}


/*********************************************************************
 * @purpose           Return entry next to previosly returned
 *                    in MT_GetFirst/ MT_GetNext loop.
 *
 *
 * @param  objId      @b{(input)} MT object handle
 * @param  p_value    @b{(output)} pointer to put value of first entry
 *                                or NULLP
 * @param  curValue   @b{(input)} start from entry with this value
 *
 * @returns           E_OK        success
 * @returns           E_FAILED    no entries any more
 * @returns           E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_GetNext(t_Handle objId, byte **p_value, byte **curValue)
{
    t_MTH   *p_H = (t_MTH *)objId;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);
    ASSERT(p_value != NULLP);

    XX_Lock(p_H->mutex);
    if(p_H->getValue == NULLP ||
        memcmp(p_H->getValue->data, curValue,
        p_H->valueLen) != 0)
    {
        /* Find entry with *curValue value */
        for(p_H->getIndex = 0; p_H->getIndex < p_H->entryNmb; p_H->getIndex++)
        {
            p_H->getValue = p_H->entry[p_H->getIndex];
            while (p_H->getValue)
            {
                if(memcmp(p_H->getValue->data, curValue,
                    p_H->valueLen) == 0)
                    break;
                p_H->getValue = p_H->getValue->next;
            }
            if(p_H->getValue != NULLP)
                break;
        }
        if(!p_H->getValue != NULLP)
        {
            XX_Unlock(p_H->mutex);
            return E_FAILED;
        }
    }

    /* Try to get next entry after found within the same linked list */
    if((p_H->getValue = p_H->getValue->next) != NULLP)
    {
        if(p_value)
        {
           PUT_VALUE(p_H, p_H->getValue, p_value);
        }
        XX_Unlock(p_H->mutex);
        return E_OK;
    }

    /* Move to next non-empty linked list */
    p_H->getIndex++;
    for( p_H->getValue = NULLP;
    p_H->getIndex < p_H->entryNmb; p_H->getIndex++)
    {
        if((p_H->getValue = p_H->entry[p_H->getIndex]) != NULLP)
           break;
    }

    if(p_H->getValue != NULLP)
    {
        if(p_value)
        {
           PUT_VALUE(p_H, p_H->getValue, p_value);
        }
        XX_Unlock(p_H->mutex);
        return E_OK;
    }

    /* Hit the end of MT object */
    p_H->getIndex = 0;
    XX_Unlock(p_H->mutex);
    return E_FAILED;
}
/*********************************************************************
 * @purpose          Move entry within MT in accordance with new key
 *
 *
 * @param  objId     @b{(input)}  MT object handle
 * @param  oldKey    @b{(input)}  pointer to existing key
 * @param  value     @b{(input)}  entry value
 * @param  newKey    @b{(input)}  pointer to new key
 *
 * @returns          E_OK        success
 * @returns          E_FAILED    entry not found
 * @returns          E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err MT_Move(t_Handle objId, byte *oldKey, byte **value, byte *newKey)
{
    t_MTH   *p_H = (t_MTH *)objId;
    t_Value *p_V;
    word index;
    ulng IndexForMacro;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);
    ASSERT(value);
    ASSERT(oldKey);
    ASSERT(newKey);

    XX_Lock(p_H->mutex);
    index = GET_INDEX(p_H, oldKey);
    p_V = p_H->entry[index];

    /* Try to find entry */
    while(p_V != NULLP)
    {
        if(memcmp(GET_KEY(p_H, p_V->data), oldKey, p_H->keyLen) == 0 &&
            memcmp(value, p_V->data, p_H->valueLen) == 0)
            break;
        p_V = p_V->next;
    }
    if(p_V == NULLP)
    {
        XX_Unlock(p_H->mutex);
        return E_FAILED; /* entry is not found */
    }

    /* Remove entry */
    XX_DelFromDLList(p_V, p_H->entry[index])

    /* Insert entry according to new key */
    index = GET_INDEX(p_H, newKey);
    XX_AddToDLList(p_V, p_H->entry[index]);
    XX_Unlock(p_H->mutex);
    return E_OK;

}


/*********************************************************************
 * @purpose        deallocate memory for all etries
 *
 *
 * @param  p_H     @b{(input)}  MT object pointer
 *
 * @returns        n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/

void MT_CleanupAllEntries(t_Handle hnd)
{
    t_MTH   *p_H = (t_MTH *)hnd;
    t_Value *value;
    word    index;

    for(index = 0; index < p_H->entryNmb &&
        p_H->valueNmb > 0; index ++)
    {
        for(value = p_H->entry[index]; value != NULLP;
        value = value->next)
        {
            XX_Free(value);
            p_H->valueNmb--;
        }
    }
}

#ifdef MATCHK
/* Display all match tables currently allocated and by what line of code */
void  MT_DisplayMTs( void )
{
   char    prntBuf[128];
   int     nMTs = 0;
   t_MTH   *p_H;

   XX_Freeze();

   for (p_H = MTObjList; p_H != NULL; p_H = p_H->next)
   {
      ++nMTs;
      sprintf(prntBuf, "     %p allocated by %s: %u", (void *)p_H, p_H->allocFile, p_H->allocLine);
      XX_Print(prntBuf);
   }

   sprintf(prntBuf, "Total of %u Match Tabls ", nMTs);
   XX_Print(prntBuf);

   XX_Thaw();
}
#endif /* MATCHK */



/****************end of file********************************/
