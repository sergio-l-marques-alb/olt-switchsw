/*
 * snooping_ptin_util.h
 *
 *  Created on: 2 de Ago de 2012
 *      Author: Daniel Figueira
 */

#ifndef SNOOPING_PTIN_UTIL_H_
#define SNOOPING_PTIN_UTIL_H_

#include "datatypes.h"
#include "snooping_ptin_defs.h"
#include "osapi.h"

//Convert an IP address in integer format to string
char* snoopPTinIPv4AddrPrint(L7_uint32 ip, char* buffer);

//Check if a given timer is running
L7_BOOL snoopPTinIsTimerRunning(osapiTimerDescr_t *timerPtr);

//Schedule LMQC Group or Group/Source Specific Query transmissions
L7_RC_t snoopPTinQuerySchedule(L7_uint16 vlanId, L7_uint32 groupAddr, L7_BOOL sFlag, L7_uint32 *sources, L7_uint8 sourcesCnt);

//Debug method that prints stored information for a specific multicast group
void snoopPTinMcastgroupPrint(L7_uint32 groupAddr, L7_uint32 vlanId);

L7_RC_t snoopPTinL2ClientAdd      ();  //PLACEHOLDER
L7_RC_t snoopPTinL2ClientRemove   ();  //PLACEHOLDER

#endif /* SNOOPING_PTIN_UTIL_H_ */
