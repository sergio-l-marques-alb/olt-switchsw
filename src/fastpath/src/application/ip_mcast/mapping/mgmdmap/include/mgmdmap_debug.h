/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   mgmd_debug.h
*
* @purpose    MGMD Mapping layer internal function prototypes
*
* @component  MGMD Mapping Layer
*
* @comments   none
*
* @create     12/04/2006
*
* @author     gkiran
* @end
*
**********************************************************************/

#ifndef _INCLUDE_MGMD_DEBUG_H
#define _INCLUDE_MGMD_DEBUG_H

#include "mgmd_debug_api.h"
/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  MGMD_MAP_DEBUG_EVENTS = 0,/* Trace all the EVENT generations and receptions */
  MGMD_MAP_DEBUG_FAILURES, /* Trace all failures */
  MGMD_MAP_DEBUG_APIS,     /* Trace all the APIs invoked */
  MGMD_MAP_DEBUG_FLAG_LAST /* This should not be used by the code. It MUST   
                            * be the last element of this list and is meant  
                            * to indicate the number of enumerations defined 
                            */
} MGMD_MAP_DEBUG_FLAGS_t;

#define MGMD_MAX_MSG_SIZE           128
#define MGMD_MAP_NUM_FLAG_BYTES     ((MGMD_MAP_DEBUG_FLAG_LAST + 7) / 8)
#define MGMD_MAP_DEBUG_ALL_SET      0xFF
#define MGMD_MAP_FLAG_BITS_MAX      8
#define MGMD_MAP_FLAG_VALUE         1

#define MGMD_MAP_DEBUG(debugType, __fmt__, __args__...) \
 if(mgmdMapDebugFlagCheck(debugType) == L7_TRUE)        \
 {                                                      \
   L7_char8  __buf1__[MGMD_MAX_MSG_SIZE];               \
   L7_char8  __buf2__[MGMD_MAX_MSG_SIZE];               \
   (void)osapiSnprintf(__buf1__, MGMD_MAX_MSG_SIZE, __fmt__, ## __args__);       \
   (void)osapiSnprintf(__buf2__, MGMD_MAX_MSG_SIZE, "\nMGMDMAP: %s() - %s",         \
                 __FUNCTION__, __buf1__);               \
   sysapiPrintf(__buf2__);                              \
 }


#define MGMD_PKT_TYPE_STR_LEN 40
#define MGMD_USER_TRACE(__fmt__, __args__... )                              \
  LOG_USER_TRACE(L7_FLEX_MGMD_MAP_COMPONENT_ID, __fmt__,##__args__);        \

/**********************************************************************
                  Function Prototypes
**********************************************************************/
/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MGMDMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugDisable(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MGMDMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugEnable(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MGMDMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugAllSet(void);
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MGMDMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none 
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugAllReset(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MGMDMAP.
*
* @param    flag      @b{(input)}  Debug Flag to set.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugFlagSet(MGMD_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MGMDMAP.
*
* @param    flag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugFlagReset(MGMD_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MGMDMAP.
*
* @param    debugType  @b{(input)}  Debug Flag to check.
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdMapDebugFlagCheck(MGMD_MAP_DEBUG_FLAGS_t debugType);


/*============================================================================*/
/*===================  START OF MGMD PACKET DEBUG TRACES =====================*/
/*============================================================================*/


/*********************************************************************
*
* @purpose Trace mgmd packets received/transmitted
*
* @param   family	@b{(input)} Address family
* @param   rxTrace	@b{(input)} Receive trace or Transmit trace
* @param   rtrIfNum	@b{(input)} router Interface Number
* @param   src  	@b{(input)} IP Source address
* @param   dest 	@b{(input)} IP Destination address
* @param   payLoad	@b{(input)} Protocol packet offset
* @param   length	@b{(input)} Packet length
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void mgmdDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
                              L7_uint32 rtrIfNum, L7_inet_addr_t *src,
                              L7_inet_addr_t *dest, L7_uchar8 *payLoad, 
                              L7_uint32 length);
/*********************************************************************
* @purpose  Get the current status of displaying mgmd packet debug info
*            
* @param    familyType   @b{(input)} Address Family type
* @param    traceFlag    @b{(input)} type of trace flag
* @param    flag         @b{(output)} trace flag value
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugTraceFlagGet(L7_uchar8 family,
                              MGMD_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
* @param    familyType   @b{(input)} Address Family type
* @param    traceFlag    @b{(input)} type of trace flag
* @param    flag         @b{(input)} trace flag value
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugTraceFlagSet(L7_uchar8 family,
                              MGMD_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL flag);

/*********************************************************************
* @purpose  Save configuration settings for mgmd trace data
*            
* @param    void
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugPacketTraceFlagSave();
/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mgmdDebugCfgUpdate(void);

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mgmdDebugCfgRead(void);
/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mgmdDebugRegister(void);
/*********************************************************************
* @purpose  DeRegister to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mgmdDebugDeRegister(void);

#endif /* End of MGMD_DEBUG.H */


