/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    dot3ad_debug.h
* @purpose     802.3ad link aggregation, LAC debug functions
* @component   dot3ad
* @comments    none
* @create      08/02/2006
* @author      cpverne
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef DOT3AD_DEBUG_H
#define DOT3AD_DEBUG_H

#include "l7_common.h"
#include "dot3ad_include.h"
#include "log.h"

extern L7_BOOL dot3adPacketDebugFlag;
extern L7_uint32 dot3adNsfDebugFlag ;

#define DOT3AD_USER_TRACE(__fmt__, __args__... )                              \
          if (dot3adDebugPacketTraceFlag == L7_TRUE)                          \
          {                                                                 \
            LOG_USER_TRACE(L7_DOT3AD_COMPONENT_ID, __fmt__,##__args__);        \
          }

typedef struct
{
  L7_uchar8   handle;  /* 00   : trace buffer handle */
  L7_uchar8   compId;  /* 01   : component id        */
  L7_ushort16 traceId;
  L7_uint32   timeStamp;
  L7_uint32   task;   

}dot3adTraceHeader_t;

typedef struct 
{
  L7_uint32 traceCode;
  L7_uint32 aggId;
} dot3adAggTrace_t;

typedef struct 
{
  L7_uint32 intfEvent;
  L7_uint32 intIfNum;
} dot3adIntfTrace_t;

typedef struct
{
  L7_ushort16   traceCode;
  L7_ushort16   aggId;
  L7_ushort16   actorPortNum;
  L7_ushort16   partnerPortNum;
  L7_uchar8   actorState;
  L7_uchar8   partnerState;
  L7_uchar8   selected;
  L7_uchar8   rxState;
  L7_uchar8   muxState;
  L7_uchar8   perState;
} dot3adPortTrace_t;

typedef struct 
{
  dot3adTraceHeader_t hdr;
  /*Header End*/
  union {
    dot3adAggTrace_t aggTrace;
    dot3adPortTrace_t portTrace;
    dot3adIntfTrace_t intfTrace;
  }u;
  
}dot3adTrace_t; 

#define DOT3AD_PORT_TRACEID          0xF1
#define DOT3AD_AGG_TRACEID           0xF2
#define DOT3AD_NIMEVENTS_TRACEID     0xF3


#define DOT3AD_TRACE_RX_INIT          0xA0
#define DOT3AD_TRACE_RX_DISABLE       0xA1
#define DOT3AD_TRACE_RX_EXPIRED       0xA2
#define DOT3AD_TRACE_RX_LACP_DISABLED 0xA3
#define DOT3AD_TRACE_RX_DEFAULTED     0xA4
#define DOT3AD_TRACE_RX_CURRENT       0xA5
#define DOT3AD_TRACE_RX_CURRWHILE_EXP 0xA6

#define DOT3AD_TRACE_PER_NO_PERIODIC   0xB0
#define DOT3AD_TRACE_PER_FAST_PERIODIC 0xB1
#define DOT3AD_TRACE_PER_SLOW_PERIODIC 0xB2
#define DOT3AD_TRACE_RX_PERWHILE_EXP   0xB6

#define DOT3AD_TRACE_MUX_DETACHED     0xC0
#define DOT3AD_TRACE_MUX_WAITING      0xC1
#define DOT3AD_TRACE_MUX_ATTACHED     0xC2
#define DOT3AD_TRACE_MUX_COLLDIST     0xC3
#define DOT3AD_TRACE_MUX_DETACHED     0xC4
#define DOT3AD_TRACE_MUX_DETACHED     0xC5
/*#define DOT3AD_TRACE_MUX_STATIC       0xC6*/
#define DOT3AD_TRACE_MUX_CD_EN_FAIL   0xCE
#define DOT3AD_TRACE_MUX_CD_DIS_FAIL  0xCF
#define DOT3AD_TRACE_RX_WAITWHILE_EXP 0xC6

#define DOT3AD_TRACE_SEL_SELECT_BEGIN 0xD0
#define DOT3AD_TRACE_SEL_UNSELECT     0xD1
#define DOT3AD_TRACE_SEL_READY        0xD2
#define DOT3AD_TRACE_SEL_SELECT_END   0xD3

#define DOT3AD_TRACE_TX_LACPDU        0xE0
#define DOT3AD_TRACE_LAG_DOWN         0x44
#define DOT3AD_TRACE_LAG_UP           0x77

#define DOT3AD_TRACE_CODE_PRINT(a) \
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"%s ", #a); 


#define DOT3AD_DEBUG_NSF_NONE           0x00000000
#define DOT3AD_DEBUG_NSF_NIM_TRACE      ((L7_uint32)(1 << 0))
#define DOT3AD_DEBUG_NSF_CNFGR_TRACE    ((L7_uint32)(1 << 1))
#define DOT3AD_DEBUG_NSF_CKPT_TRACE     ((L7_uint32)(1 << 2))
#define DOT3AD_DEBUG_NSF_CKPT_DETAIL    ((L7_uint32)(1 << 3))

#define DOT3AD_DEBUG_NSF_CKPT_MSG       ((L7_uint32)(1 << 4))
#define DOT3AD_DEBUG_NSF_TRANSPORT      ((L7_uint32)(1 << 5))
#define DOT3AD_DEBUG_NSF_HELPER         ((L7_uint32)(1 << 6))
#define DOT3AD_DEBUG_NSF_HELPER_DETAIL  ((L7_uint32)(1 << 7))

#define DOT3AD_DEBUG_NSF_RECONCILE      ((L7_uint32)(1 << 8))

#define DOT3AD_DEBUG_NSF_ALL            ~(DOT3AD_DEBUG_NSF_NONE)

#define DOT3AD_DEBUG_NSF(a) (dot3adNsfDebugFlag & (a)) 

#define DOT3AD_DEBUG_NSF_PRINT(args...) SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,args)

#define DOT3AD_DEBUG_NSF_PRINT_DEBUG(flag,args...) {\
    if (dot3adNsfDebugFlag & (flag)) \
    {\
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s:",#flag);\
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,args);\
    }\
}

/*********************************************************************
*
* @purpose Trace dot3ad packets received
*
* @param   intIfNum	@b{(input)} Internal Interface Number
* @param   *buff	@b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot3adDebugPacketRxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu);

/*********************************************************************
*
* @purpose Trace dot3ad packets transmitted
*
* @param   intIfNum	@b{(input)} Internal Interface Number
* @param   *buff	@b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot3adDebugPacketTxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu);

void dot3adNsfDebugPktDump(L7_uchar8 *pkt, L7_uint32 length);

void dot3adTraceInit(L7_uint32 numEntries, L7_uint32 numWidth);

void dot3adTraceFini();

void dot3adAggTrace(L7_uint32 aggId, L7_uchar8 traceCode);

void dot3adPortTrace(dot3ad_port_t *p, L7_uchar8 traceCode);

void dot3adIntfEventTrace(L7_uint32 intIfNum, L7_uint32 event);
#endif /* DOT3AD_DEBUG_H */
