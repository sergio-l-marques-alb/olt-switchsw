
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingIPSGDynamicBindingTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  10 June 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingIPSGDynamicBindingTable_obj.h"
#include "usmdb_dhcp_snooping.h"

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIfIndex
*
* @purpose Get 'DynamicIpsgBindingIfIndex'
 *@description  [DynamicIpsgBindingIfIndex] Interface on which this IPSG binding
* is added   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIfIndex (void *wap, void *bufp)
{
  xLibU32_t objDynamicIpsgBindingIfIndexValue;
  xLibU32_t nextObjDynamicIpsgBindingIfIndexValue;

  xLibU32_t objDynamicIpsgBindingVanIdValue;
  xLibU32_t nextObjDynamicIpsgBindingVanIdValue;

  xLibStr6_t objDynamicIpsgBindingMacAddrValue;
  xLibStr6_t nextObjDynamicIpsgBindingMacAddrValue;

  xLibU32_t objDynamicIpsgBindingIpAddrValue;
  xLibU32_t nextObjDynamicIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DynamicIpsgBindingIfIndex */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIfIndex,
                          (xLibU8_t *) & objDynamicIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjDynamicIpsgBindingIfIndexValue = 0;
    nextObjDynamicIpsgBindingVanIdValue = 0;
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    memset (nextObjDynamicIpsgBindingMacAddrValue, 0, sizeof (nextObjDynamicIpsgBindingMacAddrValue));

    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                                       (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                                       &nextObjDynamicIpsgBindingIpAddrValue,
                                                                       (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                                       IPSG_ENTRY_DYNAMIC);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingIfIndexValue, owa.len);
    nextObjDynamicIpsgBindingIfIndexValue = objDynamicIpsgBindingIfIndexValue;
    nextObjDynamicIpsgBindingVanIdValue = 0;
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    memset (nextObjDynamicIpsgBindingMacAddrValue, 0, sizeof (nextObjDynamicIpsgBindingMacAddrValue));
    
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                                         (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                                         &nextObjDynamicIpsgBindingIpAddrValue,
                                                                         (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                                         IPSG_ENTRY_DYNAMIC);
                                                                       
      objDynamicIpsgBindingVanIdValue = nextObjDynamicIpsgBindingVanIdValue;
      osapiStrncpySafe (objDynamicIpsgBindingMacAddrValue, nextObjDynamicIpsgBindingMacAddrValue,
               sizeof (objDynamicIpsgBindingMacAddrValue));
      objDynamicIpsgBindingIpAddrValue = nextObjDynamicIpsgBindingIpAddrValue;
    }
    while ((objDynamicIpsgBindingIfIndexValue == nextObjDynamicIpsgBindingIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDynamicIpsgBindingIfIndexValue, owa.len);

  /* return the object value: DynamicIpsgBindingIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDynamicIpsgBindingIfIndexValue,
                           sizeof (nextObjDynamicIpsgBindingIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingVanId
*
* @purpose Get 'DynamicIpsgBindingVanId'
 *@description  [DynamicIpsgBindingVanId] VLAN to which this IPSG binding is
* going to be added   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingVanId (void *wap, void *bufp)
{

  xLibU32_t objDynamicIpsgBindingIfIndexValue;
  xLibU32_t nextObjDynamicIpsgBindingIfIndexValue;
  
  xLibU32_t objDynamicIpsgBindingVanIdValue;
  xLibU32_t nextObjDynamicIpsgBindingVanIdValue;

  xLibStr6_t objDynamicIpsgBindingMacAddrValue;
  xLibStr6_t nextObjDynamicIpsgBindingMacAddrValue;

  xLibU32_t objDynamicIpsgBindingIpAddrValue;
  xLibU32_t nextObjDynamicIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DynamicIpsgBindingIfIndex */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIfIndex,
                          (xLibU8_t *) & objDynamicIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingIfIndexValue, owa.len);

  /* retrieve key: DynamicIpsgBindingVanId */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingVanId,
                          (xLibU8_t *) & objDynamicIpsgBindingVanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjDynamicIpsgBindingIfIndexValue = objDynamicIpsgBindingIfIndexValue;
    nextObjDynamicIpsgBindingVanIdValue = 0;
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    memset (nextObjDynamicIpsgBindingMacAddrValue, 0, sizeof (nextObjDynamicIpsgBindingMacAddrValue));

    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                                       (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                                       &nextObjDynamicIpsgBindingIpAddrValue,
                                                                       (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                                       IPSG_ENTRY_DYNAMIC);
   }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingVanIdValue, owa.len);

    nextObjDynamicIpsgBindingIfIndexValue = objDynamicIpsgBindingIfIndexValue;
    nextObjDynamicIpsgBindingVanIdValue = objDynamicIpsgBindingVanIdValue;
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    memset (nextObjDynamicIpsgBindingMacAddrValue, 0, sizeof (nextObjDynamicIpsgBindingMacAddrValue));
    
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                                         (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                                         &nextObjDynamicIpsgBindingIpAddrValue,
                                                                         (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                                         IPSG_ENTRY_DYNAMIC);
                                                                       
      memcpy (objDynamicIpsgBindingMacAddrValue, nextObjDynamicIpsgBindingMacAddrValue,
               sizeof (nextObjDynamicIpsgBindingMacAddrValue));
      objDynamicIpsgBindingIpAddrValue = nextObjDynamicIpsgBindingIpAddrValue;
    }
    while ((objDynamicIpsgBindingIfIndexValue == nextObjDynamicIpsgBindingIfIndexValue)
           && (objDynamicIpsgBindingVanIdValue == nextObjDynamicIpsgBindingVanIdValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objDynamicIpsgBindingIfIndexValue != nextObjDynamicIpsgBindingIfIndexValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDynamicIpsgBindingVanIdValue, owa.len);

  /* return the object value: DynamicIpsgBindingVanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDynamicIpsgBindingVanIdValue,
                           sizeof (nextObjDynamicIpsgBindingVanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingMacAddr
*
* @purpose Get 'DynamicIpsgBindingMacAddr'
 *@description  [DynamicIpsgBindingMacAddr] MAC address match value for the IPSG
* Binding.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingMacAddr (void *wap, void *bufp)
{
  xLibU32_t objDynamicIpsgBindingIfIndexValue;
  xLibU32_t nextObjDynamicIpsgBindingIfIndexValue;

  xLibU32_t objDynamicIpsgBindingVanIdValue;
  xLibU32_t nextObjDynamicIpsgBindingVanIdValue;

  xLibStr6_t objDynamicIpsgBindingMacAddrValue;
  xLibStr6_t nextObjDynamicIpsgBindingMacAddrValue;

  xLibU32_t nextObjDynamicIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibS32_t rv,flag=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DynamicIpsgBindingIfIndex */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIfIndex,
                          (xLibU8_t *) & objDynamicIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingIfIndexValue, owa.len);

  /* retrieve key: DynamicIpsgBindingVanId */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingVanId,
                          (xLibU8_t *) & objDynamicIpsgBindingVanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingVanIdValue, owa.len);

  /* retrieve key: DynamicIpsgBindingMacAddr */
  owa.len = sizeof(xLibStr6_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingMacAddr,
                          (xLibU8_t *) objDynamicIpsgBindingMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjDynamicIpsgBindingIfIndexValue = 0;
    nextObjDynamicIpsgBindingVanIdValue = 0;
    memset (nextObjDynamicIpsgBindingMacAddrValue, 0, sizeof (nextObjDynamicIpsgBindingMacAddrValue));
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                &nextObjDynamicIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_DYNAMIC);

      if ((objDynamicIpsgBindingIfIndexValue == nextObjDynamicIpsgBindingIfIndexValue)
           && (objDynamicIpsgBindingVanIdValue == nextObjDynamicIpsgBindingVanIdValue))
      {
        break;
      }
    }while(owa.l7rc == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDynamicIpsgBindingMacAddrValue, owa.len);

    nextObjDynamicIpsgBindingIfIndexValue = 0;
    nextObjDynamicIpsgBindingVanIdValue = 0;
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    memset(nextObjDynamicIpsgBindingMacAddrValue, 0x0, sizeof (nextObjDynamicIpsgBindingMacAddrValue));
    
    flag = 0; 
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                &nextObjDynamicIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_DYNAMIC);

      rv = memcmp(nextObjDynamicIpsgBindingMacAddrValue, objDynamicIpsgBindingMacAddrValue, 
                    sizeof (nextObjDynamicIpsgBindingMacAddrValue));
      if ((rv != 0) && (flag == 0))
      {
        continue;
      }
      else if (rv == 0)
      {
        flag=1;
      }

      if ((rv != 0) && (flag == 1))
      {
        break; 
      }
    }
    while ((objDynamicIpsgBindingIfIndexValue == nextObjDynamicIpsgBindingIfIndexValue)
           && (objDynamicIpsgBindingVanIdValue == nextObjDynamicIpsgBindingVanIdValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objDynamicIpsgBindingIfIndexValue != nextObjDynamicIpsgBindingIfIndexValue)
      || (objDynamicIpsgBindingVanIdValue != nextObjDynamicIpsgBindingVanIdValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDynamicIpsgBindingMacAddrValue, owa.len);

  /* return the object value: DynamicIpsgBindingMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDynamicIpsgBindingMacAddrValue,
                           sizeof (nextObjDynamicIpsgBindingMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIpAddr
*
* @purpose Get 'DynamicIpsgBindingIpAddr'
 *@description  [DynamicIpsgBindingIpAddr] IP address match value for the IPSG
* Binding.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIpAddr (void *wap, void *bufp)
{

  xLibU32_t objDynamicIpsgBindingIfIndexValue;
  xLibU32_t nextObjDynamicIpsgBindingIfIndexValue;
  
  xLibU32_t objDynamicIpsgBindingVanIdValue;
  xLibU32_t nextObjDynamicIpsgBindingVanIdValue;

  xLibStr6_t objDynamicIpsgBindingMacAddrValue;
  xLibStr6_t nextObjDynamicIpsgBindingMacAddrValue;

  xLibU32_t objDynamicIpsgBindingIpAddrValue;
  xLibU32_t nextObjDynamicIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DynamicIpsgBindingIfIndex */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIfIndex,
                          (xLibU8_t *) & objDynamicIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingIfIndexValue, owa.len);

  /* retrieve key: DynamicIpsgBindingVanId */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingVanId,
                          (xLibU8_t *) & objDynamicIpsgBindingVanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingVanIdValue, owa.len);

  /* retrieve key: DynamicIpsgBindingMacAddr */
  owa.len = sizeof(xLibStr6_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingMacAddr,
                          (xLibU8_t *) objDynamicIpsgBindingMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objDynamicIpsgBindingMacAddrValue, owa.len);

  /* retrieve key: DynamicIpsgBindingIpAddr */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGDynamicBindingTable_DynamicIpsgBindingIpAddr,
                          (xLibU8_t *) & objDynamicIpsgBindingIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjDynamicIpsgBindingIfIndexValue = objDynamicIpsgBindingIfIndexValue;
    nextObjDynamicIpsgBindingVanIdValue = objDynamicIpsgBindingVanIdValue;
    nextObjDynamicIpsgBindingIpAddrValue = 0;
    memcpy (nextObjDynamicIpsgBindingMacAddrValue, objDynamicIpsgBindingMacAddrValue, sizeof (nextObjDynamicIpsgBindingMacAddrValue));

    do
    {
    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                                       (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                                       &nextObjDynamicIpsgBindingIpAddrValue,
                                                                       (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                                       IPSG_ENTRY_DYNAMIC);

      if ((objDynamicIpsgBindingIfIndexValue == nextObjDynamicIpsgBindingIfIndexValue)
           && (objDynamicIpsgBindingVanIdValue == nextObjDynamicIpsgBindingVanIdValue)
           && (memcmp (nextObjDynamicIpsgBindingMacAddrValue, objDynamicIpsgBindingMacAddrValue, sizeof (nextObjDynamicIpsgBindingMacAddrValue))==0)
           && (owa.l7rc == L7_SUCCESS))
      {
        break;
      }
 
    }while(owa.l7rc == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicIpsgBindingIpAddrValue, owa.len);
    nextObjDynamicIpsgBindingIfIndexValue = objDynamicIpsgBindingIfIndexValue;
    nextObjDynamicIpsgBindingVanIdValue = objDynamicIpsgBindingVanIdValue;
    nextObjDynamicIpsgBindingIpAddrValue = objDynamicIpsgBindingIpAddrValue;
    memcpy(nextObjDynamicIpsgBindingMacAddrValue, objDynamicIpsgBindingMacAddrValue, sizeof (nextObjDynamicIpsgBindingMacAddrValue));

    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjDynamicIpsgBindingIfIndexValue,
                                                                       (L7_ushort16 *)&nextObjDynamicIpsgBindingVanIdValue,
                                                                       &nextObjDynamicIpsgBindingIpAddrValue,
                                                                       (L7_enetMacAddr_t *)nextObjDynamicIpsgBindingMacAddrValue,
                                                                       IPSG_ENTRY_DYNAMIC);
  }

  if ((objDynamicIpsgBindingIfIndexValue != nextObjDynamicIpsgBindingIfIndexValue)
      || (objDynamicIpsgBindingVanIdValue != nextObjDynamicIpsgBindingVanIdValue)
      ||  (memcmp (nextObjDynamicIpsgBindingMacAddrValue, objDynamicIpsgBindingMacAddrValue, sizeof (nextObjDynamicIpsgBindingMacAddrValue)))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDynamicIpsgBindingIpAddrValue, owa.len);

  /* return the object value: DynamicIpsgBindingIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDynamicIpsgBindingIpAddrValue,
                           sizeof (nextObjDynamicIpsgBindingIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
