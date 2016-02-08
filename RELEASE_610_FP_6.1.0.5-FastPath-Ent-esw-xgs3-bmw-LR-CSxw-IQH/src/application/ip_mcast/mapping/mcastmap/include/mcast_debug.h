/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   mcast_debug.h
*
* @purpose    MCAST Mapping layer internal function prototypes
*
* @component  MCAST Mapping Layer
*
* @comments   none
*
* @create     13/04/2006
*
* @author     gkiran
* @end
*
**********************************************************************/


#ifndef _MCAST_MAP_DEBUG_H_
#define _MCAST_MAP_DEBUG_H_

#include "sysapi.h"
#include "mcast_debug_api.h"

/**********************************************************************
                 Typedefs and Defines
**********************************************************************/
typedef enum
{
  MCAST_MAP_DEBUG_FLAG_MIN,
  MCAST_MAP_DEBUG_FAILURES,  /* Trace all failures */
  MCAST_MAP_DEBUG_APIS,      /* Trace all the APIs invoked */
  MCAST_MAP_DEBUG_CONFIGURED,/* Trace all instances which configured **
                             ** already                             */
  MCAST_MAP_DEBUG_PKT_RCVR,  /* Trace all the APIs invoked */
  MCAST_MAP_DEBUG_FLAG_MAX/* This should not be used by the code. It MUST   **
                           ** be the last element of this list and is meant  **
                           ** to indicate the number of enumerations defined */
} MCAST_MAP_DEBUG_FLAGS_t;

#define MCAST_MAP_NUM_FLAG_BYTES     ((MCAST_MAP_DEBUG_FLAG_MAX + 7) / 8)
#define MCAST_MAP_DEBUG_ALL_SET      0xFF
#define MCAST_MAP_FLAG_BITS_MAX      8
#define MCAST_MAP_FLAG_VALUE         1

#define MCAST_MAP_MAX_DBG_ADDR_SIZE  IPV6_DISP_ADDR_LEN
#define MCAST_MAP_MAX_MSG_SIZE 256
#define MCAST_MAP_DEBUG_PRINTF sysapiPrintf

#define MCAST_MAP_DEBUG(debugType, __fmt__, __args__...) \
 if(mcastMapDebugFlagCheck(debugType) == L7_TRUE) \
 { \
     L7_char8  __buf1__[MCAST_MAP_MAX_MSG_SIZE];    \
     L7_char8  __buf2__[MCAST_MAP_MAX_MSG_SIZE];          \
     L7_timespec time;                              \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, MCAST_MAP_MAX_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, MCAST_MAP_MAX_MSG_SIZE,   \
               "\n%02d:%02d:%02d-MCAST-MAP: [%s-%d]: - %s",         \
       time.hours, time.minutes, time.seconds, __FUNCTION__, __LINE__, __buf1__);   \
        MCAST_MAP_DEBUG_PRINTF(__buf2__); \
 }

#define MCAST_MAP_DEBUG_ADDR(dbg_type, __str__, __xaddr__)  \
 {                                                    \
  if (mcastMapDebugFlagCheck(dbg_type) == L7_TRUE)  {              \
     L7_char8  __buf1__[MCAST_MAP_MAX_MSG_SIZE];    \
     L7_char8  __buf2__[MCAST_MAP_MAX_MSG_SIZE];          \
     L7_timespec time;                              \
     osapiUpTime(&time);                             \
     (void)inetAddrHtop((__xaddr__),__buf1__);               \
     (void)osapiSnprintf (__buf2__, MCAST_MAP_MAX_MSG_SIZE,   \
           "\n%02d:%02d:%02d MFC: In %s() -%s %s",        \
           time.hours, time.minutes, time.seconds, __FUNCTION__, __str__, __buf1__);   \
     MCAST_MAP_DEBUG_PRINTF(__buf2__);                                      \
  }                                                   \
 }


#define MCAST_USER_TRACE(__fmt__, __args__... )                              \
  LOG_USER_TRACE(L7_FLEX_MCAST_MAP_COMPONENT_ID, __fmt__,##__args__);        \

/**********************************************************************
                  Function Prototypes
**********************************************************************/
L7_RC_t mcastMapDebugDisable(void);

L7_RC_t mcastMapDebugEnable(void);

L7_RC_t mcastMapDebugAllSet(void);

L7_RC_t mcastMapDebugAllReset(void);

L7_RC_t mcastMapDebugFlagSet(MCAST_MAP_DEBUG_FLAGS_t flag);

L7_RC_t mcastMapDebugFlagReset(MCAST_MAP_DEBUG_FLAGS_t flag);

L7_BOOL mcastMapDebugFlagCheck(MCAST_MAP_DEBUG_FLAGS_t debugType);



/*============================================================================*/
/*===================  START OF MCAST PACKET DEBUG TRACES =====================*/
/*============================================================================*/

/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
*
* @param   family	@b{(input)} Address Family
* @param   rxTrace	@b{(input)} Receive trace or Transmit trace
* @param   rtrIfNum	@b{(input)} router Interface Number
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
void mcastDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
                               L7_uint32 rtrIfNum, L7_uchar8 *payLoad,
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
L7_RC_t mcastDebugTraceFlagGet(L7_uchar8 family,
                               MCAST_DEBUG_TRACE_FLAG_t traceFlag,
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
L7_RC_t mcastDebugTraceFlagSet(L7_uchar8 family,
                               MCAST_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);

/*********************************************************************
* @purpose  Save configuration settings for mcast trace data
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
L7_RC_t mcastDebugPacketTraceFlagSave();
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
void mcastDebugCfgUpdate(void);

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
void mcastDebugCfgRead(void);
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
void mcastDebugRegister(void);
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
void mcastDebugDeRegister(void);

/*********************************************************************
* @purpose  Registered support debug dump routine for general multicast issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastMapDebugMulticastInfoDumpRegister(void);

#endif /* _MCAST_MAP_DEBUG_H_ */
