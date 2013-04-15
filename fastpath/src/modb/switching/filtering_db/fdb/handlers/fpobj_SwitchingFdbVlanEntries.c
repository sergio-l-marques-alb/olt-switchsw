
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingFdbVlanEntries.c
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
#include "_xe_SwitchingFdbVlanEntries_obj.h"

#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_dot1q_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbVlanIndex
*
* @purpose Get 'FdbVlanIndex'
 *@description  [FdbVlanIndex] <HTML>Get the next entry from the FDB for a
* specific vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbVlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbVlanIndexValue;
  xLibU32_t nextObjFdbVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbVlanIndex */
  owa.len = sizeof (objFdbVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbVlanIndex,
                          (xLibU8_t *) & objFdbVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjFdbVlanIndexValue = DOT1Q_DEFAULT_VLAN;
    owa.l7rc =  L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objFdbVlanIndexValue, owa.len);
    owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, objFdbVlanIndexValue,
                                    &nextObjFdbVlanIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjFdbVlanIndexValue, owa.len);

  /* return the object value: FdbVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjFdbVlanIndexValue,
                           sizeof (nextObjFdbVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbMac
*
* @purpose Get 'FdbMac'
 *@description  [FdbMac] <HTML>The VLAN ID and Mac Address Combination of a
* given FDB Entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbMac (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objFdbMacValue;
  xLibStr256_t nextObjFdbMacValue;
  
  xLibU16_t vlanIdValue;
  xLibU32_t keyFdbVlanIndexValue;
  
  usmdbFdbEntry_t entry;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbVlanIndex */
  owa.len = sizeof (keyFdbVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbVlanIndex,
                          (xLibU8_t *) & keyFdbVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbVlanIndexValue, owa.len);

  memset(objFdbMacValue, 0x00, sizeof(objFdbMacValue));
  memset(nextObjFdbMacValue, 0x00, sizeof(nextObjFdbMacValue));

  vlanIdValue = (xLibU16_t)keyFdbVlanIndexValue;

  /* retrieve key: FdbMac */
  owa.len = sizeof (objFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbMac,
                          (xLibU8_t *) objFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&entry, 0, sizeof(entry));	
    owa.l7rc = usmDbFDBVlanEntryNextGet (vlanIdValue, &entry);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objFdbMacValue, owa.len);
    memcpy(entry.usmdbFdbMac, objFdbMacValue, L7_FDB_KEY_SIZE);
    memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
    memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));
    owa.l7rc = usmDbFDBVlanEntryNextGet (vlanIdValue, &entry);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjFdbMacValue, owa.len);

  memcpy(nextObjFdbMacValue, entry.usmdbFdbMac, L7_FDB_KEY_SIZE);
  /* return the object value: FdbMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjFdbMacValue, L7_FDB_KEY_SIZE);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbOnlyMac
*
* @purpose Get 'FdbOnlyMac'
 *@description  [FdbOnlyMac] <HTML>MAC Address of the FDB entry for a given VLAN
* Index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbOnlyMac (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr6_t objFdbOnlyMacValue;

  xLibStr256_t keyFdbMacValue;

  xLibU32_t vlanIdValueDummy;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbMac */
  owa.len = sizeof (keyFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbMac,
                          (xLibU8_t *) keyFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFdbMacValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbEntryVidMacSeparate (keyFdbMacValue, &vlanIdValueDummy,objFdbOnlyMacValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objFdbOnlyMacValue, sizeof (objFdbOnlyMacValue));

  /* return the object value: FdbOnlyMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFdbOnlyMacValue, sizeof (objFdbOnlyMacValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbVlanMacEntryType
*
* @purpose Get 'FdbVlanMacEntryType'
 *@description  [FdbVlanMacEntryType] <HTML>Entry Type of a given VID:MAC Entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbVlanMacEntryType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbVlanMacEntryTypeValue;

  xLibStr256_t keyFdbMacValue;
  
  usmdbFdbEntry_t entry;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbMac */
  owa.len = sizeof (keyFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbMac,
                          (xLibU8_t *) keyFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFdbMacValue, owa.len);
  
  memset(&entry.usmdbFdbMac, 0, sizeof(entry.usmdbFdbMac));
  memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
  memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));
  memcpy(entry.usmdbFdbMac, keyFdbMacValue,L7_FDB_KEY_SIZE); 

  /* get the value from application */
  owa.l7rc = usmDbFDBEntryGet (L7_UNIT_CURRENT,&entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objFdbVlanMacEntryTypeValue =  entry.usmdbFdbEntryType; 
  FPOBJ_TRACE_VALUE (bufp, &objFdbVlanMacEntryTypeValue, sizeof (objFdbVlanMacEntryTypeValue));

  /* return the object value: FdbVlanMacEntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbVlanMacEntryTypeValue,
                           sizeof (objFdbVlanMacEntryTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbVanIntf
*
* @purpose Get 'FdbVanIntf'
 *@description  [FdbVanIntf] <HTML>Interface Index of a given VID:MAC Combination   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbVanIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbVanIntfValue;

  xLibStr256_t keyFdbMacValue;

  FPOBJ_TRACE_ENTER (bufp);

  usmdbFdbEntry_t entry;
  /* retrieve key: FdbMac */
  owa.len = sizeof (keyFdbMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbMac,
                          (xLibU8_t *) keyFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFdbMacValue, owa.len);
  
  memset(&entry.usmdbFdbMac, 0, sizeof(entry.usmdbFdbMac));
  memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
  memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));
  memcpy(entry.usmdbFdbMac, keyFdbMacValue,L7_FDB_KEY_SIZE); 

  /* get the value from application */
  owa.l7rc = usmDbFDBEntryGet (L7_UNIT_CURRENT,&entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objFdbVanIntfValue =  entry.usmdbFdbIntIfNum; 
  FPOBJ_TRACE_VALUE (bufp, &objFdbVanIntfValue, sizeof (objFdbVanIntfValue));

  /* return the object value: FdbVanIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbVanIntfValue, sizeof (objFdbVanIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbVlanDynamicEntriesGet
*
* @purpose Get 'FdbVlanDynamicEntriesGet'
 *@description  [FdbVlanDynamicEntriesGet] <HTML>Get count of dynamic entries in
* the L2FDB for a specific VLAN   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbVlanDynamicEntriesGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbVlanDynamicEntriesGetValue;

  xLibU16_t tmpvlanIndexValue;
  xLibU32_t keyFdbVlanIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbVlanIndex */
  owa.len = sizeof (keyFdbVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbVlanIndex,
                          (xLibU8_t *) & keyFdbVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbVlanIndexValue, owa.len);
  
  /* get the value from application */
  tmpvlanIndexValue  = (xLibU16_t)keyFdbVlanIndexValue;
  owa.l7rc = usmDbFdbVlanDynamicEntriesGet (tmpvlanIndexValue,
                              &objFdbVlanDynamicEntriesGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbVlanDynamicEntriesGetValue,
                     sizeof (objFdbVlanDynamicEntriesGetValue));

  /* return the object value: FdbVlanDynamicEntriesGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbVlanDynamicEntriesGetValue,
                           sizeof (objFdbVlanDynamicEntriesGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbVlanEntries_FdbVlanInternalEntriesGet
*
* @purpose Get 'FdbVlanInternalEntriesGet'
 *@description  [FdbVlanInternalEntriesGet] <HTML> Get count of internal entries
* in the L2FDB for a specific VLAN   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbVlanEntries_FdbVlanInternalEntriesGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbVlanInternalEntriesGetValue;
  xLibU16_t tmpvlanIndexValue;
  xLibU32_t keyFdbVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbVlanIndex */
  owa.len = sizeof (keyFdbVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbVlanEntries_FdbVlanIndex,
                          (xLibU8_t *) & keyFdbVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbVlanIndexValue, owa.len);

  /* get the value from application */
  tmpvlanIndexValue  = (xLibU16_t)keyFdbVlanIndexValue;
  owa.l7rc = usmDbFdbVlanInternalEntriesGet (tmpvlanIndexValue,
                                         &objFdbVlanInternalEntriesGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbVlanInternalEntriesGetValue,
                     sizeof (objFdbVlanInternalEntriesGetValue));

  /* return the object value: FdbVlanInternalEntriesGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbVlanInternalEntriesGetValue,
                           sizeof (objFdbVlanInternalEntriesGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
