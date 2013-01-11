/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dot1s/clicommands_dot1s.h
 *
 * @purpose header for spantree commands in clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/29/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_DOT1S_H
#define CLICOMMANDS_DOT1S_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"

/*****************************************************************/
/* SWSPAN */
/* BEGIN DOT1S TREE COMMAND STRUCTURE */

void buildTreeGlobalSWSpanSpanningTreeConf(EwsCliCommandP depth1);
void buildTreeInterfaceSWSpanSpanningTreeConf(EwsCliCommandP depth1);
void buildTreeUserExecSWSpanShowSpanningTree(EwsCliCommandP depth2);

/*****************************************************************/

/*****************************************************************/
/* SWSPAN */
/* BEGIN DOT1S FUNCTIONS SHOW & CONFIG */

const L7_char8 *cliShowSpanTree(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *cliShowSpanTreeCst(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *cliShowSpanTreeCstPort(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *cliShowSpanTreeMst(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *cliShowSpanTreeMstPort(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *cliShowMSTPortSummary(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index,L7_uint32 instance, L7_uint32 argSlotPort,L7_char8 * commandString, L7_uint32 commandStringSize, const L7_char8 * cmdParm);

const L7_char8 *commandSpanningTree (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeBpdumigrationcheck (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeBpdumigrationcheckAll (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeConfigurationName (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeConfigurationRevision (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeForceVersion (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeForwardTime (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeHelloTime (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeMaxAge (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeMstInstance (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeMstPriority (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeMstVlan (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreePortModeAll (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreePortModeInterface (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreePriority (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandSpanningTreeCstPortEdgePort (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeAutoEdge (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeRootGuard (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreePortGuard (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeTcnGuard (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeCstPortPathCost (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeCstPortPriority (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeCstPortPathExternalCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeMstPortPathCostPriority (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowSpanningTree (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeBrief (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeCstPort (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeInterface (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeMstDetailed (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeMstPortDetailed (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeMstPortSummary (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeMstSummary (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeSummary (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeVlan (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeMaxHops (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeHoldCount (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeHelloTimeInterface (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSpanningTreeMaxHopCount (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeExternalMstPortPathCost (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpanningTreeBpduGuard(EwsContext ewsContext,L7_uint32 argc, const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandSpanningTreeBpduFilterDefault(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandSpanningTreeIntfBpduFilter(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandSpanningTreeIntfBpduFlood(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

#endif
