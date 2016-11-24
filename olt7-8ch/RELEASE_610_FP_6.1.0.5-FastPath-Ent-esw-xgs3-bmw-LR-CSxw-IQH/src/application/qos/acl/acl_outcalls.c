/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   acl_outcalls.c
*
* @purpose    Access Control List component outcall functions.
*
* @component  Access Control List
*
* @comments   
*
* @create     10/25/2002
*
* @author     rjindal
*
* @end
*
*********************************************************************/
#include "acl_api.h"


#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  #include "usmdb_mib_diffserv_common.h"
  #include "l7_diffserv_api.h"
#endif


/*********************************************************************
* @purpose  Check if the specified Internal Interface Number is in
*           use by the DiffServ component
*
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    direction  @b{(input)} Interface direction
*                                  (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL aclIsDiffServIntfInUse(L7_uint32 intIfNum, L7_uint32 direction)
{
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  L7_uint32 dir;

  /* check specified direction only */
  if (direction == L7_INBOUND_ACL)
    dir = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  else if (direction == L7_OUTBOUND_ACL)
    dir = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
  else
    return L7_FALSE;

  if (diffServServiceGet(intIfNum, dir) == L7_SUCCESS)
    return L7_TRUE;

  return L7_FALSE;

#else
  return L7_FALSE;

#endif
}

/*********************************************************************
* @purpose  Determine if the interface type is valid for ACL
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL aclIsValidIntfType(L7_uint32 sysIntfType)
{
  L7_BOOL       rc = L7_FALSE;

  /* NOTE:  ACL only supports physical and LAG interfaces.  The application
   *        code relies on the device driver to manage the configuration
   *        for ports as they are acquired by and released from a LAG.
   */
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      rc = L7_TRUE;
      break;

    case L7_LAG_INTF:
      /* check the feature support to ensure LAG interfaces are allowed */
      rc = cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID,  
                                 L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID);
      break;

    default:
      break;

  } /* endswitch */

  return rc;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for ACL
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL aclIsValidIntf(L7_uint32 intIfNum)
{
  L7_BOOL       rc = L7_FALSE;
  L7_INTF_TYPES_t     sysIntfType;

  /* NIM complains about an intIfNum of 0, so check for this first */
  if ((intIfNum > 0) &&
      (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS))
  {
    rc = aclIsValidIntfType(sysIntfType);
  }

  return rc;
}

