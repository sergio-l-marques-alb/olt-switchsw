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
/*************************************************************
                    
*************************************************************/



#include "sr_snmp.h"
#include "sr_type.h"
#include "coex/coexdefs.h"
#include <stdio.h>
#include "snmpconfig.h"
#include "snmp_util_api.h"
#include "osapi_support.h"

#include <stdio.h>
#include "snmp_exports.h"
#include "dtlapi.h"

#define SNMP_ADD 1
#define SNMP_SET 2
#define SNMP_DEL 3


extern int
ManageSecurityToGroupEntry(
    const char *SecurityModel,
    const char *SecurityName,
    const char *GroupName,
    int storage,
    SR_UINT32 FLAG);

extern int 
ManageTargetAddrEntry(
    const char *Name,
    const char *TDomain,
    char *TAddr,
    int timeout,
    int retries,
    const char *TagList,
    const char *Params,
    int storage,
    char *TMask,
    int MMS,
    SR_UINT32 FLAG);

extern int 
ManageCommunityEntry(
   const char        *communityIndex,       /* snmpCommunityIndex */
   const char        *communityName,        /* snmpCommunityName */
   const char        *securityName,         /* snmpCommunitySecurityName */
   OctetString       *contextEngineID,      /* snmpCommunityContextEngineID */
   const char        *contextName,          /* snmpCommunityContextName */
   const char        *target,               /* snmpCommunityTransportTag */
   int         storage,               /* snmpCommunityStorage */
   int        status,
   SR_UINT32   FLAG);

extern int       
ManageTargetParamsEntry(const char *name,	      /* snmpTargetParamsName */
			int   mpModel,             /* snmpTargetParamsMPModel */
			const char *securityModel, /* snmpTargetParamsSecurityModel*/
			const char *securityName,   /* snmpTargetParamsSecurityName */
			const char *securityLevel,  /* snmpTargetParamsSecurityLevel*/
			int  storage,	      /* snmpTargetParamsStorageType */
			SR_UINT32 FLAG);


/* Begin Function Declarations: snmpconfig.h */

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
L7_RC_t CommunityTableSet(L7_uint32 index, L7_char8 *communityName, L7_uint32 access, L7_uint32 ip, L7_uint32 ipMask, L7_uint32 status)
{
  char                targetIndex[20];
  char                targetTaglist[20];
  char                targetIp[20];
  char                targetIpMask[20];
  char                communityIndex[10];
  char                securityIndex[10];
  char                securityName[10];
  int                 rowStatus = FALSE;

  sprintf(communityIndex, "c%07d", index);
  sprintf(securityIndex, "c%07d", index);

  if (status == L7_SNMP_COMMUNITY_STATUS_VALID)
    rowStatus = TRUE;

  if (access == L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY)
  {
    sprintf(securityName, "READ");
  }
  else
  {
    sprintf(securityName, "WRITE");
  }

  sprintf(targetIndex, "CommunityTarget%d", index);

  /* add security to group entries */

  /* snmpv1 */
  if (ManageSecurityToGroupEntry("snmpv1", securityIndex, securityName, D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if (ManageSecurityToGroupEntry("snmpv1", securityIndex, securityName, D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
      return L7_FAILURE;
  }

  /* snmpv2c */
  if (ManageSecurityToGroupEntry("snmpv2c", securityIndex, securityName, D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if (ManageSecurityToGroupEntry("snmpv2c", securityIndex, securityName, D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
      return L7_FAILURE;
  }

  sprintf(targetIp, "%d.%d.%d.%d:0", (ip >> 3*8) & 0xFF, (ip >> 2*8) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
  sprintf(targetIpMask, "%d.%d.%d.%d:0", (ipMask >> 3*8) & 0xFF, (ipMask >> 2*8) & 0xFF, (ipMask >> 8) & 0xFF, ipMask & 0xFF);

  sprintf(targetTaglist, (ip == 0) ? "-" : "CommunityTag%d", index);

  /* try to add the community target */
  if (ManageTargetAddrEntry(targetIndex, "snmpUDPDomain", targetIp, 0, 0, targetTaglist, "-", D_snmpCommunityStorageType_nonVolatile, targetIpMask, 65535, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if (ManageTargetAddrEntry(targetIndex, "snmpUDPDomain", targetIp, 0, 0, targetTaglist, "-", D_snmpCommunityStorageType_nonVolatile, targetIpMask, 65535, SNMP_SET) == -1)
      return L7_FAILURE; /* something failed */
  }

  /* try to add the community */
  if (ManageCommunityEntry(communityIndex, communityName, securityIndex, NULL, "-", targetTaglist, D_snmpCommunityStorageType_nonVolatile, rowStatus, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if (ManageCommunityEntry(communityIndex, communityName, securityIndex, NULL, "-", targetTaglist, D_snmpCommunityStorageType_nonVolatile, rowStatus, SNMP_SET) == -1)
      return L7_FAILURE; /* couldn't set for some reason */
  }

  return L7_SUCCESS;
}

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
L7_RC_t CommunityTableDelete(L7_uint32 index, L7_char8 *communityName)
{
  char                communityIndex[20];
  char                securityIndex[20];
  char                targetIndex[20];

  sprintf(communityIndex, "c%07d", index);
  sprintf(securityIndex, "c%07d", index);
  sprintf(targetIndex, "CommunityTarget%d", index);

  /* try to delete Security table entries */
  ManageSecurityToGroupEntry("snmpv1", securityIndex, "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);
  ManageSecurityToGroupEntry("snmpv2c", securityIndex, "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);

  /* try to delete target entry created for this community index */
  ManageTargetAddrEntry(targetIndex, "", "", 0, 0, "", "", 0, "", 0, SNMP_DEL);

  /* try to delete the community entry */
  if (ManageCommunityEntry(communityIndex, "", "", NULL, "-", "", 0, L7_FALSE, SNMP_DEL) == -1)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Trap Reciever table
*
* @param    index    Index of Trap Receiver to add/set
* @param    ip       Destination IP to send traps to
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t TrapReceiverTableSet(L7_uint32 index, L7_char8 *communityName, L7_sockaddr_union_t *addr, L7_uint32 status, snmpTrapVer_t version)
{
  char                securityName[10];
  char                versionIndex[15];
  char                targetIp[48];
  int                 rowStatus = FALSE;
  L7_uint32           ip;
  char               *TDomain = NULL;

  if (addr == L7_NULL) /* Check for 0.0.0.0 or IPv6 unspecified below */
    return L7_FAILURE; /* Need an non-NULL address */

  if( addr->u.sa.sa_family == L7_AF_INET)
  {
     ip = osapiNtohl(addr->u.sa4.sin_addr.s_addr);
     if (ip == 0)
       return L7_FAILURE; /* IP must be non-zero */
     sprintf(targetIp, "%d.%d.%d.%d:0", (ip >> 3*8) & 0xFF, (ip >> 2*8) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
     TDomain = "snmpUDPDomain";
  }
  else if( addr->u.sa.sa_family == L7_AF_INET6)
  {
     if(L7_IP6_IS_ADDR_UNSPECIFIED(&addr->u.sa6.sin6_addr))
       return L7_FAILURE; /* IP must be non-zero */
     osapiInetNtop(L7_AF_INET6, (void *)&addr->u.sa6.sin6_addr, targetIp,40);
     TDomain = "transportDomainUdpIpv6";
  }

  sprintf(securityName, "t%07d", index);

  if (status == L7_SNMP_TRAP_MGR_STATUS_VALID)
    rowStatus = TRUE;

  if (version & L7_SNMP_TRAP_VER_SNMPV1)
  {
  /* snmpv1 */
    /* add security to group entry */
    if (ManageSecurityToGroupEntry("snmpv1", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
    {
      /* if it already exists, try setting it */
      if (ManageSecurityToGroupEntry("snmpv1", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
        return L7_FAILURE; /* couldn't set for some reason */
    }

    sprintf(versionIndex, "%s_v1", securityName);

    /* set target parameters */
    if (ManageTargetParamsEntry(versionIndex, 0, "snmpv1", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
    {
      /* if it already exists, try setting it */
      if (ManageTargetParamsEntry(versionIndex, 0, "snmpv1", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
        return L7_FAILURE; /* couldn't set for some reason */
    }

    /* try to add the community target */
    if (ManageTargetAddrEntry(versionIndex, TDomain, targetIp, 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_ADD) == -1)
    {
      /* if it already exists, try setting it */
      if (ManageTargetAddrEntry(versionIndex, TDomain, targetIp, 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_SET) == -1)
        return L7_FAILURE; /* couldn't set for some reason */
    }
  }
  else
  {
    /* delete security to group entry */
    (void)ManageSecurityToGroupEntry("snmpv1", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);

    sprintf(versionIndex, "%s_v1", securityName);

    /* set target parameters */
    (void)ManageTargetParamsEntry(versionIndex, 0, "snmpv1", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);
    (void)ManageTargetAddrEntry(versionIndex, "snmpUDPDomain", "", 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_DEL);
  }

  if (version & L7_SNMP_TRAP_VER_SNMPV2C)
  {
  /* snmpv2c */
    /* add security to group entry */
    if (ManageSecurityToGroupEntry("snmpv2c", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
    {
      /* if it already exists, try setting it */
      if (ManageSecurityToGroupEntry("snmpv2c", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
        return L7_FAILURE; /* couldn't set for some reason */
    }

    sprintf(versionIndex, "%s_v2c", securityName);

    /* try to add the community target */
    if (ManageTargetAddrEntry(versionIndex, TDomain, targetIp, 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_ADD) == -1)
    {
      /* if it already exists, try setting it */
      if (ManageTargetAddrEntry(versionIndex, TDomain, targetIp, 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_SET) == -1)
      {
        return L7_FAILURE; /* couldn't set for some reason */
      }
    }

    if (ManageTargetParamsEntry(versionIndex, 1, "snmpv2c", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_ADD) == -1)
    {
      /* if it already exists, try setting it */
      if (ManageTargetParamsEntry(versionIndex, 1, "snmpv2c", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_SET) == -1)
        return L7_FAILURE; /* couldn't set for some reason */
    }
  }
  else
  {
    /* delete security to group entry */
    (void)ManageSecurityToGroupEntry("snmpv2c", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);

    sprintf(versionIndex, "%s_v2c", securityName);

    /* delete the community target */
    (void)ManageTargetAddrEntry(versionIndex, "snmpUDPDomain", "", 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_DEL);
    (void)ManageTargetParamsEntry(versionIndex, 1, "snmpv2c", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);
  }

  /* try to add the trap community */
  if (ManageCommunityEntry(securityName, communityName, securityName, NULL, "-", "-", D_snmpCommunityStorageType_nonVolatile, rowStatus, SNMP_ADD) == -1)
  {
    /* if it already exists, try setting it */
    if (ManageCommunityEntry(securityName, communityName, securityName, NULL, "-", "-", D_snmpCommunityStorageType_nonVolatile, rowStatus, SNMP_SET) == -1)
      return L7_FAILURE; /* couldn't set for some reason */
  }

  return L7_SUCCESS;
}

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
L7_RC_t TrapReceiverTableDelete(L7_uint32 index)
{
  char                securityName[10];
  char                versionIndex[15];

  sprintf(securityName, "t%07d", index);

/* snmpv1 */
  /* delete security to group entry */
  if (ManageSecurityToGroupEntry("snmpv1", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

  sprintf(versionIndex, "%s_v1", securityName);

  /* set target parameters */
  if (ManageTargetParamsEntry(versionIndex, 0, "snmpv1", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

  /* try to delete the community target */
  if (ManageTargetAddrEntry(versionIndex, "snmpUDPDomain", "", 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

/* snmpv2c */
  /* delete security to group entry */
  if (ManageSecurityToGroupEntry("snmpv2c", securityName, "TRAP", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

  sprintf(versionIndex, "%s_v2c", securityName);

  /* delete the community target */
  if (ManageTargetAddrEntry(versionIndex, "snmpUDPDomain", "", 100, 5, "TrapNotify", versionIndex, D_snmpCommunityStorageType_nonVolatile, "255.255.255.255:0", 65535, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

  if (ManageTargetParamsEntry(versionIndex, 1, "snmpv2c", securityName, "noAuthNoPriv", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

  /* delete the trap community */
  if (ManageCommunityEntry(securityName, "", securityName, NULL, "-", "-", D_snmpCommunityStorageType_nonVolatile, FALSE, SNMP_DEL) == -1)
    return L7_FAILURE; /* couldn't delete for some reason */

  return L7_SUCCESS;
}


L7_RC_t OldTrapReceiverTableDelete(L7_uint32 index)
{
  L7_char8            targetIndex[20];

  sprintf(targetIndex, "TrapTarget%d_v1", index);
  /* try to delete Security table entries */
  ManageSecurityToGroupEntry("snmpv1", targetIndex, "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);
  if (ManageTargetParamsEntry(targetIndex, 0, "", "", "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL) == -1)
  {
    return L7_FAILURE;
  }
  if (ManageTargetAddrEntry(targetIndex, "", "", 0, 0, "", "", D_snmpCommunityStorageType_nonVolatile, "", 0, SNMP_DEL) == -1)
  {
    return L7_FAILURE;
  }

  sprintf(targetIndex, "TrapTarget%d_v2", index);
  ManageSecurityToGroupEntry("snmpv2c", targetIndex, "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL);
  if (ManageTargetParamsEntry(targetIndex, 0, "", "", "", D_snmpCommunityStorageType_nonVolatile, SNMP_DEL) == -1)
  {
    return L7_FAILURE;
  }
  if (ManageTargetAddrEntry(targetIndex, "", "", 0, 0, "", "", D_snmpCommunityStorageType_nonVolatile, "", 0, SNMP_DEL) == -1)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/* End Function Declarations */
