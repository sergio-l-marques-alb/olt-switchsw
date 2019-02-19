
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmpconfig.c
*
* Purpose: API interface for managing SNMP configuration
*
* Created by: Colin Verne 01/31/2001
*
* Component: SNMP
*
*********************************************************************/
#include <stdio.h>
#include <string.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "coex/coexdefs.h"
#include "snmpv3/v3defs.h"

#include "snmpconfig.h"
#include "snmpconfigv3.h"
#include "snmp_exports.h"

#define SNMP_ADD 1
#define SNMP_SET 2
#define SNMP_DEL 3

extern int          ManageSecurityToGroupEntry(char *SecurityModel, char *SecurityName, char *GroupName, int storage, SR_UINT32 FLAG);

extern int          ManageUserEntry(OctetString *snmpEngineID, char *Name, char *authprot, char *privprot, int storage, char *target, char *authpass, char *privpass, SR_UINT32 FLAG);

extern int          ManageTargetAddrEntry(char *Name, char *TDomain, char *TAddr, int timeout, int retries, char *TagList, char *Params, int storage, char *TMask, int MMS, SR_UINT32 FLAG);

extern int          ManageTargetParamsEntry(char *Name, int MPModel, char *securityModel, char *SecurityName, char *securityLevel, int storage, SR_UINT32 FLAG);

/* Begin Function Declarations: snmpconfig.h */

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMPv3 User Table
*
* @param    name        User name
* @param    access      L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY - READ access
*                       L7_AGENT_COMM_ACCESS_LEVEL_READ_WRITE - WRITE access
* @param    authProto   L7_AGENT_USER_AUTH_PROTO_NONE - no authentication protocol
*                       L7_AGENT_USER_AUTH_PROTO_HMACMD5 - MD5
*                       L7_AGENT_USER_AUTH_PROTO_HMACSHA - SHA
* @param    privProto   L7_AGENT_USER_PRIV_PROTO_NONE - no privacy protocol
*                       L7_AGENT_USER_PRIV_PROTO_DES - DES
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
L7_RC_t UserTableSet(L7_char8 *name, L7_uint32 access, L7_uint32 authProto, L7_uint32 privProto, L7_uchar8 *authKey, L7_uchar8 *privKey, L7_BOOL restore)
{
  char                securityName[25];
  char                securityAccessName[15];
  char                authProtoName[25];
  char                privProtoName[20];

  bzero(securityAccessName, sizeof(securityAccessName));
  if (authProto == L7_SNMP_USER_AUTH_PROTO_NONE &&
      privProto == L7_SNMP_USER_PRIV_PROTO_NONE)
  {
    sprintf(securityAccessName, "noAuthNoPriv");
  }
  else if (authProto != L7_SNMP_USER_AUTH_PROTO_NONE &&
           privProto == L7_SNMP_USER_PRIV_PROTO_NONE)
  {
    sprintf(securityAccessName, "authNoPriv");
  }
  else if (authProto != L7_SNMP_USER_AUTH_PROTO_NONE &&
           privProto != L7_SNMP_USER_PRIV_PROTO_NONE)
  {
    sprintf(securityAccessName, "authPriv");
  }
  else
  {
    return L7_FAILURE;
  }

  bzero(securityName, sizeof(securityName));
  switch (access)
  {
  case L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY:
    sprintf(securityName, "READ_%s", securityAccessName);
    break;

  case L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE:
    sprintf(securityName, "WRITE_%s", securityAccessName);
    break;

  default:
    return L7_FAILURE;
    break;
  }

  switch (authProto)
  {
  case L7_SNMP_USER_AUTH_PROTO_NONE:
    sprintf(authProtoName, "usmNoAuthProtocol");
    break;

  case L7_SNMP_USER_AUTH_PROTO_HMACMD5:
    /* If the authentication password is less than allowable, in the case of MD5 
     * return failure
     */
    if (strlen(authKey) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
        return L7_FAILURE;
    sprintf(authProtoName, "usmHMACMD5AuthProtocol");
    break;

  case L7_SNMP_USER_AUTH_PROTO_HMACSHA:
    /* If the authentication password is less than allowable, in the case of SHA 
     * return failure
     */
    if (strlen(authKey) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
        return L7_FAILURE;
    sprintf(authProtoName, "usmHMACSHAAuthProtocol");
    break;

  default:
    return L7_FAILURE;
    break;
  }

  switch (privProto)
  {
  case L7_SNMP_USER_PRIV_PROTO_NONE:
    sprintf(privProtoName, "usmNoPrivProtocol");
    break;

  case L7_SNMP_USER_PRIV_PROTO_DES:
    /* If the privacy password is less than allowable, in the case of DES, return 
     * error
     */
    if (strlen(privKey) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN)
        return L7_FAILURE;
    sprintf(privProtoName, "usmDESPrivProtocol");
    break;

  default:
    return L7_FAILURE;
    break;
  }

  /* add security to group entries */
  /* usm */
  if (ManageSecurityToGroupEntry("usm", name, securityName, D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if(restore == L7_FALSE)
    {
      if (ManageSecurityToGroupEntry("usm", name, securityName, D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
        return L7_FAILURE;
    }
  }

  /* try to add the user */
  if (ManageUserEntry(NULL, name, authProtoName, privProtoName, D_usmUserStorageType_nonVolatile, "-", authKey, privKey, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if(restore == L7_FALSE)
    {
      if (ManageUserEntry(NULL, name, authProtoName, privProtoName, D_usmUserStorageType_nonVolatile, "-", authKey, privKey, SNMP_SET) == -1)
        return L7_FAILURE; /* couldn't set for some reason */
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t UserTableDelete(L7_char8 *name, L7_uint32 access, L7_uint32 authProto, L7_uint32 privProto)
{

  /* delete security to group entries */
  /* usm */
  ManageSecurityToGroupEntry("usm", name, "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);

  /* delete the user */
  ManageUserEntry(NULL, name, "", "", D_usmUserStorageType_nonVolatile, "", "", "", SNMP_DEL);

  return L7_SUCCESS;
}

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
L7_BOOL
UserTableHasEncryption()
{
  return L7_TRUE;
}

/* End Function Declarations */

