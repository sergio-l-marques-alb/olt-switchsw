/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename        xxtrace.c
 *
 * @purpose         Trace utility
 *
 * @component       Routing Utils Component
 *
 * @comments
 *
 * @create          
 *
 * @author
 *
 * @end                                                                        
 *
 * ********************************************************************/
#include "std.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xx.ext"
#include "xxtrace.ext"

typedef struct tagt_XXTraceEntry
{
   const char *who;
   U32        ts;
   U32        d1;
   U32        d2;
   U32        d3;
   U32        d4;
} t_XXTraceEntry ;

typedef struct tagt_XXTrace
{
   struct tagt_XXTrace *next;
   struct tagt_XXTrace *prev;
   t_XXTraceEntry *arBuf;
   char           *name;
   int            size;
   int            in;
   int            last;
   Bool           enable;
} t_XXTrace;

/* Trace list top */
static t_XXTrace *_XXTraceList;


/* XXTrc_Create
 * Creates a new circular trace buffer
 * Returns trace buffer handle
 */
t_Handle XXTrc_Create( const char *name, /* Trace buffer name */
                       int nBufs )       /* Number of entries */
{
   t_XXTrace *pTrc;
   U32       len = sizeof( t_XXTrace ) + (name?strlen( name ):0) + 1;

   pTrc = XX_Malloc( len );
   if ( pTrc == NULLP )
      return NULLP;
   memset( pTrc, 0, len );
   pTrc->name = (char *)(pTrc + 1);
   if (name)
     strcpy( pTrc->name, name );
   pTrc->arBuf = XX_Malloc( sizeof( t_XXTraceEntry ) * nBufs );
   if ( pTrc->arBuf == NULLP )
   {
      XX_Free( pTrc );
      return NULLP;
   }
   memset( pTrc->arBuf, 0, sizeof( t_XXTraceEntry ) *nBufs );
   pTrc->size = nBufs;
   pTrc->last = nBufs-1;
   XX_AddToDLList( pTrc, _XXTraceList );
   return pTrc;
}


/* XXTrc_Enable
 * Enable trace buffer
 * Returns the previous status: TRUE=enabled, FALSE=disabled
 */
Bool XXTrc_Enable( t_Handle hTrace )    /* Trace buffer handle. NULL=last created */
{
   t_XXTrace *pTrc = (t_XXTrace *)hTrace;
   Bool      enable;

   /* Last created trace is the default */
   if ( !pTrc )
   {
      pTrc = _XXTraceList;
      if ( !pTrc )
         return FALSE;
   }
   enable = pTrc->enable;
   pTrc->enable = TRUE;
   return enable;
}


/* XXTrc_Disable
 * Disable trace buffer
 * Returns the previous status: TRUE=enabled, FALSE=disabled
 */
Bool XXTrc_Disable( t_Handle hTrace ) /* Trace buffer handle. NULL=last created */
{
   t_XXTrace *pTrc = (t_XXTrace *)hTrace;
   Bool      enable;

   /* Last created trace is the default */
   if ( !pTrc )
   {
      pTrc = _XXTraceList;
      if ( !pTrc )
         return FALSE;
   }
   enable = pTrc->enable;
   pTrc->enable = FALSE;
   return enable;
}


/* XXTrc_Put
 * Add entry to the trace buffer
 */
void XXTrc_Put( t_Handle hTrace,     /* Trace buffer handle. NULL=last created */
                const char *who,     /* Tracepoint identification */
                U32 d1, U32 d2, U32 d3, U32 d4 ) /* Entry data */
{
   t_XXTrace *pTrc = (t_XXTrace *)hTrace;
   t_XXTraceEntry *pEntry;

   /* Last created trace is the default */
   if ( !pTrc )
      pTrc = _XXTraceList;

   if ( !pTrc || !pTrc->enable )
      return;
 
   XX_Freeze( );
   if ( (++pTrc->last) == pTrc->size )
      pTrc->last = 0;
   if ( pTrc->in < pTrc->size )
      ++pTrc->in;
   XX_Thaw( );
   
   pEntry = &pTrc->arBuf[pTrc->last];
   pEntry->ts = XX_TRACE_GET_TIMESTAMP;
   pEntry->who = who;
   pEntry->d1 = d1;
   pEntry->d2 = d2;
   pEntry->d3 = d3;
   pEntry->d4 = d4;
}


/* XXTrc_Print
 * Print recorded data
 */
void XXTrc_Print( t_Handle hTrace )  /* Trace buffer handle. NULL=last created */
{
   t_XXTrace   *pTrc = (t_XXTrace *)hTrace;
   Bool        trcEnable;
   char        printBuf[80];
   int         first;
   int         pos;
   int         i;

   /* Last created trace is the default */
   if ( !pTrc )
   {
      pTrc = _XXTraceList;
      if ( !pTrc )
      {
         XX_Print( "No traces" );
         return;
      }
   }

   /* Temporary disable trace */
   trcEnable = pTrc->enable;
   pTrc->enable = FALSE;
   sprintf( printBuf, "\n***Trace %s ***, %s", 
            pTrc->name, trcEnable?"active":"inactive" );
   XX_Print( printBuf );
   sprintf( printBuf, "%d entries in reverse order:", pTrc->in );
   XX_Print( printBuf );
   XX_Print( "TimeStamp:delta: who   d1  d2  d3  d4" );

   pos = pTrc->last;
   if ( pTrc->in < pTrc->size )
      first = pTrc->last - pTrc->in + 1;
   else
   {
      first = pTrc->last + 1;
      if ( first == pTrc->size )
         first = 0;
   }
   for( i=0; i<pTrc->in; i++ )
   {
      t_XXTraceEntry *pEntry = &pTrc->arBuf[pos];
      U32            delta;

      if ( pos != first )
      {
         /* Prev entry pos */
         if ( !pos ) pos = pTrc->size-1;
         else --pos;
         delta = pEntry->ts - pTrc->arBuf[pos].ts;
      }
      else 
         delta = 0;
      
      sprintf( printBuf, "%ld:%ld:\t%s\t0x%lx\t0x%lx\t0x%lx\t0x%lx", 
               pEntry->ts, delta, pEntry->who, pEntry->d1,
               pEntry->d2, pEntry->d3, pEntry->d4 );
      XX_Print( printBuf );
   }

   /* Re-enable if necessary */
   pTrc->enable = trcEnable;
}


/* XXTrc_Delete
 * Delete trace buffer
 */
void XXTrc_Delete( t_Handle hTrace )
{
   t_XXTrace   *pTrc = (t_XXTrace *)hTrace;
   ASSERT( pTrc );
   XX_DelFromDLList( pTrc, _XXTraceList );
   if ( pTrc->arBuf )
      XX_Free( pTrc->arBuf );
   XX_Free( pTrc );
}

