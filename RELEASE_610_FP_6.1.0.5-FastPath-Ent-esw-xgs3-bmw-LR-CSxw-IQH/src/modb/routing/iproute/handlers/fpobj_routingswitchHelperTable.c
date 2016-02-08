/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingswitchHelperTable.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ipconfig-object.xml
*
* @create  6 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingswitchHelperTable_obj.h"
#include "_xe_routingswitchHelperGlobal_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_util_api.h"
#include "l3_commdefs.h"


/*******************************************************************************
* @function fpObjGet_routingswitchHelperGlobal_ipHelperMode
*
* @purpose Get 'ipHelperMode'
 *@description  [ipHelperMode] Administratively enables/disables IP Helper mode
* on the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperGlobal_ipHelperMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipHelperModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperAdminModeGet(&objipHelperModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipHelperModeValue, sizeof (objipHelperModeValue));

  /* return the object value: ipHelperMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipHelperModeValue,
                           sizeof (objipHelperModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchHelperGlobal_ipHelperMode
*
* @purpose Set 'ipHelperMode'
 *@description  [ipHelperMode] Administratively enables/disables IP Helper mode
* on the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchHelperGlobal_ipHelperMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipHelperModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipHelperMode */
  owa.len = sizeof (objipHelperModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipHelperModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipHelperModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpHelperAdminModeSet(objipHelperModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTable_IfIndex
*
* @purpose Get 'IfIndex'
 *@description  [IfIndex] The IfIndex associated with this instance combo-key
* needs to be removed   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_IfIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  xLibU32_t hitCount;
  xLibU16_t udpPort;
  xLibU32_t nextObjhelperIpAddressValue;
  L7_BOOL discard;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  nextObjhelperIpAddressValue = 0;
  udpPort = 0;

  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjIfIndexValue = 0;
    owa.l7rc = usmDbIpHelperAddressFirst(&nextObjIfIndexValue, &udpPort, &nextObjhelperIpAddressValue, &discard, &hitCount);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    nextObjIfIndexValue = objIfIndexValue;
    do
    {
      owa.l7rc = usmDbIpHelperAddressNext(&nextObjIfIndexValue, &udpPort, &nextObjhelperIpAddressValue, &discard, &hitCount);
    }while((owa.l7rc == L7_SUCCESS) && (nextObjIfIndexValue == objIfIndexValue));
  }

  if ((owa.l7rc != L7_SUCCESS) || (nextObjIfIndexValue == IH_INTF_ANY)) 
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue, sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTable_helperIpAddress
*
* @purpose Get 'helperIpAddress'
 *@description  [helperIpAddress] The Helper IP Address assigned to this
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_helperIpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t nextIfIndexValue;

  fpObjWa_t kwahelperIpDestUDPPort= FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpDestUDPPortValue;
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objhelperIpAddressValue;
  xLibIpV4_t nextObjhelperIpAddressValue;
  L7_BOOL discard;
  L7_uint32 hitCount;
  L7_ushort16 udpPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);
  nextIfIndexValue = keyIfIndexValue;

  /* retrieve key: helperIpDestUDPPort */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDestUDPPort,
                                 (xLibU8_t *) &keyhelperIpDestUDPPortValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, kwahelperIpDestUDPPort.len);
  udpPort = keyhelperIpDestUDPPortValue; 
  

  /* retrieve key: helperIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpAddress,
                          (xLibU8_t *) & objhelperIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objhelperIpAddressValue = 0;
    nextObjhelperIpAddressValue = 0;
    discard = L7_TRUE;
     if ((owa.l7rc = usmDbIpHelperAddressGet(nextIfIndexValue, udpPort, nextObjhelperIpAddressValue, discard, &hitCount)) != L7_SUCCESS)
    {
      discard = L7_FALSE;
      owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &udpPort, &nextObjhelperIpAddressValue, &discard, &hitCount);
    }

  }
  else
  {
    nextObjhelperIpAddressValue = objhelperIpAddressValue;

    owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &udpPort, &nextObjhelperIpAddressValue, &discard, &hitCount);
  }
  if ((owa.l7rc != L7_SUCCESS) || (nextIfIndexValue != keyIfIndexValue) || (udpPort != keyhelperIpDestUDPPortValue))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjhelperIpAddressValue, owa.len);

  /* return the object value: helperIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjhelperIpAddressValue,
                           sizeof (objhelperIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTable_helperIpAddress
*
* @purpose Get 'helperIpAddress'
 *@description  [helperIpAddress] The Helper IP Address assigned to this
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_helperIpDestUDPPort(void *wap, void *bufp)
{

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t nextIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t nextObjhelperIpAddressValue;
  xLibU32_t objhelperIpDestUDPPortValue;
  xLibU32_t nextObjhelperIpDestUDPPortValue;
  L7_BOOL discard;
  L7_uint32 hitCount;
  L7_ushort16 udpPort=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);
  nextIfIndexValue = keyIfIndexValue;

  /* retrieve key: helperIpDestUDPPort */
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDestUDPPort,
                          (xLibU8_t *) &objhelperIpDestUDPPortValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    udpPort = 0;
    nextObjhelperIpAddressValue = 0;
    discard = L7_TRUE;
     if ((owa.l7rc = usmDbIpHelperAddressGet(nextIfIndexValue, udpPort, nextObjhelperIpAddressValue, discard, &hitCount)) != L7_SUCCESS)
    {
      discard = L7_FALSE;
      owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &udpPort, &nextObjhelperIpAddressValue,
                                           &discard, &hitCount);
    }
  }
  else
  {
    udpPort = (L7_ushort16)objhelperIpDestUDPPortValue;
    nextObjhelperIpAddressValue = 0;
    do
    {
       owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &udpPort, &nextObjhelperIpAddressValue, 
                                           &discard, &hitCount);
    } while ((udpPort == objhelperIpDestUDPPortValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS) || (nextIfIndexValue != keyIfIndexValue))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjhelperIpDestUDPPortValue = udpPort;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjhelperIpDestUDPPortValue, owa.len);

  /* return the object value: helperIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjhelperIpDestUDPPortValue,
                           sizeof (objhelperIpDestUDPPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTable_helperStatus
*
* @purpose Get 'helperStatus'
*
* @description [helperStatus]: Creates a new entry in the Helper Address
*              table. Allowed values are: createAndGo(4) - Creates an
*              entry in this table, associating the address with a giv 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_helperStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhelperStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: helperIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpAddress,
                           (xLibU8_t *) & keyhelperIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpAddressValue, kwa2.len);

  /* get the value from application */
  objhelperStatusValue = L7_ROW_STATUS_ACTIVE; 

  
  /* return the object value: helperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhelperStatusValue,
                           sizeof (objhelperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTable_helperIpDiscard
*
* @purpose Get 'ipHelperDiscard'
 *@description  [ipHelperDiscard] Discard the packets matching the specified UDP
* port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_helperIpDiscard (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t objIfIndexValue;

  fpObjWa_t kwahelperIpDestUDPPort= FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpDestUDPPortValue;
  xLibU32_t objipHelperDiscardValue;
  xLibU32_t nextObjhelperIpAddressValue=0;
  L7_BOOL discard ;
  L7_uint32 hitCount;
  L7_ushort16 udpPort=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: helperIpDestUDPPort */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDestUDPPort,
                                 (xLibU8_t *) &keyhelperIpDestUDPPortValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, kwahelperIpDestUDPPort.len);

  udpPort = keyhelperIpDestUDPPortValue; 
  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  objIfIndexValue = keyIfIndexValue;
  /* get the value from application */
  discard = L7_TRUE;
  nextObjhelperIpAddressValue = 0;
  owa.l7rc = usmDbIpHelperAddressGet(keyIfIndexValue, udpPort, nextObjhelperIpAddressValue, discard, &hitCount);
  if ((owa.l7rc != L7_SUCCESS))
  {
    discard = L7_FALSE;
  }
  objipHelperDiscardValue = discard;
  FPOBJ_TRACE_VALUE (bufp, &objipHelperDiscardValue, sizeof (objipHelperDiscardValue));

  /* return the object value: ipHelperDiscard */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipHelperDiscardValue,
                           sizeof (objipHelperDiscardValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchHelperTable_helperIpDiscard
*
* @purpose Set 'ipHelperDiscard'
 *@description  [ipHelperDiscard] Discard the packets matching the specified UDP
* port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchHelperTable_helperIpDiscard (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  fpObjWa_t kwahelperIpDestUDPPort= FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpDestUDPPortValue;
  xLibU32_t objipHelperDiscardValue;
  L7_ushort16 udpPort;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: helperIpDestUDPPort */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDestUDPPort,
                                 (xLibU8_t *) &keyhelperIpDestUDPPortValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, kwahelperIpDestUDPPort.len);

  udpPort = keyhelperIpDestUDPPortValue; 
  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  /* retrieve object: ipHelperDiscard */
  owa.len = sizeof (objipHelperDiscardValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipHelperDiscardValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipHelperDiscardValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (objipHelperDiscardValue == L7_TRUE)
  {
     owa.l7rc = usmDbIpHelperDiscardEntryAdd(keyIfIndexValue, keyhelperIpDestUDPPortValue);
  }
  else if (objipHelperDiscardValue == L7_FALSE) 
  {
     owa.l7rc = usmDbIpHelperDiscardEntryDelete(keyIfIndexValue, keyhelperIpDestUDPPortValue);
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTable_helperIpHitCount
*
* @purpose Get 'ipHelperHitCount'
 *@description  [ipHelperHitCount] This gives the number of packets relayed to
* the given helper IP received on the given interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_helperIpHitCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipHelperHitCountValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpDestUDPPortValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipHelperDiscardValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: helperIpDestUDPPort*/
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDestUDPPort,
                           (xLibU8_t *) &keyhelperIpDestUDPPortValue,
                           &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, kwa3.len);

  /* retrieve key: helperIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpAddress,
                           (xLibU8_t *) & keyhelperIpAddressValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpAddressValue, kwa2.len);

  /* retrieve key: helperIpAddress */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDiscard,
                           (xLibU8_t *) & keyipHelperDiscardValue,
                           &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipHelperDiscardValue, kwa2.len);


  /* get the value from application */
  owa.l7rc = usmDbIpHelperAddressGet(keyIfIndexValue,keyhelperIpDestUDPPortValue, keyhelperIpAddressValue, 
                                     keyipHelperDiscardValue, &objipHelperHitCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipHelperHitCountValue, sizeof (objipHelperHitCountValue));

  /* return the object value: ipHelperHitCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipHelperHitCountValue,
                           sizeof (objipHelperHitCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_routingswitchHelperTable_helperStatus
*
* @purpose Set 'helperStatus'
*
* @description [helperStatus]: Creates a new entry in the Helper Address
*              table. Allowed values are: createAndGo(4) - Creates an
*              entry in this table, associating the address with a giv 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchHelperTable_helperStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhelperStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhelperIpDestUDPPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: helperStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objhelperStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objhelperStatusValue, owa.len);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: helperIpDestUDPPort*/
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpDestUDPPort,
                           (xLibU8_t *) &keyhelperIpDestUDPPortValue,
                           &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, kwa3.len);

  /* retrieve key: helperIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_helperIpAddress,
                           (xLibU8_t *) & keyhelperIpAddressValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpAddressValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objhelperStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
   owa.l7rc =
     usmDbIpHelperAddressAdd(keyIfIndexValue, keyhelperIpDestUDPPortValue,
                             keyhelperIpAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objhelperStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    if (keyhelperIpAddressValue)
    {
      owa.l7rc =
        usmDbIpHelperAddressDelete(keyIfIndexValue, keyhelperIpDestUDPPortValue,
                                 keyhelperIpAddressValue);
    }
    else
    {
     owa.l7rc = usmDbIpHelperDiscardEntryDelete(keyIfIndexValue, keyhelperIpDestUDPPortValue);

    }
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
* @function fpObjGet_routingswitchHelperTable_MaxHelperAddressConfigured
*
* @purpose Get 'MaxHelperAddressConfigured'
 *@description  [MaxHelperAddressConfigured] <HTML>To Check whether Max Helper
* Addresses configured or not
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTable_MaxHelperAddressConfigured (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMaxHelperAddressConfiguredValue;

  xLibU32_t keyIfIndexValue;

/*
  xLibS32_t temp_val, mode, i;

  L7_rtrIntfIpAddr_t ipAddrList[L7_IP_HELPER_ENTRIES_MAX];*/

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.len = sizeof (keyIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTable_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, owa.len);

  /* get the value from application */
 /* memset(ipAddrList, 0, sizeof(L7_rtrIntfIpAddr_t) * L7_L3_NUM_HELPER_IP_ADDRESSES);
  if (usmDbIpRtrIntfModeGet(L7_UNIT_CURRENT, keyIfIndexValue, &mode) == L7_SUCCESS &&
      usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyIfIndexValue) == L7_SUCCESS &&
      usmDbVisibleInterfaceCheck(L7_UNIT_CURRENT, keyIfIndexValue, &temp_val) == L7_SUCCESS &&
      usmDbBootpDhcpRelayIntfCfgHelperIpAddrListGet(L7_UNIT_CURRENT, keyIfIndexValue, ipAddrList) == L7_SUCCESS)
  {
    i=0;    
    while (i<L7_L3_NUM_HELPER_IP_ADDRESSES)
    {
      if (ipAddrList[i].ipAddr == 0)
        break;
      else
        i++;
    }
     
    if (i == L7_L3_NUM_HELPER_IP_ADDRESSES)
      objMaxHelperAddressConfiguredValue = XLIB_TRUE;
    else
      objMaxHelperAddressConfiguredValue = XLIB_FALSE;

    owa.l7rc = L7_SUCCESS; 
  }
  else
  {
    objMaxHelperAddressConfiguredValue = XLIB_FALSE;
    owa.l7rc = L7_FAILURE;
  } */

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMaxHelperAddressConfiguredValue,
                     sizeof (objMaxHelperAddressConfiguredValue));

  /* return the object value: MaxHelperAddressConfigured */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxHelperAddressConfiguredValue,
                           sizeof (objMaxHelperAddressConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

