/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_igmp.c
*
* @purpose draft-ietf-magma-mgmd-mib-03 - MGMD-STD-MIB
*
* @component SNMP
*
* @comments
*
* @create
*
* @author Kumar Manish
*
* @end
*
**********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/
#include <k_private_base.h>
#include <inst_lib.h>
#include "k_mib_mgmd_api.h" 
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_igmp_api.h"
#endif



L7_uint32
mgmdIntfTableIndexCompare(L7_uint32 mgmdIntfV4Index, L7_uint32 mgmdIntfV6Index)
{

  if(mgmdIntfV4Index < mgmdIntfV6Index)
    return L7_AF_INET;
  else if(mgmdIntfV6Index < mgmdIntfV4Index)
    return L7_AF_INET6;
  else
    return L7_AF_INET;
}


mgmdHostInterfaceEntry_t *
k_mgmdHostInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 mgmdHostInterfaceIfIndex,
                             SR_INT32 mgmdHostInterfaceQuerierType)
{
  #ifdef MGMD_SUPPORTED

  static mgmdHostInterfaceEntry_t mgmdHostInterfaceEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 intIfNum = 0;
  
  L7_inet_addr_t rtrIntfIp;
  L7_uint32 seconds = 0;

  L7_RC_t rc1 = L7_SUCCESS, rc2 = L7_SUCCESS;
  L7_uint32 mgmdIntfV4Index, mgmdIntfV6Index;
  L7_uint32 intIfNumV4, intIfNumV6;


  
  ZERO_VALID(mgmdHostInterfaceEntryData.valid);

   if((mgmdHostInterfaceQuerierType != L7_NULL) &&
     (mgmdHostInterfaceQuerierType != L7_AF_INET) &&
     (mgmdHostInterfaceQuerierType != L7_AF_INET6))
        return(NULL);


  if (L7_TRUE == firstTime)
  {
    firstTime = L7_FALSE;
    mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerier = MakeOctetString(NULL, 0);
  }
  
  /* Populating the indices one by one*/

  /* Populating InterfaceIfIndex */

   mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex = mgmdHostInterfaceIfIndex;
  SET_VALID(I_mgmdHostInterfaceIfIndex, mgmdHostInterfaceEntryData.valid);

  usmDbIntIfNumFromExtIfNum(mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex, &intIfNum);

  inetAddressReset(&rtrIntfIp);
  if(searchType == EXACT)
  {
     if(mgmdHostInterfaceQuerierType != D_mgmdHostInterfaceQuerierType_ipv4)
          
     {
       ZERO_VALID(mgmdHostInterfaceEntryData.valid);
       return(NULL);
     }

     else if((usmDbMgmdHostInterfaceEntryGet(USMDB_UNIT_CURRENT,L7_AF_INET,intIfNum) != L7_SUCCESS) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex) != L7_SUCCESS))
     {
       ZERO_VALID(mgmdHostInterfaceEntryData.valid);
       return(NULL);
     }

     mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = D_mgmdHostInterfaceQuerierType_ipv4;
     SET_VALID(I_mgmdHostInterfaceQuerierType, mgmdHostInterfaceEntryData.valid); 

  }
  else if(searchType == NEXT)
  {
     if(mgmdHostInterfaceQuerierType >= D_mgmdHostInterfaceQuerierType_ipv6)
     {
       mgmdIntfV4Index = mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex + 1;
     }
     else if(mgmdHostInterfaceQuerierType <= D_mgmdHostInterfaceQuerierType_ipv4)
     {
       mgmdIntfV4Index = mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex;
     }
     mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = L7_AF_INET;
     if((usmDbIntIfNumFromExtIfNum(mgmdIntfV4Index, &intIfNumV4) != L7_SUCCESS) || 
        ((usmDbMgmdHostInterfaceEntryGet(USMDB_UNIT_CURRENT, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, intIfNumV4) != L7_SUCCESS) && 
         (usmDbMgmdHostInterfaceEntryNextGet(USMDB_UNIT_CURRENT, &mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, &intIfNumV4) != L7_SUCCESS)) ||
       (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdIntfV4Index) != L7_SUCCESS))
     {
       rc1 = L7_FAILURE;
     }

     mgmdIntfV6Index = mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex;
     mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = L7_AF_INET6;

     if((usmDbIntIfNumFromExtIfNum(mgmdIntfV6Index, &intIfNumV6) != L7_SUCCESS) ||
        ((usmDbMgmdHostInterfaceEntryGet(USMDB_UNIT_CURRENT, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, intIfNumV6) != L7_SUCCESS) && 
         (usmDbMgmdHostInterfaceEntryNextGet(USMDB_UNIT_CURRENT,&mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, &intIfNumV6) != L7_SUCCESS)) ||
        (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdIntfV6Index) != L7_SUCCESS))
     {
       rc2 = L7_FAILURE;
     }

     if((rc1 == L7_SUCCESS) && (rc2 == L7_FAILURE))
     {
       mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = L7_AF_INET;
       mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex = mgmdIntfV4Index;
       intIfNum = intIfNumV4;
     }
     else if((rc1 == L7_FAILURE) && (rc2 == L7_SUCCESS))
     {
       mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = L7_AF_INET6;
       mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex = mgmdIntfV6Index;
       intIfNum = intIfNumV6;
     }
     else if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
     {
       L7_int32 ret;

       ret = mgmdIntfTableIndexCompare(mgmdIntfV4Index, mgmdIntfV6Index);

       if(ret == L7_AF_INET)
       {
         mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = L7_AF_INET;
         mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex = mgmdIntfV4Index;
         intIfNum = intIfNumV4;
       }
       if(ret == L7_AF_INET6)
       {
         mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType = L7_AF_INET6;
         mgmdHostInterfaceEntryData.mgmdHostInterfaceIfIndex = mgmdIntfV6Index;
         intIfNum = intIfNumV6;
       }
     }
     else if((rc1 == L7_FAILURE) && (rc2 == L7_FAILURE))
     {
       return(NULL);
     }
  }


  switch (nominator)
  {
  case -1:
  case I_mgmdHostInterfaceIfIndex:   
  case I_mgmdHostInterfaceQuerierType:
    break;
  
  case I_mgmdHostInterfaceQuerier:
       
   /*        if (usmDbMgmdHostInterfaceQuerierGet(USMDB_UNIT_CURRENT, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, intIfNum, &rtrIntfIp) == L7_SUCCESS)
     {
       if(mgmdHostInterfaceQuerierType == D_mgmdHostInterfaceQuerierType_ipv4)
       {
         L7_uint32 ipAddr;

         if(L7_SUCCESS == inetAddressGet(L7_AF_INET, &rtrIntfIp, &ipAddr))
         {
           if (SafeMakeOctetString(&mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerier,
                                   (L7_uchar8 *)&ipAddr, sizeof(L7_uint32)) == L7_TRUE )
           {
             SET_VALID(nominator, mgmdHostInterfaceEntryData.valid);
           }
         }
       }
  
           if (SafeMakeOctetString(&mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerier,
                                   (L7_uchar8 *)&ipAddr, sizeof(L7_in6_addr_t)) == L7_TRUE )
           {
             SET_VALID(nominator, mgmdHostInterfaceEntryData.valid);
           }
         }
       }
     }*/
                 
   
    break;
 
    case I_mgmdHostInterfaceStatus:
     if (snmpMgmdProxyInterfaceRowStatusGet(USMDB_UNIT_CURRENT, intIfNum, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, &mgmdHostInterfaceEntryData.mgmdHostInterfaceStatus) == L7_SUCCESS)                                                                                                                              
        SET_VALID(nominator, mgmdHostInterfaceEntryData.valid);
    break;
  
  case I_mgmdHostInterfaceVersion:
     if (usmDbMgmdHostInterfaceVersionGet(USMDB_UNIT_CURRENT, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, intIfNum, &mgmdHostInterfaceEntryData.mgmdHostInterfaceVersion) == L7_SUCCESS)
                                                                                                                             
        SET_VALID(nominator, mgmdHostInterfaceEntryData.valid);
    break;
                                                                                                                             
  case I_mgmdHostInterfaceVersion1QuerierTimer:
     if (usmDbMgmdHostInterfaceVer1QuerierTimerGet(USMDB_UNIT_CURRENT, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, intIfNum, &seconds) == L7_SUCCESS 
     && SafeMakeTimeTicksFromSeconds(seconds, &mgmdHostInterfaceEntryData.mgmdHostInterfaceVersion1QuerierTimer) == L7_TRUE)
        SET_VALID(nominator, mgmdHostInterfaceEntryData.valid);
    break;


  case I_mgmdHostInterfaceVersion2QuerierTimer:
     if (usmDbMgmdHostInterfaceVer2QuerierTimerGet(USMDB_UNIT_CURRENT, mgmdHostInterfaceEntryData.mgmdHostInterfaceQuerierType, intIfNum, &seconds) == L7_SUCCESS
     && SafeMakeTimeTicksFromSeconds(seconds, &mgmdHostInterfaceEntryData.mgmdHostInterfaceVersion2QuerierTimer) == L7_TRUE)
        SET_VALID(nominator, mgmdHostInterfaceEntryData.valid);
    break;
                                                                                                                             
  default:
     /* unknown nominator */
     return(NULL);
     break;
 }                                                                                                                              
 if (nominator >= 0 && !VALID(nominator, mgmdHostInterfaceEntryData.valid))
     return(NULL);
 return(&mgmdHostInterfaceEntryData);
#endif /*MGMD_SUPPORTED*/
return(NULL);
}



#ifdef SETS
int
k_mgmdHostInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_mgmdHostInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{ 

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mgmdHostInterfaceEntry_set_defaults(doList_t *dp)
{
    mgmdHostInterfaceEntry_t *data = (mgmdHostInterfaceEntry_t *) (dp->data);

    if ((data->mgmdHostInterfaceQuerier = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->mgmdHostInterfaceVersion = 3;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mgmdHostInterfaceEntry_set(mgmdHostInterfaceEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  
#ifdef MGMD_SUPPORTED

  L7_uint32 intIfNum;
  /* translate the extIntnum to internal interface num*/
  if (usmDbIntIfNumFromExtIfNum(data->mgmdHostInterfaceIfIndex, &intIfNum) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }
                                                                                                                             
  if (usmDbMgmdHostInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum) != L7_SUCCESS)
  {
    /* if the entry doesn't exist, try to create it */
    if ( VALID(I_mgmdHostInterfaceStatus, data->valid) &&
       (data->mgmdHostInterfaceStatus == D_mgmdHostInterfaceStatus_createAndGo ||
        data->mgmdHostInterfaceStatus == D_mgmdHostInterfaceStatus_createAndWait))
    {
        if (usmDbMgmdInterfaceCreate(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,MGMD_PROXY_INTERFACE) != L7_SUCCESS)
        {
          CLR_VALID(I_mgmdHostInterfaceStatus, data->valid);
          return(COMMIT_FAILED_ERROR);
        }
    }
    else
    {
       return COMMIT_FAILED_ERROR;
    }
  }
  if (VALID(I_mgmdHostInterfaceVersion, data->valid) &&
      usmDbMgmdInterfaceVersionSet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum, data->mgmdHostInterfaceVersion) != L7_SUCCESS)
  {
    CLR_VALID(I_mgmdHostInterfaceVersion, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if ( VALID(I_mgmdHostInterfaceStatus, data->valid) &&
       (!(data->mgmdHostInterfaceStatus == D_mgmdHostInterfaceStatus_createAndGo ||
        data->mgmdHostInterfaceStatus == D_mgmdHostInterfaceStatus_createAndWait)))
  {
    if (snmpMgmdHostInterfaceRowStatusSet(USMDB_UNIT_CURRENT, intIfNum,
                                      data->mgmdHostInterfaceStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_mgmdHostInterfaceStatus, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }
#endif /*MGMD_SUPPORTED*/
  return NO_ERROR;
}




#ifdef SR_mgmdHostInterfaceEntry_UNDO
/* add #define SR_mgmdHostInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the mgmdHostInterfaceEntry family.
 */
int
mgmdHostInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mgmdHostInterfaceEntry_UNDO */

#endif /* SETS */

mgmdRouterInterfaceEntry_t *
k_mgmdRouterInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 mgmdRouterInterfaceIfIndex,
                               SR_INT32 mgmdRouterInterfaceQuerierType)
{
  static mgmdRouterInterfaceEntry_t mgmdRouterInterfaceEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 intIfNum = 0;
  
  L7_uint32 familyType=L7_AF_INET;
  L7_inet_addr_t rtrIntfIp;
  L7_RC_t rc1=L7_FAILURE,rc2=L7_FAILURE;
  

  ZERO_VALID(mgmdRouterInterfaceEntryData.valid);
  if(firstTime == L7_TRUE) {
     mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerier=MakeOctetString(NULL,0);
     firstTime = L7_FALSE;
  }

   /* Populating the indices one by one*/
      
     mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex=mgmdRouterInterfaceIfIndex;
      SET_VALID(I_mgmdRouterInterfaceIfIndex, mgmdRouterInterfaceEntryData.valid);
 
     /* Populating Router Cache Address Type */
   
    if(mgmdRouterInterfaceQuerierType>D_mgmdRouterInterfaceQuerierType_ipv6 && searchType==NEXT)
    {  
        if(usmDbGetNextExtIfNumber(mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex,
            &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex) != L7_SUCCESS)
          return(NULL);
    
        mgmdRouterInterfaceQuerierType=D_mgmdRouterInterfaceQuerierType_ipv4;
    }
     if(mgmdRouterInterfaceQuerierType<D_mgmdRouterInterfaceQuerierType_ipv4 && searchType==NEXT)
         mgmdRouterInterfaceQuerierType=D_mgmdRouterInterfaceQuerierType_ipv4;

   /*populating the interface index*/
 
   if(usmDbIntIfNumFromExtIfNum(mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex,
                                &intIfNum) != L7_SUCCESS)
   {
     if(searchType == EXACT)
     {
       firstTime =  L7_TRUE;
       ZERO_VALID(mgmdRouterInterfaceEntryData.valid);
       return(NULL);
     }

     if((usmDbGetNextExtIfNumber(mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex,
            &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex) != L7_SUCCESS) ||
        (usmDbIntIfNumFromExtIfNum(mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex,
            &intIfNum) != L7_SUCCESS))
     {
       intIfNum = L7_MAX_INTERFACE_COUNT;
     }
   }

  
    
  if(searchType == EXACT) 
   {
     if(mgmdRouterInterfaceQuerierType== D_mgmdRouterInterfaceQuerierType_ipv4)
      {
        familyType=L7_AF_INET;
      }
     else if(mgmdRouterInterfaceQuerierType == D_mgmdRouterInterfaceQuerierType_ipv6)
      {
        familyType=L7_AF_INET6;
      }
     else
      { 
       ZERO_VALID(mgmdRouterInterfaceEntryData.valid);
       return(NULL);
      }
     mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerierType=mgmdRouterInterfaceQuerierType; 
     SET_VALID(I_mgmdRouterInterfaceQuerierType, mgmdRouterInterfaceEntryData.valid);

    if((usmDbMgmdInterfaceEntryGet(USMDB_UNIT_CURRENT, familyType, intIfNum) != L7_SUCCESS) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex) != L7_SUCCESS))
     {
        
       ZERO_VALID(mgmdRouterInterfaceEntryData.valid);
       return(NULL);
     }

     else
     {
      if(familyType==L7_AF_INET)
         rc1=L7_SUCCESS;
      else if(familyType==L7_AF_INET6)
         rc2=L7_SUCCESS;
     }


   } /* end of searchType == EXACT */
  else if(searchType == NEXT )

   {
      if((mgmdRouterInterfaceQuerierType == D_mgmdRouterInterfaceQuerierType_ipv4)&&
           usmDbMgmdInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET,intIfNum ) == L7_SUCCESS)
            rc1=L7_SUCCESS;
      else  if(usmDbMgmdInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET6,intIfNum ) == L7_SUCCESS)
            rc2=L7_SUCCESS;
       
      else
        {
        
       if((mgmdRouterInterfaceQuerierType == D_mgmdRouterInterfaceQuerierType_ipv4)&&
           usmDbMgmdInterfaceEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &intIfNum) == L7_SUCCESS)
                rc1=L7_SUCCESS;
       else if(usmDbMgmdInterfaceEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET6, &intIfNum) == L7_SUCCESS)
                rc2=L7_SUCCESS;
        }
       
       if(usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceIfIndex) != L7_SUCCESS)
           return (NULL);
   }/* end of searchType == NEXT */
  
   /*got the ipv4 entry*/
  if(rc1==L7_SUCCESS)
     {
      familyType=L7_AF_INET;
      mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerierType = D_mgmdRouterInterfaceQuerierType_ipv4;
     }
   
   /*got the ipv6 entry*/

  else if(rc2==L7_SUCCESS)
     {
      familyType=L7_AF_INET6;
      mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerierType = D_mgmdRouterInterfaceQuerierType_ipv6;
     }

  /*entry get failed*/
  else
     {
      ZERO_VALID(mgmdRouterInterfaceEntryData.valid);
      return(NULL);
     }
 
  
  
        
  switch (nominator)
  {
  case -1:
  case I_mgmdRouterInterfaceIfIndex:
  case I_mgmdRouterInterfaceQuerierType:
    break;

  case I_mgmdRouterInterfaceQuerier:
     
       if (usmDbMgmdInterfaceQuerierGet(USMDB_UNIT_CURRENT, familyType, intIfNum, &rtrIntfIp) == L7_SUCCESS)
       {
         if(familyType==L7_AF_INET)
         {
          

          if(SafeMakeOctetString(&mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerier,(L7_uchar8*)
                          &rtrIntfIp.addr.ipv4.s_addr, sizeof(rtrIntfIp.addr.ipv4.s_addr))==L7_TRUE)

                  SET_VALID(I_mgmdRouterInterfaceQuerier, mgmdRouterInterfaceEntryData.valid);

         }
        else if(familyType==L7_AF_INET6)
         {
           if( SafeMakeOctetString(&mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerier,rtrIntfIp.addr.ipv6.in6.addr8,
                                                 sizeof(rtrIntfIp.addr.ipv6.in6.addr8))==L7_TRUE)
                  SET_VALID(I_mgmdRouterInterfaceQuerier, mgmdRouterInterfaceEntryData.valid);

         }
        
      }
           
    break;

  case I_mgmdRouterInterfaceQueryInterval:
     if (usmDbMgmdInterfaceQueryIntervalGet(USMDB_UNIT_CURRENT, familyType, intIfNum,
              &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQueryInterval) == L7_SUCCESS)
     
        SET_VALID(I_mgmdRouterInterfaceQueryInterval, mgmdRouterInterfaceEntryData.valid);
    break;
   
  case I_mgmdRouterInterfaceStatus:
     if (snmpMgmdInterfaceRowStatusGet(USMDB_UNIT_CURRENT, intIfNum,
              &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceStatus, familyType) == L7_SUCCESS)
     
        SET_VALID(I_mgmdRouterInterfaceStatus, mgmdRouterInterfaceEntryData.valid);
    break;


  case I_mgmdRouterInterfaceVersion:
     if (usmDbMgmdInterfaceVersionGet(USMDB_UNIT_CURRENT, familyType, intIfNum,
              &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceVersion) == L7_SUCCESS)
     
        SET_VALID(I_mgmdRouterInterfaceVersion, mgmdRouterInterfaceEntryData.valid);
    break;


  case I_mgmdRouterInterfaceQueryMaxResponseTime:
     if (usmDbMgmdInterfaceQueryMaxResponseTimeGet(USMDB_UNIT_CURRENT, familyType, intIfNum,
               &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQueryMaxResponseTime) == L7_SUCCESS)
     
        SET_VALID(I_mgmdRouterInterfaceQueryMaxResponseTime, mgmdRouterInterfaceEntryData.valid);
    break;
  
  case I_mgmdRouterInterfaceQuerierUpTime:
     if (snmpMgmdInterfaceQuerierUpTimeGet(USMDB_UNIT_CURRENT, intIfNum,
               &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerierUpTime, familyType) == L7_SUCCESS)
     
        SET_VALID(I_mgmdRouterInterfaceQuerierUpTime, mgmdRouterInterfaceEntryData.valid);
    break;

      
  case I_mgmdRouterInterfaceQuerierExpiryTime:
     if (snmpMgmdInterfaceQuerierExpiryTimeGet(USMDB_UNIT_CURRENT, intIfNum, 
                &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceQuerierExpiryTime, familyType) == L7_SUCCESS)
     
        SET_VALID(I_mgmdRouterInterfaceQuerierExpiryTime, mgmdRouterInterfaceEntryData.valid);
    break;
      
  case I_mgmdRouterInterfaceWrongVersionQueries:
     if (usmDbMgmdInterfaceWrongVersionQueriesGet(USMDB_UNIT_CURRENT, familyType, intIfNum,
                &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceWrongVersionQueries) == L7_SUCCESS)
     
          SET_VALID(I_mgmdRouterInterfaceWrongVersionQueries, mgmdRouterInterfaceEntryData.valid);
    break;

  case I_mgmdRouterInterfaceJoins:
     if (usmDbMgmdInterfaceJoinsGet(USMDB_UNIT_CURRENT, familyType, intIfNum, 
               &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceJoins) == L7_SUCCESS)
              
           SET_VALID(I_mgmdRouterInterfaceJoins, mgmdRouterInterfaceEntryData.valid);
        
    break;
  
  case I_mgmdRouterInterfaceProxyIfIndex:
       mgmdRouterInterfaceEntryData.mgmdRouterInterfaceProxyIfIndex = 0;
         SET_VALID(I_mgmdRouterInterfaceProxyIfIndex, mgmdRouterInterfaceEntryData.valid);
       break;

  case I_mgmdRouterInterfaceGroups:
   if (usmDbMgmdInterfaceGroupsGet(USMDB_UNIT_CURRENT, familyType, intIfNum,
             &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceGroups) == L7_SUCCESS)

      SET_VALID(I_mgmdRouterInterfaceGroups, mgmdRouterInterfaceEntryData.valid);
  break;

  case I_mgmdRouterInterfaceRobustness:
   if (usmDbMgmdInterfaceRobustnessGet(USMDB_UNIT_CURRENT, familyType, intIfNum,
             &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceRobustness) == L7_SUCCESS)

      SET_VALID(I_mgmdRouterInterfaceRobustness, mgmdRouterInterfaceEntryData.valid);
  break;

  
  case I_mgmdRouterInterfaceLastMembQueryIntvl:
   if (usmDbMgmdInterfaceLastMembQueryIntervalGet(USMDB_UNIT_CURRENT, familyType, intIfNum, 
             &mgmdRouterInterfaceEntryData.mgmdRouterInterfaceLastMembQueryIntvl) == L7_SUCCESS)

      SET_VALID(I_mgmdRouterInterfaceLastMembQueryIntvl, mgmdRouterInterfaceEntryData.valid);
  break;

  default:
     /* unknown nominator */
     return(NULL);
     break;
 }

 if (nominator >= 0 && !VALID(nominator, mgmdRouterInterfaceEntryData.valid))
     return(NULL);
 return(&mgmdRouterInterfaceEntryData);

}

#ifdef SETS
int
k_mgmdRouterInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_mgmdRouterInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mgmdRouterInterfaceEntry_set_defaults(doList_t *dp)
{
    mgmdRouterInterfaceEntry_t *data = (mgmdRouterInterfaceEntry_t *) (dp->data);
    if ((data->mgmdRouterInterfaceQuerier = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->mgmdRouterInterfaceWrongVersionQueries = (SR_UINT32) 0;
    data->mgmdRouterInterfaceJoins = (SR_UINT32) 0;

    SET_VALID(I_mgmdRouterInterfaceWrongVersionQueries, data->valid);
    SET_VALID(I_mgmdRouterInterfaceJoins, data->valid);
    return NO_ERROR;
}

int
k_mgmdRouterInterfaceEntry_set(mgmdRouterInterfaceEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

  L7_uint32 intIfNum;
  L7_uint32 familyType;
  /* translate the extIntnum to internal interface num*/
  if (usmDbIntIfNumFromExtIfNum(data->mgmdRouterInterfaceIfIndex, &intIfNum) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if (data->mgmdRouterInterfaceQuerierType == D_mgmdRouterInterfaceQuerierType_ipv4)
    familyType = L7_AF_INET;
  else if (data->mgmdRouterInterfaceQuerierType == D_mgmdRouterInterfaceQuerierType_ipv6)
    familyType = L7_AF_INET6;
  else
    {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
    }

  if (VALID(I_mgmdRouterInterfaceQueryInterval, data->valid) &&
     usmDbMgmdInterfaceQueryIntervalSet(USMDB_UNIT_CURRENT, familyType, intIfNum, data->mgmdRouterInterfaceQueryInterval) != L7_SUCCESS)
  {
    CLR_VALID(I_mgmdRouterInterfaceQueryInterval, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_mgmdRouterInterfaceVersion, data->valid) &&
      usmDbMgmdInterfaceVersionSet(USMDB_UNIT_CURRENT, familyType, intIfNum, data->mgmdRouterInterfaceVersion) != L7_SUCCESS)
  {
    CLR_VALID(I_mgmdRouterInterfaceVersion, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_mgmdRouterInterfaceQueryMaxResponseTime, data->valid) &&
      usmDbMgmdInterfaceQueryMaxResponseTimeSet(USMDB_UNIT_CURRENT, familyType, intIfNum, data->mgmdRouterInterfaceQueryMaxResponseTime) != L7_SUCCESS)
  {
    CLR_VALID(I_mgmdRouterInterfaceQueryMaxResponseTime, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_mgmdRouterInterfaceProxyIfIndex, data->valid))
  {
    /* Not supported */
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_mgmdRouterInterfaceRobustness, data->valid) &&
      usmDbMgmdInterfaceRobustnessSet(USMDB_UNIT_CURRENT, familyType, intIfNum, data->mgmdRouterInterfaceRobustness) != L7_SUCCESS)
  {
    CLR_VALID(I_mgmdRouterInterfaceRobustness, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_mgmdRouterInterfaceLastMembQueryIntvl, data->valid) &&
      usmDbMgmdInterfaceLastMembQueryIntervalSet(USMDB_UNIT_CURRENT, familyType, intIfNum, data->mgmdRouterInterfaceLastMembQueryIntvl) != L7_SUCCESS)
  {
    CLR_VALID(I_mgmdRouterInterfaceLastMembQueryIntvl, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if ( VALID(I_mgmdRouterInterfaceStatus, data->valid))
  {
    if (snmpMgmdInterfaceRowStatusSet(USMDB_UNIT_CURRENT,intIfNum,data->mgmdRouterInterfaceStatus,familyType) != L7_SUCCESS)
    {
      CLR_VALID(I_mgmdRouterInterfaceStatus, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }
  return NO_ERROR;
}

#ifdef SR_mgmdRouterInterfaceEntry_UNDO
/* add #define SR_mgmdRouterInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the mgmdRouterInterfaceEntry family.
 */
int
mgmdRouterInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mgmdRouterInterfaceEntry_UNDO */

#endif /* SETS */



mgmdHostCacheEntry_t *
k_mgmdHostCacheEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 mgmdHostCacheAddressType,
                         OctetString * mgmdHostCacheAddress,
                         SR_INT32 mgmdHostCacheIfIndex)
{
#ifdef MGMD_SUPPORTED

 
static mgmdHostCacheEntry_t mgmdHostCacheEntryData;

   /*
    * put your code to retrieve the information here
    */
   static L7_BOOL firstTime = L7_TRUE;
   L7_uint32 intIfNum = 0;
   L7_uint32 lastReporterIp = 0;
   L7_uint32 hostCacheAddress = 0;
   L7_uint32 seconds = 0;
   L7_uint32 familyType;
   L7_inet_addr_t lastReportIp;
   L7_inet_addr_t hostCacheAddr;


   ZERO_VALID(mgmdHostCacheEntryData.valid);
   /* Populating the indices one by one*/

   /* Populating Host Cache Address Type */
   if( (mgmdHostCacheAddressType <= D_mgmdHostCacheAddressType_ipv4) && searchType == NEXT )
   {
     mgmdHostCacheEntryData.mgmdHostCacheAddressType = D_mgmdHostCacheAddressType_ipv4;
   }
   else if((mgmdHostCacheAddressType > D_mgmdHostCacheAddressType_ipv4)
          ||(mgmdHostCacheAddressType < D_mgmdHostCacheAddressType_ipv4)) 
   { 
     /* Only IP v4 is supported now */
     ZERO_VALID(mgmdHostCacheEntryData.valid);
     return(NULL);
   }
   SET_VALID(I_mgmdHostCacheAddressType, mgmdHostCacheEntryData.valid); 

   /* Populating Host Cache Addres */
   mgmdHostCacheEntryData.mgmdHostCacheAddress = mgmdHostCacheAddress;

   /* Converting the Host Cache Address in Octet String to IP Address */
   hostCacheAddress = OctetStringToIP(mgmdHostCacheEntryData.mgmdHostCacheAddress);
   
   if ((firstTime == L7_TRUE) && (searchType == NEXT))
   {
     firstTime = L7_FALSE;
     hostCacheAddress = 0;
   }
     
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &hostCacheAddress, &hostCacheAddr))
     return(NULL);
   if(mgmdHostCacheAddressType == D_mgmdHostCacheAddressType_ipv4)
   {
     familyType = L7_AF_INET;
   }
   else
   {
     familyType = L7_AF_INET6;
   }
   inetAddressReset(&lastReportIp);

   /* Populating InterfaceIfIndex */
   mgmdHostCacheEntryData.mgmdHostCacheIfIndex = mgmdHostCacheIfIndex;
   SET_VALID(I_mgmdHostCacheIfIndex, mgmdHostCacheEntryData.valid);


   if(usmDbIntIfNumFromExtIfNum(mgmdHostCacheEntryData.mgmdHostCacheIfIndex, 
                                &intIfNum) != L7_SUCCESS)
   {
     if(searchType == EXACT)
     {
       firstTime =  L7_TRUE;
       ZERO_VALID(mgmdHostCacheEntryData.valid);
       return(NULL);
     } 

     if((usmDbGetNextExtIfNumber(mgmdHostCacheEntryData.mgmdHostCacheIfIndex,
            &mgmdHostCacheEntryData.mgmdHostCacheIfIndex) != L7_SUCCESS) ||
        (usmDbIntIfNumFromExtIfNum(mgmdHostCacheEntryData.mgmdHostCacheIfIndex, 
            &intIfNum) != L7_SUCCESS))
     {
       intIfNum = L7_MAX_INTERFACE_COUNT + 1;
     }
   }
   if ( (searchType == EXACT) ?
         (usmDbMgmdHostCacheEntryGet(USMDB_UNIT_CURRENT, familyType, &hostCacheAddr, intIfNum) != L7_SUCCESS) :
      ( (usmDbMgmdHostCacheEntryGet(USMDB_UNIT_CURRENT, familyType, &hostCacheAddr, intIfNum) != L7_SUCCESS) &&
           (usmDbMgmdHostCacheEntryNextGet(USMDB_UNIT_CURRENT, familyType, &hostCacheAddr, &intIfNum) != L7_SUCCESS)) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdHostCacheEntryData.mgmdHostCacheIfIndex) != L7_SUCCESS)
      )
    {
      firstTime =  L7_TRUE;
      ZERO_VALID(mgmdHostCacheEntryData.valid);
      return(NULL);
    }
    

   /* copy the new Host Cache Address to the outgoing data structure 
     by converting the IP Address to Octet String */
   mgmdHostCacheEntryData.mgmdHostCacheAddress = IPToOctetString(hostCacheAddress);
   SET_VALID(I_mgmdHostCacheAddress, mgmdHostCacheEntryData.valid);
   switch (nominator)
   {
    case -1:
    case I_mgmdHostCacheAddressType:
    case I_mgmdHostCacheAddress:
    case I_mgmdHostCacheIfIndex:
      break;
    
   case I_mgmdHostCacheLastReporter:
    if (usmDbMgmdHostCacheLastReporterGet(USMDB_UNIT_CURRENT, L7_AF_INET, &hostCacheAddr, intIfNum, 
                                      &lastReportIp) == L7_SUCCESS)
    {
      if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &lastReportIp, &lastReporterIp))
        return (NULL);
      mgmdHostCacheEntryData.mgmdHostCacheLastReporter = IPToOctetString(lastReporterIp);
      SET_VALID(I_mgmdHostCacheLastReporter, mgmdHostCacheEntryData.valid);
    }
    break;

      
   case I_mgmdHostCacheUpTime:
    if (usmDbMgmdHostCacheUpTimeGet(USMDB_UNIT_CURRENT, L7_AF_INET, &hostCacheAddr, intIfNum, &seconds) == L7_SUCCESS 
        && SafeMakeTimeTicksFromSeconds(seconds, &mgmdHostCacheEntryData.mgmdHostCacheUpTime) == L7_TRUE)
      SET_VALID(I_mgmdHostCacheUpTime, mgmdHostCacheEntryData.valid);
      break;

         
   case I_mgmdHostCacheStatus:
    if (snmpMgmdHostCacheRowStatusGet(USMDB_UNIT_CURRENT, hostCacheAddress, intIfNum, 
                                   &mgmdHostCacheEntryData.mgmdHostCacheStatus) == L7_SUCCESS)
    SET_VALID(I_mgmdHostCacheStatus, mgmdHostCacheEntryData.valid);
    break;

   case I_mgmdHostCacheSourceFilterMode:
    if (usmDbMgmdHostCacheGroupSourceFilterModeGet(USMDB_UNIT_CURRENT, L7_AF_INET, &hostCacheAddr, intIfNum,
                                      &mgmdHostCacheEntryData.mgmdHostCacheSourceFilterMode) == L7_SUCCESS)
      SET_VALID(I_mgmdHostCacheSourceFilterMode, mgmdHostCacheEntryData.valid);
      break;
 
  default:
     /* unknown nominator */
     return(NULL);
     break;
 }

 if (nominator >= 0 && !VALID(nominator, mgmdHostCacheEntryData.valid))
     return(NULL);
   return(&mgmdHostCacheEntryData);
#endif /*MGMD_SUPPORTED*/
return(NULL);
}
#ifdef SETS
int
k_mgmdHostCacheEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_mgmdHostCacheEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mgmdHostCacheEntry_set_defaults(doList_t *dp)
{
    mgmdHostCacheEntry_t *data = (mgmdHostCacheEntry_t *) (dp->data);

    if ((data->mgmdHostCacheLastReporter = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mgmdHostCacheEntry_set(mgmdHostCacheEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mgmdHostCacheEntry_UNDO
/* add #define SR_mgmdHostCacheEntry_UNDO in sitedefs.h to
 * include the undo routine for the mgmdHostCacheEntry family.
 */
int
mgmdHostCacheEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mgmdHostCacheEntry_UNDO */

#endif /* SETS */

mgmdRouterCacheEntry_t *
k_mgmdRouterCacheEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 mgmdRouterCacheAddressType,
                           OctetString * mgmdRouterCacheAddress,
                           SR_INT32 mgmdRouterCacheIfIndex)
{
  static mgmdRouterCacheEntry_t mgmdRouterCacheEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL firstCall = L7_TRUE;
  static L7_uint32 intIfNum = 0;
  L7_uint32 tempIntIfNumV4 = 0, tempIntIfNumV6 = 0, tempIntIfNumV6First = 0;
  SR_UINT32  routerCacheAddress = 0;
  L7_inet_addr_t routerAddr;
  L7_inet_addr_t lastReportIp;
  static L7_uchar8 familyType=L7_AF_INET;

  ZERO_VALID(mgmdRouterCacheEntryData.valid); 
  inetAddressZeroSet(L7_AF_INET,&routerAddr);


  if (firstTime == L7_TRUE)
  {
    firstTime=L7_FALSE;
    mgmdRouterCacheEntryData.mgmdRouterCacheAddress= MakeOctetString(NULL, 0);
    mgmdRouterCacheEntryData.mgmdRouterCacheLastReporter=MakeOctetString(NULL, 0);
  }

  /*set the indices to zero for the first time*/
  if (firstCall == L7_TRUE)
  {
    firstCall=L7_FALSE;
    if (SafeMakeOctetString(&mgmdRouterCacheEntryData.mgmdRouterCacheAddress, NULL, 0)
                          != L7_TRUE)
    {
      firstCall = L7_TRUE;
      ZERO_VALID(mgmdRouterCacheEntryData.valid);
      return L7_NULL;
    }
    if (SafeMakeOctetString(&mgmdRouterCacheEntryData.mgmdRouterCacheLastReporter, NULL, 0)
                          != L7_TRUE)
    {
      firstCall = L7_TRUE;
      ZERO_VALID(mgmdRouterCacheEntryData.valid);
      return L7_NULL;
    }
    tempIntIfNumV6First = mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex;
  }
  /*get the family type*/
  if( mgmdRouterCacheAddressType<D_mgmdRouterCacheAddressType_ipv4 && searchType == NEXT )
         familyType=mgmdRouterCacheAddressType = D_mgmdRouterCacheAddressType_ipv4;
  else if( mgmdRouterCacheAddressType==D_mgmdRouterCacheAddressType_ipv4)
         familyType=L7_AF_INET;
  else if( mgmdRouterCacheAddressType==D_mgmdRouterCacheAddressType_ipv6)
        familyType=L7_AF_INET6;
  else
  {
    firstCall = L7_TRUE;
    ZERO_VALID(mgmdRouterCacheEntryData.valid);
    return L7_NULL;
  }

   /*get the ipv4 address from octetString*/
  if(familyType==L7_AF_INET && mgmdRouterCacheAddress->length!=0)
  {
    routerCacheAddress = OctetStringToIP(mgmdRouterCacheAddress);
    inetAddressSet(L7_AF_INET,&routerCacheAddress,&routerAddr);
  }
  /*get the ipv6 address from octetString*/
  else if(familyType==L7_AF_INET6 && mgmdRouterCacheAddress->length!=0)
  {
    routerAddr.family=L7_AF_INET6;
    tempIntIfNumV6 = mgmdRouterCacheIfIndex;
    memcpy(routerAddr.addr.ipv6.in6.addr8,mgmdRouterCacheAddress->octet_ptr,sizeof(routerAddr.addr.ipv6.in6.addr8));
  }

#if 0
  /*populating the interface index*/
  if(usmDbIntIfNumFromExtIfNum(mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex,
                                &intIfNum) != L7_SUCCESS)
  {
    if(searchType == EXACT )
    {
      ZERO_VALID(mgmdRouterCacheEntryData.valid);
      return (NULL);
    }

    if((usmDbGetNextExtIfNumber(mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex,
           &mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex) != L7_SUCCESS) ||
       (usmDbIntIfNumFromExtIfNum(mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex,
           &intIfNum) != L7_SUCCESS))
    {
      intIfNum = L7_MAX_INTERFACE_COUNT + 1;
    }
  }
#endif

  if (searchType == EXACT) 
  {
    if ((usmDbIntIfNumFromExtIfNum (mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex, &intIfNum) != L7_SUCCESS) ||
        (usmDbMgmdCacheEntryGet(USMDB_UNIT_CURRENT,familyType, &routerAddr, intIfNum) != L7_SUCCESS))
    {   
      firstCall = L7_TRUE;
      ZERO_VALID(mgmdRouterCacheEntryData.valid);
      return (NULL);
    }
  }
  else if(searchType == NEXT)
  {
    if(familyType==L7_AF_INET)
    {
      tempIntIfNumV4 = mgmdRouterCacheIfIndex;

      if (((usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV4) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV4, &tempIntIfNumV4) != L7_SUCCESS)) ||
          (usmDbIntIfNumFromExtIfNum(tempIntIfNumV4, &intIfNum) != L7_SUCCESS) ||
          ((snmpMgmdCacheEntryGet(USMDB_UNIT_CURRENT, &routerAddr, intIfNum, nominator,familyType) != L7_SUCCESS) &&
           (snmpMgmdCacheEntryNextGet(USMDB_UNIT_CURRENT, &routerAddr, &intIfNum, nominator,familyType) != L7_SUCCESS)) ||
          (usmDbExtIfNumFromIntIfNum(intIfNum, &tempIntIfNumV4) != L7_SUCCESS))
      {
        familyType = L7_AF_INET6;
        tempIntIfNumV6 = tempIntIfNumV6First;
      }
    }

    /*try to get ipv6 entry*/
    if(familyType==L7_AF_INET6 )
    {
      mgmdRouterCacheEntryData.mgmdRouterCacheAddressType = L7_AF_INET6;

      if (((usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV6) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV6, &tempIntIfNumV6) != L7_SUCCESS)) ||
          (usmDbIntIfNumFromExtIfNum(tempIntIfNumV6, &intIfNum) != L7_SUCCESS) ||
          ((snmpMgmdCacheEntryGet(USMDB_UNIT_CURRENT, &routerAddr, intIfNum, nominator,familyType) != L7_SUCCESS) &&
           (snmpMgmdCacheEntryNextGet(USMDB_UNIT_CURRENT, &routerAddr, &intIfNum, nominator,familyType) != L7_SUCCESS)) ||
          (usmDbExtIfNumFromIntIfNum(intIfNum, &tempIntIfNumV6) != L7_SUCCESS))
      {
        firstCall = L7_TRUE;
        ZERO_VALID(mgmdRouterCacheEntryData.valid);
        return (NULL);
      }
    }
  }

  if(familyType==L7_AF_INET)
  {
    mgmdRouterCacheEntryData.mgmdRouterCacheAddressType=D_mgmdRouterCacheAddressType_ipv4;

    mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex = tempIntIfNumV4;
    if(SafeMakeOctetString(&mgmdRouterCacheEntryData.mgmdRouterCacheAddress,(L7_uchar8*)&routerAddr.addr.ipv4.s_addr,
                           sizeof(routerAddr.addr.ipv4.s_addr))==L7_FALSE)
      return(NULL);
  }
  else if(familyType==L7_AF_INET6)  
  {
    mgmdRouterCacheEntryData.mgmdRouterCacheAddressType=D_mgmdRouterCacheAddressType_ipv6;
    mgmdRouterCacheEntryData.mgmdRouterCacheIfIndex = tempIntIfNumV6;
    if( SafeMakeOctetString(&mgmdRouterCacheEntryData.mgmdRouterCacheAddress, 
               routerAddr.addr.ipv6.in6.addr8, sizeof( routerAddr.addr.ipv6.in6.addr8))==L7_FALSE)
      return(NULL);
  }

   switch (nominator)
   {
    case -1:
    case I_mgmdRouterCacheAddressType:
    case I_mgmdRouterCacheAddress:
    case I_mgmdRouterCacheIfIndex:
      break;
    
    case I_mgmdRouterCacheLastReporter:
    
     if(usmDbMgmdCacheLastReporterGet(USMDB_UNIT_CURRENT,  familyType, &routerAddr, intIfNum,
                                      &lastReportIp) == L7_SUCCESS)
       { 
         if(familyType==L7_AF_INET)	
           {
       
             if(SafeMakeOctetString(&mgmdRouterCacheEntryData.mgmdRouterCacheLastReporter,
                 (L7_uchar8*)&lastReportIp.addr.ipv4.s_addr, sizeof(lastReportIp.addr.ipv4.s_addr))==L7_TRUE)
     
                  SET_VALID(I_mgmdRouterCacheLastReporter, mgmdRouterCacheEntryData.valid);

           }
         else if(familyType==L7_AF_INET6) 
           {
            
             if(SafeMakeOctetString(&mgmdRouterCacheEntryData.mgmdRouterCacheLastReporter,
                lastReportIp.addr.ipv6.in6.addr8, sizeof(lastReportIp.addr.ipv6.in6.addr8))==L7_TRUE)
           
                 SET_VALID(I_mgmdRouterCacheLastReporter, mgmdRouterCacheEntryData.valid);
        
           } 
       }
           
       break;

   
   case I_mgmdRouterCacheUpTime:
     if (snmpMgmdCacheUpTimeGet(USMDB_UNIT_CURRENT, &routerAddr, intIfNum,
            &mgmdRouterCacheEntryData.mgmdRouterCacheUpTime, familyType) == L7_SUCCESS)

        SET_VALID(I_mgmdRouterCacheUpTime, mgmdRouterCacheEntryData.valid);
        break;
   
   case I_mgmdRouterCacheExpiryTime:
    if (snmpMgmdCacheExpiryTimeGet(USMDB_UNIT_CURRENT, &routerAddr, intIfNum,
            &mgmdRouterCacheEntryData.mgmdRouterCacheExpiryTime,familyType) == L7_SUCCESS)
        SET_VALID(I_mgmdRouterCacheExpiryTime, mgmdRouterCacheEntryData.valid);

      break; 
         
   case I_mgmdRouterCacheVersion1HostTimer:
    if (snmpMgmdCacheVersion1HostTimerGet(USMDB_UNIT_CURRENT, &routerAddr, intIfNum, 
              &mgmdRouterCacheEntryData.mgmdRouterCacheVersion1HostTimer,familyType) == L7_SUCCESS)
      SET_VALID(I_mgmdRouterCacheVersion1HostTimer, mgmdRouterCacheEntryData.valid);
      break;

   case I_mgmdRouterCacheVersion2HostTimer:
    if (snmpMgmdCacheVersion2HostTimerGet(USMDB_UNIT_CURRENT,&routerAddr, intIfNum,
               &mgmdRouterCacheEntryData.mgmdRouterCacheVersion2HostTimer,familyType) == L7_SUCCESS)
      SET_VALID(I_mgmdRouterCacheVersion2HostTimer, mgmdRouterCacheEntryData.valid);
      break;

   case I_mgmdRouterCacheSourceFilterMode:
    if (usmDbMgmdCacheGroupSourceFilterModeGet(USMDB_UNIT_CURRENT,familyType, &routerAddr, intIfNum, 
                            &mgmdRouterCacheEntryData.mgmdRouterCacheSourceFilterMode) == L7_SUCCESS)
      SET_VALID(I_mgmdRouterCacheSourceFilterMode, mgmdRouterCacheEntryData.valid);

      break; 

  default:
     /* unknown nominator */
     return(NULL);
     break;
 }

 if (nominator >= 0 && !VALID(nominator, mgmdRouterCacheEntryData.valid))
     return(NULL);
 return(&mgmdRouterCacheEntryData);
    
}

#ifdef SETS
int
k_mgmdRouterCacheEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_mgmdRouterCacheEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mgmdRouterCacheEntry_set_defaults(doList_t *dp)
{
    mgmdRouterCacheEntry_t *data = (mgmdRouterCacheEntry_t *) (dp->data);
    if ((data->mgmdRouterCacheLastReporter = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mgmdRouterCacheEntry_set(mgmdRouterCacheEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mgmdRouterCacheEntry_UNDO
/* add #define SR_mgmdRouterCacheEntry_UNDO in sitedefs.h to
 * include the undo routine for the mgmdRouterCacheEntry family.
 */
int
mgmdRouterCacheEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mgmdRouterCacheEntry_UNDO */

#endif /* SETS */

mgmdInverseHostCacheEntry_t *
k_mgmdInverseHostCacheEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 mgmdInverseHostCacheIfIndex,
                                SR_INT32 mgmdInverseHostCacheAddressType,
                                OctetString * mgmdInverseHostCacheAddress)
{
#ifdef MGMD_SUPPORTED
   static mgmdInverseHostCacheEntry_t mgmdInverseHostCacheEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 intIfNum = 0;
  L7_uint32 inverseHostCacheAddress = 0;
  L7_inet_addr_t inverseHostCacheAddr;
  ZERO_VALID(mgmdInverseHostCacheEntryData.valid);
  
   /* Populating the indices one by one*/

   /* Populating Inverse Host Cache Address Type */
   if( (mgmdInverseHostCacheAddressType <= D_mgmdInverseHostCacheAddressType_ipv4) && searchType == NEXT )
   {
     mgmdInverseHostCacheEntryData.mgmdInverseHostCacheAddressType = D_mgmdInverseHostCacheAddressType_ipv4;
     SET_VALID(I_mgmdInverseHostCacheAddressType, mgmdInverseHostCacheEntryData.valid); 
   }
   else if((mgmdInverseHostCacheAddressType > D_mgmdInverseHostCacheAddressType_ipv4)
          ||(mgmdInverseHostCacheAddressType < D_mgmdInverseHostCacheAddressType_ipv4)) 
   { 
     /* Only IP v4 is supported now */
     ZERO_VALID(mgmdInverseHostCacheEntryData.valid);
     return(NULL);
   }

   /* Populating Inverse Host Cache Addres */
   mgmdInverseHostCacheEntryData.mgmdInverseHostCacheAddress = mgmdInverseHostCacheAddress;

   /* Converting the Inverse Host Cache Address in Octet String to IP Address */
   inverseHostCacheAddress = OctetStringToIP(mgmdInverseHostCacheEntryData.mgmdInverseHostCacheAddress);
   
   if ((firstTime == L7_TRUE) && (searchType == NEXT))
   {
     firstTime = L7_FALSE;
     inverseHostCacheAddress = 0;
   }
     
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &inverseHostCacheAddress, &inverseHostCacheAddr))
     return(NULL);

   /* Populating InterfaceIfIndex */
   mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex = mgmdInverseHostCacheIfIndex;
   SET_VALID(I_mgmdInverseHostCacheIfIndex, mgmdInverseHostCacheEntryData.valid);

   if ( (searchType == EXACT) ?
     ( (usmDbIntIfNumFromExtIfNum(mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex, &intIfNum) != L7_SUCCESS) ||
       (usmDbMgmdHostInverseCacheEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum, &inverseHostCacheAddr) != L7_SUCCESS) ) :
      
     ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF,  0, 
                                      mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex) != L7_SUCCESS) &&
         (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                   mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex, 
                                   &mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex) != L7_SUCCESS) ) ||
      (usmDbIntIfNumFromExtIfNum(mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex, &intIfNum) != L7_SUCCESS) ||
      ( (usmDbMgmdHostInverseCacheEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,&inverseHostCacheAddr) != L7_SUCCESS) &&
        (usmDbMgmdHostInverseCacheEntryGetNext(USMDB_UNIT_CURRENT, L7_AF_INET, &intIfNum, &inverseHostCacheAddr) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdInverseHostCacheEntryData.mgmdInverseHostCacheIfIndex) != L7_SUCCESS) )
     )
   {
     firstTime =  L7_TRUE;
     ZERO_VALID(mgmdInverseHostCacheEntryData.valid);
     return(NULL);
   }
   
   /* copy the new Inverse Host Cache Address to the outgoing data structure 
     by converting the IP Address to Octet String */
   mgmdInverseHostCacheEntryData.mgmdInverseHostCacheAddress = IPToOctetString(inverseHostCacheAddress);
   SET_VALID(I_mgmdInverseHostCacheAddress, mgmdInverseHostCacheEntryData.valid);
   switch (nominator)
   {
    case -1:
    case I_mgmdInverseHostCacheIfIndex:
    case I_mgmdInverseHostCacheAddressType:
    case I_mgmdInverseHostCacheAddress:
      break;
   
    default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, mgmdInverseHostCacheEntryData.valid))
     return(NULL);
   /*
    * put your code to retrieve the information here
    */
   return(&mgmdInverseHostCacheEntryData);
#endif  /*MGMD_SUPPORTED*/
   return(NULL);
}

mgmdInverseRouterCacheEntry_t *
k_mgmdInverseRouterCacheEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 mgmdInverseRouterCacheIfIndex,
                                  SR_INT32 mgmdInverseRouterCacheAddressType,
                                  OctetString * mgmdInverseRouterCacheAddress)
{
  static mgmdInverseRouterCacheEntry_t mgmdInverseRouterCacheEntryData;
  static L7_BOOL firstTime=L7_TRUE;
  static L7_BOOL firstCall=L7_TRUE;
  static L7_uint32 intIfNum = 0;
  L7_uint32 tempIntIfNumV4 = 0, tempIntIfNumV6 = 0, tempIntIfNumV6First = 0;
  SR_UINT32 inverseRouterCacheAddress = 0;
  L7_inet_addr_t inverseRouterCacheAddr4,inverseRouterCacheAddr6, routerAddr;
  L7_uint32 familyType=L7_AF_INET;

  ZERO_VALID(mgmdInverseRouterCacheEntryData.valid);
  inetAddressZeroSet(L7_AF_INET,&inverseRouterCacheAddr4);
  inetAddressZeroSet(L7_AF_INET6,&inverseRouterCacheAddr6);

  if (firstTime == L7_TRUE)
  {
    firstTime=L7_FALSE;
    mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddress =  MakeOctetString(NULL,0);
  }

  /*set the indices to zero for the first time*/
  if (firstCall == L7_TRUE)
  {
    firstCall=L7_FALSE;
    if (SafeMakeOctetString(&mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddress, NULL, 0)
                          != L7_TRUE)
    {
      firstCall = L7_TRUE;
      ZERO_VALID(mgmdInverseRouterCacheEntryData.valid);
      return L7_NULL;
    }
    tempIntIfNumV6First = mgmdInverseRouterCacheIfIndex;
  }

  /* Populating Router Cache Address Type */
  if(mgmdInverseRouterCacheAddressType<D_mgmdInverseRouterCacheAddressType_ipv4 && searchType==NEXT)
        familyType=mgmdInverseRouterCacheAddressType=D_mgmdInverseRouterCacheAddressType_ipv4;
  else if( mgmdInverseRouterCacheAddressType == D_mgmdInverseRouterCacheAddressType_ipv4)
         familyType=L7_AF_INET;
  else if( mgmdInverseRouterCacheAddressType == D_mgmdInverseRouterCacheAddressType_ipv6)
         familyType=L7_AF_INET6;
  else
  {
    firstCall = L7_TRUE;
    ZERO_VALID(mgmdInverseRouterCacheEntryData.valid);
    return L7_NULL;
  }

  /*populating the ipv4 address from ocetString type*/ 
  if(familyType==L7_AF_INET && mgmdInverseRouterCacheAddress->length!=0)
  {
    inverseRouterCacheAddress = OctetStringToIP(mgmdInverseRouterCacheAddress);
    inetAddressSet(L7_AF_INET, &inverseRouterCacheAddress, &inverseRouterCacheAddr4);
  }
  /*populating the ipv6 address from ocetString type*/
  else if(familyType==L7_AF_INET6 && mgmdInverseRouterCacheAddress->length!=0)
  {
    inverseRouterCacheAddr6.family=L7_AF_INET6;
    tempIntIfNumV6 = mgmdInverseRouterCacheIfIndex;
    memcpy(inverseRouterCacheAddr6.addr.ipv6.in6.addr8,mgmdInverseRouterCacheAddress->octet_ptr,16);
  }

  if(searchType == EXACT)
  {
    if((usmDbMgmdCacheIntfEntryGet(USMDB_UNIT_CURRENT, familyType, intIfNum, &routerAddr) == L7_SUCCESS) &&
       (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheIfIndex) == L7_SUCCESS))
    {   
      firstCall = L7_TRUE;
      ZERO_VALID(mgmdInverseRouterCacheEntryData.valid);
      return (NULL);
    }
  }
  else if(searchType == NEXT )
  {
    if(familyType==L7_AF_INET)
    {
      tempIntIfNumV4 = mgmdInverseRouterCacheIfIndex;

      if (((usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV4) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV4, &tempIntIfNumV4) != L7_SUCCESS)) ||
          (usmDbIntIfNumFromExtIfNum(tempIntIfNumV4, &intIfNum) != L7_SUCCESS) ||
          ((usmDbMgmdCacheIntfEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum, &inverseRouterCacheAddr4) != L7_SUCCESS) &&
           (usmDbMgmdCacheIntfEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &intIfNum, &inverseRouterCacheAddr4) != L7_SUCCESS)) ||
          (usmDbExtIfNumFromIntIfNum(intIfNum, &tempIntIfNumV4) != L7_SUCCESS))
      {
        familyType = L7_AF_INET6;
        tempIntIfNumV6 = tempIntIfNumV6First;
      }
    }

    if(familyType==L7_AF_INET6 )
    {
      mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddressType = L7_AF_INET6;

      if (((usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV6) != L7_SUCCESS) &&
           (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, tempIntIfNumV6, &tempIntIfNumV6) != L7_SUCCESS)) ||
          (usmDbIntIfNumFromExtIfNum(tempIntIfNumV6, &intIfNum) != L7_SUCCESS) ||
          ((usmDbMgmdCacheIntfEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET6, intIfNum, &inverseRouterCacheAddr6) != L7_SUCCESS) &&
           (usmDbMgmdCacheIntfEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET6, &intIfNum, &inverseRouterCacheAddr6) != L7_SUCCESS)) ||
          (usmDbExtIfNumFromIntIfNum(intIfNum, &tempIntIfNumV6) != L7_SUCCESS))
      {
        firstCall = L7_TRUE;
        ZERO_VALID(mgmdInverseRouterCacheEntryData.valid);
        return (NULL);
      }
    }
  }

  if(familyType==L7_AF_INET)
  {
    mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddressType=D_mgmdInverseRouterCacheAddressType_ipv4;
    mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheIfIndex = tempIntIfNumV4;
    inetCopy(&routerAddr,&inverseRouterCacheAddr4);
    if(SafeMakeOctetString(&mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddress,(L7_uchar8*)&inverseRouterCacheAddr4.addr.ipv4.s_addr, sizeof(inverseRouterCacheAddr4.addr.ipv4.s_addr))==L7_FALSE)
      return(NULL);
  }
  else if(familyType==L7_AF_INET6)  
  {
    mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddressType=D_mgmdInverseRouterCacheAddressType_ipv6;
    mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheIfIndex = tempIntIfNumV6;
    inetCopy(&routerAddr,&inverseRouterCacheAddr6);
    if(SafeMakeOctetString(&mgmdInverseRouterCacheEntryData.mgmdInverseRouterCacheAddress,(L7_uchar8*)inverseRouterCacheAddr6.addr.ipv6.in6.addr8, sizeof(inverseRouterCacheAddr6.addr.ipv6.in6.addr8))==L7_FALSE)
      return(NULL);
  }

  SET_VALID(I_mgmdInverseRouterCacheAddress, mgmdInverseRouterCacheEntryData.valid);

  switch (nominator)
   {
    case -1:
    case I_mgmdInverseRouterCacheIfIndex:
    case I_mgmdInverseRouterCacheAddressType:
    case I_mgmdInverseRouterCacheAddress:
      break;
   
    default:
     /* unknown nominator */
       return(NULL);
     break;
   }


   if (nominator >= 0 && !VALID(nominator, mgmdInverseRouterCacheEntryData.valid))
       return(NULL);
   
   return(&mgmdInverseRouterCacheEntryData);
}

mgmdHostSrcListEntry_t *
k_mgmdHostSrcListEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 mgmdHostSrcListAddressType,
                           OctetString * mgmdHostSrcListAddress,
                           SR_INT32 mgmdHostSrcListIfIndex,
                           OctetString * mgmdHostSrcListHostAddress)
{
#ifdef MGMD_SUPPORTED
   static mgmdHostSrcListEntry_t mgmdHostSrcListEntryData;
   
  static L7_BOOL firstTime = L7_TRUE;
  static L7_uint32 intIfNum = 1;
  L7_uint32 hostSrcListAddress = 0;
  L7_uint32 hostSrcListHostAddress = 0;
  L7_inet_addr_t hostSrcListAddr, hostSrcListHostAddr;
  ZERO_VALID(mgmdHostSrcListEntryData.valid);
  /* Populating the indices one by one*/

  /* Populating Host Src List Address Type */

   if( mgmdHostSrcListAddressType == D_mgmdHostSrcListAddressType_ipv4)
   {
     mgmdHostSrcListEntryData.mgmdHostSrcListAddressType = D_mgmdHostSrcListAddressType_ipv4;
     SET_VALID(I_mgmdHostSrcListAddressType, mgmdHostSrcListEntryData.valid);
   }

   else if( (mgmdHostSrcListAddressType < D_mgmdHostSrcListAddressType_ipv4) && searchType == NEXT )
   {
     mgmdHostSrcListEntryData.mgmdHostSrcListAddressType = D_mgmdHostSrcListAddressType_ipv4;
     SET_VALID(I_mgmdHostSrcListAddressType, mgmdHostSrcListEntryData.valid); 
   }

   else if((mgmdHostSrcListAddressType > D_mgmdHostSrcListAddressType_ipv4)
          ||(mgmdHostSrcListAddressType < D_mgmdHostSrcListAddressType_ipv4)) 
   { 
     /* Only IP v4 is supported now */
     ZERO_VALID(mgmdHostSrcListEntryData.valid);
     return(NULL);
   }

   /* Populating Host Src List Address */
   mgmdHostSrcListEntryData.mgmdHostSrcListAddress = mgmdHostSrcListAddress;

   /* Converting the Host Src List Address in Octet String to IP Address */
   hostSrcListAddress = OctetStringToIP(mgmdHostSrcListEntryData.mgmdHostSrcListAddress);
   
   /* Populating Host Src List Host Address */
   mgmdHostSrcListEntryData.mgmdHostSrcListHostAddress = mgmdHostSrcListHostAddress;

   /* Converting the Host Src List Host Address in Octet String to IP Address */
   hostSrcListHostAddress = OctetStringToIP(mgmdHostSrcListEntryData.mgmdHostSrcListHostAddress);

   if ((firstTime == L7_TRUE) && (searchType == NEXT))
   {
     firstTime = L7_FALSE;
     hostSrcListAddress = 0;
     hostSrcListHostAddress = 0;
   }
     
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &hostSrcListAddress, &hostSrcListAddr))
     return(NULL);
   if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &hostSrcListHostAddress, &hostSrcListHostAddr))
     return(NULL);

   /* Populating InterfaceIfIndex */
   mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex = mgmdHostSrcListIfIndex;
   SET_VALID(I_mgmdHostSrcListIfIndex, mgmdHostSrcListEntryData.valid);

     if ( (searchType == EXACT) ?
     ( (usmDbIntIfNumFromExtIfNum(mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex, &intIfNum) != L7_SUCCESS) ||
       (usmDbMgmdHostSrcListEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, 
                                     &hostSrcListAddr, 
                                 intIfNum, &hostSrcListHostAddr) != L7_SUCCESS) ) :
      
     ( ( (usmDbExtIfNumTypeCheckValid( USMDB_UNIT_CURRENT, USM_ROUTER_INTF,  0, 
                                      mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex) != L7_SUCCESS) &&
         (usmDbExtIfNumTypeNextGet( USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, 
                                   mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex, 
                                   &mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex) != L7_SUCCESS) ) ||
      (usmDbIntIfNumFromExtIfNum(mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex, &intIfNum) != L7_SUCCESS) ||
      ( (usmDbMgmdHostSrcListEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, 
                                      &hostSrcListAddr, 
                                 intIfNum, &hostSrcListHostAddr) != L7_SUCCESS) &&
        (usmDbMgmdHostSrcListEntryGetNext(USMDB_UNIT_CURRENT, L7_AF_INET, &hostSrcListAddr, 
                                      &intIfNum, &hostSrcListHostAddr) != L7_SUCCESS)) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdHostSrcListEntryData.mgmdHostSrcListIfIndex) != L7_SUCCESS) )
     )
     {
       firstTime =  L7_TRUE;
       ZERO_VALID(mgmdHostSrcListEntryData.valid);
       return(NULL);
    }

  
   /* copy the new SrcList Address and SrcList Host Address to the outgoing data structure 
     by converting the IP Address to Octet String */
   mgmdHostSrcListEntryData.mgmdHostSrcListAddress = IPToOctetString(hostSrcListAddress);
   SET_VALID(I_mgmdHostSrcListAddress, mgmdHostSrcListEntryData.valid);

   mgmdHostSrcListEntryData.mgmdHostSrcListHostAddress = IPToOctetString(hostSrcListHostAddress);
   SET_VALID(I_mgmdHostSrcListHostAddress, mgmdHostSrcListEntryData.valid);

   switch (nominator)
   {
    case -1:
    case I_mgmdHostSrcListAddressType:
    case I_mgmdHostSrcListAddress:
    case I_mgmdHostSrcListIfIndex:
    case I_mgmdHostSrcListHostAddress:
      break;
   
    default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, mgmdHostSrcListEntryData.valid))
     return(NULL);
   return(&mgmdHostSrcListEntryData);
#endif  /*MGMD_SUPPORTED*/
   return(NULL);
}

mgmdRouterSrcListEntry_t *
k_mgmdRouterSrcListEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 mgmdRouterSrcListAddressType,
                             OctetString * mgmdRouterSrcListAddress,
                             SR_INT32 mgmdRouterSrcListIfIndex,
                             OctetString * mgmdRouterSrcListHostAddress)
{
  static mgmdRouterSrcListEntry_t mgmdRouterSrcListEntryData;
  
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 intIfNum = 1,firstValidExtIfNum=1;
  SR_UINT32 routerSrcListAddress = 0;
  SR_UINT32  routerSrcListHostAddress = 0;
  L7_inet_addr_t routerSrcListAddr, routerSrcListHostAddr,routerSrcListAddr6, routerSrcListHostAddr6;
  L7_uchar8 familyType=L7_AF_INET;

  ZERO_VALID(mgmdRouterSrcListEntryData.valid);
  inetAddressZeroSet(L7_AF_INET,&routerSrcListAddr);
  inetAddressZeroSet(L7_AF_INET,&routerSrcListHostAddr);
  inetAddressZeroSet(L7_AF_INET6,&routerSrcListAddr6);
  inetAddressZeroSet(L7_AF_INET6,&routerSrcListHostAddr6);


  
   /* for the first time set the indices to zero*/  
   if (firstTime == L7_TRUE)
   {
     firstTime=L7_FALSE;
     mgmdRouterSrcListEntryData.mgmdRouterSrcListAddress= MakeOctetString(NULL, 0);
     mgmdRouterSrcListEntryData.mgmdRouterSrcListHostAddress=MakeOctetString(NULL, 0);
   }
 
  /* get the family type*/  
  if( mgmdRouterSrcListAddressType<D_mgmdRouterSrcListAddressType_ipv4 && searchType == NEXT )
  
    familyType=mgmdRouterSrcListAddressType = D_mgmdRouterSrcListAddressType_ipv4;
    
  else if( mgmdRouterSrcListAddressType==D_mgmdRouterSrcListAddressType_ipv4 )
      familyType=L7_AF_INET;
  
  else if( mgmdRouterSrcListAddressType==D_mgmdRouterSrcListAddressType_ipv6 )
       familyType=L7_AF_INET6;
  else
   {
     
      ZERO_VALID(mgmdRouterSrcListEntryData.valid);
      return L7_NULL;
   }
  /*get the ipv4 addresses*/
  if(familyType==L7_AF_INET)
   { 
     if(mgmdRouterSrcListAddress->length!=0)
        routerSrcListAddress = OctetStringToIP(mgmdRouterSrcListAddress);
     if (inetAddressSet(L7_AF_INET,&routerSrcListAddress,&routerSrcListAddr) != L7_SUCCESS)
         return L7_NULL;
     if(mgmdRouterSrcListHostAddress->length!=0)
        routerSrcListHostAddress = OctetStringToIP(mgmdRouterSrcListHostAddress);
     if (inetAddressSet(L7_AF_INET,&routerSrcListHostAddress,&routerSrcListHostAddr) != L7_SUCCESS)
         return L7_NULL;

   }
  /*get the ipv6 addresses*/
  else if(familyType==L7_AF_INET6)
   {
     if(mgmdRouterSrcListAddress->length!=0)
      {
       routerSrcListAddr6.family=L7_AF_INET6;
       memcpy(routerSrcListAddr6.addr.ipv6.in6.addr8,mgmdRouterSrcListAddress->octet_ptr,16);
      }
     if(mgmdRouterSrcListHostAddress->length!=0)
      {
       routerSrcListHostAddr6.family=L7_AF_INET6;
       memcpy(routerSrcListHostAddr6.addr.ipv6.in6.addr8,mgmdRouterSrcListHostAddress->octet_ptr,16);
      }
   }
   mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex = mgmdRouterSrcListIfIndex;
   SET_VALID(I_mgmdRouterSrcListIfIndex, mgmdRouterSrcListEntryData.valid);

  /*get the interface index*/

  if(usmDbIntIfNumFromExtIfNum(mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex,
                                &intIfNum) != L7_SUCCESS)
   {
      if(searchType == EXACT )
      {
        ZERO_VALID(mgmdRouterSrcListEntryData.valid);
        return (NULL);
      }

      if((usmDbGetNextExtIfNumber(mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex,
            &mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex) != L7_SUCCESS) ||
        (usmDbIntIfNumFromExtIfNum(mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex,&intIfNum) != L7_SUCCESS))
     
      {
         intIfNum = L7_MAX_INTERFACE_COUNT;
      }
   
   }

  if(mgmdRouterSrcListIfIndex==1)
   firstValidExtIfNum=mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex;
  
  /* get the entry*/
  
  if(searchType == EXACT)
   {
      if(familyType ==L7_AF_INET)
       {
          if((usmDbMgmdSrcListEntryGet(USMDB_UNIT_CURRENT,L7_AF_INET,&routerSrcListAddr,intIfNum, &routerSrcListHostAddr) != L7_SUCCESS)||
            (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex) != L7_SUCCESS))
          {
          
             ZERO_VALID(mgmdRouterSrcListEntryData.valid);
             return (NULL);
          }
       }
      else if (familyType ==L7_AF_INET6)
       {
          if((usmDbMgmdSrcListEntryGet(USMDB_UNIT_CURRENT,L7_AF_INET6,&routerSrcListAddr6,intIfNum, &routerSrcListHostAddr6) != L7_SUCCESS)||
            (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex) != L7_SUCCESS))
         { 
          
            ZERO_VALID(mgmdRouterSrcListEntryData.valid);
            return (NULL);
          }
       }
      else
       return(NULL);
   }/*end of searchType == EXACT*/

  else if(searchType == NEXT)
   {

      if(familyType==L7_AF_INET)
      
        {
          if(((usmDbMgmdSrcListEntryGet(USMDB_UNIT_CURRENT,L7_AF_INET,&routerSrcListAddr,
                                 intIfNum, &routerSrcListHostAddr) != L7_SUCCESS) &&
               (usmDbMgmdSrcListEntryNextGet(USMDB_UNIT_CURRENT,L7_AF_INET, &routerSrcListAddr,
                                      &intIfNum, &routerSrcListHostAddr) != L7_SUCCESS)) ||
               (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex) != L7_SUCCESS) )
     
                 {

                        familyType=L7_AF_INET6;
                        mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex=firstValidExtIfNum;
                        intIfNum = 0;
                 }

       }/*failed to get the ipv4 entry*/

      /*try to get the ipv6 entry*/ 
      if(familyType==L7_AF_INET6 )

       {

         if(((usmDbMgmdSrcListEntryGet(USMDB_UNIT_CURRENT,L7_AF_INET6,&routerSrcListAddr6,
                                 intIfNum, &routerSrcListHostAddr6) != L7_SUCCESS) &&
            (usmDbMgmdSrcListEntryNextGet(USMDB_UNIT_CURRENT,L7_AF_INET6, &routerSrcListAddr6,
                                      &intIfNum, &routerSrcListHostAddr6) != L7_SUCCESS)) ||
             (usmDbExtIfNumFromIntIfNum(intIfNum, &mgmdRouterSrcListEntryData.mgmdRouterSrcListIfIndex) != L7_SUCCESS) )
            
                { 
                   return (NULL);
                }

       }

   }/*end of searchType == NEXT */

  /*convert the addresses to OctetString type depending upon the family type*/

  if(familyType==L7_AF_INET)
   {
     mgmdRouterSrcListEntryData.mgmdRouterSrcListAddressType=D_mgmdRouterSrcListAddressType_ipv4;
     if(SafeMakeOctetString(&mgmdRouterSrcListEntryData.mgmdRouterSrcListAddress,(L7_uchar8*)
          &routerSrcListAddr.addr.ipv4.s_addr, sizeof(routerSrcListAddr.addr.ipv4.s_addr))==L7_FALSE)
           
             return(NULL);
     
     if(SafeMakeOctetString(&mgmdRouterSrcListEntryData.mgmdRouterSrcListHostAddress,(L7_uchar8*)
         &routerSrcListHostAddr.addr.ipv4.s_addr, sizeof(routerSrcListHostAddr.addr.ipv4.s_addr))==L7_FALSE)
           
             return(NULL);

   }
  else if(familyType==L7_AF_INET6)
   {
      
      mgmdRouterSrcListEntryData.mgmdRouterSrcListAddressType=D_mgmdRouterSrcListAddressType_ipv6;  
     if(SafeMakeOctetString(&mgmdRouterSrcListEntryData.mgmdRouterSrcListAddress,
          routerSrcListAddr6.addr.ipv6.in6.addr8, sizeof( routerSrcListAddr6.addr.ipv6.in6.addr8))==L7_FALSE)
  
            return(NULL);
   
     if( SafeMakeOctetString(&mgmdRouterSrcListEntryData.mgmdRouterSrcListHostAddress,
          routerSrcListHostAddr6.addr.ipv6.in6.addr8, sizeof( routerSrcListHostAddr6.addr.ipv6.in6.addr8))==L7_FALSE)

             return(NULL);


   }

  
   SET_VALID(I_mgmdRouterSrcListAddressType, mgmdRouterSrcListEntryData.valid);
   SET_VALID(I_mgmdRouterSrcListAddress, mgmdRouterSrcListEntryData.valid);
   SET_VALID(I_mgmdRouterSrcListHostAddress, mgmdRouterSrcListEntryData.valid);

   switch (nominator)
   {
    case -1:
    case I_mgmdRouterSrcListAddressType:
    case I_mgmdRouterSrcListAddress:
    case I_mgmdRouterSrcListIfIndex:
    case I_mgmdRouterSrcListHostAddress:
      break;
   
    default:
     /* unknown nominator */
     return(NULL);
     break;
   }
   
   if (nominator >= 0 && !VALID(nominator, mgmdRouterSrcListEntryData.valid))
     return(NULL);
   
   return(&mgmdRouterSrcListEntryData);
}
