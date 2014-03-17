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
ptinMgmdGroupInfoData_t*      ptinMgmdL3EntryFind(uint32 serviceId, ptin_mgmd_inet_addr_t* mcastGroupAddr,uint32 flag);
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
ptinMgmdQuerierInfoData_t*  ptinMgmdQueryEntryFind(uint32 serviceId, uchar8 family,uint32 flag);
ptinMgmdQuerierInfoData_t*  ptinMgmdQueryEntryAdd(uint32 serviceId,uchar8 family, BOOL* newEntry);
RC_t                        ptinMgmdQueryEntryDelete(uint32 serviceId,uchar8 family);

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
RC_t                       ptinMgmdMembershipReportToIncludeProcess(ptin_mgmd_eb_t *pMgmdEB, ptinMgmdGroupInfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList, ptin_IgmpProxyCfg_t* igmpCfg);
RC_t                       ptinMgmdMembershipReportToExcludeProcess(ptin_mgmd_eb_t *pMgmdEB, ptinMgmdGroupInfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList, ptin_IgmpProxyCfg_t* igmpCfg);
RC_t                       ptinMgmdMembershipReportAllowProcess(ptin_mgmd_eb_t* pMgmdEB, ptinMgmdGroupInfoData_t* avlTreeEntry, uint32 intIfNum, uint32 clientIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t* sourceList, ptin_IgmpProxyCfg_t* igmpCfg);
RC_t                       ptinMgmdMembershipReportBlockProcess(ptinMgmdGroupInfoData_t *groupEntry, uint32 portId, uint32 clientId, ushort16 noOfSourcesInput, ptin_mgmd_inet_addr_t *sourceList, ptin_IgmpProxyCfg_t* igmpCfg);
mgmdProxyInterface_t*      ptinMgmdGeneralQueryProcess(uint32 serviceId, uint32 selectedDelay, BOOL *sendReport, uint32 *timeout);
mgmdGroupRecord_t*         ptinMgmdGroupSpecifcQueryProcess(ptinMgmdGroupInfoData_t* avlTreeEntry, uint32 selectedDelay, BOOL* sendReport, uint32* timeout);
mgmdGroupRecord_t*         ptinMgmdGroupSourceSpecifcQueryProcess(ptinMgmdGroupInfoData_t* avlTreeEntry, uint32 rootIntIdx, ushort16 noOfSources, ptin_mgmd_inet_addr_t *sourceList, uint32 selectedDelay, BOOL *sendReport, uint32 *timeout);

/******************************************************************************
 * MGMD utility methods for the internal structures
 ******************************************************************************/

ptinMgmdSource_t*          ptinMgmdSourceFind(ptinMgmdGroupInfoData_t *groupEntry, uint32 portId, ptin_mgmd_inet_addr_t *sourceAddr);
RC_t                       ptinMgmdSourceAdd(ptinMgmdGroupInfoData_t* groupEntry, uint32 portId, ptin_mgmd_inet_addr_t* sourceAddr,ptinMgmdSource_t** sourcePtr,ptin_mgmd_externalapi_t*  externalApi);
RC_t                       ptinMgmdSourceRemove(ptinMgmdGroupInfoData_t *avlTreeEntry,uint32 portId, ptinMgmdSource_t *sourcePtr);
RC_t                       ptinMgmdInitializeInterface(ptinMgmdGroupInfoData_t* groupPtr, uint16 portId);
RC_t                       ptinMgmdInterfaceRemove(ptinMgmdGroupInfoData_t *avlTreeEntry, uint32 intIfNum);
mgmdProxyInterface_t*      ptinMgmdProxyInterfaceAdd(uint32 serviceId);
mgmdGroupRecord_t*         ptinMgmdGroupRecordAdd(mgmdProxyInterface_t* interfacePtr,uint8 recordType, ptin_mgmd_inet_addr_t* groupAddr, BOOL* newEntryFlag);
RC_t                       ptinMgmdGroupRecordSourcedAdd(mgmdGroupRecord_t* groupPtr,ptin_mgmd_inet_addr_t* sourceAddr);
RC_t                       ptinMgmdGroupRecordFind(uint32 serviceId,ptin_mgmd_inet_addr_t   *groupAddr,uint8 recordType, mgmdGroupRecord_t*  groupPtr );
RC_t                       ptinMgmdGroupRecordSourceFind(uint32 serviceId,ptin_mgmd_inet_addr_t   *groupAddr,uint8 recordType, ptin_mgmd_inet_addr_t   *sourceAddr, snoopPTinSourceRecord_t*  sourcePtr );
RC_t                       ptinMgmdProxyInterfaceRemove(mgmdProxyInterface_t* interfacePtr);
RC_t                       ptinMgmdGroupRecordRemove(mgmdProxyInterface_t* interfacePtr, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType);
RC_t                       ptinMgmdGroupRecordSourceRemove(mgmdGroupRecord_t*   groupPtr, ptin_mgmd_inet_addr_t *sourceAddr);

#endif //PTIN_MGMD_DB_H_
