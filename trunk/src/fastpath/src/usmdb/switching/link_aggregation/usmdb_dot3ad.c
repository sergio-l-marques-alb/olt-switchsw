/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src\application\unitmgr\usmdb\usmdb_dot3ad.c
 *
 * @purpose Provide interface to hardware API's for unitmgr components
 *
 * @component 
 *
 * @comments 
 *
 * @create
 *
 * @author   skalyanam
 * @end
 *
 **********************************************************************/
#include <string.h>

#include "l7_common.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "osapi.h"

#include "usmdb_util_api.h"
#include "dot1q_api.h"
#include "dot3ad_api.h"
#include "dot3ad_debug_api.h"
#include "dot3ad_exports.h"

extern L7_RC_t dot3adPortChannelSystemPrioritySet( L7_uint32 priority);

static L7_RC_t usmdbDot3adLagHashTranslate(L7_BOOL getHash, L7_uint32 *hashMode);

/*********************************************************************
 *
 * @purpose  Create a LAG with an initial set of configuration attributes
 *
 * @param UnitIndex    the unit for this operation
 * @param *name        name string assigned to this LAG
 * @param adminMode    administrative mode of LAG interface
 *                       (@b{Input:  L7_ENABLE, L7_DISABLE})
 * @param linktrapMode link trap mode of LAG interface
 *                       (@b{Input:  L7_ENABLE, L7_DISABLE})
 * @param numMembers   number of port members assigned to LAG
 * @param hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
 * @param members[]    list of member internal interfaces numbers to be assigned to
 *                       the LAG; numMembers specifies list count
 * @param ifIndex      output location of new LAG ifIndex number
 *                       (@b{Output:  LAG ifIndex})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_ERROR, if any config attribute is invalid
 * @returns  L7_FAILURE, if other failure
 *
 * @notes The LAG members[] list can be empty (i.e., numMembers = 0) when
 * @notes   creating a LAG.
 *
 * @notes No internal interface number is used when creating a LAG.  The 
 * @notes   LAG component assigns this LAG to an available interface.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adCreateSet(L7_uint32 UnitIndex, L7_char8 *name,
    L7_uint32 adminMode,
    L7_uint32 linkTrapMode, L7_uint32 numMembers,
                             L7_uint32 hashMode,
    L7_uint32 *pMembers, L7_uint32 *val)
{
  L7_uint32     i;
  L7_uint32     lagMembers[L7_MAX_MEMBERS_PER_LAG]; 
  L7_uint32     intIfNum, ifIndex;
  L7_RC_t       rc;
  L7_uint32     defaultVlanID;
  L7_uint32     toBeTranslatedHashMode;

  toBeTranslatedHashMode  = hashMode;
  defaultVlanID = L7_DOT1Q_DEFAULT_VLAN;

  if (strlen(name) >= DOT3AD_MAX_NAME)
    return L7_ERROR;

  /* ensure unused lagmembers are sent as zeros*/
  for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
  {
    if (i < numMembers)
    {
      lagMembers[i] = *pMembers;
      pMembers++;
    }
    else
      lagMembers[i] = 0;
  }

  if ( L7_SUCCESS != usmdbDot3adLagHashTranslate(L7_FALSE, &toBeTranslatedHashMode) )
    return L7_ERROR;

  /* NOTE:  USMDB not holding onto the returned intIfNum here.  It can
   *        query NIM at any time to get the LAG interface number from the
   *        ifIndex passed in from the user interface code.
   */
  if (dot3adLagCreate(name, lagMembers, adminMode, linkTrapMode,
                      toBeTranslatedHashMode, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* Any dot1q setting for this lag is handled by the dot1q component
   * while responding to the create and attach events for this lag interface
   */

  if (usmDbIfIndexGet(UnitIndex, intIfNum, &ifIndex) != L7_SUCCESS)
  {
    rc = dot3adLagRemove(intIfNum);
    return L7_FAILURE;
  }

  *val = intIfNum;
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Remove a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes Removing a LAG frees the internal resources associated with the
 * @notes   LAG, thus they can be reused when creating a new LAG.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adRemoveSet(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return(dot3adLagRemove(intIfNum));
}


/*********************************************************************
 *
 * @purpose  Change the name of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param *buf         buffer containing new name string for this LAG
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_ERROR, if config parameter is invalid
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adNameSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_char8 *buf)
{
  return(dot3adAggPortNameSet(intIfNum, (L7_uchar8 *)buf));
}

/*********************************************************************
*
* @purpose  Configure Hash Mode for a LAG.
*
* @param UnitIndex    the unit for this operation
* @param intIfNum     internal interface number of a configured LAG
* @param hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if config parameter is invalid
* @returns  L7_FAILURE, if other failure
* @returns  L7_DEPENDENCY_NOT_MET, if the LAG interface is admin disabled
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adLagHashModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 hashMode)
{
  L7_uint32 toBeTranslatedHashMode = hashMode;

  if ( L7_SUCCESS != usmdbDot3adLagHashTranslate(L7_FALSE, &toBeTranslatedHashMode) )
    return L7_ERROR;

  return(dot3adAggPortHashModeSet(intIfNum, toBeTranslatedHashMode));
}

/*********************************************************************
*
* @purpose  Get the hashMode of the configured LAG.
*
* @param UnitIndex    the unit for this operation
* @param intIfNum     internal interface number of a configured LAG
* @param *hashMode    pointer to Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adLagHashModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32  * hashMode)
{
  if ( L7_SUCCESS != dot3adAggPortHashModeGet(intIfNum, hashMode))
    return L7_FAILURE;

  if ( L7_SUCCESS != usmdbDot3adLagHashTranslate(L7_TRUE, hashMode) )
    return L7_FAILURE;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Configure Hash Mode for a LAG system
*
* @param UnitIndex    the unit for this operation
* @param hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if config parameter is invalid
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adSystemHashModeSet(L7_uint32 UnitIndex, L7_uint32 hashMode)
{
  L7_uint32 toBeTranslatedHashMode = hashMode;

  if ( L7_SUCCESS != usmdbDot3adLagHashTranslate(L7_FALSE, &toBeTranslatedHashMode) )
    return L7_ERROR;

  return(dot3adAggSystemHashModeSet(toBeTranslatedHashMode));
}

/*********************************************************************
*
* @purpose  Get the hashMode of the LAG system
*
* @param UnitIndex    the unit for this operation
* @param *hashMode    pointer to Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adSystemHashModeGet(L7_uint32 UnitIndex, L7_uint32  * hashMode)
{
  if ( L7_SUCCESS != dot3adAggSystemHashModeGet(hashMode))
    return L7_FAILURE;

  if ( L7_SUCCESS != usmdbDot3adLagHashTranslate(L7_TRUE, hashMode) )
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Get the name of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param *buf         buffer to be filled in with the name of this LAG
 *                       (@b{Output:  LAG name string})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adNameGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_char8 *buf)
{
  return(dot3adAggPortNameGet(intIfNum, (L7_uchar8 *)buf));
}

/*********************************************************************
 *
 * @purpose  Add a member port to a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param memIntf      internal interface number of the member to be added
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_ERROR, if config parameter is invalid 
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adMemberAddSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 memIntf)
{
  /* NOTE:  The LAG API can be used to add multiple member interfaces in
   *        a single call (up to L7_MAX_MEMBERS_PER_LAG).  This is not being
   *        exploited here at present.
   */
  L7_RC_t rc;

  rc = usmDbDot3adPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
    return rc;

  return(dot3adLagMemeberAdd(intIfNum, 1, &memIntf));
}

/*********************************************************************
 *
 * @purpose  Delete a member port from a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param memIntf      internal interface number of the member to be deleted
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_ERROR, if config parameter is invalid 
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adMemberDeleteSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 memIntf)
{
  /* NOTE:  The LAG API can be used to delete multiple member interfaces in
   *        a single call (up to L7_MAX_MEMBERS_PER_LAG).  This is not being
   *        exploited here at present.
   */
  L7_RC_t rc;

  rc = usmDbDot3adPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
    return rc;

  return(dot3adLagMemberDelete(intIfNum, 1, &memIntf));
}

/*********************************************************************
 *
 * @purpose  Get the list of members of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param lagIntIfNum  internal interface number of a configured LAG
 * @param *val         size of LAG member list
 *                       (@b{Input:  maximum number of LAG members to output})
 *                       (@b{Output: actual number of LAG member ports in list})
 * @param *buf         buffer for outputting list of LAG members
 *                       (@b{Output: list of LAG member internal interface numbers})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adMemberListGet(L7_uint32 UnitIndex, L7_uint32 lagIntIfNum, L7_uint32 *val, L7_uint32 *buf)
{
  L7_uint32      memberIntIfNum[L7_MAX_MEMBERS_PER_LAG], numMembers;
  L7_uint32      i, max;


  numMembers = L7_MAX_MEMBERS_PER_LAG;
  if (dot3adMemberListGet(lagIntIfNum, &numMembers, memberIntIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* use smaller of returned members or caller's member list size */
  max = (numMembers <= *val) ? numMembers : *val;
  for (i = 0; i < max; i++)
  {
    buf[i] = memberIntIfNum[i];
  }

  *val = max;
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Change the administrative mode of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          administrative mode
 *                       (@b{Input:  L7_ENABLE, L7_DISABLE})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_ERROR, if config parameter is invalid 
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAdminModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  return(dot3adAdminModeSet(intIfNum, val, L7_TRUE));
}

/*********************************************************************
 *
 * @purpose  Get the administrative mode of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          location to output the administrative mode value
 *                       (@b{Output:  L7_ENABLE, L7_DISABLE})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAdminModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return(dot3adAdminModeGet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Change the link trap of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          link trap
 *                       (@b{Input:  L7_ENABLE, L7_DISABLE})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_ERROR, if config parameter is invalid 
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adLinkTrapSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  return(dot3adLinkTrapModeSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the link trap of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          location to output the link trap value
 *                       (@b{Output:  L7_ENABLE, L7_DISABLE})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adLinkTrapGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return(dot3adLinkTrapModeGet(intIfNum, val));
}


/*********************************************************************
 *
 * @purpose  Get the aggregate data rate of the LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          location to output the LAG data rate
 *                       (@b{Output:  data rate in Mbps})
 *                       
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adDataRateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return(dot3adLagDataRateGet(intIfNum, val));
}


/*********************************************************************
 *
 * @purpose  Change the spanning tree mode of a configured LAG interface
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          STP mode
 *                       (@b{Input:  L7_DOT1D_8021D, 
 *                                   L7_DOT1D_RAPID, 
 *                                   L7_DOT1D_OFF})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adSTPModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  /*return(dot1dSTPPortModeSet(intIfNum, val));*/
  /* dot1d code has been removed, use the official usmdb dot1s api to set 
   * the stp mode of an interface 
   */
  return L7_FAILURE;
}

/*********************************************************************
 *
 * @purpose  Get the spanning tree mode of a configured LAG interface
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 * @param val          location to output the STP mode value
 *                       (@b{Output:  L7_DOT1D_8021D, 
 *                                    L7_DOT1D_RAPID, 
 *                                    L7_DOT1D_OFF})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adSTPModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  /*
   *val = dot1dSTPPortModeGet(intIfNum);
   return L7_SUCCESS;
   */
  /* Use the relevant dot1s usmdb function to get stp mode state */
  return L7_FAILURE;
}

/*********************************************************************
 *
 * @purpose  Get the ifIndex number of the first LAG interface
 *
 * @param UnitIndex    the unit for this operation
 * @param *val         location to store the output information
 *                       (@b{Output:  ifIndex number of first LAG})
 *                       
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes The LAG is not guaranteed to be a configured LAG.  Use 
 *        usmDbDot3adIsConfigured() to find out.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adIfIndexGetFirst(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32      intIfNum;

  if (dot3adAggEntryGetNext(0, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (usmDbIfIndexGet(UnitIndex, intIfNum, val) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Get the ifIndex number of the next successive LAG interface
 *
 * @param UnitIndex    the unit for this operation
 * @param ifIndex      external ifIndex number of the LAG from which to start 
 *                       searching
 * @param *val         location to store the output information
 *                       (@b{Output:  ifIndex number of next LAG})
 *                       
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes The LAG is not guaranteed to be a configured LAG.  Use 
 *        usmDbDot3adIsConfigured() to find out.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adIfIndexGetNext(L7_uint32 UnitIndex, L7_uint32 ifIndex, L7_uint32 *val)
{
  L7_uint32      intIfNum, nextIfNum;

  if (usmDbIntIfNumFromTrunkGet(UnitIndex, ifIndex, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (dot3adAggEntryGetNext(intIfNum, &nextIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (usmDbIfIndexGet(UnitIndex, nextIfNum, val) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Determine if a particluar LAG is configured
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     internal interface number of a configured LAG
 *                       
 * @returns  L7_TRUE     if LAG is configured
 * @returns  L7_FALSE    if LAG is not configured, or ifIndex is not a LAG
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_BOOL usmDbDot3adIsConfigured(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return(dot3adIsLagConfigured(intIfNum));
}

/*********************************************************************
 *
 * @purpose  Determine if a particluar internal interface is member of 
 *           a configured lag and return that lag
 *
 * @param UnitIndex    the unit for this operation
 * @param memIntf      the internal interface in question
 * @param parentIntf   location to store the output information
 *                       (@b{Output:  internal interface number of lag 
 *                                    that intIfNum is member of})
 *                       
 * @returns  L7_SUCCESS    if internal interface is a member
 * @returns  L7_FAILURE    if internal interface is not a member
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adIntfIsMemberGet(L7_uint32 UnitIndex, L7_uint32 memIntf,
    L7_uint32 *parentIntf)
{

  return dot3adAggGet(memIntf, parentIntf);

}


/*********************************************************************
 *
 * @purpose  Determine if a particluar internal interface is member of a particular lag
 *
 * @param UnitIndex       the unit for this operation
 * @param lagIntIfNum     the internal interface number for the lag
 * @param memberIntIfNum  internal interface number of member to be checked
 *                       
 * @returns  L7_SUCCESS    if interface is a member
 * @returns  L7_FAILURE    if interface is not a member
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adMemberCheck(L7_uint32 UnitIndex, L7_uint32 lagIntIfNum, L7_uint32 memberIntIfNum)
{
  L7_RC_t rc;
  L7_uint32 lag;

  rc = dot3adAggGet(memberIntIfNum, &lag);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (lagIntIfNum != lag)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose  Checks if the internal interface is used by any lag.
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     member internal interface number to be added
 *                       
 * @returns  L7_SUCCESS, if the internal interface is already present or not valid 
 * @returns  L7_FAILURE, if the internal interface is free.
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adIsMember(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  if (dot3adIsLagMember(intIfNum)  == L7_TRUE)
    return L7_SUCCESS;

  return L7_FAILURE;
}


/* 802.3ad APIs */


/*********************************************************************
 * @purpose  Gets the most recent change to the dot3adAggTable,
 *           dot3adAggPortListTable or the dot3adAggPortTable
 *
 * @param    UnitIndex  the unit for this operation
 * @param    *ts        time of last change
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adTablesLastChangedGet(L7_uint32 UnitIndex, L7_timespec *ts)
{
  return dot3adTablesLastChangedGet(ts);
}

/*********************************************************************
 * @purpose  Check the validity of an aggregator interface
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 *
 * @returns  L7_SUCCESS, if valid aggregator interface
 * @returns  L7_FAILURE, if not valid aggregator interface
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggEntryGet(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  if (usmDbDot3adIsConfigured(UnitIndex, intIfNum) == L7_TRUE)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Given an internal aggregator interface number, get the
 *           next one if it exists
 *
 * @param    UnitIndex        the unit for this operation
 * @param    intIfNum         internal interface number
 * @param    *nextIntIfNum    next internal interface number
 *
 * @returns  L7_SUCCESS, if a next was found
 * @returns  L7_FAILURE, if a next doesn't exist
 *
 * @notes   if the start doesn't exist, the closest next will be returned 
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggEntryGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  while(dot3adAggEntryGetNext(intIfNum, nextIntIfNum) == L7_SUCCESS)
  {
    if (usmDbDot3adIsConfigured(UnitIndex, *nextIntIfNum) == L7_TRUE)
      return L7_SUCCESS;
    intIfNum = *nextIntIfNum;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Gets the MAC address assigned to the aggregator
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mac            MAC address, L7_MAC_ADDR_LEN in length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPhysAddressGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  return dot3adAggMACAddressGet(intIfNum, mac);
}

/*********************************************************************
 * @purpose  Gets the priority value associated with the system's actor ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       priority, 0-64k
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggActorSystemPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggActorSystemPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Sets the priority value associated with the system's actor ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    priority        priority, 0-64k
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggActorSystemPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority)
{
  return dot3adAggActorSystemPrioritySet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Gets the MAC address used as a unique identifier for the 
 *           system that contains this aggregator
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mac            MAC identifier
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggActorSystemIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  return dot3adAggActorSystemIDGet(intIfNum, (L7_char8 *)mac);
}

/*********************************************************************
 * @purpose  Determines if the aggregator represents an aggregate or 
 *           an individual link
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *link           L7_TRUE: aggregate
 *                           L7_FALISE: individual
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggAggregateOrIndividualGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *link)
{
  return dot3adAggActorAggregateOrIndividualGet(intIfNum, link);
}

/*********************************************************************
 * @purpose  Gets the current administrative value of the Key for the
 *           aggregator 
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key            key value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    admin and oper key may differ
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggActorAdminKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggActorAdminKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Sets the current administrative value of the Key for the
 *           aggregator 
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    key             key value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    admin and oper key may differ
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggActorAdminKeySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 key)
{
  return dot3adAggActorAdminKeySet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Gets the current operational value of the Key for the
 *           aggregator 
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key            key value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    admin and oper key may differ
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggActorOperKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggActorOperKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  MAC address of current partner of aggregator
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mac            MAC address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    a value 0 indicates there is no known partner
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPartnerSystemIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  return dot3adAggPartnerSystemIDGet(intIfNum, (L7_uint32 *)mac);
}

/*********************************************************************
 * @purpose  Priority value associated with partner's system ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       priority value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    If the aggregation is manually configured, it will be a
 *           value assigned by the local system
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPartnerSystemPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPartnerSystemPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  The current operational value of the key for the aggregators
 *           current protocol partner. aggregator parser and either
 *           delivering it to a MAC client or discarding it
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key            operational key
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    If the aggregation is manually configured, it will be a
 *           value assigned by the local system
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPartnerOperKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggPartnerOperKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Delay that can be imposed on the frame collector between
 *           receving a frame from and aggregator parser and either
 *           delivering it to a MAC client or discarding it
 *
 * @param    UnitIndex       the unit for this operation
 * @param    InterfaceIndex  interface
 * @param    *delay          10s of usecs
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggCollectorMaxDelayGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *delay)
{
  return dot3adAggCollectorMaxDelayGet(intIfNum, delay);
}

/*********************************************************************
 * @purpose  Delay that can be imposed on the frame collector between
 *           receving a frame from and aggregator parser and either
 *           delivering it to a MAC client or discarding it
 *
 * @param    UnitIndex       the unit for this operation
 * @param    InterfaceIndex  interface
 * @param    delay           10s of usecs
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggCollectorMaxDelaySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 delay)
{
  return dot3adAggCollectorMaxDelaySet(intIfNum, delay);
}

/*********************************************************************
 * @purpose  Check the validity of an interface
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 *
 * @returns  L7_SUCCESS, if valid interface number
 * @returns  L7_FAILURE, if not valid interface number
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortIndexGet(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return dot3adAggPortIndexGet(intIfNum);
}

/*********************************************************************
 * @purpose  Given an internal interface number, get the next one,
 *           if it exists
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number, start
 * @param    *nextIntIfNum   internal interface number, next
 *
 * @returns  L7_SUCCESS, if a next was found
 * @returns  L7_FAILURE, if a next doesn't exist
 *
 * @notes   if the start doesn't exist, the closest next will be returned 
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortIndexGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  return dot3adAggPortIndexGetNext(intIfNum, nextIntIfNum);
}

/*********************************************************************
 * @purpose  Gets the priority value associated with the system actor's ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorSystemPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPortActorSystemPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Sets the priority value associated with the system actor's ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    priority        0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorSystemPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority)
{
  return dot3adAggPortActorSystemPrioritySet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Gets the system id for the system that contains this 
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mac            system id
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorSystemIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  L7_enetMacAddr_t id;
  L7_RC_t rc;
  rc = dot3adAggPortActorSystemIDGet(intIfNum, (L7_uint32 *)&id);
  memcpy(mac,id.addr,6);
  return rc;
}

/*********************************************************************
 * @purpose  Gets current administrative value of the key for the
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key            key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorAdminKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggPortActorAdminKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Sets current administrative value of the key for the
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorAdminKeySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 key)
{
  return dot3adAggPortActorAdminKeySet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Gets current operational value of the key for the
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorOperKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggPortActorOperKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Sets current operational value of the key for the
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorOperKeySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 key)
{
  return dot3adAggPortActorOperKeySet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Gets the current administrative priority associated
 *           with the partner's system ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       priority 0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminSystemPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPortPartnerAdminSystemPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Sets the current administrative priority associated
 *           with the partner's system ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    priority        priority 0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminSystemPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority)
{
  return dot3adAggPortPartnerAdminSystemPrioritySet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Gets the current operational priority associated
 *           with the partner's system ID
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    priority        priority 0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerOperSystemPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPortPartnerOperSystemPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner system ID (administrative)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mac            ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminSystemIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  L7_enetMacAddr_t id;
  L7_RC_t rc;
  rc = dot3adAggPortPartnerAdminSystemIDGet(intIfNum, &id);
  memcpy(mac,id.addr,6);
  return rc;
}

/*********************************************************************
 * @purpose  Sets the aggregation port's partner system ID (administrative)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    mac             ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminSystemIDSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  L7_enetMacAddr_t id;
  memcpy(id.addr, mac, 6);

  return dot3adAggPortPartnerAdminSystemIDSet(intIfNum, id);
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner system ID (operational)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mac            ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerOperSystemIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *mac)
{
  L7_enetMacAddr_t id;
  L7_RC_t rc;
  rc = dot3adAggPortPartnerOperSystemIDGet(intIfNum, &id);
  memcpy(mac,id.addr,6);
  return rc;
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner key (administrative)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key            key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggPortPartnerAdminKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Sets the aggregation port's partner key (administrative)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminKeySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 key)
{
  return dot3adAggPortPartnerAdminKeySet(intIfNum, key);
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner system ID (operational)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *key            key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerOperKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *key)
{
  return dot3adAggPortPartnerOperKeyGet(intIfNum, key);
}

/*********************************************************************
 * @purpose  The aggregrator this port has selected
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *agg            aggregrator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    0 means the port has not selected and aggregator
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortSelectedAggIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *agg)
{
  return dot3adAggPortSelectedAggIDGet(intIfNum, agg);
}

/*********************************************************************
 * @purpose  The identifier value of the aggregrator this port is 
 *           currently attached to
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *id             id value of aggregator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    0 means the port is not attached to an aggregator
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortAttachedAggIDGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *id)
{
  return dot3adAggPortAttachedAggIDGet(intIfNum, id);
}

/*********************************************************************
 * @purpose  The port number locally assigned to this aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *port           port actor 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    0 means the port has not attached to an aggregrator
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorPortGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *port)
{
  return dot3adAggPortActorPortGet(intIfNum, port);
}

/*********************************************************************
 * @purpose  Get the priority assigned to this aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorPortPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPortActorPortPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Set the priority assigned to this aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    priority        priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorPortPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority)
{
  return dot3adAggPortActorPortPrioritySet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Get the administrative value of the port number for the partner
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *port           port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminPortGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *port)
{
  return dot3adAggPortPartnerAdminPortGet(intIfNum, port);
}

/*********************************************************************
 * @purpose  Set the administrative value of the port number for the partner
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    port            port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminPortSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 port)
{
  return dot3adAggPortPartnerAdminPortSet(intIfNum, port);
}

/*********************************************************************
 * @purpose  Get the operational value of the port number for the partner
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *port           port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerOperPortGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *port)
{
  return dot3adAggPortPartnerOperPortGet(intIfNum, port);
}

/*********************************************************************
 * @purpose  Get the administrative value of the port priority of the partner
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminPortPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPortPartnerAdminPortPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Set the administrative value of the port priority of the partner
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    priority        priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminPortPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority)
{
  return dot3adAggPortPartnerAdminPortPrioritySet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Get the priority value assigned to this aggregation port
 *           by the partner
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerOperPortPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *priority)
{
  return dot3adAggPortPartnerOperPortPriorityGet(intIfNum, priority);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the admin values of actor_state
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorAdminStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *state)
{
  return dot3adAggPortActorAdminStateGet(intIfNum, state);
}

/*********************************************************************
 * @purpose  Set 8 bits corresponding to the admin values of actor_state
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorAdminStateSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *state)
{
  return dot3adAggPortActorAdminStateSet(intIfNum, *state);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the admin values of actor_state
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortActorOperStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *state)
{
  return dot3adAggPortActorOperStateGet(intIfNum, state);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the admin values of the partner_state
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *state)
{
  return dot3adAggPortPartnerAdminStateGet(intIfNum, state);
}

/*********************************************************************
 * @purpose  Set 8 bits corresponding to the admin values of the partner_state
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerAdminStateSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *state)
{
  return dot3adAggPortPartnerAdminStateSet(intIfNum, state[0]);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the current values of the
 *           partner_state from the most recently received LACPDU (by the partner)
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortPartnerOperStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *state)
{
  return dot3adAggPortPartnerOperStateGet(intIfNum, state);
}

/*********************************************************************
 * @purpose  Get whether an aggregation port is able to aggregate
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortAggregateOrIndividualGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *state)
{
  return dot3adAggPortAggregateOrIndividualGet(intIfNum, state);
}

/*********************************************************************
 * @purpose  Get the number of valid LACPDUs received on this aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsLACPDUsRxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsLACPDUsRxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker PDUs received on this aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsMarkerPDUsRxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsMarkerPDUsRxGet(intIfNum, stat);
}


/*********************************************************************
 * @purpose  Get the number of marker response PDUs received ont this 
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsMarkerResponsePDUsRxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsMarkerResponsePDUsRxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Get the number of unknown frames
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    type=slow protocol but unknown PDU or
 *           addressed to the slow protocols group but dont have the
 *           type set
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsUnknownRxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsUnknownRxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Get the number of invalid LACPDUs received on this aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    type=slow protocol but unknown PDU or
 *           addressed to the slow protocols group but dont have the
 *           type set
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsIllegalRxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsIllegalRxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Get the number of valid LACPDUs transmitted on this 
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsLACPDUsTxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsLACPDUsTxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker PDUs transmitted on this 
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsMarkerPDUsTxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsMarkerPDUsTxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker response PDUs transmitted on this 
 *           aggregation port
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortStatsMarkerResponsePDUsTxGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *stat)
{
  return dot3adAggPortStatsMarkerResponsePDUsTxGet(intIfNum, stat);
}

/*********************************************************************
 * @purpose  Clears the port stats
 *
 *
 * @param    unitIndex @b{(input)} the unit for this operation
 * @param    intIfNum  @b{(input)} interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adPortStatsClear(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  return dot3adPortStatsClear(intIfNum);
}

/*********************************************************************
 * @purpose  Get a bitwise list of ports included in an aggregator
 *
 * @param    UnitIndex       the unit for this operation
 * @param    agg_intf        internal interface number of aggregator
 * @param    *list           portlist, bits sent are included
 * @param    *length         length in bytes of the portlist
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortListGet(L7_uint32 UnitIndex, L7_uint32 agg_intf, L7_uchar8 *list, L7_uint32 *length)
{
  return dot3adAggPortListGet(agg_intf, list, length);
}

/*********************************************************************
 * @purpose  Get the LACP mode on a per-port basis
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    *mode           L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE      if port not found
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortLacpModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *mode)
{
  L7_BOOL status;
  L7_RC_t rc;

  rc = dot3adAggPortLacpEnabledGet(intIfNum, &status);
  if (rc == L7_SUCCESS)
  {
    if (status == L7_TRUE)
      *mode = L7_ENABLE;
    else if (status == L7_FALSE)
      *mode = L7_DISABLE;
    else
      rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
 * @purpose  Set the LACP mode on a per-port basis
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number
 * @param    mode            L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE      if port not found
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adAggPortLacpModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_BOOL status;
  L7_RC_t rc;

  rc = usmDbDot3adPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
    return rc;
  if (mode == L7_ENABLE)
    status = L7_TRUE;
  else if (mode == L7_DISABLE)
    status = L7_FALSE;
  else
    return L7_FAILURE;

  return dot3adAggPortLacpEnabledSet(intIfNum, status);
}
/*********************************************************************
 *
 * @purpose Determine if a particluar interface is a LAG interface.
 *
 * @param    UnitIndex       the unit for this operation
 * @param    intIfNum        internal interface number of the LAG in question
 *                       
 * @returns L7_TRUE, if interface is a LAG interface
 * @returns L7_FALSE, if interface is not a LAG interface
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_BOOL usmDbDot3adValidIntfCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return(dot3adIsLag(intIfNum));
}

/*********************************************************************
 * @purpose  Check to see if the port is the appropriate type on which
 *          the parm can be set
 *
 * @param   intIfNum            Internal interface number
 *
 * @returns  L7_SUCCESS         The parm can be set on this port type
 * @returns  L7_NOT_SUPPORTED   The port does not support this parm to be set
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adPortParmCanSet(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t ifType;

  if (nimGetIntfType(intIfNum,&ifType) == L7_SUCCESS)
  {
    if (ifType == L7_LOGICAL_VLAN_INTF ||
        ifType == L7_LOOPBACK_INTF ||
        ifType == L7_TUNNEL_INTF)
    {
      return L7_NOT_SUPPORTED;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
 *
 * @purpose  Get the list of active members of a configured LAG
 *
 * @param UnitIndex    the unit for this operation
 * @param lagIntIfNum  internal interface number of a configured LAG
 * @param *val         size of LAG member list
 *                       (@b{Input:  maximum number of LAG members to output})
 *                       (@b{Output: actual number of LAG member ports in list})
 * @param *buf         buffer for outputting list of LAG members
 *                       (@b{Output: list of LAG member internal interface numbers})
 *                       
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adActiveMemberListGet(L7_uint32 UnitIndex, L7_uint32 lagIntIfNum, L7_uint32 *val, L7_uint32 *buf)
{
  L7_uint32      memberIntIfNum[L7_MAX_MEMBERS_PER_LAG], numMembers;
  L7_uint32      i, max;


  numMembers = L7_MAX_MEMBERS_PER_LAG;
  if (dot3adActiveMemberListGet(lagIntIfNum, &numMembers, memberIntIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* use smaller of returned members or caller's member list size */
  max = (numMembers <= *val) ? numMembers : *val;
  for (i = 0; i < max; i++)
  {
    buf[i] = memberIntIfNum[i];
  }

  *val = max;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Checks to see if a LAG interface is static or not
 *
 * @param    UnitIndex    the unit for this operation
 * @param    lagIntIfNum  internal interface number of the lag interface
 * @param    *isStatic    boolean pointer 
 *           
 * @returns  L7_SUCCESS      
 * @returns  L7_FAILURE      
 *
 * @notes    A LAG with no active members will have the static variale set to false
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adIsStaticLag(L7_uint32 UnitIndex, 
    L7_uint32 lagIntIfNum,
    L7_BOOL *isStatic)
{
  return dot3adIsStaticLag(lagIntIfNum,isStatic);
}
/*********************************************************************
 *
 * @purpose  Checks if the internal interface is an active member of any lag.
 *
 * @param UnitIndex    the unit for this operation
 * @param intIfNum     member internal interface number to be added
 *                       
 * @returns  L7_SUCCESS 
 * @returns  L7_FAILURE
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adIsActiveMember(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  if (dot3adIsLagActiveMember(intIfNum)  == L7_TRUE)
    return L7_SUCCESS;

  return L7_FAILURE;
}
/*********************************************************************
 *
 * @purpose Determine if a particluar LAG is configured.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 intIfNum   internal interface number of the LAG in question
 *                       
 * @returns L7_TRUE, if LAG is configured
 * @returns L7_FALSE, if LAG is not configured, or ifIndex is not a LAG
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_BOOL usmDbLagIsConfigured(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return(dot3adIsLagConfigured(intIfNum));
}

/*********************************************************************
 * @purpose Determine if an interface is valid for dot3ad
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 intIfNum   internal interface number
 *                       
 * @returns L7_TRUE, if interface is valid
 * @returns L7_FALSE, otherwise
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_BOOL usmDbDot3adIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return(dot3adIsValidIntf(intIfNum));
}

/*********************************************************************
 * @purpose  Set the Static/Dynamic mode for a LAG
 *            
 * @param    UnitIndex    the unit for this operation
 * @param    lagIntIfNum  internal interface number of a configured LAG
 * @param    mode         Enables or disables static mode
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_TABLE_IS_FULL if user is making static lag dynamic, and maximum 
 *                            number of dynamic lags already present in the system.
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adLagStaticModeSet(L7_uint32 UnitIndex, L7_uint32 lagIntIfNum, L7_BOOL mode)
{
  return dot3adLagStaticModeSet(lagIntIfNum, mode);
}

/*********************************************************************
 * @purpose  Get the current number of LAGs in the system (static and dynamic) 
 *            
 * @param    *currNumLags pointer to output location
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    This is the number Lags present in the system
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adLagCountGet(L7_uint32 *currNumLags)
{
  return (dot3adLagCountGet(currNumLags));
}

/*********************************************************************
 * @purpose  Get the current status of displaying Dot3ad packet debug info
 *            
 * @param    none
 *                      
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_BOOL usmDbDot3adPacketDebugTraceFlagGet()
{
  return dot3adDebugPacketTraceFlagGet();
}

/*********************************************************************
 * @purpose  Turns on/off the displaying of dot3ad packet debug info
 *            
 * @param    flag         new value of the Packet Debug flag
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adPacketDebugTraceFlagSet(L7_BOOL flag)
{
  return dot3adDebugPacketTraceFlagSet(flag);
}

/*********************************************************************
 * @purpose  Sets the port channel system priority
 *
 * @param    priority        priority, 0-64k
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot3adPortChannelSystemPrioritySet(L7_uint32 priority)
{
  return dot3adPortChannelSystemPrioritySet( priority);
}

/*********************************************************************
* @purpose  Given LAG index, get the intIfNum of the LAG interface
*
* @param    index      (input) LAG index
* @param    *intIfNum  (output) internal interface number
*
* @returns  L7_SUCCESS, if the LAG was found
* @returns  L7_FAILURE, if not
*
* @notes    index begins with one, but the array is zero-based.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adLagIfNumFromIndexGet(L7_uint32 index, L7_uint32 *intIfNum)
{
  return dot3adAggIfNumFromIndexGet( index, intIfNum );
}

/*********************************************************************
* @purpose  Given the intIfNum of the LAG interface, return the index
*
* @param    index      (input) LAG index
* @param    *intIfNum  (output) internal interface number
*
* @returns  L7_SUCCESS, if the LAG was found
* @returns  L7_FAILURE, if not
*
* @notes    index begins with one, but the array is zero-based.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adLagIndexFromIfNumGet(L7_uint32 intIfNum, L7_uint32 *index)
{
  return dot3adAggIndexFromIfNumGet( intIfNum, index );
}

/*********************************************************************
* @purpose  Given the intIfNum of the LAG interface, return the number of members in the lag
*
* @param    unit            (input) unit number
* @param    intIfNum        (input) internal interface number
* @param    currNumMembers  (output) the number of members of the lag 
*
* @returns  L7_SUCCESS, if the LAG was found
* @returns  L7_FAILURE, if not
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3adLagNumMembersGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 *numMembers)
{
  return dot3adLagNumMembersGet(intIfNum, numMembers);
}

/*********************************************************************
 * @purpose  Determine to which LAG interface, if any, an interface belongs.
 *            
 * @param    intIfNum     internal interface number of possible LAG member
 * @param    pLagIntfNum    pointer to result 
 *                          (@b{Output:} internal interface number of 
 *                           LAG, if the specified interface is a member; 
 *                           otherwise, original @i{intIfNum} value is provided)
 *                            
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    A return code of L7_SUCCESS is used even when the interface is
 *           not a member of a LAG.  The L7_FAILURE return code designates
 *           a malfunction in the routine; it does not imply "not found".
 *
 * @end
 *********************************************************************/
L7_RC_t usmdbDot3adWhoisOwnerLag(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 *pLagIntfNum)
{
  return dot3adWhoisOwnerLag(intIfNum, pLagIntfNum);
}

/*********************************************************************
 * @purpose  To Translate UI hash value to equivalent application hash
 *           value
 *
 * @param    getMode        (@b{Input}
 * @param    hashMode       (@b{Output}{Input}
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @end
 *********************************************************************/
static L7_RC_t usmdbDot3adLagHashTranslate(L7_BOOL getHash, L7_uint32 *hashMode)
{
  switch(*hashMode)
  {
    case 1:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH1;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH1;
      break;
    case 2:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH2;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH2;
      break;
    case 3:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH3;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH3;
      break;
    case 4:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH4;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH4;
      break;
    case 5:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH5;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH5;
      break;
    case 6:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH6;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH6;
      break;
    case 7:
      if(L7_TRUE == getHash)
        *hashMode = USMDB_LAG_HASH_GET_MAPPING_HASH7;
      else
        *hashMode = USMDB_LAG_HASH_SET_MAPPING_HASH7;
      break;
     default:
       return L7_FAILURE;
  }
  return L7_SUCCESS;
}
