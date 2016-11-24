
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpInterfaceAssociationTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to captiveportal-object.xml
*
* @create  06 July 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_captiveportalcpInterfaceAssociationTable_obj.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_util_api.h"

extern L7_RC_t usmDbConvertIntfListToMask(L7_uint32 *list, L7_uint32 listNum, L7_INTF_MASK_t *mask);

/*******************************************************************************
* @function fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID
*
* @purpose Get 'cpIntfAssociationCPID'
 *@description  [cpIntfAssociationCPID] The Captive Portal instance identifier
* associated with an interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID (void *wap,
                                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpIntfAssociationCPIDValue;
  xLibU32_t objcpIntfAssociationIfIndexValue;
  xLibU16_t nextObjcpIntfAssociationCPIDValue;
  xLibU32_t nextObjcpIntfAssociationIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationCPID */
  owa.len = sizeof (objcpIntfAssociationCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID,
                          (xLibU8_t *) & objcpIntfAssociationCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objcpIntfAssociationCPIDValue = 0;
    objcpIntfAssociationIfIndexValue = 0;
    owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpIntfAssociationCPIDValue, owa.len);
    objcpIntfAssociationIfIndexValue = 0;
    do
    {
        owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                        objcpIntfAssociationIfIndexValue, 
                        &nextObjcpIntfAssociationCPIDValue,
                        &nextObjcpIntfAssociationIfIndexValue);
        objcpIntfAssociationIfIndexValue = nextObjcpIntfAssociationIfIndexValue;
    }
    while((objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpIntfAssociationCPIDValue, owa.len);

  /* return the object value: cpIntfAssociationCPID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpIntfAssociationCPIDValue,
                           sizeof (nextObjcpIntfAssociationCPIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationIfIndex
*
* @purpose Get 'cpIntfAssociationIfIndex'
 *@description  [cpIntfAssociationIfIndex] The interface ifIndex to which the
* Captive Portal instance is associated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationIfIndex (void *wap,
                                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU16_t objcpIntfAssociationCPIDValue;
  xLibU32_t objcpIntfAssociationIfIndexValue;
  xLibU16_t nextObjcpIntfAssociationCPIDValue;
  xLibU32_t nextObjcpIntfAssociationIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationIfIndex */
  owa.len = sizeof (objcpIntfAssociationIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationIfIndex,
                   (xLibU8_t *) & objcpIntfAssociationIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpIntfAssociationIfIndexValue = 0;
    objcpIntfAssociationCPIDValue = 0;
    owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpIntfAssociationIfIndexValue, owa.len);
    objcpIntfAssociationCPIDValue = 0;
    do
    {
        owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
        objcpIntfAssociationCPIDValue = nextObjcpIntfAssociationCPIDValue;
    }
    while((objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue) && (objcpIntfAssociationIfIndexValue == nextObjcpIntfAssociationIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (!(objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpIntfAssociationIfIndexValue, owa.len);

  /* return the object value: cpIntfAssociationIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpIntfAssociationIfIndexValue,
                           sizeof (nextObjcpIntfAssociationIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationNotAssocIfIndex
*
* @purpose Get 'cpIntfAssociationNotAssocIfIndex'
 *@description  [cpIntfAssociationNotAssocIfIndex] The interface ifIndices which
* are not associated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationNotAssocIfIndex (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpIntfAssociationNotAssocIfIndexValue;
  xLibU32_t nextObjcpIntfAssociationNotAssocIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationNotAssocIfIndex */
  owa.len = sizeof (objcpIntfAssociationNotAssocIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationNotAssocIfIndex,
                   (xLibU8_t *) & objcpIntfAssociationNotAssocIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32(objcpIntfAssociationNotAssocIfIndexValue);
    owa.l7rc = usmDbCpdmCPConfigIntfNotAssocNextGet (objcpIntfAssociationNotAssocIfIndexValue,
                                    &nextObjcpIntfAssociationNotAssocIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpIntfAssociationNotAssocIfIndexValue, owa.len);
    owa.l7rc = usmDbCpdmCPConfigIntfNotAssocNextGet (objcpIntfAssociationNotAssocIfIndexValue,
                                    &nextObjcpIntfAssociationNotAssocIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpIntfAssociationNotAssocIfIndexValue, owa.len);

  /* return the object value: cpIntfAssociationNotAssocIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpIntfAssociationNotAssocIfIndexValue,
                           sizeof (nextObjcpIntfAssociationNotAssocIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationRowStatus
*
* @purpose Get 'cpIntfAssociationRowStatus'
 *@description  [cpIntfAssociationRowStatus] The Captive Portal instance to
* interface association status.Supported values:active(1)      - valid
* entry    createAndGo(4) - creates a new entry     destroy(6)    
* - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationRowStatus (void *wap,
                                                                                       void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpIntfAssociationRowStatusValue;

  xLibU32_t keycpIntfAssociationCPIDValue;
  xLibU32_t keycpIntfAssociationIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationCPID */
  owa.len = sizeof (keycpIntfAssociationCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID,
                          (xLibU8_t *) & keycpIntfAssociationCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationCPIDValue, owa.len);

  /* retrieve key: cpIntfAssociationIfIndex */
  owa.len = sizeof (keycpIntfAssociationIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationIfIndex,
                   (xLibU8_t *) & keycpIntfAssociationIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigIntIfNumGet (keycpIntfAssociationCPIDValue,
                              keycpIntfAssociationIfIndexValue);

  if (owa.l7rc == L7_SUCCESS)
    objcpIntfAssociationRowStatusValue = L7_ROW_STATUS_ACTIVE;
  else
    objcpIntfAssociationRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objcpIntfAssociationRowStatusValue,
                     sizeof (objcpIntfAssociationRowStatusValue));

  /* return the object value: cpIntfAssociationRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpIntfAssociationRowStatusValue,
                           sizeof (objcpIntfAssociationRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationRowStatus
*
* @purpose Set 'cpIntfAssociationRowStatus'
 *@description  [cpIntfAssociationRowStatus] The Captive Portal instance to
* interface association status.Supported values:active(1)      - valid
* entry    createAndGo(4) - creates a new entry     destroy(6)    
* - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationRowStatus (void *wap,
                                                                                       void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpIntfAssociationRowStatusValue;

  xLibU32_t keycpIntfAssociationCPIDValue;
  xLibU32_t keycpIntfAssociationIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpIntfAssociationRowStatus */
  owa.len = sizeof (objcpIntfAssociationRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpIntfAssociationRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpIntfAssociationRowStatusValue, owa.len);

  /* retrieve key: cpIntfAssociationCPID */
  owa.len = sizeof (keycpIntfAssociationCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID,
                          (xLibU8_t *) & keycpIntfAssociationCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationCPIDValue, owa.len);

  /* retrieve key: cpIntfAssociationIfIndex */
  owa.len = sizeof (keycpIntfAssociationIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationIfIndex,
                   (xLibU8_t *) & keycpIntfAssociationIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationIfIndexValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (usmDbCpdmCPConfigIntIfNumGet (keycpIntfAssociationCPIDValue, keycpIntfAssociationIfIndexValue) 
                  != L7_SUCCESS)
  {
    if (objcpIntfAssociationRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
        {
            owa.l7rc = usmDbCpdmCPConfigIntIfNumAdd (keycpIntfAssociationCPIDValue,
                            keycpIntfAssociationIfIndexValue);
        }
  }
  else if (objcpIntfAssociationRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbCpdmCPConfigIntIfNumDelete (keycpIntfAssociationCPIDValue,
                                   keycpIntfAssociationIfIndexValue);              
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjList_captiveportalcpInterfaceAssociationTable_cpIntfAssociationInterfaces (void *wap, void *bufp)
{
  L7_INTF_MASK_t interfacePortMask;
  L7_INTF_MASK_t mask;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keycpIntfAssociationCPIDValue;
  xLibU16_t objcpIntfAssociationCPIDValue;
  xLibU32_t objcpIntfAssociationIfIndexValue;
  xLibU16_t nextObjcpIntfAssociationCPIDValue=0;
  xLibU32_t nextObjcpIntfAssociationIfIndexValue=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationCPID */
  owa.len = sizeof (keycpIntfAssociationCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID,
                          (xLibU8_t *) & keycpIntfAssociationCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationCPIDValue, owa.len);

  objcpIntfAssociationCPIDValue = (xLibU16_t )keycpIntfAssociationCPIDValue;

  xLibU16_t bufLen = sizeof (interfacePortMask);
  memset(&interfacePortMask,0x0,sizeof(interfacePortMask)); 
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationInterfaces,
        (xLibU8_t *) &interfacePortMask, &bufLen))
  {
    objcpIntfAssociationIfIndexValue = 0;
    nextObjcpIntfAssociationIfIndexValue = 0;
#if 0
    while (owa.l7rc == L7_SUCCESS)
    {
        objcpIntfAssociationIfIndexValue = nextObjcpIntfAssociationIfIndexValue;
#endif
        do
        {
            owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
        }
        while((objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue) && (objcpIntfAssociationIfIndexValue == nextObjcpIntfAssociationIfIndexValue) && (owa.l7rc == L7_SUCCESS));

#if 0
    }
#endif
  }
  else
  {
    objcpIntfAssociationIfIndexValue = fpObjPortFromMaskGet(interfacePortMask);
    if(objcpIntfAssociationIfIndexValue != -1)
    {
#if 0 
    while (owa.l7rc == L7_SUCCESS)
    {
#endif
        do
        {
            owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
        }
        while((objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue) && (objcpIntfAssociationIfIndexValue == nextObjcpIntfAssociationIfIndexValue) && (owa.l7rc == L7_SUCCESS));
    }
#if 0
    }
#endif
  }

  if ((objcpIntfAssociationCPIDValue != nextObjcpIntfAssociationCPIDValue) || (owa.l7rc != L7_SUCCESS))
  { 
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  memset(&mask,0x0,sizeof(mask));

  L7_INTF_SETMASKBIT(mask,nextObjcpIntfAssociationIfIndexValue);

  /* return the object value: GroupInterface*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &mask,
                           sizeof (mask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationInterfaces (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_INTF_MASK_t portMask;

  L7_uint32 intfList[L7_FILTER_MAX_INTF]={0};
  L7_uint32 numIntf;
  xLibU16_t objcpIntfAssociationCPIDValue;
  xLibU32_t keycpIntfAssociationCPIDValue;
  xLibU32_t objcpIntfAssociationIfIndexValue;
  xLibU16_t nextObjcpIntfAssociationCPIDValue;
  xLibU32_t nextObjcpIntfAssociationIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationCPID */
  owa.len = sizeof (keycpIntfAssociationCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID,
                          (xLibU8_t *) & keycpIntfAssociationCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationCPIDValue, owa.len);

  numIntf = 0;
  objcpIntfAssociationCPIDValue = (xLibU16_t)keycpIntfAssociationCPIDValue;
  objcpIntfAssociationIfIndexValue = 0;
  nextObjcpIntfAssociationIfIndexValue = 0;
  while (owa.l7rc == L7_SUCCESS)
  {
    objcpIntfAssociationIfIndexValue = nextObjcpIntfAssociationIfIndexValue;
    do
    {
        owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
    }
    while((objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue) && (objcpIntfAssociationIfIndexValue == nextObjcpIntfAssociationIfIndexValue) && (owa.l7rc == L7_SUCCESS));

    if ((objcpIntfAssociationCPIDValue != nextObjcpIntfAssociationCPIDValue) || (owa.l7rc != L7_SUCCESS))
        {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
        }

    intfList[numIntf] = nextObjcpIntfAssociationIfIndexValue;
    numIntf++;
  }

  memset(&portMask, 0, sizeof(portMask));

  if (numIntf != 0)
  {
    owa.l7rc =  usmDbConvertIntfListToMask(intfList, numIntf, &portMask);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
 }

  /* return the object value: GroupInterface*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &portMask,
                           sizeof (portMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjSet_captiveportalcpInterfaceAssociationTable_cpIntfAssociationInterfaces(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t intfMask;
  memset(&intfMask,0x0,sizeof(intfMask));

  L7_uint32 numIntf, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;
  xLibU16_t objcpIntfAssociationCPIDValue;
  xLibU32_t keycpIntfAssociationCPIDValue;
  xLibU32_t objcpIntfAssociationIfIndexValue;
  xLibU16_t nextObjcpIntfAssociationCPIDValue;
  xLibU32_t nextObjcpIntfAssociationIfIndexValue;


  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
#if 0
  L7_BOOL errorFound;
  xLibRC_t rc;

  errorFound = L7_FALSE;
#endif

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpIntfAssociationCPID */
  owa.len = sizeof (keycpIntfAssociationCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpInterfaceAssociationTable_cpIntfAssociationCPID,
                          (xLibU8_t *) & keycpIntfAssociationCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpIntfAssociationCPIDValue, owa.len);

  owa.len = sizeof(intfMask);
  /* retrieve object: GroupInterface */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &intfMask, owa.len);

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
  numIntf = 0;
  numIntfPrev = 0;
  objcpIntfAssociationCPIDValue = (xLibU16_t)keycpIntfAssociationCPIDValue;
  nextObjcpIntfAssociationIfIndexValue = 0;
  do
  {
    do
    {
        objcpIntfAssociationIfIndexValue = nextObjcpIntfAssociationIfIndexValue;
        owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpIntfAssociationCPIDValue, 
                    objcpIntfAssociationIfIndexValue, 
                    &nextObjcpIntfAssociationCPIDValue,
                    &nextObjcpIntfAssociationIfIndexValue);
    }
    while((objcpIntfAssociationIfIndexValue == nextObjcpIntfAssociationIfIndexValue) && (owa.l7rc == L7_SUCCESS));

    if ((owa.l7rc == L7_SUCCESS) && (objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue))
    {
        intfListPrev[numIntfPrev++] = nextObjcpIntfAssociationIfIndexValue;
    }
  }
  while ((owa.l7rc == L7_SUCCESS) && (objcpIntfAssociationCPIDValue == nextObjcpIntfAssociationCPIDValue));

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
#if 0
    errorFound = L7_FALSE;
#endif

    if ((numIntfToAdd == 0) || (numIntfToRemove ==0))
        owa.l7rc = L7_SUCCESS;

    for (i=0; i<numIntfToAdd; i++)
    {
      owa.l7rc = usmDbCpdmCPConfigIntIfNumAdd(keycpIntfAssociationCPIDValue, intfListToAdd[i]);
#if 0
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
#endif
    }

    for (i=0; i<numIntfToRemove; i++)
    {
      owa.l7rc = usmDbCpdmCPConfigIntIfNumDelete(keycpIntfAssociationCPIDValue, intfListToRemove[i]);
      if (owa.l7rc != L7_SUCCESS)
      {
#if 0
        errorFound = L7_TRUE;
#endif
        break;
      }
   }
  }
  else    /* Remove all configured Interfaces */
  {
    numIntfToRemove = 0;
#if 0
    errorFound = L7_FALSE;
#endif

    owa.l7rc = L7_SUCCESS;
    for (i=0; i<numIntfPrev; i++)
    {
      owa.l7rc = usmDbCpdmCPConfigIntIfNumDelete(keycpIntfAssociationCPIDValue, intfListPrev[i]);
#if 0
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
#endif
    }
  }

#if 0
  if(errorFound == L7_TRUE )
  {
    owa.l7rc = L7_FAILURE;
  }
#endif

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
