
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_ports.c
*
* @purpose   user manager functions related to user port access
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
#include "cli_web_exports.h"
#include "user_manager_exports.h"
#include "nimapi.h"
#include "user_mgr.h"
#include "user_mgr_api.h"
#include "user_mgr_ports.h"
#include "user_mgr_cnfgr.h"
#include <stdio.h>


#define USER_MGR_PORT_ALL_ACCESS 0xFFFFFFFF

#define ENABLED_BIT_MASK(x)    (1<<(x))
/* This value must be max(userMgrPortUser_t)!*/
#define DISABLED_BIT_MASK(x)   (4294967295U - (1<<(x)))

extern userMgrCfgData_t userMgrCfgData;

userMgrPortUserCfg_t **userMgrPortOper_g;

/* Local prototypes */
L7_BOOL userMgrPortIsConfigurable(L7_uint32 intIfNum,userMgrPortUserCfg_t **pCfg);

/*********************************************************************
* @purpose Set the access for all users on this port
*
* @param    intIfNum (input) Internal interface number of port
* @param    allow (input) boolean indicating access (TRUE) or not (FALSE)
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface
*
* @notes
*
* @end
*********************************************************************/
L7_uint32
userMgrPortInitialize(L7_uint32 intIfNum, L7_BOOL allow  )
{
  userMgrPortUserCfg_t *pCfg = L7_NULLPTR;

  if (userMgrPortIsConfigurable(intIfNum,&pCfg) == L7_FALSE)
    return L7_FAILURE;
  else
  {
    if (L7_TRUE == allow)
      pCfg->portUsers = USER_MGR_PORT_ALL_ACCESS;
    else
      pCfg->portUsers = 0;

    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

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
userMgrPortInitializeAll( L7_BOOL allow  )
{
  L7_uint32 i;

  for (i=0; i<L7_MAX_INTERFACE_COUNT; i++)
  {
    if (L7_TRUE == allow)
      userMgrCfgData.portData[i].portUsers = USER_MGR_PORT_ALL_ACCESS;
    else
      userMgrCfgData.portData[i].portUsers = 0;
  }

  return;
}

/*********************************************************************
* @purpose Set the bit field for this user for this port
*
* @param    intIfNum (input) index into interface array
* @param    userNum (input) bit offset into value field
* @param    value (input) value to set the bit
*
* @returns  void
*
* @notes This routine assumes the interface and userNum have been validated.
*
* @notes The ENABLED_BIT_MASK sets only the bit for the user that should
*        be enabled in this call.  It is 'or'ed with the existing bits
*        to enable the specified user.  The DISABLED_BIT_MASK clears only
*        the specified bit.  It is 'and'ed with existing bits to clear
*        the bit only for the specified user.
*
* @end
*********************************************************************/
void
userMgrPortUserBitSet( L7_uint32 intIfNum, L7_uint32 userNum, L7_BOOL value  )
{
  userMgrPortUserCfg_t *pCfg = L7_NULLPTR;

  if (userMgrPortIsConfigurable(intIfNum,&pCfg) == L7_FALSE)
    return;
  else
  {
    if (L7_TRUE == value)
      pCfg->portUsers |= ENABLED_BIT_MASK(userNum);
    else
      pCfg->portUsers &= DISABLED_BIT_MASK(userNum);

    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose Return the value of the bit for this user for this port
*
* @param    intIfNum (input) index into interface array
* @param    userNum (input) bit offset into value field
* @param    pValue (output) value of the bit
*
* @returns  void
*
* @notes This routine assumes the interface and userNum have been validated.
*
* @notes The ENABLED_BIT_MASK sets only the bit for the user that should
*        be enabled in this call.  It is 'or'ed with the existing bits
*        to enable the specified user.  The DISABLED_BIT_MASK clears only
*        the specified bit.  It is 'and'ed with existing bits to clear
*        the bit only for the specified user.
*
* @end
*********************************************************************/
void
userMgrPortUserBitGet( L7_uint32 intIfNum, L7_uint32 userNum, L7_BOOL *pValue  )
{
  userMgrPortUserCfg_t *pCfg = L7_NULLPTR;

  if (userMgrPortIsConfigurable(intIfNum,&pCfg) == L7_FALSE)
    return;
  else
  {
    if ( ( pCfg->portUsers & ENABLED_BIT_MASK(userNum) ) == 0 )
      *pValue = L7_FALSE;
    else
      *pValue = L7_TRUE;
  }

  return;
}

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
userMgrPortUserGetFirst(L7_uint32 intIfNum, L7_uchar8 *pUser )
{
  L7_uint32 i;
  L7_BOOL allow;

  /* Validate the interface*/
  if (userMgrPortIsValidIntf(intIfNum) != L7_TRUE)
    return(L7_FAILURE);

  /* Find the first enabled bit in the array value */
  for (i=0; i<L7_MAX_LOGINS; i++)
  {
    userMgrPortUserBitGet( intIfNum, i, &allow  );
    if (L7_TRUE == allow)
    {
      /* Validate the user name to check for blank user name */
      if (userMgrLoginUserNameGet(i, pUser) == L7_SUCCESS &&
          strlen(pUser) != 0 )
        break;
    }
  }

  if (L7_MAX_LOGINS == i)
    return(L7_FAILURE);


  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose Return the name of the next user with access to the port
*
* @param    intIfNum (input) Internal interface number of port
* @param    pPrevUser (input) ptr to user name returned by prior getNext or getFirst
* @param    pUser (output) ptr to user name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if intIfNum is not a valid interface or
*                      if no more users have access to the port or
*                      pPrevUser is not a valid user
*
* @notes
*
* @end
*********************************************************************/
L7_uint32
userMgrPortUserGetNext(L7_uint32 intIfNum, L7_uchar8 *pPrevUser, L7_uchar8 *pUser )
{
  L7_uint32 i, index;
  L7_BOOL allow;

  /* Validate the interface*/
  if (userMgrPortIsValidIntf(intIfNum) != L7_TRUE)
    return(L7_FAILURE);

  /* Get the login index of the previous user */
  if (userMgrLoginIndexGet(pPrevUser, &index) == L7_FAILURE)
    return(L7_FAILURE);

  /*
   * Find the first enabled bit in the array value beginning with
   * the next index following the index of the previous user.
   */
  for (i=index+1; i<L7_MAX_LOGINS; i++)
  {
    userMgrPortUserBitGet( intIfNum, i, &allow  );
    if (L7_TRUE == allow)
    {
      /* Validate the user name to check for blank user name */
      if (userMgrLoginUserNameGet(i, pUser) == L7_SUCCESS &&
          strlen(pUser) != 0 )
        break;
    }
  }

  if (L7_MAX_LOGINS == i)
    return(L7_FAILURE);

  return(L7_SUCCESS);
}

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
userMgrPortUserAccessSet(L7_uint32 intIfNum, L7_uchar8 *pUser, L7_BOOL allow )
{
  L7_uint32 index;

  /* Validate the interface*/
  if (userMgrPortIsValidIntf(intIfNum) != L7_TRUE)
    return(L7_FAILURE);

  /* Find the index of this user */
  if (userMgrLoginIndexGet(pUser, &index) == L7_FAILURE ||
      strlen(pUser) == 0)
    return(L7_FAILURE);

  /* Set the index bit in the array to the (boolean) allow value */
  userMgrPortUserBitSet(intIfNum, index, allow);

  return(L7_SUCCESS);
}

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
userMgrPortUserAccessGet(L7_uint32 intIfNum, L7_uchar8 *pUser, L7_BOOL *pAllow )
{
  L7_uint32 index;

  /* Validate the interface*/
  if (userMgrPortIsValidIntf(intIfNum) != L7_TRUE)
    return(L7_FAILURE);

  /* Find the index of this user */
  if (userMgrLoginIndexGet(pUser, &index) == L7_FAILURE ||
      strlen(pUser) == 0)
    return(L7_FAILURE);

  /* Get the boolean value of index bit for this array entry */
  userMgrPortUserBitGet(intIfNum, index, pAllow);

  return(L7_SUCCESS);
}

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
* @notes index begins with 0.
*
* @end
*********************************************************************/
L7_uint32
userMgrPortUserAccessGetIndex(L7_uint32 intIfNum, L7_uint32 index, L7_BOOL *pAllow )
{
  /* Validate the interface*/
  if (userMgrPortIsValidIntf(intIfNum) != L7_TRUE)
    return(L7_FAILURE);

  /* Validate the index */
  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  /* Get the boolean value of index bit for this array entry */
  userMgrPortUserBitGet(intIfNum, index, pAllow);

  return(L7_SUCCESS);
}

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
userMgrPortUserInitialize(L7_uint32 index)
{
  L7_uint32 i;

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  for (i=1; i<L7_MAX_INTERFACE_COUNT; i++)
  {
    /* Set the index bit in the array to the (boolean) allow value */
    userMgrPortUserBitSet(i, index, L7_DOT1X_DEFAULT_USER_PORT_ACCESS);
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose Debug function to view contents of user port array
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void
dumpUserMgrPortStruct(void)
{
  L7_uint32 i, intIfNum;
  userMgrPortUserCfg_t *pCfg = L7_NULLPTR;

  printf("\nUserMgr Port User Struct:");
  for (i=0; i<L7_MAX_INTERFACE_COUNT; i++)
  {
    if (userMgrPortIsConfigurable(i,&pCfg) == L7_FALSE)
      continue;

    printf("\n Array Index %i: ", i);

    /* START OF POST RELEASE G */
    
    if (nimIntIfFromConfigIDGet(&pCfg->configID, &intIfNum) != L7_SUCCESS)
        printf("IntIfNum=  UNKNOWN  ");
    else
        printf("IntIfNum=  %i  ", intIfNum);
    
    /* END OF POST RELEASE G */

    printf("portUsers = [%x]", pCfg->portUsers );
  }

  return;
}

/*********************************************************************
* @purpose  Callback function to process interface state changes.
*
* @param    intIfnum    @b{(input)} internal interface whose state has changed
* @param    event       @b{(input)} new state (see L7_PORT_EVENTS_t for list)
*
* @returns  L7_SUCCESS
*
* @notes    This function currently only cares about interface
*           creation.  We may have tried to apply port monitoring
*           data in mirrorApplyConfigData before the interfaces were
*           created.  If the interface being created is a physical port
*           and is configured as a source/dest monitor port, save it
*           here.  This function will also set up (enable) port mirroring
*           if necessary.
*
* @end
*********************************************************************/
L7_RC_t userMgrIntfStateChangeCallback(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  nimConfigID_t configID;
  nimConfigID_t unusedConfigID;
  L7_BOOL foundUnusedConfig = L7_FALSE;
  L7_BOOL foundConfig = L7_FALSE;
  L7_uint32 index;
  L7_uint32 unusedIndex = 0;

  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum     = intIfNum;
  status.component    = L7_USER_MGR_COMPONENT_ID;
  status.response.rc  = rc;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  nimEventStatusCallback(status);



  memset((void *)&unusedConfigID,0,sizeof(nimConfigID_t));

  if ((userMgrPortIsValidIntf(intIfNum)) && (intIfNum != 0) &&
      (nimConfigIdGet(intIfNum,&configID) == L7_SUCCESS))
  {
    switch (event)
    {
      case L7_CREATE:
        /* find the configuration for this interface */
        for (index = 0; index < L7_MAX_INTERFACE_COUNT;index++)
        {
          if (NIM_CONFIG_ID_IS_EQUAL(&configID,&userMgrCfgData.portData[index].configID))
          {
            foundConfig = L7_TRUE;
            break;
          }
          else if ((foundUnusedConfig == L7_FALSE) &&
                   (NIM_CONFIG_ID_IS_EQUAL(&unusedConfigID,&userMgrCfgData.portData[index].configID)))
          {
            foundUnusedConfig = L7_TRUE;
            unusedIndex = index;
          }
        }

        if ((foundConfig == L7_TRUE) && (index < L7_MAX_INTERFACE_COUNT))
        {
          userMgrPortOper_g[intIfNum] = &userMgrCfgData.portData[index];
        }
        else if (foundUnusedConfig == L7_TRUE)
        {
          /* set the default behavior for the port */
          NIM_CONFIG_ID_COPY(&userMgrCfgData.portData[unusedIndex].configID,&configID);

          userMgrPortOper_g[intIfNum] = &userMgrCfgData.portData[unusedIndex];
          userMgrCfgData.portData[unusedIndex].portUsers = USER_MGR_PORT_ALL_ACCESS;
          userMgrCfgData.cfgHdr.dataChanged = L7_FALSE;
        }
        else
        {
          /* no space in the config for the port */
          rc = L7_FAILURE;
          break;
        }
        rc = L7_SUCCESS;
        break;

      case L7_DELETE:

        /* remove the configuration info from the interface and unmarry the oper from the config */
        memset((void *) userMgrPortOper_g[intIfNum],0, sizeof(userMgrPortUserCfg_t));
        userMgrPortOper_g[intIfNum] = L7_NULL;
        userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
        break;

      default:
        /* we don't care about the event */
        rc = L7_SUCCESS;
        break;
    } /* switch(event)  */
  } /* Only Do work if physical interface */

  return(rc);
}

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
L7_BOOL userMgrPortIsValidIntfType(L7_uint32 sysIntfType)
{
  if(sysIntfType == L7_PHYSICAL_INTF)
    return L7_TRUE;

  return L7_FALSE;
}

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
L7_BOOL userMgrPortIsValidIntf(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;
  L7_INTF_TYPES_t  sysIntfType;

  if ((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (userMgrPortIsValidIntfType(sysIntfType) == L7_TRUE))
  {
    rc = L7_TRUE;
  }
  return(rc);
}

/*********************************************************************
* @purpose  See if the interface is configurable
*
* @param    intIfNum  {(input})   Internal interface number
* @param    pCfg      {(output)}  Pointer to the cfg data for the interface
*
* @returns  L7_TRUE or L7_FALSE, and the pointer to the interface config
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL userMgrPortIsConfigurable(L7_uint32 intIfNum,userMgrPortUserCfg_t **pCfg)
{
  L7_CNFGR_RQST_t currPhase;

  currPhase = userMgrCnfgrPhaseGet();

  if ((currPhase != L7_CNFGR_RQST_U_PHASE1_START) &&
      (currPhase != L7_CNFGR_RQST_I_PHASE3_START) &&
      (currPhase != L7_CNFGR_RQST_E_START))
    return L7_FALSE;

  /* check if it is a valid interface */
  if (userMgrPortIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FALSE;

  /* see if userMgr has created the interface yet */
  if (userMgrPortOper_g[intIfNum] == L7_NULLPTR)
    return L7_FALSE;

  *pCfg = userMgrPortOper_g[intIfNum];

  return L7_TRUE;
}
