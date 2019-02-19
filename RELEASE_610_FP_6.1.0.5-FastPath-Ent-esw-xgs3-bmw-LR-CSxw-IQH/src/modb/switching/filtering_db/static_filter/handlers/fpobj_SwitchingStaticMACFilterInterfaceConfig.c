
/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 ********************************************************************************
 *
 * @filename fpobj_SwitchingStaticMACFilterInterfaceConfig.c
 *
 * @purpose  
 *
 * @component object handlers
 *
 * @comments  Refer to Switching-object.xml
 *
 * @create  20 November 2008, Thursday
 *
 * @notes   This file is auto generated and should be used as starting point to
 *          develop the object handlers
 *
 * @author  Rama Sasthri, Kristipati
 * @end
 *
 ********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingStaticMACFilterInterfaceConfig_obj.h"
#include "usmdb_filter_api.h"
#include "filter_exports.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_util_api.h"
#include "dot1q_api.h"

extern L7_RC_t usmDbNextVlanGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *nextvid);

/*******************************************************************************
 * @function fpObjGet_SwitchingStaticMACFilterInterfaceConfig_VidMac
 *
 * @purpose Get 'VidMac'
 *@description  [VidMac] Displays MAC Address with VLAN ID as the prefix which
 * is an entry in filter database.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterInterfaceConfig_VidMac (void *wap, void *bufp)
{
  L7_uchar8 vidMac[L7_VID_MAC_ADDR_LEN];
  xLibU16_t vidMacLen = sizeof (vidMac);
  L7_uchar8 mac[L7_ENET_MAC_ADDR_LEN];
  L7_uint32 vlanid;
  L7_uchar8 nextmac[L7_ENET_MAC_ADDR_LEN];
  L7_uint32 nextvlanid;
  L7_ushort16 vid;

  memset (mac, 0, sizeof (mac));
  memset (&vlanid,0,sizeof(vlanid));
  memset (nextmac,0,sizeof(nextmac));
  memset (&nextvlanid,0,sizeof(nextvlanid));
  memset (&vid,0,sizeof(vid));
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac, vidMac, &vidMacLen))
  {
    if (usmDbFilterFirstGet (L7_UNIT_CURRENT, mac,&vlanid) == L7_SUCCESS)
    {
      usmDbEntryVidMacCombine(vlanid,mac,vidMac);
      xLibBufDataSet (bufp, (xLibU8_t *) vidMac, sizeof (vidMac));
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }
  else
  {
    memcpy(&vid,vidMac,2);
    mac[0]=vidMac[2];
    mac[1]=vidMac[3];
    mac[2]=vidMac[4];
    mac[3]=vidMac[5];
    mac[4]=vidMac[6];
    mac[5]=vidMac[7];
    if (usmDbFilterNextGet (L7_UNIT_CURRENT,mac,vid,nextmac,&nextvlanid) == L7_SUCCESS)
    {
      usmDbEntryVidMacCombine(nextvlanid,nextmac,vidMac);
      xLibBufDataSet (bufp, (xLibU8_t *) vidMac, sizeof (vidMac));
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }
  return XLIBRC_SUCCESS;

}

/*******************************************************************************
 * @function fpObjGet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask
 *
 * @purpose Get 'StaticMacFilteringSourcePortMask'
 *@description  [StaticMacFilteringSourcePortMask] The Static MAC Filter source
 * port mask.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t keyVidMacValue;
  L7_uchar8 mac[L7_ENET_MAC_ADDR_LEN];
  xLibU16_t  vlanId = 0;
  L7_INTF_MASK_t nextObjStaticMacFilteringSourcePortMaskValue;
  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  xLibU32_t numIntf;
  xLibU32_t intfValue;
  xLibU32_t i = 0;


  FPOBJ_TRACE_ENTER (bufp);

  memset(&nextObjStaticMacFilteringSourcePortMaskValue,0x0,sizeof(nextObjStaticMacFilteringSourcePortMaskValue)); 
  memset(keyVidMacValue,0x0,sizeof(keyVidMacValue)); 
  memset(mac,0x0,sizeof(mac)); 
  /* retrieve key: VidMac */
  owa.len = sizeof (keyVidMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac,
      (xLibU8_t *) keyVidMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVidMacValue, owa.len);
  memcpy(&vlanId,keyVidMacValue,2);
  memcpy(mac,&keyVidMacValue[2],sizeof(mac));

  /* get the value from application */

  owa.l7rc = usmDbFilterSrcIntfListGet(L7_UNIT_CURRENT,mac,(xLibU32_t)vlanId,&numIntf,intfList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  i = 0;
  while(numIntf != 0)
  {
    if(numIntf >= 1)
    {
      intfValue = intfList[i];
      L7_INTF_SETMASKBIT(nextObjStaticMacFilteringSourcePortMaskValue, intfValue);
    }
    i++;
    numIntf--;
  }

  FPOBJ_TRACE_VALUE (bufp, nextObjStaticMacFilteringSourcePortMaskValue,
      strlen (nextObjStaticMacFilteringSourcePortMaskValue));

  /* return the object value: StaticMacFilteringSourcePortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjStaticMacFilteringSourcePortMaskValue,
      sizeof (nextObjStaticMacFilteringSourcePortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjList_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask
 *
 * @purpose List 'StaticMacFilteringSourcePortMask'
 *@description  [StaticMacFilteringSourcePortMask] The Static MAC Filter source
 * port mask.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask,
      (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjInterfaceValue);
  }
  else
  {
    objInterfaceValue = fpObjPortFromMaskGet(interfaceMask);
    if(objInterfaceValue == -1)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue);

  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  L7_INTF_SETMASKBIT(interfaceMask,nextObjInterfaceValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
      sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
 * @function fpObjSet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask
 *
 * @purpose Set 'StaticMacFilteringSourcePortMask'
 *@description  [StaticMacFilteringSourcePortMask] The Static MAC Filter source
 * port mask.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringSourcePortMask (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  L7_INTF_MASK_t intfMask;
  xLibU16_t vlanId = 0;
  xLibStr256_t keyVidMacValue;
  xLibStr6_t mac;

  L7_uint32 numIntf, intIfNum, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;

  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
  L7_BOOL errorFound;
  xLibRC_t rc;

  errorFound = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&intfMask,0x0,sizeof(intfMask));
  memset(keyVidMacValue,0x0,sizeof(keyVidMacValue));
  memset(mac,0x0,sizeof(mac));

  /* retrieve object: StaticMacFilteringSourcePortMask */
  owa.len = sizeof (intfMask);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, intfMask, owa.len);

  /* retrieve key: VidMac */
  owa.len = sizeof (keyVidMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac,
      (xLibU8_t *) keyVidMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVidMacValue, owa.len);
  memcpy(&vlanId,keyVidMacValue,2);
  memcpy(mac,&keyVidMacValue[2],sizeof(mac));
  /* set the value in application */
  if(usmDbConvertMaskToList(&intfMask,intfList,&intfCount) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(intfListPrev, 0, sizeof(intfListPrev));
  memset(intfListToRemove, 0, sizeof(intfListToRemove));
  memset(intfListToAdd, 0, sizeof(intfListToAdd));

  /* Get the list of interfaces already configured with the group */
  intIfNum = 0;
  numIntfPrev = 0;

  rc = usmDbFilterSrcIntfListGet(L7_UNIT_CURRENT,mac,(xLibU32_t)vlanId,
      &numIntfPrev, intfListPrev);

  numIntf = intfCount;
  if (numIntf > 0)
  {
    /* Figure out which ports to remove */
    numIntfToRemove = 0;
    for (i=0; i<numIntfPrev; i++)
    {
      intfFound = L7_FALSE;
      for (j=1; j<=numIntf; j++)
      {
        if (intfListPrev[i] == intfList[j])
        {
          intfFound = L7_TRUE;
          break;
        }
      }
      if (intfFound == L7_FALSE)
      {
        intfListToRemove[numIntfToRemove++] = intfListPrev[i];
      }
    }

    /* Figure out which ports to add */
    numIntfToAdd = 0;
    for (i=1; i<=numIntf; i++)
    {
      intfFound = L7_FALSE;
      for (j=0; j<numIntfPrev; j++)
      {
        if (intfList[i] == intfListPrev[j])
        {
          intfFound = L7_TRUE;
          break;
        }
      }
      if (intfFound == L7_FALSE)
      {
        intfListToAdd[numIntfToAdd++] = intfList[i];
      }
    }
    errorFound = L7_FALSE;

    for (i=0; i<numIntfToAdd; i++)
    {
      rc = usmdbFilterSrcIntfAdd(L7_UNIT_CURRENT, mac,(xLibU32_t)vlanId, intfListToAdd[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }

    for (i=0; i<numIntfToRemove; i++)
    {
      rc = usmdbFilterSrcIntfDelete(L7_UNIT_CURRENT,  mac,
          (xLibU32_t)vlanId, intfListToRemove[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
        break;
      }
    }
  }
  else    /* Remove all configured Interfaces */
  {
    numIntfToRemove = 0;
    errorFound = L7_FALSE;

    for (i=0; i<numIntfPrev; i++)
    {
      rc = usmdbFilterSrcIntfDelete(L7_UNIT_CURRENT,mac,
          (xLibU32_t)vlanId, intfListPrev[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
  }
  if(errorFound == L7_TRUE )
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
 * @function fpObjGet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask
 *
 * @purpose Get 'StaticMacFilteringDestPortMask'
 *@description  [StaticMacFilteringDestPortMask] The Static MAC Filter
 * destination port mask.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask (void *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t keyVidMacValue;
  L7_INTF_MASK_t nextObjStaticMacFilteringDestPortMaskValue;
  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  xLibU32_t numIntf;
  xLibS32_t intfValue;
  xLibU32_t i = 0;
  xLibU16_t vlanId = 0;
  xLibStr6_t mac;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VidMac */
  owa.len = sizeof (keyVidMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac,
      (xLibU8_t *) keyVidMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(mac,0x0,sizeof(mac)); 
  memcpy(&vlanId,keyVidMacValue,2);
  memcpy(mac,&keyVidMacValue[2],sizeof(mac));

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVidMacValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbFilterDstIntfListGet(L7_UNIT_CURRENT,mac,
      (xLibU32_t)vlanId,&numIntf,intfList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(&nextObjStaticMacFilteringDestPortMaskValue,0x00, sizeof (L7_INTF_MASK_t));

  while(numIntf != 0)
  {
    if(numIntf >= 1)
    {
      intfValue = intfList[i];
      L7_INTF_SETMASKBIT(nextObjStaticMacFilteringDestPortMaskValue, intfValue);
    }
    i++;
    numIntf--;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticMacFilteringDestPortMaskValue,
      sizeof (L7_INTF_MASK_t));

  /* return the object value: StaticMacFilteringDestPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjStaticMacFilteringDestPortMaskValue,
      sizeof(nextObjStaticMacFilteringDestPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjList_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask
 *
 * @purpose List 'StaticMacFilteringDestPortMask'
 *@description  [StaticMacFilteringDestPortMask] The Static MAC Filter
 * destination port mask.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);


  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask,
      (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjInterfaceValue);
  }
  else
  {
    objInterfaceValue = fpObjPortFromMaskGet(interfaceMask);
    if(objInterfaceValue == -1)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  L7_INTF_SETMASKBIT(interfaceMask,nextObjInterfaceValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
      sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}

/*******************************************************************************
 * @function fpObjSet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask
 *
 * @purpose Set 'StaticMacFilteringDestPortMask'
 *@description  [StaticMacFilteringDestPortMask] The Static MAC Filter
 * destination port mask.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringDestPortMask (void *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  L7_INTF_MASK_t intfMask;
  xLibU16_t vlanId = 0;
  xLibStr256_t keyVidMacValue;
  xLibStr6_t mac;

  L7_uint32 numIntf, intIfNum, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;

  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
  L7_BOOL errorFound;
  xLibRC_t rc;

  errorFound = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&intfMask,0x0,sizeof(intfMask));
  memset(keyVidMacValue,0x0,sizeof(keyVidMacValue));
  memset(mac,0x0,sizeof(mac));

  /* retrieve object: StaticMacFilteringSourcePortMask */
  owa.len = sizeof (intfMask);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, intfMask, owa.len);

  /* retrieve key: VidMac */
  owa.len = sizeof (keyVidMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac,
      (xLibU8_t *) keyVidMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVidMacValue, owa.len);
  memcpy(&vlanId,keyVidMacValue,2);
  memcpy(mac,&keyVidMacValue[2],sizeof(mac));
  /* set the value in application */
  if(usmDbConvertMaskToList(&intfMask,intfList,&intfCount) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(intfListPrev, 0, sizeof(intfListPrev));
  memset(intfListToRemove, 0, sizeof(intfListToRemove));
  memset(intfListToAdd, 0, sizeof(intfListToAdd));

  /* Get the list of interfaces already configured with the group */
  intIfNum = 0;
  numIntfPrev = 0;

  rc = usmDbFilterDstIntfListGet(L7_UNIT_CURRENT,mac,(xLibU32_t)vlanId,
      &numIntfPrev, intfListPrev);

  numIntf = intfCount;
  if (numIntf > 0)
  {
    /* Figure out which ports to remove */
    numIntfToRemove = 0;
    for (i=0; i<numIntfPrev; i++)
    {
      intfFound = L7_FALSE;
      for (j=1; j<=numIntf; j++)
      {
        if (intfListPrev[i] == intfList[j])
        {
          intfFound = L7_TRUE;
          break;
        }
      }
      if (intfFound == L7_FALSE)
      {
        intfListToRemove[numIntfToRemove++] = intfListPrev[i];
      }
    }
    /* Figure out which ports to add */
    numIntfToAdd = 0;
    for (i=1; i<=numIntf; i++)
    {
      intfFound = L7_FALSE;
      for (j=0; j<numIntfPrev; j++)
      {
        if (intfList[i] == intfListPrev[j])
        {
          intfFound = L7_TRUE;
          break;
        }
      }
      if (intfFound == L7_FALSE)
      {
        intfListToAdd[numIntfToAdd++] = intfList[i];
      }
    }
    errorFound = L7_FALSE;

    for (i=0; i<numIntfToAdd; i++)
    {
      rc = usmdbFilterDstIntfAdd(L7_UNIT_CURRENT, mac,(xLibU32_t)vlanId, intfListToAdd[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
    
    for (i=0; i<numIntfToRemove; i++)
    {
      rc = usmdbFilterDstIntfDelete(L7_UNIT_CURRENT,  mac,
          (xLibU32_t)vlanId, intfListToRemove[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
        break;
      }
    }
  }
  else    /* Remove all configured Interfaces */
  {
    numIntfToRemove = 0;
    errorFound = L7_FALSE;

    for (i=0; i<numIntfPrev; i++)
    {
      rc = usmdbFilterDstIntfDelete(L7_UNIT_CURRENT,mac,
          (xLibU32_t)vlanId, intfListPrev[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
  }
  if(errorFound == L7_TRUE )
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
 * @function fpObjGet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringStatus
 *
 * @purpose Get 'StaticMacFilteringStatus'
 *@description  [StaticMacFilteringStatus] The Static MAC Filter status.
 * Supported values: Remove - removes the entry   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringStatus (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStaticMacFilteringStatusValue;

  xLibStr256_t keyVidMacValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VidMac */
  owa.len = sizeof (keyVidMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac,
      (xLibU8_t *) keyVidMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVidMacValue, owa.len);

  /* get the value from application */
  objStaticMacFilteringStatusValue = L7_ROW_STATUS_ACTIVE; 
  FPOBJ_TRACE_VALUE (bufp, &objStaticMacFilteringStatusValue,
      sizeof (objStaticMacFilteringStatusValue));

  /* return the object value: StaticMacFilteringStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticMacFilteringStatusValue,
      sizeof (objStaticMacFilteringStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringStatus
 *
 * @purpose Set 'StaticMacFilteringStatus'
 *@description  [StaticMacFilteringStatus] The Static MAC Filter status.
 * Supported values: Remove - removes the entry   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterInterfaceConfig_StaticMacFilteringStatus (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStaticMacFilteringStatusValue;
  xLibU16_t vlanId = 0;

  xLibStr256_t keyVidMacValue;
  xLibStr6_t mac;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticMacFilteringStatus */
  owa.len = sizeof (objStaticMacFilteringStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStaticMacFilteringStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticMacFilteringStatusValue, owa.len);

  /* retrieve key: VidMac */
  owa.len = sizeof (keyVidMacValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterInterfaceConfig_VidMac,
      (xLibU8_t *) keyVidMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVidMacValue, owa.len);
  
  memcpy(&vlanId,keyVidMacValue,2);
  memset(mac,0x0,sizeof(mac));
  memcpy(mac,&keyVidMacValue[2],sizeof(mac)); 
  owa.l7rc = L7_SUCCESS;
  if (objStaticMacFilteringStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  { 
    /*dont do anything on success*/
    owa.l7rc = L7_SUCCESS;  
  }
  else if (objStaticMacFilteringStatusValue == L7_ROW_STATUS_DESTROY)
  {
     /* Delete the existing row */
    owa.l7rc = usmDbFilterRemove (L7_UNIT_CURRENT,
                                  mac,
                                  (xLibU32_t)vlanId);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

