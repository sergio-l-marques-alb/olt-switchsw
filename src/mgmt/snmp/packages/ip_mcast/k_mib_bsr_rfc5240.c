/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/snmp/packages/ip_mcast/k_mib_pimbsr.c
 *
 * @purpose  Provide SNMP interface( for draft mib) PIMSM vendor code.
 *
 * @component SNMP (PIM)
 *
 * @comments
 *
 * @create 27/Apr/2010
 *
 * @author truchi
 * @end
 *
 **********************************************************************/
#include "k_private_base.h"
#include "k_mib_bsr_rfc5240_api.h"
#include <inst_lib.h>
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
#endif

extern L7_uint32  pimGrpMappingTableIndexCompare(L7_int32 OriginV4, L7_int32 OriginV6);
pimBsrCandidateRPEntry_t *
k_pimBsrCandidateRPEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 pimBsrCandidateRPAddressType,
                          OctetString * pimBsrCandidateRPAddress,
                          OctetString * pimBsrCandidateRPGroupAddress,
                          SR_UINT32 pimBsrCandidateRPGroupPrefixLength)
{

 /* PROBLEM WITH GET NEXT API */
  static pimBsrCandidateRPEntry_t pimBsrCandidateRPEntryData;
  L7_inet_addr_t bsrCandtRPAddr, bsrCandtRPGrpAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
  L7_uchar8      charVal;
  static L7_BOOL initializeMem = L7_TRUE;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimBsrCandidateRPEntryData.pimBsrCandidateRPAddress = MakeOctetString(NULL, 0);
    pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupAddress = MakeOctetString(NULL, 0);
  }


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(pimBsrCandidateRPEntryData.valid);
  pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType = pimBsrCandidateRPAddressType;
  (void)SafeMakeOctetString(&pimBsrCandidateRPEntryData.pimBsrCandidateRPAddress, NULL, 0);
  (void)SafeMakeOctetString(&pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupAddress, NULL, 0);
  pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength = pimBsrCandidateRPGroupPrefixLength;
  SET_VALID(I_pimBsrCandidateRPAddressType, pimBsrCandidateRPEntryData.valid);
  SET_VALID(I_pimBsrCandidateRPAddress, pimBsrCandidateRPEntryData.valid);
  SET_VALID(I_pimBsrCandidateRPGroupAddress, pimBsrCandidateRPEntryData.valid);
  SET_VALID(I_pimBsrCandidateRPGroupPrefixLength, pimBsrCandidateRPEntryData.valid);

  if(pimBsrCandidateRPAddressType == L7_NULL)
    pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType = L7_AF_INET;

  if(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType == D_pimBsrCandidateRPAddressType_ipv4)
  {
    L7_uint32 ipAddr1 = 0, ipAddr2 = 0;

    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      ipAddr1 = 0;
      ipAddr2 = 0;
    }
    else if ((firstTime != L7_TRUE) || (searchType == EXACT))
    {
      ipAddr1 = OctetStringToIP(pimBsrCandidateRPAddress);
      ipAddr2 = OctetStringToIP(pimBsrCandidateRPGroupAddress);
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
    pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType = L7_AF_INET;
  }
  else if(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType == D_pimBsrCandidateRPAddressType_ipv6)
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
    else if ((firstTime != L7_TRUE) || (searchType == EXACT))
    {
      OctetStringToIP6(pimBsrCandidateRPAddress, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimBsrCandidateRPGroupAddress, (L7_char8*)&ipAddr2);
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
    pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType = L7_AF_INET6;
  }

  charVal = pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength;
  if((searchType == EXACT) ?
    (usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT,(L7_uchar8)pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType,
                              &bsrCandtRPAddr, &bsrCandtRPGrpAddr, 
                              (L7_uchar8) pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength) != L7_SUCCESS) :
    ((usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT,(L7_uchar8)pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType,
                               &bsrCandtRPAddr, &bsrCandtRPGrpAddr,
                               (L7_uchar8)pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength) != L7_SUCCESS) &&
    (usmDbPimsmbsrCandRPEntryNextGet((L7_uchar8 )pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType, &bsrCandtRPAddr,
                                     &bsrCandtRPGrpAddr,&charVal) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType == L7_AF_INET)
      {
        memset(&bsrCandtRPAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&bsrCandtRPGrpAddr, L7_NULL, sizeof(L7_inet_addr_t));
        bsrCandtRPAddr.family = L7_AF_INET6;
        bsrCandtRPGrpAddr.family = L7_AF_INET6;
        pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType = L7_AF_INET6;
        if((usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT, pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType, 
                                     &bsrCandtRPAddr, &bsrCandtRPGrpAddr, 
                                     pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength) != L7_SUCCESS) &&
           (usmDbPimsmbsrCandRPEntryNextGet((L7_uchar8 )pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType,
                                             &bsrCandtRPAddr, &bsrCandtRPGrpAddr,&charVal) != L7_SUCCESS))
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

  pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength = charVal;
  SET_VALID(I_pimBsrCandidateRPGroupPrefixLength, pimBsrCandidateRPEntryData.valid);

  /*  converting the IP Address to Octet String */
  if(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType == D_pimBsrCandidateRPAddressType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &bsrCandtRPAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      ZERO_VALID(pimBsrCandidateRPEntryData.valid);
      return(NULL);
    }
    if (SafeMakeOctetString(&pimBsrCandidateRPEntryData.pimBsrCandidateRPAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrCandidateRPAddress, pimBsrCandidateRPEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &bsrCandtRPGrpAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrCandidateRPGroupAddress, pimBsrCandidateRPEntryData.valid);
    }
  }
  else if(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType == D_pimBsrCandidateRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr1, ipAddr2;

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &bsrCandtRPAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimBsrCandidateRPEntryData.pimBsrCandidateRPAddress,
                            (L7_uchar8 *)&ipAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrCandidateRPAddress, pimBsrCandidateRPEntryData.valid);
    }

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &bsrCandtRPGrpAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString(&pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupAddress,
                            (L7_uchar8 *)&ipAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrCandidateRPGroupAddress, pimBsrCandidateRPEntryData.valid);
    }
  }

  switch (nominator)
  {
    case -1:
    case I_pimBsrCandidateRPAddressType: /* already got it above*/
         break;
    case I_pimBsrCandidateRPAddress:
         break;
    case I_pimBsrCandidateRPGroupAddress:
         break;
    case I_pimBsrCandidateRPGroupPrefixLength:
         break;
    case I_pimBsrCandidateRPBidir:
         break;
    case I_pimBsrCandidateRPStatus:
         if (snmpPimSmpimBsrCandidateRPStatusGet(pimBsrCandidateRPEntryData.pimBsrCandidateRPAddressType, &bsrCandtRPAddr, &bsrCandtRPGrpAddr,
               pimBsrCandidateRPEntryData.pimBsrCandidateRPGroupPrefixLength, &pimBsrCandidateRPEntryData.pimBsrCandidateRPStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, pimBsrCandidateRPEntryData.valid);
         }
         break;
    default:
         /*unknown nominator */
          return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimBsrCandidateRPEntryData.valid))
    return(NULL);

   return(&pimBsrCandidateRPEntryData);

}

int
k_pimBsrCandidateRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pimBsrCandidateRPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pimBsrCandidateRPEntry_set_defaults(doList_t *dp)
{
    pimBsrCandidateRPEntry_t *data = (pimBsrCandidateRPEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_pimBsrCandidateRPEntry_set(pimBsrCandidateRPEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t bsrCandtRPAddr, bsrCandtRPGrpAddr;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
      return(COMMIT_FAILED_ERROR);

  if(data->pimBsrCandidateRPAddressType == D_pimBsrCandidateRPAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->pimBsrCandidateRPAddress->octet_ptr, data->pimBsrCandidateRPAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->pimBsrCandidateRPGroupAddress->octet_ptr, data->pimBsrCandidateRPGroupAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->pimBsrCandidateRPAddressType == D_pimBsrCandidateRPAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->pimBsrCandidateRPGroupAddress->octet_ptr, data->pimBsrCandidateRPGroupAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &bsrCandtRPAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->pimBsrCandidateRPGroupAddress->octet_ptr, data->pimBsrCandidateRPGroupAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtRPGrpAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pimBsrCandidateRPStatus, data->valid))
  {
    if (snmpPimSmpimBsrCandidateRPStatusSet(data->pimBsrCandidateRPAddressType,
                                       bsrCandtRPAddr,
                                       bsrCandtRPGrpAddr,
                                       data->pimBsrCandidateRPGroupPrefixLength,
                                       data->pimBsrCandidateRPStatus) != L7_SUCCESS )
    {
      CLR_VALID(I_pimBsrCandidateRPStatus, data->valid);
      return (COMMIT_FAILED_ERROR);
    }
  }

   return NO_ERROR;
}

pimBsrElectedBSRRPSetEntry_t *
k_pimBsrElectedBSRRPSetEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 pimBsrElectedBSRGrpMappingAddrType,
                    OctetString * pimBsrElectedBSRGrpMappingGrpAddr,
                    SR_UINT32 pimBsrElectedBSRGrpMappingGrpPrefixLen,
                    OctetString * pimBsrElectedBSRGrpMappingRPAddr)
{
  static pimBsrElectedBSRRPSetEntry_t pimBsrElectedBSRRPSetEntryData;
  L7_inet_addr_t pimMappingGrpAddr, pimMappingRPAddr;
  L7_inet_addr_t pimMappingGrpV4Addr, pimMappingGrpV6Addr, pimMappingRPV4Addr, pimMappingRPV6Addr;
  static L7_BOOL firstTime = L7_TRUE;
  L7_RC_t rc1 = L7_SUCCESS, rc2 = L7_SUCCESS;
  L7_uint32 OriginV4=0, OriginV6 = 0, LengthV4=0, LengthV6=0;
   
  L7_inet_addr_t Len4,Len6;
  static L7_BOOL initializeMem = L7_TRUE;
  static L7_uint32 Origin = 0;

  L7_uchar8 familyType=0,*origin=L7_NULLPTR,*octetIpAddr1=L7_NULLPTR,*octetIpAddr2=L7_NULLPTR;
  L7_uchar8 prefixLength = 0;

  if (initializeMem == L7_TRUE) {
    initializeMem = L7_FALSE;
    pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpAddr = MakeOctetString(NULL, 0);
    pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingRPAddr = MakeOctetString(NULL, 0);
  }


  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  /* Clear all the bits*/
  ZERO_VALID(pimBsrElectedBSRRPSetEntryData.valid);
  pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType = pimBsrElectedBSRGrpMappingAddrType;
  (void)SafeMakeOctetString(&pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpAddr, NULL, 0);
  pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen = pimBsrElectedBSRGrpMappingGrpPrefixLen;
  (void)SafeMakeOctetString(&pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingRPAddr, NULL, 0);
  SET_VALID(I_pimBsrElectedBSRGrpMappingAddrType, pimBsrElectedBSRRPSetEntryData.valid);
  SET_VALID(I_pimBsrElectedBSRGrpMappingGrpAddr, pimBsrElectedBSRRPSetEntryData.valid);
  SET_VALID(I_pimBsrElectedBSRGrpMappingGrpPrefixLen, pimBsrElectedBSRRPSetEntryData.valid);
  SET_VALID(I_pimBsrElectedBSRGrpMappingRPAddr, pimBsrElectedBSRRPSetEntryData.valid);

  if(pimBsrElectedBSRGrpMappingAddrType == L7_NULL)
    pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType = D_pimBsrElectedBSRGrpMappingAddrType_ipv4;

  if(pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType == D_pimBsrElectedBSRGrpMappingAddrType_ipv4)
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
    else if ((firstTime != L7_TRUE) || (searchType == EXACT))
    {
      ipAddr1 = OctetStringToIP(pimBsrElectedBSRGrpMappingGrpAddr);
      ipAddr2 = OctetStringToIP(pimBsrElectedBSRGrpMappingRPAddr);
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
    pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType = D_pimBsrElectedBSRGrpMappingAddrType_ipv4;
     familyType=L7_AF_INET;

    OriginV4 = Origin;
    origin = (L7_uchar8*) &OriginV4;
    LengthV4 = pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen;
    /* convert to inet address data structure*/
    inetAddressZeroSet(L7_AF_INET, &Len4);
    if( inetMaskLenToMask(familyType,LengthV4, &Len4) != L7_SUCCESS)
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
    OriginV6 = Origin;
    LengthV6 = pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen; 
     /* convert to inet address data structure*/
    memset(&Len6, L7_NULL, sizeof(L7_in6_addr_t));
    if( inetMaskLenToMask(familyType,LengthV6, &Len6) != L7_SUCCESS)
    {
       firstTime=L7_TRUE;
       return (NULL);

    }
  }
  else if(pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType == D_pimBsrElectedBSRGrpMappingAddrType_ipv6)
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
    else if ((firstTime != L7_TRUE) || (searchType == EXACT))
    {
      OctetStringToIP6(pimBsrElectedBSRGrpMappingGrpAddr, (L7_char8*)&ipAddr1);
      OctetStringToIP6(pimBsrElectedBSRGrpMappingRPAddr, (L7_char8*)&ipAddr2);
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
    pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType = D_pimBsrElectedBSRGrpMappingAddrType_ipv6;

    familyType=L7_AF_INET6;

    OriginV6 = Origin;
    origin= (L7_uchar8*) &OriginV6;
    LengthV6 = pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen;
    /* convert to inet address data structure*/
    inetAddressZeroSet(L7_AF_INET6, &Len6);
    if( inetMaskLenToMask(familyType, LengthV6, &Len6) != L7_SUCCESS)
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
    OriginV4 = Origin;
    LengthV4 = 0;
  }
  else
  {
    return L7_NULLPTR;
  }

  if(searchType == EXACT)
  {
    if((pimBsrElectedBSRGrpMappingAddrType != D_pimBsrElectedBSRGrpMappingAddrType_ipv4) &&
        (pimBsrElectedBSRGrpMappingAddrType != D_pimBsrElectedBSRGrpMappingAddrType_ipv6))
    {
      firstTime = L7_TRUE;
      return(NULL);
    }

    Origin = D_pimGroupMappingOrigin_bsr - 1; /* In new RFC the BSR is placed one ahead */

    if (pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType == D_pimBsrElectedBSRGrpMappingAddrType_ipv4)
    {
      if(usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                        familyType,
                                        Origin,
                                        &pimMappingGrpV4Addr,
                                        &Len4,
                                        &pimMappingRPV4Addr) != L7_SUCCESS)
      {
        firstTime = L7_TRUE;
        Origin  = 0;
        return(NULL);
      }
    }

    /*ipv6*/
    if (pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType == D_pimBsrElectedBSRGrpMappingAddrType_ipv6)
    {
      if(usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                        familyType,
                                        Origin,
                                        &pimMappingGrpV6Addr,
                                        &Len6,
                                        &pimMappingRPV6Addr) != L7_SUCCESS)
      {
        firstTime = L7_TRUE;
        Origin  = 0;
        return(NULL);

      }

    }

  }
  else if(searchType == NEXT)
  {
    if(pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType >= D_pimBsrElectedBSRGrpMappingAddrType_ipv6)
    {
      *origin=OriginV4 = Origin + 1;
    }
    else if(pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType <= D_pimBsrElectedBSRGrpMappingAddrType_ipv6)
    {
      *origin=OriginV4 = Origin;
    }

    familyType = L7_AF_INET;
    if((usmDbPimsmRpGrpMappingEntryGet(USMDB_UNIT_CURRENT,
                                       familyType,
                                       Origin,
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
                                       Origin,
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
      Origin = OriginV4;

      /* convert from  inet_addressto l7_uint32 */
      if (inetMaskToMaskLen(&Len4, &prefixLength) != L7_SUCCESS)
      {
        LengthV4=0;
        firstTime=L7_TRUE;
      }
      else
      {
        LengthV4 =(L7_uint32) prefixLength;
      }
      pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen = LengthV4;
      pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType =D_pimBsrElectedBSRGrpMappingAddrType_ipv4;
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_SUCCESS))
    {
      Origin = OriginV6;
      /* convert from  inet_addressto l7_uint32 */
      if (inetMaskToMaskLen(&Len6, &prefixLength) != L7_SUCCESS)
      {
        LengthV6=0;
        firstTime=L7_TRUE;
      }
      else
      {
        LengthV6= (L7_uint32) prefixLength;
      }
      pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen = LengthV6;
      pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType = D_pimBsrElectedBSRGrpMappingAddrType_ipv6;
      inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
      inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);
    }
    else if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
    {
      L7_int32 ret;

      ret = pimGrpMappingTableIndexCompare(OriginV4, OriginV6);

      if(ret == L7_AF_INET)
      {
        Origin = OriginV4;
        /* convert from  inet_addressto l7_uint32 */
        if (inetMaskToMaskLen(&Len4, &prefixLength) != L7_SUCCESS)
        {
          LengthV4=0;
          firstTime=L7_TRUE;
        }
        else
        {
          LengthV4 =(L7_uint32) prefixLength;
        }
        pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen = LengthV4;
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);
      }
      if(ret == L7_AF_INET6)
      {
        Origin = OriginV6;
        /* convert from  inet_addressto l7_uint32 */
        if (inetMaskToMaskLen(&Len6, &prefixLength) != L7_SUCCESS)
        {
          LengthV6=0;
          firstTime=L7_TRUE;
        }
        else
        {
          LengthV6 = (L7_uint32) prefixLength;
        }
        pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen = LengthV6;
        inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
        inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);
      }
    }
    else if((rc1 == L7_FAILURE) && (rc2 == L7_FAILURE))
    {
      firstTime = L7_TRUE;
      Origin  = 0;
      return(NULL);
    }
  }

  /*  converting the IP Address to Octet String */
  if(pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType == D_pimBsrElectedBSRGrpMappingAddrType_ipv4)
  {
    L7_uint32 ipAddr1, ipAddr2;

    inetCopy(&pimMappingGrpAddr, &pimMappingGrpV4Addr);
    inetCopy(&pimMappingRPAddr, &pimMappingRPV4Addr);

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
     octetIpAddr1=(L7_uchar8 *)&ipAddr1;
    if (SafeMakeOctetString(&pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpAddr,
                            octetIpAddr1, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrElectedBSRGrpMappingGrpAddr, pimBsrElectedBSRRPSetEntryData.valid);
    }
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
    octetIpAddr2=(L7_uchar8 *)&ipAddr2;
    if (SafeMakeOctetString(&pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingRPAddr,
                            octetIpAddr2, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrElectedBSRGrpMappingRPAddr, pimBsrElectedBSRRPSetEntryData.valid);
    }
  }
  else if(pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType == D_pimBsrElectedBSRGrpMappingAddrType_ipv6)
  {
    L7_uint32 ipAddr1, ipAddr2;

    inetCopy(&pimMappingGrpAddr, &pimMappingGrpV6Addr);
    inetCopy(&pimMappingRPAddr, &pimMappingRPV6Addr);

    if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingGrpAddr, &ipAddr1))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
     octetIpAddr1=(L7_uchar8 *)&ipAddr1;
    if (SafeMakeOctetString(&pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpAddr,
                            octetIpAddr1, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrElectedBSRGrpMappingGrpAddr, pimBsrElectedBSRRPSetEntryData.valid);
    }

   if(L7_SUCCESS != inetAddressGet(L7_AF_INET6, &pimMappingRPAddr, &ipAddr2))
    {
      firstTime =  L7_TRUE;
      return(NULL);
    }
     octetIpAddr2= (L7_uchar8 *)&ipAddr2;

    if (SafeMakeOctetString(&pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingRPAddr,
                            octetIpAddr2, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_pimBsrElectedBSRGrpMappingRPAddr, pimBsrElectedBSRRPSetEntryData.valid);
    }
  }
  switch (nominator)
  {
    case -1:
    case I_pimBsrElectedBSRGrpMappingAddrType:
    case I_pimBsrElectedBSRGrpMappingGrpAddr:
    case I_pimBsrElectedBSRGrpMappingGrpPrefixLen:
    case I_pimBsrElectedBSRGrpMappingRPAddr:
         break;
    case I_pimBsrElectedBSRRPSetPriority:
         if(usmDbPimsmbsrRPSetPriorityGet(Origin, pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType,
                           &pimMappingGrpAddr, pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen, &pimMappingRPAddr,
                           &pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRRPSetPriority) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrElectedBSRRPSetEntryData.valid);
         }
        break;
    case I_pimBsrElectedBSRRPSetHoldtime:
         if(usmDbPimsmbsrRPSetHoldtimeGet(Origin, pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType,
                           &pimMappingGrpAddr, pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen, &pimMappingRPAddr,
                           &pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRRPSetHoldtime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrElectedBSRRPSetEntryData.valid);
         }
        break;
    case I_pimBsrElectedBSRRPSetExpiryTime:
         if(usmDbPimsmbsrRPSetExpiryTimeGet(Origin, pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingAddrType,
                           &pimMappingGrpAddr, pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRGrpMappingGrpPrefixLen, &pimMappingRPAddr,
                           &pimBsrElectedBSRRPSetEntryData.pimBsrElectedBSRRPSetExpiryTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrElectedBSRRPSetEntryData.valid);
         }
        break;
    default:
        /* unknown nominator */
            return(NULL);
    }

  if (nominator >= 0 && !VALID(nominator, pimBsrElectedBSRRPSetEntryData.valid))
    return(NULL);
  return(&pimBsrElectedBSRRPSetEntryData);

}

pimBsrCandidateBSREntry_t *
k_pimBsrCandidateBSREntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 pimBsrCandidateBSRZoneIndex)
{
  static pimBsrCandidateBSREntry_t pimBsrCandidateBSREntryData;
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
    pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddress = MakeOctetString(NULL, 0);
  }
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    (void)SafeMakeOctetString(&pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddress, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimBsrCandidateBSREntryData.valid);
  pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex = pimBsrCandidateBSRZoneIndex;
  SET_VALID(pimBsrCandidateBSRZoneIndex, pimBsrCandidateBSREntryData.valid);

  if(pimBsrCandidateBSRZoneIndex == L7_NULL)
    pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex = L7_AF_INET;

  if(pimBsrCandidateBSRZoneIndex == D_pimBsrCandidateBSRAddressType_ipv4)
  {
    pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex = L7_AF_INET;
  }
  else if(pimBsrCandidateBSRZoneIndex == D_pimBsrCandidateBSRAddressType_ipv6)
  {
    pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex = L7_AF_INET6;
  }

  if((searchType == EXACT) ?
    (usmDbPimsmCandBSREntryGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex) != L7_SUCCESS) :
    ((usmDbPimsmCandBSREntryGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex) != L7_SUCCESS) &&
    (usmDbPimsmCandBSREntryNextGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex == L7_AF_INET)
      {
        pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex = L7_AF_INET6;
        if((usmDbPimsmCandBSREntryGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex) != L7_SUCCESS) &&
           (usmDbPimsmCandBSREntryNextGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex) != L7_SUCCESS))
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
    case I_pimBsrCandidateBSRZoneIndex:
         break; 
    case I_pimBsrCandidateBSRAddressType: /* already got it above*/
         if(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex == D_pimSGAddressType_ipv4)
         {
           pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType = D_pimSGAddressType_ipv4;
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
         else if(pimBsrCandidateBSREntryData.pimBsrCandidateBSRZoneIndex == D_pimSGAddressType_ipv6)
         {
           pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType = D_pimSGAddressType_ipv6;
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
         break;
    case I_pimBsrCandidateBSRAddress:
         if(usmDbPimsmCandBSRAddrGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType,
                           &bsrCandtBSRAddr) == L7_SUCCESS)
         {
             if(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType == D_pimSGAddressType_ipv4)
             {
               L7_uint32 ipAddr;

               if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &bsrCandtBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
                 }
               }
             }
             else if(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType == D_pimSGAddressType_ipv6)
             {
               L7_in6_addr_t ipAddr;

               if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &bsrCandtBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
                 }
               }
             }
         }
         break;
    case I_pimBsrCandidateBSRPriority:
         if(usmDbPimsmCandBSRPriorityGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType,
                           &pimBsrCandidateBSREntryData.pimBsrCandidateBSRPriority) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
         break;
    case I_pimBsrCandidateBSRHashMaskLength:
         if(usmDbPimsmCandBSRHashMaskLengthGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType,
                           &pimBsrCandidateBSREntryData.pimBsrCandidateBSRHashMaskLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
         break;
    case I_pimBsrCandidateBSRElectedBSR:
         if(usmDbPimsmCandBSRElectedBSRGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType,
                           &pimBsrCandidateBSREntryData.pimBsrCandidateBSRElectedBSR) == L7_SUCCESS)
         {
           if (pimBsrCandidateBSREntryData.pimBsrCandidateBSRElectedBSR != L7_TRUE)
           {
               pimBsrCandidateBSREntryData.pimBsrCandidateBSRElectedBSR = D_pimBsrCandidateBSRElectedBSR_false;
           }
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
       break;
    case I_pimBsrCandidateBSRBootstrapTimer:
         if(usmDbPimsmCandBSRBootstrapTimerGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType,
                           &pimBsrCandidateBSREntryData.pimBsrCandidateBSRBootstrapTimer) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
               break;
    case I_pimBsrCandidateBSRStatus:
         if (snmpPimSmpimBsrCandidateBSRStatusGet(pimBsrCandidateBSREntryData.pimBsrCandidateBSRAddressType,
                         &pimBsrCandidateBSREntryData.pimBsrCandidateBSRStatus) == L7_SUCCESS )
         {
           SET_VALID(nominator, pimBsrCandidateBSREntryData.valid);
         }
       break;
    default:
         /* unknown nominator */
          return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimBsrCandidateBSREntryData.valid))
    return(NULL);

   return(&pimBsrCandidateBSREntryData);
}

int
k_pimBsrCandidateBSREntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pimBsrCandidateBSREntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pimBsrCandidateBSREntry_set_defaults(doList_t *dp)
{
    pimBsrCandidateBSREntry_t *data = (pimBsrCandidateBSREntry_t *) (dp->data);

    if ((data->pimBsrCandidateBSRAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->pimBsrCandidateBSRPriority = L7_PIMSM_CBSR_PRIORITY;
    data->pimBsrCandidateBSRHashMaskLength = L7_PIMSM_CBSR_HASH_MASK_LENGTH;
    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_pimBsrCandidateBSREntry_set(pimBsrCandidateBSREntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t bsrCandtBSRAddr;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
      return(COMMIT_FAILED_ERROR);

  if(data->pimBsrCandidateBSRAddressType == D_pimBsrCandidateBSRAddressType_ipv4)
  {
    L7_uint32 ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_uint32));
    memcpy(&ipAddr, data->pimBsrCandidateBSRAddress->octet_ptr, data->pimBsrCandidateBSRAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &bsrCandtBSRAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  else if(data->pimBsrCandidateBSRAddressType == D_pimBsrCandidateBSRAddressType_ipv6)
  {
    L7_in6_addr_t ipAddr;

    memset(&ipAddr, L7_NULL, sizeof(L7_in6_addr_t));
    memcpy(&ipAddr, data->pimBsrCandidateBSRAddress->octet_ptr, data->pimBsrCandidateBSRAddress->length);

    if(L7_SUCCESS != inetAddressSet(L7_AF_INET6, &ipAddr, &bsrCandtBSRAddr))
    {
        return(COMMIT_FAILED_ERROR);
    }
  }
  if (VALID(I_pimBsrCandidateBSRStatus, data->valid) ||
      VALID(I_pimBsrCandidateBSRAddress, data->valid))
  {
    if (data->pimBsrCandidateBSRStatus == D_pimBsrCandidateBSRStatus_createAndGo)
    {
      if (!VALID(I_pimBsrCandidateBSRAddress, data->valid))
         return(COMMIT_FAILED_ERROR);
    }
    else if (data->pimBsrCandidateBSRStatus == D_pimBsrCandidateBSRStatus_destroy)
    {
      if (usmDbPimsmCandBSREntryGet(data->pimBsrCandidateBSRAddressType) == L7_SUCCESS)
      {
        (void)usmDbPimsmCandBSRAddrGet(data->pimBsrCandidateBSRAddressType,
                           &bsrCandtBSRAddr);
      }
    }
    if (snmpPimSmpimBsrCandidateBSRSet(data->pimBsrCandidateBSRAddressType,
                                          data->pimBsrCandidateBSRStatus, bsrCandtBSRAddr,
                                          data->pimBsrCandidateBSRPriority,
                                          data->pimBsrCandidateBSRHashMaskLength) != L7_SUCCESS)
    {
            CLR_VALID(I_pimBsrCandidateBSRStatus, data->valid);
            return (COMMIT_FAILED_ERROR);
    }
  }
  if ( (VALID(I_pimBsrCandidateBSRPriority, data->valid) ||
      VALID(I_pimBsrCandidateBSRHashMaskLength, data->valid)) &&
       data->pimBsrCandidateBSRStatus == D_pimBsrCandidateBSRStatus_active)
  {

    if (usmDbPimsmCandBSREntryGet(data->pimBsrCandidateBSRAddressType) == L7_SUCCESS)
    {
      (void)usmDbPimsmCandBSRAddrGet(data->pimBsrCandidateBSRAddressType,
                          &bsrCandtBSRAddr);
    }
    if (snmpPimSmpimBsrCandidateBSRSet(data->pimBsrCandidateBSRAddressType,
                                          data->pimBsrCandidateBSRStatus, bsrCandtBSRAddr,
                                          data->pimBsrCandidateBSRPriority,
                                          data->pimBsrCandidateBSRHashMaskLength) != L7_SUCCESS)
    {
            return (COMMIT_FAILED_ERROR);
    }

  }


  return NO_ERROR;
}

pimBsrElectedBSREntry_t *
k_pimBsrElectedBSREntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_UINT32 pimBsrElectedBSRZoneIndex)
{
  static pimBsrElectedBSREntry_t pimBsrElectedBSREntryData;
  L7_inet_addr_t pimBsrElectedBSRAddr;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL initializeMem = L7_TRUE;

  if ((pimBsrElectedBSRZoneIndex != L7_NULL) &&
      (pimBsrElectedBSRZoneIndex != L7_AF_INET) &&
      (pimBsrElectedBSRZoneIndex != L7_AF_INET6))
    return(NULL);

  /* To check whether PIMSM component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    pimBsrElectedBSREntryData.pimBsrElectedBSRAddress = MakeOctetString(NULL, 0);
  }
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    (void)SafeMakeOctetString(&pimBsrElectedBSREntryData.pimBsrElectedBSRAddress, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID(pimBsrElectedBSREntryData.valid);
  pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex = pimBsrElectedBSRZoneIndex;
  SET_VALID(pimBsrElectedBSRZoneIndex, pimBsrElectedBSREntryData.valid);

  if(pimBsrElectedBSRZoneIndex == L7_NULL)
    pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex = L7_AF_INET;


  if((searchType == EXACT) ?
    (usmDbPimsmbsrElectedBSREntryGet(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex) != L7_SUCCESS) :
    ((usmDbPimsmbsrElectedBSREntryGet(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex) != L7_SUCCESS) &&
    (usmDbPimsmbsrElectedBSREntryNextGet(&pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex == L7_AF_INET6)
      {
        firstTime = L7_TRUE;
        return(NULL);
      }
      else if(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex == L7_AF_INET)
      {
        pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex = L7_AF_INET6;
        if((usmDbPimsmbsrElectedBSREntryGet(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex) != L7_SUCCESS) &&
           (usmDbPimsmbsrElectedBSREntryNextGet(&pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex) != L7_SUCCESS))
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
    case I_pimBsrElectedBSRZoneIndex:
         break;
    case I_pimBsrElectedBSRAddressType: /* already got it above*/
         if(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex == D_pimSGAddressType_ipv4)
         {
           pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType = D_pimSGAddressType_ipv4;
           SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
         }
         else if(pimBsrElectedBSREntryData.pimBsrElectedBSRZoneIndex == D_pimSGAddressType_ipv6)
         {
           pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType = D_pimSGAddressType_ipv6;
           SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
         } 
         break;
    case I_pimBsrElectedBSRAddress:
         if(usmDbPimsmbsrElectedBSRAddrGet(pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType,
                           (L7_uint32 *)&pimBsrElectedBSRAddr) == L7_SUCCESS)
         {
             if(pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType == D_pimSGAddressType_ipv4)
             {
               L7_uint32 ipAddr;

               if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &pimBsrElectedBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimBsrElectedBSREntryData.pimBsrElectedBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
                 }
               }
             }
             else if(pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType == D_pimSGAddressType_ipv6)
             {
               L7_in6_addr_t ipAddr;

               if(L7_SUCCESS == inetAddressGet(L7_AF_INET6, &pimBsrElectedBSRAddr, &ipAddr))
               {
                 if (SafeMakeOctetString(&pimBsrElectedBSREntryData.pimBsrElectedBSRAddress,
                                         (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
                 {
                   SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
                 }
               }
             }
         }
         break;
    case I_pimBsrElectedBSRPriority:
         if(usmDbPimsmbsrElectedBSRPriorityGet(pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType,
                           &pimBsrElectedBSREntryData.pimBsrElectedBSRPriority) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
         }
               break;
    case I_pimBsrElectedBSRHashMaskLength:
         if(usmDbPimsmbsrElectedBSRHashMaskLengthGet(pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType,
                           &pimBsrElectedBSREntryData.pimBsrElectedBSRHashMaskLength) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
         }
               break;
    case I_pimBsrElectedBSRExpiryTime:
         if(usmDbPimsmbsrElectedBSRExpiryTimeGet(pimBsrElectedBSREntryData.pimBsrElectedBSRAddressType,
                           &pimBsrElectedBSREntryData.pimBsrElectedBSRExpiryTime) == L7_SUCCESS)
         {
           SET_VALID(nominator, pimBsrElectedBSREntryData.valid);
         }
               break;
    default:
         /* unknown nominator */
          return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, pimBsrElectedBSREntryData.valid))
    return(NULL);

   return(&pimBsrElectedBSREntryData);
}


