/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_host_api.c
*
* @purpose   Ospf Host Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for ospf variables, 
*             which are specific to the host table.
*
* @comments  FASTPATH does not support configuration and advertisement
*            of routes to individual hosts. Should you decide to add 
*            support for this feature, you will need to do at least 
*            the following:
*            a) in ospf_config.h, add configuration parameters for 
*               advertisement of host routes
*            b) add APIs to set and get these new configuration parameters,
*               being sure to set the dataChanged flag whenever the 
*               configuration changes.
*            c) call the functions in this file to communicate config
*               changes to the protocol engine.
*
* @create    07/10/2001
*
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_ospfinclude.h"
#include "l7_ospf_api.h"

extern L7_ospfMapCfg_t    *pOspfMapCfgData;
                    
/*
***********************************************************************
*                     API FUNCTIONS  -  HOST CONFIG
***********************************************************************
*/

/*--------------------------------------------------------------------------*/
/* The Host/Metric Table indicates what hosts are directly attached to the  */
/* Router, and what metrics and types of service should be advertised for   */
/* them.                                                                    */
/*--------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get the host entry.
*
* @param    IPAddress   Ip address
* @param    TOS         host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A metric to be advertised, for a given type of service,
*             when a given host is reachable."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostEntryGet ( L7_uint32 IpAddress, L7_int32 TOS )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostEntryGet(IpAddress,TOS);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next host entry.
*
* @param    *IPAddress  Ip address
* @param    *TOS        host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A metric to be advertised, for a given type of service,
*             when a given host is reachable."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostEntryNext ( L7_uint32 *IpAddress, L7_int32 *TOS )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenHostEntryNext(IpAddress,TOS);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Metric to be advertised.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Metric to be Advertised."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostMetricGet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostMetricGet(IpAddress,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the Metric.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Metric to be Advertised."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostMetricSetTest ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostMetricSetTest(IpAddress,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the Metric.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Metric to be Advertised."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostMetricSet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostMetricSet(IpAddress,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostStatusGet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostStatusGet(IpAddress,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostStatusSetTest ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostStatusSetTest(IpAddress,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostStatusSet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostStatusSet(IpAddress,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Area the Host Entry is to be found within.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area the Host Entry is to be found within.  By
*             default, the area that a subsuming OSPF interface is
*             in, or 0.0.0.0."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostAreaIDGet ( L7_uint32 IpAddress, L7_int32 TOS, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenHostAreaIDGet(IpAddress,TOS,val);

  return L7_FAILURE;
}

