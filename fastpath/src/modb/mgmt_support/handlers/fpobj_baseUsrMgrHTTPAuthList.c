
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrHTTPAuthList.c
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
#include "_xe_baseUsrMgrHTTPAuthList_obj.h"
#include "user_manager_exports.h"
#include "usmdb_user_mgmt_api.h"

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethodOrder
*
* @purpose Get 'HTTPAuthMethodOrder'
 *@description  [HTTPAuthMethodOrder] The order of the HTTP authentication
* method.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethodOrder (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodOrderValue;
  xLibU32_t nextObjHTTPAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: HTTPAuthMethodOrder */
  owa.len = sizeof (objHTTPAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrHTTPAuthList_HTTPAuthMethodOrder,
                          (xLibU8_t *) & objHTTPAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	nextObjHTTPAuthMethodOrderValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objHTTPAuthMethodOrderValue, owa.len);
	 if(objHTTPAuthMethodOrderValue < L7_HTTP_MAX_AUTH_METHODS)
	 {
	    nextObjHTTPAuthMethodOrderValue = objHTTPAuthMethodOrderValue + 1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjHTTPAuthMethodOrderValue, owa.len);

  /* return the object value: HTTPAuthMethodOrder */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjHTTPAuthMethodOrderValue,
                           sizeof (nextObjHTTPAuthMethodOrderValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Get 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *httplistname_auth= "HttpListName";
  xLibU32_t keyHTTPAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: HTTPAuthMethodOrder */
  owa.len = sizeof (keyHTTPAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrHTTPAuthList_HTTPAuthMethodOrder,
                          (xLibU8_t *) & keyHTTPAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyHTTPAuthMethodOrderValue, owa.len);

  /* get the value from application */                                             
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             httplistname_auth,
                                             keyHTTPAuthMethodOrderValue,
                                             &objHTTPAuthMethodValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, sizeof (objHTTPAuthMethodValue));

  /* return the object value: HTTPAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue,
                           sizeof (objHTTPAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Set 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPAuthList_HTTPAuthMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *httplistname_auth= "HttpListName";
  xLibU32_t keyHTTPAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPAuthMethod */
  owa.len = sizeof (objHTTPAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, owa.len);

  /* retrieve key: HTTPAuthMethodOrder */
  owa.len = sizeof (keyHTTPAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrHTTPAuthList_HTTPAuthMethodOrder,
                          (xLibU8_t *) & keyHTTPAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyHTTPAuthMethodOrderValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             httplistname_auth,
                                             keyHTTPAuthMethodOrderValue,
                                             objHTTPAuthMethodValue );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Get 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod1 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof ( objHTTPAuthMethodValue);

  /* get the value from application */                                             
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             0,
                                             &objHTTPAuthMethodValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, sizeof (objHTTPAuthMethodValue));

  /* return the object value: HTTPAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue,
                           sizeof (objHTTPAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Set 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod1 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPAuthMethod */
  owa.len = sizeof (objHTTPAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             0,
                                             objHTTPAuthMethodValue );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Get 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;


  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof ( objHTTPAuthMethodValue);

  /* get the value from application */                                             
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             1,
                                             &objHTTPAuthMethodValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, sizeof (objHTTPAuthMethodValue));

  /* return the object value: HTTPAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue,
                           sizeof (objHTTPAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Set 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod2 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU32_t objHTTPAuthMethod1Value;
  
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPAuthMethod */
  owa.len = sizeof (objHTTPAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, owa.len);


  /* get the method1 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             0,
                                             &objHTTPAuthMethod1Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objHTTPAuthMethod1Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             1,
                                             objHTTPAuthMethod1Value );
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             1,
                                             objHTTPAuthMethodValue );
  }
											 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Get 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod3 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";


  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof ( objHTTPAuthMethodValue);

  /* get the value from application */                                             
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             2,
                                             &objHTTPAuthMethodValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, sizeof (objHTTPAuthMethodValue));

  /* return the object value: HTTPAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue,
                           sizeof (objHTTPAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Set 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod3 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU32_t objHTTPAuthMethod2Value;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPAuthMethod */
  owa.len = sizeof (objHTTPAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, owa.len);

  /* get the method2 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             1,
                                             &objHTTPAuthMethod2Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objHTTPAuthMethod2Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             2,
                                             objHTTPAuthMethod2Value );
  }
  else
  {

    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             2,
                                             objHTTPAuthMethodValue );
  }
											 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Get 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod4 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";


  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof ( objHTTPAuthMethodValue);

  /* get the value from application */                                             
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             3,
                                             &objHTTPAuthMethodValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, sizeof (objHTTPAuthMethodValue));

  /* return the object value: HTTPAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue,
                           sizeof (objHTTPAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrHTTPAuthList_HTTPAuthMethod
*
* @purpose Set 'HTTPAuthMethod'
 *@description  [HTTPAuthMethod] The HTTP authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrHTTPAuthListNew_AuthenticationListMethod4 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHTTPAuthMethodValue;
  xLibU32_t objHTTPAuthMethod3Value;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HTTPAuthMethod */
  owa.len = sizeof (objHTTPAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHTTPAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHTTPAuthMethodValue, owa.len);

  /* get the method3 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             2,
                                             &objHTTPAuthMethod3Value );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objHTTPAuthMethod3Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             3,
                                             objHTTPAuthMethod3Value );
  }
  else
  {

    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             3,
                                             objHTTPAuthMethodValue );
  }
											 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrHTTPAuthList_MethodList
*
* @purpose Get 'MethodList'
*
* @description
*             
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrHTTPAuthListNew_MethodList (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMethodListValue;
  xLibU32_t objHTTPAuthMethodValue;
  xLibU8_t  *pStrInfo_httplistname_auth= "HttpListName";


  xLibU32_t temp,i;
  L7_BOOL flag = L7_FALSE;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objMethodListValue,0x00,sizeof(objMethodListValue));

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof ( objHTTPAuthMethodValue);
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_HTTP,
                                             ACCESS_LEVEL_UNKNOWN,
                                             pStrInfo_httplistname_auth,
                                             0,
                                             &objHTTPAuthMethodValue );
  
  if (owa.l7rc == L7_SUCCESS)
  {
     for (i = 0; i < L7_HTTP_MAX_AUTH_METHODS; i++)
     {
      
     /* get the value from application */
       owa.l7rc = usmDbAPLAuthMethodGet (ACCESS_LINE_HTTP,
                                                  ACCESS_LEVEL_UNKNOWN,
                                         pStrInfo_httplistname_auth,
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



