/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_api.c
*
* @purpose dot1Q APIs
*
* @component dot1q
*
* @comments none
*
* @create 08/14/2000
*
* @author wjacobs,djohnson
*
* @end
*
**********************************************************************/

#include "commdefs.h"
#define DOT1D_PRIORITY_TABLE FACTORY_DEFAULT_REFER

#include <string.h>
#include "flex.h"
#include "dot1q_common.h"
#include "osapi.h"
#include "dtlapi.h"
#include "dot3ad_api.h"
#include "nimapi.h"
#include "dot1q_data.h"
#include "dot1q_cfg.h"
#include "dot1q_control.h"
#include "dot1q_intf.h"
#include "dot1q_debug.h"

#if defined(FEAT_METRO_CPE_V1_0)
#include "dot1ad_api.h"
#include "dot1ad_l2tp_api.h"
#include "simapi.h"
#endif

extern  vlanTree_t          *pVlanTree;
extern  dot1qIntfData_t     *dot1qQports;
extern  dot1qCfgData_t      *dot1qCfg;
extern  dot1qInfo_t         *dot1qInfo;
extern  osapiRWLock_t       dot1qCfgRWLock;
extern void * dot1qVlanCountSem;
extern void * dot1qInternalVlanSem;
extern dot1qInternalVlanInfo_t  *dot1qInternalVlanInfo;


extern  NIM_INTF_MASK_t     dot1qValidMemberPortsMask;
extern  dot1qVlanIntf_t     dot1qVlanIntf[DOT1Q_MAX_VLAN_INTF + 1];
extern  L7_VLAN_MASK_t      dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];

#if defined(FEAT_METRO_CPE_V1_0)
int overrideMgmtVlanSet = 0;

void overrideMgmtVlanSetModiy(L7_uint32 val)
{
  overrideMgmtVlanSet = val;
}
#endif

/* PTin added: ptin_evc
   Check if dot1q message queue is empty */
extern void *dot1qMsgQueue;
L7_BOOL dot1qQueueIsEmpty(void)
{
  L7_uint32 num;

  if (osapiMsgQueueGetNumMsgs(dot1qMsgQueue, &num) != L7_SUCCESS)
    return L7_FALSE;

  return (num == 0);
}
/* PTin end */

/*********************************************************************
* @purpose  Create a new VLAN
*
* @param    vid         VLAN ID
*           requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCreate(L7_uint32 vid, L7_uint32 requestor)
{
    L7_RC_t rc;
    DOT1Q_MSG_t msg;
    dot1qVlanCfgData_t *vCfg;
    L7_BOOL cfgEntryFound;
    rc = L7_FAILURE;


    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID) ||
        (vid == L7_DOT1Q_DEFAULT_VLAN))
    {
        return rc;
    }

    if ((requestor != DOT1Q_ADMIN) &&
        (requestor != DOT1Q_GVRP)&&
        (requestor != DOT1Q_IP_INTERNAL_VLAN)&&
      	(requestor != DOT1Q_DOT1X)&& 
        (requestor != DOT1Q_WS_L2TUNNEL))

    {
        return rc;
    }


  if (dot1qVlanCreateIsPending(vid) == L7_TRUE)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,"VLAN create currently underway for VLAN ID  %d",vid);
      return L7_FAILURE;
  }

    /* Check to see if an new VLAN can be created */
    if ( (dot1qCurrentVlanCountGet() + 1) > L7_MAX_VLANS)
    {
        return rc;
    }


    /* Handle administratively created VLANs */
    if (requestor == DOT1Q_ADMIN)
    {
        (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

        /* Check for pre-existence of the static VLAN */
        if (dot1qVlanIsConfigurable(vid, &vCfg) == L7_TRUE)
        {
            /* Static vlan already exists */
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,"VLAN ID %d is already exists as static VLAN",vid);
            (void)osapiWriteLockGive(dot1qCfgRWLock);
            return L7_SUCCESS;
        }

        cfgEntryFound = L7_FALSE;
        /* attempt to allocate an entry */
        if(dot1qVlanConfigEntryGet(vid, &vCfg) == L7_TRUE)
        {
            cfgEntryFound = L7_TRUE;
        }

        if (cfgEntryFound == L7_TRUE)
        {

            memset((void *)vCfg, 0, sizeof(dot1qVlanCfgData_t));
            /* Update cfg */
            vCfg->vlanId = vid;

            /* Set dataChanged flag */
            dot1qCfg->hdr.dataChanged = L7_TRUE;

            /* Increment current count immediately.
               If subsequent VLAN create requests are
               received before the VLAN create is actually
               processed, the count will err on the side of
               caution.
             */
            dot1qCurrentVlanCountUpdate(L7_TRUE);

        }
        (void)osapiWriteLockGive(dot1qCfgRWLock);
    } /* requestor == DOT1Q_ADMIN */
    else
    {

        /* Handle dynamic VLANs for which an instantaneous VLAN ID is needed
           e.g. do not wait around for processing */
        if (requestor == DOT1Q_IP_INTERNAL_VLAN)
        {
            /* This VLAN type will never have a port member.  This is purely a patch for hardware.
               So allow for instantaneous feedback to the caller of probable success in creation */
            (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
            if (dot1qVlanCreatePendingSet(vid, requestor) != L7_SUCCESS)
            {
              (void)osapiWriteLockGive(dot1qCfgRWLock);
              return L7_FAILURE;
            }
            (void)osapiWriteLockGive(dot1qCfgRWLock);
        }

        /* Go ahead and increment VLAN count */

        dot1qCurrentVlanCountUpdate(L7_TRUE);
    }

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.vlanId = vid;
    msg.requestor = requestor;
    msg.event =
     (requestor == DOT1Q_GVRP || requestor == DOT1Q_WS_L2TUNNEL || requestor == DOT1Q_DOT1X || requestor == DOT1Q_IP_INTERNAL_VLAN) ? dot1q_vlan_create_dynamic :  dot1q_vlan_create_static;


    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;

}

/*********************************************************************
* @purpose  Delete a VLAN
*
* @param    vid         VLAN ID
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanDelete(L7_uint32 vid, L7_uint32 requestor)
{

    L7_RC_t rc;
    DOT1Q_MSG_t msg;

    rc = L7_FAILURE;

    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID) ||
        (vid == L7_DOT1Q_DEFAULT_VLAN))
    {
        return L7_FAILURE;
    }

    if ((requestor != DOT1Q_ADMIN) &&
        (requestor != DOT1Q_GVRP) &&
        (requestor != DOT1Q_WS_L2TUNNEL) &&
        (requestor != DOT1Q_DOT1X)&& 
        (requestor != DOT1Q_IP_INTERNAL_VLAN) )

    {
        return L7_FAILURE;
    }


    /* Handle administratively created VLANs */
    if (requestor == DOT1Q_ADMIN)
    {
        (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
        if (dot1qVlanConfigEntryReset(vid) != L7_SUCCESS)
        {
            (void)osapiWriteLockGive(dot1qCfgRWLock);
            return L7_FAILURE;
        }

        /* Set dataChanged flag */
        dot1qCfg->hdr.dataChanged = L7_TRUE;

        (void)osapiWriteLockGive(dot1qCfgRWLock);
        /* Decrement current count */
        dot1qCurrentVlanCountUpdate(L7_FALSE);

    } /* requestor == DOT1Q_ADMIN */
    else
    {
        /* Go ahead and decrement VLAN count */
       dot1qCurrentVlanCountUpdate(L7_FALSE);
    }


    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.vlanId = vid;
    msg.requestor = requestor;
    msg.event =
     (requestor == DOT1Q_GVRP || requestor == DOT1Q_WS_L2TUNNEL || requestor == DOT1Q_DOT1X || requestor == DOT1Q_IP_INTERNAL_VLAN) ? dot1q_vlan_delete_dynamic :  dot1q_vlan_delete_static;

    /* Put a message on the queue */
    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;



}

/*********************************************************************
* @purpose  Set a vlan name
*
* @param    vid         vlan ID
* @param    *name       pointer to string defining vlan name,
*                       max length L7_MAX_VLAN_NAME
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_NOT_SUPPORTED, if attempt to configure a default vlan
* @returns  L7_NO_VALUE, if attempt to configure a unknown vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    Length of name is L7_MAX_VLAN_NAME characters
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanNameSet(L7_uint32 vid, L7_uchar8 *name)
{
  dot1qVlanCfgData_t *vCfg = L7_NULL;
  L7_RC_t rc = L7_SUCCESS;


  /* range check*/
  if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
  {
      return L7_FAILURE;
  }
  if (vid == L7_DOT1Q_DEFAULT_VLAN)
  {
    return L7_NOT_SUPPORTED;
  }

  /* Update the configuration */
  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  (void) dot1qVlanIsConfigurable(vid, &vCfg);
  if (vCfg != L7_NULL)
  {
    osapiStrncpySafe((L7_char8 *)(vCfg->vlanCfg.name),(L7_char8 *)name, L7_MAX_VLAN_NAME);
    dot1qCfg->hdr.dataChanged = L7_TRUE;
  }
  else
  {
    L7_uint32 type;
 
    if(dot1qOperVlanTypeGet(vid,  &type) == L7_SUCCESS)
    {
      switch(type)
      {
        case L7_DOT1Q_TYPE_GVRP_REGISTERED:
        case L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED:
        case L7_DOT1Q_TYPE_IP_INTERNAL_VLAN:
        case L7_DOT1Q_TYPE_DOT1X_REGISTERED:
          rc = L7_ERROR;  /* Dynamic VLAN */
          break;
     
        default:
          rc = L7_NO_VALUE;  /* Unknown VLAN */
          break; 
      }
    }
    else
    {
      /* Vlan does not exist yet */
      rc = L7_NOT_EXIST;
    }
  }

  (void)osapiWriteLockGive(dot1qCfgRWLock);
  return rc;

}

/*********************************************************************
* @purpose  Get a vlan name
*
* @param    vid         vlan ID
* @param    *name       pointer to string in which to store vlan name
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXIST, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    Length of name is L7_MAX_VLAN_NAME characters
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanNameGet(L7_uint32 vid, L7_uchar8 *name)
{

  dot1qVlanCfgData_t *vCfg;

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  vCfg = L7_NULL;
  (void) dot1qVlanIsConfigurable(vid, &vCfg);
  if (vCfg == L7_NULL)
  {
      (void)osapiReadLockGive(dot1qCfgRWLock);
      return L7_FAILURE;
  }

  strncpy((L7_char8 *)name, (L7_char8 *)(vCfg->vlanCfg.name), L7_MAX_VLAN_NAME);

  (void)osapiReadLockGive(dot1qCfgRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Get a vlan ID from a vlan name
*
* @param    *vlanName   name of a VLAN
* @param    *vlanId     VLAN ID
*
* @returns  L7_SUCCESS, if name found
* @returns  L7_FAILURE, if no VLAN matches the name
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIdFromNameGet(const L7_char8 *vlanName, L7_uint32 *vlanId)
{
  L7_uint32 i;
  dot1qVlanCfgData_t *vCfg = NULL;

  if (vlanName == L7_NULL)
  {
    return L7_FAILURE;
  }

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  for (i = 1; i <= L7_DOT1Q_MAX_VLAN_ID; i++)
  {
    if (!dot1qVlanIsConfigurable(i, &vCfg) || (vCfg == NULL))
      continue;

    if (strcmp(vCfg->vlanCfg.name, vlanName) == 0)
    {
      *vlanId = i;
      (void)osapiReadLockGive(dot1qCfgRWLock);
      return L7_SUCCESS;
    }
  }

  *vlanId = 0;
  (void)osapiReadLockGive(dot1qCfgRWLock);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get a VLAN's Filtering Database ID - (Operational Data)
*
* @param    vid         vlan ID
* @param    *fdbID      pointer to Filtering Database ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This routine gets operational data
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanFdbIDGet(L7_uint32 vid, L7_uint32 *fdbID)
{

  /* This read-only Q-BRIDGE-MIB object dot1qVlanFdbId is
     obtained by the routine fdbFdbIdGet() */


  return(L7_NOT_SUPPORTED);
}


static void dot1qAllIntfMaskGet(NIM_INTF_MASK_t *intMask)
{
  memset(intMask, 0, sizeof(NIM_INTF_MASK_t));
  NIM_INTF_MASKINV((*intMask));

}

/*********************************************************************
* @purpose  Set VLAN port membership
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be added
*                       or L7_ALL_INTERFACES add all interfaces
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    requestor DOT1Q_ADMIN is for all non GVRP requests
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanMemberSet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 mode, L7_uint32 requestor, DOT1Q_SWPORT_MODE_t swport_mode)
{
  L7_RC_t rc= L7_FAILURE;
  DOT1Q_MSG_t msg;
  dot1qVlanCfgData_t *vCfg = L7_NULL;
  L7_uint32 vlan_mode,vlan_type,access_vlan;
  DOT1Q_SWPORT_MODE_t curr_mode;
  NIM_INTF_MASK_t allIntMask;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);


  /* range check*/
  if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
  {
      return rc;
  }

  if ((mode != L7_DOT1Q_FIXED) &&
      (mode !=L7_DOT1Q_NORMAL_REGISTRATION) &&
      (mode !=L7_DOT1Q_FORBIDDEN))
  {
      return L7_FAILURE;
  }

  if(intIfNum == L7_ALL_INTERFACES)
  {
      /* Use the mask API available for ALL interfaces command*/
      dot1qAllIntfMaskGet(&allIntMask);
      return dot1qVlanMemberMaskSet(vid, &allIntMask, mode, swport_mode);
  }

  if (dot1qSwitchPortModeGet(intIfNum, &curr_mode) != L7_SUCCESS)
  {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID, "Cannot get the switchport mode Interface: %s", ifName);
  }

  if (requestor == DOT1Q_ADMIN)
  {
    if ((curr_mode != swport_mode) &&
        (swport_mode != DOT1Q_SWPORT_MODE_NONE))
    {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID, "Incorrect switchport mode for vlan participation. "
                                                             "Mode: %d Interface: %s", swport_mode,  ifName);
        return L7_FAILURE;
    }
  }

  if (curr_mode == DOT1Q_SWPORT_MODE_ACCESS)
  {
     if (dot1qSwitchPortAccessVlanGet(intIfNum, &access_vlan) == L7_SUCCESS)
     {
       if (mode == L7_DOT1Q_FIXED)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Port(%s) with switchport mode Access cannot participate in any vlan", ifName);
       return L7_FAILURE;
  }
       else if (mode == L7_DOT1Q_FORBIDDEN && access_vlan == vid)
       {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Port(%s) with switchport mode Access cannot be forbidden in configured access vlan", ifName);
           return L7_FAILURE;
       }
     }
     else
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID, "Cannot get the access vlan for Interface: %s", ifName);
     }
  }


  rc= dot1qVlanMemberAndTypeGet(vid,intIfNum, &vlan_mode,&vlan_type);
  if (rc == L7_NOT_EXISTS)
  {
    /* VLAN may not have been created. Flush dot1q queue to process pending create event. */
    dot1qApiQueueSynchronize(vid);
    rc= dot1qVlanMemberAndTypeGet(vid,intIfNum, &vlan_mode,&vlan_type);
  }

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID, "Invalid vlan %d used.", vid);
  }

  /* Do not allow operations on internal vlan */
  if (vlan_type == L7_DOT1Q_TYPE_IP_INTERNAL_VLAN)
  {
    return L7_FAILURE;
  }

  if (requestor!=DOT1Q_ADMIN)
  {
    /*Defect 49394 :Port association is happening even when the vlan is configured as forbidded for that port*/
    if ((vlan_type==L7_DOT1Q_ADMIN) && (vlan_mode==L7_DOT1Q_FORBIDDEN))
    {
      return L7_FAILURE;
    }
  }

  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  /* Update the config structure */
  if (requestor == DOT1Q_ADMIN)
  {
      (void) dot1qVlanIsConfigurable(vid, &vCfg);
      if (vCfg == L7_NULL)
      {
          /* A static VLAN does not exist */
          (void)osapiWriteLockGive(dot1qCfgRWLock);
          return L7_NOT_EXISTS;
      }

      (void)dot1qVlanMemberSetCfgUpdate(vid, intIfNum, mode);
      if (((FD_DOT1Q_DEFAULT_TAGGING == L7_DOT1Q_TAGGED) || (swport_mode == DOT1Q_SWPORT_MODE_TRUNK))
          && (mode == L7_DOT1Q_FIXED))
      {
        dot1qVlanTaggedMemberSetCfgUpdate(vid,intIfNum, L7_DOT1Q_TAGGED);
      }
      dot1qCfg->hdr.dataChanged = L7_TRUE;
  }

  (void)osapiWriteLockGive(dot1qCfgRWLock);
#if 0
#if defined(FEAT_METRO_CPE_V1_0)
  if (!overrideMgmtVlanSet)
  {
    DOT1AD_INTFERFACE_TYPE_t intfType;

    if ((mode == L7_DOT1Q_FIXED || mode == L7_DOT1Q_NORMAL_REGISTRATION) &&
        vid == simMgmtVlanIdGet() &&
        dot1adInterfaceTypeGet(intIfNum, &intfType) == L7_SUCCESS &&
        ( (intfType == DOT1AD_INTFERFACE_TYPE_UNI) ||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P) ||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
        )
       )

    {
      return L7_FAILURE;
    }
  }
#endif
#endif
  /* Put a message on the queue */

  memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
  msg.vlanId = vid;
  msg.data.intIfNum = intIfNum;
  msg.mode = mode;
  msg.event = dot1q_vlan_member_set;
  msg.swport_mode = swport_mode;
  msg.requestor = requestor;


  rc = dot1qIssueCmd(&msg);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
  }

    return rc; /* serviced in request tree */
}


/*********************************************************************
* @purpose  Get VLAN port membership
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
* @param    *mode       pointer to mode of participation
*                       (@b{  L7_DOT1Q_INCLUDE, L7_DOT1Q_EXCLUDE})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *mode)
{

    dot1qVlanCfgData_t *vCfg = L7_NULL;


    if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    {
       return L7_FAILURE;
    }

    if (mode == L7_NULL)
    {
       return L7_FAILURE;
    }

    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    (void) dot1qVlanIsConfigurable(vid, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiReadLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }

    if (NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.staticEgressPorts, intIfNum))
    {
      *mode = L7_DOT1Q_FIXED;
    }
    else if (NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.forbiddenEgressPorts,intIfNum))
    {
      *mode = L7_DOT1Q_FORBIDDEN;
    }
    else
      *mode = L7_DOT1Q_NORMAL_REGISTRATION;


    (void)osapiReadLockGive(dot1qCfgRWLock);

    return(L7_SUCCESS);

}

/*********************************************************************
* @purpose  Set the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanTaggedMemberSet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 tagMode,
                                   vlanRequestor_t requestor)
{
    L7_RC_t rc= L7_FAILURE;
    DOT1Q_MSG_t msg;
    dot1qVlanCfgData_t *vCfg = L7_NULL;

    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
    {
        return rc;
    }

    if ((tagMode != L7_DOT1Q_UNTAGGED) &&
        (tagMode !=L7_DOT1Q_TAGGED))
    {
        return L7_FAILURE;
    }

    /* The interface number can be L7_ALL_INTERFACES, in which case
     * we cannot validate the parameter using dot1qIntfIsValid().
     */
    if ((intIfNum != L7_ALL_INTERFACES) && 
        (dot1qIntfIsValid(intIfNum) != L7_TRUE))
    {
      return L7_FAILURE;
    }

    /* Update the configuration file */
    if (DOT1Q_ADMIN == requestor)
    {
      (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

      (void) dot1qVlanIsConfigurable(vid, &vCfg);
      if (vCfg == L7_NULL)
      {
        /* A static VLAN does not exist */
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_NOT_EXISTS;
      }

      (void)dot1qVlanTaggedMemberSetCfgUpdate(vid, intIfNum, tagMode);
      dot1qCfg->hdr.dataChanged = L7_TRUE;

      (void)osapiWriteLockGive(dot1qCfgRWLock);
    }

    /* Put a message on the queue */
    memset (&msg, 0x0, sizeof(msg));

    msg.vlanId = vid;
    msg.data.intIfNum = intIfNum;
    msg.mode = tagMode;
    msg.event = dot1q_vlan_tagged_set;
    msg.requestor = requestor;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;
}



/*********************************************************************
* @purpose  Get the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
* @param    *tagMode     Pointer to tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Use dot1q_set_vlanCfg() instead of this routine when possible
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTaggedMemberGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *tagMode)
{

    dot1qVlanCfgData_t *vCfg;

    /* initialze*/

    vCfg = L7_NULLPTR;

    if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    {
       return L7_FAILURE;
    }

    if (tagMode == L7_NULL)
    {
       return L7_FAILURE;
    }


    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    (void) dot1qVlanIsConfigurable(vid, &vCfg);
    if (vCfg == L7_NULL)
    {
       (void)osapiReadLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }

    if (NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.taggedSet, intIfNum))
      *tagMode = L7_DOT1Q_TAGGED;
    else
      *tagMode = L7_DOT1Q_UNTAGGED;

    (void)osapiReadLockGive(dot1qCfgRWLock);

    return(L7_SUCCESS);

}


#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
/*********************************************************************
* @purpose  Set the group filter configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    filterMode  group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanGroupFilterSet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 filterType)
{
   L7_RC_t rc= L7_NOT_SUPPORTED;

  return rc;
}

/*********************************************************************
* @purpose  Get the group filter configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
* @param    *filterMode  pointer to group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanGroupFilterGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *filterType)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  if (NIM_INTF_ISMASKBITSET(dataEntry.admin->groupFilter.forwardAllMask, intIfNum))
    *filterType = L7_DOT1Q_FORWARD_ALL;
  else if (NIM_INTF_ISMASKBITSET(dataEntry.admin->groupFilter.forwardUnregMask, intIfNum))
    *filterType = L7_DOT1Q_FORWARD_UNREGISTERED;
  else
    *filterType = L7_DOT1Q_FILTER_UNREGISTERED;

  return(L7_SUCCESS);
}

#endif




/*********************************************************************
* @purpose  Get the VLAN static egress ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which are members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticEgressPortsGet(L7_uint32 vlanId, NIM_INTF_MASK_t * mask,
                                        L7_uint32 *size)
{
    dot1qVlanCfgData_t *vCfg=L7_NULL;



    if (mask == L7_NULL)
    {
       return L7_FAILURE;
    }

    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiReadLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }


    *size = NIM_INTF_INDICES;
    (void)dot1qReverseMask(vCfg->vlanCfg.staticEgressPorts, mask);

    (void)osapiReadLockGive(dot1qCfgRWLock);

    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Set the VLAN static egress ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which are members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticEgressPortsSet(L7_uint32 vlanId, NIM_INTF_MASK_t * mask)
{

    L7_RC_t rc, rc_iter, rv_iter = L7_SUCCESS;
    DOT1Q_MSG_t msg;
    NIM_INTF_MASK_t tempMask;
    L7_uint32 vid, intIfNum, mode;
    dot1qVlanCfgData_t *vCfg;
    dot1qIntfCfgData_t *pCfg = L7_NULL;



    rc  = L7_FAILURE;
    vid = vlanId;

    /* range check*/
    if ( (vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
    {
        return L7_FAILURE;
    }

    if (mask == L7_NULL)
    {
       return L7_FAILURE;
    }

    memset(&tempMask, 0, sizeof(tempMask));
    /* Check that all ports in the mask can participate in dot1q */

    (void)dot1qReverseMask(*mask, &tempMask);

    if (dot1qIntfIsValidMask( (L7_INTF_MASK_t *)&tempMask) != L7_TRUE)
    {
        return L7_FAILURE;
    }

    /* Update the config structure */
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    vCfg = L7_NULL;

    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }

    /* Check if any of the ports are configured as access port */
    intIfNum = L7_NULL;
    rc_iter = nimFirstValidIntfNumber(&intIfNum);
    while (rc_iter == L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      if (NIM_INTF_ISMASKBITSET(tempMask, intIfNum) !=
          NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.staticEgressPorts, intIfNum)
         )
      {
         if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
         {
           mode = pCfg->portCfg.switchport_mode;
           if (mode == DOT1Q_SWPORT_MODE_ACCESS)
           {
              if ( pCfg->portCfg.access_vlan != vlanId ||
                  (pCfg->portCfg.access_vlan == vlanId && !NIM_INTF_ISMASKBITSET(tempMask, intIfNum)))
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                      "Invalid Port List. Cannot configure VLAN membership for an access port %s", ifName);
                rv_iter = L7_FAILURE;
              }
           }
         }
      }/* End of membership change check */
      rc_iter = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }/* End of interface iterations */

    if (rv_iter == L7_FAILURE)
    {
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_FAILURE;
    }

    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg, 0x0, sizeof(DOT1Q_MSG_t));

    /* Put a message on the queue */
    msg.vlanId = vlanId;
    msg.event = dot1q_vlan_member_set_static_mask;
    msg.mode =  L7_DOT1Q_FIXED ;
    memcpy(&msg.data.intfMask, &tempMask, sizeof(NIM_INTF_MASK_t));

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;
}


/*********************************************************************
* @purpose  Set the VLAN Forbidden Egress Ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which cannot be members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanForbiddenEgressPortsSet(L7_uint32 vlanId, NIM_INTF_MASK_t * mask)
{


    L7_RC_t rc, rc_iter, rv_iter = L7_SUCCESS;
    DOT1Q_MSG_t msg;
    NIM_INTF_MASK_t tempMask;
    L7_uint32 vid, intIfNum, mode;
    dot1qVlanCfgData_t *vCfg;
    dot1qIntfCfgData_t *pCfg = L7_NULL;



    rc  = L7_FAILURE;
    vid = vlanId;

    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
    {
        return L7_FAILURE;
    }

    if (mask == L7_NULL)
    {
       return L7_FAILURE;
    }


    /* Check that all ports in the mask can participate in dot1q */

    (void)dot1qReverseMask(*mask, &tempMask);

    if (dot1qIntfIsValidMask( (L7_INTF_MASK_t *)&tempMask) != L7_TRUE)
    {
        return L7_FAILURE;
    }

    /* Update the config structure */
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    vCfg = L7_NULL;

    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }

    /* Check if any of the ports are configured as access port */
    intIfNum = L7_NULL;
    rc_iter = nimFirstValidIntfNumber(&intIfNum);
    while (rc_iter == L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      if (NIM_INTF_ISMASKBITSET(tempMask, intIfNum) !=
          NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.forbiddenEgressPorts, intIfNum)
         )
      {
         if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
         {
           mode = pCfg->portCfg.switchport_mode;
           if (mode == DOT1Q_SWPORT_MODE_ACCESS)
           {
              if ((pCfg->portCfg.access_vlan == vlanId && NIM_INTF_ISMASKBITSET(tempMask, intIfNum)))
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                        "Invalid Port List. Cannot forbid VLAN membership for an access port %s on its access vlan", ifName);
                rv_iter = L7_FAILURE;
              }
           }
         }
      }/* End of membership change check */
      rc_iter = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }/* End of interface iterations */

    if (rv_iter == L7_FAILURE)
    {
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_FAILURE;
    }

    memcpy(&(vCfg->vlanCfg.forbiddenEgressPorts), &tempMask, sizeof(NIM_INTF_MASK_t));
    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg, 0x0, sizeof(DOT1Q_MSG_t));

    msg.vlanId = vlanId;
    msg.event = dot1q_vlan_member_set_static_mask;
    msg.mode = L7_DOT1Q_FORBIDDEN;
    memcpy(&msg.data.intfMask, &tempMask, sizeof(NIM_INTF_MASK_t));



    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;
}


/*********************************************************************
* @purpose  Get the VLAN Forbidden Egress Ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which cannot be members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanForbiddenEgressPortsGet(L7_uint32 vlanId, NIM_INTF_MASK_t *mask,
                                           L7_uint32 *size)
{

    dot1qVlanCfgData_t *vCfg=L7_NULL;



    if (mask == L7_NULL)
    {
       return L7_FAILURE;
    }

    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    vCfg = L7_NULL;
    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiReadLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }


    *size = NIM_INTF_INDICES;
    (void)dot1qReverseMask(vCfg->vlanCfg.forbiddenEgressPorts, mask);

    (void)osapiReadLockGive(dot1qCfgRWLock);

    return L7_SUCCESS;


}


/*********************************************************************
* @purpose  Get set of ports which are transmitting untagged frames
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticUntaggedPortsGet(L7_uint32 vlanId, NIM_INTF_MASK_t * mask,
                                          L7_uint32 *size)
{

    dot1qVlanCfgData_t *vCfg;
    NIM_INTF_MASK_t     tempMask;



    if (mask == L7_NULL)
    {
       return L7_FAILURE;
    }

    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    vCfg = L7_NULL;
    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiReadLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }


    memcpy(&tempMask, &(vCfg->vlanCfg.taggedSet), sizeof(NIM_INTF_MASK_t));
    /* invert mask to get untagged ports */
    NIM_INTF_MASKINV(tempMask);

     *size = NIM_INTF_INDICES;
    (void)dot1qReverseMask(tempMask, mask);

    (void)osapiReadLockGive(dot1qCfgRWLock);

    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Set the set of ports which are transmitting untagged frames
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticUntaggedPortsSet(L7_uint32 vlanId, NIM_INTF_MASK_t * mask)
{

    L7_RC_t rc, rc_iter,rv_iter = L7_SUCCESS;
    DOT1Q_MSG_t msg;
    NIM_INTF_MASK_t tempMask ;
    L7_uint32 vid, intIfNum, mode;
    dot1qVlanCfgData_t *vCfg;
    dot1qIntfCfgData_t *pCfg = L7_NULL;



    rc  = L7_FAILURE;
    vid = vlanId;

    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
    {
        return L7_FAILURE;
    }

    if (mask == L7_NULL)
    {
       return L7_FAILURE;
    }


    /* Check that all ports in the mask can participate in dot1q */

    (void)dot1qReverseMask(*mask, &tempMask);

    NIM_INTF_MASKINV(tempMask);


    if (dot1qIntfIsValidMask( (L7_INTF_MASK_t *)&tempMask) != L7_TRUE)
    {
        return L7_FAILURE;
    }

    /* Update the config structure */
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    vCfg = L7_NULL;
    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }

    /* Check if any of the ports are configured as access port */
    intIfNum = L7_NULL;
    rc_iter = nimFirstValidIntfNumber(&intIfNum);
    while (rc_iter == L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      if (NIM_INTF_ISMASKBITSET(tempMask, intIfNum) !=
          NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.taggedSet, intIfNum)
         )
      {
         if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
         {
           mode = pCfg->portCfg.switchport_mode;
           if (mode == DOT1Q_SWPORT_MODE_ACCESS || mode == DOT1Q_SWPORT_MODE_TRUNK)
           {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                      "Invalid Port List. Cannot configure VLAN tagging for an access or trunk port %s", ifName);
              rv_iter = L7_FAILURE;
              break;
           }
           else if (!NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.staticEgressPorts,intIfNum))
           {
             NIM_INTF_CLRMASKBIT(tempMask, intIfNum);
           }
         }
      }/* End of membership change check */
      rc_iter = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }/* End of interface iterations */

    if (rv_iter == L7_FAILURE)
    {
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_FAILURE;
    }
    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock); 

    /* Put a message on the queue */
    memset(&msg, 0x0, sizeof(DOT1Q_MSG_t));

    msg.vlanId = vlanId;
    msg.event = dot1q_vlan_tagged_set_static_mask;
    msg.mode = L7_DOT1Q_UNTAGGED;
    memcpy(&msg.data.intfMask, &tempMask, sizeof(NIM_INTF_MASK_t));


    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;

}

/*********************************************************************
* @purpose  Convert a dynamically learned VLAN into an
*           administratively configured one.
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qDynamicToStaticVlanConvert(L7_uint32 vid)
{
    L7_RC_t rc;
    DOT1Q_MSG_t msg;
    dot1qVlanCfgData_t *vCfg;
    L7_BOOL cfgEntryFound;

    rc = L7_FAILURE;

    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID) ||
        (vid == L7_DOT1Q_DEFAULT_VLAN))
    {
        return L7_FAILURE;
    }

    /* Check for pre-existence of the static VLAN */
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    if (dot1qVlanIsConfigurable(vid, &vCfg) == L7_TRUE)
    {
        /* Static vlan already exists */
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_SUCCESS;
    }


    /* No entry previously existed. Attempt to allocate an entry */
    cfgEntryFound = L7_FALSE;
    if(dot1qVlanConfigEntryGet(vid, &vCfg) == L7_TRUE)
    {
        cfgEntryFound = L7_TRUE;
    }

    if (cfgEntryFound == L7_TRUE)
    {
        /* Simply reserve the config structure.
           The rest of the cfg data structure will be updated
           by the task */


        /* Update cfg */
        memset((void *)vCfg, 0, sizeof(dot1qVlanCfgData_t));
        vCfg->vlanId = vid;

        /* Set dataChanged flag */
        dot1qCfg->hdr.dataChanged = L7_TRUE;

    }

    (void)osapiWriteLockGive(dot1qCfgRWLock);

     /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    msg.vlanId = vid;
    msg.event = dot1q_vlan_dynamic_to_static_set;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;
}

/*********************************************************************
* @purpose  Check to see if a VLAN exists based on a
*           VLAN ID.
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCheckValid(L7_uint32 vid)
{
  dot1q_vlan_dataEntry_t dataEntry;
  L7_RC_t                status;

  if (dot1qInfo->initialized != L7_TRUE)
  {
    return L7_FAILURE;
  }


  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    status = L7_NOT_EXISTS;
  }
  else if (dataEntry.vlanId == vid)
  {
    status = L7_SUCCESS;
  }
  else
  {
    status = L7_FAILURE;
  }

  if (status == L7_FAILURE)
  {
      /* Check to see if VLAN create is outstanding */
      if (dot1qVlanCreateIsPending(vid) == L7_TRUE)
      {
          status = L7_SUCCESS;
      }
  }

  return status;
}


/*********************************************************************
* @purpose  Synchronize with the dot1q Queue.
*
* @param    @b{(input)} None
*
* @returns  L7_SUCCESS  if the vlan Id is found and has been created 
* @returns  L7_FAILURE  if the vlan Id is not found
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1qApiQueueSynchronize(L7_uint32 vid)
{
    L7_RC_t rc;
    DOT1Q_MSG_t msg;


     /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    msg.vlanId = vid;
    msg.event =   dot1q_queueSynchronize;

    rc = dot1qIssueCmd(&msg);

    if (rc == L7_SUCCESS)
        dot1qQueueSyncSemGet();

    return rc;


}

/*********************************************************************
* @purpose  Check to see if a VLAN exists after dot1q Queue synchronization
*           based on a VLAN ID.
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanQueueSyncCheckValid(L7_uint32 vid)
{
  L7_RC_t rc=L7_FAILURE;
  L7_RC_t status = L7_FAILURE;

  rc=dot1qVlanCheckValid(vid);
  if (rc!=L7_SUCCESS)
  {
     if (rc==L7_NOT_EXISTS)
     {
        /* possible reasons for failure:
           1. vlan does not exist
          2. vlan create commnad has been issued but not yet processed */
        /* for the second case, synchronise with the dot1q msg queue and then validate vlan id
           again */

          dot1qApiQueueSynchronize(vid);

          rc = dot1qVlanCheckValid(vid);
          if (rc == L7_NOT_EXISTS)
          {
            status = L7_NOT_EXISTS;
          }
          else if (rc== L7_FAILURE)
          {
            status = L7_FAILURE;
          }
          else
            status = L7_SUCCESS;
     }
     else if (rc==L7_FAILURE)
     {
       status = L7_FAILURE;
     }
  }
  else
    status =  L7_SUCCESS;

  return status;


}

/*********************************************************************
* @purpose  Determine whether an interface is a member of a given VLAN.
*
* @param    vid         vlan ID
* @param    intIfNum    interface in question
*
* @returns  L7_TRUE if the interface's participation in the VLAN is
*           L7_DOT1Q_FIXED.
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsVlanMember(L7_uint32 vid, L7_uint32 intIfNum)
{
  L7_uint32 mode;

  if (dot1qOperVlanMemberGet(vid, intIfNum, &mode) != L7_SUCCESS)
    return L7_FALSE;

  return (mode == L7_DOT1Q_FIXED);
}


/*********************************************************************
* @purpose  Get the operational VLAN port membership
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be add
* @param    *mode       pointer to mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN })
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanMemberGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *mode)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  if (dot1qIntfIsValid(intIfNum) != L7_TRUE) 
  { 
     return L7_FAILURE;
  }

  /* If this interface is acquired by lag, return as excluded for operational value */
  /* We need to fix this by responding to ACQ and Release nim events in dot1qIntfChangeProcess
   * and updating the currentEgressPort mask in the vlan database but for now make the check here
   * so that no UI has to specifically check for lag membership
   */
  if (COMPONENT_ACQ_ISMASKBITSET(dot1qQports[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID))
  {
      *mode = L7_DOT1Q_FORBIDDEN;
      return L7_SUCCESS;
  }

  /* not really fixed/forbidden but include/exclude */
  if (NIM_INTF_ISMASKBITSET(dataEntry.current.currentEgressPorts, intIfNum))
    *mode = L7_DOT1Q_FIXED;
  else
    *mode = L7_DOT1Q_FORBIDDEN;

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Get the operational tagging mode for a port in a VLAN
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
* @param    *tagMode     Pointer to tagging mode
*                       (@b{  DOT1Q_UNTAGGED or
*                             DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Use dot1q_set_vlanCfg() instead of this routine when possible
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanTaggedMemberGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *tagMode)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  if (dot1qIntfIsValid(intIfNum) != L7_TRUE) 
  { 
     return L7_FAILURE;
  }

  if (NIM_INTF_ISMASKBITSET(dataEntry.current.taggedSet, intIfNum))
    *tagMode = L7_DOT1Q_TAGGED;
  else
    *tagMode = L7_DOT1Q_UNTAGGED;


  return(L7_SUCCESS);
}


#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
/*********************************************************************
* @purpose  Get the operational group filtering mode  for a port in a VLAN
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
* @param    *filterMode  pointer to group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Use dot1q_set_vlanCfg() instead of this routine when possible
*
* @end
*********************************************************************/
L7_RC_t dot1qOperVlanGroupFilterGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *filterType)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  if (NIM_INTF_ISMASKBITSET(dataEntry.admin->groupFilter.forwardAllMask, intIfNum))
    *filterType = L7_DOT1Q_FORWARD_ALL;
  else if (NIM_INTF_ISMASKBITSET(dataEntry.admin->groupFilter.forwardUnregMask, intIfNum))
    *filterType = L7_DOT1Q_FORWARD_UNREGISTERED;
  else
    *filterType = L7_DOT1Q_FILTER_UNREGISTERED;

  return(L7_SUCCESS);
}

#endif

/*********************************************************************
* @purpose  Get the VLAN type
*
* @param    vid         vlan ID
* @param    *type       pointer to mode of participation
*                       L7_DOT1Q_DEFAULT,
*                       L7_DOT1Q_ADMIN
*                       L7_DOT1Q_DYNAMIC
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    This information is LVL7 status only.
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanTypeGet(L7_uint32 vid,  L7_uint32 *type)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  *type = dataEntry.current.status;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get the VLAN creation time
*
* @param    vid         vlan ID
* @param    *time       pointer to L7_time_t structure
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanCreationTimeGet(L7_uint32 vid, L7_timespec *time)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  *time = dataEntry . current.creationTime;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get next vlan
*
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid
*                       (@b{  1-4094,   or
*                            DOT1Q_ADMIN_VLAN})
*
* @returns  L7_FAILURE  no more
* @returns  L7_SUCCESS  next found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  dot1qNextVlanGet(L7_uint32 vid, L7_uint32 *nextvid)
{
  dot1q_vlan_dataEntry_t *pDataEntry = (dot1q_vlan_dataEntry_t *)L7_NULL;
  L7_RC_t returnValue = L7_FAILURE;

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
  if (vid > 0)
  {
      pDataEntry = vlanNextDataEntry(pVlanTree, vid);
  }
  else if (vid == 0) /* bad first vid */
  {
     pDataEntry = vlanFirstDataEntry(pVlanTree);
  }

  if (pDataEntry != L7_NULL)
  {
      *nextvid    = pDataEntry->vlanId;
      returnValue = L7_SUCCESS;
  }

  osapiSemaGive(dot1qSem);

  return returnValue;
}

/*********************************************************************
* @purpose  Get the next static vlan in the tree
*
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid
*
* @returns  L7_FAILURE  reached the last static vlan or bad first vid
* @returns  L7_SUCCESS  next found
*
* @notes    If invoked with an invalid vid such as vid=0,
*
* @end
*********************************************************************/
L7_RC_t dot1qNextStaticVlanGet(L7_uint32 vid, L7_uint32 *nextvid)
{
  dot1q_vlan_dataEntry_t *pDataEntry;

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
  if (vid == 0) /* bad first vid */
  {
    pDataEntry = vlanFirstDataEntry(pVlanTree);

    if (pDataEntry == NULL) /* panic no vlans */
    {
       osapiSemaGive(dot1qSem);
       return L7_FAILURE;
    }

    *nextvid = pDataEntry->vlanId;
    osapiSemaGive(dot1qSem);
    return L7_SUCCESS;
  }

  /* otherwise look for the next static vlan */
  do
  {
    pDataEntry = vlanNextDataEntry(pVlanTree, vid);

    if (pDataEntry == NULL)
    {
       osapiSemaGive(dot1qSem);
       return L7_FAILURE; /* end of tree */
    }

    if (pDataEntry ->current.status == L7_DOT1Q_TYPE_GVRP_REGISTERED ||
        pDataEntry->current.status == L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED ||
        pDataEntry->current.status == L7_DOT1Q_TYPE_DOT1X_REGISTERED||
        pDataEntry->current.status == L7_DOT1Q_TYPE_IP_INTERNAL_VLAN)
      vid = pDataEntry ->vlanId;

  } while (pDataEntry->current.status == L7_DOT1Q_TYPE_GVRP_REGISTERED ||
        pDataEntry->current.status == L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED ||
        pDataEntry->current.status == L7_DOT1Q_TYPE_DOT1X_REGISTERED||
        pDataEntry->current.status == L7_DOT1Q_TYPE_IP_INTERNAL_VLAN);

  /* next static vlan in tree found */
  *nextvid = pDataEntry -> vlanId;

  osapiSemaGive(dot1qSem);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next static vlan in the tree
*
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid
*
* @returns  L7_FAILURE  reached the last static vlan or bad first vid
* @returns  L7_SUCCESS  next found
*
* @notes    If invoked with an invalid vid such as vid=0,
*
* @end
*********************************************************************/
L7_RC_t dot1qNextDynamicVlanGet(L7_uint32 vid, L7_uint32 *nextvid)
{
  dot1q_vlan_dataEntry_t *pDataEntry;

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
  if (vid == 0) /* bad first vid */
  {
    pDataEntry = vlanFirstDataEntry(pVlanTree);

    if (pDataEntry == NULL) /* panic no vlans */
    {
      osapiSemaGive(dot1qSem);
      return L7_FAILURE;
    }

    *nextvid = pDataEntry->vlanId;
    /* first one is static, continue on */
  }

  /* otherwise look for the next static vlan */
  do
  {
    pDataEntry = vlanNextDataEntry(pVlanTree, vid);

    if (pDataEntry == NULL)
    {
      osapiSemaGive(dot1qSem);
      return L7_FAILURE; /* end of tree */
    }

    if (pDataEntry ->current.status != L7_DOT1Q_TYPE_GVRP_REGISTERED &&
        pDataEntry->current.status != L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED &&
        pDataEntry->current.status != L7_DOT1Q_TYPE_DOT1X_REGISTERED &&
        pDataEntry->current.status != L7_DOT1Q_TYPE_IP_INTERNAL_VLAN)
      vid = pDataEntry ->vlanId;

  } while (pDataEntry->current.status != L7_DOT1Q_TYPE_GVRP_REGISTERED &&
        pDataEntry->current.status != L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED &&
        pDataEntry->current.status != L7_DOT1Q_TYPE_DOT1X_REGISTERED &&
        pDataEntry->current.status != L7_DOT1Q_TYPE_IP_INTERNAL_VLAN);

  /* next static vlan in tree found */
  *nextvid = pDataEntry -> vlanId;

  osapiSemaGive(dot1qSem);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset Vlan configuration to default parameters
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCfgClear(void)
{
    DOT1Q_MSG_t msg;
    L7_RC_t rc;

    /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    msg.event = dot1q_clear;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;


}

/*********************************************************************
* @purpose  Set the VID configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    vid         port vlan ID
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
* @param    requestor   DOT1Q_ADMIN or DOT1Q_DOT1X etc.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist(configuration is applied)
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVIDSet(L7_uint32 intIfNum, L7_uint32 vid, L7_uint32 vid_type,
                            L7_uint32 requestor)
{

    L7_uint32   i,start,end;
    L7_RC_t     rc;
    DOT1Q_MSG_t msg;
    dot1qIntfCfgData_t *pCfg;
    L7_BOOL dataChanged = L7_FALSE;
    dot1qVlanCfgData_t *vCfg;

    rc= L7_FAILURE;

    /* check for valid parameters */
     /* range check*/

    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "VLAN %d not in range",vid);

        return rc;
    }

    if( (dot1qStaticVlanCheckValid(vid) == L7_SUCCESS) || (requestor != DOT1Q_DOT1X))
    {
    	if (dot1qVlanIsConfigurable(vid, &vCfg) != L7_TRUE)
    	{
      	  /* A static VLAN does not exist */
      	  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
          "Cannot set pvid, Vlan(%d) not present ", vid);
      	  return L7_NOT_EXISTS;
    	}
     }	

    /* Since L7_ALL_INTERFACES is valid input, a test for
       dot1qIntfIsValid() would fail. A subsequent
       dot1qMapIntfIsConfigurable check is sufficient. */
    if (intIfNum == L7_ALL_INTERFACES)
    {
      start = 1;
      end = intIfNum;
    }
    else
    {
      start = intIfNum;
      end = intIfNum+1;
    }
    /* Update the config structure */
    if (requestor == DOT1Q_ADMIN)
    {
      (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

      for (i=start;i<end;i++)
      {
          if (dot1qMapIntfIsConfigurable(i, &pCfg) == L7_TRUE)
          {
              pCfg->portCfg.VID[vid_type] = vid;
              dataChanged = L7_TRUE;
          }
      }
      if (dataChanged == L7_TRUE)
          dot1qCfg->hdr.dataChanged = L7_TRUE;

      (void)osapiWriteLockGive(dot1qCfgRWLock);
    }
    else if (requestor == DOT1Q_DOT1X)
    {
      dataChanged = L7_TRUE;
    }


    if (dataChanged == L7_TRUE)
    {
      /* Put a message on the queue */
      memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
      msg.vlanId = vid;
      msg.event = dot1q_qports_vid_set;
      msg.mode  = vid_type;
      msg.data.intIfNum = intIfNum;
      msg.requestor = requestor;


      rc = dot1qIssueCmd(&msg);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot put a message on dot1q msg Queue, Returns:%d ",rc);
      }

    }
    return rc;
}

/*********************************************************************
* @purpose  Get the PVID configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
* @param    pvid        the port VLAN ID associated with this
*                       interface and type
*
* @returns  L7_SUCCESS, if success
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsVIDGet(L7_uint32 intIfNum,  L7_uint32 vid_type, L7_uint32 *pvid)
{
    dot1qIntfCfgData_t *pCfg;
    L7_RC_t             rc;

    rc = L7_SUCCESS;

    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
        *pvid = pCfg->portCfg.VID[vid_type];
    }
    else
        rc = L7_FAILURE;

    (void)osapiReadLockGive(dot1qCfgRWLock);
    return rc;
}


/*********************************************************************
* @purpose  Get the current PVID configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
* @param    pvid        the port VLAN ID associated with this
*                       interface and type
*
* @returns  L7_SUCCESS, if success
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsCurrentVIDGet(L7_uint32 intIfNum,  L7_uint32 vid_type, L7_uint32 *pvid)
{
  dot1qIntfCfgData_t *pCfg;
  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *pvid = dot1qQports[intIfNum].pvid[vid_type];
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Set the Acceptable Frame Type configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    type        L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsAcceptFrameTypeSet(L7_uint32 intIfNum, L7_uint32 type)
{
    L7_uint32   i,start,end;
    L7_RC_t     rc= L7_FAILURE;
    DOT1Q_MSG_t msg;
    dot1qIntfCfgData_t *pCfg;
    L7_BOOL dataChanged = L7_FALSE;

    /* check for valid parameters */

    /* range check*/

    if ((type != L7_DOT1Q_ADMIT_ALL) && (type != L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED) &&
        (type != L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED))
    {

      return L7_FAILURE;
    }

    /* Since L7_ALL_INTERFACES is valid input, a test for
       dot1qIntfIsValid() would fail. A subsequent
       dot1qMapIntfIsConfigurable check is sufficient. */
    if (intIfNum == L7_ALL_INTERFACES)
    {
      start = 1;
      end = intIfNum;
    }
    else
    {
      start = intIfNum;
      end = intIfNum+1;
    }

    /* Update the config structure */
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    for (i=start;i<end;i++)
    {
        if (dot1qMapIntfIsConfigurable(i, &pCfg) == L7_TRUE)
        {
          if (pCfg->portCfg.acceptFrameType != type) 
          {
            pCfg->portCfg.acceptFrameType = type;
            dataChanged = L7_TRUE;
          }
        }
    }

    if (dataChanged == L7_TRUE)
        dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);



    if (dataChanged == L7_TRUE)
    {
      /* Put a message on the queue */
      memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

      msg.event = dot1q_qports_accept_frame_type_set;
      msg.mode = type;
      msg.data.intIfNum = intIfNum;

      rc = dot1qIssueCmd(&msg);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
      }
    }
    else
    {
      rc = L7_SUCCESS;
    }

    return rc;
}

/*********************************************************************
* @purpose  Get the Acceptable Frame Type configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
* @param    type        L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsAcceptFrameTypeGet(L7_uint32 intIfNum, L7_uint32 *type)
{

  dot1qIntfCfgData_t *pCfg;
  L7_RC_t            rc;

  rc = L7_SUCCESS;

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
      *type = pCfg->portCfg.acceptFrameType;
      /**mode = dot1qQports[intIfNum].a*/
  }
  else
      rc = L7_FAILURE;

  (void)osapiReadLockGive(dot1qCfgRWLock);
  return rc;
}

/*********************************************************************
*
* @purpose  Set the Ingress Filtering configuration for a port
*
* @param    intIfNum    physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all eligible interfaces
* @param    status      L7_ENABLE  or L7_DISABLE
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsIngressFilterSet(L7_uint32 intIfNum, L7_uint32 status,L7_uint32 requestor)
{
    L7_uint32   i,start,end;
    L7_RC_t     rc;
    DOT1Q_MSG_t msg;
    dot1qIntfCfgData_t *pCfg;
    L7_BOOL dataChanged = L7_FALSE;


    rc= L7_FAILURE;

    /* Validity check */


    /* range check*/

    if ((status != L7_ENABLE) && (status != L7_DISABLE))
    {
      return L7_FAILURE;
    }


    /* Since L7_ALL_INTERFACES is valid input, a test for
       dot1qIntfIsValid() would fail. A subsequent
       dot1qMapIntfIsConfigurable check is sufficient. */
    if (intIfNum == L7_ALL_INTERFACES)
    {
      start = 1;
      end = intIfNum;
    }
    else
    {
      start = intIfNum;
      end = intIfNum+1;
    }

    if(requestor == DOT1Q_ADMIN)
    {
      /* Update the config structure */
      (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
      for (i=start;i<end;i++)
      {


          if (dot1qMapIntfIsConfigurable(i, &pCfg) == L7_TRUE)
          {
            if (pCfg->portCfg.enableIngressFiltering != status)
            {
              pCfg->portCfg.enableIngressFiltering = status;
              dataChanged = L7_TRUE;
            }
          }
      }

      if(dataChanged == L7_TRUE)
          dot1qCfg->hdr.dataChanged = L7_TRUE;

      (void)osapiWriteLockGive(dot1qCfgRWLock);
    }

    if(dataChanged == L7_TRUE)
    {
      /* Put a message on the queue */
      memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

      msg.event = dot1q_qports_ingress_filter_set;
      msg.data.intIfNum = intIfNum;
      msg.mode = status;

      rc = dot1qIssueCmd(&msg);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
      }

    }
    else
    {
      rc = L7_SUCCESS;
    }

    return rc;
}

/*********************************************************************
* @purpose  Get the Ingress Filtering configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
* @param    mode        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsIngressFilterGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  dot1qIntfCfgData_t *pCfg;
  L7_RC_t            rc;

  rc = L7_SUCCESS;

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
      *mode = pCfg->portCfg.enableIngressFiltering;
  }
  else
      rc = L7_FAILURE;


  (void)osapiReadLockGive(dot1qCfgRWLock);

  return rc;
}


/*********************************************************************
* @purpose  Get the Current Ingress Filtering configuration for a port
*
* @param    intIfNum    physical or logical interface to be configured
* @param    mode        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsCurrentIngressFilterGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  dot1qIntfCfgData_t *pCfg;
  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *mode = dot1qQports[intIfNum].ingressFilter;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get device capabilities
*
* @param    none
*
* @returns  deviceCapabilities   bitmask mapping of functionality
*
* @notes
*        dot1dExtendedFilteringServices(0),
*                              -- can perform filtering of
*                              -- individual multicast addresses
*                              -- controlled by GMRP.
*        dot1dTrafficClasses(1),
*                              -- can map user priority to
*                              -- multiple traffic classes.
*        dot1qStaticEntryIndividualPort(2),
*                              -- dot1qStaticUnicastReceivePort &
*                              -- dot1qStaticMulticastReceivePort
*                              -- can represent non-zero entries.
*        dot1qIVLCapable(3),   -- Independent VLAN Learning.
*        dot1qSVLCapable(4),   -- Shared VLAN Learning.
*        dot1qHybridCapable(5),
*                              -- both IVL & SVL simultaneously.
*        dot1qConfigurablePvidTagging(6),
*                              -- whether the implementation
*                              -- supports the ability to
*                              -- override the default PVID
*                              -- setting and its egress status
*                              -- (VLAN-Tagged or Untagged) on
*                              -- each port.
*        dot1dLocalVlanCapable(7)
*                              -- can support multiple local
*                              -- bridges, outside of the scope
*                              -- of 802.1Q defined VLANs.
*
*
* @end
*********************************************************************/
L7_uchar8 dot1dDeviceCapabilitiesGet()
{
  return(L7_uchar8)(dot1qInfo->deviceCapabilities);
}

/*********************************************************************
* @purpose  Get traffic class capability
*
* @param    *status    Ptr to status value output location (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If successful, the *status parameter indicates L7_TRUE if
*           more than one priority level is enabled, or L7_FALSE if a
*           single priority level is enabled.
*
* @end
*********************************************************************/
L7_RC_t dot1dTrafficClassesEnabledGet(L7_BOOL *status)
{
  if (status == L7_NULLPTR)
    return L7_FAILURE;

  *status = (L7_BOOL)dot1qInfo->TrafficClassesEnabled;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the traffic class capability
*
* @param    status    status value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A status of L7_TRUE indicates more than one priority level
*           is enabled, while L7_FALSE indicates a single priority level
*           is enabled.
*
* @end
*********************************************************************/
L7_RC_t dot1dTrafficClassesEnabledSet(L7_BOOL status)
{
    L7_RC_t rc;
    DOT1Q_MSG_t msg;


    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.mode = status;
    msg.event = dot1q_qports_traffic_classes_enabled_set;


    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


  return rc;
}

/*********************************************************************
* @purpose  Get dot1d port capabilities
*
* @param    intIfNum    internal interface number
*
* @returns  portCapabilities   bitmask mapping of functionality
*
* @notes
* SYNTAX      BITS {
*        dot1qDot1qTagging(0), -- supports 802.1Q VLAN tagging of
*                              -- frames and GVRP.
*        dot1qConfigurableAcceptableFrameTypes(1),
*                              -- allows modified values of
*                              -- dot1qPortAcceptableFrameTypes.
*        dot1qIngressFiltering(2)
*                              -- supports the discarding of any
*                              -- frame received on a Port whose
*                              -- VLAN classification does not
*                              -- include that Port in its Member
*                              -- set.
*    }
*
*
* @end
*********************************************************************/
L7_uchar8 dot1dPortCapabilitiesGet(L7_uint32 intIfNum)
{
  return(L7_uchar8)(dot1qQports[intIfNum].portCapabilities);
}

/*********************************************************************
* @purpose  Get dot1d port default user priority
*
* @param    intIfNum    internal interface number
*
* @returns  priority    default user priority
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_uint32 dot1dPortDefaultUserPriorityGet(L7_uint32 intIfNum)
{
  dot1qIntfCfgData_t *pCfg;
  L7_uint32     priority;

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  priority = 0;

  if (intIfNum == L7_ALL_INTERFACES)
  {
      priority = dot1qCfg->Qglobal.priority.DefaultUserPriority;
  }
  else
  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
      priority = pCfg->portCfg.priority.DefaultUserPriority;
  }

  (void)osapiReadLockGive(dot1qCfgRWLock);

  return priority;
}


/*********************************************************************
* @purpose  Set dot1d port default user priority
*
* @param    intIfNum   internal interface number
* @param   priority    default user priority, in range of 0-7
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultUserPrioritySet(L7_uint32 intIfNum, L7_uint32 priority)
{

    L7_RC_t     rc;
    DOT1Q_MSG_t msg;
    dot1qIntfCfgData_t *pCfg;


    rc= L7_FAILURE;

    /* check for valid parameters */
    if (dot1qIntfIsValid(intIfNum) == L7_FALSE)
    {
      return L7_FAILURE;
    }

    /* range check*/

    if ((priority < 0) || (priority > 7))
    {

      return L7_FAILURE;
    }


    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
        pCfg->portCfg.priority.DefaultUserPriority = priority;
        dot1qCfg->hdr.dataChanged = L7_TRUE;
    }
    else
    {
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_FAILURE;
    }

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.mode = priority;
    msg.data.intIfNum = intIfNum;
    msg.event = dot1q_qports_default_user_prio_set;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;
}

/*********************************************************************
* @purpose  Set dot1d port default user priority globally for all interfaces
*
* @param    priority    default user priority, in range of 0-7
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultUserPriorityGlobalSet(L7_uint32 priority)
{
    L7_RC_t rc;
    DOT1Q_MSG_t msg;
    L7_uint32 i;


    /* range check*/

    if ((priority < 0) || (priority > 7))
    {
      return L7_FAILURE;
    }


    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    /* a global config change affects all interfaces, so update their config
     * value here under the current write lock
     *
     * there is no need to send a separate msg for each interface,
     * since the global config msg processing will cause the config
     * to be applied to each settable interface
     */
    for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
    {
      /* Updating all interface config data structures here, even those without
       * a config ID assigned.  The interface creation process will also pull this
       * value from the latest global config when it builds initial defaults.
       */
      dot1qCfg->Qports[i].portCfg.priority.DefaultUserPriority = priority;
    }

    dot1qCfg->Qglobal.priority.DefaultUserPriority = priority;
    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.mode = priority;
    msg.data.intIfNum = L7_ALL_INTERFACES;
    msg.event = dot1q_qports_default_user_prio_global_set;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;
}

/*********************************************************************
* @purpose  Get dot1d number of traffic classes
*
* @param    intIfNum   internal interface number
*
* @returns  Number of traffic classes supported, in range of 1-8
*           optionally readonly
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_uint32 dot1dPortNumTrafficClassesGet(L7_uint32 intIfNum)
{

  dot1qIntfCfgData_t *pCfg;
  L7_uint32     numClasses;

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  numClasses = 0;

  if (intIfNum == L7_ALL_INTERFACES)
  {
      numClasses = dot1qCfg->Qglobal.priority.NumTrafficClasses;
  }
  else
  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
      numClasses = pCfg->portCfg.priority.NumTrafficClasses;
  }

  (void)osapiReadLockGive(dot1qCfgRWLock);

  return numClasses;
}

/*********************************************************************
* @purpose  Set dot1d number of traffic classes
*
* @param    intIfNum            internal interface number
* @param    numTrafficClasses   number of traffic classes supported, in range of 1-8
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassesSet(L7_uint32 intIfNum, L7_uint32 numTrafficClasses)
{

    L7_RC_t     rc;
    DOT1Q_MSG_t msg;
    dot1qIntfCfgData_t *pCfg;


    rc= L7_FAILURE;

    /* check for valid parameters */
    if (dot1qIntfIsValid(intIfNum) == L7_FALSE)
    {
      return L7_FAILURE;
    }

    /* range check*/

    if ((numTrafficClasses > L7_DOT1P_NUM_TRAFFIC_CLASSES) ||
        (numTrafficClasses < L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT))
    {
        return L7_FAILURE;
    }


    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
        pCfg->portCfg.priority.NumTrafficClasses = numTrafficClasses;
        dot1qCfg->hdr.dataChanged = L7_TRUE;
    }
    else
    {
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_FAILURE;
    }

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.mode = numTrafficClasses;
    msg.data.intIfNum = intIfNum;
    msg.event = dot1q_qports_num_traffic_classes_set;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;

}



/*********************************************************************
* @purpose  Set dot1d number of traffic classes globally for all interfaces
*
* @param    numTrafficClasses   number of traffic classes supported, in range of 1-8
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassesGlobalSet(L7_uint32 numTrafficClasses)
{

    L7_RC_t rc;
    DOT1Q_MSG_t msg;
    L7_uint32 i;


    /* range check*/

    if ((numTrafficClasses > L7_DOT1P_NUM_TRAFFIC_CLASSES) ||
        (numTrafficClasses < L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT))
    {
        return L7_FAILURE;
    }


    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    /* a global config change affects all interfaces, so update their config
     * value here under the current write lock
     *
     * there is no need to send a separate msg for each interface,
     * since the global config msg processing will cause the config
     * to be applied to each settable interface
     */
    for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
    {
      /* Updating all interface config data structures here, even those without
       * a config ID assigned.  The interface creation process will also pull this
       * value from the latest global config when it builds initial defaults.
       */
      dot1qCfg->Qports[i].portCfg.priority.NumTrafficClasses = numTrafficClasses;
    }

    dot1qCfg->Qglobal.priority.NumTrafficClasses = numTrafficClasses;
    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.mode = numTrafficClasses;
    msg.event = dot1q_qports_num_traffic_classes_global_set;
    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;
}

/*********************************************************************
* @purpose  Get dot1d traffic class priority
*
* @param    intIfNum            internal interface number
* @param    traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
* @param    *priority           number of priorities associated with the traffic class
*
* @returns  L7_SUCCESS        priority count value is valid
* @returns  L7_FAILURE        traffic class value is out of range
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassPriorityGet(L7_uint32 intIfNum, L7_uint32 trafficClass,
                                            L7_uint32 *priorityCount)
{
    dot1qIntfCfgData_t *pCfg;
    L7_uint32 count;
    dot1p_prio_t *pPriority = L7_NULL;
    L7_RC_t     rc = L7_SUCCESS;

    if ((trafficClass > L7_DOT1P_MAX_TRAFFIC_CLASS) ||
        (trafficClass < 0))
    {
      return L7_FAILURE;
    }


    if (priorityCount == L7_NULLPTR)
      return L7_FAILURE;




    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    if (intIfNum == L7_ALL_INTERFACES)
    {
        pPriority = &dot1qCfg->Qglobal.priority;
    }
    else if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
        pPriority = &pCfg->portCfg.priority;
    }
    else
        rc = L7_FAILURE;


    if (rc == L7_SUCCESS)
    {
        for (count = 0; count <= L7_DOT1P_MAX_PRIORITY; count++)
        {
          if (pPriority->Mapping[count] == trafficClass)
          {
            *priorityCount = *priorityCount + 1;
          }
        }
    }

    (void)osapiReadLockGive(dot1qCfgRWLock);

    return rc;

}


/*********************************************************************
* @purpose  Get dot1d traffic class
*
* @param    intIfNum            internal interface number
* @param    priority            traffic class priority
* @param    *traffic class      In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassGet(L7_uint32 intIfNum, L7_uint32 priority,
                                 L7_uint32 *trafficClass)
{
    dot1qIntfCfgData_t *pCfg;
    L7_RC_t             rc;


    rc = L7_SUCCESS;

     /* Check input parameters */
    if (priority > L7_DOT1P_MAX_PRIORITY)
      return L7_FAILURE;

    if (trafficClass == L7_NULLPTR)
      return L7_FAILURE;

    /* Read config */

    (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    if (intIfNum == L7_ALL_INTERFACES)
    {
        *trafficClass = dot1qCfg->Qglobal.priority.Mapping[priority];
    }
    else if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      *trafficClass = pCfg->portCfg.priority.Mapping[priority];
    }
    else
    {
        rc = L7_FAILURE;
    }

    (void)osapiReadLockGive(dot1qCfgRWLock);

    return rc;


}

/*********************************************************************
* @purpose  Set dot1d traffic class
*
* @param    intIfNum   internal interface number
* @param    priority   traffic class priority
* @param    traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassSet(L7_uint32 intIfNum, L7_uint32 priority,
                                 L7_uint32 trafficClass)
{


    L7_RC_t     rc;
    DOT1Q_MSG_t msg;
    dot1qIntfCfgData_t *pCfg;


    DOT1Q_DEBUG_MSG("dot1dPortTrafficClassSet : intIfNum %d priority %d trafficClass %d \n",
                    intIfNum, priority, trafficClass);



    rc= L7_FAILURE;

    /* check for valid parameters */
    if (dot1qIntfIsValid(intIfNum) == L7_FALSE)
    {
      return L7_FAILURE;
    }


    /* range check*/
    if (priority > L7_DOT1P_MAX_PRIORITY)
    {
      return L7_FAILURE;
    }

    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
        pCfg->portCfg.priority.Mapping[priority] = trafficClass;
        dot1qCfg->hdr.dataChanged = L7_TRUE;

    }
    else
    {
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_FAILURE;
    }

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.data.prio.intIfNum = intIfNum;
    msg.data.prio.trafficClass = trafficClass;
    msg.data.prio.priority = priority;
    msg.event = dot1q_qports_traffic_class_set;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;

}

/*********************************************************************
* @purpose  Set dot1d traffic class globally for all interfaces
*
* @param    priority      traffic class priority
* @param    traffic class In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassGlobalSet(L7_uint32 priority, L7_uint32 trafficClass)
{

    L7_RC_t rc = L7_FAILURE;
    DOT1Q_MSG_t msg;
    L7_uint32 numTrafficClasses;
    L7_uint32 i;


    /* range check */
    if (priority > L7_DOT1P_MAX_PRIORITY)
    {

      return rc;
    }


    numTrafficClasses = dot1dPortNumTrafficClassesGet(L7_ALL_INTERFACES);
    if (trafficClass > (numTrafficClasses - 1))
    {

      return rc;
    }

    /* Update configuration */
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    /* a global config change affects all interfaces, so update their config
     * value here under the current write lock
     *
     * there is no need to send a separate msg for each interface,
     * since the global config msg processing will cause the config
     * to be applied to each settable interface
     */
    for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
    {
      /* Updating all interface config data structures here, even those without
       * a config ID assigned.  The interface creation process will also pull this
       * value from the latest global config when it builds initial defaults.
       */
      dot1qCfg->Qports[i].portCfg.priority.Mapping[priority] = trafficClass;
    }

    dot1qCfg->Qglobal.priority.Mapping[priority] = trafficClass;
    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset(&msg,0x0,sizeof(DOT1Q_MSG_t));
    msg.data.prio.priority = priority;
    msg.data.prio.trafficClass = trafficClass;
    msg.event = dot1q_qports_traffic_class_global_set;


    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;


}


/*=========================================================================*/
/*=========================================================================*/
/*=========================================================================*/
/*======================  Status Parameters ===============================*/
/*=========================================================================*/
/*=========================================================================*/
/*=========================================================================*/

/*********************************************************************
* @purpose  Get factory default dot1d traffic class
*
* @param    intIfNum   internal interface number
* @param    priority   traffic class priority
* @param    *traffic class  In range of 0-(L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultTrafficClassGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *trafficClass)
{
  if (priority > L7_DOT1P_MAX_PRIORITY)
    return L7_FAILURE;

  if (trafficClass == L7_NULLPTR)
    return L7_FAILURE;

  /* NOTE:  The same defaults are used for global and per-intf config */
  *trafficClass = dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1][priority];
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get dot1q Vlan Version Number
*
* @param    none
*
* @returns  IEEE 802.1Q version number supported by this device
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qVlanVersionGet()
{
  return dot1qInfo->version;
}


/*********************************************************************
* @purpose  Get dot1q Max VLAN ID
*
* @param    none
*
* @returns  Maximum VLAN ID supported by this device
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qMaxVlanIDGet()
{
  return dot1qInfo->MaxVLANID;
}


/********************************************************************
* @purpose  Get dot1q Max Supported VLANs
*
* @param    none
*
* @returns  Maximum number of VLANs supported by this device
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qMaxSupportedVlanGet()
{
  return L7_MAX_VLANS;
}


/*********************************************************************
* @purpose  Get dot1q Number Vlans
*
* @param    none
*
* @returns  number of VLANs currently configured on this device
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qCurrentVlanCountGet()
{
  return dot1qInfo->CurrentVLANCount;
}

/*********************************************************************
* @purpose  Update the current dot1q Number Vlans
*
* @param    increment  Boolean parameter to add or delete the count
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void dot1qCurrentVlanCountUpdate(L7_BOOL increment)
{

   osapiSemaTake(dot1qVlanCountSem, L7_WAIT_FOREVER);
   if (increment == L7_TRUE)
   {
     dot1qInfo->CurrentVLANCount++;
   }
   else
   {
     dot1qInfo->CurrentVLANCount--;
   }
   osapiSemaGive(dot1qVlanCountSem);
}


/*********************************************************************
* @purpose  Get dot1q Most Vlans Used
*
* @param    none
*
* @returns  number of most VLANs ever in use on this device
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1qMostVlanCountGet()
{
  return dot1qInfo->MostVLANCount;
}

/*********************************************************************
* @purpose  Get dot1q number Vlans deleted
*
* @param    *NumDeletes   number of times a VLAN has been deleted
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanNumDeletesGet(L7_uint32 *NumDeletes)
{
  *NumDeletes = dot1qInfo->VLANDeletionCount;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get set of ports which are members of this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Like dot1qVlanCurrentEgressPortsGet() below, but doesn't
*           reverse the bit order in each byte of mask.
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanEgressPortsGet(L7_uint32 vlanId, NIM_INTF_MASK_t *mask)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vlanId, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  L7_INTF_MASKEQ(*mask, dataEntry.current.currentEgressPorts);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get set of ports which are members of this vlan
*
* @param    vlanId      VLAN ID
* @param    *mask       pointer to a storage area to contain returned
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCurrentEgressPortsGet(L7_uint32 vlanId, NIM_INTF_MASK_t * mask, L7_uint32 *size)
{
  L7_RC_t rc;
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vlanId, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  *size = NIM_INTF_INDICES;

  rc = dot1qReverseMask(dataEntry.current.currentEgressPorts, mask);

  return rc;
}


/*********************************************************************
* @purpose  Get set of ports which are transmitting untagged frames
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCurrentUntaggedPortsGet(L7_uint32 vlanId, NIM_INTF_MASK_t *mask, L7_uint32 *size)
{
  dot1q_vlan_dataEntry_t dataEntry;
  NIM_INTF_MASK_t tmpmask;

  if (vlanDataGet(pVlanTree, vlanId, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  memcpy(&tmpmask, &(dataEntry.current.taggedSet), sizeof(NIM_INTF_MASK_t));

  /* invert mask to get untagged ports */
  NIM_INTF_MASKINV(tmpmask);


  (void) dot1qReverseMask(tmpmask, mask);
  *size = NIM_INTF_INDICES;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the VLAN status
*
* @param    vid         vlan ID
* @param    *type       pointer to mode of participation
*                       (@b{  Default, Admin,GVRPRegistered})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStatusGet(L7_uint32 vid,  L7_uint32 *type)
{
  dot1q_vlan_dataEntry_t dataEntry;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  *type = dataEntry.current.status;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Determine if a new VLAN can be supported
*
* @param    none
*
* @returns  0, if another VLAN cannot be supported
* @returns  >=4096 if another VLAN can be supported
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qNextFreeLocalVlanIndexGet()
{
  L7_uint32 vid = 1;

  while (vid <= L7_MAX_VLANS)
  {
    if (dot1qVlanCheckValid(vid) == L7_NOT_EXISTS)
      return 4096;
    vid++;
  }

  return 0;
}


/*********************************************************************
* @purpose  Get a list of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    dtlEvent    event
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
L7_RC_t dot1qVIDListGet(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, L7_uint32 *data, L7_uint32 *numOfElements)
{
  L7_RC_t rc;
  dot1q_vlan_dataEntry_t *pDataEntry;
  nimUSP_t usp;
  dot1qVidList_t *list;
  list = (dot1qVidList_t*)data;

  *numOfElements = 0;

  rc = nimGetUnitSlotPort(intfNum, &usp);
  if (rc == L7_ERROR)
    return L7_NOT_EXISTS;

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
  pDataEntry = (dot1q_vlan_dataEntry_t*)(vlanFirstDataEntry(pVlanTree));
  while (pDataEntry != NULL)
  {
    if (NIM_INTF_ISMASKBITSET(pDataEntry ->current.currentEgressPorts, intfNum))
    {
      list[*numOfElements].vid = pDataEntry -> vlanId;

      if (NIM_INTF_ISMASKBITSET(pDataEntry->current.taggedSet, intfNum))
      {
        list[*numOfElements].tag = L7_TRUE;
      }
      else
        list[*numOfElements].tag = L7_FALSE;

      (*numOfElements)++;
    }

    pDataEntry = vlanNextDataEntry(pVlanTree, pDataEntry ->vlanId);
  }

  osapiSemaGive(dot1qSem);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get a list of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    dtlEvent    event
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
L7_RC_t dot1qVIDListCfgGet(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, L7_uint32 *data, L7_uint32 *numOfElements)
{
  L7_RC_t rc;
  dot1q_vlan_dataEntry_t *pDataEntry;
  nimUSP_t usp;
  dot1qVidList_t *list;
  list = (dot1qVidList_t*)data;

  *numOfElements = 0;

  rc = nimGetUnitSlotPort(intfNum, &usp);
  if (rc == L7_ERROR)
    return L7_NOT_EXISTS;

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
  pDataEntry = (dot1q_vlan_dataEntry_t*)(vlanFirstDataEntry(pVlanTree));
  while (pDataEntry != NULL)
  {
    if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.staticEgressPorts, intfNum))
    {
      list[*numOfElements].vid = pDataEntry -> vlanId;

      if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet, intfNum))
      {
        list[*numOfElements].tag = L7_TRUE;
      }
      else
        list[*numOfElements].tag = L7_FALSE;

      (*numOfElements)++;
    }

    pDataEntry = vlanNextDataEntry(pVlanTree, pDataEntry ->vlanId);
  }

  osapiSemaGive(dot1qSem);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get bit masked list of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    *data       pointer to data
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
*
* @notes   Storage accounted for by the caller. should have sizeof(DOT1Q_VLAN_MASK_t)
* @notes   allocated for the data.
* @end
*********************************************************************/
L7_RC_t dot1qVIDListMaskGet(L7_uint32 intfNum, L7_VLAN_MASK_t *data)
{

    nimUSP_t usp;
    L7_RC_t rc = nimGetUnitSlotPort(intfNum, &usp);

    if (rc == L7_ERROR)
        return L7_NOT_EXISTS;

    if (data != L7_NULLPTR)
    {
      memcpy(data,&dot1qIntfVidList[intfNum], sizeof(L7_VLAN_MASK_t));
    }

    return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Get number of interface included in a VLANs
*
* @param    intfNum     interface
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
* @end
*********************************************************************/
L7_RC_t dot1qIntfInVIDnumGet(L7_uint32 vlanId, NIM_INTF_MASK_t *mMask, L7_uint32 *numOfElements)
{
  dot1q_vlan_dataEntry_t dataEntry;
  NIM_INTF_MASK_t      currentMask;
  L7_uint32   index;
  L7_uint32   highestIntfNumber;
  L7_BOOL     entryPresent = L7_FALSE;



  *numOfElements = 0;
  highestIntfNumber = 0;

  if (nimGetHighestIntfNumber(&highestIntfNumber) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
              "%s: nimGetHighestIntfNumber failed \n",__FUNCTION__);
      return L7_FAILURE;
  }


  if (mMask == L7_NULL)
  {
    if (vlanDataGet(pVlanTree, vlanId, &dataEntry) ==  L7_TRUE)
    {
      memcpy(&currentMask,&dataEntry.current.currentEgressPorts,sizeof(NIM_INTF_MASK_t));
      entryPresent = L7_TRUE;
    }
    else 
    {
      return L7_FAILURE;

    }

  }
  else
  {
    memcpy(&currentMask,mMask,sizeof(NIM_INTF_MASK_t));
  }


  if ((entryPresent) || (mMask != L7_NULL))
  {
    for (index = 1; index <= highestIntfNumber; index++)
    {
      if (nimCheckIfNumber(index) != L7_SUCCESS)
          continue;

      if (NIM_INTF_ISMASKBITSET(currentMask, index))
      {
        (*numOfElements)++;
      }
    }
  }


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get number of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
* @end
*********************************************************************/
L7_RC_t dot1qVIDnumGet(L7_uint32 intfNum, L7_uint32 *numOfElements)
{
  L7_RC_t rc;
  dot1q_vlan_dataEntry_t *pDataEntry;
  nimUSP_t usp;

  *numOfElements = 0;

  rc = nimGetUnitSlotPort(intfNum, &usp);
  if (rc == L7_ERROR)
    return L7_NOT_EXISTS;

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
  pDataEntry = (dot1q_vlan_dataEntry_t*)(vlanFirstDataEntry(pVlanTree));
  while (pDataEntry != NULL)
  {
    if (NIM_INTF_ISMASKBITSET(pDataEntry ->current.currentEgressPorts, intfNum))
    {
      (*numOfElements)++;
    }

    pDataEntry = vlanNextDataEntry(pVlanTree, pDataEntry ->vlanId);
  }
  osapiSemaGive(dot1qSem);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if an interface has been acquired by a LAG
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
L7_RC_t dot1qIntfAcquiredGet(L7_uint32 intIfNum, L7_uint32 *status)
{
  nimUSP_t usp;
  L7_uint32   currAcquired;

  L7_RC_t rc = nimGetUnitSlotPort(intIfNum, &usp);

  if (rc == L7_SUCCESS)
  {
    COMPONENT_ACQ_NONZEROMASK(dot1qQports[intIfNum].acquiredList, currAcquired);
    *status = (currAcquired) ? L7_TRUE : L7_FALSE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Gets the component ID which has acquired the interface
*
* @param    intIfNum        interface
* @param    *compId         the ID of the acquiring component
*
* @returns  L7_SUCCESS     if success
* @returns  L7_NOT_EXISTS  if VLAN does not exist
*
* @notes    Storage must be allocated by the caller
*           The component ID returned is either valid or L7_FIRST_COMPONENT_ID
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfAcquiredCompGet(L7_uint32 intIfNum, L7_COMPONENT_IDS_t *compId)
{
  nimUSP_t  usp;
  L7_uint32 i;
  L7_RC_t   rc;

  *compId = L7_FIRST_COMPONENT_ID;

  rc = nimGetUnitSlotPort(intIfNum, &usp);
  if (rc == L7_SUCCESS)
  {
    for (i = L7_FIRST_COMPONENT_ID + 1; i < L7_LAST_COMPONENT_ID; i++)
    {
      if (COMPONENT_ACQ_ISMASKBITSET(dot1qQports[intIfNum].acquiredList, i))
      {
        *compId = i;
        break;
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Resets VLAN counters
*
* @param    void

* @returns  L7_SUCCESS, if success
*
* @notes    Invoked during clear stats switch
*
* @end
*********************************************************************/
void dot1qVlanResetStats(void)
{
  dot1qInfo->MostVLANCount = dot1qInfo->CurrentVLANCount;
  dot1qInfo->VLANDeletionCount = 0;
}

/*********************************************************************
* @purpose  See if an interface type is of the right type for VLANs
*
* @param    intfType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsValidType(L7_uint32 intfType)
{
  /* Must be either a physical or a LAG intertface */
   if ((intfType != L7_PHYSICAL_INTF) && (intfType != L7_LAG_INTF) &&
      (intfType != L7_CAPWAP_TUNNEL_INTF))
   {
      return L7_FALSE;
   }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  See if an interface is of the right type for VLANs
*
* @param    intifNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsValid(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_INTF_TYPES_t intfType;

  /* Check for valid interface number */
  rc = nimCheckIfNumber(intIfNum);
  if (rc == L7_ERROR)
    return L7_FALSE;

  /* Check for physical interface or LAG interface */
  rc = nimGetIntfType(intIfNum, &intfType);
  if (rc == L7_SUCCESS)
  {
    /* Must be either a physical or a LAG intertface */
    if ((intfType != L7_PHYSICAL_INTF) && (intfType != L7_LAG_INTF) &&
        (intfType != L7_CAPWAP_TUNNEL_INTF))
    {
      return L7_FALSE;
    }

    /* If this is a LAG interface number, make sure it is configured */
    if (intfType == L7_LAG_INTF)
    {
      if (dot3adIsLagConfigured(intIfNum) == L7_FALSE)
        return L7_FALSE;
    }

    return L7_TRUE;
  }

  return L7_FALSE;
}


/*********************************************************************
* @purpose  See if an interface mask is completely valid for dot1q
*
* @param    *intIfMask      pointer to an L7_INTF_MASK_t structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsValidMask(L7_INTF_MASK_t *intIfMask)
{
  NIM_INTF_MASK_t lagMask;
  NIM_INTF_MASK_t l2tnnlMask;
  NIM_INTF_MASK_t physMask;
  NIM_INTF_MASK_t tempMask;
  NIM_INTF_MASK_t testMask;
  L7_BOOL isSet;

  /*If Mask with all Zeros is passed, do not check for valid interfaces
    as there are no specific interfaces enabled in Mask that needs to be checked for validity*/
  memcpy( (L7_uchar8 *)&testMask, (L7_uchar8 *)intIfMask, sizeof(testMask));
  memset(&tempMask, 0, sizeof(NIM_INTF_MASK_t));

  NIM_INTF_NONZEROMASK(testMask, isSet);
  if(isSet==L7_FALSE)
    return L7_TRUE;


  memset(&physMask, 0, sizeof(physMask));
  memset(&lagMask, 0, sizeof(lagMask));
  memset(&l2tnnlMask, 0, sizeof(l2tnnlMask));


  (void) nimPhysicalIntfMaskGet(&physMask);
  (void) nimLagIntfMaskGet(&lagMask);
  (void) nimL2TnnlIntfMaskGet(&l2tnnlMask);


  /*  OR the valid interfaces */
  NIM_INTF_MASKOREQ(tempMask, physMask);
  NIM_INTF_MASKOREQ(tempMask, lagMask);
  NIM_INTF_MASKOREQ(tempMask, l2tnnlMask);

  /*   AND the mask passed in */
  NIM_INTF_MASKANDEQ(tempMask,testMask);

  /* Determine if any bits are still set */
  NIM_INTF_NONZEROMASK(tempMask, isSet);

  return isSet;

}

/*********************************************************************
* @purpose  Get VLAN port membership and type
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface in question
* @param    *mode       pointer to mode of participation:
*                       L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN or
*                       L7_DOT1Q_NORMAL_REGISTRATION
* @param    *type       pointer to type of participation:
*                       L7_DOT1Q_DEFAULT, L7_DOT1Q_ADMIN   or
*                       L7_DOT1Q_DYNAMIC
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    This function is a combination of dot1qVlanMemberGet() and
*           dot1qOperVlanTypeGet().  In cases when we need to call the
*           mentioned two functions one after another,
*           dot1qVlanMemberAndTypeGet() should be called instead
*           (this will save some time on calling vlanDataSearch() and
*           grabbing/releasing semaphore).
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberAndTypeGet(L7_uint32 vid,   L7_uint32 intIfNum,
                                  L7_uint32 *mode, L7_uint32 *type)
{
  dot1q_vlan_dataEntry_t dataEntry;

  *mode = 0;
  *type = 0;

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    return L7_NOT_EXISTS;
  }

  /* to stop GVRP from requesting intIfNum 0 */
  if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
  {
     return L7_FAILURE;
  }


  if ( (dataEntry.current.status == L7_DOT1Q_TYPE_ADMIN) ||
       (dataEntry.current.status == L7_DOT1Q_TYPE_DEFAULT) )
  {
      if (NIM_INTF_ISMASKBITSET(dataEntry.admin->vlanCfg.staticEgressPorts, intIfNum))
      {
        *mode = L7_DOT1Q_FIXED;
      }
      else if (NIM_INTF_ISMASKBITSET(dataEntry.admin->vlanCfg.forbiddenEgressPorts,intIfNum))
      {
        *mode = L7_DOT1Q_FORBIDDEN;
      }
      else
      {
        *mode = L7_DOT1Q_NORMAL_REGISTRATION;
      }
  }  /* Admin or Default VLAN */
  else
  {

      /* For GVRP registered VLANs, the port either is or is not currently
         participating in the VLAN.    If currently participating, return "fixed",
         else return "forbidden"

         Note that the pointer to the VLAN cfg structure will not be valid if
         the vlan is dynamic.

       */

      if (NIM_INTF_ISMASKBITSET(dataEntry.current.currentEgressPorts,intIfNum))
      {
        *mode = L7_DOT1Q_FIXED;
      }
       else
      {
        *mode = L7_DOT1Q_FORBIDDEN;
      }
  } /* Gvrp Registered */

  *type = dataEntry.current.status;


  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Get the vlanid corresponding to this interface number
*
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of this vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIntIfNumToVlanId(L7_uint32 intIfNum, L7_uint32 *vlanId)
{
  L7_uint32 i;

  for (i=1; (L7_int32)i <= L7_MAX_NUM_VLAN_INTF; i++)
  {
    if (dot1qVlanIntf[i].inUse != L7_TRUE)
           continue;

    if (dot1qVlanIntf[i].intIfNum == intIfNum)
    {
        *vlanId = dot1qVlanIntf[i].vlanId;
        return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}



/*********************************************************************
* @purpose  Get the list of interfaces that satisfy the switchport mode restrictions
*
* @param    mask        in/out interface Mask
* @param    mode        switchport mode DOT1Q_SWPORT_MODE_GENERAL
*                       (@b{  DOT1Q_SWPORT_MODE_ACCESS, DOT1Q_SWPORT_MODE_TRUNK
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for the ports for which membership
*           is changing and in turn will clear out the bits for ports which do not satisfy
*           switchport mode rules.
*
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfListForValidMode(NIM_INTF_MASK_t * mask, DOT1Q_SWPORT_MODE_t swport_mode)
{
  DOT1Q_SWPORT_MODE_t curr_mode;
  NIM_INTF_MASK_t currMask;
  L7_uint32 curr;
  L7_RC_t rc = L7_SUCCESS;

  memcpy(&currMask, mask, sizeof(NIM_INTF_MASK_t));

  NIM_INTF_FHMASKBIT(currMask,curr);
  while ( curr != 0 )
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(curr, L7_SYSNAME, ifName);

    rc = dot1qSwitchPortModeGet(curr, &curr_mode);
    if (rc != L7_SUCCESS)
    {
        /* Need to decide what to do if one interface fails
        for now return error code*/
        return rc;
    }
    if (curr_mode != swport_mode )
    {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
               "Incorrect switchport mode for vlan participation. Mode: %d Interface: %s", swport_mode,  ifName);
       NIM_INTF_CLRMASKBIT((*mask),curr);
    }

    NIM_INTF_CLRMASKBIT(currMask,curr);
    /* Get the next valid interface */
    NIM_INTF_FHMASKBIT(currMask,curr);

  } /* While curr*/


  return rc;

}

/*********************************************************************
* @purpose  Filter dynamic ports from "L7_ALL_INTERFACES" vlan participation modifications
*
* @param    mask        in/out interface Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for the ports for which membership
*           is changing and in turn will clear out the bits for ports which do not satisfy
*           switchport mode rules.
*
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfListFilter(NIM_INTF_MASK_t * mask)
{
  NIM_INTF_MASK_t currMask;
  L7_uint32 curr, intfType;
  L7_RC_t rc = L7_SUCCESS;

  memcpy(&currMask, mask, sizeof(NIM_INTF_MASK_t));

  NIM_INTF_FHMASKBIT(currMask,curr);
  while ( curr != 0 )
  {
    rc = nimGetIntfType(curr, &intfType);
    if (rc == L7_SUCCESS && intfType == L7_CAPWAP_TUNNEL_INTF)
    {
       NIM_INTF_CLRMASKBIT((*mask),curr);
    }

    NIM_INTF_CLRMASKBIT(currMask,curr);
    /* Get the next valid interface */
    NIM_INTF_FHMASKBIT(currMask,curr);

  } /* While curr*/

  return rc;
}

/*********************************************************************
* @purpose  Set VLAN port membership for a range of interfaces
*
* @param    vid         vlan ID
* @param    mask        interface Mask
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the ports,
*           whose membership is changing, in other words only the delta
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanMemberMaskSet(L7_uint32 vid, NIM_INTF_MASK_t * mask, L7_uint32 mode, DOT1Q_SWPORT_MODE_t swport_mode)
{
  L7_RC_t rc= L7_FAILURE;
  DOT1Q_MSG_t msg;
  dot1qVlanCfgData_t *vCfg = L7_NULL;


  /* range check*/
  if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
  {
      return rc;
  }

  if ((mode != L7_DOT1Q_FIXED) &&
      (mode !=L7_DOT1Q_NORMAL_REGISTRATION) &&
      (mode !=L7_DOT1Q_FORBIDDEN))
  {
      return L7_FAILURE;
  }


  dot1qIntfListForValidMode(mask, swport_mode);
  dot1qIntfListFilter(mask);


  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  /* Update the config structure */
  (void) dot1qVlanIsConfigurable(vid, &vCfg);
  if (vCfg == L7_NULL)
  {
      /* A static VLAN does not exist */
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_NOT_EXISTS;
  }

  /*(void)dot1qVlanMemberSetCfgUpdate(vid, intIfNum, mode);*/
  switch (mode)
  {
      case L7_DOT1Q_FIXED:
      {
        /* Add these ports in static list clear from forbidden list*/
        NIM_INTF_MASKOREQ(vCfg->vlanCfg.staticEgressPorts,(*mask));
        if (swport_mode == DOT1Q_SWPORT_MODE_TRUNK)
        {
          NIM_INTF_MASKOREQ(vCfg->vlanCfg.taggedSet,(*mask));
        }
        NIM_INTF_MASKANDEQINV(vCfg->vlanCfg.forbiddenEgressPorts,(*mask));
      }
      break;

      case L7_DOT1Q_NORMAL_REGISTRATION:
      {
        NIM_INTF_MASKANDEQINV(vCfg->vlanCfg.staticEgressPorts,(*mask));
        NIM_INTF_MASKANDEQINV(vCfg->vlanCfg.forbiddenEgressPorts,(*mask));
      }
      break;

      case L7_DOT1Q_FORBIDDEN:
      {
        /* Clear in static list and add in forbidden list */
        NIM_INTF_MASKANDEQINV(vCfg->vlanCfg.staticEgressPorts,(*mask));
        NIM_INTF_MASKOREQ(vCfg->vlanCfg.forbiddenEgressPorts,(*mask));
      }
      break;

   }




    dot1qCfg->hdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(dot1qCfgRWLock);

  /* Put a message on the queue */

  memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
  msg.vlanId = vid;
  memcpy(&msg.data.intfMask, mask, sizeof(NIM_INTF_MASK_t));
  msg.mode = mode;
  msg.swport_mode = swport_mode;
  msg.event =  dot1q_vlan_member_set_delta_mask;



  rc = dot1qIssueCmd(&msg);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
  }

    return rc; /* serviced in request tree */
}
/*********************************************************************
* @purpose  Set the tagging configuration for a range of interfaces
*
* @param    vid         vlan ID
* @param    mask        interface mask
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the ports,
*           whose membership is changing, in other words only the delta
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanTaggedMemberMaskSet(L7_uint32 vid, NIM_INTF_MASK_t * mask, L7_uint32 tagMode)
{
    L7_RC_t rc= L7_FAILURE;
    DOT1Q_MSG_t msg;
    dot1qVlanCfgData_t *vCfg = L7_NULL;



    /* range check*/
    if ((vid <  L7_DOT1Q_MIN_VLAN_ID) || (vid >L7_DOT1Q_MAX_VLAN_ID))
    {
        return rc;
    }

    if ((tagMode != L7_DOT1Q_UNTAGGED) &&
        (tagMode !=L7_DOT1Q_TAGGED))
    {
        return L7_FAILURE;
    }

    /* Update the configuration file */

    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    (void) dot1qVlanIsConfigurable(vid, &vCfg);
    if (vCfg == L7_NULL)
    {
        /* A static VLAN does not exist */
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_NOT_EXISTS;
    }

    /* Only process interfaces that are members of the VLAN */
    NIM_INTF_MASKANDEQ((*mask),vCfg->vlanCfg.staticEgressPorts);

    /*(void)dot1qVlanTaggedMemberSetCfgUpdate(vid, intIfNum, tagMode);*/
    if (tagMode == L7_DOT1Q_TAGGED)
    {
          /* Add these ports in tagged list*/
       NIM_INTF_MASKOREQ(vCfg->vlanCfg.taggedSet,(*mask));
    }
    else
    {
       NIM_INTF_MASKANDEQINV(vCfg->vlanCfg.taggedSet,(*mask));
    }


    dot1qCfg->hdr.dataChanged = L7_TRUE;

    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    msg.vlanId = vid;
    memcpy(&msg.data.intfMask, mask, sizeof(NIM_INTF_MASK_t));
    msg.mode = tagMode;
    msg.event = dot1q_vlan_tagged_set_delta_mask;


    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }


    return rc;
}


/******************************************************************
* @purpose  Check to see if a VLAN exists based on a
*           VLAN ID and it is static.
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qStaticVlanCheckValid(L7_uint32 vid)
{
  dot1q_vlan_dataEntry_t dataEntry;
  L7_RC_t                status;

  if (dot1qInfo->initialized != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* range check vlan ID */
  if ( (vid <= L7_DOT1Q_NULL_VLAN_ID) || (vid > L7_DOT1Q_MAX_VLAN_ID) )
  {
      return L7_FAILURE;
  }

  if (vlanDataGet(pVlanTree, vid, &dataEntry) == L7_FALSE)
  {
    status = L7_NOT_EXISTS;
  }
  else if (dataEntry.vlanId == vid && dataEntry.current.status != L7_DOT1Q_TYPE_GVRP_REGISTERED &&
           dataEntry.current.status != L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED &&
           dataEntry.current.status != L7_DOT1Q_TYPE_DOT1X_REGISTERED &&
           dataEntry.current.status != L7_DOT1Q_TYPE_IP_INTERNAL_VLAN)

  {
    status = L7_SUCCESS;
  }
  else
  {
    status = L7_FAILURE;
  }

  return status;
}
/*********************************************************************
* @purpose  For a given interface set vlan membership for a range of vlans
*
* @param    intIfNum    Internal Interface Number
* @param    vidMask     Vlan ID Mask
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the vlans,
*           whose membership is changing, in other words only the delta
*           This routine will set the membership for all valid vlans and
*           not abort in the case of a non existant or dynamic vlan
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVlanMembershipMaskSet(L7_uint32 intIfNum, L7_VLAN_MASK_t *mask, L7_uint32 mode, DOT1Q_SWPORT_MODE_t swport_mode)
{
  L7_RC_t rc= L7_FAILURE, rc1 = L7_SUCCESS;
  DOT1Q_MSG_t msg;
  dot1qVlanCfgData_t *vCfg = L7_NULL;
  L7_uint32 maskOffset = 0;
  L7_uint32 vlanId;
  L7_uint32 validVlans = 0;
  L7_BOOL valid = L7_FALSE;
  L7_VLAN_MASK_t vidMask = *mask;
  dot1qIntfCfgData_t *pCfg = L7_NULL;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* Check for valid interface number */
  if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "Invalid dot1q Interface: %s", ifName);
    return L7_FAILURE;
  }
  if (nimConfigIdMaskOffsetGet(intIfNum, &maskOffset) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Could not get nimMaskIdOffset for Interface %s",  ifName);
    return L7_FAILURE;
  }



    
  /* Check if the mode is valid */
  if ((mode != L7_DOT1Q_FIXED) &&
      (mode != L7_DOT1Q_NORMAL_REGISTRATION) &&
      (mode != L7_DOT1Q_FORBIDDEN))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Incorrect mode for vlan participation. Mode: %d Interface: %s", mode,  ifName);
      return L7_FAILURE;
  }

  valid = dot1qMapIntfIsConfigurable(intIfNum, &pCfg);
  if (valid == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Intf is not configurable: Interface: %s", ifName);
    return L7_FAILURE;
  }
  if ((pCfg->portCfg.switchport_mode != swport_mode) ||
       (pCfg->portCfg.switchport_mode == DOT1Q_SWPORT_MODE_ACCESS))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Incorrect mode for vlan participation. Mode: %d Interface: %s", swport_mode,  ifName);
    return L7_FAILURE;
  }

  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
#if defined(FEAT_METRO_CPE_V1_0)
  DOT1AD_INTFERFACE_TYPE_t intfType;
  L7_uint32 mgmtVlanId;

  if (!overrideMgmtVlanSet)
  {
    mgmtVlanId = simMgmtVlanIdGet();
    if ((mode == L7_DOT1Q_FIXED || mode == L7_DOT1Q_NORMAL_REGISTRATION) &&
        dot1adInterfaceTypeGet(intIfNum, &intfType) == L7_SUCCESS &&
        (
         (intfType == DOT1AD_INTFERFACE_TYPE_UNI) ||
         (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)||
         (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
        )
      )
    {
      if (L7_VLAN_ISMASKBITSET(vidMask, mgmtVlanId))
      {
        L7_VLAN_CLRMASKBIT(vidMask, mgmtVlanId);
        rc1 = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot set membership for user interface %s on management vlan %d", ifName,  mgmtVlanId);
      }
    }
  }
#endif
  /* For each vlan id that is set in the mask check if it is configurable */
  for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
  {
    if (L7_VLAN_ISMASKBITSET(vidMask, vlanId))
    {
      /* VlanId is set in the mask, now check if it is configurable*/
      valid =  dot1qVlanIsConfigurable(vlanId, &vCfg);
      if (valid == L7_FALSE) 
      {
        /* A static VLAN does not exist */
        /* reset the bit in the mask so that we do not access this in process side of the Q*/
        L7_VLAN_CLRMASKBIT(vidMask, vlanId);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot set membership for interface %d on non existant vlan %d", intIfNum,  vlanId);
        rc1 = L7_FAILURE;
        continue; /*Continue to the next vlan id */
      }

      validVlans++; /* If there are zero valid vlans then no need to send a msg to the Q */
      /* For each valid vlan id update it's config egress, forbidden interface mask for this interface */

      switch (mode)
      {
        case L7_DOT1Q_FIXED:
          /* Add these ports in static list clear from forbidden list*/
          NIM_INTF_SETMASKBIT(vCfg->vlanCfg.staticEgressPorts, maskOffset);
          NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts, maskOffset);
          if (swport_mode == DOT1Q_SWPORT_MODE_TRUNK)
          {
            NIM_INTF_SETMASKBIT(vCfg->vlanCfg.taggedSet, maskOffset);
          }
          break;

        case L7_DOT1Q_NORMAL_REGISTRATION:
          /* Clear from both static and forbidden list*/
          NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts, maskOffset);
          NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts, maskOffset);
          break;

        case L7_DOT1Q_FORBIDDEN:
        /* Clear in static list and add in forbidden list */
          NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts, maskOffset);
          NIM_INTF_SETMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts, maskOffset);
          break;

        default:
          break;
      }
    }
  }
  dot1qCfg->hdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(dot1qCfgRWLock);

  /* Put a message on the queue */
  if (validVlans > 0)
  {
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
    msg.data.intfVlanMask.intIfNum = intIfNum;
    memcpy(&msg.data.intfVlanMask.vlanMask, &vidMask, sizeof(L7_VLAN_MASK_t));
    msg.mode = mode;
    msg.swport_mode = swport_mode;
    msg.event =  dot1q_qports_vlan_membership_delta_mask_set;
    msg.requestor = DOT1Q_ADMIN;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }
  }
  else
    rc = L7_FAILURE;

  if (rc1 == L7_FAILURE)
  {
    return rc1;
  }
  return rc; /* serviced in request tree */
}
/*********************************************************************
* @purpose  Set the tagging configuration for a interface on a range of vlans
*
* @param    intIfNum    Internal Interface Number
* @param    vidMask     Vlan ID Mask
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the vlans,
*           whose tagging is changing, in other words only the delta
*           This routine will set the tagging for all valid vlans and
*           not abort in the case of a non existant or dynamic vlan
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVlanTagMaskSet(L7_uint32 intIfNum, L7_VLAN_MASK_t *mask, L7_uint32 tagMode)
{
  L7_RC_t rc= L7_FAILURE, rc1 = L7_SUCCESS;
  DOT1Q_MSG_t msg;
  dot1qVlanCfgData_t *vCfg = L7_NULL;
  L7_uint32 maskOffset = 0;
  L7_uint32 vlanId;
  L7_uint32 validVlans = 0;
  L7_BOOL valid = L7_FALSE;
  L7_VLAN_MASK_t vidMask = *mask;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* Check for valid interface number */
  if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Invalid dot1q Interface: %s", ifName);
    return L7_FAILURE;
  }

  if (nimConfigIdMaskOffsetGet(intIfNum, &maskOffset) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Could not get nimMaskIdOffset for Interface %s",  ifName);
    return L7_FAILURE;
  }


  /* Check if the tagMode is valid */
  if ((tagMode != L7_DOT1Q_TAGGED) &&
      (tagMode != L7_DOT1Q_UNTAGGED))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "Incorrect tagmode for vlan tagging. tagmode: %d Interface: %s", tagMode,  ifName);
      return L7_FAILURE;
  }

  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  /* For each vlan id that is set in the mask check if it is configurable */
  for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
  {
    if (L7_VLAN_ISMASKBITSET(vidMask, vlanId))
    {
      /* VlanId is set in the mask, now check if it is configurable*/
      valid =  dot1qVlanIsConfigurable(vlanId, &vCfg);
      if (valid == L7_FALSE) 
      {
        /* A static VLAN does not exist */
        /* reset the bit in the mask so that we do not access this in process side of the Q*/
        L7_VLAN_CLRMASKBIT(vidMask, vlanId);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot set tagging for interface %d on non existant vlan %d", intIfNum,  vlanId);
        rc1 = L7_FAILURE;
        continue; /*Continue to the next vlan id */
      }
      else if (!NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.staticEgressPorts,intIfNum))
      {
        /* A static VLAN does not exist */
        /* reset the bit in the mask so that we do not access this in process side of the Q*/
        L7_VLAN_CLRMASKBIT(vidMask, vlanId);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot set tagging for interface %d which is not a member of vlan %d", intIfNum,  vlanId);
        rc1 = L7_FAILURE;
        continue; /*Continue to the next vlan id */
      }

      validVlans++; /* If there are zero valid vlans then no need to send a msg to the Q */
      /* For each valid vlan id update it's config egress, forbidden interface mask for this interface */

      if (tagMode == L7_DOT1Q_TAGGED)
      {
        /* Add these ports in tagged list*/
        NIM_INTF_SETMASKBIT(vCfg->vlanCfg.taggedSet,maskOffset);
      }
      else
      {
        NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet,maskOffset);
      }
    }
  }
  dot1qCfg->hdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(dot1qCfgRWLock);

  if (validVlans > 0)
  {
    /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    msg.mode = tagMode;
    msg.data.intfVlanMask.intIfNum = intIfNum;
    memcpy(&msg.data.intfVlanMask.vlanMask, &vidMask, sizeof(L7_VLAN_MASK_t));
    msg.mode = tagMode;
    msg.event = dot1q_qports_vlan_tagging_delta_mask_set; 
    msg.requestor = DOT1Q_ADMIN;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }
  }
  else
    rc = L7_FAILURE;

  if (rc1 == L7_FAILURE)
  {
    return rc1;
  }
  return rc;
}

/*********************************************************************
* @purpose  Revert to configured vlan settings
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberRevert(L7_uint32 vid, L7_uint32 intIfNum)
{
  L7_uint32 mode;
  DOT1Q_MSG_t msg;

  if(dot1qVlanMemberGet(vid,intIfNum,&mode)== L7_SUCCESS)
  {
    /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
    msg.vlanId = vid;
    msg.data.intIfNum = intIfNum;
    msg.mode = mode;
    msg.event = dot1q_vlan_member_set;
    msg.requestor = DOT1Q_ADMIN;
    return dot1qIssueCmd(&msg);
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Revert to Dynamic vlan settings
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qDynamicVlanMemberRevert(L7_uint32 vid, L7_uint32 intIfNum)
{
  DOT1Q_MSG_t msg;

  /* Put a message on the queue */
  memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
  msg.vlanId = vid;
  msg.data.intIfNum = intIfNum;
  msg.mode = L7_DOT1Q_FORBIDDEN;
  msg.event = dot1q_vlan_member_set;
  msg.requestor =DOT1Q_DOT1X;

  return dot1qIssueCmd(&msg);
}

/*********************************************************************
* @purpose  Get the vlan type by requestor
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTypeGetByRequestor(L7_uint32 requestor, L7_uint32 *type)
{

  if((requestor < DOT1Q_ADMIN) || (requestor > DOT1Q_LAST_DYNAMIC_VLAN_COMPONENT)) 
  {
    return L7_FAILURE;
  }

  if(requestor == DOT1Q_ADMIN)
  {
    *type = L7_DOT1Q_TYPE_ADMIN;
  }
  else if(requestor == DOT1Q_GVRP)
  {
    *type = L7_DOT1Q_TYPE_GVRP_REGISTERED;
  }
  else if(requestor == DOT1Q_DOT1X)
  {
    *type = L7_DOT1Q_TYPE_DOT1X_REGISTERED;
  }
  else if(requestor == DOT1Q_WS_L2TUNNEL)
  {
    *type = L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED;
  }
  else if(requestor == DOT1Q_IP_INTERNAL_VLAN)
  {
    *type = L7_DOT1Q_TYPE_IP_INTERNAL_VLAN;
  }
  else if(requestor == DOT1Q_VOICE_VLAN)
  {
    *type = L7_DOT1Q_TYPE_ADMIN;
  }
  else 
  {
    return L7_FAILURE;
  } 
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Populate Config structure when changing the switch port mode
*
* @param    NIM_INTF_MASK   mask of ports to be set to trunk port.
* @param    swport mode     switch port mode = DOT1Q_SWPORT_MODE_TRUNK currently
*                           but can be extended later
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    - For each vlan Id present in the system,
*        for each port i.e. the mask bit set
*        configure vlan as member of the port
*        configure vlan as tagged
*
* @end
*********************************************************************/
static L7_RC_t dot1qSwitchPortPopulateCfg(NIM_INTF_MASK_t *mask,
                                          DOT1Q_SWPORT_MODE_t swport_mode,
                                          L7_uint32 accessVlan)
{
  L7_uint32 vlanId;
  L7_uint32 curr = 0;
  L7_BOOL valid = L7_FALSE;
  NIM_INTF_MASK_t currMask;
  dot1qVlanCfgData_t *vCfg = L7_NULL;

  for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
  {
    valid =  dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (valid == L7_TRUE)
    {
      memcpy(&currMask, mask, sizeof(currMask));
      NIM_INTF_FHMASKBIT(currMask,curr);
      while ( curr != 0 )
      {
        switch (swport_mode)
        {
          case DOT1Q_SWPORT_MODE_ACCESS:
            if (vlanId != accessVlan)
            {
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts, curr);
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, curr);
            }
            else
            {
              NIM_INTF_SETMASKBIT(vCfg->vlanCfg.staticEgressPorts, curr);
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, curr);
            }
            break;
          case DOT1Q_SWPORT_MODE_GENERAL:
            if (vlanId != FD_DOT1Q_DEFAULT_VLAN)
            {
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts, curr);
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, curr);
            }
            else
            {
              NIM_INTF_SETMASKBIT(vCfg->vlanCfg.staticEgressPorts, curr);
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, curr);
            }
            break;
          case DOT1Q_SWPORT_MODE_TRUNK:
            /* Remove this port from any vlans that might be present*/
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts, curr);
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, curr);
            break;
          default:
            return L7_FAILURE;

        }/* switch*/

        NIM_INTF_CLRMASKBIT(currMask,curr);
        /* Get the next valid interface */
        NIM_INTF_FHMASKBIT(currMask,curr);
      } /* While curr*/

    }/* Valid  is true*/
  } /* for */

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Change the switch port mode of the port(s)
*
* @param    NIM_INTF_MASK   list of ports for which to change the mode
* @param    DOT1Q_SWPORT_MODE_t  mode to be changed to
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortModeSet(NIM_INTF_MASK_t *mask,
                               DOT1Q_SWPORT_MODE_t swport_mode,
                               vlanRequestor_t requestor)
{
  DOT1Q_MSG_t msg;
  NIM_INTF_MASK_t currMask;
  L7_uint32 curr;
  dot1qIntfCfgData_t *pCfg = L7_NULL;
  L7_BOOL changeFlag = L7_FALSE;
  L7_uint32 pvid = FD_DOT1Q_DEFAULT_PVID;
  L7_uint32 accessVlan = FD_DOT1Q_DEFAULT_ACCESS_VLAN;
  L7_BOOL ingressFilter = FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT;
  L7_uint32 acceptFrameType = FD_DOT1Q_DEFAULT_ACCEPT_FRAME_TYPE;

  memcpy(&currMask, mask, sizeof(currMask));

  if (swport_mode == DOT1Q_SWPORT_MODE_ACCESS )
  {
      acceptFrameType = (L7_uchar8)L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED;
  }

  if (requestor == DOT1Q_ADMIN)
  {
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    /* Update the config structure */
    NIM_INTF_FHMASKBIT(currMask,curr);
    while ( curr != 0 )
    {
      if (dot1qMapIntfIsConfigurable(curr, &pCfg) == L7_TRUE)
      {
        if (pCfg->portCfg.switchport_mode  != swport_mode)
        {
          changeFlag = L7_TRUE;
          pCfg->portCfg.switchport_mode  = swport_mode;
          switch (swport_mode)
          {
            case DOT1Q_SWPORT_MODE_ACCESS:
              pvid = accessVlan;
              acceptFrameType = L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED;
              /* Fall thru */
            case DOT1Q_SWPORT_MODE_TRUNK:
              ingressFilter = L7_ENABLE;
              /* check for mode due to fall thru*/
              if (swport_mode == DOT1Q_SWPORT_MODE_TRUNK)
              {
                acceptFrameType = L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED;
              }
              /* Fall thru */
            default:
              pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID] = pvid;
                  pCfg->portCfg.acceptFrameType = acceptFrameType;
              pCfg->portCfg.enableIngressFiltering  = ingressFilter;
              pCfg->portCfg.access_vlan             = accessVlan;
              break;
          }
        }/* if mode is not the same*/
      }

      NIM_INTF_CLRMASKBIT(currMask,curr);
      /* Get the next valid interface */
      NIM_INTF_FHMASKBIT(currMask,curr);
    }

    /* return if nothing has changed */
    if (changeFlag != L7_TRUE)
    {
        (void)osapiWriteLockGive(dot1qCfgRWLock);
        return L7_SUCCESS;
    }
    if (dot1qSwitchPortPopulateCfg(mask, swport_mode, FD_DOT1Q_DEFAULT_ACCESS_VLAN) == L7_SUCCESS)
    {
      dot1qCfg->hdr.dataChanged = L7_TRUE;
    }

    (void)osapiWriteLockGive(dot1qCfgRWLock);
  }/* End requestor is Admin*/

  /* Put a message on the queue */
  memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
  memcpy(&msg.data.intfMask, mask, sizeof(NIM_INTF_MASK_t));
  msg.swport_mode = swport_mode;
  msg.event = dot1q_switchport_mode_set;
  msg.requestor = requestor;

  return dot1qIssueCmd(&msg);
}

/*********************************************************************
* @purpose  Set access vlanId for a port
*
* @param    intIfNum        physical or logical interface in question
* @param    vlanId         vlan ID
* @param    requestor    vlanRequestor_t type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortAccessVlanSet(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 requestor)
{
  DOT1Q_MSG_t msg;
  dot1qIntfCfgData_t *pCfg = L7_NULL;
  L7_RC_t rc = L7_FAILURE;
  NIM_INTF_MASK_t mask;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (dot1qIntfIsValid(intIfNum) != L7_TRUE) 
  { 
     return L7_FAILURE;
  }

  if (requestor == DOT1Q_ADMIN)
  {
    memset(&mask, 0, sizeof(mask));
    NIM_INTF_SETMASKBIT(mask, intIfNum);
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR,L7_DOT1Q_COMPONENT_ID,"Intf(%s) not configurable", ifName);
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return rc;
    }

    if (pCfg->portCfg.switchport_mode == DOT1Q_SWPORT_MODE_ACCESS)
    {
      pCfg->portCfg.access_vlan  = vlanId;
      dot1qSwitchPortPopulateCfg(&mask, DOT1Q_SWPORT_MODE_ACCESS, vlanId);
      dot1qCfg->hdr.dataChanged = L7_TRUE;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot set access vlan on port not in acces mode(%d)", pCfg->portCfg.switchport_mode);
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return rc;
    }


    (void)osapiWriteLockGive(dot1qCfgRWLock);
  }

  /* Put a message on the queue */
  memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));
  msg.data.intIfNum = intIfNum;
  msg.vlanId = vlanId;
  msg.event = dot1q_switchport_access_vlan_set;
  msg.requestor = requestor;

  if ((rc = dot1qIssueCmd(&msg)) == L7_SUCCESS)
  {
    /* Update the config for the pvid*/
    dot1qQportsVIDSet(intIfNum,vlanId, L7_DOT1Q_DEFAULT_VID_INDEX, DOT1Q_ADMIN);
  }

  return rc;

}

/*********************************************************************
* @purpose  Get access vlanId for a port
*
* @param    intIfNum        physical or logical interface in question
* @param    vlanId         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortAccessVlanGet(L7_uint32 intIfNum, L7_uint32* vlanId)
{
  dot1qIntfCfgData_t *pCfg = L7_NULL;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR,L7_DOT1Q_COMPONENT_ID,"Intf(%s) not configurable", ifName);
    (void)osapiReadLockGive(dot1qCfgRWLock);
    return rc;
  }

  if (pCfg->portCfg.switchport_mode == DOT1Q_SWPORT_MODE_ACCESS)
  {
    *vlanId = pCfg->portCfg.access_vlan  ;
    rc = L7_SUCCESS;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "Cannot Get access vlan on port not in acces mode(%d)", pCfg->portCfg.switchport_mode);
  }

  (void)osapiReadLockGive(dot1qCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Revert to configured vlan settings
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortModeGet(L7_uint32 intIfNum, L7_uint32 *mode )
{

  dot1qIntfCfgData_t *pCfg;
   L7_RC_t rc = L7_SUCCESS;

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
      *mode = pCfg->portCfg.switchport_mode ;
      /**mode = dot1qQports[intIfNum].a*/
  }
  else
      rc = L7_FAILURE;

  (void)osapiReadLockGive(dot1qCfgRWLock);
  return rc;

}

/*********************************************************************
* @purpose  Is the vlan access vlan for any port?
*
* @param    vid         vlan ID
*
* @returns  L7_TRUE, if yes L7_FALSE otherwise
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL dot1qIsAccessVlanForAnyPort(L7_uint32 vlanId )
{

  dot1qIntfCfgData_t *pCfg;
  DOT1Q_SWPORT_MODE_t swportMode;
  L7_uint32 accessVlan;
  L7_BOOL retVal = L7_FALSE;
  L7_uint32 curr=0, next = 0;
  L7_RC_t rc = L7_FAILURE;

  rc = nimFirstValidIntfNumber(&curr);

  (void)osapiReadLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

  while (rc == L7_SUCCESS)
  {

    if (dot1qMapIntfIsConfigurable(curr, &pCfg) == L7_TRUE)
    {
        swportMode = pCfg->portCfg.switchport_mode ;
        accessVlan = pCfg->portCfg.access_vlan;

        if ((swportMode == DOT1Q_SWPORT_MODE_ACCESS) && (accessVlan == vlanId))
        {
          retVal = L7_TRUE;
          break;
        }
    }

    rc = nimNextValidIntfNumber(curr, &next);
    curr = next;
  }

  (void)osapiReadLockGive(dot1qCfgRWLock);
  return retVal;

}

/*********************************************************************
* @purpose  Create a range of vlans
*
* @param    vidMask     Vlan ID Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    The vlan mask need not be contiguous
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCreateMask(L7_VLAN_MASK_t *vidMask)
{
    DOT1Q_MSG_t msg;
    L7_RC_t rc = L7_SUCCESS, rc2 = L7_SUCCESS;
    L7_uint32 vlanId;
    L7_BOOL valid = L7_FALSE;
    dot1qVlanCfgData_t *vCfg;
    L7_BOOL cfgEntryFound;

    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    /* For each vlan id that is set in the mask check if it is configurable */
    for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
    {
      if (L7_VLAN_ISMASKBITSET_POINTER(vidMask, vlanId))
      {
          /* VlanId is set in the mask, now check if it is configurable*/
        if (dot1qVlanCreateIsPending(vlanId) == L7_TRUE)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,"VLAN create currently underway for VLAN ID  %d",vlanId);
          continue;
        }

        /* VlanId is set in the mask, now check if it is configurable*/
        valid =  dot1qVlanIsConfigurable(vlanId, &vCfg);
        if (valid == L7_TRUE)
        {
          /* A static VLAN exists */
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,"VLAN ID %d already exists",vlanId);
          /* reset the bit in the mask so that we do not access this in process side of the Q*/
          L7_VLAN_CLRMASKBIT_POINTER(vidMask, vlanId);
        }
        else
        {
          if ((dot1qInfo->CurrentVLANCount + 1) > L7_MAX_VLANS)
          {
            L7_VLAN_CLRMASKBIT_POINTER(vidMask, vlanId);
            rc2 = L7_TABLE_IS_FULL;
            continue;
          }
          cfgEntryFound = L7_FALSE;
          /* attempt to allocate an entry */
          if(dot1qVlanConfigEntryGet(vlanId, &vCfg) == L7_TRUE)
          {
            cfgEntryFound = L7_TRUE;
          }

          if (cfgEntryFound == L7_TRUE)
          {

            memset((void *)vCfg, 0, sizeof(dot1qVlanCfgData_t));
            /* Update cfg */
            vCfg->vlanId = vlanId;

            /* Set dataChanged flag */
            dot1qCfg->hdr.dataChanged = L7_TRUE;

            /* Increment current count immediately.
               If subsequent VLAN create requests are
               received before the VLAN create is actually
               processed, the count will err on the side of
               caution.
             */
            dot1qInfo->CurrentVLANCount++;

          }
        }
      }
    }
    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    memcpy(&msg.data.intfVlanMask.vlanMask, vidMask, sizeof(L7_VLAN_MASK_t));
    msg.event = dot1q_vlan_create_static_mask;
    msg.requestor = DOT1Q_ADMIN;

    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc2;
}
/*********************************************************************
* @purpose  Delete a range of vlans
*
* @param    vidMask     Vlan ID Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    The vlan mask need not be contiguous
* @notes    This routine is to be used to delete ADMIN vlans ONLY.
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanDeleteMask(L7_VLAN_MASK_t *vidMask)
{
    DOT1Q_MSG_t msg;
    L7_RC_t rc = L7_SUCCESS;
    L7_uint32 vlanId;
    dot1qVlanCfgData_t *vCfg;
    L7_uint32 prevVlanCount=0;

    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    prevVlanCount = dot1qInfo->CurrentVLANCount;

    /* For each vlan id that is set in the mask check if it is configurable */
    for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
    {
      if (L7_VLAN_ISMASKBITSET_POINTER(vidMask, vlanId))
      {
        /*Default VLAN can not be (should not be) deleted */
        if(vlanId == L7_DOT1Q_DEFAULT_VLAN)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                          "Failed to delete, Default VLAN %d cannot be deleted", L7_DOT1Q_DEFAULT_VLAN);
          L7_VLAN_CLRMASKBIT_POINTER(vidMask, vlanId);
          continue;
        }

        /* Is there a static vlan with this vlan ID ?*/
        if (dot1qVlanIsConfigurable(vlanId, &vCfg) == L7_FALSE)
        {
          /* No, there is no static by this id */
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                          "Failed to delete, VLAN ID %d is not a static VLAN", vlanId);
          L7_VLAN_CLRMASKBIT_POINTER(vidMask, vlanId);
          continue;
        }
        if (dot1qVlanConfigEntryReset(vlanId) != L7_SUCCESS)
        {
          (void)osapiWriteLockGive(dot1qCfgRWLock);
          return L7_FAILURE;
        }

        /* Set dataChanged flag */
        dot1qCfg->hdr.dataChanged = L7_TRUE;
        /* Decrement current count */
        dot1qInfo->CurrentVLANCount--;
      }
    }
     
    /* Check if the previous VLAN count before deletion is equal to
       the current VLAN count. If the count is same then there are
       no VLANs to be deleted */
    if(prevVlanCount == dot1qInfo->CurrentVLANCount)
    {
      DOT1Q_DEBUG_MSG("%s(): No static VLANs to Delete\n\r", __FUNCTION__);
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      return L7_SUCCESS; 
    }
    
    (void)osapiWriteLockGive(dot1qCfgRWLock);

    /* Put a message on the queue */
    memset (&msg, 0x0,sizeof(DOT1Q_MSG_t));

    memcpy(&msg.data.intfVlanMask.vlanMask, vidMask, sizeof(L7_VLAN_MASK_t));
    msg.event = dot1q_vlan_delete_static_mask; 
    msg.requestor = DOT1Q_ADMIN;


    /* Put a message on the queue */
    rc = dot1qIssueCmd(&msg);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
              "Cannot put a message on dot1q msg Queue, Returns:%d",rc);
    }

    return rc;
}

/*  Start of dot1q internal vlan apis */

/*********************************************************************
* @purpose  Set the direction from the base VLAN ID when looking for an
*           unused VLAN ID for internal VLAN assignment
*
* @param    policy  (input)   up or down
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanPolicySet(DOT1Q_INTERNAL_VLAN_POLICY policy)
{

  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  dot1qCfg->Qglobal.internalVlanPolicy = policy;
  (void)osapiWriteLockGive(dot1qCfgRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the direction from the base VLAN ID when
*           assigning  internal vlans
*
* @param    policy  (input)   up or down
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
DOT1Q_INTERNAL_VLAN_POLICY dot1qInternalVlanPolicyGet(void)
{
  return dot1qCfg->Qglobal.internalVlanPolicy;
}

/*********************************************************************
* @purpose  Set the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    baseVlanId  (input)   VLAN ID
*
* @returns  L7_SUCCESS
*           L7_FAILURE if VLAN ID is out of range
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qBaseInternalVlanIdSet(L7_uint32 baseVlanId)
{
  if ((baseVlanId < L7_DOT1Q_MIN_VLAN_ID) || (baseVlanId > L7_DOT1Q_MAX_VLAN_ID))
    return L7_FAILURE;

  (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
  dot1qCfg->Qglobal.baseInternalVlan = baseVlanId;
  (void)osapiWriteLockGive(dot1qCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    void
*
* @returns  VLAN ID
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qBaseInternalVlanIdGet(void)
{
  return dot1qCfg->Qglobal.baseInternalVlan;
}


/*********************************************************************
* @purpose  Obtain an internal VLAN assignment
*
* @param    requestor   @b{(input)} one of vlanRequestor_t
* @param    *descr      @b{(input)} pointer to usage description
* @param    *vid        @b{(output)} pointer to assigned VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Currently requested only by DOT1Q_IP_INTERNAL_VLAN
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanRequest(L7_uint32 requestor,
                                 L7_char8 *descr,
                                 L7_uint32 *vid)
{
    L7_uint32   vlan;
    L7_uint32   i;
    L7_uint32   index;
    L7_RC_t     rc;
    L7_BOOL     entryFound;

    osapiSemaTake(dot1qInternalVlanSem, L7_WAIT_FOREVER);

    vlan = *vid;
    entryFound = L7_FALSE;
    rc = L7_SUCCESS;

    if ( vlan != 0 )
    {
        if (dot1qVlanCreateIsPending(vlan) != L7_TRUE)
        {
            /* Check to see that VLAN is not currently in use.  If it is, will need to try to assign another VLAN */
            if (dot1qVlanCheckValid(vlan) != L7_SUCCESS)
            {
                entryFound = L7_TRUE;
            }
            else
            {
                /* Force an attempt to assign another VLAN */
                vlan = 0;
            }
        }
    }

    if ( vlan == 0 )
    {
        /* Find a VLAN to assign to this physical routing interface */
        vlan = dot1qBaseInternalVlanIdGet();
        while (rc == L7_SUCCESS)
        {
            if (dot1qVlanCreateIsPending(vlan) != L7_TRUE)
            {
                if (dot1qVlanCheckValid(vlan) != L7_SUCCESS)
                {
                    *vid = vlan;
                    entryFound = L7_TRUE;
                    break;
                }
            }

          rc = dot1qInternalVlanNext(vlan, &vlan);
        }
    }

    rc = L7_FAILURE;

    if (entryFound == L7_TRUE)
    {
        /* Find an empty entry */
        index = 0;

        /* Update internal VLAN table */
        for (i = 1; i <= L7_DOT1Q_MAX_INTERNAL_VLANS; i++)
        {
           if (dot1qInternalVlanInfo[i].vlanId == 0)
           {
               index = i;
               break;
           }
        }
        if (index != 0)
        {
            /* Create a vlan */
            if (dot1qVlanCreate(vlan, DOT1Q_IP_INTERNAL_VLAN) == L7_SUCCESS)
            {
                dot1qInternalVlanInfo[index].vlanId = *vid;
                dot1qInternalVlanInfo[index].requestor = requestor;
                osapiStrncpySafe( dot1qInternalVlanInfo[index].description,descr,
                                 L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR);
                rc = L7_SUCCESS;
            }
        }
    }

    osapiSemaGive(dot1qInternalVlanSem);
    return rc;

}


/*********************************************************************
* @purpose  Release an internal VLAN
*
* @param    vid        @b{(input)} assigned VLAN ID
* @param    requestor  @b{(input)}one of vlanRequestor_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Currently requested only by DOT1Q_IP_INTERNAL_VLAN
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanRelease( L7_uint32 vid, L7_uint32 requestor)
{

    L7_uint32   i;

    osapiSemaTake(dot1qInternalVlanSem, L7_WAIT_FOREVER);

    /* Update internal VLAN table */
    for (i = 1; i <= L7_DOT1Q_MAX_INTERNAL_VLANS; i++)
    {
       if (dot1qInternalVlanInfo[i].vlanId == vid)
       {
           if (dot1qInternalVlanInfo[i].requestor == requestor)
           {
               memset((void *)&dot1qInternalVlanInfo[i], 0, sizeof(dot1qInternalVlanInfo_t));
               if (dot1qVlanDelete(vid, requestor) != L7_SUCCESS)
               {
                   osapiSemaGive(dot1qInternalVlanSem);
                   return L7_FAILURE;
               }
               break;
           }
           else
           {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID, "Requestor %d attempted to release internal vlan %d: owned by %d",
                       requestor, vid, dot1qInternalVlanInfo[i].requestor);
               osapiSemaGive(dot1qInternalVlanSem);
               return L7_FAILURE;
           }
       }
    }

    osapiSemaGive(dot1qInternalVlanSem);
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use
*
* @param    vlanId          @b{(output)} pointer to first internal vlan
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanFindFirst(L7_uint32 *vlanId)
{
    L7_uint32 i;
    L7_uint32 lowestVlanId;


    osapiSemaTake(dot1qInternalVlanSem, L7_WAIT_FOREVER);

    /* invalid VLAN ID*/
    lowestVlanId   = 0xFFFF;

    for (i = 1; i <= L7_DOT1Q_MAX_INTERNAL_VLANS; i++)
    {
       if (dot1qInternalVlanInfo[i].vlanId != 0)
       {
           if (dot1qInternalVlanInfo[i].vlanId < lowestVlanId)
           {
               lowestVlanId =  dot1qInternalVlanInfo[i].vlanId;
           }
       }
    }

    if ( lowestVlanId < 0xFFFF)
    {
        /* Entry is found */
        *vlanId     = lowestVlanId;
        osapiSemaGive(dot1qInternalVlanSem);
        return L7_SUCCESS;
    }

    /* Entry is not found */
    *vlanId     = 0;
    osapiSemaGive(dot1qInternalVlanSem);
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use
*
* @param    vid      @b{(input)} previous VLAN ID
* @param    nextvid  @b{(output)} next vlanId
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanFindNext(L7_uint32 vid, L7_uint32 *nextvid)
{

    L7_uint32 i;
    L7_uint32 nextLowestVlanId;
    L7_uint32 vlan;

    osapiSemaTake(dot1qInternalVlanSem, L7_WAIT_FOREVER);

    /* invalid VLAN ID*/
    nextLowestVlanId   = 0xFFFF;

    for (i = 1; i <= L7_DOT1Q_MAX_INTERNAL_VLANS; i++)
    {
       if (dot1qInternalVlanInfo[i].vlanId != 0)
       {
           vlan = dot1qInternalVlanInfo[i].vlanId;
           if  ( (vlan > vid) && (vlan < nextLowestVlanId) )
           {
               nextLowestVlanId = vlan;
           }
       }
    }

    if ( nextLowestVlanId < 0xFFFF)
    {
        /* Entry is found */
        *nextvid     = nextLowestVlanId;
        osapiSemaGive(dot1qInternalVlanSem);
        return L7_SUCCESS;
    }

    /* Entry is not found */
    *nextvid     = 0;
    osapiSemaGive(dot1qInternalVlanSem);
    return L7_FAILURE;

}


/*********************************************************************
* @purpose  Find the usage description for the given internal vlan
*
* @param    vid          @b{(input)} vlan ID
* @param    *descr       @b{(output)} pointer to usage description
*
* @returns  L7_SUCCESS if VLAN ID is found and is internal
* @returns  L7_FAILURE if the VLAN ID is not listed as internal
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanUsageDescrGet(L7_uint32 vid, L7_char8 *descr)
{

    L7_uint32 i;

    osapiSemaTake(dot1qInternalVlanSem, L7_WAIT_FOREVER);

    for (i = 1; i <= L7_DOT1Q_MAX_INTERNAL_VLANS; i++)
    {
       if (dot1qInternalVlanInfo[i].vlanId == vid)
       {
           osapiStrncpySafe( descr, dot1qInternalVlanInfo[i].description,
                            L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR);

           osapiSemaGive(dot1qInternalVlanSem);
           return L7_SUCCESS;
       }
    }

    osapiSemaGive(dot1qInternalVlanSem);
    return L7_FAILURE;

}


/*  End of dot1q internal vlan apis */
