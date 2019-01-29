/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_fastpathmulticast.c
*
* @purpose FASTPATH Static Multicast Route Private MIB
*
* @component SNMP
*
* @comments
*
* @create
*
* @author cpverne
*
* @end
*
**********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/

#include "k_private_base.h"
#include <inst_lib.h>
#include "usmdb_mib_mcast_api.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"
#include "osapi_support.h"

agentIpStaticMRouteEntry_t *
k_agentIpStaticMRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 agentIpStaticMRouteSrcAddressType,
                               OctetString * agentIpStaticMRouteSrcIpAddr,
                               SR_INT32 agentIpStaticMRouteSrcNetMask)
{
  static agentIpStaticMRouteEntry_t agentIpStaticMRouteEntryData;
  L7_inet_addr_t ipStaticMRouteSrcAddr;
  L7_inet_addr_t ipStaticMRouteSrcMask;
  L7_inet_addr_t ipStaticMRouteRpfAddr;
  L7_uint32 unit = USMDB_UNIT_CURRENT;
  static L7_BOOL initializeMem = L7_TRUE;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allocMem = L7_TRUE;
    
  if (usmDbComponentPresentCheck (unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) != L7_TRUE)
  {
    return L7_NULLPTR;
  }
    
  if((agentIpStaticMRouteSrcAddressType!= L7_NULL) &&
      (agentIpStaticMRouteSrcAddressType != L7_AF_INET) &&
       (agentIpStaticMRouteSrcAddressType != L7_AF_INET6))
  {
    return(NULL);
  }
  memset(&ipStaticMRouteSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&ipStaticMRouteSrcMask, L7_NULL, sizeof(L7_inet_addr_t));
  memset(&ipStaticMRouteRpfAddr, L7_NULL, sizeof(L7_inet_addr_t));

  if (initializeMem == L7_TRUE)
  {
    initializeMem = L7_FALSE;
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr = MakeOctetString (NULL, 0);
    agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr = MakeOctetString (NULL, 0);
  }
 
  if (allocMem == L7_TRUE)
  {
    allocMem = L7_FALSE;
    (void)SafeMakeOctetString(&agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr, NULL, 0);
  }

  /* Clear all the bits*/
  ZERO_VALID (agentIpStaticMRouteEntryData.valid);
  agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = agentIpStaticMRouteSrcAddressType;

  if(agentIpStaticMRouteSrcAddressType == L7_NULL)
  {
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = L7_AF_INET;  
  }
  if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv4)
  {
    L7_uint32 srcV4Addr = 0, srcV4Mask = 0;
    
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      srcV4Addr = 0;
      srcV4Mask= 0;
    }
    else if((firstTime != L7_TRUE) || (searchType == EXACT))  
    {
      srcV4Addr= OctetStringToIP(agentIpStaticMRouteSrcIpAddr);
      srcV4Mask = agentIpStaticMRouteSrcNetMask;
    }
         
    if (inetAddressSet (L7_AF_INET, &srcV4Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
    { 
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return L7_NULLPTR;
    }
        
    if (inetMaskLenToMask(L7_AF_INET, srcV4Mask, &ipStaticMRouteSrcMask) != L7_SUCCESS)
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return L7_NULLPTR;
    }
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = L7_AF_INET;  
  }
  else if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv6)
  {
    L7_in6_addr_t srcV6Addr;
    L7_uint32 srcV6Mask = 0;
    
    memset (&srcV6Addr, L7_NULL, sizeof(L7_in6_addr_t));
    if ((firstTime == L7_TRUE) && (searchType == NEXT))
    {
      firstTime = L7_FALSE;
      memset(&srcV6Addr, L7_NULL, sizeof(L7_in6_addr_t));
      srcV6Mask = 0;
    }
    else if((firstTime != L7_TRUE) || (searchType == EXACT))
    {
      OctetStringToIP6(agentIpStaticMRouteSrcIpAddr, (L7_char8*)&srcV6Addr);
      srcV6Mask = agentIpStaticMRouteSrcNetMask;
    }
     
    if (inetAddressSet (L7_AF_INET6, &srcV6Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return L7_NULLPTR;
    }
   
    if (inetMaskLenToMask(L7_AF_INET6, srcV6Mask, &ipStaticMRouteSrcMask) != L7_SUCCESS)
    {
      firstTime = L7_TRUE;
      allocMem = L7_TRUE;
      return L7_NULLPTR;
    }
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = L7_AF_INET6;
  }
    

  if((searchType == EXACT) ?
    (usmDbMcastStaticMRouteEntryGet(unit,agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask) != L7_SUCCESS) :
    ((usmDbMcastStaticMRouteEntryGet(unit,agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask) != L7_SUCCESS) &&
    (usmDbMcastStaticMRouteEntryNextGet(unit,agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask) != L7_SUCCESS)))
  {
    if(searchType != EXACT)
    {
      if(agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == L7_AF_INET6)
      { 
        firstTime = L7_TRUE;
        allocMem = L7_TRUE;
        return(NULL);
      }
      else if(agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == L7_AF_INET)
      {
        memset(&ipStaticMRouteSrcAddr, L7_NULL, sizeof(L7_inet_addr_t));
        memset(&ipStaticMRouteSrcMask, L7_NULL, sizeof(L7_inet_addr_t));
        agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = L7_AF_INET6;
        if((usmDbMcastStaticMRouteEntryGet(unit,agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask) != L7_SUCCESS) &&
           (usmDbMcastStaticMRouteEntryNextGet(unit,agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask) != L7_SUCCESS))
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
  if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv4)
  {
    L7_uint32 srcV4Addr;
    L7_uchar8 srcV4MaskLen;
    if (inetAddressGet (L7_AF_INET, &ipStaticMRouteSrcAddr, &srcV4Addr) != L7_SUCCESS) 
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr,
                           (L7_uchar8 *)&srcV4Addr, sizeof(L7_uint32)) == L7_TRUE )
    {
      SET_VALID(I_agentIpStaticMRouteSrcIpAddr, agentIpStaticMRouteEntryData.valid);
    }
    if (inetMaskToMaskLen(&ipStaticMRouteSrcMask, &srcV4MaskLen) != L7_SUCCESS) 
    { 
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcNetMask = (L7_uint32)srcV4MaskLen;
    SET_VALID(I_agentIpStaticMRouteSrcNetMask, agentIpStaticMRouteEntryData.valid);
    
  }
  else if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv6)
  {
    L7_in6_addr_t srcV6Addr;
    L7_uchar8 srcV6MaskLen;
    
    if (inetAddressGet (L7_AF_INET6, &ipStaticMRouteSrcAddr, &srcV6Addr) != L7_SUCCESS)
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr,
                           (L7_uchar8 *)&srcV6Addr, sizeof(L7_in6_addr_t)) == L7_TRUE )
    {
      SET_VALID(I_agentIpStaticMRouteSrcIpAddr, agentIpStaticMRouteEntryData.valid);
    }
    
    if (inetMaskToMaskLen(&ipStaticMRouteSrcMask, &srcV6MaskLen) != L7_SUCCESS)
    {
      firstTime =  L7_TRUE;
      allocMem = L7_TRUE;
      return(NULL);
    }
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcNetMask = (L7_uint32)srcV6MaskLen;
    SET_VALID(I_agentIpStaticMRouteSrcNetMask, agentIpStaticMRouteEntryData.valid);
    
  }
  
  SET_VALID (I_agentIpStaticMRouteSrcAddressType, agentIpStaticMRouteEntryData.valid);
  switch (nominator)
  {
    case -1:
    case I_agentIpStaticMRouteSrcAddressType: /* already got it above*/
        break;
    case I_agentIpStaticMRouteSrcIpAddr:
        break;
    case I_agentIpStaticMRouteSrcNetMask:
        break;
    case I_agentIpStaticMRouteRpfIpAddr:
      if (usmDbMcastStaticMRouteRpfAddressGet (unit, agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcMask, &ipStaticMRouteRpfAddr)
                                            == L7_SUCCESS)
      {
        if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv4)
        {
          L7_uint32 rpfV4Addr;
          
          if (inetAddressGet (L7_AF_INET, &ipStaticMRouteRpfAddr, &rpfV4Addr) == L7_SUCCESS)
          {
            if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr,
                                    (L7_uchar8 *)&rpfV4Addr, sizeof(L7_uint32))
                                  == L7_TRUE)
            {
              SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
            }
          }
        }
        else if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv6)
        {
          L7_in6_addr_t rpfV6Addr;
          if (inetAddressGet (L7_AF_INET6, &ipStaticMRouteRpfAddr, &rpfV6Addr) == L7_SUCCESS)
          {
            if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr,
                                     (L7_uchar8 *)&rpfV6Addr, sizeof(L7_in6_addr_t))
                                   == L7_TRUE)
            {
              SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
            }
          }
        }
      }
      break;

    case I_agentIpStaticMRouteIfIndex:
      if (usmDbMcastStaticMRouteInterfaceGet (unit, agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr,
                                              &ipStaticMRouteSrcMask, &agentIpStaticMRouteEntryData.agentIpStaticMRouteIfIndex)
                                            == L7_SUCCESS)
      {
        SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      }
      break;

    case I_agentIpStaticMRoutePreference:
      if (usmDbMcastStaticMRoutePreferenceGet (unit, agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcMask, &agentIpStaticMRouteEntryData.agentIpStaticMRoutePreference)
                                            == L7_SUCCESS)
      {
        SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      }
      break;

    case I_agentIpStaticMRouteStatus:
     	agentIpStaticMRouteEntryData.agentIpStaticMRouteStatus =D_agentIpStaticMRouteStatus_active;
    	SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      break;

    default:
    	 /* unknown nominator */
      return L7_NULLPTR;
  }

  if (nominator >= 0 && !VALID(nominator, agentIpStaticMRouteEntryData.valid))
  {
    return L7_NULLPTR;
  }

  return(&agentIpStaticMRouteEntryData);

}

#ifdef SETS
int
k_agentIpStaticMRouteEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentIpStaticMRouteEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentIpStaticMRouteEntry_set_defaults(doList_t *dp)
{
  agentIpStaticMRouteEntry_t *data = (agentIpStaticMRouteEntry_t *) (dp->data);

  if ((data->agentIpStaticMRouteRpfIpAddr = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentIpStaticMRouteEntry_set(agentIpStaticMRouteEntry_t *data,
                               ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t ipStaticMRouteSrcAddr;
  L7_inet_addr_t ipStaticMRouteSrcMask;
  L7_inet_addr_t ipStaticMRouteRpfAddr;
  L7_uint32 unit = USMDB_UNIT_CURRENT;
  L7_uchar8 snmp_buffer_ipDa[SNMP_BUFFER_LEN];
  L7_BOOL format = L7_FALSE;
  L7_uint i;
  if (usmDbComponentPresentCheck (unit, L7_FLEX_MCAST_MAP_COMPONENT_ID)
                               != L7_TRUE)
  {
    return COMMIT_FAILED_ERROR;
  }
  
  if (data->agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv4)
  {
    L7_uint32 srcV4Addr;
    L7_uint32 rpfV4Addr;
    
    memset (&srcV4Addr, 0, sizeof(L7_uint32));
    srcV4Addr = OctetStringToIP (data->agentIpStaticMRouteSrcIpAddr);
    
    if (inetAddressSet (L7_AF_INET, &srcV4Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (inetMaskLenToMask(L7_AF_INET, data->agentIpStaticMRouteSrcNetMask, &ipStaticMRouteSrcMask) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if ((VALID (I_agentIpStaticMRouteStatus, data->valid)) && (data->agentIpStaticMRouteStatus == D_agentIpStaticMRouteStatus_destroy))
    {
      if (usmDbMcastStaticMRouteDelete (unit, data->agentIpStaticMRouteSrcAddressType,
                                          &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask)
                                            != L7_SUCCESS)
        {
          CLR_VALID (I_agentIpStaticMRouteStatus, data->valid);
          return COMMIT_FAILED_ERROR;
        }
        return NO_ERROR;
    }
    
    memset (&rpfV4Addr, 0, sizeof(L7_uint32));
    bzero(snmp_buffer_ipDa, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer_ipDa, data->agentIpStaticMRouteRpfIpAddr->octet_ptr, 
    	         data->agentIpStaticMRouteRpfIpAddr->length);
    if(VALID (I_agentIpStaticMRouteRpfIpAddr, data->valid))
    {
      for(i=0; i < (data->agentIpStaticMRouteRpfIpAddr->length); i++)
      {
        if(snmp_buffer_ipDa[i] == '.')
        {
          format = L7_TRUE;
          if(usmDbInetAton (snmp_buffer_ipDa, (L7_uint32 *)&rpfV4Addr) != L7_SUCCESS)
          {
            return COMMIT_FAILED_ERROR;
          }
          break;
        }
      }
      if(format != L7_TRUE)
      {
        rpfV4Addr = OctetStringToIP (data->agentIpStaticMRouteRpfIpAddr);
      }
      
      if(inetAddressSet (L7_AF_INET, &rpfV4Addr, &ipStaticMRouteRpfAddr) != L7_SUCCESS)
      {
        return COMMIT_FAILED_ERROR;
      }
      
      if(!(data->agentIpStaticMRouteIfIndex == 0))
      {
        return COMMIT_FAILED_ERROR;
      }
    }
    else
    {
      if(usmDbMcastStaticMRouteRpfAddressGet (unit, data->agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcMask, &ipStaticMRouteRpfAddr) != L7_SUCCESS)
      {
        return COMMIT_FAILED_ERROR;
      }
    }
  }
  else if (data->agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv6)
  {
    L7_in6_addr_t srcV6Addr;
    L7_in6_addr_t rpfV6Addr;
    L7_uint32 intIfNum = 0;
    
    memset (&srcV6Addr, 0, sizeof(L7_in6_addr_t));
    OctetStringToIP6 (data->agentIpStaticMRouteSrcIpAddr, (L7_char8*)&srcV6Addr);
    if (inetAddressSet (L7_AF_INET6, &srcV6Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
        
    if (inetMaskLenToMask(L7_AF_INET6, data->agentIpStaticMRouteSrcNetMask, &ipStaticMRouteSrcMask) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    
    if ((VALID (I_agentIpStaticMRouteStatus, data->valid)) && (data->agentIpStaticMRouteStatus == D_agentIpStaticMRouteStatus_destroy))
    {
      if (usmDbMcastStaticMRouteDelete (unit, data->agentIpStaticMRouteSrcAddressType,
                                          &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask)
                                            != L7_SUCCESS)
        {
          CLR_VALID (I_agentIpStaticMRouteStatus, data->valid);
          return COMMIT_FAILED_ERROR;
        }
        return NO_ERROR;
    }
        
    bzero(snmp_buffer_ipDa, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer_ipDa, data->agentIpStaticMRouteRpfIpAddr->octet_ptr, 
    	             data->agentIpStaticMRouteRpfIpAddr->length);
    if(VALID (I_agentIpStaticMRouteRpfIpAddr, data->valid))
    {
      for(i=0; i < (data->agentIpStaticMRouteRpfIpAddr->length); i++)
      {
        if(snmp_buffer_ipDa[i] == ':')
        {
          format = L7_TRUE;
          if(usmDbParseInetAddrFromStr(snmp_buffer_ipDa,&ipStaticMRouteRpfAddr) != L7_SUCCESS)
          {
            return COMMIT_FAILED_ERROR;
          }
          break;
        }
      }
      memset(&rpfV6Addr, 0, sizeof(L7_in6_addr_t));
      if(format != L7_TRUE)
      {
        OctetStringToIP6 (data->agentIpStaticMRouteRpfIpAddr, (L7_char8*)&rpfV6Addr);
        if(inetAddressSet (L7_AF_INET6, &rpfV6Addr, &ipStaticMRouteRpfAddr) != L7_SUCCESS)
        {
          return COMMIT_FAILED_ERROR;
        }
      }
      if(L7_IP6_IS_ADDR_LINK_LOCAL (&ipStaticMRouteRpfAddr.addr.ipv6))
      { 
        if((usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, data->agentIpStaticMRouteIfIndex) != L7_SUCCESS) ||
              usmDbIntIfNumFromExtIfNum(data->agentIpStaticMRouteIfIndex, &intIfNum) != L7_SUCCESS)
        {
          return COMMIT_FAILED_ERROR;
        }
        else
        {
          data->agentIpStaticMRouteIfIndex = intIfNum;
        }
      }
      else
      {
        data->agentIpStaticMRouteIfIndex = 0;
      }
    }
    else
    {
      if(usmDbMcastStaticMRouteRpfAddressGet(unit, data->agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcMask, &ipStaticMRouteRpfAddr) != L7_SUCCESS)
      {
        return COMMIT_FAILED_ERROR;
      }
    }
  }
  
  if((VALID (I_agentIpStaticMRouteStatus, data->valid)) &&
     ((data->agentIpStaticMRouteStatus == D_agentIpStaticMRouteStatus_createAndGo) || 
      (data->agentIpStaticMRouteStatus == D_agentIpStaticMRouteStatus_createAndWait)))
  {
    if (usmDbMcastStaticMRouteAdd (unit,data->agentIpStaticMRouteSrcAddressType,
                                       &ipStaticMRouteSrcAddr,
                                       &ipStaticMRouteSrcMask, 
                                       &ipStaticMRouteRpfAddr,
                                       data->agentIpStaticMRouteIfIndex,
                                       data->agentIpStaticMRoutePreference)
                                    != L7_SUCCESS)
    {
      CLR_VALID (I_agentIpStaticMRouteStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }

  if(((VALID (I_agentIpStaticMRoutePreference, data->valid)) ||
      (VALID (I_agentIpStaticMRouteIfIndex, data->valid)) ||
      (VALID (I_agentIpStaticMRouteRpfIpAddr, data->valid))) && 
      (data->agentIpStaticMRouteStatus == D_agentIpStaticMRouteStatus_active))
  {
    if(usmDbMcastStaticMRouteAdd(unit,
                                 data->agentIpStaticMRouteSrcAddressType,
                                 &ipStaticMRouteSrcAddr,
                                 &ipStaticMRouteSrcMask,
                                 &ipStaticMRouteRpfAddr,
                                 data->agentIpStaticMRouteIfIndex,
                                 data->agentIpStaticMRoutePreference) != L7_SUCCESS)
   {
     CLR_VALID (I_agentIpStaticMRoutePreference, data->valid);
     CLR_VALID (I_agentIpStaticMRouteIfIndex, data->valid);
     return COMMIT_FAILED_ERROR;
   }
 } 	 
 return NO_ERROR;
}

#ifdef SR_agentIpStaticMRouteEntry_UNDO
/* add #define SR_agentIpStaticMRouteEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentIpStaticMRouteEntry family.
 */
int
agentIpStaticMRouteEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentIpStaticMRouteEntry_UNDO */

#endif /* SETS */

