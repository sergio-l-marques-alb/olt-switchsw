/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsm_map_debug.h
*
* @purpose    PIM-SM Mapping layer internal function prototypes
*
* @component  PIM-SM Mapping Layer
*
* @comments   none
*
* @create     13/04/2006
*
* @author     gkiran
* @end
*
**********************************************************************/


#ifndef _PIMSM_MAP_DEBUG_H_
#define _PIMSM_MAP_DEBUG_H_

#include "pimsm_debug_api.h"
         
/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  PIMSM_MAP_DEBUG_EVENTS=0,  /* Trace all the EVENT generations and **
                             ** receptions */
  PIMSM_MAP_DEBUG_FAILURES,  /* Trace all failures */
  PIMSM_MAP_DEBUG_APIS,      /* Trace all the APIs invoked */
  PIMSM_MAP_DEBUG_CONFIGURED,/* Trace all instances which configured **
                             ** alrealdy                             */
  PIMSM_MAP_DEBUG_FLAG_LAST/* This should not be used by the code. It MUST   **
                           ** be the last element of this list and is meant  **
                           ** to indicate the number of enumerations defined */
} PIMSM_MAP_DEBUG_FLAGS_t;

#define PIMSM_MAP_MAX_MSG_SIZE       256
#define PIMSM_MAP_NUM_FLAG_BYTES     ((PIMSM_MAP_DEBUG_FLAG_LAST + 7) / 8)
#define PIMSM_MAP_DEBUG_ALL_SET      0xFF
#define PIMSM_MAP_FLAG_BITS_MAX      8
#define PIMSM_MAP_FLAG_VALUE         1

#define PIMSM_MAP_DEBUG(debugType, __fmt__, __args__...) \
 if(pimsmMapDebugFlagCheck(debugType) == L7_TRUE)        \
 {                                                       \
   L7_char8  __buf1__[PIMSM_MAP_MAX_MSG_SIZE];               \
   L7_char8  __buf2__[PIMSM_MAP_MAX_MSG_SIZE];               \
   (void)osapiSnprintf(__buf1__, PIMSM_MAP_MAX_MSG_SIZE, __fmt__, ## __args__);  \
   (void)osapiSnprintf(__buf2__, PIMSM_MAP_MAX_MSG_SIZE, "\nPIMSMMAP: %s() - %s", \
                 __FUNCTION__, __buf1__);                \
   sysapiPrintf(__buf2__);                               \
 }

#define PIMSM_MAP_LOG_MSG(familyType, __fmt__, __args__...) \
 do \
 {\
   L7_char8  __buf1__[PIMSM_MAP_MAX_MSG_SIZE];               \
   L7_char8  __buf2__[PIMSM_MAP_MAX_MSG_SIZE];               \
   L7_char8  __buf3__[PIMSM_MAP_MAX_MSG_SIZE] ="PIMSM_MAPv4:";               \
   L7_char8  __buf4__[PIMSM_MAP_MAX_MSG_SIZE] ="PIMSM_MAPv6:";               \
   (void)osapiSnprintf(__buf1__, PIMSM_MAP_MAX_MSG_SIZE, __fmt__, ## __args__); \
   if(familyType == L7_AF_INET)        \
   {                                                       \
     (void)osapiSnprintf(__buf2__, PIMSM_MAP_MAX_MSG_SIZE, "\n%s:%s",         \
                 __buf3__, __buf1__);                \
   } \
   else if(familyType == L7_AF_INET6)        \
   {                                                       \
     (void)osapiSnprintf(__buf2__, PIMSM_MAP_MAX_MSG_SIZE, "\n %s - %s",       \
                 __buf4__, __buf1__);                \
   }\
   else \
   {     \
     L7_char8 __buf5__[PIMSM_MAP_MAX_MSG_SIZE] ="PIMSM_MAP"; \
     (void)osapiSnprintf(__buf2__, PIMSM_MAP_MAX_MSG_SIZE, "\n %s - %s",       \
                 __buf5__, __buf1__);                \
   }      \
   LOG_MSG(__buf2__);                               \
 }while(0) 

#define PIMSM_PKT_TYPE_STR_LEN 40
#define PIMSM_USER_TRACE(__fmt__, __args__... )                              \
  LOG_USER_TRACE(L7_FLEX_PIMDM_MAP_COMPONENT_ID, __fmt__,##__args__);        \

/**********************************************************************
                  Function Prototypes
**********************************************************************/
/*********************************************************************
*
* @purpose  Disable Debug Tracing for the PIMSMMAP.
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
L7_RC_t pimsmMapDebugDisable(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the PIMSMMAP.
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
L7_RC_t pimsmMapDebugEnable(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of PIMSMMAP.
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
L7_RC_t pimsmMapDebugAllSet(void);
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of PIMSMMAP.
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
L7_RC_t pimsmMapDebugAllReset(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in PIMSMMAP.
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
L7_RC_t pimsmMapDebugFlagSet(PIMSM_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in PIMSMMAP.
*
* @param    debugFlag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDebugFlagReset(PIMSM_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in PIMSMMAP.
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
L7_BOOL pimsmMapDebugFlagCheck(PIMSM_MAP_DEBUG_FLAGS_t debugType);


/*********************************************************************
* @purpose  Dumps the PIM-SM configuration information onto the
*           display screen.
*
* @param    none 
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCfgDataShow(void);
/*********************************************************************
* @purpose  Dumps the PIM-SM configuration information onto the
*           display screen.
*
* @param    none 
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsm6MapCfgDataShow(void);


/*============================================================================*/
/*===================  START OF PIMSM PACKET DEBUG TRACES =====================*/
/*============================================================================*/


/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
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
void pimsmDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
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
L7_RC_t pimsmDebugTraceFlagGet(L7_uchar8 family,
                               PIMSM_DEBUG_TRACE_FLAG_t traceFlag,
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
L7_RC_t pimsmDebugTraceFlagSet(L7_uchar8 family,
                               PIMSM_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);
/*********************************************************************
* @purpose  Save configuration settings for pimsm trace data
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
L7_RC_t pimsmDebugPacketTraceFlagSave();
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
void pimsmDebugCfgUpdate(void);

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
void pimsmDebugCfgRead(void);
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
void pimsmDebugRegister(void);
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
void pimsmDebugDeRegister(void);


#endif /* _PIMSM_MAP_DEBUG_H_ */
