/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/clicommands_cos.h
 *
 * @purpose header for clicommands_cos.c
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

#ifndef CLICOMMANDSQOSCOS_H
#define CLICOMMANDSQOSCOS_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
extern L7_char8 dscpHelp[];

const L7_char8 *commandShowInterfacesCosQueue(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowInterfacesRandomDetect(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowInterfacesTailDropThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandGlobalCosQueueMinBandwidth(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceCosQueueMinBandwidth(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandGlobalCosQueueStrict(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceCosQueueStrict(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceTrafficShape(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandGlobalTrafficShape(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

/* new dev_i functions */
const L7_char8 *commandClassofserviceIpDscpMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClassofserviceIpPrecedenceMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClassofserviceTrust(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandCosQueueMaxBandwidth(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandCosQueueRandomDetect(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandRandomDetect(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandRandomDetectExponentialWeightingConstant(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandRandomDetectQueueParms(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTailDropQueueParms(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowClassofServiceIpDscpMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowClassofServiceIpPrecedenceMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowClassofServiceTrust(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTailDropQueueParms(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

void cliCosQueueThreshStringFormat(const L7_uchar8 val[], L7_uchar8 * pStr);

void buildTreePrivSwInfoShowInterfacesTailDropThreshold(EwsCliCommandP depth3);
void buildTreePrivSwInfoShowInterfacesCosQueue(EwsCliCommandP depth3);
void buildTreeInterfaceCosQueue(EwsCliCommandP depth1);
void buildTreeGlobalCosQueue(EwsCliCommandP depth1);
void buildTreeInterfaceTrafficShape(EwsCliCommandP depth1);
void buildTreeGlobalTrafficShape(EwsCliCommandP depth1);

void buildTreeCosQueueMaxBandwidth(EwsCliCommandP depth2);
void buildTreeCosQueueRandomDetect(EwsCliCommandP depth2);
void buildTreeRandomDetect(EwsCliCommandP depth1, L7_BOOL isGlobal);
void buildTreeTailDrop(EwsCliCommandP depth1);
void buildTreeRandomDetectQueueParmsContinued(EwsCliCommandP depth1);
void buildTreeTailDropQueueParmsContinued(EwsCliCommandP depth1);
void buildTreeCosIPDscpMapping(EwsCliCommandP depth1);
void buildTreeCosIPPrecedenceMapping(EwsCliCommandP depth1);
void buildTreeInterfaceCosTrust(EwsCliCommandP depth1);
void buildTreeGlobalCosTrust(EwsCliCommandP depth1);

#endif
