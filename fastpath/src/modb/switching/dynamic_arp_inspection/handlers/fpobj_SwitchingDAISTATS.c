
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDAISTATS.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  18 May 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDAISTATS_obj.h"
#include "usmdb_dai_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_VlanIndex
*
* @purpose Get 'VlanIndex'
 *@description  [VlanIndex] The VLAN-ID or other identifier refering to this
* VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_VlanIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = 0;
    owa.l7rc = usmDbDaiVlanNextGet(objVlanIndexValue, &nextObjVlanIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    owa.l7rc = usmDbDaiVlanNextGet(objVlanIndexValue, &nextObjVlanIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue, sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_Forwarded
*
* @purpose Get 'Forwarded'
 *@description  [Forwarded] Number of packets forwarded through DAI   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_Forwarded (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objForwardedValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objForwardedValue = stats.forwarded;
  /* return the object value: Forwarded */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objForwardedValue, sizeof (objForwardedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_Dropped
*
* @purpose Get 'Dropped'
 *@description  [Dropped] Number of packets dropped through DAI   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_Dropped (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDroppedValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDroppedValue = stats.dropped;
  /* return the object value: Dropped */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDroppedValue, sizeof (objDroppedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_dhcpDrops
*
* @purpose Get 'dhcpDrops'
 *@description  [dhcpDrops] Number of packets dropped through DHCP snooping   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_dhcpDrops (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdhcpDropsValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objdhcpDropsValue = stats.dhcpDrops;
  /* return the object value: dhcpDrops */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdhcpDropsValue, sizeof (objdhcpDropsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_dhcpPermits
*
* @purpose Get 'dhcpPermits'
 *@description  [dhcpPermits] Number of packets permited through DHCP snooping   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_dhcpPermits (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdhcpPermitsValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objdhcpPermitsValue = stats.dhcpPermits;

  /* return the object value: dhcpPermits */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdhcpPermitsValue, sizeof (objdhcpPermitsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_aclDrops
*
* @purpose Get 'aclDrops'
 *@description  [aclDrops] Number of packets dropped through DAI acl   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_aclDrops (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclDropsValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objaclDropsValue = stats.aclDrops;
  /* return the object value: aclDrops */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclDropsValue, sizeof (objaclDropsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_aclPermits
*
* @purpose Get 'aclPermits'
 *@description  [aclPermits] Number of packets permitted through DAI acl   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_aclPermits (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclPermitsValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objaclPermitsValue = stats.aclPermits;
  /* return the object value: aclPermits */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclPermitsValue, sizeof (objaclPermitsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_sMacFailures
*
* @purpose Get 'sMacFailures'
 *@description  [sMacFailures] Number of packets failured  through DAI
* sMacFailure   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_sMacFailures (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsMacFailuresValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objsMacFailuresValue = stats.sMacFailures; 
  /* return the object value: sMacFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsMacFailuresValue,
                           sizeof (objsMacFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_dMacFailures
*
* @purpose Get 'dMacFailures'
 *@description  [dMacFailures] Number of packets dropped through DAI dMacFailures   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_dMacFailures (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdMacFailuresValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objdMacFailuresValue = stats.dMacFailures;

  /* return the object value: dMacFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdMacFailuresValue,
                           sizeof (objdMacFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDAISTATS_ipValidFailures
*
* @purpose Get 'ipValidFailures'
 *@description  [ipValidFailures] Number of packets dropped through DAI
* ipValidFailures   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAISTATS_ipValidFailures (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipValidFailuresValue;
  daiVlanStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDAISTATS_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanStatsGet(keyVlanIndexValue, &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objipValidFailuresValue = stats.ipValidFailures;
  /* return the object value: ipValidFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipValidFailuresValue,
                           sizeof (objipValidFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
