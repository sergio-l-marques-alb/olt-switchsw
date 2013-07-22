/*
 * snooping_ptin_util.h
 *
 *  Created on: 2 de Ago de 2012
 *      Author: Daniel Figueira
 */

#ifndef SNOOPING_PTIN_UTIL_H_
#define SNOOPING_PTIN_UTIL_H_

#include "datatypes.h"
#include "l3_addrdefs.h"
#include "snooping_ptin_defs.h"
#include "osapi.h"
#include "comm_mask.h"

#include "snooping.h"

//Check if a given timer is running
L7_BOOL snoopPTinIsTimerRunning(osapiTimerDescr_t *timerPtr);

//Schedule LMQC Group or Group/Source Specific Query transmissions
L7_RC_t snoopPTinQuerySchedule(L7_uint16 vlanId, L7_inet_addr_t* groupAddr, L7_BOOL sFlag, L7_inet_addr_t *sources, L7_uint8 sourcesCnt);

//Schedule LMQC Group or Group/Source Specific Query transmissions
L7_RC_t snoopPTinReportSchedule(L7_uint32 vlanId, L7_inet_addr_t* groupAddr, L7_uint8  reportType,L7_uint32 selectedDelay, L7_BOOL isInterface,L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr );

//Debug method that prints stored information for a specific multicast group
void snoopPTinMcastgroupPrint(L7_uint32 vlanId,L7_uint32 groupAddrText);

//Debug method that prints stored information for a specific group record
void snoopPTinGroupRecordPrint(L7_uint32 vlanId,L7_uint32 groupAddrText,L7_uint8 recordType);

L7_RC_t snoopPTinL2ClientAdd      ();  //PLACEHOLDER
L7_RC_t snoopPTinL2ClientRemove   ();  //PLACEHOLDER

L7_RC_t snoopPTinProxyL2ClientAdd      ();  //PLACEHOLDER
L7_RC_t snoopPTinProxyL2ClientRemove   ();  //PLACEHOLDER
#endif /* SNOOPING_PTIN_UTIL_H_ */
