/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     cb.c
 *
 * @purpose      Circular buffer object.
 *               A circular buffer is an object to which data
 *               is copied in and out. On reaching the end of the
 *               is copied in and out. On reaching the end of the
 *               buffer, we loop back to the start to continue.
 *               New data will automatically (and sliently) 
 *               overwrite old data should the "put" pointer lap
 *               the "get" pointer.      
 *
 * @component    Routing Utils Component
 *
 * @comments
 *       CB_Newcreate a new circular buffer
 * CB_Delete delete a circular buffer
 * CB_Putadd data to the buffer
 * CB_Getget data from the buffer
 *
 *
 * @create     10/01/1995
 *
 * @author     Jonathan Masel  
 * @end
 *
 *********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/cb.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif

/* --- standard include files --- */

#include <string.h>

#include "std.h"
#include "local.h"



/* --- specific include files --- */


/* --- external object interfaces --- */

#include "cb.ext"
#include "mem.ext"
#include "xx.ext"


/* --- internal (static) data and prototypes --- */




/*********************************************************************
 * @purpose          Allocate and initialize a new circular buffer.
 *
 *
 * @param  Size      @b{(input)}  the size in bytes of the buffer
 * @param  p_H       @b{(input)}  handle to new circular buffer returned
 *                                here
 *
 * @returns          Handle to new circular buffer on success, NULL
 *                   otherwise.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle CB_New( word Size )
{
   byte           *m;
   t_CircularBuff *p;

   m = (byte *)XX_Malloc( sizeof(t_CircularBuff) + Size );
   if( !m )
      return 0;

   p = (t_CircularBuff *)m;

   /* initialize to zero */
   memset( p, 0, sizeof(t_CircularBuff) );

   p->Size = Size;

   /* the data buffer is at the end of the data structure */
   p->p_Start = m + sizeof(t_CircularBuff);
   p->p_End = p->p_Start + Size;

   p->p_Put = p->p_Start;
   p->p_Get = p->p_Start;

   return (t_Handle)p;
}


/*********************************************************************
 * @purpose        Delete a circular buffer
 *
 *
 * @param  h       @b{(input)}  handle to the circular buffer
 *
 * @returns        n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void CB_Delete( t_Handle h )
{
   if( h )
      XX_Free( h );
}





/*********************************************************************
 * @purpose       Put data into the circular buffer (overwriting old data
 *                                                  if necessary).
 *
 *
 * @param  h       @b{(input)}  handle to circular buffer
 * @param  p_D     @b{(input)}  copy data from this address
 * @param  Count   @b{(input)}  number of bytes to copy to buffer
 *
 * @returns        The number of bytes in the buffer after this put operation.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int CB_Put( t_Handle h, byte *p_D, int Count )
{
   register t_CircularBuff *p = (t_CircularBuff *)h;
   register int n;            /* n = number of bytes left to transfer */
   register int k;            /* k = number of bytes transferred this loop */

   if( !h )
      return 0;

   /* check that the data will fit */
   if( (p->Count + Count) > p->Size )
   {
      p->PutFailures += Count;
      return 0;
   }

   n = Count;
   while( n > 0 )
   {
      /* copy from current position to end */
      k = (int)(p->p_End - p->p_Put);
      if( k > n )
         k = n;

      /* copy data to buffer */
      if( p_D )
         memcpy( p->p_Put, p_D, k );
      p->p_Put += k;
      if( p->p_Put == p->p_End )
         p->p_Put = p->p_Start;
      p_D += k;
      p->Count += k;
      p->PutCount += k;

      /* and get ready for next loop */
      n -= k;
   }

   return p->Count;
}





/*********************************************************************
 * @purpose        Get data from the circular buffer.
 *
 *
 * @param  h       @b{(input)}  handle to circular buffer
 * @param  p_D     @b{(input)}  copy data to this address
 * @param  Count   @b{(input)}  number of bytes to read from the buffer
 *
 * @returns        The number of bytes actually read from the buffer
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int CB_Get( t_Handle h, byte *p_D, int Count )
{
   register t_CircularBuff *p = (t_CircularBuff *)h;
   register int n;            /* n = number of bytes left to transfer */
   register int k;            /* k = number of bytes transferred this loop */

   if( !h )
      return 0;

   k = p->Count;
   n = (Count > k? k: Count);
   while( n > 0 )
   {
      /* how many bytes can we transfer this loop */
      if( p->p_Get >= p->p_Put )
         k = (int)(p->p_End - p->p_Get);
      else
         k = (int)(p->p_Put - p->p_Get);

      if( k > n )
         k = n;

      /* copy data from buffer */
      memcpy( p_D, p->p_Get, k );
      p->p_Get += k;
      p->GetCount += k;
      p_D += k;

      /* and get ready for next loop */
      n -= k;
      if( n > 0 )
         p->p_Get = p->p_Start;
   }

   Count -= n;
   if( p->Count > Count )
      p->Count -= Count;
   else
      p->Count = 0;

   return n;
}




/*********************************************************************
 * @purpose        How many bytes are in the circular buffer.
 *
 *
 * @param  h       @b{(input)}  handle to circular buffer
 *
 * @returns        The number of bytes in the buffer
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#if L7_DEBUG
int CB_HowMany( t_Handle h )
{
   return h? ((t_CircularBuff *)h)->Count: 0;
}
#endif


