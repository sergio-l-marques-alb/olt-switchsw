/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfInterface.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfInterfaces-object.xml
*
* @create  29 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfInterface_obj.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_util_api.h"

L7_RC_t usmDbValidOspfIntIfNumGetNext(L7_uint32 unit, L7_uint32 *interface)
{

 L7_uint32 nextInterface, intf, adminMode;
 L7_uint32 objIpAddrValue, objnetMaskValue;
 intf = *interface;
  
 if(intf == 0) 
 {
   if (usmDbValidIntIfNumFirstGet(&intf) != L7_SUCCESS)
   {
     return L7_FAILURE;
   }
 }
 else
 {
    if (usmDbValidIntIfNumNext(intf, &nextInterface) == L7_SUCCESS)
     {
        intf = nextInterface;
     }
     else
     {
        return L7_FAILURE;
     }
 } 

 while(intf)
 {
  if ((usmDbOspfIsValidIntf(unit, intf) == L7_TRUE)  &&
     (usmDbIpIntfExists(unit, intf) == L7_TRUE) &&
     (usmDbOspfAdminModeGet (unit, &adminMode) == L7_SUCCESS) &&
     (adminMode == L7_ENABLE) && (usmDbIpRtrIntfIpAddressGet(unit, intf, &objIpAddrValue, &objnetMaskValue) == L7_SUCCESS))
  {
     *interface = intf;
     return L7_SUCCESS; 
  }
  else
  {
     if (usmDbValidIntIfNumNext(intf, &nextInterface) == L7_SUCCESS)
     {
        intf = nextInterface;
     }
     else
     {
        return L7_FAILURE;
     }   
  }  
 }
     return L7_FAILURE;

}

L7_RC_t usmDbOspfIPAddressIfEntryNext(L7_uint32 unit, L7_uint32 *address , L7_uint32 *interface)
{
   L7_uint32 objIpAddrValue, objnetMaskValue, intf, ipaddr, currIntf;

   ipaddr = *address;
   intf = *interface;

   if((ipaddr == 0) && (intf == 0))
   {
      if(usmDbValidOspfIntIfNumGetNext(unit, &intf) != L7_SUCCESS)
      {
         return L7_FAILURE;
      }  
      else
      {
         if(usmDbIpRtrIntfIpAddressGet(unit, intf, &objIpAddrValue, &objnetMaskValue) == L7_SUCCESS)
         {   
           *address = objIpAddrValue;
           *interface = intf;
           return L7_SUCCESS;
         } 
         else
         {
           return L7_FAILURE;
         } 
      } 
   } 
   else if ((ipaddr != 0) && (intf == 0))
   {
     currIntf = 0;
     while(usmDbValidOspfIntIfNumGetNext(unit, &currIntf) == L7_SUCCESS)
     {
       if(usmDbIpRtrIntfIpAddressGet(unit, currIntf, &objIpAddrValue,&objnetMaskValue) == L7_SUCCESS && (objIpAddrValue == ipaddr))
       {
             *interface = currIntf;
             return L7_SUCCESS;
       }
     }
     return L7_FAILURE;
   }
   else if((ipaddr != 0) && (intf != 0))
   {
     currIntf = intf;
     while(usmDbValidOspfIntIfNumGetNext(unit, &currIntf) == L7_SUCCESS)
     {
       if(usmDbIpRtrIntfIpAddressGet(unit, currIntf, &objIpAddrValue,&objnetMaskValue) == L7_SUCCESS && (objIpAddrValue == ipaddr))
       {
         if(usmDbValidOspfIntIfNumGetNext(unit, &currIntf) == L7_SUCCESS )
         {
           if(usmDbIpRtrIntfIpAddressGet(unit, currIntf, &objIpAddrValue, &objnetMaskValue) == L7_SUCCESS)
           {
             *address = objIpAddrValue;
             *interface = currIntf;
             return L7_SUCCESS;
           }
           else
           {
             return L7_FAILURE;
           }
         }
         else
         {
             return L7_FAILURE;
         } 

       } 
     }
     return L7_FAILURE;
   }   
     return L7_FAILURE;
} 

/*******************************************************************************
* @function fpObjGet_routingospfInterface_IpAddress
*
* @purpose Get 'IpAddress'
 *@description  [IpAddress] The IP address of this OSPF interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_IpAddress (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibIpV4_t nextObjIpAddressValue;
  xLibU32_t nextObjAddressLessValue;
  xLibU32_t objAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjIpAddressValue, 0, sizeof (nextObjIpAddressValue));
    memset (&nextObjAddressLessValue, 0, sizeof (nextObjAddressLessValue));
    owa.l7rc = usmDbOspfIPAddressIfEntryNext(L7_UNIT_CURRENT, &nextObjIpAddressValue, &nextObjAddressLessValue); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);
    nextObjIpAddressValue = objIpAddressValue;
    memset (&nextObjAddressLessValue, 0, sizeof (nextObjAddressLessValue));
    /* retrieve key: AddressLess */
    owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                          (xLibU8_t *) & objAddressLessValue, &owa.len);
    if (owa.rc == XLIBRC_SUCCESS)
    {
      nextObjAddressLessValue = objAddressLessValue;
    }
    do
    {
      owa.l7rc = usmDbOspfIPAddressIfEntryNext(L7_UNIT_CURRENT, &nextObjIpAddressValue, &nextObjAddressLessValue); 

    }
    while ((objIpAddressValue == nextObjIpAddressValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpAddressValue, owa.len);

  /* return the object value: IpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpAddressValue, sizeof (objIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_AddressLess
*
* @purpose Get 'AddressLess'
 *@description  [AddressLess] For the purpose of easing the instancing of
* addressed and addressless interfaces; This variable takes the value 0
* on interfaces with IP Addresses, and the corresponding value of
* ifIndex for interfaces having no IP Address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AddressLess (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibU32_t nextObjIpAddressValue;
  xLibU32_t objAddressLessValue;
  xLibU32_t nextObjAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);

  /* retrieve key: AddressLess */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                          (xLibU8_t *) & objAddressLessValue, &owa.len);

  nextObjIpAddressValue = objIpAddressValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjAddressLessValue, 0, sizeof (nextObjAddressLessValue));
    owa.l7rc = usmDbOspfIPAddressIfEntryNext(L7_UNIT_CURRENT, &nextObjIpAddressValue, &nextObjAddressLessValue); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressLessValue, owa.len);

    nextObjAddressLessValue = objAddressLessValue;

    owa.l7rc = usmDbOspfIPAddressIfEntryNext(L7_UNIT_CURRENT, &nextObjIpAddressValue, &nextObjAddressLessValue); 

  }

  if ((objIpAddressValue != nextObjIpAddressValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAddressLessValue, owa.len);

  /* return the object value: AddressLess */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAddressLessValue,
                           sizeof (objAddressLessValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_AreaId
*
* @purpose Get 'AreaId'
*
* @description [AreaId]: A 32-bit integer uniquely identifying the area to
*              which the interface connects. Area ID 0.0.0.0 is used for
*              the OSPF backbone. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AreaId (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfAreaIdGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objAreaIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AreaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAreaIdValue,
                           sizeof (objAreaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_AreaId
*
* @purpose Set 'AreaId'
*
* @description [AreaId]: A 32-bit integer uniquely identifying the area to
*              which the interface connects. Area ID 0.0.0.0 is used for
*              the OSPF backbone. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_AreaId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AreaId */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfAreaIdSet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, objAreaIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_Type
*
* @purpose Get 'Type'
*
* @description [Type]: The OSPF interface type. By way of a default, this
*              field may be intuited from the corresponding value of ifType.
*              Broad- cast LANs, such as Ethernet and IEEE 802.5, take
*              the value 'broadcast', X.25 and similar technologies take the
*              value 'nbma', and links that are definitively point to point
*              take the value 'pointToPoint'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfTypeGet (keyAddressLessValue, &objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeValue,
                           sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_Type
*
* @purpose Set 'Type'
*
* @description [Type]: The OSPF interface type. By way of a default, this
*              field may be intuited from the corresponding value of ifType.
*              Broad- cast LANs, such as Ethernet and IEEE 802.5, take
*              the value 'broadcast', X.25 and similar technologies take the
*              value 'nbma', and links that are definitively point to point
*              take the value 'pointToPoint'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_Type (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Type */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfTypeSet (keyAddressLessValue, objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_AdminStat
*
* @purpose Get 'AdminStat'
*
* @description [AdminStat]: The OSPF interface's administrative status. The
*              value formed on the interface, and the in- terface will be
*              advertised as an internal route to some area. The value 'disabled'
*              denotes that the interface is external to OSPF. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AdminStat (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfAdminStatGet (L7_UNIT_CURRENT, 0,
                                      keyAddressLessValue, &objAdminStatValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdminStat */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminStatValue,
                           sizeof (objAdminStatValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_AdminStat
*
* @purpose Set 'AdminStat'
*
* @description [AdminStat]: The OSPF interface's administrative status. The
*              value formed on the interface, and the in- terface will be
*              advertised as an internal route to some area. The value 'disabled'
*              denotes that the interface is external to OSPF. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_AdminStat (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminStat */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminStatValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStatValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfAdminStatSet (L7_UNIT_CURRENT, 0,
                                      keyAddressLessValue, objAdminStatValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_RtrPriority
*
* @purpose Get 'RtrPriority'
*
* @description [RtrPriority]: The priority of this interface. Used in multi-access
*              networks, this field is used in the designated router
*              election algorithm. The value 0 signifies that the router
*              is not eligi- ble to become the designated router on this
*              particular network. In the event of a tie in this value,
*              routers will use their Router ID as a tie breaker. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_RtrPriority (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfRtrPriorityGet (L7_UNIT_CURRENT, 0,
                                        keyAddressLessValue,
                                        &objRtrPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RtrPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRtrPriorityValue,
                           sizeof (objRtrPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_RtrPriority
*
* @purpose Set 'RtrPriority'
*
* @description [RtrPriority]: The priority of this interface. Used in multi-access
*              networks, this field is used in the designated router
*              election algorithm. The value 0 signifies that the router
*              is not eligi- ble to become the designated router on this
*              particular network. In the event of a tie in this value,
*              routers will use their Router ID as a tie breaker. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_RtrPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrPriorityValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RtrPriority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRtrPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRtrPriorityValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfRtrPrioritySet (L7_UNIT_CURRENT, 0,
                                        keyAddressLessValue,
                                        objRtrPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_TransitDelay
*
* @purpose Get 'TransitDelay'
*
* @description [TransitDelay]: The estimated number of seconds it takes to
*              transmit a link state update packet over this interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_TransitDelay (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTransitDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfTransitDelayGet (L7_UNIT_CURRENT, 0,
                                         keyAddressLessValue,
                                         &objTransitDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TransitDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTransitDelayValue,
                           sizeof (objTransitDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_TransitDelay
*
* @purpose Set 'TransitDelay'
*
* @description [TransitDelay]: The estimated number of seconds it takes to
*              transmit a link state update packet over this interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_TransitDelay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTransitDelayValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TransitDelay */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTransitDelayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTransitDelayValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfTransitDelaySet (L7_UNIT_CURRENT, 0,
                                         keyAddressLessValue,
                                         objTransitDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_RetransInterval
*
* @purpose Get 'RetransInterval'
*
* @description [RetransInterval]: The number of seconds between link-state
*              ad- vertisement retransmissions, for adjacencies belonging
*              to this interface. This value is also used when retransmitting
*              database descrip- tion and link-state request packets.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_RetransInterval (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRetransIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfRetransIntervalGet (L7_UNIT_CURRENT, 0,
                                            keyAddressLessValue,
                                            &objRetransIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RetransInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRetransIntervalValue,
                           sizeof (objRetransIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_RetransInterval
*
* @purpose Set 'RetransInterval'
*
* @description [RetransInterval]: The number of seconds between link-state
*              ad- vertisement retransmissions, for adjacencies belonging
*              to this interface. This value is also used when retransmitting
*              database descrip- tion and link-state request packets.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_RetransInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRetransIntervalValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RetransInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRetransIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRetransIntervalValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfRetransIntervalSet (L7_UNIT_CURRENT, 0,
                                            keyAddressLessValue,
                                            objRetransIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_HelloInterval
*
* @purpose Get 'HelloInterval'
*
* @description [HelloInterval]: The length of time, in seconds, between the
*              Hello packets that the router sends on the in- terface. This
*              value must be the same for all routers attached to a common
*              network. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_HelloInterval (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHelloIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfHelloIntervalGet (L7_UNIT_CURRENT, 0,
                                          keyAddressLessValue,
                                          &objHelloIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HelloInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHelloIntervalValue,
                           sizeof (objHelloIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_HelloInterval
*
* @purpose Set 'HelloInterval'
*
* @description [HelloInterval]: The length of time, in seconds, between the
*              Hello packets that the router sends on the in- terface. This
*              value must be the same for all routers attached to a common
*              network. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_HelloInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHelloIntervalValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HelloInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objHelloIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHelloIntervalValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfHelloIntervalSet (L7_UNIT_CURRENT, 0,
                                          keyAddressLessValue,
                                          objHelloIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_RtrDeadInterval
*
* @purpose Get 'RtrDeadInterval'
*
* @description [RtrDeadInterval]: The number of seconds that a router's Hello
*              packets have not been seen before it's neigh- bors declare
*              the router down. This should be some multiple of the Hello
*              interval. This value must be the same for all routers attached
*              to a common network. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_RtrDeadInterval (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrDeadIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfRtrDeadIntervalGet (L7_UNIT_CURRENT, 0,
                                            keyAddressLessValue,
                                            &objRtrDeadIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RtrDeadInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRtrDeadIntervalValue,
                           sizeof (objRtrDeadIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_RtrDeadInterval
*
* @purpose Set 'RtrDeadInterval'
*
* @description [RtrDeadInterval]: The number of seconds that a router's Hello
*              packets have not been seen before it's neigh- bors declare
*              the router down. This should be some multiple of the Hello
*              interval. This value must be the same for all routers attached
*              to a common network. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_RtrDeadInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrDeadIntervalValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RtrDeadInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRtrDeadIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRtrDeadIntervalValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfRtrDeadIntervalSet (L7_UNIT_CURRENT, 0,
                                            keyAddressLessValue,
                                            objRtrDeadIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_State
*
* @purpose Get 'State'
*
* @description [State]: The OSPF Interface State. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_State (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfStateGet (L7_UNIT_CURRENT, 0,
                                  keyAddressLessValue, &objStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: State */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStateValue,
                           sizeof (objStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_DesignatedRouter
*
* @purpose Get 'DesignatedRouter'
*
* @description [DesignatedRouter]: The IP Address of the Designated Router.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_DesignatedRouter (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDesignatedRouterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfDesignatedRouterGet (L7_UNIT_CURRENT, 0,
                                             keyAddressLessValue,
                                             &objDesignatedRouterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DesignatedRouter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDesignatedRouterValue,
                           sizeof (objDesignatedRouterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_BackupDesignatedRouter
*
* @purpose Get 'BackupDesignatedRouter'
*
* @description [BackupDesignatedRouter]: The IP Address of the Backup Designated
*              Router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_BackupDesignatedRouter (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBackupDesignatedRouterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIfBackupDesignatedRouterGet (L7_UNIT_CURRENT, 0,
                                          keyAddressLessValue,
                                          &objBackupDesignatedRouterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BackupDesignatedRouter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBackupDesignatedRouterValue,
                           sizeof (objBackupDesignatedRouterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_Events
*
* @purpose Get 'Events'
*
* @description [Events]: The number of times this OSPF interface has changed
*              its state, or an error has occurred. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_Events (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfEventsGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objEventsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Events */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEventsValue,
                           sizeof (objEventsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_AuthKey
*
* @purpose Get 'AuthKey'
*
* @description [AuthKey]: The Authentication Key. If the Area's Author- ization
*              Type is simplePassword, and the key length is shorter
*              than 8 octets, the agent will left adjust and zero fill to
*              8 octets. Note that unauthenticated interfaces need no authentication
*              key, and simple password authen- tication cannot
*              use a key of more than 8 oc- tets. Larger keys are useful
*              only with authen- tication mechanisms not specified in this
*              docu- ment. When read, ospfIfAuthKey always returns an Oc-
*              tet String of length zero. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AuthKey (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfAuthKeyGet(L7_UNIT_CURRENT,
                              keyAddressLessValue, objAuthKeyValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthKeyValue,
                           strlen (objAuthKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_AuthKey
*
* @purpose Set 'AuthKey'
*
* @description [AuthKey]: The Authentication Key. If the Area's Author- ization
*              Type is simplePassword, and the key length is shorter
*              than 8 octets, the agent will left adjust and zero fill to
*              8 octets. Note that unauthenticated interfaces need no authentication
*              key, and simple password authen- tication cannot
*              use a key of more than 8 oc- tets. Larger keys are useful
*              only with authen- tication mechanisms not specified in this
*              docu- ment. When read, ospfIfAuthKey always returns an Oc-
*              tet String of length zero. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_AuthKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAuthKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthKeyValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfAuthKeySet (L7_UNIT_CURRENT, 0, keyAddressLessValue, objAuthKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingospfInterface_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfStatusGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    /*owa.l7rc = usmDbOspfIfStatusSet (L7_UNIT_CURRENT, 0,
                                     keyAddressLessValue, objStatusValue); */
    owa.l7rc = usmDbOspfIfAdminStatSet(L7_UNIT_CURRENT, 0,
                                     keyAddressLessValue, L7_ENABLE);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    /*owa.l7rc = usmDbOspfIfStatusSet (L7_UNIT_CURRENT, 0,
                                     keyAddressLessValue, objStatusValue); */
    owa.l7rc = usmDbOspfIfAdminStatSet(L7_UNIT_CURRENT, 0,
                                     keyAddressLessValue, L7_DISABLE); 
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_MulticastForwarding
*
* @purpose Get 'MulticastForwarding'
*
* @description [MulticastForwarding]: The way multicasts should forwarded
*              on this interface; not forwarded, forwarded as data link multicasts,
*              or forwarded as data link uni- casts. Data link multicasting
*              is not meaning- ful on point to point and NBMA
*              interfaces, and setting ospfMulticastForwarding to 0 effective-
*              ly disables all multicast forwarding. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_MulticastForwarding (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastForwardingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIfMulticastForwardingGet (L7_UNIT_CURRENT, 0,
                                       keyAddressLessValue,
                                       &objMulticastForwardingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastForwarding */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastForwardingValue,
                           sizeof (objMulticastForwardingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_MulticastForwarding
*
* @purpose Set 'MulticastForwarding'
*
* @description [MulticastForwarding]: The way multicasts should forwarded
*              on this interface; not forwarded, forwarded as data link multicasts,
*              or forwarded as data link uni- casts. Data link multicasting
*              is not meaning- ful on point to point and NBMA
*              interfaces, and setting ospfMulticastForwarding to 0 effective-
*              ly disables all multicast forwarding. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_MulticastForwarding (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastForwardingValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastForwarding */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMulticastForwardingValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastForwardingValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application 
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, 0,
                              keyAddressLessValue, objMulticastForwardingValue);*/
  owa.l7rc = L7_FAILURE;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_Demand
*
* @purpose Get 'Demand'
*
* @description [Demand]: Indicates whether Demand OSPF procedures (hel- lo
*              supression to FULL neighbors and setting the DoNotAge flag
*              on proogated LSAs) should be per- formed on this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_Demand (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDemandValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfDemandGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objDemandValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Demand */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDemandValue,
                           sizeof (objDemandValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_Demand
*
* @purpose Set 'Demand'
*
* @description [Demand]: Indicates whether Demand OSPF procedures (hel- lo
*              supression to FULL neighbors and setting the DoNotAge flag
*              on proogated LSAs) should be per- formed on this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_Demand (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDemandValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Demand */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDemandValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDemandValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfDemandSet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, objDemandValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_AuthType
*
* @purpose Get 'AuthType'
*
* @description [AuthType]: The authentication type specified for an in- terface.
*              Additional authentication types may be assigned locally.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AuthType (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfAuthTypeGet (L7_UNIT_CURRENT, 0,
                                     keyAddressLessValue, &objAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthTypeValue,
                           sizeof (objAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_AuthType
*
* @purpose Set 'AuthType'
*
* @description [AuthType]: The authentication type specified for an in- terface.
*              Additional authentication types may be assigned locally.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_AuthType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthTypeValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfAuthTypeSet (L7_UNIT_CURRENT, 0,
                                     keyAddressLessValue, objAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingospfInterface_PollInterval
*
* @purpose Get 'PollInterval'
*
* @description [PollInterval]: The larger time interval, in seconds, between
*              the Hello packets sent to an inactive non-broadcast multi-access
*              neighbor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_PollInterval (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPollIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfPollIntervalGet (L7_UNIT_CURRENT, 0,
                                         keyAddressLessValue,
                                         &objPollIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PollInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPollIntervalValue,
                           sizeof (objPollIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_PollInterval
*
* @purpose Set 'PollInterval'
*
* @description [PollInterval]: The larger time interval, in seconds, between
*              the Hello packets sent to an inactive non-broadcast multi-access
*              neighbor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_PollInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPollIntervalValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PollInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPollIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPollIntervalValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfPollIntervalSet (L7_UNIT_CURRENT, 0,
                                         keyAddressLessValue,
                                         objPollIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_ConfigAdminMode
*
* @purpose Get 'ConfigAdminMode'
*
* @description [ConfigAdminMode]: Gets the Ospf Admin mode configured for
*              the specified interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_ConfigAdminMode (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIntfConfigAdminModeGet (L7_UNIT_CURRENT, keyAddressLessValue,
                                     &objConfigAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ConfigAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfigAdminModeValue,
                           sizeof (objConfigAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_ConfigAreaId
*
* @purpose Get 'ConfigAreaId'
*
* @description [ConfigAreaId]: Gets the Ospf Area Id configured on the specified
*              interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_ConfigAreaId (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigAreaIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfConfigAreaIdGet (L7_UNIT_CURRENT, keyAddressLessValue,
                                           &objConfigAreaIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ConfigAreaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfigAreaIdValue,
                           sizeof (objConfigAreaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_ConfigSecondariesFlag
*
* @purpose Get 'ConfigSecondariesFlag'
*
* @description [ConfigSecondariesFlag]: Gets the Ospf Secondary addresses
*              advertisability setting configured on the specified interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_ConfigSecondariesFlag (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigSecondariesFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIntfConfigSecondariesFlagGet (L7_UNIT_CURRENT, keyAddressLessValue,
                                           &objConfigSecondariesFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ConfigSecondariesFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfigSecondariesFlagValue,
                           sizeof (objConfigSecondariesFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_LocalLinkOpaqueLSACksum
*
* @purpose Get 'LocalLinkOpaqueLSACksum'
*
* @description [LocalLinkOpaqueLSACksum]:  Gets the checksum of Local Link
*              Opaque LSAs 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_LocalLinkOpaqueLSACksum (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLocalLinkOpaqueLSACksumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIntfLocalLinkOpaqueLSACksumGet (L7_UNIT_CURRENT, 
                                             keyAddressLessValue,
                                             &objLocalLinkOpaqueLSACksumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LocalLinkOpaqueLSACksum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLocalLinkOpaqueLSACksumValue,
                           sizeof (objLocalLinkOpaqueLSACksumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_LocalLinkOpaqueLSAs
*
* @purpose Get 'LocalLinkOpaqueLSAs'
*
* @description [LocalLinkOpaqueLSAs]: Gets the total number of Local Link
*              Opaque LSAs 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_LocalLinkOpaqueLSAs (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLocalLinkOpaqueLSAsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIntfLocalLinkOpaqueLSAsGet (L7_UNIT_CURRENT, 
                                         keyAddressLessValue,
                                         &objLocalLinkOpaqueLSAsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LocalLinkOpaqueLSAs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLocalLinkOpaqueLSAsValue,
                           sizeof (objLocalLinkOpaqueLSAsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_LSAAckInterval
*
* @purpose Get 'LSAAckInterval'
*
* @description [LSAAckInterval]: Gets the Ospf LSA Acknowledgement Interval
*              for the specified interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_LSAAckInterval (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLSAAckIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfLSAAckIntervalGet (L7_UNIT_CURRENT, keyAddressLessValue,
                                             &objLSAAckIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LSAAckInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLSAAckIntervalValue,
                           sizeof (objLSAAckIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_SecondariesFlag
*
* @purpose Get 'SecondariesFlag'
*
* @description [SecondariesFlag]: Gets/Set the Ospf Secondary addresses advertisability
*              setting effective on the specified interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_SecondariesFlag (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSecondariesFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfIntfSecondariesFlagGet (L7_UNIT_CURRENT, keyAddressLessValue,
                                     &objSecondariesFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SecondariesFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSecondariesFlagValue,
                           sizeof (objSecondariesFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_SecondariesFlag
*
* @purpose Set 'SecondariesFlag'
*
* @description [SecondariesFlag]: Gets/Set the Ospf Secondary addresses advertisability
*              setting effective on the specified interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_SecondariesFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSecondariesFlagValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SecondariesFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSecondariesFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSecondariesFlagValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfIntfSecondariesFlagSet (L7_UNIT_CURRENT, keyAddressLessValue,
                                     objSecondariesFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingospfInterface_RtrIntfAllVirtIfEvents
*
* @purpose Get 'RtrIntfAllVirtIfEvents'
 *@description  [RtrIntfAllVirtIfEvents] Get the number of state changes or
* error events on this interface for all virtual links   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_RtrIntfAllVirtIfEvents (void *wap, void *bufp)
{

  fpObjWa_t kwaAddressLess = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  xLibS32_t objRtrIntfAllVirtIfEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwaAddressLess.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                                     (xLibU8_t *) & keyAddressLessValue, &kwaAddressLess.len);
  if (kwaAddressLess.rc != XLIBRC_SUCCESS)
  {
    kwaAddressLess.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddressLess);
    return kwaAddressLess.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwaAddressLess.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRtrIntfAllVirtIfEventsGet(L7_UNIT_CURRENT,
                              keyAddressLessValue, &objRtrIntfAllVirtIfEventsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RtrIntfAllVirtIfEvents */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRtrIntfAllVirtIfEventsValue,
                           sizeof (objRtrIntfAllVirtIfEventsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_RtrIntfAllNbrEventsCounter
*
* @purpose Get 'RtrIntfAllNbrEventsCounter'
 *@description  [RtrIntfAllNbrEventsCounter] Get the Ospf Events counter of all
* neighbour on the specified interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_RtrIntfAllNbrEventsCounter (void *wap, void *bufp)
{

  fpObjWa_t kwaAddressLess = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrIntfAllNbrEventsCounterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwaAddressLess.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                                     (xLibU8_t *) & keyAddressLessValue, &kwaAddressLess.len);
  if (kwaAddressLess.rc != XLIBRC_SUCCESS)
  {
    kwaAddressLess.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddressLess);
    return kwaAddressLess.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwaAddressLess.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRtrIntfAllNbrEventsCounterGet(L7_UNIT_CURRENT, 
                              keyAddressLessValue, &objRtrIntfAllNbrEventsCounterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RtrIntfAllNbrEventsCounter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRtrIntfAllNbrEventsCounterValue,
                           sizeof (objRtrIntfAllNbrEventsCounterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_ospfIntfIPAddr
*
* @purpose Get 'ospfIntfIPAddr'
 *@description  [ospfIntfIPAddr] Gets the Ospf IP Address for the specified
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_ospfIntfIPAddr (void *wap, void *bufp)
{
  fpObjWa_t kwaAddressLess = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objospfIntfIPAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwaAddressLess.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                                     (xLibU8_t *) & keyAddressLessValue, &kwaAddressLess.len);
  if (kwaAddressLess.rc != XLIBRC_SUCCESS)
  {
    kwaAddressLess.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddressLess);
    return kwaAddressLess.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwaAddressLess.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfIPAddrGet(L7_UNIT_CURRENT, 
                              keyAddressLessValue, &objospfIntfIPAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ospfIntfIPAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objospfIntfIPAddrValue,
                           sizeof (objospfIntfIPAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_AuthKeyId
*
* @purpose Get 'AuthKeyId'
*
* @description [AuthKeyId]: The identifier for the authentication key used
*              on this interface. This field is only meaningful when the
*              OSPF-MIB ospfIfAuthType is md5(2); otherwise, the value is
*              not used. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AuthKeyId (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthKeyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfAuthKeyIdGet (L7_UNIT_CURRENT, 0,
                                      keyAddressLessValue, &objAuthKeyIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthKeyId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthKeyIdValue,
                           sizeof (objAuthKeyIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_AuthKeyId
*
* @purpose Set 'AuthKeyId'
*
* @description [AuthKeyId]: The identifier for the authentication key used
*              on this interface. This field is only meaningful when the
*              OSPF-MIB ospfIfAuthType is md5(2); otherwise, the value is
*              not used. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_AuthKeyId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthKeyIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthKeyId */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthKeyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthKeyIdValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfAuthKeyIdSet (L7_UNIT_CURRENT, 0,
                                      keyAddressLessValue, objAuthKeyIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_IpMtuIgnoreFlag
*
* @purpose Get 'IpMtuIgnoreFlag'
*
* @description [IpMtuIgnoreFlag]: Configures the IP MTU Ignore Flag value
*              for this OSPF interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_IpMtuIgnoreFlag (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpMtuIgnoreFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfMtuIgnoreGet(L7_UNIT_CURRENT, keyAddressLessValue, &objIpMtuIgnoreFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpMtuIgnoreFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpMtuIgnoreFlagValue,
                           sizeof (objIpMtuIgnoreFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_IpMtuIgnoreFlag
*
* @purpose Set 'IpMtuIgnoreFlag'
*
* @description [IpMtuIgnoreFlag]: Configures the IP MTU Ignore Flag value
*              for this OSPF interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_IpMtuIgnoreFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpMtuIgnoreFlagValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpMtuIgnoreFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objIpMtuIgnoreFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpMtuIgnoreFlagValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIntfMtuIgnoreSet(L7_UNIT_CURRENT, keyAddressLessValue, objIpMtuIgnoreFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfInterface_PassiveMode
*
* @purpose Get 'PassiveMode'
*
* @description [PassiveMode]: Flag to determine whether the interface is passive.
*              A passive interface will not participate in the OSPF
*              adjacency formation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_PassiveMode (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPassiveModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfPassiveModeGet (L7_UNIT_CURRENT, keyAddressLessValue, &objPassiveModeValue);
  if (owa.l7rc == L7_NOT_EXIST)
  {
    /* Passive mode not configured on this interface. Return disabled */
    objPassiveModeValue = L7_DISABLE;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PassiveMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPassiveModeValue,
                           sizeof (objPassiveModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfInterface_PassiveMode
*
* @purpose Set 'PassiveMode'
*
* @description [PassiveMode]: Flag to determine whether the interface is passive.
*              A passive interface will not participate in the OSPF
*              adjacency formation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_PassiveMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPassiveModeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PassiveMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPassiveModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPassiveModeValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIntfPassiveModeSet (L7_UNIT_CURRENT, keyAddressLessValue, objPassiveModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_BdrRtrCount
*
* @purpose Get 'BdrRtrCount'
*
* @description [BdrRtrCount]: The total number of area border routers reach-
*              able within this area configured on this interface. This is
*              initially zero, and is calculated in each SPF Pass. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_BdrRtrCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue, objBdrRtrCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                          (xLibU8_t *) & keyAddressLessValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa.len);

  /* get the area from application */
  owa.l7rc = usmDbOspfIfAreaIdGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objAreaIdValue);

  if(owa.l7rc == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbOspfAreaBorderRtrCountGet (L7_UNIT_CURRENT, objAreaIdValue,
                                               &objBdrRtrCountValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BdrRtrCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBdrRtrCountValue,
                           sizeof (objBdrRtrCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_ospfAsBdrRtrCount
*
* @purpose Get 'ospfAsBdrRtrCount'
*
* @description [ospfAsBdrRtrCount]: The total number of Autonomous System
*              border routers reachable within this area configured on the
*              interface. This is initially zero, and is calculated in each SPF Pass. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_ospfAsBdrRtrCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue, objospfAsBdrRtrCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                          (xLibU8_t *) & keyAddressLessValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa.len);

  /* get the area from application */
  owa.l7rc = usmDbOspfIfAreaIdGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objAreaIdValue);

  if(owa.l7rc == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbOspfAsBdrRtrCountGet (L7_UNIT_CURRENT, objAreaIdValue,
                                          &objospfAsBdrRtrCountValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ospfAsBdrRtrCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objospfAsBdrRtrCountValue,
                           sizeof (objospfAsBdrRtrCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfInterface_LsaCount
*
* @purpose Get 'LsaCount'
*
* @description [LsaCount]: The total number of link-state advertisements in
*              this area's link-state database, excluding AS External LSA's
*              corresponding to the area configured on this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_LsaCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue, objLsaCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                          (xLibU8_t *) & keyAddressLessValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa.len);

  /* get the area from application */
  owa.l7rc = usmDbOspfIfAreaIdGet (L7_UNIT_CURRENT, 0,
                                   keyAddressLessValue, &objAreaIdValue);

  if(owa.l7rc == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbOspfAreaLSACountGet (L7_UNIT_CURRENT, objAreaIdValue,
                                         &objLsaCountValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LsaCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLsaCountValue,
                           sizeof (objLsaCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingospfInterface_AdvertiseSecondaries
*
* @purpose Set 'AdvertiseSecondaries'
*
* @description [AdvertiseSecondaries]: Flag to determine whether the
*              A passive interface will not participate in the OSPF
*              adjacency formation.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterface_AdvertiseSecondaries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdvertiseSecondariesValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);
    /* set the value in application */
  owa.l7rc = usmDbOspfIntfConfigSecondariesFlagGet(L7_UNIT_CURRENT, keyAddressLessValue, &objAdvertiseSecondariesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdvertiseSecondaries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdvertiseSecondariesValue,
                           sizeof (objAdvertiseSecondariesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfInterface_AdvertiseSecondaries
*
* @purpose Set 'AdvertiseSecondaries'
*
* @description [AdvertiseSecondaries]: Flag to determine whether the 
*              A passive interface will not participate in the OSPF
*              adjacency formation.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfInterface_AdvertiseSecondaries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdvertiseSecondariesValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdvertiseSecondaries */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdvertiseSecondariesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdvertiseSecondariesValue, owa.len);

  /* retrieve key: AddressLess */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfInterface_AddressLess,
                           (xLibU8_t *) & keyAddressLessValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessValue, kwa2.len);
    /* set the value in application */
  owa.l7rc = usmDbOspfIntfSecondariesFlagSet (L7_UNIT_CURRENT, keyAddressLessValue, objAdvertiseSecondariesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

