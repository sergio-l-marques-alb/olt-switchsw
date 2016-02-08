
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_ports.h
*
* @purpose  header file for user manager functions related to user port access
*
* @component userMgr component
*
* @comments  none
*
* @create    05/13/2003
*
* @author    jflanagan
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l7_common.h"

/*********************************************************************
* @purpose Set the access for all users on all ports
*
* @param    allow (input) boolean indicating access (TRUE) or not (FALSE)
*
* @returns  void
*
* @notes 
*       
* @end
*********************************************************************/
void 
userMgrPortInitializeAll( L7_BOOL allow  );

/*********************************************************************
* @purpose Return the name of the first user with access to the port  
*
* @param    intIfNum (input) Internal interface number of port 
* @param    pUser (output) ptr to user name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface or
*                      if no users have access to the port
*
* @notes 
*       
* @end
*********************************************************************/
L7_uint32 
userMgrPortUserGetFirst(L7_uint32 intIfNum, L7_uchar8 *pUser );

/*********************************************************************
* @purpose Return the name of the next user with access to the port  
*
* @param    intIfNum (input) Internal interface number of port 
* @param    pPrevUser (input) ptr to user name returned by prior getNext or getFirst
* @param    pUser (output) ptr to user name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface or
*                      if no more users have access to the port
*
* @notes 
*       
* @end
*********************************************************************/
L7_uint32 
userMgrPortUserGetNext(L7_uint32 intIfNum, L7_uchar8 *pPrevUser, L7_uchar8 *pUser );

/*********************************************************************
* @purpose Configure the port access for the specified user on the specified port
*
* @param    intIfNum (input) Internal interface number of port 
* @param    pUser (input) ptr to user name
* @param    allow (input) boolean indicating access (TRUE) or not (FALSE)
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface or
*                      if the user is not a configured user
*
* @notes 
*       
* @end
*********************************************************************/
L7_uint32 
userMgrPortUserAccessSet(L7_uint32 intIfNum, L7_uchar8 *pUser, L7_BOOL allow );

/*********************************************************************
* @purpose Get the port access setting for a specific user on a specific port
*
* @param    intIfNum (input) Internal interface number of port 
* @param    pUser (input) ptr to user name
* @param    pAllow (output) ptr to boolean indicating access (TRUE) or not (FALSE)
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface or
*                      if the user is not a configured user
*
* @notes 
*       
* @end
*********************************************************************/
L7_uint32 
userMgrPortUserAccessGet(L7_uint32 intIfNum, L7_uchar8 *pUser, L7_BOOL *pAllow );

/*********************************************************************
* @purpose Get the port access setting for a specific user (by index) on a specific port
*
* @param    intIfNum (input) Internal interface number of port 
* @param    index (input) index into login structure
* @param    pAllow (output) ptr to boolean indicating access (TRUE) or not (FALSE)
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface or
*                      if the index is not valid
*
* @notes 
*       
* @end
*********************************************************************/
L7_uint32 
userMgrPortUserAccessGetIndex(L7_uint32 intIfNum, L7_uint32 index, L7_BOOL *pAllow );

/*********************************************************************
* @purpose Reset the user bit on all ports
*
* @param    index (input) index into login structure
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the index is not valid
*
* @notes  Sets the user access based on defined default
*       
* @end
*********************************************************************/
L7_uint32 
userMgrPortUserInitialize(L7_uint32 index);

/*********************************************************************
* @purpose  Check to see if the interface type is valid for the userMgr ports API
*
* @param    sysIntfType  {(input}) interface type
*
* @returns  L7_TRUE or L7_FALSE
*
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL userMgrPortIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Check to see if the intIfNum is valid for the userMgr ports API
*
* @param    intIfNum  {(input}) Internal interface number
*
* @returns  L7_TRUE or L7_FALSE 
*           
*
* @notes  
*       
* @end
*********************************************************************/
L7_BOOL userMgrPortIsValidIntf(L7_uint32 intIfNum);
