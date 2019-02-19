/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  
* LL   VV  VV LL   7   77   
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      
* </pre>
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
* @create    01/18/2001
*
* @author    paulq 
*
* @end
*
*********************************************************************/

#ifndef PLATFORM_CPU_H_INCLUDED
#define PLATFORM_CPU_H_INCLUDED


/* Bulk Storage Types - Linux platforms use MTD partitions for flash */
/* access so they should register PLATFORM_FLASH_TYPE as NON_FLASH   */
#define PLATFORM_FLASH_TYPE NON_FLASH


/**************************************************************************
* The BMW card has a DTC1743 Real time clock with 8K NVRAM. FastPath has  *
* datastructures stored at the following locations:                       *
***************************************************************************/
#define L7_ENV_BUFFER_OFFSET      0x0000  /* U-Boot environment variables */
#define L7_LOG_BUFFER_OFFSET      0x0450  /* 64 bytes for last error code */
#define L7_VPD_BUFFER_OFFSET      0x1000  /* 256 bytes for VPD            */


/* Name of the operational code file.
*/
#define CODE_FILE_NAME   "code.bin"
#define XCODE_FILE_NAME  "xcode.bin" 

/* Name of the FASTPATH VPD file.
*/
#define VPD_FILE_NAME          "fastpath.vpd"



#define IPL_MODEL_TAG ((unsigned long) 0x00508270) /* All Broadcom 8270 boards*/
        
#define SYS_WATCHDOG_RESET()
#define SYS_WATCHDOG_ENABLE()
#define SYS_WATCHDOG_SERVICE()

#define WATCHDOG_TIMER_INTERVAL  (15*1000)  /* 15 seconds */


/* Start of Serial Port default characteristcs                                */

#define L7_DEFAULT_BAUDRATE               L7_BAUDRATE_9600
#define L7_DEFAULT_SERIALPORT_PARITY      L7_PARITY_NONE
#define L7_DEFAULT_SERIALPORT_STOPBITS    L7_STOP_BIT_1
#define L7_DEFAULT_SERIALPORT_FLOWCONTROL L7_DISABLE
#define L7_DEFAULT_SERIALPORT_CHARSIZE    8

#endif
