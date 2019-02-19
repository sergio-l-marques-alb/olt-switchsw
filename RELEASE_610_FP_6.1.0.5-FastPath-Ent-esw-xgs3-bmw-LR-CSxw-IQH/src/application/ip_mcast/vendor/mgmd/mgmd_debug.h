/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mgmd_debug.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (mgmd_debug.c).
*
* @component Multicast Group Membership Discovery (MGMD)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the MGMD component itself.
*            All other components MUST use the functionalities exported
*            by the mgmd_api.h file only.
*
* @create    April 25, 2006
*
* @author    Kamlesh Agrawal
* @end
*
**********************************************************************/
#ifndef _MGMD_DEBUG_H
#define _MGMD_DEBUG_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l7_common.h"
#include "mgmd.h"

/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  MGMD_DEBUG_RX = 1,     /* Trace the complete Receive Data path */
  MGMD_DEBUG_TX,         /* Trace the complete Transmission Data path */
  MGMD_DEBUG_EVENTS,     /* Trace all the EVENT generations and receptions */
  MGMD_DEBUG_TIMERS,     /* Trace all Timer activities */
  MGMD_DEBUG_FAILURE,    /* Trace all failures */
  MGMD_DEBUG_APIS,       /* Trace all the APIs invoked */
  MGMD_DEBUG_REPORTS,
  MGMD_DEBUG_QUERY,
  MGMD_DEBUG_HANDLE,
  MGMD_DEBUG_FLAG_LAST   /* This should not be used by the code. It MUST be the
                           last element of this list and is meant to indicate
                           the number of enumerations defined */
} MGMD_DEBUG_FLAGS_t;

#define MGMD_DEBUG(dbg_type, __fmt__, __args__...) \
{\
  if(L7_TRUE == mgmdDebugFlagCheck(dbg_type)) \
  { \
    L7_uchar8  __buf1__[MGMD_DBG_MSG_SIZE]; \
    L7_uchar8  __buf2__[MGMD_DBG_MSG_SIZE]; \
    (void)osapiSnprintf(__buf1__, MGMD_DBG_MSG_SIZE, __fmt__, ## __args__); \
    (void)osapiSnprintf(__buf2__, MGMD_DBG_MSG_SIZE, "\nLINE: %d: In %s - %s", \
                    __LINE__, __FUNCTION__, __buf1__); \
    sysapiPrintf(__buf2__); \
  }\
}                                                           

#define MGMD_DEBUG_ADDR(dbg_type, __str__, __xaddr__)  \
{\
  if(L7_TRUE == mgmdDebugFlagCheck(dbg_type))  \
  {\
    L7_char8  __buf1__[MGMD_DBG_MSG_SIZE];                \
    L7_char8  __buf2__[MGMD_DBG_MSG_SIZE];                \
    (void)inetAddrHtop((__xaddr__),__buf1__);                               \
    (void)osapiSnprintf(__buf2__, MGMD_DBG_MSG_SIZE, "\nLINE: %d: In %s - %s : %s",                            \
                  __LINE__, __FUNCTION__, __str__, __buf1__);   \
    sysapiPrintf(__buf2__); \
  }\
}                                  

/**********************************************************************
                  Function Prototypes
**********************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MGMD.
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
extern L7_RC_t mgmdDebugEnable(void);

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MGMD.
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
extern L7_RC_t mgmdDebugDisable(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MGMD.
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
extern L7_RC_t mgmdDebugAllSet(void);

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MGMD.
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
extern L7_RC_t mgmdDebugAllReset(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MGMD.
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
extern L7_RC_t mgmdDebugFlagSet(MGMD_DEBUG_FLAGS_t flag);

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MGMD.
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
extern L7_RC_t mgmdDebugFlagReset(MGMD_DEBUG_FLAGS_t flag);

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MGMD.
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
extern L7_BOOL mgmdDebugFlagCheck(MGMD_DEBUG_FLAGS_t dbg_type);

/*********************************************************************
*
* @purpose  Shows the current MGMD Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
extern void mgmdDebugFlagShow(void);

/*********************************************************************
*
* @purpose  Shows usage of the MGMD Debug utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
extern void mgmdDebugHelp(void);

#endif /* _MGMD_DEBUG_H */
