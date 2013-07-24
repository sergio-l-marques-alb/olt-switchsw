/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_dot1q_api.c
* @purpose     Implementaion of GARP API Functions 
* @component   GARP
* @comments    none
* @create      09/06/2005
* @author      vrynkov
* @author      
* @end
*             
**********************************************************************/

#include "commdefs.h"
#include "datatypes.h"
#include "garpapi.h"
#include "garpcfg.h"
#include "garpctlblk.h"
#include "dot3ad_api.h"
#include "dot1q_api.h"
#include "garp_dot1q_util.h"
#include "garp.h"

extern garpCfg_t     *garpCfg;
extern osapiRWLock_t garpCfgRWLock;
extern GARPCBptr     GARPCB;
extern GarpInfo_t    garpInfo;


/*********************************************************************
* @purpose  Gets GVRP or GMRP status of the device
*
* @param (out) mode           L7_TRUE or L7_FALSE
* @param (in)  application    GARP_GVRP_APP or GARP_GMRP_APP
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t garpGlobalModeGet(L7_uint32 *mode, GARPApplication application)
{
    L7_RC_t return_value = L7_FAILURE;

    (void)osapiReadLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

    if ((garpCfg_t *)L7_NULL != garpCfg) 
    {   
        if (GARP_GVRP_APP == application) 
        {           
            *mode = (L7_uint32)garpCfg->gvrp_enabled;
            return_value = L7_SUCCESS;
        }
        else if (GARP_GMRP_APP == application) 
        {
            *mode = (L7_uint32)garpCfg->gmrp_enabled;
            return_value = L7_SUCCESS;
        }        
    }

    (void)osapiReadLockGive(garpCfgRWLock);

    return return_value;
}



/*********************************************************************
* @purpose  Gets GVRP or GMRP status of the interface 
*
* @param (in) intIfNum       Interface Number
* @param (in) application    GARP_GVRP_APP or GARP_GMRP_APP
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL garpIntfModeGet(L7_uint32 intIfNum, GARPApplication application)
{
    L7_BOOL           return_value = L7_FALSE; 
    garpIntfCfgData_t *pCfg        = (garpIntfCfgData_t *)L7_NULL;

    (void)osapiReadLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

    if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
    {                                                     
        if (GARP_GVRP_APP == application) 
        {           
            return_value = (L7_BOOL)(pCfg->gvrp_enabled);
        }
        else if (GARP_GMRP_APP == application) 
        {
            return_value = (L7_BOOL)(pCfg->gmrp_enabled);
        }        
    }

    (void)osapiReadLockGive(garpCfgRWLock);

    return return_value;
}


/*********************************************************************
* @purpose  Gets GVRP status of the interface 
*
* @param (in) intIfNum       Interface Number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL garpGVRPIntfEnableGet(L7_uint32 intIfNum)
{
    L7_BOOL return_value;

    return_value = garpIntfModeGet(intIfNum, GARP_GVRP_APP);

    return return_value;
}


/*********************************************************************
* @purpose  Gets GMRP status of the interface 
*
* @param (in) intIfNum       Interface Number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL garpGMRPIntfEnableGet(L7_uint32 intIfNum)
{
    L7_BOOL return_value;

    return_value = garpIntfModeGet(intIfNum, GARP_GMRP_APP);

    return return_value;
}


/*********************************************************************
* @purpose  Gets GVRP or GMRP status of interface 
*
* @param (in)  intIfNum       Interface Number
* @param (out) status         Status of Interface intIfNum
* @param (in)  application    GARP_GVRP_APP or GARP_GMRP_APP
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t garpIntfCfgGet(L7_uint32 intIfNum, 
                       L7_uint32 *status, GARPApplication application)
{
    L7_RC_t           return_value = L7_FAILURE; 
    garpIntfCfgData_t *pCfg        = (garpIntfCfgData_t *)L7_NULL;

    (void)osapiReadLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

    if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
    {   
        if (GARP_GVRP_APP == application) 
        {           
            *status      = (L7_uint32)(pCfg->gvrp_enabled);
            return_value = L7_SUCCESS;
        }
        else if (GARP_GMRP_APP == application) 
        {           
            *status      = (L7_uint32)(pCfg->gmrp_enabled);
            return_value = L7_SUCCESS;
        }
    }
 
    (void)osapiReadLockGive(garpCfgRWLock);

    return return_value;
}


/*********************************************************************
* @purpose  Enable or Disable GVRP for an interface
*
* @param (in) intIfNum  physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param (in) status    L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpIntfGVRPModeApply(L7_uint32 intIfNum, L7_uint32 status)
{
    L7_uchar8 attrValue = 0;    /* the attribute value is unused in this sequence */
    L7_RC_t   rc;

    /* The L7_ALL_INTERFACES is passed to the GarpIssueCmd and handled on the garpCPTask */

    if (status == L7_ENABLE)
    {
      rc = GarpIssueCmd(GARP_GVRP_APP, GARP_ENABLE_PORT, intIfNum,
                         GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)&attrValue, L7_NULL);

    } /* if (status == L7_ENABLE) */

    else /* status == L7_DISABLE */
      rc = GarpIssueCmd(GARP_GVRP_APP, GARP_DISABLE_PORT, intIfNum,
                         GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)&attrValue, L7_NULL);

  return(rc);
}


/*********************************************************************
* @purpose  Enable or Disable GMRP for an interface
*
* @param (in) intIfNum  physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param (in) status    L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpIntfGMRPModeApply(L7_uint32 intIfNum, L7_uint32 status)
{
  L7_uchar8 attrValue = 0;    /* the attribute value is unused in this sequence */
  L7_RC_t   rc;

  /* The L7_ALL_INTERFACES is passed to the GarpIssueCmd and handled on the garpCPTask */

  if (status == L7_ENABLE)
  {
    rc = GarpIssueCmd(GARP_GMRP_APP, GARP_ENABLE_PORT, intIfNum, 
                      GARP_GMRP_ATTRIBUTE, (GARPAttrValue)&attrValue, L7_NULL);
  }
  else /* status == L7_DISABLE */
      {
    rc = GarpIssueCmd(GARP_GMRP_APP, GARP_DISABLE_PORT, intIfNum, 
                      GARP_GMRP_ATTRIBUTE, (GARPAttrValue)&attrValue, L7_NULL);
  }

  return(rc);
}



/*********************************************************************
* @purpose  Sets GARP Join, Leave, or Leave All Time
*
* @param (in)  intIfNum  interface number
* @param (in)  new_time  Time (in centiseconds) to be set
* @param (in)  timer     GARP_JOIN_TIME, GARP_LEAVE_TIME or GARP_LEAVEALL_TIME
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpTimeApply(L7_uint32 intIfNum, 
                    L7_uint32 new_time, GARPCommandNameTable timer)
{
    L7_RC_t   return_value  = L7_SUCCESS;
    L7_BOOL   issue_command = L7_FALSE;
    garpIntfCfgData_t *pCfg = (garpIntfCfgData_t *)L7_NULL;
    L7_uint32 time_to_set; 

    (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

    if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
    {
        time_to_set = new_time * 10;

        switch (timer) 
        {
        case GARP_JOIN_TIME:
            if (pCfg->join_time != time_to_set) 
            { 
                pCfg->join_time = time_to_set;
                issue_command   = L7_TRUE;
            }
            break;

        case GARP_LEAVE_TIME:
            if (pCfg->leave_time != time_to_set) 
            { 
                pCfg->leave_time = time_to_set;
                issue_command    = L7_TRUE;
            }           
            break;

        case GARP_LEAVEALL_TIME:
            if (pCfg->leaveall_time != time_to_set) 
            { 
                pCfg->leaveall_time = time_to_set;
                issue_command       = L7_TRUE;
            }
            break;

        default:                        
            break;
        }

        if (L7_TRUE == issue_command) 
        {
            garpCfg->hdr.dataChanged = L7_TRUE;
        }
    }

    (void)osapiWriteLockGive(garpCfgRWLock);

    if (L7_TRUE == issue_command) 
    {   
        return_value =  
            GarpIssueCmd(GARP_GVRP_APP, timer, intIfNum, 
                         GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)&new_time, 0);
    }

    return return_value;
}


           
/*********************************************************************
* @purpose  Gets GARP Join, Leave, or Leave All Time
*
* @param (in)  intIfNum  interface number
* @param (out) time      Time (in centiseconds) value read
* @param (in)  timer     GARP_JOIN_TIME, GARP_LEAVE_TIME or GARP_LEAVEALL_TIME
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpTimeGet(L7_uint32 intIfNum, 
                    L7_uint32 *time, GARPCommandNameTable timer)
{
    garpIntfCfgData_t *pCfg        = (garpIntfCfgData_t *)L7_NULL;
    L7_RC_t           return_value = L7_FAILURE;

    (void)osapiReadLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

    if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
    {
        return_value = L7_SUCCESS;

        switch (timer) 
        {
        case GARP_JOIN_TIME:
            *time = pCfg->join_time / 10;
            break;

        case GARP_LEAVE_TIME:
            *time = pCfg->leave_time / 10;
            break;

        case GARP_LEAVEALL_TIME:
            *time = pCfg->leaveall_time / 10;
            break;

        default:
            return_value = L7_FAILURE;
            break;
        }       
    }

    (void)osapiReadLockGive(garpCfgRWLock);

    return return_value;
}



/*********************************************************************
* @purpose  Enable or Disable GVRP or GMRP global mode for the device
*
* @param (in) command     L7_ENABLE or L7_DISABLE
* @param (in) application GARP_GVRP_APP or GARP_GMRP_APP
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    none
*
* @end
********************************************************************/

L7_RC_t garpModeApply(L7_uint32 command, GARPApplication application)
{
    L7_RC_t return_value;
    GARPCommandNameTable cmdName;
    L7_BOOL mode;
    GARPAttrType attrType = GARP_GVRP_APP; /* initialize to avoid compile error in Linux */
    L7_uchar8 attrValue = 0;               /* the attribute value is unused in this sequence */


    if ((GARP_GVRP_APP != application) && (GARP_GMRP_APP != application))
    {
        return L7_FAILURE;
    }

    return_value = garpGlobalModeGet(&mode, application);

    if (L7_SUCCESS != return_value) 
    {
        return return_value;
    }

    cmdName = (L7_ENABLE == command) ? GARP_ENABLE : GARP_DISABLE;

    if (((L7_TRUE  == mode) && (GARP_DISABLE == cmdName)) ||
        ((L7_FALSE == mode) && (GARP_ENABLE  == cmdName)))
    {

        if (GARP_GVRP_APP == application) 
        {
            /* change GVRP global mode to opposite */
            attrType = GARP_GVRP_VLAN_ATTRIBUTE;
        }
        else if (GARP_GMRP_APP == application) 
        {
            /* change GMRP global mode to opposite */
            attrType = GARP_GMRP_ATTRIBUTE;

        }

        return_value = GarpIssueCmd(application, cmdName, 
                                    L7_NULL, attrType, (GARPAttrValue)&attrValue, L7_NULL);

    }

    return return_value;  
}



/*********************************************************************
* @purpose  Determine if an interface has been acquired for any reason
*
* @param    intIfNum        interface
* @param    *status     L7_TRUE, if currently aquired
*                       L7_FALSE, if not aquired
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
*
* @notes    Storage must be allocated by the caller
*
* @end
*********************************************************************/
L7_RC_t garpIntfAcquiredGet(L7_uint32 intIfNum, L7_uint32 *status)
{
    L7_uint32 currAcquired;

    *status = L7_FALSE;

    COMPONENT_ACQ_NONZEROMASK(garpInfo.acquiredList[intIfNum],
                              currAcquired);

    if (currAcquired == L7_TRUE)
    {
        *status = L7_TRUE;

    }
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Determine if an interface has been detached for any reason
*
* @param    intIfNum        interface
* @param    *status     L7_TRUE, if currently aquired
*                       L7_FALSE, if not aquired
* @returns  L7_TRUE, if port is ready in hapi
* @returns  L7_FALSE  if port is detached in the hapi
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpIntfAttachedGet(L7_uint32 intIfNum)
{
  L7_NIM_QUERY_DATA_t nimQueryData;

  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
    /* should never get here */
    return L7_FALSE;
  }
  else if ((nimQueryData.data.state == L7_INTF_ATTACHING) ||
           (nimQueryData.data.state == L7_INTF_DETACHING) ||
           (nimQueryData.data.state == L7_INTF_ATTACHED))
  {
    /* Indicates hardware is ready */
    return L7_TRUE;
  }

  return L7_FALSE;
}

