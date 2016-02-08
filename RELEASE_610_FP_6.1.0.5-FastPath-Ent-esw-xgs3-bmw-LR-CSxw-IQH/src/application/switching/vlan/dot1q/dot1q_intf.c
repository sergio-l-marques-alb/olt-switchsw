/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_intf.c
*
* @purpose Interface management for vlans
*
* @component dot1q
*
* @comments none
*
* @create 05/22/2002
*
* @author skanchi
*
* @end
*             
**********************************************************************/

#define L7_MAC_ENET_BCAST
 
#include "flex.h"
#include "dot1q_common.h"
#include "osapi.h"
#include "dtlapi.h"
#include "nimapi.h"
#include "dot1q_intf.h"
#include "l7_packet.h"
#include "cardmgr_api.h"
#include "dot1q_mask.h"
#include "dot1q_data.h"

extern  vlanTree_t          *pVlanTree;
extern  dot1qVlanIntf_t		dot1qVlanIntf[DOT1Q_MAX_VLAN_INTF + 1];

extern  dot1qVidList_t*     pVidList;
extern  dot1qCfgData_t       *dot1qCfg;


static L7_VLAN_MASK_t   vlanIntfInitMask;    /* Configuration Mask */

/* Working Masks for Get Next function */
static L7_VLAN_MASK_t   vlanIntfGetNextMask;
static L7_VLAN_MASK_t   vlanIntfTestGetNextMask;

static L7_RC_t dot1qInternalVlanNext(L7_uint32 current, L7_uint32 *next);


/*********************************************************************
* @purpose  Create all currently configured vlan interfaces
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
void dot1qVlanIntfCreateInterfaces(void)
{
    L7_BOOL    entryFound;
    L7_uint32  vlanId;
    L7_uint32  intIfNum;

    memcpy ( &vlanIntfInitMask,&dot1qCfg->QvlanIntf, sizeof(vlanIntfInitMask) );
                           
    L7_VLAN_FHMASKBIT(vlanIntfInitMask, vlanId);
    L7_VLAN_NONZEROMASK(vlanIntfInitMask, entryFound);

    while (entryFound == L7_TRUE) 
    {

        if (sysNetIsValid8021qVlanId((L7_ushort16) vlanId) == L7_TRUE)
        {
            (void)dot1qVlanIntfCreate( vlanId, 0, &intIfNum);
        }
        else
        {
            entryFound = L7_FALSE;
        }

        L7_VLAN_CLRMASKBIT(vlanIntfInitMask, vlanId);
        L7_VLAN_FHMASKBIT(vlanIntfInitMask, vlanId);

    }

}


/*********************************************************************
* @purpose  Delete all currently configured vlan interfaces
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
void dot1qVlanIntfDeleteInterfaces(void)
{
    L7_BOOL    entryFound;
    L7_uint32  vlanId;

    memcpy ( &vlanIntfInitMask,&dot1qCfg->QvlanIntf, sizeof(vlanIntfInitMask) );

    entryFound = L7_TRUE;
    while (entryFound == L7_TRUE) 
    {

        L7_VLAN_FHMASKBIT(vlanIntfInitMask, vlanId);
        if (sysNetIsValid8021qVlanId((L7_ushort16) vlanId) == L7_TRUE)
        {
            L7_VLAN_CLRMASKBIT(vlanIntfInitMask, vlanId);
            (void)dot1qVlanIntfDelete( vlanId);
        }
        else
        {
            /*  No more valid vlans in mask. Discontinue interface creation */
            entryFound = L7_FALSE;
        }

    }

}

/*********************************************************************
* @purpose  Create an interface for the vlan
*
* @param    vlanId      vlan ID
* @param    intfId      interface ID. If non-zero, used as the index to the
*                       VLAN interface array. If zero, next available element
*                       in array is used.
* @param    *intIfNum   pointer to internal interface number
*
* @returns  L7_SUCCESS - VLAN interface created
*           L7_TABLE_IS_FULL - The maximum number of routing interfaces
*                              has already been created
*           L7_ADDR_INUSE - VLAN ID already in use for port based routing 
*           L7_FAILURE - other failure
*
* @notes    public API
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfCreate(L7_uint32 vlanId, L7_uint32 intfId, L7_uint32 *intIfNum)
{
  L7_uint32 tmpIntIfNum;
  L7_uint32 index;
  L7_uint32 nimCfgIntIfNum;
  nimConfigID_t configId;
  L7_uint32 min, max;

  if (intIfNum == L7_NULLPTR)
      return L7_FAILURE;

  /*Check if it is a valid vlan id*/
  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
	return L7_FAILURE;

  /* Check if we have already created intf for this vlan */
  if (dot1qVlanIntfIndexGet(vlanId, &index) == L7_SUCCESS)
  {
    if (dot1qVlanIntf[index].intfType == DOT1Q_PORT_INTF)
    {
      /* VLAN ID already in use for port based routing interface */
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(dot1qVlanIntf[index].intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1Q_COMPONENT_ID,
              "Failed to create VLAN %u. This VLAN ID is assigned internally to "
              "port-based routing interface %s. To release the VLAN ID, disable "
              "the routing interface.", vlanId, ifName);

      return L7_ADDR_INUSE;
    }
	  *intIfNum = dot1qVlanIntf[index].intIfNum;
    return L7_SUCCESS;
  }
  
  index = 0;
  /*-----------------------------------------*/
  /*  Allocate an entry                      */
  /*-----------------------------------------*/

  /* In circumstances such as a system reset, the VLAN interface should
     have the same intIfNum/slot.port assignment as it had in the previous
     boot. To assign an intIfNum and slot.port, NIM uses a relative port 
     offset provided by the interface owner.  The relative port offset 
     provided by dot1q for VLAN interfaces is the index number of the vlan
     interface entry in its internal tables.  
     
     If the interface exists in NIM's configuration, use that offset as the
     VLAN interface entry. If it does not, use a free entry.
     
     Note that no conflict should ever occur over index numbers because all 
     previously configured VLAN interfaces should be created when the dot1q
     component is initialized.
     
     */
  
  memset((void *)&configId,0,sizeof(nimConfigID_t));
  configId.type = L7_LOGICAL_VLAN_INTF;
  configId.configSpecifier.vlanId = vlanId;
  if ( nimIntIfFromConfigIDGet(&configId, &nimCfgIntIfNum) == L7_SUCCESS)
  {
      /* Find relative port offset (== index entry to be used).
         Remember that nim assigns the relative port offset to be zero-based
         while the index used by dot1q is 1-based */
     (void) nimIntIfNumRangeGet(L7_LOGICAL_VLAN_INTF, &min, &max);
     index = (nimCfgIntIfNum - min) + 1;

     /* Error check - the entry should not be in use */
     if ( (dot1qVlanIntf[index].inUse == L7_TRUE) && 
          (dot1qVlanIntf[index].vlanId != vlanId) )
     {
         LOG_MSG("vlan id %d cannot use index %d - already in use for vlan %d\n",
                 vlanId,index, dot1qVlanIntf[index].vlanId);
         index = 0;
     }
  }

  if (index == 0) 
  {
    if (intfId)
    {
      /* Caller provided the array index. Use it if valid and available. */
      /* Make sure index provided by caller is within range */
      if ((intfId < 0) || (intfId > L7_MAX_NUM_VLAN_INTF))
      {
        return L7_FAILURE;
      }
      /* Make sure index provided by caller isn't already in use */
      if (dot1qVlanIntf[intfId].inUse)
      {
        return L7_ALREADY_CONFIGURED;
      }
      index = intfId;
    }
      /* if Interface doesn't exist, check if we are out of interfaces */
    else if (dot1qVlanIntfNextAvailableIndexGet(&index) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1Q_COMPONENT_ID,
              "VLAN interface table is full. Cannot create interface for VLAN %d", vlanId);
      return L7_TABLE_IS_FULL;
    }
  }

  memset((void *)&dot1qVlanIntf[index], 0, sizeof(dot1qVlanIntf_t));
  dot1qVlanIntf[index].inUse = L7_TRUE;
  dot1qVlanIntf[index].vlanId = vlanId;
  dot1qVlanIntf[index].intfType = DOT1Q_VLAN_INTF;

  /*-----------------------------------------*/
  /*  Valid request - save the configuration */
  /*-----------------------------------------*/

  L7_VLAN_SETMASKBIT(dot1qCfg->QvlanIntf, vlanId);
  dot1qCfg->hdr.dataChanged = L7_TRUE;


  /*-----------------------------------------*/
  /*  Create interface                       */
  /*-----------------------------------------*/

  dot1qVlanIntf[index].intfState    = DOT1Q_VLAN_INTF_CREATE_PENDING;

  if (dot1qVlanIntfCreateInNim(vlanId, &tmpIntIfNum) == L7_SUCCESS)
  {
      dot1qVlanIntf[index].intIfNum     = tmpIntIfNum;
      *intIfNum = tmpIntIfNum;


      /*---------------------------------*/
      /*  Create interface in the driver */
      /*---------------------------------*/
      /*   NOTE: Technically, the driver should be on the NIM callback and create an
           instance as a result */
      if ((dtlIntfLogicalIntfCreate(tmpIntIfNum)) != L7_SUCCESS)
      {
          LOG_MSG("DOT1Q: dtl failed to create an interface for vlan %d\n", vlanId);
          return L7_FAILURE;
      }

  }
  else
  {
    /* Free the entry */
    memset((void *)&dot1qVlanIntf[index], 0, sizeof(dot1qVlanIntf_t));
    
    LOG_MSG("DOT1Q: dtl failed to create an interface for vlan %d\n", vlanId);
    
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Delete an intf for the vlan
*
* @param    vlanId         vlan ID
*
* @returns  L7_SUCCESS, if success
*           L7_NOT_EXIST, if not a VLAN routing interface
*           L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfDelete(L7_uint32 vlanId)
{

  L7_RC_t   rc;
  L7_uint32 index;
  L7_uint32 intIfNum;
  L7_uint32 linkState;
  NIM_HANDLE_t              handle;
  NIM_EVENT_NOTIFY_INFO_t   eventInfo;

  /*Check if it is a valid vlan id*/
  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId)  == L7_FALSE)
    return L7_FAILURE;

  /* Check if we have already deleted/not created  intf for this vlan */
  if (dot1qVlanIntfIndexGet(vlanId, &index) != L7_SUCCESS)
    return L7_NOT_EXIST;

  /* Make sure VLAN ID not used for port based routing */
  if (dot1qVlanIntf[index].intfType != DOT1Q_VLAN_INTF)
    return L7_NOT_EXIST;
  
  dot1qVlanIntf[index].intfState    =  DOT1Q_VLAN_INTF_DELETE_PENDING;
  intIfNum                          = dot1qVlanIntf[index].intIfNum;
  
  /* Prep the NIM event info structure */
  memset((void *)&eventInfo,0,sizeof(eventInfo));
  eventInfo.component   = L7_DOT1Q_COMPONENT_ID;
  eventInfo.intIfNum    = dot1qVlanIntf[index].intIfNum;

           
  /* Send a link down notification, only if the link is currently up */  

  eventInfo.event   = L7_DOWN;
  eventInfo.pCbFunc = L7_NULLPTR;

  /* If vlan intf is already down, then ignore */
  if (nimGetIntfLinkState(intIfNum,&linkState) == L7_SUCCESS)
  {
    if (linkState != L7_DOWN)
    {
        if (nimEventIntfNotify(eventInfo,&handle) != L7_SUCCESS)
        {
            LOG_MSG("link down notification failed for vlan %d, intIfNum %d\n", vlanId, intIfNum);
        }

    }
  }
  
  rc =  dot1qVlanIntfDeleteInNim( intIfNum);


  /*----------------------------------------------------------------------------------------*/
  /*  Valid request - save the configuration for the next reboot, regardless of NIM outcome */
  /*----------------------------------------------------------------------------------------*/

  L7_VLAN_CLRMASKBIT(dot1qCfg->QvlanIntf, vlanId);
  dot1qCfg->hdr.dataChanged = L7_TRUE;


  /*---------*/
  /*  Return */
  /*---------*/
  return rc;

}

/*********************************************************************
* @purpose  Set the "always up" attribute on a VLAN interface
*
* @param    vlanId        vlan ID
* @param    alwaysUp      L7_ENABLE or L7_DISABLE
* @param    componentId   component ID
*
* @returns  L7_SUCCESS  If vlanId is valid
* @returns  L7_FAILURE  Otherwise
*
* @notes    This has the effect of setting the "always up"
*           attribute of the interface and will cause a VLAN routing
*           interface to remain operationally up even when no
*           ports are participating in the VLAN.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfAlwaysUpSet(L7_uint32 vlanId, L7_uint32 alwaysUp,
                               L7_COMPONENT_IDS_t componentId)
{
  L7_uint32 index;
  L7_uint32 linkState;
  L7_uint32 memberStatus;
  L7_BOOL oldStatus, newStatus;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t  handle;
  L7_RC_t rc;

  /*
   * Validate parameters
   */
  if (alwaysUp != L7_ENABLE && alwaysUp != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if (componentId >= L7_LAST_COMPONENT_ID)
  {
    return L7_FAILURE;
  }

  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (dot1qVlanIntfIndexGet(vlanId, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*
   * Set/Clear per-component mask element
   */
  COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, oldStatus);
  if (alwaysUp == L7_ENABLE)
  {
    COMPONENT_SETMASKBIT(dot1qVlanIntf[index].alwaysUpMask, componentId);
  }
  else
  {
    COMPONENT_CLRMASKBIT(dot1qVlanIntf[index].alwaysUpMask, componentId);
  }
  COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, newStatus);

  /*
   * If not first to enable or last to disable, just return success
   */
  if (oldStatus == newStatus)
  {
    return L7_SUCCESS;
  }

  /*
   * If the attribute has changed the link status of the vlan interface,
   * send the appropriate NIM event to signal the link status change
   */
  rc = nimGetIntfLinkState(dot1qVlanIntf[index].intIfNum, &linkState);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (alwaysUp == L7_ENABLE)
  {
    if (linkState == L7_DOWN)
    {
      eventInfo.component = L7_DOT1Q_COMPONENT_ID;
      eventInfo.pCbFunc = L7_NULLPTR;
      eventInfo.intIfNum = dot1qVlanIntf[index].intIfNum;
      eventInfo.event = L7_UP;
      rc = nimEventIntfNotify(eventInfo, &handle);
    }
  }
  else
  {
    if ((linkState == L7_UP) &&
        (dot1qVlanIntfLinkStatus(vlanId, &memberStatus) == L7_SUCCESS) &&
        (memberStatus == L7_DOWN))
    {
      eventInfo.component = L7_DOT1Q_COMPONENT_ID;
      eventInfo.pCbFunc = L7_NULLPTR;
      eventInfo.intIfNum = dot1qVlanIntf[index].intIfNum;
      eventInfo.event = L7_DOWN;
      rc = nimEventIntfNotify(eventInfo, &handle);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the "always up" attribute of a VLAN interface
*
* @param    vlanId      vlan ID
* @param    pAlwaysUp   returns L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  If vlanId is valid
* @returns  L7_FAILURE  Otherwise
*
* @notes    none
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfAlwaysUpGet(L7_uint32 vlanId, L7_uint32 *pAlwaysUp)
{
  L7_uint32 index;
  L7_BOOL alwaysUp;

  /*
   * Validate parameters
   */
  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (dot1qVlanIntfIndexGet(vlanId, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*
   * If at least one component has enabled "always up", return L7_ENABLE
   */
  COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, alwaysUp);
  return (alwaysUp == L7_TRUE) ? L7_ENABLE : L7_DISABLE;
}

/*********************************************************************
* @purpose  Get the instance number associated with a VLAN interface
*
* @param    vlanId			vlan id
* @param    pIndex			Index of this vlan in dot1qVlanIntf
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Returns a relative instance number for the VLAN interface,
*          from 1 to L7_MAX_NUM_VLAN_INTF
*                                                      
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfPortInstanceNumGet(L7_uint32 vlanId, L7_uint32 *pIndex)
{

  return( dot1qVlanIntfIndexGet(vlanId, pIndex));
}


/*********************************************************************
* @purpose  Delete an intf for the vlan in NIM
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfDeleteInNim(L7_uint32 intIfNum)
{

  L7_BOOL   wait_for_transition;
  L7_BOOL   detach;
  NIM_HANDLE_t              handle;
  NIM_EVENT_NOTIFY_INFO_t   eventInfo;
  L7_NIM_QUERY_DATA_t       nimQueryData;

  /* Prep the NIM event info structure */
  memset((void *)&eventInfo,0,sizeof(eventInfo));
  eventInfo.component   = L7_DOT1Q_COMPONENT_ID;
  eventInfo.intIfNum    = intIfNum;
           
        
  /* Send a detach notification only if interface is attached */
  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;
  wait_for_transition   = L7_FALSE;
  detach                = L7_FALSE;

  while (1)
  { 
      if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
      {
        /* should never get here */
        LOG_MSG(" nimIntfQuery failed for intIfNum %d\n", intIfNum);
        return L7_FAILURE;
      }
       
      switch (nimQueryData.data.state) {
      case  L7_INTF_CREATED:
          /* Interface is not attached. No need to detach the interface */
          /* Go straight to delete */
          wait_for_transition   = L7_FALSE;
          detach                = L7_FALSE;
          break;
      case L7_INTF_ATTACHED:
          /* detach the interface before deleting */
          wait_for_transition   = L7_FALSE;
          detach = L7_TRUE;
          break;
      case L7_INTF_ATTACHING:
          wait_for_transition   = L7_TRUE;
          break;
      default:
          wait_for_transition   = L7_FALSE;
          detach                = L7_FALSE;
          break;
      } /* switch */

      if (wait_for_transition == L7_TRUE) 
      {
          osapiSleepMSec(10);
      }
      else
          break;

   }  /* while (1) */

  if (detach == L7_TRUE) 
  {

      /*  Now send a detach notification.  Delete will be completed in the completion callback */
      eventInfo.event   = L7_DETACH;
      eventInfo.pCbFunc = dot1qNimEventCompletionCallback;

      if (nimEventIntfNotify(eventInfo,&handle) != L7_SUCCESS)
      {
          LOG_MSG("detach notification failed for intIfNum %d\n", intIfNum);
          return L7_FAILURE;
      }
  } /* detach == L7_TRUE */
  else
  {

      wait_for_transition   = L7_FALSE;

      while (1)
      { 
          if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
          {
            /* should never get here */
             LOG_MSG(" nimIntfQuery failed for intIfNum %d in delete attempt\n", intIfNum);
            return L7_FAILURE;
          }

          switch (nimQueryData.data.state) {
          case  L7_INTF_CREATED:
              /* Go straight to delete */
              wait_for_transition   = L7_FALSE;
              break;

          case L7_INTF_ATTACHED:
          case L7_INTF_ATTACHING:
              /* Handle the case of the VLAN interface becoming attached
                 while an end user is deleting the interface. This is a valid
                 scenario for a dynamically created VLAN or a VLAN which was not
                 previously existing on the box being configured just prior to
                 a detach notification */
                 
              /*
                 WPJ NOTE:  We need to do some sort of semaphore control in this case
                 to prevent an infinite loop here  - If so, this case moves to the
                 default area */
                
              wait_for_transition   = L7_TRUE;
              break;

          case L7_INTF_DETACHING:
              wait_for_transition   = L7_TRUE;
              break;

          default:
              /* detach the interface before deleting */
              LOG_MSG("Interface %d should not be in state %d\n", 
                      intIfNum, nimQueryData.data.state);
              wait_for_transition   = L7_FALSE;
              return L7_FAILURE;
              break;

          }  /* switch */

          if (wait_for_transition == L7_TRUE) 
          {
              osapiSleepMSec(10);
          }
          else
              break;

    }  /* while (1) */

      
       /* NOTE: Technically, the driver should be on the NIM callback and delete an
          instance as a result */
      /* Delete the interface in the driver now since we are safely detached */
      if ((dtlIntfLogicalIntfDelete(intIfNum)) != L7_SUCCESS)
      {
         LOG_MSG("DOT1Q: dtl failed to delete an interface for intIfNum %d\n",intIfNum);
      }

       /*  Now send a delete notification.  Delete will be completed in the completion callback */
      eventInfo.event   = L7_DELETE;
      eventInfo.pCbFunc = dot1qNimEventCompletionCallback;

      if (nimEventIntfNotify(eventInfo,&handle) != L7_SUCCESS)
      {

          LOG_MSG("delete notification failed for intIfNum %d\n", intIfNum);
          return L7_FAILURE;
      }
  }  /* detach != L7_TRUE */

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Create a NIM instance for the vlan interface
*
* @param    vlanId			vlan id
* @param    pIntIfNum		Pointer to Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    On L7_SUCCESS,  pIntIfNum will have the required information
* @notes    information.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfCreateInNim(L7_uint32 vlanId, L7_uint32 *pIntIfNum)
{
  L7_RC_t                 rc = L7_SUCCESS;
  nimIntfCreateRequest_t  nimRequest;
  nimConfigID_t           intfIdInfo;  
  nimIntfDescr_t          intfDescr;   
  nimIntfConfig_t         defaultCfg;  
  NIM_INTF_CREATE_INFO_t  createInfo;
  NIM_EVENT_NOTIFY_INFO_t  notifyEventInfo;
  nimIntfCreateOutput_t   nimOutput;
  NIM_HANDLE_t            handle;
  L7_uint32 			  vlanIfIndex;

  do
  {
    /*---------------------------------*/
    /*  Create interface with NIM       */
    /*---------------------------------*/

    /* Initialize data structures */

    memset((void *)&intfIdInfo,0,sizeof(nimConfigID_t));
    memset((void *)&createInfo,0,sizeof(NIM_INTF_CREATE_INFO_t));
    memset((void *)&defaultCfg,0,sizeof(nimIntfConfig_t));
    memset((void *)&intfDescr,0,sizeof(nimIntfDescr_t));
    memset((void *)&nimRequest,0,sizeof(nimIntfCreateRequest_t));

    /* set up the config ID */
    intfIdInfo.configSpecifier.vlanId =  vlanId;
    intfIdInfo.type = L7_LOGICAL_VLAN_INTF;


    /* set up creation completion info */
    createInfo.component  = L7_DOT1Q_COMPONENT_ID;
    createInfo.pCbFunc    = dot1qNimEventCompletionCallback; 

    /* setup the default config for the interface */
    memset(defaultCfg.LAAMacAddr.addr, 0, L7_MAC_ADDR_LEN); 
    defaultCfg.addrType   = L7_SYSMAC_BIA;
    memset(defaultCfg.ifAlias, 0, sizeof(defaultCfg.ifAlias)); 
    defaultCfg.nameType   = L7_SYSNAME;
    defaultCfg.ifSpeed    = FD_DOT1Q_DEFAULT_VLAN_INTF_SPEED;
    defaultCfg.negoCapabilities = 0; /* Autonegotiation disabled */
    defaultCfg.adminState = FD_DOT1Q_DEFAULT_VLAN_INTF_ADMIN_STATE;
    defaultCfg.trapState  = FD_DOT1Q_DEFAULT_VLAN_INTF_TRAP_STATE;
    defaultCfg.ipMtu      = FD_DOT1Q_DEFAULT_VLAN_INTF_IP_MTU;
    defaultCfg.encapsType = FD_DOT1Q_DEFAULT_VLAN_INTF_ENCAP_TYPE;         
    defaultCfg.cfgMaxFrameSize = FD_DOT1Q_DEFAULT_VLAN_INTF_CFG_MAX_FRAME_SIZE;

    /* setup the intf characteristics for this interface */
    intfDescr.configurable   = L7_TRUE;
    intfDescr.internal       = L7_FALSE;
    intfDescr.settableParms  = L7_INTF_PARM_ADMINSTATE | L7_INTF_PARM_MTU |
                               L7_INTF_PARM_MACADDR | L7_INTF_PARM_LINKTRAP |
                               L7_INTF_PARM_LOOPBACKMODE |
                               L7_INTF_PARM_MACROPORT | L7_INTF_PARM_ENCAPTYPE;
    memcpy (&intfDescr.bcastMacAddr,  &DOT1Q_VLAN_INTF_BCAST_MAC_ADDR, sizeof(DOT1Q_VLAN_INTF_BCAST_MAC_ADDR));
    intfDescr.frameSize.largestFrameSize = DOT1Q_VLAN_INTF_MAX_FRAME_SIZE_LIMIT;
    intfDescr.ianaType       = L7_IANA_L2_VLAN;
    intfDescr.defaultSpeed   =  FD_DOT1Q_DEFAULT_VLAN_INTF_SPEED;
    intfDescr.phyCapability  =  0;
    intfDescr.connectorType  =  0;
    sprintf ((L7_char8 *)&(intfDescr.ifDescr), "%s %s",
             (L7_char8 *)&(intfDescr.ifDescr),
             IANA_L2_VLAN_DESC);

    sprintf ((L7_char8 *)&(intfDescr.ifName), "%s %d",
             DOT1Q_VLAN_INTF_IFNAME_PREFIX,vlanId);

    memset((void*)&intfDescr.macroPort,0,sizeof(nimMacroPort_t));
    intfDescr.macroPort.macroPort = 0;            /* This port is the macro port  */
    intfDescr.macroPort.macroType = L7_LOGICAL_VLAN_INTF;
    intfDescr.macroPort.macroMtu  = FD_DOT1Q_DEFAULT_VLAN_INTF_IP_MTU;
    intfDescr.macroPort.macroMaxFrame = FD_DOT1Q_DEFAULT_VLAN_INTF_CFG_MAX_FRAME_SIZE;
    intfDescr.macroPort.macroInfo = (void *)vlanId;  /* Overload the data */
    

	if (dot1qVlanIntfIndexGet(vlanId,&vlanIfIndex) != L7_SUCCESS)
	{
		LOG_MSG("Cannot get intfIndex for the vlan %d \n", vlanId);
		break;
	}
    
    if ((rc = cmgrIfaceMacGet(intfIdInfo.type, L7_LOGICAL_UNIT,platSlotVlanSlotNumGet(),vlanIfIndex,
                              intfDescr.macAddr.addr)) != L7_SUCCESS)
    {
      LOG_MSG("DOT1Q: failed to get the MAC address of vlan %d\n",vlanId);
      break; /* goto while (0) */
    }


    /* setup the request block pointers */
    nimRequest.pDefaultCfg  = &defaultCfg;
    nimRequest.pIntfDescr   = &intfDescr;
    nimRequest.pIntfIdInfo  = &intfIdInfo;
    nimRequest.pCreateInfo  = &createInfo;
    
    /* setup the output block  */
    nimOutput.handle      = &handle;
    nimOutput.intIfNum    = pIntIfNum;


    /* Tell the system about the interface */
    if ((rc = nimIntfCreate(&nimRequest,&nimOutput)) != L7_SUCCESS)
    {
      LOG_MSG("DOT1Q: failed to create the interface for vlanId %d\n", vlanId);
      break; /* goto while (0) */
    }

    notifyEventInfo.component  = L7_DOT1Q_COMPONENT_ID;
    notifyEventInfo.pCbFunc    = dot1qNimEventCompletionCallback; 
    notifyEventInfo.event      = L7_CREATE;
    notifyEventInfo.intIfNum   = *pIntIfNum;
    rc = nimEventIntfNotify(notifyEventInfo,&handle);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    /*---------------------------------*/
    /*  Create interface counters       */
    /*---------------------------------*/


    /* FUTUTRE_FUNC: No counters are created for VLAN interfaces */


  } while ( 0 );

  return rc;
}


/*
This define determines the max time (currently 2 seconds) that this
callback will wait for the intf to become valid.  Prior to adding
this, this validity was checked only once which was insufficient
to be sure everything had been initialized
*/

#define DOT1Q_INTF_VALID_MAX_LOOPS 6000

/*********************************************************************
* @purpose  Receive the completion notification from NIM event notifications
*
* @param    retVal  pointer to a NIM_NOTIFY_CB_INFO_t structure
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void dot1qNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t            handle;
  L7_uint32 intIfNum;
  L7_uint32 index;
  L7_uint32 vlanId;
  L7_uint32 linkState;
  L7_BOOL alwaysUp;
  L7_uint32 count = 0;

  if (retVal.response.rc != L7_SUCCESS)
  {
    /* Failed to complete the request */
    LOG_MSG("DOT1Q: notification failed for event(%d), intf(%d), reason(%d)\n",
            retVal.event,retVal.intIfNum,retVal.response.reason);
    return;
  }
  else
  {
    /* successful event notification */
    memset ( (void *)&eventInfo, 0, sizeof(eventInfo));
    eventInfo.component = L7_DOT1Q_COMPONENT_ID;
    eventInfo.intIfNum  = retVal.intIfNum;
    eventInfo.pCbFunc   = dot1qNimEventCompletionCallback;

    /* Complete parameter initialization */

    intIfNum  = retVal.intIfNum;

	/*
	The following IsValid check needs a loop because this callback
	can happen before the necessary elements are initialized, due
	to normal differences in the order threads execute.  Also,
	the "valid" condition of the VLAN interface can be set in
	multiple places, not all of which will be followed by this
	function.  As a result, semaphores cannot be used here instead
	of this loop.

	Currently, this loop will wait for 60 seconds before returning
	a failure.  In the future, this loop should probably last
	forever.
	*/

	while (1)
	{
		count++;
		if (dot1qVlanIntfIntIfNumIsValid(intIfNum, &index) != L7_SUCCESS)
		{
			osapiSleepMSec(10);

			if (count > DOT1Q_INTF_VALID_MAX_LOOPS)
			{
				LOG_MSG("dot1qNimEventCompletionCallback: intIfNum %d not valid\n", intIfNum);
				return;
			}

			continue;
		}
		else
		{
			break;
		}
	}

    vlanId = dot1qVlanIntf[index].vlanId;

    switch (retVal.event)
    {
    case L7_CREATE:
                
        /* Complete the creation */
        if (dot1qVlanIntf[index].intfState != DOT1Q_VLAN_INTF_CREATE_PENDING)
        {
            LOG_MSG("dot1q vlan intIf state is not in CREATE_PENDING for intIfNum %d/vlanid %d\n", 
                    intIfNum, vlanId);
            break;
        }
        
        
        dot1qVlanIntf[index].intfState = DOT1Q_VLAN_INTF_VALID;

        
        /* Send attach notification if vlan exists */
        if (dot1qVlanCheckValid(dot1qVlanIntf[index].vlanId) == L7_SUCCESS) 
        {
			L7_RC_t rc;
            eventInfo.event     = L7_ATTACH;
            rc = nimEventIntfNotify(eventInfo,&handle);
			if (rc == L7_FAILURE)
			{
				LOG_MSG("dot1qNimEventCompletionCallback: intIfNum = %d, nimEventIntfNotify failed\n",
						retVal.intIfNum);
			}
        }
		else
		{
			LOG_MSG("dot1qNimEventCompletionCallback: intIfNum = %d, vlanId %d check invalid\n",
				   retVal.intIfNum, vlanId);
		}
        
        break;

    case L7_ATTACH:

        if (dot1qVlanIntf[index].intfState != DOT1Q_VLAN_INTF_VALID)
        {
            LOG_MSG("dot1q vlan intIf state is not VALID for intIfNum %d/ vlanId %d\n", 
                    intIfNum, vlanId);
            break;
        }

        COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, alwaysUp);
        if (alwaysUp == L7_TRUE)
        {
          eventInfo.event = L7_UP;
        }
        else
        {
          if (dot1qVlanIntfLinkStatus(dot1qVlanIntf[index].vlanId, &linkState)
              != L7_SUCCESS)
          {
            LOG_MSG("dot1q vlan intIf link state cannot be determined "
                    "for intIfNum %d/ vlanId %d\n", 
                    intIfNum, vlanId);
            break;
          }
        
          if (linkState == L7_UP)
            eventInfo.event = L7_UP;
          else
            eventInfo.event = L7_DOWN;
        }

        /* Don't care to be called back on the completion of a link state event */
        eventInfo.pCbFunc   = L7_NULLPTR;
        (void) nimEventIntfNotify(eventInfo,&handle);
        break;
    
    case L7_DETACH:

        /* Only issue a delete if the interface is being deconfigured by the user.
           Do not issue one for a VLAN going away */
        if (dot1qVlanIntf[index].intfState == DOT1Q_VLAN_INTF_DELETE_PENDING)
        {

            /* NOTE:     Technically, the driver should be on the NIM callback and delete an
                         instance as a result */
            /* Delete the interface in the driver */
            if ((dtlIntfLogicalIntfDelete(intIfNum)) != L7_SUCCESS)
            {
                LOG_MSG("DOT1Q: dtl failed to delete an interface for intIfNum %d\vlan %d\n", 
                         intIfNum, dot1qVlanIntf[index].vlanId);
            }

            /* Notify NIM */
            eventInfo.event     = L7_DELETE;
            (void) nimEventIntfNotify(eventInfo,&handle); 
        }

        break;

      case L7_DELETE:

        memset((void *)&(dot1qVlanIntf[index]),0,sizeof(dot1qVlanIntf_t));
 	    dot1qVlanIntf[index].intfState =  DOT1Q_VLAN_INTF_INVALID;

        break;

      default:
        break;
    }
  }
}


/*********************************************************************
* @purpose  Process the vlan change
*
* @param    vlanId			vlan id
* @param    intIfNum		Internal Interface Number of the port
* @param    event			change event
* @param    qSemTaken	    dot1qSem status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfVlanChange(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
								L7_uint32 event)
{
  L7_uint32     vlanIntIfNum;
  L7_uint32     linkState, linkStatus;
  L7_uint32     index;
  L7_RC_t       rc;
  L7_BOOL       already_down;
  L7_BOOL       alwaysUp;
  NIM_HANDLE_t  handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  L7_uint32     vlanId;
  L7_uint32     i;
  L7_uint32     numVlans = 0;
  L7_uint32     intfLinkState = L7_DOWN;

  if (intIfNum != 0)
  {
    rc = nimGetIntfLinkState(intIfNum,&intfLinkState);
    if (rc == L7_SUCCESS && intfLinkState != L7_UP && event == VLAN_ADD_PORT_NOTIFY)
    {
      return L7_SUCCESS;
    }
  }
 
  for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++) 
  {
      if (vlanData->numVlans == 1) 
      {
          vlanId = vlanData->data.vlanId;
          /* For any continue, we will break out */
          i = L7_VLAN_MAX_MASK_BIT + 1;
      }
      else
      {
          if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i)) 
          {
              vlanId = i;
          }
          else
          {
              if (numVlans == vlanData->numVlans) 
              {
                  /* Already taken care of all the bits in the mask so break out of for loop */
                  break;
              }
              else
              {
                  /* Vlan is not set check for the next bit since there are more bits that are set*/
                  continue;
              }
          }
      }

      /* If this is not a vlan interface, return */
      if (dot1qVlanIntfVlanIdToIntIfNum(vlanId,&vlanIntIfNum) != L7_SUCCESS)
      {
          numVlans++;
          continue;
      }
    
      /* If the interface is not currently in a valid state, return */
      if (dot1qVlanIntfIntIfNumIsValid(vlanIntIfNum,  &index) != L7_SUCCESS)
      {
          numVlans++;
          continue;
      }
    
      if (dot1qVlanIntf[index].intfState != DOT1Q_VLAN_INTF_VALID)
      {
          numVlans++;
          continue;
      }
     
    
      eventInfo.component     = L7_DOT1Q_COMPONENT_ID;
      eventInfo.pCbFunc       = L7_NULLPTR;
      eventInfo.intIfNum      = vlanIntIfNum;
    
      switch (event)
      {
        case VLAN_ADD_NOTIFY:
    
          eventInfo.pCbFunc   = dot1qNimEventCompletionCallback;
          eventInfo.event     = L7_ATTACH;
          rc = nimEventIntfNotify(eventInfo,&handle);
          break;
    
        case VLAN_DELETE_NOTIFY:
            
            /*------------------------------*/
            /* First send down notification */
            /*------------------------------*/
          already_down = L7_FALSE;
    
            /* If vlan intf is already down, then ignore */
          if (nimGetIntfLinkState(vlanIntIfNum,&linkState) == L7_SUCCESS)
          {
            if (linkState == L7_DOWN)
                already_down = L7_TRUE;
          }
    
          /* If vlan intf is up, then make it down, if vlan doesn't exists or none of
             the ports in the vlan are in forwarding.
          */
          if (already_down == L7_FALSE) 
          {
    
              if (dot1qVlanIntfLinkStatus(vlanId, &linkStatus) != L7_SUCCESS)
                break;
    
              COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, alwaysUp);
              if (linkStatus == L7_DOWN || alwaysUp == L7_TRUE)
               {
                 eventInfo.event = L7_DOWN;
                 rc = nimEventIntfNotify(eventInfo,&handle);
               }
          }
    
          /*------------------------------*/
          /* Now send detach notification */
          /*------------------------------*/
          dot1qVlanIntf[index].intfState = DOT1Q_VLAN_INTF_DELETE_PENDING;
    
          eventInfo.pCbFunc   = dot1qNimEventCompletionCallback;
          eventInfo.event     = L7_DETACH;
          rc = nimEventIntfNotify(eventInfo,&handle);
    
          break;
    
        case VLAN_ADD_PORT_NOTIFY:
          if (intfLinkState != L7_UP)
            break;
          
          /* If vlan intf is already up, then ignore */
          if (nimGetIntfLinkState(vlanIntIfNum,&linkState) == L7_SUCCESS)
          {
            if (linkState == L7_UP)
              break;
          }
          else
            break;
    
          /* If vlan intf is down, then make it up if vlan exists and the
             added port has link up.
          */
          if (dot1qVlanIntfLinkStatus(vlanId, &linkStatus) != L7_SUCCESS)
            break;
    
          if (linkStatus == L7_UP)
          {
            eventInfo.event = L7_UP;
            rc = nimEventIntfNotify(eventInfo,&handle);
          }
          break;
    
        case VLAN_DELETE_PORT_NOTIFY:
    
          /* If vlan intf is already down, then ignore */
          if (nimGetIntfLinkState(vlanIntIfNum,&linkState) == L7_SUCCESS)
          {
            if (linkState == L7_DOWN)
              break;
          }
          else
            break;
    
          /* Do not transition to L7_DOWN if set to be "always up" */
          COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, alwaysUp);
          if (alwaysUp == L7_TRUE)
            break;
    
          /* If vlan intf is up, then make it down, if vlan doesn't exists or none of
             the ports in the vlan are in forwarding.
          */
          if (dot1qVlanIntfLinkStatus(vlanId, &linkStatus) != L7_SUCCESS)
            break;
    
          if (linkStatus == L7_DOWN)
          {
            eventInfo.event = L7_DOWN;
            rc = nimEventIntfNotify(eventInfo,&handle);
          }	  
        break;
    
        default:
          break;
      }
      numVlans++;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process the intf change
*
* @param    intIfNum		Internal Interface Number of the port
* @param    event			change event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIntfChange(L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32 numOfVlans = 0;
  L7_uint32 index;
  L7_uint32 type;
  L7_INTF_TYPES_t intfType;
  L7_uint32 linkState, i;
  L7_BOOL alwaysUp;
  NIM_HANDLE_t           handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

  eventInfo.component     = L7_DOT1Q_COMPONENT_ID;
  eventInfo.pCbFunc       = L7_NULLPTR; 

  /* Check if the vlan intf corresponding to this intf is down */
  if (nimGetIntfType(intIfNum,&intfType) != L7_SUCCESS)
    return L7_SUCCESS;

  if (dot1qIntfIsValidType(intfType) != L7_TRUE)
    return L7_SUCCESS;

  if (dot1qVIDListGet(intIfNum,1,(L7_uint32 *)pVidList,&numOfVlans) != L7_SUCCESS)
    return L7_SUCCESS;

  for (i = 0; i < numOfVlans; i++)
  {
    if (dot1qVlanStatusGet(pVidList[i].vid,&type) != L7_SUCCESS)
      continue;
    if (dot1qVlanIntfIndexGet(pVidList[i].vid,&index) != L7_SUCCESS)
      continue;
    if (nimGetIntfLinkState(dot1qVlanIntf[index].intIfNum,&linkState) != L7_SUCCESS)
      continue;
    
    if (event == L7_ACTIVE)
    {
      if (linkState == L7_UP)
        continue;
      eventInfo.intIfNum = dot1qVlanIntf[index].intIfNum;
      eventInfo.event = L7_UP;
      (void)nimEventIntfNotify(eventInfo,&handle);
    }
    else
    {
      if (linkState == L7_DOWN)
        continue;
      COMPONENT_NONZEROMASK(dot1qVlanIntf[index].alwaysUpMask, alwaysUp);
      if (alwaysUp == L7_TRUE)
        continue;
      if (dot1qVlanIntfLinkStatus(pVidList[i].vid,&linkState) != L7_SUCCESS)
        continue;
      if (linkState == L7_DOWN)
      {
        eventInfo.intIfNum = dot1qVlanIntf[index].intIfNum;
        eventInfo.event = L7_DOWN;
        (void) nimEventIntfNotify(eventInfo,&handle);
      }

    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To determine the status of the vlan intf based on the status
*           of the link of the member ports.
*
* @param    vlanId		vlan id 
* @param    linkStatus	Status of the link
* @parm		qSemTaken   dot1qSem semaphore status
*
* @returns  L7_SUCCESS	Successful completion of the function
* @returns  L7_FAILURE	Some error has occured
*
*                                                   
* @notes	Does not attempt to take and give the dot1qSem semaphore
*           if the qSemTaken is TRUE and helps prevent deadlocks
*********************************************************************/
L7_RC_t dot1qVlanIntfLinkStatus(L7_uint32 vlanId, L7_uint32 *linkStatus)
{
  L7_RC_t rc;
  dot1q_vlan_dataEntry_t currentDataEntry;
  L7_uint32 linkState;
  L7_uint32 i;
  NIM_INTF_MASK_t currentEgressPorts;

  bzero((L7_uchar8*)&currentEgressPorts, sizeof(NIM_INTF_MASK_t));
  
  linkState = L7_INACTIVE;

  if (vlanDataGet(pVlanTree, vlanId, &currentDataEntry) == L7_FALSE)
  {
    if (linkStatus != L7_NULLPTR)
      *linkStatus = L7_DOWN;

    return L7_SUCCESS;
  }

  memcpy((char *)&currentEgressPorts, 
         (char *)&(currentDataEntry.current.currentEgressPorts), 
         sizeof(NIM_INTF_MASK_t));


  rc = nimFirstValidIntfNumber(&i);
  while (rc == L7_SUCCESS)
  {
    if (nimCheckIfNumber(i) != L7_SUCCESS) 
        continue;
      
    if (NIM_INTF_ISMASKBITSET(currentEgressPorts,i))
    {
      if (nimGetIntfActiveState(i,&linkState) == L7_SUCCESS)
      {
        if (linkState == L7_ACTIVE)
          break;
      }
    }
    
    rc = nimNextValidIntfNumber(i, &i);
  }

  if (linkStatus != L7_NULLPTR)
  {
    if (linkState == L7_ACTIVE)
    {
      *linkStatus = L7_UP;
    }
    else
    {
      *linkStatus = L7_DOWN;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the index of the vlan interface
*
* @param    vlanId			vlan id
* @param    pIndex			Index of this vlan in dot1qVlanIntf
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Interface could be either VLAN routing or port based
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIndexGet(L7_uint32 vlanId, L7_uint32 *pIndex)
{
  L7_uint32 i;

  for (i = 1; (L7_int32)i <= DOT1Q_MAX_VLAN_INTF; i++)
  {
    if (dot1qVlanIntf[i].inUse != L7_TRUE)
      continue;

	if (dot1qVlanIntf[i].vlanId == vlanId)
	{
	  if (pIndex != L7_NULLPTR)
	    *pIndex = i;
	  return L7_SUCCESS;
	}
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next available vlan storage index
*
* @param    pIndex			Index value of the next available storage
*							in vlanIntfData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfNextAvailableIndexGet(L7_uint32 *pIndex)
{
  L7_uint32 i;
                
  for (i = 1; i <= DOT1Q_MAX_VLAN_INTF; i++)
  {
    if (dot1qVlanIntf[i].inUse == L7_TRUE)
        continue;

	if (dot1qVlanIntf[i].vlanId == 0)
	{
	  if (pIndex != L7_NULLPTR)
	    *pIndex = i;
	  return L7_SUCCESS;
	}
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Checks if the interface number for a vlan is valid
*
* @param    intIfNum		internal interface number
* @param    pIndex			pointer to the index.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    VLAN routing interfaces only.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIntIfNumIsValid(L7_uint32 intIfNum, L7_uint32 *pIndex)
{
  L7_uint32 i;

  for (i = 1; (int)i <= DOT1Q_MAX_VLAN_INTF; i++)
  {
    if (dot1qVlanIntf[i].inUse != L7_TRUE)
	{
		continue;
	}

	if ((dot1qVlanIntf[i].intIfNum == intIfNum) &&
      (dot1qVlanIntf[i].intfType == DOT1Q_VLAN_INTF))
	{
	  if (pIndex != L7_NULLPTR)
	  {
		*pIndex = i;
	  }
	  return L7_SUCCESS;
	}
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the interface number corresponding to this vlan
*
* @param    vlanid			id of the vlan
* @param    intIfNum		Internal interface number of this vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if VLAN ID does not correspond to a VLAN routing interface
*
* @notes    Cannot be used to find the intIfNum for a VLAN ID used for
*           a port based routing interface.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfVlanIdToIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum)
{
  L7_uint32 i;

  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
    return L7_FAILURE;
  for (i=1; (int)i <= DOT1Q_MAX_VLAN_INTF; i++)
  {
    if (dot1qVlanIntf[i].inUse != L7_TRUE)
      continue;

    if ((dot1qVlanIntf[i].vlanId == vlanId) &&
        (dot1qVlanIntf[i].intfType == DOT1Q_VLAN_INTF))
    {
      if (intIfNum != L7_NULLPTR)
        *intIfNum = dot1qVlanIntf[i].intIfNum;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the interface ID for a given VLAN
*
* @param    vlanid		VLAN ID
* @param    intfId		Interface ID (not internal interface number)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The interface ID is an internally assigned integer in the
*           range [1, L7_MAX_NUM_VLAN_INTF]. The user may optionally 
*           specify the interface ID. The interface ID influences the
*           u/s/p and internal interface number assigned to the VLAN
*           interface. It is listed in the text configuration in order
*           to retain the internal interface number and u/s/p across
*           reboots.  
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIdGet(L7_uint32 vlanId, L7_uint32 *intfId)
{
  return dot1qVlanIntfIndexGet(vlanId, intfId);
}

/*********************************************************************
* @purpose  Get the interface number corresponding to this internally-assigned
*           VLAN ID
*
* @param    vlanid			id of the vlan
* @param    intIfNum		Internal interface number of port-based routing interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if VLAN ID does not correspond to a port-based routing interface
*
* @notes    Cannot be used to find the intIfNum for a VLAN ID used for
*           a VLAN routing interface.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanIdToIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum)
{
  L7_uint32 i;

  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
    return L7_FAILURE;

  for (i=1; (int)i <= DOT1Q_MAX_VLAN_INTF; i++)
  {
    if (dot1qVlanIntf[i].inUse != L7_TRUE)
      continue;

    if ((dot1qVlanIntf[i].vlanId == vlanId) &&
        (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF))
    {
      if (intIfNum != L7_NULLPTR)
        *intIfNum = dot1qVlanIntf[i].intIfNum;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next vlan after this vlan which is configured to be an interface
*
* @param    vlanid          id of the vlan
* @param    nextVlanId      pointer to id of next vlan
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
L7_RC_t dot1qVlanIntfNextVlanIntfGetNext(L7_uint32 vlanId, L7_uint32 *nextVlanId)
{

    L7_uint32  next_vid;

    *nextVlanId = 0;

    /* Get vlan interface configuration */
    memcpy ( &vlanIntfGetNextMask,&dot1qCfg->QvlanIntf, sizeof(vlanIntfGetNextMask) );
    memcpy ( &vlanIntfTestGetNextMask,&dot1qCfg->QvlanIntf, sizeof(vlanIntfTestGetNextMask) );

    /* Shift left all bits in the mask by the value of vlan id. 
       The zeroeth bit in the mask corresponds to vlanID 1 */
    L7_VLAN_FILLLEFTMASKBITS(vlanIntfTestGetNextMask, (vlanId));    

    /* AND that value with the value of the configured masks */
    L7_VLAN_MASKANDEQ(vlanIntfGetNextMask, vlanIntfTestGetNextMask);

     /*  Find the next vlan interface */
    L7_VLAN_FLMASKBIT(vlanIntfGetNextMask, next_vid);             

    if (sysNetIsValid8021qVlanId((L7_ushort16) next_vid) == L7_TRUE)
    {
        *nextVlanId = next_vid;
        return L7_SUCCESS;
    }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Set the direction from the base VLAN ID when looking for an
*           unused VLAN ID to assign to a VLAN routing interface. 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanPolicySet(e_Internal_Vlan_Policy policy)
{
  dot1qCfg->Qglobal.internalVlanPolicy = policy;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the direction from the base VLAN ID when looking for an
*           unused VLAN ID to assign to a VLAN routing interface. 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
e_Internal_Vlan_Policy dot1qInternalVlanPolicyGet(void)
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

  dot1qCfg->Qglobal.baseInternalVlan = baseVlanId;
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
* @purpose  Given a VLAN ID, get the next VLAN ID that should be considered
*           for assignment to a VLAN routing interface.
*
* @param    current        current VLAN ID
* @param    next           next VLAN ID
*
* @returns  L7_SUCCESS 
*           L7_NOT_EXIST if next VLAN ID is the base VLAN ID
*           L7_ERROR if current is out of range
*
* @notes    wraps when the min or max is reached
*                                                   
* @end
*********************************************************************/
static L7_RC_t dot1qInternalVlanNext(L7_uint32 current, L7_uint32 *next)
{
  e_Internal_Vlan_Policy policy = dot1qInternalVlanPolicyGet();
  L7_uint32 vid;

  if ((current < L7_DOT1Q_MIN_VLAN_ID) || (current > L7_DOT1Q_MAX_VLAN_ID))
    return L7_ERROR;

  if (policy == DOT1Q_INT_VLAN_POLICY_ASCENDING)
  {
    if (current < L7_DOT1Q_MAX_VLAN_ID)
      vid = current + 1;
    else
      vid = L7_DOT1Q_MIN_VLAN_ID;
  }
  else
  {
    if (current > L7_DOT1Q_MIN_VLAN_ID)
      vid = current - 1;
    else
      vid = L7_DOT1Q_MAX_VLAN_ID;
  }
  /* Check if we've wrapped all the way back to the start */
  if (vid == dot1qBaseInternalVlanIdGet())
    return L7_NOT_EXIST;

  *next = vid;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Assign a VLAN ID for internal use on a port based routing interface.
*
* @param    intIfNum        internal interface number of port based routing interface
* @param    vlanId          vlanId assigned
*
* @returns  L7_SUCCESS if VLAN ID assigned
* @returns  L7_FAILURE if no VLAN IDs available 
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanAssign(L7_uint32 intIfNum, L7_uint32 *vlanId)
{
  L7_uint32 v;
  L7_uint32 index;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];


  /* First see if we have already created the maximum number of routing 
   * interfaces. */
  if (dot1qVlanIntfNextAvailableIndexGet(&index) != L7_SUCCESS)
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT1Q_COMPONENT_ID,
            "Unable to assign an internal VLAN ID to interface %s. "
            "The maxinum number of routing interfaces have already been created.",
            ifName);
    return L7_FAILURE;
  }

  v = dot1qBaseInternalVlanIdGet();
  
  while (rc == L7_SUCCESS)
  {
    if ((dot1qVlanCheckValid(v) != L7_SUCCESS) &&
        (dot1qVlanIntfIndexGet(v, &i) != L7_SUCCESS))
    {
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
              "Assigning VLAN ID %u to port based routing interface %s.",
              v, ifName);
      dot1qVlanIntf[index].inUse = L7_TRUE;
      dot1qVlanIntf[index].intIfNum = intIfNum;
      dot1qVlanIntf[index].vlanId = v;
      dot1qVlanIntf[index].intfType = DOT1Q_PORT_INTF;
      dot1qVlanIntf[index].intfState = DOT1Q_VLAN_INTF_VALID;
      if (vlanId)
      {
        *vlanId = v;
      }
      return L7_SUCCESS;
    }
    rc = dot1qInternalVlanNext(v, &v);
  }

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DOT1Q_COMPONENT_ID,
          "Unable to assign an internal VLAN ID to interface %s. "
          "All VLAN IDs are already in use.", ifName);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Release a VLAN ID used for a port based routing interface.
*
* @param    intIfNum        internal interface number of port based routing interface
*
* @returns  L7_SUCCESS if VLAN ID released
* @returns  L7_FAILURE                          
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanRelease(L7_uint32 intIfNum)
{
  L7_uint32 i;

  for (i = 0; i < DOT1Q_MAX_VLAN_INTF; i++)
  {
    if ((dot1qVlanIntf[i].inUse) &&
        (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF) &&
        (dot1qVlanIntf[i].intIfNum == intIfNum))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
              "Releasing VLAN ID %u from port based routing interface %s.",
              dot1qVlanIntf[i].vlanId, ifName);
      memset(&dot1qVlanIntf[i], 0, sizeof(dot1qVlanIntf_t));
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine whether a given VLAN is being used internally for 
*           a port based routing interface.
*
* @param    vlanId		VLAN ID
*
* @returns  L7_TRUE if VLAN ID is used internally
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_BOOL dot1qVlanIdInternal(L7_uint32 vlanId)
{
  L7_uint32 i;

  for (i = 0; i < DOT1Q_MAX_VLAN_INTF; i++)
  {
    if ((dot1qVlanIntf[i].inUse) &&
        (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF) &&
        (dot1qVlanIntf[i].vlanId == vlanId))
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the internal VLAN assigned to a port based routing interface
*
* @param    intIfNum		internal interface number of port based routing interface
*
* @returns  non-zero if VLAN ID found
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_uint32 dot1qInternalVlanIdGet(L7_uint32 intIfNum)
{
  L7_uint32 i;

  for (i = 0; i < DOT1Q_MAX_VLAN_INTF; i++)
  {
    if ((dot1qVlanIntf[i].inUse) &&
        (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF) &&
        (dot1qVlanIntf[i].intIfNum == intIfNum))
    {
      return dot1qVlanIntf[i].vlanId;
    }
  }
  return 0;
}

/*********************************************************************
* @purpose  Get the Routing Interface for the given assigned VLANID
*
* @param    vlanId VLAN ID
*
* @returns  non-zero if Routing interface is found.
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_uint32 dot1qInternalVlanRoutingInterfaceGet(L7_uint32 vlanId)
{
  L7_uint32 i;

  for (i = 0; i < DOT1Q_MAX_VLAN_INTF; i++)
  {
    if ((dot1qVlanIntf[i].inUse) &&
        (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF) &&
        (dot1qVlanIntf[i].vlanId == vlanId))
    {
      return dot1qVlanIntf[i].intIfNum;
    }
  }
  return 0;
}

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use.
*
* @param    vlanId          vlanId 
* @param    intIfNum        corresponding interface
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally                         
*
* @notes    Not very efficient.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
  L7_uint32 v, i;

  for (v = 1; v <= L7_DOT1Q_MAX_VLAN_ID; v++)
  {
    for (i = 0; i < DOT1Q_MAX_VLAN_INTF; i++)
    {
      if ((dot1qVlanIntf[i].inUse) &&
          (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF) &&
          (dot1qVlanIntf[i].vlanId == v))
      {
        *vlanId = v;
        *intIfNum = dot1qVlanIntf[i].intIfNum;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use.
*
* @param    prevVid      previous VLAN ID
* @param    vlanId       next vlanId 
* @param    intIfNum     interface corresponding to next VLAN ID
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally                         
*
* @notes    Not very efficient.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
  L7_uint32 v, i;

  for (v = prevVid + 1; v <= L7_DOT1Q_MAX_VLAN_ID; v++)
  {
    for (i = 0; i < DOT1Q_MAX_VLAN_INTF; i++)
    {
      if ((dot1qVlanIntf[i].inUse) &&
          (dot1qVlanIntf[i].intfType == DOT1Q_PORT_INTF) &&
          (dot1qVlanIntf[i].vlanId == v))
      {
        *vlanId = v;
        *intIfNum = dot1qVlanIntf[i].intIfNum;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

/* Temporary debug routine to print internal VLAN use */
void dot1qShowVlanInternalUsage(void)
{
  L7_uint32 vlanId;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  printf("\n%6s %10s", "VLAN ID", "Interface");

  if (dot1qInternalVlanFindFirst(&vlanId, &intIfNum) != L7_SUCCESS)
    return;

  while (rc == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    printf("\n%6u %10s", vlanId, ifName);
    rc = dot1qInternalVlanFindNext(vlanId, &vlanId, &intIfNum);
  }
  printf("\n");
}

/* START DOT1Q MASKS */


/*********************************************************************
* @purpose  Set bits left of k with ones, bit k and below with zeroes
*
* @param    *j              pointer to bit mask
* @param    k               bit index
*
* @returns  void
*
* @notes      
*                                                   
* @end
*********************************************************************/
void dot1qVlanMaskFillLeftMostBits(L7_VLAN_MASK_t *j, L7_int32 k)
{
  
  L7_int32     whole_bytes_shifted;                                                             
  L7_int32     bits_in_byte_shifted;                                                             
  L7_int32     x;                                                             
                                                    
                                                                               
  /* Bit index 1 occupies the zeroeth bit of the mask when setting the mask, 
     so gracefully allow for a value of k= 0 */
  
  whole_bytes_shifted   = 0;
  bits_in_byte_shifted  = 0;


  if (k > 0) 
  {

      whole_bytes_shifted = (k-1)/(8*sizeof(L7_uchar8));                          
      bits_in_byte_shifted = ((k-1) % (8*sizeof(L7_uchar8)));  

      /* The above yields a zero-based result.  To accomodate shift, add 1 */
      bits_in_byte_shifted++;
  }
                                                                               
  /* Shift bits in upper indices */                                            
  for (x = (L7_VLAN_INDICES - 1); x > whole_bytes_shifted; x--) {            
           /* Shift the higher index */                                        
           j->value[x] = 0xFF;                                        
  };                                                                           
                                                                               
  /* now fill and shift the bits in the whole byte belonging to bit k */                
  j->value[whole_bytes_shifted] = 0xFF;                   
  j->value[whole_bytes_shifted] = (j->value[whole_bytes_shifted] << bits_in_byte_shifted);                   
                                                                               
                                                                               
  /* Lastly, zero the indices below the whole byte belonging to bit k */       
  for (x = (whole_bytes_shifted - 1); x >= 0; x--) {                           
           j->value[x] = 0;                                                     
  };                                                                           
                                                                               
                                                                               
}


/*********************************************************************
* @purpose  Shift bits in mask j left by k bits
*
* @param    *j              pointer to bit mask
* @param    k               bit index
*
* @returns  void
*
* @notes       Shift bits in mask j left by k bits
*                                                   
* @end
*********************************************************************/
void dot1qVlanMaskShiftLeftBits(L7_VLAN_MASK_t *j, L7_int32 k)
{
  
  L7_int32     whole_bytes_shifted;                                                             
  L7_int32     bits_in_byte_shifted;                                                             
  L7_int32      x;                                                             
  L7_uint32     _temp_;                                                             
                                                                               
                                                    
                                                                               
  /* Bit index 1 occupies the zeroeth bit of the mask when setting the mask, 
     so gracefully allow for a value of k= 0 */
  
  whole_bytes_shifted   = 0;
  bits_in_byte_shifted  = 0;


  if (k > 0) 
  {

      whole_bytes_shifted = (k-1)/(8*sizeof(L7_uchar8));                          
      bits_in_byte_shifted = ((k-1) % (8*sizeof(L7_uchar8)));  

      /* The above yields a zero-based result.  To accomodate shift, add 1 */
      bits_in_byte_shifted++;
  }
                                                                               
                                                                               
  /* Shift bits in upper indices */                                            
  for (x = (L7_VLAN_INDICES - 1); x > whole_bytes_shifted; x--) {            
           /* Save the bits in the lower index which must be shifted  */          
           /*   into the higher index. */                                      
           _temp_ = (j->value[x-1] >> ( 8-bits_in_byte_shifted));                
                                                                               
           /* Shift the higher index */                                        
           j->value[x] = (j->value[x] << bits_in_byte_shifted);                                        
           j->value[x] |= _temp_;                                                 
  };                                                                           
                                                                               
  /* now shift the bits in the whole byte belonging to bit k */                
                                                                               
  j->value[whole_bytes_shifted] = (j->value[whole_bytes_shifted] << bits_in_byte_shifted);                   
                                                                               
                                                                               
  /* Lastly, zero the indices below the whole byte belonging to bit k */       
  for (x = (whole_bytes_shifted - 1); x >= 0; x--) {                           
           j->value[x] = 0;                                                     
  };                                                                           
                                                                               
                                                                               
}


/*********************************************************************
* @purpose  Find the least significant bit turned on in mask j
*
* @param    *j              pointer to bit mask
* @param    *k              pointer to data area to hold LSB
*
* @returns  void
*
* @notes  FLMASKBIT finds the index of the least-significant bit 
*         turned-on in mask j and returns that index in k. 
*   
*         if no bit is set, k returns an invalid value. That is, 
*         k is greater than the maximum number of bits in the mask
*         
*         k is pne-based.  That is, the lowest possible value returned
*         is one, not zero.                                          
*                                                   
* @end
*********************************************************************/
void dot1qVlanMaskFLMaskBit(L7_VLAN_MASK_t *j, L7_uint32 *k)
{

      L7_uint32 x;                                                 
      L7_uint32 m = 0; 
                                                                     
      for (x = 0; x <= (L7_VLAN_INDICES - 1);  x++) {             
                if ( j->value[x] )                                    
                        break;                                       
        };        

        if (x <= (L7_VLAN_INDICES-1)) {                                                
                L7_uint32 i;                                         
                for (i=0; i <= 7; i++) {                             
                    if ( j->value[x] & (1 << i)) {                    
                       m = i;
                       break;                                        
                    }                                                
                }                                                    
        } else {    
                /* Set k to an invalid value */
                m = L7_VLAN_MAX_MASK_BIT+1;                                            
        };  
        /* Calculate the bit offset, remembering that each index is a byte */
        /* Ensure a 1-based value is returned */
        m = (m + (x*(8)) + 1 );          

        *k = m;
}




/*********************************************************************
* @purpose  Find the most significant bit turned on in mask j
*
* @param    *j              pointer to bit mask
* @param    *k              pointer to data area to hold MSB
*
* @returns  void
*
* @notes  FHMASKBIT finds the index of the most-significant bit 
*         turned-on in mask j and returns that index in k. 
*   
*         if no bit is set, k returns an invalid value. That is, 
*         k is greater than the maximum number of bits in the mask
*
*         k is pne-based.  That is, the lowest possible value returned
*         is one, not zero.                                          
*                                                   
* @end
*********************************************************************/
void dot1qVlanMaskFHMaskBit(L7_VLAN_MASK_t *j, L7_uint32 *k)
{
        L7_int32 x;   
        L7_uint32 m = 0; 
                                                                    
        for (x = (L7_VLAN_INDICES - 1); x >= 0; x--) {           
                if ( j->value[x] )                                   
                        break;                                      
        };     

        if (x < 0)
        {
          /* Set k to an invalid value */
          *k = L7_VLAN_MAX_MASK_BIT+1;
          return;
        }

        if (x >= 0) {                                               
/* This is for i960
      asm volatile ("scanbit %1,%0" : "=d"(k) : "d"(j.value[x])); */
/* This is not */                                                   
                L7_int32 i;                                         
                for (i = 7; i >= 0; i--) {                          
                    if ( j->value[x] & (1 << i)) {                   
                       m = i;                                       
                       break;                                       
                    }                                               
                }                                                   
/* End non-i960 */                                                  
        } else {                                                    
            /* Set k to an invalid value */
            m = L7_VLAN_MAX_MASK_BIT+1;                                            
        };                                                          
        /* Calculate the bit offset, remembering that each index is a byte */
        /* Ensure a 1-based value is returned */
        m = (m + (x*(8)) + 1 );          
        *k = m;
}




/*  END DOT1Q MASKS */






