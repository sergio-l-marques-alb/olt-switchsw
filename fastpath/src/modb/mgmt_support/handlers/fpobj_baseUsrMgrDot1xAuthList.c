
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrDot1xAuthList.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 October 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseUsrMgrDot1xAuthList_obj.h"
#include "user_manager_exports.h"
#include "usmdb_user_mgmt_api.h"

/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethodOrder
*
* @purpose Get 'Dot1xAuthMethodOrder'
 *@description  [Dot1xAuthMethodOrder] The order of the Dot1x authentication
* method.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethodOrder (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodOrderValue;
  xLibU32_t nextObjDot1xAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xAuthMethodOrder */
  owa.len = sizeof (objDot1xAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrDot1xAuthList_Dot1xAuthMethodOrder,
                          (xLibU8_t *) & objDot1xAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0); 
	nextObjDot1xAuthMethodOrderValue = 1;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1xAuthMethodOrderValue, owa.len);
	 if(objDot1xAuthMethodOrderValue < L7_DOT1X_MAX_AUTH_METHODS)
	 {
	    nextObjDot1xAuthMethodOrderValue = objDot1xAuthMethodOrderValue + 1;
		owa.l7rc = L7_SUCCESS;
	 }
	 else
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDot1xAuthMethodOrderValue, owa.len);

  /* return the object value: Dot1xAuthMethodOrder */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDot1xAuthMethodOrderValue,
                           sizeof (nextObjDot1xAuthMethodOrderValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod
*
* @purpose Get 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  xLibU32_t keyDot1xAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xAuthMethodOrder */
  owa.len = sizeof (keyDot1xAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrDot1xAuthList_Dot1xAuthMethodOrder,
                          (xLibU8_t *) & keyDot1xAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1xAuthMethodOrderValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             "default",
                                             keyDot1xAuthMethodOrderValue,
                                             &objDot1xAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, sizeof (objDot1xAuthMethodValue));

  /* return the object value: Dot1xAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue,
                           sizeof (objDot1xAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod
*
* @purpose Set 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  xLibU32_t keyDot1xAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xAuthMethod */
  owa.len = sizeof (objDot1xAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, owa.len);

  /* retrieve key: Dot1xAuthMethodOrder */
  owa.len = sizeof (keyDot1xAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrDot1xAuthList_Dot1xAuthMethodOrder,
                          (xLibU8_t *) & keyDot1xAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1xAuthMethodOrderValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             "default",
                                             keyDot1xAuthMethodOrderValue,
                                             objDot1xAuthMethodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthenticationMethod
*
* @purpose Get 'Dot1xAuthenticationMethod'
 *@description  [Dot1xAuthenticationMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthenticationMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  L7_uint32 method, idx = 0;
  L7_int32  noneIdx = -1, radiusIdx = -1, iasIdx = -1;
	
  FPOBJ_TRACE_ENTER (bufp);

	/* get the value from application */
  for (idx = 0; idx < L7_DOT1X_MAX_AUTH_METHODS; idx++)
  {
    if (usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X, ACCESS_LEVEL_LOGIN,
                                             "default", idx, &method) == L7_SUCCESS)
    {
      if (method == L7_AUTH_METHOD_NONE)
        noneIdx = idx;
      else if (method == L7_AUTH_METHOD_RADIUS)
        radiusIdx = idx;
      else if (method == L7_AUTH_METHOD_IAS)
        iasIdx = idx;
    }
  }

  if (noneIdx == -1 && radiusIdx >= 0 && iasIdx == -1)
    objDot1xAuthMethodValue = L7_XUI_DOT1X_AUTH_METHOD_RADIUS;

  if (radiusIdx == -1 && noneIdx >= 0 && iasIdx == -1)
    objDot1xAuthMethodValue = L7_XUI_DOT1X_AUTH_METHOD_NONE;

  if (radiusIdx == -1 && noneIdx == -1 && iasIdx >= 0)
    objDot1xAuthMethodValue = L7_XUI_DOT1X_AUTH_METHOD_IAS;

  if (radiusIdx == -1 && noneIdx == -1 && iasIdx == -1)
    objDot1xAuthMethodValue = L7_XUI_DOT1X_AUTH_METHOD_UNDEFINED;
  
  if ((radiusIdx > noneIdx) && (radiusIdx != -1 && noneIdx != -1))
    objDot1xAuthMethodValue = L7_XUI_DOT1X_AUTH_METHOD_NONERADIUS;

  if ((radiusIdx < noneIdx) && (radiusIdx != -1 && noneIdx != -1))
    objDot1xAuthMethodValue = L7_XUI_DOT1X_AUTH_METHOD_RADIUSNONE;

	
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, sizeof (objDot1xAuthMethodValue));
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue,
                           sizeof (objDot1xAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod
*
* @purpose Set 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthenticationMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  L7_uint32  methods[2];
  L7_uint32  val;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xAuthMethod */
  owa.len = sizeof (objDot1xAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, owa.len);


  /* set the value in application */

  switch (objDot1xAuthMethodValue)
  {
    case L7_XUI_DOT1X_AUTH_METHOD_UNDEFINED:
      methods[0] = L7_AUTH_METHOD_UNDEFINED;
      methods[1] = L7_AUTH_METHOD_UNDEFINED;
      break;
    case L7_XUI_DOT1X_AUTH_METHOD_RADIUS:
      methods[0] = L7_AUTH_METHOD_RADIUS;
      methods[1] = L7_AUTH_METHOD_UNDEFINED;
      break;
    case L7_XUI_DOT1X_AUTH_METHOD_RADIUSNONE:
      methods[0] = L7_AUTH_METHOD_RADIUS;
      methods[1] = L7_AUTH_METHOD_NONE;
      break;
    case L7_XUI_DOT1X_AUTH_METHOD_NONE:
      methods[0] = L7_AUTH_METHOD_NONE;
      methods[1] = L7_AUTH_METHOD_UNDEFINED;
      break;
    case L7_XUI_DOT1X_AUTH_METHOD_NONERADIUS:
      methods[0] = L7_AUTH_METHOD_NONE;
      methods[1] = L7_AUTH_METHOD_RADIUS;
      break;
    default:
      methods[0] = L7_AUTH_METHOD_UNDEFINED;
      methods[1] = L7_AUTH_METHOD_UNDEFINED;
  }

  for (val = 0; val < L7_DOT1X_MAX_AUTH_METHODS; val++)
  {
    if (usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_LOGIN,
                                             "default",
                                             val,
                                             methods[val]) != L7_SUCCESS)
    {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
		break;

    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod1
*
* @purpose Get 'Dot1xAuthMethod1'
 *@description  [Dot1xAuthMethod1] The Dot1x authentication method1 being get .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod1 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objDot1xAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             0,
                                             &objDot1xAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, sizeof (objDot1xAuthMethodValue));

  /* return the object value: Dot1xAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue,
                           sizeof (objDot1xAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod1
*
* @purpose Set 'Dot1xAuthMethod1'
 *@description  [Dot1xAuthMethod1] The Dot1x authentication method1 being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod1 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;
  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xAuthMethod */
  owa.len = sizeof (objDot1xAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             0,
                                             objDot1xAuthMethodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod2
*
* @purpose Get 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being get .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;
  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objDot1xAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             1,
                                             &objDot1xAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, sizeof (objDot1xAuthMethodValue));

  /* return the object value: Dot1xAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue,
                           sizeof (objDot1xAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod2
*
* @purpose Set 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod2 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;
  xLibU32_t objDot1xAuthMethod1Value;

  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xAuthMethod */
  owa.len = sizeof (objDot1xAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, owa.len);


  /* get the method1 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             0,
                                             &objDot1xAuthMethod1Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objDot1xAuthMethod1Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             1,
                                             objDot1xAuthMethod1Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             1,
                                             objDot1xAuthMethodValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod3
*
* @purpose Get 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being get .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod3(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objDot1xAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             2,
                                             &objDot1xAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, sizeof (objDot1xAuthMethodValue));

  /* return the object value: Dot1xAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue,
                           sizeof (objDot1xAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod3
*
* @purpose Set 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod3 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;
  xLibU32_t objDot1xAuthMethod2Value;

  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xAuthMethod */
  owa.len = sizeof (objDot1xAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, owa.len);


  /* get the method2 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             1,
                                             &objDot1xAuthMethod2Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objDot1xAuthMethod2Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             2,
                                             objDot1xAuthMethod2Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             2,
                                             objDot1xAuthMethodValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod4
*
* @purpose Get 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being get .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod4(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;

  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objDot1xAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             3,
                                             &objDot1xAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, sizeof (objDot1xAuthMethodValue));

  /* return the object value: Dot1xAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue,
                           sizeof (objDot1xAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrDot1xAuthList_Dot1xAuthMethod4
*
* @purpose Set 'Dot1xAuthMethod'
 *@description  [Dot1xAuthMethod] The Dot1x authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrDot1xAuthListNew_AuthenticationListMethod4 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1xAuthMethodValue;
  xLibU32_t objDot1xAuthMethod2Value;

  xLibU8_t  *pStrInfo_Dot1xlistname_auth = "default";
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xAuthMethod */
  owa.len = sizeof (objDot1xAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xAuthMethodValue, owa.len);


  /* get the method2 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             2,
                                             &objDot1xAuthMethod2Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objDot1xAuthMethod2Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             3,
                                             objDot1xAuthMethod2Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             3,
                                             objDot1xAuthMethodValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrDot1xAuthList_MethodList
*
* @purpose Get 'MethodList'
*
* @description
*             
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrDot1xAuthListNew_MethodList (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMethodListValue;
  xLibU32_t objDot1xAuthMethodValue;
  xLibU8_t  *pStrInfo_Dot1xlistname_auth= "default";


  xLibU32_t temp,i;
  L7_BOOL flag = L7_FALSE;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objMethodListValue,0x00,sizeof(objMethodListValue));

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objDot1xAuthMethodValue);
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_Dot1xlistname_auth,
                                             0,
                                             &objDot1xAuthMethodValue );
  
  if (owa.l7rc == L7_SUCCESS)
  {
     for (i = 0; i < L7_DOT1X_MAX_AUTH_METHODS; i++)
     {
      
     /* get the value from application */
       owa.l7rc = usmDbAPLAuthMethodGet (ACCESS_LINE_DOT1X,
                                                  ACCESS_LEVEL_UNKNOWN,
                                         pStrInfo_Dot1xlistname_auth,
                                         i,
                                         &temp);
       
       if (owa.l7rc == L7_SUCCESS)
       {
         switch (temp)        /* auth method */
         {
           case L7_AUTH_METHOD_ENABLE:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",Enable",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"Enable",sizeof(objMethodListValue));
             }
             break;
          
           case L7_AUTH_METHOD_IAS:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",IAS",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"IAS",sizeof(objMethodListValue));
             }
             break;

           case L7_AUTH_METHOD_LINE:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",Line",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"Line",sizeof(objMethodListValue));
             }
             break;

           case L7_AUTH_METHOD_LOCAL:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",Local",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"Local",sizeof(objMethodListValue));
             }
             break;
			 
           case L7_AUTH_METHOD_NONE:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",None",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"None",sizeof(objMethodListValue));
             }
             break;
			 	 
           case L7_AUTH_METHOD_RADIUS:
             if(flag == L7_TRUE ) 
             {
               osapiStrncat(objMethodListValue,",RADIUS",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"RADIUS",sizeof(objMethodListValue));
             } 
             break;

           case L7_AUTH_METHOD_REJECT:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",Reject",sizeof(objMethodListValue));  
             }
             else
             {
               osapiStrncat(objMethodListValue,"Reject",sizeof(objMethodListValue));  
             }
             break;

           case L7_AUTH_METHOD_TACACS:
             if(flag == L7_TRUE )
             {  
               osapiStrncat(objMethodListValue,",TACACS+",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"TACACS+",sizeof(objMethodListValue));
             }
             break;

           default:
             break;
         }
         flag = L7_TRUE; 
       }
     }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    flag = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  
  /* return the object value: AuthListForDefaultUser */
  owa.rc =  xLibBufDataSet (bufp, (xLibU8_t *)  objMethodListValue,
                            strlen (objMethodListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


