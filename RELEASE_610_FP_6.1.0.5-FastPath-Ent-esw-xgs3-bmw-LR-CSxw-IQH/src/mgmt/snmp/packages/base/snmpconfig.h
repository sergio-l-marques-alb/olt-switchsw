/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmpconfig.h
*
* Purpose: API interface for managing SNMP configuration
*
* Created by: Colin Verne 01/31/2001
*
* Component: SNMP
*
*********************************************************************/
/*********************************************************************
                        
**********************************************************************
*********************************************************************/
#ifndef SNMPCONFIG_H
#define SNMPCONFIG_H

#include "l7_common.h"
#include "snmp_util_api.h"
#include "l3_addrdefs.h"

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Community Table
*
* @param    index    Index of community to add/set
* @param    name     Community name
* @param    access   0 - READ, 1 - WRITE access
* @param    ip       IP to restrict access to
* @param    ipMask   NetMask associated with ip
* @param    status   status of this community entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t CommunityTableSet(L7_uint32 index, L7_char8 *communityName, L7_uint32 access, L7_uint32 ip, L7_uint32 ipMask, L7_uint32 status);

/*********************************************************************
*
* @purpose  deletes a community from the SNMP Community Table
*
* @param    index    Index of community to delete
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t CommunityTableDelete(L7_uint32 index, L7_char8 *communityName);

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Trap Reciever table
*
* @param    index    Index of Trap Receiver to add/set
* @param    addr     Destination IP to send traps to
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t TrapReceiverTableSet(L7_uint32 index, L7_char8 *communityName, L7_sockaddr_union_t *addr, L7_uint32 status, snmpTrapVer_t version);

/*********************************************************************
*
* @purpose  Deletes an entry from the SNMP Trap Reciever table
*
* @param    index    Index of Trap Receiver to delete
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t TrapReceiverTableDelete(L7_uint32 index);

/* End Function Prototypes */

#endif /* SNMPCONFIG_H */
