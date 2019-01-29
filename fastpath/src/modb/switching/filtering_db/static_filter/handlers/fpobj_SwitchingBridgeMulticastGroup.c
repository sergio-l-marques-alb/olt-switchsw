
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_SwitchingBridgeMulticastGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  02 February 2009, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingBridgeMulticastGroup_obj.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_util_api.h"
#include "usmdb_filter_api.h"
#include "usmdb_mib_vlan_api.h"
#include "osapi_support.h"
#include "usmdb_dot1q_api.h"
#include "filter_api.h"

L7_RC_t usmWebMfdbMacGetNext (L7_uint32 UnitIndex, L7_uint32 VlanID, L7_uchar8 *macAddr, L7_uchar8 *nextMacAddr);

L7_RC_t usmWebMfdbMacGetNext (L7_uint32 UnitIndex, L7_uint32 VlanID, L7_uchar8 *macAddr, L7_uchar8 *nextMacAddr)
{
	usmdbMfdbUserInfo_t compInfo;
	xLibS8_t mfdbVidMac[L7_MFDB_KEY_SIZE];	
	xLibS8_t oldmfdbVidMac[L7_MFDB_KEY_SIZE];	
	xLibS8_t entryMac[L7_MAC_ADDR_LEN];
	xLibU32_t nextProt=0, entryVlan;
	L7_RC_t rc;
	
	memset(mfdbVidMac, 0x00, sizeof(mfdbVidMac));
	
	if (usmDbEntryVidMacCombine(VlanID, macAddr, mfdbVidMac) != L7_SUCCESS)
	  return L7_FAILURE;

  memcpy(oldmfdbVidMac, mfdbVidMac, L7_MFDB_KEY_SIZE);	
	
	rc = usmDbMfdbEntryGetNext(UnitIndex, mfdbVidMac, &nextProt, &compInfo);
  while (rc == L7_SUCCESS)
  {
    if (memcmp(oldmfdbVidMac, mfdbVidMac, L7_MFDB_KEY_SIZE) == 0)
    {
      break;
    }

    /* Get next mfdb entry */
    memset(entryMac, 0x00, L7_MAC_ADDR_LEN);
    if (usmDbEntryVidMacSeparate(mfdbVidMac, &entryVlan, entryMac) == L7_SUCCESS)
    {
      if (osapiHtons(entryVlan) == VlanID)
      {
        memcpy(nextMacAddr, entryMac, L7_MAC_ADDR_LEN);
				return L7_SUCCESS;
      }
    }
    rc = usmDbMfdbEntryGetNext(UnitIndex,mfdbVidMac, &nextProt, &compInfo);
	}
	
	return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_VlanIndex
*
* @purpose Get 'VlanIndex'
 *@description  [VlanIndex] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_VlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (objVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);    
		nextObjVlanIndexValue = L7_DOT1Q_DEFAULT_VLAN;
		owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);    
		owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, objVlanIndexValue, &nextObjVlanIndexValue);
  }	

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue,
                           sizeof (nextObjVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_VlanType
*
* @purpose Get 'VlanType'
 *@description  [VlanType] Determines the VLAN type
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_VlanType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVlanTypeValue;

  xLibU32_t keyVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbVlanTypeGet (L7_UNIT_CURRENT, keyVlanIndexValue, &objVlanTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objVlanTypeValue, sizeof (objVlanTypeValue));

  /* return the object value: VlanType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanTypeValue, sizeof (objVlanTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_MacAddress
*
* @purpose Get 'MacAddress'
 *@description  [MacAddress] The static multicast MAC address   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_MacAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr6_t objMacAddressValue;
  xLibStr6_t nextObjMacAddressValue;

  xLibU32_t keyVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

	memset(objMacAddressValue, 0x00, sizeof(objMacAddressValue));
  memset(nextObjMacAddressValue, 0x00, sizeof(nextObjMacAddressValue));

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (objMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) objMacAddressValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);    
	  memset(objMacAddressValue, 0x00, sizeof(objMacAddressValue));
  }

  memset(nextObjMacAddressValue, 0x00, sizeof(nextObjMacAddressValue));
  owa.l7rc = usmWebMfdbMacGetNext (L7_UNIT_CURRENT, keyVlanIndexValue,
                                    objMacAddressValue, nextObjMacAddressValue);	

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjMacAddressValue, owa.len);

  /* return the object value: MacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjMacAddressValue,
                           sizeof (nextObjMacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_MulticastAddress
*
* @purpose Get 'MulticastAddress'
 *@description  [MulticastAddress] Multicast IP Address range display
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_MulticastAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMulticastAddressValue;

  xLibStr6_t keyMacAddressValue;
  xLibU32_t  keyVlanIndexValue;

  usmdbMfdbUserInfo_t compInfo;
  xLibS8_t mfdbVidMac[L7_MFDB_KEY_SIZE];
  xLibS8_t oldmfdbVidMac[L7_MFDB_KEY_SIZE];
  xLibS8_t ch;
  xLibU32_t nextProt=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (keyMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) keyMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, owa.len);

  /* get the value from application */
  memset(mfdbVidMac, 0x00, sizeof(mfdbVidMac));

  if (usmDbEntryVidMacCombine(keyVlanIndexValue, keyMacAddressValue, oldmfdbVidMac) != L7_SUCCESS)
    return L7_FAILURE;

  owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
  while (owa.l7rc == L7_SUCCESS)
  {
    if (memcmp(oldmfdbVidMac, mfdbVidMac, L7_MFDB_KEY_SIZE) == 0)
    {
      break;
    }

    owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
  }

  if (owa.l7rc == L7_SUCCESS)
  {
    if (compInfo.usmdbMfdbType == L7_MFDB_TYPE_STATIC)
      ch = 's';
    else
      ch = 'd';
  
    osapiSnprintf(objMulticastAddressValue, sizeof(objMulticastAddressValue),"%s (%c)",filterMulticastMacToIpFormat(keyMacAddressValue), ch);
  }
  else 
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objMulticastAddressValue, strlen (objMulticastAddressValue));

  /* return the object value: MulticastAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMulticastAddressValue,
                           strlen (objMulticastAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingBridgeMulticastGroup_MulticastAddress
*
* @purpose Set 'MulticastAddress'
 *@description  [MulticastAddress] Multicast IP Address range display
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingBridgeMulticastGroup_MulticastAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMulticastAddressValue;

  xLibStr6_t keyMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastAddress */
  owa.len = sizeof (objMulticastAddressValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMulticastAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMulticastAddressValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (keyMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) keyMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, owa.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS; 

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_ForwardingMask
*
* @purpose Get 'ForwardingMask'
 *@description  [ForwardingMask] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_ForwardingMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_INTF_MASK_t objForwardingMaskValue;

  xLibU32_t keyVlanIndexValue;
  xLibStr6_t keyMacAddressValue;

	xLibS8_t mfdbVidMac[L7_MFDB_KEY_SIZE];
	xLibS8_t currMfdbVidMac[L7_MFDB_KEY_SIZE];	
	xLibU32_t nextProt=0;
	usmdbMfdbUserInfo_t compInfo;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (keyMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) keyMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, owa.len);

	/* get the value from application */
	
	(void)usmDbEntryVidMacCombine(keyVlanIndexValue, keyMacAddressValue, currMfdbVidMac);

  memset(mfdbVidMac, 0x00, sizeof(mfdbVidMac));	
	owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
	
	do
	{
		if (memcmp(currMfdbVidMac, mfdbVidMac, L7_MFDB_KEY_SIZE) == 0)
		{
			break;
		}
		owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
	}while (owa.l7rc == L7_SUCCESS);
	
	memset(&objForwardingMaskValue, 0x00, sizeof(objForwardingMaskValue));
		
	objForwardingMaskValue = compInfo.usmdbMfdbFwdMask;
		  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objForwardingMaskValue, strlen (objForwardingMaskValue));

  /* return the object value: ForwardingMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objForwardingMaskValue,
                           sizeof (objForwardingMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingBridgeMulticastGroup_ForwardingMask
*
* @purpose List 'ForwardingMask'
 *@description  [ForwardingMask] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingBridgeMulticastGroup_ForwardingMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
 	L7_INTF_MASK_t interfaceMask;
	xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
	
	memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (interfaceMask);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_ForwardingMask,
                          (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);    		
		owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextObjInterfaceValue);
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
    FPOBJ_TRACE_CURRENT_KEY (bufp, &interfaceMask, owa.len);    
		owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

	memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  L7_INTF_SETMASKBIT(interfaceMask, nextObjInterfaceValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));

  /* return the object value: ForwardingMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingBridgeMulticastGroup_ForwardingMask
*
* @purpose Set 'ForwardingMask'
 *@description  [ForwardingMask] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingBridgeMulticastGroup_ForwardingMask (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  FPOBJ_TRACE_ENTER (bufp);
 
  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_FilteringMask
*
* @purpose Get 'FilteringMask'
 *@description  [FilteringMask] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_FilteringMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_INTF_MASK_t objFilteringMaskValue;

  xLibU32_t keyVlanIndexValue;
  xLibStr6_t keyMacAddressValue;

	xLibS8_t mfdbVidMac[L7_MFDB_KEY_SIZE];
	xLibS8_t currMfdbVidMac[L7_MFDB_KEY_SIZE];	
	xLibU32_t nextProt=0;
	usmdbMfdbUserInfo_t compInfo;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (keyMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) keyMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, owa.len);

  /* get the value from application */
  (void)usmDbEntryVidMacCombine(keyVlanIndexValue, keyMacAddressValue, currMfdbVidMac);
	
  memset(mfdbVidMac, 0x00, sizeof(mfdbVidMac));	
	owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
	
	do
	{
		if (memcmp(currMfdbVidMac, mfdbVidMac, L7_MFDB_KEY_SIZE) == 0)
		{
			break;
		}
		owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
	}while (owa.l7rc == L7_SUCCESS);
	
	memset(&objFilteringMaskValue, 0x00, sizeof(objFilteringMaskValue));
		
	objFilteringMaskValue = compInfo.usmdbMfdbFltMask;
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objFilteringMaskValue, sizeof (objFilteringMaskValue));

  /* return the object value: FilteringMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objFilteringMaskValue, sizeof (objFilteringMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingBridgeMulticastGroup_FilteringMask
*
* @purpose List 'FilteringMask'
 *@description  [FilteringMask] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingBridgeMulticastGroup_FilteringMask (void *wap, void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
 	L7_INTF_MASK_t interfaceMask;
	xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
	
	memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (interfaceMask);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_FilteringMask,
                          (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);    
		owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextObjInterfaceValue);
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
    FPOBJ_TRACE_CURRENT_KEY (bufp, &interfaceMask, owa.len);    
		owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

	memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  L7_INTF_SETMASKBIT(interfaceMask, nextObjInterfaceValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));

  /* return the object value: ForwardingMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingBridgeMulticastGroup_FilteringMask
*
* @purpose Set 'FilteringMask'
 *@description  [FilteringMask] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingBridgeMulticastGroup_FilteringMask (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = L7_SUCCESS;
															
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingBridgeMulticastGroup_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] This object is used for creation or deletion of a
* static multicast MAC address group.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingBridgeMulticastGroup_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibU32_t keyVlanIndexValue;
  xLibStr6_t keyMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (keyMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) keyMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, owa.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingBridgeMulticastGroup_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] This object is used for creation or deletion of a
* static multicast MAC address group.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingBridgeMulticastGroup_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibU32_t keyVlanIndexValue;
  xLibStr6_t keyMacAddressValue;

  L7_INTF_MASK_t keyForwardingMaskValue;
  L7_INTF_MASK_t keyFilteringMaskValue;	
	L7_enetMacAddr_t macAddr;

  xLibS8_t mfdbVidMac[L7_MFDB_KEY_SIZE];
  xLibS8_t currMfdbVidMac[L7_MFDB_KEY_SIZE];
  xLibU32_t nextProt=0;
  usmdbMfdbUserInfo_t compInfo;
  xLibBool_t found = XLIB_FALSE;
	
	memset(&keyForwardingMaskValue, 0x00, sizeof(L7_INTF_MASK_t));
	memset(&keyFilteringMaskValue, 0x00, sizeof(L7_INTF_MASK_t));
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* retrieve key: MacAddress */
  owa.len = sizeof (keyMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_MacAddress,
                          (xLibU8_t *) keyMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
	/* retrieve key: ForwardingMask*/
  owa.len = sizeof (L7_INTF_MASK_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_ForwardingMask,
                          (xLibU8_t *) &keyForwardingMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
	/* retrieve key: FilteringMask*/
  owa.len = sizeof (L7_INTF_MASK_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingBridgeMulticastGroup_FilteringMask,
                          (xLibU8_t *) &keyFilteringMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
		
  memcpy(macAddr.addr, keyMacAddressValue, sizeof(L7_enetMacAddr_t));
	
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, owa.len);

  owa.l7rc = L7_SUCCESS;

	if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    (void)usmDbEntryVidMacCombine(keyVlanIndexValue, keyMacAddressValue, currMfdbVidMac);

    memset(mfdbVidMac, 0x00, sizeof(mfdbVidMac));
    owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);

    do
    {
      if (memcmp(currMfdbVidMac, mfdbVidMac, L7_MFDB_KEY_SIZE) == 0)
      {
        found = XLIB_TRUE;
        break;
      }
      owa.l7rc = usmDbMfdbEntryGetNext(L7_UNIT_CURRENT, mfdbVidMac, &nextProt, &compInfo);
    }while (owa.l7rc == L7_SUCCESS);

    if (found == XLIB_TRUE) /* Delete current and create new entry */
    {
      if (usmDbFilterRemove (L7_UNIT_CURRENT, keyMacAddressValue, keyVlanIndexValue) == L7_SUCCESS)
      {
        owa.l7rc = usmdbFilterDstIntfMaskAdd (L7_UNIT_CURRENT, keyVlanIndexValue, macAddr, keyForwardingMaskValue, keyFilteringMaskValue);
      }
    }
    else /* Create new entry */
    {
      owa.l7rc = usmdbFilterDstIntfMaskAdd (L7_UNIT_CURRENT, keyVlanIndexValue, macAddr, keyForwardingMaskValue, keyFilteringMaskValue);
    }
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbFilterRemove (L7_UNIT_CURRENT, keyMacAddressValue, keyVlanIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
