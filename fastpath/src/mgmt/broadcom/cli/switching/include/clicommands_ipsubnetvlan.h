/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/clicommands_ipsubnetvlan.h
 *
 * @purpose header for IP Subnet Vlan commands in clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  nshrivastav
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_IPSUBNETVLAN_H
#define CLICOMMANDS_IPSUBNETVLAN_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"

const L7_char8 * commandVlanAssociationSubnet (EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 * * argv, L7_uint32 index);


const L7_char8 *commandShowVlanAssociationSubnet(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 * * argv, L7_uint32 index);

/* Build tree functions. */
void buildTreeVlanDBSwDevVlanIpVlan(EwsCliCommandP depth3);
void buildTreeUserSwDevShowVlanIpVlan(EwsCliCommandP depth4);

#endif
