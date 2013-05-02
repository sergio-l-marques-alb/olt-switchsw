/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename emweb_url.h
*
* @purpose
*
* @component EMWEB
*
* @comments
*
* @create 02/03/2009
*
* @author Vamshi Krishna Madap
* @end
*
**********************************************************************/

typedef char* (*URLHOOK)(EwsContext context, char *url);
typedef struct emwebUrlHook_s emwebUrlHook_t;
typedef struct
{
	 char* srcurl;
	 URLHOOK func;
}urlHook_t;

typedef struct emwebUrlHook_s
{
	urlHook_t urlhook;
	emwebUrlHook_t* next;
}emwebUrlHook_t;

/*********************************************************************
* @purpose  Register a url hook function
*
* @param    srcurl  source url on which the function needs to be called
*@param     func    function to be called if the url requested matches with srcurl
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments  When the requested url matches the srcurl, func is invoked which will return the new url 
*
* @end
*
*********************************************************************/

L7_RC_t emwebUrlRegisterHook(char* srcurl,void* func);

/*********************************************************************
* @purpose  Deregister a url hook function
*
* @param    srcurl  source url on which the function needs to be called

* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t emwebUrlDeregisterHook(void* func);

L7_RC_t ewaNewUrl(EwsContext context, char *url,char* dsturl);
