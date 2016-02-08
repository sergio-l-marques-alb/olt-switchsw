/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename    sys_debug.c
*
* @purpose     system debug utilities
*
* @component 
*
* @comments 
*
* @create      10/27/2003
*
* @author      wjacobs
* @end
*
**********************************************************************/


#include "sys_debug_api.h"
#include "osapi.h"
#include "log.h"
#include "sysapi.h"

static L7_uint32 lastTraceMsecTime = 0;

void timeMillisecondsTraceTest(void);


/*********************************************************************
* @purpose  This function creates and initializes a trace block
*          
* @param 	 logEntry   @b{(input)}   L7_TRUE, if output is to be logged
*                                     Otherwise, output is printed
*	
* @returns  void
*
* @notes  

*
* @end
*
*********************************************************************/
void timeMillisecondsTrace(L7_BOOL logEntry)
{
  L7_uint32 currTime;

  currTime = osapiTimeMillisecondsGet();

  if (logEntry == L7_TRUE)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "Milliseconds passed since system reset: [%10.10u] \n", currTime);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "Milliseconds passed since last checked: %d \n", (currTime - lastTraceMsecTime) );
  }
  else
  {
      LOG_MSG("Milliseconds passed since system reset: [%10.10u] \n", currTime);
      LOG_MSG("Milliseconds passed since last checked: %d \n", (currTime - lastTraceMsecTime) );
  }

}


/*********************************************************************
* @purpose  This function forces inclusion of trace function in symbol table
*          
* @param 	 void
*	
* @returns  void
*
* @notes  
*
* @end
*
*********************************************************************/
void timeMillisecondsTraceTest(void)
{

  timeMillisecondsTrace(L7_TRUE);

}



