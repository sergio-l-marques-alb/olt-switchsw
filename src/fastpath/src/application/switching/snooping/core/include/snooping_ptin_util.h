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

//Convert an IPv4 address in integer format to string
char *snoopPTinIPv4AddrPrint(L7_uint32 ip, char* buffer);

//Convert an IP address in integer format to string
char *snoopPTinIPAddrPrint(const L7_inet_addr_t addr, L7_uchar8* buf);

//Check if a given timer is running
L7_BOOL snoopPTinIsTimerRunning(osapiTimerDescr_t *timerPtr);

//Schedule LMQC Group or Group/Source Specific Query transmissions
L7_RC_t snoopPTinQuerySchedule(L7_uint16 vlanId, L7_inet_addr_t groupAddr, L7_BOOL sFlag, L7_inet_addr_t *sources, L7_uint8 sourcesCnt);

//Schedule LMQC Group or Group/Source Specific Query transmissions
L7_RC_t snoopPTinReportSchedule(L7_uint32 vlanId, L7_inet_addr_t groupAddr, L7_uint8  reportType,L7_uint32 selectedDelay, L7_BOOL isInterface,L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr );

//Debug method that prints stored information for a specific multicast group
void snoopPTinMcastgroupPrint(L7_INTF_MASK_t rootIntfList,L7_uint32 vlanId,L7_inet_addr_t  groupAddrStr);
L7_RC_t snoopPTinL2ClientAdd      ();  //PLACEHOLDER
L7_RC_t snoopPTinL2ClientRemove   ();  //PLACEHOLDER

L7_RC_t snoopPTinProxyL2ClientAdd      ();  //PLACEHOLDER
L7_RC_t snoopPTinProxyL2ClientRemove   ();  //PLACEHOLDER
#endif /* SNOOPING_PTIN_UTIL_H_ */
