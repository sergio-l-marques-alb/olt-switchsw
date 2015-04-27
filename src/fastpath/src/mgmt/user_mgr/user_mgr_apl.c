
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_apl.c
*
* @purpose   user manager functions related to APLs
*
* @component userMgr component
*
* @comments  none
*
* @create    05/8/2003
*
* @author    jflanagan
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "l7_common.h"
#include "cli_web_exports.h"
#include "user_manager_exports.h"
#include "user_mgr.h"
#include "user_mgr_util.h"
#include "default_cnfgr.h"
#include "user_mgr_apl.h"

extern userMgrCfgData_t userMgrCfgData;

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
                         L7_char8 *pName)
{
    L7_uint32 i;
    L7_RC_t   rc;
    APL_t *authList;

    /* Verify the name is valid */
    if (strlen(pName) > L7_MAX_APL_NAME_SIZE || strlen(pName)==0) 
    {
      return L7_FAILURE;
    }

    /* Look for existing name in the APL list. If the entry exists, overwrite it. */
    if (userMgrAuthListIndexGet(line, level, pName, &i) != L7_SUCCESS)
    {
      /* Find the first empty slot in the APL list */
      rc = userMgrAuthListIndexGet(line, level, "\0", &i);
      if (rc != L7_SUCCESS)
      {
        return rc;
      }
    }
    authList = userMgrAuthListFromIndexGet(line, level, i);
    if (authList == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    memset(authList->name, 0, sizeof(authList->name));
    strcpy(authList->name, pName);


    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;

}

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
                         L7_char8 *pName)
{
    L7_uint32 i;

    /* Verify the name is valid */
    if (strlen(pName) > L7_MAX_APL_NAME_SIZE || strlen(pName) == 0) 
    {
      return L7_FAILURE;
    }
    
    /* Find the entry in the APL structure */
    if (userMgrAuthListIndexGet(line, level, pName, &i) == L7_FAILURE) /*||
        i == 0 )  the default APL can not be deleted */
    {
      return L7_FAILURE;
    }

    /* Determine if any lines are assigned this APL, 
       Default lists will not be deleted, just put back to default methods. */
    if ((userMgrAPLDefaultCheck(line, level, pName) != L7_TRUE) &&
        (userMgrAPLInUse(line, level, i) == L7_TRUE))
    {
      /* at least one user is assigned this APL */
      return L7_FAILURE;
    }                    

    userMgrAPLInit(line, level, i);

    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}

/* TODO fn header */
L7_BOOL userMgrAPLInUse(L7_ACCESS_LINE_t line,
                        L7_ACCESS_LEVEL_t level,
                        L7_uint32 index)
{
  APL_t *authList = L7_NULLPTR;

  if (line == ACCESS_LINE_HTTP || line == ACCESS_LINE_HTTPS ||
      line == ACCESS_LINE_DOT1X)
  {
    /* There is only one list for these access lines: the default one
       and it's always in use. */
    return L7_TRUE;
  }
  if (level == ACCESS_LEVEL_LOGIN)
  {
    authList = &userMgrCfgData.authListInfo.loginAuthLists[index];
  }
  else if (level == ACCESS_LEVEL_ENABLE)
  {
    authList = &userMgrCfgData.authListInfo.enableAuthLists[index];
  }
  else
  {
    return L7_FALSE; /* LOG MSG ? TODO */
  }

  if (authList->inUse == L7_TRUE)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

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
                                  L7_char8 *pName )
{
  L7_uint32 i;

  /* Verify the name is valid */
  if (strlen(pName) > L7_MAX_APL_NAME_SIZE || strlen(pName) == 0) 
      return L7_FALSE;
  
  /* Find the entry in the APL structure */
  if (userMgrAuthListIndexGet(line, level, pName, &i) == L7_FAILURE )
      return L7_FALSE;

  if (level == ACCESS_LEVEL_LOGIN)
  { 
    if (i >= 0 && i <= 1) /* 2 default lists for LOGIN : CONSOLE_DEFAULT and NETWORK_DEFAULT */
    {
      return L7_TRUE;
    }
    else
    {
      return L7_FALSE;
    }
  }
  if (level == ACCESS_LEVEL_ENABLE)
  {
    if (i == 0) /* 1 default lists for ENABLE */
    {
      return L7_TRUE;
    }
    else
    {
      return L7_FALSE;
    }
  }

  if (i == 0) 
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

#if 0
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
userMgrAPLListGetIndex(L7_uint32 index, L7_char8 *pName )
{
    if (index >= FD_MAX_APL_COUNT || strlen(userMgrCfgData.APLs[index].name) == 0)
        return L7_FAILURE;

    memcpy( pName, userMgrCfgData.APLs[index].name, L7_MAX_APL_NAME_SIZE+1 );

    return L7_SUCCESS;
}

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
userMgrAPLListGetCount(L7_uint32 *pCount )
{
    L7_uint32 i;
    L7_uint32 count = 0;

    for (i=0; i<FD_MAX_APL_COUNT; i++)
    {
        if (strlen(userMgrCfgData.APLs[i].name) != 0)
        {
            count++;
        }
    }

    *pCount = count;

    return L7_SUCCESS;
}
#endif
L7_RC_t userMgrAPLListGetMaxCount(L7_ACCESS_LINE_t line,
                                  L7_ACCESS_LEVEL_t level,
                                  L7_uint32 *pCount)
{

    switch (line)
    { /* Index argument is currently ignored for HTTP/HTTPS/DOT1X as they have only one list */
      case ACCESS_LINE_HTTP:
        *pCount = L7_HTTP_MAX_AUTH_METHODS;
        break;
      case ACCESS_LINE_HTTPS:
        *pCount = L7_HTTPS_MAX_AUTH_METHODS;
        break;
      case ACCESS_LINE_DOT1X:
        *pCount = L7_DOT1X_MAX_AUTH_METHODS;
        break;
      case ACCESS_LINE_CONSOLE:
      case ACCESS_LINE_TELNET:
      case ACCESS_LINE_SSH:
      case ACCESS_LINE_CTS:
          /* The line has to be CTS: Console/Telnet/SSH as login and enable are 
             applicable to any of these lines */
        if (level == ACCESS_LEVEL_LOGIN)
        {  
          *pCount = L7_LOGIN_MAX_AUTH_METHODS;
        }
        else
        {
          *pCount = L7_ENABLE_MAX_AUTH_METHODS;
        }
        break;
      default: /* Unacceptable value of line */ 
        return L7_FAILURE;
        break;
    }
    return L7_SUCCESS;
}

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
                                 L7_char8 *pName )
{
  switch (line)
  {
    case ACCESS_LINE_HTTP:
      /* HTTP access has only one list */
      memcpy(pName, userMgrCfgData.authListInfo.httpAuthLists[0].name, L7_MAX_APL_NAME_SIZE+1);
      break;
    case ACCESS_LINE_HTTPS:
      /* HTTPS access has only one list */
      memcpy(pName, userMgrCfgData.authListInfo.httpsAuthLists[0].name, L7_MAX_APL_NAME_SIZE+1);
      break;
    case ACCESS_LINE_DOT1X:
      /* DOT1X access has only one list */
      memcpy(pName, userMgrCfgData.authListInfo.dot1xAuthLists[0].name, L7_MAX_APL_NAME_SIZE+1);
      break;
    default:
      if (level == ACCESS_LEVEL_LOGIN)
      { 
        /* First list is the default list Confirm TODO*/
        memcpy(pName, userMgrCfgData.authListInfo.loginAuthLists[0].name, L7_MAX_APL_NAME_SIZE+1);
      }
      else
      {
        /* First list is the default list Confirm TODO*/
        memcpy(pName, userMgrCfgData.authListInfo.enableAuthLists[0].name, L7_MAX_APL_NAME_SIZE+1);
      }
      break;
  }
  return L7_SUCCESS;
}

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
                                L7_char8 *pNextName)
{
  L7_uint32 i, j = 0, maxListCount = 0;

  if (line == ACCESS_LINE_HTTP || line == ACCESS_LINE_HTTPS ||
      line == ACCESS_LINE_DOT1X)
  {
    /* There could be only one list */
    return L7_FAILURE;
  }

  if (line != ACCESS_LINE_CTS && 
      line != ACCESS_LINE_CONSOLE && 
      line != ACCESS_LINE_TELNET && 
      line != ACCESS_LINE_SSH)
  {
    /* The line has to be CTS: Console/Telnet/SSH as login and enable are 
       applicable to any of these lines */
    return L7_FAILURE;
  }

  /* Find the previous entry */
  if (userMgrAuthListIndexGet(line, level, pName, &i) == L7_FAILURE) 
  {      
    return L7_FAILURE;
  }

  if (level == ACCESS_LEVEL_LOGIN)
  {
    maxListCount = L7_MAX_LOGIN_AUTHLIST_COUNT;
    /* Find next APL starting with the previous entry */
    for (j = i + 1; j < maxListCount; j++) 
    {
      if (strlen(userMgrCfgData.authListInfo.loginAuthLists[j].name) != 0)
      {
        memcpy(pNextName, userMgrCfgData.authListInfo.loginAuthLists[j].name, L7_MAX_APL_NAME_SIZE + 1);
        break;
      }
    }
  }
  else if (level == ACCESS_LEVEL_ENABLE)
  {
    maxListCount = L7_MAX_ENABLE_AUTHLIST_COUNT;
    /* Find next APL starting with the previous entry */
    for (j = i + 1; j < maxListCount; j++) 
    {
      if (strlen(userMgrCfgData.authListInfo.enableAuthLists[j].name) != 0)
      {
        memcpy(pNextName, userMgrCfgData.authListInfo.enableAuthLists[j].name, L7_MAX_APL_NAME_SIZE + 1);
        break;
      }
    }
  }

  if (maxListCount == j) 
  {
    /* Next entry not found */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

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
                                L7_char8 *pName, 
                                L7_uint32 order, 
                                L7_USER_MGR_AUTH_METHOD_t method)
{
  APL_t *authList;

  /* Verify the name is valid */
  if (strlen(pName) > L7_MAX_APL_NAME_SIZE || strlen(pName)==0) 
      return L7_FAILURE;

  authList = userMgrAuthListFromNameGet(line, level, pName);
  if (authList == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  authList->methodList[order] = method;

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;

}

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
                                L7_USER_MGR_AUTH_METHOD_t *pMethod)
{
  APL_t *authList;

  authList = userMgrAuthListFromNameGet(line, level, name);
  if (authList == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *pMethod = authList->methodList[order];
  return L7_SUCCESS;
}

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
userMgrAPLUserSet(L7_char8 *pUser,L7_COMPONENT_IDS_t component, L7_char8 *pAPL )
{
    L7_uint32 index;
    L7_uint32 i;

    /* Find the login entry for this user */
    if (userMgrLoginIndexGet(pUser, &index) == L7_FAILURE || index == 0)
        return L7_FAILURE;

#if 0
    /* Validate the APL */
    if (userMgrAPLIndexGet(pAPL, &i) == L7_FAILURE)
        return L7_FAILURE;
#endif

    /* Find the APL entry for this component */
    for (i=0; i<L7_APL_COMPONENT_COUNT; i++)
    {
        if (userMgrCfgData.systemLogins[index].userAuth[i].componentId == component)
        {
          memset( userMgrCfgData.systemLogins[index].userAuth[i].APLName, 0,
                  sizeof( userMgrCfgData.systemLogins[index].userAuth[i].APLName ));
          osapiStrncpySafe( userMgrCfgData.systemLogins[index].userAuth[i].APLName, pAPL,
                            sizeof(userMgrCfgData.systemLogins[index].userAuth[i].APLName) );
          break;
        }
    }

    if (L7_APL_COMPONENT_COUNT == i)
        return L7_FAILURE;


    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}

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
userMgrAPLUserGet(L7_char8 *pUser,L7_COMPONENT_IDS_t component, L7_char8 *pAPL )
{
    L7_uint32 index;
    L7_uint32 i;

    /* Find the login entry for this user */
    if (userMgrLoginIndexGet(pUser, &index) == L7_FAILURE)
    {
        /* Return the APL for the non-configured user for this component */
        return userMgrAPLNonConfiguredUserGet( component, pAPL );
    }

    /* Find the APL entry for this component */
    for (i=0; i<L7_APL_COMPONENT_COUNT; i++)
    {
        if (userMgrCfgData.systemLogins[index].userAuth[i].componentId == component)
        {
          memset( pAPL, 0, L7_MAX_APL_NAME_SIZE+1);
          osapiStrncpySafe( pAPL, userMgrCfgData.systemLogins[index].userAuth[i].APLName, L7_MAX_APL_NAME_SIZE+1);
          break;
        }
    }

    if (L7_APL_COMPONENT_COUNT == i)
        return L7_FAILURE;

    return L7_SUCCESS;

}

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
userMgrAPLUserGetIndex(L7_uint32 index,L7_COMPONENT_IDS_t component, L7_char8 *pAPL )
{
    L7_uint32 i;
    L7_uchar8 userName[L7_LOGIN_SIZE];

    /* Validate the index and the user  */
    if (( userMgrLoginUserNameGet(index, userName) == L7_FAILURE ) ||
        ( strlen( userName ) == 0 ) )
        return L7_FAILURE;


    /* Find the APL entry for this component */
    for (i=0; i<L7_APL_COMPONENT_COUNT; i++)
    {
        if (userMgrCfgData.systemLogins[index].userAuth[i].componentId == component)
        {
          memset( pAPL, 0, L7_MAX_APL_NAME_SIZE+1);
          osapiStrncpySafe( pAPL, userMgrCfgData.systemLogins[index].userAuth[i].APLName, L7_MAX_APL_NAME_SIZE+1);
          break;
        }
    }

    if (L7_APL_COMPONENT_COUNT == i)
        return L7_FAILURE;

    return L7_SUCCESS;

}

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
userMgrAPLNonConfiguredUserSet( L7_COMPONENT_IDS_t component, L7_char8 *pAPL )
{
    L7_uint32 i;

#if 0
    /* Validate the APL */
    if (userMgrAPLIndexGet(pAPL, &i) == L7_FAILURE)
        return L7_FAILURE;
#endif

    /* Find the entry for this component in the nonConfigured struct */
    for (i=0; i<L7_APL_COMPONENT_COUNT; i++)
    {
        if (userMgrCfgData.nonConfiguredUserAuth[i].componentId == component)
        {
            memset( userMgrCfgData.nonConfiguredUserAuth[i].APLName, 0,
                    sizeof( userMgrCfgData.nonConfiguredUserAuth[i].APLName) );
            osapiStrncpySafe( userMgrCfgData.nonConfiguredUserAuth[i].APLName, pAPL,
                              sizeof(userMgrCfgData.nonConfiguredUserAuth[i].APLName));
            break;

        }
    }

    if (L7_APL_COMPONENT_COUNT == i)
        return L7_FAILURE;

    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}

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
userMgrAPLNonConfiguredUserGet(L7_COMPONENT_IDS_t component, L7_char8 *pAPL )
{
    L7_uint32 i;

    /* Find the entry for this component in the nonConfigured struct */
    for (i=0; i<L7_APL_COMPONENT_COUNT; i++)
    {
        if (userMgrCfgData.nonConfiguredUserAuth[i].componentId == component)
        {
            memset( pAPL, 0,
                    sizeof( userMgrCfgData.nonConfiguredUserAuth[i].APLName) );
            osapiStrncpySafe( pAPL, userMgrCfgData.nonConfiguredUserAuth[i].APLName, L7_MAX_APL_NAME_SIZE+1);
            break;

        }
    }

    if (L7_APL_COMPONENT_COUNT == i)
        return L7_FAILURE;

    return L7_SUCCESS;
}

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
userMgrAPLAllUsersGetFirst(L7_char8 *pAPLName, L7_char8 *pUserName, L7_uint32 *pComponent )
{
    L7_uint32 i, j;

#if 0
    /* Validate the APL */
    if (userMgrAPLIndexGet(pAPLName, &i) == L7_FAILURE)
        return L7_FAILURE;
#endif

    /* Start at the first login and the first component */
    for (i=0; i<L7_MAX_LOGINS; i++)
    {
        for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
        {
            if (userMgrNoCaseCompare(userMgrCfgData.systemLogins[i].userAuth[j].APLName,
                                     pAPLName ) == L7_TRUE  &&
                strlen( userMgrCfgData.systemLogins[i].loginName ) != 0 )
            {
                memcpy( pUserName, userMgrCfgData.systemLogins[i].loginName,
                        sizeof( userMgrCfgData.systemLogins[i].loginName ) );
                *pComponent = userMgrCfgData.systemLogins[i].userAuth[j].componentId;
                break;
            }
        }
        if (j < L7_APL_COMPONENT_COUNT)
            break;
    }

    if (L7_MAX_LOGINS == i)
    {
        /*
         * Nothing found in the login structure.
         * Check the nonconfigured users structure.
         */
        for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
        {
            if ( userMgrNoCaseCompare( userMgrCfgData.nonConfiguredUserAuth[j].APLName,
                                       pAPLName ) == L7_TRUE )
            {
                osapiStrncpySafe( pUserName, L7_USER_MGR_DEFAULT_USER_STRING, sizeof(userMgrCfgData.systemLogins[i].loginName) );
                *pComponent = userMgrCfgData.nonConfiguredUserAuth[j].componentId;
                break;
            }

        }

        if (L7_APL_COMPONENT_COUNT == j)
            return L7_FAILURE;
    }

    return L7_SUCCESS;
}

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
                          L7_char8 *pNextUserName, L7_uint32 *pNextComponent )
{
    L7_uint32 i = 0, j, index, componentIndex;
    L7_BOOL nonconfiguredUserFlag = L7_FALSE;

    /* TO DO: Clean up this function!!! */
#if 0
    /* Validate the APL */
    if (userMgrAPLIndexGet(pAPLName, &i) == L7_FAILURE)
        return L7_FAILURE;
#endif

    /* Find the login entry for this user */
    if (userMgrLoginIndexGet(pCurrentUserName, &index) == L7_FAILURE)
    {
        /*
         * Either this is an invalid user or this is the nonconfigured user.
         */
        if (userMgrNoCaseCompare(pCurrentUserName, L7_USER_MGR_DEFAULT_USER_STRING) == L7_TRUE)
            nonconfiguredUserFlag = L7_TRUE;
        else
            return L7_FAILURE;
    }

    if (nonconfiguredUserFlag == L7_FALSE)
    {
        /* Look for the matching current entry in the configured users struct */

        for (componentIndex = 0; componentIndex<L7_APL_COMPONENT_COUNT; componentIndex++)
        {
            if (userMgrCfgData.systemLogins[index].userAuth[componentIndex].componentId == currentComponent )
            {
                /* This is the match to start with */
                break;
            }
        }

        if (componentIndex < L7_APL_COMPONENT_COUNT)
        {
            /* The previous match was a configured user.  Begin there. */
            /* Start at the current login but ignore the current component */
            for (i=index; i<L7_MAX_LOGINS; i++)
            {
                for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
                {
                    if (userMgrNoCaseCompare(userMgrCfgData.systemLogins[i].userAuth[j].APLName,
                                             pAPLName ) == L7_TRUE)
                    {
                        /* Verify this match is beyond the "current" match passed in
                         * and it is not an empty login field.
                         */
                        if (!(i == index && j <= componentIndex ) &&
                            strlen(userMgrCfgData.systemLogins[i].loginName) != 0 )
                        {
                            memcpy( pNextUserName, userMgrCfgData.systemLogins[i].loginName,
                                    sizeof( userMgrCfgData.systemLogins[i].loginName ) );
                            *pNextComponent = userMgrCfgData.systemLogins[i].userAuth[j].componentId;
                            break;
                        }
                    }
                }
                if (j < L7_APL_COMPONENT_COUNT)
                    break;
            }
        }


        if (componentIndex == L7_APL_COMPONENT_COUNT || L7_MAX_LOGINS == i)
        {
            /*
             * Nothing found in the login structure.
             * Check the nonconfigured users structure.
             */
            for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
            {
                if ( userMgrNoCaseCompare( userMgrCfgData.nonConfiguredUserAuth[j].APLName,
                                           pAPLName ) == L7_TRUE )
                {
                    memset( pNextUserName, 0,
                            sizeof( userMgrCfgData.systemLogins[i].loginName ) );
                    osapiStrncpySafe( pNextUserName, L7_USER_MGR_DEFAULT_USER_STRING, sizeof( userMgrCfgData.systemLogins[i].loginName ) );
                    *pNextComponent = userMgrCfgData.nonConfiguredUserAuth[j].componentId;
                    break;
                }

            }

            if (L7_APL_COMPONENT_COUNT == j)
                return L7_FAILURE;
        }
    } /* end if current user was in configured user structure */
    else
    {
        /* "current" user was a nonconfigured user.  Assume the configured user
         * struct has already been searched.
         */
        for (componentIndex = 0; componentIndex<L7_APL_COMPONENT_COUNT; componentIndex++)
        {
            if (userMgrCfgData.nonConfiguredUserAuth[componentIndex].componentId == currentComponent )
            {
                /* This is the match to start with */
                break;
            }
        }

        if (componentIndex < L7_APL_COMPONENT_COUNT)
        {
            for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
            {
                if ( userMgrNoCaseCompare( userMgrCfgData.nonConfiguredUserAuth[j].APLName,
                                           pAPLName ) == L7_TRUE &&
                     j > componentIndex )
                {
                    osapiStrncpySafe( pNextUserName, L7_USER_MGR_DEFAULT_USER_STRING,
                                      sizeof( userMgrCfgData.systemLogins[i].loginName ) );
                    *pNextComponent = userMgrCfgData.nonConfiguredUserAuth[j].componentId;
                    break;
                }

            }

            if (L7_APL_COMPONENT_COUNT == j)
                return L7_FAILURE;
        }
        else
        {
            return L7_FAILURE;
        }

    }

    return L7_SUCCESS;
}

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
                       L7_uint32 index)
{
  APL_t *authList = userMgrAuthListFromIndexGet(line, level, index);

  if (authList == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  userMgrAPLResetMethodsIndex(line, level, index);

  return L7_SUCCESS;
}

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
                                 L7_char8 *pAPLName)
{
    L7_uint32 i;

    if (userMgrAuthListIndexGet(line, level, pAPLName, &i) == L7_FAILURE)
        return L7_FAILURE;

    userMgrAPLResetMethodsIndex(line, level, i);

    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the index of the specified APL in the APL list
*
* @param    pAPLName (input) name of APL
* @param    pIndex (output) ptr to APL index
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes This function can be used to find the first blank entry in the
*        APL list.  Therefore, a name of size 0 is valid.
*
* @end
*********************************************************************/
L7_uint32
userMgrAPLIndexGet(L7_char8 * pAPLName, L7_uint32 *pIndex )
{
#if 0
    L7_uint32 i;

    /* Verify the name is valid */
    if (strlen(pAPLName) > L7_MAX_APL_NAME_SIZE)
        return L7_FAILURE;

    for (i=0; i<FD_MAX_APL_COUNT; i++)
    {
        if (userMgrNoCaseCompare(userMgrCfgData.APLs[i].name, pAPLName) == L7_TRUE)
            break;
    }

    if (FD_MAX_APL_COUNT == i)
    {
      if (strcmp(pAPLName, "\0") != 0)
        return L7_FAILURE;
      else
        return L7_TABLE_IS_FULL;
    }

    *pIndex = i;
#endif
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the index of the specified APL in the APL list
*
* @param    pAPLName (input) name of APL
* @param    pIndex (output) ptr to APL index
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes This function can be used to find the first blank entry in the
*        APL list.  Therefore, a name of size 0 is valid.
*       
* @end
*********************************************************************/
L7_RC_t userMgrAuthListIndexGet(L7_ACCESS_LINE_t line,
                                L7_ACCESS_LEVEL_t level, 
                                L7_char8 *pAPLName, 
                                L7_uint32 *pIndex)
{
  L7_uint32 i;
  L7_BOOL matchFound = L7_FALSE;

  /* Verify the name is valid */
  if (strlen(pAPLName) > L7_MAX_APL_NAME_SIZE) 
  {
    return L7_FAILURE;
  }
  /* First take care of HTTP, HTTPS, and DOT1X lines */
  if (line == ACCESS_LINE_HTTP || line == ACCESS_LINE_HTTPS || 
      line == ACCESS_LINE_DOT1X)
  {
    /* only one list for these access lines */
    if (strcmp(pAPLName, "\0") != L7_NULL)
    {
      return L7_FAILURE;
    }
    *pIndex = 0;
    return L7_SUCCESS;
  }

  /* Now take care of LOGIN and ENABLE levels for the other lines viz. CONSOLE, TELNET, SSH */
  if (line != ACCESS_LINE_CTS && 
      line != ACCESS_LINE_CONSOLE && 
      line != ACCESS_LINE_TELNET && 
      line != ACCESS_LINE_SSH)
  {
    /* The line has to be CTS for Console/Telnet/SSH as login and enable are 
       applicable to any of these lines */
    return L7_FAILURE;
  }

  if (level == ACCESS_LEVEL_LOGIN)
  {
    for (i = 0; i < L7_MAX_LOGIN_AUTHLIST_COUNT; i++) 
    {
      if (strcmp(userMgrCfgData.authListInfo.loginAuthLists[i].name, pAPLName) == L7_NULL)
      {
        matchFound = L7_TRUE;
        break;
      }
    }
  }
  else /* ENABLE access level */
  {
    for (i = 0; i < L7_MAX_ENABLE_AUTHLIST_COUNT; i++) 
    {
      if (strcmp(userMgrCfgData.authListInfo.enableAuthLists[i].name, pAPLName) == L7_NULL)
      {
        matchFound = L7_TRUE;
        break;
      }
    }
  }

  if (matchFound == L7_FALSE)
  {
    if (strcmp(pAPLName, "\0") != 0)
    {
      return L7_FAILURE;
    }
    else
    {
      return L7_TABLE_IS_FULL;
    }
  }

  *pIndex = i;
  return L7_SUCCESS;
}

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
L7_RC_t  userMgrAPLLineSet(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_char8 *pName )
{
  L7_uint32 i;
  APL_t *authList;
  L7_BOOL inUse = L7_FALSE;
  L7_char8    strAuthListName[L7_MAX_APL_NAME_SIZE + 1];

  memset(strAuthListName, 0, sizeof(strAuthListName));

  if (line < L7_ACCESS_LINE_NUM && level < L7_ACCESS_LEVEL_NUM)
  {
    if (userMgrAuthListDependencyCheck(line, level, pName) != L7_SUCCESS)
    {
      return L7_DEPENDENCY_NOT_MET;
    }
    /* Check if the APL with that name exists */
    if (userMgrAuthListIndexGet(line, level, pName, &i) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    /* store away the old name */
    osapiStrncpySafe(strAuthListName, userMgrCfgData.authListInfo.authListName[line][level], sizeof(strAuthListName));

    /* Set that APL in use */
    authList = userMgrAuthListFromIndexGet(line, level, i);
    if (authList != L7_NULLPTR)
    {
      osapiStrncpySafe(authList->name,pName,sizeof(authList->name));
      osapiStrncpySafe(userMgrCfgData.authListInfo.authListName[line][level], authList->name, L7_MAX_APL_NAME_SIZE + 1);
      authList->inUse = L7_TRUE;
      userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    else
    {
      return L7_FAILURE;
    }

    /* Reset 'in use' for the former APL for this line */
    for (i = 0; i < L7_ACCESS_LINE_NUM; i++) 
    {
      if (strcmp(userMgrCfgData.authListInfo.authListName[i][level], strAuthListName) == L7_NULL)
      {
        inUse = L7_TRUE;
      }
    }
    if (inUse == L7_FALSE)
    {
      authList = userMgrAuthListFromNameGet(line, level, strAuthListName);
      if (authList != L7_NULLPTR)
      {
        authList->inUse = L7_FALSE;
      }
    }

  }
  else
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check depedency of List assigned to console
*
* @param    line   (input) pointer to user name
* @param    level  (input)ID of component to use APL
* @param    pName   (input) pointer to APL name
*
* @returns             L7_DEPENDENCY_NOT_MET  
*                  if login list assigned to console doesnot include 
*                     local or enbale method.
*                  if enable list assigned to console doesnot include 
*                    enbale method.
*
* @end
*********************************************************************/

L7_RC_t userMgrAuthListDependencyCheck(L7_ACCESS_LINE_t line,                                        
                                       L7_ACCESS_LEVEL_t level,
                                       L7_char8  *pName)
{
  L7_uint32   i;
  L7_BOOL     flag = L7_FALSE;
  APL_t       *list;

  /* List assigned to console is necessary to met the following conditions
   *  1) Login List  --> Either of the local or enable method should be part of list
   *  2) Enable List --> Enbale method should be part of list
   */

  if (userMgrAuthListIndexGet(line, level, pName, &i) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

    /* Set that APL in use */
  list = userMgrAuthListFromIndexGet(line, level, i);


  if (list != L7_NULLPTR)
  { 
    if (line == ACCESS_LINE_CONSOLE)
    {
      if (level == ACCESS_LEVEL_LOGIN)
      {
        for ( i = 0; i< L7_MAX_AUTH_METHODS; i++)
        {
          if (list->methodList[i] == L7_AUTH_METHOD_LOCAL ||
              list->methodList[i] == L7_AUTH_METHOD_ENABLE)
          {
            /* either of the method 'local' or 'enable' is present*/
            flag = L7_TRUE;
            break;
          }
        }
      }
      else if (level == ACCESS_LEVEL_ENABLE)
      {
        for ( i = 0; i< L7_MAX_AUTH_METHODS; i++)
        {
          if (list->methodList[i] == L7_AUTH_METHOD_ENABLE)
          {
            flag = L7_TRUE;
            break;
          }
        }
      }
      
      if (flag == L7_FALSE)
      {
       return L7_DEPENDENCY_NOT_MET;
      }
    } /* end of if (line == ....*/
  }/* if (list != L7_NULLPTR)*/
  return L7_SUCCESS;
}

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
L7_uint32 userMgrAPLLineGet(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_char8 *pName)
{
  if (line < L7_ACCESS_LINE_NUM && level < L7_ACCESS_LEVEL_NUM)
  {
    memset(pName, 0, L7_MAX_APL_NAME_SIZE+1);
    strcpy(pName, (userMgrCfgData.authListInfo.authListName[line][level]));
  }
  else
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/* TODO fn header */
APL_t *userMgrAuthListFromIndexGet(L7_ACCESS_LINE_t line,
                                   L7_ACCESS_LEVEL_t level,
                                   L7_uint32 index)
{
  /* TODO: validate index */
  APL_t *authList = L7_NULLPTR;

  switch (line)
  { /* Index argument is currently ignored for HTTP/HTTPS/DOT1X as they have only one list */
    case ACCESS_LINE_HTTP:
      authList = &userMgrCfgData.authListInfo.httpAuthLists[0];
      break;
    case ACCESS_LINE_HTTPS:
      authList = &userMgrCfgData.authListInfo.httpsAuthLists[0];
      break;
    case ACCESS_LINE_DOT1X:
      authList = &userMgrCfgData.authListInfo.dot1xAuthLists[0];
      break;
    case ACCESS_LINE_CONSOLE:
    case ACCESS_LINE_TELNET:
    case ACCESS_LINE_SSH:
    case ACCESS_LINE_CTS:
      /* The line has to be CTS for Console/Telnet/SSH as login and enable are 
         applicable to any of these lines */
      if (level == ACCESS_LEVEL_LOGIN)
      {  
        authList = &userMgrCfgData.authListInfo.loginAuthLists[index];
      }
      else
      {
        /* snk - user Mgr modify */ 
        /*authList = &userMgrCfgData.authListInfo.loginAuthLists[index];*/
          authList = &userMgrCfgData.authListInfo.enableAuthLists[index];
      }
      break;
    default: /* not acceptable line value */
      break;
  }
  return authList;
}


/* TODO fn header */
APL_t *userMgrAuthListFromNameGet(L7_ACCESS_LINE_t line,
                                  L7_ACCESS_LEVEL_t level,
                                  L7_char8 *pAPLName)
{
  /* TODO: validate index */
  APL_t *authList = L7_NULLPTR;

  switch (line)
  {/* Name argument is currently ignored for HTTP/HTTPS/DOT1X as they have only one list */
    case ACCESS_LINE_HTTP:
      authList = &userMgrCfgData.authListInfo.httpAuthLists[0];
      break;
    case ACCESS_LINE_HTTPS:
      authList = &userMgrCfgData.authListInfo.httpsAuthLists[0];
      break;
    case ACCESS_LINE_DOT1X:
      authList = &userMgrCfgData.authListInfo.dot1xAuthLists[0];
      break;
    case ACCESS_LINE_CONSOLE:
    case ACCESS_LINE_TELNET:
    case ACCESS_LINE_SSH:
    case ACCESS_LINE_CTS:
      /* The line has to be CTS for Console/Telnet/SSH as login and enable are 
         applicable to any of these lines */
      if (level == ACCESS_LEVEL_LOGIN)
      {  
        L7_uint32 i;
        for (i = 0; i < L7_MAX_LOGIN_AUTHLIST_COUNT; i++) 
        {
          if (strcmp(userMgrCfgData.authListInfo.loginAuthLists[i].name, pAPLName) == L7_NULL)
          {
            authList = &userMgrCfgData.authListInfo.loginAuthLists[i];
          }
        }
      }
      else
      {
        L7_uint32 i;
        for (i = 0; i < L7_MAX_ENABLE_AUTHLIST_COUNT; i++) 
        {
          if (strcmp(userMgrCfgData.authListInfo.enableAuthLists[i].name, pAPLName) == L7_NULL)
          {
            authList = &userMgrCfgData.authListInfo.enableAuthLists[i];
          }
        }
      }
      break;
    default:
      break;
  }
  return authList;
}

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
L7_RC_t userMgrAPLAuthMethodsRemove(L7_ACCESS_LINE_t line,
                                    L7_ACCESS_LEVEL_t level, 
                                    L7_char8 *pName)
{
  L7_uint32 count = 0;
  L7_uint32 i;
  APL_t *authList;

  /* Get the auth list */
  authList = userMgrAuthListFromNameGet(line, level, pName);
  if (authList == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /* Get the maximum number of methods depending on the line an/or level */
  userMgrAPLListGetMaxCount(line, level, &count);

  /* Mark all the methods in the list as undefined which effectively means removed */
  for (i = 0; i < count; i++)
  {
    authList->methodList[i] = L7_AUTH_METHOD_UNDEFINED;
  }
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/* TO DO Function Header */
L7_RC_t userMgrAPLGetNameByIndex(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t access, 
                                 L7_uint32 sIndex, L7_char8* namebuf)
{
    L7_uint32 pIndex;
    L7_char8 tempbuf[L7_MAX_APL_NAME_SIZE]; 
    L7_RC_t rc; 

    memset(namebuf, 0x00, L7_MAX_APL_NAME_SIZE);
    memset(tempbuf, 0x00, L7_MAX_APL_NAME_SIZE);
   
    rc = userMgrAPLListGetFirst(0,access,namebuf);  /* Get First name in the list */
 
    /* Get Index of the first name */
     
    if ( rc == L7_SUCCESS )
       rc = userMgrAuthListIndexGet(line, access, namebuf, &pIndex);

    while ( (rc == L7_SUCCESS) && (sIndex != pIndex) )
    {
        memset(tempbuf,0x00,sizeof(tempbuf));
        osapiStrncpySafe(tempbuf, namebuf, sizeof(tempbuf));
        rc = userMgrAPLListGetNext(line,access,tempbuf,namebuf);
        if ( rc == L7_SUCCESS )
          rc = userMgrAuthListIndexGet(line,access,namebuf,&pIndex);
     }
     return rc;
}

