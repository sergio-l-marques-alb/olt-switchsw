/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_sshc.c
*
* @purpose SSHC USMDB API functions
*
* @component sshc
*
* @comments none
*
* @create 03/28/2006
*
* @author ikiran
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "usmdb_sshc_api.h"
#include "sshc_api.h"
#include "string.h"

/*********************************************************************
*
* @purpose Set the username of remote host
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_char8  *username  @b((input)) username
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSshcTransferRemoteUsernameSet(L7_uint32 UnitIndex, L7_char8 *username)
{
  sshcTransferRemoteUsernameSet(username);
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Set the password of remote host
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_char8  *password @b((input)) password
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSshcTransferRemotePasswordSet(L7_uint32 UnitIndex, L7_char8 *password)
{
  sshcTransferRemotePasswordSet(password);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the username of remote host
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_char8  *username  @b((output)) username
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSshcTransferRemoteUsernameGet(L7_uint32 UnitIndex, L7_char8 *username)
{
  return sshcTransferRemoteUsernameGet(username);
}
/*********************************************************************
*
* @purpose Get the password of remote host
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_char8  *password @b((output)) password
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSshcTransferRemotePasswordGet(L7_uint32 UnitIndex, L7_char8 *password)
{
  return sshcTransferRemotePasswordGet(password);
}

