/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimdm_map_debug.h
*
* @purpose    PIMDM Mapping layer internal function prototypes
*
* @component  PIMDM Mapping Layer
*
* @comments   none
*
* @create     12/04/2006
*
* @author     gkiran
* @end
*
**********************************************************************/
#ifndef _INCLUDE_PIMDM_DEBUG_H
#define _INCLUDE_PIMDM_DEBUG_H

#include "pimdm_debug_api.h"
/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  PIMDM_MAP_DEBUG_EVENTS = 0, /* Trace all the EVENT generations and **
                              ** receptions                          */
  PIMDM_MAP_DEBUG_FAILURES,   /* Trace all failures */
  PIMDM_MAP_DEBUG_APIS,     /* Trace all the APIs invoked */
  PIMDM_MAP_DEBUG_FLAG_LAST /* This should not be used by the code. It MUST   **
                           ** be the last element of this list and is meant  **
                           ** to indicate the number of enumerations defined */
} PIMDM_MAP_DEBUG_FLAGS_t;

#define PIMDM_MAX_MSG_SIZE           256
#define PIMDM_MAP_NUM_FLAG_BYTES     ((PIMDM_MAP_DEBUG_FLAG_LAST + 7) / 8)
#define PIMDM_MAP_DEBUG_ALL_SET      0xFF
#define PIMDM_MAP_FLAG_BITS_MAX      8
#define PIMDM_MAP_FLAG_VALUE         1

#define PIMDM_MAP_DEBUG(debugType, __fmt__, __args__...)                       \
 if(pimdmMapDebugFlagCheck(debugType) == L7_TRUE)                              \
 {                                                                             \
   L7_char8  __buf1__[PIMDM_MAX_MSG_SIZE];                                     \
   L7_char8  __buf2__[PIMDM_MAX_MSG_SIZE];                                     \
   (void)osapiSnprintf(__buf1__, PIMDM_MAX_MSG_SIZE, __fmt__, ## __args__);    \
   (void)osapiSnprintf(__buf2__, PIMDM_MAX_MSG_SIZE, "\nPIMDMMAP: %s() - %s",  \
                       __FUNCTION__, __buf1__);                                \
   sysapiPrintf(__buf2__);                                                     \
 }

#define PIMDM_MAP_LOG_MSG(familyType, __fmt__, __args__...) \
 do \
 {\
   L7_char8  __buf1__[PIMDM_MAX_MSG_SIZE];               \
   L7_char8  __buf2__[PIMDM_MAX_MSG_SIZE];               \
   L7_char8  __buf3__[PIMDM_MAX_MSG_SIZE] ="PIMDM_MAPv4:";               \
   L7_char8  __buf4__[PIMDM_MAX_MSG_SIZE] ="PIMDM_MAPv6:";               \
   (void)osapiSnprintf(__buf1__,PIMDM_MAX_MSG_SIZE, __fmt__, ## __args__);     \
   if(familyType == L7_AF_INET)        \
   {                                                       \
     (void)osapiSnprintf(__buf2__,PIMDM_MAX_MSG_SIZE, "\n%s:%s",         \
                 __buf3__, __buf1__);                \
   } \
   else if(familyType == L7_AF_INET6)        \
   {                                                       \
     (void)osapiSnprintf(__buf2__,PIMDM_MAX_MSG_SIZE, "\n %s - %s",         \
                 __buf4__, __buf1__);                \
   }\
   else \
   {     \
     L7_char8 __buf5__[PIMDM_MAX_MSG_SIZE] ="PIMDM_MAP"; \
     (void)osapiSnprintf(__buf2__,PIMDM_MAX_MSG_SIZE, "\n %s - %s",         \
                 __buf5__, __buf1__);                \
   }      \
   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID, __buf2__);                               \
 }while(0)


#define PIMDM_PKT_TYPE_STR_LEN 40
#define PIMDM_USER_TRACE(__fmt__, __args__... )                              \
  LOG_USER_TRACE(L7_FLEX_PIMDM_MAP_COMPONENT_ID, __fmt__,##__args__);        \

/**********************************************************************
                  Function Prototypes
**********************************************************************/
/*********************************************************************
*
* @purpose  Disable Debug Tracing for the PIMDMMAP.
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
L7_RC_t pimdmMapDebugDisable(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the PIMDMMAP.
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
L7_RC_t pimdmMapDebugEnable(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of PIMDMMAP.
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
L7_RC_t pimdmMapDebugAllSet(void);
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of PIMDMMAP.
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
L7_RC_t pimdmMapDebugAllReset(void);
/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in PIMDMMAP.
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
L7_RC_t pimdmMapDebugFlagSet(PIMDM_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in PIMDMMAP.
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
L7_RC_t pimdmMapDebugFlagReset(PIMDM_MAP_DEBUG_FLAGS_t flag);
/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in PIMDMMAP.
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
L7_BOOL pimdmMapDebugFlagCheck(PIMDM_MAP_DEBUG_FLAGS_t debugType);


/*============================================================================*/
/*===================  START OF PIMDM PACKET DEBUG TRACES =====================*/
/*============================================================================*/


/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
*
* @param   family   @b{(input)} Address family
* @param   rxTrace  @b{(input)} Receive trace or Transmit trace
* @param   rtrIfNum @b{(input)} router Interface Number
* @param   src      @b{(input)} IP Source address
* @param   dest     @b{(input)} IP Destination address
* @param   payLoad  @b{(input)} Protocol packet offset
* @param   length   @b{(input)} Packet length
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void pimdmDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
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
L7_RC_t pimdmDebugTraceFlagGet(L7_uchar8 family,
                               PIMDM_DEBUG_TRACE_FLAG_t traceFlag,
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
L7_RC_t pimdmDebugTraceFlagSet(L7_uchar8 family,
                               PIMDM_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);
/*********************************************************************
* @purpose  Save configuration settings for pimdm trace data
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
L7_RC_t pimdmDebugPacketTraceFlagSave();
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
void pimdmDebugCfgUpdate(void);

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
void pimdmDebugCfgRead(void);
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
void pimdmDebugRegister(void);
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
void pimdmDebugDeRegister(void);


#endif /* End of PIMDM_MAP_DEBUG.H */


