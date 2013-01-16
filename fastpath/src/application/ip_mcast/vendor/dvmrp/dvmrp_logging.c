/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   dvmrp_debug.c
*
* @purpose    Implements the Debug support functionality DVMRP module
*
* @component  DVMRP
*
* @comments   none
*
* @create     April 20,2006
*
* @author     Prakash/Shashidhar.
* @end
*
**********************************************************************/
/**********************************************************************
                  Includes
***********************************************************************/
#include <string.h>
#include "log.h"
#include "datatypes.h"
#include "sysapi.h"
#include "dvmrp_logging.h"
#include "dvmrp_debug.h"

/**********************************************************************
                  Typedefs & Defines
***********************************************************************/
#define DVMRP_NUM_FLAG_BYTES     ((DVMRP_DEBUG_FLAG_LAST + 7) / 8)


/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[DVMRP_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

/*****************************************************************
    Function Definitions
******************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the DVMRP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dvmrpDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the DVMRP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dvmrpDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of DVMRP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dvmrpDebugAllSet(void)
{
  memset(debugFlags, 0xFF, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of DVMRP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dvmrpDebugAllReset(void)
{
  memset(debugFlags, 0, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in DVMRP.
*
* @param    flag   -  @b{(input)} : Debug Flags
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes    None 
*
* @end
*********************************************************************/
L7_RC_t dvmrpDebugFlagSet(DVMRP_DEBUG_FLAGS_t flag)
{
  if (flag >= DVMRP_DEBUG_FLAG_LAST)
  {
    return L7_FAILURE;
  }
  debugFlags[flag/8] |= (1 << (flag % 8));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in DVMRP.
*
* @param    flag   -  @b{(input)} : Debug Flags
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dvmrpDebugFlagReset(DVMRP_DEBUG_FLAGS_t flag)
{
  if (flag >= DVMRP_DEBUG_FLAG_LAST)
  {
    return L7_FAILURE;
  }
  debugFlags[flag/8] &= (~(1 << (flag % 8)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MFC.
*
* @param    dbg_type   -  @b{(input)} : Debug Flags Type
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL dvmrpDebugFlagCheck(DVMRP_DEBUG_FLAGS_t dbg_type)
{
  if (debugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  }
  if (dbg_type >= DVMRP_DEBUG_FLAG_LAST)
  {
    return L7_FALSE;
  }
  if ((debugFlags[dbg_type/8] & (1 << (dbg_type % 8))) != 0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current MFC Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrpDebugFlagShow(void)
{
  L7_uint32 i;

  if (debugEnabled == L7_TRUE)
  {
    sysapiPrintf(" DVMRP Debugging : Enabled\n");
    for (i = 0;  i < DVMRP_DEBUG_FLAG_LAST ; i ++)
    {
      if (dvmrpDebugFlagCheck(i) == L7_TRUE)
      {
        sysapiPrintf("     debugFlag [%d] : %s\n", i , "Enabled");
      }
      else
      {
        sysapiPrintf("     debugFlag [%d] : %s\n", i , "Disabled");
      }
    }
  }
  else
  {
    sysapiPrintf (" DVMRP Debugging : Disabled\n");
  }
}


