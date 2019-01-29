/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dvmrp_map_debug.h
*
* @purpose    DVMRP Mapping layer internal function prototypes
*
* @component  DVMRP Mapping Layer
*
* @comments   none
*
* @create     13/04/2006
*
* @author     gkiran
* @end
*
**********************************************************************/


#ifndef _DVMRP_MAP_DEBUG_H_
#define _DVMRP_MAP_DEBUG_H_

#include "dvmrp_debug_api.h"
/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  DVMRP_MAP_DEBUG_RX = 0,    /* Trace the complete Receive Data path */
  DVMRP_MAP_DEBUG_TX,        /* Trace the complete Transmission Data path */
  DVMRP_MAP_DEBUG_EVENTS,    /* Trace all the EVENT generations and **
                             ** receptions */
  DVMRP_MAP_DEBUG_FAILURES,  /* Trace all failures */
  DVMRP_MAP_DEBUG_APIS,      /* Trace all the APIs invoked */
  DVMRP_MAP_DEBUG_CONFIGURED,/* Trace all instances which configured **
                             ** alrealdy                             */
  DVMRP_MAP_DEBUG_FLAG_LAST/* This should not be used by the code. It MUST   **
                           ** be the last element of this list and is meant  **
                           ** to indicate the number of enumerations defined */
} DVMRP_MAP_DEBUG_FLAGS_t;

#define DVMRP_MAX_MSG_SIZE           128
#define DVMRP_MAP_NUM_FLAG_BYTES     ((DVMRP_MAP_DEBUG_FLAG_LAST + 7) / 8)
#define DVMRP_MAP_DEBUG_ALL_SET      0xFF
#define DVMRP_MAP_FLAG_BITS_MAX      8
#define DVMRP_MAP_FLAG_VALUE         1

#define DVMRP_MAP_DEBUG(debugType, __fmt__, __args__...) \
 if(dvmrpMapDebugFlagCheck(debugType) == L7_TRUE)        \
 {                                                       \
   L7_char8  __buf1__[DVMRP_MAX_MSG_SIZE];               \
   L7_char8  __buf2__[DVMRP_MAX_MSG_SIZE];               \
   (void)osapiSnprintf(__buf1__,DVMRP_MAX_MSG_SIZE, __fmt__, ## __args__);        \
   (void)osapiSnprintf(__buf2__,DVMRP_MAX_MSG_SIZE, "\nDVMRPMAP: %s() - %s",         \
                 __FUNCTION__, __buf1__);                \
   sysapiPrintf(__buf2__);                               \
 }

#define DVMRP_PKT_TYPE_STR_LEN 40
#define DVMRP_USER_TRACE(__fmt__, __args__... )                              \
  LOG_USER_TRACE(L7_FLEX_DVMRP_MAP_COMPONENT_ID, __fmt__,##__args__);        \

/**********************************************************************
                  Function Prototypes
**********************************************************************/
/*********************************************************************
*
* @purpose  Disable Debug Tracing for the DVMRPMAP.
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
L7_RC_t dvmrpMapDebugDisable(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the DVMRPMAP.
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
L7_RC_t dvmrpMapDebugEnable(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of DVMRPMAP.
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
L7_RC_t dvmrpMapDebugAllSet(void);
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of DVMRPMAP.
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
L7_RC_t dvmrpMapDebugAllReset(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in DVMRPMAP.
*
* @param    flag      @b{(input)}  Debug Flag to set.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugFlagSet(DVMRP_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in DVMRPMAP.
*
* @param    flag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugFlagReset(DVMRP_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in DVMRPMAP.
*
* @param    debugType  @b{(input)}  Debug Flag to check.
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapDebugFlagCheck(DVMRP_MAP_DEBUG_FLAGS_t debugType);


/*============================================================================*/
/*===================  START OF DVMRP PACKET DEBUG TRACES =====================*/
/*============================================================================*/

/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
*
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
void dvmrpDebugPacketRxTxTrace(L7_BOOL rxTrace,
                              L7_uint32 rtrIfNum, L7_inet_addr_t *src,
                              L7_inet_addr_t *dest, L7_uchar8 *payLoad, 
                              L7_uint32 length);
/*********************************************************************
* @purpose  Get the current status of displaying mgmd packet debug info
*            
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
L7_RC_t dvmrpDebugTraceFlagGet(DVMRP_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
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
L7_RC_t dvmrpDebugTraceFlagSet(DVMRP_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);


/*********************************************************************
* @purpose  Save configuration settings for dvmrp trace data
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
L7_RC_t dvmrpDebugPacketTraceFlagSave();
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
void dvmrpDebugCfgUpdate(void);

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
void dvmrpDebugCfgRead(void);
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
void dvmrpDebugRegister(void);
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
void dvmrpDebugDeRegister(void);

#endif /* _DVMRP_MAP_DEBUG_H_ */
