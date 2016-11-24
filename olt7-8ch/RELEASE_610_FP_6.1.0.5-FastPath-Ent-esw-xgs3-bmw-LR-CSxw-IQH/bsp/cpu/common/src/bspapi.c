/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   bspapi.c
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

#include <stdio.h>

#include "l7_common.h"
#include "platform_cpu.h"
#include "bspapi.h"
#include "bspcpu_api.h"

#ifdef _L7_OS_VXWORKS_
unsigned char ServicePortName[L7_SERVICE_PORT_MAX_LENGTH] = SERVICE_PORT_NAME;
#else
extern unsigned char ServicePortName[L7_SERVICE_PORT_MAX_LENGTH];
#endif
extern void print_vpd_in_flash ();
/*********************************************************************
* @purpose  Returns the IPL Model Tag for this unit
*
* @returns  IPL_MODEL_TAG
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiIplModelGet(void)
{
  return IPL_MODEL_TAG;
}

/*********************************************************************
* @purpose  Returns the Default Baud Rate for this unit
*
* @returns  L7_DEFAULT_BAUDRATE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiDefaultBaudRateGet(void)
{
  return L7_DEFAULT_BAUDRATE;
}

/*********************************************************************
* @purpose  Returns the Service Port Name for this unit
*
* @returns  L7_FAILURE, no service port
* @returns  L7_SUCCESS, name of service port
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_char8 *bspapiServicePortNameGet(void)
{
  return ServicePortName;
}

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
L7_RC_t bspapiSerialPortParmGet ( L7_uint32 parm, L7_uint32 *result )
{
  L7_RC_t rc = L7_SUCCESS;

  switch ( parm )
  {
    case SYSAPISERIALPORT_CHARSIZE :
      *result = 8;
      break;

    case SYSAPISERIALPORT_FLOWCONTROL :
      *result = ( L7_uint32 )(L7_DISABLE);
      break;

    case SYSAPISERIALPORT_PARITYTYPE :
      *result = ( L7_uint32 )(L7_PARITY_NONE);
      break;

    case SYSAPISERIALPORT_STOPBITS :
      *result = ( L7_uint32 )(L7_STOP_BIT_1);  
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

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
L7_RC_t bspapiWatchdogEnable( void )
{
  SYS_WATCHDOG_ENABLE();

  return L7_SUCCESS;
}

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
L7_RC_t bspapiWatchdogDisable( void )
{
  SYS_WATCHDOG_RESET();

  return L7_SUCCESS;
}

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
L7_RC_t bspapiWatchdogService( void )
{
  SYS_WATCHDOG_SERVICE();

  return L7_SUCCESS;
}

/*********************************************************************
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
int bspapiWatchdogInterval( void )
{
  return WATCHDOG_TIMER_INTERVAL;
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_RC_t bspapiDebugRegisteredSysInfoDump(void)
{
  printf("\r\n");
  printf("/*=====================================================================*/\n");
  printf("/*                  BSP  INFORMATION                                   */\n");
  printf("/*=====================================================================*/\n");


  printf("\r\n");
  printf("\n");

  print_vpd_in_flash();

  printf("\r\n");
  printf("\n");

  return L7_SUCCESS;
}

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
extern L7_uint32 cpu_card_id;
L7_uint32 bspapiCpuBoardIdGet(void)
{
  return cpu_card_id;
}

