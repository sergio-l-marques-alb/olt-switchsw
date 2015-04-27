/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpAssoIpAddr.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to vrrpOper-object.xml
*
* @create  19 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingvrrpAssoIpAddr_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpAssoIpAddr_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] A unique value, greater than zero, for each interface.
* It is recommended that values are assigned contiguously starting
* from 1. The value for each interface sub-layer must remain
* constant at least from one re-initialization of the entity's network
* management system to the next re-initialization.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpAssoIpAddr_ifIndex (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t  objVrIdValue;
  xLibU8_t  nextObjVrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objifIndexValue = 0;
    nextObjifIndexValue = 0; 
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    owa.l7rc =
      usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                           &nextObjVrIdValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = 0;
    do
    {
      memcpy(&objVrIdValue,&nextObjVrIdValue,sizeof(nextObjVrIdValue));
      owa.l7rc =
      usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                             &nextObjVrIdValue, &nextObjifIndexValue);

    }
    while ((objifIndexValue == nextObjifIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingvrrpAssoIpAddr_VrId
*
* @purpose Get 'VrId'
 *@description  [VrId] This object contains the Virtual Router Identifier (VRID).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpAssoIpAddr_VrId (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t objVrIdValue;
  xLibU8_t nextObjVrIdValue;
  xLibU32_t vrId = 0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_VrId,
                          (xLibU8_t *) & vrId, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrIdValue, owa.len);
    objVrIdValue = (xLibU8_t)vrId;
  }

  nextObjVrIdValue = 0;
  nextObjifIndexValue = 0;

  owa.l7rc =
    usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, (xLibU8_t)objVrIdValue, objifIndexValue,
                         (xLibU8_t*)&nextObjVrIdValue, &nextObjifIndexValue);
 
  if ((objifIndexValue != nextObjifIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  vrId = nextObjVrIdValue;
  /* return the object value: VrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & vrId, sizeof (vrId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingvrrpAssoIpAddr_AssoIpAddr
*
* @purpose Get 'AssoIpAddr'
 *@description  [AssoIpAddr] The assigned IP addresses that a virtual router is
* responsible for backing up   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpAssoIpAddr_AssoIpAddr (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t  objVrIdValue;
  xLibU8_t  nextObjVrIdValue;
  xLibU32_t vrId;
  xLibIpV4_t primaryIpAddress;
  xLibIpV4_t objAssoIpAddrValue;
  xLibIpV4_t nextObjAssoIpAddrValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_VrId,
                          (xLibU8_t *) & vrId, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &vrId, kwa.len);

  /* retrieve key: AssoIpAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_AssoIpAddr,
                          (xLibU8_t *) & objAssoIpAddrValue, &owa.len);

  objVrIdValue = (xLibU8_t)vrId;
  usmDbVrrpOperIpAddressGet (L7_UNIT_CURRENT, objVrIdValue, objifIndexValue, &primaryIpAddress);

  if (owa.rc != XLIBRC_SUCCESS)
  {

	  FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	  memset (&objAssoIpAddrValue, 0, sizeof (objAssoIpAddrValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
	  nextObjifIndexValue = nextObjAssoIpAddrValue = 0;

    do
    {
   	  owa.l7rc = usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue, 
                                               objAssoIpAddrValue, &nextObjAssoIpAddrValue);
      objAssoIpAddrValue = nextObjAssoIpAddrValue;
    } while ((nextObjAssoIpAddrValue == primaryIpAddress)&&(owa.l7rc == L7_SUCCESS));

    if (owa.l7rc != L7_SUCCESS)
	  {
	    owa.rc = XLIBRC_ENDOF_TABLE;
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
  }
  else
  {
	  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAssoIpAddrValue, owa.len);	  

    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = nextObjAssoIpAddrValue = 0;
    do
    {
   	  owa.l7rc = usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue, 
                                               objAssoIpAddrValue, &nextObjAssoIpAddrValue);
      objAssoIpAddrValue = nextObjAssoIpAddrValue;
    } while ((nextObjAssoIpAddrValue == primaryIpAddress)&&(owa.l7rc == L7_SUCCESS));

    if (owa.l7rc != L7_SUCCESS)
	  {
	    owa.rc = XLIBRC_ENDOF_TABLE;
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
  }

  /* return the object value: AssoIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAssoIpAddrValue,
                           sizeof (nextObjAssoIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpAssoIpAddr_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: The row status variable, used according to installation
*              and removal conventions for conceptual rows. Setting
*              this object to active(1) or createAndGo(4) results in the
*              addition of an associated address for a virtual router.
*              Destroying the entry or setting it to notInService(2) removes
*              the associated address from the virtual router. The use
*              of other values is implementation-dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpAssoIpAddr_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t  keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAssoIpAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* retrieve key: AssoIpAddr */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_AssoIpAddr,
                           (xLibU8_t *) & keyAssoIpAddrValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAssoIpAddrValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpAssoIpAddr_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: The row status variable, used according to installation
*              and removal conventions for conceptual rows. Setting
*              this object to active(1) or createAndGo(4) results in the
*              addition of an associated address for a virtual router.
*              Destroying the entry or setting it to notInService(2) removes
*              the associated address from the virtual router. The use
*              of other values is implementation-dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpAssoIpAddr_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAssoIpAddrValue, primaryIpAddress;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* retrieve key: AssoIpAddr */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_AssoIpAddr,
                           (xLibU8_t *) & keyAssoIpAddrValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAssoIpAddrValue, kwa3.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;

  /* call the usmdb only for add and delete */
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    if (usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, objVrIdValue, keyifIndexValue, 0, &primaryIpAddress) == L7_SUCCESS)
    {    
      if (primaryIpAddress != keyAssoIpAddrValue) 
      {
        owa.l7rc =
          usmDbVrrpAssocSecondaryIpAddress(L7_UNIT_CURRENT, objVrIdValue,
                                         keyifIndexValue, keyAssoIpAddrValue);
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_VRRP_SECADDR_ADD_FAILED;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc =
      usmDbVrrpDissocSecondaryIpAddress(L7_UNIT_CURRENT, objVrIdValue,
                                         keyifIndexValue, keyAssoIpAddrValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_VRRP_SECADDR_DEL_FAILED; 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_routingvrrpAssoIpAddr_AssoIpAddrReturnzeroIfnone
*
* @purpose Get 'AssoIpAddr'
 *@description  [AssoIpAddr] The assigned IP addresses that a virtual router is
* responsible for backing up
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpAssoIpAddr_AssoIpAddrReturnzeroIfnone (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t  objVrIdValue;
  xLibU8_t  nextObjVrIdValue;
  xLibU32_t vrId;
  xLibIpV4_t primaryIpAddress;
  xLibIpV4_t objAssoIpAddrValue;
  xLibIpV4_t nextObjAssoIpAddrValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_VrId,
                          (xLibU8_t *) & vrId, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &vrId, kwa.len);

  /* retrieve key: AssoIpAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpAssoIpAddr_AssoIpAddrReturnzeroIfnone,
                          (xLibU8_t *) & objAssoIpAddrValue, &owa.len);

  objVrIdValue = (xLibU8_t)vrId;
  usmDbVrrpOperIpAddressGet (L7_UNIT_CURRENT, objVrIdValue, objifIndexValue, &primaryIpAddress);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objAssoIpAddrValue, 0, sizeof (objAssoIpAddrValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = nextObjAssoIpAddrValue = 0;

    do
    {
      owa.l7rc = usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                                               objAssoIpAddrValue, &nextObjAssoIpAddrValue);
      objAssoIpAddrValue = nextObjAssoIpAddrValue;
    } while ((nextObjAssoIpAddrValue == primaryIpAddress)&&(owa.l7rc == L7_SUCCESS));

    if (owa.l7rc != L7_SUCCESS)
    {
         nextObjAssoIpAddrValue =0;
    }
  }
    else
  {
    if (objAssoIpAddrValue == 0)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
      
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAssoIpAddrValue, owa.len);

    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = nextObjAssoIpAddrValue = 0;
    do
    {
 owa.l7rc = usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                                               objAssoIpAddrValue, &nextObjAssoIpAddrValue);
      objAssoIpAddrValue = nextObjAssoIpAddrValue;
    } while ((nextObjAssoIpAddrValue == primaryIpAddress)&&(owa.l7rc == L7_SUCCESS));
     if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
/* return the object value: AssoIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAssoIpAddrValue,
                           sizeof (nextObjAssoIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


