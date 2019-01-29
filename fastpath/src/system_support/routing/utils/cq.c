/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      cq.c
 *
 * @purpose       Circular queue object.
 *                A circular queue is an object that holds pointers
 *                (usually to frames or buffers). 
 *                Elements are put in and taken out of the queue
 *                in FIFO order. In addition, the queue has a 
 *                current position that may be read (without   
 *                affecting the contents of the queue. 
 *
 * @component     Routing Utils Component
 *
 * @comments      Circular queue object.
 * A circular queue is an object that holds pointers
 * (usually to frames or buffers).
 * Elements are put in and taken out of the queue
 * in FIFO order. In addition, the queue has a
 * current position that may be read (without
 * affecting the contents of the queue.
 *
 * External Routines:
 * CQ_Newcreate a new circular queue
 * CQ_Delete delete a circular queue
 * CQ_Putadd an element to the queue
 * CQ_Getget an element from the queue
 * CQ_Read read the current element
 * CQ_Incadvance the current poisition
 * CQ_HowManyhow many elements are in queue
 * CQ_Emptyis the queue empty?
 *
 * @create         01/01/1995
 *
 * @author         Jonathan Masel
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\cq.c 1.1.3.1 2002/02/13 23:05:45EST ssaraswatula Development  $";
#endif


/* --- standard include files --- */

#include "std.h"
#include "local.h"




/* --- specific include files --- */

#include "cq.h"


/* --- external object interfaces --- */

#include "cq.ext"
#include "xx.ext"


/* --- internal (static) data and prototypes --- */



 /*********************************************************************
 * @purpose         Allocate and initialize a new circular queue.
 *
 *
 * @param Size      @b{(input)}  max number of elements in queue
 *                               (each is 4-bytes long)
 *
 * @returns         Handle to the new object on success
 * @returns         NULLP on failure (out of memory or Size = 0)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle CQ_New( word Size )
{
   t_CQ *p_CQ;
   
   if(!Size)
      return NULLP;
   
   p_CQ = (t_CQ *)XX_Malloc((word) (sizeof(t_CQ) + Size*sizeof(void *)) );
   if( p_CQ )
   {
      p_CQ->Max = Size+1;
      p_CQ->First = 0;
      p_CQ->Last = 0;
      p_CQ->Read = 0;
      p_CQ->PutFailures = 0;
      p_CQ->MaxIn = 0;
   }
   
   return p_CQ;
}


 /*********************************************************************
 * @purpose       Deletes a circular queue. All elements are assumed to
 *                have been discarded prior to this.
 *
 *
 * @param  CQ     @b{(input)}  handle to the circular queue
 *
 * @returns       n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void CQ_Delete( t_Handle CQ )
{
   XX_Free( CQ );
}



/*********************************************************************
 * @purpose       Puts a new element in the circular queue.
 *
 *
 * @param  CQ     @b{(input)}  handle to the circular queue
 * @param  p      @b{(input)}  element to be added
 *
 * @returns       -1        on failure (queue is full)
 * @returns       otherwise the number of elements PREVIOUSLY in the queue
 *                (i.e. before the put operation)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int CQ_Put( t_Handle CQ, void *p )
{
   t_CQ *p_CQ = (t_CQ *)CQ;
   register int n;
   register int k;

   /* see if we can freely advance the last pointer */
   XX_Freeze();
   n = p_CQ->Last;
   k = CQ_HowMany(CQ);
   if( (k+1) == p_CQ->Max )
   {
      p_CQ->PutFailures++;
      XX_Thaw();
      return -1;
   }

   if( ++n == p_CQ->Max )
      n = 0;

   /* add element to queue */
   p_CQ->Table[p_CQ->Last] = p;
   p_CQ->Last = n;
   if( (k+1) > p_CQ->MaxIn )
      p_CQ->MaxIn = k+1;
   XX_Thaw();

   return k;
}


/*********************************************************************
 * @purpose       Gets (and removes) an element from the circular queue.
 *
 *
 * @param  CQ     @b{(input)}  handle to the circular queu
 *
 * @returns       returns the element (void pointer) on success
 * @returns       NULL on failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void *CQ_Get( t_Handle CQ )
{
   t_CQ *p_CQ = (t_CQ *)CQ;
   register int n;
   register int k;
   void *p;

   XX_Freeze();
   n = p_CQ->First;
   /* see if the queue is not empty */
   if( n == p_CQ->Last )
   {
      XX_Thaw();
      return 0;
   }

   p = p_CQ->Table[n];
   if( ++n == p_CQ->Max )
      n = 0;
   if( p_CQ->Read == p_CQ->First )
      p_CQ->Read = n;
   p_CQ->First = n;

   /* see if we've passed our previous maximum */
   k = CQ_HowMany(CQ);
   if( k > p_CQ->MaxIn )
      p_CQ->MaxIn = k;

   XX_Thaw();
   return p;
}


/*********************************************************************
 * @purpose        Reads the current element in the queue. The contents
 *                 of the queue are not altered.
 *
 *
 * @param CQ       @b{(input)}  handle to the circular queue
 *
 * @returns        Returns the element (void pointer) on success
 * @returns        NULL on failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void *CQ_Read( t_Handle CQ )
{
   register t_CQ *p_CQ = (t_CQ *)CQ;
   void *p;

   XX_Freeze();
   /* see if the queue is not empty */
   if( p_CQ->Read == p_CQ->Last )
   {  
      XX_Thaw();
      return 0;
   }
   p = p_CQ->Table[p_CQ->Read];
   XX_Thaw();

   return p;
}



/*********************************************************************
 * @purpose       Routines for altering the current reading position.
 *                CQ_Inc advances one poisition
 *                CQ_Rewind rewinds the current position to the start
 *
 *
 * @param  CQ     @b{(input)}  handle to the circular queue
 *
 * @returns       n/a
 *
 * @notes         CQ_Inc advances one poisition CQ_Rewind rewinds the
 *                current position to the start
 *
 * @end
 * ********************************************************************/
void CQ_Inc( t_Handle CQ )
{
   t_CQ *p_CQ = (t_CQ *)CQ;
   register int n;

   XX_Freeze();
   if( p_CQ->Read == p_CQ->Last )
   {
      XX_Thaw();
      return;
   }
   n = p_CQ->Read + 1;
   if( n == p_CQ->Max )
      n = 0;
   p_CQ->Read = n;
   XX_Thaw();
}

void CQ_Rewind( t_Handle CQ )
{
   t_CQ *p_CQ = (t_CQ *)CQ;

   XX_Freeze();
   p_CQ->Read = p_CQ->First;
   XX_Thaw();
}


/*********************************************************************
 * @purpose      Asks whether the circular queue is empty or not
 *
 *
 * @param  CQ    @b{(input)}  handle to the circular queue
 *
 * @returns      TRUE: the queue is currently empty
 * @returns      FALSE: the queue is not empty
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool CQ_Empty( t_Handle CQ )
{
   return ((t_CQ *)CQ)->First == ((t_CQ *)CQ)->Last;
}

/*********************************************************************
 * @purpose       How many elements are currently in the queue
 *
 *
 * @param  CQ     @b{(input)}  handle to the circular queue
 *
 * @returns       number of elements in the circular queue
 *
 * @notes
 *
 * @end
 * ********************************************************************/
word CQ_HowMany( t_Handle CQ )
{
   register int j = ((t_CQ *)CQ)->Last;
   register int i = ((t_CQ *)CQ)->First;

   return (j>=i? j-i: (j+((t_CQ *)CQ)->Max) - i);
}



/* --- end of file cq.c --- */

