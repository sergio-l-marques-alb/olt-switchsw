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
#include "garp_dot1q_api.h"
#include "garp_dot1q_util.h"

extern garpCfg_t     *garpCfg;
extern osapiRWLock_t garpCfgRWLock;
extern GARPCBptr     GARPCB;

#define GARP_LEAVE_TIMER_MIN_VAL      20
#define GARP_LEAVE_TIMER_MAX_VAL     600

#define GARP_LEAVEALL_TIMER_MIN_VAL  200
#define GARP_LEAVEALL_TIMER_MAX_VAL 6000

#define GARP_JOIN_TIMER_MIN_VAL       10
#define GARP_JOIN_TIMER_MAX_VAL      100


              
/*********************************************************************
* @purpose  Gets GVRP status of the device
*
* @param (out) mode    L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t garpGVRPEnableGet(L7_uint32 *mode)
{
    L7_RC_t return_value;

    return_value = garpGlobalModeGet(mode, GARP_GVRP_APP);

    return return_value;
}

/*********************************************************************
* @purpose  Gets GMRP status of the device
*
* @param (out) mode    L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t garpGMRPEnableGet(L7_uint32 *mode)
{
    L7_RC_t return_value;

    return_value = garpGlobalModeGet(mode, GARP_GMRP_APP);

    return return_value;
}




/*********************************************************************
* @purpose  Gets GVRP status of the interface
*
* @param (in)  intIfNum       Interface Number
* @param (out) status         Status of Interface intIfNum
*
* @returns  status      L7_ENABLE  or L7_DISABLE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpQportsGVRPCfgGet(L7_uint32 intIfNum, L7_uint32 *status)
{
    L7_RC_t return_value; 

    return_value = garpIntfCfgGet(intIfNum, status, GARP_GVRP_APP);

    return return_value;
}


/*********************************************************************
* @purpose  Gets GMRP status of the interface
*
* @param (in)  intIfNum       Interface Number
* @param (out) status         Status of Interface intIfNum
*
* @returns  status      L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpQportsGMRPCfgGet(L7_uint32 intIfNum, L7_uint32 *status)
{
    L7_RC_t return_value; 

    return_value = garpIntfCfgGet(intIfNum, status, GARP_GMRP_APP);

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
L7_RC_t garpIntfGVRPModeSet(L7_uint32 intIfNum, L7_uint32 status)
{
  L7_uint32 i,start,end;
  garpIntfCfgData_t *pCfg        = (garpIntfCfgData_t *)L7_NULL;
  L7_RC_t rc= L7_FAILURE;

  /* Validity checks */
  if ((status != L7_ENABLE) && (status != L7_DISABLE))
      return L7_FAILURE;

  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  if (intIfNum != L7_ALL_INTERFACES) 
  {
      if (garpIsValidIntf(intIfNum) != L7_TRUE) 
      {
          return L7_FAILURE;
      }

      
      (void)osapiReadLockTake(garpCfgRWLock, L7_WAIT_FOREVER); 
      if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
      {

        if (pCfg->gvrp_enabled == status)
        {
          (void)osapiReadLockGive(garpCfgRWLock); 
          return L7_SUCCESS;
        }
        else
          (void)osapiReadLockGive(garpCfgRWLock);  
      }

  }


            
  /*   Apply configuration */

  /* The L7_ALL_INTERFACES is passed to the GarpIssueCmd and handled on the garpCPTask */

  (void)garpIntfGVRPModeApply(intIfNum, status);

  /* Save configuration */

  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = L7_MAX_INTERFACE_COUNT;
    for (i=start;i<end;i++)
    {  
        (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
        garpCfg->intf[i].gvrp_enabled = status;
        garpCfg->hdr.dataChanged = L7_TRUE;
        (void)osapiWriteLockGive(garpCfgRWLock);

    }
    rc=L7_SUCCESS;

  }
  else
  {
      if(pCfg != (garpIntfCfgData_t *)L7_NULL)
      {
        (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
        pCfg->gvrp_enabled = status;
        garpCfg->hdr.dataChanged = L7_TRUE;
        (void)osapiWriteLockGive(garpCfgRWLock);
        rc=L7_SUCCESS;

      }
      else
      {
        if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
        {
          (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
          pCfg->gvrp_enabled = status;
          garpCfg->hdr.dataChanged = L7_TRUE;
          (void)osapiWriteLockGive(garpCfgRWLock);
          rc=L7_SUCCESS;
        }
      }
  }

    
 
  
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
L7_RC_t garpIntfGMRPModeSet(L7_uint32 intIfNum, L7_uint32 status)
{
  L7_uint32 i,start,end;
  garpIntfCfgData_t *pCfg        = (garpIntfCfgData_t *)L7_NULL;
  L7_RC_t rc= L7_FAILURE;


  /* Validity checks */
  if ((status != L7_ENABLE) && (status != L7_DISABLE))
      return L7_FAILURE;

  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  if (intIfNum != L7_ALL_INTERFACES) 
  {
      if (garpIsValidIntf(intIfNum) != L7_TRUE) 
      {
          return L7_FAILURE;
      }

      
      (void)osapiReadLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
      if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
      {

        if (pCfg->gmrp_enabled == status)
        {
          (void)osapiReadLockGive(garpCfgRWLock);
          return L7_SUCCESS;
        }
        else
          (void)osapiReadLockGive(garpCfgRWLock);
      }
  }

  /*   Apply configuration */


  /* The L7_ALL_INTERFACES is passed to the GarpIssueCmd and handled on the garpCPTask */

  (void)garpIntfGMRPModeApply(intIfNum, status);

  /* Save configuration */
  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = L7_MAX_INTERFACE_COUNT;

    for (i=start;i<end;i++)
    {

        (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
        garpCfg->intf[i].gmrp_enabled = status;
        garpCfg->hdr.dataChanged  = L7_TRUE;
        (void)osapiWriteLockGive(garpCfgRWLock);

    }
    rc=L7_SUCCESS;
  }

  else
  {
     (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
     if(pCfg != (garpIntfCfgData_t *) L7_NULL)
     {
       pCfg->gmrp_enabled = status;
       garpCfg->hdr.dataChanged = L7_TRUE;
       rc=L7_SUCCESS;

     }
     else
     {
       if (L7_TRUE == garpMapIntfIsConfigurable(intIfNum, &pCfg))
       {
         pCfg->gmrp_enabled = status;
         garpCfg->hdr.dataChanged = L7_TRUE;
         rc=L7_SUCCESS;
       }
     }
     (void)osapiWriteLockGive(garpCfgRWLock);

  }

 
  return(rc);
}


/*********************************************************************
* @purpose  Sets GARP Join Time
*
* @param (in)  intIfNum  interface number
* @param (in)  time      Time (in centiseconds) to be set 
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    The GARP Leave Timer must be greater than or equal to 
*           three times the GARP Join Timer.
*
* @end
*********************************************************************/
L7_RC_t  garpJoinTimeSet(L7_uint32 intIfNum, L7_uint32 time)
{
  L7_uint32 leaveTime;

  /* check validity of join timer */
  if ((time < GARP_JOIN_TIMER_MIN_VAL) || (time > GARP_JOIN_TIMER_MAX_VAL) ||
      (garpTimeGet(intIfNum, &leaveTime, GARP_LEAVE_TIME) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  /* check if the join timer condition is satisfied */
  if (leaveTime < (GARP_TIMER_MULTIPLICATION_VAL * time))
  {
    return L7_ERROR;
  }

  return garpTimeApply(intIfNum, time, GARP_JOIN_TIME);
}

/*********************************************************************
* @purpose  Sets GARP Leave Time
*
* @param (in)  intIfNum  interface number
* @param (in)  time      Time (in centiseconds) to be set
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    The GARP LeaveAll Timer must be greater than the GARP Leave Timer.
*           Also, the GARP Leave Timer must be greater than or equal to 
*           three times the GARP Join Timer.
*
* @end
*********************************************************************/
L7_RC_t garpLeaveTimeSet(L7_uint32 intIfNum, L7_uint32 time)
{
  L7_uint32 leaveAllTime, joinTime;

  /* check validity of leave timer */
  if ((time < GARP_LEAVE_TIMER_MIN_VAL) || (time > GARP_LEAVE_TIMER_MAX_VAL) ||
      (garpTimeGet(intIfNum, &leaveAllTime, GARP_LEAVEALL_TIME) != L7_SUCCESS) ||
      (garpTimeGet(intIfNum, &joinTime, GARP_JOIN_TIME) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  /* check if the leave timer conditions are satisfied */

  if (leaveAllTime <= time)
  {
    return L7_REQUEST_DENIED;
  }

  if (time < (GARP_TIMER_MULTIPLICATION_VAL * joinTime))
  {
    return L7_ERROR;
  }

  return garpTimeApply(intIfNum, time, GARP_LEAVE_TIME);
}


/*********************************************************************
* @purpose  Sets GARP Leave All Time
*
* @param (in)  intIfNum  interface number
* @param (in)  time      Time (in centiseconds) to be set
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    The GARP LeaveAll Timer must be greater than the GARP Leave Timer.
*
* @end
*********************************************************************/
L7_RC_t garpLeaveAllTimeSet(L7_uint32 intIfNum, L7_uint32 time)
{
  L7_uint32 leaveTime;

  /* check validity of leaveall timer */
  if ((time < GARP_LEAVEALL_TIMER_MIN_VAL) || (time > GARP_LEAVEALL_TIMER_MAX_VAL) ||
      (garpTimeGet(intIfNum, &leaveTime, GARP_LEAVE_TIME) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  /* check if the leaveall timer condition is satisfied */
  if (time <= leaveTime)
  {
    return L7_ERROR;
  }

  return garpTimeApply(intIfNum, time, GARP_LEAVEALL_TIME);
}

/*********************************************************************
* @purpose  Gets GARP Join Time
*
* @param (in)   UnitIndex  unused
* @param (in)   intIfNum   interface number
* @param (out)  time       Time (in centiseconds) read
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpJoinTimeGet(L7_uint32 UnitIndex, 
                        L7_uint32 intIfNum, L7_uint32 *time)
{
    L7_RC_t return_value;

    return_value = garpTimeGet(intIfNum, time, GARP_JOIN_TIME);

    return return_value;
}


/*********************************************************************
* @purpose  Gets GARP Leave Time
*
* @param (in)   UnitIndex  unused
* @param (in)   intIfNum   interface number
* @param (out)  time       Time (in centiseconds) read
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpLeaveTimeGet(L7_uint32 UnitIndex, 
                         L7_uint32 intIfNum, L7_uint32 *time)
{
    L7_RC_t return_value;

    return_value = garpTimeGet(intIfNum, time, GARP_LEAVE_TIME);

    return return_value;
}


/*********************************************************************
* @purpose  Gets GARP Leave All Time
*
* @param (in)   UnitIndex  unused
* @param (in)   intIfNum   interface number
* @param (out)  time       Time (in centiseconds) read
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpLeaveAllTimeGet(L7_uint32 UnitIndex, 
                            L7_uint32 intIfNum, L7_uint32 *time)
{
    L7_RC_t return_value;

    return_value = garpTimeGet(intIfNum, time, GARP_LEAVEALL_TIME);

    return return_value;
}



/*********************************************************************
* @purpose  Enable or Disable GVRP global mode for the device
*
* @param (in) command   L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpGVRPModeSet(L7_uint32 command)
{
    L7_RC_t return_value;

    if ( (command != L7_TRUE) && (command != L7_FALSE) )
        return L7_FAILURE;

    if (garpCfg->gvrp_enabled == command)
        return L7_SUCCESS;

    return_value = garpModeApply(command, GARP_GVRP_APP);

    (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);
    
    garpCfg->gvrp_enabled = command; 
    garpCfg->hdr.dataChanged = L7_TRUE;


    (void)osapiWriteLockGive(garpCfgRWLock);


    return return_value;
}



/*********************************************************************
* @purpose  Enable or Disable GMRP global mode for the device
*
* @param (in) command   L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpGMRPModeSet(L7_uint32 command)
{
    L7_RC_t return_value;


    if ( (command != L7_TRUE) && (command != L7_FALSE) )
        return L7_FAILURE;

    if (garpCfg->gmrp_enabled == command)
        return L7_SUCCESS;

    return_value = garpModeApply(command, GARP_GMRP_APP);

    (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

    garpCfg->gmrp_enabled = command;
    garpCfg->hdr.dataChanged = L7_TRUE;


    (void)osapiWriteLockGive(garpCfgRWLock);


    return return_value;
}


