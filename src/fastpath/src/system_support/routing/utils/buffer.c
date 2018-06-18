/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename        buffer.c
 *
 * @purpose         Buffer management implementation
 *
 * @component       Routing Utils Component
 *
 * @comments
 *
 * @create          01/09/1993
 *
 *
 * @author          Jonathan Masel
 *                  Igor Bryskin
 * @end
 *
 * ********************************************************************/
 #ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\buffer.c 1.1.3.1 2002/02/13 23:05:29EST ssaraswatula Development  $";
#endif


#include "string.h"
#include "std.h"
#include "common.h"
#include "xx.ext"
#include "mempool.h"
#include "mem.ext"
#include "buffer.h"
#include "buffer.ext"

t_Handle defBufPool;             /* Default memory pool to allocate buffer structures */
t_Handle defDatPool;             /* Default memory pool to allocate data buffers      */

/* BUFFER MANAGEMENT FUNCTIONS */
#if BUFOBJ_FREELIST

/* Free buffer pool's head */
t_Buffer *freeBufferList;

#define bf_LL_Put(p)                              \
    {                                             \
        XX_Freeze();                              \
        (p)->next = freeBufferList;               \
        freeBufferList = (p);                     \
        XX_Thaw();                                \
    }

#define bf_LL_Get(p)                              \
    {                                             \
        XX_Freeze();                              \
        if( ((p) = freeBufferList) )              \
           freeBufferList = freeBufferList->next; \
        XX_Thaw();                                \
    }

/* Initialization of free buffer list */
void B_FreeListInit( void )
{
   freeBufferList = NULLP;
}

/* Release all buffers from buffer free list */
void B_FreeListFlush( void )
{
   t_Buffer *buf;

   while (TRUE)
   {
      bf_LL_Get(buf);
      if(!buf)
         break;
      MEM_Put(buf->bufPool, buf);
   }
}

#endif

/* Return the default data buffer pool handle */
t_Handle  B_DataPool( void )
{
   return defDatPool;
}

/* Allocate a new buffer from default buffer pools */
void   *B_New(void)
{
   t_Buffer *buf;

   XX_Freeze();
#if BUFOBJ_FREELIST
   bf_LL_Get(buf);
   if( !buf )
#endif
   /* First try to allocate the buffer structure */
   if( (buf = (t_Buffer *)MEM_Get(defBufPool, B_Sizeof())) == NULLP)
   {
     XX_Thaw();
     return NULLP;
   }

   memset(buf, 0, B_Sizeof());
   buf->bufPool  = defBufPool;
   buf->datPool  = defDatPool;
   buf->useCount = 1;

   /* Now try to allocate the data buffer */
   if( (buf->data = MEM_Get(defDatPool, B_BUFFER_SIZE)) == NULLP)
   {
      MEM_Put(defBufPool,buf);
      XX_Thaw();
      return NULLP;
   }

   buf->size = defDatPool ? MEM_GetBufferSize(defDatPool) : B_BUFFER_SIZE;
   buf->head = buf->tail = B_OFFSET;

#ifdef ERRCHK
   buf->status = XX_STAT_Valid;
#endif
   XX_Thaw();
   return (void *)buf;
}

/* Allocate a new buffer from explicit buffer pools */
void   *B_NewEx(t_Handle bufPool, t_Handle datPool, word datSize)
{
   t_Buffer *buf;
#ifdef ERRCHK
   if( bufPool != NULLP)
   {
      MEM_ChkPool(bufPool);
      ASSERT(MEM_GetBufferSize(bufPool) == B_Sizeof());
   }
   if( datPool != NULLP )
      MEM_ChkPool(datPool);
#endif

   XX_Freeze();
   /* Try to allocate the buffer structure */
#if BUFOBJ_FREELIST
   if(bufPool == defBufPool)
   {
      bf_LL_Get(buf);
      if( !buf )
      {
        /* Try to allocate the buffer structure */
        if( (buf = (t_Buffer *)MEM_Get(bufPool, B_Sizeof())) == NULLP)
        {
          XX_Thaw();
          return NULLP;
        }
      }
   }
   else
#endif
   if( (buf = (t_Buffer *)MEM_Get(bufPool, B_Sizeof())) == NULLP)
   {
     XX_Thaw();
     return NULLP;
   }

   memset(buf, 0, B_Sizeof());
   buf->bufPool = bufPool;
   buf->datPool = datPool;
   buf->useCount = 1;

   /* Now allocate the data buffer */
   if(!datSize)
      datSize = datPool ? MEM_GetBufferSize(datPool) : B_BUFFER_SIZE;
   if( (buf->data = MEM_Get(datPool, datSize)) == NULLP)
   {
      MEM_Put(bufPool,buf);
      XX_Thaw();
      return NULLP;
   }

   buf->size = datSize;
   if(datPool && datSize > MEM_GetBufferSize(datPool))
       buf->size = MEM_GetBufferSize(datPool);
   buf->head = buf->tail = B_OFFSET;

#ifdef ERRCHK
   buf->status = XX_STAT_Valid;
#endif
   XX_Thaw();
   return (void *)buf;
}

/* Allocate a new buffer structure and connect the data buffer to it */
void   *B_NewExDat(void    *p_Buff,
                   word     maxSize,
                   word     offset,
                   word     dataSize,
                   t_Handle bufPool,
                   t_Handle datPool)
{
   t_Buffer *buf;
#ifdef ERRCHK
   ASSERT(p_Buff && maxSize);
   if( bufPool != NULLP )
   {
      MEM_ChkPool( bufPool );
      ASSERT(MEM_GetBufferSize( bufPool ) == B_Sizeof());
   }
   if( datPool != NULLP )
   {
      word  size;
      MEM_ChkPool(datPool);
      ASSERT(((size=MEM_GetBufferSize(datPool)) == maxSize) || !size );
   }
   ASSERT(offset < maxSize);
   ASSERT(dataSize <= maxSize);
#endif

#if BUFOBJ_FREELIST
   if(bufPool == defBufPool)
   {
      bf_LL_Get(buf);
      if( !buf )
      {
         /* Try to allocate the buffer structure */
         if( (buf = (t_Buffer *)MEM_Get(bufPool, B_Sizeof())) == NULLP)
            return NULLP;
      }
   }
   else
#endif
   /* Try to allocate the buffer structure */
   if( (buf = (t_Buffer *)MEM_Get(bufPool, B_Sizeof())) == NULLP)
      return NULLP;

   memset(buf, 0, B_Sizeof());
   buf->bufPool = bufPool;
   buf->datPool = datPool;
   buf->useCount = 1;

   buf->data = p_Buff;
   buf->size = maxSize;
   buf->head = offset;
   if( (buf->tail = buf->head+dataSize) > maxSize)
      buf->tail -= maxSize;
   B_SETFULL(buf);
#ifdef ERRCHK
   buf->status = XX_STAT_Valid;
#endif
   return (void *)buf;
}

/* Release a buffer */
void   *B_Delete( void *pBuff, Bool delData)
{
   t_Buffer *buf = (t_Buffer *)pBuff;

   XX_Freeze();
   ASSERT(buf && buf->status == XX_STAT_Valid);

   if(--buf->useCount == 0)
   {
      void *data = buf->data;
      t_Handle datPool = buf->datPool;

#ifdef ERRCHK
      buf->status = XX_STAT_Invalid;
#endif

#if BUFOBJ_FREELIST
      if (buf->bufPool == defBufPool)
         bf_LL_Put(buf)
      else
#endif
      MEM_Put(buf->bufPool, buf);

      if(!delData)
      {
        XX_Thaw();
        return data;
      }

      MEM_Put(datPool, data);
   }
   XX_Thaw();
   return NULLP;
}

/* Release a buffer */
void   *B_DeleteEx( void *pBuff,
                    Bool delData,
                    t_Handle *poolData)
{
   t_Buffer *buf = (t_Buffer *)pBuff;

   XX_Freeze();
   ASSERT(buf && buf->status == XX_STAT_Valid);

   if(--buf->useCount == 0)
   {
      void *data = buf->data;
      t_Handle datPool = buf->datPool;

#ifdef ERRCHK
      buf->status = XX_STAT_Invalid;
#endif

#if BUFOBJ_FREELIST
      if (buf->bufPool == defBufPool)
         bf_LL_Put(buf)
      else
#endif
      MEM_Put(buf->bufPool, buf);

      if(!delData)
      {
         *poolData = datPool;
         XX_Thaw();
         return data;
      }
      MEM_Put(datPool, data);
   }
   XX_Thaw();
   return NULLP;
}

/* Release data buffer allocated from default pool */
void    B_DeleteData( byte *pData )
{
   MEM_Put(defDatPool, pData);
}

/* Release data buffer allocated from explicit pool */
void    B_DeleteDataEx( byte *pData, t_Handle datPool)
{
#ifdef ERRCHK
   MEM_ChkPool(datPool);
#endif
   MEM_Put(datPool, pData);
}

/* Get pointer to data */
void *B_GetDataEx(void *pBuf,
                  word *size,
                  Bool *bigTail,
                  t_Handle *datPool)
{
   t_Buffer *buf = (t_Buffer *)pBuf;
   ASSERT(buf && buf->status == XX_STAT_Valid);
   *size = B_GetLength(buf);
   *bigTail = B_BIG_TAIL(buf);
   *datPool = buf->datPool;
   return B_GetData(buf);
}

/* Add array to start of the buffer*/
word    B_AddToStart( void *pBuf, byte *pData, word len )
{
   word      l;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(len);
   ASSERT(len+B_GetLength (pBuf)<=buf->size);
   if(buf->head < (l=len))
   {
      if(buf->head)
      {
         if(pData)
            memcpy(buf->data, pData+l-buf->head, buf->head);
         l -= buf->head;
      }
      buf->head = buf->size;
   }
   buf->head -= l;
   if(pData)
      memcpy(buf->data + buf->head, pData, l);
   B_SETFULL(buf);
   return len;
}

/* Add array to end  of the buffer */
word    B_AddToEnd( void *pBuf, byte *pData, word len )
{
   word      l, t;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(len);
   ASSERT(len+B_GetLength (pBuf)<=buf->size);
   if((t = buf->size - buf->tail) < (l = len))
   {
      if(t)
      {
         if(pData)
         {
            memcpy(buf->data + buf->tail, pData, t);
            pData += t;
         }
         l -= t;
      }
      buf->tail = 0;
   }
   if(pData)
      memcpy(buf->data + buf->tail, pData, l);
   buf->tail += l;
   B_SETFULL(buf);
   return len;
}

/* Remove data from start of the buffer*/
word    B_CutFromStart( void *pBuf, word len )
{
   word      l, t;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(len);
   ASSERT(len<=B_GetLength (pBuf));

   if((t = buf->size - buf->head) < (l = len))
   {
      l -= t;
      buf->head =0;
   }
   if((buf->head+=l)==buf->size)
      buf->head = 0;
   buf->fFull = FALSE;
   return len;
}

/* Remove data from end of the buffer*/
word    B_CutFromEnd( void *pBuf, word len )
{
   word      l;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(len);
   ASSERT(len<=B_GetLength (pBuf));

   if(buf->tail<(l=len))
   {
      l-=buf->tail;
      buf->tail  = buf->size;
   }
   buf->tail-=l;
   buf->fFull = FALSE;
   return len;
}

/* Remove slice from specified offset in the buffer*/
word B_CutSlice( void *p_Buf, word offset, word len)
{
   word      l, t, offbeg, offend;
   t_Buffer *buf = (t_Buffer *)p_Buf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(len);
   ASSERT(offset+len <= (word)B_GetLength(buf));

   if((offbeg = buf->head + offset) > buf->size)
      offbeg -= buf->size;

   if((offend = offbeg + len) > buf->size)
      offend -= buf->size;

   t = buf->tail >= offend ? buf->tail - offend : buf->size-offend + buf->tail;
   if(t != 0)
   {
      if(t > (l = buf->size - offbeg))
      {
      if(l > (word)(buf->size - offend))
         {
            memcpy(buf->data + offbeg, buf->data + offend, buf->size - offend);
            offbeg += buf->size - offend;
            l -= buf->size - offend;
            t -= buf->size - offend;
            offend = 0;
         }
         memcpy(buf->data + offbeg, buf->data + offend, l);
         offbeg = 0;
         offend += l;
         t -= l;
      }
      if(t >(word)(buf->size - offend))
      {
         memcpy(buf->data + offbeg, buf->data + offend, buf->size - offend);
         offbeg += buf->size - offend;
         t -= buf->size - offend;
         offend = 0;
      }
      memcpy(buf->data + offbeg, buf->data + offend, t);
   }
   buf->tail  = offbeg + t;
   buf->fFull = FALSE;
   return len;
}

/* Read array from the specified offset in the buffer */
word  B_Read(void *p, void *pBuf, word offset, word len)
{
   word      l, t;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(p);
   ASSERT(offset + len <= (word)B_GetLength(buf));

   if((offset = buf->head + offset) > buf->size)
      offset -= buf->size;

   if((t = buf->size - offset) < (l = len))
   {
      memcpy(p, buf->data + offset, t);
      p = (void *)((byte *)p + t);
      offset = 0;
      l -= t;
   }

   memcpy(p, buf->data + offset, l);
   return len;
}

/* Overwrite data in the buffer beginning from the specified offset from array */
word  B_Write(void *p, void *p_Buf, word offset, word Length)
{
   word      l, t;
   t_Buffer *buf = (t_Buffer *)p_Buf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(p && Length);
   ASSERT(offset + Length <= (word)B_GetLength(buf));

   if((offset = buf->head + offset) > buf->size)
      offset -= buf->size;

   if((t = buf->size - offset) < (l = Length))
   {
      memcpy(buf->data + offset, p, t);
      p=(void *)((byte *)p + t);
      offset = 0;
      l -= t;
   }

   memcpy(buf->data + offset, p, l);
   return Length;
}


/* Read array from start and remove data from the buffer */
word  B_ReadStartRemove(void *p, void *pBuf, word len)
{
   word      l, t;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(p);
   ASSERT(len <= (word)B_GetLength(buf));

   if (!len)
      return (0);

   if((t = buf->size - buf->head) < (l = len))
   {
      memcpy(p, buf->data + buf->head, t);
      p = (void *)((byte *)p + t);
      buf->head = 0;
      l -= t;
   }

   memcpy(p, buf->data + buf->head, l);
   if((buf->head+=l)==buf->size)
      buf->head = 0;
   buf->fFull = FALSE;
   return len;
}


/* Read array from end and remove data from the buffer */
word  B_ReadEndRemove(void *p, void *pBuf, word len)
{
   word      l;
   t_Buffer *buf = (t_Buffer *)pBuf;

   ASSERT(buf && buf->status == XX_STAT_Valid);
   ASSERT(p);
   ASSERT(len <= (word)B_GetLength(buf));

   if (!len)
      return (0);

   if(buf->tail < (l = len))
   {
      memcpy((byte *)p + l - buf->tail, buf->data, buf->tail);
      l -= buf->tail;
      buf->tail = buf->size;
   }

   memcpy(p, buf->data + buf->tail - l, l);
   buf->tail -= l;
   buf->fFull = FALSE;
   return len;
}


/* Copy one buffer to another */
word B_Copy(void *destbuf, void *srcbuf)
{
   t_Buffer *dbuf = (t_Buffer *)destbuf;
   t_Buffer *sbuf = (t_Buffer *)srcbuf;

   ASSERT(dbuf && dbuf->status == XX_STAT_Valid);
   ASSERT(sbuf && sbuf->status == XX_STAT_Valid);
   ASSERT(sbuf->size == dbuf->size);

   dbuf->head = sbuf->head;
   dbuf->tail = sbuf->tail;
   dbuf->fFull= sbuf->fFull;

   if(!sbuf->fFull && sbuf->head == sbuf->tail)  /* DAN 14.12.97 */
      return 0;

   if(sbuf->head < sbuf->tail)
   {
      memcpy(dbuf->data + dbuf->head, sbuf->data + sbuf->head,
         sbuf->tail - sbuf->head);
      return (dbuf->tail - dbuf->head);
   }

   memcpy(dbuf->data + dbuf->head, sbuf->data + sbuf->head, sbuf->size - sbuf->head);
   memcpy(dbuf->data, sbuf->data, sbuf->tail);
   return(dbuf->size - dbuf->head + dbuf->tail);
}


/* Getting next buffer. TRUE if the last */
void *B_GetNextBuffer( void *pY, Bool *pLast )
{
   t_Buffer    *pB = (t_Buffer *)pY;

   ASSERT(pB);

   pB = pB->next;

   if( pB )
   {
      if( pB->next )
         *pLast = FALSE;
      else
         *pLast = TRUE;
   }

   return( (void *)pB );
}

/* Rewrap buffer to zero offset */
void *B_ReWrap(void *pBuf)
{
   t_Buffer *buf = (t_Buffer *)pBuf;

   if (B_BIG_TAIL( buf ))
   {
      buf->tail = B_GetLength( buf );
      memmove( buf->data, buf->data + buf->head, buf->tail );
      buf->head = 0;
   }
   else
   {
      word len;
      word hlen;

      hlen = buf->size - buf->head;
      len  = buf->tail + hlen;

      if (hlen <= MAX_REWRAP_INPLACE_SEG)
      {
         byte temp[MAX_REWRAP_INPLACE_SEG];

         memcpy( temp, buf->data + buf->head, hlen );
         memmove( buf->data + hlen, buf->data, buf->tail );
         memcpy( buf->data, temp, hlen );
      }
      else
      {
         byte *temp;

         if( (temp = XX_Malloc( hlen )) == NULLP )
         {
            ASSERT(FALSE);
            return NULLP;
         }
         memcpy( temp, buf->data + buf->head, hlen );
         memmove( buf->data + hlen, buf->data, buf->tail );
         memcpy( buf->data, temp, hlen );

         XX_Free( temp );
      }
      buf->head = 0;
      buf->tail = len;
   }
   return buf;
}
