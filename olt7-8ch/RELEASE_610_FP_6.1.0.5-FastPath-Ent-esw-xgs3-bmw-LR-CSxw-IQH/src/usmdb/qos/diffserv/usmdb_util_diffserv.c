/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_util_diffserv.c
*
* @purpose Util functions for diffserv
*
* @component diffserv
*
* @comments tba
*
* @create 08/08/02
*
* @author gbrown/kmcdowell
* @end
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l7_common.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_util_diffserv_api.h"
#endif

#include "comm_mask.h"

#include "l7_diffserv_api.h"


/*********************************************************************
* @purpose  Check if the specified Internal Interface Number is in
*           use by the ACL component
*
* @param    unitIndex   @b{(input)} Index of the Unit
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ifDirection @b{(input)} Interface Direction
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL usmDbDiffServIsAclIntfInUse(L7_uint32 unitIndex, L7_uint32 intIfNum,
                                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  return diffServIsAclIntfInUse(intIfNum, ifDirection);
}

/*********************************************************************
* @purpose  Get the MAC Access List name for the specified ACL number
*
* @param    unitIndex   @b{(input)} Index of the Unit
* @param    aclNum      @b{(input)} MAC Access List number
* @param    pName       @b{(output)} MAC ACL name pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Caller must provide a name buffer of at least 
*           (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1) characters.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAclMacNameGet(L7_uint32 unitIndex, L7_uint32 aclNum, 
                                   L7_uchar8 *pName)
{
  return diffServAclMacNameGet(aclNum, pName);
}

/*********************************************************************
* @purpose  Query if the specified ACL number matches a configured
*           named IPv4 Access List
*
* @param    unitIndex   @b{(input)} Index of the Unit
* @param    aclNum          @b{(input)} IP Access List number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServIsAclNamedIpAcl(L7_uint32 unitIndex, L7_uint32 aclNum)
{
  return diffServIsAclNamedIpAcl(aclNum);
}

/*********************************************************************
* @purpose  Get the IP Access List name for the specified ACL number
*
* @param    unitIndex   @b{(input)} Index of the Unit
* @param    aclNum      @b{(input)} named IP Access List number
* @param    pName       @b{(output)} named IP ACL name pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Caller must provide a name buffer of at least 
*           (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1) characters.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAclIpNameGet(L7_uint32 unitIndex, L7_uint32 aclNum, 
                                 L7_uchar8 *pName)
{
  return diffServAclIpNameGet(aclNum, pName);
}

/*********************************************************************
* @purpose  Get the index number of a MAC access list, given its name.
*
* @param    unitIndex   @b{(input)} Index of the Unit
* @param    pName       @b{(input)} access list name pointer
* @param    pAclNum     @b{(output)} access list index pointer
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAclMacNameToIndex(L7_uint32 unitIndex, L7_uchar8 *pName,
                                       L7_uint32 *pAclNum)
{
  return diffServAclMacNameToIndex(pName, pAclNum);
}
