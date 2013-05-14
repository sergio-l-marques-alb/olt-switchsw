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

//Search a specific source in a given source list
L7_RC_t snoopPTinSourceFind(snoopPTinL3Source_t *sourceList, L7_uint32 sourceAddr, L7_uint8* foundIdx);

//Adds a new source to the given source list in the first free index
L7_RC_t snoopPTinSourceAdd(snoopPTinL3Interface_t *interfacePtr, L7_uint32 sourceAddr, L7_uint8* newSourceIdx);

//Remove a source entry, reseting all its fields and stopping the associated timer
L7_RC_t snoopPTinSourceRemove(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourceList);

//Initializes an interface
void snoopPTinInitializeInterface(snoopPTinL3Interface_t *snoopEntry);

//Remove an interface entry, reseting all its fields and stopping the group and query timers
L7_RC_t snoopPTinInterfaceRemove(snoopPTinL3Interface_t *interfacePtr);

#endif /* SNOOPING_PTIN_DB_H_ */
