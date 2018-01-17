/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trapstr_mcast_layer3.c
*
* @purpose Trap Manager Layer 3 Multicast String File
*
* @component trapstr_mcast_layer3.c
*
* @comments none
*
* @created 08/26/2002
*
* @author mfiorito
*
* @end
*
**********************************************************************/

#include <l7_common.h>
#include <trapstr_mcast_layer3.h>

/* PIM */
L7_char8 pimNeighborLoss_str[] = "PIM Neighbor Lost: Neighbor UP Time: %d seconds";
/* DVMRP */
L7_char8 dvmrpNeighborLoss_str[]       = "DVMRP Neighbor Lost: LocalAddr: %s NewState: %s";
L7_char8 dvmrpNeighborNotPruning_str[] = "DVMRP Neighbor Not Pruning: LocalAddr: %s NeighborCapabilities: %s";

/* DVMRP Neighbor States */
L7_char8 dvmrpNeighborStateOneway[]    = "ONEWAY";
L7_char8 dvmrpNeighborStateActive[]    = "ACTIVE";
L7_char8 dvmrpNeighborStateIgnoring[]  = "IGNORING";
L7_char8 dvmrpNeighborStateDown[]      = "DOWN";

/* DVMRP Capabilities */
L7_char8 dvmrpCapabilitiesLeaf[]       = "LEAF ";
L7_char8 dvmrpCapabilitiesPrune[]      = "PRUNE ";
L7_char8 dvmrpCapabilitiesGenId[]      = "GENID ";
L7_char8 dvmrpCapabilitiesMTrace[]     = "MTRACE ";
