/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename  dtl_util.h
*
* @purpose   
*
* @component Generic
*
* @comments
*
* @create   06/03/2005
*
* @author   ryadagiri
*
* @end
*
**********************************************************************/

#include "comm_mask.h"
#include "nimapi.h"
#include "dapi.h"

/*************************************************************************
*
* @purpose Populate an array of DAPI USP structures from an interface mask
*
* @param   intfMask    @b{(input)}  interface mask
* @param   *portUsp    @b{(output)} pointr to array of DAPI_USP_t structures 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
************************************************************************/
L7_RC_t dtlIntfMaskToDapiUSPList(L7_INTF_MASK_t mask, DAPI_USP_t *portUsp);
