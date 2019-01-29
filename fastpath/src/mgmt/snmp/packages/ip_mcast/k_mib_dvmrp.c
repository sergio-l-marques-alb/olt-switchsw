/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_dvmrp.c
*
* @component SNMP
*
* @comments
*
* @create
*
* Created by: Mahe Korukonda (BRI) 03/19/2002
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_dvmrp_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_dvmrp_api.h"
#endif



dvmrpScalar_t *
k_dvmrpScalar_get(int serialNum, ContextInfo *contextInfo,
                  int nominator)
{
   static dvmrpScalar_t dvmrpScalarData;

   static L7_BOOL firstTime = L7_TRUE;
   char snmp_buffer[SNMP_BUFFER_LEN];


   /* Clear all the bits*/
   ZERO_VALID(dvmrpScalarData.valid);
   
   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);
 
    if (firstTime == L7_TRUE)
    {
        firstTime = L7_FALSE;
        dvmrpScalarData.dvmrpVersionString = MakeOctetString(NULL, 0);
    }

   switch(nominator)
   {
        case -1:
        break;

        case I_dvmrpVersionString:
        if (usmDbDvmrpVersionStringGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS )
        if (SafeMakeOctetStringFromText(&dvmrpScalarData.dvmrpVersionString, snmp_buffer) == L7_TRUE)
            SET_VALID(I_dvmrpVersionString, dvmrpScalarData.valid);

        break;

        case I_dvmrpNumRoutes:
        if (usmDbDvmrpNumRoutesGet(USMDB_UNIT_CURRENT, &dvmrpScalarData.dvmrpNumRoutes) == L7_SUCCESS )
          SET_VALID(I_dvmrpNumRoutes, dvmrpScalarData.valid);
        break;

        case I_dvmrpReachableRoutes:
        if (usmDbDvmrpReachableRoutesGet(USMDB_UNIT_CURRENT, &dvmrpScalarData.dvmrpReachableRoutes) == L7_SUCCESS )
          SET_VALID(I_dvmrpReachableRoutes, dvmrpScalarData.valid);
        break;

      default:
        /* unknown nominator */
        return(NULL);
        break;
    }
   if (nominator >= 0 && !VALID(nominator, dvmrpScalarData.valid))
      return(NULL);

   return(&dvmrpScalarData);
}

dvmrpInterfaceEntry_t *
k_dvmrpInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 dvmrpInterfaceIfIndex)
{

   static dvmrpInterfaceEntry_t dvmrpInterfaceEntryData;
   L7_inet_addr_t dvmrpNbrAddr;
   L7_ushort16 tmpMetric;
   L7_uint32 intIfNum;
   static L7_BOOL firstTime = L7_TRUE;

   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;
     dvmrpInterfaceEntryData.dvmrpInterfaceInterfaceKey = MakeOctetString(NULL, 0);
   }

  /* Clear all the bits*/
    ZERO_VALID(dvmrpInterfaceEntryData.valid);
    dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex = dvmrpInterfaceIfIndex;
    SET_VALID(I_dvmrpInterfaceIfIndex, dvmrpInterfaceEntryData.valid);
  
  
  if ( (searchType == EXACT) ?
    ( (usmDbIntIfNumFromExtIfNum(dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
      (usmDbDvmrpIntfEntryGet(USMDB_UNIT_CURRENT, &intIfNum) != L7_SUCCESS) ) :

    ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                      dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex) != L7_SUCCESS) &&
        (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                   dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex, 
                                   &dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex) != L7_SUCCESS) ) ||
      (usmDbIntIfNumFromExtIfNum(dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
      ( (snmpDvmrpIntfEntryGet(USMDB_UNIT_CURRENT, &intIfNum, (L7_uint32)nominator) != L7_SUCCESS) &&
        (snmpDvmrpInterfaceEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum, (L7_uint32)nominator) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex) != L7_SUCCESS) )
    )
    {
        return(NULL);
    }
/*
  if ( (searchType == EXACT) ?
    ( (usmDbIntIfNumFromExtIfNum(dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
      (usmDbDvmrpIntfEntryGet(USMDB_UNIT_CURRENT, &intIfNum) != L7_SUCCESS) ) :

    ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex) != L7_SUCCESS) &&
        (usmDbGetNextVisibleExtIfNumber(dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex, &dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex) != L7_SUCCESS) ) ||
      (usmDbIntIfNumFromExtIfNum(dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
      ( (snmpDvmrpIntfEntryGet(USMDB_UNIT_CURRENT, &intIfNum, (L7_uint32)nominator) != L7_SUCCESS) &&
        (snmpDvmrpInterfaceEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum, (L7_uint32)nominator) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &dvmrpInterfaceEntryData.dvmrpInterfaceIfIndex) != L7_SUCCESS) )
    )
    {
        return(NULL);
    }
*/

   switch(nominator)
   {
        case -1:
        break;

        case I_dvmrpInterfaceIfIndex: /* already got it above*/
        break;

        case I_dvmrpInterfaceLocalAddress:
        inetAddressReset(&dvmrpNbrAddr);
        if (usmDbDvmrpIntfLocalAddressGet(USMDB_UNIT_CURRENT, intIfNum,&dvmrpNbrAddr) == L7_SUCCESS )
        {
          if(L7_SUCCESS != inetAddressGet(L7_AF_INET,&dvmrpNbrAddr, &dvmrpInterfaceEntryData.dvmrpInterfaceLocalAddress))
            return(NULL);
  
          SET_VALID(I_dvmrpInterfaceLocalAddress, dvmrpInterfaceEntryData.valid);
        }
        break;

        case I_dvmrpInterfaceMetric:
        if (usmDbDvmrpIntfMetricGet(USMDB_UNIT_CURRENT, intIfNum,&tmpMetric) == L7_SUCCESS )
        {
          dvmrpInterfaceEntryData.dvmrpInterfaceMetric = tmpMetric;
          SET_VALID(I_dvmrpInterfaceMetric, dvmrpInterfaceEntryData.valid);
        }
        break;

        case I_dvmrpInterfaceStatus:
        if(snmpDvmrpInterfaceStatusGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpInterfaceEntryData.dvmrpInterfaceStatus) == L7_SUCCESS)
          SET_VALID(I_dvmrpInterfaceStatus, dvmrpInterfaceEntryData.valid);
        break;

        case I_dvmrpInterfaceRcvBadPkts:
        if (usmDbDvmrpIntfRcvBadPktsGet(USMDB_UNIT_CURRENT, intIfNum,(L7_ulong32 *)(&dvmrpInterfaceEntryData.dvmrpInterfaceRcvBadPkts)) == L7_SUCCESS )
          SET_VALID(I_dvmrpInterfaceRcvBadPkts, dvmrpInterfaceEntryData.valid);
        break;

        case I_dvmrpInterfaceRcvBadRoutes:
        if (usmDbDvmrpIntfRcvBadRoutesGet(USMDB_UNIT_CURRENT, intIfNum,(L7_ulong32 *)(&dvmrpInterfaceEntryData.dvmrpInterfaceRcvBadRoutes)) == L7_SUCCESS )
          SET_VALID(I_dvmrpInterfaceRcvBadRoutes, dvmrpInterfaceEntryData.valid);
        break;

        case I_dvmrpInterfaceSentRoutes:
        if (usmDbDvmrpIntfSentRoutesGet(USMDB_UNIT_CURRENT, intIfNum,(L7_ulong32 *)(&dvmrpInterfaceEntryData.dvmrpInterfaceSentRoutes)) == L7_SUCCESS )
          SET_VALID(I_dvmrpInterfaceSentRoutes, dvmrpInterfaceEntryData.valid);
        break;

      default:
        /* unknown nominator */
        return(NULL);
        break;
   }

   if (nominator >= 0 && !VALID(nominator, dvmrpInterfaceEntryData.valid))
      return(NULL);

   return(&dvmrpInterfaceEntryData);
}

#ifdef SETS
int
k_dvmrpInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    /* Following line implemented per VLAN mib implementaion...*/
    ZERO_VALID(((dvmrpInterfaceEntry_t *) (dp->data))->valid);
    return NO_ERROR;
}

int
k_dvmrpInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dvmrpInterfaceEntry_set_defaults(doList_t *dp)
{
    dvmrpInterfaceEntry_t *data = (dvmrpInterfaceEntry_t *) (dp->data);

    data->dvmrpInterfaceMetric = 1;
    data->dvmrpInterfaceRcvBadPkts = (SR_UINT32) 0;
    data->dvmrpInterfaceRcvBadRoutes = (SR_UINT32) 0;
    data->dvmrpInterfaceSentRoutes = (SR_UINT32) 0;
    if ((data->dvmrpInterfaceInterfaceKey = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    
    SET_VALID(I_dvmrpInterfaceMetric, data->valid);
    SET_VALID(I_dvmrpInterfaceRcvBadPkts, data->valid);
    SET_VALID(I_dvmrpInterfaceRcvBadRoutes, data->valid);
    SET_VALID(I_dvmrpInterfaceSentRoutes, data->valid);
    SET_VALID(I_dvmrpInterfaceSentRoutes, data->valid);

    return NO_ERROR;
}

int
k_dvmrpInterfaceEntry_set(dvmrpInterfaceEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);

  /* Getting internal interface number */
  if (usmDbIntIfNumFromExtIfNum(data->dvmrpInterfaceIfIndex, &intIfNum) != L7_SUCCESS )
      {
          ZERO_VALID(data->valid);
          return(COMMIT_FAILED_ERROR);
      }


/*  if (VALID(I_dvmrpInterfaceLocalAddress, data->valid))
  {
      if (usmDbDvmrpInterfaceLocalAddressSet(USMDB_UNIT_CURRENT, data->dvmrpInterfaceIfIndex,
                                    data->dvmrpInterfaceLocalAddress) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
*/

  if (VALID(I_dvmrpInterfaceLocalAddress, data->valid))
  {
    return(READ_ONLY_ERROR);
  }

  if (VALID(I_dvmrpInterfaceMetric, data->valid) && 
      usmDbDvmrpIntfMetricSet(USMDB_UNIT_CURRENT, intIfNum, data->dvmrpInterfaceMetric) != L7_SUCCESS)
  {
      CLR_VALID(I_dvmrpInterfaceMetric, data->valid); 
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_dvmrpInterfaceStatus, data->valid) &&
      snmpDvmrpInterfaceStatusSet(USMDB_UNIT_CURRENT, intIfNum, data->dvmrpInterfaceStatus) != L7_SUCCESS)
    {
       CLR_VALID(I_dvmrpInterfaceStatus, data->valid);
       return (COMMIT_FAILED_ERROR);
    }

  return NO_ERROR;
}

#ifdef SR_dvmrpInterfaceEntry_UNDO
/* add #define SR_dvmrpInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the dvmrpInterfaceEntry family.
 */
int
dvmrpInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dvmrpInterfaceEntry_UNDO */

#endif /* SETS */

dvmrpNeighborEntry_t *
k_dvmrpNeighborEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 dvmrpNeighborIfIndex,
                         SR_UINT32 dvmrpNeighborAddress)
{
   static dvmrpNeighborEntry_t dvmrpNeighborEntryData;
   L7_inet_addr_t dvmrpNbrAddr;
   L7_uint32 intIfNum;
   L7_uint32 seconds;
   L7_ushort16 capabilities;
   L7_ushort16 version;
   L7_uchar8 val8;
   static L7_BOOL firstTime = L7_TRUE;

    ZERO_VALID(dvmrpNeighborEntryData.valid);
   
   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);
 
    dvmrpNeighborEntryData.dvmrpNeighborIfIndex = dvmrpNeighborIfIndex;
    dvmrpNeighborEntryData.dvmrpNeighborAddress = dvmrpNeighborAddress;

    inetAddressReset(&dvmrpNbrAddr);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpNeighborEntryData.dvmrpNeighborAddress,&dvmrpNbrAddr))
      return(NULL);
     
    if (firstTime == L7_TRUE)
    {
        firstTime = L7_FALSE;
        dvmrpNeighborEntryData.dvmrpNeighborCapabilities = MakeOctetString(NULL, 0);
    }

    SET_VALID(I_dvmrpNeighborIfIndex, dvmrpNeighborEntryData.valid);
    SET_VALID(I_dvmrpNeighborAddress, dvmrpNeighborEntryData.valid);
    
    
    if ( (searchType == EXACT) ?
    ( (usmDbIntIfNumFromExtIfNum(dvmrpNeighborEntryData.dvmrpNeighborIfIndex, &intIfNum) != L7_SUCCESS) ||
      (usmDbDvmrpNeighborEntryGet(USMDB_UNIT_CURRENT, &intIfNum,&dvmrpNbrAddr) != L7_SUCCESS) ) :
    
    ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                      dvmrpNeighborEntryData.dvmrpNeighborIfIndex) != L7_SUCCESS) &&
        (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                   dvmrpNeighborEntryData.dvmrpNeighborIfIndex, 
                                   &dvmrpNeighborEntryData.dvmrpNeighborIfIndex) != L7_SUCCESS) ) ||
      (usmDbIntIfNumFromExtIfNum(dvmrpNeighborEntryData.dvmrpNeighborIfIndex, &intIfNum) != L7_SUCCESS) ||
      ( (usmDbDvmrpNeighborEntryGet(USMDB_UNIT_CURRENT, &intIfNum,&dvmrpNbrAddr) != L7_SUCCESS) &&
        (usmDbDvmrpNeighborEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum,&dvmrpNbrAddr) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &dvmrpNeighborEntryData.dvmrpNeighborIfIndex) != L7_SUCCESS) )
    )
    {
        return(NULL);
    }

/*
    if ( (searchType == EXACT) ?
    ( (usmDbIntIfNumFromExtIfNum(dvmrpNeighborEntryData.dvmrpNeighborIfIndex, &intIfNum) != L7_SUCCESS) ||
      (usmDbDvmrpNeighborEntryGet(USMDB_UNIT_CURRENT, &intIfNum,&dvmrpNbrAddr) != L7_SUCCESS) ) :
    
    ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, dvmrpNeighborEntryData.dvmrpNeighborIfIndex) != L7_SUCCESS) &&
        (usmDbGetNextVisibleExtIfNumber(dvmrpNeighborEntryData.dvmrpNeighborIfIndex, &dvmrpNeighborEntryData.dvmrpNeighborIfIndex) != L7_SUCCESS) ) ||
      (usmDbIntIfNumFromExtIfNum(dvmrpNeighborEntryData.dvmrpNeighborIfIndex, &intIfNum) != L7_SUCCESS) ||
      ( (usmDbDvmrpNeighborEntryGet(USMDB_UNIT_CURRENT, &intIfNum,&dvmrpNbrAddr) != L7_SUCCESS) &&
        (usmDbDvmrpNeighborEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum,&dvmrpNbrAddr) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &dvmrpNeighborEntryData.dvmrpNeighborIfIndex) != L7_SUCCESS) )
    )
    {
        return(NULL);
    }
*/
   if(inetAddressGet(L7_AF_INET, &dvmrpNbrAddr,&dvmrpNeighborEntryData.dvmrpNeighborAddress)!=L7_SUCCESS)
  return(NULL);

   switch(nominator)
   {
        case -1:
        break;

        case I_dvmrpNeighborIfIndex: /* already got it above*/
        break;

        case I_dvmrpNeighborAddress: /* already got it above*/
        break;

        case I_dvmrpNeighborUpTime:
        if (usmDbDvmrpNeighborUpTimeGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, (void *)(&seconds)) == L7_SUCCESS )
        {
          /* convert seconds into ticks 1 second = 100 ticks */
          dvmrpNeighborEntryData.dvmrpNeighborUpTime = seconds * 100;
          SET_VALID(I_dvmrpNeighborUpTime, dvmrpNeighborEntryData.valid);
        }
        break;

        case I_dvmrpNeighborExpiryTime:
        if (usmDbDvmrpNeighborExpiryTimeGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, (void *)(&seconds) ) == L7_SUCCESS )
        {
            /* convert seconds into ticks 1 second = 100 ticks */
            dvmrpNeighborEntryData.dvmrpNeighborExpiryTime = seconds * 100;
            SET_VALID(I_dvmrpNeighborExpiryTime, dvmrpNeighborEntryData.valid);
        }
        break;

        case I_dvmrpNeighborGenerationId:
        if (usmDbDvmrpNeighborGenIdGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, (L7_ulong32 *)(&dvmrpNeighborEntryData.dvmrpNeighborGenerationId)) == L7_SUCCESS )
          SET_VALID(I_dvmrpNeighborGenerationId, dvmrpNeighborEntryData.valid);
        break;

        case I_dvmrpNeighborMajorVersion:
        if (usmDbDvmrpNeighborMajorVersionGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, &version ) == L7_SUCCESS )
        {
          dvmrpNeighborEntryData.dvmrpNeighborMajorVersion = version;
          SET_VALID(I_dvmrpNeighborMajorVersion, dvmrpNeighborEntryData.valid);
        }
        break;

        case I_dvmrpNeighborMinorVersion:
        if (usmDbDvmrpNeighborMinorVersionGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, &version ) == L7_SUCCESS )
        {
          dvmrpNeighborEntryData.dvmrpNeighborMinorVersion = version;
          SET_VALID(I_dvmrpNeighborMinorVersion, dvmrpNeighborEntryData.valid);
        }
        break;

        case I_dvmrpNeighborCapabilities:
        if (usmDbDvmrpNeighborCapabilitiesGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, &capabilities) == L7_SUCCESS )
        {
            val8 = (L7_uchar8)(capabilities & 0x00FF);
            if (SafeMakeOctetString(&dvmrpNeighborEntryData.dvmrpNeighborCapabilities, &val8, sizeof(L7_uchar8)) == L7_TRUE)
                SET_VALID(I_dvmrpNeighborCapabilities, dvmrpNeighborEntryData.valid);
        }
        break;

        case I_dvmrpNeighborRcvRoutes:
        if (usmDbDvmrpNeighborRcvRoutesGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, (L7_ulong32 *)(&dvmrpNeighborEntryData.dvmrpNeighborRcvRoutes)) == L7_SUCCESS )
          SET_VALID(I_dvmrpNeighborRcvRoutes, dvmrpNeighborEntryData.valid);
        break;

        case I_dvmrpNeighborRcvBadPkts:
        if (usmDbDvmrpNeighborRcvBadPktsGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, (L7_ulong32 *)(&dvmrpNeighborEntryData.dvmrpNeighborRcvBadPkts)) == L7_SUCCESS )
          SET_VALID(I_dvmrpNeighborRcvBadPkts, dvmrpNeighborEntryData.valid);
        break;

        case I_dvmrpNeighborRcvBadRoutes:
        if (usmDbDvmrpNeighborRcvBadRoutesGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, (L7_ulong32 *)(&dvmrpNeighborEntryData.dvmrpNeighborRcvBadRoutes)) == L7_SUCCESS )
          SET_VALID(I_dvmrpNeighborRcvBadRoutes, dvmrpNeighborEntryData.valid);
        break;

        case I_dvmrpNeighborState:
        if (snmpDvmrpNeighborStateGet(USMDB_UNIT_CURRENT, intIfNum, &dvmrpNbrAddr, &dvmrpNeighborEntryData.dvmrpNeighborState) == L7_SUCCESS )
          SET_VALID(I_dvmrpNeighborState, dvmrpNeighborEntryData.valid);
        break;

      default:
        /* unknown nominator */
        return(NULL);
        break;
    }

   if (nominator >= 0 && !VALID(nominator, dvmrpNeighborEntryData.valid))
      return(NULL);

    return(&dvmrpNeighborEntryData);
}

dvmrpRouteEntry_t *
k_dvmrpRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_UINT32 dvmrpRouteSource,
                      SR_UINT32 dvmrpRouteSourceMask)
{
   static dvmrpRouteEntry_t dvmrpRouteEntryData;
   L7_ushort16 metric;
   L7_uint32 intIfNum;
   L7_inet_addr_t dvmrpNbrAddr;
   L7_inet_addr_t dvmrpNbrMask;
   L7_inet_addr_t dvmrpUpsNbr;

   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   /* Clear all the bits*/
   ZERO_VALID(dvmrpRouteEntryData.valid);
   dvmrpRouteEntryData.dvmrpRouteSource = dvmrpRouteSource;
   dvmrpRouteEntryData.dvmrpRouteSourceMask = dvmrpRouteSourceMask;
   SET_VALID(I_dvmrpRouteSource, dvmrpRouteEntryData.valid);
   SET_VALID(I_dvmrpRouteSourceMask, dvmrpRouteEntryData.valid);

   
   inetAddressReset(&dvmrpNbrAddr);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteEntryData.dvmrpRouteSource,&dvmrpNbrAddr))
     return(NULL);
   inetAddressReset(&dvmrpNbrMask);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteEntryData.dvmrpRouteSourceMask,&dvmrpNbrMask))
     return(NULL);
   inetAddressReset(&dvmrpUpsNbr);

   if ((searchType == EXACT) ?
    (usmDbDvmrpRouteEntryGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask ) != L7_SUCCESS) :
    ((usmDbDvmrpRouteEntryGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask ) != L7_SUCCESS) &&
    (usmDbDvmrpRouteEntryNextGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask ) != L7_SUCCESS)))
   {
       return(NULL);
   }

    inetAddressGet(L7_AF_INET,&dvmrpNbrAddr,&dvmrpRouteEntryData.dvmrpRouteSource);
    inetAddressGet(L7_AF_INET,&dvmrpNbrMask,&dvmrpRouteEntryData.dvmrpRouteSourceMask);

   switch(nominator)
   {
        case -1:
        break;

        case I_dvmrpRouteSource: /* already got it above*/
        break;

        case I_dvmrpRouteSourceMask: /* already got it above*/
        break;

        case I_dvmrpRouteUpstreamNeighbor:
        if (usmDbDvmrpRouteUpstreamNeighborGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask, &dvmrpUpsNbr) == L7_SUCCESS )
        {
          if(L7_SUCCESS != inetAddressGet(L7_AF_INET,&dvmrpUpsNbr, &dvmrpRouteEntryData.dvmrpRouteUpstreamNeighbor))
            return(NULL);

          SET_VALID(I_dvmrpRouteUpstreamNeighbor, dvmrpRouteEntryData.valid);
        }
        break;

        case I_dvmrpRouteIfIndex:
        if (usmDbDvmrpRouteIfIndexGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask, &intIfNum) == L7_SUCCESS )
        {
            usmDbExtIfNumFromIntIfNum(intIfNum, &dvmrpRouteEntryData.dvmrpRouteIfIndex);
            SET_VALID(I_dvmrpRouteIfIndex, dvmrpRouteEntryData.valid);
        }
        break;

        case I_dvmrpRouteMetric:
        if (usmDbDvmrpRouteMetricGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask, &metric ) == L7_SUCCESS )
        {
            dvmrpRouteEntryData.dvmrpRouteMetric = metric;
            SET_VALID(I_dvmrpRouteMetric, dvmrpRouteEntryData.valid);
        }
        break;

        case I_dvmrpRouteExpiryTime:
        if (snmpDvmrpRouteExpiryTimeGet(USMDB_UNIT_CURRENT, dvmrpRouteEntryData.dvmrpRouteSource,dvmrpRouteEntryData.dvmrpRouteSourceMask , &dvmrpRouteEntryData.dvmrpRouteExpiryTime) == L7_SUCCESS )
          SET_VALID(I_dvmrpRouteExpiryTime, dvmrpRouteEntryData.valid);
        break;

        case I_dvmrpRouteUpTime:
        if (snmpDvmrpRouteUpTimeGet(USMDB_UNIT_CURRENT, dvmrpRouteEntryData.dvmrpRouteSource,dvmrpRouteEntryData.dvmrpRouteSourceMask, &dvmrpRouteEntryData.dvmrpRouteUpTime) == L7_SUCCESS )
          SET_VALID(I_dvmrpRouteUpTime, dvmrpRouteEntryData.valid);
        break;

      default:
        /* unknown nominator */
        return(NULL);
        break;
    }

   if (nominator >= 0 && !VALID(nominator, dvmrpRouteEntryData.valid))
      return(NULL);

   return(&dvmrpRouteEntryData);
}

dvmrpRouteNextHopEntry_t *
k_dvmrpRouteNextHopEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 dvmrpRouteNextHopSource,
                             SR_UINT32 dvmrpRouteNextHopSourceMask,
                             SR_INT32 dvmrpRouteNextHopIfIndex)
{
   static dvmrpRouteNextHopEntry_t dvmrpRouteNextHopEntryData;
   L7_inet_addr_t dvmrpNbrAddr, dvmrpNbrMask;
   L7_uint32 intIfNum;
  
   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);
 
   ZERO_VALID(dvmrpRouteNextHopEntryData.valid);
    
  dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSource = dvmrpRouteNextHopSource;
  dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSourceMask = dvmrpRouteNextHopSourceMask;
  dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex = dvmrpRouteNextHopIfIndex;
  SET_VALID(I_dvmrpRouteNextHopSource, dvmrpRouteNextHopEntryData.valid);
  SET_VALID(I_dvmrpRouteNextHopSourceMask, dvmrpRouteNextHopEntryData.valid);
  SET_VALID(I_dvmrpRouteNextHopIfIndex, dvmrpRouteNextHopEntryData.valid);
    
  inetAddressReset(&dvmrpNbrAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSource,&dvmrpNbrAddr))
    return(NULL);
  inetAddressReset(&dvmrpNbrMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSourceMask,&dvmrpNbrMask))
    return(NULL);

    
  if (searchType == EXACT)
  {
    if ((usmDbIntIfNumFromExtIfNum(dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS) ||
        (usmDbDvmrpNextHopEntryGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask,&intIfNum) != L7_SUCCESS))
      return(NULL);
  }
  
  else
  {
    if (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex) == L7_SUCCESS)
    {
      /* if the external interface is valid, try to convert it to internal */
      if (usmDbIntIfNumFromExtIfNum(dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS)
        return(NULL);
    }
    else if (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                       dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex, 
                                       &dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex) != L7_SUCCESS)
    {
      /* if there aren't any external interfaces after this one, then set the internal to max Int */
			  dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSourceMask++ ;
			  inetAddressReset(&dvmrpNbrMask);
			  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSourceMask,&dvmrpNbrMask))
			    return(NULL);
			  intIfNum = 1;
    }
    else
    {
      /* if there is annother external interface, try to convert it to internal */
      if (usmDbIntIfNumFromExtIfNum(dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS)
        return(NULL);
    }
      
    if (((usmDbDvmrpNextHopEntryGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask,&intIfNum) != L7_SUCCESS) &&
         (usmDbDvmrpRouteNextHopEntryNextGet(USMDB_UNIT_CURRENT, &dvmrpNbrAddr, &dvmrpNbrMask, &intIfNum) != L7_SUCCESS)) ||
        (usmDbExtIfNumFromIntIfNum(intIfNum, &dvmrpRouteNextHopEntryData.dvmrpRouteNextHopIfIndex) != L7_SUCCESS))
      return(NULL);
  }

    inetAddressGet(L7_AF_INET,&dvmrpNbrAddr,&dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSource);
      inetAddressGet(L7_AF_INET,&dvmrpNbrMask,&dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSourceMask);

   switch(nominator)
   {
        case -1:
        break;

        case I_dvmrpRouteNextHopSource: /* already got it above*/
        break;

        case I_dvmrpRouteNextHopSourceMask: /* already got it above*/
        break;

        case I_dvmrpRouteNextHopIfIndex: /* already got it above*/
        break;

        case I_dvmrpRouteNextHopType:
        if (snmpDvmrpNextHopTypeGet(USMDB_UNIT_CURRENT, dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSource, dvmrpRouteNextHopEntryData.dvmrpRouteNextHopSourceMask, intIfNum, &dvmrpRouteNextHopEntryData.dvmrpRouteNextHopType) == L7_SUCCESS )
          SET_VALID(I_dvmrpRouteNextHopType, dvmrpRouteNextHopEntryData.valid);
        break;

      default:
        /* unknown nominator */
        return(NULL);
        break;
    }

   if (nominator >= 0 && !VALID(nominator, dvmrpRouteNextHopEntryData.valid))
      return(NULL);

   return(&dvmrpRouteNextHopEntryData);
}

dvmrpPruneEntry_t *
k_dvmrpPruneEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_UINT32 dvmrpPruneGroup,
                      SR_UINT32 dvmrpPruneSource,
                      SR_UINT32 dvmrpPruneSourceMask)
{
   static dvmrpPruneEntry_t dvmrpPruneEntryData;
   L7_inet_addr_t dvmrpPrunSrc, dvmrpPrunGrp, dvmrpPrunMask;

   /* To check whether DVMRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

  /* Clear all the bits*/
   ZERO_VALID(dvmrpPruneEntryData.valid);
   dvmrpPruneEntryData.dvmrpPruneGroup = dvmrpPruneGroup;
  dvmrpPruneEntryData.dvmrpPruneSource = dvmrpPruneSource;
  dvmrpPruneEntryData.dvmrpPruneSourceMask = dvmrpPruneSourceMask;
   SET_VALID(I_dvmrpPruneGroup, dvmrpPruneEntryData.valid);
  SET_VALID(I_dvmrpPruneSource, dvmrpPruneEntryData.valid);
  SET_VALID(I_dvmrpPruneSourceMask, dvmrpPruneEntryData.valid);

   inetAddressReset(&dvmrpPrunSrc);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpPruneEntryData.dvmrpPruneSource,&dvmrpPrunSrc))
     return(NULL);
   inetAddressReset(&dvmrpPrunMask);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpPruneEntryData.dvmrpPruneSourceMask,&dvmrpPrunMask))
     return(NULL);
   inetAddressReset(&dvmrpPrunGrp);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpPruneEntryData.dvmrpPruneGroup,&dvmrpPrunGrp))
     return(NULL);
   
   if ((searchType == EXACT) ?
    (usmDbDvmrpPruneEntryGet(USMDB_UNIT_CURRENT, &dvmrpPrunGrp, &dvmrpPrunSrc, &dvmrpPrunMask) != L7_SUCCESS) :
    ((usmDbDvmrpPruneEntryGet(USMDB_UNIT_CURRENT, &dvmrpPrunGrp, &dvmrpPrunSrc, &dvmrpPrunMask) != L7_SUCCESS) &&
    (usmDbDvmrpPruneEntryNextGet(USMDB_UNIT_CURRENT, &dvmrpPrunGrp, &dvmrpPrunSrc, &dvmrpPrunMask) != L7_SUCCESS)))
   {
       return(NULL);
   }
  inetAddressGet(L7_AF_INET,&dvmrpPrunGrp,&dvmrpPruneEntryData.dvmrpPruneGroup);
  inetAddressGet(L7_AF_INET,&dvmrpPrunSrc,&dvmrpPruneEntryData.dvmrpPruneSource);
  inetAddressGet(L7_AF_INET,&dvmrpPrunMask,&dvmrpPruneEntryData.dvmrpPruneSourceMask);
   switch(nominator)
   {
        case -1:
        break;

        case I_dvmrpPruneGroup: /* already got it above*/
        break;

        case I_dvmrpPruneSource: /* already got it above*/
        break;

        case I_dvmrpPruneSourceMask: /* already got it above*/
        break;

        case I_dvmrpPruneExpiryTime:
        if (snmpDvmrpPruneExpiryTimeGet(USMDB_UNIT_CURRENT, dvmrpPruneEntryData.dvmrpPruneGroup, dvmrpPruneEntryData.dvmrpPruneSource, dvmrpPruneEntryData.dvmrpPruneSourceMask, &dvmrpPruneEntryData.dvmrpPruneExpiryTime ) == L7_SUCCESS )
          SET_VALID(I_dvmrpPruneExpiryTime, dvmrpPruneEntryData.valid);
        break;

      default:
        /* unknown nominator */
        return(NULL);
        break;
    }

   if (nominator >= 0 && !VALID(nominator, dvmrpPruneEntryData.valid))
      return(NULL);

   return(&dvmrpPruneEntryData);
}
