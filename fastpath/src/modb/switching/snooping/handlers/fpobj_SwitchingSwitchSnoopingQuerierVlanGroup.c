
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingSwitchSnoopingQuerierVlanGroup.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  24 May 2008, Saturday
*

* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingSwitchSnoopingQuerierVlanGroup_obj.h"
#include "usmdb_snooping_api.h"
#include "dot1q_exports.h"

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex
*
* @purpose Get 'dot1qVlanIndex'
 *@description  [dot1qVlanIndex] The VLAN-ID or other identifier refering to
* this VLAN   
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex (void *wap, void *bufp)
{

  xLibU32_t objdot1qVlanIndexValue;
  xLibU32_t nextObjdot1qVlanIndexValue;
  xLibU32_t objSnoopingProtocolValue;
  xLibU32_t vlanMode;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                          (xLibU8_t *) &objSnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: dot1qVlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                          (xLibU8_t *) & objdot1qVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdot1qVlanIndexValue = L7_DOT1Q_MIN_VLAN_ID;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1qVlanIndexValue, owa.len);
    nextObjdot1qVlanIndexValue = objdot1qVlanIndexValue+1;
  }

  owa.l7rc = L7_SUCCESS;
 
  do 
  { 
    if (nextObjdot1qVlanIndexValue > L7_DOT1Q_MAX_VLAN_ID)
    {
      owa.l7rc = L7_FAILURE;
      break;  
    }
    vlanMode = L7_DISABLE;
    (void)usmDbSnoopQuerierVlanModeGet(nextObjdot1qVlanIndexValue,&vlanMode,objSnoopingProtocolValue);
    if(vlanMode == L7_ENABLE)
    {
      break;
    }
    nextObjdot1qVlanIndexValue += 1; 
  }while(owa.l7rc == L7_SUCCESS);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1qVlanIndexValue, owa.len);

  /* return the object value: dot1qVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1qVlanIndexValue,
                           sizeof (objdot1qVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol
*
* @purpose Get 'SnoopingProtocol'
 *@description  [SnoopingProtocol] The protocol type of network protocol in use   
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol (void *wap, void *bufp)
{

/*  xLibU32_t objdot1qVlanIndexValue;
  xLibU32_t nextObjdot1qVlanIndexValue;*/
  xLibU32_t objSnoopingProtocolValue;
  xLibU32_t nextObjSnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
#if 0
  /* retrieve key: dot1qVlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                          (xLibU8_t *) & objdot1qVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1qVlanIndexValue, owa.len);
#endif
  /* retrieve key: SnoopingProtocol */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                          (xLibU8_t *) & objSnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    /*nextObjdot1qVlanIndexValue = objdot1qVlanIndexValue;*/
    memset (&objSnoopingProtocolValue, 0, sizeof (objSnoopingProtocolValue));
    
    nextObjSnoopingProtocolValue = L7_AF_INET;

/*    owa.l7rc = snmpSnoopVlanGetNext(L7_UNIT_CURRENT, &nextObjdot1qVlanIndexValue,
                                    objSnoopingProtocolValue, 
                                    &nextObjSnoopingProtocolValue);
*/
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSnoopingProtocolValue, owa.len);
/*    
    nextObjdot1qVlanIndexValue = objdot1qVlanIndexValue;
    owa.l7rc = snmpSnoopVlanGetNext(L7_UNIT_CURRENT, &nextObjdot1qVlanIndexValue,
                                    objSnoopingProtocolValue, 
                                    &nextObjSnoopingProtocolValue);
*/
    if(objSnoopingProtocolValue == L7_AF_INET)
      nextObjSnoopingProtocolValue = L7_AF_INET6;
    else
      owa.l7rc = L7_FAILURE;   
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSnoopingProtocolValue, owa.len);

  /* return the object value: SnoopingProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSnoopingProtocolValue,
                           sizeof (objSnoopingProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingQuerierVlanAdminMode
*
* @purpose Get 'SnoopingQuerierVlanAdminMode'
 *@description  [SnoopingQuerierVlanAdminMode] This enables or disables Snooping
* Querier on a selected Vlan interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingQuerierVlanAdminMode (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingQuerierVlanAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierVlanModeGet(keydot1qVlanIndexValue,
                                &objSnoopingQuerierVlanAdminModeValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingQuerierVlanAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingQuerierVlanAdminModeValue,
                    sizeof (objSnoopingQuerierVlanAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingQuerierVlanAdminMode
*
* @purpose Set 'SnoopingQuerierVlanAdminMode'
 *@description  [SnoopingQuerierVlanAdminMode] This enables or disables Snooping
* Querier on a selected Vlan interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingQuerierVlanAdminMode (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingQuerierVlanAdminModeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingQuerierVlanAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingQuerierVlanAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingQuerierVlanAdminModeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopQuerierVlanModeSet (keydot1qVlanIndexValue,objSnoopingQuerierVlanAdminModeValue,
                                           keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingQuerierVlanCfgDelete
*
* @purpose Set 'SnoopingQuerierVlanCfgDelete'
 *@description  [SnoopingQuerierVlanCfgDelete] This disables snooping
* Querier on a selected Vlan interface and defaults all the querier configuration related
* to that VLAN
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingQuerierVlanCfgDelete (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingQuerierVlanAdminModeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  xLibStr256_t querierAddress;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingQuerierVlanAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingQuerierVlanAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingQuerierVlanAdminModeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopQuerierVlanModeSet (keydot1qVlanIndexValue,objSnoopingQuerierVlanAdminModeValue,
                                           keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  /* Querier Vlan election participate */
  /* set the value in application */
  owa.l7rc = usmDbSnoopQuerierVlanElectionModeSet (keydot1qVlanIndexValue,
                                                   keySnoopingProtocolValue == L7_AF_INET ? 
                                                   FD_IGMP_SNOOPING_QUERIER_VLAN_ELECTION_MODE : 
                                                   FD_MLD_SNOOPING_QUERIER_VLAN_ELECTION_MODE,
                                                   keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  /* Querier Vlan address */
  memset(querierAddress,0x0,sizeof(querierAddress));
  owa.l7rc = usmDbSnoopQuerierVlanAddressSet(keydot1qVlanIndexValue,querierAddress,
                                             keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  else
  {
    (void)xLibFilterClear(wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex);
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierVlanOperMode
*
* @purpose Get 'QuerierVlanOperMode'
 *@description  [QuerierVlanOperMode] This specifies the current state of the
* Snooping Querier on a selected vlan interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierVlanOperMode (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierVlanOperModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierOperStateGet(keydot1qVlanIndexValue,
                                           &objQuerierVlanOperModeValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierVlanOperMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierVlanOperModeValue,
                           sizeof (objQuerierVlanOperModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierElectionParticipateMode
*
* @purpose Get 'QuerierElectionParticipateMode'
 *@description  [QuerierElectionParticipateMode] This enables or disables the
* Snooping Querier Election Participation mode. When enabled snooping
* querier switch will participate in querier election up on
* discovering another querier in the specified vlan. When disabled, up on
* discovering another querier,   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierElectionParticipateMode (void *wap,
                                                                                          void
                                                                                          *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierElectionParticipateModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierVlanElectionModeGet(keydot1qVlanIndexValue,
                              &objQuerierElectionParticipateModeValue,keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierElectionParticipateMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierElectionParticipateModeValue,
                    sizeof (objQuerierElectionParticipateModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierElectionParticipateMode
*
* @purpose Set 'QuerierElectionParticipateMode'
 *@description  [QuerierElectionParticipateMode] This enables or disables the
* Snooping Querier Election Participation mode. When enabled snooping
* querier switch will participate in querier election up on
* discovering another querier in the specified vlan. When disabled, up on
* discovering another querier,   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierElectionParticipateMode (void *wap,
                                                                                          void
                                                                                          *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierElectionParticipateModeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierElectionParticipateMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objQuerierElectionParticipateModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQuerierElectionParticipateModeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopQuerierVlanElectionModeSet (keydot1qVlanIndexValue,objQuerierElectionParticipateModeValue,
                                                   keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierVlanAddress
*
* @purpose Get 'QuerierVlanAddress'
 *@description  [QuerierVlanAddress] This specifies the default source ip
* address to be used while generating general queries on the specified
* vlan.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierVlanAddress (void *wap, void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof ( xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objQuerierVlanAddressValue;
  xLibStr256_t querierAddress;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierVlanAddressGet (keydot1qVlanIndexValue,
                              querierAddress,keySnoopingProtocolValue);

  
  memset(&objQuerierVlanAddressValue,0x0,sizeof(L7_inet_addr_t));
  inetAddressSet( keySnoopingProtocolValue, querierAddress,
                 (L7_inet_addr_t *)&objQuerierVlanAddressValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierVlanAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierVlanAddressValue,
                           sizeof (objQuerierVlanAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierVlanAddress
*
* @purpose Set 'QuerierVlanAddress'
 *@description  [QuerierVlanAddress] This specifies the default source ip
* address to be used while generating general queries on the specified
* vlan.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierVlanAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objQuerierVlanAddressValue;
  xLibStr256_t querierAddress;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierVlanAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objQuerierVlanAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQuerierVlanAddressValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  memset(querierAddress,0x0,sizeof(querierAddress));
  inetAddressGet(keySnoopingProtocolValue,&objQuerierVlanAddressValue,querierAddress );


  /* set the value in application */
  owa.l7rc = usmDbSnoopQuerierVlanAddressSet(keydot1qVlanIndexValue,querierAddress,
                                             keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierOperVersion
*
* @purpose Get 'QuerierOperVersion'
 *@description  [QuerierOperVersion] This specifies the multicast protocol
* version that is currently being used by the snooping switch for the
* specified vlan while generating query messages   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierOperVersion (void *wap, void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierOperVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierOperVersionGet(keydot1qVlanIndexValue,&objQuerierOperVersionValue,
                                     keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierOperVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierOperVersionValue,
                           sizeof (objQuerierOperVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierOperMaxResponseTime
*
* @purpose Get 'QuerierOperMaxResponseTime'
 *@description  [QuerierOperMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected vlan because
* it did not receive a report for a particular group in the
* interface participating in the vlan. This object is valid only when
* SwitchSnoopingQuerierOperVers   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierOperMaxResponseTime (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierOperMaxResponseTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierOperMaxRespTimeGet(keydot1qVlanIndexValue, 
                                         &objQuerierOperMaxResponseTimeValue,
                                         keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierOperMaxResponseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierOperMaxResponseTimeValue,
                    sizeof (objQuerierOperMaxResponseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierLastQuerierAddress
*
* @purpose Get 'QuerierLastQuerierAddress'
 *@description  [QuerierLastQuerierAddress] This specifies the last querier's ip
* address for the specified vlan. It represents the detected other
* multicast querier in the vlan.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierLastQuerierAddress (void *wap,
                                                                                     void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  L7_inet_addr_t objQuerierLastQuerierAddressValue;
  xLibStr256_t querierLastAddress;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierLastQuerierAddressGet (keydot1qVlanIndexValue,
                            querierLastAddress,keySnoopingProtocolValue);


  memset(&objQuerierLastQuerierAddressValue,0x0,sizeof(L7_inet_addr_t));
  inetAddressSet( keySnoopingProtocolValue, querierLastAddress,
                 (L7_inet_addr_t *)&objQuerierLastQuerierAddressValue );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierLastQuerierAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierLastQuerierAddressValue,
                           sizeof (objQuerierLastQuerierAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierLastQuerierVersion
*
* @purpose Get 'QuerierLastQuerierVersion'
 *@description  [QuerierLastQuerierVersion] This specifies the multicast
* protocol version that is currently being used by the detected other
* multicast querier for the specified vlan.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierVlanGroup_QuerierLastQuerierVersion (void *wap,
                                                                                     void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierLastQuerierVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_dot1qVlanIndex,
                   (xLibU8_t *) & keydot1qVlanIndexValue, &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingQuerierVlanGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopQuerierLastQuerierVersionGet(keydot1qVlanIndexValue,
                              &objQuerierLastQuerierVersionValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierLastQuerierVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierLastQuerierVersionValue,
                    sizeof (objQuerierLastQuerierVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
