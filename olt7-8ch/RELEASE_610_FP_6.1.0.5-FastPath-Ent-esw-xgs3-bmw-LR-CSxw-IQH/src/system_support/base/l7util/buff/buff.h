/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    buff.h
*
* @purpose     File contains defines needed for a buffer manager implimentation
*
* @component 
*
* @comments 
*
* @create      6/21/2001
*
* @author      atsigler
* @end
*
**********************************************************************/

#ifndef _BUFF_H_
#define _BUFF_H_

#include "commdefs.h"
#include "datatypes.h"
#include "l7_resources.h"

#define L7_MAX_BUFFER_DESCR_SIZE 16

/* Lowest buffer pool ID.
*/
#define L7_LOW_BUFFER_POOL_ID  100


/* Header for each buffer 
*/
typedef struct 
{
  L7_ushort16 id;  /* Buffer pool ID */

  L7_ushort16 in_use; /* Flag indicating that the buffer is in use */

  L7_uchar8 data[0];  /* Start of user data. This address is returned
                      ** to the caller of bufferPoolAllocate.
                      */
} bufferDescrType;

/* This structure is maintained for every buffer pool.
*/
typedef struct 
{
  L7_uint32 id;     /* Buffer pool ID */
  L7_uint32 addr;   /* Buffer pool address */
  L7_uint32 pool_size; /* Buffer pool size */
  L7_uint32 buf_size;  /* buffer size */
  L7_uint32 total;    /* Total number of buffers in the pool */
  L7_uchar8 descr [L7_MAX_BUFFER_DESCR_SIZE]; /* Buffer pool description */

  L7_uint32 free_count; /* Number of free buffers in the pool */
  L7_uint32 num_allocs; /* Number of allocations from this buffer pool */

  L7_uint32 floor;  /* Last free buffer index. This variable is used normally set
                    ** to zero, but may be increased to simulate out of buffer scenario.
                    */

  L7_uint32 no_buffers_count; /* Number of buffer requests from an empty pool */

  bufferDescrType  **free_list; /* List of pointers to free buffers. */
} bufferPoolType;

#endif
