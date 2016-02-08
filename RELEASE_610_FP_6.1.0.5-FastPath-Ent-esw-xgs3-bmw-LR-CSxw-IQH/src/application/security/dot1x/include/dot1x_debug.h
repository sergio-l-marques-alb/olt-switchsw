
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_debug.h
*
* @purpose   dot1x debug utilities
*
* @component dot1x
*
* @comments   none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_DEBUG_H
#define INCLUDE_DOT1X_DEBUG_H

#define DOT1X_TRACE(format,args...)                         \
{                                                           \
  if ( dot1xDebugLogTraceModeGet() == L7_ENABLE)           \
  {                                                        \
    LOG_COMPONENT_MSG(L7_DOT1X_COMPONENT_ID,format,##args); \
  }                                                        \
}

#define DOT1X_ERROR_SEVERE(format,args...)                  \
{                                                           \
    LOG_COMPONENT_MSG(L7_DOT1X_COMPONENT_ID,format,##args); \
}

extern L7_RC_t dot1xDebugInit();
extern void dot1xDebugRegister(void);
extern void dot1xDebugMsgQueue();
extern void dot1xDebugTraceIdGet();
extern void dot1xDebugSizesShow();
extern void dot1xDebugPortCfgShow(L7_uint32 intIfNum);
extern void dot1xDebugPortInfoShow(L7_uint32 intIfNum);
extern void dot1xDebugPortStatsShow(L7_uint32 intIfNum);
extern L7_RC_t dot1xDebugLogTraceModeSet(L7_uint32 mode);
extern L7_uint32 dot1xDebugLogTraceModeGet();
extern L7_BOOL dot1xDebugPacketTraceTxFlag;
extern L7_BOOL dot1xDebugPacketTraceRxFlag;

extern void dot1xBuildTestConfigData(void);
extern L7_RC_t dot1xDebugSave(void);
extern L7_RC_t dot1xDebugRestore(void);
extern void dot1xDebugBuildDefaultConfigData(L7_uint32 ver);
extern L7_RC_t dot1xApplyDebugConfigData(void);
extern L7_BOOL dot1xDebugHasDataChanged(void);
extern void dot1xDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);
extern L7_RC_t dot1xDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);
extern void dot1xDebugPacketRxTrace(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern void dot1xDebugPacketTxTrace(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern void dot1xDebugPacketTrace(L7_uint32 intIfNum, L7_netBufHandle bufHandle,L7_BOOL rxFlag,L7_BOOL txFlag);
extern void dot1xDebugCfgRead(void);
extern void dot1xDebugPacketDump(L7_uint32 flag,L7_uint32 physPort,L7_uint32 intIfNum,L7_netBufHandle bufHandle);
extern void dot1xDebugDataDump(L7_uint32 flag, L7_uint32 physPort,L7_uchar8  *data,L7_uint32 len);

#define DOT1X_USER_TRACE_TX(__fmt__, __args__... )                              \
      if (dot1xDebugPacketTraceTxFlag == L7_TRUE)                               \
      {                                                                   \
        LOG_USER_TRACE(L7_DOT1X_COMPONENT_ID, __fmt__,##__args__);        \
      }

#define DOT1X_USER_TRACE_RX(__fmt__, __args__... )                              \
      if (dot1xDebugPacketTraceRxFlag == L7_TRUE)                               \
      {                                                                   \
        LOG_USER_TRACE(L7_DOT1X_COMPONENT_ID, __fmt__,##__args__);        \
      }

#define DOT1X_TRACE_PORT_STATUS     0x0001
#define DOT1X_TRACE_EVENTS          0x0002
#define DOT1X_TRACE_API_CALLS       0x0004
#define DOT1X_TRACE_FSM_EVENTS      0x0008
#define DOT1X_TRACE_FAILURE         0x0010
#define DOT1X_TRACE_EXTERNAL        0x0020
#define DOT1X_TRACE_RADIUS          0x0040
#define DOT1X_TRACE_TIMER           0x0080
#define DOT1X_TRACE_PDU             0x0100
#define DOT1X_TRACE_MAC_ADDR_DB     0x0200
#define DOT1X_TRACE_MAB             0x0400


extern L7_uint32 dot1xDebugTraceFlag; 
extern L7_uint32 dot1xDebugTraceIntf; 
#define DOT1X_EVENT_TRACE(flag,intf,__fmt__, __args__...)         \
  if ((dot1xDebugTraceFlag&flag) != 0 && (intf==0 || dot1xDebugTraceIntf ==0 || intf == dot1xDebugTraceIntf))  {\
   L7_char8  __buf1__[256];    \
   (void)osapiSnprintf (__buf1__, 256, __fmt__, ## __args__);  \
   sysapiPrintf(__buf1__);}

#endif /* INCLUDE_DOT1X_DEBUG_H*/
