/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/clicommands_acl.h
 *
 * @purpose header for clicommands_acl.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  08/23/2002
 *
 * @author  jagdishc
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDSACLIS_H
#define CLICOMMANDSACLIS_H

#define ACL_STANDARD 0
#define ACL_EXTENDED 1
#define ACL_NOTDEFIND 2
#define CLIACL_SYNTAXLENGTH 500
#define CLIACL_IPSTRSIZE 16

L7_uint32  checkAcltype(L7_uint32 aclId);
L7_char8 *  getSrcIPAddressAndPort(EwsContext ewsContext, const L7_char8 * * argv,
                                   L7_uint32 index,L7_uint32 * relIndex, L7_uint32 * ipAddr,
                                   L7_uint32 * ipMask);

const L7_char8 *commandAccessList(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandAclTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandIPAccessGroup(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandIPAccessGroupAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowIpAccessList(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowIpv6AccessLists(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const char *commandShowMacAccessLists(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowAccessListInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowAccessListVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

void commandShowAclTrapflags(EwsContext ewsContext);

void cliTreeAccessList(EwsCliCommandP depth1);
void cliTreeAclTrapflags(EwsCliCommandP depth1);
void cliTreeAccessListNormalAction(EwsCliCommandP depth1);
void cliTreeAccessListExtendedAction(EwsCliCommandP depth1);

void cliTreeShowAccesslist(EwsCliCommandP depth1);
void cliTreeShowIpAccesslist(EwsCliCommandP depth1);
void cliTreeShowIpv6Accesslist(EwsCliCommandP depth2);
void cliTreeShowMacAccessList(EwsCliCommandP depth1);
void cliTreeIpAccessGroup(EwsCliCommandP depth1);

void cliTreeIpAccessGroupAll(EwsCliCommandP depth1);
void cliTreeIpAccessList(EwsCliCommandP depth2);

const L7_char8 *cliIpAccessListMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliIpAccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

const L7_char8 *cliIpv6AccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliIpv6AccessListMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpv6AccessList(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpAccessListRename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpv6TrafficFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
void buildTreeInterfaceIpv6TrafficFilter(EwsCliCommandP depth1);

void buildTreeGlobalMacAccessList(EwsCliCommandP depth1);
void buildTreeInterfaceMacAccessList(EwsCliCommandP depth1);
void buildTreeMacAccessListDenyPermit(EwsCliCommandP depth2);

const L7_char8 *cliMacAccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliMacAccessListMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacAccessListDenyPermit(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacAccessListRename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacAccessGroup(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

void  buildTreeMacAccessListSource(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListDest(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListOptional(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListEtherType(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListVlan(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListCos(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListCosEq(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListCosRange(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListSecondaryVlan(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListSecondaryCos(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListSecondaryCosEq(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListSecondaryCosRange(EwsCliCommandP depth1, L7_BOOL permitMenu);
void  buildTreeMacAccessListLog(EwsCliCommandP depth1);
void  buildTreeMacAccessListAssignQueue(EwsCliCommandP depth1);
void  buildTreeMacAccessListMirror(EwsCliCommandP depth1);
void  buildTreeMacAccessListRedirect(EwsCliCommandP depth1);

void  macAclSyntaxWrite(EwsContext ewsContext);

#endif
