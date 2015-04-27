/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: l7_getvar.h
*
* Purpose: Externalize the i_GetVar function
*
* Created by: Colin Verne, 07-17-01
*
* Component: SNMP
*
*********************************************************************/

/**********************************************************************
                       
**********************************************************************/
#ifndef L7_GETVAR_H
#define L7_GETVAR_H

#include <l7_common.h>

/*********************************************************************
* @purpose  Retrieve an unsigned integer MIB object
*
* @parms    *oid_buf      integer buffer containing the OID to retreive
*           *val          unsigned integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an unsigned integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
L7GetVarUInt( L7_char8 *oid_buf, L7_uint32 *val);

/*********************************************************************
* @purpose  Retrieve a signed integer MIB object
*
* @parms    *oid_buf      integer buffer containing the OID to retreive
*           *val          signed integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not a signed integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
L7GetVarSInt( L7_char8 *oid_buf, L7_int32 *val);

/*********************************************************************
* @purpose  Retrieve an character buffer MIB object
*
* @parms    *oid_buf      integer buffer containing the OID to retreive
*           *buf          character buffer to place the value into
*           *buf_len      unsigned integer to place the buffer length into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an octet string
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
L7GetVarCharBuf( L7_char8 *oid_buf, L7_char8 *buf, L7_uint32 *buf_len);

#endif /* L7_GETVAR_H */
