
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
#ifndef SNMPCONFIGV3_H
#define SNMPCONFIGV3_H

#include "l7_common.h"                    

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMPv3 User Table
*
* @param    name        User name
* @param    access      L7_SNMP_COMM_ACCESS_LEVEL_READ_ONLY - READ access
*                       L7_SNMP_COMM_ACCESS_LEVEL_READ_WRITE - WRITE access
* @param    authProto   L7_SNMP_USER_AUTH_PROTO_NONE - no authentication protocol
*                       L7_SNMP_USER_AUTH_PROTO_HMACMD5 - MD5
*                       L7_SNMP_USER_AUTH_PROTO_HMACSHA - SHA
* @param    privProto   L7_SNMP_USER_PRIV_PROTO_NONE - no privacy protocol
*                       L7_SNMP_USER_PRIV_PROTO_DES - DES
*                       Note: can't be DES if authProto is NONE
* @param    authKey     authentication password (must be "" if authProto is NONE)
* @param    privKey     privacy password (must be "" if privProto is NONE)
*
* @param    restore     L7_TRUE - Restore of the users from CLI, so shouldn't
*                       overwrite existing SNMP users
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t UserTableSet(L7_char8 *name, L7_uint32 access, L7_uint32 authProto, L7_uint32 privProto, L7_uchar8 *authKey, L7_uchar8 *privKey, L7_BOOL restore);

/*********************************************************************
*
* @purpose  Delete an entry from the SNMPv3 User Table
*
* @param    name        User name
* @param    access      L7_SNMP_COMM_ACCESS_LEVEL_READ_ONLY - READ access
*                       L7_SNMP_COMM_ACCESS_LEVEL_READ_WRITE - WRITE access
* @param    authProto   L7_SNMP_USER_AUTH_PROTO_NONE - no authentication protocol
*                       L7_SNMP_USER_AUTH_PROTO_HMACMD5 - MD5
*                       L7_SNMP_USER_AUTH_PROTO_HMACSHA - SHA
* @param    privProto   L7_SNMP_USER_PRIV_PROTO_NONE - no privacy protocol
*                       L7_SNMP_USER_PRIV_PROTO_DES - DES
*                       Note: can't be DES if authProto is NONE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t UserTableDelete(L7_char8 *name, L7_uint32 access, L7_uint32 authProto, L7_uint32 privProto);

/*********************************************************************
*
* @purpose  Returns status of encryption code in the SNMPv3 User Table
*
* @param    void
*
* @returns  L7_TRUE     if encryption code is present
* @returns  L7_FALSE    if not
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL UserTableHasEncryption();

/* End Function Prototypes */

#endif /* SNMPCONFIGV3_H */
