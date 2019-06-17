/* $Id: sand_chip_descriptors.c,v 1.11 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $
*/
/* $Id: sand_chip_descriptors.c,v 1.11 Broadcom SDK $
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <shared/bsl.h>

#include <soc/dnx/legacy/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dnx/legacy/SAND/Management/sand_callback_handles.h>
#include <soc/dnx/legacy/SAND/Utils/sand_bitstream.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>
#include <soc/dnx/legacy/SAND/Management/sand_device_management.h>
#include <soc/dnx/legacy/SAND/Utils/sand_tcm.h>
#include <soc/dnxc/legacy/error.h>


#define FE200_REV_B_AS_REV_A  0

/*
 * The chips that are handled by this driver
 * and the Array Size
 */
  DNX_SAND_DEVICE_DESC
    *Dnx_soc_sand_chip_descriptors = NULL ;
/*
 */
  uint32
    Dnx_soc_sand_array_size        = 0 ;
/*
 * a Mutex to guard all register / unregister actions on the array
 * (they must be atomic, cause we can't allow registering the same chip
 * twice, etc).
 */
static
  sal_mutex_t
    Dnx_soc_sand_array_mutex       = 0 ;
/*
 * Dnx_soc_sand_up_counter implemenmts the logic of each descriptor magic word.
 * Its purpose is to provide us with a unique identifier, so it does it
 * by always advancing a counter when soc_get_magic is called.
 */
  uint32 Dnx_soc_sand_up_counter = 0 ;
/*
 */
static
  uint32
    soc_get_magic(
      void
    )
{
  Dnx_soc_sand_up_counter ++ ;
  return Dnx_soc_sand_up_counter ;
}

shr_error_e
  dnx_sand_array_mutex_take()
{
  return dnx_sand_os_mutex_take(Dnx_soc_sand_array_mutex, DNX_SAND_INFINITE_TIMEOUT);
}

shr_error_e
  dnx_sand_array_mutex_give()
{
  return dnx_sand_os_mutex_give(Dnx_soc_sand_array_mutex);
}

/*
 */
/*****************************************************
*NAME:
* dnx_sand_clear_chip_descriptor
*DATE:
* 27/OCT/2002
*FUNCTION:
* clearing one chip descriptor
* This is useful by init, and when unregistering one device.
*INPUT:
*  DNX_SAND_DIRECT:
*     uint32 index -
*       index in the array of the descriptor to clear
*     uint32 init_flag
*       0 - this method is called for init
*       1 - this method is called for romoval
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*REMARKS:
* static (private, local) method used by init_descriptors(),
* and by remove_descriptor(). The method can be called on
* a valid entry or on an empty entry.
* Mutex issue: it is assumed that if the device's mutex existed
* before (init_flag = TRUE) clearing this descriptor,
* then the upper layer took it, an it is our responsibility
* to delete it.
*SEE ALSO:
*****************************************************/
static
  shr_error_e
    dnx_sand_clear_chip_descriptor(
      uint32 index,
      uint32 init_flag
  )
{
  int
    iii ;
  shr_error_e
    ex = _SHR_E_NONE;

  /*
   * clear pointres, and params
   */
  Dnx_soc_sand_chip_descriptors[index].base_addr            = 0 ;
  Dnx_soc_sand_chip_descriptors[index].chip_type            = 0 ;
  Dnx_soc_sand_chip_descriptors[index].dbg_ver              = 0 ;
  Dnx_soc_sand_chip_descriptors[index].chip_ver             = 0 ;
  /*
   * clear interrupt bitstream (cause_id)
   */
  dnx_sand_bitstream_clear(Dnx_soc_sand_chip_descriptors[index].interrupt_bitstream,
                        SIZE_OF_BITSTRAEM_IN_UINT32S
                      ) ;
  /*
   * clear interrupt callback array
   */
  for (iii = 0 ; iii < MAX_INTERRUPT_CAUSE ; ++iii)
  {
    Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[iii].func   = NULL ;
    /*
     * free the callback buffer
     */
    if (init_flag && Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[iii].buffer)
    {
      dnx_sand_os_free(
        Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[iii].buffer
      ) ;
    }
    /*
     */
    Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[iii].buffer = NULL ;
    Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[iii].size   = 0 ;
  }
  Dnx_soc_sand_chip_descriptors[index].unmask_ptr                      = NULL ;
  Dnx_soc_sand_chip_descriptors[index].is_bit_auto_clear_ptr           = NULL ;
  Dnx_soc_sand_chip_descriptors[index].is_device_interrupts_masked_ptr = NULL ;
  Dnx_soc_sand_chip_descriptors[index].get_device_interrupts_mask_ptr  = NULL ;
  Dnx_soc_sand_chip_descriptors[index].device_interrupt_mask_counter   = 0;
  Dnx_soc_sand_chip_descriptors[index].device_is_between_isr_to_tcm    = FALSE;
  Dnx_soc_sand_chip_descriptors[index].magic = soc_get_magic() ;
  Dnx_soc_sand_chip_descriptors[index].device_at_init = FALSE ;
  Dnx_soc_sand_chip_descriptors[index].valid_word = 0 ;
  /*
   * delete chip mutex (if exist)
   * other tasks waiting on the chip mutex get error
   */
  if (init_flag)
  {
    dnx_sand_os_mutex_give(Dnx_soc_sand_chip_descriptors[index].mutex_id);
    ex = dnx_sand_os_mutex_delete(Dnx_soc_sand_chip_descriptors[index].mutex_id) ;
    if (ex != _SHR_E_NONE)
    {
      ex = _SHR_E_INTERNAL;
      goto exit ;
    }
  }
  Dnx_soc_sand_chip_descriptors[index].mutex_id      = 0 ;
  Dnx_soc_sand_chip_descriptors[index].mutex_owner   = 0 ;
  Dnx_soc_sand_chip_descriptors[index].mutex_counter = 0 ;

exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_init_chip_descriptors
*DATE:
* 27/OCT/2002
*FUNCTION:
* The initialization routine of the chip descriptors array
*INPUT:
*  DNX_SAND_DIRECT:
*    uint32 max_devices   -
*       number of devices to allocate space to
*  DNX_SAND_INDIRECT:
*   the array is initilized
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*REMARKS:
* allocates the array and its mutex, and then
* simply go over the array and call
* dnx_sand_clear_chip_descriptor() for each descriptor
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_init_chip_descriptors(
    uint32 max_devices
  )
{
  unsigned
    int
      iii ;
  shr_error_e
    ex = _SHR_E_NONE;

  /*
   * lets check that the array was deleted before
   * we are starting it now
   */
  if (Dnx_soc_sand_chip_descriptors || Dnx_soc_sand_array_mutex)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * OK, time to allocate the new array, and the new mutex
   */
  Dnx_soc_sand_chip_descriptors = dnx_sand_os_malloc((max_devices * sizeof(DNX_SAND_DEVICE_DESC)), "Dnx_soc_sand_chip_descriptors") ;
  if (!Dnx_soc_sand_chip_descriptors)
  {
    ex = _SHR_E_MEMORY ;
    goto exit ;
  }
  /*
   */
  Dnx_soc_sand_array_size = max_devices ;
  /*
   */
  Dnx_soc_sand_array_mutex = dnx_sand_os_mutex_create() ;
  if (!Dnx_soc_sand_array_mutex)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  if (_SHR_E_NONE != dnx_sand_os_mutex_take(Dnx_soc_sand_array_mutex, DNX_SAND_INFINITE_TIMEOUT))
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * go over the array and clear all descriptors
   */
   for (iii = 0 ; iii < Dnx_soc_sand_array_size ; ++iii)
   {
     dnx_sand_clear_chip_descriptor(iii,FALSE) ;
   }
   /*
    */
   if (_SHR_E_NONE != dnx_sand_os_mutex_give(Dnx_soc_sand_array_mutex))
   {
     ex = _SHR_E_INTERNAL ;
     goto exit ;
   }
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_delete_chip_descriptors
*DATE:
* 30/OCT/2002
*FUNCTION:
* The opposite of the initialization routine of the descriptors
*INPUT:
*  DNX_SAND_DIRECT:
*  DNX_SAND_INDIRECT:
*   the array is deleted
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*REMARKS:
* The opposite of init. clears the whole array,
* and then de-allocate the array and the mutex
*SEE ALSO:
*****************************************************/
shr_error_e
 dnx_sand_delete_chip_descriptors(
   void
 )
{
  unsigned
    int
      iii ;
  shr_error_e
    ex = _SHR_E_NONE;

  /*
   * lets check that we have an array and mutex to delete
   */
  if (!Dnx_soc_sand_chip_descriptors)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  if (!Dnx_soc_sand_array_mutex)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * First of all lets clear all descriptors
   */
  /*
   * go over the array and take all mutexes
   */
  for (iii=0 ; iii<Dnx_soc_sand_array_size ; ++iii)
  {
    if(dnx_sand_is_chip_descriptor_valid(iii))
    {
      if (_SHR_E_NONE != dnx_sand_take_chip_descriptor_mutex(iii)){
        ex = _SHR_E_INTERNAL ;
        goto exit ;
      }
    }
  }
  /*
   * take array mutex
   */
  if (_SHR_E_NONE != dnx_sand_os_mutex_take(Dnx_soc_sand_array_mutex, DNX_SAND_INFINITE_TIMEOUT))
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * go over the array and clear all descriptors
   */
  for (iii=0 ; iii<Dnx_soc_sand_array_size ; ++iii)
  {
    if(dnx_sand_is_chip_descriptor_valid(iii))
    {
      /* also deletes the chip mutex */
      dnx_sand_clear_chip_descriptor(iii, TRUE) ;
    }
    else
    {
      dnx_sand_clear_chip_descriptor(iii, FALSE) ;
    }
  }
  /*
   * Now we can free the allocated array
   */
  dnx_sand_os_free(Dnx_soc_sand_chip_descriptors) ;
  Dnx_soc_sand_chip_descriptors = NULL ;
  Dnx_soc_sand_array_size       = 0 ;
  /*
   * And now we can finally delete the mutex (whomever
   * is trying to take it gets an error)
   */
  dnx_sand_os_mutex_delete(Dnx_soc_sand_array_mutex) ;
  Dnx_soc_sand_array_mutex = 0 ;

exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_remove_chip_descriptor
*DATE:
* 30/OCT/2002
*FUNCTION:
* The opposite of the dnx_sand_add_chip_descriptor()
*INPUT:
*  DNX_SAND_DIRECT:
*     uint32 index - index in the array
*  DNX_SAND_INDIRECT:
*   the entry at index is deleted
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*REMARKS:
* The opposite of add. clears the entry.
*SEE ALSO:
*****************************************************/
shr_error_e
 dnx_sand_remove_chip_descriptor(
   uint32 index
 )
{
  shr_error_e
    ex = _SHR_E_NONE;

  /*
   * check that the array was started successfully
   */
  if (!Dnx_soc_sand_chip_descriptors || !Dnx_soc_sand_array_mutex)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * check if index is legal (0 < index < Dnx_soc_sand_array_size)
   */
  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * the actual clearing of the descriptor should be done under mutex protection
   */
  if (_SHR_E_NONE != dnx_sand_os_mutex_take(Dnx_soc_sand_array_mutex, DNX_SAND_INFINITE_TIMEOUT))
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   */
  dnx_sand_clear_chip_descriptor(index, TRUE) ;
  /*
   */
  if (_SHR_E_NONE != dnx_sand_os_mutex_give(Dnx_soc_sand_array_mutex))
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }

exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_add_chip_descriptor
*DATE:
* 27/OCT/2002
*FUNCTION:
* adds a chip descriptor to the array of descriptors
*INPUT:
*  DNX_SAND_DIRECT:
*     uint32  *base_address    -
*       start address of the device on the user board
*     uint32  mem_size         -
*       memory size (in bytes) that the device takes
*       in order to make sure it is not overlapping
*       with other devices.
*     DESC_SAND_REG  *reg_file        -
*       logical register table of that device,
*       usefull for logical access to the device.
*       currently not used.
*     DNX_SAND_UNMASK_FUNC_PTR  func_ptr       -
*       a pointer to a mthod that unmask the inettrupt
*       line of this specific device.
*     uint32  chip_type        -
*   	chip_typas defined by Dune in the device's data sheet
*     uint32  handle           -
*   	Specific handle to be used for device. If negative, handle
*   	will be allocated dynamically
*  DNX_SAND_INDIRECT:
*   a descriptor is added to the array
*OUTPUT:
*  DNX_SAND_DIRECT:
*     int -
*     non negative (handle to array) if success
*     negative if error:
*     -1: array or mutex not initialized
*     -2: failed to take the array mutex
*     -3: array is full
*     -4: memory over lap from front
*     -5: memory over lap from back
*     -6: failed to create mutex
*  DNX_SAND_INDIRECT:
*REMARKS:
*   This method does not check that the registered chip
*   is ctually there, and the chip type parameter is saved
*   for the upper layers use.
*   The mothod does not set the "valid word" into the descriptor,
*   but it is rather the upper layer reponsibility to check
*   that the method succeeded, and that the chip is really alive,
*   and only then to set it to be valid
*SEE ALSO:
*****************************************************/
int
  dnx_sand_add_chip_descriptor(
    uint32                  *base_address,
    uint32                  mem_size, /*in bytes*/
    DNX_SAND_UNMASK_FUNC_PTR                unmask_func_ptr,
    DNX_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR     is_bit_ac_func_ptr,
    DNX_SAND_IS_DEVICE_INTERRUPTS_MASKED    is_dev_int_mask_func_ptr,
    DNX_SAND_GET_DEVICE_INTERRUPTS_MASK     get_dev_mask_func_ptr,
    DNX_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE  mask_specific_interrupt_cause_ptr,
    DNX_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr,
    DNX_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR is_read_write_protect_ptr,
    uint32                  chip_type,
    DNX_SAND_DEVICE_TYPE               logic_chip_type,
	int                            handle
  )
{
  uint32
    iii;
  sal_mutex_t
    mutex ;
  shr_error_e
    ex = _SHR_E_NONE;

  /*
   */
  iii = 0 ;
  mutex = 0 ;
  /*
   * check that the array was started successfully
   */
  if (!Dnx_soc_sand_chip_descriptors || !Dnx_soc_sand_array_mutex)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }

  if (handle > 0)
  {
	  if (dnx_sand_is_chip_descriptor_valid(handle))
      {
		  /* handle already in use */
		  ex = _SHR_E_INTERNAL;
		  goto exit;
	  }
	  else
	  {
		  iii = handle;
	  }
  }
  else
  {
	  for (iii=0 ; iii<Dnx_soc_sand_array_size ; ++iii)
	  {
		if (!dnx_sand_is_chip_descriptor_valid(iii))
		{
		   break ;
		}
	  }
  }

  /*
   * check if the array is already full
   */
  if (iii >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
 /*
  * create the mutex for the new device
  */
  mutex = dnx_sand_os_mutex_create() ;
  if (0 == mutex)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }

exit:
  if (0 == ex)
  {
   Dnx_soc_sand_chip_descriptors[iii].mutex_id                          = mutex;
   Dnx_soc_sand_chip_descriptors[iii].mutex_id                          = mutex;
   Dnx_soc_sand_chip_descriptors[iii].base_addr                         = base_address;
   Dnx_soc_sand_chip_descriptors[iii].mem_size                          = mem_size;
   Dnx_soc_sand_chip_descriptors[iii].unmask_ptr                        = unmask_func_ptr;
   Dnx_soc_sand_chip_descriptors[iii].is_bit_auto_clear_ptr             = is_bit_ac_func_ptr;
   Dnx_soc_sand_chip_descriptors[iii].is_device_interrupts_masked_ptr   = is_dev_int_mask_func_ptr;
   Dnx_soc_sand_chip_descriptors[iii].get_device_interrupts_mask_ptr    = get_dev_mask_func_ptr;
   Dnx_soc_sand_chip_descriptors[iii].mask_specific_interrupt_cause_ptr = mask_specific_interrupt_cause_ptr;
   Dnx_soc_sand_chip_descriptors[iii].reset_device_ptr                  = reset_device_ptr;
   Dnx_soc_sand_chip_descriptors[iii].is_read_write_protect_ptr         = is_read_write_protect_ptr;
   Dnx_soc_sand_chip_descriptors[iii].logic_chip_type                   = logic_chip_type;
   Dnx_soc_sand_chip_descriptors[iii].chip_type                         = chip_type;
   Dnx_soc_sand_chip_descriptors[iii].magic                             = soc_get_magic();
   Dnx_soc_sand_chip_descriptors[iii].valid_word                        = 0;
  }
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}



/*
 */
/*****************************************************
*NAME:
* dnx_sand_register_event_callback
*DATE:
* 03/NOV/2002
*FUNCTION:
* adds a callback interrupt handling routine
* in charge of a specific interrupt cause
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN     DNX_SAND_TCM_CALLBACK      func      - callback to call
*    DNX_SAND_IN     uint32      *buffer   - buffer to pass to the callback
*    DNX_SAND_IN     uint32      size      - buffer size
*    DNX_SAND_IN     uint32         index     - device id
*    DNX_SAND_IN     uint32         cause     - interrupt bit
*    DNX_SAND_INOUT  uint32          *handle   - user suppllied buffer
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*   Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*   Handle to this specific callback (to be used by unregister_event_callback)
*REMARKS:
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_register_event_callback(
    DNX_SAND_INOUT  DNX_SAND_TCM_CALLBACK func,   /* callback to call*/
    DNX_SAND_INOUT  uint32     *buffer,/* buffer to pass to the callback */
    DNX_SAND_INOUT  uint32     size,   /* buffer size */
    DNX_SAND_IN     uint32      index,  /* device id */
    DNX_SAND_IN     uint32      cause,  /* interrupt bit */
    DNX_SAND_INOUT  uint32     *handle
  )
{
  uint32
      res ;
  shr_error_e
    ex = _SHR_E_NONE;

  /*
   * check that index is legal
   */
  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * check that cause is in the right range
   */
  if (cause >= MAX_INTERRUPT_CAUSE)
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * check that descriptor is valid
   */
  if (!dnx_sand_is_chip_descriptor_valid(index))
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * Check that no callback is already there
   * (if so, it should have been unregistered)
   */
  if (_SHR_E_NONE != dnx_sand_take_chip_descriptor_mutex(index))
  {
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  if (Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause].func)
  {
      ex = dnx_sand_give_chip_descriptor_mutex(index);
    if (_SHR_E_NONE != ex)
    {
      goto exit ;
    }
    ex = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * OK, set the callback in its place
   */
  Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause].func    = func ;
  Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause].buffer  = buffer ;
  Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause].size    = size ;
  if (_SHR_E_NONE != dnx_sand_give_chip_descriptor_mutex(index))
  {
      ex = _SHR_E_INTERNAL;
      goto exit ;
  }
  /*
   * as long a both index and cause are less the 16 bit, we can place
   * them together within a 32 bit handle
   */
  res   = index ;
  res <<= 16 ;
  res  += cause ;
  *handle = res ;

exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_unregister_event_callback
*DATE:
* 03/NOV/2002
*FUNCTION:
* removes a callback interrupt handling routine
* in charge of a specific interrupt cause
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN  uint32   handle   -
*       the handle that was passed by register_event_callback
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*   Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*   The callback is deleted from the table
*REMARKS:
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_unregister_event_callback(
    DNX_SAND_IN     uint32            handle
  )
{
  shr_error_e
    ex = _SHR_E_NONE ;
  unsigned
    int
      index,
      cause1,
      cause2 ;

  /*
   * as long a both index and cause are less the 16 bit, they were
   * put together within a 32 bit handle
   */
  index = handle >> 16 ;
  cause1 = (handle << 24) >> 24 ;
  cause2 = (handle << 16) >> 24 ;
  /*
   * check that index is legal
   */
  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * check that descriptor is valid
   */
  if (!dnx_sand_is_chip_descriptor_valid(index))
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * Check that no callback is already there
   * (if so, it should have been unregistered)
   */
  if (_SHR_E_NONE != dnx_sand_take_chip_descriptor_mutex(index))
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * first, lets mask the specific bit (chain of bits actually) at the device
   */
  dnx_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(index)(index, cause1);
  /*
   * free the callback buffer
   */
  if (Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].buffer)
  {
    dnx_sand_os_free(
      Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].buffer
    ) ;
  }
  /*
   * and zero everything else
   */
  Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].func    = NULL ;
  Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].buffer  = NULL ;
  Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].size    = 0 ;
  /*
   * and if there was a cause2 to, lets do him to
   */
  if (cause2)
  {
  /*
   * first, lets mask the specific bit (chain of bits actually) at the device
   */
  dnx_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(index)(index, cause2);
    /*
     * free the callback buffer
     */
    if (Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].buffer)
    {
      dnx_sand_os_free(
        Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].buffer
      ) ;
    }
    /*
     * and zero everything else
     */
    Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].func    = NULL ;
    Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].buffer  = NULL ;
    Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].size    = 0 ;
  }
  if (dnx_sand_give_chip_descriptor_mutex(index) != _SHR_E_NONE)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }

exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_combine_2_event_callback_handles
*DATE:
* 18/NOV/2002
*FUNCTION:
* combines 2 interrupts handels of the same unit
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN     uint32            handle_1  -
*         handle_1 to combine
*    DNX_SAND_IN     uint32            handle_2  -
*         handle_2 to combine
*    DNX_SAND_INOUT  uint32           *handle    -
*         combined handle
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*   Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_combine_2_event_callback_handles(
    DNX_SAND_IN     uint32            handle_1,
    DNX_SAND_IN     uint32            handle_2,
    DNX_SAND_INOUT  uint32           *handle
  )
{
  shr_error_e
    ex = _SHR_E_NONE;
  unsigned
    int
      index_1,
      index_2,
      cause_2 ;

  /*
   * as long a both index and cause are less the 16 bit, they were
   * put together within a 32 bit handle
   */
  index_1 = handle_1 >> 16 ;
  index_2 = handle_2 >> 16 ;
  cause_2 = (handle_2 << 16) >> 16 ;
  /*
   */

  if (!handle_1)
  {
    *handle = handle_2 ;
    goto exit ;
  }
  /*
   */
  if (!handle_2)
  {
    *handle = handle_1 ;
    goto exit ;
  }

  if (index_1 != index_2)
  {
    /*
     * not the same device
     */
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * Since, there are only 256 (8 bit) causes,
   * we can combine to of them together
   */
  cause_2 <<= 8 ;
  *handle = handle_1 | cause_2 ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;

}

/*
 * check all device counter
 * return true / false
 */

uint32
  dnx_sand_is_any_chip_descriptor_taken_by_task(
    sal_thread_t task_id
  )
{
  uint32
    chip_desc_taken_flag = FALSE,
    index_i;

  for(index_i = 0;index_i < Dnx_soc_sand_array_size; index_i++)
  {
    if(Dnx_soc_sand_chip_descriptors[index_i].mutex_owner == task_id)
    {
      chip_desc_taken_flag = TRUE;
      goto exit;
    }
  }
exit:
  return chip_desc_taken_flag;
}

uint32
  dnx_sand_is_any_chip_descriptor_taken(
    void
  )
{
  uint32
    chip_desc_taken_flag = FALSE,
    index_i;

  for(index_i = 0;index_i < Dnx_soc_sand_array_size; index_i++)
  {
    if(dnx_sand_is_chip_descriptor_valid(index_i))
    {
      chip_desc_taken_flag = TRUE;
      goto exit;
    }
  }
exit:
  return chip_desc_taken_flag;
}
/*****************************************************
*NAME:
* dnx_sand_get_chip_descriptor_mutex_owner
* dnx_sand_set_chip_descriptor_mutex_owner
* dnx_sand_get_chip_descriptor_mutex_counter
* dnx_sand_set_chip_descriptor_mutex_counter
* dnx_sand_inc_chip_descriptor_mutex_counter
* dnx_sand_dec_chip_descriptor_mutex_counter
*DATE:
* 10/NOV/2002
*FUNCTION:
* mutex_owner / mutex counter get / set metods
*INPUT:
*  DNX_SAND_DIRECT:
*     uint32 index - index in the array
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*  DNX_SAND_INDIRECT:
*REMARKS:
* These 2 members allows for the same task th take
* the mutex more then once.
*SEE ALSO: dnx_sand_take_chip_descriptor_mutex / dnx_sand_give_chip_descriptor_mutex
*****************************************************/
sal_thread_t
  dnx_sand_get_chip_descriptor_mutex_owner(
    uint32 index
  )
{
  sal_thread_t
    ex ;
  shr_error_e
    error_code ;
  error_code = _SHR_E_NONE ;
  /*
   */
  if (index >= Dnx_soc_sand_array_size)
  {
    ex = (sal_thread_t)(-1) ;
    error_code = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].mutex_owner ;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
static
  shr_error_e
    dnx_sand_set_chip_descriptor_mutex_owner(
      uint32 index,
      sal_thread_t         tid
    )
{
  shr_error_e
    ex = _SHR_E_NONE ;
  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  Dnx_soc_sand_chip_descriptors[index].mutex_owner = tid ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*
 */
static
  int32
    dnx_sand_get_chip_descriptor_mutex_counter(
      uint32 index
    )
{
  int32
    ex = 0;
  shr_error_e
    error_code ;
  error_code = _SHR_E_NONE ;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].mutex_counter ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
static
  shr_error_e
    dnx_sand_set_chip_descriptor_mutex_counter(
      uint32 index,
      int32 cnt
    )
{
  shr_error_e
    ex = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
   Dnx_soc_sand_chip_descriptors[index].mutex_counter = cnt ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*
 */
static
  shr_error_e
    dnx_sand_inc_chip_descriptor_mutex_counter(
      uint32 index
    )
{
  shr_error_e
    ex = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   */
  Dnx_soc_sand_chip_descriptors[index].mutex_counter++ ;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*
 */
static
  shr_error_e
    dnx_sand_dec_chip_descriptor_mutex_counter(
      uint32 index
    )
{
  shr_error_e
    ex = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   */
  Dnx_soc_sand_chip_descriptors[index].mutex_counter-- ;
  if (Dnx_soc_sand_chip_descriptors[index].mutex_counter < 0)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME:
* dnx_sand_take_chip_descriptor_mutex
* dnx_sand_give_chip_descriptor_mutex
*DATE:
* 29/OCT/2002
*FUNCTION:
*  takes / gives the mutex associated with the specific
*  chip, not forgetting several important legitimate
*  values tests.
*INPUT:
*  DNX_SAND_DIRECT:
*    uint32 index - index in the array
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    zero      - semaphore taken
*    negative  - error
*    positive  - device was cleared in the middle
*  DNX_SAND_INDIRECT:
*REMARKS:
*  These 2 methods are the one that should be carried
*SEE ALSO:
*****************************************************/
int
  dnx_sand_take_chip_descriptor_mutex(
    uint32 index
  )
{
  shr_error_e
    ex = _SHR_E_INTERNAL;
  uint32
      local_magic ;
  sal_thread_t
    current_task_id;

  /*
   * check that index is legal
   */
  current_task_id = dnx_sand_os_get_current_tid();
  if (index >= Dnx_soc_sand_array_size)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * check that descriptor is valid
   */
  if (!dnx_sand_is_chip_descriptor_valid(index))
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * if the mutex is already owned by this task, we increase the ownership
   * counter (it gets decreased only with a call to give), and return (OK),
   * since the mutex is already (rightfully) ours.
   */
  if (current_task_id == dnx_sand_get_chip_descriptor_mutex_owner(index))
  {
    dnx_sand_inc_chip_descriptor_mutex_counter(index) ;
    ex = _SHR_E_NONE ;
    goto exit ;
  }
  if(current_task_id == dnx_sand_handles_delta_list_get_owner())
  {
    dnx_sand_inc_chip_descriptor_mutex_counter(index) ;
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }

  /*
   * if we got here, we have a different task_id then the owner (or maybe
   * there is no owner yet), so lets try and take it.
   *
   * we mark for ourself the last value of magic before trying to take
   * the mutex. This is a classic software defensive messure:
   * We get ready for the possibility that while we are waiting for the
   * semaphore another task will delete the descriptor (making it invalid)
   * and allocate a new one in its place (making it valid, but with different magic).
   *
   * and another point: although the value of chip_descriptor_mutex_owner might change
   * (even several times) before we actually gonna take it, it cannot be our task there,
   * since our task is waiting (infintely) on the mutex's Q.
   */
  local_magic = dnx_sand_get_chip_descriptor_magic(index) ;
  if (_SHR_E_NONE != dnx_sand_os_mutex_take(Dnx_soc_sand_chip_descriptors[index].mutex_id, (long)DNX_SAND_INFINITE_TIMEOUT))
  {
    if (!dnx_sand_is_chip_descriptor_valid(index))
    {
      /*
       * we got the != _SHR_E_NONE because someone deleted the device mutex,
       * and cleared the descriptor (hence invalid)
       */
      ex = _SHR_E_INTERNAL ;
      goto exit ;
    }
    else
    {
      /*
       * sem_take failed
       */
      ex = _SHR_E_INTERNAL ;
      goto exit ;
    }
  }
  /*
   * And we're not done with the errors.
   * Another possible scenario, is that while we were waiting
   * for the semaphore, someone deleted it, and allocated a new one.
   * So the taking of the mutex succeeded. This is a very difficult
   * bug to catch...
   */
  if(dnx_sand_get_chip_descriptor_magic(index) != local_magic)
  {
    dnx_sand_os_mutex_give(Dnx_soc_sand_chip_descriptors[index].mutex_id) ;
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   * Only if we got here, the mutex is rightfully ours (for the first
   * time), so lets mark our ownership, and return OK.
   */
  dnx_sand_set_chip_descriptor_mutex_owner(index, dnx_sand_os_get_current_tid()) ;
  dnx_sand_set_chip_descriptor_mutex_counter(index, 1) ;

exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*
 */
shr_error_e
  dnx_sand_give_chip_descriptor_mutex(
    uint32 index
  )
{
  shr_error_e
    ex = _SHR_E_NONE ;

  if (dnx_sand_os_get_current_tid() != dnx_sand_get_chip_descriptor_mutex_owner(index))
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  dnx_sand_dec_chip_descriptor_mutex_counter(index) ;
  if (dnx_sand_get_chip_descriptor_mutex_counter(index) == 0)
  {
    dnx_sand_set_chip_descriptor_mutex_owner(index, 0) ;
    dnx_sand_os_mutex_give(Dnx_soc_sand_chip_descriptors[index].mutex_id) ;
  }
  else if (dnx_sand_get_chip_descriptor_mutex_counter(index) < 0)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
exit:
  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}


/*****************************************************
*NAME:
* dnx_sand_set_chip_descriptor_ver_info
*DATE:
* 17/NOV/2004
*FUNCTION:
* sets a device version to a specific chip descriptor
*INPUT:
*  DNX_SAND_DIRECT:
*     uint32 index - index in the array
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*   0 - error, otherwise the field that was requested
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_set_chip_descriptor_ver_info(
    DNX_SAND_IN uint32    index,
    DNX_SAND_IN uint32   dbg_ver,
    DNX_SAND_IN uint32   chip_ver
  )
{
  shr_error_e
    error_code = _SHR_E_NONE ;


  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    goto exit ;
  }


  Dnx_soc_sand_chip_descriptors[index].dbg_ver  = dbg_ver ;
  Dnx_soc_sand_chip_descriptors[index].chip_ver = chip_ver ;

exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return error_code ;
}
/*****************************************************
*NAME:
* dnx_sand_get_chip_descriptor_base_addr
* dnx_sand_get_chip_descriptor_memory_size
* dnx_sand_get_chip_descriptor_interrupt_callback_array
* dnx_sand_get_chip_descriptor_interrupt_bitstream
* dnx_sand_get_chip_descriptor_chip_type
* dnx_sand_set_chip_descriptor_valid
* dnx_sand_is_chip_descriptor_valid
* dnx_sand_get_chip_descriptor_device_at_init
* dnx_sand_get_chip_descriptor_magic
*DATE:
* 27/OCT/2002
*FUNCTION:
* gets a specific field from a specific chip descriptor
*INPUT:
*  DNX_SAND_DIRECT:
*     uint32 index - index in the array
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*   0 - error, otherwise the field that was requested
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  *dnx_sand_get_chip_descriptor_base_addr(
    uint32 index
  )
{
  uint32
      *ex = NULL;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].base_addr ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * size of memory space taken by this device
 */
uint32
  dnx_sand_get_chip_descriptor_memory_size(
    uint32 index
  )
{
  uint32
      ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].mem_size ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * This is an array of callbacks, that do not require polling,
 * but rather act as event notifications. The cause for such
 * events are interrupts, so this is actually an arrray of callbacks,
 * each handles a different interrupt.
 */
struct dnx_sand_tcm_callback_str
  *dnx_sand_get_chip_descriptor_interrupt_callback_array(
    uint32 index
   )
{
   struct
    dnx_sand_tcm_callback_str
      *ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].interrupt_callback_array ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * This is the actual connection to the interrupt line. The interrupt serving routine
 * checks what caused the interrupt, and sets the right bit.
 */
uint32
  *dnx_sand_get_chip_descriptor_interrupt_bitstream(
    uint32 index
  )
{
  uint32
     *ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].interrupt_bitstream ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * The interrupt unmasking method tied up to this chip
 */
DNX_SAND_UNMASK_FUNC_PTR
  dnx_sand_get_chip_descriptor_unmask_func(
    uint32 index
  )
{
  DNX_SAND_UNMASK_FUNC_PTR ex;
  shr_error_e error_code;

  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = NULL ;
    goto exit ;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].unmask_ptr ;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
DNX_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR
  dnx_sand_get_chip_descriptor_is_bit_auto_clear_func(
    uint32 index
  )
{
  DNX_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR ex;
  shr_error_e error_code;

  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = NULL;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].is_bit_auto_clear_ptr;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * Tcm interrupt serving loop must be carried out when device interrupts are masked.
 * if thery are not masked it is a fatal error.
 */
DNX_SAND_IS_DEVICE_INTERRUPTS_MASKED
  dnx_sand_get_chip_descriptor_is_device_interrupts_masked_func(
    uint32 index
  )
{
  DNX_SAND_IS_DEVICE_INTERRUPTS_MASKED ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = NULL;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].is_device_interrupts_masked_ptr;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * Every interrupt source in the device architecture has an associated
 * mask bit. the tcm must respect this bit as well, before calling
 * registered callback functions.
 */
DNX_SAND_GET_DEVICE_INTERRUPTS_MASK
  dnx_sand_chip_descriptor_get_interrupts_mask_func(
    uint32 index
  )
{
  DNX_SAND_GET_DEVICE_INTERRUPTS_MASK ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = NULL;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].get_device_interrupts_mask_ptr;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * Every interrupt source in the device architecture has an associated
 * mask bit. this is the mean to reset (mask) it so the source will
 * not generate interrupts.
 */
DNX_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE
  dnx_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(
    uint32 index
  )
{
  DNX_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = NULL;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].mask_specific_interrupt_cause_ptr;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
DNX_SAND_RESET_DEVICE_FUNC_PTR
  dnx_sand_chip_descriptor_get_reset_device_func(
    uint32 index
  )
{
  DNX_SAND_RESET_DEVICE_FUNC_PTR ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = NULL;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].reset_device_ptr;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
DNX_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR
  dnx_sand_chip_descriptor_get_is_read_write_protect_func(
    uint32 index
  )
{
  DNX_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = NULL;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].is_read_write_protect_ptr;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex;
}
/*
 * mask / unmask counter and level
 * The logic goes something like this:
 * you can mask the general interrupt bit (fecint) from 2 places:
 * the interrupt handler, or the user code
 * (through fe200_logical_write() api method of this logic field_id)
 * The interrupt is superior to the user - meaning, we don't want
 * the user to unmask them while interrupts are still pending to
 * be served (that's the reason they were masked to begin with)
 * That's why we keep a level - Interrupt level = TRUE, user level = FALSE
 * The same apply to the unmasking only now it's the tcm that unmasks the
 * fecint bit, when done serving interrupts.
 * We maintain the counter in order to know to which state to restore.
 * > 0 => masked, < 0 => unmasked
 */
uint32
  dnx_sand_device_is_between_isr_to_tcm(
    uint32 index
  )
{
  uint32 ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = FALSE;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].device_is_between_isr_to_tcm;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
void
  dnx_sand_device_set_between_isr_to_tcm(
    uint32 index,
  uint32 level
  )
{
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    goto exit;
  }
  /*
   */
  Dnx_soc_sand_chip_descriptors[index].device_is_between_isr_to_tcm = level;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
}
/*
 */
int
  dnx_sand_device_get_interrupt_mask_counter(
    uint32 index
  )
{
  int ex;
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    ex = 0;
    goto exit;
  }
  /*
   */
  ex = Dnx_soc_sand_chip_descriptors[index].device_interrupt_mask_counter;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
void
  dnx_sand_device_inc_interrupt_mask_counter(
    uint32 index
  )
{
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    goto exit;
  }
  /*
   */
  Dnx_soc_sand_chip_descriptors[index].device_interrupt_mask_counter ++;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
}
void
  dnx_sand_device_dec_interrupt_mask_counter(
    uint32 index
  )
{
  shr_error_e error_code;
  /*
   */
  error_code = _SHR_E_NONE;
  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    goto exit;
  }
  /*
   */
  Dnx_soc_sand_chip_descriptors[index].device_interrupt_mask_counter --;
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
}
/*
 */
uint32
  dnx_sand_is_chip_descriptor_chip_ver_bigger_eq(
    uint32   index,
    uint32  chip_ver_bigger_eq
  )
{
  unsigned
    int
      ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = FALSE ;
    goto exit ;
  }

#if FE200_REV_B_AS_REV_A
  /*
   * REV-B act as REV-A
   */
  if (Dnx_soc_sand_chip_descriptors[index].chip_type == 0xFE200)
  {
    ex = FALSE;
    goto exit;
  }
#endif

  if(Dnx_soc_sand_chip_descriptors[index].chip_ver >= chip_ver_bigger_eq)
  {
    ex = TRUE;
  }
  else
  {
    ex = FALSE;
  }

exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
#define SIMULATE_REV_B_ON_REV_A 0
uint32
  dnx_sand_get_chip_descriptor_chip_ver(
    uint32 index
  )
{
  uint32
      ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].chip_ver ;
#if SIMULATE_REV_B_ON_REV_A
  ex = 0x02/*FAP10M_EXPECTED_CHIP_VER_02*/;
#endif
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
uint32
  dnx_sand_get_chip_descriptor_dbg_ver(
    uint32 index
  )
{
  uint32
      ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].dbg_ver ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
uint32
  dnx_sand_get_chip_descriptor_chip_type(
    uint32 index
  )
{
  uint32
      ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].chip_type ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
uint32
  dnx_sand_get_chip_descriptor_logic_chip_type(
    uint32 index
  )
{
  uint32
      ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].logic_chip_type ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 */
void
  dnx_sand_set_chip_descriptor_valid(
    uint32 index
  )
{
  Dnx_soc_sand_chip_descriptors[index].valid_word = DNX_SAND_DEVICE_DESC_VALID_WORD ;
}
/*
 * TRUE if valid FALSE if invalid
 */
uint32
  dnx_sand_is_chip_descriptor_valid(
    uint32 index
  )
{
  uint32
    ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = FALSE ;
    goto exit ;
  }
  /*
   */
  if (DNX_SAND_DEVICE_DESC_VALID_WORD == Dnx_soc_sand_chip_descriptors[index].valid_word)
  {
    ex = TRUE ;
  }
  else
  {
    ex = FALSE ;
  }
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}
/*
 * Write flag indicating initialization stage of this device
 */
shr_error_e
  dnx_sand_set_chip_descriptor_device_at_init(
    uint32 index,
    uint32 val
  )
{
  shr_error_e error_code;
  error_code = _SHR_E_NONE;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL;
    goto exit;
  }
  if (val)
  {
    Dnx_soc_sand_chip_descriptors[index].device_at_init = TRUE ;
  }
  else
  {
    Dnx_soc_sand_chip_descriptors[index].device_at_init = FALSE ;
  }
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return (error_code) ;
}
/*
 * Read flag indicating initialization stage of this device
 */
shr_error_e
  dnx_sand_get_chip_descriptor_device_at_init(
    uint32 index,
    uint32 *val
  )
{
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    goto exit ;
  }
  *val = Dnx_soc_sand_chip_descriptors[index].device_at_init ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return (error_code) ;
}
/*
 * a unique uint32 for that instance of descriptor
 */
uint32
  dnx_sand_get_chip_descriptor_magic(
    uint32 index
  )
{
  uint32
    ex ;
  shr_error_e
    error_code = _SHR_E_NONE ;

  if (index >= Dnx_soc_sand_array_size)
  {
    error_code = _SHR_E_INTERNAL ;
    ex = 0 ;
    goto exit ;
  }
  ex = Dnx_soc_sand_chip_descriptors[index].magic ;
exit:
  SAND_IF_ERR_WARN(NO_UNIT, error_code);
  return ex ;
}

void
  dnx_sand_chip_descriptors_print(
    void
  )
{
  uint32
    descriptor_i;
  /*
   */
  if ( (0    == Dnx_soc_sand_array_mutex) ||
       (NULL == Dnx_soc_sand_chip_descriptors)
     )
  {
    LOG_CLI((BSL_META("dnx_sand_chip_descriptors(): do not have internal params (probably the driver was not started)\n\r")));
    goto exit;
  }
  /*
   */
  if (_SHR_E_NONE != dnx_sand_os_mutex_take(Dnx_soc_sand_array_mutex, DNX_SAND_INFINITE_TIMEOUT))
  {
    LOG_CLI((BSL_META("dnx_sand_chip_descriptors(): dnx_sand_os_mutex_take() failed\n\r")));
    goto exit ;
  }
  /*
   */
  for (descriptor_i=0; descriptor_i<Dnx_soc_sand_array_size; descriptor_i++)
  {
    if (0 == Dnx_soc_sand_chip_descriptors[descriptor_i].valid_word)
    {
      /*
       * No device registered here.
       */
      continue;
    }
    if (FALSE == dnx_sand_is_chip_descriptor_valid(descriptor_i))
    {
      /*
       * Error faound.
       */
      LOG_CLI((BSL_META("device[%2u] descriptor not valid\n\r"), descriptor_i));
      continue;
    }
    LOG_CLI((BSL_META("device[%2u] %s (0x%08X), chip_ver:%2u dbg_ver:%2u, \n\r"
"            address:0x%08X. Mutex: counter-%lu, owner-%p\n\r"),
             descriptor_i,
             dnx_sand_DEVICE_TYPE_to_str(
             Dnx_soc_sand_chip_descriptors[descriptor_i].logic_chip_type
             ),
             Dnx_soc_sand_chip_descriptors[descriptor_i].chip_type,
             Dnx_soc_sand_chip_descriptors[descriptor_i].chip_ver,
             Dnx_soc_sand_chip_descriptors[descriptor_i].dbg_ver,
             PTR_TO_INT(Dnx_soc_sand_chip_descriptors[descriptor_i].base_addr),
             (unsigned long)Dnx_soc_sand_chip_descriptors[descriptor_i].mutex_counter,
             Dnx_soc_sand_chip_descriptors[descriptor_i].mutex_owner
             ));
  }
  /*
   */
  if (_SHR_E_NONE != dnx_sand_os_mutex_give(Dnx_soc_sand_array_mutex))
  {
    LOG_CLI((BSL_META("dnx_sand_chip_descriptors(): dnx_sand_os_mutex_give() failed\n\r")));
    goto exit ;
  }
  /*
   */
exit:
  return;
}


