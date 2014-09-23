/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_intf_map_api.c
*
* @purpose    Interface mapping API for all other components
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nim_data.h"
#include "nim_util.h"
#include "dtlapi.h"
#include "dot3ad_api.h"
#include "platform_config.h"
#include "nim_config.h"
#include "avl_api.h"
#include "nim_outcalls.h"
#include "nimapi.h"

/*********************************************************************
* @purpose  Returns the descripion for port event
*
* @param    event     interface event
*
* @returns  description for the event

* @notes    none
*
* @end
*********************************************************************/
L7_char8 *nimGetIntfEvent(L7_PORT_EVENTS_t event)
  {
    switch (event)
    {
      case L7_PORT_DISABLE:    return "L7_PORT_DISABLE";
      case L7_PORT_ENABLE:    return "L7_PORT_ENABLE";
      case L7_PORT_INSERT:    return "L7_PORT_INSERT";
      case L7_DOWN:    return "L7_DOWN";
      case L7_UP:    return "L7_UP";
      case L7_INACTIVE:    return "L7_INACTIVE";
      case L7_ACTIVE:    return "L7_ACTIVE";    break;
      case L7_FORWARDING:    return "L7_FORWARDING";
      case L7_NOT_FORWARDING:    return "L7_NOT_FORWARDING";
      case L7_CREATE:    return "L7_CREATE";
      case L7_CREATE_COMPLETE:    return "L7_CREATE_COMPLETE";
      case L7_DELETE_PENDING:    return "L7_DELETE_PENDING";
      case L7_DELETE:    return "L7_DELETE";
      case L7_DELETE_COMPLETE:    return "L7_DELETE_COMPLETE";
      case L7_LAG_ACQUIRE:    return "L7_LAG_ACQUIRE";
      case L7_LAG_RELEASE:    return "L7_LAG_RELEASE";
      case L7_SPEED_CHANGE:    return "L7_SPEED_CHANGE";
      case L7_LAG_CFG_CREATE:    return "L7_LAG_CFG_CREATE";
      case L7_LAG_CFG_MEMBER_CHANGE:    return "L7_LAG_CFG_MEMBER_CHANGE";
      case L7_LAG_CFG_REMOVE:    return "L7_LAG_CFG_REMOVE";
      case L7_LAG_CFG_END:    return "L7_LAG_CFG_END";
      case L7_PROBE_SETUP:    return "L7_PROBE_SETUP";
      case L7_PROBE_TEARDOWN:    return "L7_PROBE_TEARDOWN";
      case L7_SET_INTF_SPEED:    return "L7_SET_INTF_SPEED";
      case L7_SET_MTU_SIZE:    return "L7_SET_MTU_SIZE";
      case L7_PORT_ROUTING_ENABLED:    return "L7_PORT_ROUTING_ENABLED";
      case L7_PORT_ROUTING_DISABLED:    return "L7_PORT_ROUTING_DISABLED";
      case L7_PORT_BRIDGING_ENABLED:    return "L7_PORT_BRIDGING_ENABLED";
      case L7_PORT_BRIDGING_DISABLED:    return "L7_PORT_BRIDGING_DISABLED";
      case L7_VRRP_TO_MASTER:    return "L7_VRRP_TO_MASTER";
      case L7_VRRP_FROM_MASTER:    return "L7_VRRP_FROM_MASTER";
      case L7_DOT1X_PORT_AUTHORIZED:    return "L7_DOT1X_PORT_AUTHORIZED";
      case L7_DOT1X_PORT_UNAUTHORIZED:    return "L7_DOT1X_PORT_UNAUTHORIZED";
      case L7_ATTACH:    return "L7_ATTACH";
      case L7_ATTACH_COMPLETE:    return "L7_ATTACH_COMPLETE";
      case L7_DETACH:    return "L7_DETACH";
      case L7_DETACH_COMPLETE:    return "L7_DETACH_COMPLETE";
      case L7_DOT1X_ACQUIRE: return "L7_DOT1X_ACQUIRE";
      case L7_DOT1X_RELEASE: return "L7_DOT1X_RELEASE";
      case L7_PORT_STATS_RESET: return "L7_PORT_STATS_RESET";

   default:
      return "Unknown Port Event";
    }
  }



/*********************************************************************
* @purpose  check if the port is a macro port
*
* @param    intIfNum    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL nimIsMacroPort(L7_uint32 intIfNum)
{
  L7_BOOL   returnVal = L7_FALSE;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    returnVal =  L7_FALSE;
  }
  else if (nimCtlBlk_g->nimPorts[intIfNum].operInfo.macroPort.macroPort == L7_NULL)
  {
    returnVal = L7_TRUE;
  }
  return(returnVal);
}


/*********************************************************************
* @purpose  Convert filter bitmasks to SNMP octet-strings
*
* @param    NIM_INTF_MASK_t  in  @b((input))  mask to be converted
* @param    NIM_INTF_MASK_t* out @b((output)) pointer to storage to hold the converted mask
*
* @returns  L7_SUCCESS
*
* @notes    Works both ways, basically converts lsb to msb
*
* @end
*********************************************************************/
L7_RC_t nimReverseMask(NIM_INTF_MASK_t in, NIM_INTF_MASK_t* out)
{
  L7_uint32 j,k;
  NIM_INTF_MASK_t temp;

  memset(&temp,0,sizeof(NIM_INTF_MASK_t));

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_FAILURE);

  for (k=0;k<NIM_INTF_INDICES;k++)  /* bytes 0 to max-1 */
  {
    for (j=1;j<=8;j++)  /* interfaces 1 to 8 within a byte */
    {
      if (NIM_INTF_ISMASKBITSET(in, 8*k + j)) /* kth byte, jth interface */
        NIM_INTF_SETMASKBIT( temp,8*k + (8-j+1) ); /* reverse msb-lsb */
      else
        NIM_INTF_CLRMASKBIT(temp, 8*k + (8-j+1) );
    }
  }

  memcpy(out, &temp, NIM_INTF_INDICES);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Convert a mask to a densely packed list of internal interface numbers
*
* @param    NIM_INTF_MASK_t*  mask     @b((input))  mask to be converted
* @param    L7_uint32            list[]   @b{(output)} densely packed list
* @param    L7_uint32*           numList  @b{(output)} number of interfaces in the above list
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimMaskToList(NIM_INTF_MASK_t *mask,
                      L7_uint32 list[],
                      L7_uint32 *numList)
{
  L7_uint32 i;
  L7_uint32 count = 0;
  NIM_INTF_MASK_t temp;

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_FAILURE);

  memcpy(&temp, mask, NIM_INTF_INDICES);
  memset(list, 0x00, L7_MAX_INTERFACE_COUNT*sizeof(L7_uint32));

  for (i=1;i<=L7_MAX_INTERFACE_COUNT;i++)
    if (NIM_INTF_ISMASKBITSET(temp,i))
    {
      list[count] = i;
      count++;
    }
  *numList = count;
  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Convert a densely packed list of internal interface numbers to a mask
*
* @param    L7_uint32            list[]   @b{(input)} densely packed list
* @param    L7_uint32            numList  @b{(input)} number of interfaces in the above list
* @param    NIM_INTF_MASK_t*  mask     @b((output))  mask to be generated
*
* @returns  L7_SUCCESS
*
* @notes    mask is application oriented i.e. lsb/rightmost-1 bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimListToMask(L7_uint32 list[],
                      L7_uint32 numList,
                      NIM_INTF_MASK_t *mask)
{
  NIM_INTF_MASK_t temp;
  L7_uint32 i;

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_FAILURE);

  memset((void *)&temp,0x00,sizeof(NIM_INTF_MASK_t));
  for (i=0;i<numList;i++)
  {
    NIM_INTF_SETMASKBIT(temp,list[i]);
  }
  memcpy((void *)mask,(void *)&temp,sizeof(NIM_INTF_MASK_t));
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns the ifIndex associated with the
*           internal interface number
*
* @param    intIfNum    @b{(input)}   internal interface number
* @param    ifIndex     @b{(output)}  pointer to ifIndex,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfIfIndex(L7_uint32 intIfNum, L7_uint32 *ifIndex)
{
  L7_RC_t   rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {

    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *ifIndex = nimCtlBlk_g->nimPorts[intIfNum].ifIndex;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Returns the internal interface number
*           associated with the **external** interface number, or ifIndex
*
* @param    extIfNum    @b{(input)}  external interface number
* @param    intIfNum    @b{(output)} pointer to Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfNumber(L7_uint32 extIfNum, L7_uint32 *intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = nimIfIndexIntIfNumGet(extIfNum,intIfNum);

  return(rc);
}

/*********************************************************************
* @purpose  Obtain the next **external** interface number, or ifIndex
*
* @param    extIfNum     @b{(input)}   external interface number
* @param    nextextIfNum @b{(output)}  pointer to next external interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetNextExtIfNumber(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32       intIfNum;
  L7_RC_t         rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    rc = nimIfIndexNextGet(extIfNum,nextExtIfNum,&intIfNum);
  }

  return(rc);
}

/*********************************************************************
* @purpose  Determine if this **external** interface number, or ifIndex
*           exists
*
* @param    extIfNum       @b{(input)} external interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimCheckExtIfNumber(L7_uint32 extIfNum)
{
  L7_uint32   intIfNum;
  L7_RC_t     rc  = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    rc = nimIfIndexIntIfNumGet(extIfNum,&intIfNum);
  }

  return(rc);
}


/*********************************************************************
* @purpose  Determine if this internal interface number is valid
*
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimCheckIfNumber(L7_uint32 intIfNum)
{
  L7_RC_t     rc = L7_ERROR;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_NOT_EXIST;

    NIM_LOG_MSG("nimCheckIfNumber: incorrect phase for operation\n");
  }
  else if (intIfNum <= platIntfTotalMaxCountGet())
  {
    NIM_CRIT_SEC_READ_ENTER();

    if (intIfNum < 1)
    {
        rc = L7_FAILURE;
    }
    else if (nimCtlBlk_g->nimPorts[intIfNum].present != L7_TRUE)
    {
        rc = L7_ERROR;
    }
    else
    {
        rc = L7_SUCCESS;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }
  else
  {
#if 0 //Removed this print
    NIM_LOG_MSG("nimCheckIfNumber: internal interface number %d out of range\n", (int)intIfNum);
#endif
    rc = L7_FAILURE;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Returns the Unit-Slot-Port
*           associated with the internal interface number
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    usp         @b{(output)} pointer to nimUSP_t structure,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetUnitSlotPort(L7_uint32 intIfNum, nimUSP_t *usp)
{
  L7_RC_t   rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_NOT_EXIST;
    NIM_LOG_MSG("nimGetUnitSlotPort: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    if ((intIfNum < 1) || (intIfNum > platIntfTotalMaxCountGet()))
    {
        rc = L7_FAILURE;
    }
    else if (nimCtlBlk_g->nimPorts[intIfNum].present != L7_TRUE)
    {
        rc = L7_ERROR;
    }
    else
    {
        rc = L7_SUCCESS;
    }

    if (rc == L7_SUCCESS)
    {
      memcpy (usp, &nimCtlBlk_g->nimPorts[intIfNum].usp, sizeof(nimUSP_t));
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Returns the internal interface number
*           associated with the Unit-Slot-Port
*
* @param    usp         @b{(input)}  pointer to nimUSP_t structure
* @param    intIfNum    @b{(output)} pointer to internal interface number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntIfNumFromUSP(nimUSP_t* usp, L7_uint32 *intIfNum)
{

  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   unit = 0, slot = 0, port = 0;
  L7_INTF_TYPES_t sysIntfType;
  nimUspIntIfNumTreeData_t *pData;

  /* check the usp */
  if (usp != L7_NULL)
  {
    unit = usp->unit;
    slot = usp->slot;
    port = usp->port;
  }
  else
  {
    NIM_LOG_MSG("NIM: usp is NULL\n");
    rc = L7_FAILURE;
  }

  if ((rc != L7_SUCCESS) || (nimPhaseStatusCheck() != L7_TRUE))
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((unit > nimCtlBlk_g->maxNumOfUnits) ||
           (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit) ||
           (port == 0))
  {
    NIM_TRACE_MSG("NIM: out of bounds usp used U=%d, S=%d P=%d\n",unit,slot,port);
    rc = L7_FAILURE;
  }
  else if ( (nimGetIntfTypeFromUSP(usp, &sysIntfType) != L7_SUCCESS) ||
            ( port > nimMaxIntfForIntfTypeGet(sysIntfType) ))
  {
    /* The port number should not exceed the maximum number of interfaces for the type */
    NIM_TRACE_MSG("NIM: out of bounds usp used U=%d, S=%d P=%d - port too large\n",unit,slot,port);
    rc = L7_FAILURE;
  }

  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    pData = avlSearchLVL7 (&nimCtlBlk_g->nimUspTreeData, usp, AVL_EXACT);

    if (pData != L7_NULLPTR)
    {
      *intIfNum = pData->intIfNum;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_ERROR;
    }

    NIM_CRIT_SEC_READ_EXIT();

  }

  return(rc);
}


/*********************************************************************
* @purpose  Given a usp, get the interface type associated with the slot
*
* @param    usp         @b{(input)}  pointer to nimUSP_t structure
* @param    sysIntfType @b{(output)} pointer to a parm of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfTypeFromUSP(nimUSP_t* usp, L7_INTF_TYPES_t *sysIntfType)
{
/* WPJ_TBD:  Make this more extensible */

  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   unit = 0, slot = 0, port = 0;

  /* check the usp */
  if (usp != L7_NULL)
  {
    unit = usp->unit;
    slot = usp->slot;
    port = usp->port;
  }
  else
  {
    NIM_LOG_MSG("NIM: usp is NULL\n");
    rc = L7_FAILURE;
  }

  if ((rc != L7_SUCCESS) || (nimPhaseStatusCheck() != L7_TRUE))
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((unit > nimCtlBlk_g->maxNumOfUnits) ||
           (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit) ||
           (port == 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: out of bounds usp used U=%d, S=%d P=%d\n",unit,slot,port);
    rc = L7_FAILURE;
  }

  if (slot == platSlotVlanSlotNumGet() )
    *sysIntfType = L7_LOGICAL_VLAN_INTF;
  else if (slot == platSlotCpuSlotNumGet() )
    *sysIntfType = L7_CPU_INTF;
  else if (slot == platSlotLagSlotNumGet() )
    *sysIntfType = L7_LAG_INTF;
  else if (slot == platSlotLoopbackSlotNumGet() )
    *sysIntfType = L7_LOOPBACK_INTF;
  else if (slot == platSlotTunnelSlotNumGet() )
    *sysIntfType = L7_TUNNEL_INTF;
  else if (slot == platSlotwirelessNetSlotNumGet() )
    *sysIntfType = L7_WIRELESS_INTF;
  else if (slot == platSlotL2TunnelSlotNumGet() )
    *sysIntfType = L7_CAPWAP_TUNNEL_INTF;
  else if (slot == platSlotVlanPortSlotNumGet() )     /* PTin added: virtual ports */
    *sysIntfType = L7_VLAN_PORT_INTF;
  else
    *sysIntfType = L7_PHYSICAL_INTF;  /* WPJ_TBD:  Assume physical until more automatic mapping is done */
  return rc;
}


/*********************************************************************
* @purpose  Returns the internal interface number of the LAG of which
*           this interface is a member.  If it is not a member, returns
*           this interface number
*
* @param    intIntfNum  @b{(input)}  Internal Interface Number
* @param    lagIntfNum  @b{(output)} pointer to Lag Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimGetLagIntfNum(L7_uint32 intIfNum, L7_uint32 *lagIntfNum)
{
  L7_RC_t   rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    NIM_CRIT_SEC_READ_EXIT();

    if (rc != L7_SUCCESS)
    {
      rc = L7_ERROR;
    }
    else if ( (rc = dot3adWhoisOwnerLag(intIfNum, lagIntfNum)) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Return Internal Interface Number of next valid port
*
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    nextintIfNum @b{(output)} Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimNextValidIntfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 maxIntf;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {

    NIM_CRIT_SEC_READ_ENTER();
    maxIntf = platIntfTotalMaxCountGet();
    /* next avaliable Physical internal interface number */
    for (intIfNum++; intIfNum <= maxIntf; intIfNum++)
    {
      if ((nimCtlBlk_g->nimPorts[intIfNum].present == L7_TRUE))
      {
        break;
      }
    }

    if (intIfNum <= maxIntf)
    {
      *nextIntIfNum = intIfNum;
    }
    else
    {
      rc = L7_FAILURE;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid port
*
* @param    firstIntIfNum    @b{(output)} first valid internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimFirstValidIntfNumber(L7_uint32 *firstIntIfNum)
{
  L7_uint32 intIfNum;
  L7_uint32 maxIntf;
  L7_RC_t rc = L7_SUCCESS;

  maxIntf = platIntfTotalMaxCountGet();

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    /* first avaliable Physical internal interface number */
    for (intIfNum = 1; intIfNum <= maxIntf ; intIfNum++)
    {
      if (nimCtlBlk_g->nimPorts[intIfNum].present == L7_TRUE)
      {
        break;
      }
    }

    if (intIfNum <= maxIntf)
    {
      *firstIntIfNum = intIfNum;
    }
    else
    {
      rc = L7_ERROR;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Return Internal Interface Number of next valid interface for
*           the specified system interface type.
*
* @param    sysIntfType  @b{(input)}  The type of interface requested
* @param    intIfNum     @b{(input)}  The present Internal Interface Number
* @param    nextintIfNum @b{(output)} The Next Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimNextValidIntfNumberByType(L7_INTF_TYPES_t sysIntfType, L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  L7_uint32 maxIntf;
  L7_RC_t rc = L7_SUCCESS;


  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    /* do not get the same intf number */
    /* look to the next one first */
    intIfNum++;

    maxIntf = platIntfTotalMaxCountGet();

    for (; intIfNum <= maxIntf; intIfNum++)
    {
      if ((nimCtlBlk_g->nimPorts[intIfNum].present == L7_TRUE) &&
          (nimCtlBlk_g->nimPorts[intIfNum].sysIntfType == sysIntfType))
      {
        break;
      }
    }

    if (intIfNum <= maxIntf)
    {
      *nextIntIfNum = intIfNum;
    }
    else
    {
      rc = L7_FAILURE;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface for
*           the specified interface type.
*
* @param    sysIntfType  @b{(input)}  The type of interface requested
* @param    intIfNum     @b{(output)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimFirstValidIntfNumberByType(L7_INTF_TYPES_t sysIntfType, L7_uint32 *firstIntIfNum)
{
  L7_uint32 maxIntf;
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;


  maxIntf = platIntfTotalMaxCountGet();

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc =  L7_ERROR;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    /* first avaliable internal interface number for the specified type */
    for (intIfNum = 1; intIfNum <= maxIntf; intIfNum++)
    {
      if ((nimCtlBlk_g->nimPorts[intIfNum].present == L7_TRUE) &&
          (nimCtlBlk_g->nimPorts[intIfNum].sysIntfType == sysIntfType))
      {
        break;
      }
    } /* end for */

    if (intIfNum <= maxIntf)
    {
      *firstIntIfNum = intIfNum;
    }
    else
    {
      rc =  L7_ERROR;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Returns the max interface number
*
* @param    *maxintf  @b{(output)} The max interface number
*
* @returns  L7_SUCCESS
*
* @notes    The number of network interfaces (regardless of
*           their current state) present on this system
*
* @end
*********************************************************************/
L7_RC_t nimIfNumberGet(L7_uint32 *maxintf)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 tmp;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    /* may need to revisit this calculation, it mirrors the old style
       of using the #defines */
    tmp = platIntfTotalMaxCountGet() - 2;
    *maxintf = tmp;
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Return the first interface in the unit
*
* @param    unit    @b{(input)}  The unit to be operated on
* @param    *intIfNum   @b{(output)} The returned internal interface number
* @param    *usp    @b{(output)} The returned USP
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimFirstIntfOnUnitGet(L7_uchar8 unit, L7_uint32 *intIfNum, nimUSP_t *usp)
{
  L7_RC_t   rc = L7_SUCCESS;
  nimUSP_t tempUsp;
  nimUspIntIfNumTreeData_t *pData;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((unit < 1) || (unit > nimCtlBlk_g->maxNumOfUnits))
  {
    rc = L7_FAILURE;
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    tempUsp.unit = unit;
    tempUsp.slot = 0;
    tempUsp.port = 0;


    pData = avlSearchLVL7 (&nimCtlBlk_g->nimUspTreeData, &tempUsp, AVL_NEXT);

    if (pData != L7_NULL)
    {
      if (unit == pData->usp.unit)
      {
        *usp      = pData->usp;
        *intIfNum =  pData->intIfNum;
        rc = L7_SUCCESS;
      }
      else
        rc = L7_ERROR;
    }
    else
    {
      rc = L7_ERROR;
    }

    NIM_CRIT_SEC_READ_EXIT();

  }

  return(rc);
}


/*********************************************************************
*
* @purpose  Return the next interface in the unit
*
* @param    *usp  @b{(inout)}  The unit/slot/port being operated on
* @param    *intIfNum @b{(output)} The next internal interface number found
*
* @returns  L7_SUCCESS
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimNextIntfOnUnitGet(nimUSP_t *usp,L7_uint32 *intIfNum)
{
  L7_uchar8   unit = 0,slot = 0;
  L7_ushort16 port = 0;
  L7_RC_t rc  = L7_SUCCESS;
  L7_INTF_TYPES_t sysIntfType;
  nimUspIntIfNumTreeData_t *pData;

  if (usp != L7_NULL)
  {
    unit = usp->unit;
    slot = usp->slot;
    port = usp->port;
  }
  else
  {
    NIM_LOG_MSG("NIM: usp is NULL\n");
    rc = L7_FAILURE;
  }

  if ((rc != L7_SUCCESS) || (nimPhaseStatusCheck() != L7_TRUE))
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((unit > nimCtlBlk_g->maxNumOfUnits) ||
           (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit) ||
           (port == 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: out of bounds usp used U=%d, S=%d P=%d\n",unit,slot,port);
    rc = L7_FAILURE;
  }
  else if ( (nimGetIntfTypeFromUSP(usp, &sysIntfType) != L7_SUCCESS) ||
            ( port > nimMaxIntfForIntfTypeGet(sysIntfType) ))
  {
    /* The port number should not exceed the maximum number of interfaces for the type */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: out of bounds usp used U=%d, S=%d P=%d - port too large\n",unit,slot,port);
    rc = L7_FAILURE;
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    pData = avlSearchLVL7 (&nimCtlBlk_g->nimUspTreeData, usp, AVL_NEXT);

    if (pData != L7_NULLPTR)
    {
      if (usp->unit == pData->usp.unit)
      {
        *usp = pData->usp;
        *intIfNum = pData->intIfNum;
        rc = L7_SUCCESS;
      }
      else
        rc = L7_ERROR;
    }
    else
      rc = L7_ERROR;

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Return the first interface in the slot
*
* @param    unit    @b{(input)}  The unit to be operated on
* @param    slot    @b{(input)}  The slot to be operated on
* @param    intIfNum  @b{(output)} The returned internal interface number
* @param    *usp    @b{(output)} The returned USP
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimFirstIntfOnSlotGet(L7_uchar8 unit,L7_uchar8 slot, L7_uint32 *intIfNum , nimUSP_t *usp)
{
  L7_RC_t rc  = L7_SUCCESS;
  nimUSP_t  tempUsp;
  nimUspIntIfNumTreeData_t *pData;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((unit > nimCtlBlk_g->maxNumOfUnits) ||
           (unit < 1) || (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit))
  {
    rc = L7_FAILURE;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: unit/slot out of bounds\n");
  }
  else if (usp == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: NULL usp ptr\n");
  }
  else
  {

    tempUsp.unit = unit;
    tempUsp.slot = slot;
    tempUsp.port = 0;

    NIM_CRIT_SEC_READ_ENTER();

    pData = avlSearchLVL7 (&nimCtlBlk_g->nimUspTreeData, &tempUsp, AVL_NEXT);



    if (pData != L7_NULLPTR)
    {
      if ((unit == pData->usp.unit) && (slot == pData->usp.slot))
      {
        *usp = pData->usp;
        *intIfNum = pData->intIfNum;
        rc = L7_SUCCESS;
      }
      else
        rc = L7_ERROR;
    }
    else
      rc = L7_ERROR;

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Return the next interface in the slot
*
* @param    *usp  @b{(inout)}     The unit/slot/port being operated on
* @param    *intIfNum   @b{(output)}   Internal interface number returned
*
* @returns  L7_SUCCESS
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimNextIntfOnSlotGet(nimUSP_t *usp,L7_uint32 *intIfNum )
{
  L7_uchar8   unit = 0,slot = 0;
  L7_ushort16 port = 0;
  L7_RC_t rc  = L7_SUCCESS;
  L7_INTF_TYPES_t sysIntfType;
  nimUspIntIfNumTreeData_t *pData;

  if (usp != L7_NULL)
  {
    unit = usp->unit;
    slot = usp->slot;
    port = usp->port;
  }
  else
  {
    NIM_LOG_MSG("NIM: usp is NULL\n");
    rc = L7_FAILURE;
  }

  if ((rc != L7_SUCCESS) || (nimPhaseStatusCheck() != L7_TRUE))
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((unit > nimCtlBlk_g->maxNumOfUnits) ||
           (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit) ||
           (port > nimCtlBlk_g->maxNumOfPhysicalPortsPerSlot) ||
           (port == 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: out of bounds usp used U=%d, S=%d P=%d\n",unit,slot,port);
    rc = L7_FAILURE;
  }
  else if ( (nimGetIntfTypeFromUSP(usp, &sysIntfType) != L7_SUCCESS) ||
            ( port > nimMaxIntfForIntfTypeGet(sysIntfType) ))
  {
    /* The port number should not exceed the maximum number of interfaces for the type */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: out of bounds usp used U=%d, S=%d P=%d - port too large\n",unit,slot,port);
    rc = L7_FAILURE;
  }

  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    pData = avlSearchLVL7 (&nimCtlBlk_g->nimUspTreeData, usp, AVL_NEXT);


    if (pData != L7_NULLPTR)
    {
      if ((usp->slot == pData->usp.slot) && (usp->unit == pData->usp.unit))
      {
        *usp = pData->usp;
        *intIfNum = pData->intIfNum;
        rc = L7_SUCCESS;
      }
      else
        rc = L7_ERROR;
    }
    else
      rc = L7_ERROR;

    NIM_CRIT_SEC_READ_EXIT();
  }


  return(rc);
}

/*********************************************************************
*
* @purpose  Get the configuration ID for the given Internal Interface ID
*
* @param    internalIntfId  @b{(input)}  Internal Interface ID
* @param    configId        @b{(output)} Pointer to the new config ID
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimConfigIdGet(L7_uint32 internalIntfId,nimConfigID_t *configId)
{
  L7_RC_t rc = L7_FAILURE;

  NIM_CRIT_SEC_READ_ENTER();

  do
  {

    if ((nimCtlBlk_g == L7_NULLPTR) || (nimCtlBlk_g->nimPorts == L7_NULLPTR))
    {
      rc = L7_FAILURE;
      break;
    }
    else if ((internalIntfId < 1) || (internalIntfId > platIntfTotalMaxCountGet()))
    {
      rc = L7_FAILURE;
      break;
    }
    else
    {
      IS_INTIFNUM_PRESENT(internalIntfId,rc);

      if (rc == L7_SUCCESS)
      {
        NIM_CONFIG_ID_COPY(configId ,&nimCtlBlk_g->nimPorts[internalIntfId].configInterfaceId);
        rc = L7_SUCCESS;
      }
    }

  } while ( 0 );
  NIM_CRIT_SEC_READ_EXIT();

  return(rc);
}

/*********************************************************************
*
* @purpose  Get the USP for the given Configuration ID
*
* @param    configId        @b{(input)}  Configuration Interface ID
* @param    usp             @b{(output)} Pointer to the USP
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimUspFromConfigIDGet(nimConfigID_t *configId,nimUSP_t *usp)
{
  L7_RC_t rc = L7_SUCCESS;
  nimUSP_t  tmpUSP;
  L7_uint32 intIfNum;
  L7_uint32 port;

  switch (configId->type)
  {
  case L7_LAG_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotLagSlotNumGet();
    tmpUSP.port = configId->configSpecifier.dot3adIntf;
    break;
  case L7_LOGICAL_VLAN_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotVlanSlotNumGet();
    if (nimPortInstanceNumGet(*configId, &port) == L7_SUCCESS)
        tmpUSP.port = port;
    else
        tmpUSP.port = platIntfMaxCountGet() + 1;  /* Set to an invalid value */
    break;
  case L7_LOOPBACK_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotLoopbackSlotNumGet();
    tmpUSP.port = configId->configSpecifier.loopbackId + 1;
    break;
  case L7_TUNNEL_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotTunnelSlotNumGet();
    tmpUSP.port = configId->configSpecifier.tunnelId + 1;
    break;
  case L7_WIRELESS_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotwirelessNetSlotNumGet();
    tmpUSP.port = configId->configSpecifier.wirelessNetId;
    break;
  case L7_CAPWAP_TUNNEL_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotL2TunnelSlotNumGet();
    tmpUSP.port = configId->configSpecifier.l2tunnelId;
    break;
  /* PTin added: virtual ports */
  case L7_VLAN_PORT_INTF:
    tmpUSP.unit = L7_LOGICAL_UNIT;
    tmpUSP.slot = (L7_uchar8)platSlotVlanPortSlotNumGet();
    tmpUSP.port = configId->configSpecifier.vlanportId;
    break;
  case L7_PHYSICAL_INTF:
  case L7_CPU_INTF:
    tmpUSP.unit = configId->configSpecifier.usp.unit;
    tmpUSP.slot = configId->configSpecifier.usp.slot;
    tmpUSP.port = configId->configSpecifier.usp.port;
    break;
  default:
    return L7_FAILURE;
  }

  if ((rc = nimGetIntIfNumFromUSP(&tmpUSP,&intIfNum)) == L7_SUCCESS)
  {
    NIM_CRIT_SEC_READ_ENTER();

    /* make sure the config IDs are equal */
    if (NIM_CONFIG_ID_IS_EQUAL(configId,&nimCtlBlk_g->nimPorts[intIfNum].configInterfaceId))
    {
      memcpy(usp,&nimCtlBlk_g->nimPorts[intIfNum].usp,sizeof(nimUSP_t));
      rc = L7_SUCCESS;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Get the USP for the given Configuration ID
*
* @param    configId        @b{(input)}  Configuration Interface ID
* @param    intIfNum        @b{(output)} Pointer to the intIfNum
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimIntIfFromConfigIDGet(nimConfigID_t *configId,L7_uint32 *intIfNum)
{
    nimConfigIdTreeData_t  configIdInfo;

    /*----------------------------------------------------------------------*/
    /* Determine if an intIfNum was assigned from a previous boot and reuse */
    /*----------------------------------------------------------------------*/

    if (nimConfigIdTreeEntryGet(configId, &configIdInfo) == L7_SUCCESS)
    {
        *intIfNum = configIdInfo.intIfNum;
        return L7_SUCCESS;
    }

    return L7_FAILURE;

}

/*********************************************************************
*
* @purpose  A deep copy for the Configuration ID
*
* @param    src             @b{(input)}  Pointer to the source configID
* @param    dst             @b{(output)} Pointer to the destination configID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t nimConfigIdCopy(nimConfigID_t *src,nimConfigID_t *dst)
{
  L7_RC_t rc = L7_SUCCESS;

  if ((src == L7_NULLPTR) || (dst == L7_NULLPTR))
  {
    rc = L7_FAILURE;
  }
  else
  {
    memcpy(dst,src,sizeof(nimConfigID_t));
  }

  return(rc);
}
/*********************************************************************
*
* @purpose  Get the Configuration Mask Offset
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    maskOffset    @b{(output)} The config mask offset within a mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      If the internal interface is not valid
* @returns  L7_FAILURE    If there was no config for the interface in the past
*
* @notes    The config files using a config mask must be in sync with NIM
*           or the mask offset will be invalid
*
* @end
*********************************************************************/
L7_RC_t nimConfigIdMaskOffsetGet(L7_uint32 intIfNum,L7_uint32 *maskOffset)
{
  L7_RC_t rc = L7_SUCCESS;

  if ((rc = nimCheckIfNumber(intIfNum)) == L7_SUCCESS)
  {
    NIM_CRIT_SEC_READ_ENTER();

    *maskOffset = nimCtlBlk_g->nimPorts[intIfNum].configPort.configIdMaskOffset;

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}


/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all created interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimIntfMaskOfCreatedGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->createdMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();

    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all present interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimIntfMaskOfPresentGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->presentMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();
    rc = L7_SUCCESS;
  }


  return(rc);
}

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all CPU interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimCpuIntfMaskGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->cpuIntfMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all Physical interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimPhysicalIntfMaskGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->physicalIntfMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();
    rc = L7_SUCCESS;
  }

  return(rc);

}

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all lag interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimLagIntfMaskGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->lagIntfMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();
    rc = L7_SUCCESS;
  }

  return(rc);

}

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all vlan interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimVlanIntfMaskGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->vlanIntfMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();
    rc = L7_SUCCESS;
  }

  return(rc);

}

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all l2 tunnel interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
L7_RC_t nimL2TnnlIntfMaskGet(NIM_INTF_MASK_t *mask)
{
  L7_RC_t rc = L7_FAILURE;

  if (mask != L7_NULL)
  {
    NIM_CRIT_SEC_READ_ENTER();

    memcpy(mask,&nimCtlBlk_g->l2tnnlIntfMask,sizeof(NIM_INTF_MASK_t));

    NIM_CRIT_SEC_READ_EXIT();
    rc = L7_SUCCESS;
  }

  return(rc);

}

/*********************************************************************
* @purpose  Get valid range of valid internal inteface numbers
*           for a given interface type
*
* @param    intfType  @b{(input)}  one of L7_INTF_TYPES_t
* @param    *min      @b{(output)} pointer to parm to store min value
* @param    *max      @b{(output)} pointer to parm to store max value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero
*
* @end
*********************************************************************/
L7_RC_t nimIntIfNumRangeGet(L7_INTF_TYPES_t intfType, L7_uint32 *min, L7_uint32 *max)
{

  *min = 0;
  *max = 0;

  if (intfType >= L7_MAX_INTF_TYPE_VALUE)
      return L7_FAILURE;

  *min = nimCtlBlk_g->intfTypeData[intfType].minIntIfNumber;
  *max = nimCtlBlk_g->intfTypeData[intfType].maxIntIfNumber;

  return L7_SUCCESS;
}
