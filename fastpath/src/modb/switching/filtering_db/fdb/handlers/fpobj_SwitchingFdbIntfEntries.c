
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingFdbIntfEntries.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  16 October 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingFdbIntfEntries_obj.h"

#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbInterface
*
* @purpose Get 'FdbInterface'
 *@description  [FdbInterface] <HTML>The Key Interface Number for the MAC
* Address Entries   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbInterfaceValue;
  xLibU32_t nextObjFdbInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbInterface */
  owa.len = sizeof (objFdbInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbInterface,
                          (xLibU8_t *) & objFdbInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjFdbInterfaceValue = 0; 
    owa.l7rc = usmDbGetNextVisibleIntIfNumber (nextObjFdbInterfaceValue, &nextObjFdbInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objFdbInterfaceValue, owa.len);
    owa.l7rc = usmDbGetNextVisibleIntIfNumber (objFdbInterfaceValue,
                                    &nextObjFdbInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjFdbInterfaceValue, owa.len);

  /* return the object value: FdbInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjFdbInterfaceValue,
                           sizeof (nextObjFdbInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbMac
*
* @purpose Get 'FdbMac'
 *@description  [FdbMac] <HTML>The VLAN ID and Mac Address Combination of a
* given FDB Entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbMac (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objFdbMacValue;
  xLibStr256_t nextObjFdbMacValue;

  xLibU32_t keyFdbInterfaceValue;

  usmdbFdbEntry_t entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbInterface */
  owa.len = sizeof (keyFdbInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbInterface,
                          (xLibU8_t *) & keyFdbInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbInterfaceValue, owa.len);

  memset(objFdbMacValue,0x00,sizeof(objFdbMacValue));
  memset(nextObjFdbMacValue,0x00,sizeof(nextObjFdbMacValue));

  /* retrieve key: FdbMac */
  owa.len = sizeof (objFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbMac,
                          (xLibU8_t *) objFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&entry, 0, sizeof(entry));	
    owa.l7rc = usmDbFDBIntfEntryNextGet (keyFdbInterfaceValue, &entry);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objFdbMacValue, owa.len);
    memcpy(entry.usmdbFdbMac, objFdbMacValue, L7_FDB_KEY_SIZE);
    memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
    memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));
    owa.l7rc = usmDbFDBIntfEntryNextGet (keyFdbInterfaceValue, &entry);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   memcpy(nextObjFdbMacValue, entry.usmdbFdbMac, L7_FDB_KEY_SIZE);
   FPOBJ_TRACE_NEW_KEY (bufp, nextObjFdbMacValue, owa.len);
 
  /* return the object value: FdbMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjFdbMacValue, L7_FDB_KEY_SIZE);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbIntfVlanId
*
* @purpose Get 'FdbIntfVlanId'
 *@description  [FdbIntfVlanId] <HTML>VLAN ID of given FDB Entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbIntfVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbIntfVlanIdValue;

  xLibStr256_t keyFdbMacValue;
  xLibStr6_t macValueDummy;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbMac */
  owa.len = sizeof (keyFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbMac,
                          (xLibU8_t *) keyFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFdbMacValue, owa.len);
  
  memset(macValueDummy,0x00,sizeof(macValueDummy));

  /* get the value from application */
  owa.l7rc = usmDbEntryVidMacSeparate (keyFdbMacValue, &objFdbIntfVlanIdValue,macValueDummy);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbIntfVlanIdValue, sizeof (objFdbIntfVlanIdValue));

  /* return the object value: FdbIntfVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbIntfVlanIdValue,
                           sizeof (objFdbIntfVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbIntfMac
*
* @purpose Get 'FdbIntfMac'
 *@description  [FdbIntfMac] <HTML>MAC Address learned on the specific Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbIntfMac (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr6_t objFdbIntfMacValue;
 
  xLibU32_t vlanIdValueDummy;
  xLibStr256_t keyFdbMacValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbMac */
  owa.len = sizeof (keyFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbMac,
                          (xLibU8_t *) keyFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFdbMacValue, owa.len);

  memset(objFdbIntfMacValue,0x00,sizeof(objFdbIntfMacValue));
  /* get the value from application */
  owa.l7rc = usmDbEntryVidMacSeparate (keyFdbMacValue, &vlanIdValueDummy,objFdbIntfMacValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objFdbIntfMacValue, sizeof (objFdbIntfMacValue));

  /* return the object value: FdbIntfMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFdbIntfMacValue, sizeof (objFdbIntfMacValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbIntfEntryType
*
* @purpose Get 'FdbbIntfEntryType'
 *@description  [FdbbIntfEntryType] <HTML>Entry Type of the specified Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbIntfEntryType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbbIntfEntryTypeValue;

  xLibStr256_t keyFdbMacValue;
  usmdbFdbEntry_t entry;

  FPOBJ_TRACE_ENTER (bufp);

  memset(keyFdbMacValue,0x00,sizeof(keyFdbMacValue));
  /* retrieve key: FdbMac */
  owa.len = sizeof (keyFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbMac,
                          (xLibU8_t *) keyFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFdbMacValue, owa.len);

  memcpy(entry.usmdbFdbMac, keyFdbMacValue,L7_FDB_KEY_SIZE); 
  memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
  memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));
 
  /* get the value from application */
  owa.l7rc = usmDbFDBEntryGet (L7_UNIT_CURRENT,&entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objFdbbIntfEntryTypeValue =  entry.usmdbFdbEntryType;
  FPOBJ_TRACE_VALUE (bufp, &objFdbbIntfEntryTypeValue, sizeof (objFdbbIntfEntryTypeValue));
  /* return the object value: FdbbIntfEntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbbIntfEntryTypeValue,
                           sizeof (objFdbbIntfEntryTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbIntfDynamicEntriesGet
*
* @purpose Get 'FdbIntfDynamicEntriesGet'
 *@description  [FdbIntfDynamicEntriesGet] <HTML>Get count of dynamic entries in
* the L2FDB for a specific interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbIntfDynamicEntriesGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbIntfDynamicEntriesGetValue;

  xLibU32_t keyFdbInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbInterface */
  owa.len = sizeof (keyFdbInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbInterface,
                          (xLibU8_t *) & keyFdbInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbFdbIntfDynamicEntriesGet(keyFdbInterfaceValue,&objFdbIntfDynamicEntriesGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbIntfDynamicEntriesGetValue,
                     sizeof (objFdbIntfDynamicEntriesGetValue));

  /* return the object value: FdbIntfDynamicEntriesGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbIntfDynamicEntriesGetValue,
                           sizeof (objFdbIntfDynamicEntriesGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbIntfEntries_FdbIntfInternalEntriesGet
*
* @purpose Get 'FdbIntfInternalEntriesGet'
 *@description  [FdbIntfInternalEntriesGet] <HTML>Get count of internal entries
* in the L2FDB for a specific interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbIntfEntries_FdbIntfInternalEntriesGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbIntfInternalEntriesGetValue;

  xLibU32_t keyFdbInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbInterface */
  owa.len = sizeof (keyFdbInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbIntfEntries_FdbInterface,
                          (xLibU8_t *) & keyFdbInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbFdbIntfInternalEntriesGet (keyFdbInterfaceValue,
                                     &objFdbIntfInternalEntriesGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbIntfInternalEntriesGetValue,
                     sizeof (objFdbIntfInternalEntriesGetValue));

  /* return the object value: FdbIntfInternalEntriesGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbIntfInternalEntriesGetValue,
                           sizeof (objFdbIntfInternalEntriesGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
