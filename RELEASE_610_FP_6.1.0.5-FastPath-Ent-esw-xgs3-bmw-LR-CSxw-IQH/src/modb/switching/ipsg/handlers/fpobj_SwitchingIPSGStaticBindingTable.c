
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingIPSGStaticBindingTable.c
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
#include "_xe_SwitchingIPSGStaticBindingTable_obj.h"
#include "usmdb_dhcp_snooping.h"
#include "dot1q_exports.h"
#include "usmdb_util_api.h"


extern L7_RC_t usmDbValidIntIfNumFirstGet(L7_uint32 *intIfNum);
extern L7_RC_t usmDbValidIntIfNumNext(L7_uint32 interface, L7_uint32 *intIfNum);

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex
*
* @purpose Get 'StaticIpsgBindingIfIndex'
 *@description  [StaticIpsgBindingIfIndex] Interface on which this IPSG binding
* is going to be added   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex (void *wap, void *bufp)
{

  xLibU32_t objStaticIpsgBindingIfIndexValue;
  xLibU32_t nextObjStaticIpsgBindingIfIndexValue;
  
  xLibU32_t objStaticIpsgBindingVanIdValue;
  xLibU32_t nextObjStaticIpsgBindingVanIdValue;

  xLibStr6_t objStaticIpsgBindingMacAddrValue;
  xLibStr6_t nextObjStaticIpsgBindingMacAddrValue;

  xLibU32_t objStaticIpsgBindingIpAddrValue;
  xLibU32_t nextObjStaticIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticIpsgBindingIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex,
                          (xLibU8_t *) & objStaticIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjStaticIpsgBindingIfIndexValue = 0;
    nextObjStaticIpsgBindingVanIdValue = 0;
    nextObjStaticIpsgBindingIpAddrValue = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0, sizeof (nextObjStaticIpsgBindingMacAddrValue));

    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                                                       (L7_ushort16 *)&nextObjStaticIpsgBindingVanIdValue,
                                                                       (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                                                       (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                                                       IPSG_ENTRY_STATIC);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingIfIndexValue, owa.len);
    nextObjStaticIpsgBindingIfIndexValue = objStaticIpsgBindingIfIndexValue;
    nextObjStaticIpsgBindingVanIdValue = 0;
    nextObjStaticIpsgBindingIpAddrValue = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0, sizeof (nextObjStaticIpsgBindingMacAddrValue));
    
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                                (L7_ushort16 *)&nextObjStaticIpsgBindingVanIdValue,
                                                (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_STATIC);

      objStaticIpsgBindingVanIdValue = nextObjStaticIpsgBindingVanIdValue;

      memcpy (objStaticIpsgBindingMacAddrValue, nextObjStaticIpsgBindingMacAddrValue,
                   sizeof (objStaticIpsgBindingMacAddrValue));

      objStaticIpsgBindingIpAddrValue = nextObjStaticIpsgBindingIpAddrValue;
    }
    while ((objStaticIpsgBindingIfIndexValue == nextObjStaticIpsgBindingIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticIpsgBindingIfIndexValue, owa.len);

  /* return the object value: StaticIpsgBindingIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticIpsgBindingIfIndexValue,
                           sizeof (nextObjStaticIpsgBindingIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId
*
* @purpose Get 'StaticIpsgBindingVanId'
 *@description  [StaticIpsgBindingVanId] VLAN to which this IPSG binding is
* going to be added   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId (void *wap, void *bufp)
{

  xLibU32_t objStaticIpsgBindingIfIndexValue;
  xLibU32_t nextObjStaticIpsgBindingIfIndexValue;

  xLibU32_t objStaticIpsgBindingVanIdValue;
  xLibU32_t nextObjStaticIpsgBindingVanIdValue;

  xLibStr6_t objStaticIpsgBindingMacAddrValue;
  xLibStr6_t nextObjStaticIpsgBindingMacAddrValue;

  xLibU32_t objStaticIpsgBindingIpAddrValue;
  xLibU32_t nextObjStaticIpsgBindingIpAddrValue;
  L7_ushort16 vlanId;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticIpsgBindingIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex,
                          (xLibU8_t *) & objStaticIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingIfIndexValue, owa.len);

  /* retrieve key: StaticIpsgBindingVanId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId,
                          (xLibU8_t *) & objStaticIpsgBindingVanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjStaticIpsgBindingIfIndexValue = objStaticIpsgBindingIfIndexValue;
    vlanId = 0;
    nextObjStaticIpsgBindingIpAddrValue = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0, sizeof (nextObjStaticIpsgBindingMacAddrValue));

    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                             &vlanId,
                                             (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                             (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                             IPSG_ENTRY_STATIC);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingVanIdValue, owa.len);
    nextObjStaticIpsgBindingIfIndexValue = objStaticIpsgBindingIfIndexValue;
    vlanId = (L7_ushort16)objStaticIpsgBindingVanIdValue;
    nextObjStaticIpsgBindingIpAddrValue = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0, sizeof (nextObjStaticIpsgBindingMacAddrValue));

    do
    { 
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                                &vlanId,
                                                (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_STATIC);
                                                                       
      memcpy (objStaticIpsgBindingMacAddrValue, nextObjStaticIpsgBindingMacAddrValue,
               sizeof (objStaticIpsgBindingMacAddrValue));
               
      objStaticIpsgBindingIpAddrValue = nextObjStaticIpsgBindingIpAddrValue;
    }
    while ((objStaticIpsgBindingIfIndexValue == nextObjStaticIpsgBindingIfIndexValue)
           && (objStaticIpsgBindingVanIdValue == vlanId)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objStaticIpsgBindingIfIndexValue != nextObjStaticIpsgBindingIfIndexValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjStaticIpsgBindingVanIdValue = vlanId;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticIpsgBindingVanIdValue, owa.len);

  /* return the object value: StaticIpsgBindingVanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticIpsgBindingVanIdValue,
                           sizeof (nextObjStaticIpsgBindingVanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingMacAddr
*
* @purpose Get 'StaticIpsgBindingMacAddr'
 *@description  [StaticIpsgBindingMacAddr] MAC address match value for the IPSG
* Binding.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingMacAddr (void *wap, void *bufp)
{

  xLibU32_t objStaticIpsgBindingIfIndexValue;
  xLibU32_t nextObjStaticIpsgBindingIfIndexValue;
  
  xLibU32_t objStaticIpsgBindingVanIdValue;

  xLibStr6_t objStaticIpsgBindingMacAddrValue;
  xLibStr6_t nextObjStaticIpsgBindingMacAddrValue;

  xLibU32_t nextObjStaticIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  
  xLibS32_t rv,flag=0;
  L7_ushort16 vlanId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticIpsgBindingIfIndex */
  owa.len  = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex,
                          (xLibU8_t *) & objStaticIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingIfIndexValue, owa.len);

  /* retrieve key: StaticIpsgBindingVanId */
  owa.len  = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId,
                          (xLibU8_t *) & objStaticIpsgBindingVanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingVanIdValue, owa.len);

  /* retrieve key: StaticIpsgBindingMacAddr */
  owa.len  = sizeof(objStaticIpsgBindingMacAddrValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingMacAddr,
                          (xLibU8_t *) objStaticIpsgBindingMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjStaticIpsgBindingIfIndexValue = 0;
    vlanId = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0, sizeof (nextObjStaticIpsgBindingMacAddrValue));
    nextObjStaticIpsgBindingIpAddrValue = 0;
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                                &vlanId,
                                                (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_STATIC);

      if ((objStaticIpsgBindingIfIndexValue == nextObjStaticIpsgBindingIfIndexValue)
           && (objStaticIpsgBindingVanIdValue == vlanId))
      {
        break;
      }
    }while(owa.l7rc == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objStaticIpsgBindingMacAddrValue, owa.len);
    nextObjStaticIpsgBindingIfIndexValue = 0;
    vlanId = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0x0, sizeof (nextObjStaticIpsgBindingMacAddrValue));
    nextObjStaticIpsgBindingIpAddrValue = 0;
   
    flag = 0; 
    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                                &vlanId,
                                                (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_STATIC);

      rv = memcmp(nextObjStaticIpsgBindingMacAddrValue, objStaticIpsgBindingMacAddrValue, 
                    sizeof (nextObjStaticIpsgBindingMacAddrValue));
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
    while ((objStaticIpsgBindingIfIndexValue == nextObjStaticIpsgBindingIfIndexValue)
           && (objStaticIpsgBindingVanIdValue == vlanId)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objStaticIpsgBindingIfIndexValue != nextObjStaticIpsgBindingIfIndexValue)
      || (objStaticIpsgBindingVanIdValue != vlanId)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjStaticIpsgBindingMacAddrValue, owa.len);

  /* return the object value: StaticIpsgBindingMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjStaticIpsgBindingMacAddrValue,
                           sizeof (nextObjStaticIpsgBindingMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIpAddr
*
* @purpose Get 'StaticIpsgBindingIpAddr'
 *@description  [StaticIpsgBindingIpAddr] IP address match value for the IPSG
* Binding.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIpAddr (void *wap, void *bufp)
{

  xLibU32_t objStaticIpsgBindingIfIndexValue;
  xLibU32_t nextObjStaticIpsgBindingIfIndexValue;
  
  xLibU32_t objStaticIpsgBindingVanIdValue;

  xLibStr6_t objStaticIpsgBindingMacAddrValue;
  xLibStr6_t nextObjStaticIpsgBindingMacAddrValue;

  xLibU32_t objStaticIpsgBindingIpAddrValue;
  xLibU32_t nextObjStaticIpsgBindingIpAddrValue;
  L7_ushort16 vlanId;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticIpsgBindingIfIndex */
  owa.len  = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex,
                          (xLibU8_t *) & objStaticIpsgBindingIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingIfIndexValue, owa.len);

  /* retrieve key: StaticIpsgBindingVanId */
  owa.len  = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId,
                          (xLibU8_t *) & objStaticIpsgBindingVanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingVanIdValue, owa.len);

  /* retrieve key: StaticIpsgBindingMacAddr */
  owa.len  = sizeof(objStaticIpsgBindingMacAddrValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingMacAddr,
                          (xLibU8_t *) objStaticIpsgBindingMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objStaticIpsgBindingMacAddrValue, owa.len);

  /* retrieve key: StaticIpsgBindingIpAddr */
  owa.len  = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIpAddr,
                          (xLibU8_t *) & objStaticIpsgBindingIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjStaticIpsgBindingIfIndexValue = 0;
    vlanId = 0;
    memset (nextObjStaticIpsgBindingMacAddrValue, 0x0, sizeof (nextObjStaticIpsgBindingMacAddrValue));
    nextObjStaticIpsgBindingIpAddrValue = 0;

    do
    {
      owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                                &vlanId,
                                                (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                                (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                                IPSG_ENTRY_STATIC);
      if ((objStaticIpsgBindingIfIndexValue == nextObjStaticIpsgBindingIfIndexValue)
           && (objStaticIpsgBindingVanIdValue == vlanId)
           && (memcmp (nextObjStaticIpsgBindingMacAddrValue, objStaticIpsgBindingMacAddrValue, sizeof (nextObjStaticIpsgBindingMacAddrValue))==0)
           && (owa.l7rc == L7_SUCCESS))
      {
        break;
      }
 
    }while(owa.l7rc == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticIpsgBindingIpAddrValue, owa.len);

    nextObjStaticIpsgBindingIfIndexValue = objStaticIpsgBindingIfIndexValue;
    vlanId = (L7_ushort16) objStaticIpsgBindingVanIdValue;
    memcpy (nextObjStaticIpsgBindingMacAddrValue, objStaticIpsgBindingMacAddrValue, sizeof (nextObjStaticIpsgBindingMacAddrValue));
    nextObjStaticIpsgBindingIpAddrValue = objStaticIpsgBindingIpAddrValue;
    
    owa.l7rc = usmDbIpsgBindingGetNextByType (&nextObjStaticIpsgBindingIfIndexValue,
                                              &vlanId,
                                              (L7_uint32 *)&nextObjStaticIpsgBindingIpAddrValue,
                                              (L7_enetMacAddr_t*)nextObjStaticIpsgBindingMacAddrValue,
                                              IPSG_ENTRY_STATIC);
  }

  if ((objStaticIpsgBindingIfIndexValue != nextObjStaticIpsgBindingIfIndexValue)
      || (objStaticIpsgBindingVanIdValue != vlanId)
      ||  (memcmp(nextObjStaticIpsgBindingMacAddrValue, objStaticIpsgBindingMacAddrValue, sizeof (nextObjStaticIpsgBindingMacAddrValue))!=0)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticIpsgBindingIpAddrValue, owa.len);

  /* return the object value: StaticIpsgBindingIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticIpsgBindingIpAddrValue,
                           sizeof (nextObjStaticIpsgBindingIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingRowStatus
*
* @purpose Get 'StaticIpsgBindingRowStatus'
 *@description  [StaticIpsgBindingRowStatus] The status of this conceptual row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingRowStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaStaticIpsgBindingIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticIpsgBindingIfIndexValue;
  
  fpObjWa_t kwaStaticIpsgBindingVanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticIpsgBindingVanIdValue;

  fpObjWa_t kwaStaticIpsgBindingMacAddr = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticIpsgBindingMacAddrValue;

  fpObjWa_t kwaStaticIpsgBindingIpAddr = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticIpsgBindingIpAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t nextObjStaticIpsgBindingRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticIpsgBindingIfIndex */
  owa.len  = sizeof(xLibU32_t);
  kwaStaticIpsgBindingIfIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex,
                   (xLibU8_t *) & keyStaticIpsgBindingIfIndexValue,
                   &kwaStaticIpsgBindingIfIndex.len);
  if (kwaStaticIpsgBindingIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaStaticIpsgBindingIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingIfIndex);
    return kwaStaticIpsgBindingIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingIfIndexValue,
                           kwaStaticIpsgBindingIfIndex.len);

  /* retrieve key: StaticIpsgBindingVanId */
  owa.len  = sizeof(xLibU32_t);
  kwaStaticIpsgBindingVanId.rc =
    xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId,
                   (xLibU8_t *) & keyStaticIpsgBindingVanIdValue, &kwaStaticIpsgBindingVanId.len);
  if (kwaStaticIpsgBindingVanId.rc != XLIBRC_SUCCESS)
  {
    kwaStaticIpsgBindingVanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingVanId);
    return kwaStaticIpsgBindingVanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingVanIdValue, kwaStaticIpsgBindingVanId.len);

  /* retrieve key: StaticIpsgBindingMacAddr */
  owa.len  = sizeof(keyStaticIpsgBindingMacAddrValue);
  kwaStaticIpsgBindingMacAddr.rc =
    xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingMacAddr,
                   (xLibU8_t *) keyStaticIpsgBindingMacAddrValue, &kwaStaticIpsgBindingMacAddr.len);
  if (kwaStaticIpsgBindingMacAddr.rc != XLIBRC_SUCCESS)
  {
    kwaStaticIpsgBindingMacAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingMacAddr);
    return kwaStaticIpsgBindingMacAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticIpsgBindingMacAddrValue, kwaStaticIpsgBindingMacAddr.len);

  /* retrieve key: StaticIpsgBindingIpAddr */
  owa.len  = sizeof(xLibU32_t);
  kwaStaticIpsgBindingIpAddr.rc =
    xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIpAddr,
                   (xLibU8_t *) & keyStaticIpsgBindingIpAddrValue, &kwaStaticIpsgBindingIpAddr.len);
  if (kwaStaticIpsgBindingIpAddr.rc != XLIBRC_SUCCESS)
  {
    kwaStaticIpsgBindingIpAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingIpAddr);
    return kwaStaticIpsgBindingIpAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingIpAddrValue, kwaStaticIpsgBindingIpAddr.len);

  /* retrieve key: StaticIpsgBindingRowStatus */
  owa.l7rc = usmDbIpsgBindingGetByType(&keyStaticIpsgBindingIfIndexValue, 
                                                              (L7_ushort16 *)&keyStaticIpsgBindingVanIdValue,
                                                              (L7_uint32 *)&keyStaticIpsgBindingIpAddrValue,
                                                              (L7_enetMacAddr_t*)keyStaticIpsgBindingMacAddrValue, 
                                                              IPSG_ENTRY_STATIC);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    nextObjStaticIpsgBindingRowStatusValue = L7_ROW_STATUS_ACTIVE;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticIpsgBindingRowStatusValue, owa.len);

  /* return the object value: StaticIpsgBindingRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticIpsgBindingRowStatusValue,
                           sizeof (nextObjStaticIpsgBindingRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingRowStatus
*
* @purpose Set 'StaticIpsgBindingRowStatus'
 *@description  [StaticIpsgBindingRowStatus] The status of this conceptual row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingIPSGStaticBindingTable_StaticIpsgBindingRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticIpsgBindingRowStatusValue;

  fpObjWa_t kwaStaticIpsgBindingIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticIpsgBindingIfIndexValue;
  
  fpObjWa_t kwaStaticIpsgBindingVanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticIpsgBindingVanIdValue;
  
  fpObjWa_t kwaStaticIpsgBindingMacAddr = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticIpsgBindingMacAddrValue={0};
  
  fpObjWa_t kwaStaticIpsgBindingIpAddr = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticIpsgBindingIpAddrValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticIpsgBindingRowStatus */
  owa.len  = sizeof(xLibU32_t);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStaticIpsgBindingRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticIpsgBindingRowStatusValue, owa.len);
  /* if (objStaticIpsgBindingRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) */
  if (0)
  {
    /* retrieve key: StaticIpsgBindingIfIndex */
    owa.len  = sizeof(xLibU32_t);
    kwaStaticIpsgBindingIfIndex.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_IfIndex,
                     (xLibU8_t *) & keyStaticIpsgBindingIfIndexValue,
                     &kwaStaticIpsgBindingIfIndex.len);
    if (kwaStaticIpsgBindingIfIndex.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingIfIndex.rc = XLIBRC_IPSG_INTERFACE_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingIfIndex);
      return kwaStaticIpsgBindingIfIndex.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingIfIndexValue,
                             kwaStaticIpsgBindingIfIndex.len);
  
    /* retrieve key: StaticIpsgBindingVanId */
    owa.len  = sizeof(xLibU32_t);
    kwaStaticIpsgBindingVanId.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_VlanId,
                     (xLibU8_t *) & keyStaticIpsgBindingVanIdValue, &kwaStaticIpsgBindingVanId.len);
    if (kwaStaticIpsgBindingVanId.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingVanId.rc = XLIBRC_VLANID_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingVanId);
      return kwaStaticIpsgBindingVanId.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingVanIdValue, kwaStaticIpsgBindingVanId.len);

    /* retrieve key: StaticIpsgBindingMacAddr */
    owa.len  = sizeof(xLibStr6_t);
    kwaStaticIpsgBindingMacAddr.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_MacAddress,
                   (xLibU8_t *) keyStaticIpsgBindingMacAddrValue, &kwaStaticIpsgBindingMacAddr.len);
    if (kwaStaticIpsgBindingMacAddr.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingMacAddr.rc = XLIBRC_MAC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingMacAddr);
      return kwaStaticIpsgBindingMacAddr.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticIpsgBindingMacAddrValue, kwaStaticIpsgBindingMacAddr.len);

    /* retrieve key: StaticIpsgBindingIpAddr */
    owa.len  = sizeof(xLibU32_t);
    kwaStaticIpsgBindingIpAddr.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_IpAddress,
                   (xLibU8_t *) & keyStaticIpsgBindingIpAddrValue, &kwaStaticIpsgBindingIpAddr.len);
    if (kwaStaticIpsgBindingIpAddr.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingIpAddr.rc = XLIBRC_IP_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingIpAddr);
      return kwaStaticIpsgBindingIpAddr.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingIpAddrValue, kwaStaticIpsgBindingIpAddr.len);
  }
  else
  {
    /* retrieve key: StaticIpsgBindingIfIndex */
    owa.len  = sizeof(xLibU32_t);
    kwaStaticIpsgBindingIfIndex.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIfIndex,
                     (xLibU8_t *) & keyStaticIpsgBindingIfIndexValue,
                     &kwaStaticIpsgBindingIfIndex.len);
    if (kwaStaticIpsgBindingIfIndex.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingIfIndex.rc = XLIBRC_IPSG_INTERFACE_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingIfIndex);
      return kwaStaticIpsgBindingIfIndex.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingIfIndexValue,
                             kwaStaticIpsgBindingIfIndex.len);

    /* retrieve key: StaticIpsgBindingVanId */
    owa.len  = sizeof(xLibU32_t);
    kwaStaticIpsgBindingVanId.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingVanId,
                     (xLibU8_t *) & keyStaticIpsgBindingVanIdValue, &kwaStaticIpsgBindingVanId.len);
    if (kwaStaticIpsgBindingVanId.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingVanId.rc = XLIBRC_VLANID_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingVanId);
      return kwaStaticIpsgBindingVanId.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingVanIdValue, kwaStaticIpsgBindingVanId.len);

    /* retrieve key: StaticIpsgBindingMacAddr */
    owa.len  = sizeof(keyStaticIpsgBindingMacAddrValue);
    kwaStaticIpsgBindingMacAddr.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingMacAddr,
                   (xLibU8_t *) keyStaticIpsgBindingMacAddrValue, &kwaStaticIpsgBindingMacAddr.len);
    if (kwaStaticIpsgBindingMacAddr.rc != XLIBRC_SUCCESS)
    {
      kwaStaticIpsgBindingMacAddr.rc = XLIBRC_MAC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingMacAddr);
      return kwaStaticIpsgBindingMacAddr.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticIpsgBindingMacAddrValue, kwaStaticIpsgBindingMacAddr.len);

    /* retrieve key: StaticIpsgBindingIpAddr */
    owa.len  = sizeof(xLibU32_t);
    kwaStaticIpsgBindingIpAddr.rc =
      xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_StaticIpsgBindingIpAddr,
                    (xLibU8_t *) & keyStaticIpsgBindingIpAddrValue, &kwaStaticIpsgBindingIpAddr.len);
    if (kwaStaticIpsgBindingIpAddr.rc != XLIBRC_SUCCESS)
    { 
      kwaStaticIpsgBindingIpAddr.rc = XLIBRC_IP_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaStaticIpsgBindingIpAddr);
      return kwaStaticIpsgBindingIpAddr.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticIpsgBindingIpAddrValue, kwaStaticIpsgBindingIpAddr.len);
  }

  owa.l7rc = L7_SUCCESS;
  if (objStaticIpsgBindingRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbIpsgStaticEntryAdd(keyStaticIpsgBindingIfIndexValue,keyStaticIpsgBindingVanIdValue,
                                                            (L7_enetMacAddr_t*)&keyStaticIpsgBindingMacAddrValue,keyStaticIpsgBindingIpAddrValue);
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_IPSG_BINDING_ADD_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;    
     }
  } 
  else if (objStaticIpsgBindingRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbIpsgStaticEntryRemove(keyStaticIpsgBindingIfIndexValue,keyStaticIpsgBindingVanIdValue,
                                           (L7_enetMacAddr_t*) &keyStaticIpsgBindingMacAddrValue,keyStaticIpsgBindingIpAddrValue);
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_IPSG_BINDING_DEL_FAILURE ;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc; 
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*************** Newly Added ***********/

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_IfIndex
*
* @purpose Get 'IfIndex'
* @description  [StaticIpsgBindingIfIndex] Interface on which this IPSG binding
* is going to be added   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_IfIndex (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue=0;
  xLibU32_t nextObjIfIndexValue=0;
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext(objIfIndexValue, &nextObjIfIndexValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue,
                           sizeof (nextObjIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingIPSGStaticBindingTable_VlanId
*
* @purpose Get 'VanId'
 *@description  [VanId] VLAN to which this IPSG binding is
* going to be added   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGStaticBindingTable_VlanId (void *wap, void *bufp)
{

  xLibU32_t objVlanIdValue;
  xLibU32_t nextObjVlanIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticIpsgBindingVanId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGStaticBindingTable_VlanId,
                          (xLibU8_t *) & objVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanIdValue = L7_DOT1Q_MIN_VLAN_ID; 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIdValue, owa.len);
    nextObjVlanIdValue = objVlanIdValue+1;
    if (nextObjVlanIdValue > L7_DOT1Q_MAX_VLAN_ID)
    {
      owa.l7rc = L7_FAILURE;  
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIdValue, owa.len);

  /* return the object value: VlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIdValue,
                           sizeof (nextObjVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_SwitchingIPSGStaticBindingTable_IpAddress
*
 *@description  [StaticIpsgBindingRowStatus] The status of this conceptual row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingIPSGStaticBindingTable_IpAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibIpV4_t objStaticIpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&objStaticIpAddressValue, 0, sizeof(objStaticIpAddressValue));

  /* retrieve object: IpAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStaticIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticIpAddressValue, owa.len);

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingIPSGStaticBindingTable_MacAddress
*
* @purpose Set 'StaticIpsgBindingRowStatus'
 *@description  [StaticIpsgBindingRowStatus] The status of this conceptual row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingIPSGStaticBindingTable_MacAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objStaticMacAddressValue={0};

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MacAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objStaticMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objStaticMacAddressValue, owa.len);

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
