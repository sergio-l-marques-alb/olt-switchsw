/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mgmd_proxy_debug.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (mgmd_proxy_debug.c).
*
* @component Multicast Group Membership Discovery Proxy (MGMD_PROXY)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the MGMD component itself.
*            All other components MUST use the functionalities exported
*            by the mgmd_api.h file only.
*
* @create    Dec 27, 2006
*
* @author    ddevi
* @end
*
**********************************************************************/
#ifndef _MGMD_PROXY_DEBUG_H
#define _MGMD_PROXY_DEBUG_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l7_common.h"

/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  MGMD_PROXY_DEBUG_RX = 1,     /* Trace the complete Receive Data path */
  MGMD_PROXY_DEBUG_TX,         /* Trace the complete Transmission Data path */
  MGMD_PROXY_DEBUG_EVENTS,     /* Trace all the EVENT generations and receptions */
  MGMD_PROXY_DEBUG_TIMERS,     /* Trace all Timer activities */
  MGMD_PROXY_DEBUG_FAILURE,    /* Trace all failures */
  MGMD_PROXY_DEBUG_APIS,       /* Trace all the APIs invoked */
  MGMD_PROXY_DEBUG_REPORTS,
  MGMD_PROXY_DEBUG_QUERY,
  MGMD_PROXY_DEBUG_FLAG_LAST   /* This should not be used by the code. It MUST be the
                           last element of this list and is meant to indicate
                           the number of enumerations defined */
} MGMD_PROXY_DEBUG_FLAGS_t;

#define MGMD_PROXY_DBG_MSG_SIZE           128
#define MGMD_PRINTF sysapiPrintf

#define MGMD_PROXY_DEBUG(dbg_type, __fmt__, __args__...) \
{\
  if(mgmdProxyDebugFlagCheck(dbg_type) == L7_TRUE)        \
  {                                                       \
    L7_char8  __buf1__[MGMD_PROXY_DBG_MSG_SIZE];               \
    L7_char8  __buf2__[MGMD_PROXY_DBG_MSG_SIZE];               \
    (void)osapiSnprintf(__buf1__, MGMD_PROXY_DBG_MSG_SIZE, __fmt__, ## __args__);        \
    (void)osapiSnprintf(__buf2__, MGMD_PROXY_DBG_MSG_SIZE, "\n MGMD_PROXY : %s() , %d - %s ",         \
                  __FUNCTION__, __LINE__, __buf1__);                \
    sysapiPrintf(__buf2__);                               \
  } \
}                                                           

#define MGMD_PROXY_DEBUG_ADDR(dbg_type, __str__, __xaddr__)  \
{\
  if(L7_TRUE == mgmdProxyDebugFlagCheck(dbg_type))  \
  {\
    L7_char8  __buf1__[MGMD_PROXY_DBG_MSG_SIZE];                \
    L7_char8  __buf2__[MGMD_PROXY_DBG_MSG_SIZE];                \
    (void)inetAddrHtop((__xaddr__),__buf1__);                               \
    (void)osapiSnprintf(__buf2__, MGMD_PROXY_DBG_MSG_SIZE, "\n%s %d: - %s : %s",                            \
                  __FUNCTION__, __LINE__,  __str__, __buf1__);   \
    sysapiPrintf(__buf2__);                               \
  }\
}                                  

#define MGMD_PROXY_DEBUG_BITS(dbg_type, __str__, __xaddr__) \
{ \
  if(L7_TRUE == mgmdProxyDebugFlagCheck(dbg_type))  \
  {\
    L7_char8  __buf1__[MGMD_PROXY_DBG_MSG_SIZE];               \
    (void)osapiSnprintf(__buf1__, MGMD_PROXY_DBG_MSG_SIZE, "\n %s , %d : %s == \n",      \
                        __FUNCTION__, __LINE__, __str__);        \
     BITX_DEBUG_PRINT(__xaddr__); \
  }\
}

/**********************************************************************
                  Function Prototypes
**********************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmdProxyDebugEnable(void);

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmdProxyDebugDisable(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmdProxyDebugAllSet(void);

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmdProxyDebugAllReset(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MGMD_PROXY.
*
* @param    flag    @b{(input)}   Flags to be Set
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmdProxyDebugFlagSet(MGMD_PROXY_DEBUG_FLAGS_t flag);

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MGMD_PROXY.
*
* @param    flag    @b{(input)}   Flags to be Cleared
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmdProxyDebugFlagReset(MGMD_PROXY_DEBUG_FLAGS_t flag);

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MGMD_PROXY.
*
* @param    dbg_type  @b{(input)}   Flag to be Checked
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL mgmdProxyDebugFlagCheck(MGMD_PROXY_DEBUG_FLAGS_t dbg_type);

/*********************************************************************
*
* @purpose  Shows the current MGMD_PROXY Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
extern void mgmdProxyDebugFlagShow(void);

/*********************************************************************
*
* @purpose  Shows usage of the MGMD_PROXY Debug utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
extern void mgmdProxyDebugHelp(void);

#endif /* _MGMD_PROXY_DEBUG_H */

