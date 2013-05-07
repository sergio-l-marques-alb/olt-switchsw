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

//Initializes an interface
void snoopPTinInitializeInterface(snoopPTinL3Interface_t *snoopEntry);

//Processes a Source Specific Membership Report with record type MODE_IS_INCLUDE
L7_RC_t snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList);

//Processes a Source Specific Membership Report with record type MODE_IS_EXCLUDE
L7_RC_t snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList);

//Processes a Source Specific Membership Report with record type MODE_TO_INCLUDE
L7_RC_t snoopPTinMembershipReportToIncludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList);

//Processes a Source Specific Membership Report with record type MODE_TO_EXCLUDE
L7_RC_t snoopPTinMembershipReportToExcludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList);

//Processes a Source Specific Membership Report with record type ALLOW_NEW_SOURCES
L7_RC_t snoopPTinMembershipReportAllowProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList);

//Processes a Source Specific Membership Report with record type BLOCK_OLD_SOURCES
L7_RC_t snoopPTinMembershipReportBlockProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList);

L7_RC_t  snoopPTinClientFind           (L7_uint32 *clientList, L7_uint32 clientIdx);
L7_RC_t  snoopPTinClientAdd            (snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx);
L7_RC_t  snoopPTinClientRemove         (snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx);
L7_RC_t  snoopPTinSourceFind           (snoopPTinL3Source_t *sourceList, L7_uint32 sourceAddr, L7_uint8* foundIdx);
L7_RC_t  snoopPTinSourceAdd            (snoopPTinL3Interface_t *interfacePtr, L7_uint32 sourceAddr, L7_uint8* newSourceIdx);
L7_RC_t  snoopPTinSourceRemove         (snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourceList);
L7_RC_t  snoopPTinInterfaceRemove      (snoopPTinL3Interface_t *interfacePtr);

#endif /* SNOOPING_PTIN_DB_H_ */
