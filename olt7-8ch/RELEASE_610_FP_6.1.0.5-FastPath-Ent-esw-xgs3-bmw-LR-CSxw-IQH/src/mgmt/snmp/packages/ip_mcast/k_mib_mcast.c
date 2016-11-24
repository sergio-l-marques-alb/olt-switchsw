/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_mcast.c
*
* @purpose RFC2932 MIB
*
* @component SNMP
*
* @comments
*
* @create
*
* @author Nitish Kumar
*
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_mcast_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_mcast_api.h"
#endif


ipMRoute_t *
k_ipMRoute_get(int serialNum, ContextInfo *contextInfo,
               int nominator)
{
   static ipMRoute_t ipMRouteData;

   /*clear all bits*/
   ZERO_VALID(ipMRouteData.valid);

   switch (nominator)
   {
   case -1:
   break;

   case I_ipMRouteEnable:
    if (snmpMcastRoutingStatusGet(USMDB_UNIT_CURRENT, &ipMRouteData.ipMRouteEnable) == L7_SUCCESS )
      SET_VALID(I_ipMRouteEnable, ipMRouteData.valid);
    break;

   case I_ipMRouteEntryCount:
    if (usmDbMcastIpMRouteEntryCountGet(USMDB_UNIT_CURRENT, &ipMRouteData.ipMRouteEntryCount) == L7_SUCCESS)
      SET_VALID(I_ipMRouteEntryCount, ipMRouteData.valid);

   break;
    default:
     /* unknown nominator */
      return(NULL);
    break;
   }

   if (nominator >= 0 && !VALID(nominator, ipMRouteData.valid))
      return(NULL);

   return(&ipMRouteData);
}


#ifdef SETS
int
k_ipMRoute_test(ObjectInfo *object, ObjectSyntax *value,
                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ipMRoute_ready(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRoute_set(ipMRoute_t *data,
               ContextInfo *contextInfo, int function)
{

  if (VALID(I_ipMRouteEnable, data->valid) &&
      snmpMcastRoutingStatusSet(USMDB_UNIT_CURRENT,data->ipMRouteEnable) != L7_SUCCESS)
  {
    CLR_VALID(I_ipMRouteEnable, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;

}

#ifdef SR_ipMRoute_UNDO
/* add #define SR_ipMRoute_UNDO in sitedefs.h to
 * include the undo routine for the ipMRoute family.
 */
int
ipMRoute_undo(doList_t *doHead, doList_t *doCur,
              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipMRoute_UNDO */

#endif /* SETS */


ipMRouteEntry_t *
k_ipMRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_UINT32 ipMRouteGroup,
                    SR_UINT32 ipMRouteSource)
{
  static ipMRouteEntry_t ipMRouteEntryData;
  L7_uint32 ipMRouteSourceMask = 32;
  L7_uint32 tmpIntIfNum = 0;
  L7_inet_addr_t inetMRtGrp, inetMRtSrc, inetSrcMask,upsNbr,rtAddr,rtMask;
  L7_uchar8 addrFamily = L7_AF_INET;

  ZERO_VALID(ipMRouteEntryData.valid);

  ipMRouteEntryData.ipMRouteGroup = ipMRouteGroup;
  ipMRouteEntryData.ipMRouteSource = ipMRouteSource;
  ipMRouteEntryData.ipMRouteSourceMask = ipMRouteSourceMask;

  SET_VALID(I_ipMRouteGroup, ipMRouteEntryData.valid);
  SET_VALID(I_ipMRouteSource, ipMRouteEntryData.valid);
  SET_VALID(I_ipMRouteSourceMask, ipMRouteEntryData.valid);

  if(L7_SUCCESS != inetAddressSet(addrFamily, &ipMRouteGroup, &inetMRtGrp))
    return (NULL);
  if(L7_SUCCESS != inetAddressSet(addrFamily, &ipMRouteSource, &inetMRtSrc))
    return (NULL);
  if(L7_SUCCESS != inetAddressSet(addrFamily, &ipMRouteSourceMask, &inetSrcMask))
    return (NULL);

  if ((searchType == EXACT) ?
      (usmDbMcastIpMRouteEntryGet(addrFamily, USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc, &inetSrcMask) != L7_SUCCESS) :
      ((usmDbMcastIpMRouteEntryGet(addrFamily, USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc, &inetSrcMask) != L7_SUCCESS) &&
       (usmDbMcastIpMRouteEntryNextGet(addrFamily, USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc, &inetSrcMask) != L7_SUCCESS)))
  {
    return(NULL);
  }

  if(L7_SUCCESS != inetAddressGet(addrFamily, &inetMRtGrp, &ipMRouteEntryData.ipMRouteGroup))
    return (NULL);
  if(L7_SUCCESS != inetAddressGet(addrFamily, &inetMRtSrc, &ipMRouteEntryData.ipMRouteSource))
    return (NULL);
  if(L7_SUCCESS != inetAddressGet(addrFamily, &inetSrcMask, &ipMRouteEntryData.ipMRouteSourceMask))
    return (NULL);

  switch (nominator)
  {
  case -1:
  case I_ipMRouteGroup: /* already got it above*/
  case I_ipMRouteSource: /* already got it above*/
  case I_ipMRouteSourceMask: /* already got it above*/
      break;

  case I_ipMRouteUpstreamNeighbor:
      if( (usmDbMcastIpMRouteUpstreamNbrGet(addrFamily, USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc,
                              &inetSrcMask,&upsNbr) == L7_SUCCESS) &&
          (L7_SUCCESS == inetAddressGet(addrFamily, &upsNbr, &ipMRouteEntryData.ipMRouteUpstreamNeighbor)))


      SET_VALID(I_ipMRouteUpstreamNeighbor, ipMRouteEntryData.valid);
      break;

  case I_ipMRouteInIfIndex:
      if (usmDbMcastIpMRouteIfIndexGet(addrFamily, USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc,
                              &inetSrcMask, &tmpIntIfNum) == L7_SUCCESS){

          usmDbExtIfNumFromIntIfNum(tmpIntIfNum,&ipMRouteEntryData.ipMRouteInIfIndex);
          SET_VALID(I_ipMRouteInIfIndex, ipMRouteEntryData.valid);
       }
      break;

  case I_ipMRouteUpTime:
      if (snmpIpMRouteUpTimeGet(USMDB_UNIT_CURRENT, ipMRouteEntryData.ipMRouteGroup, ipMRouteEntryData.ipMRouteSource,
                              ipMRouteEntryData.ipMRouteSourceMask, &ipMRouteEntryData.ipMRouteUpTime) == L7_SUCCESS)
      SET_VALID(I_ipMRouteUpTime, ipMRouteEntryData.valid);
      break;

  case I_ipMRouteExpiryTime:
      if (snmpIpMRouteExpiryGet(USMDB_UNIT_CURRENT, ipMRouteEntryData.ipMRouteGroup, ipMRouteEntryData.ipMRouteSource,
                              ipMRouteEntryData.ipMRouteSourceMask, &ipMRouteEntryData.ipMRouteExpiryTime) == L7_SUCCESS)
      SET_VALID(I_ipMRouteExpiryTime, ipMRouteEntryData.valid);
      break;

#ifdef NOT_SUPPORTED
  case I_ipMRoutePkts:
      if (usmDbMcastIpMRoutePktsGet(USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc,
                              &inetSrcMask, &ipMRouteEntryData.ipMRoutePkts) == L7_SUCCESS)
      SET_VALID(I_ipMRoutePkts, ipMRouteEntryData.valid);
      break;

  case I_ipMRouteDifferentInIfPackets:
      break;

  case I_ipMRouteOctets:
      break;
#endif /* NOT_SUPPORTED */

  case I_ipMRouteProtocol:
      if (snmpIpMRouteProtocolGet(USMDB_UNIT_CURRENT, ipMRouteEntryData.ipMRouteGroup, ipMRouteEntryData.ipMRouteSource,
                              ipMRouteEntryData.ipMRouteSourceMask, &ipMRouteEntryData.ipMRouteProtocol) == L7_SUCCESS)
      SET_VALID(I_ipMRouteProtocol, ipMRouteEntryData.valid);
      break;

  case I_ipMRouteRtProto:
      if (usmDbMcastIpMRouteRtProtoGet(USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc,
                              &inetSrcMask, &ipMRouteEntryData.ipMRouteRtProto) == L7_SUCCESS)
      SET_VALID(I_ipMRouteRtProto, ipMRouteEntryData.valid);
      break;

   case I_ipMRouteRtAddress:

      if( (usmDbMcastIpMRouteRtAddressGet(USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc, &inetSrcMask,&rtAddr ) == L7_SUCCESS)&&
            (L7_SUCCESS == inetAddressGet(addrFamily, &rtAddr, &ipMRouteEntryData.ipMRouteRtAddress)))
      SET_VALID(I_ipMRouteRtAddress, ipMRouteEntryData.valid);
      break;

  case I_ipMRouteRtMask:
      if( (usmDbMcastIpMRouteRtMaskGet(USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc,
                              &inetSrcMask,&rtMask) == L7_SUCCESS)&&
          (L7_SUCCESS == inetAddressGet(addrFamily, &rtMask, &ipMRouteEntryData.ipMRouteRtMask)))

                   SET_VALID(I_ipMRouteRtMask, ipMRouteEntryData.valid);
      break;

  case I_ipMRouteRtType:
      if (usmDbMcastIpMRouteRtTypeGet(USMDB_UNIT_CURRENT, &inetMRtGrp, &inetMRtSrc,
                              &inetSrcMask, &ipMRouteEntryData.ipMRouteRtType) == L7_SUCCESS)
          SET_VALID(I_ipMRouteRtType, ipMRouteEntryData.valid);
      break;

/*   case I_ipMRouteHCOctets: <Not supported>  */

  default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, ipMRouteEntryData.valid))
    return(NULL);

  return(&ipMRouteEntryData);
}

ipMRouteNextHopEntry_t *
k_ipMRouteNextHopEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 ipMRouteNextHopGroup,
                           SR_UINT32 ipMRouteNextHopSource,
                           SR_UINT32 ipMRouteNextHopSourceMask,
                           SR_INT32 ipMRouteNextHopIfIndex,
                           SR_UINT32 ipMRouteNextHopAddress)
{

   static ipMRouteNextHopEntry_t ipMRouteNextHopEntryData;
   L7_uint32 intIfNum;
   L7_inet_addr_t inetMRtGrp, inetMRtSrc, inetSrcMask, inetNextHopAddr;


   ZERO_VALID(ipMRouteNextHopEntryData.valid);
   ipMRouteNextHopEntryData.ipMRouteNextHopGroup = ipMRouteNextHopGroup;
   ipMRouteNextHopEntryData.ipMRouteNextHopSource = ipMRouteNextHopSource;
   ipMRouteNextHopEntryData.ipMRouteNextHopSourceMask = ipMRouteNextHopSourceMask;
   ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex = ipMRouteNextHopIfIndex;
   ipMRouteNextHopEntryData.ipMRouteNextHopAddress = ipMRouteNextHopAddress;
   SET_VALID(I_ipMRouteNextHopGroup, ipMRouteNextHopEntryData.valid);
   SET_VALID(I_ipMRouteNextHopSource, ipMRouteNextHopEntryData.valid);
   SET_VALID(I_ipMRouteNextHopSourceMask, ipMRouteNextHopEntryData.valid);
   SET_VALID(I_ipMRouteNextHopIfIndex, ipMRouteNextHopEntryData.valid);
   SET_VALID(I_ipMRouteNextHopAddress, ipMRouteNextHopEntryData.valid);

   inetAddressReset(&inetMRtGrp);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopGroup, &inetMRtGrp))
     return (NULL);
   inetAddressReset(&inetMRtSrc);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopSource, &inetMRtSrc))
     return (NULL);
   inetAddressReset(&inetSrcMask);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopSourceMask, &inetSrcMask))
     return (NULL);
   inetAddressReset(&inetNextHopAddr);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopAddress, &inetNextHopAddr))
     return (NULL);

   if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS) ||
         (usmDbMcastIpMRouteNextHopEntryGet(USMDB_UNIT_CURRENT, &inetMRtGrp,&inetMRtSrc,&inetSrcMask, intIfNum, &inetNextHopAddr) != L7_SUCCESS) ) :

       ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                         ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                      ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex, 
                                      &ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS) ||
         ((usmDbMcastIpMRouteNextHopEntryGet(USMDB_UNIT_CURRENT, &inetMRtGrp,&inetMRtSrc,&inetSrcMask, intIfNum, &inetNextHopAddr) != L7_SUCCESS) &&
         (usmDbMcastIpMRouteNextHopEntryNextGet(USMDB_UNIT_CURRENT, &inetMRtGrp,&inetMRtSrc,&inetSrcMask, intIfNum, &inetNextHopAddr) != L7_SUCCESS)) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex) != L7_SUCCESS) )
       )
   {
       return(NULL);
   }

   switch (nominator)
   {
   case -1:
   case I_ipMRouteNextHopGroup: /* already got it above*/
   case I_ipMRouteNextHopSource: /* already got it above*/
   case I_ipMRouteNextHopSourceMask: /* already got it above*/            
   case I_ipMRouteNextHopIfIndex: /* already got it above*/
   case I_ipMRouteNextHopAddress: /* already got it above*/            
       break;
   
   case I_ipMRouteNextHopState:
       if (usmDbMcastIpMRouteNextHopStateGet(USMDB_UNIT_CURRENT, &inetMRtGrp,&inetMRtSrc,&inetSrcMask, intIfNum, 
            &inetNextHopAddr, &ipMRouteNextHopEntryData.ipMRouteNextHopState) == L7_SUCCESS)
       SET_VALID(I_ipMRouteNextHopState, ipMRouteNextHopEntryData.valid);   
       break;
   
/*   case I_ipMRouteNextHopUpTime:   <Not supported> */

/*   case I_ipMRouteNextHopExpiryTime: <Not supported> */

/*   case I_ipMRouteNextHopClosestMemberHops: <Not supported> */
   
/*   case I_ipMRouteNextHopProtocol: <Not supported> */
   
/*   case I_ipMRouteNextHopPkts: <Not supported>*/
   
   default:
     /* unknown nominator */
     return(NULL);
     break;
  } /* end of switch(nominator)  */
   
   
   if (nominator >= 0 && !VALID(nominator, ipMRouteNextHopEntryData.valid))
      return(NULL);

   return(&ipMRouteNextHopEntryData);
}

ipMRouteInterfaceEntry_t *
k_ipMRouteInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 ipMRouteInterfaceIfIndex)
{

   static ipMRouteInterfaceEntry_t ipMRouteInterfaceEntryData;
   L7_uint32 intIfNum;
   static L7_BOOL firstTime = L7_TRUE;

   if (firstTime == L7_TRUE) {
     ipMRouteInterfaceEntryData.ipMRouteInterfaceHCInMcastOctets = MakeCounter64(0);
     ipMRouteInterfaceEntryData.ipMRouteInterfaceHCOutMcastOctets = MakeCounter64(0);
     firstTime = L7_FALSE;
   }
   ZERO_VALID(ipMRouteInterfaceEntryData.valid);
   ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex = ipMRouteInterfaceIfIndex;   
   SET_VALID(I_ipMRouteInterfaceIfIndex, ipMRouteInterfaceEntryData.valid);

   if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
         (usmDbMcastIpMRouteInterfaceEntryGet(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS) ) :

       ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                         ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                      ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex, 
                                      &ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
         ( (usmDbMcastIpMRouteInterfaceEntryGet(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS) &&
           (usmDbMcastIpMRouteInterfaceEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum) != L7_SUCCESS)) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex) != L7_SUCCESS) )
       )
   {
       return(NULL);
   }

   switch (nominator)
   {
   case -1:
   case I_ipMRouteInterfaceIfIndex: /* already got it above*/
       break;
   
   case I_ipMRouteInterfaceTtl:
       if (usmDbMcastIpMRouteInterfaceTtlGet(USMDB_UNIT_CURRENT, intIfNum, 
                            &ipMRouteInterfaceEntryData.ipMRouteInterfaceTtl) == L7_SUCCESS)
       SET_VALID(I_ipMRouteInterfaceTtl, ipMRouteInterfaceEntryData.valid);   
       break;
   
   case I_ipMRouteInterfaceProtocol:
       if (snmpIpMRouteInterfaceProtocolGet(USMDB_UNIT_CURRENT, intIfNum,
                            &ipMRouteInterfaceEntryData.ipMRouteInterfaceProtocol) == L7_SUCCESS)
       SET_VALID(I_ipMRouteInterfaceProtocol, ipMRouteInterfaceEntryData.valid);   
       break;
   
/*   case I_ipMRouteInterfaceRateLimit: <Not supported> */
   
/*   case I_ipMRouteInterfaceInMcastOctets: <Not supported> */
   
/*   case I_ipMRouteInterfaceOutMcastOctets: <Not supported> */

/*   case I_ipMRouteInterfaceHCInMcastOctets: <Not supported> */
   
/*   case I_ipMRouteInterfaceHCOutMcastOctets: <Not supported> */

   default:
       /* unknown nominator */
       return(NULL);
       break;
   } /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ipMRouteInterfaceEntryData.valid))
      return(NULL);
   
   return(&ipMRouteInterfaceEntryData);
}

#ifdef SETS
int
k_ipMRouteInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ipMRouteInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRouteInterfaceEntry_set_defaults(doList_t *dp)
{
    ipMRouteInterfaceEntry_t *data = (ipMRouteInterfaceEntry_t *) (dp->data);

    data->ipMRouteInterfaceRateLimit = 0;
    data->ipMRouteInterfaceInMcastOctets = (SR_UINT32) 0;
    data->ipMRouteInterfaceOutMcastOctets = (SR_UINT32) 0;
    data->ipMRouteInterfaceHCInMcastOctets = (SR_UINT32) 0;
    data->ipMRouteInterfaceHCOutMcastOctets = (SR_UINT32) 0;

    SET_VALID(I_ipMRouteInterfaceTtl, data->valid);
    return NO_ERROR;
}

int
k_ipMRouteInterfaceEntry_set(ipMRouteInterfaceEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

        /* translate the extIntnum to internal interface num*/
    if (usmDbIntIfNumFromExtIfNum(data->ipMRouteInterfaceIfIndex, &intIfNum) != L7_SUCCESS )
    {
          ZERO_VALID(data->valid);
          return(COMMIT_FAILED_ERROR);
    }
    
    if (VALID(I_ipMRouteInterfaceTtl, data->valid) &&
        usmDbMcastIpMRouteInterfaceTtlSet(USMDB_UNIT_CURRENT, intIfNum, data->ipMRouteInterfaceTtl) != L7_SUCCESS)
    {
      CLR_VALID(I_ipMRouteInterfaceTtl, data->valid);
      return COMMIT_FAILED_ERROR;
    }

     return NO_ERROR;
}

#ifdef SR_ipMRouteInterfaceEntry_UNDO
/* add #define SR_ipMRouteInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the ipMRouteInterfaceEntry family.
 */
int
ipMRouteInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipMRouteInterfaceEntry_UNDO */

#endif /* SETS */

ipMRouteBoundaryEntry_t *
k_ipMRouteBoundaryEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 ipMRouteBoundaryIfIndex,
                            SR_UINT32 ipMRouteBoundaryAddress,
                            SR_UINT32 ipMRouteBoundaryAddressMask)
{
static ipMRouteBoundaryEntry_t ipMRouteBoundaryEntryData;
   L7_inet_addr_t inetMRtSrc, inetSrcMask;

   L7_uint32 intIfNum;
   ZERO_VALID(ipMRouteBoundaryEntryData.valid);
   ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex = ipMRouteBoundaryIfIndex;
   ipMRouteBoundaryEntryData.ipMRouteBoundaryAddress = ipMRouteBoundaryAddress;
   ipMRouteBoundaryEntryData.ipMRouteBoundaryAddressMask = ipMRouteBoundaryAddressMask;
   SET_VALID(I_ipMRouteBoundaryIfIndex, ipMRouteBoundaryEntryData.valid);
   SET_VALID(I_ipMRouteBoundaryAddress, ipMRouteBoundaryEntryData.valid);
   SET_VALID(I_ipMRouteBoundaryAddressMask, ipMRouteBoundaryEntryData.valid);

   inetAddressReset(&inetMRtSrc);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteBoundaryAddress, &inetMRtSrc))
     return (NULL);
   inetAddressReset(&inetSrcMask);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteBoundaryAddressMask, &inetSrcMask))
     return (NULL);

   if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex,
                                    &intIfNum) != L7_SUCCESS) ||
         (usmDbMcastMrouteBoundaryEntryGet(USMDB_UNIT_CURRENT, intIfNum,
           &inetMRtSrc,&inetSrcMask) != L7_SUCCESS)) :

       ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                         ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                      ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex, 
                                      &ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex,
                                    &intIfNum) != L7_SUCCESS) ||
         ( (usmDbMcastMrouteBoundaryEntryGet(USMDB_UNIT_CURRENT, intIfNum, 
             &inetMRtSrc,&inetSrcMask) != L7_SUCCESS) &&
           (usmDbMcastMrouteBoundaryEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum, 
             &inetMRtSrc,&inetSrcMask) != L7_SUCCESS)) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, 
             &ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex) != L7_SUCCESS) )
      )
   {
       return(NULL);
   }
   inetAddressGet(L7_AF_INET,&inetMRtSrc,&ipMRouteBoundaryEntryData.ipMRouteBoundaryAddress);
   inetAddressGet(L7_AF_INET,&inetSrcMask,&ipMRouteBoundaryEntryData.ipMRouteBoundaryAddressMask);
 
   switch (nominator)
   {
   case -1:
   case I_ipMRouteBoundaryIfIndex: /* already got it above*/
   case I_ipMRouteBoundaryAddress: /* already got it above*/       
   case I_ipMRouteBoundaryAddressMask: /* already got it above*/       
       break;

   case I_ipMRouteBoundaryStatus:
       ipMRouteBoundaryEntryData.ipMRouteBoundaryStatus = D_ipMRouteBoundaryStatus_active;       
       SET_VALID(I_ipMRouteBoundaryStatus, ipMRouteBoundaryEntryData.valid);
       break;
   
   default:
       /* unknown nominator */
       return(NULL);

    } /* end of switch(nominator)  */
   
   if (nominator >= 0 && !VALID(nominator, ipMRouteBoundaryEntryData.valid))
      return(NULL);
    
   return(&ipMRouteBoundaryEntryData);
}

#ifdef SETS
int
k_ipMRouteBoundaryEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ipMRouteBoundaryEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRouteBoundaryEntry_set_defaults(doList_t *dp)
{
    ipMRouteBoundaryEntry_t *data = (ipMRouteBoundaryEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_ipMRouteBoundaryEntry_set(ipMRouteBoundaryEntry_t *data,
                            ContextInfo *contextInfo, int function)
{
    L7_uint32 intIfNum;
    
    L7_inet_addr_t inetMRtSrc, inetSrcMask;

    inetAddressReset(&inetMRtSrc);
     inetAddressReset(&inetSrcMask);
     if( (L7_SUCCESS != inetAddressSet(L7_AF_INET, &(data->ipMRouteBoundaryAddress), &inetMRtSrc))||
      (L7_SUCCESS != inetAddressSet(L7_AF_INET, &(data->ipMRouteBoundaryAddressMask), &inetSrcMask))||
      (usmDbIntIfNumFromExtIfNum(data->ipMRouteBoundaryIfIndex, &intIfNum) != L7_SUCCESS ))
    { 
      ZERO_VALID(data->valid);
      return (COMMIT_FAILED_ERROR);
    }

      if (VALID(I_ipMRouteBoundaryStatus, data->valid) &&
         (snmpMcastMrouteBoundaryStatusSet(USMDB_UNIT_CURRENT, intIfNum,
             data->ipMRouteBoundaryAddress, data->ipMRouteBoundaryAddressMask, data->ipMRouteBoundaryStatus) != L7_SUCCESS))
      {
        CLR_VALID(I_ipMRouteBoundaryStatus, data->valid);
        return COMMIT_FAILED_ERROR;
      }

       return NO_ERROR;
}

#ifdef SR_ipMRouteBoundaryEntry_UNDO
/* add #define SR_ipMRouteBoundaryEntry_UNDO in sitedefs.h to
 * include the undo routine for the ipMRouteBoundaryEntry family.
 */
int
ipMRouteBoundaryEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipMRouteBoundaryEntry_UNDO */

#endif /* SETS */

ipMRouteScopeNameEntry_t *
k_ipMRouteScopeNameEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 ipMRouteScopeNameAddress,
                             SR_UINT32 ipMRouteScopeNameAddressMask,
                             OctetString * ipMRouteScopeNameLanguage)
{
#ifdef NOT_YET
   static ipMRouteScopeNameEntry_t ipMRouteScopeNameEntryData;

   /*
    * put your code to retrieve the information here
    */

   ipMRouteScopeNameEntryData.ipMRouteScopeNameAddress = ;
   ipMRouteScopeNameEntryData.ipMRouteScopeNameAddressMask = ;
   ipMRouteScopeNameEntryData.ipMRouteScopeNameLanguage = ;
   ipMRouteScopeNameEntryData.ipMRouteScopeNameString = ;
   ipMRouteScopeNameEntryData.ipMRouteScopeNameDefault = ;
   ipMRouteScopeNameEntryData.ipMRouteScopeNameStatus = ;
   SET_ALL_VALID(ipMRouteScopeNameEntryData.valid);
   return(&ipMRouteScopeNameEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_ipMRouteScopeNameEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ipMRouteScopeNameEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRouteScopeNameEntry_set_defaults(doList_t *dp)
{
    ipMRouteScopeNameEntry_t *data = (ipMRouteScopeNameEntry_t *) (dp->data);

    if ((data->ipMRouteScopeNameString = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->ipMRouteScopeNameDefault = D_ipMRouteScopeNameDefault_false;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_ipMRouteScopeNameEntry_set(ipMRouteScopeNameEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_ipMRouteScopeNameEntry_UNDO
/* add #define SR_ipMRouteScopeNameEntry_UNDO in sitedefs.h to
 * include the undo routine for the ipMRouteScopeNameEntry family.
 */
int
ipMRouteScopeNameEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipMRouteScopeNameEntry_UNDO */

#endif /* SETS */
