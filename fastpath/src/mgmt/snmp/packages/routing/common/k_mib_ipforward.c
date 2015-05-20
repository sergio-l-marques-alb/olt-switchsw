#include <k_private_base.h>
#include <k_mib_ipforward_api.h>
#include <inst_lib.h>
#include "usmdb_common.h"
#include "usmdb_mib_ipfwd_api.h"
#include "usmdb_util_api.h"

ipForward_t *
k_ipForward_get(int serialNum, ContextInfo *contextInfo,
                int nominator)
{

   static ipForward_t ipForwardData;

   ZERO_VALID(ipForwardData.valid);

   switch (nominator)
   {
   case -1:
     break;

   case I_inetCidrRouteNumber :
     if (usmDbIpCidrRouteNumberGet(USMDB_UNIT_CURRENT, 
                             &ipForwardData.inetCidrRouteNumber) == L7_SUCCESS)
       SET_VALID(I_inetCidrRouteNumber, ipForwardData.valid);
     break;
   
    /*	Route Discards not implemented now */
   /*
	 ipForwardData.inetCidrRouteDiscards = ;
   */

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, ipForwardData.valid))
     return(NULL);

   return(&ipForwardData);
}

ipForwardEntry_t *
k_ipForwardEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_UINT32 ipForwardDest,
                     SR_INT32 ipForwardProto,
                     SR_INT32 ipForwardPolicy,
                     SR_UINT32 ipForwardNextHop)
{
#ifdef NOT_YET
   static ipForwardEntry_t ipForwardEntryData;

   /*
    * put your code to retrieve the information here
    */

   ipForwardEntryData.ipForwardDest = ;
   ipForwardEntryData.ipForwardMask = ;
   ipForwardEntryData.ipForwardPolicy = ;
   ipForwardEntryData.ipForwardNextHop = ;
   ipForwardEntryData.ipForwardIfIndex = ;
   ipForwardEntryData.ipForwardType = ;
   ipForwardEntryData.ipForwardProto = ;
   ipForwardEntryData.ipForwardAge = ;
   ipForwardEntryData.ipForwardInfo = ;
   ipForwardEntryData.ipForwardNextHopAS = ;
   ipForwardEntryData.ipForwardMetric1 = ;
   ipForwardEntryData.ipForwardMetric2 = ;
   ipForwardEntryData.ipForwardMetric3 = ;
   ipForwardEntryData.ipForwardMetric4 = ;
   ipForwardEntryData.ipForwardMetric5 = ;
   SET_ALL_VALID(ipForwardEntryData.valid);
   return(&ipForwardEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_ipForwardEntry_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ipForwardEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipForwardEntry_set_defaults(doList_t *dp)
{
    ipForwardEntry_t *data = (ipForwardEntry_t *) (dp->data);

    data->ipForwardIfIndex = 0;
    data->ipForwardType = D_ipForwardType_invalid;
    data->ipForwardAge = 0;
    if ((data->ipForwardInfo = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->ipForwardNextHopAS = 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_ipForwardEntry_set(ipForwardEntry_t *data,
                     ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_ipForwardEntry_UNDO
/* add #define SR_ipForwardEntry_UNDO in sitedefs.h to
 * include the undo routine for the ipForwardEntry family.
 */
int
ipForwardEntry_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipForwardEntry_UNDO */

#endif /* SETS */

ipCidrRouteEntry_t *
k_ipCidrRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_UINT32 ipCidrRouteDest,
                       SR_UINT32 ipCidrRouteMask,
                       SR_INT32 ipCidrRouteTos,
                       SR_UINT32 ipCidrRouteNextHop)
{
#ifdef NOT_YET
   static ipCidrRouteEntry_t ipCidrRouteEntryData;

   /*
    * put your code to retrieve the information here
    */

   ipCidrRouteEntryData.ipCidrRouteDest = ;
   ipCidrRouteEntryData.ipCidrRouteMask = ;
   ipCidrRouteEntryData.ipCidrRouteTos = ;
   ipCidrRouteEntryData.ipCidrRouteNextHop = ;
   ipCidrRouteEntryData.ipCidrRouteIfIndex = ;
   ipCidrRouteEntryData.ipCidrRouteType = ;
   ipCidrRouteEntryData.ipCidrRouteProto = ;
   ipCidrRouteEntryData.ipCidrRouteAge = ;
   ipCidrRouteEntryData.ipCidrRouteInfo = ;
   ipCidrRouteEntryData.ipCidrRouteNextHopAS = ;
   ipCidrRouteEntryData.ipCidrRouteMetric1 = ;
   ipCidrRouteEntryData.ipCidrRouteMetric2 = ;
   ipCidrRouteEntryData.ipCidrRouteMetric3 = ;
   ipCidrRouteEntryData.ipCidrRouteMetric4 = ;
   ipCidrRouteEntryData.ipCidrRouteMetric5 = ;
   ipCidrRouteEntryData.ipCidrRouteStatus = ;
   SET_ALL_VALID(ipCidrRouteEntryData.valid);
   return(&ipCidrRouteEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_ipCidrRouteEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ipCidrRouteEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipCidrRouteEntry_set_defaults(doList_t *dp)
{
    ipCidrRouteEntry_t *data = (ipCidrRouteEntry_t *) (dp->data);

    data->ipCidrRouteIfIndex = 0;
    data->ipCidrRouteAge = 0;
    if ((data->ipCidrRouteInfo = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->ipCidrRouteNextHopAS = 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_ipCidrRouteEntry_set(ipCidrRouteEntry_t *data,
                       ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_ipCidrRouteEntry_UNDO
/* add #define SR_ipCidrRouteEntry_UNDO in sitedefs.h to
 * include the undo routine for the ipCidrRouteEntry family.
 */
int
ipCidrRouteEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipCidrRouteEntry_UNDO */

#endif /* SETS */


inetCidrRouteEntry_t *
k_inetCidrRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 inetCidrRouteDestType,
                         OctetString * inetCidrRouteDest,
                         SR_UINT32 inetCidrRoutePfxLen,
                         OID * inetCidrRoutePolicy,
                         SR_INT32 inetCidrRouteNextHopType,
                         OctetString * inetCidrRouteNextHop)
{
  static inetCidrRouteEntry_t inetCidrRouteEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum = 0;
  L7_uint32 routeDest = 0;
  L7_uint32 routeNextHop = 0;

  ZERO_VALID(inetCidrRouteEntryData.valid);
  bzero(snmp_buffer, sizeof(snmp_buffer));

  /* Populating the indices one by one*/
  /* Populating RouteDestType */
  if( (inetCidrRouteDestType <= D_inetCidrRouteDestType_ipv4) && searchType == NEXT )
  {
    inetCidrRouteDestType = D_inetCidrRouteDestType_ipv4;
  }
  else if(inetCidrRouteDestType > D_inetCidrRouteDestType_ipv4) 
  { 
    /* Only IP v4 is supported now */
    ZERO_VALID(inetCidrRouteEntryData.valid);
    return(NULL);
  }
  inetCidrRouteEntryData.inetCidrRouteDestType= inetCidrRouteDestType;
  SET_VALID(I_inetCidrRouteDestType, inetCidrRouteEntryData.valid);
  /* Populating RouteDest */
  /* Converting the Route Dest in Octet String to IP Address */
  if ( inetCidrRouteDest->length != 0 )
      routeDest = OctetStringToIP(inetCidrRouteDest);
  SET_VALID(I_inetCidrRouteDest, inetCidrRouteEntryData.valid);

  /* Populating route Policy */
  FreeOID(inetCidrRouteEntryData.inetCidrRoutePolicy);
  inetCidrRouteEntryData.inetCidrRoutePolicy = MakeOIDFromDot("0.0");
  SET_VALID(I_inetCidrRoutePolicy, inetCidrRouteEntryData.valid);

  /* Populating route Next Hop Type */
  if( (inetCidrRouteNextHopType <= D_inetCidrRouteNextHopType_ipv4) && searchType == NEXT)
  {
    inetCidrRouteNextHopType = D_inetCidrRouteNextHopType_ipv4;
    
  } 
  else if( inetCidrRouteNextHopType > D_inetCidrRouteNextHopType_ipv4) 
  { 
    /* Only IP v4 is supported now */
    ZERO_VALID(inetCidrRouteEntryData.valid);
    return(NULL);
  }

  inetCidrRouteEntryData.inetCidrRouteNextHopType = inetCidrRouteNextHopType;
  SET_VALID(I_inetCidrRouteNextHopType, inetCidrRouteEntryData.valid);

  /* Converting the Route Next Hop in Octet String to IP Address */
  if ( inetCidrRouteNextHop->length != 0 )
      routeNextHop = OctetStringToIP(inetCidrRouteNextHop);
  SET_VALID(I_inetCidrRouteNextHop, inetCidrRouteEntryData.valid);

  /* validate the input ENTRY data */
  if ((searchType == EXACT) ?
          (snmpInetCidrRouteEntryValidate(routeDest, inetCidrRoutePfxLen, 
                                          routeNextHop) != L7_SUCCESS) :
          ((snmpInetCidrRouteEntryValidate(routeDest, inetCidrRoutePfxLen,
                                           routeNextHop) != L7_SUCCESS) &&
           /* calling GET-NEXT function for WALK / Table-view / getNext operations */
           (snmpInetCidrRouteEntryGetNext(&routeDest, &inetCidrRoutePfxLen,
                                          &routeNextHop) != L7_SUCCESS)))
  {
      ZERO_VALID(inetCidrRouteEntryData.valid);
      return(NULL);
  }

  /* copy the new Destination Route Address to the outgoing data structure by 
   * converting the IP Address to Octet String */
  FreeOctetString(inetCidrRouteEntryData.inetCidrRouteDest);
  inetCidrRouteEntryData.inetCidrRouteDest = IPToOctetString(routeDest);
  SET_VALID(I_inetCidrRouteDest, inetCidrRouteEntryData.valid);

  /* copy the new Route Prefix Length */
  inetCidrRouteEntryData.inetCidrRoutePfxLen = inetCidrRoutePfxLen;
  SET_VALID(I_inetCidrRoutePfxLen, inetCidrRouteEntryData.valid);

  /* copy the new Next Hop to the outgoing data structure by 
   * converting the IP Address to Octet String */
  FreeOctetString(inetCidrRouteEntryData.inetCidrRouteNextHop);
  inetCidrRouteEntryData.inetCidrRouteNextHop = IPToOctetString(routeNextHop);
  SET_VALID(I_inetCidrRouteNextHop, inetCidrRouteEntryData.valid);

    switch (nominator)
    {
    case -1:
    case I_inetCidrRouteDestType :
    case I_inetCidrRouteDest :
    case I_inetCidrRoutePfxLen :
    case I_inetCidrRoutePolicy :
    case I_inetCidrRouteNextHopType :
    case I_inetCidrRouteNextHop :
      break; 

    case I_inetCidrRouteIfIndex :
       if ((snmpInetCidrRouteIfIndexGet(routeDest,
                                       inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                       routeNextHop,
                                       &intIfNum) == L7_SUCCESS) &&
           (usmDbExtIfNumFromIntIfNum(intIfNum, 
                          &inetCidrRouteEntryData.inetCidrRouteIfIndex) == L7_SUCCESS))
         SET_VALID(I_inetCidrRouteIfIndex, inetCidrRouteEntryData.valid);
       break;
       
    case I_inetCidrRouteType :
       if (snmpInetCidrRouteTypeGet(routeDest,
                                       inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                       routeNextHop,
                                       &inetCidrRouteEntryData.inetCidrRouteType) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteType, inetCidrRouteEntryData.valid);
       break;

    case I_inetCidrRouteProto :
       if (snmpInetCidrRouteProtoGet(routeDest,
                                       inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                       routeNextHop,
                                       &inetCidrRouteEntryData.inetCidrRouteProto) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteProto, inetCidrRouteEntryData.valid);
       break;

     case I_inetCidrRouteAge :
       if (snmpInetCidrRouteAgeGet(routeDest,
                                       inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                       routeNextHop,
                                       &inetCidrRouteEntryData.inetCidrRouteAge) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteAge, inetCidrRouteEntryData.valid);
       break;
    
    case I_inetCidrRouteNextHopAS :
       

       /* Presently as Next Hop AS implementation is not in the RTO, so just returning default 
          0 */
       /*
       if (snmpInetCidrRouteNextHopASGet(snmp_buffer_RouteDest,
                                       inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                       snmp_buffer_RouteNextHop,
                                       &inetCidrRouteEntryData.inetCidrRouteNextHopAS) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteNextHopAS, inetCidrRouteEntryData.valid);
       */
       inetCidrRouteEntryData.inetCidrRouteNextHopAS = 0;
       SET_VALID(I_inetCidrRouteNextHopAS, inetCidrRouteEntryData.valid);
       break;
     
     case I_inetCidrRouteMetric1 :
       if (snmpInetCidrRouteMetric1Get(routeDest,
                                 inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                 routeNextHop,
                                 &inetCidrRouteEntryData.inetCidrRouteMetric1) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteMetric1, inetCidrRouteEntryData.valid);
       break;
     
     case I_inetCidrRouteMetric2 :
       if (snmpInetCidrRouteMetric2Get(routeDest,
                                 inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                 routeNextHop,
                                 &inetCidrRouteEntryData.inetCidrRouteMetric2) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteMetric2, inetCidrRouteEntryData.valid);
       break;

     case I_inetCidrRouteMetric3 :
       if (snmpInetCidrRouteMetric3Get(routeDest,
                                 inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                 routeNextHop,
                                 &inetCidrRouteEntryData.inetCidrRouteMetric3) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteMetric3, inetCidrRouteEntryData.valid);
       break;

     case I_inetCidrRouteMetric4 :
       if (snmpInetCidrRouteMetric4Get(routeDest,
                                 inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                 routeNextHop,
                                 &inetCidrRouteEntryData.inetCidrRouteMetric4) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteMetric4, inetCidrRouteEntryData.valid);
       break;

     case I_inetCidrRouteMetric5 :
       if (snmpInetCidrRouteMetric5Get(routeDest,
                                 inetCidrRouteEntryData.inetCidrRoutePfxLen, 
                                 routeNextHop,
                                 &inetCidrRouteEntryData.inetCidrRouteMetric5) == L7_SUCCESS)
         SET_VALID(I_inetCidrRouteMetric5, inetCidrRouteEntryData.valid);
       break;

     case I_inetCidrRouteStatus :
         inetCidrRouteEntryData.inetCidrRouteStatus = D_inetCidrRouteStatus_active;
         SET_VALID(I_inetCidrRouteStatus, inetCidrRouteEntryData.valid);
       break;

     default:
       /* unknown nominator */
       return(NULL);
       break;
  }

  if (nominator >= 0 && !VALID(nominator, inetCidrRouteEntryData.valid))
  {
      return(NULL);
  }
    
  return(&inetCidrRouteEntryData);

}

#ifdef SETS
int
k_inetCidrRouteEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_inetCidrRouteEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_inetCidrRouteEntry_set_defaults(doList_t *dp)
{
    inetCidrRouteEntry_t *data = (inetCidrRouteEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_inetCidrRouteEntry_set(inetCidrRouteEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_inetCidrRouteEntry_UNDO
/* add #define SR_inetCidrRouteEntry_UNDO in sitedefs.h to
 * include the undo routine for the inetCidrRouteEntry family.
 */
int
inetCidrRouteEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_inetCidrRouteEntry_UNDO */

#endif /* SETS */
