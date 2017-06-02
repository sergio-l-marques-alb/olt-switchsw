
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basePortStats.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 June 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basePortStats_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_garp.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_counters_api.h"
#include "usmdb_dot1q_api.h"

/*******************************************************************************
* @function fpObjSet_basePortStats_resetAllPortStats
*
* @purpose Set 'resetAllPortStats'
 *@description  [resetAllPortStats] Clear all port stats   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortStats_resetAllPortStats (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objresetAllPortStatsValue;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: resetAllPortStats */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objresetAllPortStatsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objresetAllPortStatsValue, owa.len);

  /* if row status object is specified and eual to delete return success */
  owa.l7rc = L7_SUCCESS;

  /* set the value in application */
  rc = usmDbValidIntIfNumFirstGet(&intIfIndex);
  while (rc == L7_SUCCESS)
  {
    usmDbIntfStatReset(L7_UNIT_CURRENT, intIfIndex);
    rc = usmDbValidIntIfNumNext(intIfIndex, &intIfIndex);
  }
		
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_basePortStats_resetSwitchStats
*
* @purpose Set 'resetSwitchStats'
 *@description  [resetSwitchStats] Clear switch stats   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortStats_resetSwitchStats (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objresetSwitchStatsValue;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: resetSwitchStats */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objresetSwitchStatsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objresetSwitchStatsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbMgtSwitchintIfNumGet(L7_UNIT_CURRENT, &intIfIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
      owa.l7rc = usmDbIntfStatReset(L7_UNIT_CURRENT, intIfIndex);
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qVlanEntriesInUseCount
*
* @purpose Get 'dot1qVlanEntriesInUseCount'
 *@description  [dot1qVlanEntriesInUseCount] VLAN Entries Currently in Use    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qVlanEntriesInUseCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1qVlanEntriesInUseCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qNumVlansGet (L7_UNIT_CURRENT, &objdot1qVlanEntriesInUseCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdot1qVlanEntriesInUseCountValue,
                     sizeof (objdot1qVlanEntriesInUseCountValue));

  /* return the object value: dot1qVlanEntriesInUseCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1qVlanEntriesInUseCountValue,
                           sizeof (objdot1qVlanEntriesInUseCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qMostVlanEntriesEverInUseCount
*
* @purpose Get 'dot1qMostVlanEntriesEverInUseCount'
 *@description  [dot1qMostVlanEntriesEverInUseCount] Most VLAN Entries ever in
* use.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qMostVlanEntriesEverInUseCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1qMostVlanEntriesEverInUseCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMostVlanEntriesGet (L7_UNIT_CURRENT, &objdot1qMostVlanEntriesEverInUseCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdot1qMostVlanEntriesEverInUseCountValue,
                     sizeof (objdot1qMostVlanEntriesEverInUseCountValue));

  /* return the object value: dot1qMostVlanEntriesEverInUseCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1qMostVlanEntriesEverInUseCountValue,
                           sizeof (objdot1qMostVlanEntriesEverInUseCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qMaxVlanEntriesSupported
*
* @purpose Get 'dot1qMaxVlanEntriesSupported'
 *@description  [dot1qMaxVlanEntriesSupported] Max VLAN Entries supported.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qMaxVlanEntriesSupported (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1qMaxVlanEntriesSupportedValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qMaxSupportedVlansGet (L7_UNIT_CURRENT, &objdot1qMaxVlanEntriesSupportedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdot1qMaxVlanEntriesSupportedValue,
                     sizeof (objdot1qMaxVlanEntriesSupportedValue));

  /* return the object value: dot1qMaxVlanEntriesSupported */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1qMaxVlanEntriesSupportedValue,
                           sizeof (objdot1qMaxVlanEntriesSupportedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qStaticVlanCount
*
* @purpose Get 'dot1qStaticVlanCount'
 *@description  [dot1qStaticVlanCount] Total Static VLAN s.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qStaticVlanCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1qStaticVlanCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStaticVlanEntriesGet (L7_UNIT_CURRENT, &objdot1qStaticVlanCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdot1qStaticVlanCountValue, sizeof (objdot1qStaticVlanCountValue));

  /* return the object value: dot1qStaticVlanCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1qStaticVlanCountValue,
                           sizeof (objdot1qStaticVlanCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qDynamicVlanCount
*
* @purpose Get 'dot1qDynamicVlanCount'
 *@description  [dot1qDynamicVlanCount] Total Dynamic VLAN s.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qDynamicVlanCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1qDynamicVlanCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDynamicVlanEntriesGet (L7_UNIT_CURRENT, &objdot1qDynamicVlanCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdot1qDynamicVlanCountValue, sizeof (objdot1qDynamicVlanCountValue));

  /* return the object value: dot1qDynamicVlanCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1qDynamicVlanCountValue,
                           sizeof (objdot1qDynamicVlanCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_basePortStats_statsInterfaceType
*
* @purpose Set 'statsInterfaceType'
 *@description  [statsInterfaceType] If the object StatsInterfaceType is set to
* Switch, only CPU interfaces will be populates else port
* interfaces will be populated.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortStats_statsInterfaceType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstatsInterfaceTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: statsInterfaceType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objstatsInterfaceTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objstatsInterfaceTypeValue, owa.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_basePortStats_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interface no   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Interface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

   /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface, (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_L2_TUNNEL_INTF, 
                                         0, &nextObjInterfaceValue);

  }
  else
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_L2_TUNNEL_INTF, 
                                        0, objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_basePortStats_resetintfPortStats
*
* @purpose Set 'resetintfPortStats'
 *@description  [resetintfPortStats] Clear stats on specified port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_CpuInterface(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  xLibU32_t val;

   /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_basePortStats_CpuInterface, (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbMgtSwitchintIfNumGet(L7_UNIT_CURRENT, &val);

    if (owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbIfIndexGet(L7_UNIT_CURRENT, val, &nextObjInterfaceValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGe_basePortStats_timeSinceCountersLastCleared
*
* @purpose Get 'timeSinceCountersLastCleared'
*
* @description [timeSinceCountersLastCleared] timeSinceCountersLastCleared
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_timeSinceCountersLastCleared (void *wap, void *bufp)
{

  fpObjWa_t kwaBasePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyBasePortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtimeSinceCountersLastClearedValue;
  FPOBJ_TRACE_ENTER (bufp);

    usmDbTimeSpec_t ts;


  /* retrieve key: interface */
  kwaBasePort.rc = xLibFilterGet (wap,XOBJ_basePortStats_Interface ,
                                       (xLibU8_t *) & keyBasePortValue, &kwaBasePort.len);
  if (kwaBasePort.rc != XLIBRC_SUCCESS)
  {
    kwaBasePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaBasePort);
    return kwaBasePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyBasePortValue, kwaBasePort.len);

  /* set the value in application */
  owa.l7rc = usmDbTimeSinceLastStatsResetGet (L7_UNIT_CURRENT, keyBasePortValue, &ts);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  sprintf(objtimeSinceCountersLastClearedValue,"%d day %d hr %d min %d sec", ts.days, ts.hours, ts.minutes, ts.seconds);


  /* return the object value: timeSinceCountersLastCleared */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtimeSinceCountersLastClearedValue,
                           strlen (objtimeSinceCountersLastClearedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjSet_basePortStats_resetintfPortStats (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objresetintfPortStatsValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: resetintfPortStats */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objresetintfPortStatsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objresetintfPortStatsValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbIntfStatReset (L7_UNIT_CURRENT, keyInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_basePortStats_totalPacketsRx
*
* @purpose Get 'totalPacketsRx'
 *@description  [totalPacketsRx] total packets received   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_totalPacketsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtotalPacketsRxValue;
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_BYTES, 
                                     keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objtotalPacketsRxValue, 0x00, sizeof(objtotalPacketsRxValue));
  memcpy(objtotalPacketsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: totalPacketsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtotalPacketsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts64OctetsRx
*
* @purpose Get 'Pkts64OctetsRx'
 *@description  [Pkts64OctetsRx] total received Packets that are 64 octets in
* length .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts64OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts64OctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_64, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts64OctetsRxValue, 0x00, sizeof(objPkts64OctetsRxValue));
  memcpy(objPkts64OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts64OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts65To127OctetsRx
*
* @purpose Get 'Pkts65To127OctetsRx'
 *@description  [Pkts65To127OctetsRx] total received Packets that are 65 to 127
* octets in length (excluding framing bits, including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts65To127OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts65To127OctetsRxValue;
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_65_127, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts65To127OctetsRxValue, 0x00, sizeof(objPkts65To127OctetsRxValue));
  memcpy(objPkts65To127OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts65To127OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts65To127OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts128To255OctetsRx
*
* @purpose Get 'Pkts128To255OctetsRx'
 *@description  [Pkts128To255OctetsRx] total received Packets that are 128 to
* 255 octets in length (excluding framing bits, including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts128To255OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts128To255OctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_128_255, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts128To255OctetsRxValue, 0x00, sizeof(objPkts128To255OctetsRxValue));
  memcpy(objPkts128To255OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts128To255OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts128To255OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts256To511OctetsRx
*
* @purpose Get 'Pkts256To511OctetsRx'
 *@description  [Pkts256To511OctetsRx] total received Packets that are 256 To
* 511 octets in length (excluding framing bits, including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts256To511OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts256To511OctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_256_511, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts256To511OctetsRxValue, 0x00, sizeof(objPkts256To511OctetsRxValue));
  memcpy(objPkts256To511OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts256To511OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts256To511OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts512To1023OctetsRx
*
* @purpose Get 'Pkts512To1023OctetsRx'
 *@description  [Pkts512To1023OctetsRx] total received Packets that are 512 to
* 1023 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts512To1023OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts512To1023OctetsRxValue;
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_512_1023, keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts512To1023OctetsRxValue, 0x00, sizeof(objPkts512To1023OctetsRxValue));
  memcpy(objPkts512To1023OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts512To1023OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts512To1023OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1024To1518OctetsRx
*
* @purpose Get 'Pkts1024To1518OctetsRx'
 *@description  [Pkts1024To1518OctetsRx] total received Packets that are 1024 to
* 1518 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1024To1518OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1024To1518OctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_1024_1518, keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1024To1518OctetsRxValue, 0x00, sizeof(objPkts1024To1518OctetsRxValue));
  memcpy(objPkts1024To1518OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1024To1518OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1024To1518OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1519To1530OctetsRx
*
* @purpose Get 'Pkts1519To1530OctetsRx'
 *@description  [Pkts1519To1530OctetsRx] total received Packets that are 1519 to
* 1530 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1519To1530OctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1519To1530OctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_1519_1530, keyInterfaceValue, &high, &low);  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1519To1530OctetsRxValue, 0x00, sizeof(objPkts1519To1530OctetsRxValue));
  memcpy(objPkts1519To1530OctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1519To1530OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1519To1530OctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsGoodOversizedOctetsRx
*
* @purpose Get 'PktsGoodOversizedOctetsRx'
 *@description  [PktsGoodOversizedOctetsRx] total received Packets that are good
* oversized   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsGoodOversizedOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsGoodOversizedOctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_GOOD_OVERSIZE, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsGoodOversizedOctetsRxValue, 0x00, sizeof(objPktsGoodOversizedOctetsRxValue));
  memcpy(objPktsGoodOversizedOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsGoodOversizedOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsGoodOversizedOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsErrorOversizedOctetsRx
*
* @purpose Get 'PktsErrorOversizedOctetsRx'
 *@description  [PktsErrorOversizedOctetsRx] total received Packets that are
* error  oversized   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsErrorOversizedOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsErrorOversizedOctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_ERROR_OVERSIZE, 
                                        keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsErrorOversizedOctetsRxValue, 0x00, sizeof(objPktsErrorOversizedOctetsRxValue));
  memcpy(objPktsErrorOversizedOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsErrorOversizedOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsErrorOversizedOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsGoodUnderrsizedOctetsRx
*
* @purpose Get 'PktsGoodUnderrsizedOctetsRx'
 *@description  [PktsGoodUnderrsizedOctetsRx] total received Packets that are
* good under-sized   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsGoodUnderrsizedOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsGoodUnderrsizedOctetsRxValue;
	
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */


  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_GOOD_UNDERSIZE, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsGoodUnderrsizedOctetsRxValue, 0x00, sizeof(objPktsGoodUnderrsizedOctetsRxValue));
  memcpy(objPktsGoodUnderrsizedOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsGoodUnderrsizedOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsGoodUnderrsizedOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsErrorUnderrsizedOctetsRx
*
* @purpose Get 'PktsErrorUnderrsizedOctetsRx'
 *@description  [PktsErrorUnderrsizedOctetsRx] total received Packets that are
* error under-sized   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsErrorUnderrsizedOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsErrorUnderrsizedOctetsRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_ERROR_UNDERSIZE, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsErrorUnderrsizedOctetsRxValue, 0x00, sizeof(objPktsErrorUnderrsizedOctetsRxValue));
  memcpy(objPktsErrorUnderrsizedOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsErrorUnderrsizedOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsErrorUnderrsizedOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsAlignErrorOctetsRx
*
* @purpose Get 'PktsAlignErrorOctetsRx'
 *@description  [PktsAlignErrorOctetsRx] total received Packets that are Align
* error.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsAlignErrorOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsAlignErrorOctetsRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_ALIGN_ERRORS, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsAlignErrorOctetsRxValue, 0x00, sizeof(objPktsAlignErrorOctetsRxValue));
  memcpy(objPktsAlignErrorOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsAlignErrorOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsAlignErrorOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsFCSErrorOctetsRx
*
* @purpose Get 'PktsFCSErrorOctetsRx'
 *@description  [PktsFCSErrorOctetsRx] total received Packets that are FCS error.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsFCSErrorOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsFCSErrorOctetsRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_FCS_ERRORS, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsFCSErrorOctetsRxValue, 0x00, sizeof(objPktsFCSErrorOctetsRxValue));
  memcpy(objPktsFCSErrorOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsFCSErrorOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsFCSErrorOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsOverRunsOctetsRx
*
* @purpose Get 'PktsOverRunsOctetsRx'
 *@description  [PktsOverRunsOctetsRx] total received Packets that are over runs.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsOverRunsOctetsRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsOverRunsOctetsRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_OVERRUNS, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsOverRunsOctetsRxValue, 0x00, sizeof(objPktsOverRunsOctetsRxValue));
  memcpy(objPktsOverRunsOctetsRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsOverRunsOctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsOverRunsOctetsRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_totalPacketsTx
*
* @purpose Get 'totalPacketsTx'
 *@description  [totalPacketsTx] total packets transmitted   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_totalPacketsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtotalPacketsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_BYTES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objtotalPacketsTxValue, 0x00, sizeof(objtotalPacketsTxValue));
  memcpy(objtotalPacketsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: totalPacketsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtotalPacketsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts64OctetsTx
*
* @purpose Get 'Pkts64OctetsTx'
 *@description  [Pkts64OctetsTx] total transmitted Packets that are 64 octets in
* length .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts64OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts64OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
 owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_64, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts64OctetsTxValue, 0x00, sizeof(objPkts64OctetsTxValue));
  memcpy(objPkts64OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts64OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts64OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts65To127OctetsTx
*
* @purpose Get 'Pkts65To127OctetsTx'
 *@description  [Pkts65To127OctetsTx] total transmitted Packets that are 65 to
* 127 octets in length (excluding framing bits, including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts65To127OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts65To127OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_65_127, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts65To127OctetsTxValue, 0x00, sizeof(objPkts65To127OctetsTxValue));
  memcpy(objPkts65To127OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts65To127OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts65To127OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts128To255OctetsTx
*
* @purpose Get 'Pkts128To255OctetsTx'
 *@description  [Pkts128To255OctetsTx] total transmitted Packets that are 128 to
* 255 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts128To255OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts128To255OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_128_255, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts128To255OctetsTxValue, 0x00, sizeof(objPkts128To255OctetsTxValue));
  memcpy(objPkts128To255OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts128To255OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts128To255OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts256To511OctetsTx
*
* @purpose Get 'Pkts256To511OctetsTx'
 *@description  [Pkts256To511OctetsTx] total transmitted Packets that are 256 To
* 511 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts256To511OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts256To511OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_256_511, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts256To511OctetsTxValue, 0x00, sizeof(objPkts256To511OctetsTxValue));
  memcpy(objPkts256To511OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts256To511OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts256To511OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts512To1023OctetsTx
*
* @purpose Get 'Pkts512To1023OctetsTx'
 *@description  [Pkts512To1023OctetsTx] total transmitted Packets that are 512
* to 1023 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts512To1023OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts512To1023OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_512_1023, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts512To1023OctetsTxValue, 0x00, sizeof(objPkts512To1023OctetsTxValue));
  memcpy(objPkts512To1023OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts512To1023OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts512To1023OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1024To1518OctetsTx
*
* @purpose Get 'Pkts1024To1518OctetsTx'
 *@description  [Pkts1024To1518OctetsTx] total transmitted Packets that are 1024
* to 1518 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1024To1518OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1024To1518OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_1024_1518, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1024To1518OctetsTxValue, 0x00, sizeof(objPkts1024To1518OctetsTxValue));
  memcpy(objPkts1024To1518OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1024To1518OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1024To1518OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1519To1530OctetsTx
*
* @purpose Get 'Pkts1519To1530OctetsTx'
 *@description  [Pkts1519To1530OctetsTx] total transmitted Packets that are 1519
* to 1530 octets in length (excluding framing bits, including FCS
* octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1519To1530OctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1519To1530OctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_1519_1530, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1519To1530OctetsTxValue, 0x00, sizeof(objPkts1519To1530OctetsTxValue));
  memcpy(objPkts1519To1530OctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1519To1530OctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1519To1530OctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsFCSErrorOctetsTx
*
* @purpose Get 'PktsFCSErrorOctetsTx'
 *@description  [PktsFCSErrorOctetsTx] total transmitted Packets that has FCS
* errors.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsFCSErrorOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsFCSErrorOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_FCS_ERRORS, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsFCSErrorOctetsTxValue, 0x00, sizeof(objPktsFCSErrorOctetsTxValue));
  memcpy(objPktsFCSErrorOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsFCSErrorOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsFCSErrorOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsOverSizedOctetsTx
*
* @purpose Get 'PktsOverSizedOctetsTx'
 *@description  [PktsOverSizedOctetsTx] total transmitted Packets that are
* over-sized errors.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsOverSizedOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsOverSizedOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_OVERSIZED, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsOverSizedOctetsTxValue, 0x00, sizeof(objPktsOverSizedOctetsTxValue));
  memcpy(objPktsOverSizedOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsOverSizedOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsOverSizedOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsUnderRunErrorOctetsTx
*
* @purpose Get 'PktsUnderRunErrorOctetsTx'
 *@description  [PktsUnderRunErrorOctetsTx] total transmitted Packets that has
* under run errors.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsUnderRunErrorOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsUnderRunErrorOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_UNDERRUN_ERRORS, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsUnderRunErrorOctetsTxValue, 0x00, sizeof(objPktsUnderRunErrorOctetsTxValue));
  memcpy(objPktsUnderRunErrorOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsUnderRunErrorOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsUnderRunErrorOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsOneCollisionOctetsTx
*
* @purpose Get 'PktsOneCollisionOctetsTx'
 *@description  [PktsOneCollisionOctetsTx] total transmitted Packets that has
* one collision.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsOneCollisionOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsOneCollisionOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_ONE_COLLISION, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsOneCollisionOctetsTxValue, 0x00, sizeof(objPktsOneCollisionOctetsTxValue));
  memcpy(objPktsOneCollisionOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));
  /* return the object value: PktsOneCollisionOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsOneCollisionOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsMultipleCollisionOctetsTx
*
* @purpose Get 'PktsMultipleCollisionOctetsTx'
 *@description  [PktsMultipleCollisionOctetsTx] total transmitted Packets that
* has multiple collisions.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsMultipleCollisionOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsMultipleCollisionOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_MULTIPLE_COLLISION, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsMultipleCollisionOctetsTxValue, 0x00, sizeof(objPktsMultipleCollisionOctetsTxValue));
  memcpy(objPktsMultipleCollisionOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsMultipleCollisionOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsMultipleCollisionOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsExcessiveCollisionOctetsTx
*
* @purpose Get 'PktsExcessiveCollisionOctetsTx'
 *@description  [PktsExcessiveCollisionOctetsTx] total transmitted Packets that
* has Excessive collisions.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsExcessiveCollisionOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsExcessiveCollisionOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_EXCESSIVE_COLLISION, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsExcessiveCollisionOctetsTxValue, 0x00, sizeof(objPktsExcessiveCollisionOctetsTxValue));
  memcpy(objPktsExcessiveCollisionOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsExcessiveCollisionOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsExcessiveCollisionOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_PktsLateCollisionOctetsTx
*
* @purpose Get 'PktsLateCollisionOctetsTx'
 *@description  [PktsLateCollisionOctetsTx] total transmitted Packets that has
* Late collisions.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_PktsLateCollisionOctetsTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPktsLateCollisionOctetsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_LATE_COLLISION, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPktsLateCollisionOctetsTxValue, 0x00, sizeof(objPktsLateCollisionOctetsTxValue));
  memcpy(objPktsLateCollisionOctetsTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: PktsLateCollisionOctetsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPktsLateCollisionOctetsTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts64OctetsRxTx
*
* @purpose Get 'Pkts64OctetsRxTx'
 *@description  [Pkts64OctetsRxTx] total received and transmitted Packets that
* are 64 octets in length .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts64OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts64OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_64, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts64OctetsRxTxValue, 0x00, sizeof(objPkts64OctetsRxTxValue));
  memcpy(objPkts64OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts64OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts64OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts65To127OctetsRxTx
*
* @purpose Get 'Pkts65To127OctetsRxTx'
 *@description  [Pkts65To127OctetsRxTx] total received and transmitted Packets
* that are 65 to 127 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts65To127OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts65To127OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_65_127, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts65To127OctetsRxTxValue, 0x00, sizeof(objPkts65To127OctetsRxTxValue));
  memcpy(objPkts65To127OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts65To127OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts65To127OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts128To255OctetsRxTx
*
* @purpose Get 'Pkts128To255OctetsRxTx'
 *@description  [Pkts128To255OctetsRxTx] total received and transmitted Packets
* that are 128 to 255 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts128To255OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts128To255OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_128_255, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts128To255OctetsRxTxValue, 0x00, sizeof(objPkts128To255OctetsRxTxValue));
  memcpy(objPkts128To255OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts128To255OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts128To255OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts256To511OctetsRxTx
*
* @purpose Get 'Pkts256To511OctetsRxTx'
 *@description  [Pkts256To511OctetsRxTx] total received and transmitted Packets
* that are 256 To 511 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts256To511OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts256To511OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_256_511, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts256To511OctetsRxTxValue, 0x00, sizeof(objPkts256To511OctetsRxTxValue));
  memcpy(objPkts256To511OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts256To511OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts256To511OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts512To1023OctetsRxTx
*
* @purpose Get 'Pkts512To1023OctetsRxTx'
 *@description  [Pkts512To1023OctetsRxTx] total received and transmitted Packets
* that are 512 to 1023 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts512To1023OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts512To1023OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_512_1023, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts512To1023OctetsRxTxValue, 0x00, sizeof(objPkts512To1023OctetsRxTxValue));
  memcpy(objPkts512To1023OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts512To1023OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts512To1023OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1024To1518OctetsRxTx
*
* @purpose Get 'Pkts1024To1518OctetsRxTx'
 *@description  [Pkts1024To1518OctetsRxTx] total received and transmitted
* Packets that are 1024 to 1518 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1024To1518OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1024To1518OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_1024_1518, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1024To1518OctetsRxTxValue, 0x00, sizeof(objPkts1024To1518OctetsRxTxValue));
  memcpy(objPkts1024To1518OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1024To1518OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1024To1518OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1519To1522OctetsRxTx
*
* @purpose Get 'Pkts1519To1522OctetsRxTx'
 *@description  [Pkts1519To1522OctetsRxTx] total received and transmitted
* Packets that are 1519 to 1522 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1519To1522OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1519To1522OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_1519_1522, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1519To1522OctetsRxTxValue, 0x00, sizeof(objPkts1519To1522OctetsRxTxValue));
  memcpy(objPkts1519To1522OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1519To1522OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1519To1522OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts1523To2047OctetsRxTx
*
* @purpose Get 'Pkts1523To2047OctetsRxTx'
 *@description  [Pkts1523To2047OctetsRxTx] total received and transmitted
* Packets that are 1523 to 2047 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts1523To2047OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts1523To2047OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_1523_2047, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts1523To2047OctetsRxTxValue, 0x00, sizeof(objPkts1523To2047OctetsRxTxValue));
  memcpy(objPkts1523To2047OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts1523To2047OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts1523To2047OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts2048To4095OctetsRxTx
*
* @purpose Get 'Pkts2048To4095OctetsRxTx'
 *@description  [Pkts2048To4095OctetsRxTx] total received and transmitted
* Packets that are 2048 to 4095 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts2048To4095OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts2048To4095OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_2048_4095, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts2048To4095OctetsRxTxValue, 0x00, sizeof(objPkts2048To4095OctetsRxTxValue));
  memcpy(objPkts2048To4095OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts2048To4095OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts2048To4095OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_Pkts4096To9216OctetsRxTx
*
* @purpose Get 'Pkts4096To9216OctetsRxTx'
 *@description  [Pkts4096To9216OctetsRxTx] total received and transmitted
* Packets that are 4096 to 9216 octets in length (excluding framing bits,
* including FCS octets)   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_Pkts4096To9216OctetsRxTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPkts4096To9216OctetsRxTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_RX_4096_9216, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objPkts4096To9216OctetsRxTxValue, 0x00, sizeof(objPkts4096To9216OctetsRxTxValue));
  memcpy(objPkts4096To9216OctetsRxTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: Pkts4096To9216OctetsRxTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPkts4096To9216OctetsRxTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalFramesRx
*
* @purpose Get 'TotalFramesRx'
 *@description  [TotalFramesRx] total frames received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_FRAMES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalFramesRxValue, 0x00, sizeof(objTotalFramesRxValue));
  memcpy(objTotalFramesRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalFramesRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalErrorFramesRx
*
* @purpose Get 'TotalErrorFramesRx'
 *@description  [TotalErrorFramesRx] total error frames received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalErrorFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalErrorFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_ERROR_FRAMES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalErrorFramesRxValue, 0x00, sizeof(objTotalErrorFramesRxValue));
  memcpy(objTotalErrorFramesRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalErrorFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalErrorFramesRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalFramesTx
*
* @purpose Get 'TotalFramesTx'
 *@description  [TotalFramesTx] total frames transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_FRAMES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalFramesTxValue, 0x00, sizeof(objTotalFramesTxValue));
  memcpy(objTotalFramesTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalFramesTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalErrorFramesTx
*
* @purpose Get 'TotalErrorFramesTx'
 *@description  [TotalErrorFramesTx] total error frames transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalErrorFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalErrorFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_ERROR_FRAMES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalErrorFramesTxValue, 0x00, sizeof(objTotalErrorFramesTxValue));
  memcpy(objTotalErrorFramesTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalErrorFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalErrorFramesTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalCollisionFramesTx
*
* @purpose Get 'TotalCollisionFramesTx'
 *@description  [TotalCollisionFramesTx] total collision frames transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalCollisionFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalCollisionFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_COLLISION_FRAMES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalCollisionFramesTxValue, 0x00, sizeof(objTotalCollisionFramesTxValue));
  memcpy(objTotalCollisionFramesTxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalCollisionFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalCollisionFramesTxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalCRCErrorFramesRx
*
* @purpose Get 'TotalCRCErrorFramesRx'
 *@description  [TotalCRCErrorFramesRx] total  frames received with CRC errors.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalCRCErrorFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalCRCErrorFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_CRC_ERRORS, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalCRCErrorFramesRxValue, 0x00, sizeof(objTotalCRCErrorFramesRxValue));
  memcpy(objTotalCRCErrorFramesRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalCRCErrorFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalCRCErrorFramesRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalMacErrorFramesRx
*
* @purpose Get 'TotalMacErrorFramesRx'
 *@description  [TotalMacErrorFramesRx] total  frames received with Mac errors.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalMacErrorFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalMacErrorFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);
	
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

	/* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES, 
                                        keyInterfaceValue, &high, &low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalMacErrorFramesRxValue, 0x00, sizeof(objTotalMacErrorFramesRxValue));
  memcpy(objTotalMacErrorFramesRxValue,(void *)&data64, sizeof(L7_ulong64));

  /* return the object value: TotalMacErrorFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalMacErrorFramesRxValue,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_TotalPacketsNotFwdRx
*
* @purpose Get 'TotalPacketsNotFwdRx'
 *@description  [TotalPacketsNotFwdRx] Total Received Packets Not Forwarded.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_TotalPacketsNotFwdRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalPacketsNotFwdRxValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_RX_TOTAL_NOT_FORWARDED_FRAMES, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objTotalPacketsNotFwdRxValue, 0x00, sizeof(objTotalPacketsNotFwdRxValue));
  memcpy(objTotalPacketsNotFwdRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalPacketsNotFwdRxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_LocalTrafficFrames
*
* @purpose Get 'LocalTrafficFrames'
 *@description  [LocalTrafficFrames] LocalTrafficFrames.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_LocalTrafficFrames (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLocalTrafficFramesValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_LOCAL_TRAFFIC_DISCARDS , keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objLocalTrafficFramesValue, 0x00, sizeof(objLocalTrafficFramesValue));
  memcpy(objLocalTrafficFramesValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLocalTrafficFramesValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qPauseFramesRx
*
* @purpose Get 'dot1qPauseFramesRx'
 *@description  [dot1qPauseFramesRx] 802.3x Pause Frames Received .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qPauseFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qPauseFramesRxValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_RX_PAUSE_FRAMES, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qPauseFramesRxValue, 0x00, sizeof(objdot1qPauseFramesRxValue));
  memcpy(objdot1qPauseFramesRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qPauseFramesRxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qUnacceptableFrameType
*
* @purpose Get 'dot1qUnacceptableFrameType'
 *@description  [dot1qUnacceptableFrameType] Unacceptable Frame Type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qUnacceptableFrameType (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qUnacceptableFrameTypeValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
 owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_UNACCEPTABLE_FRAME_TYPE_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qUnacceptableFrameTypeValue, 0x00, sizeof(objdot1qUnacceptableFrameTypeValue));
  memcpy(objdot1qUnacceptableFrameTypeValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qUnacceptableFrameTypeValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
	
}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qMCastTreeViablediscards
*
* @purpose Get 'dot1qMCastTreeViablediscards'
 *@description  [dot1qMCastTreeViablediscards] dot1q MCast Tree Viable discards .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qMCastTreeViablediscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qMCastTreeViablediscardsValue;

  	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_MULTICAST_TREE_VIABLE_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qMCastTreeViablediscardsValue, 0x00, sizeof(objdot1qMCastTreeViablediscardsValue));
  memcpy(objdot1qMCastTreeViablediscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qMCastTreeViablediscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qReservedAddressDiscards
*
* @purpose Get 'dot1qReservedAddressDiscards'
 *@description  [dot1qReservedAddressDiscards] Reserved Address Discards.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qReservedAddressDiscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qReservedAddressDiscardsValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_RESERVED_ADDRESS_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qReservedAddressDiscardsValue, 0x00, sizeof(objdot1qReservedAddressDiscardsValue));
  memcpy(objdot1qReservedAddressDiscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qReservedAddressDiscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qBroadcastStrmRecoverDiscards
*
* @purpose Get 'dot1qBroadcastStrmRecoverDiscards'
 *@description  [dot1qBroadcastStrmRecoverDiscards] dot1q Broadcast Storm
* Recovery discards .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qBroadcastStrmRecoverDiscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qBroadcastStrmRecoverDiscardsValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_BCAST_STORM_RECOVERY_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qBroadcastStrmRecoverDiscardsValue, 0x00, sizeof(objdot1qBroadcastStrmRecoverDiscardsValue));
  memcpy(objdot1qBroadcastStrmRecoverDiscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qBroadcastStrmRecoverDiscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qCFIDiscards
*
* @purpose Get 'dot1qCFIDiscards'
 *@description  [dot1qCFIDiscards] dot1q CFI Discards.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qCFIDiscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qCFIDiscardsValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_CFI_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qCFIDiscardsValue, 0x00, sizeof(objdot1qCFIDiscardsValue));
  memcpy(objdot1qCFIDiscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qCFIDiscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qUpStreamthresholdDiscards
*
* @purpose Get 'dot1qUpStreamthresholdDiscards'
 *@description  [dot1qUpStreamthresholdDiscards] dot1q UpStream threshold
* Discards.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qUpStreamthresholdDiscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qUpStreamthresholdDiscardsValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_UPSTREAM_THRESHOLD_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qUpStreamthresholdDiscardsValue, 0x00, sizeof(objdot1qUpStreamthresholdDiscardsValue));
  memcpy(objdot1qUpStreamthresholdDiscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qUpStreamthresholdDiscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qPortMembershipDiscards
*
* @purpose Get 'dot1qPortMembershipDiscards'
 *@description  [dot1qPortMembershipDiscards] dot1q PortMembership Discards.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qPortMembershipDiscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qPortMembershipDiscardsValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_PORT_MEMBERSHIP_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qPortMembershipDiscardsValue, 0x00, sizeof(objdot1qPortMembershipDiscardsValue));
  memcpy(objdot1qPortMembershipDiscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qPortMembershipDiscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qEgressVLANViableDiscards
*
* @purpose Get 'dot1qEgressVLANViableDiscards'
 *@description  [dot1qEgressVLANViableDiscards] dot1qEgressVLANViableDiscards.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qEgressVLANViableDiscards (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qEgressVLANViableDiscardsValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_EGRESS_VLAN_VIABLE_DISCARDS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qEgressVLANViableDiscardsValue, 0x00, sizeof(objdot1qEgressVLANViableDiscardsValue));
  memcpy(objdot1qEgressVLANViableDiscardsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qEgressVLANViableDiscardsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_dot1qPauseFramesTx
*
* @purpose Get 'dot1qPauseFramesTx'
 *@description  [dot1qPauseFramesTx] dot1q Pause Frames Transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_dot1qPauseFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1qPauseFramesTxValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_DOT1Q_TX_PAUSE_FRAMES, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objdot1qPauseFramesTxValue, 0x00, sizeof(objdot1qPauseFramesTxValue));
  memcpy(objdot1qPauseFramesTxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1qPauseFramesTxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}

#if L7_FEAT_GMRP
/*******************************************************************************
* @function fpObjGet_basePortStats_GVRPPDUCountRx
*
* @purpose Get 'GVRPPDUCountRx'
 *@description  [GVRPPDUCountRx] Total GVRP PDUs received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_GVRPPDUCountRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGVRPPDUCountRxValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_GVRP_PDU_COUNT, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objGVRPPDUCountRxValue, 0x00, sizeof(objGVRPPDUCountRxValue));
  memcpy(objGVRPPDUCountRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGVRPPDUCountRxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_GVRPPDUCountTx
*
* @purpose Get 'GVRPPDUCountTx'
 *@description  [GVRPPDUCountTx] Total GVRP PDUs transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_GVRPPDUCountTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGVRPPDUCountTxValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT,  L7_CTR_TX_GVRP_PDU_COUNT, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objGVRPPDUCountTxValue, 0x00, sizeof(objGVRPPDUCountTxValue));
  memcpy(objGVRPPDUCountTxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGVRPPDUCountTxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_GVRPFailedRegistrations
*
* @purpose Get 'GVRPFailedRegistrations'
 *@description  [GVRPFailedRegistrations] GVRP Failed Registrations.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_GVRPFailedRegistrations (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGVRPFailedRegistrationsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
	
  owa.l7rc = usmDbDot1qPortGvrpFailedRegistrations (L7_UNIT_CURRENT, keyInterfaceValue, &objGVRPFailedRegistrationsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GVRPFailedRegistrations */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objGVRPFailedRegistrationsValue,
                           sizeof (objGVRPFailedRegistrationsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_GMRPPDUCountRx
*
* @purpose Get 'GMRPPDUCountRx'
 *@description  [GMRPPDUCountRx] Total GMRP PDUs received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_GMRPPDUCountRx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGMRPPDUCountRxValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */

 owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_RX_GMRP_PDU_COUNT, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objGMRPPDUCountRxValue, 0x00, sizeof(objGMRPPDUCountRxValue));
  memcpy(objGMRPPDUCountRxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGMRPPDUCountRxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_GMRPPDUCountTx
*
* @purpose Get 'GMRPPDUCountTx'
 *@description  [GMRPPDUCountTx] Total GMRP PDUs transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_GMRPPDUCountTx (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGMRPPDUCountTxValue;

	  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_TX_GMRP_PDU_COUNT, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objGMRPPDUCountTxValue, 0x00, sizeof(objGMRPPDUCountTxValue));
  memcpy(objGMRPPDUCountTxValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGMRPPDUCountTxValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortStats_GMRPFailedRegistrations
*
* @purpose Get 'GMRPFailedRegistrations'
 *@description  [GMRPFailedRegistrations] GMRP Failed Registrations.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortStats_GMRPFailedRegistrations (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGMRPFailedRegistrationsValue;

  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_basePortStats_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet64(L7_UNIT_CURRENT, L7_CTR_GMRP_FAILED_REGISTRATIONS, keyInterfaceValue, &high, &low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objGMRPFailedRegistrationsValue, 0x00, sizeof(objGMRPFailedRegistrationsValue));
  memcpy(objGMRPFailedRegistrationsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: Pkts64OctetsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGMRPFailedRegistrationsValue,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif
