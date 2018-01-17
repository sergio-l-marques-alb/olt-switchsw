/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingarpEntryPurge.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to arpcache-object.xml
*
* @create  27 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingarpEntryPurge_obj.h"
#include "usmdb_ip_api.h"


/*******************************************************************************
* @function fpObjGet_routingarpEntryPurge_ipaddress
*
* @purpose Get 'ipaddress'
 *@description  [ipaddress] IP address of the ARP entry to start the search (for
* deleting the entry)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpEntryPurge_ipaddress (void *wap, void *bufp)
{

  xLibIpV4_t objipaddressValue;
  xLibIpV4_t nextObjipaddressValue;
  xLibU32_t objinterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  L7_arpEntry_t pArp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_ipaddress,
                          (xLibU8_t *) & objipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objipaddressValue, 0, sizeof (objipaddressValue));
    memset (&objinterfaceValue, 0, sizeof (objinterfaceValue));
    owa.l7rc = usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipaddressValue, objinterfaceValue, &pArp);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipaddressValue, owa.len);
    memset (&objinterfaceValue, 0, sizeof (objinterfaceValue));
    do
    {
      owa.l7rc = usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipaddressValue, objinterfaceValue, &pArp);
      objinterfaceValue = pArp.intIfNum;
    }
    while ((objipaddressValue == pArp.ipAddr) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjipaddressValue = pArp.ipAddr;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipaddressValue, owa.len);

  /* return the object value: ipaddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipaddressValue, sizeof (objipaddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingarpEntryPurge_interface
*
* @purpose Get 'interface'
 *@description  [interface] Internal interface number of the ARP entry for
* deleting.L7_INVALID_INTF if interface not configured   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpEntryPurge_interface (void *wap, void *bufp)
{

  xLibIpV4_t objipaddressValue;
  xLibU32_t objinterfaceValue;
  xLibU32_t nextObjinterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_arpEntry_t pArp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_ipaddress,
                          (xLibU8_t *) & objipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipaddressValue, owa.len);

  /* retrieve key: interface */
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_interface,
                          (xLibU8_t *) & objinterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objinterfaceValue, 0, sizeof (objinterfaceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinterfaceValue, owa.len);

  }

  owa.l7rc = usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipaddressValue, objinterfaceValue, &pArp);
   
  if ((objipaddressValue != pArp.ipAddr) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjinterfaceValue = pArp.intIfNum;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinterfaceValue, owa.len);

  /* return the object value: interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinterfaceValue, sizeof (objinterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#if 0
/*******************************************************************************
* @function fpObjGet_routingarpEntryPurge_ipaddress_interface
*
* @purpose Get 'ipaddress + interface +'
*
* @description [ipaddress]: IP address of the ARP entry to start the search
*              (for deleting the entry) 
*              [interface]: Internal interface number of the ARP entry for
*              deleting.L7_INVALID_INTF if interface not configured 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpEntryPurge_ipaddress_interface (void *wap,
                                                            void *bufp[],
                                                            xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaipaddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipaddressValue, nextObjipaddressValue;
  fpObjWa_t owainterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinterfaceValue, nextObjinterfaceValue;
  L7_arpEntry_t pArp;
  void *outipaddress = (void *) bufp[--keyCount];
  void *outinterface = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outipaddress);
  FPOBJ_TRACE_ENTER (outinterface);

  /* retrieve key: ipaddress */
  owaipaddress.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_ipaddress,
                                   (xLibU8_t *) & objipaddressValue,
                                   &owaipaddress.len);
  if (owaipaddress.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: interface */
    owainterface.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_interface,
                                     (xLibU8_t *) & objinterfaceValue,
                                     &owainterface.len);
  }
  else
  {
    objipaddressValue = 0;
    objinterfaceValue = 0;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (outipaddress, &objipaddressValue, owaipaddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outinterface, &objinterfaceValue, owainterface.len);

  owa.rc =
    usmDbIpArpEntryNext(L7_UNIT_CURRENT, objipaddressValue, objinterfaceValue, &pArp);

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outipaddress, owaipaddress);
    FPOBJ_TRACE_EXIT (outinterface, owainterface);
    return owa.rc;
  }

  nextObjipaddressValue = pArp.ipAddr;
  nextObjinterfaceValue = pArp.intIfNum;

  FPOBJ_TRACE_CURRENT_KEY (outipaddress, &nextObjipaddressValue,
                           owaipaddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outinterface, &nextObjinterfaceValue,
                           owainterface.len);

  /* return the object value: ipaddress */
  xLibBufDataSet (outipaddress,
                  (xLibU8_t *) & nextObjipaddressValue,
                  sizeof (nextObjipaddressValue));

  /* return the object value: interface */
  xLibBufDataSet (outinterface,
                  (xLibU8_t *) & nextObjinterfaceValue,
                  sizeof (nextObjinterfaceValue));
  FPOBJ_TRACE_EXIT (outipaddress, owaipaddress);
  FPOBJ_TRACE_EXIT (outinterface, owainterface);
  return XLIBRC_SUCCESS;
}

#endif
/*******************************************************************************
* @function fpObjGet_routingarpEntryPurge_rowStatus
*
* @purpose Get 'rowStatus'
*
* @description [rowStatus]: The row status variable, used according to installation
*              and removal conventions for conceptual rows.Destroying
*              the entry removes the associated entry from ARP table.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpEntryPurge_rowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipaddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objrowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_ipaddress,
                           (xLibU8_t *) & keyipaddressValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipaddressValue, kwa1.len);

  /* retrieve key: interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_interface,
                           (xLibU8_t *) & keyinterfaceValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwa2.len);

  /* get the value from application 
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipaddressValue,
                              keyinterfaceValue, &objrowStatusValue); */
  objrowStatusValue = L7_ROW_STATUS_ACTIVE;

  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: rowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objrowStatusValue,
                           sizeof (objrowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpEntryPurge_rowStatus
*
* @purpose Set 'rowStatus'
*
* @description [rowStatus]: The row status variable, used according to installation
*              and removal conventions for conceptual rows.Destroying
*              the entry removes the associated entry from ARP table.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpEntryPurge_rowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objrowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipaddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: rowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objrowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objrowStatusValue, owa.len);

  /* retrieve key: ipaddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_ipaddress,
                           (xLibU8_t *) & keyipaddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipaddressValue, kwa1.len);

  /* retrieve key: interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingarpEntryPurge_interface,
                           (xLibU8_t *) & keyinterfaceValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objrowStatusValue == L7_ROW_STATUS_ACTIVE)
  {
    /* Create a row */
    owa.l7rc = L7_SUCCESS;
    /*owa.l7rc = usmDbIpArpEntryPurge (L7_UNIT_CURRENT, keyipaddressValue,
                                     keyinterfaceValue, objrowStatusValue); */
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objrowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbIpArpEntryPurge (L7_UNIT_CURRENT, keyipaddressValue,
                                     keyinterfaceValue);
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
