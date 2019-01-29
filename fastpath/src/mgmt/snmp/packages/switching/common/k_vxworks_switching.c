/*
 *
 * Copyright (C) 1992-2000 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */


#include "sr_conf.h"

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <m2Lib.h>

#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "sitedefs.h"
#include "trap.h"
#include "diag.h"
SR_FILENAME


#include "k_ipforw.c"
#ifdef OLD_CODE
#include "linktrap.c"
#endif /* OLD_CODE */

#include <snmpsupp.h>

#include "l7_common.h"
#include "usmdb_1213_api.h"
#include "usmdb_2233_stats_api.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

#include "osapi.h"
#include "ipstk_mib_api.h"

/* lvl7_@p0986 start */
#define SNMP_BUFFER_LEN 128
/* lvl7_@p0986 end */

#define M2_SEARCH(x) (((x)==(EXACT)) ? (M2_EXACT_VALUE) : (M2_NEXT_VALUE))
#define M2_SEARCH_TEXT(x) (((x)==(EXACT)) ? "M2_EXACT_VALUE" : "M2_NEXT_VALUE")


extern system_t systemData;

void
print_errno()
{
  switch (errno)
  {
  case S_m2Lib_INVALID_PARAMETER:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_INVALID_PARAMETER\n", errno));
    break;
  case S_m2Lib_ENTRY_NOT_FOUND:
    DPRINTF((APTRACE,
             "errno 0x%x: S_m2Lib_ENTRY_NOT_FOUND\n", errno));
    break;
  case S_m2Lib_TCPCONN_FD_NOT_FOUND:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_TCPCONN_FD_NOT_FOUND\n", errno));
    break;
  case S_m2Lib_INVALID_VAR_TO_SET:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_INVALID_VAR_TO_SET\n", errno));
    break;
  case S_m2Lib_CANT_CREATE_SYS_SEM:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_CANT_CREATE_SYS_SEM\n", errno));
    break;
  case S_m2Lib_CANT_CREATE_IF_SEM:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_CANT_CREATE_IF_SEM\n", errno));
    break;
  case S_m2Lib_CANT_CREATE_ROUTE_SEM:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_CANT_CREATE_ROUTE_SEM\n", errno));
    break;
  case S_m2Lib_ARP_PHYSADDR_NOT_SPECIFIED:
    DPRINTF((APERROR,
             "errno 0x%x: S_m2Lib_ARP_PHYSADDR_NOT_SPECIFIED\n", errno));
    break;
  case S_m2Lib_IF_TBL_IS_EMPTY:
    DPRINTF((APWARN,
             "errno 0x%x: S_m2Lib_IF_TBL_IS_EMPTY\n", errno));
    break;
  case S_m2Lib_IF_CNFG_CHANGED:
    DPRINTF((APWARN,
             "errno 0x%x: S_m2Lib_IF_CNFG_CHANGED\n", errno));
    break;
  default:
    DPRINTF((APERROR,
             "errno 0x%x (unknown)\n", errno));
    break;
  }
}

#ifdef OLD_CODE
void
TrapGenerator(trapType, interfaceIndex, timerInfo)
int trapType;
int interfaceIndex;
void *timerInfo;
{
  FNAME("TrapGenerator")
  switch (trapType)
  {
  case M2_LINK_DOWN_TRAP:
    DPRINTF((APTRAP, "%s: calling send_linkDown_trap()\n", Fname));
    send_linkDown_trap((SR_INT32) interfaceIndex, NULL, NULL);
    break;
  case M2_LINK_UP_TRAP:
    DPRINTF((APTRAP, "%s: callling send_linkUp_trap()\n", Fname));
    send_linkUp_trap((SR_INT32) interfaceIndex, NULL, NULL);
    break;
  default:
    DPRINTF((APWARN, "%s: trapType %d not implemented.\n",
             Fname, trapType));
    break;
  }
}
#endif /* OLD_CODE */

int
k_initialize()
{
  FNAME("k_initialize")
  int err = 1; /* NO ERROR */


#ifndef M2_MAX_ROUTE_TABLE_SIZE





/*
 * %%% DSS - UNDOCUMENTED IN THE VXWORKS 5.2 PROGRAMMER'S GUIDE:
 * The units are rows.
 * If you call m2IpInit with parameter 0 it will pick the default of 40 rows.
 * The default value should be enought for most applications.
 */
#define M2_MAX_ROUTE_TABLE_SIZE 0
#endif /* M2_MAX_ROUTE_TABLE_SIZE */

  return(err);
}

int
k_terminate()
{
  return(1);
}
/* lvl7_@p0095 start */
#if 0 /* removed to allow inclusion of RFC 2233 in extension compile */
/* lvl7_@p0095 end */
interfaces_t *
k_interfaces_get(serialNum, contextInfo, nominator)
int serialNum;
ContextInfo *contextInfo;
int nominator;
{
  FNAME("k_interfaces_get")
  static interfaces_t interfacesData;
  static M2_INTERFACE ifVars;

  DPRINTF((APTRACE, "%s: calling m2IfGroupInfoGet()\n", Fname));
  if (m2IfGroupInfoGet(&ifVars)==OK)
  {
    interfacesData.ifNumber = (SR_INT32) ifVars.ifNumber;
    SET_ALL_VALID(interfacesData.valid);
    return(&interfacesData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IfGroupInfoGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

ifEntry_t *
k_ifEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_INT32 ifIndex;
{
  FNAME("k_ifEntry_get")
  static ifEntry_t ifEntryData;
  static M2_INTERFACETBL interfaceEntry;
  char snmp_buffer[128];
  unsigned int snmp_buffer_uint[128];
  unsigned int snmp_buffer_uint_len = 128;
  unsigned int UnitIndex = USMDB_UNIT_CURRENT;

  SET_ALL_VALID(ifEntryData.valid);
  ifEntryData.ifIndex = ifIndex;
/*
   if (((searchType == EXACT) ?
       usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, ifEntryData.ifIndex) :
       usmDbIfEntryNext(&UnitIndex, &ifEntryData.ifIndex) != L7_SUCCESS) || UnitIndex != USMDB_UNIT_CURRENT)
   {
      ZERO_VALID(ifEntryData.valid);
      return(NULL);
   }

   FreeOctetString(ifEntryData.ifDescr);
   if (usmDbIfDescrGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, snmp_buffer))
   {
      CLR_VALID(I_ifDescr, ifEntryData.valid);
   } else
   {
      if ((ifEntryData.ifDescr = MakeOctetStringFromText(snmp_buffer)) == NULL)
         return(NULL);
   }

   if (usmDbIfTypeGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifType))
      CLR_VALID(I_ifType, ifEntryData.valid);

   if (usmDbIfMtuGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifMtu))
      CLR_VALID(I_ifMtu, ifEntryData.valid);

   if (usmDbIfSpeedGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifSpeed))
      CLR_VALID(I_ifSpeed, ifEntryData.valid);

   FreeOctetString(ifEntryData.ifPhysAddress);
   if (usmDbIfPhysAddressGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, snmp_buffer))
   {
      CLR_VALID(I_ifPhysAddress, ifEntryData.valid);
   } else
   {
      if ((ifEntryData.ifPhysAddress = MakeOctetStringFromText(snmp_buffer)) == NULL)
         return(NULL);
   }


   if (usmDbIfAdminStatusGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifAdminStatus))
      CLR_VALID(I_ifAdminStatus, ifEntryData.valid);

   if (usmDbIfOperStatusGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOperStatus))
      CLR_VALID(I_ifOperStatus, ifEntryData.valid);

   if (usmDbIfLastChangeGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifLastChange))
      CLR_VALID(I_ifLastChange, ifEntryData.valid);


   if (usmDbIfInOctetsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifInOctets))
      CLR_VALID(9, ifEntryData.valid);

   if (usmDbIfInUcastPktsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifInUcastPkts))
      CLR_VALID(I_ifInUcastPkts, ifEntryData.valid);

   if (usmDbIfInNUcastPktsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifInNUcastPkts))
      CLR_VALID(I_ifInNUcastPkts, ifEntryData.valid);

   if (usmDbIfInDiscardsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifInDiscards))
      CLR_VALID(I_ifInDiscards, ifEntryData.valid);

   if (usmDbIfInErrorsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifInErrors))
      CLR_VALID(I_ifInErrors, ifEntryData.valid);

   if (usmDbIfInUnknownProtosGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifInUnknownProtos))
      CLR_VALID(I_ifInUnknownProtos, ifEntryData.valid);

   if (usmDbIfOutOctetsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOutOctets))
      CLR_VALID(15, ifEntryData.valid);

   if (usmDbIfOutUcastPktsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOutUcastPkts))
      CLR_VALID(I_ifOutUcastPkts, ifEntryData.valid);

   if (usmDbIfOutNUcastPktsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOutNUcastPkts))
      CLR_VALID(I_ifOutNUcastPkts, ifEntryData.valid);

   if (usmDbIfOutDiscardsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOutDiscards))
      CLR_VALID(I_ifOutDiscards, ifEntryData.valid);

   if (usmDbIfOutErrorsGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOutErrors))
      CLR_VALID(I_ifOutErrors, ifEntryData.valid);

   if (usmDbIfOutQLenGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, &ifEntryData.ifOutQLen))
      CLR_VALID(I_ifOutQLen, ifEntryData.valid);

   FreeOID(ifEntryData.ifSpecific);
   if (usmDbIfSpecificGet(USMDB_UNIT_CURRENT, ifEntryData.ifIndex, snmp_buffer_uint, &snmp_buffer_uint_len))
      CLR_VALID(I_ifSpecific, ifEntryData.valid);
   else
      ifEntryData.ifSpecific = MakeOID(snmp_buffer_uint, snmp_buffer_uint_len);

   return(&ifEntryData);*/
  return NULL;
}

  #ifdef SETS
int
k_ifEntry_set_defaults(dp)
doList_t       *dp;
{
  ifEntry_t *data = (ifEntry_t *) (dp->data);

  return(NO_ERROR);
}

int
k_ifEntry_set(data, contextInfo, function)
ifEntry_t *data;
ContextInfo *contextInfo;
int function;
{
  FNAME("k_ifEntry_set")
  M2_INTERFACETBL ifEntryToSet;

  if (VALID(I_ifAdminStatus, data->valid))
  {
    switch (data->ifAdminStatus)
    {
    case D_ifAdminStatus_up:
      ifEntryToSet.ifAdminStatus = M2_ifAdminStatus_up;
      break;
    case D_ifAdminStatus_down:
      ifEntryToSet.ifAdminStatus = M2_ifAdminStatus_down;
      break;
    case D_ifAdminStatus_testing:
      ifEntryToSet.ifAdminStatus = M2_ifAdminStatus_testing;
      break;
    }
  }
  else
  {
    return(GEN_ERROR);
  }

  ifEntryToSet.ifIndex = (int) data->ifIndex;

  DPRINTF((APTRACE, "%s: calling m2IfTblEntrySet()\n", Fname));
  if (m2IfTblEntrySet(&ifEntryToSet)==OK)
  {
    return(NO_ERROR);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IfTblEntrySet returned ERROR\n", Fname));
    print_errno();
    return(GEN_ERROR);
  }
}
  #endif /* SETS */

/* lvl7_@p0095 start */
#endif /* end remove */
/* lvl7_@p0095 start */

ip_t *
k_ip_get(serialNum, contextInfo, nominator)
int serialNum;
ContextInfo *contextInfo;
int nominator;
{
  FNAME("k_ip_get")
  static ip_t ipData;
#ifdef OLD_CODE
  static M2_IP ipVars;

  DPRINTF((APTRACE, "%s: calling m2IpGroupInfoGet()\n", Fname));
  if (m2IpGroupInfoGet(&ipVars)==OK)
  {
    ipData.ipForwarding = (SR_INT32) ipVars.ipForwarding;
    ipData.ipDefaultTTL = (SR_INT32) ipVars.ipDefaultTTL;
    ipData.ipInReceives = (SR_UINT32) ipVars.ipInReceives;
    ipData.ipInHdrErrors = (SR_UINT32) ipVars.ipInHdrErrors;
    ipData.ipInAddrErrors = (SR_UINT32) ipVars.ipInAddrErrors;
    ipData.ipForwDatagrams = (SR_UINT32) ipVars.ipForwDatagrams;
    ipData.ipInUnknownProtos = (SR_UINT32) ipVars.ipInUnknownProtos;
    ipData.ipInDiscards = (SR_UINT32) ipVars.ipInDiscards;
    ipData.ipInDelivers = (SR_UINT32) ipVars.ipInDelivers;
    ipData.ipOutRequests = (SR_UINT32) ipVars.ipOutRequests;
    ipData.ipOutDiscards = (SR_UINT32) ipVars.ipOutDiscards;
    ipData.ipOutNoRoutes = (SR_UINT32) ipVars.ipOutNoRoutes;
    ipData.ipReasmTimeout = (SR_INT32) ipVars.ipReasmTimeout;
    ipData.ipReasmReqds = (SR_UINT32) ipVars.ipReasmReqds;
    ipData.ipReasmOKs = (SR_UINT32) ipVars.ipReasmOKs;
    ipData.ipReasmFails = (SR_UINT32) ipVars.ipReasmFails;
    ipData.ipFragOKs = (SR_UINT32) ipVars.ipFragOKs;
    ipData.ipFragFails = (SR_UINT32) ipVars.ipFragFails;
    ipData.ipFragCreates = (SR_UINT32) ipVars.ipFragCreates;
    ipData.ipRoutingDiscards = (SR_UINT32) ipVars.ipRoutingDiscards;
    SET_ALL_VALID(ipData.valid);
    return(&ipData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpGroupInfoGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
#endif /* OLD_CODE */

  ZERO_VALID(ipData.valid);

  switch (nominator)
  {
  case I_ipForwarding:
    if (usmDbIpRtrAdminModeGet(USMDB_UNIT_CURRENT, &ipData.ipForwarding) == L7_SUCCESS)
    {
/* lvl7_@p1112 start */
      SET_VALID(I_ipForwarding, ipData.valid);
      switch(ipData.ipForwarding)
      {
      case L7_ENABLE:
        ipData.ipForwarding = D_ipForwarding_forwarding;
        break;
      case L7_DISABLE:
        ipData.ipForwarding = D_ipForwarding_not_forwarding;
        break;
      default:
        CLR_VALID(I_ipForwarding, ipData.valid);
        break;
      }
    }
/* lvl7_@p1112 end */
    break;

  case I_ipDefaultTTL:
    if (usmDbIpDefaultTTLGet(USMDB_UNIT_CURRENT, &ipData.ipDefaultTTL) == L7_SUCCESS)
      SET_VALID(I_ipDefaultTTL, ipData.valid);
    break;

  case I_ipInReceives:
    if (usmDbIpInReceivesGet(USMDB_UNIT_CURRENT, &ipData.ipInReceives) == L7_SUCCESS)
      SET_VALID(I_ipInReceives, ipData.valid);
    break;

  case I_ipInHdrErrors:
    if (usmDbIpInHdrErrorsGet(USMDB_UNIT_CURRENT, &ipData.ipInHdrErrors) == L7_SUCCESS)
      SET_VALID(I_ipInHdrErrors, ipData.valid);
    break;

  case I_ipInAddrErrors:
    if (usmDbIpInAddrErrorsGet(USMDB_UNIT_CURRENT, &ipData.ipInAddrErrors) == L7_SUCCESS)
      SET_VALID(I_ipInAddrErrors, ipData.valid);
    break;

  case I_ipForwDatagrams:
    if (usmDbIpForwDatagramsGet(USMDB_UNIT_CURRENT, &ipData.ipForwDatagrams) == L7_SUCCESS)
      SET_VALID(I_ipForwDatagrams, ipData.valid);
    break;

  case I_ipInUnknownProtos:
    if (usmDbIpInUnknownProtosGet(USMDB_UNIT_CURRENT, &ipData.ipInUnknownProtos) == L7_SUCCESS)
      SET_VALID(I_ipInUnknownProtos, ipData.valid);
    break;

  case I_ipInDiscards:
    if (usmDbIpInDiscards(USMDB_UNIT_CURRENT, &ipData.ipInDiscards) == L7_SUCCESS)
      SET_VALID(I_ipInDiscards, ipData.valid);
    break;

  case I_ipInDelivers:
    if (usmDbIpInDelivers(USMDB_UNIT_CURRENT, &ipData.ipInDelivers) == L7_SUCCESS)
      SET_VALID(I_ipInDelivers, ipData.valid);
    break;

  case I_ipOutRequests:
    if (usmDbIpOutRequests(USMDB_UNIT_CURRENT, &ipData.ipOutRequests) == L7_SUCCESS)
      SET_VALID(I_ipOutRequests, ipData.valid);
    break;

  case I_ipOutDiscards:
    if (usmDbIpOutDiscardsGet(USMDB_UNIT_CURRENT, &ipData.ipOutDiscards) == L7_SUCCESS)
      SET_VALID(I_ipOutDiscards, ipData.valid);
    break;

  case I_ipOutNoRoutes:
    if (usmDbIpNoRoutesGet(USMDB_UNIT_CURRENT, &ipData.ipOutNoRoutes) == L7_SUCCESS)
      SET_VALID(I_ipOutNoRoutes, ipData.valid);
    break;

  case I_ipReasmTimeout:
    if (usmDbIpReasmTimeoutGet(USMDB_UNIT_CURRENT, &ipData.ipReasmTimeout) == L7_SUCCESS)
      SET_VALID(I_ipReasmTimeout, ipData.valid);
    break;

  case I_ipReasmReqds:
    if (usmDbIpReasmReqdsGet(USMDB_UNIT_CURRENT, &ipData.ipReasmReqds) == L7_SUCCESS)
      SET_VALID(I_ipReasmReqds, ipData.valid);
    break;

  case I_ipReasmOKs:
    if (usmDbIpReasmOKsGet(USMDB_UNIT_CURRENT, &ipData.ipReasmOKs) == L7_SUCCESS)
      SET_VALID(I_ipReasmOKs, ipData.valid);
    break;

  case I_ipReasmFails:
    if (usmDbIpReasmFailsGet(USMDB_UNIT_CURRENT, &ipData.ipReasmFails) == L7_SUCCESS)
      SET_VALID(I_ipReasmFails, ipData.valid);
    break;

  case I_ipFragOKs:
    if (usmDbIpFragOKsGet(USMDB_UNIT_CURRENT, &ipData.ipFragOKs) == L7_SUCCESS)
      SET_VALID(I_ipFragOKs, ipData.valid);
    break;

  case I_ipFragFails:
    if (usmDbIpFragFailsGet(USMDB_UNIT_CURRENT, &ipData.ipFragFails) == L7_SUCCESS)
      SET_VALID(I_ipFragFails, ipData.valid);
    break;

  case I_ipFragCreates:
    if (usmDbIpFragCreatesGet(USMDB_UNIT_CURRENT, &ipData.ipFragCreates) == L7_SUCCESS)
      SET_VALID(I_ipFragCreates, ipData.valid);
    break;

  case I_ipRoutingDiscards:
    if (usmDbIpRoutingDiscardsGet(USMDB_UNIT_CURRENT, &ipData.ipRoutingDiscards) == L7_SUCCESS)
      SET_VALID(I_ipRoutingDiscards, ipData.valid);
    break;

  default:
    /* Could not recognize the requested counter id */
    return(NULL);
    break;
  }

  if (nominator != 0 && !VALID(nominator, ipData.valid))
    return(NULL);

  return(&ipData);

}

#ifdef SETS
/* lvl7_@p1112 start */
#ifdef OLD_CODE
/* lvl7_@p1112 end */
int
k_ip_set(data, contextInfo, function)
ip_t *data;
ContextInfo *contextInfo;
int function;
{
  FNAME("k_ip_set")
  M2_IP ipVars;
  int varToSet = 0;

  if (VALID(I_ipForwarding, data->valid))
  {
    varToSet |= M2_IPFORWARDING;
    switch (data->ipForwarding)
    {
    case D_ipForwarding_forwarding:
      ipVars.ipForwarding = M2_ipForwarding_forwarding;
      break;
    case D_ipForwarding_not_forwarding:
      ipVars.ipForwarding = M2_ipForwarding_not_forwarding;
      break;
    }
  }
  if (VALID(I_ipDefaultTTL, data->valid))
  {
    varToSet |= M2_IPDEFAULTTTL;
    ipVars.ipDefaultTTL = (long) data->ipDefaultTTL;
  }
  if (varToSet==0)
  {
    return(GEN_ERROR);
  }

  DPRINTF((APTRACE, "%s: calling m2IpGroupInfoSet()\n", Fname));
  if (m2IpGroupInfoSet(varToSet, &ipVars)==OK)
  {
    return(NO_ERROR);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpGroupInfoSet returned ERROR\n", Fname));
    print_errno();
    return(GEN_ERROR);
  }
}
/* lvl7_@p1112 start */
#else /* OLD_CODE */
int
k_ip_set(ip_t *data, ContextInfo *contextInfo, int function)
{
  FNAME("k_ip_set")

  if (VALID(I_ipForwarding, data->valid))
  {
    switch (data->ipForwarding)
    {
    case D_ipForwarding_forwarding:
      if (usmDbIpRtrAdminModeSet(USMDB_UNIT_CURRENT, L7_ENABLE) != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;
      break;
    case D_ipForwarding_not_forwarding:
      if (usmDbIpRtrAdminModeSet(USMDB_UNIT_CURRENT, L7_DISABLE) != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;
      break;
    default:
      return COMMIT_FAILED_ERROR;
      break;
    }
  }

  return NO_ERROR;
}
#endif /* OLD_CODE */
/* lvl7_@p1112 end */
#endif /* SETS */


#ifdef OLD_CODE
ipAddrEntry_t *
k_ipAddrEntry_get(serialNum, contextInfo, nominator, searchType, ipAdEntAddr)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 ipAdEntAddr;
{
  FNAME("k_ipAddrEntry_get")
  static ipAddrEntry_t ipAddrEntryData;
  static M2_IPADDRTBL ipAddrEntry;

  ipAddrEntry.ipAdEntAddr = (unsigned long) osapiNtohl((unsigned long) ipAdEntAddr);
  DPRINTF((APTRACE, "%s: calling m2IpAddrTblEntryGet()\n", Fname));
  if (m2IpAddrTblEntryGet(M2_SEARCH(searchType), &ipAddrEntry)==OK)
  {
    ipAddrEntryData.ipAdEntAddr = (SR_UINT32) osapiHtonl((unsigned long) ipAddrEntry.ipAdEntAddr);
    ipAddrEntryData.ipAdEntIfIndex = (SR_INT32) ipAddrEntry.ipAdEntIfIndex;
    ipAddrEntryData.ipAdEntNetMask = (SR_UINT32) ipAddrEntry.ipAdEntNetMask;
    ipAddrEntryData.ipAdEntBcastAddr = (SR_INT32) ipAddrEntry.ipAdEntBcastAddr;
    ipAddrEntryData.ipAdEntReasmMaxSize = (SR_INT32) ipAddrEntry.ipAdEntReasmMaxSize;

    SET_ALL_VALID(ipAddrEntryData.valid);
    return(&ipAddrEntryData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpAddrTblEntryGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}
#else /* OLD_CODE */
ipAddrEntry_t *
k_ipAddrEntry_get(serialNum, contextInfo, nominator, searchType, ipAdEntAddr)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 ipAdEntAddr;
{
  FNAME("k_ipAddrEntry_get")
  static ipAddrEntry_t ipAddrEntryData;
  L7_uint32 intIfNum = 0;

  ZERO_VALID(ipAddrEntryData.valid);
  ipAddrEntryData.ipAdEntAddr = ipAdEntAddr;
  SET_VALID(I_ipAdEntAddr, ipAddrEntryData.valid);

  if ((searchType == EXACT) ? 
      usmDbIpAdEntAddrGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, intIfNum) != L7_SUCCESS :
    (usmDbIpAdEntAddrGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, intIfNum) != L7_SUCCESS &&
     usmDbIpAdEntAddrGetNext(USMDB_UNIT_CURRENT, &ipAddrEntryData.ipAdEntAddr, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(ipAddrEntryData.valid);
    return NULL;
  }

  switch (nominator)
  {
  case I_ipAdEntAddr:
    break;

  case I_ipAdEntIfIndex:
    if (usmDbIpEntIfIndexGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntIfIndex) == L7_SUCCESS)
      SET_VALID(I_ipAdEntIfIndex, ipAddrEntryData.valid);
    break;

  case I_ipAdEntNetMask:
    if (usmDbIpAdEntNetMaskGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntNetMask) == L7_SUCCESS)
      SET_VALID(I_ipAdEntNetMask, ipAddrEntryData.valid);
    break;

  case I_ipAdEntBcastAddr:
    if (usmDbIpAdEntBCastGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntBcastAddr) == L7_SUCCESS)
      SET_VALID(I_ipAdEntBcastAddr, ipAddrEntryData.valid);
    break;

  case I_ipAdEntReasmMaxSize:
    if (usmDbIpAdEntReasmMaxSizeGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntReasmMaxSize) == L7_SUCCESS)
      SET_VALID(I_ipAdEntReasmMaxSize, ipAddrEntryData.valid);
    break;

  default:
    /* unknown index */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, ipAddrEntryData.valid))
    return(NULL);

  return(&ipAddrEntryData);
}
#endif /* OLD_CODE */

ipRouteEntry_t *
k_ipRouteEntry_get(serialNum, contextInfo, nominator, searchType, ipRouteDest)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 ipRouteDest;
{
  FNAME("k_ipRouteEntry_get")
  static ipRouteEntry_t ipRouteEntryData;
  static ipstkIpRouteEntryInfo_t ipRouteTblEntry;
  static OID ipRouteInfo;

  ipRouteTblEntry.ipRouteDest = (unsigned long) osapiNtohl((unsigned long) ipRouteDest);
  DPRINTF((APTRACE, "%s: calling m2IpRouteTblEntryGet()\n", Fname));
  DPRINTF((APTRACE, "%s: ipRouteDest is %lu\n", Fname, ipRouteTblEntry.ipRouteDest));
  DPRINTF((APTRACE, "%s: search type is %s\n", Fname, M2_SEARCH_TEXT(searchType)));
  if (osapiIpRouteTblEntryGet((searchType == NEXT)?L7_MATCH_GETNEXT:L7_MATCH_EXACT, &ipRouteTblEntry)==L7_SUCCESS)
  {
    ipRouteEntryData.ipRouteDest = (SR_UINT32) osapiHtonl((unsigned long) ipRouteTblEntry.ipRouteDest);
    ipRouteEntryData.ipRouteIfIndex = (SR_INT32) ipRouteTblEntry.ipRouteIfIndex;
    ipRouteEntryData.ipRouteMetric1 = (SR_INT32) ipRouteTblEntry.ipRouteMetric1;
    ipRouteEntryData.ipRouteMetric2 = (SR_INT32) ipRouteTblEntry.ipRouteMetric2;
    ipRouteEntryData.ipRouteMetric3 = (SR_INT32) ipRouteTblEntry.ipRouteMetric3;
    ipRouteEntryData.ipRouteMetric4 = (SR_INT32) ipRouteTblEntry.ipRouteMetric4;
    ipRouteEntryData.ipRouteNextHop = (SR_UINT32) osapiHtonl(ipRouteTblEntry.ipRouteNextHop);
    ipRouteEntryData.ipRouteType = (SR_INT32) ipRouteTblEntry.ipRouteType;
    ipRouteEntryData.ipRouteProto = (SR_INT32) ipRouteTblEntry.ipRouteProto;
    ipRouteEntryData.ipRouteAge = (SR_INT32) ipRouteTblEntry.ipRouteAge;
    ipRouteEntryData.ipRouteMask = (SR_UINT32) ipRouteTblEntry.ipRouteMask;
    ipRouteEntryData.ipRouteMetric5 = (SR_INT32) ipRouteTblEntry.ipRouteMetric5;
    /* build a static OID on the fly */
    ipRouteInfo.oid_ptr = (SR_UINT32 *) ipRouteTblEntry.ipRouteInfoArray;
    ipRouteInfo.length = (SR_INT32) ipRouteTblEntry.ipRouteInfoLength;
    ipRouteEntryData.ipRouteInfo = &ipRouteInfo;

    SET_ALL_VALID(ipRouteEntryData.valid);
    return(&ipRouteEntryData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpRouteTblEntryGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

#ifdef SETS
int
k_ipRouteEntry_set(data, contextInfo, function)
ipRouteEntry_t *data;
ContextInfo *contextInfo;
int function;
{

#ifdef OLD_CODE
  FNAME("k_ipRouteEntry_set")
  static M2_IPROUTETBL ipRouteTblEntryToSet;
  int varToSet = 0;

  if (VALID(I_ipRouteDest, data->valid))
  {
    varToSet |= M2_IP_ROUTE_DEST;
    ipRouteTblEntryToSet.ipRouteDest = (unsigned long) osapiNtohl((unsigned long) data->ipRouteDest);
  }
  else
  {
    return(GEN_ERROR);
  }

  if (VALID(I_ipRouteNextHop, data->valid))
  {
    varToSet |= M2_IP_ROUTE_NEXT_HOP;
    ipRouteTblEntryToSet.ipRouteNextHop = (unsigned long) osapiNtohl((unsigned long) data->ipRouteNextHop);
  }
  else
  {
    return(GEN_ERROR);
  }

  if (VALID(I_ipRouteType, data->valid))
  {
    varToSet |= M2_IP_ROUTE_TYPE;
    switch (data->ipRouteType)
    {
    case D_ipRouteType_other:
      ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_other;
      break;
    case D_ipRouteType_invalid:
      ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_invalid;
      break;
    case D_ipRouteType_direct:
      ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_direct;
      break;
    case D_ipRouteType_indirect:
      ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_indirect;
      break;
    }
  }

  DPRINTF((APTRACE, "%s: calling m2IpRouteTblEntryGet()\n", Fname));
  if (m2IpRouteTblEntrySet(varToSet, &ipRouteTblEntryToSet)==OK)
  {
    return(NO_ERROR);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpRouteTblEntrySet returned ERROR\n", Fname));
    print_errno();
    return(GEN_ERROR);
  }
#else
    return(GEN_ERROR);
#endif
}
#endif /* SETS */

ipNetToMediaEntry_t *
k_ipNetToMediaEntry_get(serialNum, contextInfo, nominator, searchType, ipNetToMediaIfIndex, ipNetToMediaNetAddress)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_INT32 ipNetToMediaIfIndex;
SR_UINT32 ipNetToMediaNetAddress;
{
  FNAME("k_ipNetToMediaEntry_get")
  static ipNetToMediaEntry_t ipNetToMediaEntryData;
#ifdef OLD_CODE
  static M2_IPATRANSTBL ipAtEntry;
  static OctetString ipNetToMediaPhysAddress;

  ipAtEntry.ipNetToMediaIfIndex = (long) ipNetToMediaIfIndex;
  ipAtEntry.ipNetToMediaNetAddress = (unsigned long) osapiNtohl((unsigned long) ipNetToMediaNetAddress);
  DPRINTF((APTRACE, "%s: calling m2IpAtransTblEntryGet()\n", Fname));
  if (m2IpAtransTblEntryGet(M2_SEARCH(searchType), &ipAtEntry)==OK)
  {
    ipNetToMediaEntryData.ipNetToMediaIfIndex = (SR_INT32) ipAtEntry.ipNetToMediaIfIndex;

    /* build a static OctetString on the fly */
    ipNetToMediaPhysAddress.octet_ptr = (unsigned char *) ipAtEntry.ipNetToMediaPhysAddress.phyAddress;
    ipNetToMediaPhysAddress.length = (SR_INT32) ipAtEntry.ipNetToMediaPhysAddress.addrLength;
    ipNetToMediaEntryData.ipNetToMediaPhysAddress = &ipNetToMediaPhysAddress;

    ipNetToMediaEntryData.ipNetToMediaNetAddress = (SR_UINT32) osapiHtonl((unsigned long) ipAtEntry.ipNetToMediaNetAddress);

    switch (ipAtEntry.ipNetToMediaType)
    {
    case M2_ipNetToMediaType_other:
      ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_other;
      break;
    case M2_ipNetToMediaType_invalid:
      ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_invalid;
      break;
    case M2_ipNetToMediaType_dynamic:
      ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_dynamic;
      break;
    case M2_ipNetToMediaType_static:
      ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_static;
      break;
    }
    if (ipNetToMediaEntryData.ipNetToMediaIfIndex < ipNetToMediaIfIndex ||
        ipNetToMediaEntryData.ipNetToMediaIfIndex == ipNetToMediaIfIndex &&
        ipNetToMediaEntryData.ipNetToMediaNetAddress < ipNetToMediaNetAddress)
    {
      DPRINTF((APTRACE, "%s: m2IpAtransTblEntryGet returned a\nlexicographically lesser index for searchType %s\n", Fname, M2_SEARCH_TEXT(searchType)));
      return(NULL);
    }

    SET_ALL_VALID(ipNetToMediaEntryData.valid);
    return(&ipNetToMediaEntryData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpAtransTblEntryGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
#else /* OLD_CODE */
  OctetString *temp_os;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;
  L7_uint32 intIfNum;

  ZERO_VALID(ipNetToMediaEntryData.valid);
  ipNetToMediaEntryData.ipNetToMediaIfIndex = ipNetToMediaIfIndex;
  SET_VALID(I_ipNetToMediaIfIndex, ipNetToMediaEntryData.valid);
  ipNetToMediaEntryData.ipNetToMediaNetAddress = ipNetToMediaNetAddress;
  SET_VALID(I_ipNetToMediaNetAddress, ipNetToMediaEntryData.valid);


  if ((usmDbIntIfNumFromExtIfNum(ipNetToMediaEntryData.ipNetToMediaIfIndex, &intIfNum) != L7_SUCCESS) ||
     (usmDbIpNetToMediaEntryGet(USMDB_UNIT_CURRENT, intIfNum, ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS))
  {
    if ((searchType == EXACT) ||
        ((usmDbIntIfNumFromExtIfNum(ipNetToMediaEntryData.ipNetToMediaIfIndex, &intIfNum) != L7_SUCCESS) &&
         ((usmDbGetNextExtIfNumber(ipNetToMediaEntryData.ipNetToMediaIfIndex, &ipNetToMediaEntryData.ipNetToMediaIfIndex) != L7_SUCCESS) ||
         (usmDbIntIfNumFromExtIfNum(ipNetToMediaEntryData.ipNetToMediaIfIndex, &intIfNum) != L7_SUCCESS))) ||
        (usmDbIpNetToMediaEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum, &ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS) ||
        (usmDbExtIfNumFromIntIfNum(intIfNum, &ipNetToMediaEntryData.ipNetToMediaIfIndex) != L7_SUCCESS))
    {
      ZERO_VALID(ipNetToMediaEntryData.valid);
      return NULL;
    }
  }

#ifdef OLD_CODE
  if ((searchType == EXACT) ? 
      (usmDbIpNetToMediaEntryGet(USMDB_UNIT_CURRENT, intIfNum, ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS) :
      ((usmDbIpNetToMediaEntryGet(USMDB_UNIT_CURRENT, intIfNum, ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS) &&
       (usmDbIpNetToMediaEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum, &ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS)
  {
    ZERO_VALID(ipNetToMediaEntryData.valid);
    return(NULL);
  }
#endif /* OLD_CODE */

  switch (nominator)
  {
  case -1:
  case I_ipNetToMediaIfIndex:
  case I_ipNetToMediaNetAddress:
    break;

  case I_ipNetToMediaPhysAddress:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbIpNetToMediaPhysAddressGet(USMDB_UNIT_CURRENT, ipNetToMediaEntryData.ipNetToMediaIfIndex, ipNetToMediaEntryData.ipNetToMediaNetAddress,
                                        snmp_buffer, &snmp_buffer_len) == L7_SUCCESS)
    {
      if ((temp_os = MakeOctetString(snmp_buffer, snmp_buffer_len)) != NULL)
      {
        FreeOctetString(ipNetToMediaEntryData.ipNetToMediaPhysAddress);
        ipNetToMediaEntryData.ipNetToMediaPhysAddress = temp_os;
        SET_VALID(I_ipNetToMediaPhysAddress, ipNetToMediaEntryData.valid);
      }
    }
    break;

  case I_ipNetToMediaType:
    if (usmDbIpNetToMediaTypeGet(USMDB_UNIT_CURRENT, ipNetToMediaEntryData.ipNetToMediaIfIndex, ipNetToMediaEntryData.ipNetToMediaNetAddress,
                                 &ipNetToMediaEntryData.ipNetToMediaType) == L7_SUCCESS)
    {
      SET_VALID(I_ipNetToMediaType, ipNetToMediaEntryData.valid);
      switch (ipNetToMediaEntryData.ipNetToMediaType)
      {
      case L7_IP_AT_TYPE_OTHER:
        ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_other;
        break;
      case L7_IP_AT_TYPE_INVALID:
        ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_invalid;
        break;
      case L7_IP_AT_TYPE_DYNAMIC:
        ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_dynamic;
        break;
      case L7_IP_AT_TYPE_STATIC:
        ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_static;
        break;
      default:
        ipNetToMediaEntryData.ipNetToMediaType = 0;
        CLR_VALID(I_ipNetToMediaType, ipNetToMediaEntryData.valid);
        break;
      }
      break;
    }
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, ipNetToMediaEntryData.valid))
    return(NULL);

  return(&ipNetToMediaEntryData);
#endif /* OLD_CODE */
}

#ifdef SETS
int
k_ipNetToMediaEntry_set(data, contextInfo, function)
ipNetToMediaEntry_t *data;
ContextInfo *contextInfo;
int function;
{
  FNAME("k_ipNetToMediaEntry_set")
#ifdef OLD_CODE
  static M2_IPATRANSTBL ipAtEntryToSet;

  if (VALID(I_ipNetToMediaType, data->valid))
  {
    switch (data->ipNetToMediaType)
    {
    case D_ipNetToMediaType_other:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_other;
      break;
    case D_ipNetToMediaType_invalid:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_invalid;
      break;
    case D_ipNetToMediaType_dynamic:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_dynamic;
      break;
    case D_ipNetToMediaType_static:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_static;
      break;
    }
  }
  else
  {
    return(GEN_ERROR);
  }

  if (ipAtEntryToSet.ipNetToMediaType != M2_ipNetToMediaType_invalid)
  {
    if (VALID(I_ipNetToMediaPhysAddress, data->valid))
    {
      ipAtEntryToSet.ipNetToMediaPhysAddress.addrLength = MIN(data->ipNetToMediaPhysAddress->length, MAXIFPHYADDR);
      memcpy(ipAtEntryToSet.ipNetToMediaPhysAddress.phyAddress, data->ipNetToMediaPhysAddress->octet_ptr, ipAtEntryToSet.ipNetToMediaPhysAddress.addrLength);
    }
    else
    {
      return(GEN_ERROR);
    }
  }

  ipAtEntryToSet.ipNetToMediaIfIndex = (long) data->ipNetToMediaIfIndex;
  ipAtEntryToSet.ipNetToMediaNetAddress = (unsigned long) osapiNtohl((unsigned long) data->ipNetToMediaNetAddress);

  DPRINTF((APTRACE, "%s: calling m2IpAtransTblEntrySet()\n", Fname));
  if (m2IpAtransTblEntrySet(&ipAtEntryToSet)==OK)
  {
    return(NO_ERROR);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpAtransTblEntrySet returned ERROR\n", Fname));
    print_errno();
    return(GEN_ERROR);
  }
#else /* OLD_CODE */

  /* TODO: test this code on network port entries */

  if (usmDbIpNetToMediaEntryGet(USMDB_UNIT_CURRENT, data->ipNetToMediaIfIndex, 
                                data->ipNetToMediaNetAddress) != L7_SUCCESS)
  {
    return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_ipNetToMediaType, data->valid))
  {
    switch (data->ipNetToMediaType)
    {
    case D_ipNetToMediaType_other:
      data->ipNetToMediaType = L7_IP_AT_TYPE_OTHER;
      break;

    case D_ipNetToMediaType_invalid:
      data->ipNetToMediaType = L7_IP_AT_TYPE_INVALID;
      break;

    case D_ipNetToMediaType_dynamic:
      data->ipNetToMediaType = L7_IP_AT_TYPE_DYNAMIC;
      break;

    case D_ipNetToMediaType_static:
      data->ipNetToMediaType = L7_IP_AT_TYPE_STATIC;
      break;

    default:
      return COMMIT_FAILED_ERROR;
      break;
    }

    if (usmDbIpNetToMediaTypeSet(USMDB_UNIT_CURRENT, data->ipNetToMediaIfIndex,
                                 data->ipNetToMediaNetAddress,
                                 data->ipNetToMediaType) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  /* check that the type wasn't set to invalid */
  if (VALID(I_ipNetToMediaPhysAddress, data->valid) &&
      (!VALID(I_ipNetToMediaType, data->valid) || data->ipNetToMediaType != L7_IP_AT_TYPE_INVALID) &&
      usmDbIpNetToMediaPhysAddressSet(USMDB_UNIT_CURRENT, data->ipNetToMediaIfIndex,
                                      data->ipNetToMediaNetAddress,
                                      data->ipNetToMediaPhysAddress->octet_ptr,
                                      data->ipNetToMediaPhysAddress->length) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
#endif /* OLD_CODE */
}
#endif /* SETS */

icmp_t *
k_icmp_get(serialNum, contextInfo, nominator)
int serialNum;
ContextInfo *contextInfo;
int nominator;
{
  FNAME("k_icmp_get")
  static icmp_t icmpData;
  static ipstkIcmpInfo_t icmpVars;

  DPRINTF((APTRACE, "%s: calling m2IcmpGroupInfoGet()\n", Fname));
  if (osapiIcmpGroupInfoGet(&icmpVars)==L7_SUCCESS)
  {
    icmpData.icmpInMsgs = (SR_UINT32) icmpVars.icmpInMsgs;
    icmpData.icmpInErrors = (SR_UINT32) icmpVars.icmpInErrors;
    icmpData.icmpInDestUnreachs = (SR_UINT32) icmpVars.icmpInDestUnreachs;
    icmpData.icmpInTimeExcds = (SR_UINT32) icmpVars.icmpInTimeExcds;
    icmpData.icmpInParmProbs = (SR_UINT32) icmpVars.icmpInParmProbs;
    icmpData.icmpInSrcQuenchs = (SR_UINT32) icmpVars.icmpInSrcQuenchs;
    icmpData.icmpInRedirects = (SR_UINT32) icmpVars.icmpInRedirects;
    icmpData.icmpInEchos = (SR_UINT32) icmpVars.icmpInEchos;
    icmpData.icmpInEchoReps = (SR_UINT32) icmpVars.icmpInEchoReps;
    icmpData.icmpInTimestamps = (SR_UINT32) icmpVars.icmpInTimestamps;
    icmpData.icmpInTimestampReps = (SR_UINT32) icmpVars.icmpInTimestampReps;
    icmpData.icmpInAddrMasks = (SR_UINT32) icmpVars.icmpInAddrMasks;
    icmpData.icmpInAddrMaskReps = (SR_UINT32) icmpVars.icmpInAddrMaskReps;
    icmpData.icmpOutMsgs = (SR_UINT32) icmpVars.icmpOutMsgs;
    icmpData.icmpOutErrors = (SR_UINT32) icmpVars.icmpOutErrors;
    icmpData.icmpOutDestUnreachs = (SR_UINT32) icmpVars.icmpOutDestUnreachs;
    icmpData.icmpOutTimeExcds = (SR_UINT32) icmpVars.icmpOutTimeExcds;
    icmpData.icmpOutParmProbs = (SR_UINT32) icmpVars.icmpOutParmProbs;
    icmpData.icmpOutSrcQuenchs = (SR_UINT32) icmpVars.icmpOutSrcQuenchs;
    icmpData.icmpOutRedirects = (SR_UINT32) icmpVars.icmpOutRedirects;
    icmpData.icmpOutEchos = (SR_UINT32) icmpVars.icmpOutEchos;
    icmpData.icmpOutEchoReps = (SR_UINT32) icmpVars.icmpOutEchoReps;
    icmpData.icmpOutTimestamps = (SR_UINT32) icmpVars.icmpOutTimestamps;
    icmpData.icmpOutTimestampReps = (SR_UINT32) icmpVars.icmpOutTimestampReps;
    icmpData.icmpOutAddrMasks = (SR_UINT32) icmpVars.icmpOutAddrMasks;
    icmpData.icmpOutAddrMaskReps = (SR_UINT32) icmpVars.icmpOutAddrMaskReps;
    SET_ALL_VALID(icmpData.valid);
    return(&icmpData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IcmpGroupInfoGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

tcp_t *
k_tcp_get(serialNum, contextInfo, nominator)
int serialNum;
ContextInfo *contextInfo;
int nominator;
{
  FNAME("k_tcp_get")
  static tcp_t tcpData;
  static ipstkTcpInfo_t tcpVars;

  DPRINTF((APTRACE, "%s: calling m2TcpGroupInfoGet()\n", Fname));
  if (osapiTcpGroupInfoGet(&tcpVars)==L7_SUCCESS)
  {
    tcpData.tcpRtoAlgorithm = (SR_INT32) tcpVars.tcpRtoAlgorithm;
    tcpData.tcpRtoMin = (SR_INT32) tcpVars.tcpRtoMin;
    tcpData.tcpRtoMax = (SR_INT32) tcpVars.tcpRtoMax;
    tcpData.tcpMaxConn = (SR_INT32) tcpVars.tcpMaxConn;
    tcpData.tcpActiveOpens = (SR_UINT32) tcpVars.tcpActiveOpens;
    tcpData.tcpPassiveOpens = (SR_UINT32) tcpVars.tcpPassiveOpens;
    tcpData.tcpAttemptFails = (SR_UINT32) tcpVars.tcpAttemptFails;
    tcpData.tcpEstabResets = (SR_UINT32) tcpVars.tcpEstabResets;
    tcpData.tcpCurrEstab = (SR_UINT32) tcpVars.tcpCurrEstab;
    tcpData.tcpInSegs = (SR_UINT32) tcpVars.tcpInSegs;
    tcpData.tcpOutSegs = (SR_UINT32) tcpVars.tcpOutSegs;
    tcpData.tcpRetransSegs = (SR_UINT32) tcpVars.tcpRetransSegs;
    tcpData.tcpInErrs = (SR_UINT32) tcpVars.tcpInErrs;
    tcpData.tcpOutRsts = (SR_UINT32) tcpVars.tcpOutRsts;
    SET_ALL_VALID(tcpData.valid);
    return(&tcpData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2TcpGroupInfoGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

tcpConnEntry_t *
k_tcpConnEntry_get(serialNum, contextInfo, nominator, searchType, tcpConnLocalAddress, tcpConnLocalPort, tcpConnRemAddress, tcpConnRemPort)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 tcpConnLocalAddress;
SR_INT32 tcpConnLocalPort;
SR_UINT32 tcpConnRemAddress;
SR_INT32 tcpConnRemPort;
{
  FNAME("k_tcpConnEntry_get")
  static tcpConnEntry_t tcpConnEntryData;
  static ipstkTcpConnInfo_t tcpEntry;

  tcpEntry.tcpConnLocalAddress = (unsigned long) osapiNtohl((unsigned long) tcpConnLocalAddress);
  tcpEntry.tcpConnLocalPort = (long) tcpConnLocalPort;
  tcpEntry.tcpConnRemAddress = (unsigned long) osapiNtohl((unsigned long) tcpConnRemAddress);
  tcpEntry.tcpConnRemPort = (long) tcpConnRemPort;

  DPRINTF((APTRACE, "%s: calling m2TcpConnEntryGet()\n", Fname));
  if (osapiTcpConnInfoGet((searchType == NEXT)?L7_MATCH_GETNEXT:L7_MATCH_EXACT, &tcpEntry)==L7_SUCCESS)
  {
    tcpConnEntryData.tcpConnState = (SR_INT32) tcpEntry.tcpConnState;
    tcpConnEntryData.tcpConnLocalAddress = (SR_UINT32) osapiHtonl((unsigned long) tcpEntry.tcpConnLocalAddress);
    tcpConnEntryData.tcpConnLocalPort = (SR_INT32) tcpEntry.tcpConnLocalPort;
    tcpConnEntryData.tcpConnRemAddress = (SR_UINT32) osapiHtonl((unsigned long) tcpEntry.tcpConnRemAddress);
    tcpConnEntryData.tcpConnRemPort = (SR_INT32) tcpEntry.tcpConnRemPort;
    SET_ALL_VALID(tcpConnEntryData.valid);
    return(&tcpConnEntryData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2TcpConnEntryGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

#ifdef SETS

int
k_tcpConnEntry_set(data, contextInfo, function)
tcpConnEntry_t *data;
ContextInfo *contextInfo;
int function;
{
  FNAME("k_tcpConnEntry_set")
  static ipstkTcpConnInfo_t tcpEntryToSet;

  tcpEntryToSet.tcpConnLocalAddress = (unsigned long) osapiNtohl((unsigned long) data->tcpConnLocalAddress);
  tcpEntryToSet.tcpConnLocalPort = (long) data->tcpConnLocalPort;
  tcpEntryToSet.tcpConnRemAddress = (unsigned long) osapiNtohl((unsigned long) data->tcpConnRemAddress);
  tcpEntryToSet.tcpConnRemPort = (long) data->tcpConnRemPort;

  if (VALID(I_tcpConnState, data->valid) && data->tcpConnState == D_tcpConnState_deleteTCB)
  {
    tcpEntryToSet.tcpConnState = (long) M2_tcpConnState_deleteTCB;
    DPRINTF((APTRACE, "%s: calling m2TcpConnEntrySet()\n", Fname));
    if (osapiTcpConnInfoSet(&tcpEntryToSet)==L7_SUCCESS)
    {
      return(NO_ERROR);
    }
    else
    {
      DPRINTF((APTRACE, "%s: m2TcpConnEntrySet returned ERROR\n", Fname));
      print_errno();
      return(GEN_ERROR);
    }
  }
  return(NO_ERROR);
}
#endif /* SETS */

udp_t *
k_udp_get(serialNum, contextInfo, nominator)
int serialNum;
ContextInfo *contextInfo;
int nominator;
{
  FNAME("k_udp_get")
  static udp_t udpData;
  static ipstkUdpInfo_t  udpVars;

  DPRINTF((APTRACE, "%s: calling m2UdpGroupInfoGet()\n", Fname));
  if (osapiUdpGroupInfoGet(&udpVars)==L7_SUCCESS)
  {
    udpData.udpInDatagrams = (SR_UINT32) udpVars.udpInDatagrams;
    udpData.udpNoPorts = (SR_UINT32) udpVars.udpNoPorts;
    udpData.udpInErrors = (SR_UINT32) udpVars.udpInErrors;
    udpData.udpOutDatagrams = (SR_UINT32) udpVars.udpOutDatagrams;
    SET_ALL_VALID(udpData.valid);
    return(&udpData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2UdpGroupInfoGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

udpEntry_t *
k_udpEntry_get(serialNum, contextInfo, nominator, searchType, udpLocalAddress, udpLocalPort)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 udpLocalAddress;
SR_INT32 udpLocalPort;
{
  FNAME("k_udpEntry_get")
  static udpEntry_t udpEntryData;
  static ipstkUdpListInfo_t udpEntry;

  udpEntry.udpLocalAddress = (unsigned long) osapiNtohl((unsigned long) udpLocalAddress);
  udpEntry.udpLocalPort = (long) udpLocalPort;
  DPRINTF((APTRACE, "%s: calling m2UdpTblEntryGet()\n", Fname));
  if (osapiUdpListInfoGet((searchType == NEXT)?L7_MATCH_GETNEXT:L7_MATCH_EXACT, &udpEntry)==L7_SUCCESS)
  {
    udpEntryData.udpLocalAddress = (SR_UINT32) osapiHtonl((unsigned long) udpEntry.udpLocalAddress);
    udpEntryData.udpLocalPort = (SR_INT32) udpEntry.udpLocalPort;
    SET_ALL_VALID(udpEntryData.valid);
    return(&udpEntryData);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2UdpTblEntryGet returned ERROR\n", Fname));
    print_errno();
    return(NULL);
  }
}

