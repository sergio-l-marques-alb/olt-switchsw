/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   bspapi.h
*
* @purpose    BSP API
*
* @component  bsp component
*
* @comments   none
*
* @create     05/02/2006
*
* @author     Brady Rutherford
*
* @end
*             
**********************************************************************/

#ifndef BSP_API_H
#define BSP_API_H

#include <time.h>

#include "l7_common.h"

#define L7_SERVICE_PORT_MAX_LENGTH  12
/*
 * filesystem path info
 */
#define CONFIG_PATH     "/usr/local/ptin/log/fastpath/"
#define EXEC_PATH       "/usr/local/ptin/sbin/"
#ifdef _L7_OS_VXWORKS_
  #define DOWNLOAD_PATH   "/download"
  #define DOWNLOAD_CODE_FILE  "code.stk"
#else
  #define DOWNLOAD_PATH   "/tmp/"
#endif

/* Name of the update script.
 * */
#define UPDATE_SCRIPT_NAME    "UPDATE"
#define UPDATE_KERNEL_SCRIPT_NAME     "UPDATE_KERNEL"
#define UPDATE_BOOTROM_SCRIPT_NAME    "UPDATE_BOOTCODE"
#define VPD_FILE                      "fastpath.vpd"
#define USYSTEM_FILE                  "/tmp/uSystem"

/**************************************************************************
*
* Serial Port parameter types.
*
* @notes For SYSAPISERIALPORT_CHARSIZE, valid values are between L7_SERIALPORT_MIN_CHAR_SIZE and L7_SERIALPORT_MAX_CHAR_SIZE.
*        For SYSAPISERIALPORT_PARITYTYPE, specify either L7_PARITY_EVEN, L7_PARITY_ODD, L7_DISABLE.
*        For SYSAPISERIALPORT_STOPBITS, specify either L7_STOP_BIT_1 or L7_STOP_BIT_2.
*
*************************************************************************/
typedef enum
{

    SYSAPISERIALPORT_CHARSIZE,
    SYSAPISERIALPORT_FLOWCONTROL,
    SYSAPISERIALPORT_PARITYTYPE,
    SYSAPISERIALPORT_STOPBITS

} SYSAPISERIALPORT_PARMS_t;


/*********************************************************************
* @purpose  Returns the IPL Model Tag for this unit
*
* @returns  IPL_MODEL_TAG
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiIplModelGet(void);

/*********************************************************************
* @purpose  Returns the Default Baud Rate for this unit
*
* @returns  L7_DEFAULT_BAUDRATE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiDefaultBaudRateGet(void);

/*********************************************************************
* @purpose  Returns the Service Port Name for this unit
*
* @returns  Service port name.
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_char8 *bspapiServicePortNameGet(void);

/**************************************************************************
*
* @purpose  Retrieve a serial port setting
*
* @param    parm specifies which serial port parameter to retrieve
* @param    result ptr where to place parameter setting
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if parm is not supported or recognized
*
* @end
*
*************************************************************************/
L7_RC_t bspapiSerialPortParmGet ( L7_uint32 parm, L7_uint32 *result );

/**************************************************************************
*
* @purpose  Enables the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogEnable( void );

/**************************************************************************
*
* @purpose  Disables the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogDisable( void );

/**************************************************************************
*
* @purpose  Services the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogService( void );

/**************************************************************************
*
* @purpose  Retrieve the Hardware Watchdog Service Interval
*
* @param    void
*
* @returns  Service interval in milliseconds
*
* @end
*
*************************************************************************/
int bspapiWatchdogInterval( void );

/*********************************************************************
* @purpose  Dump BSP debug information
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t bspapiDebugRegisteredSysInfoDump(void);

/***************************************************************************
*
* @purpose Return file descriptor for the console port.
*
* @comments Return file descriptor for the console port.
*
* @returns file descriptor for the console port.
*
* @end
*
***************************************************************************/
int bspapiConsoleFdGet(void);
int bspapiConsoleFdOutGet(void);

/***************************************************************************
*
* @purpose Sets file descriptor for the console port.
*
* @comments Sets file descriptor for the console port.
*
* @returns L7_SUCCESS
*
* @end
*
***************************************************************************/
L7_RC_t bspapiConsoleFdSet(int consoleFd);

/*********************************************************************
* @purpose Read time from the RTC
*
* @param    time ptr where to write time values
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t bspapiRTCRead(struct tm *rtcTime);

/**************************************************************************
*
* @purpose  Crash box with a given task ID.
*
* @param    taskID - task to save context for.
*
* @returns  Does not return
*
* @end
*
*************************************************************************/
void bspapiCrash( int taskID );

/**************************************************************************
*
* @purpose  Reset the switch
*
* @param    none.
*
* @returns  none.
*
* @end
*
*************************************************************************/
void bspapiSwitchReset (void);

/***************************************************************************
*
* @purpose Returns the Board ID for the CPU.
*
* @param    none.
*
* @returns Board ID
*
* @end
*
***************************************************************************/
/* Physical CPU cards */
#define CARD_CPU_BMW_REV_1_ID               0x82450000
#define CARD_CPU_DNI_REV_1_ID               0x85410000
#define CARD_CPU_ALPHA_REV_1_ID             0x82A10000
#define CARD_CPU_RAPTOR_REV_1_ID            0x56218000
#define CARD_CPU_CFM_REV_1_ID               0x11250000
#define CARD_CPU_NSX_REV_1_ID               0x11250000
#define CARD_CPU_LM_REV_1_ID                0x47040000
#define CARD_CPU_JAG_REV_1_ID               0x47040000
#define CARD_CPU_LINE_REV_1_ID              0x17010000
#define CARD_CPU_GTX_REV_1_ID               0x11250002
#define CARD_CPU_GTO_REV_1_ID               0x85480000

L7_uint32 bspapiCpuBoardIdGet(void);

#define START_OPR_CODE                  1
#define START_BOOT_MENU                 2
#define START_DIAGS                     3
#define START_OPR_CODE_PASSWD_RECOVERY  4
#define START_VXSHELL                   5
/**************************************************************************
*
* @purpose  Returns the startType for how this code was started.
*
* @param    
*
* @returns  startType
*
* @end
*
*************************************************************************/
L7_uint32 bspapiStartTypeGet();

#define BOOTCFG_FILE_NAME     "boot.cfg"

#endif /* BSP_API_H */
