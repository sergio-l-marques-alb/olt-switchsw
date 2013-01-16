/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/snmp/packages/ip_mcast/k_mib_pimsm.c
 *
 * @purpose  Provide SNMP interface( for draft mib) PIMSM vendor code.
 *
 * @component SNMP (PIMSM)
 *
 * @comments 
 *
 * @create 04/05/2006
 *
 * @author Dasoju Shashidhar
 * @end
 *
 **********************************************************************/
#include "k_private_base.h"
#include "k_mib_pim_rfc5060_api.h"
#include <inst_lib.h>
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
#endif


/*L7_uint32 snmp_pim_mode = 1;   L7_SNMP_PIM_MODE_DENSE*/
#define IPV4_ADDR_LENGTH 4
#define IPV6_ADDR_LENGTH 16

/**********************************************************************
 * PIM-SM INTERFACE TABLE IMPLEMENTAION
 *********************************************************************/

L7_uint32  pimGrpMappingTableIndexCompare(L7_int32 OriginV4, L7_int32 OriginV6)
{
  if (OriginV4 < OriginV6)
    return L7_AF_INET;
  else if (OriginV4 > OriginV6)
    return L7_AF_INET6;
  else
    return L7_AF_INET;
}

L7_uint32
pimIntfTableIndexCompare(L7_uint32 pimIntfV4Index, L7_uint32 pimIntfV6Index)
{

  if(pimIntfV4Index < pimIntfV6Index)
    return L7_AF_INET;
  else if(pimIntfV6Index < pimIntfV4Index)
    return L7_AF_INET6;
  else 
    return L7_AF_INET;
}

pimInterfaceEntry_t *
k_pimInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_INT32 pimInterfaceIfIndex,
    SR_INT32 pimInterfaceIPVersion)
{
  static pimInterfaceEntry_t pimInterfaceEntryData;
  L7_inet_addr_t pimIntfAddr;
  L7_uint32  intIfNum = 0;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    pimInterfaceEntryData.pimInterfaceAddress = MakeOctetString(NULL, 0);
    pimInterfaceEntryData.pimInterfaceDR = MakeOctetString(NULL, 0);
    if(searchType == NEXT)
    {
      pimInterfaceIfIndex = 0;
      pimInterfaceIPVersion = L7_AF_INET;
    }
  }

  /* Clear all the bits*/
  ZERO_VALID(pimInterfaceEntryData.valid);
  pimInterfaceEntryData.pimInterfaceIfIndex = pimInterfaceIfIndex;
  pimInterfaceEntryData.pimInterfaceIPVersion = pimInterfaceIPVersion;
  SET_VALID(I_pimInterfaceIfIndex, pimInterfaceEntryData.valid);
  SET_VALID(I_pimInterfaceIPVersion, pimInterfaceEntryData.valid);

  if(searchType == EXACT)
  {
    if (snmpPimInterfaceEntryGet(USMDB_UNIT_CURRENT, pimInterfaceIfIndex, pimInterfaceIPVersion) != L7_SUCCESS) 
    {
      ZERO_VALID(pimInterfaceEntryData.valid);
      firstTime = L7_TRUE;
      return(NULL);
    }
  }
  else if(searchType == NEXT)
  {
    if((snmpPimInterfaceEntryGet(USMDB_UNIT_CURRENT, pimInterfaceIfIndex, pimInterfaceIPVersion) != L7_SUCCESS) && 
        (snmpPimInterfaceEntryNextGet(USMDB_UNIT_CURRENT, pimInterfaceIfIndex, pimInterfaceIPVersion, 
        &pimInterfaceEntryData.pimInterfaceIfIndex, &pimInterfaceEntryData.pimInterfaceIPVersion) != L7_SUCCESS))
    {
      ZERO_VALID(pimInterfaceEntryData.valid);
      firstTime = L7_TRUE;
      return(NULL);
    }
  }

  if (usmDbIntIfNumFromExtIfNum(pimInterfaceEntryData.pimInterfaceIfIndex, &intIfNum) != L7_SUCCESS)
  {
     ZERO_VALID(pimInterfaceEntryData.valid);
     return(NULL);
  }

  pimInterfaceIfIndex = pimInterfaceEntryData.pimInterfaceIfIndex;
  pimInterfaceIPVersion = pimInterfaceEntryData.pimInterfaceIPVersion;

  SET_VALID(I_pimInterfaceIfIndex, pimInterfaceEntryData.valid);
  SET_VALID(I_pimInterfaceIPVersion, pimInterfaceEntryData.valid);
  switch (nominator)
  {
    case -1:
    case I_pimInterfaceIfIndex: /* already got it above*/
        break;
    case I_pimInterfaceIPVersion:
        break;
    case I_pimInterfaceAddressType:
         pimInterfaceEntryData.pimInterfaceAddressType = pimInterfaceEntryData.pimInterfaceIPVersion;
         SET_VALID(nominator, pimInterfaceEntryData.valid);
        break;
    case I_pimInterfaceAddress:
         if(usmDbPimInterfaceIPAddressGet(USMDB_UNIT_CURRENT, pimInterfaceIPVersion,
                                           intIfNum,&pimIntfAddr) == L7_SUCCESS)
         {
           if(pimInterfaceIPVersion == D_pimInterfaceIPVersion_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimInterfaceEntryData.pimInterfaceAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimInterfaceEntryData.valid);
               }
             }
           }
           else if(pimInterfaceIPVersion == D_pimInterfaceIPVersion_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimInterfaceEntryData.pimInterfaceAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimInterfaceEntryData.valid);
               }
             }
           }
         }
         break;

    case I_pimInterfaceDR:
         /* Supported only for PIM-SM. */
         if(usmDbPimsmInterfaceDRGet(USMDB_UNIT_CURRENT,  pimInterfaceEntryData.pimInterfaceIPVersion,
                                        intIfNum, &pimIntfAddr) == L7_SUCCESS)
         {
           if(pimInterfaceEntryData.pimInterfaceIPVersion == D_pimInterfaceIPVersion_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimInterfaceEntryData.pimInterfaceDR,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimInterfaceEntryData.valid);
               }
             }
           }
           else if(pimInterfaceEntryData.pimInterfaceIPVersion == D_pimInterfaceIPVersion_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimInterfaceEntryData.pimInterfaceDR,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimInterfaceEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimInterfaceHelloInterval:
         if(usmDbPimIntfHelloIntervalGet(USMDB_UNIT_CURRENT, pimInterfaceIPVersion ,
             intIfNum,&pimInterfaceEntryData.pimInterfaceHelloInterval) == L7_SUCCESS )
           SET_VALID(nominator, pimInterfaceEntryData.valid);
        break;
    case I_pimInterfaceTrigHelloInterval:
        break;
    case I_pimInterfaceJoinPruneInterval:
         if (usmDbPimInterfaceJoinPruneIntervalGet(USMDB_UNIT_CURRENT, pimInterfaceIPVersion, intIfNum,
                    &pimInterfaceEntryData.pimInterfaceJoinPruneInterval) == L7_SUCCESS )
           SET_VALID(nominator, pimInterfaceEntryData.valid);
        break;
    case I_pimInterfaceDFElectionRobustness:
        break;
    case I_pimInterfaceHelloHoldtime:
        break;
    case I_pimInterfaceJoinPruneHoldtime:
        break;
    case I_pimInterfacePropagationDelay:
        break;
    case I_pimInterfaceOverrideInterval:
        break;
    case I_pimInterfaceGenerationIDValue:
         if(usmDbPimInterfaceGenerationIDValueGet(intIfNum, pimInterfaceIPVersion,
               &pimInterfaceEntryData.pimInterfaceGenerationIDValue) == L7_SUCCESS )
           SET_VALID(nominator, pimInterfaceEntryData.valid);
        break;
    case I_pimInterfaceDRPriority:
         /* Supported only for PIM-SM. */ 
         if(usmDbPimsmInterfaceDRPriorityGet(USMDB_UNIT_CURRENT, pimInterfaceEntryData.pimInterfaceIPVersion,
              intIfNum,&pimInterfaceEntryData.pimInterfaceDRPriority) == L7_SUCCESS)
           SET_VALID(nominator, pimInterfaceEntryData.valid);
       break;
    case I_pimInterfaceLanDelayEnabled:
        break;
    case I_pimInterfaceEffectPropagDelay:
        break;
    case I_pimInterfaceEffectOverrideIvl:
        break;
    case I_pimInterfaceSuppressionEnabled:
        break;
    case I_pimInterfaceBidirCapable:
        break;
    case I_pimInterfaceDRPriorityEnabled:
        /* usmDbPimInterfaceUseDRPriorityGet used but it always returns a SUCCESS...the comment in usmdb arises confusion */
        break;
    case I_pimInterfaceDomainBorder:
         if(usmDbPimsmInterfaceBSRBorderGet(intIfNum, pimInterfaceEntryData.pimInterfaceIPVersion, 
                                            &pimInterfaceEntryData.pimInterfaceDomainBorder) == L7_SUCCESS)
         {
           if ( pimInterfaceEntryData.pimInterfaceDomainBorder == 0)
           pimInterfaceEntryData.pimInterfaceDomainBorder= D_pimInterfaceDomainBorder_false;
           SET_VALID(nominator, pimInterfaceEntryData.valid);
         }
         break;
    case I_pimInterfaceStatus:
         if (snmpPimInterfaceStatusGet(USMDB_UNIT_CURRENT, pimInterfaceIPVersion, intIfNum, 
               &pimInterfaceEntryData.pimInterfaceStatus) == L7_SUCCESS )
           SET_VALID(nominator, pimInterfaceEntryData.valid);
      	 break;

    default:
	/* unknown nominator */
	    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, pimInterfaceEntryData.valid))
  {
    return(NULL);
  }

  return(&pimInterfaceEntryData);
}

int
k_pimInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{
  /* Following line implemented per VLAN mib implementaion...*/
  ZERO_VALID(((pimInterfaceEntry_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_pimInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pimInterfaceEntry_set_defaults(doList_t *dp)
{
  pimInterfaceEntry_t *data = (pimInterfaceEntry_t *) (dp->data);

  if ((data->pimInterfaceAddress = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->pimInterfaceDR = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pimInterfaceHelloInterval = 30;
  data->pimInterfaceTrigHelloInterval = 5;
  data->pimInterfaceJoinPruneInterval = 60;
  data->pimInterfaceDFElectionRobustness = 3;
  data->pimInterfaceHelloHoldtime = 105;
  data->pimInterfaceJoinPruneHoldtime = 210;
  data->pimInterfacePropagationDelay = 500;
  data->pimInterfaceOverrideInterval = 2500;
  data->pimInterfaceDRPriority = 1;
  data->pimInterfaceDomainBorder = D_pimInterfaceDomainBorder_false;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_pimInterfaceEntry_set(pimInterfaceEntry_t *data,
    ContextInfo *contextInfo, int function)
{
  L7_RC_t rc;
  L7_uint32 pimInterfaceIfIndex;

  /* Getting internal interface number */
  rc = usmDbIntIfNumFromExtIfNum(data->pimInterfaceIfIndex, &pimInterfaceIfIndex);
  data->pimInterfaceIfIndex = pimInterfaceIfIndex;


  if (VALID(I_pimInterfaceStatus, data->valid))
  {
    if ( snmpPimInterfaceStatusSet(pimInterfaceIfIndex,
                                         data->pimInterfaceIPVersion,
                                         data->pimInterfaceStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_pimInterfaceStatus, data->valid);
      return (COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimInterfaceHelloInterval, data->valid))
  {
    if (!((data->pimInterfaceHelloInterval >= L7_PIM_INTERFACE_HELLO_INTERVAL_MIN) &&
	  (data->pimInterfaceHelloInterval <= L7_PIM_INTERFACE_HELLO_INTERVAL_MAX)) ||
	   usmDbPimIntfHelloIntervalSet(USMDB_UNIT_CURRENT, data->pimInterfaceIPVersion,
                                                     data->pimInterfaceIfIndex,
                                                     data->pimInterfaceHelloInterval) != L7_SUCCESS)
    {
      	    return(COMMIT_FAILED_ERROR);
    }
  }


  if (VALID(I_pimInterfaceJoinPruneInterval, data->valid))
  {
    if (usmDbPimsmInterfaceJoinPruneIntervalSet(USMDB_UNIT_CURRENT, data->pimInterfaceIPVersion,
                                        pimInterfaceIfIndex, data->pimInterfaceJoinPruneInterval) != L7_SUCCESS)
	    return(COMMIT_FAILED_ERROR);
  }
  if (VALID(I_pimInterfaceDRPriority, data->valid))
  {
    if (usmDbPimsmInterfaceDRPrioritySet(USMDB_UNIT_CURRENT, data->pimInterfaceIPVersion, 
                                 pimInterfaceIfIndex, data->pimInterfaceDRPriority) != L7_SUCCESS)
	     return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_pimInterfaceDomainBorder, data->valid))
  {
    if ( data->pimInterfaceDomainBorder == D_pimInterfaceDomainBorder_false)
    {
      if (usmDbPimsmInterfaceBSRBorderSet(pimInterfaceIfIndex, data->pimInterfaceIPVersion, L7_DISABLE) != L7_SUCCESS)
        return(COMMIT_FAILED_ERROR);    
    }
    else
    {
      if (usmDbPimsmInterfaceBSRBorderSet(pimInterfaceIfIndex, data->pimInterfaceIPVersion, L7_ENABLE) != L7_SUCCESS)  
        return(COMMIT_FAILED_ERROR);
    } 
  }
 
  return NO_ERROR;
}

/**********************************************************************
 * PIM-SM NEIGHBOR TABLE IMPLEMENTAION
 *********************************************************************/
pimNeighborEntry_t *
k_pimNeighborEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pimNeighborIfIndex,
                       SR_INT32 pimNeighborAddressType,
                       OctetString * pimNeighborAddress)
{
  static pimNeighborEntry_t pimNeighborEntryData;
  L7_inet_addr_t pimSmNbrAddr;
  static L7_BOOL initializeMem = L7_TRUE;
  L7_uint32 intIfNum; 
 
  if(initializeMem == L7_TRUE)
  {
    pimNeighborEntryData.pimNeighborAddress = MakeOctetString(NULL, 0);
    initializeMem = L7_FALSE;
  }

  if((pimNeighborAddressType == 0) && (searchType != EXACT)) 
  {
    (void)SafeMakeOctetString(&pimNeighborEntryData.pimNeighborAddress, NULL, 0);
    pimNeighborIfIndex = 0;
    pimNeighborAddressType = 1;
  } 

  /* Clear all the bits*/
  ZERO_VALID(pimNeighborEntryData.valid);
  pimNeighborEntryData.pimNeighborIfIndex = pimNeighborIfIndex;
  pimNeighborEntryData.pimNeighborAddressType = pimNeighborAddressType;
  SET_VALID(I_pimNeighborAddressType, pimNeighborEntryData.valid);
  SET_VALID(I_pimNeighborIfIndex, pimNeighborEntryData.valid);

  memset(&pimSmNbrAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if(pimNeighborEntryData.pimNeighborAddressType == D_pimNeighborAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    if (pimNeighborIfIndex != 0 )
    {
      ipAddr = OctetStringToIP(pimNeighborAddress);
    }
    if(inetAddressSet(L7_AF_INET, &ipAddr, &pimSmNbrAddr) != L7_SUCCESS)
    {
      return(NULL);
    }
  }
  else if(pimNeighborEntryData.pimNeighborAddressType == D_pimNeighborAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;
    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));

    if(pimNeighborIfIndex != 0)
    {
      OctetStringToIP6(pimNeighborAddress, (L7_char8*)&ipAddr);
    }
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmNbrAddr))
    {
      return(NULL);
    }
 
  }

  if(searchType == EXACT) 
  {
    if(snmpPimNeighborEntryGet(USMDB_UNIT_CURRENT, pimNeighborAddressType, pimNeighborEntryData.pimNeighborIfIndex, 
                               &pimSmNbrAddr) != L7_SUCCESS)
    {
      return(NULL);
    }
  }
  else
  {
     
    if((snmpPimNeighborEntryGet(USMDB_UNIT_CURRENT, pimNeighborAddressType, pimNeighborEntryData.pimNeighborIfIndex,
                               &pimSmNbrAddr) != L7_SUCCESS) &&
       (snmpPimNeighborEntryNextGet(USMDB_UNIT_CURRENT, pimNeighborEntryData.pimNeighborIfIndex, &pimSmNbrAddr, 
                                   &pimNeighborEntryData.pimNeighborIfIndex, 
                                   &pimNeighborEntryData.pimNeighborAddressType) != L7_SUCCESS))
    {
      return(NULL);
    }
  }  

  SET_VALID(I_pimNeighborAddressType, pimNeighborEntryData.valid);
  SET_VALID(I_pimNeighborIfIndex, pimNeighborEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(pimNeighborEntryData.pimNeighborIfIndex, &intIfNum) != L7_SUCCESS)
  {
     ZERO_VALID(pimNeighborEntryData.valid);
     return(NULL);
  }

  


  /* copy the new pim Neighbor Address to the outgoing data structure
    by converting the IP Address to Octet String */
  if(pimNeighborEntryData.pimNeighborAddressType == D_pimNeighborAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmNbrAddr, &ipAddr))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimNeighborEntryData.pimNeighborAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimNeighborAddress, pimNeighborEntryData.valid);
    }
  }
  else if(pimNeighborEntryData.pimNeighborAddressType == D_pimNeighborAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmNbrAddr, &ipAddr))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimNeighborEntryData.pimNeighborAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimNeighborAddress, pimNeighborEntryData.valid);
    }
  }

  switch (nominator)
  {
      case -1:
      case I_pimNeighborIfIndex: /* already got it above*/
      case I_pimNeighborAddressType:
      case I_pimNeighborAddress:
	   if (nominator != -1) break;


      case I_pimNeighborUpTime:
           if(snmpPimNeighborUpTimeGet(USMDB_UNIT_CURRENT,pimNeighborEntryData.pimNeighborAddressType,
                             intIfNum, &pimSmNbrAddr, &pimNeighborEntryData.pimNeighborUpTime) == L7_SUCCESS)
           {
             SET_VALID(I_pimNeighborUpTime, pimNeighborEntryData.valid);
           }
	   if (nominator != -1) break;

      case I_pimNeighborExpiryTime:
           if(snmpPimNeighborExpiryTimeGet(USMDB_UNIT_CURRENT, pimNeighborEntryData.pimNeighborAddressType,
                          intIfNum, &pimSmNbrAddr,&pimNeighborEntryData.pimNeighborExpiryTime) == L7_SUCCESS)
           {
             SET_VALID(I_pimNeighborExpiryTime, pimNeighborEntryData.valid);
           }
           if (nominator != -1) break;

      case I_pimNeighborLanPruneDelayPresent:
           if (nominator != -1) break;

      case I_pimNeighborPropagationDelay:
           if (nominator != -1) break;

      case I_pimNeighborOverrideInterval:
           if (nominator != -1) break;

      case I_pimNeighborTBit:
           if (nominator != -1) break;

      case I_pimNeighborGenerationIDPresent:
           if (nominator != -1) break;

      case I_pimNeighborGenerationIDValue:
           if (nominator != -1) break;

      case I_pimNeighborBidirCapable:
           if (nominator != -1) break;

      case I_pimNeighborDRPriorityPresent:
           if(usmDbPimsmNeighborDRPriorityPresentGet(intIfNum, pimNeighborEntryData.pimNeighborAddressType, &pimSmNbrAddr,
                             &pimNeighborEntryData.pimNeighborDRPriorityPresent) == L7_SUCCESS)
           {
             SET_VALID(I_pimNeighborDRPriorityPresent, pimNeighborEntryData.valid);
           }
	   if (nominator != -1) break;

      case I_pimNeighborDRPriority:
           if(usmDbPimsmNeighborDRPriorityGet(USMDB_UNIT_CURRENT,pimNeighborEntryData.pimNeighborAddressType, 
                                intIfNum, &pimSmNbrAddr,&pimNeighborEntryData.pimNeighborDRPriority) == L7_SUCCESS )
           {
             SET_VALID(I_pimNeighborDRPriority, pimNeighborEntryData.valid);
           }
	   break;
      default:
	/* unknown nominator */
	    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimNeighborEntryData.valid))
    return(NULL);

  return(&pimNeighborEntryData);
}

pimNbrSecAddressEntry_t *
k_pimNbrSecAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 pimNbrSecAddressIfIndex,
                            SR_INT32 pimNbrSecAddressType,
                            OctetString * pimNbrSecAddressPrimary,
                            OctetString * pimNbrSecAddress)
{
 #ifdef NOT_SUPPORTED
  static pimNbrSecAddressEntry_t pimNbrSecAddressEntryData;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);


  /* Clear all the bits*/
  ZERO_VALID(pimNbrSecAddressEntryData.valid);
  pimNbrSecAddressEntryData.pimNbrSecAddressIfIndex = pimNbrSecAddressIfIndex;
  pimNbrSecAddressEntryData.pimNbrSecAddressType = pimNbrSecAddressType;
  pimNbrSecAddressEntryData.pimNbrSecAddressPrimary = pimNbrSecAddressPrimary;
  pimNbrSecAddressEntryData.pimNbrSecAddress = pimNbrSecAddress;
  SET_VALID(I_pimNbrSecAddressIfIndex, pimNbrSecAddressEntryData.valid);
  SET_VALID(I_pimNbrSecAddressType, pimNbrSecAddressEntryData.valid);
  SET_VALID(I_pimNbrSecAddressPrimary, pimNbrSecAddressEntryData.valid);
  SET_VALID(I_pimNbrSecAddress, pimNbrSecAddressEntryData.valid);


    switch (nominator)
    {
      case -1:
      case I_pimNbrSecAddressIfIndex: /* already got it above*/
	         break;
      case I_pimNbrSecAddressType:
	         break;
      case I_pimNbrSecAddressPrimary:
	         break;
      case I_pimNbrSecAddress:
	         break;
      default:
	/* unknown nominator */
	    return(NULL);
      return(NULL);

    }

  if (nominator >= 0 && !VALID(nominator, pimNbrSecAddressEntryData.valid))
    return(NULL);

   return(&pimNbrSecAddressEntryData);
  #else
    return(NULL);
  #endif
}

pimStarGEntry_t *
k_pimStarGEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 pimStarGAddressType,
                    OctetString * pimStarGGrpAddress)
{
  static pimStarGEntry_t pimStarGEntryData;
  L7_inet_addr_t pimStarGGrpAddr, pimStarGRPAddr, pimStarGUpstreamNbr, pimStarGRPFNextHop, pimStarGRPFRouteAddr;

  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimStarGAddressType != L7_NULL) &&
     (pimStarGAddressType != L7_AF_INET) &&
     (pimStarGAddressType != L7_AF_INET6))
        return(NULL);


  memset(&pimStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimStarGRPAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimStarGUpstreamNbr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimStarGRPFNextHop, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimStarGRPFRouteAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimStarGEntryData.pimStarGRPAddress = MakeOctetString(NULL, 0);
    pimStarGEntryData.pimStarGUpstreamNeighbor = MakeOctetString(NULL, 0);
    pimStarGEntryData.pimStarGRPFNextHop = MakeOctetString(NULL, 0);
    pimStarGEntryData.pimStarGRPFRouteAddress = MakeOctetString(NULL, 0);
  }

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimStarGEntryData.pimStarGRPAddress,NULL, 0);
    (void)SafeMakeOctetString(&pimStarGEntryData.pimStarGUpstreamNeighbor, NULL, 0);
    (void)SafeMakeOctetString(&pimStarGEntryData.pimStarGRPFNextHop, NULL, 0);
    (void)SafeMakeOctetString(&pimStarGEntryData.pimStarGRPFRouteAddress, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimStarGEntryData.valid);
  if(pimStarGAddressType == L7_NULL)
  {
    pimStarGEntryData.pimStarGAddressType = L7_AF_INET;
  }
  else
  {
    pimStarGEntryData.pimStarGAddressType = pimStarGAddressType;
  }
  SET_VALID(I_pimStarGAddressType, pimStarGEntryData.valid);

  if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    ipAddr = OctetStringToIP(pimStarGGrpAddress);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimStarGGrpAddr))
    {
      return(NULL);
    }
  }
  else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    OctetStringToIP6(pimStarGGrpAddress, (L7_char8*)&ipAddr);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimStarGGrpAddr))
    {
      return(NULL);
    }
  }

  if((searchType == EXACT) ?
    (usmDbPimsmStarGEntryGet(pimStarGEntryData.pimStarGAddressType, &pimStarGGrpAddr) != L7_SUCCESS) :
    ((usmDbPimsmStarGEntryGet(pimStarGEntryData.pimStarGAddressType, &pimStarGGrpAddr) != L7_SUCCESS) &&
    (usmDbPimsmStarGEntryNextGet(pimStarGEntryData.pimStarGAddressType, &pimStarGGrpAddr) != L7_SUCCESS)))
  {
    if((searchType != EXACT) && (pimStarGEntryData.pimStarGAddressType == L7_AF_INET))
    {
      memset(&pimStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
      pimStarGGrpAddr.family = L7_AF_INET6;
      pimStarGEntryData.pimStarGAddressType = L7_AF_INET6;
      if((usmDbPimsmStarGEntryGet(pimStarGEntryData.pimStarGAddressType, &pimStarGGrpAddr) != L7_SUCCESS) &&
         (usmDbPimsmStarGEntryNextGet(pimStarGEntryData.pimStarGAddressType, &pimStarGGrpAddr) != L7_SUCCESS))
      {
        ZERO_VALID(pimStarGEntryData.valid);
        return (NULL);
      }
    }
    else
    {
      ZERO_VALID(pimStarGEntryData.valid);
      return (NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimStarGGrpAddr, &ipAddr))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimStarGEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimStarGGrpAddress, pimStarGEntryData.valid);
    }
  }
  else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimStarGGrpAddr, &ipAddr))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimStarGEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimStarGGrpAddress, pimStarGEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimStarGAddressType: /* already got it above*/
        break;
    case I_pimStarGGrpAddress:
        break;
    case I_pimStarGUpTime:
         if(usmDbPimsmStarGUpTimeGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
         break;
    case I_pimStarGPimMode:
         if(usmDbPimsmStarGPimModeGet(pimStarGEntryData.pimStarGAddressType, 
                                            &pimStarGGrpAddr, &pimStarGEntryData.pimStarGPimMode) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGRPAddressType:
         if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
         {
            pimStarGEntryData.pimStarGRPAddressType = D_pimStarGAddressType_ipv4;
            SET_VALID(nominator, pimStarGEntryData.valid);
         }
         else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
         {
            pimStarGEntryData.pimStarGRPAddressType = D_pimStarGAddressType_ipv6;
            SET_VALID(nominator, pimStarGEntryData.valid);
         }
         break;           
    case I_pimStarGRPAddress:
         if(usmDbPimsmStarGRPAddrGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGRPAddr) == L7_SUCCESS)
         {
           if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStarGRPAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGRPAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
           else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStarGRPAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGRPAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimStarGPimModeOrigin:
         if(usmDbPimsmStarGRPOriginGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGPimModeOrigin) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGRPIsLocal:
         if(usmDbPimsmStarGRPIsLocalGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGRPIsLocal) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGUpstreamJoinState:
         if(usmDbPimsmStarGUpstreamJoinStateGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGUpstreamJoinState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGUpstreamJoinTimer:
         if(usmDbPimsmStarGUpstreamJoinTimerGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGUpstreamJoinTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGUpstreamNeighborType:
         if(usmDbPimsmStarGUpstreamNbrGet(pimStarGEntryData.pimStarGAddressType,
                                           &pimStarGGrpAddr, &pimStarGUpstreamNbr) == L7_SUCCESS)
         {
           if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
              pimStarGEntryData.pimStarGUpstreamNeighborType = D_pimStarGAddressType_ipv4;
              SET_VALID(nominator, pimStarGEntryData.valid);
           }
           else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
              pimStarGEntryData.pimStarGUpstreamNeighborType = D_pimStarGAddressType_ipv6;
              SET_VALID(nominator, pimStarGEntryData.valid);
           }
         }
         break; 
    case I_pimStarGUpstreamNeighbor:
         if(usmDbPimsmStarGUpstreamNbrGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGUpstreamNbr) == L7_SUCCESS)
         {
           if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStarGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
           else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStarGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimStarGRPFIfIndex:
         if(usmDbPimsmStarGRPFIfIndexGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGRPFIfIndex) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGRPFNextHopType:
         if(usmDbPimsmStarGRPFNextHopGet(pimStarGEntryData.pimStarGAddressType,
                                           &pimStarGGrpAddr, &pimStarGRPFNextHop) == L7_SUCCESS)
         {
           if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
              pimStarGEntryData.pimStarGRPFNextHopType = D_pimStarGAddressType_ipv4;
              SET_VALID(nominator, pimStarGEntryData.valid);
           }
           else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
              pimStarGEntryData.pimStarGRPFNextHopType = D_pimStarGAddressType_ipv6;
              SET_VALID(nominator, pimStarGEntryData.valid);
           }
         }
         break;
    case I_pimStarGRPFNextHop:
         if(usmDbPimsmStarGRPFNextHopGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGRPFNextHop) == L7_SUCCESS)
         {
           if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStarGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
           else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStarGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimStarGRPFRouteProtocol:
         if(usmDbPimsmStarGRPFRouteProtocolGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGRPFRouteProtocol) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGRPFRouteAddress:
         if(usmDbPimsmStarGRPFRouteAddrGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGRPFRouteAddr) == L7_SUCCESS)
         {
           if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStarGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
           else if(pimStarGEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStarGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGEntryData.pimStarGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimStarGRPFRoutePrefixLength:
         if(usmDbPimsmStarGRPFRoutePrefixLenGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGRPFRoutePrefixLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGRPFRouteMetricPref:
         if(usmDbPimsmStarGRPFRouteMetricPrefGet(pimStarGEntryData.pimStarGAddressType, 
                                           &pimStarGGrpAddr, &pimStarGEntryData.pimStarGRPFRouteMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    case I_pimStarGRPFRouteMetric:
         if(usmDbPimsmStarGRPFRouteMetricGet(pimStarGEntryData.pimStarGAddressType, 
                               &pimStarGGrpAddr, &pimStarGEntryData.pimStarGRPFRouteMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGEntryData.valid);
         }
        break;
    default:
	/* unknown nominator */
	    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimStarGEntryData.valid))
    return(NULL);

  return(&pimStarGEntryData);
}

pimStarGIEntry_t *
k_pimStarGIEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 pimStarGAddressType,
                     OctetString * pimStarGGrpAddress,
                     SR_INT32 pimStarGIIfIndex)
{
  static pimStarGIEntry_t pimStarGIEntryData;
  L7_inet_addr_t pimStarGIAssertWinnerAddr, pimStarGGrpAddr;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if((pimStarGAddressType != L7_NULL) &&
     (pimStarGAddressType != L7_AF_INET) &&
     (pimStarGAddressType != L7_AF_INET6))
        return(NULL);

  memset(&pimStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimStarGIAssertWinnerAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimStarGIEntryData.pimStarGIAssertWinnerAddress = MakeOctetString(NULL, 0);
    pimStarGIEntryData.pimStarGGrpAddress = MakeOctetString(NULL, 0);
  }

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimStarGIEntryData.pimStarGIAssertWinnerAddress,NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimStarGIEntryData.valid);
  if(pimStarGAddressType == L7_NULL)
  {
    pimStarGIEntryData.pimStarGAddressType = L7_AF_INET;
  }
  else
  {
    pimStarGIEntryData.pimStarGAddressType = pimStarGAddressType;
  }

  (void)SafeMakeOctetString(&pimStarGIEntryData.pimStarGGrpAddress,NULL, 0);
  pimStarGIEntryData.pimStarGIIfIndex = pimStarGIIfIndex;
  SET_VALID(I_pimStarGIEntryIndex_pimStarGAddressType, pimStarGIEntryData.valid);
  SET_VALID(I_pimStarGIIfIndex, pimStarGIEntryData.valid);

  if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    ipAddr = OctetStringToIP(pimStarGGrpAddress);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimStarGGrpAddr))
    {
      return(NULL);
    }

  }
  else if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));

    OctetStringToIP6(pimStarGGrpAddress, (L7_char8*)&ipAddr);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimStarGGrpAddr))
    {
      return(NULL);
    }

  }

  if((searchType == EXACT) ?
    (usmDbPimsmStarGIEntryGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr, pimStarGIEntryData.pimStarGIIfIndex) != L7_SUCCESS) :
    ((usmDbPimsmStarGIEntryGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr, pimStarGIEntryData.pimStarGIIfIndex) != L7_SUCCESS) &&
    (usmDbPimStarGIEntryNextGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr, &pimStarGIEntryData.pimStarGIIfIndex) != L7_SUCCESS)))
  {

    if((searchType != EXACT) && (pimStarGIEntryData.pimStarGAddressType == L7_AF_INET))
    {
      memset(&pimStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
      pimStarGGrpAddr.family = L7_AF_INET6;
      pimStarGIEntryData.pimStarGAddressType = L7_AF_INET6;
      if((usmDbPimsmStarGIEntryGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr, pimStarGIEntryData.pimStarGIIfIndex) != L7_SUCCESS) &&
         (usmDbPimsmStarGIEntryNextGet(pimStarGIEntryData.pimStarGAddressType,&pimStarGGrpAddr,&pimStarGIEntryData.pimStarGIIfIndex) != L7_SUCCESS))
      { 
        return(NULL);
      }
    }
    else
    { 
      return(NULL);
    }
  }
  /*  converting the IP Address to Octet String */
  if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimStarGGrpAddr, &ipAddr))
    { 
      return(NULL);
    }
    if (SafeMakeOctetString(&pimStarGIEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimStarGIEntryIndex_pimStarGGrpAddress, pimStarGIEntryData.valid);
    }
  }
  else if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimStarGGrpAddr, &ipAddr))
    { 
      return(NULL);
    }
    if (SafeMakeOctetString(&pimStarGIEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimStarGIEntryIndex_pimStarGGrpAddress, pimStarGIEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimStarGIEntryIndex_pimStarGAddressType:
    case I_pimStarGIEntryIndex_pimStarGGrpAddress:
    case I_pimStarGIIfIndex:
        break;
    case I_pimStarGIUpTime:
         if(usmDbPimsmStarGIUpTimeGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGILocalMembership:
         if(usmDbPimsmStarGILocalMembershipGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGILocalMembership) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIJoinPruneState:
         if(usmDbPimsmStarGIJoinPruneStateGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIJoinPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIPrunePendingTimer:
         if(usmDbPimsmStarGIPrunePendingTimerGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIPrunePendingTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIJoinExpiryTimer:
         if(usmDbPimsmStarGIJoinExpiryTimerGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIJoinExpiryTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIAssertState:
         if(usmDbPimsmStarGIAssertStateGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIAssertState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIAssertTimer:
         if(usmDbPimsmStarGIAssertTimerGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIAssertTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIAssertWinnerAddressType:
         if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
         {
           pimStarGIEntryData.pimStarGIAssertWinnerAddressType = D_pimStarGAddressType_ipv4;
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
         else
         if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
         {
           pimStarGIEntryData.pimStarGIAssertWinnerAddressType = D_pimStarGAddressType_ipv6;
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
         break;  
    case I_pimStarGIAssertWinnerAddress:
         if(usmDbPimsmStarGIAssertWinnerAddrGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIAssertWinnerAddr) == L7_SUCCESS)
         {
           if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStarGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGIEntryData.pimStarGIAssertWinnerAddress,
                                       (L7_char8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGIEntryData.valid);
               }
             }
           }
           else if(pimStarGIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStarGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimStarGIEntryData.pimStarGIAssertWinnerAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimStarGIEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimStarGIAssertWinnerMetricPref:
         if(usmDbPimsmStarGIRPFRouteMetricPrefGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIAssertWinnerMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    case I_pimStarGIAssertWinnerMetric:
         if(usmDbPimsmStarGIAssertWinnerMetricGet(pimStarGIEntryData.pimStarGAddressType, &pimStarGGrpAddr,
                                            pimStarGIEntryData.pimStarGIIfIndex, &pimStarGIEntryData.pimStarGIAssertWinnerMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimStarGIEntryData.valid);
         }
        break;
    default:
    /* unknown nominator */
     return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, pimStarGIEntryData.valid))
    return(NULL);

  return(&pimStarGIEntryData);
}

/**********************************************************************
 * PIM-SM SGENTRY TABLE IMPLEMENTAION
 *********************************************************************/
pimSGEntry_t *
k_pimSGEntry_get(int serialNum, ContextInfo *contextInfo,
                 int nominator,
                 int searchType,
                 SR_INT32 pimSGAddressType,
                 OctetString * pimSGGrpAddress,
                 OctetString * pimSGSrcAddress)
{
  static pimSGEntry_t pimSGEntryData;
  L7_inet_addr_t pimSGGrpAddr, pimSGSrcAddr, pimSGRPFNextHop, pimSGRPRegisterPMBRAddr, pimSGRPFRouteAddr, pimSGUpstreamNbr;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);


  if((pimSGAddressType != L7_NULL) &&
     (pimSGAddressType != L7_AF_INET) &&
     (pimSGAddressType != L7_AF_INET6))
        return(NULL);

  memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSGRPFNextHop, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimSGEntryData.pimSGRPFNextHop = MakeOctetString(NULL, 0);
    pimSGEntryData.pimSGGrpAddress = MakeOctetString(NULL, 0);
    pimSGEntryData.pimSGSrcAddress = MakeOctetString(NULL, 0);
  }

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimSGEntryData.pimSGRPFNextHop, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimSGEntryData.valid);
  if(pimSGAddressType == L7_NULL)
  {
    pimSGEntryData.pimSGAddressType = L7_AF_INET;
  }
  else
  {
    pimSGEntryData.pimSGAddressType = pimSGAddressType;
  }
  (void)SafeMakeOctetString(&pimSGEntryData.pimSGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSGEntryData.pimSGSrcAddress, NULL, 0);
  SET_VALID(I_pimSGAddressType, pimSGEntryData.valid);
  SET_VALID(I_pimSGGrpAddress, pimSGEntryData.valid);
  SET_VALID(I_pimSGSrcAddress, pimSGEntryData.valid);

  if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    ipAddr1 = OctetStringToIP(pimSGGrpAddress);
    ipAddr2 = OctetStringToIP(pimSGSrcAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }
  else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    OctetStringToIP6(pimSGGrpAddress, (L7_char8*)&ipAddr1);
    OctetStringToIP6(pimSGSrcAddress, (L7_char8*)&ipAddr2);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }

  if((searchType == EXACT) ?
    (usmDbPimSGEntryGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS) :
    ((usmDbPimSGEntryGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS) &&
    (usmDbPimSGEntryNextGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS)))
  {
    if((searchType != EXACT) && (pimSGEntryData.pimSGAddressType == L7_AF_INET))
    {
      memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
      memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
      pimSGEntryData.pimSGAddressType = L7_AF_INET6;
      if((usmDbPimSGEntryGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS) &&
         (usmDbPimSGEntryNextGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS))
      {
        return(NULL);
      }
    }
    else
    {
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGEntryData.pimSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGGrpAddress, pimSGEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGEntryData.pimSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGSrcAddress, pimSGEntryData.valid);
    }
  }
  else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGEntryData.pimSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGGrpAddress, pimSGEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGEntryData.pimSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGSrcAddress, pimSGEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSGAddressType: /* already got it above*/
        break;
    case I_pimSGGrpAddress:
        break;
    case I_pimSGSrcAddress:
        break;
    case I_pimSGUpTime:
         if(usmDbPimsmSGUpTimeGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGPimMode:
         if(usmDbPimsmSGPimModeGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           (pimMode_t*) &pimSGEntryData.pimSGPimMode) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGUpstreamJoinState:
         if(usmDbPimsmSGUpstreamJoinStateGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGUpstreamJoinState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGUpstreamJoinTimer:
         if(usmDbPimsmSGUpstreamJoinTimerGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGUpstreamJoinTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGUpstreamNeighbor:
         if(usmDbPimsmSGUpstreamNeighborGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGUpstreamNbr) == L7_SUCCESS)
         {
           if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
           else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
         }
         break;
    case I_pimSGRPFIfIndex:
         if(usmDbPimsmSGRPFIfIndexGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGRPFIfIndex) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGRPFNextHopType:
         if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
         {
           pimSGEntryData.pimSGRPFNextHopType = D_pimSGAddressType_ipv4;
           SET_VALID(nominator, pimSGEntryData.valid);
         }  
         else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
         {
           pimSGEntryData.pimSGRPFNextHopType = D_pimSGAddressType_ipv6;
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break; 
    case I_pimSGRPFNextHop:
         if(usmDbPimsmSGRPFNextHopGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGRPFNextHop) == L7_SUCCESS)
         {
           if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
           else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
         }
         break;
    case I_pimSGRPFRouteProtocol:
         if(usmDbPimsmSGRPFRouteProtocolGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGRPFRouteProtocol) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGRPFRouteAddress:
         if(usmDbPimsmSGRPFRouteAddressGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGRPFRouteAddr) == L7_SUCCESS)
         {
           if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
           else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
         }
         break;
    case I_pimSGRPFRoutePrefixLength:
         if(usmDbPimsmSGRPFRoutePrefixLengthGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGRPFRoutePrefixLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGRPFRouteMetricPref:
         if(usmDbPimsmSGRPFRouteMetricPrefGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGRPFRouteMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGRPFRouteMetric:
         if(usmDbPimsmSGRPFRouteMetricGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGRPFRouteMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGSPTBit:
         if(usmDbPimsmSGSPTBitGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGSPTBit) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGKeepaliveTimer:
         if(usmDbPimsmSGKeepaliveTimerGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGKeepaliveTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGDRRegisterState:
         if(usmDbPimsmSGDRRegisterStateGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGDRRegisterState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGDRRegisterStopTimer:
         if(usmDbPimsmSGDRRegisterStopTimerGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGEntryData.pimSGDRRegisterStopTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGRPRegisterPMBRAddressType:
         if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
         {
           pimSGEntryData.pimSGRPRegisterPMBRAddressType = D_pimSGAddressType_ipv4;
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
         {
           pimSGEntryData.pimSGRPRegisterPMBRAddressType = D_pimSGAddressType_ipv6;
           SET_VALID(nominator, pimSGEntryData.valid);
         }
         break;
    case I_pimSGRPRegisterPMBRAddress:
         if(usmDbPimsmSGRPRegisterPMBRAddressGet(pimSGEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGRPRegisterPMBRAddr) == L7_SUCCESS)
         {
           if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSGRPRegisterPMBRAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGRPRegisterPMBRAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
           else if(pimSGEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSGRPRegisterPMBRAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGEntryData.pimSGRPRegisterPMBRAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGEntryData.valid);
               }
             }
           }
         }
         break;
    default:
  /* unknown nominator */
   return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSGEntryData.valid))
    return(NULL);

  return(&pimSGEntryData);
}

pimSGIEntry_t *
k_pimSGIEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 pimSGAddressType,
                  OctetString * pimSGGrpAddress,
                  OctetString * pimSGSrcAddress,
                  SR_INT32 pimSGIIfIndex)
{
  static pimSGIEntry_t pimSGIEntryData;
  L7_inet_addr_t pimSGGrpAddr, pimSGSrcAddr, pimSGIAssertWinnerAddr;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSGIEntryData.pimSGGrpAddress = MakeOctetString(NULL, 0);
    pimSGIEntryData.pimSGSrcAddress = MakeOctetString(NULL, 0);
    pimSGIEntryData.pimSGIAssertWinnerAddress = MakeOctetString(NULL, 0);
  }


  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimSGIEntryData.pimSGIAssertWinnerAddress, NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimSGAddressType != L7_NULL) &&
     (pimSGAddressType != L7_AF_INET) &&
     (pimSGAddressType != L7_AF_INET6))
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(pimSGIEntryData.valid);

  if(pimSGAddressType == L7_NULL)
  {
    pimSGIEntryData.pimSGAddressType = L7_AF_INET;
  }
  else
  {
    pimSGIEntryData.pimSGAddressType = pimSGAddressType;
  }

  (void)SafeMakeOctetString(&pimSGIEntryData.pimSGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSGIEntryData.pimSGSrcAddress, NULL, 0);

  pimSGIEntryData.pimSGIIfIndex = pimSGIIfIndex;
  SET_VALID(I_pimSGIEntryIndex_pimSGAddressType, pimSGIEntryData.valid);
  SET_VALID(I_pimSGIEntryIndex_pimSGGrpAddress, pimSGIEntryData.valid);
  SET_VALID(I_pimSGIEntryIndex_pimSGSrcAddress, pimSGIEntryData.valid);
  SET_VALID(I_pimSGIIfIndex, pimSGIEntryData.valid);


  memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSGIAssertWinnerAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    ipAddr1 = OctetStringToIP(pimSGGrpAddress);
    ipAddr2 = OctetStringToIP(pimSGSrcAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSGGrpAddr))
    { 
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }
  else if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    OctetStringToIP6(pimSGGrpAddress, (L7_char8*)&ipAddr1);
    OctetStringToIP6(pimSGSrcAddress, (L7_char8*)&ipAddr2);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }

  if((searchType == EXACT) ?
    (usmDbPimSGIEntryGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr, pimSGIEntryData.pimSGIIfIndex) != L7_SUCCESS) :
    ((usmDbPimSGIEntryGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr, pimSGIEntryData.pimSGIIfIndex) != L7_SUCCESS) &&
    (usmDbPimSGIEntryNextGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr, &pimSGIEntryData.pimSGIIfIndex) != L7_SUCCESS)))
  {
    if((searchType != EXACT) && (pimSGIEntryData.pimSGAddressType == L7_AF_INET))
    {
      memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
      memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
      pimSGIEntryData.pimSGAddressType = L7_AF_INET6;
      pimSGGrpAddr.family = L7_AF_INET6;
      pimSGSrcAddr.family = L7_AF_INET6;
      if((usmDbPimSGIEntryGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr, pimSGIEntryData.pimSGIIfIndex) != L7_SUCCESS) &&
         (usmDbPimSGIEntryNextGet(pimSGIEntryData.pimSGAddressType,&pimSGGrpAddr,&pimSGSrcAddr, &pimSGIEntryData.pimSGIIfIndex) != L7_SUCCESS))
      {
        return(NULL);
      }
    }
    else
    {
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGIEntryData.pimSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGIEntryIndex_pimSGGrpAddress, pimSGIEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGIEntryData.pimSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGIEntryIndex_pimSGSrcAddress, pimSGIEntryData.valid);
    }
  }
  else if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGIEntryData.pimSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGIEntryIndex_pimSGGrpAddress, pimSGIEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGIEntryData.pimSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGIEntryIndex_pimSGSrcAddress, pimSGIEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSGIEntryIndex_pimSGAddressType:
    case I_pimSGIEntryIndex_pimSGGrpAddress:
    case I_pimSGIEntryIndex_pimSGSrcAddress:
    case I_pimSGIIfIndex:
        break;
    case I_pimSGIUpTime:
         if(usmDbPimSGIUpTimeGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGILocalMembership:
         if(usmDbPimSGILocalMembershipGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGILocalMembership) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIJoinPruneState:
         if(usmDbPimSGIJoinPruneStateGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIJoinPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIPrunePendingTimer:
         if(usmDbPimSGIPrunePendingTimerGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIPrunePendingTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIJoinExpiryTimer:
         if(usmDbPimSGIJoinExpiryTimerGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIJoinExpiryTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIAssertState:
         if(usmDbPimSGIAssertStateGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIAssertState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIAssertTimer:
         if(usmDbPimSGIAssertTimerGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIAssertTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIAssertWinnerAddressType:
         if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
         { 
           pimSGIEntryData.pimSGIAssertWinnerAddressType = D_pimSGAddressType_ipv4;
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
         else if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
         {
           pimSGIEntryData.pimSGIAssertWinnerAddressType = D_pimSGAddressType_ipv6;
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
         break;
    case I_pimSGIAssertWinnerAddress:
         if(usmDbPimSGIAssertWinnerAddrGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIAssertWinnerAddr) == L7_SUCCESS)
         {
           if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGIEntryData.pimSGIAssertWinnerAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGIEntryData.valid);
               }
             }
           }
           else if(pimSGIEntryData.pimSGAddressType == D_pimSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSGIEntryData.pimSGIAssertWinnerAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSGIEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSGIAssertWinnerMetricPref:
         if(usmDbPimSGIRPFRouteMetricPrefGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIAssertWinnerMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    case I_pimSGIAssertWinnerMetric:
         if(usmDbPimSGIAssertWinnerMetricGet(pimSGIEntryData.pimSGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGIEntryData.pimSGIIfIndex, &pimSGIEntryData.pimSGIAssertWinnerMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGIEntryData.valid);
         }
        break;
    default:
 /* unknown nominator */
     return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, pimSGIEntryData.valid))
    return(NULL);

  return(&pimSGIEntryData);
}

/**********************************************************************
 * PIM-SM SGRptENTRY TABLE IMPLEMENTAION
 *********************************************************************/
pimSGRptEntry_t *
k_pimSGRptEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 pimStarGAddressType,
                    OctetString * pimStarGGrpAddress,
                    OctetString * pimSGRptSrcAddress)
{
  static pimSGRptEntry_t pimSGRptEntryData;
  L7_inet_addr_t pimSGGrpAddr, pimSGSrcAddr;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSGRptEntryData.pimStarGGrpAddress = MakeOctetString(NULL, 0);
    pimSGRptEntryData.pimSGRptSrcAddress = MakeOctetString(NULL, 0);
  }


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimStarGAddressType != L7_NULL) &&
    (pimStarGAddressType != L7_AF_INET) &&
    (pimStarGAddressType != L7_AF_INET6))
            return(NULL);


   memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
   memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));

  /* Clear all the bits*/
  ZERO_VALID(pimSGRptEntryData.valid);
  if(pimStarGAddressType == L7_NULL)
  {
    pimSGRptEntryData.pimStarGAddressType = L7_AF_INET;
  }
  else
  {
    pimSGRptEntryData.pimStarGAddressType = pimStarGAddressType;
  }

  (void)SafeMakeOctetString(&pimSGRptEntryData.pimStarGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSGRptEntryData.pimSGRptSrcAddress, NULL, 0);
  SET_VALID(I_pimSGRptEntryIndex_pimStarGAddressType, pimSGRptEntryData.valid);
  SET_VALID(I_pimSGRptEntryIndex_pimStarGGrpAddress, pimSGRptEntryData.valid);
  SET_VALID(I_pimSGRptSrcAddress, pimSGRptEntryData.valid);

  if(pimSGRptEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    ipAddr1 = OctetStringToIP(pimStarGGrpAddress);
    ipAddr2 = OctetStringToIP(pimSGRptSrcAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }
  else if(pimSGRptEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    OctetStringToIP6(pimStarGGrpAddress, (L7_char8*)&ipAddr1);
    OctetStringToIP6(pimSGRptSrcAddress, (L7_char8*)&ipAddr2);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }
  else
  {
    return L7_NULLPTR;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmSGRptEntryGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS) :
    ((usmDbPimsmSGRptEntryGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS) &&
    (usmDbPimsmSGRptEntryNextGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS)))
  {
    if((searchType != EXACT) && (pimSGRptEntryData.pimStarGAddressType == L7_AF_INET))
    {
      memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
      memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
      pimSGRptEntryData.pimStarGAddressType = L7_AF_INET6;
      pimSGGrpAddr.family = L7_AF_INET6;
      pimSGSrcAddr.family = L7_AF_INET6;
      if((usmDbPimsmSGRptEntryGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS) &&
         (usmDbPimsmSGRptEntryNextGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr) != L7_SUCCESS))
      {
        return(NULL);
      }
    }
    else
    {
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSGRptEntryData.pimStarGAddressType == D_pimSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptEntryIndex_pimStarGGrpAddress, pimSGRptEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptEntryData.pimSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptSrcAddress, pimSGRptEntryData.valid);
    }
  }
  else if(pimSGRptEntryData.pimStarGAddressType == D_pimSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptEntryIndex_pimStarGGrpAddress, pimSGRptEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptEntryData.pimSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptSrcAddress, pimSGRptEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSGRptEntryIndex_pimStarGAddressType: /* already got it above*/
  break;
    case I_pimSGRptEntryIndex_pimStarGGrpAddress:
  break;
    case I_pimSGRptSrcAddress:
  break;
    case I_pimSGRptUpTime:
         if(usmDbPimsmSGRptUpTimeGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGRptEntryData.pimSGRptUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptEntryData.valid);
         }
  break;
    case I_pimSGRptUpstreamPruneState:
         if(usmDbPimsmSGRptUpstreamPruneStateGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGRptEntryData.pimSGRptUpstreamPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptEntryData.valid);
         }
  break;
    case I_pimSGRptUpstreamOverrideTimer:
         if(usmDbPimsmSGRptUpstreamOverrideTimerGet(pimSGRptEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           &pimSGRptEntryData.pimSGRptUpstreamOverrideTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptEntryData.valid);
         }
  break;
    default:
  /* unknown nominator */
   return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSGRptEntryData.valid))
    return(NULL);

  return(&pimSGRptEntryData);
}

pimSGRptIEntry_t *
k_pimSGRptIEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 pimStarGAddressType,
                     OctetString * pimStarGGrpAddress,
                     OctetString * pimSGRptSrcAddress,
                     SR_INT32 pimSGRptIIfIndex)
{
  static pimSGRptIEntry_t pimSGRptIEntryData;
  L7_inet_addr_t pimSGGrpAddr, pimSGSrcAddr;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSGRptIEntryData.pimStarGGrpAddress = MakeOctetString(NULL, 0);
    pimSGRptIEntryData.pimSGRptSrcAddress = MakeOctetString(NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimStarGAddressType != L7_NULL) &&
      (pimStarGAddressType != L7_AF_INET) &&
      (pimStarGAddressType != L7_AF_INET6))
    return(NULL);

  memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));

  /* Clear all the bits*/
  ZERO_VALID(pimSGRptIEntryData.valid);
  if(pimStarGAddressType == L7_NULL)
  {
    pimSGRptIEntryData.pimStarGAddressType = L7_AF_INET;
  }
  else
  {
    pimSGRptIEntryData.pimStarGAddressType = pimStarGAddressType;
  }
  (void)SafeMakeOctetString(&pimSGRptIEntryData.pimStarGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSGRptIEntryData.pimSGRptSrcAddress, NULL, 0);
  pimSGRptIEntryData.pimSGRptIIfIndex = pimSGRptIIfIndex;
  SET_VALID(I_pimSGRptIEntryIndex_pimStarGAddressType, pimSGRptIEntryData.valid);
  SET_VALID(I_pimSGRptIEntryIndex_pimStarGGrpAddress, pimSGRptIEntryData.valid);
  SET_VALID(I_pimSGRptIEntryIndex_pimSGRptSrcAddress, pimSGRptIEntryData.valid);
  SET_VALID(I_pimSGRptIIfIndex, pimSGRptIEntryData.valid);


  if(pimSGRptIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    ipAddr1 = OctetStringToIP(pimStarGGrpAddress);
    ipAddr2 = OctetStringToIP(pimSGRptSrcAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }
  else if(pimSGRptIEntryData.pimStarGAddressType == D_pimStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
   
    OctetStringToIP6(pimStarGGrpAddress, (L7_char8*)&ipAddr1);
    OctetStringToIP6(pimSGRptSrcAddress, (L7_char8*)&ipAddr2);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSGGrpAddr))
    {
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSGSrcAddr))
    {
      return(NULL);
    }
  }

  if((searchType == EXACT) ?
    (usmDbPimsmSGRptIEntryGet(pimSGRptIEntryData.pimStarGAddressType,&pimSGGrpAddr,&pimSGSrcAddr,pimSGRptIEntryData.pimSGRptIIfIndex)!=L7_SUCCESS) :
    ((usmDbPimsmSGRptIEntryGet(pimSGRptIEntryData.pimStarGAddressType,&pimSGGrpAddr,&pimSGSrcAddr,pimSGRptIEntryData.pimSGRptIIfIndex)!=L7_SUCCESS) &&
    (usmDbPimsmSGRptIEntryNextGet(pimSGRptIEntryData.pimStarGAddressType,&pimSGGrpAddr,&pimSGSrcAddr,&pimSGRptIEntryData.pimSGRptIIfIndex)!=L7_SUCCESS)))
  {
    if((searchType != EXACT) && (pimSGRptIEntryData.pimStarGAddressType == L7_AF_INET))
    {
      memset(&pimSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
      memset(&pimSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
      pimSGRptIEntryData.pimStarGAddressType = L7_AF_INET6;
      pimSGGrpAddr.family = L7_AF_INET6;
      pimSGSrcAddr.family = L7_AF_INET6;
      if((usmDbPimsmSGRptIEntryGet(pimSGRptIEntryData.pimStarGAddressType,&pimSGGrpAddr,&pimSGSrcAddr,
                                         pimSGRptIEntryData.pimSGRptIIfIndex)!=L7_SUCCESS) &&
         (usmDbPimsmSGRptIEntryNextGet(pimSGRptIEntryData.pimStarGAddressType,&pimSGGrpAddr,
                                         &pimSGSrcAddr,&pimSGRptIEntryData.pimSGRptIIfIndex) != L7_SUCCESS))
      {
        return(NULL);
      }
    }
    else
    {
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSGRptIEntryData.pimStarGAddressType == D_pimSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptIEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptIEntryIndex_pimStarGGrpAddress, pimSGRptIEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptIEntryData.pimSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptIEntryIndex_pimSGRptSrcAddress, pimSGRptIEntryData.valid);
    }
  }
  else if(pimSGRptIEntryData.pimStarGAddressType == D_pimSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGGrpAddr, &ipAddr1))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptIEntryData.pimStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptIEntryIndex_pimStarGGrpAddress, pimSGRptIEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSGSrcAddr, &ipAddr2))
    {
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSGRptIEntryData.pimSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSGRptIEntryIndex_pimSGRptSrcAddress, pimSGRptIEntryData.valid);
    }
  }


  switch (nominator)
  {
    case -1:
    case I_pimSGRptIEntryIndex_pimStarGAddressType:
        break;
    case I_pimSGRptIEntryIndex_pimStarGGrpAddress:
        break;
    case I_pimSGRptIEntryIndex_pimSGRptSrcAddress:    
        break;
    case I_pimSGRptIIfIndex:
        break;
    case I_pimSGRptIUpTime:
         if(usmDbPimsmSGRptIUpTimeGet(pimSGRptIEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGRptIEntryData.pimSGRptIIfIndex, &pimSGRptIEntryData.pimSGRptIUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptIEntryData.valid);
         }
        break;
    case I_pimSGRptILocalMembership:
         if(usmDbPimsmSGRptILocalMembershipGet(pimSGRptIEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGRptIEntryData.pimSGRptIIfIndex, &pimSGRptIEntryData.pimSGRptILocalMembership) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptIEntryData.valid);
         }
        break;
    case I_pimSGRptIJoinPruneState:
         if(usmDbPimsmSGRptIJoinPruneStateGet(pimSGRptIEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGRptIEntryData.pimSGRptIIfIndex, &pimSGRptIEntryData.pimSGRptIJoinPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptIEntryData.valid);
         }
        break;
    case I_pimSGRptIPrunePendingTimer:
         if(usmDbPimsmSGRptIPrunePendingTimerGet(pimSGRptIEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGRptIEntryData.pimSGRptIIfIndex, &pimSGRptIEntryData.pimSGRptIPrunePendingTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptIEntryData.valid);
         }
        break;
    case I_pimSGRptIPruneExpiryTimer:
         if(usmDbPimsmSGRptIJoinExpiryTimerGet(pimSGRptIEntryData.pimStarGAddressType, &pimSGGrpAddr, &pimSGSrcAddr,
                           pimSGRptIEntryData.pimSGRptIIfIndex, &pimSGRptIEntryData.pimSGRptIPruneExpiryTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSGRptIEntryData.valid);
         }
        break;
    default:
 /* unknown nominator */
     return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSGRptIEntryData.valid))
    return(NULL);
  return(&pimSGRptIEntryData);
}

pimBidirDFElectionEntry_t *
k_pimBidirDFElectionEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 pimBidirDFElectionAddressType,
                              OctetString * pimBidirDFElectionRPAddress,
                              SR_INT32 pimBidirDFElectionIfIndex)
{

  #ifdef NOT_SUPPORTED

   static pimBidirDFElectionEntry_t pimBidirDFElectionEntryData;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(pimBidirDFElectionEntryData.valid);
  pimBidirDFElectionEntryData.pimBidirDFElectionAddressType = pimBidirDFElectionAddressType;
  pimBidirDFElectionEntryData.pimBidirDFElectionRPAddress = MakeOctetString(NULL, 0);
  pimBidirDFElectionEntryData.pimBidirDFElectionIfIndex = pimBidirDFElectionIfIndex;
  SET_VALID(I_pimBidirDFElectionAddressType, pimBidirDFElectionEntryData.valid);
  SET_VALID(I_pimBidirDFElectionRPAddress, pimBidirDFElectionEntryData.valid);
  SET_VALID(I_pimBidirDFElectionIfIndex, pimBidirDFElectionEntryData.valid);


    switch (nominator)
    {
      case -1:
      case I_pimBidirDFElectionAddressType:
	         break;
      case I_pimBidirDFElectionRPAddress:
	         break;
      case I_pimBidirDFElectionIfIndex:
	         break;
      case I_pimBidirDFElectionWinnerAddressType:
	         break;
      case I_pimBidirDFElectionWinnerAddress:
	         break;
      case I_pimBidirDFElectionWinnerUpTime:
	         break;
      case I_pimBidirDFElectionWinnerMetricPref:
	         break;
      case I_pimBidirDFElectionWinnerMetric:
	         break;
      case I_pimBidirDFElectionState:
	         break;
      case I_pimBidirDFElectionStateTimer:
	         break;
      default:
	/* unknown nominator */
	    return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, pimBidirDFElectionEntryData.valid))
    return(NULL);
   return(&pimBidirDFElectionEntryData);
  #else
   return(NULL);
  #endif
 
}
#if 0
/**********************************************************************
 * PIM-SM RANGE TABLE IMPLEMENTAION
 *********************************************************************/
pimSmSsmRangeEntry_t *
k_pimSmSsmRangeEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pimSmSsmRangeAddressType,
                       OctetString * pimSmSsmRangeAddress,
                       SR_UINT32 pimSmSsmRangePrefixLength)
{
  static pimSmSsmRangeEntry_t pimSmSsmRangeEntryData;
  L7_inet_addr_t pimSmSsmRangeAddr;
  L7_uchar8 charVal = L7_NULL;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSmSsmRangeEntryData.pimSmSsmRangeAddress = MakeOctetString(NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

 

  /* Clear all the bits*/
  ZERO_VALID(pimSmSsmRangeEntryData.valid);
  pimSmSsmRangeEntryData.pimSmSsmRangeAddressType = pimSmSsmRangeAddressType;
  (void)SafeMakeOctetString(&pimSmSsmRangeEntryData.pimSmSsmRangeAddress, NULL, 0);
  pimSmSsmRangeEntryData.pimSmSsmRangePrefixLength = pimSmSsmRangePrefixLength;
  SET_VALID(I_pimSmSsmRangeAddressType, pimSmSsmRangeEntryData.valid);
  SET_VALID(I_pimSmSsmRangeAddress, pimSmSsmRangeEntryData.valid);
  SET_VALID(I_pimSmSsmRangePrefixLength, pimSmSsmRangeEntryData.valid);

  charVal = (L7_uchar8)pimSmSsmRangePrefixLength;
  if(pimSmSsmRangeAddressType == L7_NULL)
    pimSmSsmRangeEntryData.pimSmSsmRangeAddressType = L7_AF_INET;

  if(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType == D_pimSmSsmRangeAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr = 0;
    }
    else 
    {
      ipAddr = OctetStringToIP(pimSmSsmRangeAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmSsmRangeAddr))
    {
        firstTime =  L7_TRUE;
        return(NULL);
    }
    pimSmSsmRangeEntryData.pimSmSsmRangeAddressType = L7_AF_INET;
  }
  else if(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType == D_pimSmSsmRangeAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else 
    {
      OctetStringToIP6(pimSmSsmRangeAddress, (L7_char8*)&ipAddr);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmSsmRangeAddr))
    {
        firstTime =  L7_TRUE;
        ZERO_VALID(pimSmSsmRangeEntryData.valid);
        return(NULL);
    }
    pimSmSsmRangeEntryData.pimSmSsmRangeAddressType = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmSsmRangeEntryGet(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType, &pimSmSsmRangeAddr, charVal) != L7_SUCCESS) :
    ((usmDbPimsmSsmRangeEntryGet(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType, &pimSmSsmRangeAddr, charVal) != L7_SUCCESS) &&
    (usmDbPimsmSsmRangeEntryNextGet(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType,&pimSmSsmRangeAddr,&charVal) != L7_SUCCESS)))

  {
    if(searchType != EXACT)
    {
      if(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType == L7_AF_INET)
      {
        memset(&pimSmSsmRangeAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmSsmRangeEntryData.pimSmSsmRangeAddressType = L7_AF_INET6;
        pimSmSsmRangeAddr.family = L7_AF_INET6;
        charVal = L7_NULL;
        if((usmDbPimsmSsmRangeEntryGet(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType, &pimSmSsmRangeAddr, charVal) != L7_SUCCESS) &&
           (usmDbPimsmSsmRangeEntryNextGet(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType, &pimSmSsmRangeAddr,&charVal) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          return(NULL);
        }
      }
    }
    else
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
  }

  pimSmSsmRangeEntryData.pimSmSsmRangePrefixLength = charVal;

  /*  by converting the IP Address to Octet String */
  if(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType == D_pimSmSsmRangeAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSsmRangeAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSsmRangeEntryData.pimSmSsmRangeAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSsmRangeAddress, pimSmSsmRangeEntryData.valid);
    }
  }
  else if(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType == D_pimSmSsmRangeAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSsmRangeAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSsmRangeEntryData.pimSmSsmRangeAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSsmRangeAddress, pimSmSsmRangeEntryData.valid);
    }
  }
  SET_VALID (I_pimSmSsmRangePrefixLength, pimSmSsmRangeEntryData.valid);

  switch (nominator)
  {
    case -1:
    case I_pimSmSsmRangeAddressType: /* already got it above*/
	       break;
    case I_pimSmSsmRangeAddress:
	       break;
    case I_pimSmSsmRangePrefixLength:
	       break;
    case I_pimSmSsmRangeRowStatus:
         if (snmpPimSmSsmRangeStatusGet(pimSmSsmRangeEntryData.pimSmSsmRangeAddressType, &pimSmSsmRangeAddr,
               pimSmSsmRangeEntryData.pimSmSsmRangePrefixLength, &pimSmSsmRangeEntryData.pimSmSsmRangeRowStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, pimSmSsmRangeEntryData.valid);
         }
	       break;
    default:
	 /*unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmSsmRangeEntryData.valid))
    return(NULL);

  return(&pimSmSsmRangeEntryData);
}

int
k_pimSmSsmRangeEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pimSmSsmRangeEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pimSmSsmRangeEntry_set_defaults(doList_t *dp)
{
    pimSmSsmRangeEntry_t *data = (pimSmSsmRangeEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_pimSmSsmRangeEntry_set(pimSmSsmRangeEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t pimSmSsmRangeAddr;

  /*Convert from OctetString Addr to L7_inet_addr_t */
  if(data->pimSmSsmRangeAddressType == D_pimSmSsmRangeAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->pimSmSsmRangeAddress->octet_ptr, data->pimSmSsmRangeAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmSsmRangeAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->pimSmSsmRangeAddressType == D_pimSmSsmRangeAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->pimSmSsmRangeAddress->octet_ptr, data->pimSmSsmRangeAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmSsmRangeAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimSmSsmRangeRowStatus, data->valid))
  {
    if ( snmpPimSmSsmRangeStatusSet(data->pimSmSsmRangeAddressType,
                                         &pimSmSsmRangeAddr,
                                         data->pimSmSsmRangePrefixLength, data->pimSmSsmRangeRowStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_pimSmSsmRangeRowStatus, data->valid);
      return (COMMIT_FAILED_ERROR);
    }
  }

   return NO_ERROR;
}
#endif
pimStaticRPEntry_t *
k_pimStaticRPEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pimStaticRPAddressType,
                       OctetString * pimStaticRPGrpAddress,
                       SR_UINT32 pimStaticRPGrpPrefixLength)
{
  static pimStaticRPEntry_t pimStaticRPEntryData;
  L7_inet_addr_t pimStaticRPGrpAddr, pimStaticRPRPAddr;
  static L7_BOOL initializeMem = L7_TRUE;

  L7_char8 temp_length;

   L7_in_addr_t ipv4_addr;
   L7_in6_addr_t ipv6_addr;
  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimStaticRPEntryData.pimStaticRPRPAddress = MakeOctetString(NULL, 0);
    pimStaticRPEntryData.pimStaticRPGrpAddress = MakeOctetString(NULL, 0);
  }
  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  inetAddressZeroSet(pimStaticRPAddressType, &pimStaticRPGrpAddr);
  inetAddressZeroSet(pimStaticRPAddressType, &pimStaticRPRPAddr);

  /* Clear all the bits*/
  ZERO_VALID(pimStaticRPEntryData.valid);
  pimStaticRPEntryData.pimStaticRPAddressType = pimStaticRPAddressType;
  (void)SafeMakeOctetString(&pimStaticRPEntryData.pimStaticRPGrpAddress, NULL, 0);
  pimStaticRPEntryData.pimStaticRPGrpPrefixLength = pimStaticRPGrpPrefixLength;
  SET_VALID(I_pimStaticRPAddressType, pimStaticRPEntryData.valid);
  SET_VALID(I_pimStaticRPGrpPrefixLength, pimStaticRPEntryData.valid);
  SET_VALID(I_pimStaticRPGrpAddress, pimStaticRPEntryData.valid);

  if(pimStaticRPEntryData.pimStaticRPAddressType == L7_NULL) {
      pimStaticRPEntryData.pimStaticRPAddressType = D_pimStaticRPAddressType_ipv4;
  }
  if (pimStaticRPEntryData.pimStaticRPAddressType == D_pimStaticRPAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    if (pimStaticRPGrpAddress->length == sizeof(L7_uint32)) {
        ipAddr = OctetStringToIP(pimStaticRPGrpAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimStaticRPGrpAddr))
        return(NULL);
  }
  if(pimStaticRPEntryData.pimStaticRPAddressType == D_pimStaticRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    if (pimStaticRPGrpAddress->length == sizeof(L7_in6_addr_t)) {
        OctetStringToIP6(pimStaticRPGrpAddress, ipAddr.in6.addr8);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimStaticRPGrpAddr))
        return(NULL);
  }

  temp_length = pimStaticRPEntryData.pimStaticRPGrpPrefixLength;
  if((searchType == EXACT) ?
    (usmDbPimsmStaticRPEntryGet(pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, pimStaticRPEntryData.pimStaticRPGrpPrefixLength) != L7_SUCCESS) :
    ((usmDbPimsmStaticRPEntryGet(pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, pimStaticRPEntryData.pimStaticRPGrpPrefixLength) != L7_SUCCESS) &&
    (usmDbPimsmStaticRPEntryNextGet((L7_uchar8)pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, &temp_length) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimStaticRPEntryData.pimStaticRPAddressType == D_pimStaticRPAddressType_ipv6)
      {
        return(NULL);
      }
      else if(pimStaticRPEntryData.pimStaticRPAddressType == D_pimStaticRPAddressType_ipv4)
      {
        memset(&pimStaticRPGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimStaticRPEntryData.pimStaticRPAddressType = L7_AF_INET6;
        pimStaticRPGrpAddr.family = L7_AF_INET6;
        temp_length = 0;
        if((usmDbPimsmStaticRPEntryNextGet((L7_uchar8)pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, &temp_length) != L7_SUCCESS))
          return(NULL);
      }
    }
    else {
      return(NULL);
    }
  }
  pimStaticRPEntryData.pimStaticRPGrpPrefixLength = temp_length;

   if(pimStaticRPEntryData.pimStaticRPAddressType == L7_AF_INET)
  {
    if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStaticRPGrpAddr, &ipv4_addr.s_addr) &&
       SafeMakeOctetString(&pimStaticRPEntryData.pimStaticRPGrpAddress,
                           (L7_uchar8 *)&ipv4_addr.s_addr, sizeof(ipv4_addr.s_addr)) == L7_TRUE )
    {
      SET_VALID(I_pimStaticRPGrpAddress, pimStaticRPEntryData.valid);
    }
    else {
      return(NULL);
    }
  }
  else if(pimStaticRPEntryData.pimStaticRPAddressType == L7_AF_INET6)
  { 
    if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStaticRPGrpAddr, &ipv6_addr) &&
       SafeMakeOctetString(&pimStaticRPEntryData.pimStaticRPGrpAddress,
                           ipv6_addr.in6.addr8, sizeof(ipv6_addr.in6.addr8)) == L7_TRUE )
    {
      SET_VALID(I_pimStaticRPGrpAddress, pimStaticRPEntryData.valid);
    }
    else
    {
      return(NULL);
    }
  }

  SET_VALID(I_pimStaticRPGrpAddress, pimStaticRPEntryData.valid);
  SET_VALID(I_pimStaticRPAddressType, pimStaticRPEntryData.valid);
  SET_VALID(I_pimStaticRPGrpPrefixLength, pimStaticRPEntryData.valid);
  switch (nominator)
  {
    case -1:
    case I_pimStaticRPAddressType: /* already got it above*/
	       break;
    case I_pimStaticRPGrpAddress:
	       break;
    case I_pimStaticRPGrpPrefixLength:
	       break;
    case I_pimStaticRPRPAddress:
         if(usmDbPimsmStaticRPAddrGet(pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, 
                           pimStaticRPEntryData.pimStaticRPGrpPrefixLength, &pimStaticRPRPAddr) == L7_SUCCESS)
         {
             if(pimStaticRPEntryData.pimStaticRPAddressType == D_pimSGAddressType_ipv4)
             {
               L7_uint32 ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimStaticRPRPAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimStaticRPEntryData.pimStaticRPRPAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimStaticRPEntryData.valid);
                 }
               }
             }
             else if(pimStaticRPEntryData.pimStaticRPAddressType == D_pimSGAddressType_ipv6)
             {
               L7_in6_addr_t ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimStaticRPRPAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimStaticRPEntryData.pimStaticRPRPAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimStaticRPEntryData.valid);
                 }
               }
             }
           SET_VALID(I_pimStaticRPRPAddress, pimStaticRPEntryData.valid);
         }
         break;
    case I_pimStaticRPPimMode:
         if(usmDbPimsmStaticRPPimModeGet(pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, 
               pimStaticRPEntryData.pimStaticRPGrpPrefixLength, &pimStaticRPEntryData.pimStaticRPPimMode) != L7_SUCCESS)
         {
           pimStaticRPEntryData.pimStaticRPPimMode = D_pimStaticRPPimMode_asm;
         }
         SET_VALID(nominator, pimStaticRPEntryData.valid);
         break;
    case I_pimStaticRPOverrideDynamic:
         if(usmDbPimsmStaticRPOverrideDynamicGet(pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr, 
                           pimStaticRPEntryData.pimStaticRPGrpPrefixLength, &pimStaticRPEntryData.pimStaticRPOverrideDynamic) == L7_SUCCESS)
         {
           if (pimStaticRPEntryData.pimStaticRPOverrideDynamic != L7_TRUE)
           {
             pimStaticRPEntryData.pimStaticRPOverrideDynamic = D_pimStaticRPOverrideDynamic_false;
           }
           SET_VALID(nominator, pimStaticRPEntryData.valid);
         }
         break;
    case I_pimStaticRPRowStatus:
         if (snmpPimSmStaticRPStatusGet(pimStaticRPEntryData.pimStaticRPAddressType, &pimStaticRPGrpAddr,
               pimStaticRPEntryData.pimStaticRPGrpPrefixLength, &pimStaticRPEntryData.pimStaticRPRowStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, pimStaticRPEntryData.valid);
         }
	 break;
    default:
	 /*unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimStaticRPEntryData.valid))
    return(NULL);

   return(&pimStaticRPEntryData);
}

int
k_pimStaticRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pimStaticRPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pimStaticRPEntry_set_defaults(doList_t *dp)
{
    pimStaticRPEntry_t *data = (pimStaticRPEntry_t *) (dp->data);

    if ((data->pimStaticRPRPAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->pimStaticRPPimMode = D_pimStaticRPPimMode_asm;
    data->pimStaticRPOverrideDynamic = D_pimStaticRPOverrideDynamic_false;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_pimStaticRPEntry_set(pimStaticRPEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t pimStaticRPGrpAddr, pimStaticRPRPAddr;
  L7_uint32 overRide = L7_FALSE;
  
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
      return(COMMIT_FAILED_ERROR);

  if(data->pimStaticRPAddressType == D_pimStaticRPAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    ipAddr = OctetStringToIP(data->pimStaticRPGrpAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimStaticRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    ipAddr = OctetStringToIP(data->pimStaticRPRPAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimStaticRPRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->pimStaticRPAddressType == D_pimStaticRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    OctetStringToIP6( data->pimStaticRPGrpAddress, (L7_char8*)&ipAddr);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimStaticRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    OctetStringToIP6( data->pimStaticRPRPAddress, (L7_char8*)&ipAddr);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimStaticRPRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimStaticRPRowStatus, data->valid))
  {
    if (snmpPimSmStaticRPStatusSet(data->pimStaticRPAddressType, 
                                       pimStaticRPRPAddr,
                                       pimStaticRPGrpAddr,
                                       data->pimStaticRPGrpPrefixLength, 
                                       data->pimStaticRPOverrideDynamic,
                                       data->pimStaticRPRowStatus) != L7_SUCCESS )
    {
 	    CLR_VALID(I_pimStaticRPRowStatus, data->valid);
	    return (COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimStaticRPRPAddress, data->valid) )
  {
    if(usmDbPimsmStaticRPAddrSet(data->pimStaticRPAddressType, 
                                         &pimStaticRPGrpAddr, 
                                         data->pimStaticRPGrpPrefixLength,
                                         &pimStaticRPRPAddr) != L7_SUCCESS)
    {
   
	    return(COMMIT_FAILED_ERROR);
    }
  }
#ifdef NOT_SUPPORTED
  if (VALID(I_pimStaticRPPimMode, data->valid))
  {
    if (usmDbPimsmStaticRPPimModeSet(data->pimStaticRPAddressType,
                                           &pimStaticRPGrpAddr, 
                                           data->pimStaticRPGrpPrefixLength,
	                                         data->pimStaticRPPimMode) != L7_SUCCESS)
    {
	    return(COMMIT_FAILED_ERROR);
    }
  }
#endif
  if (VALID(I_pimStaticRPOverrideDynamic, data->valid))
  {
    if (data->pimStaticRPOverrideDynamic == D_pimStaticRPOverrideDynamic_true)
    {
       overRide = L7_TRUE;
    }
    if (usmDbPimsmStaticRPOverrideDynamicSet(data->pimStaticRPAddressType,
                                                   &pimStaticRPGrpAddr, 
                                                   data->pimStaticRPGrpPrefixLength,
 	                                           overRide) != L7_SUCCESS)
    {
	     return(COMMIT_FAILED_ERROR);
    }
  }
  return NO_ERROR;
}

pimGroupMappingEntry_t *
k_pimGroupMappingEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 pimGroupMappingOrigin,
                           SR_INT32 pimGroupMappingAddressType,
                           OctetString * pimGroupMappingGrpAddress,
                           SR_UINT32 pimGroupMappingGrpPrefixLength,
                           SR_INT32 pimGroupMappingRPAddressType,  
                           OctetString * pimGroupMappingRPAddress)
{
  static pimGroupMappingEntry_t pimGroupMappingEntryData;
  L7_inet_addr_t pimMappingGrpAddr, pimMappingRPAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;
  L7_RC_t rc1 = L7_SUCCESS, rc2 = L7_SUCCESS;
  L7_uchar8 OriginV4 = 0, OriginV6 = 0, LengthV4 = 0, LengthV6 = 0;
  L7_inet_addr_t pimMappingGrpV4Addr, pimMappingGrpV6Addr, pimMappingRPV4Addr, pimMappingRPV6Addr;
  L7_uchar8 familyType = L7_NULL; 

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimGroupMappingEntryData.pimGroupMappingGrpAddress = MakeOctetString(NULL, 0);
    pimGroupMappingEntryData.pimGroupMappingRPAddress = MakeOctetString(NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimGroupMappingAddressType != L7_NULL) &&
     (pimGroupMappingAddressType != L7_AF_INET) &&
     (pimGroupMappingAddressType != L7_AF_INET6))
    return(NULL);


  /* Clear all the bits*/
  ZERO_VALID(pimGroupMappingEntryData.valid);
  pimGroupMappingEntryData.pimGroupMappingOrigin = pimGroupMappingOrigin;
  pimGroupMappingEntryData.pimGroupMappingAddressType = pimGroupMappingAddressType;
  (void)SafeMakeOctetString(&pimGroupMappingEntryData.pimGroupMappingGrpAddress, NULL, 0);
  pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength = pimGroupMappingGrpPrefixLength;
  (void)SafeMakeOctetString(&pimGroupMappingEntryData.pimGroupMappingRPAddress, NULL, 0);
  
  SET_VALID(I_pimGroupMappingOrigin, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingAddressType, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingGrpAddress, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingGrpPrefixLength, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingRPAddress, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingRPAddressType, pimGroupMappingEntryData.valid);

  if(pimGroupMappingAddressType == L7_NULL)
    pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET;
  if(pimGroupMappingEntryData.pimGroupMappingAddressType == D_pimGroupMappingAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;
    L7_in6_addr_t ipAddr3;

    memset(&ipAddr3, L7_NULL, sizeof(L7_in6_addr_t));
    inetAddressZeroSet(L7_AF_INET, &pimMappingGrpV4Addr);
    inetAddressZeroSet(L7_AF_INET, &pimMappingRPV4Addr);
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {

      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if(firstTime != L7_TRUE) 
    {
      ipAddr1 = OctetStringToIP(pimGroupMappingGrpAddress);
      ipAddr2 = OctetStringToIP(pimGroupMappingRPAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimMappingGrpV4Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimMappingRPV4Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET;
    familyType=L7_AF_INET;
    OriginV4 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingOrigin;
    LengthV4 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength;

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr3, &pimMappingGrpV6Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr3, &pimMappingRPV6Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    OriginV6 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingOrigin;
    LengthV6 = 0;
  }
  else if(pimGroupMappingEntryData.pimGroupMappingAddressType == D_pimGroupMappingAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;
    L7_uint32 ipAddr3 = 0;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    
    inetAddressZeroSet(L7_AF_INET6, &pimMappingGrpV6Addr);
    inetAddressZeroSet(L7_AF_INET6, &pimMappingRPV6Addr);

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if(firstTime != L7_TRUE) 
    {
      OctetStringToIP6(pimGroupMappingGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimGroupMappingRPAddress, (L7_char8*)&ipAddr2);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimMappingGrpV6Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimMappingRPV6Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET6;
    familyType=L7_AF_INET6;
    OriginV6 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingOrigin;
    LengthV6 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength;

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr3, &pimMappingGrpV4Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr3, &pimMappingRPV4Addr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    OriginV4 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingOrigin;
    LengthV4 = 0;
  }

  if(searchType == EXACT)
  {
    if((pimGroupMappingAddressType != D_pimGroupMappingAddressType_ipv4) &&
        (pimGroupMappingAddressType != D_pimGroupMappingAddressType_ipv6))
    {
      return(NULL);
    }
    if(usmDbPimsmGroupMappingEntryGet(pimGroupMappingEntryData.pimGroupMappingOrigin, 
                                            pimGroupMappingEntryData.pimGroupMappingAddressType, 
                                            &pimMappingGrpAddr, 
                                            pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength, 
                                            &pimMappingRPAddr) != L7_SUCCESS)
    {
      return(NULL);
    }
  }
  else if(searchType == NEXT)
  {
    if(pimGroupMappingAddressType >= D_pimGroupMappingAddressType_ipv6)
    {
      OriginV4 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingOrigin + 1;
    }
    else if(pimGroupMappingAddressType <= D_pimGroupMappingAddressType_ipv6)
    {
      OriginV4 = (L7_uchar8)pimGroupMappingEntryData.pimGroupMappingOrigin;
    }
    pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET;
    if((usmDbPimsmGroupMappingEntryGet(OriginV4,
                                            pimGroupMappingEntryData.pimGroupMappingAddressType,
                                            &pimMappingGrpV4Addr,
                                            LengthV4,
                                            &pimMappingRPV4Addr) != L7_SUCCESS) && 
       (usmDbPimsmGroupMappingEntryNextGet(&OriginV4,
                                               familyType,
                                             &pimMappingGrpV4Addr,
                                             &LengthV4,
                                            &pimMappingRPV4Addr) != L7_SUCCESS))
    {
      rc1 = L7_FAILURE;
    }

    familyType = L7_AF_INET6;
    if((usmDbPimsmGroupMappingEntryGet(OriginV6,
                                            familyType,
                                            &pimMappingGrpV6Addr,
                                            LengthV6,
                                            &pimMappingRPV6Addr) != L7_SUCCESS) && 
       (usmDbPimsmGroupMappingEntryNextGet(&OriginV6,
                                            familyType,
                                            &pimMappingGrpV6Addr,
                                             &LengthV6,
                                            &pimMappingRPV6Addr) != L7_SUCCESS))
    {
      rc2 = L7_FAILURE;
    }

    if((rc1 == L7_SUCCESS) && (rc2 == L7_FAILURE))
    {
      pimGroupMappingEntryData.pimGroupMappingOrigin = OriginV4;
      pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength = LengthV4;
      pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET;
      inetAddressZeroSet(L7_AF_INET, &pimMappingGrpAddr);
      inetAddressZeroSet(L7_AF_INET, &pimMappingRPAddr);
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_SUCCESS))
    {
      pimGroupMappingEntryData.pimGroupMappingOrigin = OriginV6;
      pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength = LengthV6;
      pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET6;
      inetAddressZeroSet(L7_AF_INET6, &pimMappingGrpAddr);
      inetAddressZeroSet(L7_AF_INET6, &pimMappingRPAddr);
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);
    }
    else if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
    {
      L7_int32 ret;

      ret = pimGrpMappingTableIndexCompare(OriginV4, OriginV6);

      if(ret == L7_AF_INET)
      {
        pimGroupMappingEntryData.pimGroupMappingOrigin = OriginV4;
        pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength = LengthV4;
        pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET;
        inetAddressZeroSet(L7_AF_INET, &pimMappingGrpAddr);
        inetAddressZeroSet(L7_AF_INET, &pimMappingRPAddr);
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
      }
      if(ret == L7_AF_INET6)
      {
        pimGroupMappingEntryData.pimGroupMappingOrigin = OriginV6;
        pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength = LengthV6;
        pimGroupMappingEntryData.pimGroupMappingAddressType = L7_AF_INET6;
        inetAddressZeroSet(L7_AF_INET6, &pimMappingGrpAddr);
        inetAddressZeroSet(L7_AF_INET6, &pimMappingRPAddr);
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);
      }
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_FAILURE))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
  }
  SET_VALID(I_pimGroupMappingOrigin, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingAddressType, pimGroupMappingEntryData.valid);
  SET_VALID(I_pimGroupMappingGrpPrefixLength, pimGroupMappingEntryData.valid);
  /*  converting the IP Address to Octet String */
  if(pimGroupMappingEntryData.pimGroupMappingAddressType == D_pimGroupMappingAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimGroupMappingEntryData.pimGroupMappingGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimGroupMappingGrpAddress, pimGroupMappingEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimGroupMappingEntryData.pimGroupMappingRPAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimGroupMappingRPAddress, pimGroupMappingEntryData.valid);
      pimGroupMappingEntryData.pimGroupMappingRPAddressType  = D_pimGroupMappingAddressType_ipv4;
      SET_VALID(I_pimGroupMappingRPAddressType, pimGroupMappingEntryData.valid);     
    }
  }
  else if(pimGroupMappingEntryData.pimGroupMappingAddressType == D_pimGroupMappingAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimGroupMappingEntryData.pimGroupMappingGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimGroupMappingGrpAddress, pimGroupMappingEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimGroupMappingEntryData.pimGroupMappingRPAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimGroupMappingRPAddress, pimGroupMappingEntryData.valid);
      pimGroupMappingEntryData.pimGroupMappingRPAddressType  = D_pimGroupMappingAddressType_ipv6;
      SET_VALID(I_pimGroupMappingRPAddressType, pimGroupMappingEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimGroupMappingOrigin: /* already got it above*/
	 break;
    case I_pimGroupMappingAddressType:
	 break;
    case I_pimGroupMappingGrpAddress:
	 break;
    case I_pimGroupMappingGrpPrefixLength:
	 break;
    case I_pimGroupMappingRPAddressType:
	 break;
    case I_pimGroupMappingRPAddress:
	 break;
    case I_pimGroupMappingPimMode:
         if(usmDbPimsmGroupMappingPimModeGet(pimGroupMappingEntryData.pimGroupMappingOrigin, pimGroupMappingEntryData.pimGroupMappingAddressType, &pimMappingGrpAddr, pimGroupMappingEntryData.pimGroupMappingGrpPrefixLength, &pimMappingRPAddr,
                           &pimGroupMappingEntryData.pimGroupMappingPimMode) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimGroupMappingEntryData.valid);
         }
	 break;
    case I_pimGroupMappingPrecedence:
	 break;
    default:
	 /*unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimGroupMappingEntryData.valid))
  {
    firstTime = L7_TRUE;
    return(NULL);
   }
   return(&pimGroupMappingEntryData);
}



