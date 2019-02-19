#ifndef _BUFF_API_H_
#define _BUFF_API_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    buff_api.h
*
* @purpose     File contains all the APIs, declared as external, needed for 
*              the buffer manager component.
*
* @component 
*
* @comments 
*         Buffer Manager enables applications to create buffer pools of
*         fixed size buffers. Application that absolutely require dynamic
*         memory should use buffer pools instead of variable size block
*         allocation functions. 
*
*         The size of buffers is a multiple of four.
*
*         The buffer management routines use only interrupt-safe 
*         function calls. The critical sections are protected by 
*         disabling interrupts. Buffers may be allocated and 
*         deallocated inside interrupt handlers.
*  
*
* @create      6/22/2001
*
* @author      atsigler
* @end
*
**********************************************************************/
/*************************************************************
                    
*************************************************************/



/* USE C Declarations */
#ifdef __cplusplus
extern "C" {
#endif


#include <commdefs.h>
#include <datatypes.h>


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

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes   This routine both allocates the memory for a buffer pool and creates
* @notes   the buffer pool. It is preferred over bufferPoolCreate().
*
* @end
*********************************************************************/
L7_uint32 bufferPoolInit (L7_uint32 num_buffers,L7_uint32 buffer_size,
                     L7_char8  *description, L7_uint32 * buffer_pool_id);


/*********************************************************************
*
* @purpose  Create a buffer pool.
*
* @param   buffer_pool_addr - Buffer pool memory address. 
*                             This memory must be statically or dynamically
*                             allocated by the caller. 
* @param   buffer_pool_size - Number of bytes in the buffer_pool_addr. 
* @param   buffer_size - Size of buffers in the pool. The buffer size is rounded
*                        up to the next multiple of four.
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
                            L7_uint32 * buffer_count);




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
L7_RC_t bufferPoolTerminate (L7_uint32  buffer_pool_id);


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
*
* @end
*********************************************************************/
  L7_RC_t bufferPoolDelete (L7_uint32 buffer_pool_id);


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
                              L7_uchar8 ** buffer_addr);


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
void bufferPoolFree (L7_uint32 buffer_pool_id, L7_uchar8 * buffer_addr);

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
L7_RC_t bufferPoolIdGet (L7_uchar8 * buffer_addr, L7_uint32 *buffer_pool_id);

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
                                   L7_uint32 buffer_size);

/*********************************************************************
*
* @purpose  Show information about buffer pools.
*
* @param   buffer_pool_id - 0 - Show detailed information about all buffers
*                           1 - Show summary information about all buffers
*                           X - Show detailed information about buffer pool X
*                                     
*
* @returns  None
*
* @notes  
*     This is a debug function that can be called only through devshell.
*
* @end
*********************************************************************/
  void bufferPoolShow (L7_uint32 buffer_pool_id);


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
                               L7_uint32 buffer_pool_num);


  /* USE C Declarations */
#ifdef __cplusplus
}
#endif

#endif

