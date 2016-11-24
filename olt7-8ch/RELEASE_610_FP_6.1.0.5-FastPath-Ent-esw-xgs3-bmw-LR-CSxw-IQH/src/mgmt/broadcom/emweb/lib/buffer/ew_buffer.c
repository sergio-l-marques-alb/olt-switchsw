/*
 * Product:  EmStack/Stack
 * Release:  R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 */
#include <stdlib.h>
#include "ew_types.h"
#include "eb_config.h"

#define _EW_BUFFER_C_
#include "ew_buffer.h"

/* ################################################################
 * Configuration Settings
 *
 * This implementation uses the following application-provided items;
 * see ../config/eb_config.h:
 *
 *   EB_MAX_BUFFER_DESCR   - maximum number of buffer descriptors
 *   EB_MAX_BUFFER_SEGS    - maximum number of buffer segments
 *   ebNetBufferSpaceAlloc - routine (or macro) to allocate buffer
 *                            space.
 *   ebNetBufferSpaceFree  - routine (or macro) to free buffer
 *                            space.
 * ################################################################ */

/*
 * Moved these configuration settings to global variables so that they
 * may be overridden at runtime
 */
uintf eb_max_buffer_descr = EB_MAX_BUFFER_DESCR;
uintf eb_max_buffer_segs = EB_MAX_BUFFER_SEGS;

#ifdef EB_SYS_SEG_DYNAMIC
#define ebNetBufferSpaceFree( space ) ebFree( space )
#define ebNetBufferSpaceAlloc( space, len )    \
{                                               \
  space = (uint8 *) ebAlloc( EB_SYS_SEGMENT_SIZE );  \
  len   = EB_SYS_SEGMENT_SIZE;              \
}

#endif

/* ################################################################
 * Types & Constants
 * ################################################################ */

typedef struct EbNetBufferSegment_s
{
  uint8*     block;     /* actual data area */
  uint16     refCount;  /* released when this reaches zero */
  uint16     size;      /* max bytes in block */
} EbNetBufferSegment_t;

#define EB_NET_BUFFER_SEG_NULL ((EbNetBufferSegment)0)

/* ################################################################
 * Static Data
 * ################################################################ */

static uintf DescriptorsAllocated;

static EbNetBuffer FreeDescriptors;

static uintf SegmentsAllocated;

static EbNetBufferSegment FreeSegments;

/* ################################################################
 * Static Function Declarations
 * ################################################################ */

static
EbNetBuffer newBuffer( void );

static
void releaseBuffer( EbNetBuffer bufDesc );

static
EbNetBufferSegment newSegment( void );

static
void releaseSegment( EbNetBufferSegment segDesc );

#ifdef EB_BUFFER_CHECK
#ifdef EB_BUFFER_ATEXIT_CHECK
static void bufferExitCheck(void);
static boolean bufferCheckInitialized;
#endif /* EB_BUFFER_ATEXIT_CHECK */

static int BuffersOutstanding;
static int SegmentsOutstanding;
#endif /* EB_BUFFER_CHECK */


/* ################################################################
 * External Interfaces
 * ################################################################ */

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferAlloc ( void )
{
  EbNetBuffer        buf;
  EbNetBufferSegment seg;
  uint8*              space;
  uintf               len;

  EMBUF_TRACE(("ebNetBufferAlloc\n"));

# ifdef EB_BUFFER_ATEXIT_CHECK
  if ( ! bufferCheckInitialized )
    {
      if ( atexit( bufferExitCheck ) < 0 )
        {
          fprintf( stderr, "BUFFER CHECK: init failed: %s\n"
                  ,strerror(errno)
                  );
          exit(1);
        }
    }
# endif

  ebNetBufferSpaceAlloc( space, len );

  if ( space != NULL )
    {
      seg = newSegment();

      if ( seg != EB_NET_BUFFER_SEG_NULL )
        {
          buf = newBuffer();

          if ( buf != EB_NET_BUFFER_NULL )
            {
              buf->segment = seg;

              seg->size   = len;
              buf->length = len;

              seg->block = space;
              buf->data  = space;

              seg->refCount = 1;
              buf->next     = EB_NET_BUFFER_NULL;
            }
          else
            {
              releaseSegment( seg );
              ebNetBufferSpaceFree( space );
 
              seg = NULL;
              buf = EB_NET_BUFFER_NULL;

            }
        }
      else
        {
          ebNetBufferSpaceFree( space );
          buf = EB_NET_BUFFER_NULL;
        }
    }
  else
    {
      buf = EB_NET_BUFFER_NULL;
      EMBUF_WARN(("ebNetBufferSpaceAlloc() failed\n"));
    }

  EMBUF_TRACE(("ebNetBufferAlloc %p\n", buf));

  return buf;
}



_EXTERN_EW_BUFFER
void         ebNetBufferFree ( EbNetBuffer buffer )
{
  EbNetBuffer nxtBuf;

  EMBUF_TRACE(("ebNetBufferFree %p\n", buffer));

  for ( ; buffer != EB_NET_BUFFER_NULL; buffer = nxtBuf )
    {
      nxtBuf = buffer->next;
                   
      if (buffer->segment->refCount > 0)
      {
        if ( --buffer->segment->refCount == 0 )
        {
          releaseSegment( buffer->segment );
        }
      }

      releaseBuffer( buffer );
    }
}

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferReset ( EbNetBuffer buffer )
{
  EMBUF_TRACE(("ebNetBufferReset\n"));

  /* if buffer is only in use by me, safe to change
   * buffer pointers...
   */
  if (buffer->segment->refCount == 1)
    {
      if ( buffer->next != EB_NET_BUFFER_NULL )
        {
          ebNetBufferFree( buffer->next );
          buffer->next = EB_NET_BUFFER_NULL;
        }
      buffer->data   = buffer->segment->block;
      buffer->length = buffer->segment->size;
      return( buffer );
    }
  else          /* shared buffer */
    {
      /* free the buffer - others are using it, so it's
       * data will stick around.  Get us a new one.
       */
      ebFree( buffer );
      return( ebNetBufferAlloc() );
    }
}


_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferPrepend( EbNetBuffer buffer, uintf headLen )
{
  EbNetBuffer newbuf;

  EMBUF_TRACE(("ebNetBufferPrepend %p %d\n", buffer, headLen));

  if (   (buffer->segment->refCount == 1)
      && (uintf)( buffer->data - buffer->segment->block ) >= headLen
     )
    {
      newbuf = buffer;
      buffer->data   -= headLen;
      buffer->length += headLen;
    }
  else
    {
      newbuf = ebNetBufferAlloc();

      if ( newbuf != EB_NET_BUFFER_NULL )
        {
          newbuf->next   = buffer;
          newbuf->data   = newbuf->segment->block + newbuf->length - headLen;
          newbuf->length = headLen;
        }
    }

  return newbuf;
}

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferCopy( EbNetBuffer parentBuf, uintf copyLength )
{
  EbNetBuffer copy;
  uintf        thisLength;

  EMBUF_TRACE(("ebNetBufferCopy\n"));

  thisLength = parentBuf->length;

  copy = newBuffer();
  if ( copy != EB_NET_BUFFER_NULL )
    {
      if ( copyLength > thisLength )
        {
          copy->next = ebNetBufferCopy( parentBuf->next
                                        ,copyLength - thisLength
                                        );

          if ( copy->next == EB_NET_BUFFER_NULL )
            {
              /* failed - release now-useless copy descriptor */
              releaseBuffer( copy );
              copy = EB_NET_BUFFER_NULL;
            }
        }
      else
        {
          thisLength = copyLength;
          copy->next = EB_NET_BUFFER_NULL;
        }
    }

  if ( copy != EB_NET_BUFFER_NULL )
    {
      copy->length  = thisLength;
      copy->data    = parentBuf->data;
      copy->segment = parentBuf->segment;

      copy->segment->refCount++; /* bump seg refcount only when copy succeeds */
    }

  return copy;
}

void ebNetBufferShutdown( void )
{
  EbNetBuffer thisBuf;
  EbNetBufferSegment thisSeg;

  while (FreeDescriptors != EB_NET_BUFFER_NULL)
    {
      thisBuf = FreeDescriptors;
      FreeDescriptors = FreeDescriptors->next;
      ebFree( thisBuf );
    }
  DescriptorsAllocated = 0;

  while(FreeSegments)
    {
      thisSeg = FreeSegments;
      FreeSegments = (EbNetBufferSegment)(thisSeg->block);
      ebFree( thisSeg );
    }
  SegmentsAllocated = 0;

# ifdef EB_BUFFER_CHECK
  BuffersOutstanding = 0;
  SegmentsOutstanding = 0;
# endif /* EB_BUFFER_CHECK */

}

_EXTERN_EW_BUFFER
boolean ebNetBufferFreeCheck( EbNetBuffer checkBuf )
{
  EbNetBuffer thisBuf;

  while (FreeDescriptors != EB_NET_BUFFER_NULL)
  {
    if (checkBuf == FreeDescriptors)
      return FALSE;
    thisBuf = FreeDescriptors;
    FreeDescriptors = FreeDescriptors->next;
  }
  return TRUE;
}


/* ################################################################
 * Static Functions
 * ################################################################ */

static
EbNetBuffer newBuffer( void )
{
  EbNetBuffer buf;

  if ( FreeDescriptors != EB_NET_BUFFER_NULL )
    {
      buf = FreeDescriptors;
      FreeDescriptors = buf->next;
      buf->next = EB_NET_BUFFER_NULL;
    }
  else
    {
      if ( DescriptorsAllocated < eb_max_buffer_descr )
        {
          EMBUF_TRACE(("newBuffer(): Allocating buffer %ld\n", DescriptorsAllocated));
          buf = (EbNetBuffer) ebAlloc( sizeof( EbNetBuffer_t ) );
          if ( buf != EB_NET_BUFFER_NULL )
            {
              DescriptorsAllocated++;
              buf->next = EB_NET_BUFFER_NULL;
            }
        }
      else
        {
          buf = EB_NET_BUFFER_NULL;
        }
    }

# ifdef EB_BUFFER_CHECK
  if ( buf != EB_NET_BUFFER_NULL )
    {
      buf->header_magic = 0x01AB02CD;
      buf->inuse = 1;
      BuffersOutstanding++;
      EMBUF_TRACE(("Buffer: Descriptor Alloc %d\n",BuffersOutstanding));
    }
  else
    {
      EMBUF_TRACE(("Buffer: Descriptor Alloc %d FAILED\n",BuffersOutstanding));
    }
# endif
  return buf;
}

static
void releaseBuffer( EbNetBuffer bufDesc )
{
# ifdef EB_BUFFER_CHECK
  if (bufDesc->header_magic != 0x01AB02CD)
    {
      EMBUF_ERROR(("releaseBuffer: bad hdr magic\n"));
    }
  if (bufDesc->inuse != 1)
    {
      EMBUF_ERROR(("releaseBuffer: buffer already free\n"));
    }
  bufDesc->inuse = 0;

  FreeDescriptors  = EB_NET_BUFFER_NULL;
  ebFree( bufDesc );
  DescriptorsAllocated--;
  BuffersOutstanding--;
  EMBUF_TRACE(("Buffer: Descriptor Free %d\n", BuffersOutstanding));
# else
  bufDesc->next = FreeDescriptors;
  FreeDescriptors = bufDesc;
# endif
}

static
EbNetBufferSegment newSegment( void )
{
  EbNetBufferSegment seg;

  if ( FreeSegments != NULL )
    {
      seg = FreeSegments;
      FreeSegments = (EbNetBufferSegment)seg->block;
      seg->block = NULL;
    }
  else
    {
      if ( SegmentsAllocated < eb_max_buffer_segs )
        {
          seg = (EbNetBufferSegment) ebAlloc( sizeof( EbNetBufferSegment_t ) );
          if ( seg != NULL )
            {
              SegmentsAllocated++;
              seg->block = NULL;
            }
        }
      else
        {
          seg = EB_NET_BUFFER_SEG_NULL;
        }
    }

#ifdef EB_BUFFER_CHECK
  if ( seg != NULL )
    {
      SegmentsOutstanding++;
      EMBUF_TRACE(("Buffer: Segment Alloc %d\n", SegmentsOutstanding));
    }
  else
    {
      EMBUF_TRACE(("Buffer: Segment Alloc %d FAILED\n", SegmentsOutstanding));
    }
#endif
  return seg;
}


static
void releaseSegment( EbNetBufferSegment segDesc )
{
  if( NULL != segDesc->block )
    {
  ebNetBufferSpaceFree( segDesc->block );
    }

  segDesc->block = (uint8*)FreeSegments;
  FreeSegments = segDesc;

#ifdef EB_BUFFER_CHECK
  SegmentsOutstanding--;
  EMBUF_TRACE(("Buffer: Segment Free %d\n",SegmentsOutstanding));
#endif
}

#ifdef EB_BUFFER_CHECK
uintf ebBuffersInUse( void )
{
  uintf inUse;

  inUse = BuffersOutstanding + SegmentsOutstanding;
  return inUse;
}

#ifdef EB_BUFFER_ATEXIT_CHECK
static
void bufferExitCheck( void )
{
  if ( ebBuffersInUse() )
    {
      if ( BuffersOutstanding )
        {
          fprintf( stderr, "BUFFER CHECK: %d descriptors outstanding.\n"
                  ,BuffersOutstanding
                  );
        }
      if ( SegmentsOutstanding )
        {
          fprintf( stderr, "BUFFER CHECK: %d segments outstanding.\n"
                  ,SegmentsOutstanding
                  );
        }
    }
}
#endif /* EB_BUFFER_ATEXIT_CHECK */
#endif /* EB_BUFFER_CHECK */


/*
*** Local Variables: ***
*** mode: c ***
*** tab-width: 4 ***
*** End: ***
 */
