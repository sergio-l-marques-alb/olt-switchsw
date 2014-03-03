/*
 * ptin_mgmd_db.h
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Filipe Figueira
 */

#ifndef PTIN_MGMD_DB_H_
#define PTIN_MGMD_DB_H_

#include "ptin_mgmd_core.h"
#include "ptin_mgmd_util.h"
#include "ptin_mgmd_statistics.h"

/******************************************************************************
 * API methods for the AVLs used in MGMD
 ******************************************************************************/
snoopPTinL3InfoData_t*      ptinMgmdL3EntryFind(uint32 serviceId, ptin_mgmd_inet_addr_t* mcastGroupAddr,uint32 flag);
RC_t                        ptinMgmdL3EntryAdd(uint32 serviceId,ptin_mgmd_inet_addr_t* mcastGroupAddr);
RC_t                        ptinMgmdL3EntryDelete(uint32 serviceId,ptin_mgmd_inet_addr_t* mcastGroupAddr);
snoopPTinSourceRecord_t*    ptinMgmdProxySourceEntryFind(mgmdGroupRecord_t* groupPtr, ptin_mgmd_inet_addr_t* sourceAddr,uint32 flag);
snoopPTinSourceRecord_t*    ptinMgmdProxySourceEntryAdd(mgmdGroupRecord_t* groupPtr, ptin_mgmd_inet_addr_t* sourceAddr, BOOL* newEntry);
snoopPTinSourceRecord_t*    ptinMgmdProxySourceEntryDelete(mgmdGroupRecord_t* groupPtr, ptin_mgmd_inet_addr_t* sourceAddr);
mgmdGroupRecord_t*          ptinMgmdProxyGroupEntryFind(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType, uint32 flag);
mgmdGroupRecord_t*          ptinMgmdProxyGroupEntryAdd(mgmdProxyInterface_t* interfacePtr, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType, BOOL* newEntry);
mgmdGroupRecord_t*          ptinMgmdProxyGroupEntryDelete(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType);
mgmdProxyInterface_t*       ptinMgmdProxyInterfaceEntryFind(uint32 serviceId, uint32 flag);
mgmdProxyInterface_t*       ptinMgmdProxyInterfaceEntryAdd(uint32 serviceId, BOOL* newEntry);
RC_t                        ptinMgmdProxyInterfaceEntryDelete(uint32 serviceId);
mgmdPTinQuerierInfoData_t*  ptinMgmdQueryEntryFind(uint16 serviceId, uchar8 family,uint32 flag);
mgmdPTinQuerierInfoData_t*  ptinMgmdQueryEntryAdd(uint16 sId,uchar8 family, BOOL* newEntry);
RC_t                        ptinMgmdQueryEntryDelete(uint16 sId,uchar8 family);

/******************************************************************************
 * API methods for the CTRL events
 ******************************************************************************/
RC_t                       ptinMgmdactivegroups_get(uint32 serviceId, uint32 portId, uint32 clientId, ptin_mgmd_groupInfo_t *channelList, uint32 *numChannels);
RC_t                       ptinMgmdgroupclients_get(uint32 serviceId, uint32 portId, ptin_mgmd_inet_addr_t* groupAddr, ptin_mgmd_inet_addr_t* sourceAddr, uint8* clientList,uint16* numClients);
RC_t                       ptinMgmdAddStaticGroup(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr, uint32 noOfSources, ptin_mgmd_inet_addr_t* sourceAddr);
RC_t                       ptinMgmdRemoveStaticGroup(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, uint32 noOfSources, ptin_mgmd_inet_addr_t* sourceAddr);

/******************************************************************************
 * Methods used for processing IGMP/MLD packets
 ******************************************************************************/
//RC_t                       snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, uint32 portId, uint32 clientIdx, ushort16 noOfSources, ptin_inet_addr_t* sourceList,uint32 *noOfRecords, mgmdGroupRecord_t* groupPtr);
//RC_t                       snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_inet_addr_t* sourceList,uint32 *noOfRecords, mgmdGroupRecord_t* groupPtr);
RC_t                       ptinMgmdMembershipReportToIncludeProcess(mgmd_eb_t *pMgmdEB, snoopPTinL3InfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList);
RC_t                       ptinMgmdMembershipReportToExcludeProcess(mgmd_eb_t *pMgmdEB, snoopPTinL3InfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList);
RC_t                       ptinMgmdMembershipReportAllowProcess(mgmd_eb_t* pMgmdEB, snoopPTinL3InfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList);
RC_t                       ptinMgmdMembershipReportBlockProcess(snoopPTinL3InfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList);
mgmdProxyInterface_t*      ptinMgmdGeneralQueryProcess(uint32 serviceId, uint32 selectedDelay, BOOL *sendReport, uint32 *timeout);
mgmdGroupRecord_t*         ptinMgmdGroupSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, uint32 selectedDelay, BOOL* sendReport, uint32* timeout);
mgmdGroupRecord_t*         ptinMgmdGroupSourceSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, uint32 rootIntIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t *sourceList, uint32 selectedDelay, BOOL *sendReport, uint32 *timeout);

/******************************************************************************
 * MGMD utility methods for the internal structures
 ******************************************************************************/

snoopPTinL3Source_t*       ptinMgmdSourceFind(snoopPTinL3InfoData_t *groupEntry, uint32 portId, ptin_mgmd_inet_addr_t *sourceAddr);
RC_t                       ptinMgmdSourceAdd(snoopPTinL3InfoData_t *avlTreeEntry, uint32 portId, ptin_mgmd_inet_addr_t *sourceAddr,snoopPTinL3Source_t **sourcePtr);
RC_t                       ptinMgmdSourceRemove(snoopPTinL3InfoData_t *avlTreeEntry,uint32 portId, snoopPTinL3Source_t *sourcePtr);
RC_t                       ptinMgmdInitializeInterface(snoopPTinL3InfoData_t* groupPtr, uint16 portId);
RC_t                       ptinMgmdInterfaceRemove(snoopPTinL3InfoData_t *avlTreeEntry, uint32 intIfNum);
mgmdProxyInterface_t*      ptinMgmdProxyInterfaceAdd(uint32 serviceId);
mgmdGroupRecord_t*         ptinMgmdGroupRecordAdd(mgmdProxyInterface_t* interfacePtr,uint8 recordType, ptin_mgmd_inet_addr_t* groupAddr, BOOL* newEntryFlag);
RC_t                       ptinMgmdGroupRecordSourcedAdd(mgmdGroupRecord_t* groupPtr,ptin_mgmd_inet_addr_t* sourceAddr);
RC_t                       ptinMgmdGroupRecordFind(uint32 serviceId,ptin_mgmd_inet_addr_t   *groupAddr,uint8 recordType, mgmdGroupRecord_t*  groupPtr );
RC_t                       ptinMgmdGroupRecordSourceFind(uint32 serviceId,ptin_mgmd_inet_addr_t   *groupAddr,uint8 recordType, ptin_mgmd_inet_addr_t   *sourceAddr, snoopPTinSourceRecord_t*  sourcePtr );
RC_t                       ptinMgmdProxyInterfaceRemove(mgmdProxyInterface_t* interfacePtr);
RC_t                       ptinMgmdGroupRecordRemove(mgmdProxyInterface_t* interfacePtr, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType);
RC_t                       ptinMgmdGroupRecordSourceRemove(mgmdGroupRecord_t*   groupPtr, ptin_mgmd_inet_addr_t *sourceAddr);

#endif //PTIN_MGMD_DB_H_
