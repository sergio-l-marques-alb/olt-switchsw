#include <unistd.h>
#include <stdint.h>

#include "ptin/shm_startup_api.h"

/* Shared memory descriptors */
static shm_startup_t shmem_app[SHMEM_APP_MAX];
static char *shmem_app_name[SHMEM_APP_MAX] = APP_NAME_LIST;

/**
 * Create SWDRV shared memory structure
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_swdrv_create(void)
{
  int32_t rv;

  shm_startup_init_t(&shmem_app[SHMEM_APP_SWDRV]);

  rv = shm_startup_create(&shmem_app[SHMEM_APP_SWDRV], shmem_app_name[SHMEM_APP_SWDRV]);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error creating shared memory: rv=%d", rv);
    return -1;
  }

  /* Booting stage */
  rv = shm_startup_write_status(&shmem_app[SHMEM_APP_SWDRV], SHM_STATUS_BOOTING);
  if (rv != 0)
  {
    shm_startup_close(&shmem_app[SHMEM_APP_SWDRV]);
    SHM_STARTUP_LOG_ERROR("Error setting default status: rv=%d", rv);
    return -1;
  }
  /* Booting stage */
  rv = shm_startup_write_extended_status(&shmem_app[SHMEM_APP_SWDRV], EXT_STATUS_BOOT_START);
  if (rv != 0)
  {
    shm_startup_close(&shmem_app[SHMEM_APP_SWDRV]);
    SHM_STARTUP_LOG_ERROR("Error setting default extended status: rv=%d", rv);
    return -1;
  }
  /* Version and release */
  rv = shm_startup_write_version_release(&shmem_app[SHMEM_APP_SWDRV], 4, 0);
  if (rv != 0)
  {
    shm_startup_close(&shmem_app[SHMEM_APP_SWDRV]);
    SHM_STARTUP_LOG_ERROR("Error setting version and release: rv=%d", rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("Success creating shared memory.");

  return 0;
}

/**
 * Close SWDRV shared memory
 * 
 * @param void 
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_swdrv_close(void)
{
  int32_t rv;

  /* Check if shared memory is open and mapped */
  if (!shm_startup_is_open(&shmem_app[SHMEM_APP_SWDRV]))
  {
    SHM_STARTUP_LOG_ERROR("Shared memory is not open!");
    return 0;
  }

  rv = shm_startup_close(&shmem_app[SHMEM_APP_SWDRV]);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error closing shared memory: rv=%d", rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("Success closing shared memory.");

  return 0;
}

/**
 * Set status value for SWDRV shared memory
 * 
 * @param status 
 * @param extended_status 
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_swdrv_status_set(uint32_t status, uint32_t extended_status)
{
  int32_t rv;

  /* Check if shared memory is open and mapped */
  if (!shm_startup_is_open(&shmem_app[SHMEM_APP_SWDRV]))
  {
    SHM_STARTUP_LOG_ERROR("Shared memory is not open!");
    return -1;
  }

  /* Update status */
  rv = shm_startup_write_status(&shmem_app[SHMEM_APP_SWDRV], status);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error updating shmem status to %u: rv=%d",
                          status, rv);
    return -1;
  }

  /* Update extended status */
  rv = shm_startup_write_extended_status(&shmem_app[SHMEM_APP_SWDRV], extended_status);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error updating shmem extended status to %u: rv=%d",
                          extended_status, rv);
    return -1;
  }

  /* If new status is OK, clear error */
  rv = shm_startup_write_error(&shmem_app[SHMEM_APP_SWDRV], SHM_STARTUP_ERROR_NONE);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error updating shmem error value: rv=%u", rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("Success updating shmem status values (status=%u ext_status=%u)",
                        status, extended_status);

  return 0;
}

/**
 * Set error value for SWDRV shared memory
 * 
 * @param error 
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_swdrv_error_set(uint32_t error)
{
  uint32_t status;
  int32_t rv;

  /* Check if shared memory is open and mapped */
  if (!shm_startup_is_open(&shmem_app[SHMEM_APP_SWDRV]))
  {
    SHM_STARTUP_LOG_ERROR("Shared memory is not open!");
    return -1;
  }

  /* Error to set is not null? */
  if (error != SHM_STARTUP_ERROR_NONE)
  {
    /* Update status to ERROR state */
    rv = shm_startup_write_status(&shmem_app[SHMEM_APP_SWDRV], SHM_STATUS_ERROR);
    if (rv != 0)
    {
      SHM_STARTUP_LOG_ERROR("Error updating shmem to ERROR status: rv=%d", rv);
      return -1;
    }
  }
  /* Error to set is null? (OK status) */
  else 
  {
    /* Read current status */
    rv = shm_startup_read_status(&shmem_app[SHMEM_APP_SWDRV], &status);
    if (rv != 0)
    {
      SHM_STARTUP_LOG_ERROR("Error reading shmem status: rv=%d", rv);
      return -1;
    }
    /* If current status is ERROR, update it to OK status */
    if (status == SHM_STATUS_ERROR)
    {
      rv = shm_startup_write_status(&shmem_app[SHMEM_APP_SWDRV], SHM_STATUS_OK);
      if (rv != 0)
      {
        SHM_STARTUP_LOG_ERROR("Error updating shmem to OK state: rv=%d", rv);
        return -1;
      }
    }
  }

  /* Update error value */
  rv = shm_startup_write_error(&shmem_app[SHMEM_APP_SWDRV], error);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error updating shmem error value! (rv=%u)", rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("Success updating shmem error value to %u", error);

  return 0;
}

/**
 * Open shared memories of other applications
 *  
 * @param app : Application (SHMEM_STARTUP_APP_t)  
 * @param tries : maximum number of tries to open each shmem (-1
 *              for infinite)
 * @param pause_time : time in seconds between tries 
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_app_open(SHMEM_STARTUP_APP_t app, 
                          int32_t tries, int32_t pause_time)
{
  int32_t i;
  int32_t rv;

  /* Validate app */
  if (app >= SHMEM_APP_MAX)
  {
    SHM_STARTUP_LOG_ERROR("Invalid app (%u)", app);
    return -1;
  }
  
  /* Check if shared memory is already open */
  if (shm_startup_is_open(&shmem_app[app]))
  {
    SHM_STARTUP_LOG_WARN("%s shmem is already open.", shmem_app_name[app]);
    return 0;
  }
  
  /* Open shared memory */
  shm_startup_init_t(&shmem_app[app]);

  i = 0;
  do
  {
    /* If opening our own shared mem, allow write permission */
    if (app == SHMEM_APP_SWDRV)
    {
      rv = shm_startup_open_rw(&shmem_app[app], shmem_app_name[app]);
    }
    else
    {
      rv = shm_startup_open(&shmem_app[app], shmem_app_name[app]);
    }
    /* If success, break loop */
    if (rv == 0)
    {
      break;
    }
    SHM_STARTUP_LOG_ERROR("Failed opening %s shmem: rv=%d",
                          shmem_app_name[app], rv);
    /* If another try is possible, wait some time */
    if ((tries < 0) || ((++i) < tries))
    {
      sleep(pause_time);
    }
  }
  while ((tries < 0) || (i < tries));

  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error opening %s shmem: rv=%d",
                          shmem_app_name[app], rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("%s shmem open.", shmem_app_name[app]);

  return 0;
}

/**
 * Close app shared memory
 * 
 * @param app : Application (SHMEM_STARTUP_APP_t) 
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_app_close(SHMEM_STARTUP_APP_t app)
{
  int32_t rv;

  /* Validate app */
  if (app >= SHMEM_APP_MAX)
  {
    SHM_STARTUP_LOG_ERROR("Invalid app (%u)", app);
    return -1;
  }

  /* Check if shared memory is already closed */
  if (!shm_startup_is_open(&shmem_app[app]))
  {
    SHM_STARTUP_LOG_WARN("%s shmem is already closed",
                         shmem_app_name[app]);
    return 0;
  }

  /* Close shared memory */
  rv = shm_startup_close(&shmem_app[app]);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error closing %s shmem: rv=%d",
                          shmem_app_name[app], rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("%s shmem closed.", shmem_app_name[app]);
  
  return 0;
}


/**
 * Check if APP shared memory is open
 *  
 * @param app : Application (SHMEM_STARTUP_APP_t) 
 *  
 * @return BOOL : TRUE:1 / FALSE:0
 */
unsigned char shm_startup_app_is_open(SHMEM_STARTUP_APP_t app)
{
  /* Validate app */
  if (app >= SHMEM_APP_MAX)
  {
    SHM_STARTUP_LOG_ERROR("Invalid app (%u)", app);
    return -1;
  }
  
  return shm_startup_is_open(&shmem_app[app]);
}


/**
 * Get data values from FWCTRL shared memory
 *  
 * @param app : Application (SHMEM_STARTUP_APP_t)  
 * @param status 
 * @param extended_status 
 * @param error 
 * 
 * @return int : RC_OK:0 / RC_FAILED:-1 
 */
int shm_startup_app_get(SHMEM_STARTUP_APP_t app,
                         uint32_t *status, uint32_t *extended_status,
                         uint32_t *error)
{
  uint32_t ret_status, ret_extended_status, ret_error;
  int32_t rv;

  /* Validate app */
  if (app >= SHMEM_APP_MAX)
  {
    SHM_STARTUP_LOG_ERROR("Invalid app (%u)", app);
    return -1;
  }

  /* Check if shared memory is open */
  if (!shm_startup_is_open(&shmem_app[app]))
  {
    SHM_STARTUP_LOG_ERROR("%s shmem is not open!", shmem_app_name[app]);
    return -1;
  }

  /* Read status, extended_status and error values */
  rv = shm_startup_read_status(&shmem_app[app], &ret_status);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error reading %s status value: rv=%d",
                          shmem_app_name[app], rv);
    return -1;
  }
  rv = shm_startup_read_extended_status(&shmem_app[app], &ret_extended_status);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error reading %s extended status value: rv=%d",
                          shmem_app_name[app], rv);
    return -1;
  }
  rv = shm_startup_read_error(&shmem_app[app], &ret_error);
  if (rv != 0)
  {
    SHM_STARTUP_LOG_ERROR("Error reading %s error value: rv=%d",
                          shmem_app_name[app], rv);
    return -1;
  }

  SHM_STARTUP_LOG_TRACE("Success reading %s shmem status values: "
                        "status=%u ext_status=%u error=%u",
                        shmem_app_name[app],
                        ret_status, ret_extended_status, ret_error);

  /* return values */
  if (status != NULL)
  {
    *status = ret_status;
  }
  if (extended_status != NULL)
  {
    *extended_status = ret_extended_status;
  }
  if (error != NULL)
  {
    *error = ret_error;
  }

  return 0;
}

