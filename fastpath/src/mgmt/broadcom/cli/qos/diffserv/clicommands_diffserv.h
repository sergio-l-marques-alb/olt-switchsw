/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/clicommands_diffserv.h
 *
 * @purpose header for clicommands_diffserv.c
 *
 * @component user interface
 *
 * @comments
 *
 * @create  07/08/2002
 *
 * @author  Kathy McDowell
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDSDIFFSERV_H
#define CLICOMMANDSDIFFSERV_H

#define L7_CLI_DIFFSERV_LARGE_STRING               110

extern L7_char8 dscpHelp[];

/*****************************
**
** BEGIN DIFFSERV FUNCTIONS HELP
**
******************************/

/*--------------------------------
**
** BEGIN DIFFSERV FUNCTIONS TREE COMMAND STRUCTURE
**
** diffserv command depth functions
**These functions are defined in the clicommands_diffserv.c
**--------------------------------*/
/* BEGIN */

/* global config */
void cliTreeGlobalConfigClassMap(EwsCliCommandP depth1);
void cliTreeGlobalConfigPolicyMap(EwsCliCommandP depth1);
void cliTreeGlobalConfigDiffserv(EwsCliCommandP depth1);
void cliTreeGlobalConfigServicePolicy(EwsCliCommandP depth1);
void cliTreeGlobalConfigRenameClassMap(EwsCliCommandP depth2);

/* privileged exec */
void cliTreePrivilegedExecShowDiffserv(EwsCliCommandP depth2);
void cliTreePrivilegedExecShowServicePolicy(EwsCliCommandP depth2);
void cliTreePrivilegedExecPolicyMap(EwsCliCommandP depth1);

/* interface config */
void cliTreeInterfaceConfigServicePolicy(EwsCliCommandP depth1);

/* user exec */
void cliTreeUserExecClassMap(EwsCliCommandP depth2);

/* class-map config */
void cliTreeClassMapConfig(EwsCliCommandP depth1);

/* policy-map config */
void cliTreePolicyMapClass(EwsCliCommandP depth1);

/* policy-class-map config */
void cliTreePolicyClassMapConfig(EwsCliCommandP depth1);

/* policy-class-map config tree creation helper fns */
void cliTreePolicyClassMapViolateAction(EwsCliCommandP depth1);
void cliTreePolicyClassMapPoliceExceedAction(EwsCliCommandP depth1);
void cliTreePolicyClassMapPoliceSimple(EwsCliCommandP depth1);
void cliTreePolicyClassMapPoliceSingleRate(EwsCliCommandP depth1);
void cliTreePolicyClassMapPoliceTwoRate(EwsCliCommandP depth1);
void cliTreePolicyClassMapConformColor(EwsCliCommandP depth1);

/* END */

/*--------------------------------
**
** END DIFFSERV FUNCTIONS TREE STRUCTURE  SHOW & CONFIG
**
   --------------------------------*/

/*--------------------------------
**
** BEGIN DIFFSERV FUNCTIONS COMMANDS STRUCTURE CONFIG & SHOW
**
** command execution functions
** These functions are defined in cli_show_qos.c and cli_config_qos.c
   --------------------------------*/
/* BEGIN SHOW COMMANDS PROTOTYPES */
const char *commandShowDiffserv(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowClassMap(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowPolicyMap(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowPolicyMapInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowDiffservServiceBrief(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowDiffservService(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandShowServicePolicy(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

/* END SHOW COMMANDS PROTOTYPES */

/* BEGIN CONFIG PROTOTYPES*/
const char *commandDiffserv(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchCos(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotCos(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchSecondaryCos(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotSecondaryCos(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchDstip(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotDstip(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchDstip6(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotDstip6(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchDstl4port(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotDstl4port(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchDestinationAddressMac(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotDestinationAddressMac(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchAny(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotAny(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchIpDscp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotIpDscp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchEtherType(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotEtherType(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchIp6FlowLbl(EwsContext ewsContext, uintf argc, const L7_char8 * * argv, uintf index);
const char *commandMatchNotIp6FlowLbl(EwsContext ewsContext, uintf argc, const L7_char8 * * argv, uintf index);
const char *commandMatchIpPrecedence(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotIpPrecedence(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchIpTos(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotIpTos(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchClassMap(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchSrcip(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotSrcip(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchSrcip6(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotSrcip6(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchSrcl4port(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotSrcl4port(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchSourceAddressMac(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotSourceAddressMac(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchVlan(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotVlan(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchSecondaryVlan(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMatchNotSecondaryVlan(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandClassMapRename(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMark(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandPoliceSimple(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandPoliceSingleRate(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandPoliceTwoRate(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandConformColor(EwsContext ewsContext, uintf argc, const L7_char8 * * argv, uintf index);

void cliConfigDiffservPolicyPoliceConform(EwsContext ewsContext, char * strPolicyName,                                           char * strClassName,
                                          L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t actionType,
                                          L7_uint32 markVal);

void cliConfigDiffservPolicyPoliceExceed(EwsContext ewsContext, char * strPolicyName,
                                         char * strClassName,
                                         L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t actionType,
                                         L7_uint32 markVal);
void cliConfigDiffservPolicyPoliceNonconform(EwsContext ewsContext, char * strPolicyName,
                                             char * strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t actionType,
                                             L7_uint32 markVal);
const char *commandPolicyMapRename(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandAssignQueue(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandRedirect(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandMirror(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandDrop(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

L7_RC_t cliServicePolicy(EwsContext ewsContext, uintf argc, const char * * argv, uintf index, L7_uint32 direction);
L7_RC_t cliServicePolicyAll(EwsContext ewsContext, uintf argc, const char * * argv, uintf index, L7_uint32 direction);
const char *commandServicePolicyIn(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandServicePolicyOut(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandServicePolicyInAll(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandServicePolicyOutAll(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandClassMap(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *commandExpedite(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
L7_RC_t cliDiffservClassCreate( EwsContext ewsContext, L7_char8 * strName, L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType, L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Protocol, L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t aclType, L7_uint32 aclNumber );
const char *cliClassMapMode(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliPolicyMapMode(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const char *cliPolicyClassMapMode(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

/* END CONFIG PROTOTYPES*/

#endif
