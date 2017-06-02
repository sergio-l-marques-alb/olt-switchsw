/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_sflow.h
 *
 * @purpose header for 802.1AB commands in clicommands_lldp.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  12/07/2007
 *
 * @author  Rajesh G
 * @end
 *
 ***********************************************************************/

#ifndef CLICOMMANDS_SFLOW_H
#define CLICOMMANDS_SFLOW_H

/*****************************************************************/
extern void buildTreeInterfacesFlow(EwsCliCommandP depth1);            /* depth1 = "Interface Config" */

extern void buildTreeGlobalsFlowConfig(EwsCliCommandP depth2);         /*depth1 = "sflow" */
/*********************build Sflow show commands***************************************/
extern void buildTreeUserExecSWMgmtShowSflow(EwsCliCommandP depth2);
extern L7_RC_t cliRunningConfigSflowInfo(EwsContext ewsContext, L7_uint32 unit);

/********************* sflow config commands *****************************************/

extern const L7_char8 *commandsFlowReceiver(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

extern const L7_char8 *commandsFlowReceiverIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandsFlowReceiverPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandsFlowReceiverMaxData(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandInterfacesFlowSampler(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandInterfacesFlowSpMaxHSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandInterfacesFlowSpRate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandInterfacesFlowPoller(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandInterfacesFlowCpInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/********************** sFlow show commands *****************************************/
extern const L7_char8 *commandShowsFlowAgent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandShowsFlowReceivers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandShowsFlowSamplers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

extern const L7_char8 *commandShowsFlowPollers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* END SFLOW ACTION  COMMANDS DECL */
#endif
