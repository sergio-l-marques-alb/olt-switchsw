#ifndef _SHM_STARTUP_API__H_
#define _SHM_STARTUP_API__H_

#include "logger.h"
#include "globaldefs.h"
#include "nbtools/shm_startup.h"

/**************************************************** 
 * In this section adapt this code to your project
 ****************************************************/

/* MACROS to abstract loggings */
#define SHM_STARTUP_LOG_TRACE(fmt, args...) \
        xLOG_TRACE(LOG_CTX_STARTUP, fmt, ##args)
#define SHM_STARTUP_LOG_WARN(fmt, args...) \
        xLOG_WARN(LOG_CTX_STARTUP, fmt, ##args)
#define SHM_STARTUP_LOG_ERROR(fmt, args...) \
        xLOG_ERROR(LOG_CTX_STARTUP, fmt, ##args)

/* MACRO to validate an API call (for callers with a rc return) */
#define SHM_STARTUP_API_CHECK_LOG(api_function) \
  {                                           \
    int rc;                                   \
    rc = api_function;                        \
    if (rc != 0) /* RC_OK */                  \
    {                                         \
      SHM_STARTUP_LOG_ERROR("Error with " #api_function " : rc=%d", rc); \
    }                                         \
    else                                      \
    {                                         \
      SHM_STARTUP_LOG_TRACE("Success executing " #api_function); \
    }                                         \
  }

/* MACRO to validate an API call (for callers with a rc return) */
#define SHM_STARTUP_API_CHECK_RET(api_function)   \
  {                                           \
    int rc;                                   \
    rc = api_function;                        \
    if (rc != 0) /* RC_OK */                  \
    {                                         \
      SHM_STARTUP_LOG_ERROR("Error with " #api_function " : rc=%d", rc); \
      return (rc);                            \
    }                                         \
    else                                      \
    {                                         \
      SHM_STARTUP_LOG_TRACE("Success executing " #api_function); \
    }                                         \
  }

/* MACRO to validate an API call (for callers with a void return) */
#define SHM_STARTUP_API_CHECK_VOID(api_function) \
  {                                           \
    int rc;                                   \
    rc = api_function;                        \
    if (rc != 0) /* RC_OK */                  \
    {                                         \
      SHM_STARTUP_LOG_ERROR("Error with " #api_function " : rc=%d", rc); \
      return;                                 \
    }                                         \
    else                                      \
    {                                         \
      SHM_STARTUP_LOG_TRACE("Success executing " #api_function); \
    }                                         \
  }

/* MACRO to validate an API call (for callers with a rc return) */
#define SHM_STARTUP_API_CHECK_EXIT(api_function) \
  {                                           \
    int rc;                                   \
    rc = api_function;                        \
    if (rc != 0) /* RC_OK */                  \
    {                                         \
      SHM_STARTUP_LOG_ERROR("Error with " #api_function " : rc=%d", rc); \
      exit (rc);                              \
    }                                         \
    else                                      \
    {                                         \
      SHM_STARTUP_LOG_TRACE("Success executing " #api_function); \
    }                                         \
  }


/**************************************************** 
 * Constants definition
 ****************************************************/

/* List of applications */
typedef enum {
  SHMEM_APP_SWDRV = 0,
  SHMEM_APP_HWINIT,
  SHMEM_APP_PHYCTRL,
  SHMEM_APP_FWCTRL,
  SHMEM_APP_OLTD,
  SHMEM_APP_OPENSAF,
  SHMEM_APP_MAX
} SHMEM_STARTUP_APP_t;

/* Application names */
#define APP_NAME_LIST { \
    "/shmem_switch",  \
    "/shmem_hwinit",  \
    "/shmem_phyctrl", \
    "/shmem_fwctrl",  \
    "/shmem_oltd",    \
    "/shmem_opensaf", \
}

enum {
  EXT_STATUS_BOOT_START             = 0,
  EXT_STATUS_BOOT_MEMMAP_STAGE      = 1,
  EXT_STATUS_BOOT_INIT_STAGE        = 2,
  EXT_STATUS_BOOT_SOC_INIT_STAGE    = 3,
  EXT_STATUS_BOOT_BCM_PROBE_STAGE   = 4,
  EXT_STATUS_BOOT_BCM_DETECT_STAGE  = 5,
  EXT_STATUS_BOOT_BCM_SW1_ATTACH    = 6,
  EXT_STATUS_BOOT_BCM_SW2_ATTACH    = 7,
  EXT_STATUS_BOOT_BCM_SW1_INIT      = 8,
  EXT_STATUS_BOOT_BCM_SW2_INIT      = 9,
  EXT_STATUS_BOOT_BCM_MMU_INIT      = 10,
  EXT_STATUS_BOOT_BCM_POST_INIT     = 11,
  EXT_STATUS_BOOT_BCMX_INIT         = 12,
  EXT_STATUS_BOOT_BCM_RPC_INIT      = 13,
  EXT_STATUS_BOOT_BCM_DEFCONFIG_INIT= 14,
  EXT_STATUS_BOOT_BCM_FINAL_INIT    = 15,
  EXT_STATUS_BOOT_SEM_QUEUES_INIT   = 16,
  EXT_STATUS_BOOT_MISC_INIT         = 17,
  EXT_STATUS_BOOT_NETWORK_INIT      = 18,
  EXT_STATUS_BOOT_DTL_INIT          = 19,
  EXT_STATUS_BOOT_CNFGR_INIT        = 20,
  EXT_STATUS_BOOT_CNFGR_PHASE1_INIT = 21,
  EXT_STATUS_BOOT_CNFGR_PHASE2_INIT = 22,
  EXT_STATUS_BOOT_CNFGR_PHASE3_INIT = 23,
  EXT_STATUS_BOOT_CNFGR_POST_INIT   = 24,
  EXT_STATUS_BOOT_PTIN_INIT         = 25,
  EXT_STATUS_WAIT_APPS_TO_START     = 100,
  EXT_STATUS_WAIT_HWINIT_STATUS_OK  = 110,
  EXT_STATUS_WAIT_PHYCTRL_STATUS_OK = 115,
  EXT_STATUS_WAIT_FWCTRL_STATUS_OK  = 120,
  EXT_STATUS_WAIT_OPENSAF_STATUS_OK = 125,
  EXT_STATUS_NORMAL_OPERATION       = 300,
  EXT_STATUS_REBOOT                 = 500,
  EXT_STATUS_REBOOT_AND_FLUSH       = 555
};

enum {
  SHM_STARTUP_ERROR_NONE            = 0,
  SHM_STARTUP_ERROR_MEMMAP          = 1,
  SHM_STARTUP_ERROR_INIT_STAGE      = 2,
  SHM_STARTUP_ERROR_SOC_INIT        = 3,
  SHM_STARTUP_ERROR_BCM_PROBE       = 4,
  SHM_STARTUP_ERROR_BCM_DETECTION   = 5,
  SHM_STARTUP_ERROR_BCM_ATTACH      = 6,
  SHM_STARTUP_ERROR_BCM_INIT        = 8,
  SHM_STARTUP_ERROR_BCM_MMU         = 10,
  SHM_STARTUP_ERROR_BCM_POST_INIT   = 11,
  SHM_STARTUP_ERROR_BCMX_INIT       = 12,
  SHM_STARTUP_ERROR_MEMORY          = 16,
  SHM_STARTUP_ERROR_MISC            = 17,
  SHM_STARTUP_ERROR_CNFGR_INIT      = 20,
  SHM_STARTUP_ERROR_PTIN_INIT       = 21,
  SHM_STARTUP_ERROR_APPS_MISSING    = 100,
  SHM_STARTUP_ERROR_APPS_STATUS_NOK = 101,
  SHM_STARTUP_ERROR_NO_DDRAM_TUNE_FILE = 200,
};


/* FWCTRL EXTENDED STATUS FLAG */
enum {
    EXT_STATUS_FWCTRL_CHECKPOINT_START= 101
};

/**************************************************** 
 * Prototypes
 ****************************************************/

/**
 * Create SWDRV shared memory structure
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1
 */
extern 
int shm_startup_swdrv_create(void);

/**
 * Close SWDRV shared memory
 * 
 * @param void 
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1 
 */
extern 
int shm_startup_swdrv_close(void);

/**
 * Set status value for SWDRV shared memory
 * 
 * @param status 
 * @param extended_status 
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1 
 */
extern 
int shm_startup_swdrv_status_set(uint32_t status, uint32_t extended_status);

/**
 * Set error value for SWDRV shared memory
 * 
 * @param error 
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1 
 */
extern 
int shm_startup_swdrv_error_set(uint32_t error);

/**
 * Open shared memories of other applications
 *  
 * @param app : Application (SHMEM_STARTUP_APP_t)  
 * @param tries : maximum number of tries to open each shmem (-1
 *              for infinite)
 * @param pause_time : time in seconds between tries 
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1 
 */
extern 
int shm_startup_app_open(SHMEM_STARTUP_APP_t app, 
                          int32_t tries, int32_t pause_time);

/**
 * Close app shared memory
 * 
 * @param app : Application (SHMEM_STARTUP_APP_t) 
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1 
 */
extern 
int shm_startup_app_close(SHMEM_STARTUP_APP_t app);

/**
 * Check if APP shared memory is open
 *  
 * @param app : Application (SHMEM_STARTUP_APP_t) 
 *  
 * @return BOOL : TRUE:1 / FALSE:0
 */
extern 
unsigned char shm_startup_app_is_open(SHMEM_STARTUP_APP_t app);

/**
 * Get data values from FWCTRL shared memory
 *  
 * @param app : Application (SHMEM_STARTUP_APP_t)  
 * @param status 
 * @param extended_status 
 * @param error 
 * 
 * @return RC_t : RC_OK:0 / RC_FAILED:-1 
 */
extern 
int shm_startup_app_get(SHMEM_STARTUP_APP_t app,
                            uint32_t *status, uint32_t *extended_status,
                            uint32_t *error);

#endif /*_SHM_STARTUP_API__H_*/
