
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_apl.h
*
* @purpose   header file for user manager functions related to APLs
*
* @component userMgr component
*
* @comments  none
*
* @create    05/11/2003
*
* @author    jflanagan
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef USER_MGR_APL_H
#define USER_MGR_APL_H

#define L7_USER_MGR_DEFAULT_USER_STRING "default"
#include "user_manager_exports.h"

/*********************************************************************
* @purpose  Creates a named APL with a single default authentication method
*
* @param    pName    pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if name is empty or too long or 
*                      if max number of APLs has been reached or
*                      if the APL name already exists.
*
* @notes pName must be null terminated.   
*       
* @end
*********************************************************************/
L7_RC_t userMgrAPLCreate(L7_ACCESS_LINE_t line,
                         L7_ACCESS_LEVEL_t level,
                         L7_char8 *pName);

/*********************************************************************
* @purpose  Deletes a named APL  
*
* @param    pName    pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL does not exist or 
*                      users have this APL assigned and therefore it cannot be deleted or 
*                      APL name is not valid or
*                      the APL being deleted is the default (system) APL which
*                           can never be deleted
*
* @notes  pName must be null terminated.  
*       
* @end
*********************************************************************/
L7_RC_t userMgrAPLDelete(L7_ACCESS_LINE_t line,
                         L7_ACCESS_LEVEL_t level,
                         L7_char8 *pName);

/*********************************************************************
* @purpose  Determines if the specified APL is the system default
*
* @param    pName    pointer to APL name   
*
* @returns  L7_TRUE if the specified APL is the defaults
*           L7_FALSE if the specified APL is not the default
*
* @notes  This routine is helpful to determine why an attempt to delete
*         an APL fails.
*       
* @end
*********************************************************************/
L7_uint32  userMgrAPLDefaultCheck(L7_ACCESS_LINE_t line,
                                  L7_ACCESS_LEVEL_t level,
                                  L7_char8 *pName);


/*********************************************************************
* @purpose  Returns the APL at the specified index in the list
*
* @param    pName    (output) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if index is not valid or if there is not
*                      an APL at the specified index.
*
* @notes index begins with 0
*
* @end
*********************************************************************/
L7_uint32 
userMgrAPLListGetIndex(L7_uint32 index, L7_char8 *pName );

/*********************************************************************
* @purpose  Returns the number of configured APLs
*
* @param    pCount (output) number of APLs 
*
* @returns  L7_SUCCESS
*
* @notes 
*
* @end
*********************************************************************/
L7_uint32 
userMgrAPLListGetCount(L7_uint32 *pCount );

/*********************************************************************
* @purpose  Returns the first configured APL
*
* @param    pName    (output) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE 
*
* @notes pName must be at least L7_MAX_APL_NAME_SIZE+1 in length
*
* @end
*********************************************************************/
L7_RC_t userMgrAPLListGetFirst(L7_ACCESS_LINE_t line,
                               L7_ACCESS_LEVEL_t level,
                               L7_char8 *pName);

/*********************************************************************
* @purpose  Gets the APL following the specified name in the APL list
*
* @param    pName    pointer to current APL name   
* @param    pNextName pointer to next APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if current APL name does not correspond to a 
*              configured APL or if the end of the list has been reached.
*
* @notes pName must be NULL terminated
* @notes pNextName must be at least L7_MAX_APL_NAME_SIZE+1 in length
*
* @end
*********************************************************************/
L7_RC_t userMgrAPLListGetNext(L7_ACCESS_LINE_t line,
                                L7_ACCESS_LEVEL_t level,
                                L7_char8 *pName,
                                L7_char8 *pNextName);

/*********************************************************************
* @purpose  Adds an authentication method to an APL
*
* @param    pName    (input) pointer to APL name   
* @param    order    (input) order this method should be attempted in the APL   
* @param    method   (input) authentication method value   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if order is >= L7_MAX_AUTH_METHODS or
*                      if method is not recognized or
*                      if name does not match a configured APL or
*                      if the APL is default (which is not changeable)
*
* @notes Order begins with 0  
* @notes To remove an auth method, issue a set with L7_AUTH_METHOD_UNDEFINED.
*        All auth methods will be attempted in the APL until a definitive
*        response is found.  If there is an UNDEFINED entry in the middle of 
*        the APL, that entry will be skipped and the remaining entries will 
*        be used.  
*       
* @end
*********************************************************************/
L7_RC_t userMgrAPLAuthMethodSet(L7_ACCESS_LINE_t line,
                                  L7_ACCESS_LEVEL_t level,
                                  L7_char8 *name,
                                  L7_uint32 order,
                                  L7_USER_MGR_AUTH_METHOD_t method);


/*********************************************************************
* @purpose  Returns the authentication method at the specified index in 
*           the APL
*
* @param    pName    (input) pointer to APL name   
* @param    order    (input) order this method should be attempted in the APL   
* @param    pMethod   (output) authentication method value   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if order is >= L7_MAX_AUTH_METHODS or
*                      if method is not recognized or
*                      if name does not match a configured APL
*
* @notes Order begins with 0  
* @notes The authentication methods should be attempted in order
*       
* @end
*********************************************************************/
L7_RC_t userMgrAPLAuthMethodGet(L7_ACCESS_LINE_t line,
                                  L7_ACCESS_LEVEL_t level,
                                  L7_char8 *name,
                                  L7_uint32 order,
                                  L7_USER_MGR_AUTH_METHOD_t *pMethod);

/*********************************************************************
* @purpose  Assign an APL to a user for a specific component
*
* @param    pUser    (input) pointer to user name   
* @param    component (input)ID of component to use APL   
* @param    pAPL     (input) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not a configured APL or
*                      if user name is not a configured user or
*                      if component id is not a component that handles APLs or
*                      if the user id is 'admin' (which is not changeable)
*
* @notes All users will always have an APL configured for all components that
*        expect them.  Therefore, APL assignments can not be removed just re-set.
* @notes APL name must be NULL terminated.
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLUserSet(L7_char8 *pUser,L7_COMPONENT_IDS_t component, L7_char8 *pAPL );

/*********************************************************************
* @purpose  Return an APL assigned to a user for a specific component
*
* @param    pUser    (input ) pointer to user name   
* @param    component (input) ID of component to use APL   
* @param    pAPL     (output) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes If the user is not configured, the APL for non-configured users
*        is returned.  Therefore, this function can not be used to determine
*        if a user is configured locally.
*
* @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
*        value set will be NULL terminated.
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLUserGet(L7_char8 *pUser,L7_COMPONENT_IDS_t component, L7_char8 *pAPL );

/*********************************************************************
* @purpose  Return an APL assigned to a user (by index) for a specific component 
*
* @param    index    (input ) index of user in login structure
* @param    component (input) ID of component to use APL   
* @param    pAPL     (output) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if the index is not valid or
*                       if the component is not valid
*
* @notes This function will only return the APL for the locally configured
*        user at the specified index.
*
* @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
*        value set will be NULL terminated.
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLUserGetIndex(L7_uint32 index, L7_COMPONENT_IDS_t component, L7_char8 *pAPL );

/*********************************************************************
* @purpose  Assign an APL for nonconfigured users for a specific component
*
* @param    component (input)ID of component to use APL   
* @param    pAPL     (input) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not a configured APL or
*                      if component id is not a component that handles APLs
*
* @notes pAPL must be NULL terminated
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLNonConfiguredUserSet( L7_COMPONENT_IDS_t component, L7_char8 *pAPL );

/*********************************************************************
* @purpose  Return the APL assigned to nonconfigured users for a specific component
*
* @param    component (input) ID of component to use APL   
* @param    pAPL     (output) pointer to APL name   
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLNonConfiguredUserGet( L7_COMPONENT_IDS_t component, L7_char8 *pAPL );

/*********************************************************************
* @purpose  Return the first user in the login structure using the specified APL
*
* @param    pAPLName (input) ptr to APL name   
* @param    pUserName (output) ptr to user name
* @param    pComponent (output) ptr to component
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if there are no users assigned this APL or
*                       if the APL name is not configured
*
* @notes If the nonconfigured user is assigned this APL, the user name is set
*        with text that indicates non-configured user.
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLAllUsersGetFirst(L7_char8 *pAPLName, L7_char8 *pUserName, L7_uint32 *pComponent );

/*********************************************************************
* @purpose  Return the first user in the login structure using the specified APL
*
* @param    pAPLName (input) ptr to APL name   
* @param    pCurrentUserName (input) ptr to user name from getFirst or priorGetNext
* @param    currentComponent (input) component ID from getFirst or priorGetNext
* @param    pNextUserName (output) ptr to next user name
* @param    pNextComponent (output) ptr to next component ID
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if there are no more users assigned this APL.
*           L7_ERROR  if the APL name is not configured or if the specified
*                     user name or component id is not valid
*
* @notes If the nonconfigured user is assigned this APL, the user name is set
*        with text that indicates non-configured user.
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLAllUsersGetNext(L7_char8 *pAPLName, 
                          L7_char8 *pCurrentUserName, L7_uint32 currentComponent,
                          L7_char8 *pNextUserName, L7_uint32 *pNextComponent );

/*********************************************************************
* @purpose  Reset the APL at the specified index
*
* @param    index (input) index into APLs structure
*
* @returns  void
*
* @notes 
*       
* @end
*********************************************************************/
L7_RC_t userMgrAPLInit(L7_ACCESS_LINE_t line,
                       L7_ACCESS_LEVEL_t level,
                       L7_uint32 index);

/*********************************************************************
* @purpose  Reset the methods for the APL at the specified index
*
* @param    index (input) index into APLs structure
*
* @returns  void
*
* @notes Intended for use within User Manager component.  Other 
*        components should call the method that takes an APL name.
*       
* @end
*********************************************************************/
void userMgrAPLResetMethodsIndex(L7_ACCESS_LINE_t line,
                                 L7_ACCESS_LEVEL_t level, 
                                 L7_uint32 index );

/*********************************************************************
* @purpose  Reset the methods for the specified APL
*
* @param    pAPLName (input) name of APL
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes 
*       
* @end
*********************************************************************/
L7_RC_t userMgrAPLResetMethods(L7_ACCESS_LINE_t line,
                               L7_ACCESS_LEVEL_t level,
                               L7_char8 *pAPLName);

/*********************************************************************
* @purpose  Return the index of the specified APL in the APL list
*
* @param    pAPLName (input) name of APL
* @param    pIndex (output) ptr to APL index
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes 
*       
* @end
*********************************************************************/
L7_uint32 
userMgrAPLIndexGet(L7_char8 * pAPLName, L7_uint32 *pIndex );

/*********************************************************************
* @purpose  Return an APL assigned to a line for a level
*
* @param    pUser    (input ) pointer to user name
* @param    component (input) ID of component to use APL
* @param    pAPL     (output) pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes If the user is not configured, the APL for non-configured users
*        is returned.  Therefore, this function can not be used to determine
*        if a user is configured locally.
*
* @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
*        value set will be NULL terminated.
*
* @end
*********************************************************************/
L7_uint32 userMgrAPLLineGet(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_char8 *pAPL);

/*********************************************************************
* @purpose  Assign an APL to a line for a level
*
* @param    pUser    (input) pointer to user name
* @param    component (input)ID of component to use APL
* @param    pAPL     (input) pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not a configured APL or
*                      if user name is not a configured user or
*                      if component id is not a component that handles APLs or
*                      if the user id is 'admin' (which is not changeable)
*
* @notes All users will always have an APL configured for all components that
*        expect them.  Therefore, APL assignments can not be removed just re-set.
* @notes APL name must be NULL terminated.
*      
* @end
*********************************************************************/
L7_RC_t  userMgrAPLLineSet(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_char8 *pAPL);

/*********************************************************************
* @purpose  Return the index of the specified APL in the APL list
*
* @param    pAPLName (input) name of APL
* @param    pIndex (output) ptr to APL index
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 userMgrAuthListIndexGet(L7_ACCESS_LINE_t line,
                             L7_ACCESS_LEVEL_t level,
                             L7_char8 * pAPLName,
                             L7_uint32 *pIndex);

/*********************************************************************
* @purpose  Removes the methods from a specific APL
*
* @param    UnitIndex (input) the unit for this operation
* @param    authMode <input>  the authentication Mode for this operation
* @param    pName    pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL does not exist or users have this APL assigned
*             and therefore it cannot be deleted or if APL name is not valid.
*
* @notes  pName must be null terminated.
*
* @end
*********************************************************************/
L7_RC_t userMgrAPLAuthMethodsRemove(L7_ACCESS_LINE_t accessLine,
                                    L7_ACCESS_LEVEL_t accessLevel,
                                    L7_char8 *pName);

/* TODO fn header */
L7_BOOL userMgrAPLInUse(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_uint32 index);

L7_uint32
userMgrAPLIndexGet(L7_char8 * pAPLName, L7_uint32 *pIndex );

/* TO DO Function Header */
L7_RC_t userMgrAPLGetNameByIndex(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t access, 
								 L7_uint32 sIndex, L7_char8* namebuf);

#endif /* USER_MGR__APL_H */
