/*
 * snooping_ptin_db.h
 *
 *  Created on: 23 de Jul de 2012
 *      Author: x01414
 */

#ifndef SNOOPING_PTIN_DB_H_
#define SNOOPING_PTIN_DB_H_

#include "snooping.h"
#include "snooping_ptin_util.h"
#include "snooping_ptin_defs.h"

#include "datatypes.h"

//Processes a Source Specific Membership Report with record type MODE_IS_INCLUDE
L7_RC_t snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type MODE_IS_EXCLUDE
L7_RC_t snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type MODE_TO_INCLUDE
L7_RC_t snoopPTinMembershipReportToIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type MODE_TO_EXCLUDE
L7_RC_t snoopPTinMembershipReportToExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type ALLOW_NEW_SOURCES
L7_RC_t snoopPTinMembershipReportAllowProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type BLOCK_OLD_SOURCES
L7_RC_t snoopPTinMembershipReportBlockProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Search a specific source in a given source list
L7_RC_t snoopPTinSourceFind(snoopPTinL3Source_t *sourceList, L7_inet_addr_t* sourceAddr, L7_int8* foundIdx);

//Adds a new source to the given source list in the first free index
L7_RC_t snoopPTinSourceAdd(snoopPTinL3Interface_t* interfacePtr, L7_inet_addr_t* sourceAddr, L7_uint8* newSourceIdx);

//Remove a source entry, reseting all its fields and stopping the associated timer
L7_RC_t snoopPTinSourceRemove(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourceList);

//Initializes an interface
L7_RC_t snoopPTinInitializeInterface(snoopPTinL3Interface_t *snoopEntry,L7_uint32 vlanId, L7_inet_addr_t* mcastGroupAddr,L7_uint16 interfaceIdx);

//Remove an interface entry, reseting all its fields and stopping the group and query timers
L7_RC_t snoopPTinInterfaceRemove(snoopPTinL3Interface_t *interfacePtr,L7_uint32 vlanId, L7_inet_addr_t* mcastGroupAddr,L7_uint16 interfaceIdx);

//verify if this bitmap is already initialized
L7_RC_t snoopPTinZeroClients(L7_uint32* clients,L7_uint16 numberOfClients);

/************************************************************************************************************/
/*MGMD Proxy*/
#if 1
//Adds a new source to the given proxy source list in the first free index
L7_RC_t snoopPTinProxySourceAdd(snoopPTinL3InfoData_t* avlPtr,L7_uint32 interfaceIdx, L7_inet_addr_t* sourceAddr);

//Remove a source entry, reseting all its fields and stopping the associated timer
L7_RC_t snoopPTinProxySourceRemove(snoopPTinL3InfoData_t* avlPtr,L7_uint32 clientIdx, L7_inet_addr_t* sourceAddr);

//Process General Query Message
snoopPTinProxyGroup_t* snoopPTinGeneralQueryProcess(L7_uint32 vlanId, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, L7_uint32 *noOfRecords, L7_uint32 *timeout,L7_uint8 robustnessVariable);

//Process Group Specific Query Message
snoopPTinProxyGroup_t* snoopPTinGroupSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, L7_uint32 *timeout,L7_uint8 robustnessVariable);

//Process Group & Source Specific Query Message
snoopPTinProxyGroup_t* snoopPTinGroupSourceSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx, L7_ushort16 noOfSources, L7_inet_addr_t *sourceList, L7_uint32 selectedDelay, L7_BOOL *sendReport, L7_uint32 *timeout,L7_uint8 robustnessVariable);

//Adds a Static Multicast Group with a Source List
L7_RC_t snoopPTinAddStaticGroup(L7_uint32 vlanId, L7_uint32 intIfNum,L7_inet_addr_t *groupAddr,L7_uint32 noOfSources,L7_inet_addr_t* sourceAddr);

//Removes a Static Multicast Group with a Source List
L7_RC_t snoopPTinRemoveStaticGroup(L7_uint32 vlanId, L7_uint32 intIfNum,L7_inet_addr_t *groupAddr,L7_uint32 noOfSources,L7_inet_addr_t* sourceAddr);

snoopPTinProxyInterface_t* snoopPTinProxyInterfaceAdd(L7_uint32 vlanId);

snoopPTinProxyGroup_t* snoopPTinGroupRecordAdd(snoopPTinProxyInterface_t* interfacePtr,L7_uint8 recordType, L7_inet_addr_t* groupAddr, L7_BOOL* newEntryFlag,L7_uint8 robustnessVariable);

L7_RC_t snoopPTinGroupRecordSourcedAdd(snoopPTinProxyGroup_t* groupPtr,L7_inet_addr_t* sourceAddr,L7_uint8 robustnessVariable);

L7_RC_t snoopPTinGroupRecordFind(L7_uint32 vlanId,L7_inet_addr_t   *groupAddr,L7_uint8 recordType, snoopPTinProxyGroup_t*  groupPtr );

L7_RC_t snoopPTinGroupRecordSourceFind(L7_uint32 vlanId,L7_inet_addr_t   *groupAddr,L7_uint8 recordType, L7_inet_addr_t   *sourceAddr, snoopPTinProxySource_t*  sourcePtr );

L7_RC_t snoopPTinGroupRecordAddSourceList( L7_uint32  vlanId, L7_inet_addr_t *groupAddr, L7_uint8 recordType, L7_inet_addr_t   *sourceAddr, L7_uint32 sourceCnt,snoopPTinProxyGroup_t *groupPtr);

L7_RC_t snoopPTinProxyInterfaceRemove(snoopPTinProxyInterface_t* interfacePtr);

L7_RC_t snoopPTinGroupRecordRemoveAll(snoopPTinProxyInterface_t* interfacePtr,L7_inet_addr_t* groupAddr);

L7_RC_t snoopPTinGroupRecordRemove(snoopPTinProxyInterface_t* interfacePtr, L7_inet_addr_t* groupAddr,L7_uint8 recordType);

L7_RC_t snoopPTinGroupRecordSourceRemove(snoopPTinProxyGroup_t*   groupPtr, L7_inet_addr_t *sourceAddr);

L7_RC_t snoopPTinGroupRecordSourceRemoveAll(snoopPTinProxyGroup_t*   groupPtr);


#endif
/*End MGMD Proxy*/
/************************************************************************************************************/



#endif /* SNOOPING_PTIN_DB_H_ */
