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

#ifdef L7_TIMERANGES_PACKAGE
  #include "timerange_api.h"
#endif /* L7_TIMERANGES_PACKAGE */


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

    case L7_CPU_INTF:
      rc = L7_TRUE;
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

/*********************************************************************
*
* @purpose  Checks if the time range name is a valid string
*
* @param    *name       @b{(input)} time range name string
*
* @returns  L7_SUCCESS  valid time range name string
* @returns  L7_FAILURE  invalid time range name string
*
* @comments This function only checks the name string syntax for a time range.
*
* @end
*
*********************************************************************/
L7_RC_t aclTimeRangeNameStringCheck(L7_uchar8 *name)
{
#ifdef L7_TIMERANGES_PACKAGE
  return timeRangeNameStringCheck(name);
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
*
* @purpose  To get the index of a time range, given its name.
*
* @param    *name      @b{(input)} time range name
* @param    *timeRangeIndex  @b{(output)} time range index
*
* @returns  L7_SUCCESS  time range index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    time range name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclTimeRangeNameToIndex(L7_uchar8 *name, L7_uint32 *timeRangeIndex)
{
#ifdef L7_TIMERANGES_PACKAGE
   return timeRangeNameToIndex(name, timeRangeIndex);
#else
  return L7_FAILURE;
#endif
}
/*********************************************************************
*
* @purpose  Get the status of the specified time range
*
* @param    timeRangeIndex    @b{(input)}  time range index to begin search
* @param    *status           @b{(output)} pointer to status output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    time range does not exist
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclTimeRangeStatusGet(L7_uint32 timeRangeIndex, L7_uint32 *status)
{
#ifdef L7_TIMERANGES_PACKAGE
  return timeRangeStatusGet(timeRangeIndex, status);
#else
  return L7_FAILURE;
#endif
}

/****************************************************************************
 * @purpose  Register a routine to be called for a specified time range's
 *           event
 * @param    componentID  @b{(input)} registering compoent id
 *                                      See L7_COMPONENT_ID_t for more info.
 * @param    *name        @b{(input)} pointer to the name of the registered
 *                                    function.
 *                                    up to TIMERANGE_NOTIFY_FUNC_NAME_SIZE
 *                                    characters will be stored.
 * @param    *notify      @b{(input)} pointer to a routine to be invoked for
 *                                    time range events
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t aclTimeRangeEventCallbackRegister(L7_COMPONENT_IDS_t  componentID,
                                                       L7_uchar8 *name,
     L7_RC_t (*notify)(L7_uchar8 *timeRangeName, L7_uint32 event))
{
#ifdef L7_TIMERANGES_PACKAGE
   return timeRangeEventCallbackRegister(componentID, name, notify);
#else
  return L7_FAILURE;
#endif
}
