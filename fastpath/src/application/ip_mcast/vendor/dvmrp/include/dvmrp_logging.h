/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dvmrp_logging.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (dvmrp_logging.c).
*
* @component Distance Vector Multicast Routing Protocol(DVMRP)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the DVMRP component itself.
*            All other components MUST use the functionalities exported
*            by the dvmrp_api.h file only.
*
* @create    April 20, 2006
*
* @author    Dasoju Shashidhar.
* @end
*
**********************************************************************/
#ifndef _DVMRP_LOGGING_H
#define _DVMRP_LOGGING_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l7_common.h"

/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  DVMRP_DEBUG_INFO = 0,   /* Just to print information required*/
  DVMRP_DEBUG_RX,         /* Trace the complete Receive Data path */
  DVMRP_DEBUG_TX,         /* Trace the complete Transmission Data path */
  DVMRP_DEBUG_EVENTS,     /* Trace all the EVENT generations and receptions */
  DVMRP_DEBUG_GROUP,      /* Trace all the EVENT generations and receptions */
  DVMRP_DEBUG_NOCACHE,    /* Trace all the EVENT generations and receptions */
  DVMRP_DEBUG_ROUTE,      /* Trace all the EVENT generations and receptions */
  DVMRP_DEBUG_TIMERS,     /* Trace all Timer activities */
  DVMRP_DEBUG_FAILURES,   /* Trace all failures */
  DVMRP_DEBUG_GRAFT,      /* Trace all grafts */
  DVMRP_DEBUG_PROBE,      /* Trace all probes */
  DVMRP_DEBUG_APIS,       /* Trace all the APIs invoked */
  DVMRP_DEBUG_FLAG_LAST   /* This should not be used by the code. It MUST be the
                           last element of this list and is meant to indicate
                           the number of enumerations defined */
} DVMRP_DEBUG_FLAGS_t;

#define DVMRP_DEBUG(dbg_type, __fmt__, __args__...) \
 if(dvmrpDebugFlagCheck(dbg_type) == L7_TRUE) \
 { \
   sysapiPrintf(__fmt__, ## __args__); \
 }

/*   L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_DVMRP_MAP_COMPONENT_ID, __fmt__, ## __args__); \*/
/**********************************************************************
                  Function Prototypes
**********************************************************************/

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
L7_RC_t dvmrpDebugEnable(void);

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
L7_RC_t dvmrpDebugDisable(void);

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
L7_RC_t dvmrpDebugAllSet(void);

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
L7_RC_t dvmrpDebugAllReset(void);

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
L7_RC_t dvmrpDebugFlagSet(DVMRP_DEBUG_FLAGS_t flag);

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
L7_RC_t dvmrpDebugFlagReset(DVMRP_DEBUG_FLAGS_t flag);

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
L7_BOOL dvmrpDebugFlagCheck(DVMRP_DEBUG_FLAGS_t dbg_type);

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
void dvmrpDebugFlagShow(void);

/*********************************************************************
*
* @purpose  Shows usage of the DVMRP Debug utility
*
* @param    None.
*
* @returns  None.
*
* @notes  None
*
* @end
*********************************************************************/
void dvmrpDebugHelp(void);

#endif /* _DVMRP_LOGGING_H */
