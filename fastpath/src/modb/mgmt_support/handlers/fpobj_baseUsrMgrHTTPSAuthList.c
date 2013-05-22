
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrHTTPSAuthList.c
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
#include "_xe_baseUsrMgrHTTPSAuthList_obj.h"
#include "user_manager_exports.h"
#include "usmdb_user_mgmt_api.h"

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethodOrder
*
* @purpose Get 'HTTPSAuthMethodOrder'
 *@description  [HTTPSAuthMethodOrder] The order of the HTTPS authentication
* method.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethodOrder (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodOrderValue;
  xLibU32_t nextObjHTTPSAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: HTTPSAuthMethodOrder */
  owa.len = sizeof (objHTTPSAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrHTTPSAuthList_HTTPSAuthMethodOrder,
                          (xLibU8_t *) & objHTTPSAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	nextObjHTTPSAuthMethodOrderValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objHTTPSAuthMethodOrderValue, owa.len);
	 if(objHTTPSAuthMethodOrderValue < L7_HTTPS_MAX_AUTH_METHODS)
	 {
	    nextObjHTTPSAuthMethodOrderValue = objHTTPSAuthMethodOrderValue + 1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjHTTPSAuthMethodOrderValue, owa.len);

  /* return the object value: HTTPSAuthMethodOrder */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjHTTPSAuthMethodOrderValue,
                           sizeof (nextObjHTTPSAuthMethodOrderValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Get 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU8_t  *httplistname_auth= "HttpListName";
  xLibU32_t keyHTTPSAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: HTTPSAuthMethodOrder */
  owa.len = sizeof (keyHTTPSAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrHTTPSAuthList_HTTPSAuthMethodOrder,
                          (xLibU8_t *) & keyHTTPSAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyHTTPSAuthMethodOrderValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_UNKNOWN,
                                             httplistname_auth,
                                             keyHTTPSAuthMethodOrderValue,
                                             &objHTTPSAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, sizeof (objHTTPSAuthMethodValue));

  /* return the object value: HTTPSAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue,
                           sizeof (objHTTPSAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Set 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU8_t  *httplistname_auth= "HttpListName";
  xLibU32_t keyHTTPSAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPSAuthMethod */
  owa.len = sizeof (objHTTPSAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, owa.len);

  /* retrieve key: HTTPSAuthMethodOrder */
  owa.len = sizeof (keyHTTPSAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrHTTPSAuthList_HTTPSAuthMethodOrder,
                          (xLibU8_t *) & keyHTTPSAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyHTTPSAuthMethodOrderValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_UNKNOWN,
                                             httplistname_auth,
                                             keyHTTPSAuthMethodOrderValue,
                                             objHTTPSAuthMethodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Get 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod1 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;

  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objHTTPSAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             0,
                                             &objHTTPSAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, sizeof (objHTTPSAuthMethodValue));

  /* return the object value: HTTPSAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue,
                           sizeof (objHTTPSAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Set 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod1 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPSAuthMethod */
  owa.len = sizeof (objHTTPSAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             0,
                                             objHTTPSAuthMethodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Get 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objHTTPSAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             1,
                                             &objHTTPSAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, sizeof (objHTTPSAuthMethodValue));

  /* return the object value: HTTPSAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue,
                           sizeof (objHTTPSAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Set 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod2 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU32_t objHTTPSAuthMethod1Value;

  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPSAuthMethod */
  owa.len = sizeof (objHTTPSAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, owa.len);

  /* get the method1 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             0,
                                             &objHTTPSAuthMethod1Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objHTTPSAuthMethod1Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             1,
                                             objHTTPSAuthMethod1Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             1,
                                             objHTTPSAuthMethodValue);
  }
											 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Get 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod3(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;

  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objHTTPSAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             2,
                                             &objHTTPSAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, sizeof (objHTTPSAuthMethodValue));

  /* return the object value: HTTPSAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue,
                           sizeof (objHTTPSAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Set 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod3 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU32_t objHTTPSAuthMethod2Value;

  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPSAuthMethod */
  owa.len = sizeof (objHTTPSAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, owa.len);


  /* get the method2 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             1,
                                             &objHTTPSAuthMethod2Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objHTTPSAuthMethod2Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             2,
                                             objHTTPSAuthMethod2Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             2,
                                             objHTTPSAuthMethodValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Get 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod4(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;

  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objHTTPSAuthMethodValue);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             3,
                                             &objHTTPSAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, sizeof (objHTTPSAuthMethodValue));

  /* return the object value: HTTPSAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue,
                           sizeof (objHTTPSAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPSAuthList_HTTPSAuthMethod
*
* @purpose Set 'HTTPSAuthMethod'
 *@description  [HTTPSAuthMethod] The HTTPS authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPSAuthListNew_AuthenticationListMethod4 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU32_t objHTTPSAuthMethod3Value;

  xLibU8_t  *pStrInfo_httpslistname_auth = "HttpsListName";
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPSAuthMethod */
  owa.len = sizeof (objHTTPSAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPSAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPSAuthMethodValue, owa.len);


  /* get the method3 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             2,
                                             &objHTTPSAuthMethod3Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objHTTPSAuthMethod3Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             3,
                                             objHTTPSAuthMethod3Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             3,
                                             objHTTPSAuthMethodValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPSAuthList_MethodList
*
* @purpose Get 'MethodList'
*
* @description
*             
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPSAuthListNew_MethodList (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMethodListValue;
  xLibU32_t objHTTPSAuthMethodValue;
  xLibU8_t  *pStrInfo_httpslistname_auth= "HttpsListName";


  xLibU32_t temp,i;
  L7_BOOL flag = L7_FALSE;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objMethodListValue,0x00,sizeof(objMethodListValue));

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objHTTPSAuthMethodValue);
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTPS,
                                             ACCESS_LEVEL_LOGIN,
                                             pStrInfo_httpslistname_auth,
                                             0,
                                             &objHTTPSAuthMethodValue );
  
  if (owa.l7rc == L7_SUCCESS)
  {
     for (i = 0; i < L7_HTTPS_MAX_AUTH_METHODS; i++)
     {
      
     /* get the value from application */
       owa.l7rc = usmDbAPLAuthMethodGet (ACCESS_LINE_HTTPS,
                                                  ACCESS_LEVEL_LOGIN,
                                         pStrInfo_httpslistname_auth,
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

