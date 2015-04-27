/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename           ll.c
 *
 * @purpose            Linked list management implementation
 *
 * @component          Routing Utils Component
 *
 * @comments
 *                      Linked list object.
 *        Implementation of a linked list.
 *        Elements are put in and taken out of the list
 *        in FIFO order. In addition, the list has a
 *        current position that may be read (without
 *        affecting the contents of the list).
 *
 * External Routines:
 * LL_New create a new linked list
 * LL_Delete delete a linked list
 * LL_Put add an element to the list
 * LL_Get get an element from the list
 * LL_Cut remove item from linked list
 * LL_Insert insert item into linked list
 * LL_ReadLast  read the last element
 * LL_SetOffset set offset of linked list
 * LL_Readget the current read pointer
 * LL_Inc advance the current read pointer
 * LL_IncIf  advance the current read pointer
 * only if it was not touched
 * LL_HowManyhow many elements are in list
 * LL_Empty  is the list empty?
 *
 *
 * @create       02/01/1996
 *
 * @author       Jonathan Masel
 *
 * @end
 *
 *********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\ll.c 1.1.3.1 2002/02/13 23:06:21EST ssaraswatula Development  $";
#endif




/* --- standard include files --- */

#include "std.h"
#include "local.h"

#include <string.h>
#include <stdarg.h>


/* --- external object interfaces --- */

#include "xx.ext"
#include "ll.h"
#include "ll.ext"


/* --- internal (static) data and prototypes --- */

/* preallocated data structures */
#if MAX_PREALLOC_LL > 0
static t_LL _LL[MAX_PREALLOC_LL];
#endif

static t_Mutex LL_Mutex;
static void _inc_FC( t_LL_FlowControl *fc, word n );
static void _dec_FC( t_LL_FlowControl *fc, word n );
static void _delete_FC( t_LL *l );
static word _dfltResources_FC(void *p);

/*********************************************************************
 * @purpose     Global initialization for the linked-list object.
 *              Initializes the preallocated data structures.
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LL_InitAll( void )
{
#if MAX_PREALLOC_LL > 0
   int   i;

   /* clear global array */
   memset( _LL, 0, sizeof(_LL) );

   /* make sure that the Prealloc flag is set for each one here */
   for(i = 0; i < MAX_PREALLOC_LL; i++)
   {
      _LL[i].Taken    = FALSE;    /* redundant - just for emphasis */
      _LL[i].Prealloc = TRUE;
   }
#endif

   LL_Mutex = 0;
}

/*********************************************************************
 * @purpose                 Allocate and initialize a new linked list.
 *
 *
 * @param Size              @b{(input)}  max number of elements in list
 * @param NextOffset        @b{(input)}  offset of "next" field in
 *                                       application object
 *
 * @returns                 Handle to the new object on success
 * @returns                 NULL on failure (out of memory)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle LL_NewExt( word Size, word NextOffset )
{
   t_LL *p_LL = 0;

   /* first scan the preallocated list (if any) */
#if MAX_PREALLOC_LL > 0
   XX_Lock( &LL_Mutex );
   {
      int i;

      for(i = 0; i < MAX_PREALLOC_LL; i++)
      {
         if( _LL[i].Taken == FALSE )
         {
            memset( &_LL[i], 0, sizeof(t_LL) );
            _LL[i].Taken = TRUE;
            _LL[i].Prealloc = TRUE;
            p_LL = &_LL[i];
            break;
         }
      }
   }
   XX_Unlock( &LL_Mutex );
#endif


   /* we may need to allocate memory from the heap */
   if( !p_LL )
   {
      p_LL = XX_Malloc( sizeof(t_LL) );
      if( p_LL )
         memset( p_LL, 0, sizeof(t_LL) );
   }

   if( p_LL )
   {
      p_LL->Max = Size;
      p_LL->NextOffset = NextOffset;
   }

   return p_LL;
}


/*********************************************************************
 * @purpose            Adds flow control to a linked list.
 *
 *
 * @param LL           @b{(input)}  handle of the linked list
 * @param Owner        @b{(input)}  owner for congestion notifications
 * @param Max          @b{(input)}  maximum number of elements in linked
 *                                  list (0xffff means don't change)
 * @param NumLevels    @b{(input)}  number of congestion levels
 * @param f_Resources  @b{(input)}  calculates resources taken by each
 *                                  member in linked list
 * @param f_CongInd    @b{(input)}  call-back routine for indicating
 *                                  changes in congestion state
 *
 * @param ...          @b{(input)}  congestion level thresholds in pairs:
 *                                  clear1, set1,clear2,set2...
 *
 * @returns            E_OK          success
 * @returns            otherwise     error code
 *
 * @notes
 *                    If NumLevels == 0, the flow control object is deleted.
 *
 * @end
 * ********************************************************************/
e_Err LL_FlowControl( t_Handle LL, t_Handle Owner,
                      word Max, word NumLevels,
                      word (*f_Resources)(void *), void (*f_CongInd)(t_Handle, word), ... )
{
   t_LL             *l = (t_LL *)LL;
   t_LL_FlowControl *p = l->p_FC;
   va_list           ap;
   int               i;
   ulng              previous_set_level = 0;
   ulng              previous_clear_level = 0;


   if( LL == NULLP )
      return E_BADPARM;

   /* NumLevels zero means delete the flow control object only */
   if( NumLevels == 0 )
   {
      _delete_FC(l);
      return E_OK;
   }

   /* Check more parameters */
   if( f_CongInd == NULLP )
      return E_BADPARM;

   /* Allocate memory for flow control object */
   p = XX_Malloc( sizeof(t_LL_FlowControl) + (sizeof(ulng) * (2*NumLevels-1)) );
   if( p == NULLP )
      return E_NOMEMORY;


   memset( p, 0, sizeof(t_LL_FlowControl) + (sizeof(ulng) * (2*NumLevels-1)) );
   p->Owner       = Owner;
   p->MaxLevel    = NumLevels;
   p->f_Resources = f_Resources ? f_Resources : _dfltResources_FC;
   p->f_CongInd   = f_CongInd;

   /* Get congestion levels thresholds */
   va_start(ap, f_CongInd);
   for(i = 0; i < (NumLevels*2); i++)
   {
      int current_arg = va_arg(ap, int);

      if( current_arg == -1 )
      {
         /* Unexpected end of the level's list */
            XX_Free(p);
            va_end(ap);
            return E_BADPARM;
      }

      /* Even index - clear level, odd index - set_level */
      if(i & 1)
      {
         /* Check set_level */
         if( ((ulng)current_arg > previous_set_level) &&
             ((ulng)current_arg >= previous_clear_level))
            previous_set_level = (ulng)current_arg;
         else
         {
            XX_Free(p);
            va_end(ap);
            return E_BADPARM;
         }
      }
      else
      {
         /* Check clear_level */
         if( (ulng)current_arg >= previous_set_level)
            previous_clear_level = (ulng)current_arg;
         else
         {
            XX_Free(p);
            va_end(ap);
            return E_BADPARM;
         }
      }

      p->CongLevels[i] = current_arg;
   }
   va_end(ap);

   if( Max != 0xffff )
      l->Max = Max;

   /* Release previous flow control object it it exists */
   _delete_FC(l);

   /* Connect newly allocated flow control object to LL object */
   l->p_FC = p;

   return E_OK;
}


/*********************************************************************
 * @purpose       Deletes a linked list. All elements are assumed to have
 *                been discarded prior to this.
 *
 *
 * @param LL      @b{(input)}  handle to the linked list
 *
 * @returns       n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LL_Delete( t_Handle LL )
{
   t_LL *l = (t_LL *)LL;

   if( l )
   {
      if( l->p_FC )
         _delete_FC(l);

      if( l->Prealloc )
         l->Taken = FALSE;
      else
         XX_Free( LL );
   }
}



/*********************************************************************
 * @purpose       Puts a new element in the linked list.
 *
 *
 * @param  LL     @b{(input)}  handle to the linked list
 * @param  p      @b{(input)}  element to be added
 *
 * @returns       -1        on failure (list is full)
 * @returns       otherwise the number of elements PREVIOUSLY in the queue
 *                (i.e. before the put operation)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int LL_Put( t_Handle LL, void *p )
{
   t_LL *p_LL = (t_LL *)LL;
   register int k;
   t_LL_FlowControl *fc = p_LL->p_FC;

   /* see if another element will fit into the linked list */
   XX_Freeze();
   k = p_LL->Count;
   if( (p_LL->Max > 0) && (k >= (int)p_LL->Max) )
   {
      p_LL->PutFailures++;
      XX_Thaw();
      return -1;
   }

   /* clear the next pointer */
   LL_NEXT(p_LL, p) = 0;

   /* check read field  (AO)*/
   if(!p_LL->Read)
      p_LL->Read = p;

   /* add element to linked list */
   if( p_LL->Last )
      LL_NEXT(p_LL, p_LL->Last) = p;
   else
   {
      p_LL->First = p_LL->Read = p;
   }
   p_LL->Last = p;
   p_LL->Count++;
   if( p_LL->Count > p_LL->MaxIn )
      p_LL->MaxIn = p_LL->Count;

   /* Do flow control if necessary */
   if( (fc = p_LL->p_FC) )
      _inc_FC( fc, fc->f_Resources(p) );
    else
        XX_Thaw();

   return k;
}


/*********************************************************************
 * @purpose     Gets (and removes) an element from the linked list.
 *
 *
 * @param       @b{(input)}  handle to the linked list
 *
 * @returns     Returns the element (void pointer) on success
 * @returns     NULL on failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void *LL_Get( t_Handle LL )
{
   t_LL *p_LL = (t_LL *)LL;
   void *p;
   t_LL_FlowControl *fc;

   XX_Freeze();
   p = (void *)p_LL->First;
   /* see if the list is not empty */
   if( !p )
   {
      XX_Thaw();
      return 0;
   }

   /* advance the head of list pointer */
   p_LL->First = LL_NEXT(p_LL, p_LL->First);
   if( p_LL->Read == p )
   {
      p_LL->Read = p_LL->First;
      ++p_LL->ReadShiftCount;
   }
   if( p_LL->Last == p )
      p_LL->Last = p_LL->First;
   p_LL->Count--;

   /* Do flow control if necessary */
   if( (fc = p_LL->p_FC) )
      _dec_FC( fc, fc->f_Resources(p) );
    else
        XX_Thaw();

   LL_NEXT(p_LL, p) = 0;

   return p;
}


/*********************************************************************
 * @purpose         Removes an item from a linked list
 *
 *
 * @param  LL       @b{(input)}    handle to the linked list
 * @param  pPrev    @b{(input)}    item preceding item to be removed
 *                                 (NULL on first item)
 * @param  pCur     @b{(input)}    item to be removed from linked list
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LL_Cut( t_Handle LL, void *pPrev, void *pCur)
{
   t_LL             *p_LL = (t_LL *)LL;
   t_LL_FlowControl *fc;

   XX_Freeze();
   if(pPrev)
      LL_NEXT(p_LL, pPrev) = LL_NEXT(p_LL, pCur);
   else
      p_LL->First = LL_NEXT(p_LL, pCur);
   if( p_LL->Read == pCur )
      p_LL->Read = LL_NEXT(p_LL, pCur);
   if( p_LL->Last == pCur )
      p_LL->Last = pPrev;
   if( p_LL->Count )
      p_LL->Count--;

   /* Do flow control if necessary */
   if( (fc = p_LL->p_FC) )
      _dec_FC( fc, fc->f_Resources(pCur) );
    else
        XX_Thaw();
}

/*********************************************************************
 * @purpose       Insert an item into a linked list
 *
 *
 * @param  LL     @b{(input)}  handle to the linked list
 * @param  pCur   @b{(input)}  current item after that insertion will
 *                             perfromed(NULL if insert to start)
 * @param  p      @b{(input)}  item to be inserted to linked list
 *
 * @returns       -1        on failure (list is full)
 * @returns       otherwise the number of elements PREVIOUSLY in the queue
 *                (i.e. before the put operation)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int LL_Insert( t_Handle LL, void *pCur, void *p)
{
   t_LL *p_LL = (t_LL *)LL;
   register int k;
   t_LL_FlowControl *fc;

   /* see if another element will fit into the linked list */
   XX_Freeze();
   k = p_LL->Count;
   if( (p_LL->Max > 0) && (k >= (int)p_LL->Max) )
   {
      p_LL->PutFailures++;
      XX_Thaw();
      return -1;
   }

   /* Add element to linked list */
   if(pCur)
   {
      LL_NEXT(p_LL, p) = LL_NEXT(p_LL, pCur);
      LL_NEXT(p_LL, pCur) = p;

      if(p_LL->Last == pCur)
         p_LL->Last = p;
   }
   else
   {
      LL_NEXT(p_LL, p) = p_LL->First;
      p_LL->First = p;
      if( p_LL->Last == 0 )
         p_LL->Last = p_LL->Read = p;
   }

   p_LL->Count++;
   if( p_LL->Count > p_LL->MaxIn )
      p_LL->MaxIn = p_LL->Count;

   /* Do flow control if necessary */
   if( (fc = p_LL->p_FC) )
      _inc_FC( fc, fc->f_Resources(p) );
    else
        XX_Thaw();

   return k;

}


/*********************************************************************
 * @purpose       Reads the current element in the linked list. The
 *                content of the list are not altered
 *
 *
 * @param  LL     @b{(input)}  handle to the linked list
 *
 * @returns       Returns the element (void pointer) on success
 * @returns       NULL on failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
void *LL_Read( t_Handle LL )
{
   register t_LL *p_LL = (t_LL *)LL;

   return ((void *)p_LL->Read);
}
#endif


/*********************************************************************
 * @purpose       Reads the current value of ReadShiftCount running counter.
 *                The contents of the list are not altered.
 *
 *
 * @param LL      @b{(input)}  handle to the linked list
 *
 * @returns       Returns current value of ReadShiftCount running counter
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
word LL_ReadShiftCount( t_Handle LL )
{
   register t_LL *p_LL = (t_LL *)LL;

   return p_LL->ReadShiftCount;
}
#endif


/*********************************************************************
 * @purpose      Reads the last element in the linked list. The contents
 *               of the list are not altered.
 *
 *
 * @param  LL    @b{(input)}  handle to the linked list
 *
 * @returns      Returns the element (void pointer) on success
 * @returns      NULL on failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
void  *LL_ReadLast( t_Handle LL )
{
   register t_LL *p_LL = (t_LL *)LL;

   return ((void *)p_LL->Last);
}
#endif

/*********************************************************************
 * @purpose      Set offset of linked list. Update Read field to the
 *               specified value.
 *
 *
 * @param  LL    @b{(input)}   handle to the linked list
 * @param  p     @b{(input)}   element to set offset to
 *
 * @returns      n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
void  LL_SetOffset(t_Handle LL, void *p)
{
   register t_LL *p_LL = (t_LL *)LL;

   p_LL->Read = p;
}
#endif



/*********************************************************************
 * @purpose     Routines for altering the current reading position.
 *
 *
 * @param  LL     @b{(input)}     handle to the circular queue
 *
 * @returns       n/a
 *
 * @notes
 *                 LL_Inc advances read pointer by one position
 *                 LL_IncIf advances read pointer only if it was not touched
 *                 LL_Rewind rewinds the current position to the start
 *
 *
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
void LL_Inc( t_Handle LL )
{
   t_LL *p_LL = (t_LL *)LL;

   XX_Freeze();
   if( p_LL->Read )
      p_LL->Read = LL_NEXT(p_LL, p_LL->Read);
   XX_Thaw();
}

void LL_IncIf( t_Handle LL, word c)
{
   t_LL *p_LL = (t_LL *)LL;

   XX_Freeze();
   if( p_LL->ReadShiftCount == c )
      p_LL->Read = LL_NEXT(p_LL, p_LL->Read);
   XX_Thaw();
}



void LL_Rewind( t_Handle LL )
{
   t_LL *p_LL = (t_LL *)LL;

   p_LL->Read = p_LL->First;
}
#endif


/*********************************************************************
 * @purpose      Asks whether the linked list is empty or not
 *
 *
 * @param  LL    @b{(input)}  handle to the linked list
 *
 * @returns      TRUE: the list is currently empty
 * @returns      FALSE: the list is not empty
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
Bool LL_Empty( t_Handle LL )
{
   return ((t_LL *)LL)->Count == 0;
}
#endif



/*********************************************************************
 * @purpose       How many elements are currently in the list
 *
 *
 * @param  LL     @b{(input)}  handle to the linked list
 *
 * @returns       number of elements in the linked list
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
word LL_HowMany( t_Handle LL )
{
   return ((t_LL *)LL)->Count;
}
#endif



/*********************************************************************
 * @purpose    Internal routine. Increment flow control counters.
 *
 *
 * @param  fc      @b{(input)}  pointer to the flow control object
 * @param  n      @b{(input)}   resource counter increment
 *
 * @returns      n/a
 *
 * @notes
 *            This function is always called from within code running
 *            under XX_Freeze(). The corresponding XX_Thaw() is in this
 *            function.
 *
 *
 * @end
 * ********************************************************************/
static void _inc_FC( t_LL_FlowControl *fc, word n )
{
   int  k = fc->CurrLevel;
   word currLevel;

   /* Increment resource count */
   fc->CurrCount += n;
   if( (k < fc->MaxLevel) && (fc->CurrCount >= fc->CongLevels[2*k+1]) )
   {
      fc->CurrLevel++;
      currLevel = fc->CurrLevel;
      XX_Thaw();
      if( fc->f_CongInd )
         fc->f_CongInd( fc->Owner, currLevel );
   }
   else
      XX_Thaw();
}


/*********************************************************************
 * @purpose    Internal routine. Decrement flow control counters.
 *
 *
 * @param  fc     @b{(input)}  pointer to the flow control object
 * @param  n     @b{(input)}   resource counter decrement
 *
 * @returns      n/a
 *
 * @notes
 *               This function is always called from within code running
 *               under XX_Freeze(). The corresponding XX_Thaw() is in this
 *              function
 *
 *
 * @end
 * ********************************************************************/
static void _dec_FC( t_LL_FlowControl *fc, word n )
{
   int k = fc->CurrLevel;
   word currLevel;

   /* Decrement resource count */
   if( fc->CurrCount > n )
      fc->CurrCount -= n;
   else
      fc->CurrCount = 0;

   if( (k > 0) && (fc->CurrCount <= fc->CongLevels[2*(k-1)]) )
   {
      fc->CurrLevel--;
      currLevel = fc->CurrLevel;
      XX_Thaw();
      if( fc->f_CongInd )
         fc->f_CongInd( fc->Owner, currLevel );
   }
    else
       XX_Thaw();
}


/*********************************************************************
 * @purpose    Internal routine. This routine calculates flow control
 *             resources for the given queue element. It is the default
 *             function always returning one.
 *
 *
 * @param  p   @b{(input)} pointer to a queue element
 *
 * @returns    resources for the given element
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static word _dfltResources_FC(void *p)
{
   UNUSED( p );
   return 1;
}


/*********************************************************************
 * @purpose      Internal routine. Delete a flow control object.
 *
 *
 * @param  l     @b{(input)}  pointer to the linked list
 *
 * @returns      n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void _delete_FC( t_LL *l )
{
   if( l->p_FC )
      XX_Free( l->p_FC );
}




/* --- end of file ll.c --- */
