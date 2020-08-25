/* $Id: sand_module_management.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $
*/

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <shared/bsl.h>

#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Utils/sand_trace.h>

#include <soc/dnx/legacy/SAND/Management/sand_module_management.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_params.h>
#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>
#include <soc/dnx/legacy/SAND/Management/sand_callback_handles.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dnxc/legacy/error.h>

#define DNX_SAND_MODULE_MANAGEMENT_PRINTFS 0

/* $Id: sand_module_management.c,v 1.9 Broadcom SDK $
 * Static, general parameters of the DNX_SAND driver
 *
 * Dnx_soc_sand_start_module_shut_down_mutex
 * ------------------------------
 * the flag acts as test & set mutex making dnx_sand_module_open
 * and dnx_sand_module_close atomic. lets take dnx_sand_module_open() for instance:
 * at the begining of the method, under "disable intrrupts"
 * the first task to enter sets Inside dnx_sand_module_open to TRUE
 * (after checking that it was FALSE before, hence test & set)
 * ("disable interrupts" gurantees no context switching).
 * We set the flag back to FALSE before returning, after the
 * global Dnx_soc_sand_driver_is_started is alredy started, which will block
 * the rest of the tasks trying to start the driver when already started.
 * The same apply for dnx_sand_module_close()
 */
int Dnx_soc_sand_start_module_shut_down_mutex = FALSE ;

/*****************************************************
*NAME
* dnx_sand_module_init_all
*TYPE:
*  PROC
*DATE:
*  15-Jan-03
*FUNCTION:
*  Call to all files in DNX_SAND which have an initilization
*  function.
*INPUT:
*  DNX_SAND_DIRECT:
*    void -
*  DNX_SAND_INDIRECT:
*    None.
*OUTPUT:
*  DNX_SAND_DIRECT:
*    shr_error_e -
*      If not _SHR_E_NONE Error occurred in the opening of inner module.
*  DNX_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static
  shr_error_e
    dnx_sand_module_init_all(
      void
    )
{
  shr_error_e
    ex ;

  ex = dnx_sand_trace_init() ;

  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
*NAME
* dnx_sand_module_end_all
*TYPE:
*  PROC
*DATE:
*  15-Jan-03
*FUNCTION:
*  Call to all files in DNX_SAND which have an ending function.
*INPUT:
*  DNX_SAND_DIRECT:
*    void -
*  DNX_SAND_INDIRECT:
*    None.
*OUTPUT:
*  DNX_SAND_DIRECT:
*    shr_error_e -
*      If not _SHR_E_NONE Error occurred in the opening of inner module.
*  DNX_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static
  shr_error_e
    dnx_sand_module_end_all(
      void
    )
{
  shr_error_e
    ex ;

  ex = dnx_sand_trace_end() ;

  SAND_IF_ERR_WARN(NO_UNIT, ex);
  return ex ;
}
/*****************************************************
 * See details in dnx_sand_module_management.h
 *****************************************************/
shr_error_e
  dnx_sand_module_open(
    uint32      max_num_devices,
    uint32      system_tick_in_ms,
    uint32      soc_tcmtask_priority,
    uint32      min_time_between_tcm_activation,
    uint32      soc_tcmmockup_interrupts,
    int         error_handler,
    char              *error_description,
    uint32      *is_already_opened
    )
{
  shr_error_e err;
  DNX_SAND_INTERRUPT_INIT_DEFS;
  

  err = _SHR_E_NONE ;

  /*
   * see above for further explanations. This is the mechanism making sure
   * dnx_sand_module_open() is atomic
   */
  DNX_SAND_INTERRUPTS_STOP;
  {
    if (Dnx_soc_sand_start_module_shut_down_mutex)
    {
      err = _SHR_E_INTERNAL;
      goto exit_busy ;
    }
    Dnx_soc_sand_start_module_shut_down_mutex = TRUE ;
  }
  /*
   * we cannot start an already started driver
   * but we don't want to return an error on this.
   * meaning it's legal to try and open an already
   * openned driver (that way both fap & fe drivers
   * can try and open it without getting an error,
   * and with no regard to the order.
   */
  if (dnx_sand_general_get_driver_is_started())
  {
    if (is_already_opened)
    {
      *is_already_opened = _SHR_E_INTERNAL;
    }
    goto exit ;
  }
  else
  {
    if (is_already_opened)
    {
      *is_already_opened = _SHR_E_NONE ;
    }
  }

  /*
   * End all inner modules - if exist.
   */
  err =  dnx_sand_module_end_all() ;
  if (err != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * Initilaize all inner modules.
   */
  err = dnx_sand_module_init_all() ;
  if (err != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * setting global static parameter Max_num_devices
   */
  if (dnx_sand_general_set_max_num_devices(max_num_devices) != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * setting global static parameter System_tick_in_ms
   */
  if (dnx_sand_general_set_system_tick_in_ms(system_tick_in_ms) != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * setting global static parameter Tcm_task_priority
   */
  if (dnx_sand_general_set_tcm_task_priority(soc_tcmtask_priority) != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * setting global static parameter Min_time_between_polls
   */
  if (dnx_sand_general_set_min_time_between_tcm_activation(min_time_between_tcm_activation) != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * setting global static parameter Tcm_mockup_interrupts
   */
  if (dnx_sand_general_set_tcm_mockup_interrupts(soc_tcmmockup_interrupts) != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit;
  }
  /*
   * initilizes the aray of chip descriptors
   */
  if (dnx_sand_init_chip_descriptors(max_num_devices) != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * initilizes the linked list of callback handles
   */
  if (dnx_sand_handles_init_handles() != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * initilizes the Timed Callback Machine (TCM) including:
   * a task that wait on a message queue, and a delta-list.
   */
  /*
   * Clear the indirect information.
   */
  if (dnx_sand_indirect_clear_info_all() != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * Clear the interrupt mask address information.
   */
  if (dnx_sand_mem_interrupt_mask_address_clear_all() != _SHR_E_NONE)
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * after setting this flag the rest of the API can operate
   */
  dnx_sand_general_set_driver_is_started(TRUE) ;
exit:
  Dnx_soc_sand_start_module_shut_down_mutex = FALSE ;
exit_busy:
  if ( err != _SHR_E_NONE )
  {
    dnx_sand_module_end_all() ;
#if DNX_SAND_MODULE_MANAGEMENT_PRINTFS
    LOG_INFO(BSL_LS_SOC_MANAGEMENT,
             (BSL_META("dnx_sand_module_open(): failure. err = %d \r\n"), err));
#endif
  }
  else
  {
#if DNX_SAND_MODULE_MANAGEMENT_PRINTFS
    LOG_INFO(BSL_LS_SOC_MANAGEMENT,
             (BSL_META("dnx_sand_module_open(): success\r\n")));
#endif
  }
  SAND_IF_ERR_WARN(NO_UNIT, err);
  DNX_SAND_INTERRUPTS_START_IF_STOPPED;
  return (err) ;
}
/*****************************************************
 * See details in dnx_sand_module_management.h
 *****************************************************/
shr_error_e
  dnx_sand_module_close(
    void
  )
{
  shr_error_e err;
  DNX_SAND_INTERRUPT_INIT_DEFS;
  

  err = _SHR_E_NONE ;

  /*
   * see above for further explanations. This is the mechanism making sure
   * dnx_sand_module_close() is atomic
   */
  DNX_SAND_INTERRUPTS_STOP;
  {
    if (Dnx_soc_sand_start_module_shut_down_mutex)
    {
      err = _SHR_E_INTERNAL;
      goto exit ;
    }
    Dnx_soc_sand_start_module_shut_down_mutex = TRUE ;
  }

  /* If any devices are still registered, don't close */
  if (dnx_sand_is_any_chip_descriptor_taken()) {
      goto exit;
  }

  /*
   * We can only shut_down started drivers
   */
  if (!dnx_sand_general_get_driver_is_started())
  {
    err = _SHR_E_INTERNAL;
    goto exit ;
  }
  /*
   * Mark driver as shut down - no other API calls can enter
   */
  dnx_sand_general_set_driver_is_started(FALSE) ;

  DNX_SAND_INTERRUPTS_START_IF_STOPPED;
  /*
   * stop the TCM (delete task, empty the delta-list)
   */
  /*
   * delete the callback handles linked list
   */
  dnx_sand_handles_shut_down_handles() ;
  /*
   * clears the array of chip descriptors
   */
  dnx_sand_delete_chip_descriptors() ;
  /*
   * Setting the interrupt mask addresses to non-valid address
   */
  dnx_sand_mem_interrupt_mask_address_clear_all() ;
  /*
   * Clear the indirect information.
   */
  dnx_sand_indirect_clear_info_all() ;

  dnx_sand_module_end_all() ;
exit:
  Dnx_soc_sand_start_module_shut_down_mutex = FALSE ;
  SAND_IF_ERR_WARN(NO_UNIT, err);
  DNX_SAND_INTERRUPTS_START_IF_STOPPED;
  return (err) ;
}

