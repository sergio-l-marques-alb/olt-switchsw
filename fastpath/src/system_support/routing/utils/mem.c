/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename        mem.c
 *
 * @purpose         Memory pool management implementation
 *
 * @component       Routing Utils Component
 *
 * @comments
 *
 * @create          01/09/93
 *
 * @author          Jonathan Masel
 *                  Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\mem.c 1.1.3.1 2002/02/13 23:06:33EST ssaraswatula Development  $";
#endif


#include <string.h>

#include "std.h"
#include "local.h"

#include "xx.ext"
#include "mempool.h"
#include "mem.ext"

#include "log.h"

/* MEMORY POOL MANAGEMENT FUNCTIONS */

/* MEM data buffer header structure */
typedef struct tagt_DatBuffer
{
   struct tagt_DatBuffer *next;
   void    *p_data;
#ifdef ERRCHK
   t_Handle poolHndle;
#endif
} t_DatBuffer;

#define DATEXTRA sizeof(t_DatBuffer)


/* Pool list */
t_MemPoolInfo *poolList;

/* Initiate memory management */
void  MEM_InitAll( void )
{
   poolList = NULLP;
   OS_MEM_InitAll();
}

/* Shut down memory management */
void  MEM_ShutDown( void )
{
   t_MemPoolInfo *pool;
   for(pool = poolList; pool;)
   {
      t_MemPoolInfo *p = pool;
      pool = p->next;
      OS_MEM_Delete(p->osPoolHandle);
      XX_Free(p);
   }
   poolList = NULLP;
}

/* Create  a memory pool */
e_Err MEM_Init( const char Name[],
               t_Handle *p_Handle,
               void *startAddress,
               word bufferNmb,
               word bufferSize )
{
   return ( MEM_InitAlign( Name,
                           p_Handle,
                           startAddress,
                           bufferNmb,
                           bufferSize,
                           ALIGN ) );
}

/* Calculates size of data partition allocated by MEM_InitAlign */
ulng MEM_SizeofInitAlign(Bool staticPartition,
                         word bufferNmb,
                         word bufferSize,
                         word align)
{
   ulng  memsize = DATEXTRA;

   if(memsize <= align)
      memsize = align;
   else
      ALIGN_ADDRESS(memsize, align, ulng);

   memsize += bufferSize;
   ALIGN_ADDRESS(memsize, align, word);

   memsize *= bufferNmb;

   return OS_MEM_SizeofInitAlign(staticPartition,  memsize,  align);
}

/* Create aligned memory pool */
e_Err MEM_InitAlign( const char Name[],
                     t_Handle *p_Handle,
                     void *startAddress,
                     word bufferNmb,
                     word bufferSize,
                     word align)
{
   t_MemPoolInfo *pool;
   e_Err          rc;
   word           i;
   word           aligned_size;

   /* Allocate and create t_MemPoolInfo structure */
   if( !(pool = (t_MemPoolInfo *)XX_Malloc(sizeof(t_MemPoolInfo))) )
      return E_NOMEMORY;
   memset(pool, 0, sizeof(t_MemPoolInfo));
   if(Name)
      memcpy(&pool->Name, Name, sizeof(t_Name));
   pool->startAddress = startAddress;
   pool->bufferNmb    = pool->buffersFree = pool->minBuffersFree = bufferNmb;
   pool->bufferSize   = bufferSize;
   pool->align        = align;

   /* skip memory allocation for the empty buffers pool */
   if(!bufferSize || !bufferNmb)
       goto cont;
   if( (pool->dataOffset = DATEXTRA) <= align)
      pool->dataOffset = align;
   else
      ALIGN_ADDRESS(pool->dataOffset, align, word);

   aligned_size = pool->dataOffset + pool->bufferSize;
   ALIGN_ADDRESS(aligned_size, align, word);

   /* Initialize partition on the OS level */
   if( (rc = OS_MEM_InitAlign(startAddress,
            bufferNmb * aligned_size,
            align, &pool->osPoolHandle)) != E_OK)
   {
      ASSERT(TRUE);
      return rc;
   }

   /*Generate list of buffers */
   for(i = 0; i < bufferNmb; i++)
   {
      t_DatBuffer *b;

      if( !(b = (t_DatBuffer *)OS_GetMem(pool->osPoolHandle, aligned_size)) )
      {
         ASSERT(b);
         return E_NOMEMORY;
      }
#ifdef ERRCHK
      b->poolHndle = (t_Handle)pool;
#endif
      if(!pool->bufferList)
         b->next = NULLP;
      else
         b->next = pool->bufferList;
      b->p_data = (void *)((byte *)b + pool->dataOffset);
      pool->bufferList = b;
   }
cont:
#ifdef ERRCHK
   pool->status = XX_STAT_Valid;
#endif

   /* Add pool to the list of pools */
   XX_AddToDLList( pool, poolList );

   *p_Handle = (t_Handle)pool;
   return E_OK;
}


/* Return alignment of the data buffers in pool */
word MEM_PoolAlignment(t_Handle pool)
{
   return pool ? ((t_MemPoolInfo *)pool)->align : ALIGN;
}



void MEM_InitPool(t_Handle handle, ulng (*f_InitBuffer)(void*,ulng), ulng param)
{
   t_MemPoolInfo *pool;
   t_DatBuffer   *buffer;

   if(!handle)
      return;
   pool = (t_MemPoolInfo *)handle;
   ASSERT(pool && pool->status == XX_STAT_Valid);
   for(buffer = pool->bufferList; buffer; buffer = buffer->next)
      param = f_InitBuffer(buffer->p_data, param);
}

/* Allocate a buffer from the pool */
void *MEM_Get( t_Handle handle,
               word     size)
{
   t_MemPoolInfo *pool;
   t_DatBuffer   *buffer;

   if(!handle)
      return XX_MallocChunk(size);

   pool = (t_MemPoolInfo *)handle;
   ASSERT(pool && pool->status == XX_STAT_Valid);
   if(pool->bufferSize == 0)
       return NULLP;
   XX_Freeze();
   if(!pool->buffersFree)
   {
     L7_uchar8 msg[512];
      XX_Thaw();
      osapiSnprintf(msg, 512, "\nFatal error. Buffer pool %#x exhausted.", pool);
      XX_Print(msg);
      ASSERT(0);      /* print memory usage and die */
      return NULLP;   /* no free buffers */
   }

   pool->buffersFree --;
   if (pool->buffersFree < pool->minBuffersFree)
     pool->minBuffersFree = pool->buffersFree;
   buffer = pool->bufferList;
   pool->bufferList = buffer->next;
   XX_Thaw();

   if(pool->f_UsrCallback)
   {
      if((pool->buffersFree < pool->thresholdDown) && !pool->poolState)
      {
         pool->poolState = TRUE;
         pool->f_UsrCallback(pool->userId, TRUE);
      }
   }

   return buffer->p_data;
}

/* Allocate n buffers from the pool */
word  MEM_GetN( t_Handle handle,  word size,  word n, void *buffers[])
{
   word i;

   for(i = 0; i < n; i++)
      if( (buffers[i] = MEM_Get(handle,  size)) == NULLP)
         break;
   return i;
}


/* Return a buffer to the pool */
e_Err MEM_Put( t_Handle handle,  void *p_Block)
{
   t_MemPoolInfo *pool;
   t_DatBuffer   *buffer;

   if(!handle)
   {
      XX_Free(p_Block);
      return E_OK;
   }

   pool = (t_MemPoolInfo *)handle;
   ASSERT(pool && pool->status == XX_STAT_Valid);
   if(pool->bufferSize == 0)
       return E_OK;
   ASSERT(p_Block);
   buffer = (t_DatBuffer *)((byte *)p_Block - pool->dataOffset);

#ifdef ERRCHK
   ASSERT(buffer->poolHndle == handle);
#endif
   XX_Freeze();
   pool->buffersFree++;
   ASSERT(pool->buffersFree <= pool->bufferNmb);
   buffer->next = pool->bufferList;
   pool->bufferList = buffer;
   XX_Thaw();

   if(pool->f_UsrCallback)
   {
      if((pool->buffersFree >= pool->thresholdUp) && pool->poolState)
      {
         pool->poolState = FALSE;
         pool->f_UsrCallback(pool->userId, FALSE);
      }
   }

   if(!pool->markDel)
      return E_OK;

   /* pool is makred to be deleted */
   if(pool->bufferNmb == pool->buffersFree)
   {
     return MEM_Delete(pool);
   }
   return E_OK;
}

/* Check pool resources */
e_Err MEM_ChkResources(t_Handle handle, word *status)
{
   t_MemPoolInfo *pool;

   if((pool = (t_MemPoolInfo *)handle)==NULLP)
      return XX_ChkResources(status);

   *status = (word)(pool->buffersFree * 100 / pool->bufferNmb);
   return E_OK;
}


/* Delete a memory pool */
e_Err MEM_DeleteExt( t_Handle handle, Bool force )
{
   e_Err rc;
   t_MemPoolInfo *pool = (t_MemPoolInfo *)handle;
   ASSERT(pool && pool->status == XX_STAT_Valid);

   if(!force && (pool->bufferNmb != pool->buffersFree) )
   {
       /* not all the buffers was returned to the pool
          mark pool to be deleted
       */
       ASSERT(!pool->markDel);
       pool->markDel = TRUE;
       return E_BUSY;
   }

   /* Just delete partition on the OS level */
   if(pool->osPoolHandle)
   {
       rc = OS_MEM_Delete(pool->osPoolHandle);
       ASSERT(rc == E_OK);
   }

   /*Take the pool from the poolList */
   XX_DelFromDLList( pool, poolList );

#ifdef ERRCHK
   pool->status = XX_STAT_Invalid;
#endif
   XX_Free(pool);
   return E_OK;
}


/* Adjust buffer size in pool */
void MEM_AdjustPoolTrailer(t_Handle handle, word trailerSize)
{
   t_MemPoolInfo *pool = (t_MemPoolInfo *)handle;
   ASSERT(pool && pool->status == XX_STAT_Valid);

   pool->trailerSize = trailerSize;
    pool->bufferSize -= trailerSize;
}

/* Check pool handle */
void MEM_ChkPool(t_Handle handle)
{
   ASSERT( ((t_MemPoolInfo *)handle)->status == XX_STAT_Valid );
}

/* Bind pool empty event notification callback, Thresholds in percents */
e_Err MEM_BindPoolStateNotify(t_Handle PoolId, t_Handle UserId, word ThresholdDown, word ThresholdUp,
                              void (*f_UsrCallback)(t_Handle UserId, Bool ThreshCrossedDown))
{
   t_MemPoolInfo *pool = (t_MemPoolInfo *) PoolId;

   ASSERT(pool && pool->status == XX_STAT_Valid);
   ASSERT(ThresholdDown && ThresholdUp && f_UsrCallback);

   if(!ThresholdDown || !ThresholdUp || !f_UsrCallback)
      return E_FAILED;

   pool->thresholdDown = (pool->bufferNmb * ThresholdDown) / 100;
   pool->thresholdUp = (pool->bufferNmb * ThresholdUp) / 100;
   pool->userId = UserId;
   pool->f_UsrCallback = f_UsrCallback;

   return E_OK;
}

#if L7_CORE_CPU == MPC860
/* for the 860, optimize copy of n*4 bytes */
void mem4cpy( byte *to, byte *from, ulng count)
{
#ifdef __GNUC__
   __asm("  mfspr     6,9      ");        /* save counter */
   __asm("  mtspr     9,5      ");        /* set block counter */
   __asm("  lwzu      0,0(4)   ");        /* do 1 transaction out of loop */
   __asm("  stwu      0,0(3)   ");
   __asm("  bdz       Mexit    ");        /* exit if only single transaction */
   __asm("Mloop:               ");
   __asm("  lwzu      0,4(4)   ");        /* get data */
   __asm("  stwu      0,4(3)   ");        /* then store it */
   __asm("  bdnz      Mloop    ");
   __asm("Mexit:               ");
   __asm("  mtspr     9,6      ");
#else
   __asm("  mfspr     r6,9     ");        /* save counter */
   __asm("  mtspr     9,r5     ");        /* set block counter */
   __asm("  lwzu      r0,0(r4) ");        /* do 1 transaction out of loop */
   __asm("  stwu      r0,0(r3) ");
   __asm("  bdz       Mexit    ");        /* exit if only single transaction */
   __asm("Mloop:               ");
   __asm("  lwzu      r0,4(r4) ");        /* get data */
   __asm("  stwu      r0,4(r3) ");        /* then store it */
   __asm("  bdnz      Mloop    ");
   __asm("Mexit:               ");
   __asm("  mtspr     9,r6     ");
#endif
}
#endif

