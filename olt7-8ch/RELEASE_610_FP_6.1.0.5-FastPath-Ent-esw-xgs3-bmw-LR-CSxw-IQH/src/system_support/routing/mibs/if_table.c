/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename              if_table.c
 *
 * @purpose               Interface table and interface stack table
 *                        routines for MIB-II support
 *
 * @component             Routing MIB Component
 *
 * @comments              ifTable_New
                          ifTable_Delete
                          ifTable_Lookup
                          ifTable_Active

                          ifStackTable_New
                          ifStackTable_Delete
                          ifStackTable_GetNextByUpper
 *
 * @create
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/mibs/if_table.c,v 1.1 2011/04/18 17:10:52 mruas Exp $";
#endif

/* --- standard include files --- */

#include <string.h>
#include <stddef.h>

#include "std.h"
#include "local.h"


#if L7MIB_MIB2


/* --- specific include files --- */

#include "rfc1213.h"


/* --- external object interfaces --- */

#include "if_table.ext"
#include "oll.ext"
#include "timer.ext"
#include "xx.ext"

/*#include "phy.ext"*/

/* --- internal defines/data --- */

typedef struct tag_ifT
{
   struct tag_ifT *next;
   struct tag_ifT *prev;
   ulng            ifIndex;     /* key for searches */
   t_Handle        Owner;

   t_ifTable       Entry;
} t_ifT;

static t_Handle _ifTable_;

/* interface stack table entry */

typedef struct tag_ifST
{
   struct tag_ifST *next;
   struct tag_ifST *prev;
   ulng             Upper;
   ulng             Lower;
   t_ifStackTable   Entry;
} t_ifST;

static t_Handle _ifStackTable_;



/*********************************************************************
 * @purpose        Indicates that a new interface table object has
 *                 been created.
 *
 *
 * @param Owner    @b{(input)} owner of interface
 * @param ifc      @b{(input)} interface table itself
 *
 * @returns        E_OK           success
 * @returns        E_IN_MATCH     entry already exists
 * @returns        E_NOMEMORY     can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ifTable_New( ulng ifIndex )
{
   t_ifT *p;
   e_Err  e;

   if( !ifIndex )
      return E_BADPARM;

   /* check if an existing interface table entry already exists */
   if(OLL_Find(_ifTable_, &ifIndex))
      return E_IN_MATCH;

   if(_ifTable_ == NULLP)
      _ifTable_ = OLL_New(0,                               /* do not use DA */
                          1,
                          (ulng)offsetof(t_ifT, ifIndex),
                          (ulng)sizeof(ulng)
                         );

   /* allocate new interface control table */
   p = XX_Malloc(sizeof(t_ifT));
   if( !p )
      return E_NOMEMORY;

   memset( p, 0, sizeof(t_ifT) );
   p->ifIndex = ifIndex;

   /* add to hash list and return */
   e = OLL_Insert( _ifTable_, (void *)p );
   if( e == E_OK )
   {
      /* set default values where possible */
      ifINDEX(&p->Entry)       = ifIndex;
      ifADMINSTATUS(&p->Entry) = ifStatus__down;
      ifOPERSTATUS(&p->Entry)  = ifStatus__down;
      ifLASTCHANGE(&p->Entry)  = SYSUPTIME;
   }

   return e;
}


/*********************************************************************
 * @purpose          Indicates that an interface table object has been
 *                   deleted.
 *
 *
 * @param Index      @b{(input)}  index of interface
 *
 * @returns          n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ifTable_Delete( ulng Index )
{
   void *p;

   p = OLL_Find( _ifTable_, &Index );
   if( p )
   {
      OLL_Extract( _ifTable_, p );
      XX_Free(p);
   }

   if(_ifTable_ && OLL_GetNext( _ifTable_, NULLP ) == NULLP)
   {
      OLL_Delete(_ifTable_);
      _ifTable_ = NULLP;
   }

   return E_OK;
}


/*********************************************************************
 * @purpose          Get interface table
 *
 *
 * @param Index      @b{(input)}  interface index
 *
 * @returns          pointer to ifTable on success
 * @returns          NULLP otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ifTable_Lookup( ulng Index )
{
   t_ifT *p;

   p = (t_ifT *)OLL_Find(_ifTable_, &Index);
   return p? (t_Handle)&p->Entry: NULLP;
}

/*********************************************************************
 * @purpose          Activates the interface
 *
 *
 * @param Index      @b{(input)}  interface index
 *
 * @returns          E_OK
 * @returns          error code if failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ifTable_Active( ulng Index, void *extra )
{
   t_ifT *p;

   p = (t_ifT *)OLL_Find(_ifTable_, &Index);
   if( !p )
      return E_BADPARM;

   switch(ifTYPE(&p->Entry))
   {
   default:
      return E_BADPARM;
   }
}


/*********************************************************************
 * @purpose          Get interface (port) handle from ifTable structure
 *
 *
 * @param Index      @b{(input)}  interface index
 *
 * @returns          interface (port) handle or NULL if value was not set
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ifTable_GetOwner( ulng Index )
{
   t_ifT *p;

   p = (t_ifT *)OLL_Find(_ifTable_, &Index);
   return p? p->Owner: NULLP;
}




/*********************************************************************
 * @purpose          Set interface (port) handle to ifTable structure
 *
 *
 * @param Index      @b{(input)}  interface index
 * @param Owner      @b{(input)}  port handle value
 *
 * @returns          E_OK           success
 * @returns          E_NOT_FOUND    table with such index does not exist
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ifTable_SetOwner( ulng Index, t_Handle Owner )
{
   t_ifT *p;

   p = (t_ifT *)OLL_Find(_ifTable_, &Index);
   if( p )
   {
      p->Owner = Owner;
      return E_OK;
   }
   return E_NOT_FOUND;
}




/*********************************************************************
 * @purpose          Get next entry in the interface table
 *
 *
 * @param Index      @b{(input)}  interface index
 *
 * @returns          pointer to next entry in the ifTable on success
 * @returns          NULLP otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ifTable_GetNext( ulng prevIndex )
{
   t_ifT *next;

   if(_ifTable_ == 0)
      return 0;

   next = (t_ifT *)OLL_FindNext(_ifTable_, &prevIndex);
   return next? (t_Handle)&next->Entry: 0;
}





/*********************************************************************
 * @purpose          Build a new entry in the Interface Stack table
 *
 *
 * @param Upper      @b{(input)}  upper interface index
 * @param Lower      @b{(input)}  lower interface index
 * @param Status     @b{(input)}  row status
 *
 * @returns          E_OK           success
 * @returns          E_IN_MATCH     entry already exists
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ifStackTable_New( ulng Upper, ulng Lower, e_RowStatus Status )
{
   t_ifST *p;

   /* check if an existing interface stack table entry already exists */
   if(OLL_Find(_ifStackTable_, &Upper, &Lower))
      return E_IN_MATCH;

   if(_ifStackTable_ == NULLP)
      _ifStackTable_ = OLL_New(0,                               /* do not use DA */
                               2,
                               (ulng)offsetof(t_ifST, Upper),
                               (ulng)sizeof(ulng),
                               (ulng)offsetof(t_ifST, Lower),
                               (ulng)sizeof(ulng)
                              );

   /* allocate new interface control table */
   p = XX_Malloc(sizeof(t_ifST));
   if( !p )
      return E_NOMEMORY;

   memset( p, 0, sizeof(t_ifST) );
   p->Upper = Upper;
   p->Lower = Lower;
   ifSTACK_HIGHERLAYER(&p->Entry) = Upper;
   ifSTACK_LOWERLAYER(&p->Entry)  = Lower;
   ifSTACK_STATUS(&p->Entry)   = Status;

   /* add to hash list and return */
   return OLL_Insert( _ifStackTable_, p );
}


/*********************************************************************
 * @purpose          Delete the Interface Stack table entry
 *
 *
 * @param Upper      @b{(input)}  upper interface index
 * @param Lower      @b{(input)}  lower interface index
 *
 * @returns          n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ifStackTable_Delete( ulng Upper, ulng Lower )
{
   void *p;

   p = OLL_Find( _ifStackTable_, &Upper, &Lower );
   if( p )
   {
      OLL_Extract( _ifStackTable_, p );
      XX_Free(p);
   }

   if(OLL_GetNext( _ifStackTable_, NULLP ) == NULLP)
   {
      OLL_Delete(_ifStackTable_);
      _ifStackTable_ = NULLP;
   }

   return E_OK;
}


/*********************************************************************
 * @purpose         Get next upper interface
 *
 *
 * @param Lower     @b{(input)} lower interface index
 * @param Prev      @b{(input)}  previous ifStackTable control structure
 *                              (0 = start)
 *
 * @returns         pointer to ifST on success
 * @returns         NULLP otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ifStackTable_GetNextUpper( ulng Lower, t_Handle Prev )
{
   t_ifST *p;

   if(_ifStackTable_ == 0)
      return 0;

   /* Prev points to Entry field.  OLL expects for pointer to */
   /* t_ifST. Correct Prev */
   if (Prev)
      Prev = (t_Handle)((char *)Prev - offsetof(t_ifT, Entry));

   p = (t_ifST *)OLL_GetNext( _ifStackTable_, Prev );
   return p? &p->Entry: 0;
}

t_Handle ifStackTable_GetNext( ulng prev )
{
   t_ifST *next;

   if(_ifStackTable_ == 0)
      return 0;

   next = (t_ifST *)OLL_FindNext( _ifStackTable_, &prev );
   return next? (t_Handle)&next->Entry: 0;
}

#endif

/* --- end of file if_table.c --- */
