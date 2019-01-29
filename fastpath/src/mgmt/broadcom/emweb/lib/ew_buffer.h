/*
 * Product: EmWeb/EmStack
 * Release: R6_2_0
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
 *
 * EmStack/Stack Buffer Interface
 *
 */
#ifndef _EW_BUFFER_H_
#define _EW_BUFFER_H_


#ifndef _EW_BUFFER_C_
#   define _EXTERN_EW_BUFFER extern
#else
#   define _EXTERN_EW_BUFFER
#endif

#include "eb_config.h"

/*
 * This is a reference implementation of the network buffer support
 * needed by both EmWeb/Server and EmStack.
 *
 * Both EmWeb/Server and EmStack can use the same buffer implementation.
 * However, these products use different names for these buffer interfaces.
 * This was done to allow for different buffer implementations across
 * these two products, although the same implementation _may_ be used.
 *
 * The interfaces defined here are prefixed with "eb".  The
 * system-dependent configuration information for this file can be found in
 * config.<$CONFIG>/eb_config.h.
 *
 * By default, each product (EmWeb/EmStack) includes this file, and renames
 * these interfaces to match their requirements (see ew_config.h/es_config.h).
 *
 */


/*
 * NETWORK BUFFERS HANDLE
 */

typedef struct EbNetBuffer_s* EbNetBuffer;

/* typedefs for reference implementation */

typedef struct EbNetBufferSegment_s* EbNetBufferSegment;

typedef struct EbNetBuffer_s
{
  EbNetBuffer          next;
# ifdef EB_BUFFER_CHECK
  uint32               header_magic;   /* valid header */
  uintf                inuse;          /* 1 if alloced */
# endif
  EbNetBufferSegment   segment;
  uint8*                data;
  uintf                 length;
} EbNetBuffer_t;

#define EB_NET_BUFFER_NULL     ((EbNetBuffer)0)

#if 0 /* prototypes - use these if implementing as subroutines */

/* ################################################################
 * Buffer Primitives
 *    These definitions specify what the primitives do;
 *    they may be implemented as either macros or subroutines.
 * ################################################################ */

/*
 * Allocate and Free
 *
 * ebNetBufferAlloc
 *     Returns a buffer handle, or EB_NET_BUFFER_NULL on failure.
 *     The length of the buffer must be initialized to the number
 *     of bytes available for use.
 */
_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferAlloc ( void );

/*
 * ebNetBufferReset
 *     Reset the data pointer to the location it would be in a
 *     newly allocated buffer, and reset the length to the maximum
 *     available; used when turning around an inbound buffer to use
 *     as an outbound buffer.
 */
_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferReset( EbNetBuffer buffer );

/*
 * ebNetBufferFree
 *     Release a buffer and any buffers chained after it.
 */
_EXTERN_EW_BUFFER
void         ebNetBufferFree ( EbNetBuffer buffer );

/*
 * Length Manipulation
 *     Get or Set the length of the specified buffer segment.
 *
 *   - Note that the length of a newly allocated buffer must be
 *     set to the maximum size of the segment, and may then be
 *     modified using the Set primitive to show the amount of data
 *     actually in the buffer.
 */
_EXTERN_EW_BUFFER
uintf        ebNetBufferLengthGet( EbNetBuffer buffer );

_EXTERN_EW_BUFFER
void         ebNetBufferLengthSet( EbNetBuffer buffer, uintf len );

/*
 * Data Pointer Manipulation
 *     Get or Set the data pointer for a buffer segment.
 *
 *   - The data pointer may be Set only forward in the buffer;
 *     that is; its purpose is to remove headers.
 *   - When initially allocated, the data pointer for a segment
 *     may or may not point to the actual beginning of the
 *     data segment; this is an implementation decision.
 *   > Also see the Prepend primitive, below
 */
_EXTERN_EW_BUFFER
uint8*       ebNetBufferDataGet( EbNetBuffer buffer );

_EXTERN_EW_BUFFER
void         ebNetBufferDataSet( EbNetBuffer buffer, void* datap );

/*
 * Chain Manipulation
 *     Get or Set the link pointer to the buffer following a
 *     specified buffer.
 *   - EB_NET_BUFFER_NULL is used to indicate the end of the chain.
 */
_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferNextGet( EbNetBuffer buffer );

_EXTERN_EW_BUFFER
void         ebNetBufferNextSet( EbNetBuffer buffer,  EbNetBuffer next );

/*
 * Prepend
 *     The Prepend operation returns a buffer handle which has
 *     enough room for headLen bytes preceeding the specified
 *     buffer.
 *
 *     ! The space specified by 'headLen' MUST NOT span a buffer
 *     ! boundary.  If 20 bytes are requested and 10 are available
 *     ! in the buffer, this routine may not prepend another buffer
 *     ! and use 10 from that and 10 in the current buffer.
 *
 *      - If the specified buffer already had headLen bytes in
 *        its data space, Prepend should return the same buffer
 *        pointer, with its data pointer moved and length
 *        adjusted by headLen.
 *      - If the specified buffer does not have room for the
 *        header, Prepend should allocate a new buffer, chain the
 *        specified buffer after it, and return it with its length
 *        preset to headLen (in this case, it is recommended that
 *        the data pointer of the new buffer be set to the last
 *        headLen bytes of the data space, so that a subsequent
 *        Prepend will have room).
 *
 *   > May return EB_NET_BUFFER_NULL if there was not enough room and
 *     no new buffer could be allocated.
 */
_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferPrepend( EbNetBuffer buffer, uintf headLen );

/*
 * Duplicate a single buffer
 *     Returns a buffer handle for a new buffer (may be
 *     EB_NET_BUFFER_NULL if allocation failed) with a copy of
 *     'length' bytes of the data from the specified buffer.
 *
 *     The copy SHOULD be implemented without physically
 *     copying the data; allocating a new descriptor which
 *     increments a reference count on the data segment.
 */
_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferCopy( EbNetBuffer buffer, uintf length );

/*
 * Release all dynamic resources used for buffer system
 */
_EXTERN_EW_BUFFER
void ebNetBufferShutdown( void );

#endif /* prototypes for buffer primitives */

/* ################################################################
 * Example Implementation
 *    Everything below here is part of a reference implementation
 *    of the required buffer primitives; so long as your implementation
 *    meets the requirements defined above it may or may not be
 *    done in the same way.
 * ################################################################ */

/* these are simple enough to implement as macros */

#define ebNetBufferLengthGet(buffer)      (buffer)->length
#define ebNetBufferLengthSet(buffer, len) (buffer)->length = len
#define ebNetBufferDataGet(buffer)        (buffer)->data
#define ebNetBufferDataSet(buffer, datap) (buffer)->data = datap
#define ebNetBufferNextGet(buffer)        (buffer)->next
#define ebNetBufferNextSet(buffer, nxt)   (buffer)->next = nxt

/* prototypes copied from above for things implemented as subroutines */

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferAlloc ( void );

_EXTERN_EW_BUFFER
void         ebNetBufferFree ( EbNetBuffer buffer );

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferReset( EbNetBuffer buffer );

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferPrepend( EbNetBuffer buffer, uintf headLen );

_EXTERN_EW_BUFFER
EbNetBuffer ebNetBufferCopy( EbNetBuffer buffer, uintf length );

_EXTERN_EW_BUFFER
void ebNetBufferShutdown( void );

_EXTERN_EW_BUFFER
boolean ebNetBufferFreeCheck( EbNetBuffer checkBuf );


#ifdef EB_BUFFER_CHECK
_EXTERN_EW_BUFFER
/*
 * Returns the number of buffers + segments not free
 * (really only for use in testing for leaks)
 */
uintf ebBuffersInUse( void );
#endif

extern uintf eb_max_buffer_descr;
extern uintf eb_max_buffer_segs;


/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EW_BUFFER_H_ */
