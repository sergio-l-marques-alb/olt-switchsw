/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_port_user.h
*
* @purpose header file for interface for user manager user/port configuration
*
* @component usmDb
*
* @comments 
*
* @create 05/13/2004
*
* @author jflanagan
*
* @end
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "user_mgr_ports.h"

             
/*********************************************************************
* @purpose Return the name of the first user with access to the port  
*
* @param   UnitIndex (input) the unit for this operation
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
usmDbPortUserGetFirst(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *pUser );

/*********************************************************************
* @purpose Return the name of the next user with access to the port  
*
* @param   UnitIndex (input) the unit for this operation
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
usmDbPortUserGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *pPrevUser, L7_uchar8 *pUser );

/*********************************************************************
* @purpose Configure the port access for the specified user on the specified port
*
* @param   UnitIndex (input) the unit for this operation
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
usmDbPortUserAccessSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *pUser, L7_BOOL allow );

/*********************************************************************
* @purpose Get the port access setting for a specific user on a specific port
*
* @param   UnitIndex (input) the unit for this operation
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
usmDbPortUserAccessGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *pUser, L7_BOOL *pAllow );

/*********************************************************************
* @purpose Get the port access setting for a specific user (by index) on a specific port
*
* @param    UnitIndex (input) the unit for this operation
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
usmDbPortUserAccessGetIndex(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 index, L7_BOOL *pAllow );
