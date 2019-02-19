/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename    frame.c
 *
 * @purpose     Frame management implementation
 *
 * @component   Routing Utils Component
 *
 * @comments
 *
 * @create      01/09/1993
 *
 * @author      Jonathan Masel
 *              Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/frame.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif


#include "string.h"
#include "xx.ext"
#include "mempool.h"
#include "mem.ext"
#include "buffer.h"
#include "buffer.ext"
#include "frame.h"
#include "frame.ext"

#ifdef FRMCHK
#include <stdio.h>
t_Frame *FrameList;                     /* frame list anchor */
#endif
/* FRAME MANAGEMENT FUNCTIONS */

static t_Handle defFramesPool;          /* default memory pool to allocate frames            */
extern t_Handle defBufPool;             /* default memory pool to allocate buffer structures */
extern t_Handle defDatPool;             /* default memory pool to allocate data buffers      */

#if FRMOBJ_FREELIST

/* Free frame pool's head */
t_Frame *freeFrameList;

#define fr_LL_Put(p)                              \
    {                                             \
        XX_Freeze();                              \
        (p)->nextFrame = freeFrameList;           \
        freeFrameList = (p);                      \
        XX_Thaw();                                \
    }

#define fr_LL_Get(p)                              \
    {                                             \
        XX_Freeze();                              \
        if( ((p) = freeFrameList) )               \
           freeFrameList = freeFrameList->nextFrame; \
        XX_Thaw();                                \
    }

/* Initialization of free frame list */
void F_FreeListInit( void )
{
   freeFrameList = NULLP;
}

/* Release all frames from frame free list */
void F_FreeListFlush( void )
{
   t_Frame *frame;

   while (TRUE)
   {
      fr_LL_Get(frame);
      if( !frame )
         break;
      MEM_Put(frame->framesPool, frame);
   }
}

#endif

/* Assign default pools for frames, buffer structures, and data buffers */
e_Err F_AssignDefPools(t_Handle def_frames_pool,
                       t_Handle def_buf_pool,
                       t_Handle def_dat_pool)
{
#ifdef ERRCHK
   if( def_frames_pool != NULLP )
   {
      MEM_ChkPool(def_frames_pool);
      ASSERT(MEM_GetBufferSize(def_frames_pool) == F_Sizeof());
   }
   if( def_buf_pool != NULLP)
   {
      MEM_ChkPool(def_buf_pool);
      ASSERT(MEM_GetBufferSize(def_buf_pool) == B_Sizeof());
   }
   if( def_dat_pool != NULLP )
      MEM_ChkPool(def_dat_pool);
#endif

   defFramesPool =  def_frames_pool;
   defBufPool    =  def_buf_pool;
   defDatPool    =  def_dat_pool;

   return E_OK;
}

/* Allocate a new frame from default pool */
#ifdef FRMCHK
void *_F_New(void *p_Buff, const char *file, word line)
#else
void *F_New( void *p_Buff)
#endif
{
   t_Buffer *buf = (t_Buffer *)p_Buff;
   t_Frame  *frame;

   XX_Freeze();
#if FRMOBJ_FREELIST
   fr_LL_Get(frame);
   if( !frame )
#endif
   /* Try to allocate the frame structure */
   if( (frame = MEM_Get(defFramesPool, F_Sizeof())) == NULLP)
   {
     XX_Thaw();
     return NULLP;
   }

   memset(frame, 0, F_Sizeof());
   frame->bufPool    = defBufPool;
   frame->datPool    = defDatPool;
   frame->framesPool = defFramesPool;
   frame->useCount   = 1;

   if(buf)
   {
      ASSERT(buf->status == XX_STAT_Valid);
      buf->prev = buf->next = NULLP;
      frame->firstBuf = buf;
      frame->frameLen = B_GetLength(buf);
   }

#ifdef ERRCHK
   frame->status = XX_STAT_Valid;
#endif

#ifdef FRMCHK
   frame->allocFile = file;
   frame->allocLine = line;
   XX_AddToDLList(frame, FrameList);
#endif
   XX_Thaw();
   return(void *)frame;
}

/* Allocate a new frame from explicit pool */
#ifdef FRMCHK
void *_F_NewEx(void *p_Buff,
           t_Handle framesPool,
           t_Handle bufPool,
           t_Handle datPool,
           const char *file,
           word line)
#else
void   *F_NewEx(void *p_Buff,
                t_Handle framesPool,
                t_Handle bufPool,
                t_Handle datPool)
#endif
{
   t_Buffer *buf = (t_Buffer *)p_Buff;
   t_Frame  *frame;

#ifdef ERRCHK
   if( framesPool != NULLP )
   {
      MEM_ChkPool(framesPool);
      ASSERT(MEM_GetBufferSize(framesPool) == F_Sizeof());
   }
   if( bufPool != NULLP)
   {
      MEM_ChkPool(bufPool);
      ASSERT(MEM_GetBufferSize(bufPool) == B_Sizeof());
   }
   if( datPool != NULLP)
      MEM_ChkPool(datPool);
#endif

   XX_Freeze();
#if FRMOBJ_FREELIST
   if(framesPool == defFramesPool)
   {
     fr_LL_Get(frame);
     if( !frame )
     {
        /* Try to allocate the frame structure */
        if( (frame = (t_Frame *)MEM_Get(framesPool, F_Sizeof())) == NULLP)
        {
          XX_Thaw();
          return NULLP;
        }
     }
   }
   else
#endif
   /* Try to allocate the frame structure */
   if( (frame = MEM_Get(framesPool, F_Sizeof())) == NULLP)
   {
     XX_Thaw();
     return NULLP;
   }

   memset(frame, 0, F_Sizeof());
   frame->bufPool    = bufPool;
   frame->datPool    = datPool;
   frame->framesPool = framesPool;
   frame->useCount   = 1;

   if(buf)
   {
      ASSERT(buf->status == XX_STAT_Valid);
      buf->prev = buf->next = NULLP;
      frame->firstBuf = buf;
      frame->frameLen = B_GetLength(buf);
   }

#ifdef ERRCHK
   frame->status = XX_STAT_Valid;
#endif

#ifdef FRMCHK
   frame->allocFile = file;
   frame->allocLine = line;
   XX_AddToDLList(frame, FrameList);
#endif
   XX_Thaw();
   return(void *)frame;
}

/* Reinitiate frame with releasing all it's buffers */
e_Err F_Init(void *p_Frame)
{
   t_Buffer *buffer;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame && frame->status == XX_STAT_Valid);
   if(frame->useCount > 1)
      return E_FAILED;

   /*Release all buffers except the first one*/
   if(frame->firstBuf != NULLP)
   {
      for(buffer=frame->firstBuf->next; buffer;)
      {
       t_Buffer *buf = buffer;
       buffer = buf->next;
       B_Delete(buf, TRUE);
      }
      frame->firstBuf->next = NULLP;
      frame->firstBuf->fFull = FALSE;
      frame->firstBuf->head = frame->firstBuf->tail = 0;
   }
   frame->frameLen = 0;
   return E_OK;
}



/* Delete a frame */
#ifdef FRMCHK
void _F_DeleteExt(void *p_Frame, Bool delDataBuf, const char *file, word line)
#else
void F_DeleteExt(void *p_Frame, Bool delDataBuf)
#endif
{
   t_Buffer *buffer;
   t_Frame *frame = (t_Frame *)p_Frame;

   XX_Freeze();

   /* If the frame does not exist or if it has already been
      freed return gracefully */
   if((!frame) || (frame->status != XX_STAT_Valid))
   {
     XX_Thaw();
     return;
   }

   if(--frame->useCount == 0)
   {
      if(frame->f_ReleaseInd != NULLP)
          frame->f_ReleaseInd(frame->relParam);

      /*Release all buffers*/
      for(buffer=frame->firstBuf; buffer;)
      {
         t_Buffer *buf = buffer;
         buffer = buf->next;
         B_Delete(buf, delDataBuf);
      }

#ifdef ERRCHK
      frame->status = XX_STAT_Invalid;
#endif

#ifdef FRMCHK
      frame->freeFile = file;
      frame->freeLine = line;
      XX_DelFromDLList(frame, FrameList);
#endif

#if FRMOBJ_FREELIST
      if (frame->framesPool == defFramesPool)
         fr_LL_Put(frame)
      else
#endif
      MEM_Put(frame->framesPool, frame);
   }

   XX_Thaw();
}

/*Get a pointer to the buffer */
void   *F_GetBuffer( void *p_Frame, word BuffNum, Bool *pLast )
{
   t_Buffer *buffer;
   word i;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame && frame->status == XX_STAT_Valid);
   buffer = frame->firstBuf;

   for(i=0; buffer && i!=BuffNum; i++)
      buffer = buffer->next;

   if(i!=BuffNum || !buffer)
      return NULLP;

   *pLast = (buffer->next == NULLP);
   return buffer;
}

/*Get a pointer to the first buffer and remove it from the frame*/
void   *F_RemoveFirstBuffer( void *p_Frame)
{
   t_Buffer *buffer;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame && frame->status == XX_STAT_Valid);
   if((buffer = frame->firstBuf) != NULLP)
   {
      if ((frame->firstBuf = buffer->next) != NULLP)
        frame->firstBuf->prev = NULLP;
     frame->frameLen-=B_GetLength(buffer);
   }
   return buffer;
}

/*Put the buffer to the end of the frame*/
void    F_PutBuffer( void *p_Frame, void *p_Buff )
{
   t_Buffer *buf;
   t_Buffer *buffer = (t_Buffer *)p_Buff;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame && frame->status == XX_STAT_Valid);
   ASSERT(buffer && buffer->status == XX_STAT_Valid);
   buffer->next = NULLP;
   if((buf = frame->firstBuf) == NULLP)
   {
      buffer->prev = NULLP;
      frame->firstBuf = buffer;
      frame->frameLen += B_GetLength(buffer);
      return;
   }

   while(buf->next)
      buf = buf->next;
   buf->next = buffer;
   buffer->prev = buf;
   /* change frame length */
   frame->frameLen += B_GetLength(buffer);
}

/*Put the buffer to the start of the frame*/
void    F_PutToStartBuffer( void *p_Frame, void *p_Buff )
{
   t_Buffer *buffer = (t_Buffer *)p_Buff;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame && frame->status == XX_STAT_Valid);
   ASSERT(buffer && buffer->status == XX_STAT_Valid);
   buffer->prev = NULLP;
   buffer->next = frame->firstBuf;
   if(frame->firstBuf != NULLP)
     frame->firstBuf->prev = buffer;
   frame->firstBuf = buffer;
   /* change frame length */
   frame->frameLen += B_GetLength(buffer);
}

/* Add array to the start of the frame */
word    F_AddToStart( void *p_Frame, byte *pData, word Length )
{
   t_Buffer *buffer;
   word      len      = 0;
   byte      useCount = 1;
   t_Frame  *frame    = (t_Frame *)p_Frame;
#if B_OFFSET > 0
   word      boffset;   /* Buffer offset for non-first frame buffer */
#endif

   ASSERT(frame && frame->status == XX_STAT_Valid);
   ASSERT(Length);

   if((buffer = frame->firstBuf) != NULLP)
   {
      /* Not empty frame */
      useCount = buffer->useCount;
      len = B_GetSize(buffer) - B_GetLength(buffer);
      if ( len != 0 )
      {
         if( len >= Length )
         {
            /* first buffer has enough space to prepend data */
            frame->frameLen += Length;
#ifdef ERRCHK
            B_AddToStart( buffer, pData, Length );
#else
            B_ADDTOSTART( buffer, pData, Length);
#endif
            return Length;
         }
         else
         {
#ifdef ERRCHK
            B_AddToStart( buffer, pData ? pData+Length-len : NULLP, len );
#else
            B_ADDTOSTART( buffer, (pData ? pData+Length-len : NULLP), len );
#endif
            Length -= len;
         }
      }
#if B_OFFSET > 0
      boffset = 0;
   }
   else
      boffset = B_OFFSET;
#else
   }
#endif

   /* Allocate new buffers and put data to them */
   while(Length)
   {
      word l;
      word bs;

      if( (buffer = (t_Buffer *)B_NewEx(frame->bufPool,  frame->datPool, B_BUFFER_SIZE)) == NULLP)
         return len;

      buffer->useCount = useCount;
#if B_OFFSET > 0
      /* Set buffer offset =0 if the buffer is not the first in the frame */
      B_SetOffset( buffer, boffset );
      bs = B_GetSize(buffer) - boffset;
      boffset = 0;
#else
      bs = B_GetSize(buffer);
#endif
      if(Length <= bs)
         l = Length;
      else
      {
         l = bs;
      }
#ifdef ERRCHK
      B_AddToStart( buffer, pData ? pData + Length - l : NULLP, l );
#else
      B_ADDTOSTART( buffer, (pData ? pData + Length -l : NULLP), l );
#endif
      len += l;
      Length -= l;
      if((buffer->next = frame->firstBuf) != NULLP)
         frame->firstBuf->prev = buffer;
      frame->firstBuf = buffer;
   }
   frame->frameLen += len;
   return len;
}

/* Add 1 byte to the start of the frame */
word    F_AddByteToStart( void *p_Frame, byte data)
{
   t_Buffer *buffer, **p;
   t_Frame  *frame    = (t_Frame *)p_Frame;
   byte      useCount = 1;

   ASSERT(frame && frame->status == XX_STAT_Valid);

   p = &frame->firstBuf;
   if((buffer = frame->firstBuf) != NULLP)
   {
      /* Not empty frame */
      useCount = buffer->useCount;
      if(B_GetSize(buffer) > (word)B_GetLength(buffer))
      {
         /* first buffer has enough space to prepend data */
         frame->frameLen++;
         B_AddByteToStart( buffer, data );
         return TRUE;
      }
      p = &buffer->prev;
   }

   /* Allocate a new buffer and put data into it */
   if( (buffer = (t_Buffer *)B_NewEx(frame->bufPool,  frame->datPool, B_BUFFER_SIZE)) == NULLP)
      return 0;

   buffer->next = frame->firstBuf;
   *p = buffer; /* where (*p) is (firstBuf==NULL) ? firstBuf : firstBuf->prev */
   frame->firstBuf = buffer;
   buffer->useCount = useCount;
   frame->frameLen++;
   B_AddByteToStart( buffer, data );
   return TRUE;
}

/* Add array to the end of the frame */
word   F_AddToEnd( void *p_Frame, byte *pData, word Length )
{
   t_Buffer *buffer, **p;
   word      len = 0;
   byte      useCount = 1;
   t_Frame  *frame = (t_Frame *)p_Frame;
#if B_OFFSET > 0
   word      boffset;   /* Buffer offset for non-first frame buffer */
#endif

   ASSERT(frame && frame->status == XX_STAT_Valid);
   ASSERT(Length);
   p = &frame->firstBuf;

   /* look for the last buffer */
   for ( buffer = frame->firstBuf; buffer; buffer = buffer->next )
   {
      p = &buffer->next;
      if(!buffer->next)
         break;
   }

   if(buffer)
   {
      /* frame is not empty */
      useCount = buffer->useCount;
      if((len = B_GetSize(buffer) - B_GetLength(buffer)) >= Length)
      {
         /* the buffer has enough space to pospend data */
         frame->frameLen += Length;
#ifdef ERRCHK
         B_AddToEnd( buffer, pData, Length);
#else
         B_ADDTOEND( buffer, pData, Length);
#endif
         return Length;
      }
      else if(len)
      {
#ifdef ERRCHK
         B_AddToEnd( buffer, pData, len );
#else
         B_ADDTOEND( buffer, pData, len );
#endif
         Length -= len;
      }
#if B_OFFSET > 0
      boffset = 0;
   }
   else
      boffset = B_OFFSET;
#else
   }
#endif

   /* Allocate new buffers and put data to them */
   while(Length)
   {
      word      l;
      word      bs;
      t_Buffer *buf;

      if( (buf = (t_Buffer *)B_NewEx(frame->bufPool,  frame->datPool, B_BUFFER_SIZE)) == NULLP)
         return len;

      buf->useCount = useCount;
#if B_OFFSET > 0
      /* Set buffer offset =0 if the buffer is not the first in the frame */
      B_SetOffset( buf, boffset );
      bs = B_GetSize(buf) - boffset;
      boffset = 0;
#else
      bs = B_GetSize(buf);
#endif
      if(Length <= bs)
         l = Length;
      else
         l = bs;

#ifdef ERRCHK
      B_AddToEnd( buf, pData ? pData + len : NULLP, l);
#else
      B_ADDTOEND( buf, (pData ? pData + len : NULLP), l);
#endif
      len += l;
      Length -= l;
      *p = buf;
      buf->prev = buffer;
      p = &buf->next;
      buffer = buf;
   }

   frame->frameLen += len;
   return len;
}

/* Add 1 byte to the end of the frame */
word    F_AddByteToEnd( void *p_Frame, byte data)
{
   t_Buffer *buffer, *buf, **p;
   byte      useCount = 1;
   t_Frame  *frame    = (t_Frame *)p_Frame;

   ASSERT(frame && frame->status == XX_STAT_Valid);
   p = &frame->firstBuf;

   /* look for the last buffer */
   for(buffer = frame->firstBuf; buffer; buffer = buffer->next)
   {
      p= &buffer->next ;
      if( buffer->next == NULLP )
         break;
   }

   if(buffer != NULLP)
   {
      /* Not empty frame */
      useCount = buffer->useCount;
      if(B_GetSize(buffer)>(word)B_GetLength(buffer))
      {
         /* last buffer has enough space to pospend data */
         frame->frameLen++;
         B_AddByteToEnd( buffer, data );
         return TRUE;
      }
   }

   /* Try allocate a new buffer and put data into it */
   if( (buf = (t_Buffer *)B_NewEx(frame->bufPool,  frame->datPool, B_BUFFER_SIZE)) == NULLP)
      return 0;

   buf->useCount = useCount;
   *p = buf;
   frame->frameLen++;
   buf->prev = buffer;
   B_AddByteToEnd( buf, data );
   return TRUE;
}


/* Remove data from the start of the frame */
word    F_CutFromStart( void *p_Frame, word Length )
{
   t_Buffer *buffer;
   word      len   = 0;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame && frame->status == XX_STAT_Valid);
   ASSERT(Length);

   for(buffer =frame->firstBuf; buffer && (Length != 0);)
   {
      t_Buffer *buf = buffer;
      word buflen   = B_GetLength(buf);

      buffer = buf->next;
      if(Length < buflen)
      {
#ifdef ERRCHK
         B_CutFromStart(buf, Length);
#else
         B_CUTFROMSTART(buf, Length);
#endif
         frame->frameLen -= len + Length;
         return len + Length;
      }

      /* release whole buffer */
      if(buffer)
      {
         if((buffer->prev = buf->prev) != NULLP)
            buf->prev->next = buffer;
         else
            frame->firstBuf = buffer;
      }
      else
         frame->firstBuf = NULLP;

      Length -= buflen;
      len +=  buflen;
      B_Delete(buf, TRUE);
   }

   frame->frameLen -= len;
   return len;
}


/* Remove data from the end of the frame */
word    F_CutFromEnd( void *p_Frame, word Length )
{
   t_Buffer *buffer;
   word      len   = 0;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame->status == XX_STAT_Valid);

   /* Find the last buffer */
   for(buffer = frame->firstBuf; buffer && buffer->next; buffer = buffer->next)
      ;

   while(buffer && Length)
   {
      t_Buffer *buf = buffer;
      word buflen = B_GetLength(buf);

      if(Length < buflen)
      {
#ifdef ERRCHK
         B_CutFromEnd(buf, Length);
#else
         B_CUTFROMEND(buf, Length);
#endif
         frame->frameLen -= len + Length;
         return len + Length;
      }

      /* release whole buffer */
      if((buffer = buf->prev) == NULLP)
         frame->firstBuf = NULLP;
      else
      {
         if((buffer->next = buf->next) != NULLP)
            buf->next->prev = buffer;
      }

      Length -= buflen;
      len +=  buflen;
      B_Delete(buf, TRUE);
   }

   frame->frameLen -= len;
   return len;
}

/* Remove data begining from the offset */
word    F_CutSlice( void *p_Frame, word offset, word Length )
{
   t_Buffer *buffer, *buf;
   word len    = 0;
   word offs   = 0;
   word buflen = 0;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame->status == XX_STAT_Valid);

   /* find the offset */
   for(buffer = frame->firstBuf; buffer ; buffer = buffer->next)
   {
      buflen = B_GetLength(buffer);
      if((word)(offs + buflen) > offset)
         break;
      offs += buflen;
   }

   if(buffer == NULL)
       return 0;

   if(buflen-(offset-offs)>=Length)
   {
      /* the whole slice is within the buffer */
      B_CutSlice(buffer,(word)(offset - offs),Length);
      frame->frameLen -= Length;
      return Length;
   }

   B_CutFromEnd(buffer, (word)(buflen-(offset-offs)));
   len += buflen-(offset-offs);
   Length-= buflen-(offset-offs);

   for(buffer = buffer->next; buffer;)
   {
      buf = buffer;
      buflen = B_GetLength(buf);
      if(Length<buflen)
         break;

      /*remove the whole buffer */
      if((buffer = buf->next) != NULLP)
         buffer->prev = buf->prev;

      buf->prev->next = buffer;

      Length-=buflen;
      len += buflen;
      B_Delete(buf, TRUE);
   }

   if(buffer && Length)
   {
      /* remove remains of the slice */
      B_CutFromStart(buffer, Length);
      len += Length;
   }
   frame->frameLen -= len;
   return len;
}

/* Read to array beginning from the offset */
word    F_Read( void *p, void *p_Frame, word offset, word Length )
{
   t_Buffer *buffer;
   word len    = 0;
   word offs   = 0;
   word buflen = 0;
   t_Frame *frame = (t_Frame *)p_Frame;
   ASSERT(frame->status == XX_STAT_Valid);

   /* find the offset */
   for( buffer = frame->firstBuf; ; buffer = buffer->next)
   {
      /* Check if the frame is empty or we are out of frame */
      if( !buffer )
         return 0;

      buflen = B_GetLength(buffer);
      if((word)(offs + buflen) > offset)
         break;
      offs += buflen;
   }

   if((word)(buflen-(offset-offs))>=Length)
   {
      /*the whole peace is within the buffer */
#ifdef ERRCHK
      B_Read(p, buffer,(word)(offset-offs),Length);
#else
      B_READ(p, buffer,offset-offs,Length);
#endif
      return Length;
   }

#ifdef ERRCHK
   B_Read(p, buffer,(word)(offset-offs),(word)(buflen-(offset-offs)));
#else
   B_READ(p, buffer,offset-offs,buflen-(offset-offs));
#endif
   p=(void *)((byte *)p + buflen-(offset-offs));
   len += buflen-(offset-offs);
   Length -= buflen-(offset-offs);

   for(buffer = buffer->next; buffer; buffer = buffer->next)
   {
      word l;
      buflen = B_GetLength(buffer);
      l = Length<buflen ? Length : buflen;
#ifdef ERRCHK
      B_Read(p, buffer, 0, l);
#else
      B_READ(p, buffer, 0, l);
#endif
      p=(void *)((byte *)p + l);
      len += l;
      if((Length -= l) == 0)
         break;
   }
   return len;
}

/* Overwrite data in frame beginning from the offset from array  */
word    F_Write( void *p, void *p_Frame, word offset, word Length )
{
   t_Buffer *buffer;
   word      len    = 0;
   word      offs   = 0;
   word      buflen = 0;
   t_Frame  *frame  = (t_Frame *)p_Frame;

   ASSERT(frame->status == XX_STAT_Valid);

   /* find the offset */
   for(buffer = frame->firstBuf; buffer ; buffer = buffer->next)
   {
      buflen = B_GetLength(buffer);
      if((word)(offs + buflen) > offset)
         break;
      offs += buflen;
   }

   if(buffer == NULL)
       return 0;

   if((word)(buflen-(offset-offs))>=Length)
   {
      /*the whole peace is within the buffer */
#ifdef ERRCHK
      B_Write(p, buffer,(word)(offset-offs),Length);
#else
      B_WRITE(p, buffer,offset-offs,Length);
#endif
      return Length;
   }

#ifdef ERRCHK
   B_Write(p, buffer,(word)(offset-offs),(word)(buflen-(offset-offs)));
#else
   B_WRITE(p, buffer,offset-offs,buflen-(offset-offs));
#endif
   p=(void *)((byte *)p + buflen-(offset-offs));
   len += buflen-(offset-offs);
   Length -= buflen-(offset-offs);

   for(buffer = buffer->next; buffer; buffer = buffer->next)
   {
      word l;
      buflen = B_GetLength(buffer);
      l = Length<buflen ? Length : buflen;
#ifdef ERRCHK
      B_Write(p, buffer, 0, l);
#else
      B_WRITE(p, buffer, 0, l);
#endif
      p=(void *)((byte *)p + l);
      len += l;
      if((Length -= l) == 0)
         break;
   }
   return len;
}

/* Read array from start and remove from the frame */
word    F_ReadStartRemove( void *p, void *pFrame, word Length )
{
   t_Buffer *buffer;
   word      len = 0;
   word      buflen;
   t_Frame  *frame = (t_Frame *)pFrame;

   ASSERT(frame->status == XX_STAT_Valid);
   ASSERT(p);

   for(buffer = frame->firstBuf; buffer;)
   {
      t_Buffer *buf = buffer;
      word l;

      buflen = B_GetLength(buf);
      l = buflen>Length ? Length : buflen;
#ifdef ERRCHK
      B_ReadStartRemove(p, buf, l);
#else
      B_READSTARTREMOVE(p, buf, l);
#endif
      len += l;
      if(buflen == l)
      {
          /*Remove the buffer */
          if((buffer = buf->next) != NULLP)
              buffer->prev = NULLP;
          frame->firstBuf = buffer;
          B_Delete(buf, TRUE);
      }
      if((Length -= l) == 0)
         break;

      p=(void *)((byte *)p + l);

   }
   frame->frameLen -= len;
   return len;
}

/* Read array from end and remove from the frame */
word    F_ReadEndRemove( void *p, void *pFrame, word Length )
{
   t_Buffer *buffer, *buf;
   word      len = 0;
   word      buflen = 0;
   word      offs= 0;
   word      frame_offs;
   t_Frame  *frame = (t_Frame *)pFrame;

   ASSERT(frame->status == XX_STAT_Valid);
   ASSERT(p);

   /* Look for the first relevant buffer */
   if (Length >= frame->frameLen)
      frame_offs = 0;
   else
      frame_offs = frame->frameLen - Length;

   for (buffer = frame->firstBuf; buffer; buffer = buffer->next)
   {
       buflen = B_GetLength(buffer);
       if(offs + buflen > frame_offs)
          break;
       offs += buflen;
   }

   /* Now we should read & remove the tail of the first relevant buffer
     and then read & remove all the following buffers.
   */
   if (buffer)
   {
      word l;

      l = buflen - (frame_offs - offs);
#ifdef ERRCHK
       B_ReadEndRemove(p, buffer, l);
#else
       B_READENDREMOVE(p, buffer, l);
#endif
      len += l;

      p = (void *)((byte *)p + l);

      /* Detach the remaining buffers from the frame */
      buf = buffer->next;
      buffer->next = NULLP;
      buffer = buf;

      /* Read and remove the remaining buffers from the frame */
      while(buffer)
      {

         l = B_GetLength(buffer);

#ifdef ERRCHK
         B_Read(p, buffer, 0, l);
#else
         B_READ(p, buffer, 0, l);
#endif
         len += l;
         p = (void *)((byte *)p + l);

         /* Release the buffer */
         buf = buffer->next;
         B_Delete(buffer, TRUE);
         buffer = buf;
      }
   }
   frame->frameLen -= len;
   return len;
}

/* Read one byte from the frame */
word    F_Peek( byte *p, void *p_Frame, word offset )
{
   t_Buffer *buffer;
   word      offs = 0;
   word      buflen = 0;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame->status == XX_STAT_Valid);
   ASSERT(p);

   if(offset >= frame->frameLen)
      return 0;

   /* find the offset */
   for(buffer = frame->firstBuf; buffer; buffer = buffer->next)
   {
      buflen = B_GetLength(buffer);
      if((word)(offs + buflen) > offset)
         break;
      offs += buflen;
   }

   if(buffer)
       return B_Peek(p, buffer, offset - offs);
   else
       return 0;
}

/* Replace one byte in the frame */
word    F_Replace( byte data, void *p_Frame, word offset )
{
   t_Buffer *buffer;
   word      offs = 0;
   word      buflen;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame->status == XX_STAT_Valid);

   /* find the offset */
   for(buffer = frame->firstBuf; buffer; buffer = buffer->next)
   {
      buflen = B_GetLength(buffer);
      if((word)(offs + buflen) > offset)
         break;
      offs += buflen;
   }

   if(buffer)
       return B_Replace(data, buffer, offset - offs);
   else
       return 0;
}

/* Prepend frame 1 to frame 2 and discard frame 1 */
void *F_PrependAndDisc1(void *p_F1, void *p_F2)
{
   t_Frame  *frame2;
   t_Buffer *buffer, *p, **n;
   t_Frame  *frame1 = (t_Frame *)p_F1;

   ASSERT(frame1 && frame1->status == XX_STAT_Valid);
   frame2 = (t_Frame *) p_F2;
   ASSERT(frame2 && frame2->status == XX_STAT_Valid);
   ASSERT(frame2 != frame1);

   p = frame1->firstBuf;
   n = &frame1->firstBuf;

   /* Look for the last buffer of the first message */
   for(buffer = frame1->firstBuf; buffer; buffer = buffer->next)
   {
      n = &buffer->next;
      p = buffer;
   }

   /* Move all buffers from frame2 to frame1 */
   if((*n = frame2->firstBuf) != NULLP)
   {
      if(p != frame1->firstBuf)
         (*n)->prev = p;
      else
         (*n)->prev = NULLP;
   }

   frame2->frameLen += frame1->frameLen;
   frame2->firstBuf = frame1->firstBuf;
   frame1->frameLen = 0;
   frame1->firstBuf = NULLP;
   F_Delete(frame1);
   return frame2;
}

/* Concatenate frame 2 to frame 1 */
void *F_Concatenate(void *p_F1, void *p_F2)
{
   t_Frame  *frame2;
   t_Buffer *buffer, *p, **n;
   t_Frame  *frame1 = (t_Frame *)p_F1;

   ASSERT(frame1 && frame1->status == XX_STAT_Valid);
   frame2 = (t_Frame *) F_Copy(p_F2);
   ASSERT(frame2 && frame2->status == XX_STAT_Valid);
   ASSERT(frame2 != frame1);

   p = frame1->firstBuf;
   n = &frame1->firstBuf;

   /*Look for the last buffer of the first message */
   for(buffer = frame1->firstBuf; buffer; buffer = buffer->next)
   {
      n = &buffer->next;
      p = buffer;
   }

   /* Move all buffers from frame2 to frame1 */
   if((*n = frame2->firstBuf) != NULLP)
   {
      (*n)->prev = p;
   }
   frame1->frameLen += frame2->frameLen;
   frame2->frameLen = 0;
   frame2->firstBuf = NULLP;
   F_Delete(frame2);
   return frame1;
}

/* Concatenate frame 2 to frame 1 and discard frame 2 */
void* F_ConcAndDisc(void *p_F1, void *p_F2)
{
   t_Frame  *frame2;
   t_Buffer *buffer, *p, **n;
   t_Frame  *frame1 = (t_Frame *)p_F1;

   ASSERT(frame1 && frame1->status == XX_STAT_Valid);
   frame2 = (t_Frame *) p_F2;
   ASSERT(frame2 && frame2->status == XX_STAT_Valid);
   ASSERT(frame2 != frame1);

   p = frame1->firstBuf;
   n = &frame1->firstBuf;

   /* Look for the last buffer of the first message */
   for(buffer = frame1->firstBuf; buffer; buffer = buffer->next)
   {
      n = &buffer->next;
      p = buffer;
   }

   /* Move all buffers from frame2 to frame1 */
   if((*n = frame2->firstBuf) != NULLP)
   {
      (*n)->prev = p;
   }
   frame1->frameLen += frame2->frameLen;
   frame2->frameLen = 0;
   frame2->firstBuf = NULLP;
   F_Delete(frame2);
   return frame1;
}

/* Split frame from start */
void *F_SplitFromStart(void *p_F, word length)
{
    t_Frame  *newFrame;
    t_Frame  *p_Frame = (t_Frame *)p_F;
    t_Buffer *buffer;
    t_Buffer *newBuffer;

    ASSERT(p_Frame && p_Frame->status == XX_STAT_Valid);
    if(length >= p_Frame->frameLen)
        return NULLP;

#if FRMOBJ_FREELIST
   if(p_Frame->framesPool == defFramesPool)
   {
      fr_LL_Get(newFrame);
      if( !newFrame )
      {
         /* Try to allocate the frame structure */
         if( (newFrame = (t_Frame *)MEM_Get(p_Frame->framesPool, F_Sizeof())) == NULLP)
           return NULLP;
      }
   }
   else
#endif
   /* Try to allocate new frame structure from the same pool */
   if( (newFrame = MEM_Get(p_Frame->framesPool, sizeof(t_Frame))) == NULLP)
      return NULLP;

   memset(newFrame, 0, sizeof(t_Frame));

   /* Copy the frame header */
   newFrame->bufPool    = p_Frame->bufPool;
   newFrame->datPool    = p_Frame->datPool;
   newFrame->framesPool = p_Frame->framesPool ;
   newFrame->useCount   = 1;
#ifdef ERRCHK
   newFrame->status = XX_STAT_Valid;
#endif

   for(buffer = p_Frame->firstBuf; length; buffer = buffer->next)
   {
       word      bufLen = 0;

       bufLen = B_GetLength(buffer);
       if(length >= bufLen)
       {
           if(!newFrame->firstBuf)
               newFrame->firstBuf = buffer;
           length -= bufLen;
           p_Frame->frameLen -= bufLen;
           newFrame->frameLen += bufLen;
           p_Frame->firstBuf = buffer->next;

           if(!length)
           {
               if(buffer->next)
                   buffer->next->prev = NULLP;
               buffer->next = NULLP;
               return newFrame;
           }
       }
       else
       {
           /* Allocate a new buffer, make it the first buffer of the old frame
           */
           newBuffer = B_NewEx(buffer->bufPool, buffer->datPool, buffer->size);
           if(!newBuffer)
           {
               /* Check if the old frame has been modified */
               if(newFrame->firstBuf)
               {
                   t_Buffer *bf = buffer->prev;
                   ASSERT(bf);
                   if(bf->next)
                       bf->next->prev = NULLP;
                   bf->next = NULLP;
                   return newFrame;
               }
               newFrame->firstBuf = NULLP;
               F_Delete(newFrame);
               return NULLP;
           }
           B_Copy(newBuffer, buffer);

           /* Work out pointers first */
           p_Frame->firstBuf = newBuffer;
           newBuffer->prev = NULLP;
           newBuffer->next = buffer->next;
           if ( buffer->next )
              buffer->next->prev = newBuffer;
           B_CutFromStart(newBuffer, length);
           p_Frame->frameLen -= length;
           newFrame->frameLen += length;

           B_CutFromEnd(buffer, (word)(bufLen - length));
           buffer->next = NULLP;
           if(!newFrame->firstBuf)
              newFrame->firstBuf = buffer;

           return newFrame;
       }
   }
   ASSERT(0);
   return NULLP;
}


/* Create a copy of the frame */
#ifdef FRMCHK
void *_F_Copy(void *p_Frame, const char *file, word line)
#else
void *F_Copy(void *p_Frame)
#endif
{
   t_Frame  *cpyframe;
   t_Buffer *buf,*cpybuf, **p_buf, *prevbuf;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame && frame->status == XX_STAT_Valid);

#if FRMOBJ_FREELIST
   if(frame->framesPool == defFramesPool)
   {
      fr_LL_Get(cpyframe);
      if( !cpyframe )
      {
         /* Try to allocate the frame structure */
         if( (cpyframe = (t_Frame *)MEM_Get(frame->framesPool, F_Sizeof())) == NULLP)
           return NULLP;
      }
   }
   else
#endif
   /* Try to allocate new frame structure from the same pool */
   if( (cpyframe = MEM_Get(frame->framesPool, sizeof(t_Frame))) == NULLP)
      return NULLP;

   memset(cpyframe, 0, sizeof(t_Frame));

   /* Copy the frame header */
   cpyframe->bufPool    = frame->bufPool;
   cpyframe->datPool    = frame->datPool;
   cpyframe->framesPool = frame->framesPool ;
   cpyframe->useCount   = 1;
#ifdef ERRCHK
   cpyframe->status = XX_STAT_Valid;
#endif

   /* Copy the buffers */
   p_buf = &cpyframe->firstBuf;
   prevbuf = NULLP;
   for (buf = frame->firstBuf;
        buf &&  cpyframe->frameLen < frame->frameLen;
        buf = buf->next)
   {
      /* Try to allocate a buffer */
      if( (cpybuf =
         (t_Buffer *)B_NewEx(cpyframe->bufPool, cpyframe->datPool, B_GetSize(buf))) == NULLP)
      {
         /* Buffer allocation failed - release new frame */
         F_Delete(cpyframe);
         return NULLP;
      }

#ifdef ERRCHK
      B_Copy(cpybuf, buf);
#else
      B_COPY(cpybuf, buf);
#endif
      *p_buf = cpybuf;
      cpybuf->prev = prevbuf;
      prevbuf = cpybuf;
      p_buf = &cpybuf->next;
      cpyframe->frameLen += B_GetLength(buf);
   }

#ifdef FRMCHK
   cpyframe->allocFile = file;
   cpyframe->allocLine = line;
   XX_AddToDLList(cpyframe, FrameList);
#endif
   cpyframe->frameLen = frame->frameLen;
   return (void *)cpyframe;
}

/* Create a clone of the frame */
#ifdef FRMCHK
void *_F_Clone(void *p_Frame, const char *file, word line)
#else
void *F_Clone(void *p_Frame)
#endif
{
   t_Frame  *cpyframe;
   t_Buffer *buf;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame && frame->status == XX_STAT_Valid);

#if FRMOBJ_FREELIST
   if(frame->framesPool == defFramesPool)
   {
      fr_LL_Get(cpyframe);
      if( !cpyframe )
      {
        /* Try to allocate the frame structure */
        if( (cpyframe = (t_Frame *)MEM_Get(frame->framesPool, F_Sizeof())) == NULLP)
          return NULLP;
      }
   }
   else
#endif
   /* Try to allocate new frame structure from the same pool */
   if( (cpyframe = MEM_Get(frame->framesPool, sizeof(t_Frame))) == NULLP)
      return NULLP;

   memcpy(cpyframe, frame, sizeof(t_Frame));
   cpyframe->useCount = 1;
#ifdef FRMCHK
   cpyframe->allocFile = file;
   cpyframe->allocLine = line;
   XX_AddToDLList(cpyframe, FrameList);
#endif
   /* Make links to all buffers */
   for(buf = frame->firstBuf; buf; buf = buf->next)
      B_Link(buf);
   return (void *)cpyframe;
}

/* Return the number of buffers in the frame */
word    F_HowManyBuffs( void *p_Frame)
{
   t_Buffer *buf;
   word      n = 0;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame && frame->status == XX_STAT_Valid);

   for(buf = frame->firstBuf; buf; buf = buf->next)
      n++;
   return n;
}

/* Return the number of segments in the frame */
/* Each Big-Tail buffer with length > 0 is of one segment. */
/* Each non Big-Tail buffer with length > 0 is of two segments */
/* p_nBuffs gives number of buffers with length > 0 */
word    F_HowManySegs( void *p_Frame, word *p_nBuffs)
{
   t_Buffer *buf;
   word      n = 0;
   word      ns= 0;
   t_Frame  *frame = (t_Frame *)p_Frame;

   ASSERT(frame && frame->status == XX_STAT_Valid);

   for(buf = frame->firstBuf; buf; buf = buf->next)  {
      if (B_GetLength (buf) > 0)  {
         n++;
         if (B_BIG_TAIL (buf) )
            ++ns;
         else
            ns += 2;
      }
   }

   if (p_nBuffs)
      *p_nBuffs = n;

   return ns;

}

/* Return the number of bytes in the frame */
/* This function should not be redefined as a macro  */
/* because it was created to use as callback routine */
word    F_HowManyBytes( void *p_Frame)
{
#ifdef ERRCHK
   t_Frame  *frame = (t_Frame *)p_Frame;
#endif

   ASSERT(frame && frame->status == XX_STAT_Valid);
   return F_GetLength(p_Frame );
}



/* Pack frame starts from buffer p_Buff */
/* If p_Buff=NULL, pack the whole frame */
/* In the packed frame buffers are aligned to zero offset */
/* and all buffers but may be the last are fully filled */
/* Returns E_OK if success or */
/* E_NOMEMORY if B_ReWrap has failed */
e_Err F_Pack( void *p_Frame, void *p_Buff )
{
   t_Frame  *frame = (t_Frame *)p_Frame;
   t_Buffer *buf;
   t_Buffer *bufn;
   word      len_add;
   word      len_cut;

   ASSERT(frame && frame->status == XX_STAT_Valid);

   if (p_Buff == NULLP)
      buf = frame->firstBuf;
   else
      buf = (t_Buffer *)p_Buff;

   while( buf )
   {
      bufn = buf->next;

      /* Rewrap buffer with non-zero offset */
      if (buf->head)
      {
         if (B_ReWrap( buf ) == NULLP)
            return E_NOMEMORY;
      }
      /* Fill partially filled buffer */
      len_add = B_GetSize( buf ) - B_GetLength( buf );
      while ( len_add && bufn)
      {
         if ((len_cut=B_GetLength( bufn )) <= len_add)
         {
            B_ReadStartRemove( buf->data+buf->tail, bufn, len_cut );
            /* There is no more data in the buffer */
            /* It should be removed regardless on its usecount */
            buf->next = bufn->next;
            bufn->useCount = 1;
            B_Delete( bufn, TRUE );
            bufn = buf->next;
            if (bufn)
               bufn->prev = buf;
         }
         else
         {
            len_cut = len_add;
            B_ReadStartRemove( buf->data+buf->tail, bufn, len_add );
         }
         buf->tail += len_cut;
         len_add -= len_cut;
      }
      buf = bufn;
   }

   return E_OK;
}

/* Get pointer to the last buffer */
void   *F_GetLastBuf( void *p_Frame )
{
   register t_Buffer *buffer;
   t_Frame           *frame = (t_Frame *)p_Frame;

   ASSERT(frame->status == XX_STAT_Valid);

   /* Find the last buffer */
   buffer = frame->firstBuf;
   if (buffer)
   {
      while( buffer->next )
         buffer = buffer->next;
   }

   return buffer;
}

/* Convert frame to array extended. Returns also max array length */
byte *F_Frame2ArrayExt( void *p_Frame, word *p_maxArrayLength )
{
   t_Frame  *p_F = (t_Frame *)p_Frame;
   t_Buffer *p_B = p_F->firstBuf;

   if ( !p_B ||
        p_B->next ||
        !B_BIG_TAIL(p_B) )
      return NULLP;

   *p_maxArrayLength = B_GetSize( p_B ) - B_GetOffset( p_B );

   return B_GetData( p_B );
}

/* Set frame length. This function is useful when frame data   */
/* was modified outside frame interface. E.g., flat buffer     */
/* pointer can by obtained by using F_Frame2ArrayExt.          */
/* The function can be used only for single-buffer flat frames */
e_Err F_SetLength( void *p_Frame, word length )
{
   t_Frame  *p_F = (t_Frame *)p_Frame;
   t_Buffer *p_B = p_F->firstBuf;

   if ( !p_B ||
        p_B->next ||
        !B_BIG_TAIL(p_B) )
      return E_FAILED;
   B_SetLength( p_B, length );
   p_F->frameLen = length;
   return E_OK;
}


#ifdef FRMCHK
/* Display all frames currently allocated and by what line of code */
void  F_DisplayFrames( void )
{
   char    prntBuf[128];
   int     nFrames = 0;
   t_Frame *f;

   XX_Freeze();

   for (f = FrameList; f != NULL; f = f->next)
   {
      ++nFrames;
      sprintf(prntBuf, "     %p allocated by %s: %u", (void *)f, f->allocFile, f->allocLine);
      XX_Print(prntBuf);
   }

   sprintf(prntBuf, "Total of %u frames", nFrames);
   XX_Print(prntBuf);

   XX_Thaw();
}
#endif /* FRMCHK */



/* --- end of file frame.c --- */
