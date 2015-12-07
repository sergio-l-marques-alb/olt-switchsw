/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   buff.c
*
* @purpose    File contains the buffer pool implementation.
*
* @component
*
* @comments
*   The buffer management code checks that buffers are
*   not corrupted and that the same buffer is not returned twice.
*   In general these types of errors are fatal to the system, so we
*   reset the box.
*
* @create     6/21/2001
*
* @author     atsigler
* @end
*
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commdefs.h"
#include "datatypes.h"
#include "buff_api.h"
#include "buff.h"
#include "log.h"
#include "osapi.h"

/* Static variables.
*/
static L7_uint32 NumBufferPools = 0; /* Number of created buffer pools */
static L7_uint32 MaxBufferPools = 0; /* Maximum number of pools that have
                                     ** ever been created.
                                     */
/* Array of buffer pool descriptors.
*/
static bufferPoolType BufferPoolList[L7_MAX_BUFFER_POOLS];

static void *BufferPoolLockSem = L7_NULL;

/*********************************************************************
* @purpose  Allocates and creates a buffer pool
*
*
* @param   num_buffers      - Number of buffers requested
* @param   buffer_size      - Size of buffers in the pool. The buffer size
*                             must be a multiple of four.
* @param   buffer_description - Text string up to 16 characters describing the
*                               buffer pool. The string is displayed by the
*                               bufferPoolShow command.
* @param   buffer_pool_id    (output) - Application uses this ID to reference the
*                            buffer pool.

* @returns  None
*
* @notes   This routine both allocates the memory for a buffer pool and creates
*          the buffer pool. It is preferred over bufferPoolCreate().
*
*          Function calls LOG_ERROR if the it detects memory corruption.
*
* @end
*********************************************************************/
L7_uint32 bufferPoolInit (L7_uint32 num_buffers,L7_uint32 buffer_size,
                     L7_char8  *description, L7_uint32 * buffer_pool_id)
{
  L7_RC_t rc;
  L7_uint32 pool_size;
  L7_uchar8 *pool;
  L7_uint32 buff_count;


  /* Determine how much memory we need for the desired number of buffers.
  */
  pool_size = bufferPoolSizeCompute (num_buffers, buffer_size);

  /* Allocate memory for the buffer pool.
  */
  pool = osapiMalloc (L7_SIM_COMPONENT_ID, pool_size);


  if (pool == L7_NULLPTR)
  {
    L7_LOG_ERROR(0); /* Out of memory */
  }

  /* Create the buffer pool.
  */
  rc = bufferPoolCreate (pool,
                         pool_size,
                         buffer_size,
                         description,
                         buffer_pool_id,
                         &buff_count);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  /* Since we use bufferPoolSizeCompute function to determine pool size, the
  ** number of buffers in the pool should be equal to the number of buffers we want.
  */
  if (buff_count != num_buffers)
  {
    L7_LOG_ERROR(buff_count);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Create a buffer pool.
*
* @param   buffer_pool_addr - Buffer pool memory address.
*                             This memory must be statically or dynamically
*                             allocated by the caller.
* @param   buffer_pool_size - Number of bytes in the buffer_pool_addr.
* @param   buffer_size - Size of buffers in the pool. The buffer size must be
*                        multiple of four.
* @param   buffer_description - Text string up to 16 characters describing the
*                               buffer pool. The string is displayed by the
*                               bufferPoolShow command.
* @param   buffer_pool_id    (output) - Application uses this ID to reference the
*                            buffer pool.
* @param   buffer_count      (output) - Number of buffers in the pool.
*
* @returns  L7_SUCCESS, if success
*
* @returns  L7_ERROR
*                 - Buffer memory size is too small for even one buffer.
* @returns  L7_FAILURE
*                 - Maximum number of buffer pools have already been created.
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bufferPoolCreate (void * buffer_pool_addr,
                          L7_uint32 buffer_pool_size,
                          L7_uint32 buffer_size,
                          L7_char8  * description,

                          L7_uint32 * buffer_pool_id,
                          L7_uint32 * buffer_count)
{
  L7_uint32 num_bufs;
  L7_uint32 tot_buf_size; /* Buffer size plus overhead */
  L7_uint32 pool_id;
  bufferPoolType *pool;
  L7_uint32 i;
  L7_uchar8 *user_data; /* Start of data buffer area */
  bufferDescrType *descr;
  L7_uint32 pool_addr;

  if (BufferPoolLockSem == L7_NULL)
  {
    if ((BufferPoolLockSem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == NULL)
    {
      L7_LOG_ERROR(L7_ERROR);
    }
  }

  /* If we reached the maximum number of buffer pools then return an
  ** error.
  */
  if (NumBufferPools >= L7_MAX_BUFFER_POOLS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "we reached the maximum number (%d) of buffer pools\n", NumBufferPools);
    return L7_FAILURE;
  }

  /* Make sure that buffer pool address is alligned on a four-byte
  ** boundary.
  */
  pool_addr = (L7_uint32) buffer_pool_addr;
  if (pool_addr != (pool_addr & 0xFFFFFFFC))
  {
    return  L7_ERROR;
  }

  /* Make sure that buffer size is a multiple of four.
  ** Round up the buffer size if necessary.
  */
  if (buffer_size != (buffer_size & 0xFFFFFFC))
  {
    buffer_size += 4;
    buffer_size &= 0xFFFFFFFC;
  }

  /* Zero out the buffer storage area.
  */
  memset (buffer_pool_addr, 0, buffer_pool_size);

  /* Compute how much memory is required for each buffer. The overhead
  ** includes buffer descriptor and a 4-byte pointer in the free list.
  */
  tot_buf_size = buffer_size +
                 sizeof (bufferDescrType) +
                 4;

  /* Determine how many buffer we can allocate.
  */
  num_bufs = buffer_pool_size / tot_buf_size;

  /* If we can't allocate any buffers then return an error.
  */
  if (num_bufs == 0)
  {
    return L7_ERROR;
  }

  /* >>>>>>>>>>> Start Critical Section
  ** Determine first unused pool ID.
  */
  if (osapiSemaTake(BufferPoolLockSem, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  pool_id = 0;
  while (pool_id < MaxBufferPools)
  {
    if (BufferPoolList[pool_id].pool_size == 0)
      break;  /* Found an unused buffer pool */
    pool_id++;
  }
  NumBufferPools++;
  pool = &BufferPoolList[pool_id];
  pool->id = pool_id + L7_LOW_BUFFER_POOL_ID;
  pool->pool_size = buffer_pool_size;

  if (NumBufferPools > MaxBufferPools)
  {
    MaxBufferPools = NumBufferPools;
  }

  if (osapiSemaGive(BufferPoolLockSem) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  /* <<<<<<<<<<< End Critical Section
  */

  /* Set up the buffer pool.
  */
  pool->addr = (L7_uint32) buffer_pool_addr;
  pool->buf_size = buffer_size;
  pool->total = num_bufs;
  strncpy(pool->descr,description,L7_MAX_BUFFER_DESCR_SIZE);
  pool->descr[L7_MAX_BUFFER_DESCR_SIZE-1] = 0;

  pool->free_count = num_bufs;

  pool->num_allocs = 0;
  pool->no_buffers_count = 0;
  pool->floor = 0;
  pool->high_watermark = 0;

  pool->free_list = (bufferDescrType **) buffer_pool_addr;

  user_data = ((L7_uchar8 *) buffer_pool_addr) +
              (4 * num_bufs);

  for (i = 0; i < num_bufs; i++)
  {
    pool->free_list[i] = (bufferDescrType *) user_data;

    descr = pool->free_list[i];
    descr->id = (L7_ushort16) (pool_id + L7_LOW_BUFFER_POOL_ID);
    descr->in_use = 0;

    user_data += (buffer_size + sizeof (bufferDescrType));
  }

  *buffer_pool_id = pool_id + L7_LOW_BUFFER_POOL_ID;
  *buffer_count = num_bufs;

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Deletes a buffer pool and deallocates its memory.
*
*
* @param   buffer_pool_id    Application uses this ID to reference the
*                            buffer pool.

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes   This routine both deletes a buffer pool and frees the memory
* @notes   for the buffer pool. It is preferred over bufferPoolDelete().
*
* @end
*********************************************************************/
L7_RC_t bufferPoolTerminate (L7_uint32  buffer_pool_id)
{
  L7_uint32 buffer_pool_addr;
  L7_uint32 pool_id;

  pool_id = buffer_pool_id - L7_LOW_BUFFER_POOL_ID;
  buffer_pool_addr = BufferPoolList[pool_id].addr;

  if ((void *)buffer_pool_addr ==  L7_NULLPTR)
  {
      L7_LOG_ERROR(1);
      return L7_FAILURE;
  }

  if (bufferPoolDelete(buffer_pool_id) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Cannot delete buffer pool- pool id = %x\n", buffer_pool_id);
      return L7_FAILURE;
  }


  /* Deallocate memory for the buffer pool.
  */
  osapiFree(L7_SIM_COMPONENT_ID, (void *)buffer_pool_addr);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Delete a buffer pool.
*
* @param   buffer_pool_id - Buffer pool to be deleted.
*
* @returns  L7_SUCCESS, if success
*
* @returns  L7_FAILURE
*                 - Buffer pool does not exist.
*
*
* @notes
*   Invalid Input is probably due to memory corruption.
*   If in the future we have a recovery mechanism
*
* @end
*********************************************************************/
L7_RC_t bufferPoolDelete (L7_uint32 buffer_pool_id)
{
  L7_uint32 pool_id;

  pool_id = buffer_pool_id - L7_LOW_BUFFER_POOL_ID;

  if (pool_id >= MaxBufferPools)
  {
    L7_LOG_ERROR(buffer_pool_id);
  }


  /*>>>>>>>>>>>>>>>>> Start Critical Section
  */
  if (osapiSemaTake(BufferPoolLockSem, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  if ((BufferPoolList[pool_id].pool_size == 0) ||
      (BufferPoolList[pool_id].id != buffer_pool_id))
  {
    /*<<< Exit Critical Section.
    */
    if (osapiSemaGive(BufferPoolLockSem) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    return L7_FAILURE;
  }

  BufferPoolList[pool_id].id = 0;
  BufferPoolList[pool_id].pool_size = 0;

  NumBufferPools--;

  if (osapiSemaGive(BufferPoolLockSem) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
  /* <<<<<<<<<<<<<<<< End Critical Section
  */

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Allocate a buffer from the buffer pool.
*
* @param   buffer_pool_id - Buffer pool from which to allocate buffer.
* @param   buffer_addr - (output) Address of the buffer.
*
* @returns  L7_SUCCESS, if success
*
* @returns  L7_ERROR
*                 - Buffer pool does not have any more free buffers.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bufferPoolAllocate (L7_uint32 buffer_pool_id,
                            L7_uchar8 ** buffer_addr)
{
  bufferDescrType * descr;
  L7_uint32 pool_id, current_alloc;

  pool_id = buffer_pool_id - L7_LOW_BUFFER_POOL_ID;

  if (pool_id >= MaxBufferPools)
  {
    L7_LOG_ERROR(buffer_pool_id);
  }


  if ((BufferPoolList[pool_id].pool_size == 0) ||
      (BufferPoolList[pool_id].id != buffer_pool_id))
  {
    L7_LOG_ERROR(buffer_pool_id);
  }

  /*>>>>>>>>>>>>>>>>> Start Critical Section
  */
  if (osapiSemaTake(BufferPoolLockSem, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  /* Return an error if we don't have any more free buffers.
  */
  if (BufferPoolList[pool_id].free_count == BufferPoolList[pool_id].floor)
  {
    BufferPoolList[pool_id].no_buffers_count++;

    /*<<< Exit Critical Section.
    */
    if (osapiSemaGive(BufferPoolLockSem) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    return L7_ERROR;
  }

  /* Allocate a buffer
  */

  BufferPoolList[pool_id].free_count--;
  descr = BufferPoolList[pool_id].free_list[BufferPoolList[pool_id].free_count];
  BufferPoolList[pool_id].num_allocs++;
  current_alloc = BufferPoolList[pool_id].total - BufferPoolList[pool_id].free_count;
  if (current_alloc > BufferPoolList[pool_id].high_watermark)
  {
    BufferPoolList[pool_id].high_watermark = current_alloc;
  }

  if (osapiSemaGive(BufferPoolLockSem) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  /* <<<<<<<<<<<<<<<< End Critical Section
  */

  /* Make sure that the buffer is not corrputed.
  */
  if (descr->in_use)
  {
    L7_LOG_ERROR((L7_uint32) descr);
  }


  descr->in_use = 1; /* Mark this buffer "In Use" */

  *buffer_addr = &descr->data[0];

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Return a buffer to the buffer pool.
*
* @param   buffer_pool_id - Buffer pool from which to allocate buffer.
* @param   buffer_addr -  Address of the buffer.
*
* @returns  None
*
* @notes
*      Function calls LOG_ERROR if buffer is corrupted or returned
*      to the wrong pool.
*
* @end
*********************************************************************/
void bufferPoolFree (L7_uint32 buffer_pool_id, L7_uchar8 * buffer_addr)
{
  bufferDescrType * descr;
  L7_uint32 pool_id;

  if (buffer_addr == NULL)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "bufferPoolFree: ID %d, buffer_addr NULL!\n", buffer_pool_id);
      return;
  }

  pool_id = buffer_pool_id - L7_LOW_BUFFER_POOL_ID;

  /* Verify that the buffer pool ID is valid.
  */
  if (pool_id >= MaxBufferPools)
  {
    L7_LOG_ERROR(buffer_pool_id);
  }

  if ((BufferPoolList[pool_id].pool_size == 0) ||
      (BufferPoolList[pool_id].id != buffer_pool_id))
  {
    L7_LOG_ERROR(buffer_pool_id);
  }

  if (BufferPoolList[pool_id].free_count >=
      BufferPoolList[pool_id].total )
  {
    L7_LOG_ERROR(buffer_pool_id);
  }

  /* Set up a pointer to the buffer descriptor.
  */
  descr = (bufferDescrType *) (((unsigned char *) buffer_addr) - sizeof (bufferDescrType));

  /* Verify that the buffer belongs to this pool and is not
  ** corrupted.
  */
  if (descr->in_use == 0)
  {
    L7_LOG_ERROR((L7_uint32) descr);
  }

  if (descr->id != (L7_ushort16) buffer_pool_id)
  {
    L7_LOG_ERROR((L7_uint32) descr);
  }


  /* Looks like the buffer pool and the buffer are OK.
  ** Return the buffer into the pool.
  */

  descr->in_use = 0;

  /*>>>>>>>>>>>>>>>>> Start Critical Section
  */
  if (osapiSemaTake(BufferPoolLockSem, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOG_ERROR((L7_uint32) L7_FAILURE);
  }

  BufferPoolList[pool_id].free_list[BufferPoolList[pool_id].free_count] = descr;
  BufferPoolList[pool_id].free_count++;

  if (osapiSemaGive(BufferPoolLockSem) != L7_SUCCESS)
  {
    L7_LOG_ERROR((L7_uint32) L7_FAILURE);
  }
  /* <<<<<<<<<<<<<<<< End Critical Section
  */

  return;
}

/*********************************************************************
*
* @purpose  Gets the pool ID that a buffer belongs to
*
* @param   buffer_addr -  Address of the buffer.
* @param   buffer_pool_id - (output) ID of buffer pool
*
* @returns  L7_SUCCESS, if success
*
* @notes
*      Function calls LOG_ERROR if buffer is corrupted
*
* @end
*********************************************************************/
L7_RC_t bufferPoolIdGet (L7_uchar8 * buffer_addr, L7_uint32 *buffer_pool_id)
{
  bufferDescrType * descr;

  /* Set up a pointer to the buffer descriptor.
  */
  descr = (bufferDescrType *) (((unsigned char *) buffer_addr) - sizeof (bufferDescrType));

  /* Verify that the buffer belongs to this pool and is not
  ** corrupted.
  */
  if (descr->in_use == 0)
  {
    L7_LOG_ERROR((L7_uint32) descr);
  }


  *buffer_pool_id = (L7_uint32) descr->id;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Tell the caller how much memory is needed for a
*           given number of buffers with a given buffer size.
*           The information may be used to figure out how much
*           memory to allocate before calling bufferPoolCreate()
*
* @param   num_buffers - Number of buffers desired in the pool.
* @param   buffer_size - Size of buffers in the pool.
*
* @returns  Required number of bytes for specified buffer pool attributes.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 bufferPoolSizeCompute (L7_uint32 num_buffers,
                                 L7_uint32 buffer_size)
{
  L7_uint32 mem_size, tot_buf_size;

  /* If buffer size is not a multiple of four then round it up.
  */
  if ((buffer_size & 0xFFFFFFFC) != buffer_size)
  {
    buffer_size += 4;
    buffer_size &= 0xFFFFFFFC;
  }

  /* Compute how much memory is required for each buffer. The overhead
  ** includes buffer descriptor and a 4-byte pointer in the free list.
  */
  tot_buf_size = buffer_size +
                 sizeof (bufferDescrType) +
                 4;

  mem_size = tot_buf_size * num_buffers;

  return mem_size;
}


/*********************************************************************
*
* @purpose  Show information about all buffer pools.
*
* @param   buffer_pool_id - 0 - Show detailed information about all buffers
*                           1 - Show summary information about all buffers
*                           X - Show detailed information about buffer pool X
*
* @returns  None
*
* @notes
*     This is a debug function that can be called only through devshell.
*
* @end
*********************************************************************/
void bufferPoolShow (L7_uint32 buffer_pool_id)
{
  L7_uint32 i;
  bufferPoolType *pool;

  printf("\nTotal Buffer Pools: %d.\n",
         NumBufferPools);


  if (buffer_pool_id == 0)
  {
    for (i = 0; i < MaxBufferPools; i++)
    {
      /* Don't display information about deleted buffer pools.
      */
      if (BufferPoolList[i].pool_size == 0)
      {
        continue;
      }

      pool = &BufferPoolList[i];
      printf("------\n");
      printf("Pool ID: %d, Pool Address: 0x%x, Pool Size: %d, Description: %s \n",
             pool->id,
             pool->addr,
             pool->pool_size,
             pool->descr);

      printf("Tot. Buffs: %d, Free Buffs: %d, Buff. Size: %d, Num Allocs: %d, Num Empty: %d High watermark: %d\n",
             pool->total,
             pool->free_count,
             pool->buf_size,
             pool->num_allocs,
             pool->no_buffers_count,
             pool->high_watermark);

    }
  }
}


/*********************************************************************
*
* @purpose  Reduce the number of buffers available in the buffer pool.
*           This function is intended to force an out-of-buffer condition
*           in order to unit-test applications.
*
* @param   buffer_pool_id - Buffer for which to set the new size.
* @param   buffer_pool_num - New number of buffers in the pool.
*
*
* @returns  L7_SUCCESS, New buffer pool size has been set.
* @returns  L7_ERROR, Invalid size for the specified buffer pool.
* @returns  L7_FAILURE, Invalid pool ID.
*
* @notes
*     This is a debug function that can be called only through devshell.
*     The specified buffer pool size must be less or equal to the
*     initial buffer pool size setting.
*     Also the new buffer size may not be set to 0.
*
* @end
*********************************************************************/
L7_uint32 bufferPoolSizeSet (L7_uint32 buffer_pool_id,
                             L7_uint32 buffer_pool_num)
{
  L7_uint32 pool_id;

  pool_id = buffer_pool_id - L7_LOW_BUFFER_POOL_ID;

  /* Verify that the buffer pool ID is valid.
  */
  if (pool_id >= MaxBufferPools)
  {
    return L7_FAILURE;
  }

  if ((BufferPoolList[pool_id].pool_size == 0) ||
      (BufferPoolList[pool_id].id != buffer_pool_id))
  {
    return L7_FAILURE;
  }

  if (buffer_pool_num > BufferPoolList[pool_id].total)
  {
    return L7_ERROR;
  }

  if (buffer_pool_num <= 0)
  {
    return L7_ERROR;
  }

  /* Set the new floor for the buffer pool.
  */
  BufferPoolList[pool_id].floor = BufferPoolList[pool_id].total -
                                  buffer_pool_num;


  return L7_SUCCESS;
}



/*************************************************************************
**************************************************************************
Temporary test functions.
**************************************************************************
*************************************************************************/
#if 1
/* Create a buffer pool
** Allocate a buffer
** Delete a buffer pool.
*/
void bpool1 (void)
{
  L7_RC_t rc;
  char    *pool_area;
  int     pool_size = 1000;
  int     pool_id;
  int     buff_count;
  L7_uchar8    *buffer_addr;

  pool_area = malloc (pool_size);

  rc = bufferPoolCreate (pool_area,
                         pool_size,
                         128,     /* Buffer Size */
                         "First Pool",
                         &pool_id,
                         &buff_count);


  printf("bpool1: Create - rc = %d, id = %d, count = %d\n",
         rc, pool_id, buff_count);

  rc = bufferPoolAllocate (pool_id,  &buffer_addr);


  printf("bpool1: Allocate - rc = %d, addr = 0x%x\n",
         rc, (L7_uint32) buffer_addr);

  bufferPoolShow (0);

  bufferPoolFree (pool_id, buffer_addr);

  rc = bufferPoolDelete (pool_id);

  printf("bpool1: Delete Pool - rc = %d\n",
         rc);

  free (pool_area);

  bufferPoolShow (0);
}

/* Create a buffer pool
** Attempt to allocate more buffers than available.
** Delete a buffer pool.
*/
void bpool2 (void)
{
  L7_RC_t rc;
  char    *pool_area;
  int     pool_size;
  int     pool_id;
  int     buff_count;
  L7_uchar8    *buffer_addr;

  pool_size = bufferPoolSizeCompute (7, 127);

  pool_area = malloc (pool_size);

  rc = bufferPoolCreate (pool_area,
                         pool_size,
                         127,     /* Buffer Size */
                         "Second Pool",
                         &pool_id,
                         &buff_count);


  printf("bpool2: Create - rc = %d, id = %d, count = %d\n",
         rc, pool_id, buff_count);



  do
  {
    rc = bufferPoolAllocate (pool_id,  &buffer_addr);
    printf("bpool2: Allocate - rc = %d, addr = 0x%x\n",
           rc, (L7_uint32) buffer_addr);

    bufferPoolShow (0);

  } while (rc == L7_SUCCESS);


  rc = bufferPoolDelete (pool_id);

  printf("bpool2: Delete Pool - rc = %d\n",
         rc);

  free (pool_area);

  bufferPoolShow (0);
}

/* Create maximum number of buffer pools.
** Attempt to allocate another buffer pool.
** Delete a buffer pool.
** Create a buffer pool.
*/
void bpool3 (void)
{
  L7_RC_t rc;
  char    *pool_area[L7_MAX_BUFFER_POOLS + 1];
  int     pool_size;
  int     buff_count;
  L7_uint32 pool_id[L7_MAX_BUFFER_POOLS + 1];
  int i;

  pool_size = bufferPoolSizeCompute (7, 127);

  /* Create MAX pools.
  */
  rc = L7_FAILURE;
  for (i = 0; i < L7_MAX_BUFFER_POOLS; i++)
  {
    pool_area[i] = malloc (pool_size);

    rc = bufferPoolCreate (pool_area[i],
                           pool_size,
                           127,     /* Buffer Size */
                           "Third Pool",
                           &pool_id[i],
                           &buff_count);


    if (rc != L7_SUCCESS)
      break;
  }

  if (rc == L7_SUCCESS)
  {
    printf("bpool3: Successfully allocated %d buffer pools.\n", i);
  }

  /* Try to allocate another buffer pool. This should fail.
  */
  rc = bufferPoolCreate (pool_area[i],
                         pool_size,
                         127,     /* Buffer Size */
                         "Third Pool",
                         &pool_id[i],
                         &buff_count);
  printf("bpool3: Allocate too many pools, rc = %d\n", rc);


  /* Delete one pool at random.
  */
  rc = bufferPoolDelete (pool_id[20]);

  printf("bpool3: Delete Pool - rc = %d\n",
         rc);

  /* Create another pool. This should work.
  */
  rc = bufferPoolCreate (pool_area[20],
                         pool_size,
                         127,     /* Buffer Size */
                         "Third Pool",
                         &pool_id[20],
                         &buff_count);
  printf("bpool3: Created another pool, rc = %d\n", rc);

  bufferPoolShow (0);


  /* Delete all pools
  */
  rc = L7_FAILURE;
  for (i = 0; i < L7_MAX_BUFFER_POOLS; i++ )
  {
    rc = bufferPoolDelete (pool_id[i]);
    if (rc != L7_SUCCESS)
      break;

    free (pool_area[i]);
  }

  if (rc == L7_SUCCESS)
  {
    printf("bpool3: Successfuly deleted %d pools.\n", i);
  }


  bufferPoolShow (0);
}


/* Create a buffer pool
** Allocate all buffers buffer
** Fill buffers with data.
** Return buffers in reverse order.
** Delete a buffer pool.
*/
void bpool4 (void)
{
  L7_RC_t rc;
  char    *pool_area;
  int     pool_size = 1000;
  int     pool_id;
  int     buff_count;
  L7_uchar8    *buffer_addr[10];
  int i, j;

  pool_area = malloc (pool_size);

  rc = bufferPoolCreate (pool_area,
                         pool_size,
                         128,     /* Buffer Size */
                         "Fourth Pool",
                         &pool_id,
                         &buff_count);


  printf("bpool4: Create - rc = %d, id = %d, count = %d\n",
         rc, pool_id, buff_count);

  for (j = 0; j < 2; j++)
  {
    printf("\nbpool4: >>>>>>>>> Iteration %d\n", j);

    /* Allocate all buffers from the pool, set content to loop index.
    */
    for (i = 0; i < buff_count; i++)
    {
      rc = bufferPoolAllocate (pool_id,  &buffer_addr[i]);


      printf("bpool4: Allocate - rc = %d, addr = 0x%x\n",
             rc, (L7_uint32) buffer_addr[i]);

      bufferPoolShow (0);

      memset (buffer_addr[i], (char) i, 128);
    }

    /* Free buffers.
    */
    for (i = 0; i < buff_count; i++)
    {
      bufferPoolFree (pool_id, buffer_addr[i]);

      bufferPoolShow (0);
    };


  }

  rc = bufferPoolDelete (pool_id);
  printf("bpool4: Delete Pool - rc = %d\n",
         rc);

  free (pool_area);

  bufferPoolShow (0);
};


/* Create a buffer pool
** Reduce the size of the buffer pool.
** Attempt to allocate more buffers than available.
** Delete a buffer pool.
*/
void bpool5 (void)
{
  L7_RC_t rc;
  char    *pool_area;
  int     pool_size;
  int     pool_id;
  int     buff_count;
  L7_uchar8    *buffer_addr;

  pool_size = bufferPoolSizeCompute (7, 127);

  pool_area = malloc (pool_size);

  rc = bufferPoolCreate (pool_area,
                         pool_size,
                         127,     /* Buffer Size */
                         "Second Pool",
                         &pool_id,
                         &buff_count);


  printf("bpool5: Create - rc = %d, id = %d, count = %d\n",
         rc, pool_id, buff_count);

  bufferPoolShow (0);

  rc = bufferPoolSizeSet (pool_id, (buff_count - 2));

  printf("bpool5: Reduce size by 2 - rc = %d,\n",
         rc);

  bufferPoolShow (0);
  do
  {
    rc = bufferPoolAllocate (pool_id,  &buffer_addr);
    printf("bpool5: Allocate - rc = %d, addr = 0x%x\n",
           rc, (L7_uint32) buffer_addr);

    bufferPoolShow (0);

  } while (rc == L7_SUCCESS);


  rc = bufferPoolDelete (pool_id);

  printf("bpool5: Delete Pool - rc = %d\n",
         rc);

  free (pool_area);

  bufferPoolShow (0);
}


#endif
