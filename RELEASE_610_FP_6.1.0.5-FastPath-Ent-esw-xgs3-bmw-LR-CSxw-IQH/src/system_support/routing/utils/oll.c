/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename            oll.c
 *
 * @purpose             Ordered Linked List implementation
 *                      Ordered Linked List implementation
 *                      An ordered linked list is an object that stores
 *                      elements and imposes an order on them.
 *                      An arbtitrary number of keys may be used for
 *                      ordering.
 *
 * @component           Routing Utils Component
 *
 * @comments
 *                      Restrictions:
 *                        1. The elements in the OLL must be suitable for
 *                           a double linked list: the first two fields MUST
 *                           be a next and prev void pointer.
 *
 * @create              01/01/1999
 *
 * @author              Jonathan Masel
 *                      Igor Achkinazi
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/oll.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif


/* --- standard include files --- */

#include "std.h"
#include "local.h"

#include <string.h>
#include <stdarg.h>


/* --- specific include files --- */

#include "oll.ext"


/* --- external object interfaces --- */

#include "xx.ext"
#include "da.ext"


/* --- internal defines/data --- */

typedef struct t_OLL_Key
{
   word off;              /* offset of key in entry */
   word len;              /* length of key */
} t_OLL_Key;

typedef struct t_SKey
{
   Bool     Locked;        /* Search keys are locked - another process uses them */
   byte     *searchKey;    /* Array with key to search, 
                              used for search element in list    */
   word     nKeysToSearch; /* Number of keys in this array       */
} t_SKey;

typedef struct t_OLL
{
   void     *p_First;      /* first entry in linked list         */
   word     nKeys;         /* number of keys                     */
   word     Num;           /* number of entries in linked list   */
   Bool     Locked;        /* linked list is locked (busy)       */
   t_SKey   search[OLL_MAX_SEARCH];
   t_Handle indxArray;     /* Indexing array for fast search     */

   t_OLL_Key Key[1];       /* fake size - dynamically calculated */
} t_OLL;

#define LOWEST_EXP       8  /* do linear searches below this range */

#define NEXT(p)          (*(void **)(p))
#define PREV(p)          (*((void **)(p)+1))


int          compareNKeys( t_OLL *p, void *a, void *b, word n );
static void *findNearestElm( t_OLL *p, word nSearch, Bool *p_Match );
static void *findInIndexArray( t_OLL *p, word nSearch, Bool *p_Match, word *p_Index );
static int   compareElms( t_OLL *p, byte *a, byte *b );
static       t_OLL *getNElement( t_OLL *l, word n, void *p );



 /*********************************************************************
 * @purpose     Creates a new ordered linked list
 *
 *
 * @param daClustSize      @b{(input)}  dynamic array cluster size, if
 *                                      DA object is
 *                                      used zero, if DA object is not used
 * @param nKeys            @b{(input)}  number of keys
 * @param ...              @b{(input)}  offset of key 1
 *                                      length of key 1
 *                                      and so on
 *
 *
 * @returns        Pointer to handle of created object on success
 * @returns        0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle OLL_New(ulng daClustSize, ulng nKeys, ... )
{
   word       i;
   word       off, len;
   word       totalKeyLen = 0;
   va_list    parms;
   t_OLL     *p;
   t_OLL_Key *pk;

   if( !nKeys )
   {
      ASSERT(FALSE);
      return NULLP;
   }

   /* allocate and initialize memory */
   p = (t_OLL *)XX_MallocChunk(sizeof(t_OLL) + (nKeys-1)*(sizeof(t_OLL_Key)));
   if( !p )
      return 0;

   memset(p, 0, sizeof(t_OLL));
   p->Locked = FALSE;
   p->nKeys = (word)nKeys;
   pk = &p->Key[0];
   memset(&parms, 0, sizeof(parms));
   va_start(parms, nKeys);
   for(i = 0; i < nKeys; i++)
   {
      /* get next parameters */
      off = (word)va_arg(parms, ulng);
      len = (word)va_arg(parms, ulng);
      if( (off == (word)-1) || (len == (word)-1) )
      {
         XX_Free(p);
         return 0;
      }
      pk->off = off;
      pk->len = len;
      totalKeyLen += len;
      pk++;
   }

   if(daClustSize)
   {
      if(DA_Construct((word)daClustSize, &p->indxArray) != E_OK)
      {
         XX_Free(p);
         return 0;
      }
   }

   va_end(parms);

   for(i = 0; i < OLL_MAX_SEARCH; i++)
   {
      p->search[i].searchKey = XX_MallocChunk(totalKeyLen);
      if(!p->search[i].searchKey)
      {
         OLL_Delete(p);
         p = 0;
         break;
      }
   }
   return (t_Handle )p;
}


/* delete an OLL */


 /*********************************************************************
 * @purpose    Deletes existing ordered linked list
 *
 *
 * @param  h      @b{(input)}  handle of linked list to delete
 *
 * @returns       cannot fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void OLL_Delete( t_Handle h )
{
   t_OLL *p_OLL = (t_OLL *)h;
   word i;

   ASSERT(h);
   if( h )
   {
      if(p_OLL->indxArray)
         DA_Destruct(&p_OLL->indxArray);
      for(i = 0; i < OLL_MAX_SEARCH; i++)
         if(p_OLL->search[i].searchKey)
            XX_Free(p_OLL->search[i].searchKey);
      XX_Free( h );
   }
}


 /*********************************************************************
 * @purpose    Inserts a new element to an ordered linked list
 *
 *
 * @param h      @b{(input)}  handle of linked list
 * @param Elm    @b{(input)}  element to insert
 *
 * @returns      E_OK     success
 * @returns      otherwise failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err OLL_Insert( t_Handle h, void *Elm )
{
   register t_OLL *p_OLL = (t_OLL *)h;
   register void  *p;
   Bool            Match;
   word            searchKeyOff = 0;
   word            index = 0;
   word            i, n;

   ASSERT(h);
   ASSERT(Elm);
   
   XX_Freeze();
   if( p_OLL->Locked )
   {
      XX_Thaw();
      return E_BUSY;
   }
   p_OLL->Locked = TRUE;

   for(n = 0; n < OLL_MAX_SEARCH; n++)
      if(!p_OLL->search[n].Locked)
         break;
   if(n == OLL_MAX_SEARCH)
   {
      XX_Thaw();
      return E_BUSY;
   }
   p_OLL->search[n].Locked = TRUE;
   XX_Thaw();

   for(i = 0; i < p_OLL->nKeys; i++)
   {
      memcpy(p_OLL->search[n].searchKey + searchKeyOff, 
             (byte *)Elm+p_OLL->Key[i].off, 
             p_OLL->Key[i].len );
      searchKeyOff += p_OLL->Key[i].len;
   }
   p_OLL->search[n].nKeysToSearch = p_OLL->nKeys;

   if(p_OLL->indxArray)
      p = findInIndexArray( p_OLL, n, &Match, &index );
   else
      p = findNearestElm( p_OLL, n, &Match );

   if( Match )
   {
      XX_Freeze();
      p_OLL->Locked = FALSE;
      p_OLL->search[n].Locked = FALSE;
      XX_Thaw();
      return E_EXISTS;
   }

   if(p && compareElms(p_OLL, p, Elm) >= 0)
   {
      ASSERT(FALSE);
      return E_FAILED;
   }

   /* insert in linked list */
   if( !p )
   {
      PREV(Elm) = 0;
      NEXT(Elm) = p_OLL->p_First;
      p_OLL->p_First = Elm;
   } 
   else
   {
      NEXT(Elm) = NEXT(p);
      PREV(Elm) = p;
      NEXT(p)   = Elm;
   }

   if( NEXT(Elm) )
      PREV(NEXT(Elm)) = Elm;

   /* Insert into indexed address */
   if(p_OLL->indxArray)
      DA_Insert(p_OLL->indxArray, Elm, index);

   p_OLL->Num++;

   XX_Freeze();
   p_OLL->Locked = FALSE;
   p_OLL->search[n].Locked = FALSE;
   XX_Thaw();

   return E_OK;
}

 /*********************************************************************
 * @purpose    Remove an element from an ordered linked list
 *
 * @param h      @b{(input)}  handle of linked list
 * @param Elm    @b{(input)}  element to remove from the list
 *
 * @returns     E_OK     success
 * @returns     E_FAILED failed
 *
 * @notes       Do not use if the OLL uses dynamic array (DA).
 *
 * @end
 * ********************************************************************/
e_Err OLL_Remove( t_Handle h, void *Elm )
{
   register t_OLL *p_OLL = (t_OLL *)h;
   register void  *p = Elm;

   if( !h || !Elm )
      return E_BADPARM;

   XX_Freeze();
   if( p_OLL->Locked )
   {
      XX_Thaw();
      return E_BUSY;
   }
   p_OLL->Locked = TRUE;
   XX_Thaw();

   if( PREV(p) )
      NEXT(PREV(p)) = NEXT(p);
   else
      p_OLL->p_First = NEXT(p);

   if( NEXT(p) )
      PREV(NEXT(p)) = PREV(p);

   if( p_OLL->Num )
      p_OLL->Num--;

   XX_Freeze();
   p_OLL->Locked = FALSE;
   XX_Thaw();

   return E_OK;
}

 /*********************************************************************
 * @purpose    xtracts an element from an ordered linked list
 *
 *
 * @param h      @b{(input)}  handle of linked list
 * @param Elm    @b{(input)}  object containing the key for the element
 *                            to be removed
 *
 * @returns     E_OK     success
 * @returns     E_FAILED failed
 *
 * @notes       Elm is treated as an object that holds the values that
 *              make up the key for the element to be removed from the
 *              list. A search is done for the element that matches 
 *              this key. To delete a specific element from a list, use
 *              OLL_Remove().
 *
 * @end
 * ********************************************************************/
e_Err OLL_Extract( t_Handle h, void *Elm )
{
   register t_OLL *p_OLL = (t_OLL *)h;
   register void  *p;
   Bool            Match;
   word            searchKeyOff = 0;
   word            index = 0;
   word            i, n;

   if( !h || !Elm )
      return E_BADPARM;

   XX_Freeze();
   if( p_OLL->Locked )
   {
      XX_Thaw();
      return E_BUSY;
   }
   p_OLL->Locked = TRUE;

   for(n = 0; n < OLL_MAX_SEARCH; n++)
      if(!p_OLL->search[n].Locked)
         break;
   if(n == OLL_MAX_SEARCH)
   {
      XX_Thaw();
      return E_BUSY;
   }
   p_OLL->search[n].Locked = TRUE;
   XX_Thaw();

   for(i = 0; i < p_OLL->nKeys; i++)
   {
      memcpy(p_OLL->search[n].searchKey + searchKeyOff, 
             (byte *)Elm+p_OLL->Key[i].off, 
             p_OLL->Key[i].len );
      searchKeyOff += p_OLL->Key[i].len;
   }
   p_OLL->search[n].nKeysToSearch = p_OLL->nKeys;

   if(p_OLL->indxArray)
      p = findInIndexArray( p_OLL, n, &Match, &index );
   else
      p = findNearestElm( p_OLL, n, &Match );

   if( !Match )
   {
      XX_Freeze();
      p_OLL->Locked = FALSE;
      p_OLL->search[n].Locked = FALSE;
      XX_Thaw();
      return E_BADPARM;
   }

   /* extract from linked list */
   if( !p )
      p_OLL->p_First = 0;
   else
   {
      if( PREV(p) )
         NEXT(PREV(p)) = NEXT(p);
      else
         p_OLL->p_First = NEXT(p);

      if( NEXT(p) )
         PREV(NEXT(p)) = PREV(p);
   }

   /* Insert into indexed address */
   if(p_OLL->indxArray)
      DA_Delete(p_OLL->indxArray, index);

   if( p_OLL->Num )
      p_OLL->Num--;

   XX_Freeze();
   p_OLL->search[n].Locked = FALSE;
   p_OLL->Locked = FALSE;
   XX_Thaw();

   return E_OK;
}



 /*********************************************************************
 * @purpose    Finds an element in ordered linked list
 *
 *
 * @param  h     @b{(input)}  handle of linked list
 * @param ...    @b{(input)}  key 1
 *                            and so on
 *
 * @returns      Pointer to found element on success
 * @returns      0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle OLL_Find( t_Handle h, ... )
{
   register t_OLL *p_OLL = (t_OLL *)h;
   register void  *p;
   va_list         parms;
   word            i, n;
   Bool            Match;
   word            searchKeyOff = 0;

   if( !h )
      return 0;

   XX_Freeze();
   for(n = 0; n < OLL_MAX_SEARCH; n++)
      if(!p_OLL->search[n].Locked)
         break;
   if(n == OLL_MAX_SEARCH)
   {
      XX_Thaw();
      return 0;
   }
   p_OLL->search[n].Locked = TRUE;
   XX_Thaw();
   memset (&parms,0,sizeof(parms));
   /* start scanning list for key */
   va_start(parms, h);

   for(i = 0; i < p_OLL->nKeys; i++)
   {
      p = va_arg(parms, void *);
      if( p == (void *)-1 )
      {
         va_end(parms);
         XX_Freeze();
         p_OLL->search[n].Locked = FALSE;
         XX_Thaw();
         return 0;
      }
      memcpy(p_OLL->search[n].searchKey + searchKeyOff, p, p_OLL->Key[i].len );
      searchKeyOff += p_OLL->Key[i].len;
   }
   va_end(parms);

   p_OLL->search[n].nKeysToSearch = p_OLL->nKeys;

   if(p_OLL->indxArray)
      p = findInIndexArray( p_OLL, n, &Match, NULLP);
   else
      p = findNearestElm( p_OLL, n, &Match );

   XX_Freeze();
   p_OLL->search[n].Locked = FALSE;
   XX_Thaw();

   if( !Match )
      return 0;
   return p;
}


 /*********************************************************************
 * @purpose     Finds next element in ordered linked list
 *
 *
 * @param h      @b{(input)}  handle of linked list
 * @param ...    @b{(input)}  key 1
 *                            and so on
 *
 * @returns      Pointer to found element on success
 * @returns      0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle OLL_FindNext( t_Handle h, ... )
{
   register t_OLL *p_OLL = (t_OLL *)h;
   register void  *p;
   va_list         parms;
   word            i, n;
   Bool            Match;
   word            searchKeyOff = 0;

   if( !h )
      return 0;

   XX_Freeze();
   for(n = 0; n < OLL_MAX_SEARCH; n++)
      if(!p_OLL->search[n].Locked)
         break;
   if(n == OLL_MAX_SEARCH)
   {
      XX_Thaw();
      return 0;
   }
   p_OLL->search[n].Locked = TRUE;
   XX_Thaw();
   memset(&parms,0,sizeof(parms));
   va_start(parms, h);

   /* start scanning list for key */
   for(i = 0; i < p_OLL->nKeys; i++)
   {
      p = va_arg(parms, void *);
      if( p == (void *)-1 )
         break;
      memcpy(p_OLL->search[n].searchKey + searchKeyOff, p, p_OLL->Key[i].len );
      searchKeyOff += p_OLL->Key[i].len;
   }
   va_end(parms);

   p_OLL->search[n].nKeysToSearch = i;
   if(p_OLL->search[n].nKeysToSearch == 0)
   {
      XX_Freeze();
      p_OLL->search[n].Locked = FALSE;
      XX_Thaw();
      return p_OLL->p_First;
   }

   if(p_OLL->indxArray)
      p = findInIndexArray( p_OLL, n, &Match, NULLP);
   else
      p = findNearestElm( p_OLL, n, &Match );

   XX_Freeze();
   p_OLL->search[n].Locked = FALSE;
   XX_Thaw();

   if(p)
   {
      if(Match && p_OLL->search[n].nKeysToSearch < p_OLL->nKeys)
         return p;
      return NEXT(p);
   }
   return p_OLL->p_First;
}



 /*********************************************************************
 * @purpose     Gets the next element in an oll
 *
 *
 * @param h      @b{(input)}  handle of linked list
 * @param Elm    @b{(input)}  get element following this one (if null,
 *                              then first)
 *
 * @returns       Pointer to found element on success
 * @returns       0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle OLL_GetNext( t_Handle h, void *Elm )
{
   if(!h)
      return 0;

   return Elm? NEXT(Elm): ((t_OLL *)h)->p_First;
}


 /*********************************************************************
 * @purpose      Gets the total number of elements in an oll
 *
 *
 * @param  h     @b{(input)}  handle of linked list
 *
 * @returns      the total number of elements on success
 * @returns      0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
word OLL_HowMany( t_Handle h )
{
   if(!h)
      return 0;

   return ((t_OLL *)h)->Num;
}


 /*********************************************************************
 * @purpose    Gets the last element in an oll
 *
 *
 * @param  h     @b{(input)}  handle of linked list
 *
 * @returns      Pointer to the last element on success
 * @returns      0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle OLL_GetLast( t_Handle h )
{
   void *Elm, *Last;

   if(!h)
      return 0;

   Last = NULLP;
   for(Elm = ((t_OLL *)h)->p_First; Elm; Elm = NEXT(Elm))
      Last = Elm;

   return Last;
}




/*******************************************************************/
/*                                                                 */
/*                     internal routines                           */
/*                                                                 */
/*******************************************************************/

/*
 * Get the n-th element in OLL "l" after element "p".
 * If start is NULL, start at beginning of OLL.
 *
 * The last element in the list is returned if the end of the list is passed
 */
static t_OLL *getNElement( t_OLL *l, word n, void *p )
{
   register int i;

   ASSERT(l);
   if( !p )
      p = l->p_First;

   for(i = 0; (i < n) && NEXT(p); i++)
      p = NEXT(p);

   return p;
}


/*
 * compare two elements of OLL p: a, b
 * Returns: -1 if a < b
 *           0 if a = b
 *           1 if a > b
 */
static int compareElms( t_OLL *p, byte *a, byte *b )
{
   int res;
   t_OLL_Key *pk;

   if( a == b )
      return 0;

   for(pk = &p->Key[0]; pk != &p->Key[p->nKeys]; pk++)
   {
      res = memcmp(a+pk->off, b+pk->off, pk->len);
      if( res != 0 )
         return res;
   }

   return 0;
}

/*
 * find the previous or match element to the specified one
 */
static void *findNearestElm( t_OLL *p, word nSearch, Bool *p_Match )
{
   register word  base, range;
   register int   res;
   register int   i, n;
   register void *nth = 0;
   void          *prev = 0;
   word          searchKeyOff;

   /* initial search range is entire OLL */
   range = p->Num;
   base = range/2;
   if( p_Match )
      *p_Match = FALSE;

   for(;;)
   {
      if( range == 0 )
         return 0;

      /* range is less than LOW_EXP, do a linear search */
      if( range < LOWEST_EXP )
      {

         if( !nth )
            nth = p->p_First;

         /* elemenst MUST be in this range */
         for(n = 0; (n < range) && nth; n++)
         {
            /* compare results */
            for(i = 0, res = 0, searchKeyOff = 0; 
                (i < p->search[nSearch].nKeysToSearch) && (res == 0);
                i++)
            {
               res = memcmp((byte*)nth + p->Key[i].off,
                            p->search[nSearch].searchKey + searchKeyOff, 
                            p->Key[i].len);
               searchKeyOff += p->Key[i].len;
            }

            if(res == 0) /* if equal - we've found it */
            {
               if(p_Match)
                  *p_Match = TRUE;
               return nth;
            }

            /* if we passed it, return current */
            if( res > 0 )
               return prev;

            /* else go to next and try again */
            prev = nth;
            nth = NEXT(nth);
         }
         return prev;
      }

      prev = nth;

      /* do binary search for right element */
      nth = getNElement(p, base, nth);

      /* compare results */
      for(i = 0, res = 0, searchKeyOff = 0; 
          (i < p->search[nSearch].nKeysToSearch) && (res == 0); 
          i++)
      {
         res = memcmp((byte*)nth + p->Key[i].off,
                      p->search[nSearch].searchKey + searchKeyOff, 
                      p->Key[i].len);
         searchKeyOff += p->Key[i].len;
      }

      if(res == 0) /* if equal - return */
      {
         if(p_Match)
            *p_Match = TRUE;
         return nth;
      }

      /* if nth is greater than target - step down */
      if( res > 0 )
         nth = prev;

      /* move range down by half */
      range = range&1 ? range/2 + 1 : range/2;
      base = range/2;
   }
}

static void *findInIndexArray( t_OLL *p, word nSearch, Bool *p_Match, word *p_Index )
{
   register word  base, range;
   register int   res;
   register int   i;
   register void *nth = 0;
   void          *Elm;
   word          searchKeyOff;

   /* initial search range is entire OLL */
   range = p->Num;
   base = range/2;
   if( p_Match )
      *p_Match = FALSE;
   if(p_Index)
      *p_Index = 0;

   for(;;)
   {
      if( range == 0 )
         return 0;

      DA_Get(p->indxArray, &Elm, base);

      /* compare results */
      for(i = 0, res = 0, searchKeyOff = 0; 
          (i < p->search[nSearch].nKeysToSearch) && (res == 0); 
          i++)
      {
         res = memcmp((byte*)Elm + p->Key[i].off,
                      p->search[nSearch].searchKey + searchKeyOff, 
                      p->Key[i].len);
         searchKeyOff += p->Key[i].len;
      }

      if(res == 0)  /* if equal */
      {
         if(p_Match)
            *p_Match = TRUE;
         if(p_Index)
            *p_Index = base;
         return nth;
      }

      /* move range down by half */
      range = range&1 ? range/2 + 1 : range/2;
      base = res > 0 ? base + range/2 : base - range/2;
   }
}

/*
 * compare the first n keys in elements a and b
 * Returns: -1 if a < b
 *           0 if a = b
 *           1 if a > b
 */
int compareNKeys( t_OLL *p, void *a, void *b, word n )
{
   int res;
   t_OLL_Key *pk;

   if( a == b )
      return 0;

   for(pk = &p->Key[0]; pk != &p->Key[n]; pk++)
   {
      res = memcmp((byte *)a+pk->off, (byte *)b+pk->off, pk->len);
      if( res != 0 )
         return res;
   }

   return 0;
}


/* --- end of file oll.c --- */
