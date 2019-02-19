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
#include "k_mib_pimsm_api.h"
#include <inst_lib.h>
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pimsm_api.h"
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

pimSmInterfaceEntry_t *
k_pimSmInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_INT32 pimSmInterfaceIfIndex,
    SR_INT32 pimSmInterfaceIPVersion)
{
  static pimSmInterfaceEntry_t pimSmInterfaceEntryData;
  L7_inet_addr_t pimIntfAddr, pimSmInterfaceNetMask;
  L7_uint32  intIfNum = 0;
  static L7_BOOL firstTime = L7_TRUE;


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);


  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    pimSmInterfaceEntryData.pimSmInterfaceAddress = MakeOctetString(NULL, 0);
    pimSmInterfaceEntryData.pimSmInterfaceDR = MakeOctetString(NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimSmInterfaceEntryData.valid);
  pimSmInterfaceEntryData.pimSmInterfaceIfIndex = pimSmInterfaceIfIndex;
  pimSmInterfaceEntryData.pimSmInterfaceIPVersion = pimSmInterfaceIPVersion;
  SET_VALID(I_pimSmInterfaceIfIndex, pimSmInterfaceEntryData.valid);
  SET_VALID(I_pimSmInterfaceIPVersion, pimSmInterfaceEntryData.valid);


  if(searchType == EXACT)
  {
    if (snmpPimSmInterfaceProtocolGet(pimSmInterfaceIfIndex, pimSmInterfaceIPVersion) != L7_SUCCESS)
    {
      ZERO_VALID(pimSmInterfaceEntryData.valid);
      return(NULL);
    }
  }
  else if(searchType == NEXT)
  {
    if (snmpPimSmInterfaceProtocolGet(pimSmInterfaceIfIndex, pimSmInterfaceIPVersion) != L7_SUCCESS)
    {
      if(snmpPimSmInterfaceProtocolNextGet(pimSmInterfaceIfIndex, pimSmInterfaceIPVersion,
                                           &pimSmInterfaceIfIndex, &pimSmInterfaceIPVersion )
                                           != L7_SUCCESS)
      {
        ZERO_VALID(pimSmInterfaceEntryData.valid);
        return(NULL);
      }
    }
  }

  if (usmDbIntIfNumFromExtIfNum(pimSmInterfaceIfIndex, &intIfNum) != L7_SUCCESS)
  {
     ZERO_VALID(pimSmInterfaceEntryData.valid);
     return(NULL);
  }

  pimSmInterfaceEntryData.pimSmInterfaceIfIndex = pimSmInterfaceIfIndex;
  pimSmInterfaceEntryData.pimSmInterfaceIPVersion = pimSmInterfaceIPVersion;

  SET_VALID(I_pimSmInterfaceIfIndex, pimSmInterfaceEntryData.valid);
  SET_VALID(I_pimSmInterfaceIPVersion, pimSmInterfaceEntryData.valid);
  switch (nominator)
  {
    case -1:
    case I_pimSmInterfaceIfIndex: /* already got it above*/
        break;
    case I_pimSmInterfaceIPVersion:
        break;
    case I_pimSmInterfaceAddressType:
         pimSmInterfaceEntryData.pimSmInterfaceAddressType = pimSmInterfaceEntryData.pimSmInterfaceIPVersion;
         SET_VALID(nominator, pimSmInterfaceEntryData.valid);
        break;
    case I_pimSmInterfaceAddress:
         if(usmDbPimsmInterfaceIPAddressGet(USMDB_UNIT_CURRENT, pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
                                           intIfNum,&pimIntfAddr) == L7_SUCCESS)
         {
           if(pimSmInterfaceEntryData.pimSmInterfaceIPVersion == D_pimSmInterfaceIPVersion_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmInterfaceEntryData.pimSmInterfaceAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmInterfaceEntryData.valid);
               }
             }
           }
           else if(pimSmInterfaceEntryData.pimSmInterfaceIPVersion == D_pimSmInterfaceIPVersion_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmInterfaceEntryData.pimSmInterfaceAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmInterfaceEntryData.valid);
               }
             }
           }
         }
         break;
    case I_pimSmInterfaceNetMaskLength:
         if(usmDbPimsmInterfaceNetMaskGet(USMDB_UNIT_CURRENT, pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
                                             intIfNum,&pimSmInterfaceNetMask) == L7_SUCCESS )
         {
           L7_uchar8 maskLen = L7_NULL;

           if(L7_SUCCESS == inetMaskToMaskLen(&pimSmInterfaceNetMask, &maskLen))
           {
             pimSmInterfaceEntryData.pimSmInterfaceNetMaskLength = maskLen;
             SET_VALID(nominator, pimSmInterfaceEntryData.valid);
           }
         }
       	 break;
    case I_pimSmInterfaceDR:
         if(usmDbPimsmInterfaceDRGet(USMDB_UNIT_CURRENT,  pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
                                        intIfNum, &pimIntfAddr) == L7_SUCCESS)
         {
           if(pimSmInterfaceEntryData.pimSmInterfaceIPVersion == D_pimSmInterfaceIPVersion_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmInterfaceEntryData.pimSmInterfaceDR,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmInterfaceEntryData.valid);
               }
             }
           }
           else if(pimSmInterfaceEntryData.pimSmInterfaceIPVersion == D_pimSmInterfaceIPVersion_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimIntfAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmInterfaceEntryData.pimSmInterfaceDR,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmInterfaceEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmInterfaceHelloInterval:
         if(usmDbPimsmInterfaceHelloIntervalGet(USMDB_UNIT_CURRENT, pimSmInterfaceEntryData.pimSmInterfaceIPVersion ,
             intIfNum,&pimSmInterfaceEntryData.pimSmInterfaceHelloInterval) == L7_SUCCESS )
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
        break;
    case I_pimSmInterfaceTrigHelloInterval:
        break;
    case I_pimSmInterfaceJoinPruneInterval:
         if (usmDbPimsmInterfaceJoinPruneIntervalGet(USMDB_UNIT_CURRENT,
                    pimSmInterfaceEntryData.pimSmInterfaceIPVersion, intIfNum,
                    &pimSmInterfaceEntryData.pimSmInterfaceJoinPruneInterval) == L7_SUCCESS )
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
        break;
    case I_pimSmInterfaceDFElectionRobustness:
        break;
    case I_pimSmInterfaceHelloHoldtime:
        break;
    case I_pimSmInterfaceJoinPruneHoldtime:
        break;
    case I_pimSmInterfaceUseLanPruneDelay:
        break;
    case I_pimSmInterfacePropagationDelay:
        break;
    case I_pimSmInterfaceOverrideInterval:
        break;
    case I_pimSmInterfaceUseGenerationID:
        break;
    case I_pimSmInterfaceGenerationIDValue:
         if(usmDbPimsmInterfaceGenerationIDValueGet(intIfNum, pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
               &pimSmInterfaceEntryData.pimSmInterfaceGenerationIDValue) == L7_SUCCESS )
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
        break;
    case I_pimSmInterfaceUseDRPriority:
      #ifdef NOT_SUPPORTED
         if(usmDbPimsmInterfaceUseDRPriorityGet(intIfNum, pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
               &pimSmInterfaceEntryData.pimSmInterfaceUseDRPriority) == L7_SUCCESS )
      #endif
           pimSmInterfaceEntryData.pimSmInterfaceUseDRPriority = D_pimSmInterfaceUseDRPriority_true;
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
        break;
    case I_pimSmInterfaceDRPriority:
         if(usmDbPimsmInterfaceDRPriorityGet(USMDB_UNIT_CURRENT, pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
              intIfNum,&pimSmInterfaceEntryData.pimSmInterfaceDRPriority) == L7_SUCCESS)
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
       break;
    case I_pimSmInterfaceLanDelayEnabled:
        break;
    case I_pimSmInterfaceEffectPropagDelay:
        break;
    case I_pimSmInterfaceEffectOverrideIvl:
        break;
    case I_pimSmInterfaceSuppressionEnabled:
        break;
    case I_pimSmInterfaceBidirCapable:
        break;
    case I_pimSmInterfaceDRPriorityEnabled:
        break;
    case I_pimSmInterfaceBSRBorder:
         if(usmDbPimsmInterfaceBsrBorderGet(USMDB_UNIT_CURRENT,pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
                       intIfNum,&pimSmInterfaceEntryData.pimSmInterfaceBSRBorder) == L7_SUCCESS)
         {
           if ( pimSmInterfaceEntryData.pimSmInterfaceBSRBorder == 0)
           pimSmInterfaceEntryData.pimSmInterfaceBSRBorder = D_pimSmInterfaceBSRBorder_false;
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
         }
        break;
    case I_pimSmInterfaceStatus:
         if (snmpPimSmInterfaceStatusGet(intIfNum, pimSmInterfaceEntryData.pimSmInterfaceIPVersion,
               &pimSmInterfaceEntryData.pimSmInterfaceStatus) == L7_SUCCESS )
           SET_VALID(nominator, pimSmInterfaceEntryData.valid);
      	 break;

    default:
	/* unknown nominator */
	    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, pimSmInterfaceEntryData.valid))
  {
    return(NULL);
  }

  return(&pimSmInterfaceEntryData);
}

int
k_pimSmInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{
  /* Following line implemented per VLAN mib implementaion...*/
  ZERO_VALID(((pimSmInterfaceEntry_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_pimSmInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pimSmInterfaceEntry_set_defaults(doList_t *dp)
{
  pimSmInterfaceEntry_t *data = (pimSmInterfaceEntry_t *) (dp->data);

  if ((data->pimSmInterfaceAddress = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->pimSmInterfaceDR = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pimSmInterfaceHelloInterval = 30;
  data->pimSmInterfaceTrigHelloInterval = 5;
  data->pimSmInterfaceJoinPruneInterval = 60;
  data->pimSmInterfaceDFElectionRobustness = 3;
  data->pimSmInterfaceHelloHoldtime = 105;
  data->pimSmInterfaceJoinPruneHoldtime = 210;
  data->pimSmInterfaceUseLanPruneDelay = D_pimSmInterfaceUseLanPruneDelay_true;
  data->pimSmInterfacePropagationDelay = 500;
  data->pimSmInterfaceOverrideInterval = 2500;
  data->pimSmInterfaceUseGenerationID = D_pimSmInterfaceUseGenerationID_true;
  data->pimSmInterfaceUseDRPriority = D_pimSmInterfaceUseDRPriority_true;
  data->pimSmInterfaceDRPriority = 1;
  data->pimSmInterfaceBSRBorder = D_pimSmInterfaceBSRBorder_false;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_pimSmInterfaceEntry_set(pimSmInterfaceEntry_t *data,
    ContextInfo *contextInfo, int function)
{
  L7_RC_t rc;
  L7_uint32 pimSmInterfaceIfIndex;

  /* Getting internal interface number */
  rc = usmDbIntIfNumFromExtIfNum(data->pimSmInterfaceIfIndex, &pimSmInterfaceIfIndex);
  data->pimSmInterfaceIfIndex = pimSmInterfaceIfIndex;


  if (VALID(I_pimSmInterfaceStatus, data->valid))
  {
    if ( snmpPimSmInterfaceStatusSet(pimSmInterfaceIfIndex,
                                         data->pimSmInterfaceIPVersion,
                                         data->pimSmInterfaceStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_pimSmInterfaceStatus, data->valid);
      return (COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimSmInterfaceHelloInterval, data->valid))
  {
    if (!((data->pimSmInterfaceHelloInterval >= L7_PIMSM_INTERFACE_HELLO_INTERVAL_MIN) &&
	  (data->pimSmInterfaceHelloInterval <= L7_PIMSM_INTERFACE_HELLO_INTERVAL_MAX)) ||
	   usmDbPimsmInterfaceHelloIntervalSet(USMDB_UNIT_CURRENT, data->pimSmInterfaceIPVersion,
                                                     data->pimSmInterfaceIfIndex,
                                                     data->pimSmInterfaceHelloInterval) != L7_SUCCESS)
    {
      	    return(COMMIT_FAILED_ERROR);
    }
  }


  if (VALID(I_pimSmInterfaceJoinPruneInterval, data->valid))
  {
    if (usmDbPimsmInterfaceJoinPruneIntervalSet(USMDB_UNIT_CURRENT, data->pimSmInterfaceIPVersion,
                                        pimSmInterfaceIfIndex, data->pimSmInterfaceJoinPruneInterval) != L7_SUCCESS)
	    return(COMMIT_FAILED_ERROR);
  }
#ifdef NOT_SUPPORTED
  if (VALID(I_pimSmInterfaceUseDRPriority, data->valid))
  {
    if (usmDbPimsmInterfaceUseDRPrioritySet(pimSmInterfaceIfIndex,
                                                  data->pimSmInterfaceIPVersion,
	                                          data->pimSmInterfaceUseDRPriority) != L7_SUCCESS)
	     return(COMMIT_FAILED_ERROR);
  }
#endif
  if (VALID(I_pimSmInterfaceDRPriority, data->valid))
  {
    if (usmDbPimsmInterfaceDRPrioritySet(USMDB_UNIT_CURRENT, data->pimSmInterfaceIPVersion, 
                                 pimSmInterfaceIfIndex, data->pimSmInterfaceDRPriority) != L7_SUCCESS)
	     return(COMMIT_FAILED_ERROR);
  }
  if (VALID(I_pimSmInterfaceBSRBorder, data->valid))
  {
    if ( data->pimSmInterfaceBSRBorder == D_pimSmInterfaceBSRBorder_false)
     {
      if (usmDbPimsmInterfaceBsrBorderSet(USMDB_UNIT_CURRENT,data->pimSmInterfaceIPVersion, 
                                        pimSmInterfaceIfIndex,L7_DISABLE) != L7_SUCCESS)
	     return(COMMIT_FAILED_ERROR);
     }
    else
      if (usmDbPimsmInterfaceBsrBorderSet(USMDB_UNIT_CURRENT,data->pimSmInterfaceIPVersion,
                                        pimSmInterfaceIfIndex,L7_ENABLE) != L7_SUCCESS)
          return(COMMIT_FAILED_ERROR);
  }
 

  return NO_ERROR;
}

/**********************************************************************
 * PIM-SM NEIGHBOR TABLE IMPLEMENTAION
 *********************************************************************/
pimSmNeighborEntry_t *
k_pimSmNeighborEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pimSmNeighborIfIndex,
                       SR_INT32 pimSmNeighborAddressType,
                       OctetString * pimSmNeighborAddress)
{
  static pimSmNeighborEntry_t pimSmNeighborEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_inet_addr_t pimSmNbrAddr;
  L7_uint32 intIfNumV4, intIfNumV6, intIfNum;
  L7_RC_t rc1 = L7_SUCCESS, rc2 = L7_SUCCESS;
  L7_uint32 pimSmNbrV4Index, pimSmNbrV6Index;
  L7_inet_addr_t pimSmNbrV4Addr, pimSmNbrV6Addr;
  static L7_BOOL initializeMem = L7_TRUE;

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if(initializeMem == L7_TRUE)
  {
    pimSmNeighborEntryData.pimSmNeighborAddress = MakeOctetString(NULL, 0);
    initializeMem = L7_FALSE;
  }

   if(firstTime == L7_TRUE)
   {
     (void)SafeMakeOctetString(&pimSmNeighborEntryData.pimSmNeighborAddress, NULL, 0);
     firstTime = L7_FALSE;
   }

  if((pimSmNeighborAddressType != L7_NULL) &&
     (pimSmNeighborAddressType != L7_AF_INET) &&
     (pimSmNeighborAddressType != L7_AF_INET6))
        return(NULL);



  /* Clear all the bits*/
  ZERO_VALID(pimSmNeighborEntryData.valid);
  pimSmNeighborEntryData.pimSmNeighborIfIndex = pimSmNeighborIfIndex;
  /*pimSmNeighborEntryData.pimSmNeighborAddress = MakeOctetString(NULL, 0);*/
  SET_VALID(I_pimSmNeighborAddressType, pimSmNeighborEntryData.valid);
  SET_VALID(I_pimSmNeighborIfIndex, pimSmNeighborEntryData.valid);

  memset(&pimSmNbrAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if(pimSmNeighborAddressType == L7_NULL)
    pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET;

  if(pimSmNeighborEntryData.pimSmNeighborAddressType == D_pimSmNeighborAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr = 0;
    }
    else if(firstTime != L7_TRUE)
    {
      ipAddr = OctetStringToIP(pimSmNeighborAddress);
    }

    if(inetAddressSet(L7_AF_INET, &ipAddr, &pimSmNbrAddr) != L7_SUCCESS)
    {
        firstTime =  L7_TRUE;
        return(NULL);
    }
  }
  else if(pimSmNeighborEntryData.pimSmNeighborAddressType == D_pimSmNeighborAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if(firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmNeighborAddress, (L7_char8*)&ipAddr);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmNbrAddr))
    {
        firstTime =  L7_TRUE;
        return(NULL);
    }
  }

  if(searchType == EXACT) 
  {
    if((pimSmNeighborAddressType != D_pimSmNeighborAddressType_ipv4) &&
        (pimSmNeighborAddressType != D_pimSmNeighborAddressType_ipv6))
    {
      return(NULL);
    }
    if((usmDbIntIfNumFromExtIfNum(pimSmNeighborEntryData.pimSmNeighborIfIndex, &intIfNum) != L7_SUCCESS) ||
       (usmDbPimsmNeighborEntryGet(USMDB_UNIT_CURRENT,pimSmNeighborAddressType, intIfNum, &pimSmNbrAddr) != L7_SUCCESS))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
  }
  else
  {
    if(pimSmNeighborAddressType >= D_pimSmNeighborAddressType_ipv6)
    {
      pimSmNbrV4Index = pimSmNeighborEntryData.pimSmNeighborIfIndex + 1;
    }
    else if(pimSmNeighborAddressType <= D_pimSmNeighborAddressType_ipv4)
    {
      pimSmNbrV4Index = pimSmNeighborEntryData.pimSmNeighborIfIndex;
    }
    pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET;
    inetCopy(&pimSmNbrV4Addr, &pimSmNbrAddr);
 
  
    if(((usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, pimSmNbrV4Index) != L7_SUCCESS) &&
      (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, pimSmNbrV4Index, &pimSmNbrV4Index) != L7_SUCCESS)) ||
      (usmDbIntIfNumFromExtIfNum(pimSmNbrV4Index, &intIfNumV4) != L7_SUCCESS) ||
      ((usmDbPimsmNeighborEntryGet(USMDB_UNIT_CURRENT, pimSmNeighborEntryData.pimSmNeighborAddressType, 
                       intIfNumV4, &pimSmNbrV4Addr) != L7_SUCCESS) && 
               (usmDbPimsmNeighborEntryNextGet(USMDB_UNIT_CURRENT, pimSmNeighborEntryData.pimSmNeighborAddressType,
                 &intIfNumV4,&pimSmNbrV4Addr) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNumV4, &pimSmNbrV4Index) != L7_SUCCESS))
    {
      rc1 = L7_FAILURE;
    }

    pimSmNbrV6Index = pimSmNeighborEntryData.pimSmNeighborIfIndex;
    pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET6;
    inetCopy(&pimSmNbrV6Addr, &pimSmNbrAddr);

    if(((usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, pimSmNbrV6Index) != L7_SUCCESS) &&
      (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, pimSmNbrV6Index, &pimSmNbrV6Index) != L7_SUCCESS)) ||
      (usmDbIntIfNumFromExtIfNum(pimSmNbrV6Index, &intIfNumV6) != L7_SUCCESS) ||
      ((usmDbPimsmNeighborEntryGet(USMDB_UNIT_CURRENT,pimSmNeighborEntryData.pimSmNeighborAddressType,
           intIfNumV6, &pimSmNbrV6Addr) != L7_SUCCESS) && 
                (usmDbPimsmNeighborEntryNextGet(USMDB_UNIT_CURRENT, pimSmNeighborEntryData.pimSmNeighborAddressType,&intIfNumV6, &pimSmNbrV6Addr) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNumV6, &pimSmNbrV6Index) != L7_SUCCESS))
    {
      rc2 = L7_FAILURE;
    }

    if((rc1 == L7_SUCCESS) && (rc2 == L7_FAILURE))
    {
      pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET;
      pimSmNeighborEntryData.pimSmNeighborIfIndex = pimSmNbrV4Index;
      inetCopy(&pimSmNbrAddr, &pimSmNbrV4Addr);
      intIfNum = intIfNumV4;
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_SUCCESS))
    {
      pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET6;
      pimSmNeighborEntryData.pimSmNeighborIfIndex = pimSmNbrV6Index;
      inetCopy(&pimSmNbrAddr, &pimSmNbrV6Addr);
      intIfNum = intIfNumV6;
    }
    else if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
    {
      L7_int32 ret;

      ret = pimIntfTableIndexCompare(pimSmNbrV4Index, pimSmNbrV6Index);

      if(ret == L7_AF_INET)
      {
        pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET;
        pimSmNeighborEntryData.pimSmNeighborIfIndex = pimSmNbrV4Index;
        inetCopy(&pimSmNbrAddr, &pimSmNbrV4Addr);
        intIfNum = intIfNumV4;
      }
      if(ret == L7_AF_INET6)
      {
        pimSmNeighborEntryData.pimSmNeighborAddressType = L7_AF_INET6;
        pimSmNeighborEntryData.pimSmNeighborIfIndex = pimSmNbrV6Index;
        inetCopy(&pimSmNbrAddr, &pimSmNbrV6Addr);
        intIfNum = intIfNumV6;
      }
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_FAILURE))
    {
      firstTime =  L7_TRUE;
      ZERO_VALID(pimSmNeighborEntryData.valid);
      return(NULL);
    }
  }



  /* copy the new pim Neighbor Address to the outgoing data structure
    by converting the IP Address to Octet String */
  if(pimSmNeighborEntryData.pimSmNeighborAddressType == D_pimSmNeighborAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmNbrAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmNeighborEntryData.pimSmNeighborAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmNeighborAddress, pimSmNeighborEntryData.valid);
    }
  }
  else if(pimSmNeighborEntryData.pimSmNeighborAddressType == D_pimSmNeighborAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmNbrAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmNeighborEntryData.pimSmNeighborAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmNeighborAddress, pimSmNeighborEntryData.valid);
    }
  }



  switch (nominator)
  {
      case -1:
      case I_pimSmNeighborIfIndex: /* already got it above*/
	   break;
      case I_pimSmNeighborAddressType:
	   break;
      case I_pimSmNeighborAddress:
            pimSmNeighborEntryData.pimSmNeighborAddress = pimSmNeighborAddress;
           SET_VALID(I_pimSmNeighborAddress, pimSmNeighborEntryData.valid);
           
	   break;
      case I_pimSmNeighborUpTime:
           if(usmDbPimsmNeighborUpTimeGet(USMDB_UNIT_CURRENT,pimSmNeighborEntryData.pimSmNeighborAddressType,
                             intIfNum, &pimSmNbrAddr, &pimSmNeighborEntryData.pimSmNeighborUpTime) == L7_SUCCESS)
           {
             SET_VALID(nominator, pimSmNeighborEntryData.valid);
           }
	   break;
      case I_pimSmNeighborExpiryTime:
           if(usmDbPimsmNeighborExpiryTimeGet(USMDB_UNIT_CURRENT, pimSmNeighborEntryData.pimSmNeighborAddressType,
                          intIfNum, &pimSmNbrAddr,&pimSmNeighborEntryData.pimSmNeighborExpiryTime) == L7_SUCCESS)
           {
             SET_VALID(nominator, pimSmNeighborEntryData.valid);
           }
           break;
      case I_pimSmNeighborLanPruneDelayPresent:
           break;
      case I_pimSmNeighborPropagationDelay:
           break;
      case I_pimSmNeighborOverrideInterval:
           break;
      case I_pimSmNeighborTBit:
           break;
      case I_pimSmNeighborGenerationIDPresent:
           break;
      case I_pimSmNeighborGenerationIDValue:
           break;
      case I_pimSmNeighborBidirCapable:
           break;
      case I_pimSmNeighborDRPriorityPresent:
           if(usmDbPimsmNeighborDRPriorityPresentGet(intIfNum, pimSmNeighborEntryData.pimSmNeighborAddressType, &pimSmNbrAddr,
                             &pimSmNeighborEntryData.pimSmNeighborDRPriorityPresent) == L7_SUCCESS)
           {
             SET_VALID(nominator, pimSmNeighborEntryData.valid);
           }
	   break;
      case I_pimSmNeighborDRPriority:
           if(usmDbPimsmNeighborDRPriorityGet(USMDB_UNIT_CURRENT,pimSmNeighborEntryData.pimSmNeighborAddressType, 
                                intIfNum, &pimSmNbrAddr,&pimSmNeighborEntryData.pimSmNeighborDRPriority) == L7_SUCCESS )
           {
             SET_VALID(nominator, pimSmNeighborEntryData.valid);
           }
	   break;
      default:
	/* unknown nominator */
	    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmNeighborEntryData.valid))
    return(NULL);

  return(&pimSmNeighborEntryData);
}

pimSmNbrSecAddressEntry_t *
k_pimSmNbrSecAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 pimSmNbrSecAddressIfIndex,
                            SR_INT32 pimSmNbrSecAddressType,
                            OctetString * pimSmNbrSecAddressPrimary,
                            OctetString * pimSmNbrSecAddress)
{
 #ifdef NOT_SUPPORTED
  static pimSmNbrSecAddressEntry_t pimSmNbrSecAddressEntryData;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);


  /* Clear all the bits*/
  ZERO_VALID(pimSmNbrSecAddressEntryData.valid);
  pimSmNbrSecAddressEntryData.pimSmNbrSecAddressIfIndex = pimSmNbrSecAddressIfIndex;
  pimSmNbrSecAddressEntryData.pimSmNbrSecAddressType = pimSmNbrSecAddressType;
  pimSmNbrSecAddressEntryData.pimSmNbrSecAddressPrimary = pimSmNbrSecAddressPrimary;
  pimSmNbrSecAddressEntryData.pimSmNbrSecAddress = pimSmNbrSecAddress;
  SET_VALID(I_pimSmNbrSecAddressIfIndex, pimSmNbrSecAddressEntryData.valid);
  SET_VALID(I_pimSmNbrSecAddressType, pimSmNbrSecAddressEntryData.valid);
  SET_VALID(I_pimSmNbrSecAddressPrimary, pimSmNbrSecAddressEntryData.valid);
  SET_VALID(I_pimSmNbrSecAddress, pimSmNbrSecAddressEntryData.valid);


    switch (nominator)
    {
      case -1:
      case I_pimSmNbrSecAddressIfIndex: /* already got it above*/
	         break;
      case I_pimSmNbrSecAddressType:
	         break;
      case I_pimSmNbrSecAddressPrimary:
	         break;
      case I_pimSmNbrSecAddress:
	         break;
      default:
	/* unknown nominator */
	    return(NULL);
      return(NULL);

    }

  if (nominator >= 0 && !VALID(nominator, pimSmNbrSecAddressEntryData.valid))
    return(NULL);

   return(&pimSmNbrSecAddressEntryData);
  #else
    return(NULL);
  #endif
}

pimSmStarGEntry_t *
k_pimSmStarGEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 pimSmStarGAddressType,
                    OctetString * pimSmStarGGrpAddress)
{
  static pimSmStarGEntry_t pimSmStarGEntryData;
  L7_inet_addr_t pimSmStarGGrpAddr, pimSmStarGRPAddr, pimSmStarGUpstreamNbr, pimSmStarGRPFNextHop, pimSmStarGRPFRouteAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimSmStarGAddressType != L7_NULL) &&
     (pimSmStarGAddressType != L7_AF_INET) &&
     (pimSmStarGAddressType != L7_AF_INET6))
        return(NULL);


  memset(&pimSmStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmStarGRPAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmStarGUpstreamNbr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmStarGRPFNextHop, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmStarGRPFRouteAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimSmStarGEntryData.pimSmStarGRPAddress = MakeOctetString(NULL, 0);
    pimSmStarGEntryData.pimSmStarGUpstreamNeighbor = MakeOctetString(NULL, 0);
    pimSmStarGEntryData.pimSmStarGRPFNextHop = MakeOctetString(NULL, 0);
    pimSmStarGEntryData.pimSmStarGRPFRouteAddress = MakeOctetString(NULL, 0);
    pimSmStarGEntryData.pimSmStarGGrpAddress = MakeOctetString(NULL, 0);
  }

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPAddress,NULL, 0);
    (void)SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGUpstreamNeighbor, NULL, 0);
    (void)SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPFNextHop, NULL, 0);
    (void)SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPFRouteAddress, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimSmStarGEntryData.valid);
  pimSmStarGEntryData.pimSmStarGAddressType = pimSmStarGAddressType;
  (void)SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGGrpAddress,NULL, 0);
  SET_VALID(I_pimSmStarGAddressType, pimSmStarGEntryData.valid);

  if(pimSmStarGAddressType == L7_NULL)
    pimSmStarGEntryData.pimSmStarGAddressType = L7_AF_INET;

  if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr = 0;
    }
    else if(firstTime != L7_TRUE)
    {
      ipAddr = OctetStringToIP(pimSmStarGGrpAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmStarGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
  }
  else if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if(firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmStarGGrpAddress, (L7_char8*)&ipAddr);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmStarGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
  }


  if((searchType == EXACT) ?
    (usmDbPimsmStarGEntryGet(pimSmStarGEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr) != L7_SUCCESS) :
    ((usmDbPimsmStarGEntryGet(pimSmStarGEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr) != L7_SUCCESS) &&
    (usmDbPimsmStarGEntryNextGet(pimSmStarGEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimSmStarGEntryData.pimSmStarGAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        allocMem = L7_TRUE;
        return(NULL);
      }
      else if(pimSmStarGEntryData.pimSmStarGAddressType == L7_AF_INET)
      {
        memset(&pimSmStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmStarGGrpAddr.family = L7_AF_INET6;
        pimSmStarGEntryData.pimSmStarGAddressType = L7_AF_INET6;
        if((usmDbPimsmStarGEntryGet(pimSmStarGEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr) != L7_SUCCESS) &&
           (usmDbPimsmStarGEntryNextGet(pimSmStarGEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
          return(NULL);
        }
      }
    }
    else
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmStarGGrpAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmStarGGrpAddress, pimSmStarGEntryData.valid);
    }
  }
  else if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmStarGGrpAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmStarGGrpAddress, pimSmStarGEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSmStarGAddressType: /* already got it above*/
        break;
    case I_pimSmStarGGrpAddress:
        break;
    case I_pimSmStarGUpTime:
         if(usmDbPimsmStarGUpTimeGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
         break;
    case I_pimSmStarGPimSmMode:
         if(usmDbPimsmStarGPimModeGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                            &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGPimSmMode) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGRPAddress:
         if(usmDbPimsmStarGRPAddrGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGRPAddr) == L7_SUCCESS)
         {
           if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStarGRPAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
           else if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStarGRPAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmStarGRPOrigin:
         if(usmDbPimsmStarGRPOriginGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPOrigin) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGRPIsLocal:
         if(usmDbPimsmStarGRPIsLocalGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPIsLocal) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGUpstreamJoinState:
         if(usmDbPimsmStarGUpstreamJoinStateGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGUpstreamJoinState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGUpstreamJoinTimer:
         if(usmDbPimsmStarGUpstreamJoinTimerGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGUpstreamJoinTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGUpstreamNeighbor:
         if(usmDbPimsmStarGUpstreamNbrGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGUpstreamNbr) == L7_SUCCESS)
         {
           if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStarGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
           else if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStarGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmStarGRPFIfIndex:
         if(usmDbPimsmStarGRPFIfIndexGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPFIfIndex) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGRPFNextHop:
         if(usmDbPimsmStarGRPFNextHopGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGRPFNextHop) == L7_SUCCESS)
         {
           if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStarGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
           else if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStarGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmStarGRPFRouteProtocol:
         if(usmDbPimsmStarGRPFRouteProtocolGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPFRouteProtocol) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGRPFRouteAddress:
         if(usmDbPimsmStarGRPFRouteAddrGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGRPFRouteAddr) == L7_SUCCESS)
         {
           if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStarGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
           else if(pimSmStarGEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStarGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGEntryData.pimSmStarGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmStarGRPFRoutePrefixLength:
         if(usmDbPimsmStarGRPFRoutePrefixLenGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPFRoutePrefixLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGRPFRouteMetricPref:
         if(usmDbPimsmStarGRPFRouteMetricPrefGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                                           &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPFRouteMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    case I_pimSmStarGRPFRouteMetric:
         if(usmDbPimsmStarGRPFRouteMetricGet(pimSmStarGEntryData.pimSmStarGAddressType, 
                               &pimSmStarGGrpAddr, &pimSmStarGEntryData.pimSmStarGRPFRouteMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGEntryData.valid);
         }
        break;
    default:
	/* unknown nominator */
	    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmStarGEntryData.valid))
    return(NULL);

  return(&pimSmStarGEntryData);
}

pimSmStarGIEntry_t *
k_pimSmStarGIEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 pimSmStarGAddressType,
                     OctetString * pimSmStarGGrpAddress,
                     SR_INT32 pimSmStarGIIfIndex)
{
  static pimSmStarGIEntry_t pimSmStarGIEntryData;
  L7_inet_addr_t pimSmStarGIAssertWinnerAddr, pimSmStarGGrpAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimSmStarGAddressType != L7_NULL) &&
     (pimSmStarGAddressType != L7_AF_INET) &&
     (pimSmStarGAddressType != L7_AF_INET6))
        return(NULL);

  memset(&pimSmStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmStarGIAssertWinnerAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimSmStarGIEntryData.pimSmStarGIAssertWinnerAddress = MakeOctetString(NULL, 0);
    pimSmStarGIEntryData.pimSmStarGGrpAddress = MakeOctetString(NULL, 0);
  }

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimSmStarGIEntryData.pimSmStarGIAssertWinnerAddress,NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimSmStarGIEntryData.valid);
  pimSmStarGIEntryData.pimSmStarGAddressType = pimSmStarGAddressType;
  (void)SafeMakeOctetString(&pimSmStarGIEntryData.pimSmStarGGrpAddress,NULL, 0);
  pimSmStarGIEntryData.pimSmStarGIIfIndex = pimSmStarGIIfIndex;
  SET_VALID(I_pimSmStarGIEntryIndex_pimSmStarGAddressType, pimSmStarGIEntryData.valid);
  SET_VALID(I_pimSmStarGIIfIndex, pimSmStarGIEntryData.valid);


  if(pimSmStarGAddressType == L7_NULL)
    pimSmStarGIEntryData.pimSmStarGAddressType = L7_AF_INET;

  if(pimSmStarGIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr = 0;
    }
    else if (firstTime != L7_TRUE)
    {
      ipAddr = OctetStringToIP(pimSmStarGGrpAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmStarGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    pimSmStarGIEntryData.pimSmStarGAddressType = L7_AF_INET;
  }
  else if(pimSmStarGIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;


    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if (firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmStarGGrpAddress, (L7_char8*)&ipAddr);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmStarGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    pimSmStarGIEntryData.pimSmStarGAddressType = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmStarGIEntryGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr, pimSmStarGIEntryData.pimSmStarGIIfIndex) != L7_SUCCESS) :
    ((usmDbPimsmStarGIEntryGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr, pimSmStarGIEntryData.pimSmStarGIIfIndex) != L7_SUCCESS) &&
    (usmDbPimsmStarGIEntryNextGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr, &pimSmStarGIEntryData.pimSmStarGIIfIndex) != L7_SUCCESS)))
  {

    if(searchType != EXACT)
    {
      if(pimSmStarGIEntryData.pimSmStarGAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        allocMem = L7_TRUE;
        return(NULL);
      }
      else if(pimSmStarGIEntryData.pimSmStarGAddressType == L7_AF_INET)
      {
        memset(&pimSmStarGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmStarGGrpAddr.family = L7_AF_INET6;
        pimSmStarGIEntryData.pimSmStarGAddressType = L7_AF_INET6;
        if((usmDbPimsmStarGIEntryGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr, pimSmStarGIEntryData.pimSmStarGIIfIndex) != L7_SUCCESS) &&
           (usmDbPimsmStarGIEntryNextGet(pimSmStarGIEntryData.pimSmStarGAddressType,&pimSmStarGGrpAddr,&pimSmStarGIEntryData.pimSmStarGIIfIndex) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
          return(NULL);
        }
      }
    }
    else
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSmStarGIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmStarGGrpAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmStarGIEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmStarGIEntryIndex_pimSmStarGGrpAddress, pimSmStarGIEntryData.valid);
    }
  }
  else if(pimSmStarGIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;
    

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmStarGGrpAddr, &ipAddr))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmStarGIEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmStarGIEntryIndex_pimSmStarGGrpAddress, pimSmStarGIEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSmStarGIEntryIndex_pimSmStarGAddressType:
    case I_pimSmStarGIEntryIndex_pimSmStarGGrpAddress:
    case I_pimSmStarGIIfIndex:
        break;
    case I_pimSmStarGIUpTime:
         if(usmDbPimsmStarGIUpTimeGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGILocalMembership:
         if(usmDbPimsmStarGILocalMembershipGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGILocalMembership) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIJoinPruneState:
         if(usmDbPimsmStarGIJoinPruneStateGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIJoinPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIPrunePendingTimer:
         if(usmDbPimsmStarGIPrunePendingTimerGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIPrunePendingTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIJoinExpiryTimer:
         if(usmDbPimsmStarGIJoinExpiryTimerGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIJoinExpiryTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIAssertState:
         if(usmDbPimsmStarGIAssertStateGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIAssertState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIAssertTimer:
         if(usmDbPimsmStarGIAssertTimerGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIAssertTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIAssertWinnerAddress:
         if(usmDbPimsmStarGIAssertWinnerAddrGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIAssertWinnerAddr) == L7_SUCCESS)
         {
           if(pimSmStarGIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStarGIAssertWinnerAddr, &ipAddr))
             {
{
L7_char8 winner[25];
inetAddrHtop(&pimSmStarGIAssertWinnerAddr, winner);
}
               if (SafeMakeOctetString(&pimSmStarGIEntryData.pimSmStarGIAssertWinnerAddress,
                                       (L7_char8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGIEntryData.valid);
               }
             }
           }
           else if(pimSmStarGIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStarGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmStarGIEntryData.pimSmStarGIAssertWinnerAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmStarGIEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmStarGIAssertWinnerMetricPref:
         if(usmDbPimsmStarGIRPFRouteMetricPrefGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIAssertWinnerMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    case I_pimSmStarGIAssertWinnerMetric:
         if(usmDbPimsmStarGIAssertWinnerMetricGet(pimSmStarGIEntryData.pimSmStarGAddressType, &pimSmStarGGrpAddr,
                                            pimSmStarGIEntryData.pimSmStarGIIfIndex, &pimSmStarGIEntryData.pimSmStarGIAssertWinnerMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmStarGIEntryData.valid);
         }
        break;
    default:
	/* unknown nominator */
	    return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, pimSmStarGIEntryData.valid))
    return(NULL);

  return(&pimSmStarGIEntryData);
}

/**********************************************************************
 * PIM-SM SGENTRY TABLE IMPLEMENTAION
 *********************************************************************/
pimSmSGEntry_t *
k_pimSmSGEntry_get(int serialNum, ContextInfo *contextInfo,
                 int nominator,
                 int searchType,
                 SR_INT32 pimSmSGAddressType,
                 OctetString * pimSmSGGrpAddress,
                 OctetString * pimSmSGSrcAddress)
{
  static pimSmSGEntry_t pimSmSGEntryData;
  L7_inet_addr_t pimSmSGGrpAddr, pimSmSGSrcAddr, pimSmSGRPFNextHop, pimSmSGRPRegisterPMBRAddr, pimSmSGRPFRouteAddr, pimSmSGUpstreamNbr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  L7_char8 grp[25], src[25];

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);


  if((pimSmSGAddressType != L7_NULL) &&
     (pimSmSGAddressType != L7_AF_INET) &&
     (pimSmSGAddressType != L7_AF_INET6))
        return(NULL);

  memset(&pimSmSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmSGRPFNextHop, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimSmSGEntryData.pimSmSGRPFNextHop = MakeOctetString(NULL, 0);
    pimSmSGEntryData.pimSmSGGrpAddress = MakeOctetString(NULL, 0);
    pimSmSGEntryData.pimSmSGSrcAddress = MakeOctetString(NULL, 0);
  }

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPFNextHop, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimSmSGEntryData.valid);
  pimSmSGEntryData.pimSmSGAddressType = pimSmSGAddressType;
  (void)SafeMakeOctetString(&pimSmSGEntryData.pimSmSGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSmSGEntryData.pimSmSGSrcAddress, NULL, 0);
  SET_VALID(I_pimSmSGAddressType, pimSmSGEntryData.valid);
  SET_VALID(I_pimSmSGGrpAddress, pimSmSGEntryData.valid);
  SET_VALID(I_pimSmSGSrcAddress, pimSmSGEntryData.valid);

  if(pimSmSGAddressType == L7_NULL)
    pimSmSGEntryData.pimSmSGAddressType = L7_AF_INET;

  if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if (firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(pimSmSGGrpAddress);
      ipAddr2 = OctetStringToIP(pimSmSGSrcAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    pimSmSGEntryData.pimSmSGAddressType = L7_AF_INET;
  }
  else if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if (firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmSGGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimSmSGSrcAddress, (L7_char8*)&ipAddr2);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    pimSmSGEntryData.pimSmSGAddressType = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmSGEntryGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS) :
    ((usmDbPimsmSGEntryGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS) &&
    (usmDbPimsmSGEntryNextGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimSmSGEntryData.pimSmSGAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        allocMem = L7_TRUE;
        return(NULL);
      }
      else if(pimSmSGEntryData.pimSmSGAddressType == L7_AF_INET)
      {
        memset(&pimSmSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&pimSmSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmSGEntryData.pimSmSGAddressType = L7_AF_INET6;
        if((usmDbPimsmSGEntryGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS) &&
           (usmDbPimsmSGEntryNextGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
          return(NULL);
        }
      }
    }
    else
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGGrpAddress, pimSmSGEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGSrcAddress, pimSmSGEntryData.valid);
    }
  }
  else if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGGrpAddress, pimSmSGEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGSrcAddress, pimSmSGEntryData.valid);
    }
  }

inetAddrHtop(&pimSmSGGrpAddr, grp);
inetAddrHtop(&pimSmSGSrcAddr, src);
  switch (nominator)
  {
    case -1:
    case I_pimSmSGAddressType: /* already got it above*/
	       break;
    case I_pimSmSGGrpAddress:
	       break;
    case I_pimSmSGSrcAddress:
	       break;
    case I_pimSmSGUpTime:
         if(usmDbPimsmSGUpTimeGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
         break;
    case I_pimSmSGPimSmMode:
         if(usmDbPimsmSGPimModeGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGPimSmMode) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGUpstreamJoinState:
         if(usmDbPimsmSGUpstreamJoinStateGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGUpstreamJoinState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGUpstreamJoinTimer:
         if(usmDbPimsmSGUpstreamJoinTimerGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGUpstreamJoinTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGUpstreamNeighbor:
         if(usmDbPimsmSGUpstreamNeighborGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGUpstreamNbr) == L7_SUCCESS)
         {
           if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmSGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
           else if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmSGUpstreamNbr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGUpstreamNeighbor,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
         }
       	 break;
    case I_pimSmSGRPFIfIndex:
         if(usmDbPimsmSGRPFIfIndexGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGRPFIfIndex) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGRPFNextHop:
         if(usmDbPimsmSGRPFNextHopGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGRPFNextHop) == L7_SUCCESS)
         {
           if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmSGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
           else if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmSGRPFNextHop, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPFNextHop,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
         }
       	 break;
    case I_pimSmSGRPFRouteProtocol:
         if(usmDbPimsmSGRPFRouteProtocolGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGRPFRouteProtocol) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGRPFRouteAddress:
         if(usmDbPimsmSGRPFRouteAddressGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGRPFRouteAddr) == L7_SUCCESS)
         {
           if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmSGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
           else if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmSGRPFRouteAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPFRouteAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
         }
       	 break;
    case I_pimSmSGRPFRoutePrefixLength:
         if(usmDbPimsmSGRPFRoutePrefixLengthGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGRPFRoutePrefixLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGRPFRouteMetricPref:
         if(usmDbPimsmSGRPFRouteMetricPrefGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGRPFRouteMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGRPFRouteMetric:
         if(usmDbPimsmSGRPFRouteMetricGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGRPFRouteMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGSPTBit:
         if(usmDbPimsmSGSPTBitGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGSPTBit) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
	 break;
    case I_pimSmSGKeepaliveTimer:
         if(usmDbPimsmSGKeepaliveTimerGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGKeepaliveTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGDRRegisterState:
         if(usmDbPimsmSGDRRegisterStateGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGDRRegisterState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGDRRegisterStopTimer:
         if(usmDbPimsmSGDRRegisterStopTimerGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGEntryData.pimSmSGDRRegisterStopTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGEntryData.valid);
         }
       	 break;
    case I_pimSmSGRPRegisterPMBRAddress:
         if(usmDbPimsmSGRPRegisterPMBRAddressGet(pimSmSGEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGRPRegisterPMBRAddr) == L7_SUCCESS)
         {
           if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmSGRPRegisterPMBRAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPRegisterPMBRAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
           else if(pimSmSGEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmSGRPRegisterPMBRAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGEntryData.pimSmSGRPRegisterPMBRAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGEntryData.valid);
               }
             }
           }
         }
       	 break;
    default:
	 /* unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmSGEntryData.valid))
    return(NULL);

  return(&pimSmSGEntryData);
}

pimSmSGIEntry_t *
k_pimSmSGIEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 pimSmSGAddressType,
                  OctetString * pimSmSGGrpAddress,
                  OctetString * pimSmSGSrcAddress,
                  SR_INT32 pimSmSGIIfIndex)
{
  static pimSmSGIEntry_t pimSmSGIEntryData;
  L7_inet_addr_t pimSmSGGrpAddr, pimSmSGSrcAddr, pimSmSGIAssertWinnerAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSmSGIEntryData.pimSmSGGrpAddress = MakeOctetString(NULL, 0);
    pimSmSGIEntryData.pimSmSGSrcAddress = MakeOctetString(NULL, 0);
    pimSmSGIEntryData.pimSmSGIAssertWinnerAddress = MakeOctetString(NULL, 0);
  }


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimSmSGAddressType != L7_NULL) &&
     (pimSmSGAddressType != L7_AF_INET) &&
     (pimSmSGAddressType != L7_AF_INET6))
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(pimSmSGIEntryData.valid);
  pimSmSGIEntryData.pimSmSGAddressType = pimSmSGAddressType;
  (void)SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGSrcAddress, NULL, 0);
  pimSmSGIEntryData.pimSmSGIIfIndex = pimSmSGIIfIndex;
  SET_VALID(I_pimSmSGIEntryIndex_pimSmSGAddressType, pimSmSGIEntryData.valid);
  SET_VALID(I_pimSmSGIEntryIndex_pimSmSGGrpAddress, pimSmSGIEntryData.valid);
  SET_VALID(I_pimSmSGIEntryIndex_pimSmSGSrcAddress, pimSmSGIEntryData.valid);
  SET_VALID(I_pimSmSGIIfIndex, pimSmSGIEntryData.valid);


  memset(&pimSmSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&pimSmSGIAssertWinnerAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGIAssertWinnerAddress, NULL, 0);
  }

  if(pimSmSGAddressType == L7_NULL)
   {
     pimSmSGIEntryData.pimSmSGAddressType = L7_AF_INET;
   }

  if(pimSmSGIEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;
   

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if (firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(pimSmSGGrpAddress);
      ipAddr2 = OctetStringToIP(pimSmSGSrcAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    pimSmSGIEntryData.pimSmSGAddressType = L7_AF_INET;
  }
  else if(pimSmSGIEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
  {

    L7_in6_addr_t ipAddr1, ipAddr2;
    

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if (firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmSGGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimSmSGSrcAddress, (L7_char8*)&ipAddr2);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    pimSmSGIEntryData.pimSmSGAddressType = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmSGIEntryGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr, pimSmSGIEntryData.pimSmSGIIfIndex) != L7_SUCCESS) :
    ((usmDbPimsmSGIEntryGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr, pimSmSGIEntryData.pimSmSGIIfIndex) != L7_SUCCESS) &&
    (usmDbPimsmSGIEntryNextGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr, &pimSmSGIEntryData.pimSmSGIIfIndex) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimSmSGIEntryData.pimSmSGAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        allocMem = L7_TRUE;
        return(NULL);
      }
      else if(pimSmSGIEntryData.pimSmSGAddressType == L7_AF_INET)
      {
        memset(&pimSmSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&pimSmSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmSGIEntryData.pimSmSGAddressType = L7_AF_INET6;
        pimSmSGGrpAddr.family = L7_AF_INET6;
        pimSmSGSrcAddr.family = L7_AF_INET6;
        if((usmDbPimsmSGIEntryGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr, pimSmSGIEntryData.pimSmSGIIfIndex) != L7_SUCCESS) &&
           (usmDbPimsmSGIEntryNextGet(pimSmSGIEntryData.pimSmSGAddressType,&pimSmSGGrpAddr,&pimSmSGSrcAddr, &pimSmSGIEntryData.pimSmSGIIfIndex) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
          return(NULL);
        }
      }
    }
    else
    {

      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimSmSGIEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGIEntryIndex_pimSmSGGrpAddress, pimSmSGIEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGIEntryIndex_pimSmSGSrcAddress, pimSmSGIEntryData.valid);
    }
  }
  else if(pimSmSGIEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;


    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGIEntryIndex_pimSmSGGrpAddress, pimSmSGIEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGIEntryIndex_pimSmSGSrcAddress, pimSmSGIEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSmSGIEntryIndex_pimSmSGAddressType:
    case I_pimSmSGIEntryIndex_pimSmSGGrpAddress:
    case I_pimSmSGIEntryIndex_pimSmSGSrcAddress:
    case I_pimSmSGIIfIndex:
        break;
    case I_pimSmSGIUpTime:
         if(usmDbPimsmSGIUpTimeGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGILocalMembership:
         if(usmDbPimsmSGILocalMembershipGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGILocalMembership) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIJoinPruneState:
         if(usmDbPimsmSGIJoinPruneStateGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIJoinPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIPrunePendingTimer:
         if(usmDbPimsmSGIPrunePendingTimerGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIPrunePendingTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIJoinExpiryTimer:
         if(usmDbPimsmSGIJoinExpiryTimerGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIJoinExpiryTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIAssertState:
         if(usmDbPimsmSGIAssertStateGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIAssertState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIAssertTimer:
         if(usmDbPimsmSGIAssertTimerGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIAssertTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIAssertWinnerAddress:
         if(usmDbPimsmSGIAssertWinnerAddrGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIAssertWinnerAddr) == L7_SUCCESS)
         {
           if(pimSmSGIEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv4)
           {
             L7_uint32 ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmSGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGIAssertWinnerAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGIEntryData.valid);
               }
             }
           }
           else if(pimSmSGIEntryData.pimSmSGAddressType == D_pimSmSGAddressType_ipv6)
           {
             L7_in6_addr_t ipAddr;

             if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmSGIAssertWinnerAddr, &ipAddr))
             {
               if (SafeMakeOctetString(&pimSmSGIEntryData.pimSmSGIAssertWinnerAddress,
                                       (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
               {
                 SET_VALID(nominator, pimSmSGIEntryData.valid);
               }
             }
           }
         }
        break;
    case I_pimSmSGIAssertWinnerMetricPref:
         if(usmDbPimsmSGIRPFRouteMetricPrefGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIAssertWinnerMetricPref) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    case I_pimSmSGIAssertWinnerMetric:
         if(usmDbPimsmSGIAssertWinnerMetricGet(pimSmSGIEntryData.pimSmSGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGIEntryData.pimSmSGIIfIndex, &pimSmSGIEntryData.pimSmSGIAssertWinnerMetric) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGIEntryData.valid);
         }
        break;
    default:
	/* unknown nominator */
	    return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, pimSmSGIEntryData.valid))
    return(NULL);

  return(&pimSmSGIEntryData);
}

/**********************************************************************
 * PIM-SM SGRptENTRY TABLE IMPLEMENTAION
 *********************************************************************/
pimSmSGRptEntry_t *
k_pimSmSGRptEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 pimSmStarGAddressType,
                    OctetString * pimSmStarGGrpAddress,
                    OctetString * pimSmSGRptSrcAddress)
{
  static pimSmSGRptEntry_t pimSmSGRptEntryData;
  L7_inet_addr_t pimSmSGGrpAddr, pimSmSGSrcAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  L7_char8 grp[25], src[25];

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSmSGRptEntryData.pimSmStarGGrpAddress = MakeOctetString(NULL, 0);
    pimSmSGRptEntryData.pimSmSGRptSrcAddress = MakeOctetString(NULL, 0);
  }


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if(pimSmSGRptEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    inetAddressZeroSet (L7_AF_INET, &pimSmSGGrpAddr);
    inetAddressZeroSet (L7_AF_INET, &pimSmSGSrcAddr);
  }
  else if(pimSmSGRptEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    inetAddressZeroSet (L7_AF_INET6, &pimSmSGGrpAddr);
    inetAddressZeroSet (L7_AF_INET6, &pimSmSGSrcAddr);
  }
  else
  {
    return L7_NULLPTR;
  }

  /* Clear all the bits*/
  ZERO_VALID(pimSmSGRptEntryData.valid);
  pimSmSGRptEntryData.pimSmStarGAddressType = pimSmStarGAddressType;
  (void)SafeMakeOctetString(&pimSmSGRptEntryData.pimSmStarGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSmSGRptEntryData.pimSmSGRptSrcAddress, NULL, 0);
  SET_VALID(I_pimSmSGRptEntryIndex_pimSmStarGAddressType, pimSmSGRptEntryData.valid);
  SET_VALID(I_pimSmSGRptEntryIndex_pimSmStarGGrpAddress, pimSmSGRptEntryData.valid);
  SET_VALID(I_pimSmSGRptSrcAddress, pimSmSGRptEntryData.valid);

  if(pimSmStarGAddressType == L7_NULL)
    pimSmSGRptEntryData.pimSmStarGAddressType = L7_AF_INET;

  if(pimSmSGRptEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if(firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(pimSmStarGGrpAddress);
      ipAddr2 = OctetStringToIP(pimSmSGRptSrcAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    pimSmSGRptEntryData.pimSmStarGAddressType = L7_AF_INET;
  }
  else if(pimSmSGRptEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if(firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmStarGGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimSmSGRptSrcAddress, (L7_char8*)&ipAddr2);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    pimSmSGRptEntryData.pimSmStarGAddressType = L7_AF_INET6;
  }
  else
  {
    return L7_NULLPTR;
  }

inetAddrHtop(&pimSmSGGrpAddr, grp);
inetAddrHtop(&pimSmSGSrcAddr, src);
  if((searchType == EXACT) ?
    (usmDbPimsmSGRptEntryGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS) :
    ((usmDbPimsmSGRptEntryGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS) &&
    (usmDbPimsmSGRptEntryNextGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimSmSGRptEntryData.pimSmStarGAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(pimSmSGRptEntryData.pimSmStarGAddressType == L7_AF_INET)
      {
        memset(&pimSmSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&pimSmSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmSGRptEntryData.pimSmStarGAddressType = L7_AF_INET6;
        pimSmSGGrpAddr.family = L7_AF_INET6;
        pimSmSGSrcAddr.family = L7_AF_INET6;
        if((usmDbPimsmSGRptEntryGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS) &&
           (usmDbPimsmSGRptEntryNextGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
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

  /*  converting the IP Address to Octet String */
  if(pimSmSGRptEntryData.pimSmStarGAddressType == D_pimSmSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptEntryIndex_pimSmStarGGrpAddress, pimSmSGRptEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptEntryData.pimSmSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptSrcAddress, pimSmSGRptEntryData.valid);
    }
  }
  else if(pimSmSGRptEntryData.pimSmStarGAddressType == D_pimSmSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptEntryIndex_pimSmStarGGrpAddress, pimSmSGRptEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptEntryData.pimSmSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptSrcAddress, pimSmSGRptEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSmSGRptEntryIndex_pimSmStarGAddressType: /* already got it above*/
	 break;
    case I_pimSmSGRptEntryIndex_pimSmStarGGrpAddress:
	 break;
    case I_pimSmSGRptSrcAddress:
	 break;
    case I_pimSmSGRptUpTime:
         if(usmDbPimsmSGRptUpTimeGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGRptEntryData.pimSmSGRptUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptEntryData.valid);
         }
	 break;
    case I_pimSmSGRptUpstreamPruneState:
         if(usmDbPimsmSGRptUpstreamPruneStateGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGRptEntryData.pimSmSGRptUpstreamPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptEntryData.valid);
         }
	 break;
    case I_pimSmSGRptUpstreamOverrideTimer:
         if(usmDbPimsmSGRptUpstreamOverrideTimerGet(pimSmSGRptEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           &pimSmSGRptEntryData.pimSmSGRptUpstreamOverrideTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptEntryData.valid);
         }
	 break;
    default:
	 /* unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmSGRptEntryData.valid))
    return(NULL);

  return(&pimSmSGRptEntryData);
}

pimSmSGRptIEntry_t *
k_pimSmSGRptIEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 pimSmStarGAddressType,
                     OctetString * pimSmStarGGrpAddress,
                     OctetString * pimSmSGRptSrcAddress,
                     SR_INT32 pimSmSGRptIIfIndex)
{
  static pimSmSGRptIEntry_t pimSmSGRptIEntryData;
  L7_inet_addr_t pimSmSGGrpAddr, pimSmSGSrcAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSmSGRptIEntryData.pimSmStarGGrpAddress = MakeOctetString(NULL, 0);
    pimSmSGRptIEntryData.pimSmSGRptSrcAddress = MakeOctetString(NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(pimSmSGRptIEntryData.valid);
  pimSmSGRptIEntryData.pimSmStarGAddressType = pimSmStarGAddressType;
  (void)SafeMakeOctetString(&pimSmSGRptIEntryData.pimSmStarGGrpAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimSmSGRptIEntryData.pimSmSGRptSrcAddress, NULL, 0);
  pimSmSGRptIEntryData.pimSmSGRptIIfIndex = pimSmSGRptIIfIndex;
  SET_VALID(I_pimSmSGAddressType, pimSmSGRptIEntryData.valid);
  SET_VALID(I_pimSmSGGrpAddress, pimSmSGRptIEntryData.valid);
  SET_VALID(I_pimSmSGRptSrcAddress, pimSmSGRptIEntryData.valid);
  SET_VALID(I_pimSmSGRptIIfIndex, pimSmSGRptIEntryData.valid);

  if(pimSmStarGAddressType == L7_NULL)
    pimSmSGRptIEntryData.pimSmStarGAddressType = L7_AF_INET;

  if(pimSmSGRptIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if(firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(pimSmStarGGrpAddress);
      ipAddr2 = OctetStringToIP(pimSmSGRptSrcAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    pimSmSGRptIEntryData.pimSmStarGAddressType = L7_AF_INET;
  }
  else if(pimSmSGRptIEntryData.pimSmStarGAddressType == D_pimSmStarGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if(firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmStarGGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimSmSGRptSrcAddress, (L7_char8*)&ipAddr2);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &pimSmSGGrpAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &pimSmSGSrcAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    pimSmSGRptIEntryData.pimSmStarGAddressType = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmSGRptIEntryGet(pimSmSGRptIEntryData.pimSmStarGAddressType,&pimSmSGGrpAddr,&pimSmSGSrcAddr,pimSmSGRptIEntryData.pimSmSGRptIIfIndex)!=L7_SUCCESS) :
    ((usmDbPimsmSGRptIEntryGet(pimSmSGRptIEntryData.pimSmStarGAddressType,&pimSmSGGrpAddr,&pimSmSGSrcAddr,pimSmSGRptIEntryData.pimSmSGRptIIfIndex)!=L7_SUCCESS) &&
    (usmDbPimsmSGRptIEntryNextGet(pimSmSGRptIEntryData.pimSmStarGAddressType,&pimSmSGGrpAddr,&pimSmSGSrcAddr,&pimSmSGRptIEntryData.pimSmSGRptIIfIndex)!=L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimSmSGRptIEntryData.pimSmStarGAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(pimSmSGRptIEntryData.pimSmStarGAddressType == L7_AF_INET)
      {
        memset(&pimSmSGGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&pimSmSGSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmSGRptIEntryData.pimSmStarGAddressType = L7_AF_INET6;
        pimSmSGGrpAddr.family = L7_AF_INET6;
        pimSmSGSrcAddr.family = L7_AF_INET6;
        if((usmDbPimsmSGRptIEntryGet(pimSmSGRptIEntryData.pimSmStarGAddressType,&pimSmSGGrpAddr,&pimSmSGSrcAddr,
                                           pimSmSGRptIEntryData.pimSmSGRptIIfIndex)!=L7_SUCCESS) &&
           (usmDbPimsmSGRptIEntryNextGet(pimSmSGRptIEntryData.pimSmStarGAddressType,&pimSmSGGrpAddr,
                                           &pimSmSGSrcAddr,&pimSmSGRptIEntryData.pimSmSGRptIIfIndex) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
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

  /*  converting the IP Address to Octet String */
  if(pimSmSGRptIEntryData.pimSmStarGAddressType == D_pimSmSGAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptIEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptEntryIndex_pimSmStarGGrpAddress, pimSmSGRptIEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptIEntryData.pimSmSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptSrcAddress, pimSmSGRptIEntryData.valid);
    }
  }
  else if(pimSmSGRptIEntryData.pimSmStarGAddressType == D_pimSmSGAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptIEntryData.pimSmStarGGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptEntryIndex_pimSmStarGGrpAddress, pimSmSGRptIEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimSmSGSrcAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmSGRptIEntryData.pimSmSGRptSrcAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmSGRptSrcAddress, pimSmSGRptIEntryData.valid);
    }
  }


  switch (nominator)
  {
    case -1:
    case I_pimSmSGRptIEntryIndex_pimSmStarGAddressType:
    case I_pimSmSGRptIEntryIndex_pimSmStarGGrpAddress:
    case I_pimSmSGRptIEntryIndex_pimSmSGRptSrcAddress:    
    case I_pimSmSGRptIIfIndex:
        break;
    case I_pimSmSGRptIUpTime:
         if(usmDbPimsmSGRptIUpTimeGet(pimSmSGRptIEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGRptIEntryData.pimSmSGRptIIfIndex, &pimSmSGRptIEntryData.pimSmSGRptIUpTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptIEntryData.valid);
         }
        break;
    case I_pimSmSGRptILocalMembership:
         if(usmDbPimsmSGRptILocalMembershipGet(pimSmSGRptIEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGRptIEntryData.pimSmSGRptIIfIndex, &pimSmSGRptIEntryData.pimSmSGRptILocalMembership) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptIEntryData.valid);
         }
        break;
    case I_pimSmSGRptIJoinPruneState:
         if(usmDbPimsmSGRptIJoinPruneStateGet(pimSmSGRptIEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGRptIEntryData.pimSmSGRptIIfIndex, &pimSmSGRptIEntryData.pimSmSGRptIJoinPruneState) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptIEntryData.valid);
         }
        break;
    case I_pimSmSGRptIPrunePendingTimer:
         if(usmDbPimsmSGRptIPrunePendingTimerGet(pimSmSGRptIEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGRptIEntryData.pimSmSGRptIIfIndex, &pimSmSGRptIEntryData.pimSmSGRptIPrunePendingTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptIEntryData.valid);
         }
        break;
    case I_pimSmSGRptIPruneExpiryTimer:
         if(usmDbPimsmSGRptIJoinExpiryTimerGet(pimSmSGRptIEntryData.pimSmStarGAddressType, &pimSmSGGrpAddr, &pimSmSGSrcAddr,
                           pimSmSGRptIEntryData.pimSmSGRptIIfIndex, &pimSmSGRptIEntryData.pimSmSGRptIPruneExpiryTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmSGRptIEntryData.valid);
         }
        break;
    default:
	/* unknown nominator */
	    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmSGRptIEntryData.valid))
    return(NULL);
  return(&pimSmSGRptIEntryData);
}

pimSmBidirDFElectionEntry_t *
k_pimSmBidirDFElectionEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 pimSmBidirDFElectionAddressType,
                              OctetString * pimSmBidirDFElectionRPAddress,
                              SR_INT32 pimSmBidirDFElectionIfIndex)
{

  #ifdef NOT_SUPPORTED

   static pimSmBidirDFElectionEntry_t pimSmBidirDFElectionEntryData;

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

pimSmStaticRPEntry_t *
k_pimSmStaticRPEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pimSmStaticRPAddressType,
                       OctetString * pimSmStaticRPGrpAddress,
                       SR_UINT32 pimSmStaticRPGrpPrefixLength)
{
  static pimSmStaticRPEntry_t pimSmStaticRPEntryData;
  L7_inet_addr_t pimSmStaticRPGrpAddr, pimSmStaticRPRPAddr;
  static L7_BOOL initializeMem = L7_TRUE;

  L7_char8 temp_length;

   L7_in_addr_t ipv4_addr;
   L7_in6_addr_t ipv6_addr;
  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSmStaticRPEntryData.pimSmStaticRPRPAddress = MakeOctetString(NULL, 0);
    pimSmStaticRPEntryData.pimSmStaticRPGrpAddress = MakeOctetString(NULL, 0);
  }
  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  inetAddressZeroSet(pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr);
  inetAddressZeroSet(pimSmStaticRPAddressType, &pimSmStaticRPRPAddr);

  /* Clear all the bits*/
  ZERO_VALID(pimSmStaticRPEntryData.valid);
  pimSmStaticRPEntryData.pimSmStaticRPAddressType = pimSmStaticRPAddressType;
  (void)SafeMakeOctetString(&pimSmStaticRPEntryData.pimSmStaticRPGrpAddress, NULL, 0);
  pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength = pimSmStaticRPGrpPrefixLength;
  SET_VALID(I_pimSmStaticRPAddressType, pimSmStaticRPEntryData.valid);
  SET_VALID(I_pimSmStaticRPGrpPrefixLength, pimSmStaticRPEntryData.valid);
  SET_VALID(I_pimSmStaticRPGrpAddress, pimSmStaticRPEntryData.valid);

  if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == L7_NULL) {
      pimSmStaticRPEntryData.pimSmStaticRPAddressType = D_pimSmStaticRPAddressType_ipv4;
  }
  if (pimSmStaticRPEntryData.pimSmStaticRPAddressType == D_pimSmStaticRPAddressType_ipv4)
  {
    L7_uint32 ipAddr = 0;

    if (pimSmStaticRPGrpAddress->length == sizeof(L7_uint32)) {
        ipAddr = OctetStringToIP(pimSmStaticRPGrpAddress);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmStaticRPGrpAddr))
        return(NULL);
  }
  if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == D_pimSmStaticRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    if (pimSmStaticRPGrpAddress->length == sizeof(L7_in6_addr_t)) {
        OctetStringToIP6(pimSmStaticRPGrpAddress, ipAddr.in6.addr8);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmStaticRPGrpAddr))
        return(NULL);
  }

  temp_length = pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength;
  if((searchType == EXACT) ?
    (usmDbPimsmStaticRPEntryGet(pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength) != L7_SUCCESS) :
    ((usmDbPimsmStaticRPEntryGet(pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength) != L7_SUCCESS) &&
    (usmDbPimsmStaticRPEntryNextGet((L7_uchar8)pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, &temp_length) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == D_pimSmStaticRPAddressType_ipv6)
      {
        return(NULL);
      }
      else if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == D_pimSmStaticRPAddressType_ipv4)
      {
        memset(&pimSmStaticRPGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        pimSmStaticRPEntryData.pimSmStaticRPAddressType = L7_AF_INET6;
        pimSmStaticRPGrpAddr.family = L7_AF_INET6;
        temp_length = 0;
        if((usmDbPimsmStaticRPEntryNextGet((L7_uchar8)pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, &temp_length) != L7_SUCCESS))
          return(NULL);
      }
    }
    else {
      return(NULL);
    }
  }
  pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength = temp_length;

   if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == L7_AF_INET)
  {
    if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStaticRPGrpAddr, &ipv4_addr.s_addr) &&
       SafeMakeOctetString(&pimSmStaticRPEntryData.pimSmStaticRPGrpAddress,
                           (L7_uchar8 *)&ipv4_addr.s_addr, sizeof(ipv4_addr.s_addr)) == L7_TRUE )
    {
      SET_VALID(I_pimSmStaticRPGrpAddress, pimSmStaticRPEntryData.valid);
    }
    else {
      return(NULL);
    }
  }
  else if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == L7_AF_INET6)
  { 
    if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStaticRPGrpAddr, &ipv6_addr) &&
       SafeMakeOctetString(&pimSmStaticRPEntryData.pimSmStaticRPGrpAddress,
                           ipv6_addr.in6.addr8, sizeof(ipv6_addr.in6.addr8)) == L7_TRUE )
    {
      SET_VALID(I_pimSmStaticRPGrpAddress, pimSmStaticRPEntryData.valid);
    }
    else
    {
      return(NULL);
    }
  }

  SET_VALID(I_pimSmStaticRPGrpAddress, pimSmStaticRPEntryData.valid);
  SET_VALID(I_pimSmStaticRPAddressType, pimSmStaticRPEntryData.valid);
  SET_VALID(I_pimSmStaticRPGrpPrefixLength, pimSmStaticRPEntryData.valid);
  switch (nominator)
  {
    case -1:
    case I_pimSmStaticRPAddressType: /* already got it above*/
	       break;
    case I_pimSmStaticRPGrpAddress:
	       break;
    case I_pimSmStaticRPGrpPrefixLength:
	       break;
    case I_pimSmStaticRPRPAddress:
         if(usmDbPimsmStaticRPAddrGet(pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, 
                           pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength, &pimSmStaticRPRPAddr) == L7_SUCCESS)
         {
             if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == D_pimSmSGAddressType_ipv4)
             {
               L7_uint32 ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimSmStaticRPRPAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimSmStaticRPEntryData.pimSmStaticRPRPAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimSmStaticRPEntryData.valid);
                 }
               }
             }
             else if(pimSmStaticRPEntryData.pimSmStaticRPAddressType == D_pimSmSGAddressType_ipv6)
             {
               L7_in6_addr_t ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimSmStaticRPRPAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimSmStaticRPEntryData.pimSmStaticRPRPAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimSmStaticRPEntryData.valid);
                 }
               }
             }
           SET_VALID(I_pimSmStaticRPRPAddress, pimSmStaticRPEntryData.valid);
         }
         break;
    case I_pimSmStaticRPPimSmMode:
         if(usmDbPimsmStaticRPPimModeGet(pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, 
               pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength, &pimSmStaticRPEntryData.pimSmStaticRPPimSmMode) != L7_SUCCESS)
         {
           pimSmStaticRPEntryData.pimSmStaticRPPimSmMode = D_pimSmStaticRPPimSmMode_asm;
         }
         SET_VALID(nominator, pimSmStaticRPEntryData.valid);
         break;
    case I_pimSmStaticRPOverrideDynamic:
         if(usmDbPimsmStaticRPOverrideDynamicGet(pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr, 
                           pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength, &pimSmStaticRPEntryData.pimSmStaticRPOverrideDynamic) == L7_SUCCESS)
         {
           if (pimSmStaticRPEntryData.pimSmStaticRPOverrideDynamic != L7_TRUE)
           {
             pimSmStaticRPEntryData.pimSmStaticRPOverrideDynamic = D_pimSmStaticRPOverrideDynamic_false;
           }
           SET_VALID(nominator, pimSmStaticRPEntryData.valid);
         }
         break;
    case I_pimSmStaticRPRowStatus:
         if (snmpPimSmStaticRPStatusGet(pimSmStaticRPEntryData.pimSmStaticRPAddressType, &pimSmStaticRPGrpAddr,
               pimSmStaticRPEntryData.pimSmStaticRPGrpPrefixLength, &pimSmStaticRPEntryData.pimSmStaticRPRowStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, pimSmStaticRPEntryData.valid);
         }
	 break;
    default:
	 /*unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmStaticRPEntryData.valid))
    return(NULL);

   return(&pimSmStaticRPEntryData);
}

int
k_pimSmStaticRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pimSmStaticRPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pimSmStaticRPEntry_set_defaults(doList_t *dp)
{
    pimSmStaticRPEntry_t *data = (pimSmStaticRPEntry_t *) (dp->data);

    if ((data->pimSmStaticRPRPAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->pimSmStaticRPPimSmMode = D_pimSmStaticRPPimSmMode_asm;
    data->pimSmStaticRPOverrideDynamic = D_pimSmStaticRPOverrideDynamic_false;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_pimSmStaticRPEntry_set(pimSmStaticRPEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t pimSmStaticRPGrpAddr, pimSmStaticRPRPAddr;
  L7_uint32 overRide = L7_FALSE;
  
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
      return(COMMIT_FAILED_ERROR);

  if(data->pimSmStaticRPAddressType == D_pimSmStaticRPAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    ipAddr = OctetStringToIP(data->pimSmStaticRPGrpAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmStaticRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    ipAddr = OctetStringToIP(data->pimSmStaticRPRPAddress);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmStaticRPRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->pimSmStaticRPAddressType == D_pimSmStaticRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    OctetStringToIP6( data->pimSmStaticRPGrpAddress, (L7_char8*)&ipAddr);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &pimSmStaticRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    OctetStringToIP6( data->pimSmStaticRPRPAddress, (L7_char8*)&ipAddr);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &pimSmStaticRPRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimSmStaticRPRowStatus, data->valid))
  {
    if (snmpPimSmStaticRPStatusSet(data->pimSmStaticRPAddressType, 
                                       pimSmStaticRPRPAddr,
                                       pimSmStaticRPGrpAddr,
                                       data->pimSmStaticRPGrpPrefixLength, 
                                       data->pimSmStaticRPOverrideDynamic,
                                       data->pimSmStaticRPRowStatus) != L7_SUCCESS )
    {
 	    CLR_VALID(I_pimSmStaticRPRowStatus, data->valid);
	    return (COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimSmStaticRPRPAddress, data->valid) )
  {
    if(usmDbPimsmStaticRPAddrSet(data->pimSmStaticRPAddressType, 
                                         &pimSmStaticRPGrpAddr, 
                                         data->pimSmStaticRPGrpPrefixLength,
                                         &pimSmStaticRPRPAddr) != L7_SUCCESS)
    {
   
	    return(COMMIT_FAILED_ERROR);
    }
  }
#ifdef NOT_SUPPORTED
  if (VALID(I_pimSmStaticRPPimSmMode, data->valid))
  {
    if (usmDbPimsmStaticRPPimModeSet(data->pimSmStaticRPAddressType,
                                           &pimSmStaticRPGrpAddr, 
                                           data->pimSmStaticRPGrpPrefixLength,
	                                         data->pimSmStaticRPPimSmMode) != L7_SUCCESS)
    {
	    return(COMMIT_FAILED_ERROR);
    }
  }
#endif
  if (VALID(I_pimSmStaticRPOverrideDynamic, data->valid))
  {
    if (data->pimSmStaticRPOverrideDynamic == D_pimSmStaticRPOverrideDynamic_true)
    {
       overRide = L7_TRUE;
    }
    if (usmDbPimsmStaticRPOverrideDynamicSet(data->pimSmStaticRPAddressType,
                                                   &pimSmStaticRPGrpAddr, 
                                                   data->pimSmStaticRPGrpPrefixLength,
 	                                           overRide) != L7_SUCCESS)
    {
	     return(COMMIT_FAILED_ERROR);
    }
  }
  return NO_ERROR;
}

pimSmGroupMappingEntry_t *
k_pimSmGroupMappingEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 pimSmGroupMappingOrigin,
                           SR_INT32 pimSmGroupMappingAddressType,
                           OctetString * pimSmGroupMappingGrpAddress,
                           SR_UINT32 pimSmGroupMappingGrpPrefixLength,
                           OctetString * pimSmGroupMappingRPAddress)
{
  static pimSmGroupMappingEntry_t pimSmGroupMappingEntryData;
  L7_inet_addr_t pimMappingGrpAddr, pimMappingRPAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;
  L7_RC_t rc1 = L7_SUCCESS, rc2 = L7_SUCCESS;
  L7_uchar8 OriginV4 = 0, OriginV6 = 0, LengthV4 = 0, LengthV6 = 0;
  L7_inet_addr_t pimMappingGrpV4Addr, pimMappingGrpV6Addr, pimMappingRPV4Addr, pimMappingRPV6Addr;
  L7_uchar8 familyType = L7_NULL; 

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimSmGroupMappingEntryData.pimSmGroupMappingGrpAddress = MakeOctetString(NULL, 0);
    pimSmGroupMappingEntryData.pimSmGroupMappingRPAddress = MakeOctetString(NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if((pimSmGroupMappingAddressType != L7_NULL) &&
     (pimSmGroupMappingAddressType != L7_AF_INET) &&
     (pimSmGroupMappingAddressType != L7_AF_INET6))
    return(NULL);


  /* Clear all the bits*/
  ZERO_VALID(pimSmGroupMappingEntryData.valid);
  pimSmGroupMappingEntryData.pimSmGroupMappingOrigin = pimSmGroupMappingOrigin;
  pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = pimSmGroupMappingAddressType;
  (void)SafeMakeOctetString(&pimSmGroupMappingEntryData.pimSmGroupMappingGrpAddress, NULL, 0);
  pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength = pimSmGroupMappingGrpPrefixLength;
  (void)SafeMakeOctetString(&pimSmGroupMappingEntryData.pimSmGroupMappingRPAddress, NULL, 0);
  
  SET_VALID(I_pimSmGroupMappingOrigin, pimSmGroupMappingEntryData.valid);
  SET_VALID(I_pimSmGroupMappingAddressType, pimSmGroupMappingEntryData.valid);
  SET_VALID(I_pimSmGroupMappingGrpAddress, pimSmGroupMappingEntryData.valid);
  SET_VALID(I_pimSmGroupMappingGrpPrefixLength, pimSmGroupMappingEntryData.valid);
  SET_VALID(I_pimSmGroupMappingRPAddress, pimSmGroupMappingEntryData.valid);

  if(pimSmGroupMappingAddressType == L7_NULL)
    pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET;
  if(pimSmGroupMappingEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv4)
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
    else if (firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(pimSmGroupMappingGrpAddress);
      ipAddr2 = OctetStringToIP(pimSmGroupMappingRPAddress);
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
    pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET;
    familyType=L7_AF_INET;
    OriginV4 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingOrigin;
    LengthV4 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength;

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
    OriginV6 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingOrigin;
    LengthV6 = 0;
  }
  else if(pimSmGroupMappingEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv6)
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
    else if (firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmGroupMappingGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimSmGroupMappingRPAddress, (L7_char8*)&ipAddr2);
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
    pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET6;
    familyType=L7_AF_INET6;
    OriginV6 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingOrigin;
    LengthV6 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength;

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
    OriginV4 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingOrigin;
    LengthV4 = 0;
  }

  if(searchType == EXACT)
  {
    if((pimSmGroupMappingAddressType != D_pimSmGroupMappingAddressType_ipv4) &&
        (pimSmGroupMappingAddressType != D_pimSmGroupMappingAddressType_ipv6))
    {
      return(NULL);
    }
    if(usmDbPimsmGroupMappingEntryGet(pimSmGroupMappingEntryData.pimSmGroupMappingOrigin, 
                                            pimSmGroupMappingEntryData.pimSmGroupMappingAddressType, 
                                            &pimMappingGrpAddr, 
                                            pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength, 
                                            &pimMappingRPAddr) != L7_SUCCESS)
    {
      return(NULL);
    }
  }
  else if(searchType == NEXT)
  {
    if(pimSmGroupMappingAddressType >= D_pimSmGroupMappingAddressType_ipv6)
    {
      OriginV4 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingOrigin + 1;
    }
    else if(pimSmGroupMappingAddressType <= D_pimSmGroupMappingAddressType_ipv6)
    {
      OriginV4 = (L7_uchar8)pimSmGroupMappingEntryData.pimSmGroupMappingOrigin;
    }
    pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET;
    if((usmDbPimsmGroupMappingEntryGet(OriginV4,
                                            pimSmGroupMappingEntryData.pimSmGroupMappingAddressType,
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
      pimSmGroupMappingEntryData.pimSmGroupMappingOrigin = OriginV4;
      pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength = LengthV4;
      pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET;
      inetAddressZeroSet(L7_AF_INET, &pimMappingGrpAddr);
      inetAddressZeroSet(L7_AF_INET, &pimMappingRPAddr);
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_SUCCESS))
    {
      pimSmGroupMappingEntryData.pimSmGroupMappingOrigin = OriginV6;
      pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength = LengthV6;
      pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET6;
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
        pimSmGroupMappingEntryData.pimSmGroupMappingOrigin = OriginV4;
        pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength = LengthV4;
        pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET;
        inetAddressZeroSet(L7_AF_INET, &pimMappingGrpAddr);
        inetAddressZeroSet(L7_AF_INET, &pimMappingRPAddr);
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
      }
      if(ret == L7_AF_INET6)
      {
        pimSmGroupMappingEntryData.pimSmGroupMappingOrigin = OriginV6;
        pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength = LengthV6;
        pimSmGroupMappingEntryData.pimSmGroupMappingAddressType = L7_AF_INET6;
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
  SET_VALID(I_pimSmGroupMappingOrigin, pimSmGroupMappingEntryData.valid);
  SET_VALID(I_pimSmGroupMappingAddressType, pimSmGroupMappingEntryData.valid);
  SET_VALID(I_pimSmGroupMappingGrpPrefixLength, pimSmGroupMappingEntryData.valid);
  /*  converting the IP Address to Octet String */
  if(pimSmGroupMappingEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmGroupMappingEntryData.pimSmGroupMappingGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmGroupMappingGrpAddress, pimSmGroupMappingEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmGroupMappingEntryData.pimSmGroupMappingRPAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimSmGroupMappingRPAddress, pimSmGroupMappingEntryData.valid);
    }
  }
  else if(pimSmGroupMappingEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmGroupMappingEntryData.pimSmGroupMappingGrpAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmGroupMappingGrpAddress, pimSmGroupMappingEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimSmGroupMappingEntryData.pimSmGroupMappingRPAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimSmGroupMappingRPAddress, pimSmGroupMappingEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimSmGroupMappingOrigin: /* already got it above*/
	 break;
    case I_pimSmGroupMappingAddressType:
	 break;
    case I_pimSmGroupMappingGrpAddress:
	 break;
    case I_pimSmGroupMappingGrpPrefixLength:
	 break;
    case I_pimSmGroupMappingRPAddress:
	 break;
    case I_pimSmGroupMappingPimSmMode:
         if(usmDbPimsmGroupMappingPimModeGet(pimSmGroupMappingEntryData.pimSmGroupMappingOrigin, pimSmGroupMappingEntryData.pimSmGroupMappingAddressType, &pimMappingGrpAddr, pimSmGroupMappingEntryData.pimSmGroupMappingGrpPrefixLength, &pimMappingRPAddr,
                           &pimSmGroupMappingEntryData.pimSmGroupMappingPimSmMode) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimSmGroupMappingEntryData.valid);
         }
	 break;
    case I_pimSmGroupMappingPrecedence:
	 break;
    case I_pimSmGroupMappingActive:
         pimSmGroupMappingEntryData.pimSmGroupMappingActive = D_pimSmGroupMappingActive_true;
         SET_VALID(nominator, pimSmGroupMappingEntryData.valid);
	 break;
    default:
	 /*unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimSmGroupMappingEntryData.valid))
  {
    firstTime = L7_TRUE;
    return(NULL);
   }
   return(&pimSmGroupMappingEntryData);
}

bsrCandidateRPEntry_t *
k_bsrCandidateRPEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 bsrCandidateRPAddressType,
                          OctetString * bsrCandidateRPAddress,
                          OctetString * bsrCandidateRPGroupAddress,
                          SR_UINT32 bsrCandidateRPGroupPrefixLength)
{

 /* PROBLEM WITH GET NEXT API */
  static bsrCandidateRPEntry_t bsrCandidateRPEntryData;
  L7_inet_addr_t bsrCandtRPAddr, bsrCandtRPGrpAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  L7_uchar8      charVal;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    bsrCandidateRPEntryData.bsrCandidateRPAddress = MakeOctetString(NULL, 0);
    bsrCandidateRPEntryData.bsrCandidateRPGroupAddress = MakeOctetString(NULL, 0);
  }


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(bsrCandidateRPEntryData.valid);
  bsrCandidateRPEntryData.bsrCandidateRPAddressType = bsrCandidateRPAddressType;
  (void)SafeMakeOctetString(&bsrCandidateRPEntryData.bsrCandidateRPAddress, NULL, 0);
  (void)SafeMakeOctetString(&bsrCandidateRPEntryData.bsrCandidateRPGroupAddress, NULL, 0);
  bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength = bsrCandidateRPGroupPrefixLength;
  SET_VALID(I_bsrCandidateRPAddressType, bsrCandidateRPEntryData.valid);
  SET_VALID(I_bsrCandidateRPAddress, bsrCandidateRPEntryData.valid);
  SET_VALID(I_bsrCandidateRPGroupAddress, bsrCandidateRPEntryData.valid);
  SET_VALID(I_bsrCandidateRPGroupPrefixLength, bsrCandidateRPEntryData.valid);

  if(bsrCandidateRPAddressType == L7_NULL)
    bsrCandidateRPEntryData.bsrCandidateRPAddressType = L7_AF_INET;

  if(bsrCandidateRPEntryData.bsrCandidateRPAddressType == D_bsrCandidateRPAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if(firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(bsrCandidateRPAddress);
      ipAddr2 = OctetStringToIP(bsrCandidateRPGroupAddress);
    }

    if( inetAddressSet(L7_AF_INET, &ipAddr1, &bsrCandtRPAddr) != L7_SUCCESS)
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr2, &bsrCandtRPGrpAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    bsrCandidateRPEntryData.bsrCandidateRPAddressType = L7_AF_INET;
  }
  else if(bsrCandidateRPEntryData.bsrCandidateRPAddressType == D_bsrCandidateRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if(firstTime != L7_TRUE)
    {
      OctetStringToIP6(bsrCandidateRPAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(bsrCandidateRPGroupAddress, (L7_char8*)&ipAddr2);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr1, &bsrCandtRPAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr2, &bsrCandtRPGrpAddr))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }
    bsrCandidateRPEntryData.bsrCandidateRPAddressType = L7_AF_INET6;
  }

  charVal = bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength;
  if((searchType == EXACT) ?
    (usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT,(L7_uchar8)bsrCandidateRPEntryData.bsrCandidateRPAddressType                                                      , &bsrCandtRPAddr, &bsrCandtRPGrpAddr,
                                     (L7_uchar8) bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength) != L7_SUCCESS) :
   ((usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT,(L7_uchar8)bsrCandidateRPEntryData.bsrCandidateRPAddressType                                                      , &bsrCandtRPAddr, &bsrCandtRPGrpAddr, 
                            (L7_uchar8)bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength) != L7_SUCCESS) &&
    (usmDbPimsmbsrCandRPEntryNextGet((L7_uchar8 )bsrCandidateRPEntryData.bsrCandidateRPAddressType, &bsrCandtRPAddr, &bsrCandtRPGrpAddr,&charVal) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(bsrCandidateRPEntryData.bsrCandidateRPAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(bsrCandidateRPEntryData.bsrCandidateRPAddressType == L7_AF_INET)
      {
        memset(&bsrCandtRPAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&bsrCandtRPGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        bsrCandtRPAddr.family = L7_AF_INET6;
        bsrCandtRPGrpAddr.family = L7_AF_INET6;
        bsrCandidateRPEntryData.bsrCandidateRPAddressType = L7_AF_INET6;
        if((usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT, bsrCandidateRPEntryData.bsrCandidateRPAddressType, &bsrCandtRPAddr, &bsrCandtRPGrpAddr, bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength) != L7_SUCCESS) &&
           (usmDbPimsmbsrCandRPEntryNextGet((L7_uchar8 )bsrCandidateRPEntryData.bsrCandidateRPAddressType, &bsrCandtRPAddr, &bsrCandtRPGrpAddr,&charVal) != L7_SUCCESS))
        {
          firstTime = L7_TRUE;
          allocMem = L7_TRUE;
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

   bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength = charVal;
  SET_VALID(I_bsrCandidateRPGroupPrefixLength, bsrCandidateRPEntryData.valid);

  /*  converting the IP Address to Octet String */
  if(bsrCandidateRPEntryData.bsrCandidateRPAddressType == D_bsrCandidateRPAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &bsrCandtRPAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      ZERO_VALID(bsrCandidateRPEntryData.valid);
      return(NULL);
    }
    if (SafeMakeOctetString(&bsrCandidateRPEntryData.bsrCandidateRPAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_bsrCandidateRPAddress, bsrCandidateRPEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &bsrCandtRPGrpAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&bsrCandidateRPEntryData.bsrCandidateRPGroupAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_bsrCandidateRPGroupAddress, bsrCandidateRPEntryData.valid);
    }
  }
  else if(bsrCandidateRPEntryData.bsrCandidateRPAddressType == D_bsrCandidateRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &bsrCandtRPAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&bsrCandidateRPEntryData.bsrCandidateRPAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_bsrCandidateRPAddress, bsrCandidateRPEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &bsrCandtRPGrpAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&bsrCandidateRPEntryData.bsrCandidateRPGroupAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_bsrCandidateRPGroupAddress, bsrCandidateRPEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_bsrCandidateRPAddressType: /* already got it above*/
         break;
    case I_bsrCandidateRPAddress:
         break;
    case I_bsrCandidateRPGroupAddress:
         break;
    case I_bsrCandidateRPGroupPrefixLength:
         break;
    case I_bsrCandidateRPBidir:
         break;
    case I_bsrCandidateRPStatus:
         if (snmpPimSmbsrCandidateRPStatusGet(bsrCandidateRPEntryData.bsrCandidateRPAddressType, &bsrCandtRPAddr, &bsrCandtRPGrpAddr,
               bsrCandidateRPEntryData.bsrCandidateRPGroupPrefixLength, &bsrCandidateRPEntryData.bsrCandidateRPStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, bsrCandidateRPEntryData.valid);
         }
         break;
    default:
	 /*unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, bsrCandidateRPEntryData.valid))
    return(NULL);

   return(&bsrCandidateRPEntryData);

}

int
k_bsrCandidateRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_bsrCandidateRPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_bsrCandidateRPEntry_set_defaults(doList_t *dp)
{
    bsrCandidateRPEntry_t *data = (bsrCandidateRPEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_bsrCandidateRPEntry_set(bsrCandidateRPEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t bsrCandtRPAddr, bsrCandtRPGrpAddr;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
      return(COMMIT_FAILED_ERROR);

  if(data->bsrCandidateRPAddressType == D_bsrCandidateRPAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->bsrCandidateRPAddress->octet_ptr, data->bsrCandidateRPAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->bsrCandidateRPGroupAddress->octet_ptr, data->bsrCandidateRPGroupAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->bsrCandidateRPAddressType == D_bsrCandidateRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->bsrCandidateRPGroupAddress->octet_ptr, data->bsrCandidateRPGroupAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &bsrCandtRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->bsrCandidateRPGroupAddress->octet_ptr, data->bsrCandidateRPGroupAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_bsrCandidateRPStatus, data->valid))
  {
    if (snmpPimSmbsrCandidateRPStatusSet(data->bsrCandidateRPAddressType,
                                       bsrCandtRPAddr,
                                       bsrCandtRPGrpAddr,
                                       data->bsrCandidateRPGroupPrefixLength,
                                       data->bsrCandidateRPStatus) != L7_SUCCESS )
    {
      CLR_VALID(I_bsrCandidateRPStatus, data->valid);
      return (COMMIT_FAILED_ERROR);
    }
  }

   return NO_ERROR;
}

bsrRPSetEntry_t *
k_bsrRPSetEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 pimSmGroupMappingOrigin,
                    SR_INT32 pimSmGroupMappingAddressType,
                    OctetString * pimSmGroupMappingGrpAddress,
                    SR_UINT32 pimSmGroupMappingGrpPrefixLength,
                    OctetString * pimSmGroupMappingRPAddress)
{
  static bsrRPSetEntry_t bsrRPSetEntryData;
  L7_inet_addr_t pimMappingGrpAddr, pimMappingRPAddr;
  L7_inet_addr_t pimMappingGrpV4Addr, pimMappingGrpV6Addr, pimMappingRPV4Addr, pimMappingRPV6Addr;
  static L7_BOOL firstTime = L7_TRUE;
  L7_RC_t rc1 = L7_SUCCESS, rc2 = L7_SUCCESS;
  L7_uint32 OriginV4=0, OriginV6=0, LengthV4=0, LengthV6=0;
  L7_inet_addr_t Len4,Len6;
  static L7_BOOL initializeMem = L7_TRUE;

  L7_uchar8 familyType=0,*prefixLength=L7_NULLPTR,*origin=L7_NULLPTR,*octetIpAddr1=L7_NULLPTR,*octetIpAddr2=L7_NULLPTR;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    bsrRPSetEntryData.pimSmGroupMappingGrpAddress = MakeOctetString(NULL, 0);
    bsrRPSetEntryData.pimSmGroupMappingRPAddress = MakeOctetString(NULL, 0);
  }

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);



  /* Clear all the bits*/
  ZERO_VALID(bsrRPSetEntryData.valid);
  bsrRPSetEntryData.pimSmGroupMappingOrigin = pimSmGroupMappingOrigin;
  bsrRPSetEntryData.pimSmGroupMappingAddressType = pimSmGroupMappingAddressType;
  (void)SafeMakeOctetString(&bsrRPSetEntryData.pimSmGroupMappingGrpAddress, NULL, 0);
  bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength = pimSmGroupMappingGrpPrefixLength;
  (void)SafeMakeOctetString(&bsrRPSetEntryData.pimSmGroupMappingRPAddress, NULL, 0);
  SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingOrigin, bsrRPSetEntryData.valid);
  SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingAddressType, bsrRPSetEntryData.valid);
  SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingGrpAddress, bsrRPSetEntryData.valid);
  SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingGrpPrefixLength, bsrRPSetEntryData.valid);
  SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingRPAddress, bsrRPSetEntryData.valid);

  if(pimSmGroupMappingAddressType == L7_NULL)
    bsrRPSetEntryData.pimSmGroupMappingAddressType = D_pimSmGroupMappingAddressType_ipv4;   

  if(bsrRPSetEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;
    L7_in6_addr_t ipAddr3;

    memset(&ipAddr3, L7_NULL, sizeof(L7_in6_addr_t));
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if (firstTime != L7_TRUE)
    {
      ipAddr1 = OctetStringToIP(pimSmGroupMappingGrpAddress);
      ipAddr2 = OctetStringToIP(pimSmGroupMappingRPAddress);
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
    bsrRPSetEntryData.pimSmGroupMappingAddressType = D_pimSmGroupMappingAddressType_ipv4;
     familyType=L7_AF_INET;

    OriginV4 = bsrRPSetEntryData.pimSmGroupMappingOrigin;
    origin = (L7_uchar8*) &OriginV4;
    LengthV4 = bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength;
    /* convert to inet address data structure*/
    inetAddressZeroSet(L7_AF_INET, &Len4);
    prefixLength = (L7_uchar8*) &LengthV4;
    if( inetMaskLenToMask(familyType,*prefixLength, &Len4) != L7_SUCCESS)
    {
      firstTime=L7_TRUE;
      return (NULL);
    }
    
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
    OriginV6 = bsrRPSetEntryData.pimSmGroupMappingOrigin;
    LengthV6 = bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength;
     /* convert to inet address data structure*/
    memset(&Len6, L7_NULL, sizeof(L7_in6_addr_t));
    if( inetMaskLenToMask(familyType,LengthV6, &Len6) != L7_SUCCESS)
    {
       firstTime=L7_TRUE;
       return (NULL);

    }
  }
  else if(bsrRPSetEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;
    L7_uint32 ipAddr3 = 0;

    memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
    memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&ipAddr1, L7_NULL, sizeof(L7_in6_addr_t));
      memset(&ipAddr2, L7_NULL, sizeof(L7_in6_addr_t));
    }
    else if (firstTime != L7_TRUE)
    {
      OctetStringToIP6(pimSmGroupMappingGrpAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimSmGroupMappingRPAddress, (L7_char8*)&ipAddr2);
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
    bsrRPSetEntryData.pimSmGroupMappingAddressType = D_pimSmGroupMappingAddressType_ipv6;
 
   familyType=L7_AF_INET6;

    OriginV6 = bsrRPSetEntryData.pimSmGroupMappingOrigin;
    origin= (L7_uchar8*) &OriginV6;
    LengthV6 = bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength;
    /* convert to inet address data structure*/
    inetAddressZeroSet(L7_AF_INET6, &Len6);
    prefixLength = (L7_uchar8*) &LengthV6;
    if( inetMaskLenToMask(familyType,*prefixLength, &Len6) != L7_SUCCESS)
    {
      firstTime=L7_TRUE;
      return (NULL);

    }
    

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
    OriginV4 = bsrRPSetEntryData.pimSmGroupMappingOrigin;
    LengthV4 = 0;
  }
  else
  {
    return L7_NULLPTR;
  }

  if(searchType == EXACT)
  {
    if((pimSmGroupMappingAddressType != D_pimSmGroupMappingAddressType_ipv4) &&
        (pimSmGroupMappingAddressType != D_pimSmGroupMappingAddressType_ipv6))
    {
      return(NULL);
    }
    /* if origin != BSR */
    if (pimSmGroupMappingOrigin != D_pimSmGroupMappingOrigin_bsr)
    {
      return(NULL);
    }
    /*ipv4*/ 
    
    if (bsrRPSetEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv4)
    {
      if(usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                        familyType,
                                        bsrRPSetEntryData.pimSmGroupMappingOrigin,
                                        &pimMappingGrpV4Addr, 
                                        &Len4, 
                                        &pimMappingRPV4Addr) != L7_SUCCESS)
      {
        return(NULL); 
      }
    } 

    /*ipv6*/
    if (bsrRPSetEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv6)
    {
      if(usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                        familyType,
                                        bsrRPSetEntryData.pimSmGroupMappingOrigin,
                                        &pimMappingGrpV6Addr, 
                                        &Len6, 
                                        &pimMappingRPV6Addr) != L7_SUCCESS)
      {
        return(NULL);
        
      }

    }
    
  }
  else if(searchType == NEXT)
  {
    if(bsrRPSetEntryData.pimSmGroupMappingAddressType >= D_pimSmGroupMappingAddressType_ipv6)
    {
      *origin=OriginV4 = bsrRPSetEntryData.pimSmGroupMappingOrigin + 1;
    }
    else if(bsrRPSetEntryData.pimSmGroupMappingAddressType <= D_pimSmGroupMappingAddressType_ipv6)
    {
      *origin=OriginV4 = bsrRPSetEntryData.pimSmGroupMappingOrigin;
    }
   
    familyType = L7_AF_INET;
    if((usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                       familyType,
                                       *origin,
                                       &pimMappingGrpV4Addr,
                                       &Len4,
                                       &pimMappingRPV4Addr) != L7_SUCCESS) && 
       (snmpBsrRPSetGetNext(USMDB_UNIT_CURRENT,
                                            familyType,
                                            origin,
                                            &pimMappingGrpV4Addr,
                                            &Len4, 
                                            &pimMappingRPV4Addr) != L7_SUCCESS))
    {
      rc1 = L7_FAILURE;
    }
   
     else
    {
     OriginV4=*origin;
    }




    familyType=L7_AF_INET6;
    if((usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                       familyType,
                                       *origin,
                                       &pimMappingGrpV6Addr,
                                       &Len6,
                                       &pimMappingRPV6Addr) != L7_SUCCESS) && 
       (snmpBsrRPSetGetNext(USMDB_UNIT_CURRENT,
                                            familyType,
                                            origin,
                                            &pimMappingGrpV6Addr,
                                            &Len6,
                                            &pimMappingRPV6Addr) != L7_SUCCESS)) 
    {
      rc2 = L7_FAILURE;
    }
   else
    {
     OriginV6=*origin; 
    }

    if((rc1 == L7_SUCCESS) && (rc2 == L7_FAILURE))
    {
      bsrRPSetEntryData.pimSmGroupMappingOrigin = OriginV4;
     
      /* convert from  inet_addressto l7_uint32 */
      if (inetMaskToMaskLen(&Len4, prefixLength) != L7_SUCCESS)
      {
        LengthV4=0;
        firstTime=L7_TRUE;
      }
      else
      {
        LengthV4 =(L7_uint32) *prefixLength;
      }
      bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength = LengthV4;
      bsrRPSetEntryData.pimSmGroupMappingAddressType =D_pimSmGroupMappingAddressType_ipv4;
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_SUCCESS))
    {
      bsrRPSetEntryData.pimSmGroupMappingOrigin = OriginV6;
      
      /* convert from  inet_addressto l7_uint32 */
      if (inetMaskToMaskLen(&Len6, prefixLength) != L7_SUCCESS)
      {
        LengthV6=0;
        firstTime=L7_TRUE;
      }
      else
      {
        LengthV6= (L7_uint32)*prefixLength;
      }
      bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength = LengthV6;
      bsrRPSetEntryData.pimSmGroupMappingAddressType = D_pimSmGroupMappingAddressType_ipv6; 
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);
    }
    else if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
    {
      L7_int32 ret;

      ret = pimGrpMappingTableIndexCompare(OriginV4, OriginV6);

      if(ret == L7_AF_INET)
      {
        bsrRPSetEntryData.pimSmGroupMappingOrigin = OriginV4;
        
        /* convert from  inet_addressto l7_uint32 */
        if (inetMaskToMaskLen(&Len4, prefixLength) != L7_SUCCESS)
        {
          LengthV4=0;
          firstTime=L7_TRUE;
        }
        else
        {
          LengthV4 =(L7_uint32) *prefixLength;
        }
        bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength = LengthV4;
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
      }
      if(ret == L7_AF_INET6)
      {
        bsrRPSetEntryData.pimSmGroupMappingOrigin = OriginV6;
        
        /* convert from  inet_addressto l7_uint32 */
        if (inetMaskToMaskLen(&Len6, prefixLength) != L7_SUCCESS)
        {
          LengthV6=0;
          firstTime=L7_TRUE;
        }
        else
        {
          LengthV6 = (L7_uint32) *prefixLength;
        }
        bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength = LengthV6;
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);
      }
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_FAILURE))
    {
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(bsrRPSetEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
     octetIpAddr1=(L7_uchar8 *)&ipAddr1;
    if (SafeMakeOctetString(&bsrRPSetEntryData.pimSmGroupMappingGrpAddress,
                            octetIpAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingGrpAddress, bsrRPSetEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    octetIpAddr2=(L7_uchar8 *)&ipAddr2;
    if (SafeMakeOctetString(&bsrRPSetEntryData.pimSmGroupMappingRPAddress,
                            octetIpAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingRPAddress, bsrRPSetEntryData.valid);
    }
  }
  else if(bsrRPSetEntryData.pimSmGroupMappingAddressType == D_pimSmGroupMappingAddressType_ipv6)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
     octetIpAddr1=(L7_uchar8 *)&ipAddr1;
    if (SafeMakeOctetString(&bsrRPSetEntryData.pimSmGroupMappingGrpAddress,
                            octetIpAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingGrpAddress, bsrRPSetEntryData.valid);
    }

   if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
     octetIpAddr2= (L7_uchar8 *)&ipAddr2;

    if (SafeMakeOctetString(&bsrRPSetEntryData.pimSmGroupMappingRPAddress,
                            octetIpAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_bsrRPSetEntryIndex_pimSmGroupMappingRPAddress, bsrRPSetEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_bsrRPSetEntryIndex_pimSmGroupMappingOrigin:
    case I_bsrRPSetEntryIndex_pimSmGroupMappingAddressType:
    case I_bsrRPSetEntryIndex_pimSmGroupMappingGrpAddress:
    case I_bsrRPSetEntryIndex_pimSmGroupMappingGrpPrefixLength:
    case I_bsrRPSetEntryIndex_pimSmGroupMappingRPAddress:
    case I_bsrRPSetPriority:
         if(usmDbPimsmbsrRPSetPriorityGet(bsrRPSetEntryData.pimSmGroupMappingOrigin, bsrRPSetEntryData.pimSmGroupMappingAddressType,
                           &pimMappingGrpAddr, bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength, &pimMappingRPAddr, 
                           &bsrRPSetEntryData.bsrRPSetPriority) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrRPSetEntryData.valid);
         }
        break;
    case I_bsrRPSetHoldtime:
         if(usmDbPimsmbsrRPSetHoldtimeGet(bsrRPSetEntryData.pimSmGroupMappingOrigin, bsrRPSetEntryData.pimSmGroupMappingAddressType,
                           &pimMappingGrpAddr, bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength, &pimMappingRPAddr, 
                           &bsrRPSetEntryData.bsrRPSetHoldtime) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrRPSetEntryData.valid);
         }
        break;
    case I_bsrRPSetExpiryTime:
         if(usmDbPimsmbsrRPSetExpiryTimeGet(bsrRPSetEntryData.pimSmGroupMappingOrigin, bsrRPSetEntryData.pimSmGroupMappingAddressType,
                           &pimMappingGrpAddr, bsrRPSetEntryData.pimSmGroupMappingGrpPrefixLength, &pimMappingRPAddr, 
                           &bsrRPSetEntryData.bsrRPSetExpiryTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrRPSetEntryData.valid);
         }
        break;
    default:
	/* unknown nominator */
	    return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, bsrRPSetEntryData.valid))
    return(NULL);
  return(&bsrRPSetEntryData);
}

bsrCandidateBSREntry_t *
k_bsrCandidateBSREntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 bsrCandidateBSRAddressType)
{
  static bsrCandidateBSREntry_t bsrCandidateBSREntryData;
  L7_inet_addr_t bsrCandtBSRAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    bsrCandidateBSREntryData.bsrCandidateBSRAddress = MakeOctetString(NULL, 0);
  }
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    (void)SafeMakeOctetString(&bsrCandidateBSREntryData.bsrCandidateBSRAddress, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(bsrCandidateBSREntryData.valid);
  bsrCandidateBSREntryData.bsrCandidateBSRAddressType = bsrCandidateBSRAddressType;
  SET_VALID(I_bsrCandidateBSRAddressType, bsrCandidateBSREntryData.valid);

  if(bsrCandidateBSRAddressType == L7_NULL)
    bsrCandidateBSREntryData.bsrCandidateBSRAddressType = L7_AF_INET;

  if(bsrCandidateBSRAddressType == D_bsrCandidateBSRAddressType_ipv4)
  {
    bsrCandidateBSREntryData.bsrCandidateBSRAddressType = L7_AF_INET;
  }
  else if(bsrCandidateBSRAddressType == D_bsrCandidateBSRAddressType_ipv6)
  {
    bsrCandidateBSREntryData.bsrCandidateBSRAddressType = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmCandBSREntryGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType) != L7_SUCCESS) :
    ((usmDbPimsmCandBSREntryGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType) != L7_SUCCESS) &&
    (usmDbPimsmCandBSREntryNextGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(bsrCandidateBSREntryData.bsrCandidateBSRAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(bsrCandidateBSREntryData.bsrCandidateBSRAddressType == L7_AF_INET)
      {
        bsrCandidateBSREntryData.bsrCandidateBSRAddressType = L7_AF_INET6;
        if((usmDbPimsmCandBSREntryGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType) != L7_SUCCESS) &&
           (usmDbPimsmCandBSREntryNextGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType) != L7_SUCCESS))
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

  switch (nominator)
  {
    case -1:
    case I_bsrCandidateBSRAddressType: /* already got it above*/
         break;
    case I_bsrCandidateBSRAddress:
         if(usmDbPimsmCandBSRAddrGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType,
                           &bsrCandtBSRAddr) == L7_SUCCESS)
         {
             if(bsrCandidateBSREntryData.bsrCandidateBSRAddressType == D_pimSmSGAddressType_ipv4)
             {
               L7_uint32 ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &bsrCandtBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&bsrCandidateBSREntryData.bsrCandidateBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
                 {
                   SET_VALID(nominator, bsrCandidateBSREntryData.valid);
                 }
               }
             }
             else if(bsrCandidateBSREntryData.bsrCandidateBSRAddressType == D_pimSmSGAddressType_ipv6)
             {
               L7_in6_addr_t ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &bsrCandtBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&bsrCandidateBSREntryData.bsrCandidateBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
                 {
                   SET_VALID(nominator, bsrCandidateBSREntryData.valid);
                 }
               }
             }
         }
         break;
    case I_bsrCandidateBSRPriority:
         if(usmDbPimsmCandBSRPriorityGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType,
                           &bsrCandidateBSREntryData.bsrCandidateBSRPriority) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrCandidateBSREntryData.valid);
         }
         break;
    case I_bsrCandidateBSRHashMaskLength:
         if(usmDbPimsmCandBSRHashMaskLengthGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType,
                           &bsrCandidateBSREntryData.bsrCandidateBSRHashMaskLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrCandidateBSREntryData.valid);
         }
         break;
    case I_bsrCandidateBSRElectedBSR:
         if(usmDbPimsmCandBSRElectedBSRGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType,
                           &bsrCandidateBSREntryData.bsrCandidateBSRElectedBSR) == L7_SUCCESS)
         {
           if (bsrCandidateBSREntryData.bsrCandidateBSRElectedBSR != L7_TRUE)
           {
               bsrCandidateBSREntryData.bsrCandidateBSRElectedBSR = D_bsrCandidateBSRElectedBSR_false;
           }
           SET_VALID(nominator, bsrCandidateBSREntryData.valid);
         }
       break;
    case I_bsrCandidateBSRBootstrapTimer:
         if(usmDbPimsmCandBSRBootstrapTimerGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType,
                           &bsrCandidateBSREntryData.bsrCandidateBSRBootstrapTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrCandidateBSREntryData.valid);
         }
	       break;
    case I_bsrCandidateBSRStatus:
         if (snmpPimSmbsrCandidateBSRStatusGet(bsrCandidateBSREntryData.bsrCandidateBSRAddressType,
                         &bsrCandidateBSREntryData.bsrCandidateBSRStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, bsrCandidateBSREntryData.valid);
         }
       break;
    default:
	 /* unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, bsrCandidateBSREntryData.valid))
    return(NULL);

   return(&bsrCandidateBSREntryData);
}

int
k_bsrCandidateBSREntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_bsrCandidateBSREntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_bsrCandidateBSREntry_set_defaults(doList_t *dp)
{
    bsrCandidateBSREntry_t *data = (bsrCandidateBSREntry_t *) (dp->data);

    if ((data->bsrCandidateBSRAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->bsrCandidateBSRPriority = L7_PIMSM_CBSR_PRIORITY;
    data->bsrCandidateBSRHashMaskLength = L7_PIMSM_CBSR_HASH_MASK_LENGTH;
    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_bsrCandidateBSREntry_set(bsrCandidateBSREntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t bsrCandtBSRAddr;
  
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
      return(COMMIT_FAILED_ERROR);

  if(data->bsrCandidateBSRAddressType == D_bsrCandidateBSRAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->bsrCandidateBSRAddress->octet_ptr, data->bsrCandidateBSRAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtBSRAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->bsrCandidateBSRAddressType == D_bsrCandidateBSRAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->bsrCandidateBSRAddress->octet_ptr, data->bsrCandidateBSRAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &bsrCandtBSRAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  if (VALID(I_bsrCandidateBSRStatus, data->valid) ||
      VALID(I_bsrCandidateBSRAddress, data->valid))
  {
    if (data->bsrCandidateBSRStatus == D_bsrCandidateBSRStatus_createAndGo)
    {
      if (!VALID(I_bsrCandidateBSRAddress, data->valid))
         return(COMMIT_FAILED_ERROR);
    }
    else if (data->bsrCandidateBSRStatus == D_bsrCandidateBSRStatus_destroy)
    {
      if (usmDbPimsmCandBSREntryGet(data->bsrCandidateBSRAddressType) == L7_SUCCESS)
      {
        (void)usmDbPimsmCandBSRAddrGet(data->bsrCandidateBSRAddressType,
                           &bsrCandtBSRAddr);
      }
    }
    if (snmpPimSmbsrCandidateBSRSet(data->bsrCandidateBSRAddressType, 
                                          data->bsrCandidateBSRStatus, bsrCandtBSRAddr,
                                          data->bsrCandidateBSRPriority,
                                          data->bsrCandidateBSRHashMaskLength) != L7_SUCCESS)
    {
 	    CLR_VALID(I_bsrCandidateBSRStatus, data->valid);
	    return (COMMIT_FAILED_ERROR);
    }
  }
  if ( (VALID(I_bsrCandidateBSRPriority, data->valid) ||
      VALID(I_bsrCandidateBSRHashMaskLength, data->valid)) && 
       data->bsrCandidateBSRStatus == D_bsrCandidateBSRStatus_active)
  {

    if (usmDbPimsmCandBSREntryGet(data->bsrCandidateBSRAddressType) == L7_SUCCESS)
    {
      (void)usmDbPimsmCandBSRAddrGet(data->bsrCandidateBSRAddressType,
                          &bsrCandtBSRAddr);
    }
    if (snmpPimSmbsrCandidateBSRSet(data->bsrCandidateBSRAddressType,
                                          data->bsrCandidateBSRStatus, bsrCandtBSRAddr,
                                          data->bsrCandidateBSRPriority,
                                          data->bsrCandidateBSRHashMaskLength) != L7_SUCCESS)
    {
            return (COMMIT_FAILED_ERROR);
    }
 
  }


  return NO_ERROR;
}

bsrElectedBSREntry_t *
k_bsrElectedBSREntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 bsrElectedBSRAddressType)
{
  static bsrElectedBSREntry_t bsrElectedBSREntryData;
  L7_inet_addr_t bsrElectedBSRAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if ((bsrElectedBSRAddressType != L7_NULL) &&
      (bsrElectedBSRAddressType != L7_AF_INET) &&
      (bsrElectedBSRAddressType != L7_AF_INET6))
    return(NULL);

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    bsrElectedBSREntryData.bsrElectedBSRAddress = MakeOctetString(NULL, 0);
  }
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    (void)SafeMakeOctetString(&bsrElectedBSREntryData.bsrElectedBSRAddress, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(bsrElectedBSREntryData.valid);
  bsrElectedBSREntryData.bsrElectedBSRAddressType = bsrElectedBSRAddressType;
  SET_VALID(I_bsrElectedBSRAddressType, bsrElectedBSREntryData.valid);

  if(bsrElectedBSRAddressType == L7_NULL)
    bsrElectedBSREntryData.bsrElectedBSRAddressType = L7_AF_INET;


  if((searchType == EXACT) ?
    (usmDbPimsmbsrElectedBSREntryGet(bsrElectedBSREntryData.bsrElectedBSRAddressType) != L7_SUCCESS) :
    ((usmDbPimsmbsrElectedBSREntryGet(bsrElectedBSREntryData.bsrElectedBSRAddressType) != L7_SUCCESS) &&
    (usmDbPimsmbsrElectedBSREntryNextGet(&bsrElectedBSREntryData.bsrElectedBSRAddressType) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(bsrElectedBSREntryData.bsrElectedBSRAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(bsrElectedBSREntryData.bsrElectedBSRAddressType == L7_AF_INET)
      {
        bsrElectedBSREntryData.bsrElectedBSRAddressType = L7_AF_INET6;
        if((usmDbPimsmbsrElectedBSREntryGet(bsrElectedBSREntryData.bsrElectedBSRAddressType) != L7_SUCCESS) &&
           (usmDbPimsmbsrElectedBSREntryNextGet(&bsrElectedBSREntryData.bsrElectedBSRAddressType) != L7_SUCCESS))
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

  switch (nominator)
  {
    case -1:
    case I_bsrElectedBSRAddressType: /* already got it above*/
	       break;
    case I_bsrElectedBSRAddress:
         if(usmDbPimsmbsrElectedBSRAddrGet(bsrElectedBSREntryData.bsrElectedBSRAddressType,
                           (L7_uint32 *)&bsrElectedBSRAddr) == L7_SUCCESS)
         {
             if(bsrElectedBSREntryData.bsrElectedBSRAddressType == D_pimSmSGAddressType_ipv4)
             {
               L7_uint32 ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &bsrElectedBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&bsrElectedBSREntryData.bsrElectedBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
                 {
                   SET_VALID(nominator, bsrElectedBSREntryData.valid);
                 }
               }
             }
             else if(bsrElectedBSREntryData.bsrElectedBSRAddressType == D_pimSmSGAddressType_ipv6)
             {
               L7_in6_addr_t ipAddr;
 
               if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &bsrElectedBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&bsrElectedBSREntryData.bsrElectedBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
                 {
                   SET_VALID(nominator, bsrElectedBSREntryData.valid);
                 }
               }
             }
         }
         break;
    case I_bsrElectedBSRPriority:
         if(usmDbPimsmbsrElectedBSRPriorityGet(bsrElectedBSREntryData.bsrElectedBSRAddressType,
                           &bsrElectedBSREntryData.bsrElectedBSRPriority) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrElectedBSREntryData.valid);
         }
	       break;
    case I_bsrElectedBSRHashMaskLength:
         if(usmDbPimsmbsrElectedBSRHashMaskLengthGet(bsrElectedBSREntryData.bsrElectedBSRAddressType,
                           &bsrElectedBSREntryData.bsrElectedBSRHashMaskLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrElectedBSREntryData.valid);
         }
	       break;
    case I_bsrElectedBSRExpiryTime:
         if(usmDbPimsmbsrElectedBSRExpiryTimeGet(bsrElectedBSREntryData.bsrElectedBSRAddressType,
                           &bsrElectedBSREntryData.bsrElectedBSRExpiryTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrElectedBSREntryData.valid);
         }
	       break;
    case I_bsrElectedBSRCRPAdvTimer:
         if(usmDbPimsmbsrElectedBSRCRPAdvTimerGet(bsrElectedBSREntryData.bsrElectedBSRAddressType,
                           &bsrElectedBSREntryData.bsrElectedBSRCRPAdvTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, bsrElectedBSREntryData.valid);
         }
	       break;
    default:
	 /* unknown nominator */
	  return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, bsrElectedBSREntryData.valid))
    return(NULL);

   return(&bsrElectedBSREntryData);
}


