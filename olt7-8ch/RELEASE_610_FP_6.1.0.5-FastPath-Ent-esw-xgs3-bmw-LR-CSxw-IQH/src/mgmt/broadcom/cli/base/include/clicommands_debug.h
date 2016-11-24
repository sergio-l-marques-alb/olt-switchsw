/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_debug.h
 *
 * @purpose header for clicommands_debug.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/24/2006
 *
 * @author  cpverne
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DEBUG_H
#define CLICOMMANDS_DEBUG_H

/**************************************
**
** BEGIN DEBUG FUNCTIONS HELP
**
***************************************/

#define CLIDEBUGSNOOP_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpSnoopingDebugFlags \
  : pStrInfo_base_CfgureMldSnoopingDebugFlags
#define CLIDEBUGSNOOPPACKET_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_TurnOnIgmpSnoopingPktDebugTrace \
  : pStrInfo_base_TurnOnMldSnoopingPktDebugTrace
#define CLIDEBUGSNOOPTXPACKET_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_TurnOnIgmpSnoopingTxPktDebugTrace \
  : pStrInfo_base_TurnOnMldSnoopingTxPktDebugTrace
#define CLIDEBUGSNOOPRXPACKET_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_TurnOnIgmpSnoopingReceivePktDebugTrace \
  : pStrInfo_base_TurnOnMldSnoopingReceivePktDebugTrace

/*********************************************************
**
** BEGIN DEBUG FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void buildTreePrivledgedDebug(EwsCliCommandP depth1);
void buildTreePrivShowDebugging(EwsCliCommandP depth2);

/********************************************************************
**
** BEGIN DEBUG FUNCTIONS COMMANDS STRUCTURE
**
*********************************************************************/
const L7_char8 *commandShowDebugging(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDebugConsole(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDebugLacpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDebugSpanningTreePacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDebugSnoopPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDebugPingPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandDebugDot1xPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
#ifdef L7_METRO_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
const L7_char8 *commandDebugDot3ahPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
#endif
#endif
const L7_char8 *commandDebugArpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDebugIpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDebugVrrpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDebugVoIPPacket(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 **argv,
                                       L7_uint32 index);
const L7_char8 *commandDebugsFlowPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandDebugIsdpPacket(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);


/*********************************************************
**
** BEGIN DEBUG SYNTAX ERROR MESSAGES
**
**********************************************************/

/* for igmp snooping command*/
#define CLISYNTAX_DEBUG_SNOOP_PACKET(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseDebugIgmpsnoopingPkt, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseDebugMldsnoopingPkt, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoDebugIgmpsnoopingPkt, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoDebugMldsnoopingPkt, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_ENABLED(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_IgmpSnoopingPktTracingEnbldTxAndReceive, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_MldSnoopingPktTracingEnbldTxAndReceive, 1, 0, 0, 0, L7_NULLPTR)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_TX_ENABLED(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_IgmpSnoopingPktTracingEnbldTxOnly, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_MldSnoopingPktTracingEnbldTxOnly, 1, 0, 0, 0, L7_NULLPTR)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_RX_ENABLED(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_IgmpSnoopingPktTracingEnbldReceiveOnly, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_MldSnoopingPktTracingEnbldReceiveOnly, 1, 0, 0, 0, L7_NULLPTR)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_DISABLED(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_IgmpSnoopingPktTracingDsbld, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_MldSnoopingPktTracingDsbld, 1, 0, 0, 0, L7_NULLPTR)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_TX_DISABLED(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_IgmpSnoopingTxPktTracingDsbld, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_MldSnoopingTxPktTracingDsbld, 1, 0, 0, 0, L7_NULLPTR)
#define CLISYNTAX_DEBUG_SNOOP_PACKET_RX_DISABLED(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_IgmpSnoopingReceivePktTracingDsbld, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_MldSnoopingReceivePktTracingDsbld, 1, 0, 0, 0, L7_NULLPTR)
/* for ping command*/

#endif /* CLICOMMANDS_DEBUG_H */
