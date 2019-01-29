/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename dot1q_debug.h
*
* @purpose   Vlan Debug file
*
* @component dot1q
*
* @comments
*
* @create    06/22/2005
*
* @author    akulkarni
*
* @end
*
**********************************************************************/
#ifndef __DOT1Q_DEBUG_H__
#define __DOT1Q_DEBUG_H__


#include "l7_common.h"
#include "dot1q_control.h"
#include "log.h"
#include "comm_mask.h"

extern L7_uint32 dot1qDebugMsgs; /* Working flag to display selected statements */
extern L7_uint32 dot1qProbeMask;

#define DOT1Q_PROBE_POINTS 4
#define DOT1Q_PROBE_INDEXES (L7_PLATFORM_MAX_VLAN_ID + 1)

#define L7_INTF_MASK_STRING_LENGTH  (((L7_MAX_INTERFACE_COUNT/32)+1)*15)   /* TBD: Reuse for mask utility */

#define DOT1Q_PROBE_MASK_NONE               0x00000000
#define DOT1Q_PROBE_MASK_VLAN_CREATE        0x00000001
#define DOT1Q_PROBE_MASK_VLAN_DELETE        0x00000002
#define DOT1Q_PROBE_MASK_VLAN_PARTICIPATE   0x00000004
#define DOT1Q_PROBE_MASK_VLAN_TAGGEDSET     0x00000008

#define DOT1Q_IS_PROBE_MASK_SET(a) (a & dot1qProbeMask)

#define DOT1Q_DEBUG_MSG(args...) {\
    if (dot1qDebugMsgs == L7_TRUE) \
    {\
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,args);\
    }\
}

#define DOT1Q_TRACE(__fmt__,__args__...)                        \
{                                                               \
    if ( dot1qDebugTraceModeGet() == L7_ENABLE)                 \
    {                                                           \
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,__fmt__,##__args__);  \
    }                                                                 \
                                                            \
    if (dot1qDebugConsoleTraceModeGet() == L7_ENABLE)       \
    {                                                       \
        L7_char8  __buf__[L7_LOG_FORMAT_BUF_SIZE];          \
        (void)sprintf(__buf__, __fmt__, ## __args__);       \
        dot1qTraceWrite( __buf__);                          \
                                                            \
    }                                                       \
}


/* Prototypes */

void dot1qDebugInit();
void dot1qDebugMsgQueue(L7_uint32 detailed);
L7_uint32 dot1qDebugTraceModeGet(void);
L7_RC_t dot1qDebugTraceFlagsSet(L7_uint32 mode);
L7_uint32 dot1qDebugConsoleTraceModeGet(void);
L7_RC_t dot1qDebugConsoleTraceFlagsSet(L7_uint32 mode);
L7_uint32 dot1qDebugMsgModeGet(void);
L7_RC_t dot1qDebugMsgModeSet(L7_uint32 mode);
void dot1qTraceWrite(L7_uchar8 *traceMsg);
void dot1qTraceQueueMsgSend(DOT1Q_MSG_t *msg);
void dot1qTraceQueueMsgRecv(DOT1Q_MSG_t *msg);
void dot1qDebugListToMaskString(L7_ushort16 *intIfList,L7_uint32 numPorts, L7_uchar8 *pMaskString);
void dot1qDebugTestIntfMaskDump (L7_INTF_MASK_t *mask);
void dot1qDebugIntfMaskToString(L7_INTF_MASK_t *mask, L7_uchar8 *pString);
void dot1qDebugApiTest(L7_uint32 directive);

void dot1qDebugProbe(L7_uint32 probeIndex, L7_uint32 probePoint, L7_uint32 mask);
void dot1qProbeMaskSet(L7_uint32 probeMask);
void dot1qDebugInternalUsageShow(void);
void dot1qDebugPendingVlanShow(void);

#endif /* __DOT1Q_DEBUG_H__ */
