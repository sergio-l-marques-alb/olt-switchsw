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
L7_RC_t snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type MODE_IS_EXCLUDE
L7_RC_t snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type MODE_TO_INCLUDE
L7_RC_t snoopPTinMembershipReportToIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type MODE_TO_EXCLUDE
L7_RC_t snoopPTinMembershipReportToExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type ALLOW_NEW_SOURCES
L7_RC_t snoopPTinMembershipReportAllowProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Processes a Source Specific Membership Report with record type BLOCK_OLD_SOURCES
L7_RC_t snoopPTinMembershipReportBlockProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecords, snoopPTinProxyGroup_t* groupPtr);

//Search a specific source in a given source list
L7_RC_t snoopPTinSourceFind(snoopPTinL3Source_t *sourceList, L7_inet_addr_t sourceAddr, L7_uint8* foundIdx);

//Adds a new source to the given source list in the first free index
L7_RC_t snoopPTinSourceAdd(snoopPTinL3Interface_t* interfacePtr, L7_inet_addr_t sourceAddr, L7_uint8* newSourceIdx);

//Remove a source entry, reseting all its fields and stopping the associated timer
L7_RC_t snoopPTinSourceRemove(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourceList);

//Initializes an interface
L7_RC_t snoopPTinInitializeInterface(snoopPTinL3Interface_t *snoopEntry,L7_uint32 vlanId, L7_inet_addr_t mcastGroupAddr,L7_uint16 interfaceIdx);

//Remove an interface entry, reseting all its fields and stopping the group and query timers
L7_RC_t snoopPTinInterfaceRemove(snoopPTinL3Interface_t *interfacePtr,L7_uint32 vlanId, L7_inet_addr_t mcastGroupAddr,L7_uint16 interfaceIdx);



/************************************************************************************************************/
/*MGMD Proxy*/
#if 1
L7_RC_t snoopPTinGroupRecordInterfaceAdd(L7_uint32 vlanId, snoopPTinProxyInterface_t* interfacePtr);

L7_RC_t snoopPTinGroupRecordGroupAdd(snoopPTinProxyInterface_t* interfacePtr,L7_uint8 recordType, L7_inet_addr_t   groupAddr,snoopPTinProxyGroup_t*  groupPtr );

L7_RC_t snoopPTinGroupRecordSourcedAdd(snoopPTinProxyGroup_t * groupPtr,L7_inet_addr_t   sourceAddr, snoopPTinProxySource_t* sourcePtr);

L7_RC_t snoopPTinGroupRecordGroupFind(L7_uint32 vlanId,L7_inet_addr_t   *groupAddr,L7_uint8 recordType, snoopPTinProxyGroup_t*  groupPtr );

L7_RC_t snoopPTinGroupRecordSourceFind(L7_uint32 vlanId,L7_inet_addr_t   *groupAddr,L7_uint8 recordType, L7_inet_addr_t   *sourceAddr, snoopPTinProxySource_t*  sourcePtr );

L7_RC_t snoopPTinGroupRecordSourceListAdd( L7_uint32  vlanId, L7_inet_addr_t *groupAddr, L7_uint8 recordType, L7_inet_addr_t   *sourceAddr, L7_uint32 sourceCnt,snoopPTinProxyGroup_t *groupPtr);

L7_RC_t snoopPTinInterfaceDelete(snoopPTinProxyInterface_t* interfacePtr);

L7_RC_t snoopPTinGroupDeleteAll(snoopPTinProxyInterface_t* interfacePtr,L7_inet_addr_t* groupAddr);

L7_RC_t snoopPTinGroupRecordDelete(snoopPTinProxyInterface_t* interfacePtr, L7_inet_addr_t* groupAddr,L7_uint8 recordType);

L7_RC_t snoopPTinGroupRecordSourceDelete(snoopPTinProxyGroup_t*   groupPtr, L7_inet_addr_t *sourceAddr);

L7_RC_t snoopPTinGroupRecordSourceDeleteAll(snoopPTinProxyGroup_t*   groupPtr);

L7_RC_t snoopPTinPendingReport2GeneralQuery(L7_uint32 vlanId, L7_BOOL* pendingReport, L7_uint32* timeout, snoopPTinProxyInterface_t* interfacePtr);

L7_RC_t snoopPTinPendingReport2GroupQuery(snoopPTinL3InfoData_t* avlTreeEntry, snoopPTinProxyInterface_t* interfacePtr, L7_BOOL *pendingReport, L7_uint32 *timeout, snoopPTinProxyGroup_t* groupPtr);

L7_RC_t snoopPTinGeneralQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, snoopPTinProxyGroup_t* groupPtr,L7_uint32 *noOfRecords, L7_uint32 *timeout);

L7_RC_t snoopPTinGroupSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, snoopPTinProxyGroup_t* groupPtr, L7_uint32 *timeout);

L7_RC_t snoopPTinGroupSourceSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList, L7_uint32 selectedDelay, L7_BOOL *sendReport, snoopPTinProxyGroup_t* groupPtr, L7_uint32 *timeOut);

L7_RC_t snoopPTinBuildCSR(L7_uint32 vlanId,snoopPTinProxyGroup_t *groupPtr, L7_uint32 *noOfRecords);


#endif
/*End MGMD Proxy*/
/************************************************************************************************************/



#endif /* SNOOPING_PTIN_DB_H_ */
