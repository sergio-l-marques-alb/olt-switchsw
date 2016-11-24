/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingarpIntfConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to arp-object.xml
*
* @create  19 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingarpIntfConfig_obj.h"
#include "usmdb_ip_api.h"


/*******************************************************************************
* @function fpObjGet_routingarpIntfConfig_ipAddress
*
* @purpose Get 'ipAddress'
 *@description  [ipAddress] The Type of Address assigned to this interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpIntfConfig_ipAddress (void *wap, void *bufp)
{

  xLibIpV4_t objipAddressValue;
  xLibIpV4_t nextObjipAddressValue;
  xLibU32_t objifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_arpEntry_t pArp;

  /* retrieve key: ipAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                          (xLibU8_t *) & objipAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objipAddressValue, 0, sizeof (objipAddressValue));
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
    owa.l7rc = usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipAddressValue, objifIndexValue, &pArp); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipAddressValue, sizeof(objipAddressValue));
    nextObjipAddressValue = objipAddressValue;
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
    do
    {
      
      owa.l7rc = usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipAddressValue, objifIndexValue, &pArp); 
      objipAddressValue = pArp.ipAddr;
      objifIndexValue = pArp.intIfNum;
    }
    while ((owa.l7rc == L7_SUCCESS) && (pArp.ipAddr == nextObjipAddressValue));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  nextObjipAddressValue = pArp.ipAddr;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipAddressValue, sizeof(nextObjipAddressValue));

  /* return the object value: ipAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipAddressValue, sizeof (objipAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingarpIntfConfig_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] The Associated IfIndex which identified the ARP Entry.
* This value must be specified for static ARP entries on an
* unnumbered interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpIntfConfig_ifIndex (void *wap, void *bufp)
{

  xLibIpV4_t objipAddressValue;
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_arpEntry_t pArp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                          (xLibU8_t *) & objipAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipAddressValue, owa.len);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
  }

  owa.l7rc = usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipAddressValue, objifIndexValue, &pArp);
  if ((owa.l7rc != L7_SUCCESS) || (objipAddressValue != pArp.ipAddr))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjifIndexValue = pArp.intIfNum;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, sizeof(nextObjifIndexValue));

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingarpIntfConfig_age
*
* @purpose Get 'age'
*
* @description [age]: This defines the time (in seconds) since the ARP entry
*              was last refreshed. This value is 0 for ARP entries of type
*              local(1) or static(3), since these entries are not subject
*              to aging. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpIntfConfig_age (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objageValue;
  L7_arpEntry_t pArp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                           (xLibU8_t *) & keyipAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddressValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbIpArpEntryGet (L7_UNIT_CURRENT, keyipAddressValue,
                                 keyifIndexValue, &pArp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objageValue = pArp.ageSecs;
  /* return the object value: age */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objageValue,
                           sizeof (objageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpIntfConfig_macAddress
*
* @purpose Get 'macAddress'
*
* @description [macAddress]: The hardware MAX Address that each interface
*              maps to. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpIntfConfig_macAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objmacAddressValue;
  L7_arpEntry_t pArp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                           (xLibU8_t *) & keyipAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddressValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbIpArpEntryGet (L7_UNIT_CURRENT, keyipAddressValue,
                                 keyifIndexValue, &pArp);

  if (owa.l7rc != L7_SUCCESS || pArp.macAddr.type != 1)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memcpy(objmacAddressValue, pArp.macAddr.addr.enetAddr.addr, pArp.macAddr.len);

  /* return the object value: macAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmacAddressValue,
                           sizeof(objmacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpIntfConfig_macAddress
*
* @purpose Set 'macAddress'
*
* @description [macAddress]: The hardware MAX Address that each interface
*              maps to. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpIntfConfig_macAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objmacAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddressValue;
#if 0
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
#endif
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objmacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objmacAddressValue, owa.len);

  /* retrieve key: ipAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                           (xLibU8_t *) & keyipAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddressValue, kwa1.len);

 /* When Unnumbered Interfaces Support is there then remove this block of code from if 0 */
#if 0
  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len); 
#endif

  /* set the value in application */
  owa.l7rc = usmDbIpStaticArpAdd (L7_UNIT_CURRENT, keyipAddressValue,
                                  L7_INVALID_INTF, objmacAddressValue);
  if (owa.l7rc == L7_NOT_EXIST)
  {
    owa.rc = XLIBRC_ARP_ENTRY_NON_LOCAL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpIntfConfig_type
*
* @purpose Get 'type'
*
* @description [type]: The type of ARP entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpIntfConfig_type (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objtypeValue;
  L7_arpEntry_t pArp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                           (xLibU8_t *) & keyipAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddressValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbIpArpEntryGet (L7_UNIT_CURRENT, keyipAddressValue,
                                 keyifIndexValue, &pArp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(pArp.flags & L7_ARP_LOCAL)
  {
    objtypeValue = L7_ARP_ENTRY_LOCAL;
  }
  else if(pArp.flags & L7_ARP_GATEWAY)
  {
    objtypeValue = L7_ARP_ENTRY_GATEWAY;
  }
  else if(pArp.flags & L7_ARP_STATIC)
  {
    objtypeValue = L7_ARP_ENTRY_STATIC;
  }
  else
  {
    objtypeValue = L7_ARP_ENTRY_DYNAMIC;
  }

  /* return the object value: type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtypeValue,
                           sizeof (objtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpIntfConfig_status
*
* @purpose Get 'status'
*
* @description [status]: The status of this ARP entry. Setting this object
*              destroy will remove the entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpIntfConfig_status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                           (xLibU8_t *) & keyipAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddressValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objstatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objstatusValue,
                           sizeof (objstatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpIntfConfig_status
*
* @purpose Set 'status'
*
* @description [status]: The status of this ARP entry. Setting this object
*              destroy will remove the entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpIntfConfig_status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objstatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objstatusValue, owa.len);

  /* retrieve key: ipAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ipAddress,
                           (xLibU8_t *) & keyipAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddressValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpIntfConfig_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if(objstatusValue == L7_ROW_STATUS_DESTROY)
  {
     /* set the value in application */
     owa.l7rc = usmDbIpMapStaticArpDelete (L7_UNIT_CURRENT, keyipAddressValue,
                                        keyifIndexValue);
  }
  else
  {
     owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
