/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trapstr_mcast_layer3.h
*
* @purpose Trap Manager Layer 3 Multicast String File
*
* @component trapstr_mcast_layer3.h
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

/* PIM */
extern L7_char8 pimNeighborLoss_str[];

/* DVMRP */
extern L7_char8 dvmrpNeighborLoss_str[];
extern L7_char8 dvmrpNeighborNotPruning_str[];

/* DVMRP Neighbor States */
extern L7_char8 dvmrpNeighborStateOneway[];
extern L7_char8 dvmrpNeighborStateActive[];
extern L7_char8 dvmrpNeighborStateIgnoring[];
extern L7_char8 dvmrpNeighborStateDown[];

/* DVMRP Capabilities */
extern L7_char8 dvmrpCapabilitiesLeaf[];
extern L7_char8 dvmrpCapabilitiesPrune[];
extern L7_char8 dvmrpCapabilitiesGenId[];
extern L7_char8 dvmrpCapabilitiesMTrace[];
