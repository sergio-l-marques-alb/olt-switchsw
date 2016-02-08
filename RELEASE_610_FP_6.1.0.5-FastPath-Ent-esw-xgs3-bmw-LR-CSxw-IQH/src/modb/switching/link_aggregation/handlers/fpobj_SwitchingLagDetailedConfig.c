
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingLagDetailedConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  16 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingLagDetailedConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"

L7_RC_t
fpObjUtilLagIndexGetNext(L7_uint32 UnitIndex, L7_uint32 *index)
{
  L7_uint32 tmpIntIfNum = 0;
  L7_uint32 tmpExtIfNum = 0;

  /* iterate through interface numbers */
  while (usmDbDot3adAggEntryGetNext(UnitIndex, tmpIntIfNum, &tmpIntIfNum) == L7_SUCCESS)
  {
    /* convert internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(tmpIntIfNum, &tmpExtIfNum) == L7_SUCCESS)
    {
      /* if really the next ext interface number */
      if (tmpExtIfNum > *index)
      {
        *index = tmpExtIfNum;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t
fpObjUtilLagMemberIfIndexGet(L7_uint32 UnitIndex, L7_uint32 lagIndex, L7_uint32 ifIndex)
{
  L7_uint32 lagIntIfNum, memberIntIfNum;

  if ((usmDbIntIfNumFromExtIfNum(lagIndex, &lagIntIfNum) == L7_SUCCESS) &&
      (usmDbIntIfNumFromExtIfNum(ifIndex, &memberIntIfNum) == L7_SUCCESS) &&
      (usmDbDot3adMemberCheck(UnitIndex, lagIntIfNum, memberIntIfNum) == L7_SUCCESS))
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
fpObjUtilLagMemberIfIndexGetNext(L7_uint32 UnitIndex, L7_uint32 lagIndex, L7_uint32 *ifIndex)
{
  while (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS)
  {
    if (fpObjUtilLagMemberIfIndexGet(UnitIndex, lagIndex, *ifIndex) == L7_SUCCESS)
      return L7_SUCCESS;
  }
  return L7_FAILURE;
}


L7_RC_t
fpObjUtilLagDetailedConfigEntryNext ( L7_uint32 UnitIndex, L7_uint32 *lagIndex, L7_uint32 *ifIndex)
{
  /* try to get the next member for this lag*/
  if (fpObjUtilLagMemberIfIndexGetNext(UnitIndex, *lagIndex, ifIndex) == L7_SUCCESS)
    return L7_SUCCESS;

  /* If there are no more members in the current lag loop through available lags */
  while (fpObjUtilLagIndexGetNext(UnitIndex, lagIndex) == L7_SUCCESS)
  {
    *ifIndex = 0;
    if (fpObjUtilLagMemberIfIndexGetNext(UnitIndex, *lagIndex, ifIndex) == L7_SUCCESS)
      return L7_SUCCESS;
  }

  /* no more lags */
  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLagDetailedConfig_LagIndex
*
* @purpose Get 'LagIndex'
 *@description  [LagIndex] The LAG Id   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLagDetailedConfig_LagIndex (void *wap, void *bufp)
{

  xLibU32_t objLagIndexValue;
  xLibU32_t nextObjLagIndexValue;
  xLibU32_t objMemberIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LagIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_LagIndex,
                          (xLibU8_t *) & objLagIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjLagIndexValue, 0, sizeof (nextObjLagIndexValue));
    memset (&objMemberIndexValue, 0, sizeof (objMemberIndexValue));
    owa.l7rc = fpObjUtilLagDetailedConfigEntryNext(L7_UNIT_CURRENT, &nextObjLagIndexValue, &objMemberIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLagIndexValue, owa.len);
    memset (&objMemberIndexValue, 0, sizeof (objMemberIndexValue));
    nextObjLagIndexValue = objLagIndexValue;
    do
    {
      owa.l7rc = fpObjUtilLagDetailedConfigEntryNext(L7_UNIT_CURRENT, &nextObjLagIndexValue, &objMemberIndexValue);
    }
    while ((objLagIndexValue == nextObjLagIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLagIndexValue, owa.len);

  /* return the object value: LagIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLagIndexValue, sizeof (objLagIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingLagDetailedConfig_MemberIndex
*
* @purpose Get 'MemberIndex'
 *@description  [MemberIndex] Member Interface Id   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLagDetailedConfig_MemberIndex (void *wap, void *bufp)
{

  xLibU32_t objLagIndexValue;
  xLibU32_t nextObjLagIndexValue;
  xLibU32_t objMemberIndexValue;
  xLibU32_t nextObjMemberIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LagIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_LagIndex,
                          (xLibU8_t *) & objLagIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLagIndexValue, owa.len);

  /* retrieve key: MemberIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_MemberIndex,
                          (xLibU8_t *) & objMemberIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    nextObjLagIndexValue = objLagIndexValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjMemberIndexValue, 0, sizeof (nextObjMemberIndexValue));

    owa.l7rc = fpObjUtilLagDetailedConfigEntryNext(L7_UNIT_CURRENT,
                                     &nextObjLagIndexValue,
                                     &nextObjMemberIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMemberIndexValue, owa.len);
    nextObjLagIndexValue = objLagIndexValue;
    owa.l7rc = fpObjUtilLagDetailedConfigEntryNext(L7_UNIT_CURRENT,
                                        &nextObjLagIndexValue,
                                    &objMemberIndexValue);

    nextObjMemberIndexValue = objMemberIndexValue;
  }

  if ((objLagIndexValue != nextObjLagIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMemberIndexValue, owa.len);

  /* return the object value: MemberIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMemberIndexValue,
                           sizeof (objMemberIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingLagDetailedConfig_LagMemberPortSpeed
*
* @purpose Get 'LagMemberPortSpeed'
 *@description  [LagMemberPortSpeed] LAG port speed   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLagDetailedConfig_LagMemberPortSpeed (void *wap, void *bufp)
{

  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLagIndexValue;
  fpObjWa_t kwaMemberIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMemberIndexValue;
  L7_uint32 ifSpeed;
  L7_uint32 ifConnectorType;
  L7_RC_t rc;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLagMemberPortSpeedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LagIndex */
  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_LagIndex,
                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);
  if (kwaLagIndex.rc != XLIBRC_SUCCESS)
  {
    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);
    return kwaLagIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);

  /* retrieve key: MemberIndex */
  kwaMemberIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_MemberIndex,
                                     (xLibU8_t *) & keyMemberIndexValue, &kwaMemberIndex.len);
  if (kwaMemberIndex.rc != XLIBRC_SUCCESS)
  {
    kwaMemberIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMemberIndex);
    return kwaMemberIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMemberIndexValue, kwaMemberIndex.len);

  memset(objLagMemberPortSpeedValue, 0x0, sizeof(objLagMemberPortSpeedValue));

  /* get the value from application */
  owa.l7rc = usmDbIfSpeedGet(L7_UNIT_CURRENT, keyMemberIndexValue, &ifSpeed);

  if (owa.l7rc == L7_SUCCESS)
  {
    switch (ifSpeed)
    {
    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      strcpy(objLagMemberPortSpeedValue, "dot3MauType10BaseTHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      strcpy(objLagMemberPortSpeedValue, "dot3MauType10BaseTFD");
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      strcpy(objLagMemberPortSpeedValue, "dot3MauType100BaseTXHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      strcpy(objLagMemberPortSpeedValue, "dot3MauType100BaseTXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      strcpy(objLagMemberPortSpeedValue, "dot3MauType100BaseFXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      rc = usmDbIntfConnectorTypeGet(keyMemberIndexValue, &ifConnectorType);

      if (rc == L7_SUCCESS)
      {
        switch (ifConnectorType)
        {
        case L7_MTRJ:
          strcpy(objLagMemberPortSpeedValue, "dot3MauType1000BaseSXFD");
          break;

        case L7_RJ45:
          strcpy(objLagMemberPortSpeedValue, "dot3MauType1000BaseTFD");
          break;

        default:
          strcpy(objLagMemberPortSpeedValue, "dot3MauType1000BaseXFD");
          break;
        }
      }
      else
      {
        /* if connector type is unknown, return the default */
        strcpy(objLagMemberPortSpeedValue, "dot3MauType1000BaseXFD");
        rc = L7_SUCCESS;
      }

      break;

    case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
      rc = usmDbIntfConnectorTypeGet(keyMemberIndexValue, &ifConnectorType);

      if (rc == L7_SUCCESS)
      {
        switch (ifConnectorType)
        {
        case L7_MTRJ:
          strcpy(objLagMemberPortSpeedValue, "dot3MauType2500BaseSXFD");
          break;

        case L7_RJ45:
          strcpy(objLagMemberPortSpeedValue, "dot3MauType2500BaseTFD");
          break;

        default:
          strcpy(objLagMemberPortSpeedValue, "dot3MauType2500BaseXFD");
          break;
        }
      }
      else
      {
        /* if connector type is unknown, return the default */
        strcpy(objLagMemberPortSpeedValue, "dot3MauType2500BaseXFD");
        rc = L7_SUCCESS;
      }
      break;

     case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      strcpy(objLagMemberPortSpeedValue, "dot3MauType10GigBaseX");
      break;

    default:
      strcpy(objLagMemberPortSpeedValue, "0.0");
      break;
    }
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LagMemberPortSpeed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLagMemberPortSpeedValue,
                           strlen (objLagMemberPortSpeedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingLagDetailedConfig_LagMemberPortStatus
*
* @purpose Get 'LagMemberPortStatus'
 *@description  [LagMemberPortStatus] LAG port status.active(1) - Actively
* participating in the LAG.inactive(2) - Not participating in the LAG.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLagDetailedConfig_LagMemberPortStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLagIndexValue;
  fpObjWa_t kwaMemberIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMemberIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLagMemberPortStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LagIndex */
  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_LagIndex,
                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);
  if (kwaLagIndex.rc != XLIBRC_SUCCESS)
  {
    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);
    return kwaLagIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);

  /* retrieve key: MemberIndex */
  kwaMemberIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagDetailedConfig_MemberIndex,
                                     (xLibU8_t *) & keyMemberIndexValue, &kwaMemberIndex.len);
  if (kwaMemberIndex.rc != XLIBRC_SUCCESS)
  {
    kwaMemberIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMemberIndex);
    return kwaMemberIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMemberIndexValue, kwaMemberIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adIsActiveMember(L7_UNIT_CURRENT, keyMemberIndexValue);

  if (owa.l7rc == L7_SUCCESS)
     objLagMemberPortStatusValue = L7_TRUE;
  else
     objLagMemberPortStatusValue = L7_FALSE;

  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LagMemberPortStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagMemberPortStatusValue,
                           sizeof (objLagMemberPortStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
