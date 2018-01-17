/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename  platform_cpu.h
*
* @purpose   
*
* @component hardware
*
* @comments  
*
* @create    03/21/2008
*
* @author    cmutter
*
* @end
*
*********************************************************************/

#ifndef PLATFORM_CPU_H_INCLUDED
#define PLATFORM_CPU_H_INCLUDED

/* No watchdog timer */
#define SYS_WATCHDOG_RESET()
#define SYS_WATCHDOG_ENABLE()
#define SYS_WATCHDOG_SERVICE()
#define WATCHDOG_TIMER_INTERVAL  (15*1000)  /* 15 seconds */

/* This should be a unique ID for this platform. A unique ID will prevent */
/* downloads for the wrong architecture from updating the Boot ROM, thus  */
/* rendering the unit unrecoverable.                                      */
#define IPL_MODEL_TAG   ((unsigned long) 0x00502040)

#define LOG_NVM_FILE     "logNvmSave.bin"

extern unsigned char ServicePortName[];
#define SERVICE_PORT_NAME      ServicePortName
#define SERVICE_PORT_UNIT      0

/* Start of Serial Port default characteristcs                                */

#define L7_DEFAULT_BAUDRATE               L7_BAUDRATE_9600
#define L7_DEFAULT_SERIALPORT_PARITY      L7_PARITY_NONE
#define L7_DEFAULT_SERIALPORT_STOPBITS    L7_STOP_BIT_1
#define L7_DEFAULT_SERIALPORT_FLOWCONTROL L7_DISABLE
#define L7_DEFAULT_SERIALPORT_CHARSIZE    8

/* End of Serial Port default characteristcs                                  */

#define DIM_CFG_FILENAME                "boot.dim"  

#define MAX_BOOT_IMAGES                 2
#define DIM_IMAGE1_NAME                 "image1"
#define DIM_IMAGE2_NAME                 "image2"

#define DIM_BACKUP_IMAGE                "backup"
#define DIM_ACTIVE_IMAGE                "active"

#endif
