/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename   dtl_l3_rtr_intf.c
 *
 * @purpose    Dtl Common Routing Interface functions
 *
 * @component  Device Transformation Layer
 *
 * @comments   Provides L3 Protocol Neutral interface management
 *
 * @create     04/05/2005
 *
 * @author     elund
 * @end
 *
 **********************************************************************/

#include "l3_comm_structs.h"
#include "l3_defaultconfig.h"
#include "dtlinclude.h"
#include "dtl_l3_api.h"
#include "dtl_ip.h"
#include "l7_ip_api.h"



/*********************************************************************
* @purpose  Get the vlan corresponding to this interface number
*
* @param    intIfNum        Internal interface number of this vlan
* @param    vlanid              pointer to vlan id output location
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
static
L7_RC_t dtlRtrIntIfNumToVlanId(L7_uint32 intIfNum, L7_ushort16 *vlanId)
{
    nimMacroPort_t    macroPortIntf;
    L7_uint32 sysIntfType, vid;

    if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
    {
        if (sysIntfType == L7_LOGICAL_VLAN_INTF)
        {
            if ( nimGetMacroPortAssignment(intIfNum, &macroPortIntf) == L7_SUCCESS)
            {
                vid    = (L7_uint32)macroPortIntf.macroInfo;
                *vlanId = (L7_ushort16)vid;
                return L7_SUCCESS;
            }
        }
        else
        {
            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Set up parameters of dapi command to enable
*           or disable a routing interface.
*           The routing interface may be enabled for IPv4 or IPv6 or both.
*
* @param    pIpCircuit @b{(input)} Pointer to a dtlRtrIntf_t structure
* @param    pDapiCmd   @b{(input)} Pointer to a DAPI_ROUTING_INTF_MGMT_CMD_t
*                                  structure
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlRoutingIntfCmdBuild(dtlRtrIntf_t *ipCircuit,
                               DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd)
{
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uint32 encapType;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(ipCircuit->intIfNum, L7_SYSNAME, ifName);

  DTL_IP_TRACE("%s : intf = %u, %s, vlanID = %d \n",
                 __FUNCTION__,
                 ipCircuit->intIfNum, ifName, ipCircuit->vlanId);

  if (nimGetUnitSlotPort(ipCircuit->intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  /*---------------------------------------------*/
  /*  Build the command structure                */
  /*---------------------------------------------*/
  memset(dapiCmd, 0, sizeof(DAPI_ROUTING_INTF_MGMT_CMD_t));
  dapiCmd->cmdData.rtrIntfAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd->cmdData.rtrIntfAdd.broadcastCapable = L7_TRUE;
  dapiCmd->cmdData.rtrIntfAdd.vlanID = ipCircuit->vlanId;

  /* Just set MTU to default value here. If a different value has been
   * configured, ipMapIntfEnable() will apply it later. */
  dapiCmd->cmdData.rtrIntfAdd.mtu = FD_RTR_DEFAULT_MTU;

  nimEncapsulationTypeGet(ipCircuit->intIfNum, &encapType);
  if (encapType == L7_ENCAP_ETHERNET)
    dapiCmd->cmdData.rtrIntfAdd.encapsType = DAPI_ROUTING_INTF_ENCAPS_TYPE_ETHERNET;
  else
    dapiCmd->cmdData.rtrIntfAdd.encapsType = DAPI_ROUTING_INTF_ENCAPS_TYPE_SNAP;

  /* Get MAC address of routing interface */
  nimGetIntfL3MacAddress(ipCircuit->intIfNum, 0,
                    (L7_uchar8 *)&(dapiCmd->cmdData.rtrIntfAdd.macAddr));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables or disables the routing capability for this
* @purpose  particular router interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    vlanId   @b{(input)} internal VLAN ID assigned to port based routing interface
* @param    mode     @b{(input)} L7_ENABLE to enable routing on this interface
*                                L7_DISABLE to disable routing on this interface
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRtrIntfModeSet(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  dtlRtrIntf_t ipCircuit;
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  DTL_IP_TRACE("%s : intf %u, %s, mode =  %s,  \n",
                 __FUNCTION__, intIfNum, ifName,
                 mode ? "enable" : "disable");

  ipCircuit.intIfNum    = intIfNum;
  ipCircuit.vlanId = vlanId;

  /* If a VLAN routing interface, override value passed in with value from NIM. */
  if(dtlRtrIntIfNumToVlanId(intIfNum, &ipCircuit.vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (nimGetUnitSlotPort(ipCircuit.intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  if (dtlRoutingIntfCmdBuild(&ipCircuit, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  if (mode == L7_ENABLE)
  {
     rc = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_ADD, &dapiCmd);
  }
  else
  {
    rc = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_DELETE, &dapiCmd);
  }

  return rc;
}

/*********************************************************************
 * @purpose  Create an L3 interface for use
 *
 * @param    intIfNum @b{(input)} The internal interface number for this
 *                                router interface
 *
 * @returns  L7_SUCCESS  on a successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    The new L3 interface has no L3 attributes yet, these will
 *           be added by functions belonging to the appropriate address
 *           families.
 *
 * @end
 *********************************************************************/
L7_RC_t
dtlRtrIntfCreate(L7_uint32 intIfNum)
{

  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
 * @purpose  Destroy an L3 interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for this
 *                                router interface
 *
 * @returns  L7_SUCCESS  on a successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t
dtlRtrIntfDelete(L7_uint32 intIfNum)
{

  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
 * @purpose  Destroy an L3 interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for this
 *                                router interface
 *
 * @returns  L7_SUCCESS  on a successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t
dtlIntfMacAddrSet(L7_uint32 intIfNum, L7_uchar8 *macAddr)
{

  L7_RC_t rc = L7_SUCCESS;

  return rc;
}
