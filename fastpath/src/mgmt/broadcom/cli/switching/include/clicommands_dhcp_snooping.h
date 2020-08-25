/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_dhcp_snooping.h
*
* @purpose Dhcp Snooping commands header file
*
* @component Dhcp Snooping
*
* @comments none
*
* @create  03/16/2007
*
* @author  rrice
*
* @end
*
*********************************************************************/

#ifndef CLIDHCPSNOOPING_H
#define CLIDHCPSNOOPING_H

/**************************************
**
** BEGIN DHCP SNOOPING FUNCTIONS HELP
**
***************************************/

#define CLI_MAX_SCROLL_LINES_DHCP_FILTER            (CLI_MAX_SCROLL_LINES-9)

/*************************************
**
** END DHCP SNOOPING FUNCTIONS HELP
**
**************************************/

/*****************************************
**
** BEGIN DHCP SNOOPING FUNCTIONS SYNTAX
**
*******************************************/

/*****************************************
**
** END DHCP SNOOPING FUNCTIONS SYNTAX
**
******************************************/

/*********************************************************
**
** BEGIN DHCP SNOOPING FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void buildTreeGlobalDhcpSnooping(EwsCliCommandP depth1);
void buildTreeIfIpDhcpSnoopingTrust(EwsCliCommandP depth1);
void buildTreeShowIpDhcpSnooping(EwsCliCommandP depth1);
void buildTreeClearDhcpSnoop(EwsCliCommandP depth4);
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/* DHCP L2 Relay Global config commands.*/
void buildTreeGlobalDhcpL2Relay(EwsCliCommandP depth1);
void buildTreeIntfDhcpL2Relay(EwsCliCommandP depth1);
void buildTreeShowDhcpL2Relay(EwsCliCommandP depth1);
void buildTreeClearDhcpL2Relay(EwsCliCommandP depth1);
#endif
#ifdef L7_IPSG_PACKAGE
void buildTreeGlobalIpsg(EwsCliCommandP depth1);
void buildTreeIpsgVerifySource(EwsCliCommandP depth2);
void buildTreeShowIpVerifySource(EwsCliCommandP depth1);
void buildTreeShowIpSourceBinding(EwsCliCommandP depth1);
#endif

/*********************************************************
**
** END CARD FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/

/********************************************************************
**
** BEGIN DHCP SNOOPING FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
*********************************************************************/

/* commands defined in cli_show_dhcp_snooping.c */
const char *commandShowIpDhcpSnooping(EwsContext ewsContext, L7_uint32 argc, 
                                      const L7_char8 **argv, L7_uint32 index);
const char *commandShowIpDhcpSnoopingBinding(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index);
const char *commandShowIpDhcpSnoopingIf(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index);
const char *commandShowIpDhcpSnoopingDatabase(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index);
const char *commandShowIpDhcpSnoopingStats(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);

#ifdef L7_DHCP_L2_RELAY_PACKAGE
/* DHCP L2 Relay show command.*/

const char *commandShowDhcpL2Relay(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index);
const char *commandShowDhcpL2RelayIntfStats(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearDhcpL2RelayStats(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index);
/* DHCP L2 Relay commands.*/
const L7_char8 *commandDhcpL2RelayGlobalMode(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandDhcpL2RelayVlan(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandDhcpL2RelayVlanCircuitId(EwsContext ewsContext, L7_uint32 argc,
                                                  const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDhcpL2RelayVlanRemoteId(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandDhcpL2RelayIntfMode(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDhcpL2RelayIntfTrust(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
const char *commandShowDhcpL2RelaySubscription(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDhcpL2RelaySubscription(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandDhcpL2RelayCircuitIdSubscription(EwsContext ewsContext,
                                                         L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandDhcpL2RelayRemoteIdSubscription(EwsContext ewsContext,
                                                         L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);

#endif
#endif
/* cli_clear_dhcp_snooping.c */
const L7_char8 *commandClearIpDhcpSnoopingStats(EwsContext ewsContext, L7_uint32 argc,
                                                const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearIpDhcpSnoopingBinding(EwsContext ewsContext, L7_uint32 argc,
                                                  const L7_char8 **argv, L7_uint32 index);

#ifdef L7_IPSG_PACKAGE

const char *commandShowIpVerifySource(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const char *commandShowIpVerify(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const char *commandShowIpsgBinding(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
#endif

/* commands defined in cli_config_dhcp_snooping.c */
const L7_char8 *commandIpDhcpSnooping(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingVerifyMac(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingDbUrl(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingDbStoreInterval(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingBinding(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingVlan(EwsContext ewsContext, L7_uint32 argc, 
                                          const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingTrust(EwsContext ewsContext, L7_uint32 argc, 
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingIfRateLimit(EwsContext ewsContext, L7_uint32 argc, 
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDhcpSnoopingLogInvalid(EwsContext ewsContext, L7_uint32 argc, 
                                                const L7_char8 **argv, L7_uint32 index);

#ifdef L7_IPSG_PACKAGE
const L7_char8 *commandIpsgEntry(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);

/* IP Source Guard */
const L7_char8 *commandIpVerifySource(EwsContext ewsContext, L7_uint32 argc, 
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpVerifySourcePortSecurity(EwsContext ewsContext, L7_uint32 argc, 
                                                  const L7_char8 **argv, L7_uint32 index);
#endif

/********************************************************************
**
** END DHCP SNOOPING FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
*********************************************************************/

/*************************************************
**
** BEGIN DHCP SNOOPING COMMANDS HELPER ROUTINES
**
**************************************************/

/* routines defined in cliutil_card.c */

/**************************************************
**
** END DHCP SNOOPING COMMANDS HELPER ROUTINES
**
***************************************************/

#endif /* CLIDHCPSNOOPING_H*/
