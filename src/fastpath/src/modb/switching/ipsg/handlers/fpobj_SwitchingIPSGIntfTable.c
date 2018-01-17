
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingIPSGIntfTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  09 June 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingIPSGIntfTable_obj.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGIntfTable_IpsgIntIfNumber
*
* @purpose Get 'IpsgIntIfNumber'
 *@description  [IpsgIntIfNumber] Interface number   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGIntfTable_IpsgIntIfNumber (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpsgIntIfNumberValue;
  xLibU32_t nextObjIpsgIntIfNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpsgIntIfNumber */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIntIfNumber,
                          (xLibU8_t *) & objIpsgIntIfNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjIpsgIntIfNumberValue = 0;
    objIpsgIntIfNumberValue = 0;
  }

  owa.l7rc = L7_FAILURE;
  while (usmDbGetNextVisibleIntIfNumber(objIpsgIntIfNumberValue, &nextObjIpsgIntIfNumberValue) == L7_SUCCESS)
  {
      if (usmDbDsIntfIsValid(nextObjIpsgIntIfNumberValue) == L7_TRUE)
      {
        owa.l7rc = L7_SUCCESS;
        break;
      }
      else
      {
        objIpsgIntIfNumberValue = nextObjIpsgIntIfNumberValue;
      }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpsgIntIfNumberValue, owa.len);

  /* return the object value: IpsgIntIfNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpsgIntIfNumberValue,
                           sizeof (nextObjIpsgIntIfNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGIntfTable_IpsgIfVerifySource
*
* @purpose Get 'IpsgIfVerifySource'
 *@description  [IpsgIfVerifySource] This object indicates whether the interface
* is enabled for IPSG to forward the data based up on source Ip
* address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGIntfTable_IpsgIfVerifySource (void *wap, void *bufp)
{

  fpObjWa_t kwaIpsgIntIfNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpsgIntIfNumberValue;
  xLibU32_t temp_val;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpsgIfVerifySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpsgIntIfNumber */
  kwaIpsgIntIfNumber.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIntIfNumber,
                                         (xLibU8_t *) & keyIpsgIntIfNumberValue,
                                         &kwaIpsgIntIfNumber.len);
  if (kwaIpsgIntIfNumber.rc != XLIBRC_SUCCESS)
  {
    kwaIpsgIntIfNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIpsgIntIfNumber);
    return kwaIpsgIntIfNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpsgIntIfNumberValue, kwaIpsgIntIfNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbIpsgVerifySourceGet (keyIpsgIntIfNumberValue,&objIpsgIfVerifySourceValue,&temp_val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpsgIfVerifySource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpsgIfVerifySourceValue,
                           sizeof (objIpsgIfVerifySourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingIPSGIntfTable_IpsgIfVerifySource
*
* @purpose Set 'IpsgIfVerifySource'
 *@description  [IpsgIfVerifySource] This object indicates whether the interface
* is enabled for IPSG to forward the data based up on source Ip
* address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingIPSGIntfTable_IpsgIfVerifySource (void *wap, void *bufp)
{

  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objIpsgIfVerifySourceValue;
  xLibU32_t objIpsgIfPortSecurityValue;
  xLibU32_t tempIpsgIfVerifySourceValue;
  xLibU32_t tempIpsgIfPortSecurityValue;
  
  fpObjWa_t kwaIpsgIntIfNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpsgIntIfNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpsgIfVerifySource */
  owa1.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIpsgIfVerifySourceValue, &owa1.len);
  if (owa1.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa1);
    return owa1.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpsgIfVerifySourceValue, owa1.len);



   /* retrieve object: IpsgIfPortSecurityValue */
  owa2.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIfPortSecurity,
                                (xLibU8_t *) &objIpsgIfPortSecurityValue,
                                     &owa2.len);
  if (owa2.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa2);
    return owa2.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpsgIfPortSecurityValue, owa2.len);

/* retrieve key: IpsgIntIfNumber */
  kwaIpsgIntIfNumber.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIntIfNumber,
                                         (xLibU8_t *) & keyIpsgIntIfNumberValue,
                                         &kwaIpsgIntIfNumber.len);
  if (kwaIpsgIntIfNumber.rc != XLIBRC_SUCCESS)
  {
    kwaIpsgIntIfNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIpsgIntIfNumber);
    return kwaIpsgIntIfNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpsgIntIfNumberValue, kwaIpsgIntIfNumber.len);



  if ((owa1.l7rc = usmDbIpsgVerifySourceGet (keyIpsgIntIfNumberValue,&tempIpsgIfVerifySourceValue,&tempIpsgIfPortSecurityValue)) == L7_SUCCESS)
  {
     if( (tempIpsgIfVerifySourceValue == objIpsgIfVerifySourceValue ) && ( tempIpsgIfPortSecurityValue == objIpsgIfPortSecurityValue ) )
     {
       owa1.rc = XLIBRC_SUCCESS;
       FPOBJ_TRACE_EXIT (bufp, owa1);
       return owa1.rc;

     }
  }

  owa1.l7rc = usmDbIpsgVerifySourceSet (keyIpsgIntIfNumberValue, objIpsgIfVerifySourceValue,objIpsgIfPortSecurityValue);


  if (owa1.l7rc != L7_SUCCESS)
  {
    owa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa1);
    return owa1.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa1);
  return owa1.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIPSGIntfTable_IpsgIfPortSecurity
*
* @purpose Get 'IpsgIfPortSecurity'
 *@description  [IpsgIfPortSecurity] This object indicates whether the interface
* is enabled for IPSG to forward the data based up on source mac
* address in fdb table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIPSGIntfTable_IpsgIfPortSecurity (void *wap, void *bufp)
{

  fpObjWa_t kwaIpsgIntIfNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpsgIntIfNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpsgIfPortSecurityValue;
  xLibU32_t temp_val;
    
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpsgIntIfNumber */
  kwaIpsgIntIfNumber.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIntIfNumber,
                                         (xLibU8_t *) & keyIpsgIntIfNumberValue,
                                         &kwaIpsgIntIfNumber.len);
  if (kwaIpsgIntIfNumber.rc != XLIBRC_SUCCESS)
  {
    kwaIpsgIntIfNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIpsgIntIfNumber);
    return kwaIpsgIntIfNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpsgIntIfNumberValue, kwaIpsgIntIfNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbIpsgVerifySourceGet (keyIpsgIntIfNumberValue,&temp_val,&objIpsgIfPortSecurityValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpsgIfPortSecurity */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpsgIfPortSecurityValue,
                           sizeof (objIpsgIfPortSecurityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingIPSGIntfTable_IpsgIfPortSecurity
*
* @purpose Set 'IpsgIfPortSecurity'
 *@description  [IpsgIfPortSecurity] This object indicates whether the interface
* is enabled for IPSG to forward the data based up on source mac
* address in fdb table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingIPSGIntfTable_IpsgIfPortSecurity (void *wap, void *bufp)
{

  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpsgIfPortSecurityValue;


  fpObjWa_t owa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpsgIfVerifySourceValue;

  xLibU32_t tempIpsgIfPortSecurityValue;
  xLibU32_t tempIpsgIfVerifySourceValue;

  fpObjWa_t kwaIpsgIntIfNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpsgIntIfNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpsgIfPortSecurity */
  owa1.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIpsgIfPortSecurityValue, &owa1.len);
  if (owa1.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa1);
    return owa1.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpsgIfPortSecurityValue, owa1.len);

  owa2.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIfVerifySource,
                                         (xLibU8_t *) &objIpsgIfVerifySourceValue,
                                         &owa2.len);


  if (owa2.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa2);
    return owa2.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpsgIfVerifySourceValue, owa2.len);


  /* retrieve key: IpsgIntIfNumber */
  kwaIpsgIntIfNumber.rc = xLibFilterGet (wap, XOBJ_SwitchingIPSGIntfTable_IpsgIntIfNumber,
                                         (xLibU8_t *) & keyIpsgIntIfNumberValue,
                                         &kwaIpsgIntIfNumber.len);
  if (kwaIpsgIntIfNumber.rc != XLIBRC_SUCCESS)
  {
    kwaIpsgIntIfNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIpsgIntIfNumber);
    return kwaIpsgIntIfNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpsgIntIfNumberValue, kwaIpsgIntIfNumber.len);


  if ((owa1.l7rc = usmDbIpsgVerifySourceGet (keyIpsgIntIfNumberValue,&tempIpsgIfVerifySourceValue,&tempIpsgIfPortSecurityValue)) == L7_SUCCESS)
  {
     if( (tempIpsgIfVerifySourceValue == objIpsgIfVerifySourceValue ) && ( tempIpsgIfPortSecurityValue == objIpsgIfPortSecurityValue ) )
     {
       owa1.rc = XLIBRC_SUCCESS;
       FPOBJ_TRACE_EXIT (bufp, owa1);
       return owa1.rc;

     }
  }



  owa1.l7rc = usmDbIpsgVerifySourceSet (keyIpsgIntIfNumberValue, objIpsgIfVerifySourceValue,objIpsgIfPortSecurityValue);

  if (owa1.l7rc != L7_SUCCESS)
  {
    owa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa1);
  return owa1.rc;

}
